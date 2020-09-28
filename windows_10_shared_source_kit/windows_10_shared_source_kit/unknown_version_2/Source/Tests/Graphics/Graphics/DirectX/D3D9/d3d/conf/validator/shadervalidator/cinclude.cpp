#include "CInclude.h"


HRESULT CIncludeFromResource::Open
(
    D3DXINCLUDE_TYPE,
    LPCSTR            pFileName,
    LPCVOID,
    LPCVOID          * ppData,
    UINT             * pBytes
)
{
    LPVOID pData = NULL;

    if ( !LoadFromResource( pFileName, &pData, (DWORD *)pBytes ) )
    {
        return E_FAIL;
    }

    *ppData = pData;

    return S_OK;
}


HRESULT CIncludeFromFile::Open
(
    D3DXINCLUDE_TYPE,
    LPCSTR             pFileName,
    LPCVOID,
    LPCVOID          * ppData,
    UINT             * pBytes
)
{
    FILE * pFile = NULL;

    if ( 0 == strcmp( pFileName, "Test_Include.fx" ) )
    {
        sprintf( m_szFileName, ".\\TestCases\\%s", pFileName );
    }
    else
    {
        strcpy( m_szFileName, pFileName );
    }

    if ( NULL == ( pFile = fopen( m_szFileName, "rt" ) ) )
    {
        return E_FAIL;
    }

    m_ppData[ m_uDataSetIndex ] = new BYTE [ MAX_INCLUDE_DATA_SIZE ];

    *pBytes = fread( (void *)m_ppData[ m_uDataSetIndex ], sizeof( BYTE ), MAX_INCLUDE_DATA_SIZE, pFile );
    *ppData = (LPCVOID)(m_ppData[ m_uDataSetIndex ]);

    ++m_uDataSetIndex;

    fclose( pFile );

    return S_OK;
}