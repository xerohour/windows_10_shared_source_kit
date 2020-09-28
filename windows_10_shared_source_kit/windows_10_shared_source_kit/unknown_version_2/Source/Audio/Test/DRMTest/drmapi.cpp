// Copyright (C) Microsoft. All rights reserved.
#include "common.h"
#include "drmtest.h"

static INT  nCreateCount;
static INT  nDestroyCount;

#pragma warning ( disable : 4800 )


void
GetFormatString
(
    PWAVEFORMATEX   wfx,
    LPTSTR          szFormat
)
{
    BOOL fInvalidFormat = FALSE;
    
    if (szFormat)
    {
        if (wfx)
        {
            if (WAVE_FORMAT_PCM == wfx->wFormatTag)
            {
                _stprintf(szFormat, _T("PCM - %d - %d - %d - %d"),
                    wfx->nSamplesPerSec,
                    wfx->nChannels,
                    wfx->wBitsPerSample,
                    wfx->cbSize);
            }
            else if (WAVE_FORMAT_DRM == wfx->wFormatTag)
            {
                PDRMWAVEFORMAT dwfx = (PDRMWAVEFORMAT) wfx;
                
                _stprintf(szFormat, _T("PCM - %d - %d - %d - %d - 0x%08X - %d"),
                    dwfx->wfxSecure.nSamplesPerSec,
                    dwfx->wfxSecure.nChannels,
                    dwfx->wfxSecure.wBitsPerSample,
                    dwfx->wfxSecure.cbSize,
                    dwfx->ulContentId,
                    dwfx->wfx.nBlockAlign);
            }
            else
            {
                fInvalidFormat = TRUE;
            }
        }
        else
        {
            fInvalidFormat = TRUE;
        }
    }
    
    
    if (fInvalidFormat)
    {
        _tcscpy(szFormat, _T("invalid format"));
    }
} // GetFormatString

#define CASE_ERROR(a)     case a: return _T(#a)

LPTSTR
GetDrmErrorString
(
    DWORD dwErrorCode
)
{
    switch (dwErrorCode)
    {
        CASE_ERROR(DRM_OK);
        CASE_ERROR(DRM_SYSERR);
        CASE_ERROR(DRM_KRM_NOT_RUNNING);
        CASE_ERROR(DRM_KRM_COMMS);
        CASE_ERROR(DRM_FILENOTFOUND);
        CASE_ERROR(DRM_FILEERR);
        CASE_ERROR(DRM_BADFILE);
        CASE_ERROR(DRM_BADTYPE);
        CASE_ERROR(DRM_DEVENUMERATE);
        CASE_ERROR(DRM_DEVDETAIL);
        CASE_ERROR(DRM_KRMNOTFOUND);
        CASE_ERROR(DRM_NOTTRUSTED);
        CASE_ERROR(DRM_FILENOTINCAT);
        CASE_ERROR(DRM_BADHANDLE);
        CASE_ERROR(DRM_AUTHFAILURE);
        CASE_ERROR(DRM_BADDRIVER);
        CASE_ERROR(DRM_BADPARAM);
        CASE_ERROR(DRM_BADKRMCERTSIG);
        CASE_ERROR(DRM_BADKRMCERT);
        CASE_ERROR(DRM_OUTOFMEMORY);
        CASE_ERROR(DRM_BADLIBRARY);
        CASE_ERROR(DRM_MISSINGLIBRARY);
        CASE_ERROR(DRM_MISSINGDLL);
        CASE_ERROR(DRM_BADPROVINGFUNCTION);
        CASE_ERROR(DRM_DATALENGTH);
        CASE_ERROR(DRM_BUFSIZE);
        CASE_ERROR(DRM_BADDRMLEVEL);
        CASE_ERROR(DRM_NODRMATTRIB);
        CASE_ERROR(DRM_RIGHTSNOTSUPPORTED);
        CASE_ERROR(DRM_VERIFIERENABLED);

        CASE_ERROR(KRM_BADIOCTL);
        CASE_ERROR(KRM_BUFSIZE);
        CASE_ERROR(KRM_BADALIGNMENT);
        CASE_ERROR(KRM_BADADDRESS);
        CASE_ERROR(KRM_OUTOFHANDLES);
        CASE_ERROR(KRM_BADSTREAM);
        CASE_ERROR(KRM_SYSERR);
        CASE_ERROR(DRM_BADKRMVERSION);
        CASE_ERROR(KRM_NOTPRIMARY);

        CASE_ERROR(DRM_INVALIDPROVING);
        CASE_ERROR(DRM_BADIMAGE);
    }

    return _T("");
} // GetDrmErrorString

