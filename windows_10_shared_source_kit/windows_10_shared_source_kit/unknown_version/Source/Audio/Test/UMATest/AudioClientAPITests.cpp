// Copyright(C) Microsoft. All rights reserved.

//
// AudioClientAPITests.cpp
//
// Description:
//
//  IAudioClient API Tests

#include <stdafx.h>
#include <UMATestStreaming.h>


#include <TestParams.h>
#include "AudioClientAPITests.h"

const UINT32 DEFAULT_CP_BUFFER_SIZE = 200000; // 20 ms
const UINT32 HNS_PER_SEC = 10000000;

DWORD TC_AudioClient_GetService_Invalid()
{   
    HRESULT hr = S_OK;
    AudioClientProperties clientProperties = {0};

    CComPtr<IMMDevice> spEndpointUnderTest = NULL;

    LPCWSTR wszDeviceUnderTest = GetEndpointIDUnderTest();
    if(NULL == wszDeviceUnderTest)
    {
        XLOG(XFAIL,
             eFatalError,
             "Unable to obtain the ID of the endpoint under test");
        goto END;
    }

    clientProperties.bIsOffload = FALSE;
    clientProperties.eCategory = AudioCategory_Other;
    clientProperties.cbSize = sizeof(clientProperties);

    hr = GetEndpointUnderTest(&spEndpointUnderTest);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eFatalError, 
             "Error calling IGetEndpointUnderTest (0x%8x)",
             hr);
        goto END;
    }

    //  To instance spAudioClient.
    {
        CComPtr<IAudioClient> spAudioClient = NULL;
        CComPtr<IAudioRenderClient> spRenderClient = NULL;
        CComPtr<IUnknown> spUnknown = NULL;
        CComPtr<IAudioCaptureClient> spCaptureClient = NULL;
        CComPtr<IAudioClock> spClock = NULL;
        CComPtr<IAudioClockAdjustment> spClockRateAdjustment = NULL;
    
        hr = spEndpointUnderTest->Activate(__uuidof(IAudioClient),
                                           CLSCTX_ALL,
                                           NULL,
                                           (void**)&spAudioClient);
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eFatalError, 
                 "Error calling IMMDevice::Activate(IAudioClient) (0x%8x)",
                 hr);
            goto END;
        }
       
        // invalid IID
        METHODCHECKRESULT("IAudioClient::GetService",
                          hr,
                          spAudioClient->GetService(GUID_NULL, 
                                                    (LPVOID*)&spUnknown));
        if (SUCCEEDED(hr)) 
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: GetService succeeded for GUID_NULL");    
        }

        // NULL
#pragma prefast(suppress:6309, "Disable parameter checking")
        METHODCHECKRESULT("IAudioClient::GetService",
                          hr,
                          spAudioClient->GetService(__uuidof(IAudioClock), 
                                                    NULL ));
        if (SUCCEEDED(hr)) 
        {
            XLOG(XFAIL,
                 eError, 
                 "FAIL: GetService succeeded w/ NULL output param");
        }

        // check services are not available
        METHODCHECKRESULT("IAudioClient::GetService(IAudioCaptureClient)", 
                          hr, 
                          spAudioClient->GetService(__uuidof(IAudioCaptureClient), 
                          (LPVOID*)&spCaptureClient));
        if (SUCCEEDED(hr)) 
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: GetService should have failed if not initialized");
        }

        METHODCHECKRESULT("IAudioClient::GetService(IAudioRenderClient)", 
                          hr, 
                          spAudioClient->GetService(__uuidof(IAudioRenderClient), 
                                                    (LPVOID*)&spRenderClient));
        if (SUCCEEDED(hr)) 
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: GetService should have failed if not initialized");
        }

        METHODCHECKRESULT("IAudioClient::GetService(IAudioClock)", 
                          hr, 
                          spAudioClient->GetService( __uuidof(IAudioClock), 
                                                    (LPVOID*)&spClock));
        if (SUCCEEDED(hr)) 
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: GetService should have failed if not initialized");
        }

        METHODCHECKRESULT("IAudioClient::GetService(IAudioClockRateAdjustment)", 
                          hr, 
                          spAudioClient->GetService( __uuidof(IAudioClockAdjustment), 
                                                    (LPVOID*)&spClockRateAdjustment));
        if (SUCCEEDED(hr)) 
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: GetService should have failed if not initialized");
        }
    }

    if(DEVICE_TYPE_RENDER == GetDeviceTypeUnderTest())
    {
        //
        // Shared Mode Render
        //
        SLOG(eInfo1, "<SHARED MODE RENDER>");
        {
            CComPtr<IAudioClient> spAudioClient = NULL;
            CComPtr<IAudioCaptureClient> spCaptureClient = NULL;
            CComPtr<IRenderTestStreamingEngine> spRenderEngine = NULL;
            CComPtr<ITestStreamingEngine> spRenderEngineCommon = NULL;
            CComPtr<IAudioClockAdjustment> spClockRateAdjustment = NULL;
        
            METHODCHECKRESULT("CreateTestStreamingEngineOnEndpointId(IRenderTestStreamingEngine)",
                              hr,
                              CreateTestStreamingEngineOnEndpointId(IID_IRenderTestStreamingEngine,
                                                        wszDeviceUnderTest,
                                                        TIMER_PUSH_RENDER,
                                                        GetBasicLogger(),
                                                        (void**)(&spRenderEngine)));
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: Unable to create test streaming engine");
                goto END;
            }

            METHODCHECKRESULT("IRenderTestStreamingEngine::QueryInterface(ITestStreamingEngine)",
                              hr,
                              spRenderEngine->QueryInterface(IID_ITestStreamingEngine,
                                                             (void**)(&spRenderEngineCommon)));

            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: Unable to obtain ITestStreamingEngine interface");
                goto END;
            }

            METHODCHECKRESULT("ITestStreamingEngine::SetRequestedShareMode",
                              hr,
                              spRenderEngineCommon->SetRequestedShareMode(AUDCLNT_SHAREMODE_SHARED));
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: Unable to set requested share mode");
                goto END;
            }

            METHODCHECKRESULT("ITestStreamingEngine::Initialize",
                              hr,
                              spRenderEngineCommon->Initialize(&clientProperties));
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: ITestStreamingEngine::Initialize returned unexpected result");
                goto END;
            }

            // get the initialize IAudioClient pointer
            METHODCHECKRESULT("ITestStreamingEngine::GetAudioClient",
                              hr,
                              spRenderEngineCommon->GetAudioClient(&spAudioClient));
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: ITestStreamingENgine::GetAudioClient returned unexpected result");
                goto END;
            }

            // attempt to get an IAudioCapture client interface
            METHODCHECKRESULT("IAudioClient::GetService(IAudioCaptureClient)", 
                              hr, 
                              spAudioClient->GetService( __uuidof(IAudioCaptureClient), 
                                                        (LPVOID*)&spCaptureClient));
            if (SUCCEEDED(hr)) 
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: GetService should not succeed for a render endpoint!");
            }

            // attempt to get an IAudioClockAdjustmentInterface
            METHODCHECKRESULT("IAudioClient::GetService(IAudioCaptureClient)", 
                              hr, 
                              spAudioClient->GetService( __uuidof(IAudioClockAdjustment), 
                                                        (LPVOID*)&spClockRateAdjustment));
            if (SUCCEEDED(hr)) 
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: GetService should not succeed without RATEADJUST flag specified!");
            }        

            METHODCHECKRESULT("ITestStreamingEngine::Reset",
                              hr,                          
                              spRenderEngineCommon->Reset());
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: ITestStreamingEngine::Reset returned unexpected result");
                goto END;
            }
        }
        
        //
        //  Exclusive Mode Render
        //
        SLOG(eInfo1, "<EXCLUSIVE MODE RENDER>");
        if (GetDeviceUnderTestAllowsExclusiveMode())
        {
            CComPtr<IAudioClient> spAudioClient = NULL;
            CComPtr<IAudioCaptureClient> spCaptureClient = NULL;
            CComPtr<IRenderTestStreamingEngine> spRenderEngine = NULL;
            CComPtr<ITestStreamingEngine> spRenderEngineCommon = NULL;
            CComPtr<IAudioClockAdjustment> spClockRateAdjustment = NULL;
        
            METHODCHECKRESULT("CreateTestStreamingEngineOnEndpointId(IRenderTestStreamingEngine)",
                              hr,
                              CreateTestStreamingEngineOnEndpointId(IID_IRenderTestStreamingEngine,
                                                        wszDeviceUnderTest,
                                                        TIMER_PUSH_RENDER,
                                                        GetBasicLogger(),
                                                        (void**)(&spRenderEngine)));
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: Unable to create test streaming engine");
                goto END;
            }

            METHODCHECKRESULT("IRenderTestStreamingEngine::QueryInterface(ITestStreamingEngine)",
                              hr,
                              spRenderEngine->QueryInterface(IID_ITestStreamingEngine,
                                                             (void**)(&spRenderEngineCommon)));

            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: Unable to obtain ITestStreamingEngine interface");
                goto END;
            }
            
            METHODCHECKRESULT("ITestStreamingEngine::SetRequestedShareMode",
                              hr,
                              spRenderEngineCommon->SetRequestedShareMode(AUDCLNT_SHAREMODE_EXCLUSIVE));
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: ITestStreamingEngine::SetRequestedShareMode returned unexpected result");
                goto END;
            }

            METHODCHECKRESULT("ITestStreamingEngine::SetRequestedStreamingFormat",
                              hr,
                              spRenderEngineCommon->SetRequestedStreamingFormat(NULL));
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: ITestStreamingEngine::SetRequestedStreamingFormat returned unexpected result");
                goto END;
            }
        
            METHODCHECKRESULT("ITestStreamingEngine::Initialize",
                              hr,
                              spRenderEngineCommon->Initialize(&clientProperties));
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: ITestStreamingEngine::Initialize should not have failed");
                goto END;
            }

            // get the initialize IAudioClient pointer
            METHODCHECKRESULT("ITestStreamingEngine::GetAudioClient",
                              hr,
                              spRenderEngineCommon->GetAudioClient(&spAudioClient));
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: ITestStreamingEngine::GetAudioClient returned unexpected result");
                goto END;
            }

            // attempt to get an IAudioCaptureClient interface
            METHODCHECKRESULT("IAudioClient::GetService(IAudioCaptureClient)", 
                              hr, 
                              spAudioClient->GetService( __uuidof(IAudioCaptureClient), 
                                                        (LPVOID*)&spCaptureClient));
            if(SUCCEEDED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: ITestStreamingEngine::GetService returned unexpected result");
                goto END;
            }

            // attempt to get an IAudioClockAdjustmentInterface
            METHODCHECKRESULT("IAudioClient::GetService(IAudioCaptureClient)", 
                              hr, 
                              spAudioClient->GetService( __uuidof(IAudioClockAdjustment), 
                                                        (LPVOID*)&spClockRateAdjustment));
            if (SUCCEEDED(hr)) 
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: GetService should not succeed without RATEADJUST flag specified!");
            }   
        
            METHODCHECKRESULT("ITestStreamingEngine::Reset",
                              hr,                          
                              spRenderEngineCommon->Reset());
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: ITestStreamingEngine::Reset returned unexpected result");
                goto END;
            }
        }
        else
        {
            SLOG(eInfo2, "Skipping Exclusive mode portion of test on Endpoint that doesn't allow Exclusive Mode");
        }

        //
        // Shared Mode Loopback (Render)
        //
        SLOG(eInfo1, "<SHARED MODE LOOPBACK (RENDER)>");

        {
            CComPtr<IAudioRenderClient> spRenderClient = NULL;
            CComPtr<ITestStreamingEngine> spCaptureEngine = NULL;
            CComPtr<ITestStreamingEngine> spCaptureEngineCommon = NULL;
            CComPtr<IAudioClient> spAudioClient = NULL;
            CComPtr<IAudioClockAdjustment> spClockRateAdjustment = NULL;
            
            // create a capture streaming engine
            METHODCHECKRESULT("CreateTestStreamingEngineOnEndpointId(ICaptureTestStreamingEngine)",
                              hr,
                              CreateTestStreamingEngineOnEndpointId(IID_ICaptureTestStreamingEngine,
                                                        wszDeviceUnderTest,
                                                        TIMER_PUSH_CAPTURE,
                                                        GetBasicLogger(),
                                                        (void**)(&spCaptureEngine)));
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: CreateTestStreamingEngineOnEndpointId returned unexpected result");
                goto END;
            }

            METHODCHECKRESULT("ICaptureTestStreamingEngine::QueryInterface(ITestStreamingEngine)",
                              hr,
                              spCaptureEngine->QueryInterface(IID_ITestStreamingEngine,
                                                              (void**)(&spCaptureEngineCommon)));

            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: ICaptureTestStreamingEngine::QueryInterface returned unexpected result");
                goto END;
            }

            METHODCHECKRESULT("ITestStreamingEngine::SetRequestedShareMode",
                              hr,
                              spCaptureEngineCommon->SetRequestedShareMode(AUDCLNT_SHAREMODE_SHARED));
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: ITestStreamingEngine::SetRequestedShareMode returned unexpected result");
                goto END;
            }

            METHODCHECKRESULT("ITestStreamingEngine::SetRequestedStreamingFlags",
                              hr,
                              spCaptureEngineCommon->SetRequestedStreamingFlags(AUDCLNT_STREAMFLAGS_LOOPBACK));
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: ITestStreamingEngine::SetRequestedStreamFlags returned unexpected result");
                goto END;
            }

            METHODCHECKRESULT("ITestStreamingEngine::SetRequestedStreamingFormat",
                              hr,
                              spCaptureEngineCommon->SetRequestedStreamingFormat(NULL));
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: ITestStreamingEngine::SetRequestedStreamingFormat returned unexpected result");
                goto END;
            }

            METHODCHECKRESULT("ITestStreamingEngine::Initialize",
                              hr,
                              spCaptureEngineCommon->Initialize(&clientProperties));
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: ITestStreamingEngine::Initialize returned unexpected result");
                goto END;
            }

            // get the initialize IAudioClient pointer
            METHODCHECKRESULT("ITestStreamingEngine::GetAudioClient",
                              hr,
                              spCaptureEngineCommon->GetAudioClient(&spAudioClient));
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: ITestStreamingEngine::Initialize returned unexpected result");
                goto END;
            }

            // attempt to get an IAudioRenderClient interface
            METHODCHECKRESULT("IAudioClient::GetService(IAudioRenderClient) (Loopback)", 
                              hr, 
                              spAudioClient->GetService( __uuidof(IAudioRenderClient), 
                                                        (LPVOID*)&spRenderClient));
            if (SUCCEEDED(hr)) 
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: GetService should not succeed for a render endpoint in loopback mode!");
            }             

            // attempt to get an IAudioClockAdjustmentInterface
            METHODCHECKRESULT("IAudioClient::GetService(IAudioCaptureClient)", 
                              hr, 
                              spAudioClient->GetService( __uuidof(IAudioClockAdjustment), 
                                                        (LPVOID*)&spClockRateAdjustment));
            if (SUCCEEDED(hr)) 
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: GetService should not succeed without RATEADJUST flag specified!");
            }   
        }
    }

    if(DEVICE_TYPE_CAPTURE == GetDeviceTypeUnderTest())
    {
        //
        // Shared Mode Capture
        //

        SLOG(eInfo1, "<SHARED MODE CAPTURE>");
        {
            CComPtr<IAudioClient> spAudioClient = NULL;
            CComPtr<IAudioRenderClient> spRenderClient = NULL;
            CComPtr<ITestStreamingEngine> spCaptureEngine = NULL;
            CComPtr<ITestStreamingEngine> spCaptureEngineCommon = NULL;
            CComPtr<IAudioClockAdjustment> spClockRateAdjustment = NULL;
            
            // create a capture streaming engine
            METHODCHECKRESULT("CreateTestStreamingEngineOnEndpointId(ICaptureTestStreamingEngine)",
                              hr,
                              CreateTestStreamingEngineOnEndpointId(IID_ICaptureTestStreamingEngine,
                                                        wszDeviceUnderTest,
                                                        TIMER_PUSH_CAPTURE,
                                                        GetBasicLogger(),
                                                        (void**)(&spCaptureEngine)));
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: CreateTestStreamingEngineOnEndpointId returned unexpected result");
                goto END;       
            }

            METHODCHECKRESULT("ICaptureTestStreamingEngine::QueryInterface(ITestStreamingEngine)",
                              hr,
                              spCaptureEngine->QueryInterface(IID_ITestStreamingEngine,
                                                              (void**)(&spCaptureEngineCommon)));

            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: ICaptureTestStreamingEngine::QueryInterface returned unexpected result");
                goto END;
            }

            METHODCHECKRESULT("ITestStreamingEngine::SetRequestedShareMode",
                              hr,
                              spCaptureEngineCommon->SetRequestedShareMode(AUDCLNT_SHAREMODE_SHARED));
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: ITestStreamingEngine::SetRequestedShareMode returned unexpected result");
                goto END;
            }

            METHODCHECKRESULT("ITestStreamingEngine::Initialize",
                              hr,
                              spCaptureEngineCommon->Initialize(&clientProperties));
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: ITestStreamingEngine::Initialize returned unexpected result");
                goto END;
            }

            // get the initialize IAudioClient pointer
            METHODCHECKRESULT("ITestStreamingEngine::GetAudioClient",
                              hr,
                              spCaptureEngineCommon->GetAudioClient(&spAudioClient));
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: ITestStreamingEngine::GetAudioClient returned unexpected result");
                goto END;
            }

            // attempt to get an IAudioRenderClient interface
            METHODCHECKRESULT("IAudioClient::GetService(IAudioRenderClient) (Loopback)", 
                              hr, 
                              spAudioClient->GetService( __uuidof(IAudioRenderClient), 
                                                        (LPVOID*)&spRenderClient));
            if (SUCCEEDED(hr)) 
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: GetService should not succeed for a capture endpoint!");
            }             

            // attempt to get an IAudioClockAdjustmentInterface
            METHODCHECKRESULT("IAudioClient::GetService(IAudioCaptureClient)", 
                              hr, 
                              spAudioClient->GetService( __uuidof(IAudioClockAdjustment), 
                                                        (LPVOID*)&spClockRateAdjustment));
            if (SUCCEEDED(hr)) 
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: GetService should not succeed without RATEADJUST flag specified!");
            }   

            METHODCHECKRESULT("ITestStreamingEngine::Reset",
                              hr,                          
                              spCaptureEngineCommon->Reset());
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: ITestStreamingEngine::Reset returned unexpected result");
                goto END;
            }

        }


        {
            CComPtr<IAudioClient> spAudioClient = NULL;
            CComPtr<ITestStreamingEngine> spCaptureEngine = NULL;
            CComPtr<ITestStreamingEngine> spCaptureEngineCommon = NULL;
            CComPtr<IAudioClockAdjustment> spClockRateAdjustment = NULL;

            // create a capture streaming engine
            METHODCHECKRESULT("CreateTestStreamingEngineOnEndpointId(ICaptureTestStreamingEngine)",
                              hr,
                              CreateTestStreamingEngineOnEndpointId(IID_ICaptureTestStreamingEngine,
                                                        wszDeviceUnderTest,
                                                        TIMER_PUSH_CAPTURE,
                                                        GetBasicLogger(),
                                                        (void**)(&spCaptureEngine)));
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: CreateTestStreamingEngineOnEndpointId returned unexpected result");
                goto END;       
            }

            METHODCHECKRESULT("ICaptureTestStreamingEngine::QueryInterface(ITestStreamingEngine)",
                              hr,
                              spCaptureEngine->QueryInterface(IID_ITestStreamingEngine,
                                                              (void**)(&spCaptureEngineCommon)));

            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: ICaptureTestStreamingEngine::QueryInterface returned unexpected result");
                goto END;
            }
            
            //
            // ensure IAudioClockAdjustment is never available for capture endpoints
            //

            METHODCHECKRESULT("ITestStreamingEngine::SetRequestedShareMode",
                              hr,
                              spCaptureEngineCommon->SetRequestedStreamingFlags(AUDCLNT_STREAMFLAGS_RATEADJUST));
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: ITestStreamingEngine::SetRequestedShareMode returned unexpected result");
                goto END;
            }

            METHODCHECKRESULT("ITestStreamingEngine::Initialize",
                              hr,
                              spCaptureEngineCommon->Initialize(&clientProperties));
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: ITestStreamingEngine::Initialize returned unexpected result");
                goto END;
            }

            // get the initialized IAudioClient pointer
            METHODCHECKRESULT("ITestStreamingEngine::GetAudioClient",
                              hr,
                              spCaptureEngineCommon->GetAudioClient(&spAudioClient));
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: ITestStreamingEngine::GetAudioClient returned unexpected result");
                goto END;
            }

            // attempt to get an IAudioClockAdjustmentInterface
            METHODCHECKRESULT("IAudioClient::GetService(IAudioClockAdjustment)", 
                              hr, 
                              spAudioClient->GetService( __uuidof(IAudioClockAdjustment), 
                                                        (LPVOID*)&spClockRateAdjustment));
            if (SUCCEEDED(hr)) 
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: GetService should not succeed for capture endpoints!");
            }   
        }

        // Exclusive Mode Capture
        SLOG(eInfo1, "<EXCLUSIVE MODE CAPTURE>");
        if (GetDeviceUnderTestAllowsExclusiveMode())
        {
            CComPtr<IAudioClient> spAudioClient = NULL;
            CComPtr<IAudioRenderClient> spRenderClient = NULL;
            CComPtr<ITestStreamingEngine> spCaptureEngine = NULL;
            CComPtr<ITestStreamingEngine> spCaptureEngineCommon = NULL;
            CComPtr<IAudioClockAdjustment> spClockRateAdjustment = NULL;
            
            // create a capture streaming engine
            METHODCHECKRESULT("CreateTestStreamingEngineOnEndpointId(ICaptureTestStreamingEngine)",
                              hr,
                              CreateTestStreamingEngineOnEndpointId(IID_ICaptureTestStreamingEngine,
                                                        wszDeviceUnderTest,
                                                        TIMER_PUSH_CAPTURE,
                                                        GetBasicLogger(),
                                                        (void**)(&spCaptureEngine)));
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: CreateTestStreamingEngineOnEndpointId returned unexpected result");
                goto END;       
            }

            METHODCHECKRESULT("ICaptureTestStreamingEngine::QueryInterface(ITestStreamingEngine)",
                              hr,
                              spCaptureEngine->QueryInterface(IID_ITestStreamingEngine,
                                                              (void**)(&spCaptureEngineCommon)));
                                                              
            METHODCHECKRESULT("ITestStreamingEngine::SetRequestedShareMode",
                              hr,
                              spCaptureEngineCommon->SetRequestedShareMode(AUDCLNT_SHAREMODE_EXCLUSIVE));
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: ITestStreamingEngine::SetRequestedShareMode returned unexpected result");
                goto END;
            }

            METHODCHECKRESULT("ITestStreamingEngine::SetRequestedStreamingFormat",
                              hr,
                              spCaptureEngineCommon->SetRequestedStreamingFormat(NULL));
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: ITestStreamingEngine::SetRequestedStreamingFormat returned unexpected result");
                goto END;
            }

            METHODCHECKRESULT("ITestStreamingEngine::Initialize",
                              hr,
                              spCaptureEngineCommon->Initialize(&clientProperties));
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: ITestStreamingEngine::Initialize returned unexpected result");
                goto END;
            }

            // get the initialize IAudioClient pointer
            METHODCHECKRESULT("ITestStreamingEngine::GetAudioClient",
                              hr,
                              spCaptureEngineCommon->GetAudioClient(&spAudioClient));
            if(FAILED(hr))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: ITestStreamingEngine::GetAudioClient returned unexpected result");
                goto END;
            }

            // attempt to get an IAudioRenderClient interface
            METHODCHECKRESULT("IAudioClient::GetService(IAudioRenderClient)", 
                              hr, 
                              spAudioClient->GetService( __uuidof(IAudioRenderClient), 
                                                        (LPVOID*)&spRenderClient));
            if (SUCCEEDED(hr)) 
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: GetService should not succeed for a capture endpoint!");
            }             

            // attempt to get an IAudioClockAdjustmentInterface
            METHODCHECKRESULT("IAudioClient::GetService(IAudioCaptureClient)", 
                              hr, 
                              spAudioClient->GetService( __uuidof(IAudioClockAdjustment), 
                                                        (LPVOID*)&spClockRateAdjustment));
            if (SUCCEEDED(hr)) 
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: GetService should not succeed without RATEADJUST flag specified!");
            }   
        }
        else
        {
            SLOG(eInfo1, "Skipping Exclusive mode on endpoint that doesn't allow it");
        }
    }

