// Copyright(C) Microsoft. All rights reserved.

//
// DuckingTests.cpp
//  
//
// Description:
//
//  WASAPI Ducking API Tests

#include <stdafx.h>
#include <UMATestStreaming.h>
#include <OOPNotify.h>
#include "TestPal.h"
#include <deviceaccesshelper.h>


#include "DummyDuckingClient.h"

const FLOAT DEFAULT_TEST_TONE_FREQ = 500.0f;
const FLOAT COMM_TEST_TONE_FREQ = 250.0f;
const FLOAT TEST_TONE_AMP = 0.01f; // play nice and quiet
const FLOAT DUCK_FULL_ATTENUATION = 0.000016f;
const FLOAT DUCK_NO_ATTENUATION = 1.0f;
const FLOAT EPSILON = .00001f;

const UINT32 MULTI_SESSION_COUNT = 3;

DWORD TC_DuckingNotification_VerifySubscrNotif_Generic(bool bExplicitSubscribe,
                                                       UINT32 nCommSessions=1);

using namespace OOPNotify;

//------------------------------------------------------------------------------
// Desc: Helper function to print ducking notification data.
//
// Params:
//              pInfo [in] - Structure containing the notification data.
//
// Returns:
//              N/A
//------------------------------------------------------------------------------
void DumpDuckNotifyInfo(MM_AVDN_NotifyInfo sInfo)
{
    SLOG(eInfo1, "ducking_event:    MM_AVDN_NotifyInfo:\n");
    SLOG(eInfo1, "ducking_event:      szwDuckCommSessionID:           [%ls]", sInfo.szwDuckCommSessionID);
    SLOG(eInfo1, "ducking_event:      countCommunicationsSessions:    [%d]", sInfo.countCommunicationsSessions);
    SLOG(eInfo1, "ducking_event:      szwUnduckCommSessionID:         [%ls]", sInfo.szwUnduckCommSessionID);
    SLOG(eInfo1, "ducking_event:      cDuckNotifications:             [%d]", sInfo.cDuckNotifications);
    SLOG(eInfo1, "ducking_event:      cUnduckNotifications:           [%d]", sInfo.cUnduckNotifications);
}

//------------------------------------------------------------------------------
// Desc: Basic ducking notification subscribe/unsubscribe verification
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD TC_DuckingNotification_BasicSubscription()
{
    if(SkipDuckingTests())
    {
        XLOG(XSKIP, eError, "Ducking tests not applicable for platform.");
        return FNS_SKIPPED;
    }

    HRESULT                                 hr = S_OK;
    AudioClientProperties                   clientProperties = {0};

    CComPtr<IBasicLog>                      spLogger = NULL;

    CComPtr<IAudioVolumeDuckNotification>   spDuckNotify = NULL;
    CComPtr<IMMDevice>                      spDeviceUnderTest = NULL;
    CComPtr<IAudioSessionManager>           spSessionManager = NULL;
    CComPtr<IAudioSessionManager2>          spSessionManager2 = NULL;
    CComPtr<IAudioSessionControl2>          spSessionControl2 = NULL;

    CComPtr<ITestStreamingEngine>           spStreamingEngineCommon = NULL;
    CComPtr<IRenderTestStreamingEngine>     spRenderStreamingEngine = NULL;
    CComPtr<ICaptureTestStreamingEngine>    spCaptureStreamingEngine = NULL;
    
    CDummyDuckingNotificationClient*        pobjDuckNotifyClient = NULL;
    CComHeapPtr<WCHAR>                      sp_wszSessionID;

    if (GetModule()->IsAppContainer())
    {
        XLOG(XSKIP, eError, "IAudioSessionManager2 activation is blocked in AppContainer. Test case skipping...");
        return FNS_SKIPPED;
    }

    clientProperties.bIsOffload = FALSE;
    clientProperties.eCategory = AudioCategory_Other;
    clientProperties.cbSize = sizeof(clientProperties);

    //
    //  Get an IBasicLog
    //
    hr = g_IShell->GetBasicLog(&spLogger);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to create IBasicLog!");
        return FNS_PASS;
    }

    //
    //  Get the endpoint under test
    //
    hr = GetEndpointUnderTest(&spDeviceUnderTest);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: GetEndpointUnderTest! 0x%08X", hr);
        return FNS_PASS;
    }

    if (spDeviceUnderTest == NULL)
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IMMDevice under test!");
        return FNS_PASS;
    }

    //
    //  Create a session on the endpoint under test
    //
    if (DEVICE_TYPE_RENDER == GetDeviceTypeUnderTest() || DEVICE_TYPE_RENDEROFFLOAD == GetDeviceTypeUnderTest())
    {
        clientProperties.bIsOffload = (DEVICE_TYPE_RENDEROFFLOAD == GetDeviceTypeUnderTest() ? TRUE : FALSE);
        clientProperties.eCategory = (DEVICE_TYPE_RENDEROFFLOAD == GetDeviceTypeUnderTest() ? AudioCategory_Media : AudioCategory_Other);
        hr = CreateTestStreamingEngineOnMMDevice(__uuidof(IRenderTestStreamingEngine),
                                       spDeviceUnderTest,
                                       TIMER_PUSH_RENDER,
                                       spLogger,
                                       reinterpret_cast<void**>(&spRenderStreamingEngine));
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: CreateTestStreamingEngineOnMMDevice(IID_IRenderTestStreamingEngine) returned 0x%08X", hr);
            return FNS_PASS;
        }

        hr = spRenderStreamingEngine->QueryInterface(__uuidof(ITestStreamingEngine),
                                                     reinterpret_cast<void**>(&spStreamingEngineCommon));
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: IRenderTestStreamingEngine::QueryInterface(IID_ITestStreamingEngine) returned 0x%08X", hr);
            return FNS_PASS;
        }
    }
    else if (DEVICE_TYPE_CAPTURE == GetDeviceTypeUnderTest())
    {
        hr = CreateTestStreamingEngineOnMMDevice(__uuidof(ICaptureTestStreamingEngine),
                                       spDeviceUnderTest,
                                       TIMER_PUSH_CAPTURE,
                                       spLogger,
                                       reinterpret_cast<void**>(&spCaptureStreamingEngine));
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: CreateTestStreamingEngineOnMMDevice(IID_ICaptureTestStreamingEngine) returned 0x%08X", hr);
            return FNS_PASS;
        }

        hr = spCaptureStreamingEngine->QueryInterface(__uuidof(ITestStreamingEngine),
                                                      reinterpret_cast<void**>(&spStreamingEngineCommon));
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: IRenderTestStreamingEngine::QueryInterface(IID_ITestStreamingEngine) returned 0x%08X", hr);
            return FNS_PASS;
        }
    }
    else
    {
        ATLASSERT(!"this should never happen");
        XLOG(XFAIL, eError, "FAIL: Encountered unknown device type!");
        return FNS_PASS;
    }

    hr = spStreamingEngineCommon->Initialize(&clientProperties);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: ITestStreamingEngine::Initialize returned 0x%08X", hr);
        return FNS_PASS;
    }

    hr = spStreamingEngineCommon->GetSessionControlExtended(&spSessionControl2);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: ITestStreamingEngine::GetSessionControlExtended returned 0x%08x", hr);
        return FNS_PASS;
    }

    hr = spSessionControl2->GetSessionIdentifier(&sp_wszSessionID);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IAudioSessionControl2::GetSessionIdentifier returned 0x%08x", hr);
        return FNS_PASS;
    }

    //
    //  Create a dummy notification handler
    //
    pobjDuckNotifyClient = new CDummyDuckingNotificationClient();
    if (NULL == pobjDuckNotifyClient)
    {
        XLOG(XFAIL, eError, "FAIL: Out of memory!");
        return FNS_PASS;
    }
    spDuckNotify.Attach(pobjDuckNotifyClient);

    //
    //  Activate an IAudioSessionManager and get an IAudioSessionManager2 pointer
    //
    hr = spDeviceUnderTest->Activate(__uuidof(IAudioSessionManager),
                                     CLSCTX_ALL,
                                     NULL,
                                     reinterpret_cast<void**>(&spSessionManager));
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IMMDevice::Activate(IID_IAudioSessionManager) returned 0x%08X", hr);
        return FNS_PASS;
    }

    hr = spSessionManager->QueryInterface(__uuidof(IAudioSessionManager2),
                                          reinterpret_cast<void**>(&spSessionManager2));
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IAudioSessionManager::QueryInterface(IID_IAudioSessionManager2) returne 0x%08x", hr);
        return FNS_PASS;
    }

    //
    //  Subscribe/Unsubscribe to ducking notifications for all sessions
    //
    
    //  Subscribe to notifications
    XLOG(XMSG, eBlab1, "Subscribing...");
    hr = spSessionManager2->RegisterDuckNotification(NULL, spDuckNotify);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IAudioSessionManager2::RegisterDuckNotification(NULL, Client) returned 0x%08x", hr);
    }

    //  Unsubscribe to notification
    XLOG(XMSG, eBlab1, "Unsubscribing...");
    hr = spSessionManager2->UnregisterDuckNotification(spDuckNotify);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IAudioSessionManager2::UnregisterDuckNotification(pClient) returned 0x%08x", hr);
    }

    //
    //  Subscribe/unsubscribe to ducking notifications for a specific session
    //
    XLOG(XMSG, eBlab1, "Subscribing to notifications for session \'%S\'...", (LPCWSTR)sp_wszSessionID);
    hr = spSessionManager2->RegisterDuckNotification(sp_wszSessionID, spDuckNotify);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IAudioSessionManager2::RegisterDuckNotification(SessionID, Client) returned 0x%08x", hr);
    }

    //  Unsubscribe to notification
    XLOG(XMSG, eBlab1, "Unsubscribing...");
    hr = spSessionManager2->UnregisterDuckNotification(spDuckNotify);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IAudioSessionManager2::UnregisterDuckNotification(pClient) returned 0x%08x", hr);
    }    
    
    return FNS_PASS;
}


