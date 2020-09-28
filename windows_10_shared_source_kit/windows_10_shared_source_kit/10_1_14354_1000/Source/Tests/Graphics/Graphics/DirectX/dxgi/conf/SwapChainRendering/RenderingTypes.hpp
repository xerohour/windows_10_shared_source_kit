//////////////////////////////////////////////////////////////////////
// File:  RenderingTypes.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Type definitions and constants related to Direct3D rendering.
//
// History:
// 12 Aug 2015  SEdmison  Factoured out of CSceneRenderer.hpp.
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
// Win32 headers
//

#include <windows.h>

// DirectX headers
#include <DXGI1_4.h>

#include <D3D12.h>
#include <D3DX12.h>

#include <D3D11_1.h>
#include <D3DX11Async.h>

#if COMPILE_SHADERS_AT_RUNTIME
#include <D3DCompiler.h>
#endif

#include <D3DCommon.h>

#pragma prefast( push )
#pragma prefast( disable: 28922 )
#include <D3DX10Math.h>
#pragma prefast( pop )


//
// Test infrastructure headers
//

#include <ISimpleLog.h>

#if USE_DWMSLICER
#include <FormatHelper.h>
#include <FormatDesc.hpp>
#endif


/////////////////////////////////////////////////////////////////
//
// Type definitions
//
/////////////////////////////////////////////////////////////////

#if USE_DWMSLICER
// FLOAT_COLOR is defined in FormatHelper.h.
#else
///////////////////////////////////////////////////////
// Structure:  FLOAT_COLOR
//
// Purpose:
// Holds a four-floating-point-channel color value.
///////////////////////////////////////////////////////
struct FLOAT_COLOR
{
	float r;
	float g;
	float b;
	float a;
};
#endif


///////////////////////////////////////////////////////
// Structure:  FLOAT_RECT
//
// Purpose:
// Holds a four-floating-point-coordinate rectangle.
///////////////////////////////////////////////////////
struct FLOAT_RECT
{
	float left;
	float top;
	float right;
	float bottom;
};


///////////////////////////////////////////////////////
// Structure:  COLOR_SWATCH
//
// Purpose:
// Holds information about a two-dimensional swatch of
// color.
///////////////////////////////////////////////////////
struct COLOR_SWATCH
{
	FLOAT_RECT RelativeRectangle;
	FLOAT_COLOR Color;
};


///////////////////////////////////////////////////////
// Enum:  SCENE_TYPE
//
// Purpose:
// Identifies the type of scene to be rendered.
///////////////////////////////////////////////////////
enum SCENE_TYPE
{
	SCENE_TYPE_CUBE_SHADED = 0,

	SCENE_TYPE_PLANE_TEXTURED = 1,

	SCENE_TYPE_SWATCH_FIXEDCOLOR = 2,
	SCENE_TYPE_SWATCH_RANDOMCOLOR = 3,
	SCENE_TYPE_SWATCH_TRANSPARENT = 4,

	SCENE_TYPE_PATTERN = 5,

	SCENE_TYPE_NONE = 6,

	SCENE_TYPE_FIRST = SCENE_TYPE_CUBE_SHADED,
	SCENE_TYPE_COUNT = SCENE_TYPE_NONE + 1
};


///////////////////////////////////////////////////////
// Enum:  SWAPCHAIN_SLOT
//
// Purpose:
// Named index into arrays of swap chains, allowing for
// a default or background swap chain slot and an
// optional foreground swap chain slot.
//
// Note:
// Foreground swap chains can currently only be created
// via CreateSwapChainForCoreWindow.
///////////////////////////////////////////////////////
enum SWAPCHAIN_SLOT
{
	SWAPCHAIN_SLOT_DEFAULT = 0,
	SWAPCHAIN_SLOT_BACKGROUND = SWAPCHAIN_SLOT_DEFAULT,

	SWAPCHAIN_SLOT_FOREGROUND = 1,

	SWAPCHAIN_SLOT_FIRST = SWAPCHAIN_SLOT_DEFAULT,
	SWAPCHAIN_SLOT_COUNT = SWAPCHAIN_SLOT_FOREGROUND + 1
};


