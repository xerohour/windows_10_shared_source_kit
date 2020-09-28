////////////////////////////////////////////////////////////////
// RawStructured.h
////////////////////////////////////////////////////////////////

#pragma once

#include "D3D11Test.h"
#include "TiledResources.h"

#include <array>

using namespace WindowsTest;

enum BUFFER_RESIDENCY
{
    BUFFER_RESIDENCY_FULL,
    BUFFER_RESIDENCY_FRAGMENTED
};

DECLARE_NAMED_VALUES(BUFFER_RESIDENCY);


// Base class for all Raw Structred tests
template <bool isStructured, TILED_RESOURCE_BASE resourceBase>
class CRawStructuredTestBase : public CResourceBaseTestHelper<resourceBase>
{
protected:
    // Helpers to clean up typenames
    CRESOURCE_BASE_TEST_HELPER__TYPENAME_TYPEDEFS

    struct
    {
        CVariationParameter<BUFFER_RESIDENCY> *pResidencyPattern;
    } m_parameters;

    UINT GetResourceMiscFlags()
    {
        if (isStructured)
        {
            return D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        }
        else
        {
            return D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
        }
    }

    void InitTestParameters();

    TEST_RESULT ExecuteTestCase();
    TEST_RESULT CreateBuffers(_Inout_ ID3D11Buffer **ppTiledBuffer, _Inout_ ID3D11Buffer **ppNormalBuffer);
    TEST_RESULT CreateShaders(_Inout_ ID3D11ComputeShader **ppTiledShader, _Inout_ ID3D11ComputeShader **ppNormalShader);
    TEST_RESULT CreateViews(_In_ ID3D11Buffer *pTiledBuffer, _In_ ID3D11Buffer *pNormalBuffer, _Inout_ ID3D11ShaderResourceView **ppTiledView, _Inout_ ID3D11ShaderResourceView **ppNormalView);
    TEST_RESULT TestTexture3DClear();
};




// Actual Tests
// NOTE: Testing raw buffers with Texture3D may not add any benefit over Texture2D
//       Leaving these here (as well as set up below) in case this benefit decision changes
class CRawTestTex2D : public CRawStructuredTestBase < false, TRB_TEXTURE_2D > { };
class CRawTestTex3D : public CRawStructuredTestBase < false, TRB_TEXTURE_3D > { };

class CStructuredTestTex2D : public CRawStructuredTestBase < true, TRB_TEXTURE_2D > { };
class CStructuredTestTex3D : public CRawStructuredTestBase < true, TRB_TEXTURE_3D > { };




// Implementations
template <bool isStructured, TILED_RESOURCE_BASE resourceBase>
void CRawStructuredTestBase<isStructured, resourceBase>::InitTestParameters()
{
    CreateParameter(_T("ResidencyPattern"), &m_parameters.pResidencyPattern);

    CUserValueSet<BUFFER_RESIDENCY> *pResidencyPatternValues = new CUserValueSet<BUFFER_RESIDENCY>();
    pResidencyPatternValues->AddValue(BUFFER_RESIDENCY_FULL);
    pResidencyPatternValues->AddValue(BUFFER_RESIDENCY_FRAGMENTED);

    testfactor::RFactor residencyPattern = AddParameterValueSet<BUFFER_RESIDENCY>(
        m_parameters.pResidencyPattern,
        pResidencyPatternValues
        );

    // When using Texture2D, skip building factors for Tier 3 (Tier 2 tests correct functionality)
    // When using Texture3D, skip building factors for Tiers 1 and 2
    testfactor::RFactor tiledResourcesTier = InitTiledResourcesTierParam((TRB_TEXTURE_3D == resourceBase));

    SetRootTestFactor(tiledResourcesTier * residencyPattern);
}

