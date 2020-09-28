// latencytest.h
//
// Copyright (C) Microsoft. All rights reserved.

#ifndef LATENCYTEST_H
#define LATENCYTEST_H

// taef
using namespace WEX::Logging;
using namespace WEX::TestExecution;
using namespace WEX::Common;

class LatencyTest : public WEX::TestClass<LatencyTest> {
   public:
    BEGIN_TEST_CLASS(LatencyTest)
        TEST_CLASS_PROPERTY(L"ThreadingModel",
                            L"MTA")  // TAEF will call CoInitialize MTA
        TEST_CLASS_PROPERTY(L"Options: ", L"/'p:render_pcm' for initialize with fixed point on render stream")
        TEST_CLASS_PROPERTY(L"Options: ", L"/'p:capture_pcm' for initialize with fixed point on render stream")
    END_TEST_CLASS()

    TEST_CLASS_SETUP(Setup);
    TEST_CLASS_CLEANUP(Cleanup);

    // looks for further options on the command line
    TEST_METHOD(Custom);

    // "low period" is new for Threshold
    TEST_METHOD(Vanilla);
#if NTDDI_VERSION >= NTDDI_WINTHRESHOLD
    TEST_METHOD(LowPeriod);
#endif

    TEST_METHOD(Raw);
#if NTDDI_VERSION >= NTDDI_WINTHRESHOLD
    TEST_METHOD(RawLowPeriod);
#endif

    TEST_METHOD(Communications);
#if NTDDI_VERSION >= NTDDI_WINTHRESHOLD
    TEST_METHOD(CommunicationsLowPeriod);
#endif

    TEST_METHOD(Media);
#if NTDDI_VERSION >= NTDDI_WINTHRESHOLD
    TEST_METHOD(MediaLowPeriod);
#endif

    // "game chat" is new for threshold
#if NTDDI_VERSION >= NTDDI_WINTHRESHOLD
    TEST_METHOD(GameChat);
    TEST_METHOD(GameChatLowPeriod);
#endif

    // "speech" is new for threshold
#if NTDDI_VERSION >= NTDDI_WINTHRESHOLD
    TEST_METHOD(Speech);
    TEST_METHOD(SpeechLowPeriod);
#endif

   private:
    void ListEffects(
        RoundtripOptions options,
        HSTRING device_render_id,
        HSTRING device_capture_id,
        bool *pAEC
    );
    void Roundtrip(RoundtripOptions options);
       
    bool just_played_;
};
#endif  // LATENCYTEST_H
