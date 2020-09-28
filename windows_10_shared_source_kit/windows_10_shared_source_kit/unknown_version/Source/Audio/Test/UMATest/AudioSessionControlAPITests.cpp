// Copyright(C) Microsoft. All rights reserved.

//
// AudioSessionControlAPITests.cpp
//  
//
// Description:
//
//  WASAPI ISimpleAudioVolume API Tests

#include <stdafx.h>
#include <UMATestStreaming.h>

#include "TestUtilities.h"

#include "StreamingControlTestFramework.h"
#include "AudioSessionEventHandler.h"

// GUID used for Volume API Test Contexts
// {3BE92C1F-253B-4d40-8EC4-6B6B3D75D8C8}
static const GUID GUID_AUDIOSESSION_TEST = 
{ 0x3be92c1f, 0x253b, 0x4d40, { 0x8e, 0xc4, 0x6b, 0x6b, 0x3d, 0x75, 0xd8, 0xc8 } };



DWORD TC_AudioSessionControl_GetSessionID_Valid0()
{
    RunStreamingControlTest(AUDIOSESSIONCONTROL_GETSESSIONIDENTIFIER_VALID0);
    return FNS_PASS;       
}

DWORD TC_AudioSessionControl_GetState_Valid0()
{
    RunStreamingControlTest(AUDIOSESSIONCONTROL_GETSTATE_VALID0);
    return FNS_PASS;
}

DWORD TC_AudioSessionControl_GetState_Valid1()
{
    RunStreamingControlTest(AUDIOSESSIONCONTROL_GETSTATE_VALID1);
    return FNS_PASS;
}

DWORD TC_AudioSessionControl_GetLastActication_Valid0()
{
    RunStreamingControlTest(AUDIOSESSIONCONTROL_GETLASTACTIVATION_VALID0);
    return FNS_PASS;
}

DWORD TC_AudioSessionControl_GetLastActication_Valid1()
{
    RunStreamingControlTest(AUDIOSESSIONCONTROL_GETLASTACTIVATION_VALID1);
    return FNS_PASS;
}

DWORD TC_AudioSessionControl_GetLastInactivation_Valid0()
{
    RunStreamingControlTest(AUDIOSESSIONCONTROL_GETLASTINACTIVATION_VALID0);
    return FNS_PASS;
}

DWORD TC_AudioSessionControl_GetLastInactivation_Valid1()
{
    RunStreamingControlTest(AUDIOSESSIONCONTROL_GETLASTINACTIVATION_VALID1);
    return FNS_PASS;
}

DWORD TC_AudioSessionControl_GetDisplayName_Valid0()
{
    RunStreamingControlTest(AUDIOSESSIONCONTROL_GETDISPLAYNAME_VALID0);
    return FNS_PASS;
}

DWORD TC_AudioSessionControl_SetDisplayName_Valid0()
{
    RunStreamingControlTest(AUDIOSESSIONCONTROL_SETDISPLAYNAME_VALID0);
    return FNS_PASS;
}

DWORD TC_AudioSessionControl_SetDisplayName_Valid1()
{
    RunStreamingControlTest(AUDIOSESSIONCONTROL_SETDISPLAYNAME_VALID1);
    return FNS_PASS;
}

DWORD TC_AudioSessionControl_GetIconPath_Valid0()
{
    RunStreamingControlTest(AUDIOSESSIONCONTROL_GETICONPATH_VALID0);
    return FNS_PASS;
}

DWORD TC_AudioSessionControl_SetIconPath_Valid0()
{
    RunStreamingControlTest(AUDIOSESSIONCONTROL_SETICONPATH_VALID0);
    return FNS_PASS;
}

DWORD TC_AudioSessionControl_GetGroupingParam_Valid0()
{
    RunStreamingControlTest(AUDIOSESSIONCONTROL_GETGROUPINGPARAM_VALID0);
    return FNS_PASS;
}

DWORD TC_AudioSessionControl_SetGroupingParam_Valid0()
{
    RunStreamingControlTest(AUDIOSESSIONCONTROL_SETGROUPINGPARAM_VALID0);
    return FNS_PASS;
}

DWORD TC_AudioSessionControl_Notifications_Valid0()
{
    RunStreamingControlTest(AUDIOSESSIONCONTROL_NOTIFICATIONS_VALID0);
    return FNS_PASS;
}

