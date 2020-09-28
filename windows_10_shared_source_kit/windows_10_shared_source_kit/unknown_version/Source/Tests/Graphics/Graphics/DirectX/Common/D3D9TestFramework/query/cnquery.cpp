#include "QueryPCH.h"

#include "CQuery.h"

CnQuery::~CnQuery()
{
	if( !m_pQueries )
		return;

	for( UINT i= 0; i< m_uDevices; i++ )
	{
		RELEASE( m_pQueries[i] );
	}
	delete[] m_pQueries;
}

bool CnQuery::Init(UINT uDevices)
{
	m_pQueries = new CQuery*[uDevices];
	
	if( m_pQueries == NULL )
		return false;

	m_uDevices = uDevices;
	memset( m_pQueries, 0, uDevices * sizeof( CQuery*) );
	return true;
}

bool CnQuery::GetQuery(CQuery **ppQuery, UINT n)
{
	if( n >= m_uDevices || ppQuery == NULL )
		return false;

	m_pQueries[n]->AddRef();
	*ppQuery = m_pQueries[n];
	return true;
}

bool CnQuery::SetQuery(CQuery *pQuery, UINT n)
{
	if( n >= m_uDevices )
		return false;

	if( m_pQueries[n] != NULL )
		return false;

	m_pQueries[n] = pQuery;
	return true;
}
/*
bool CnQuery::GetDevice(CnDevice* ppDevice)
{
	for (UINT i=0; i<m_uDevices; i++)
	{
        CDevice *pDevice;

		HRESULT hr = m_pQueries[i]->GetDevice(&pDevice);
		if (FAILED(hr))
		{
            return false;
		}

        ppDevice
	}

    return false;
}
*/
QUERYTYPE CnQuery::GetType() 
{
    return m_pQueries[0]->GetType();
}

DWORD CnQuery::GetDataSize()
{
    return m_pQueries[0]->GetDataSize();
}

bool CnQuery::Issue(DWORD dwIssueFlags)
{
	for (UINT i=0; i<m_uDevices; i++)
	{
		HRESULT hr = m_pQueries[i]->Issue(dwIssueFlags);
		if (FAILED(hr))
		{
            return false;
		}
        break;
	}

    return true;
}

HRESULT CnQuery::GetData(void* pData,DWORD dwSize,DWORD dwGetDataFlags)
{
    return m_pQueries[0]->GetData(pData, dwSize, dwGetDataFlags);
}
