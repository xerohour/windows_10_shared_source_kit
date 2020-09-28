// Copyright(C) Microsoft. All rights reserved.

//
// SessionEventingTests.cpp
//  
//
// Description:
//
//  WASAPI AudioSession Eventing Scenario Tests

#include <stdafx.h>
#include <UMATestStreaming.h>


#include "TestUtilities.h"

#include "StreamingControlTestFramework.h"
#include "AudioSessionEventHandler.h"
#include "EndpointVolumeEventHandler.h"

const UINT32 EVENT_TIMEOUT = 10000; // timout after 10s

// {5C765221-5DAF-43d8-9A4B-E4442D216432}
static const GUID GUID_EVENT_SCENARIO = 
{ 0x5c765221, 0x5daf, 0x43d8, { 0x9a, 0x4b, 0xe4, 0x44, 0x2d, 0x21, 0x64, 0x32 } };

DWORD TC_PerStreamEvents_Scenario1()
{
    RunStreamingControlTest(PERSTREAMEVENTS_SCENARIO1);
    return FNS_PASS;       
}

DWORD TC_PerStreamEvents_Scenario2()
{
    RunStreamingControlTest(PERSTREAMEVENTS_SCENARIO2);
    return FNS_PASS;       
}

DWORD TC_PerStreamEvents_Scenario3()
{
    RunStreamingControlTest(PERSTREAMEVENTS_SCENARIO3);
    return FNS_PASS;       
}

DWORD TC_PerStreamEvents_Scenario4()
{
    RunStreamingControlTest(PERSTREAMEVENTS_SCENARIO4);
    return FNS_PASS;       
}

DWORD TC_PerStreamEvents_Scenario5()
{
    RunStreamingControlTest(PERSTREAMEVENTS_SCENARIO5);
    return FNS_PASS;       
}

DWORD TC_PerStreamEvents_Scenario6()
{
    RunStreamingControlTest(PERSTREAMEVENTS_SCENARIO6);
    return FNS_PASS;       
}

DWORD TC_PerStreamEvents_Scenario7()
{
    RunStreamingControlTest(PERSTREAMEVENTS_SCENARIO7);
    return FNS_PASS;       
}

DWORD TC_PerStreamEvents_Scenario8()
{
    RunStreamingControlTest(PERSTREAMEVENTS_SCENARIO8);
    return FNS_PASS;       
}

DWORD TC_PerStreamEvents_Scenario9()
{
    RunStreamingControlTest(PERSTREAMEVENTS_SCENARIO9, TRUE); // suppress streaming errors
    return FNS_PASS;       
}


//-----------------------------------------------------------------------------
// Desc: getaudiosessionevent_volume
//
//-----------------------------------------------------------------------------
DWORD Test_PerStreamEvents_Scenario1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl> spAudSesCtrl = NULL;
    CComPtr<IAudioSessionEvents> spAudioSessionEvents = NULL;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;
    CComPtr<IAudioEndpointVolumeCallback> spEndpointEventHandler = NULL;
    CComPtr<ISimpleAudioVolume> spSimpleAudioVolume = NULL;
    CComPtr<IChannelAudioVolume> spChannelAudioVolume = NULL;


    BOOLEAN bEndpointEventsSubscribed = FALSE;
    BOOLEAN bSessionEventsSubscribed = FALSE;

    CAudioSessionEventHandler* pSessionEventHandler = NULL;
    CEndpointVolumeEventHandler* pobjEndpointEventHandler = NULL;

    UINT nSessionChannels = 0;
    UINT nVolumeChanges = 0;
    FLOAT fMasterVolumeLevel = 0.0f;
    FLOAT fNewVolumeLevel = 0.0f;
    
    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControl(&spAudSesCtrl);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl Interface");
        goto END;
    }

    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume Interface");
        goto END;
    }

    hr = pStreamingEngine->GetSimpleAudioVolume(&spSimpleAudioVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume Interface");
        goto END;
    }

    hr = pStreamingEngine->GetChannelAudioVolume(&spChannelAudioVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IChannelAudioVolume Interface");
        goto END;
    }

    //  set the channel volumes for the session to unity.
    hr = spChannelAudioVolume->GetChannelCount(&nSessionChannels);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: IChannelAudioVolume::GetChannelCount returned 0x%08x", hr);
        goto END;
    }

    for (UINT32 i=0; i<nSessionChannels; i++)
    {
        hr = spChannelAudioVolume->SetChannelVolume(i, 1.0f, NULL);
        if (FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: IChannelAudioVolume::SetChannelVolume returned 0x%08x", hr);
            goto END;
        }
    }


    // Create an event handler
    pSessionEventHandler = new CAudioSessionEventHandler();
    if(NULL == pSessionEventHandler)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }

    METHODCHECKRESULT("CTestAudioSessionEvents::QueryInterface",
                      hr,
                      pSessionEventHandler->QueryInterface(__uuidof(IAudioSessionEvents),
                                                            (void**)&spAudioSessionEvents));
    if(FAILED(hr)) 
    { 
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected result!");
    }

    METHODCHECKRESULT("IAudioSessionControl::RegisterAudioSessionNotification",
                      hr,
                      spAudSesCtrl->RegisterAudioSessionNotification(spAudioSessionEvents));       
    if(FAILED(hr)) 
    {
        XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected result!");
    }
    else
    {
        bSessionEventsSubscribed = TRUE;
    }

    //
    // If this is a capture scenario, subscribe to endpoint volume notifications
    //
    if(DEVICE_TYPE_CAPTURE & dwDeviceType)
    {
        pobjEndpointEventHandler = new CEndpointVolumeEventHandler();
        if(NULL == pobjEndpointEventHandler)
        {
            XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
            goto END;
        }        

        METHODCHECKRESULT("IAudioEndpointVolumeHandler::QueryInterface",
                          hr,
                          pobjEndpointEventHandler->QueryInterface(IID_IAudioEndpointVolumeCallback,
                                                                  (void**)(&spEndpointEventHandler)));

        if(FAILED(hr))
        {
            goto END;
        }

        // register for notifications
        METHODCHECKRESULT("IAudioEndpointVolume::RegisterControlChangeNotify",
                          hr,
                          spEndpointVolume->RegisterControlChangeNotify(spEndpointEventHandler));
        if(FAILED(hr))
        {
            goto END;
        }
        else
        {
            bEndpointEventsSubscribed = TRUE;
        }        
    }

    // Clear any pending session or endpoint events
    pSessionEventHandler->SetEventFilterAndReset(&GUID_EVENT_SCENARIO);
    
    if(DEVICE_TYPE_CAPTURE & dwDeviceType)
    {
        pobjEndpointEventHandler->Reset();
    }
   
    //// start scenario

    // get the master volume level 
    METHODCHECKRESULT("ISimpleAudioVolume::GetMasterVolume",
                      hr,
                      spSimpleAudioVolume->GetMasterVolume(&fMasterVolumeLevel));
    if(FAILED(hr)) 
    { 
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected result");
        goto END; 
    }

    // change the master volume level    
    fNewVolumeLevel = fMasterVolumeLevel + 0.1f;
    if(fNewVolumeLevel > 1.0f)
    {
        fNewVolumeLevel -= 0.2f;
    }

    METHODCHECKRESULT("ISimpleAudioVolume::SetMasterVolume",
                      hr,
                      spSimpleAudioVolume->SetMasterVolume(fNewVolumeLevel, &GUID_EVENT_SCENARIO));                         
    if(FAILED(hr)) 
    { 
        XLOG(XFAIL, eError, "FAIL: ISimpleVolume::SetMasterVolume(%f) failed", fNewVolumeLevel);
        goto END; 
    }

    hr = WaitForSessionEvent(pSessionEventHandler, 
                             AUDIO_EVENT_SIMPLEVOLUME_CHANGED, 
                             EVENT_TIMEOUT);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: timeout waiting for session event");
        goto END;
    }    
       
    // verify that the correct number of events were dispatched
    nVolumeChanges = pSessionEventHandler->GetVolumeChangedEventCount();
    if(1 != nVolumeChanges)
    {
        XLOG(XFAIL,
             eFatalError, 
             "FAIL: Expected 1 in-context volume change notification, %d reported",
             nVolumeChanges);
        goto END;
    }

    // verify that the correct volume was reported
    pSessionEventHandler->GetLastReportedSimpleVolume(&fMasterVolumeLevel); 

    if(EPSILON < abs(fMasterVolumeLevel-fNewVolumeLevel))
    {
        XLOG(XFAIL,
             eError, 
             "FAIL: Set (%f) and event (%f) master volume levels differ",
             fNewVolumeLevel, 
             fMasterVolumeLevel);
    }

    // if this is a capture scenario, verify that the endpoint volume matches
    if(DEVICE_TYPE_CAPTURE & dwDeviceType)
    {
        FLOAT fEndpointVol = 0.0f;

        hr = WaitForEndpointEvent(pobjEndpointEventHandler, EVENT_TIMEOUT);
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Endpoint event not received in response to session volume change");
            goto END;
        }

        Sleep(100);

        METHODCHECKRESULT("IAudioEndpointVolume::GetMasterVolumeLevelScalar",
                          hr,
                          spEndpointVolume->GetMasterVolumeLevelScalar(&fEndpointVol));
        if(EPSILON<abs(fEndpointVol-fMasterVolumeLevel))
        { 
            XLOG(XFAIL,
                 eError, 
                 "FAIL: Expected (%f) and reported (%f) endpoint volumes do not match (session volume should have propagated)",
                 fNewVolumeLevel,
                 fEndpointVol);
            goto END;
        }        
    }
    
    //// end scenario 
