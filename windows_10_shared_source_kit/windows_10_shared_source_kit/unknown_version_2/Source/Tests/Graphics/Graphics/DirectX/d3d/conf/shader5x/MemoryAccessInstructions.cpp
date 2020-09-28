_VSCROLL | WS_TABSTOP
END

#include <common.ver>



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
﻿<?xml version='1.0' encoding='utf-8' standalone='yes'?>
<Content xmlns="http://microsoft.com/schemas/Windows/Kits/Manifest">
  <ContentGroup
      BuildArchitecture="each"
      Destination="$(_BuildArch)"
      >
    <FeatureReference Name="Device.Graphics"/>
    <BinaryFile
        Destination="nttest\windowstest\graphics\d3d\conf"
        Name="HOSurf.exe"
        Source="$(OBJ_PATH)\$(O)"
        />
  </ContentGroup>
</Content>
// roundtriplatency.cpp
//
// Copyright (C) Microsoft. All rights reserved.

#include "pch.h"
#include "roundtriplatency.h"

// Callback to render
void RoundtripLatency::ToRender(unsigned char* data, int available_frames,
                                int, int bit_depth, int channels,
                                uint64_t,
                                uint64_t) {

    // set buffer to 0
    int frame_size_in_bytes = available_frames * channels * (bit_depth / 8);
    memset(data, 0, frame_size_in_bytes);

    // if we should render a peak
    if (make_peak_ == true) {
        make_peak_ = false;
        // ||--------------------------|
        // ^^                          ^ 
        // ||Unit sample               |
        // |Start Frame                | 

        // render a unit sample
        if (bit_depth == 16) {
            short* samples = reinterpret_cast<short*>(data);
            samples[0] = INT16_MAX;
            // if stereo then send out on other channel as well
            if (channels > 1) {
                samples[1] = INT16_MAX;
            }
            // if more than 2 channels we just don't render on them
        }
        if (bit_depth == 32) {
            float* samples = reinterpret_cast<float*>(data);
            samples[0] = 1.f;
            // if stereo then send out on other channel as well
            if (channels > 1) {
                samples[1] = 1.f;
            }
            // if more than 2 channels we just don't render on them
        }
        peak_generation_time_ = timer_->GetTimeStampNS();
    }
}

// Callback to capture
void RoundtripLatency::FromCapture(unsigned char* data, int available_frames,
                                   int sampling_rate, int bit_depth,
                                   int channels, uint64_t,
                                   uint64_t event_timestamp) {
    uint64_t time_since_peak_generation_ = timer_->GetTimeStampNS() - peak_generation_time_;

    // first time
    if (peak_generation_time_ == 0) {
        make_peak_ = true;
    }

    // if peak has been rendered and time since generation is less than 2 ms
    if (timer_->NSToMS(time_since_peak_generation_) > 2 && make_peak_ == false) {

        float* float_samples = reinterpret_cast<float*>(data);
        short* short_samples = reinterpret_cast<short*>(data);
        float delta = 0;

        for (int i = channels; i < available_frames * channels; i++) {

            // diff
            if (bit_depth == 16) {
                delta = static_cast<short>(short_samples[i] - short_samples[i - channels]);
            } else if (bit_depth == 32) {
                delta = float_samples[i] - float_samples[i - channels];
            } else {
                break;
            }

            // abs
            if (delta < 0) {
                delta -= delta;
            }

            // save max, in frame position and frame time
            if (delta > peak_value_) {
                peak_value_ = delta;
                peak_position_ = i;
                peak_frame_delay_ = event_timestamp - peak_generation_time_;
            }
        }
    }

    // if one second has passed then identify the peak and generate new
    if (timer_->NSToMS(time_since_peak_generation_) > 1000.0 && make_peak_ == false) {
        // calculate where in the frame the peak is located in 100ns
        uint64_t peak_adjustment_time_ = static_cast<uint64_t>(1e7 * ((static_cast<double>(peak_position_ / channels)) / sampling_rate));
        // add the peak_adjustment_time to the peak_frame_delay.
        double delay = timer_->NSToMS(peak_frame_delay_ + peak_adjustment_time_);

        delay_values_.push_back(delay);
        make_peak_ = true;
        peak_value_ = 0;
    }
}

double RoundtripLatency::GetAverageDelay() {
    if (delay_values_.size() == 0) {
        return 0;
    }
    double mean = 0;
    for (auto& i : delay_values_) {
        mean += i;
    }
    mean /= delay_values_.size();
    return mean;
};

double RoundtripLatency::GetMaxDelay() {
    if (delay_values_.size() == 0) {
        return 0;
    }
    double max = 0;
    for (auto& i : delay_values_) {
        if (i > max) {
            max = i;
        }
    }
    return max;
};

double RoundtripLatency::GetMinDelay() {
    if (delay_values_.size() == 0) {
        return 0;
    }
    double min = 999;
    for (auto& i : delay_values_) {
        if (i < min) {
            min = i;
        }
    }
    return min;
};

RoundtripLatency::RoundtripLatency(
    Microsoft::WRL::ComPtr<WASAPICapture> capture,
    Microsoft::WRL::ComPtr<WASAPIRender> render)
    : capture_(capture), render_(render), make_peak_(false),
    peak_generation_time_(0), peak_frame_delay_(0),
    peak_value_(0), delay_values_(0) {
    timer_ = std::make_unique<TimeStamp>();
    // register for buffer events
    capture_->AddCallback(
        "RoundtripLatency",
        [this](unsigned char* buffer, int buffer_size, int sampling_rate,
               int bit_depth, int channels, uint64_t audio_timestamp,
               uint64_t system_timestamp) {
            FromCapture(buffer, buffer_size, sampling_rate, bit_depth, channels,
                        audio_timestamp, system_timestamp);
        });
    render_->AddCallback(
        "RoundtripLatency",
        [this](unsigned char* buffer, int buffer_size, int sampling_rate,
               int bit_depth, int channels, uint64_t audio_timestamp,
               uint64_t system_timestamp) {
            ToRender(buffer, buffer_size, sampling_rate, bit_depth, channels,
                     audio_timestamp, system_timestamp);
        });
}

