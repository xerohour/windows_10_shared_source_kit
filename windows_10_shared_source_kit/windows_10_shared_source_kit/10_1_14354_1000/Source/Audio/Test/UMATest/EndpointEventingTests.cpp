// Copyright(C) Microsoft. All rights reserved.

//
// EndpointEventingTests.cpp
//  
//
// Description:
//
//  WASAPI IAudioEndpointVolume Eventing Scenarios

#include <stdafx.h>
#include <UMATestStreaming.h>

#include "TestUtilities.h"

#include "StreamingControlTestFramework.h"
#include "EndpointVolumeEventHandler.h"

const UINT32 EVENT_TIMEOUT = 10000; // timout after 10s

// GUID used for Endpoitn Volume API Test Contexts
// {A0662494-BC26-43e3-A5B4-268B721A79CE}
static const GUID GUID_ENDPOINTVOLUME_TEST = 
{ 0xa0662494, 0xbc26, 0x43e3, { 0xa5, 0xb4, 0x26, 0x8b, 0x72, 0x1a, 0x79, 0xce } };

const static float DB_TOLER = .0001f;
const static float SCALAR_TOLER = .1f;

const static float MIN_SCALAR_ATTEN = 0.0f;
const static float MAX_SCALAR_ATTEN = 1.0f;

DWORD TC_PerEndpointEvents_Scenario1()
{
    RunStreamingControlTest(PERENDPOINTEVENTS_SCENARIO1);
    return FNS_PASS;
}

DWORD TC_PerEndpointEvents_Scenario2()
{
    RunStreamingControlTest(PERENDPOINTEVENTS_SCENARIO2);
    return FNS_PASS;
}

DWORD TC_PerEndpointEvents_Scenario3()
{
    RunStreamingControlTest(PERENDPOINTEVENTS_SCENARIO3);
    return FNS_PASS;
}

DWORD TC_PerEndpointEvents_ScenarioIV1()
{
    RunStreamingControlTest(PERENDPOINTEVENTS_SCENARIOIV1);
    return FNS_PASS;
}

//-------------------------------------------------------------------------
// Description:
//  Test master volume eventing notification registration
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_PerEndpointEvents_Scenario1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;
    CComPtr<IAudioEndpointVolumeCallback> spEndpointCallback = NULL;
    CEndpointVolumeEventHandler* pobjEndpointEventHandler = NULL;

    if(NULL == (pobjEndpointEventHandler = new CEndpointVolumeEventHandler()))        
    {
        XLOG(XFAIL, eError, "FAIL: Out of memory!");
        goto END;
    }

    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    hr = pobjEndpointEventHandler->QueryInterface(IID_IAudioEndpointVolumeCallback,
                                                  (void**)&spEndpointCallback);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain endpoint volume event handler interface");
        goto END;
    }

    // Begin Scenario

    // register for notifications
    METHODCHECKRESULT("IAudioEndpointVolume::RegisterControlChangeNotify",
                      hr,                      
                      spEndpointVolume->RegisterControlChangeNotify(spEndpointCallback));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    // un-register for notifications
    METHODCHECKRESULT("IAudioEndpointVolume::RegisterControlChangeNotify",
                      hr,                      
                      spEndpointVolume->UnregisterControlChangeNotify(spEndpointCallback));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

END:
    SAFE_RELEASE_POINTER(pobjEndpointEventHandler);
    return FNS_PASS;
}   

DWORD Test_PerEndpointEvents_Scenario2(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;
    CComPtr<IAudioEndpointVolumeCallback> spEndpointCallback = NULL;
    CEndpointVolumeEventHandler* pobjEndpointEventHandler = NULL;

    if(NULL == (pobjEndpointEventHandler = new CEndpointVolumeEventHandler()))        
    {
        XLOG(XFAIL, eError, "FAIL: Out of memory!");
        goto END;
    }

    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    hr = pobjEndpointEventHandler->QueryInterface(IID_IAudioEndpointVolumeCallback,
                                                  (void**)&spEndpointCallback);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain endpoint volume event handler interface");
        goto END;
    }

    // set the volume to a baseline
    METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevelScalar",
                      hr,
                      spEndpointVolume->SetMasterVolumeLevelScalar(0.5f, &GUID_ENDPOINTVOLUME_TEST));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Unable to set master volume to 0.0!");
        goto END;
    }

    // register for notifications
    METHODCHECKRESULT("IAudioEndpointVolume::RegisterControlChangeNotify",
                      hr,                      
                      spEndpointVolume->RegisterControlChangeNotify(spEndpointCallback));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }
    
    // un-register for notifications
    METHODCHECKRESULT("IAudioEndpointVolume::RegisterControlChangeNotify",
                      hr,                      
                      spEndpointVolume->UnregisterControlChangeNotify(spEndpointCallback));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    Sleep(750);

    // reset notification flag
    pobjEndpointEventHandler->Reset();
   
    // change the master volume to generate an event
    METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevelScalar",
                      hr,                      
                      spEndpointVolume->SetMasterVolumeLevelScalar(1.0f, &GUID_ENDPOINTVOLUME_TEST));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to set endpoint master volume level!");
        goto END;                   
    }

    // verify that no events have been received
    if(pobjEndpointEventHandler->CheckEventReceived())
    {
        XLOG(XFAIL, eError, "FAIL: Event should not have been received!");
    }

