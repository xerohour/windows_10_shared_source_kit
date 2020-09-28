// wasapicapture.cpp
//
// Copyright (C) Microsoft. All rights reserved.

#include "pch.h"
#include "wasapicapture.h"

#include <Windows.Foundation.h>
#include <Windows.System.Threading.h>
#include <wrl/event.h>

#include <hstring.h>
#include <WexTestClass.h>

using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::System::Threading;
using namespace Microsoft::WRL::Wrappers;
using namespace WEX::Logging;
using namespace WEX::Common;

namespace{
template <class T>
void SafeRelease(T **ppT) {
    if (*ppT) {
        (*ppT)->Release();
        *ppT = NULL;
    }
}
}

WASAPICapture::WASAPICapture(RoundtripCaptureOptions options)
: raw_(options.processing == ABI::Windows::Media::AudioProcessing::AudioProcessing_Raw)
, pcm_(options.pcm)
, media_category_(options.category)
#if NTDDI_VERSION >= NTDDI_WINTHRESHOLD
, frames_(options.frames)
#endif
, initialized_(false)
, started_(false)
, mix_format_(nullptr)
{
    timer_ = std::make_unique<TimeStamp>();
}

HRESULT WASAPICapture::Record() {
    // Block the starting of the client until the async init is done
    WaitForSingleObjectEx(audio_client_activated_, INFINITE, NULL);
    if (!initialized_) {
        ERR(L"%s", L"Capture not initialized");
        return E_FAIL;
    }

    HRESULT hr = audio_client_->Start();
    if (FAILED(hr)) { return hr; }
    started_ = true;
    // Register MMCSS work queue
    DWORD dw_task_id = 0;
    hr = MFLockSharedWorkQueue(L"Pro Audio", 1, &dw_task_id, &dw_queue_id_);
    if (FAILED(hr)) { return hr; }
    // Set the capture event work queue to use the MMCSS queue
    m_xBufferAvailable.SetQueueID(dw_queue_id_);
    // Note the startup of the thread might take some ms
    return MFPutWorkItem2(dw_queue_id_, 1, &m_xStartCapture, nullptr);
}

HRESULT WASAPICapture::StartCapture(IMFAsyncResult*) {
    // started capture wait for first capture event
    return MFPutWaitingWorkItem(sample_ready_event_, 1,
                                sample_ready_async_result_, &sample_ready_key_);
}

HRESULT WASAPICapture::BufferAvailable(IMFAsyncResult*) {
    HRESULT hr = AudioBufferAvailable();
    if (FAILED(hr)) { return hr; }
    hr = MFPutWaitingWorkItem(sample_ready_event_, 1,
                                         sample_ready_async_result_,
                                         &sample_ready_key_);
    if (FAILED(hr)) { return hr; }
    return S_OK;
}

// This is the main capture loop
inline HRESULT WASAPICapture::AudioBufferAvailable() {
    uint64_t system_timestamp = timer_->GetTimeStampNS();  // system QPC
    UINT64 audio_timestamp = 0;
    DWORD flags;
    BYTE *data = nullptr;
    UINT available_frames;

    HRESULT hr = audio_capture_client_->GetNextPacketSize(&available_frames);
    if (FAILED(hr)) { return hr; }

    while (available_frames > 0) {
        hr = audio_capture_client_->GetBuffer(
            &data, &available_frames, &flags, NULL, &audio_timestamp);
        if (FAILED(hr)) { return hr; }
        // might wanna add some logging for glitch here
        if (flags & AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY) {
        }
        // call every function that is registered
        for (auto &i : callbacks_) {
            i.second(data, available_frames, mix_format_->nSamplesPerSec,
                mix_format_->wBitsPerSample, mix_format_->nChannels,
                static_cast<uint64_t>(audio_timestamp), system_timestamp);
        }
        hr = audio_capture_client_->ReleaseBuffer(available_frames);
        if (FAILED(hr)) { return hr; }

        hr = audio_capture_client_->GetNextPacketSize(&available_frames);
        if (FAILED(hr)) { return hr; }
    }
    return hr;
}

