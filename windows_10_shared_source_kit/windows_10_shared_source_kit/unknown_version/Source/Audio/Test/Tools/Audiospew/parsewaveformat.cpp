// parsewaveformat.cpp

// Windows
#include <objbase.h>

#include <atlbase.h>
#include <atlsync.h>
#include <strsafe.h>
#include <propidl.h>

#include <audioclient.h>

// CRT
#include <stdio.h>
#include <math.h>

// AVCORE
#include <mmdeviceapi.h>
#include <mmsystem.h>
#include <DevPKey.h>
#include <DSound.h>

// Renderlib
#include <audiostreaming.h>

// MMtest
#include <debug.h>
#include <basiclog.h>
#include <basicprintf.h>
#include <coinitialize.h>
#include <BasicLogHelper.h>
#include <helperclasses/wfx.h>
#include <helperclasses/cptr.h>
#include <waveio.h>
#include <doonexit.h>

#include "endpoint-id.h"
#include "log.h"
#include "parse.h"

_Success_( return != 0)
bool GetMMDevice(EDataFlow flow, LPCWSTR endpoint, _Out_ IMMDevice **ppMMDevice);

HRESULT string_to_ul(LPCWSTR szString, _Out_ PDWORD pNum);
HRESULT string_to_binary(LPCWSTR szString, _Outptr_result_buffer_(*pnBytes) PBYTE *ppByte, _Out_ int *pnBytes);
bool IsHex(WCHAR t);
BYTE HexValue(WCHAR t);

#define BITS_PER_BYTE 8