///////////////////////////////////////////////////////
// Enum:  SWAPCHAIN_TYPE
//
// Purpose:
// Identifies the type of a swap chain in terms of the
// method by which it was created.
///////////////////////////////////////////////////////
enum SWAPCHAIN_TYPE
{
	SWAPCHAIN_TYPE_NONE = 0,

	SWAPCHAIN_TYPE_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACEHANDLE = 1,
	SWAPCHAIN_TYPE_CREATESWAPCHAIN = 2,
	SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOMPOSITIONSURFACEHANDLE = 3,
	SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOREWINDOW = 4,
	SWAPCHAIN_TYPE_CREATESWAPCHAINFORHWND = 5,

	SWAPCHAIN_TYPE_FIRST = SWAPCHAIN_TYPE_NONE,
	SWAPCHAIN_TYPE_COUNT = SWAPCHAIN_TYPE_CREATESWAPCHAINFORHWND + 1
};


///////////////////////////////////////////////////////
// Enum:  TEST_API
//
// Purpose:
// Specifies the API to test (and thus which type of
// scene renderer to use) for a given test case.
///////////////////////////////////////////////////////
#if 1
// N.B. [SEdmison]:
// Apparently this pattern leads PREfast to think that
// there is some possibility of underflow when indexing
// into an array of TEST_API_COUNT items, even if only
// valid TEST_API enumerants are used as indices.
// However, using the UINT approach below leaves me
// without a distinct type on which to hang FromString
// parsing.
enum TEST_API
{
	TEST_API_D3D11 = 0,
	TEST_API_D3D12 = 1,

	TEST_API_FIRST = TEST_API_D3D11,
	TEST_API_COUNT = TEST_API_D3D12 + 1
};
#else
const UINT TEST_API_D3D11 = 0;
const UINT TEST_API_D3D12 = 1;
const UINT TEST_API_FIRST = TEST_API_D3D11;
const UINT TEST_API_COUNT = TEST_API_D3D12 + 1;

typedef UINT TEST_API;
#endif


///////////////////////////////////////////////////////
// Struct:  VertexShaderMatrices
//
// Purpose:
// Holds world, view, and projection matrices for
// access by a vertex shader.
///////////////////////////////////////////////////////
struct VertexShaderMatrices
{
	D3DXMATRIX m_World;
	D3DXMATRIX m_View;
	D3DXMATRIX m_Projection;
};


/////////////////////////////////////////////////////////////////
//
// Helper functions
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  Clamp
//
// Purpose:
// Clamps the specified value to the range [min, max]
// and returns the result.
///////////////////////////////////////////////////////
inline float Clamp
(
	float Value,
	float Minimum,
	float Maximum
)
{
	// Clamp the value between max and min.
	return ( Value < Minimum )
		? Minimum
		:
			(
				( Value > Maximum )
					? Maximum
					: Value
			);
};


///////////////////////////////////////////////////////
// Function:  InitializeClearViewColor
//
// Purpose:
// Initializes an array of 4 floating-point color
// components to be used as a render-target clear
// color.
///////////////////////////////////////////////////////
HRESULT InitializeClearViewColor
(
	DXGI_FORMAT Format,
	DXGI_ALPHA_MODE AlphaMode,
	_In_ const FLOAT_COLOR & NormalizedColorWithStraightAlpha,
	_Out_writes_( 4 ) float * ClearColor
);


/////////////////////////////////////////////////////////////////
//
// Interfaces
//
/////////////////////////////////////////////////////////////////

#ifndef PURE
#define PURE = 0
#endif


class CSceneRenderer;
class CD3D11SceneRenderer;
class CD3D12SceneRenderer;


///////////////////////////////////////////////////////
// Class:  CRenderingTechnique
//
// Purpose:
// Base class for rendering techniques for use in
// conjunction with CSceneRenderer-derived classes.
///////////////////////////////////////////////////////
class CRenderingTechnique
{
	//
	// Construction/destruction.
	//

	public:
		CRenderingTechnique
		(
			_In_ ISimpleLog * pLogger
		);

		virtual ~CRenderingTechnique();


	//
	// Public methods.
	//

	public:
		virtual HRESULT CreateResources
		(
			SWAPCHAIN_SLOT CurrentSwapChainSlot
		) PURE;

		virtual void DestroyResources() PURE;


