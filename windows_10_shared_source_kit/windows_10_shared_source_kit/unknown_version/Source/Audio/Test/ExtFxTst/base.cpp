//--------------------------------------------------------------------------;
//
//  File: Base.cpp
//
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.
//
//  Abstract:
//      Base classes for SysFx objects and helper functions.
//
//  Contents:
//
//  History:
//      11/04/2005  Fwong       Initial implementation.
//
//--------------------------------------------------------------------------;
#include "common.h"
#include "propkey.h"
#include <AudioClient.h>    // for AUDCLNT_BUFFERFLAGS_SILENT
#include <functiondiscoverykeys.h>
#include <wfx.h>

//typedef struct _UNCOMPRESSEDAUDIOFORMAT
//    {
//    GUID guidFormatType;
//    DWORD dwSamplesPerFrame;
//    DWORD dwBytesPerSampleContainer;
//    DWORD dwValidBitsPerSample;
//    FLOAT fFramesPerSecond;
//    DWORD dwChannelMask;
//    } 	UNCOMPRESSEDAUDIOFORMAT;
//

#define TESTFORMATFLAG_NATIVE       0x00000001
#define TESTFORMATFLAG_SUGGEST      0x00000002

const ULONG HNSTIME1MSEC = 10000;

typedef struct __TESTFORMAT
{
    DWORD                       fdwFlags;
    UNCOMPRESSEDAUDIOFORMAT     format;
} TESTFORMAT;

TESTFORMAT  gaTestFormats[] = 
{   
    { TESTFORMATFLAG_NATIVE, { KSDATAFORMAT_SUBTYPE_PCM,        2, 2, 16, 44100, 0} },
    { TESTFORMATFLAG_NATIVE, { KSDATAFORMAT_SUBTYPE_PCM,        2, 2, 16, 48000, 0} },
    { TESTFORMATFLAG_NATIVE, { KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 2, 4, 32, 44100, 0} },
//    { TESTFORMATFLAG_NATIVE, { KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 2, 4, 32, 48000, 0} },
    { TESTFORMATFLAG_NATIVE, { KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 6, 4, 32, 48000, 0} },
};

BOOL InRangeScalar
(
    float       fValue,
    float       fCompare,
    float       fTolerance
)
{
    if (fCompare < (fValue - fTolerance))
    {
        return (FALSE);
    }
    
    if (fCompare > (fValue + fTolerance))
    {
        return (FALSE);
    }
    
    return (TRUE);
}

CCTMString::CCTMString(__inout_opt LPWSTR pstrString)
: m_pstrString(NULL)
{
    m_pstrString = pstrString;
}

CCTMString::~CCTMString()
{ 
    if (NULL != m_pstrString)
    {
        CoTaskMemFree(m_pstrString);
        m_pstrString = NULL;
    }
}

LPWSTR CCTMString::operator=(__inout_opt LPWSTR pstrString)
{
    if (NULL != m_pstrString)
    {
        CoTaskMemFree(m_pstrString);
        m_pstrString = NULL;
    }

    m_pstrString = pstrString;
    
    return (m_pstrString);
}

LPWSTR CCTMString::operator=(__inout_opt LPSTR pstrString)
{
    UINT    cchSize = (lstrlenA(pstrString) + 1);

    if (NULL != m_pstrString)
    {
        CoTaskMemFree(m_pstrString);
        m_pstrString = NULL;
    }

    m_pstrString = (LPWSTR)CoTaskMemAlloc(cchSize * sizeof(WCHAR));
    
    if (NULL != m_pstrString)
    {
        mbstowcs(m_pstrString, pstrString, cchSize);
    }
    
    return (m_pstrString);
}

void CCTMString::JumbleCase()
{
    UINT    iLen;
    WCHAR   ch;

    if (NULL != m_pstrString)
    {
        for (iLen = lstrlenW(m_pstrString); iLen; iLen--)
        {
            if (0 == (g_IShell->Random() % 2))
            {
                //  Toggle!!
                
                ch = m_pstrString[iLen - 1];
                
                if ((ch >= 'A') && (ch <= 'Z'))
                {
                    m_pstrString[iLen - 1] = 'a' + ch - 'A';
                }
                
                if ((ch >= 'a') && (ch <= 'z'))
                {
                    m_pstrString[iLen - 1] = 'A' + ch - 'a';
                }
            }
        }
    }
}

//IAudioMediaType *GetAMTFormat
//(
//    UNCOMPRESSEDAUDIOFORMAT    *pFormat
//)
//{
//    IAudioMediaType    *pAMTFormat = NULL;
//    HRESULT             hr;
//
//    hr = CreateAudioMediaTypeFromUncompressedAudioFormat(pFormat, &pAMTFormat);
//
//    if (S_OK == hr)
//    {
//        return (pAMTFormat);
//    }
//    else
//    {
//        SLOG(eError, "Could not instatiate IAudioMediaType for format; CreateAudioMediaTypeFromUncompressedAudioFormat() returned error (0x%08lx).", hr);
//        return (NULL);
//    }
//}

void GetFormatString
(
    UNCOMPRESSEDAUDIOFORMAT    *pFormat,
    __out_ecount(100) LPSTR     pszFormat
)
{
    sprintf_s(
        pszFormat,
        sizeof(pszFormat),
        "%d Hz, %d channels, %d bits",
        ((UINT)pFormat->fFramesPerSecond),
        pFormat->dwSamplesPerFrame,
        pFormat->dwValidBitsPerSample);
}

CTestFormats::CTestFormats()
: m_cFormats(0)
{
    m_cFormats = (sizeof(gaTestFormats)/sizeof(gaTestFormats[0]));
}

UNCOMPRESSEDAUDIOFORMAT *CTestFormats::GetFormat
(
    UINT    iFormat
)
{
    if (iFormat < m_cFormats)
    {
        return (&(gaTestFormats[iFormat].format));
    }
    else
    {
        return (NULL);
    }
}

HRESULT CTestFormats::GetAMTFormat
(
    UINT                iFormat,
    IAudioMediaType   **ppIAMTFormat
)
{
    if (NULL == ppIAMTFormat)
    {
        return (E_POINTER);
    }
    
    *ppIAMTFormat = NULL;

    if (iFormat < m_cFormats)
    {
        HRESULT     hr;
    
        hr = CreateAudioMediaTypeFromUncompressedAudioFormat(
                GetFormat(iFormat),
                ppIAMTFormat);
        
        return (hr);
    }
    else if (iFormat == m_cFormats)
    {
        //  This is the last case where format should be NULL...
        return (S_OK);
    }
    else
    {
        return (E_INVALIDARG);
    }
}

DWORD CTestFormats::GetFormatFlags
(
    UINT    iFormat
)
{
    if (iFormat < m_cFormats)
    {
        return (gaTestFormats[iFormat].fdwFlags);
    }
    else
    {
        //  NULL formats (open) must always be suggested, no?
        return (TESTFORMATFLAG_SUGGEST);
    }
}

void CTestFormats::GetFormatString
(
    UINT                        iFormat,
    __out_ecount(256) LPSTR     pszFormat
)
{
    UNCOMPRESSEDAUDIOFORMAT  *pFormat = GetFormat(iFormat);
    
    if (iFormat < m_cFormats)
    {
        ::GetFormatString(pFormat, pszFormat);
    }
    else
    {
        lstrcpy(pszFormat, "No format");
    }
}

