#include "QueryPCH.h"

#include "CQuery.h"


// --------------------------------------------------------------------------------------

CQuery::CQuery()
{
    m_pQuery9 = NULL;
    pReleaseD3D = NULL;
}

// --------------------------------------------------------------------------------------

CQuery::~CQuery()
{
	ReleaseAll();
}

// --------------------------------------------------------------------------------------

ULONG WINAPI CQuery::AddRef(void)
{
    if(m_pDevice)
		m_pDevice->AddRef();
	return CObject::AddRef();
}

// --------------------------------------------------------------------------------------

ULONG WINAPI CQuery::Release(void)
{   
	if(m_pDevice)
		m_pDevice->Release();
	return CObject::Release();
}

// --------------------------------------------------------------------------------------

void CQuery::ReleaseAll()
{
    if (pReleaseD3D)
        ReleaseD3D();
}

// -------------------------------------------------------------------------------------

void *CQuery::GetInternalPointer(void)
{
    return m_pQuery9;
}
// -------------------------------------------------------------------------------------

bool CQuery::Init(LPVOID pQuery, CDevice *pDevice)
{
	if(pDevice == NULL)
	{
		DPF(1, _T("CQuery::Init() - Invalid params.\n"));
		return false;
	}

    bool bRet = false;

    // Release any pre-existing interfaces
	ReleaseAll();

    m_pDevice = pDevice;
	pDevice->AddRef();

	if (6.0f == m_pDevice->GetInterface())
	{
        pGetDevice = &CQuery::GetDevice6;
        pGetType = &CQuery::GetType6;
        pGetDataSize = &CQuery::GetDataSize6;
        pIssue = &CQuery::Issue6;
        pGetData = &CQuery::GetData6;
        pReleaseD3D = &CQuery::ReleaseD3D6;
        bRet = Init6(pQuery);
    }
	else if (7.0f == m_pDevice->GetInterface())
	{
        pGetDevice = &CQuery::GetDevice7;
        pGetType = &CQuery::GetType7;
        pGetDataSize = &CQuery::GetDataSize7;
        pIssue = &CQuery::Issue7;
        pGetData = &CQuery::GetData7;
        pReleaseD3D = &CQuery::ReleaseD3D7;
        bRet = Init7(pQuery);
    }
	else if (8.0f <= m_pDevice->GetInterface() && 9.0f > m_pDevice->GetInterface())
	{
        pGetDevice = &CQuery::GetDevice8;
        pGetType = &CQuery::GetType8;
        pGetDataSize = &CQuery::GetDataSize8;
        pIssue = &CQuery::Issue8;
        pGetData = &CQuery::GetData8;
        pReleaseD3D = &CQuery::ReleaseD3D8;
        bRet = Init8(pQuery);
    }
	else if (9.0f <= m_pDevice->GetInterface() && 10.0f > m_pDevice->GetInterface())
	{
        pGetDevice = &CQuery::GetDevice9;
        pGetType = &CQuery::GetType9;
        pGetDataSize = &CQuery::GetDataSize9;
        pIssue = &CQuery::Issue9;
        pGetData = &CQuery::GetData9;
        pReleaseD3D = &CQuery::ReleaseD3D9;
        bRet = Init9(pQuery);
    }

	return bRet;
}