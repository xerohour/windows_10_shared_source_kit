// latencytest.cpp
//
// Copyright (C) Microsoft. All rights reserved.

#include "pch.h"

namespace {
static void Sleep(int ms);
}  // namespace

bool LatencyTest::Setup() {

    just_played_ = false;

    return true;
};

void LatencyTest::Custom() {
    RoundtripOptions options;

    // Raw
    bool raw = false;
    HRESULT hr = RuntimeParameters::TryGetValue(L"render_raw", raw);
    if (FAILED(hr) || raw == false) {
        Log::Comment(L"Will initiate audio engine render stream in default mode");
        Log::Comment(L"To override this and use raw mode then specify /p:render_raw=true");
    } else if (raw) {
        options.render.processing = ABI::Windows::Media::AudioProcessing::AudioProcessing_Raw;
        Log::Comment(L"Will initiate audio engine render stream in raw mode");
    }
    hr = RuntimeParameters::TryGetValue(L"capture_raw", raw);
    if (FAILED(hr) || raw == false) {
        Log::Comment(L"Will initiate audio engine capture stream in default mode");
        Log::Comment(L"To override this and use raw mode then specify /p:capture_raw=true");
    } else if (raw) {
        options.capture.processing = ABI::Windows::Media::AudioProcessing::AudioProcessing_Raw;
        Log::Comment(L"Will initiate audio engine capture stream in raw mode");
    }

    // PCM
    hr = RuntimeParameters::TryGetValue(L"render_pcm", options.render.pcm);
    if (FAILED(hr) || options.render.pcm == false) {
        Log::Comment(L"Will initiate audio engine render stream in floating point");
        Log::Comment(L"To override this and use fixed point then specify /p:render_pcm=true");
    } else if (options.render.pcm) {
        Log::Comment(L"Will not use default format instead initiate audio engine render stream in fixed point");
    }
    hr = RuntimeParameters::TryGetValue(L"capture_pcm", options.capture.pcm);
    if (FAILED(hr) || options.capture.pcm == false) {
        Log::Comment(L"Will initiate audio engine capture stream in floating point");
        Log::Comment(L"To override this please and use fixed point please specify /p:capture_pcm=true");
    } else if (options.capture.pcm == true) {
        Log::Comment(L"Will not use default format instead initiate audio engine capture stream in fixed point");
    }

    // Audio category
    String category;
    hr = RuntimeParameters::TryGetValue(L"render_category", category);
    if (FAILED(hr)) {
        Log::Comment(L"Will render using AudioRenderCategory_Other; use /p:render_category=Communications to override");
    } else {
        hr = AudioRenderCategoryFromString(category, &options.render.category);
        if (FAILED(hr)) {
            return;
        }

        LOG(L"Will render using %s (%d)", StringFromAudioRenderCategory(options.render.category), options.render.category);
    }

    hr = RuntimeParameters::TryGetValue(L"capture_category", category);
    if (FAILED(hr)) {
        Log::Comment(L"Will capture using MediaCategory_Other; use /p:capture_category=Communications to override");
    } else {
        hr = MediaCategoryFromString(category, &options.capture.category);
        if (FAILED(hr)) {
            return;
        }

        LOG(L"Will capture using %s (%d)", StringFromMediaCategory(options.capture.category), options.capture.category);
    }

    // Fill render buffer
    hr = RuntimeParameters::TryGetValue(L"fill_render_buffer", options.render.fill_buffer);
    if (FAILED(hr) || options.render.fill_buffer == false) {
        Log::Comment(L"Will fill the IAudioRenderClient buffer as little as possible");
        Log::Comment(L"To override this and fill it entirely then specify /p:fill_render_buffer=true");
    } else if (options.render.fill_buffer == true) {
        Log::Comment(L"Will fill the IAudioRenderClient buffer all the way");
    }

    // period
#if NTDDI_VERSION >= NTDDI_WINTHRESHOLD
    hr = RuntimeParameters::TryGetValue(L"render_frames", options.render.frames);
    if (FAILED(hr) || options.render.frames == -1) {
        Log::Comment(L"Will use the default render period");
        Log::Comment(L"To specify a render period use /p:render_frames=64, or /p:render_frames=0 to use the minimum");
    } else if (options.render.frames == 0) {
        Log::Comment(L"Will use the fundamental render period");
    } else {
        LOG(L"Will use a render period of %d frames", options.render.frames);
    }

    hr = RuntimeParameters::TryGetValue(L"capture_frames", options.capture.frames);
    if (FAILED(hr) || options.capture.frames == -1) {
        Log::Comment(L"Will use the default capture period");
        Log::Comment(L"To specify a capture period use /p:capture_frames=64, or /p:capture_frames=0 to use the minimum");
    } else if (options.capture.frames == 0) {
        Log::Comment(L"Will use the fundamental capture period");
    } else {
        LOG(L"Will use a capture period of %d frames", options.capture.frames);
    }
#endif

    Roundtrip(options);
}