END:
    return FNS_PASS;
}

DWORD RunAudioClientTest(STREAMING_MODE nStreamingMode,
                         AUDIOCLIENT_TEST_ID nTestID,
                         HNSTIME hnsBufferDuration,
                         HNSTIME hnsPeriodicity)
{
    CAudioClientApiTest* pobjTest = NULL;

    //
    // Initialize a test object using the appropriate streaming mode.
    //
    switch(nStreamingMode)
    {
        case RENDER_SHARED:
            // skip if not a render device
            if(DEVICE_TYPE_RENDER != GetDeviceTypeUnderTest())
            {
                goto END;
            }

            SLOG(eInfo1, "<RENDER SHARED>");
            
            pobjTest = new  CAudioClientApiTest(AUDIO_CLIENT_RENDER,
                                                AUDCLNT_SHAREMODE_SHARED,
                                                0,
                                                hnsBufferDuration,
                                                hnsPeriodicity);
            break;

        case RENDER_COMPAT:
            // skip if not a render device
            if(DEVICE_TYPE_RENDER != GetDeviceTypeUnderTest())
            {
                goto END;
            }

            SLOG(eInfo1, "<RENDER COMPAT>");

            pobjTest = new CAudioClientApiTest(AUDIO_CLIENT_RENDER,
                                               AUDCLNT_SHAREMODE_SHARED,
                                               AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                               hnsBufferDuration,
                                               hnsPeriodicity);
            break;

        case RENDER_COMPAT_ENHANCED:
            // skip if not a render device
            if(DEVICE_TYPE_RENDER != GetDeviceTypeUnderTest())
            {
                goto END;
            }

            SLOG(eInfo1, "<RENDER COMPAT ENHANCED (SRC_DEFAULT_QUALITY)>");

            pobjTest = new CAudioClientApiTest(AUDIO_CLIENT_RENDER,
                                               AUDCLNT_SHAREMODE_SHARED,
                                               AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                               AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                               hnsBufferDuration,
                                               hnsPeriodicity);
            break;

        case RENDER_SHARED_LOOPBACK:
            // skip if not a render device
            if(DEVICE_TYPE_RENDER != GetDeviceTypeUnderTest())
            {
                goto END;
            }

            SLOG(eInfo1, "<RENDER SHARED, LOOPBACK>");
            
            pobjTest = new CAudioClientApiTest(AUDIO_CLIENT_LOOPBACK,
                                               AUDCLNT_SHAREMODE_SHARED,
                                               AUDCLNT_STREAMFLAGS_LOOPBACK,
                                               hnsBufferDuration,
                                               hnsPeriodicity);
            break;

        case RENDER_COMPAT_LOOPBACK:
            // skip if not a render device
            if(DEVICE_TYPE_RENDER != GetDeviceTypeUnderTest())
            {
                goto END;
            }

            SLOG(eInfo1, "<RENDER COMPAT, LOOPBACK>");

            pobjTest = new CAudioClientApiTest(AUDIO_CLIENT_LOOPBACK,
                                               AUDCLNT_SHAREMODE_SHARED,
                                               AUDCLNT_STREAMFLAGS_LOOPBACK | 
                                               AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                               hnsBufferDuration,
                                               hnsPeriodicity);
            break;

        case RENDER_COMPAT_ENHANCED_LOOPBACK:
            // skip if not a render device
            if(DEVICE_TYPE_RENDER != GetDeviceTypeUnderTest())
            {
                goto END;
            }

            SLOG(eInfo1, "<RENDER COMPAT ENHANCED, LOOPBACK>");

            pobjTest = new CAudioClientApiTest(AUDIO_CLIENT_LOOPBACK,
                                               AUDCLNT_SHAREMODE_SHARED,
                                               AUDCLNT_STREAMFLAGS_LOOPBACK | 
                                               AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                               AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                               hnsBufferDuration,
                                               hnsPeriodicity);
            break;
        case RENDER_EXCLUSIVE:
            if (GetDeviceUnderTestAllowsExclusiveMode())
            {
                // skip if not a render device
                if(DEVICE_TYPE_RENDER != GetDeviceTypeUnderTest())
                {
                    goto END;
                }

                SLOG(eInfo1, "<RENDER EXCLUSIVE>");

                pobjTest = new CAudioClientApiTest(AUDIO_CLIENT_RENDER,
                                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                                   0,
                                                   hnsBufferDuration,
                                                   hnsPeriodicity);
            }
            else
            {
                SLOG(eInfo1, "Skipping exclusive mode for endpoint that doesn't allow it");
                goto END;
            }
            break;
            
        case RENDER_OFFLOAD:
            // skip if not a render device
            if(DEVICE_TYPE_RENDEROFFLOAD != GetDeviceTypeUnderTest())
            {
                goto END;
            }

            SLOG(eInfo1, "<RENDER OFFLOAD>");

            //  Note:  Alternate constructor to account for offload property...
            pobjTest = new CAudioClientApiTest(AUDIO_CLIENT_RENDER,
                                               AUDCLNT_SHAREMODE_SHARED,
                                               0,
                                               AUDIOCLIENTPROPERTY_OFFLOAD,
                                               hnsBufferDuration,
                                               hnsPeriodicity);
            break;

        case RENDER_COMPAT_OFFLOAD:
            // skip if not a render device
            if(DEVICE_TYPE_RENDEROFFLOAD != GetDeviceTypeUnderTest())
            {
                goto END;
            }

            SLOG(eInfo1, "<RENDER COMPAT OFFLOAD>");

            //  Note:  Alternate constructor to account for offload property...
            pobjTest = new CAudioClientApiTest(AUDIO_CLIENT_RENDER,
                                               AUDCLNT_SHAREMODE_SHARED,
                                               AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                               AUDIOCLIENTPROPERTY_OFFLOAD,
                                               hnsBufferDuration,
                                               hnsPeriodicity);
            break;
            
        case CAPTURE_SHARED:
            // skip if not a capture device
            if(DEVICE_TYPE_CAPTURE != GetDeviceTypeUnderTest())
            {
                goto END;
            }

            SLOG(eInfo1, "<CAPTURE SHARED>");
            
            pobjTest = new CAudioClientApiTest(AUDIO_CLIENT_CAPTURE,
                                               AUDCLNT_SHAREMODE_SHARED,
                                               0,
                                               hnsBufferDuration,
                                               hnsPeriodicity);
            break;

        case CAPTURE_COMPAT:
            // skip if not a capture device
            if(DEVICE_TYPE_CAPTURE != GetDeviceTypeUnderTest())
            {
                goto END;
            }

            SLOG(eInfo1, "<CAPTURE COMPAT>");
            
            pobjTest = new CAudioClientApiTest(AUDIO_CLIENT_CAPTURE,
                                               AUDCLNT_SHAREMODE_SHARED,
                                               AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                               hnsBufferDuration,
                                               hnsPeriodicity);
            break;

        case CAPTURE_COMPAT_ENHANCED:
            // skip if not a capture device
            if(DEVICE_TYPE_CAPTURE != GetDeviceTypeUnderTest())
            {
                goto END;
            }

            SLOG(eInfo1, "<CAPTURE COMPAT ENHANCED>");
            
            pobjTest = new CAudioClientApiTest(AUDIO_CLIENT_CAPTURE,
                                               AUDCLNT_SHAREMODE_SHARED,
                                               AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | 
                                               AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                               hnsBufferDuration,
                                               hnsPeriodicity);
            break;
            
        case CAPTURE_EXCLUSIVE:
            if (GetDeviceUnderTestAllowsExclusiveMode())
            {
                // skip if not a capture device
                if(DEVICE_TYPE_CAPTURE != GetDeviceTypeUnderTest())
                {
                    goto END;
                }

                SLOG(eInfo1, "<CAPTURE EXCLUSIVE>");
                
                pobjTest = new CAudioClientApiTest(AUDIO_CLIENT_CAPTURE,
                                                   AUDCLNT_SHAREMODE_EXCLUSIVE,
                                                   0,
                                                   hnsBufferDuration,
                                                   hnsPeriodicity);
            }
            else
            {
                SLOG(eInfo1, "Skipping exclusive mode for endpoint that doesn't allow it");
                goto END;
            }
            break;
        
        default:            
            XLOG(XFAIL,
                 eError,
                 "FAIL: Encountered unknown streaming mode!");
            goto END;
    }

    if(NULL == pobjTest)
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Out of memory!");
        goto END;
    }

    //
    // Run the appropriate test
    //
    switch(nTestID)
    {
        //
        // IAudioClient (Invalid)
        //

        case ISFORMATSUPPORTED_INVALID:
            pobjTest->IsFormatSupported_InvalidParamsTest();
            break;
            
        case INITIALIZE_INVALID:
            pobjTest->Initialize_InvalidParamsTest();
            break;
            
        case GETBUFFERSIZE_INVALID:
            pobjTest->GetBufferSize_InvalidParamsTest();
            break;
            
        case GETSTREAMLATENCY_INVALID:
            pobjTest->GetStreamLatency_InvalidParamsTest();
            break;
            
        case GETCURRENTPADDING_INVALID:
            pobjTest->GetCurrentPadding_InvalidParamsTest();
            break;

        case DISCONNECTSERVICE_INVALID:
            pobjTest->DisconnectService_InvalidParamsTest();
            break;

        case QUERYINTERFACE_INVALID:
            pobjTest->QueryInterface_InvalidParamsTest();
            break;

        case GETMIXFORMAT_INVALID:
            pobjTest->GetMixFormat_InvalidParamsTest();
            break;

        case SETEVENTHANDLE_INVALID:            
            pobjTest->SetEventHandle_InvalidParamsTest();
            break;

        //
        // IAudioClient (Valid)
        //

        case GETSERVICE_VALID:
            pobjTest->GetService_ValidParamsTest();
            break;

        case GETBUFFERSIZE_VALID:
            pobjTest->GetBufferSize_ValidParamsTest();
            break;

        case GETSTREAMLATENCY_VALID:
            pobjTest->GetStreamLatency_ValidParamsTest();
            break;

        case SETEVENTHANDLE_VALID:
            pobjTest->SetEventHandle_ValidParamsTest();
            break;

        //
        // IAudioRenderClient (Invalid)
        //
        case RENDER_GETBUFFER_INVALID:
            pobjTest->Render_GetBuffer_InvalidParamsTest();
            break;

        case RENDER_GETBUFFERHACKER_INVALID:
            pobjTest->Render_GetBuffer_Hacker();
            break;

        case RENDER_RELEASEBUFFER_INVALID:
            pobjTest->Render_ReleaseBuffer_InvalidParamsTest();
            break;

        //
        // IAudioCaptureClient (Invalid)
        //
        case CAPTURE_GETBUFFER_INVALID:
            pobjTest->Capture_GetBuffer_InvalidParamsTest();
            break;

        case CAPTURE_GETBUFFERHACKER_INVALID:
            pobjTest->Capture_GetBuffer_Hacker();
            break;

        case CAPTURE_RELEASEBUFFER_INVALID:
            pobjTest->Capture_ReleaseBuffer_InvalidParamsTest();
            break;              
            
        //
        // IAudioClock (Invalid)
        //

        case CLOCK_GETPOSITION_INVALID:
            pobjTest->Clock_GetPosition_InvalidParamsTest();
            break;

        case CLOCK_GETFREQUENCY_INVALID:
            pobjTest->Clock_GetFrequency_InvalidParamsTest();
            break;

        case CLOCK_GETCHARACTERISTICS_INVALID:
            pobjTest->Clock_GetCharacteristics_InvalidParamsTest();
            break;
                          
        default:
            XLOG(XFAIL,
                 eError,
                 "FAIL: Invalid test ID: %d", nTestID);
            goto END;
    }

