// endpoint-id.cpp

// Windows
#include <objbase.h>
#include <ntassert.h>
#define ASSERT(x) NT_ASSERT(x)
#if DBG
#define VERIFY(x) ASSERT(x)
#else
#define VERIFY(x) (x)
#endif

#include <atlbase.h>
#include <strsafe.h>
#include <propidl.h>

#include <audioclient.h>

// CRT
#include <stdio.h>

// SDKTOOLS
#include <basiclog.h>

// AVCORE
#include <mmdeviceapi.h>
#include <mmsystem.h>
#include <DevPKey.h>

// MMtest
#include <basicprintf.h>
#include <BasicLogHelper.h>
#include <doonexit.h>

#include "endpoint-id.h"
#include "log.h"

#define PROPERTYKEY_FROM_DEVPROPKEY(dpk) { dpk.fmtid, dpk.pid }

HRESULT FindWasapiEndpointId(EDataFlow flow, LPCWSTR in, _Outptr_ LPWSTR *out) {
    CComPtr<IMMDeviceEnumerator> pEnumerator;
    HRESULT hr = pEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
    if (FAILED(hr)) {
        ERR(L"CoCreateInstance(MMDeviceEnumerator) failed: hr = 0x%08x", hr);
        return hr;
    }
    
    bool bUseDefault = true;
    ERole role = eConsole;
    
    if (0 == _wcsicmp(L"", in)) {
        role = eConsole;
    } else if (0 == _wcsicmp(L"console", in)) {
        role = eConsole;
    } else if (0 == _wcsicmp(L"communications", in)) {
        role = eCommunications;
    } else if (0 == _wcsicmp(L"multimedia", in)) {
        role = eMultimedia;
    } else {
        bUseDefault = false;
    }
    
    if (bUseDefault) {
        if (eAll == flow) {
            ERR(L"Don't know whether to query the default render or capture device");
            return E_INVALIDARG;
        }
        
        CComPtr<IMMDevice> pMMDevice;
        hr = pEnumerator->GetDefaultAudioEndpoint(flow, role, &pMMDevice);
        if (FAILED(hr)) {
            ERR(L"IMMDeviceEnumerator::GetDefaultAudioEndpoint failed: hr = 0x%08x", hr);
            return hr;
        }
        
        hr = pMMDevice->GetId(out);
        if (FAILED(hr)) {
            ERR(L"IMMDevice::GetId failed: hr = 0x%08x", hr);
            return hr;
        }
        
        return S_OK;
    }

    // enumerate devices and see
    // if it's a WASAPI endpoint ID
    // or a PKEY_AudioEndpoint_GUID
    // or a friendly name

    CComPtr<IMMDeviceCollection> pMMDeviceCollection;
    hr = pEnumerator->EnumAudioEndpoints(flow, DEVICE_STATE_ACTIVE, &pMMDeviceCollection);
    if (FAILED(hr)) {
        ERR(L"IMMDeviceEnumerator::EnumAudiEndpoints failed: hr = 0x%08x", hr);
        return hr;
    }
    
    UINT nCount;
    hr = pMMDeviceCollection->GetCount(&nCount);
    if (FAILED(hr)) {
        ERR(L"IMMDeviceCollection::GetCount failed: hr = 0x%08x", hr);
        return hr;
    }
    
    CComHeapPtr<WCHAR> szId;
    for (UINT i = 0; i < nCount; i++) {
        CComPtr<IMMDevice> pMMDevice;
        hr = pMMDeviceCollection->Item(i, &pMMDevice);
        if (FAILED(hr)) {
            ERR(L"IMMDeviceCollection::Item failed: hr = 0x%08x", hr);
            return hr;
        }
        
        // see if it's a WASAPI ID
        CComHeapPtr<WCHAR> szTempId;
        hr = pMMDevice->GetId(&szTempId);
        if (FAILED(hr)) {
            ERR(L"IMMDevice::GetId failed: hr = 0x%08x", hr);
            return hr;
        }
        
        if (0 == _wcsicmp(in, szTempId)) {
            if (nullptr != szId) {
                ERR(L"At least two \"%s\" devices found", static_cast<LPCWSTR>(szId));
                return E_INVALIDARG;
            }
            
            // got it!
            szId.Attach(szTempId.Detach());
            continue;
        }
        
        // see if it's a known property
        PROPERTYKEY properties[] = {
            PKEY_AudioEndpoint_GUID,
            PROPERTYKEY_FROM_DEVPROPKEY(DEVPKEY_Device_FriendlyName),
        };
        CComPtr<IPropertyStore> pPropertyStore;
        hr = pMMDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);
        if (FAILED(hr)) {
            ERR(L"IMMDevice::OpenPropertyStore(STGM_READ) failed: hr = 0x%08x", hr);
            return hr;
        }
        
        for (int j = 0; j < ARRAYSIZE(properties); j++) {        
            PROPVARIANT var; PropVariantInit(&var);
            DoOnExit<PROPVARIANT*> doeClearPropVariant(
                PropVariantClear, Success_NonNegative,
                &var, g_pLog, L"Clearing PropVariant"
            );
            hr = pPropertyStore->GetValue(properties[j], &var);
            if (FAILED(hr)) {
                ERR(L"IPropertyStore::GetValue(...) failed: hr = 0x%08x", hr);
                return hr;
            }

            if (VT_LPWSTR != var.vt) {
                ERR(L"Property is of unexpected vartype %u instead of VT_LPWSTR", var.vt);
                return HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
            }
        
            if (0 == _wcsicmp(in, var.pwszVal)) {
                if (nullptr != szId) {
                    ERR(L"At least two \"%s\" devices found", var.pwszVal);
                    return E_INVALIDARG;
                }
                
                // got it!
                szId.Attach(szTempId.Detach());
                break; // out of property loop
            }
        } // for each property
    } // for each device
    
    if (nullptr == szId) {
        ERR(L"Device \"%s\" not found", in);
        return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    }
    
    *out = szId.Detach();
    return S_OK;
}