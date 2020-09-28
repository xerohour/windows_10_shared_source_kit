// Copyright(C) Microsoft. All rights reserved.

#include "stdafx.h"

#include "Util.h"

//KSRenderEndpoint supported interfaces
//  IAudioClock
//  IAudioCrossProcessClientEndpoint
//  IAudioDeviceEndpoint
//  IAudioEndpoint
//  IAudioEndpointControl
//  IAudioEndpointGlitch
//  IAudioEndpointRT
//  IAudioOutputEndpointRT

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSRenderEndpointCreate
//
// Description:
//  TestCase to test if KSRenderEndpoint class is registered and Endpoint interface can be obtained from it
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSRenderEndpointCreate()
{
    CKSEndpointRenderTest EndpointTest;
    return EndpointTest.EndpointCreate();
}

/******************************************************************************************************************/
/******************************         IAudioClock Tests                            ******************************/
/******************************************************************************************************************/
//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSRenderEndpointGetFrequency
//
// Description:
//  TestCase to test IAudioClock::GetFrequency API on the KSRenderEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSRenderEndpointGetFrequency()
{
    CKSEndpointRenderTest EndpointTest;
    return EndpointTest.EndpointGetFrequency();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSRenderEndpointGetPosition
//
// Description:
//  TestCase to test IAudioClock::GetPosition API on the KSRenderEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSRenderEndpointGetPosition()
{
    CKSEndpointRenderTest EndpointTest;
    return EndpointTest.EndpointGetPosition();
}

/******************************************************************************************************************/
/******************************         IAudioCrossProcessClientEndpoint Tests       ******************************/
/******************************************************************************************************************/
//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSRenderEndpointCPClientInitialize
//
// Description:
//  TestCase to test IAudioCrossProcessClientEndpoint::Initialize API on the KSRenderEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSRenderEndpointCPClientInitialize()
{
    CKSEndpointRenderTest EndpointTest;
    return EndpointTest.EndpointCPClientInitialize();
}

// Class: 
// Method: Tc_KSRenderEndpointSetBuffer
//
// Description:
//  TestCase to test IAudioDeviceEndpoint::SetBuffer API on the KSRenderEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSRenderEndpointSetBuffer()
{
    CKSEndpointRenderTest EndpointTest;
    return EndpointTest.EndpointSetBuffer();
}

// Class: 
// Method: Tc_KSRenderEndpointWriteExclusiveModeParametersToSharedMemory
//
// Description:
//  TestCase to test IAudioDeviceEndpoint::WriteExclusiveModeParametersToSharedMemory API on the KSRenderEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSRenderEndpointWriteExclusiveModeParametersToSharedMemory()
{
    CKSEndpointRenderTest EndpointTest;
    return EndpointTest.EndpointWriteExclusiveModeParametersToSharedMemory();
}

// Class: 
// Method: Tc_KSRenderEndpointGetRTCaps
//
// Description:
//  TestCase to test IAudioDeviceEndpoint::GetRTCaps API on the KSRenderEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSRenderEndpointGetRTCaps()
{
    CKSEndpointRenderTest EndpointTest;
    return EndpointTest.EndpointGetRTCaps();
}


/******************************************************************************************************************/
/******************************         IAudioEndpoint Tests                         ******************************/
/******************************************************************************************************************/
//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSRenderEndpointGetFrameFormat
//
// Description:
//  TestCase to test IAudioEndpoint::GetFrameFormat API on the KSRenderEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSRenderEndpointGetFrameFormat()
{
    CKSEndpointRenderTest EndpointTest;
    return EndpointTest.EndpointGetFrameFormat();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSRenderEndpointGetLatency
//
// Description:
//  TestCase to test IAudioEndpoint::GetLatency API on the KSRenderEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSRenderEndpointGetLatency()
{
    CKSEndpointRenderTest EndpointTest;
    return EndpointTest.EndpointGetLatency();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSRenderEndpointGetFramesPerPacket
//
// Description:
//  TestCase to test IAudioEndpoint::GetFramesPerPacket API on the KSRenderEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSRenderEndpointGetFramesPerPacket()
{
    CKSEndpointRenderTest EndpointTest;
    return EndpointTest.EndpointGetFramesPerPacket();
}


/******************************************************************************************************************/
/******************************         IAudioEndpointControl Tests                  ******************************/
/******************************************************************************************************************/
//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSRenderEndpointReset
//
// Description:
//  TestCase to test IAudioEndpointControl::Reset API on the KSRenderEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSRenderEndpointReset()
{
    CKSEndpointRenderTest EndpointTest;
    return EndpointTest.EndpointReset();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSRenderEndpointStart
//
// Description:
//  TestCase to test IAudioEndpointControl::Start API on the KSRenderEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSRenderEndpointStart()
{
    CKSEndpointRenderTest EndpointTest;
    return EndpointTest.EndpointStart();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSRenderEndpointStop
//
// Description:
//  TestCase to test IAudioEndpointControl::Stop API on the KSRenderEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSRenderEndpointStop()
{
    CKSEndpointRenderTest EndpointTest;
    return EndpointTest.EndpointStop();
}

/******************************************************************************************************************/
/******************************         IAudioEndpointGlitch Tests                       ******************************/
/******************************************************************************************************************/
//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSRenderEndpointGetGlitchCount
//
// Description:
//  TestCase to test IAudioClock::GetGlitchCount API on the KSRenderEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSRenderEndpointGetGlitchCount()
{
    CKSEndpointRenderTest EndpointTest;
    return EndpointTest.EndpointGetGlitchCount();
}

/******************************************************************************************************************/
/******************************         IAudioEndpointRT Tests                       ******************************/
/******************************************************************************************************************/
//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSRenderEndpointGetCurrentPadding
//
// Description:
//  TestCase to test IAudioClock::GetCurrentPadding API on the KSRenderEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSRenderEndpointGetCurrentPadding()
{
    CKSEndpointRenderTest EndpointTest;
    return EndpointTest.EndpointGetCurrentPadding();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSRenderEndpointGetCurrentTimeStamp
//
// Description:
//  TestCase to test IAudioClock::GetCurrentTimeStamp API on the KSRenderEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSRenderEndpointGetCurrentTimeStamp()
{
    CKSEndpointRenderTest EndpointTest;
    return EndpointTest.EndpointGetTimeStamp();
}

/******************************************************************************************************************/
/******************************         IAudioOutputEndpointRT Tests                 ******************************/
/******************************************************************************************************************/
//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSRenderEndpointGetOutputDataPointer
//
// Description:
//  TestCase to test IAudioOutputEndpointRT::GetOutputDataPointer API on the KSRenderEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSRenderEndpointGetOutputDataPointer()
{
    CKSEndpointRenderTest EndpointTest;
    return EndpointTest.EndpointGetOutputDataPointer();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSRenderEndpointReleaseOutputDataPointer
//
// Description:
//  TestCase to test IAudioOutputEndpointRT::ReleaseOutputDataPointer API on the KSRenderEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSRenderEndpointReleaseOutputDataPointer()
{
    CKSEndpointRenderTest EndpointTest;
    return EndpointTest.EndpointReleaseOutputDataPointer();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSRenderEndpointTestPositions
//
// Description:
//  TestCase to check that the Render Endpoint returns the right positions
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSRenderEndpointTestPositions()
{
    CKSEndpointRenderTest EndpointTest;
    return EndpointTest.EndpointTestPositions();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSRenderEndpointStream
//
// Description:
//  TestCase to check that the Endpoint streams properly
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_KSRenderEndpointStream()
{
    CKSEndpointRenderTest EndpointTest;
    return EndpointTest.EndpointStream();
}


/******************************************************************************************************************/
/******************************         IAudioProtectedOutput Tests                 ******************************/
/******************************************************************************************************************/

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSRenderEndpointStream
//
// Description:
//  TestCase to test IAudioProtectedOutput::GetProtectedOutputController on the KSRenderEndpoint
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//-------------------------------------------------------------------------
DWORD Tc_KSRenderEndpointGetProtectedOutputController()
{
    CKSEndpointRenderTest EndpointTest;
    return EndpointTest.EndpointGetProtectedOutputController();
}



/******************************************************************************************************************/
/******************************         IAudioProtectedOutputController Tests        ******************************/
/******************************************************************************************************************/
//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSRenderEndpointCreateVirtualProtectedOutput
//
// Description:
//  TestCase to test IAudioProtectedOutputController::CreateVirtualProtectedOutput
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//-------------------------------------------------------------------------
DWORD Tc_KSRenderEndpointCreateVirtualProtectedOutput()
{
    CKSEndpointRenderTest EndpointTest;
    return EndpointTest.EndpointCreateVirtualProtectedOutput();
}


/******************************************************************************************************************/
/******************************         IAudioVirtualProtectedOutput Tests           ******************************/
/******************************************************************************************************************/
//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSRenderEndpointGetDescription
//
// Description:
//  TestCase to test IAudioVirtualProtectedOutput::GetDescription
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//-------------------------------------------------------------------------
DWORD Tc_KSRenderEndpointGetDescription()
{
    CKSEndpointRenderTest EndpointTest;
    return EndpointTest.EndpointGetDescription();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_KSRenderEndpointSetPolicySchemas
//
// Description:
//  TestCase to test IAudioVirtualProtectedOutput::SetPolicySchemas
//  
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//-------------------------------------------------------------------------
DWORD Tc_KSRenderEndpointSetPolicySchemas()
{
    CKSEndpointRenderTest EndpointTest;
    return EndpointTest.EndpointSetPolicySchemas();
}


//DWORD CAudioEndpointKSTest::AudioEndpointKSCreate()
//{
//    HRESULT hr = S_OK;    
//    IAudioEndpointRTPtr pEndpoint = NULL;
//
//    SLOG(eInfo1, "Creating Audio FileReadEndpoint");
//    hr = pEndpoint.CreateInstance(__uuidof(AudioFileReadEndpoint));
//    if(SUCCEEDED(hr))
//    {
//        SLOG(eInfo1, "Succeeded in creating AudioFileReadEndpoint");
//        return FNS_PASS;
//    }
//    else
//    {
//        SLOG(eError,"FAILED trying to create AudioFileReadEndpointKS, error: 0x%X", hr);
//        return FNS_FAIL;
//    }    
//}

//-------------------------------------------------------------------------
// Class: CKSEndpointRenderTest
// Method: InitializeAndSetBuffer
//
// Description:
//  This method initializes the RenderEndpoint, It gets the appropriate symbolic name for the Render Device
//  and then calls Initialize on the Endpoint
//  The method then sets the buffer on the endpoint with the preferred format and a period of 10 msec
//  
// Parameters:
//      LPCWSTR szPnpID
//              The PnpID of the device that the endpoint is to be initialized with. It would be generally obtained
//              from the funtion instance object. The default value is NULL in which case the method proceeds with
//              the PnpID of the default Render device
//      PROPVARIANT* pCreatParams
//              The Parameters (pinId and Format) that the device is to be initialized with.
//              Default is NULL in which case the device is initialized with the default render pin and the device's
//              preferred render format.
//
// Return values (HRESULT):
//     Return S_OK on Success, appropriate error code on Failure
//
HRESULT CKSEndpointRenderTest::InitializeAndSetBuffer(LPCWSTR szPnpID, PROPVARIANT* pCreateParams, MFTIME mfBufferTime)
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
    hr = SetDeviceBuffer(mfBufferTime, GET_RENDER_WFX());
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
// Class: CKSEndpointRenderTest
// Method: EndpointGetOutputDataPointer
//
// Description:
//  TestCase for IAudioOutputEndpointRT::EndpointGetOutputDataPointer API 
//
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CKSEndpointRenderTest::EndpointGetOutputDataPointer()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_FAIL;
    _ASSERT(m_pDevEndpoint != NULL);
    SLOG(eError, "Test NOT IMPLEMENTED");

    return dw;
}

//-------------------------------------------------------------------------
// Class: CKSEndpointRenderTest
// Method: EndpointReleaseOutputDataPointer
//
// Description:
//  TestCase for IAudioOutputEndpointRT::EndpointReleaseOutputDataPointer API 
//
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CKSEndpointRenderTest::EndpointReleaseOutputDataPointer()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_FAIL;
    _ASSERT(m_pDevEndpoint != NULL);
    SLOG(eError, "Test NOT IMPLEMENTED");

    return dw;
}

//-------------------------------------------------------------------------
// Class: CKSEndpointRenderTest
// Method: EndpointReleaseOutputDataPointer
//
// Description:
//  Functional test to check if we can stream data to a KsRenderEndpoint
//  This test directly creates a KsRenderEndpoint and streams data to it
//  with just a Volume APO in the graph
//
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CKSEndpointRenderTest::EndpointStream()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;
    CComObject<CMemoryAudioEndpoint>* pInMemEndpoint = new CComObject<CMemoryAudioEndpoint>();
    FLOAT32* pfSineWave = NULL;
    IAudioEndpointRTPtr pPumpEndpoint = NULL;
    IAudioEndpointRTPtr pInputEndpoint = NULL;
    CGraphRepresentation graph;
    MFTIME mfPumpPeriod = 20 * MFTIME1MSEC;
    MFTIME mfPumpBudget = 0;
    AUDIO_FRAME_FORMAT frameFormatIn;
    AUDIO_FRAME_FORMAT frameFormatOut;    
    LPWAVEFORMATEX lpWfx = NULL;
    UINT32 u32NumFrames = 0; 
    UINT32 u32InBufferSize = NULL;
    LPWSTR lpwszDevice = NULL;
    ULONG  ulEndpointType = 0; 
    CComPtr<IMMDevice> pIMMEndpoint = NULL;
    CComPtr<IBasicLog> pBasicLog;
    WAVEFORMATEXTENSIBLE wfxExt;

    _ASSERT(m_pDevEndpoint != NULL);    
    _ASSERT(pInMemEndpoint != NULL);

    if (NULL == m_pDevEndpoint)
    {
        SLOG(eError, "ERROR: NULL == m_pDevEndpoint");
        return (FNS_FAIL);
    }
 
    if (NULL == pInMemEndpoint)
    {
        SLOG(eError, "ERRROR: NULL == pInMemEndpoint");
        return (FNS_FAIL);
    }

    //Build the In and Out formats
    lpWfx = GET_RENDER_WFX(); //The output has to be 16bit, if not the following will break
    BUILD_FORMAT(&frameFormatIn, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, lpWfx->nChannels, 4, 32, lpWfx->nSamplesPerSec);
    BUILD_FORMAT(&frameFormatOut, KSDATAFORMAT_SUBTYPE_PCM, lpWfx->nChannels, lpWfx->nBlockAlign/lpWfx->nChannels, lpWfx->wBitsPerSample, lpWfx->nSamplesPerSec);
    u32NumFrames = (UINT32)((lpWfx->nSamplesPerSec * 20) / 1000);// 20 ms of data at the devices preferred frame rate.
    //
    // Graph descriptors
    // apo's for the graph
    GRAPH_APO_DESCRIPTOR aApoDescriptors[] = { { APO_FMT_f32i16, NULL } };
	//
    // Bindings between apo's and connections
    // use only one connection
    GRAPH_BINDING_DESCRIPTOR aBindingDescriptors[] = { { 0, 0, true  }, { 0, 1, false } };
	//
    // Connection descriptors
    GRAPH_CONNECTION_DESCRIPTOR aConnectionDescriptors[2]; 
    ZeroMemory(aConnectionDescriptors, sizeof(aConnectionDescriptors[0])*2);
    CopyMemory(&aConnectionDescriptors[0].FrameFormat, &frameFormatIn, sizeof(frameFormatIn));
    CopyMemory(&aConnectionDescriptors[1].FrameFormat, &frameFormatOut, sizeof(frameFormatOut));    

    //
    // Calculate connection max number of frames
    aConnectionDescriptors[0].ConnectionDescriptor.u32MaxFrameCount =  u32NumFrames;
    aConnectionDescriptors[1].ConnectionDescriptor.u32MaxFrameCount =  u32NumFrames;

    //Allocate memory for stream data
    u32InBufferSize = u32NumFrames * frameFormatIn.u32BytesPerSampleContainer * frameFormatIn.u32SamplesPerFrame;
    pfSineWave = (FLOAT32*)LocalAlloc(LPTR, u32InBufferSize);
    if(pfSineWave == NULL)
    {
        SLOG(eError, "Memory allocation for Endpoint Buffers FAILED");
        dw = FNS_FAIL;
        goto exit;
    }

    FORMAT2WAVEXT(&frameFormatIn, &wfxExt);
    
    hr = g_IShell->GetBasicLog(&pBasicLog);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Could not get a basic logger", dw, FNS_FAIL);
 
    hr = FillBufferWithSineSignal(
        pBasicLog, XFAIL,
        &wfxExt.Format,
        1.0, // amplitude
        1000.0, // frequency
        0.0, // initial phase,
        0.0, // dc
        Method_NoDithering,
        reinterpret_cast<PBYTE>(pfSineWave),
        u32NumFrames,
        u32InBufferSize
    );
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Could not fill buffer", dw, FNS_FAIL);

    hr = m_pDevEndpoint->QueryInterface(&pPumpEndpoint);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Unable to QI IAudioEndpointRT from KsRenderEndpoint", dw, FNS_FAIL);
    hr = pInMemEndpoint->QueryInterface(&pInputEndpoint);

    //Initialize the Input and Output Endpoints
    hr = pInMemEndpoint->Initialize(&frameFormatIn, pfSineWave, u32InBufferSize, u32NumFrames, true);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Unable to Initialize Mem Input Endpoint", dw, FNS_FAIL);
    //Set the Input buffer to loop
    pInMemEndpoint->SetLooped();    
    //Initialize KSEndpoint
    lpwszDevice = GetDeviceInfo(true)->GetPnpId();
    pIMMEndpoint = GetDeviceInfo(true)->GetIMMDevice();
//    hr = m_pDevEndpoint->Initialize(pIMMEndpoint, lpwszDevice, GetDefaultPin(true), (ULONG)GetAvEndpointType(lpwszDevice), GET_RENDER_WFX());
    hr = pIMMEndpoint->Activate(__uuidof(IAudioDeviceEndpoint), CLSCTX_ALL, NULL, (void**)&m_pDevEndpoint);
    METHOD_CHECKFAIL_EXIT(hr, "ERROR: Initializing KSRender Endpoint FAILED");
    //SetBuffer on KS Endpoint
    SLOG(eInfo1, "Calling SetBuffer on the Endpoint");
    hr = m_pDevEndpoint->SetBuffer(mfPumpPeriod, 0/*Minimum latency*/); 
    METHOD_CHECKFAIL_EXIT(hr, "ERROR: SetBuffer on the KSEndpoint FAILED");

    //Initialize Local graph
    graph.Initialize(aConnectionDescriptors, 2,
                     aBindingDescriptors, 2,
                     aApoDescriptors, 1,
                     u32NumFrames,
                     u32NumFrames,
                     pPumpEndpoint,
                     1000,
                     pInputEndpoint);

    //Build the graph
    hr = graph.BuildGraph();
    METHOD_CHECKFAIL_EXIT(hr, "ERROR: Building of Local Graph FAILED");
    //Initialize the Pump for this graph
    mfPumpBudget = 2 * MFTIME1MSEC;
    hr = graph.InitializePump(&mfPumpPeriod, &mfPumpBudget);
    METHOD_CHECKFAIL_EXIT(hr, "ERROR: FAILED to initialize Pump for local graph");

    //Start Streaming
    hr = graph.Start();
    METHOD_CHECKFAIL_EXIT(hr, "ERROR: Starting of LocalGraph FAILED");
    Sleep(2000); //Sleep 500 msec;
    hr = graph.Stop();
    METHOD_CHECKFAIL_EXIT(hr, "ERROR: Stoping of LocalGraph FAILED");
    
exit:
    SAFE_FREE_POINTER(pfSineWave); 
    return dw;
}

//-------------------------------------------------------------------------
// Class: CKSEndpointRenderTest
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
HRESULT CKSEndpointRenderTest::CheckExclusiveLoopedBufferSize()
{
    HRESULT hr = S_OK;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // CURRENTLY REALTIME ENDPOINTS ARE NOT ENABLED, SO THE REALTIME SECTION OF 
    // THE FOLLOWING IS COMMENTED OUT
    // UNCOMMENT THE FOLLOWING WHEN REALTIME ENDPOINTS ARE ENABLED
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    if(GetModule()->IsRt(m_bRender))
    { //Realtime streaming
        hr = CheckRealtimeExclusiveLoopedBufferSize();
    }
    else
    { //Standard streaming
        hr = CheckStandardExclusiveLoopedBufferSize();
    }

    return hr;
}

//-------------------------------------------------------------------------
// Class: CKSEndpointRenderTest
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
HRESULT CKSEndpointRenderTest::CheckRealtimeExclusiveLoopedBufferSize()
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
    
    SLOG(eInfo1, "Testing real time exclusive mode looped buffer size");
    //For RenderEndpoints the following values are allowed for the looped buffersize
    //for REALTIME Streaming
    // greater than or equal to REALTIME_RENDER_MIN_LATENCY_COEFFICIENT
    //                                      1
    
    pWfx = GET_RENDER_WFX();
    //Try initializing the endpoint with latency = 0
    m_pDevEndpoint = NULL;
    hr = CreateEndpoint();
    METHOD_CHECKFAIL_EXIT(hr, "ERROR: Unable to Create DeviceEndpoint");    

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
    
    hr = CPClient.CreateInstance(__uuidof(AudioKSRenderEndpoint));
    METHOD_CHECKFAIL_EXIT(hr, "ERROR: Unable to Create CPClientEndpoint");

    spAudioDeviceEndpoint2 = m_pDevEndpoint;
    VERIFY_IS_NOT_NULL(spAudioDeviceEndpoint2);

    VERIFY_SUCCEEDED(spAudioDeviceEndpoint2->GetPinHandle(&hPin.m_h));

    SLOG(eInfo1, "Initializing when latency is 0");
    hr = CPClient->Initialize(hSharedMem, NULL, reinterpret_cast<UINT_PTR>(hPin.m_h), NULL, NULL, 0);
    METHOD_CHECKFAIL_EXIT(hr, "ERROR:Initializing with latency = 0 FAILED");

    hr = CPClient->GetBufferSize(&u32NumFrames);
    METHOD_CHECKFAIL_EXIT(hr, "ERROR:GetBufferSize on CPClientEndpoint FAILED");

    u32ExpectedNumFrames = 10 * (pWfx->nSamplesPerSec / 1000);
    if(u32NumFrames < u32ExpectedNumFrames)
    { //Check if the num frames == 10ms worth of data, since WriteExcl was called with 10ms Buffer
        hr = E_FAIL;
        SLOG(eError, "ERROR: GetBufferSize returned the incorrect number of frames");
        SLOG(eError, "ERROR: Expected NumFrames%d, Returned NumFrames %d", u32ExpectedNumFrames, u32NumFrames);
        goto exit;
    }
    SLOG(eInfo1, "Buffer size (frames) is sufficient: buffer granted = %d, buffer expected = %d", u32NumFrames, u32ExpectedNumFrames);
    
    //Try min latency and the Buffer more than 1 quantum
    m_pDevEndpoint = NULL;
    hr = CreateEndpoint();
    METHOD_CHECKFAIL_EXIT(hr, "ERROR: Unable to Create DeviceEndpoint");
    
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
    
    hr = CPClient.CreateInstance(__uuidof(AudioKSRenderEndpoint));
    METHOD_CHECKFAIL_EXIT(hr, "ERROR: Unable to Create CPClientEndpoint");
    
    spAudioDeviceEndpoint2 = m_pDevEndpoint;
    VERIFY_IS_NOT_NULL(spAudioDeviceEndpoint2);

    VERIFY_SUCCEEDED(spAudioDeviceEndpoint2->GetPinHandle(&hPin1.m_h));

    SLOG(eInfo1, "Initializing CPEndpoint");
    hr = CPClient->Initialize(hSharedMem, NULL, reinterpret_cast<UINT_PTR>(hPin1.m_h), NULL, NULL, 0);
    METHOD_CHECKFAIL_EXIT(hr, "ERROR:Initializing with latency = 0 FAILED");

    hr = CPClient->GetBufferSize(&u32NumFrames);
    METHOD_CHECKFAIL_EXIT(hr, "ERROR:GetBufferSize on CPClientEndpoint FAILED");

    u32ExpectedNumFrames = 100 * (pWfx->nSamplesPerSec / 1000); //Initialized with 100ms Buffer
    if(u32NumFrames < u32ExpectedNumFrames)
    { //Check if the num frames == 100ms worth of data, since WriteExcl was called with 100ms Buffer
        hr = E_FAIL;
        SLOG(eError, "ERROR: GetBufferSize returned the incorrect number of frames");
        SLOG(eError, "ERROR: Expected NumFrames %d, Returned NumFrames %d", u32ExpectedNumFrames, u32NumFrames);
        goto exit;
    }
    SLOG(eInfo1, "Buffer size (frames) is sufficient: buffer granted = %d, buffer expected = %d", u32NumFrames, u32ExpectedNumFrames);
    
exit:
    return hr;
}


//-------------------------------------------------------------------------
// Class: CKSEndpointRenderTest
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
HRESULT CKSEndpointRenderTest::CheckStandardExclusiveLoopedBufferSize()
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
        
    SLOG(eInfo1, "Testing standard exclusive mode looped buffer size");
    
    pWfx = GET_RENDER_WFX();

    //For RenderEndpoints the following values are allowed for the looped buffersize
    //for STANDARD Streaming
    //  STANDARD_RENDER_MIN_LATENCY_COEFFICIENT * STANDARD_RENDER_BUFFER_COEFFICIENT 
    //                   2                                  4   

    //Try initializing the endpoint with latency < 2 and BufferSize == quantum
    m_pDevEndpoint = NULL;
    hr = CreateEndpoint();
    METHOD_CHECKFAIL_EXIT(hr, "ERROR: Unable to Create DeviceEndpoint");   

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
    
    hr = CPClient.CreateInstance(__uuidof(AudioKSRenderEndpoint));
    METHOD_CHECKFAIL_EXIT(hr, "ERROR: Unable to Create CPClientEndpoint");

    spAudioDeviceEndpoint2 = m_pDevEndpoint;
    VERIFY_IS_NOT_NULL(spAudioDeviceEndpoint2);

    VERIFY_SUCCEEDED(spAudioDeviceEndpoint2->GetPinHandle(&hPin.m_h));

    SLOG(eInfo1, "Initializing CPEndpoint");
    hr = CPClient->Initialize(hSharedMem, NULL, reinterpret_cast<UINT_PTR>(hPin.m_h), NULL, NULL, 0);
    METHOD_CHECKFAIL_EXIT(hr, "ERROR:CPEndpoint Initialization FAILED");

    hr = CPClient->GetBufferSize(&u32NumFrames);
    METHOD_CHECKFAIL_EXIT(hr, "ERROR:GetBufferSize on CPClientEndpoint FAILED");

    //We should expect a buffer atleast equal to 8 periods
    u32ExpectedNumFrames = 8 * 10 * (pWfx->nSamplesPerSec / 1000);
    if(u32NumFrames != u32ExpectedNumFrames)
    { //Check if the num frames == 10ms worth of data, since WriteExcl was called with 10ms Buffer
        hr = E_FAIL;
        SLOG(eError, "ERROR: GetBufferSize returned the incorrect number of frames");
        SLOG(eError, "ERROR: Expected NumFrames%d, Returned NumFrames %d", u32ExpectedNumFrames, u32NumFrames);
        goto exit;
    }
    
    SLOG(eInfo1, "Buffer size (frames) is sufficient: buffer granted = %d, buffer expected = %d", u32NumFrames, u32ExpectedNumFrames);
    
    //Try initializing the endpoint with latency = 2 and BufferSize == 100 quantums
    m_pDevEndpoint = NULL;
    hr = CreateEndpoint();
    METHOD_CHECKFAIL_EXIT(hr, "ERROR: Unable to Create DeviceEndpoint");    

    //Write the Exclusive mode params to Shared Memory
    SLOG(eInfo1, "Calling DevEndpoint->WriteExclModeParams");
    hr = m_pDevEndpoint->WriteExclusiveModeParametersToSharedMemory((UINT_PTR)GetCurrentProcess(),
                                                                    10*MFTIME1MSEC, //10 msPeriod
                                                                    100*MFTIME1MSEC, //Buffer is 100ms
                                                                    2, //Latency
                                                                    &u32cbSharedMemorySize,
                                                                    &hSharedMem
                                                                   );
    METHOD_CHECKFAIL_EXIT(hr, "ERROR: WriteExclModeParams FAILED");
    
    hr = CPClient.CreateInstance(__uuidof(AudioKSRenderEndpoint));
    METHOD_CHECKFAIL_EXIT(hr, "ERROR: Unable to Create CPClientEndpoint");

    spAudioDeviceEndpoint2 = m_pDevEndpoint;
    VERIFY_IS_NOT_NULL(spAudioDeviceEndpoint2);

    VERIFY_SUCCEEDED(spAudioDeviceEndpoint2->GetPinHandle(&hPin1.m_h));

    SLOG(eInfo1, "Initializing CPEndpoint");
    hr = CPClient->Initialize(hSharedMem, NULL, reinterpret_cast<UINT_PTR>(hPin1.m_h), NULL, NULL, 0);
    METHOD_CHECKFAIL_EXIT(hr, "ERROR:CPEndpoint Initialization FAILED");

    hr = CPClient->GetBufferSize(&u32NumFrames);
    METHOD_CHECKFAIL_EXIT(hr, "ERROR:GetBufferSize on CPClientEndpoint FAILED");

    //We should expect a buffer equal to 10 periods (or 100 msec)
    u32ExpectedNumFrames = 100 * (pWfx->nSamplesPerSec / 1000);
    if(u32NumFrames != u32ExpectedNumFrames)
    { //Check if the num frames == 10ms worth of data, since WriteExcl was called with 10ms Buffer
        hr = E_FAIL;
        SLOG(eError, "ERROR: GetBufferSize returned the incorrect number of frames");
        SLOG(eError, "ERROR: Expected NumFrames%d, Returned NumFrames %d", u32ExpectedNumFrames, u32NumFrames);
        goto exit;
    }
    SLOG(eInfo1, "Buffer size (frames) is sufficient: buffer granted = %d, buffer expected = %d", u32NumFrames, u32ExpectedNumFrames);
    
exit:
    return hr;
}//CKSEndpointRenderTest::CheckStandardExclusiveLoopedBufferSize


//-------------------------------------------------------------------------
// Class: CKSEndpointRenderTest
// Method: EndpointGetOutputDataPointer
//
// Description:
//  TestCase for IAudioProtectedOutput::GetProtectedOutputController API 
//
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//-------------------------------------------------------------------------
DWORD CKSEndpointRenderTest::EndpointGetProtectedOutputController()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;

    UINT    uiAction = 0;
    IAudioProtectedOutputPtr ProtectedOutput = NULL;
    IAudioProtectedOutputControllerPtr  ProtectedOutputController = NULL;
    bool bOffloadCapable = false;

    _ASSERT(m_pDevEndpoint != NULL);    

    if (NULL == m_pDevEndpoint)
    {
        SLOG(eError, "ERROR: NULL == m_pDevEndpoint");
        return (FNS_FAIL);
    }

    //Get the IAudioProtectedOutput interface
    SLOG(eInfo1, "Querying IAudioProtectedOutput from KSEndpoint");
    hr = m_pDevEndpoint.QueryInterface(__uuidof(IAudioProtectedOutput), &ProtectedOutput);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:QueryInterface for IAudioProtectedOutput FAILED", dw, FNS_FAIL);

    SLOG(eInfo1, "Calling GetPOC with NULL params");
    hr = ProtectedOutput->GetProtectedOutputController(uiAction, NULL);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: GetPOC SUCCEEDED with NULL POC pointer", dw, FNS_FAIL);

    SLOG(eInfo1, "Calling GetPOC with invalid Action");
    uiAction = (UINT)-1;
    hr = ProtectedOutput->GetProtectedOutputController(uiAction, &ProtectedOutputController);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: GetPOC SUCCEEDED with invalid Action", dw, FNS_FAIL);


    //Make valid calls
    //Get POC for ACTION PLAY
    SLOG(eInfo1, "Calling GetPOC with Action PEACTION_PLAY");
    uiAction = (UINT)PEACTION_PLAY;
    hr = ProtectedOutput->GetProtectedOutputController(uiAction, &ProtectedOutputController);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: GetPOC FAILED for PEACTION_PLAY", dw, FNS_FAIL);

    //  Checking if endpoint is Offload capable...
    {
        CComPtr<IMMDevice> pMMDevice = NULL;
        CComPtr<IAudioClient2> pAudioClient2 = NULL;
        BOOL bCapable;

        hr = GetIMMDevice(&pMMDevice);
        METHOD_CHECKFAIL_EXIT(hr, "ERROR: GetIMMDevice FAILED");
    
        // Need to enable hardware acceleration for the endpoint if applicable and allow classic apps for offload
        CClassicOffloadEnabler cor;
        CHardwareAccelerationEnabler har(pMMDevice);
        
        hr = pMMDevice->Activate(__uuidof(IAudioClient2), CLSCTX_ALL, NULL, (void**)&pAudioClient2);
        METHOD_CHECKFAIL_EXIT(hr, "ERROR: Activate IAudioClient2 FAILED");
        
        hr = pAudioClient2->IsOffloadCapable(AudioCategory_Media, &bCapable);
        METHOD_CHECKFAIL_EXIT(hr, "ERROR: IAudioClient2::IsOffloadCapable FAILED");
        
        bOffloadCapable = ((FALSE != bCapable)?true:false);
    }

    //Get POC for ACTION EXTRACT. KSRenderEndpoint only supports PLAY Action
    SLOG(eInfo1, "Calling GetPOC with Action PEACTION_EXTRACT");
    uiAction = (UINT)PEACTION_EXTRACT;
    hr = ProtectedOutput->GetProtectedOutputController(uiAction, &ProtectedOutputController);

    if (bOffloadCapable)
    {
        METHOD_CHECKFAIL_SETVAL(hr, "ERROR: GetPOC FAILED for PEACTION_EXTRACT", dw, FNS_FAIL);
    }
    else
    {
        METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: GetPOC SUCCEEDED for PEACTION_EXTRACT", dw, FNS_FAIL);
    }

exit:
    return dw;
}//CKSEndpointRenderTest::EndpointGetProtectedOutputController


//-------------------------------------------------------------------------
// Class: CKSEndpointRenderTest
// Method: EndpointCreateVirtualProtectedOutput
//
// Description:
//  TestCase for IAudioProtectedOutputController::CreateVirtualProtectedOutput API 
//
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//-------------------------------------------------------------------------
DWORD CKSEndpointRenderTest::EndpointCreateVirtualProtectedOutput()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;

    UINT    uiAction = 0;
    IAudioProtectedOutputPtr ProtectedOutput = NULL;
    IAudioProtectedOutputControllerPtr  ProtectedOutputController = NULL;
    IAudioVirtualProtectedOutputPtr AudioVPO = NULL;
    IAudioProcessingObjectPtr   pConstrictorApo = NULL;


    _ASSERT(m_pDevEndpoint != NULL);    
    
    if (NULL == m_pDevEndpoint)
    {
        SLOG(eError, "ERROR: NULL == m_pDevEndpoint");
        return (FNS_FAIL);
    }

    //Get the IAudioProtectedOutput interface
    SLOG(eInfo1, "Querying IAudioProtectedOutput from KSEndpoint");
    hr = m_pDevEndpoint.QueryInterface(__uuidof(IAudioProtectedOutput), &ProtectedOutput);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:QueryInterface for IAudioProtectedOutput FAILED", dw, FNS_FAIL);    

    //GetPOC for PEACTION_PLAY
    uiAction = (UINT)PEACTION_PLAY;
    SLOG(eInfo1, "Calling GetPOC for PEACTION_PLAY");
    hr = ProtectedOutput->GetProtectedOutputController(uiAction, &ProtectedOutputController);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: GetPOC for PEACTION_PLAY FAILED", dw, FNS_FAIL);

    //Create Constrictor APO, required by CreateVPO.
    SLOG(eInfo1, "Create constrictor APO required by POC");
    hr = pConstrictorApo.CreateInstance(__uuidof(AudioConstrictor));
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: Unable to create AudioConstrictor object", dw, FNS_FAIL);
    
    //CreateVirtualProtectedOutput with NULL params
    SLOG(eInfo1, "Calling CreateVirtualProtectedOutput with NULL param");
    hr = ProtectedOutputController->CreateVirtualProtectedOutput(NULL, NULL);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: CreateVPO with NULL param sSUCCEEDED", dw, FNS_FAIL);

    SLOG(eInfo1, "Calling CreateVirtualProtectedOutput with NULL VPO");
    hr = ProtectedOutputController->CreateVirtualProtectedOutput(pConstrictorApo, NULL);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: CreateVPO with NULL VPO SUCCEEDED", dw, FNS_FAIL);

    //CreateVirtualProtectedOutput can succeed with a NULL APO, this would only happen
    //in exclusive mode, or if there was a failure instantiating the constrictor APO.
    //However the KSEndpoint doesn't know of any of these facts and does accept a NULL APO
    SLOG(eInfo1, "Calling CreateVirtualProtectedOutput with NULL APO");
    hr = ProtectedOutputController->CreateVirtualProtectedOutput(NULL, &AudioVPO);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: CreateVPO with NULL APO SUCCEEDED", dw, FNS_FAIL);


    SLOG(eInfo1, "Calling CreateVirtualProtectedOutput with valid param");
    hr = ProtectedOutputController->CreateVirtualProtectedOutput(pConstrictorApo, &AudioVPO);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: CreateVPO with valid param FAILED", dw, FNS_FAIL);

exit:
    return dw;
}//CKSEndpointRenderTest::EndpointCreateVirtualProtectedOutput



//-------------------------------------------------------------------------
// Class: CKSEndpointRenderTest
// Method: EndpointGetDescription
//
// Description:
//  TestCase for IAudioVirtualProtectedOutput::GetDescription API 
//
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//-------------------------------------------------------------------------
DWORD CKSEndpointRenderTest::EndpointGetDescription()
{
    DWORD dw = FNS_PASS;
    HRESULT hr = S_OK;
    IAudioVirtualProtectedOutputPtr AudioVPO = NULL;
    UINT    uiAction = PEACTION_NO;
    DWORD   dwAttributes = 0;
    GUID    guidOutputSubType = GUID_NULL;
    ULONG   ulProtectionSchemasSupported = 0;
    GUID    *pguidSchemasSupported = NULL;

    _ASSERT(NULL != m_pDevEndpoint);

    //Set Action to PEACTION_PLAY
    uiAction = (UINT)PEACTION_PLAY;
    
    SLOG(eInfo1, "GetAudioVPO from KSRenderEndpoint");
    hr = GetAudioVPO(uiAction, &AudioVPO);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Unable to GetAudioVPO from KSRenderEndpoint", dw, FNS_FAIL);

    SLOG(eInfo1, "Calling GetDescription on AudioVPO with NULL/invalid params");    
    hr = AudioVPO->GetDescription(NULL, NULL, NULL, NULL);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: GetDescription with all NULL params SUCCEEDED", dw, FNS_FAIL);

    SLOG(eInfo1, "Calling GetDescription on AudioVPO with NULL pdwAttributes");    
    hr = AudioVPO->GetDescription(NULL, &guidOutputSubType, &ulProtectionSchemasSupported, &pguidSchemasSupported);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: GetDescription with NULL pdwAttributes", dw, FNS_FAIL);

    SLOG(eInfo1, "Calling GetDescription on AudioVPO with NULL guidOutputSubType");    
    hr = AudioVPO->GetDescription(&dwAttributes, NULL, &ulProtectionSchemasSupported, &pguidSchemasSupported);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: GetDescription with NULL guidOutputSubType SUCCEEDED", dw, FNS_FAIL);

    SLOG(eInfo1, "Calling GetDescription on AudioVPO with NULL ulSchemasSupported");    
    hr = AudioVPO->GetDescription(&dwAttributes, &guidOutputSubType, NULL, &pguidSchemasSupported);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: GetDescription with NULL ulSchemasSupported SUCCEEDED", dw, FNS_FAIL);

    SLOG(eInfo1, "Calling GetDescription on AudioVPO with NULL ppguidSchemasSupported");    
    hr = AudioVPO->GetDescription(&dwAttributes, &guidOutputSubType, &ulProtectionSchemasSupported, NULL);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: GetDescription with NULL ppguidSchemasSupported SUCCEEDED", dw, FNS_FAIL);

    SLOG(eInfo1, "Calling GetDescription on AudioVPO with valid Params");
    hr = AudioVPO->GetDescription(&dwAttributes, &guidOutputSubType, &ulProtectionSchemasSupported, &pguidSchemasSupported);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: GetDescription with valid params FAILED", dw, FNS_FAIL);

    //Release IAudioDeviceEndpoint and call GetDescription
    m_pDevEndpoint = NULL;
    SLOG(eInfo1, "Calling GetDescription on AudioVPO with valid Params on released DeviceEndpoint");    
    hr = AudioVPO->GetDescription(&dwAttributes, &guidOutputSubType, &ulProtectionSchemasSupported, &pguidSchemasSupported);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: GetDescription with valid params FAILED", dw, FNS_FAIL);

exit:
    return dw;
}//CKSEndpointRenderTest::EndpointGetDescription



//-------------------------------------------------------------------------
// Class: CKSEndpointRenderTest
// Method: EndpointSetPolicySchemas
//
// Description:
//  TestCase for IAudioVirtualProtectedOutput::SetPolicySchemas API 
//
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//-------------------------------------------------------------------------
DWORD CKSEndpointRenderTest::EndpointSetPolicySchemas()
{
    DWORD dw = FNS_PASS;
    HRESULT hr = S_OK;
    IAudioVirtualProtectedOutputPtr AudioVPO = NULL;
    UINT    uiAction = PEACTION_NO;
    ULONG   ulcSchemas = 0;
    GUID    guidSchema = GUID_NULL;
    DWORD   dwSchemeData = 0;

    _ASSERT(NULL != m_pDevEndpoint);

    //Set Action to PEACTION_PLAY
    uiAction = (UINT)PEACTION_PLAY;
    
    SLOG(eInfo1, "GetAudioVPO from KSRenderEndpoint");
    hr = GetAudioVPO(uiAction, &AudioVPO);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Unable to GetAudioVPO from KSRenderEndpoint", dw, FNS_FAIL);

    SLOG(eInfo1, "SetPolicySchema with null/invalid params");
    hr = AudioVPO->SetPolicySchemas(ulcSchemas, NULL, NULL);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: SetPolicySchema with NULL/invalid params SUCCEEDED", dw, FNS_FAIL);

    SLOG(eInfo1, "SetPolicySchema with null prgSchemaGuid");
    hr = AudioVPO->SetPolicySchemas(ulcSchemas, NULL, &dwSchemeData);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: SetPolicySchema with NULL prgSchemaGUID SUCCEEDED", dw, FNS_FAIL);

    SLOG(eInfo1, "SetPolicySchema with null pdwSchemeData");
    hr = AudioVPO->SetPolicySchemas(ulcSchemas, &guidSchema, NULL);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: SetPolicySchema with NULL pdwSchemeData SUCCEEDED", dw, FNS_FAIL);

    SLOG(eInfo1, "SetPolicySchema with valid params");
    hr = AudioVPO->SetPolicySchemas(ulcSchemas, &guidSchema, &dwSchemeData);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: SetPolicySchema with valid params FAILED", dw, FNS_FAIL);

    //Release IAudioDeviceEndpoint and call SetPolicySchema
    m_pDevEndpoint = NULL;
    SLOG(eInfo1, "SetPolicySchema with valid params");
    hr = AudioVPO->SetPolicySchemas(ulcSchemas, &guidSchema, &dwSchemeData);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: SetPolicySchema with valid params FAILED", dw, FNS_FAIL);

exit:
    return dw;
}//CKSEndpointRenderTest::EndpointSetPolicySchemas


//-------------------------------------------------------------------------
// Class: CKSEndpointRenderTest
// Method: GetAudioVPO
//
// Description:
//  Method to get IAudioProtectedOutput from KSRenderEndpoint and eventually return an AudioVPO
//
// Parameters:
//
// Return values (HRESULT):
//     Return S_OK on SUCCESS, approp error code on FAILURE
//-------------------------------------------------------------------------
HRESULT CKSEndpointRenderTest::GetAudioVPO(UINT uiAction, IAudioVirtualProtectedOutput** ppAudioVPO)
{
    HRESULT hr = S_OK;
    DWORD   dw = FNS_PASS;

    _ASSERT(m_pDevEndpoint != NULL);
    IAudioProtectedOutputPtr ProtectedOutput = NULL;
    IAudioProtectedOutputControllerPtr ProtectedOutputController = NULL;
    IAudioProcessingObjectPtr  pConstrictorApo = NULL;

    if (NULL == m_pDevEndpoint)
    {
        SLOG(eError, "ERROR: NULL == m_pDevEndpoint");
        return (E_FAIL);
    }

    //Get the IAudioProtectedOutput interface
    SLOG(eInfo1, "Querying IAudioProtectedOutput from KSEndpoint");
    hr = m_pDevEndpoint.QueryInterface(__uuidof(IAudioProtectedOutput), &ProtectedOutput);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:QueryInterface for IAudioProtectedOutput FAILED", dw, FNS_FAIL);    

    //GetPOC    
    SLOG(eInfo1, "Calling GetPOC");
    hr = ProtectedOutput->GetProtectedOutputController(uiAction, &ProtectedOutputController);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: GetPOC for PEACTION_PLAY FAILED", dw, FNS_FAIL);

    //Create a constrictor APO for POC
    SLOG(eInfo1, "Create instance of Constrictor APO, required by POC");
    hr = pConstrictorApo.CreateInstance(__uuidof(AudioConstrictor));
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Unable to create constrictor APO", dw, FNS_FAIL);
    
    //CreateVirtualProtectedOutput
    SLOG(eInfo1, "Calling CreateVirtualProtectedOutput");
    hr = ProtectedOutputController->CreateVirtualProtectedOutput(pConstrictorApo, ppAudioVPO);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: CreateVPO FAILED", dw, FNS_FAIL);

exit:
    return hr;
}//CKSEndpointRenderTest::GetAudioVPO
