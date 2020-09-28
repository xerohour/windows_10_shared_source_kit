#include "BuffersPCH.h"

#include <d3d8.h>
#include "CBuffers.h"
#include "CDevice.h"



// --------------------------------------------------------------------------------------

bool CIndexBuffer::Init8(LPVOID pIB)
{
	m_pIB8 = (LPDIRECT3DINDEXBUFFER8)pIB;
	m_pIB8->AddRef();

	return true;
}

// --------------------------------------------------------------------------------------

HRESULT CIndexBuffer::Lock8(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags)
{
	HRESULT		hr;

	hr = m_pIB8->Lock(OffsetToLock, SizeToLock, (BYTE**)ppbData, Flags);
	if ( FAILED(hr) )
	{
		DPF(1, _T("CIndexBuffer::Lock8() - Lock() failed.\n"));
		SetLastError(hr);
	}

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CIndexBuffer::Unlock8()
{
	HRESULT		hr;

	hr = m_pIB8->Unlock();
	if ( FAILED(hr) )
	{
		DPF(1, _T("CIndexBuffer::Unlock8() - Unlock() failed.\n"));
		SetLastError(hr);
	}

	return hr;
}

// --------------------------------------------------------------------------------------

UINT CIndexBuffer::GetSize8()
{
	D3DINDEXBUFFER_DESC desc;
	memset( &desc, 0, sizeof(desc) );

	if( FAILED( m_pIB8->GetDesc( &desc ) ) )
		return 0;

	return (desc.Size);
}

// -------------------------------------------------------------------------------------

HRESULT CIndexBuffer::GetDesc8(PINDEXBUFFERDESC pDesc)
{	 
	HRESULT hr;
    D3DINDEXBUFFER_DESC D3DDesc;
	
	if (m_pIB8 == NULL || pDesc == NULL)
		return E_FAIL;
	
    ZeroMemory(pDesc, sizeof(INDEXBUFFERDESC));

	hr = m_pIB8->GetDesc(&D3DDesc);
	if(FAILED(hr))
		return hr;
	
	//Fill in the version independent struct.
	pDesc->Format.d3dfFormat = D3DDesc.Format;
	pDesc->Type = D3DDesc.Type;
	pDesc->Usage = (DWORD)D3DDesc.Usage;
	pDesc->Pool = (DWORD)D3DDesc.Pool;
	pDesc->Size = D3DDesc.Size;
	d3df2ddpf(D3DDesc.Format, &(pDesc->Format.ddpfPixelFormat));

	return S_OK;
}

// --------------------------------------------------------------------------------------

void CIndexBuffer::ReleaseD3D8()
{
	RELEASE(m_pIB8);
}

// --------------------------------------------------------------------------------------

DWORD CIndexBuffer::SetPriority8(DWORD dwNewPriority)
{
	return m_pIB8->SetPriority(dwNewPriority);
}

// --------------------------------------------------------------------------------------

DWORD CIndexBuffer::GetPriority8()
{
	return m_pIB8->GetPriority();
}

// --------------------------------------------------------------------------------------

void CIndexBuffer::PreLoad8()
{
	m_pIB8->PreLoad();
}