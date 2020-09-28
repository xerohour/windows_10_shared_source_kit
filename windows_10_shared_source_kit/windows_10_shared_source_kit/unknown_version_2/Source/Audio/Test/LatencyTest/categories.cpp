// categories.cpp
//
// Copyright (C) Microsoft. All rights reserved.

#include "pch.h"

// Build a table of media categories
// and a table of audio render categories
//
// Each entry has:
// * A short name "GameChat"
// * A long name "MediaCategory_GameChat"
// * An enum value MediaCategory_GameChat
// * An associated stream category AudioCategory_GameChat
// * An associated default device role AudioDeviceRole_Communications
//
// Once we have the table built, all of the conversion functions just become lookups
// MediaCategoryFromString(...) keys on either the short or long name and returns the enum value
// StringFromMediaCategory(...) keys on the enum value and returns the long name
// AudioStreamCategoryFromMediaCategory(...) keys on the enum value and returns the stream category
// AudioDeviceRoleFromMediaCategory(...) keys on the enum value and returns the default device role

struct MediaCategoryEntry {
    LPCWSTR shortName;
    LPCWSTR longName;
    ABI::Windows::Media::Capture::MediaCategory value;
    AUDIO_STREAM_CATEGORY stream;
    ABI::Windows::Media::Devices::AudioDeviceRole role;

    MediaCategoryEntry(
        LPCWSTR _shortName,
        LPCWSTR _longName,
        ABI::Windows::Media::Capture::MediaCategory _value,
        AUDIO_STREAM_CATEGORY _stream
    ) :
        shortName(_shortName),
        longName(_longName),
        value(_value),
        stream(_stream),
        role(AudioDeviceRole(_value))
    {}

private:
    static ABI::Windows::Media::Devices::AudioDeviceRole AudioDeviceRole(ABI::Windows::Media::Capture::MediaCategory c) {
        switch (c) {
            case ABI::Windows::Media::Capture::MediaCategory::MediaCategory_Communications:
#if NTDDI_VERSION >= NTDDI_WINTHRESHOLD
            case ABI::Windows::Media::Capture::MediaCategory::MediaCategory_GameChat:
#endif
                return ABI::Windows::Media::Devices::AudioDeviceRole::AudioDeviceRole_Communications;
            default:
                return ABI::Windows::Media::Devices::AudioDeviceRole::AudioDeviceRole_Default;
        }
    }
};

#define MEDIACATEGORYENTRY(s) \
    MediaCategoryEntry( \
        L ## #s, \
        L"MediaCategory_" L ## #s, \
        ABI::Windows::Media::Capture::MediaCategory::MediaCategory_ ## s, \
        AudioCategory_ ## s \
    )

// This is the MediaCategory table
MediaCategoryEntry g_MediaCategories[] = {
    MEDIACATEGORYENTRY(Other),
    MEDIACATEGORYENTRY(Communications),
#if NTDDI_VERSION >= NTDDI_WINTHRESHOLD
    MEDIACATEGORYENTRY(Media),
    MEDIACATEGORYENTRY(GameChat),
    MEDIACATEGORYENTRY(Speech),
#endif
};

struct AudioRenderCategoryEntry {
    LPCWSTR shortName;
    LPCWSTR longName;
    ABI::Windows::Media::Render::AudioRenderCategory value;
    AUDIO_STREAM_CATEGORY stream;
    ABI::Windows::Media::Devices::AudioDeviceRole role;

    AudioRenderCategoryEntry(
        LPCWSTR _shortName,
        LPCWSTR _longName,
        ABI::Windows::Media::Render::AudioRenderCategory _value,
        AUDIO_STREAM_CATEGORY _stream
    ) :
        shortName(_shortName),
        longName(_longName),
        value(_value),
        stream(_stream),
        role(AudioDeviceRole(_value))
    {}

private:
    static ABI::Windows::Media::Devices::AudioDeviceRole AudioDeviceRole(ABI::Windows::Media::Render::AudioRenderCategory c) {
        switch (c) {
            case ABI::Windows::Media::Render::AudioRenderCategory_Communications:
#if NTDDI_VERSION >= NTDDI_WINTHRESHOLD
          case ABI::Windows::Media::Render::AudioRenderCategory_GameChat:
#endif
              return ABI::Windows::Media::Devices::AudioDeviceRole::AudioDeviceRole_Communications;
          default:
              return ABI::Windows::Media::Devices::AudioDeviceRole::AudioDeviceRole_Default;
        }
    }
};

#define AUDIORENDERCATEGORYENTRY(s) \
    AudioRenderCategoryEntry( \
        L ## #s, \
        L"AudioRenderCategory_" L ## #s, \
        ABI::Windows::Media::Render::AudioRenderCategory::AudioRenderCategory_ ## s, \
        AudioCategory_ ## s \
    )

