// Copyright(C) Microsoft.All rights reserved.

// formats.cpp

#include <atlbase.h>
#include <atlstr.h>
#include <ks.h>
#include <mmreg.h>
#include <wmcodecdsp.h>
#include <ksmedia.h>
#include <mfapi.h>
#include <mmdeviceapi.h>
#include <devicetopology.h>
#include <audioclient.h>
#include <endpointvolume.h>
#include <strsafe.h>

#include <basiclog.h>
#include <BasicLogHelper.h>
#include <basicprintf.h>

#include <OffloadPath.h>
#include <WexTestClass.h>

#include "haptest.h"
#include "log.h"
#include "multiarray.h"

#define ROUNDUP_8(x) (((x) + 7) & ~7)

using namespace WEX::Logging;
using namespace WEX::TestExecution;

struct BitInfo {
    WORD nContainerBits;
    WORD nValidBits;

    BitInfo(WORD c, WORD v) : nContainerBits(c), nValidBits(v) {}
};

HRESULT GetPinDataRanges(IBasicLog* pLogger, IMMDevice* pMMDevice, bool forOffloadPin, KSMULTIPLE_ITEM** ppKsMultipleItem);
bool IsAllowedOffloadPinSubFormat(GUID guidSubTypeFormat);
bool IsAllowedHostPinSubFormat(GUID guidSubTypeFormat);