RoundtripLatency::~RoundtripLatency() {
    // remove yourself from buffer events
    capture_->RemoveCallback("RoundtripLatency");
    render_->RemoveCallback("RoundtripLatency");
}
// ResourceAccess Vertex Shader


struct VSIn
{
	float2 vPos	: pos;
	float4 vTexCoords : texCoord;
};

struct VSOut
{
	float4 vPos : SV_Position;
	float2 vTexCoords : texCoord;
};

cbuffer cb0 : register( b0 )
{	
	float4		mxTrans;
};

VSOut VShaderMain( VSIn input )
{
	VSOut output;
	
	// just to slow down processing enough to get DXGI_ERROR_WAS_STILL_DRAWING
	for( int i = 0; i < 300; i++ )
	{
		output.vPos = float4( 0.f, 0.f, 0.f, 1.f );
		output.vTexCoords = float2( 0.f, 0.f );
	}
	
	return output;
}/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	ZCmp.h
 *
 ***************************************************************************/

#ifndef __ZCMP_H__
#define __ZCMP_H__

#include "CD3DTest.h"

// Defines
#ifdef FAST_TEST
const int MAX_TESTS = 10;
#else
const int MAX_TESTS = 16;
#endif

// Base Class definitions
class CZCmpTest: public CD3DTest
{
    // Data
    private:
    D3DTLVERTEX     VertexList[24];
    DWORD           dwVertexCount;
	bool			bCapAbort;
	TCHAR			msgString[80];

    // Data
	public:
	DWORD			dwZCmpCap;

	public:
	CZCmpTest();
	~CZCmpTest();

	// Framework functions
	virtual bool CapsCheck(void);
	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT uTestNum);
	virtual void SceneRefresh(void);

	// Helper functions
	void ComboTests(float Zval);
	void AlwaysTests(int nTest);
	void LessTests(int nTest);
	bool EqualTests(int nTest);
	void GreaterTests(int nTest);
};

// Never Class definitions
class CZNeverTest: public CZCmpTest
{
	public:
	CZNeverTest();

	bool SetDefaultRenderStates(void);
};

// Less Class definitions
class CZLessTest: public CZCmpTest
{
	public:
	CZLessTest();

	bool SetDefaultRenderStates(void);
};

// Equal Class definitions
class CZEqualTest: public CZCmpTest
{
	public:
	CZEqualTest();

	bool SetDefaultRenderStates(void);
    bool ClearFrame(void);
};

// LessEqual Class definitions
class CZLessEqualTest: public CZCmpTest
{
	public:
	CZLessEqualTest();

	bool SetDefaultRenderStates(void);
};

// Greater Class definitions
class CZGreaterTest: public CZCmpTest
{
	public:
	CZGreaterTest();

	bool SetDefaultRenderStates(void);
    bool ClearFrame(void);
};

// NotEqual Class definitions
class CZNotEqualTest: public CZCmpTest
{
	public:
	CZNotEqualTest();

	bool SetDefaultRenderStates(void);
    bool ClearFrame(void);
};

// GreaterEqual Class definitions
class CZGreaterEqualTest: public CZCmpTest
{
	public:
	CZGreaterEqualTest();

	bool SetDefaultRenderStates(void);
    bool ClearFrame(void);
};

// Always Class definitions
class CZAlwaysTest: public CZCmpTest
{
	public:
	CZAlwaysTest();

	bool SetDefaultRenderStates(void);
};

#endif
/*++
 *
Copyright (c) Microsoft Corporation

Module Name:

    counter.h

Abstract:

    Responsible for creating and closing instances of various counter sets.
    Most of the logic is about creating a proper instance name.

--*/

#include "pch.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, Counter_Register)
#pragma alloc_text(PAGE, Counter_Unregister)
#pragma alloc_text(PAGE, Counter_CreateInterrupterInstance)
#pragma alloc_text(PAGE, Counter_CreateCommonBufferInstance)
#pragma alloc_text(PAGE, Counter_CreateTransferRingInstance)
#endif

VOID
__drv_requiresIRQL(PASSIVE_LEVEL)
Counter_Register()
{
    PAGED_CODE();

    if (g_WdfDriverUsbXhciContext->TestModeOn) {
        (VOID)Ctr_RegisterInterrupter(NULL, NULL);
        (VOID)Ctr_RegisterCommonBuffer(NULL, NULL);
        (VOID)Ctr_RegisterTransferRing(NULL, NULL);
    }
}

