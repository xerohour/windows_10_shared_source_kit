//
// Common2.cpp
//
// Common Methods #2 with different #includes
//

#include <d3dx9.h>
#include <StrSafe.h>
#include <atltime.h>
#include "Common.hpp"


//////////////////////////////////////////////////////////////////////////////
//
// Global Function Definitions
//
//////////////////////////////////////////////////////////////////////////////

//
BOOL GetFileInfo( const CHAR* pcsFilename, FILEIDENTITYINFO* pFileIdentityInfo )
{
	DWORD dwFileFlags = 0;
	DWORD dwFVInfoSize = 0;
	BYTE * pFVInfo = NULL;
	UINT nUChars = 0;
	VS_FIXEDFILEINFO * pFFInfo = NULL;


	if ( (dwFVInfoSize = GetFileVersionInfoSizeA((LPSTR)pcsFilename, &dwFileFlags) ) == 0 )
    {
		return FALSE;
    }

	if ( ( pFVInfo = new BYTE[dwFVInfoSize] ) == NULL )
    {
		return FALSE;
    }


	// Get the file info
	GetFileVersionInfo((LPSTR)pcsFilename, NULL, dwFVInfoSize, pFVInfo);

	// Query for the fixed file info
	VerQueryValue(pFVInfo, _T("\\"), (PVOID*)&pFFInfo, &nUChars);

	// Get fixed file info
	if (pFFInfo)
	{
		pFileIdentityInfo->bDebug = pFFInfo->dwFileFlags & VS_FF_DEBUG;
		pFileIdentityInfo->dwFileVersionMS = pFFInfo->dwFileVersionMS;
		pFileIdentityInfo->dwFileVersionLS = pFFInfo->dwFileVersionLS;

        // Divide the version number into its 4 components.
        pFileIdentityInfo->wVersion[0] = HIWORD(pFFInfo->dwFileVersionMS);
        pFileIdentityInfo->wVersion[1] = LOWORD(pFFInfo->dwFileVersionMS);
        pFileIdentityInfo->wVersion[2] = HIWORD(pFFInfo->dwFileVersionLS);
        pFileIdentityInfo->wVersion[3] = LOWORD(pFFInfo->dwFileVersionLS);
	}

    // Get File Version String
    //
    PCHAR pVersionString = NULL;
    TCHAR szVersionString[ c_nFILEIDENTITYINFOSTRINGLEN / 2 ] = _T("");
    DWORD dwLangPrimary = 0;
    PDWORD pdwLang = NULL;
    TCHAR szQuery[100] = _T("");

    if ( VerQueryValue( pFVInfo, _T("\\VarFileInfo\\Translation"), (PVOID*)&pdwLang, &nUChars )
         && pdwLang && nUChars )
    {
        dwLangPrimary = *pdwLang;
        StringCchPrintf(
            szQuery,
            ARRAYSIZE(szQuery),
            _T("\\StringFileInfo\\%04x%04x\\FileVersion"),
            LOWORD(dwLangPrimary),
            HIWORD(dwLangPrimary)
            );
    } 

    if ( VerQueryValue( pFVInfo, szQuery, (PVOID*)&pVersionString, &nUChars ) 
        && pVersionString && nUChars > 0 )
    {
        StringCchCopy( szVersionString, ARRAYSIZE(szVersionString), pVersionString );
    }

    // Get Mod Date and File Size, format them, and return them in pFileInfo.
    // Don't flag any errors if it can't be obtained.
    //
    WIN32_FILE_ATTRIBUTE_DATA FileAttribData = {0};
    FILETIME ftLocalFileTime = {0};

    if ( GetFileAttributesEx( pcsFilename, GetFileExInfoStandard, &FileAttribData ) )
    {
        CTime fileTime( FileAttribData.ftLastWriteTime ); 

        StringCchPrintf(
            pFileIdentityInfo->tszFileIdentityInfo,
            ARRAYSIZE(pFileIdentityInfo->tszFileIdentityInfo),
            _T("TestApp Version: %s  Mod Date: %s  Size: %d bytes"),
            szVersionString,
            (LPCTSTR) fileTime.Format(_T("%c")),
            FileAttribData.nFileSizeLow
            );
    }

    delete [] pFVInfo;
    pFVInfo = NULL;

    return TRUE;
}