END:
    if(bEndpointEventsSubscribed)
    {
        assert(!(spEndpointEventHandler == NULL));
        spEndpointVolume->UnregisterControlChangeNotify(spEndpointEventHandler);
    }

    if(bSessionEventsSubscribed)
    {
        assert(!(spAudioSessionEvents == NULL));
        spAudSesCtrl->UnregisterAudioSessionNotification(spAudioSessionEvents);            
    }

    SAFE_RELEASE_POINTER(pobjEndpointEventHandler);
    SAFE_RELEASE_POINTER(pSessionEventHandler);

    return FNS_PASS;    
}

//-----------------------------------------------------------------------------
// Desc: getaudiosessionevent_mute
//
//-----------------------------------------------------------------------------
DWORD Test_PerStreamEvents_Scenario2(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl> spAudSesCtrl = NULL;
    CComPtr<IAudioSessionEvents> spAudioSessionEvents = NULL;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;
    CComPtr<IAudioEndpointVolumeCallback> spEndpointEventHandler = NULL;
    CComPtr<ISimpleAudioVolume> spSimpleAudioVolume = NULL;

    BOOLEAN bEndpointEventsSubscribed = FALSE;
    BOOLEAN bSessionEventsSubscribed = FALSE;

    CAudioSessionEventHandler* pSessionEventHandler = NULL;
    CEndpointVolumeEventHandler* pobjEndpointEventHandler = NULL;
    
    BOOL bMuteState = FALSE;
    BOOL bNewMuteState = FALSE;
    BOOL bEndpointMuteState = FALSE;
    UINT nVolumeEvents = 0;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControl(&spAudSesCtrl);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl Interface");
        goto END;
    }

    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume Interface");
        goto END;
    }

    hr = pStreamingEngine->GetSimpleAudioVolume(&spSimpleAudioVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume Interface");
        goto END;
    }

    pSessionEventHandler = new CAudioSessionEventHandler();
    if(NULL == pSessionEventHandler)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }

    METHODCHECKRESULT("CTestAudioSessionEvents::QueryInterface",
                      hr,
                      pSessionEventHandler->QueryInterface(__uuidof(IAudioSessionEvents),
                                                            (void**)&spAudioSessionEvents));
    if(FAILED(hr)) 
    { 
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected result!");
    }

    METHODCHECKRESULT("IAudioSessionControl::RegisterAudioSessionNotification",
                      hr,
                      spAudSesCtrl->RegisterAudioSessionNotification(spAudioSessionEvents));       
    if(FAILED(hr)) 
    {
        XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected result!");
    }
    else
    {
        bSessionEventsSubscribed = TRUE;
    }

    //
    // If this is a capture scenario, subscribe to endpoint volume notifications
    //
    if(DEVICE_TYPE_CAPTURE & dwDeviceType)
    {
        pobjEndpointEventHandler = new CEndpointVolumeEventHandler();
        if(NULL == pobjEndpointEventHandler)
        {
            XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
            goto END;
        }        

        METHODCHECKRESULT("IAudioEndpointVolumeHandler::QueryInterface",
                          hr,
                          pobjEndpointEventHandler->QueryInterface(IID_IAudioEndpointVolumeCallback,
                                                                  (void**)(&spEndpointEventHandler)));

        if(FAILED(hr))
        {
            goto END;
        }

        // register for notifications
        METHODCHECKRESULT("IAudioEndpointVolume::RegisterControlChangeNotify",
                          hr,
                          spEndpointVolume->RegisterControlChangeNotify(spEndpointEventHandler));
        if(FAILED(hr))
        {
            goto END;
        }
        else
        {
            bEndpointEventsSubscribed = TRUE;
        }        
    }

    //// start scenario
    
    pSessionEventHandler->ResetHandler();
    pSessionEventHandler->SetEventFilterAndReset(&GUID_EVENT_SCENARIO); 
    if(DEVICE_TYPE_CAPTURE & dwDeviceType)
    {
        pobjEndpointEventHandler->Reset();
    }
    
    // get initial mute notification event count  
    nVolumeEvents = pSessionEventHandler->GetVolumeChangedEventCount();
    if(0 != nVolumeEvents)
    {
        XLOG(XFAIL, eError, "FAIL: Volume change count = %d, expected 0", nVolumeEvents);
        goto END;
    }    

    // get the current audiosession mute value
    METHODCHECKRESULT("ISimpleAudioVolume::GetMute",
                      hr,
                      spSimpleAudioVolume->GetMute(&bMuteState));
    if(FAILED(hr)) 
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }
    
    // toggle mute once
    METHODCHECKRESULT("ISimpleAudioVolume::SetMute",
                      hr,
                      spSimpleAudioVolume->SetMute(!bMuteState, &GUID_EVENT_SCENARIO));
    SLOG(eInfo1, "Setting session mute state to %d", !bMuteState);        

    hr = WaitForSessionEvent(pSessionEventHandler, 
                             AUDIO_EVENT_SIMPLEVOLUME_CHANGED, 
                             EVENT_TIMEOUT);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: timeout waiting for event");
        goto END;
    }    
    
    // verify the correct number of events were dispatched
    nVolumeEvents = pSessionEventHandler->GetVolumeChangedEventCount();   
    if(1 != nVolumeEvents)
    {
        XLOG(XFAIL, eError, "FAIL: 1 Expected %d volume events, observed %d", 1, nVolumeEvents);
        goto END;
    }

    // verify that the correct mute state was reported
    pSessionEventHandler->GetLastReportedMuteState(&bNewMuteState);
    if(bNewMuteState != (!bMuteState))
    {
        XLOG(XFAIL, eError, "FAIL: Mute state %d reported, expected %d", bNewMuteState, !bMuteState);
        goto END;
    }

    // if this is a capture scenario, verify that the endpoint volume matches
    if(DEVICE_TYPE_CAPTURE & dwDeviceType)
    {     
        hr = WaitForEndpointEvent(pobjEndpointEventHandler, EVENT_TIMEOUT);
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Endpoint event not received in response to session volume change");
            goto END;
        }

        Sleep(100);

        METHODCHECKRESULT_GOTO("IAudioEndpointVolume::GetMute",
                               hr,
                               spEndpointVolume->GetMute(&bEndpointMuteState),
                               END);
        if(bEndpointMuteState != bNewMuteState)
        {
            XLOG(XFAIL,
                 eFatalError, 
                 "ERROR: Expected (%d) and observed (%d) endpoint mute states do not match!",
                 bNewMuteState,
                 bEndpointMuteState);
            goto END;
        }
    }                      

    // update the event filter
    pSessionEventHandler->SetEventFilterAndReset(&GUID_EVENT_SCENARIO);
    
    // toggle mute again
    METHODCHECKRESULT("ISimpleAudioVolume::SetMute",
                      hr,
                      spSimpleAudioVolume->SetMute(bMuteState, &GUID_EVENT_SCENARIO));


    hr = WaitForSessionEvent(pSessionEventHandler, 
                             AUDIO_EVENT_SIMPLEVOLUME_CHANGED, 
                             EVENT_TIMEOUT);
    if(FAILED(hr))
    {
        SLOG(eFatalError, "FAIL: timeout waiting for event");
        goto END;
    }    

    // verify the correct number of events were dispatched
    nVolumeEvents = pSessionEventHandler->GetVolumeChangedEventCount();   
    if(1 > nVolumeEvents)
    {
        XLOG(XFAIL,
             eFatalError, 
             "FAIL: <2nd try> Expected %d volume events, observed %d", 
             1,
             nVolumeEvents);
        goto END;
    }
    
    // verify that the correct mute state was reported
    pSessionEventHandler->GetLastReportedMuteState(&bNewMuteState);
    if(bNewMuteState != bMuteState)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Mute state %d reported, expected %d", bNewMuteState, bMuteState);
        goto END;
    }

    // if this is a capture scenario, verify that the endpoint volume matches
    if(DEVICE_TYPE_CAPTURE & dwDeviceType)
    {
        // verify that the endpoint mute state agrees
        METHODCHECKRESULT("IAudioEndpointVolume::GetMute",
                           hr,
                           spEndpointVolume->GetMute(&bEndpointMuteState));
        if(bEndpointMuteState != bNewMuteState)
        {
            XLOG(XFAIL,
                 eFatalError, 
                 "FAIL: Expected (%d) and observed (%d) endpoint mute states do not match!",
                 bNewMuteState,
                 bEndpointMuteState);
            goto END;
        }
    }

    //// end scenario 
END:
    if(bEndpointEventsSubscribed)
    {
        assert(!(spEndpointEventHandler == NULL));
        spEndpointVolume->UnregisterControlChangeNotify(spEndpointEventHandler);
    }

    if(bSessionEventsSubscribed)
    {
        assert(!(spAudioSessionEvents == NULL));
        spAudSesCtrl->UnregisterAudioSessionNotification(spAudioSessionEvents);            
    }

    SAFE_RELEASE_POINTER(pobjEndpointEventHandler);
    SAFE_RELEASE_POINTER(pSessionEventHandler);

    return FNS_PASS;    

}

