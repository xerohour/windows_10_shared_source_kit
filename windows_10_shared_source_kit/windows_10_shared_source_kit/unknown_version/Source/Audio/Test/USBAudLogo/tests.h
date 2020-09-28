// Copyright (C) Microsoft. All rights reserved.
#pragma once

// Test class needs to derive from ITest
// We're using CTestTemplate base class which
// already implements afterConstruct and simplifies logging a bit

class CUSBHidTest : public CTestTemplate
{
public:

    // 1. Hook Switch control is Usage ID 20 of the HUT Telephone Usage Page (table 16)
    // 2. SpeakerPhone control is Usage ID 2B of the HUT Telephone Usage Page (table 16)
    // 3. Microphone Mute == Phone Mute control is Usage ID 2F of the HUT Telephone Usage Page (table 16)
    // 4. Volume control is Usage ID E0 of the HUT Consumer Page Usage Page (table 17)
    // 5. The Microphone Mute indicator is ID 21 in the HUT LED Usage Page (table 13)
    // 6. The Off-Hook indicator is ID 17 in the HUT LED Usage Page (table 13)

    DWORD TC_Telephony_Hook(ISRDevice*);
    DWORD TC_Telephony_SpeakerPhone(ISRDevice*);
    DWORD TC_Telephony_PhoneMute(ISRDevice*);
    DWORD TC_Consumer_Volume(ISRDevice*);

    DWORD TestPerf(EHidControl hidCtrl, UINT cSamples);
    DWORD TC_Perf_Mute(ISRDevice*);
    DWORD TC_Perf_VolumeUp(ISRDevice*);
    DWORD TC_Perf_VolumeDown(ISRDevice*);
};
