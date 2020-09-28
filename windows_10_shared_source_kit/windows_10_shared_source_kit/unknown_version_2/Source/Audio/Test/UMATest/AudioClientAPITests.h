// Copyright(C) Microsoft. All rights reserved.

//
// AudioClientAPITests.h
//  
//
// Description:
//
//  IAudioClient API Tests
//

#pragma once

typedef enum {RENDER_SHARED = 0,
              RENDER_COMPAT,
              RENDER_COMPAT_ENHANCED,           // AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY
              RENDER_EXCLUSIVE,
              RENDER_SHARED_LOOPBACK,
              RENDER_COMPAT_LOOPBACK,
              RENDER_COMPAT_ENHANCED_LOOPBACK,  // AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY
              RENDER_OFFLOAD,
              RENDER_COMPAT_OFFLOAD,
              CAPTURE_SHARED,
              CAPTURE_COMPAT,
              CAPTURE_COMPAT_ENHANCED,          // AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY
              CAPTURE_EXCLUSIVE} STREAMING_MODE;
               
typedef enum {ISFORMATSUPPORTED_INVALID,
              INITIALIZE_INVALID,
              GETBUFFERSIZE_INVALID,
              GETSTREAMLATENCY_INVALID,
              GETCURRENTPADDING_INVALID,
              DISCONNECTSERVICE_INVALID,
              QUERYINTERFACE_INVALID,
              GETMIXFORMAT_INVALID,
              GETSERVICE_VALID,
              GETBUFFERSIZE_VALID,
              GETSTREAMLATENCY_VALID,
              SETEVENTHANDLE_VALID,
              SETEVENTHANDLE_INVALID,
              RENDER_GETBUFFER_INVALID,
              RENDER_GETBUFFERHACKER_INVALID,
              RENDER_RELEASEBUFFER_INVALID,
              CAPTURE_GETBUFFER_INVALID,
              CAPTURE_GETBUFFERHACKER_INVALID,
              CAPTURE_RELEASEBUFFER_INVALID,
              CLOCK_GETPOSITION_INVALID,
              CLOCK_GETFREQUENCY_INVALID,
              CLOCK_GETCHARACTERISTICS_INVALID} AUDIOCLIENT_TEST_ID;

const DWORD AUDIOCLIENTPROPERTY_OFFLOAD = 0x00000800;  //  Intentionally *not* LSB...

DWORD RunAudioClientTest(STREAMING_MODE nStreamingMode,
                         AUDIOCLIENT_TEST_ID nTestID,
                         HNSTIME hnsBufferDuration = 0,
                         HNSTIME hnsPeriodicity = 0);
                         

class CAudioClientApiTest
{
    AudioClientType         m_nClientType;
    AUDCLNT_SHAREMODE       m_nShareMode;
    DWORD                   m_dwRequestedStreamFlags;
    DWORD                   m_dwClientProperties;
    HNSTIME                 m_hnsBufferDuration;
    HNSTIME                 m_hnsPeriodicity;

public:
    CAudioClientApiTest(AudioClientType nClientType, 
                        AUDCLNT_SHAREMODE nShareMode, 
                        DWORD dwRequestedStreamFlags,
                        HNSTIME hnsBufferDuration,
                        HNSTIME hnsPeriodicity);

    // New constructor to account for properties through IAudioClient2::SetClientProperties()
    CAudioClientApiTest(AudioClientType nClientType, 
                        AUDCLNT_SHAREMODE nShareMode, 
                        DWORD dwRequestedStreamFlags,
                        DWORD dwClientProperties,
                        HNSTIME hnsBufferDuration,
                        HNSTIME hnsPeriodicity);

    //
    // Invalid Scenarios
    //
    
    // IAudioClient
    DWORD                   IsFormatSupported_InvalidParamsTest();
    DWORD                   Initialize_InvalidParamsTest();
    DWORD                   GetBufferSize_InvalidParamsTest();
    DWORD                   GetStreamLatency_InvalidParamsTest();
    DWORD                   GetCurrentPadding_InvalidParamsTest();
    DWORD                   DisconnectService_InvalidParamsTest();
    DWORD                   QueryInterface_InvalidParamsTest();
    DWORD                   GetMixFormat_InvalidParamsTest();
    DWORD                   SetEventHandle_InvalidParamsTest();

    //
    // Valid Scenarios
    //

    // IAudioClient
    DWORD                   GetService_ValidParamsTest();
    DWORD                   GetBufferSize_ValidParamsTest();
    DWORD                   GetStreamLatency_ValidParamsTest();    
    DWORD                   SetEventHandle_ValidParamsTest();

    // IAudioRenderClient
    DWORD Render_GetBuffer_InvalidParamsTest();
    DWORD Render_ReleaseBuffer_InvalidParamsTest();
    DWORD Render_GetBuffer_Hacker();

    // IAudioCaptureClient
    DWORD Capture_GetBuffer_InvalidParamsTest();
    DWORD Capture_ReleaseBuffer_InvalidParamsTest();
    DWORD Capture_GetBuffer_Hacker();

    // IAudioClock
    DWORD Clock_GetPosition_InvalidParamsTest();
    DWORD Clock_GetFrequency_InvalidParamsTest();
    DWORD Clock_GetCharacteristics_InvalidParamsTest();

private:
    //
    // Test Helpers
    //

    // Get an IAudioClient interface associated with the current endpoint under test
    HRESULT GetAudioClient(IAudioClient** ppAudioClient);

    // Initialize an IAudioClient interface in the share mode specified by the
    // state of this object.
    //
    // NOTE: If everything else succeeds, the result of IAudioClient::Initialize
    // is returned to the client.
    //
    // If the optional ppwfxStreamingFormat parameter is specified, a copy of the 
    // WAVEFORMATEX used to initialize the client is returned.
    // The caller must CoTaskMemFree this pointer.   
    HRESULT InitializeAudioClient(IAudioClient** ppAudioClient,
                                  WAVEFORMATEX** ppwfxStreamingFormat = NULL);


    // IAudioClient::IsFormatSupported Test Wrapper
    HRESULT Method_IsFormatSupported(ISFORMATSUPPORTED_PARAM_SPACE::ISFORMATSUPPORTED_PARAMS params,
                                     IAudioClient* pAudioClient);

    // IAudioClient::Initialize Test Wrapper
    HRESULT Method_Initialize(INITIALIZE_PARAM_SPACE::INITIALIZE_PARAMS params,
                              IAudioClient* pAudioClient);


    LPCTSTR GetMode(DWORD dwMode);
    LPCTSTR GetType(AudioClientType type);
    LPCTSTR GetFlags(DWORD dwStreamFlags);

};

