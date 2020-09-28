//////////////////////////////////////////////////////////////////////////
//  D3D11Test.h
//  created:	2004/10/07
//
//  purpose: Base framework classes for wrapped D3D test classes.
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <windows.h>
#include <detours.h>
#include <d3d11_4.h>
#include <d3d11.h>
#include <d3d10_1.h>
#include <d3d10.h>

#include "D3DDeviceManager.h"
#include "d3d11internaltest.h" // Internal interfaces for ID3D11DeviceTest
#include "d3d11videocapture.h"

#include "WGFTestCore.h"
#include "DXGIValueSets.h"
#include "d3d11.h"
#include "dxgi1_2.h"
#include "NResult.h"
#include "ResourceViewer.h"
#include "TiledResourceHelper.h"

#include "new_off.h"
#include "d3dx10.h"
#include "new_on.h"

#include <GraphicsTest.h>

#include <assert.h>

//////////////////////////////////////////////////////////////////////////
// CD3D11TestFramework
//
// Base class for D3D testing.  
// NOTE: Requires all test classes be derived from CD3D11Test.
//////////////////////////////////////////////////////////////////////////
class CD3D11TestFramework : public CGraphicsTestFramework
{
	CD3D11DeviceManager *m_pD3DDeviceManager;

public:
	CD3D11TestFramework();
	virtual ~CD3D11TestFramework();

	virtual bool Setup();
	virtual void Cleanup();

	virtual TEST_RESULT SetupTest();
	virtual void CleanupTest();

	virtual void InitOptionVariables();

	virtual void PostCleanupTestCase();

	virtual void PreSetupTest();

	virtual void PostCleanupTest();

	virtual TEST_RESULT PreSetupTestCase();

	bool UpdateCurrentDeviceInfo(DXGLOG_DEVICEINFO * pDeviceInfo) 
	{
		return m_pD3DDeviceManager->UpdateCurrentDeviceInfo(pDeviceInfo); 
	}

	virtual HRESULT SetupProxyDriver(HMODULE *hProxyDriver) { return E_FAIL; }

	virtual UINT GetDefaultQueryTimeout() { return 5000; }

	CD3D11DeviceManager *GetD3DDeviceManager() const { return m_pD3DDeviceManager; }
	CGraphicsDeviceManager *GetGraphicsDeviceManager() const { return m_pD3DDeviceManager; }

	void InitOutputParameters(_In_ class CD3D11Test *pTest);

	HRESULT IsLegacyD3D10WarpDll(_In_ IDXGIAdapter* pAdapter, _In_ ID3D11Device* pDevice, _Out_ bool& IsLegacyWarp10Dll) const;

	//
	// Private Data Members
	//
private:
	CResourceViewer			m_resourceViewer;
	CTiledResourceHelper    m_tiledResourceHelper;
	bool					m_viewResources;
	bool                    m_bMapDefaultDisabled;

	D3D_OPTIONS m_D3DOptionsCached;
	D3D_RENDER_TARGET_OPTIONS m_RenderTargetOptionsCached;

	//
	// Public Data Members
	//
public:
	D3D_OPTIONS m_D3DOptions;
	D3D_RENDER_TARGET_OPTIONS m_RenderTargetOptions;
	bool				m_bSDKTest;
	CVariationParameter<bool> *m_pOutputParamTiledResourceUsed;
	CVariationParameter<bool> *m_pOutputParamMapDefaultResourceUsed;

	//
	// Accessors
	//
public:
	D3D_OPTIONS *GetD3DOptions()                        { return m_pD3DDeviceManager->GetD3DOptions(); }

	IDXGIFactory *GetDXGIFactory()			            { return m_pD3DDeviceManager->GetDXGIFactory(); }
	IDXGIFactory1 *GetDXGIFactory1()		            { return m_pD3DDeviceManager->GetDXGIFactory1(); }
	IDXGIFactory2 *GetDXGIFactory2()		            { return m_pD3DDeviceManager->GetDXGIFactory2(); }
	IDXGIAdapter *GetDXGIAdapter()			            { return m_pD3DDeviceManager->GetDXGIAdapter(); }
	IDXGIOutput *GetDXGIOutput()	                    { return m_pD3DDeviceManager->GetDXGIOutput(); }

	IDXGISwapChain1 *GetSwapChain()	                    { return m_pD3DDeviceManager->GetDXGISwapChain(); }

