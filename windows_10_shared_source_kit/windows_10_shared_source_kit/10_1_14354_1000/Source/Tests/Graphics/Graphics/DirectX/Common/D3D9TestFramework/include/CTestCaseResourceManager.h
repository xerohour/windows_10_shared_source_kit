#ifndef ___CTESTCASERESOURCEMANAGER_H___
#define ___CTESTCASERESOURCEMANAGER_H___


#include <windows.h>
#include <tchar.h>
#include <string.h>
#include <TestUtils.h>

#define TEST_GROUP_ID_MAX_LENGTH  255
#define SAFE_RELEASE( x )  if ( (x) ) { (x)->Release();  (x) = NULL; }
#define SAFE_DELETE( x )   do { delete (x);  (x) = NULL; } while ( FALSE );
#define SAFE_DELETEA( x )  do { delete [] (x);  (x) = NULL; } while ( FALSE );

class CTestCaseResourceManager
{
public:

    CTestCaseResourceManager( TCHAR* _szTestGroupID );
    ~CTestCaseResourceManager();

    BOOL Populate_TestCaseNames();
    UINT GetCount_TestCases()  { return m_uCount_List_TestCaseNames; }

    BOOL GetTestCase_Name( UINT _uTestCaseIndex, TCHAR** _pszTestCaseName );
    BOOL GetTestCase_Contents( UINT _uTestCaseIndex, CHAR* _szEndToken, CHAR** _ppTestCaseContents );

private:

    friend BOOL CALLBACK EnumResNameProc_Count( HMODULE _hModule, LPCTSTR _lpszType, LPTSTR _lpszName, LONG_PTR _lParam );
    friend BOOL CALLBACK EnumResNameProc_Names( HMODULE _hModule, LPCTSTR _lpszType, LPTSTR _lpszName, LONG_PTR _lParam );

    TCHAR   m_szTestGroupID[ TEST_GROUP_ID_MAX_LENGTH + 1 ];
    TCHAR** m_ppList_TestCaseNames;
    UINT    m_uCount_List_TestCaseNames;
};


#endif