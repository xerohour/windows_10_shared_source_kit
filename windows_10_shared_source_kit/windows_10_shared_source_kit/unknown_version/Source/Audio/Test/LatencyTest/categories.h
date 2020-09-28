// categories.h
//
// Copyright (C) Microsoft. All rights reserved.

HRESULT MediaCategoryFromString(
    LPCWSTR s,
    ABI::Windows::Media::Capture::MediaCategory *pMedia
);

LPCWSTR StringFromMediaCategory(
    ABI::Windows::Media::Capture::MediaCategory media
);

HRESULT AudioStreamCategoryFromMediaCategory(
    ABI::Windows::Media::Capture::MediaCategory media,
    AUDIO_STREAM_CATEGORY *pStream
);

HRESULT AudioDeviceRoleFromMediaCategory(
    ABI::Windows::Media::Capture::MediaCategory media,
    ABI::Windows::Media::Devices::AudioDeviceRole *pRole
);

HRESULT AudioRenderCategoryFromString(
    LPCWSTR s,
    ABI::Windows::Media::Render::AudioRenderCategory *pRender
);

LPCWSTR StringFromAudioRenderCategory(
    ABI::Windows::Media::Render::AudioRenderCategory render
);

HRESULT AudioStreamCategoryFromAudioRenderCategory(
    ABI::Windows::Media::Render::AudioRenderCategory render,
    AUDIO_STREAM_CATEGORY *pStream
);

HRESULT AudioDeviceRoleFromAudioRenderCategory(
    ABI::Windows::Media::Render::AudioRenderCategory render,
    ABI::Windows::Media::Devices::AudioDeviceRole *pRole
);