void LatencyTest::Vanilla() {
    Roundtrip(RoundtripOptions());
}

#if NTDDI_VERSION >= NTDDI_WINTHRESHOLD
void LatencyTest::LowPeriod() {
    RoundtripOptions options;
    options.render.frames = 0;
    options.capture.frames = 0;

    Roundtrip(options);
}
#endif

void LatencyTest::Raw() {
    RoundtripOptions options;
    options.render.processing = ABI::Windows::Media::AudioProcessing::AudioProcessing_Raw;
    options.capture.processing = ABI::Windows::Media::AudioProcessing::AudioProcessing_Raw;

    Roundtrip(options);
}

#if NTDDI_VERSION >= NTDDI_WINTHRESHOLD
void LatencyTest::RawLowPeriod() {
    RoundtripOptions options;
    options.render.processing = ABI::Windows::Media::AudioProcessing::AudioProcessing_Raw;
    options.capture.processing = ABI::Windows::Media::AudioProcessing::AudioProcessing_Raw;
    options.render.frames = 0;
    options.capture.frames = 0;

    Roundtrip(options);
}
#endif

void LatencyTest::Communications() {
    RoundtripOptions options;
    options.render.category = ABI::Windows::Media::Render::AudioRenderCategory::AudioRenderCategory_Communications;
    options.capture.category = ABI::Windows::Media::Capture::MediaCategory::MediaCategory_Communications;

    Roundtrip(options);
}

#if NTDDI_VERSION >= NTDDI_WINTHRESHOLD
void LatencyTest::CommunicationsLowPeriod() {
    RoundtripOptions options;
    options.render.category = ABI::Windows::Media::Render::AudioRenderCategory::AudioRenderCategory_Communications;
    options.capture.category = ABI::Windows::Media::Capture::MediaCategory::MediaCategory_Communications;
    options.render.frames = 0;
    options.capture.frames = 0;

    Roundtrip(options);
}
#endif

void LatencyTest::Media() {
    RoundtripOptions options;
    options.render.category = ABI::Windows::Media::Render::AudioRenderCategory::AudioRenderCategory_Media;
#if NTDDI_VERSION >= NTDDI_WINTHRESHOLD
    options.capture.category = ABI::Windows::Media::Capture::MediaCategory::MediaCategory_Media;
#endif

    Roundtrip(options);
}

#if NTDDI_VERSION >= NTDDI_WINTHRESHOLD
void LatencyTest::MediaLowPeriod() {
    RoundtripOptions options;
    options.render.category = ABI::Windows::Media::Render::AudioRenderCategory::AudioRenderCategory_Media;
    options.capture.category = ABI::Windows::Media::Capture::MediaCategory::MediaCategory_Media;
    options.render.frames = 0;
    options.capture.frames = 0;

    Roundtrip(options);
}
#endif