DWORD TC_AudioSessionControl_Notifications_Valid1()
{
    RunStreamingControlTest(AUDIOSESSIONCONTROL_NOTIFICATIONS_VALID1);
    return FNS_PASS;
}

DWORD TC_AudioSessionControl_GetProcessId()
{
    RunStreamingControlTest(AUDIOSESSIONCONTROL_GETPROCESSID_VALID0);
    return FNS_PASS;
}

DWORD TC_AudioSessionControl_GetSessionID_Invalid0()
{
    RunStreamingControlTest(AUDIOSESSIONCONTROL_GETSESSIONID_INVALID0);
    return FNS_PASS;
}

DWORD TC_AudioSessionControl_GetState_Invalid0()
{
    RunStreamingControlTest(AUDIOSESSIONCONTROL_GETSTATE_INVALID0);
    return FNS_PASS;
}

DWORD TC_AudioSessionControl_GetLastActivation_Invalid0()
{
    RunStreamingControlTest(AUDIOSESSIONCONTROL_GETLASTACTIVATION_INVALID0);
    return FNS_PASS;
}

DWORD TC_AudioSessionControl_GetLastInactivation_Invalid0()
{
    RunStreamingControlTest(AUDIOSESSIONCONTROL_GETLASTINACTIVATION_INVALID0);
    return FNS_PASS;
}

DWORD TC_AudioSessionControl_GetDisplayName_Invalid0()
{
    RunStreamingControlTest(AUDIOSESSIONCONTROL_GETDISPLAYNAME_INVALID0);
    return FNS_PASS;
}

DWORD TC_AudioSessionControl_SetDisplayName_Invalid0()
{
    RunStreamingControlTest(AUDIOSESSIONCONTROL_SETDISPLAYNAME_INVALID0);
    return FNS_PASS;
}

DWORD TC_AudioSessionControl_GetGroupingParam_Invalid0()
{
    RunStreamingControlTest(AUDIOSESSIONCONTROL_GETGROUPINGPARAM_INVALID0);
    return FNS_PASS;
}

DWORD TC_AudioSessionControl_AddAudioSesNotif_Invalid0()
{
    RunStreamingControlTest(AUDIOSESSIONCONTROL_ADDAUDIOSESNOTIF_INVALID0);
    return FNS_PASS;
}

DWORD TC_AudioSessionControl_DelAudioSesNotif_Invalid0()
{
    RunStreamingControlTest(AUDIOSESSIONCONTROL_DELAUDIOSESNOTIF_INVALID0);
    return FNS_PASS;
}

DWORD TC_AudioSessionControl_GetProcessId_Invalid0()
{
    RunStreamingControlTest(AUDIOSESSIONCONTROL_GETPROCESSID_INVALID0);
    return FNS_PASS;
}

DWORD TC_AudioSessionControl_DisconnectService_Invalid0()
{
    RunStreamingControlTest(AUDIOSESSIONCONTROL_DISCONNECTSERVICE_INVALID0);
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: Basic verification of the IAudioSessionControl::GetIdentifier/GetInstanceIdentifier methods
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_AudioSessionControl_GetSessionID_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl2> spAudSesCtrlEx = NULL;

    LPWSTR wszId1 = NULL;
    LPWSTR wszId2 = NULL;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControlExtended(&spAudSesCtrlEx);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl2 Interface (0x%x)");
        goto END;
    }

    METHODCHECKRESULT("IAudioSessionControl2::GetSessionIdentifier",
                      hr,
                      spAudSesCtrlEx->GetSessionIdentifier(&wszId1));
    if(S_OK == hr)
    {
        SLOG(eInfo1, "Session Identifier: %S", wszId1);
        SAFE_COTASKMEMFREE(wszId1);
    }
    else
    {
        XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");
    }

    METHODCHECKRESULT("IAudioSessionControl2::GetSessionInstanceIdentifier",
                      hr,
                      spAudSesCtrlEx->GetSessionInstanceIdentifier(&wszId2));
    if(S_OK == hr)
    {
        SLOG(eInfo1, "Session Identifier: %S", wszId2);
        SAFE_COTASKMEMFREE(wszId2);
    }
    else
    {
        XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");
    }

END:
    return FNS_PASS;
}

