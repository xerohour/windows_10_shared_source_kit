// Copyright(C) Microsoft. All rights reserved.

//
// TestFunctions.h
//
// Description:
//
//  Test function prototypes for UMATest
//

#pragma once

//
// IAudioClient Invalid Scenarios
//
DWORD TC_AudioClient_GetService_Invalid();
DWORD TC_AudioClient_IsFormatSupported_Invalid();
DWORD TC_AudioClient_Initialize_Invalid();
DWORD TC_AudioClient_GetBufferSize_Invalid();
DWORD TC_AudioClient_GetStreamLatency_Invalid();
DWORD TC_AudioClient_GetCurrentPadding_Invalid();
DWORD TC_AudioClient_DisconnectService_Invalid();
DWORD TC_AudioClient_QueryInterface_Invalid();
DWORD TC_AudioClient_GetMixFormat_Invalid();
DWORD TC_AudioClient_SetEventHandle_Invalid();

//
// IAudioClient Valid Scenarios
//
DWORD TC_AudioClient_GetService_Valid();
DWORD TC_AudioClient_GetBufferSize_Valid();
DWORD TC_AudioClient_GetStreamLatency_Valid();
DWORD TC_AudioClient_SetEventHandle_Valid();
DWORD TC_AudioClient_ActivateViaAudioInterfaceAsync();

//
// IAudioClient2 Invalid Scenarios
//
DWORD TC_AudioClient2_IsOffloadCapable_Invalid();
DWORD TC_AudioClient2_SetClientProperties_Invalid();
DWORD TC_AudioClient2_GetBufferSizeLimits_Invalid();
DWORD TC_AudioClient2_TimerDrivenOffload_Invalid();

//
// IAudioClient2 Valid Scenarios
//
DWORD TC_AudioClient2_IsOffloadCapable_Valid();
DWORD TC_AudioClient2_SetClientProperties_Valid();
DWORD TC_AudioClient2_GetBufferSizeLimits_Valid();

//
// IAudioRenderClient Invalid Scenarios
//
DWORD TC_AudioRenderClient_GetBuffer_Invalid();
DWORD TC_AudioRenderClient_GetBuffer_Hacker_Invalid();
DWORD TC_AudioRenderClient_ReleaseBuffer_Invalid();

//
// IAudioCaptureClient Invalid Scenarios
//
DWORD TC_AudioCaptureClient_GetBuffer_Invalid();
DWORD TC_AudioCaptureClient_GetBuffer_Hacker_Invalid();
DWORD TC_AudioCaptureClient_ReleaseBuffer_Invalid();

//
// IAudioClock Invalid Scenarios
//
DWORD TC_AudioClock_GetPosition_Invalid();
DWORD TC_AudioClock_GetFrequency_Invalid();
DWORD TC_AudioClock_GetCharacteristics_Invalid();

//
// IPolicyConfig Invalid Scenarios
//
DWORD TC_PolicyConfig_GetDeviceFormat_Invalid();
DWORD TC_PolicyConfig_GetMixFormat_Invalid();
DWORD TC_PolicyConfig_SetDeviceFormat_Invalid();
DWORD TC_PolicyConfig_GetProcessingPeriod_Invalid();
DWORD TC_PolicyConfig_SetProcessingPeriod_Invalid();
DWORD TC_PolicyConfig_GetShareMode_Invalid();
DWORD TC_PolicyConfig_SetShareMode_Invalid();
DWORD TC_PolicyConfig_SetDefaultEndpoint_Invalid();
DWORD TC_PolicyConfig_SetEndpointVisibility_Invalid();

//
// ISimpleAudioVolume Valid Scenarios
//
DWORD TC_SimpleAudioVolume_SetMasterVolume_Basic0();
DWORD TC_SimpleAudioVolume_SetMasterVolume_Valid0();
DWORD TC_SimpleAudioVolume_GetSetMute_Basic0();

//
// ISimpleAudioVolume Invalid Scenarios
//
DWORD TC_SimpleAudioVolume_SetMasterVolume_Invalid0();
DWORD TC_SimpleAudioVolume_SetMasterVolume_Invalid1();
DWORD TC_SimpleAudioVolume_GetMasterVolume_Invalid0();
DWORD TC_SimpleAudioVolume_GetMute_Invalid0();
DWORD TC_SimpleAudioVolume_DisconnectService();

// IChannelAudioVolume Valid Scenarios
DWORD TC_ChannelAudioVolume_GetChannelCount_Valid0();
DWORD TC_ChannelAudioVolume_SetChannelVolume_Valid0();
DWORD TC_ChannelAudioVolume_SetChannelVolume_Valid1();
DWORD TC_ChannelAudioVolume_GetSetAllVolumes_Valid0();
DWORD TC_ChannelAudioVolume_GetSetAllVolumes_Valid1();

// IChannelAudioVolume Invalid Scenarios
DWORD TC_ChannelAudioVolume_GetChannelCount_Invalid0();
DWORD TC_ChannelAudioVolume_SetChannelVolume_Invalid0();
DWORD TC_ChannelAudioVolume_SetChannelVolume_Invalid1();
DWORD TC_ChannelAudioVolume_SetChannelVolume_Invalid2();
DWORD TC_ChannelAudioVolume_GetChannelVolume_Invalid0();
DWORD TC_ChannelAudioVolume_GetChannelVolume_Invalid1();
DWORD TC_ChannelAudioVolume_GetChannelVolume_Invalid2();
DWORD TC_ChannelAudioVolume_SetAllVolumes_Invalid0();
DWORD TC_ChannelAudioVolume_SetAllVolumes_Invalid1();
DWORD TC_ChannelAudioVolume_SetAllVolumes_Invalid2();
DWORD TC_ChannelAudioVolume_SetAllVolumes_Invalid3();
DWORD TC_ChannelAudioVolume_GetAllVolumes_Invalid0();
DWORD TC_ChannelAudioVolume_GetAllVolumes_Invalid1();
DWORD TC_ChannelAudioVolume_GetAllVolumes_Invalid2();
DWORD TC_ChannelAudioVolume_DisconnectService();

// IAudioStreamVolume Valid Scenarios
DWORD TC_StreamAudioVolume_GetChannelCount_Valid0();
DWORD TC_StreamAudioVolume_GetSetChannelVolume_Valid0();
DWORD TC_StreamAudioVolume_GetSetAllVolumes_Valid0();
DWORD TC_StreamAudioVolume_GetSetAllVolumes_Valid1();

