// Copyright(C) Microsoft. All rights reserved.

//
// StreamingControlTestFramework.cpp
//  
//
// Description:
//
//  Helper routines for streaming control interface tests

#include <stdafx.h>
#include <UMATestStreaming.h>

#include "StreamingControlTestFramework.h"

const DOUBLE TEST_TONE_FREQUENCY = 750.0;   // Render test tones at 750Hz
const DOUBLE TEST_TONE_AMPLITUDE = 0.01;     // Render test tones nice and quiet-like


DWORD RunStreamingControlTest(SCONTROL_TEST_ID nTestID, BOOL bSuppressStreamingErrors)
{
    if(DEVICE_TYPE_RENDER == GetDeviceTypeUnderTest())
    {
        RunRenderStreamingControlTest(nTestID, bSuppressStreamingErrors, FALSE);
    }
    else if(DEVICE_TYPE_RENDEROFFLOAD == GetDeviceTypeUnderTest())
    {
        RunRenderStreamingControlTest(nTestID, bSuppressStreamingErrors, TRUE);
    }
    else if(DEVICE_TYPE_CAPTURE == GetDeviceTypeUnderTest())
    {
        RunCaptureStreamingControlTest(nTestID, bSuppressStreamingErrors);
    }
    else
    {
        XLOG(XFAIL, eError, "FAIL: Endpoint under test does not appear to support streaming");
    }
    
    return FNS_PASS;
}

DWORD RunRenderStreamingControlTest(SCONTROL_TEST_ID nTestID, BOOL bSuppressStreamingErrors, BOOL bEnableHardwareOffload)
{
    HRESULT hr = S_OK;
    AudioClientProperties clientProperties = {0};
    CComPtr<ITestStreamingEngine> spRenderEngineCommon = NULL;
    CComPtr<IRenderTestStreamingEngine> spRenderEngine = NULL;
    LPCWSTR pwszEndpointUnderTest = GetEndpointIDUnderTest();
    CComPtr<IBasicLog> pobjLogger = NULL;
    
    clientProperties.bIsOffload = (FALSE != bEnableHardwareOffload);
    clientProperties.eCategory = (bEnableHardwareOffload ? AudioCategory_Media : AudioCategory_Other);
    clientProperties.cbSize = sizeof(clientProperties);

    hr = g_IShell->GetBasicLog(&pobjLogger);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to create IBasicLog!");
        goto END;
    }    
    
    if(NULL == pwszEndpointUnderTest)
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain endpoint ID for endpoint under test");
        goto END;
    }

    if(bSuppressStreamingErrors)
    {
        SLOG(eInfo1, "WARNING: Suppressing streaming errors!");
    }

    METHODCHECKRESULT("CreateTestStreamingEngineOnEndpointId()",
                      hr,
                      CreateTestStreamingEngineOnEndpointId(IID_IRenderTestStreamingEngine,
                                                pwszEndpointUnderTest,
                                                bEnableHardwareOffload ? EVENT_PULL_RENDER : TIMER_PUSH_RENDER,
                                                pobjLogger,
                                                (PVOID*)(&spRenderEngine)));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to create Render Test Streaming Engine!");
        goto END;
    }

    METHODCHECKRESULT("IRenderTestStreamingEngine::QueryInterface",
                      hr,
                      spRenderEngine->QueryInterface(IID_ITestStreamingEngine,
                                                     (PVOID*)&spRenderEngineCommon));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to QI IRenderTestStreamingEngine for ITestStreamingEngine");
        goto END;
    }

    METHODCHECKRESULT("ITestStreamingEngine::Initialize()",
                      hr,
                      spRenderEngineCommon->Initialize(&clientProperties));

    if(hr == UTS_E_OFFLOAD_NOT_SUPPORTED)
    {
    	XLOG(XSKIP, eError, "SKIP: Render Endpoint does not support offload");
    	goto SKIPPED;
    }
    else if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to initialize render engine");
        goto END;
    }

    METHODCHECKRESULT("IRenderTestStreamingEngine::RenderSineToneAsync",
                      hr,
                      spRenderEngine->RenderSineTone(TEST_TONE_FREQUENCY,
                                                     TEST_TONE_AMPLITUDE,
                                                     0,
                                                     DITHER_TYPE_TPD,
                                                     NULL));
    if(FAILED(hr))
    {
        if(TRUE == bSuppressStreamingErrors)
        {
            SLOG(eError, "WARNING: Error rendering sine tone");
        }
        else
        {
            XLOG(XFAIL, eError, "FAIL: Error rendering sine tone");
        }
        goto END;           
    }

    //
    // Run the actual test
    //
    DispatchTest(nTestID, DEVICE_TYPE_RENDER, spRenderEngineCommon, bSuppressStreamingErrors);

    METHODCHECKRESULT("IRenderTestStreamingEngine::StopStreaming",
                      hr,
                      spRenderEngine->StopStreaming());
    if(FAILED(hr))
    {
        if(TRUE == bSuppressStreamingErrors)       
        {
            // warn but don't fail
            SLOG(eError, "WARNING: Error calling IRenderTestStreamingEngine::StopStreaming");
        }
        else
        {
            XLOG(XFAIL, eError, "FAIL: Error calling IRenderTestStreamingEngine::StopStreaming");
        }        
    }