// This is the AudioRenderCategory table
AudioRenderCategoryEntry g_AudioRenderCategories[] = {
    AUDIORENDERCATEGORYENTRY(Other),
#if NTDDI_VERSION < NTDDI_WINTHRESHOLD
    AUDIORENDERCATEGORYENTRY(ForegroundOnlyMedia),
    AUDIORENDERCATEGORYENTRY(BackgroundCapableMedia),
#endif
    AUDIORENDERCATEGORYENTRY(Communications),
    AUDIORENDERCATEGORYENTRY(Alerts),
    AUDIORENDERCATEGORYENTRY(SoundEffects),
    AUDIORENDERCATEGORYENTRY(GameEffects),
    AUDIORENDERCATEGORYENTRY(GameMedia),
#if NTDDI_VERSION >= NTDDI_WINTHRESHOLD
    AUDIORENDERCATEGORYENTRY(GameChat),
    AUDIORENDERCATEGORYENTRY(Speech),
    AUDIORENDERCATEGORYENTRY(Movie),
    AUDIORENDERCATEGORYENTRY(Media),    
#endif
};

HRESULT MediaCategoryFromString(
    LPCWSTR s,
    ABI::Windows::Media::Capture::MediaCategory *pMedia
) {
    for (int i = 0; i < ARRAYSIZE(g_MediaCategories); i++) {
        if (0 == _wcsicmp(s, g_MediaCategories[i].shortName)) {
            *pMedia = g_MediaCategories[i].value;
            return S_OK;
        }

        if (0 == _wcsicmp(s, g_MediaCategories[i].longName)) {
            *pMedia = g_MediaCategories[i].value;
            return S_OK;
        }
    }

    ERR(L"Unrecognized media category: %s", s);
    return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
}

LPCWSTR StringFromMediaCategory(
    ABI::Windows::Media::Capture::MediaCategory media
) {
    for (int i = 0; i < ARRAYSIZE(g_MediaCategories); i++) {
        if (media == g_MediaCategories[i].value) {
            return g_MediaCategories[i].longName;
        }
    }

    return L"(Unrecognized media category)";
}

HRESULT AudioRenderCategoryFromString(
    LPCWSTR s,
    ABI::Windows::Media::Render::AudioRenderCategory *pRender
) {
    for (int i = 0; i < ARRAYSIZE(g_AudioRenderCategories); i++) {
        if (0 == _wcsicmp(s, g_AudioRenderCategories[i].shortName)) {
            *pRender = g_AudioRenderCategories[i].value;
            return S_OK;
        }

        if (0 == _wcsicmp(s, g_AudioRenderCategories[i].longName)) {
            *pRender = g_AudioRenderCategories[i].value;
            return S_OK;
        }
    }

    ERR(L"Unrecognized audio render category: %s", s);
    return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
}

LPCWSTR StringFromAudioRenderCategory(
    ABI::Windows::Media::Render::AudioRenderCategory render
) {
    for (int i = 0; i < ARRAYSIZE(g_AudioRenderCategories); i++) {
        if (render == g_AudioRenderCategories[i].value) {
            return g_AudioRenderCategories[i].longName;
        }
    }

    return L"(Unrecognized audio render category)";
}

HRESULT AudioStreamCategoryFromMediaCategory(
    ABI::Windows::Media::Capture::MediaCategory media,
    AUDIO_STREAM_CATEGORY *pStream
) {
    for (int i = 0; i < ARRAYSIZE(g_MediaCategories); i++) {
        if (media == g_MediaCategories[i].value) {
            *pStream = g_MediaCategories[i].stream;
            return S_OK;
        }
    }

    ERR(L"Unrecognized media category: %d", media);
    return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
}

HRESULT AudioStreamCategoryFromAudioRenderCategory(
    ABI::Windows::Media::Render::AudioRenderCategory render,
    AUDIO_STREAM_CATEGORY *pStream
) {
    for (int i = 0; i < ARRAYSIZE(g_AudioRenderCategories); i++) {
        if (render == g_AudioRenderCategories[i].value) {
            *pStream = g_AudioRenderCategories[i].stream;
            return S_OK;
        }
    }

    ERR(L"Unrecognized audio render category: %d", render);
    return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
}

HRESULT AudioDeviceRoleFromMediaCategory(
    ABI::Windows::Media::Capture::MediaCategory media,
    ABI::Windows::Media::Devices::AudioDeviceRole *pRole
) {
    for (int i = 0; i < ARRAYSIZE(g_MediaCategories); i++) {
        if (media == g_MediaCategories[i].value) {
            *pRole = g_MediaCategories[i].role;
            return S_OK;
        }
    }

    ERR(L"Unrecognized media category: %d", media);
    return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
}

HRESULT AudioDeviceRoleFromAudioRenderCategory(
    ABI::Windows::Media::Render::AudioRenderCategory render,
    ABI::Windows::Media::Devices::AudioDeviceRole *pRole
) {
    for (int i = 0; i < ARRAYSIZE(g_AudioRenderCategories); i++) {
        if (render == g_AudioRenderCategories[i].value) {
            *pRole = g_AudioRenderCategories[i].role;
            return S_OK;
        }
    }

    ERR(L"Unrecognized audio render category: %d", render);
    return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);

}