// IAudioStreamVolume Invalid Scenarios
DWORD TC_StreamAudioVolume_GetChannelCount_Invalid0();
DWORD TC_StreamAudioVolume_SetChannelVolume_Invalid0();
DWORD TC_StreamAudioVolume_GetChannelVolume_Invalid0();
DWORD TC_StreamAudioVolume_GetAllVolumes_Invalid0();
DWORD TC_StreamAudioVolume_SetAllVolumes_Invalid0();
DWORD TC_StreamAudioVolume_DisconnectService();

// IAudioSessionControl Valid Scenarios
DWORD TC_AudioSessionControl_GetSessionID_Valid0();
DWORD TC_AudioSessionControl_GetState_Valid0();
DWORD TC_AudioSessionControl_GetState_Valid1();
DWORD TC_AudioSessionControl_GetLastActication_Valid0();
DWORD TC_AudioSessionControl_GetLastActication_Valid1();
DWORD TC_AudioSessionControl_GetLastInactivation_Valid0();
DWORD TC_AudioSessionControl_GetLastInactivation_Valid1();
DWORD TC_AudioSessionControl_GetDisplayName_Valid0();
DWORD TC_AudioSessionControl_SetDisplayName_Valid0();
DWORD TC_AudioSessionControl_SetDisplayName_Valid1();
DWORD TC_AudioSessionControl_GetIconPath_Valid0();
DWORD TC_AudioSessionControl_SetIconPath_Valid0();
DWORD TC_AudioSessionControl_GetGroupingParam_Valid0();
DWORD TC_AudioSessionControl_SetGroupingParam_Valid0();
DWORD TC_AudioSessionControl_Notifications_Valid0();
DWORD TC_AudioSessionControl_Notifications_Valid1();
DWORD TC_AudioSessionControl_Notifications_Valid2();
DWORD TC_AudioSessionControl_GetProcessId();

// IAudioSessionControl Invalid Scenarios
DWORD TC_AudioSessionControl_GetSessionID_Invalid0();
DWORD TC_AudioSessionControl_GetState_Invalid0();
DWORD TC_AudioSessionControl_GetLastActivation_Invalid0();
DWORD TC_AudioSessionControl_GetLastInactivation_Invalid0();
DWORD TC_AudioSessionControl_GetDisplayName_Invalid0();
DWORD TC_AudioSessionControl_SetDisplayName_Invalid0();
DWORD TC_AudioSessionControl_GetGroupingParam_Invalid0();
DWORD TC_AudioSessionControl_AddAudioSesNotif_Invalid0();
DWORD TC_AudioSessionControl_DelAudioSesNotif_Invalid0();
DWORD TC_AudioSessionControl_GetProcessId_Invalid0();
DWORD TC_AudioSessionControl_DisconnectService_Invalid0();

// Per-Session/Stream Eventing Scenarios
DWORD TC_PerStreamEvents_Scenario1();
DWORD TC_PerStreamEvents_Scenario2();
DWORD TC_PerStreamEvents_Scenario3();
DWORD TC_PerStreamEvents_Scenario4();
DWORD TC_PerStreamEvents_Scenario5();
DWORD TC_PerStreamEvents_Scenario6();
DWORD TC_PerStreamEvents_Scenario7();
DWORD TC_PerStreamEvents_Scenario8();
DWORD TC_PerStreamEvents_Scenario9();

// Per-Endpoint Eventing Scenarios
DWORD TC_PerEndpointEvents_Scenario1();
DWORD TC_PerEndpointEvents_Scenario2();
DWORD TC_PerEndpointEvents_Scenario3();

DWORD TC_PerEndpointEvents_ScenarioIV1();

// Endpoint Streaming Property Tests
DWORD TC_EndpointProperty_HardwareAcceleration();
DWORD TC_EndpointProperty_DisableSystemEffects();

// KS Instances tests
DWORD TC_KSInstance_MF();
DWORD TC_KSInstance_DSound();
DWORD TC_KSInstance_WinMM();
DWORD TC_KSInstance_WasapiShared();
DWORD TC_KSInstance_WasapiExclusive();

// UMATestStreaming tests
DWORD TC_UmaTestStreaming_RenderLoop();
DWORD TC_UmaTestStreaming_CaptureLoop();
    
// IAudioEndpointVolume Valid Scenarios
DWORD TC_EndpointVolume_SetMasterVol_Valid0();
DWORD TC_EndpointVolume_SetMasterVol_Valid1();
DWORD TC_EndpointVolume_SetMasterVol_Valid2();
DWORD TC_EndpointVolume_SetMasterVol_Valid3();
DWORD TC_EndpointVolume_SetMasterVol_Scalar_Valid0();
DWORD TC_EndpointVolume_SetMasterVol_Scalar_Valid1();
DWORD TC_EndpointVolume_SetMasterVol_Scalar_Valid2();
DWORD TC_EndpointVolume_SetMasterVol_Scalar_Valid3();
DWORD TC_EndpointVolume_SetMute_Valid0();
DWORD TC_EndpointVolume_SetMute_Valid1();
DWORD TC_EndpointVolume_VolumeStepping_Valid1();
DWORD TC_EndpointVolume_VolumeStepping_Valid2();
DWORD TC_EndpointVolume_GetChannelCount_Valid1();
DWORD TC_EndpointVolume_SetChannelVolumeLevel_Valid1();
DWORD TC_EndpointVolume_SetChannelVolumeLevel_Valid2();
DWORD TC_EndpointVolume_SetChannelVolumeLevel_Valid3();
DWORD TC_EndpointVolume_SetChannelVolumeLevel_Valid4();
DWORD TC_EndpointVolume_SetChannelVolumeLevel_Valid5();
DWORD TC_EndpointVolume_SetChannelVolumeLevelScalar_Valid1();
DWORD TC_EndpointVolume_SetChannelVolumeLevelScalar_Valid2();
DWORD TC_EndpointVolume_SetChannelVolumeLevelScalar_Valid3();
DWORD TC_EndpointVolume_SetChannelVolumeLevelScalar_Valid4();
DWORD TC_EndpointVolume_SetChannelVolumeLevelScalar_Valid5();
DWORD TC_EndpointVolume_QueryHardwareSupport_Valid1();
DWORD TC_EndpointVolume_GetVolumeRange_Valid1();

