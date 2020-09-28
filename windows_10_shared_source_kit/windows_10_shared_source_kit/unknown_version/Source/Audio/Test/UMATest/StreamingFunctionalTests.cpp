// Copyright(C) Microsoft. All rights reserved.

//
// StreamingFunctionalTests.cpp
//  
//
// Description:
//
//  WASAPI Streaming Functional Tests

#include <stdafx.h>

#include <UMATestStreaming.h>


#include "StreamingHelper.h"

// Test tone properties
const FLOAT TEST_TONE_FREQUENCY = 750.0;   // Render test tones at 750Hz
const FLOAT TEST_TONE_AMPLITUDE = 0.01f;    // Render test tones nice and quiet-like
const UINT32 TEST_TONE_DURATION = 500;    // Render tones for half a second
const UINT32 BITS_PER_BYTE = 8;

HRESULT RenderPCMWaveFormatExTone(DWORD nSamplesPerSec,
                                  WORD wBitsPerSample,
                                  WORD nChannels,
                                  AUDCLNT_SHAREMODE nShareMode,
                                  DWORD dwStreamFlags,
                                  TEST_STREAMING_ALGORITHM eAlgorithm);

HRESULT CapturePCMWaveFormatEx(DWORD nSamplesPerSec,
                               WORD wBitsPerSample,
                               WORD nChannels,
                               AUDCLNT_SHAREMODE nShareMode,
                               DWORD dwStreamFlags,
                               TEST_STREAMING_ALGORITHM eAlgorithm);

HRESULT RenderPCMWaveFormatExtensibleTone(DWORD nSamplesPerSec,
                                          WORD wValidBitsPerSample,
                                          WORD wBitsPerSample,
                                          WORD nChannels,
                                          DWORD dwChannelMask,
                                          AUDCLNT_SHAREMODE nShareMode,
                                          DWORD dwStreamFlags,
                                          TEST_STREAMING_ALGORITHM eAlgorithm);

HRESULT CapturePCMWaveFormatExtensible(DWORD nSamplesPerSec,
                                       WORD wValidBitsPerSample,
                                       WORD wBitsPerSample,
                                       WORD nChannels,
                                       DWORD dwChannelMask,
                                       AUDCLNT_SHAREMODE nShareMode,
                                       DWORD dwStreamFlags,
                                       TEST_STREAMING_ALGORITHM eAlgorithm);
                                  

////
// Description: Test Shared Push-Mode Render Functionality
//
// Params:
//  N/A
//
// Returns:
//  FNS_PASS (faliure is signalled via XLOG
////
DWORD TC_Render_Shared_Push()
{
    HRESULT hr = S_OK;

    // put up a message for the user
    SLOG(eError, "Rendering test tone...");

    METHODCHECKRESULT("TestRenderTone",
                      hr,
                      TestRenderTone(AUDCLNT_SHAREMODE_SHARED, 
                                     0, 
                                     NULL, 
                                     TIMER_PUSH_RENDER, 
                                     TEST_TONE_AMPLITUDE, 
                                     TEST_TONE_FREQUENCY, 
                                     TEST_TONE_DURATION));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error rendering test tone");
    }

    return FNS_PASS;
}

////
// Description: Test Shared Push-Mode Render Functionality
//              Verify that a tone of the expected frequency was rendered using software loopback
//
// Params:
//  N/A
//
// Returns:
//  FNS_PASS (faliure is signalled via XLOG
////
DWORD TC_Render_Shared_Push_SWLoopback()
{
    HRESULT hr = S_OK;

    if (StrStrI(GetModule()->GetDevice()->m_szRep, _T("Virtual Audio Device (WDM)")) != NULL)
    {
        XLOG(XSKIP, 1, "SKIP: Virtual audio device does not implement accurate loopback. Hence skipping this test.");
        return FNS_SKIPPED;
    }

    // put up a message for the user
    SLOG(eError, "Rendering test tone...");

    METHODCHECKRESULT("TestRenderTone",
                      hr,
                      TestRenderTone(AUDCLNT_SHAREMODE_SHARED, 
                                     0, 
                                     NULL, 
                                     TIMER_PUSH_RENDER, 
                                     TEST_TONE_AMPLITUDE, 
                                     TEST_TONE_FREQUENCY, 
                                     0,
                                     true));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error rendering and verifying test tone!");
    }

    return FNS_PASS;
}


////
// Description: Test Shared Pull-Mode Render Functionality
//
// Params:
//  N/A
//
// Returns:
//  FNS_PASS (faliure is signalled via XLOG
////
DWORD TC_Render_Shared_Pull()
{
    HRESULT hr = S_OK;

    // put up a message for the user
    SLOG(eError, "Rendering test tone...");

    METHODCHECKRESULT("TestRenderTone",
                      hr,
                      TestRenderTone(AUDCLNT_SHAREMODE_SHARED, 
                                     0, 
                                     NULL, 
                                     EVENT_PULL_RENDER, 
                                     TEST_TONE_AMPLITUDE, 
                                     TEST_TONE_FREQUENCY, 
                                     TEST_TONE_DURATION));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error rendering and verifying test tone");
    }

    return FNS_PASS;
}

////
// Description: Test Shared Pull-Mode Render Functionality
//              Verify that a tone of the expected frequency was rendered using software loopback
//
// Params:
//  N/A
//
// Returns:
//  FNS_PASS (faliure is signalled via XLOG
////
DWORD TC_Render_Shared_Pull_SWLoopback()
{
    HRESULT hr = S_OK;

    if (StrStrI(GetModule()->GetDevice()->m_szRep, _T("Virtual Audio Device (WDM)")) != NULL)
    {
        XLOG(XSKIP, 1, "SKIP: Virtual audio device does not implement accurate loopback. Hence skipping this test.");
        return FNS_SKIPPED;
    }

    // put up a message for the user
    SLOG(eError, "Rendering test tone...");

    METHODCHECKRESULT("TestRenderTone",
                      hr,
                      TestRenderTone(AUDCLNT_SHAREMODE_SHARED, 
                                     0, 
                                     NULL, 
                                     EVENT_PULL_RENDER, 
                                     TEST_TONE_AMPLITUDE, 
                                     TEST_TONE_FREQUENCY, 
                                     0,
                                     true));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error rendering and verifying test tone");
    }

    return FNS_PASS;
}
    
////
// Description: Test Shared Push-Mode Capture Functionality
//
// Params:
//  N/A
//
// Returns:
//  FNS_PASS (faliure is signalled via XLOG
////
DWORD TC_Capture_Shared_Push()
{
    HRESULT hr = S_OK;

    // put up a message for the user
    SLOG(eError, "Capturing audio...");

    METHODCHECKRESULT("TestCapture",
                      hr,
                      TestCapture(AUDCLNT_SHAREMODE_SHARED, 
                                  0, 
                                  NULL, 
                                  TIMER_PUSH_CAPTURE, 
                                  TEST_TONE_DURATION));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error detected during capture");
    }

    return FNS_PASS;
}

////
// Description: Test Shared Event-Pull Capture Functionality
//
// Params:
//  N/A
//
// Returns:
//  FNS_PASS (faliure is signalled via XLOG
////
DWORD TC_Capture_Shared_Pull()
{
    HRESULT hr = S_OK;

    // put up a message for the user
    SLOG(eError, "Capturing audio...");

    METHODCHECKRESULT("TestCapture",
                      hr,
                      TestCapture(AUDCLNT_SHAREMODE_SHARED, 
                                  0, 
                                  NULL, 
                                  EVENT_PULL_CAPTURE, 
                                  TEST_TONE_DURATION));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error detected during capture");
    }

    return FNS_PASS;
}

////
// Description: Test Timer-Push Loopback Functionality
//
// Params:
//  N/A
//
// Returns:
//  FNS_PASS (faliure is signalled via XLOG
////
DWORD TC_Loopback_Push_Shared()
{
    HRESULT hr = S_OK;

    // put up a message for the user
    SLOG(eError, "Capturing loopback...");

    METHODCHECKRESULT("TestCapture",
                      hr,
                      TestCapture(AUDCLNT_SHAREMODE_SHARED, 
                                  AUDCLNT_STREAMFLAGS_LOOPBACK, 
                                  NULL, 
                                  TIMER_PUSH_CAPTURE, 
                                  TEST_TONE_DURATION));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error detected during loopback capture");
    }

    return FNS_PASS;
}

DWORD TC_Loopback_Push_Compat()
{
    HRESULT hr = S_OK;

    // put up a message for the user
    SLOG(eError, "Capturing loopback...");

    METHODCHECKRESULT("TestCapture",
                      hr,
                      TestCapture(AUDCLNT_SHAREMODE_SHARED, 
                                  AUDCLNT_STREAMFLAGS_LOOPBACK |
                                  AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM, 
                                  NULL, 
                                  TIMER_PUSH_CAPTURE, 
                                  TEST_TONE_DURATION));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error detected during loopback capture");
    }

    return FNS_PASS;
}

DWORD TC_Loopback_Push_Compat_Enhanced()
{
    HRESULT hr = S_OK;

    // put up a message for the user
    SLOG(eError, "Capturing loopback...");

    METHODCHECKRESULT("TestCapture",
                      hr,
                      TestCapture(AUDCLNT_SHAREMODE_SHARED, 
                                  AUDCLNT_STREAMFLAGS_LOOPBACK |
                                  AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                  AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY, 
                                  NULL, 
                                  TIMER_PUSH_CAPTURE, 
                                  TEST_TONE_DURATION));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error detected during loopback capture");
    }

    return FNS_PASS;
}

////
// Description: Test Shared Event-Pull Loopback Functionality
//
// Params:
//  N/A
//
// Returns:
//  FNS_PASS (faliure is signalled via XLOG
////
DWORD TC_Loopback_Pull_Shared()
{
    HRESULT hr = S_OK;

    // put up a message for the user
    SLOG(eError, "Capturing loopback...");

    METHODCHECKRESULT("TestCapture",
                      hr,
                      TestCapture(AUDCLNT_SHAREMODE_SHARED, 
                                  AUDCLNT_STREAMFLAGS_LOOPBACK, 
                                  NULL, 
                                  EVENT_PULL_CAPTURE, 
                                  TEST_TONE_DURATION));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error detected during loopback capture");
    }

    return FNS_PASS;
}

DWORD TC_Loopback_Pull_Compat()
{
    HRESULT hr = S_OK;

    // put up a message for the user
    SLOG(eError, "Capturing loopback...");

    METHODCHECKRESULT("TestCapture",
                      hr,
                      TestCapture(AUDCLNT_SHAREMODE_SHARED, 
                                  AUDCLNT_STREAMFLAGS_LOOPBACK |
                                  AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM, 
                                  NULL, 
                                  EVENT_PULL_CAPTURE, 
                                  TEST_TONE_DURATION));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error detected during loopback capture");
    }

    return FNS_PASS;
}

DWORD TC_Loopback_Pull_Compat_Enhanced()
{
    HRESULT hr = S_OK;

    // put up a message for the user
    SLOG(eError, "Capturing loopback...");

    METHODCHECKRESULT("TestCapture",
                      hr,
                      TestCapture(AUDCLNT_SHAREMODE_SHARED, 
                                  AUDCLNT_STREAMFLAGS_LOOPBACK |
                                  AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                  AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY, 
                                  NULL, 
                                  EVENT_PULL_CAPTURE, 
                                  TEST_TONE_DURATION));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error detected during loopback capture");
    }

    return FNS_PASS;
}