_Success_(return == true)
bool ParseWaveFormat(
    EDataFlow flow,
    LPCWSTR endpoint,
    _Inout_ int *pIndex,
    int argc,
    _In_reads_(argc) LPCWSTR argv[],
    _Out_ WAVEFORMATEX **ppWfx
) {
    // syntactic sugar...
    int &i = *pIndex;
    
    // at start i points to the -format argument; we need at least one parameter
    if (++i >= argc) {
        ERR(L"-format needs an argument");
        return false;
    }
    
    // software-mix
    if (0 == _wcsicmp(argv[i], L"software-mix")) {
        LOG(L"Looking up the software mix format");
        CComPtr<IMMDevice> pMMDevice;
        if (!GetMMDevice(flow, endpoint, &pMMDevice)) {
            return false;
        }
        
        // activate an IAudioClient
        CComPtr<IAudioClient> pAudioClient;
        HRESULT hr = pMMDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient);
        if (FAILED(hr)) {
            ERR(L"IMMDevice::Activate(IAudioClient) failed: hr = 0x%08x", hr);
            return false;
        }
        
        // query the mix format
        hr = pAudioClient->GetMixFormat(ppWfx);
        if (FAILED(hr)) {
            ERR(L"IAudioClient::GetMixFormat failed: hr = 0x%08x", hr);
            return false;
        }
        
        return true;
    }
    
    // offload-mix
    if (0 == _wcsicmp(argv[i], L"offload-mix")) {
        LOG(L"Looking up the offload mix format");
        CComPtr<IMMDevice> pMMDevice;
        if (!GetMMDevice(flow, endpoint, &pMMDevice)) {
            return false;
        }
        
        // activate an IAudioClient2
        CComPtr<IAudioClient2> pAudioClient2;
        HRESULT hr = pMMDevice->Activate(__uuidof(IAudioClient2), CLSCTX_ALL, NULL, (void**)&pAudioClient2);
        if (FAILED(hr)) {
            ERR(L"IMMDevice::Activate(IAudioClient2) failed: hr = 0x%08x", hr);
            return false;
        }
        
        // put it into offload mode
        AudioClientProperties clientProperties = { 0 };
        clientProperties.cbSize = sizeof(clientProperties);
        clientProperties.bIsOffload = TRUE;
        clientProperties.eCategory = AudioCategory_Movie;
        
        hr = pAudioClient2->SetClientProperties(&clientProperties);
        if (FAILED(hr)) {
            ERR(L"IAudioClient2::SetClientProperties failed: hr = 0x%08x", hr);
            return false;
        }
        
        // query the mix format
        hr = pAudioClient2->GetMixFormat(ppWfx);
        if (FAILED(hr)) {
            ERR(L"IAudioClient2::GetMixFormat failed: hr = 0x%08x", hr);
            return false;
        }
        
        return true;
    }

    // device
    if (0 == _wcsicmp(argv[i], L"device")) {
        LOG(L"Looking up the device format");
        CComPtr<IMMDevice> pMMDevice;
        if (!GetMMDevice(flow, endpoint, &pMMDevice)) {
            return false;
        }
        
        // open the property store for read
        CComPtr<IPropertyStore> pPropertyStore;
        HRESULT hr = pMMDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);
        if (FAILED(hr)) {
            ERR(L"IMMDevice::OpenPropertyStore(STGM_READ) failed: hr = 0x%08x", hr);
            return false;
        }
        
        // see if we can read the device format from the property store first
        PROPVARIANT var; PropVariantInit(&var);
        DoOnExit<PROPVARIANT*> doeClearPropVariant(
            PropVariantClear, Success_NonNegative,
            &var, g_pLog, L"Clearing PropVariant"
        );
        hr = pPropertyStore->GetValue(PKEY_AudioEngine_DeviceFormat, &var);
        if (FAILED(hr)) {
            ERR(L"IPropertyStore::GetValue(PKEY_AudioEngine_DeviceFormat) failed: hr = 0x%08x", hr);
            return false;
        }
        
        switch (var.vt) {
            case VT_BLOB:
                // good, the property exists
                break;
        
            case VT_EMPTY:
                ERR(
                    "Device has no device format (yet.) "
                    "Try capturing in shared mode (or at least calling IAudioClient::GetMixFormat()) and then trying again."
                );
                return false;

            default:
                ERR(L"IPropertyStore::GetValue(PKEY_AudioEngine_DeviceFormat) returned unexpected variant type %u", var.vt);
                return false;
        }

        *ppWfx = reinterpret_cast<LPWAVEFORMATEX>(var.blob.pBlobData); // skipping a CoTaskMemAlloc()...
        doeClearPropVariant.Cancel(); // and a CoTaskMemFree()
        return true;
    }
    
    // ex [ int | float ] <channels> <sample-rate> <bits>
    if (0 == _wcsicmp(L"ex", argv[i])) {
        *ppWfx = (PWAVEFORMATEX)CoTaskMemAlloc(sizeof(WAVEFORMATEX));
        if (NULL == *ppWfx) {
            ERR(L"Could not allocate %Iu bytes for a WAVEFORMATEX", sizeof(WAVEFORMATEX));
            return false;
        }

        if (i + 4 >= argc) {
            ERR(L"-format ex needs four additional arguments");
            return false;
        }

        i++;
        if (0 == _wcsicmp(argv[i], L"int")) {
            (*ppWfx)->wFormatTag = WAVE_FORMAT_PCM;
        } else if (0 == _wcsicmp(argv[i], L"float")) {
            (*ppWfx)->wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
        } else {
            ERR(L"Expected \"int\" or \"float\" instead of %s", argv[i]);
            return false;
        }

        i++;
        DWORD dwChannels;
        HRESULT hr = string_to_ul(argv[i], &dwChannels);
        if (FAILED(hr)) {
            return false;
        }
        if (dwChannels > 0xffff) { // WORD_MAX
            ERR(L"Too many channels (%u) for a WORD", dwChannels);
            return false;
        }
        (*ppWfx)->nChannels = (WORD)dwChannels;

        i++;
        hr = string_to_ul(argv[i], &(*ppWfx)->nSamplesPerSec);
        if (FAILED(hr)) {
            return false;
        }

        i++;
        DWORD dwBits;
        hr = string_to_ul(argv[i], &dwBits);
        if (FAILED(hr)) {
            return false;
        }
        if (dwBits > 0xffff) { // WORD_MAX
            ERR(L"Too many bits per sample (%u) for a WORD", dwBits);
            return false;
        }
        (*ppWfx)->wBitsPerSample = (WORD)dwBits;

        DWORD dwBlockAlign = dwBits * dwChannels / 8;
        if (dwBlockAlign > 0xffff) { // WORD_MAX
            ERR(L"Block align (%u) doesn't fit in a WORD", dwBlockAlign);
            return false;
        }
        (*ppWfx)->nBlockAlign = (WORD)dwBlockAlign;

        UINT64 u64AvgBytesPerSec = (UINT64)(*ppWfx)->nSamplesPerSec * (UINT64)(*ppWfx)->nBlockAlign;
        if (u64AvgBytesPerSec > 0xffffffff) { // DWORD_MAX
            ERR(L"Average bytes per second (%I64u) doesn't fit in a DWORD", u64AvgBytesPerSec);
            return false;
        }
        (*ppWfx)->nAvgBytesPerSec = (DWORD)u64AvgBytesPerSec;

        (*ppWfx)->cbSize = 0;
        return true;
    }
    
    // ext [ int | float ] <channels> <sample-rate> <bits> <valid> <mask>
    if (0 == _wcsicmp(L"ext", argv[i])) {
        *ppWfx = (PWAVEFORMATEX)CoTaskMemAlloc(sizeof(WAVEFORMATEXTENSIBLE));
        if (NULL == *ppWfx) {
            ERR(L"Could not allocate %Iu bytes for a WAVEFORMATEXTENSIBLE", sizeof(WAVEFORMATEXTENSIBLE));
            return false;
        }
        PWAVEFORMATEXTENSIBLE wfxt = (PWAVEFORMATEXTENSIBLE)(*ppWfx);

        if (i + 6 >= argc) {
            ERR(L"-format ext needs six additional arguments");
            return false;
        }

        wfxt->Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;

        i++;
        if (0 == _wcsicmp(argv[i], L"int")) {
            wfxt->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
        } else if (0 == _wcsicmp(argv[i], L"float")) {
            wfxt->SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
        } else {
            ERR(L"Expected \"int\" or \"float\" instead of %s", argv[i]);
            return false;
        }

        i++;
        DWORD dwChannels;
        HRESULT hr = string_to_ul(argv[i], &dwChannels);
        if (FAILED(hr)) {
            return false;
        }
        if (dwChannels > 0xffff) { // WORD_MAX
            ERR(L"Too many channels (%u) for a WORD", dwChannels);
            return false;
        }
        wfxt->Format.nChannels = (WORD)dwChannels;

        i++;
        hr = string_to_ul(argv[i], &wfxt->Format.nSamplesPerSec);
        if (FAILED(hr)) {
            return false;
        }

        i++;
        DWORD dwBits;
        hr = string_to_ul(argv[i], &dwBits);
        if (FAILED(hr)) {
            return false;
        }
        if (dwBits > 0xffff) { // WORD_MAX
            ERR(L"Too many bits per sample (%u) for a WORD", dwBits);
            return false;
        }
        wfxt->Format.wBitsPerSample = (WORD)dwBits;

        i++;
        DWORD dwValidBits;
        hr = string_to_ul(argv[i], &dwValidBits);
        if (FAILED(hr)) {
            return false;
        }
        if (dwValidBits > 0xffff) { // WORD_MAX
            ERR(L"Too many valid bits per sample (%u) for a WORD", dwValidBits);
            return false;
        }
        wfxt->Samples.wValidBitsPerSample = (WORD)dwValidBits;
        if (dwValidBits > dwBits) {
            ERR(L"More valid bits (%u) than container bits (%u)", dwValidBits, dwBits);
            return false;
        }

        i++;
        hr = string_to_ul(argv[i], &wfxt->dwChannelMask);
        if (FAILED(hr)) {
            return false;
        }
        // channel mask can be 0 or have dwChannels bits set
        if (0 != wfxt->dwChannelMask) {
            DWORD bitsSet = 0;
            for (int j = 0; j < sizeof(wfxt->dwChannelMask) * BITS_PER_BYTE; j++) {
                if (wfxt->dwChannelMask & (1 << j)) {
                    bitsSet++;
                }
            }

            if (bitsSet != dwChannels) {
                ERR(
                    "Channel mask 0x%08x has %u bits set but there are %u channels",
                    wfxt->dwChannelMask, bitsSet, dwChannels
                );
                return false;
            }
        }

        DWORD dwBlockAlign = dwBits * dwChannels / 8;
        if (dwBlockAlign > 0xffff) { // WORD_MAX
            ERR(L"Block align (%u) doesn't fit in a WORD", dwBlockAlign);
            return false;
        }
        wfxt->Format.nBlockAlign = (WORD)dwBlockAlign;

        UINT64 u64AvgBytesPerSec = (UINT64)(*ppWfx)->nSamplesPerSec * (UINT64)(*ppWfx)->nBlockAlign;
        if (u64AvgBytesPerSec > 0xffffffff) { // DWORD_MAX
            ERR(L"Average bytes per second (%I64u) doesn't fit in a DWORD", u64AvgBytesPerSec);
            return false;
        }
        wfxt->Format.nAvgBytesPerSec = (DWORD)u64AvgBytesPerSec;

        wfxt->Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
        return true;
    }
    
    // raw
    if (0 == _wcsicmp(argv[i], L"raw")) {
        if (++i >= argc) {
            ERR(L"-format raw requires an argument");
            return false;
        }

        int nBytes;
        HRESULT hr = string_to_binary(argv[i], (PBYTE*)ppWfx, &nBytes);
        if (FAILED(hr)) {
            return false;
        }
        
        if (nBytes < 2) {
            ERR(L"Expected at least two bytes: got %d", nBytes);
            return false;
        }
        
        switch ((*ppWfx)->wFormatTag) {
            case WAVE_FORMAT_PCM:
                // must be at least sizeof(PCMWAVEFORMAT)
                if (nBytes < sizeof(PCMWAVEFORMAT)) {
                    ERR(L"WAVE_FORMAT_PCM formats must be at least sizeof(PCMWAVEFORMAT), not %d", nBytes);
                    return false;
                }
                break;
                
            case WAVE_FORMAT_EXTENSIBLE:
                // must be at least sizeof(WAVEFORMATEXTENSIBLE)
                if (nBytes < sizeof(WAVEFORMATEXTENSIBLE)) {
                    ERR(L"WAVE_FORMAT_EXTENSIBLE formats must be at least sizeof(WAVEFORMATEXTENSIBLE), not %d", nBytes);
                    return false;
                }
                
                if (nBytes != (int)sizeof(WAVEFORMATEX) + (*ppWfx)->cbSize) {
                    ERR(L"number of bytes does not match cbSize");
                    return false;
                }
                break;

            default:
                // must be at least sizeof(WAVEFORMATEX)
                if (nBytes < sizeof(WAVEFORMATEX)) {
                    ERR(L"Most formats must be at least sizeof(WAVEFORMATEX), not %d", nBytes);
                    return false;
                }
                
                if (nBytes != (int)sizeof(WAVEFORMATEX) + (*ppWfx)->cbSize) {
                    ERR(L"number of bytes does not match cbSize");
                    return false;
                }
        }

        return true;
    } // raw
    
    ERR(L"Unrecognized -format option %s", argv[i]);
    return false;
}

