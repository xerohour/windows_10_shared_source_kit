#include "QueryPCH.h"

#include "CQuery.h"
#include "D3D8.h"

// --------------------------------------------------------------------------------------

bool CQuery::Init7(LPVOID pQuery)
{
	return false;
}

// --------------------------------------------------------------------------------------

HRESULT CQuery::GetDevice7(CDevice** ppDevice)
{
	DPF(1, _T("CQuery::GetDevice7() - not supported on DX7.\n"));
	return E_FAIL;
}

// --------------------------------------------------------------------------------------

QUERYTYPE CQuery::GetType7()
{
	DPF(1, _T("CQuery::GetType7() - not supported on DX7.\n"));
	return (QUERYTYPE)0;
}

// --------------------------------------------------------------------------------------

DWORD CQuery::GetDataSize7()
{
	DPF(1, _T("CQuery::GetDataSize7() - not supported on DX7.\n"));
	return 0;
}

// --------------------------------------------------------------------------------------

HRESULT CQuery::Issue7(DWORD dwIssueFlags)
{
	DPF(1, _T("CQuery::Issue7() - not supported on DX7.\n"));
	return E_FAIL;
}

// --------------------------------------------------------------------------------------

HRESULT CQuery::GetData7(void* pData,DWORD dwSize,DWORD dwGetDataFlags)
{
	DPF(1, _T("CQuery::GetData7() - not supported on DX7.\n"));
	return E_FAIL;
}

// --------------------------------------------------------------------------------------

void CQuery::ReleaseD3D7()
{
	DPF(1, _T("CQuery::ReleaseD3D7() - not supported on DX7.\n"));
}
