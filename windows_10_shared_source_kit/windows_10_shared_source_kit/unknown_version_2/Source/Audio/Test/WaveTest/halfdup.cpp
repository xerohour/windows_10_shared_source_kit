// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
// Module Name:
//
//  tcommon.cpp
//
// Abstract:
//
//  Implementation file for CWaveTst::TestCommon
//




// -------------------------------------------------------------------------------

#include "PreComp.h"
#include "WaveTestTaef.h"
#include "halfdup.h"
#include <tests.h>
#include <dithering.h>
#include <signal-source.h>
#include <sine-signal-source.h>
#include <dvmodule.h>
#include <debug.h>

#include <CPropvariant.h>
#include <avendpointkeys.h>
#include <hrstring.h>

extern USHORT ChannelsFromMask(DWORD);

using namespace WEX::Logging;
using namespace WEX::TestExecution;

WORD BitDepthFromMask(DWORD dw)
{
    switch(dw & FMTM_BITDEPTH)
    {
        case FMT_8BIT    : return 8;
        case FMT_16BIT   : return 16;
        case FMT_20BIT   : return 20;
        case FMT_24BIT   : return 24;
        case FMT_32BIT   : return 32;
    }

    return 0;
}

ULONG SampleRateFromMask(DWORD dw)
{
    switch (dw & FMTM_SAMPLERATE)
    {
        case FMT_8000HZ  : return  8000;
        case FMT_11025HZ : return 11025;
        case FMT_16000HZ : return 16000;
        case FMT_22050HZ : return 22050;
        case FMT_32000HZ : return 32000;
        case FMT_44100HZ : return 44100;
        case FMT_48000HZ : return 48000;
    }
    return 0;
}

ULONG SpeakerMaskFromMask(DWORD dw)
{
    switch (dw & FMTM_SPEAKCONFIG)
    {
        case FMT_MONO    :  return KSAUDIO_SPEAKER_MONO;
        case FMT_STEREO  :  return KSAUDIO_SPEAKER_STEREO;
        case FMT_QUAD    :  return KSAUDIO_SPEAKER_QUAD;
        case FMT_5_1     :  return KSAUDIO_SPEAKER_5POINT1;
    }
    return 0;
}

// ------------------------------------------------------------------------------
// GenerateFormatEntries
// ------------------------------------------------------------------------------
BOOL
CHalfApp::GenerateFormatEntries
(
    void
)
{
    BOOL                    fRes = TRUE;
    DWORD                   dwSupportMask = m_pPin->m_dwWaveFormatMask;
    DWORD                   dwBD, dwSR, dwSC;
    UINT_PTR                nFormat = 0;
    PWAVEFORMATEXTENSIBLE   pwfx = NULL;

    USHORT cBD = BitsSetInDWORD(dwSupportMask & FMTM_BITDEPTH);     // # of bitdepths supported
    USHORT cSR = BitsSetInDWORD(dwSupportMask & FMTM_SAMPLERATE);   // # of samplerates supported
    USHORT cSC = BitsSetInDWORD(dwSupportMask & FMTM_SPEAKCONFIG);  // # of speaker configs supported

    m_cFmts = cBD * cSR * cSC;

    // Allocate memory for two arrays one to hold the WAVEFORMATEX formats
    // and one to hold similar WAVEFORMATEXTENSIBLE formats. Even though some
    // formats are not valid for WAVEFORMATEX, the driver should reject them
    delete[] m_rgFmts;
    m_rgFmts = new FORMAT_ENTRY[m_cFmts];
    if (NULL == m_rgFmts) { goto bail; }
    SecureZeroMemory(m_rgFmts, sizeof(FORMAT_ENTRY)*m_cFmts);

    for (dwBD = FMT_8BIT; dwBD <= FMT_32BIT; dwBD *= 2)
    {
        if ((dwSupportMask & dwBD) == 0) { continue; }

        for (dwSR = FMT_8000HZ; dwSR <= FMT_48000HZ; dwSR *= 2)
        {
            if ((dwSupportMask & dwSR) == 0) { continue; }

            for (dwSC = FMT_MONO; dwSC <= FMT_7_1; dwSC *= 2)
            {
                if ((dwSupportMask & dwSC) == 0) { continue; }
                
                ASSERT(nFormat < m_cFmts);

                // Fill in WAVEFORMATEXTENSIBLE format
                pwfx = &m_rgFmts[nFormat].wfxFormat;

                pwfx->Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
                pwfx->Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
                pwfx->Format.wBitsPerSample = (WORD)BitDepthFromMask(dwBD);
                pwfx->Samples.wValidBitsPerSample = (pwfx->Format.wBitsPerSample > 16) ? 32 : pwfx->Format.wBitsPerSample;
                pwfx->dwChannelMask = SpeakerMaskFromMask(dwSC);
                pwfx->Format.nChannels = BitsSetInDWORD(pwfx->dwChannelMask);
                pwfx->Format.nSamplesPerSec = SampleRateFromMask(dwSR);
                pwfx->Format.nBlockAlign = pwfx->Format.wBitsPerSample * pwfx->Format.nChannels / 8;
                pwfx->Format.nAvgBytesPerSec = pwfx->Format.nBlockAlign * pwfx->Format.nSamplesPerSec;
                pwfx->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
                
                nFormat++;
            }
        }
    }

    ASSERT(nFormat == m_cFmts);
    
    return fRes;

bail:
    m_cFmts = 0;
    return FALSE;
}

