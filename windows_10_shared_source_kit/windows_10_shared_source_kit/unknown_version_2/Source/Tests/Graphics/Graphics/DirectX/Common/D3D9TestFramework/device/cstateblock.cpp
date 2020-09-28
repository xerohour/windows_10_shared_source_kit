#include "DevicePCH.h"

#include "CDevice.h"
#include "D3D9.h"

// -------------------------------------------------------------------------------------

CStateblock::CStateblock()
{
	m_dwStateblock 	= 0;
	m_pStateblock9 	= NULL;
    m_pDevice		= NULL;
}

// -------------------------------------------------------------------------------------

CStateblock::~CStateblock()
{
	RELEASE(m_pStateblock9);
}

// -------------------------------------------------------------------------------------

bool CStateblock::Init(PVOID pStateblock, CDevice *pDevice)
{
	bool bRet = false;

	if(NULL == pDevice)
		return false;

	m_pDevice = pDevice;
	m_pDevice->AddRef();

	if(m_pDevice->GetInterface() >= 9.f && m_pDevice->GetInterface() < 10.f)
	{
		pApply = &CStateblock::Apply9;
        pCapture = &CStateblock::Capture9;
        pGetDevice = &CStateblock::GetDevice9;
		bRet = Init9(pStateblock);
	}
	else if(m_pDevice->GetInterface() >= 7.f && m_pDevice->GetInterface() <= 8.1f)
	{
		pApply = &CStateblock::ApplyUnsup;
        pCapture = &CStateblock::CaptureUnsup;
        pGetDevice = &CStateblock::GetDeviceUnsup;
		m_dwStateblock = (DWORD)((UINT_PTR)pStateblock);
		bRet = true;
	}

	return bRet;
}

ULONG WINAPI CStateblock::AddRef(void)
{
    if(m_pDevice)
		m_pDevice->AddRef();
	return CObject::AddRef();
}

// --------------------------------------------------------------------------------------

ULONG WINAPI CStateblock::Release(void)
{   
	if(m_pDevice)
		m_pDevice->Release();
	return CObject::Release();
}

// -------------------------------------------------------------------------------------
// For now, we'll do everything in one file so we don't explode with tiny CPPs.

bool CStateblock::Init9(PVOID pStateblock)
{
	if(NULL == pStateblock)
		return false;

	m_pStateblock9 = (LPDIRECT3DSTATEBLOCK9)pStateblock;
	m_pStateblock9->AddRef();

	return true;
}

// -------------------------------------------------------------------------------------

HRESULT CStateblock::Apply9()
{
	return m_pStateblock9->Apply();
}

// -------------------------------------------------------------------------------------

HRESULT CStateblock::Capture9()
{
	return m_pStateblock9->Capture();
}

// -------------------------------------------------------------------------------------

HRESULT CStateblock::GetDevice9(PVOID *pDevice)
{
	return m_pStateblock9->GetDevice((LPDIRECT3DDEVICE9*)pDevice);
}

// -------------------------------------------------------------------------------------

HRESULT CStateblock::ApplyUnsup()
{
	DPF(1, "CStateblock: Apply() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
    return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CStateblock::CaptureUnsup()
{
	DPF(1, "CStateblock: Capture() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
    return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CStateblock::GetDeviceUnsup(PVOID *pDevice)
{
	DPF(1, "CStateblock: GetDevice() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
    return E_NOTIMPL;
}