void APOTest_Initialize
(
    GUID        guidClassId,
    UINT32      cbAPOInit,
    BYTE       *pAPOInit,
    LPCSTR      pszLabel,
    HRESULT     hrExpected,
    LPCSTR      pszHRLabel
)
{
    CComPtr<IAudioProcessingObject>     pIAPOTest = NULL;
    HRESULT                             hr;

    if (S_OK != (hr = pIAPOTest.CoCreateInstance(guidClassId)))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: CoCreateInstance returned error (0x%08lx).",
            hr);
    }
    else
    {
        hr = pIAPOTest->Initialize(cbAPOInit, pAPOInit);
        
        if (hrExpected == hr)
        {
            SLOG(
                eInfo1,
                "Initialize %s returns %s.",
                pszLabel,
                pszHRLabel);
        }
        else
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: Initialize %s returns error (0x%08lx) expecting %s.",
                pszLabel,
                hr,
                pszHRLabel);
        }
    }
}

void InterleaveChannel
(
    LPBYTE      pDestination,
    LPBYTE      pSource,
    UINT        iOffset,
    UINT        iTotal,
    UINT        cSamples,
    UINT        cBytesPerSample
)
{
    switch (cBytesPerSample)
    {
        case 1:
        {
            //  8-bit samples...
        
            LPBYTE  pDst = (LPBYTE)pDestination;
            LPBYTE  pSrc = (LPBYTE)pSource;
        
            pDst = &(pDst[iOffset]);
            for (; cSamples; --cSamples)
            {
                *pDst = *pSrc;
                pDst += iTotal;
                pSrc++;
            }
            break;
        }
            
        case 2:
        {
            //  16-bit samples...
            
            LPWORD  pDst = (LPWORD)pDestination;
            LPWORD  pSrc = (LPWORD)pSource;
        
            pDst = &(pDst[iOffset]);
            for (; cSamples; --cSamples)
            {
                *pDst = *pSrc;
                pDst += iTotal;
                pSrc++;
            }
            break;
        }
            
        case 4:
        {
            //  32-bit (float) samples...
        
            float  *pDst = (float*)pDestination;
            float  *pSrc = (float*)pSource;
        
            pDst = &(pDst[iOffset]);
            for (; cSamples; --cSamples)
            {
                *pDst = *pSrc;
                pDst += iTotal;
                pSrc++;
            }
            break;
        }
            
        default:
            break;
    }
}

CAudioBuffer::CAudioBuffer()
: m_pData(NULL), m_cbData(0), m_pFormat(NULL)
{
    
}

CAudioBuffer::CAudioBuffer
(
    IAudioMediaType      *pFormat
)
: m_pData(NULL), m_cbData(0), m_pFormat(NULL)
{
    SetFormat(pFormat);
}

void CAudioBuffer::SetFormat
(
    IAudioMediaType      *pFormat
)
{
    m_pFormat = pFormat;
    m_pFormat->AddRef();    
}

CAudioBuffer::~CAudioBuffer()
{
    if (NULL != m_pData)
    {
        delete[] m_pData;
        m_pData = NULL;
    }
    
    m_pFormat->Release();
}

BOOL CAudioBuffer::AllocateBuffer
(
    float   fSeconds
)
{
    UNCOMPRESSEDAUDIOFORMAT     format;
    UINT                        uBlockAlign;
    UINT                        uSamples;

    if (NULL == m_pFormat)
    {
        return (FALSE);
    }

    if (NULL != m_pData)
    {
        delete[] m_pData;
        m_pData  = NULL;
        m_cbData = 0;
    }
    
    if (S_OK != m_pFormat->GetUncompressedAudioFormat(&format))
    {
        return (FALSE);
    }
    
    uBlockAlign = format.dwSamplesPerFrame * format.dwBytesPerSampleContainer;
    uSamples    = (UINT)(fSeconds * format.fFramesPerSecond);
    m_cbData    = uBlockAlign * uSamples;
    m_pData     = (LPVOID)(new BYTE[m_cbData]);
    
    if (NULL == m_pData)
    {
        m_cbData = 0;
        return (FALSE);
    }
    
    return (TRUE);
}

BOOL CAudioBuffer::GenerateBuffer
(
    float   fSeconds
)
{
    HRESULT                     hr;
    LPCWAVEFORMATEX             pWfx;
    
    if (NULL == m_pFormat)
    {
        return (FALSE);
    }

    if (!AllocateBuffer(fSeconds))
    {
        return (FALSE);
    }

    // get a logger
    CComPtr<IBasicLog> pBasicLog;
    hr = g_IShell->GetBasicLog(&pBasicLog);
    
    if (S_OK != hr)
    {
        XLOG(XFAIL,
            eError,
            "FAIL: Error getting basic log object: 0x%08x", hr);
        return (FALSE);
    }
  
    pWfx = m_pFormat->GetAudioFormat();
  
    hr = FillBufferWithSineSignal(
        pBasicLog, XFAIL,
        pWfx,
        1.0, // amplitude
        1000.0, // frequency
        0.0, // initial phase,
        0.0, // dc
        Method_NoDithering,
        static_cast<PBYTE>(m_pData),
        
        // m_cbData is block aligned per CAudioBuffer::AllocateBuffer()
        m_cbData / pWfx->nBlockAlign,
        m_cbData
    );

    if (S_OK != hr)
    {
        XLOG(XFAIL,
            eError,
            "FAIL: Could not fill buffer: 0x%08x", hr);
        return (FALSE);
    }
    
    return (TRUE);
}

//  In some cases (MicArray) we need to generate a buffer where all the
//  channels have different values, GenerateOffsetBuffer uses an different
//  offset into a mono buffer to get sample data...
BOOL CAudioBuffer::GenerateOffsetBuffer
(
    float   fSeconds
)
{
    CAutoBuffer                 Buffer;
    LPBYTE                      pBuffer = NULL;
    UINT                        cbAlloc;
    UINT                        cSamples;
    UINT                        iOffset;
    UINT                        cChannels, iChannel;
    HRESULT                     hr;
    LPCWAVEFORMATEX             pWfxNormal;
    CComPtr<IBasicLog>          pBasicLog;
    
    if (NULL == m_pFormat)
    {
        return (FALSE);
    }

    if (!AllocateBuffer(fSeconds))
    {
        return (FALSE);
    }

    pWfxNormal = m_pFormat->GetAudioFormat();
    
    // create a mono version of the wave format
    CAutoWaveFormatEx wfxMono(pWfxNormal);
    wfxMono->nChannels = 1;
    wfxMono->nBlockAlign = wfxMono->wBitsPerSample / 8 * wfxMono->nChannels;
    wfxMono->nAvgBytesPerSec = wfxMono->nSamplesPerSec * wfxMono->nBlockAlign;
    
    cChannels  = pWfxNormal->nChannels;
    
    // m_cbData is frame-aligned in pWfxNormal frames
    // so (m_cbData / cChannels) is frame-aligned in wfxMono frames
    cbAlloc    = 2 * (m_cbData / cChannels); //  2x size of mono tone...
    
    cSamples   = m_cbData / (pWfxNormal->nBlockAlign);
    iOffset    = (pWfxNormal->wBitsPerSample / 8) * (cSamples / (cChannels + 1));
    
    pBuffer   = (LPBYTE)Buffer.Allocate(cbAlloc);
    
    // get a logger
    hr = g_IShell->GetBasicLog(&pBasicLog);
    
    if (S_OK != hr)
    {
        XLOG(XFAIL,
            eError,
            "FAIL: Error getting basic log object: 0x%08x", hr);
        return (FALSE);
    }

    hr = FillBufferWithSineSignal(
        pBasicLog, XFAIL,
        wfxMono,
        1.0, // amplitude
        1000.0, // frequency
        0.0, // initial phase,
        0.0, // dc
        Method_NoDithering,
        pBuffer,
        cbAlloc / wfxMono->nBlockAlign,        
        cbAlloc
    );

    if (S_OK != hr)
    {
        return (FALSE);
    }
    
    for (iChannel = cChannels; iChannel; --iChannel)
    {
        InterleaveChannel(
            (LPBYTE)m_pData,
            &(pBuffer[iOffset * (iChannel - 1)]),
            iChannel - 1,
            cChannels,
            cSamples,
            pWfxNormal->wBitsPerSample / 8);
    }
    
    return (TRUE);
}