END:
    return FNS_PASS;
}

DWORD TC_AudioClient_IsFormatSupported_Invalid()
{
    //
    // Render
    //
    RunAudioClientTest(RENDER_SHARED,           ISFORMATSUPPORTED_INVALID);
    RunAudioClientTest(RENDER_COMPAT,           ISFORMATSUPPORTED_INVALID);
    RunAudioClientTest(RENDER_SHARED_LOOPBACK,  ISFORMATSUPPORTED_INVALID);
    RunAudioClientTest(RENDER_COMPAT_LOOPBACK,  ISFORMATSUPPORTED_INVALID);
    RunAudioClientTest(RENDER_EXCLUSIVE,        ISFORMATSUPPORTED_INVALID);

    //
    // Capture
    //
    RunAudioClientTest(CAPTURE_SHARED,          ISFORMATSUPPORTED_INVALID);
    RunAudioClientTest(CAPTURE_COMPAT,          ISFORMATSUPPORTED_INVALID);
    RunAudioClientTest(CAPTURE_EXCLUSIVE,       ISFORMATSUPPORTED_INVALID);
        
    return FNS_PASS;
}

DWORD TC_AudioClient_Initialize_Invalid()
{
    //
    // Render
    //
    RunAudioClientTest(RENDER_SHARED,           INITIALIZE_INVALID);
    RunAudioClientTest(RENDER_COMPAT,           INITIALIZE_INVALID);
    RunAudioClientTest(RENDER_COMPAT_ENHANCED,  INITIALIZE_INVALID);
    RunAudioClientTest(RENDER_SHARED_LOOPBACK,  INITIALIZE_INVALID);
    RunAudioClientTest(RENDER_COMPAT_LOOPBACK,  INITIALIZE_INVALID);
    RunAudioClientTest(RENDER_COMPAT_ENHANCED_LOOPBACK,  INITIALIZE_INVALID);
    RunAudioClientTest(RENDER_EXCLUSIVE,        INITIALIZE_INVALID);


    //
    // Capture
    //
    RunAudioClientTest(CAPTURE_SHARED,          INITIALIZE_INVALID);
    RunAudioClientTest(CAPTURE_COMPAT,          INITIALIZE_INVALID);
    RunAudioClientTest(CAPTURE_COMPAT_ENHANCED, INITIALIZE_INVALID);
    RunAudioClientTest(CAPTURE_EXCLUSIVE,       INITIALIZE_INVALID);
        
    return FNS_PASS;
}

DWORD TC_AudioClient_GetBufferSize_Invalid()
{
    //
    // Render
    //
    RunAudioClientTest(RENDER_SHARED,           GETBUFFERSIZE_INVALID);
    RunAudioClientTest(RENDER_COMPAT,           GETBUFFERSIZE_INVALID);
    RunAudioClientTest(RENDER_SHARED_LOOPBACK,  GETBUFFERSIZE_INVALID);
    RunAudioClientTest(RENDER_COMPAT_LOOPBACK,  GETBUFFERSIZE_INVALID);
    RunAudioClientTest(RENDER_EXCLUSIVE,        GETBUFFERSIZE_INVALID);


    //
    // Capture
    //
    RunAudioClientTest(CAPTURE_SHARED,          GETBUFFERSIZE_INVALID);
    RunAudioClientTest(CAPTURE_COMPAT,          GETBUFFERSIZE_INVALID);
    RunAudioClientTest(CAPTURE_EXCLUSIVE,       GETBUFFERSIZE_INVALID);
        
    return FNS_PASS;
}

DWORD TC_AudioClient_GetStreamLatency_Invalid()
{
    //
    // Render
    //
    RunAudioClientTest(RENDER_SHARED,           GETSTREAMLATENCY_INVALID);
    RunAudioClientTest(RENDER_COMPAT,           GETSTREAMLATENCY_INVALID);
    RunAudioClientTest(RENDER_SHARED_LOOPBACK,  GETSTREAMLATENCY_INVALID);
    RunAudioClientTest(RENDER_COMPAT_LOOPBACK,  GETSTREAMLATENCY_INVALID);
    RunAudioClientTest(RENDER_EXCLUSIVE,        GETSTREAMLATENCY_INVALID);


    //
    // Capture
    //
    RunAudioClientTest(CAPTURE_SHARED,          GETSTREAMLATENCY_INVALID);
    RunAudioClientTest(CAPTURE_COMPAT,          GETSTREAMLATENCY_INVALID);
    RunAudioClientTest(CAPTURE_EXCLUSIVE,       GETSTREAMLATENCY_INVALID);

    return FNS_PASS;
}

DWORD TC_AudioClient_GetCurrentPadding_Invalid()
{
    //
    // Render
    //
    RunAudioClientTest(RENDER_SHARED,           GETCURRENTPADDING_INVALID);
    RunAudioClientTest(RENDER_COMPAT,           GETCURRENTPADDING_INVALID);
    RunAudioClientTest(RENDER_SHARED_LOOPBACK,  GETCURRENTPADDING_INVALID);
    RunAudioClientTest(RENDER_COMPAT_LOOPBACK,  GETCURRENTPADDING_INVALID);
    RunAudioClientTest(RENDER_EXCLUSIVE,        GETCURRENTPADDING_INVALID);


    //
    // Capture
    //
    RunAudioClientTest(CAPTURE_SHARED,          GETCURRENTPADDING_INVALID);
    RunAudioClientTest(CAPTURE_COMPAT,          GETCURRENTPADDING_INVALID);
    RunAudioClientTest(CAPTURE_EXCLUSIVE,       GETCURRENTPADDING_INVALID);

    return FNS_PASS;
}

DWORD TC_AudioClient_DisconnectService_Invalid()
{
    //
    // Render
    //
    RunAudioClientTest(RENDER_SHARED,           DISCONNECTSERVICE_INVALID);
    RunAudioClientTest(RENDER_COMPAT,           DISCONNECTSERVICE_INVALID);
    RunAudioClientTest(RENDER_SHARED_LOOPBACK,  DISCONNECTSERVICE_INVALID);
    RunAudioClientTest(RENDER_COMPAT_LOOPBACK,  DISCONNECTSERVICE_INVALID);
    RunAudioClientTest(RENDER_EXCLUSIVE,        DISCONNECTSERVICE_INVALID);


    //
    // Capture
    //
    RunAudioClientTest(CAPTURE_SHARED,          DISCONNECTSERVICE_INVALID);
    RunAudioClientTest(CAPTURE_COMPAT,          DISCONNECTSERVICE_INVALID);
    RunAudioClientTest(CAPTURE_EXCLUSIVE,       DISCONNECTSERVICE_INVALID);

    return FNS_PASS;
}

DWORD TC_AudioClient_QueryInterface_Invalid()
{
    //
    // Render
    //
    RunAudioClientTest(RENDER_SHARED,           QUERYINTERFACE_INVALID);
    RunAudioClientTest(RENDER_COMPAT,           QUERYINTERFACE_INVALID);
    RunAudioClientTest(RENDER_SHARED_LOOPBACK,  QUERYINTERFACE_INVALID);
    RunAudioClientTest(RENDER_COMPAT_LOOPBACK,  QUERYINTERFACE_INVALID);
    RunAudioClientTest(RENDER_EXCLUSIVE,        QUERYINTERFACE_INVALID);


    //
    // Capture
    //
    RunAudioClientTest(CAPTURE_SHARED,          QUERYINTERFACE_INVALID);
    RunAudioClientTest(CAPTURE_COMPAT,          QUERYINTERFACE_INVALID);
    RunAudioClientTest(CAPTURE_EXCLUSIVE,       QUERYINTERFACE_INVALID);

    return FNS_PASS;
}

DWORD TC_AudioClient_GetMixFormat_Invalid()
{
    //
    // Render
    //
    RunAudioClientTest(RENDER_SHARED,           GETMIXFORMAT_INVALID);
    RunAudioClientTest(RENDER_COMPAT,           GETMIXFORMAT_INVALID);
    RunAudioClientTest(RENDER_SHARED_LOOPBACK,  GETMIXFORMAT_INVALID);
    RunAudioClientTest(RENDER_COMPAT_LOOPBACK,  GETMIXFORMAT_INVALID);
    RunAudioClientTest(RENDER_EXCLUSIVE,        GETMIXFORMAT_INVALID);


    //
    // Capture
    //
    RunAudioClientTest(CAPTURE_SHARED,          GETMIXFORMAT_INVALID);
    RunAudioClientTest(CAPTURE_COMPAT,          GETMIXFORMAT_INVALID);
    RunAudioClientTest(CAPTURE_EXCLUSIVE,       GETMIXFORMAT_INVALID);

    return FNS_PASS;
}

DWORD TC_AudioClient_SetEventHandle_Invalid()
{
    //
    // Render
    //
    RunAudioClientTest(RENDER_SHARED,           SETEVENTHANDLE_INVALID);
    RunAudioClientTest(RENDER_COMPAT,           SETEVENTHANDLE_INVALID);
    RunAudioClientTest(RENDER_SHARED_LOOPBACK,  SETEVENTHANDLE_INVALID);
    RunAudioClientTest(RENDER_COMPAT_LOOPBACK,  SETEVENTHANDLE_INVALID);
    RunAudioClientTest(RENDER_EXCLUSIVE,        SETEVENTHANDLE_INVALID);


    //
    // Capture
    //
    RunAudioClientTest(CAPTURE_SHARED,          SETEVENTHANDLE_INVALID);
    RunAudioClientTest(CAPTURE_COMPAT,          SETEVENTHANDLE_INVALID);
    RunAudioClientTest(CAPTURE_EXCLUSIVE,       SETEVENTHANDLE_INVALID);

    return FNS_PASS;
}

DWORD TC_AudioClient_GetService_Valid()
{
    //
    // Render
    //
    RunAudioClientTest(RENDER_SHARED,           GETSERVICE_VALID);
    RunAudioClientTest(RENDER_COMPAT,           GETSERVICE_VALID);
    RunAudioClientTest(RENDER_SHARED_LOOPBACK,  GETSERVICE_VALID);
    RunAudioClientTest(RENDER_COMPAT_LOOPBACK,  GETSERVICE_VALID);
    RunAudioClientTest(RENDER_EXCLUSIVE,        GETSERVICE_VALID);


    //
    // Capture
    //
    RunAudioClientTest(CAPTURE_SHARED,          GETSERVICE_VALID);
    RunAudioClientTest(CAPTURE_COMPAT,          GETSERVICE_VALID);
    RunAudioClientTest(CAPTURE_EXCLUSIVE,       GETSERVICE_VALID);

    return FNS_PASS;
}

DWORD TC_AudioClient_GetBufferSize_Valid()
{
    //
    // Render
    //
    RunAudioClientTest(RENDER_SHARED,           GETBUFFERSIZE_VALID);
    RunAudioClientTest(RENDER_COMPAT,           GETBUFFERSIZE_VALID);
    RunAudioClientTest(RENDER_SHARED_LOOPBACK,  GETBUFFERSIZE_VALID);
    RunAudioClientTest(RENDER_COMPAT_LOOPBACK,  GETBUFFERSIZE_VALID);
    RunAudioClientTest(RENDER_EXCLUSIVE,        GETBUFFERSIZE_VALID);


    //
    // Capture
    //
    RunAudioClientTest(CAPTURE_SHARED,          GETBUFFERSIZE_VALID);
    RunAudioClientTest(CAPTURE_COMPAT,          GETBUFFERSIZE_VALID);
    RunAudioClientTest(CAPTURE_EXCLUSIVE,       GETBUFFERSIZE_VALID);

    return FNS_PASS;
}

DWORD TC_AudioClient_GetStreamLatency_Valid()
{
    //
    // Render
    //
    RunAudioClientTest(RENDER_SHARED,           GETSTREAMLATENCY_VALID);
    RunAudioClientTest(RENDER_COMPAT,           GETSTREAMLATENCY_VALID);
    RunAudioClientTest(RENDER_SHARED_LOOPBACK,  GETSTREAMLATENCY_VALID);
    RunAudioClientTest(RENDER_COMPAT_LOOPBACK,  GETSTREAMLATENCY_VALID);
    RunAudioClientTest(RENDER_EXCLUSIVE,        GETSTREAMLATENCY_VALID);


    //
    // Capture
    //
    RunAudioClientTest(CAPTURE_SHARED,          GETSTREAMLATENCY_VALID);
    RunAudioClientTest(CAPTURE_COMPAT,          GETSTREAMLATENCY_VALID);
    RunAudioClientTest(CAPTURE_EXCLUSIVE,       GETSTREAMLATENCY_VALID);

    return FNS_PASS;
}

DWORD TC_AudioClient_SetEventHandle_Valid()
{
    //
    // Render
    //
    RunAudioClientTest(RENDER_SHARED,           SETEVENTHANDLE_VALID);
    RunAudioClientTest(RENDER_COMPAT,           SETEVENTHANDLE_VALID);
    RunAudioClientTest(RENDER_SHARED_LOOPBACK,  SETEVENTHANDLE_VALID);
    RunAudioClientTest(RENDER_COMPAT_LOOPBACK,  SETEVENTHANDLE_VALID);
    RunAudioClientTest(RENDER_EXCLUSIVE,        SETEVENTHANDLE_VALID);

    //
    // Capture
    //
    RunAudioClientTest(CAPTURE_SHARED,          SETEVENTHANDLE_VALID);
    RunAudioClientTest(CAPTURE_COMPAT,          SETEVENTHANDLE_VALID);
    RunAudioClientTest(CAPTURE_EXCLUSIVE,       SETEVENTHANDLE_VALID);

    return FNS_PASS;
}

