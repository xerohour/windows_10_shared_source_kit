// Copyright(C) Microsoft. All rights reserved.

#include "stdafx.h"


//-------------------------------------------------------------------------
// Class: 
// Method: Tc_AudioPumpCreate
//
// Description:
//  TestCase to test if AudioPump class is registered and IAudioPump interface can be obtained from it
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_AudioPumpCreate()
{   
    CAudioPumpTest AudioPumpTest;
    return AudioPumpTest.AudioPumpCreate();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_AudioPumpInitializeParent
//
// Description:
//  TestCase to test InitializeParent method on IAudioPump
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_AudioPumpInitialize()
{
    CAudioPumpTest AudioPumpTest;
    return AudioPumpTest.AudioPumpInitialize();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_AudioPumpInitializeDependant
//
// Description:
//  TestCase to test InitializeDependant method on IAudioPump
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
//DWORD Tc_AudioPumpInitializeDependant()
//{
//    CAudioPumpTest AudioPumpTest;
//    return AudioPumpTest.AudioPumpInitializeDependant();
//}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_AudioPumpStart
//
// Description:
//  TestCase to test Start method on IAudioPump
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_AudioPumpStart()
{
    CAudioPumpTest AudioPumpTest;
    return AudioPumpTest.AudioPumpStart();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_AudioPumpStop
//
// Description:
//  TestCase to test Stop method on IAudioPump
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_AudioPumpStop()
{
    CAudioPumpTest AudioPumpTest;
    return AudioPumpTest.AudioPumpStop();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_AudioPumpGetRegisteredClock
//
// Description:
//  TestCase to test GetRegisteredClock method on IAudioPump
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
//DWORD Tc_AudioPumpGetRegisteredClock()
//{
//    CAudioPumpTest AudioPumpTest;
//    return AudioPumpTest.AudioPumpGetRegisteredClock();
//}
//-------------------------------------------------------------------------
// Class: 
// Method: Tc_AudioPumpReconfigureTimer
//
// Description:
//  TestCase to test ReconfigureTimer method on IAudioPump
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_AudioPumpReconfigureTimer()
{
    CAudioPumpTest AudioPumpTest;
    return AudioPumpTest.AudioPumpReconfigureTimer();
}

//-------------------------------------------------------------------------
// Class: CAudioPumpTest
// Method: CreatePump
//
// Description:
//  This method creates an instance of the AudioPump and assigns it to the internal variable
//   
// Parameters:
// Return values (HRESULT):
//     Return S_OK on Success, Error on Failure
//
HRESULT CAudioPumpTest::CreatePump()
{
    HRESULT hr = S_OK;
    ASSERTION(TEXT("Creating Audio Pump"));
    hr = m_pAudioPump.CreateInstance(__uuidof(AudioPump));
    METHODCHECKFAIL("CreateInstance for AudioPump", hr);    
    return hr;
}

//-------------------------------------------------------------------------
// Class: CAudioPumpTest
// Method: CreateRenderEndpoint
//
// Description:
//  In the future sometime this method will create a AudioRenderEndpoint
//   
// Parameters:
//      IAudioEndPointRT* ppAudioRenderEndpoint
//      Argument the receives an initialized AudioRenderEndpoint
// Return values (HRESULT):
//     Return S_OK on Success, Error on Failure
//

HRESULT CAudioPumpTest::CreateRenderEndpoint(IAudioEndpointRTPtr* ppAudioRenderEndpoint)
{
    HRESULT hr = E_FAIL;
    
    return hr;
}

//-------------------------------------------------------------------------
// Class: CAudioPumpTest
// Method: InitializePump
//
// Description:
//  This method intializes the internal pump
//   
// Parameters:
//      LPVOID  pBuffer
//      Buffer that is passed to AudioEndpoint
//      UINT32  cbBuffer
//      Size in bytes of the buffer
// Return values (HRESULT):
//     Return S_OK on Success, Error on Failure
//
HRESULT CAudioPumpTest::InitializePump(LPVOID pBuffer, UINT32 cbBuffer)
{ 
    HRESULT hr = S_OK;
    IAudioProcessRTPtr pIAudioProcessor = NULL;
    IAudioEndpointRTPtr pIAudEndpointRT = NULL;
    CComObject<CMemoryAudioEndpoint>* pAudioEndpoint = NULL;    
    DWORD dwPeriod = 10 * MFTIME1MSEC;
    ASSERTION("Initializing Pump with Valid Params");
    _ASSERT(m_pAudioPump != NULL);           
	DWORD dwBudget = dwPeriod/5;
    UINT32 ui32PumpCreationFlags = AUDIOPUMP_NONE;
	GUID guidParent = GUID_NULL;

    if (SUCCEEDED(hr))
    {
        ASSERTION("Trying to create Audio Processor");
        hr = pIAudioProcessor.CreateInstance(__uuidof(AudioProcessor));
        METHODCHECKFAIL("CreateInstance for AudioProcessor", hr);
        if(FAILED(hr))
        {            
            return hr;
        }
    }     
    // Create the IAudioEndPointRT  object    
    if (SUCCEEDED(hr))
    {
        ASSERTION("Creating MemoryEndpoint");
        pAudioEndpoint = new CComObject<CMemoryAudioEndpoint>();    
        if(!pAudioEndpoint)
        {
            hr = E_OUTOFMEMORY;
            METHODCHECKFAIL("MemoryEndpoint Creation", hr);
            goto exit;
        }
        ASSERTION("Trying to Initialize MemoryEndpoint");        
        AUDIO_FRAME_FORMAT AudioFormat;
        BUILD_FORMAT(&AudioFormat, KSDATAFORMAT_SUBTYPE_PCM, 1, 2, 16, 44100);
        hr = pAudioEndpoint->Initialize(&AudioFormat, pBuffer, cbBuffer, 
                          cbBuffer/(AudioFormat.u32SamplesPerFrame * AudioFormat.u32BytesPerSampleContainer),
                                 FALSE); // Output Endpoint        
        METHODCHECKFAIL("MemoryEndpoint Initialization", hr);
        if(FAILED(hr))
        {            
            goto exit;
        }
        pAudioEndpoint->AddRef();
        hr = pAudioEndpoint->QueryInterface(__uuidof(IAudioEndpointRT), (void**)&pIAudEndpointRT);
        if(FAILED(hr))
        {            
            goto exit;
        }
    }

    // We have the Processor, Endpoint and the Clock, Initialize the Pump
    ASSERTION("Trying to Initialize Parent Reserve for the Pump");
    ui32PumpCreationFlags = AUDIOPUMP_ENDPOINT_OUTPUT;
    hr = m_pAudioPump->Initialize(ui32PumpCreationFlags, 
                                  dwPeriod,
								  GUID_NULL,
                                  pIAudioProcessor, 
                                  pIAudEndpointRT);
    METHODCHECKFAIL("InitializeParent", hr);

exit:
    SAFE_RELEASE_POINTER(pAudioEndpoint);    
    return hr;
}

//-------------------------------------------------------------------------
// Class: CAudioPumpTest
// Method: AudioPumpInitialize
//
// Description:
//  This is the test case for InitializeParent Method call
//  This method iterates through the different valid/invalid values for the arguments
//   
// Parameters:
// Return values (HRESULT):
//     Return S_OK on Success, Error on Failure
//

DWORD CAudioPumpTest::AudioPumpInitialize()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;      

    IAudioProcessRTPtr pIAudioProcessor = NULL;    
    IAudioEndpointRTPtr pIAudioEndpointRT = NULL; //Use a Memory Endpoint    
    DWORD dwPeriod = 0;	
    FLOAT32 *pfOutput = NULL;
    UINT32 cbBufferSize = 8; //One frame of FLOAT32 stereo data
    UINT32 u32NumFrames = 1;
    UINT32 ui32PumpCreationFlags = AUDIOPUMP_NONE;

    _ASSERT(m_pAudioPump != NULL);

    
    // Create the Processor Object
    SLOG(eInfo1, "Creating Audio Processor");
    if (SUCCEEDED(hr))
    {
        hr = pIAudioProcessor.CreateInstance(__uuidof(AudioProcessor));
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, CreateInstance for AudioProcessor FAILED", dw, FNS_FAIL);
    }

    // Create the IAudioEndPointRT object    
    SLOG(eInfo1, "Creating Memory(Output) Endpoint");
    CComObject<CMemoryAudioEndpoint>* pAudioEndpoint = new CComObject<CMemoryAudioEndpoint>();
    
    _ASSERT(pAudioEndpoint);
    //Initialize Endpoints
    AUDIO_FRAME_FORMAT AudioFormat;
    BUILD_FORMAT(&AudioFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 2, 4, 32, 48000);
    
    SLOG(eInfo1, "Initializing Output Endpoint");
    pfOutput = new FLOAT32[8];
    _ASSERT(pfOutput);    
    hr = pAudioEndpoint->Initialize(&AudioFormat, pfOutput, cbBufferSize, u32NumFrames, FALSE);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Output Endpoint Initialization FAILED", dw, FNS_FAIL);
    
    //Query Output Endpoint
    SLOG(eInfo1, "Querying Endpoint for IAudioEndpointRT");
    hr = pAudioEndpoint->QueryInterface(__uuidof(IAudioEndpointRT), (void**)&pIAudioEndpointRT);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Querying for EndpointRT from Output Endpoint FAILED", dw, FNS_FAIL);

    dwPeriod = 20 * MFTIME1MSEC; //Run every 20 msecs    
    ui32PumpCreationFlags = AUDIOPUMP_ENDPOINT_OUTPUT; // we are using OutputEndpoint

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\\
    //Try NULL  and INVALID parameters for each argument
    ASSERTION("Trying with NULL and invalid parameters");
    //Try All NULL    
    ASSERTION("Trying with ALL NULL parameters");
    hr = m_pAudioPump->Initialize(NULL, NULL, GUID_NULL, NULL, NULL);
    if (SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR:Succeeded with NULL Params, 0x%X", hr);
        m_pAudioPump = NULL; //the pointer is smart
        dw = FNS_FAIL;
        hr = CreatePump();
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
    }        
    
    //Try NULL Period
    ASSERTION("Trying with NULL dwPeriod");
    hr = m_pAudioPump->Initialize(ui32PumpCreationFlags, NULL, GUID_NULL, pIAudioProcessor, pIAudioEndpointRT);
    if (SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR:Succeeded with NULL dwPeriod, 0x%X", hr);
        m_pAudioPump = NULL; //the pointer is smart
        dw = FNS_FAIL;
        hr = CreatePump();
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
    }
    
    //Try NULL AudioProcessor
    ASSERTION("Trying with NULL AudioProcessor");
    hr = m_pAudioPump->Initialize(ui32PumpCreationFlags, dwPeriod, GUID_NULL, NULL, pIAudioEndpointRT);
    if (SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR:Succeeded with NULL IAudioProcessRT*, 0x%X", hr);
        m_pAudioPump = NULL; //the pointer is smart        
        dw = FNS_FAIL;
        hr = CreatePump();
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
    }
    
    //Try NULL AudioEndPoint
    ASSERTION("Trying with NULL AudioEndPoint");
    hr = m_pAudioPump->Initialize(ui32PumpCreationFlags, dwPeriod, GUID_NULL, pIAudioProcessor, NULL);
    if (SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR:Succeeded with NULL IAudioEndPointRT*, 0x%X", hr);
        m_pAudioPump = NULL; //the pointer is smart
        hr = CreatePump();
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\\
    //Try Double Initialization   
    ASSERTION("Trying Double Initialization");
    //First Initialization with all valid parameters, Should SUCCEED
    ASSERTION("Trying First Initialize, should SUCCEED");
    dwPeriod = 20 * MFTIME1MSEC; //run every 20 msecs	
    hr = m_pAudioPump->Initialize(ui32PumpCreationFlags, dwPeriod, GUID_NULL, pIAudioProcessor, pIAudioEndpointRT);
    if (FAILED(hr))
    {
        SLOG(eError, "ERROR:Initialize FAILED with all valid Params 0x%X", hr);
        dw = FNS_FAIL;
    }       
    else
    {
        // Try the second initialization, it should FAIL here.
        ASSERTION("Trying Second Initialize, should FAIL");
        hr = m_pAudioPump->Initialize(ui32PumpCreationFlags, dwPeriod, GUID_NULL, pIAudioProcessor, pIAudioEndpointRT);
        METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Double Initializion SUCCEEDED", dw, FNS_FAIL);          	
    }
    // At this point Audio Pump is probably initialized if the above calls went fine, 
    // So we should release m_pAudioPump to get rid of Pump and 
    // restart testing from this point after calling CreatePump to get a brand new IAudioPump        
    m_pAudioPump = NULL; //the pointer is smart        
    hr = CreatePump();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\\
    //Try Different Pump Creation Flags
    ASSERTION("Trying Different Pump Creation Flags");
    ui32PumpCreationFlags = AUDIOPUMP_NONE;
    ASSERTION("Trying AUDIOPUMP_NONE creation flag");
    hr = m_pAudioPump->Initialize(ui32PumpCreationFlags, dwPeriod, GUID_NULL, pIAudioProcessor, pIAudioEndpointRT);
    if (SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: AudioPump Initialization SUCCEEDED with AUDIOPUMP_NONE Creation Flags, 0x%X", hr);
        dw = FNS_FAIL;
        // The call succeeded, Release Pump, and create a new one            
        m_pAudioPump = NULL;
        hr = CreatePump();
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
    }
    
    //Flags have to be one of AUDIO_PUMP_BEFORE or AFTER
    ASSERTION("Trying AUDIOPUMP_THREAD_BEFORE and AUDIOPUMP_THREAD_AFTER creation flag");
    ui32PumpCreationFlags = AUDIOPUMP_THREAD_BEFORE | AUDIOPUMP_THREAD_AFTER | AUDIOPUMP_ENDPOINT_OUTPUT;    
    hr = m_pAudioPump->Initialize(ui32PumpCreationFlags, dwPeriod, GUID_NULL, pIAudioProcessor, pIAudioEndpointRT);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: AudioPump Initialization SUCCEEDED with AUDIOPUMP_THREAD_BEFORE and AUDIOPUMP_THREAD_AFTER Creation Flags, 0x%X", hr);
        dw = FNS_FAIL;
        // The call succeeded, Release Pump, and create a new one            
        m_pAudioPump = NULL;
        hr = CreatePump();
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
    }

    //A valid call needs one of either INPUT or OUTPUT flag.
    ASSERTION("Trying AUDIOPUMP_ENDPOINT_INPUT and AUDIOPUMP_ENDPOINT_OUTPUT creation flag");
    ui32PumpCreationFlags = AUDIOPUMP_ENDPOINT_INPUT | AUDIOPUMP_ENDPOINT_OUTPUT | AUDIOPUMP_THREAD_BEFORE;
    hr = m_pAudioPump->Initialize(ui32PumpCreationFlags, dwPeriod, GUID_NULL, pIAudioProcessor, pIAudioEndpointRT);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: AudioPump Initialization SUCCEEDED with AUDIOPUMP_ENDPOINT_INPUT and AUDIOPUMP_ENDPOINT_OUTPUT Creation Flags, 0x%X", hr);
        dw = FNS_FAIL;
        // The call succeeded, Release Pump, and create a new one
        m_pAudioPump = NULL;
        hr = CreatePump();
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
    }

    // Try Individual Flags, i.e. each of BEFORE/AFTER/OUTPUT/INPUT by themselves
    // These are all invalid cases
    ASSERTION("Trying ony AUDIOPUMP_THREAD_BEFORE creation flag");
    ui32PumpCreationFlags = AUDIOPUMP_THREAD_BEFORE;
    hr = m_pAudioPump->Initialize(ui32PumpCreationFlags, dwPeriod, GUID_NULL, pIAudioProcessor, pIAudioEndpointRT);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: AudioPump Initialization SUCCEEDED with only AUDIOPUMP_THREAD_BEFORE Creation Flags, 0x%X", hr);
        dw = FNS_FAIL;
        // The call succeeded, Release Pump, and create a new one
        m_pAudioPump = NULL;
        hr = CreatePump();
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
    }

    ASSERTION("Trying ony AUDIOPUMP_THREAD_AFTER creation flag");
    ui32PumpCreationFlags = AUDIOPUMP_THREAD_AFTER;
    hr = m_pAudioPump->Initialize(ui32PumpCreationFlags, dwPeriod, GUID_NULL, pIAudioProcessor, pIAudioEndpointRT);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: AudioPump Initialization SUCCEEDED with only AUDIOPUMP_THREAD_AFTER Creation Flags, 0x%X", hr);
        dw = FNS_FAIL;
        // The call succeeded, Release Pump, and create a new one
        m_pAudioPump = NULL;
        hr = CreatePump();
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
    }

    ASSERTION("Trying ony AUDIOPUMP_ENDPOINT_INPUT creation flag");
    ui32PumpCreationFlags = AUDIOPUMP_ENDPOINT_INPUT;
    hr = m_pAudioPump->Initialize(ui32PumpCreationFlags, dwPeriod, GUID_NULL, pIAudioProcessor, pIAudioEndpointRT);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: AudioPump Initialization SUCCEEDED with only AUDIOPUMP_THREAD_INPUT Creation Flags, 0x%X", hr);
        dw = FNS_FAIL;
        // The call succeeded, Release Pump, and create a new one
        m_pAudioPump = NULL;
        hr = CreatePump();
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
    }

    ASSERTION("Trying ony AUDIOPUMP_ENDPOINT_OUTPUT creation flag");
    ui32PumpCreationFlags = AUDIOPUMP_ENDPOINT_OUTPUT;
    hr = m_pAudioPump->Initialize(ui32PumpCreationFlags, dwPeriod, GUID_NULL, pIAudioProcessor, pIAudioEndpointRT);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: AudioPump Initialization SUCCEEDED with only AUDIOPUMP_THREAD_OUTPUT Creation Flags, 0x%X", hr);
        dw = FNS_FAIL;
        // The call succeeded, Release Pump, and create a new one
        m_pAudioPump = NULL;
        hr = CreatePump();
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
    }
    
    //Try Valid tests with Output Endpoint
    //Try AUDIOPUMP_THREAD_BEFORE flag with ENDPOINT_OUTPUT, this should succeed
    ASSERTION("Trying AUDIOPUMP_THREAD_BEFORE creation flag on ENDPOINT_OUTPUT");    
    ui32PumpCreationFlags = AUDIOPUMP_THREAD_BEFORE | AUDIOPUMP_ENDPOINT_OUTPUT;
    SLOG(eError, "FAILURE EXPECTED: Windows OS Bugs: 1127510");
    hr = m_pAudioPump->Initialize(ui32PumpCreationFlags, dwPeriod, GUID_NULL, pIAudioProcessor, pIAudioEndpointRT);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Initializing AudioPump with AUDIOPUMP_THREAD_BEFORE Creation Flags (ENDPOINT_OUTPUT) FAILED, 0x%X", hr);    	
        dw = FNS_FAIL;
        // The call succeeded, Release Pump, and create a new one 
        m_pAudioPump = NULL;
        hr = CreatePump();
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
    }
    //Try AUDIOPUMP_THREAD_AFTER flag with ENDPOINT_OUTPUT, this should succeed
    ASSERTION("Trying AUDIOPUMP_THREAD_AFTER creation flag on ENDPOINT_OUTPUT");    
    ui32PumpCreationFlags = AUDIOPUMP_THREAD_AFTER | AUDIOPUMP_ENDPOINT_OUTPUT;
    SLOG(eError, "FAILURE EXPECTED: Windows OS Bugs: 1127510");
    hr = m_pAudioPump->Initialize(ui32PumpCreationFlags, dwPeriod, GUID_NULL, pIAudioProcessor, pIAudioEndpointRT);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Initializing AudioPump with AUDIOPUMP_THREAD_AFTER Creation Flags (ENDPOINT_OUTPUT) FAILED, 0x%X", hr);    	
        dw = FNS_FAIL;
        // The call succeeded, Release Pump, and create a new one 
        m_pAudioPump = NULL;
        hr = CreatePump();
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
    }

    //Try Valid tests with Input Endpoint
    //Reinitialize the Endpoint as Input
    SLOG(eInfo1, "Initializing Input Endpoint");
    hr = pAudioEndpoint->Initialize(&AudioFormat, pfOutput, cbBufferSize, u32NumFrames, TRUE);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Input Endpoint Initialization FAILED", dw, FNS_FAIL);    

    //Try AUDIOPUMP_THREAD_BEFORE flag with ENDPOINT_INPUT, this should succeed
    ASSERTION("Trying AUDIOPUMP_THREAD_BEFORE creation flag on ENDPOINT_INPUT");
    ui32PumpCreationFlags = AUDIOPUMP_THREAD_BEFORE | AUDIOPUMP_ENDPOINT_INPUT;
    SLOG(eError, "FAILURE EXPECTED: Windows OS Bugs: 1127510");
    hr = m_pAudioPump->Initialize(ui32PumpCreationFlags, dwPeriod, GUID_NULL, pIAudioProcessor, pIAudioEndpointRT);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Initializing AudioPump with AUDIOPUMP_THREAD_BEFORE Creation Flags (ENDPOINT_INPUT) FAILED, 0x%X", hr);    	
        dw = FNS_FAIL;
        // The call succeeded, Release Pump, and create a new one 
        m_pAudioPump = NULL;
        hr = CreatePump();
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
    }    
    //Try AUDIOPUMP_THREAD_AFTER flag with ENDPOINT_INPUT, this should succeed
    ASSERTION("Trying AUDIOPUMP_THREAD_AFTER creation flag on ENDPOINT_INPUT");
    ui32PumpCreationFlags = AUDIOPUMP_THREAD_AFTER | AUDIOPUMP_ENDPOINT_INPUT;
    SLOG(eError, "FAILURE EXPECTED: Windows OS Bugs: 1127510");
    hr = m_pAudioPump->Initialize(ui32PumpCreationFlags, dwPeriod, GUID_NULL, pIAudioProcessor, pIAudioEndpointRT);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Initializing AudioPump with AUDIOPUMP_THREAD_AFTER Creation Flags (ENDPOINT_INPUT) FAILED, 0x%X", hr);    	
        dw = FNS_FAIL;
        // The call succeeded, Release Pump, and create a new one 
        m_pAudioPump = NULL;
        hr = CreatePump();
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
    }    

    ui32PumpCreationFlags = AUDIOPUMP_ENDPOINT_INPUT;

    //
    // TRY DIFFERENT VALUES FOR PERIOD, WHAT ARE THE INVALID VALUES ????????????

    ASSERTION("Trying different values for Time Period");
    ASSERTION("Initializing Pump with Period = 0");
    dwPeriod = 0;    
    hr = m_pAudioPump->Initialize(ui32PumpCreationFlags, dwPeriod, GUID_NULL, pIAudioProcessor, pIAudioEndpointRT);    
    if (SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: Error Initializing Pump with Period = 0 SUCCEEDED, 0x%X", hr);
        dw = FNS_FAIL;
        // The call succeeded, Release Pump, and create a new one            
        m_pAudioPump = NULL;
        hr = CreatePump();
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);                
    }
    
    ASSERTION("Initializing Pump with Period = 56789 (Not an integral multiple of msec)");
    dwPeriod = 56789;
    hr = m_pAudioPump->Initialize(ui32PumpCreationFlags, dwPeriod, GUID_NULL, pIAudioProcessor, pIAudioEndpointRT);
    if (FAILED(hr))
    {
        SLOG(eError, "ERROR: Error Initializing Pump with Period = 56789, 0x%X", hr);
        dw = FNS_FAIL;
    }
    else
    {    // The call succeeded, Release Pump, and create a new one            
        m_pAudioPump = NULL;
        hr = CreatePump();
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
    }
    
    ASSERTION("Initializing Pump with Period = MAX_UINT64 (Boundary Condition)");
    dwPeriod = 0x7FFFFFFF;
    hr = m_pAudioPump->Initialize(ui32PumpCreationFlags, dwPeriod, GUID_NULL, pIAudioProcessor, pIAudioEndpointRT);
    if (FAILED(hr))
    {
        SLOG(eError, "ERROR: Error Initializing Pump with Period = MAX_UINT64 FAILED, 0x%X", hr);
        dw = FNS_FAIL;
    }
    else
    {    // The call succeeded, Release Pump, and create a new one            
        m_pAudioPump = NULL;
        hr = CreatePump();
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);                
    }

    ASSERTION("Initializing Pump with sub msec period");
    dwPeriod = 1;
    hr = m_pAudioPump->Initialize(ui32PumpCreationFlags, dwPeriod, GUID_NULL, pIAudioProcessor, pIAudioEndpointRT);
    if (FAILED(hr))
    {
        SLOG(eError, "ERROR: Error Initializing Pump with sub msec period FAILED, 0x%X", hr);
        dw = FNS_FAIL;
    }
    else
    {    // The call succeeded, Release Pump, and create a new one            
        m_pAudioPump = NULL;
        hr = CreatePump();
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);                
    }    