DWORD Test_AudioSessionControl_GetState_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl> spAudSesCtrl = NULL;
    AudioSessionState audiosessionstate;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControl(&spAudSesCtrl);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl Interface");
        goto END;
    }

    METHODCHECKRESULT("IAudioSessionControl::GetState",
                      hr,
                      spAudSesCtrl->GetState(&audiosessionstate));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");
        goto END;
    }

END:
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: Verification of the IAudioSessionControl::GetState method
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_AudioSessionControl_GetState_Valid1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl> spAudSesCtrl = NULL;
    AudioSessionState audiosessionstate;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControl(&spAudSesCtrl);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl Interface");
        goto END;
    }

    // Give the session time to transition into the active state
    Sleep(500);

    METHODCHECKRESULT("IAudioSessionControl::GetState",
                      hr,
                      spAudSesCtrl->GetState(&audiosessionstate));

    if(S_OK == hr)
    {
        switch(audiosessionstate)
        {
        case AudioSessionStateInactive:
            XLOG(XFAIL, eError,TEXT("FAIL: AudioSessionState is Inactive. This is NOT as expected."));
            break;
        case AudioSessionStateActive:
            SLOG(eInfo1,TEXT("AudioSessionState is Active as expected."));
            break;
        default:
            XLOG(XFAIL, eError,TEXT("FAIL: AudioSessionState is an unknown definition. This is NOT as expected."));
            break;
        }
    }

END:    
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: Basic verification of the 
//       IAudioSessionControlInternal::GetLastActivation method
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_AudioSessionControl_GetLastActivation_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControlInternal> spAudSesCtrlInt = NULL;
    DATE date = 0;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControlInternal(&spAudSesCtrlInt);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControlInternal Interface");
        goto END;
    }

    METHODCHECKRESULT("IAudioSessionControl::GetLastActivation",
                      hr,
                      spAudSesCtrlInt->GetLastActivation(&date));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");
    }
    if(S_OK == hr)
    {
        LogDate(date);
    }   

    METHODCHECKRESULT("IAudioSession::GetLastActivation",
                      hr,
                      spAudSesCtrlInt->GetLastActivation(&date));

    METHODCHECKRESULT("VerifyDate",
                      hr,
                      VerifyDate(date));

END:
    return FNS_PASS;
}
    
//------------------------------------------------------------------------------
// Desc: Verification of the IAudioSessionControlInternal::GetLastActivation method
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_AudioSessionControl_GetLastActivation_Valid1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControlInternal> spAudSesCtrlInt = NULL;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControlInternal(&spAudSesCtrlInt);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControlInternal Interface");
        goto END;
    }



END:
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: Basic verification of the 
//       IAudioSessionControlInternal::GetLastInacitivation method
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_AudioSessionControl_GetLastInactivation_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControlInternal> spAudSesCtrlInt = NULL;
    DATE date = 0;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControlInternal(&spAudSesCtrlInt);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControlInternal Interface");
        goto END;
    }

    METHODCHECKRESULT("IAudioSessionControl::GetLastInactivation",
                      hr,
                      spAudSesCtrlInt->GetLastInactivation(&date));

    if(S_OK == hr)
    {
        LogDate(date);
    }
    else
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT!");
    }

END:
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: Verification of the IAudioSessionControlInternal::GetLastInactivation method
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_AudioSessionControl_GetLastInactivation_Valid1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControlInternal> spAudSesCtrlInt = NULL;
    DATE date = 0;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControlInternal(&spAudSesCtrlInt);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControlInternal Interface");
        goto END;
    }

    METHODCHECKRESULT("IAudioSession::GetLastInactivation",
                      hr,
                      spAudSesCtrlInt->GetLastInactivation(&date));


    METHODCHECKRESULT("VerifyDate",
                      hr,
                      VerifyDate(date));


END:
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: Basic verification of the 
//       IAudioSessionControl::GetDisplayName method
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_AudioSessionControl_GetDisplayName_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl> spAudSesCtrl = NULL;
    LPWSTR pwszString = NULL;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControl(&spAudSesCtrl);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl Interface");
        goto END;
    }

    METHODCHECKRESULT("IAudioSessionControl::GetDisplayName",
                      hr,
                      spAudSesCtrl->GetDisplayName(&pwszString));
    if(S_OK == hr)
    {
        SLOG(eInfo1,"Display Name: %ws", pwszString);
    }
    else
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
    }

