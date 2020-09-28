// wasapirender.h
//
// Copyright (C) Microsoft. All rights reserved.

#ifndef WASAPIRENDER_H
#define WASAPIRENDER_H

#include "timestamp.h"

// windows includes
#include <wrl\implements.h>
#include <AudioClient.h>
#include <mmdeviceapi.h>
#include <mfapi.h>

using namespace Microsoft::WRL;

class WASAPIRender
    : public RuntimeClass<RuntimeClassFlags<ClassicCom>, FtmBase,
                          IActivateAudioInterfaceCompletionHandler> {
   public:
    HRESULT InitializeAudioDeviceAsync(PCWSTR);  // PCWSTR is the device ID
    HRESULT Play();
    HRESULT StopPlaying();
    STDMETHOD(ActivateCompleted)(IActivateAudioInterfaceAsyncOperation*);
    void AddCallback(const std::string& name,
                     std::function<void(unsigned char* buffer, int buffer_size,
                                        int sampling_rate, int bit_depth,
                                        int channels, uint64_t audio_timestamp,
                                        uint64_t system_timestamp)> callback);
    void RemoveCallback(const std::string& name);
    WASAPIRender(RoundtripRenderOptions options);

   private:
    ~WASAPIRender();
    bool initialized_;
    bool started_;
    bool raw_;
    bool pcm_;
    bool fill_buffer_;
#if NTDDI_VERSION >= NTDDI_WINTHRESHOLD
    int frames_;
#endif
    WAVEFORMATEX* mix_format_;
    HRESULT StartPlayback(IMFAsyncResult*);
    HRESULT NeedBuffer(IMFAsyncResult*);
    HRESULT AudioBufferRequested(bool);
    IMFAsyncResult* sample_ready_async_result_;
    MFWORKITEM_KEY sample_ready_key_;
    DWORD dw_queue_id_;
    ComPtr<IAudioRenderClient> audio_render_client_;
#if NTDDI_VERSION >= NTDDI_WINTHRESHOLD
    ComPtr<IAudioClient3> audio_client_;
#else
    ComPtr<IAudioClient2> audio_client_;
#endif
    UINT32 buffer_size_;
    HANDLE sample_ready_event_;
    HANDLE shutdown_thread_;
    HANDLE audio_client_activated_;
    std::mutex mutex_;
    std::unique_ptr<TimeStamp> timer_;
    uint64_t prev_timer;
    ABI::Windows::Media::Render::AudioRenderCategory audio_render_category_;
    std::unordered_map<
        std::string, std::function<void(unsigned char* buffer, int buffer_size,
                                        int sampling_rate, int bit_depth,
                                        int channels, uint64_t audio_timestamp,
                                        uint64_t system_timestamp)>> callbacks_;

#ifndef METHODASYNCCALLBACK
#define METHODASYNCCALLBACK(Parent, AsyncCallback, pfnCallback)             \
    class Callback##AsyncCallback : public IMFAsyncCallback {               \
       public:                                                              \
        Callback##AsyncCallback()                                           \
            : _parent((reinterpret_cast<Parent*>(                           \
                  (reinterpret_cast<BYTE*>(this) -                          \
                   offsetof(Parent, m_x##AsyncCallback))))),                \
              _dwQueueID(MFASYNC_CALLBACK_QUEUE_MULTITHREADED) {}           \
                                                                            \
        STDMETHOD_(ULONG, AddRef)() { return _parent->AddRef(); }           \
        STDMETHOD_(ULONG, Release)() { return _parent->Release(); }         \
        STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject) {          \
            if (riid == IID_IMFAsyncCallback || riid == IID_IUnknown) {     \
                (*ppvObject) = this;                                        \
                AddRef();                                                   \
                return S_OK;                                                \
            }                                                               \
            *ppvObject = NULL;                                              \
            return E_NOINTERFACE;                                           \
        }                                                                   \
        STDMETHOD(GetParameters)(                                           \
            /* [out] */ __RPC__out DWORD* pdwFlags,                         \
            /* [out] */ __RPC__out DWORD* pdwQueue) {                       \
            *pdwFlags = 0;                                                  \
            *pdwQueue = _dwQueueID;                                         \
            return S_OK;                                                    \
        }                                                                   \
        STDMETHOD(Invoke)(/* [out] */ __RPC__out IMFAsyncResult* pResult) { \
            _parent->pfnCallback(pResult);                                  \
            return S_OK;                                                    \
        }                                                                   \
        void SetQueueID(DWORD dwQueueID) { _dwQueueID = dwQueueID; }        \
                                                                            \
       protected:                                                           \
        Parent* _parent;                                                    \
        DWORD _dwQueueID;                                                   \
                                                                            \
    } m_x##AsyncCallback;
#endif  // METHODASYNCCALLBACK
    METHODASYNCCALLBACK(WASAPIRender, NeedBuffer, NeedBuffer);
    METHODASYNCCALLBACK(WASAPIRender, StartPlayback, StartPlayback);
};
#endif  // WASAPIRENDER_H
