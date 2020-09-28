//////////////////////////////////////////////////////////////////////////
//  D3D11Test.h
//  created:	2004/10/07
//
//  purpose: Base framework classes for wrapped D3D test classes.
//////////////////////////////////////////////////////////////////////////

#pragma once

//
// Win32 headers
//

#include <windows.h>
#include <detours.h>

// Direct3D/DXGI headers
#include <d3d11_2.h>
#include <d3d11.h>
#include <d3d10_1.h>
#include <d3d10.h>
#include <dxgi1_3.h>

#define INITGUID
#include <guiddef.h>
#include <DXGIDebug.h>
#undef INITGUID


//
// Test infrastructure headers
//

#include "d3d11internaltest.h" // Internal interfaces for ID3D11DeviceTest

#include <WGFTestCore.h>
#include <DXGIValueSets.h>
#include <D3D11ValueSets.h>
#include <NResult.h>
#include <ResourceViewer.h>

#include <new_off.h>
#include <d3dx10.h>
#include <new_on.h>

#include <GraphicsTest.h>
#include <HResultHelp.h>


//
// C runtime headers
//

#include <assert.h>


// Driver verifier enums
#define D3D11_CREATE_DEVICE_DRIVER_VERIFIER     ( 0x20000000 )
#define D3D11_MESSAGE_CATEGORY_DRIVER_VERIFIER  ((D3D11_MESSAGE_CATEGORY) 11 )

#define	D3D11_BUFFEROFFSET_APPEND               ( 0xffffffffL )

enum D3DTEST_TILED_RESOURCE_MODE
{
	D3DTEST_TILED_RESOURCE_MODE_NEVER = 0,  // Never try to use tiled resources
	D3DTEST_TILED_RESOURCE_MODE_ONLY,       // CreateTexture2DTiledOptional returns E_NOTIMPL when tiled resource not created
	D3DTEST_TILED_RESOURCE_MODE_ALLOW,      // Allow CreateTexture2DTiledOptional to successfully create a tiled resouce if supported
};

DECLARE_NAMED_VALUES(D3DTEST_TILED_RESOURCE_MODE);


struct D3D_OPTIONS : BASE_D3D_OPTIONS
{
	D3D_FEATURE_LEVEL wMinFeatureLevel; 
	D3D_FEATURE_LEVEL wMaxFeatureLevel; 
	bool SrcOnly;
	D3D_DRIVER_TYPE RefDriverType;

	union
	{
		D3D_DRIVER_TYPE DriverType;
		D3D_DRIVER_TYPE SrcDriverType;
	};

	D3D_FEATURE_LEVEL FeatureLevel;
	D3D_FEATURE_LEVEL origFeatureLevel;
	D3DTEST_INTERFACE_LEVEL origInterfaceLevel;
	D3DTEST_TILED_RESOURCE_MODE tiledResourceMode;
	D3D11_CREATE_DEVICE_FLAG CreateDeviceFlags;
	bool useD3DShim;
	bool bgraSupport;
};


struct D3D_RENDER_TARGET_OPTIONS
{
	DXGI_SWAP_CHAIN_DESC1 SwapChainDesc;
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC FullscreenDesc;
	HWND OutputWindow;
	BOOL Windowed;

	D3D11_RENDER_TARGET_VIEW_DESC RenderTargetViewDesc;
	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;

	bool UseDefaultRenderTargetView;
	bool UseDefaultDepthStencilView;
	bool NoRenderTargets;
	bool NoDepthStencils;
	bool NoSwapChains;

	union
	{
		D3D11_TEXTURE1D_DESC DepthTextureDesc1D;
		D3D11_TEXTURE2D_DESC DepthTextureDesc;
	};

	union
	{
		D3D11_BUFFER_DESC RenderTargetBufferDesc;
		D3D11_TEXTURE1D_DESC RenderTargetTextureDesc1D;
		D3D11_TEXTURE2D_DESC RenderTargetTextureDesc;
	};
};

enum D3D_SHADER_STAGE
{
	D3D_SHADER_STAGE_VERTEX,
	D3D_SHADER_STAGE_GEOMETRY,
	D3D_SHADER_STAGE_PIXEL,
	D3D_SHADER_STAGE_HULL,
	D3D_SHADER_STAGE_DOMAIN,
	D3D_SHADER_STAGE_COMPUTE
};
DECLARE_NAMED_VALUES( D3D_SHADER_STAGE )