//------------------------------------------------------------------------------
// Desc: Verify duck/unduck notifications are received for subscriptions with
//       explicit audiosession selection
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD TC_DuckingNotification_VerifyExplicitSubscrNotif()
{
    return TC_DuckingNotification_VerifySubscrNotif_Generic(true);
}

//------------------------------------------------------------------------------
// Desc: Verify duck/unduck notifications are received for subscriptions with
//       implicit audiosession selection
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD TC_DuckingNotification_VerifyImplicitSubscrNotif()
{
    return TC_DuckingNotification_VerifySubscrNotif_Generic(false);
}

//------------------------------------------------------------------------------
// Desc: Verify duck/unduck notifications are received for multiple sessions
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD TC_DuckingNotification_MultipleCommSessions()
{
    return TC_DuckingNotification_VerifySubscrNotif_Generic(true, MULTI_SESSION_COUNT);
}

//------------------------------------------------------------------------------
// Desc: Verify duck/unduck notifications are received for subscriptions with
//       implicit or explicit subscriptions.
//
// Params:
//              bExplicitSubscribe  -  True IFF ducking notifications subscriptions
//                                     should be made using an explicit session ID.
//
//              nCommSessions       -  The number of communications scenarios to
//                                     include in the scenario
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD TC_DuckingNotification_VerifySubscrNotif_Generic(bool bExplicitSubscribe,
                                                       UINT32 nCommSessions)
{
    if(SkipDuckingTests())
    {
        XLOG(XSKIP, eError, "Ducking tests not applicable for platform.");
        return FNS_SKIPPED;
    }

    HRESULT                                                     hr = S_OK;
    AudioClientProperties                                       clientProperties = {0};

    CComPtr<IBasicLog>                                          spLogger = NULL;
    MM_AVDN_NotifyInfo                                          sDuckInfo = {0};
    CComPtr<IAudioVolumeDuckNotificationHelper>                 spDuckHelper = NULL;    

    CComPtr<IMMDevice>                                          spDeviceUnderTest = NULL;    
    CComPtr<IMMDeviceEnumerator>                                spDeviceEnumerator = NULL;
    CComPtr<IMMDevice>                                          spDefaultCommDevice = NULL;
    CComPtr<IMMDevice>                                          spDefaultConsoleDevice = NULL;

    // "Non-communications" session supporting interfaces
    CComPtr<ITestStreamingEngine>                               spNonCommStreamingEngineCommon = NULL;
    CComPtr<IRenderTestStreamingEngine>                         spNonCommRenderStreamingEngine = NULL;
    CComPtr<IAudioSessionControl2>                              spSessionControl2 = NULL;    

    // "Communications" session supporting interfaces
    CInterfaceArray<ITestStreamingEngine>                       aspCommStreamingEngineCommon;
    CInterfaceArray<IRenderTestStreamingEngine>                 aspCommRenderStreamingEngine;
    CInterfaceArray<ICaptureTestStreamingEngine>                aspCommCaptureStreamingEngine;
    CInterfaceArray<IAudioSessionControl2>                      aspCommSessionControl2;    
    

    CComHeapPtr<WCHAR>                                          sp_wszNonCommSessionID;
    CComHeapPtr<CComHeapPtr<WCHAR>>                             asp_wszCommSessionID;
    CComHeapPtr<GUID>                                           aguidCommSessionGuid;

    CComHeapPtr<WCHAR>                                          sp_wszEndpointUnderTest;
    CComHeapPtr<WCHAR>                                          sp_wszDefaultCommDevice;

    clientProperties.bIsOffload = FALSE;
    clientProperties.eCategory = AudioCategory_Other;
    clientProperties.cbSize = sizeof(clientProperties);

    if (GetModule()->IsAppContainer())
    {
        XLOG(XSKIP, eError, "IAudioSessionManager2 activation is blocked in AppContainer. Test case skipping...");
        return FNS_SKIPPED;
    }

    //
    //  Get an IBasicLog
    //
    hr = g_IShell->GetBasicLog(&spLogger);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to create IBasicLog!");
        return FNS_PASS;
    }

    //  Allocate space for the communications stream interfaces
    for (UINT32 i=0; i<nCommSessions; i++)
    {
        aspCommStreamingEngineCommon.Add(NULL);
        aspCommRenderStreamingEngine.Add(NULL);
        aspCommCaptureStreamingEngine.Add(NULL);
        aspCommSessionControl2.Add(NULL);
    }

    //  Allocate space for communications stream GUIDs and Session IDs
    aguidCommSessionGuid.Allocate(nCommSessions*sizeof(GUID));
    asp_wszCommSessionID.Allocate(nCommSessions*sizeof(CComHeapPtr<WCHAR>));
    
    //
    //  Create a device enumerator
    //
    hr = spDeviceEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: CoCreateInstance(__uuidof(IMMDeviceEnumerator)) returned 0x%08x", hr);
        return FNS_PASS;
    }

    //
    //  Get the endpoint under test
    //

    hr = GetEndpointUnderTest(&spDeviceUnderTest);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: GetEndpointUnderTest! 0x%08X", hr);
        return FNS_PASS;
    }

    if (spDeviceUnderTest == NULL)
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IMMDevice under test!");
        return FNS_PASS;
    }    

    //
    //  Save the id of the endpoint under test
    //
    hr = spDeviceUnderTest->GetId(&sp_wszEndpointUnderTest);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IMMDevice::GetId returned 0x%08X", hr);
        return FNS_PASS;
    }

    //
    //  Create a non-communications session on the default render endpoint.
    //
    hr = spDeviceEnumerator->GetDefaultAudioEndpoint(eRender,
                                                     eConsole,
                                                     &spDefaultConsoleDevice);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IMMDeviceGetDefaultAudioEndpoint returned 0x%08X", hr);
        return FNS_PASS;
    }
    

    hr = CreateTestStreamingEngineOnMMDevice(__uuidof(IRenderTestStreamingEngine),
                                   spDefaultConsoleDevice,
                                   TIMER_PUSH_RENDER,
                                   spLogger,
                                   reinterpret_cast<void**>(&spNonCommRenderStreamingEngine));
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: CreateTestStreamingEngineOnMMDevice(IID_ICaptureTestStreamingEngine) returned 0x%08X", hr);
        return FNS_PASS;
    }

    hr = spNonCommRenderStreamingEngine->QueryInterface(__uuidof(ITestStreamingEngine),
                                                        reinterpret_cast<void**>(&spNonCommStreamingEngineCommon));
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IRenderTestStreamingEngine::QueryInterface(IID_ITestStreamingEngine) returned 0x%08X", hr);
        return FNS_PASS;
    }

    hr = spNonCommStreamingEngineCommon->Initialize(&clientProperties);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: ITestStreamingEngine::Initialize returned 0x%08X", hr);
        return FNS_PASS;
    }

    //  Get the ID of the new session.
    hr = spNonCommStreamingEngineCommon->GetSessionControlExtended(&spSessionControl2);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: ITestStreamingEngine::GetSessionControlExtended returned 0x%08x", hr);
        return FNS_PASS;
    }

    hr = spSessionControl2->GetSessionInstanceIdentifier(&sp_wszNonCommSessionID);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IAudioSessionControl2::GetSessionIdentifier returned 0x%08x", hr);
        return FNS_PASS;
    }

    SLOG(eInfo1, "Non Communications Session ID: %S", (LPCWSTR)sp_wszNonCommSessionID);

    //
    //  start the non-communications stream
    //
    hr = spNonCommRenderStreamingEngine->RenderSineTone(DEFAULT_TEST_TONE_FREQ, TEST_TONE_AMP, 0, DITHER_TYPE_TPD, NULL);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IRenderTestStreamingEngine::RenderSineTone returned 0x%08x", hr);
        return FNS_PASS;
    }

    // let audio stream for 1 second
    Sleep(1000);

    //
    //  Initialize OOPNotify
    //
    if (TRUE == bExplicitSubscribe)
    {
        hr = GetIAudioVolumeDuckNotificationHelper(spDeviceUnderTest, sp_wszNonCommSessionID, FALSE, &spDuckHelper);
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: GetIAudioVolumeDuckNotificationHelper returned 0x%08x", hr);
            return FNS_PASS;
        }
    }
    else
    {
        hr = GetIAudioVolumeDuckNotificationHelper(spDeviceUnderTest, NULL, FALSE, &spDuckHelper);
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: GetIAudioVolumeDuckNotificationHelper returned 0x%08x", hr);
            return FNS_PASS;
        }
    }

    //
    //  Get the default communications device
    //
    if (DEVICE_TYPE_RENDER == GetDeviceTypeUnderTest())
    {
        hr = spDeviceEnumerator->GetDefaultAudioEndpoint(eRender,
                                                         eCommunications,
                                                         &spDefaultCommDevice);
    }
    else if (DEVICE_TYPE_CAPTURE == GetDeviceTypeUnderTest())
    {
        hr = spDeviceEnumerator->GetDefaultAudioEndpoint(eCapture,
                                                         eCommunications,
                                                         &spDefaultCommDevice);
    }
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IMMDeviceGetDefaultAudioEndpoint returned 0x%08X", hr);
        return FNS_PASS;
    }        

    //
    //  Save the id of the default communications device
    //
    hr = spDefaultCommDevice->GetId(&sp_wszDefaultCommDevice);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IMMDevice::GetId returned 0x%08X", hr);
        return FNS_PASS;
    }

    //
    // We expect to receive ducking notifications on the non comm session only if it is playing to the 
    // default communications endpoint
    //
    bool bShouldEndpointUnderTestBeDucked = wcsstr(sp_wszNonCommSessionID, sp_wszDefaultCommDevice) || (FALSE == bExplicitSubscribe);

    //
    //  create communications stream(s) on the endpoint under test (render OR capture)
    //
    for (UINT32 nCommSession=0; nCommSession<nCommSessions; nCommSession++)
    {
        //  Get the default communications device
        if (DEVICE_TYPE_RENDER == GetDeviceTypeUnderTest())
        {
            hr = CreateTestStreamingEngineOnMMDevice(__uuidof(IRenderTestStreamingEngine),
                                           spDefaultCommDevice,
                                           TIMER_PUSH_RENDER,
                                           spLogger,
                                           reinterpret_cast<void**>(&aspCommRenderStreamingEngine[nCommSession]));
            if (S_OK != hr)
            {
                XLOG(XFAIL, eError, "FAIL: CreateTestStreamingEngineOnMMDevice(IID_ICaptureTestStreamingEngine) returned 0x%08X", hr);
                return FNS_PASS;
            }

            hr = aspCommRenderStreamingEngine[nCommSession]->QueryInterface(__uuidof(ITestStreamingEngine),
                                                                            reinterpret_cast<void**>(&aspCommStreamingEngineCommon[nCommSession]));
            if (S_OK != hr)
            {
                XLOG(XFAIL, eError, "FAIL: IRenderTestStreamingEngine::QueryInterface(IID_ITestStreamingEngine) returned 0x%08X", hr);
                return FNS_PASS;
            }
        }
        else if (DEVICE_TYPE_CAPTURE == GetDeviceTypeUnderTest())
        {
            hr = CreateTestStreamingEngineOnMMDevice(__uuidof(ICaptureTestStreamingEngine),
                                           spDefaultCommDevice,
                                           TIMER_PUSH_CAPTURE,
                                           spLogger,
                                           reinterpret_cast<void**>(&aspCommCaptureStreamingEngine[nCommSession]));
            if (S_OK != hr)
            {
                XLOG(XFAIL, eError, "FAIL: CreateTestStreamingEngineOnMMDevice(IID_ICaptureTestStreamingEngine) returned 0x%08X", hr);
                return FNS_PASS;
            }

            hr = aspCommCaptureStreamingEngine[nCommSession]->QueryInterface(__uuidof(ITestStreamingEngine),
                                                                            reinterpret_cast<void**>(&aspCommStreamingEngineCommon[nCommSession]));
            if (S_OK != hr)
            {
                XLOG(XFAIL, eError, "FAIL: IRenderTestStreamingEngine::QueryInterface(IID_ITestStreamingEngine) returned 0x%08X", hr);
                return FNS_PASS;
            }        
        }
        else
        {
            ATLASSERT(!"this should never happen");
            XLOG(XFAIL, eError, "FAIL: Encountered unknown device type!");
            return FNS_PASS;            
        }

        //  be sure a new session is created so ducking is actually triggered
        hr = CoCreateGuid(&aguidCommSessionGuid[nCommSession]);
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: CoCreateGuid returned 0x%08X", hr);
            return FNS_PASS;
        }

        hr = aspCommStreamingEngineCommon[nCommSession]->SetRequestedSessionGUID(aguidCommSessionGuid[nCommSession]);
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: ITestStreamingEngine::SetSessionGUID returned 0x%08X", hr);
            return FNS_PASS;
        }

        //
        //  start the communications stream
        //
        hr = aspCommStreamingEngineCommon[nCommSession]->Initialize(&clientProperties);
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: ITestStreamingEngine::Initialize returned 0x%08X", hr);
            return FNS_PASS;
        }
        
        if (DEVICE_TYPE_RENDER == GetDeviceTypeUnderTest())
        {
            hr = aspCommRenderStreamingEngine[nCommSession]->RenderSineTone(COMM_TEST_TONE_FREQ, 
                                                                            TEST_TONE_AMP, 
                                                                            0, 
                                                                            DITHER_TYPE_TPD, 
                                                                            NULL);
            if (S_OK != hr)
            {
                XLOG(XFAIL, eError, "FAIL: IRenderTestStreamingEngine::RenderSineTone returned 0x%08x", hr);
                return FNS_PASS;
            }
        }
        else if (DEVICE_TYPE_CAPTURE == GetDeviceTypeUnderTest())
        {
            hr = aspCommCaptureStreamingEngine[nCommSession]->NullCapture(0, 
                                                                          FALSE);
            if (S_OK != hr)
            {
                XLOG(XFAIL, eError, "FAIL: ICaptureTestStreamingEngine::NullCapture returned 0x%08x", hr);
                return FNS_PASS;
            }
        }
        else
        {
            ATLASSERT(!"this should never happen");
            XLOG(XFAIL, eError, "FAIL: Encountered unknown device type!");
            return FNS_PASS;
        }    

        //  Get the ID of the new session.
        hr = aspCommStreamingEngineCommon[nCommSession]->GetSessionControlExtended(&aspCommSessionControl2[nCommSession]);
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: ITestStreamingEngine::GetSessionControlExtended returned 0x%08x", hr);
            return FNS_PASS;
        }

        hr = aspCommSessionControl2[nCommSession]->GetSessionInstanceIdentifier(&asp_wszCommSessionID[nCommSession]);
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: IAudioSessionControl2::GetSessionIdentifier returned 0x%08x", hr);
            return FNS_PASS;
        }

        SLOG(eInfo1, "Communications Session %d ID: %S", nCommSession, (LPCWSTR)asp_wszCommSessionID[nCommSession]);


        SLOG(eBlab1, "Waiting for duck notification...");

        //  Wait a second for the duck to take place
        Sleep(1000);

        //
        //  Verify that a ducking event was triggered
        //
        ZeroMemory(&sDuckInfo, sizeof(sDuckInfo));
        hr = spDuckHelper->IsNotifyInfoConsistent();
        if (FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Notification info is inconsitent across process boundaries! (hr: 0x%08x)", hr);
        }
        
        hr = spDuckHelper->GetNotifyInfoInProc(&sDuckInfo);
        if (FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: IAudioVolumeDuckNotificationHelper::GetNotifyInfoInProc returned 0x%08x", hr);
            return FNS_PASS;
        }
        DumpDuckNotifyInfo(sDuckInfo);

        SLOG(eInfo1, "Verifying ducking notification information...");

        //  verify the proper number of ducking notifications
        if (bShouldEndpointUnderTestBeDucked)
        {
            // Expect duck notifications if the default communications endpoint is the same as the endpoint under test
            if ((nCommSession+1) != sDuckInfo.cDuckNotifications)
            {
                XLOG(XFAIL, 
                     eFatalError,
                     "  FAIL: Expected %d duck notification, observed %d",
                     nCommSession+1,
                     sDuckInfo.cDuckNotifications);
            }

            //  verify that the currently reported communications session count is accurate
            if (nCommSession+1 != sDuckInfo.countCommunicationsSessions)
            {
                XLOG(XFAIL, 
                     eFatalError, 
                     "  FAIL: Expected %d active communications session(s), observed %d", 
                     nCommSession+1,
                     sDuckInfo.countCommunicationsSessions);  
             }

            if (0 != wcsncmp(asp_wszCommSessionID[nCommSession], sDuckInfo.szwDuckCommSessionID, MAX_PATH))
            {
                XLOG(XFAIL, 
                     eFatalError, 
                     "  FAIL: Expected session '%S'\n"
                     "        Observed session '%S'", 
                     (LPCWSTR)asp_wszCommSessionID[nCommSession],
                     sDuckInfo.szwDuckCommSessionID);
            }
        }
        else
        {
            // Expect no duck notifications if the default communications endpoint is different from the endpoint under test
            if (0 != sDuckInfo.cDuckNotifications)
            {
                XLOG(XFAIL, 
                     eFatalError,
                     "  FAIL: Expected 0 duck notification as the comm stream is playing to another endpoint, observed %d",
                     sDuckInfo.cDuckNotifications);
            }

            if (0 != sDuckInfo.countCommunicationsSessions)
            {
                XLOG(XFAIL, 
                     eFatalError, 
                     "  FAIL: Expected 0 active communications session(s) as the comm stream is playing to another endpoint, observed %d", 
                     sDuckInfo.countCommunicationsSessions);  
             }
        }

        //  verify that no sessions were reported as unducked
        if (0 != sDuckInfo.cUnduckNotifications)
        {
            XLOG(XFAIL, 
                eFatalError, 
                "  FAIL: Expected 0 unduck notifications, observed %d", 
                sDuckInfo.cUnduckNotifications);
        }

    }

    //
    //  Stop the communications stream(s)
    //
    for (UINT32 nCommSession=0; nCommSession<nCommSessions; nCommSession++)
    {
        aspCommCaptureStreamingEngine.RemoveAt(0);
        aspCommRenderStreamingEngine.RemoveAt(0);
        aspCommStreamingEngineCommon.RemoveAt(0);

        //  Wait a second for the stream to un-duck
        Sleep(1000);

        //
        //  Verify that an un-ducking event was triggered if the last communications stream was destroyed
        //
        hr = spDuckHelper->IsNotifyInfoConsistent();
        if (FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Notification info is inconsitent across process boundaries! (hr: 0x%08x)", hr);
        }

        ZeroMemory(&sDuckInfo, sizeof(sDuckInfo));
        hr = spDuckHelper->GetNotifyInfoInProc(&sDuckInfo);
        if (FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: IAudioVolumeDuckNotificationHelper::GetNotifyInfoInProc returned 0x%08x", hr);
            return FNS_PASS;
        }
        DumpDuckNotifyInfo(sDuckInfo);

        SLOG(eInfo1, "Verifying un-duck notification information...");

        if ((nCommSessions-1) == nCommSession && bShouldEndpointUnderTestBeDucked)
        {
            //  The last comm session was destroyed, we expect an un-duck                      
            if (1 != sDuckInfo.cUnduckNotifications)
            {
                XLOG(XFAIL, eFatalError, "  FAIL: Expected 1 un-duck notification, observed %d", sDuckInfo.cUnduckNotifications);
            }

            if (0 != wcsncmp(asp_wszCommSessionID[nCommSession], sDuckInfo.szwUnduckCommSessionID, MAX_PATH))
            {
                XLOG(XFAIL, 
                     eFatalError, 
                     "  FAIL: Expected session '%S'\n"
                     "        Observed session '%S'", 
                     (LPCWSTR)asp_wszCommSessionID[nCommSession],
                     sDuckInfo.szwUnduckCommSessionID);        
            }
        }
        else
        {
            // There are still outstanding comm sessions; ensure we didn't un-duck
            if (0 != sDuckInfo.cUnduckNotifications)
            {
                XLOG(XFAIL, eFatalError, "  FAIL: Expected 0 un-duck notifications, observed %d", sDuckInfo.cUnduckNotifications);
            }
        }
    }

    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: Verify no ducking notifications are received when a client subscribes
