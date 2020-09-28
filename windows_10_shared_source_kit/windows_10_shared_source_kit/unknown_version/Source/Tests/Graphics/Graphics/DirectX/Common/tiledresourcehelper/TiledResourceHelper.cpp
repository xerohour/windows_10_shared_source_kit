////////////////////////////////////////////////////////////////
// TiledResourceHelper.cpp
//

#include "TiledResourceHelper.h"
#include "CommonMacros.h"
#include "FormatHelper.h"
#include "StringConverters.hpp"
#include <atlbase.h>
#include <assert.h>


BEGIN_NAMED_VALUES(CTiledResourceHelper::TILED_RESOURCE_MODE)
	NAMED_VALUE( _T( "NEVER" ), CTiledResourceHelper::TILED_RESOURCE_MODE_NEVER )
	NAMED_VALUE( _T( "ALLOW" ), CTiledResourceHelper::TILED_RESOURCE_MODE_ALLOW )
END_NAMED_VALUES(CTiledResourceHelper::TILED_RESOURCE_MODE)

#if !defined(PASTER2)
#define PASTER2(x,y) x ## y
#endif

#if !defined(PASTER)
#define PASTER(x,y) PASTER2(x,y)
#endif

#if !defined(UNICODIFY)
#define UNICODIFY( x ) PASTER(L,x)
#endif

#if !defined(__FILEW__)
#define __FILEW__ UNICODIFY(__FILE__)
#endif

#define _LOG_ERROR(errorType, errorCode, errorMessage) \
    if(m_pLogger) \
    { \
	    m_pLogger->LogError(errorType, (UINT) errorCode, errorMessage, __FILEW__, __LINE__, __FUNCTION__); \
    }

#define _LOG_HRESULT_ERROR(errorCode, errorMessage) \
	_LOG_ERROR(ERROR_TYPE_HRESULT, (UINT) errorCode, errorMessage)

#define _LOG_BOOL_ERROR(errorCode, errorMessage) \
	_LOG_ERROR(ERROR_TYPE_BOOL, (UINT) errorCode, errorMessage)

#if !defined(SAFE_RELEASE)
#define SAFE_RELEASE(ptr) {if(ptr){ptr->Release(); ptr = nullptr;}}
#endif

#if !defined(SAFE_DELETE)
#define SAFE_DELETE(ptr) {if(ptr){delete(ptr); ptr = nullptr;}}
#endif

#if !defined(SAFE_DELETE_ARRAY)
#define SAFE_DELETE_ARRAY(ptr) {if(ptr){delete[](ptr); ptr = nullptr;}}
#endif

CTiledResourceHelper::CTiledResourceHelper() :
    m_pTilePoolManager(nullptr),
    m_mode(TILED_RESOURCE_MODE_ALLOW),
    m_pDevice(nullptr),
    m_wasLastResourceTiled(false),
    m_pLogger(nullptr)
{
}

CTiledResourceHelper::~CTiledResourceHelper()
{
    Cleanup();
}

HRESULT CTiledResourceHelper::Setup(_In_ ID3D11Device2 *pDevice, ISimpleLog *pLogger)
{
    HRESULT hr = S_OK;

    // Guard against calling Setup() twice
    assert(nullptr == m_pDevice);

    // Make sure we start in a clean state
    Cleanup();

    m_pLogger = pLogger;
    if(m_pLogger)
    {
        m_pLogger->AddRef();
    }

	//
	// Create the tile pool manager
	//
	if(pDevice)
	{
		D3D11_FEATURE_DATA_D3D11_OPTIONS1 featureData;
		hr = pDevice->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS1, &featureData, sizeof(featureData));
        if(FAILED(hr))
        {
        
            _LOG_HRESULT_ERROR(hr, L"CheckFeatureSupport failed");
            return hr;
        }

		if(featureData.TiledResourcesTier != D3D11_TILED_RESOURCES_NOT_SUPPORTED)
		{
			m_pTilePoolManager = new CTilePoolManager(m_pLogger);
			m_pTilePoolManager->Create(pDevice);
		}
	}

    m_pDevice = pDevice;
    m_pDevice->AddRef();

    return hr;
}

void CTiledResourceHelper::InitOptionVariables(CPropertyManager *pPropertyManager)
{
    CSettingProperty<TILED_RESOURCE_MODE> *pModeProperty = new CSettingProperty<TILED_RESOURCE_MODE>(_T("Tiled"), &m_mode, nullptr);
    pPropertyManager->RegisterProperty(pModeProperty);
    pModeProperty->SetAttribute(_T("Description"), _T("ALLOW - Allow test to substitue tiled resources, NEVER - Never substitute tiled resources"));
    pModeProperty->SetAttribute(_T("Category"), _T("D3D"));
}

void CTiledResourceHelper::Cleanup()
{
    if(m_pDevice)
    {
	    // Cleanup any tiled resources no longer in use
	    if(m_pTilePoolManager)
	    {
		    m_pTilePoolManager->CleanupResources();
		    SAFE_DELETE(m_pTilePoolManager);
	    }

        m_pDevice->Release();
        m_pDevice = nullptr;
    }

    SAFE_RELEASE(m_pLogger);
}