//////////////////////////////////////////////////////////////////////////
// CD3D11TestApp
// D3DDisassemble and D3DReflect entry points.  (Windows build doesn't have these.)
//////////////////////////////////////////////////////////////////////////
HRESULT WINAPI LL_D3DDisassemble(
	LPCVOID pSrcData,
	SIZE_T SrcDataSize,
	UINT Flags,
	LPCSTR szComments,
	ID3D10Blob** ppDisassembly);

HRESULT WINAPI LL_D3DReflect(
	LPCVOID pSrcData,
	SIZE_T SrcDataSize,
	REFIID pInterface,
	void** ppReflector);


// Entry point D3DGetInputAndOutputSignatureBlob
HRESULT WINAPI LL_D3DGetInputAndOutputSignatureBlob(
	LPCVOID pSrcData,
	SIZE_T SrcDataSize,
	ID3DBlob** ppSignatureBlob);


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
	CTilePoolManager();
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
	TEST_RESULT ShrinkTilePoolIfPossible();	
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
};

//////////////////////////////////////////////////////////////////////////
// CD3D11TestApp
//
// Base class for D3D testing.  
// NOTE: Requires all test classes be derived from CD3D11Test.
//////////////////////////////////////////////////////////////////////////
class CD3D11TestFramework : public CGraphicsTestFramework
{
public:
	CD3D11TestFramework();
	virtual ~CD3D11TestFramework();

	virtual bool Setup();
	virtual void Cleanup();

	virtual void CleanupTest();

	virtual bool UpdateCurrentDeviceInfo(DXGLOG_DEVICEINFO * pDeviceInfo);

	virtual TEST_RESULT PreSetupTestCase();


private:
	TEST_RESULT SetupDXGI();
	TEST_RESULT SetupD3D();

	TEST_RESULT QueryAdapter();


public:
	// Determines the device creation method to use based on the DriverType and Adapter settings.
	// Also respects the valid flags for both DXGI and D3D when determening what needs to be done.
	virtual TEST_RESULT SetupDXGIAndD3D();
	void CleanupDXGI();
	virtual void CleanupD3D();
    virtual TEST_RESULT CheckForDriverErrors();

	virtual TEST_RESULT SetupRenderTargets();
	virtual void CleanupRenderTargets();

	virtual bool Present(RECT *pSrc, RECT *pDest, void *pSourceDirtyRegion, void *pDestClipRegion, UINT SyncInterval, UINT dwFlags);

	virtual void InitOptionVariables();

	void InvalidateD3D() { m_bD3DValid = false; }
	void InvalidateDXGI() { m_bDXGIValid = false; }
	void InvalidateRenderTargets();

	NRESULT CompileShaderFromResource( HMODULE hModule, LPCTSTR pResource, LPCSTR pFunctionName, D3D_SHADER_STAGE ShaderStage, D3D_FEATURE_LEVEL MinFeatureLevel, D3D_FEATURE_LEVEL MaxFeatureLevel, UINT Flags, ID3D10Blob **ppShader, ID3D10Blob **ppErrorMessages, CONST D3D10_SHADER_MACRO *pDefines = NULL, ID3D10Blob **ppShaderText = NULL );
	NRESULT CompileShaderFromFile( LPCTSTR pFile, LPCSTR pFunctionName, D3D_SHADER_STAGE ShaderStage, D3D_FEATURE_LEVEL MinFeatureLevel, D3D_FEATURE_LEVEL MaxFeatureLevel, UINT Flags, ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs, CONST D3D10_SHADER_MACRO *pDefines = NULL, ID3D10Blob **ppShaderText = NULL );
	NRESULT CompileShaderFromMemory( LPCSTR pSrcData, SIZE_T SrcDataLen, LPCSTR pFunctionName, D3D_SHADER_STAGE ShaderStage, D3D_FEATURE_LEVEL MinFeatureLevel, D3D_FEATURE_LEVEL MaxFeatureLevel, UINT Flags, ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs, CONST D3D10_SHADER_MACRO *pDefines = NULL, ID3D10Blob **ppShaderText = NULL );

	// Accessors
	IDXGIFactory *GetDXGIFactory()			{ return m_DXGIFactory.pFactory; }
	IDXGIFactory1 *GetDXGIFactory1()			{ return m_DXGIFactory.pFactory1; }
	IDXGIFactory2 *GetDXGIFactory2()			{ return m_DXGIFactory.pFactory2; }
	IDXGIAdapter **GetDXGIAdapters()			{ return m_pAdapters; }
	IDXGIAdapter *GetDXGIAdapter(UINT uDev)	{ if (m_pAdapters && m_uDeviceCount > uDev) return m_pAdapters[ uDev ]; else return NULL; }
	IDXGIOutput *GetDXGIOutput(UINT uDev)	{ if (m_pAdapterOutputs && m_uDeviceCount > uDev) return m_pAdapterOutputs[ uDev ]; else return NULL; }