template <bool isStructured, TILED_RESOURCE_BASE resourceBase>
TEST_RESULT CRawStructuredTestBase<isStructured, resourceBase>::ExecuteTestCase()
{
    // Temporary test to check whether ClearView works on Texture3D resources
    //return TestTexture3DClear();

    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;
    ID3D11Device2 *pDevice = GetDevice2();
    ID3D11DeviceContext2 *pContext = GetImmediateContext2();
    CHandle<ID3D11TextureType> pObservedTexture;
    CHandle<ID3D11TextureType> pExpectedTexture;
    CHandle<ID3D11Buffer> pTiledBuffer;
    CHandle<ID3D11Buffer> pNormalBuffer;
    CHandle<ID3D11ComputeShader> pTiledShader;
    CHandle<ID3D11ComputeShader> pNormalShader;
    CHandle<ID3D11ShaderResourceView> pTiledBufferView;
    CHandle<ID3D11ShaderResourceView> pNormalBufferView;
    CHandle<ID3D11UnorderedAccessView> pObservedView;
    CHandle<ID3D11UnorderedAccessView> pExpectedView;
    CTiledResourceHelper::CTilePoolManager *pTilePoolManager = GetFramework()->GetTilePoolManager();
    TileMappingType tileMapping;

    // Compute such that 16 tiles fit per depth slice
    const UINT width  = 512;
    const UINT height = 512;
    const UINT depth  = 4; // depth ignored for 2D

    // Create output textures
    CD3D11_TEXTURE_TYPE_DESC desc;
    CResourceBaseTestHelper<resourceBase>::GenerateTextureDesc(
        desc,
        DXGI_FORMAT_R8G8B8A8_TYPELESS,
        width,
        height,
        depth,
        0,
        D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_RENDER_TARGET
        );

    tr = CTiledResourcesTest::CreateTexture(&desc, &pObservedTexture);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    // Create a similar control texture for comparing rendered results to
    tr = CTiledResourcesTest::CreateTexture(&desc, &pExpectedTexture);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    //
    // Create the buffers
    //
    tr = CreateBuffers(&pTiledBuffer, &pNormalBuffer);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    //
    // Initialize buffer data
    //

    //
    // Create the shaders
    //
    tr = CreateShaders(&pTiledShader, &pNormalShader);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    //
    // Create the views
    //
    tr = CreateViews(pTiledBuffer, pNormalBuffer, &pTiledBufferView, &pNormalBufferView);
    if (RESULT_PASS != tr)
    {
        return tr;
    }


    // UAV's
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    uavDesc.Format = DXGI_FORMAT_R32_UINT;
    if (TRB_TEXTURE_2D == resourceBase)
    {
        uavDesc.Texture2D.MipSlice = 0;
        uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
    }
    else
    {
        uavDesc.Texture3D.MipSlice = 0;
        uavDesc.Texture3D.FirstWSlice = 0;
        uavDesc.Texture3D.WSize = -1;
        uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
    }
    hr = pDevice->CreateUnorderedAccessView(pExpectedTexture, &uavDesc, &pExpectedView);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateUnorderedAccessView);
        return RESULT_FAIL;
    }

    hr = pDevice->CreateUnorderedAccessView(pObservedTexture, &uavDesc, &pObservedView);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateUnorderedAccessView);
        return RESULT_FAIL;
    }

    //
    // Configure the compute pipeline
    //
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
    CResourceBaseTestHelper<resourceBase>::SetTileMappingFromStrings(tileMapping, fullyMappedRows);

    //
    // Set up correct dispatch size based on Texture2D or Texture3D
    //
    //const UINT dispatchNumX = (TRB_TEXTURE_2D == resourceBase ? 64 : 16);
    //const UINT dispatchNumY = (TRB_TEXTURE_2D == resourceBase ? 64 : 16);
    //const UINT dispatchNumZ = (TRB_TEXTURE_2D == resourceBase ? 1 : 8);
    const UINT dispatchNumX = 64;
    const UINT dispatchNumY = 64;
    const UINT dispatchNumZ = (TRB_TEXTURE_2D == resourceBase ? 1 : 4);

    //
    // Draw control image
    //
    pContext->CSSetShaderResources(0, 1, &pNormalBufferView);
    pContext->CSSetUnorderedAccessViews(1, 1, &pExpectedView, nullptr);
    pContext->CSSetShader(pNormalShader, nullptr, 0);
    pContext->Dispatch(dispatchNumX, dispatchNumY, dispatchNumZ);
    pContext->Flush();

    //
    // Draw test image
    //
    pContext->CSSetShaderResources(0, 1, &pTiledBufferView);
    pContext->CSSetUnorderedAccessViews(1, 1, &pObservedView, nullptr);
    pContext->CSSetShader(pTiledShader, nullptr, 0);
    pContext->Dispatch(dispatchNumX, dispatchNumY, dispatchNumZ);
    pContext->Flush();

    if (BUFFER_RESIDENCY_FRAGMENTED == m_parameters.pResidencyPattern->GetValue())
    {
        if (TRB_TEXTURE_2D == resourceBase)
        {
            // Clear the rectangles that represent null regions in the tiled resource
            const D3D11_RECT clearRects[2] =
            {
                {
                    0,
                    64,         // tileHeight / 2
                    512,        // tileWidth * 4
                    64 + 128    // tileHeight / 2 + tileHeight
                },
                {
                    0,
                    128 + 256,  // tileHeight + 2 * tileHeight
                    512,        // tileWidth * 4
                    512         // tileHeight * 4
                },
            };

            const float black[4] = { 0 };
            pContext->ClearView(pExpectedView, black, clearRects, 2);
            pContext->Flush();
            pContext->ClearView(pObservedView, black, clearRects, 2);
            pContext->Flush();
        }
        else
        {
            const D3D11_BOX clearBoxes[2] =
            {
                {
                    0,
                    64,         // tileHeight / 2
                    0,
                    512,        // tileWidth * 4
                    64 + 128,   // tileHeight / 2 + tileHeight
                    4
                },
                {
                    0,
                    128 + 256,  // tileHeight + 2 * tileHeight
                    0,
                    512,        // tileWidth * 4
                    512,        // tileHeight * 4
                    4
                },
            };

            const float black[4] = { 0 };
            CResourceBaseTestHelper<resourceBase>::ClearViewTex3D(pExpectedView, DXGI_FORMAT_R8G8B8A8_UINT, black, clearBoxes, 2);
            pContext->Flush();
            CResourceBaseTestHelper<resourceBase>::ClearViewTex3D(pObservedView, DXGI_FORMAT_R8G8B8A8_UINT, black, clearBoxes, 2);
            pContext->Flush();
        }
    }

    //
    // Display the results
    //
    {
        const UINT numSlices = (TRB_TEXTURE_2D == resourceBase ? 1 : depth);
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
            ShowResource(GenerateName(L"Observed Output Texture", slice), pObservedTexture, 0, 0, slice, 1.0f, DXGI_FORMAT_R8G8B8A8_UNORM);
            ShowResource(GenerateName(L"Expected Output Texture", slice), pExpectedTexture, 0, 0, slice, 1.0f, DXGI_FORMAT_R8G8B8A8_UNORM);
        }
    }

    //
    // Compare results against control
    //
    tr = CTiledResourcesTest::CompareTiledResourceToReference(pObservedTexture, pExpectedTexture, 0, 0, tileMapping);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    return tr;
}

