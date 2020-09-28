//--------------------------------------------------------------------------
//
//  File: PullMode.cpp
//
//  Copyright (C) Microsoft. All rights reserved.
//
//  Abstract:
//      This is the skeleton for the test application.
//
//  Contents:
//      TC_RenderExclusivePullMode()
//      TC_CaptureExclusivePullMode()
//
//
//--------------------------------------------------------------------------
#include "Common.h"
#include <HRString.h>
#include <audiostreaming.h>
#include <cpropvariant.h>
#include <mmsystem.h>
#include <devtrans.h>

#define IF_FAILED_RETURN(hr, msg, res) \
    if (FAILED(hr)) \
    { \
        XLOG(XFAIL, 1, msg L"(%s)", GetHRString(hr)); \
        return res; \
    } else { /*no-op*/ }

#define TEST_DURATION_IN_MS 10000

//--------------------------------------------------------------------------;
//
//  DWORD TC_RenderExclusivePullMode
//
//  Description:
//      TestCase: Render exclusively in pull mode on the current endpoint
//
//  Arguments:
//      None.
//
//  Return (DWORD):
//      Unused.  Failures propagated through XLOG.
//
//
//--------------------------------------------------------------------------;
DWORD 
TC_RenderExclusivePullMode()
{
    CAudioLogoDevice *pCurrentDevice = NULL;
    if (NULL == (pCurrentDevice = g_pAudioLogoDeviceTestModule->GetDevice()))
    {
        XLOG(XFAIL, eError, L"FAIL: GetDevice encountered error.");
        return (FNS_FAIL);
    }

    CComPtr<IMMDevice> pEndpoint = NULL;
    if (NULL == (pEndpoint = pCurrentDevice->GetEndpoint()))
    {
        XLOG(XFAIL, eError, L"FAIL: GetEndpoint encountered error.");
        return (FNS_FAIL);
    }

    CComPtr<IBasicLog> pLog;
    HRESULT hr = g_IShell->GetBasicLog(&pLog);
    IF_FAILED_RETURN(hr, L"FAIL: GetBasicLog returned error.", FNS_FAIL);
    
    CComPtr<IAudioStreamingRender> pRender;
    hr = CreateAudioStreamingRender(api_WASAPIExclusive, pEndpoint, pLog, &pRender);
    IF_FAILED_RETURN(hr, L"FAIL: create WASAPI exclusive render [CreateAudioStreamingRender]", FNS_FAIL);

    hr = pRender->Initialize(NULL); // use default format
    IF_FAILED_RETURN(hr, L"FAIL: initialize renderer with default format [IAudioStreamingRender::Initialize]", FNS_FAIL);
    
    XLOG(XMSG, 5, L"Playing sine wave...");
    hr = pRender->PlaySineWave(TEST_FREQUENCY, TEST_AMPLITUDE, Method_NoDithering, TEST_DURATION_IN_MS);
    IF_FAILED_RETURN(hr, L"FAIL: play sine wave [IAudioStreamingRender::PlaySineWave]", FNS_FAIL);

    return FNS_PASS;
}

//--------------------------------------------------------------------------;
//
//  DWORD TC_CaptureExclusivePullMode
//
//  Description:
//      TestCase: Capture exclusively in pull mode on the current endpoint
//
//  Arguments:
//      None.
//
//  Return (DWORD):
//      Unused.  Failures propagated through XLOG.
//
//
//--------------------------------------------------------------------------;
DWORD 
TC_CaptureExclusivePullMode()
{
    CAudioLogoDevice *pCurrentDevice = NULL;
    if (NULL == (pCurrentDevice = g_pAudioLogoDeviceTestModule->GetDevice()))
    {
        XLOG(XFAIL, eError, L"FAIL: GetDevice encountered error.");
        return (FNS_FAIL);
    }

    CComPtr<IMMDevice> pEndpoint = NULL;
    if (NULL == (pEndpoint = pCurrentDevice->GetEndpoint()))
    {
        XLOG(XFAIL, eError, L"FAIL: GetEndpoint encountered error.");
        return (FNS_FAIL);
    }

    CComPtr<IBasicLog> pLog;
    HRESULT hr = g_IShell->GetBasicLog(&pLog);
    IF_FAILED_RETURN(hr, L"FAIL: GetBasicLog returned error.", FNS_FAIL);
    
    CComPtr<IAudioStreamingCapture> pCapture;
    hr = CreateAudioStreamingCapture(api_WASAPIExclusive, pEndpoint, pLog, &pCapture);
    IF_FAILED_RETURN(hr, L"FAIL: create WASAPI exclusive render [CreateAudioStreamingCapture]", FNS_FAIL);

    hr = pCapture->Initialize(NULL); // use default format
    IF_FAILED_RETURN(hr, L"FAIL: initialize renderer with default format [IAudioStreamingCapture::Initialize]", FNS_FAIL);

    XLOG(XMSG, 5, L"Capturing samples...");
    hr = pCapture->CaptureWithoutBufferAsync();
    IF_FAILED_RETURN(hr, L"FAIL: capture samples [IAudioStreamingCapture::CaptureWithoutBufferAsync]", FNS_FAIL);
    
    Sleep(TEST_DURATION_IN_MS);

    hr = pCapture->Stop();
    if (FAILED(hr))
    {
        XLOG(XFAIL, 1, L"FAIL: failed to stop capturing [IAudioStreamingCapture::Stop] (hr = %s)", GetHRString(hr));
        
        EndpointFormFactor formfactor;
        hr = DevTrans::GetEndpointFormFactor(&formfactor, pEndpoint);
        if (FAILED(hr)) 
        { 
            XLOG(XWARN, 1, L"WARN: failed to decide endpoint formfactor"); 
            return FNS_PASS;
        }
        
        if (SPDIF == formfactor)
        {
            XLOG(XFAIL, 1, L"The device under test is SPDIF capture." \
                L"Make sure that a functioning SPDIF render device is plugged into this device when testing if you see this failure." \
                L"SPDIF capture devices often require a rendering source to update the stream position.");
        }
    }
    
    return FNS_PASS;
}