	IDXGISwapChain1 **GetDXGISwapChains(UINT uRT=0)		{ if (m_pSwapChains) return m_pSwapChains + uRT*m_uDeviceCount; else return NULL; }
	IDXGISwapChain1 *GetSwapChain(UINT uRT, UINT uDev)	{ if (m_pSwapChains) return m_pSwapChains[ uRT*m_uDeviceCount+uDev ]; else return NULL; }

	ID3D11Device *GetDevice()	{ return m_pD3DDevice; }
	ID3D11Device1 *GetDevice1()	{ return m_pD3DDevice1; }
	ID3D11Device2 *GetDevice2()	{ return m_pD3DDevice2; }
	ID3D11DeviceTest *GetTestDevice() { return m_pTestDevice; };
	ID3D11DeviceContext *GetImmediateContext()	{ return m_pD3DDeviceImmediateContext; }
	ID3D11DeviceContext1 *GetImmediateContext1()	{ return m_pD3DDeviceImmediateContext1; }
	ID3D11DeviceContext2 *GetImmediateContext2()	{ return m_pD3DDeviceImmediateContext2; }
	ID3D11DeviceContext *GetEffectiveContext()	{ return m_bDeferredContextUsed ? m_pD3DDeviceDeferredContext : m_pD3DDeviceImmediateContext; }
	ID3D11DeviceContext1 *GetEffectiveContext1()	{ return m_bDeferredContextUsed ? m_pD3DDeviceDeferredContext1 : m_pD3DDeviceImmediateContext1; }
	ID3D11DeviceContext2 *GetEffectiveContext2()	{ return m_bDeferredContextUsed ? m_pD3DDeviceDeferredContext2 : m_pD3DDeviceImmediateContext2; }
	bool IsDeferredContextUsed() const { return m_bDeferredContextUsed; }
	D3DTEST_TILED_RESOURCE_MODE GetTiledResourceMode() const { return m_D3DOptions.tiledResourceMode; }
	CTilePoolManager *GetTilePoolManager() { return m_pTilePoolManager; }
	HRESULT ExecuteEffectiveContext();

	IDXGIDebug *GetDXGIDebug() { return m_pDXGIDebug; }

	IDXGIInfoQueue *GetDXGIInfoQueue() { return m_pDXGIInfoQueue; }

	ID3D11Debug *GetDebug()	{ return m_pD3DDebug; }

	ID3D11InfoQueue *GetInfoQueue()	{ return m_pD3DInfoQueue; }

	// BUGBUG - chrisza: Switch to ref not implemented in M1
	//ID3D11SwitchToRef *GetSwitchToRef()	{ return m_pD3DSwitchToRef; }

	ID3D11Buffer *GetRTBuffer(UINT uRT=0)	{ if (m_pRTBuffers) return m_pRTBuffers[ uRT ]; else return NULL; }

	ID3D11Texture1D *GetRTTexture1D(UINT uRT=0)	{ if (m_pRTTextures1D) return m_pRTTextures1D[ uRT ]; else return NULL; }

	ID3D11Texture2D *GetRTTexture(UINT uRT=0)	{ if (m_pRTTextures) return m_pRTTextures[ uRT ]; else return NULL; }

	ID3D11RenderTargetView *GetRTView(UINT uRT=0)	{ if (m_pRTViews) return m_pRTViews[ uRT ]; else return NULL; }

	ID3D11Texture1D *GetDSTexture1D(UINT uRT=0) { return m_pDepthStencil1D; }

	ID3D11Texture2D *GetDSTexture(UINT uRT=0) { return m_pDepthStencil; }

	ID3D11DepthStencilView *GetDSView() { return m_pDSView; }

	CResourceViewer &GetResourceViewer() { return m_resourceViewer; }
	CResourceViewerWindow *ShowResource(
		_In_ PCWSTR name,
		_In_ ID3D11Resource *pResource, 
		UINT mipLevel = 0,
		UINT arraySlice = 0,
		UINT depthSlice = 0,
		float resizeScale = 1.0f,
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN
		)
	{
		CResourceViewerWindow *pWindow = nullptr;

		if(m_viewResources)
		{
			pWindow = m_resourceViewer.ShowResource(name, pResource, mipLevel, arraySlice, depthSlice, resizeScale, format);
		}

		return pWindow;
	}


