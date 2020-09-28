// Copyright(C) Microsoft. All rights reserved.

//
// SessionVolumeStress.cpp
//  
//
// Description:
//
//  Session volume manipulation stress test

#include <stdafx.h>
#include <UMATestStreaming.h>

#include "TestUtilities.h"

#include "SessionVolumeStress.h"

//-------------------------------------------------------------------------
// Method: Tc_SessionVolumeStress
//
// Description:
//  Create a number of audiosessions and manipulate their volumes
//    
// Parameters:
//
// Return values (DWORD):
//     FNS_PASS on success, FNS_FAIL on failure
//
DWORD Tc_SessionVolumeStress()
{
    CVolumeStressor objStressor;
    objStressor.RunStress(7);

    return FNS_PASS;   
}

//-------------------------------------------------------------------------
// Class: VolumeStressor
// Method: VolumeStressor
//
// Description:
//  Default Constructor.

//
// Parameters: N/A
//
// Return values: N/A
//     
CVolumeStressor::CVolumeStressor()
{ 
    m_nStreamsUnderTest = 0;

    m_aspStreamingEngines = NULL;
    m_aspRenderStreamingEngines = NULL;
    m_aspCaptureStreamingEngines = NULL;
    m_aspAudioSessionControls = NULL;

    m_awszSessionIDs = NULL;
        
    return;
}

//-------------------------------------------------------------------------
// Class:  CVolumeStressor
// Method: ~CVolumeStressor()
//
// Description:
//  Class Destructor.
// Parameters:
//  N/A
//
// Return values:
//  N/A
CVolumeStressor::~CVolumeStressor()
{
    Cleanup();
}

//-------------------------------------------------------------------------
// Class:  CVolumeStressor
// Method: ~CVolumeStressor()
//
// Description:
//  Perform misc cleanup.
// Parameters:
//  N/A
//
// Return values:
//  N/A
void CVolumeStressor::Cleanup()
{
    SAFE_DELETE_ARRAY(m_aspStreamingEngines);
    SAFE_DELETE_ARRAY(m_aspRenderStreamingEngines);
    SAFE_DELETE_ARRAY(m_aspCaptureStreamingEngines);
    SAFE_DELETE_ARRAY(m_aspAudioSessionControls);

    if(NULL != m_awszSessionIDs)
    {
        for(UINT32 i=0;i<m_nStreamsUnderTest;i++)
        {
            SAFE_COTASKMEMFREE(m_awszSessionIDs[i]);
        }

        SAFE_DELETE_ARRAY(m_awszSessionIDs);
    }
   
    return;
}

//-------------------------------------------------------------------------
// Class:  CVolumeStressor
// Method: RunStress
//
// Description:
//  Launch a large number of client sessions, obtain AudioSessionControl
//  interfaces for the associated audiosessions and manipulate the volumes
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
VOID CVolumeStressor::RunStress(UINT nClients)
{
    HRESULT hr = S_OK;

    assert(0 == m_nStreamsUnderTest);

    m_nStreamsUnderTest = nClients;

    // Create nClients audio streams and start streaming
    hr = SpawnAudioSessions();
    if(FAILED(hr))
    {
        goto END;
    }

    Sleep(3000);

    // tinker with the session volumes
    RampSessionVolumes();
    
END:
    // Stop streaming and clean everything up
    Cleanup();
    return;
}

