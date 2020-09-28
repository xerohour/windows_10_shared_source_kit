// Copyright (C) Microsoft Corporation. All rights reserved.

#include "common.h"

#define CASE_RETURN(prefix, token) case prefix ## token: return L ## #token



LPCWSTR StringFromAudioEffectType( Windows::Media::Effects::AudioEffectType t ) {
    switch (t) {
        CASE_RETURN(Windows::Media::Effects::AudioEffectType::, Other);
        CASE_RETURN(Windows::Media::Effects::AudioEffectType::, AcousticEchoCancellation);
        CASE_RETURN(Windows::Media::Effects::AudioEffectType::, NoiseSuppression);
        CASE_RETURN(Windows::Media::Effects::AudioEffectType::, AutomaticGainControl);
        CASE_RETURN(Windows::Media::Effects::AudioEffectType::, BeamForming);
        CASE_RETURN(Windows::Media::Effects::AudioEffectType::, ConstantToneRemoval);
        CASE_RETURN(Windows::Media::Effects::AudioEffectType::, Equalizer);
        CASE_RETURN(Windows::Media::Effects::AudioEffectType::, LoudnessEqualizer);
        CASE_RETURN(Windows::Media::Effects::AudioEffectType::, BassBoost);
        CASE_RETURN(Windows::Media::Effects::AudioEffectType::, VirtualSurround);
        CASE_RETURN(Windows::Media::Effects::AudioEffectType::, VirtualHeadphones);
        CASE_RETURN(Windows::Media::Effects::AudioEffectType::, SpeakerFill);
        CASE_RETURN(Windows::Media::Effects::AudioEffectType::, RoomCorrection);
        CASE_RETURN(Windows::Media::Effects::AudioEffectType::, BassManagement);
        CASE_RETURN(Windows::Media::Effects::AudioEffectType::, EnvironmentalEffects);
        CASE_RETURN(Windows::Media::Effects::AudioEffectType::, SpeakerProtection);
        CASE_RETURN(Windows::Media::Effects::AudioEffectType::, SpeakerCompensation);
        CASE_RETURN(Windows::Media::Effects::AudioEffectType::, DynamicRangeCompression);
        default: return L"Unrecognized";
    }
}

Windows::Media::Effects::AudioEffectType AudioEffectTypeFromString( LPCWSTR t ) {
    if (_wcsicmp( t, L"Other") == 0)
		return Windows::Media::Effects::AudioEffectType::Other;		
    if (_wcsicmp( t, L"AcousticEchoCancellation") == 0)
		return Windows::Media::Effects::AudioEffectType::AcousticEchoCancellation;
			
	if (_wcsicmp( t, L"NoiseSuppression")  == 0)
		return Windows::Media::Effects::AudioEffectType::NoiseSuppression;
			
	if (_wcsicmp( t, L"AutomaticGainControl")  == 0)
		return Windows::Media::Effects::AudioEffectType::AutomaticGainControl;
			
	if (_wcsicmp( t, L"BeamForming")  == 0)
		return Windows::Media::Effects::AudioEffectType::BeamForming;
			
	if (_wcsicmp( t, L"ConstantToneRemoval")  == 0)
		return Windows::Media::Effects::AudioEffectType::ConstantToneRemoval;
			
	if (_wcsicmp( t, L"Equalizer")  == 0)
		return Windows::Media::Effects::AudioEffectType::Equalizer;
			
	if (_wcsicmp( t, L"LoudnessEqualizer")  == 0)
		return Windows::Media::Effects::AudioEffectType::LoudnessEqualizer;
			
	if (_wcsicmp( t, L"BassBoost")  == 0)
		return Windows::Media::Effects::AudioEffectType::BassBoost;
			
	if (_wcsicmp( t, L"VirtualSurround")  == 0)
		return Windows::Media::Effects::AudioEffectType::VirtualSurround;
			
	if (_wcsicmp( t, L"VirtualHeadphones")  == 0)
		return Windows::Media::Effects::AudioEffectType::VirtualHeadphones;
			
	if (_wcsicmp( t, L"SpeakerFill")  == 0)
		return Windows::Media::Effects::AudioEffectType::SpeakerFill;
			
	if (_wcsicmp( t, L"RoomCorrection")  == 0)
		return Windows::Media::Effects::AudioEffectType::RoomCorrection;
			
	if (_wcsicmp( t, L"BassManagement")  == 0)
		return Windows::Media::Effects::AudioEffectType::BassManagement;
			
	if (_wcsicmp( t, L"EnvironmentalEffects")  == 0)
		return Windows::Media::Effects::AudioEffectType::EnvironmentalEffects;
			
	if (_wcsicmp( t, L"SpeakerProtection")  == 0)
		return Windows::Media::Effects::AudioEffectType::SpeakerProtection;
			
	if (_wcsicmp( t, L"SpeakerCompensation")  == 0)
		return Windows::Media::Effects::AudioEffectType::SpeakerCompensation;
			
	if (_wcsicmp( t, L"DynamicRangeCompression")  == 0)
		return Windows::Media::Effects::AudioEffectType::DynamicRangeCompression;

    Log::Error(L"Unrecognized AudioEffectsType: %s", t);

	return Windows::Media::Effects::AudioEffectType::Other;

}   