	ID3D11Device *GetDevice()	                        { return m_pD3DDeviceManager->GetDevice(); }
	ID3D11Device1 *GetDevice1()	                        { return m_pD3DDeviceManager->GetDevice1(); }
	ID3D11Device2 *GetDevice2()	                        { return m_pD3DDeviceManager->GetDevice2(); }
	ID3D11DeviceTest *GetTestDevice()                   { return m_pD3DDeviceManager->GetTestDevice(); };
	ID3D11DeviceContext *GetImmediateContext()	        { return m_pD3DDeviceManager->GetImmediateContext(); }
	ID3D11DeviceContext1 *GetImmediateContext1()	    { return m_pD3DDeviceManager->GetImmediateContext1(); }
	ID3D11DeviceContext2 *GetImmediateContext2()	    { return m_pD3DDeviceManager->GetImmediateContext2(); }
	ID3D11DeviceContext *GetEffectiveContext()	        { return m_pD3DDeviceManager->GetEffectiveContext(); }
	ID3D11DeviceContext1 *GetEffectiveContext1()	    { return m_pD3DDeviceManager->GetEffectiveContext1(); }
	ID3D11DeviceContext2 *GetEffectiveContext2()	    { return m_pD3DDeviceManager->GetEffectiveContext2(); }
	bool IsDeferredContextUsed() const                  { return m_pD3DDeviceManager->IsDeferredContextUsed(); }

	CTiledResourceHelper::CTilePoolManager *GetTilePoolManager() { return m_tiledResourceHelper.GetTilePoolManager(); }

	HRESULT CreateTexture2DTiledOptional(_In_ const D3D11_TEXTURE2D_DESC *pDesc, _In_ const D3D11_SUBRESOURCE_DATA *pInitialData, _Out_ ID3D11Texture2D **ppTexture2D);
	HRESULT CreateBufferTiledOptional(_In_ const D3D11_BUFFER_DESC *pDesc, _In_ const D3D11_SUBRESOURCE_DATA *pInitialData, _Out_ ID3D11Buffer **ppBuffer, DXGI_FORMAT ViewFormat = DXGI_FORMAT_UNKNOWN);
	HRESULT CreateBufferTiledOptionalMapDefaultOptional(_In_ const D3D11_BUFFER_DESC *pDesc, _In_ const D3D11_SUBRESOURCE_DATA *pInitialData, _Out_ ID3D11Buffer **ppBuffer, DXGI_FORMAT ViewFormat = DXGI_FORMAT_UNKNOWN);
	static bool FormatSupportsMap(DXGI_FORMAT format);
	
	HRESULT TryGetMapDefaultParameters(
		bool fRequireLinearCpuAccess,
		UINT dimensionCount,
		DXGI_FORMAT format,
		UINT mipLevels,
		UINT arraySize,
		UINT bindFlags,
		DXGI_SAMPLE_DESC sampleDesc,
		__out bool *pfSupported,
		__out UINT *pCpuFlags,
		__out D3D11_TEXTURE_LAYOUT *pTextureLayout
		);

	HRESULT CreateTexture2DTiledOptionalMapDefaultOptional(
		_In_ const D3D11_TEXTURE2D_DESC *pDesc,
		_In_opt_ const D3D11_SUBRESOURCE_DATA *pInitialData,
		_Outptr_ ID3D11Texture2D **ppTexture2D
		);
		
	HRESULT CreateTexture2DMapDefaultOptional(
		_In_ const D3D11_TEXTURE2D_DESC *pDesc,
		_In_opt_ const D3D11_SUBRESOURCE_DATA *pInitialData,
		_Outptr_ ID3D11Texture2D **ppTexture2D
		);
	
	HRESULT CreateTexture2DHelper(
		_In_ const D3D11_TEXTURE2D_DESC *pDesc,
		_In_opt_ const D3D11_SUBRESOURCE_DATA *pInitialData,
		bool fTiledOptional,
		bool fMapDefaultOptional,
		_Outptr_ ID3D11Texture2D **ppTexture2D
		);
	
	HRESULT CreateTexture3DMapDefaultOptional(
		_In_ const D3D11_TEXTURE3D_DESC *pDesc,
		_In_opt_ const D3D11_SUBRESOURCE_DATA *pInitialData, 
		_Outptr_ ID3D11Texture3D **ppTexture3D
		);
	
//	D3DTEST_TILED_RESOURCE_MODE GetTiledResourceMode() const { return m_D3DOptions.tiledResourceMode; }

//	CTilePoolManager *GetTilePoolManager() { return m_pTilePoolManager; }
	const D3D11_TEXTURE2D_DESC* ExerciseVariousNV12Capabilities(_In_ const D3D11_TEXTURE2D_DESC* pDesc, _Out_ D3D11_TEXTURE2D_DESC* pLclDesc);

