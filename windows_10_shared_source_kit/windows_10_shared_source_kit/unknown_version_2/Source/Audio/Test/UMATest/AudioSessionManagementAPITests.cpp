// Copyright(C) Microsoft. All rights reserved.

//
// AudioSessionManagementAPITests.cpp
//  
//
// Description:
//
//  WASAPI AudioSession Management API Tests

#include <stdafx.h>
#include <UMATestStreaming.h>

#include "TestUtilities.h"
#include "StreamingControlTestFramework.h"
#include "SessionManagerEventHandler.h"

//-------------------------------------------------------------------------
// Description:
//  Basic GetSessionEnumerator tests
//    
// Parameters:
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD TC_SessionManagement_GetSessionEnum_Valid1()
{
    RunStreamingControlTest(SESSIONMGMT_GETSESSIONENUM_VALID1);
    return FNS_PASS;
}

DWORD TC_SessionManagement_GetSessionEnum_Valid2()
{
    RunStreamingControlTest(SESSIONMGMT_GETSESSIONENUM_VALID2);
    return FNS_PASS;
}

DWORD TC_SessionManagement_GetSessionEnum_Valid3()
{
    RunStreamingControlTest(SESSIONMGMT_GETSESSIONENUM_VALID3);
    return FNS_PASS;
}

DWORD TC_SessionManagement_AddSessionNotif_Valid1()
{
    RunStreamingControlTest(SESSIONMGMT_ADDSESSIONNOTIF_VALID1);
    return FNS_PASS;
}

DWORD TC_SessionManagement_AddSessionNotif_Valid2()
{
    RunStreamingControlTest(SESSIONMGMT_ADDSESSIONNOTIF_VALID2);
    return FNS_PASS;
}

DWORD TC_SessionManagement_AddSessionNotif_Valid3()
{
    RunStreamingControlTest(SESSIONMGMT_ADDSESSIONNOTIF_VALID3);
    return FNS_PASS;
}

DWORD TC_SessionManagement_AddSessionNotif_Valid4()
{
    RunStreamingControlTest(SESSIONMGMT_ADDSESSIONNOTIF_VALID4);
    return FNS_PASS;
}

DWORD TC_SessionManagement_DeleteSessionNotif_Valid1()
{
    RunStreamingControlTest(SESSIONMGMT_DELSESSIONNOTIF_VALID1);
    return FNS_PASS;
}

DWORD TC_SessionManagement_GetSession_Valid1()
{
    RunStreamingControlTest(SESSIONMGMT_GETSESSION_VALID1);
    return FNS_PASS;
}

DWORD TC_SessionManagement_GetSession_Valid2()
{
    RunStreamingControlTest(SESSIONMGMT_GETSESSION_VALID2);
    return FNS_PASS;
}

DWORD TC_SessionManagement_GetSession_Valid3()
{
    RunStreamingControlTest(SESSIONMGMT_GETSESSION_VALID3);
    return FNS_PASS;
}

DWORD TC_SessionManagement_GetSession_Valid4()
{
    RunStreamingControlTest(SESSIONMGMT_GETSESSION_VALID4);
    return FNS_PASS;
}

DWORD TC_SessionManagement_AddSessionNotif_Invalid1()
{
    RunStreamingControlTest(SESSIONMGMT_ADDSESSIONNOTIF_INVALID1);
    return FNS_PASS;
}

DWORD TC_SessionManagement_DeleteSessionNotif_Invalid1()
{
    RunStreamingControlTest(SESSIONMGMT_DELSESSIONNOTIF_INVALID1);
    return FNS_PASS;
}

DWORD TC_SessionManagement_DisconnectService()
{
    RunStreamingControlTest(SESSIONMGMT_DISCONNECTSERVICE);
    return FNS_PASS;
}

//-------------------------------------------------------------------------
// Description:
//  Basic GetSessionEnumerator tests
//    
// Parameters:
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_SessionManagement_GetSessionEnum_Valid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    DWORD dwRes = FNS_PASS;
    HRESULT hr = S_OK;
    
    CComPtr<IAudioSessionManager2> spAudSessManExtended = NULL;
    CComPtr<IAudioSessionEnumerator> spEnumSessions = NULL;

    if (GetModule()->IsAppContainer())
    {
        XLOG(XSKIP, eError, "SKIP: IAudioSessionManager2 activation is blocked in AppContainer. Test case skipping...");
        goto END;
    }

    METHODCHECKRESULT("ITestStreamingEngine::GetAudioSessionManager2",
                      hr,
                      pStreamingEngine->GetAudioSessionManager2(&spAudSessManExtended));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionManager2 interface");
        goto END;
    }
    

    METHODCHECKRESULT("IAudioSessionManager::GetSessionEnumerator",
                      hr,
                      spAudSessManExtended->GetSessionEnumerator(&spEnumSessions));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
    }

    if(spEnumSessions == NULL)
    {
        XLOG(XFAIL, eError, "FAIL: GetSessionEnumerator returned NULL");
    }

END:
    return dwRes;
}

