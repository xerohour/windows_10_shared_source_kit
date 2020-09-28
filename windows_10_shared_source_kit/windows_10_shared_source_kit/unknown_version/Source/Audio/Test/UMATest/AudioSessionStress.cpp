// Copyright(C) Microsoft. All rights reserved.

//
// AudioSessionStress.cpp
//  
//
// Description:
//
//  AudioSession creation stress test

#include <stdafx.h>
#include <UMATestStreaming.h>

#include "TestUtilities.h"

//-------------------------------------------------------------------------
// Method: Tc_AudioSessionStress
//
// Description:
//  Create a high number of audiosessions
//    
// Parameters:
//
// Return values (DWORD):
//     FNS_PASS on success, FNS_FAIL on failure
//
DWORD Tc_AudioSessionStress()
{
    HRESULT hr = S_OK;

    // num sessions to create
    const int NUM_SESSIONS = 100;

    CComPtr<IAudioSessionManager> spAudSessMan = NULL;
    CComPtr<IMMDevice> spEndpointUnderTest = NULL;

    LPWSTR wszNewSessionID = NULL;
    int numCreatedSessions = 0;
    
    SLOG(eError, "Running Scenario!");

    // Get the IMMDevice for the endpoint currently under test
    hr = GetEndpointUnderTest(&spEndpointUnderTest);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: GetEndpointUnderTest 0x%08X", hr);
    }

    if(NULL == spEndpointUnderTest)
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IMMDevice under test!");
        goto END;
    }
    
    // Get a session manager for the endpoint under test.
    METHODCHECKRESULT("IMMDevice::Activate",
                      hr,
                      spEndpointUnderTest->Activate(__uuidof(IAudioSessionManager),
                                                    CLSCTX_ALL,
                                                    NULL,
                                                    (void**)&spAudSessMan));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionManager interface");
        goto END;
    }
    
    //
    // Start Stressing Out!
    //

    for(int i=0;i<=NUM_SESSIONS;i++)
    {
        METHODCHECKRESULT("Creating AudioSession",
                          hr,
                          CreateEmptySessionInProcess(spAudSessMan, wszNewSessionID));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Unable to create audiosession");
        }
        else
        {
            SAFE_COTASKMEMFREE(wszNewSessionID);
            numCreatedSessions++;
            SLOG(eInfo1, "Created %d sessions...",i);
        }        
    }

    int waited = 0;
    // now wait 6 minutes
    for(int minutes=1;minutes<=6;minutes++)
    {
        for(int seconds=1;seconds<=60;seconds+=1)
        {
            SLOG(eInfo1, "Waited %d seconds", waited);
            Sleep(1000);   
            waited+= 1;
        }
    }

END:    
    return FNS_PASS;   
}

