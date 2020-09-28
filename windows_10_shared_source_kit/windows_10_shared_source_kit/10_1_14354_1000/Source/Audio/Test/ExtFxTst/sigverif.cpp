// Copyright (C) Microsoft. All rights reserved.
#include "common.h"
#include <SoftPub.h>
#include <WinTrust.h>
#include <MSCat.h>
//#include <cryptuiapi.h>
#include <regstr.h>

#define  ISSUER_REQUIRED    "Digital Media Division"
#define  EKU_REQUIRED       "Placeholder"

LPSTR StringFiletime
(
    FILETIME   *pFileTime
)
{
    static char     szTime[100];
    SYSTEMTIME      SystemTime;
    bool            fPM;
    UINT            uHour;
    
    FileTimeToSystemTime(pFileTime, &SystemTime);
    
    fPM   = (SystemTime.wHour > 11);
    uHour = SystemTime.wHour % 12;
    uHour = ((0 == uHour)?(12):(uHour));
    
    sprintf_s(
        szTime,
        "%02d/%02d/%04d %d:%02d %cM",
        SystemTime.wMonth,
        SystemTime.wDay,
        SystemTime.wYear,
        uHour,
        SystemTime.wMinute,
        (fPM?'P':'A'));
    
    return (szTime);
}

BOOL GetAPOLibraryPath
(
    CAPODevice                     *pDevice,
    __out_ecount(MAX_PATH) LPWSTR   pszFileName
)
{
    char                szPath[MAX_PATH];
    LPSTR               pszTemp;
    HKEY                hKeyObject = NULL;
    HKEY                hKeySub = NULL;
    HMODULE             hMod = NULL;
    UINT                ii;
    DWORD               cbData = sizeof(szPath);
    
    pszFileName[0] = 0;
    lstrcpy(szPath, "SOFTWARE\\Classes\\CLSID\\");
    pszTemp = &(szPath[lstrlen(szPath)]);
    
    {
        GUID    guidClassId;
        WCHAR   szClassId[40];
        
        pDevice->GetClsID(&guidClassId);
        SAL_TERMINATE(szClassId);
        StringFromGUID2(guidClassId, szClassId, sizeof(szClassId)/sizeof(szClassId[0]));
        wcstombs(pszTemp, szClassId, sizeof(szClassId)/sizeof(szClassId[0]));
    }
    
    if (ERROR_SUCCESS != RegOpenKey(HKEY_LOCAL_MACHINE, szPath, &hKeyObject))
    {
        return (FALSE);
    }
    
    do
    {
        if (ERROR_SUCCESS == RegOpenKey(hKeyObject, "LocalServer32", &hKeySub))
        {
            break;
        }
        
        if (ERROR_SUCCESS == RegOpenKey(hKeyObject, "InprocServer32", &hKeySub))
        {
            break;
        }
        
        //  Could not find either LocalServer32 or InprocServer32 subkeys.
        RegCloseKey(hKeyObject);
        return (FALSE);
    }
    while(0);
    RegCloseKey(hKeyObject);
   
    if (ERROR_SUCCESS != RegQueryValueEx(hKeySub, NULL, NULL, NULL, (LPBYTE)(&szPath[0]), &cbData))
    {
        RegCloseKey(hKeySub);
        return (FALSE);
    }
    RegCloseKey(hKeySub);
    
    for (ii = lstrlen(szPath); ii; --ii)
    {
        if ('\\' == szPath[ii - 1])
        {
            //  This is a full path!!
            mbstowcs(pszFileName, szPath, lstrlen(szPath) + 1);
            return (TRUE);
        }
    }
    
    //  Let's use LoadLibraryEx to resolve the path of the library...
    hMod = LoadLibraryEx(szPath, NULL, DONT_RESOLVE_DLL_REFERENCES);
    GetModuleFileName(hMod, szPath, sizeof(szPath)/sizeof(szPath[0]));
    FreeLibrary(hMod);
    
    mbstowcs(pszFileName, szPath, lstrlen(szPath) + 1);
    return (TRUE);
}

