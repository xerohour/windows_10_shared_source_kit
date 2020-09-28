#include "SwapChainPCH.h"

#include "CSwapChain.h"
#include "CSurface.h"
#include "D3D9.h"


// --------------------------------------------------------------------------------------

bool CSwapChain::Init(IDirect3DSwapChain9* pSwapChain, PRESENT_PARAMETERS *pParams, CDevice *pDevice)
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
	m_pSwapChain9 = pSwapChain;
	m_pSwapChain9->AddRef();

    pReleaseD3D = &CSwapChain::ReleaseD3D9;
	pPresent = &CSwapChain::Present8To9;
    pPresent2 = &CSwapChain::Present9;
    pGetBackBuffer = &CSwapChain::GetBackBuffer9;
    pGetFrontBuffer = &CSwapChain::GetFrontBuffer9;
    pGetDisplayMode = &CSwapChain::GetDisplayMode9;
    pGetRasterStatus = &CSwapChain::GetRasterStatus9;

    // Wrap all the BackBuffers
    m_pBackBuffers = new CSurface*[pParams->uBackBufferCount];
	if(m_pBackBuffers == NULL)
	{
		DPF(1, _T("CSwapChain::Initialize9() - Out of memory.\n"));
		goto Error;
	}
	memset(m_pBackBuffers, 0, sizeof(CSurface*) * pParams->uBackBufferCount);
	
	for(i=0; i < pParams->uBackBufferCount; i++)
	{
		// Alloc the surface wrapper
        hr = GetBackBuffer(i, BACKBUFFER_TYPE_MONO, &m_pBackBuffers[i]);
		if(FAILED(hr))
		{
			DPF(1, _T("CSwapChain::Initialize9() - GetBackBuffer(%d) failed. hr=%s(%x).\n"),
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

HRESULT CSwapChain::GetBackBuffer9(UINT BackBuffer, DWORD dwType, CSurface** ppBackBuffer)
{
	LPDIRECT3DSURFACE9 pSurf = NULL;
	CSurface* pBackBuffer = NULL;

    if (m_pSwapChain9 == NULL || ppBackBuffer == NULL)
	{
		DPF(1, _T("CSwapChain::GetBackBuffer9() - Invalid Device.\n"));
		return E_FAIL;
	}

	// Alloc the surface wrapper
	pBackBuffer = new CSurface();
	if(pBackBuffer == NULL)
	{
		DPF(1, _T("CSwapChain::GetBackBuffer9() - Out of memory.\n"));
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
    HRESULT hr = m_pSwapChain9->GetBackBuffer(BackBuffer, Type, &pSurf);
	if(FAILED(hr))
	{
		DPF(1, _T("CSwapChain::GetBackBuffer9() - GetBackBuffer(%d) failed. hr=%s(%x).\n"),
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
		DPF(1, _T("CSwapChain::GetBackBuffer9() - CSurface::Init failed for backbuffer #%d.\n"), BackBuffer);
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

HRESULT CSwapChain::Present8To9(RECT* pSourceRect, RECT* pDestRect, HWND hDestWindowOverride, RGNDATA* pDirtyRegion)
{
    if (m_pSwapChain9 == NULL)
	{
		DPF(1, _T("CSwapChain::Present9() - Invalid SwapChain.\n"));
		return E_FAIL;
	}

    return m_pSwapChain9->Present(pSourceRect,pDestRect,hDestWindowOverride,pDirtyRegion, 0);
}

// --------------------------------------------------------------------------------------

HRESULT CSwapChain::Present9(RECT* pSourceRect, RECT* pDestRect, HWND hDestWindowOverride, RGNDATA* pDirtyRegion, DWORD dwFlags)
{
    if (m_pSwapChain9 == NULL)
	{
		DPF(1, _T("CSwapChain::Present9() - Invalid SwapChain.\n"));
		return E_FAIL;
	}

    return m_pSwapChain9->Present(pSourceRect,pDestRect,hDestWindowOverride,pDirtyRegion, dwFlags);
}

// --------------------------------------------------------------------------------------

void CSwapChain::ReleaseD3D9()
{
	RELEASE(m_pSwapChain9);
}

// --------------------------------------------------------------------------------------

HRESULT CSwapChain::GetFrontBuffer9(CSurface* pDestSurface)
{
    if (m_pSwapChain9 == NULL)
	{
		DPF(1, _T("CSwapChain::GetFrontBuffer9() - Invalid SwapChain.\n"));
		return E_FAIL;
	}

    return m_pSwapChain9->GetFrontBufferData(pDestSurface->GetD3DSurface9());
}

// --------------------------------------------------------------------------------------

HRESULT CSwapChain::GetDisplayMode9(DISPLAYMODE* pMode)
{
    if (m_pSwapChain9 == NULL)
	{
		DPF(1, _T("CSwapChain::GetDisplayMode9() - Invalid SwapChain.\n"));
		return E_FAIL;
	}

	D3DDISPLAYMODE Mode;

	HRESULT hr=m_pSwapChain9->GetDisplayMode(pMode != NULL ? &Mode : NULL);
	
	if( pMode != NULL )
	{
		pMode->dwWidth  = Mode.Width;
		pMode->dwHeight = Mode.Height;
		pMode->dwRefreshRate = Mode.RefreshRate;
		pMode->Format.d3dfFormat = Mode.Format;
		d3df2ddpf(Mode.Format,&pMode->Format.ddpfPixelFormat);
		pMode->dwBPP = pMode->Format.ddpfPixelFormat.dwRGBBitCount;
	}

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CSwapChain::GetRasterStatus9(RASTER_STATUS* pRasterStatus)
{
    if (m_pSwapChain9 == NULL)
	{
		DPF(1, _T("CSwapChain::GetRasterStatus9() - Invalid SwapChain.\n"));
		return E_FAIL;
	}

	return m_pSwapChain9->GetRasterStatus(pRasterStatus);
}