void CHapTest::SupportedFormatsPcm() {
    // don't want to see VERIFY spew
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    CComPtr<IBasicLog> pLogger;
    if (!VERIFY_SUCCEEDED(CreateWexBasicLog(&pLogger))) { return; }

    CComPtr<IMMDevice> pMMDevice;
    if (!DeviceUnderTest(pLogger, &pMMDevice)) { return; }

    CComPtr<IAudioClient2> pAudioClient2;
    if (!VERIFY_SUCCEEDED(
        pMMDevice->Activate(
            __uuidof(IAudioClient2),
            CLSCTX_ALL,
            NULL,
            reinterpret_cast<void**>(&pAudioClient2)
    ))) { return; }

    AudioClientPropertiesWin8 clientProperties = {0};
    clientProperties.bIsOffload = TRUE;
    clientProperties.eCategory = AudioCategory_Media;
    clientProperties.cbSize = sizeof(clientProperties);
#pragma prefast(suppress: __WARNING_BUFFER_OVERFLOW, "client properties is variable-sized")
    if (!VERIFY_SUCCEEDED(
        pAudioClient2->SetClientProperties(
            reinterpret_cast<AudioClientProperties *>(&clientProperties)
        )
    )) { return; }

    ChannelInfo rChannels[] = {
        CHANNELINFO(KSAUDIO_SPEAKER_MONO, L"mono"),
        CHANNELINFO(KSAUDIO_SPEAKER_STEREO, L"stereo"),
        CHANNELINFO(SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_LOW_FREQUENCY, L"2.1"),
        CHANNELINFO(SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY, L"3.1"),
        CHANNELINFO(KSAUDIO_SPEAKER_QUAD, L"4.0 quad (front left, front right, rear left, rear right)"),
        CHANNELINFO(KSAUDIO_SPEAKER_SURROUND, L"4.0 surround (front left, front right, front center, rear center)"),
        CHANNELINFO(KSAUDIO_SPEAKER_5POINT1_SURROUND & ~SPEAKER_LOW_FREQUENCY, L"5.0"),
        CHANNELINFO(KSAUDIO_SPEAKER_5POINT1_SURROUND, L"5.1"),
        CHANNELINFO(KSAUDIO_SPEAKER_7POINT1_SURROUND, L"7.1")
    };
    LOG(L"Standard channel layouts:");
    for (int i = 0; i < ARRAYSIZE(rChannels); i++) {
        LOG(L"%s", rChannels[i].szName);
    }

    BitInfo rBits[] = {
        BitInfo(8, 8), // uint8
        BitInfo(16, 16), // int16
        BitInfo(24, 20), // 20-bit data in 24-bit containers
        BitInfo(24, 24), // 24-bit data in 24-bit containers
        BitInfo(32, 24), // 24-bit data in 32-bit containers
        BitInfo(32, 32), // 32-bit data in 32-bit containers
    };
    LOG(L"Standard bit depths:");
    for (int i = 0; i < ARRAYSIZE(rBits); i++) {
        LOG(L"%u valid bits in %u-bit containers", rBits[i].nValidBits, rBits[i].nContainerBits);
    }

    UINT32 rSampleRates[] = {
        8000,
        11025,
        16000,
        22050,
        32000,
        44100,
        48000,
        88200,
        96000,
        176400,
        192000,
    };
    LOG(L"Standard sample rates:");
    for (int i = 0; i < ARRAYSIZE(rSampleRates); i++) {
        LOG(L"%s", static_cast<LPCWSTR>(StringSampleRate(rSampleRates[i])));
    }
    
    HRESULT hr = S_OK;
    // this keeps track of whether the format is supported
    int maxes[] = { ARRAYSIZE(rChannels), ARRAYSIZE(rBits), ARRAYSIZE(rSampleRates) };
    CHeapBoolMultiArray<ARRAYSIZE(maxes)> rSupported(
        maxes,
        hr
    );
    if (FAILED(hr)) {
        ERR(L"CHeapBoolMultiArray returned 0x%08x", hr);
        return;
    }
        
    const int total = ARRAYSIZE(rChannels) * ARRAYSIZE(rBits) * ARRAYSIZE(rSampleRates);
    int supported = 0;

    LOG(L"Querying %d PCM formats...", total);
    int coordinates[ARRAYSIZE(maxes)] = {0};
    for (int c = 0; c < ARRAYSIZE(rChannels); c++) {
        coordinates[0] = c;
        
        for (int b = 0; b < ARRAYSIZE(rBits); b++) {
            coordinates[1] = b;
            
            for (int s = 0; s < ARRAYSIZE(rSampleRates); s++) {
                coordinates[2] = s;
                
                WAVEFORMATEXTENSIBLE wfx = { 0 };
                wfx.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
                wfx.Format.nChannels = rChannels[c].nChannels;
                wfx.Format.nSamplesPerSec = rSampleRates[s];
                wfx.Format.wBitsPerSample = rBits[b].nContainerBits;
                wfx.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
                wfx.Format.nBlockAlign = wfx.Format.nChannels * wfx.Format.wBitsPerSample / 8;
                wfx.Format.nAvgBytesPerSec = wfx.Format.nBlockAlign * wfx.Format.nSamplesPerSec;
                wfx.Samples.wValidBitsPerSample = rBits[b].nValidBits;
                wfx.dwChannelMask = rChannels[c].dwChannelMask;
                wfx.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;

                CComHeapPtr<WAVEFORMATEX> pClosest;

                HRESULT hr = pAudioClient2->IsFormatSupported(
                    AUDCLNT_SHAREMODE_SHARED,
                    &wfx.Format,
                    &pClosest // necessary or IsFormatSupported returns E_POINTER
                );

                switch (hr) {
                    case S_OK:
                        rSupported(coordinates) = true;
                        supported++;
                        break;
                    case AUDCLNT_E_UNSUPPORTED_FORMAT:
                        break;
                    default: {
                        CString format;
                        format.Format(
                            L"%s %u-in-%u-bit %s PCM",
                            rChannels[c].szName,
                            rBits[b].nValidBits, rBits[b].nContainerBits,
                            static_cast<LPCWSTR>(StringSampleRate(rSampleRates[s]))
                        );
                        
                        ERR(
                            L"Unexpected HRESULT 0x%08x returned from IAudioClient2::IsFormatSupported with format %s",
                            hr,
                            static_cast<LPCWSTR>(format)
                        );
                        break;
                    }
                }
            } // for each sample rate
        } // for each bit depth
    } // for each channel mask

    LOG(L"%d of %d required PCM formats supported", supported, total);

    // if they're all supported, great
    if (!rSupported.Any(false)) {
        LOG(L"All standard PCM formats supported.");
        return;
    }
    
    // if none of them are supported, that's very bad.
    if (!rSupported.Any(true)) {
        ERR(L"No standard PCM formats are supported!");
        return;
    }
}

