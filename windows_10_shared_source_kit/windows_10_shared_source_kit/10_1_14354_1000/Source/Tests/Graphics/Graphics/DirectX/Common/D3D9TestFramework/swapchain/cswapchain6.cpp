#include "SwapChainPCH.h"

#include "CSwapChain.h"
#include "CSurface.h"
#include "D3D.h"


// --------------------------------------------------------------------------------------

bool CSwapChain::Init(IDirectDrawSurface4 **pSurface, PRESENT_PARAMETERS *pParams, CDevice *pDevice)
{
    int i;
	HRESULT hr;
    LPDIRECTDRAWSURFACE4 pSurf = NULL;

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

	pPresent = &CSwapChain::Present6;
    pPresent2 = &CSwapChain::Present9To6;
    pGetBackBuffer = &CSwapChain::GetBackBuffer6;
    pReleaseD3D = &CSwapChain::ReleaseD3D6;
    pGetFrontBuffer = &CSwapChain::GetFrontBuffer6;
    pGetDisplayMode = &CSwapChain::GetDisplayMode6;
    pGetRasterStatus = &CSwapChain::GetRasterStatus6;

	return true;

Error:
	
	ReleaseAll();
	return false;
}

// --------------------------------------------------------------------------------------

HRESULT CSwapChain::GetBackBuffer6(UINT BackBuffer,DWORD dwType, CSurface** ppBackBuffer)
{
	DPF(1, _T("CSwapChain::GetBackBuffer6() - not supported on DX6.\n"));
	return E_FAIL;
}

// --------------------------------------------------------------------------------------

HRESULT CSwapChain::Present6(RECT* pSourceRect,RECT* pDestRect,HWND hDestWindowOverride,RGNDATA* pDirtyRegion)
{

    if (m_pDevice == NULL || m_pDevice->GetPrimary() == NULL || m_pBackBuffers == NULL || m_pBackBuffers[0] == NULL)
	{
		DPF(1, _T("CSwapChain::Present6() - Invalid params.\n"));
		return E_FAIL;
	}

	POINT pt = {0, 0};
	RECT Dest;
	HWND hWnd = hDestWindowOverride == 0 ? m_pDevice->GetPresParams()->hDeviceWindow : hDestWindowOverride;
	ADAPTER *pAdapter = m_pDevice->GetAdapter();
    CSurface* pPrimary = m_pDevice->GetPrimary();

	if(!ClientToScreen(hWnd, &pt))
	{
		DPF(1, _T("CSwapChain::Present6() - ClientToScreen failed.\n"));
		return E_FAIL;
	}

	Dest.left = pDestRect->left + pt.x - pAdapter->rMonitorRect.left;
	Dest.right = pDestRect->right + pt.x - pAdapter->rMonitorRect.left;
	Dest.top = pDestRect->top + pt.y - pAdapter->rMonitorRect.top;
	Dest.bottom = pDestRect->bottom + pt.y - pAdapter->rMonitorRect.top;
	
	return pPrimary->GetD3DSurface6()->Blt(&Dest, m_pBackBuffers[0]->GetD3DSurface6(), pSourceRect, DDBLT_WAIT, NULL);
}

// --------------------------------------------------------------------------------------

HRESULT CSwapChain::Present9To6(RECT* pSourceRect,RECT* pDestRect,HWND hDestWindowOverride,RGNDATA* pDirtyRegion, DWORD dwFlags)
{
    return Present6(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

// --------------------------------------------------------------------------------------

void CSwapChain::ReleaseD3D6()
{
}

// --------------------------------------------------------------------------------------

HRESULT CSwapChain::GetFrontBuffer6(CSurface* pDestSurface)
{
	DPF(1, _T("CSwapChain::GetFrontBuffer6() - not supported on DX6.\n"));
	return E_FAIL;
}

// --------------------------------------------------------------------------------------

HRESULT CSwapChain::GetDisplayMode6(DISPLAYMODE* pMode)
{
	DPF(1, _T("CSwapChain::GetDisplayMode6() - not supported on DX6.\n"));
	return E_FAIL;
}

// --------------------------------------------------------------------------------------

HRESULT CSwapChain::GetRasterStatus6(RASTER_STATUS* pRasterStatus)
{
	DPF(1, _T("CSwapChain::GetRasterStatus6() - not supported on DX6.\n"));
	return E_FAIL;
}
