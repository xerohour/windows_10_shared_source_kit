#ifndef DXGI_TEST_D3D10_SWAPCHAIN_INCLUDED
#define DXGI_TEST_D3D10_SWAPCHAIN_INCLUDED

#include <windows.h>
#include <dxgi1_2.h>
#include <d3d9.h>
#include <d3d10_1.h>
#include <d3dx10.h>

#include <string>
#include <map>

#include "WGFTestCore.h"
#include "DXGITestFramework.h"

enum DXGI_VERSION
{
	DXGI_VERSION_1_0 = 0,
	DXGI_VERSION_1_1,
	DXGI_VERSION_1_2
};

typedef HRESULT (WINAPI *PDIRECT3DCREATE9EX)
(
	UINT SDKVersion,
	IDirect3D9Ex **
);


typedef HRESULT (WINAPI *PD3D10COMPILESHADER)
(
	LPCSTR pSrcData,
	SIZE_T SrcDataLen,
	LPCSTR pFileName,
	CONST D3D10_SHADER_MACRO* pDefines,
	LPD3D10INCLUDE pInclude, 
    LPCSTR pFunctionName,
	LPCSTR pProfile,
	UINT Flags,
	ID3D10Blob** ppShader,
	ID3D10Blob** ppErrorMsgs
);

class clsD3D9Device
{
	//
	// Construction/destruction
	//

	public:
		clsD3D9Device();

		virtual ~clsD3D9Device();


	//
	// Public methods
	//

	public:
		HRESULT Initialize
		(
			bool bTerminalServices,
            bool recreateDevice
		);

		void Cleanup();


		// Uses Direct3D9's GetFrontBufferData to
		// do a screen scrape, and then converts
		// to a DXGI surface.   
        bool GetFrontBufferData
		(
			LPCWSTR DeviceName,
			IDXGISurface * pDXGISurface
		);

    private:
    
        bool GetFrontBufferData
	    (
		    HMONITOR hMonitor,
		    IDXGISurface * pDXGISurface
	    );
		
	//
	// Member data
	//

	private:
		HMODULE m_hD3D9;
		PDIRECT3DCREATE9EX m_pDirect3DCreate9Ex;

		IDirect3D9Ex * m_pD3DEx;

		// Map from device names to HMONITORs
		std::map< std::wstring, HMONITOR > m_DeviceNameToMonitorMap;

		// Map from HMONITORs to adapters
		std::map< HMONITOR, UINT > m_MonitorMap;

		// Map from HMONITORs to GetFrontBufferData surfaces
		std::map< HMONITOR, IDirect3DSurface9 * > m_SurfaceMap;

		// Device collection
		std::vector< IDirect3DDevice9Ex* > m_DeviceExVector;

        bool m_RecreateDevice;
};


class clsD3D10Device
{
	public:
		clsD3D10Device(CTest *pInitLoggingGroup);

		virtual ~clsD3D10Device();


	private:
		friend class clsDXGISwapChain;

		LPD3DX10SPRITE pAuxiliarySprite;

		bool m_bAlternateRender;
		UINT m_nxDivisions, m_nyDivisions;
		UINT m_nNumVertices, m_nNumIndixes;
		UINT m_nStride;

		bool bReadyForTexRender;
		ID3D10Buffer *m_pVB;
		ID3D10Buffer *m_pIB;
		ID3D10Buffer *m_pVSBuffer;
		ID3D10InputLayout *m_pEL;
		ID3D10PixelShader *m_pPixelShader;
		ID3D10VertexShader *m_pVertexShader;
		ID3D10SamplerState *m_pSampler;
		ID3D10RasterizerState *m_pRastState;
		ID3D10BlendState *m_pBlendState;
		ID3D10DepthStencilState *m_pDepthState;


	protected:
		CTest * const pLoggingGroup;

		ID3D10Device *pD3D10Device;

		DXGI_MODE_ROTATION m_RotationMode;


	public:
		HRESULT InitializeDevice
		(
			IDXGIAdapter * pAdapter,
			D3D10_DRIVER_TYPE DriverType,
			HMODULE hSoftware,
			UINT Flags,
			UINT32 SDKVersion,
			TEST_FEATURE_LEVEL TestFeatureLevel = TEST_FEATURE_10_0_LEVEL_10_0,
			PFN_D3D10_CREATE_DEVICE1 pD3D10CreateDevice1 = NULL
		);

		HRESULT CreateAuxiliarySprite();

		bool IsReadyForTexRender() {return bReadyForTexRender;}

		HRESULT LoadShader
		(
			LPCSTR ShaderCode,
			__out  void **ppShader,
			LPCSTR pFuncName,
			LPCSTR pProfile,
			__out_opt ID3D10Blob **ppRetainCompiledShader = NULL
		);

		bool CreateTexRenderResources
		(
			LPCSTR VertexShaderModel,
			LPCSTR PixelShaderModel
		);

