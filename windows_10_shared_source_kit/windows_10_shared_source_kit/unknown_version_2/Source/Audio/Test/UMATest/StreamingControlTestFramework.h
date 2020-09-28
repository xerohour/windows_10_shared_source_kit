// Copyright(C) Microsoft. All rights reserved.

//
// StreamingControlTestFramework.h
//  
//
// Description:
//
//  Common streaming control interface test functions
//

#pragma once

//
// Tolerance when comparing volume values
//
const static DOUBLE EPSILON = .1;

//
// Test ID Enums
//

typedef enum {
                SIMPLEAUDIOVOLUME_SETMASTERVOLUME_BASIC0=0,
                SIMPLEAUDIOVOLUME_SETMASTERVOLUME_VALID0,
                SIMPLEAUDIOVOLUME_GETSETMUTE_BASIC0,
                SIMPLEAUDIOVOLUME_SETMASTERVOLUME_INVALID0,
                SIMPLEAUDIOVOLUME_SETMASTERVOLUME_INVALID1,
                SIMPLEAUDIOVOLUME_GETMASTERVOLUME_INVALID0,
                SIMPLEAUDIOVOLUME_GETMUTE_INVALID0,
                SIMPLEAUDIOVOLUME_DISCONNECTSERVICE,
                CHANNELAUDIOVOLUME_GETCHANNELCOUNT_VALID0,
                CHANNELAUDIOVOLUME_SETCHANNELVOLUME_VALID0,
                CHANNELAUDIOVOLUME_SETCHANNELVOLUME_VALID1,
                CHANNELAUDIOVOLUME_GETSETALLVOLUMES_VALID0,
                CHANNELAUDIOVOLUME_GETSETALLVOLUMES_VALID1,
                CHANNELAUDIOVOLUME_GETCHANNELCOUNT_INVALID0,
                CHANNELAUDIOVOLUME_SETCHANNELVOLUME_INVALID0,
                CHANNELAUDIOVOLUME_SETCHANNELVOLUME_INVALID1,
                CHANNELAUDIOVOLUME_SETCHANNELVOLUME_INVALID2,
                CHANNELAUDIOVOLUME_GETCHANNELVOLUME_INVALID0,
                CHANNELAUDIOVOLUME_GETCHANNELVOLUME_INVALID1,
                CHANNELAUDIOVOLUME_GETCHANNELVOLUME_INVALID2,
                CHANNELAUDIOVOLUME_SETALLVOLUMES_INVALID0,
                CHANNELAUDIOVOLUME_SETALLVOLUMES_INVALID1,
                CHANNELAUDIOVOLUME_SETALLVOLUMES_INVALID2,
                CHANNELAUDIOVOLUME_SETALLVOLUMES_INVALID3,
                CHANNELAUDIOVOLUME_GETALLVOLUMES_INVALID0,
                CHANNELAUDIOVOLUME_GETALLVOLUMES_INVALID1,
                CHANNELAUDIOVOLUME_GETALLVOLUMES_INVALID2,
                CHANNELAUDIOVOLUME_DISCONNECTSERVICE,
                STREAMAUDIOVOLUME_GETCHANNELCOUNT_VALID0,
                STREAMAUDIOVOLUME_GETSETCHANNELVOLUME_VALID0,
                STREAMAUDIOVOLUME_GETSETALLVOLUMES_VALID0,
                STREAMAUDIOVOLUME_GETSETALLVOLUMES_VALID1,
                STREAMAUDIOVOLUME_GETCHANNELCOUNT_INVALID0,
                STREAMAUDIOVOLUME_SETCHANNELVOLUME_INVALID0,
                STREAMAUDIOVOLUME_GETCHANNELVOLUME_INVALID0,
                STREAMAUDIOVOLUME_GETALLVOLUMES_INVALID0,
                STREAMAUDIOVOLUME_SETALLVOLUMES_INVALID0,
                STREAMAUDIOVOLUME_DISCONNECTSERVICE,
                AUDIOSESSIONCONTROL_GETSESSIONIDENTIFIER_VALID0,
                AUDIOSESSIONCONTROL_GETSTATE_VALID0,
                AUDIOSESSIONCONTROL_GETSTATE_VALID1,
                AUDIOSESSIONCONTROL_GETLASTACTIVATION_VALID0,
                AUDIOSESSIONCONTROL_GETLASTACTIVATION_VALID1,
                AUDIOSESSIONCONTROL_GETLASTINACTIVATION_VALID0,
                AUDIOSESSIONCONTROL_GETLASTINACTIVATION_VALID1,
                AUDIOSESSIONCONTROL_GETDISPLAYNAME_VALID0,
                AUDIOSESSIONCONTROL_GETDISPLAYNAME_VALID1,
                AUDIOSESSIONCONTROL_SETDISPLAYNAME_VALID0,
                AUDIOSESSIONCONTROL_SETDISPLAYNAME_VALID1,
                AUDIOSESSIONCONTROL_GETICONPATH_VALID0,
                AUDIOSESSIONCONTROL_SETICONPATH_VALID0,
                AUDIOSESSIONCONTROL_GETGROUPINGPARAM_VALID0,
                AUDIOSESSIONCONTROL_SETGROUPINGPARAM_VALID0,
                AUDIOSESSIONCONTROL_NOTIFICATIONS_VALID0,
                AUDIOSESSIONCONTROL_NOTIFICATIONS_VALID1,
                AUDIOSESSIONCONTROL_GETPROCESSID_VALID0,
                AUDIOSESSIONCONTROL_GETSESSIONID_INVALID0,
                AUDIOSESSIONCONTROL_GETSTATE_INVALID0,
                AUDIOSESSIONCONTROL_GETLASTACTIVATION_INVALID0,
                AUDIOSESSIONCONTROL_GETLASTINACTIVATION_INVALID0,
                AUDIOSESSIONCONTROL_GETDISPLAYNAME_INVALID0,
                AUDIOSESSIONCONTROL_SETDISPLAYNAME_INVALID0,
                AUDIOSESSIONCONTROL_GETGROUPINGPARAM_INVALID0,
                AUDIOSESSIONCONTROL_ADDAUDIOSESNOTIF_INVALID0,
                AUDIOSESSIONCONTROL_DELAUDIOSESNOTIF_INVALID0,
                AUDIOSESSIONCONTROL_GETPROCESSID_INVALID0,
                AUDIOSESSIONCONTROL_DISCONNECTSERVICE_INVALID0,
                PERSTREAMEVENTS_SCENARIO1,
                PERSTREAMEVENTS_SCENARIO2,
                PERSTREAMEVENTS_SCENARIO3,
                PERSTREAMEVENTS_SCENARIO4,
                PERSTREAMEVENTS_SCENARIO5,
                PERSTREAMEVENTS_SCENARIO6,                
                PERSTREAMEVENTS_SCENARIO7,
                PERSTREAMEVENTS_SCENARIO8,
                PERSTREAMEVENTS_SCENARIO9,
                ENDPOINTVOLUME_SETMASTERVOL_VALID0,
                ENDPOINTVOLUME_SETMASTERVOL_VALID1,
                ENDPOINTVOLUME_SETMASTERVOL_VALID2,
                ENDPOINTVOLUME_SETMASTERVOL_VALID3,
                ENDPOINTVOLUME_SETMASTERVOL_SCALAR_VALID0,
                ENDPOINTVOLUME_SETMASTERVOL_SCALAR_VALID1,
                ENDPOINTVOLUME_SETMASTERVOL_SCALAR_VALID2,
                ENDPOINTVOLUME_SETMASTERVOL_SCALAR_VALID3,
                ENDPOINTVOLUME_SETMUTE_VALID0,
                ENDPOINTVOLUME_SETMUTE_VALID1,
                ENDPOINTVOLUME_VOLUMESTEPPING_VALID1,
                ENDPOINTVOLUME_VOLUMESTEPPING_VALID2,
                ENDPOINTVOLUME_GETCHANNELCOUNT_VALID1,
                ENDPOINTVOLUME_SETCHANNELVOLUME_VALID1,
                ENDPOINTVOLUME_SETCHANNELVOLUME_VALID2,
                ENDPOINTVOLUME_SETCHANNELVOLUME_VALID3,
                ENDPOINTVOLUME_SETCHANNELVOLUME_VALID4,
                ENDPOINTVOLUME_SETCHANNELVOLUME_VALID5,
                ENDPOINTVOLUME_SETCHANNELVOLUMESCALAR_VALID1,
                ENDPOINTVOLUME_SETCHANNELVOLUMESCALAR_VALID2,
                ENDPOINTVOLUME_SETCHANNELVOLUMESCALAR_VALID3,
                ENDPOINTVOLUME_SETCHANNELVOLUMESCALAR_VALID4,
                ENDPOINTVOLUME_SETCHANNELVOLUMESCALAR_VALID5,
                ENDPOINTVOLUME_QUERYHWSUPPORT_VALID1,
                ENDPOINTVOLUME_GETVOLUMERANGE_VALID1,
                ENDPOINTVOLUME_GETMASTERVOL_INVALID0,
                ENDPOINTVOLUME_GETMASTERVOL_SCALAR_INVALID0,
                ENDPOINTVOLUME_GETMUTE_INVALID0,
                ENDPOINTVOLUME_VOLUMESTEPPING_INVALID0,
                ENDPOINTVOLUME_GETCHANNELCOUNT_INVALID0,
                ENDPOINTVOLUME_SETCHANNELVOLUMELEVEL_INVALID0,
                ENDPOINTVOLUME_GETCHANNELVOLUMELEVEL_INVALID0,
                ENDPOINTVOLUME_SETCHANNELVOLUMELEVELSCALAR_INVALID0,
                ENDPOINTVOLUME_GETCHANNELVOLUMELEVELSCALAR_INVALID0,
                ENDPOINTVOLUME_QUERYHWSUPPORT_INVALID0,
                ENDPOINTVOLUME_GETVOLUMERANGE_INVALID1,
                ENDPOINTVOLUME_DISCONNECTSERVICE_INVALID0,
                PERENDPOINTEVENTS_SCENARIO1,
                PERENDPOINTEVENTS_SCENARIO2,
                PERENDPOINTEVENTS_SCENARIO3,
                PERENDPOINTEVENTS_SCENARIOIV1,
                AUDMETERINF_VALID0,
                AUDMETERINF_VALID1,
                AUDMETERINF_INVALID0,
                AUDMETERINF_DISCONNECTSERVICE,
                SESSIONMGMT_GETSESSIONENUM_VALID1,
                SESSIONMGMT_GETSESSIONENUM_VALID2,
                SESSIONMGMT_GETSESSIONENUM_VALID3,
                SESSIONMGMT_ADDSESSIONNOTIF_VALID1,
                SESSIONMGMT_ADDSESSIONNOTIF_VALID2,
                SESSIONMGMT_ADDSESSIONNOTIF_VALID3,
                SESSIONMGMT_ADDSESSIONNOTIF_VALID4,                
                SESSIONMGMT_DELSESSIONNOTIF_VALID1,
                SESSIONMGMT_GETSESSION_VALID1,
                SESSIONMGMT_GETSESSION_VALID2,
                SESSIONMGMT_GETSESSION_VALID3,
                SESSIONMGMT_GETSESSION_VALID4,
                SESSIONMGMT_ADDSESSIONNOTIF_INVALID1,
                SESSIONMGMT_DELSESSIONNOTIF_INVALID1,
                SESSIONMGMT_DISCONNECTSERVICE
              } SCONTROL_TEST_ID;