exit:
    SAFE_DELETE_ARRAY(pfOutput);
    return dw;
}


//-------------------------------------------------------------------------
// Class: CAudioPumpTest
// Method: AudioPumpInitializeDependant
//
// Description:
//  This is the test case for InitializeDependant Method call
//  This method iterates through the different valid/invalid values for the arguments
//   
// Parameters:
// Return values (HRESULT):
//     Return S_OK on Success, Error on Failure
//

//DWORD CAudioPumpTest::AudioPumpInitializeDependant()
//{
//    HRESULT hr = S_OK;
//    DWORD dw = FNS_PASS;
//
//    // Try for different values of IMFClock, IAudioProcess, Flags    
//
//    IAudioProcessRTPtr pIAudioProcessor = NULL;    
//    IAudioEndpointRTPtr pIAudioEndpointRT = NULL;    //Use a Memory Endpoint
//    CComPtr<IAVReserveMgr>    pAVReserveMgr = NULL;
//	CComPtr<IAVReserve>       pAVReserve = NULL;
//    _ASSERT(m_pAudioPump != NULL);
//    MFTIME dwPeriod = 0;
//	MFTIME dwBudget = 0;
//	GUID guidParent = GUID_NULL;
//    // {F586F050-58DB-4333-9AF1-67E9143DE688}
//    static const GUID guidTemp = 
//        { 0xf586f050, 0x58db, 0x4333, { 0x9a, 0xf1, 0x67, 0xe9, 0x14, 0x3d, 0xe6, 0x88 } };
//
//    FLOAT32 *pfOutput = NULL;
//    UINT32 cbBufferSize = 8; //One frame of FLOAT32 stereo data
//    UINT32 u32NumFrames = 1;
//    UINT32 ui32PumpCreationFlags = AUDIOPUMP_NONE;
//    DWORD  dwScaleFactor = 0;
//    DWORD  dwReserveCreateFlags = AV_RESERVE_PERIODIC | AV_RESERVE_PARENT;    
//
//    // Create the Processor Object
//    SLOG(eInfo1, "Creating Audio Processor");
//    if (SUCCEEDED(hr))
//    {
//        hr = pIAudioProcessor.CreateInstance(__uuidof(AudioProcessor));
//        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, CreateInstance for AudioProcessor FAILED", dw, FNS_FAIL);
//    }   
//    
//    // Create the IAudioEndPointRT object    
//    SLOG(eInfo1, "Creating Memory(Output) Endpoint");
//    CComObject<CMemoryAudioEndpoint>* pAudioEndpoint = new CComObject<CMemoryAudioEndpoint>();
//    _ASSERT(pAudioEndpoint);
//    
//    //Initialize Endpoints
//    AUDIO_FRAME_FORMAT AudioFormat;
//    BUILD_FORMAT(&AudioFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 2, 4, 32, 48000);
//    
//    SLOG(eInfo1, "Initializing Output Endpoint");
//    pfOutput = new FLOAT32[8];
//    _ASSERT(pfOutput);    
//    hr = pAudioEndpoint->Initialize(&AudioFormat, pfOutput, cbBufferSize, u32NumFrames, FALSE);
//    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Output Endpoint Initialization FAILED", dw, FNS_FAIL);
//    
//    //Query Output Endpoint
//    SLOG(eInfo1, "Querying Endpoint for IAudioEndpointRT");
//    hr = pAudioEndpoint->QueryInterface(__uuidof(IAudioEndpointRT), (void**)&pIAudioEndpointRT);
//    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Querying for EndpointRT from Output Endpoint FAILED", dw, FNS_FAIL);
//
//    dwPeriod = 20 * MFTIME1MSEC; //Run every 20 msecs
//    dwBudget = dwPeriod / 5; //Run for 4 msecs
//    ui32PumpCreationFlags = AUDIOPUMP_ENDPOINT_OUTPUT; // we are using OutputEndpoint
//
//    //Create AVReserve
//    hr = pAVReserveMgr.CoCreateInstance(__uuidof(AVReserveMgr));
//    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR creating AVReserveMgr", dw, FNS_FAIL);    
//    
//    hr = pAVReserveMgr->CreateReserve(NULL, &AUDIOENGINE_AVRESERVE_CLASS, dwPeriod, dwBudget, dwReserveCreateFlags, 0, &pAVReserve);
//    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Create Reserve FAILED", dw, FNS_FAIL);
//
//    hr = pAVReserve->get_ReserveId(&guidParent);
//    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Failed to get ReserveId from the AVReserve", dw, FNS_FAIL);
//
//    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\\
//    //Try NULL  and INVALID parameters for each argument
//    ASSERTION("Trying with NULL and invalid parameters");
//    //Try All NULL    
//    ASSERTION("Trying with ALL NULL parameters");
//    hr = m_pAudioPump->InitializeDependant(NULL, NULL, NULL, NULL, NULL, NULL, NULL);
//    if (SUCCEEDED(hr))
//    {
//        SLOG(eError, "ERROR:Succeeded with NULL Params, 0x%X", hr);
//        m_pAudioPump = NULL; //the pointer is smart
//        hr = CreatePump();
//        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
//        dw = FNS_FAIL;
//    }        
//    //Try NULL MFTIME
//    ui32PumpCreationFlags = AUDIOPUMP_THREAD_AFTER | AUDIOPUMP_ENDPOINT_OUTPUT | AUDIOPUMP_REALTIME;
//    dwScaleFactor = 1;
//    dwPeriod = 20 * MFTIME1MSEC; //Run every 20 msecs
//    dwBudget = 2 * MFTIME1MSEC; //Run for 2 msec
//    ASSERTION("Trying with NULL dwPeriod");
//    hr = m_pAudioPump->InitializeDependant(ui32PumpCreationFlags, NULL, dwBudget, dwScaleFactor, pIAudioProcessor, pIAudioEndpointRT, &guidParent);
//    if (SUCCEEDED(hr))
//    {
//        SLOG(eError, "ERROR:Succeeded with NULL dwPeriod, 0x%X", hr);
//        m_pAudioPump = NULL; //the pointer is smart
//        hr = CreatePump();
//        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
//        dw = FNS_FAIL;
//    }
//    ASSERTION("Trying with NULL dwBudget");
//    hr = m_pAudioPump->InitializeDependant(ui32PumpCreationFlags, dwPeriod, NULL, dwScaleFactor, pIAudioProcessor, pIAudioEndpointRT, &guidParent);
//    if (SUCCEEDED(hr))
//    {
//        SLOG(eError, "ERROR:Succeeded with NULL dwBudget, 0x%X", hr);
//        m_pAudioPump = NULL; //the pointer is smart
//        hr = CreatePump();
//        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
//        dw = FNS_FAIL;
//    }
//    ASSERTION("Trying with NULL ScaleFactor");
//    hr = m_pAudioPump->InitializeDependant(ui32PumpCreationFlags, dwPeriod, dwBudget, NULL, pIAudioProcessor, pIAudioEndpointRT, &guidParent);
//    if (SUCCEEDED(hr))
//    {
//        SLOG(eError, "ERROR:Succeeded with NULL ScaleFactor, 0x%X", hr);
//        m_pAudioPump = NULL; //the pointer is smart
//        hr = CreatePump();
//        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
//        dw = FNS_FAIL;
//    }
//    //Try NULL AudioProcessor
//    ASSERTION("Trying with NULL AudioProcessor");
//    hr = m_pAudioPump->InitializeDependant(ui32PumpCreationFlags, dwPeriod, dwBudget, dwScaleFactor, NULL, pIAudioEndpointRT, &guidParent);
//    if (SUCCEEDED(hr))
//    {
//        SLOG(eError, "ERROR:Succeeded with NULL IAudioProcessRT*, 0x%X", hr);
//        m_pAudioPump = NULL; //the pointer is smart
//        hr = CreatePump();
//        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
//        dw = FNS_FAIL;
//    }
//    //Try NULL AudioEndPoint
//    ASSERTION("Trying with NULL AudioEndPoint");
//    hr = m_pAudioPump->InitializeDependant(ui32PumpCreationFlags, dwPeriod, dwBudget, dwScaleFactor, pIAudioProcessor, NULL, &guidParent);
//    if (SUCCEEDED(hr))
//    {
//        SLOG(eError, "ERROR:Succeeded with NULL IAudioEndPointRT*, 0x%X", hr);
//        m_pAudioPump = NULL; //the pointer is smart
//        hr = CreatePump();
//        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
//        dw = FNS_FAIL;
//    }
//
//    //Try NULL GUID ptr
//    ASSERTION("Trying with NULL GUIDReserve");
//    hr = m_pAudioPump->InitializeDependant(ui32PumpCreationFlags, dwPeriod, dwBudget, dwScaleFactor, pIAudioProcessor, pIAudioEndpointRT, NULL);
//    if (SUCCEEDED(hr))
//    {
//        SLOG(eError, "ERROR:Succeeded with NULL GUID*, 0x%X", hr);
//        m_pAudioPump = NULL; //the pointer is smart
//        hr = CreatePump();
//        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
//        dw = FNS_FAIL;
//    }
//
//
//    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\\
//    //Try Double Initialization   
//    ASSERTION("Trying Double Initialization");
//    //First Initialization with all valid parameters, Should SUCCEED
//    ASSERTION("Trying First Initialize, should SUCCEED");
//    dwPeriod = 20 * MFTIME1MSEC;
//	dwBudget=2 * MFTIME1MSEC;
//    hr = m_pAudioPump->InitializeDependant(ui32PumpCreationFlags, dwPeriod, dwBudget, dwScaleFactor, pIAudioProcessor, pIAudioEndpointRT, &guidParent);
//    if (FAILED(hr))
//    {
//        SLOG(eError, "ERROR:Initialize FAILED with all valid Params 0x%X", hr);
//        dw = FNS_FAIL;
//    }       
//    else
//    {
//        // Try the second initialization, it should FAIL here.
//        ASSERTION("Trying Second Initialize, should FAIL");
//        hr = m_pAudioPump->InitializeDependant(ui32PumpCreationFlags, dwPeriod, dwBudget, dwScaleFactor, pIAudioProcessor, pIAudioEndpointRT, &guidParent);
//        METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Double Initializion SUCCEEDED", dw, FNS_FAIL);          	
//    }
//    // At this point Audio Pump is probably initialized if the above calls went fine, So we should release m_pAudioPump to get rid of Pump and 
//    // restart testing from this point after calling CreatePump to get a brand new IAudioPump        
//    m_pAudioPump = NULL; //the pointer is smart
//    hr = CreatePump();    
//    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
//    // All is good and m_pAudioPump has a valid IAudioPump.
//
//    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\\
//    //Try Different Pump Creation Flags
//    ASSERTION("Trying Different Pump Creation Flags");
//    ui32PumpCreationFlags = AUDIOPUMP_NONE;
//    ASSERTION("Trying AUDIOPUMP_NONE creation flag");
//    hr = m_pAudioPump->InitializeDependant(ui32PumpCreationFlags, dwPeriod, dwBudget, dwScaleFactor, pIAudioProcessor, pIAudioEndpointRT, &guidParent);
//    if (SUCCEEDED(hr))
//    {
//        SLOG(eError, "ERROR: AudioPump Initialization SUCCEEDED with AUDIOPUMP_NONE Creation Flags, 0x%X", hr);
//        dw = FNS_FAIL;
//        // The call succeeded, Release Pump, and create a new one            
//        m_pAudioPump = NULL;
//        hr = CreatePump();
//        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
//    }
//    // A valid call needs one of either Input or Output Flag
//    ASSERTION("Trying AUDIOPUMP_THREAD_BEFORE and AUDIOPUMP_THREAD_AFTER creation flag");
//    ui32PumpCreationFlags = AUDIOPUMP_THREAD_BEFORE | AUDIOPUMP_THREAD_AFTER | AUDIOPUMP_ENDPOINT_OUTPUT | AUDIOPUMP_REALTIME;
//    hr = m_pAudioPump->InitializeDependant(ui32PumpCreationFlags, dwPeriod, dwBudget, dwScaleFactor, pIAudioProcessor, pIAudioEndpointRT, &guidParent);
//    if(SUCCEEDED(hr))
//    {
//        SLOG(eError, "ERROR: AudioPump Initialization SUCCEEDED with AUDIOPUMP_THREAD_BEFORE and AUDIOPUMP_THREAD_AFTER Creation Flags, 0x%X", hr);
//        dw = FNS_FAIL;
//        // The call succeeded, Release Pump, and create a new one            
//        m_pAudioPump = NULL;
//        hr = CreatePump();
//        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
//    }
//    ASSERTION("Trying AUDIOPUMP_THREAD_BEFORE creation flag");    
//    ui32PumpCreationFlags = AUDIOPUMP_THREAD_BEFORE | AUDIOPUMP_ENDPOINT_OUTPUT | AUDIOPUMP_REALTIME;
//    hr = m_pAudioPump->InitializeDependant(ui32PumpCreationFlags, dwPeriod, dwBudget, dwScaleFactor, pIAudioProcessor, pIAudioEndpointRT, &guidParent);
//    if (FAILED(hr))
//    {
//        SLOG(eError, "ERROR: Initializing AudioPump with AUDIOPUMP_THREAD_BEFORE Creation Flags FAILED, 0x%X", hr);    	
//        dw = FNS_FAIL;
//    }
//    else
//    {   // The call succeeded, Release Pump, and create a new one 
//        m_pAudioPump = NULL;
//        hr = CreatePump();
//        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
//    }
//    ASSERTION("Trying AUDIOPUMP_THREAD_AFTER creation flag");    
//    ui32PumpCreationFlags = AUDIOPUMP_THREAD_AFTER | AUDIOPUMP_ENDPOINT_OUTPUT | AUDIOPUMP_REALTIME;
//    hr = m_pAudioPump->InitializeDependant(ui32PumpCreationFlags, dwPeriod, dwBudget, dwScaleFactor, pIAudioProcessor, pIAudioEndpointRT, &guidParent);
//    if (FAILED(hr))
//    {
//        SLOG(eError, "ERROR: Initializing AudioPump with AUDIOPUMP_THREAD_AFTER Creation Flags FAILED, 0x%X", hr);    	
//        dw = FNS_FAIL;
//    }
//    else
//    {   // The call succeeded, Release Pump, and create a new one 
//        m_pAudioPump = NULL;
//        hr = CreatePump();
//        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);                
//    }
//    //A valid call needs one of either BEFORE or AFTER flag.
//    ASSERTION("Trying AUDIOPUMP_ENDPOINT_INPUT and AUDIOPUMP_ENDPOINT_OUTPUT creation flag");
//    ui32PumpCreationFlags = AUDIOPUMP_ENDPOINT_INPUT | AUDIOPUMP_ENDPOINT_OUTPUT | AUDIOPUMP_THREAD_BEFORE | AUDIOPUMP_REALTIME;
//    hr = m_pAudioPump->InitializeDependant(ui32PumpCreationFlags, dwPeriod, dwBudget, dwScaleFactor, pIAudioProcessor, pIAudioEndpointRT, &guidParent);
//    if(SUCCEEDED(hr))
//    {
//        SLOG(eError, "ERROR: AudioPump Initialization SUCCEEDED with AUDIOPUMP_ENDPOINT_INPUT and AUDIOPUMP_ENDPOINT_OUTPUT Creation Flags, 0x%X", hr);
//        dw = FNS_FAIL;
//        // The call succeeded, Release Pump, and create a new one
//        m_pAudioPump = NULL;
//        hr = CreatePump();
//        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
//    }
//    ASSERTION("Trying AUDIOPUMP_ENDPOINT_OUTPUT creation flag");    
//    ui32PumpCreationFlags = AUDIOPUMP_ENDPOINT_OUTPUT | AUDIOPUMP_THREAD_BEFORE | AUDIOPUMP_REALTIME;
//    hr = m_pAudioPump->InitializeDependant(ui32PumpCreationFlags, dwPeriod, dwBudget, dwScaleFactor, pIAudioProcessor, pIAudioEndpointRT, &guidParent);
//    if (FAILED(hr))
//    {
//        SLOG(eError, "ERROR: Initializing AudioPump with AUDIOPUMP_ENDPOINT_OUTPUT Creation Flags FAILED, 0x%X", hr);    	
//        dw = FNS_FAIL;
//    }
//    else
//    {   // The call succeeded, Release Pump, and create a new one 
//        m_pAudioPump = NULL;
//        hr = CreatePump();
//        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
//    }
//    //Reinitialize the Endpoint as Input
//    SLOG(eInfo1, "Initializing Input Endpoint");
//    hr = pAudioEndpoint->Initialize(&AudioFormat, pfOutput, cbBufferSize, u32NumFrames, TRUE);
//    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Input Endpoint Initialization FAILED", dw, FNS_FAIL);
//
//    ASSERTION("Trying AUDIOPUMP_ENDPOINT_INPUT creation flag");    
//    ui32PumpCreationFlags = AUDIOPUMP_ENDPOINT_INPUT | AUDIOPUMP_THREAD_BEFORE | AUDIOPUMP_REALTIME;
//    hr = m_pAudioPump->InitializeDependant(ui32PumpCreationFlags, dwPeriod, dwBudget, dwScaleFactor, pIAudioProcessor, pIAudioEndpointRT, &guidParent);
//    if (FAILED(hr))
//    {
//        SLOG(eError, "ERROR: Initializing AudioPump with AUDIOPUMP_ENDPOINT_INPUT Creation Flags FAILED, 0x%X", hr);    	
//        dw = FNS_FAIL;
//    }
//    else
//    {   // The call succeeded, Release Pump, and create a new one 
//        m_pAudioPump = NULL;
//        hr = CreatePump();
//        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);
//    }
//    
//    ASSERTION("Trying without AUDIOPUMP_REALTIME creation flag");    
//    ui32PumpCreationFlags = AUDIOPUMP_ENDPOINT_INPUT | AUDIOPUMP_THREAD_BEFORE;
//    hr = m_pAudioPump->InitializeDependant(ui32PumpCreationFlags, dwPeriod, dwBudget, dwScaleFactor, pIAudioProcessor, pIAudioEndpointRT, &guidParent);
//    if (SUCCEEDED(hr))
//    {
//        SLOG(eError, "ERROR: Initializing AudioPump with AUDIOPUMP_ENDPOINT_INPUT Creation Flags FAILED, 0x%X", hr);    	
//        dw = FNS_FAIL;
//        // The call succeeded, Release Pump, and create a new one 
//        m_pAudioPump = NULL;
//        hr = CreatePump();
//        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);
//    }
//
//    ui32PumpCreationFlags = AUDIOPUMP_ENDPOINT_INPUT | AUDIOPUMP_THREAD_BEFORE | AUDIOPUMP_REALTIME;    
//
//    // TRY DIFFERENT VALUES FOR PERIOD, WHAT ARE THE INVALID VALUES ????????????
//
//    ASSERTION("Trying different values for Time Period & Budget");
//    ASSERTION("Initializing Pump with Budget and Period = 0");
//    dwBudget = 0;
//    dwPeriod = 0;
//    hr = m_pAudioPump->InitializeDependant(ui32PumpCreationFlags, dwPeriod, dwBudget, dwScaleFactor, pIAudioProcessor, pIAudioEndpointRT, &guidParent);    
//    if (SUCCEEDED(hr))
//    {
//        SLOG(eError, "ERROR: Error Initializing Pump with Budget and Period = 0 SUCCEEDED, 0x%X", hr);
//        dw = FNS_FAIL;
//        // The call succeeded, Release Pump, and create a new one            
//        m_pAudioPump = NULL;
//        hr = CreatePump();
//        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
//    }
//
//    ASSERTION("Initializing Pump with Period = 0");
//    dwBudget = 2 * MFTIME1MSEC;//2 msec
//    dwPeriod = 0;
//    hr = m_pAudioPump->InitializeDependant(ui32PumpCreationFlags, dwPeriod, dwBudget, dwScaleFactor, pIAudioProcessor, pIAudioEndpointRT, &guidParent);
//    if (SUCCEEDED(hr))
//    {
//        SLOG(eError, "ERROR: Error Initializing Pump with Period = 0 SUCCEEDED, 0x%X", hr);
//        dw = FNS_FAIL;
//        // The call succeeded, Release Pump, and create a new one            
//        m_pAudioPump = NULL;
//        hr = CreatePump();
//        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
//        
//    }
//    ASSERTION("Initializing Pump with Budget = 0");
//    dwBudget = 0;
//    dwPeriod = 20 * MFTIME1MSEC;
//    hr = m_pAudioPump->InitializeDependant(ui32PumpCreationFlags, dwPeriod, dwBudget, dwScaleFactor, pIAudioProcessor, pIAudioEndpointRT, &guidParent);
//    if (FAILED(hr))
//    {
//        SLOG(eError, "ERROR: Error Initializing Pump with Budget = 0 FAILED, 0x%X", hr);
//        dw = FNS_FAIL;
//    }
//    else    
//    {   
//        // The call succeeded, Release Pump, and create a new one            
//        m_pAudioPump = NULL;
//        hr = CreatePump();
//        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);
//    }    
//    ASSERTION("Initializing Pump with Budget (20msec) > Period (10msec)");
//    dwBudget = 20 * MFTIME1MSEC;
//    dwPeriod = dwBudget / 2;
//    hr = m_pAudioPump->InitializeDependant(ui32PumpCreationFlags, dwPeriod, dwBudget, dwScaleFactor, pIAudioProcessor, pIAudioEndpointRT, &guidParent);
//    if (SUCCEEDED(hr))
//    {
//        SLOG(eError, "ERROR: Error Initializing Pump with Budget (20msec) > Period (10msec)SUCCEEDED, 0x%X", hr);
//        dw = FNS_FAIL;
//        // The call succeeded, Release Pump, and create a new one            
//        m_pAudioPump = NULL;
//        hr = CreatePump();
//        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);
//    }
//
//    ASSERTION("Initializing Pump with Period = 20000 (AudPump Min Period * Default Throttle)");
//    dwPeriod = 20000;
//    dwBudget = dwPeriod / 2;
//    hr = m_pAudioPump->InitializeDependant(ui32PumpCreationFlags, dwPeriod, dwBudget, dwScaleFactor, pIAudioProcessor, pIAudioEndpointRT, &guidParent);
//    if (FAILED(hr))
//    {
//        SLOG(eError, "ERROR: Error Initializing Pump with Period = 20000, 0x%X", hr);
//        dw = FNS_FAIL;
//    }
//    else
//    {    // The call succeeded, Release Pump, and create a new one            
//        m_pAudioPump = NULL;
//        hr = CreatePump();
//        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);
//    }
//    ASSERTION("Initializing Pump with Period = 25000 (Not an integral multiple of msec)");
//    dwPeriod = 25000;
//    hr = m_pAudioPump->InitializeDependant(ui32PumpCreationFlags, dwPeriod, dwBudget, dwScaleFactor, pIAudioProcessor, pIAudioEndpointRT, &guidParent);
//    if (FAILED(hr))
//    {
//        SLOG(eError, "ERROR: Error Initializing Pump with Period = 25000, 0x%X", hr);
//        dw = FNS_FAIL;
//    }
//    else
//    {    // The call succeeded, Release Pump, and create a new one            
//        m_pAudioPump = NULL;
//        hr = CreatePump();
//        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);
//    }
//    ASSERTION("Initializing Pump with Period = 56789 (Not an integral multiple of Def Throttle)");
//    dwPeriod = 56789;
//    hr = m_pAudioPump->InitializeDependant(ui32PumpCreationFlags, dwPeriod, dwBudget, dwScaleFactor, pIAudioProcessor, pIAudioEndpointRT, &guidParent);
//    if (FAILED(hr))
//    {
//        SLOG(eError, "ERROR: Error Initializing Pump with Period = 56789, 0x%X", hr);
//        dw = FNS_FAIL;
//    }
//    else
//    {    // The call succeeded, Release Pump, and create a new one            
//        m_pAudioPump = NULL;
//        hr = CreatePump();
//        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);
//    }
//    ASSERTION("Initializing Pump with Period = MAX_UINT64 (Boundary Condition)");
//    dwPeriod = 0x7FFFFFFF;
//    hr = m_pAudioPump->InitializeDependant(ui32PumpCreationFlags, dwPeriod, dwBudget, dwScaleFactor, pIAudioProcessor, pIAudioEndpointRT, &guidParent);
//    if (FAILED(hr))
//    {
//        SLOG(eError, "ERROR: Error Initializing Pump with Period = MAX_UINT64 FAILED, 0x%X", hr);
//        dw = FNS_FAIL;
//    }
//    else
//    {    // The call succeeded, Release Pump, and create a new one            
//        m_pAudioPump = NULL;
//        hr = CreatePump();
//        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);
//    }
//    ASSERTION("Initializing Pump with Budget > Period");
//    dwPeriod = 10 * MFTIME1MSEC;
//    dwBudget = 11 * MFTIME1MSEC;
//    hr = m_pAudioPump->InitializeDependant(ui32PumpCreationFlags, dwPeriod, dwBudget, dwScaleFactor, pIAudioProcessor, pIAudioEndpointRT, &guidParent);
//    if (SUCCEEDED(hr))
//    {
//        SLOG(eError, "ERROR: Initializing Pump with Budget > Period SUCCEEDED, 0x%X", hr);
//        dw = FNS_FAIL;
//    }
//    else
//    {    // The call succeeded, Release Pump, and create a new one            
//        m_pAudioPump = NULL;
//        hr = CreatePump();
//        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);
//    }
//
//    ASSERTION("Initializing Pump with invalid GUIDReserve");
//    dwPeriod = 20 * MFTIME1MSEC;
//    dwBudget = 2 * MFTIME1MSEC;    
//    hr = m_pAudioPump->InitializeDependant(ui32PumpCreationFlags, dwPeriod, dwBudget, dwScaleFactor, pIAudioProcessor,
//                                           pIAudioEndpointRT, const_cast<GUID*>(&guidTemp));
//    if (SUCCEEDED(hr))
//    {
//        SLOG(eError, "ERROR: Initializing Pump with invalid GUIDReserve SUCCEEDED, 0x%X", hr);
//        dw = FNS_FAIL;
//    }
//    else
//    {    // The call succeeded, Release Pump, and create a new one            
//        m_pAudioPump = NULL;        
//    }
//exit:
//    SAFE_DELETE_ARRAY(pfOutput);
//    pAVReserve = NULL;
//    pAVReserveMgr = NULL;
//    return dw;
//}