DWORD TC_AudioRenderClient_GetBuffer_Invalid()
{
    //
    // Render
    //
    RunAudioClientTest(RENDER_SHARED,           RENDER_GETBUFFER_INVALID);
    RunAudioClientTest(RENDER_COMPAT,           RENDER_GETBUFFER_INVALID);
    RunAudioClientTest(RENDER_EXCLUSIVE,        RENDER_GETBUFFER_INVALID);

    return FNS_PASS;
}

DWORD TC_AudioRenderClient_GetBuffer_Hacker_Invalid()
{
    //
    // Render
    //
    RunAudioClientTest(RENDER_SHARED,           RENDER_GETBUFFERHACKER_INVALID);
    RunAudioClientTest(RENDER_COMPAT,           RENDER_GETBUFFERHACKER_INVALID);

    return FNS_PASS;
}

DWORD TC_AudioRenderClient_ReleaseBuffer_Invalid()
{
    //
    // Render
    //
    RunAudioClientTest(RENDER_SHARED,           RENDER_RELEASEBUFFER_INVALID);
    RunAudioClientTest(RENDER_COMPAT,           RENDER_RELEASEBUFFER_INVALID);
    RunAudioClientTest(RENDER_EXCLUSIVE,        RENDER_RELEASEBUFFER_INVALID);

    return FNS_PASS;
}

DWORD TC_AudioCaptureClient_GetBuffer_Invalid()
{
    //
    // Loopback
    //
    RunAudioClientTest(RENDER_SHARED_LOOPBACK,  CAPTURE_GETBUFFER_INVALID);
    RunAudioClientTest(RENDER_COMPAT_LOOPBACK,  CAPTURE_GETBUFFER_INVALID);

    //
    // Capture
    //
    RunAudioClientTest(CAPTURE_SHARED,          CAPTURE_GETBUFFER_INVALID);
    RunAudioClientTest(CAPTURE_COMPAT,          CAPTURE_GETBUFFER_INVALID);
    RunAudioClientTest(CAPTURE_EXCLUSIVE,       CAPTURE_GETBUFFER_INVALID);

    return FNS_PASS;
}

DWORD TC_AudioCaptureClient_GetBuffer_Hacker_Invalid()
{
    //
    // Render
    //
    RunAudioClientTest(RENDER_SHARED_LOOPBACK,  CAPTURE_GETBUFFERHACKER_INVALID);
    RunAudioClientTest(RENDER_COMPAT_LOOPBACK,  CAPTURE_GETBUFFERHACKER_INVALID);

    //
    // Capture
    //
    RunAudioClientTest(CAPTURE_SHARED,          CAPTURE_GETBUFFERHACKER_INVALID);
    RunAudioClientTest(CAPTURE_COMPAT,          CAPTURE_GETBUFFERHACKER_INVALID);

    return FNS_PASS;
}

DWORD TC_AudioCaptureClient_ReleaseBuffer_Invalid()
{
    //
    // Render
    //
    RunAudioClientTest(RENDER_SHARED_LOOPBACK,  CAPTURE_RELEASEBUFFER_INVALID);
    RunAudioClientTest(RENDER_COMPAT_LOOPBACK,  CAPTURE_RELEASEBUFFER_INVALID);

    //
    // Capture
    //
    RunAudioClientTest(CAPTURE_SHARED,          CAPTURE_RELEASEBUFFER_INVALID);
    RunAudioClientTest(CAPTURE_COMPAT,          CAPTURE_RELEASEBUFFER_INVALID);
    RunAudioClientTest(CAPTURE_EXCLUSIVE,       CAPTURE_RELEASEBUFFER_INVALID);

    return FNS_PASS;
}

DWORD TC_AudioClock_GetPosition_Invalid()
{
    //
    // Render
    //
    RunAudioClientTest(RENDER_SHARED,           CLOCK_GETPOSITION_INVALID);
    RunAudioClientTest(RENDER_COMPAT,           CLOCK_GETPOSITION_INVALID);
    RunAudioClientTest(RENDER_SHARED_LOOPBACK,  CLOCK_GETPOSITION_INVALID);
    RunAudioClientTest(RENDER_COMPAT_LOOPBACK,  CLOCK_GETPOSITION_INVALID);
    RunAudioClientTest(RENDER_EXCLUSIVE,        CLOCK_GETPOSITION_INVALID);


    //
    // Capture
    //
    RunAudioClientTest(CAPTURE_SHARED,          CLOCK_GETPOSITION_INVALID);
    RunAudioClientTest(CAPTURE_COMPAT,          CLOCK_GETPOSITION_INVALID);
    RunAudioClientTest(CAPTURE_EXCLUSIVE,       CLOCK_GETPOSITION_INVALID);

    return FNS_PASS;
}

DWORD TC_AudioClock_GetFrequency_Invalid()
{
    //
    // Render
    //
    RunAudioClientTest(RENDER_SHARED,           CLOCK_GETFREQUENCY_INVALID);
    RunAudioClientTest(RENDER_COMPAT,           CLOCK_GETFREQUENCY_INVALID);
    RunAudioClientTest(RENDER_SHARED_LOOPBACK,  CLOCK_GETFREQUENCY_INVALID);
    RunAudioClientTest(RENDER_COMPAT_LOOPBACK,  CLOCK_GETFREQUENCY_INVALID);
    RunAudioClientTest(RENDER_EXCLUSIVE,        CLOCK_GETFREQUENCY_INVALID);


    //
    // Capture
    //
    RunAudioClientTest(CAPTURE_SHARED,          CLOCK_GETFREQUENCY_INVALID);
    RunAudioClientTest(CAPTURE_COMPAT,          CLOCK_GETFREQUENCY_INVALID);
    RunAudioClientTest(CAPTURE_EXCLUSIVE,       CLOCK_GETFREQUENCY_INVALID);

    return FNS_PASS;
}

DWORD TC_AudioClock_GetCharacteristics_Invalid()
{
    //
    // Render
    //
    RunAudioClientTest(RENDER_SHARED,           CLOCK_GETCHARACTERISTICS_INVALID);
    RunAudioClientTest(RENDER_COMPAT,           CLOCK_GETCHARACTERISTICS_INVALID);
    RunAudioClientTest(RENDER_SHARED_LOOPBACK,  CLOCK_GETCHARACTERISTICS_INVALID);
    RunAudioClientTest(RENDER_COMPAT_LOOPBACK,  CLOCK_GETCHARACTERISTICS_INVALID);
    RunAudioClientTest(RENDER_EXCLUSIVE,        CLOCK_GETCHARACTERISTICS_INVALID);


    //
    // Capture
    //
    RunAudioClientTest(CAPTURE_SHARED,          CLOCK_GETCHARACTERISTICS_INVALID);
    RunAudioClientTest(CAPTURE_COMPAT,          CLOCK_GETCHARACTERISTICS_INVALID);
    RunAudioClientTest(CAPTURE_EXCLUSIVE,       CLOCK_GETCHARACTERISTICS_INVALID);

    return FNS_PASS;
}

HRESULT CAudioClientApiTest::GetAudioClient(IAudioClient** ppAudioClient)
{
    HRESULT hr = S_OK;
    CComPtr<IMMDevice> spEndpointUnderTest;
    LPCWSTR wszDeviceUnderTest = NULL; 
    IAudioClient* pAudioClient = NULL;

    if(NULL == ppAudioClient)
    {
        hr = E_INVALIDARG;
        goto END;
    }
            
    wszDeviceUnderTest = GetEndpointIDUnderTest();
    if(NULL == wszDeviceUnderTest)
    {
        SLOG(eFatalError,
             "FAIL: Unable to obtain the ID of the endpoint under test");
        hr = E_FAIL;
        goto END;
    }

    hr = GetEndpointUnderTest(&spEndpointUnderTest);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eFatalError, 
             "Error calling GetEndpointUnderTest (0x%8x)",
             hr);
        goto END;
    }

    hr = spEndpointUnderTest->Activate(__uuidof(IAudioClient),
                                       CLSCTX_ALL,
                                       NULL,
                                       (void**)&pAudioClient);
    if(FAILED(hr))
    {
        SLOG(eFatalError, 
             "FAIL: Error calling IMMDevice::Activate(IAudioClient) (0x%8x)",
             hr);
        goto END;
    }

    *ppAudioClient = pAudioClient;

END:
    return hr;
}

HRESULT CAudioClientApiTest::InitializeAudioClient(IAudioClient** ppAudioClient,
                                                   WAVEFORMATEX** ppwfxStreamingFormat)
{
    HRESULT hr = S_OK;
    WAVEFORMATEX* pwfxStreamingFormat = NULL;

    if((NULL == ppAudioClient)||(NULL == *ppAudioClient))
    {
        SLOG(eFatalError, "FAIL: Cannot initialize NULL IAudioClient interface!");
        hr = E_POINTER;
        goto END;
    }

    // In exclusive mode, we use the default device format
    // In all others, we use the mix format.
    if (AUDCLNT_SHAREMODE_EXCLUSIVE == m_nShareMode)
    {
        pwfxStreamingFormat = GetPreferredDeviceFormatUnderTest();
    }
    else
    {
        pwfxStreamingFormat = GetMixFormatUnderTest();

    }

    if(NULL == pwfxStreamingFormat)
    {
        XLOG(XFAIL,
             eFatalError, 
             "FAIL: Unable to obtain a suitable streaming format!");
        hr = E_FAIL;
        goto END;
    }
    
    METHODCHECKRESULT("IAudioClient::Initialize",
                      hr,
                      (*ppAudioClient)->Initialize(m_nShareMode,
                                                   m_dwRequestedStreamFlags,
                                                   m_hnsBufferDuration,
                                                   m_hnsPeriodicity,
                                                   pwfxStreamingFormat,
                                                   NULL));
    if (AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED == hr)
    {
        // this error can only be returned in exclusive pull mode
        // (render or capture)
        if((AUDCLNT_SHAREMODE_EXCLUSIVE != m_nShareMode) ||
            (0 == (AUDCLNT_STREAMFLAGS_EVENTCALLBACK & m_dwRequestedStreamFlags)))
        {
            XLOG(XFAIL,
                 eFatalError,
                 "FAIL: IAudioClient::Initialize returned AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED"
                 " but share mode was %u and stream flags were 0x%08x -"
                 " AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED should only be returned"
                 " if share mode is AUDCLNT_SHAREMODE_EXCLUSIVE (%u) and"
                 " the AUDCLNT_STREAMFLAGS_EVENTCALLBACK flag (0x%08x) is set",
                 m_nShareMode, m_dwRequestedStreamFlags,
                 AUDCLNT_SHAREMODE_EXCLUSIVE,
                 AUDCLNT_STREAMFLAGS_EVENTCALLBACK
                );
            goto END;
        }

        bool bPeriodicityEqualsBufferSize = (m_hnsPeriodicity == m_hnsBufferDuration);
        UINT32 nAlignedBufferSizeInFrames = 0;

        // We're in exclusive mode.  Try an aligned buffer.
        SLOG(eInfo1,
             "IAudioClient::Initialize returned AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED. "
             "Unaligned buffer size is %I64u hns.",
             m_hnsBufferDuration
             );

        // normally you can't call methods on an IAudioClient if initialize fails.
        // but if the failure code is AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED,
        // it's OK to call GetBufferSize() only.
        METHODCHECKRESULT("IAudioClient::GetBufferSize",
                          hr,
                          (*ppAudioClient)->GetBufferSize((UINT32*)(&nAlignedBufferSizeInFrames)));
        if(FAILED(hr))
        {
            goto END;
        }
        
        // this IAudioClient is no good - throw it away
        SAFE_RELEASE_POINTER(*ppAudioClient);

        // convert aligned size from frames to hns
        // hns = (hns / ms) * (ms / sec) / (frames / sec) * frames
        m_hnsBufferDuration =
            (REFERENCE_TIME)(
                (10000.0 * 1000
                    / pwfxStreamingFormat->nSamplesPerSec
                    * nAlignedBufferSizeInFrames) + 0.5);
        SLOG(eInfo1, "Aligned buffer size is %u frames, %I64u hns.", nAlignedBufferSizeInFrames, m_hnsBufferDuration);

        // if the periodicity was equal to the buffer size, update that too
        if(bPeriodicityEqualsBufferSize)
        {
            m_hnsPeriodicity = m_hnsBufferDuration;
        }

        // now get a new IAudioClient
        METHODCHECKRESULT("IMMDevice::Activate(IAudioClient)",
                          hr,
                          GetAudioClient(ppAudioClient));
        if(FAILED(hr))
        {
            goto END;
        }
        
        // attempt to initialize IAudioClient
        METHODCHECKRESULT("IAudioClient::Initialize, this time with an aligned buffer size",
                          hr,
                          (*ppAudioClient)->Initialize(m_nShareMode,
                                                       m_dwRequestedStreamFlags,
                                                       m_hnsBufferDuration,
                                                       m_hnsPeriodicity,
                                                       pwfxStreamingFormat,
                                                       NULL));
        if(FAILED(hr))
        {
            goto END;
        }
    }
    else if(FAILED(hr))
    {
        XLOG(XFAIL,
             eFatalError, 
             "FAIL: Error calling IAudioClient::Initialize");
        goto END;
    }

    if (AUDIOCLIENTPROPERTY_OFFLOAD == (AUDIOCLIENTPROPERTY_OFFLOAD & m_dwClientProperties))
    {
        CComPtr<IAudioClient2>      pIAudioClient2 = NULL;
        BOOL                        bOffloadCapable;
        
        //  At this point we have a valid IAudioClient object...
        
        hr = (*ppAudioClient)->QueryInterface(__uuidof(IAudioClient2), (void**)&pIAudioClient2);
        if (FAILED(hr))
        {
            XLOG(XFAIL,
                 eFatalError, 
                 "FAIL: Error calling IAudioClient::QueryInterface for IAudioClient2");
            goto END;
        }
        
        //  Do we support Offload capability?
        hr = pIAudioClient2->IsOffloadCapable(AudioCategory_Media, &bOffloadCapable);
        if (FAILED(hr))
        {
            XLOG(XFAIL,
                 eFatalError, 
                 "FAIL: Error calling IAudioClient2::IsOffloadCapable");
            goto END;
        }
        
        if (bOffloadCapable)
        {
            //  Woo Hoo!  Let's set the property...
            AudioClientProperties   acp = {0};
            
            acp.cbSize     = sizeof(acp);
            acp.bIsOffload = TRUE;
            acp.eCategory  = AudioCategory_Media;
            
            hr = pIAudioClient2->SetClientProperties(&acp);
            if (FAILED(hr))
            {
                XLOG(XFAIL,
                     eFatalError, 
                     "FAIL: Error calling IAudioClient2::SetClientProperties");
                goto END;
            }
        }
        else
        {
            //  Trying to offload on an endpoint that doesn't support it...
            //  Should we log a warning here?!
            XLOG(XWARN,
                 eInfo1, 
                 "WARN: Requesting offload streaming on endpoint which doesn't support it.");
        }
    }

    if(NULL != ppwfxStreamingFormat)
    {
        *ppwfxStreamingFormat = pwfxStreamingFormat;
    }
    else
    {
        CoTaskMemFree(pwfxStreamingFormat);
    }

    pwfxStreamingFormat = NULL;

END:
    SAFE_COTASKMEMFREE(pwfxStreamingFormat);
    return hr;
}

CAudioClientApiTest::CAudioClientApiTest(AudioClientType nClientType, 
                                         AUDCLNT_SHAREMODE nShareMode, 
                                         DWORD dwRequestedStreamFlags,
                                         HNSTIME hnsBufferDuration,
                                         HNSTIME hnsPeriodicity)
{
    m_nClientType = nClientType;
    m_nShareMode = nShareMode;
    m_dwRequestedStreamFlags = dwRequestedStreamFlags;
    m_dwClientProperties = 0L;
    m_hnsBufferDuration = hnsBufferDuration;
    m_hnsPeriodicity = hnsPeriodicity;
}

//  New constructor to account for properties through IAudioClient2::SetClientProperties()
CAudioClientApiTest::CAudioClientApiTest(AudioClientType nClientType, 
                                         AUDCLNT_SHAREMODE nShareMode, 
                                         DWORD dwRequestedStreamFlags,
                                         DWORD dwClientProperties,
                                         HNSTIME hnsBufferDuration,
                                         HNSTIME hnsPeriodicity)
{
    m_nClientType = nClientType;
    m_nShareMode = nShareMode;
    m_dwRequestedStreamFlags = dwRequestedStreamFlags;
    m_dwClientProperties = dwClientProperties;
    m_hnsBufferDuration = hnsBufferDuration;
    m_hnsPeriodicity = hnsPeriodicity;
}