BOOL CheckCertFile
(
    LPCSTR      pszInfFilename
)
{
    char                szFindStr[MAX_PATH];
    WIN32_FIND_DATA     FindFileData;
    LPSTR               pszBaseName = NULL;
    PCCERT_CONTEXT      pCertContext = NULL;
    HANDLE              hFind;
    HCERTSTORE          hCert;
    LONG                lTimeValid;
    CAutoBuffer         Buffer;
    UINT                ii;
    BOOL                fSuccess;
    DWORD               cbSize;
    CERT_NAME_INFO     *pCertName = NULL;
    CERT_ENHKEY_USAGE  *pUsage = NULL;
    BOOL                fCertified = TRUE;
    BOOL                fFound;
    
    if (ARRAYSIZE(szFindStr) < lstrlenA(pszInfFilename) + 1)
    {
        return (FALSE);
    }
    
//    pUsage = (CERT_ENHKEY_USAGE*)(pCertName = (CERT_NAME_INFO*)Buffer.Allocate(4096));
    
    lstrcpyA(szFindStr, pszInfFilename);
    
    for (ii = lstrlenA(szFindStr) - 1; ii; --ii)
    {
        if ('\\' == szFindStr[ii])
        {
            pszBaseName = &(szFindStr[ii + 1]);
            break;
        }
    }
    
    if (NULL == pszBaseName)
    {
        SLOG(eError, "FAIL: Invalid .inf path name.");
        return (FALSE);
    }
    lstrcpy(pszBaseName, "*.p7b");
    
    //  Assuming there's precisely ONE such file in this directory.
    hFind = FindFirstFile(szFindStr, &FindFileData);
    if (INVALID_HANDLE_VALUE == hFind)
    {
        SLOG(eError, "FAIL: Unable to find certificate (.p7b) file.");
        return (FALSE);
    }
    FindClose(hFind);
    
    lstrcpyA(pszBaseName, FindFileData.cFileName);
    SLOG(eInfo1, "Found Certificate file: [%s]", szFindStr);
    
    //  Opening certificate file...
    hCert = CertOpenStore(
                CERT_STORE_PROV_FILENAME_A,
                0,
                NULL,
                CERT_STORE_OPEN_EXISTING_FLAG,
                szFindStr);
    
    if (NULL == hCert)
    {
        SLOG(eError, "FAIL: CertOpenStore returned NULL.");
        return (FALSE);
    }
    
    //  Parsing certificate file...
    SLOG(eInfo1, "Parsing certificate file...");
    for (pCertContext = NULL; ; )
    {
        pCertContext = CertEnumCertificatesInStore(hCert, pCertContext);
        
        if (NULL == pCertContext)
        {
            break;
        }
        
        SLOG(eInfo1, "Certificate not valid before: %s.", StringFiletime(&(pCertContext->pCertInfo->NotBefore)));
        SLOG(eInfo1, " Certificate not valid after: %s.", StringFiletime(&(pCertContext->pCertInfo->NotAfter)));
        
        lTimeValid = CertVerifyTimeValidity(NULL, pCertContext->pCertInfo);
        
        if (0 == lTimeValid)
        {
            SLOG(eInfo1, "Certificate is currently active.");
        }
        else if (0 > lTimeValid)
        {
            SLOG(eError, "FAIL: Certificate is not valid yet.");
            fCertified = FALSE;
        }
        else
        {
            SLOG(eError, "FAIL: Certificate has expired.");
            fCertified = FALSE;
        }
       
        //  Getting 'Issuer' information:
        //  Calling with NULL first to find the size...
        cbSize = 0;
        fSuccess = CryptDecodeObjectEx(
            X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
            X509_NAME,
            pCertContext->pCertInfo->Issuer.pbData,
            pCertContext->pCertInfo->Issuer.cbData,
            CRYPT_DECODE_NOCOPY_FLAG,
            NULL,
            (void*)NULL,
            &cbSize);
    
        if (!fSuccess)
        {
            SLOG(eError, "FAIL: Unable to decode issuer information.");
            fCertified = FALSE;
            continue;
        }
    
        if (NULL == (pCertName = (CERT_NAME_INFO*)Buffer.Allocate(cbSize)))
        {
            SLOG(eError, "FAIL: Out of memory.");
            fCertified = FALSE;
            continue;
        }

        //  Calling with adequate buffer...            
        fSuccess = CryptDecodeObjectEx(
            X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
            X509_NAME,
            pCertContext->pCertInfo->Issuer.pbData,
            pCertContext->pCertInfo->Issuer.cbData,
            CRYPT_DECODE_NOCOPY_FLAG,
            NULL,
            (void*)pCertName,
            &cbSize);
        
        if (fSuccess)
        {
            SLOG(eInfo1, "Certificate issued by: [%s] (%d bytes).", pCertName->rgRDN[0].rgRDNAttr[0].Value.pbData, pCertName->rgRDN[0].rgRDNAttr[0].Value.cbData);
            
            if (0 == lstrcmp(ISSUER_REQUIRED, (LPSTR)pCertName->rgRDN[0].rgRDNAttr[0].Value.pbData))
            {
                SLOG(eInfo1, "  Correct issuer (%s): Meets certification check.", ISSUER_REQUIRED);
            }
            else
            {
                SLOG(eError, "  FAIL: incorrect issuer; expecting (%s).", ISSUER_REQUIRED);
                fCertified = FALSE;
            }
        }
        else
        {
            SLOG(eError, "FAIL: Unable to decode issuer information.");
            fCertified = FALSE;
            continue;
        }
        
        //  Getting Enhanced Key Usage Information:
        //  Calling with NULL first to find the size...
        cbSize = 0;
        fSuccess = CertGetEnhancedKeyUsage(pCertContext, 0, NULL, &cbSize);
        
        if (!fSuccess)
        {
            SLOG(eError, "FAIL: Unable to get Enhanced Key Usage information.");
            fCertified = FALSE;
            continue;
        }
        
        if (NULL == (pUsage = (CERT_ENHKEY_USAGE*)Buffer.Allocate(cbSize)))
        {
            SLOG(eError, "FAIL: Out of memory.");
            fCertified = FALSE;
            continue;
        }
        
        //  Calling with adequate buffer.
        CertGetEnhancedKeyUsage(pCertContext, 0, pUsage, &cbSize);
        
        //  Change this later!!  Still don't know the EKU to check.
        SLOG(eInfo1, "Number of usage identifiers: %d", pUsage->cUsageIdentifier);
#if 1
        for (ii = pUsage->cUsageIdentifier, fFound = TRUE; ii; --ii)
#else
        for (ii = pUsage->cUsageIdentifier, fFound = FALSE; ii; --ii)
#endif
        {
            SLOG(eInfo1, "Usage Indentifier[%d]: [%s]", ii - 1, pUsage->rgpszUsageIdentifier[ii - 1]);
            
            if (0 == lstrcmp(EKU_REQUIRED , pUsage->rgpszUsageIdentifier[ii - 1]))
            {
                SLOG(eInfo1, "  Correct identifier: Meets certification check.");
                fFound = TRUE;
            }
        }
        
        if (!fFound)
        {
            SLOG(eError, "FAIL: Unable to find appropriate usage indentifier.");
            fCertified = FALSE;
        }
        
//        if (CryptUIDlgViewContext(CERT_STORE_CERTIFICATE_CONTEXT, pCertContext, NULL, NULL, 0, NULL))
//        {
//            SLOG(eInfo1, "UI!!");
//        }
    }
    
    CertCloseStore(hCert, CERT_CLOSE_STORE_FORCE_FLAG);
    return (fCertified);
}