END:                
    SAFE_COTASKMEMFREE(pwszString);
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: Basic verification of the 
//       IAudioSessionControl::SetDisplayName method
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_AudioSessionControl_SetDisplayName_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl> spAudSesCtrl = NULL;
    WCHAR* wszStr = NULL;
    WCHAR* wszNewStr = NULL;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControl(&spAudSesCtrl);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl Interface");
        goto END;
    }

    wszStr = L"NEW AUDIOSESSION DISPLAY NAME";

    METHODCHECKRESULT("IAudioSessionControl::SetDisplayName",
                      hr,
                      spAudSesCtrl->SetDisplayName(wszStr, &GUID_AUDIOSESSION_TEST));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    METHODCHECKRESULT("IAudioSessionControl::GetDisplayName",
                      hr,
                      spAudSesCtrl->GetDisplayName(&wszNewStr));

    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    if(0 != wcscmp(wszNewStr,wszStr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Get (%ws) and set (%ws) strings do not match!",wszNewStr,wszStr);
    }
    
END:
    SAFE_COTASKMEMFREE(wszNewStr);
    return FNS_PASS;
}

DWORD Test_AudioSessionControl_SetDisplayName_Valid1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl> spAudSesCtrl = NULL;
    LPWSTR pwGetStr = NULL;
    LPWSTR pwStr = L"NEW AUDIOSESSION DISPLAY NAME";

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControl(&spAudSesCtrl);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl Interface");
        goto END;
    }

    METHODCHECKRESULT("IAudioSessionControl::SetDisplayName",
                      hr,
                      spAudSesCtrl->SetDisplayName(pwStr, &GUID_AUDIOSESSION_TEST));
    
    METHODCHECKRESULT("IAudioSessionControl::GetDisplayName",
                      hr,
                      spAudSesCtrl->GetDisplayName(&pwGetStr));

    if(NULL == pwGetStr)
    {
        XLOG(XFAIL, eError, "FAIL: GetDisplayName returned NULL!");
    }
    else if(0 != wcscmp(pwStr,pwGetStr))
    {
        XLOG(XFAIL, eError, "FAIL: Get (%ws) and Set (%ws) strings do not match", pwGetStr, pwStr);
    }
    
END:
    SAFE_COTASKMEMFREE(pwGetStr);
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: Basic verification of the 
//       IAudioSessionControl::GetIconPath method
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_AudioSessionControl_GetIconPath_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl> spAudSesCtrl = NULL;
    LPWSTR pwszString = NULL;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControl(&spAudSesCtrl);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl Interface");
        goto END;
    }

    METHODCHECKRESULT("IAudioSessionControl::GetIconPath",
                      hr,
                      spAudSesCtrl->GetIconPath(&pwszString));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    SLOG(eInfo1,"Icon Path: %ws",spAudSesCtrl);                
    
END:
    SAFE_COTASKMEMFREE(pwszString);
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: Basic verification of the 
//       IAudioSessionControl::SetIconPath method
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_AudioSessionControl_SetIconPath_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl> spAudSesCtrl = NULL;
    WCHAR* pwszStr = NULL;
    WCHAR* pwszNewStr = NULL;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControl(&spAudSesCtrl);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl Interface");
        goto END;
    }

    pwszStr = L"+-+-+-+-NEW AUDIOSESSION ICON PATH-+-+-+-+";

    METHODCHECKRESULT("IAudioSessionControl::SetIconPath",
                      hr,
                      spAudSesCtrl->SetIconPath(pwszStr, &GUID_AUDIOSESSION_TEST));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected result");
        goto END;
    }
    

    METHODCHECKRESULT("IAudioSessionControl::GetIconPath",
                      hr,
                      spAudSesCtrl->GetIconPath(&pwszNewStr));

    if((NULL == pwszNewStr)||(FAILED(hr)))
    {
        XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected result");
        goto END;
    }

    if(0 != wcscmp(pwszNewStr,pwszStr))
    {
        XLOG(XFAIL, eError, "FAIL: Get (%ws) and set (%ws) strings do not match!",pwszNewStr,pwszStr);
    }