template <bool isStructured, TILED_RESOURCE_BASE resourceBase>
TEST_RESULT CRawStructuredTestBase<isStructured, resourceBase>::CreateBuffers(_Inout_ ID3D11Buffer **ppTiledBuffer, _Inout_ ID3D11Buffer **ppNormalBuffer)
{
    HRESULT hr = S_OK;
    CHandle<ID3D11Buffer> pTiledBuffer;
    CHandle<ID3D11Buffer> pNormalBuffer;
    ID3D11Device2 *pDevice = GetDevice2();
    ID3D11DeviceContext2 *pContext = GetImmediateContext2();
    CTiledResourceHelper::CTilePoolManager *pTilePoolManager = GetFramework()->GetTilePoolManager();
    ID3D11Buffer *pTilePool = pTilePoolManager->GetTilePool();
    BUFFER_RESIDENCY residencyPattern = m_parameters.pResidencyPattern->GetValue();
    const UINT tier = CTiledResourcesTest::m_parameters.pTiledResourcesTier->GetValue();

    //
    // Create buffers
    //
    const UINT numPixelsWithinTile = CResourceBaseTestHelper<resourceBase>::widthPerTile * CResourceBaseTestHelper<resourceBase>::heightPerTile * CResourceBaseTestHelper<resourceBase>::depthPerTile;
    const UINT totalNumPixels = numPixelsWithinTile * CResourceBaseTestHelper<resourceBase>::totalNumTiles;

    static std::array<UINT, totalNumPixels> sysMem;
    static bool arrayInitialized = false;
    if (!arrayInitialized)
    {
        for (UINT i = 0; i < totalNumPixels; ++i)
        {
            sysMem[i] = static_cast<UINT>(i + 1);
        }
        arrayInitialized = true;
    }

    // Normal buffer
    CD3D11_BUFFER_DESC bufDesc(
        TILE_SIZE_IN_BYTES * CResourceBaseTestHelper<resourceBase>::totalNumTiles,
        D3D11_BIND_SHADER_RESOURCE
        );
    bufDesc.StructureByteStride = sizeof(UINT);
    bufDesc.MiscFlags = GetResourceMiscFlags();
    bufDesc.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = sysMem.data();
    data.SysMemPitch = sysMem.size();
    data.SysMemSlicePitch = 0;
    hr = pDevice->CreateBuffer(&bufDesc, &data, &pNormalBuffer);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateBuffer);
        return RESULT_FAIL;
    }

    // Tiled buffer
    bufDesc.MiscFlags = GetResourceMiscFlags() | D3D11_RESOURCE_MISC_TILED;
    hr = pDevice->CreateBuffer(&bufDesc, nullptr, &pTiledBuffer);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateBuffer);
        return RESULT_FAIL;
    }

    D3D11_TILED_RESOURCE_COORDINATE startCoord;
    D3D11_TILE_REGION_SIZE regionSize;
    UINT offsets[CResourceBaseTestHelper<resourceBase>::totalNumTiles];
    UINT tileCounts[CResourceBaseTestHelper<resourceBase>::totalNumTiles];
    UINT rangeFlags[CResourceBaseTestHelper<resourceBase>::totalNumTiles];
    UINT startOffset = pTilePoolManager->FetchRange(pTiledBuffer, CResourceBaseTestHelper<resourceBase>::totalNumTiles);
    UINT numRanges = 0;

    //
    // Update tile mappings
    //
    switch (residencyPattern)
    {
    case BUFFER_RESIDENCY_FULL:
        offsets[0] = startOffset;
        tileCounts[0] = CResourceBaseTestHelper<resourceBase>::totalNumTiles;
        startCoord.Subresource = 0;
        rangeFlags[0] = 0;
        startCoord.X = 0;
        startCoord.Y = 0;
        startCoord.Z = 0;
        regionSize.bUseBox = FALSE;
        regionSize.NumTiles = CResourceBaseTestHelper<resourceBase>::totalNumTiles;
        numRanges = 1;
        break;

    case BUFFER_RESIDENCY_FRAGMENTED:
        // Repeating the same pattern above for each depth slice
        const UINT numSlicesToRun = (TRB_TEXTURE_2D == resourceBase ? 1 : 4);
        for (UINT slice = 0; slice < numSlicesToRun; ++slice)
        {
            const UINT indexOffset = 4 * slice;
            const UINT startTileOffset = startOffset + 16 * slice;
            offsets[indexOffset + 0] = startTileOffset;
            tileCounts[indexOffset + 0] = 2;
            rangeFlags[indexOffset + 0] = 0;

            offsets[indexOffset + 1] = m_blankTileIndex;
            tileCounts[indexOffset + 1] = 4;
            rangeFlags[indexOffset + 1] = (1 == tier ? D3D11_TILE_RANGE_REUSE_SINGLE_TILE : D3D11_TILE_RANGE_NULL);

            offsets[indexOffset + 2] = startTileOffset + 8;
            tileCounts[indexOffset + 2] = 6;
            rangeFlags[indexOffset + 2] = 0;

            offsets[indexOffset + 3] = m_blankTileIndex;
            tileCounts[indexOffset + 3] = 4;
            rangeFlags[indexOffset + 3] = (1 == tier ? D3D11_TILE_RANGE_REUSE_SINGLE_TILE : D3D11_TILE_RANGE_NULL);
        }
        startCoord.Subresource = 0;
        startCoord.X = 0;
        startCoord.Y = 0;
        startCoord.Z = 0;
        regionSize.bUseBox = FALSE;
        regionSize.NumTiles = CResourceBaseTestHelper<resourceBase>::totalNumTiles;
        numRanges = 4 * numSlicesToRun;
        break;
    }

    hr = pContext->UpdateTileMappings(pTiledBuffer, 1, &startCoord, &regionSize, pTilePool, numRanges, rangeFlags, offsets, tileCounts, D3D11_TILE_MAPPING_NO_OVERWRITE);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, UpdateTileMappings);
        return RESULT_FAIL;
    }

    // Copy Normal buffer data to tiled buffer
    pContext->TiledResourceBarrier(nullptr, pTiledBuffer);
    pContext->CopyResource(pTiledBuffer, pNormalBuffer);

    *ppTiledBuffer = pTiledBuffer;
    *ppNormalBuffer = pNormalBuffer;

    pTiledBuffer.Detach();
    pNormalBuffer.Detach();

    return RESULT_PASS;
}

