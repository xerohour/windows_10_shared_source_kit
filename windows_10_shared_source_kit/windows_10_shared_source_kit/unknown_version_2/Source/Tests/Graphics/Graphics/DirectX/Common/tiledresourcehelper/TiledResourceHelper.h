////////////////////////////////////////////////////////////////
// TiledResourceHelper.h

#pragma once

#include <d3d11_2.h>
#include <list>
#include <map>
#include "stringutils.h"
#include "SettingProperties.h"
#include "ISimpleLog.h"

class CTiledResourceHelper
{
public:
	////////////////////////////////////////////////////////////////////////////////
	// CTilePoolManager
	//
	// The tile pool manager is used when the tiled resource option is enabled.
	// 
	////////////////////////////////////////////////////////////////////////////////
	class CTilePoolManager
	{
		// Buffer for tile pool
		ID3D11Buffer *m_pTilePool;
        ID3D11Device2 *m_pDevice;
		UINT m_poolSizeInTiles;

		struct TILE_RANGE
		{
			UINT start;
			UINT size;
		};

		// The range list will be iterated through to find the first chunk
		// of tiles of the size requested.
		// Note: this is not the most efficient method of managing memory but
		// for the purposes of tests it will suffice.
		typedef std::list<TILE_RANGE> RangeListType;
		RangeListType m_freeRangeList;

		// When a tiled resource allocates a range the relationship is recorded here.
		// When it is determined that a tiled resource is no longer needed
		// this information is used to free the ranges associated with it.
		typedef std::map<ID3D11Resource *, RangeListType> AllocatedRangeMapType;
		AllocatedRangeMapType m_allocatedRanges;

	public:
		CTilePoolManager(ISimpleLog *pLogger);
		virtual ~CTilePoolManager();

		// Fetch a range of tiles of the given size and return the tile offset in the tile pool.
		// If there are no ranges of the requested size avaialble then grow the tile pool.
		// FetchRange() may throw an out-of-memory exception if no ranges can be found or allocated.
		UINT FetchRange(_In_ ID3D11Resource *pResource, UINT sizeInTiles);

		// Fetches ranges based on the number of tiles requested. Fragmented tile pools may result
		// in range counts larger than one.  The total number of tiles
		// fetched will be equal to the number of tiles requested as long as the number of ranges
		// required do not exceel the size of the output buffers.
		// Returns the number of ranges used/needed.
		// If the buffers are not large enough the method will return the number of ranges needed and leave the output buffers unmodified.
		// Returns 0 if there was an error
		UINT FetchTiles(_In_ ID3D11Resource *pResource, UINT numTiles, _Inout_updates_(bufferSizesInTiles) UINT *pOffsets, _Inout_updates_(bufferSizesInTiles) UINT *pRangeSizes, UINT bufferSizesInTiles);

		// When a tiled resource has a refcount of 1 it is assumed that the tile ranges
		// associated are no longer needed.  This method frees up the ranges and 
		// calls a final release on the tiled resource.
		// This method should be called by the framework during test case cleanup and test group cleanup.
		// CleanupResources() may reduce the size of the tile pool if the high-water mark drops to less-than-half
		// the size of the tile pool
		void CleanupResources();

		// Release all ranges associated with the given resource
		void ReleaseRanges(ID3D11Resource *pResource);

		HRESULT Create(_In_ ID3D11Device2 *pDevice);
		HRESULT ShrinkTilePoolIfPossible();	
		ID3D11Buffer *GetTilePool() { return m_pTilePool; }

	private:
		// Release all ranges associated with the given resource
		AllocatedRangeMapType::iterator ReleaseRanges(AllocatedRangeMapType::iterator it);

		// Finds the range list for the specified resource.
		// If the specified resource doesnt yet have a range list this method creates an empty
		// range list and associates it with the resource and adds a reference count to the resource.
		RangeListType &FindRangeList(_In_ ID3D11Resource *pResource);

		static void InsertRange(RangeListType &rangeList, _In_ TILE_RANGE *pRange);
		static bool RangeIntersectsList(_In_ const CTilePoolManager::TILE_RANGE &range, _Inout_ RangeListType &rangeList);

		void ClearMappingsIfTier1(_In_ ID3D11Resource *pResource);

        ISimpleLog *m_pLogger;
	};

	////////////////////////////////////////////////////////////////////////////////
    //
    // Enums
    //
	////////////////////////////////////////////////////////////////////////////////

public:
    enum TILED_RESOURCE_MODE
    {
        TILED_RESOURCE_MODE_NEVER,
        TILED_RESOURCE_MODE_ALLOW,
    };

public:

	CTiledResourceHelper();
	~CTiledResourceHelper();

	////////////////////////////////////////////////////////////////////////////////
	//
	// Methods
	//
	////////////////////////////////////////////////////////////////////////////////

	HRESULT CreateTilePoolForTiledOptional();
	HRESULT CreateTexture2DTiledOptional(_In_ const D3D11_TEXTURE2D_DESC *pDesc, _In_ const D3D11_SUBRESOURCE_DATA *pInitialData, _Out_ ID3D11Texture2D **ppTexture2D);
	HRESULT CreateBufferTiledOptional(_In_ const D3D11_BUFFER_DESC *pDesc, _In_ const D3D11_SUBRESOURCE_DATA *pInitialData, _Out_ ID3D11Buffer **ppBuffer, DXGI_FORMAT ViewFormat = DXGI_FORMAT_UNKNOWN);

    bool WasLastResourceTiled() const { return m_wasLastResourceTiled; }

    HRESULT Setup(_In_ ID3D11Device2 *pDevice, _In_ ISimpleLog *pLogger);
    void Cleanup();

    void InitOptionVariables(CPropertyManager *pPropertyManager);

    HRESULT Start();
    void Finish();

	////////////////////////////////////////////////////////////////////////////////
	//
	// Accessors
	//
	////////////////////////////////////////////////////////////////////////////////
public:

	CTilePoolManager *GetTilePoolManager() { return m_pTilePoolManager; }
    TILED_RESOURCE_MODE GetMode() const { return m_mode; }

	////////////////////////////////////////////////////////////////////////////////
	//
	// Static Methods
	//
	////////////////////////////////////////////////////////////////////////////////
public:
    static HRESULT CTiledResourceHelper::GetTileSize(DXGI_FORMAT format, _Out_ UINT &tileWidth, _Out_ UINT &tileHeight);

	////////////////////////////////////////////////////////////////////////////////
	//
	// Data
	//
	////////////////////////////////////////////////////////////////////////////////
private:

	CTilePoolManager *m_pTilePoolManager;
    TILED_RESOURCE_MODE m_mode;
    ID3D11Device2 *m_pDevice;
    bool m_wasLastResourceTiled;
    ISimpleLog *m_pLogger;

	////////////////////////////////////////////////////////////////////////////////
	//
	// Member classes
	//
	////////////////////////////////////////////////////////////////////////////////
public:

};

DECLARE_NAMED_VALUES(CTiledResourceHelper::TILED_RESOURCE_MODE);