bool 
CHalfApp::IsPropertySupported(
    IN ULONG nProperty, 
    IN DWORD dwSupport
)
{
    bool fRes = false;

    if (IsWaveRTPin())
    {
        DWORD dwSupport2;
        if (m_pPin->PropertyBasicSupport(KSPROPSETID_RtAudio, nProperty, &dwSupport2))
        {
            fRes = ((dwSupport2 & dwSupport) != 0);
        }
        else
        {
            // Ensure correct error codes
            VERIFY_IS_TRUE((GetLastError() == ERROR_SET_NOT_FOUND) || (GetLastError() == ERROR_NOT_FOUND));
        }
    }

    return fRes;
}

// ----------------------------------------------------------------------------------
// PreparePin
// RTDRIVER flag added for testing of RTDRIVER. This is temporary. 
// Instead of having flag here we can have memeber of CHalfClss which tell us that this filter represent RTDRIVER Filter.
// ----------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------
// PreparePin
// ----------------------------------------------------------------------------------
BOOL
CHalfApp::PreparePin(
            WAVEFORMATEX *pWfxIn,
            BOOL fLooped,
            BOOL /*RTPort*/,
            OPTIONAL ULONG ulBufferTime,
            BOOL bRawSupport)
{
    CComPtr<IBasicLog>  pBasicLog;
    HRESULT     hr = S_OK;
    
    // make sure we have enough parts to make some noise
    if (NULL == m_pPin)
    {
        SLOG(eWarn1, "PreparePin: Pin passed in is NULL");
        goto cleanup;
    }

    if (!InstantiatePinWfx (m_pPin, pWfxIn, fLooped,bRawSupport))
    {
        SLOG(eWarn1, "PreparePin: Unable to instantiate pin");
        goto cleanup;
    }

    // Does the pin support wave rt pcakets
    m_fIsWaveRTPinPacketSupport = (m_flow == render) ? IsPropertySupported(KSPROPERTY_RTAUDIO_SETWRITEPACKET, KSPROPERTY_TYPE_SET) :
                                                       IsPropertySupported(KSPROPERTY_RTAUDIO_GETREADPACKET, KSPROPERTY_TYPE_GET);

    if(!m_pPin->CreateDataBuffer(ulBufferTime, pWfxIn->nBlockAlign))
    {
        SLOG(eWarn1, "PreparePin: Unable to create a data buffer");
        goto cleanup;
    }

    if (m_flow == render)
    {
        // get a logger
        pBasicLog = g_pBasicLog;
    
        if (S_OK != hr)
        {
            XLOG(XFAIL,
                eError,
                "FAIL: Error getting basic log object: 0x%08x", hr);
            goto cleanup;
        }

        hr = FillBufferWithSineSignal(
            pBasicLog, XFAIL,
            pWfxIn,
            TEST_AMPLITUDE,
            TEST_FREQUENCY,
            0.0, // initial phase,
            0.0, // dc
            Method_NoDithering,
            m_pPin->m_pbStreamData,
            m_pPin->m_cbStreamData / pWfxIn->nBlockAlign,
            m_pPin->m_cbStreamData
        );

        if (S_OK != hr)
        {
            XLOG(XFAIL,
                eError,
                "FAIL: Error filling buffer with sine: 0x%08x", hr);
            goto cleanup;
        }
    }
    
    return TRUE;

cleanup:
    if (NULL != m_pPin)
    {
        m_pPin->SetState(KSSTATE_STOP);
        m_pPin->ClosePin();
    }
    return FALSE;
}
BOOL
CHalfApp::PreparePin(BOOL fLooped, BOOL /*RTPort*/, OPTIONAL ULONG ulBufferTime)
{
    BOOL                fRes = FALSE;
    CComPtr<IBasicLog>  pBasicLog;
    HRESULT             hr;
    
    // make sure we have enough parts to make some noise
    fRes = (m_pPin != NULL);
    if (!fRes)
    {
        SLOG(eWarn1, "Graph not complete");
        goto cleanup;
    }

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    try
    {
        // Create the render pin on the audio adapter filter
        fRes = 
            CreatePin_WAVEFORMATEXTENSIBLE
            (
                m_pPin,
                NULL,
                &m_pFmtCurrent->wfxFormat,
                fLooped,
                m_Mode
            );
        if (!fRes)
            throw("Failed to create Sink pin on Renderer (Wave Port sink pin)");

        // pull the WAVEFORMATEX out of this struct
        WAVEFORMATEX*               pwfx;
        PKSDATAFORMAT_WAVEFORMATEX  pFormatWfx = (PKSDATAFORMAT_WAVEFORMATEX)m_pPin->m_pksDataFormat;

        pwfx = &pFormatWfx->WaveFormatEx;

        // Does the pin support wave rt pcakets
        m_fIsWaveRTPinPacketSupport = (m_flow == render) ? IsPropertySupported(KSPROPERTY_RTAUDIO_SETWRITEPACKET, KSPROPERTY_TYPE_SET) :
                                                           IsPropertySupported(KSPROPERTY_RTAUDIO_GETREADPACKET, KSPROPERTY_TYPE_GET);

        // allocate data
        fRes = m_pPin->CreateDataBuffer(ulBufferTime, pwfx->nBlockAlign, 16);

        if (!fRes) {
            DWORD dwErr = GetLastError();
            SLOG(eError, "Could not create data buffer for pin - GetLastError() returned %d (0x%08x)", dwErr, dwErr);
            throw("Could not create data buffer for pin");
        }

        if (m_flow == render)
        {
            // get a logger
            pBasicLog = g_pBasicLog;

            // make a sine tone
            hr = FillBufferWithSineSignal(
                pBasicLog,
                XFAIL,
                pwfx,
                TEST_AMPLITUDE,
                TEST_FREQUENCY,
                0.0, // initial phase
                0.0, // dc
                Method_NoDithering,
                m_pPin->m_pbStreamData,
                m_pPin->m_cbStreamData / pwfx->nBlockAlign,
                m_pPin->m_cbStreamData
            );
            if (FAILED(hr))
            {
                fRes = FALSE;
                throw("FillBufferWithSineSignal failed");
            }
        }
    }

    // handle bad things
    catch(char* str)
    {
        SLOG(eWarn1, str);
    }

    return fRes;

cleanup:
    //
    // clean up
    //
    if (m_pPin)
    {
        m_pPin->SetState(KSSTATE_STOP);
        m_pPin->ClosePin();
    }

    return fRes;
}