//       then unsubscribes to ducking notifications.
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD TC_DuckingNotification_SubscrUnsubscr()
{
    if(SkipDuckingTests())
    {
        XLOG(XSKIP, eError, "Ducking tests not applicable for platform.");
        return FNS_SKIPPED;
    }

    HRESULT                                 hr = S_OK;
    AudioClientProperties                   clientProperties = {0};

    CComPtr<IBasicLog>                      spLogger = NULL;
    CComPtr<IAudioVolumeDuckNotification>   spDuckNotify = NULL;
    CDummyDuckingNotificationClient*        pobjDuckNotifyClient = NULL;

    CComPtr<IMMDevice>                      spDeviceUnderTest = NULL;    
    CComPtr<IAudioSessionManager>           spSessionManager = NULL;
    CComPtr<IAudioSessionManager2>          spSessionManager2 = NULL;
    CComPtr<IAudioSessionControl2>          spSessionControl2 = NULL;
    CComPtr<IMMDeviceEnumerator>            spDeviceEnumerator = NULL;
    CComPtr<IMMDevice>                      spDefaultCommDevice = NULL;
    CComPtr<IMMDevice>                      spDefaultConsoleDevice = NULL;

    // "Non-communications" session supporting interfaces
    CComPtr<ITestStreamingEngine>           spNonCommStreamingEngineCommon = NULL;
    CComPtr<IRenderTestStreamingEngine>     spNonCommRenderStreamingEngine = NULL;

    // "Communications" session supporting interfaces
    CComPtr<ITestStreamingEngine>           spCommStreamingEngineCommon = NULL;
    CComPtr<IRenderTestStreamingEngine>     spCommRenderStreamingEngine = NULL;
    CComPtr<ICaptureTestStreamingEngine>    spCommCaptureStreamingEngine = NULL;
    CComPtr<IAudioSessionControl2>          spCommSessionControl2 = NULL;    
    

    CComHeapPtr<WCHAR>                      sp_wszNonCommSessionID;
    CComHeapPtr<WCHAR>                      sp_wszCommSessionID;
    GUID                                    guidCommSessionGuid = GUID_NULL;

    clientProperties.bIsOffload = FALSE;
    clientProperties.eCategory = AudioCategory_Other;
    clientProperties.cbSize = sizeof(clientProperties);

    if (GetModule()->IsAppContainer())
    {
        XLOG(XSKIP, eError, "IAudioSessionManager2 activation is blocked in AppContainer. Test case skipping...");
        return FNS_SKIPPED;
    }

    //
    //  Get an IBasicLog
    //
    hr = g_IShell->GetBasicLog(&spLogger);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to create IBasicLog!");
        return FNS_PASS;
    }

    //
    //  Create a device enumerator
    //
    hr = spDeviceEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: CoCreateInstance(__uuidof(IMMDeviceEnumerator)) returned 0x%08x", hr);
        return FNS_PASS;
    }

    //
    //  Get the endpoint under test
    //
    hr = GetEndpointUnderTest(&spDeviceUnderTest);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: GetEndpointUnderTest! 0x%08X", hr);
        return FNS_PASS;
    }

    if (spDeviceUnderTest == NULL)
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IMMDevice under test!");
        return FNS_PASS;
    }    

    //
    //  Create a notification handler
    //
    pobjDuckNotifyClient = new CDummyDuckingNotificationClient();
    if (NULL == pobjDuckNotifyClient)
    {
        XLOG(XFAIL, eError, "FAIL: Out of memory!");
        return FNS_PASS;
    }
    spDuckNotify.Attach(pobjDuckNotifyClient);

    //
    //  Activate an IAudioSessionManager and get an IAudioSessionManager2 pointer
    //
    hr = spDeviceUnderTest->Activate(__uuidof(IAudioSessionManager),
                                     CLSCTX_ALL,
                                     NULL,
                                     reinterpret_cast<void**>(&spSessionManager));
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IMMDevice::Activate(IID_IAudioSessionManager) returned 0x%08X", hr);
        return FNS_PASS;
    }

    hr = spSessionManager->QueryInterface(__uuidof(IAudioSessionManager2),
                                          reinterpret_cast<void**>(&spSessionManager2));
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IAudioSessionManager::QueryInterface(IID_IAudioSessionManager2) returne 0x%08x", hr);
        return FNS_PASS;
    }

    //
    //  Subscribe/Unsubscribe to ducking notifications as appropriate
    //
    
    //  Subscribe to notifications
    XLOG(XMSG, eBlab1, "Subscribing...");
    hr = spSessionManager2->RegisterDuckNotification(NULL, spDuckNotify);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IAudioSessionManager2::RegisterDuckNotification(NULL, Client) returned 0x%08x", hr);
    }

    //  Unsubscribe to notification
    XLOG(XMSG, eBlab1, "Unsubscribing...");
    hr = spSessionManager2->UnregisterDuckNotification(spDuckNotify);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IAudioSessionManager2::UnregisterDuckNotification(pClient) returned 0x%08x", hr);
    }

    pobjDuckNotifyClient->ResetEvents();

    //
    //  Create a non-communications session on the default render endpoint.
    //
    hr = spDeviceEnumerator->GetDefaultAudioEndpoint(eRender,
                                                     eConsole,
                                                     &spDefaultConsoleDevice);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IMMDeviceGetDefaultAudioEndpoint returned 0x%08X", hr);
        return FNS_PASS;
    }
    

    hr = CreateTestStreamingEngineOnMMDevice(__uuidof(IRenderTestStreamingEngine),
                                   spDefaultConsoleDevice,
                                   TIMER_PUSH_RENDER,
                                   spLogger,
                                   reinterpret_cast<void**>(&spNonCommRenderStreamingEngine));
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: CreateTestStreamingEngineOnMMDevice(IID_ICaptureTestStreamingEngine) returned 0x%08X", hr);
        return FNS_PASS;
    }

    hr = spNonCommRenderStreamingEngine->QueryInterface(__uuidof(ITestStreamingEngine),
                                                        reinterpret_cast<void**>(&spNonCommStreamingEngineCommon));
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IRenderTestStreamingEngine::QueryInterface(IID_ITestStreamingEngine) returned 0x%08X", hr);
        return FNS_PASS;
    }

    hr = spNonCommStreamingEngineCommon->Initialize(&clientProperties);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: ITestStreamingEngine::Initialize returned 0x%08X", hr);
        return FNS_PASS;
    }

    //  Get the ID of the new session.
    hr = spNonCommStreamingEngineCommon->GetSessionControlExtended(&spSessionControl2);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: ITestStreamingEngine::GetSessionControlExtended returned 0x%08x", hr);
        return FNS_PASS;
    }

    hr = spSessionControl2->GetSessionInstanceIdentifier(&sp_wszNonCommSessionID);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IAudioSessionControl2::GetSessionIdentifier returned 0x%08x", hr);
        return FNS_PASS;
    }

    SLOG(eInfo1, "Non Communications Session ID: %S", (LPCWSTR)sp_wszNonCommSessionID);

    //
    //  start the non-communications stream
    //
    hr = spNonCommRenderStreamingEngine->RenderSineTone(DEFAULT_TEST_TONE_FREQ, TEST_TONE_AMP, 0, DITHER_TYPE_TPD, NULL);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IRenderTestStreamingEngine::RenderSineTone returned 0x%08x", hr);
        return FNS_PASS;
    }

    // let audio stream for 1 second
    Sleep(1000);

    //
    //  create a communications stream on the endpoint under test (render OR capture)
    //

    //  Get the default communications device
    if (DEVICE_TYPE_RENDER == GetDeviceTypeUnderTest())
    {
        hr = spDeviceEnumerator->GetDefaultAudioEndpoint(eRender,
                                                         eCommunications,
                                                         &spDefaultCommDevice);
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: IMMDeviceGetDefaultAudioEndpoint returned 0x%08X", hr);
            return FNS_PASS;
        }
        

        hr = CreateTestStreamingEngineOnMMDevice(__uuidof(IRenderTestStreamingEngine),
                                       spDefaultCommDevice,
                                       TIMER_PUSH_RENDER,
                                       spLogger,
                                       reinterpret_cast<void**>(&spCommRenderStreamingEngine));
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: CreateTestStreamingEngineOnMMDevice(IID_ICaptureTestStreamingEngine) returned 0x%08X", hr);
            return FNS_PASS;
        }

        hr = spCommRenderStreamingEngine->QueryInterface(__uuidof(ITestStreamingEngine),
                                                         reinterpret_cast<void**>(&spCommStreamingEngineCommon));
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: IRenderTestStreamingEngine::QueryInterface(IID_ITestStreamingEngine) returned 0x%08X", hr);
            return FNS_PASS;
        }
    }
    else if (DEVICE_TYPE_CAPTURE == GetDeviceTypeUnderTest())
    {
        hr = spDeviceEnumerator->GetDefaultAudioEndpoint(eCapture,
                                                         eCommunications,
                                                         &spDefaultCommDevice);
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: IMMDeviceGetDefaultAudioEndpoint returned 0x%08X", hr);
            return FNS_PASS;
        }        

        hr = CreateTestStreamingEngineOnMMDevice(__uuidof(ICaptureTestStreamingEngine),
                                       spDefaultCommDevice,
                                       TIMER_PUSH_CAPTURE,
                                       spLogger,
                                       reinterpret_cast<void**>(&spCommCaptureStreamingEngine));
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: CreateTestStreamingEngineOnMMDevice(IID_ICaptureTestStreamingEngine) returned 0x%08X", hr);
            return FNS_PASS;
        }

        hr = spCommCaptureStreamingEngine->QueryInterface(__uuidof(ITestStreamingEngine),
                                                          reinterpret_cast<void**>(&spCommStreamingEngineCommon));
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: IRenderTestStreamingEngine::QueryInterface(IID_ITestStreamingEngine) returned 0x%08X", hr);
            return FNS_PASS;
        }        
    }
    else
    {
        ATLASSERT(!"this should never happen");
    }

    //  be sure a new session is created so ducking is actually triggered
    hr = CoCreateGuid(&guidCommSessionGuid);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: CoCreateGuid returned 0x%08X", hr);
        return FNS_PASS;
    }

    hr = spCommStreamingEngineCommon->SetRequestedSessionGUID(guidCommSessionGuid);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: ITestStreamingEngine::SetSessionGUID returned 0x%08X", hr);
        return FNS_PASS;
    }

    //
    //  start the communications stream
    //
    hr = spCommStreamingEngineCommon->Initialize(&clientProperties);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: ITestStreamingEngine::Initialize returned 0x%08X", hr);
        return FNS_PASS;
    }
    
    if (DEVICE_TYPE_RENDER == GetDeviceTypeUnderTest())
    {
        hr = spCommRenderStreamingEngine->RenderSineTone(COMM_TEST_TONE_FREQ, TEST_TONE_AMP, 0, DITHER_TYPE_TPD, NULL);
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: IRenderTestStreamingEngine::RenderSineTone returned 0x%08x", hr);
            return FNS_PASS;
        }
    }
    else if (DEVICE_TYPE_CAPTURE == GetDeviceTypeUnderTest())
    {
        hr = spCommCaptureStreamingEngine->NullCapture(0, FALSE);
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: ICaptureTestStreamingEngine::NullCapture returned 0x%08x", hr);
            return FNS_PASS;
        }
    }
    else
    {
        ATLASSERT(!"this should never happen");
        XLOG(XFAIL, eError, "FAIL: Encountered unknown device type!");
        return FNS_PASS;        
    }    

    //  Get the ID of the new session.
    hr = spCommStreamingEngineCommon->GetSessionControlExtended(&spCommSessionControl2);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: ITestStreamingEngine::GetSessionControlExtended returned 0x%08x", hr);
        return FNS_PASS;
    }

    hr = spCommSessionControl2->GetSessionInstanceIdentifier(&sp_wszCommSessionID);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IAudioSessionControl2::GetSessionIdentifier returned 0x%08x", hr);
        return FNS_PASS;
    }

    SLOG(eInfo1, "Communications Session ID: %S", (LPCWSTR)sp_wszCommSessionID);


    SLOG(eBlab1, "Waiting for duck...");

    //  Wait a second for the duck to take place
    Sleep(1000);

    //
    //  Verify that no event was triggered
    //

    SLOG(eInfo1, "Verifying no notifications were received...");
    UINT32 nEventCount = pobjDuckNotifyClient->EventCount();
    if (0 != nEventCount)
    {
        XLOG(XFAIL, eFatalError, "  FAIL: %d events unexpectedly received!", nEventCount);
    }

    //
    //  Stop the communications stream
    //
    spCommCaptureStreamingEngine = NULL;
    spCommRenderStreamingEngine = NULL;
    spCommStreamingEngineCommon = NULL;

    //  Wait a second for the stream to un-duck
    Sleep(1000);

    //
    //  Verify that no un-ducking event was triggered
    //
    nEventCount = pobjDuckNotifyClient->EventCount();
    if (0 != nEventCount)
    {
        XLOG(XFAIL, eFatalError, "  FAIL: %d events unexpectedly received!", nEventCount);
    }    

    return FNS_PASS;
}