template <bool isStructured, TILED_RESOURCE_BASE resourceBase>
TEST_RESULT CRawStructuredTestBase<isStructured, resourceBase>::CreateShaders(_Inout_ ID3D11ComputeShader **ppTiledShader, _Inout_ ID3D11ComputeShader **ppNormalShader)
{
    HRESULT hr = S_OK;
    CHandle<ID3D11ComputeShader> pTiledShader;
    CHandle<ID3D11ComputeShader> pNormalShader;
    ID3D11Device2 *pDevice = GetDevice2();

    const CTiledResourcesTest::LOADABLE_SHADER_BYTECODE_TYPE shaderTypeToLoad = (isStructured ? CTiledResourcesTest::LSBT_COMPUTE_PROCESS_STUCTURED_BUFFER : CTiledResourcesTest::LSBT_COMPUTE_PROCESS_RAW_BUFFER);
    const BYTE *pShaderByteCodeNormal;
    const BYTE *pShaderByteCodeTiled;
    SIZE_T shaderLenNormal;
    SIZE_T shaderLenTiled;
    CTiledResourcesTest::GetShaderByteCode(shaderTypeToLoad, &pShaderByteCodeNormal, &shaderLenNormal, false, (TRB_TEXTURE_3D == resourceBase));
    CTiledResourcesTest::GetShaderByteCode(shaderTypeToLoad, &pShaderByteCodeTiled, &shaderLenTiled, true, (TRB_TEXTURE_3D == resourceBase));

    hr = pDevice->CreateComputeShader(pShaderByteCodeNormal, shaderLenNormal, nullptr, &pNormalShader);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateComputeShader);
        return RESULT_FAIL;
    }

    if (1 == CTiledResourcesTest::m_parameters.pTiledResourcesTier->GetValue())
    {
        pTiledShader = pNormalShader;
    }
    else
    {
        hr = pDevice->CreateComputeShader(pShaderByteCodeTiled, shaderLenTiled, nullptr, &pTiledShader);
        if (FAILED(hr))
        {
            REPORT_HRESULT_FAILURE(hr, CreateComputeShader);
            return RESULT_FAIL;
        }
    }

    *ppTiledShader = pTiledShader;
    *ppNormalShader = pNormalShader;
    pTiledShader.Detach();
    pNormalShader.Detach();

    return RESULT_PASS;
}

