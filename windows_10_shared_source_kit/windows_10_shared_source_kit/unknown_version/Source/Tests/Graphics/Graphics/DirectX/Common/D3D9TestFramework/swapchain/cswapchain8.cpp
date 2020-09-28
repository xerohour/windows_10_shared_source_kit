#include "SwapChainPCH.h"

#include "CSwapChain.h"
#include "CSurface.h"
#include "D3D8.h"


// --------------------------------------------------------------------------------------

bool CSwapChain::Init(IDirect3DSwapChain8* pSwapChain, PRESENT_PARAMETERS *pParams, CDevice *pDevice)
{
	HRESULT hr;
    int i;

	if(pSwapChain == NULL || pParams == NULL || pDevice == NULL)
	{
		DPF(1, _T("CSwapChain::Init() - Invalid params.\n"));
		return false;
	}

    // Release any pre-existing interfaces
	ReleaseAll();

    m_pDevice = pDevice;
	pDevice->AddRef();
	m_pSwapChain8 = pSwapChain;
	m_pSwapChain8->AddRef();

    pReleaseD3D = &CSwapChain::ReleaseD3D8;
	pPresent = &CSwapChain::Present8;
    pPresent2 = &CSwapChain::Present9To8;
    pGetBackBuffer = &CSwapChain::GetBackBuffer8;
    pGetFrontBuffer = &CSwapChain::GetFrontBuffer8;
    pGetDisplayMode = &CSwapChain::GetDisplayMode8;
    pGetRasterStatus = &CSwapChain::GetRasterStatus8;

    // Wrap all the BackBuffers
    m_pBackBuffers = new CSurface*[pParams->uBackBufferCount];
	if(m_pBackBuffers == NULL)
	{
		DPF(1, _T("CSwapChain::Initialize8() - Out of memory.\n"));
		goto Error;
	}
	memset(m_pBackBuffers, 0, sizeof(CSurface*) * pParams->uBackBufferCount);
	
	for(i=0; i < pParams->uBackBufferCount; i++)
	{
		// Alloc the surface wrapper
        hr = GetBackBuffer(i, BACKBUFFER_TYPE_MONO, &m_pBackBuffers[i]);
		if(FAILED(hr))
		{
			DPF(1, _T("CSwapChain::Initialize8() - GetBackBuffer(%d) failed. hr=%s(%x).\n"),
				i, m_pDevice->GetD3D()->HResultToString(hr), hr);
			goto Error;
		}

		// Increment the buffer counter
		m_nBackBuffers++;
	}

	return true;

Error:
	ReleaseAll();
	return false;
}

// --------------------------------------------------------------------------------------

HRESULT CSwapChain::GetBackBuffer8(UINT BackBuffer, DWORD dwType, CSurface** ppBackBuffer)
{
	LPDIRECT3DSURFACE8 pSurf = NULL;
	CSurface* pBackBuffer = NULL;

    if (m_pSwapChain8 == NULL || ppBackBuffer == NULL)
	{
		DPF(1, _T("CSwapChain::GetBackBuffer8() - Invalid Device.\n"));
		return E_FAIL;
	}

	// Alloc the surface wrapper
	pBackBuffer = new CSurface();
	if(pBackBuffer == NULL)
	{
		DPF(1, _T("CSwapChain::GetBackBuffer8() - Out of memory.\n"));
		return E_FAIL;
	}

	D3DBACKBUFFER_TYPE Type;
	switch( dwType )
	{
	case BACKBUFFER_TYPE_MONO:
		Type = D3DBACKBUFFER_TYPE_MONO;
		break;
	case BACKBUFFER_TYPE_LEFT:
		Type = D3DBACKBUFFER_TYPE_LEFT;
		break;
	case BACKBUFFER_TYPE_RIGHT:
		Type = D3DBACKBUFFER_TYPE_RIGHT;
		break;
	default:
		Type = (D3DBACKBUFFER_TYPE)0;
		break;
	}
	// Get the real backbuffer ptr
    HRESULT hr = m_pSwapChain8->GetBackBuffer(BackBuffer, Type, &pSurf);
	if(FAILED(hr))
	{
		DPF(1, _T("CSwapChain::GetBackBuffer8() - GetBackBuffer(%d) failed. hr=%s(%x).\n"),
			BackBuffer, m_pDevice->GetD3D()->HResultToString(hr), hr);
		goto Error;
	}

	// Initialize the surface with the actual surface ptr
	if(pBackBuffer->Init(pSurf, m_pDevice, NULL))
	{
		// we succeeded!
		if( ppBackBuffer )
			*ppBackBuffer = pBackBuffer;
	}
	else
	{
		DPF(1, _T("CSwapChain::GetBackBuffer8() - CSurface::Init failed for backbuffer #%d.\n"), BackBuffer);
		hr = E_FAIL;
		goto Error;
	}

	// Release our BackBuffer
	RELEASE(pSurf);

	return S_OK;

Error:
	RELEASE(pBackBuffer);
	RELEASE(pSurf);
    return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CSwapChain::Present8(RECT* pSourceRect, RECT* pDestRect, HWND hDestWindowOverride, RGNDATA* pDirtyRegion)
{
    if (m_pSwapChain8 == NULL)
	{
		DPF(1, _T("CSwapChain::Present8() - Invalid SwapChain.\n"));
		return E_FAIL;
	}

    return m_pSwapChain8->Present(pSourceRect,pDestRect,hDestWindowOverride,pDirtyRegion);
}

// --------------------------------------------------------------------------------------

HRESULT CSwapChain::Present9To8(RECT* pSourceRect,RECT* pDestRect,HWND hDestWindowOverride,RGNDATA* pDirtyRegion, DWORD dwFlags)
{
    return Present8(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

// --------------------------------------------------------------------------------------

void CSwapChain::ReleaseD3D8()
{
	RELEASE(m_pSwapChain8);
}

// --------------------------------------------------------------------------------------

HRESULT CSwapChain::GetFrontBuffer8(CSurface* pDestSurface)
{
	DPF(1, _T("CSwapChain::GetFrontBuffer8() - not supported on DX8.\n"));
	return E_FAIL;
}

// --------------------------------------------------------------------------------------

HRESULT CSwapChain::GetDisplayMode8(DISPLAYMODE* pMode)
{
	DPF(1, _T("CSwapChain::GetDisplayMode8() - not supported on DX8.\n"));
	return E_FAIL;
}

// --------------------------------------------------------------------------------------

HRESULT CSwapChain::GetRasterStatus8(RASTER_STATUS* pRasterStatus)
{
	DPF(1, _T("CSwapChain::GetRasterStatus8() - not supported on DX8.\n"));
	return E_FAIL;
}