CBufferCollection::CBufferCollection
(
    UINT    cBuffers
)
: m_fIsValid(FALSE), m_ppBuffers(NULL), m_cBuffers(0)
{
    UINT    ii;

    m_ppBuffers = new (CAudioBuffer*[cBuffers]);
    
    if (NULL == m_ppBuffers)
    {
        return;
    }
    
    m_cBuffers = cBuffers;
    
    for (ii = cBuffers; ii; --ii)
    {
        m_ppBuffers[ii - 1] = NULL;
    }
    
    m_fIsValid = TRUE;
}

CBufferCollection::~CBufferCollection()
{
    UINT    ii;
    
    if (NULL == m_ppBuffers)
    {
        return;
    }
    
    for (ii = m_cBuffers; ii; --ii)
    {
        if (NULL != m_ppBuffers[ii - 1])
        {
            delete m_ppBuffers[ii - 1];
            m_ppBuffers[ii - 1] = NULL;
        }
    }
    
    delete []m_ppBuffers;
    m_ppBuffers = NULL;
}

void CBufferCollection::SetBuffer
(
    UINT            iBuffer,
    CAudioBuffer   *pBuffer
)
{
    if (NULL != m_ppBuffers[iBuffer])
    {
        delete m_ppBuffers[iBuffer];
    }
    
    m_ppBuffers[iBuffer] = pBuffer;
}


HRESULT ApplySilenceFlag
(
    APO_CONNECTION_DESCRIPTOR   *pConnection,
    APO_CONNECTION_PROPERTY     *pProperty
)
{
    UNCOMPRESSEDAUDIOFORMAT     format;
    UINT                        cbZero;
    HRESULT                     hr;
    
    if ((NULL == pConnection) ||
        (NULL == pProperty) ||
        (NULL == pConnection->pFormat))
    {
        SLOG(
            eError,
            "SilenceOutputBuffer() received invalid parameters.");

        return (E_POINTER);
    }
    
    hr = pConnection->pFormat->GetUncompressedAudioFormat(&format);
    
    if (S_OK != hr)
    {
        SLOG(
            eError,
            "GetUncompressedAudioFormat in SilenceOutputBuffer "
                "returned error (0x%08lx).",
            hr);

        return (hr);
    }
    
    cbZero  = pProperty->u32ValidFrameCount;
    cbZero *= format.dwBytesPerSampleContainer;
    cbZero *= format.dwSamplesPerFrame;
    
    if (0 != (pProperty->u32BufferFlags & AUDCLNT_BUFFERFLAGS_SILENT))
    {
        //  Silencing output buffer...  Note: This *happens* to work since
        //  the zero value for INT8, INT16, and FLOAT32 happens to have the
        //  same bit representation.  If we start supporting other formats
        //  where this isn't the case, we'll have to special case here...
        ZeroMemory((void*)pProperty->pBuffer, cbZero);
    }
    
    return (S_OK);
}

void Log_ApoRegProperties
(
    PAPO_REG_PROPERTIES     pApoRegProperties
)
{
    UINT    ii;
    WCHAR   szGUID[40];

    SLOG(eInfo1, "      APO_REG_PROPERTIES:");

    SAL_TERMINATE(szGUID);    
    if (0 != StringFromGUID2(pApoRegProperties->clsid, szGUID, sizeof(szGUID)/sizeof(szGUID[0])))
    {
        SLOG(eInfo1, "                   clsid: %ls", szGUID);
    }
    
    SLOG(eInfo1, "                   Flags: 0x%08lx", pApoRegProperties->Flags);
    if (APO_FLAG_DEFAULT == pApoRegProperties->Flags)
    {
        SLOG(eInfo1, "                          APO_FLAG_DEFAULT (0x%08lx)", APO_FLAG_DEFAULT);
    }
    else
    {
        if (0 == pApoRegProperties->Flags)
        {
            SLOG(eInfo1, "                          APO_FLAG_NONE (0x%08lx)", APO_FLAG_NONE);
        }
        
        if (0 != (APO_FLAG_INPLACE & pApoRegProperties->Flags))
        {
            SLOG(eInfo1, "                          APO_FLAG_INPLACE (0x%08lx)", APO_FLAG_INPLACE);
        }
        
        if (0 != (APO_FLAG_SAMPLESPERFRAME_MUST_MATCH & pApoRegProperties->Flags))
        {
            SLOG(eInfo1, "                          APO_FLAG_SAMPLESPERFRAME_MUST_MATCH (0x%08lx)", APO_FLAG_SAMPLESPERFRAME_MUST_MATCH);
        }
        
        if (0 != (APO_FLAG_FRAMESPERSECOND_MUST_MATCH & pApoRegProperties->Flags))
        {
            SLOG(eInfo1, "                          APO_FLAG_FRAMESPERSECOND_MUST_MATCH (0x%08lx)", APO_FLAG_FRAMESPERSECOND_MUST_MATCH);
        }
        
        if (0 != (APO_FLAG_BITSPERSAMPLE_MUST_MATCH & pApoRegProperties->Flags))
        {
            SLOG(eInfo1, "                          APO_FLAG_BITSPERSAMPLE_MUST_MATCH (0x%08lx)", APO_FLAG_BITSPERSAMPLE_MUST_MATCH);
        }
    }
    
    SLOG(eInfo1, "          szFriendlyName: [%ls]", pApoRegProperties->szFriendlyName);
    SLOG(eInfo1, "         szCopyrightInfo: [%ls]", pApoRegProperties->szCopyrightInfo);
    SLOG(eInfo1, "                 Version: %u.%u", pApoRegProperties->u32MajorVersion, pApoRegProperties->u32MinorVersion);
    SLOG(eInfo1, "  u32MinInputConnections: %u", pApoRegProperties->u32MinInputConnections);
    SLOG(eInfo1, "  u32MaxInputConnections: %u", pApoRegProperties->u32MaxInputConnections);
    SLOG(eInfo1, " u32MinOutputConnections: %u", pApoRegProperties->u32MinOutputConnections);
    SLOG(eInfo1, " u32MaxOutputConnections: %u", pApoRegProperties->u32MaxOutputConnections);
    SLOG(eInfo1, "         u32MaxInstances: %u", pApoRegProperties->u32MaxInstances);
    SLOG(eInfo1, "     u32NumAPOInterfaces: %u", pApoRegProperties->u32NumAPOInterfaces);
  
    for (ii = 0; ii < pApoRegProperties->u32NumAPOInterfaces; ++ii)
    {
        if (0 != StringFromGUID2(pApoRegProperties->iidAPOInterfaceList[ii], szGUID, sizeof(szGUID)/sizeof(szGUID[0])))
        {
            SLOG(eInfo1, "       InterfacesID[%03d]: %ls", ii, szGUID);
        }
    }  
}