DWORD Test_SessionManagement_GetSessionEnum_Valid2(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;

    CComPtr<IAudioSessionManager2> spAudSessManExtended = NULL;
    CComPtr<IAudioSessionManager> spAudSessMan = NULL;
    CComPtr<IAudioSessionEnumerator> spEnumSessions = NULL;
    CComPtr<IAudioSessionControl> spAudSessCtrl = NULL;
    CComPtr<IAudioSessionControl> spCurrSession = NULL;
    CComPtr<IAudioSessionControl2> spCurrSessionEx = NULL;

    INT nSesCount = 0;
    LPWSTR t_wszDispName = NULL;

    // Get the AudioSessionManager interface
    METHODCHECKRESULT("ITestStreamingEngine::GetSessionManager",
                      hr,
                      pStreamingEngine->GetAudioSessionManager(&spAudSessMan));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionManager interface");
        goto END;
    }

    if (GetModule()->IsAppContainer())
    {
        XLOG(XSKIP, eError, "SKIP: IAudioSessionManager2 activation is blocked in AppContainer. Test case skipping...");
        goto END;
    }

    // Get the internal AudioSessionManager interface
    METHODCHECKRESULT("ITestStreamingEngine::GetSessionManager2",
                      hr,
                      pStreamingEngine->GetAudioSessionManager2(&spAudSessManExtended));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionManager2 interface");
        goto END;
    }

    // create a new audiosession
    METHODCHECKRESULT("CreateEmptySessionInProcess",
                      hr,
                      CreateEmptySessionInProcess(spAudSessMan,
                                                  t_wszDispName));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "Unable to create audiosession!");
        goto END;
    }

        
    // Get an audiosession enumerator object
    METHODCHECKRESULT("IAudioSessionManager::GetSessionEnumerator",
                      hr,
                      spAudSessManExtended->GetSessionEnumerator(&spEnumSessions));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain session enumerator");
        goto END;
    }

    if(NULL == spEnumSessions)
    {
        XLOG(XFAIL, eError, "FAIL: GetSessionEnumerator returned NULL");
        goto END;
    }

    // Go through the sessions within the enum object, and attempt to find the one just created

    // get the number of sessions created
    METHODCHECKRESULT("IAudioSessionEnumerator::GetCount",
                      hr,
                      spEnumSessions->GetCount(&nSesCount));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    SLOG(eInfo1, "Enumerator contained %d audiosessions", nSesCount);

    BOOL bSessionFound = FALSE;
    for(int i=0;i<nSesCount;i++)
    {
        BSTR t_wszCurrID = NULL;

        METHODCHECKRESULT("IAudioSessionEnumerator::GetSession",
                          hr,
                          spEnumSessions->GetSession(i, &spCurrSession));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Unable to obtain audiosession control");
            goto END;
        }

        // get the extended session control interface
        METHODCHECKRESULT("IAudioSessionControl::QueryInterface",
                          hr,
                          spCurrSession->QueryInterface(__uuidof(IAudioSessionControl2),
                          (void**)&spCurrSessionEx));

        if(FAILED(hr)) 
        { 
            XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl2 interface");
            goto END; 
        }
    
        // get the ID of the audiosession currently under examination
        METHODCHECKRESULT("IAudioSessionControl2::GetSessionInstanceIdentifier",
                          hr,
                          spCurrSessionEx->GetSessionInstanceIdentifier(&t_wszCurrID));

        if(FAILED(hr))
        {
            SLOG(eInfo1, "FAIL: Unable to obtain audiosession ID");
            goto END;
        }

        if(0 == wcscmp(t_wszDispName, t_wszCurrID))
        {
            bSessionFound = TRUE;
        }
    
        spCurrSession = NULL;
        spCurrSessionEx = NULL;
        
        SAFE_COTASKMEMFREE(t_wszCurrID);
    
        // small optimization
        if(bSessionFound) { break; }
    }

    if(FALSE == bSessionFound)
    {
        SLOG(eFatalError, "FAIL: Could not find newly created audiosession in enumerator!");
    }

END:  
    SAFE_COTASKMEMFREE(t_wszDispName);
    return FNS_PASS;
}

//-------------------------------------------------------------------------
// Description:
//  GetSessionEnumerator - Multiple new sessions
//    
// Parameters: N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_SessionManagement_GetSessionEnum_Valid3(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionManager> spAudSessMan;
    CComPtr<IAudioSessionManager2> spAudSessManExtended;
    CComPtr<IAudioSessionEnumerator> pEnumSessions = NULL;

    INT nSessCount = 0;
    const INT NUM_SESSIONS = 5;
    
    // initialize a series of pointers for storing newborn sessionids
    LPWSTR pwszSessionIDs[NUM_SESSIONS];
    for(int i=0;i<NUM_SESSIONS;i++)
    {
        pwszSessionIDs[i] = NULL;
    }

    if (GetModule()->IsAppContainer())
    {
        XLOG(XSKIP, eError, "SKIP: IAudioSessionManager2 activation is blocked in AppContainer. Test case skipping...");
        goto END;
    }
    
    // Get an IAudioSessionManager2 interface pointer
    METHODCHECKRESULT("ITestStreamingEngine:GetAudioSessionManager2",
                      hr,
                      pStreamingEngine->GetAudioSessionManager2(&spAudSessManExtended)); 
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL:  to obtain extended session manager interface");
        goto END;
    }

    // Get an IAudioSessionManager interface pointer
    METHODCHECKRESULT("ITestStreamingEngine:GetAudioSessionManager",
                      hr,
                      pStreamingEngine->GetAudioSessionManager(&spAudSessMan)); 
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL:  to obtain ession manager interface");
        goto END;
    }    

    // create NUM_SESSIONS audiosessions, storing the session ids
    for(int i=0;i<NUM_SESSIONS;i++)
    {
        // create a new audiosession
        METHODCHECKRESULT("CreateDummySessionInProcess",
                          hr,
                          CreateEmptySessionInProcess(spAudSessMan,
                                                      pwszSessionIDs[i]));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Unable to create audiosession!");
            goto END;
        }
    }           

    // wait for the session to arrive
    Sleep(500);

    // Get an audiosession enumerator, and verify that the newly created audiosessions are all
    // enumerated    
    METHODCHECKRESULT("IAudioSessionManager::GetSessionEnumerator",
                      hr,
                      spAudSessManExtended->GetSessionEnumerator(&pEnumSessions));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain session enumerator");
        goto END;
    }

    if(NULL == pEnumSessions)
    {
        XLOG(XFAIL, eError, "FAIL: GetSessionEnumerator returned NULL");
    }

    // get the session count
    METHODCHECKRESULT("IAudioSessionEnumerator::GetCount",
                      hr,
                      pEnumSessions->GetCount(&nSessCount));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    // for each newly created audiosession
    for(int i=0;i<NUM_SESSIONS;i++)
    {        
        BOOL bSessionFound = FALSE;
        // for each enumerated audiosession
        for(int j=0;j<nSessCount;j++)
        {        
 
            BSTR t_wszCurrID = NULL;
            CComPtr<IAudioSessionControl> spCurrSession = NULL;
            CComPtr<IAudioSessionControl2> spCurrSessionEx = NULL;

            METHODCHECKRESULT("IAudioSessionEnumerator::GetSession",
                              hr,
                              pEnumSessions->GetSession(j, &spCurrSession));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: Unable to obtain audiosession control");
                goto END;
            }
            else if(NULL == spCurrSession)
            {
                XLOG(XFAIL, eError, "FAIL: GetSession returned NULL session control!");
                goto END;
                
            }
            else
            {
                SLOG(eInfo1, "Obtained IAudioSessionControl interface");
            }

            METHODCHECKRESULT("IAudioSessionControl::QueryInterface",
                              hr,
                              spCurrSession->QueryInterface(__uuidof(IAudioSessionControl2),
                                                              (void**)&spCurrSessionEx));

            if(FAILED(hr)) 
            { 
                XLOG(XFAIL, eError, "FAIL: unable to obtain IAudioSessionControl2 interface");
                goto END; 
            }

            // get the ID of the audiosession currently under examination
            METHODCHECKRESULT("IAudioSessionControl2::GetSessionInstanceIdentifier",
                              hr,
                              spCurrSessionEx->GetSessionInstanceIdentifier(&t_wszCurrID));

            if(FAILED(hr))
            {
                SLOG(eInfo1, "Unable to obtain audiosession ID");
                goto END;
            }

            if(0 == wcscmp(pwszSessionIDs[i], t_wszCurrID))
            {
                SLOG(eInfo1, "session found");
                bSessionFound = TRUE;
            }
                          
            SAFE_COTASKMEMFREE(t_wszCurrID);

            // this block is here both as an optimization, and to prevent duplicate session IDs
            // from incorrectly passing this test
            if(bSessionFound) 
            { 
                break;
            }
        }

        if(FALSE == bSessionFound)
        {           
            // the session wasn't found
            XLOG(XFAIL, eError, "FAIL: Session ID %ws not found in enumerator", pwszSessionIDs[i]);
            goto END;
        }
        else
        {
            // the session id was found during the last iteration; reset it for the next one
            bSessionFound = FALSE;
        }
    }
   
    