//-------------------------------------------------------------------------
// Class: CAudioPumpTest
// Method: AudioPumpCreate
//
// Description:
//  This method creates an instance of AudioPump to test if the object has been registered with COM
//   
// Parameters:
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on error
//
DWORD CAudioPumpTest::AudioPumpCreate()
{
    HRESULT hr = S_OK;
    IAudioPumpPtr pAudioPump = NULL;    

    // Confirm if the standard usage is supposed to be with CoCreate instead of smart pointers

    ASSERTION("Creating AudioPump");
    hr = pAudioPump.CreateInstance(__uuidof(AudioPump));
    if (SUCCEEDED(hr))
    {   
        ASSERTION("Succeeded in creating Audio Pump");
        return FNS_PASS;
    }
    else
    {
        SLOG(eError, "Audio Pump Creation Failed with error code 0x%X", hr);
        return FNS_FAIL;
    }
}


//-------------------------------------------------------------------------
// Class: CAudioPumpTest
// Method: AudioPumpStart
//
// Description:
//  This method tests IAudioPump::Start
//   
// Parameters:
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on error
//
DWORD CAudioPumpTest::AudioPumpStart()
{
    DWORD dw = FNS_PASS;
    MFTIME mfStartTime = 0;
    HRESULT hr = S_OK;

    _ASSERT(m_pAudioPump != NULL);

    //if (NULL == m_pAudioPump) // Check to see if we have the Pump interface
    //{
    //    hr = CreatePump();        
    //}
    //if(FAILED(hr)) // Failed to Create a Pump interface
    //{    
    //    dw = FNS_FAIL;
    //    goto exit;
    //}
    SLOG(eInfo1, "Call Start without Initializing first");        
    hr = m_pAudioPump->Start(&mfStartTime, NULL, NULL);
    if(SUCCEEDED(hr))
    {
        SLOG(eError,"ERROR: Start SUCCEEDED on an uninitialized Pump");
        dw = FNS_FAIL;
        // Since it has started, we should Stop it.
        m_pAudioPump->Stop();
    }
    SLOG(eInfo1, "Initializing Audio Pump");
    // Endpoint Buffer for 10msecs 44.1khz mono 16 PCM
    LPVOID pBuffer = LocalAlloc(LMEM_FIXED, 441 * 2);
    hr = InitializePump(pBuffer, 441*2);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Initializing the Pump with Valid Parameters FAILED", dw, FNS_FAIL);
    
    //Try with NULL Parameters
    SLOG(eInfo1, "Trying to Start with NULL param");
    hr = m_pAudioPump->Start(NULL, NULL, NULL);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: Start SUCCEEDED with NULL Start Time");
        // Since it has started, we should Stop it.
        m_pAudioPump->Stop();
    }
    //Trying to double start a Pump
    SLOG(eInfo1, "Trying to double start a running Pump");        
    hr = m_pAudioPump->Start(&mfStartTime, NULL, NULL);
    if(FAILED(hr))
    {
        SLOG(eError, "Trying to Start a Pump with valid Parameters FAILED");
        dw = FNS_FAIL;
    }
    else
    {
        //The first Start succeeded, so try again
        hr = m_pAudioPump->Start(&mfStartTime, NULL, NULL);
        METHOD_CHECKSUCCESS_SETVAL(hr, "Calling Start on a running Pump SUCCEEDED", dw, FNS_FAIL);

        //Stop the Pump
        hr = m_pAudioPump->Stop();
        METHOD_CHECKFAIL_SETVAL(hr, "Calling Stop on a running Pump FAILED", dw, FNS_FAIL);        
    }    