// ----------------------------------------------------------------------------------
// PrepareBuffer
// ----------------------------------------------------------------------------------
BOOL
CHalfApp::PrepareBuffer(BOOL /*RTPort*/, OPTIONAL ULONG ulBufferTime)
{
    BOOL                fRes = FALSE;
    CComPtr<IBasicLog>  pBasicLog;
    HRESULT             hr;
    
    // check to see if the buffer is already prepared
    if (m_pPin->m_cbStreamData || m_pPin->m_pbStreamData) {

        // buffer claims to be allocated... sanity check
        if (m_pPin->m_pbStreamData && m_pPin->m_cbStreamData) {
            // the buffer is allocated and sane... nothing to do
            return TRUE;
        }

        // something's amiss! the buffer is only partially constructed... sanity check fails
        SLOG(eError, "pin buffer inconsistent: m_pbStreamData is 0x%p but m_cbStreamData is %d",
            m_pPin->m_pbStreamData, m_pPin->m_cbStreamData);
        return FALSE;
    }

    try
    {
        // pull the WAVEFORMATEX out of this struct
        WAVEFORMATEX*               pwfx;
        PKSDATAFORMAT_WAVEFORMATEX  pFormatWfx = (PKSDATAFORMAT_WAVEFORMATEX)m_pPin->m_pksDataFormat;

        pwfx = &pFormatWfx->WaveFormatEx;

        // allocate data
        fRes = m_pPin->CreateDataBuffer(ulBufferTime, pwfx->nBlockAlign, 16);

        if (!fRes) {
            DWORD dwErr = GetLastError();
            SLOG(eError, "Could not create data buffer for pin - GetLastError() returned %d (0x%08x)", dwErr, dwErr);
            throw("Could not create data buffer for pin");
        }

        if (m_flow == render)
        {
            // get a logger
            pBasicLog = g_pBasicLog;

            // make a sine tone
            hr = FillBufferWithSineSignal(
                pBasicLog,
                XFAIL,
                pwfx,
                TEST_AMPLITUDE,
                TEST_FREQUENCY,
                0.0, // initial phase
                0.0, // dc
                Method_NoDithering,
                m_pPin->m_pbStreamData,
                m_pPin->m_cbStreamData / pwfx->nBlockAlign,
                m_pPin->m_cbStreamData
            );
            if (FAILED(hr))
            {
                fRes = FALSE;
                throw("FillBufferWithSineSignal failed");
            }
        }
    }
    // handle bad things
    catch(char* str)
    {
        SLOG(eWarn1, str);
    }

    return fRes;

}