	// BUGBUG - chrisza: GoldenImage uses D3DX10SaveTextureToMemory which requires a D3D10 resource
	//NRESULT GoldenImage( ID3D11Resource* pResource, LPCWSTR pFile );

	HRESULT CreateDevice( IDXGIAdapter **ppAdapter, D3D_DRIVER_TYPE DriverType, UINT uFlags, D3D_FEATURE_LEVEL FeatureLevel, ID3D11Device **ppDevice, ID3D11DeviceContext **ppDeviceContext );
	
	// Wraps call to appropriate D3D<n>CreateDevice method depending on the value of m_D3DOptions.origInterfaceLevel
	HRESULT LocalD3D11CreateDevice(
		IDXGIAdapter* pAdapter,
		D3D_DRIVER_TYPE DriverType,
		HMODULE Software,
		UINT Flags,
		CONST D3D_FEATURE_LEVEL* pFeatureLevels,
		UINT FeatureLevels,
		UINT SDKVersion,
		ID3D11Device** ppDevice,
		D3D_FEATURE_LEVEL* pFeatureLevel,
		ID3D11DeviceContext** ppImmediateContext );

	bool IsSDKTest() const { return m_bSDKTest; }
	bool Is11_1DDISupported( ID3D11Device *pDevice ) const;

	static const char * GetShaderProfile(D3D_SHADER_STAGE ShaderStage, D3D_FEATURE_LEVEL MinFeatureLevel, D3D_FEATURE_LEVEL MaxFeatureLevel, const D3D_OPTIONS &D3DOptions);
	HRESULT GetDataWithTimeout( ID3D11DeviceContext* pDeviceContext, ID3D11Asynchronous* pAsync, void* pData, UINT dataSize, UINT dataFlags );

	HRESULT CreateTilePoolForTiledOptional();
	HRESULT CreateTexture2DTiledOptional(_In_ const D3D11_TEXTURE2D_DESC *pDesc, _In_ const D3D11_SUBRESOURCE_DATA *pInitialData, _Out_ ID3D11Texture2D **ppTexture2D);
	HRESULT CreateBufferTiledOptional(_In_ const D3D11_BUFFER_DESC *pDesc, _In_ const D3D11_SUBRESOURCE_DATA *pInitialData, _Out_ ID3D11Buffer **ppBuffer, DXGI_FORMAT ViewFormat = DXGI_FORMAT_UNKNOWN);

	void InitOutputParameters(_In_ class CD3D11Test *pTest);

protected:
	void SetRuntimeVersion();

	virtual TEST_RESULT SetupTest();
	virtual void PreGenerateTestCase();
	virtual void PostCleanupTestCase();

	// It's the test application's resposibility to implement SetupProxyDriver if it uses proxy driver
	virtual HRESULT SetupProxyDriver(HMODULE *hProxyDirver) {return E_FAIL; }

	virtual void PreAllocateTest();


private:
	// Helper functions.
	bool UpdateGraphicsValidFlags();
	HRESULT CreateDevice11( IDXGIAdapter **ppAdapter, D3D_DRIVER_TYPE DriverType, UINT uFlags, D3D_FEATURE_LEVEL FeatureLevel, ID3D11Device **ppDevice, ID3D11DeviceContext **ppDeviceContext );
	TEST_RESULT RefreshGraphics();
	void ReportLiveDXGIObjects();


public:	
	// Flags to indicate what the last call to RefreshGraphics has actually done
	enum REFRESH_GRAPHICS_FLAGS
	{
		DXGI_RECYCLED	=                        	(1 << 0),
		D3D_RECYCLED	=                        	(1 << 1),
		RT_RECYCLED		=                        	(1 << 2),
	};

	UINT m_RefreshGraphicsFlags;