exit:    
    SAFE_FREE_POINTER(pBuffer);
    return dw;
}


//-------------------------------------------------------------------------
// Class: CAudioPumpTest
// Method: AudioPumpStop
//
// Description:
//  This method tests IAudioPump::Stop
//   
// Parameters:
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on error
//
DWORD CAudioPumpTest::AudioPumpStop()
{
    DWORD dw = FNS_PASS;
    HRESULT hr = S_OK;
    MFTIME mfStartTime = 0;

    _ASSERT(m_pAudioPump != NULL);
    //if (NULL == m_pAudioPump) // Check to see if we have the Pump interface
    //{        
    //    hr = CreatePump();        
    //}
    //if(FAILED(hr))
    //{    
    //    dw = FNS_FAIL;
    //    goto exit;
    //}
    SLOG(eInfo1, "Trying to STOP an uninitialized Pump");
    hr = m_pAudioPump->Stop();
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Stopping an unintialized Pump SUCCEEDED", dw, FNS_FAIL);

    SLOG(eInfo1, "Trying to Stop a \"Not Running\" Pump");
    // First Initialize the Pump
    // Endpoint Buffer for 10msecs 44.1khz mono 16 PCM
    LPVOID pBuffer = LocalAlloc(LMEM_FIXED, 441 * 2);
    hr = InitializePump(pBuffer, 441*2);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Initializing the Pump with Valid Parameters FAILED", dw, FNS_FAIL);
    
    // Stop the Pump
    SLOG(eInfo1, "Stopping Audio Pump");
    hr = m_pAudioPump->Stop();
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Stopping a \"Not Running\" Pump SUCCEEDED", dw, FNS_FAIL);

    // The Pump at this point is initialized, 
    // So Start it and then make a legitimate Stop call    
    SLOG(eInfo1, "Trying a legit Start and Stop on Pump");
    SLOG(eInfo1, "Starting Audio Pump");

    hr = m_pAudioPump->Start(&mfStartTime, NULL, NULL);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Starting an Initialized Pump in a legitimate way FAILED", dw, FNS_FAIL);
    
    // Started, so Stop it
    SLOG(eInfo1, "Stopping Audio Pump");
    hr = m_pAudioPump->Stop();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Making a legitimate Stop call FAILED", dw, FNS_FAIL);

    // Try to double Stop a Pump
    SLOG(eInfo1, "Trying to double Stop a Pump");
    hr = m_pAudioPump->Stop();
    METHOD_CHECKSUCCESS_SETVAL_EXIT(hr, "ERROR: Trying to double Stop Pump SUCCEEDED", dw, FNS_FAIL);    