//
// Test Helper Prototypes
//
DWORD RunStreamingControlTest(SCONTROL_TEST_ID nTestID, BOOL bSuppressStreamingErrors=FALSE);
DWORD RunCaptureStreamingControlTest(SCONTROL_TEST_ID nTestID, BOOL bSuppressStreamingErrors);
DWORD RunRenderStreamingControlTest(SCONTROL_TEST_ID nTestID, BOOL bSuppressStreamingErrors, BOOL bEnableHardwareOffload);

DWORD DispatchTest(SCONTROL_TEST_ID nTestID, DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine, BOOL& bIgnoreStreamingErrors);

//
// Test Case Helper Functions
//

// ISimpleAudioVolume
DWORD Test_SimpleAudioVolume_SetMasterVolume_Basic0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_SimpleAudioVolume_SetMasterVolume_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_SimpleAudioVolume_GetSetMute_Basic0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_SimpleAudioVolume_SetMasterVolume_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_SimpleAudioVolume_SetMasterVolume_Invalid1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_SimpleAudioVolume_GetMasterVolume_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_SimpleAudioVolume_GetMute_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_SimpleAudioVolume_DisconnectService(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);

// IChannelAudioVolume
DWORD Test_ChannelAudioVolume_GetChannelCount_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_ChannelAudioVolume_SetChannelVolume_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_ChannelAudioVolume_SetChannelVolume_Valid1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_ChannelAudioVolume_GetSetAllVolumes_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_ChannelAudioVolume_GetSetAllVolumes_Valid1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_ChannelAudioVolume_GetChannelCount_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_ChannelAudioVolume_SetChannelVolume_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_ChannelAudioVolume_SetChannelVolume_Invalid1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_ChannelAudioVolume_SetChannelVolume_Invalid2(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_ChannelAudioVolume_GetChannelVolume_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_ChannelAudioVolume_GetChannelVolume_Invalid1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_ChannelAudioVolume_GetChannelVolume_Invalid2(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_ChannelAudioVolume_SetAllVolumes_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_ChannelAudioVolume_SetAllVolumes_Invalid1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_ChannelAudioVolume_SetAllVolumes_Invalid2(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_ChannelAudioVolume_SetAllVolumes_Invalid3(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_ChannelAudioVolume_GetAllVolumes_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_ChannelAudioVolume_GetAllVolumes_Invalid1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_ChannelAudioVolume_GetAllVolumes_Invalid2(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_ChannelAudioVolume_DisconnectService(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);

// IAudioStreamVolume
DWORD Test_StreamAudioVolume_GetChannelCount_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_StreamAudioVolume_GetSetChannelVolume_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_StreamAudioVolume_GetSetAllVolumes_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_StreamAudioVolume_GetSetAllVolumes_Valid1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_StreamAudioVolume_GetChannelCount_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_StreamAudioVolume_SetChannelVolume_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_StreamAudioVolume_GetChannelVolume_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_StreamAudioVolume_GetAllVolumes_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_StreamAudioVolume_SetAllVolumes_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_StreamAudioVolume_DisconnectService(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);

// IAudioSessionControl
DWORD Test_AudioSessionControl_GetSessionID_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_AudioSessionControl_GetState_Valid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_AudioSessionControl_GetState_Valid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_AudioSessionControl_GetLastActivation_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_AudioSessionControl_GetLastActivation_Valid1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_AudioSessionControl_GetLastInactivation_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_AudioSessionControl_GetLastInactivation_Valid1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_AudioSessionControl_GetDisplayName_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_AudioSessionControl_SetDisplayName_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_AudioSessionControl_SetDisplayName_Valid1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_AudioSessionControl_GetIconPath_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_AudioSessionControl_SetIconPath_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_AudioSessionControl_GetGroupingParam_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_AudioSessionControl_SetGroupingParam_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_AudioSessionControl_Notifications_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_AudioSessionControl_Notifications_Valid1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_AudioSessionControl_GetProcessId_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_AudioSessionControl_GetSessionID_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_AudioSessionControl_GetState_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_AudioSessionControl_GetLastActivation_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_AudioSessionControl_GetLastInactivation_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_AudioSessionControl_GetDisplayName_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_AudioSessionControl_SetDisplayName_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_AudioSessionControl_GetGroupingParam_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_AudioSessionControl_AddAudioSesNotif_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_AudioSessionControl_DelAudioSesNotif_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_AudioSessionControl_GetProcessId_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_AudioSessionControl_DisconnectService_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);

// Per-Stream/Session Eventing Scenarios
DWORD Test_PerStreamEvents_Scenario1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_PerStreamEvents_Scenario2(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_PerStreamEvents_Scenario3(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_PerStreamEvents_Scenario4(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_PerStreamEvents_Scenario5(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_PerStreamEvents_Scenario6(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_PerStreamEvents_Scenario7(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_PerStreamEvents_Scenario8(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_PerStreamEvents_Scenario9(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);

// IAudioEndpointVolume
DWORD Test_EndpointVolume_SetMasterVol_Valid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_SetMasterVol_Valid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_SetMasterVol_Valid2(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_SetMasterVol_Valid3(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_SetMasterVol_Scalar_Valid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_SetMasterVol_Scalar_Valid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_SetMasterVol_Scalar_Valid2(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_SetMasterVol_Scalar_Valid3(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_SetMute_Valid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_SetMute_Valid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_VolumeStepping_Valid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_VolumeStepping_Valid2(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_GetChannelCount_Valid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_SetChannelVolumeLevel_Valid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_SetChannelVolumeLevel_Valid2(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_SetChannelVolumeLevel_Valid3(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_SetChannelVolumeLevel_Valid4(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_SetChannelVolumeLevel_Valid5(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_SetChannelVolumeLevelScalar_Valid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_SetChannelVolumeLevelScalar_Valid2(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_SetChannelVolumeLevelScalar_Valid3(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_SetChannelVolumeLevelScalar_Valid4(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_SetChannelVolumeLevelScalar_Valid5(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_QueryHardwareSupport_Valid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_GetVolumeRange_Valid1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);

DWORD Test_EndpointVolume_GetMasterVol_Invalid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_GetMasterVol_Scalar_Invalid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_GetMute_Invalid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_VolumeStepping_Invalid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_GetChannelCount_Invalid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_SetChannelVolumeLevel_Invalid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_GetChannelVolumeLevel_Invalid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_SetChannelVolumeLevelScalar_Invalid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_GetChannelVolumeLevelScalar_Invalid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_QueryHardwareSupport_Invalid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_EndpointVolume_GetVolumeRange_Invalid1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine);
DWORD Test_EndpointVolume_DisconnectService_Invalid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);

// Per-Endpoint Eventing Scenarios
DWORD Test_PerEndpointEvents_Scenario1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_PerEndpointEvents_Scenario2(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_PerEndpointEvents_Scenario3(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_PerEndpointEvents_ScenarioIV1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);

// IAudioMeterInformation
DWORD Test_AudioMeterInf_Valid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_AudioMeterInf_Valid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);

DWORD Test_AudioMeterInf_Invalid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_AudioMeterInf_DisconnectService(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);

// AudioSession Management Scenarios
DWORD Test_SessionManagement_GetSessionEnum_Valid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_SessionManagement_GetSessionEnum_Valid2(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_SessionManagement_GetSessionEnum_Valid3(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_SessionManagement_AddSessionNotif_Valid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_SessionManagement_AddSessionNotif_Valid2(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_SessionManagement_AddSessionNotif_Valid3(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_SessionManagement_AddSessionNotif_Valid4(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_SessionManagement_DeleteSessionNotif_Valid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_SessionManagement_GetSession_Valid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_SessionManagement_GetSession_Valid2(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_SessionManagement_GetSession_Valid3(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_SessionManagement_GetSession_Valid4(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);

DWORD Test_SessionManagement_AddSessionNotif_Invalid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_SessionManagement_DeleteSessionNotif_Invalid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
DWORD Test_SessionManagement_DisconnectService(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine);