		virtual void SetLogger
		(
			_In_ ISimpleLog * pLogger
		);


	//
	// Member data
	//

	protected:
		ISimpleLog * m_pLogger;
};


///////////////////////////////////////////////////////
// Class:  CD3D11RenderingTechnique
//
// Purpose:
// Base class for Direct3D 11 rendering techniques for
// use in conjunction with the CD3D11SceneRenderer
// class.
///////////////////////////////////////////////////////
class CD3D11RenderingTechnique : public CRenderingTechnique
{
	//
	// Construction/destruction
	//

	public:
		CD3D11RenderingTechnique
		(
			_In_ CD3D11SceneRenderer * pParentRenderer
		);


	//
	// Public methods
	//

	public:
		virtual HRESULT RenderScene
		(
			SWAPCHAIN_SLOT CurrentSwapChainSlot
		) PURE;


	//
	// Helper methods
	//

	protected:
		virtual HRESULT InitializeSwapChainBuffers
		(
			SWAPCHAIN_SLOT CurrentSwapChainSlot,
			DXGI_FORMAT SwapChainBufferFormat,
			DXGI_ALPHA_MODE AlphaMode
		);


	//
	// Member data
	//

	protected:
		CD3D11SceneRenderer * m_pParentRenderer;
};


///////////////////////////////////////////////////////
// Class:  CD3D12RenderingTechnique
//
// Purpose:
// Base class for Direct3D 12 rendering techniques for
// use in conjunction with the CD3D12SceneRenderer
// class.
///////////////////////////////////////////////////////
class CD3D12RenderingTechnique : public CRenderingTechnique
{
	//
	// Construction/destruction
	//

	public:
		CD3D12RenderingTechnique
		(
			_In_ CD3D12SceneRenderer * pParentRenderer
		);


	//
	// Public methods.
	//

	public:
		virtual HRESULT RenderScene
		(
			SWAPCHAIN_SLOT CurrentSwapChainSlot,
			_In_ ID3D12Resource * pCurrentSwapChainBuffer
		) PURE;


	//
	// Helper methods
	//

	protected:
		virtual HRESULT InitializeSwapChainBuffers
		(
			SWAPCHAIN_SLOT CurrentSwapChainSlot,
			DXGI_FORMAT SwapChainBufferFormat,
			DXGI_ALPHA_MODE AlphaMode
		);


	//
	// Member data
	//

	protected:
		CD3D12SceneRenderer * m_pParentRenderer;
};


///////////////////////////////////////////////////////
// Class:  CSceneRenderer
//
// Purpose:
// Base class for Direct3D scene renderers.
///////////////////////////////////////////////////////
class CSceneRenderer
{
	//
	// Construction/destruction.
	//

	public:
		virtual ~CSceneRenderer()
		{
		};


	//
	// Public methods.
	//

	public:
	//
	// Initialization/uninitialization methods
	//

	public:
		// Overload for CreateSwapChain and CreateSwapChainForHwnd
		virtual HRESULT Initialize
		(
			SCENE_TYPE SceneType,
			SWAPCHAIN_TYPE SwapChainType,
			_In_ HWND hWindow,
			bool bRestrictToOutput0,
			UINT BufferCount,
			DXGI_SWAP_EFFECT SwapEffect,
			UINT MaximumFrameLatency
		) PURE;

		// Overload for CreateSwapChainForCoreWindow
		virtual HRESULT Initialize
		(
			_In_reads_( SWAPCHAIN_SLOT_COUNT ) SCENE_TYPE SceneTypes[],
			_In_ IUnknown * pWindow,
			int Width,
			int Height,
			bool bRestrictToOutput0,
			UINT BufferCount,
			DXGI_SWAP_EFFECT SwapEffect,
			UINT MaximumFrameLatency
		) PURE;

