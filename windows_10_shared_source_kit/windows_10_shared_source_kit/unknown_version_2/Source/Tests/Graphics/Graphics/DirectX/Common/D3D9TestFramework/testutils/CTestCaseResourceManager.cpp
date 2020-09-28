#include "TestUtilsPCH.h"
#include <CTestCaseResourceManager.h>


BOOL CALLBACK EnumResNameProc_Count
(
    HMODULE  _hModule,
    LPCTSTR  _lpszType,
    LPTSTR   _lpszName,
    LONG_PTR _lParam
)
{
    TCHAR* szID    = ((CTestCaseResourceManager*)_lParam)->m_szTestGroupID;
    UINT*  puCount = &(((CTestCaseResourceManager*)_lParam)->m_uCount_List_TestCaseNames);

    if ( 0 == _tcsnicmp( _lpszName, szID, _tcslen( szID ) ) )
    {
        ++(*puCount);
    }

    return TRUE;
}


BOOL CALLBACK EnumResNameProc_Names
(
    HMODULE  _hModule,
    LPCTSTR  _lpszType,
    LPTSTR   _lpszName,
    LONG_PTR _lParam
)
{
    static UINT s_uIndex = 0;

    TCHAR* szID = ((CTestCaseResourceManager*)_lParam)->m_szTestGroupID;

    if ( 0 == _tcsnicmp( _lpszName, szID, _tcslen( szID ) ) )
    {
        TCHAR** ppName = &(((CTestCaseResourceManager*)_lParam)->m_ppList_TestCaseNames[ s_uIndex ]);

        if ( NULL == ( *ppName = new TCHAR[ _tcslen( _lpszName ) + 1 ] ) )
        {
            OutputDebugStringFmt( _T("-------- ::EnumResNameProc_Contents() -- Failed!\n"
                                     "             Cond. : NULL == ( ppName = new TCHAR[ _tcslen( _lpszName ) ] )\n\n") );
            return FALSE;
        }

        _tcscpy( *ppName, _lpszName );

        ++s_uIndex;

        // Very importnat!!!
        if ( s_uIndex >= ((CTestCaseResourceManager*)_lParam)->m_uCount_List_TestCaseNames )
        {
            s_uIndex = 0;
        }
    }

    return TRUE;
}


CTestCaseResourceManager::CTestCaseResourceManager
(
    TCHAR* _szTestGroupID
)
{
    _tcsncpy( m_szTestGroupID, _szTestGroupID, TEST_GROUP_ID_MAX_LENGTH );
    m_szTestGroupID[ TEST_GROUP_ID_MAX_LENGTH ] = '\0';

    m_uCount_List_TestCaseNames = 0;
}


CTestCaseResourceManager::~CTestCaseResourceManager()
{
    for ( UINT i = 0; i < m_uCount_List_TestCaseNames; ++i )
    {
        SAFE_DELETEA( m_ppList_TestCaseNames[ i ] );
    }

    SAFE_DELETEA( m_ppList_TestCaseNames );
}


BOOL CTestCaseResourceManager::Populate_TestCaseNames()
{
    if ( 0 == ::EnumResourceNames( NULL, RT_RCDATA, (ENUMRESNAMEPROC)EnumResNameProc_Count, (LONG_PTR)this ) )
    {
        OutputDebugStringFmt( _T("-------- CTestCaseResourceManager::Populate_TestCaseNames() -- Failed!\n"
                                 "             Cond. : 0 == ::EnumResourceNames( NULL, RT_RCDATA, (ENUMRESNAMEPROC)EnumResNameProc_Count, (LONG_PTR)this )\n\n" ) );
        return FALSE;
    }

    if ( NULL == ( m_ppList_TestCaseNames = new (TCHAR*[ m_uCount_List_TestCaseNames ]) ) )
    {
        OutputDebugStringFmt( _T("-------- CTestCaseResourceManager::Populate_TestCaseNames() -- Failed!\n"
                                 "             Cond. : NULL == ( m_ppList_TestCaseNames = new (TCHAR*)[ m_uCount_List_TestCaseNames ] )\n\n" ) );
        return FALSE;
    }

    if ( 0 == ::EnumResourceNames( NULL, RT_RCDATA, (ENUMRESNAMEPROC)EnumResNameProc_Names, (LONG_PTR)this ) )
    {
        OutputDebugStringFmt( _T("-------- CTestCaseResourceManager::Populate_TestCaseNames() -- Failed!\n"
                                 "             Cond. : 0 == ::EnumResourceNames( NULL, RT_RCDATA, (ENUMRESNAMEPROC)EnumResNameProc_Names, (LONG_PTR)this )\n\n" ) );
        return FALSE;
    }

    return TRUE;
}


