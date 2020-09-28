#include "QueryPCH.h"

#include "CQuery.h"
#include "D3D9.h"


// --------------------------------------------------------------------------------------

bool CQuery::Init9(LPVOID pQuery)
{
    if (pQuery == NULL)
	{
		DPF(1, _T("CQuery::Init9() - Invalid Query.\n"));
		return false;
	}

    m_pQuery9 = (LPDIRECT3DQUERY9)pQuery;
    m_pQuery9->AddRef();
	return true;
}

// --------------------------------------------------------------------------------------

HRESULT CQuery::GetDevice9(CDevice** ppDevice)
{
    if (m_pQuery9 == NULL)
	{
		DPF(1, _T("CQuery::GetDevice9() - Invalid Query.\n"));
		return E_FAIL;
	}

    IDirect3DDevice9 *pDevice;
    HRESULT hr = m_pQuery9->GetDevice( PassPointer( &pDevice, ppDevice ) );

    if ( SUCCEEDED( hr ) )
    {
        pDevice->Release();
    }

    if ( ValidPointer( ppDevice ) )
    {
        *ppDevice = m_pDevice;
        m_pDevice->AddRef();
    }

	return hr;
}

// --------------------------------------------------------------------------------------

QUERYTYPE CQuery::GetType9()
{
    if (m_pQuery9 == NULL)
	{
		DPF(1, _T("CQuery::GetType9() - Invalid Query.\n"));
		return (QUERYTYPE)0;
	}

    return (QUERYTYPE) m_pQuery9->GetType();
}

// --------------------------------------------------------------------------------------

DWORD CQuery::GetDataSize9()
{
    if (m_pQuery9 == NULL)
	{
		DPF(1, _T("CQuery::GetDataSize9() - Invalid Query.\n"));
		return E_FAIL;
	}

    return m_pQuery9->GetDataSize();
}

// --------------------------------------------------------------------------------------

HRESULT CQuery::Issue9(DWORD dwIssueFlags)
{
    if (m_pQuery9 == NULL)
	{
		DPF(1, _T("CQuery::Issue9() - Invalid Query.\n"));
		return E_FAIL;
	}

    return m_pQuery9->Issue(dwIssueFlags);
}

// --------------------------------------------------------------------------------------

HRESULT CQuery::GetData9(void* pData,DWORD dwSize,DWORD dwGetDataFlags)
{
    if (m_pQuery9 == NULL)
	{
		DPF(1, _T("CQuery::GetData9() - Invalid Query.\n"));
		return E_FAIL;
	}

	return m_pQuery9->GetData(pData, dwSize, dwGetDataFlags);
}

// --------------------------------------------------------------------------------------

void CQuery::ReleaseD3D9()
{
	RELEASE(m_pQuery9);
}