#if NTDDI_VERSION >= NTDDI_WINTHRESHOLD
void LatencyTest::GameChat() {
    RoundtripOptions options;
    options.render.category = ABI::Windows::Media::Render::AudioRenderCategory::AudioRenderCategory_GameChat;
    options.capture.category = ABI::Windows::Media::Capture::MediaCategory::MediaCategory_GameChat;
    options.capture.category = ABI::Windows::Media::Capture::MediaCategory::MediaCategory_Communications;

    Roundtrip(options);
}

void LatencyTest::GameChatLowPeriod() {
    RoundtripOptions options;
    options.render.category = ABI::Windows::Media::Render::AudioRenderCategory::AudioRenderCategory_GameChat;
    options.capture.category = ABI::Windows::Media::Capture::MediaCategory::MediaCategory_GameChat;
    options.render.frames = 0;
    options.capture.frames = 0;

    Roundtrip(options);
}
#endif

#if NTDDI_VERSION >= NTDDI_WINTHRESHOLD
void LatencyTest::Speech() {
    RoundtripOptions options;
    options.render.category = ABI::Windows::Media::Render::AudioRenderCategory::AudioRenderCategory_Speech;
    options.capture.category = ABI::Windows::Media::Capture::MediaCategory::MediaCategory_Speech;

    Roundtrip(options);
}

void LatencyTest::SpeechLowPeriod() {
    RoundtripOptions options;
    options.render.category = ABI::Windows::Media::Render::AudioRenderCategory::AudioRenderCategory_Speech;
    options.capture.category = ABI::Windows::Media::Capture::MediaCategory::MediaCategory_Speech;
    options.render.frames = 0;
    options.capture.frames = 0;

    Roundtrip(options);
}
#endif

void LatencyTest::ListEffects(
    RoundtripOptions options,
    HSTRING device_render_id,
    HSTRING device_capture_id,
    bool *pAEC
) {
    std::vector<std::string> render_effects;
    if (!VERIFY_SUCCEEDED(
        GetEffectsRenderDevice(
            device_render_id,
            render_effects,
            options.render.category,
            options.render.processing
        )
    )) { return; }

    if (render_effects.size() > 0) {
        LOG(L"-- Render effects --");
        for (auto i : render_effects) {
            Log::Comment(L"    " + String().Append(i.c_str()));
        }
    }

    std::vector<std::string> capture_effects;
    if (!VERIFY_SUCCEEDED(
        GetEffectsCaptureDevice(
            device_capture_id,
            capture_effects,
            options.capture.category,
            options.capture.processing)
    )) { return; }

    if (capture_effects.size() > 0) {
        LOG(L"-- Capture effects --");
        for (auto i : capture_effects) {
            Log::Comment(L"    " + String().Append(i.c_str()));
            if (i == "AcousticEchoCancellation") {
                *pAEC = true;
            }
        }
    }
}

bool LatencyTest::Cleanup() {
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);
    return true;
}

namespace {
    static void Sleep(int ms) {
        WaitForSingleObjectEx(GetCurrentThread(), ms, FALSE);
    }
}  // namespace

