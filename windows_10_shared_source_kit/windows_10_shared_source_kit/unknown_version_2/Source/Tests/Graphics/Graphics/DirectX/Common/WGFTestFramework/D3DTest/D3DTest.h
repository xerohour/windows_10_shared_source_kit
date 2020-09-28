//////////////////////////////////////////////////////////////////////////
//  D3DTest.h
//  created:	2004/10/07
//
//  purpose: Base framework classes for wrapped D3D test classes.
//////////////////////////////////////////////////////////////////////////

#pragma once


//
// Test infrastructure headers
//

#include <WGFTestCore.h>
#include <D3DMultiWrapped.h>
#include <D3DValueSets.h>
#include <D3DEnumsInfo.h>
#include <DXGIValueSets.h>
#include <GraphicsTest.h>
#include <HresultHelp.h>


//
// C runtime headers
//

#include <assert.h>


struct SHADER_MACRO
{
	LPCSTR Name;
	LPCSTR Description;
};


// Mirror defines for D3D<x>_SHADER_FLAGS
enum D3D_SHADER_FLAGS
{
	D3D_SHADER_DEBUG =                        	(1 << 0),
	D3D_SHADER_SKIP_VALIDATION =              	(1 << 1),
	D3D_SHADER_SKIP_OPTIMIZATION =            	(1 << 2),
	D3D_SHADER_PACK_MATRIX_ROW_MAJOR =        	(1 << 3),
	D3D_SHADER_PACK_MATRIX_COLUMN_MAJOR =     	(1 << 4),
	D3D_SHADER_PARTIAL_PRECISION =            	(1 << 5),
	D3D_SHADER_FORCE_VS_SOFTWARE_NO_OPT =     	(1 << 6),
	D3D_SHADER_FORCE_PS_SOFTWARE_NO_OPT =     	(1 << 7),
	D3D_SHADER_NO_PRESHADER =                 	(1 << 8),
	D3D_SHADER_AVOID_FLOW_CONTROL =           	(1 << 9),
	D3D_SHADER_PREFER_FLOW_CONTROL =          	(1 << 10),
	D3D_SHADER_ENABLE_STRICTNESS =            	(1 << 11),
	D3D_SHADER_ENABLE_BACKWARDS_COMPATIBILITY =	(1 << 12),
};

#define	D3D_BUFFEROFFSET_APPEND	( 0xffffffffL )


struct D3D_OPTIONS : BASE_D3D_OPTIONS
{
	// Version is represented in hex with major version number in 
	// high-byte and minor version in low byte (ie: DX 10.1 is 0x1010)
	WORD wVersion; 
	WORD wMinVersion; 
	WORD wMaxVersion; 
	bool SrcOnly;
	D3D_DRIVER_TYPE RefDriverType;

	union
	{
		D3D_DRIVER_TYPE DriverType;
		D3D_DRIVER_TYPE SrcDriverType;
	};

	D3D_FEATURE_LEVEL1 FeatureLevel;
	bool bStricterValidationLayer;
	D3DTEST_INTERFACE_LEVEL origInterfaceLevel;
	D3D_FEATURE_LEVEL1 origFeatureLevel;
	bool useD3DShim;
};


extern float D3DVersionToFloat( WORD wVersion );

struct D3D_RENDER_TARGET_OPTIONS
{
	DXGI_SWAP_CHAIN_DESC1 SwapChainDesc;
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC FullscreenDesc;
    HWND OutputWindow;
    BOOL Windowed;

	D3D_RENDER_TARGET_VIEW_DESC RenderTargetViewDesc;
	D3D_DEPTH_STENCIL_DESC DepthStencilDesc;
	D3D_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;

	bool UseDefaultRenderTargetView;
	bool UseDefaultDepthStencilView;
	bool NoRenderTargets;
	bool NoDepthStencils;
	bool NoSwapChains;
	
	union
	{
		D3D_TEXTURE1D_DESC DepthTextureDesc1D;
		D3D_TEXTURE2D_DESC DepthTextureDesc;
	};
	
