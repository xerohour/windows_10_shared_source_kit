// Copyright(C) Microsoft. All rights reserved.

//
// SimpleAudioVolumeAPITests.cpp
//  
//
// Description:
//
//  WASAPI ISimpleAudioVolume API Tests

#include <stdafx.h>
#include <UMATestStreaming.h>


#include "StreamingControlTestFramework.h"

// GUID used for Volume API Test Contexts
// {5E1F96CA-E07E-4b45-8362-6A719283CFD3}
static const GUID VOLUME_TEST_GUID = 
{ 0x5e1f96ca, 0xe07e, 0x4b45, { 0x83, 0x62, 0x6a, 0x71, 0x92, 0x83, 0xcf, 0xd3 } };

//-----------------------------------------------------------------------------
// Method: VerifyEndpointAgreement
//
// Description:
//             Verify that the appropriate endpoint volume matches the session
//             volume for capture scenarios
//
//    
// Parameters: dwDeviceType [in]        - The streaming type of the device under test
//             fSessionVolume [in]      - The current attenuation of the session being tested
//             bSessionMute [in]        - The current mute state of the session being tested
//             pAssocEndpointVol [in]   - An IAudioEndpointVolume interface
//                                        activated on the endpoint associated
//                                        with the stream being examined.
//
// Return values: S_OK on success.  Any failures will be logged via XLOG
//
// ----------------------------------------------------------------------------
HRESULT VerifyEndpointAgreement(DWORD dwClientType,
                                FLOAT fSessionVolume,
                                BOOL bSessionMute,
                                IAudioEndpointVolume* pAssocEndpointVol)
{
    HRESULT hr = S_OK;
    FLOAT fEndpointVol = 0.0f;
    BOOL bEndpointMute = FALSE;

    if(DEVICE_TYPE_CAPTURE != dwClientType)
    {
        // This function is a no-op if we're not dealing with capture
        goto END;
    }
    

    METHODCHECKRESULT_GOTO("IAudioEndpointVolume::GetMasterVolumeLevelScalar",
                           hr,
                           pAssocEndpointVol->GetMasterVolumeLevelScalar(&fEndpointVol),
                           END);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Interface under test returned unexpected HRESULT");
        goto END;
    }

    METHODCHECKRESULT_GOTO("IAudioEndpointVolume::GetMute",
                           hr,
                           pAssocEndpointVol->GetMute(&bEndpointMute),
                           END);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Interface under test returned unexpected HRESULT");
        goto END;
    }

    if(bEndpointMute != bSessionMute)
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Capture Session %d and observed endpoint mute states %d to not match",
             bSessionMute,
             bEndpointMute);
    }

    if(EPSILON < abs(fEndpointVol-fSessionVolume))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Capture Session %f and observed endpoint volumes %f to not match",
             fSessionVolume,
             fEndpointVol);
    }

END:
    return hr;
}



DWORD TC_SimpleAudioVolume_SetMasterVolume_Basic0()
{
    RunStreamingControlTest(SIMPLEAUDIOVOLUME_SETMASTERVOLUME_BASIC0);
    return FNS_PASS;       
}

DWORD TC_SimpleAudioVolume_SetMasterVolume_Valid0()
{
    RunStreamingControlTest(SIMPLEAUDIOVOLUME_SETMASTERVOLUME_VALID0);
    return FNS_PASS;       
}    

DWORD TC_SimpleAudioVolume_GetSetMute_Basic0()
{
    RunStreamingControlTest(SIMPLEAUDIOVOLUME_GETSETMUTE_BASIC0);
    return FNS_PASS;       
}

DWORD TC_SimpleAudioVolume_SetMasterVolume_Invalid0()
{
    RunStreamingControlTest(SIMPLEAUDIOVOLUME_SETMASTERVOLUME_INVALID0);
    return FNS_PASS;       
}

DWORD TC_SimpleAudioVolume_SetMasterVolume_Invalid1()
{
    RunStreamingControlTest(SIMPLEAUDIOVOLUME_SETMASTERVOLUME_INVALID1);
    return FNS_PASS;       
}