HRESULT CTiledResourceHelper::Start()
{
    HRESULT hr = S_OK;

	if(m_pTilePoolManager)
	{
		hr = m_pTilePoolManager->ShrinkTilePoolIfPossible();
	}

    return hr;
}

void CTiledResourceHelper::Finish()
{
	// Cleanup any tiled resources no longer in use
	if(m_pTilePoolManager)
    {
		m_pTilePoolManager->CleanupResources();
    }
}


const UINT DefaultSampleCount = 1; // No AA.

HRESULT CTiledResourceHelper::GetTileSize(DXGI_FORMAT format, _Out_ UINT &tileWidth, _Out_ UINT &tileHeight)
{
    HRESULT hr = S_OK;
	UINT elementSize = CFormatHelper::ElementBits(format);

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
            assert(0);  // Format dimensions cannot be tiled
			hr = E_INVALIDARG;
    		break;
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

    return hr;
}

template<typename T>
bool Tileable(_In_ ID3D11Device2* pDevice, _In_ const T* pDesc, DXGI_FORMAT Format, UINT SampleCount, UINT ExtraBindFlagsToDisallow = 0, UINT ExtraMiscFlagsToDisallow = 0, UINT ArraySize = 1, UINT Width = 0, UINT Height = 0, UINT MipLevels = 1)
{
	// Without ExtraBindFlagsToDisallow or ExtraMiscFlagsToDisallow, this will check all flags that are incompatible with all tiled resources
	const UINT IncompatibleBindFlags = D3D11_BIND_CONSTANT_BUFFER | D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_INDEX_BUFFER | D3D11_BIND_STREAM_OUTPUT |
									   D3D11_BIND_DECODER | D3D11_BIND_VIDEO_ENCODER | ExtraBindFlagsToDisallow;
	const UINT IncompatibleMiscFlags = D3D11_RESOURCE_MISC_SHARED | D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX | D3D11_RESOURCE_MISC_GDI_COMPATIBLE | D3D11_RESOURCE_MISC_SHARED_NTHANDLE |
									   D3D11_RESOURCE_MISC_RESTRICTED_CONTENT | D3D11_RESOURCE_MISC_RESTRICT_SHARED_RESOURCE | D3D11_RESOURCE_MISC_RESTRICT_SHARED_RESOURCE_DRIVER |
									   D3D11_RESOURCE_MISC_GUARDED | D3D11_RESOURCE_MISC_TILE_POOL | ExtraMiscFlagsToDisallow;

	// Check to see if the device supports tiled resources
	D3D11_FEATURE_DATA_D3D11_OPTIONS1 featureData;
	HRESULT hr = pDevice->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS1, &featureData, sizeof(featureData));

	D3D11_FEATURE_DATA_FORMAT_SUPPORT2 formatSupportData = {Format, D3D11_FORMAT_SUPPORT2_TILED};
	UINT NumQualityLevels = D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT;
	if (SUCCEEDED(hr) && Format != DXGI_FORMAT_UNKNOWN)
	{
		// If a format is specified, make sure it supports tiled resources
		hr = pDevice->CheckFeatureSupport(D3D11_FEATURE_FORMAT_SUPPORT2, &formatSupportData, sizeof(formatSupportData)); 
		if (SUCCEEDED(hr) && SampleCount > DefaultSampleCount)
		{
			// If a non-default sample count is specified, make sure it supports tiled resources.
			hr = pDevice->CheckMultisampleQualityLevels1(Format, SampleCount, D3D11_CHECK_MULTISAMPLE_QUALITY_LEVELS_TILED_RESOURCE, &NumQualityLevels);
		}

		if(SUCCEEDED(hr) && (formatSupportData.OutFormatSupport2 & D3D11_FORMAT_SUPPORT2_TILED) && ArraySize > 1)
		{
			UINT leastDetailedMipWidth = 1;
			UINT leastDetailedMipHeight = 1;

			if(MipLevels > 0)
			{
				leastDetailedMipWidth = Width;
				leastDetailedMipHeight = Height;

				CFormatHelper::GetMipDimensions(MipLevels - 1, &leastDetailedMipWidth, &leastDetailedMipHeight);
			}

			UINT TileWidth = 0;
			UINT TileHeight = 0;
			
			hr = CTiledResourceHelper::GetTileSize(Format, TileWidth, TileHeight);
			
            if(SUCCEEDED(hr))
            {
			    if(D3D11_TILED_RESOURCES_TIER_2 & featureData.TiledResourcesTier)
			    {
				    // On tier-2 arrays of tiled resources with packed mips are only supported
				    // if the least detailed mip is larger than 1/4 the width and height of a
				    // tile in each dimension
				    if(leastDetailedMipWidth < TileWidth || leastDetailedMipHeight < TileHeight)
				    {
					    return false;
				    }
			    }
			    else if(D3D11_TILED_RESOURCES_TIER_1 & featureData.TiledResourcesTier)
			    {
				    // On Tier-1 array textures with packed mips cannot be tiled.
				    // Also on tier-1 all mips must be multiples of tile size in order to not 
				    // be considered packed.
				    if(
					    static_cast<float>(leastDetailedMipWidth / TileWidth) != static_cast<float>(leastDetailedMipWidth) / static_cast<float>(TileWidth) ||
					    static_cast<float>(leastDetailedMipHeight / TileHeight) != static_cast<float>(leastDetailedMipHeight) / static_cast<float>(TileHeight)
					    )
				    {
					    return false;
				    }
			    }
            }
		}
	}

	return SUCCEEDED(hr) && (featureData.TiledResourcesTier != D3D11_TILED_RESOURCES_NOT_SUPPORTED) && (formatSupportData.OutFormatSupport2 & D3D11_FORMAT_SUPPORT2_TILED) &&
		   (pDesc->Usage == D3D11_USAGE_DEFAULT) && !(pDesc->BindFlags & IncompatibleBindFlags) && !(pDesc->MiscFlags & IncompatibleMiscFlags) && NumQualityLevels;
}