// IAudioEndpointVolume Invalid Scenarios
DWORD TC_EndpointVolume_GetMasterVol_Invalid0();
DWORD TC_EndpointVolume_GetMasterVol_Scalar_Invalid0();
DWORD TC_EndpointVolume_GetMute_Invalid0();
DWORD TC_EndpointVolume_VolumeStepping_Invalid0();
DWORD TC_EndpointVolume_GetChannelCount_Invalid0();
DWORD TC_EndpointVolume_SetChannelVolumeLevel_Invalid0();
DWORD TC_EndpointVolume_GetChannelVolumeLevel_Invalid0();
DWORD TC_EndpointVolume_SetChannelVolumeLevelScalar_Invalid0();
DWORD TC_EndpointVolume_GetChannelVolumeLevelScalar_Invalid0();
DWORD TC_EndpointVolume_QueryHardwareSupport_Invalid0();
DWORD TC_EndpointVolume_DisconnectService_Invalid0();
DWORD TC_EndpointVolume_GetVolumeRange_Invalid1();

// IAudioMeterInformation Valid Scenarios
DWORD TC_AudioMeterInf_Valid0();
DWORD TC_AudioMeterInf_Valid1();

// IAudioMeterInformation Invalid Scenarios
DWORD TC_AudioMeterInf_Invalid0();
DWORD TC_AudioMeterInf_DisconnectService();

// AudioSession Management Valid Scenarios
DWORD TC_SessionManagement_GetSessionEnum_Valid1();
DWORD TC_SessionManagement_GetSessionEnum_Valid2();
DWORD TC_SessionManagement_GetSessionEnum_Valid3();
DWORD TC_SessionManagement_AddSessionNotif_Valid1();
DWORD TC_SessionManagement_AddSessionNotif_Valid2();
DWORD TC_SessionManagement_AddSessionNotif_Valid3();
DWORD TC_SessionManagement_AddSessionNotif_Valid4();
DWORD TC_SessionManagement_DeleteSessionNotif_Valid1();
DWORD TC_SessionManagement_GetSession_Valid1();
DWORD TC_SessionManagement_GetSession_Valid2();
DWORD TC_SessionManagement_GetSession_Valid3();
DWORD TC_SessionManagement_GetSession_Valid4();

// AudioSession Management Invalid Scenarios
DWORD TC_SessionManagement_AddSessionNotif_Invalid1();
DWORD TC_SessionManagement_DeleteSessionNotif_Invalid1();
DWORD TC_SessionManagement_DisconnectService();

// Ducking Tests
DWORD TC_DuckingNotification_BasicSubscription();
DWORD TC_DuckingNotification_VerifyExplicitSubscrNotif();
DWORD TC_DuckingNotification_VerifyImplicitSubscrNotif();
DWORD TC_DuckingNotification_SubscrUnsubscr();
DWORD TC_DuckingNotification_NoIDIntersect();
DWORD TC_DuckingNotification_MultipleCommSessions();
DWORD TC_Ducking_SetDuckingPreferenceBasic();
DWORD TC_Ducking_DuckingStateE2E();

// IAudioClockAdjustment Valid Scenarios
DWORD TC_ClockRateAdjust_Valid1();
DWORD TC_ClockRateAdjust_Valid2();
DWORD TC_ClockRateAdjust_Valid3();
DWORD TC_ClockRateAdjust_Valid4();
DWORD TC_ClockRateAdjust_Valid5();
DWORD TC_ClockRateAdjust_Valid6();
DWORD TC_ClockRateAdjust_Valid7();
DWORD TC_ClockRateAdjust_Valid8();

// IAudioClockAdjustment Invalid Scenarios
DWORD TC_ClockRateAdjust_Invalid1();


// ActivateAudioInterfaceAsync Tests
DWORD TC_ActivateAsync_AllApplicable();

// Functional Streaming Scenarios

// - Render
//   -Shared
//    -Push
DWORD TC_Render_Shared_Push();
DWORD TC_Render_Shared_Push_SWLoopback();
//    -Pull
DWORD TC_Render_Shared_Pull();
DWORD TC_Render_Shared_Pull_SWLoopback();
//   -Compat
//    -Push
DWORD TC_Render_Compat_Push_0();
DWORD TC_Render_Compat_Push_1();
DWORD TC_Render_Compat_Push_2();
DWORD TC_Render_Compat_Push_3();
DWORD TC_Render_Compat_Push_4();
DWORD TC_Render_Compat_Push_5();
DWORD TC_Render_Compat_Push_6();
DWORD TC_Render_Compat_Push_7();
DWORD TC_Render_Compat_Push_8();
DWORD TC_Render_Compat_Push_9();
DWORD TC_Render_Compat_Push_10();
DWORD TC_Render_Compat_Push_11();
DWORD TC_Render_Compat_Push_12();
DWORD TC_Render_Compat_Push_13();
DWORD TC_Render_Compat_Push_14();
DWORD TC_Render_Compat_Push_15();
DWORD TC_Render_Compat_Push_16();
DWORD TC_Render_Compat_Push_17();
DWORD TC_Render_Compat_Push_18();
DWORD TC_Render_Compat_Push_19();
DWORD TC_Render_Compat_Push_20();
DWORD TC_Render_Compat_Push_21();
DWORD TC_Render_Compat_Push_22();
DWORD TC_Render_Compat_Push_23();
DWORD TC_Render_Compat_Push_24();
DWORD TC_Render_Compat_Push_25();
DWORD TC_Render_Compat_Push_26();
DWORD TC_Render_Compat_Push_27();
DWORD TC_Render_Compat_Push_28();
DWORD TC_Render_Compat_Push_29();
DWORD TC_Render_Compat_Push_30();
DWORD TC_Render_Compat_Push_31();
DWORD TC_Render_Compat_Push_32();
DWORD TC_Render_Compat_Push_33();
DWORD TC_Render_Compat_Push_34();
DWORD TC_Render_Compat_Push_35();
DWORD TC_Render_Compat_Push_36();
DWORD TC_Render_Compat_Push_37();
DWORD TC_Render_Compat_Push_38();
DWORD TC_Render_Compat_Push_39();
DWORD TC_Render_Compat_Push_40();
DWORD TC_Render_Compat_Push_41();
DWORD TC_Render_Compat_Push_42();
DWORD TC_Render_Compat_Push_43();
DWORD TC_Render_Compat_Push_44();
DWORD TC_Render_Compat_Push_45();
DWORD TC_Render_Compat_Push_46();
DWORD TC_Render_Compat_Push_47();
DWORD TC_Render_Compat_Push_48();
DWORD TC_Render_Compat_Push_49();
DWORD TC_Render_Compat_Push_50();
DWORD TC_Render_Compat_Push_51();
DWORD TC_Render_Compat_Push_52();
DWORD TC_Render_Compat_Push_53();
DWORD TC_Render_Compat_Push_54();
DWORD TC_Render_Compat_Push_55();
DWORD TC_Render_Compat_Push_56();
DWORD TC_Render_Compat_Push_57();
DWORD TC_Render_Compat_Push_58();
DWORD TC_Render_Compat_Push_59();
DWORD TC_Render_Compat_Push_60();
DWORD TC_Render_Compat_Push_61();
DWORD TC_Render_Compat_Push_62();