DWORD TC_SimpleAudioVolume_GetMasterVolume_Invalid0()
{
    RunStreamingControlTest(SIMPLEAUDIOVOLUME_GETMASTERVOLUME_INVALID0);
    return FNS_PASS;       
}

DWORD TC_SimpleAudioVolume_GetMute_Invalid0()
{
    RunStreamingControlTest(SIMPLEAUDIOVOLUME_GETMUTE_INVALID0);
    return FNS_PASS;       
}

DWORD TC_SimpleAudioVolume_DisconnectService()
{
    RunStreamingControlTest(SIMPLEAUDIOVOLUME_DISCONNECTSERVICE);
    return FNS_PASS;       
}

//------------------------------------------------------------------------------
// Name: Test_SimpleAudioVolume_SetMasterVolume_Basic0
// Desc: Test SetMasterVolume with various valid volumes and ensure HRESULTs are S_OK
//
// Returns:
//              FNS_PASS (failure is signalled by XLOG
//------------------------------------------------------------------------------
DWORD Test_SimpleAudioVolume_SetMasterVolume_Basic0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<ISimpleAudioVolume> spSimpleVolume = NULL;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;
    FLOAT fEndpointVolume = 0.5f;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSimpleAudioVolume(&spSimpleVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume for test!");
        return FNS_PASS;
    }


    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume for test!");
        return FNS_PASS;
    }

    // set the endpoint volume to 0.5 to ensure propogated volumes are unbiased
    // by the original volumes
    METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevelScalar",
                      hr,
                      spEndpointVolume->SetMasterVolumeLevelScalar(fEndpointVolume, &(VOLUME_TEST_GUID)));                     
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: IAudioEndpointVolume::SetMasterVolume returned unexpected result.");
    }
  
    Sleep(500);
    // actual testing starts here
    for(float fSetVol=0.0f;fSetVol<=1.0f;fSetVol+=.2f)
    {
        // to avoid floating-point errors
        if(fSetVol>1.0f)
        {
            fSetVol = 1.0f;
        }

        SLOG(eInfo1,"Setting session master volume to %.3f...",fSetVol);
        METHODCHECKRESULT("ISimpleAudioVolume::SetMasterVolume",
                          hr,
                          spSimpleVolume->SetMasterVolume(fSetVol, &(VOLUME_TEST_GUID)));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: ISimpleVolume::SetMasterVolume returned unexpected result.");
        }

        Sleep(75);
    }       

    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Name: Test_SimpleAudioVolume_SetMasterVolume_Valid0