	HRESULT ExecuteEffectiveContext()                   { return m_pD3DDeviceManager->ExecuteEffectiveContext(); }

	ID3D11Debug *GetDebug()	{ return m_pD3DDeviceManager->GetDebug(); }

	ID3D11InfoQueue *GetInfoQueue()	{ return m_pD3DDeviceManager->GetInfoQueue(); }

	ID3D11Buffer *GetRTBuffer(UINT uRT=0)	{ return m_pD3DDeviceManager->GetRTBuffer(); }

	ID3D11Texture1D *GetRTTexture1D(UINT uRT=0)	{ return m_pD3DDeviceManager->GetRTTexture1D(); }

	ID3D11Texture2D *GetRTTexture(UINT uRT=0)	{ return m_pD3DDeviceManager->GetRTTexture(); }

	ID3D11RenderTargetView *GetRTView(UINT uRT=0)	{ return m_pD3DDeviceManager->GetRTView(); }

	ID3D11Texture1D *GetDSTexture1D(UINT uRT=0) { return m_pD3DDeviceManager->GetDSTexture1D(); }

	ID3D11Texture2D *GetDSTexture(UINT uRT=0) { return m_pD3DDeviceManager->GetDSTexture(); }

	ID3D11DepthStencilView *GetDSView() { return m_pD3DDeviceManager->GetDSView(); }

	CResourceViewer &GetResourceViewer() { return m_resourceViewer; }
	bool ViewResources() { return m_viewResources; }
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
		CResourceViewerWindow *pWindow = nullptr;

		if(m_viewResources)
		{
			pWindow = m_resourceViewer.ShowResource(name, pResource, mipLevel, arraySlice, depthSlice, resizeScale, format);
		}

		return pWindow;
	}

	bool IsSDKTest() const { return m_bSDKTest; }
	bool Is11_1DDISupported( ID3D11Device *pDevice ) const { return m_pD3DDeviceManager->Is11_1DDISupported(pDevice); }

	const char * GetShaderProfile(D3D_SHADER_STAGE ShaderStage, D3D_FEATURE_LEVEL MinFeatureLevel, D3D_FEATURE_LEVEL MaxFeatureLevel)
	{
		return CD3D11DeviceManager::GetShaderProfile(ShaderStage, MinFeatureLevel, MaxFeatureLevel, m_D3DOptions);
	}

	bool Present(RECT *pSrc, RECT *pDest, void *pSourceDirtyRegion, void *pDestClipRegion, UINT SyncInterval, UINT dwFlags)
	{
		ShowResource(L"Present", GetRTTexture());
		return m_pD3DDeviceManager->Present(pSrc, pDest, pSourceDirtyRegion, pDestClipRegion, SyncInterval, dwFlags);
	}

	TEST_RESULT SetupDXGIAndD3D();

	TEST_RESULT SetupDXGI();

	TEST_RESULT SetupD3D();

	TEST_RESULT SetupRenderTarget();

	TEST_RESULT RefreshGraphics();

	void SetRenderTargetProperties();

	void CleanupDXGI();

	void CleanupD3D();

	void CleanupRenderTarget();

	HRESULT CreateDevice( IDXGIAdapter *pAdapter, D3D_DRIVER_TYPE DriverType, UINT uFlags, D3D_FEATURE_LEVEL FeatureLevel, ID3D11Device **ppDevice, ID3D11DeviceContext **ppDeviceContext )
	{
		return m_pD3DDeviceManager->CreateDevice(pAdapter, DriverType, uFlags, FeatureLevel, ppDevice, ppDeviceContext);
	}

	UINT GetRefreshGraphicsFlags() const {return m_pD3DDeviceManager->m_RefreshGraphicsFlags; }

	//////////////////////////////////////////////////////////////////////////
	// D3DDisassemble and D3DReflect entry points.  (Windows build doesn't have these.)
	//////////////////////////////////////////////////////////////////////////
	HRESULT LL_D3DDisassemble( 
				   LPCVOID pSrcData,
				   SIZE_T SrcDataSize,
				   UINT Flags,
				   LPCSTR szComments,
				   ID3D10Blob** ppDisassembly);
	HRESULT LL_D3DReflect(
					LPCVOID pSrcData,
					SIZE_T SrcDataSize,
					REFIID pInterface,
					void** ppReflector);

	// Entry point D3DGetInputAndOutputSignatureBlob
	//////////////////////////////////////////////////////////////////////////
	HRESULT LL_D3DGetInputAndOutputSignatureBlob(
					LPCVOID pSrcData,
					SIZE_T SrcDataSize,
					ID3DBlob** ppSignatureBlob);

	UINT GetQueryTimeout() const { return m_pD3DDeviceManager->m_nQueryTimeout; }
	HRESULT GetDataWithTimeout( ID3D11DeviceContext* pDeviceContext, ID3D11Asynchronous* pAsync, void* pData, UINT dataSize, UINT dataFlags, UINT timeout = 0 )
	{
		return m_pD3DDeviceManager->GetDataWithTimeout(pDeviceContext, pAsync, pData, dataSize, dataFlags, timeout);
	}

	//
	// Static Methods
	//