END:
    SAFE_COTASKMEMFREE(pwszNewStr);
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: Basic verification of the 
//       IAudioSessionControl::GetGroupingParam method
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_AudioSessionControl_GetGroupingParam_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl> spAudSesCtrl = NULL;
    GUID guid_get = GUID_NULL;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControl(&spAudSesCtrl);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl Interface");
        goto END;
    }

    METHODCHECKRESULT("IAudioSessionControl::GetGroupingParam",
                      hr,
                      spAudSesCtrl->GetGroupingParam(&guid_get));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    LogGUID(guid_get);

END:
    return FNS_PASS;

}

//------------------------------------------------------------------------------
// Desc: Basic verification of the 
//       IAudioSessionControl::SetGroupingPram method
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_AudioSessionControl_SetGroupingParam_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl> spAudSesCtrl = NULL;
    GUID guid_get = GUID_NULL;
    GUID guid_put = GUID_NULL;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControl(&spAudSesCtrl);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl Interface");
        goto END;
    }

    METHODCHECKRESULT("CoCreateGuid",
                      hr,
                      CoCreateGuid(&guid_put));        
    if(FAILED(hr)) 
    { 
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected result");
    }


    METHODCHECKRESULT("IAudioSessionControl::SetGroupingParam",
                      hr,
                      spAudSesCtrl->SetGroupingParam(&guid_put, &GUID_AUDIOSESSION_TEST));        
    if(FAILED(hr)) 
    { 
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected result");
        goto END;
    }

    // get the session class guid
    METHODCHECKRESULT("IAudioSessionControl::GetGroupingParam",
                  hr,
                  spAudSesCtrl->GetGroupingParam(&guid_get));        
    if(FAILED(hr)) 
    { 
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected result");
        goto END; 
    }

    if(!IsEqualGUID(guid_get,guid_put))
    {
        XLOG(XFAIL,eError,"FAIL: GetGroupingParam did not return the expected value.");         
        SLOG(eError, "Set/Get GUIDS: ");
        LogGUID(guid_put);
        LogGUID(guid_get);
    }

END:
    return FNS_PASS;   
}

//------------------------------------------------------------------------------
// Desc: Basic verification of the 
//       IAudioSessionControl::Add/DeleteSessionNotification methods
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_AudioSessionControl_Notifications_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl> spAudSesCtrl = NULL;
    CComPtr<IAudioSessionEvents> spAudioSessionEvents = NULL;
    CAudioSessionEventHandler* pSessionEventHandler = NULL;

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
    
    METHODCHECKRESULT("IAudioSessionControl::UnregisterAudioSessionNotification",
                      hr,
                      spAudSesCtrl->UnregisterAudioSessionNotification(spAudioSessionEvents));
    if(FAILED(hr)) 
    {
        XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected result!");
    }

END:
    SAFE_RELEASE_POINTER(pSessionEventHandler);
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: IAudioSessionControl no-delete session notification scenario
//
// Returns:
//              FNS_PASS if the test passes
//              FNS_FAIL otherwise
//------------------------------------------------------------------------------
DWORD Test_AudioSessionControl_Notifications_Valid1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl> spAudSesCtrl = NULL;
    CComPtr<IAudioSessionEvents> spAudioSessionEvents = NULL;
    CAudioSessionEventHandler* pSessionEventHandler = NULL;

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

END:
    SAFE_RELEASE_POINTER(pSessionEventHandler);
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: Basic verification of the 
//       IAudioSessionControl2::GetProcessId method
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_AudioSessionControl_GetProcessId_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl2> spAudSesCtrlEx = NULL;
    DWORD dwPid = 0;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControlExtended(&spAudSesCtrlEx);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl2 Interface");
        goto END;
    }

    METHODCHECKRESULT("IAudioSessionControl::GetProcessId",
                      hr,
                      spAudSesCtrlEx->GetProcessId(&dwPid));

    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected value");
        goto END;
    }
   
    SLOG(eInfo1, "PID: %d" , dwPid);

    if(0 == dwPid)
    {
        XLOG(XFAIL, eError, "FAIL: PID not set!");
    }
    
END:    
    return hr;
}

//------------------------------------------------------------------------------
// Desc: IAudioSessionControl::GetId invalid scenarios
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_AudioSessionControl_GetSessionID_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl2> spAudSesCtrlEx = NULL;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControlExtended(&spAudSesCtrlEx);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl2 Interface (0x%8x)");
        goto END;
    }