		// Overload for CreateDecodeSwapChainForCompositionSurface and CreateSwapChainForCompositionSurface
		virtual HRESULT Initialize
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
		) PURE;


		virtual HRESULT Uninitialize() PURE;



	//
	// Accessors
	//

	public:
		virtual IDXGISwapChain * GetSwapChain
		(
			SWAPCHAIN_SLOT CurrentSwapChainSlot
		) PURE;


	//
	// Rendering and resizing methods
	//

	public:
		virtual HRESULT RecreateSizeDependentResources
		(
			bool bRestrictToOutput0 = false,
			UINT BufferCount = 0
		) PURE;

		virtual HRESULT RecreateSizeDependentResources
		(
			UINT NewWidth,
			UINT NewHeight,
			bool bRestrictToOutput0 = false,
			UINT BufferCount = 0
		) PURE;

		virtual HRESULT RecreateSizeDependentResources
		(
			_In_reads_( SWAPCHAIN_SLOT_COUNT ) DXGI_SWAP_CHAIN_DESC1 SwapChainDescriptors[],
			_In_reads_( SWAPCHAIN_SLOT_COUNT ) DXGI_DECODE_SWAP_CHAIN_DESC DecodeSwapChainDescriptors[],
			_In_ DXGI_SWAP_CHAIN_FULLSCREEN_DESC * pFullscreenDescriptor,
			bool bRestrictToOutput0 = false
		) PURE;


		virtual HRESULT RenderAndPresentOnce() PURE;

		virtual HRESULT RenderAndPresentOnce
		(
			UINT SyncInterval,
			UINT PresentFlags
		) PURE;

		virtual HRESULT RenderAndPresentOnce
		(
			UINT SyncInterval,
			UINT PresentFlags,
			_In_ DXGI_PRESENT_PARAMETERS * pPresentParameters
		) PURE;


		virtual void StartAnimation() PURE;

		virtual void StopAnimation() PURE;
};


/////////////////////////////////////////////////////////////////
//
// Constants
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Constant:  DEFAULT_RENDER_TARGET_BLEND_DESCRIPTOR
//
// Purpose:
// Default render target blend descriptor (since in
// Direct3D 12, just starting with a structure that
// has been zero-initialized is no longer sufficient
// to get default behavior.
///////////////////////////////////////////////////////
const D3D12_RENDER_TARGET_BLEND_DESC DEFAULT_RENDER_TARGET_BLEND_DESCRIPTOR =
{
	FALSE,
	FALSE,
	D3D12_BLEND_ONE,
	D3D12_BLEND_ZERO,
	D3D12_BLEND_OP_ADD,
	D3D12_BLEND_ONE,
	D3D12_BLEND_ZERO,
	D3D12_BLEND_OP_ADD,
	D3D12_LOGIC_OP_NOOP,
	D3D12_COLOR_WRITE_ENABLE_ALL,
};


///////////////////////////////////////////////////////
// Constant:  DEFAULT_DEPTH_STENCIL_OPERATION
//
// Purpose:
// Default depth stencil operation (since in Direct3D
// 12, just starting with a structure that has been
// zero-initialized is no longer sufficient to get
// default behavior.
///////////////////////////////////////////////////////
const D3D12_DEPTH_STENCILOP_DESC DEFAULT_STENCIL_OPERATION =
{
	D3D12_STENCIL_OP_KEEP,
	D3D12_STENCIL_OP_KEEP,
	D3D12_STENCIL_OP_KEEP,
	D3D12_COMPARISON_FUNC_ALWAYS
};


///////////////////////////////////////////////////////
// Constant:  MIN_DEPTH
//
// Purpose:
// Minimum depth for the z-axis in the view frustum.
///////////////////////////////////////////////////////
const float MIN_DEPTH = 0.f;

///////////////////////////////////////////////////////
// Constant:  MAX_DEPTH
//
// Purpose:
// Maximum depth for the z-axis in the view frustum.
///////////////////////////////////////////////////////
const float MAX_DEPTH = 1000.f;


///////////////////////////////////////////////////////
// Constant:  RENDER_TIME
//
// Purpose:
// The amount of time in milliseconds during which each
// test case should render and present.
///////////////////////////////////////////////////////
const DWORD RENDER_TIME = 2000;

///////////////////////////////////////////////////////
// Constant:  RENDER_TIME
//
// Purpose:
// The amount of time during which each test case
// should render and present when a debugger is
// attached.  This time may be in milliseconds, or may
// be the constant INFINITE for the purpose of
// troubleshooting threading issues.
///////////////////////////////////////////////////////
const DWORD RENDER_TIME_UNDER_DEBUGGER = 2000;