void CHapTest::ForbiddenFormats() {
    // don't want to see VERIFY spew
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    CComPtr<IBasicLog> pLogger;
    if (!VERIFY_SUCCEEDED(CreateWexBasicLog(&pLogger))) { return; }

    CComPtr<IMMDevice> pMMDevice;
    if (!DeviceUnderTest(pLogger, &pMMDevice)) { return; }

    // validate host pin formats
    {
        CComHeapPtr<KSMULTIPLE_ITEM> spKsMultipleItem;
        if (!VERIFY_SUCCEEDED(GetPinDataRanges(pLogger, pMMDevice, false, &spKsMultipleItem))) { return; }

        PKSDATARANGE pKsDataRange = (PKSDATARANGE)((PKSMULTIPLE_ITEM)spKsMultipleItem + 1);
        bool nextIsAttributeList = false;
        bool thisIsAttributeList = false;
        
        for (
            UINT nRange = 0;
            nRange < spKsMultipleItem->Count;
            nRange++, pKsDataRange = (
                thisIsAttributeList ?
                    (PKSDATARANGE)((PBYTE)pKsDataRange + ROUNDUP_8(((PKSMULTIPLE_ITEM)pKsDataRange)->Size)) :
                    (PKSDATARANGE)((PBYTE)pKsDataRange + ROUNDUP_8(pKsDataRange->FormatSize))
            )
        ) {
            thisIsAttributeList = nextIsAttributeList;
            nextIsAttributeList = false;

            // ignore attribute lists
            if (thisIsAttributeList) { continue; }
            
            nextIsAttributeList = ((pKsDataRange->Flags & KSDATARANGE_ATTRIBUTES) == KSDATARANGE_ATTRIBUTES);
            
            if (!VERIFY_ARE_EQUAL(TRUE, IsEqualGUID(KSDATAFORMAT_TYPE_AUDIO, pKsDataRange->MajorFormat))) {
                CComHeapPtr<WCHAR> spMajorFormatGuid;
                StringFromCLSID(pKsDataRange->MajorFormat, &spMajorFormatGuid);

                ERR(L"Major format is NOT KSDATAFORMAT_TYPE_AUDIO.  Actual %s", spMajorFormatGuid.m_pData);
                continue;
            }

            if (!IsAllowedHostPinSubFormat(pKsDataRange->SubFormat)) {
                CComHeapPtr<WCHAR> spSubFormatGuid;
                StringFromCLSID(pKsDataRange->SubFormat, &spSubFormatGuid);

                ERR(L"Host pin should NOT support this format: %s", spSubFormatGuid.m_pData);
            }
        } // for each data range
    } // validate host pin formats

    // validate offload pin formats
    {
        CComHeapPtr<KSMULTIPLE_ITEM> spKsMultipleItem;
        if (!VERIFY_SUCCEEDED(GetPinDataRanges(pLogger, pMMDevice, true, &spKsMultipleItem))) { return; }

        PKSDATARANGE pKsDataRange = (PKSDATARANGE)((PKSMULTIPLE_ITEM)spKsMultipleItem + 1);
        bool nextIsAttributeList = false;
        bool thisIsAttributeList = false;
        
        for (
            UINT nRange = 0;
            nRange < spKsMultipleItem->Count;
            nRange++, pKsDataRange = (
                thisIsAttributeList ?
                    (PKSDATARANGE)((PBYTE)pKsDataRange + ROUNDUP_8(((PKSMULTIPLE_ITEM)pKsDataRange)->Size)) :
                    (PKSDATARANGE)((PBYTE)pKsDataRange + ROUNDUP_8(pKsDataRange->FormatSize))
            )
        ) {
            thisIsAttributeList = nextIsAttributeList;
            nextIsAttributeList = false;

            // ignore attribute lists
            if (thisIsAttributeList) { continue; }
            
            nextIsAttributeList = ((pKsDataRange->Flags & KSDATARANGE_ATTRIBUTES) == KSDATARANGE_ATTRIBUTES);
            
            if (!VERIFY_ARE_EQUAL(TRUE, IsEqualGUID(KSDATAFORMAT_TYPE_AUDIO, pKsDataRange->MajorFormat))) {
                CComHeapPtr<WCHAR> spMajorFormatGuid;
                StringFromCLSID(pKsDataRange->MajorFormat, &spMajorFormatGuid);

                ERR(L"Major format is NOT KSDATAFORMAT_TYPE_AUDIO.  Actual %s", spMajorFormatGuid.m_pData);
                continue;
            }

            if (!IsAllowedOffloadPinSubFormat(pKsDataRange->SubFormat)) {
                CComHeapPtr<WCHAR> spSubFormatGuid;
                StringFromCLSID(pKsDataRange->SubFormat, &spSubFormatGuid);

                ERR(L"Offload pin should NOT support this format: %s", spSubFormatGuid.m_pData);
            }
        } // for each data range
    } // validate offload pin formats
}