// Is the texture2d desc compatible with tiled resources?
bool TileableTexture2D(_In_ ID3D11Device2* pDevice, _In_ const D3D11_TEXTURE2D_DESC* pDesc)
{
	return Tileable(pDevice, pDesc, pDesc->Format, pDesc->SampleDesc.Count, 0, 0, pDesc->ArraySize, pDesc->Width, pDesc->Height, pDesc->MipLevels); 
}

// Is the buffer desc compatible with tiled resources?
bool TileableBuffer(_In_ ID3D11Device2* pDevice, _In_ const D3D11_BUFFER_DESC* pDesc, const DXGI_FORMAT ViewFormat)
{
	const UINT UnsupportedViewBpp = 96;
	const UINT IncompatibleBindFlags = D3D11_BIND_DEPTH_STENCIL; // Depth stencils specifically incompatible with tiled buffers
	if (pDesc->Usage == D3D11_USAGE_DEFAULT && pDesc->CPUAccessFlags != 0) 
	{ 
		// This is a 'Map Default' scenario, which isn't compatible with tiled resources
		return false; 
	}
	return (CFormatHelper::ElementBits(ViewFormat) != UnsupportedViewBpp) && Tileable(pDevice, pDesc, DXGI_FORMAT_UNKNOWN, DefaultSampleCount, IncompatibleBindFlags);
}

HRESULT CTiledResourceHelper::CreateTexture2DTiledOptional(_In_ const D3D11_TEXTURE2D_DESC *pDesc, _In_ const D3D11_SUBRESOURCE_DATA *pInitialData, _Out_ ID3D11Texture2D **ppTexture2D)
{
	HRESULT hr = S_OK;
	ID3D11Texture2D *pTexture2D = nullptr;
    CComPtr<ID3D11DeviceContext2> pContext = nullptr;
	const TILED_RESOURCE_MODE TilingMode = GetMode();
	const bool useTiled = (TilingMode != TILED_RESOURCE_MODE_NEVER) && m_pTilePoolManager && TileableTexture2D(m_pDevice, pDesc);

    // Cache the immediate context
    m_pDevice->GetImmediateContext2(&pContext);

	// Default range buffers
	const UINT defaultNumRanges = 16;
	static UINT defaultRangeOffsetBuffer[defaultNumRanges];
	static UINT defaultRangeCountBuffer[defaultNumRanges];

	// Pointers to range buffers (may be changed if larger buffers are needed)
	UINT numRanges = defaultNumRanges;
	UINT *pRangeOffsets = defaultRangeOffsetBuffer;
	UINT *pRangeCounts = defaultRangeCountBuffer;

	if(useTiled)
	{
        m_wasLastResourceTiled = true;

		D3D11_TEXTURE2D_DESC newDesc;
		CopyMemory(&newDesc, pDesc, sizeof(D3D11_TEXTURE2D_DESC));

		newDesc.MiscFlags |= D3D11_RESOURCE_MISC_TILED;
		hr = m_pDevice->CreateTexture2D(&newDesc, NULL, &pTexture2D);

		if(hr != S_OK)
			goto end;

		// Get the fully resolved desc (to determine the number of mips)
		pTexture2D->GetDesc(&newDesc);

		// Determine the resource tiling configuration
		UINT numTilesNeeded;
		D3D11_PACKED_MIP_DESC packedMipDesc;
		D3D11_TILE_SHAPE tileShapeNonPacked;
		D3D11_SUBRESOURCE_TILING subresourceTilingNonPacked;
		m_pDevice->GetResourceTiling(pTexture2D, &numTilesNeeded, NULL, NULL, NULL, 0, &subresourceTilingNonPacked);

		// Get the necessary number of tiles from the tile pool
		UINT count = m_pTilePoolManager->FetchTiles(pTexture2D, numTilesNeeded, pRangeOffsets, pRangeCounts, numRanges);
		if(count > numRanges)
		{
			numRanges = count;
			pRangeOffsets = new UINT[count];
			pRangeCounts = new UINT[count];
			count = m_pTilePoolManager->FetchTiles(pTexture2D, numTilesNeeded, pRangeOffsets, pRangeCounts, numRanges);
		}

		if(count == 0)
		{
            _LOG_BOOL_ERROR(FALSE, L"Unable to fetch the required number of tiles from the managed tile pool");
			hr = E_FAIL;
			goto end;
		}

		numRanges = count;

		// Update tile mapping for the entire resource list
		D3D11_TILED_RESOURCE_COORDINATE startCoord;
		startCoord.X = 0;
		startCoord.Y = 0;
		startCoord.Z = 0;
		startCoord.Subresource = 0;
		D3D11_TILE_REGION_SIZE regionSize;
		regionSize.bUseBox = false;
		regionSize.NumTiles = numTilesNeeded;
		hr = pContext->UpdateTileMappings(
			pTexture2D, 
			1,
			&startCoord, 
			&regionSize, 
			m_pTilePoolManager->GetTilePool(),
			numRanges, 
			NULL, 
			pRangeOffsets, 
			pRangeCounts, 
			D3D11_TILE_MAPPING_NO_OVERWRITE
			);

		if(hr != S_OK)
			goto end;

		if(pInitialData)
		{
			// Create a staging resource to store the initialization data
			ID3D11Texture2D *pStagingTexture = NULL;
			newDesc.BindFlags = 0;
			newDesc.Usage = D3D11_USAGE_DEFAULT;
			newDesc.MiscFlags = 0;

			hr = m_pDevice->CreateTexture2D(&newDesc, pInitialData, &pStagingTexture);

			if(hr == S_OK)
			{
				// Copy the initialization data to the tiled resource
				pContext->CopyResource(pTexture2D, pStagingTexture);
			}

			SAFE_RELEASE(pStagingTexture);
		}

	}
	else
	{
        // Let the caller decide what to do when a tiled resource won't be created:
        m_wasLastResourceTiled = false;
        hr = E_FAIL;
	}

end:
	if(pRangeOffsets != defaultRangeOffsetBuffer)
	{
		SAFE_DELETE_ARRAY(pRangeOffsets);
		SAFE_DELETE_ARRAY(pRangeCounts);
	}

	*ppTexture2D = pTexture2D;

	return hr;
}