LPCTSTR CAudioClientApiTest::GetMode(DWORD dwMode)
{
    switch(dwMode)
    {
        case AUDCLNT_SHAREMODE_EXCLUSIVE:
            return TEXT("EXCLUSIVE");

        case AUDCLNT_SHAREMODE_SHARED:
            return TEXT("SHARED");            
        default:
            return TEXT("N/A");
    }
}

LPCTSTR CAudioClientApiTest::GetFlags(DWORD dwStreamFlags)
{
    switch(dwStreamFlags)
    {
        case AUDCLNT_STREAMFLAGS_CROSSPROCESS:
            return TEXT("CROSSPROCESS");
        case AUDCLNT_STREAMFLAGS_LOOPBACK:
            return TEXT("LOOPBACK");
        case AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM:
            return TEXT("AUTOCONVERTPCM");
        case AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY:
            return TEXT("SRC_DEFAULT_QUALITY");
        default:
            return TEXT("");
    }
}

LPCTSTR CAudioClientApiTest::GetType(AudioClientType type) {
    switch ( type ) 
    {
        case AUDIO_CLIENT_RENDER:
            return TEXT("RENDER");
        case AUDIO_CLIENT_CAPTURE:
            return TEXT("CAPTURE");
        case AUDIO_CLIENT_LOOPBACK:
            return TEXT("LOOPBACK");
        default:
            return TEXT("N/A");
    }
}

DWORD CAudioClientApiTest::IsFormatSupported_InvalidParamsTest()
{
    HRESULT hr     = S_OK;
    CComPtr<IAudioClient> spAudioClient = NULL;
    WAVEFORMATEX* pwfxDeviceFormat = NULL;
    WAVEFORMATEX* pwfxMixFormat = NULL;
    WAVEFORMATEX* pwfxDummyFormat = NULL;

    // get an IAudioClient interface
    hr = GetAudioClient(&spAudioClient);
    if(FAILED(hr))
    {
        goto END;
    }

    pwfxDeviceFormat = GetPreferredDeviceFormatUnderTest();
    if(NULL == pwfxDeviceFormat)
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Unable to obtain the preferred device format for the endpoint under test.");
             
        goto END;
    }

    pwfxMixFormat = GetMixFormatUnderTest();
    if(NULL == pwfxMixFormat)
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Unable to obtain the mix format for the endpoint under test.");
             
        goto END;
    }


    // IsFormatSupported should work before initialize
    if (GetDeviceUnderTestAllowsExclusiveMode())
    {
        METHODCHECKRESULT("IAudioClient::IsFormatSupported",
                          hr, 
                          spAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_EXCLUSIVE, 
                                                           pwfxDeviceFormat, 
                                                           &pwfxDummyFormat));
        if ( S_OK != hr ) 
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: IsFormatSupported != S_OK the device format!");
        }

        SAFE_COTASKMEMFREE(pwfxDummyFormat);
    }

    // IsFormatSupported should work before initialize
    METHODCHECKRESULT("IAudioClient::IsFormatSupported",
                      hr, 
                      spAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, 
                                                       pwfxMixFormat, 
                                                       &pwfxDummyFormat));
    if ( S_OK != hr ) 
    {
        XLOG(XFAIL,
             eError,
             "FAIL: IsFormatSupported != S_OK for the mix format!");
    }


    // IsFormatSupported should not work when no suggested format is specified
    METHODCHECKRESULT("IAudioClient::IsFormatSupported",
                      hr, 
                      spAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, 
                                                       pwfxMixFormat, 
                                                       NULL));
    if ( SUCCEEDED(hr) ) 
    {
        XLOG(XFAIL,
             eError,
             "FAIL: IsFormatSupported (SHARED) should not succeed with ppSuggestedFormat == NULL");
    }

    // invalid params test cases before initialize
    UINT32 nCount = ISFORMATSUPPORTED_PARAM_SPACE::getInvalidParamCasesCount();
    for (UINT i=0; i < nCount; i++)
    {
        hr = Method_IsFormatSupported(ISFORMATSUPPORTED_PARAM_SPACE::getInvalidParamCase(i),
                                      spAudioClient);
        if ( S_OK == hr ) 
        { 
            XLOG(XFAIL, 
                 eError,
                 "FAIL: call should have failed 0x%X", 
                 hr); 
        } 
    }

    SLOG(eError," >>WAVEFORMATEX invalid test cases<< ");
    // invalid values test cases before initialize wave
    nCount = ISFORMATSUPPORTED_PARAM_SPACE::getInvalidCasesCount();
    for (UINT i=0; i < nCount; i++)
    {
        hr = Method_IsFormatSupported(ISFORMATSUPPORTED_PARAM_SPACE::getInvalidCase(i),
                                      spAudioClient);
        if ( S_OK == hr ) 
        { 
            XLOG(XFAIL, eError,"FAIL: call should have failed 0x%X", hr); 
        } 
    }

    SLOG(eError," >>WAVEFORMATEXTENSIBLE invalid test cases<< ");
    // invalid values test cases before initialize wave
    nCount = ISFORMATSUPPORTED_PARAM_SPACE::getInvalidCasesExtCount();
    for (UINT i=0; i < nCount; i++)
    {
        hr = Method_IsFormatSupported(ISFORMATSUPPORTED_PARAM_SPACE::getInvalidCaseExt(i),
                                      spAudioClient);
        if ( S_OK == hr ) 
        { 
            XLOG(XFAIL,
                 eError,
                 "FAIL: call should have failed 0x%X", 
                 hr); 
        } 
    }

END:
    SAFE_COTASKMEMFREE(pwfxDeviceFormat);
    SAFE_COTASKMEMFREE(pwfxMixFormat);
    SAFE_COTASKMEMFREE(pwfxDummyFormat);
    
    return FNS_PASS;
}


DWORD CAudioClientApiTest::Initialize_InvalidParamsTest()
{
    HRESULT hr = S_OK;
    AudioClientProperties clientProperties = {0};

    WAVEFORMATEX* pwfxMixFormat = NULL;
    
    CComPtr<IMMDevice> spEndpointUnderTest = NULL;
    CComPtr<IAudioClient> spAudioClient = NULL;
    CComPtr<ITestStreamingEngine> spCommonCaptureEngine;
    CComPtr<ITestStreamingEngine> spCommonRenderEngine;
    CComPtr<ICaptureTestStreamingEngine> spCaptureEngine;
    CComPtr<IRenderTestStreamingEngine> spRenderEngine;
    LPCWSTR pwszDeviceUnderTest = GetEndpointIDUnderTest();

    clientProperties.bIsOffload = FALSE;
    clientProperties.eCategory = AudioCategory_Other;
    clientProperties.cbSize = sizeof(clientProperties);

    // Get an IAudioClient Interface
    if(NULL == pwszDeviceUnderTest)
    {
        XLOG(XFAIL,
             eFatalError,
             "FAIL: Unable to obtain the ID of the endpoint under test");
        goto END;
    }

    hr = GetEndpointUnderTest(&spEndpointUnderTest);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eFatalError, 
             "Error calling GetEndpointUnderTest (0x%8x)",
             hr);
        goto END;
    }

    // Get the mix format for the endpoint under test
    pwfxMixFormat = GetMixFormatUnderTest();
    if(NULL == pwfxMixFormat)
    {
        XLOG(XFAIL,
             eFatalError,
             "FAIL: Unable to obtain the mix format for the endpoint under test!");

        goto END;
    }


    hr = spEndpointUnderTest->Activate(__uuidof(IAudioClient),
                                       CLSCTX_ALL,
                                       NULL,
                                       (void**)&spAudioClient);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eFatalError, 
             "FAIL: Error calling IMMDevice::Activate(IAudioClient) (0x%8x)",
             hr);
        goto END;
    }
    
    //
    // Verify that initialize fails when the endpoint is already in use
    //
    if(DEVICE_TYPE_CAPTURE == GetDeviceTypeUnderTest())
    {
        //
        // Shared Mode Capture
        //
        
        // create a capture streaming engine
        METHODCHECKRESULT("CreateTestStreamingEngineOnEndpointId(ICaptureTestStreamingEngine)",
                          hr,
                          CreateTestStreamingEngineOnEndpointId(IID_ICaptureTestStreamingEngine,
                                                    pwszDeviceUnderTest,
                                                    TIMER_PUSH_CAPTURE,
                                                    GetBasicLogger(),
                                                    (void**)(&spCaptureEngine)));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: CreateTestStreamingEngineOnEndpointId failed unexpectedly");
            goto END;
        }

        METHODCHECKRESULT("ICaptureTestStreamingEngine::QueryInterface(ITestStreamingEngine)",
                          hr,
                          spCaptureEngine->QueryInterface(IID_ITestStreamingEngine,
                                                          (void**)(&spCommonCaptureEngine)));

        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: ICaptureTestStreamingEngine::QueryInterface failed unexpectedly");
            goto END;
        }
        
        if (GetDeviceUnderTestAllowsExclusiveMode())
        {
            METHODCHECKRESULT("ITestStreamingEngine::SetRequestedShareMode",
                              hr,
                              spCommonCaptureEngine->SetRequestedShareMode(AUDCLNT_SHAREMODE_EXCLUSIVE));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: ITestStreamingEngine::SetRequestedShareMode failed unexpectedly");
                goto END;
            }

            METHODCHECKRESULT("ITestStreamingEngine::Initialize",
                              hr,
                              spCommonCaptureEngine->Initialize(&clientProperties));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: ITestStreamingEngine::Initialize failed unexpectedly");
                goto END;
            }

            // now try to initialize on this endpoint
            METHODCHECKRESULT("IAudioClient::Initialize",
                              hr,
                              spAudioClient->Initialize(m_nShareMode,
                                                        m_dwRequestedStreamFlags,
                                                        0,
                                                        0,
                                                        pwfxMixFormat,
                                                        NULL));
            if(SUCCEEDED(hr))
            {
                XLOG(XFAIL,
                     eFatalError,
                     "FAIL: IAudioClient::Initialize should not succeed while the endpoint is busy");
            }
        }
        
        // Uninitialize the audio client
        spCommonCaptureEngine->Reset();
    }
    else if(DEVICE_TYPE_RENDER == GetDeviceTypeUnderTest())
    {
        //
        // Shared Mode Capture
        //
        
        // create a capture streaming engine
        METHODCHECKRESULT("CreateTestStreamingEngineOnEndpointId(IRenderTestStreamingEngine)",
                          hr,
                          CreateTestStreamingEngineOnEndpointId(IID_IRenderTestStreamingEngine,
                                                    pwszDeviceUnderTest,
                                                    TIMER_PUSH_RENDER,
                                                    GetBasicLogger(),
                                                    (void**)(&spRenderEngine)));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: CreateTestStreamingEngineOnEndpointId failed unexpectedly");
            goto END;
        }

        METHODCHECKRESULT("IRenderTestStreamingEngine::QueryInterface(ITestStreamingEngine)",
                          hr,
                          spRenderEngine->QueryInterface(IID_ITestStreamingEngine,
                                                         (void**)(&spCommonRenderEngine)));

        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: IRenderTestStreamingEngine::QueryInterface failed unexpectedly");
            goto END;
        }
            
        if (GetDeviceUnderTestAllowsExclusiveMode())
        {
            METHODCHECKRESULT("ITestStreamingEngine::SetRequestedShareMode",
                              hr,
                              spCommonRenderEngine->SetRequestedShareMode(AUDCLNT_SHAREMODE_EXCLUSIVE));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: ITestStreamingEngine::SetRequestedShareMode failed unexpectedly");
                goto END;
            }

            METHODCHECKRESULT("ITestStreamingEngine::Initialize",
                              hr,
                              spCommonRenderEngine->Initialize(&clientProperties));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: ITestStreamingEngine::Initialize failed unexpectedly");
                goto END;
            }

            // now try to initialize on this endpoint
            METHODCHECKRESULT("IAudioClient::Initialize",
                              hr,
                              spAudioClient->Initialize(m_nShareMode,
                                                        m_dwRequestedStreamFlags,
                                                        0,
                                                        0,
                                                        pwfxMixFormat,
                                                        NULL));
            if(SUCCEEDED(hr))
            {
                XLOG(XFAIL,
                     eFatalError,
                     "FAIL: IAudioClient::Initialize should not succeed while the endpoint is busy");
            }
        }

        // Uninitialize the audio client
        spCommonRenderEngine->Reset();
    }

    // invalid params test cases
    UINT32 nCount = INITIALIZE_PARAM_SPACE::getInvalidParamCasesCount();
    for (UINT i=0; i < nCount; i++)
    {
        // get a fresh IAudioClient interface
        spAudioClient = NULL;
        hr = spEndpointUnderTest->Activate(__uuidof(IAudioClient),
                                           CLSCTX_ALL,
                                           NULL,
                                           (void**)&spAudioClient);
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eFatalError, 
                 "FAIL: Error calling IMMDevice::Activate(IAudioClient) (0x%8x)",
                 hr);
            goto END;
        }

        INITIALIZE_PARAM_SPACE::INITIALIZE_PARAMS  params = INITIALIZE_PARAM_SPACE::getInvalidParamCase(i);
        // check that the test case applies
        if ( params.Type == m_nClientType&&
             params.Mode == m_nShareMode&&
             params.StreamFlags == m_dwRequestedStreamFlags)    
         {
            hr = Method_Initialize(params,
                                   spAudioClient);

            if ( SUCCEEDED(hr) ) 
            { 
                XLOG(XFAIL,
                     eError,
                     "FAIL: call should have failed 0x%X", 
                     hr); 
            } 
        }
    }

    SLOG(eError," >>WAVEFORMATEX invalid test cases<< ");
    // invalid values test cases
    nCount = INITIALIZE_PARAM_SPACE::getInvalidCasesCount();
    for (UINT i=0; i < nCount; i++)
    {
        // get a fresh IAudioClient interface
        spAudioClient = NULL;
        hr = spEndpointUnderTest->Activate(__uuidof(IAudioClient),
                                           CLSCTX_ALL,
                                           NULL,
                                           (void**)&spAudioClient);
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eFatalError, 
                 "FAIL: Error calling IMMDevice::Activate(IAudioClient) (0x%8x)",
                 hr);
            goto END;
        }

        hr = Method_Initialize(INITIALIZE_PARAM_SPACE::getInvalidCase(i, 
                                                                      m_nClientType, 
                                                                      m_nShareMode, 
                                                                      m_dwRequestedStreamFlags),
                                                                      spAudioClient);
        if ( SUCCEEDED(hr) ) 
        {   
            XLOG(XFAIL,
                 eError,
                 "FAIL: call should have failed 0x%X", 
                  hr); 
        } 

        if ( hr != E_INVALIDARG && hr != AUDCLNT_E_UNSUPPORTED_FORMAT ) 
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Error code is invalid for this call");
        }
    }

END:
    SAFE_COTASKMEMFREE(pwfxMixFormat);

    return FNS_PASS;
}

DWORD CAudioClientApiTest::GetBufferSize_InvalidParamsTest()
{
    HRESULT hr = S_OK;
    UINT32 u32Size = 0;
    CComPtr<IAudioClient> spAudioClient;
       
    //
    // Get an IAudioClient interface
    //
    hr = GetAudioClient(&spAudioClient);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioClient interface pointer (0x%8x)", hr);
        goto END;
    }

    // should not work before initialize
    METHODCHECKRESULT("IAudioClient::GetBufferSize", 
                      hr, 
                      spAudioClient->GetBufferSize( &u32Size ) );
    if ( SUCCEEDED(hr) ) {
        XLOG(XFAIL, eError, "FAIL: GetBufferSize should fail it not initialized");
    }

    // Initialize the IAudioClientInterface
    hr = InitializeAudioClient(&(spAudioClient.p));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error initializing IAudioClient (0x%8x)", hr);
        goto END;
    }

    // NULL param
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioClient::GetBufferSize",
                      hr,
                      spAudioClient->GetBufferSize( NULL ));
    if ( SUCCEEDED(hr) ) 
    {
        XLOG(XFAIL, eError, "FAIL: GetBufferSize succeeded for NULL output param");
    }

END:
    return FNS_PASS;
}


