#include "BuffersPCH.h"

#include <d3d.h>
#include "CBuffers.h"
#include "CDevice.h"

// --------------------------------------------------------------------------------------

bool CVertexBuffer::Initialize7(LPVOID pVB)
{
	m_pVB7 = (LPDIRECT3DVERTEXBUFFER7)pVB;
	m_pVB7->AddRef();

	return true;
}

// --------------------------------------------------------------------------------------

void CVertexBuffer::ReleaseD3D7()
{
	RELEASE(m_pVB7);
}

// --------------------------------------------------------------------------------------

HRESULT CVertexBuffer::Lock7(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags)
{
	HRESULT		hr;

	hr = m_pVB7->Lock(Flags, ppbData, NULL);
	if ( FAILED(hr) )
	{
		DPF(1, _T("CVertexBuffer::Lock7() - Lock() failed.\n"));
		SetLastError(hr);
	}

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CVertexBuffer::Unlock7()
{
	HRESULT		hr;

	hr = m_pVB7->Unlock();
	if ( FAILED(hr) )
	{
		DPF(1, _T("CVertexBuffer::Unlock7() - Unlock() failed.\n"));
		SetLastError(hr);
	}

	return hr;
}

// --------------------------------------------------------------------------------------

UINT CVertexBuffer::GetSize7()
{
	D3DVERTEXBUFFERDESC desc;
	memset( &desc, 0, sizeof(desc) );
	desc.dwSize = sizeof(desc);

	if( FAILED( m_pVB7->GetVertexBufferDesc( &desc ) ) )
		return 0;

	return ((UINT)desc.dwNumVertices * (UINT)m_pDevice->GetVertexSize(desc.dwFVF));
}

// --------------------------------------------------------------------------------------

HRESULT CVertexBuffer::Optimize7(void)
{
	HRESULT		hr;

	hr = m_pVB7->Optimize(m_pDevice->GetDevicePointer7(), 0);
	if (FAILED(hr))
	{
		DPF(1, _T("CVertexBuffer::Optimize7() - Optimize() failed.\n"));
		SetLastError(hr);
	}

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CVertexBuffer::ProcessVertices7(DWORD VertexOp, DWORD DestIndex, DWORD Count, LPVERTEXBUFFER SrcBuffer, DWORD SrcIndex, DWORD dwFlags)
{
	HRESULT		hr;

	hr = m_pVB7->ProcessVertices(VertexOp, DestIndex, Count, ((SrcBuffer != NULL) ? SrcBuffer->GetVertexBufferPointer7() : NULL), SrcIndex, m_pDevice->GetDevicePointer7(), dwFlags);
	if (FAILED(hr))
	{
		DPF(1, _T("CVertexBuffer::ProcessVertices7() - ProcessVertices() failed.\n"));
		SetLastError(hr);
	}

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CVertexBuffer::ProcessVerticesStrided7(DWORD VertexOp, DWORD DestIndex, DWORD Count, LPDRAWPRIMITIVESTRIDEDDATA SrcVertexArray, DWORD SrcIndex, DWORD dwFlags)
{
	HRESULT		hr;

	hr = m_pVB7->ProcessVerticesStrided(VertexOp, DestIndex, Count, (LPD3DDRAWPRIMITIVESTRIDEDDATA)SrcVertexArray, SrcIndex, m_pDevice->GetDevicePointer7(), dwFlags);
	if (FAILED(hr))
	{
		DPF(1, _T("CVertexBuffer::ProcessVerticesStrided7() - ProcessVerticesStrided() failed.\n"));
		SetLastError(hr);
	}

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CVertexBuffer::GetDesc7(PVERTEXBUFFERDESC pDesc)
{	 
	HRESULT hr;
    D3DVERTEXBUFFERDESC D3DDesc;
	
	if (m_pVB7 == NULL || pDesc == NULL)
		return E_FAIL;
	
    ZeroMemory(pDesc, sizeof(VERTEXBUFFERDESC));
	D3DDesc.dwSize = sizeof(D3DVERTEXBUFFERDESC);
	hr = m_pVB7->GetVertexBufferDesc(&D3DDesc);
	if(FAILED(hr))
		return hr;

	//Fill in the version independent struct.
	pDesc->dwSize = D3DDesc.dwSize;
	pDesc->dwFVF = D3DDesc.dwFVF;
	pDesc->dwCaps = D3DDesc.dwCaps;
	pDesc->dwNumVertices = D3DDesc.dwNumVertices;

	return S_OK;
}