VOID
__drv_requiresIRQL(PASSIVE_LEVEL)
Counter_Unregister()
{
    PAGED_CODE();

    if (g_WdfDriverUsbXhciContext->TestModeOn) {
        Ctr_UnregisterInterrupter();
        Ctr_UnregisterCommonBuffer();
        Ctr_UnregisterTransferRing();
    }
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Counter_CreateInterrupterInstance(
    __in
        PINTERRUPTER_DATA   InterrupterData
    )
{
    HANDLE          controllerHandle;
    UNICODE_STRING  instanceName;
    WCHAR           instanceNameBuffer[RTL_NUMBER_OF("VVVV.IIII.65535")];

    PAGED_CODE();

    TRY {

        controllerHandle = InterrupterData->ControllerHandle;

        if (g_WdfDriverUsbXhciContext->TestModeOn == FALSE) {
            LEAVE;
        }

        if (InterrupterData->Counter.PcwInstanceHandle != NULL) {
            LEAVE;
        }

        if (Controller_GetParentBusType(controllerHandle) == UcxControllerParentBusTypePci) {

            RtlStringCchPrintfW(instanceNameBuffer,
                                RTL_NUMBER_OF(instanceNameBuffer),
                                L"%04X.%u.%u",
                                Controller_GetPciVendorID(controllerHandle),
                                Controller_GetInstanceNumber(controllerHandle),
                                InterrupterData->MessageNumber);

        } else {

            RtlStringCchPrintfW(instanceNameBuffer,
                                RTL_NUMBER_OF(instanceNameBuffer),
                                L"%S.%u.%u",
                                Controller_GetAcpiVendorID(controllerHandle),
                                Controller_GetInstanceNumber(controllerHandle),
                                InterrupterData->MessageNumber);
        }

        RtlInitUnicodeString(&instanceName, instanceNameBuffer);

        (VOID)Ctr_CreateInterrupter(&(InterrupterData->Counter.PcwInstanceHandle),
                                    &instanceName,
                                    &(InterrupterData->Counter));

    } FINALLY {

    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Counter_CloseInterrupterInstance(
    __in
        PINTERRUPTER_DATA   InterrupterData
    )
{
    if (InterrupterData->Counter.PcwInstanceHandle != NULL) {
        Ctr_CloseInterrupter(InterrupterData->Counter.PcwInstanceHandle);
    }
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Counter_CreateCommonBufferInstance(
    __in
        PCOMMON_BUFFER_DATA CommonBufferData
    )
{
    PCONTROLLER_DATA    controllerData;
    UNICODE_STRING      instanceName;
    WCHAR               instanceNameBuffer[RTL_NUMBER_OF("VVVV.IIII")];

    PAGED_CODE();

    TRY {

        controllerData = CommonBufferData->ControllerHandle;

        if (g_WdfDriverUsbXhciContext->TestModeOn == FALSE) {
            LEAVE;
        }

        if (CommonBufferData->Counter.PcwInstanceHandle != NULL) {
            LEAVE;
        }

        if (Controller_GetParentBusType(controllerData) == UcxControllerParentBusTypePci) {

            RtlStringCchPrintfW(instanceNameBuffer,
                                RTL_NUMBER_OF(instanceNameBuffer),
                                L"%04X.%u",
                                Controller_GetPciVendorID(controllerData),
                                Controller_GetInstanceNumber(controllerData));

        } else {

            RtlStringCchPrintfW(instanceNameBuffer,
                                RTL_NUMBER_OF(instanceNameBuffer),
                                L"%S.%u",
                                Controller_GetAcpiVendorID(controllerData),
                                Controller_GetInstanceNumber(controllerData));
        }

        RtlInitUnicodeString(&instanceName, instanceNameBuffer);

        (VOID)Ctr_CreateCommonBuffer(&(CommonBufferData->Counter.PcwInstanceHandle),
                                     &instanceName,
                                     &(CommonBufferData->Counter));

    } FINALLY {

    }

    return;

}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Counter_CloseCommonBufferInstance(
    __in
        PCOMMON_BUFFER_DATA CommonBufferData
    )
{
    if (CommonBufferData->Counter.PcwInstanceHandle != NULL) {
        Ctr_CloseCommonBuffer(CommonBufferData->Counter.PcwInstanceHandle);
    }
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Counter_CreateTransferRingInstance(
    __in
        PENDPOINT_DATA         EndpointData,
    __in
        ULONG                  StreamId,
    __in
        PTRANSFERRING_COUNTER  TransferRingCounter
    )
{
    UNICODE_STRING  instanceName;
    WCHAR           instanceNameBuffer[RTL_NUMBER_OF("VVVV.IIII.VENN.EE.SS")];

    PAGED_CODE();

    TRY {

        if (g_WdfDriverUsbXhciContext->TestModeOn == FALSE) {
            LEAVE;
        }

        if (TransferRingCounter->PcwInstanceHandle != NULL) {
            LEAVE;
        }

        if (Controller_GetParentBusType(EndpointData->ControllerHandle) ==
            UcxControllerParentBusTypePci) {

            RtlStringCchPrintfW(instanceNameBuffer,
                                RTL_NUMBER_OF(instanceNameBuffer),
                                L"%04X.%u.%04X.%02X.%u",
                                Controller_GetPciVendorID(EndpointData->ControllerHandle),
                                Controller_GetInstanceNumber(EndpointData->ControllerHandle),
                                UsbDevice_GetVendorId(EndpointData->UsbDeviceHandle),
                                Endpoint_GetAddress(EndpointData),
                                StreamId);

        } else {

            RtlStringCchPrintfW(instanceNameBuffer,
                                RTL_NUMBER_OF(instanceNameBuffer),
                                L"%S.%u.%04X.%02X.%u",
                                Controller_GetAcpiVendorID(EndpointData->ControllerHandle),
                                Controller_GetInstanceNumber(EndpointData->ControllerHandle),
                                UsbDevice_GetVendorId(EndpointData->UsbDeviceHandle),
                                Endpoint_GetAddress(EndpointData),
                                StreamId);
        }

        RtlInitUnicodeString(&instanceName, instanceNameBuffer);

        (VOID)Ctr_CreateTransferRing(&TransferRingCounter->PcwInstanceHandle,
                                     &instanceName,
                                     TransferRingCounter);

    } FINALLY {

    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Counter_CloseTransferRingInstance(
    __in
        PTRANSFERRING_COUNTER TransferRingCounter
    )
{
    if (TransferRingCounter->PcwInstanceHandle != NULL) {
        Ctr_CloseTransferRing(TransferRingCounter->PcwInstanceHandle);
    }
}
//////////////////////////////////////////////////////////////////////////
//  MemoryInstructions.cpp
//  created:	2009/06/11
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////


#include <float.h>
#include "MemoryAccessInstructions.h"
#include <strsafe.h>

using namespace MemoryAccessInstructions;

//---------------------------------------------------------------------------------------------------------------------------------
void ThrowTRFailure( HRESULT hr, const char * Message)
{
	if (FAILED(hr))
	{
		TRException exc(Message);
		char hrMsg[1000];
		StringCchPrintf(hrMsg, ARRAYSIZE(hrMsg), " hr = %08x, " );
		exc.Prepend(hrMsg);
		throw exc;
	}
}

//---------------------------------------------------------------------------------------------------------------------------------
class CMapResource
{
public:
    CMapResource( CD3D11Test * pTest, ID3D11Resource * pResource, UINT Subresource = 0 ) :
        m_Subresource( Subresource ),
        m_pTest( pTest ),
        m_pResource( pResource )
    {
		ThrowTRFailure( m_pTest->ExecuteEffectiveContext(), 
            "Failed to execute effective context before map");

        ThrowTRFailure( m_pTest->GetImmediateContext()->Map( m_pResource, m_Subresource, D3D11_MAP_READ_WRITE, 0, &m_mappedResource ), 
            "Couldn't map output staging");
    }
    ~CMapResource(  )
    {
        m_pTest->GetImmediateContext()->Unmap( m_pResource, m_Subresource );
    }
    UINT32 *    Uint32( UINT x, UINT y = 0, UINT z = 0 )
    { 
        BYTE * pb = reinterpret_cast<BYTE*>(m_mappedResource.pData);
        return reinterpret_cast<UINT32*>( 
            pb + 
            x * sizeof(UINT32) +
            y * m_mappedResource.RowPitch + 
            z * m_mappedResource.DepthPitch  );
    }

protected:
	WindowsTest::CHandle<ID3D11Resource> m_pResource; 
    UINT             m_Subresource;
    CD3D11Test     * m_pTest; 
    D3D11_MAPPED_SUBRESOURCE m_mappedResource;
};


//---------------------------------------------------------------------------------------------------------------------------------
namespace MemoryAccessInstructions
{
BEGIN_NAMED_VALUES( MemoryAccessInstructions::AddressType )
	NAMED_VALUE( _T( "Out_Low" ), Out_Low )
	NAMED_VALUE( _T( "Out_High" ), Out_High )
	NAMED_VALUE( _T( "Out_PartialLow" ), Out_PartialLow )
	NAMED_VALUE( _T( "Out_PartialHigh" ), Out_PartialHigh )
	NAMED_VALUE( _T( "In_Start" ), In_Start )
	NAMED_VALUE( _T( "In_End" ), In_End )
	NAMED_VALUE( _T( "In_Middle" ), In_Middle )
	NAMED_VALUE( _T( "NoAddressType" ), NoAddressType )
END_NAMED_VALUES( MemoryAccessInstructions::AddressType )

BEGIN_NAMED_VALUES( MemoryType )
	NAMED_VALUE( _T( "UAV" ), UAV )
	NAMED_VALUE( _T( "TGSM" ), TGSM )
END_NAMED_VALUES( MemoryType )

BEGIN_NAMED_VALUES( ViewType )
	NAMED_VALUE( _T( "Raw" ), Raw )
	NAMED_VALUE( _T( "Structured" ), Structured )
	NAMED_VALUE( _T( "Typed" ), Typed )
END_NAMED_VALUES( ViewType )

BEGIN_NAMED_VALUES( SelectComponent )
	NAMED_VALUE( _T( "SelectX" ), SelectX )
	NAMED_VALUE( _T( "SelectY" ), SelectY )
	NAMED_VALUE( _T( "SelectZ" ), SelectZ )
	NAMED_VALUE( _T( "SelectW" ), SelectW )
END_NAMED_VALUES( SelectComponent )

BEGIN_NAMED_VALUES( CounterOperation )
	NAMED_VALUE( _T( "Decrement" ), Decrement )
	NAMED_VALUE( _T( "Underflow" ), Underflow )
	NAMED_VALUE( _T( "Increment" ), Increment )
	NAMED_VALUE( _T( "Overflow" ), Overflow )
END_NAMED_VALUES( CounterOperation )

BEGIN_NAMED_VALUES( CounterUAVType )
	NAMED_VALUE( _T( "NotACounter" ), NotACounter )
	NAMED_VALUE( _T( "Append" ), Append )
	NAMED_VALUE( _T( "Count" ), Count )
END_NAMED_VALUES( CounterUAVType )
};

//---------------------------------------------------------------------------------------------------------------------------------
static const TCHAR* SelectComponentToMaskString(SelectComponent cmp)
{
	switch(cmp)
	{
	case SelectX:
		return _T("x");

	case SelectY:
		return _T("y");

	case SelectZ:
		return _T("z");

	case SelectW:
		return _T("w");

	default:
		return _T("");
	}
}

//---------------------------------------------------------------------------------------------------------------------------------
void CMemoryAccessShader5xTest::CreateTestFactors()
{
	AddParameter< AddressType       >( _T("AddressType"),       &m_AddressType );
	AddParameter< MemoryType        >( _T("MemoryType"),        &m_MemoryType );
	AddParameter< ViewType          >( _T("ViewType"),          &m_ViewType );
	AddParameter< SelectComponent   >( _T("SelectSourceComponent"),   &m_SelectSourceComponent );
	AddParameter< SelectComponent   >( _T("SelectSource2Component"),  &m_SelectSource2Component );
	AddParameter< SelectComponent   >( _T("DestMask"),          &m_DestMask );
	AddParameter< D3D_SHADER_STAGE  >( _T("SHADER_STAGE_TYPE"), &m_ShaderStage );
	AddParameter< DXGI_FORMAT       >( _T("UAVFormat"),         &m_UAVFormat );
	AddParameter< UINT              >( _T("NumComponents"),     &m_NumComponents );
	AddParameter< CounterOperation  >( _T("CounterOperation"),  &m_CounterOperation );
	AddParameter< CounterUAVType    >( _T("CounterUAVType"),    &m_CounterUAVType );

	testfactor::RFactor rNotACounterUAV= 
		AddParameterValue( _T("CounterUAVType"), NotACounter );
	testfactor::RFactor rAppendUAVType = 
		AddParameterValue( _T("CounterUAVType"), Append );
	testfactor::RFactor rCountUAVType = 
		AddParameterValue( _T("CounterUAVType"), Count );

	testfactor::RFactor rDestMaskX = 
		AddParameterValue( _T("DestMask"), SelectX );
	testfactor::RFactor rDestMaskY = 
		AddParameterValue( _T("DestMask"), SelectY );
	testfactor::RFactor rDestMaskZ = 
		AddParameterValue( _T("DestMask"), SelectZ );
	testfactor::RFactor rDestMaskW = 
		AddParameterValue( _T("DestMask"), SelectW );

	testfactor::RFactor rDecUnder = 
		AddParameterValue( _T("CounterOperation"), Decrement ) +
		AddParameterValue( _T("CounterOperation"), Underflow );

	testfactor::RFactor rIncOver = 
		AddParameterValue( _T("CounterOperation"), Increment ) +
		AddParameterValue( _T("CounterOperation"), Overflow );

	testfactor::RFactor r1Component = 
		AddParameterValue( _T("NumComponents"), (UINT) 1 );
	testfactor::RFactor r4Components = 
		AddParameterValue( _T("NumComponents"), (UINT) 4 );

	testfactor::RFactor rPS = 
		AddParameterValue( _T("SHADER_STAGE_TYPE"), D3D_SHADER_STAGE_PIXEL );

	testfactor::RFactor rCS = 
		AddParameterValue( _T("SHADER_STAGE_TYPE"), D3D_SHADER_STAGE_COMPUTE );

	testfactor::RFactor rVS = 
		AddParameterValue( _T("SHADER_STAGE_TYPE"), D3D_SHADER_STAGE_VERTEX );

	testfactor::RFactor rGS = 
		AddParameterValue( _T("SHADER_STAGE_TYPE"), D3D_SHADER_STAGE_GEOMETRY );

	testfactor::RFactor rHS = 
		AddParameterValue( _T("SHADER_STAGE_TYPE"), D3D_SHADER_STAGE_HULL );

	testfactor::RFactor rDS = 
		AddParameterValue( _T("SHADER_STAGE_TYPE"), D3D_SHADER_STAGE_DOMAIN );

	// Pixel shader is the only rendering shader supported below feature level 11.1
	testfactor::RFactor rNonCSShaderStages = rPS;
	if( g_TestApp.IsFL11_1OrGreater() )
	{
		rNonCSShaderStages = rNonCSShaderStages + rVS + rGS + rHS + rDS;
	}

	testfactor::RFactor rAddressOut = 
		AddParameterValue( _T("AddressType"), Out_Low ) +
		AddParameterValue( _T("AddressType"), Out_High ) +
		AddParameterValue( _T("AddressType"), Out_PartialLow ) +
		AddParameterValue( _T("AddressType"), Out_PartialHigh );

	testfactor::RFactor rAddressMiddle = 
		AddParameterValue( _T("AddressType"), In_Middle );

	testfactor::RFactor rAddressInButNotMiddle = 
		AddParameterValue( _T("AddressType"), In_Start ) +
		AddParameterValue( _T("AddressType"), In_End );

	testfactor::RFactor rNoAddressType = 
		AddParameterValue( _T("AddressType"), NoAddressType );

	testfactor::RFactor rUAV = 
		AddParameterValue( _T("MemoryType"), UAV );

	testfactor::RFactor rTGSM = 
		AddParameterValue( _T("MemoryType"), TGSM );

	testfactor::RFactor FormatFactorUINT = 
		AddParameterValue( _T("UAVFormat"), DXGI_FORMAT_R32_UINT );
	testfactor::RFactor FormatFactorSINT = 
		AddParameterValue( _T("UAVFormat"), DXGI_FORMAT_R32_SINT );
	testfactor::RFactor FormatFactorFloat = 
		AddParameterValue( _T("UAVFormat"), DXGI_FORMAT_R32_FLOAT );

	testfactor::RFactor rSelectSourceComponentX = 
		AddParameterValue( _T("SelectSourceComponent"), SelectX );

	testfactor::RFactor rSelectSourceComponentFactor = 
        rSelectSourceComponentX +
		AddParameterValue( _T("SelectSourceComponent"), SelectY ) +
		AddParameterValue( _T("SelectSourceComponent"), SelectZ ) +
		AddParameterValue( _T("SelectSourceComponent"), SelectW );

	testfactor::RFactor rRaw = 
		AddParameterValue( _T("ViewType"), Raw );
	testfactor::RFactor rStructured = 
		AddParameterValue( _T("ViewType"), Structured );
	testfactor::RFactor rTyped = 
		AddParameterValue( _T("ViewType"), Typed );

	//-----------------------------------------------------------------------------------------------------------------------------
//	m_SelectSource2 = 
	testfactor::RFactor rSelectSource2X = 
		AddParameterValue( _T("SelectSource2Component"), SelectX );
	testfactor::RFactor rSelectSource2 = 
        rSelectSource2X +
		AddParameterValue( _T("SelectSource2Component"), SelectY ) +
		AddParameterValue( _T("SelectSource2Component"), SelectZ ) +
		AddParameterValue( _T("SelectSource2Component"), SelectW );
	//-----------------------------------------------------------------------------------------------------------------------------
	testfactor::RFactor rRaw_X_Structured = rRaw + rStructured;

	testfactor::RFactor rDestMask1 = 
		rDestMaskX + rDestMaskY + rDestMaskZ + rDestMaskW;

	testfactor::RFactor rAllMaskCombos = 
        rDestMask1 * rSelectSourceComponentFactor;

	testfactor::RFactor rSimpleMasks = 
        rDestMaskX * rSelectSourceComponentX;

	testfactor::RFactor rAllMaskCombos_2Sources = 
        rDestMask1 * rSelectSourceComponentFactor * rSelectSource2;

	testfactor::RFactor rSimpleMasks_2Sources = 
        rDestMaskX * rSelectSourceComponentX * rSelectSource2X;

    //Only "in middle" is given all possible combos of source and dest masks
	testfactor::RFactor rComplexForInMiddle_SimpleForOtherIns = 
        (rAddressInButNotMiddle + rAddressOut) * rSimpleMasks + rAddressMiddle * rAllMaskCombos;

	testfactor::RFactor rComplexForInMiddle_SimpleForOtherIns_2Sources = 
        (rAddressInButNotMiddle + rAddressOut) * rSimpleMasks_2Sources + rAddressMiddle * rAllMaskCombos_2Sources;


	testfactor::RFactor UINTUAVFactors = 
		( ( rRaw_X_Structured * r1Component ) + ( rTyped * r4Components * FormatFactorUINT ) ) * rUAV;
	testfactor::RFactor SINTUAVFactors = 
		( ( rRaw_X_Structured * r1Component ) + ( rTyped * r4Components * FormatFactorSINT ) ) * rUAV;

	testfactor::RFactor TGSMFactors = rTGSM * rRaw_X_Structured * r1Component * FormatFactorUINT; //spec doesn't specify to multiply by num components (as it does for UAV)

	//-----------------------------------------------------------------------------------------------------------------------------
	m_UINTFactors = 
		( (UINTUAVFactors * rComplexForInMiddle_SimpleForOtherIns * rNotACounterUAV) % rNonCSShaderStages ) + 
		( ((UINTUAVFactors + TGSMFactors) * rComplexForInMiddle_SimpleForOtherIns * rNotACounterUAV) % rCS );

	m_SINTFactors = 
		( (SINTUAVFactors * rComplexForInMiddle_SimpleForOtherIns * rNotACounterUAV) % rNonCSShaderStages ) + 
		( ((SINTUAVFactors + TGSMFactors) * rComplexForInMiddle_SimpleForOtherIns * rNotACounterUAV) % rCS );

	m_UINTFactors2Sources = 
		( (UINTUAVFactors * rComplexForInMiddle_SimpleForOtherIns_2Sources * rNotACounterUAV) % rNonCSShaderStages ) +
		( ((UINTUAVFactors + TGSMFactors) * rComplexForInMiddle_SimpleForOtherIns_2Sources * rNotACounterUAV) % rCS );

	m_SINTFactors2Sources = 
		( (SINTUAVFactors * rComplexForInMiddle_SimpleForOtherIns_2Sources * rNotACounterUAV) % rNonCSShaderStages ) +
		( ((SINTUAVFactors + TGSMFactors) * rComplexForInMiddle_SimpleForOtherIns_2Sources * rNotACounterUAV) % rCS );

#if 0
    m_AddressOut = rAddressOut * 
        ( ((rUAV  * rTyped * r4Components * FormatFactorUINT) % rNonCSShaderStages) + 
          ( rCS * rTGSM * rRaw   * r1Component  * FormatFactorUINT) ) *
        rNotACounterUAV * 
        rSimpleMasks);
#endif

	//-----------------------------------------------------------------------------------------------------------------------------
	testfactor::RFactor rCounters =
		(rStructured * rDestMask1 * r1Component);

	//-----------------------------------------------------------------------------------------------------------------------------
	testfactor::RFactor rAppendConsume =
		rCounters * (rAppendUAVType + rCountUAVType) * rUAV * rNoAddressType;

	m_AppendFactor = (rAppendConsume * rIncOver) % (rNonCSShaderStages + rCS);
	m_ConsumeFactor = (rAppendConsume * rDecUnder) % (rNonCSShaderStages + rCS);

}

//---------------------------------------------------------------------------------------------------------------------------------
void CMemoryAccessShader5xTest::CreateResourceBasedOnFactors(UINT32 ByteSize, UINT32 StructureByteStride)
{
	CleanupResource();

	if ( m_NumComponents == 1 )
	{
        //THis buffer is created even when the memory under test is TGSM, in order to have a mechanism to 
        //place initial data in the TGSM and to retrieve the TGSM as modified by the instruction under test.
        MemoryAccessInstructions::ViewType MyViewType = m_ViewType; 
        if ( m_MemoryType == TGSM )
            MyViewType = Typed;

		D3D11_BUFFER_DESC DescBuffer;

		DescBuffer.ByteWidth            = ByteSize;
		DescBuffer.Usage                = D3D11_USAGE_DEFAULT;
		DescBuffer.BindFlags            = D3D11_BIND_UNORDERED_ACCESS;
		DescBuffer.CPUAccessFlags       = 0;
		DescBuffer.MiscFlags            = 0;
		DescBuffer.MiscFlags           |= MyViewType == Structured ? D3D11_RESOURCE_MISC_BUFFER_STRUCTURED      : 0;
		DescBuffer.MiscFlags           |= MyViewType == Raw        ? D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS : 0;
		DescBuffer.StructureByteStride  = MyViewType == Structured ? StructureByteStride                        : 0;

		ThrowTRFailure( GetFramework()->GetDevice()->CreateBuffer(&DescBuffer, 0, reinterpret_cast<ID3D11Buffer**>(&m_pTestResource) ),
			"Couldn't create default test buffer");

		DescBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ; //this is read also, because of the CS_4_0 case. See Validate or CSUAVRead for details.
		DescBuffer.Usage = D3D11_USAGE_STAGING;
		DescBuffer.BindFlags = 0;
		DescBuffer.MiscFlags = 0;

		ThrowTRFailure( GetFramework()->GetDevice()->CreateBuffer(&DescBuffer, 0, &m_pStagingBuffer),
			"Couldn't create staging test buffer");

        {
		    D3D11_UNORDERED_ACCESS_VIEW_DESC uavd;

		    uavd.ViewDimension          = D3D11_UAV_DIMENSION_BUFFER;
		    uavd.Buffer.FirstElement    = 0;
    		
            //If testing TGSM, then the uav is for transporting test data, and we choose "Typed"
            switch( MyViewType )
		    {
		    case Typed:         uavd.Buffer.Flags = 0;                         uavd.Format = m_UAVFormat;              break;
		    case Raw:           uavd.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW; uavd.Format = DXGI_FORMAT_R32_TYPELESS; break;
		    case Structured:    uavd.Buffer.Flags = 0;                         uavd.Format = DXGI_FORMAT_UNKNOWN;      break;
		    default : assert( false ); break;
		    }

		    UINT elementByteSize = DXGI_FORMAT_UNKNOWN == uavd.Format ? StructureByteStride : GetBitsPerElement( uavd.Format ) / 8;

		    uavd.Buffer.NumElements = ByteSize / elementByteSize;

		    switch( m_CounterUAVType )
		    {
		    case Append: uavd.Buffer.Flags |= D3D11_BUFFER_UAV_FLAG_APPEND;  break;
		    case Count:  uavd.Buffer.Flags |= D3D11_BUFFER_UAV_FLAG_COUNTER; break;
		    }

		    ThrowTRFailure( GetFramework()->GetDevice()->CreateUnorderedAccessView(m_pTestResource, &uavd, & m_pTestUAV),
			    "Couldn't create UAV ");
        }
	}
	else if ( m_NumComponents == 4 )
	{
		assert( m_ViewType == Typed );

		D3D11_TEXTURE3D_DESC Tex3ddesc;
		Tex3ddesc.Width = ByteSize/sizeof(UINT32);
		Tex3ddesc.Height = ByteSize/sizeof(UINT32);
		Tex3ddesc.Depth = ByteSize/sizeof(UINT32);
		Tex3ddesc.MipLevels = 1;
		Tex3ddesc.Format = m_UAVFormat;
		Tex3ddesc.Usage = D3D11_USAGE_DEFAULT;
		Tex3ddesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		Tex3ddesc.CPUAccessFlags = 0;
		Tex3ddesc.MiscFlags = 0;

		ThrowTRFailure( GetFramework()->GetDevice()->CreateTexture3D(&Tex3ddesc, 0, reinterpret_cast<ID3D11Texture3D**>(&m_pTestResource) ),
			"Couldn't create default test tex3d");

		Tex3ddesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ; 
		Tex3ddesc.Usage = D3D11_USAGE_STAGING;
		Tex3ddesc.BindFlags = 0;
		Tex3ddesc.MiscFlags = 0;

		ThrowTRFailure( GetFramework()->GetDevice()->CreateTexture3D(&Tex3ddesc, 0, &m_pStagingTex3D),
			"Couldn't create staging test tex3d");

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavd;

		uavd.Format                 = m_UAVFormat; 
		uavd.ViewDimension          = D3D11_UAV_DIMENSION_TEXTURE3D;
		uavd.Texture3D.MipSlice     = 0;
		uavd.Texture3D.FirstWSlice  = 0;
		uavd.Texture3D.WSize        = Tex3ddesc.Depth;

		ThrowTRFailure( GetFramework()->GetDevice()->CreateUnorderedAccessView(m_pTestResource, &uavd, & m_pTestUAV),
			"Couldn't create UAV ");
	}
	else
	{
		assert(false);
	}
}

//---------------------------------------------------------------------------------------------------------------------------------
void CMemoryAccessShader5xTest::CleanupResource()
{
	m_pTestResource = 0; //releases
	m_pStagingBuffer = 0; //releases
	m_pStagingTex3D = 0; //releases
	m_pTestUAV = 0; //releases
}

//---------------------------------------------------------------------------------------------------------------------------------
UINT32 * CMemoryAccessShader5xTest::PointToComponent( UINT32 *p, SelectComponent c )
{
    return p + (UINT32) c;
}

//---------------------------------------------------------------------------------------------------------------------------------
int CMemoryAccessShader5xTest::AddressTypeToOffset( AddressType a )
{
    int offset = 0;
    switch(a)
    {
    case Out_PartialLow:    offset = -4;                             break;
	// Note this is +4. This means it is not really testing "partially out". However the test was written in a way that makes this very difficult to control.
    case Out_PartialHigh:   offset = m_BufferSize+4;                 break;	

    case Out_Low:           offset = -20;                            break;
    case Out_High:          offset = m_BufferSize*2;                 break;
    case In_Start:          offset = 0;                              break;
    case In_End:            offset = m_BufferSize-(sizeof(UINT32));  break;
    case In_Middle:         offset = sizeof(UINT32);                 break;
//    case In_MiddleUnaligned:offset = m_BufferSize/2+1;               break;

    default : assert(false); return 0;
    }

    //locations are 
    return offset;
}

//---------------------------------------------------------------------------------------------------------------------------------
bool CMemoryAccessShader5xTest::IsOutOfBoundsOffset( AddressType a)
{
    switch(a)
    {
    case Out_Low:           
    case Out_High:          
    case Out_PartialLow:    
    case Out_PartialHigh:   
//    case In_MiddleUnaligned:
        return true;
    default : return false;
    }
}

//---------------------------------------------------------------------------------------------------------------------------------
// Base class for everything but append/consume
//---------------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------------------
CShader5xTest_Atomic_instruction::CShader5xTest_Atomic_instruction() :
      MemoryAccessInstructions::CMemoryAccessShader5xTest( DXGI_FORMAT_R32G32B32A32_UINT )
{
}

      
//---------------------------------------------------------------------------------------------------------------------------------
void CShader5xTest_Atomic_instruction::InitTestParameters()
{
    CreateTestFactors();
    testfactor::RFactor myFactors;
    testfactor::RFactor mySINTFactors;
    testfactor::RFactor myUINTFactors;

    if ( 2 == NumSources() )
    {
        mySINTFactors = m_SINTFactors2Sources;
        myUINTFactors = m_UINTFactors2Sources;
    }
    else
    {
        mySINTFactors = m_SINTFactors;
        myUINTFactors = m_UINTFactors;
    }

    if ( CanOperateOnSINT() && CanOperateOnUINT() )
    {
        myFactors  = mySINTFactors + myUINTFactors;
    }
    else if ( CanOperateOnUINT()  )
    {
        myFactors = myUINTFactors;
    }
    else if ( CanOperateOnSINT() )
    {
        myFactors = mySINTFactors;
    }
    else
    {
        assert(false && "Please override one or both of CanOperateOnSINT and CanOperateOnUINT");
    }

    SetRootTestFactor( myFactors ); 

/*
switch( NumSources() )
    {
    case 1 : SetRootTestFactor( SetRootTestFactormyFactors + m_AddressOut ); break;
    case 2 : SetRootTestFactor( ( myFactors + m_AddressOut ) * m_SelectSource2 ); break;
    default: assert(false); break;
    }
    */

	AddPriorityPatternFilter(FilterEqual<AddressType>(_T("AddressType"), In_Middle), WeightFactorVeryLarge);

	// Out-of-bounds addressing is not pri 1
	AddPriorityPatternFilter(FilterEqual<AddressType>(_T("AddressType"), Out_Low), WeightFactorVerySmall);
	AddPriorityPatternFilter(FilterEqual<AddressType>(_T("AddressType"), Out_High), WeightFactorVerySmall);
	AddPriorityPatternFilter(FilterEqual<AddressType>(_T("AddressType"), Out_PartialLow), WeightFactorVerySmall);
	AddPriorityPatternFilter(FilterEqual<AddressType>(_T("AddressType"), Out_PartialHigh), WeightFactorVerySmall);

	// Swizzling and masks are not pri 1
	AddPriorityPatternFilter(FilterEqual<SelectComponent>(_T("SelectSourceComponent"), SelectX), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SelectComponent>(_T("SelectSource2Component"), SelectX), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SelectComponent>(_T("DestMask"), SelectX), WeightFactorLarge);

	AddPriorityPatternFilter(FilterEqual<ViewType>(_T("ViewType"), Structured), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<ViewType>(_T("ViewType"), Raw), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<ViewType>(_T("ViewType"), Typed), WeightFactorNormal);	// Not as important as structured and raw
	AddPriorityPatternFilter(FilterEqual<CounterUAVType>(_T("CounterUAVType"), Append), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<CounterUAVType>(_T("CounterUAVType"), Count), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("NumComponents"), 4), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("NumComponents"), 1), WeightFactorSmall);

	if( GetInstruction() == _T("atomic_cmp_store") ||
		GetInstruction() == _T("imm_atomic_cmp_exch"))
	{
		DeclarePriorityLevelByPriorityValue( 1, 6.75f );
		DeclarePriorityLevelByPriorityValue( 2, 6.0f );
	}
	else
	{
		DeclarePriorityLevelByPriorityValue( 1, 4.5f );
		DeclarePriorityLevelByPriorityValue( 2, 3.0f );
	}
}

//---------------------------------------------------------------------------------------------------------------------------------
TEST_RESULT CShader5xTest_Atomic_instruction::Setup()
{
	if ( GetFramework()->m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_11_0 )
    {
        return RESULT_SKIP;
    }

    TEST_RESULT tRes = RESULT_PASS;
    try
    {
        tRes = CShader5xTest::Setup();
    }
    catch(TRException &exc)
    {
        CleanupTestCase();

	    tRes = exc.GetResult();
	    exc.Prepend("[CInteractionBufferTest::Setup()] ");
	    if (tRes != RESULT_PASS)
        {
		    WriteToLog( exc.GetError().c_str() );
            return tRes;
        }
    }

    return tRes;
}

//---------------------------------------------------------------------------------------------------------------------------------
void CShader5xTest_Atomic_instruction::CleanupTestCase()
{
    CleanupResource();
}

//---------------------------------------------------------------------------------------------------------------------------------
std::string  CMemoryAccessShader5xTest::SelectComponentToString( SelectComponent c )
{
    switch ( c )
    {
    case SelectX: return "x"; 
    case SelectY: return "y"; 
    case SelectZ: return "z"; 
    case SelectW: return "w"; 
    default : assert(false);
    }
    return "";
}

//---------------------------------------------------------------------------------------------------------------------------------
TEST_RESULT CShader5xTest_Atomic_instruction::SetupTestCase()
{
    TEST_RESULT tRes = RESULT_PASS;
    try
    {
        CreateResourceBasedOnFactors(m_BufferSize,sizeof(UINT32));
        // build create and set shader

        //            struct CBLayout
        //            {
        //                UINT32 Source0[4];
        //                UINT32 Source1[4];
        //                UINT32 Dest[4];
        //                UINT32 Offset;
        //            };
        // mov r0,cb.0
        // mov r1,cb.1
        // <instruction> <dest reg r2> u0/g0, <destaddress>.swizzle, r0.select, <r1.select>
        // mov u1+1, r2 //out