HRESULT GetPinDataRanges(IBasicLog* pLogger, IMMDevice* pMMDevice, bool forOffloadPin, KSMULTIPLE_ITEM** ppKsMultipleItem) {
    HRESULT hr = S_OK;

    *ppKsMultipleItem = nullptr;

    CComPtr<IOffloadPath> pOffloadPath;
    if (!VERIFY_SUCCEEDED(hr = GetOffloadPath(pLogger, pMMDevice, &pOffloadPath))) { return hr; } // GetOffloadPath does its own VERIFY

    UINT pinId = (UINT)-1;
    if (forOffloadPin) {
        if (!VERIFY_SUCCEEDED(hr = pOffloadPath->GetOffloadPinFactoryId(&pinId))) { return hr; }
    } else {
        if (!VERIFY_SUCCEEDED(hr = pOffloadPath->GetHostPinFactoryId(&pinId))) { return hr; }
    }

    CComPtr<IKsControl> pKsControl;
    if (!VERIFY_SUCCEEDED(hr = GetAdapterKsControl(pLogger, pOffloadPath, &pKsControl))) { return hr; }

    KSP_PIN ksPin;
    ksPin.PinId = pinId;
    ksPin.Property.Set = KSPROPSETID_Pin;
    ksPin.Property.Id = KSPROPERTY_PIN_DATARANGES;
    ksPin.Property.Flags = KSPROPERTY_TYPE_GET;
    ksPin.Reserved = 0;

    // see how much memory we need
    ULONG cbBufferSize = 0;
    if (!VERIFY_SUCCEEDED(hr = pKsControl->KsProperty(&(ksPin.Property), sizeof(KSP_PIN), NULL, 0, &cbBufferSize))) { return hr; }

    // it must be at least a KSMULTIPLE_ITEM
    if (!VERIFY_IS_GREATER_THAN_OR_EQUAL(cbBufferSize, sizeof(KSMULTIPLE_ITEM))) { return E_UNEXPECTED; }

    // allocate that much memory
    CComHeapPtr<KSMULTIPLE_ITEM> spKsMultipleItem;
    if (!VERIFY_IS_TRUE(spKsMultipleItem.AllocateBytes(cbBufferSize))) { return E_OUTOFMEMORY; }

    // get the property
    ULONG cbWritten;
    if (!VERIFY_SUCCEEDED(hr = pKsControl->KsProperty(&(ksPin.Property), sizeof(KSP_PIN), spKsMultipleItem.m_pData, cbBufferSize,
        &cbWritten))) { return hr; }
        
    // validate that the size didn't change
    if (!VERIFY_ARE_EQUAL(cbBufferSize, cbWritten)) { return E_UNEXPECTED; }
    
    // validate that the buffer size matches the KSMULTIPLE_ITEM.Size
    if (!VERIFY_ARE_EQUAL(cbBufferSize, spKsMultipleItem->Size)) { return E_UNEXPECTED; }

    // iterate over the data formats and verify that the sizes add up and the count is accurate
    PKSMULTIPLE_ITEM pRanges = (PKSMULTIPLE_ITEM)spKsMultipleItem;
    PBYTE pEnd = (PBYTE)pRanges + pRanges->Size;
    UINT nRange = 0;
    bool nextIsAttributeList = false;
    bool thisIsAttributeList = false;
    
    for (
        PKSDATARANGE pKsDataRange = (PKSDATARANGE)(pRanges + 1);
        (PBYTE)pKsDataRange < pEnd;
        pKsDataRange = (
            thisIsAttributeList ?
                (PKSDATARANGE)((PBYTE)pKsDataRange + ROUNDUP_8(((PKSMULTIPLE_ITEM)pKsDataRange)->Size)) :
                (PKSDATARANGE)((PBYTE)pKsDataRange + ROUNDUP_8(pKsDataRange->FormatSize))
        )
    ) {
        nRange++;
        if (!VERIFY_IS_LESS_THAN_OR_EQUAL(nRange, pRanges->Count)) { return E_UNEXPECTED; }

        thisIsAttributeList = nextIsAttributeList;
        nextIsAttributeList = false;
        
        ULONG bytesLeft = (ULONG)(pEnd - (PBYTE)pKsDataRange);
        
        if (thisIsAttributeList) {
            // verify there's enough space to hold the KSMULTIPLE_ITEM header
            if (!VERIFY_IS_GREATER_THAN_OR_EQUAL(bytesLeft, sizeof(KSMULTIPLE_ITEM))) { return E_UNEXPECTED; }

            PKSMULTIPLE_ITEM pAttributeIds = (PKSMULTIPLE_ITEM)pKsDataRange;
                        
            // verify there's enough space to hold the data
            if (!VERIFY_IS_GREATER_THAN_OR_EQUAL(bytesLeft, pAttributeIds->Size)) { return E_UNEXPECTED; }
            
            // if this is the last range, verify we take up the rest of the buffer
            if (nRange == pRanges->Count) {
                if (!VERIFY_IS_TRUE(
                    pAttributeIds->Size == bytesLeft ||
                    ROUNDUP_8(pAttributeIds->Size) == bytesLeft
                )) { return E_UNEXPECTED; }
            }
        } else {
            // verify there's enough space to hold the KSDATARANGE header
            if (!VERIFY_IS_GREATER_THAN_OR_EQUAL(bytesLeft, sizeof(KSDATARANGE))) { return E_UNEXPECTED; }
                        
            // verify there's enough space to hold the data
            if (!VERIFY_IS_GREATER_THAN_OR_EQUAL(bytesLeft, pKsDataRange->FormatSize)) { return E_UNEXPECTED; }
            
            if (pKsDataRange->Flags & KSDATARANGE_ATTRIBUTES) {
                nextIsAttributeList = true;
                
                // there must be an attribute list pseudo-datarange following
                if (!VERIFY_IS_LESS_THAN(nRange, pRanges->Count)) { return E_UNEXPECTED; }
            }

            // if this is the last range, verify we take up the rest of the buffer
            if (nRange == pRanges->Count) {
                if (!VERIFY_IS_TRUE(
                    pKsDataRange->FormatSize == bytesLeft ||
                    ROUNDUP_8(pKsDataRange->FormatSize) == bytesLeft
                )) { return E_UNEXPECTED; }
            }
        }
    }
    
    if (!VERIFY_ARE_EQUAL(nRange, pRanges->Count)) { return E_UNEXPECTED; }
    
    *ppKsMultipleItem = spKsMultipleItem.Detach();

    return S_OK;
}

