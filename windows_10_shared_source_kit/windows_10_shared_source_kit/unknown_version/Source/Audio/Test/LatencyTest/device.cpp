// device.cpp
//
// Copyright (C) Microsoft. All rights reserved.


#include "pch.h"
#include "device.h"
#include <wrl/event.h>

using namespace Microsoft::WRL;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Media::Effects;

// macros
#define CHECK_HR_RETURN(hr)        \
    {                              \
        if (FAILED(hr)) return hr; \
    }
#define CHECK_HR_EXIT(hr)          \
    {                              \
        if (FAILED(hr)) goto exit; \
    }
#define SAFE_RELEASE(x) \
    { SafeRelease(&x); }
template <class T>
void SafeRelease(T **ppT) {
    if (*ppT) {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

template <class T>
class MyIAsyncOperationCompletedHandler
    : public IAsyncOperationCompletedHandler<T> {
    LONG c_ref_;
    HANDLE op_completed_;

   public:
    MyIAsyncOperationCompletedHandler() : c_ref_(1) {
        op_completed_ =
            CreateEventExW(NULL, NULL, 0, SYNCHRONIZE | EVENT_MODIFY_STATE);
    }
    STDMETHODIMP QueryInterface(REFIID riid, void **ppv) {
        if (__uuidof(IAsyncOperationCompletedHandler) == riid) {
            AddRef();
            *ppv = static_cast<IAsyncOperationCompletedHandler *>(this);
        } else if (__uuidof(IUnknown) == riid) {
            AddRef();
            *ppv = static_cast<IUnknown *>(this);
        } else {
            *ppv = NULL;
            return E_NOINTERFACE;
        }
        return S_OK;
    }

    STDMETHODIMP_(ULONG) AddRef() { return InterlockedIncrement(&c_ref_); }

    STDMETHODIMP_(ULONG) Release() {
        LONG c_ref = InterlockedDecrement(&c_ref_);
        if (c_ref == 0) {
            delete this;
        }
        return c_ref;
    }

    bool Wait(unsigned int timeout) {
        if (op_completed_) {
            DWORD result = WaitForSingleObjectEx(op_completed_, timeout, FALSE);
            return result != WAIT_TIMEOUT;
        } else {
            return false;
        }
    }

   protected:
    void Signal() {
        if (op_completed_) {
            SetEvent(op_completed_);
        }
    }
    virtual ~MyIAsyncOperationCompletedHandler() { CloseHandle(op_completed_); }
};

template <class T>
class ReadyHandler : public MyIAsyncOperationCompletedHandler<T *> {
   public:
    ReadyHandler() : completion_status_(AsyncStatus::Started) {}
    __override IFACEMETHOD(Invoke)(IAsyncOperation<T *> *,
                                   AsyncStatus status) {
        completion_status_ = status;
        Signal();

        return S_OK;
    }

    AsyncStatus completion_status_;
};

template <class T>
class AsyncHandler {
    ComPtr<ABI::Windows::Foundation::IAsyncOperation<T *>> async_op_;
    ReadyHandler<T> *ready_handler_;

   public:
    ComPtr<ABI::Windows::Foundation::IAsyncOperation<T *>> &get() {
        return async_op_;
    }
    template <class Y>
    bool getResult(ComPtr<Y> &result, int timeout) {
        ready_handler_ = new ReadyHandler<T>;
        HRESULT hr = async_op_->put_Completed(ready_handler_);
        if (hr != S_OK) {
            ready_handler_->Release();
            return false;
        }
        ready_handler_->Wait(timeout);
        if (ready_handler_->completion_status_ == AsyncStatus::Completed) {
            hr = async_op_->GetResults(&result);
            ready_handler_->Release();
            return hr == S_OK;
        }
        if (ready_handler_->completion_status_ == AsyncStatus::Started) {
            ComPtr<IAsyncInfo> async_info;
            async_op_.As(&async_info);
            async_info->Cancel();
            ready_handler_->Wait(1000);
            if (ready_handler_->completion_status_ != AsyncStatus::Canceled) {
            }
        }
        ready_handler_->Release();
        return false;
    }
};
#pragma warning(disable : 4512)
class PropView
    : public ABI::Windows::Foundation::Collections::IIterable<HSTRING> {
    ULONG c_ref_;
    const std::vector<std::wstring> &props;

   public:
    virtual HRESULT STDMETHODCALLTYPE
    QueryInterface(REFIID,
                   _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppv) {
        if (nullptr != ppv) {
            *ppv = nullptr;
        }
        return E_NOTIMPL;
    }
    STDMETHODIMP_(ULONG) AddRef() { return InterlockedIncrement(&c_ref_); }

    STDMETHODIMP_(ULONG) Release() {
        LONG c_ref = InterlockedDecrement(&c_ref_);
        if (c_ref == 0) {
            delete this;
        }
        return c_ref;
    }
    virtual HRESULT STDMETHODCALLTYPE
    GetIids(__RPC__out ULONG *iidCount,
            __RPC__deref_out_ecount_full_opt(*iidCount) IID **ppIids) {
        UNREFERENCED_PARAMETER(iidCount);
        if (nullptr != ppIids) {
            *ppIids = nullptr;
        }
        return E_NOTIMPL;
    }

    virtual HRESULT STDMETHODCALLTYPE
    GetRuntimeClassName(__RPC__deref_out_opt HSTRING *) {
        return E_NOTIMPL;
    }

    virtual HRESULT STDMETHODCALLTYPE
    GetTrustLevel(__RPC__out TrustLevel *) {
        return E_NOTIMPL;
    }
    virtual HRESULT STDMETHODCALLTYPE First(
        _Outptr_result_maybenull_
            ABI::Windows::Foundation::Collections::IIterator<HSTRING> **first);

    explicit PropView(const std::vector<std::wstring> &props)
        : c_ref_(1), props(props) {}
    const std::vector<std::wstring> &getProps() const { return props; }
};

class PropViewIterator
    : public ABI::Windows::Foundation::Collections::IIterator<HSTRING> {
    const PropView &iterable_;
    size_t index_;
    ULONG c_ref_;
    HSTRING prop_cache_;

   public:
    virtual HRESULT STDMETHODCALLTYPE
    QueryInterface(REFIID ,
                   _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppv) {
        if (nullptr != ppv) {
            *ppv = nullptr;
        }
        return E_NOTIMPL;
    }
    STDMETHODIMP_(ULONG) AddRef() { return InterlockedIncrement(&c_ref_); }

    STDMETHODIMP_(ULONG) Release() {
        LONG c_ref = InterlockedDecrement(&c_ref_);
        if (c_ref == 0) {
            delete this;
        }
        return c_ref;
    }
    virtual HRESULT STDMETHODCALLTYPE
    GetIids(__RPC__out ULONG *iidCount,
            __RPC__deref_out_ecount_full_opt(*iidCount) IID **ppIids) {
        UNREFERENCED_PARAMETER(iidCount);
        if (nullptr != ppIids) {
            *ppIids = nullptr;
        }
        return E_NOTIMPL;
    }

    virtual HRESULT STDMETHODCALLTYPE
    GetRuntimeClassName(__RPC__deref_out_opt HSTRING *) {
        return E_NOTIMPL;
    }

    virtual HRESULT STDMETHODCALLTYPE
    GetTrustLevel(__RPC__out TrustLevel *) {
        return E_NOTIMPL;
    }

    virtual HRESULT STDMETHODCALLTYPE get_Current(_Out_ HSTRING *current) {
        HRESULT hr = S_OK;
        if (index_ < iterable_.getProps().size()) {
            hr = WindowsCreateString(
                (wchar_t *)iterable_.getProps()[index_].c_str(),
                (UINT)iterable_.getProps()[index_].length(), &prop_cache_);
            *current = prop_cache_;
        } else {
            *current = 0;
        }
        return hr;
    }
    virtual HRESULT STDMETHODCALLTYPE
    get_HasCurrent(_Out_ boolean *hasCurrent) {
        *hasCurrent = (index_ < iterable_.getProps().size()) ? true : false;
        return S_OK;
    }
    virtual HRESULT STDMETHODCALLTYPE MoveNext(_Out_ boolean *hasCurrent) {
        if (index_ < iterable_.getProps().size()) {
            index_++;
        }
        *hasCurrent = (index_ < iterable_.getProps().size()) ? true : false;
        return S_OK;
    }
    virtual HRESULT STDMETHODCALLTYPE
    GetMany(_In_ unsigned capacity,
            _Out_writes_to_(capacity, *actual) HSTRING *,
            _Out_ unsigned *actual) {
        UNREFERENCED_PARAMETER(capacity);
        UNREFERENCED_PARAMETER(actual);
        return E_NOTIMPL;
    }

    explicit PropViewIterator(const PropView &iterable)
        : iterable_(iterable), index_(0), c_ref_(1), prop_cache_(0) {}
    ~PropViewIterator() { WindowsDeleteString(prop_cache_); }
};

HRESULT STDMETHODCALLTYPE PropView::First(
    ABI::Windows::Foundation::Collections::IIterator<HSTRING> **first) {
    *first = reinterpret_cast<
        ABI::Windows::Foundation::Collections::IIterator<HSTRING> *>(
        new PropViewIterator(*this));
    return S_OK;
}

HSTRING GetDefaultDevice(
    ABI::Windows::Media::Devices::AudioDeviceRole role,
    bool capture
) {
    HRESULT hr S_OK;
    HSTRING device_id = nullptr;
    ComPtr<ABI::Windows::Media::Devices::IMediaDeviceStatics> media_device;
    hr = Windows::Foundation::GetActivationFactory(
        Wrappers::HStringReference(
            RuntimeClass_Windows_Media_Devices_MediaDevice).Get(),
        &media_device);
    if (FAILED(hr)) {
        return device_id;
    }
    if (capture) {
        hr = media_device->GetDefaultAudioCaptureId(
            role,
            &device_id);
    } else {
        hr = media_device->GetDefaultAudioRenderId(
            role,
            &device_id);
    }
    if (FAILED(hr)) {
        return device_id;
    }
    return device_id;
}
std::string AudioEffectTypeString(
    ABI::Windows::Media::Effects::AudioEffectType type) {
    switch (type) {
        case AudioEffectType_Other:
            return "Other";
        case AudioEffectType_AcousticEchoCancellation:
            return "AcousticEchoCancellation";
        case AudioEffectType_NoiseSuppression:
            return "NoiseSuppression";
        case AudioEffectType_AutomaticGainControl:
            return "AutomaticGainControl";
        case AudioEffectType_BeamForming:
            return "BeamForming";
        case AudioEffectType_ConstantToneRemoval:
            return "ConstantToneRemoval";
        case AudioEffectType_Equalizer:
            return "Equalizer";
        case AudioEffectType_LoudnessEqualizer:
            return "LoudnessEqualizer";
        case AudioEffectType_BassBoost:
            return "BassBoost";
        case AudioEffectType_VirtualSurround:
            return "VirtualSurround";
        case AudioEffectType_VirtualHeadphones:
            return "VirtualHeadphones";
        case AudioEffectType_SpeakerFill:
            return "SpeakerFill";
        case AudioEffectType_RoomCorrection:
            return "RoomCorrection";
        case AudioEffectType_BassManagement:
            return "BassManagement";
        case AudioEffectType_EnvironmentalEffects:
            return "EnvironmentalEffects";
        case AudioEffectType_SpeakerProtection:
            return "SpeakerProtection";
        case AudioEffectType_SpeakerCompensation:
            return "SpeakerCompensation";
        case AudioEffectType_DynamicRangeCompression:
            return "DynamicRangeCompression";
        default:
            return "Unknown";
    }
}

HRESULT GetFriendlyName(HSTRING device_id, HSTRING& name) {
    ComPtr<ABI::Windows::Devices::Enumeration::IDeviceInformationStatics>
        device_information_methods;
    CHECK_HR_RETURN(Windows::Foundation::GetActivationFactory(
        Microsoft::WRL::Wrappers::HStringReference(
            RuntimeClass_Windows_Devices_Enumeration_DeviceInformation).Get(),
        &device_information_methods));
    AsyncHandler<ABI::Windows::Devices::Enumeration::DeviceInformation>
        async_op;
    CHECK_HR_RETURN(
        device_information_methods->CreateFromIdAsync(device_id, &async_op.get()));
    ComPtr<ABI::Windows::Devices::Enumeration::IDeviceInformation> device_info;
    if (async_op.getResult(device_info, 3000) == false) {
        return S_FALSE;
    }
    CHECK_HR_RETURN(device_info->get_Name(&name));
    return S_OK;
}

HRESULT DeviceSupportsRaw(HSTRING device_id, bool &raw_supported) {
    HRESULT hr = S_OK;
    raw_supported = false;
    const std::wstring raw_property_key =
        L"{8943B373-388C-4395-B557-BC6DBAFFAFDB} 2";

    ComPtr<ABI::Windows::Devices::Enumeration::IDeviceInformationStatics>
        device_information_methods;
    CHECK_HR_RETURN(Windows::Foundation::GetActivationFactory(
        Microsoft::WRL::Wrappers::HStringReference(
            RuntimeClass_Windows_Devices_Enumeration_DeviceInformation).Get(),
        &device_information_methods));

    std::vector<std::wstring> properties;
    properties.push_back(raw_property_key);
    PropView prop_view(properties);
    AsyncHandler<ABI::Windows::Devices::Enumeration::DeviceInformation>
        async_op;
    CHECK_HR_RETURN(
        device_information_methods->CreateFromIdAsyncAdditionalProperties(
            device_id, &prop_view, &async_op.get()));
    ComPtr<ABI::Windows::Devices::Enumeration::IDeviceInformation> device_info;
    if (async_op.getResult(device_info, 3000) == false) {
        return S_FALSE;
    }
    ComPtr<ABI::Windows::Foundation::Collections::IMapView<HSTRING,
                                                           IInspectable *>> dip;
    CHECK_HR_RETURN(device_info->get_Properties(&dip));
    ComPtr<ABI::Windows::Foundation::IPropertyValue> raw_processing_value;
    CHECK_HR_RETURN(dip->Lookup(Microsoft::WRL::Wrappers::HStringReference(
                                    raw_property_key.c_str()).Get(),
                                &raw_processing_value));
    if (raw_processing_value.Get()) {
        boolean raw_support;
        hr = raw_processing_value->GetBoolean(&raw_support);
        if (hr == S_OK) {
            raw_supported = raw_support ? true : false;
        }
    }
    return hr;
}

HSTRING GetDefaultRenderDevice(ABI::Windows::Media::Devices::AudioDeviceRole role) { return GetDefaultDevice(role, false); }
HSTRING GetDefaultCaptureDevice(ABI::Windows::Media::Devices::AudioDeviceRole role) { return GetDefaultDevice(role, true); }

HRESULT GetEffectsRenderDevice(HSTRING device_id,
                               std::vector<std::string> &effects_lists,
                               ABI::Windows::Media::Render::AudioRenderCategory category,
                               ABI::Windows::Media::AudioProcessing processing) {
    HRESULT hr S_OK;
    ComPtr<IAudioEffectsManagerStatics> audio_effects_manager_methods;
    ComPtr<ABI::Windows::Foundation::Collections::IVectorView<
        ABI::Windows::Media::Effects::AudioEffect * >> effects;
    CHECK_HR_RETURN(GetActivationFactory(
        Wrappers::HStringReference(
        RuntimeClass_Windows_Media_Effects_AudioEffectsManager).Get(),
        &audio_effects_manager_methods));
    ComPtr<IAudioRenderEffectsManager> render_effects_manager;
    CHECK_HR_RETURN(
        audio_effects_manager_methods->CreateAudioRenderEffectsManagerWithMode(
        device_id,
        category,
        processing,
        &render_effects_manager));
    CHECK_HR_RETURN(
        render_effects_manager->GetAudioRenderEffects(&effects));
    if (!effects.Get()) {
        return hr;
    }
    unsigned int listsize;
    CHECK_HR_RETURN(effects->get_Size(&listsize));
    for (unsigned int i = 0; i < listsize; i++) {
        ComPtr<ABI::Windows::Media::Effects::IAudioEffect> effect;
        if (effects->GetAt(i, &effect) != S_OK || !effect) {
            continue;
        }
        ABI::Windows::Media::Effects::AudioEffectType type;
        if (effect->get_AudioEffectType(&type) != S_OK) {
            continue;
        }
        effects_lists.push_back(AudioEffectTypeString(type));
    }
    return hr;
}
HRESULT GetEffectsCaptureDevice(HSTRING device_id,
                                std::vector<std::string> &effects_lists,
                                ABI::Windows::Media::Capture::MediaCategory category,
                                ABI::Windows::Media::AudioProcessing processing) {
    HRESULT hr S_OK;
    ComPtr<IAudioEffectsManagerStatics> audio_effects_manager_methods;
    ComPtr<ABI::Windows::Foundation::Collections::IVectorView<
        ABI::Windows::Media::Effects::AudioEffect * >> effects;
    CHECK_HR_RETURN(GetActivationFactory(
        Wrappers::HStringReference(
        RuntimeClass_Windows_Media_Effects_AudioEffectsManager).Get(),
        &audio_effects_manager_methods));
    ComPtr<IAudioCaptureEffectsManager> capture_effects_manager;
    CHECK_HR_RETURN(
        audio_effects_manager_methods->CreateAudioCaptureEffectsManagerWithMode(
        device_id,
        category,
        processing,
        &capture_effects_manager));
    CHECK_HR_RETURN(
        capture_effects_manager->GetAudioCaptureEffects(&effects));
    unsigned int listsize;
    CHECK_HR_RETURN(effects->get_Size(&listsize));
    for (unsigned int i = 0; i < listsize; i++) {
        ComPtr<ABI::Windows::Media::Effects::IAudioEffect> effect;
        if (effects->GetAt(i, &effect) != S_OK || !effect) {
            continue;
        }
        ABI::Windows::Media::Effects::AudioEffectType type;
        if (effect->get_AudioEffectType(&type) != S_OK) {
            continue;
        }
        effects_lists.push_back(AudioEffectTypeString(type));
    }
    return hr;
}