HRESULT CTiledResourceHelper::CreateBufferTiledOptional(_In_ const D3D11_BUFFER_DESC *pDesc, _In_ const D3D11_SUBRESOURCE_DATA *pInitialData, _Out_ ID3D11Buffer **ppBuffer, DXGI_FORMAT ViewFormat)
{
    CComPtr<ID3D11DeviceContext2> pContext = nullptr;
	const TILED_RESOURCE_MODE TilingMode = GetMode();
	const bool bTileable = (TilingMode != TILED_RESOURCE_MODE_NEVER) && m_pTilePoolManager && TileableBuffer(m_pDevice, pDesc, ViewFormat);
    m_wasLastResourceTiled = false;

    // Cache the immediate context
    m_pDevice->GetImmediateContext2(&pContext);

	// Default range buffers
	const UINT defaultNumRanges = 16;
	static UINT defaultRangeOffsetBuffer[defaultNumRanges];
	static UINT defaultRangeCountBuffer[defaultNumRanges];

	// Pointers to range buffers (may be changed if larger buffers are needed)
	UINT numRanges = defaultNumRanges;
	UINT *pRangeOffsets = defaultRangeOffsetBuffer;
	UINT *pRangeCounts = defaultRangeCountBuffer;

	if (!bTileable)
	{
		return m_pDevice->CreateBuffer(pDesc, pInitialData, ppBuffer);
	}

	assert(bTileable && (TilingMode == TILED_RESOURCE_MODE_ALLOW));

    m_wasLastResourceTiled = true;

	D3D11_BUFFER_DESC TileableDesc = *pDesc;
	TileableDesc.MiscFlags |= D3D11_RESOURCE_MISC_TILED;

	CComPtr<ID3D11Buffer> spBuffer;
    HRESULT hr = m_pDevice->CreateBuffer(&TileableDesc, nullptr, &spBuffer);
	if (SUCCEEDED(hr))
	{
		// Determine how many tiles are required.
		UINT numTilesNeeded;
		m_pDevice->GetResourceTiling(spBuffer, &numTilesNeeded, nullptr, nullptr, nullptr, 0, nullptr);

		// Get the necessary number of tiles from the tile pool
		UINT count = m_pTilePoolManager->FetchTiles(spBuffer, numTilesNeeded, pRangeOffsets, pRangeCounts, numRanges);
		if(count > numRanges)
		{
			numRanges = count;
			pRangeOffsets = new UINT[count];
			pRangeCounts = new UINT[count];
			count = m_pTilePoolManager->FetchTiles(spBuffer, numTilesNeeded, pRangeOffsets, pRangeCounts, numRanges);
		}

		if(count == 0)
		{
			_LOG_BOOL_ERROR(FALSE, L"Unable to fetch the required number of tiles from the managed tile pool");
			hr = E_FAIL;
			goto end;
		}

		numRanges = count;

		// Map all tiles
		const D3D11_TILED_RESOURCE_COORDINATE startCoord = {};
		const D3D11_TILE_REGION_SIZE regionSize = {numTilesNeeded};

		hr = pContext->UpdateTileMappings(spBuffer, 1, &startCoord, &regionSize, m_pTilePoolManager->GetTilePool(),
														numRanges, NULL, pRangeOffsets, pRangeCounts, D3D11_TILE_MAPPING_NO_OVERWRITE);
		if (SUCCEEDED(hr) && pInitialData)
		{
			// Create a staging resource to store the initialization data
			D3D11_BUFFER_DESC& StagingDesc = TileableDesc;
			StagingDesc.BindFlags = 0;
			StagingDesc.MiscFlags = 0;

            CComPtr<ID3D11Buffer> spStaging;
			hr = m_pDevice->CreateBuffer(&StagingDesc, pInitialData, &spStaging);
			if (SUCCEEDED(hr))
			{
                pContext->CopyResource(spBuffer, spStaging);
			}
		}
	}

end:
	*ppBuffer = SUCCEEDED(hr) ? spBuffer.Detach() : nullptr;

	if(pRangeOffsets != defaultRangeOffsetBuffer)
	{
		SAFE_DELETE_ARRAY(pRangeOffsets);
		SAFE_DELETE_ARRAY(pRangeCounts);
	}

	return hr;
}


