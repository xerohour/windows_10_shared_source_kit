////////////////////////////////////////////////////////////////////////////////
// TiledResources.h
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "d3d11_2.h"
#include "ResourceViewer.h"

#include <functional>

////////////////////////////////////////////////////////////////////////////////
//
// Constants
//
////////////////////////////////////////////////////////////////////////////////
extern const UINT TILE_SIZE_IN_BYTES;
extern const UINT MAX_NUM_TILE_POOL_RANGES;
extern const UINT FIXED_ARRAY_SIZE;
extern const DXGI_FORMAT FIXED_FORMAT_TEX1D;
extern const DXGI_FORMAT FIXED_FORMAT_TEX2D;
extern const DXGI_FORMAT FIXED_FORMAT_TEX3D;
extern const UINT TEX1D_WIDTH_PER_TILE;

////////////////////////////////////////////////////////////////////////////////
//
// Useful macros
//
////////////////////////////////////////////////////////////////////////////////

#define REPORT_HRESULT_FAILURE(hr, api) \
    WriteToLog(_T(#api "() unexpectedly failed with error code %s"), D3DHResultToString(hr).c_str())

// Macro for quickly testing a variable agains an expected value
#define TEST_VARIABLE_VALUE(variable, value, tr) \
    if(value != variable ) \
        { \
        WriteToLog( _T(#variable " incorrectly set to %s.  Expected: %s"), ToString(variable).c_str(), ToString(value).c_str()); \
        tr = RESULT_FAIL; \
        }

// Macro for quickly testing a variable agains a bitfield
#define TEST_VARIABLE_BITS(variable, value, tr) \
    if(0 != (variable ^ value)) \
        { \
        WriteToLog( _T(#variable " incorrectly set to 0x%8.8x.  Expected: 0x%8.8x"), variable, value); \
        tr = RESULT_FAIL; \
        }


////////////////////////////////////////////////////////////////////////////////
//
// CTiledResourcesApp
//
////////////////////////////////////////////////////////////////////////////////
class CTiledResourcesApp : public CD3D11TestFramework
{
public:
    CTiledResourcesApp();

    TESTTYPE GetTestType() const { return TESTTYPE_CONF; }

private:
    bool InitTestGroups();
    void InitOptionVariables();
};



class CTileMapping
{
public:
    CTileMapping();
    CTileMapping(const CTileMapping &copy);
    CTileMapping(UINT width, UINT height, const UINT *mappingData);

    void Set(const CTileMapping &copy);
    void SetFromStrings(const char *rows[4]);
    void GetStrings(char *rows[4]) const;

    void Transform(_Inout_ CTileMapping &newmap, const int xform[4][4]);

    void Overlay(const CTileMapping &overlay);

    void Clear();

    struct
    {
        UINT Column[4];
    } Row[4];

    UINT Width;
    UINT Height;

    bool operator==(const CTileMapping &tileMapping) const;

    inline void Set(const UINT value, const UINT column, const UINT row, const UINT slice = 0)
    {
        Row[row].Column[column] = value;
    }

    inline UINT Get(const UINT column, const UINT row, const UINT slice = 0) const
    {
        return Row[row].Column[column];
    }
};

class CVolumeTileMapping
{
public:
    CVolumeTileMapping();
    CVolumeTileMapping(const CVolumeTileMapping &copy);
    CVolumeTileMapping(UINT width, UINT height, UINT depth, const UINT *mappingData);

    void Set(const CVolumeTileMapping &copy);
    void SetFromStrings(const char *slices[4][4]);
    void GetStrings(char *slices[4][4]) const;

    void Transform(_Inout_ CVolumeTileMapping &newmap, const int xform[4][4][4]);

    void Overlay(const CVolumeTileMapping &overlay);

    void Clear();

    struct
    {
        struct
        {
            UINT Column[4];
        } Row[4];
    } Slice[4];

    UINT Width;
    UINT Height;
    UINT Depth;

    bool operator==(const CVolumeTileMapping &tileMapping) const;

    inline void Set(const UINT value, const UINT column, const UINT row, const UINT slice)
    {
        Slice[slice].Row[row].Column[column] = value;
    }

    inline UINT Get(const UINT column, const UINT row, const UINT slice) const
    {
        return Slice[slice].Row[row].Column[column];
    }

    static char EncodeChar(const UINT toEncode);
    static UINT DecodeChar(const char toDecode);
};

class CBufferMappingBaseParent
{
public:
    virtual UINT GetTile(const UINT tileIndex) const = 0;
};

template <UINT NUM_TILES>
class CBufferMappingBase : public CBufferMappingBaseParent
{
public:
    CBufferMappingBase() { for (UINT i = 0; i < NUM_TILES; ++i) Tiles[i] = i; }

    void Clear() { for (UINT i = 0; i < NUM_TILES; ++i) Tiles[i] = 0xffffffffu; }

    UINT GetTile(const UINT tileIndex) const { return Tiles[tileIndex]; }

    UINT Tiles[NUM_TILES];
};

class CBufferMapping : public CBufferMappingBase < 16 > { };
class CVolumeBufferMapping : public CBufferMappingBase < 64 > { };

struct R8G8B8A8_PIXEL
{
    BYTE r;
    BYTE g;
    BYTE b;
    BYTE a;
    bool operator!=(const R8G8B8A8_PIXEL& x) const
    {
        return !(r == x.r && g == x.g && b == x.b && a == x.a);
    }
};

////////////////////////////////////////////////////////////////////////////////
//
// CTiledResourcesTest
//
////////////////////////////////////////////////////////////////////////////////
class CTiledResourcesTest : public CD3D11Test
{
public:
    CTiledResourcesTest();
    virtual TEST_RESULT Setup();
    virtual void Cleanup();
    virtual TEST_RESULT SetupTestCase();

    struct R32_PIXEL
    {
        DWORD r;
    };

    struct R16_PIXEL
    {
        WORD r;
    };

    struct R8_PIXEL
    {
        BYTE r;
    };

    struct TEXTURE_PIXEL_POINTER
    {
        union
        {
            BYTE *pBits;
            R8G8B8A8_PIXEL *pRGBA8Data;
            R32_PIXEL *pR32Data;
            R16_PIXEL *pR16Data;
            R8_PIXEL *pR8Data;
        };
    };

    //
    // Enum values
    //

    // Residency patterns for buffer and texture 1D
    enum RESIDENCY_PATTERN_1D
    {
        RP1D_FRAGMENTED_MULTI_TILE,
        RP1D_REUSE_SINGLE_TILE,
    };

    // Residency patterns for texture 2D and texture 3D
    enum RESIDENCY_PATTERN_2D_3D
    {
        RP2D_3D_FULL,
        RP2D_3D_FULL_MULTI_REGION,
        RP2D_3D_FULL_MIP_1,
        RP2D_3D_FRAGMENTED_MULTI_TILE,
        RP2D_3D_FRAGMENTED_MULTI_REGION,
        RP2D_3D_FRAGMENTED_MULTI_TILE_SKIPS,
        RP2D_3D_FRAGMENTED_REUSE_SINGLE_TILE,
        RP2D_3D_FRAGMENTED_MIP_1,
        RP2D_3D_UNIFORM_BOX,
        RP2D_3D_CLEAR_MIP_0,
        RP2D_3D_CLEAR_MIP_1,
        RP2D_3D_CLEAR_ALL_MIPS
    };

    enum MINMAX_FILTER
    {
        MMF_MINIMUM_MIN_MAG_MIP_LINEAR,
        MMF_MAXIMUM_MIN_MAG_MIP_LINEAR
    };

    enum COPY_TILE_MAPPINGS_PATTERN
    {
        COPY_TILE_MAPPINGS_FULL,
        COPY_TILE_MAPPINGS_PARTIAL,
        COPY_TILE_MAPPINGS_OVERLAPPED,
    };

    enum COPY_TILES_PATTERN
    {
        COPY_TILES_FULL,
        COPY_TILES_CENTER_BOX,
        COPY_TILES_PARTIAL,
        COPY_TILES_MIP1
    };

    enum LOADABLE_SHADER_BYTECODE_TYPE
    {
        LSBT_COMPUTE_COPY_TEX,                      // CopyTexNormal and CopyTexTiled from TiledResourcesCS.hlsl
        LSBT_COMPUTE_PROCESS_RAW_BUFFER,            // ProcessRawBufferNormal and ProcessRawBufferTiled from TiledResourcesCS.hlsl
        LSBT_COMPUTE_PROCESS_STUCTURED_BUFFER,      // ProcessStructuredBufferNormal and ProcessStructuredBufferTiled from TiledResourcesCS.hlsl
        LSBT_PIXEL_COPY_TEX_TIER_1,                 // CopyTexTier1PS from TiledResourcesPS.hlsl
        LSBT_PIXEL_COPY_TEX_TIER_2,                 // CopyTexTier2PS from TiledResourcesPS.hlsl
        LSBT_PIXEL_RENDER_DEPTH,                    // RenderDepthPS from TiledResourcesPS.hlsl (no version for Texture3D)
        LSBT_PIXEL_RENDER_COLOR_USING_DEPTH,        // RenderColorUsingDepthPS from TiledResourcesPS.hlsl (no version for Texture3D)
        LSBT_PIXEL_FILTER_TIER_1,                   // FilterNormalTier1PS and FilterTiledTier1PS from TiledResourcesPS.hlsl (no version for Texture3D)
        LSBT_PIXEL_FILTER_TIER_2,                   // FilterNormalTier2PS and FilterTiledTier2PS from TiledResourcesPS.hlsl
        LSBT_PIXEL_GATHER_TIER_1,                   // GatherNormalTier1PS and GatherTiledTier1PS from TiledResourcesPS.hlsl (no version for Texture3D)
        LSBT_PIXEL_GATHER_TIER_2,                   // GatherNormalTier2PS and GatherTiledTier2PS from TiledResourcesPS.hlsl (no version for Texture3D)
        LSBT_VERTEX_COPY_TEX,                       // CopyTexVS from TiledResourcesVS.hlsl
        LSBT_GEOMETRY_COPY_TEX_3D,                  // CopyTexGS from TiledResourcesPS.hlsl
    };

protected:
    //
    // Data members
    //
    UINT m_blankTileIndex;
    UINT m_maxSupportedTiledResourcesTier;
    struct
    {
        CVariationParameter<UINT> *pTiledResourcesTier;
    } m_parameters;
    ID3D11VertexShader *m_pCopyTexVS;
    ID3D11PixelShader *m_pCopyTexTier1PS;
    ID3D11PixelShader *m_pCopyTexTier2PS;
    ID3D11VertexShader *m_pCopyTexVSTex3D;
    ID3D11GeometryShader *m_pCopyTexGSTex3D;
    ID3D11PixelShader *m_pCopyTexTier1PSTex3D;
    ID3D11PixelShader *m_pCopyTexTier2PSTex3D;
    ID3D11Texture2D *m_pNullSentinelTexture; // Always NULL mapped and occasionally checked for corruption (should always be 'black')

    //
    // Helper functions
    //

    testfactor::RFactor InitTiledResourcesTierParam(const bool skipTiers12 = false);
    void GetTileSize(DXGI_FORMAT format, _Out_ UINT &tileWidth, _Out_ UINT &tileHeight);
    void GetTileSize(DXGI_FORMAT format, _Out_ UINT &tileWidth, _Out_ UINT &tileHeight, _Out_ UINT &tileDepth);
    UINT GetBlankRects(_In_ const CTileMapping &tileMapping, _Out_ D3D11_RECT *pRects, DXGI_FORMAT format);
    UINT GetBlankBoxes(_In_ const CVolumeTileMapping &tileMapping, _Out_ D3D11_BOX *pBoxes, DXGI_FORMAT format);
    void ClearBlankRegions(_In_ const CTileMapping &tileMapping, _In_ ID3D11View *pView, DXGI_FORMAT format);
    void ClearBlankRegions(_In_ const CVolumeTileMapping &tileMapping, _In_ ID3D11View *pView, DXGI_FORMAT format, const UINT mip = 0);
    HRESULT ClearViewTex3D(_In_ ID3D11View *pView, DXGI_FORMAT format, _In_ const float *pColor, _In_reads_opt_(numBoxes) const D3D11_BOX *pBox, const UINT numBoxes, const UINT mip = 0);

    static void GetShaderByteCode(LOADABLE_SHADER_BYTECODE_TYPE shaderType, _Out_ const BYTE **ppShaderCode, _Out_ SIZE_T *pShaderCodeLength, const bool getTiledVersion, const bool getTexture3DVersion);

    // General resource creation
    TEST_RESULT CreateTexture(_In_ const D3D11_TEXTURE2D_DESC *pResourceDesc, _Out_ ID3D11Texture2D **ppTiledResource);
    TEST_RESULT CreateTexture(_In_ const D3D11_TEXTURE3D_DESC *pResourceDesc, _Out_ ID3D11Texture3D **ppTiledResource);
    TEST_RESULT CreateBuffer(_In_ const D3D11_BUFFER_DESC *pResourceDesc, _Out_ ID3D11Buffer **ppTiledResource);

    // Tiled resource creation
    TEST_RESULT CreateTiledTexture(const UINT width, const UINT height, const UINT unused, _Out_ ID3D11Texture2D **ppTiledResource, UINT additionalBindFlags = 0);
    TEST_RESULT CreateTiledTexture(const UINT width, const UINT height, const UINT depth, _Out_ ID3D11Texture3D **ppTiledResource, UINT additionalBindFlags = 0);
    TEST_RESULT CreateTiledBuffer(UINT byteWidth, _Out_ ID3D11Buffer **ppTiledResource);
    TEST_RESULT CreateCustomTiledTexture(_In_ const D3D11_TEXTURE2D_DESC *pDesc, _Out_ ID3D11Texture2D **ppTiledResource);
    TEST_RESULT CreateCustomTiledTexture(_In_ const D3D11_TEXTURE3D_DESC *pDesc, _Out_ ID3D11Texture3D **ppTiledResource);

    // Staging resource creation
    TEST_RESULT CreateStagingTex1D(UINT width, _Out_ ID3D11Texture1D **ppStagingResource, bool isSource);
    TEST_RESULT CreateStagingTex2D(UINT width, UINT height, _Out_ ID3D11Texture2D **ppStagingResource, bool isSource);
    TEST_RESULT CreateStagingTex3D(UINT width, UINT height, UINT depth, _Out_ ID3D11Texture3D **ppStagingResource, bool isSource);
    TEST_RESULT CreateStagingBuffer(UINT byteWidth, _Out_ ID3D11Buffer **ppStagingResource, bool isSource);

    // Residency pattern initialization
    TEST_RESULT UpdateTileMappingsTex1D(_In_ ID3D11Texture1D *pTiledResource, _In_ ID3D11Buffer *pTilePool, RESIDENCY_PATTERN_1D pattern);
    TEST_RESULT UpdateTileMappingsTex2D(_In_ ID3D11Texture2D *pTiledResource, UINT tilePoolBaseOffset, _In_ ID3D11Buffer *pTilePool, RESIDENCY_PATTERN_2D_3D pattern, _Out_ CTileMapping &tileMapping, bool useNoOverwrite = true);
    TEST_RESULT UpdateTileMappingsTex3D(_In_ ID3D11Texture3D *pTiledResource, UINT tilePoolBaseOffset, _In_ ID3D11Buffer *pTilePool, RESIDENCY_PATTERN_2D_3D pattern, _Out_ CVolumeTileMapping &tileMapping, bool useNoOverwrite = true);
    TEST_RESULT UpdateTileMappingsBuffer(_In_ ID3D11Buffer *pTileResource, _In_ ID3D11Buffer *pTilePool, RESIDENCY_PATTERN_1D pattern);

    // Useful helper method for initializing a tiled resource to a default
    // tile index for Tier-1 since NULL-mapped tiles are not supported
    TEST_RESULT MapEntireTextureToSingleTile(_In_ ID3D11Texture2D *pTiledResource, UINT tileIndex, _In_ ID3D11Buffer *pTilePool);
    TEST_RESULT MapEntireTextureToSingleTile(_In_ ID3D11Texture3D *pTiledResource, UINT tileIndex, _In_ ID3D11Buffer *pTilePool);

    // Copying tile mappings
    TEST_RESULT CopyTileMappings(_In_ ID3D11Texture2D *pDest, _In_ ID3D11Texture2D *pSrc, COPY_TILE_MAPPINGS_PATTERN pattern, _Inout_ CTileMapping &tileMapping);
    TEST_RESULT CopyTileMappings(_In_ ID3D11Texture3D *pDest, _In_ ID3D11Texture3D *pSrc, COPY_TILE_MAPPINGS_PATTERN pattern, _Inout_ CVolumeTileMapping &tileMapping);

    // CopyTiles
    TEST_RESULT CopyTiles(_In_ ID3D11Texture2D *pTiledResource, _In_ ID3D11Buffer *pBuffer, UINT bufferStartOffset, COPY_TILES_PATTERN pattern, UINT flags, _Out_ CTileMapping &bufferMapping);
    TEST_RESULT CopyTiles(_In_ ID3D11Texture3D *pTiledResource, _In_ ID3D11Buffer *pBuffer, UINT bufferStartOffset, COPY_TILES_PATTERN pattern, UINT flags, _Out_ CVolumeTileMapping &bufferMapping);
    TEST_RESULT CreateTileStreamBuffer(_Out_ ID3D11Buffer **ppBuffer, UINT bufferSize, UINT dataOffset, BYTE *bufferData);
    TEST_RESULT FillBufferWithTilePattern(_Inout_ R8G8B8A8_PIXEL *pBuffer, UINT tileWidth, UINT tileHeight);
    TEST_RESULT FillBufferWithTilePattern(_Inout_ R8G8B8A8_PIXEL *pBuffer, UINT tileWidth, UINT tileHeight, UINT tileDepth);
    TEST_RESULT GetCoordinatesAndSizeFromCopyPattern(_In_ COPY_TILES_PATTERN pattern, _Out_ D3D11_TILED_RESOURCE_COORDINATE &tiledResourceCoord, _Out_ D3D11_TILE_REGION_SIZE &tileRegionSize, _Inout_ UINT &bufferOffset, _Out_ CTileMapping &bufferMapping);
    TEST_RESULT GetCoordinatesAndSizeFromCopyPattern(_In_ COPY_TILES_PATTERN pattern, _Out_ D3D11_TILED_RESOURCE_COORDINATE &tiledResourceCoord, _Out_ D3D11_TILE_REGION_SIZE &tileRegionSize, _Inout_ UINT &bufferOffset, _Out_ CVolumeTileMapping &bufferMapping);

    TEST_RESULT UpdateTiles(_In_ ID3D11Texture2D *pTiledResource, _In_ BYTE *pBuffer, COPY_TILES_PATTERN pattern, UINT flags, _Out_ CTileMapping &bufferMapping);
    TEST_RESULT UpdateTiles(_In_ ID3D11Texture3D *pTiledResource, _In_ BYTE *pBuffer, COPY_TILES_PATTERN pattern, UINT flags, _Out_ CVolumeTileMapping &bufferMapping);

    TEST_RESULT IntersectMappings(_In_ const CTileMapping& residencyMap, COPY_TILES_PATTERN copyPattern, _Out_ CTileMapping& copiedMap, _Out_ CBufferMapping& bufferMap);
    TEST_RESULT IntersectMappings(_In_ const CVolumeTileMapping& residencyMap, COPY_TILES_PATTERN copyPattern, _Out_ CVolumeTileMapping& copiedMap, _Out_ CVolumeBufferMapping& bufferMap);

    // Convert staging resource data to CTileMapping
    TEST_RESULT TileDataFromTex2D(_In_ ID3D11Texture2D *pTiledResource, UINT subResource, CTileMapping &foundMapping);

    // Copy via render methods
    TEST_RESULT RenderTextureToSimilar(_In_ ID3D11Texture2D *pDest, _In_ ID3D11Texture2D *pSrc, const UINT mipLevel, UINT const arraySlice);
    TEST_RESULT RenderTextureToSimilar(_In_ ID3D11Texture3D *pDest, _In_ ID3D11Texture3D *pSrc, const UINT mipLevel, UINT const arraySlice);
    TEST_RESULT RenderViewToSimilar(_In_ ID3D11RenderTargetView *pDest, _In_ ID3D11ShaderResourceView *pSrc, const UINT width, const UINT height, const UINT depth = 1, bool isTexture3D = false);


    //
    // Validation methods
    //

    // Comparing tiled resource to staging resource
    TEST_RESULT ValidateTileMapping(_In_ ID3D11Texture2D *pTiledResourceTest, _In_ ID3D11Texture2D *pReference, UINT mipLevel, UINT arraySlice, _In_ const CTileMapping &expectedMapping);
    TEST_RESULT ValidateTileMapping(_In_ ID3D11Texture3D *pTiledResourceTest, _In_ ID3D11Texture3D *pReference, UINT mipLevel, UINT unused, _In_ const CVolumeTileMapping &expectedMapping);
    TEST_RESULT CompareTiledResourceToReference(_In_ ID3D11Texture2D *pTiledResourceTest, _In_ ID3D11Texture2D *pReference, UINT mipLevel, UINT arraySlice, _In_ const CTileMapping &expectedMapping, UINT tileWidth = 0, UINT tileHeight = 0, UINT unused = 0);
    TEST_RESULT CompareTiledResourceToReference(_In_ ID3D11Texture3D *pTiledResourceTest, _In_ ID3D11Texture3D *pReference, UINT mipLevel, UINT unused, _In_ const CVolumeTileMapping &expectedMapping, UINT tileWidth = 0, UINT tileHeight = 0, UINT tileDepth = 0);
    TEST_RESULT ValidateTileAgainstBuffer(_In_ ID3D11Texture2D *pTiledResourceTest, _In_ R8G8B8A8_PIXEL *pBuffer, UINT mipLevel, UINT arraySlice, _In_ const CTileMapping &expectedMapping, float clearColor[4]);
    TEST_RESULT ValidateTileAgainstBuffer(_In_ ID3D11Texture3D *pTiledResourceTest, _In_ R8G8B8A8_PIXEL *pBuffer, UINT mipLevel, UINT unused, _In_ const CVolumeTileMapping &expectedMapping, float clearColor[4]);
    TEST_RESULT ValidateBufferAgainstOriginal(_In_ ID3D11Buffer *pOutputBuffer, _In_ R8G8B8A8_PIXEL *pOriginal, UINT numPixels, UINT bufferOffset, _In_ const CBufferMappingBaseParent &expectedMapping);


    // CheckNullSentinel()
    // Makes sure that a NULL-mapped resource always contains zeros no matter how many times
    // other resources "write" to their own NULL-mapped logical tiles
    TEST_RESULT CheckNullSentinel();

    // TestNullTiles() 
    // Ensures that writes to one NULL-mapped logical tile in a tiled resource do not cause corruptions when reading from other NULL-mapped logical tiles in the same resource.
    // Requires at least two NULL-mapped tiles in the tiled resource
    TEST_RESULT TestNullTiles(_In_ ID3D11Texture2D *pTiledResource, UINT mipLevel, UINT arraySlice, _In_ const CTileMapping &tileMapping);
    TEST_RESULT TestNullTiles(_In_ ID3D11Texture3D *pTiledResource, UINT mipLevel, UINT unused, _In_ const CVolumeTileMapping &tileMapping);


private:
    // Helper functions which should not be called directly. Their purpose is to eliminate copy/paste overhead
    // while keeping usage simple.
    //
    // NOTE: some of the parameters may not be useful depending whether Texture2D or Texture3D is the base.
    // This is noted by using unused as parameter name.
    template <class D3D11_TEXTURE_DESC_TYPE = D3D11_TEXTURE2D_DESC, class ID3D11_TEXTURE_TYPE = ID3D11Texture2D>
    TEST_RESULT CreateCustomTiledTextureHelper(_In_ const D3D11_TEXTURE_DESC_TYPE *pDesc, _Out_ ID3D11_TEXTURE_TYPE **ppTiledResource);

    template <class ID3D11_TEXTURE_TYPE = ID3D11Texture2D>
    TEST_RESULT MapEntireTextureToSingleTile(_In_ ID3D11_TEXTURE_TYPE *pTiledResource, UINT tileIndex, _In_ ID3D11Buffer *pTilePool);

    template <class ID3D11_TEXTURE_TYPE = ID3D11Texture2D, class TILE_MAPPING_TYPE = CTileMapping>
    TEST_RESULT CopyTiles(_In_ ID3D11_TEXTURE_TYPE *pTiledResource, _In_ ID3D11Buffer *pBuffer, UINT bufferStartOffset, COPY_TILES_PATTERN pattern, UINT flags, _Out_ TILE_MAPPING_TYPE &bufferMapping);

    template <class ID3D11_TEXTURE_TYPE = ID3D11Texture2D, class TILE_MAPPING_TYPE = CTileMapping>
    TEST_RESULT UpdateTiles(_In_ ID3D11_TEXTURE_TYPE *pTiledResource, _In_ BYTE *pBuffer, COPY_TILES_PATTERN pattern, UINT flags, _Out_ TILE_MAPPING_TYPE &bufferMapping);

    template <bool isTexture3D = false, class ID3D11_TEXTURE_TYPE = ID3D11Texture2D, class D3D11_TEXTURE_DESC_TYPE = D3D11_TEXTURE2D_DESC>
    TEST_RESULT RenderTextureToSimilar(_In_ ID3D11_TEXTURE_TYPE *pDest, _In_ ID3D11_TEXTURE_TYPE *pSrc, const UINT mipLevel, UINT const arraySlice);

    template <bool isTexture3D = false, class ID3D11_TEXTURE_TYPE = ID3D11Texture2D, class D3D11_TEXTURE_DESC_TYPE = D3D11_TEXTURE2D_DESC, class TILE_MAPPING_TYPE = CTileMapping>
    TEST_RESULT ValidateTileMapping(_In_ ID3D11_TEXTURE_TYPE *pTiledResource, _In_ ID3D11_TEXTURE_TYPE *pReference, UINT mipLevel, UINT arraySlice, const TILE_MAPPING_TYPE &expectedMapping);

    template <bool isTexture3D = false, class ID3D11_TEXTURE_TYPE = ID3D11Texture2D, class D3D11_TEXTURE_DESC_TYPE = D3D11_TEXTURE2D_DESC, class TILE_MAPPING_TYPE = CTileMapping>
    TEST_RESULT CompareTiledResourceToReference(_In_ ID3D11_TEXTURE_TYPE *pTiledResource, _In_ ID3D11_TEXTURE_TYPE *pReference, UINT mipLevel, UINT arraySlice, const TILE_MAPPING_TYPE &expectedMapping, UINT tileWidth, UINT tileHeight, UINT tileDepth);

    template <bool isTexture3D = false, class ID3D11_TEXTURE_TYPE = ID3D11Texture2D, class D3D11_TEXTURE_DESC_TYPE = D3D11_TEXTURE2D_DESC, class TILE_MAPPING_TYPE = CTileMapping>
    TEST_RESULT ValidateTileAgainstBuffer(_In_ ID3D11_TEXTURE_TYPE *pTiledResource, _In_ R8G8B8A8_PIXEL *pBuffer, UINT mipLevel, UINT arraySlice, _In_ const TILE_MAPPING_TYPE &expectedMapping, float clearColor[4]);

    template <bool isTexture3D = false, class ID3D11_TEXTURE_TYPE = ID3D11Texture2D>
    TEST_RESULT CreateStagingTextureHelper(
        UINT width,
        UINT height,
        UINT depth,
        ID3D11_TEXTURE_TYPE **ppStagingResource,
        bool isSource,
        const UINT widthPerTile,
        const UINT heightPerTile,
        const UINT depthPerTile,
        const std::function<void(R8G8B8A8_PIXEL **pixel, const UINT i, const UINT j, const UINT k, const UINT a, const UINT b, const UINT c, const UINT count)> &PerPixelFunction
        );
};


////////////////////////////////////////////////////////////////////////////////
//
// Abstractions for Texture2D vs Texture3D Tiled Resources tests
//
// Basically, any test should inherit from CResourceBaseTestHelper
// And must use its functionality to write a unified test (where possible)
// for both 2D and 3D tiled resources.
//
// Hence CResourceBaseTestHelper can be thought of as a layer between
// a 2D or 3D test and any API calls specific to either
//
////////////////////////////////////////////////////////////////////////////////

// Base texture for the tiled resources we'll be initializing
enum TILED_RESOURCE_BASE
{
    TRB_TEXTURE_2D,
    TRB_TEXTURE_3D
};

// Generalized Resource base class abstracting dependency on Texture2D and Texture3D
// Values below are to appease the compiler and should never be called
template <TILED_RESOURCE_BASE resourceBase>
class CResourceBaseTestHelper : public CTiledResourcesTest
{
};


// Macro to help in defining shorter types for base tests deriving from CResourceBaseTestHelper <>
// NOTE: these must correspond to what's defined in specialized classes below
#define CRESOURCE_BASE_TEST_HELPER__TYPENAME_TYPEDEFS \
    typedef typename CResourceBaseTestHelper<resourceBase>::ID3D11TextureType        ID3D11TextureType; \
    typedef typename CResourceBaseTestHelper<resourceBase>::CD3D11_TEXTURE_TYPE_DESC CD3D11_TEXTURE_TYPE_DESC; \
    typedef typename CResourceBaseTestHelper<resourceBase>::TileMapping              TileMappingType; \
    typedef typename CResourceBaseTestHelper<resourceBase>::BufferMapping            BufferMappingType;


// Specializations to using Texture2D resources
template <>
class CResourceBaseTestHelper < TRB_TEXTURE_2D > : public CTiledResourcesTest
{
public:
    // Tile sizes, etc
    static const UINT widthPerTile = 128;
    static const UINT heightPerTile = 128;
    static const UINT depthPerTile = 1;
    static const UINT totalNumTiles = 16;


    // Helper definitions
    typedef ID3D11Texture2D       ID3D11TextureType;
    typedef CD3D11_TEXTURE2D_DESC CD3D11_TEXTURE_TYPE_DESC;
    typedef CTileMapping          TileMapping;
    typedef CBufferMapping        BufferMapping;


    // Helper struct generators
    static void GenerateTileMapping(
        _Out_ TileMapping &tileMapping,
        const UINT width,
        const UINT height,
        const UINT depth,
        _In_ const UINT *mappingData)
    {
        tileMapping.Set(CTileMapping(width, height, mappingData));
    }

    static void SetTileMappingFromStrings(
        _Out_ TileMapping &tileMapping,
        _In_ const char *mappingStrings[4][4])
    {
        tileMapping.SetFromStrings(mappingStrings[0]);
    }

    static void GetTileMappingStrings(
        _In_ const TileMapping &tileMapping,
        _Out_ char *mappingStrings[4][4])
    {
        tileMapping.GetStrings(mappingStrings[0]);
    }

    static void GenerateRenderTargetViewDesc(
        _Out_ CD3D11_RENDER_TARGET_VIEW_DESC &desc,
        _In_ ID3D11TextureType *pTexture,
        D3D11_RTV_DIMENSION viewDimension,
        DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN,
        const UINT mipSlice = 0,
        const UINT firstArraySlice = 0,
        const UINT arraySize = -1)
    {
        desc = CD3D11_RENDER_TARGET_VIEW_DESC(pTexture, viewDimension, format, mipSlice, firstArraySlice, arraySize);
    }

    static void GenerateTextureDesc(
        _Out_ CD3D11_TEXTURE_TYPE_DESC &desc,
        DXGI_FORMAT format,
        const UINT width,
        const UINT height,
        const UINT arraySize = 1,
        const UINT mipLevels = 0,
        const UINT bindFlags = D3D11_BIND_SHADER_RESOURCE,
        const D3D11_USAGE usage = D3D11_USAGE_DEFAULT,
        const UINT cpuaccessFlags = 0,
        const UINT miscFlags = 0,
        const UINT sampleCount = 1,
        const UINT sampleQuality = 0)
    {
        desc = CD3D11_TEXTURE2D_DESC(format, width, height, arraySize, mipLevels, bindFlags, usage, cpuaccessFlags, sampleCount, sampleQuality, miscFlags);
    }

    static void GenerateUnorderedAccessViewDesc(
        _Out_ CD3D11_UNORDERED_ACCESS_VIEW_DESC &desc,
        _In_ ID3D11TextureType *pTexture,
        D3D11_UAV_DIMENSION viewDimension,
        DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN,
        const UINT mipSlice = 0,
        const UINT firstArraySlice = 0,
        const UINT arraySize = -1)
    {
        desc = CD3D11_UNORDERED_ACCESS_VIEW_DESC(pTexture, viewDimension, format, mipSlice, firstArraySlice, arraySize);
    }

    static void GenerateShaderResourceViewDesc(
        _Out_ CD3D11_SHADER_RESOURCE_VIEW_DESC &desc,
        _In_ ID3D11TextureType *pTexture,
        D3D11_SRV_DIMENSION viewDimension,
        DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN,
        const UINT mostDetailedMip = 0,
        const UINT mipLevels = -1,
        const UINT firstArraySlice = 0,
        const UINT arraySize = -1)
    {
        desc = CD3D11_SHADER_RESOURCE_VIEW_DESC(pTexture, viewDimension, format, mostDetailedMip, mipLevels, firstArraySlice, arraySize);
    }

    void GetTileShapeFromTextureDesc(
        _Out_ D3D11_TILE_SHAPE &shape,
        _In_ const CD3D11_TEXTURE_TYPE_DESC &textureDesc)
    {
        CTiledResourcesTest::GetTileSize(textureDesc.Format, shape.WidthInTexels, shape.HeightInTexels);
        shape.DepthInTexels = 1;

        // adjust for multi-sampling
        // If multisampling then modify the expected shape
        switch (textureDesc.SampleDesc.Count)
        {
        default:
            break;

        case 2:
            shape.WidthInTexels /= 2;
            break;

        case 4:
            shape.WidthInTexels /= 2;
            shape.HeightInTexels /= 2;
            break;

        case 8:
            shape.WidthInTexels /= 4;
            shape.HeightInTexels /= 2;
            break;

        case 16:
            shape.WidthInTexels /= 4;
            shape.HeightInTexels /= 4;
            break;
        }
    }


    // Helper functions, which interface to crrect functionality in CTiledResourcesTest
    TEST_RESULT FillBufferWithTilePattern(
        _Inout_ R8G8B8A8_PIXEL *pBuffer,
        const UINT tileWidth,
        const UINT tileHeight,
        const UINT tileDepth)
    {
        return CTiledResourcesTest::FillBufferWithTilePattern(pBuffer, tileWidth, tileHeight);
    }

    TEST_RESULT CreateStagingTexture(
        const UINT width,
        const UINT height,
        const UINT depth,
        _Out_ ID3D11TextureType **ppStagingResource,
        bool isSource)
    {
        return CTiledResourcesTest::CreateStagingTex2D(width, height, ppStagingResource, isSource);
    }

    TEST_RESULT UpdateTileMappings(
        _In_ ID3D11TextureType *pTiledResource,
        const UINT tilePoolBaseOffset,
        _In_ ID3D11Buffer *pTilePool,
        RESIDENCY_PATTERN_2D_3D pattern,
        _Out_ TileMapping &tileMapping,
        bool useNoOverwrite = true)
    {
        return CTiledResourcesTest::UpdateTileMappingsTex2D(pTiledResource, tilePoolBaseOffset, pTilePool, pattern, tileMapping, useNoOverwrite);
    }

    void ClearView(
        _In_ ID3D11View *pView,
        _In_ DXGI_FORMAT format,
        _In_ const float color[4],
        _In_reads_opt_(numRects) const D3D11_RECT *pRect,
        const UINT numRects
        )
    {
        CTiledResourcesTest::GetImmediateContext2()->ClearView(pView, color, pRect, numRects);
    }
};


// Specializations to using Texture3D resources
template <>
class CResourceBaseTestHelper < TRB_TEXTURE_3D > : public CTiledResourcesTest
{
public:
    // Tile sizes, etc
    static const UINT widthPerTile = 32;
    static const UINT heightPerTile = 32;
    static const UINT depthPerTile = 16;
    static const UINT totalNumTiles = 64;


    // Helper definitions
    typedef ID3D11Texture3D       ID3D11TextureType;
    typedef CD3D11_TEXTURE3D_DESC CD3D11_TEXTURE_TYPE_DESC;
    typedef CVolumeTileMapping    TileMapping;
    typedef CVolumeBufferMapping  BufferMapping;


    // Helper struct generators
    static void GenerateTileMapping(
        _Inout_ TileMapping &tileMapping,
        const UINT width,
        const UINT height,
        const UINT depth,
        _In_ const UINT *mappingData)
    {
        tileMapping.Set(CVolumeTileMapping(width, height, depth, mappingData));
    }

    static void SetTileMappingFromStrings(
        _Out_ TileMapping &tileMapping,
        _In_ const char *mappingStrings[4][4])
    {
        tileMapping.SetFromStrings(mappingStrings);
    }

    static void GetTileMappingStrings(
        _In_ const TileMapping &tileMapping,
        _Out_ char *mappingStrings[4][4])
    {
        tileMapping.GetStrings(mappingStrings);
    }

    static void GenerateRenderTargetViewDesc(
        _Out_ CD3D11_RENDER_TARGET_VIEW_DESC &desc,
        _In_ ID3D11TextureType *pTexture,
        _In_ D3D11_RTV_DIMENSION viewDimension,
        _In_ DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN,
        _In_ const UINT mipSlice = 0,
        _In_ const UINT firstWSlice = 0,
        _In_ const UINT wSize = -1)
    {
        desc = CD3D11_RENDER_TARGET_VIEW_DESC(pTexture, format, mipSlice, firstWSlice, wSize);
    }

    static void GenerateTextureDesc(
        _Out_ CD3D11_TEXTURE_TYPE_DESC &desc,
        DXGI_FORMAT format,
        const UINT width,
        const UINT height,
        const UINT depth,
        const UINT mipLevels = 0,
        const UINT bindFlags = D3D11_BIND_SHADER_RESOURCE,
        const D3D11_USAGE usage = D3D11_USAGE_DEFAULT,
        const UINT cpuaccessFlags = 0,
        const UINT miscFlags = 0,
        const UINT sampleCount = 1,
        const UINT sampleQuality = 0)
    {
        desc = CD3D11_TEXTURE3D_DESC(format, width, height, depth, mipLevels, bindFlags, usage, cpuaccessFlags, miscFlags);
    }

    static void GenerateUnorderedAccessViewDesc(
        _Out_ CD3D11_UNORDERED_ACCESS_VIEW_DESC &desc,
        _In_ ID3D11TextureType *pTexture,
        D3D11_UAV_DIMENSION viewDimension,
        DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN,
        const UINT mipSlice = 0,
        const UINT firstWSlice = 0,
        const UINT wSize = -1)
    {
        desc = CD3D11_UNORDERED_ACCESS_VIEW_DESC(pTexture, format, mipSlice, firstWSlice, wSize);
    }

    static void GenerateShaderResourceViewDesc(
        _Out_ CD3D11_SHADER_RESOURCE_VIEW_DESC &desc,
        _In_ ID3D11TextureType *pTexture,
        D3D11_SRV_DIMENSION viewDimension,
        DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN,
        const UINT mostDetailedMip = 0,
        const UINT mipLevels = -1,
        const UINT firstArraySlice = 0,
        const UINT arraySize = -1)
    {
        desc = CD3D11_SHADER_RESOURCE_VIEW_DESC(pTexture, format, mostDetailedMip, mipLevels);
    }

    void GetTileShapeFromTextureDesc(
        _Out_ D3D11_TILE_SHAPE &shape,
        _In_ const CD3D11_TEXTURE_TYPE_DESC &textureDesc)
    {
        CTiledResourcesTest::GetTileSize(textureDesc.Format, shape.WidthInTexels, shape.HeightInTexels, shape.DepthInTexels);
    }


    // Helper functions, which interface to crrect functionality in CTiledResourcesTest
    TEST_RESULT FillBufferWithTilePattern(
        _Inout_ R8G8B8A8_PIXEL *pBuffer,
        const UINT tileWidth,
        const UINT tileHeight,
        const UINT tileDepth)
    {
        return CTiledResourcesTest::FillBufferWithTilePattern(pBuffer, tileWidth, tileHeight, tileDepth);
    }

    TEST_RESULT CreateStagingTexture(
        const UINT width,
        const UINT height,
        const UINT depth,
        _Out_ ID3D11TextureType **ppStagingResource,
        bool isSource)
    {
        return CTiledResourcesTest::CreateStagingTex3D(width, height, depth, ppStagingResource, isSource);
    }

    TEST_RESULT UpdateTileMappings(
        _In_ ID3D11TextureType *pTiledResource,
        const UINT tilePoolBaseOffset,
        _In_ ID3D11Buffer *pTilePool,
        RESIDENCY_PATTERN_2D_3D pattern,
        _Out_ TileMapping &tileMapping,
        bool useNoOverwrite = true)
    {
        return CTiledResourcesTest::UpdateTileMappingsTex3D(pTiledResource, tilePoolBaseOffset, pTilePool, pattern, tileMapping, useNoOverwrite);
    }

    void ClearView(
        _In_ ID3D11View *pView,
        _In_ DXGI_FORMAT format,
        _In_ const float color[4],
        _In_reads_opt_(numBoxes) const D3D11_BOX *pBoxes,
        const UINT numBoxes,
        const UINT mip = 0
        )
    {
        CTiledResourcesTest::ClearViewTex3D(pView, format, color, pBoxes, numBoxes, mip);
    }
};


DECLARE_NAMED_VALUES(CTiledResourcesTest::RESIDENCY_PATTERN_1D)
DECLARE_NAMED_VALUES(CTiledResourcesTest::RESIDENCY_PATTERN_2D_3D)
DECLARE_NAMED_VALUES(CTiledResourcesTest::COPY_TILE_MAPPINGS_PATTERN)
DECLARE_NAMED_VALUES(CTiledResourcesTest::COPY_TILES_PATTERN)
DECLARE_NAMED_VALUES(CTiledResourcesTest::MINMAX_FILTER)
