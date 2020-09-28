#include "BuffersPCH.h"

#include <d3d8.h>
#include "CBuffers.h"
#include "CDevice.h"

// --------------------------------------------------------------------------------------

bool CVertexBuffer::Initialize8(LPVOID pVB)
{
	m_pVB8 = (LPDIRECT3DVERTEXBUFFER8)pVB;
	m_pVB8->AddRef();

	return true;
}

// --------------------------------------------------------------------------------------

void CVertexBuffer::ReleaseD3D8()
{
	RELEASE(m_pVB8);
}

// --------------------------------------------------------------------------------------

HRESULT CVertexBuffer::Lock8(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags)
{
	HRESULT		hr;

	hr = m_pVB8->Lock(OffsetToLock, SizeToLock, (BYTE**)ppbData, Flags);
	if ( FAILED(hr) )
	{
		DPF(1, _T("CVertexBuffer::Lock8() - Lock() failed.\n"));
		SetLastError(hr);
	}

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CVertexBuffer::Unlock8()
{
	HRESULT		hr;

	hr = m_pVB8->Unlock();
	if ( FAILED(hr) )
	{
		DPF(1, _T("CVertexBuffer::Unlock8() - Unlock() failed.\n"));
		SetLastError(hr);
	}

	return hr;
}

// --------------------------------------------------------------------------------------

UINT CVertexBuffer::GetSize8()
{
	D3DVERTEXBUFFER_DESC desc;
	memset( &desc, 0, sizeof(desc) );

	if( FAILED( m_pVB8->GetDesc( &desc ) ) )
		return 0;

	return (desc.Size);
}

// --------------------------------------------------------------------------------------

DWORD CVertexBuffer::GetPriority8()
{
	return m_pVB8->GetPriority();
}

// --------------------------------------------------------------------------------------

DWORD CVertexBuffer::SetPriority8(DWORD PriorityNew)
{
	return m_pVB8->SetPriority(PriorityNew);
}

// -------------------------------------------------------------------------------------

void CVertexBuffer::PreLoad8()
{
	m_pVB8->PreLoad();
}

// --------------------------------------------------------------------------------------

HRESULT CVertexBuffer::GetDesc8(PVERTEXBUFFERDESC pDesc)
{	 
	HRESULT hr;
    D3DVERTEXBUFFER_DESC D3DDesc;
	
	if (m_pVB8 == NULL || pDesc == NULL)
		return E_FAIL;
	
    ZeroMemory(pDesc, sizeof(VERTEXBUFFERDESC));

	hr = m_pVB8->GetDesc(&D3DDesc);
	if(FAILED(hr))
		return hr;
	
	//Fill in the version independent struct.
	pDesc->dwFVF = D3DDesc.FVF;
	pDesc->Format.d3dfFormat = D3DDesc.Format;
	pDesc->Type = D3DDesc.Type;
	pDesc->Usage = (DWORD)D3DDesc.Usage;
	pDesc->Pool = (DWORD)D3DDesc.Pool;
	pDesc->Size = D3DDesc.Size;
	d3df2ddpf(D3DDesc.Format, &(pDesc->Format.ddpfPixelFormat));

	return S_OK;
}		