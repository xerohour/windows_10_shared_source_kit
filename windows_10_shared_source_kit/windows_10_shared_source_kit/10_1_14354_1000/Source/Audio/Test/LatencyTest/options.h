// options.h
//
// Copyright (C) Microsoft. All rights reserved.

struct RoundtripCommonOptions {
    bool pcm;
    ABI::Windows::Media::AudioProcessing processing;
#if NTDDI_VERSION >= NTDDI_WINTHRESHOLD
    int frames;
#endif

    // defaults
    RoundtripCommonOptions()
        : pcm(false)
        , processing(ABI::Windows::Media::AudioProcessing::AudioProcessing_Default)
#if NTDDI_VERSION >= NTDDI_WINTHRESHOLD
        , frames(-1)
#endif
    {}
};

struct RoundtripRenderOptions : RoundtripCommonOptions {
    ABI::Windows::Media::Render::AudioRenderCategory category;
    bool fill_buffer;

    RoundtripRenderOptions()
        : category(ABI::Windows::Media::Render::AudioRenderCategory::AudioRenderCategory_Other)
        , fill_buffer(false)
    {}
};

struct RoundtripCaptureOptions : RoundtripCommonOptions {
    ABI::Windows::Media::Capture::MediaCategory category;

    RoundtripCaptureOptions()
        : category(ABI::Windows::Media::Capture::MediaCategory::MediaCategory_Other)
    {}
};

struct RoundtripOptions {
    RoundtripRenderOptions render;
    RoundtripCaptureOptions capture;
};
