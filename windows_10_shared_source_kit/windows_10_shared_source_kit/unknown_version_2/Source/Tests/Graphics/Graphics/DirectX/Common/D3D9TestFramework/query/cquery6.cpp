#include "QueryPCH.h"

#include "CQuery.h"
#include "D3D8.h"

// --------------------------------------------------------------------------------------

bool CQuery::Init6(LPVOID pQuery)
{
	return false;
}

// --------------------------------------------------------------------------------------

HRESULT CQuery::GetDevice6(CDevice** ppDevice)
{
	DPF(1, _T("CQuery::GetDevice6() - not supported on DX6.\n"));
	return E_FAIL;
}

// --------------------------------------------------------------------------------------

QUERYTYPE CQuery::GetType6()
{
	DPF(1, _T("CQuery::GetType6() - not supported on DX6.\n"));
	return (QUERYTYPE)0;
}

// --------------------------------------------------------------------------------------

DWORD CQuery::GetDataSize6()
{
	DPF(1, _T("CQuery::GetDataSize6() - not supported on DX6.\n"));
	return 0;
}

// --------------------------------------------------------------------------------------

HRESULT CQuery::Issue6(DWORD dwIssueFlags)
{
	DPF(1, _T("CQuery::Issue6() - not supported on DX6.\n"));
	return E_FAIL;
}

// --------------------------------------------------------------------------------------

HRESULT CQuery::GetData6(void* pData,DWORD dwSize,DWORD dwGetDataFlags)
{
	DPF(1, _T("CQuery::GetData6() - not supported on DX6.\n"));
	return E_FAIL;
}

// --------------------------------------------------------------------------------------

void CQuery::ReleaseD3D6()
{
	DPF(1, _T("CQuery::ReleaseD3D6() - not supported on DX6.\n"));
}