//--------------------------------------------------------------------------;
//
//  BOOL TrustCheckDriverFileNoCatalog
//
//  Description:
//      Checks the driver file in question without the catalog file.
//      This is less reliable than the check with the catalog file.
//
//  Arguments:
//      WCHAR *pwszDrvFile: Driver file.
//
//  Return (BOOL):
//
//  History:
//      11/13/97    Fwong
//      01/23/06    Fwong       Borrowed code and modified for test.
//
//--------------------------------------------------------------------------;

BOOL WINAPI TrustCheckDriverFileNoCatalog
(
    LPCWSTR     pwszDrvFile
)
{
    GUID                    gDriverSigning = DRIVER_ACTION_VERIFY;
    DRIVER_VER_INFO         dvi;
    WINTRUST_DATA           wtd;
    WINTRUST_FILE_INFO      wtfi;
    HRESULT                 hr;
    OSVERSIONINFO           OSVer;

    ZeroMemory(&wtd, sizeof(WINTRUST_DATA));
    wtd.cbStruct            = sizeof(WINTRUST_DATA);
    wtd.dwUIChoice          = WTD_UI_NONE;
    wtd.fdwRevocationChecks = WTD_REVOKE_NONE;
    wtd.dwUnionChoice       = WTD_CHOICE_FILE;
    wtd.pFile               = &wtfi;
    wtd.pPolicyCallbackData = (LPVOID)&dvi;

    ZeroMemory(&wtfi, sizeof(WINTRUST_FILE_INFO));
    wtfi.cbStruct      = sizeof(WINTRUST_FILE_INFO);
    wtfi.pcwszFilePath = pwszDrvFile;

    ZeroMemory(&dvi, sizeof(DRIVER_VER_INFO));
    dvi.cbStruct = sizeof(DRIVER_VER_INFO);

    ZeroMemory(&OSVer, sizeof(OSVERSIONINFO));
    OSVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    if (GetVersionEx(&OSVer))
    {
        dvi.dwPlatform = OSVer.dwPlatformId;
        dvi.dwVersion  = OSVer.dwMajorVersion;

        dvi.sOSVersionLow.dwMajor  = OSVer.dwMajorVersion;
        dvi.sOSVersionLow.dwMinor  = OSVer.dwMinorVersion;
        dvi.sOSVersionHigh.dwMajor = OSVer.dwMajorVersion;
        dvi.sOSVersionHigh.dwMinor = OSVer.dwMinorVersion;
    }

    hr = WinVerifyTrust(NULL, &gDriverSigning, &wtd);

    if (NULL != dvi.pcSignerCertContext)
    {
        CertFreeCertificateContext(dvi.pcSignerCertContext);
        dvi.pcSignerCertContext = NULL;
    }

    return SUCCEEDED(hr);
} // TrustCheckDriverFileNoCatalog()

//--------------------------------------------------------------------------;
//
//  BOOL TrustCheckDriverFile
//
//  Description:
//      Checks whether the particular file name is certified.
//
//  Arguments:
//      WCHAR *pwszDrvFile:
//
//  Return (BOOL):  TRUE if driver file is certified, FALSE otherwise.
//
//  History:
//      10/17/97    PBerkman        Created.
//      11/12/97    Fwong           API removed; re-structured.
//      01/23/06    Fwong           Borrowed code and modified for test.
//
//--------------------------------------------------------------------------;