// Desc: Test SetMasterVolume with valid volume in [0,1], ensure HRESULTs are S_OK
//       and verify that calls to GetMasterVolume reflect the updated volume
//
// Returns:
//              FNS_PASS (Failure is signalled by XLOG)
//------------------------------------------------------------------------------
DWORD Test_SimpleAudioVolume_SetMasterVolume_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;
    CComPtr<ISimpleAudioVolume> spSimpleVolume = NULL;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume for test!");
        goto END;
    }

    hr = pStreamingEngine->GetSimpleAudioVolume(&spSimpleVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume for test!");
        goto END;
    }


    // set the endpoint volume to 0.5 to ensure propogated volumes are unbiased
    // by the original volumes
    METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevelScalar",
                      hr,
                      spEndpointVolume->SetMasterVolumeLevelScalar(0.5, &(VOLUME_TEST_GUID)));                     

    Sleep(500);
    
    for(float fSetVol=0.0f;fSetVol<=1.02f;fSetVol+=.2f)
    {
        // to avoid floating-point errors
        if(fSetVol>1.0f)
        {
            fSetVol = 1.0f;
        }

        float fGetVol = -1.0f;
        BOOL bGetMute = FALSE;

        SLOG(eInfo1,"Setting session master volume to %.3f...",fSetVol);
        METHODCHECKRESULT("ISimpleAudioVolume::SetMasterVolume",
                          hr,
                          spSimpleVolume->SetMasterVolume(fSetVol, &(VOLUME_TEST_GUID)));

        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Interface under test returned unexpected HRESULT");
        }

        SLOG(eInfo1,"Verifying session master volume = %.3f",fSetVol);
        METHODCHECKRESULT("ISimpleVolume::GetMasterVolume",
                          hr,
                          spSimpleVolume->GetMasterVolume(&fGetVol));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Interface under test returned unexpected HRESULT");
        }

        // clip volume at 1.0f for verification purposes
        if(fSetVol >= 1.0f)
        {
            fSetVol = 1.0f;
        }
                                   
        if(EPSILON < abs(fSetVol-fGetVol))
        {
            XLOG(XFAIL,
                 eError,
                 "Set (%f) and retrieved (%f) volume values do not match!",
                 fSetVol,
                 fGetVol);
        }
        else
        {
            SLOG(eInfo1, "Set and get volumes match");
        }

        // get the session mute volume for comparison with the endpoint.
        METHODCHECKRESULT("ISimpleVolume::GetMute",
                          hr,
                          spSimpleVolume->GetMute(&bGetMute));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Interface under test returned unexpected HRESULT");
        }



        METHODCHECKRESULT("Verify Endpoint Agreement",
                          hr,
                          VerifyEndpointAgreement(dwDeviceType,
                                                  fGetVol,
                                                  bGetMute,
                                                  spEndpointVolume));
        
    }
    
END:
    return FNS_PASS;        
}

//------------------------------------------------------------------------------
// Name: Test_SimpleAudioVolume_GetSetMute_Basic0
// Desc: Verify the basic functionality of ISimpleVolume::SetMute
//
// Returns:
//              FNS_PASS (failures are reported via XLOG
//------------------------------------------------------------------------------
DWORD Test_SimpleAudioVolume_GetSetMute_Basic0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;    
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;
    CComPtr<ISimpleAudioVolume> spSimpleVolume = NULL;
    
    BOOL bSetVal = FALSE;
    BOOL bGetVal = FALSE;
    FLOAT fEndpointVolume = 0.5f;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSimpleAudioVolume(&spSimpleVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume for test!");
        goto END;
    }

    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume for test!");
        goto END;
    }

    // set the endpoint volume to 0.5 to ensure propogated volumes are unbiased
    // by the original volumes
    METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevelScalar",
                      hr,
                      spEndpointVolume->SetMasterVolumeLevelScalar(fEndpointVolume, &(VOLUME_TEST_GUID)));                     
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: IAudioEndpointVolume::SetMasterVolume returned unexpected result.");
    }

    for(int i=1;i<5;i++)
    {
        // set the mute value
        SLOG(eBlab1,"Setting mute to: %s",bSetVal?"TRUE":"FALSE");  

        METHODCHECKRESULT("ISimpleAudioVolume::SetMute",
                          hr,
                          spSimpleVolume->SetMute(bSetVal, &(VOLUME_TEST_GUID)));
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Interface under test returned unexpected HRESULT");
        }
    
        // verify that the mute call succeeded
        METHODCHECKRESULT("ISimpleAudioVolume::GetMute",
                          hr,
                          spSimpleVolume->GetMute(&bGetVal));

        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Interface under test returned unexpected HRESULT");
   
        }

        if(bGetVal != bSetVal)
        {
            XLOG(XFAIL, eError, "FAIL: GetMute and SetMute values do not match!");
            
        }

        // verify that the mute call propagated to teh endpoint
        VerifyEndpointAgreement(dwDeviceType,
                                fEndpointVolume, // constant
                                bGetVal, 
                                spEndpointVolume);
                                
                    
        // flip the mute value for the next test case at random
        if((g_IShell->Random()%2)==0)            
        {
            bSetVal = !bSetVal;   
        }
    }

    // ensure the stream is not muted at the end of the test
    METHODCHECKRESULT("ISimpleAudioVolume::SetMute",
                       hr,
                       spSimpleVolume->SetMute(FALSE, &(VOLUME_TEST_GUID)));
    if(FAILED(hr)) 
    {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Interface under test returned unexpected HRESULT");
    }                       