END:
    return FNS_PASS;

SKIPPED:
	return FNS_SKIPPED;
}


DWORD RunCaptureStreamingControlTest(SCONTROL_TEST_ID nTestID, BOOL bSuppressStreamingErrors)
{
    HRESULT hr = S_OK;
    AudioClientProperties clientProperties = {0};
    CComPtr<ITestStreamingEngine> spCaptureEngineCommon = NULL;
    CComPtr<ICaptureTestStreamingEngine> spCaptureEngine = NULL;
    LPCWSTR pwszEndpointUnderTest = GetEndpointIDUnderTest();
    CComPtr<IBasicLog> pobjLogger = NULL;
    
    clientProperties.bIsOffload = FALSE;
    clientProperties.eCategory = AudioCategory_Other;
    clientProperties.cbSize = sizeof(clientProperties);

    hr = g_IShell->GetBasicLog(&pobjLogger);
    
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to create IBasicLog!");
        goto END;
    }
    
    if(NULL == pwszEndpointUnderTest)
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain endpoint ID for endpoint under test");
        goto END;
    }

    if(bSuppressStreamingErrors)
    {
        SLOG(eInfo1, "WARNING: Suppressing streaming errors!");
    }

    METHODCHECKRESULT("CreateTestStreamingEngineOnEndpointId()",
                      hr,
                      CreateTestStreamingEngineOnEndpointId(IID_ICaptureTestStreamingEngine,
                                                pwszEndpointUnderTest,
                                                TIMER_PUSH_CAPTURE,                                                
                                                pobjLogger,
                                                (PVOID*)(&spCaptureEngine)));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to create Capture Test Streaming Engine!");
        goto END;
    }

    METHODCHECKRESULT("ICaptureTestStreamingEngine::QueryInterface",
                      hr,
                      spCaptureEngine->QueryInterface(IID_ITestStreamingEngine,
                                                      (PVOID*)&spCaptureEngineCommon));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to QI ICaptureTestStreamingEngine for ITestStreamingEngine");
        goto END;
    }

    METHODCHECKRESULT("ITestStreamingEngine::Initialize()",
                      hr,
                      spCaptureEngineCommon->Initialize(&clientProperties));
    if(FAILED(hr))
    {
        if(TRUE == bSuppressStreamingErrors)
        {
            SLOG(eError, "WARNING: Unable to initialize capture engine");
        }
        else
        {
            XLOG(XFAIL, eError, "FAIL: Unable to initialize capture engine");
        }
        goto END;
    }

    METHODCHECKRESULT("ICaptureTestStreamingEngine::NullCapture",
                      hr,
                      spCaptureEngine->NullCapture(0, NULL));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error initiating capture");
        goto END;
    }

    //
    // Run the actual test
    //
    DispatchTest(nTestID, DEVICE_TYPE_CAPTURE, spCaptureEngineCommon, bSuppressStreamingErrors);

    METHODCHECKRESULT("ICaptureTestStreamingEngine::StopStreaming",
                      hr,
                      spCaptureEngine->StopStreaming());
    if(FAILED(hr))
    {
        if(FALSE == bSuppressStreamingErrors)
        {
            XLOG(XFAIL, eError, "FAIL: Error calling ICaptureTestStreamingEngine::StopStreaming");
        }
        else
        {
            // warn but don't fail
            SLOG(eError, "WARNING: Error calling ICaptureTestStreamingEngine::StopStreaming");
        }
    }

END:
    return FNS_PASS;
}