BOOL CTestCaseResourceManager::GetTestCase_Name
(
    UINT    _uTestCaseIndex,
    TCHAR** _pszTestCaseName
)
{
    if ( _uTestCaseIndex >= m_uCount_List_TestCaseNames )
    {
        OutputDebugStringFmt( _T("-------- CTestCaseResourceManager::GetTestCase_Name() -- Failed!\n"
                                 "             Cond. : _uTestCaseIndex (%d) >= m_uCount_List_TestCaseNames (%d)\n\n"), _uTestCaseIndex, m_uCount_List_TestCaseNames );
        *_pszTestCaseName = NULL;
        return FALSE;
    }

    *_pszTestCaseName = m_ppList_TestCaseNames[ _uTestCaseIndex ];

    return TRUE;
}


BOOL CTestCaseResourceManager::GetTestCase_Contents
(
    UINT   _uTestCaseIndex,
    CHAR*  _szEndToken,
    CHAR** _ppTestCaseContents
)
{
    if ( _uTestCaseIndex >= m_uCount_List_TestCaseNames )
    {
        OutputDebugStringFmt( _T("-------- CTestCaseResourceManager::GetTestCase_Contents() -- Failed!\n"
                                 "                Cond. : _uTestCaseIndex (%d) >= m_uCount_List_TestCaseNames (%d)\n"
                                 "             TestCase : %s\n\n"), _uTestCaseIndex, m_uCount_List_TestCaseNames,  m_ppList_TestCaseNames[ _uTestCaseIndex ] );
        *_ppTestCaseContents = NULL;
        return FALSE;
    }

    HRSRC hRes = NULL;

    if ( NULL == ( hRes = ::FindResource( NULL, m_ppList_TestCaseNames[ _uTestCaseIndex ], RT_RCDATA ) ) )
    {
        OutputDebugStringFmt( _T("-------- CTestCaseResourceManager::GetTestCase_Contents() -- Failed!\n"
                                 "                Func. : ::FindResource() -- Failed!\n"
                                 "             TestCase : %s\n\n"), m_ppList_TestCaseNames[ _uTestCaseIndex ] );
        return FALSE;
    }

    HGLOBAL hGlobal = NULL;

    if ( NULL == ( hGlobal = ::LoadResource( NULL, hRes ) ) ) 
    {
        OutputDebugStringFmt( _T("-------- CTestCaseResourceManager::GetTestCase_Contents() -- Failed!\n"
                                 "                Func. : ::LoadResource() -- Failed!\n"
                                 "             TestCase : %s\n\n"), m_ppList_TestCaseNames[ _uTestCaseIndex ] );
        return FALSE;
    }

    LPVOID pRes = NULL;

    if ( NULL == ( pRes = ::LockResource( hGlobal ) ) )
    {
        OutputDebugStringFmt( _T("-------- CTestCaseResourceManager::GetTestCase_Contents() -- Failed!\n"
                                 "                Func. : ::LockResource() -- Failed!\n"
                                 "             TestCase : %s\n\n" ), m_ppList_TestCaseNames[ _uTestCaseIndex ] );
        return FALSE;
    }

    CHAR* szContents_Start = (CHAR*)pRes;
    CHAR* szContents_End;

    if ( NULL == ( szContents_End = strstr( szContents_Start, _szEndToken ) ) )
    {
        OutputDebugStringFmt( _T("-------- CTestCaseResourceManager::GetTestCase_Contents() -- Failed!\n"
                                 "                Cond. : NULL == ( _tcsstr( szContents_Start, \"%s\" ) )\n"
                                 "             TestCase : %s\n\n"), _szEndToken, m_ppList_TestCaseNames[ _uTestCaseIndex ] );
        return FALSE;
    }

    UINT_PTR uContentLength = szContents_End - szContents_Start;

    if ( NULL == ( *_ppTestCaseContents = new CHAR[ uContentLength + 1 ] ) )
    {
        OutputDebugStringFmt( _T("-------- CTestCaseResourceManager::GetTestCase_Contents() -- Failed!\n"
                                 "                Cond. : NULL == ( *_ppTestCaseContents = new CHAR[ szContents_End - szContents_Start + 2 ] )\n"
                                 "             TestCase : %s\n\n"), m_ppList_TestCaseNames[ _uTestCaseIndex ] );
        return FALSE;
    }

    strncpy( *_ppTestCaseContents, szContents_Start, uContentLength );

    (*_ppTestCaseContents)[ uContentLength ] = '\0';

    return TRUE;
}