//------------------------------------------------------------------------------
// Desc: Session Class Change Eventing Scenario
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_PerStreamEvents_Scenario3(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl> spAudSesCtrl = NULL;
    CComPtr<IAudioSessionEvents> spAudioSessionEvents = NULL;

    BOOLEAN bSessionEventsSubscribed = FALSE;

    CAudioSessionEventHandler* pSessionEventHandler = NULL;

    GUID    put_guid = GUID_NULL;
    GUID    get_guid = GUID_NULL;

    UINT nEvents = 0;
    UINT nReportedEvents= 0;
    

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControl(&spAudSesCtrl);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl Interface");
        goto END;
    }

    pSessionEventHandler = new CAudioSessionEventHandler();
    if(NULL == pSessionEventHandler)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }

    METHODCHECKRESULT("CTestAudioSessionEvents::QueryInterface",
                      hr,
                      pSessionEventHandler->QueryInterface(__uuidof(IAudioSessionEvents),
                                                            (void**)&spAudioSessionEvents));
    if(FAILED(hr)) 
    { 
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected result!");
    }

    METHODCHECKRESULT("IAudioSessionControl::RegisterAudioSessionNotification",
                      hr,
                      spAudSesCtrl->RegisterAudioSessionNotification(spAudioSessionEvents));       
    if(FAILED(hr)) 
    {
        XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected result!");
    }
    else
    {
        bSessionEventsSubscribed = TRUE;
    }

    // Clear any pending events
    pSessionEventHandler->SetEventFilterAndReset(&GUID_EVENT_SCENARIO);

    //// start scenario

    // get initial grouping param notification event count    
    nEvents = pSessionEventHandler->GetGroupingParamChangedEventCount();
    if(nEvents != 0)
    {
        XLOG(XFAIL, eError, "FAIL: Expected event count = 0, observed %d", nEvents);
        goto END;
    }

    // set the grouping param
    hr = CoCreateGuid(&put_guid);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Unable to create a GUID! (0x%8x)", hr);
        goto END;
    }

    METHODCHECKRESULT("IAudioSessionControl::SetGroupingParam",
                      hr,
                      spAudSesCtrl->SetGroupingParam(&put_guid, &GUID_EVENT_SCENARIO));
    if(FAILED(hr)) { goto END; }
        
    // wait for event
    hr = WaitForSessionEvent(pSessionEventHandler, 
                             AUDIO_EVENT_GROUPINGPARAM_CHANGED, 
                             EVENT_TIMEOUT);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: timeout waiting for event");
        goto END;
    }    

    // Get the GroupingParamChanged event count
    nReportedEvents = pSessionEventHandler->GetGroupingParamChangedEventCount();
    if(1 != nReportedEvents)
    {
        XLOG(XFAIL, eError, "FAIL: Expected 1 event, observed %d", nReportedEvents);
        goto END;
    }

    // Verify that the correct GUID was reported
    pSessionEventHandler->GetLastReportedGroupingParam(&get_guid);
    
    if(!IsEqualGUID(put_guid,get_guid))
    {
        XLOG(XFAIL, eError,"FAIL: New GroupingParam value incorrect. (put_guid != get_guid)");
        LogGUID(put_guid);
        LogGUID(get_guid);
        goto END;
    }

    //// end scenario
END:
    if(bSessionEventsSubscribed)
    {
        assert(!(spAudioSessionEvents == NULL));
        spAudSesCtrl->UnregisterAudioSessionNotification(spAudioSessionEvents);            
    }

    SAFE_RELEASE_POINTER(pSessionEventHandler);

    return FNS_PASS; 
}

//------------------------------------------------------------------------------
// Desc: master volume fade eventing scenario
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_PerStreamEvents_Scenario4(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl> spAudSesCtrl = NULL;
    CComPtr<IAudioSessionEvents> spAudioSessionEvents = NULL;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;
    CComPtr<IAudioEndpointVolumeCallback> spEndpointEventHandler = NULL;
    CComPtr<ISimpleAudioVolume> spSimpleAudioVolume = NULL;

    BOOLEAN bEndpointEventsSubscribed = FALSE;
    BOOLEAN bSessionEventsSubscribed = FALSE;
    GUID guidEventContextId = GUID_NULL;
    
    CAudioSessionEventHandler* pSessionEventHandler = NULL;
    CEndpointVolumeEventHandler* pobjEndpointEventHandler = NULL;
    
    FLOAT fSetVolume = 0.0f;
    FLOAT fReportedVolume = 0.0f;
    FLOAT fReportedEndpointVolume = 0.0f;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControl(&spAudSesCtrl);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl Interface");
        goto END;
    }

    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume Interface");
        goto END;
    }

    hr = pStreamingEngine->GetSimpleAudioVolume(&spSimpleAudioVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume Interface");
        goto END;
    }

    pSessionEventHandler = new CAudioSessionEventHandler();
    if(NULL == pSessionEventHandler)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }

    METHODCHECKRESULT("CTestAudioSessionEvents::QueryInterface",
                      hr,
                      pSessionEventHandler->QueryInterface(__uuidof(IAudioSessionEvents),
                                                            (void**)&spAudioSessionEvents));
    if(FAILED(hr)) 
    { 
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected result!");
    }

    METHODCHECKRESULT("IAudioSessionControl::RegisterAudioSessionNotification",
                      hr,
                      spAudSesCtrl->RegisterAudioSessionNotification(spAudioSessionEvents));       
    if(FAILED(hr)) 
    {
        XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected result!");
    }
    else
    {
        bSessionEventsSubscribed = TRUE;
    }

    //
    // If this is a capture scenario, subscribe to endpoint volume notifications
    //
    if(DEVICE_TYPE_CAPTURE & dwDeviceType)
    {
        pobjEndpointEventHandler = new CEndpointVolumeEventHandler();
        if(NULL == pobjEndpointEventHandler)
        {
            XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
            goto END;
        }        

        METHODCHECKRESULT("IAudioEndpointVolumeHandler::QueryInterface",
                          hr,
                          pobjEndpointEventHandler->QueryInterface(IID_IAudioEndpointVolumeCallback,
                                                                  (void**)(&spEndpointEventHandler)));

        if(FAILED(hr))
        {
            goto END;
        }

        // register for notifications
        METHODCHECKRESULT("IAudioEndpointVolume::RegisterControlChangeNotify",
                          hr,
                          spEndpointVolume->RegisterControlChangeNotify(spEndpointEventHandler));
        if(FAILED(hr))
        {
            goto END;
        }
        else
        {
            bEndpointEventsSubscribed = TRUE;
        }        
    }

    // Clear any pending events
    pSessionEventHandler->SetEventFilterAndReset(&GUID_EVENT_SCENARIO);

    if(DEVICE_TYPE_CAPTURE & dwDeviceType)
    {
        pobjEndpointEventHandler->Reset();
    }
   
    //// start scenario

    // get the current master volume level
    METHODCHECKRESULT("ISimpleAudioVolume::GetMasterVolume",            
                      hr,
                      spSimpleAudioVolume->GetMasterVolume(&fReportedVolume));        
    if((fReportedVolume < 0.0f) || (fReportedVolume > 1.0f))
    {
        XLOG(XFAIL,eError,"FAIL: Volume value out of range [0,1] (%f)",fReportedVolume);
        goto END;
    }

    // if this is a capture scenario, verify that the endpoint volume matches
    if(DEVICE_TYPE_CAPTURE & dwDeviceType)
    {
        Sleep(100);

        // get the current endpoint master volume level
        METHODCHECKRESULT("IAudioEndpointVolume::GetMasterVolumeLevel",
                          hr,
                          spEndpointVolume->GetMasterVolumeLevelScalar(&fReportedEndpointVolume));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        }        
        if(EPSILON<abs(fReportedEndpointVolume-fReportedVolume))
        {
            XLOG(XFAIL,
                 eFatalError,
                 "FAIL: Observed (%f) and expected (%f) endpoint volumes do not match",
                 fReportedEndpointVolume,
                 fReportedVolume);
            goto END;             
        }
    }    

    SLOG(eInfo1, "Current Master Volume; %f", fReportedVolume);


    fSetVolume = 1.0f;
    
    //
    // fade from current volume to zero in increments of 0.2
    //
    while(true)
    {
        Sleep(200);

        // generate a context guid for this event
        METHODCHECKRESULT("::CoCreateGuid",
                          hr,
                          CoCreateGuid(&guidEventContextId));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eFatalError, "FAIL: Unable to create GUID");
            goto END;
        }

        pSessionEventHandler->SetEventFilterAndReset(&guidEventContextId);
        if(DEVICE_TYPE_CAPTURE & dwDeviceType)
        {
            pobjEndpointEventHandler->Reset();
        }

        fSetVolume = max(0.0f, fSetVolume-.2f);
        SLOG(eInfo1, "Setting master volume to %.2f", fSetVolume);
        METHODCHECKRESULT("ISimpleVolume::SetMasterVolume",
                          hr,
                          spSimpleAudioVolume->SetMasterVolume(fSetVolume,&guidEventContextId));
        if(FAILED(hr))
        {
            goto END;
        }

        // wait up to two seconds for the volume changed event to arrive
        hr = WaitForSessionEvent(pSessionEventHandler, 
                                 AUDIO_EVENT_SIMPLEVOLUME_CHANGED, 
                                 EVENT_TIMEOUT);
        if(FAILED(hr))
        {
            XLOG(XFAIL, eFatalError, "FAIL: timeout waiting for session event");
            goto END;
        }

        Sleep(100);
        
        // verify that the set value was correct
        pSessionEventHandler->GetLastReportedSimpleVolume(&fReportedVolume);
        if(EPSILON < abs(fReportedVolume-fSetVolume))
        {
            XLOG(XFAIL,
                 eFatalError, 
                 "FAIL: set (%f) and observed (%f) volumes do not match",
                 fSetVolume,
                 fReportedVolume);         
        }
        
        if(FAILED(hr))
        {
            goto END;
        }
        else
        {        
            SLOG(eInfo1, "Event data validated.");
        } 

        // if this is a capture scenario, verify that the endpoint volume matches
        if(DEVICE_TYPE_CAPTURE & dwDeviceType)
        {
            hr = WaitForEndpointEvent(pobjEndpointEventHandler, EVENT_TIMEOUT);
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: Endpoint event not received in response to session volume change");
                goto END;
            }

            Sleep(100);            

            // verify that the reported volume matches the endpoint volume
            METHODCHECKRESULT("IAudioEndpointVolume::GetMasterVolumeLevelScalar",
                              hr,
                              spEndpointVolume->GetMasterVolumeLevelScalar(&fReportedEndpointVolume));                           
            if(EPSILON<abs(fSetVolume-fReportedEndpointVolume))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: Observed (%f) and expected (%f) endpoint volumes do not match",
                     fReportedEndpointVolume,
                     fSetVolume);
                goto END;
            }
        }
        
        // break if we're done fading down
        if(0.0f >= fSetVolume)
        {
            break;
        }
    }

    //
    // fade from current volume to 1.0 in increments of .2
    //
    fSetVolume=0.0f;
    while(true)
    {
        Sleep(200);
    
        // generate a context guid for this event
        METHODCHECKRESULT("::CoCreateGuid",
                          hr,
                          CoCreateGuid(&guidEventContextId));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eFatalError, "FAIL: Unable to create GUID");
            goto END;
        }

        pSessionEventHandler->SetEventFilterAndReset(&guidEventContextId);
        if(DEVICE_TYPE_CAPTURE & dwDeviceType)
        {
            pobjEndpointEventHandler->Reset();
        }

        fSetVolume = min(1.0f, fSetVolume+.2f);
        SLOG(eInfo1, "Setting master volume to %.2f", fSetVolume);
        METHODCHECKRESULT("ISimpleVolume::SetMasterVolume",
                          hr,
                          spSimpleAudioVolume->SetMasterVolume(fSetVolume, &guidEventContextId));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        }        

        // wait up to two seconds for the volume changed event to arrive
        hr = WaitForSessionEvent(pSessionEventHandler, 
                                 AUDIO_EVENT_SIMPLEVOLUME_CHANGED, 
                                 EVENT_TIMEOUT);
        if(FAILED(hr))
        {
            XLOG(XFAIL, eFatalError, "FAIL: TIMEOUT waiting for event");
            goto END;
        }

        Sleep(100);
        
        // verify that the set value was correct
        pSessionEventHandler->GetLastReportedSimpleVolume(&fReportedVolume);
        if(EPSILON < abs(fReportedVolume-fSetVolume))
        {
            XLOG(XFAIL,
                 eFatalError, 
                 "FAIL: Set (%f) and observed (%f) volumes do not match",
                 fSetVolume,
                 fReportedVolume);
        }
       
        if(FAILED(hr))
        {
            goto END;
        }
        else
        {
            SLOG(eInfo1, "Event data validated.");
        }

        // if this is a capture scenario, verify that the endpoint volume matches
        if(DEVICE_TYPE_CAPTURE & dwDeviceType)
        {
            hr = WaitForEndpointEvent(pobjEndpointEventHandler, EVENT_TIMEOUT);
            if(FAILED(hr))
            {
                XLOG(XFAIL, eFatalError, "FAIL: Endpoint event not received in response to session volume change");
                goto END;
            }

            Sleep(100);

            // verify that the reported volume matches the endpoint volume
            METHODCHECKRESULT("IAudioEndpointVolume::GetMasterVolumeLevelScalar",
                              hr,
                              spEndpointVolume->GetMasterVolumeLevelScalar(&fReportedEndpointVolume));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            }
            if(EPSILON<abs(fSetVolume-fReportedEndpointVolume))
            {
                XLOG(XFAIL,
                    eFatalError,
                    "FAIL: Observed (%f) and expected (%f) endpoint volumes do not match",
                    fReportedEndpointVolume,
                    fSetVolume);
                goto END;
            }
        }
        
        // break if we're done fading up
        if(1.0f <= fSetVolume)
        {
            break;
        }
    }
    

    //// end scenario 