//    -Pull
DWORD TC_Render_Compat_Pull_0();
DWORD TC_Render_Compat_Pull_1();
DWORD TC_Render_Compat_Pull_2();
DWORD TC_Render_Compat_Pull_3();
DWORD TC_Render_Compat_Pull_4();
DWORD TC_Render_Compat_Pull_5();
DWORD TC_Render_Compat_Pull_6();
DWORD TC_Render_Compat_Pull_7();
DWORD TC_Render_Compat_Pull_8();
DWORD TC_Render_Compat_Pull_9();
DWORD TC_Render_Compat_Pull_10();
DWORD TC_Render_Compat_Pull_11();
DWORD TC_Render_Compat_Pull_12();
DWORD TC_Render_Compat_Pull_13();
DWORD TC_Render_Compat_Pull_14();
DWORD TC_Render_Compat_Pull_15();
DWORD TC_Render_Compat_Pull_16();
DWORD TC_Render_Compat_Pull_17();
DWORD TC_Render_Compat_Pull_18();
DWORD TC_Render_Compat_Pull_19();
DWORD TC_Render_Compat_Pull_20();
DWORD TC_Render_Compat_Pull_21();
DWORD TC_Render_Compat_Pull_22();
DWORD TC_Render_Compat_Pull_23();
DWORD TC_Render_Compat_Pull_24();
DWORD TC_Render_Compat_Pull_25();
DWORD TC_Render_Compat_Pull_26();
DWORD TC_Render_Compat_Pull_27();
DWORD TC_Render_Compat_Pull_28();
DWORD TC_Render_Compat_Pull_29();
DWORD TC_Render_Compat_Pull_30();
DWORD TC_Render_Compat_Pull_31();
DWORD TC_Render_Compat_Pull_32();
DWORD TC_Render_Compat_Pull_33();
DWORD TC_Render_Compat_Pull_34();
DWORD TC_Render_Compat_Pull_35();
DWORD TC_Render_Compat_Pull_36();
DWORD TC_Render_Compat_Pull_37();
DWORD TC_Render_Compat_Pull_38();
DWORD TC_Render_Compat_Pull_39();
DWORD TC_Render_Compat_Pull_40();
DWORD TC_Render_Compat_Pull_41();
DWORD TC_Render_Compat_Pull_42();
DWORD TC_Render_Compat_Pull_43();
DWORD TC_Render_Compat_Pull_44();
DWORD TC_Render_Compat_Pull_45();
DWORD TC_Render_Compat_Pull_46();
DWORD TC_Render_Compat_Pull_47();
DWORD TC_Render_Compat_Pull_48();
DWORD TC_Render_Compat_Pull_49();
DWORD TC_Render_Compat_Pull_50();
DWORD TC_Render_Compat_Pull_51();
DWORD TC_Render_Compat_Pull_52();
DWORD TC_Render_Compat_Pull_53();
DWORD TC_Render_Compat_Pull_54();
DWORD TC_Render_Compat_Pull_55();
DWORD TC_Render_Compat_Pull_56();
DWORD TC_Render_Compat_Pull_57();
DWORD TC_Render_Compat_Pull_58();
DWORD TC_Render_Compat_Pull_59();
DWORD TC_Render_Compat_Pull_60();
DWORD TC_Render_Compat_Pull_61();
DWORD TC_Render_Compat_Pull_62();

//   -Compat (SRC Default Quality)
//    -Push
DWORD TC_Render_Compat_SRC_Enhanced_Push_0();
DWORD TC_Render_Compat_SRC_Enhanced_Push_1();
DWORD TC_Render_Compat_SRC_Enhanced_Push_2();
DWORD TC_Render_Compat_SRC_Enhanced_Push_3();
DWORD TC_Render_Compat_SRC_Enhanced_Push_4();
DWORD TC_Render_Compat_SRC_Enhanced_Push_5();
DWORD TC_Render_Compat_SRC_Enhanced_Push_6();
DWORD TC_Render_Compat_SRC_Enhanced_Push_7();
DWORD TC_Render_Compat_SRC_Enhanced_Push_8();
DWORD TC_Render_Compat_SRC_Enhanced_Push_9();
DWORD TC_Render_Compat_SRC_Enhanced_Push_10();
DWORD TC_Render_Compat_SRC_Enhanced_Push_11();
DWORD TC_Render_Compat_SRC_Enhanced_Push_12();
DWORD TC_Render_Compat_SRC_Enhanced_Push_13();
DWORD TC_Render_Compat_SRC_Enhanced_Push_14();
DWORD TC_Render_Compat_SRC_Enhanced_Push_15();
DWORD TC_Render_Compat_SRC_Enhanced_Push_16();
DWORD TC_Render_Compat_SRC_Enhanced_Push_17();
DWORD TC_Render_Compat_SRC_Enhanced_Push_18();
DWORD TC_Render_Compat_SRC_Enhanced_Push_19();
DWORD TC_Render_Compat_SRC_Enhanced_Push_20();
DWORD TC_Render_Compat_SRC_Enhanced_Push_21();
DWORD TC_Render_Compat_SRC_Enhanced_Push_22();
DWORD TC_Render_Compat_SRC_Enhanced_Push_23();
DWORD TC_Render_Compat_SRC_Enhanced_Push_24();
DWORD TC_Render_Compat_SRC_Enhanced_Push_25();
DWORD TC_Render_Compat_SRC_Enhanced_Push_26();
DWORD TC_Render_Compat_SRC_Enhanced_Push_27();
DWORD TC_Render_Compat_SRC_Enhanced_Push_28();
DWORD TC_Render_Compat_SRC_Enhanced_Push_29();
DWORD TC_Render_Compat_SRC_Enhanced_Push_30();
DWORD TC_Render_Compat_SRC_Enhanced_Push_31();
DWORD TC_Render_Compat_SRC_Enhanced_Push_32();
DWORD TC_Render_Compat_SRC_Enhanced_Push_33();
DWORD TC_Render_Compat_SRC_Enhanced_Push_34();
DWORD TC_Render_Compat_SRC_Enhanced_Push_35();
DWORD TC_Render_Compat_SRC_Enhanced_Push_36();
DWORD TC_Render_Compat_SRC_Enhanced_Push_37();
DWORD TC_Render_Compat_SRC_Enhanced_Push_38();
DWORD TC_Render_Compat_SRC_Enhanced_Push_39();
DWORD TC_Render_Compat_SRC_Enhanced_Push_40();
DWORD TC_Render_Compat_SRC_Enhanced_Push_41();
DWORD TC_Render_Compat_SRC_Enhanced_Push_42();
DWORD TC_Render_Compat_SRC_Enhanced_Push_43();
DWORD TC_Render_Compat_SRC_Enhanced_Push_44();
DWORD TC_Render_Compat_SRC_Enhanced_Push_45();
DWORD TC_Render_Compat_SRC_Enhanced_Push_46();
DWORD TC_Render_Compat_SRC_Enhanced_Push_47();
DWORD TC_Render_Compat_SRC_Enhanced_Push_48();
DWORD TC_Render_Compat_SRC_Enhanced_Push_49();
DWORD TC_Render_Compat_SRC_Enhanced_Push_50();
DWORD TC_Render_Compat_SRC_Enhanced_Push_51();
DWORD TC_Render_Compat_SRC_Enhanced_Push_52();
DWORD TC_Render_Compat_SRC_Enhanced_Push_53();
DWORD TC_Render_Compat_SRC_Enhanced_Push_54();
DWORD TC_Render_Compat_SRC_Enhanced_Push_55();
DWORD TC_Render_Compat_SRC_Enhanced_Push_56();
DWORD TC_Render_Compat_SRC_Enhanced_Push_57();
DWORD TC_Render_Compat_SRC_Enhanced_Push_58();
DWORD TC_Render_Compat_SRC_Enhanced_Push_59();
DWORD TC_Render_Compat_SRC_Enhanced_Push_60();
DWORD TC_Render_Compat_SRC_Enhanced_Push_61();
DWORD TC_Render_Compat_SRC_Enhanced_Push_62();