END:
    // free any strings
    for(int i=0;i<NUM_SESSIONS;i++)
    {
        SAFE_COTASKMEMFREE(pwszSessionIDs[i]);
    }
    
    return FNS_PASS;
}

//-------------------------------------------------------------------------
// Description:
//  Basic session arrival notification tests
//    
// Parameters:
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_SessionManagement_AddSessionNotif_Valid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionManager2> spAudSessMan = NULL;
    CComPtr<IAudioSessionNotification> spEventHandler = NULL;

    // Create a session notification event handler
    CSessionManagerEventHandler* pobjEventHandler = NULL;

    if (GetModule()->IsAppContainer())
    {
        XLOG(XSKIP, eError, "SKIP: IAudioSessionManager2 activation is blocked in AppContainer. Test case skipping...");
        goto END;
    }

    // Get a session manager interface
    METHODCHECKRESULT("ITestStreamingEngine::GetAudioSessionManager2",
                      hr,
                      pStreamingEngine->GetAudioSessionManager2(&spAudSessMan));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to get IAudioSessionManager2 interface");
        goto END;
    }

    if(NULL == (pobjEventHandler = new CSessionManagerEventHandler()))
    {
        XLOG(XFAIL, eError, "FAIL: Out of memory!");
        goto END;
    }

    METHODCHECKRESULT("CSessionManagerEventHandler::QueryInterface",
                      hr,
                      pobjEventHandler->QueryInterface(__uuidof(IAudioSessionNotification),
                                                         (void**)(&spEventHandler)));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain audiosession notification event handler");
        goto END;
    }

    // Subscribe to session arrival notification events
    METHODCHECKRESULT("IAudioSessionManager2::RegisterSessionNotification",
                      hr,                      
                      spAudSessMan->RegisterSessionNotification(spEventHandler));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to register for session arrival notifications");
        goto END;
    }
    
      // Unsubscribe to session arrival notification events
    METHODCHECKRESULT("IAudioSessionManager2::UnregisterSessionNotification",
                      hr,                      
                      spAudSessMan->UnregisterSessionNotification(spEventHandler));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to un-register for session arrival notifications");
        goto END;
    }
    
END:
    SAFE_RELEASE_POINTER(pobjEventHandler);
    return FNS_PASS;
}

