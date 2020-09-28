/*++

    Copyright (C) Microsoft. All rights reserved.

Module Name:
   dbgclient.cpp

Environment:
   User Mode

Author:
   jinsh 10/3/2010
--*/

#include "dbgClient.h"
#include <vector>
#include <msi.h>
#include <objbase.h>
#include <atlbase.h>

#define SIG_FILEINFO 0xFEEF04BD
//#define DBGENG_MIN_VER 0x0006000223F00000   // 6.2.9200.0
#define DBGENG_MIN_VER 0x0010000026A00000   // 10.0.9888.4


#ifdef _WIN64
#define DEBUGGER_UPGRADE_CODE L"{1AC565E6-B336-4DAD-8BD3-B987DED1079E}"
#else
#define DEBUGGER_UPGRADE_CODE L"{AEBA607E-D79B-47EC-9C9B-4B3807853863}"
#endif


IDebugDataSpaces2 *DbgClient::s_pSpaces = 0;
IDebugSymbols3 *DbgClient::s_pSymbols = 0;


BOOL TryLoadDbgEngFromWPDK(HMODULE *phDbgEng)
{
    CRegKey hKey;

    //
    // If the WDK is installed (as it should be if you got raw2dump from
    // the WPDK), then the path to the debuggers will live at
    // HKLM\SOFTWARE\Microsoft\Windows Kits\Installed Roots under the
    // value WindowsDebuggersRoot81 (for the 8.1 WDK) or the
    // value WindowsDebuggersRoot (for previous WDKs)
    //

    if (ERROR_SUCCESS !=
            hKey.Open(
                HKEY_LOCAL_MACHINE,
                L"Software\\Microsoft\\Windows Kits\\Installed Roots",
                KEY_READ | KEY_WOW64_32KEY)) {

        return FALSE;
    }

    LPCWSTR KeyValues[] = { L"WindowsDebuggersRoot81", L"WindowsDebuggersRoot" };

    for (int i = 0; i < _countof(KeyValues); i++) {
        std::vector<wchar_t> debugPath;
        ULONG cchDebugPath = 0;

        if (ERROR_SUCCESS !=
                hKey.QueryStringValue(
                    KeyValues[i],
                    NULL,
                    &cchDebugPath)) {

            continue;
        }

        if (cchDebugPath == 0) {
            continue;
        }

        debugPath.resize(cchDebugPath);

        if (ERROR_SUCCESS !=
                hKey.QueryStringValue(
                    KeyValues[i],
                    &debugPath[0],
                    &cchDebugPath)) {

            continue;
        }

        //
        // Make sure the path is well-formed and null-terminated
        //
        if (cchDebugPath < 2) {
            continue;
        }

        if (debugPath[cchDebugPath - 1] != L'\0') {
            continue;
        }

        //
        // Add a trailing backslash if one isn't already present, and lose the
        // terminating null
        //
        if (debugPath[cchDebugPath - 2] != L'\\') {
            debugPath[cchDebugPath - 1] = L'\\';
        } else {
            debugPath.pop_back();
        }

        //
        // Append the relative path to dbgeng.dll from the debugger root
        //
#ifdef _WIN64
        static const wchar_t szDbgEngRelPath[] = L"x64\\dbgeng.dll";
#else
        static const wchar_t szDbgEngRelPath[] = L"x86\\dbgeng.dll";
#endif

        debugPath.insert(
            debugPath.end(),
            szDbgEngRelPath,
            szDbgEngRelPath + (sizeof szDbgEngRelPath / sizeof szDbgEngRelPath[0]));

        *phDbgEng = LoadLibraryExW(&debugPath[0], NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
        if (*phDbgEng != NULL) {
            break;
        }
    }

    return (*phDbgEng != NULL);
}

BOOL TryLoadDbgEngFromMSI(HMODULE *phDbgEng)
{
    DWORD iProduct = 0;

    //
    // If the debuggers are installed as an MSI, we should be able to get
    // their install location by querying MSI
    //

    for (;;) {
       wprintf(L"Loop\n\r");
        WCHAR ProductCode[39];  // 39 is the documented required buffer length
        WCHAR InstallPath[MAX_PATH];
        DWORD cchInstallPath = _countof(InstallPath);
        UINT status;

        // Enumerate installed debuggers
        status = MsiEnumRelatedProductsW(DEBUGGER_UPGRADE_CODE, 0, iProduct++, ProductCode);
        if (status != ERROR_SUCCESS) {
            break;
        }

        // Get install location
        status = MsiGetProductInfoW(ProductCode, INSTALLPROPERTY_INSTALLLOCATION, InstallPath, &cchInstallPath);
        if (status != ERROR_SUCCESS) {
            continue;
        }

        if (0 != wcscat_s(InstallPath, L"dbgeng.dll")) {
            continue;
        }

        // Try to load dbgeng.dll
        try {
            *phDbgEng = LoadLibraryExW(InstallPath, NULL, 0 /*LOAD_WITH_ALTERED_SEARCH_PATH*/);
         }
         catch (std::exception *e) {
            if (e)
                wprintf(L"Cant Load dbgEng through MSI \n\r");
         }
        if (*phDbgEng == NULL) {
            continue;
        }

       return TRUE;
    }

    return FALSE;
}

BOOL TryLoadDbgEngFromSxSWPDK(HMODULE *phDbgEng)
{
    std::vector<wchar_t> debugPath;
    DWORD cchDebugPath = 0;

    //
    // If the WDK has been installed side-by-side in a lab environment, we
    // can find the location of the debuggers by querying %WDKCONTENTROOT%
    //

    cchDebugPath = ExpandEnvironmentStringsW(L"%WDKCONTENTROOT%", NULL, 0);
    if (cchDebugPath == 0) {
        return FALSE;
    }

    debugPath.resize(cchDebugPath);

    cchDebugPath = ExpandEnvironmentStrings(L"%WDKCONTENTROOT%", &debugPath[0], cchDebugPath);
    if (cchDebugPath == 0 || cchDebugPath > debugPath.size()) {
        return FALSE;
    }

    //
    // Add a trailing backslash if one isn't already present, and lose the
    // terminating nul
    //
    if (debugPath[cchDebugPath - 2] != L'\\') {
        debugPath[cchDebugPath - 1] = L'\\';
    } else {
        debugPath.pop_back();
    }

    //
    // Append the relative path to dbgeng.dll from the debugger root
    //
#ifdef _WIN64
    static const wchar_t szDbgEngRelPath[] = L"Debuggers\\x64\\dbgeng.dll";
#else
    static const wchar_t szDbgEngRelPath[] = L"Debuggers\\x86\\dbgeng.dll";
#endif

    debugPath.insert(
        debugPath.end(),
        szDbgEngRelPath,
        szDbgEngRelPath + (sizeof szDbgEngRelPath / sizeof szDbgEngRelPath[0]));

    *phDbgEng = LoadLibraryExW(&debugPath[0], NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    return (*phDbgEng != NULL);
}

BOOL LoadDbgEng(HMODULE *phDbgEng)
{
    *phDbgEng = NULL;

    //
    // Try to load dbgeng.dll first from the path specified in the registry,
    // and then from the usual DLL load path
    //
    if (!TryLoadDbgEngFromWPDK(phDbgEng)) {
        if (!TryLoadDbgEngFromSxSWPDK(phDbgEng)) {
   //         if (!TryLoadDbgEngFromMSI(phDbgEng)) {
                   *phDbgEng = LoadLibraryExW(L"dbgeng.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
   //                  *phDbgEng = LoadLibraryW(L"C:\\windows\\system32\\dbgeng.dll");
   //         }
        }
    }

    if (*phDbgEng == NULL) {
        // This should be pretty much impossible, since dbgeng.dll ships with the OS
        wprintf(L"Unable to load dbgeng.dll.  Dump may be partially unreadable.\n\r");
        return FALSE;
    }

    return TRUE;
}

void CheckDbgEngVersion(HMODULE hDbgEng)
{
    WCHAR path[MAX_PATH];
    
    if (MAX_PATH == GetModuleFileNameW(hDbgEng, path, MAX_PATH)) {
        return;
    }

    //
    // Read the version resource out of dbgeng.dll
    //

    DWORD cbVerInfo = GetFileVersionInfoSizeW(path, NULL);

    if (cbVerInfo == 0) {
        return;
    }

    std::vector<unsigned char> verInfo(cbVerInfo);

    if (!GetFileVersionInfoW(path, NULL, cbVerInfo, &verInfo[0])) {
        return;
    }

    VS_FIXEDFILEINFO *pFileInfo;
    UINT cbFileInfo;

    if (!VerQueryValueW(&verInfo[0], L"\\", (LPVOID *)&pFileInfo, &cbFileInfo)) {
        return;
    }

    if ((cbFileInfo < sizeof(VS_FIXEDFILEINFO)) ||
        (pFileInfo->dwSignature != SIG_FILEINFO)) {
        return;
    }

    ULONGLONG ver = (((ULONGLONG)pFileInfo->dwFileVersionMS) << 32) | pFileInfo->dwFileVersionLS;
    
    wprintf(L"Debug Eng Version is:  0x%I64x \n", ver);

    if (ver < DBGENG_MIN_VER) {
        wprintf(L"The version of dbgeng.dll on the system is too old, and several features of this tool \
            will not work correctly.  You can get a newer version of dbgeng.dll by installing the Windows \
            Phone Driver Kit.\n\r`");
    }
}

BOOL DbgClient::Initialize(__in PCWSTR pszDump, __in_opt PCWSTR pszSymPath)
{
    HRESULT hr;
    HMODULE hDbgEng;
    CComPtr<IDebugClient4> pClient;
    CComPtr<IDebugControl> pControl;
    CComPtr<IDebugSymbols3> pSymbols;
    CComPtr<IDebugDataSpaces2> pSpaces;

    if (!LoadDbgEng(&hDbgEng)) {
        return FALSE;
    }    

    //
    // Get DebugCreate out of dbgeng.dll
    //

    typedef
    HRESULT
    (STDAPICALLTYPE *PFN_DEBUG_CREATE)(
        __in REFIID InterfaceId,
        __out PVOID* Interface
        );    

    PFN_DEBUG_CREATE pfnDebugCreate = (PFN_DEBUG_CREATE)GetProcAddress(hDbgEng, "DebugCreate");
 

    
    if (pfnDebugCreate == NULL) {
        wprintf(L"Unable to call DebugCreate in dbgeng.dll\n\r");
        return FALSE;
    }

    if (FAILED(hr = (*pfnDebugCreate)(__uuidof(IDebugClient4), (PVOID *)&pClient))) {
        wprintf(L"Failed to create debug client Error 0x%x \n\r", hr);
        CheckDbgEngVersion(hDbgEng);
        return FALSE;
    }

    if (FAILED(hr = pClient->OpenDumpFileWide(pszDump, 0))) {
        wprintf(L"Failed to open dump file with dbgeng Error 0x%x \n\r", hr);
        CheckDbgEngVersion(hDbgEng);
        
        return FALSE;
    }

    if (FAILED(hr = pClient.QueryInterface(&pSpaces)) ||
        FAILED(hr = pClient.QueryInterface(&pSymbols)) ||
        FAILED(hr = pClient.QueryInterface(&pControl))) {

        wprintf(L"Failed to QueryInterface on IDebugClient Error 0x%x \n\r", hr);
        CheckDbgEngVersion(hDbgEng);
        return FALSE;
    }  
   
    if (pszSymPath != NULL) {
        if (FAILED(hr = pSymbols->SetSymbolPathWide(pszSymPath))) {
            wprintf(L"Failed to set symbol path Error 0x%x \n\r", hr);
        }
    }

    // S_FALSE indicates the wait timed out, but that should never happen
    // when the target is a dump file
    if ((hr = pControl->WaitForEvent(0, 60 * 1000)) != S_OK) {
        wprintf(L"Failed to wait for initial debug event Error 0x%x \n\r", hr);
        return FALSE;
    }

    ULONG Class, Qual;
    if (FAILED(hr = pControl->GetDebuggeeType(&Class, &Qual))) {
        wprintf(L"Failed to query dump type Error 0x%x \n\r", hr);

    } else if (Class != DEBUG_CLASS_KERNEL) {
        wprintf(L"Expected kernel mode dump\n\r");
    }

    s_pSpaces = pSpaces.Detach();
    s_pSymbols = pSymbols.Detach();
    return TRUE;
}

IDebugSymbols3 *DbgClient::GetSymbols()
{
    return s_pSymbols;
}

IDebugDataSpaces2 *DbgClient::GetDataSpaces()
{
    return s_pSpaces;
}