void LatencyTest::Roundtrip(RoundtripOptions options) {
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    ABI::Windows::Media::Devices::AudioDeviceRole role;
    if (!VERIFY_SUCCEEDED(
        AudioDeviceRoleFromMediaCategory(options.capture.category, &role)
    )) { return; }

    HSTRING device_capture_id = GetDefaultCaptureDevice(role);
    WindowsDeleteStringOnExit deleteCaptureId(device_capture_id);
    if (device_capture_id == nullptr) {
        Log::Result(TestResults::Blocked,
            L"No capture device found, make sure you have a working capture device, skipping.");
        return;
    } else {
        LOG(L"Capture device id: %s",
            WindowsGetStringRawBuffer(device_capture_id, nullptr));
        HSTRING capture_friendly_name;
        if (!VERIFY_SUCCEEDED(
            GetFriendlyName(device_capture_id, capture_friendly_name)
        )){ return; }

        LOG(L"Capture friendly name: %s",
            WindowsGetStringRawBuffer(capture_friendly_name, nullptr));
        WindowsDeleteString(capture_friendly_name);
    }

    if (!VERIFY_SUCCEEDED(
        AudioDeviceRoleFromAudioRenderCategory(options.render.category, &role)
    )) { return;  }

    HSTRING device_render_id = GetDefaultRenderDevice(role);
    WindowsDeleteStringOnExit deleteRenderId(device_render_id);
    if (device_render_id == nullptr) {
        Log::Result(TestResults::Blocked,
            L"No render device found, make sure you have a working render device, skipping.");
        return;
    } else {
        LOG(L"Render device id:  %s",
            WindowsGetStringRawBuffer(device_render_id, nullptr));
        HSTRING render_friendly_name;
        if (!VERIFY_SUCCEEDED(
            GetFriendlyName(device_render_id, render_friendly_name)
        )) { return; }

        LOG(L"Render friendly name: %s",
            WindowsGetStringRawBuffer(render_friendly_name, nullptr));
        WindowsDeleteString(render_friendly_name);
    }

    // make sure Raw is supported before initialize it
    bool raw_support = false;
    if (options.capture.processing == ABI::Windows::Media::AudioProcessing::AudioProcessing_Raw) {
        if (!VERIFY_SUCCEEDED(
            DeviceSupportsRaw(device_capture_id, raw_support)
        )) { return; }

        if (!raw_support) {
            Log::Result(TestResults::Skipped,
                        L"Raw capture not supported, skipping.");
            return;
        }
    }
    // make sure Raw is supported before initilize it
    raw_support = false;
    if (options.render.processing == ABI::Windows::Media::AudioProcessing::AudioProcessing_Raw) {
        if (!VERIFY_SUCCEEDED(
            DeviceSupportsRaw(device_render_id, raw_support)
        )) { return; }

        if (!raw_support) {
            Log::Result(TestResults::Skipped,
                        L"Raw render not supported, skipping.");
            return;
        }
    }

    bool aec = false;
    ListEffects(options, device_render_id, device_capture_id, &aec);
    if (aec) {
        Log::Result(TestResults::Skipped,
            L"There is Acoustic Echo Cancellation "
            L"in the capture pipeline, skipping.");
        return;
    }

    // the Record and Play will block until capture and render is initialized
    if (just_played_) {
        // let everything settle down
        Sleep(5000);
        just_played_ = false;
    }

    // Initialize MF
    if (!VERIFY_SUCCEEDED(
        MFStartup(MF_VERSION, MFSTARTUP_LITE)
    )) { return; }

    // create render and capture
    ComPtr<WASAPICapture> capture = Make<WASAPICapture>(options.capture);
    ComPtr<WASAPIRender> render = Make<WASAPIRender>(options.render);

    // initialize them async with a given device id
    if (!VERIFY_SUCCEEDED(
        capture->InitializeAudioDeviceAsync(
            WindowsGetStringRawBuffer(device_capture_id, nullptr)
        )
    )) { return; }

    if (!VERIFY_SUCCEEDED(
        render->InitializeAudioDeviceAsync(
            WindowsGetStringRawBuffer(device_render_id, nullptr)
        )
    )) { return; }

    // register test module
    auto latency = std::make_unique<RoundtripLatency>(capture, render);

    if (!VERIFY_SUCCEEDED(
        render->Play()
    )) { return; }

    just_played_ = true;
    Sleep(2);
    if (!VERIFY_SUCCEEDED(
        capture->Record()
    )) { return;  }

    Sleep(10000);

    // stop will set cancel the wait for event and call stop to device
    if (!VERIFY_SUCCEEDED(
        render->StopPlaying()
    )) { return; }
    if (!VERIFY_SUCCEEDED(
        capture->Stop()
    )) { return; }

    LOG(L"Average Delay: %g", latency->GetAverageDelay());
    LOG(L"Max Delay: %g", latency->GetMaxDelay());
    LOG(L"Min Delay: %g", latency->GetMinDelay());
    for (auto i : latency->GetDelayVector()) {
        LOG(L"Delay value: %g", i);
    }
    VERIFY_SUCCEEDED(MFShutdown());
}
