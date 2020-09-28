//////////////////////////////////////////////////////////////////////
// File:  CD3D11SceneRenderer.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for a Direct3D scene management object.
//
// History:
// 12 Dec 2013  SEdmison  Imported from
//                        windowstest\DWM\Common\DXScene\DXScene.h.
// 12 Aug 2015  SEdmison  Factored into D3D 11 and 12 versions.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Pragmas
//
/////////////////////////////////////////////////////////////////

#pragma once


/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include "GraphicsTAEFTest.hpp"
#include "RenderingTypes.hpp"


/////////////////////////////////////////////////////////////////
//
// Type definitions
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Structure:  D3D_DRIVER_TYPE_PREFERENCE
//
// Purpose:
// Holds a preference as to whether a particular
// Direct3D driver type should be considered at device
// creation.
///////////////////////////////////////////////////////
struct D3D_DRIVER_TYPE_PREFERENCE
{
	D3D_DRIVER_TYPE DriverType;
	bool bConsider;
};


/////////////////////////////////////////////////////////////////
//
// Interfaces
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Class interfaces
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Class:  CD3D11SceneRenderer
//
// Purpose:
// Manages Direct3D 11 and DXGI state necessary for
// rendering simple scenes to a window, core window, or
// composition surface.
///////////////////////////////////////////////////////
class CD3D11SceneRenderer : public CSceneRenderer
{
	//
	// Friend declarations
	//

	friend class CD3D11RenderingTechnique;

	friend class CD3D11ClearRenderingTechnique;
	friend class CD3D11ColorSwatchPatternRenderingTechnique;
	friend class CD3D11ShadedCubeRenderingTechnique;
	friend class CD3D11TexturedPlaneRenderingTechnique;


	//
	// Construction/destruction
	//

	public:
		// Constructors.
		CD3D11SceneRenderer();


		// Destructor.
		virtual ~CD3D11SceneRenderer();


	//
	// Initialization/uninitialization methods
	//

	public:
		// Overload for CreateSwapChain and CreateSwapChainForHwnd
		HRESULT Initialize
		(
			SCENE_TYPE SceneType,
			SWAPCHAIN_TYPE SwapChainType,
			_In_ HWND hWindow,
			bool bRestrictToOutput0,
			UINT BufferCount,
			DXGI_SWAP_EFFECT SwapEffect,
			UINT MaximumFrameLatency
		);

		// Overload for CreateSwapChainForCoreWindow
		HRESULT Initialize
		(
			_In_reads_( SWAPCHAIN_SLOT_COUNT ) SCENE_TYPE SceneTypes[],
			_In_ IUnknown * pWindow,
			int Width,
			int Height,
			bool bRestrictToOutput0,
			UINT BufferCount,
			DXGI_SWAP_EFFECT SwapEffect,
			UINT MaximumFrameLatency
		);

		// Overload for CreateDecodeSwapChainForCompositionSurface and CreateSwapChainForCompositionSurface
		HRESULT Initialize
		(
			SCENE_TYPE SceneType,
			SWAPCHAIN_TYPE SwapChainType,
			_In_ HANDLE hCompositionSurface,
			int Width,
			int Height,
			bool bRestrictToOutput0,
			UINT BufferCount,
			DXGI_SWAP_EFFECT SwapEffect,
			UINT MaximumFrameLatency
		);

	private:
		// Overload that finishes initialization.
		HRESULT Initialize
		(
			_In_reads_( SWAPCHAIN_SLOT_COUNT ) SCENE_TYPE SceneTypes[],
			_In_reads_( SWAPCHAIN_SLOT_COUNT ) SWAPCHAIN_TYPE SwapChainTypes[],
			int Width,
			int Height,
			bool bRestrictToOutput0,
			UINT BufferCount,
			DXGI_SWAP_EFFECT SwapEffect,
			UINT MaximumFrameLatency
		);


	public:
		HRESULT Uninitialize();


		void SetLogger
		(
			_In_ ISimpleLog * pLogger
		);


	//
	// Accessors
	//

	public:
		IDXGISwapChain * GetSwapChain
		(
			SWAPCHAIN_SLOT CurrentSwapChainSlot
		);


	//
	// Rendering and resizing methods
	//

	public:
		HRESULT RecreateSizeDependentResources
		(
			bool bRestrictToOutput0 = false,
			UINT BufferCount = 0
		);

		HRESULT RecreateSizeDependentResources
		(
			UINT NewWidth,
			UINT NewHeight,
			bool bRestrictToOutput0 = false,
			UINT BufferCount = 0
		);

		HRESULT RecreateSizeDependentResources
		(
			_In_reads_( SWAPCHAIN_SLOT_COUNT ) DXGI_SWAP_CHAIN_DESC1 SwapChainDescriptors[],
			_In_reads_( SWAPCHAIN_SLOT_COUNT ) DXGI_DECODE_SWAP_CHAIN_DESC DecodeSwapChainDescriptors[],
			_In_ DXGI_SWAP_CHAIN_FULLSCREEN_DESC * pFullscreenDescriptor,
			bool bRestrictToOutput0 = false
		);


	//
	// Rendering public methods
	//

	public:
		HRESULT RenderAndPresentOnce();

		HRESULT RenderAndPresentOnce
		(
			UINT SyncInterval,
			UINT PresentFlags
		);