DWORD CAudioClientApiTest::GetStreamLatency_InvalidParamsTest()
{
    HRESULT hr = S_OK;
    CComPtr<IAudioClient> spAudioClient = NULL;
    REFERENCE_TIME Latency;
        
    //
    // Get an IAudioClient interface
    //
    hr = GetAudioClient(&(spAudioClient.p));
    {
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioClient interface pointer (0x%8x)", hr);
            goto END;
        }
    }
    

    // should not work before initialize
    METHODCHECKRESULT("IAudioClient::GetStreamLatency", 
                      hr, 
                      spAudioClient->GetStreamLatency(&Latency));
    if ( SUCCEEDED(hr) ) 
    {
        XLOG(XFAIL, eError,"FAIL: GetStreamLatency should fail IAudioClient is not initialized");
    }

    // initialize the IAudioClient interface
    hr = InitializeAudioClient(&(spAudioClient.p));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to initialize IAudioClient (0x%8x)", hr);
        goto END;
    }

    // NULL param
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioClient::GetStreamLatency",
                      hr,
                      spAudioClient->GetStreamLatency( NULL ));
    if ( SUCCEEDED(hr) ) 
    {
        XLOG(XFAIL, eError,"FAIL: GetStreamLatency succeeded for NULL output param");
    }

END:
    return FNS_PASS;
}

DWORD CAudioClientApiTest::GetCurrentPadding_InvalidParamsTest()
{
    HRESULT hr = S_OK;
    UINT32 u32Padding;

    CComPtr<IAudioClient> spAudioClient = NULL;    
        
    //
    // Get an IAudioClient interface
    //
    hr = GetAudioClient(&spAudioClient);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioClient interface pointer (0x%8x)", hr);
        goto END;
    }

    // should not work before initialize
    METHODCHECKRESULT("IAudioClient::GetCurrentPadding", 
                      hr, 
                      spAudioClient->GetCurrentPadding(&u32Padding));
    if ( SUCCEEDED(hr) ) 
    {
        XLOG(XFAIL, eError, "FAIL: GetCurrentPadding should fail it not initialized");
    }

    // Initialize the IAudioClient interface
    hr = InitializeAudioClient(&(spAudioClient.p));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to initialize IAudioClient (0x%8x)", hr);
        goto END;
    }


    // NULL param
#pragma prefast(suppress:6309, "Disable parameter checking")
    hr = spAudioClient->GetCurrentPadding( NULL );
    if ( SUCCEEDED(hr) ) 
    {
        XLOG(XFAIL, eError, "FAIL: GetCurrentPadding succeeded for NULL output param");
    }
   
END:
    return FNS_PASS;
}


DWORD CAudioClientApiTest::DisconnectService_InvalidParamsTest()
{
    HRESULT hr = S_OK;
    
    CComPtr<IAudioClient> spAudioClient = NULL;
    CComPtr<IAudioRenderClient> spRenderClient = NULL;
    CComPtr<IAudioCaptureClient> spCaptureClient = NULL;
    CComPtr<IAudioClock> spClock = NULL;
    
    LPWAVEFORMATEX pWfx = NULL;

    UINT32 u32Padding;
    REFERENCE_TIME Latency;
    UINT64 u64Position, u64QpcPosition;
        
    //
    // Get an IAudioClient interface
    //
    hr = GetAudioClient(&spAudioClient);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioClient interface pointer (0x%8x)", hr);
        goto END;
    }

    // Initialize the interface
    hr = InitializeAudioClient(&(spAudioClient.p));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to initialize IAudioClient (0x%8x)", hr);
        goto END;
    }


    if((AUDIO_CLIENT_CAPTURE == m_nClientType)||
       (AUDIO_CLIENT_LOOPBACK == m_nClientType))
    {
        // Get an IAudioCaptureClient Interface
        METHODCHECKRESULT("IAudioClient::GetService(IAudioCaptureClient)",
                          hr,
                          spAudioClient->GetService(__uuidof(IAudioCaptureClient),
                                                    (void**)(&spCaptureClient)));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioCaptureClient");
            goto END;
        }
    }
    else
    {
        // Get an IAudioRenderClient Interface
        METHODCHECKRESULT("IAudioClient::GetService(IAudioRenderClient)",
                          hr,
                          spAudioClient->GetService(__uuidof(IAudioRenderClient),
                                                    (void**)(&spRenderClient)));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioRenderClient");
            goto END;
        }
    }

    // Get an IAudioClock Interface
    METHODCHECKRESULT("IAudioClient::GetService(IAudioClock)",
                      hr,
                      spAudioClient->GetService(__uuidof(IAudioClock),
                                                (void**)(&spClock)));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioClock");
        goto END;
    }

    // Stop AudioSrv
    if(!StopAudioService())
    {
        XLOG(XFAIL, eError, "Unable to stop AudioSrv!");
        goto END;
    }
    

    // now call the methods that depend on the service
    METHODCHECKRESULT(TEXT("IAudioClient::GetMixFormat"), hr, spAudioClient->GetMixFormat( &pWfx ) );
    if ( SUCCEEDED( hr ) ) 
    {
        XLOG(XFAIL, eError,"FAIL: GetMixFormat worked even if the service is stopped");
    }


    METHODCHECKRESULT(TEXT("IAudioClient::GetCurrentPadding"), hr, spAudioClient->GetCurrentPadding( &u32Padding ) );
    if ( SUCCEEDED( hr ) ) 
    {
        XLOG(XFAIL, eError,"FAIL: GetCurrentPadding worked even if the service is stopped");
    }


    METHODCHECKRESULT(TEXT("IAudioClient::GetStreamLatency"), hr, spAudioClient->GetStreamLatency( &Latency ) );
    if ( SUCCEEDED( hr ) ) 
    {
        XLOG(XFAIL, eError,"FAIL: GetStreamLatency worked even if the service is stopped");
    }


    METHODCHECKRESULT(TEXT("IAudioClient::Start"), hr, spAudioClient->Start() );
    if ( SUCCEEDED( hr ) ) 
    {
        XLOG(XFAIL, eError,"FAIL: Start worked even if the service is stopped");
    }

    if ( NULL != spRenderClient ) 
    {
        LPVOID pData;
        METHODCHECKRESULT(TEXT("IAudioRenderClient::GetBuffer"), hr, spRenderClient->GetBuffer(1, (LPBYTE*)&pData) );
        if ( SUCCEEDED( hr ) ) 
        {
            XLOG(XFAIL, eError,"FAIL: GetBuffer worked even if the service is stopped");
        }
    }

    if ( NULL != spCaptureClient ) 
    {
        LPVOID pData;
        DWORD  dwFlags;
        UINT32 u32Frames;
        METHODCHECKRESULT(TEXT("IAudioCaptureClient::GetBuffer"), hr, spCaptureClient->GetBuffer((LPBYTE*)&pData, &u32Frames, &dwFlags, NULL, NULL) );
        if ( SUCCEEDED( hr ) ) 
        {
            XLOG(XFAIL, eError,"FAIL: GetBuffer worked even if the service is stopped");
        }
    }

    METHODCHECKRESULT(TEXT("IAudioClock::GetPosition"), hr, spClock->GetPosition(&u64Position, &u64QpcPosition) );
    if ( SUCCEEDED( hr ) ) 
    {
        XLOG(XFAIL, eError,"FAIL: GetPosition worked even if the service is stopped");
    }

END:
    // Start audiosrv back up. Moving this into end section so that even if this function fails, the following test cases do not fail.
    SLOG(eError,"Re-Starting audiosrv...");
    if(!StartAudioService())
    {
        XLOG(XFAIL, eFatalError, "FAIL: Unable to re-start AudioSrv! System may be in an unusable state!");
    }
    
    return FNS_PASS;
}

DWORD CAudioClientApiTest::QueryInterface_InvalidParamsTest()
{
    HRESULT hr = S_OK;
    CComPtr<IAudioClient> spAudioClient = NULL;
    CComPtr<IUnknown> spUnknown = NULL;
        
    //
    // Get an IAudioClient interface
    //
    hr = GetAudioClient(&spAudioClient);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "Unable to obtain IAudioClient interface (0x%x)", hr);
        goto END;
    }

    // invalid IID
    hr = spAudioClient->QueryInterface( __uuidof( IAudioClock ), (LPVOID*)&spUnknown);
    if ( SUCCEEDED(hr) ) 
    {
        XLOG(XFAIL, eError, "FAIL: QueryInterface succeeded for IAudioClock");
    }

    spUnknown = NULL;

    // invalid IID
    hr = spAudioClient->QueryInterface( __uuidof( IAudioRenderClient ), (LPVOID*)&spUnknown);
    if ( SUCCEEDED(hr) ) 
    {
        XLOG(XFAIL, eError, "FAIL: QueryInterface succeeded for IAudioClock");
    }

    spUnknown = NULL;

    // invalid IID
    hr = spAudioClient->QueryInterface( __uuidof( IAudioCaptureClient ), (LPVOID*)&spUnknown);
    if ( SUCCEEDED(hr) ) 
    {
        XLOG(XFAIL, eError, "FAIL: QueryInterface succeeded for IAudioClock");        
    }

    spUnknown = NULL;

    // NULL param
#pragma prefast(suppress:6309, "Disable parameter checking")
    hr = spAudioClient->QueryInterface( __uuidof( IUnknown ), NULL );
    if ( SUCCEEDED(hr) ) 
    {
        XLOG(XFAIL, eError, "FAIL: QueryInterface succeeded w/ NULL output param");
    }

END:
    return FNS_PASS;
}

DWORD CAudioClientApiTest::GetMixFormat_InvalidParamsTest()
{
    HRESULT hr = S_OK;
    CComPtr<IAudioClient> spAudioClient = NULL;

    //
    // Get an IAudioClient interface
    //
#pragma prefast(suppress:6309, "Disable parameter checking")
    hr = GetAudioClient(&spAudioClient);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "Unable to obtain IAudioClient interface (0x%x)", hr);
        goto END;
    }

#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioClient::GetMixFormat",
                      hr,
                      spAudioClient->GetMixFormat(NULL));
    if ( SUCCEEDED(hr) ) 
    {
        XLOG(XFAIL, eError, "FAIL: GetMixFormat succeeded for NULL output param");
    }

    // initialize audio client object
    hr = InitializeAudioClient(&(spAudioClient.p));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "Unable to initialize IAudioClient (0x%x)", hr);
        goto END;
    }

    // try again now that we're initialized

    // NULL param
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioClient::GetMixFormat",
                      hr,
                      spAudioClient->GetMixFormat(NULL));
    if ( SUCCEEDED(hr) ) 
    {
        XLOG(XFAIL, eError, "FAIL: GetMixFormat succeeded for NULL output param");
        goto END;
    }


END:
    return FNS_PASS;
}

DWORD CAudioClientApiTest::SetEventHandle_InvalidParamsTest()
{
    HRESULT hr = S_OK;
    CComPtr<IAudioClient> spAudioClient = NULL;
    HANDLE h = INVALID_HANDLE_VALUE;

    // Create an event handle
    h = CreateEvent(NULL,
                    FALSE,
                    FALSE,
                    NULL);
    if((INVALID_HANDLE_VALUE == h)||(NULL == h))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Unable to allocate event handle!");
        goto END;
    }

    // Get an IAudioClient Pointer
    hr = GetAudioClient(&spAudioClient);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "Unable to obtain IAudioClient interface (0x%x)", hr);
        goto END;
    }

    // Uninitialized client, NULL handle value
    METHODCHECKRESULT("IAudioClient::SetEventHandle",
                      hr,
                      spAudioClient->SetEventHandle(NULL));
    if(SUCCEEDED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Method should not have succeeded for uninit. client, NULL param");
    }

    // Uninitialized client, valid handle value
    METHODCHECKRESULT("IAudioClient::SetEventHandle",
                      hr,
                      spAudioClient->SetEventHandle(h));
    if(SUCCEEDED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Method should not have succeeded for uninit. client");
    }

    //
    // Initialize the IAudioClient without the AUDCLNT_STREAMFLAGS_EVENTCALLBACK param
    //
    hr = InitializeAudioClient(&(spAudioClient.p));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "Unable to initialize IAudioClient (0x%x)", hr);
    }

    METHODCHECKRESULT("IAudioClient::SetEventHandle",
                      hr,
                      spAudioClient->SetEventHandle(h));
    if(SUCCEEDED(hr))
    {
        XLOG(XFAIL,
             eFatalError, 
             "FAIL: Method should not have succeeded without specifiying the AUDCLNT_STREAMFLAGS_EVENTCALLBACK param");
    }

END:
    SAFE_CLOSE_HANDLE(h);
    return FNS_PASS;
}

DWORD CAudioClientApiTest::GetService_ValidParamsTest()
{
    HRESULT hr = S_OK;
    CComPtr<IAudioClient> spAudioClient = NULL;

    // we create arrays of 2 CComPtrs for each of
    // interface to test double IAudioClient->GetService(...)
    // calls.
    CComPtr<IAudioClock>  aspClocks[2];
    CComPtr<IAudioRenderClient> aspRenderClients[2];
    CComPtr<IAudioCaptureClient>  aspCaptureClients[2];
    CComPtr<IAudioClockAdjustment> aspClockAdjustments[2];

    //
    // Get an IAudioClient interface
    //
    hr = GetAudioClient(&spAudioClient);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "Unable to obtain IAudioClient interface (0x%x)", hr);
        goto END;
    }

    // set the RATEADJUST streamflag
    m_dwRequestedStreamFlags |= AUDCLNT_STREAMFLAGS_RATEADJUST;

    // initialize audio client object
    hr = InitializeAudioClient(&(spAudioClient.p));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "Unable to initialize IAudioClient (0x%x)", hr);
        goto END;
    }
        
    if(AUDIO_CLIENT_RENDER == m_nClientType)
    {
        // IAudioRenderClient
        METHODCHECKRESULT("IAudioClient::GetService", 
                          hr, 
                          spAudioClient->GetService( __uuidof( IAudioRenderClient) , 
                                                    (LPVOID*)&aspRenderClients[0] ) );
        if ( FAILED( hr ) ) 
        {
            XLOG(XFAIL, eError, "FAIL: GetService failed for IAudioRenderClient");  
        }


        // IAudioRenderClient
        METHODCHECKRESULT("IAudioClient::GetService", 
                          hr, 
                          spAudioClient->GetService( __uuidof( IAudioRenderClient) , 
                                                    (LPVOID*)&aspRenderClients[1] ) );
        if ( FAILED( hr ) ) 
        {
            XLOG(XFAIL, eError, "FAIL: GetService failed for IAudioRenderClient");
        }

        // IAudioClockAdjustment
        METHODCHECKRESULT(TEXT("IAudioClient::GetService"), 
                          hr, 
                          spAudioClient->GetService( __uuidof(IAudioClockAdjustment) ,
                                                    (LPVOID*)&aspClockAdjustments[0] ) );
        if ( FAILED( hr ) ) 
        {
            XLOG(XFAIL, eError, "FAIL: GetService failed for IAudioClockAdjustments");
        }

        // IAudioClockAdjustment
        METHODCHECKRESULT(TEXT("IAudioClient::GetService"), 
                          hr, 
                          spAudioClient->GetService( __uuidof(IAudioClockAdjustment) , 
                                                    (LPVOID*)&aspClockAdjustments[1] ) );
        if ( FAILED( hr ) ) 
        {
            XLOG(XFAIL, eError, "FAIL: GetService failed for IAudioClockAdjustment");
        }
    }
    else
    {
        // IAudioCaptureClient
        METHODCHECKRESULT(TEXT("IAudioClient::GetService"), 
                          hr, 
                          spAudioClient->GetService( __uuidof( IAudioCaptureClient) , 
                                                    (LPVOID*)&aspCaptureClients[0] ) );
        if ( FAILED( hr ) ) 
        {
            XLOG(XFAIL, eError, "FAIL: GetService failed for IAudioCaptureClient");
        }

        // IAudioCaptureClient
        METHODCHECKRESULT(TEXT("IAudioClient::GetService"), 
                          hr, 
                          spAudioClient->GetService( __uuidof( IAudioCaptureClient) , 
                                                    (LPVOID*)&aspCaptureClients[1] ) );
        if ( FAILED( hr ) ) 
        {
            XLOG(XFAIL, eError, "FAIL: GetService failed for IAudioCaptureClient");
        }
    }

    // IAudioClock
    METHODCHECKRESULT(TEXT("IAudioClient::GetService"), 
                      hr, 
                      spAudioClient->GetService( __uuidof(IAudioClock) , 
                                                (LPVOID*)&aspClocks[0] ) );
    if ( FAILED( hr ) ) 
    {
        XLOG(XFAIL, eError,"FAIL: GetService failed for IAudioClock");
    }


    // IAudioClock
    METHODCHECKRESULT(TEXT("IAudioClient::GetService"), 
                      hr, 
                      spAudioClient->GetService(__uuidof(IAudioClock), 
                                                (LPVOID*)&aspClocks[1] ) );
    if ( FAILED( hr ) ) 
    {
        XLOG(XFAIL, eError,"FAIL: GetService failed for IAudioClock");
    }

END:
    return FNS_PASS;
}