LPTSTR
GetFailureType
(
    DWORD                       dwFailure
)
{
    switch (dwFailure)
    {
        CASE_ERROR(AuthOK);
        CASE_ERROR(AuthNoCert);
        CASE_ERROR(AuthInadequateDRMLevel);
        CASE_ERROR(AuthTampered);
        CASE_ERROR(AuthCantParse);
        CASE_ERROR(AuthBadProvingFunc);
        CASE_ERROR(AuthBadImage);
        CASE_ERROR(AuthNoDrmAttrib);
    }

    return _T("");
} // GetFailureType

void
LogDRMKStats()
{
#ifndef WHQL
    XLOG(XMSG, eWarn1, _T("\n\nKRMProxy Stats"));
    XLOG(XMSG, eWarn1, _T("    Streams Created   : %8d"), nCreateCount);
    XLOG(XMSG, eWarn1, _T("    Streams Destroyed : %8d"), nDestroyCount);
#endif
}

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
DRM_STATUS 
SXDRMKOpen(OUT PDRMKHANDLE pH)
{ 
    DRM_STATUS drmStatus;
    DWORD dwStart = GetTickCount();
    
    XLOG(XMSG, eInfo3, _T("====> DRMKOpen"));
    drmStatus = DRMKOpen(pH);
    XLOG(XMSG, eInfo3, _T("---------> %s(0x%08X) : DRM Handle 0x%08X : (API took %d ms)"), 
        GetDrmErrorString(drmStatus), drmStatus, *pH, GetTickCount() - dwStart);

    return drmStatus;
} // SXDRMKOpen

// ------------------------------------------------------------------------------
DRM_STATUS 
SXDRMKCreateStream
(
    IN     DRMKHANDLE DrmHandle, 
    IN     HANDLE          KsPinHandle,
    IN     IPropertyStore *Properties,
    OUT DWORD* StreamId, 
    OUT PSTREAMHANDLE StreamHandle, 
    IN     PDRMRIGHTS Rights
)
{
    DRM_STATUS drmStatus;
    DWORD dwStart = GetTickCount();

#ifndef BUILD_TAEF
    XLOG(XMSG, eInfo3, _T("====> DRMKCreateStream(0x%08X, ")
        _T("KsPinHandle=0x%08X, IPropertyStore=0x%08X, ")
        _T("ScmsCopyrighted=%s, ScmsOriginal=%s, DigitalOutputDisable=%s)"),
        DrmHandle, 
        KsPinHandle, Properties,
        COPYRIGHT_STRING(Rights), ORIGINAL_STRING(Rights), 
        DIGITAL_STRING(Rights));
#endif // BUILD/TAEF

    drmStatus = DRMKCreateStream(  DrmHandle, KsPinHandle, Properties, 
                            StreamId,  StreamHandle, Rights);

    XLOG(XMSG, eInfo3, _T("---------> %s(0x%08X) : StreamId = 0x%08X : ")
        _T("Stream Handle = 0x%08X : (API took %d ms)"), 
        GetDrmErrorString(drmStatus), drmStatus, *StreamId, *StreamHandle,
        GetTickCount() - dwStart);

    nCreateCount++;

    return drmStatus;
} // SXDRMKCreateStream