END:    
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Name: Test_SimpleAudioVolume_SetMasterVolume_Invalid0
// Desc: Test SetMasterVolume with invalid params (attenuation <0)
//       Verify that SetMasterVolume calls fail with invalid params and that
//       the master volume level remains unchanged
// Returns:
//              FNS_PASS (failures are reported via XLOG
//------------------------------------------------------------------------------
DWORD Test_SimpleAudioVolume_SetMasterVolume_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    const FLOAT fBaseVol = 0.5f;
    float pfTestVolumes[] = {-.0001f, -5.0f, -10.0f, -999.0f, -9999.0f };
    CComPtr<ISimpleAudioVolume> spSimpleVolume = NULL;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSimpleAudioVolume(&spSimpleVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume for test!");
        goto END;
    }

    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume for test!");
        goto END;
    }
    
    //
    // Set a baseline volume so we can ensure that the volume wasn't changed
    // by invalid calls.
    //
    METHODCHECKRESULT("ISimpleAudioVolume::SetMasterVolume",
                      hr,
                      spSimpleVolume->SetMasterVolume(fBaseVol,
                                                      &(VOLUME_TEST_GUID)));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unexpected HRESULT from API under test");
        goto END;
    }

    for(int i=0;i<sizeof(pfTestVolumes)/sizeof(FLOAT);i++)
    {
        float fGetVol = 0.0f;
        float fSetVol = pfTestVolumes[i];

        SLOG(eBlab1,"Setting session master volume to: %f",fSetVol);
        METHODCHECKRESULT("ISimpleAudioVolume::SetMasterVolume",
                          hr,
                          spSimpleVolume->SetMasterVolume(fSetVol, &(VOLUME_TEST_GUID)));

        // verify the invalid param caused the method to fail
        if(!FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: SetMasterVolume(%f) should not have succeeded",fSetVol);
        }                                                          

        // make sure the failed call didn't have any side-effects
        METHODCHECKRESULT("ISimpleAudioVolume::GetMasterVolume",
                          hr,
                          spSimpleVolume->GetMasterVolume(&fGetVol));

        if(EPSILON < abs(fBaseVol-fGetVol))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: MasterVolume (%f) should not have been modified",
                 fGetVol);
        }

        METHODCHECKRESULT("Verify Endpoint Agreement",
                          hr,
                          VerifyEndpointAgreement(dwDeviceType,
                                                  0.5f,
                                                  FALSE, //constant
                                                  spEndpointVolume));        
    }

END:
    return FNS_PASS;    
}