		void ReleaseTexRenderResources();

		bool RenderTexture
		(
			__in     ID3D10RenderTargetView *pRTView,
			__in     ID3D10ShaderResourceView *pTexSRV,
			__in     const D3D10_VIEWPORT *pD3D10ViewPort
		);

		bool GetFactory
		(
			__inout     IDXGIFactory **pFactoryPointer
		);

		BOOL GetDevicePtr
		(
			__inout     ID3D10Device **pDevicePointer
		);

        BOOL  SetDevicePtr
        (
	        __in ID3D10Device *pD3DDevice
        );

		void ReleaseAllObjects();

		inline void SetAlternateRenderPath( bool bAlternateRenderPath )
		{
			m_bAlternateRender = bAlternateRenderPath;
		};

		inline void SetRotationMode( DXGI_MODE_ROTATION rotationMode )
		{
			m_RotationMode = rotationMode;
		};
};


class clsGDIData
{
	public:
		clsGDIData()
		{
		};

		~clsGDIData()
		{
		};


	public:
		UINT  GetAttachedDisplayDeviceCount()
		{
			DWORD               dwDevNum = 0;
			DISPLAY_DEVICE      dispDev = {0};
			BOOL                bReturn = FALSE;
			UINT                nCount = 0;


			for ( dwDevNum = 0; (int)dwDevNum < 10; dwDevNum++ )
			{ 
				memset( &dispDev, 0, sizeof(dispDev) );
				dispDev.cb = sizeof(dispDev);

				if ( ( bReturn = EnumDisplayDevices( NULL, dwDevNum, &dispDev, 0 ) ) == TRUE 
					&& !(dispDev.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) 
					&& (dispDev.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) )
				{
					nCount++;
				}
			}

			return nCount;
		};
};


class clsDXGISwapChain
{
	private:
		UINT nDisplayDeviceCount;


	protected:
		CTest * const pLoggingGroup;
		clsD3D10Device * const pParentD3D10Device;
		IDXGISwapChain *pSwapChain;


	protected:
		BOOL  InitializeSwapChainImpl(HWND hWnd,  DXGI_SWAP_CHAIN_DESC1 *pSwapChainDesc, DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc, DXGI_VERSION dxgiVersion );
		BOOL  InitializeFullscreenSwapChainImpl(HWND hWnd, DXGI_SWAP_CHAIN_DESC1 *pSwapChainDesc, DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscrenDesc, DXGI_VERSION dxgiVersion );


	public:
		clsDXGISwapChain
		(
			CTest * pInitLoggingGroup,
			clsD3D10Device * pInitParentD3D10Device
		)
		:
			pLoggingGroup( pInitLoggingGroup ),
			pParentD3D10Device( pInitParentD3D10Device ),
			pSwapChain( NULL )
		{
			clsGDIData  GDIData;

			nDisplayDeviceCount = GDIData.GetAttachedDisplayDeviceCount();
		};

		virtual ~clsDXGISwapChain();


	public:
		BOOL  InitializeSwapChain( DXGI_SWAP_CHAIN_DESC *pSwapChainDesc );
		BOOL  InitializeSwapChain1( DXGI_SWAP_CHAIN_DESC *pSwapChainDesc );
		BOOL  InitializeSwapChain1( HWND hwnd, DXGI_SWAP_CHAIN_DESC1 *pSwapChainDesc, DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc );

		BOOL  InitializeFullscreenSwapChain( DXGI_SWAP_CHAIN_DESC* pSwapChainDesc );
		BOOL  InitializeFullscreenSwapChain1( DXGI_SWAP_CHAIN_DESC* pSwapChainDesc );
		BOOL  InitializeFullscreenSwapChain1( HWND hwnd, DXGI_SWAP_CHAIN_DESC1 *pSwapChainDesc, DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc );

		static DXGI_SWAP_CHAIN_DESC1 CopyToSwapChainDesc1( const DXGI_SWAP_CHAIN_DESC &swapChainDesc, DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc );
		static DXGI_SWAP_CHAIN_DESC CopyToSwapChainDesc( HWND hWnd, const DXGI_SWAP_CHAIN_DESC1 &swapChainDesc1, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* fullscreenDesc );
		static DXGI_MODE_DESC1 CopyToModeDesc1( const DXGI_MODE_DESC &modeDesc );
		static DXGI_MODE_DESC CopyToModeDesc( const DXGI_MODE_DESC1 &modeDesc1 );

		void  ReleaseAllObjects();

		bool  InitBBFrom2DTexture
		(
			UINT uiBBIndex,
			__in ID3D10ShaderResourceView *pTexSRV,
			LPCSTR VertexShaderModel,
			LPCSTR PixelShaderModel
		);

		bool  InitBBViaClear( UINT uiBBIndex );

		BOOL  GetSwapChainPtr( IDXGISwapChain **pSwapChainPointer );
};

#endif