	D3D_OPTIONS m_D3DOptions;
	D3D_RENDER_TARGET_OPTIONS m_RenderTargetOptions;
	bool m_bForceFeatureLevel;
	UINT m_nQueryTimeout;

private:
	enum ThreadCommand
	{
		TC_None,
		TC_CreateDevice,
		TC_EnterLeaveLoop,
		TC_Exit,
	};
	static TEST_RESULT s_ThreadResult;
	static volatile ThreadCommand s_ThreadCommand;
	DWORD			dwMainThread;
	HANDLE			hMainWaiting;
	HANDLE			hWorkerWaiting;


protected:
	UINT					m_uDeviceCount;
	UINT					m_uSwapChainCount;
	DXGI_FACTORY_POINTER	m_DXGIFactory;
	IDXGIAdapter			**m_pAdapters;
	IDXGIOutput				**m_pAdapterOutputs;
	IDXGISwapChain1			**m_pSwapChains;		
	ID3D11Device			*m_pD3DDevice;
	ID3D11DeviceContext		*m_pD3DDeviceImmediateContext;
	ID3D11DeviceContext		*m_pD3DDeviceDeferredContext;
	ID3D11Device1			*m_pD3DDevice1;
	ID3D11DeviceContext1	*m_pD3DDeviceImmediateContext1;
	ID3D11DeviceContext1	*m_pD3DDeviceDeferredContext1;
	ID3D11Device2			*m_pD3DDevice2;
	ID3D11DeviceContext2	*m_pD3DDeviceImmediateContext2;
	ID3D11DeviceContext2	*m_pD3DDeviceDeferredContext2;
	IDXGIDebug				*m_pDXGIDebug;
	IDXGIInfoQueue			*m_pDXGIInfoQueue;
	ID3D11Debug				*m_pD3DDebug;
	ID3D11InfoQueue			*m_pD3DInfoQueue;
	ID3D11DeviceTest		*m_pTestDevice;
	// BUGBUG - chrisza: Switch to ref not implemented in M1
	//ID3D11SwitchToRef		*m_pD3DSwitchToRef;

	ID3D11Texture2D			**m_pRTTextures;
	ID3D11Texture1D			**m_pRTTextures1D;
	ID3D11Buffer			**m_pRTBuffers;

	ID3D11RenderTargetView	**m_pRTViews;

	ID3D11Texture2D			*m_pDepthStencil;
	ID3D11Texture1D			*m_pDepthStencil1D;

	ID3D11DepthStencilView	*m_pDSView;
	CONST void				*m_pExtensions;

	bool				m_bDeferredContextUsed;

	CResourceViewer			m_resourceViewer;
	bool					m_viewResources;
	bool					m_bEmulateR8WithL8;

private:
	bool				m_bD3DValid;
	bool				m_bRenderTargetValid;
	bool				m_bDXGIValid;
	bool				m_bSetupCalled;
	bool				m_bSDKTest;

	// Global cached device options
	DXGI_OPTIONS m_DXGIOptionsGlobal;
	D3D_OPTIONS m_D3DOptionsGlobal;
	D3D_RENDER_TARGET_OPTIONS m_RenderTargetOptionsGlobal;

	// Test Group wide cached device options
	DXGI_OPTIONS m_DXGIOptionsGroup;
	D3D_OPTIONS m_D3DOptionsGroup;
	D3D_RENDER_TARGET_OPTIONS m_RenderTargetOptionsGroup;

	CTilePoolManager *m_pTilePoolManager;

	CVariationParameter<bool> *m_pOutputParamTiledResourceUsed;
};


//////////////////////////////////////////////////////////////////////////
// CD3D11Test
//
// Base class for tests using the Wrapped D3D Interfaces.
//////////////////////////////////////////////////////////////////////////
class CD3D11Test : public CTest
{
public:
	CD3D11Test();

	TEST_RESULT Setup() { return RESULT_PASS; }
    TEST_RESULT PostExecuteTestCase() { return GetFramework()->CheckForDriverErrors(); }
	void Cleanup();

	virtual int GetNumRenderWindows() const { return 0; }
	virtual bool FullScreen() const { return false; }

	virtual void InitOutputParameters();

	CD3D11TestFramework *GetFramework() const { return dynamic_cast< CD3D11TestFramework *>( ::GetFramework() ); }
	HRESULT CreateTexture2DTiledOptional(_In_ D3D11_TEXTURE2D_DESC *pDesc, _In_ const D3D11_SUBRESOURCE_DATA *pInitialData, _Out_ ID3D11Texture2D **ppTexture2D)
	{ return GetFramework()->CreateTexture2DTiledOptional(pDesc, pInitialData, ppTexture2D); }

	IDXGIFactory *GetDXGIFactory()					{ return GetFramework()->GetDXGIFactory(); }
	IDXGIAdapter **GetDXGIAdapters()				{ return GetFramework()->GetDXGIAdapters(); }
	IDXGISwapChain1 **GetDXGISwapChains(UINT uRT=0)	{ return GetFramework()->GetDXGISwapChains( uRT ); }