HRESULT WASAPICapture::Stop() {
    HRESULT hr = S_OK;
    if (0 != sample_ready_key_) {
        hr = MFCancelWorkItem(sample_ready_key_);
        sample_ready_key_ = 0;
    }
    if (started_) {
        hr = audio_client_->Stop();
    }
    SafeRelease(&sample_ready_async_result_);
    return hr;
}

HRESULT WASAPICapture::ActivateCompleted(
    IActivateAudioInterfaceAsyncOperation *operation) {
    HRESULT hr = S_OK;
    HRESULT hr_activate_result = S_OK;
    IUnknown *audio_interface = nullptr;
    AudioClientProperties client_properties = {0};
    client_properties.cbSize = sizeof(client_properties);
    client_properties.bIsOffload = false;
    client_properties.Options =
        (raw_) ? AUDCLNT_STREAMOPTIONS_RAW : AUDCLNT_STREAMOPTIONS_NONE;
    
    hr = AudioStreamCategoryFromMediaCategory(media_category_, &client_properties.eCategory);
    if (FAILED(hr)) {
        goto exit;
    }

    hr = operation->GetActivateResult(&hr_activate_result, &audio_interface);
    if (FAILED(hr)) {
        ERR(L"IActivateAudioInterfaceAsyncOperation::GetActivateResult failed: 0x%08x", hr);
        goto exit;
    }
    if (FAILED(hr_activate_result)) {
        ERR(L"IActivateAudioInterfaceAsyncOperation::GetActivateResult reports activation failure: 0x%08x", hr_activate_result);
        goto exit;
    }
    hr = audio_interface->QueryInterface(IID_PPV_ARGS(&audio_client_));
    if (FAILED(hr)) {
        ERR(L"IUnknown::QueryInterface to audio client interface failed: 0x%08x", hr);
        goto exit;
    }
    hr = audio_client_->SetClientProperties(&client_properties);
    if (FAILED(hr)) {
        ERR(L"IAudioClient::SetClientProperties failed: 0x%08x", hr);
        goto exit;
    }

    hr = audio_client_->GetMixFormat(&mix_format_);
    if (FAILED(hr)) {
        ERR(L"IAudioClient::GetMixFormat failed: 0x%08x", hr);
        goto exit;
    }

    LOG(L"Capture sample rate is %u", mix_format_->nSamplesPerSec);

    if (pcm_) {
        // force fixed point
        mix_format_->wBitsPerSample = 16;
        mix_format_->nBlockAlign = mix_format_->nChannels * mix_format_->wBitsPerSample / 8;
        mix_format_->nAvgBytesPerSec = mix_format_->nSamplesPerSec * mix_format_->nBlockAlign;
        mix_format_->wFormatTag = WAVE_FORMAT_PCM;
        mix_format_->cbSize = 0;
        WAVEFORMATEX* closest_format = nullptr;
        hr = audio_client_->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, mix_format_, &closest_format);
        if (closest_format != nullptr) {
            CoTaskMemFree(closest_format);
        }
        if (FAILED(hr)) {
            ERR(L"IAudioClient::IsFormatSupported with PCM format failed: 0x%08x", hr);
            goto exit;
        }
    }