public:
	HRESULT CompileShaderFromResource( HMODULE hModule, LPCTSTR pResource, LPCSTR pFunctionName, D3D_SHADER_STAGE ShaderStage, D3D_FEATURE_LEVEL MinFeatureLevel, D3D_FEATURE_LEVEL MaxFeatureLevel, UINT Flags, ID3D10Blob **ppShader, ID3D10Blob **ppErrorMessages, CONST D3D10_SHADER_MACRO *pDefines = NULL, ID3D10Blob **ppShaderText = NULL )
	{
		return m_pD3DDeviceManager->CompileShaderFromResource(hModule, pResource, pFunctionName, ShaderStage, MinFeatureLevel, MaxFeatureLevel, Flags, ppShader, ppErrorMessages, pDefines, ppShaderText);
	}
	HRESULT CompileShaderFromFile( LPCTSTR pFile, LPCSTR pFunctionName, D3D_SHADER_STAGE ShaderStage, D3D_FEATURE_LEVEL MinFeatureLevel, D3D_FEATURE_LEVEL MaxFeatureLevel, UINT Flags, ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs, CONST D3D10_SHADER_MACRO *pDefines = NULL, ID3D10Blob **ppShaderText = NULL )
	{
		return m_pD3DDeviceManager->CompileShaderFromFile(pFile, pFunctionName, ShaderStage, MinFeatureLevel, MaxFeatureLevel, Flags, ppShader, ppErrorMsgs, pDefines, ppShaderText);
	}
	HRESULT CompileShaderFromMemory( LPCSTR pSrcData, SIZE_T SrcDataLen, LPCSTR pFunctionName, D3D_SHADER_STAGE ShaderStage, D3D_FEATURE_LEVEL MinFeatureLevel, D3D_FEATURE_LEVEL MaxFeatureLevel, UINT Flags, ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs, CONST D3D10_SHADER_MACRO *pDefines = NULL, ID3D10Blob **ppShaderText = NULL )
	{
		return m_pD3DDeviceManager->CompileShaderFromMemory(pSrcData, SrcDataLen, pFunctionName, ShaderStage, MinFeatureLevel, MaxFeatureLevel, Flags, ppShader, ppErrorMsgs, pDefines, ppShaderText);
	}
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
	TEST_RESULT PostExecuteTestCase()
	{
		HRESULT hr = GetFramework()->GetD3DDeviceManager()->CheckForDriverErrors();
		return S_OK == hr ? RESULT_PASS : RESULT_FAIL;
	}
	void Cleanup();

	virtual int GetNumRenderWindows() const { return 0; }
	virtual bool FullScreen() const { return false; }
	virtual bool CreateRTFromSwapChainProps() const { return false; }

	virtual void InitOutputParameters();

	CD3D11TestFramework *GetFramework() const { return dynamic_cast< CD3D11TestFramework *>( ::GetFramework() ); }

	IDXGIFactory *GetDXGIFactory()					{ return GetFramework()->GetDXGIFactory(); }
	IDXGIAdapter *GetDXGIAdapter()				    { return GetFramework()->GetDXGIAdapter(); }
	IDXGISwapChain1 *GetDXGISwapChain()	            { return GetFramework()->GetSwapChain(); }

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
		_In_ PCWSTR name,
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