END:
    if(bEndpointEventsSubscribed)
    {
        assert(!(spEndpointEventHandler == NULL));
        spEndpointVolume->UnregisterControlChangeNotify(spEndpointEventHandler);
    }

    if(bSessionEventsSubscribed)
    {
        assert(!(spAudioSessionEvents == NULL));
        spAudSesCtrl->UnregisterAudioSessionNotification(spAudioSessionEvents);            
    }

    SAFE_RELEASE_POINTER(pobjEndpointEventHandler);
    SAFE_RELEASE_POINTER(pSessionEventHandler);

    return FNS_PASS;    
}


//------------------------------------------------------------------------------
// Desc: Display Name Change Eventing Scenario
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_PerStreamEvents_Scenario5(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl> spAudSesCtrl = NULL;
    CComPtr<IAudioSessionEvents> spAudioSessionEvents = NULL;

    BOOLEAN bSessionEventsSubscribed = FALSE;

    CAudioSessionEventHandler* pSessionEventHandler = NULL;

    UINT nEvents = 0;
    UINT nReportedEvents= 0;

    // constant strings
    const LPCWSTR wszNames[] = {L"Display Name 1",
                                L"--DISPLAY NAME 2--",
                                L"xxxxxxx3333333333333333333333xxxxxx"};
    

    GUID       guidEventContextId = GUID_NULL;
    LPCWSTR    wszGetName    = NULL;    

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControl(&spAudSesCtrl);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl Interface");
        goto END;
    }

    pSessionEventHandler = new CAudioSessionEventHandler();
    if(NULL == pSessionEventHandler)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }

    METHODCHECKRESULT("CTestAudioSessionEvents::QueryInterface",
                      hr,
                      pSessionEventHandler->QueryInterface(__uuidof(IAudioSessionEvents),
                                                            (void**)&spAudioSessionEvents));
    if(FAILED(hr)) 
    { 
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected result!");
    }

    METHODCHECKRESULT("IAudioSessionControl::RegisterAudioSessionNotification",
                      hr,
                      spAudSesCtrl->RegisterAudioSessionNotification(spAudioSessionEvents));       
    if(FAILED(hr)) 
    {
        XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected result!");
    }
    else
    {
        bSessionEventsSubscribed = TRUE;
    }

    // Clear any pending events
    pSessionEventHandler->SetEventFilterAndReset(&GUID_EVENT_SCENARIO);

    //// start scenario

    // get initial grouping param notification event count    
    nEvents= pSessionEventHandler->GetDisplayNameChangedEventCount();
    if(nEvents != 0)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Expected event count = 0, observed %d", nEvents);
        goto END;
    }

    for(int i=0;i<3;i++)
    {
        // generate a guid for the context id
        METHODCHECKRESULT("::CoCreateGuid",
                          hr,
                          CoCreateGuid(&guidEventContextId));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eFatalError, "FAIL: unable to generate a GUID");
            goto END;
        }

        //update the event filter
        pSessionEventHandler->SetEventFilterAndReset(&guidEventContextId);

        // set the display name      
        METHODCHECKRESULT("IAudioSessionControl::SetDisplayName",
                          hr,
                          spAudSesCtrl->SetDisplayName(wszNames[i], &guidEventContextId));
        if(FAILED(hr)) 
        { 
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            goto END; 
        }
        
        hr = WaitForSessionEvent(pSessionEventHandler, 
                                 AUDIO_EVENT_DISPLAYNAME_CHANGED, 
                                 EVENT_TIMEOUT);
        if(FAILED(hr))
        {
            XLOG(XFAIL, eFatalError, "FAIL: timeout waiting for event");
            goto END;
        }    
       
        // Get the GroupingParamChanged event count
        nReportedEvents = pSessionEventHandler->GetDisplayNameChangedEventCount();
        if(nReportedEvents <= 0)
        {
            XLOG(XFAIL, eFatalError, "FAIL: Expected 1 event, observed %d", nReportedEvents);
            goto END;
        }

        hr = pSessionEventHandler->GetLastReportedDisplayName(&wszGetName);
        if(FAILED(hr))
        {
            XLOG(XFAIL, eFatalError, "FAIL: Error getting display name");
            goto END;
        }

        // Verify that the correct string was reported
        if(0 == wcscmp(wszNames[i], wszGetName))
        {
            SLOG(eInfo1, "Strings match.");
            SAFE_DELETE_ARRAY(wszGetName);
        }
        else
        {
            XLOG(XFAIL, eError, "FAIL: Event (%ws) and set (%ws) strings do not match!", wszNames[i], wszGetName);
            SAFE_DELETE_ARRAY(wszGetName);
        }
        

        // this call is necessary to prevent possible access violations
        pSessionEventHandler->ClearContextIdentifiers();        
    }    

    //// end scenario
END:
    SAFE_DELETE_ARRAY(wszGetName);

    if(bSessionEventsSubscribed)
    {
        assert(!(spAudioSessionEvents == NULL));
        spAudSesCtrl->UnregisterAudioSessionNotification(spAudioSessionEvents);            
    }

    SAFE_RELEASE_POINTER(pSessionEventHandler);

    return FNS_PASS; 
}