// ------------------------------------------------------------------------------
DRM_STATUS 
SXDRMKDestroyStream(IN STREAMHANDLE StreamId)
{
    DRM_STATUS drmStatus;
    DWORD dwStart = GetTickCount();
    
    XLOG(XMSG, eInfo3, _T("====> DRMKDestroyStream(0x%08X)"), StreamId);

    drmStatus = DRMKDestroyStream(StreamId);

    XLOG(XMSG, eInfo3, _T("---------> %s(0x%08X) : (API took %d ms)"), 
        GetDrmErrorString(drmStatus), drmStatus, GetTickCount() - dwStart);

    nDestroyCount++;

    return drmStatus;
} // SXDRMKDestroyStream

// ------------------------------------------------------------------------------
DRM_STATUS
SXDRMKAuthenticate
(
    IN STREAMHANDLE     StreamHandle, 
    BOOL                CheckCat, 
    DWORD               MinDrmLevel, 
    BOOL                RequireCerts, 
    BOOL                CheckImage, 
    PAUTHSTRUCT         Failures, 
    DWORD*              NumFailures, 
    DWORD               MaxFailures,
    BOOL                AllowTestCert
)
{
    DRM_STATUS drmStatus;
    DWORD   dwStart = GetTickCount();

#ifndef BUILD_TAEF
    XLOG(XMSG, eInfo3, 
        _T("====> DRMKAuthenticate(0x%08X, CheckCat=%s, MinDrmLevel=%d, ")
        _T("RequireCerts=%s, CheckImage=%s, AllowTestCert=%s")
        ,StreamHandle, BOOL_STRING(CheckCat),MinDrmLevel,
        BOOL_STRING(RequireCerts), BOOL_STRING(CheckImage),
        BOOL_STRING(AllowTestCert)
        );
#endif // BUILD_TAEF

    drmStatus = DRMKAuthenticate(StreamHandle, CheckCat,
            MinDrmLevel, RequireCerts, CheckImage,
            Failures, NumFailures, MaxFailures,
            AllowTestCert);

    XLOG(XMSG, eInfo3, _T("---------> %s(0x%08X) : (API took %d ms)"), 
        GetDrmErrorString(drmStatus), drmStatus, GetTickCount() - dwStart);

    for (DWORD i = 0; i < *NumFailures; i++)
    {
        XLOG(XMSG, eInfo3, _T("           %-50s - %s(0x%08X) "),
                CString(Failures[i].path),
                GetFailureType(Failures[i].type),
                Failures[i].type);
    }

    return drmStatus;
} // SXDRMKAuthenticate

// ------------------------------------------------------------------------------
DRM_STATUS 
SXDRMKClose
(
    IN DRMKHANDLE H
)
{
    DRM_STATUS drmStatus;
    DWORD dwStart = GetTickCount();
    
    XLOG(XMSG, eInfo3, _T("====> DRMKClose(0x%08X)"), H);

    drmStatus = DRMKClose(H);

    XLOG(XMSG, eInfo3, _T("---------> %s(0x%08X) : (API took %d ms)"), 
        GetDrmErrorString(drmStatus), drmStatus, GetTickCount() - dwStart);

    LogDRMKStats();

    return drmStatus;
} // SXDRMKClose

