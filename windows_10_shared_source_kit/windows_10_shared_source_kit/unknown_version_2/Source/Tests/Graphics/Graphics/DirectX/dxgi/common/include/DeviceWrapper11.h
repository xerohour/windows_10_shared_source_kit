#pragma once


//
// Win32 headers
//

#include <windows.h>

#include <dxgi.h>

#include <d3d11.h>

#include <d3d10_1.h>
#include <d3dx10.h>


//
// STL headers
//

#include <map>


//
// Framework headers
//

#include <WGFTestCore.h>
#include <DXGITestFramework.h>


//
// Project headers
//

#include "D3D10SwapChain.h"


class CDeviceWrapper11
{
	public:
		CDeviceWrapper11
		(
			CTest * pInitLoggingGroup
		);

		virtual ~CDeviceWrapper11();


	public:
		HRESULT WrapDevice
		(
			ID3D11Device * pDevice
		);


		bool IsReadyForTexRender() {return bReadyForTexRender;}

		HRESULT LoadShader
		(
			LPCSTR ShaderCode,
			__out  void **ppShader,
			LPCSTR pFuncName,
			LPCSTR pProfile,
			__out_opt ID3D10Blob ** ppRetainCompiledShader = NULL
		);

		bool CreateTexRenderResources
		(
			LPCSTR VertexShaderModel,
			LPCSTR PixelShaderModel
		);

		void ReleaseTexRenderResources();

		bool RenderTexture
		(
			__in ID3D11RenderTargetView * pRTView,
			__in ID3D11ShaderResourceView * pTexSRV,
			__in const D3D11_VIEWPORT * pD3D11ViewPort,
            __in BOOL renderXR_BIAS
		);

		bool GetFactory
		(
			__out IDXGIFactory ** pFactoryPointer
		);

		BOOL GetDevicePtr
		(
			__inout ID3D11Device ** pDevicePointer
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


	public:
		ID3D11DeviceContext * pContext;


	protected:
		CTest * const pLoggingGroup;

		ID3D11Device * pD3DDevice;

		DXGI_MODE_ROTATION m_RotationMode;


	private:
		friend class CSwapChainWrapper11;

		bool m_bAlternateRender;
		UINT m_nxDivisions;
		UINT m_nyDivisions;
		UINT m_nNumVertices;
		UINT m_nNumIndixes;
		UINT m_nStride;

		bool bReadyForTexRender;

		ID3D11Buffer * m_pVB;
		ID3D11Buffer * m_pIB;
		ID3D11Buffer * m_pVSBuffer;
		ID3D11InputLayout * m_pEL;
		ID3D11PixelShader * m_pPixelShader;
        ID3D11PixelShader *m_pPixelShaderRemoveXR_BIAS;
		ID3D11VertexShader * m_pVertexShader;
		ID3D11SamplerState * m_pSampler;
		ID3D11RasterizerState * m_pRastState;
		ID3D11BlendState * m_pBlendState;
		ID3D11DepthStencilState * m_pDepthState;
};


class CSwapChainWrapper11
{
	private:
		UINT nDisplayDeviceCount;


	protected:
		CTest * const pLoggingGroup;
		CDeviceWrapper11 * const pParentD3DDevice;
		IDXGISwapChain *pSwapChain;


	public:
		CSwapChainWrapper11
		(
			CTest * pInitLoggingGroup,
			CDeviceWrapper11 * pInitParentD3D10Device
		)
		:
			pLoggingGroup( pInitLoggingGroup ),
			pParentD3DDevice( pInitParentD3D10Device ),
			pSwapChain( NULL )
		{
			clsGDIData  GDIData;

			nDisplayDeviceCount = GDIData.GetAttachedDisplayDeviceCount();
		};

		virtual ~CSwapChainWrapper11();


	public:
		BOOL  InitializeSwapChain( DXGI_SWAP_CHAIN_DESC *pSwapChainDesc );
		BOOL  InitializeFullscreenSwapChain( DXGI_SWAP_CHAIN_DESC* pSwapChainDesc );

		void  ReleaseAllObjects();

		bool  InitBBFrom2DTexture
		(
			UINT uiBBIndex,
			__in ID3D11ShaderResourceView *pTexSRV,
			LPCSTR VertexShaderModel,
			LPCSTR PixelShaderModel
		);

		bool  InitBBViaClear( UINT uiBBIndex );

		BOOL  GetSwapChainPtr( IDXGISwapChain **pSwapChainPointer );

};