#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioSessionControl2::GetSessionIdentifier",
                      hr,
                      spAudSesCtrlEx->GetSessionIdentifier(NULL));
    if(SUCCEEDED(hr)) 
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected value");
    }

#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioSessionControl2::GetSessionInstanceIdentifier",
                      hr,
                      spAudSesCtrlEx->GetSessionInstanceIdentifier(NULL));
    if(SUCCEEDED(hr)) 
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected value");
    }

END:
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: IAudioSessionControl::GetState invalid scenarios
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_AudioSessionControl_GetState_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl> spAudSesCtrl = NULL;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControl(&spAudSesCtrl);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl Interface");
        goto END;
    }

#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioSessionControl::GetState",
                      hr,
                      spAudSesCtrl->GetState(NULL));
    if(SUCCEEDED(hr)) 
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected value");
    }
    
END:    
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: IAudioSessionControl::GetLastActivation invalid scenarios
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_AudioSessionControl_GetLastActivation_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControlInternal> spAudSesCtrlInt = NULL;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControlInternal(&spAudSesCtrlInt);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControlInternal Interface");
        goto END;
    }

#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioSessionControlInternal::GetLastActivation",
                      hr,
                      spAudSesCtrlInt->GetLastActivation(NULL));
    if(SUCCEEDED(hr)) 
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected value");
    }

END:
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: IAudioSessionControl::GetLastInactivation invalid scenarios
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_AudioSessionControl_GetLastInactivation_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControlInternal> spAudSesCtrlInt = NULL;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControlInternal(&spAudSesCtrlInt);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControlInternal Interface");
        goto END;
    }

#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioSessionControlInternal::GetLastInactivation",
                      hr,
                      spAudSesCtrlInt->GetLastInactivation(NULL));
    if(SUCCEEDED(hr)) 
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected value");
    }

END:
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: IAudioSessionControl::GetDisplayName invalid scenarios
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_AudioSessionControl_GetDisplayName_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl> spAudSesCtrl = NULL;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControl(&spAudSesCtrl);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl Interface");
        goto END;
    }

#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioSessionControl::GetDisplayName",
                      hr,
                      spAudSesCtrl->GetDisplayName(NULL));
    if(SUCCEEDED(hr)) 
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected value");
    }
    
END:    
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: IAudioSessionControl::SetDisplayName invalid scenarios
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_AudioSessionControl_SetDisplayName_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl> spAudSesCtrl = NULL;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControl(&spAudSesCtrl);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl Interface");
        goto END;
    }

#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioSessionControl::SetDisplayName",
                      hr,
                      spAudSesCtrl->SetDisplayName(NULL, &GUID_AUDIOSESSION_TEST));
    if(SUCCEEDED(hr)) 
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected value");
    }
    
END:    
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: IAudioSessionControl::GetGroupingParam invalid scenarios
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_AudioSessionControl_GetGroupingParam_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl> spAudSesCtrl = NULL;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControl(&spAudSesCtrl);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl Interface");
        goto END;
    }

#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioSessionControl::GetGroupingParam",
                      hr,
                      spAudSesCtrl->GetGroupingParam(NULL));
    if(SUCCEEDED(hr)) 
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected value");
    }
    
END:    
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: IAudioSessionControl::RegisterAudioSessionNotification invalid scenarios
//
// Returns:
//              FNS_PASS if the test passes
//              FNS_FAIL otherwise
//------------------------------------------------------------------------------
DWORD Test_AudioSessionControl_AddAudioSesNotif_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl> spAudSesCtrl = NULL;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControl(&spAudSesCtrl);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl Interface");
        goto END;
    }

#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioSessionControl::RegisterAudioSessionNotification",
                      hr,
                      spAudSesCtrl->RegisterAudioSessionNotification(NULL));
    if(SUCCEEDED(hr)) 
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected value");
    }
    
END:    
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: IAudioSessionControl::DeleteAudioSessionNotification invalid scenarios
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_AudioSessionControl_DelAudioSesNotif_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl> spAudSesCtrl = NULL;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControl(&spAudSesCtrl);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl Interface");
        goto END;
    }

#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioSessionControl::UnregisterAudioSessionNotification",
                      hr,
                      spAudSesCtrl->UnregisterAudioSessionNotification(NULL));
    if(SUCCEEDED(hr)) 
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected value");
    }
    