DWORD CAudioClientApiTest::GetBufferSize_ValidParamsTest()
{
    HRESULT hr     = S_OK;
    UINT32 u32BufferFrames = 0;
    UINT32 u32MinExpectedBufferFrames = 0;
    CComPtr<IAudioClient> spAudioClient = NULL;
    WAVEFORMATEX* pwfxStreamingFormat = NULL;

    // Get an IAudioClient
    hr = GetAudioClient(&spAudioClient);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "Unable to obtain IAudioClient interface (0x%x)", hr);
        goto END;
    }

    // Initialize the Audio Client
    hr = InitializeAudioClient(&(spAudioClient.p), &pwfxStreamingFormat);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "Unable to initialize IAudioClient interface (0x%x)", hr);
        goto END;
    }

    METHODCHECKRESULT("IAudioClient::GetBufferSize()",
                      hr,
                      spAudioClient->GetBufferSize(&u32BufferFrames));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error calling IAudioClient::GetBufferSize");
        goto END;
    }
        
    // get minimum number of expected frames in the CP buffer
    u32MinExpectedBufferFrames = 
    (UINT32)( ( ((FLOAT)m_hnsBufferDuration) / ((FLOAT)(HNS_PER_SEC))) *
    (FLOAT)(pwfxStreamingFormat->nSamplesPerSec) );
                                 

    SLOG(eInfo1, "Validating buffer size...");

    SLOG(eInfo1, "Minimum Expected Buffer Size: %d (frames)", u32MinExpectedBufferFrames);
    SLOG(eInfo1, "ACTUAL Buffer Size: %d (frames)", u32BufferFrames);

    // verify that buffer size is >= the requested buffer length
    if ( u32BufferFrames < u32MinExpectedBufferFrames)
    {
        XLOG(XFAIL, eError, "FAIL: CP buffer size is less than expected!");
    }

END:
    SAFE_COTASKMEMFREE(pwfxStreamingFormat);
    return FNS_PASS;
} // GetBufferSize_ValidParamsTest

DWORD CAudioClientApiTest::GetStreamLatency_ValidParamsTest()
{
    HRESULT hr = S_OK;
    CComPtr<IAudioClient> spAudioClient = NULL;
    REFERENCE_TIME hnsLatency;

    // Get an IAudioClient Pointer
    hr = GetAudioClient(&spAudioClient);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "Unable to obtain IAudioClient interface (0x%x)", hr);
        goto END;
    }

    // Initialize the IAudioClient
    hr = InitializeAudioClient(&(spAudioClient.p));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "Unable to initialize IAudioClient interface (0x%x)", hr);
        goto END;
    }

    // valid param
    METHODCHECKRESULT("IAudioClient::GetStreamLatency",
                      hr,
                      spAudioClient->GetStreamLatency( &hnsLatency ));
    if ( FAILED(hr) ) 
    {
        XLOG(XFAIL, eError, "FAIL: GetStreamLatency failed for non-NULL output param");
        goto END;
    }

    SLOG(eInfo1,"###Latency == %I64d(us)", hnsLatency/10);

END:
    return FNS_PASS;
}

DWORD CAudioClientApiTest::Render_GetBuffer_InvalidParamsTest()
{
    HRESULT hr = S_OK;
    CComPtr<IAudioClient> spAudioClient;
    CComPtr<IAudioRenderClient> spRenderClient;
    UINT32 nBufferFrames = 0;

    LPBYTE pData = NULL;

    // Get an IAudioClient Pointer
    hr = GetAudioClient(&spAudioClient);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioClient interface (0x%8x)", hr);
        goto END;
    }

    //
    // Initialize the IAudioClient
    //
    hr = InitializeAudioClient(&(spAudioClient.p));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to initialize IAudioClient (0x%8x)", hr);
        goto END;
    }    

    //
    // Get an IAudioRenderClient pointer
    //
    METHODCHECKRESULT("IAudioClient::GetService",
                      hr,
                      spAudioClient->GetService(__uuidof(IAudioRenderClient),
                                                (void**)(&spRenderClient)));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioRenderClient interface (0x%8x)", hr);
        goto END;
    }

    //
    // Get the buffer size for later use
    //
    METHODCHECKRESULT("IAudioClient::GetBufferSize",
                      hr,
                      spAudioClient->GetBufferSize(&nBufferFrames));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Unable to obtain the buffer size!");
        goto END;
    }
                                            
    // get buffer NULL param    
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioRenderClient::GetBuffer", 
                      hr, 
                      spRenderClient->GetBuffer( 1, NULL ));
    if ( SUCCEEDED( hr ) ) 
    {
        XLOG(XFAIL, eError, "FAIL: GetBuffer NULL param should fail");
    }

    // huge frame count
    METHODCHECKRESULT("IAudioRenderClient::GetBuffer", 
                      hr, 
                      spRenderClient->GetBuffer( (UINT32)-1, &pData ));
    if ( SUCCEEDED( hr ) ) 
    {
        XLOG(XFAIL, eError, "FAIL: GetBuffer for -1 frames should fail");  
    }

    // 0 frame count ( accepted )
    METHODCHECKRESULT("IAudioRenderClient::GetBuffer", 
                      hr, 
                      spRenderClient->GetBuffer( 0, &pData ));
    if ( FAILED( hr ) ) 
    {
        XLOG(XFAIL, eError, "FAIL: GetBuffer for 0 frames should always work");   
    }

    // greater than actual size
    METHODCHECKRESULT("IAudioRenderClient::GetBuffer", 
                      hr, 
                      spRenderClient->GetBuffer( nBufferFrames + 1, &pData ));
    if ( SUCCEEDED( hr ) ) 
    {
        XLOG(XFAIL, eError, "FAIL: GetBuffer for CP buffer length + 1 frames should fail");
    }

END:
    return FNS_PASS;
}

DWORD CAudioClientApiTest::Render_ReleaseBuffer_InvalidParamsTest()
{
    HRESULT hr = S_OK;
    CComPtr<IAudioClient> spAudioClient;
    CComPtr<IAudioRenderClient> spRenderClient;

    LPBYTE pData = NULL;

    // Get an IAudioClient Pointer
    hr = GetAudioClient(&spAudioClient);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioClient interface (0x%8x)", hr);
        goto END;
    }

    //
    // Initialize the IAudioClient
    //
    hr = InitializeAudioClient(&(spAudioClient.p));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to initialize IAudioClient (0x%8x)", hr);
        goto END;
    }    

    //
    // Get an IAudioRenderClient pointer
    //
    METHODCHECKRESULT("IAudioClient::GetService",
                      hr,
                      spAudioClient->GetService(__uuidof(IAudioRenderClient),
                                                (void**)(&spRenderClient)));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioRenderClient interface (0x%8x)", hr);
        goto END;
    }

    // no get buffer
    METHODCHECKRESULT("IAudioRenderClient::ReleaseBuffer", 
                      hr, 
                      spRenderClient->ReleaseBuffer( 1, 0 ));
    if ( SUCCEEDED( hr ) ) 
    {
        XLOG(XFAIL, eError, "FAIL: ReleaseBuffer worked with no previous GetBuffer");
    }


    // get buffer should work    
    METHODCHECKRESULT("IAudioRenderClient::GetBuffer", 
                      hr, 
                      spRenderClient->GetBuffer( 1, &pData ));
    if ( FAILED( hr ) ) 
    {
        XLOG(XFAIL, eError, "FAIL: GetBuffer should always succeed for 1 frame in stop state");   
    }

    // release with invalid flags
    METHODCHECKRESULT("IAudioRenderClient::ReleaseBuffer", 
                      hr, 
                      spRenderClient->ReleaseBuffer( 1, (DWORD)-1 ));
    if ( SUCCEEDED( hr ) ) 
    {
        XLOG(XFAIL, eError, "FAIL: ReleaseBuffer worked with invalid flag -1");
    }


    // release with huge number of frames
    METHODCHECKRESULT("IAudioRenderClient::ReleaseBuffer", 
                      hr, 
                      spRenderClient->ReleaseBuffer( (UINT32)-1, 0 ) );
    if ( SUCCEEDED( hr ) ) 
    {
        XLOG(XFAIL, eError, "FAIL: ReleaseBuffer worked with with -1 number of frames even if we locked 1");
    }

END:
    return FNS_PASS;
}

DWORD CAudioClientApiTest::Render_GetBuffer_Hacker()
{
    HRESULT hr = S_OK;
    CComPtr<IAudioClient> spAudioClient;
    CComPtr<IAudioRenderClient> spRenderClient;

    LPBYTE pData = NULL;

    // Get an IAudioClient Pointer
    hr = GetAudioClient(&spAudioClient);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioClient interface (0x%8x)", hr);
        goto END;
    }

    //
    // Initialize the IAudioClient
    //
    hr = InitializeAudioClient(&(spAudioClient.p));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to initialize IAudioClient (0x%8x)", hr);
        goto END;
    }    

    //
    // Get an IAudioRenderClient pointer
    //
    METHODCHECKRESULT("IAudioClient::GetService",
                      hr,
                      spAudioClient->GetService(__uuidof(IAudioRenderClient),
                                                (void**)(&spRenderClient)));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioRenderClient interface (0x%8x)", hr);
        goto END;
    }

    // get buffer 
    METHODCHECKRESULT("IAudioRenderClient::GetBuffer", 
                      hr, 
                      spRenderClient->GetBuffer(1, &pData));
    if(FAILED(hr)) 
    {
        XLOG(XFAIL, eError, "FAIL: GetBuffer for 1 frame should not fail");
        goto END;  
    }

    // start client
    METHODCHECKRESULT("IAudioClient::Start",
                      hr,
                      spAudioClient->Start());
    if (FAILED(hr)) 
    {
        XLOG(XFAIL, eError, "FAIL: IAudioClient::Start  failed"); 
        goto END; 
    }

    // we know that at offset 0x800 from pData we have the control data
    struct VolatileControlData
    {
        volatile UINT32  u32QueueReadItem;              // queue read index
        volatile UINT32  u32QueueWriteItem;             // queue write index
        volatile INT64   i64TotalReadOffset;            // total bytes read (including silence)
        volatile INT64   i64ValidBytesWritten;          // total bytes written
        volatile INT64   i64ValidBytesRead;             // total bytes read (not including silence)
        volatile UINT32  u32StarvationBytes;            // bytes written due to starvation filling
        volatile UINT32  u32Flags;                      // see ENDPOINT_FLAGS below
        volatile LONG    lSharedUseCount;               // incremented when GetXXXDataPointer called,
                                                        // and decremented when ReleaseXXXDataPointer called
    };

    struct StaticControlData
    {
        UINT32 u32Size;
        MFTIME mfDeviceEndpointLatency;                 // The latency of device endpoint.
        UINT32 u32MaxQueueItems;
        UINT32 u32QueueOffset;
        UINT32 u32StartOfDataOffset;
        UINT32 u32EndOfDataOffset;                      // this is effectively the size of the control data + queue + data buffer
        WAVEFORMATEX frameFormat;
    };

    VolatileControlData* pControl = (VolatileControlData*)(pData - 0x800);
    StaticControlData* pStatic = (StaticControlData*)(((LPBYTE)( pControl + 1 ) ) + 1);    
    
    SLOG(eInfo1,"Huge offsets ...");

    // attack 1: huge offsets
    #pragma prefast(suppress:26001, "Intentionally disable potential underflow check")
    pControl->i64ValidBytesWritten = -1;

    Sleep( 1000 );

    pControl->i64ValidBytesWritten = 0;
    #pragma prefast(suppress:26001, "Intentionally disable potential underflow check")
    pControl->i64ValidBytesRead    = -1;

    Sleep( 1000 );

    #pragma prefast(suppress:26001, "Intentionally disable potential underflow check")
    pControl->i64ValidBytesRead    = -1;
    #pragma prefast(suppress:26001, "Intentionally disable potential underflow check")
    pControl->i64ValidBytesWritten = -1;

    Sleep(1000 ); 

    SLOG(eInfo1,"Full buffer write ...");

    // attack 2: full buffer write
    pControl->i64ValidBytesRead    = 0;
    UINT32 u32BufferLength = pStatic->u32EndOfDataOffset - pStatic->u32StartOfDataOffset;
    #pragma prefast(suppress:26001, "Intentionally disable potential underflow check")
    pControl->i64ValidBytesWritten = u32BufferLength - 1;

    Sleep(1000 );

    SLOG(eInfo1,"Read ahead of write ...");

    // attack 3: read ahead of write
    #pragma prefast(suppress:26001, "Intentionally disable potential underflow check")
    pControl->i64ValidBytesRead    = u32BufferLength - 1;
    #pragma prefast(suppress:26001, "Intentionally disable potential underflow check")
    pControl->i64ValidBytesWritten = u32BufferLength - 2;

    Sleep(1000);


    pControl->i64ValidBytesRead    = u32BufferLength;
    #pragma prefast(suppress:26001, "Intentionally disable potential underflow check")
    pControl->i64ValidBytesWritten = u32BufferLength - 1;

    SLOG(eInfo1,"Unaligned reads ...");

    // attack 4: unaligned read
    pControl->i64ValidBytesWritten = u32BufferLength;
    pControl->i64ValidBytesRead    = 1;
    
    Sleep(1000);
   
END:
    return FNS_PASS;
}

DWORD CAudioClientApiTest::Capture_GetBuffer_InvalidParamsTest()
{
    HRESULT hr = S_OK;
    CComPtr<IAudioClient> spAudioClient;
    CComPtr<IAudioCaptureClient> spCaptureClient;

    DWORD dwFlags = 0;
    UINT32 u32Frames = 0;
    LPBYTE pData = NULL;

    // Get an IAudioClient Pointer
    hr = GetAudioClient(&spAudioClient);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioClient interface (0x%8x)", hr);
        goto END;
    }

    //
    // Initialize the IAudioClient
    //
    hr = InitializeAudioClient(&(spAudioClient.p));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to initialize IAudioClient (0x%8x)", hr);
        goto END;
    }    

    //
    // Get an IAudioCaptureClient pointer
    //
    METHODCHECKRESULT("IAudioClient::GetService",
                      hr,
                      spAudioClient->GetService(__uuidof(IAudioCaptureClient),
                                                (void**)(&spCaptureClient)));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioCaptureClient interface (0x%8x)", hr);
        goto END;
    }

    // get buffer NULL param 1    
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioCaptureClient::GetBuffer", 
                      hr, 
                      spCaptureClient->GetBuffer( NULL, 
                                                  &u32Frames, 
                                                  &dwFlags, 
                                                  NULL,
                                                  NULL ));
    if ( SUCCEEDED( hr ) ) 
    {
        XLOG(XFAIL, eError, "FAIL: GetBuffer NULL param 1 should fail");    
    }


    // get buffer NULL param 2
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioCaptureClient::GetBuffer", 
                      hr, 
                      spCaptureClient->GetBuffer((LPBYTE*)&pData, 
                                                 NULL, 
                                                 &dwFlags, 
                                                 NULL, 
                                                 NULL));
    if ( SUCCEEDED( hr ) ) 
    {
        XLOG(XFAIL, eError, "FAIL: GetBuffer NULL param 2 should fail");
    }


    // get buffer NULL param 3
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioCaptureClient::GetBuffer", 
                      hr, 
                      spCaptureClient->GetBuffer((LPBYTE*)&pData, 
                                                 &u32Frames, 
                                                 NULL, 
                                                 NULL, 
                                                 NULL) );
    if ( SUCCEEDED( hr ) ) 
    {
        XLOG(XFAIL, eError, "FAIL: GetBuffer NULL param 3 should fail");
    }

END:
    return FNS_PASS;
}
   
DWORD CAudioClientApiTest::Capture_ReleaseBuffer_InvalidParamsTest()
{
    HRESULT hr = S_OK;
    CComPtr<IAudioClient> spAudioClient;
    CComPtr<IAudioCaptureClient> spCaptureClient;

    // Get an IAudioClient Pointer
    hr = GetAudioClient(&spAudioClient);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioClient interface (0x%8x)", hr);
        goto END;
    }

    //
    // Initialize the IAudioClient
    //
    hr = InitializeAudioClient(&(spAudioClient.p));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to initialize IAudioClient (0x%8x)", hr);
        goto END;
    }    

    //
    // Get an IAudioCaptureClient pointer
    //
    METHODCHECKRESULT("IAudioClient::GetService",
                      hr,
                      spAudioClient->GetService(__uuidof(IAudioCaptureClient),
                                                (void**)(&spCaptureClient)));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioCaptureClient interface (0x%8x)", hr);
        goto END;
    }

    // no get buffer
    METHODCHECKRESULT("IAudioCaptureClient::ReleaseBuffer", 
                      hr, 
                      spCaptureClient->ReleaseBuffer( 1 ));
    if ( SUCCEEDED( hr ) ) 
    {
        XLOG(XFAIL, eError, "FAIL: ReleaseBuffer(1) worked with no previous GetBuffer");
    }

    // release should fail when there's nothing to release
    METHODCHECKRESULT("IAudioCaptureClient::ReleaseBuffer", 
                      hr, 
                      spCaptureClient->ReleaseBuffer( 0 ));
    if ( FAILED( hr ) ) 
    {
        XLOG(XFAIL, eError, "FAIL: ReleaseBuffer(0) should not fail under any circumstances");
    }

    // no get buffer
    METHODCHECKRESULT("IAudioCaptureClient::ReleaseBuffer", 
                      hr, 
                      spCaptureClient->ReleaseBuffer( (UINT32)-1 ));
    if ( SUCCEEDED( hr ) ) 
    {
        XLOG(XFAIL, eError, "FAIL: ReleaseBuffer(-1) worked with no previous GetBuffer");
    }

