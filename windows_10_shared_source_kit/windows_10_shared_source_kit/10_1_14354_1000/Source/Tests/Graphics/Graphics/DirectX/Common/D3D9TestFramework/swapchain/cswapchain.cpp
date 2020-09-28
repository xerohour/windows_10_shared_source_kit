#include "SwapChainPCH.h"

#include "CSwapChain.h"
#include "CSurface.h"


// --------------------------------------------------------------------------------------

CSwapChain::CSwapChain()
{
    m_pSwapChain8 = NULL;
    m_pSwapChain9 = NULL;
    m_pBackBuffers = NULL;
    m_nBackBuffers = 0;
}

// --------------------------------------------------------------------------------------

CSwapChain::~CSwapChain()
{
	ReleaseAll();
    ReleaseD3D();
}

// --------------------------------------------------------------------------------------

ULONG WINAPI CSwapChain::AddRef(void)
{
    if(m_pDevice)
		m_pDevice->AddRef();
	return CObject::AddRef();
}

// --------------------------------------------------------------------------------------

ULONG WINAPI CSwapChain::Release(void)
{   
	if(m_pDevice)
		m_pDevice->Release();
	return CObject::Release();
}

// --------------------------------------------------------------------------------------

HRESULT CSwapChain::GetBackBufferCached(UINT BackBuffer, DWORD Type, CSurface** ppBackBuffer)
{
    if (m_pBackBuffers == NULL || BackBuffer >= m_nBackBuffers)
	{
		DPF(1, _T("CSwapChain::GetBackBufferCached() - Invalid params.\n"));
		return E_FAIL;
	}

	m_pBackBuffers[BackBuffer]->AddRef();
    *ppBackBuffer = m_pBackBuffers[BackBuffer];

	return S_OK;
}

// --------------------------------------------------------------------------------------

void CSwapChain::ReleaseAll()
{
   	for(int i=0; i<m_nBackBuffers; i++)
	{
		RELEASE(m_pBackBuffers[i]);
	}
    m_nBackBuffers = 0;

	SAFEDELETEA(m_pBackBuffers);
}

// -------------------------------------------------------------------------------------

void * CSwapChain::GetInternalPointer(void)
{
	if(m_pSwapChain9)
		return m_pSwapChain9;
    else if(m_pSwapChain8)
		return m_pSwapChain8;
	else
		return NULL;
}