#if NTDDI_VERSION >= NTDDI_WINTHRESHOLD
    switch (frames_) {
        case -1: {
            hr = audio_client_->Initialize(AUDCLNT_SHAREMODE_SHARED,
                AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                0, 0, mix_format_, nullptr);
            if (FAILED(hr)) {
                ERR(L"IAudioClient::Initialize failed: 0x%08x", hr);
                goto exit;
            }
            break;
        }

        case 0: {
            // query the fundamental periodicity
            UINT32 dummy1;
            UINT32 dummy2;
            UINT32 minimumPeriod;
            UINT32 dummy3;
            hr = audio_client_->GetSharedModeEnginePeriod(
                mix_format_,
                &dummy1, // don't care about the default
                &dummy2, // don't care about the fundamental
                &minimumPeriod,
                &dummy3 // don't care about the max
            );
            if (FAILED(hr)) {
                ERR(L"IAudioClient3::GetSharedModeEnginePeriodicity failed: 0x%08x", hr);
                goto exit;
            }

            frames_ = minimumPeriod;

            LOG(L"Minimum capture period is %d frames", frames_);
            // intentional fallthrough
        }

        default: {
            hr = audio_client_->InitializeSharedAudioStream(
                AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                frames_,
                mix_format_,
                nullptr
            );
            if (FAILED(hr)) {
                ERR(L"IAudioClient3::InitializeSharedAudioStream failed: 0x%08x", hr);
                goto exit;
            }
        }
    }
#else
    hr = audio_client_->Initialize(AUDCLNT_SHAREMODE_SHARED,
        AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
        0, 0, mix_format_, nullptr);
    if (FAILED(hr)) {
        ERR(L"IAudioClient::Initialize failed: 0x%08x", hr);
        goto exit;
    }
#endif

    hr = audio_client_->GetBufferSize(&buffer_size_);
    if (FAILED(hr)) {
        ERR(L"IAudioClient::GetBufferSize failed: 0x%08x", hr);
        goto exit;
    }
    LOG(L"First capture buffer_size after init is %u", buffer_size_);
    hr = audio_client_->GetService(IID_PPV_ARGS(&audio_capture_client_));
    if (FAILED(hr)) {
        ERR(L"IAudioClient::GetService(IAudioCaptureClient) failed: 0x%08x", hr);
        goto exit;
    }
    hr = MFCreateAsyncResult(nullptr, &m_xBufferAvailable, nullptr,
                                      &sample_ready_async_result_);
    if (FAILED(hr)) {
        ERR(L"MFCreateAsyncResult failed: 0x%08x", hr);
        goto exit;
    }
    hr = audio_client_->SetEventHandle(sample_ready_event_);
    if (FAILED(hr)) {
        ERR(L"IAudioClient::SetEventHandle failed: 0x%08x", hr);
        goto exit;
    }
    initialized_ = true;
exit:
    SetEvent(audio_client_activated_);
    SafeRelease(&audio_interface);
    if (FAILED(hr_activate_result)) {
        return hr_activate_result;
    }
    return hr;
}

HRESULT WASAPICapture::InitializeAudioDeviceAsync(PCWSTR device_id) {
    HRESULT hr = S_OK;

    audio_client_activated_ =
        CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
    if (!audio_client_activated_) {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    sample_ready_event_ =
        CreateEventExW(nullptr, nullptr, 0, SYNCHRONIZE | EVENT_MODIFY_STATE);
    if (!sample_ready_event_) {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    ComPtr<IActivateAudioInterfaceAsyncOperation> asyncop; 
    hr = ActivateAudioInterfaceAsync(device_id, __uuidof(*audio_client_.Get()),
                                     nullptr, this, &asyncop);
    return hr;
}

void WASAPICapture::AddCallback(
    const std::string &name,
    std::function<void(unsigned char *buffer, int buffer_size,
                       int sampling_rate, int bit_depth, int channels,
                       uint64_t audio_timestamp, uint64_t system_timestamp)>
        callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto found = callbacks_.find(name);
    if (found != callbacks_.end()) {
        return;
    }
    callbacks_.emplace(name, callback);
}

void WASAPICapture::RemoveCallback(const std::string &name) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto found = callbacks_.find(name);
    if (found == callbacks_.end()) {
        return;
    }
    callbacks_.erase(found);
}

WASAPICapture::~WASAPICapture() {
    if (mix_format_ != nullptr) {
        CoTaskMemFree(mix_format_);
    }
}