UINT GetEndpointFriendlyNameW
(
    IMMDevice                      *pIEndpoint,
    __out_ecount(cchName) LPWSTR    pszName,
    UINT                            cchName
)
{
    CComPtr<IPropertyStore>         pIPropertyStoreDevice = NULL;
    UINT                            cchCopied, ii;
    HRESULT                         hr;
    PROPVARIANT                     pv;

    //  if we encounter error, preset.
    pszName[0] = 0;
    cchCopied  = 0;

    if (S_OK != (hr = pIEndpoint->OpenPropertyStore(STGM_READ, &pIPropertyStoreDevice)))
    {
        SLOG(eError, "IMMDevice::OpenPropertyStore returned error (0x%08lx).", hr);
        return (0);
    }

    cchName--;  //  Accounting for terminator.
    PropVariantInit(&pv);
    if (S_OK == (hr = pIPropertyStoreDevice->GetValue(PKEY_Device_FriendlyName, &pv)))
    {
        if (VT_LPWSTR == V_VT(&pv))
        {
            cchCopied = lstrlenW(pv.pwszVal);
            cchCopied = min(cchCopied, cchName);
            
            for (ii = cchCopied, pszName[ii] = 0; ii; --ii)
            {
                pszName[ii - 1] = pv.pwszVal[ii - 1];
            }
        }
    }
    PropVariantClear(&pv);

    return cchCopied;
}

UINT GetEndpointFriendlyNameA
(
    IMMDevice                      *pIEndpoint,
    __out_ecount(cchName) LPSTR     pszName,
    UINT                            cchName
)
{
    CComPtr<IPropertyStore>         pIPropertyStoreDevice = NULL;
    UINT                            cchCopied, ii;
    HRESULT                         hr;
    PROPVARIANT                     pv;

    //  if we encounter error, preset.
    pszName[0] = 0;
    cchCopied  = 0;

    if (S_OK != (hr = pIEndpoint->OpenPropertyStore(STGM_READ, &pIPropertyStoreDevice)))
    {
        SLOG(eError, "IMMDevice::OpenPropertyStore returned error (0x%08lx).", hr);
        return (0);
    }

    cchName--;  //  Accounting for terminator.
    PropVariantInit(&pv);
    if (S_OK == (hr = pIPropertyStoreDevice->GetValue(PKEY_Device_FriendlyName, &pv)))
    {
        if (VT_LPWSTR == V_VT(&pv))
        {
            cchCopied = lstrlenW(pv.pwszVal);
            cchCopied = min(cchCopied, cchName);
            
            for (ii = cchCopied, pszName[ii] = 0; ii; --ii)
            {
                pszName[ii - 1] = (CHAR)(pv.pwszVal[ii - 1]);
            }
        }
    }
    PropVariantClear(&pv);

    return cchCopied;
}

