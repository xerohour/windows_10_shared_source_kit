#include "BuffersPCH.h"

#include "CBuffers.h"
#include "CDevice.h"

// --------------------------------------------------------------------------------------


CIndexBuffer::CIndexBuffer()
{
	m_pDevice = NULL;
	m_pIB8 = NULL;
    m_pIB9 = NULL;
	pReleaseD3D = NULL;
}

// --------------------------------------------------------------------------------------

CIndexBuffer::~CIndexBuffer()
{
	if( pReleaseD3D )
		ReleaseD3D();
}

// --------------------------------------------------------------------------------------

ULONG WINAPI CIndexBuffer::AddRef(void)
{
    if(m_pDevice)
		m_pDevice->AddRef();
	return CObject::AddRef();
}

// --------------------------------------------------------------------------------------

ULONG WINAPI CIndexBuffer::Release(void)
{
    if(m_pDevice)
		m_pDevice->Release();
	return CObject::Release();
}

// --------------------------------------------------------------------------------------

bool CIndexBuffer::Initialize(LPVOID pIB, CDevice *pDevice)
{
	bool bRet = false;

	if (NULL == pDevice || NULL == pIB)
		return false;

	m_pDevice = pDevice;
	m_pDevice->AddRef();

	// Initialize function pointers
	if (8.0f <= m_pDevice->GetInterface() && 9.0f > m_pDevice->GetInterface())
	{
		pLock = &CIndexBuffer::Lock8;
		pUnlock = &CIndexBuffer::Unlock8;
		pSetPriority = &CIndexBuffer::SetPriority8;
		pGetPriority = &CIndexBuffer::GetPriority8;
		pPreLoad = &CIndexBuffer::PreLoad8;
		pGetSize = &CIndexBuffer::GetSize8;
		pGetDesc = &CIndexBuffer::GetDesc8;
        pReleaseD3D = &CIndexBuffer::ReleaseD3D8;
		bRet = Init8(pIB);
	}
	else if (9.0f <= m_pDevice->GetInterface() && 10.0f > m_pDevice->GetInterface())
	{
		pLock = &CIndexBuffer::Lock9;
		pUnlock = &CIndexBuffer::Unlock9;
		pGetSize = &CIndexBuffer::GetSize9;
		pGetDesc = &CIndexBuffer::GetDesc9;
        pReleaseD3D = &CIndexBuffer::ReleaseD3D9;
		pSetPriority = &CIndexBuffer::SetPriority9;
		pGetPriority = &CIndexBuffer::GetPriority9;
		pPreLoad = &CIndexBuffer::PreLoad9;
		bRet = Init9(pIB);
	}
	

    GetDesc(&m_Desc);

	return bRet;
}

// --------------------------------------------------------------------------------------

HRESULT CIndexBuffer::LockNotImplemented(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags)
{
	DPF(1, _T("CIndexBuffer not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// --------------------------------------------------------------------------------------

HRESULT CIndexBuffer::UnlockNotImplemented()
{
	DPF(1, _T("CIndexBuffer not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// --------------------------------------------------------------------------------------

DWORD CIndexBuffer::SetPriorityNotImplemented(DWORD dwNewPriority)
{
	DPF(1, _T("CIndexBuffer not implemented in this interface version\n"));
	return 0;
}

// --------------------------------------------------------------------------------------

DWORD CIndexBuffer::GetPriorityNotImplemented()
{
	DPF(1, _T("CIndexBuffer not implemented in this interface version\n"));
	return 0;
}

// --------------------------------------------------------------------------------------

void CIndexBuffer::PreLoadNotImplemented()
{
	DPF(1, _T("CIndexBuffer not implemented in this interface version\n"));
}

// --------------------------------------------------------------------------------------

UINT CIndexBuffer::GetSizeNotImplemented()
{
	DPF(1, _T("CIndexBuffer not implemented in this interface version\n"));
	return 0;
}

// -------------------------------------------------------------------------------------

HRESULT CIndexBuffer::GetDescNotImplemented(PINDEXBUFFERDESC pDesc)
{
	DPF(1, _T("CIndexBuffer not implemented in this interface version\n"));
	return E_NOTIMPL;
}	