BOOL WINAPI TrustCheckDriverFile
(
    LPCWSTR     pwszDrvFile
)
{
    GUID                    gDriverSigning = DRIVER_ACTION_VERIFY;
    HCATADMIN               hCatAdmin;
    HANDLE                  hFile;
    HRESULT                 hr;
    CATALOG_INFO            CatalogInfo;
    DWORD                   cbHash;
    BYTE                    *pHash;
    HCATINFO                hCatInfo, hCatInfoPrev;
    LPCWSTR                 pwszBaseName;
    WINTRUST_DATA           sWTD;
    WINTRUST_CATALOG_INFO   sWTCI;
    DRIVER_VER_INFO         VerInfo;
    OSVERSIONINFO           OSVer;

    if (!(pwszDrvFile))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    pwszBaseName = pwszDrvFile + lstrlenW(pwszDrvFile) - 1;

    while(pwszBaseName > pwszDrvFile && L'\\' != *pwszBaseName)
    {
        pwszBaseName--;
    }

    if(pwszBaseName > pwszDrvFile)
    {
        pwszBaseName++;
    }

    if (!(pwszBaseName))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    if (!(CryptCATAdminAcquireContext(&hCatAdmin, &gDriverSigning, 0)))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    hFile = CreateFileW(
        pwszDrvFile,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if(INVALID_HANDLE_VALUE == hFile)
    {
        CryptCATAdminReleaseContext(hCatAdmin, 0);
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    cbHash = 0;
    pHash  = NULL;
    
    if (!(CryptCATAdminCalcHashFromFileHandle(hFile, &cbHash, pHash, 0)))
    {
        //  Expecting 'not enough memory' error here!!
        if (ERROR_NOT_ENOUGH_MEMORY != GetLastError())
        {
            CloseHandle(hFile);
            CryptCATAdminReleaseContext(hCatAdmin, 0);
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
        
        //  cbHash set to new value by CryptCATAdminCalcHashFromFileHandle
        pHash = new BYTE[cbHash];
        
        if (NULL == pHash)
        {
            CloseHandle(hFile);
            CryptCATAdminReleaseContext(hCatAdmin, 0);
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
        
        if (!(CryptCATAdminCalcHashFromFileHandle(hFile, &cbHash, pHash, 0)))
        {
            //  No excuse now...

            delete []pHash;
            CloseHandle(hFile);
            CryptCATAdminReleaseContext(hCatAdmin, 0);
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
    }

    CatalogInfo.cbStruct = sizeof(CATALOG_INFO);

    for (hCatInfoPrev = NULL;;)
    {
        hCatInfo = CryptCATAdminEnumCatalogFromHash(
                hCatAdmin,
                pHash,
                cbHash,
                0,
                &hCatInfoPrev);

        if (NULL == hCatInfo)
        {
            CloseHandle(hFile);
            delete []pHash;
            CryptCATAdminReleaseContext(hCatAdmin, 0);

            //  We can't seem to get a catalog context, so let's try to check
            //  the driver w/out a catalog file.

            if(TrustCheckDriverFileNoCatalog(pwszDrvFile))
            {
                return TRUE;
            }

            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        CatalogInfo.wszCatalogFile[0] = 0;

        if (!(CryptCATCatalogInfoFromContext(hCatInfo, &CatalogInfo, 0)))
        {
            CloseHandle(hFile);
            delete []pHash;
            CryptCATAdminReleaseCatalogContext(hCatAdmin, hCatInfo, 0);
            CryptCATAdminReleaseContext(hCatAdmin, 0);
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        ZeroMemory(&sWTD, sizeof(WINTRUST_DATA));
        sWTD.cbStruct            = sizeof(WINTRUST_DATA);
        sWTD.dwUIChoice          = WTD_UI_NONE;
        sWTD.fdwRevocationChecks = WTD_REVOKE_NONE;
        sWTD.dwUnionChoice       = WTD_CHOICE_CATALOG;
        sWTD.dwStateAction       = WTD_STATEACTION_AUTO_CACHE;
        sWTD.pPolicyCallbackData = (LPVOID)&VerInfo;
        sWTD.pCatalog            = &sWTCI;

        ZeroMemory(&VerInfo, sizeof(DRIVER_VER_INFO));
        VerInfo.cbStruct = sizeof(DRIVER_VER_INFO);

        ZeroMemory(&sWTCI, sizeof(WINTRUST_CATALOG_INFO));
        sWTCI.cbStruct              = sizeof(WINTRUST_CATALOG_INFO);
        sWTCI.pcwszCatalogFilePath  = CatalogInfo.wszCatalogFile;
        sWTCI.pcwszMemberTag        = pwszBaseName;
        sWTCI.pcwszMemberFilePath   = pwszDrvFile;
        sWTCI.hMemberFile           = hFile;
        sWTCI.pbCalculatedFileHash  = pHash;
        sWTCI.cbCalculatedFileHash  = cbHash;

        ZeroMemory(&OSVer, sizeof(OSVERSIONINFO));
        OSVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

        if (GetVersionEx(&OSVer))
        {
            VerInfo.dwPlatform = OSVer.dwPlatformId;
            VerInfo.dwVersion  = OSVer.dwMajorVersion;

            VerInfo.sOSVersionLow.dwMajor  = OSVer.dwMajorVersion;
            VerInfo.sOSVersionLow.dwMinor  = OSVer.dwMinorVersion;
            VerInfo.sOSVersionHigh.dwMajor = OSVer.dwMajorVersion;
            VerInfo.sOSVersionHigh.dwMinor = OSVer.dwMinorVersion;
        }

        hr = WinVerifyTrust(NULL, &gDriverSigning, &sWTD);

        if (NULL != VerInfo.pcSignerCertContext)
        {
            CertFreeCertificateContext(VerInfo.pcSignerCertContext);
            VerInfo.pcSignerCertContext = NULL;
        }

        if (hr == ERROR_SUCCESS)
        {
            CloseHandle(hFile);
            delete []pHash;
            CryptCATAdminReleaseCatalogContext(hCatAdmin, hCatInfo, 0);
            CryptCATAdminReleaseContext(hCatAdmin, 0);
            return TRUE;
        }

        if (NULL == hCatInfoPrev)
        {
            CloseHandle(hFile);
            delete []pHash;
            CryptCATAdminReleaseCatalogContext(hCatAdmin, hCatInfo, 0);
            CryptCATAdminReleaseContext(hCatAdmin, 0);
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
    }

    //  We should never get here...
#pragma prefast(suppress:28940, "We should never get here, but if we ever change the above code, we might.") 
    CloseHandle(hFile);
    delete []pHash;
    CryptCATAdminReleaseContext(hCatAdmin, 0);
    return FALSE;
} // TrustCheckDriverFile()

#if 0
//--------------------------------------------------------------------------;
//
//  void GetFullInfPath
//
//  Description:
//      Gets the full path to an .inf file.  This will be either:
//          [WINDOWS]\INF or [WINDOWS]\INF\OTHER.
//
//  WARNING!!!:  This will write over the current contents of the buffer.
//
//  Arguments:
//      LPTSTR pszInf: Pointer to the base inf file AND destination for
//                     full path.
//
//  Return (void):
//
//  History:
//      10/29/97    Fwong       Ported from AndyRaf.
//
//--------------------------------------------------------------------------;

void GetFullInfPath
(
    LPTSTR  pszInf
)
{
    HANDLE      hFile;
    TCHAR       szFullPath[MAX_PATH];
    TCHAR       szWinPath[MAX_PATH];

    if (!pszInf) return;
    if (!GetWindowsDirectory(szWinPath, sizeof(szWinPath)/sizeof(szWinPath[0]))) return;

    //  Assuming the [WINDOWS]\INF directory...

    lstrcpy(szFullPath, szWinPath);
    lstrcat(szFullPath, TEXT("\\INF\\"));
    lstrcat(szFullPath, pszInf);

    //  Checking if it exists...

    hFile = CreateFile(
                szFullPath,
                0,
                FILE_SHARE_READ,
                NULL,
                OPEN_EXISTING,
                0,
                NULL);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        lstrcpy(szFullPath, szWinPath);
        lstrcat(szFullPath, TEXT("\\INF\\OTHER\\"));
        lstrcat(szFullPath, pszInf);

        hFile = CreateFile(
                    szFullPath,
                    0,
                    FILE_SHARE_READ,
                    NULL,
                    OPEN_EXISTING,
                    0,
                    NULL);

        if(INVALID_HANDLE_VALUE == hFile)
        {
            return;
        }

        CloseHandle(hFile);
    }
    else
    {
        CloseHandle(hFile);
    }

    lstrcpy(pszInf, szFullPath);
} // GetFullInfPath()

UINT CALLBACK enumFile
(
    PVOID    pContext,
    UINT     uNotification,
    UINT_PTR uParam1,
    UINT_PTR uParam2
)
{
    PTCHAR      pszFile = (PTCHAR)uParam1;
    
    SLOG(eInfo1, "enumFile->File: %s", pszFile);
    
    return (NO_ERROR);
}

BOOL CheckFileFromQueue
(
    HSPFILEQ    hFileQ
)
{
    BOOL    fSuccess;
    DWORD   dw;

    fSuccess = SetupScanFileQueue(
                hFileQ,
                SPQ_SCAN_USE_CALLBACK,
                NULL,
                enumFile,
                NULL,
                &dw);
        
    return (fSuccess);
}

BOOL CheckDriverFileInInf
(
    LPSTR       pszFilename,
    LPSTR       pszAttachedDevice
)
{
    SP_DEVINFO_DATA                     DevInfoData;
    SP_DRVINFO_DATA                     DrvInfoData;
    SP_DEVICE_INTERFACE_DATA            did;
    SP_DEVINSTALL_PARAMS                InstParams;
    HKEY                                hKeyDev;
    HSPFILEQ                            hFileQ;
    HDEVINFO                            hDevInfo;
    UINT                                ii;
    DWORD                               cbSize, dw, dwType;
    BOOL                                fSuccess;
    CAutoBuffer                         Buffer_pidd(MAX_PATH + sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA));
    PSP_DEVICE_INTERFACE_DETAIL_DATA    pdidd;
    GUID                                guidClass = KSCATEGORY_AUDIO;
    char                                szSection[MAX_PATH];
    char                                szDevInst[MAX_PATH];
    
    cbSize = MAX_PATH + sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
    
    //  Whack off the '{2].' prefix.
    pszAttachedDevice += 4;
    
    //  Lop off the path from the binary.
    for (ii = lstrlenA(pszFilename) - 1; ii; --ii)
    {
        if ('\\' == pszFilename[ii])
        {
            pszFilename = &(pszFilename[ii + 1]);
            break;
        }
    }
    
    if (NULL == (pdidd = (PSP_DEVICE_INTERFACE_DETAIL_DATA)Buffer_pidd.GetBuffer()))
    {
        return (FALSE);
    }

    pdidd->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
    
    hDevInfo = SetupDiGetClassDevs(
            &guidClass,
            NULL,
            NULL,
            DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
    
    if (INVALID_HANDLE_VALUE == hDevInfo)
    {
        return (FALSE);
    }
    
    SecureZeroMemory(&did, sizeof(did));
    did.cbSize = sizeof(did);
    
    SecureZeroMemory(&DevInfoData, sizeof(DevInfoData));
    DevInfoData.cbSize    = sizeof(DevInfoData);
    DevInfoData.ClassGuid = KSCATEGORY_AUDIO;
    
    for (ii = 0; ; ++ii)
    {
        fSuccess = SetupDiEnumDeviceInterfaces(
            hDevInfo,
            NULL,
            &guidClass,
            ii,
            &did);
    
        if (!fSuccess)
        {
            break;
        }
        
        fSuccess = SetupDiGetDeviceInterfaceDetail(
            hDevInfo,
            &did,
            pdidd,
            cbSize,
            &dw,
            &DevInfoData);
    
        if (!fSuccess)
        {
            break;
        }
        
        if (0 == lstrcmpi(pdidd->DevicePath, pszAttachedDevice))
        {
            //  Just being paranoid and making sure the case of the
            //  string matches

            fSuccess = TRUE;
            break;
        }
    }
    
    if (!fSuccess)
    {
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return (FALSE);
    }
    
    //  Need device instance ID to open device info.
    if (!SetupDiGetDeviceInstanceId(
        hDevInfo,
        &DevInfoData,
        szDevInst,
        sizeof(szDevInst),
        NULL))
    {
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return (FALSE);
    }
    
    SetupDiDestroyDeviceInfoList(hDevInfo);
    
    //  Creating a device info list and open up device info element for
    //  a device within that set.
    hDevInfo = SetupDiCreateDeviceInfoList(NULL, NULL);
    
    if(INVALID_HANDLE_VALUE == hDevInfo)
    {
        return (FALSE);
    }

    SecureZeroMemory(&DevInfoData, sizeof(DevInfoData));
    DevInfoData.cbSize     = sizeof(DevInfoData);

    if(!SetupDiOpenDeviceInfo(
        hDevInfo,
        szDevInst,
        NULL,
        0,
        &DevInfoData))
    {
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return (FALSE);
    }

    ZeroMemory(&InstParams, sizeof(InstParams));
    InstParams.cbSize = sizeof(InstParams);
    
    //  Getting current settings, we'll be modifying some fields.
    if(!SetupDiGetDeviceInstallParams(hDevInfo, &DevInfoData, &InstParams))
    {
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return (FALSE);
    }

    hKeyDev = SetupDiOpenDevRegKey(
        hDevInfo,
        &DevInfoData,
        DICS_FLAG_GLOBAL,
        0,
        DIREG_DRV,
        KEY_ALL_ACCESS);

    if(INVALID_HANDLE_VALUE == hKeyDev)
    {
        SetupDiDestroyDeviceInfoList(hDevInfo);
        ii = GetLastError();
        return (FALSE);
    }

    //  Getting INF path and setting bit that says that we'll be using a
    //  single inf (vs directory).
    cbSize = sizeof(InstParams.DriverPath);
    ii = RegQueryValueEx(
        hKeyDev,
        REGSTR_VAL_INFPATH,
        NULL,
        &dwType,
        (LPBYTE)InstParams.DriverPath,
        &cbSize);

    if(ERROR_SUCCESS != ii)
    {
        RegCloseKey(hKeyDev);
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return (FALSE);
    }

    GetFullInfPath(InstParams.DriverPath);

    cbSize = sizeof(szSection);
    ii = RegQueryValueEx(
        hKeyDev,
        REGSTR_VAL_INFSECTION,
        NULL,
        &dwType,
        (LPBYTE)szSection,
        &cbSize);

    if(ERROR_SUCCESS != ii)
    {
        RegCloseKey(hKeyDev);
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return (FALSE);
    }
    
    InstParams.Flags |= DI_ENUMSINGLEINF;

    if(!SetupDiSetDeviceInstallParams(hDevInfo, &DevInfoData, &InstParams))
    {
        RegCloseKey(hKeyDev);
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return (FALSE);
    }

    //  Building class driver info list.
    if(!SetupDiBuildDriverInfoList(hDevInfo, &DevInfoData, SPDIT_CLASSDRIVER))
    {
        RegCloseKey(hKeyDev);
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return (FALSE);
    }

    //  Filling out DrvInfoData structure.
    ZeroMemory(&DrvInfoData, sizeof(DrvInfoData));

    cbSize = sizeof(DrvInfoData.ProviderName);
    ii = RegQueryValueEx(
        hKeyDev,
        REGSTR_VAL_PROVIDER_NAME,
        NULL,
        &dwType,
        (LPBYTE)DrvInfoData.ProviderName,
        &cbSize);

    if(ERROR_SUCCESS != ii)
    {
        DrvInfoData.ProviderName[0] = (TCHAR)(0);
    }

    if(!SetupDiGetDeviceRegistryProperty(
        hDevInfo,
        &DevInfoData,
        SPDRP_MFG,
        NULL,
        (PBYTE)DrvInfoData.MfgName,
        sizeof(DrvInfoData.MfgName),
        NULL))
    {
        RegCloseKey(hKeyDev);
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return (FALSE);
    }

    if(!SetupDiGetDeviceRegistryProperty(
        hDevInfo,
        &DevInfoData,
        SPDRP_DEVICEDESC,
        NULL,
        (PBYTE)DrvInfoData.Description,
        sizeof(DrvInfoData.Description),
        NULL))
    {
        RegCloseKey(hKeyDev);
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return (FALSE);
    }

    DrvInfoData.cbSize     = sizeof(SP_DRVINFO_DATA);
    DrvInfoData.DriverType = SPDIT_CLASSDRIVER;
    DrvInfoData.Reserved   = 0;

    // Search for the driver and select it if found
    if(!SetupDiSetSelectedDriver(hDevInfo, &DevInfoData, &DrvInfoData))
    {
        RegCloseKey(hKeyDev);
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return (FALSE);
    }

    RegCloseKey(hKeyDev);

    //  Setting up file queue.
    hFileQ = SetupOpenFileQueue();

    if(INVALID_HANDLE_VALUE == hFileQ)
    {
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return (FALSE);
    }

    ZeroMemory(&InstParams, sizeof(SP_DEVINSTALL_PARAMS));
    InstParams.cbSize = sizeof(SP_DEVINSTALL_PARAMS);

    //  Setting up a user-supplied queue and setting the bit to signify
    if(!SetupDiGetDeviceInstallParams(hDevInfo, &DevInfoData, &InstParams))
    {
        SetupCloseFileQueue(hFileQ);
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return (FALSE);
    }

    //  Adding options...
    InstParams.Flags     |= DI_NOVCP;
    InstParams.FileQueue  = hFileQ;

    if(!SetupDiSetDeviceInstallParams(hDevInfo, &DevInfoData, &InstParams))
    {
        SetupCloseFileQueue(hFileQ);
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return (FALSE);
    }

    //  This fills up the queue.
    if(!SetupDiCallClassInstaller(
            DIF_INSTALLDEVICEFILES,
            hDevInfo,
            &DevInfoData))
    {
        SetupCloseFileQueue(hFileQ);
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return (FALSE);
    }

    //
    //  Removing options.  If we don't do this before closing the file queue
    //  and destroying the device info list, we get memory leaks in setupapi.
    //
    InstParams.Flags     &= (~DI_NOVCP);
    InstParams.FileQueue  = NULL;

    if(!SetupDiSetDeviceInstallParams(hDevInfo, &DevInfoData, &InstParams))
    {
        SetupCloseFileQueue(hFileQ);
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return (FALSE);
    }

    //  Checks the files in the queue
    fSuccess = CheckFileFromQueue(hFileQ);

    SetupCloseFileQueue(hFileQ);
    SetupDiDestroyDeviceInfoList(hDevInfo);

    return fSuccess;
}

void RemapEndpoints()
{
    DWORD                           dwRet = FNS_PASS;
    UINT                            iDevice;
    CComPtr<IMMDeviceEnumerator>    pIDeviceEnumerator = NULL;
    CComPtr<IMMDeviceCollection>    pDevices = NULL;
    char                            szName[MAX_PATH];
    CAPODevice                     *pAPODevice;
    HRESULT                         hr;
    
    if (S_OK != (hr = pIDeviceEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator))))
    {
        SLOG(eError, "CoCreateInstance returned error (0x%08lx).", hr);
        return;
    }
    
    if (S_OK != (hr = pIDeviceEnumerator->EnumAudioEndpoints(eAll, DEVICE_STATEMASK_INTERNAL, &pDevices)))
    {
        SLOG(eError, "IMMDeviceEnumerator::EnumAudioEndpoints returned error (0x%08lx).", hr);
        return;
    }
    
    if (S_OK != (hr = pDevices->GetCount(&iDevice)))
    {
        SLOG(eError, "IMMDeviceCollection::GetCount returned error (0x%08lx).", hr);
        return;
    }
    
    for (; iDevice; iDevice--)
    {
        CComPtr<IMMDevice>              pIMMDevice = NULL;
        CComPtr<IDeviceTopology>        pIDTEP = NULL;
        CComPtr<IConnector>             pIConnectorEP = NULL;
        CCTMString                      strDeviceId = NULL;
        
        if (S_OK != (hr = pDevices->Item(iDevice - 1, &pIMMDevice)))
        {
            SLOG(eError, "IMMDeviceCollection::Item returned error (0x%08lx).", hr);
            return;
        }
        
        GetEndpointFriendlyNameA(pIMMDevice, szName, sizeof(szName)/sizeof(szName[0]));
        
        if (S_OK != (hr = pIMMDevice->Activate(__uuidof(IDeviceTopology), CLSCTX_INPROC_SERVER, NULL, (void**)&pIDTEP)))
        {
            SLOG(eError, "IMMDevice::Activate returned error (0x%08lx).", hr);
            return;
        }
        
        if (S_OK != (hr = pIDTEP->GetConnector(0, &pIConnectorEP)))
        {
            SLOG(eError, "IDeviceTopology::GetConnector returned error (0x%08lx).", hr);
            return;
        }
        
        if (S_OK != (hr = pIConnectorEP->GetDeviceIdConnectedTo(&strDeviceId)))
        {
            SLOG(eError, "IDeviceTopology::GetDeviceIdConnectedTo returned error (0x%08lx).", hr);
            return;
        }
        
        SLOG(eInfo1, "Endpoint: %s", szName);
        SLOG(eInfo1, "  Connected to: %ls", strDeviceId);
    }
    
    return;
}
#endif

#ifndef BUILD_TAEF 
DWORD WINAPI TC_VerifySignature()
{
    CAPODevice         *pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    WCHAR               wszBinPath[MAX_PATH];
    char                szBinPath[MAX_PATH];
    char                szInfPath[MAX_PATH];
    
    g_IShell->GetString(szInfPath, sizeof(szInfPath), FNSSTRING_COMMANDLINE);
    
    if (0 == lstrlen(szInfPath))
    {
        XLOG(XFAIL, eError, "FAIL: Expecting INF file path as parameter.");
        return (FNS_PASS);
    }
    SLOG(eInfo1, "Application parameter: [%s]", szInfPath);
    
    if (!GetAPOLibraryPath(pCurrentDevice, wszBinPath))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to resolve the SysFx library path.");
        return (FNS_PASS);
    }
    SLOG(eInfo1, "Resolved filename of library: [%ls]", wszBinPath);
    wcstombs(szBinPath, wszBinPath, MAX_PATH);
    
    //  Check the cert...
    SLOG(eInfo1, "\r\nChecking if certificate file meets requirements...");
    if (CheckCertFile(szInfPath))
    {
        SLOG(eInfo1, "Certificate file meets requirements.");
    }
    else
    {
        XLOG(XFAIL, eError, "FAIL: Certificate does not meet requirements.");
    }
    
    //  Verifying APO library is signed (sigverif.exe)...
    SLOG(eInfo1, "\r\nChecking if SysFx library is signed...");
    if (!TrustCheckDriverFile(wszBinPath))
    {
        XLOG(XFAIL, eError, "FAIL: Library is not signed.");
    }
    else
    {
        SLOG(eInfo1, "Library is signed.");
    }
    
    //  Binary compare files...
    {
        char            szInfBinPath[MAX_PATH];
        UINT            ii;
        
        if (ARRAYSIZE(szInfBinPath) > lstrlen(szInfPath) + 1)
        {
            lstrcpy(szInfBinPath, szInfPath);
        }
        else
        {
            SLOG(eError, "  FAIL: Inf Path longer than expected (%d chars).", ARRAYSIZE(szInfBinPath));
            return (FNS_FAIL);
        }
        
        //  Whack off the file so we can get the path...
        for (ii = lstrlenA(szInfBinPath); ii; --ii)
        {
            if ('\\' == szInfBinPath[ii - 1])
            {
                szInfBinPath[ii] = 0;
                break;
            }
        }
        
        for (ii = lstrlenA(szBinPath); ii; --ii)
        {
            if ('\\' == szBinPath[ii - 1])
            {
                lstrcat(szInfBinPath, &(szBinPath[ii]));
                break;
            }
        }
        
        SLOG(eInfo1, "\r\nBinary comparing [%s] vs [%s]:", szBinPath, szInfBinPath);
        if (FileCompare(szBinPath, szInfBinPath))
        {
            SLOG(eInfo1, "  File contents are identical.");
        }
        else
        {
            XLOG(XFAIL, eError, "  FAIL: Installed and .inf pathed files are different.");
        }
    }
    
    //  Verify file is in .INF...
//    CheckDriverFileInInf(szBinPath, pCurrentDevice->GetAttachedDevice());
//    SLOG(eInfo1, "Attached DeviceInterface to this device: %s", pCurrentDevice->GetAttachedDevice());
//    RemapEndpoints();
    return (FNS_PASS);
}
#endif

DWORD WINAPI TC_DisplayFilelist()
{
    CAPODevice         *pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    WCHAR               wszBinPath[MAX_PATH];
    char                szBinPath[MAX_PATH];
    LPSTR               pszBinary = NULL;
    
    if (!GetAPOLibraryPath(pCurrentDevice, wszBinPath))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to resolve the SysFx library path.");
        return (FNS_PASS);
    }

    SLOG(eInfo1, "Start PEAttribAudio Files");
    
    wcstombs(szBinPath, wszBinPath, MAX_PATH);
    pszBinary = &szBinPath[lstrlen(szBinPath)];
    for (; '\\' != *pszBinary; --pszBinary);
    pszBinary++;
    SLOG(eInfo1, "%s:PETrusted:1", pszBinary);
    SLOG(eInfo1, "%s:PageHashes=True", pszBinary);
    
    SLOG(eInfo1, "End PEAttribAudio Files");
    
    return (FNS_PASS);
}