LPSTR GetFormatTagString
(
    WORD    wFormatTag
)
{
    switch (wFormatTag)
    {
        //  Most common...
        case WAVE_FORMAT_PCM:
            return ((LPSTR)(&"WAVE_FORMAT_PCM"));
            
        case WAVE_FORMAT_EXTENSIBLE:
            return ((LPSTR)(&"WAVE_FORMAT_EXTENSIBLE"));
            
        case WAVE_FORMAT_IEEE_FLOAT:
            return ((LPSTR)(&"WAVE_FORMAT_IEEE_FLOAT"));
            
        //  Everything else...    
        case WAVE_FORMAT_UNKNOWN:
            return ((LPSTR)(&"WAVE_FORMAT_UNKNOWN"));
            
        case WAVE_FORMAT_ADPCM:
            return ((LPSTR)(&"WAVE_FORMAT_ADPCM"));
            
        case WAVE_FORMAT_VSELP:
            return ((LPSTR)(&"WAVE_FORMAT_VSELP"));
            
        case WAVE_FORMAT_IBM_CVSD:
            return ((LPSTR)(&"WAVE_FORMAT_IBM_CVSD"));
            
        case WAVE_FORMAT_ALAW:
            return ((LPSTR)(&"WAVE_FORMAT_ALAW"));
            
        case WAVE_FORMAT_MULAW:
            return ((LPSTR)(&"WAVE_FORMAT_MULAW"));
            
        case WAVE_FORMAT_DTS:
            return ((LPSTR)(&"WAVE_FORMAT_DTS"));
            
        case WAVE_FORMAT_DRM:
            return ((LPSTR)(&"WAVE_FORMAT_DRM"));
            
        case WAVE_FORMAT_WMAVOICE9:
            return ((LPSTR)(&"WAVE_FORMAT_WMAVOICE9"));
            
        case WAVE_FORMAT_WMAVOICE10:
            return ((LPSTR)(&"WAVE_FORMAT_WMAVOICE10"));
            
        case WAVE_FORMAT_OKI_ADPCM:
            return ((LPSTR)(&"WAVE_FORMAT_OKI_ADPCM"));
            
//        case WAVE_FORMAT_DVI_ADPCM:
//            return ((LPSTR)(&"WAVE_FORMAT_DVI_ADPCM"));
            
        case WAVE_FORMAT_IMA_ADPCM:
            return ((LPSTR)(&"WAVE_FORMAT_IMA_ADPCM"));
            
        case WAVE_FORMAT_MEDIASPACE_ADPCM:
            return ((LPSTR)(&"WAVE_FORMAT_MEDIASPACE_ADPCM"));
            
        case WAVE_FORMAT_SIERRA_ADPCM:
            return ((LPSTR)(&"WAVE_FORMAT_SIERRA_ADPCM"));
            
        case WAVE_FORMAT_G723_ADPCM:
            return ((LPSTR)(&"WAVE_FORMAT_G723_ADPCM"));
            
        case WAVE_FORMAT_DIGISTD:
            return ((LPSTR)(&"WAVE_FORMAT_DIGISTD"));
            
        case WAVE_FORMAT_DIGIFIX:
            return ((LPSTR)(&"WAVE_FORMAT_DIGIFIX"));
            
        case WAVE_FORMAT_DIALOGIC_OKI_ADPCM:
            return ((LPSTR)(&"WAVE_FORMAT_DIALOGIC_OKI_ADPCM"));
            
        case WAVE_FORMAT_MEDIAVISION_ADPCM:
            return ((LPSTR)(&"WAVE_FORMAT_MEDIAVISION_ADPCM"));
            
        case WAVE_FORMAT_CU_CODEC:
            return ((LPSTR)(&"WAVE_FORMAT_CU_CODEC"));
            
        case WAVE_FORMAT_YAMAHA_ADPCM:
            return ((LPSTR)(&"WAVE_FORMAT_YAMAHA_ADPCM"));
            
        case WAVE_FORMAT_SONARC:
            return ((LPSTR)(&"WAVE_FORMAT_SONARC"));
            
        case WAVE_FORMAT_DSPGROUP_TRUESPEECH:
            return ((LPSTR)(&"WAVE_FORMAT_DSPGROUP_TRUESPEECH"));
            
        case WAVE_FORMAT_ECHOSC1:
            return ((LPSTR)(&"WAVE_FORMAT_ECHOSC1"));
            
        case WAVE_FORMAT_AUDIOFILE_AF36:
            return ((LPSTR)(&"WAVE_FORMAT_AUDIOFILE_AF36"));
            
        case WAVE_FORMAT_APTX:
            return ((LPSTR)(&"WAVE_FORMAT_APTX"));
            
        case WAVE_FORMAT_AUDIOFILE_AF10:
            return ((LPSTR)(&"WAVE_FORMAT_AUDIOFILE_AF10"));
            
        case WAVE_FORMAT_PROSODY_1612:
            return ((LPSTR)(&"WAVE_FORMAT_PROSODY_1612"));
            
        case WAVE_FORMAT_LRC:
            return ((LPSTR)(&"WAVE_FORMAT_LRC"));
            
        case WAVE_FORMAT_DOLBY_AC2:
            return ((LPSTR)(&"WAVE_FORMAT_DOLBY_AC2"));
            
        case WAVE_FORMAT_GSM610:
            return ((LPSTR)(&"WAVE_FORMAT_GSM610"));
            
        case WAVE_FORMAT_MSNAUDIO:
            return ((LPSTR)(&"WAVE_FORMAT_MSNAUDIO"));
            
        case WAVE_FORMAT_ANTEX_ADPCME:
            return ((LPSTR)(&"WAVE_FORMAT_ANTEX_ADPCME"));
            
        case WAVE_FORMAT_CONTROL_RES_VQLPC:
            return ((LPSTR)(&"WAVE_FORMAT_CONTROL_RES_VQLPC"));
            
        case WAVE_FORMAT_DIGIREAL:
            return ((LPSTR)(&"WAVE_FORMAT_DIGIREAL"));
            
        case WAVE_FORMAT_DIGIADPCM:
            return ((LPSTR)(&"WAVE_FORMAT_DIGIADPCM"));
            
        case WAVE_FORMAT_CONTROL_RES_CR10:
            return ((LPSTR)(&"WAVE_FORMAT_CONTROL_RES_CR10"));
            
        case WAVE_FORMAT_NMS_VBXADPCM:
            return ((LPSTR)(&"WAVE_FORMAT_NMS_VBXADPCM"));
            
        case WAVE_FORMAT_CS_IMAADPCM:
            return ((LPSTR)(&"WAVE_FORMAT_CS_IMAADPCM"));
            
        case WAVE_FORMAT_ECHOSC3:
            return ((LPSTR)(&"WAVE_FORMAT_ECHOSC3"));
            
        case WAVE_FORMAT_ROCKWELL_ADPCM:
            return ((LPSTR)(&"WAVE_FORMAT_ROCKWELL_ADPCM"));
            
        case WAVE_FORMAT_ROCKWELL_DIGITALK:
            return ((LPSTR)(&"WAVE_FORMAT_ROCKWELL_DIGITALK"));
            
        case WAVE_FORMAT_XEBEC:
            return ((LPSTR)(&"WAVE_FORMAT_XEBEC"));
            
        case WAVE_FORMAT_G721_ADPCM:
            return ((LPSTR)(&"WAVE_FORMAT_G721_ADPCM"));
            
        case WAVE_FORMAT_G728_CELP:
            return ((LPSTR)(&"WAVE_FORMAT_G728_CELP"));
            
        case WAVE_FORMAT_MSG723:
            return ((LPSTR)(&"WAVE_FORMAT_MSG723"));
            
        case WAVE_FORMAT_MPEG:
            return ((LPSTR)(&"WAVE_FORMAT_MPEG"));
            
        case WAVE_FORMAT_RT24:
            return ((LPSTR)(&"WAVE_FORMAT_RT24"));
            
        case WAVE_FORMAT_PAC:
            return ((LPSTR)(&"WAVE_FORMAT_PAC"));
            
        case WAVE_FORMAT_MPEGLAYER3:
            return ((LPSTR)(&"WAVE_FORMAT_MPEGLAYER3"));
            
        case WAVE_FORMAT_LUCENT_G723:
            return ((LPSTR)(&"WAVE_FORMAT_LUCENT_G723"));
            
        case WAVE_FORMAT_CIRRUS:
            return ((LPSTR)(&"WAVE_FORMAT_CIRRUS"));
            
        case WAVE_FORMAT_ESPCM:
            return ((LPSTR)(&"WAVE_FORMAT_ESPCM"));
            
        case WAVE_FORMAT_VOXWARE:
            return ((LPSTR)(&"WAVE_FORMAT_VOXWARE"));
            
        case WAVE_FORMAT_CANOPUS_ATRAC:
            return ((LPSTR)(&"WAVE_FORMAT_CANOPUS_ATRAC"));
            
        case WAVE_FORMAT_G726_ADPCM:
            return ((LPSTR)(&"WAVE_FORMAT_G726_ADPCM"));
            
        case WAVE_FORMAT_G722_ADPCM:
            return ((LPSTR)(&"WAVE_FORMAT_G722_ADPCM"));
            
        case WAVE_FORMAT_DSAT_DISPLAY:
            return ((LPSTR)(&"WAVE_FORMAT_DSAT_DISPLAY"));
            
        case WAVE_FORMAT_VOXWARE_BYTE_ALIGNED:
            return ((LPSTR)(&"WAVE_FORMAT_VOXWARE_BYTE_ALIGNED"));
            
        case WAVE_FORMAT_VOXWARE_AC8:
            return ((LPSTR)(&"WAVE_FORMAT_VOXWARE_AC8"));
            
        case WAVE_FORMAT_VOXWARE_AC10:
            return ((LPSTR)(&"WAVE_FORMAT_VOXWARE_AC10"));
            
        case WAVE_FORMAT_VOXWARE_AC16:
            return ((LPSTR)(&"WAVE_FORMAT_VOXWARE_AC16"));
            
        case WAVE_FORMAT_VOXWARE_AC20:
            return ((LPSTR)(&"WAVE_FORMAT_VOXWARE_AC20"));
            
        case WAVE_FORMAT_VOXWARE_RT24:
            return ((LPSTR)(&"WAVE_FORMAT_VOXWARE_RT24"));
            
        case WAVE_FORMAT_VOXWARE_RT29:
            return ((LPSTR)(&"WAVE_FORMAT_VOXWARE_RT29"));
            
        case WAVE_FORMAT_VOXWARE_RT29HW:
            return ((LPSTR)(&"WAVE_FORMAT_VOXWARE_RT29HW"));
            
        case WAVE_FORMAT_VOXWARE_VR12:
            return ((LPSTR)(&"WAVE_FORMAT_VOXWARE_VR12"));
            
        case WAVE_FORMAT_VOXWARE_VR18:
            return ((LPSTR)(&"WAVE_FORMAT_VOXWARE_VR18"));
            
        case WAVE_FORMAT_VOXWARE_TQ40:
            return ((LPSTR)(&"WAVE_FORMAT_VOXWARE_TQ40"));
            
        case WAVE_FORMAT_SOFTSOUND:
            return ((LPSTR)(&"WAVE_FORMAT_SOFTSOUND"));
            
        case WAVE_FORMAT_VOXWARE_TQ60:
            return ((LPSTR)(&"WAVE_FORMAT_VOXWARE_TQ60"));
            
        case WAVE_FORMAT_MSRT24:
            return ((LPSTR)(&"WAVE_FORMAT_MSRT24"));
            
        case WAVE_FORMAT_G729A:
            return ((LPSTR)(&"WAVE_FORMAT_G729A"));
            
        case WAVE_FORMAT_MVI_MVI2:
            return ((LPSTR)(&"WAVE_FORMAT_MVI_MVI2"));
            
        case WAVE_FORMAT_DF_G726:
            return ((LPSTR)(&"WAVE_FORMAT_DF_G726"));
            
        case WAVE_FORMAT_DF_GSM610:
            return ((LPSTR)(&"WAVE_FORMAT_DF_GSM610"));
            
        case WAVE_FORMAT_ISIAUDIO:
            return ((LPSTR)(&"WAVE_FORMAT_ISIAUDIO"));
            
        case WAVE_FORMAT_ONLIVE:
            return ((LPSTR)(&"WAVE_FORMAT_ONLIVE"));
            
        case WAVE_FORMAT_SBC24:
            return ((LPSTR)(&"WAVE_FORMAT_SBC24"));
            
        case WAVE_FORMAT_DOLBY_AC3_SPDIF:
            return ((LPSTR)(&"WAVE_FORMAT_DOLBY_AC3_SPDIF"));
            
        case WAVE_FORMAT_MEDIASONIC_G723:
            return ((LPSTR)(&"WAVE_FORMAT_MEDIASONIC_G723"));
            
        case WAVE_FORMAT_PROSODY_8KBPS:
            return ((LPSTR)(&"WAVE_FORMAT_PROSODY_8KBPS"));
            
        case WAVE_FORMAT_ZYXEL_ADPCM:
            return ((LPSTR)(&"WAVE_FORMAT_ZYXEL_ADPCM"));
            
        case WAVE_FORMAT_PHILIPS_LPCBB:
            return ((LPSTR)(&"WAVE_FORMAT_PHILIPS_LPCBB"));
            
        case WAVE_FORMAT_PACKED:
            return ((LPSTR)(&"WAVE_FORMAT_PACKED"));
            
        case WAVE_FORMAT_MALDEN_PHONYTALK:
            return ((LPSTR)(&"WAVE_FORMAT_MALDEN_PHONYTALK"));
            
        case WAVE_FORMAT_RHETOREX_ADPCM:
            return ((LPSTR)(&"WAVE_FORMAT_RHETOREX_ADPCM"));
            
        case WAVE_FORMAT_IRAT:
            return ((LPSTR)(&"WAVE_FORMAT_IRAT"));
            
        case WAVE_FORMAT_VIVO_G723:
            return ((LPSTR)(&"WAVE_FORMAT_VIVO_G723"));
            
        case WAVE_FORMAT_VIVO_SIREN:
            return ((LPSTR)(&"WAVE_FORMAT_VIVO_SIREN"));
            
        case WAVE_FORMAT_DIGITAL_G723:
            return ((LPSTR)(&"WAVE_FORMAT_DIGITAL_G723"));
            
        case WAVE_FORMAT_SANYO_LD_ADPCM:
            return ((LPSTR)(&"WAVE_FORMAT_SANYO_LD_ADPCM"));
            
        case WAVE_FORMAT_SIPROLAB_ACEPLNET:
            return ((LPSTR)(&"WAVE_FORMAT_SIPROLAB_ACEPLNET"));
            
        case WAVE_FORMAT_SIPROLAB_ACELP4800:
            return ((LPSTR)(&"WAVE_FORMAT_SIPROLAB_ACELP4800"));
            
        case WAVE_FORMAT_SIPROLAB_ACELP8V3:
            return ((LPSTR)(&"WAVE_FORMAT_SIPROLAB_ACELP8V3"));
            
        case WAVE_FORMAT_SIPROLAB_G729:
            return ((LPSTR)(&"WAVE_FORMAT_SIPROLAB_G729"));
            
        case WAVE_FORMAT_SIPROLAB_G729A:
            return ((LPSTR)(&"WAVE_FORMAT_SIPROLAB_G729A"));
            
        case WAVE_FORMAT_SIPROLAB_KELVIN:
            return ((LPSTR)(&"WAVE_FORMAT_SIPROLAB_KELVIN"));
            
        case WAVE_FORMAT_G726ADPCM:
            return ((LPSTR)(&"WAVE_FORMAT_G726ADPCM"));
            
        case WAVE_FORMAT_QUALCOMM_PUREVOICE:
            return ((LPSTR)(&"WAVE_FORMAT_QUALCOMM_PUREVOICE"));
            
        case WAVE_FORMAT_QUALCOMM_HALFRATE:
            return ((LPSTR)(&"WAVE_FORMAT_QUALCOMM_HALFRATE"));
            
        case WAVE_FORMAT_TUBGSM:
            return ((LPSTR)(&"WAVE_FORMAT_TUBGSM"));
            
        case WAVE_FORMAT_MSAUDIO1:
            return ((LPSTR)(&"WAVE_FORMAT_MSAUDIO1"));
            
        case WAVE_FORMAT_WMAUDIO2:
            return ((LPSTR)(&"WAVE_FORMAT_WMAUDIO2"));
            
        case WAVE_FORMAT_WMAUDIO3:
            return ((LPSTR)(&"WAVE_FORMAT_WMAUDIO3"));
            
        case WAVE_FORMAT_WMAUDIO_LOSSLESS:
            return ((LPSTR)(&"WAVE_FORMAT_WMAUDIO_LOSSLESS"));
            
        case WAVE_FORMAT_WMASPDIF:
            return ((LPSTR)(&"WAVE_FORMAT_WMASPDIF"));
            
        case WAVE_FORMAT_UNISYS_NAP_ADPCM:
            return ((LPSTR)(&"WAVE_FORMAT_UNISYS_NAP_ADPCM"));
            
        case WAVE_FORMAT_UNISYS_NAP_ULAW:
            return ((LPSTR)(&"WAVE_FORMAT_UNISYS_NAP_ULAW"));
            
        case WAVE_FORMAT_UNISYS_NAP_ALAW:
            return ((LPSTR)(&"WAVE_FORMAT_UNISYS_NAP_ALAW"));
            
        case WAVE_FORMAT_UNISYS_NAP_16K:
            return ((LPSTR)(&"WAVE_FORMAT_UNISYS_NAP_16K"));
            
        case WAVE_FORMAT_CREATIVE_ADPCM:
            return ((LPSTR)(&"WAVE_FORMAT_CREATIVE_ADPCM"));
            
        case WAVE_FORMAT_CREATIVE_FASTSPEECH8:
            return ((LPSTR)(&"WAVE_FORMAT_CREATIVE_FASTSPEECH8"));
            
        case WAVE_FORMAT_CREATIVE_FASTSPEECH10:
            return ((LPSTR)(&"WAVE_FORMAT_CREATIVE_FASTSPEECH10"));
            
        case WAVE_FORMAT_UHER_ADPCM:
            return ((LPSTR)(&"WAVE_FORMAT_UHER_ADPCM"));
            
        case WAVE_FORMAT_QUARTERDECK:
            return ((LPSTR)(&"WAVE_FORMAT_QUARTERDECK"));
            
        case WAVE_FORMAT_ILINK_VC:
            return ((LPSTR)(&"WAVE_FORMAT_ILINK_VC"));
            
        case WAVE_FORMAT_RAW_SPORT:
            return ((LPSTR)(&"WAVE_FORMAT_RAW_SPORT"));
            
        case WAVE_FORMAT_ESST_AC3:
            return ((LPSTR)(&"WAVE_FORMAT_ESST_AC3"));
            
        case WAVE_FORMAT_GENERIC_PASSTHRU:
            return ((LPSTR)(&"WAVE_FORMAT_GENERIC_PASSTHRU"));
            
        case WAVE_FORMAT_IPI_HSX:
            return ((LPSTR)(&"WAVE_FORMAT_IPI_HSX"));
            
        case WAVE_FORMAT_IPI_RPELP:
            return ((LPSTR)(&"WAVE_FORMAT_IPI_RPELP"));
            
        case WAVE_FORMAT_CS2:
            return ((LPSTR)(&"WAVE_FORMAT_CS2"));
            
        case WAVE_FORMAT_SONY_SCX:
            return ((LPSTR)(&"WAVE_FORMAT_SONY_SCX"));
            
        case WAVE_FORMAT_FM_TOWNS_SND:
            return ((LPSTR)(&"WAVE_FORMAT_FM_TOWNS_SND"));
            
        case WAVE_FORMAT_BTV_DIGITAL:
            return ((LPSTR)(&"WAVE_FORMAT_BTV_DIGITAL"));
            
        case WAVE_FORMAT_QDESIGN_MUSIC:
            return ((LPSTR)(&"WAVE_FORMAT_QDESIGN_MUSIC"));
            
        case WAVE_FORMAT_VME_VMPCM:
            return ((LPSTR)(&"WAVE_FORMAT_VME_VMPCM"));
            
        case WAVE_FORMAT_TPC:
            return ((LPSTR)(&"WAVE_FORMAT_TPC"));
            
        case WAVE_FORMAT_OLIGSM:
            return ((LPSTR)(&"WAVE_FORMAT_OLIGSM"));
            
        case WAVE_FORMAT_OLIADPCM:
            return ((LPSTR)(&"WAVE_FORMAT_OLIADPCM"));
            
        case WAVE_FORMAT_OLICELP:
            return ((LPSTR)(&"WAVE_FORMAT_OLICELP"));
            
        case WAVE_FORMAT_OLISBC:
            return ((LPSTR)(&"WAVE_FORMAT_OLISBC"));
            
        case WAVE_FORMAT_OLIOPR:
            return ((LPSTR)(&"WAVE_FORMAT_OLIOPR"));
            
        case WAVE_FORMAT_LH_CODEC:
            return ((LPSTR)(&"WAVE_FORMAT_LH_CODEC"));
            
        case WAVE_FORMAT_NORRIS:
            return ((LPSTR)(&"WAVE_FORMAT_NORRIS"));
            
        case WAVE_FORMAT_SOUNDSPACE_MUSICOMPRESS:
            return ((LPSTR)(&"WAVE_FORMAT_SOUNDSPACE_MUSICOMPRESS"));
            
        case WAVE_FORMAT_MPEG_ADTS_AAC:
            return ((LPSTR)(&"WAVE_FORMAT_MPEG_ADTS_AAC"));
            
        case WAVE_FORMAT_MPEG_RAW_AAC:
            return ((LPSTR)(&"WAVE_FORMAT_MPEG_RAW_AAC"));
            
        case WAVE_FORMAT_NOKIA_MPEG_ADTS_AAC:
            return ((LPSTR)(&"WAVE_FORMAT_NOKIA_MPEG_ADTS_AAC"));
            
        case WAVE_FORMAT_NOKIA_MPEG_RAW_AAC:
            return ((LPSTR)(&"WAVE_FORMAT_NOKIA_MPEG_RAW_AAC"));
            
        case WAVE_FORMAT_DVM:
            return ((LPSTR)(&"WAVE_FORMAT_DVM"));
            
        default:
            return ((LPSTR)(&"Unknown"));
    }
}