//------------------------------------------------------------------------------
// Desc: Icon Path Change Eventing Scenario
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_PerStreamEvents_Scenario6(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl> spAudSesCtrl = NULL;
    CComPtr<IAudioSessionEvents> spAudioSessionEvents = NULL;

    BOOLEAN bSessionEventsSubscribed = FALSE;

    CAudioSessionEventHandler* pSessionEventHandler = NULL;

    const LPCWSTR wszIconPaths[] = {L"\\path\\one\\",
                                    L"c:\\path\\3\\blah\\blah\\blah\\\\",
                                    L"xxxxxx\\x333333333\\333\\333333333\\3:xxxxxx"};
    

    LPCWSTR    wszGetPath    = NULL;

    UINT nEvents = 0;
    UINT nReportedEvents= 0;
    GUID guidContextId = GUID_NULL;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControl(&spAudSesCtrl);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl Interface");
        goto END;
    }

    pSessionEventHandler = new CAudioSessionEventHandler();
    if(NULL == pSessionEventHandler)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }

    METHODCHECKRESULT("CTestAudioSessionEvents::QueryInterface",
                      hr,
                      pSessionEventHandler->QueryInterface(__uuidof(IAudioSessionEvents),
                                                            (void**)&spAudioSessionEvents));
    if(FAILED(hr)) 
    { 
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected result!");
    }

    METHODCHECKRESULT("IAudioSessionControl::RegisterAudioSessionNotification",
                      hr,
                      spAudSesCtrl->RegisterAudioSessionNotification(spAudioSessionEvents));       
    if(FAILED(hr)) 
    {
        XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected result!");
    }
    else
    {
        bSessionEventsSubscribed = TRUE;
    }

    // Clear any pending events
    pSessionEventHandler->SetEventFilterAndReset(&GUID_EVENT_SCENARIO);

    //// start scenario

    // get initial grouping param notification event count    
    nEvents = pSessionEventHandler->GetIconPathChangedEventCount();
    if(nEvents != 0)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Expected event count = 0, observed %d", nEvents);
        goto END;
    }

    for(int i=0;i<3;i++)
    {
        // generate a GUID for the event context ID
        METHODCHECKRESULT("::CoCreateGUID",
                          hr,
                          CoCreateGuid(&guidContextId));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eFatalError, "FAIL: Unable to create GUID!");
            goto END;
        }

        pSessionEventHandler->SetEventFilterAndReset(&guidContextId);
                
        // set the icon path
        METHODCHECKRESULT("IAudioSessionControl::SetIconPath",
                          hr,
                          spAudSesCtrl->SetIconPath(wszIconPaths[i], &guidContextId));
        if(FAILED(hr)) 
        { 
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            goto END; 
        }
                
        if(S_OK == hr)
        {
            hr = WaitForSessionEvent(pSessionEventHandler, 
                                     AUDIO_EVENT_ICONPATH_CHANGED, 
                                     EVENT_TIMEOUT);
            if(FAILED(hr))
            {
                XLOG(XFAIL, eFatalError, "FAIL: timeout waiting for event");
                goto END;
            }    
            
            // Get the GroupingParamChanged event count
            if(1 != pSessionEventHandler->GetIconPathChangedEventCount())
            {
                XLOG(XFAIL, eFatalError, "FAIL: Expected 1 event, observed %d", nReportedEvents);
                goto END;
            }

            hr = pSessionEventHandler->GetLastReportedIconPath(&wszGetPath);
            if(FAILED(hr))
            {
                XLOG(XFAIL, eFatalError, "FAIL: Error getting icon paths");
                goto END;
            }

            // Verify that the correct string was reported
            if(0 == wcscmp(wszIconPaths[i], wszGetPath))
            {
                SLOG(eInfo1, "Event data verified.");
                SAFE_DELETE_ARRAY(wszGetPath);
            }
            else
            {
                XLOG(XFAIL, eError, "FAIL: Event (%ws) and set (%ws) strings do not match!", wszIconPaths[i], wszGetPath);
                SAFE_DELETE_ARRAY(wszGetPath);
            }       
        }
    }    
    //// end scenario
END:
    SAFE_DELETE_ARRAY(wszGetPath);

    if(bSessionEventsSubscribed)
    {
        assert(!(spAudioSessionEvents == NULL));
        spAudSesCtrl->UnregisterAudioSessionNotification(spAudioSessionEvents);            
    }

    SAFE_RELEASE_POINTER(pSessionEventHandler);

    return FNS_PASS; 
}

//------------------------------------------------------------------------------
// Desc: Channel Volume Eventing Scenario
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_PerStreamEvents_Scenario7(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl> spAudSesCtrl = NULL;
    CComPtr<IAudioSessionEvents> spAudioSessionEvents = NULL;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;
    CComPtr<IAudioEndpointVolumeCallback> spEndpointEventHandler = NULL;
    CComPtr<IChannelAudioVolume> spChannelAudioVolume = NULL;
    CComPtr<ISimpleAudioVolume> spSimpleAudioVolume = NULL;

    BOOLEAN bEndpointEventsSubscribed = FALSE;
    BOOLEAN bSessionEventsSubscribed = FALSE;
    
    CAudioSessionEventHandler* pSessionEventHandler = NULL;
    CEndpointVolumeEventHandler* pobjEndpointEventHandler = NULL;
    
    UINT nChannelCount;
    FLOAT* pfLocalVolumes = NULL; // local volume array for verification
    GUID guidContextId = GUID_NULL;
        
    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControl(&spAudSesCtrl);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl Interface");
        goto END;
    }

    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume Interface");
        goto END;
    }

    hr = pStreamingEngine->GetChannelAudioVolume(&spChannelAudioVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IChannelAudioVolume Interface");
        goto END;
    }

    hr = pStreamingEngine->GetSimpleAudioVolume(&spSimpleAudioVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume Interface");
        goto END;
    }

    pSessionEventHandler = new CAudioSessionEventHandler();
    if(NULL == pSessionEventHandler)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }

    METHODCHECKRESULT("CTestAudioSessionEvents::QueryInterface",
                      hr,
                      pSessionEventHandler->QueryInterface(__uuidof(IAudioSessionEvents),
                                                            (void**)&spAudioSessionEvents));
    if(FAILED(hr)) 
    { 
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected result!");
    }

    METHODCHECKRESULT("IAudioSessionControl::RegisterAudioSessionNotification",
                      hr,
                      spAudSesCtrl->RegisterAudioSessionNotification(spAudioSessionEvents));       
    if(FAILED(hr)) 
    {
        XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected result!");
    }
    else
    {
        bSessionEventsSubscribed = TRUE;
    }

    //
    // If this is a capture scenario, subscribe to endpoint volume notifications
    //
    if(DEVICE_TYPE_CAPTURE & dwDeviceType)
    {
        pobjEndpointEventHandler = new CEndpointVolumeEventHandler();
        if(NULL == pobjEndpointEventHandler)
        {
            XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
            goto END;
        }        

        METHODCHECKRESULT("IAudioEndpointVolumeHandler::QueryInterface",
                          hr,
                          pobjEndpointEventHandler->QueryInterface(IID_IAudioEndpointVolumeCallback,
                                                                  (void**)(&spEndpointEventHandler)));

        if(FAILED(hr))
        {
            goto END;
        }

        // register for notifications
        METHODCHECKRESULT("IAudioEndpointVolume::RegisterControlChangeNotify",
                          hr,
                          spEndpointVolume->RegisterControlChangeNotify(spEndpointEventHandler));
        if(FAILED(hr))
        {
            goto END;
        }
        else
        {
            bEndpointEventsSubscribed = TRUE;
        }        
    }

    // Clear any pending events
    pSessionEventHandler->SetEventFilterAndReset(&GUID_EVENT_SCENARIO);

    if(DEVICE_TYPE_CAPTURE & dwDeviceType)
    {
        pobjEndpointEventHandler->Reset();
    }
   
    //// start scenario

    // set all the channel volumes to 0
    METHODCHECKRESULT("IChannelAudioVolume::GetChannelCount",
                      hr,
                      spChannelAudioVolume->GetChannelCount(&nChannelCount));
    if(FAILED(hr)) 
    { 
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END; 
    }                      

    // allocate the local volume array to track volume changes
    pfLocalVolumes = new FLOAT[nChannelCount];
    if(NULL == pfLocalVolumes)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }
    else
    {
        ZeroMemory(pfLocalVolumes, nChannelCount*sizeof(float));
    }


    for(UINT i=0;i<nChannelCount;i++)
    {          
        METHODCHECKRESULT("IChannelAudioVolume::SetChannelVolume",
                          hr,
                          spChannelAudioVolume->SetChannelVolume(i, 1.0f, &GUID_EVENT_SCENARIO));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eFatalError, "FAIL: Error setting channel volume, aborting!");
            goto END;
        }
        else
        {
            // record the change locally
            pfLocalVolumes[i] = 1.0f;     
        }
    }

    if(FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Error while setting channel volumes");
        goto END;
    }

    if(DEVICE_TYPE_CAPTURE & dwDeviceType)
    {
        Sleep(2000);
    }
               
    // modify each channel volume and verify the event data
    for(UINT nChangedChannel=0;nChangedChannel<nChannelCount;nChangedChannel++)
    {
        HRESULT hrTemp = S_OK;
        DWORD dwEventNumChannels = 0;
        DWORD dwEventChangedChannel = 0;
        FLOAT* pfEventChannelVolumes = NULL;
        
        // generate a unique volume for each channel (good up to 20 channels)
        FLOAT fSetVol = ((FLOAT)(nChangedChannel+1))*.05f; 

        // generate a GUID for the event context ID
        METHODCHECKRESULT("::CoCreateGUID",
                          hr,
                          CoCreateGuid(&guidContextId));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eFatalError, "FAIL: Unable to create GUID!");
            goto END;
        }

        // reset the handler
        pSessionEventHandler->SetEventFilterAndReset(&guidContextId);
        if(DEVICE_TYPE_CAPTURE & dwDeviceType)
        {
            pobjEndpointEventHandler->Reset();
        }

        // set volume
        METHODCHECKRESULT("IChannelAudioVolume::SetChannelVolume",
                          hrTemp,
                          spChannelAudioVolume->SetChannelVolume(nChangedChannel, 
                                                                 fSetVol,
                                                                 &guidContextId));
        if(FAILED(hrTemp))
        {
            XLOG(XFAIL, eError, "FAIL: Error setting channel volume, aborting!");
            XLOG(XFAIL, eError, "FAIL: CHAN: %d, VOL: %.2f", nChangedChannel, fSetVol); 
            goto END;
        }
        else
        {
            pfLocalVolumes[nChangedChannel] = fSetVol;
        }

        hr = WaitForSessionEvent(pSessionEventHandler, 
                                 AUDIO_EVENT_CHANNELVOLUME_CHANGED, 
                                 EVENT_TIMEOUT);
        if(FAILED(hr))
        {
            XLOG(XFAIL, eFatalError, "FAIL: timeout waiting for event");
            goto END;
        }    


        //
        // verify that the event came through
        //

        // get the event data
        METHODCHECKRESULT("CAudioSessionEventHandler::GetLastReportedChannelVolume",
                          hrTemp,
                          pSessionEventHandler->GetLastReportedChannelVolume(&dwEventNumChannels,
                                                                             &pfEventChannelVolumes,
                                                                             &dwEventChangedChannel));
        if(FAILED(hrTemp)||(S_FALSE == hrTemp))
        {
            XLOG(XFAIL, eError, "FAIL: Error getting volume data");
            goto END_LOOP;
        }

        // verify the data

        // channel count
        if(nChannelCount != dwEventNumChannels)
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: ERROR: Set (%d) and reported (%d) channel counts do not match!",
                 nChannelCount,
                 dwEventNumChannels);
            goto END_LOOP;                 
        }   
        
        // changed channel
        if(dwEventChangedChannel != nChangedChannel)
        {
            XLOG(XFAIL,
                 eError, 
                 "FAIL: Set (%d) and reported (%d) changed channel do not match!",
                 nChangedChannel,
                 dwEventChangedChannel);
            goto END_LOOP;
        }

        // volume data
        if(nChannelCount == dwEventNumChannels)
        {
            HRESULT hrChannelCheck = S_OK;
            for(UINT i=0;i<nChannelCount;i++)
            {
                if(EPSILON < abs(pfLocalVolumes[i]-pfEventChannelVolumes[i]))
                {
                    hrChannelCheck = E_FAIL;
                    XLOG(XFAIL,
                         eError, 
                         "FAIL: Observed (%f) and expected (%f) volumes for channel %d do not match",
                         pfEventChannelVolumes[i],
                         pfLocalVolumes[i],
                         i);                  
                }
            }

            if(FAILED(hrChannelCheck))
            {
                goto END_LOOP;
            }
        }    

        // in the capture case, verify that an endpoint event was generated, and the
        // volume was correctly propagated to the endpoint
        if(DEVICE_TYPE_CAPTURE & dwDeviceType)
        {
            hrTemp = WaitForEndpointEvent(pobjEndpointEventHandler, EVENT_TIMEOUT);
            UINT32 nEndpointChannelCount = 0;
            FLOAT fCurrSessionMasterVolume = 0.0f;
            FLOAT* pfCurrEndpointVolumes = NULL;
            bool bEndpointControlUniform = false;

            if(FAILED(hrTemp))
            {
                XLOG(XFAIL, eFatalError, "FAIL: Endpoint event not received in response to session volume change");
                goto END;
            }
            
            // Query the endpoint for its current state, and verify that info matches
            // the set values
            METHODCHECKRESULT("IAudioEndpointVolume::GetChannelCount",
                              hr,
                              spEndpointVolume->GetChannelCount(&nEndpointChannelCount));
            if(FAILED(hrTemp))
            {
                goto END_LOOP;
            }
                              
            // the endpoint must have at least as many channels as the session.
            if(nEndpointChannelCount < nChannelCount)
            {
                if (1 == nEndpointChannelCount)
                {
                    SLOG(eInfo1, "INFO: Endpoint volume control appears to be uniform.");
                    bEndpointControlUniform = true;
                }
                else
                {
                    XLOG(XFAIL,
                         eFatalError, 
                         "FAIL: Endpoint Channels (%d) < Session Channels (%d)",
                         nEndpointChannelCount,
                         nChannelCount);
                    hrTemp = E_FAIL;
                    goto END_LOOP;
                }
            }

            // get the current session master volume
            METHODCHECKRESULT("ISimpleAudioVolume::GetMasterVolumeLevel",
                              hrTemp,
                              spSimpleAudioVolume->GetMasterVolume(&fCurrSessionMasterVolume));
            if(FAILED(hrTemp))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
                goto END_LOOP;
            }
            
            pfCurrEndpointVolumes = new FLOAT[nEndpointChannelCount];

            for(UINT32 i=0;i<nEndpointChannelCount;i++)
            {
                METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevelScalar",
                                  hrTemp,
                                  spEndpointVolume->GetChannelVolumeLevelScalar(i, &pfCurrEndpointVolumes[i]));
                if(FAILED(hrTemp)) 
                {
                    XLOG(XFAIL, eError, "FAIL: IAudioEndpointVolume::GetChannelVolumeLevelScalar returned 0x%08x", hr);
                    goto END_ENDPOINT_VERIFICATION;
                }                                               
            }
            
            // verify the channel volumes
            // 
            // There are two possiblities here:
            //
            // 1) The channel was correctly forwarded to the endpoint
            //
            // 2) The channel volume was forwarded to the endpoint, however,
            //    the endpoint has a uniform volume control.  In this case
            //    we need to verify that the endpoint volume == channel 0
            //    of the session volume.
            
            // print out the channel volumes for debugging/tracing purposes
            for(UINT32 i=0;i<nEndpointChannelCount;i++)
            {
                SLOG(eInfo1, 
                     "Endpoint Volume Channel %d: [ %f ]",
                     i,
                     pfCurrEndpointVolumes[i]);
            }

            if (false == bEndpointControlUniform)
            {
                // standard volume control

                if(EPSILON < abs(pfEventChannelVolumes[nChangedChannel] * fCurrSessionMasterVolume-pfCurrEndpointVolumes[nChangedChannel]))
                {
                    XLOG(XFAIL,
                         eInfo1, 
                         "FAIL: Session [%f] and Endpoint [%f] volumes do not match for channel %d", 
                         pfEventChannelVolumes[nChangedChannel] * fCurrSessionMasterVolume, 
                         pfCurrEndpointVolumes[nChangedChannel],
                         nChangedChannel);
                }
            }
            else
            {
                // uniform volume control
                // verify that the endpoint volume == session channel 0.

                if (EPSILON < abs(pfEventChannelVolumes[0] * fCurrSessionMasterVolume - pfCurrEndpointVolumes[0]))
                {
                    XLOG(XFAIL,
                         eInfo1,
                         "FAIL: Session Channel 0 [%f] and Endpoint [%f] volumes do not match for uniform endpoint volume control",
                         pfEventChannelVolumes[0] * fCurrSessionMasterVolume,
                         pfCurrEndpointVolumes[0]);
                }
            }