	union
	{
		D3D_BUFFER_DESC RenderTargetBufferDesc;
		D3D_TEXTURE1D_DESC RenderTargetTextureDesc1D;
		D3D_TEXTURE2D_DESC RenderTargetTextureDesc;
	};
};

// Overloaded D3DCreateDevice() functions
extern HRESULT WGFTESTAPI D3DCreateDevice( 
	IDXGIAdapter ** ppAdapter,
	D3D_DRIVER_TYPE DriverType,
	UINT Flags,
	CONST void* pExtensions,
	UINT32 SDKVersion,
	ID3D10Device **ppDevice );
	

//////////////////////////////////////////////////////////////////////////
// Class:  CD3DTestFramework
//
// Base class for D3D testing.  
// NOTE: Requires all test classes be derived from CD3DTest.
//////////////////////////////////////////////////////////////////////////
class CD3DTestFramework : public CGraphicsTestFramework
{
public:
	CD3DTestFramework();

	virtual ~CD3DTestFramework();

	virtual bool AllocLowerHalfOfAddressSpace( DWORD flags );
	virtual bool Setup();
	virtual bool FreeLowerHalfOfAddressSpace();
	virtual void Cleanup();

	virtual void CleanupTest();

	virtual bool UpdateCurrentDeviceInfo(DXGLOG_DEVICEINFO * pDeviceInfo);
	
	virtual TEST_RESULT PreSetupTestCase();

	TEST_RESULT SetupDXGI();
	void CleanupDXGI();

	virtual TEST_RESULT SetupD3D();
	virtual void CleanupD3D();

	virtual TEST_RESULT SetupRenderTargets();
	virtual void CleanupRenderTargets();

	virtual bool Present(RECT *pSrc, RECT *pDest, void *pSourceDirtyRegion, void *pDestClipRegion, UINT SyncInterval, UINT dwFlags);
	
	virtual void InitOptionVariables();

	void InvalidateD3D() { m_bD3DValid = false; }
	void InvalidateDXGI() { m_bDXGIValid = false; }
	void InvalidateRenderTargets();

	// Accessors
	IDXGIFactory **GetDXGIFactories()			{ return (IDXGIFactory**)m_DXGIFactories; }
	IDXGIFactory *GetDXGIFactory(UINT uDev)			{ if (m_DXGIFactories && m_uDeviceCount > uDev) return m_DXGIFactories[ uDev ].pFactory; else return NULL; }
	IDXGIFactory1 *GetDXGIFactory1(UINT uDev)			{ if (m_DXGIFactories && m_uDeviceCount > uDev) return m_DXGIFactories[ uDev ].pFactory1; else return NULL; }
	IDXGIFactory2 *GetDXGIFactory2(UINT uDev)			{ if (m_DXGIFactories && m_uDeviceCount > uDev) return m_DXGIFactories[ uDev ].pFactory2; else return NULL; }
	IDXGIAdapter **GetDXGIAdapters()			{ return m_pAdapters; }
	IDXGIAdapter *GetSrcDXGIAdapter()		{ if (m_pAdapters && m_uDeviceCount > 0) return m_pAdapters[ 0 ]; else return NULL; }
	IDXGIAdapter *GetRefDXGIAdapter()		{ if (m_pAdapters && m_uDeviceCount > 1) return m_pAdapters[ 1 ]; else return NULL; }
	IDXGIAdapter *GetDXGIAdapter(UINT uDev)	{ if (m_pAdapters && m_uDeviceCount > uDev) return m_pAdapters[ uDev ]; else return NULL; }
	IDXGIOutput *GetSrcDXGIOutput()			{ if (m_pAdapterOutputs && m_uDeviceCount > 0) return m_pAdapterOutputs[ 0 ]; else return NULL; }
	IDXGIOutput *GetRefDXGIOutput()			{ if (m_pAdapterOutputs && m_uDeviceCount > 1) return m_pAdapterOutputs[ 1 ]; else return NULL; }
	IDXGIOutput *GetDXGIOutput(UINT uDev)	{ if (m_pAdapterOutputs && m_uDeviceCount > uDev) return m_pAdapterOutputs[ uDev ]; else return NULL; }
	