//-------------------------------------------------------------------------
// Description:
//  Basic session arrival notification tests
//    
// Parameters:
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
DWORD Test_SessionManagement_AddSessionNotif_Valid2(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    BOOL bSubscribedToSessionNotifications = FALSE;

    CComPtr<IAudioSessionControl> spAudSessCtrl = NULL;
    CComPtr<IAudioSessionManager2> spAudSessMan = NULL;
    CComPtr<IAudioSessionNotification> spEventHandler = NULL;
    CComPtr<IAudioSessionEnumerator> spSessionEnumerator = NULL;

    CComPtr<ITestStreamingEngine> spStreamingEngine = NULL;
    CComPtr<IRenderTestStreamingEngine> spRenderTestStreamingEngine = NULL;
    CComPtr<ICaptureTestStreamingEngine> spCaptureTestStreamingEngine = NULL;
    
    CSessionManagerEventHandler* pobjEventHandler = NULL;
    
    LPWSTR wszNewSessionID = NULL;

    if (GetModule()->IsAppContainer())
    {
        XLOG(XSKIP, eError, "SKIP: IAudioSessionManager2 activation is blocked in AppContainer. Test case skipping...");
        goto END;
    }
    
    // Subscribe to session arrival notifications
    METHODCHECKRESULT("ITestStreamingEngine::GetAudioSessionManager2",
                      hr,
                      pStreamingEngine->GetAudioSessionManager2(&spAudSessMan));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to get IAudioSessionManager2 interface");
        goto END;
    }

    // Allocate a session notification event handler
    if(NULL == (pobjEventHandler = new CSessionManagerEventHandler()))
    {
        XLOG(XFAIL, eError, "FAIL: Out of memory!");
        goto END;
    }

    METHODCHECKRESULT("CSessionManagerEventHandler::QueryInterface",
                      hr,
                      pobjEventHandler->QueryInterface(__uuidof(IAudioSessionNotification),
                                                       (void**)(&spEventHandler)));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain audiosession notification event handler");
        goto END;
    }

    // Subscribe to session arrival notification events
    METHODCHECKRESULT("IAudioSessionManager2::RegisterSessionNotification",
                      hr,                      
                      spAudSessMan->RegisterSessionNotification(spEventHandler));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to register for session arrival notifications");
        goto END;
    }

    bSubscribedToSessionNotifications = TRUE;

    // Get a session enumerator so that events will begin flowing
    METHODCHECKRESULT("IAudioSessionManager2::GetSessionEnumerator",
                      hr,
                      spAudSessMan->GetSessionEnumerator(&spSessionEnumerator))
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain AudioSession enumerator!");
        goto END;
    }

    // create a new audiosession, and obtain the new session id
    METHODCHECKRESULT("CreateStreamSessionInProcess",
                      hr,
                      CreateStreamSessionInProcess(&spStreamingEngine, 
                                                   &spRenderTestStreamingEngine, 
                                                   &spCaptureTestStreamingEngine, 
                                                   wszNewSessionID));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: CreateStreamSessionInProcess returned unexpected result.");
        goto END;
    }

    Sleep(500);

    // ensure that the session event handler is healthy    
    if(pobjEventHandler->ErrorReported())
    {
        XLOG(XFAIL, eError, "FAIL: Session Notification handler reports that it is in an unusable state");
        goto END;
    }

    spAudSessCtrl = pobjEventHandler->GetAudioSession(wszNewSessionID);
    if(spAudSessCtrl == NULL)
    {
        XLOG(XFAIL, eError, "FAIL: Session arrival notification not detected!");
    }
    else
    {
        SLOG(eInfo1, "Detected arrival of session %ws", wszNewSessionID);
    }

END:
    // Unsubscribe to notifications, if necessary
    if(bSubscribedToSessionNotifications)
    {
        METHODCHECKRESULT("IAudioSessionManager2::UnregisterSessionNotification",
                          hr,                      
                          spAudSessMan->UnregisterSessionNotification(spEventHandler));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Unable to un-register for session arrival notifications");
        }
    }

    SAFE_RELEASE_POINTER(pobjEventHandler);
    SAFE_COTASKMEMFREE(wszNewSessionID);       

    return FNS_PASS;
}

//-------------------------------------------------------------------------
// Description:
//  Dummy session arrival test
//    
// Parameters:
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_SessionManagement_AddSessionNotif_Valid3(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    BOOL bSubscribedToSessionNotifications = FALSE;

    CComPtr<IAudioSessionControl> spAudSessCtrl = NULL;
    CComPtr<IAudioSessionManager> spAudSessMan = NULL;
    CComPtr<IAudioSessionManager2> spAudSessManExtended = NULL;
    CComPtr<IAudioSessionNotification> spEventHandler = NULL;
    CComPtr<IAudioSessionEnumerator> spSessionEnumerator = NULL;

    CComPtr<ITestStreamingEngine> spStreamingEngine = NULL;
    CComPtr<IRenderTestStreamingEngine> spRenderTestStreamingEngine = NULL;
    CComPtr<ICaptureTestStreamingEngine> spCaptureTestStreamingEngine = NULL;
    
    CSessionManagerEventHandler* pobjEventHandler = NULL;
    
    LPWSTR wszNewSessionID = NULL;

    if (GetModule()->IsAppContainer())
    {
        XLOG(XSKIP, eError, "SKIP: IAudioSessionManager2 activation is blocked in AppContainer. Test case skipping...");
        goto END;
    }

    // Subscribe to session arrival notifications
    METHODCHECKRESULT("ITestStreamingEngine::GetAudioSessionManager",
                      hr,
                      pStreamingEngine->GetAudioSessionManager(&spAudSessMan));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to get IAudioSessionManager interface");
        goto END;
    }

    METHODCHECKRESULT("ITestStreamingEngine::GetAudioSessionManager2",
                      hr,
                      pStreamingEngine->GetAudioSessionManager2(&spAudSessManExtended));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to get IAudioSessionManager2 interface");
        goto END;
    }

    // Allocate a session notification event handler
    if(NULL == (pobjEventHandler = new CSessionManagerEventHandler()))
    {
        XLOG(XFAIL, eError, "FAIL: Out of memory!");
        goto END;
    }

    METHODCHECKRESULT("CSessionManagerEventHandler::QueryInterface",
                      hr,
                      pobjEventHandler->QueryInterface(__uuidof(IAudioSessionNotification),
                                                       (void**)(&spEventHandler)));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain audiosession notification event handler");
        goto END;
    }

    // Subscribe to session arrival notification events
    METHODCHECKRESULT("IAudioSessionManager2::RegisterSessionNotification",
                      hr,                      
                      spAudSessManExtended->RegisterSessionNotification(spEventHandler));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to register for session arrival notifications");
        goto END;
    }

    bSubscribedToSessionNotifications = TRUE;

    // Get a session enumerator so that events will begin flowing
    METHODCHECKRESULT("IAudioSessionManager2::GetSessionEnumerator",
                      hr,
                      spAudSessManExtended->GetSessionEnumerator(&spSessionEnumerator))
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain AudioSession enumerator!");
        goto END;
    }

    // create a new audiosession, and obtain the new session id
    METHODCHECKRESULT("CreateEmptySessionInProcess",
                      hr,
                      CreateEmptySessionInProcess(spAudSessMan,
                                                  wszNewSessionID));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to create empty audiosession in current process");
    }

    Sleep(500);

    // ensure that the session event handler is healthy    
    if(pobjEventHandler->ErrorReported())
    {
        XLOG(XFAIL, eError, "FAIL: Session Notification handler reports that it is in an unusable state");
        goto END;
    }

    spAudSessCtrl = pobjEventHandler->GetAudioSession(wszNewSessionID);
    if(spAudSessCtrl == NULL)
    {
        XLOG(XFAIL, eError, "FAIL: Session arrival notification not detected!");
    }
    else
    {
        SLOG(eInfo1, "Detected arrival of session %ws", wszNewSessionID);
    }