Windows::Media::Render::AudioRenderCategory AudioRenderCategoryFromString( LPCWSTR t ) {
	if (_wcsicmp( t, L"other")  == 0)
		return Windows::Media::Render::AudioRenderCategory::Other;
	if (_wcsicmp( t, L"foregroundOnlyMedia")  == 0)
		return Windows::Media::Render::AudioRenderCategory::ForegroundOnlyMedia;
	if (_wcsicmp( t, L"backgroundCapableMedia")  == 0)
		return Windows::Media::Render::AudioRenderCategory::BackgroundCapableMedia;
	if (_wcsicmp( t, L"communications")  == 0)
		return Windows::Media::Render::AudioRenderCategory::Communications;
	if (_wcsicmp( t, L"alerts")  == 0)
		return Windows::Media::Render::AudioRenderCategory::Alerts;
	if (_wcsicmp( t, L"soundEffects")  == 0)
		return Windows::Media::Render::AudioRenderCategory::SoundEffects;
	if (_wcsicmp( t, L"gameEffects")  == 0)
		return Windows::Media::Render::AudioRenderCategory::GameEffects;
	if (_wcsicmp( t, L"gameMedia")  == 0)
		return Windows::Media::Render::AudioRenderCategory::GameMedia;

     Log::Error(L"Unrecognized AudioRenderCategory: %s", t);

	return Windows::Media::Render::AudioRenderCategory::Other;

}


LPCWSTR StringFromAudioRenderCategory( Windows::Media::Render::AudioRenderCategory c ) {
    switch (c) {
        CASE_RETURN(Windows::Media::Render::AudioRenderCategory::, Other);
        CASE_RETURN(Windows::Media::Render::AudioRenderCategory::, ForegroundOnlyMedia);
        CASE_RETURN(Windows::Media::Render::AudioRenderCategory::, BackgroundCapableMedia);
        CASE_RETURN(Windows::Media::Render::AudioRenderCategory::, Communications);
        CASE_RETURN(Windows::Media::Render::AudioRenderCategory::, Alerts);
        CASE_RETURN(Windows::Media::Render::AudioRenderCategory::, SoundEffects);
        CASE_RETURN(Windows::Media::Render::AudioRenderCategory::, GameEffects);
        CASE_RETURN(Windows::Media::Render::AudioRenderCategory::, GameMedia);
        default: return L"Unrecognized";
    }
}

LPCWSTR StringFromCaptureMediaCategory( Windows::Media::Capture::MediaCategory c ) {
    switch (c) {
        CASE_RETURN(Windows::Media::Capture::MediaCategory::, Other);
        CASE_RETURN(Windows::Media::Capture::MediaCategory::, Communications);
        default: return L"Unrecognized";
    }
}

Windows::Media::Capture::MediaCategory MediaCaptureCategoryFromString( LPCWSTR c ) {
	if ( _wcsicmp( c, L"Other" ) == 0)
		return Windows::Media::Capture::MediaCategory:: Other;
	if ( _wcsicmp( c, L"Communications" ) == 0)
		return Windows::Media::Capture::MediaCategory::Communications;
    
    Log::Error(L"Unrecognized MediaCaptureCategoryFromString: %s", c);
	return Windows::Media::Capture::MediaCategory:: Other; 
}

LPCWSTR StringFromAudioProcessing( Windows::Media::AudioProcessing ap ) {
    switch (ap) {
        CASE_RETURN(Windows::Media::AudioProcessing::, Default);
        CASE_RETURN(Windows::Media::AudioProcessing::, Raw);
        default: return L"Unrecognized";
    }
}

Windows::Media::AudioProcessing AudioProcessingFromString( LPCWSTR ap ) {
	if ( _wcsicmp( ap, L"default" ) == 0)
		return Windows::Media::AudioProcessing::Default;
	if ( _wcsicmp( ap, L"raw" ) == 0)
		return Windows::Media::AudioProcessing::Raw;

    Log::Error(L"Unrecognized AudioProcessing: %s", ap);
	return Windows::Media::AudioProcessing::Default;
}

LPCWSTR StringFromDeviceWatcherStatus( Windows::Devices::Enumeration::DeviceWatcherStatus s ) {
    switch (s) {
        CASE_RETURN(Windows::Devices::Enumeration::DeviceWatcherStatus::, Created);
        CASE_RETURN(Windows::Devices::Enumeration::DeviceWatcherStatus::, Started);
        CASE_RETURN(Windows::Devices::Enumeration::DeviceWatcherStatus::, EnumerationCompleted);
        CASE_RETURN(Windows::Devices::Enumeration::DeviceWatcherStatus::, Stopping);
        CASE_RETURN(Windows::Devices::Enumeration::DeviceWatcherStatus::, Stopped);
        CASE_RETURN(Windows::Devices::Enumeration::DeviceWatcherStatus::, Aborted);
        default: return L"Unrecognized";
    }
}