LPSTR GetExtensibleFormatSubType
(
    GUID                        SubType,
    __out_ecount(40) LPSTR      pszName
)
{
    if (IsEqualGUID(SubType, KSDATAFORMAT_SUBTYPE_PCM))
    {
        lstrcpy(pszName, "KSDATAFORMAT_SUBTYPE_PCM");
    }
    else if (IsEqualGUID(SubType, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT))
    {
        lstrcpy(pszName, "KSDATAFORMAT_SUBTYPE_IEEE_FLOAT");
    }
    else
    {
        WCHAR   szGUID[40];
     
        SAL_TERMINATE(szGUID);   
        StringFromGUID2(SubType, szGUID, sizeof(szGUID)/sizeof(szGUID[0]));
        sprintf_s(pszName, sizeof(pszName), "%ls", szGUID);
    }
    
    return (pszName);
}


LPSTR GetFormatString
(
    IAudioMediaType            *pIAMTFormat,
    __out_ecount(256) LPSTR     pszName
)
{
    WAVEFORMATEX const     *pwfx = NULL;
    
    if (NULL == pIAMTFormat)
    {
        lstrcpy(pszName, "No Format");
    }
    else
    {
        pwfx = pIAMTFormat->GetAudioFormat();
    
        if (WAVE_FORMAT_EXTENSIBLE == pwfx->wFormatTag)
        {
            PWAVEFORMATEXTENSIBLE       pwfext = (PWAVEFORMATEXTENSIBLE)pwfx;
            char                        szGUID[256];
            
            sprintf_s(
                pszName,
                sizeof(pszName),
                "[FormatTag:%s (%d):%d Hz: %d channels: %d bits: SubFormat: %s]",
                GetFormatTagString(pwfx->wFormatTag),
                pwfx->wFormatTag,
                pwfx->nSamplesPerSec,
                pwfx->nChannels,
                pwfx->wBitsPerSample,
                GetExtensibleFormatSubType(pwfext->SubFormat, szGUID));
        }
        else
        {
            sprintf_s(
                pszName,
                sizeof(pszName),
                "[FormatTag:%s (%d):%d Hz: %d channels: %d bits]",
                GetFormatTagString(pwfx->wFormatTag),
                pwfx->wFormatTag,
                pwfx->nSamplesPerSec,
                pwfx->nChannels,
                pwfx->wBitsPerSample);
        }
    }
    
    return (pszName);
}