bool CHalfApp::IsTestSupportedForDeviceType(BOOL fTestingLoopedDevice) 
{ 
    return ((fTestingLoopedDevice && m_fIsStdLoopedStrmPin) // If the current test is for looped streaming device and the current device supports looped streaming;
           || (!fTestingLoopedDevice && m_fIsStdStrmPin));  // or, if the current test is for standard streaming device and the current device supports standard streaming.
}

bool CHalfApp::IsWaveRTPacketStreamingSupported()
{
    return m_fIsWaveRTPinPacketSupport;
}

bool CHalfApp::IsBTHPin()
{
    HRESULT hr;
    bool bBTHFound = false;
    UINT cEndpoints = 0;
    CComPtr<IMMDeviceCollection> pEndpointCollection;
    CComPtr<IBasicLog> pLog = g_pBasicLog;

    // Find endpoint collection related to current testing pin
    hr = GetCurrentEndpointsUnderTest(&pEndpointCollection);
    if (FAILED(hr))
    {
        return bBTHFound;
    }

    // Get the count of endpoint collection
    hr = pEndpointCollection->GetCount(&cEndpoints);
    if (FAILED(hr))
    {
        return bBTHFound;
    }

    XLOG(XMSG, eInfo1, "There is a total of %d endpoints for the current pin", cEndpoints);
    if (0 == cEndpoints)
    {
        // Didn't find any endpoint. Just exit.
        return bBTHFound;
    }

    for (UINT idxEP = 0; idxEP < cEndpoints; idxEP++)
    {
        CComPtr<IMMDevice> pEndpoint;

        hr = pEndpointCollection->Item(idxEP, &pEndpoint);
        if (FAILED(hr))
        {
            XLOG(XMSG, eInfo1, "Failed to get endpoint at index %d", idxEP);
            continue;
        }

        CComHeapPtr<WCHAR> szId; // use the ID for logging purpose
        hr = pEndpoint->GetId(&szId);
        if (FAILED(hr))
        {
            XLOG(XMSG, eInfo1, "Failed to get endpoint id at index %d", idxEP);
            continue;
        }

        XLOG(XMSG, eInfo2, "Audio endpoint device under test has ID: %ls", S_W(szId));

        CComPtr<IPropertyStore> pPropertyStore;
        hr = pEndpoint->OpenPropertyStore(STGM_READ, &pPropertyStore);
        if (FAILED(hr))
        {
            XLOG(XMSG, eInfo1, "Failed to open property store on endpoint (%ls) to read (IMMDevice::OpenPropertyStore) (hr = %s)", S_W(szId), GetHRString(hr));
            continue;
        }

        CPropVariant varIsBTH;
        hr = pPropertyStore->GetValue(PKEY_Endpoint_IsBluetooth, &varIsBTH);
        if (FAILED(hr))
        {
            XLOG(XMSG, eInfo1, "Failed to get PKEY_Endpoint_IsBluetooth on endpoint (%ls) (IPropertyStore::GetValue) (hr = %s)", S_W(szId), GetHRString(hr));
            continue;
        }

        if (VT_EMPTY == varIsBTH.vt)
        {
            XLOG(XMSG, eInfo1, "PKEY_Endpoint_IsBluetooth not defined on endpoint (%ls).", S_W(szId));
            continue;
        }

        bBTHFound = (varIsBTH.ulVal ? true : false);
        if (true == bBTHFound)
        {
            XLOG(XMSG, eInfo1, "Current device is bluetooth.");
            return bBTHFound;
        }
    }

    return bBTHFound;
}