///
// Compat Mode Testing
///
DWORD TC_Render_Compat_Pull_0()
{
    // 192k / 32-bit / 1ch
    RenderPCMWaveFormatExTone(192000,
                              32,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              EVENT_PULL_RENDER);
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_1()
{
    // 11kHz / 8-bit / 1ch
    RenderPCMWaveFormatExTone(11025,
                              8,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_2()
{
    // 22kHz / 32-bit / 2ch
    RenderPCMWaveFormatExTone(22050,
                              32,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_3()
{
    // 3kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(3000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_4()
{
    // 88.2kHz / 16-bit / 1ch
    RenderPCMWaveFormatExTone(88200,
                              16,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_5()
{
    // 96kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(96000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_6()
{
    // 64kHz / 8-bit / 1ch
    RenderPCMWaveFormatExTone(64000,
                              8,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_7()
{
    // 96kHz / 8-bit / 2ch
    RenderPCMWaveFormatExTone(96000,
                              8,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_8()
{
    // 48kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(48000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_9()
{
    // 48kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(48000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_10()
{
    // 64kHz / 32-bit / 2ch
    RenderPCMWaveFormatExTone(64000,
                              32,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_11()
{
    // 11kHz / 24-bit / 2ch
    RenderPCMWaveFormatExTone(11025,
                              24,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_12()
{
    // 192kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(192000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_13()
{
    // 8kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(8000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_14()
{
    // 44.1kHz / 32-bit / 2ch
    RenderPCMWaveFormatExTone(44100,
                              32,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_15()
{
    // 32kHz / 8-bit / 2ch
    RenderPCMWaveFormatExTone(32000,
                              8,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_16()
{
    // 32kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(32000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_17()
{
    // 44.1kHz / 8-bit / 1ch
    RenderPCMWaveFormatExTone(44100,
                              8,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_18()
{
    // 3kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(3000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_19()
{
    // 22.05kHz / 16-bit / 1ch
    RenderPCMWaveFormatExTone(22050,
                              16,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_20()
{
    // 48kHz / 24-bit / 2ch
    RenderPCMWaveFormatExTone(48000,
                              24,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_21()
{
    // 48kHz / 32-bit / 2ch
    RenderPCMWaveFormatExTone(48000,
                              32,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_22()
{
    // 44.1kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(44100,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_23()
{
    // 96kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(96000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_24()
{
    // 16kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(16000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_25()
{
    // 11.025kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(11025,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_26()
{
    // 22.05kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(22050,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_27()
{
    // 64kHz / 16-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(64000,
                                      16,       // container
                                      16,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_28()
{
    // 16kHz / 24-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(16000,
                                      24,       // container
                                      24,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_29()
{
    // 64kHz / 24-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(64000,
                                      24,       // container
                                      24,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_30()
{
    // 11.025kHz / 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(11025,
                                      32,       // container
                                      32,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_31()
{
    // 32kHz / 16-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(32000,
                                      16,       // container
                                      16,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_32()
{
    // 3kHz / 8-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(3000,
                                      8,        // container
                                      8,        // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_33()
{
    // 88.2kHz/ 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(88200,
                                      32,       // container
                                      32,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_34()
{
    // 96kHz/ 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(96000,
                                      32,       // container
                                      32,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_35()
{
    // 16kHz/ 8-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(16000,
                                      8,        // container
                                      8,        // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_36()
{
    // 3kHz/ 32-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(3000,
                                      32,       // container
                                      32,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_37()
{
    // 8kHz/ 8-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(8000,
                                      8,        // container
                                      8,        // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_38()
{
    // 48kHz/ 8-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      8,        // container
                                      8,        // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_39()
{
    // 96kHz/ 16-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(96000,
                                      16,       // container
                                      16,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_40()
{
    // 11.025kHz/ 16-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(11025,
                                      16,       // container
                                      16,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_41()
{
    // 192kHz/ 24-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(192000,
                                      24,       // container
                                      24,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_42()
{
    // 8kHz/ 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(8000,
                                      32,       // container
                                      32,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_43()
{
    // 88.2kHz/ 24-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(88200,
                                      24,       // container
                                      24,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_44()
{
    // 22.050kHz/ 24-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(22050,
                                      24,       // container
                                      24,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_45()
{
    // 48kHz/ 32-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      32,       // container
                                      32,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_46()
{
    // 192kHz/ 8-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(192000,
                                      8,        // container
                                      8,        // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_47()
{
    // 32kHz/ 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(32000,
                                      32,       // container
                                      32,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_48()
{
    // 44.1kHz/ 24-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(44100,
                                      24,       // container
                                      24,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_49()
{
    // 22.05kHz/ 8-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(22050,
                                      8,        // container
                                      8,        // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_50()
{
    // 44.1kHz/ 16-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(44100,
                                      16,       // container
                                      16,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_51()
{
    // 16kHz/ 32-bit / 32ch
    RenderPCMWaveFormatExtensibleTone(16000,
                                      32,       // container
                                      32,       // valid bits 
                                      32,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_52()
{
    // 88.2kHz/ 8-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(88200,
                                      8,        // container
                                      8,        // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_53()
{
    // 16kHz/ 16-bit / 16ch
    RenderPCMWaveFormatExtensibleTone(16000,
                                      16,       // container
                                      16,       // valid bits 
                                      16,       // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_54()
{
    // 48kHz/ 32-bit / 2ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      20,       // container
                                      32,       // valid bits 
                                      2,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_55()
{
    // 48kHz/ 32-bit / 2ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      24,       // container
                                      32,       // valid bits 
                                      2,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_56()
{
    // 48kHz/ 32-bit / 2ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      32,       // container
                                      32,       // valid bits 
                                      2,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_57()
{
    // 48kHz/ 16-bit / 4ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      4,       // channels 
                                      KSAUDIO_SPEAKER_QUAD, // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_58()
{
    // 48kHz/ 16-bit / 4ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      4,       // channels 
                                      KSAUDIO_SPEAKER_SURROUND, // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_59()
{
    // 48kHz/ 16-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      6,       // channels 
                                      KSAUDIO_SPEAKER_5POINT1, // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_60()
{
    // 48kHz/ 16-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      8,       // channels 
                                      KSAUDIO_SPEAKER_7POINT1, // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_61()
{
    // 48kHz/ 16-bit / 1ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      1,       // channels 
                                      KSAUDIO_SPEAKER_MONO,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Pull_62()
{
    // 48kHz/ 16-bit / 2ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      2,       // channels 
                                      KSAUDIO_SPEAKER_STEREO, // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_0()
{
    // 192k / 32-bit / 1ch
    RenderPCMWaveFormatExTone(192000,
                              32,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              TIMER_PUSH_RENDER);

    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_1()
{
    // 11kHz / 8-bit / 1ch
    RenderPCMWaveFormatExTone(11025,
                              8,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_2()
{
    // 22kHz / 32-bit / 2ch
    RenderPCMWaveFormatExTone(22050,
                              32,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_3()
{
    // 3kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(3000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_4()
{
    // 88.2kHz / 16-bit / 1ch
    RenderPCMWaveFormatExTone(88200,
                              16,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_5()
{
    // 96kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(96000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_6()
{
    // 64kHz / 8-bit / 1ch
    RenderPCMWaveFormatExTone(64000,
                              8,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_7()
{
    // 96kHz / 8-bit / 2ch
    RenderPCMWaveFormatExTone(96000,
                              8,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_8()
{
    // 48kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(48000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_9()
{
    // 48kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(48000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_10()
{
    // 64kHz / 32-bit / 2ch
    RenderPCMWaveFormatExTone(64000,
                              32,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_11()
{
    // 11kHz / 24-bit / 2ch
    RenderPCMWaveFormatExTone(11025,
                              24,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_12()
{
    // 192kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(192000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_13()
{
    // 8kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(8000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_14()
{
    // 44.1kHz / 32-bit / 2ch
    RenderPCMWaveFormatExTone(44100,
                              32,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_15()
{
    // 32kHz / 8-bit / 2ch
    RenderPCMWaveFormatExTone(32000,
                              8,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_16()
{
    // 32kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(32000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_17()
{
    // 44.1kHz / 8-bit / 1ch
    RenderPCMWaveFormatExTone(44100,
                              8,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_18()
{
    // 3kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(3000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_19()
{
    // 22.05kHz / 16-bit / 1ch
    RenderPCMWaveFormatExTone(22050,
                              16,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_20()
{
    // 48kHz / 24-bit / 2ch
    RenderPCMWaveFormatExTone(48000,
                              24,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_21()
{
    // 48kHz / 32-bit / 2ch
    RenderPCMWaveFormatExTone(48000,
                              32,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_22()
{
    // 44.1kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(44100,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_23()
{
    // 96kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(96000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_24()
{
    // 16kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(16000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_25()
{
    // 11.025kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(11025,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_26()
{
    // 22.05kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(22050,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_27()
{
    // 64kHz / 16-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(64000,
                                      16,       // container
                                      16,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_28()
{
    // 16kHz / 24-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(16000,
                                      24,       // container
                                      24,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_29()
{
    // 64kHz / 24-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(64000,
                                      24,       // container
                                      24,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_30()
{
    // 11.025kHz / 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(11025,
                                      32,       // container
                                      32,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_31()
{
    // 32kHz / 16-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(32000,
                                      16,       // container
                                      16,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_32()
{
    // 3kHz / 8-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(3000,
                                      8,        // container
                                      8,        // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_33()
{
    // 88.2kHz/ 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(88200,
                                      32,       // container
                                      32,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_34()
{
    // 96kHz/ 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(96000,
                                      32,       // container
                                      32,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_35()
{
    // 16kHz/ 8-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(16000,
                                      8,        // container
                                      8,        // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_36()
{
    // 3kHz/ 32-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(3000,
                                      32,       // container
                                      32,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_37()
{
    // 8kHz/ 8-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(8000,
                                      8,        // container
                                      8,        // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_38()
{
    // 48kHz/ 8-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      8,        // container
                                      8,        // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_39()
{
    // 96kHz/ 16-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(96000,
                                      16,       // container
                                      16,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_40()
{
    // 11.025kHz/ 16-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(11025,
                                      16,       // container
                                      16,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_41()
{
    // 192kHz/ 24-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(192000,
                                      24,       // container
                                      24,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_42()
{
    // 8kHz/ 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(8000,
                                      32,       // container
                                      32,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_43()
{
    // 88.2kHz/ 24-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(88200,
                                      24,       // container
                                      24,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_44()
{
    // 22.050kHz/ 24-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(22050,
                                      24,       // container
                                      24,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_45()
{
    // 48kHz/ 32-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      32,       // container
                                      32,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_46()
{
    // 192kHz/ 8-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(192000,
                                      8,        // container
                                      8,        // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_47()
{
    // 32kHz/ 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(32000,
                                      32,       // container
                                      32,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_48()
{
    // 44.1kHz/ 24-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(44100,
                                      24,       // container
                                      24,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_49()
{
    // 22.05kHz/ 8-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(22050,
                                      8,        // container
                                      8,        // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_50()
{
    // 44.1kHz/ 16-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(44100,
                                      16,       // container
                                      16,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_51()
{
    // 16kHz/ 32-bit / 32ch
    RenderPCMWaveFormatExtensibleTone(16000,
                                      32,       // container
                                      32,       // valid bits 
                                      32,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_52()
{
    // 88.2kHz/ 8-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(88200,
                                      8,        // container
                                      8,        // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_53()
{
    // 16kHz/ 16-bit / 16ch
    RenderPCMWaveFormatExtensibleTone(16000,
                                      16,       // container
                                      16,       // valid bits 
                                      16,       // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_54()
{
    // 48kHz/ 32-bit / 2ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      20,       // container
                                      32,       // valid bits 
                                      2,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_55()
{
    // 48kHz/ 32-bit / 2ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      24,       // container
                                      32,       // valid bits 
                                      2,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_56()
{
    // 48kHz/ 32-bit / 2ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      32,       // container
                                      32,       // valid bits 
                                      2,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_57()
{
    // 48kHz/ 16-bit / 4ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      4,       // channels 
                                      KSAUDIO_SPEAKER_QUAD, // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_58()
{
    // 48kHz/ 16-bit / 4ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      4,       // channels 
                                      KSAUDIO_SPEAKER_SURROUND, // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_59()
{
    // 48kHz/ 16-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      6,       // channels 
                                      KSAUDIO_SPEAKER_5POINT1, // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_60()
{
    // 48kHz/ 16-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      8,       // channels 
                                      KSAUDIO_SPEAKER_7POINT1, // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_61()
{
    // 48kHz/ 16-bit / 1ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      1,       // channels 
                                      KSAUDIO_SPEAKER_MONO,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_Push_62()
{
    // 48kHz/ 16-bit / 2ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      2,       // channels 
                                      KSAUDIO_SPEAKER_STEREO, // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

//
// Compat-Mode Capture
//
DWORD TC_Capture_Compat_Push_0()
{
    // 192k / 32-bit / 1ch
    CapturePCMWaveFormatEx(192000,
                           32,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           TIMER_PUSH_CAPTURE);

    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_1()
{
    // 11kHz / 8-bit / 1ch
    CapturePCMWaveFormatEx(11025,
                           8,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_2()
{
    // 22kHz / 32-bit / 2ch
    CapturePCMWaveFormatEx(22050,
                           32,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_3()
{
    // 3kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(3000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_4()
{
    // 88.2kHz / 16-bit / 1ch
    CapturePCMWaveFormatEx(88200,
                           16,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_5()
{
    // 96kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(96000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_6()
{
    // 64kHz / 8-bit / 1ch
    CapturePCMWaveFormatEx(64000,
                           8,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_7()
{
    // 96kHz / 8-bit / 2ch
    CapturePCMWaveFormatEx(96000,
                           8,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_8()
{
    // 48kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(48000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_9()
{
    // 48kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(48000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_10()
{
    // 64kHz / 32-bit / 2ch
    CapturePCMWaveFormatEx(64000,
                           32,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_11()
{
    // 11kHz / 24-bit / 2ch
    CapturePCMWaveFormatEx(11025,
                           24,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_12()
{
    // 192kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(192000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_13()
{
    // 8kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(8000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_14()
{
    // 44.1kHz / 32-bit / 2ch
    CapturePCMWaveFormatEx(44100,
                           32,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_15()
{
    // 32kHz / 8-bit / 2ch
    CapturePCMWaveFormatEx(32000,
                           8,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_16()
{
    // 32kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(32000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_17()
{
    // 44.1kHz / 8-bit / 1ch
    CapturePCMWaveFormatEx(44100,
                           8,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_18()
{
    // 3kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(3000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_19()
{
    // 22.05kHz / 16-bit / 1ch
    CapturePCMWaveFormatEx(22050,
                           16,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_20()
{
    // 48kHz / 24-bit / 2ch
    CapturePCMWaveFormatEx(48000,
                           24,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_21()
{
    // 48kHz / 32-bit / 2ch
    CapturePCMWaveFormatEx(48000,
                           32,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_22()
{
    // 44.1kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(44100,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_23()
{
    // 96kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(96000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_24()
{
    // 16kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(16000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_25()
{
    // 11.025kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(11025,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_26()
{
    // 22.05kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(22050,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_27()
{
    // 64kHz / 16-bit / 8ch
    CapturePCMWaveFormatExtensible(64000,
                                   16,       // container
                                   16,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_28()
{
    // 16kHz / 24-bit / 8ch
    CapturePCMWaveFormatExtensible(16000,
                                   24,       // container
                                   24,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_29()
{
    // 64kHz / 24-bit / 6ch
    CapturePCMWaveFormatExtensible(64000,
                                   24,       // container
                                   24,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_30()
{
    // 11.025kHz / 32-bit / 8ch
    CapturePCMWaveFormatExtensible(11025,
                                   32,       // container
                                   32,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_31()
{
    // 32kHz / 16-bit / 6ch
    CapturePCMWaveFormatExtensible(32000,
                                   16,       // container
                                   16,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_32()
{
    // 3kHz / 8-bit / 8ch
    CapturePCMWaveFormatExtensible(3000,
                                   8,        // container
                                   8,        // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_33()
{
    // 88.2kHz/ 32-bit / 8ch
    CapturePCMWaveFormatExtensible(88200,
                                   32,       // container
                                   32,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
 
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_34()
{
    // 96kHz/ 32-bit / 8ch
    CapturePCMWaveFormatExtensible(96000,
                                   32,       // container
                                   32,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_35()
{
    // 16kHz/ 8-bit / 6ch
    CapturePCMWaveFormatExtensible(16000,
                                   8,        // container
                                   8,        // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_36()
{
    // 3kHz/ 32-bit / 6ch
    CapturePCMWaveFormatExtensible(3000,
                                   32,       // container
                                   32,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_37()
{
    // 8kHz/ 8-bit / 6ch
    CapturePCMWaveFormatExtensible(8000,
                                   8,        // container
                                   8,        // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_38()
{
    // 48kHz/ 8-bit / 8ch
    CapturePCMWaveFormatExtensible(48000,
                                   8,        // container
                                   8,        // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_39()
{
    // 96kHz/ 16-bit / 6ch
    CapturePCMWaveFormatExtensible(96000,
                                   16,       // container
                                   16,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_40()
{
    // 11.025kHz/ 16-bit / 6ch
    CapturePCMWaveFormatExtensible(11025,
                                   16,       // container
                                   16,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_41()
{
    // 192kHz/ 24-bit / 8ch
    CapturePCMWaveFormatExtensible(192000,
                                   24,       // container
                                   24,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_42()
{
    // 8kHz/ 32-bit / 8ch
    CapturePCMWaveFormatExtensible(8000,
                                   32,       // container
                                   32,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_43()
{
    // 88.2kHz/ 24-bit / 8ch
    CapturePCMWaveFormatExtensible(88200,
                                   24,       // container
                                   24,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_44()
{
    // 22.050kHz/ 24-bit / 8ch
    CapturePCMWaveFormatExtensible(22050,
                                   24,       // container
                                   24,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_45()
{
    // 48kHz/ 32-bit / 6ch
    CapturePCMWaveFormatExtensible(48000,
                                   32,       // container
                                   32,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_46()
{
    // 192kHz/ 8-bit / 6ch
    CapturePCMWaveFormatExtensible(192000,
                                   8,        // container
                                   8,        // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_47()
{
    // 32kHz/ 32-bit / 8ch
    CapturePCMWaveFormatExtensible(32000,
                                   32,       // container
                                   32,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_48()
{
    // 44.1kHz/ 24-bit / 6ch
    CapturePCMWaveFormatExtensible(44100,
                                   24,       // container
                                   24,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_49()
{
    // 22.05kHz/ 8-bit / 6ch
    CapturePCMWaveFormatExtensible(22050,
                                   8,        // container
                                   8,        // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_50()
{
    // 44.1kHz/ 16-bit / 8ch
    CapturePCMWaveFormatExtensible(44100,
                                   16,       // container
                                   16,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_51()
{
    // 16kHz/ 32-bit / 32ch
    CapturePCMWaveFormatExtensible(16000,
                                   32,       // container
                                   32,       // valid bits 
                                   32,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_52()
{
    // 88.2kHz/ 8-bit / 8ch
    CapturePCMWaveFormatExtensible(88200,
                                   8,        // container
                                   8,        // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_53()
{
    // 16kHz/ 16-bit / 16ch
    CapturePCMWaveFormatExtensible(16000,
                                   16,       // container
                                   16,       // valid bits 
                                   16,       // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_54()
{
    // 48kHz/ 32-bit / 2ch
    CapturePCMWaveFormatExtensible(48000,
                                   20,       // container
                                   32,       // valid bits 
                                   2,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_55()
{
    // 48kHz/ 32-bit / 2ch
    CapturePCMWaveFormatExtensible(48000,
                                   24,       // container
                                   32,       // valid bits 
                                   2,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_56()
{
    // 48kHz/ 32-bit / 2ch
    CapturePCMWaveFormatExtensible(48000,
                                   32,       // container
                                   32,       // valid bits 
                                   2,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_57()
{
    // 48kHz/ 16-bit / 4ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   4,       // channels 
                                   KSAUDIO_SPEAKER_QUAD, // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_58()
{
    // 48kHz/ 16-bit / 4ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   4,       // channels 
                                   KSAUDIO_SPEAKER_SURROUND, // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
 
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_59()
{
    // 48kHz/ 16-bit / 6ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   6,       // channels 
                                   KSAUDIO_SPEAKER_5POINT1, // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_60()
{
    // 48kHz/ 16-bit / 8ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   8,       // channels 
                                   KSAUDIO_SPEAKER_7POINT1, // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_61()
{
    // 48kHz/ 16-bit / 1ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   1,       // channels 
                                   KSAUDIO_SPEAKER_MONO,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Push_62()
{
    // 48kHz/ 16-bit / 2ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   2,       // channels 
                                   KSAUDIO_SPEAKER_STEREO, // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_0()
{
    // 192k / 32-bit / 1ch
    CapturePCMWaveFormatEx(192000,
                           32,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           EVENT_PULL_CAPTURE);

    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_1()
{
    // 11kHz / 8-bit / 1ch
    CapturePCMWaveFormatEx(11025,
                           8,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_2()
{
    // 22kHz / 32-bit / 2ch
    CapturePCMWaveFormatEx(22050,
                           32,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_3()
{
    // 3kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(3000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_4()
{
    // 88.2kHz / 16-bit / 1ch
    CapturePCMWaveFormatEx(88200,
                           16,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_5()
{
    // 96kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(96000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_6()
{
    // 64kHz / 8-bit / 1ch
    CapturePCMWaveFormatEx(64000,
                           8,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_7()
{
    // 96kHz / 8-bit / 2ch
    CapturePCMWaveFormatEx(96000,
                           8,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_8()
{
    // 48kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(48000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_9()
{
    // 48kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(48000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_10()
{
    // 64kHz / 32-bit / 2ch
    CapturePCMWaveFormatEx(64000,
                           32,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_11()
{
    // 11kHz / 24-bit / 2ch
    CapturePCMWaveFormatEx(11025,
                           24,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_12()
{
    // 192kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(192000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_13()
{
    // 8kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(8000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_14()
{
    // 44.1kHz / 32-bit / 2ch
    CapturePCMWaveFormatEx(44100,
                           32,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_15()
{
    // 32kHz / 8-bit / 2ch
    CapturePCMWaveFormatEx(32000,
                           8,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_16()
{
    // 32kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(32000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_17()
{
    // 44.1kHz / 8-bit / 1ch
    CapturePCMWaveFormatEx(44100,
                           8,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_18()
{
    // 3kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(3000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_19()
{
    // 22.05kHz / 16-bit / 1ch
    CapturePCMWaveFormatEx(22050,
                           16,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_20()
{
    // 48kHz / 24-bit / 2ch
    CapturePCMWaveFormatEx(48000,
                           24,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_21()
{
    // 48kHz / 32-bit / 2ch
    CapturePCMWaveFormatEx(48000,
                           32,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_22()
{
    // 44.1kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(44100,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_23()
{
    // 96kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(96000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_24()
{
    // 16kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(16000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_25()
{
    // 11.025kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(11025,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_26()
{
    // 22.05kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(22050,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_27()
{
    // 64kHz / 16-bit / 8ch
    CapturePCMWaveFormatExtensible(64000,
                                   16,       // container
                                   16,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_28()
{
    // 16kHz / 24-bit / 8ch
    CapturePCMWaveFormatExtensible(16000,
                                   24,       // container
                                   24,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_29()
{
    // 64kHz / 24-bit / 6ch
    CapturePCMWaveFormatExtensible(64000,
                                   24,       // container
                                   24,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_30()
{
    // 11.025kHz / 32-bit / 8ch
    CapturePCMWaveFormatExtensible(11025,
                                   32,       // container
                                   32,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_31()
{
    // 32kHz / 16-bit / 6ch
    CapturePCMWaveFormatExtensible(32000,
                                   16,       // container
                                   16,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_32()
{
    // 3kHz / 8-bit / 8ch
    CapturePCMWaveFormatExtensible(3000,
                                   8,        // container
                                   8,        // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_33()
{
    // 88.2kHz/ 32-bit / 8ch
    CapturePCMWaveFormatExtensible(88200,
                                   32,       // container
                                   32,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
 
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_34()
{
    // 96kHz/ 32-bit / 8ch
    CapturePCMWaveFormatExtensible(96000,
                                   32,       // container
                                   32,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_35()
{
    // 16kHz/ 8-bit / 6ch
    CapturePCMWaveFormatExtensible(16000,
                                   8,        // container
                                   8,        // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_36()
{
    // 3kHz/ 32-bit / 6ch
    CapturePCMWaveFormatExtensible(3000,
                                   32,       // container
                                   32,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_37()
{
    // 8kHz/ 8-bit / 6ch
    CapturePCMWaveFormatExtensible(8000,
                                   8,        // container
                                   8,        // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_38()
{
    // 48kHz/ 8-bit / 8ch
    CapturePCMWaveFormatExtensible(48000,
                                   8,        // container
                                   8,        // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_39()
{
    // 96kHz/ 16-bit / 6ch
    CapturePCMWaveFormatExtensible(96000,
                                   16,       // container
                                   16,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_40()
{
    // 11.025kHz/ 16-bit / 6ch
    CapturePCMWaveFormatExtensible(11025,
                                   16,       // container
                                   16,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_41()
{
    // 192kHz/ 24-bit / 8ch
    CapturePCMWaveFormatExtensible(192000,
                                   24,       // container
                                   24,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_42()
{
    // 8kHz/ 32-bit / 8ch
    CapturePCMWaveFormatExtensible(8000,
                                   32,       // container
                                   32,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_43()
{
    // 88.2kHz/ 24-bit / 8ch
    CapturePCMWaveFormatExtensible(88200,
                                   24,       // container
                                   24,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_44()
{
    // 22.050kHz/ 24-bit / 8ch
    CapturePCMWaveFormatExtensible(22050,
                                   24,       // container
                                   24,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_45()
{
    // 48kHz/ 32-bit / 6ch
    CapturePCMWaveFormatExtensible(48000,
                                   32,       // container
                                   32,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_46()
{
    // 192kHz/ 8-bit / 6ch
    CapturePCMWaveFormatExtensible(192000,
                                   8,        // container
                                   8,        // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_47()
{
    // 32kHz/ 32-bit / 8ch
    CapturePCMWaveFormatExtensible(32000,
                                   32,       // container
                                   32,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_48()
{
    // 44.1kHz/ 24-bit / 6ch
    CapturePCMWaveFormatExtensible(44100,
                                   24,       // container
                                   24,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_49()
{
    // 22.05kHz/ 8-bit / 6ch
    CapturePCMWaveFormatExtensible(22050,
                                   8,        // container
                                   8,        // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_50()
{
    // 44.1kHz/ 16-bit / 8ch
    CapturePCMWaveFormatExtensible(44100,
                                   16,       // container
                                   16,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_51()
{
    // 16kHz/ 32-bit / 32ch
    CapturePCMWaveFormatExtensible(16000,
                                   32,       // container
                                   32,       // valid bits 
                                   32,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_52()
{
    // 88.2kHz/ 8-bit / 8ch
    CapturePCMWaveFormatExtensible(88200,
                                   8,        // container
                                   8,        // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_53()
{
    // 16kHz/ 16-bit / 16ch
    CapturePCMWaveFormatExtensible(16000,
                                   16,       // container
                                   16,       // valid bits 
                                   16,       // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_54()
{
    // 48kHz/ 32-bit / 2ch
    CapturePCMWaveFormatExtensible(48000,
                                   20,       // container
                                   32,       // valid bits 
                                   2,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_55()
{
    // 48kHz/ 32-bit / 2ch
    CapturePCMWaveFormatExtensible(48000,
                                   24,       // container
                                   32,       // valid bits 
                                   2,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_56()
{
    // 48kHz/ 32-bit / 2ch
    CapturePCMWaveFormatExtensible(48000,
                                   32,       // container
                                   32,       // valid bits 
                                   2,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_57()
{
    // 48kHz/ 16-bit / 4ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   4,       // channels 
                                   KSAUDIO_SPEAKER_QUAD, // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_58()
{
    // 48kHz/ 16-bit / 4ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   4,       // channels 
                                   KSAUDIO_SPEAKER_SURROUND, // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
 
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_59()
{
    // 48kHz/ 16-bit / 6ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   6,       // channels 
                                   KSAUDIO_SPEAKER_5POINT1, // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_60()
{
    // 48kHz/ 16-bit / 8ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   8,       // channels 
                                   KSAUDIO_SPEAKER_7POINT1, // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_61()
{
    // 48kHz/ 16-bit / 1ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   1,       // channels 
                                   KSAUDIO_SPEAKER_MONO,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_Pull_62()
{
    // 48kHz/ 16-bit / 2ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   2,       // channels 
                                   KSAUDIO_SPEAKER_STEREO, // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}


///
// Compat Mode (SRC Default Quality) Testing
///
DWORD TC_Render_Compat_SRC_Enhanced_Pull_0()
{
    // 192k / 32-bit / 1ch
    RenderPCMWaveFormatExTone(192000,
                              32,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              EVENT_PULL_RENDER);
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_1()
{
    // 11kHz / 8-bit / 1ch
    RenderPCMWaveFormatExTone(11025,
                              8,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_2()
{
    // 22kHz / 32-bit / 2ch
    RenderPCMWaveFormatExTone(22050,
                              32,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_3()
{
    // 3kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(3000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_4()
{
    // 88.2kHz / 16-bit / 1ch
    RenderPCMWaveFormatExTone(88200,
                              16,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_5()
{
    // 96kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(96000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_6()
{
    // 64kHz / 8-bit / 1ch
    RenderPCMWaveFormatExTone(64000,
                              8,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_7()
{
    // 96kHz / 8-bit / 2ch
    RenderPCMWaveFormatExTone(96000,
                              8,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_8()
{
    // 48kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(48000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_9()
{
    // 48kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(48000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_10()
{
    // 64kHz / 32-bit / 2ch
    RenderPCMWaveFormatExTone(64000,
                              32,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_11()
{
    // 11kHz / 24-bit / 2ch
    RenderPCMWaveFormatExTone(11025,
                              24,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_12()
{
    // 192kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(192000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_13()
{
    // 8kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(8000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_14()
{
    // 44.1kHz / 32-bit / 2ch
    RenderPCMWaveFormatExTone(44100,
                              32,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_15()
{
    // 32kHz / 8-bit / 2ch
    RenderPCMWaveFormatExTone(32000,
                              8,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_16()
{
    // 32kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(32000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_17()
{
    // 44.1kHz / 8-bit / 1ch
    RenderPCMWaveFormatExTone(44100,
                              8,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_18()
{
    // 3kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(3000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_19()
{
    // 22.05kHz / 16-bit / 1ch
    RenderPCMWaveFormatExTone(22050,
                              16,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_20()
{
    // 48kHz / 24-bit / 2ch
    RenderPCMWaveFormatExTone(48000,
                              24,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_21()
{
    // 48kHz / 32-bit / 2ch
    RenderPCMWaveFormatExTone(48000,
                              32,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_22()
{
    // 44.1kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(44100,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_23()
{
    // 96kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(96000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_24()
{
    // 16kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(16000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_25()
{
    // 11.025kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(11025,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_26()
{
    // 22.05kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(22050,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_27()
{
    // 64kHz / 16-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(64000,
                                      16,       // container
                                      16,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_28()
{
    // 16kHz / 24-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(16000,
                                      24,       // container
                                      24,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_29()
{
    // 64kHz / 24-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(64000,
                                      24,       // container
                                      24,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_30()
{
    // 11.025kHz / 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(11025,
                                      32,       // container
                                      32,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_31()
{
    // 32kHz / 16-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(32000,
                                      16,       // container
                                      16,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_32()
{
    // 3kHz / 8-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(3000,
                                      8,        // container
                                      8,        // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_33()
{
    // 88.2kHz/ 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(88200,
                                      32,       // container
                                      32,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_34()
{
    // 96kHz/ 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(96000,
                                      32,       // container
                                      32,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_35()
{
    // 16kHz/ 8-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(16000,
                                      8,        // container
                                      8,        // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_36()
{
    // 3kHz/ 32-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(3000,
                                      32,       // container
                                      32,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_37()
{
    // 8kHz/ 8-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(8000,
                                      8,        // container
                                      8,        // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_38()
{
    // 48kHz/ 8-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      8,        // container
                                      8,        // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_39()
{
    // 96kHz/ 16-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(96000,
                                      16,       // container
                                      16,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_40()
{
    // 11.025kHz/ 16-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(11025,
                                      16,       // container
                                      16,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_41()
{
    // 192kHz/ 24-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(192000,
                                      24,       // container
                                      24,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_42()
{
    // 8kHz/ 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(8000,
                                      32,       // container
                                      32,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_43()
{
    // 88.2kHz/ 24-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(88200,
                                      24,       // container
                                      24,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_44()
{
    // 22.050kHz/ 24-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(22050,
                                      24,       // container
                                      24,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_45()
{
    // 48kHz/ 32-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      32,       // container
                                      32,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_46()
{
    // 192kHz/ 8-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(192000,
                                      8,        // container
                                      8,        // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_47()
{
    // 32kHz/ 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(32000,
                                      32,       // container
                                      32,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_48()
{
    // 44.1kHz/ 24-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(44100,
                                      24,       // container
                                      24,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_49()
{
    // 22.05kHz/ 8-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(22050,
                                      8,        // container
                                      8,        // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_50()
{
    // 44.1kHz/ 16-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(44100,
                                      16,       // container
                                      16,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_51()
{
    // 16kHz/ 32-bit / 32ch
    RenderPCMWaveFormatExtensibleTone(16000,
                                      32,       // container
                                      32,       // valid bits 
                                      32,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_52()
{
    // 88.2kHz/ 8-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(88200,
                                      8,        // container
                                      8,        // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_53()
{
    // 16kHz/ 16-bit / 16ch
    RenderPCMWaveFormatExtensibleTone(16000,
                                      16,       // container
                                      16,       // valid bits 
                                      16,       // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_54()
{
    // 48kHz/ 32-bit / 2ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      20,       // container
                                      32,       // valid bits 
                                      2,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_55()
{
    // 48kHz/ 32-bit / 2ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      24,       // container
                                      32,       // valid bits 
                                      2,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_56()
{
    // 48kHz/ 32-bit / 2ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      32,       // container
                                      32,       // valid bits 
                                      2,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_57()
{
    // 48kHz/ 16-bit / 4ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      4,       // channels 
                                      KSAUDIO_SPEAKER_QUAD, // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_58()
{
    // 48kHz/ 16-bit / 4ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      4,       // channels 
                                      KSAUDIO_SPEAKER_SURROUND, // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_59()
{
    // 48kHz/ 16-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      6,       // channels 
                                      KSAUDIO_SPEAKER_5POINT1, // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_60()
{
    // 48kHz/ 16-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      8,       // channels 
                                      KSAUDIO_SPEAKER_7POINT1, // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_61()
{
    // 48kHz/ 16-bit / 1ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      1,       // channels 
                                      KSAUDIO_SPEAKER_MONO,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Pull_62()
{
    // 48kHz/ 16-bit / 2ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      2,       // channels 
                                      KSAUDIO_SPEAKER_STEREO, // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_0()
{
    // 192k / 32-bit / 1ch
    RenderPCMWaveFormatExTone(192000,
                              32,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              TIMER_PUSH_RENDER);

    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_1()
{
    // 11kHz / 8-bit / 1ch
    RenderPCMWaveFormatExTone(11025,
                              8,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_2()
{
    // 22kHz / 32-bit / 2ch
    RenderPCMWaveFormatExTone(22050,
                              32,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_3()
{
    // 3kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(3000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_4()
{
    // 88.2kHz / 16-bit / 1ch
    RenderPCMWaveFormatExTone(88200,
                              16,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_5()
{
    // 96kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(96000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_6()
{
    // 64kHz / 8-bit / 1ch
    RenderPCMWaveFormatExTone(64000,
                              8,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_7()
{
    // 96kHz / 8-bit / 2ch
    RenderPCMWaveFormatExTone(96000,
                              8,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_8()
{
    // 48kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(48000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_9()
{
    // 48kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(48000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_10()
{
    // 64kHz / 32-bit / 2ch
    RenderPCMWaveFormatExTone(64000,
                              32,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_11()
{
    // 11kHz / 24-bit / 2ch
    RenderPCMWaveFormatExTone(11025,
                              24,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_12()
{
    // 192kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(192000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_13()
{
    // 8kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(8000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_14()
{
    // 44.1kHz / 32-bit / 2ch
    RenderPCMWaveFormatExTone(44100,
                              32,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_15()
{
    // 32kHz / 8-bit / 2ch
    RenderPCMWaveFormatExTone(32000,
                              8,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_16()
{
    // 32kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(32000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_17()
{
    // 44.1kHz / 8-bit / 1ch
    RenderPCMWaveFormatExTone(44100,
                              8,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_18()
{
    // 3kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(3000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_19()
{
    // 22.05kHz / 16-bit / 1ch
    RenderPCMWaveFormatExTone(22050,
                              16,
                              1,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_20()
{
    // 48kHz / 24-bit / 2ch
    RenderPCMWaveFormatExTone(48000,
                              24,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_21()
{
    // 48kHz / 32-bit / 2ch
    RenderPCMWaveFormatExTone(48000,
                              32,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_22()
{
    // 44.1kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(44100,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_23()
{
    // 96kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(96000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_24()
{
    // 16kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(16000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_25()
{
    // 11.025kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(11025,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_26()
{
    // 22.05kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(22050,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                              AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_27()
{
    // 64kHz / 16-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(64000,
                                      16,       // container
                                      16,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_28()
{
    // 16kHz / 24-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(16000,
                                      24,       // container
                                      24,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_29()
{
    // 64kHz / 24-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(64000,
                                      24,       // container
                                      24,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_30()
{
    // 11.025kHz / 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(11025,
                                      32,       // container
                                      32,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_31()
{
    // 32kHz / 16-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(32000,
                                      16,       // container
                                      16,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_32()
{
    // 3kHz / 8-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(3000,
                                      8,        // container
                                      8,        // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_33()
{
    // 88.2kHz/ 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(88200,
                                      32,       // container
                                      32,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_34()
{
    // 96kHz/ 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(96000,
                                      32,       // container
                                      32,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_35()
{
    // 16kHz/ 8-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(16000,
                                      8,        // container
                                      8,        // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_36()
{
    // 3kHz/ 32-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(3000,
                                      32,       // container
                                      32,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_37()
{
    // 8kHz/ 8-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(8000,
                                      8,        // container
                                      8,        // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_38()
{
    // 48kHz/ 8-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      8,        // container
                                      8,        // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_39()
{
    // 96kHz/ 16-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(96000,
                                      16,       // container
                                      16,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_40()
{
    // 11.025kHz/ 16-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(11025,
                                      16,       // container
                                      16,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_41()
{
    // 192kHz/ 24-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(192000,
                                      24,       // container
                                      24,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_42()
{
    // 8kHz/ 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(8000,
                                      32,       // container
                                      32,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_43()
{
    // 88.2kHz/ 24-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(88200,
                                      24,       // container
                                      24,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_44()
{
    // 22.050kHz/ 24-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(22050,
                                      24,       // container
                                      24,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_45()
{
    // 48kHz/ 32-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      32,       // container
                                      32,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_46()
{
    // 192kHz/ 8-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(192000,
                                      8,        // container
                                      8,        // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_47()
{
    // 32kHz/ 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(32000,
                                      32,       // container
                                      32,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_48()
{
    // 44.1kHz/ 24-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(44100,
                                      24,       // container
                                      24,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_49()
{
    // 22.05kHz/ 8-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(22050,
                                      8,        // container
                                      8,        // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_50()
{
    // 44.1kHz/ 16-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(44100,
                                      16,       // container
                                      16,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_51()
{
    // 16kHz/ 32-bit / 32ch
    RenderPCMWaveFormatExtensibleTone(16000,
                                      32,       // container
                                      32,       // valid bits 
                                      32,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_52()
{
    // 88.2kHz/ 8-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(88200,
                                      8,        // container
                                      8,        // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_53()
{
    // 16kHz/ 16-bit / 16ch
    RenderPCMWaveFormatExtensibleTone(16000,
                                      16,       // container
                                      16,       // valid bits 
                                      16,       // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_54()
{
    // 48kHz/ 32-bit / 2ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      20,       // container
                                      32,       // valid bits 
                                      2,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_55()
{
    // 48kHz/ 32-bit / 2ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      24,       // container
                                      32,       // valid bits 
                                      2,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_56()
{
    // 48kHz/ 32-bit / 2ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      32,       // container
                                      32,       // valid bits 
                                      2,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_57()
{
    // 48kHz/ 16-bit / 4ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      4,       // channels 
                                      KSAUDIO_SPEAKER_QUAD, // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_58()
{
    // 48kHz/ 16-bit / 4ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      4,       // channels 
                                      KSAUDIO_SPEAKER_SURROUND, // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_59()
{
    // 48kHz/ 16-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      6,       // channels 
                                      KSAUDIO_SPEAKER_5POINT1, // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_60()
{
    // 48kHz/ 16-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      8,       // channels 
                                      KSAUDIO_SPEAKER_7POINT1, // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_61()
{
    // 48kHz/ 16-bit / 1ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      1,       // channels 
                                      KSAUDIO_SPEAKER_MONO,        // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Compat_SRC_Enhanced_Push_62()
{
    // 48kHz/ 16-bit / 2ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      2,       // channels 
                                      KSAUDIO_SPEAKER_STEREO, // channel mask
                                      AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

//
// Compat-Mode (SRC Default Quality) Capture
//
DWORD TC_Capture_Compat_SRC_Enhanced_Push_0()
{
    // 192k / 32-bit / 1ch
    CapturePCMWaveFormatEx(192000,
                           32,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           TIMER_PUSH_CAPTURE);

    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_1()
{
    // 11kHz / 8-bit / 1ch
    CapturePCMWaveFormatEx(11025,
                           8,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_2()
{
    // 22kHz / 32-bit / 2ch
    CapturePCMWaveFormatEx(22050,
                           32,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_3()
{
    // 3kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(3000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_4()
{
    // 88.2kHz / 16-bit / 1ch
    CapturePCMWaveFormatEx(88200,
                           16,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_5()
{
    // 96kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(96000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_6()
{
    // 64kHz / 8-bit / 1ch
    CapturePCMWaveFormatEx(64000,
                           8,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_7()
{
    // 96kHz / 8-bit / 2ch
    CapturePCMWaveFormatEx(96000,
                           8,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_8()
{
    // 48kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(48000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_9()
{
    // 48kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(48000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_10()
{
    // 64kHz / 32-bit / 2ch
    CapturePCMWaveFormatEx(64000,
                           32,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_11()
{
    // 11kHz / 24-bit / 2ch
    CapturePCMWaveFormatEx(11025,
                           24,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_12()
{
    // 192kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(192000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_13()
{
    // 8kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(8000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_14()
{
    // 44.1kHz / 32-bit / 2ch
    CapturePCMWaveFormatEx(44100,
                           32,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_15()
{
    // 32kHz / 8-bit / 2ch
    CapturePCMWaveFormatEx(32000,
                           8,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_16()
{
    // 32kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(32000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_17()
{
    // 44.1kHz / 8-bit / 1ch
    CapturePCMWaveFormatEx(44100,
                           8,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_18()
{
    // 3kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(3000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_19()
{
    // 22.05kHz / 16-bit / 1ch
    CapturePCMWaveFormatEx(22050,
                           16,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_20()
{
    // 48kHz / 24-bit / 2ch
    CapturePCMWaveFormatEx(48000,
                           24,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_21()
{
    // 48kHz / 32-bit / 2ch
    CapturePCMWaveFormatEx(48000,
                           32,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_22()
{
    // 44.1kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(44100,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_23()
{
    // 96kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(96000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_24()
{
    // 16kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(16000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_25()
{
    // 11.025kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(11025,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_26()
{
    // 22.05kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(22050,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_27()
{
    // 64kHz / 16-bit / 8ch
    CapturePCMWaveFormatExtensible(64000,
                                   16,       // container
                                   16,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_28()
{
    // 16kHz / 24-bit / 8ch
    CapturePCMWaveFormatExtensible(16000,
                                   24,       // container
                                   24,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_29()
{
    // 64kHz / 24-bit / 6ch
    CapturePCMWaveFormatExtensible(64000,
                                   24,       // container
                                   24,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_30()
{
    // 11.025kHz / 32-bit / 8ch
    CapturePCMWaveFormatExtensible(11025,
                                   32,       // container
                                   32,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_31()
{
    // 32kHz / 16-bit / 6ch
    CapturePCMWaveFormatExtensible(32000,
                                   16,       // container
                                   16,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_32()
{
    // 3kHz / 8-bit / 8ch
    CapturePCMWaveFormatExtensible(3000,
                                   8,        // container
                                   8,        // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_33()
{
    // 88.2kHz/ 32-bit / 8ch
    CapturePCMWaveFormatExtensible(88200,
                                   32,       // container
                                   32,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
 
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_34()
{
    // 96kHz/ 32-bit / 8ch
    CapturePCMWaveFormatExtensible(96000,
                                   32,       // container
                                   32,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_35()
{
    // 16kHz/ 8-bit / 6ch
    CapturePCMWaveFormatExtensible(16000,
                                   8,        // container
                                   8,        // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_36()
{
    // 3kHz/ 32-bit / 6ch
    CapturePCMWaveFormatExtensible(3000,
                                   32,       // container
                                   32,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_37()
{
    // 8kHz/ 8-bit / 6ch
    CapturePCMWaveFormatExtensible(8000,
                                   8,        // container
                                   8,        // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_38()
{
    // 48kHz/ 8-bit / 8ch
    CapturePCMWaveFormatExtensible(48000,
                                   8,        // container
                                   8,        // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_39()
{
    // 96kHz/ 16-bit / 6ch
    CapturePCMWaveFormatExtensible(96000,
                                   16,       // container
                                   16,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_40()
{
    // 11.025kHz/ 16-bit / 6ch
    CapturePCMWaveFormatExtensible(11025,
                                   16,       // container
                                   16,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_41()
{
    // 192kHz/ 24-bit / 8ch
    CapturePCMWaveFormatExtensible(192000,
                                   24,       // container
                                   24,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_42()
{
    // 8kHz/ 32-bit / 8ch
    CapturePCMWaveFormatExtensible(8000,
                                   32,       // container
                                   32,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_43()
{
    // 88.2kHz/ 24-bit / 8ch
    CapturePCMWaveFormatExtensible(88200,
                                   24,       // container
                                   24,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_44()
{
    // 22.050kHz/ 24-bit / 8ch
    CapturePCMWaveFormatExtensible(22050,
                                   24,       // container
                                   24,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_45()
{
    // 48kHz/ 32-bit / 6ch
    CapturePCMWaveFormatExtensible(48000,
                                   32,       // container
                                   32,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_46()
{
    // 192kHz/ 8-bit / 6ch
    CapturePCMWaveFormatExtensible(192000,
                                   8,        // container
                                   8,        // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_47()
{
    // 32kHz/ 32-bit / 8ch
    CapturePCMWaveFormatExtensible(32000,
                                   32,       // container
                                   32,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_48()
{
    // 44.1kHz/ 24-bit / 6ch
    CapturePCMWaveFormatExtensible(44100,
                                   24,       // container
                                   24,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_49()
{
    // 22.05kHz/ 8-bit / 6ch
    CapturePCMWaveFormatExtensible(22050,
                                   8,        // container
                                   8,        // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_50()
{
    // 44.1kHz/ 16-bit / 8ch
    CapturePCMWaveFormatExtensible(44100,
                                   16,       // container
                                   16,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_51()
{
    // 16kHz/ 32-bit / 32ch
    CapturePCMWaveFormatExtensible(16000,
                                   32,       // container
                                   32,       // valid bits 
                                   32,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_52()
{
    // 88.2kHz/ 8-bit / 8ch
    CapturePCMWaveFormatExtensible(88200,
                                   8,        // container
                                   8,        // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_53()
{
    // 16kHz/ 16-bit / 16ch
    CapturePCMWaveFormatExtensible(16000,
                                   16,       // container
                                   16,       // valid bits 
                                   16,       // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_54()
{
    // 48kHz/ 32-bit / 2ch
    CapturePCMWaveFormatExtensible(48000,
                                   20,       // container
                                   32,       // valid bits 
                                   2,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_55()
{
    // 48kHz/ 32-bit / 2ch
    CapturePCMWaveFormatExtensible(48000,
                                   24,       // container
                                   32,       // valid bits 
                                   2,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_56()
{
    // 48kHz/ 32-bit / 2ch
    CapturePCMWaveFormatExtensible(48000,
                                   32,       // container
                                   32,       // valid bits 
                                   2,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_57()
{
    // 48kHz/ 16-bit / 4ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   4,       // channels 
                                   KSAUDIO_SPEAKER_QUAD, // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_58()
{
    // 48kHz/ 16-bit / 4ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   4,       // channels 
                                   KSAUDIO_SPEAKER_SURROUND, // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
 
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_59()
{
    // 48kHz/ 16-bit / 6ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   6,       // channels 
                                   KSAUDIO_SPEAKER_5POINT1, // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_60()
{
    // 48kHz/ 16-bit / 8ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   8,       // channels 
                                   KSAUDIO_SPEAKER_7POINT1, // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_61()
{
    // 48kHz/ 16-bit / 1ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   1,       // channels 
                                   KSAUDIO_SPEAKER_MONO,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Push_62()
{
    // 48kHz/ 16-bit / 2ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   2,       // channels 
                                   KSAUDIO_SPEAKER_STEREO, // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_0()
{
    // 192k / 32-bit / 1ch
    CapturePCMWaveFormatEx(192000,
                           32,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           EVENT_PULL_CAPTURE);

    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_1()
{
    // 11kHz / 8-bit / 1ch
    CapturePCMWaveFormatEx(11025,
                           8,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_2()
{
    // 22kHz / 32-bit / 2ch
    CapturePCMWaveFormatEx(22050,
                           32,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_3()
{
    // 3kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(3000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_4()
{
    // 88.2kHz / 16-bit / 1ch
    CapturePCMWaveFormatEx(88200,
                           16,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_5()
{
    // 96kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(96000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_6()
{
    // 64kHz / 8-bit / 1ch
    CapturePCMWaveFormatEx(64000,
                           8,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_7()
{
    // 96kHz / 8-bit / 2ch
    CapturePCMWaveFormatEx(96000,
                           8,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_8()
{
    // 48kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(48000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_9()
{
    // 48kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(48000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_10()
{
    // 64kHz / 32-bit / 2ch
    CapturePCMWaveFormatEx(64000,
                           32,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_11()
{
    // 11kHz / 24-bit / 2ch
    CapturePCMWaveFormatEx(11025,
                           24,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_12()
{
    // 192kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(192000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_13()
{
    // 8kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(8000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_14()
{
    // 44.1kHz / 32-bit / 2ch
    CapturePCMWaveFormatEx(44100,
                           32,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_15()
{
    // 32kHz / 8-bit / 2ch
    CapturePCMWaveFormatEx(32000,
                           8,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_16()
{
    // 32kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(32000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_17()
{
    // 44.1kHz / 8-bit / 1ch
    CapturePCMWaveFormatEx(44100,
                           8,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_18()
{
    // 3kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(3000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_19()
{
    // 22.05kHz / 16-bit / 1ch
    CapturePCMWaveFormatEx(22050,
                           16,
                           1,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_20()
{
    // 48kHz / 24-bit / 2ch
    CapturePCMWaveFormatEx(48000,
                           24,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_21()
{
    // 48kHz / 32-bit / 2ch
    CapturePCMWaveFormatEx(48000,
                           32,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_22()
{
    // 44.1kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(44100,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_23()
{
    // 96kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(96000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_24()
{
    // 16kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(16000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_25()
{
    // 11.025kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(11025,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_26()
{
    // 22.05kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(22050,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_27()
{
    // 64kHz / 16-bit / 8ch
    CapturePCMWaveFormatExtensible(64000,
                                   16,       // container
                                   16,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_28()
{
    // 16kHz / 24-bit / 8ch
    CapturePCMWaveFormatExtensible(16000,
                                   24,       // container
                                   24,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_29()
{
    // 64kHz / 24-bit / 6ch
    CapturePCMWaveFormatExtensible(64000,
                                   24,       // container
                                   24,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_30()
{
    // 11.025kHz / 32-bit / 8ch
    CapturePCMWaveFormatExtensible(11025,
                                   32,       // container
                                   32,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_31()
{
    // 32kHz / 16-bit / 6ch
    CapturePCMWaveFormatExtensible(32000,
                                   16,       // container
                                   16,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_32()
{
    // 3kHz / 8-bit / 8ch
    CapturePCMWaveFormatExtensible(3000,
                                   8,        // container
                                   8,        // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_33()
{
    // 88.2kHz/ 32-bit / 8ch
    CapturePCMWaveFormatExtensible(88200,
                                   32,       // container
                                   32,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
 
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_34()
{
    // 96kHz/ 32-bit / 8ch
    CapturePCMWaveFormatExtensible(96000,
                                   32,       // container
                                   32,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_35()
{
    // 16kHz/ 8-bit / 6ch
    CapturePCMWaveFormatExtensible(16000,
                                   8,        // container
                                   8,        // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_36()
{
    // 3kHz/ 32-bit / 6ch
    CapturePCMWaveFormatExtensible(3000,
                                   32,       // container
                                   32,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_37()
{
    // 8kHz/ 8-bit / 6ch
    CapturePCMWaveFormatExtensible(8000,
                                   8,        // container
                                   8,        // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_38()
{
    // 48kHz/ 8-bit / 8ch
    CapturePCMWaveFormatExtensible(48000,
                                   8,        // container
                                   8,        // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_39()
{
    // 96kHz/ 16-bit / 6ch
    CapturePCMWaveFormatExtensible(96000,
                                   16,       // container
                                   16,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_40()
{
    // 11.025kHz/ 16-bit / 6ch
    CapturePCMWaveFormatExtensible(11025,
                                   16,       // container
                                   16,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_41()
{
    // 192kHz/ 24-bit / 8ch
    CapturePCMWaveFormatExtensible(192000,
                                   24,       // container
                                   24,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_42()
{
    // 8kHz/ 32-bit / 8ch
    CapturePCMWaveFormatExtensible(8000,
                                   32,       // container
                                   32,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_43()
{
    // 88.2kHz/ 24-bit / 8ch
    CapturePCMWaveFormatExtensible(88200,
                                   24,       // container
                                   24,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_44()
{
    // 22.050kHz/ 24-bit / 8ch
    CapturePCMWaveFormatExtensible(22050,
                                   24,       // container
                                   24,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_45()
{
    // 48kHz/ 32-bit / 6ch
    CapturePCMWaveFormatExtensible(48000,
                                   32,       // container
                                   32,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_46()
{
    // 192kHz/ 8-bit / 6ch
    CapturePCMWaveFormatExtensible(192000,
                                   8,        // container
                                   8,        // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_47()
{
    // 32kHz/ 32-bit / 8ch
    CapturePCMWaveFormatExtensible(32000,
                                   32,       // container
                                   32,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_48()
{
    // 44.1kHz/ 24-bit / 6ch
    CapturePCMWaveFormatExtensible(44100,
                                   24,       // container
                                   24,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_49()
{
    // 22.05kHz/ 8-bit / 6ch
    CapturePCMWaveFormatExtensible(22050,
                                   8,        // container
                                   8,        // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_50()
{
    // 44.1kHz/ 16-bit / 8ch
    CapturePCMWaveFormatExtensible(44100,
                                   16,       // container
                                   16,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_51()
{
    // 16kHz/ 32-bit / 32ch
    CapturePCMWaveFormatExtensible(16000,
                                   32,       // container
                                   32,       // valid bits 
                                   32,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_52()
{
    // 88.2kHz/ 8-bit / 8ch
    CapturePCMWaveFormatExtensible(88200,
                                   8,        // container
                                   8,        // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_53()
{
    // 16kHz/ 16-bit / 16ch
    CapturePCMWaveFormatExtensible(16000,
                                   16,       // container
                                   16,       // valid bits 
                                   16,       // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_54()
{
    // 48kHz/ 32-bit / 2ch
    CapturePCMWaveFormatExtensible(48000,
                                   20,       // container
                                   32,       // valid bits 
                                   2,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_55()
{
    // 48kHz/ 32-bit / 2ch
    CapturePCMWaveFormatExtensible(48000,
                                   24,       // container
                                   32,       // valid bits 
                                   2,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_56()
{
    // 48kHz/ 32-bit / 2ch
    CapturePCMWaveFormatExtensible(48000,
                                   32,       // container
                                   32,       // valid bits 
                                   2,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_57()
{
    // 48kHz/ 16-bit / 4ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   4,       // channels 
                                   KSAUDIO_SPEAKER_QUAD, // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_58()
{
    // 48kHz/ 16-bit / 4ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   4,       // channels 
                                   KSAUDIO_SPEAKER_SURROUND, // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
 
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_59()
{
    // 48kHz/ 16-bit / 6ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   6,       // channels 
                                   KSAUDIO_SPEAKER_5POINT1, // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_60()
{
    // 48kHz/ 16-bit / 8ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   8,       // channels 
                                   KSAUDIO_SPEAKER_7POINT1, // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_61()
{
    // 48kHz/ 16-bit / 1ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   1,       // channels 
                                   KSAUDIO_SPEAKER_MONO,        // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Compat_SRC_Enhanced_Pull_62()
{
    // 48kHz/ 16-bit / 2ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   2,       // channels 
                                   KSAUDIO_SPEAKER_STEREO, // channel mask
                                   AUDCLNT_SHAREMODE_SHARED,
                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}



//
// Exclusive Mode
//

// Timer-Push Render
DWORD TC_Render_Exclusive_Push_0()
{
    // 192k / 32-bit / 1ch
    RenderPCMWaveFormatExTone(192000,
                              32,
                              1,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              TIMER_PUSH_RENDER);

    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_1()
{
    // 11kHz / 8-bit / 1ch
    RenderPCMWaveFormatExTone(11025,
                              8,
                              1,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_2()
{
    // 22kHz / 32-bit / 2ch
    RenderPCMWaveFormatExTone(22050,
                              32,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_3()
{
    // 3kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(3000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_4()
{
    // 88.2kHz / 16-bit / 1ch
    RenderPCMWaveFormatExTone(88200,
                              16,
                              1,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_5()
{
    // 96kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(96000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_6()
{
    // 64kHz / 8-bit / 1ch
    RenderPCMWaveFormatExTone(64000,
                              8,
                              1,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_7()
{
    // 96kHz / 8-bit / 2ch
    RenderPCMWaveFormatExTone(96000,
                              8,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_8()
{
    // 48kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(48000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_9()
{
    // 48kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(48000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_10()
{
    // 64kHz / 32-bit / 2ch
    RenderPCMWaveFormatExTone(64000,
                              32,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_11()
{
    // 11kHz / 24-bit / 2ch
    RenderPCMWaveFormatExTone(11025,
                              24,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_12()
{
    // 192kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(192000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_13()
{
    // 8kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(8000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_14()
{
    // 44.1kHz / 32-bit / 2ch
    RenderPCMWaveFormatExTone(44100,
                              32,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_15()
{
    // 32kHz / 8-bit / 2ch
    RenderPCMWaveFormatExTone(32000,
                              8,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_16()
{
    // 32kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(32000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_17()
{
    // 44.1kHz / 8-bit / 1ch
    RenderPCMWaveFormatExTone(44100,
                              8,
                              1,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_18()
{
    // 3kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(3000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_19()
{
    // 22.05kHz / 16-bit / 1ch
    RenderPCMWaveFormatExTone(22050,
                              16,
                              1,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_20()
{
    // 48kHz / 24-bit / 2ch
    RenderPCMWaveFormatExTone(48000,
                              24,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_21()
{
    // 48kHz / 32-bit / 2ch
    RenderPCMWaveFormatExTone(48000,
                              32,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_22()
{
    // 44.1kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(44100,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_23()
{
    // 96kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(96000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_24()
{
    // 16kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(16000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_25()
{
    // 11.025kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(11025,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_26()
{
    // 22.05kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(22050,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_27()
{
    // 64kHz / 16-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(64000,
                                      16,       // container
                                      16,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_28()
{
    // 16kHz / 24-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(16000,
                                      24,       // container
                                      24,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_29()
{
    // 64kHz / 24-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(64000,
                                      24,       // container
                                      24,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_30()
{
    // 11.025kHz / 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(11025,
                                      32,       // container
                                      32,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_31()
{
    // 32kHz / 16-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(32000,
                                      16,       // container
                                      16,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_32()
{
    // 3kHz / 8-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(3000,
                                      8,        // container
                                      8,        // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_33()
{
    // 88.2kHz/ 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(88200,
                                      32,       // container
                                      32,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_34()
{
    // 96kHz/ 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(96000,
                                      32,       // container
                                      32,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_35()
{
    // 16kHz/ 8-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(16000,
                                      8,        // container
                                      8,        // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_36()
{
    // 3kHz/ 32-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(3000,
                                      32,       // container
                                      32,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_37()
{
    // 8kHz/ 8-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(8000,
                                      8,        // container
                                      8,        // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_38()
{
    // 48kHz/ 8-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      8,        // container
                                      8,        // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_39()
{
    // 96kHz/ 16-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(96000,
                                      16,       // container
                                      16,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_40()
{
    // 11.025kHz/ 16-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(11025,
                                      16,       // container
                                      16,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_41()
{
    // 192kHz/ 24-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(192000,
                                      24,       // container
                                      24,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_42()
{
    // 8kHz/ 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(8000,
                                      32,       // container
                                      32,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_43()
{
    // 88.2kHz/ 24-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(88200,
                                      24,       // container
                                      24,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_44()
{
    // 22.050kHz/ 24-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(22050,
                                      24,       // container
                                      24,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_45()
{
    // 48kHz/ 32-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      32,       // container
                                      32,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_46()
{
    // 192kHz/ 8-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(192000,
                                      8,        // container
                                      8,        // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_47()
{
    // 32kHz/ 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(32000,
                                      32,       // container
                                      32,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_48()
{
    // 44.1kHz/ 24-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(44100,
                                      24,       // container
                                      24,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_49()
{
    // 22.05kHz/ 8-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(22050,
                                      8,        // container
                                      8,        // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_50()
{
    // 44.1kHz/ 16-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(44100,
                                      16,       // container
                                      16,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_51()
{
    // 16kHz/ 32-bit / 32ch
    RenderPCMWaveFormatExtensibleTone(16000,
                                      32,       // container
                                      32,       // valid bits 
                                      32,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_52()
{
    // 88.2kHz/ 8-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(88200,
                                      8,        // container
                                      8,        // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_53()
{
    // 16kHz/ 16-bit / 16ch
    RenderPCMWaveFormatExtensibleTone(16000,
                                      16,       // container
                                      16,       // valid bits 
                                      16,       // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_54()
{
    // 48kHz/ 32-bit / 2ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      20,       // container
                                      32,       // valid bits 
                                      2,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_55()
{
    // 48kHz/ 32-bit / 2ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      24,       // container
                                      32,       // valid bits 
                                      2,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_56()
{
    // 48kHz/ 32-bit / 2ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      32,       // container
                                      32,       // valid bits 
                                      2,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_57()
{
    // 48kHz/ 16-bit / 4ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      4,       // channels 
                                      KSAUDIO_SPEAKER_QUAD, // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_58()
{
    // 48kHz/ 16-bit / 4ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      4,       // channels 
                                      KSAUDIO_SPEAKER_SURROUND, // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_59()
{
    // 48kHz/ 16-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      6,       // channels 
                                      KSAUDIO_SPEAKER_5POINT1, // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_60()
{
    // 48kHz/ 16-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      8,       // channels 
                                      KSAUDIO_SPEAKER_7POINT1, // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_61()
{
    // 48kHz/ 16-bit / 1ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      1,       // channels 
                                      KSAUDIO_SPEAKER_MONO,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Push_62()
{
    // 48kHz/ 16-bit / 2ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      2,       // channels 
                                      KSAUDIO_SPEAKER_STEREO, // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      TIMER_PUSH_RENDER);
    
    return FNS_PASS;
}

// Event-Pull Render
DWORD TC_Render_Exclusive_Pull_0()
{
    // 192k / 32-bit / 1ch
    RenderPCMWaveFormatExTone(192000,
                              32,
                              1,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              EVENT_PULL_RENDER);

    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_1()
{
    // 11kHz / 8-bit / 1ch
    RenderPCMWaveFormatExTone(11025,
                              8,
                              1,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_2()
{
    // 22kHz / 32-bit / 2ch
    RenderPCMWaveFormatExTone(22050,
                              32,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_3()
{
    // 3kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(3000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_4()
{
    // 88.2kHz / 16-bit / 1ch
    RenderPCMWaveFormatExTone(88200,
                              16,
                              1,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_5()
{
    // 96kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(96000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_6()
{
    // 64kHz / 8-bit / 1ch
    RenderPCMWaveFormatExTone(64000,
                              8,
                              1,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_7()
{
    // 96kHz / 8-bit / 2ch
    RenderPCMWaveFormatExTone(96000,
                              8,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_8()
{
    // 48kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(48000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_9()
{

    // Skipping this test on Sink Description until GetPosition bug is fixed
    if (StrStrI(GetModule()->GetDevice()->m_szRep, _T("SinkDescription Sample on Virtual Audio Device (WDM)")) != NULL)
    {
        XLOG(XSKIP, 1, "SKIP: Virtual audio device does not implement accurate loopback. Hence skipping this test.");
        return FNS_SKIPPED;
    }
    // 48kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(48000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_10()
{
    // 64kHz / 32-bit / 2ch
    RenderPCMWaveFormatExTone(64000,
                              32,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_11()
{
    // 11kHz / 24-bit / 2ch
    RenderPCMWaveFormatExTone(11025,
                              24,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_12()
{
    // 192kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(192000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_13()
{
    // 8kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(8000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_14()
{
    // 44.1kHz / 32-bit / 2ch
    RenderPCMWaveFormatExTone(44100,
                              32,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_15()
{
    // 32kHz / 8-bit / 2ch
    RenderPCMWaveFormatExTone(32000,
                              8,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_16()
{
    // 32kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(32000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_17()
{
    // 44.1kHz / 8-bit / 1ch
    RenderPCMWaveFormatExTone(44100,
                              8,
                              1,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_18()
{
    // 3kHz / 24-bit / 1ch
    RenderPCMWaveFormatExTone(3000,
                              24,
                              1,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_19()
{
    // 22.05kHz / 16-bit / 1ch
    RenderPCMWaveFormatExTone(22050,
                              16,
                              1,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_20()
{
    // 48kHz / 24-bit / 2ch
    RenderPCMWaveFormatExTone(48000,
                              24,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_21()
{
    // 48kHz / 32-bit / 2ch
    RenderPCMWaveFormatExTone(48000,
                              32,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_22()
{
    // Skipping this test on Sink Description until GetPosition bug is fixed
    if (StrStrI(GetModule()->GetDevice()->m_szRep, _T("SinkDescription Sample on Virtual Audio Device (WDM)")) != NULL)
    {
        XLOG(XSKIP, 1, "SKIP: Virtual audio device does not implement accurate loopback. Hence skipping this test.");
        return FNS_SKIPPED;
    }
    // 44.1kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(44100,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_23()
{
    // Skipping this test on Sink Description until GetPosition bug is fixed
    if (StrStrI(GetModule()->GetDevice()->m_szRep, _T("SinkDescription Sample on Virtual Audio Device (WDM)")) != NULL)
    {
        XLOG(XSKIP, 1, "SKIP: Virtual audio device does not implement accurate loopback. Hence skipping this test.");
        return FNS_SKIPPED;
    }
    // 96kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(96000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_24()
{
    // 16kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(16000,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_25()
{
    // 11.025kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(11025,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_26()
{
    // 22.05kHz / 16-bit / 2ch
    RenderPCMWaveFormatExTone(22050,
                              16,
                              2,
                              AUDCLNT_SHAREMODE_EXCLUSIVE,
                              0,
                              EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_27()
{
    // 64kHz / 16-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(64000,
                                      16,       // container
                                      16,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_28()
{
    // 16kHz / 24-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(16000,
                                      24,       // container
                                      24,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_29()
{
    // 64kHz / 24-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(64000,
                                      24,       // container
                                      24,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_30()
{
    // 11.025kHz / 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(11025,
                                      32,       // container
                                      32,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_31()
{
    // 32kHz / 16-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(32000,
                                      16,       // container
                                      16,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_32()
{
    // 3kHz / 8-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(3000,
                                      8,        // container
                                      8,        // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_33()
{
    // 88.2kHz/ 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(88200,
                                      32,       // container
                                      32,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_34()
{
    // 96kHz/ 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(96000,
                                      32,       // container
                                      32,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_35()
{
    // 16kHz/ 8-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(16000,
                                      8,        // container
                                      8,        // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_36()
{
    // 3kHz/ 32-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(3000,
                                      32,       // container
                                      32,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_37()
{
    // 8kHz/ 8-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(8000,
                                      8,        // container
                                      8,        // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_38()
{
    // 48kHz/ 8-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      8,        // container
                                      8,        // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_39()
{
    // 96kHz/ 16-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(96000,
                                      16,       // container
                                      16,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_40()
{
    // 11.025kHz/ 16-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(11025,
                                      16,       // container
                                      16,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_41()
{
    // 192kHz/ 24-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(192000,
                                      24,       // container
                                      24,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_42()
{
    // 8kHz/ 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(8000,
                                      32,       // container
                                      32,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_43()
{
    // 88.2kHz/ 24-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(88200,
                                      24,       // container
                                      24,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_44()
{
    // 22.050kHz/ 24-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(22050,
                                      24,       // container
                                      24,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_45()
{
    // 48kHz/ 32-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      32,       // container
                                      32,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_46()
{
    // 192kHz/ 8-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(192000,
                                      8,        // container
                                      8,        // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_47()
{
    // 32kHz/ 32-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(32000,
                                      32,       // container
                                      32,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_48()
{
    // 44.1kHz/ 24-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(44100,
                                      24,       // container
                                      24,       // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_49()
{
    // 22.05kHz/ 8-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(22050,
                                      8,        // container
                                      8,        // valid bits 
                                      6,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_50()
{
    // 44.1kHz/ 16-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(44100,
                                      16,       // container
                                      16,       // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_51()
{
    // 16kHz/ 32-bit / 32ch
    RenderPCMWaveFormatExtensibleTone(16000,
                                      32,       // container
                                      32,       // valid bits 
                                      32,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_52()
{
    // 88.2kHz/ 8-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(88200,
                                      8,        // container
                                      8,        // valid bits 
                                      8,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_53()
{
    // 16kHz/ 16-bit / 16ch
    RenderPCMWaveFormatExtensibleTone(16000,
                                      16,       // container
                                      16,       // valid bits 
                                      16,       // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_54()
{
    // 48kHz/ 32-bit / 2ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      20,       // container
                                      32,       // valid bits 
                                      2,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_55()
{
    // 48kHz/ 32-bit / 2ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      24,       // container
                                      32,       // valid bits 
                                      2,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_56()
{
    // 48kHz/ 32-bit / 2ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      32,       // container
                                      32,       // valid bits 
                                      2,        // channels 
                                      0,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_57()
{
    // 48kHz/ 16-bit / 4ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      4,       // channels 
                                      KSAUDIO_SPEAKER_QUAD, // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_58()
{
    // 48kHz/ 16-bit / 4ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      4,       // channels 
                                      KSAUDIO_SPEAKER_SURROUND, // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_59()
{
    // 48kHz/ 16-bit / 6ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      6,       // channels 
                                      KSAUDIO_SPEAKER_5POINT1, // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_60()
{
    // 48kHz/ 16-bit / 8ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      8,       // channels 
                                      KSAUDIO_SPEAKER_7POINT1, // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_61()
{
    // 48kHz/ 16-bit / 1ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      1,       // channels 
                                      KSAUDIO_SPEAKER_MONO,        // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

DWORD TC_Render_Exclusive_Pull_62()
{
    // Skipping this test on Sink Description until GetPosition bug is fixed
    if (StrStrI(GetModule()->GetDevice()->m_szRep, _T("SinkDescription Sample on Virtual Audio Device (WDM)")) != NULL)
    {
        XLOG(XSKIP, 1, "SKIP: Virtual audio device does not implement accurate loopback. Hence skipping this test.");
        return FNS_SKIPPED;
    }
    // 48kHz/ 16-bit / 2ch
    RenderPCMWaveFormatExtensibleTone(48000,
                                      16,       // container
                                      16,       // valid bits 
                                      2,       // channels 
                                      KSAUDIO_SPEAKER_STEREO, // channel mask
                                      AUDCLNT_SHAREMODE_EXCLUSIVE,
                                      0,
                                      EVENT_PULL_RENDER);
    
    return FNS_PASS;
}

//
// Exclusive-Mode Capture (Push)
//
DWORD TC_Capture_Exclusive_Push_0()
{
    // 192k / 32-bit / 1ch
    CapturePCMWaveFormatEx(192000,
                           32,
                           1,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           TIMER_PUSH_CAPTURE);

    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_1()
{
    // 11kHz / 8-bit / 1ch
    CapturePCMWaveFormatEx(11025,
                           8,
                           1,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_2()
{
    // 22kHz / 32-bit / 2ch
    CapturePCMWaveFormatEx(22050,
                           32,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_3()
{
    // 3kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(3000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_4()
{
    // 88.2kHz / 16-bit / 1ch
    CapturePCMWaveFormatEx(88200,
                           16,
                           1,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_5()
{
    // 96kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(96000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_6()
{
    // 64kHz / 8-bit / 1ch
    CapturePCMWaveFormatEx(64000,
                           8,
                           1,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_7()
{
    // 96kHz / 8-bit / 2ch
    CapturePCMWaveFormatEx(96000,
                           8,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_8()
{
    // 48kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(48000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_9()
{
    // 48kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(48000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_10()
{
    // 64kHz / 32-bit / 2ch
    CapturePCMWaveFormatEx(64000,
                           32,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_11()
{
    // 11kHz / 24-bit / 2ch
    CapturePCMWaveFormatEx(11025,
                           24,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_12()
{
    // 192kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(192000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_13()
{
    // 8kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(8000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_14()
{
    // 44.1kHz / 32-bit / 2ch
    CapturePCMWaveFormatEx(44100,
                           32,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_15()
{
    // 32kHz / 8-bit / 2ch
    CapturePCMWaveFormatEx(32000,
                           8,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_16()
{
    // 32kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(32000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_17()
{
    // 44.1kHz / 8-bit / 1ch
    CapturePCMWaveFormatEx(44100,
                           8,
                           1,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_18()
{
    // 3kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(3000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_19()
{
    // 22.05kHz / 16-bit / 1ch
    CapturePCMWaveFormatEx(22050,
                           16,
                           1,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_20()
{
    // 48kHz / 24-bit / 2ch
    CapturePCMWaveFormatEx(48000,
                           24,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_21()
{
    // 48kHz / 32-bit / 2ch
    CapturePCMWaveFormatEx(48000,
                           32,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_22()
{
    // 44.1kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(44100,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_23()
{
    // 96kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(96000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_24()
{
    // 16kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(16000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_25()
{
    // 11.025kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(11025,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_26()
{
    // 22.05kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(22050,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_27()
{
    // 64kHz / 16-bit / 8ch
    CapturePCMWaveFormatExtensible(64000,
                                   16,       // container
                                   16,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_28()
{
    // 16kHz / 24-bit / 8ch
    CapturePCMWaveFormatExtensible(16000,
                                   24,       // container
                                   24,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_29()
{
    // 64kHz / 24-bit / 6ch
    CapturePCMWaveFormatExtensible(64000,
                                   24,       // container
                                   24,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_30()
{
    // 11.025kHz / 32-bit / 8ch
    CapturePCMWaveFormatExtensible(11025,
                                   32,       // container
                                   32,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_31()
{
    // 32kHz / 16-bit / 6ch
    CapturePCMWaveFormatExtensible(32000,
                                   16,       // container
                                   16,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_32()
{
    // 3kHz / 8-bit / 8ch
    CapturePCMWaveFormatExtensible(3000,
                                   8,        // container
                                   8,        // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_33()
{
    // 88.2kHz/ 32-bit / 8ch
    CapturePCMWaveFormatExtensible(88200,
                                   32,       // container
                                   32,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
 
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_34()
{
    // 96kHz/ 32-bit / 8ch
    CapturePCMWaveFormatExtensible(96000,
                                   32,       // container
                                   32,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_35()
{
    // 16kHz/ 8-bit / 6ch
    CapturePCMWaveFormatExtensible(16000,
                                   8,        // container
                                   8,        // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_36()
{
    // 3kHz/ 32-bit / 6ch
    CapturePCMWaveFormatExtensible(3000,
                                   32,       // container
                                   32,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_37()
{
    // 8kHz/ 8-bit / 6ch
    CapturePCMWaveFormatExtensible(8000,
                                   8,        // container
                                   8,        // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_38()
{
    // 48kHz/ 8-bit / 8ch
    CapturePCMWaveFormatExtensible(48000,
                                   8,        // container
                                   8,        // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_39()
{
    // 96kHz/ 16-bit / 6ch
    CapturePCMWaveFormatExtensible(96000,
                                   16,       // container
                                   16,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_40()
{
    // 11.025kHz/ 16-bit / 6ch
    CapturePCMWaveFormatExtensible(11025,
                                   16,       // container
                                   16,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_41()
{
    // 192kHz/ 24-bit / 8ch
    CapturePCMWaveFormatExtensible(192000,
                                   24,       // container
                                   24,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_42()
{
    // 8kHz/ 32-bit / 8ch
    CapturePCMWaveFormatExtensible(8000,
                                   32,       // container
                                   32,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_43()
{
    // 88.2kHz/ 24-bit / 8ch
    CapturePCMWaveFormatExtensible(88200,
                                   24,       // container
                                   24,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_44()
{
    // 22.050kHz/ 24-bit / 8ch
    CapturePCMWaveFormatExtensible(22050,
                                   24,       // container
                                   24,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_45()
{
    // 48kHz/ 32-bit / 6ch
    CapturePCMWaveFormatExtensible(48000,
                                   32,       // container
                                   32,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_46()
{
    // 192kHz/ 8-bit / 6ch
    CapturePCMWaveFormatExtensible(192000,
                                   8,        // container
                                   8,        // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_47()
{
    // 32kHz/ 32-bit / 8ch
    CapturePCMWaveFormatExtensible(32000,
                                   32,       // container
                                   32,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_48()
{
    // 44.1kHz/ 24-bit / 6ch
    CapturePCMWaveFormatExtensible(44100,
                                   24,       // container
                                   24,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_49()
{
    // 22.05kHz/ 8-bit / 6ch
    CapturePCMWaveFormatExtensible(22050,
                                   8,        // container
                                   8,        // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_50()
{
    // 44.1kHz/ 16-bit / 8ch
    CapturePCMWaveFormatExtensible(44100,
                                   16,       // container
                                   16,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_51()
{
    // 16kHz/ 32-bit / 32ch
    CapturePCMWaveFormatExtensible(16000,
                                   32,       // container
                                   32,       // valid bits 
                                   32,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_52()
{
    // 88.2kHz/ 8-bit / 8ch
    CapturePCMWaveFormatExtensible(88200,
                                   8,        // container
                                   8,        // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_53()
{
    // 16kHz/ 16-bit / 16ch
    CapturePCMWaveFormatExtensible(16000,
                                   16,       // container
                                   16,       // valid bits 
                                   16,       // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_54()
{
    // 48kHz/ 32-bit / 2ch
    CapturePCMWaveFormatExtensible(48000,
                                   20,       // container
                                   32,       // valid bits 
                                   2,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_55()
{
    // 48kHz/ 32-bit / 2ch
    CapturePCMWaveFormatExtensible(48000,
                                   24,       // container
                                   32,       // valid bits 
                                   2,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_56()
{
    // 48kHz/ 32-bit / 2ch
    CapturePCMWaveFormatExtensible(48000,
                                   32,       // container
                                   32,       // valid bits 
                                   2,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_57()
{
    // 48kHz/ 16-bit / 4ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   4,       // channels 
                                   KSAUDIO_SPEAKER_QUAD, // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_58()
{
    // 48kHz/ 16-bit / 4ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   4,       // channels 
                                   KSAUDIO_SPEAKER_SURROUND, // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
 
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_59()
{
    // 48kHz/ 16-bit / 6ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   6,       // channels 
                                   KSAUDIO_SPEAKER_5POINT1, // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_60()
{
    // 48kHz/ 16-bit / 8ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   8,       // channels 
                                   KSAUDIO_SPEAKER_7POINT1, // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_61()
{
    // 48kHz/ 16-bit / 1ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   1,       // channels 
                                   KSAUDIO_SPEAKER_MONO,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Push_62()
{
    // 48kHz/ 16-bit / 2ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   2,       // channels 
                                   KSAUDIO_SPEAKER_STEREO, // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   TIMER_PUSH_CAPTURE);
    
    return FNS_PASS;
}

//
// Exclusive-Mode Capture (Pull)
//
DWORD TC_Capture_Exclusive_Pull_0()
{
    // 192k / 32-bit / 1ch
    CapturePCMWaveFormatEx(192000,
                           32,
                           1,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           EVENT_PULL_CAPTURE);

    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_1()
{
    // 11kHz / 8-bit / 1ch
    CapturePCMWaveFormatEx(11025,
                           8,
                           1,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_2()
{
    // 22kHz / 32-bit / 2ch
    CapturePCMWaveFormatEx(22050,
                           32,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_3()
{
    // 3kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(3000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_4()
{
    // 88.2kHz / 16-bit / 1ch
    CapturePCMWaveFormatEx(88200,
                           16,
                           1,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_5()
{
    // 96kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(96000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_6()
{
    // 64kHz / 8-bit / 1ch
    CapturePCMWaveFormatEx(64000,
                           8,
                           1,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_7()
{
    // 96kHz / 8-bit / 2ch
    CapturePCMWaveFormatEx(96000,
                           8,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_8()
{
    // 48kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(48000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_9()
{
    // 48kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(48000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_10()
{
    // 64kHz / 32-bit / 2ch
    CapturePCMWaveFormatEx(64000,
                           32,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_11()
{
    // 11kHz / 24-bit / 2ch
    CapturePCMWaveFormatEx(11025,
                           24,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_12()
{
    // 192kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(192000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_13()
{
    // 8kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(8000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_14()
{
    // 44.1kHz / 32-bit / 2ch
    CapturePCMWaveFormatEx(44100,
                           32,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_15()
{
    // 32kHz / 8-bit / 2ch
    CapturePCMWaveFormatEx(32000,
                           8,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_16()
{
    // 32kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(32000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_17()
{
    // 44.1kHz / 8-bit / 1ch
    CapturePCMWaveFormatEx(44100,
                           8,
                           1,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_18()
{
    // 3kHz / 24-bit / 1ch
    CapturePCMWaveFormatEx(3000,
                           24,
                           1,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_19()
{
    // 22.05kHz / 16-bit / 1ch
    CapturePCMWaveFormatEx(22050,
                           16,
                           1,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_20()
{
    // 48kHz / 24-bit / 2ch
    CapturePCMWaveFormatEx(48000,
                           24,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_21()
{
    // 48kHz / 32-bit / 2ch
    CapturePCMWaveFormatEx(48000,
                           32,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_22()
{
    // 44.1kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(44100,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_23()
{
    // 96kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(96000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_24()
{
    // 16kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(16000,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_25()
{
    // 11.025kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(11025,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_26()
{
    // 22.05kHz / 16-bit / 2ch
    CapturePCMWaveFormatEx(22050,
                           16,
                           2,
                           AUDCLNT_SHAREMODE_EXCLUSIVE,
                           0,
                           EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_27()
{
    // 64kHz / 16-bit / 8ch
    CapturePCMWaveFormatExtensible(64000,
                                   16,       // container
                                   16,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_28()
{
    // 16kHz / 24-bit / 8ch
    CapturePCMWaveFormatExtensible(16000,
                                   24,       // container
                                   24,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_29()
{
    // 64kHz / 24-bit / 6ch
    CapturePCMWaveFormatExtensible(64000,
                                   24,       // container
                                   24,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_30()
{
    // 11.025kHz / 32-bit / 8ch
    CapturePCMWaveFormatExtensible(11025,
                                   32,       // container
                                   32,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_31()
{
    // 32kHz / 16-bit / 6ch
    CapturePCMWaveFormatExtensible(32000,
                                   16,       // container
                                   16,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_32()
{
    // 3kHz / 8-bit / 8ch
    CapturePCMWaveFormatExtensible(3000,
                                   8,        // container
                                   8,        // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_33()
{
    // 88.2kHz/ 32-bit / 8ch
    CapturePCMWaveFormatExtensible(88200,
                                   32,       // container
                                   32,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
 
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_34()
{
    // 96kHz/ 32-bit / 8ch
    CapturePCMWaveFormatExtensible(96000,
                                   32,       // container
                                   32,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_35()
{
    // 16kHz/ 8-bit / 6ch
    CapturePCMWaveFormatExtensible(16000,
                                   8,        // container
                                   8,        // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_36()
{
    // 3kHz/ 32-bit / 6ch
    CapturePCMWaveFormatExtensible(3000,
                                   32,       // container
                                   32,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_37()
{
    // 8kHz/ 8-bit / 6ch
    CapturePCMWaveFormatExtensible(8000,
                                   8,        // container
                                   8,        // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_38()
{
    // 48kHz/ 8-bit / 8ch
    CapturePCMWaveFormatExtensible(48000,
                                   8,        // container
                                   8,        // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_39()
{
    // 96kHz/ 16-bit / 6ch
    CapturePCMWaveFormatExtensible(96000,
                                   16,       // container
                                   16,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_40()
{
    // 11.025kHz/ 16-bit / 6ch
    CapturePCMWaveFormatExtensible(11025,
                                   16,       // container
                                   16,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_41()
{
    // 192kHz/ 24-bit / 8ch
    CapturePCMWaveFormatExtensible(192000,
                                   24,       // container
                                   24,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_42()
{
    // 8kHz/ 32-bit / 8ch
    CapturePCMWaveFormatExtensible(8000,
                                   32,       // container
                                   32,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_43()
{
    // 88.2kHz/ 24-bit / 8ch
    CapturePCMWaveFormatExtensible(88200,
                                   24,       // container
                                   24,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_44()
{
    // 22.050kHz/ 24-bit / 8ch
    CapturePCMWaveFormatExtensible(22050,
                                   24,       // container
                                   24,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_45()
{
    // 48kHz/ 32-bit / 6ch
    CapturePCMWaveFormatExtensible(48000,
                                   32,       // container
                                   32,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_46()
{
    // 192kHz/ 8-bit / 6ch
    CapturePCMWaveFormatExtensible(192000,
                                   8,        // container
                                   8,        // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_47()
{
    // 32kHz/ 32-bit / 8ch
    CapturePCMWaveFormatExtensible(32000,
                                   32,       // container
                                   32,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_48()
{
    // 44.1kHz/ 24-bit / 6ch
    CapturePCMWaveFormatExtensible(44100,
                                   24,       // container
                                   24,       // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_49()
{
    // 22.05kHz/ 8-bit / 6ch
    CapturePCMWaveFormatExtensible(22050,
                                   8,        // container
                                   8,        // valid bits 
                                   6,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_50()
{
    // 44.1kHz/ 16-bit / 8ch
    CapturePCMWaveFormatExtensible(44100,
                                   16,       // container
                                   16,       // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_51()
{
    // 16kHz/ 32-bit / 32ch
    CapturePCMWaveFormatExtensible(16000,
                                   32,       // container
                                   32,       // valid bits 
                                   32,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_52()
{
    // 88.2kHz/ 8-bit / 8ch
    CapturePCMWaveFormatExtensible(88200,
                                   8,        // container
                                   8,        // valid bits 
                                   8,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_53()
{
    // 16kHz/ 16-bit / 16ch
    CapturePCMWaveFormatExtensible(16000,
                                   16,       // container
                                   16,       // valid bits 
                                   16,       // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_54()
{
    // 48kHz/ 32-bit / 2ch
    CapturePCMWaveFormatExtensible(48000,
                                   20,       // container
                                   32,       // valid bits 
                                   2,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_55()
{
    // 48kHz/ 32-bit / 2ch
    CapturePCMWaveFormatExtensible(48000,
                                   24,       // container
                                   32,       // valid bits 
                                   2,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_56()
{
    // 48kHz/ 32-bit / 2ch
    CapturePCMWaveFormatExtensible(48000,
                                   32,       // container
                                   32,       // valid bits 
                                   2,        // channels 
                                   0,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_57()
{
    // 48kHz/ 16-bit / 4ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   4,       // channels 
                                   KSAUDIO_SPEAKER_QUAD, // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_58()
{
    // 48kHz/ 16-bit / 4ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   4,       // channels 
                                   KSAUDIO_SPEAKER_SURROUND, // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
 
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_59()
{
    // 48kHz/ 16-bit / 6ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   6,       // channels 
                                   KSAUDIO_SPEAKER_5POINT1, // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_60()
{
    // 48kHz/ 16-bit / 8ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   8,       // channels 
                                   KSAUDIO_SPEAKER_7POINT1, // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_61()
{
    // 48kHz/ 16-bit / 1ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   1,       // channels 
                                   KSAUDIO_SPEAKER_MONO,        // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

DWORD TC_Capture_Exclusive_Pull_62()
{
    // 48kHz/ 16-bit / 2ch
    CapturePCMWaveFormatExtensible(48000,
                                   16,       // container
                                   16,       // valid bits 
                                   2,       // channels 
                                   KSAUDIO_SPEAKER_STEREO, // channel mask
                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                   0,
                                   EVENT_PULL_CAPTURE);
    
    return FNS_PASS;
}

////
// Render A PCM format Sine Tone Specified using a WAVEFORMATEX structure
//
// Params:
//  nSamplesPerSec - Samples/Sec for the tone to be rendered
//  wBitsPerSample - Bit-Depth for the tone to be rendered
//  nChannels      - Channel count for the tone to be rendered
//  nSharemode     - AudioClient share mode to be used
//  eAlgorithm     - The streaming algorithm to be used
//
// Return Value:
//  The appropriate HRESULT on success, E_FAIL on failure.
//  Note: Test case failure is signalled via XLOG
////
HRESULT RenderPCMWaveFormatExTone(DWORD nSamplesPerSec,
                                  WORD wBitsPerSample,
                                  WORD nChannels,
                                  AUDCLNT_SHAREMODE nShareMode,
                                  DWORD dwStreamFlags,
                                  TEST_STREAMING_ALGORITHM eAlgorithm)
{
    HRESULT hr = S_OK;

    // build the necessary format
    WAVEFORMATEX wfxStreamingFormat = {0};
    wfxStreamingFormat.wFormatTag = WAVE_FORMAT_PCM;
    wfxStreamingFormat.nChannels = nChannels;
    wfxStreamingFormat.nSamplesPerSec = nSamplesPerSec;
    wfxStreamingFormat.wBitsPerSample = wBitsPerSample;
    wfxStreamingFormat.nBlockAlign = (wfxStreamingFormat.wBitsPerSample *
                                      wfxStreamingFormat.nChannels) / BITS_PER_BYTE;
    wfxStreamingFormat.nAvgBytesPerSec = wfxStreamingFormat.nBlockAlign *
                                         wfxStreamingFormat.nSamplesPerSec;
    wfxStreamingFormat.cbSize = 0;
           
    // put up a message for the user
    SLOG(eError, "Rendering test tone...");
    SLOG(eError, 
         "%.3f kHz / %d-bit / %d ch",
         ((FLOAT)(nSamplesPerSec))/1000.0f,
         wBitsPerSample,
         nChannels);

    METHODCHECKRESULT("TestRenderTone",
                      hr,
                      TestRenderTone(nShareMode, 
                                     dwStreamFlags, 
                                     &wfxStreamingFormat, 
                                     eAlgorithm, 
                                     TEST_TONE_AMPLITUDE, 
                                     TEST_TONE_FREQUENCY, 
                                     TEST_TONE_DURATION));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error rendering test tone");
    }

    return hr;
}

////
// Capture PCM data using a WAVEFORMATEX structure
//
// Params:
//  nSamplesPerSec - Samples/Sec for the tone to be rendered
//  wBitsPerSample - Bit-Depth for the tone to be rendered
//  nChannels      - Channel count for the tone to be rendered
//  nSharemode     - AudioClient share mode to be used
//  eAlgorithm     - The streaming algorithm to be used
//
// Return Value:
//  The appropriate HRESULT on success, E_FAIL on failure.
//  Note: Test case failure is signalled via XLOG
////
HRESULT CapturePCMWaveFormatEx(DWORD nSamplesPerSec,
                               WORD wBitsPerSample,
                               WORD nChannels,
                               AUDCLNT_SHAREMODE nShareMode,
                               DWORD dwStreamFlags,
                               TEST_STREAMING_ALGORITHM eAlgorithm)
{
    HRESULT hr = S_OK;

    // build the necessary format
    WAVEFORMATEX wfxStreamingFormat = {0};
    wfxStreamingFormat.wFormatTag = WAVE_FORMAT_PCM;
    wfxStreamingFormat.nChannels = nChannels;
    wfxStreamingFormat.nSamplesPerSec = nSamplesPerSec;
    wfxStreamingFormat.wBitsPerSample = wBitsPerSample;
    wfxStreamingFormat.nBlockAlign = (wfxStreamingFormat.wBitsPerSample *
                                      wfxStreamingFormat.nChannels) / BITS_PER_BYTE;
    wfxStreamingFormat.nAvgBytesPerSec = wfxStreamingFormat.nBlockAlign *
                                         wfxStreamingFormat.nSamplesPerSec;
    wfxStreamingFormat.cbSize = 0;
           
    // put up a message for the user
    SLOG(eError, 
         "%.3f kHz / %d-bit / %d ch",
         ((FLOAT)(nSamplesPerSec))/1000.0f,
         wBitsPerSample,
         nChannels);
    SLOG(eError, "Capturing audio data...");

    METHODCHECKRESULT("TestCapture",
                      hr,
                      TestCapture(nShareMode, 
                                  dwStreamFlags, 
                                  &wfxStreamingFormat, 
                                  eAlgorithm,                                   
                                  TEST_TONE_DURATION));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error during capture");
    }

    return hr;
}

////
// Render A PCM format Sine Tone Specified using a WAVEFORMATEXTENSIBLE structure
//
// Params:
//  nSamplesPerSec          - Samples/Sec for the tone to be rendered
//  wValidBitsPerSample     - Valid bits per sample
//  wBitsPerSample          - Container size for the tone to be rendered
//  nChannels               - Channel count for the tone to be rendered
//  dwChannelMask           - Channel Mask
//  nSharemode              - AudioClient share mode to be used
//  eAlgorithm              - The streaming algorithm to be used
//
// Return Value:
//  The appropriate HRESULT on success, E_FAIL on failure.
//  Note: Test case failure is signalled via XLOG
////
HRESULT RenderPCMWaveFormatExtensibleTone(DWORD nSamplesPerSec,
                                          WORD wValidBitsPerSample,
                                          WORD wBitsPerSample,
                                          WORD nChannels,
                                          DWORD dwChannelMask,
                                          AUDCLNT_SHAREMODE nShareMode,
                                          DWORD dwStreamFlags,
                                          TEST_STREAMING_ALGORITHM eAlgorithm)
{
    HRESULT hr = S_OK;

    // build the necessary format
    WAVEFORMATEXTENSIBLE wfxStreamingFormat = {0};
    wfxStreamingFormat.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
    wfxStreamingFormat.Format.nChannels = nChannels;
    wfxStreamingFormat.Format.nSamplesPerSec = nSamplesPerSec;
    wfxStreamingFormat.Format.wBitsPerSample = wBitsPerSample;
    wfxStreamingFormat.Format.nBlockAlign = (wfxStreamingFormat.Format.wBitsPerSample *
                                            wfxStreamingFormat.Format.nChannels) / BITS_PER_BYTE;
    wfxStreamingFormat.Format.nAvgBytesPerSec = wfxStreamingFormat.Format.nBlockAlign *
                                                wfxStreamingFormat.Format.nSamplesPerSec;
    wfxStreamingFormat.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE)-sizeof(WAVEFORMATEX);
    wfxStreamingFormat.Samples.wValidBitsPerSample = wValidBitsPerSample;
    wfxStreamingFormat.dwChannelMask = dwChannelMask;
    wfxStreamingFormat.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
           
    // put up a message for the user
    SLOG(eError, 
         "%.3f kHz / %d-bit (container) / %d-bit (valid) / %d ch / 0x%x (mask)",
         ((FLOAT)(nSamplesPerSec))/1000.0f,
         wBitsPerSample,
         wValidBitsPerSample,
         nChannels,
         dwChannelMask);
    SLOG(eError, "Rendering test tone...");

    METHODCHECKRESULT("TestRenderTone",
                      hr,
                      TestRenderTone(nShareMode, 
                                     dwStreamFlags, 
                                     (PWAVEFORMATEX)&wfxStreamingFormat, 
                                     eAlgorithm, 
                                     TEST_TONE_AMPLITUDE, 
                                     TEST_TONE_FREQUENCY, 
                                     TEST_TONE_DURATION));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error rendering test tone");
    }

    return hr;
}

////
// Capture data using a WAVEFORMATEXTENSIBLE structure
//
// Params:
//  nSamplesPerSec          - Samples/Sec for the tone to be rendered
//  wValidBitsPerSample     - Valid bits per sample
//  wBitsPerSample          - Container size for the tone to be rendered
//  nChannels               - Channel count for the tone to be rendered
//  dwChannelMask           - Channel Mask
//  nSharemode              - AudioClient share mode to be used
//  eAlgorithm              - The streaming algorithm to be used
//
// Return Value:
//  The appropriate HRESULT on success, E_FAIL on failure.
//  Note: Test case failure is signalled via XLOG
////
HRESULT CapturePCMWaveFormatExtensible(DWORD nSamplesPerSec,
                                       WORD wValidBitsPerSample,
                                       WORD wBitsPerSample,
                                       WORD nChannels,
                                       DWORD dwChannelMask,
                                       AUDCLNT_SHAREMODE nShareMode,
                                       DWORD dwStreamFlags,
                                       TEST_STREAMING_ALGORITHM eAlgorithm)
{
    HRESULT hr = S_OK;

    // build the necessary format
    WAVEFORMATEXTENSIBLE wfxStreamingFormat = {0};
    wfxStreamingFormat.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
    wfxStreamingFormat.Format.nChannels = nChannels;
    wfxStreamingFormat.Format.nSamplesPerSec = nSamplesPerSec;
    wfxStreamingFormat.Format.wBitsPerSample = wBitsPerSample;
    wfxStreamingFormat.Format.nBlockAlign = (wfxStreamingFormat.Format.wBitsPerSample *
                                            wfxStreamingFormat.Format.nChannels) / BITS_PER_BYTE;
    wfxStreamingFormat.Format.nAvgBytesPerSec = wfxStreamingFormat.Format.nBlockAlign *
                                                wfxStreamingFormat.Format.nSamplesPerSec;
    wfxStreamingFormat.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE)-sizeof(WAVEFORMATEX);
    wfxStreamingFormat.Samples.wValidBitsPerSample = wValidBitsPerSample;
    wfxStreamingFormat.dwChannelMask = dwChannelMask;
    wfxStreamingFormat.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
           
    // put up a message for the user
    SLOG(eError, 
         "%.3f kHz / %d-bit (container) / %d-bit (valid) / %d ch / 0x%x (mask)",
         ((FLOAT)(nSamplesPerSec))/1000.0f,
         wBitsPerSample,
         wValidBitsPerSample,
         nChannels,
         dwChannelMask);
    SLOG(eError, "Capturing Audio Data...");

    METHODCHECKRESULT("TestCapture",
                      hr,
                      TestCapture(nShareMode, 
                                  dwStreamFlags, 
                                  (PWAVEFORMATEX)&wfxStreamingFormat, 
                                  eAlgorithm,                                  
                                  TEST_TONE_DURATION));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error during capture");
    }

    return hr;
}