exit:
    return dw;
}

//-------------------------------------------------------------------------
// Class: CAudioPumpTest
// Method: AudioPumpStop
//
// Description:
//  This method tests IAudioPump::GetRegisteredClock
//   
// Parameters:
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on error
//
//DWORD CAudioPumpTest::AudioPumpGetRegisteredClock()
//{
//    HRESULT hr = S_OK;
//    DWORD dw = FNS_PASS;
//    IMFClock* pIMFClock = NULL;
//
//    _ASSERT(m_pAudioPump != NULL);
//
//    //if(m_pAudioPump == NULL) // Check to see if we have a Pump
//    //{        
//    //    hr = CreatePump();
//    //}
//    //if(FAILED(hr))   // Creating Pump failed    
//    //{    
//    //    dw = FNS_FAIL;
//    //    goto exit;
//    //}
//    SLOG(eInfo1, "Trying to get the Clock from an unitialized Pump");
//    hr = m_pAudioPump->GetRegisteredClock(&pIMFClock);
//    if (SUCCEEDED(hr) || (NULL != pIMFClock))
//    {
//        SLOG(eError, "ERROR: Getting a Clock from an Uninitialized Pump SUCCEEDED");
//        dw =FNS_FAIL;
//    }
//    SLOG(eInfo1, "Trying NULL Param");
//    // First Initialize the Pump
//    SLOG(eInfo1, "Initializing Audio Pump");
//    // Endpoint Buffer for 10msecs 44.1khz mono 16 PCM
//    LPVOID pBuffer = LocalAlloc(LMEM_FIXED, 441 * 2);
//    hr = InitializePump(pBuffer, 441*2);
//    if(FAILED(hr)) // Initializing the Pump with valid params FAILED
//    {
//        SLOG(eError, "ERROR: Initializing the Pump with Valid Parameters FAILED");
//        dw = FNS_FAIL;
//        goto exit;
//    }
//    SLOG(eInfo1, "Getting Registered Clock");
//    hr = m_pAudioPump->GetRegisteredClock(NULL);
//    if(SUCCEEDED(hr))
//    {
//        SLOG(eError, "Trying GetRegisteredClock with NULL Param SUCCEEDED");
//        dw = FNS_FAIL;
//    }
//    // Pump is still Initialized at this Point
//    // Try a valid call
//    SLOG(eInfo1, "Trying a valid GetRegisteredClock call");
//    hr = m_pAudioPump->GetRegisteredClock(&pIMFClock);
//    if(FAILED(hr))
//    {
//        SLOG(eError, "ERROR: Trying a valid GetRegisteredClock call FAILED");
//        dw = FNS_FAIL;
//    }
//
//exit:
//    return dw;
//}