// ------------------------------------------------------------------------------
DRM_STATUS 
SXDRMKEncryptSamples
(
    IN STREAMHANDLE             StreamHandle, 
    BYTE*                       InData, 
    DWORD                       InDataLength, 
    DWORD*                      pInDataUsed,
    IN PDRMWAVEFORMAT           Wfmt
)
{
    DRM_STATUS drmStatus;
    DWORD dwStart = GetTickCount();
    TCHAR szFormat[MAX_PATH];

    GetFormatString((PWAVEFORMATEX) Wfmt, szFormat);
    XLOG(XMSG, eInfo3, 
        _T("====> DRMKEncryptSamples(0x%08X, 0x%08X, %d, %s)"), 
        StreamHandle, InData, InDataLength, szFormat);

    drmStatus = DRMKEncryptSamples(StreamHandle, InData,
            InDataLength, pInDataUsed, Wfmt);

    XLOG(XMSG, eInfo3, _T("---------> %s(0x%08X) : (API took %d ms)"), 
        GetDrmErrorString(drmStatus), drmStatus, GetTickCount() - dwStart);
    return drmStatus;
} // SXDRMKEncryptSamples

// ------------------------------------------------------------------------------
DRM_STATUS
SXDRMKMakeHeader
(
    IN WAVEFORMATEX*            InFormat,
    OUT DRMWAVEFORMAT*          OutFormat, 
    OUT DWORD*                  OutLen,
    IN DWORD                    MaxLen
)
{
    DRM_STATUS drmStatus;
    DWORD dwStart = GetTickCount();
    TCHAR szFormat[MAX_PATH];

    GetFormatString(InFormat, szFormat);
    XLOG(XMSG, eInfo3, _T("====> DRMKMakeHeader(%s)"), szFormat);
    
    drmStatus = DRMKMakeHeader(InFormat, OutFormat, OutLen, MaxLen);

    GetFormatString((PWAVEFORMATEX) OutFormat, szFormat);
    XLOG(XMSG, eInfo3, _T("---------> %s(0x%08X) : Format = %s : (API took %d ms)"), 
        GetDrmErrorString(drmStatus), drmStatus, szFormat, GetTickCount() - dwStart);

    return drmStatus;
} // SXDRMKMakeHeader



// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
CDrmHelper::CDrmHelper()
{
    m_DrmHandle = NULL;
    SXDRMKOpen(&m_DrmHandle);
} // CDrmHelper

// ------------------------------------------------------------------------------
CDrmHelper::~CDrmHelper()
{
    if (m_DrmHandle)
    {
        SXDRMKClose(m_DrmHandle);
    }
} // CDrmHelper

// ------------------------------------------------------------------------------
BOOL CDrmHelper::Authenticate(STREAMHANDLE StreamHandle, BOOL fIsDrmCompliant)
{
    DWORD   dwDRMRes;

    dwDRMRes = SXDRMKAuthenticate(
            StreamHandle, 
            g_pDRMTest->m_DefaultAuthenticate.ParseCatalogFile, 
	        g_pDRMTest->m_DefaultAuthenticate.MinDRMLevel,
            g_pDRMTest->m_DefaultAuthenticate.RequireSignedCatalog,
	        g_pDRMTest->m_DefaultAuthenticate.CheckDriverImages,
            g_pDRMTest->m_DefaultAuthenticate.DriverFailures,
            &g_pDRMTest->m_DefaultAuthenticate.NumFailures,
            g_pDRMTest->m_DefaultAuthenticate.MaxFailures,
	        g_pDRMTest->m_DefaultAuthenticate.AllowTestCertificate
            );

    if (IS_DRM_SUCCESS(dwDRMRes) != fIsDrmCompliant)
    {
        XLOG(XMSG, eError, _T("%s"),
            fIsDrmCompliant ? 
            _T("Authentication failed on a Drm Compliant Driver") :
            _T("Authentication succeeded on a Non-Drm Compliant Driver"));
        return FALSE;
    }

    return TRUE;    
} // Authenticate