bool IsAllowedOffloadPinSubFormat(GUID guidSubTypeFormat) {
    GUID guidAllowedOffloadSubTypeArray[] =
    {   /* Uncompressed */
        KSDATAFORMAT_SUBTYPE_PCM,
        KSDATAFORMAT_SUBTYPE_IEEE_FLOAT,

        /* AAC (MEDIASUBTYPE_RAW_AAC1 not allowed, MEDIASUBTYPE_MPEG_ADTS_AAC subtype not used) */
        KSDATAFORMAT_SUBTYPE_MPEG_HEAAC,

        /* Dolby */
        MFAudioFormat_Dolby_AC3,
        MEDIASUBTYPE_DVM,
        MEDIASUBTYPE_DOLBY_DDPLUS,

        /* MPEG Audio */
        KSDATAFORMAT_SUBTYPE_MPEG,

        /* MP3 */
        KSDATAFORMAT_SUBTYPE_MPEGLAYER3,

        /* WMA (not including WMA Voice 9 or 10) */
        KSDATAFORMAT_SUBTYPE_WMAUDIO2,
        KSDATAFORMAT_SUBTYPE_WMAUDIO3,
        KSDATAFORMAT_SUBTYPE_WMAUDIO_LOSSLESS,
    };

    for (UINT i = 0; i < ARRAYSIZE(guidAllowedOffloadSubTypeArray); i++) {
        if (IsEqualGUID(guidAllowedOffloadSubTypeArray[i], guidSubTypeFormat)) {
                return true;
        }
    }

    return false;
}

bool IsAllowedHostPinSubFormat(GUID guidSubTypeFormat) {
    GUID guidAllowedHostSubTypeArray[] =
    {   /* Uncompressed */
        KSDATAFORMAT_SUBTYPE_PCM,
        KSDATAFORMAT_SUBTYPE_IEEE_FLOAT,

        /* SPDIF */
        KSDATAFORMAT_SUBTYPE_IEC61937_DOLBY_DIGITAL,
        KSDATAFORMAT_SUBTYPE_IEC61937_WMA_PRO,
        KSDATAFORMAT_SUBTYPE_IEC61937_MPEG1,
    };

    // look through known subtypes
    for (UINT i = 0; i < ARRAYSIZE(guidAllowedHostSubTypeArray); i++) {
        if (IsEqualGUID(guidAllowedHostSubTypeArray[i], guidSubTypeFormat)) {
                return true;
        }
    }

    // SPDIF-ized formats that match the CEA 861 specifciation will have
    // a subtype of {XXXXXXXX-0CEA-0010-8000-00AA00389B71}
    GUID guidCEASubType = { 0x00000000, 0x0CEA, 0x00100, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 };
    guidSubTypeFormat.Data1 = 0x00000000;
    if (IsEqualGUID(guidCEASubType, guidSubTypeFormat)) {
        return true;
    }

    return false;
}

WORD OnBits(UINT32 mask) {
    WORD on = 0;
    for (UINT i = 0; i < sizeof(mask) * 8; i++) {
        if (mask & ((UINT32)1 << i)) {
            on++;
        }
    }
    
    return on;
}
