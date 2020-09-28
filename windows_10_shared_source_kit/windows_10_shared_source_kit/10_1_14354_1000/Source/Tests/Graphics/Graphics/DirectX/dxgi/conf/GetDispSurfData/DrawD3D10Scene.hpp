/************************************************************
 *
 *   File: DrawD3D10Scene.hpp
 *   Class declarations for drawing a 2D checker pattern in 3D space.
 *   Copyright (c) Microsoft Corporation, 2007
 *
 *   Program:   GetDispSurfData - API Test for IDXGIOutput::GetDisplaySurfaceData() 
 *   Created:   7/07/07
 *   Author :   RichGr
 *
 ************************************************************/

#pragma once

#include "WrappedDevice.hpp"

#include <dxgi.h>
#include <d3d10.h>
#include <d3dx10.h>


//------------------------------------------------------------------------------------------
// VERTEX is the vertex layout for the shapes created by these classes
//------------------------------------------------------------------------------------------
struct VERTEX
{
    D3DXVECTOR3     pos;
    D3DXVECTOR4     color;
};

//------------------------------------------------------------------------------------------
//
// D3D10 Scene class for drawing a 2D checker pattern on the back buffer in 3D space,
// then calling Present().
//
//------------------------------------------------------------------------------------------

class D3D10Scene 
{
public:
	D3D10Scene();
	~D3D10Scene();

    HRESULT
	Initialize(
        __in    D3DWrappedDevice*  pWrappedD3DDevice,
        __in    IDXGISwapChain*  pSwapChain,
        __in    DXGI_SWAP_CHAIN_DESC&  swapChainDesc
        );

    HRESULT
    Create3DState(
        __in    float       flColorIntensity
        );

	HRESULT  RenderScene();
	HRESULT  PresentScene();
    void     ReleaseViews();
	void     Uninitialize();

protected:

	HRESULT  SetupRenderTarget();

    HRESULT
    SetupInputLayout(
        __in    float       flColorIntensity
        );

private:
    // Member variables
    IDXGISwapChain                     *m_pSwapChain;
    DXGI_SWAP_CHAIN_DESC                m_swapChainDesc;
    ID3D10RenderTargetView             *m_pRenderTargetView;
    ID3D10InputLayout                  *m_pInputLayout;
    ID3D10Effect                       *m_pEffect;
    ID3D10EffectTechnique              *m_pTechnique;
    ID3D10Buffer                       *m_pVertexBuffer;

	D3DWrappedDevice					*m_pWrappedD3DDevice;
};


//------------------------------------------------------------------------------------------
//
// D3D10 VidSurface class for drawing a 2D checker pattern on a vidmem surface in 3D space.
//
//------------------------------------------------------------------------------------------

class D3D10VidSurface 
{
public:
	D3D10VidSurface();
	~D3D10VidSurface();

    HRESULT
	Initialize(
		__in    D3DWrappedDevice*  pWrappedD3DDevice,
        __in    IDXGISwapChain*  pSwapChain,
        __in    DXGI_SWAP_CHAIN_DESC&  swapChainDesc
        );

    HRESULT
    Create3DState(
        __in    UINT        width,
        __in    UINT        height,
        __in    DXGI_FORMAT nFormat,
        __in    float       flColorIntensity
        );

	HRESULT
    RenderScene(
        __in    ID3D10Texture2D   **ppVidMemTex
        );

    void     ReleaseViews();
	void     Uninitialize();

protected:

    HRESULT
    SetupRenderTarget(
        __in    UINT        width,
        __in    UINT        height,
        __in    DXGI_FORMAT nFormat
        );

    HRESULT
    SetupInputLayout(
        __in    float       flColorIntensity
        );

private:
    // Member variables
    IDXGISwapChain                     *m_pSwapChain;
    DXGI_SWAP_CHAIN_DESC                m_swapChainDesc;
    ID3D10Texture2D                    *m_pVidMemTex;
    ID3D10RenderTargetView             *m_pRenderTargetView;
    ID3D10InputLayout                  *m_pInputLayout;
    ID3D10Effect                       *m_pEffect;
    ID3D10EffectTechnique              *m_pTechnique;
    ID3D10Buffer                       *m_pVertexBuffer;

	D3DWrappedDevice					*m_pWrappedD3DDevice;

};