END:
    // Unsubscribe to notifications, if necessary
    if(bSubscribedToSessionNotifications)
    {
        METHODCHECKRESULT("IAudioSessionManager2::UnregisterSessionNotification",
                          hr,                      
                          spAudSessManExtended->UnregisterSessionNotification(spEventHandler));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Unable to un-register for session arrival notifications");
        }
    }

    SAFE_RELEASE_POINTER(pobjEventHandler);
    SAFE_COTASKMEMFREE(wszNewSessionID);       

    return FNS_PASS;
}

//-------------------------------------------------------------------------
// Description:
//  Multiple dummy session arrivals
//    
// Parameters:
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_SessionManagement_AddSessionNotif_Valid4(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    BOOL bSubscribedToSessionNotifications = FALSE;

    CComPtr<IAudioSessionControl> spAudSessCtrl = NULL;
    CComPtr<IAudioSessionManager> spAudSessMan = NULL;
    CComPtr<IAudioSessionManager2> spAudSessManExtended = NULL;
    CComPtr<IAudioSessionNotification> spEventHandler = NULL;
    CComPtr<IAudioSessionEnumerator> spSessionEnumerator = NULL;

    CComPtr<ITestStreamingEngine> spStreamingEngine = NULL;
    CComPtr<IRenderTestStreamingEngine> spRenderTestStreamingEngine = NULL;
    CComPtr<ICaptureTestStreamingEngine> spCaptureTestStreamingEngine = NULL;
    
    CSessionManagerEventHandler* pobjEventHandler = NULL;

    const INT NUM_SESSIONS = 5;
    LPWSTR awszNewSessionIDs[NUM_SESSIONS];
    for(UINT32 i=0;i<NUM_SESSIONS;i++)
    {
        awszNewSessionIDs[i] = NULL;
    }

    if (GetModule()->IsAppContainer())
    {
        XLOG(XSKIP, eError, "SKIP: IAudioSessionManager2 activation is blocked in AppContainer. Test case skipping...");
        goto END;
    }

    // Subscribe to session arrival notifications
    METHODCHECKRESULT("ITestStreamingEngine::GetAudioSessionManager",
                      hr,
                      pStreamingEngine->GetAudioSessionManager(&spAudSessMan));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to get IAudioSessionManager interface");
        goto END;
    }

    METHODCHECKRESULT("ITestStreamingEngine::GetAudioSessionManager2",
                      hr,
                      pStreamingEngine->GetAudioSessionManager2(&spAudSessManExtended));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to get IAudioSessionManager2 interface");
        goto END;
    }

    // Allocate a session notification event handler
    if(NULL == (pobjEventHandler = new CSessionManagerEventHandler()))
    {
        XLOG(XFAIL, eError, "FAIL: Out of memory!");
        goto END;
    }

    METHODCHECKRESULT("CSessionManagerEventHandler::QueryInterface",
                      hr,
                      pobjEventHandler->QueryInterface(__uuidof(IAudioSessionNotification),
                                                       (void**)(&spEventHandler)));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain audiosession notification event handler");
        goto END;
    }

    // Subscribe to session arrival notification events
    METHODCHECKRESULT("IAudioSessionManager2::RegisterSessionNotification",
                      hr,                      
                      spAudSessManExtended->RegisterSessionNotification(spEventHandler));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to register for session arrival notifications");
        goto END;
    }

    bSubscribedToSessionNotifications = TRUE;

    // Get a session enumerator so that events will begin flowing
    METHODCHECKRESULT("IAudioSessionManager2::GetSessionEnumerator",
                      hr,
                      spAudSessManExtended->GetSessionEnumerator(&spSessionEnumerator))
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain AudioSession enumerator!");
        goto END;
    }

    for(UINT32 i=0;i<NUM_SESSIONS;i++)
    {
        // create a new audiosession, and obtain the new session id
        METHODCHECKRESULT("CreateEmptySessionInProcess",
                          hr,
                          CreateEmptySessionInProcess(spAudSessMan,
                                                      awszNewSessionIDs[i]));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Unable to create empty audiosession in current process");
        }
    }

    Sleep(500);

    // ensure that the session event handler is healthy    
    if(pobjEventHandler->ErrorReported())
    {
        XLOG(XFAIL, eError, "FAIL: Session Notification handler reports that it is in an unusable state");
        goto END;
    }

    for(UINT32 i=0;i<NUM_SESSIONS;i++)
    {
        spAudSessCtrl = pobjEventHandler->GetAudioSession(awszNewSessionIDs[i]);
        if(spAudSessCtrl == NULL)
        {
            XLOG(XFAIL, eError, "FAIL: Session arrival notification not detected!");
        }
        else
        {
            SLOG(eInfo1, "Detected arrival of session %ws", awszNewSessionIDs[i]);
        }
    }
    
END:
    // Unsubscribe to notifications, if necessary
    if(bSubscribedToSessionNotifications)
    {
        METHODCHECKRESULT("IAudioSessionManager2::UnregisterSessionNotification",
                          hr,                      
                          spAudSessManExtended->UnregisterSessionNotification(spEventHandler));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Unable to un-register for session arrival notifications");
        }
    }

    SAFE_RELEASE_POINTER(pobjEventHandler);

    for(UINT32 i=0;i<NUM_SESSIONS;i++)
    {
        SAFE_COTASKMEMFREE(awszNewSessionIDs[i]);       
    }

    return FNS_PASS;
}