	IDXGISwapChain1 **GetDXGISwapChains(UINT uRT=0)		{ if (m_pSwapChains) return &(m_pSwapChains[uRT*m_uDeviceCount].pSwapChain1 ); else return NULL; }
	IDXGISwapChain1 *GetSrcSwapChain(UINT uRT=0)			{ if (m_pSwapChains) return m_pSwapChains[ uRT*m_uDeviceCount ].pSwapChain1; else return NULL; }
	IDXGISwapChain1 *GetRefSwapChain(UINT uRT=0)			{ if (m_pSwapChains) return m_pSwapChains[ uRT*m_uDeviceCount+1 ].pSwapChain1; else return NULL; }
	IDXGISwapChain1 *GetSwapChain(UINT uRT, UINT uDev)	{ if (m_pSwapChains) return m_pSwapChains[ uRT*m_uDeviceCount+uDev ].pSwapChain1; else return NULL; }
	
	CDevice *GetDevice()	{ return m_pD3DDevices; }
	CDevice *GetSrcDevice()			{ if (m_pD3DDevices) return m_pD3DDevices->Src(); else return NULL; }
	CDevice *GetRefDevice()			{ if (m_pD3DDevices) return m_pD3DDevices->Ref(); else return NULL; }
	CDevice *GetSubDevice(UINT uDev)	{ if (m_pD3DDevices) return m_pD3DDevices->GetSubDevice( uDev ); else return NULL; }
	
	CDebug *GetDebug()	{ return m_pD3DDebugs; }
	CDebug *GetSrcDebug()		{ if (m_pD3DDebugs) return m_pD3DDebugs->Src(); else return NULL; }
	CDebug *GetRefDebug()		{ if (m_pD3DDebugs) return m_pD3DDebugs->Ref(); else return NULL; }
	CDebug *GetSubDebug(UINT uDev)	{ if (m_pD3DDebugs) return m_pD3DDebugs->GetSubDebug( uDev ); else return NULL; }
	
	CInfoQueue *GetInfoQueue()	{ return m_pD3DInfoQueues; }
	CInfoQueue *GetSrcInfoQueue()		{ if (m_pD3DInfoQueues) return m_pD3DInfoQueues->Src(); else return NULL; }
	CInfoQueue *GetRefInfoQueue()		{ if (m_pD3DInfoQueues) return m_pD3DInfoQueues->Ref(); else return NULL; }
	CInfoQueue *GetSubInfoQueue(UINT uDev)	{ if (m_pD3DInfoQueues) return m_pD3DInfoQueues->GetSubInfoQueue( uDev ); else return NULL; }
	
	CSwitchToRef *GetSwitchToRef()	{ return m_pD3DSwitchToRef; }
	CSwitchToRef *GetSrcSwitchToRef()		{ if (m_pD3DSwitchToRef) return m_pD3DSwitchToRef->Src(); else return NULL; }
	CSwitchToRef *GetRefSwitchToRef()		{ if (m_pD3DSwitchToRef) return m_pD3DSwitchToRef->Ref(); else return NULL; }
	CSwitchToRef *GetSubSwitchToRef(UINT uDev)	{ if (m_pD3DSwitchToRef) return m_pD3DSwitchToRef->GetSubSwitchToRef( uDev ); else return NULL; }

	CMultithread *GetMultithread()	{ return m_pD3DMultithreads; }
	CMultithread *GetSrcMultithread()		{ if (m_pD3DMultithreads) return m_pD3DMultithreads->Src(); else return NULL; }
	CMultithread *GetRefMultithread()		{ if (m_pD3DMultithreads) return m_pD3DMultithreads->Ref(); else return NULL; }
	CMultithread *GetSubMultithread(UINT uDev)	{ if (m_pD3DMultithreads) return m_pD3DMultithreads->GetSubMultithread( uDev ); else return NULL; }
	