template <bool isStructured, TILED_RESOURCE_BASE resourceBase>
TEST_RESULT CRawStructuredTestBase<isStructured, resourceBase>::CreateViews(_In_ ID3D11Buffer *pTiledBuffer, _In_ ID3D11Buffer *pNormalBuffer, _Inout_ ID3D11ShaderResourceView **ppTiledView, _Inout_ ID3D11ShaderResourceView **ppNormalView)
{
    HRESULT hr = S_OK;
    CHandle<ID3D11ShaderResourceView> pTiledView;
    CHandle<ID3D11ShaderResourceView> pNormalView;
    ID3D11Device2 *pDevice = GetDevice2();

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    if (isStructured)
    {
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    }
    else
    {
        srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
        srvDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
    }
    srvDesc.BufferEx.FirstElement = 0;
    srvDesc.BufferEx.NumElements = TILE_SIZE_IN_BYTES * CResourceBaseTestHelper<resourceBase>::totalNumTiles / 4;

    // Normal view
    hr = pDevice->CreateShaderResourceView(pNormalBuffer, &srvDesc, &pNormalView);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateShaderResourceView);
        return RESULT_FAIL;
    }

    // Tiled view
    hr = pDevice->CreateShaderResourceView(pTiledBuffer, &srvDesc, &pTiledView);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateShaderResourceView);
        return RESULT_FAIL;
    }

    *ppNormalView = pNormalView;
    *ppTiledView = pTiledView;
    pTiledView.Detach();
    pNormalView.Detach();

    return RESULT_PASS;
}