END:
    SAFE_RELEASE_POINTER(pobjEndpointEventHandler);
    return FNS_PASS;
}   

//-------------------------------------------------------------------------
// Description:
//  Test various endpoint volume eventing scenarios
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_PerEndpointEvents_Scenario3(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;
    CComPtr<IAudioEndpointVolumeCallback> spEndpointCallback = NULL;
    CEndpointVolumeEventHandler* pobjEndpointEventHandler = NULL;


    GUID guidEventContext = GUID_NULL;
    FLOAT fMinDbVol = 0.0f, fMaxDbVol = 0.0f, fVolInc = 0.0f;        
    UINT32 nChannels = 0;

    if(NULL == (pobjEndpointEventHandler = new CEndpointVolumeEventHandler()))        
    {
        XLOG(XFAIL, eError, "FAIL: Out of memory!");
        goto END;
    }

    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    hr = pobjEndpointEventHandler->QueryInterface(IID_IAudioEndpointVolumeCallback,
                                                  (void**)&spEndpointCallback);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain endpoint volume event handler interface");
        goto END;
    }

    // register for notifications
    METHODCHECKRESULT("IAudioEndpointVolume::RegisterControlChangeNotify",
                      hr,                      
                      spEndpointVolume->RegisterControlChangeNotify(spEndpointCallback));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    // get the volume range on the endpoint under test
    METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeRange",
                      hr,
                      spEndpointVolume->GetVolumeRange(&fMinDbVol, &fMaxDbVol, &fVolInc));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto UNREGISTER;
    }    

    //
    // Begin Scenario
    //

    // initially set the volume to the min since we start testing using the max and we want to see an event.
    METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevel(MIN_VOLUME)",
                      hr,
                      spEndpointVolume->SetMasterVolumeLevel(fMinDbVol, NULL));
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Unable to set the endpoint volume to the minimum of %f dB (0x%8x)",
             fMinDbVol,
             hr);
        goto UNREGISTER;
    }

    Sleep(1000);

    // ramp the volume using SetMasterVolumeLevel, verifying events in the mean time
    for(float fSetVolDB=fMaxDbVol;fSetVolDB>=fMinDbVol;fSetVolDB-=5.0f)
    {                
        // create a GUID for event context tracking
        METHODCHECKRESULT("::CoCreateGuid",
                          hr,
                          ::CoCreateGuid(&guidEventContext))
        if(FAILED(hr))            
        {
            XLOG(XFAIL, eError, "FAIL: Unable to generate GUID");
            goto UNREGISTER;
        }
        pobjEndpointEventHandler->SetEventContextFilterAndReset(&guidEventContext);
        
        SLOG(eInfo1, "SetMasterVolumeLevel -> %f dB", fSetVolDB);

        METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevel",
                          hr,                          
                          spEndpointVolume->SetMasterVolumeLevel(fSetVolDB, &guidEventContext));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: unable to set master endpoint volume with atten: %f", fSetVolDB);
            goto UNREGISTER;
        }

        if(S_FALSE != hr)
        {
            // wait for the event to fire
            METHODCHECKRESULT("WaitForEndpointEvent",
                              hr,
                              WaitForEndpointEvent(pobjEndpointEventHandler,
                                                   EVENT_TIMEOUT));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: Expected endpoint event not received");
                goto UNREGISTER;
            }

            // check to see if the event has fired
            if(pobjEndpointEventHandler->CheckEventReceived())
            {
                AUDIO_VOLUME_NOTIFICATION_DATA data = pobjEndpointEventHandler->GetLastVolumeData();
                FLOAT fSetScalarAtten = 0.0f;

                // verify the data
                //
                // Note: Since volume levels are always reported as
                // as wiper values, it's necessary to convert
                // the dB value used to set the endpoint volume

                fSetScalarAtten = ConvertDbToWiper(fSetVolDB, 
                                                   fMinDbVol,
                                                   fMaxDbVol,
                                                   dwDeviceType);

                SLOG(eInfo1, 
                     "Event volume receieved: %f, expected: %f <%f dB>)",
                     data.fMasterVolume,
                     fSetScalarAtten,
                     fSetVolDB);


                // check volume data
                if(SCALAR_TOLER < abs(fSetScalarAtten - data.fMasterVolume))
                {
                    XLOG(XFAIL,
                         eError, 
                         "FAIL: Set (%f) and event (%f) wiper values do not match!",
                         fSetScalarAtten,
                         data.fMasterVolume);
                }

            }
            else
            {
                XLOG(XFAIL, eFatalError, "FAIL: Volume Event Not Receieved"); 
            }
        }
    }         


    // set the master volume scalar to establish a baseline
    METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevelScalar(MAX_SCALAR_ATTEN)",
                      hr,                          
                      spEndpointVolume->SetMasterVolumeLevelScalar(MAX_SCALAR_ATTEN,&GUID_ENDPOINTVOLUME_TEST));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to set endpoint master volue level");
        goto UNREGISTER;
    }   

    Sleep(1000);

    // ramp the volume using SetMasterVolumeLevelScalar, verifying events in the mean time
    for(float fSetVol=MIN_SCALAR_ATTEN;fSetVol<MAX_SCALAR_ATTEN;fSetVol+=.05f)
    {
        // create a GUID for event context tracking
        METHODCHECKRESULT("::CoCreateGuid",
                          hr,
                          ::CoCreateGuid(&guidEventContext));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Unable to create GUID");
        }

        pobjEndpointEventHandler->SetEventContextFilterAndReset(&guidEventContext);
        METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevelScalar",
                          hr,                          
                          spEndpointVolume->SetMasterVolumeLevelScalar(fSetVol,&guidEventContext));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Failed to set master endpoint volume with atten: %f", fSetVol);
            goto UNREGISTER;
        }

        if(S_FALSE != hr)
        {
            // wait for the event to fire
            METHODCHECKRESULT("WaitForEndpointEvent",
                              hr,
                              WaitForEndpointEvent(pobjEndpointEventHandler,
                                                   EVENT_TIMEOUT));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: Expected endpoint event not received");                
                goto UNREGISTER;
            }    

            // check to see if the event has fired
            if(pobjEndpointEventHandler->CheckEventReceived())
            {
                AUDIO_VOLUME_NOTIFICATION_DATA data = pobjEndpointEventHandler->GetLastVolumeData();
                // verify the data
                if(((data.fMasterVolume)-fSetVol)>DB_TOLER)
                {
                    XLOG(XFAIL,
                         eError, 
                         "FAIL: Event data is incorrect (expected: %f, received: %f",
                         fSetVol,
                         data.fMasterVolume);
                }
            }
            else
            {
                XLOG(XFAIL, eError, "FAIL: Volume Event Not Receieved"); 
            }
        }
    }

    // get the number of channels
    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelCount",
                      hr,
                      spEndpointVolume->GetChannelCount(&nChannels));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to get endpoint channel count");
        goto UNREGISTER;
    }

    // set a baseline volume for each channel
    for(UINT32 i=0;i<nChannels;i++)
    {       
        METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevelScalar",
                          hr,
                          spEndpointVolume->SetChannelVolumeLevelScalar(i,0.0f,&GUID_ENDPOINTVOLUME_TEST));  
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Unable to set endpoint volume");
            goto UNREGISTER;
        }
    }

    Sleep(1000);

    // ramp the volume for each channel using SetChannelVolumeLevel, verifying events in the mean time
    for(float fSetVol=fMaxDbVol;fSetVol>=fMinDbVol;fSetVol-=5.0f)
    {

        for(UINT32 i=0;i<nChannels;i++)
        {
            // generate a guid
            METHODCHECKRESULT("::CoCreateGuid",
                              hr,
                              ::CoCreateGuid(&guidEventContext));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: Unable to created GUID");
                goto UNREGISTER;
            }
            
            pobjEndpointEventHandler->SetEventContextFilterAndReset(&guidEventContext);

            METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevel",
                              hr,                          
                              spEndpointVolume->SetChannelVolumeLevel(i,fSetVol,&guidEventContext));       
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: Unable to set master endpoint volume with atten: %f", fSetVol);
                goto UNREGISTER;
            }

            if(S_FALSE != hr)
            {
                // wait for the event to fire
                METHODCHECKRESULT("WaitForEndpointEvent",
                                  hr,
                                  WaitForEndpointEvent(pobjEndpointEventHandler,
                                                       EVENT_TIMEOUT));
                if(FAILED(hr))
                {
                    XLOG(XFAIL, eError, "FAIL: No endpoint event received");
                    goto UNREGISTER;
                }
           
                // check to see if the event has fired
                if(pobjEndpointEventHandler->CheckEventReceived())
                {
                    AUDIO_VOLUME_NOTIFICATION_DATA data = pobjEndpointEventHandler->GetLastVolumeData();

                    // verify the data
                    if(data.nChannels != nChannels)
                    {
                        XLOG(XFAIL, eFatalError, "FAIL: Wrong number of channels in event data");
                    }        
                }
                else
                {
                    XLOG(XFAIL, eError, "FAIL: Volume Event Not Receieved");
                }
            }
        }
    }

    // set a baseline volume for each channel
    for(UINT32 i=0;i<nChannels;i++)
    {               
        METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevelScalar",
                          hr,
                          spEndpointVolume->SetChannelVolumeLevelScalar(i,MAX_SCALAR_ATTEN,&GUID_ENDPOINTVOLUME_TEST));  
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Unable to set endpoint channel volume (scalar) to %f", 0.0f);
            goto UNREGISTER;
        }
    }

    Sleep(1000);

    // ramp the volume for each channel using SetChannelVolumeLevelScalar, verifying events in the mean time
    for(float fSetVol=MIN_SCALAR_ATTEN;fSetVol<=MAX_SCALAR_ATTEN;fSetVol+=.05f)
    {

        for(UINT32 i=0;i<nChannels;i++)
        {
            // create a guid
            METHODCHECKRESULT("::CoCreateGuid",
                              hr,
                              ::CoCreateGuid(&guidEventContext));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: unable to create GUID");
                goto UNREGISTER;
            }
            
            pobjEndpointEventHandler->SetEventContextFilterAndReset(&guidEventContext);

            METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevelScalar",
                              hr,                          
                              spEndpointVolume->SetChannelVolumeLevelScalar(i,fSetVol,&guidEventContext));
        

            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: unable to set volume for channel %d with atten %f", i, fSetVol);
            }
            else
            {
                SLOG(eInfo1, "Set Scalar Channel Volume for channel %d to %f", i, fSetVol);
            }


            if(S_FALSE != hr)
            {
                // wait for the event to fire
                METHODCHECKRESULT("WaitForEndpointEvent",
                                  hr,
                                  WaitForEndpointEvent(pobjEndpointEventHandler,
                                                       EVENT_TIMEOUT));
                if(FAILED(hr))
                {
                    XLOG(XFAIL, eError, "FAIL: Endpoint event not received.");
                    goto UNREGISTER;
                }
       
                // check to see if the event has fired
                if(pobjEndpointEventHandler->CheckEventReceived())
                {
                    AUDIO_VOLUME_NOTIFICATION_DATA data = pobjEndpointEventHandler->GetLastVolumeData();

                    // verify the data
                    if(data.nChannels != nChannels)
                    {
                        XLOG(XFAIL, eError, "FAIL: Wrong number of channels in event data");
                    }            
                }
                else
                {
                    XLOG(XFAIL, eError, "FAIL: Volume Event Not Receieved");
                    goto UNREGISTER;
                }
            }
        }
    }


    //
    // End Scenario
    //

UNREGISTER:
    // un-register for notifications
    METHODCHECKRESULT("IAudioEndpointVolume::RegisterControlChangeNotify",
                      hr,                      
                      spEndpointVolume->UnregisterControlChangeNotify(spEndpointCallback));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

END:
    SAFE_RELEASE_POINTER(pobjEndpointEventHandler);
    return FNS_PASS;
}

//-------------------------------------------------------------------------
// Description:
//  Test master volume eventing notification unregistration (invalid scenarios)
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_PerEndpointEvents_ScenarioIV1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;

    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

       // Call RegisterControlChangeNotify with a NULL pointer
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioEndpointVolume::UnegisterControlChangeNotify",
                      hr,                      
                      spEndpointVolume->UnregisterControlChangeNotify(NULL));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should not have succeeded with a NULL parameter");
        goto END;
    }    

END:
    return FNS_PASS;
}