END:    
    return FNS_PASS;

}

//------------------------------------------------------------------------------
// Desc: IAudioSessionControl::GetProcessId invalid scenarios
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_AudioSessionControl_GetProcessId_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl2> spAudSesCtrlEx = NULL;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControlExtended(&spAudSesCtrlEx);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl2 Interface");
        goto END;
    }

#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioSessionControl2::GetProcessId",
                      hr,
                      spAudSesCtrlEx->GetProcessId(NULL));
    if(SUCCEEDED(hr)) 
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected value");
    }

END:
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: IAudioSessionControl RPC failure scenarios
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG
//------------------------------------------------------------------------------
DWORD Test_AudioSessionControl_DisconnectService_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;

    CComPtr<IAudioSessionControl> spAudSesCtrl = NULL;
    CComPtr<IAudioSessionControl2> spAudSesCtrlEx = NULL;
    CComPtr<IAudioSessionControlInternal> spAudSesCtrlInt = NULL;

    LPWSTR wszString = NULL;
    LPWSTR wszValidString = L"Test";
    GUID    guidGroupingParam =  GUID_NULL;
    AudioSessionState AudSessState;
    DATE sDate = 0;
    DWORD dwPid = 0;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetSessionControl(&spAudSesCtrl);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl Interface");
        goto END;
    }

    hr = pStreamingEngine->GetSessionControlExtended(&spAudSesCtrlEx);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControl2 Interface");
        goto END;
    }

    hr = pStreamingEngine->GetSessionControlInternal(&spAudSesCtrlInt);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioSessionControlInternal Interface");
        goto END;
    }

    // shut down audiosrv
    if (!StopAudioService())
    {
        XLOG(XFAIL, eError, "FAIL: Unable to stop AudioSrv!");
        goto END;
    }

    // BEGIN IAudioSessionControl API Calls
    ////

    // GetState
    METHODCHECKRESULT("IAudioSessionControl::GetState",
                      hr,
                      spAudSesCtrl->GetState(&AudSessState));
    if(SUCCEEDED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should have failed");
     
    }

    // GetDisplayName
    METHODCHECKRESULT("IAudioSessionControl::GetDisplayName",
                      hr,
                      spAudSesCtrl->GetDisplayName(&wszString));
    SAFE_COTASKMEMFREE(wszString);
    if(SUCCEEDED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should have failed");

    }

    // SetDisplayName
    METHODCHECKRESULT("IAudioSessionControl::SetDisplayName",
                      hr,
                      spAudSesCtrl->SetDisplayName(wszValidString, &GUID_AUDIOSESSION_TEST));
    if(SUCCEEDED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should have failed");
    }

    //GetGroupingParam
    METHODCHECKRESULT("IAudioSessionControl::GetGroupingParam",
                      hr,
                      spAudSesCtrl->GetGroupingParam(&guidGroupingParam));
    if(SUCCEEDED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should have failed");
    }

    //SetGroupingParam
    METHODCHECKRESULT("IAudioSessionControl::SetGroupingParam",
                      hr,
                      spAudSesCtrl->SetGroupingParam(&guidGroupingParam, &GUID_AUDIOSESSION_TEST));
    if(SUCCEEDED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should have failed");

    }

    // GetProcessId
    METHODCHECKRESULT("IAudioSessionControl2::GetProcessId",
                      hr,
                      spAudSesCtrlEx->GetProcessId(&dwPid));
    if(SUCCEEDED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should have failed");
    }    

    // GetLastActivation
    METHODCHECKRESULT("IAudioSessionControlInternal::GetLastActivation",
                      hr,
                      spAudSesCtrlInt->GetLastActivation(&sDate));
    if(SUCCEEDED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should have failed");
    }    

    // GetLastInactivation
    METHODCHECKRESULT("IAudioSessionControlInternal::GetLastInactivation",
                      hr,
                      spAudSesCtrlInt->GetLastInactivation(&sDate));
    if(SUCCEEDED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should have failed");
    }    

    ////
    // END IAudioSessionControl API Calls

END:
    // Start audiosrv back up. Moving this into end section so that even if this function fails, the following test cases do not fail.
    if (!StartAudioService())
    {
        XLOG(XFAIL, eFatalError, "FAIL: Unable to re-start AudioSrv! System may be in an unusable state!");
    }   

    return FNS_PASS;
}

