#include "BuffersPCH.h"

#include <d3d9.h>
#include "CBuffers.h"
#include "CDevice.h"

// --------------------------------------------------------------------------------------

bool CVertexBuffer::Initialize9(LPVOID pVB)
{
	m_pVB9 = (LPDIRECT3DVERTEXBUFFER9)pVB;
	m_pVB9->AddRef();

	return true;
}

// --------------------------------------------------------------------------------------

void CVertexBuffer::ReleaseD3D9()
{
	RELEASE(m_pVB9);
}

// --------------------------------------------------------------------------------------

HRESULT CVertexBuffer::Lock9(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags)
{
	HRESULT		hr;

	hr = m_pVB9->Lock(OffsetToLock, SizeToLock, ppbData, Flags);
	if ( FAILED(hr) )
	{
		DPF
		(
			1,
			_T("CVertexBuffer::Lock9(%u, %u, %p, %lu) failed with error %lu (0x%lx).\n"),
			OffsetToLock,
			SizeToLock,
			ppbData,
			Flags,
			hr,
			hr
		);

		SetLastError(hr);
	}

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CVertexBuffer::Unlock9()
{
	HRESULT		hr;

	hr = m_pVB9->Unlock();
	if ( FAILED(hr) )
	{
		DPF(1, _T("CVertexBuffer::Unlock9() - Unlock() failed.\n"));
		SetLastError(hr);
	}

	return hr;
}

// --------------------------------------------------------------------------------------

UINT CVertexBuffer::GetSize9()
{
	D3DVERTEXBUFFER_DESC desc;
	memset( &desc, 0, sizeof(desc) );

	if( FAILED( m_pVB9->GetDesc( &desc ) ) )
		return 0;

	return (desc.Size);
}

// --------------------------------------------------------------------------------------

DWORD CVertexBuffer::GetPriority9()
{
	return m_pVB9->GetPriority();
}

// --------------------------------------------------------------------------------------

DWORD CVertexBuffer::SetPriority9(DWORD PriorityNew)
{
	return m_pVB9->SetPriority(PriorityNew);
}

// --------------------------------------------------------------------------------------

HRESULT CVertexBuffer::GetDesc9(PVERTEXBUFFERDESC pDesc)
{	 
	HRESULT hr;
    D3DVERTEXBUFFER_DESC D3DDesc;
	
	if (m_pVB9 == NULL || pDesc == NULL)
		return E_FAIL;
	
    ZeroMemory(pDesc, sizeof(VERTEXBUFFERDESC));

	hr = m_pVB9->GetDesc(&D3DDesc);
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

// -------------------------------------------------------------------------------------

void CVertexBuffer::PreLoad9()
{
	m_pVB9->PreLoad();
}