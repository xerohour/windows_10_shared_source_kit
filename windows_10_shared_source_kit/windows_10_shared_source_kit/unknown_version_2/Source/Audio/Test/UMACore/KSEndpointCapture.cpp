// Copyright(C) Microsoft. All rights reserved.

#include "stdafx.h"


//KSCaptureEndpoint supported interfaces
//  IAudioClock
//  IAudioCrossProcessClientEndpoint
//  IAudioDeviceEndpoint
//  IAudioEndpoint
//  IAudioEndpointControl
//  IAudioEndpointGlitch
//  IAudioEndpointRT
//  IAudioInputEndpointRT

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSCaptureEndpointCreate
//
// Description:
//  TestCase to test if KSCaptureEndpoint class is registered and Endpoint interface can be obtained from it
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSCaptureEndpointCreate()
{
    CKSEndpointCaptureTest EndpointTest;
    return EndpointTest.EndpointCreate();
}

/******************************************************************************************************************/
/******************************         IAudioClock Tests                            ******************************/
/******************************************************************************************************************/
//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSCaptureEndpointGetFrequency
//
// Description:
//  TestCase to test IAudioClock::GetFrequency API on the KSCaptureEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSCaptureEndpointGetFrequency()
{
    CKSEndpointCaptureTest EndpointTest;
    return EndpointTest.EndpointGetFrequency();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSCaptureEndpointGetPosition
//
// Description:
//  TestCase to test IAudioClock::GetPosition API on the KSCaptureEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSCaptureEndpointGetPosition()
{
    CKSEndpointCaptureTest EndpointTest;
    return EndpointTest.EndpointGetPosition();
}

/******************************************************************************************************************/
/******************************         IAudioCrossProcessClientEndpoint Tests       ******************************/
/******************************************************************************************************************/
//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSCaptureEndpointCPClientInitialize
//
// Description:
//  TestCase to test IAudioCrossProcessClientEndpoint::Initialize API on the KSCaptureEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSCaptureEndpointCPClientInitialize()
{
    CKSEndpointCaptureTest EndpointTest;
    return EndpointTest.EndpointCPClientInitialize();
}

/******************************************************************************************************************/
/******************************         IAudioDeviceEndpoint Tests                   ******************************/
/******************************************************************************************************************/
// Class: 
// Method: Tc_KSCaptureEndpointSetBuffer
//
// Description:
//  TestCase to test IAudioDeviceEndpoint::SetBuffer API on the KSCaptureEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSCaptureEndpointSetBuffer()
{
    CKSEndpointCaptureTest EndpointTest;
    return EndpointTest.EndpointSetBuffer();
}

// Class: 
// Method: Tc_KSCaptureEndpointWriteExclusiveModeParametersToSharedMemory
//
// Description:
//  TestCase to test IAudioDeviceEndpoint::WriteExclusiveModeParametersToSharedMemory API on the KSCaptureEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSCaptureEndpointWriteExclusiveModeParametersToSharedMemory()
{
    CKSEndpointCaptureTest EndpointTest;
    return EndpointTest.EndpointWriteExclusiveModeParametersToSharedMemory();
}

// Class: 
// Method: Tc_KSCaptureEndpointGetRTCaps
//
// Description:
//  TestCase to test IAudioDeviceEndpoint::GetRTCaps API on the KSCaptureEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSCaptureEndpointGetRTCaps()
{
    CKSEndpointCaptureTest EndpointTest;
    return EndpointTest.EndpointGetRTCaps();
}


/******************************************************************************************************************/
/******************************         IAudioEndpoint Tests                         ******************************/
/******************************************************************************************************************/
//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSCaptureEndpointGetFrameFormat
//
// Description:
//  TestCase to test IAudioEndpoint::GetFrameFormat API on the KSCaptureEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSCaptureEndpointGetFrameFormat()
{
    CKSEndpointCaptureTest EndpointTest;
    return EndpointTest.EndpointGetFrameFormat();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSCaptureEndpointGetLatency
//
// Description:
//  TestCase to test IAudioEndpoint::GetLatency API on the KSCaptureEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSCaptureEndpointGetLatency()
{
    CKSEndpointCaptureTest EndpointTest;
    return EndpointTest.EndpointGetLatency();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSCaptureEndpointGetFramesPerPacket
//
// Description:
//  TestCase to test IAudioEndpoint::GetFramesPerPacket API on the KSCaptureEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSCaptureEndpointGetFramesPerPacket()
{
    CKSEndpointCaptureTest EndpointTest;
    return EndpointTest.EndpointGetFramesPerPacket();
}


/******************************************************************************************************************/
/******************************         IAudioEndpointControl Tests                  ******************************/
/******************************************************************************************************************/
//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSCaptureEndpointReset
//
// Description:
//  TestCase to test IAudioEndpointControl::Reset API on the KSCaptureEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSCaptureEndpointReset()
{
    CKSEndpointCaptureTest EndpointTest;
    return EndpointTest.EndpointReset();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSCaptureEndpointStart
//
// Description:
//  TestCase to test IAudioEndpointControl::Start API on the KSCaptureEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSCaptureEndpointStart()
{
    CKSEndpointCaptureTest EndpointTest;
    return EndpointTest.EndpointStart();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSCaptureEndpointStop
//
// Description:
//  TestCase to test IAudioEndpointControl::Stop API on the KSCaptureEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSCaptureEndpointStop()
{
    CKSEndpointCaptureTest EndpointTest;
    return EndpointTest.EndpointStop();
}

/******************************************************************************************************************/
/******************************         IAudioEndpointGlitch Tests                       ******************************/
/******************************************************************************************************************/
//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSCaptureEndpointGetGlitchCount
//
// Description:
//  TestCase to test IAudioClock::GetGlitchCount API on the KSCaptureEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSCaptureEndpointGetGlitchCount()
{
    CKSEndpointCaptureTest EndpointTest;
    return EndpointTest.EndpointGetGlitchCount();
}

/******************************************************************************************************************/
/******************************         IAudioEndpointRT Tests                       ******************************/
/******************************************************************************************************************/
//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSCaptureEndpointGetCurrentPadding
//
// Description:
//  TestCase to test IAudioClock::GetCurrentPadding API on the KSCaptureEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSCaptureEndpointGetCurrentPadding()
{
    CKSEndpointCaptureTest EndpointTest;
    return EndpointTest.EndpointGetCurrentPadding();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSCaptureEndpointGetCurrentTimeStamp
//
// Description:
//  TestCase to test IAudioClock::GetCurrentTimeStamp API on the KSCaptureEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSCaptureEndpointGetCurrentTimeStamp()
{
    CKSEndpointCaptureTest EndpointTest;
    return EndpointTest.EndpointGetTimeStamp();
}

/******************************************************************************************************************/
/******************************         IAudioInputEndpointRT Tests                  ******************************/
/******************************************************************************************************************/
//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSCaptureEndpointGetInputDataPointer
//
// Description:
//  TestCase to test IAudioinputEndpointRT::GetInputDataPointer API on the KSCaptureEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSCaptureEndpointGetInputDataPointer()
{
    CKSEndpointCaptureTest EndpointTest;
    return EndpointTest.EndpointGetInputDataPointer();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSCaptureEndpointReleaseInputDataPointer
//
// Description:
//  TestCase to test IAudioinputEndpointRT::ReleaseInputDataPointer API on the KSCaptureEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSCaptureEndpointReleaseInputDataPointer()
{
    CKSEndpointCaptureTest EndpointTest;
    return EndpointTest.EndpointReleaseInputDataPointer();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSCaptureEndpointTestPositions
//
// Description:
//  TestCase to that the Capture Endpoint returns the right positions
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSCaptureEndpointTestPositions()
{
    CKSEndpointCaptureTest EndpointTest;
    return EndpointTest.EndpointTestPositions();
}

//-------------------------------------------------------------------------
// Class: CKSEndpointCaptureTest
// Method: InitializeAndSetBuffer
//
// Description:
//  This method initializes the CaptureEndpoint, It gets the appropriate symbolic name for the Capture Device
//  and then calls Initialize on the Endpoint.
//  The method then sets the buffer on the endpoint with the preferred format and a period of 10 msec
//  
// Parameters:
//      LPCWSTR szPnpID
//              The PnpID of the device that the endpoint is to be initialized with. It would be generally obtained
//              from the funtion instance object. The default value is NULL in which case the method proceeds with
//              the PnpID of the default Capture device
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
HRESULT CKSEndpointCaptureTest::InitializeAndSetBuffer(LPCWSTR szPnpID, PROPVARIANT* pCreateParams, MFTIME mfBufferTime)
{
    CComPtr<IAudioEndpoint> spAudioEndpoint;
    BOOL isEventCapable;
    HANDLE eventHandle = NULL;
    HRESULT hr = S_OK;

    _ASSERT(m_pDevEndpoint != NULL);

    if (NULL == m_pDevEndpoint)
    {
        SLOG(eError, "ERROR: NULL == m_pDevEndpoint");
        return (FNS_FAIL);
    }
    
    hr = m_pDevEndpoint->GetEventDrivenCapable(&isEventCapable);
    METHOD_CHECKFAIL_EXIT(hr, "ERROR: GetEventDrivenCapable FAILED");

    // If the endpoint is event capable, then initialize it this way since that
    // is how the audio engine will use it, and the newest DDIs assume this.
    if (isEventCapable)
    {
        hr = m_pDevEndpoint->QueryInterface(IID_PPV_ARGS(&spAudioEndpoint));
        METHOD_CHECKFAIL_EXIT(hr, "ERROR: GetEventDrivenCapable FAILED");
        hr = spAudioEndpoint->SetStreamFlags(AUDCLNT_STREAMFLAGS_EVENTCALLBACK);
        METHOD_CHECKFAIL_EXIT(hr, "ERROR: SetStreamFlags FAILED");
    }

    SLOG(eInfo1, "Calling SetBuffer on the Endpoint");
    hr = SetDeviceBuffer(mfBufferTime, GET_CAPTURE_WFX());
    METHOD_CHECKFAIL_EXIT(hr, "ERROR: SetBuffer on the Endpoint FAILED");

    if (isEventCapable)
    {
        eventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
        hr = (eventHandle != NULL) ? S_OK : HRESULT_FROM_WIN32(GetLastError());
        METHOD_CHECKFAIL_EXIT(hr, "ERROR: CreateEvent FAILED");
        hr = spAudioEndpoint->SetEventHandle(eventHandle);
        METHOD_CHECKFAIL_EXIT(hr, "ERROR: SetEventHandle FAILED");
    }

exit:    
    if (eventHandle != NULL)
    {
        CloseHandle(eventHandle);
    }
    return hr;
}

//-------------------------------------------------------------------------
// Class: CKSEndpointCaptureTest
// Method: EndpointGetInputDataPointer
//
// Description:
//  TestCase for IAudioinputEndpointRT::EndpointGetInputDataPointer API 
//
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CKSEndpointCaptureTest::EndpointGetInputDataPointer()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_FAIL;
    _ASSERT(m_pDevEndpoint != NULL);
    SLOG(eError, "Test NOT IMPLEMENTED");

    return dw;
}

//-------------------------------------------------------------------------
// Class: CKSEndpointCaptureTest
// Method: EndpointReleaseInputDataPointer
//
// Description:
//  TestCase for IAudioinputEndpointRT::EndpointReleaseInputDataPointer API 
//
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CKSEndpointCaptureTest::EndpointReleaseInputDataPointer()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_FAIL;
    _ASSERT(m_pDevEndpoint != NULL);
    SLOG(eError, "Test NOT IMPLEMENTED");

    return dw;
}

//-------------------------------------------------------------------------
// Class: CKSEndpointCaptureTest
// Method: CheckExclusiveLoopedBufferSize
//
// Description:
//  API Test to check that a client can specify the looped buffer size in Exclusive Mode
//  This test tries the corner cases as well as valid values for the looped buffer size
//
// Parameters:
//
// Return values (HRESULT):
//     Return S_OK on Success, approp error on Failure
//
HRESULT CKSEndpointCaptureTest::CheckExclusiveLoopedBufferSize()
{
    HRESULT hr = S_OK;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // CURRENTLY REALTIME ENDPOINTS ARE NOT ENABLED, SO THE REALTIME SECTION OF 
    // THE FOLLOWING IS COMMENTED OUT
    // UNCOMMENT THE FOLLOWING WHEN REALTIME ENDPOINTS ARE ENABLED
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    //if(GetModule()->IsRt(m_bRender))
    //{ //Realtime streaming
    //    hr = CheckRealtimeExclusiveLoopedBufferSize();
    //}
    //else
    //{ //Standard streaming
        hr = CheckStandardExclusiveLoopedBufferSize();
    //}

    return hr;
}

//-------------------------------------------------------------------------
// Class: CKSEndpointCaptureTest
// Method: CheckRealtimeExclusiveLoopedBufferSize
//
// Description:
//  API Test to check looped buffer size for Realtime streaming
//
// Parameters:
//
// Return values (HRESULT):
//     Return S_OK on Success, approp error on Failure
//
HRESULT CKSEndpointCaptureTest::CheckRealtimeExclusiveLoopedBufferSize()
{
    HRESULT hr = S_OK;
    IAudioCrossProcessClientEndpointPtr CPClient = NULL;    
    UINT_PTR        hSharedMem = NULL;
    UINT32          u32cbSharedMemorySize = 0;
    HANDLE          hCreateParams = NULL;
    WAVEFORMATEX*   pWfx = NULL;
    UINT32          u32NumFrames = 0;
    UINT32          u32ExpectedNumFrames = 0;
    CComQIPtr<IAudioDeviceEndpoint2> spAudioDeviceEndpoint2;
    CHandle         hPin, hPin1;

    //UINT_PTR phSharedMutex = (UINT_PTR)&hSharedEvent;
    
    //For CaptureEndpoints the following values are allowed for the looped buffersize
    //for REALTIME Streaming
    // greater than or equal to REALTIME_CAPTURE_LATENCY_COEFFICIENT
    //                                      1
    
    pWfx = GET_CAPTURE_WFX();
    //Try initializing the endpoint with latency = 0
    m_pDevEndpoint = NULL;    
    hr = CreateEndpoint();
    METHOD_CHECKFAIL_EXIT(hr, "ERROR: Unable to Create DeviceEndpoint");
    hr = CPClient.CreateInstance(__uuidof(AudioKSCaptureEndpoint));
    METHOD_CHECKFAIL_EXIT(hr, "ERROR: Unable to Create CPClientEndpoint");

    //Write the Exclusive mode params to Shared Memory
    SLOG(eInfo1, "Calling DevEndpoint->WriteExclModeParams");
    hr = m_pDevEndpoint->WriteExclusiveModeParametersToSharedMemory((UINT_PTR)GetCurrentProcess(),
                                                                    10*MFTIME1MSEC, //10 msPeriod
                                                                    10*MFTIME1MSEC, //Buffer is 10ms
                                                                    0, //Latency
                                                                    &u32cbSharedMemorySize,
                                                                    &hSharedMem
                                                                   );
    METHOD_CHECKFAIL_EXIT(hr, "ERROR: WriteExclModeParams FAILED");

    spAudioDeviceEndpoint2 = m_pDevEndpoint;
    VERIFY_IS_NOT_NULL(spAudioDeviceEndpoint2);

    VERIFY_SUCCEEDED(spAudioDeviceEndpoint2->GetPinHandle(&hPin.m_h));
    
    SLOG(eInfo1, "Initializing when latency is 0");
    hr = CPClient->Initialize(hSharedMem, NULL, reinterpret_cast<UINT_PTR>(hPin.m_h), NULL, NULL, 0);
    METHOD_CHECKFAIL_EXIT(hr, "ERROR:Initializing with latency = 0 FAILED");

    hr = CPClient->GetBufferSize(&u32NumFrames);
    METHOD_CHECKFAIL_EXIT(hr, "ERROR:GetBufferSize on CPClientEndpoint FAILED");

    u32ExpectedNumFrames = 10 * (pWfx->nSamplesPerSec / 1000);
    if(u32NumFrames != u32ExpectedNumFrames)
    { //Check if the num frames == 10ms worth of data, since WriteExcl was called with 10ms Buffer
        hr = E_FAIL;
        SLOG(eError, "ERROR: GetBufferSize returned the incorrect number of frames");
        SLOG(eError, "ERROR: Expected NumFrames%d, Returned NumFrames %d", u32ExpectedNumFrames, u32NumFrames);
        goto exit;
    }


    //Try min latency and the Buffer more than 1 quantum
    m_pDevEndpoint = NULL;
    hr = CreateEndpoint();
    METHOD_CHECKFAIL_EXIT(hr, "ERROR: Unable to Create DeviceEndpoint");
    hr = CPClient.CreateInstance(__uuidof(AudioKSCaptureEndpoint));
    METHOD_CHECKFAIL_EXIT(hr, "ERROR: Unable to Create CPClientEndpoint");

    //Write the Exclusive mode params to Shared Memory
    SLOG(eInfo1, "Calling DevEndpoint->WriteExclModeParams");
    hr = m_pDevEndpoint->WriteExclusiveModeParametersToSharedMemory((UINT_PTR)GetCurrentProcess(),
                                                                    10*MFTIME1MSEC, //10 msPeriod
                                                                    100*MFTIME1MSEC, //Buffer is 100ms
                                                                    1, //Latency
                                                                    &u32cbSharedMemorySize,
                                                                    &hSharedMem
                                                                   );
    METHOD_CHECKFAIL_EXIT(hr, "ERROR: WriteExclModeParams FAILED");
    
    spAudioDeviceEndpoint2 = m_pDevEndpoint;
    VERIFY_IS_NOT_NULL(spAudioDeviceEndpoint2);

    VERIFY_SUCCEEDED(spAudioDeviceEndpoint2->GetPinHandle(&hPin1.m_h));

    SLOG(eInfo1, "Initializing CPEndpoint");
    hr = CPClient->Initialize(hSharedMem, NULL, reinterpret_cast<UINT_PTR>(hPin1.m_h), NULL, NULL, 0);
    METHOD_CHECKFAIL_EXIT(hr, "ERROR:Initializing with latency = 0 FAILED");

    hr = CPClient->GetBufferSize(&u32NumFrames);
    METHOD_CHECKFAIL_EXIT(hr, "ERROR:GetBufferSize on CPClientEndpoint FAILED");

    u32ExpectedNumFrames = 100 * (pWfx->nSamplesPerSec / 1000); //Initialized with 100ms Buffer
    if(u32NumFrames != u32ExpectedNumFrames)
    { //Check if the num frames == 100ms worth of data, since WriteExcl was called with 100ms Buffer
        hr = E_FAIL;
        SLOG(eError, "ERROR: GetBufferSize returned the incorrect number of frames");
        SLOG(eError, "ERROR: Expected NumFrames %d, Returned NumFrames %d", u32ExpectedNumFrames, u32NumFrames);
        goto exit;
    }   

exit:
    return hr;
}


//-------------------------------------------------------------------------
// Class: CKSEndpointCaptureTest
// Method: CheckStandardExclusiveLoopedBufferSize
//
// Description:
//  API Test to check looped buffer size for Standard streaming
//
// Parameters:
//
// Return values (HRESULT):
//     Return S_OK on Success, approp error on Failure
//
HRESULT CKSEndpointCaptureTest::CheckStandardExclusiveLoopedBufferSize()
{
    HRESULT hr = S_OK;
    IAudioCrossProcessClientEndpointPtr CPClient = NULL;    
    UINT_PTR        hSharedMem = NULL;
    UINT32          u32cbSharedMemorySize = 0;
    HANDLE          hCreateParams = NULL;
    WAVEFORMATEX*   pWfx = NULL;
    UINT32          u32NumFrames = 0;
    UINT32          u32ExpectedNumFrames = 0;
    CComQIPtr<IAudioDeviceEndpoint2> spAudioDeviceEndpoint2;
    CHandle         hPin, hPin1;

    //UINT_PTR phSharedMutex = (UINT_PTR)&hSharedMutex;
    
    pWfx = GET_CAPTURE_WFX();

    //For CaptureEndpoints the following values are allowed for the looped buffersize
    //for STANDARD Streaming
    //  STANDARD_CAPTURE_BUFFER_COEFFICIENT 
    //                   6

    //Try initializing the endpoint with BufferSize < 6 quantums
    m_pDevEndpoint = NULL;
    hr = CreateEndpoint();
    METHOD_CHECKFAIL_EXIT(hr, "ERROR: Unable to Create DeviceEndpoint");
    hr = CPClient.CreateInstance(__uuidof(AudioKSCaptureEndpoint));
    METHOD_CHECKFAIL_EXIT(hr, "ERROR: Unable to Create CPClientEndpoint");

    //Write the Exclusive mode params to Shared Memory
    SLOG(eInfo1, "Calling DevEndpoint->WriteExclModeParams");
    hr = m_pDevEndpoint->WriteExclusiveModeParametersToSharedMemory((UINT_PTR)GetCurrentProcess(),
                                                                    10*MFTIME1MSEC, //10 msPeriod
                                                                    10*MFTIME1MSEC, //Buffer is 10ms
                                                                    1, //Latency
                                                                    &u32cbSharedMemorySize,
                                                                    &hSharedMem
                                                                   );
    METHOD_CHECKFAIL_EXIT(hr, "ERROR: WriteExclModeParams FAILED");

    spAudioDeviceEndpoint2 = m_pDevEndpoint;
    VERIFY_IS_NOT_NULL(spAudioDeviceEndpoint2);

    VERIFY_SUCCEEDED(spAudioDeviceEndpoint2->GetPinHandle(&hPin.m_h));
    
    SLOG(eInfo1, "Initializing CPEndpoint");
    hr = CPClient->Initialize(hSharedMem, NULL, reinterpret_cast<UINT_PTR>(hPin.m_h), NULL, NULL, 0);
    METHOD_CHECKFAIL_EXIT(hr, "ERROR:CPEndpoint Initialization FAILED");

    hr = CPClient->GetBufferSize(&u32NumFrames);
    METHOD_CHECKFAIL_EXIT(hr, "ERROR:GetBufferSize on CPClientEndpoint FAILED");

    //We should expect a buffer atleast equal to 6 periods
    u32ExpectedNumFrames = 6 * 10 * (pWfx->nSamplesPerSec / 1000);
    if(u32NumFrames != u32ExpectedNumFrames)
    { //Check if the num frames == 10ms worth of data, since WriteExcl was called with 10ms Buffer
        hr = E_FAIL;
        SLOG(eError, "ERROR: GetBufferSize returned the incorrect number of frames");
        SLOG(eError, "ERROR: Expected NumFrames%d, Returned NumFrames %d", u32ExpectedNumFrames, u32NumFrames);
        goto exit;
    }

    //Try initializing the endpoint with BufferSize == 100 quantums
    m_pDevEndpoint = NULL;    
    hr = CreateEndpoint();
    METHOD_CHECKFAIL_EXIT(hr, "ERROR: Unable to Create DeviceEndpoint");
    hr = CPClient.CreateInstance(__uuidof(AudioKSCaptureEndpoint));
    METHOD_CHECKFAIL_EXIT(hr, "ERROR: Unable to Create CPClientEndpoint");

    //Write the Exclusive mode params to Shared Memory
    SLOG(eInfo1, "Calling DevEndpoint->WriteExclModeParams");
    hr = m_pDevEndpoint->WriteExclusiveModeParametersToSharedMemory((UINT_PTR)GetCurrentProcess(),
                                                                    10*MFTIME1MSEC, //10 msPeriod
                                                                    100*MFTIME1MSEC, //Buffer is 100ms
                                                                    1, //Latency
                                                                    &u32cbSharedMemorySize,
                                                                    &hSharedMem
                                                                   );
    METHOD_CHECKFAIL_EXIT(hr, "ERROR: WriteExclModeParams FAILED");
    
    spAudioDeviceEndpoint2 = m_pDevEndpoint;
    VERIFY_IS_NOT_NULL(spAudioDeviceEndpoint2);

    VERIFY_SUCCEEDED(spAudioDeviceEndpoint2->GetPinHandle(&hPin1.m_h));

    SLOG(eInfo1, "Initializing CPEndpoint");
    hr = CPClient->Initialize(hSharedMem, NULL, reinterpret_cast<UINT_PTR>(hPin1.m_h), NULL, NULL, 0);
    METHOD_CHECKFAIL_EXIT(hr, "ERROR:CPEndpoint Initialization FAILED");

    hr = CPClient->GetBufferSize(&u32NumFrames);
    METHOD_CHECKFAIL_EXIT(hr, "ERROR:GetBufferSize on CPClientEndpoint FAILED");

    //We should expect a buffer equal to 100 msec
    u32ExpectedNumFrames = 100  * (pWfx->nSamplesPerSec / 1000);
    if(u32NumFrames != u32ExpectedNumFrames)
    { //Check if the num frames == 10ms worth of data, since WriteExcl was called with 10ms Buffer
        hr = E_FAIL;
        SLOG(eError, "ERROR: GetBufferSize returned the incorrect number of frames");
        SLOG(eError, "ERROR: Expected NumFrames%d, Returned NumFrames %d", u32ExpectedNumFrames, u32NumFrames);
        goto exit;
    }

exit:
    return hr;
}
