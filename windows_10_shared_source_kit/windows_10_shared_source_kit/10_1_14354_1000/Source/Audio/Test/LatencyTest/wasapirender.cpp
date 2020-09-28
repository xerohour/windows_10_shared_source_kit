// wasapirender.cpp
//
// Copyright (C) Microsoft. All rights reserved.

#include "pch.h"
#include "wasapirender.h"
#include <math.h>

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

namespace {
template <class T>
void SafeRelease(T **ppT) {
    if (*ppT) {
        (*ppT)->Release();
        *ppT = NULL;
    }
}
}

WASAPIRender::WASAPIRender(RoundtripRenderOptions options)
: raw_(options.processing == ABI::Windows::Media::AudioProcessing::AudioProcessing_Raw)
, pcm_(options.pcm)
, audio_render_category_(options.category)
, fill_buffer_(options.fill_buffer)
#if NTDDI_VERSION >= NTDDI_WINTHRESHOLD
, frames_(options.frames)
#endif
, initialized_(false)
, started_(false)
, mix_format_(nullptr)
{
    timer_ = std::make_unique<TimeStamp>();
}

HRESULT WASAPIRender::Play() {
    // Block the starting of the client until the async init is done
    WaitForSingleObjectEx(audio_client_activated_, INFINITE, NULL);

    if (!initialized_) {
        ERR(L"%s", L"Initialization failed");
        return E_FAIL;
    }

    // Pre-Roll the buffer with silence
    HRESULT hr = AudioBufferRequested(true);
    if (FAILED(hr)) { return hr; }
    // Actually start the playback
    hr = audio_client_->Start();
    if (FAILED(hr)) { return hr; }
    started_ = true;
    // start thread in threadpool
    // Register MMCSS work queue
    DWORD dw_task_id = 0;
    hr = MFLockSharedWorkQueue(L"Pro Audio", 0, &dw_task_id, &dw_queue_id_);
    if (FAILED(hr)) { return hr; }
    // Set the capture event work queue to use the MMCSS queue
    m_xNeedBuffer.SetQueueID(dw_queue_id_);
    return MFPutWorkItem2(dw_queue_id_, 0, &m_xStartPlayback, nullptr);
}

HRESULT WASAPIRender::StartPlayback(IMFAsyncResult *) {
    return MFPutWaitingWorkItem(sample_ready_event_, 0,
                                sample_ready_async_result_, &sample_ready_key_);
}

HRESULT WASAPIRender::NeedBuffer(IMFAsyncResult *) {
    HRESULT hr = AudioBufferRequested(false);
    if (FAILED(hr)) { return hr; }
    hr = MFPutWaitingWorkItem(sample_ready_event_, 0,
                                         sample_ready_async_result_,
                                         &sample_ready_key_);
    if (FAILED(hr)) { return hr; }
    return S_OK;
}

inline HRESULT WASAPIRender::AudioBufferRequested(bool silence) {
    uint64_t system_timestamp = timer_->GetTimeStampNS();  // system QPC
    UINT32 padding;
    HRESULT hr = audio_client_->GetCurrentPadding(&padding);
    if (FAILED(hr)) {
        ERR(L"IAudioClient::GetCurrentPadding returned failure: 0x%08x", hr);
        return hr;
    }

    UINT fill_up_to = (
        fill_buffer_ ? buffer_size_ : // fill the buffer up all the way
#if NTDDI_VERSION >= NTDDI_WINTHRESHOLD
        (frames_ > 0) ? frames_ : // fill up exactly one period
#endif
        buffer_size_ / 2 // use half-the-buffer as the best approximation of one period
    );

    // if it's already full, this is a no-op
    if (padding >= fill_up_to) { return S_FALSE; }

    UINT need_frames = fill_up_to - padding;

    BYTE *data = nullptr;
    hr = audio_render_client_->GetBuffer(need_frames, &data);
    if (FAILED(hr)) {
        ERR(L"IAudioRenderClient::GetBuffer returned failure: 0x%08x", hr);
        return hr;
    }
    if (silence) {
        hr = audio_render_client_->ReleaseBuffer(
            need_frames, AUDCLNT_BUFFERFLAGS_SILENT);
        if (FAILED(hr)) {
            ERR(L"IAudioRenderClient::ReleaseBuffer returned failure: 0x%08x", hr);
            return hr;
        }
    } else {
        for (auto &i : callbacks_) {
            i.second(data, need_frames, mix_format_->nSamplesPerSec,
                     mix_format_->wBitsPerSample, mix_format_->nChannels, 0,
                     system_timestamp);
        }
        hr = audio_render_client_->ReleaseBuffer(need_frames, 0);
        if (FAILED(hr)) {
            ERR(L"IAudioRenderClient::ReleaseBuffer returned failure: 0x%08x", hr);
            return hr;
        }
    }
    return S_OK;
}


HRESULT WASAPIRender::StopPlaying() {
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

HRESULT WASAPIRender::ActivateCompleted(
    IActivateAudioInterfaceAsyncOperation *operation) {

    HRESULT hr = S_OK;
    HRESULT hr_activate_result = S_OK;
    IUnknown *audio_interface = nullptr;
    AudioClientProperties client_properties = {0};
    client_properties.cbSize = sizeof(client_properties);
    client_properties.bIsOffload = false;
    client_properties.Options =
        (raw_) ? AUDCLNT_STREAMOPTIONS_RAW : AUDCLNT_STREAMOPTIONS_NONE;

    hr = AudioStreamCategoryFromAudioRenderCategory(audio_render_category_, &client_properties.eCategory);
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

    LOG(L"Render sample rate is %u", mix_format_->nSamplesPerSec);

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
                &dummy2,// don't care about the fundamental
                &minimumPeriod, 
                &dummy3 // don't care about the max
                );
            if (FAILED(hr)) {
                ERR(L"IAudioClient3::GetSharedModeEnginePeriodicity failed: 0x%08x", hr);
                goto exit;
            }

            frames_ = minimumPeriod;

            LOG(L"Minimum render period is %d frames", frames_);
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
    LOG(L"First render buffer_size after init is %u", buffer_size_);
    hr = audio_client_->GetService(IID_PPV_ARGS(&audio_render_client_));
    if (FAILED(hr)) {
        ERR(L"IAudioClient::GetService(IAudioRenderClient) failed: 0x%08x", hr);
        goto exit;
    }
    hr = MFCreateAsyncResult(nullptr, &m_xNeedBuffer, nullptr,
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

HRESULT WASAPIRender::InitializeAudioDeviceAsync(PCWSTR device_id) {

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
    HRESULT hr = ActivateAudioInterfaceAsync(device_id, __uuidof(*audio_client_.Get()),
                                     nullptr, this, &asyncop);
    return hr;
}

void WASAPIRender::AddCallback(
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
    mutex_.unlock();
}

void WASAPIRender::RemoveCallback(const std::string &name) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto found = callbacks_.find(name);
    if (found == callbacks_.end()) {
        return;
    }
    callbacks_.erase(found);
}
WASAPIRender::~WASAPIRender() {
    if (mix_format_ != nullptr) {
        CoTaskMemFree(mix_format_);
    }
}