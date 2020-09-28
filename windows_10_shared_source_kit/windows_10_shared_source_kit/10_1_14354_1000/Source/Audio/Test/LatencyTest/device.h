// device.h
//
// Copyright (C) Microsoft. All rights reserved.

#ifndef DEVICE_H
#define DEVICE_H

HSTRING GetDefaultRenderDevice(ABI::Windows::Media::Devices::AudioDeviceRole role);
HSTRING GetDefaultCaptureDevice(ABI::Windows::Media::Devices::AudioDeviceRole role);
HRESULT DeviceSupportsRaw(HSTRING device_id, bool &raw_supported);
HRESULT GetEffectsRenderDevice(HSTRING device_id,
                               std::vector<std::string> &effects_lists,
                               ABI::Windows::Media::Render::AudioRenderCategory category,
                               ABI::Windows::Media::AudioProcessing processing);
HRESULT GetEffectsCaptureDevice(HSTRING device_id,
                                std::vector<std::string> &effects_lists,
                                ABI::Windows::Media::Capture::MediaCategory category,
                                ABI::Windows::Media::AudioProcessing processing);
HRESULT GetFriendlyName(HSTRING device_id, HSTRING& name);

#endif  // DEVICE_H