	CBuffer *GetRTBuffer(UINT uRT=0)	{ if (m_pRTBuffers) return m_pRTBuffers[ uRT ]; else return NULL; }
	CBuffer *GetSrcRTBuffer(UINT uRT=0)		{ if (m_pRTBuffers && m_pRTBuffers[ uRT ]) return m_pRTBuffers[ uRT ]->Src(); else return NULL; }
	CBuffer *GetRefRTBuffer(UINT uRT=0)		{ if (m_pRTBuffers && m_pRTBuffers[ uRT ]) return m_pRTBuffers[ uRT ]->Ref(); else return NULL; }
	CBuffer *GetSubRTBuffer(UINT uDev, UINT uRT=0)	{ if (m_pRTBuffers && m_pRTBuffers[ uRT ]) return m_pRTBuffers[ uRT ]->GetSubBuffer( uDev ); else return NULL; }
	
	CTexture1D *GetRTTexture1D(UINT uRT=0)	{ if (m_pRTTextures1D) return m_pRTTextures1D[ uRT ]; else return NULL; }
	CTexture1D *GetSrcRTTexture1D(UINT uRT=0)		{ if (m_pRTTextures1D && m_pRTTextures1D[ uRT ]) return m_pRTTextures1D[ uRT ]->Src(); else return NULL; }
	CTexture1D *GetRefRTTexture1D(UINT uRT=0)		{ if (m_pRTTextures1D && m_pRTTextures1D[ uRT ]) return m_pRTTextures1D[ uRT ]->Ref(); else return NULL; }
	CTexture1D *GetSubRTTexture1D(UINT uDev, UINT uRT=0)	{ if (m_pRTTextures1D && m_pRTTextures1D[ uRT ]) return m_pRTTextures1D[ uRT ]->GetSubTexture1D( uDev ); else return NULL; }
	
	CTexture2D *GetRTTexture(UINT uRT=0)	{ if (m_pRTTextures) return m_pRTTextures[ uRT ]; else return NULL; }
	CTexture2D *GetSrcRTTexture(UINT uRT=0)		{ if (m_pRTTextures && m_pRTTextures[ uRT ]) return m_pRTTextures[ uRT ]->Src(); else return NULL; }
	CTexture2D *GetRefRTTexture(UINT uRT=0)		{ if (m_pRTTextures && m_pRTTextures[ uRT ]) return m_pRTTextures[ uRT ]->Ref(); else return NULL; }
	CTexture2D *GetSubRTTexture(UINT uDev, UINT uRT=0)	{ if (m_pRTTextures && m_pRTTextures[ uRT ]) return m_pRTTextures[ uRT ]->GetSubTexture2D( uDev ); else return NULL; }
	
	CRenderTargetView *GetRTView(UINT uRT=0)	{ if (m_pRTViews) return m_pRTViews[ uRT ]; else return NULL; }
	CRenderTargetView *GetSrcRTView(UINT uRT=0)		{ if (m_pRTViews && m_pRTViews[ uRT ]) return m_pRTViews[ uRT ]->Src(); else return NULL; }
	CRenderTargetView *GetRefRTView(UINT uRT=0)		{ if (m_pRTViews && m_pRTViews[ uRT ]) return m_pRTViews[ uRT ]->Ref(); else return NULL; }
	CRenderTargetView *GetSubRTView(UINT uDev, UINT uRT=0)	{ if (m_pRTViews && m_pRTViews[ uRT ]) return m_pRTViews[ uRT ]->GetSubRenderTargetView( uDev ); else return NULL; }
	
	CTexture1D *GetDSTexture1D(UINT uRT=0) { return m_pDepthStencils1D; }
	CTexture1D *GetSrcDSTexture1D(UINT uRT=0)	 { if (m_pDepthStencils1D) return m_pDepthStencils1D->Src(); else return NULL; }
	CTexture1D *GetRefDSTexture1D(UINT uRT=0)	 { if (m_pDepthStencils1D) return m_pDepthStencils1D->Ref(); else return NULL; }
	CTexture1D *GetSubDSTexture1D(UINT uDev, UINT uRT=0) { if (m_pDepthStencils1D) return m_pDepthStencils1D->GetSubTexture1D( uDev ); else return NULL; }
	