//////////////////////////////////////////////////////////////////////////
//
// CTilePoolManager
//
//////////////////////////////////////////////////////////////////////////
#define TILE_SIZE_IN_BYTES (65536)

//////////////////////////////////////////////////////////////////////////
// CTilePoolManager::CTilePoolManager()
//////////////////////////////////////////////////////////////////////////
CTiledResourceHelper::CTilePoolManager::CTilePoolManager(ISimpleLog *pLogger) :
	m_poolSizeInTiles(0),
	m_pTilePool(nullptr),
    m_pDevice(nullptr),
    m_pLogger(pLogger)
{
    if(nullptr != pLogger)
    {
        pLogger->AddRef();
    }
}

//////////////////////////////////////////////////////////////////////////
// CTilePoolManager::~CTilePoolManager()
//////////////////////////////////////////////////////////////////////////
CTiledResourceHelper::CTilePoolManager::~CTilePoolManager()
{
	// Ensure that there are no more allocated ranges
	if(m_pTilePool)
	{
		assert(m_allocatedRanges.size() == 0);
		assert(m_freeRangeList.size() == 1);
	}

	// Release the tile pool resource
	SAFE_RELEASE(m_pTilePool);
    SAFE_RELEASE(m_pLogger);
    SAFE_RELEASE(m_pDevice);
}