END_ENDPOINT_VERIFICATION:
            SAFE_DELETE_ARRAY(pfCurrEndpointVolumes);     
        }
        
END_LOOP:
        SAFE_DELETE_ARRAY(pfEventChannelVolumes);
        if(FAILED(hrTemp))
        {
            goto END;
        }
    }     
  
    //// end scenario 
END:
    if(bEndpointEventsSubscribed)
    {
        assert(!(spEndpointEventHandler == NULL));
        spEndpointVolume->UnregisterControlChangeNotify(spEndpointEventHandler);
    }

    if(bSessionEventsSubscribed)
    {
        assert(!(spAudioSessionEvents == NULL));
        spAudSesCtrl->UnregisterAudioSessionNotification(spAudioSessionEvents);            
    }

    SAFE_RELEASE_POINTER(pobjEndpointEventHandler);
    SAFE_RELEASE_POINTER(pSessionEventHandler);

    return FNS_PASS;    
}

//------------------------------------------------------------------------------
// Desc: More complex channel volume eventing scenario
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_PerStreamEvents_Scenario8(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl> spAudSesCtrl = NULL;
    CComPtr<IAudioSessionEvents> spAudioSessionEvents = NULL;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;
    CComPtr<IAudioEndpointVolumeCallback> spEndpointEventHandler = NULL;
    CComPtr<IChannelAudioVolume> spChannelAudioVolume = NULL;
    CComPtr<ISimpleAudioVolume> spSimpleAudioVolume = NULL;

    BOOLEAN bEndpointEventsSubscribed = FALSE;
    BOOLEAN bSessionEventsSubscribed = FALSE;
    
    CAudioSessionEventHandler* pSessionEventHandler = NULL;
    CEndpointVolumeEventHandler* pobjEndpointEventHandler = NULL;
    
    UINT nChannelCount;
    FLOAT* pfLocalVolumes = NULL; // local volume array for verification
    FLOAT fSetVol = 0.0f;

    GUID guidContextId = GUID_NULL; 
        
    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControl(&spAudSesCtrl);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl Interface");
        goto END;
    }

    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume Interface");
        goto END;
    }

    hr = pStreamingEngine->GetChannelAudioVolume(&spChannelAudioVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IChannelAudioVolume Interface");
        goto END;
    }

    hr = pStreamingEngine->GetSimpleAudioVolume(&spSimpleAudioVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume Interface");
        goto END;
    }

    pSessionEventHandler = new CAudioSessionEventHandler();
    if(NULL == pSessionEventHandler)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }

    METHODCHECKRESULT("CTestAudioSessionEvents::QueryInterface",
                      hr,
                      pSessionEventHandler->QueryInterface(__uuidof(IAudioSessionEvents),
                                                            (void**)&spAudioSessionEvents));
    if(FAILED(hr)) 
    { 
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected result!");
    }

    METHODCHECKRESULT("IAudioSessionControl::RegisterAudioSessionNotification",
                      hr,
                      spAudSesCtrl->RegisterAudioSessionNotification(spAudioSessionEvents));       
    if(FAILED(hr)) 
    {
        XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected result!");
    }
    else
    {
        bSessionEventsSubscribed = TRUE;
    }

    //
    // If this is a capture scenario, subscribe to endpoint volume notifications
    //
    if(DEVICE_TYPE_CAPTURE & dwDeviceType)
    {
        pobjEndpointEventHandler = new CEndpointVolumeEventHandler();
        if(NULL == pobjEndpointEventHandler)
        {
            XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
            goto END;
        }        

        METHODCHECKRESULT("IAudioEndpointVolumeHandler::QueryInterface",
                          hr,
                          pobjEndpointEventHandler->QueryInterface(IID_IAudioEndpointVolumeCallback,
                                                                  (void**)(&spEndpointEventHandler)));

        if(FAILED(hr))
        {
            goto END;
        }

        // register for notifications
        METHODCHECKRESULT("IAudioEndpointVolume::RegisterControlChangeNotify",
                          hr,
                          spEndpointVolume->RegisterControlChangeNotify(spEndpointEventHandler));
        if(FAILED(hr))
        {
            goto END;
        }
        else
        {
            bEndpointEventsSubscribed = TRUE;
        }        
    }

    // Clear any pending events
    pSessionEventHandler->SetEventFilterAndReset(&GUID_EVENT_SCENARIO);

    if(DEVICE_TYPE_CAPTURE & dwDeviceType)
    {
        pobjEndpointEventHandler->Reset();
    }
   
    //// start scenario

    // initialize channel volumes
    // set even channels to 1.0, odd channels to 0.0

    METHODCHECKRESULT("IChannelAudioVolume::GetChannelCount",
                      hr,
                      spChannelAudioVolume->GetChannelCount(&nChannelCount));
    if(FAILED(hr)) { goto END; }                      

    // allocate the local volume array to track volume changes
    pfLocalVolumes = new FLOAT[nChannelCount];
    if(NULL == pfLocalVolumes)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }
    else
    {
        ZeroMemory(pfLocalVolumes, nChannelCount*sizeof(float));
    }


    for(UINT i=0;i<nChannelCount;i++)
    {        
        // even channels to 0.9, odds to 0.1
        if(0 == (i%2))
        {
            fSetVol = 0.9f;
        }
        else
        {
            fSetVol = 0.1f;
        }

        METHODCHECKRESULT("IChannelAudioVolume::SetChannelVolume",
                          hr,
                          spChannelAudioVolume->SetChannelVolume(i, fSetVol, &GUID_EVENT_SCENARIO));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eFatalError, "FAIL: Error setting channel volume, aborting!");
            goto END;
        }
        else
        {
            // record the change locally
            pfLocalVolumes[i] = fSetVol;     
        }
    }

    if(FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Error while setting channel volumes");
        goto END;
    }

    if(DEVICE_TYPE_CAPTURE & dwDeviceType)
    {
        Sleep(2000);
    }
      
    // everything up to here is basically initialization
    // the actual volume eventing testing starts here
    

    // fade even channels from 1.0 -> 0.0 -> 1.0
    // fade odd channels from 0.0 -> 1.0 -> 0.0
    FLOAT afEvenVolumes[] = {1.0f, 0.8f, 0.6f, 0.4f, 0.2f, 0.0f, 0.2f, 0.4f, 0.6f, 0.8f, 1.0f};
    FLOAT afOddVolumes[] =  {0.0f, 0.2f, 0.4f, 0.6f, 0.8f, 1.0f, 0.8f, 0.6f, 0.4f, 0.2f, 0.0f};
    static_assert(sizeof(afEvenVolumes) == sizeof(afOddVolumes), "afEvenVolumes and afOddVolumes are of different sizes");

    for(UINT cVolumeIndex=0;cVolumeIndex < ARRAYSIZE(afEvenVolumes);cVolumeIndex++)
    {
        for(UINT nChannel=0;nChannel<nChannelCount;nChannel++)
        {
            HRESULT hrTemp = S_OK;
            DWORD dwEventNumChannels = 0;
            DWORD dwEventChangedChannel = 0;
            FLOAT* pfEventChannelVolumes = NULL;
            FLOAT fSetVol = 0.0f;

            // generate a GUID for the event context ID
            METHODCHECKRESULT("::CoCreateGUID",
                              hrTemp,
                              CoCreateGuid(&guidContextId));
            if(FAILED(hrTemp))
            {
                XLOG(XFAIL, eFatalError, "FAIL: Unable to create GUID!");
                goto END;
            }

            if (0 == (nChannel%2))
            {
                fSetVol = afEvenVolumes[cVolumeIndex];
            }
            else
            {
                fSetVol = afOddVolumes[cVolumeIndex];
            }
            
            pSessionEventHandler->SetEventFilterAndReset(&guidContextId);
            if(DEVICE_TYPE_CAPTURE & dwDeviceType)
            {
                pobjEndpointEventHandler->Reset();
            }

            SLOG(eInfo1, 
                 "**********Setting Session Volume [Chan. %d => %f]",
                 nChannel,
                 fSetVol);
            
            // set volume
            METHODCHECKRESULT("IChannelAudioVolume::SetChannelVolume",
                              hrTemp,
                              spChannelAudioVolume->SetChannelVolume(nChannel, 
                                                                     fSetVol,
                                                                     &guidContextId));
            if(FAILED(hrTemp))
            {
                XLOG(XFAIL, eError, "FAIL: Error setting channel volume, aborting!");
                XLOG(XFAIL, eError, "FAIL: CHAN: %d, VOL: %.2f", nChannel, fSetVol); 
                goto END;
            }
            else
            {
                pfLocalVolumes[nChannel] = fSetVol;
            }

            hr = WaitForSessionEvent(pSessionEventHandler, 
                                     AUDIO_EVENT_CHANNELVOLUME_CHANGED, 
                                     EVENT_TIMEOUT);
            if(FAILED(hr))
            {
                XLOG(XFAIL, eFatalError, "FAIL: timeout waiting for event");
                goto END;
            }    


            // get the event data
            METHODCHECKRESULT("CAudioSessionEventHandler::GetLastReportedChannelVolume",
                              hrTemp,
                              pSessionEventHandler->GetLastReportedChannelVolume(&dwEventNumChannels,
                                                                                 &pfEventChannelVolumes,
                                                                                 &dwEventChangedChannel));
            if(FAILED(hrTemp)||(S_FALSE == hrTemp))
            {
                XLOG(XFAIL, eError, "FAIL: timeout while waiting for event data");
                goto END_LOOP;
            }

            // verify the data

            // channel count
            if(nChannelCount != dwEventNumChannels)
            {
                hrTemp = E_FAIL;
                XLOG(XFAIL,
                     eError,
                     "FAIL: Set (%d) and reported (%d) channel counts do not match!",
                     nChannelCount,
                     dwEventNumChannels); 
                goto END_LOOP;                     
            }   
            
            // changed channel
            if(dwEventChangedChannel != nChannel)
            {
                hrTemp = E_FAIL;
                XLOG(XFAIL,
                     eError, 
                     "FAIL:: Set (%d) and reported (%d) changed channel do not match!",
                     nChannel,
                     dwEventChangedChannel);
                goto END_LOOP;                     
            }

            // volume data
            if(nChannelCount == dwEventNumChannels)
            {
                HRESULT hrChannelCheck = S_OK;
                
                for(UINT i=0;i<nChannelCount;i++)
                {
                    if(EPSILON < abs(pfLocalVolumes[i]-pfEventChannelVolumes[i]))
                    {
                        hrChannelCheck = E_FAIL;
                        XLOG(XFAIL,
                             eError, 
                             "FAIL: Observed (%f) and expected (%f) volumes for channel %d do not match",
                             pfEventChannelVolumes[i],
                             pfLocalVolumes[i],
                             i);
                    }
                }

                if(FAILED(hrChannelCheck))
                {
                    hrTemp = E_FAIL;
                    goto END_LOOP;
                }
            }   

            // in the capture case, verify that an endpoint event was generated, and the
            // volume was correctly propagated to the endpoint
            if(DEVICE_TYPE_CAPTURE & dwDeviceType)
            {
                hrTemp = WaitForEndpointEvent(pobjEndpointEventHandler, EVENT_TIMEOUT);
                if(FAILED(hrTemp))
                {
                    XLOG(XFAIL, eError, "FAIL: timeout waiting for endpoint event!");
                    goto END_LOOP;
                }

                UINT32 nEndpointChannelCount = 0;
                FLOAT fCurrSessionMasterVolume = 0.0f;
                FLOAT* pfCurrEndpointVolumes = NULL;
                bool bEndpointControlUniform = false;

                // Query the endpoint for its current state, and verify that info matches
                // the set values
                METHODCHECKRESULT("IAudioEndpointVolume::GetChannelCount",
                                  hr,
                                  spEndpointVolume->GetChannelCount(&nEndpointChannelCount));
                if(FAILED(hrTemp))
                {
                    XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
                    goto END_LOOP;
                }
                                  
                // match endpoint and session channel counts
                if(nEndpointChannelCount < nChannelCount)
                {
                    if (1 == nEndpointChannelCount)
                    {
                        SLOG(eInfo1, "INFO: Endpoint volume control appears to be uniform.");
                        bEndpointControlUniform = true;
                    }
                    else
                    {
                        XLOG(XFAIL,
                             eFatalError, 
                             "FAIL: Endpoint Channels (%d) < Session Channels (%d)",
                             nEndpointChannelCount,
                             nChannelCount);
                        hrTemp = E_FAIL;
                        goto END_LOOP;
                    }
                }

                // get the current session master volume
                METHODCHECKRESULT("ISimpleAudioVolume::GetMasterVolumeLevel",
                                  hrTemp,
                                  spSimpleAudioVolume->GetMasterVolume(&fCurrSessionMasterVolume));
                if(FAILED(hrTemp))
                {
                    goto END_LOOP;
                }
                
                pfCurrEndpointVolumes = new FLOAT[nEndpointChannelCount];

                for(UINT32 i=0;i<nEndpointChannelCount;i++)
                {
                    METHODCHECKRESULT("IAudioEndpoint::GetChannelVolumeLevelScalar",
                                      hrTemp,
                                      spEndpointVolume->GetChannelVolumeLevelScalar(i, &pfCurrEndpointVolumes[i]));
                    if(FAILED(hrTemp)) 
                    {
                        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
                        goto END_ENDPOINT_VERIFICATION;
                    }                                               
                }
                
            // verify the channel volumes
            // 
            // There are two possiblities here:
            //
            // 1) The channel was correctly forwarded to the endpoint
            //
            // 2) The channel volume was forwarded to the endpoint, however,
            //    the endpoint has a uniform volume control.  In this case
            //    we need to verify that the endpoint volume == channel 0
            //    of the session volume.
            
            // print out the channel volumes for debugging/tracing purposes
            for(UINT32 i=0;i<nEndpointChannelCount;i++)
            {
                SLOG(eInfo1, 
                     "Endpoint Volume Channel %d: [ %f ]",
                     i,
                     pfCurrEndpointVolumes[i]);
            }

            if (false == bEndpointControlUniform)
            {
                // standard volume control

                if(EPSILON < abs(pfEventChannelVolumes[nChannel] * fCurrSessionMasterVolume-pfCurrEndpointVolumes[nChannel]))
                {
                    XLOG(XFAIL,
                         eInfo1, 
                         "FAIL: Session [%f] and Endpoint [%f] volumes do not match for channel %d", 
                         pfEventChannelVolumes[nChannel] * fCurrSessionMasterVolume, 
                         pfCurrEndpointVolumes[nChannel],
                         nChannel);
                }
            }
            else
            {
                // uniform volume control
                // verify that the endpoint volume == session channel 0.

                if (EPSILON < abs(pfEventChannelVolumes[0] * fCurrSessionMasterVolume - pfCurrEndpointVolumes[0]))
                {
                    XLOG(XFAIL,
                         eInfo1,
                         "FAIL: Session Channel 0 [%f] and Endpoint [%f] volumes do not match for uniform endpoint volume control",
                         pfEventChannelVolumes[0] * fCurrSessionMasterVolume,
                         pfCurrEndpointVolumes[0]);
                }
            }

END_ENDPOINT_VERIFICATION:
            SAFE_DELETE_ARRAY(pfCurrEndpointVolumes); 
            }   
                          
END_LOOP:    
            SAFE_DELETE_ARRAY(pfEventChannelVolumes);
            if(FAILED(hrTemp))
            {
                goto END;
            }
        }
    }    

    

    //// end scenario 