//------------------------------------------------------------------------------
// Name: Test_SimpleAudioVolume_SetMasterVolume_Invalid1
// Desc: Test SetMasterVolume with valid volumes (>1) and ensure FAILED hresults
//
// Returns:
//              FNS_PASS if the test passes
//              FNS_FAIL otherwise
//------------------------------------------------------------------------------
DWORD Test_SimpleAudioVolume_SetMasterVolume_Invalid1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    const FLOAT fBaseVol = 0.5f;
    FLOAT pfTestVolumes[] = {1.0001f, 5.0f, 10.0f, 999.0f, 9999.0f };
    CComPtr<ISimpleAudioVolume> spSimpleVolume = NULL;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSimpleAudioVolume(&spSimpleVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume for test!");
        goto END;
    }

    //
    // Set a baseline volume so we can ensure that the volume wasn't changed
    // by invalid calls.
    //
    METHODCHECKRESULT("ISimpleAudioVolume::SetMasterVolume",
                      hr,
                      spSimpleVolume->SetMasterVolume(fBaseVol,
                                                      &(VOLUME_TEST_GUID)));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unexpected HRESULT from API under test");
        goto END;
    }

    for(int i=0;i<sizeof(pfTestVolumes)/sizeof(FLOAT);i++)
    {
        FLOAT fSetVol = pfTestVolumes[i];
        FLOAT fGetVol = -1.0f;
        

        SLOG(eInfo1,
             "Setting session master volume to %.3f...",
             fSetVol);

        METHODCHECKRESULT("ISimpleAudioVolume::SetMasterVolume",
                          hr,
                          spSimpleVolume->SetMasterVolume(fSetVol, &(VOLUME_TEST_GUID)));
        if(!FAILED(hr))
        {
            XLOG(XFAIL, eFatalError, "FAIL: Set call succeeded for invalid attenuation.");
        }           

        // make sure the volume hasn't changed
        METHODCHECKRESULT("ISimpleAudioVolume::GetMasterVolume",
                          hr,
                          spSimpleVolume->GetMasterVolume(&fGetVol));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Unexpected HRESULT from API under test");            
        }

        if(fGetVol != fBaseVol)
        {
            XLOG(XFAIL, 
                 eError, 
                 "FAIL: Volume should have remained constant, but changed from %f to %f",
                 fBaseVol,
                 fGetVol);
        }
    }

END:
    return FNS_PASS;        
}

//------------------------------------------------------------------------------
// Name: TC_AudioClient_SimpleAudioVolume_GetMasterVolume_Invalid_0
// Desc: Verify GetMasterVolume behaves appropriately when passed a null pointer
//
// Returns:
//      FNS_PASS; Failure signalled via XLOG
//------------------------------------------------------------------------------
DWORD Test_SimpleAudioVolume_GetMasterVolume_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    float fVol = 0.0f;
    CComPtr<ISimpleAudioVolume> spSimpleVolume = NULL;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSimpleAudioVolume(&spSimpleVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume for test!");
        goto END;
    }

    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume for test!");
        goto END;
    }

    // set the volume to a baseline value
    METHODCHECKRESULT("ISimpleAudioVolume::SetMasterVolume",
                      hr,
                      spSimpleVolume->SetMasterVolume(0.5f, &(VOLUME_TEST_GUID)));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error setting initial volume.  Abandoning test.");
        goto END;
    }
                              
    METHODCHECKRESULT("ISimpleAudioVolume::GetMasterVolume",
                      hr,
                      spSimpleVolume->GetMasterVolume(&fVol));
    if(FAILED(hr) || (EPSILON < abs(0.5f-fVol)))
    {
        XLOG(XFAIL, 
             eError, 
             "FAIL: Error verifying initial volume.  Abandoning test.");
    }
    
    // attempt to call get volume with a NULL parameter
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("ISimpleAudioVolume::GetMasterVolume",
                      hr,
                      spSimpleVolume->GetMasterVolume(NULL));
    if(SUCCEEDED(hr))
    {            
        XLOG(XFAIL, eError, "FAIL: GetMasterVolume should not have succeeded");
    }

    // verify that the volume was not modified by the previous call
    METHODCHECKRESULT("ISimpleAudioVolume::GetMasterVolume",
                      hr,
                      spSimpleVolume->GetMasterVolume(&fVol));
    if(FAILED(hr) || (EPSILON < abs(0.5f-fVol)))
    {
        XLOG(XFAIL, 
             eError, 
             "ERROR: Error verifying volume.");
    }

    METHODCHECKRESULT("Verify Endpoint Agreement",
                      hr,
                      VerifyEndpointAgreement(dwDeviceType,
                                              0.5f,
                                              FALSE, //constant
                                              spEndpointVolume)); 
    
