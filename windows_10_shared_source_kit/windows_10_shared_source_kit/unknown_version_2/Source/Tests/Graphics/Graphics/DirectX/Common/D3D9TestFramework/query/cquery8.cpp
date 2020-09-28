#include "QueryPCH.h"

#include "CQuery.h"
#include "D3D8.h"

// --------------------------------------------------------------------------------------

bool CQuery::Init8(LPVOID pQuery)
{
	return false;
}

// --------------------------------------------------------------------------------------

HRESULT CQuery::GetDevice8(CDevice** ppDevice)
{
	DPF(1, _T("CQuery::GetDevice8() - not supported on DX8.\n"));
	return E_FAIL;
}

// --------------------------------------------------------------------------------------

QUERYTYPE CQuery::GetType8()
{
	DPF(1, _T("CQuery::GetType8() - not supported on DX8.\n"));
	return (QUERYTYPE)0;
}

// --------------------------------------------------------------------------------------

DWORD CQuery::GetDataSize8()
{
	DPF(1, _T("CQuery::GetDataSize8() - not supported on DX8.\n"));
	return 0;
}

// --------------------------------------------------------------------------------------

HRESULT CQuery::Issue8(DWORD dwIssueFlags)
{
	DPF(1, _T("CQuery::Issue8() - not supported on DX8.\n"));
	return E_FAIL;
}

// --------------------------------------------------------------------------------------

HRESULT CQuery::GetData8(void* pData,DWORD dwSize,DWORD dwGetDataFlags)
{
	DPF(1, _T("CQuery::GetData8() - not supported on DX8.\n"));
	return E_FAIL;
}

// --------------------------------------------------------------------------------------

void CQuery::ReleaseD3D8()
{
	DPF(1, _T("CQuery::ReleaseD3D8() - not supported on DX8.\n"));
}