//-------------------------------------------------------------------------
// Description:
//  Verify session arrival notifications cease after unsubscription
//    
// Parameters:
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_SessionManagement_DeleteSessionNotif_Valid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;

    CComPtr<IAudioSessionControl> spAudSessCtrl = NULL;
    CComPtr<IAudioSessionManager> spAudSessMan = NULL;
    CComPtr<IAudioSessionManager2> spAudSessManExtended = NULL;
    CComPtr<IAudioSessionNotification> spEventHandler = NULL;
    CComPtr<IAudioSessionEnumerator> spSessionEnumerator = NULL;

    CComPtr<ITestStreamingEngine> spStreamingEngine = NULL;
    CComPtr<IRenderTestStreamingEngine> spRenderTestStreamingEngine = NULL;
    CComPtr<ICaptureTestStreamingEngine> spCaptureTestStreamingEngine = NULL;

    CSessionManagerEventHandler* pobjEventHandler = NULL;
    LPWSTR pwszNewSessionID = NULL;

    if (GetModule()->IsAppContainer())
    {
        XLOG(XSKIP, eError, "SKIP: IAudioSessionManager2 activation is blocked in AppContainer. Test case skipping...");
        goto END;
    }

    // Subscribe to session arrival notifications
    METHODCHECKRESULT("ITestStreamingEngine::GetAudioSessionManager",
                      hr,
                      pStreamingEngine->GetAudioSessionManager(&spAudSessMan));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to get IAudioSessionManager interface");
        goto END;
    }

    METHODCHECKRESULT("ITestStreamingEngine::GetAudioSessionManager2",
                      hr,
                      pStreamingEngine->GetAudioSessionManager2(&spAudSessManExtended));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to get IAudioSessionManager2 interface");
        goto END;
    }

    // Allocate a session notification event handler
    if(NULL == (pobjEventHandler = new CSessionManagerEventHandler()))
    {
        XLOG(XFAIL, eError, "FAIL: Out of memory!");
        goto END;
    }

    METHODCHECKRESULT("CSessionManagerEventHandler::QueryInterface",
                      hr,
                      pobjEventHandler->QueryInterface(__uuidof(IAudioSessionNotification),
                                                       (void**)(&spEventHandler)));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain audiosession notification event handler");
        goto END;
    }

    // Subscribe to session arrival notification events
    METHODCHECKRESULT("IAudioSessionManager2::RegisterSessionNotification",
                      hr,                      
                      spAudSessManExtended->RegisterSessionNotification(spEventHandler));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to register for session arrival notifications");
        goto END;
    }

    // Get a session enumerator so that events will begin flowing
    METHODCHECKRESULT("IAudioSessionManager2::GetSessionEnumerator",
                      hr,
                      spAudSessManExtended->GetSessionEnumerator(&spSessionEnumerator))
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain AudioSession enumerator!");
        goto END;
    }

    // Immediately un-subscribe to session arrival notification events
    METHODCHECKRESULT("IAudioSessionManager2::UnregisterSessionNotification",
                      hr,                      
                      spAudSessManExtended->UnregisterSessionNotification(spEventHandler));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to un-register for session arrival notifications");
    }    

    // create a new audiosession, and obtain the new session id
    METHODCHECKRESULT("CreateEmptySessionInProcess",
                      hr,
                      CreateEmptySessionInProcess(spAudSessMan,
                                                  pwszNewSessionID));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to create empty audiosession in current process");
    }
    
    Sleep(1000);

    // ensure that the session event handler is healthy    
    if(pobjEventHandler->ErrorReported())
    {
        XLOG(XFAIL, eError, "FAIL: Session Notification handler reports that it is in an unusable state");
        goto END;
    }
 
    spAudSessCtrl = pobjEventHandler->GetAudioSession(pwszNewSessionID);
    if(spAudSessCtrl != NULL)
    {
        XLOG(XFAIL, eError, "FAIL: Session %d arrival notification _after_ unsubscription!", pwszNewSessionID);
    }
 
END:
    SAFE_RELEASE_POINTER(pobjEventHandler);
    SAFE_COTASKMEMFREE(pwszNewSessionID);       
    
    return FNS_PASS;
}

//-------------------------------------------------------------------------
// Description:
//  Basic GetAudioSession tests
//    
// Parameters:
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_SessionManagement_GetSession_Valid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    DWORD dwRes = FNS_PASS;
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionManager> spAudSessMan;
    LPWSTR wszNewSessionID = NULL;

    METHODCHECKRESULT("ITestStreamingEngine::GetAudioSessionManager",
                      hr,
                      pStreamingEngine->GetAudioSessionManager(&spAudSessMan));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain AudioSessionManager interface");
        goto END;
    }

    // Create a new audiosession in the current process using IAudioSessionManager::GetAudioSession
    METHODCHECKRESULT("CreateEmptySessionInProcess",
                      hr,
                      CreateEmptySessionInProcess(spAudSessMan, wszNewSessionID));

    if((FAILED(hr))||(NULL == wszNewSessionID))
    {
        XLOG(XFAIL, eError, "FAIL: GetAudioSession call failed!");
    }

END:
    SAFE_COTASKMEMFREE(wszNewSessionID);
    return dwRes;
}

//-------------------------------------------------------------------------
// Description:
//  Obtain a control for a pre-existing AudioSession
//    
// Parameters:
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//