bool CTiledResourceHelper::CTilePoolManager::RangeIntersectsList(const CTilePoolManager::TILE_RANGE &range, RangeListType &rangeList)
{
	for(RangeListType::iterator it = rangeList.begin(); it != rangeList.end(); ++it)
	{
		if(range.start < (*it).start && range.start + range.size > (*it).start)
			return true;

		if(range.start >= (*it).start && range.start < (*it).start + (*it).size)
			return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
// CTilePoolManager::FetchRange()
//////////////////////////////////////////////////////////////////////////
UINT CTiledResourceHelper::CTilePoolManager::FetchRange(ID3D11Resource *pResource, UINT sizeInTiles)
{
    HRESULT hr = S_OK;
	TILE_RANGE range = {0,0};
    CComPtr<ID3D11DeviceContext2> pContext2;

    m_pDevice->GetImmediateContext2(&pContext2);

	assert(m_pTilePool);

	// Iterate through the range list and find a range that will fit sizeInTiles
	for(RangeListType::iterator it = m_freeRangeList.begin(); it != m_freeRangeList.end(); ++it)
	{
		if(it->size >= sizeInTiles)
		{
			range.size = sizeInTiles;
			range.start = it->start;

			if(sizeInTiles < it->size)
			{
				// Decrease the available size of the selected range
				it->start += sizeInTiles;
				it->size -= sizeInTiles;
			}
			else
			{
				// Remove the range if the entire range was allocated
				m_freeRangeList.erase(it);
			}

			// Exit the search loop
			break;
		}
	}

	if(range.size == sizeInTiles)
	{
		assert(false == RangeIntersectsList(range, m_freeRangeList));

		// Find the other ranges allocated for this resource
		RangeListType &rangeList = FindRangeList(pResource);
		InsertRange(rangeList, &range);
	}
	else
	{
		// No range found.
		// Double the tile pool size and try again
		TILE_RANGE newRange;
		newRange.size = m_poolSizeInTiles;
		newRange.start = m_poolSizeInTiles;

		m_poolSizeInTiles *= 2;
		assert(m_poolSizeInTiles > 0);
		assert(m_freeRangeList.size() + m_allocatedRanges.size() > 0);
		HRESULT hr = pContext2->ResizeTilePool(m_pTilePool, TILE_SIZE_IN_BYTES * m_poolSizeInTiles);

		if( hr != S_OK)
			throw std::exception("Out of Memory");

		// Append the range to the end of the free range list
		InsertRange(m_freeRangeList, &newRange);

		return FetchRange(pResource, sizeInTiles);
	}

	return range.start;
}

UINT CTiledResourceHelper::CTilePoolManager::FetchTiles(
    _In_ ID3D11Resource *pResource,
    UINT numTiles,
    _Inout_updates_(bufferSizesInTiles) UINT *pOffsets,
    _Inout_updates_(bufferSizesInTiles) UINT *pRangeSizes,
    UINT bufferSizesInTiles
    )
{
    HRESULT hr = S_OK;
    CComPtr<ID3D11DeviceContext2> pContext2;
	UINT numRangesNeeded = 1; // At least one range is needed
	UINT numTilesLeft = numTiles;

    assert(pResource);

    // Get the ID3D11DeviceContext2 immediate context pointer
    m_pDevice->GetImmediateContext2(&pContext2);

	// Interate through the available ranges and determine the number of ranges required to for the requested tile count
	for(RangeListType::iterator it = m_freeRangeList.begin(); it != m_freeRangeList.end(); ++it)
	{
		// Break out of this loop if the remaining tiles fit in this range
        if (it->size >= numTilesLeft)
        {
            numTilesLeft = 0;
            break;
        }

		numTilesLeft -= it->size;
		numRangesNeeded += 1;
	}

	// If the number of ranges needed exceeds to buffer sizes...
	if(numRangesNeeded >= bufferSizesInTiles || NULL == pRangeSizes || NULL == pOffsets)
	{
		// ...exit now
		return numRangesNeeded;
	}

	// If the tile pool is too small then grow the tile pool to the smallest power of 2
	// large enough for all the tiles
	if(numTilesLeft > 0)
	{
		// If newPoolSize overflows, we can get infinitely stuck in the for loop below
		assert(m_poolSizeInTiles < UINT_MAX / 2);

		UINT newPoolSize = m_poolSizeInTiles * 2;

		for(;;)
		{
			if(numTilesLeft < newPoolSize - m_poolSizeInTiles)
				break;

			newPoolSize *= 2;
		}

		// Grow the tile pool
		TILE_RANGE newRange;
		newRange.size = newPoolSize - m_poolSizeInTiles;
		newRange.start = m_poolSizeInTiles;

		HRESULT hr = pContext2->ResizeTilePool(m_pTilePool, TILE_SIZE_IN_BYTES * newPoolSize);
		if(FAILED(hr))
		{
			_LOG_HRESULT_ERROR(
                hr, 
                std::wstring(L"Unexpected failure resizing tile pool. Original size: "
                + ToStringW(m_poolSizeInTiles)
                + L", Requested size: "
                + ToStringW(newPoolSize)
                ).c_str()
                );

			return 0;
		}

		// Append the range to the end of the free range list
		InsertRange(m_freeRangeList, &newRange);

		m_poolSizeInTiles = newPoolSize;
	}

	// Reset the range counter
	numRangesNeeded = 0;

	// Now iterate through the ranges and actually allocate them
	// Interate through the available ranges and determine the number of ranges required to for the requested tile count
	numTilesLeft = numTiles;
	RangeListType &rangeList = FindRangeList(pResource);

	for(RangeListType::iterator it = m_freeRangeList.begin(); numTilesLeft != 0 && it != m_freeRangeList.end(); )
	{
		TILE_RANGE range = *it;

		// Break out of this loop if the remaining tiles fit in this range
		if(it->size > numTilesLeft)
		{
			// Set the allocated range size to the remaining tile count
			range.size = numTilesLeft;

			// Trim the free range
			it->size -= numTilesLeft;
			it->start += numTilesLeft;
		}
		else
		{
			// Remove this range from the free list
			it = m_freeRangeList.erase(it);
		}

		// Insert the entire range into the resource's allocated range list
		InsertRange(rangeList, &range);
								
		// Update the output buffers
		pOffsets[numRangesNeeded] = range.start;
		pRangeSizes[numRangesNeeded] = range.size;
		numRangesNeeded++;
		numTilesLeft -= range.size;
	}

	return numRangesNeeded;
}

//////////////////////////////////////////////////////////////////////////
// CTiledResourceHelper::CTilePoolManager::ReleaseRanges()
//////////////////////////////////////////////////////////////////////////
void CTiledResourceHelper::CTilePoolManager::ReleaseRanges(ID3D11Resource *pResource)
{
	AllocatedRangeMapType::iterator it = m_allocatedRanges.find(pResource);
	if(it != m_allocatedRanges.end())
	{
		CTilePoolManager::ReleaseRanges(it);
	}
	assert(m_poolSizeInTiles > 0);
	assert(m_freeRangeList.size() + m_allocatedRanges.size() > 0);
}

//////////////////////////////////////////////////////////////////////////
// CTiledResourceHelper::CTilePoolManager::ReleaseRanges()
//////////////////////////////////////////////////////////////////////////
CTiledResourceHelper::CTilePoolManager::AllocatedRangeMapType::iterator CTiledResourceHelper::CTilePoolManager::ReleaseRanges(AllocatedRangeMapType::iterator it)
{
	RangeListType &rangeList = it->second;

	assert(rangeList.size() > 0);

	// found list of ranges associated with the resource
	for(RangeListType::iterator rlit = rangeList.begin(); rlit != rangeList.end(); ++rlit)
	{
		TILE_RANGE range = *rlit;

		// Add the range back to the free list
		InsertRange(m_freeRangeList, &range);
	}

	// Remove this element from the allocated range map
	return m_allocatedRanges.erase(it);
}

//////////////////////////////////////////////////////////////////////////
// CTiledResourceHelper::CTilePoolManager::CleanupResources()
//////////////////////////////////////////////////////////////////////////
void CTiledResourceHelper::CTilePoolManager::CleanupResources()
{
	assert(m_poolSizeInTiles > 0);
	assert(m_freeRangeList.size() + m_allocatedRanges.size() > 0);
	for(AllocatedRangeMapType::iterator it = m_allocatedRanges.begin(); it != m_allocatedRanges.end();)
	{
		ID3D11Resource *pResource = it->first;

		if(pResource && pResource->AddRef() == 2)
		{
			// The resource is no longer in use so free up tile pool ranges
			it = ReleaseRanges(it);

			// Clear tile mappings if tier 1
			ClearMappingsIfTier1(pResource);

			// Final release the resource
			pResource->Release();
		}
		else
		{
			++it;
		}

		if(pResource)
			pResource->Release();
	}
	assert(m_poolSizeInTiles > 0);
	assert(m_freeRangeList.size() + m_allocatedRanges.size() > 0);
}

//////////////////////////////////////////////////////////////////////////
// CTiledResourceHelper::CTilePoolManager::Create()
//////////////////////////////////////////////////////////////////////////
HRESULT CTiledResourceHelper::CTilePoolManager::Create(ID3D11Device2 *pDevice)
{
	HRESULT hr = S_OK;
	// Initial tile pool size is 16 tiles
	m_poolSizeInTiles = 16;

    // Guard against calling Setup() twice
    assert(nullptr == m_pDevice);

	D3D11_BUFFER_DESC tilePoolDesc;
	tilePoolDesc.BindFlags = 0;
	tilePoolDesc.ByteWidth = TILE_SIZE_IN_BYTES * m_poolSizeInTiles;
	tilePoolDesc.CPUAccessFlags = 0;
	tilePoolDesc.MiscFlags = D3D11_RESOURCE_MISC_TILE_POOL;
	tilePoolDesc.Usage = D3D11_USAGE_DEFAULT;

	hr = pDevice->CreateBuffer(&tilePoolDesc, NULL, &m_pTilePool);
	if(FAILED(hr))
	{
		_LOG_HRESULT_ERROR(hr, L"CreateBuffer() failed creating tile pool");
	}

	// Initialize the free range list
	TILE_RANGE initRange;
	initRange.size = m_poolSizeInTiles;
	initRange.start = 0;
	InsertRange(m_freeRangeList, &initRange);

	assert(m_poolSizeInTiles > 0);
	assert(m_freeRangeList.size() + m_allocatedRanges.size() > 0);

    m_pDevice = pDevice;
    m_pDevice->AddRef();

	return hr;
}

HRESULT CTiledResourceHelper::CTilePoolManager::ShrinkTilePoolIfPossible()
{
    HRESULT hr = S_OK;
    CComPtr<ID3D11DeviceContext2> pContext2;

    // Get the ID3D11DeviceContext2 immediate context pointer
    m_pDevice->GetImmediateContext2(&pContext2);

	// Find the upper-bound of the allocated ranges
	CTilePoolManager::RangeListType::reverse_iterator rit = m_freeRangeList.rbegin();

	if(rit->start + rit->size == m_poolSizeInTiles)
	{
		UINT newSize = m_poolSizeInTiles;
		for(UINT halfSize = newSize >> 1; newSize > 16 && rit->start < (newSize >> 1); halfSize >>= 1)
		{
			newSize = halfSize;
		}

		if(newSize < m_poolSizeInTiles)
		{
			HRESULT hr = pContext2->ResizeTilePool(m_pTilePool, TILE_SIZE_IN_BYTES * newSize);
			if(FAILED(hr))
			{
			    _LOG_HRESULT_ERROR(
                    hr, 
                    std::wstring(L"Unexpected failure resizing tile pool. Original size: "
                    + ToStringW(m_poolSizeInTiles)
                    + L", Requested size: "
                    + ToStringW(newSize)
                    ).c_str()
                    );

                return hr;
			}

			rit->size -= m_poolSizeInTiles - newSize;
			m_poolSizeInTiles = newSize;
		}
	}

	return hr;
}

//////////////////////////////////////////////////////////////////////////
// CTiledResourceHelper::CTilePoolManager::InsertRange()
//////////////////////////////////////////////////////////////////////////
void CTiledResourceHelper::CTilePoolManager::InsertRange(RangeListType &rangeList, TILE_RANGE *pRange)
{
	if(rangeList.empty())
	{
		// Range list is empty so add a new range
		rangeList.push_back(*pRange);
		return;
	}

	RangeListType::iterator it;
	RangeListType::iterator itLast = rangeList.end();

	for(it = rangeList.begin(); it != rangeList.end();)
	{
		assert(pRange->start != (*it).start);

		if(pRange->start < (*it).start)
		{
			// Insert before the current range
			if(pRange->start + pRange->size < (*it).start)
			{
				if(itLast != rangeList.end() && pRange->start == (*itLast).start + (*itLast).size)
				{
					// Append to the previous range
					(*itLast).size += pRange->size;
				}
				else
				{
					// Add a new range
					assert(pRange->start + pRange->size < (*it).start);
					rangeList.insert(it, *pRange);
				}
			}
			else
			{
				assert(pRange->start + pRange->size == (*it).start);

				if(itLast != rangeList.end() && pRange->start == (*itLast).start + (*itLast).size)
				{
					// Merge all three 
					pRange->start = (*itLast).start;
					pRange->size += (*itLast).size + (*it).size;
					rangeList.insert(it, *pRange);

					// Erase old ranges
					it = rangeList.erase(it);
					it = rangeList.erase(itLast);

				}
				else
				{
					// Prepend the range
					(*it).start = pRange->start;
					(*it).size += pRange->size;
				}
			}

			assert(rangeList.size() > 0);
			return;
		}

		itLast = it++;
	}

	if(it == rangeList.end())
	{
		assert(itLast != rangeList.end());

		// Append the range to the end of the free range list
		if((*itLast).start + (*itLast).size == pRange->start)
		{
			// Append the range to this one
			(*itLast).size += pRange->size;
		}
		else
		{
			// Add the range to the end of the list
			rangeList.push_back(*pRange);
			assert(pRange->start > (*itLast).start + (*itLast).size);
		}
	}

	assert(rangeList.size() > 0);
}

//////////////////////////////////////////////////////////////////////////
// CTiledResourceHelper::CTilePoolManager::FindRangeList()
//////////////////////////////////////////////////////////////////////////
CTiledResourceHelper::CTilePoolManager::RangeListType &CTiledResourceHelper::CTilePoolManager::FindRangeList(ID3D11Resource *pResource)
{
	// Get the rangeList for the associated resource
	AllocatedRangeMapType::iterator it = m_allocatedRanges.find(pResource);
	if(it == m_allocatedRanges.end())
	{
		// First range allocation so add it to the list
		RangeListType emptyRangeList;
		AllocatedRangeMapType::value_type value(pResource, emptyRangeList);
		it = m_allocatedRanges.insert(value).first;

		// Hold a ref count on the resource for cleanup later
		if(pResource)
			pResource->AddRef();
	}

	return it->second;
}

void CTiledResourceHelper::CTilePoolManager::ClearMappingsIfTier1(_In_ ID3D11Resource *pResource)
{
	HRESULT hr = S_OK;

	// Determine the tiled resource tier
	D3D11_FEATURE_DATA_D3D11_OPTIONS1 featureData;
	hr = m_pDevice->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS1, &featureData, sizeof(featureData));
    if(FAILED(hr))
	{
        RaiseException(
            STATUS_INVALID_PARAMETER, 
            0,      // execute the exception handler
            0, 
            NULL);
	}
	if(featureData.TiledResourcesTier == D3D11_TILED_RESOURCES_TIER_1)
	{
		CComPtr<ID3D11DeviceContext2> pContext;
		m_pDevice->GetImmediateContext2(&pContext);
		D3D11_TILED_RESOURCE_COORDINATE startCoords;
		startCoords.Subresource = 0;
		startCoords.X = 0;
		startCoords.Y = 0;
		startCoords.Z = 0;
		D3D11_TILE_REGION_SIZE regionSize;
		regionSize.bUseBox = false;
		// Clear the tile mappings on the buffer
		m_pDevice->GetResourceTiling(
			pResource, 
			&regionSize.NumTiles,
			nullptr,
			nullptr,
			nullptr,
			0,
			nullptr
			);
		UINT rangeFlags = D3D11_TILE_RANGE_NULL; // NULL out all tile mappings
		UINT startOffsets = 0;
		UINT rangeCounts = regionSize.NumTiles;
		hr = pContext->UpdateTileMappings(
			pResource, 
			1, 
			&startCoords, 
			&regionSize, 
			m_pTilePool, 
			1, 
			&rangeFlags, 
			&startOffsets, 
			&rangeCounts, 
			0
			);
		if(FAILED(hr))
		{
			throw(std::exception("UpdateTileMappings unexpectedly failed during while clearing tile mappings"));
			return;
		}
	}
}