END:
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Name: TC_AudioClient_SimpleAudioVolume_GetMute_Invalid_0
// Desc: Test GetMute with a NULL parameter
//
// Returns:
//      FNS_PASS (Failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_SimpleAudioVolume_GetMute_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<ISimpleAudioVolume> spSimpleVolume = NULL;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSimpleAudioVolume(&spSimpleVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume for test!");
        goto END;
    }
   
    // set the mute-state of the session to establish a baseline
    METHODCHECKRESULT("ISimpleAudioVolume::SetMute",
                      hr,
                      spSimpleVolume->SetMute(TRUE, &(VOLUME_TEST_GUID)));
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: SetMute failed unexpectedly... abandoning test");
        goto END;
    }

    // verify that the SetMute call was successful
    BOOL bMuteVal;
    METHODCHECKRESULT("ISimpleAudioVolume::GetMute",
                      hr,
                      spSimpleVolume->GetMute(&bMuteVal));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: GetMute failed unexpectedly... abandoning test");
        goto END;
    }

    if(TRUE != bMuteVal)
    {
        XLOG(XFAIL,
            eError,
            "FAIL: Initial call to SetMute did not take effect... abandoning test");
        goto END;
    }


    // attempt to call get mute with a null parameter
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("ISimpleAudioVolume::GetMute",
                      hr,
                      spSimpleVolume->GetMute(NULL));

    if(SUCCEEDED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: GetMute should not have succeeded");
    }

    // verify that the GetMute call had no effect on the mute state
    METHODCHECKRESULT("ISimpleAudioVolume::GetMute",
                      hr,
                      spSimpleVolume->GetMute(&bMuteVal));

    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Call to GetMute failed!");
    }

    if(TRUE != bMuteVal)
    {
        XLOG(XFAIL, eError, "FAIL: Audio session mute state was unexpectedly altered!");
    }

    // ensure the stream is not muted at the end of the test
    METHODCHECKRESULT("ISimpleAudioVolume::SetMute",
                       hr,
                       spSimpleVolume->SetMute(FALSE, &(VOLUME_TEST_GUID)));

END:
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Name: TC_AudioClient_SimpleAudioVolume_DisconnectService
// Desc: Test ISimpleAudioVolume methods under RPC failure conditions
//
// Returns:
//              FNS_PASS (Failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_SimpleAudioVolume_DisconnectService(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<ISimpleAudioVolume> spSimpleVolume = NULL;
    BOOL bMute = FALSE;
    FLOAT fVol = 0.0f;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSimpleAudioVolume(&spSimpleVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume for test!");
        goto END;
    }

    // shut down audiosrv
    if (!StopAudioService())
    {
        XLOG(XFAIL, eError, "FAIL: Unable to stop AudioSrv!");
        goto END;
    }  
    
    // BEGIN ISimpleAudioVolume API Calls
    ////
    
    // SetMasterVolume
    METHODCHECKRESULT("ISimpleAudioVolume::SetMasterVolume",
                       hr,
                       spSimpleVolume->SetMasterVolume(fVol, &(VOLUME_TEST_GUID)));
    if(SUCCEEDED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should not have succeeded");
    }

    // GetMasterVolume
    METHODCHECKRESULT("ISimpleAudioVolume::GetMasterVolume",
                      hr,
                      spSimpleVolume->GetMasterVolume(&fVol));
    if(SUCCEEDED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should not have succeeded");
    }

    // SetMute
    METHODCHECKRESULT("ISimpleAudioVolume::SetMute",
                      hr,
                      spSimpleVolume->SetMute(FALSE, &(VOLUME_TEST_GUID)));
    if(SUCCEEDED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should not have succeeded");
    }

    // GetMute
    METHODCHECKRESULT("ISimpleAudioVolume::SetMute",
                      hr,
                      spSimpleVolume->GetMute(&bMute));
    if(SUCCEEDED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should not have succeeded");
    }
   
    ////
    // END ISimpleAudioVolume API Calls
    
END:
   // Start audiosrv back up. Moving this into end section so that even if this function fails, the following test cases do not fail.
    if (!StartAudioService())
    {
        XLOG(XFAIL, eFatalError, "FAIL: Unable to re-start AudioSrv! System may be in an unusable state!");
    }
    
    return FNS_PASS;
}
