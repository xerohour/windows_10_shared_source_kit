// Copyright(C) Microsoft. All rights reserved.

//
// StreamingHelper.h
//  
//
// Description:
//
//  Helper functions to simplify functional streaming tests
//

#pragma once

////
// Render a Sine tone to the endpoint under test
//
// Params:
//      nShareMode                  - The WASAPI share mode of interest
//      dwStreamFlags               - The WASAPI streamflags to be used
//      wfxStreamingFormat          - The streaming format to be used (NULL ok in shared mode)
//      eStreamingAlgorithm         - The streaming algorithm to be used
//      fToneAmplitude              - The amplitude of the tone being rendered
//      fToneFrequency              - The frequency of the tone being rendered
//      nDuration                   - Number of milliseconds to stream
//      bEnableLoopbackVerification - Verify the render activity using loopback
//
// Returns:
//      S_OK on success, the appropriated HRESULT on failure
////
HRESULT TestRenderTone(AUDCLNT_SHAREMODE nShareMode,
                       DWORD dwStreamFlags,
                       PWAVEFORMATEX pwfxStreamingFormat,
                       TEST_STREAMING_ALGORITHM eStreamingAlgorithm,
                       FLOAT fToneAmplitude,
                       FLOAT fToneFrequency,
                       UINT32 nDuration,
                       bool bEnableLoopbackVerification = false);

////
// Capture from the endpoint under test and throw the data on the floor
//
// Params:
//      nShareMode              - The WASAPI share mode of interest
//      dwStreamFlags           - The WASAPI streamflags to be used
//      wfxStreamingFormat      - The streaming format to be used (NULL ok in shared mode)
//      eStreamingAlgorithm     - The streaming algorithm to be used
//      nDuration               - Number of milliseconds to stream
//
// Returns:
//      S_OK on success, the appropriated HRESULT on failure
////
HRESULT TestCapture(AUDCLNT_SHAREMODE nShareMode,
                    DWORD dwStreamFlags,
                    PWAVEFORMATEX pwfxStreamingFormat,
                    TEST_STREAMING_ALGORITHM eStreamingAlgorithm,
                    UINT32 nDuration);
////
// Verify whether the endpoint under test supports a format in a given share mode
//
// Params:
//      nShareMode              - The WASAPI share mode of interest
//      wfxStreamingFormat      - The streaming format to be tested
//
// Returns:
//      S_OK if the format is supported, the appropriate HRESULT on failure
//      Note: Failure is signalled via XLOG if the format is not supported, and S_FALSE is returned.
////
HRESULT CheckFormatSupported(AUDCLNT_SHAREMODE nShareMode,
                             PWAVEFORMATEX pwfxStreamingFormat);

////
// Capture loopback data from the given endpoints and report back the fundamental frequency
// of the captured buffer for each channel.
//
// Params:
//      wszRenderEndpoint  [in]         - The ID of the endpoint from which to capture loopback data
//      nMillisCaptureDuration  [in]    - The portion of time to analyze
//      pnCapturedChannels  [out]       - The number of captured channels
//      ppdCapturedToneFreqs  [out]     - Pointer to an array containing the fundamental frequencies of the captured tones.  The caller
//                                        must free this memory using CoTaskMemFree.
//      ppdCapturedToneSNR  [out]       - Pointer to an array containing the SNR values for each channel (in dB).  The caller must free
//                                        this memory using CoTaskMemFree
//      
//
// Returns:
//      S_OK if the format is supported, the appropriate HRESULT on failure
//      Note: Failure is signalled via XLOG if the format is not supported, and S_FALSE is returned.
////
HRESULT LoopbackVerify(LPCWSTR wszRenderEndpoint,
                       UINT32 nMillisCaptureDuration,
                       UINT32* pnCapturedChannels,
                       DOUBLE** ppdCapturedToneFreqs,
                       DOUBLE** ppdCapturedToneSNR);

////
// Given a buffer of audio data, return the fundamental frequency and SNR for each channel of said buffer
//
// Arguments:
//      pbBuffer [in]                       -   Pointer to a buffer containing the audio data to be analyzed
//      cbBuffer [in]                       -   Length, in bytes, of the buffer to be analyzed
//      pwfxBufferFormat [in]               -   Format of the audio data to be analyzed
//      pnFundamentalFrequencies [out]      -   The number of channels for which fundamental frequencies were calculated
//      ppdFundamentalFrequencies [out]     -   The fundamental frequencies calculated by the routine.  The caller must
//                                              free this memory using CoTaskMemFree.
//      ppdSNRValues [out]                  -   The SNR values calculated by the routine.  The caller must
//                                              free this memory using CoTaskMemFree.

//
// Return value:
//      S_OK on success, the appropriate HRESULT on failure.
////
HRESULT GetBufferFundamentalFrequencyAndSNR(LPBYTE pbBuffer,
                                            const size_t cbBuffer,
                                            LPCWAVEFORMATEX pwfxBufferFormat,
                                            PUINT32 pnFundamentalFrequencies,
                                            DOUBLE** ppdFundamentalFrequencies,
                                            DOUBLE** ppdSNRValues);
