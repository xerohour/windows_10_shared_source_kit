// Copyright(C) Microsoft. All rights reserved.

#include <AudioPolicy.h>
#include <AudioEngineEndpoint.h>
#include <AudioEngineEndpointP.h>
int BldCheck()
{
    //  AudioSessionDisconnectReason
    C_ASSERT( 0 == DisconnectReasonDeviceRemoval);
    C_ASSERT( 1 == DisconnectReasonServerShutdown);
    C_ASSERT( 2 == DisconnectReasonFormatChanged);
    C_ASSERT( 3 == DisconnectReasonSessionLogoff);
    C_ASSERT( 4 == DisconnectReasonSessionDisconnected);
    C_ASSERT( 5 == DisconnectReasonExclusiveModeOverride);

    C_ASSERT( 1 == AUDIOCLOCK_CHARACTERISTIC_FIXED_FREQ);

    //  AE_POSITION_FLAGS
    C_ASSERT( 0 == POSITION_INVALID);
    C_ASSERT( 1 == POSITION_DISCONTINUOUS);
    C_ASSERT( 2 == POSITION_CONTINUOUS);
    C_ASSERT( 4 == POSITION_QPC_ERROR);
    
    //  AUDCLNT_X_ codes...
    C_ASSERT(0x88890001 == (DWORD)(AUDCLNT_E_NOT_INITIALIZED));
    C_ASSERT(0x88890002 == (DWORD)(AUDCLNT_E_ALREADY_INITIALIZED));
    C_ASSERT(0x88890003 == (DWORD)(AUDCLNT_E_WRONG_ENDPOINT_TYPE));
    C_ASSERT(0x88890004 == (DWORD)(AUDCLNT_E_DEVICE_INVALIDATED));
    C_ASSERT(0x88890005 == (DWORD)(AUDCLNT_E_NOT_STOPPED));
    C_ASSERT(0x88890006 == (DWORD)(AUDCLNT_E_BUFFER_TOO_LARGE));
    C_ASSERT(0x88890007 == (DWORD)(AUDCLNT_E_OUT_OF_ORDER));
    C_ASSERT(0x88890008 == (DWORD)(AUDCLNT_E_UNSUPPORTED_FORMAT));
    C_ASSERT(0x88890009 == (DWORD)(AUDCLNT_E_INVALID_SIZE));
    C_ASSERT(0x8889000a == (DWORD)(AUDCLNT_E_DEVICE_IN_USE));
    C_ASSERT(0x8889000b == (DWORD)(AUDCLNT_E_BUFFER_OPERATION_PENDING));
    C_ASSERT(0x8889000c == (DWORD)(AUDCLNT_E_THREAD_NOT_REGISTERED));
    C_ASSERT(0x8889000e == (DWORD)(AUDCLNT_E_EXCLUSIVE_MODE_NOT_ALLOWED));
    C_ASSERT(0x8889000f == (DWORD)(AUDCLNT_E_ENDPOINT_CREATE_FAILED));
    C_ASSERT(0x88890010 == (DWORD)(AUDCLNT_E_SERVICE_NOT_RUNNING));
    C_ASSERT(0x88890011 == (DWORD)(AUDCLNT_E_EVENTHANDLE_NOT_EXPECTED));
    C_ASSERT(0x88890012 == (DWORD)(AUDCLNT_E_EXCLUSIVE_MODE_ONLY));
    C_ASSERT(0x88890013 == (DWORD)(AUDCLNT_E_BUFDURATION_PERIOD_NOT_EQUAL));
    C_ASSERT(0x88890014 == (DWORD)(AUDCLNT_E_EVENTHANDLE_NOT_SET));
    C_ASSERT(0x88890015 == (DWORD)(AUDCLNT_E_INCORRECT_BUFFER_SIZE));
    C_ASSERT(0x88890016 == (DWORD)(AUDCLNT_E_BUFFER_SIZE_ERROR));
    C_ASSERT(0x88890017 == (DWORD)(AUDCLNT_E_CPUUSAGE_EXCEEDED));
    C_ASSERT(0x08890001 == (DWORD)(AUDCLNT_S_BUFFER_EMPTY));
    C_ASSERT(0x08890002 == (DWORD)(AUDCLNT_S_THREAD_ALREADY_REGISTERED));
    C_ASSERT(0x08890003 == (DWORD)(AUDCLNT_S_POSITION_STALLED));
    
    //  _AUDCLNT_BUFFERFLAGS
    C_ASSERT(0x1 == AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY);
    C_ASSERT(0x2 == AUDCLNT_BUFFERFLAGS_SILENT);
    C_ASSERT(0x4 == AUDCLNT_BUFFERFLAGS_TIMESTAMP_ERROR);
    
    //  AUDIO_ENDPOINT_EXCLUSIVE_CREATE_PARAMS
    C_ASSERT(88 == sizeof(AUDIO_ENDPOINT_EXCLUSIVE_CREATE_PARAMS));
    C_ASSERT( 0 == (DWORD_PTR)&(((PAUDIO_ENDPOINT_EXCLUSIVE_CREATE_PARAMS)(NULL))->u32Size));
    C_ASSERT( 8 == (DWORD_PTR)&(((PAUDIO_ENDPOINT_EXCLUSIVE_CREATE_PARAMS)(NULL))->hConnection));
    C_ASSERT(16 == (DWORD_PTR)&(((PAUDIO_ENDPOINT_EXCLUSIVE_CREATE_PARAMS)(NULL))->bIsRtCapable));
    C_ASSERT(24 == (DWORD_PTR)&(((PAUDIO_ENDPOINT_EXCLUSIVE_CREATE_PARAMS)(NULL))->hnsBufferDuration));
    C_ASSERT(32 == (DWORD_PTR)&(((PAUDIO_ENDPOINT_EXCLUSIVE_CREATE_PARAMS)(NULL))->hnsPeriod));
    C_ASSERT(40 == (DWORD_PTR)&(((PAUDIO_ENDPOINT_EXCLUSIVE_CREATE_PARAMS)(NULL))->u32LatencyCoefficient));
    C_ASSERT(44 == (DWORD_PTR)&(((PAUDIO_ENDPOINT_EXCLUSIVE_CREATE_PARAMS)(NULL))->u32HwAudEngineId));
    C_ASSERT(48 == (DWORD_PTR)&(((PAUDIO_ENDPOINT_EXCLUSIVE_CREATE_PARAMS)(NULL))->graphId));
    C_ASSERT(64 == (DWORD_PTR)&(((PAUDIO_ENDPOINT_EXCLUSIVE_CREATE_PARAMS)(NULL))->wfxDeviceFormat));

    //  AUDIO_ENDPOINT_SHARED_CREATE_PARAMS
    C_ASSERT(32 == sizeof(AUDIO_ENDPOINT_SHARED_CREATE_PARAMS));
    C_ASSERT( 0 == (DWORD_PTR)&(((PAUDIO_ENDPOINT_SHARED_CREATE_PARAMS)(NULL))->u32Size));
    C_ASSERT( 4 == (DWORD_PTR)&(((PAUDIO_ENDPOINT_SHARED_CREATE_PARAMS)(NULL))->u32TSSessionId));
    C_ASSERT( 8 == (DWORD_PTR)&(((PAUDIO_ENDPOINT_SHARED_CREATE_PARAMS)(NULL))->targetEndpointConnectorType));
    C_ASSERT( 12 == (DWORD_PTR)&(((PAUDIO_ENDPOINT_SHARED_CREATE_PARAMS)(NULL))->wfxDeviceFormat));

    //  AE_CURRENT_POSITION
    C_ASSERT(40 == sizeof(AE_CURRENT_POSITION));
    C_ASSERT( 0 == (DWORD_PTR)&(((PAE_CURRENT_POSITION)(NULL))->u64DevicePosition));
    C_ASSERT( 8 == (DWORD_PTR)&(((PAE_CURRENT_POSITION)(NULL))->u64StreamPosition));
    C_ASSERT(16 == (DWORD_PTR)&(((PAE_CURRENT_POSITION)(NULL))->u64PaddingFrames));
    C_ASSERT(24 == (DWORD_PTR)&(((PAE_CURRENT_POSITION)(NULL))->hnsQPCPosition));
    C_ASSERT(32 == (DWORD_PTR)&(((PAE_CURRENT_POSITION)(NULL))->f32FramesPerSecond));
    C_ASSERT(36 == (DWORD_PTR)&(((PAE_CURRENT_POSITION)(NULL))->Flag));

    //  IAudioSessionEvents
    C_ASSERT((10 * sizeof(LPVOID)) == sizeof(IAudioSessionEventsVtbl));
    C_ASSERT(( 3 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionEventsVtbl*)(NULL))->OnDisplayNameChanged));
    C_ASSERT(( 4 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionEventsVtbl*)(NULL))->OnIconPathChanged));
    C_ASSERT(( 5 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionEventsVtbl*)(NULL))->OnSimpleVolumeChanged));
    C_ASSERT(( 6 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionEventsVtbl*)(NULL))->OnChannelVolumeChanged));
    C_ASSERT(( 7 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionEventsVtbl*)(NULL))->OnGroupingParamChanged));
    C_ASSERT(( 8 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionEventsVtbl*)(NULL))->OnStateChanged));
    C_ASSERT(( 9 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionEventsVtbl*)(NULL))->OnSessionDisconnected));
    
    //  IAudioSessionControl
    C_ASSERT((12 * sizeof(LPVOID)) == sizeof(IAudioSessionControlVtbl));
    C_ASSERT(( 3 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionControlVtbl*)(NULL))->GetState));
    C_ASSERT(( 4 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionControlVtbl*)(NULL))->GetDisplayName));
    C_ASSERT(( 5 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionControlVtbl*)(NULL))->SetDisplayName));
    C_ASSERT(( 6 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionControlVtbl*)(NULL))->GetIconPath));
    C_ASSERT(( 7 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionControlVtbl*)(NULL))->SetIconPath));
    C_ASSERT(( 8 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionControlVtbl*)(NULL))->GetGroupingParam));
    C_ASSERT(( 9 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionControlVtbl*)(NULL))->SetGroupingParam));
    C_ASSERT((10 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionControlVtbl*)(NULL))->RegisterAudioSessionNotification));
    C_ASSERT((11 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionControlVtbl*)(NULL))->UnregisterAudioSessionNotification));

    //  IAudioSessionControl2
    C_ASSERT((17 * sizeof(LPVOID)) == sizeof(IAudioSessionControl2Vtbl));
    C_ASSERT(( 3 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionControl2Vtbl*)(NULL))->GetState));
    C_ASSERT(( 4 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionControl2Vtbl*)(NULL))->GetDisplayName));
    C_ASSERT(( 5 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionControl2Vtbl*)(NULL))->SetDisplayName));
    C_ASSERT(( 6 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionControl2Vtbl*)(NULL))->GetIconPath));
    C_ASSERT(( 7 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionControl2Vtbl*)(NULL))->SetIconPath));
    C_ASSERT(( 8 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionControl2Vtbl*)(NULL))->GetGroupingParam));
    C_ASSERT(( 9 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionControl2Vtbl*)(NULL))->SetGroupingParam));
    C_ASSERT((10 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionControl2Vtbl*)(NULL))->RegisterAudioSessionNotification));
    C_ASSERT((11 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionControl2Vtbl*)(NULL))->UnregisterAudioSessionNotification));
    C_ASSERT((12 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionControl2Vtbl*)(NULL))->GetSessionIdentifier));
    C_ASSERT((13 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionControl2Vtbl*)(NULL))->GetSessionInstanceIdentifier));
    C_ASSERT((14 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionControl2Vtbl*)(NULL))->GetProcessId));
    C_ASSERT((15 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionControl2Vtbl*)(NULL))->IsSystemSoundsSession));
    C_ASSERT((16 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionControl2Vtbl*)(NULL))->SetDuckingPreference));

    //  IAudioSessionManager
    C_ASSERT(( 5 * sizeof(LPVOID)) == sizeof(IAudioSessionManagerVtbl));
    C_ASSERT(( 3 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionManagerVtbl*)(NULL))->GetAudioSessionControl));
    C_ASSERT(( 4 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionManagerVtbl*)(NULL))->GetSimpleAudioVolume));
    
    //  IAudioSessionManager2
    C_ASSERT((10 * sizeof(LPVOID)) == sizeof(IAudioSessionManager2Vtbl));
    C_ASSERT(( 5 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionManager2Vtbl*)(NULL))->GetSessionEnumerator));
    C_ASSERT(( 6 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionManager2Vtbl*)(NULL))->RegisterSessionNotification));
    C_ASSERT(( 7 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionManager2Vtbl*)(NULL))->UnregisterSessionNotification));
    C_ASSERT(( 8 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionManager2Vtbl*)(NULL))->RegisterDuckNotification));
    C_ASSERT(( 9 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionManager2Vtbl*)(NULL))->UnregisterDuckNotification));
    
    //  IAudioVolumeDuckNotification
    C_ASSERT(( 5 * sizeof(LPVOID)) == sizeof(IAudioVolumeDuckNotificationVtbl));
    C_ASSERT(( 3 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioVolumeDuckNotificationVtbl*)(NULL))->OnVolumeDuckNotification));
    C_ASSERT(( 4 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioVolumeDuckNotificationVtbl*)(NULL))->OnVolumeUnduckNotification));

    //  IAudioSessionNotification
    C_ASSERT(( 4 * sizeof(LPVOID)) == sizeof(IAudioSessionNotificationVtbl));
    C_ASSERT(( 3 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionNotificationVtbl*)(NULL))->OnSessionCreated));
    
    //  IAudioSessionEnumerator
    C_ASSERT(( 5 * sizeof(LPVOID)) == sizeof(IAudioSessionEnumeratorVtbl));
    C_ASSERT(( 3 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionEnumeratorVtbl*)(NULL))->GetCount));
    C_ASSERT(( 4 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioSessionEnumeratorVtbl*)(NULL))->GetSession));
    
    //  IAudioDeviceEndpoint
    C_ASSERT(( 7 * sizeof(LPVOID)) == sizeof(IAudioDeviceEndpointVtbl));
    C_ASSERT(( 3 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioDeviceEndpointVtbl*)(NULL))->SetBuffer));
    C_ASSERT(( 4 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioDeviceEndpointVtbl*)(NULL))->GetRTCaps));
    C_ASSERT(( 5 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioDeviceEndpointVtbl*)(NULL))->GetEventDrivenCapable));
    C_ASSERT(( 6 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioDeviceEndpointVtbl*)(NULL))->WriteExclusiveModeParametersToSharedMemory));
    
    //  IAudioEndpoint
    C_ASSERT(( 8 * sizeof(LPVOID)) == sizeof(IAudioEndpointVtbl));
    C_ASSERT(( 3 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioEndpointVtbl*)(NULL))->GetFrameFormat));
    C_ASSERT(( 4 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioEndpointVtbl*)(NULL))->GetFramesPerPacket));
    C_ASSERT(( 5 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioEndpointVtbl*)(NULL))->GetLatency));
    C_ASSERT(( 6 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioEndpointVtbl*)(NULL))->SetStreamFlags));
    C_ASSERT(( 7 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioEndpointVtbl*)(NULL))->SetEventHandle));
    
    //  IAudioEndpointControl
    C_ASSERT(( 6 * sizeof(LPVOID)) == sizeof(IAudioEndpointControlVtbl));
    C_ASSERT(( 3 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioEndpointControlVtbl*)(NULL))->Start));
    C_ASSERT(( 4 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioEndpointControlVtbl*)(NULL))->Reset));
    C_ASSERT(( 5 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioEndpointControlVtbl*)(NULL))->Stop));
    
    //  IAudioEndpointRT
    C_ASSERT(( 7 * sizeof(LPVOID)) == sizeof(IAudioEndpointRTVtbl));
    C_ASSERT(( 3 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioEndpointRTVtbl*)(NULL))->GetCurrentPadding));
    C_ASSERT(( 4 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioEndpointRTVtbl*)(NULL))->ProcessingComplete));
    C_ASSERT(( 5 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioEndpointRTVtbl*)(NULL))->SetPinInactive));
    C_ASSERT(( 6 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioEndpointRTVtbl*)(NULL))->SetPinActive));
    
    //  IAudioInputEndpointRT
    C_ASSERT(( 6 * sizeof(LPVOID)) == sizeof(IAudioInputEndpointRTVtbl));
    C_ASSERT(( 3 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioInputEndpointRTVtbl*)(NULL))->GetInputDataPointer));
    C_ASSERT(( 4 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioInputEndpointRTVtbl*)(NULL))->ReleaseInputDataPointer));
    C_ASSERT(( 5 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioInputEndpointRTVtbl*)(NULL))->PulseEndpoint));
    
    //  IAudioOutputEndpointRT
    C_ASSERT(( 6 * sizeof(LPVOID)) == sizeof(IAudioOutputEndpointRTVtbl));
    C_ASSERT(( 3 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioOutputEndpointRTVtbl*)(NULL))->GetOutputDataPointer));
    C_ASSERT(( 4 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioOutputEndpointRTVtbl*)(NULL))->ReleaseOutputDataPointer));
    C_ASSERT(( 5 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioOutputEndpointRTVtbl*)(NULL))->PulseEndpoint));

    //  IAudioClient
    C_ASSERT((15 * sizeof(LPVOID)) == sizeof(IAudioClientVtbl));
    C_ASSERT(( 3 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioClientVtbl*)(NULL))->Initialize));
    C_ASSERT(( 4 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioClientVtbl*)(NULL))->GetBufferSize));
    C_ASSERT(( 5 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioClientVtbl*)(NULL))->GetStreamLatency));
    C_ASSERT(( 6 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioClientVtbl*)(NULL))->GetCurrentPadding));
    C_ASSERT(( 7 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioClientVtbl*)(NULL))->IsFormatSupported));
    C_ASSERT(( 8 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioClientVtbl*)(NULL))->GetMixFormat));
    C_ASSERT(( 9 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioClientVtbl*)(NULL))->GetDevicePeriod));
    C_ASSERT((10 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioClientVtbl*)(NULL))->Start));
    C_ASSERT((11 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioClientVtbl*)(NULL))->Stop));
    C_ASSERT((12 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioClientVtbl*)(NULL))->Reset));
    C_ASSERT((13 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioClientVtbl*)(NULL))->SetEventHandle));
    C_ASSERT((14 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioClientVtbl*)(NULL))->GetService));

    //  IAudioRenderClient
    C_ASSERT(( 5 * sizeof(LPVOID)) == sizeof(IAudioRenderClientVtbl));
    C_ASSERT(( 3 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioRenderClientVtbl*)(NULL))->GetBuffer));
    C_ASSERT(( 4 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioRenderClientVtbl*)(NULL))->ReleaseBuffer));
    
    //  IAudioCaptureClient
    C_ASSERT(( 6 * sizeof(LPVOID)) == sizeof(IAudioCaptureClientVtbl));
    C_ASSERT(( 3 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioCaptureClientVtbl*)(NULL))->GetBuffer));
    C_ASSERT(( 4 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioCaptureClientVtbl*)(NULL))->ReleaseBuffer));
    C_ASSERT(( 5 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioCaptureClientVtbl*)(NULL))->GetNextPacketSize));
    
    //  IAudioClock
    C_ASSERT(( 6 * sizeof(LPVOID)) == sizeof(IAudioClockVtbl));
    C_ASSERT(( 3 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioClockVtbl*)(NULL))->GetFrequency));
    C_ASSERT(( 4 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioClockVtbl*)(NULL))->GetPosition));
    C_ASSERT(( 5 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioClockVtbl*)(NULL))->GetCharacteristics));

    //  IAudioClockAdjustment
    C_ASSERT(( 4 * sizeof(LPVOID)) == sizeof(IAudioClockAdjustmentVtbl));
    C_ASSERT(( 3 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioClockAdjustmentVtbl*)(NULL))->SetSampleRate));
    
    //  ISimpleAudioVolume
    C_ASSERT(( 7 * sizeof(LPVOID)) == sizeof(ISimpleAudioVolumeVtbl));
    C_ASSERT(( 3 * sizeof(LPVOID)) == (DWORD_PTR)&(((ISimpleAudioVolumeVtbl*)(NULL))->SetMasterVolume));
    C_ASSERT(( 4 * sizeof(LPVOID)) == (DWORD_PTR)&(((ISimpleAudioVolumeVtbl*)(NULL))->GetMasterVolume));
    C_ASSERT(( 5 * sizeof(LPVOID)) == (DWORD_PTR)&(((ISimpleAudioVolumeVtbl*)(NULL))->SetMute));
    C_ASSERT(( 6 * sizeof(LPVOID)) == (DWORD_PTR)&(((ISimpleAudioVolumeVtbl*)(NULL))->GetMute));

    //  IAudioStreamVolume
    C_ASSERT(( 8 * sizeof(LPVOID)) == sizeof(IAudioStreamVolumeVtbl));
    C_ASSERT(( 3 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioStreamVolumeVtbl*)(NULL))->GetChannelCount));
    C_ASSERT(( 4 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioStreamVolumeVtbl*)(NULL))->SetChannelVolume));
    C_ASSERT(( 5 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioStreamVolumeVtbl*)(NULL))->GetChannelVolume));
    C_ASSERT(( 6 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioStreamVolumeVtbl*)(NULL))->SetAllVolumes));
    C_ASSERT(( 7 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioStreamVolumeVtbl*)(NULL))->GetAllVolumes));
    
    //  IChannelAudioVolume
    C_ASSERT(( 8 * sizeof(LPVOID)) == sizeof(IChannelAudioVolumeVtbl));
    C_ASSERT(( 3 * sizeof(LPVOID)) == (DWORD_PTR)&(((IChannelAudioVolumeVtbl*)(NULL))->GetChannelCount));
    C_ASSERT(( 4 * sizeof(LPVOID)) == (DWORD_PTR)&(((IChannelAudioVolumeVtbl*)(NULL))->SetChannelVolume));
    C_ASSERT(( 5 * sizeof(LPVOID)) == (DWORD_PTR)&(((IChannelAudioVolumeVtbl*)(NULL))->GetChannelVolume));
    C_ASSERT(( 6 * sizeof(LPVOID)) == (DWORD_PTR)&(((IChannelAudioVolumeVtbl*)(NULL))->SetAllVolumes));
    C_ASSERT(( 7 * sizeof(LPVOID)) == (DWORD_PTR)&(((IChannelAudioVolumeVtbl*)(NULL))->GetAllVolumes));
    
    return (1);
}

#include <AudioClientP.h>

int BldCheckP()
{
    //  IAudioClientTrustedOutputPriv
    C_ASSERT(( 4 * sizeof(LPVOID)) == sizeof(IAudioClientTrustedOutputPrivVtbl));
    C_ASSERT(( 3 * sizeof(LPVOID)) == (DWORD_PTR)&(((IAudioClientTrustedOutputPrivVtbl*)(NULL))->PolicyChanged));

    return (1);
}
