// Copyright (C) Microsoft. All rights reserved.
//  General
DWORD WINAPI TC_General_VerifyInterfaceSupport();

//  APO
DWORD WINAPI TC_IAudioProcessingObject_GetLatency();
DWORD WINAPI TC_IAudioProcessingObject_GetRegistrationProperties();
DWORD WINAPI TC_IAudioProcessingObject_VerifyInputFormatSupport();
DWORD WINAPI TC_IAudioProcessingObject_VerifyOutputFormatSupport();
DWORD WINAPI TC_IAudioProcessingObject_VerifyProcess();
DWORD WINAPI TC_IAudioProcessingObject_VerifyNoSRC();
DWORD WINAPI TC_VerifyCustomFormats();
DWORD WINAPI TC_VerifyCustomFormatSupport();

//  SigVerif
DWORD WINAPI TC_VerifySignature();
DWORD WINAPI TC_DisplayFilelist();

//  Device Specific
DWORD WINAPI TC_VerifyMicArrayRaw();

//  External
DWORD WINAPI TC_External_NULL_DeviceCollection();
DWORD WINAPI TC_External_NULL_EndpointProperties();

//  AudioVolume
DWORD WINAPI TC_CAudioVolume_VerifyRegistration();
DWORD WINAPI TC_CAudioVolume_Initialize();
DWORD WINAPI TC_CAudioVolume_States();
DWORD WINAPI TC_IAudioVolume_SetVolumeCoefficients();
DWORD WINAPI TC_IAudioVolume_GetVolumeCoefficients();
DWORD WINAPI TC_IAudioVolume_CURVE_NONE();
DWORD WINAPI TC_IAudioVolume_CURVE_WINDOWS_FADE();
DWORD WINAPI TC_IAudioVolume_CURVE_HANN();
DWORD WINAPI TC_IAudioVolume_NoInitialRamp();
DWORD WINAPI TC_IAudioMuteAPO_SetMute();
DWORD WINAPI TC_IAudioMuteAPO_GetMute();
DWORD WINAPI TC_IAudioMuteAPO_CURVE_NONE();
DWORD WINAPI TC_IAudioMuteAPO_CURVE_WINDOWS_FADE();
DWORD WINAPI TC_IAudioMuteAPO_CURVE_HANN();
DWORD WINAPI TC_IAudioMuteAPO_NoInitialRamp();

//  AudioRateConvertCMPT
DWORD WINAPI TC_CAudioRateConvertCMPT_VerifyRegistration();
DWORD WINAPI TC_CAudioRateConvertCMPT_Initialize();
DWORD WINAPI TC_IAudioRateConvert_SetInputFrameRate();
DWORD WINAPI TC_IAudioRateConvert_SetOutputFrameRate();
DWORD WINAPI TC_IAudioRateConvertRT_GetInputFrameRate();
DWORD WINAPI TC_IAudioRateConvertRT_GetOutputFrameRateRT();
DWORD WINAPI TC_IAudioProcessingObjectVBR_CalcMaxInputFrames();
DWORD WINAPI TC_IAudioProcessingObjectVBR_CalcMaxOutputFrames();

DWORD WINAPI TC_CAudioRateConvertCMPT_Functional_NoChange_Linear();
DWORD WINAPI TC_CAudioRateConvertCMPT_Functional_Increase1Percent_Linear();
DWORD WINAPI TC_CAudioRateConvertCMPT_Functional_Increase20Percent_Linear();
DWORD WINAPI TC_CAudioRateConvertCMPT_Functional_Decrease1Percent_Linear();
DWORD WINAPI TC_CAudioRateConvertCMPT_Functional_Decrease20Percent_Linear();
DWORD WINAPI TC_CAudioRateConvertCMPT_Functional_NoChange_SINC3();
DWORD WINAPI TC_CAudioRateConvertCMPT_Functional_Increase1Percent_SINC3();
DWORD WINAPI TC_CAudioRateConvertCMPT_Functional_Increase20Percent_SINC3();
DWORD WINAPI TC_CAudioRateConvertCMPT_Functional_Decrease1Percent_SINC3();
DWORD WINAPI TC_CAudioRateConvertCMPT_Functional_Decrease20Percent_SINC3();