//-------------------------------------------------------------------------
// Class:  CVolumeStressor
// Method: SpawnAudioSessions
//
// Description:
//  Spawn a number of audio streams
// Parameters:
//
// Return values (HRESULT):
//     S_OK on success, the appropriate HRESULT on failure
HRESULT CVolumeStressor::SpawnAudioSessions()
{
    HRESULT hr = S_OK;

    // make sure we're not leaking memory anywhere
    assert(NULL == m_aspStreamingEngines);
    assert(NULL == m_aspRenderStreamingEngines);
    assert(NULL == m_aspCaptureStreamingEngines);
    assert(NULL == m_aspAudioSessionControls);
    assert(NULL == m_awszSessionIDs);
    
    if(m_nStreamsUnderTest == 0)
    {
        return S_FALSE;
    }

    if(NULL == (m_aspStreamingEngines = new CComPtr<ITestStreamingEngine>[m_nStreamsUnderTest]))
    {
        hr = E_OUTOFMEMORY;
        goto END;
    }
    
    if(NULL == (m_aspRenderStreamingEngines = new CComPtr<IRenderTestStreamingEngine>[m_nStreamsUnderTest]))
    {
        hr = E_OUTOFMEMORY;
        goto END;
    }

    if(NULL == (m_aspCaptureStreamingEngines = new CComPtr<ICaptureTestStreamingEngine>[m_nStreamsUnderTest]))
    {
        hr = E_OUTOFMEMORY;
        goto END;
    }
    
    if(NULL == (m_aspAudioSessionControls = new CComPtr<IAudioSessionControl>[m_nStreamsUnderTest]))
    {
        hr = E_OUTOFMEMORY;
        goto END;
    }
    
    if(NULL == (m_awszSessionIDs = new LPWSTR[m_nStreamsUnderTest]))
    {
        hr = E_OUTOFMEMORY;
        goto END;
    }

    // initialize Session ID string pointers to NULL
    for(UINT32 i=0; i<m_nStreamsUnderTest; i++)
    {
        m_awszSessionIDs[i] = NULL;
    }

    // create the streams
    for(UINT32 i=0; i<m_nStreamsUnderTest;i++)
    {
        hr = CreateStreamSessionInProcess(&(m_aspStreamingEngines[i]),
                                          &(m_aspRenderStreamingEngines[i]),
                                          &(m_aspCaptureStreamingEngines[i]),
                                          (m_awszSessionIDs[i]));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Unable to create streaming engine in the current process! (hr: 0x%8x)", hr);
            goto END;
        }

        // store the IAudioSessionControl pointer for the newly created engine
        hr = m_aspStreamingEngines[i]->GetSessionControl(&(m_aspAudioSessionControls[i]));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl for new session (hr: 0x%8x)", hr);
            goto END;
        }
    }
    
    // render some tones
    for(UINT32 i=0;i<m_nStreamsUnderTest;i++)
    {
        if((DEVICE_TYPE_RENDER == GetDeviceTypeUnderTest())||(DEVICE_TYPE_RENDEROFFLOAD == GetDeviceTypeUnderTest()))
        {
            if(m_aspRenderStreamingEngines[i] != NULL)
            {
                hr = m_aspRenderStreamingEngines[i]->RenderSineTone((i+1)*100, // unique frequency
                                                                    0.01,
                                                                    0,
                                                                    DITHER_TYPE_TPD,
                                                                    NULL);
                if(FAILED(hr))
                {
                    XLOG(XFAIL, eError, "FAIL: Unable to stream sine tone (hr: 0x%8x)", hr);
                    goto END;
                }
            }
        }
        else if(DEVICE_TYPE_CAPTURE == GetDeviceTypeUnderTest())
        {
            if(m_aspCaptureStreamingEngines[i] != NULL)
            {
                hr = m_aspCaptureStreamingEngines[i]->NullCapture(0, NULL);
                if(FAILED(hr))
                {
                    XLOG(XFAIL, eError, "FAIL: Unable to initiate capture (hr: 0x%8x)", hr);
                    goto END;
                }
            }
        }
    }
    
END:
    return hr;
}

//-------------------------------------------------------------------------
// Class:  CVolumeStressor
// Method: RampSessionVolumes
//
// Description:
//  Rapidly modify the volume for all IAudioSessionControl interfaces in
//  m_aspAudioSessionControls
//
// Parameters:N/A
//
// Return values (HRESULT):
//     S_OK on success, the appropriate HRESULT on failure
//
HRESULT CVolumeStressor::RampSessionVolumes()
{
    HRESULT hr = S_OK;

    if((NULL == m_aspAudioSessionControls)||(0 == m_nStreamsUnderTest))
    {
        hr = E_FAIL;
        goto END;
    }

    for (UINT32 i=0;i<m_nStreamsUnderTest;i++)
    {        
        hr = RampSingleSession(m_aspAudioSessionControls[i]);
        if(FAILED(hr))
        {
            goto END;
        }

        Sleep(1000);
    }   

END:
    return hr;
}

//-------------------------------------------------------------------------
// Class:  CVolumeStressor
// Method: RampSingleSession
//
// Description:
//  Rapidly modify the volume and mute settings for a single AudioSession
// Parameters:
//      pSession - the IAudioSessionControl interface to be manipulated
//              
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
HRESULT CVolumeStressor::RampSingleSession(IAudioSessionControl* pSession)
{
    HRESULT hr = S_OK;
    CComPtr<ISimpleAudioVolume> spVolume = NULL;

    if(NULL == pSession)
    {
        hr = E_POINTER;
        goto END;
    }
    
    METHODCHECKRESULT("IAudioSession::QueryInterface",
                      hr,
                      pSession->QueryInterface(__uuidof(ISimpleAudioVolume), 
                                               (LPVOID*)&spVolume));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume interface");
        goto END;
    }

    // mute session
    METHODCHECKRESULT("ISimpleAudioVolume::SetMute",
                      hr,
                      spVolume->SetMute(TRUE, NULL));
    // ignore failure

    Sleep(10);

    // unmute session
    METHODCHECKRESULT("ISimpleAudioVolume::SetMute",
                      hr,
                      spVolume->SetMute(FALSE, NULL));
    // ignore failure


    // ramp session volume
    for(float fVol=0.0f;fVol<=1.0f;fVol+=.01f)
    {
        METHODCHECKRESULT("ISimpleAudioVolume::SetMasterVolume",
                          hr,
                          spVolume->SetMasterVolume(fVol, NULL));
        // ignore failure
        Sleep(10);                         
    }
    for(float fVol=1.0f;fVol>0.5f;fVol-=.01f)
    {
        METHODCHECKRESULT("ISimpleAudioVolume::SetMasterVolume",
                          hr,
                          spVolume->SetMasterVolume(fVol, NULL));
        // ignore failure
        Sleep(5);
    }

    for(int i=0;i<3;i++)
    {
        // mute session
        METHODCHECKRESULT("ISimpleAudioVolume::SetMute",
                          hr,
                          spVolume->SetMute(TRUE, NULL));

        // ignore failure

    }
    
    // un-mute session
    METHODCHECKRESULT("ISimpleAudioVolume::SetMute",
                      hr,
                      spVolume->SetMute(FALSE, NULL));

    // if we exit the function through this path, ignore failed HRESULTs
    hr = S_OK;
    
END:
    return hr;
}
