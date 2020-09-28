#include "BuffersPCH.h"

#include <d3d.h>
#include "CBuffers.h"
#include "CDevice.h"

// --------------------------------------------------------------------------------------

bool CVertexBuffer::Initialize6(LPVOID pVB)
{
	m_pVB6 = (LPDIRECT3DVERTEXBUFFER)pVB;
	m_pVB6->AddRef();
	return true;
}

// --------------------------------------------------------------------------------------

void CVertexBuffer::ReleaseD3D6()
{
	RELEASE(m_pVB6);
}

// --------------------------------------------------------------------------------------

HRESULT CVertexBuffer::Lock6(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags)
{
	HRESULT		hr;

	hr = m_pVB6->Lock(Flags, ppbData, NULL);
	if ( FAILED(hr) )
	{
		DPF(1, _T("CVertexBuffer::Lock6() - Lock() failed.\n"));
		SetLastError(hr);
	}

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CVertexBuffer::Unlock6()
{
	HRESULT		hr;

	hr = m_pVB6->Unlock();
	if ( FAILED(hr) )
	{
		DPF(1, _T("CVertexBuffer::Unlock6() - Unlock() failed.\n"));
		SetLastError(hr);
	}

	return hr;
}

// --------------------------------------------------------------------------------------

UINT CVertexBuffer::GetSize6()
{
	D3DVERTEXBUFFERDESC desc;
	memset( &desc, 0, sizeof(desc) );
	desc.dwSize = sizeof(desc);

	if( FAILED( m_pVB6->GetVertexBufferDesc( &desc ) ) )
		return 0;

	return ((UINT)desc.dwNumVertices * (UINT)m_pDevice->GetVertexSize(desc.dwFVF));
}

// --------------------------------------------------------------------------------------

HRESULT CVertexBuffer::Optimize6(void)
{
	HRESULT		hr;

	hr = m_pVB6->Optimize(m_pDevice->GetDevicePointer6(), 0);
	if (FAILED(hr))
	{
		DPF(1, _T("CVertexBuffer::Optimize6() - Optimize() failed.\n"));
		SetLastError(hr);
	}

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CVertexBuffer::ProcessVertices6(DWORD VertexOp, DWORD DestIndex, DWORD Count, LPVERTEXBUFFER SrcBuffer, DWORD SrcIndex, DWORD dwFlags)
{
	HRESULT		hr;

	hr = m_pVB6->ProcessVertices(VertexOp, DestIndex, Count, ((SrcBuffer != NULL) ? SrcBuffer->GetVertexBufferPointer6() : NULL), SrcIndex, m_pDevice->GetDevicePointer6(), dwFlags);
	if (FAILED(hr))
	{
		DPF(1, _T("CVertexBuffer::ProcessVertices6() - ProcessVertices() failed.\n"));
		SetLastError(hr);
	}

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CVertexBuffer::GetDesc6(PVERTEXBUFFERDESC pDesc)
{	 
	HRESULT hr;
    D3DVERTEXBUFFERDESC D3DDesc;
	
	if (m_pVB6 == NULL || pDesc == NULL)
		return E_FAIL;
	
    ZeroMemory(pDesc, sizeof(VERTEXBUFFERDESC));
	D3DDesc.dwSize = sizeof(D3DVERTEXBUFFERDESC);
	hr = m_pVB6->GetVertexBufferDesc(&D3DDesc);
	if(FAILED(hr))
		return hr;

	//Fill in the version independent struct.
	pDesc->dwSize = D3DDesc.dwSize;
	pDesc->dwFVF = D3DDesc.dwFVF;
	pDesc->dwCaps = D3DDesc.dwCaps;
	pDesc->dwNumVertices = D3DDesc.dwNumVertices;

	return S_OK;
}