END:
    if(bEndpointEventsSubscribed)
    {
        assert(!(spEndpointEventHandler == NULL));
        spEndpointVolume->UnregisterControlChangeNotify(spEndpointEventHandler);
    }

    if(bSessionEventsSubscribed)
    {
        assert(!(spAudioSessionEvents == NULL));
        spAudSesCtrl->UnregisterAudioSessionNotification(spAudioSessionEvents);            
    }

    SAFE_RELEASE_POINTER(pobjEndpointEventHandler);
    SAFE_RELEASE_POINTER(pSessionEventHandler);

    return FNS_PASS;   
}

//------------------------------------------------------------------------------
// Desc: Verify that the appropriate OnSessionDisconnected event is dispatched
//       when the device format is changed.
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_PerStreamEvents_Scenario9(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT                             hr = S_OK;
    CComPtr<IAudioSessionControl>       spAudSesCtrl = NULL;
    CComPtr<IAudioSessionEvents>        spAudSesEvents = NULL;
    CComPtr<IAudioClient>               spAudClient = NULL;
    CComPtr<IPolicyConfig>              spPolicyConfig = NULL;
    CComPtr<IWaveFormatEnum>            spFormatEnumerator = NULL;
    BOOLEAN                             bSessionEventsSubscribed = FALSE;
    LPCWSTR                             pwszEndpointUnderTest;
    CComHeapPtr<WAVEFORMATEX>           sp_wfxCurrentDeviceFormat;
    PWAVEFORMATEX                       pwfxNewDeviceFormat = NULL;
    CAudioSessionEventHandler*          pSessionEventHandler = NULL;
    bool                                bDeviceFormatChanged = false;
            
    assert(NULL != pStreamingEngine);

    pwszEndpointUnderTest = GetEndpointIDUnderTest();
    if(NULL == pwszEndpointUnderTest)
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain current endpoint ID");
        goto END;
    }

    hr = pStreamingEngine->GetAudioClient(&spAudClient);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl Interface (0x%08X)", hr);
        goto END;
    }
    
    hr = pStreamingEngine->GetSessionControl(&spAudSesCtrl);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl Interface (0x%08X)", hr);
        goto END;
    }

    hr = spPolicyConfig.CoCreateInstance(__uuidof(PolicyConfig));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to CoCreate IPolicyConfig Interface (0x%08X)", hr);
        goto END;
    }

    // Get the current device format
    hr = spPolicyConfig->GetDeviceFormat(pwszEndpointUnderTest,
                                         FALSE,
                                         &sp_wfxCurrentDeviceFormat);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: IPolicyConfig::GetDeviceFormat returned 0x%08X", hr);
        return hr;
    }

    //
    // Get a supported streaming format different from the current one
    //

    SLOG(eInfo1, "Searching for a format supported by the endpoint under test...");    

    // Enumerate a bunch o' PCM formats and find one that
    //  1) Is supported by the endpoint
    //  2) Is different from the current format

    hr = CreateWaveFormatEnum(&spFormatEnumerator);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: CreateWaveFormatEnum returned 0x%08X", hr);
        goto END;
    }

    // Confine our search to 8 or 16 bit mono and stereo formats.
    hr = spFormatEnumerator->AddPCMFormats((IWaveFormatEnum::WF_BIT_8)|(IWaveFormatEnum::WF_BIT_16),
                                           IWaveFormatEnum::WF_SAMPLE_ALL,
                                           (IWaveFormatEnum::WF_CHANNEL_1)|(IWaveFormatEnum::WF_CHANNEL_2));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: IWaveFormatEnum::AddPCMFormats returned 0x%08X", hr);
        goto END;
    }

    SLOG(eInfo1, "Enumerated %d possible formats", spFormatEnumerator->GetCount());

    pwfxNewDeviceFormat = NULL;

    for(UINT i=0; i<spFormatEnumerator->GetCount(); i++)
    {
        PWAVEFORMATEX t_wfxCurrFormat = NULL;

        // SLOG(eBlab1, 
        //      "Testing format:\n%s", 
        //      (LPCSTR)(spFormatEnumerator->GetStringAt(i, (CHAR**)(NULL))));

        t_wfxCurrFormat = spFormatEnumerator->GetAt(i);

        // skip the format if it's identical to the current device format
        if(IsSimilarWaveFormat(t_wfxCurrFormat, sp_wfxCurrentDeviceFormat))
        {
            continue;
        }

        // See if the format is supported by the endpoint

        // ISSUE: 19 March 2008 NiBenson - 
        // We don't need to manage the memory pointed to by t_wfxCurrFormat.  It will
        // live the as long as the IWaveFormatEnum interface.  This design should probably
        // be changed in WaveFormatEnum
        hr = spAudClient->IsFormatSupported(AUDCLNT_SHAREMODE_EXCLUSIVE,
                                            t_wfxCurrFormat,
                                            NULL);
        if(S_OK == hr)
        {
            SLOG(eInfo1,
                 "Found New Format:\n%s", (LPCSTR)(spFormatEnumerator->GetStringAt(i, (CHAR**)(NULL)))); 
            pwfxNewDeviceFormat = t_wfxCurrFormat;
            break;
        }

    }

    // We couldn't find a format that met our criteria.
    if(NULL == pwfxNewDeviceFormat)
    {
        XLOG(XFAIL, eError, "FAIL: Unable to find a second supported streaming format for the endpoint under test!");
        goto END;
    }

    //
    // Set up event handling
    //

    SLOG(eInfo1, "Subscribing to session notifications...");
    
    pSessionEventHandler = new CAudioSessionEventHandler();
    if(NULL == pSessionEventHandler)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }


    spAudSesEvents.Attach((IAudioSessionEvents*)(pSessionEventHandler));
    
    METHODCHECKRESULT("IAudioSessionControl::RegisterAudioSessionNotification",
                      hr,
                      spAudSesCtrl->RegisterAudioSessionNotification(spAudSesEvents));       
    if(FAILED(hr)) 
    {
        XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected result!");
    }
    else
    {
        bSessionEventsSubscribed = TRUE;
    }

    // Clear any pending events
    pSessionEventHandler->SetEventFilterAndReset(&GUID_EVENT_SCENARIO);
   
    //// start scenario

    // set the default format    
    hr = spPolicyConfig->SetDeviceFormat(pwszEndpointUnderTest,
                                         pwfxNewDeviceFormat,
                                         NULL);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: IPolicyConfig::SetDeviceFormat returned 0x%08X", hr);
        goto END;
    }
    bDeviceFormatChanged = true;

    // verify that a session disconnected event was received
    hr = WaitForSessionEvent(pSessionEventHandler, 
                             AUDIO_EVENT_SESSION_DISCONNECTED, 
                             EVENT_TIMEOUT);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: timeout waiting for session disconnection event");
        goto END;
    }    

    // event count    
    UINT32 nEvents = pSessionEventHandler->GetSessionDisconnectedEventCount();
    if(1 != nEvents)
    {
        XLOG(XFAIL, 
             eError, "FAIL: Expected %d SessionDisconnection events, encountered %d",
             1,
             nEvents);
    }

    // reason
    if(0 != nEvents)
    {
        AudioSessionDisconnectReason Reason;
        hr = pSessionEventHandler->GetLastReportedDisconnection(&Reason);
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: CSessionEventHandler::GetLastReportedDisconnection returned 0x%08X", hr);
            goto END;
        }

        if(DisconnectReasonFormatChanged != Reason)
        {
            XLOG(XFAIL, 
                 eError, 
                 "FAIL: Expected session disconnection reason 'DisconnectionReasonFormatChanged', encountered '%s' (%d)",
                 StringFromDisconnectReason(Reason),
                 Reason);
        }
    }
    
    //// end scenario 
END:
    if(bSessionEventsSubscribed)
    {
        assert(!(spAudSesCtrl == NULL));
        hr = spAudSesCtrl->UnregisterAudioSessionNotification(spAudSesEvents);
        if(FAILED(hr))
        {
            XLOG(XFAIL, 
                 eError, 
                 "FAIL: IAudioSessionControl::UnregisterAudioSessionNotification failed: hr = 0x%08x",
                 hr);
        }
    }
    
    if(bDeviceFormatChanged)
    {
        hr = spPolicyConfig->SetDeviceFormat(pwszEndpointUnderTest,
                                             sp_wfxCurrentDeviceFormat,
                                             NULL);
        if(FAILED(hr))
        {
            XLOG(XFAIL, 
                 eError, 
                 "FAIL: Restoring the device format failed: hr = 0x%08x",
                 hr);
        }
    }

    return FNS_PASS;   
}