BOOL IsEqualFormat
(
    IAudioMediaType      *pIAMTFormat1,
    IAudioMediaType      *pIAMTFormat2
)
{
    UINT                        cbBuffer1, cbBuffer2;
    WAVEFORMATEX const         *pwfx1, *pwfx2;
    UNCOMPRESSEDAUDIOFORMAT     format1, format2;
    
    pwfx1     = pIAMTFormat1->GetAudioFormat();
    pwfx2     = pIAMTFormat2->GetAudioFormat();
    cbBuffer1 = ((WAVE_FORMAT_PCM == pwfx1->wFormatTag)?sizeof(PCMWAVEFORMAT):(sizeof(WAVEFORMATEX) + pwfx1->cbSize));
    cbBuffer2 = ((WAVE_FORMAT_PCM == pwfx2->wFormatTag)?sizeof(PCMWAVEFORMAT):(sizeof(WAVEFORMATEX) + pwfx2->cbSize));
    
    if (cbBuffer1 != cbBuffer2)
    {
        return (FALSE);
    }
    
    //  Two formats don't compare as WAVEFORMATEX...
    if (0 != memcmp(pwfx1, pwfx2, cbBuffer1))
    {
        return (FALSE);
    }
    
    SecureZeroMemory(&format1, sizeof(format1));
    SecureZeroMemory(&format2, sizeof(format2));
    
    if (S_OK != pIAMTFormat1->GetUncompressedAudioFormat(&format1))
    {
        return (FALSE);
    }
    
    if (S_OK != pIAMTFormat2->GetUncompressedAudioFormat(&format2))
    {
        return (FALSE);
    }
    
    //  Two formats don't compare as WAVEFORMATEX...
    if (0 != memcmp(&format1, &format2, sizeof(UNCOMPRESSEDAUDIOFORMAT)))
    {
        return (FALSE);
    }
    
    return (TRUE);
}

KSDATAFORMAT_WAVEFORMATEX *CreateKSDataFromWFX
(
    WAVEFORMATEX               *pwfx
)
{
    UINT                        cbAlloc;
    KSDATAFORMAT_WAVEFORMATEX  *pKsFormat;
    
    cbAlloc = sizeof(KSDATAFORMAT_WAVEFORMATEX) + pwfx->cbSize;
    
    pKsFormat = (KSDATAFORMAT_WAVEFORMATEX*)(CoTaskMemAlloc(cbAlloc));
    
    if (NULL == pKsFormat)
    {
        return (NULL);
    }
    
    pKsFormat->DataFormat.FormatSize  = cbAlloc;
    pKsFormat->DataFormat.Flags       = 0;
    pKsFormat->DataFormat.Reserved    = 0;
    pKsFormat->DataFormat.MajorFormat = KSDATAFORMAT_TYPE_AUDIO;
    pKsFormat->DataFormat.Specifier   = KSDATAFORMAT_SPECIFIER_WAVEFORMATEX;
    
    CopyMemory(&(pKsFormat->WaveFormatEx), pwfx, sizeof(WAVEFORMATEX) + pwfx->cbSize);
    
    if (WAVE_FORMAT_EXTENSIBLE == pwfx->wFormatTag)
    {
        pKsFormat->DataFormat.SubFormat = ((WAVEFORMATEXTENSIBLE*)(pwfx))->SubFormat;
    }
    else
    {
        INIT_WAVEFORMATEX_GUID(&(pKsFormat->DataFormat.SubFormat), pwfx->wFormatTag);
    }
    
    return (pKsFormat);
}