//    -Pull
DWORD TC_Render_Compat_SRC_Enhanced_Pull_0();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_1();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_2();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_3();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_4();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_5();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_6();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_7();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_8();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_9();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_10();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_11();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_12();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_13();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_14();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_15();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_16();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_17();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_18();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_19();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_20();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_21();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_22();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_23();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_24();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_25();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_26();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_27();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_28();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_29();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_30();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_31();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_32();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_33();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_34();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_35();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_36();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_37();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_38();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_39();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_40();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_41();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_42();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_43();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_44();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_45();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_46();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_47();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_48();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_49();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_50();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_51();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_52();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_53();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_54();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_55();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_56();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_57();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_58();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_59();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_60();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_61();
DWORD TC_Render_Compat_SRC_Enhanced_Pull_62();


//   -Exclusive
//    -Push
DWORD TC_Render_Exclusive_Push_0();
DWORD TC_Render_Exclusive_Push_1();
DWORD TC_Render_Exclusive_Push_2();
DWORD TC_Render_Exclusive_Push_3();
DWORD TC_Render_Exclusive_Push_4();
DWORD TC_Render_Exclusive_Push_5();
DWORD TC_Render_Exclusive_Push_6();
DWORD TC_Render_Exclusive_Push_7();
DWORD TC_Render_Exclusive_Push_8();
DWORD TC_Render_Exclusive_Push_9();
DWORD TC_Render_Exclusive_Push_10();
DWORD TC_Render_Exclusive_Push_11();
DWORD TC_Render_Exclusive_Push_12();
DWORD TC_Render_Exclusive_Push_13();
DWORD TC_Render_Exclusive_Push_14();
DWORD TC_Render_Exclusive_Push_15();
DWORD TC_Render_Exclusive_Push_16();
DWORD TC_Render_Exclusive_Push_17();
DWORD TC_Render_Exclusive_Push_18();
DWORD TC_Render_Exclusive_Push_19();
DWORD TC_Render_Exclusive_Push_20();
DWORD TC_Render_Exclusive_Push_21();
DWORD TC_Render_Exclusive_Push_22();
DWORD TC_Render_Exclusive_Push_23();
DWORD TC_Render_Exclusive_Push_24();
DWORD TC_Render_Exclusive_Push_25();
DWORD TC_Render_Exclusive_Push_26();
DWORD TC_Render_Exclusive_Push_27();
DWORD TC_Render_Exclusive_Push_28();
DWORD TC_Render_Exclusive_Push_29();
DWORD TC_Render_Exclusive_Push_30();
DWORD TC_Render_Exclusive_Push_31();
DWORD TC_Render_Exclusive_Push_32();
DWORD TC_Render_Exclusive_Push_33();
DWORD TC_Render_Exclusive_Push_34();
DWORD TC_Render_Exclusive_Push_35();
DWORD TC_Render_Exclusive_Push_36();
DWORD TC_Render_Exclusive_Push_37();
DWORD TC_Render_Exclusive_Push_38();
DWORD TC_Render_Exclusive_Push_39();
DWORD TC_Render_Exclusive_Push_40();
DWORD TC_Render_Exclusive_Push_41();
DWORD TC_Render_Exclusive_Push_42();
DWORD TC_Render_Exclusive_Push_43();
DWORD TC_Render_Exclusive_Push_44();
DWORD TC_Render_Exclusive_Push_45();
DWORD TC_Render_Exclusive_Push_46();
DWORD TC_Render_Exclusive_Push_47();
DWORD TC_Render_Exclusive_Push_48();
DWORD TC_Render_Exclusive_Push_49();
DWORD TC_Render_Exclusive_Push_50();
DWORD TC_Render_Exclusive_Push_51();
DWORD TC_Render_Exclusive_Push_52();
DWORD TC_Render_Exclusive_Push_53();
DWORD TC_Render_Exclusive_Push_54();
DWORD TC_Render_Exclusive_Push_55();
DWORD TC_Render_Exclusive_Push_56();
DWORD TC_Render_Exclusive_Push_57();
DWORD TC_Render_Exclusive_Push_58();
DWORD TC_Render_Exclusive_Push_59();
DWORD TC_Render_Exclusive_Push_60();
DWORD TC_Render_Exclusive_Push_61();
DWORD TC_Render_Exclusive_Push_62();
//    -Pull
DWORD TC_Render_Exclusive_Pull_0();
DWORD TC_Render_Exclusive_Pull_1();
DWORD TC_Render_Exclusive_Pull_2();
DWORD TC_Render_Exclusive_Pull_3();
DWORD TC_Render_Exclusive_Pull_4();
DWORD TC_Render_Exclusive_Pull_5();
DWORD TC_Render_Exclusive_Pull_6();
DWORD TC_Render_Exclusive_Pull_7();
DWORD TC_Render_Exclusive_Pull_8();
DWORD TC_Render_Exclusive_Pull_9();
DWORD TC_Render_Exclusive_Pull_10();
DWORD TC_Render_Exclusive_Pull_11();
DWORD TC_Render_Exclusive_Pull_12();
DWORD TC_Render_Exclusive_Pull_13();
DWORD TC_Render_Exclusive_Pull_14();
DWORD TC_Render_Exclusive_Pull_15();
DWORD TC_Render_Exclusive_Pull_16();
DWORD TC_Render_Exclusive_Pull_17();
DWORD TC_Render_Exclusive_Pull_18();
DWORD TC_Render_Exclusive_Pull_19();
DWORD TC_Render_Exclusive_Pull_20();
DWORD TC_Render_Exclusive_Pull_21();
DWORD TC_Render_Exclusive_Pull_22();
DWORD TC_Render_Exclusive_Pull_23();
DWORD TC_Render_Exclusive_Pull_24();
DWORD TC_Render_Exclusive_Pull_25();
DWORD TC_Render_Exclusive_Pull_26();
DWORD TC_Render_Exclusive_Pull_27();
DWORD TC_Render_Exclusive_Pull_28();
DWORD TC_Render_Exclusive_Pull_29();
DWORD TC_Render_Exclusive_Pull_30();
DWORD TC_Render_Exclusive_Pull_31();
DWORD TC_Render_Exclusive_Pull_32();
DWORD TC_Render_Exclusive_Pull_33();
DWORD TC_Render_Exclusive_Pull_34();
DWORD TC_Render_Exclusive_Pull_35();
DWORD TC_Render_Exclusive_Pull_36();
DWORD TC_Render_Exclusive_Pull_37();
DWORD TC_Render_Exclusive_Pull_38();
DWORD TC_Render_Exclusive_Pull_39();
DWORD TC_Render_Exclusive_Pull_40();
DWORD TC_Render_Exclusive_Pull_41();
DWORD TC_Render_Exclusive_Pull_42();
DWORD TC_Render_Exclusive_Pull_43();
DWORD TC_Render_Exclusive_Pull_44();
DWORD TC_Render_Exclusive_Pull_45();
DWORD TC_Render_Exclusive_Pull_46();
DWORD TC_Render_Exclusive_Pull_47();
DWORD TC_Render_Exclusive_Pull_48();
DWORD TC_Render_Exclusive_Pull_49();
DWORD TC_Render_Exclusive_Pull_50();
DWORD TC_Render_Exclusive_Pull_51();
DWORD TC_Render_Exclusive_Pull_52();
DWORD TC_Render_Exclusive_Pull_53();
DWORD TC_Render_Exclusive_Pull_54();
DWORD TC_Render_Exclusive_Pull_55();
DWORD TC_Render_Exclusive_Pull_56();
DWORD TC_Render_Exclusive_Pull_57();
DWORD TC_Render_Exclusive_Pull_58();
DWORD TC_Render_Exclusive_Pull_59();
DWORD TC_Render_Exclusive_Pull_60();
DWORD TC_Render_Exclusive_Pull_61();
DWORD TC_Render_Exclusive_Pull_62();
// - Capture
//   -Shared
//    -Push
DWORD TC_Capture_Shared_Push();
//    -Pull
DWORD TC_Capture_Shared_Pull();
//   -Compat
//    -Push
DWORD TC_Capture_Compat_Push_0();
DWORD TC_Capture_Compat_Push_1();
DWORD TC_Capture_Compat_Push_2();
DWORD TC_Capture_Compat_Push_3();
DWORD TC_Capture_Compat_Push_4();
DWORD TC_Capture_Compat_Push_5();
DWORD TC_Capture_Compat_Push_6();
DWORD TC_Capture_Compat_Push_7();
DWORD TC_Capture_Compat_Push_8();
DWORD TC_Capture_Compat_Push_9();
DWORD TC_Capture_Compat_Push_10();
DWORD TC_Capture_Compat_Push_11();
DWORD TC_Capture_Compat_Push_12();
DWORD TC_Capture_Compat_Push_13();
DWORD TC_Capture_Compat_Push_14();
DWORD TC_Capture_Compat_Push_15();
DWORD TC_Capture_Compat_Push_16();
DWORD TC_Capture_Compat_Push_17();
DWORD TC_Capture_Compat_Push_18();
DWORD TC_Capture_Compat_Push_19();
DWORD TC_Capture_Compat_Push_20();
DWORD TC_Capture_Compat_Push_21();
DWORD TC_Capture_Compat_Push_22();
DWORD TC_Capture_Compat_Push_23();
DWORD TC_Capture_Compat_Push_24();
DWORD TC_Capture_Compat_Push_25();
DWORD TC_Capture_Compat_Push_26();
DWORD TC_Capture_Compat_Push_27();
DWORD TC_Capture_Compat_Push_28();
DWORD TC_Capture_Compat_Push_29();
DWORD TC_Capture_Compat_Push_30();
DWORD TC_Capture_Compat_Push_31();
DWORD TC_Capture_Compat_Push_32();
DWORD TC_Capture_Compat_Push_33();
DWORD TC_Capture_Compat_Push_34();
DWORD TC_Capture_Compat_Push_35();
DWORD TC_Capture_Compat_Push_36();
DWORD TC_Capture_Compat_Push_37();
DWORD TC_Capture_Compat_Push_38();
DWORD TC_Capture_Compat_Push_39();
DWORD TC_Capture_Compat_Push_40();
DWORD TC_Capture_Compat_Push_41();
DWORD TC_Capture_Compat_Push_42();
DWORD TC_Capture_Compat_Push_43();
DWORD TC_Capture_Compat_Push_44();
DWORD TC_Capture_Compat_Push_45();
DWORD TC_Capture_Compat_Push_46();
DWORD TC_Capture_Compat_Push_47();
DWORD TC_Capture_Compat_Push_48();
DWORD TC_Capture_Compat_Push_49();
DWORD TC_Capture_Compat_Push_50();
DWORD TC_Capture_Compat_Push_51();
DWORD TC_Capture_Compat_Push_52();
DWORD TC_Capture_Compat_Push_53();
DWORD TC_Capture_Compat_Push_54();
DWORD TC_Capture_Compat_Push_55();
DWORD TC_Capture_Compat_Push_56();
DWORD TC_Capture_Compat_Push_57();
DWORD TC_Capture_Compat_Push_58();
DWORD TC_Capture_Compat_Push_59();
DWORD TC_Capture_Compat_Push_60();
DWORD TC_Capture_Compat_Push_61();
DWORD TC_Capture_Compat_Push_62();
//    -Pull
DWORD TC_Capture_Compat_Pull_0();
DWORD TC_Capture_Compat_Pull_1();
DWORD TC_Capture_Compat_Pull_2();
DWORD TC_Capture_Compat_Pull_3();
DWORD TC_Capture_Compat_Pull_4();
DWORD TC_Capture_Compat_Pull_5();
DWORD TC_Capture_Compat_Pull_6();
DWORD TC_Capture_Compat_Pull_7();
DWORD TC_Capture_Compat_Pull_8();
DWORD TC_Capture_Compat_Pull_9();
DWORD TC_Capture_Compat_Pull_10();
DWORD TC_Capture_Compat_Pull_11();
DWORD TC_Capture_Compat_Pull_12();
DWORD TC_Capture_Compat_Pull_13();
DWORD TC_Capture_Compat_Pull_14();
DWORD TC_Capture_Compat_Pull_15();
DWORD TC_Capture_Compat_Pull_16();
DWORD TC_Capture_Compat_Pull_17();
DWORD TC_Capture_Compat_Pull_18();
DWORD TC_Capture_Compat_Pull_19();
DWORD TC_Capture_Compat_Pull_20();
DWORD TC_Capture_Compat_Pull_21();
DWORD TC_Capture_Compat_Pull_22();
DWORD TC_Capture_Compat_Pull_23();
DWORD TC_Capture_Compat_Pull_24();
DWORD TC_Capture_Compat_Pull_25();
DWORD TC_Capture_Compat_Pull_26();
DWORD TC_Capture_Compat_Pull_27();
DWORD TC_Capture_Compat_Pull_28();
DWORD TC_Capture_Compat_Pull_29();
DWORD TC_Capture_Compat_Pull_30();
DWORD TC_Capture_Compat_Pull_31();
DWORD TC_Capture_Compat_Pull_32();
DWORD TC_Capture_Compat_Pull_33();
DWORD TC_Capture_Compat_Pull_34();
DWORD TC_Capture_Compat_Pull_35();
DWORD TC_Capture_Compat_Pull_36();
DWORD TC_Capture_Compat_Pull_37();
DWORD TC_Capture_Compat_Pull_38();
DWORD TC_Capture_Compat_Pull_39();
DWORD TC_Capture_Compat_Pull_40();
DWORD TC_Capture_Compat_Pull_41();
DWORD TC_Capture_Compat_Pull_42();
DWORD TC_Capture_Compat_Pull_43();
DWORD TC_Capture_Compat_Pull_44();
DWORD TC_Capture_Compat_Pull_45();
DWORD TC_Capture_Compat_Pull_46();
DWORD TC_Capture_Compat_Pull_47();
DWORD TC_Capture_Compat_Pull_48();
DWORD TC_Capture_Compat_Pull_49();
DWORD TC_Capture_Compat_Pull_50();
DWORD TC_Capture_Compat_Pull_51();
DWORD TC_Capture_Compat_Pull_52();
DWORD TC_Capture_Compat_Pull_53();
DWORD TC_Capture_Compat_Pull_54();
DWORD TC_Capture_Compat_Pull_55();
DWORD TC_Capture_Compat_Pull_56();
DWORD TC_Capture_Compat_Pull_57();
DWORD TC_Capture_Compat_Pull_58();
DWORD TC_Capture_Compat_Pull_59();
DWORD TC_Capture_Compat_Pull_60();
DWORD TC_Capture_Compat_Pull_61();
DWORD TC_Capture_Compat_Pull_62();
//   -Compat (SRC Default Quality)
//    -Push
DWORD TC_Capture_Compat_SRC_Enhanced_Push_0();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_1();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_2();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_3();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_4();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_5();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_6();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_7();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_8();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_9();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_10();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_11();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_12();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_13();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_14();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_15();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_16();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_17();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_18();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_19();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_20();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_21();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_22();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_23();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_24();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_25();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_26();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_27();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_28();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_29();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_30();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_31();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_32();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_33();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_34();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_35();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_36();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_37();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_38();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_39();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_40();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_41();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_42();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_43();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_44();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_45();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_46();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_47();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_48();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_49();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_50();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_51();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_52();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_53();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_54();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_55();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_56();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_57();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_58();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_59();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_60();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_61();
DWORD TC_Capture_Compat_SRC_Enhanced_Push_62();
//    -Pull
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_0();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_1();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_2();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_3();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_4();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_5();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_6();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_7();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_8();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_9();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_10();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_11();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_12();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_13();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_14();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_15();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_16();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_17();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_18();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_19();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_20();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_21();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_22();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_23();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_24();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_25();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_26();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_27();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_28();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_29();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_30();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_31();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_32();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_33();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_34();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_35();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_36();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_37();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_38();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_39();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_40();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_41();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_42();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_43();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_44();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_45();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_46();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_47();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_48();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_49();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_50();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_51();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_52();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_53();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_54();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_55();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_56();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_57();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_58();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_59();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_60();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_61();
DWORD TC_Capture_Compat_SRC_Enhanced_Pull_62();
//   -Exclusive
//    -Push
DWORD TC_Capture_Exclusive_Push_0();
DWORD TC_Capture_Exclusive_Push_1();
DWORD TC_Capture_Exclusive_Push_2();
DWORD TC_Capture_Exclusive_Push_3();
DWORD TC_Capture_Exclusive_Push_4();
DWORD TC_Capture_Exclusive_Push_5();
DWORD TC_Capture_Exclusive_Push_6();
DWORD TC_Capture_Exclusive_Push_7();
DWORD TC_Capture_Exclusive_Push_8();
DWORD TC_Capture_Exclusive_Push_9();
DWORD TC_Capture_Exclusive_Push_10();
DWORD TC_Capture_Exclusive_Push_11();
DWORD TC_Capture_Exclusive_Push_12();
DWORD TC_Capture_Exclusive_Push_13();
DWORD TC_Capture_Exclusive_Push_14();
DWORD TC_Capture_Exclusive_Push_15();
DWORD TC_Capture_Exclusive_Push_16();
DWORD TC_Capture_Exclusive_Push_17();
DWORD TC_Capture_Exclusive_Push_18();
DWORD TC_Capture_Exclusive_Push_19();
DWORD TC_Capture_Exclusive_Push_20();
DWORD TC_Capture_Exclusive_Push_21();
DWORD TC_Capture_Exclusive_Push_22();
DWORD TC_Capture_Exclusive_Push_23();
DWORD TC_Capture_Exclusive_Push_24();
DWORD TC_Capture_Exclusive_Push_25();
DWORD TC_Capture_Exclusive_Push_26();
DWORD TC_Capture_Exclusive_Push_27();
DWORD TC_Capture_Exclusive_Push_28();
DWORD TC_Capture_Exclusive_Push_29();
DWORD TC_Capture_Exclusive_Push_30();
DWORD TC_Capture_Exclusive_Push_31();
DWORD TC_Capture_Exclusive_Push_32();
DWORD TC_Capture_Exclusive_Push_33();
DWORD TC_Capture_Exclusive_Push_34();
DWORD TC_Capture_Exclusive_Push_35();
DWORD TC_Capture_Exclusive_Push_36();
DWORD TC_Capture_Exclusive_Push_37();
DWORD TC_Capture_Exclusive_Push_38();
DWORD TC_Capture_Exclusive_Push_39();
DWORD TC_Capture_Exclusive_Push_40();
DWORD TC_Capture_Exclusive_Push_41();
DWORD TC_Capture_Exclusive_Push_42();
DWORD TC_Capture_Exclusive_Push_43();
DWORD TC_Capture_Exclusive_Push_44();
DWORD TC_Capture_Exclusive_Push_45();
DWORD TC_Capture_Exclusive_Push_46();
DWORD TC_Capture_Exclusive_Push_47();
DWORD TC_Capture_Exclusive_Push_48();
DWORD TC_Capture_Exclusive_Push_49();
DWORD TC_Capture_Exclusive_Push_50();
DWORD TC_Capture_Exclusive_Push_51();
DWORD TC_Capture_Exclusive_Push_52();
DWORD TC_Capture_Exclusive_Push_53();
DWORD TC_Capture_Exclusive_Push_54();
DWORD TC_Capture_Exclusive_Push_55();
DWORD TC_Capture_Exclusive_Push_56();
DWORD TC_Capture_Exclusive_Push_57();
DWORD TC_Capture_Exclusive_Push_58();
DWORD TC_Capture_Exclusive_Push_59();
DWORD TC_Capture_Exclusive_Push_60();
DWORD TC_Capture_Exclusive_Push_61();
DWORD TC_Capture_Exclusive_Push_62();
//    -Pull
DWORD TC_Capture_Exclusive_Pull_0();
DWORD TC_Capture_Exclusive_Pull_1();
DWORD TC_Capture_Exclusive_Pull_2();
DWORD TC_Capture_Exclusive_Pull_3();
DWORD TC_Capture_Exclusive_Pull_4();
DWORD TC_Capture_Exclusive_Pull_5();
DWORD TC_Capture_Exclusive_Pull_6();
DWORD TC_Capture_Exclusive_Pull_7();
DWORD TC_Capture_Exclusive_Pull_8();
DWORD TC_Capture_Exclusive_Pull_9();
DWORD TC_Capture_Exclusive_Pull_10();
DWORD TC_Capture_Exclusive_Pull_11();
DWORD TC_Capture_Exclusive_Pull_12();
DWORD TC_Capture_Exclusive_Pull_13();
DWORD TC_Capture_Exclusive_Pull_14();
DWORD TC_Capture_Exclusive_Pull_15();
DWORD TC_Capture_Exclusive_Pull_16();
DWORD TC_Capture_Exclusive_Pull_17();
DWORD TC_Capture_Exclusive_Pull_18();
DWORD TC_Capture_Exclusive_Pull_19();
DWORD TC_Capture_Exclusive_Pull_20();
DWORD TC_Capture_Exclusive_Pull_21();
DWORD TC_Capture_Exclusive_Pull_22();
DWORD TC_Capture_Exclusive_Pull_23();
DWORD TC_Capture_Exclusive_Pull_24();
DWORD TC_Capture_Exclusive_Pull_25();
DWORD TC_Capture_Exclusive_Pull_26();
DWORD TC_Capture_Exclusive_Pull_27();
DWORD TC_Capture_Exclusive_Pull_28();
DWORD TC_Capture_Exclusive_Pull_29();
DWORD TC_Capture_Exclusive_Pull_30();
DWORD TC_Capture_Exclusive_Pull_31();
DWORD TC_Capture_Exclusive_Pull_32();
DWORD TC_Capture_Exclusive_Pull_33();
DWORD TC_Capture_Exclusive_Pull_34();
DWORD TC_Capture_Exclusive_Pull_35();
DWORD TC_Capture_Exclusive_Pull_36();
DWORD TC_Capture_Exclusive_Pull_37();
DWORD TC_Capture_Exclusive_Pull_38();
DWORD TC_Capture_Exclusive_Pull_39();
DWORD TC_Capture_Exclusive_Pull_40();
DWORD TC_Capture_Exclusive_Pull_41();
DWORD TC_Capture_Exclusive_Pull_42();
DWORD TC_Capture_Exclusive_Pull_43();
DWORD TC_Capture_Exclusive_Pull_44();
DWORD TC_Capture_Exclusive_Pull_45();
DWORD TC_Capture_Exclusive_Pull_46();
DWORD TC_Capture_Exclusive_Pull_47();
DWORD TC_Capture_Exclusive_Pull_48();
DWORD TC_Capture_Exclusive_Pull_49();
DWORD TC_Capture_Exclusive_Pull_50();
DWORD TC_Capture_Exclusive_Pull_51();
DWORD TC_Capture_Exclusive_Pull_52();
DWORD TC_Capture_Exclusive_Pull_53();
DWORD TC_Capture_Exclusive_Pull_54();
DWORD TC_Capture_Exclusive_Pull_55();
DWORD TC_Capture_Exclusive_Pull_56();
DWORD TC_Capture_Exclusive_Pull_57();
DWORD TC_Capture_Exclusive_Pull_58();
DWORD TC_Capture_Exclusive_Pull_59();
DWORD TC_Capture_Exclusive_Pull_60();
DWORD TC_Capture_Exclusive_Pull_61();
DWORD TC_Capture_Exclusive_Pull_62();

// -Loopback
DWORD TC_Loopback_Push_Shared();
DWORD TC_Loopback_Push_Compat();
DWORD TC_Loopback_Push_Compat_Enhanced();
DWORD TC_Loopback_Pull_Shared();
DWORD TC_Loopback_Pull_Compat();
DWORD TC_Loopback_Pull_Compat_Enhanced();

// Security Tests
DWORD TC_Security_InvalidFloatStreaming();

// Stress Tests
DWORD Tc_AudioSessionStress();
DWORD Tc_SessionVolumeStress();

// Longhaul Tests
DWORD Tc_RenderLonghaul();
DWORD Tc_CaptureLonghaul();
