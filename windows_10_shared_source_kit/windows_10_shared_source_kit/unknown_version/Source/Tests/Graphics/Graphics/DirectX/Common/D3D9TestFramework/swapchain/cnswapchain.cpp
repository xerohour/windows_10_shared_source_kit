#include "SwapChainPCH.h"

//#define STRICT
#include "CSwapChain.h"
#include "CSurface.h"

CnSwapChain::~CnSwapChain()
{
	if( !m_pSwapChains )
		return;

	for( UINT i= 0; i< m_uDevices; i++ )
	{
		RELEASE( m_pSwapChains[i] );
	}
	delete[] m_pSwapChains;
}

bool CnSwapChain::Init(UINT uDevices)
{
	m_pSwapChains = new CSwapChain*[uDevices];
	
	if( m_pSwapChains == NULL )
		return false;

	m_uDevices = uDevices;
	memset( m_pSwapChains, 0, uDevices * sizeof( CSwapChain*) );
	return true;
}

bool CnSwapChain::GetSwapChain(CSwapChain **ppSwapChain, UINT n)
{
	if( n >= m_uDevices || ppSwapChain == NULL )
		return false;

	m_pSwapChains[n]->AddRef();
	*ppSwapChain = m_pSwapChains[n];
	return true;
}

bool CnSwapChain::SetSwapChain(CSwapChain *pSwapChain, UINT n)
{
	if( n >= m_uDevices )
		return false;

	if( m_pSwapChains[n] != NULL )
		return false;

	m_pSwapChains[n] = pSwapChain;
	return true;
}

bool CnSwapChain::Present(RECT *pSourceRect, RECT **pDestRect, HWND hDstWndOverride, RGNDATA **pDirtyRegion)
{
	bool bRes = true;
	HRESULT hr;

	for(UINT i=0; i<m_uDevices; i++)
	{

		hr = m_pSwapChains[i]->Present(pSourceRect, pDestRect ? pDestRect[i] : NULL, hDstWndOverride, pDirtyRegion ? pDirtyRegion[i] : NULL);
		if(FAILED(hr))
		{
			bRes= false;
		}
	}
	return bRes;
}

bool CnSwapChain::Present(RECT *pSourceRect, RECT **pDestRect, HWND hDstWndOverride, RGNDATA **pDirtyRegion, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr;

	for(UINT i=0; i<m_uDevices; i++)
	{

		hr = m_pSwapChains[i]->Present(pSourceRect, pDestRect ? pDestRect[i] : NULL, hDstWndOverride, pDirtyRegion ? pDirtyRegion[i] : NULL, dwFlags);
		if(FAILED(hr))
		{
			bRes= false;
		}
	}
	return bRes;
}

bool CnSwapChain::GetBackBuffer(UINT BackBuffer,DWORD Type,CnSurface** ppBackBuffer)
{
    bool bRes = true;
    UINT i;
	HRESULT hr;
	LPSURFACE pSurface;

	if( ppBackBuffer == NULL )
		return false;

	LPSURFACES pSurfaces = new CnSurface;
	if( pSurfaces == NULL )
	{
		DPF(1, _T("CD3DTest:GetBackBuffer out of memory error.\n") );
		return false;
	}
	if( !pSurfaces->Init( m_uDevices ) )
	{
		DPF(1, _T("CD3DTest:GetBackBuffer out of memory error.\n") );
		return false;
	}
	
	for (i=0; i<m_uDevices; i++)
	{
		hr = m_pSwapChains[i]->GetBackBuffer(BackBuffer,Type,&pSurface);
		if (FAILED(hr))
		{
			bRes= false;
		}
		else
			pSurfaces->SetSurface( pSurface, i );
	}

	// If Any of the intended calls failed, fail them all
	if( !bRes )
	{
		RELEASE( pSurfaces );
		return bRes;
	}

	*ppBackBuffer = pSurfaces;
	return bRes;
}

bool CnSwapChain::GetBackBufferCached(UINT BackBuffer, DWORD Type, CnSurface** ppBackBuffer)
{
    bool bRes = true;
    UINT i;
	HRESULT hr;
	LPSURFACE pSurface;

	if( ppBackBuffer == NULL )
		return false;

	LPSURFACES pSurfaces = new CnSurface;
	if( pSurfaces == NULL )
	{
		DPF(1, _T("CD3DTest:GetBackBuffer out of memory error.\n") );
		return false;
	}
	if( !pSurfaces->Init( m_uDevices ) )
	{
		DPF(1, _T("CD3DTest:GetBackBuffer out of memory error.\n") );
		return false;
	}
	
	for (i=0; i<m_uDevices; i++)
	{
		
		hr = m_pSwapChains[i]->GetBackBufferCached(BackBuffer,Type,&pSurface);
		if (FAILED(hr))
		{
			bRes= false;
		}
		else
			pSurfaces->SetSurface( pSurface, i );
	}

	// If Any of the intended calls failed, fail them all
	if( !bRes )
	{
		RELEASE( pSurfaces );
		return bRes;
	}

	*ppBackBuffer = pSurfaces;
	return bRes;
}

bool CnSwapChain::GetFrontBuffer(CnSurface* pDestSurface)
{
    bool bRes = true;
    UINT i;
	HRESULT hr;
	LPSURFACE pSurface;

	if( pDestSurface == NULL )
		return false;
	
	for (i=0; i<m_uDevices; i++)
	{
        if ( !pDestSurface->GetSurface(&pSurface, i) )
        {
			bRes= false;
            continue;
		}
		hr = m_pSwapChains[i]->GetFrontBuffer(pSurface);
		if (FAILED(hr))
		{
			bRes= false;
		}
        pSurface->Release();
	}

	return bRes;
}

bool CnSwapChain::GetDisplayMode(DISPLAYMODE* pMode)
{
	for (UINT i=0; i<m_uDevices; i++)
	{
		HRESULT hr = m_pSwapChains[i]->GetDisplayMode(pMode);
		if (SUCCEEDED(hr))
		{
            return true;
		}
	}

    return false;
}

bool CnSwapChain::GetRasterStatus(RASTER_STATUS* pRasterStatus)
{
	for (UINT i=0; i<m_uDevices; i++)
	{
		HRESULT hr = m_pSwapChains[i]->GetRasterStatus(pRasterStatus);
		if (SUCCEEDED(hr))
		{
			return true;
		}
	}

    return false;
}