//-------------------------------------------------------------------------
// Class: CAudioPumpTest
// Method: AudioPumpStop
//
// Description:
//  This method tests IAudioPump::ReconfigureTimer
//   
// Parameters:
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on error
//
DWORD CAudioPumpTest::AudioPumpReconfigureTimer()
{
    HRESULT hr = S_OK;
    DWORD dwPeriod = 0;    
    DWORD dw = FNS_PASS;

  
	_ASSERT(m_pAudioPump != NULL);    

	// Method has been removed from the interface
/*    SLOG(eInfo1, "Calling Reconfigure Timer on an uninitialized Pump ");
    hr = m_pAudioPump->ReconfigureTimer(&dwPeriod);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: Reconfigure Timer on an unitialized Pump SUCCEEDED");
        dw = FNS_FAIL;
        m_pAudioPump = NULL;
        hr = CreatePump();
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FATAL ERROR, No Pump available to continue", dw, FNS_FAIL);        
    }
    SLOG(eInfo1, "Initializing Audio Pump");
    // Endpoint Buffer for 10msecs 48khz mono 16bit PCM
    LPVOID pBuffer = LocalAlloc(LMEM_FIXED, 480 * 2);
    hr = InitializePump(pBuffer, 480*2);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Initializing the Pump with Valid Parameters FAILED", dw, FNS_FAIL);

    SLOG(eInfo1, "Calling Reconfigure Timer with valid params");
    hr = m_pAudioPump->ReconfigureTimer(&dwPeriod);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Making a valid call to Reconfigure Timer failed with code 0x%X", hr);
        dw = FNS_FAIL;
    }

exit:
*/
    return dw;
}
