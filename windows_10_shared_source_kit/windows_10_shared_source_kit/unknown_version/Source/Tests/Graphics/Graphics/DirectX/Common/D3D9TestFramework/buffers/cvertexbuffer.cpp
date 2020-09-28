#include "BuffersPCH.h"

#include "CBuffers.h"
#include "CDevice.h"



// --------------------------------------------------------------------------------------


CVertexBuffer::CVertexBuffer()
{
	m_pDevice = NULL;
	m_pVB6 = NULL;
	m_pVB7 = NULL;
	m_pVB8 = NULL;
	
	pReleaseD3D = NULL;
}

// --------------------------------------------------------------------------------------

CVertexBuffer::~CVertexBuffer()
{
	if( pReleaseD3D )
		ReleaseD3D();
}

// --------------------------------------------------------------------------------------

ULONG WINAPI CVertexBuffer::AddRef(void)
{
    if(m_pDevice)
		m_pDevice->AddRef();
	return CObject::AddRef();
}

// --------------------------------------------------------------------------------------

ULONG WINAPI CVertexBuffer::Release(void)
{
    if(m_pDevice)
		m_pDevice->Release();
	return CObject::Release();
}

// --------------------------------------------------------------------------------------

bool CVertexBuffer::Initialize(LPVOID pVB, CDevice *pDevice)
{
	bool bRet = false;

	if (NULL == pVB || NULL == pDevice)
		return false;

	m_pDevice = pDevice;
	m_pDevice->AddRef();

	if (6.0f == m_pDevice->GetInterface())
	{
       	pReleaseD3D = &CVertexBuffer::ReleaseD3D6;
		pUnlock	 = &CVertexBuffer::Unlock6;
		pLock	 = &CVertexBuffer::Lock6;
		pGetSize = &CVertexBuffer::GetSize6;
		pOptimize = &CVertexBuffer::Optimize6;
		pProcessVertices = &CVertexBuffer::ProcessVertices6;
		pProcessVerticesStrided = &CVertexBuffer::ProcessVerticesStridedNotSupported;
		pPreLoad = &CVertexBuffer::PreLoadNotSupported;
		pGetPriority = &CVertexBuffer::GetPriorityNotSupported;
		pSetPriority = &CVertexBuffer::SetPriorityNotSupported;
		pGetDesc = &CVertexBuffer::GetDesc6;
		bRet = Initialize6(pVB);
	}
	else if (7.0f == m_pDevice->GetInterface())
	{
       	pReleaseD3D = &CVertexBuffer::ReleaseD3D7;
		pUnlock	 = &CVertexBuffer::Unlock7;
		pLock	 = &CVertexBuffer::Lock7;
		pGetSize = &CVertexBuffer::GetSize7;
		pOptimize = &CVertexBuffer::Optimize7;
		pProcessVertices = &CVertexBuffer::ProcessVertices7;
		pProcessVerticesStrided = &CVertexBuffer::ProcessVerticesStrided7;
		pPreLoad = &CVertexBuffer::PreLoadNotSupported;
		pGetPriority = &CVertexBuffer::GetPriorityNotSupported;
		pSetPriority = &CVertexBuffer::SetPriorityNotSupported;
		pGetDesc = &CVertexBuffer::GetDesc7;
		bRet = Initialize7(pVB);
	}
	else if (8.0f <= m_pDevice->GetInterface() && 9.0f > m_pDevice->GetInterface())
	{
		// Initialize function pointers
		pReleaseD3D = &CVertexBuffer::ReleaseD3D8;
		pUnlock	 = &CVertexBuffer::Unlock8;
		pLock	 = &CVertexBuffer::Lock8;
		pGetSize = &CVertexBuffer::GetSize8;
		pOptimize = &CVertexBuffer::OptimizeNotSupported;
		pProcessVertices = &CVertexBuffer::ProcessVerticesNotSupported;
		pProcessVerticesStrided = &CVertexBuffer::ProcessVerticesStridedNotSupported;
		pPreLoad = &CVertexBuffer::PreLoad8;
		pGetPriority = &CVertexBuffer::GetPriority8;
		pSetPriority = &CVertexBuffer::SetPriority8;
		pGetDesc = &CVertexBuffer::GetDesc8;
		bRet = Initialize8(pVB);
	}
    else if (9.0f <= m_pDevice->GetInterface() && 10.0f > m_pDevice->GetInterface())
	{
		// Initialize function pointers
		pReleaseD3D = &CVertexBuffer::ReleaseD3D9;
		pUnlock	 = &CVertexBuffer::Unlock9;
		pLock	 = &CVertexBuffer::Lock9;
		pGetSize = &CVertexBuffer::GetSize9;
		pOptimize = &CVertexBuffer::OptimizeNotSupported;
		pProcessVertices = &CVertexBuffer::ProcessVerticesNotSupported;
		pProcessVerticesStrided = &CVertexBuffer::ProcessVerticesStridedNotSupported;
		pPreLoad = &CVertexBuffer::PreLoad9;
		pGetPriority = &CVertexBuffer::GetPriority9;
		pSetPriority = &CVertexBuffer::SetPriority9;
		pGetDesc = &CVertexBuffer::GetDesc9;
		bRet = Initialize9(pVB);
	}

	GetDesc(&m_Desc);
	
	return bRet;
}

// --------------------------------------------------------------------------------------

HRESULT CVertexBuffer::OptimizeNotSupported(void)
{
	DebugOut.Write(1, "CVertexBuffer: Optimize() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
	return m_pDevice->SetLastError(E_FAIL);
}

// -------------------------------------------------------------------------------------

HRESULT CVertexBuffer::ProcessVerticesNotSupported(DWORD VertexOp, DWORD DestIndex, DWORD Count, LPVERTEXBUFFER SrcBuffer, DWORD SrcIndex, DWORD dwFlags)
{
	DebugOut.Write(1, "CVertexBuffer: ProcessVertices() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
	return m_pDevice->SetLastError(E_FAIL);
}

// -------------------------------------------------------------------------------------

HRESULT CVertexBuffer::ProcessVerticesStridedNotSupported(DWORD VertexOp, DWORD DestIndex, DWORD Count, LPDRAWPRIMITIVESTRIDEDDATA SrcVertexArray, DWORD SrcIndex, DWORD dwFlags)
{
	DebugOut.Write(1, "CVertexBuffer: ProcessVerticesStrided() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
	return m_pDevice->SetLastError(E_FAIL);
}

// -------------------------------------------------------------------------------------

DWORD CVertexBuffer::GetPriorityNotSupported(void)
{
	DebugOut.Write(1, "CVertexBuffer: GetPriority() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
	return (DWORD)0;
}

// -------------------------------------------------------------------------------------

DWORD CVertexBuffer::SetPriorityNotSupported(DWORD PriorityNew)
{
	DebugOut.Write(1, "CVertexBuffer: SetPriority() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
	return (DWORD)0;
}

// -------------------------------------------------------------------------------------

void CVertexBuffer::PreLoadNotSupported()
{
	DebugOut.Write(1, "CVertexBuffer: PreLoad() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
}

// -------------------------------------------------------------------------------------

void * CVertexBuffer::GetInternalPointer(void)
{
	if (m_pVB9)
		return m_pVB9;
	else if (m_pVB8)
		return m_pVB8;
	else if (m_pVB7)
		return m_pVB7;
	else if (m_pVB6)
		return m_pVB6;

	//No internal pointer set
	return NULL;
}