DWORD Test_SessionManagement_GetSession_Valid2(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;

    CComPtr<IAudioSessionManager> spAudSessMan;
    CComPtr<IAudioSessionControl> spAudSessCtrl1 = NULL, spAudSessCtrl2 = NULL;
    CComPtr<IAudioSessionControl2> spAudSessCtrlEx = NULL;

    LPWSTR wszNewSessionID1 = NULL, wszNewSessionID2 = NULL;
    
    GUID newGuid = GUID_NULL;

    if (GetModule()->IsAppContainer())
    {
        XLOG(XSKIP, eError, "SKIP: IAudioSessionManager2 activation is blocked in AppContainer. Test case skipping...");
        goto END;
    }

    // Get an audiosession manager interface
    METHODCHECKRESULT("ITestStreamingEngine::GetAudioSessionManager",
                      hr,
                      pStreamingEngine->GetAudioSessionManager(&spAudSessMan));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionManager interface");
        goto END;
    }
    
    // get a new guid to ensure that a new audiosession is created
    METHODCHECKRESULT("::CoCreateGuid",
                      hr,
                      ::CoCreateGuid(&newGuid));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error creating new guid");
        goto END;
    }
    
    METHODCHECKRESULT("IAudioSessionManager::GetAudioSessionControl",
                      hr,
                      spAudSessMan->GetAudioSessionControl(&newGuid, FALSE, &spAudSessCtrl1));
    if(FAILED(hr)||(spAudSessCtrl1 == NULL))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");     
        goto END;
    }

    // get the ID associated with the IAudioSessionControl
    METHODCHECKRESULT("IAudioSessionControl::QueryInterface",
                      hr,
                      spAudSessCtrl1->QueryInterface(__uuidof(IAudioSessionControl2),
                      (void**)&spAudSessCtrlEx));

    if(FAILED(hr)) 
    { 
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT"); 
        goto END; 
    }

    METHODCHECKRESULT("IAudioSessionControl2::GetSessionInstanceIdentifier",
                      hr,
                      spAudSessCtrlEx->GetSessionInstanceIdentifier(&wszNewSessionID1));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eInfo1, "FAIL: Unable to obtain audiosession ID");
        goto END;
    }
    else
    {
        SLOG(eInfo1, "New AudioSession ID: %ws", wszNewSessionID1);
    }

    spAudSessCtrlEx = NULL;

    // Obtain a new IAudioSessionControl associated with the previously created session.
    METHODCHECKRESULT("IAudioSessionManager::GetAudioSessionControl",
                      hr,
                      spAudSessMan->GetAudioSessionControl(&newGuid, FALSE, &spAudSessCtrl2));
    if(FAILED(hr)||(spAudSessCtrl1 == NULL))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");     
        goto END;
    }

    // get the ID associated with the IAudioSessionControl
    METHODCHECKRESULT("IAudioSessionControl::QueryInterface",
                      hr,
                      spAudSessCtrl2->QueryInterface(__uuidof(IAudioSessionControl2),
                      (void**)&spAudSessCtrlEx));

    if(FAILED(hr)) 
    { 
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT"); 
        goto END; 
    }

    METHODCHECKRESULT("IAudioSessionControl2::GetSessionInstanceIdentifier",
                      hr,
                      spAudSessCtrlEx->GetSessionInstanceIdentifier(&wszNewSessionID2));
    if(FAILED(hr) || (NULL == wszNewSessionID1))
    {
        XLOG(XFAIL, eInfo1, "FAIL: Unable to obtain audiosession ID");
        goto END;
    }
    else
    {
        SLOG(eInfo1, "New AudioSession ID: %ws", wszNewSessionID1);
    }


    // verify that the session ids for the two sessions are identical
    if(0 != wcscmp(wszNewSessionID1, wszNewSessionID2))
    {
        SLOG(eFatalError, "Session IDs GetAudioSession calls with the same GUID do not match!");
        goto END;
    }

END:
    return FNS_PASS;
}

//-------------------------------------------------------------------------
// Description:
//  Create multiple empty sessions in the current process
//    
// Parameters:
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_SessionManagement_GetSession_Valid3(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    const INT NUM_SESSIONS = 5;

    CComPtr<IAudioSessionManager> spAudSessMan;
    LPWSTR wszNewSessionID = NULL;

    METHODCHECKRESULT("ITestStreamingEngine::GetAudioSessionManager",
                      hr,
                      pStreamingEngine->GetAudioSessionManager(&spAudSessMan));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: unable to obtain IAudioSessionManager interface");
        goto END;
    }
    
    for(UINT32 i=0;i<NUM_SESSIONS;i++)
    {
        // Create a new audiosession in the current process using IAudioSessionManager::GetAudioSession
        METHODCHECKRESULT("CreateEmptySessionInProcess",
                          hr,
                          CreateEmptySessionInProcess(spAudSessMan, wszNewSessionID));
        if((FAILED(hr))||(NULL == wszNewSessionID))
        {
            XLOG(XFAIL, eFatalError, "FAIL: Unable to create empty AudioSession");   
        }

        SAFE_COTASKMEMFREE(wszNewSessionID);
    }

END:
    return FNS_PASS;
}