// ------------------------------------------------------------------------------
BOOL
CDrmHelper::Authenticate
(
    STREAMHANDLE                StreamHandle,
    BOOL                        ParseCatalogFile,
    BOOL                        MinDrmLevel,
    BOOL                        RequireSignedCatalog,
    BOOL                        CheckDriverImages,
    PAUTHSTRUCT                 DriverFailures,
    PDWORD                      NumFailures,
    DWORD                       MaxFailure,
    BOOL                        AllowTestCertificate
)
{
    DWORD dwDRMRes = DRM_SYSERR;

    dwDRMRes = SXDRMKAuthenticate(
                    StreamHandle,
                    ParseCatalogFile,
                    MinDrmLevel,
                    RequireSignedCatalog,
                    CheckDriverImages,
                    DriverFailures,
                    NumFailures,
                    MaxFailure,
                    AllowTestCertificate
                    );

    return IS_DRM_SUCCESS(dwDRMRes);
} // Authenticate

// ------------------------------------------------------------------------------
BOOL                
CDrmHelper::CreateStream
(
  HANDLE            KsPinHandle,
  IPropertyStore   *Properties,
  DWORD            *pdwStreamId, 
  PSTREAMHANDLE     pStreamHandle,
  PDRMRIGHTS        pRights
)
{
    if (!m_DrmHandle)
        return FALSE;

    return 0 == SXDRMKCreateStream(m_DrmHandle,
                                   KsPinHandle, Properties,
                                   pdwStreamId, pStreamHandle,
                                   pRights);
    return FALSE;
} // CreateStream

// ------------------------------------------------------------------------------
void CDrmHelper::DestroyStream(STREAMHANDLE StreamHandle)
{
    if (StreamHandle)
    {
        SXDRMKDestroyStream(StreamHandle);
    }        
} // DestroyStream

// ------------------------------------------------------------------------------
BOOL                
CDrmHelper::Encrypt
(
    STREAMHANDLE                StreamHandle,
    PBYTE                       pInData,
    DWORD                       InDataLength,
    PDRMWAVEFORMAT              pWfx
)
{
    DWORD  dwTemp = 0;

    return 0 == SXDRMKEncryptSamples(   
                    StreamHandle, pInData, InDataLength, 
                    &dwTemp, pWfx);
} // Encrypt

// ------------------------------------------------------------------------------
BOOL
CDrmHelper::EncryptBuffer
(
    LPDIRECTSOUNDBUFFER         lpDirectSoundBuffer,
    PDRMWAVEFORMAT              pDrmWaveFormat,
    STREAMHANDLE                StreamHandle
)
{
    BOOL    fResult = TRUE, fLocked = FALSE;
    PBYTE   pData1 = NULL, pData2 = NULL;
    DWORD   cbData1 = 0, cbData2 = 0;
    DWORD   dwDataReturned = 0;

    try
    {
        if (FAILED(lpDirectSoundBuffer->Lock(
                0, 0, 
                (PVOID *) &pData1, &cbData1,
                (PVOID *) &pData2, &cbData2,
                DSBLOCK_ENTIREBUFFER)))
        {
            throw(_T("DirectSoundBuffer Lock failed"));
        }

        fLocked = TRUE;

        if (StreamHandle)
        {
            if (!Encrypt(StreamHandle, pData1, cbData1, pDrmWaveFormat))
            {
                throw(_T("DRMKEncryptSamples failed"));
            }
        }
    }
    catch (LPTSTR szError)
    {
        XLOG(XMSG, eError, _T("Failure condition: %s"), szError);
        fResult = FALSE;
    }

    if (fLocked)
    {
        lpDirectSoundBuffer->Unlock(pData1, cbData1, pData2, cbData2);
    }

    return fResult;
} // EncryptBuffer

// ------------------------------------------------------------------------------
BOOL                
CDrmHelper::MakeHeader
(
    PWAVEFORMATEX               pInFormat,
    PDRMWAVEFORMAT              pOutFormat
)
{
    DWORD   dwMaxLen = sizeof(DRMWAVEFORMAT) + pInFormat->cbSize;//sizeof(*pOutFormat);
    DWORD   dwOutLen = 0;

    return 0 == SXDRMKMakeHeader(pInFormat, pOutFormat, 
                                    &dwOutLen, dwMaxLen);
} // MakeHeader