_Success_( return != 0)
bool GetMMDevice(EDataFlow flow, LPCWSTR endpoint, _Out_ IMMDevice **ppMMDevice) {
    CComHeapPtr<WCHAR> endpointId;
    HRESULT hr = FindWasapiEndpointId(flow, endpoint, &endpointId);
    if (FAILED(hr)) {
        return false;
    }
    
    CComPtr<IMMDeviceEnumerator> pEnumerator;
    hr = pEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
    if (FAILED(hr)) {
        ERR(L"CoCreateInstance(MMDeviceEnumerator) failed: hr = 0x%08x", hr);
        return false;
    }

    hr = pEnumerator->GetDevice(endpointId, ppMMDevice);
    if (FAILED(hr)) {
        ERR(L"IMMDeviceEnumerator::GetDevice(%ls) failed: hr = 0x%08x", endpoint, hr);
        return false;
    }
    
    return true;
}

HRESULT string_to_ul(LPCWSTR szString, _Out_ PDWORD pNum) {
    if (NULL == szString || NULL == pNum) {
        ERR(
            "At least one of szString (%p) or pNum (%p) is NULL",
            szString, pNum
        );
        return E_POINTER;
    }

    LPWSTR szEnd = L"";
    UINT32 nNum = wcstoul(szString, &szEnd, 0);
    if (0 != errno || L'\0' != *szEnd) {
        ERR(L"Error converting string to number: %s", szString);
        return E_INVALIDARG;
    }

    *pNum = nNum;
    return S_OK;
}