//-------------------------------------------------------------------------
// Description:
//  Extended GetAudiosession tests
//    
// Parameters:
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_SessionManagement_GetSession_Valid4(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    
    CComPtr<IAudioSessionManager> spAudSessMan;
    CComPtr<IAudioSessionControl> spAudSessCtrl, spCurrAudSessCtrl = NULL;
    CComPtr<IAudioSessionControl2> spAudSessCtrlEx = NULL;
    LPWSTR wszNewSessionID = NULL, wszCurrSessionID = NULL;

    const UINT NUM_SESSIONS = 5;
    GUID newGuid;

    METHODCHECKRESULT("ITestStreamingEngine:GetAudioSessionManager",
                      hr,
                      pStreamingEngine->GetAudioSessionManager(&spAudSessMan));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionManager");
        goto END;
    }

    // get a new guid to ensure that a new audiosession is created
    METHODCHECKRESULT("CoCreateGuid",
                      hr,
                      ::CoCreateGuid(&newGuid));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error creating new guid");
        goto END;
    }
    
    // Get the ID of the newly created session
    METHODCHECKRESULT("IAudioSessionManager::GetAudioSessionControl",
                      hr,
                      spAudSessMan->GetAudioSessionControl(&newGuid, FALSE, &spAudSessCtrl));
    if(FAILED(hr)||(spAudSessCtrl == NULL))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");       
        goto END;
    }

    METHODCHECKRESULT("IAudioSessionControl::QueryInterface",
                      hr,
                      spAudSessCtrl->QueryInterface(__uuidof(IAudioSessionControl2),
                                                    (void**)&spAudSessCtrlEx));

    if(FAILED(hr)) 
    { 
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END; 
    }

    METHODCHECKRESULT("IAudioSessionControl2::GetSessionInstanceIdentifier",
                      hr,
                      spAudSessCtrlEx->GetSessionInstanceIdentifier(&wszNewSessionID));
    if(FAILED(hr) || (NULL == wszNewSessionID))
    {
        XLOG(XFAIL, eError, "Unable to obtain audiosession ID");
        goto END;
    }
    else
    {
        SLOG(eInfo1, "New AudioSession ID: %ws", wszNewSessionID);
    }

    for(int i=0;i<NUM_SESSIONS;i++)
    {
        CComPtr<IAudioSessionControl> spControl = NULL;
        CComPtr<IAudioSessionControl2> spControlEx = NULL;
        
        // now that we've created new session, call get on it multiple times
        METHODCHECKRESULT("IAudioSessionManager::GetAudioSession",
                          hr,
                          spAudSessMan->GetAudioSessionControl(&newGuid, FALSE, &spControl));
        if(FAILED(hr)||(spControl == NULL))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            continue;
        }

        METHODCHECKRESULT("IAudioSessionControl::QueryInterface",
                          hr,
                          spControl->QueryInterface(__uuidof(IAudioSessionControl2),
                                                    (void**)&spControlEx));
        if(FAILED(hr)) { 
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            continue;
        }
   
        // get the ID of the audiosession
        METHODCHECKRESULT("IAudioSessionControl2::GetSessionInstanceIdentifier",
                          hr,
                          spControlEx->GetSessionInstanceIdentifier(&wszCurrSessionID));
        if(FAILED(hr) || (NULL == wszCurrSessionID))
        {
            XLOG(XFAIL, eError, "FAIL: Unable to obtain audiosession ID");
            continue;
        }
        else
        {
            SLOG(eInfo1, "New AudioSession ID: %ws", wszCurrSessionID);
        }

        // verify that the session ids for the two sessions are identical
        if(0 != wcscmp(wszCurrSessionID, wszNewSessionID))
        {
            XLOG(XFAIL, eError, "FAIL: Session IDs GetAudioSession calls with the same GUID do not match!");
        }

        SAFE_COTASKMEMFREE(wszCurrSessionID);
    }

END:
    SAFE_COTASKMEMFREE(wszCurrSessionID);
    SAFE_COTASKMEMFREE(wszNewSessionID);

    return FNS_PASS;
}

//-------------------------------------------------------------------------
// Description:
//  RegisterSessionNotification NULL param tests
//    
// Parameters:
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_SessionManagement_AddSessionNotif_Invalid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionManager2> spSesManEx = NULL;

    if (GetModule()->IsAppContainer())
    {
        XLOG(XSKIP, eError, "SKIP: IAudioSessionManager2 activation is blocked in AppContainer. Test case skipping...");
        goto END;
    }

    METHODCHECKRESULT("ITestStreamingEngine::GetAudioSessionManager2",
                      hr,
                      pStreamingEngine->GetAudioSessionManager2(&spSesManEx));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionManager2");
        goto END;
    }

    // make an erroneous subscription request
    METHODCHECKRESULT("IAudioSessionManager::AddSessionNotification",
                      hr,
                      spSesManEx->RegisterSessionNotification(NULL)); 
    if(SUCCEEDED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test should not have succeeded for NULL param");
        goto END;
    }

END:
    return FNS_PASS;
}

//-------------------------------------------------------------------------
// Description:
//  UnregisterSessionNotification NULL param tests
//    
// Parameters:
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_SessionManagement_DeleteSessionNotif_Invalid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionManager2> spSesManEx = NULL;

    if (GetModule()->IsAppContainer())
    {
        XLOG(XSKIP, eError, "SKIP: IAudioSessionManager2 activation is blocked in AppContainer. Test case skipping...");
        goto END;
    }

    METHODCHECKRESULT("ITestStreamingEngine::GetAudioSessionManager2",
                      hr,
                      pStreamingEngine->GetAudioSessionManager2(&spSesManEx));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionManager2");
        goto END;
    }

    // make an erroneous subscription request
    METHODCHECKRESULT("IAudioSessionManager::AddSessionNotification",
                      hr,
                      spSesManEx->UnregisterSessionNotification(NULL)); 
    if(SUCCEEDED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test should not have succeeded for NULL param");
        goto END;
    }

END:
    return FNS_PASS;
}

DWORD Test_SessionManagement_DisconnectService(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;

    CComPtr<IAudioSessionManager> spAudSessMan = NULL;
    CComPtr<IAudioSessionControl> spAudSessCtrl = NULL;
    GUID newGuid = GUID_NULL;

    METHODCHECKRESULT("ITestStreamingEngine::GetAudioSessionManager",
                      hr,
                      pStreamingEngine->GetAudioSessionManager(&spAudSessMan));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionManager");
        goto END;
    }

    // shut down audiosrv
    if (!StopAudioService())
    {
        XLOG(XFAIL, eError, "FAIL: Unable to shut down AudioSrv");
        goto END;
    }

    // BEGIN IAudioSessionManager API Calls
    ////

    // 
    METHODCHECKRESULT("ISimpleAudioVolume:GetAudioSession",
                      hr,
                      spAudSessMan->GetAudioSessionControl(&newGuid, FALSE, &spAudSessCtrl));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should not have succeeded");
    }    
           
    ////
    // END IAudioSessionManager API Calls

END:
    if (!StartAudioService())
    {
        XLOG(XFAIL, eFatalError, "ERROR: Unable to re-start AudioSrv! System may be in an unusable state!");
    }

    return FNS_PASS;
}