END:
    return FNS_PASS;
}

DWORD CAudioClientApiTest::Capture_GetBuffer_Hacker()
{
    HRESULT hr = S_OK;
    CComPtr<IAudioClient> spAudioClient;
    CComPtr<IAudioCaptureClient> spCaptureClient;

    DWORD dwFlags = 0;
    UINT32 u32Frames = 0;
    LPBYTE pData = NULL;

    // Get an IAudioClient Pointer
    hr = GetAudioClient(&spAudioClient);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioClient interface (0x%8x)", hr);
        goto END;
    }

    //
    // Initialize the IAudioClient
    //
    hr = InitializeAudioClient(&(spAudioClient.p));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to initialize IAudioClient (0x%8x)", hr);
        goto END;
    }    

    //
    // Get an IAudioRenderClient pointer
    //
    METHODCHECKRESULT("IAudioClient::GetService",
                      hr,
                      spAudioClient->GetService(__uuidof(IAudioCaptureClient),
                                                (void**)(&spCaptureClient)));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioCaptureClient interface (0x%8x)", hr);
        goto END;
    }

    // start client
    METHODCHECKRESULT("IAudioClient::Start",
                      hr,
                      spAudioClient->Start());

    if (FAILED(hr)) 
    {
        XLOG(XFAIL, eError, "FAIL: IAudioClient::Start  failed");
        goto END;
    }

    // wait until we get some data
    Sleep( 100 );

    // get buffer 
    METHODCHECKRESULT("IAudioCaptureClient::GetBuffer", 
                      hr, 
                      spCaptureClient->GetBuffer(&pData, 
                                                 &u32Frames, 
                                                 &dwFlags, 
                                                 NULL, 
                                                 NULL ) );
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: GetBuffer should not fail!");
        goto END;
    }
    else if(AUDCLNT_S_BUFFER_EMPTY == hr)
    {
        // don't probe the buffer when there's no data
        goto END;
    }

    if(NULL == pData)
    {
        XLOG(XFAIL, eError, "FAIL: GetBuffer should not return NULL!");
        goto END;
    }
    
    // we know that at offset 0x800 from pData we have the control data
    struct VolatileControlData
    {
        volatile UINT32  u32QueueReadItem;              // queue read index
        volatile UINT32  u32QueueWriteItem;             // queue write index
        volatile INT64   i64TotalReadOffset;            //  total bytes read (including silence)
        volatile INT64   i64ValidBytesWritten;          // total bytes written
        volatile INT64   i64ValidBytesRead;             // total bytes read (not including silence)
        volatile UINT32  u32StarvationBytes;            // bytes written due to starvation filling
        volatile UINT32  u32Flags;                      // see ENDPOINT_FLAGS below
        volatile LONG    lSharedUseCount;               // incremented when GetXXXDataPointer called,
                                                        // and decremented when ReleaseXXXDataPointer called
    };

    struct StaticControlData
    {
        UINT32 u32Size;
        MFTIME mfDeviceEndpointLatency;                 // The latency of device endpoint.
        UINT32 u32MaxQueueItems;
        UINT32 u32QueueOffset;
        UINT32 u32StartOfDataOffset;
        UINT32 u32EndOfDataOffset;                      // this is effectively the size of the control data + queue + data buffer
        WAVEFORMATEX frameFormat;
    };

    VolatileControlData* pControl = (VolatileControlData*)(pData - 0x800);
    StaticControlData* pStatic = (StaticControlData*)(((LPBYTE)( pControl + 1 ) ) + 1);    

    // attack 1: huge offsets
    SLOG(eInfo1,"Huge offsets ...");  
    #pragma prefast(suppress:26001, "Intentionally disable potential underflow check")
    pControl->i64ValidBytesWritten = -1;

    Sleep( 1000 );

    pControl->i64ValidBytesWritten = 0;
    #pragma prefast(suppress:26001, "Intentionally disable potential underflow check")
    pControl->i64ValidBytesRead    = -1;

    Sleep( 1000 );

    #pragma prefast(suppress:26001, "Intentionally disable potential underflow check")
    pControl->i64ValidBytesRead    = -1;
    #pragma prefast(suppress:26001, "Intentionally disable potential underflow check")
    pControl->i64ValidBytesWritten = -1;

    Sleep(1000 ); 

    // attack 2: full buffer write
    SLOG(eInfo1,"Full buffer write ...");
    pControl->i64ValidBytesRead    = 0;
    UINT32 u32BufferLength = pStatic->u32EndOfDataOffset - pStatic->u32StartOfDataOffset;
    #pragma prefast(suppress:26001, "Intentionally disable potential underflow check")
    pControl->i64ValidBytesWritten = u32BufferLength - 1;

    Sleep(1000 );

    // attack 3: read ahead of write
    SLOG(eInfo1,"Read ahead of write ...");
    #pragma prefast(suppress:26001, "Intentionally disable potential underflow check")
    pControl->i64ValidBytesRead    = u32BufferLength - 1;
    #pragma prefast(suppress:26001, "Intentionally disable potential underflow check")
    pControl->i64ValidBytesWritten = u32BufferLength - 2;

    Sleep(1000);


    pControl->i64ValidBytesRead    = u32BufferLength;
    #pragma prefast(suppress:26001, "Intentionally disable potential underflow check")
    pControl->i64ValidBytesWritten = u32BufferLength - 1;

    // attack 4: unaligned read
    SLOG(eInfo1,"Unaligned reads ...");
    pControl->i64ValidBytesWritten = u32BufferLength;
    pControl->i64ValidBytesRead    = 1;
    
    Sleep(1000);

END:
    return FNS_PASS;
}

DWORD CAudioClientApiTest::Clock_GetPosition_InvalidParamsTest()
{
    HRESULT hr = S_OK;
    CComPtr<IAudioClient> spAudioClient;
    CComPtr<IAudioClock> spClock;
    
    UINT64 u64QPCPosition = 0;

    // Get an IAudioClient Pointer
    hr = GetAudioClient(&(spAudioClient.p));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioClient interface (0x%8x)", hr);
        goto END;
    }

    //
    // Initialize the IAudioClient
    //
    hr = InitializeAudioClient(&(spAudioClient.p));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to initialize IAudioClient (0x%8x)", hr);
        goto END;
    }    

    //
    // Get an IAudioRenderClient pointer
    //
    METHODCHECKRESULT("IAudioClient::GetService",
                      hr,
                      spAudioClient->GetService(__uuidof(IAudioClock),
                                                (void**)(&spClock)));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioClock interface (0x%8x)", hr);
        goto END;
    }

    // NULL param 1
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioClock::GetPosition",
                      hr,
                      spClock->GetPosition(NULL, &u64QPCPosition));
    if(SUCCEEDED(hr)) 
    {
        XLOG(XFAIL, eError, "FAIL: GetPosition succeeded for NULL output param 1");
    }

    // all NULL params
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioClock::GetPosition",
                      hr,
                      spClock->GetPosition(NULL, NULL));
    if(SUCCEEDED(hr)) 
    {
        XLOG(XFAIL, eError, "FAIL: GetPosition succeeded for NULL output params 1, 2");
    }

END:
    return FNS_PASS;
}

DWORD CAudioClientApiTest::Clock_GetFrequency_InvalidParamsTest()
{
    HRESULT hr = S_OK;
    CComPtr<IAudioClient> spAudioClient;
    CComPtr<IAudioClock> spClock;

    // Get an IAudioClient Pointer
    hr = GetAudioClient(&spAudioClient);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioClient interface (0x%8x)", hr);
        goto END;
    }

    //
    // Initialize the IAudioClient
    //
    hr = InitializeAudioClient(&(spAudioClient.p));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to initialize IAudioClient (0x%8x)", hr);
        goto END;
    }    

    //
    // Get an IAudioRenderClient pointer
    //
    METHODCHECKRESULT("IAudioClient::GetService",
                      hr,
                      spAudioClient->GetService(__uuidof(IAudioClock),
                                                (void**)(&spClock)));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioClock interface (0x%8x)", hr);
        goto END;
    }

    // NULL param 1
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioClock::GetFrequency",
                      hr,
                      spClock->GetFrequency(NULL));
    if(SUCCEEDED(hr)) 
    {
        XLOG(XFAIL, eError, "FAIL: GetFrequency succeeded for NULL output param");
    }

END:
    return FNS_PASS;
}

DWORD CAudioClientApiTest::Clock_GetCharacteristics_InvalidParamsTest()
{
    HRESULT hr = S_OK;
    CComPtr<IAudioClient> spAudioClient;
    CComPtr<IAudioClock> spClock;

    // Get an IAudioClient Pointer
    hr = GetAudioClient(&spAudioClient);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioClient interface (0x%8x)", hr);
        goto END;
    }

    //
    // Initialize the IAudioClient
    //
    hr = InitializeAudioClient(&(spAudioClient.p));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to initialize IAudioClient (0x%8x)", hr);
        goto END;
    }    

    //
    // Get an IAudioRenderClient pointer
    //
    METHODCHECKRESULT("IAudioClient::GetService",
                      hr,
                      spAudioClient->GetService(__uuidof(IAudioClock),
                                                (void**)(&spClock)));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioClock interface (0x%8x)", hr);
        goto END;
    }

    // NULL param 1
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioClock::GetCharacteristics",
                      hr,
                      spClock->GetCharacteristics(NULL));
    if(SUCCEEDED(hr)) 
    {
        XLOG(XFAIL, eError, "FAIL: GetCharacteristics succeeded for NULL output param");
    }

END:
    return FNS_PASS;
}

DWORD CAudioClientApiTest::SetEventHandle_ValidParamsTest()
{
    HRESULT hr = S_OK;
    CComPtr<IAudioClient> spAudioClient = NULL;
    HANDLE h = INVALID_HANDLE_VALUE;


    // Get an IAudioClient Pointer
    hr = GetAudioClient(&spAudioClient);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "Unable to obtain IAudioClient interface (0x%x)", hr);
        goto END;
    }

    // Ensure that the AUDCLNT_STREAMFLAGS_EVENTCALLBACK flag is set
    m_dwRequestedStreamFlags |= AUDCLNT_STREAMFLAGS_EVENTCALLBACK;
    
    // if we're streaming in exclusive mode, match the buffer size and engine periodicity
    if(AUDCLNT_SHAREMODE_EXCLUSIVE == m_nShareMode)
    {
        m_hnsBufferDuration = DEFAULT_CP_BUFFER_SIZE;
        m_hnsPeriodicity = DEFAULT_CP_BUFFER_SIZE;
    }

    
    // Initialize the IAudioClient
    hr = InitializeAudioClient(&(spAudioClient.p));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "Unable to initialize IAudioClient interface (0x%x)", hr);
        goto END;
    }

    // Create an event handle
    h = CreateEvent(NULL,
                    FALSE,
                    FALSE,
                    NULL);
    if((INVALID_HANDLE_VALUE == h)||(NULL == h))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Unable to allocate event handle!");
        goto END;
    }

    // Attempt the SetHandle call
    METHODCHECKRESULT("IAudioClient::SetEventHandle()",
                      hr,
                      spAudioClient->SetEventHandle(h));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eInfo1, "FAIL: Method IAudioClient::SetEventHandle failed for valid param");
    }   
                    
END:
    SAFE_CLOSE_HANDLE(h);
    return FNS_PASS;
}

HRESULT CAudioClientApiTest::Method_IsFormatSupported(ISFORMATSUPPORTED_PARAM_SPACE::ISFORMATSUPPORTED_PARAMS params,
                                                      IAudioClient* pAudioClient) 
{
        HRESULT hr = S_OK;
        WAVEFORMATEX* pwfxDummyFormat = 0;

        SLOG(eInfo1,
             "          SHARE MODE = %s ; CLIENT TYPE = %s", 
             GetMode(m_nShareMode), 
             GetType(m_nClientType));
        
        if (!params.getFormat())   
        { 
            SLOG(eInfo1,"          FORMAT == NULL"); 
        } 
        else 
        {
            SLOG(eInfo1, 
                 "          FORMAT: (tag)= 0x%X (ch) = %ld (bits)= %ld (sr) = %ld (br) = %ld (align) = %ld (size) = %ld",
                 params.getFormat()->wFormatTag,
                 params.getFormat()->nChannels,
                 params.getFormat()->wBitsPerSample,
                 params.getFormat()->nSamplesPerSec,
                 params.getFormat()->nAvgBytesPerSec,
                 params.getFormat()->nBlockAlign,
                 params.getFormat()->cbSize);
        }

        if ( params.getFormat() && params.getFormat()->wFormatTag == WAVE_FORMAT_EXTENSIBLE ) 
        {
            WAVEFORMATEXTENSIBLE* pWfx = (WAVEFORMATEXTENSIBLE*)params.getFormat();
            SLOG(eInfo1, 
                 "          FORMAT: (valid bits)= %ld (speaker) = 0x%X ",
                 pWfx->Samples.wValidBitsPerSample,
                 pWfx->dwChannelMask);
        }

        METHODCHECKRESULT(TEXT("IAudioClient::IsFormatSupported"), 
                          hr, 
                          pAudioClient->IsFormatSupported(params.getShareMode(), 
                                                          params.getFormat(), 
                                                          &pwfxDummyFormat));
        SAFE_COTASKMEMFREE(pwfxDummyFormat);

        return hr;
}

HRESULT CAudioClientApiTest::Method_Initialize(INITIALIZE_PARAM_SPACE::INITIALIZE_PARAMS params,
                                               IAudioClient* pAudioClient)
{
        HRESULT hr = S_OK;
        SLOG(eInfo1,
             "          FLOW = %s ; MODE = %s; FLAGS = %s %s %s;", 
             GetMode(params.Mode), 
             GetType(m_nClientType),
             GetFlags(params.StreamFlags & AUDCLNT_STREAMFLAGS_LOOPBACK),
             GetFlags(params.StreamFlags & AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM),
             GetFlags(params.StreamFlags & AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY));

        SLOG(eInfo1,
             "          PARAM1 = %ld", 
             params.Mode );

        SLOG(eInfo2,
             "          PARAM2 = %I64d", 
             params.Time._data );

        WAVEFORMATEX* Format = params.getFormat();

        if ( !Format )  
        { 
            SLOG(eInfo1,"          PARAM3 == NULL"); 
        } 
        else 
        {
            SLOG(eInfo1, "          PARAM3: (tag)= 0x%X (ch) = %ld (bits)= %ld (sr) = %ld (br) = %ld (align) = %ld (size) = %ld",
                        Format->wFormatTag,
                        Format->nChannels,
                        Format->wBitsPerSample,
                        Format->nSamplesPerSec,
                        Format->nAvgBytesPerSec,
                        Format->nBlockAlign,
                        Format->cbSize);
        }
        if ( params.getFormat() && params.getFormat()->wFormatTag == WAVE_FORMAT_EXTENSIBLE ) {
            WAVEFORMATEXTENSIBLE* pWfx = (WAVEFORMATEXTENSIBLE*)params.getFormat();
            SLOG(eInfo1, "          PARAM1: (valid bits)= %ld (speaker) = 0x%X ",
                        pWfx->Samples.wValidBitsPerSample,
                        pWfx->dwChannelMask);
        }
        if ( !params.Submix ) { SLOG(eInfo2,"          PARAM4 == NULL"); }
        SLOG(eInfo2,"          STREAMFLAGS = %ld", params.StreamFlags );

        METHODCHECKRESULT(TEXT("IAudioClient::Initialize"), 
                          hr, 
                          pAudioClient->Initialize(params.Mode, 
                          params.StreamFlags, 
                          params.Time, 
                          0, 
                          params.WaveFormatEx, 
                          params.Submix) );

        return hr;
}