HRESULT string_to_binary(
    LPCWSTR szString,
    _Outptr_result_buffer_(*pnBytes) PBYTE *ppByte,
    _Out_ int *pnBytes
) {
    size_t nLen = wcslen(szString);
    size_t nHex = 0;

    // allow spaces and hex characters
    for (size_t i = 0; i < nLen; i++) {
        if (IsHex(szString[i])) {
            nHex++;
        } else if (L' ' != szString[i]) {
            ERR(L"Invalid character '%c' in binary string", szString[i]);
            return E_INVALIDARG;        
        }
    }

    if (0 == nHex) {
        ERR(L"No hex characters found");
        return E_INVALIDARG;
    }

    if (0 != (nHex % 2)) {
        ERR(L"Odd number (%Iu) of hex characters found", nHex);
        return E_INVALIDARG;
    }

    int nBytes = (int)nHex / 2;
    
    PBYTE pByte = (PBYTE)CoTaskMemAlloc(nBytes);
    if (NULL == pByte) {
        ERR(L"Could not allocate %u bytes", nBytes);
        return E_OUTOFMEMORY;
    }

    // two characters equals one byte
    WCHAR cFirstCharacter = L'\0';
    size_t iByte = 0;
    for (size_t iChar = 0; iChar < nLen; iChar++) {
        if (IsHex(szString[iChar])) {
            if (L'\0' == cFirstCharacter) {
                cFirstCharacter = szString[iChar];
            } else {
#pragma prefast(suppress: __WARNING_BUFFER_OVERFLOW, "iByte can't overflow here")
                pByte[iByte++] = (BYTE)16 * HexValue(cFirstCharacter) + HexValue(szString[iChar]);
                cFirstCharacter = L'\0';
            }
        }
    }

    *ppByte = pByte;
    *pnBytes = nBytes;
    return S_OK;
}

bool IsHex(WCHAR t) {
    return 
        (L'0' <= t && t <= L'9') ||
        (L'a' <= t && t <= L'f') ||
        (L'A' <= t && t <= L'F');
}

BYTE HexValue(WCHAR t) {

    if (L'0' <= t && t <= L'9') {
        return (BYTE)(t - L'0');
    }

    if (L'a' <= t && t <= L'f') {
        return (BYTE)(t - L'a' + 0xa);
    }

    if (L'A' <= t && t <= L'F') {
        return (BYTE)(t - L'A' + 0xA);
    }

    ERR(L"HexValue() called with non-hex character %c", t);
    return 0;
}