DWORD TC_DuckingNotification_NoIDIntersect()
{
    if(SkipDuckingTests())
    {
        XLOG(XSKIP, eError, "Ducking tests not applicable for platform.");
        return FNS_SKIPPED;
    }

    HRESULT                                 hr = S_OK;
    AudioClientProperties                   clientProperties = {0};

    CComPtr<IBasicLog>                      spLogger = NULL;
    CComPtr<IAudioVolumeDuckNotification>   spDuckNotify = NULL;
    CDummyDuckingNotificationClient*        pobjDuckNotifyClient = NULL;

    CComPtr<IMMDevice>                      spDeviceUnderTest = NULL;    
    CComPtr<IAudioSessionManager>           spSessionManager = NULL;
    CComPtr<IAudioSessionManager2>          spSessionManager2 = NULL;
    CComPtr<IAudioSessionControl2>          spSessionControl2 = NULL;
    CComPtr<IMMDeviceEnumerator>            spDeviceEnumerator = NULL;
    CComPtr<IMMDevice>                      spDefaultCommDevice = NULL;
    CComPtr<IMMDevice>                      spDefaultConsoleDevice = NULL;

    // "Non-communications" session supporting interfaces
    CComPtr<ITestStreamingEngine>           spNonCommStreamingEngineCommon = NULL;
    CComPtr<IRenderTestStreamingEngine>     spNonCommRenderStreamingEngine = NULL;

    // "Communications" session supporting interfaces
    CComPtr<ITestStreamingEngine>           spCommStreamingEngineCommon = NULL;
    CComPtr<IRenderTestStreamingEngine>     spCommRenderStreamingEngine = NULL;
    CComPtr<ICaptureTestStreamingEngine>    spCommCaptureStreamingEngine = NULL;
    CComPtr<IAudioSessionControl2>          spCommSessionControl2 = NULL;    
    

    CComHeapPtr<WCHAR>                      sp_wszNonCommSessionID;
    CComHeapPtr<WCHAR>                      sp_wszCommSessionID;
    GUID                                    guidCommSessionGuid = GUID_NULL;

    clientProperties.bIsOffload = FALSE;
    clientProperties.eCategory = AudioCategory_Other;
    clientProperties.cbSize = sizeof(clientProperties);

    if (GetModule()->IsAppContainer())
    {
        XLOG(XSKIP, eError, "IAudioSessionManager2 activation is blocked in AppContainer. Test case skipping...");
        goto EXIT;
    }

    //
    //  Get an IBasicLog
    //
    hr = g_IShell->GetBasicLog(&spLogger);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to create IBasicLog!");
        goto EXIT;
    }

    //
    //  Create a device enumerator
    //
    hr = spDeviceEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: CoCreateInstance(__uuidof(IMMDeviceEnumerator)) returned 0x%08x", hr);
        goto EXIT;
    }

    //
    //  Get the endpoint under test
    //
    hr = GetEndpointUnderTest(&spDeviceUnderTest);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: GetEndpointUnderTest! 0x%08X", hr);
        return FNS_PASS;
    }

    if (spDeviceUnderTest == NULL)
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IMMDevice under test!");
        goto EXIT;
    }    

    //
    //  Create a notification handler
    //
    pobjDuckNotifyClient = new CDummyDuckingNotificationClient();
    if (NULL == pobjDuckNotifyClient)
    {
        XLOG(XFAIL, eError, "FAIL: Out of memory!");
        goto EXIT;
    }
    spDuckNotify.Attach(pobjDuckNotifyClient);

    //
    //  Activate an IAudioSessionManager and get an IAudioSessionManager2 pointer
    //
    hr = spDeviceUnderTest->Activate(__uuidof(IAudioSessionManager),
                                     CLSCTX_ALL,
                                     NULL,
                                     reinterpret_cast<void**>(&spSessionManager));
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IMMDevice::Activate(IID_IAudioSessionManager) returned 0x%08X", hr);
        goto EXIT;
    }

    hr = spSessionManager->QueryInterface(__uuidof(IAudioSessionManager2),
                                          reinterpret_cast<void**>(&spSessionManager2));
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IAudioSessionManager::QueryInterface(IID_IAudioSessionManager2) returne 0x%08x", hr);
        goto EXIT;
    }

    //  Subscribe to notifications for a bogus client
    XLOG(XMSG, eBlab1, "Registering for notifications using a bogus session instance ID...");
    hr = spSessionManager2->RegisterDuckNotification(L"asdf1234", spDuckNotify);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IAudioSessionManager2::RegisterDuckNotification(NULL, Client) returned 0x%08x", hr);
        goto EXIT;
    }

    //
    //  Create a non-communications session on the default render endpoint.
    //
    hr = spDeviceEnumerator->GetDefaultAudioEndpoint(eRender,
                                                     eConsole,
                                                     &spDefaultConsoleDevice);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IMMDeviceGetDefaultAudioEndpoint returned 0x%08X", hr);
        goto UNREGISTER_AND_EXIT;
    }
    

    hr = CreateTestStreamingEngineOnMMDevice(__uuidof(IRenderTestStreamingEngine),
                                   spDefaultConsoleDevice,
                                   TIMER_PUSH_RENDER,
                                   spLogger,
                                   reinterpret_cast<void**>(&spNonCommRenderStreamingEngine));
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: CreateTestStreamingEngineOnMMDevice(IID_ICaptureTestStreamingEngine) returned 0x%08X", hr);
        goto UNREGISTER_AND_EXIT;
    }

    hr = spNonCommRenderStreamingEngine->QueryInterface(__uuidof(ITestStreamingEngine),
                                                        reinterpret_cast<void**>(&spNonCommStreamingEngineCommon));
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IRenderTestStreamingEngine::QueryInterface(IID_ITestStreamingEngine) returned 0x%08X", hr);
        goto UNREGISTER_AND_EXIT;
    }

    hr = spNonCommStreamingEngineCommon->Initialize(&clientProperties);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: ITestStreamingEngine::Initialize returned 0x%08X", hr);
        goto UNREGISTER_AND_EXIT;
    }

    //  Get the ID of the new session.
    hr = spNonCommStreamingEngineCommon->GetSessionControlExtended(&spSessionControl2);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: ITestStreamingEngine::GetSessionControlExtended returned 0x%08x", hr);
        goto UNREGISTER_AND_EXIT;
    }

    hr = spSessionControl2->GetSessionInstanceIdentifier(&sp_wszNonCommSessionID);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IAudioSessionControl2::GetSessionIdentifier returned 0x%08x", hr);
        goto UNREGISTER_AND_EXIT;
    }

    SLOG(eInfo1, "Non Communications Session ID: %S", (LPCWSTR)sp_wszNonCommSessionID);

    //
    //  start the non-communications stream
    //
    hr = spNonCommRenderStreamingEngine->RenderSineTone(DEFAULT_TEST_TONE_FREQ, TEST_TONE_AMP, 0, DITHER_TYPE_TPD, NULL);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IRenderTestStreamingEngine::RenderSineTone returned 0x%08x", hr);
        goto UNREGISTER_AND_EXIT;
    }

    // let audio stream for 1 second
    Sleep(1000);

    //
    //  create a communications stream on the endpoint under test (render OR capture)
    //

    //  Get the default communications device
    if (DEVICE_TYPE_RENDER == GetDeviceTypeUnderTest())
    {
        hr = spDeviceEnumerator->GetDefaultAudioEndpoint(eRender,
                                                         eCommunications,
                                                         &spDefaultCommDevice);
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: IMMDeviceGetDefaultAudioEndpoint returned 0x%08X", hr);
            goto UNREGISTER_AND_EXIT;
        }
        

        hr = CreateTestStreamingEngineOnMMDevice(__uuidof(IRenderTestStreamingEngine),
                                       spDefaultCommDevice,
                                       TIMER_PUSH_RENDER,
                                       spLogger,
                                       reinterpret_cast<void**>(&spCommRenderStreamingEngine));
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: CreateTestStreamingEngineOnMMDevice(IID_ICaptureTestStreamingEngine) returned 0x%08X", hr);
            goto UNREGISTER_AND_EXIT;
        }

        hr = spCommRenderStreamingEngine->QueryInterface(__uuidof(ITestStreamingEngine),
                                                         reinterpret_cast<void**>(&spCommStreamingEngineCommon));
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: IRenderTestStreamingEngine::QueryInterface(IID_ITestStreamingEngine) returned 0x%08X", hr);
            goto UNREGISTER_AND_EXIT;
        }
    }
    else if (DEVICE_TYPE_CAPTURE == GetDeviceTypeUnderTest())
    {
        hr = spDeviceEnumerator->GetDefaultAudioEndpoint(eCapture,
                                                         eCommunications,
                                                         &spDefaultCommDevice);
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: IMMDeviceGetDefaultAudioEndpoint returned 0x%08X", hr);
            goto UNREGISTER_AND_EXIT;
        }        

        hr = CreateTestStreamingEngineOnMMDevice(__uuidof(ICaptureTestStreamingEngine),
                                       spDefaultCommDevice,
                                       TIMER_PUSH_CAPTURE,
                                       spLogger,
                                       reinterpret_cast<void**>(&spCommCaptureStreamingEngine));
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: CreateTestStreamingEngineOnMMDevice(IID_ICaptureTestStreamingEngine) returned 0x%08X", hr);
            goto UNREGISTER_AND_EXIT;
        }

        hr = spCommCaptureStreamingEngine->QueryInterface(__uuidof(ITestStreamingEngine),
                                                          reinterpret_cast<void**>(&spCommStreamingEngineCommon));
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: IRenderTestStreamingEngine::QueryInterface(IID_ITestStreamingEngine) returned 0x%08X", hr);
            goto UNREGISTER_AND_EXIT;
        }        
    }
    else
    {
        ATLASSERT(!"this should never happen");
        XLOG(XFAIL, eError, "FAIL: Encountered unknown device type!");
        return FNS_PASS;
    }

    //  be sure a new session is created so ducking is actually triggered
    hr = CoCreateGuid(&guidCommSessionGuid);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: CoCreateGuid returned 0x%08X", hr);
        goto UNREGISTER_AND_EXIT;
    }

    hr = spCommStreamingEngineCommon->SetRequestedSessionGUID(guidCommSessionGuid);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: ITestStreamingEngine::SetSessionGUID returned 0x%08X", hr);
        goto UNREGISTER_AND_EXIT;
    }

    //
    //  start the communications stream
    //
    hr = spCommStreamingEngineCommon->Initialize(&clientProperties);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: ITestStreamingEngine::Initialize returned 0x%08X", hr);
        goto UNREGISTER_AND_EXIT;
    }
    
    if (DEVICE_TYPE_RENDER == GetDeviceTypeUnderTest())
    {
        hr = spCommRenderStreamingEngine->RenderSineTone(COMM_TEST_TONE_FREQ, TEST_TONE_AMP, 0, DITHER_TYPE_TPD, NULL);
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: IRenderTestStreamingEngine::RenderSineTone returned 0x%08x", hr);
            goto UNREGISTER_AND_EXIT;
        }
    }
    else if (DEVICE_TYPE_CAPTURE == GetDeviceTypeUnderTest())
    {
        hr = spCommCaptureStreamingEngine->NullCapture(0, FALSE);
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: ICaptureTestStreamingEngine::NullCapture returned 0x%08x", hr);
            goto UNREGISTER_AND_EXIT;
        }
    }
    else
    {
        ATLASSERT(!"this should never happen");
        XLOG(XFAIL, eError, "FAIL: Encountered unknown device type!");
        return FNS_PASS;
    }    

    //  Get the ID of the new session.
    hr = spCommStreamingEngineCommon->GetSessionControlExtended(&spCommSessionControl2);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: ITestStreamingEngine::GetSessionControlExtended returned 0x%08x", hr);
        goto UNREGISTER_AND_EXIT;
    }

    hr = spCommSessionControl2->GetSessionInstanceIdentifier(&sp_wszCommSessionID);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IAudioSessionControl2::GetSessionIdentifier returned 0x%08x", hr);
        goto UNREGISTER_AND_EXIT;
    }

    SLOG(eInfo1, "Communications Session ID: %S", (LPCWSTR)sp_wszCommSessionID);


    SLOG(eBlab1, "Waiting for duck...");

    //  Wait a second for the duck to take place
    Sleep(1000);

    //
    //  Verify that no event was triggered
    //

    SLOG(eInfo1, "Verifying no notifications were received...");
    UINT32 nEventCount = pobjDuckNotifyClient->EventCount();
    if (0 != nEventCount)
    {
        XLOG(XFAIL, eFatalError, "  FAIL: %d events unexpectedly received!", nEventCount);
    }

    //
    //  Stop the communications stream
    //
    spCommCaptureStreamingEngine = NULL;
    spCommRenderStreamingEngine = NULL;
    spCommStreamingEngineCommon = NULL;

    SLOG(eInfo1, "Un-ducking...");

    //  Wait a second for the stream to un-duck
    Sleep(1000);

    //
    //  Verify that no un-ducking event was triggered
    //
    nEventCount = pobjDuckNotifyClient->EventCount();
    if (0 != nEventCount)
    {
        XLOG(XFAIL, eFatalError, "  FAIL: %d events unexpectedly received!", nEventCount);
    }    

