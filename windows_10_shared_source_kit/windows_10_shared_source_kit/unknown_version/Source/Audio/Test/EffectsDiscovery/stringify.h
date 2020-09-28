// Copyright (C) Microsoft Corporation. All rights reserved.


LPCWSTR StringFromAudioEffectType( Windows::Media::Effects::AudioEffectType t );
LPCWSTR StringFromAudioRenderCategory( Windows::Media::Render::AudioRenderCategory c );
LPCWSTR StringFromCaptureMediaCategory( Windows::Media::Capture::MediaCategory c );
LPCWSTR StringFromAudioProcessing( Windows::Media::AudioProcessing ap );
LPCWSTR StringFromDeviceWatcherStatus( Windows::Devices::Enumeration::DeviceWatcherStatus s );
Windows::Media::Effects::AudioEffectType AudioEffectTypeFromString( LPCWSTR t );
Windows::Media::AudioProcessing AudioProcessingFromString( LPCWSTR ap );
Windows::Media::Capture::MediaCategory MediaCaptureCategoryFromString( LPCWSTR c ) ;
Windows::Media::Render::AudioRenderCategory AudioRenderCategoryFromString( LPCWSTR t );

