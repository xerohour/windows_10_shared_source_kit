#include "SwapChainPCH.h"

#include "CSwapChain.h"
#include "CSurface.h"
#include "D3D.h"


// --------------------------------------------------------------------------------------

bool CSwapChain::Init(IDirectDrawSurface7 **pSurface, PRESENT_PARAMETERS *pParams, CDevice *pDevice)
{
    int i;
	HRESULT hr;
    LPDIRECTDRAWSURFACE7 pSurf = NULL;

	if(pSurface == NULL || pParams == NULL || pDevice == NULL)
	{
		DPF(1, _T("CSwapChain::Init() - Invalid params.\n"));
		return false;
	}

    // Release any pre-existing interfaces
	ReleaseAll();

	m_pDevice = pDevice;
    pDevice->AddRef();


	// Allocate the BackBuffer Ptrs
	m_pBackBuffers = new CSurface*[pParams->uBackBufferCount];
	if(m_pBackBuffers == NULL)
	{
		DPF(1, _T("CSwapChain::Init() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Error;
	}
	memset(m_pBackBuffers, 0, sizeof(CSurface*) * pParams->uBackBufferCount);

	// Create the actual buffers
	for(i=0; i<pParams->uBackBufferCount; i++)
	{
		pSurf = pSurface[i];

        m_pBackBuffers[i] = new CSurface();
		if(m_pBackBuffers[i] == NULL)
		{
			DPF(1, _T("CSwapChain::Init() - Out of memory.\n"));
			goto Error;
		}

		if(!m_pBackBuffers[i]->Init(pSurf, m_pDevice, NULL))
		{
			DPF(2, _T("CSwapChain::Init() - pBackBuffer->Init() failed.\n"));
			goto Error;
		}

        m_nBackBuffers++;
	}

	pPresent = &CSwapChain::Present7;
    pPresent2 = &CSwapChain::Present9To7;
    pGetBackBuffer = &CSwapChain::GetBackBuffer7;
	pReleaseD3D = &CSwapChain::ReleaseD3D7;
    pGetFrontBuffer = &CSwapChain::GetFrontBuffer7;
    pGetDisplayMode = &CSwapChain::GetDisplayMode7;
    pGetRasterStatus = &CSwapChain::GetRasterStatus7;

	return true;

Error:
	
	ReleaseAll();
	return false;
}

// --------------------------------------------------------------------------------------

HRESULT CSwapChain::GetBackBuffer7(UINT BackBuffer,DWORD dwType, CSurface** ppBackBuffer)
{
	DPF(1, _T("CSwapChain::GetBackBuffer7() - not supported on DX7.\n"));
	return E_FAIL;
}

// --------------------------------------------------------------------------------------

HRESULT CSwapChain::Present7(RECT* pSourceRect,RECT* pDestRect,HWND hDestWindowOverride,RGNDATA* pDirtyRegion)
{

    if (m_pDevice == NULL || m_pDevice->GetPrimary() == NULL || m_pBackBuffers == NULL || m_pBackBuffers[0] == NULL)
	{
		DPF(1, _T("CSwapChain::Present7() - Invalid params.\n"));
		return E_FAIL;
	}

	POINT pt = {0, 0};
	RECT Dest;
	HWND hWnd = hDestWindowOverride == 0 ? m_pDevice->GetPresParams()->hDeviceWindow : hDestWindowOverride;
	ADAPTER *pAdapter = m_pDevice->GetAdapter();
    CSurface* pPrimary = m_pDevice->GetPrimary();

	if(!ClientToScreen(hWnd, &pt))
	{
		DPF(1, _T("CSwapChain::Present7() - ClientToScreen failed.\n"));
		return E_FAIL;
	}

	Dest.left = pDestRect->left + pt.x - pAdapter->rMonitorRect.left;
	Dest.right = pDestRect->right + pt.x - pAdapter->rMonitorRect.left;
	Dest.top = pDestRect->top + pt.y - pAdapter->rMonitorRect.top;
	Dest.bottom = pDestRect->bottom + pt.y - pAdapter->rMonitorRect.top;
	
	return pPrimary->GetD3DSurface7()->Blt(&Dest, m_pBackBuffers[0]->GetD3DSurface7(), pSourceRect, DDBLT_WAIT, NULL);
}

// --------------------------------------------------------------------------------------

HRESULT CSwapChain::Present9To7(RECT* pSourceRect,RECT* pDestRect,HWND hDestWindowOverride,RGNDATA* pDirtyRegion, DWORD dwFlags)
{
    return Present7(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

// --------------------------------------------------------------------------------------

void CSwapChain::ReleaseD3D7()
{
}

// --------------------------------------------------------------------------------------

HRESULT CSwapChain::GetFrontBuffer7(CSurface* pDestSurface)
{
	DPF(1, _T("CSwapChain::GetFrontBuffer7() - not supported on DX7.\n"));
	return E_FAIL;
}

// --------------------------------------------------------------------------------------

HRESULT CSwapChain::GetDisplayMode7(DISPLAYMODE* pMode)
{
	DPF(1, _T("CSwapChain::GetDisplayMode7() - not supported on DX7.\n"));
	return E_FAIL;
}

// --------------------------------------------------------------------------------------

HRESULT CSwapChain::GetRasterStatus7(RASTER_STATUS* pRasterStatus)
{
	DPF(1, _T("CSwapChain::GetRasterStatus7() - not supported on DX7.\n"));
	return E_FAIL;
}