	CTexture2D *GetDSTexture(UINT uRT=0) { return m_pDepthStencils; }
	CTexture2D *GetSrcDSTexture(UINT uRT=0)	 { if (m_pDepthStencils) return m_pDepthStencils->Src(); else return NULL; }
	CTexture2D *GetRefDSTexture(UINT uRT=0)	 { if (m_pDepthStencils) return m_pDepthStencils->Ref(); else return NULL; }
	CTexture2D *GetSubDSTexture(UINT uDev, UINT uRT=0) { if (m_pDepthStencils) return m_pDepthStencils->GetSubTexture2D( uDev ); else return NULL; }
	
	CDepthStencilView *GetDSView() { return m_pDSViews; }
	CDepthStencilView *GetSrcDSView()	 { if (m_pDSViews) return m_pDSViews->Src(); else return NULL; }
	CDepthStencilView *GetRefDSView()	 { if (m_pDSViews) return m_pDSViews->Ref(); else return NULL; }
	CDepthStencilView *GetSubDSView(UINT uDev) { if (m_pDSViews) return m_pDSViews->GetSubDepthStencilView( uDev ); else return NULL; }
	
	NRESULT CompileShaderFromResource( HMODULE hModule, LPCSTR pResource, LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags, CBlob **ppShader, CBlob **ppErrorMessages, SHADER_MACRO const* pMacros = nullptr );
	NRESULT CompileShaderFromResource( HMODULE hModule, LPCWSTR pResource, LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags, CBlob **ppShader, CBlob **ppErrorMessages, SHADER_MACRO const* pMacros = nullptr );

	NRESULT CompileShaderFromFile( LPCSTR pFile, LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags, CBlob** ppShader, CBlob** ppErrorMsgs);
	NRESULT CompileShaderFromFile( LPCWSTR pFile, LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags, CBlob** ppShader, CBlob** ppErrorMsgs);

	NRESULT CompileShaderFromMemory( LPCSTR pSrcData, SIZE_T SrcDataLen, LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags, CBlob** ppShader, CBlob** ppErrorMsgs);

	NRESULT SaveTextureToMemory( CResource* pResource, CBlob** ppDestBuf );
	NRESULT CreateTextureFromMemory( CDevice* pDevice, CBlob* pSrcBuf, CResource** ppDestResource, DXGI_FORMAT newFormat, CResource* pClonedResource, bool bStaging );

	NRESULT GoldenImage( CResource* pResource, LPCWSTR pFile );

	HRESULT CreateDevice( IDXGIAdapter **ppAdapter, D3D_DRIVER_TYPE DriverType, UINT uFlags, WORD wVersion, CDevice **ppDevice );

	HRESULT GetDataWithTimeout( CQuery *pQuery, void* pData, UINT dataSize, UINT dataFlags );

protected:
	void SetRuntimeVersion();

	virtual TEST_RESULT SetupTest();
	virtual void PreGenerateTestCase();
	virtual void PostCleanupTestCase();
	
	virtual void PreAllocateTest();

private:
	// Helper functions.
	bool UpdateGraphicsValidFlags();
	TEST_RESULT RefreshGraphics();
	HRESULT CreateDevice10( IDXGIAdapter **ppAdapter, D3D_DRIVER_TYPE DriverType, UINT uFlags, CDevice **ppDevice );
	HRESULT CreateDevice10_1( IDXGIAdapter **ppAdapter, D3D_DRIVER_TYPE DriverType, UINT uFlags, CDevice **ppDevice );
	HRESULT CreateDevice11( IDXGIAdapter **ppAdapter, D3D_DRIVER_TYPE DriverType, UINT uFlags, CDevice **ppDevice );

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