template <bool isStructured, TILED_RESOURCE_BASE resourceBase>
TEST_RESULT CRawStructuredTestBase<isStructured, resourceBase>::TestTexture3DClear()
{
    //
    // Create Textures
    //
    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;
    CHandle<ID3D11TextureType> pExpectedTexture;
    CHandle<ID3D11TextureType> pObservedTexture;
    CHandle<ID3D11RenderTargetView> pRTVExpected;
    CHandle<ID3D11RenderTargetView> pRTVObserved;
    ID3D11Device2 *pDevice = GetDevice2();
    ID3D11DeviceContext2 *pContext = GetImmediateContext2();
    CTiledResourceHelper::CTilePoolManager *pTilePoolManager = GetFramework()->GetTilePoolManager();
    ID3D11Buffer *pTilePool = pTilePoolManager->GetTilePool();
    BUFFER_RESIDENCY residencyPattern = m_parameters.pResidencyPattern->GetValue();
    const UINT tier = CTiledResourcesTest::m_parameters.pTiledResourcesTier->GetValue();

    //
    // Create buffers
    //
    const UINT totalNumPixels = CResourceBaseTestHelper<resourceBase>::widthPerTile * CResourceBaseTestHelper<resourceBase>::heightPerTile * CResourceBaseTestHelper<resourceBase>::depthPerTile;

    static std::array<UINT, totalNumPixels> sysMem2;
    static bool array2Initialized = false;
    if (!array2Initialized)
    {
        union PIXEL
        {
            R8G8B8A8_PIXEL pxl;
            UINT val;
        };
        for (BYTE k = 0; k < CResourceBaseTestHelper<resourceBase>::depthPerTile; ++k)
        {
            for (BYTE j = 0; j < CResourceBaseTestHelper<resourceBase>::heightPerTile; ++j)
            {
                for (BYTE i = 0; i < CResourceBaseTestHelper<resourceBase>::widthPerTile; ++i)
                {
                    PIXEL pixel;
                    //                    pixel.pxl.r = i * (TRB_TEXTURE_2D == resourceBase ? 2 : 8);
                    //                    pixel.pxl.g = j * (TRB_TEXTURE_2D == resourceBase ? 2 : 8);
                    //                    pixel.pxl.b = k * (TRB_TEXTURE_2D == resourceBase ? 1 : 16);
                    pixel.pxl.r = i * (TRB_TEXTURE_2D == resourceBase ? 2 : 6) + k * (TRB_TEXTURE_2D == resourceBase ? 0 : 4);
                    pixel.pxl.g = j * (TRB_TEXTURE_2D == resourceBase ? 2 : 6) + k * (TRB_TEXTURE_2D == resourceBase ? 0 : 4);
                    pixel.pxl.b = 0;
                    pixel.pxl.a = 255;
                    sysMem2[i + CResourceBaseTestHelper<resourceBase>::widthPerTile * (j + CResourceBaseTestHelper<resourceBase>::heightPerTile * k)] = pixel.val;
                }
            }
        }
        array2Initialized = true;
    }

    // Normal texture
    CD3D11_TEXTURE_TYPE_DESC desc;
    CResourceBaseTestHelper<resourceBase>::GenerateTextureDesc(
        desc,
        DXGI_FORMAT_R8G8B8A8_TYPELESS,
        CResourceBaseTestHelper<resourceBase>::widthPerTile,
        CResourceBaseTestHelper<resourceBase>::heightPerTile,
        CResourceBaseTestHelper<resourceBase>::depthPerTile,
        0,
        D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_RENDER_TARGET
        );

    tr = CTiledResourcesTest::CreateTexture(&desc, &pExpectedTexture);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    desc.MiscFlags = D3D11_RESOURCE_MISC_TILED;
    tr = CTiledResourcesTest::CreateTexture(&desc, &pObservedTexture);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    // Copy memory to Normal texture
    D3D11_BOX destRegion;
    destRegion.left = 0;
    destRegion.top = 0;
    destRegion.front = 0;
    destRegion.right = CResourceBaseTestHelper<resourceBase>::widthPerTile;
    destRegion.bottom = CResourceBaseTestHelper<resourceBase>::heightPerTile;
    destRegion.back = CResourceBaseTestHelper<resourceBase>::depthPerTile;

    pContext->UpdateSubresource(
        pExpectedTexture,
        0,
        &destRegion,
        sysMem2.data(),
        4 * CResourceBaseTestHelper<resourceBase>::widthPerTile,
        4 * CResourceBaseTestHelper<resourceBase>::widthPerTile * CResourceBaseTestHelper<resourceBase>::heightPerTile
        );

    // finish tiled texture and copy data into it
    D3D11_TILED_RESOURCE_COORDINATE startCoord;
    D3D11_TILE_REGION_SIZE regionSize;
    UINT offsets[CResourceBaseTestHelper<resourceBase>::totalNumTiles];
    UINT tileCounts[CResourceBaseTestHelper<resourceBase>::totalNumTiles];
    UINT rangeFlags[CResourceBaseTestHelper<resourceBase>::totalNumTiles];
    UINT startOffset = pTilePoolManager->FetchRange(pObservedTexture, CResourceBaseTestHelper<resourceBase>::totalNumTiles);
    UINT numRanges = 0;

    //
    // Update tile mappings
    //
    switch (residencyPattern)
    {
    case BUFFER_RESIDENCY_FULL:
        offsets[0] = startOffset;
        tileCounts[0] = CResourceBaseTestHelper<resourceBase>::totalNumTiles;
        startCoord.Subresource = 0;
        rangeFlags[0] = 0;
        startCoord.X = 0;
        startCoord.Y = 0;
        startCoord.Z = 0;
        regionSize.bUseBox = FALSE;
        regionSize.NumTiles = CResourceBaseTestHelper<resourceBase>::totalNumTiles;
        numRanges = 1;
        break;

    case BUFFER_RESIDENCY_FRAGMENTED:
        // Repeating the same pattern above for each depth slice
        const UINT numSlicesToRun = (TRB_TEXTURE_2D == resourceBase ? 1 : 4);
        for (UINT slice = 0; slice < numSlicesToRun; ++slice)
        {
            const UINT indexOffset = 4 * slice;
            const UINT startTileOffset = startOffset + 16 * slice;
            offsets[indexOffset + 0] = startTileOffset;
            offsets[indexOffset + 1] = m_blankTileIndex;
            offsets[indexOffset + 2] = startTileOffset + 8;
            offsets[indexOffset + 3] = m_blankTileIndex;
            tileCounts[indexOffset + 0] = 2;
            tileCounts[indexOffset + 1] = 4;
            tileCounts[indexOffset + 2] = 6;
            tileCounts[indexOffset + 3] = 4;
            rangeFlags[indexOffset + 0] = 0;
            rangeFlags[indexOffset + 1] = (1 == tier ? D3D11_TILE_RANGE_REUSE_SINGLE_TILE : D3D11_TILE_RANGE_NULL);
            rangeFlags[indexOffset + 2] = 0;
            rangeFlags[indexOffset + 3] = (1 == tier ? D3D11_TILE_RANGE_REUSE_SINGLE_TILE : D3D11_TILE_RANGE_NULL);
        }
        startCoord.Subresource = 0;
        startCoord.X = 0;
        startCoord.Y = 0;
        startCoord.Z = 0;
        regionSize.bUseBox = FALSE;
        regionSize.NumTiles = CResourceBaseTestHelper<resourceBase>::totalNumTiles;
        numRanges = 4 * numSlicesToRun;
        break;
    }

    hr = pContext->UpdateTileMappings(pObservedTexture, 1, &startCoord, &regionSize, pTilePool, numRanges, rangeFlags, offsets, tileCounts, D3D11_TILE_MAPPING_NO_OVERWRITE);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, UpdateTileMappings);
        return RESULT_FAIL;
    }

    // Copy Normal buffer data to tiled buffer
    pContext->TiledResourceBarrier(nullptr, pObservedTexture);
    pContext->CopyResource(pObservedTexture, pExpectedTexture);

    //
    // Use ClearView for exact middle of the texture
    //
    CD3D11_RENDER_TARGET_VIEW_DESC descRTV;
    CResourceBaseTestHelper<resourceBase>::GenerateRenderTargetViewDesc(
        descRTV,
        pExpectedTexture,
        D3D11_RTV_DIMENSION_TEXTURE2D,  // ignored for Texture3D
        DXGI_FORMAT_R8G8B8A8_UNORM
        );

    hr = pDevice->CreateRenderTargetView(pExpectedTexture, &descRTV, &pRTVExpected);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateRenderTargetView);
        return RESULT_FAIL;
    }

    hr = pDevice->CreateRenderTargetView(pObservedTexture, &descRTV, &pRTVObserved);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateRenderTargetView);
        return RESULT_FAIL;
    }

    const float black[4] = { 0 };
    if (TRB_TEXTURE_2D == resourceBase)
    {
        D3D11_RECT rectToClear;
        rectToClear.left = CResourceBaseTestHelper<resourceBase>::widthPerTile / 4;
        rectToClear.top = CResourceBaseTestHelper<resourceBase>::heightPerTile / 4;
        rectToClear.right = (3 * CResourceBaseTestHelper<resourceBase>::widthPerTile) / 4;
        rectToClear.bottom = (3 * CResourceBaseTestHelper<resourceBase>::heightPerTile) / 4;

        pContext->ClearView(pRTVObserved, black, &rectToClear, 1);
        pContext->ClearView(pRTVExpected, black, &rectToClear, 1);
    }
    else
    {
        D3D11_BOX boxToClear;
        boxToClear.left = CResourceBaseTestHelper<resourceBase>::widthPerTile / 4;
        boxToClear.top = CResourceBaseTestHelper<resourceBase>::heightPerTile / 4;
        boxToClear.front = CResourceBaseTestHelper<resourceBase>::depthPerTile / 4;
        boxToClear.right = (3 * CResourceBaseTestHelper<resourceBase>::widthPerTile) / 4;
        boxToClear.bottom = (3 * CResourceBaseTestHelper<resourceBase>::heightPerTile) / 4;
        boxToClear.back = (3 * CResourceBaseTestHelper<resourceBase>::depthPerTile) / 4;

        CResourceBaseTestHelper<resourceBase>::ClearViewTex3D(pRTVObserved, DXGI_FORMAT_R8G8B8A8_UNORM, black, &boxToClear, 1);
        CResourceBaseTestHelper<resourceBase>::ClearViewTex3D(pRTVExpected, DXGI_FORMAT_R8G8B8A8_UNORM, black, &boxToClear, 1);
    }

    //
    // Show the resources
    //
    {
        const UINT numSlices = CResourceBaseTestHelper<resourceBase>::depthPerTile;
        UINT nameIndex = 0;
        wchar_t combinedName[2 * numSlices][64];
        for (UINT slice = 0; slice < numSlices; ++slice)
        {
            wchar_t sliceName[32];
            auto GenerateName = [&](const wchar_t *prependName, const UINT slice) -> const wchar_t*
            {
                swprintf_s(sliceName, ARRAYSIZE(sliceName), L" (Slice %d)", slice);
                swprintf_s(combinedName[nameIndex], ARRAYSIZE(combinedName[nameIndex]), L"%s%s", prependName, (numSlices == 1 ? L"" : sliceName));
                return combinedName[nameIndex++];
            };
            ShowResource(GenerateName(L"Observed Output Texture", slice), pObservedTexture, 0, 0, slice, 2.0f, DXGI_FORMAT_R8G8B8A8_UNORM);
            ShowResource(GenerateName(L"Expected Output Texture", slice), pExpectedTexture, 0, 0, slice, 2.0f, DXGI_FORMAT_R8G8B8A8_UNORM);
        }
    }

    //
    // Clean up
    //
    pExpectedTexture.Detach();

    return RESULT_PASS;
}