		HRESULT RenderAndPresentOnce
		(
			UINT SyncInterval,
			UINT PresentFlags,
			_In_ DXGI_PRESENT_PARAMETERS * pPresentParameters
		);


		void StartAnimation();

		void StopAnimation();


	//
	// Direct3D 11 resource management helper methods
	//

	private:
		HRESULT CreateDepthStencil();

		void ReleaseDepthStencil();


	//
	// DXGI resource management helper methods
	//

	private:
		HRESULT FindSwapChainBufferFormat
		(
			SWAPCHAIN_SLOT CurrentSwapChainSlot,
			_Out_ DXGI_FORMAT * pSwapChainBufferFormat
		);


		HRESULT CreateSwapChainBuffers
		(
			SWAPCHAIN_SLOT CurrentSwapChainSlot,
			DXGI_FORMAT SwapChainBufferFormat,
			UINT Width,
			UINT Height
		);

		void ReleaseSwapChainBuffers();


		HRESULT CreateSwapChains
		(
			_In_reads_( SWAPCHAIN_SLOT_COUNT ) DXGI_SWAP_CHAIN_DESC1 SwapChainDescriptors[],
			_In_reads_( SWAPCHAIN_SLOT_COUNT ) DXGI_DECODE_SWAP_CHAIN_DESC DecodeSwapChainDescriptors[],
			_In_ DXGI_SWAP_CHAIN_FULLSCREEN_DESC * pFullscreenDescriptor,
			bool bRestrictToOutput0
		);

		void ReleaseSwapChains();


	//
	// Rendering helper methods
	//

	private:
		HRESULT ClearRenderTarget
		(
			SWAPCHAIN_SLOT CurrentSwapChainSlot
		);


		HRESULT SetShaderConstants();


	//
	// Static data
	//

	public:
		static D3D_DRIVER_TYPE_PREFERENCE s_D3DDriverTypePreferences[ 6 ];


	private:
		static FLOAT_COLOR s_SwapChainClearColors[ SWAPCHAIN_SLOT_COUNT ];


	//
	// Member data
	//

	private:
		//
		// Logger state
		//

		ISimpleLog * m_pLogger;


		//
		// Window state
		//

		HWND m_hWindow;

		IUnknown * m_pWindow;


		//
		// DirectComposition state
		//

		HANDLE m_hCompositionSurface;


		//
		// Scene state
		//

		SCENE_TYPE m_SceneTypes[ SWAPCHAIN_SLOT_COUNT ];

		UINT m_SceneWidth;
		UINT m_SceneHeight;

		D3D_DRIVER_TYPE m_SelectedD3DDriverType;

		SWAPCHAIN_TYPE m_SwapChainTypes[ SWAPCHAIN_SLOT_COUNT ];

		DXGI_FORMAT m_SwapChainBufferFormats[ SWAPCHAIN_SLOT_COUNT ];

		D3DXMATRIX m_WorldMatrix;
		D3DXMATRIX m_ViewMatrix;
		D3DXMATRIX m_ProjectionMatrix;

		CRITICAL_SECTION m_RenderingGuard;

		UINT m_PresentFlags;

		bool m_bAnimating;


		//
		// Fundamental Direct3D and DXGI state
		//

		IDXGIDebug1 * m_pDXGIDebug_IDXGIDebug1;

		IDXGIFactory4 * m_pFactory_IDXGIFactory4;
		IDXGIFactoryMedia * m_pFactory_IDXGIFactoryMedia;

		IDXGIAdapter1 * m_pAdapter_IDXGIAdapter1;

		IDXGIOutput1 * m_pOutput_IDXGIOutput1;

		ID3D11Device * m_pDevice_ID3D11Device;
		IDXGIDevice1 * m_pDevice_IDXGIDevice1;
		ID3D11VideoDevice * m_pDevice_ID3D11VideoDevice;

		ID3D11DeviceContext1 * m_pImmediateContext_ID3D11DeviceContext1;
		ID3D11VideoContext * m_pImmediateContext_ID3D11VideoContext;

		UINT m_cSwapChainBufferTextureArraySlices[ SWAPCHAIN_SLOT_COUNT ];
		ID3D11Texture2D * m_ppSwapChainBufferTextureArrays[ SWAPCHAIN_SLOT_COUNT ];

		DXGI_SWAP_CHAIN_DESC1 m_LastSwapChainDescriptors[ SWAPCHAIN_SLOT_COUNT ];
		DXGI_DECODE_SWAP_CHAIN_DESC m_LastDecodeSwapChainDescriptors[ SWAPCHAIN_SLOT_COUNT ];
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC m_LastSwapChainFullscreenDescriptor;

		IDXGISwapChain1 * m_ppSwapChains_IDXGISwapChain1[ SWAPCHAIN_SLOT_COUNT ];
		IDXGIDecodeSwapChain * m_ppSwapChains_IDXGIDecodeSwapChain[ SWAPCHAIN_SLOT_COUNT ];
		IDXGISwapChainMedia * m_ppSwapChains_IDXGISwapChainMedia[ SWAPCHAIN_SLOT_COUNT ];


		//
		// Render target state
		//

		ID3D11DepthStencilView * m_pDepthStencilView;


		//
		// Vertex shader constant state
		//

		ID3D11Buffer * m_pVertexShaderConstantBuffer;


		//
		// Rendering techniques
		//

		CD3D11RenderingTechnique * m_ppRenderingTechniques[ SCENE_TYPE_COUNT ];
};