	int m_iSubPixelPrecision; //Added for WARP support - brushan
	bool m_bWARP; //Added for WARP-specific tests - brushan
	bool m_bReserveLowerMemory;
	bool m_bCommitLowerMemory;
	bool m_bForce10_0;
	bool m_bForce10_1;
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
	DXGI_FACTORY_POINTER	*m_DXGIFactories;
	IDXGIAdapter			**m_pAdapters;
	IDXGIOutput				**m_pAdapterOutputs;
	DXGI_SWAPCHAIN_POINTER	*m_pSwapChains;		
	CDevice				*m_pD3DDevices;
	CDebug				*m_pD3DDebugs;
	CInfoQueue			*m_pD3DInfoQueues;
	CMultithread		*m_pD3DMultithreads;
	CSwitchToRef		*m_pD3DSwitchToRef;

	CTexture2D		**m_pRTTextures;
	CTexture1D		**m_pRTTextures1D;
	CBuffer			**m_pRTBuffers;

	CRenderTargetView	**m_pRTViews;

	CTexture2D			*m_pDepthStencils;
	CTexture1D			*m_pDepthStencils1D;

	CDepthStencilView	*m_pDSViews;
	CONST void				*m_pExtensions;


private:
	bool				m_bD3DValid;
	bool				m_bRenderTargetValid;
	bool				m_bDXGIValid;
	bool				m_bSetupCalled;

	// Global cached device options
	DXGI_OPTIONS m_DXGIOptionsGlobal;
	D3D_OPTIONS m_D3DOptionsGlobal;
	D3D_RENDER_TARGET_OPTIONS m_RenderTargetOptionsGlobal;
	
	// Test Group wide cached device options
	DXGI_OPTIONS m_DXGIOptionsGroup;
	D3D_OPTIONS m_D3DOptionsGroup;
	D3D_RENDER_TARGET_OPTIONS m_RenderTargetOptionsGroup;
};


//////////////////////////////////////////////////////////////////////////
// CD3DTest
//
// Base class for tests using the Wrapped D3D Interfaces.
//////////////////////////////////////////////////////////////////////////
class CD3DTest : public CTest
{
public:
	TEST_RESULT Setup() { return RESULT_PASS; }
	void Cleanup();
		
	virtual int GetNumRenderWindows() const { return 0; }
	virtual bool FullScreen() const { return false; }

	CD3DTestFramework *GetFramework() const { return dynamic_cast< CD3DTestFramework *>( ::GetFramework() ); }

	IDXGIFactory **GetDXGIFactories()					{ return GetFramework()->GetDXGIFactories(); }
	IDXGIFactory *GetDXGIFactory(UINT uDev)				{ return GetFramework()->GetDXGIFactory( uDev ); }
	IDXGIAdapter **GetDXGIAdapters()					{ return GetFramework()->GetDXGIAdapters(); }
	IDXGISwapChain1 **GetDXGISwapChains(UINT uRT=0)	{ return GetFramework()->GetDXGISwapChains( uRT ); }
	
	CDevice *GetDevice()				{ return GetFramework()->GetDevice(); }
	CDebug *GetDebug()					{ return GetFramework()->GetDebug(); }
	CMultithread *GetD3DMultithread()	{ return GetFramework()->GetMultithread(); }

	CTexture2D *GetRTTexture(UINT uRT=0)		{ return GetFramework()->GetRTTexture( uRT ); }
	CRenderTargetView *GetRTView(UINT uRT=0)	{ return GetFramework()->GetRTView( uRT ); }
	CTexture2D *GetDSTexture()					{ return GetFramework()->GetDSTexture(); }
	CDepthStencilView *GetDSView()				{ return GetFramework()->GetDSView(); }
	
	bool Present(RECT *pSrc, RECT *pDest, void *pSourceDirtyRegion, void *pDestClipRegion, UINT SyncInterval, UINT dwFlags)
						{ return GetFramework()->Present(pSrc, pDest, pSourceDirtyRegion, pDestClipRegion, SyncInterval, dwFlags); }
};