	ID3D11Device *GetDevice()						{ return GetFramework()->GetDevice(); }
	ID3D11DeviceContext *GetImmediateContext()		{ return GetFramework()->GetImmediateContext(); }
	ID3D11DeviceContext *GetEffectiveContext()		{ return GetFramework()->GetEffectiveContext(); }
	ID3D11Device1 *GetDevice1()						{ return GetFramework()->GetDevice1(); }
	ID3D11Device2 *GetDevice2()						{ return GetFramework()->GetDevice2(); }
	ID3D11DeviceContext1 *GetImmediateContext1()	{ return GetFramework()->GetImmediateContext1(); }
	ID3D11DeviceContext2 *GetImmediateContext2()	{ return GetFramework()->GetImmediateContext2(); }
	ID3D11DeviceContext1 *GetEffectiveContext1()	{ return GetFramework()->GetEffectiveContext1(); }
	ID3D11DeviceContext2 *GetEffectiveContext2()	{ return GetFramework()->GetEffectiveContext2(); }
	HRESULT ExecuteEffectiveContext()				{ return GetFramework()->ExecuteEffectiveContext(); }
	IDXGIDebug *GetDXGIDebug()						{ return GetFramework()->GetDXGIDebug(); }
	ID3D11Debug *GetDebug()							{ return GetFramework()->GetDebug(); }
	
	ID3D11Texture2D *GetRTTexture(UINT uRT=0)		{ return GetFramework()->GetRTTexture( uRT ); }
	ID3D11RenderTargetView *GetRTView(UINT uRT=0)	{ return GetFramework()->GetRTView( uRT ); }
	ID3D11Texture2D *GetDSTexture()					{ return GetFramework()->GetDSTexture(); }
	ID3D11DepthStencilView *GetDSView()				{ return GetFramework()->GetDSView(); }

    // BlockingFinish()
    // Forces GPU to flush all commands up to and including the most recent
	bool BlockingFinish();

	CResourceViewer &GetResourceViewer()			{ return GetFramework()->GetResourceViewer(); }
	CResourceViewerWindow *ShowResource(
		_In_ const LPCWSTR &name,
		_In_ ID3D11Resource *pResource, 
		UINT mipLevel = 0,
		UINT arraySlice = 0,
		UINT depthSlice = 0,
		float resizeScale = 1.0f,
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN
		)
	{
		return GetFramework()->ShowResource(name, pResource, mipLevel, arraySlice, depthSlice, resizeScale, format);
	}


	bool Present(RECT *pSrc, RECT *pDest, void *pSourceDirtyRegion, void *pDestClipRegion, UINT SyncInterval, UINT dwFlags)
	{ return GetFramework()->Present(pSrc, pDest, pSourceDirtyRegion, pDestClipRegion, SyncInterval, dwFlags); }
};

template <typename T> class MapHelper : public D3D11_MAPPED_SUBRESOURCE
{
	public:
		MapHelper() :	pCtx(nullptr),
						pRes(nullptr),
						SubResIdx(0),
						hr(S_FALSE)
		{
		}

		MapHelper(ID3D11DeviceContext2* Context, T* Resource, UINT SubresourceIndex, D3D11_MAP Flags) : pCtx(Context),
																										pRes(Resource),
																										SubResIdx(SubresourceIndex)
		{
			hr = pCtx->Map(pRes, SubResIdx, Flags, 0, this);
		}

		~MapHelper()
		{
			Unmap();
		}

		HRESULT Map()
		{
			assert(nullptr != pRes);
			return hr;
		}

		HRESULT Map(ID3D11DeviceContext2* Context, T* Resource, UINT SubresourceIndex, D3D11_MAP Flags)
		{
			// Unmap just to be safe
			Unmap();

			pCtx = Context;
			pRes = Resource;
			SubResIdx = SubresourceIndex;
			hr = pCtx->Map(pRes, SubResIdx, Flags, 0, this);
			return hr;
		}

		void Unmap()
		{
			if(S_OK == hr)
			{
				pCtx->Unmap(pRes, SubResIdx);
				pRes = nullptr;
				hr = S_FALSE;
				pCtx = nullptr;
			}
		}

	private:
		MapHelper(const MapHelper&);
		MapHelper& operator=(const MapHelper&);
		HRESULT hr;
		ID3D11DeviceContext2* pCtx;
		T* pRes;
		UINT SubResIdx;
};