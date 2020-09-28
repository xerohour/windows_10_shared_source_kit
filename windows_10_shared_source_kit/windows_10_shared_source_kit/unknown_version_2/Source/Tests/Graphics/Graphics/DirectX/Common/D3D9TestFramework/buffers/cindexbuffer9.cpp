#include "BuffersPCH.h"

#include <d3d9.h>
#include "CBuffers.h"
#include "CDevice.h"

// --------------------------------------------------------------------------------------

bool CIndexBuffer::Init9(LPVOID pIB)
{
	m_pIB9 = (LPDIRECT3DINDEXBUFFER9)pIB;
	m_pIB9->AddRef();

	return true;
}

// --------------------------------------------------------------------------------------

HRESULT CIndexBuffer::Lock9(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags)
{
	HRESULT		hr;

	hr = m_pIB9->Lock(OffsetToLock, SizeToLock, ppbData, Flags);
	if ( FAILED(hr) )
	{
		DPF(1, _T("CIndexBuffer::Lock9() - Lock() failed.\n"));
		SetLastError(hr);
	}

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CIndexBuffer::Unlock9()
{
	HRESULT		hr;

	hr = m_pIB9->Unlock();
	if ( FAILED(hr) )
	{
		DPF(1, _T("CIndexBuffer::Unlock9() - Unlock() failed.\n"));
		SetLastError(hr);
	}

	return hr;
}

// --------------------------------------------------------------------------------------

UINT CIndexBuffer::GetSize9()
{
	D3DINDEXBUFFER_DESC desc;
	memset( &desc, 0, sizeof(desc) );

	if( FAILED( m_pIB9->GetDesc( &desc ) ) )
		return 0;

	return (desc.Size);
}

// -------------------------------------------------------------------------------------

HRESULT CIndexBuffer::GetDesc9(PINDEXBUFFERDESC pDesc)
{	 
	HRESULT hr;
    D3DINDEXBUFFER_DESC D3DDesc;
	
	if (m_pIB9 == NULL || pDesc == NULL)
		return E_FAIL;
	
    ZeroMemory(pDesc, sizeof(INDEXBUFFERDESC));

	hr = m_pIB9->GetDesc(&D3DDesc);
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

void CIndexBuffer::ReleaseD3D9()
{
	RELEASE(m_pIB9);
}

// --------------------------------------------------------------------------------------

DWORD CIndexBuffer::SetPriority9(DWORD dwNewPriority)
{
	return m_pIB9->SetPriority(dwNewPriority);
}

// --------------------------------------------------------------------------------------

DWORD CIndexBuffer::GetPriority9()
{
	return m_pIB9->GetPriority();
}

// --------------------------------------------------------------------------------------

void CIndexBuffer::PreLoad9()
{
	m_pIB9->PreLoad();
}