BOOL VerifyFormatFill
(
    IAudioMediaType      *pAMTFormat
)
{
    UNCOMPRESSEDAUDIOFORMAT   formatCalled, formatPoisoned;
    BOOL                        fCorrect = TRUE;
    
    //  Poisoning buffers with same value.
    FillMemory(&formatCalled, sizeof(formatCalled), 169);
    FillMemory(&formatPoisoned, sizeof(formatPoisoned), 169);
    
    if(S_OK != pAMTFormat->GetUncompressedAudioFormat(&formatCalled))
    {
        //  Can't fail condition if API never returns buffer...
        return (fCorrect);
    }
    
    if (formatCalled.guidFormatType == formatPoisoned.guidFormatType)
    {
        XLOG(
            XFAIL,
            eError,
            " FAIL: IAudioMediaType::GetUncompressedAudioFormat does not set "
                "the guidFormatType field.");
                
        fCorrect = FALSE;
    }
    
    if (formatCalled.dwSamplesPerFrame == formatPoisoned.dwSamplesPerFrame)
    {
        XLOG(
            XFAIL,
            eError,
            " FAIL: IAudioMediaType::GetUncompressedAudioFormat does not set "
                "the dwSamplesPerFrame field.");
        
        fCorrect = FALSE;
    }
    
    if (formatCalled.dwBytesPerSampleContainer == formatPoisoned.dwBytesPerSampleContainer)
    {
        XLOG(
            XFAIL,
            eError,
            " FAIL: IAudioMediaType::GetUncompressedAudioFormat does not set "
                "the dwBytesPerSampleContainer field.");
        
        fCorrect = FALSE;
    }
    
    if (formatCalled.dwValidBitsPerSample == formatPoisoned.dwValidBitsPerSample)
    {
        XLOG(
            XFAIL,
            eError,
            " FAIL: IAudioMediaType::GetUncompressedAudioFormat does not set "
                "the dwValidBitsPerSample field.");
                
        fCorrect = FALSE;
    }
    
    if (formatCalled.fFramesPerSecond == formatPoisoned.fFramesPerSecond)
    {
        XLOG(
            XFAIL,
            eError,
            " FAIL: IAudioMediaType::GetUncompressedAudioFormat does not set "
                "the fFramesPerSecond field.");
            
        fCorrect = FALSE;
    }
    
    if (formatCalled.dwChannelMask == formatPoisoned.dwChannelMask)
    {
        XLOG(
            XFAIL,
            eError,
            " FAIL: IAudioMediaType::GetUncompressedAudioFormat does not set "
                "the dwChannelMask field.");
                
        fCorrect = FALSE;
    }
    
    if (fCorrect)
    {
        SLOG(eInfo1, " IAudioMediaType::GetUncompressedAudioFormat sets all the fields in UNCOMPRESSEDAUDIOFORMAT.");
    }
    
    return (fCorrect);
}

CAutoBuffer::CAutoBuffer()
:m_pData(NULL)
{
    m_cbData = 0;
}

CAutoBuffer::CAutoBuffer
(
    UINT    cbAlloc
)
:m_cbData(0), m_pData(NULL)
{
    Allocate(cbAlloc);
}

CCTMAutoFree::CCTMAutoFree(LPVOID pBuffer)
:m_pBuffer(NULL)
{
    m_pBuffer = pBuffer;
}

CCTMAutoFree::~CCTMAutoFree()
{
    if (NULL != m_pBuffer)
    {
        CoTaskMemFree(m_pBuffer);
    }
    
    m_pBuffer = NULL;
}

LPVOID CAutoBuffer::Allocate
(
    UINT    cbAlloc
)
{
    delete []m_pData;
    
    m_pData = new BYTE[cbAlloc];
    
    if (NULL != m_pData)
    {
        m_cbData = cbAlloc;
    }
    else
    {
        m_cbData = 0;
    }
        
    return (m_pData);
}

CAutoBuffer::~CAutoBuffer()
{
    delete []m_pData;
    m_pData  = NULL;
    m_cbData = 0;
}

CFileStream::CFileStream()
{
    m_hFile = INVALID_HANDLE_VALUE;
}

CFileStream::CFileStream
(
    LPCSTR      pszFileName,
    DWORD       dwAccess,
    DWORD       dwShare,
    DWORD       dwDisposition,
    DWORD       dwFlags
)
: m_hFile(INVALID_HANDLE_VALUE)
{
    OpenFile(pszFileName, dwAccess, dwShare, dwDisposition, dwFlags);
}

HANDLE CFileStream::OpenFile
(
    LPCSTR      pszFileName,
    DWORD       dwAccess,
    DWORD       dwShare,
    DWORD       dwDisposition,
    DWORD       dwFlags
)
{
    return (m_hFile = CreateFileA(pszFileName, dwAccess, dwShare, NULL, dwDisposition, dwFlags, NULL));
}

CFileStream::~CFileStream()
{
    if (INVALID_HANDLE_VALUE != m_hFile)
    {
        CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
    }
}

BOOL FileCompare
(
    LPCSTR          pszFileName1,
    LPCSTR          pszFileName2
)
{
    const UINT      cbBuffer = 4096;
    CFileStream     File1(pszFileName1, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL);
    CFileStream     File2(pszFileName2, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL);
    CAutoBuffer     Buffer1(cbBuffer);
    CAutoBuffer     Buffer2(cbBuffer);
    HANDLE          hFile1, hFile2;
    DWORD           cbRemain, cbRead, cbActualRead;
    LPBYTE          pData1, pData2;
    
    hFile1 = File1.GetFileHandle();
    hFile2 = File2.GetFileHandle();
    
    if ((INVALID_HANDLE_VALUE == hFile1) || (INVALID_HANDLE_VALUE == hFile2))
    {
        return (FALSE);
    }
    
    pData1 = (LPBYTE)Buffer1.GetBuffer();
    pData2 = (LPBYTE)Buffer2.GetBuffer();
    
    if ((NULL == pData1) || (NULL == pData2))
    {
        return (FALSE);
    }
    
    if (INVALID_FILE_SIZE == (cbRemain = GetFileSize(hFile1, NULL)))
    {
        return (FALSE);
    }
    
    if (INVALID_FILE_SIZE == (cbRead = GetFileSize(hFile2, NULL)))
    {
        return (FALSE);
    }
    
    if (cbRemain != cbRead)
    {
        return (FALSE);
    }
    
    while (0 != cbRemain)
    {
        cbRead = min(cbBuffer, cbRemain);
        
        if (!ReadFile(hFile1, pData1, cbRead, &cbActualRead, NULL))
        {
            return (FALSE);
        }
        
        if (cbActualRead != cbRead)
        {
            return (FALSE);
        }
        
        if (!ReadFile(hFile2, pData2, cbRead, &cbActualRead, NULL))
        {
            return (FALSE);
        }
        
        if (cbActualRead != cbRead)
        {
            return (FALSE);
        }
        
        if (0 != memcmp(pData1, pData2, cbRead))
        {
            return (FALSE);
        }
        
        cbRemain = cbRemain - cbRead;
    }
    
    return (TRUE);
}