DWORD DispatchTest(SCONTROL_TEST_ID nTestID, 
                   DWORD dwDeviceType, 
                   ITestStreamingEngine* pStreamingEngine,
                   BOOL &bIgnoreStreamingErrors)
{
    // dispatch the correct test case
    switch(nTestID)
    {
        case SIMPLEAUDIOVOLUME_SETMASTERVOLUME_BASIC0:
            Test_SimpleAudioVolume_SetMasterVolume_Basic0(dwDeviceType, pStreamingEngine);
            break;
        case SIMPLEAUDIOVOLUME_SETMASTERVOLUME_VALID0:
            Test_SimpleAudioVolume_SetMasterVolume_Valid0(dwDeviceType, pStreamingEngine);
            break;
        case SIMPLEAUDIOVOLUME_GETSETMUTE_BASIC0:
            Test_SimpleAudioVolume_GetSetMute_Basic0(dwDeviceType, pStreamingEngine);
            break;
        case SIMPLEAUDIOVOLUME_SETMASTERVOLUME_INVALID0:
            Test_SimpleAudioVolume_SetMasterVolume_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case SIMPLEAUDIOVOLUME_SETMASTERVOLUME_INVALID1:
            Test_SimpleAudioVolume_SetMasterVolume_Invalid1(dwDeviceType, pStreamingEngine);
            break;
        case SIMPLEAUDIOVOLUME_GETMASTERVOLUME_INVALID0:
            Test_SimpleAudioVolume_GetMasterVolume_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case SIMPLEAUDIOVOLUME_GETMUTE_INVALID0:
            Test_SimpleAudioVolume_GetMute_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case SIMPLEAUDIOVOLUME_DISCONNECTSERVICE:
            Test_SimpleAudioVolume_DisconnectService(dwDeviceType, pStreamingEngine);
            bIgnoreStreamingErrors = TRUE;
            break;
        case CHANNELAUDIOVOLUME_GETCHANNELCOUNT_VALID0:
            Test_ChannelAudioVolume_GetChannelCount_Valid0(dwDeviceType, pStreamingEngine);
            break;
        case CHANNELAUDIOVOLUME_SETCHANNELVOLUME_VALID0:
            Test_ChannelAudioVolume_SetChannelVolume_Valid0(dwDeviceType, pStreamingEngine);
            break;
        case CHANNELAUDIOVOLUME_SETCHANNELVOLUME_VALID1:
            Test_ChannelAudioVolume_SetChannelVolume_Valid1(dwDeviceType, pStreamingEngine);
            break;
        case CHANNELAUDIOVOLUME_GETSETALLVOLUMES_VALID0:
            Test_ChannelAudioVolume_GetSetAllVolumes_Valid0(dwDeviceType, pStreamingEngine);
            break;
        case CHANNELAUDIOVOLUME_GETSETALLVOLUMES_VALID1:
            Test_ChannelAudioVolume_GetSetAllVolumes_Valid1(dwDeviceType, pStreamingEngine);
            break;
        case CHANNELAUDIOVOLUME_GETCHANNELCOUNT_INVALID0:
            Test_ChannelAudioVolume_GetChannelCount_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case CHANNELAUDIOVOLUME_SETCHANNELVOLUME_INVALID0:
            Test_ChannelAudioVolume_SetChannelVolume_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case CHANNELAUDIOVOLUME_SETCHANNELVOLUME_INVALID1:
            Test_ChannelAudioVolume_SetChannelVolume_Invalid1(dwDeviceType, pStreamingEngine);
            break;
        case CHANNELAUDIOVOLUME_SETCHANNELVOLUME_INVALID2:
            Test_ChannelAudioVolume_SetChannelVolume_Invalid2(dwDeviceType, pStreamingEngine);
            break;            
        case CHANNELAUDIOVOLUME_GETCHANNELVOLUME_INVALID0:
            Test_ChannelAudioVolume_GetChannelVolume_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case CHANNELAUDIOVOLUME_GETCHANNELVOLUME_INVALID1:
            Test_ChannelAudioVolume_GetChannelVolume_Invalid1(dwDeviceType, pStreamingEngine);
            break;
        case CHANNELAUDIOVOLUME_GETCHANNELVOLUME_INVALID2:
            Test_ChannelAudioVolume_GetChannelVolume_Invalid2(dwDeviceType, pStreamingEngine);
            break;
        case CHANNELAUDIOVOLUME_SETALLVOLUMES_INVALID0:
            Test_ChannelAudioVolume_SetAllVolumes_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case CHANNELAUDIOVOLUME_SETALLVOLUMES_INVALID1:
            Test_ChannelAudioVolume_SetAllVolumes_Invalid1(dwDeviceType, pStreamingEngine);
            break;
        case CHANNELAUDIOVOLUME_SETALLVOLUMES_INVALID2:
            Test_ChannelAudioVolume_SetAllVolumes_Invalid2(dwDeviceType, pStreamingEngine);
            break;
        case CHANNELAUDIOVOLUME_SETALLVOLUMES_INVALID3:
            Test_ChannelAudioVolume_SetAllVolumes_Invalid3(dwDeviceType, pStreamingEngine);
            break;
        case CHANNELAUDIOVOLUME_GETALLVOLUMES_INVALID0:
            Test_ChannelAudioVolume_GetAllVolumes_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case CHANNELAUDIOVOLUME_GETALLVOLUMES_INVALID1:
            Test_ChannelAudioVolume_GetAllVolumes_Invalid1(dwDeviceType, pStreamingEngine);
            break;
        case CHANNELAUDIOVOLUME_GETALLVOLUMES_INVALID2:
            Test_ChannelAudioVolume_GetAllVolumes_Invalid2(dwDeviceType, pStreamingEngine);
            break;
        case CHANNELAUDIOVOLUME_DISCONNECTSERVICE:
            Test_ChannelAudioVolume_DisconnectService(dwDeviceType, pStreamingEngine);
            bIgnoreStreamingErrors = TRUE;
            break;
        case STREAMAUDIOVOLUME_GETCHANNELCOUNT_VALID0:
            Test_StreamAudioVolume_GetChannelCount_Valid0(dwDeviceType, pStreamingEngine);
            break;
        case STREAMAUDIOVOLUME_GETSETCHANNELVOLUME_VALID0:
            Test_StreamAudioVolume_GetSetChannelVolume_Valid0(dwDeviceType, pStreamingEngine);
            break;
        case STREAMAUDIOVOLUME_GETSETALLVOLUMES_VALID0:
            Test_StreamAudioVolume_GetSetAllVolumes_Valid0(dwDeviceType, pStreamingEngine);
            break;
        case STREAMAUDIOVOLUME_GETSETALLVOLUMES_VALID1:
            Test_StreamAudioVolume_GetSetAllVolumes_Valid1(dwDeviceType, pStreamingEngine);
            break;
        case STREAMAUDIOVOLUME_GETCHANNELCOUNT_INVALID0:
            Test_StreamAudioVolume_GetChannelCount_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case STREAMAUDIOVOLUME_SETCHANNELVOLUME_INVALID0:
            Test_StreamAudioVolume_SetChannelVolume_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case STREAMAUDIOVOLUME_GETCHANNELVOLUME_INVALID0:
            Test_StreamAudioVolume_GetChannelVolume_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case STREAMAUDIOVOLUME_GETALLVOLUMES_INVALID0:
            Test_StreamAudioVolume_GetAllVolumes_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case STREAMAUDIOVOLUME_SETALLVOLUMES_INVALID0:
            Test_StreamAudioVolume_SetAllVolumes_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case STREAMAUDIOVOLUME_DISCONNECTSERVICE:
            Test_StreamAudioVolume_DisconnectService(dwDeviceType, pStreamingEngine);
            bIgnoreStreamingErrors = TRUE;
            break;
        case AUDIOSESSIONCONTROL_GETSESSIONIDENTIFIER_VALID0:
            Test_AudioSessionControl_GetSessionID_Valid0(dwDeviceType, pStreamingEngine);
            break;
        case AUDIOSESSIONCONTROL_GETSTATE_VALID0:
            Test_AudioSessionControl_GetState_Valid0(dwDeviceType, pStreamingEngine);
            break;
        case AUDIOSESSIONCONTROL_GETSTATE_VALID1:            
            Test_AudioSessionControl_GetState_Valid1(dwDeviceType, pStreamingEngine);
            break;
        case AUDIOSESSIONCONTROL_GETLASTACTIVATION_VALID0:
            Test_AudioSessionControl_GetLastActivation_Valid0(dwDeviceType, pStreamingEngine);
            break;
        case AUDIOSESSIONCONTROL_GETLASTACTIVATION_VALID1:
            Test_AudioSessionControl_GetLastActivation_Valid1(dwDeviceType, pStreamingEngine);
            break;
        case AUDIOSESSIONCONTROL_GETLASTINACTIVATION_VALID0:
            Test_AudioSessionControl_GetLastInactivation_Valid0(dwDeviceType, pStreamingEngine);
            break;
        case AUDIOSESSIONCONTROL_GETLASTINACTIVATION_VALID1:
            Test_AudioSessionControl_GetLastInactivation_Valid1(dwDeviceType, pStreamingEngine);
            break;
        case AUDIOSESSIONCONTROL_GETDISPLAYNAME_VALID0:
            Test_AudioSessionControl_GetDisplayName_Valid0(dwDeviceType, pStreamingEngine);
            break;
        case AUDIOSESSIONCONTROL_SETDISPLAYNAME_VALID0:
            Test_AudioSessionControl_SetDisplayName_Valid0(dwDeviceType, pStreamingEngine);
            break;
        case AUDIOSESSIONCONTROL_SETDISPLAYNAME_VALID1:
            Test_AudioSessionControl_SetDisplayName_Valid1(dwDeviceType, pStreamingEngine);
            break;
        case AUDIOSESSIONCONTROL_GETICONPATH_VALID0:
            Test_AudioSessionControl_GetIconPath_Valid0(dwDeviceType, pStreamingEngine);
            break;
        case AUDIOSESSIONCONTROL_SETICONPATH_VALID0:
            Test_AudioSessionControl_SetIconPath_Valid0(dwDeviceType, pStreamingEngine);
            break;
        case AUDIOSESSIONCONTROL_GETGROUPINGPARAM_VALID0:
            Test_AudioSessionControl_GetGroupingParam_Valid0(dwDeviceType, pStreamingEngine);
            break;
        case AUDIOSESSIONCONTROL_SETGROUPINGPARAM_VALID0:
            Test_AudioSessionControl_SetGroupingParam_Valid0(dwDeviceType, pStreamingEngine);
            break;
        case AUDIOSESSIONCONTROL_NOTIFICATIONS_VALID0:
            Test_AudioSessionControl_Notifications_Valid0(dwDeviceType, pStreamingEngine);
            break;
        case AUDIOSESSIONCONTROL_NOTIFICATIONS_VALID1:
            Test_AudioSessionControl_Notifications_Valid1(dwDeviceType, pStreamingEngine);
            break;
        case AUDIOSESSIONCONTROL_GETPROCESSID_VALID0:
            Test_AudioSessionControl_GetProcessId_Valid0(dwDeviceType, pStreamingEngine);
            break;
        case AUDIOSESSIONCONTROL_GETSESSIONID_INVALID0:
            Test_AudioSessionControl_GetSessionID_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case AUDIOSESSIONCONTROL_GETSTATE_INVALID0:
            Test_AudioSessionControl_GetState_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case AUDIOSESSIONCONTROL_GETLASTACTIVATION_INVALID0:
            Test_AudioSessionControl_GetLastActivation_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case AUDIOSESSIONCONTROL_GETLASTINACTIVATION_INVALID0:
            Test_AudioSessionControl_GetLastInactivation_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case AUDIOSESSIONCONTROL_GETDISPLAYNAME_INVALID0:
            Test_AudioSessionControl_GetDisplayName_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case AUDIOSESSIONCONTROL_SETDISPLAYNAME_INVALID0:
            Test_AudioSessionControl_SetDisplayName_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case AUDIOSESSIONCONTROL_GETGROUPINGPARAM_INVALID0:
            Test_AudioSessionControl_GetGroupingParam_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case AUDIOSESSIONCONTROL_ADDAUDIOSESNOTIF_INVALID0:
            Test_AudioSessionControl_AddAudioSesNotif_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case AUDIOSESSIONCONTROL_DELAUDIOSESNOTIF_INVALID0:
            Test_AudioSessionControl_DelAudioSesNotif_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case AUDIOSESSIONCONTROL_GETPROCESSID_INVALID0:
            Test_AudioSessionControl_GetProcessId_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case AUDIOSESSIONCONTROL_DISCONNECTSERVICE_INVALID0:
            Test_AudioSessionControl_DisconnectService_Invalid0(dwDeviceType, pStreamingEngine);
            bIgnoreStreamingErrors = TRUE;
            break;
        case PERSTREAMEVENTS_SCENARIO1:
            Test_PerStreamEvents_Scenario1(dwDeviceType, pStreamingEngine);
            break;
        case PERSTREAMEVENTS_SCENARIO2:
            Test_PerStreamEvents_Scenario2(dwDeviceType, pStreamingEngine);
            break;
        case PERSTREAMEVENTS_SCENARIO3:
            Test_PerStreamEvents_Scenario3(dwDeviceType, pStreamingEngine);
            break;
        case PERSTREAMEVENTS_SCENARIO4:
            Test_PerStreamEvents_Scenario4(dwDeviceType, pStreamingEngine);
            break;
        case PERSTREAMEVENTS_SCENARIO5:
            Test_PerStreamEvents_Scenario5(dwDeviceType, pStreamingEngine);
            break;
        case PERSTREAMEVENTS_SCENARIO6:
            Test_PerStreamEvents_Scenario6(dwDeviceType, pStreamingEngine);
            break;
        case PERSTREAMEVENTS_SCENARIO7:
            Test_PerStreamEvents_Scenario7(dwDeviceType, pStreamingEngine);
            break;
        case PERSTREAMEVENTS_SCENARIO8:
            Test_PerStreamEvents_Scenario8(dwDeviceType, pStreamingEngine);
            break;
         case PERSTREAMEVENTS_SCENARIO9:
            Test_PerStreamEvents_Scenario9(dwDeviceType, pStreamingEngine);
            break;           
        case ENDPOINTVOLUME_SETMASTERVOL_VALID0:
            Test_EndpointVolume_SetMasterVol_Valid0(dwDeviceType, pStreamingEngine);
            break;
        case ENDPOINTVOLUME_SETMASTERVOL_VALID1:
            Test_EndpointVolume_SetMasterVol_Valid1(dwDeviceType, pStreamingEngine);
            break;
        case ENDPOINTVOLUME_SETMASTERVOL_VALID2:
            Test_EndpointVolume_SetMasterVol_Valid2(dwDeviceType, pStreamingEngine);
            break;
        case ENDPOINTVOLUME_SETMASTERVOL_VALID3:
            Test_EndpointVolume_SetMasterVol_Valid3(dwDeviceType, pStreamingEngine);
            break;
        case ENDPOINTVOLUME_SETMASTERVOL_SCALAR_VALID0:
            Test_EndpointVolume_SetMasterVol_Scalar_Valid0(dwDeviceType, pStreamingEngine);
            break;
        case ENDPOINTVOLUME_SETMASTERVOL_SCALAR_VALID1:
            Test_EndpointVolume_SetMasterVol_Scalar_Valid1(dwDeviceType, pStreamingEngine);
            break;
        case ENDPOINTVOLUME_SETMASTERVOL_SCALAR_VALID2:
            Test_EndpointVolume_SetMasterVol_Scalar_Valid2(dwDeviceType, pStreamingEngine);
            break;
        case ENDPOINTVOLUME_SETMASTERVOL_SCALAR_VALID3:
            Test_EndpointVolume_SetMasterVol_Scalar_Valid3(dwDeviceType, pStreamingEngine);
            break;
        case ENDPOINTVOLUME_SETMUTE_VALID0:
            Test_EndpointVolume_SetMute_Valid0(dwDeviceType, pStreamingEngine);
            break;
        case ENDPOINTVOLUME_SETMUTE_VALID1:
            Test_EndpointVolume_SetMute_Valid1(dwDeviceType, pStreamingEngine);
            break;
        case ENDPOINTVOLUME_VOLUMESTEPPING_VALID1:
            Test_EndpointVolume_VolumeStepping_Valid1(dwDeviceType, pStreamingEngine);
            break;
        case ENDPOINTVOLUME_VOLUMESTEPPING_VALID2:
            Test_EndpointVolume_VolumeStepping_Valid2(dwDeviceType, pStreamingEngine);
            break;
        case ENDPOINTVOLUME_GETCHANNELCOUNT_VALID1:
            Test_EndpointVolume_GetChannelCount_Valid1(dwDeviceType, pStreamingEngine);
            break;
        case ENDPOINTVOLUME_SETCHANNELVOLUME_VALID1:
            Test_EndpointVolume_SetChannelVolumeLevel_Valid1(dwDeviceType, pStreamingEngine);
            break;
        case ENDPOINTVOLUME_SETCHANNELVOLUME_VALID2:
            Test_EndpointVolume_SetChannelVolumeLevel_Valid2(dwDeviceType, pStreamingEngine);
            break;
        case ENDPOINTVOLUME_SETCHANNELVOLUME_VALID3:
            Test_EndpointVolume_SetChannelVolumeLevel_Valid3(dwDeviceType, pStreamingEngine);
            break;
        case ENDPOINTVOLUME_SETCHANNELVOLUME_VALID4:
            Test_EndpointVolume_SetChannelVolumeLevel_Valid4(dwDeviceType, pStreamingEngine);
            break;
        case ENDPOINTVOLUME_SETCHANNELVOLUME_VALID5:
            Test_EndpointVolume_SetChannelVolumeLevel_Valid5(dwDeviceType, pStreamingEngine);
            break;            
        case ENDPOINTVOLUME_SETCHANNELVOLUMESCALAR_VALID1:
            Test_EndpointVolume_SetChannelVolumeLevelScalar_Valid1(dwDeviceType, pStreamingEngine);
            break;            
        case ENDPOINTVOLUME_SETCHANNELVOLUMESCALAR_VALID2:
            Test_EndpointVolume_SetChannelVolumeLevelScalar_Valid2(dwDeviceType, pStreamingEngine);
            break;            
        case ENDPOINTVOLUME_SETCHANNELVOLUMESCALAR_VALID3:
            Test_EndpointVolume_SetChannelVolumeLevelScalar_Valid3(dwDeviceType, pStreamingEngine);
            break;            
        case ENDPOINTVOLUME_SETCHANNELVOLUMESCALAR_VALID4:
            Test_EndpointVolume_SetChannelVolumeLevelScalar_Valid4(dwDeviceType, pStreamingEngine);
            break;            
        case ENDPOINTVOLUME_SETCHANNELVOLUMESCALAR_VALID5:
            Test_EndpointVolume_SetChannelVolumeLevelScalar_Valid5(dwDeviceType, pStreamingEngine);
            break;            
        case ENDPOINTVOLUME_QUERYHWSUPPORT_VALID1:
            Test_EndpointVolume_QueryHardwareSupport_Valid1(dwDeviceType, pStreamingEngine);
            break;               
        case ENDPOINTVOLUME_GETVOLUMERANGE_VALID1:
            Test_EndpointVolume_GetVolumeRange_Valid1(dwDeviceType, pStreamingEngine);
            break;
        case ENDPOINTVOLUME_GETMASTERVOL_INVALID0:
            Test_EndpointVolume_GetMasterVol_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case ENDPOINTVOLUME_GETMASTERVOL_SCALAR_INVALID0:
            Test_EndpointVolume_GetMasterVol_Scalar_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case ENDPOINTVOLUME_GETMUTE_INVALID0:            
            Test_EndpointVolume_GetMute_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case ENDPOINTVOLUME_VOLUMESTEPPING_INVALID0:
            Test_EndpointVolume_VolumeStepping_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case ENDPOINTVOLUME_GETCHANNELCOUNT_INVALID0:
            Test_EndpointVolume_GetChannelCount_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case ENDPOINTVOLUME_SETCHANNELVOLUMELEVEL_INVALID0:
            Test_EndpointVolume_SetChannelVolumeLevel_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case ENDPOINTVOLUME_GETCHANNELVOLUMELEVEL_INVALID0:
            Test_EndpointVolume_GetChannelVolumeLevel_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case ENDPOINTVOLUME_SETCHANNELVOLUMELEVELSCALAR_INVALID0:
            Test_EndpointVolume_SetChannelVolumeLevelScalar_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case ENDPOINTVOLUME_GETCHANNELVOLUMELEVELSCALAR_INVALID0:
            Test_EndpointVolume_GetChannelVolumeLevelScalar_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case ENDPOINTVOLUME_QUERYHWSUPPORT_INVALID0:
            Test_EndpointVolume_QueryHardwareSupport_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case ENDPOINTVOLUME_GETVOLUMERANGE_INVALID1:
            Test_EndpointVolume_GetVolumeRange_Invalid1(dwDeviceType, pStreamingEngine);
            break;
        case ENDPOINTVOLUME_DISCONNECTSERVICE_INVALID0:
            Test_EndpointVolume_DisconnectService_Invalid0(dwDeviceType, pStreamingEngine);
            bIgnoreStreamingErrors = TRUE;
            break;
        case PERENDPOINTEVENTS_SCENARIO1:
            Test_PerEndpointEvents_Scenario1(dwDeviceType, pStreamingEngine);
            break;
        case PERENDPOINTEVENTS_SCENARIO2:
            Test_PerEndpointEvents_Scenario2(dwDeviceType, pStreamingEngine);
            break;
        case PERENDPOINTEVENTS_SCENARIO3:
            Test_PerEndpointEvents_Scenario3(dwDeviceType, pStreamingEngine);
            break;
        case PERENDPOINTEVENTS_SCENARIOIV1:
            Test_PerEndpointEvents_ScenarioIV1(dwDeviceType, pStreamingEngine);
            break;

        case AUDMETERINF_VALID0:
            Test_AudioMeterInf_Valid0(dwDeviceType, pStreamingEngine);
            break;
        case AUDMETERINF_VALID1:
            Test_AudioMeterInf_Valid1(dwDeviceType, pStreamingEngine);
            break;
        case AUDMETERINF_INVALID0:
            Test_AudioMeterInf_Invalid0(dwDeviceType, pStreamingEngine);
            break;
        case AUDMETERINF_DISCONNECTSERVICE:
            Test_AudioMeterInf_DisconnectService(dwDeviceType, pStreamingEngine);
            bIgnoreStreamingErrors = TRUE;
            break;
        case SESSIONMGMT_GETSESSIONENUM_VALID1:
            Test_SessionManagement_GetSessionEnum_Valid1(dwDeviceType, pStreamingEngine);
            break;
        case SESSIONMGMT_GETSESSIONENUM_VALID2:
            Test_SessionManagement_GetSessionEnum_Valid2(dwDeviceType, pStreamingEngine);
            break;
        case SESSIONMGMT_GETSESSIONENUM_VALID3:
            Test_SessionManagement_GetSessionEnum_Valid3(dwDeviceType, pStreamingEngine);
            break;
        case SESSIONMGMT_ADDSESSIONNOTIF_VALID1:
            Test_SessionManagement_AddSessionNotif_Valid1(dwDeviceType, pStreamingEngine);
            break;
        case SESSIONMGMT_ADDSESSIONNOTIF_VALID2:
            Test_SessionManagement_AddSessionNotif_Valid2(dwDeviceType, pStreamingEngine);
            break;
        case SESSIONMGMT_ADDSESSIONNOTIF_VALID3:
            Test_SessionManagement_AddSessionNotif_Valid3(dwDeviceType, pStreamingEngine);
            break;
        case SESSIONMGMT_ADDSESSIONNOTIF_VALID4:
            Test_SessionManagement_AddSessionNotif_Valid4(dwDeviceType, pStreamingEngine);
            break;
        case SESSIONMGMT_DELSESSIONNOTIF_VALID1:
            Test_SessionManagement_DeleteSessionNotif_Valid1(dwDeviceType, pStreamingEngine);
            break;
        case SESSIONMGMT_GETSESSION_VALID1:
            Test_SessionManagement_GetSession_Valid1(dwDeviceType, pStreamingEngine);
            break;
        case SESSIONMGMT_GETSESSION_VALID2:
            Test_SessionManagement_GetSession_Valid2(dwDeviceType, pStreamingEngine);
            break;
        case SESSIONMGMT_GETSESSION_VALID3:
            Test_SessionManagement_GetSession_Valid3(dwDeviceType, pStreamingEngine);
            break;
        case SESSIONMGMT_GETSESSION_VALID4:
            Test_SessionManagement_GetSession_Valid4(dwDeviceType, pStreamingEngine);
            break;
        case SESSIONMGMT_ADDSESSIONNOTIF_INVALID1:
            Test_SessionManagement_AddSessionNotif_Invalid1(dwDeviceType, pStreamingEngine);
            break;
        case SESSIONMGMT_DELSESSIONNOTIF_INVALID1:
            Test_SessionManagement_DeleteSessionNotif_Invalid1(dwDeviceType, pStreamingEngine);
            break;
        case SESSIONMGMT_DISCONNECTSERVICE:
            Test_SessionManagement_DisconnectService(dwDeviceType, pStreamingEngine);
            bIgnoreStreamingErrors = TRUE;
            break;
        default:
            XLOG(XFAIL, eError, "FAIL: Unknown internal test ID: %d", nTestID);
    }

    return FNS_PASS;
}