UNREGISTER_AND_EXIT:
    hr = spSessionManager2->UnregisterDuckNotification(spDuckNotify);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: IAudioSessionManager2::UnregisterDuckNotification returned 0x%08X", hr);
    }
EXIT:
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: SetDuckingPreference Basic Functionality Verification
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD TC_Ducking_SetDuckingPreferenceBasic()
{
    if(SkipDuckingTests())
    {
        XLOG(XSKIP, eError, "Ducking tests not applicable for platform.");
        return FNS_SKIPPED;
    }

    HRESULT                                 hr = S_OK;
    AudioClientProperties                   clientProperties = {0};

    CComPtr<ITestStreamingEngine>           spStreamingEngineCommon = NULL;
    CComPtr<IRenderTestStreamingEngine>     spRenderStreamingEngine = NULL;
    CComPtr<ICaptureTestStreamingEngine>    spCaptureStreamingEngine = NULL;
    CComPtr<IAudioSessionControl2>          spSessionControl2 = NULL;

    CComPtr<IBasicLog>                      spLogger = NULL;
    CComPtr<IMMDevice>                      spDeviceUnderTest = NULL;   

    clientProperties.bIsOffload = FALSE;
    clientProperties.eCategory = AudioCategory_Other;
    clientProperties.cbSize = sizeof(clientProperties);

    //
    //  Get an IBasicLog
    //
    hr = g_IShell->GetBasicLog(&spLogger);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to create IBasicLog!");
        return FNS_PASS;
    }


    //
    //  Get the endpoint under test
    //
    hr = GetEndpointUnderTest(&spDeviceUnderTest);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: GetEndpointUnderTest! 0x%08X", hr);
        return FNS_PASS;
    }

    if (spDeviceUnderTest == NULL)
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IMMDevice under test!");
        return FNS_PASS;
    }

    //
    //  Create an audio session
    //
    if (DEVICE_TYPE_RENDER == GetDeviceTypeUnderTest())
    {
        //  Create a render session
        hr = CreateTestStreamingEngineOnMMDevice(__uuidof(IRenderTestStreamingEngine),
                                       spDeviceUnderTest,
                                       TIMER_PUSH_RENDER,
                                       spLogger,
                                       reinterpret_cast<void**>(&spRenderStreamingEngine));
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: CreateTestStreamingEngineOnMMDevice(IID_RenderTestStreamingEngine)returned 0x%08X", hr);
            return FNS_PASS;
        }

        hr = spRenderStreamingEngine->QueryInterface(__uuidof(ITestStreamingEngine),
                                                     reinterpret_cast<void**>(&spStreamingEngineCommon));
        if (S_OK != hr)
        {
            XLOG(XFAIL, 
                 eError, 
                 "FAIL: IRenderTestStreamingEngine::QueryInterface(IID_ITestStreamingEngine) returned 0x%08X",
                 hr);
            return FNS_PASS;
        }
    }
    else if (DEVICE_TYPE_CAPTURE == GetDeviceTypeUnderTest())
    {
        //  Create a capture session
        hr = CreateTestStreamingEngineOnMMDevice(__uuidof(ICaptureTestStreamingEngine),
                                       spDeviceUnderTest,
                                       TIMER_PUSH_CAPTURE,
                                       spLogger,
                                       reinterpret_cast<void**>(&spCaptureStreamingEngine));
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: CreateTestStreamingEngineOnMMDevice(IID_CaptureTestStreamingEngine)returned 0x%08X", hr);
            return FNS_PASS;
        }

        hr = spCaptureStreamingEngine->QueryInterface(__uuidof(ITestStreamingEngine),
                                                     reinterpret_cast<void**>(&spStreamingEngineCommon));
        if (S_OK != hr)
        {
            XLOG(XFAIL, 
                 eError, 
                 "FAIL: ICaptureTestStreamingEngine::QueryInterface(IID_ITestStreamingEngine) returned 0x%08X",
                 hr);
            return FNS_PASS;
        }        
    }
    else
    {
        ATLASSERT(!"This should never happen!");
        XLOG(XFAIL, eError, "FAIL: Encountered unknown device type!");
        return FNS_PASS;
    }

    //
    //  Initialize the streaming engine
    //
    hr = spStreamingEngineCommon->Initialize(&clientProperties);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: ITestStreamingEngine::Initialize returned 0x%08X", hr);
        return FNS_PASS;
    }

    //
    //  Get the IAudioSessionControl2 interface
    //
    hr = spStreamingEngineCommon->GetSessionControlExtended(&spSessionControl2);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: ITestStreamingEngine::GetSessionControlExtended returned 0x%08X", hr);
        return FNS_PASS;
    }
    
    //
    //  Opt-out of ducking
    //
    hr = spSessionControl2->SetDuckingPreference(TRUE);
    if (DEVICE_TYPE_RENDER == GetDeviceTypeUnderTest())
    {
        if (S_OK != hr)
        {
            XLOG(XFAIL, 
                 eError, 
                 "FAIL: IAudioSessionControl2::SetDuckingPreference(TRUE) returned (0x%08X) <Expected: S_OK>", 
                 hr);
        }
    }
    else if (DEVICE_TYPE_CAPTURE == GetDeviceTypeUnderTest())
    {
        if (AUDCLNT_E_WRONG_ENDPOINT_TYPE != hr)
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: IAudioSessionControl2::SetDuckingPreference(TRUE) returned (0x%08X) <Expected: AUDCLNT_E_WRONG_ENDPOINT_TYPE>",
                 hr);
        }
    }
    else
    {
        XLOG(XFAIL, eError, "FAIL: Encountered unknown endpoint type!");
        return FNS_PASS;
    }
    
    //  Double opt-out
    hr = spSessionControl2->SetDuckingPreference(TRUE);
    if (DEVICE_TYPE_RENDER == GetDeviceTypeUnderTest())
    {
        if (S_OK != hr)
        {
            XLOG(XFAIL, 
                 eError, 
                 "FAIL: IAudioSessionControl2::SetDuckingPreference(TRUE) returned (0x%08X) <Expected: S_OK>", 
                 hr);
        }
    }
    else if (DEVICE_TYPE_CAPTURE == GetDeviceTypeUnderTest())
    {
        if (AUDCLNT_E_WRONG_ENDPOINT_TYPE != hr)
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: IAudioSessionControl2::SetDuckingPreference(TRUE) returned (0x%08X) <Expected: AUDCLNT_E_WRONG_ENDPOINT_TYPE>",
                 hr);
        }
    }
    else
    {
        XLOG(XFAIL, eError, "FAIL: Encountered unknown endpoint type!");
        return FNS_PASS;
    }

    //
    //  Opt-in to ducking
    //
    hr = spSessionControl2->SetDuckingPreference(FALSE);
    if (DEVICE_TYPE_RENDER == GetDeviceTypeUnderTest())
    {
        if (S_OK != hr)
        {
            XLOG(XFAIL, 
                 eError, 
                 "FAIL: IAudioSessionControl2::SetDuckingPreference(FALSE) returned (0x%08X) <Expected: S_OK>", 
                 hr);
        }
    }
    else if (DEVICE_TYPE_CAPTURE == GetDeviceTypeUnderTest())
    {
        if (AUDCLNT_E_WRONG_ENDPOINT_TYPE != hr)
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: IAudioSessionControl2::SetDuckingPreference(FALSE) returned (0x%08X) <Expected: AUDCLNT_E_WRONG_ENDPOINT_TYPE>",
                 hr);
        }
    }
    else
    {
        XLOG(XFAIL, eError, "FAIL: Encountered unknown endpoint type!");
        return FNS_PASS;
    }

    //  Double opt-in
    hr = spSessionControl2->SetDuckingPreference(FALSE);    
    if (DEVICE_TYPE_RENDER == GetDeviceTypeUnderTest())
    {
        if (S_OK != hr)
        {
            XLOG(XFAIL, 
                 eError, 
                 "FAIL: IAudioSessionControl2::SetDuckingPreference(FALSE) returned (0x%08X) <Expected: S_OK>", 
                 hr);
        }
    }
    else if (DEVICE_TYPE_CAPTURE == GetDeviceTypeUnderTest())
    {
        if (AUDCLNT_E_WRONG_ENDPOINT_TYPE != hr)
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: IAudioSessionControl2::SetDuckingPreference(FALSE) returned (0x%08X) <Expected: AUDCLNT_E_WRONG_ENDPOINT_TYPE>",
                 hr);
        }
    }
    else
    {
        XLOG(XFAIL, eError, "FAIL: Encountered unknown endpoint type!");
        return FNS_PASS;
    }

    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: Verify IAudioSessionControlInternal::GetDuckingState returns accurate
//       values in an end-to-end scenario.
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD TC_Ducking_DuckingStateE2E()
{
    if(SkipDuckingTests())
    {
        XLOG(XSKIP, eError, "Ducking tests not applicable for platform.");
        return FNS_SKIPPED;
    }

    HRESULT                                 hr = S_OK;
    DWORD                                   dwRes = 0;
    AudioClientProperties                   clientProperties = {0};

    CComPtr<IBasicLog>                      spLogger = NULL;

    CComPtr<IMMDevice>                      spDeviceUnderTest = NULL;    
    CComPtr<IMMDeviceEnumerator>            spDeviceEnumerator = NULL;
    CComPtr<IMMDevice>                      spDefaultCommDevice = NULL;
    CComPtr<IMMDevice>                      spDefaultConsoleDevice = NULL;

    // "Non-communications" session supporting interfaces
    CComPtr<ITestStreamingEngine>           spNonCommStreamingEngineCommon = NULL;
    CComPtr<IRenderTestStreamingEngine>     spNonCommRenderStreamingEngine = NULL;
    CComPtr<IAudioSessionControlInternal>   spNonCommSessionControlInternal = NULL;

    // "Communications" session supporting interfaces
    CComPtr<ITestStreamingEngine>           spCommStreamingEngineCommon = NULL;
    CComPtr<IRenderTestStreamingEngine>     spCommRenderStreamingEngine = NULL;
    CComPtr<ICaptureTestStreamingEngine>    spCommCaptureStreamingEngine = NULL;
    GUID                                    guidCommSessionGuid = GUID_NULL;

    CComHeapPtr<WCHAR>                      sp_wszDefaultConsoleDevice;
    CComHeapPtr<WCHAR>                      sp_wszDefaultCommEndpoint;

    clientProperties.bIsOffload = FALSE;
    clientProperties.eCategory = AudioCategory_Other;
    clientProperties.cbSize = sizeof(clientProperties);

    //
    //  Get an IBasicLog
    //
    hr = g_IShell->GetBasicLog(&spLogger);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to create IBasicLog!");
        return FNS_PASS;
    }

    //
    //  Create a device enumerator
    //
    hr = spDeviceEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: CoCreateInstance(__uuidof(IMMDeviceEnumerator)) returned 0x%08x", hr);
        return FNS_PASS;
    }

    //
    //  Get the endpoint under test
    //
    hr = GetEndpointUnderTest(&spDeviceUnderTest);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: GetEndpointUnderTest! 0x%08X", hr);
        return FNS_PASS;
    }

    if (spDeviceUnderTest == NULL)
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IMMDevice under test!");
        return FNS_PASS;
    }

    //
    //  Set the ducking policy to mute (full attenuation)
    //
    
    if (GetModule()->IsAppContainer())
    {
        CComPtr<IAudioDeviceAccessHelper> pHelper = GetModule()->GetDeviceAccessHelper();
        hr = pHelper->UpdateDuckingPreference(DuckingPreferenceMute);
        if (FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: IAudioDeviceAccessHelper::UpdateDuckingPreference retuned 0x%08X", hr);
            return FNS_PASS;
        }
    }
    else
    {
        DWORD   dwDuckingPreference = DuckingPreferenceMute;
        dwRes = RegSetKeyValueW(HKEY_CURRENT_USER, 
                              DUCKING_PREFERENCE_REGISTRY_KEY, 
                              DUCKING_PREFERENCE_REGISTRY_VALUE, 
                              REG_DWORD, 
                              &dwDuckingPreference, 
                              sizeof(DWORD));
        hr = HRESULT_FROM_WIN32(dwRes);
        if (FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: RegSetKeyValue retuned 0x%08X", hr);
            return FNS_PASS;
        }
    }
    //
    //  Create a non-communications session on the default render endpoint.
    //
    hr = spDeviceEnumerator->GetDefaultAudioEndpoint(eRender,
                                                     eConsole,
                                                     &spDefaultConsoleDevice);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IMMDeviceGetDefaultAudioEndpoint returned 0x%08X", hr);
        return FNS_PASS;
    }
    
    //
    //  Save the id of the endpoint under test
    //
    hr = spDefaultConsoleDevice->GetId(&sp_wszDefaultConsoleDevice);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IMMDevice::GetId returned 0x%08X", hr);
        return FNS_PASS;
    }

    hr = CreateTestStreamingEngineOnMMDevice(__uuidof(IRenderTestStreamingEngine),
                                   spDefaultConsoleDevice,
                                   TIMER_PUSH_RENDER,
                                   spLogger,
                                   reinterpret_cast<void**>(&spNonCommRenderStreamingEngine));
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: CreateTestStreamingEngineOnMMDevice(IID_ICaptureTestStreamingEngine) returned 0x%08X", hr);
        return FNS_PASS;
    }

    hr = spNonCommRenderStreamingEngine->QueryInterface(__uuidof(ITestStreamingEngine),
                                                        reinterpret_cast<void**>(&spNonCommStreamingEngineCommon));
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IRenderTestStreamingEngine::QueryInterface(IID_ITestStreamingEngine) returned 0x%08X", hr);
        return FNS_PASS;
    }

    hr = spNonCommStreamingEngineCommon->Initialize(&clientProperties);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: ITestStreamingEngine::Initialize returned 0x%08X", hr);
        return FNS_PASS;
    }

    //
    //  start the non-communications stream
    //
    hr = spNonCommRenderStreamingEngine->RenderSineTone(DEFAULT_TEST_TONE_FREQ, TEST_TONE_AMP, 0, DITHER_TYPE_TPD, NULL);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IRenderTestStreamingEngine::RenderSineTone returned 0x%08x", hr);
        return FNS_PASS;
    }

    CComPtr<IAudioSessionControl2> spNonCommSessionControl2;
    hr = spNonCommStreamingEngineCommon->GetSessionControlExtended(&spNonCommSessionControl2);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: ITestStreamingEngine::GetSessionControlExtended returned 0x%08x", hr);
        return FNS_PASS;
    }

    CComHeapPtr<WCHAR> sp_wszNonCommSessionID;
    hr = spNonCommSessionControl2->GetSessionInstanceIdentifier(&sp_wszNonCommSessionID);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IAudioSessionControl2::GetSessionIdentifier returned 0x%08x", hr);
        return FNS_PASS;
    }

    SLOG(eInfo1, "Non Communications Session ID: %S", (LPCWSTR)sp_wszNonCommSessionID);

    //
    //  Get an IAudioSessionControlInternal and verify audio is un-ducked
    //
    hr = spNonCommStreamingEngineCommon->GetSessionControlInternal(&spNonCommSessionControlInternal);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: ITestStreamingEngine::GetSessionControlInternal returned 0x%08X", hr);
        return FNS_PASS;
    }

    FLOAT       fDuckingValue = 0.0f;
    hr = spNonCommSessionControlInternal->GetDuckingState(&fDuckingValue);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IAudioSessionControlInternal::GetDuckingState returned 0x%08X", hr);
    }
    else
    {
        // verify we're un-ducked
        if (abs(DUCK_NO_ATTENUATION-fDuckingValue) > EPSILON)
        {
            XLOG(XFAIL, 
                 eError,
                 "FAIL: Expected the stream to be un-ducked (reported scalar attenuation of %.2f)", 
                 fDuckingValue);
        }
    }

    // let audio stream for 1 second
    Sleep(1000);

    //
    //  create a communications stream on the endpoint under test (render OR capture)
    //

    //  Get the default communications device
    if (DEVICE_TYPE_RENDER == GetDeviceTypeUnderTest())
    {
        hr = spDeviceEnumerator->GetDefaultAudioEndpoint(eRender,
                                                         eCommunications,
                                                         &spDefaultCommDevice);
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: IMMDeviceGetDefaultAudioEndpoint returned 0x%08X", hr);
            return FNS_PASS;
        }
        
        //
        //  Save the id of the default communications device
        //
        hr = spDefaultCommDevice->GetId(&sp_wszDefaultCommEndpoint);
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: IMMDevice::GetId returned 0x%08X", hr);
            return FNS_PASS;
        }

        hr = CreateTestStreamingEngineOnMMDevice(__uuidof(IRenderTestStreamingEngine),
                                       spDefaultCommDevice,
                                       TIMER_PUSH_RENDER,
                                       spLogger,
                                       reinterpret_cast<void**>(&spCommRenderStreamingEngine));
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: CreateTestStreamingEngineOnMMDevice(IID_ICaptureTestStreamingEngine) returned 0x%08X", hr);
            return FNS_PASS;
        }

        hr = spCommRenderStreamingEngine->QueryInterface(__uuidof(ITestStreamingEngine),
                                                         reinterpret_cast<void**>(&spCommStreamingEngineCommon));
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: IRenderTestStreamingEngine::QueryInterface(IID_ITestStreamingEngine) returned 0x%08X", hr);
            return FNS_PASS;
        }
    }
    else if (DEVICE_TYPE_CAPTURE == GetDeviceTypeUnderTest())
    {
        hr = spDeviceEnumerator->GetDefaultAudioEndpoint(eCapture,
                                                         eCommunications,
                                                         &spDefaultCommDevice);
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: IMMDeviceGetDefaultAudioEndpoint returned 0x%08X", hr);
            return FNS_PASS;
        }        

        //
        //  Save the id of the default communications device
        //
        hr = spDefaultCommDevice->GetId(&sp_wszDefaultCommEndpoint);
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: IMMDevice::GetId returned 0x%08X", hr);
            return FNS_PASS;
        }

        hr = CreateTestStreamingEngineOnMMDevice(__uuidof(ICaptureTestStreamingEngine),
                                       spDefaultCommDevice,
                                       TIMER_PUSH_CAPTURE,
                                       spLogger,
                                       reinterpret_cast<void**>(&spCommCaptureStreamingEngine));
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: CreateTestStreamingEngineOnMMDevice(IID_ICaptureTestStreamingEngine) returned 0x%08X", hr);
            return FNS_PASS;
        }

        hr = spCommCaptureStreamingEngine->QueryInterface(__uuidof(ITestStreamingEngine),
                                                          reinterpret_cast<void**>(&spCommStreamingEngineCommon));
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: IRenderTestStreamingEngine::QueryInterface(IID_ITestStreamingEngine) returned 0x%08X", hr);
            return FNS_PASS;
        }        
    }
    else
    {
        ATLASSERT(!"this should never happen");
        XLOG(XFAIL, eError, "FAIL: Encountered unknown device type!");
        return FNS_PASS;     
    }

    //  be sure a new session is created so ducking is actually triggered
    hr = CoCreateGuid(&guidCommSessionGuid);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: CoCreateGuid returned 0x%08X", hr);
        return FNS_PASS;
    }

    hr = spCommStreamingEngineCommon->SetRequestedSessionGUID(guidCommSessionGuid);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: ITestStreamingEngine::SetSessionGUID returned 0x%08X", hr);
        return FNS_PASS;
    }

    //
    //  start the communications stream
    //
    hr = spCommStreamingEngineCommon->Initialize(&clientProperties);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: ITestStreamingEngine::Initialize returned 0x%08X", hr);
        return FNS_PASS;
    }
    
    if (DEVICE_TYPE_RENDER == GetDeviceTypeUnderTest())
    {
        hr = spCommRenderStreamingEngine->RenderSineTone(COMM_TEST_TONE_FREQ, TEST_TONE_AMP, 0, DITHER_TYPE_TPD, NULL);
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: IRenderTestStreamingEngine::RenderSineTone returned 0x%08x", hr);
            return FNS_PASS;
        }
    }
    else if (DEVICE_TYPE_CAPTURE == GetDeviceTypeUnderTest())
    {
        hr = spCommCaptureStreamingEngine->NullCapture(0, FALSE);
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: ICaptureTestStreamingEngine::NullCapture returned 0x%08x", hr);
            return FNS_PASS;
        }
    }
    else
    {
        ATLASSERT(!"this should never happen");
        XLOG(XFAIL, eError, "FAIL: Encountered unknown device type!");
        return FNS_PASS;
    }    

    CComPtr<IAudioSessionControl2> spCommSessionControl2;
    hr = spCommStreamingEngineCommon->GetSessionControlExtended(&spCommSessionControl2);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: ITestStreamingEngine::GetSessionControlExtended returned 0x%08x", hr);
        return FNS_PASS;
    }

    CComHeapPtr<WCHAR> sp_wszCommSessionID;
    hr = spCommSessionControl2->GetSessionInstanceIdentifier(&sp_wszCommSessionID);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IAudioSessionControl2::GetSessionIdentifier returned 0x%08x", hr);
        return FNS_PASS;
    }

    SLOG(eInfo1, "Communications Session ID: %S", (LPCWSTR)sp_wszCommSessionID);

    Sleep(1000);

    // Duck only if the communications and non-communications sessions are on the same endpoint
    bool bExpectDucking = wcscmp(sp_wszDefaultCommEndpoint, sp_wszDefaultConsoleDevice) == 0;

    //
    //  Verify that we're ducked.
    //
    fDuckingValue = 0.0f;
    hr = spNonCommSessionControlInternal->GetDuckingState(&fDuckingValue);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IAudioSessionControlInternal::GetDuckingState returned 0x%08X", hr);
    }
    else
    {
        if (bExpectDucking)
        {
            if (abs(DUCK_FULL_ATTENUATION - fDuckingValue) > EPSILON)
            {
                XLOG(XFAIL,
                    eError,
                    "FAIL: Expected the stream to be ducked (reported scalar attenuation of %f)",
                    fDuckingValue);
            }
        }
        else
        {
            if (abs(DUCK_NO_ATTENUATION - fDuckingValue) > EPSILON)
            {
                XLOG(XFAIL,
                    eError,
                    "FAIL: Expected the stream to NOT be ducked since the communications and non communications sessions are on different endpoints (reported scalar attenuation of %f)",
                    fDuckingValue);
            }
        }
    }
    
    //
    //  Stop the communications stream.
    //
    spCommCaptureStreamingEngine = NULL;
    spCommRenderStreamingEngine = NULL;
    spCommStreamingEngineCommon = NULL;

    Sleep(1000);

    //
    //  Verify that we're un-ducked
    //
    fDuckingValue = 0.0f;
    hr = spNonCommSessionControlInternal->GetDuckingState(&fDuckingValue);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: IAudioSessionControlInternal::GetDuckingState returned 0x%08X", hr);
    }
    else
    {
        if (abs(DUCK_NO_ATTENUATION-fDuckingValue)>EPSILON)
        {
            XLOG(XFAIL, 
                 eError,
                 "FAIL: Expected the stream to be un-ducked (reported scalar attenuation of %.2f)", 
                 fDuckingValue);
        }
    }    

    return FNS_PASS;
}
