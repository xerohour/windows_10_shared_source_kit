// Copyright(C) Microsoft. All rights reserved.

#include "stdafx.h"


//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOMeterCreate
//
// Description:
//  TestCase to test if AudioMeter class is registered and IAudioMeter interface can be obtained from it
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOMeterCreate()
{    
    CAPOMeterTest APOMeterTest;
    return APOMeterTest.APOMeterCreate();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOMeterGetMeteringData
//
// Description:
//  TestCase to test IAudioMeter->GetMeteringData API call
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOMeterGetMeteringData()
{
    CAPOMeterTest APOMeterTest;
    return APOMeterTest.APOMeterGetMeteringData();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOMeterInitialize
//
// Description:
//  TestCase to test IAudioMeter->Initialize API call
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOMeterInitialize()
{    
    CAPOMeterTest APOMeterTest;
    return APOMeterTest.APOMeterInitialize();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOMeterCheckAllocation
//
// Description:
//  TestCase to test the Allocation and deallocation of memory in Meter APO
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOMeterCheckAllocation()
{    
    CAPOMeterTest APOMeterTest;
    return APOMeterTest.APOCheckAllocation();
}


//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOMeterProcess
//
// Description:
//  TestCase to test APOProcess API call
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOMeterProcess()
{    
    CAPOMeterTest APOMeterTest;
    return APOMeterTest.APOTestMeter();
}


//-------------------------------------------------------------------------
// Class: CAPOMeterTest
// Method: APOMeterCreate
//
// Description:
//   Actual testcase to check if AudioMeter is registered with COM and if IAudioMeter and  
//   IAudioProcessingObjetPtr can be successfully obtained from it.
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOMeterTest::APOMeterCreate()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;    
    IAudioMeterPtr                 pMeter = NULL;    
    IAudioProcessingObjectPtr       pAPO = NULL;
    IAudioProcessingObjectRTPtr             pAPORT = NULL;
    IAudioProcessingObjectConfigurationPtr  pAPOConfig  = NULL;    

    // Check if all the required APO interfaces can be queried for
    //Create Meter Object
    SLOG(eInfo1, "Creating Audio Meter object");
    hr = pMeter.CreateInstance(__uuidof(AudioMeter));
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: FAILED to create Audio Meter object, 0x%X", hr);
        dw = FNS_FAIL;
    }
    SLOG(eInfo1, "Querying APO object from Meter");
    hr = pMeter.QueryInterface(__uuidof(IAudioProcessingObject), &pAPO);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: FAILED to query for  APO object from Meter, 0x%X", hr);
        dw = FNS_FAIL;
    }
    SLOG(eInfo1, "Querying APO object from Volume");
    hr = pMeter.QueryInterface(__uuidof(IAudioProcessingObjectRT), &pAPORT);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: FAILED to query for APORT object from Meter, 0x%X", hr);
        dw = FNS_FAIL;
    }
    SLOG(eInfo1, "Querying APO object from Meter");
    hr = pMeter.QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), &pAPOConfig);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: FAILED to query for  APO Config object from Meter, 0x%X", hr);
        dw = FNS_FAIL;
    }
    
    return dw;
}

//-------------------------------------------------------------------------
// Class: CAPOMeterTest
// Method: APOMeterGetMeteringData
//
// Description:
//   
//   
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOMeterTest::APOMeterGetMeteringData()
{
    DWORD dw = FNS_PASS;
    HRESULT hr = S_OK;
    AUDIO_METER_DATA*  pAudioMeterData = NULL;
    UINT32 u32SamplesPerFrame = 0;

    _ASSERT( (m_pBaseAPO != NULL) && (m_pMeter != NULL) );

    SLOG(eInfo1, "Trying GetMeteringData on an uninitialized APO");
    u32SamplesPerFrame = 2;
    pAudioMeterData = (AUDIO_METER_DATA*)LocalAlloc(LMEM_FIXED, sizeof(AUDIO_METER_DATA) * u32SamplesPerFrame);
    _ASSERT(pAudioMeterData != NULL);
    hr = m_pMeter->GetMeteringData(pAudioMeterData, u32SamplesPerFrame);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: GetMeteringData SUCCEEDED on an uninitialized APO", dw, FNS_FAIL);
    SAFE_FREE_POINTER(pAudioMeterData);

    //Initialize APO for 2 channels, 1 frame/avg, 1 avg history and 1 frame/quant
    SLOG(eInfo1, "Initializing APO for 2 channels");
    u32SamplesPerFrame = 2;

	apoParams.u32SamplesPerFrame   = u32SamplesPerFrame;
	apoParams.u32FramesPerAverage  = 1; 
	apoParams.u32NumAverages	   = 1; 
	apoParams.u32FramesPerQuantum  = 1; 

    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Initialize on the APO FAILED", dw, FNS_FAIL);    

    SLOG(eInfo1, "Trying GetMeteringData on an Inactive/UnLocked APO");
    u32SamplesPerFrame = 2;
    pAudioMeterData = (AUDIO_METER_DATA*)LocalAlloc(LMEM_FIXED, sizeof(AUDIO_METER_DATA) * u32SamplesPerFrame);
    _ASSERT(pAudioMeterData != NULL);
    hr = m_pMeter->GetMeteringData(pAudioMeterData, u32SamplesPerFrame);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: GetMeteringData FAILED on an Inactive/UnLocked APO", dw, FNS_FAIL);    
    SAFE_FREE_POINTER(pAudioMeterData);
    //Meter APO now initialized for two channels

    //Try GetMeter call with NULL/Invalid Params
    //Try all NULL params
    SLOG(eInfo1, "Trying GetMeteringData with all params as NULL");
    hr = m_pMeter->GetMeteringData(NULL, NULL);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: GetMeteringData SUCCEEDED with all NULL Params", dw, FNS_FAIL);
    
    //NULL pMeteringData    
    SLOG(eInfo1, "Trying GetMeteringData NULL pMeteringData");        
    hr = m_pMeter->GetMeteringData(NULL, u32SamplesPerFrame);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: GetMeteringData SUCCEEDED with NULL pMeteringData", dw, FNS_FAIL);    
    // NULL ChannelCount
    SLOG(eInfo1, "Trying GetMeteringData NULL channelCount");
    u32SamplesPerFrame = 2;
    pAudioMeterData = (AUDIO_METER_DATA*)LocalAlloc(LMEM_FIXED, sizeof(AUDIO_METER_DATA) * u32SamplesPerFrame);
    _ASSERT(pAudioMeterData != NULL);
    hr = m_pMeter->GetMeteringData(pAudioMeterData, NULL);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: GetMeteringData SUCCEEDED with NULL channelCount", dw, FNS_FAIL);
    SAFE_FREE_POINTER(pAudioMeterData);
    //Channel count different from the initialized value
    SLOG(eInfo1, "Trying GetMeteringData with Channel count not equal to Initialized value");
    u32SamplesPerFrame = 2;
    pAudioMeterData = (AUDIO_METER_DATA*)LocalAlloc(LMEM_FIXED, sizeof(AUDIO_METER_DATA) * u32SamplesPerFrame);
    _ASSERT(pAudioMeterData != NULL);
    hr = m_pMeter->GetMeteringData(pAudioMeterData, 0xFF);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: GetMeteringData SUCCEEDED with ChannelCount value different from the Initialized value", dw, FNS_FAIL);
    SAFE_FREE_POINTER(pAudioMeterData);

exit:
    return dw;
}

//-------------------------------------------------------------------------
// Class: CAPOMeterTest
// Method: APOMeterInitialize
//
// Description:
//   
//   
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//

DWORD CAPOMeterTest::APOMeterInitialize()
{
    DWORD dw = FNS_PASS;
    HRESULT hr = S_OK;
    UINT32 ui32Channels = 1;
    UINT32 ui32FramesPerAvg = 1;
    UINT32 ui32NumAvgs = 1;
    UINT32 ui32FramesPerQuant = 1;
    APO_REG_PROPERTIES_FULL  regProp;
    APO_REG_PROPERTIES_FULL *pRegProp = &regProp;

    _ASSERT((m_pMeter != NULL) && (m_pAPO != NULL) );

    APORegistration::FillAllApoProperties( APO_METER, regProp );

    //Try Initialize call with NULL/Invalid Params

    //Try NULL param
    SLOG(eInfo1, "Calling Initialize with NULL Initialize data", hr);
    hr = m_pAPO->Initialize( 0, NULL);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Initialize SUCCEEDED with NULL param", dw, FNS_FAIL);

    //Try Channels = NULL
    ui32Channels = 0;
    SLOG(eInfo1, "Calling Initialize with # channels = 0", hr);    
    apoParams.u32SamplesPerFrame   = ui32Channels;
    apoParams.u32FramesPerAverage  = ui32FramesPerAvg; 
    apoParams.u32NumAverages	   = ui32NumAvgs; 
    apoParams.u32FramesPerQuantum  = ui32FramesPerQuant; 
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams );
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Initialize SUCCEEDED with NULL param", dw, FNS_FAIL);
    ui32Channels = 1;

    //Try FramesPerAvg = NULL
    ui32FramesPerAvg = 0;
    SLOG(eInfo1, "Calling Initialize with FramesPerAvg = 0", hr);    
    apoParams.u32SamplesPerFrame   = ui32Channels;
    apoParams.u32FramesPerAverage  = ui32FramesPerAvg; 
    apoParams.u32NumAverages	   = ui32NumAvgs; 
    apoParams.u32FramesPerQuantum  = ui32FramesPerQuant; 
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams );
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Initialize SUCCEEDED with NULL param", dw, FNS_FAIL);
    ui32FramesPerAvg = 1;

    //Try NumAverages = NULL
    ui32NumAvgs = 0;
    SLOG(eInfo1, "Calling Initialize with NumAverages = 0", hr);
    apoParams.u32SamplesPerFrame   = ui32Channels;
    apoParams.u32FramesPerAverage  = ui32FramesPerAvg; 
    apoParams.u32NumAverages	   = ui32NumAvgs; 
    apoParams.u32FramesPerQuantum  = ui32FramesPerQuant; 
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams );
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Initialize SUCCEEDED with NULL param", dw, FNS_FAIL);
    ui32NumAvgs = 1;

    //Try FramesPerQuantum = NULL
    ui32FramesPerQuant = 0;
    SLOG(eInfo1, "Calling Initialize with FramesPerQuantum = 0", hr);
    apoParams.u32SamplesPerFrame   = ui32Channels;
    apoParams.u32FramesPerAverage  = ui32FramesPerAvg; 
    apoParams.u32NumAverages	   = ui32NumAvgs; 
    apoParams.u32FramesPerQuantum  = ui32FramesPerQuant; 
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams );
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Initialize SUCCEEDED with NULL param", dw, FNS_FAIL);
    ui32FramesPerQuant = 1;

    // Try MAX_UINT channels
    SLOG(eInfo1, "Calling Initialize with MAX_UINT channels", hr);
    ui32Channels = 0xFFFFFFFF;
    apoParams.u32SamplesPerFrame   = ui32Channels;
    apoParams.u32FramesPerAverage  = ui32FramesPerAvg; 
    apoParams.u32NumAverages	   = ui32NumAvgs; 
    apoParams.u32FramesPerQuantum  = ui32FramesPerQuant; 
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams );
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Initialize SUCCEEDED with MAX_UINT channels", dw, FNS_FAIL);

    //Check Initialize behaviour outside of values specified by APO_REG_PROPS
    // Query for IAPO
    //IAudioProcessingObjectPtr pAPO = NULL;
    //hr = m_pMeter.QueryInterface(__uuidof(IAudioProcessingObject), &pAPO);
    //METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: QueryInterface for IAPO FAILED", dw, FNS_FAIL);

    // Get the Registration Properties
    //    SLOG(eInfo1, "Getting APO Registration Properties");        
    //    hr = pAPO->GetRegistrationProperties(&pRegProp);
    //    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: GetRegistrationProp call FAILED", dw, FNS_FAIL);

    // Try number of channels lower than Prop.min 
    SLOG(eInfo1, "Initializing with lower than MinSamplesPerFrame");
    apoParams.u32SamplesPerFrame   = UINT32(pRegProp->InFormat.u32MinSamplesPerFrame - 1);
    apoParams.u32FramesPerAverage  = ui32FramesPerAvg; 
    apoParams.u32NumAverages	   = ui32NumAvgs; 
    apoParams.u32FramesPerQuantum  = ui32FramesPerQuant; 
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams );

    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: Initializing with lower than MinSamplesPerFrame SUCCEEDED, 0x%X", hr);
        dw = FNS_FAIL;
        m_pMeter = NULL; m_pBaseAPO = NULL; m_pAPO = NULL;
        hr = CreateObjects();
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Meter Object Creation FAILED", dw, FNS_FAIL);
    }
    // Try number of channels greater than Prop.max 
    SLOG(eInfo1, "Initializing with greater than MaxSamplesPerFrame");
    apoParams.u32SamplesPerFrame   = UINT32(pRegProp->InFormat.u32MaxSamplesPerFrame + 1);
    apoParams.u32FramesPerAverage  = ui32FramesPerAvg; 
    apoParams.u32NumAverages	   = ui32NumAvgs; 
    apoParams.u32FramesPerQuantum  = ui32FramesPerQuant; 
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams );

    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: Initializing with greater than MaxSamplesPerFrame SUCCEEDED, 0x%X", hr);
        dw = FNS_FAIL;
        m_pMeter = NULL; m_pBaseAPO = NULL; m_pAPO = NULL;
        hr = CreateObjects();
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Meter Object Creation FAILED", dw, FNS_FAIL);
    }
    //Check if double initialization works
    //Lock the APO
    SLOG(eInfo1, "Checking behavior with double initialization");
    SLOG(eInfo1, "Trying first initialization with 2 channels");
    ui32Channels = 2;
    apoParams.u32SamplesPerFrame   = ui32Channels;
    apoParams.u32FramesPerAverage  = ui32FramesPerAvg; 
    apoParams.u32NumAverages	   = ui32NumAvgs; 
    apoParams.u32FramesPerQuantum  = ui32FramesPerQuant; 
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams );
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Initialize FAILED for 2 channels, 0x%X", hr);
        dw = FNS_FAIL;
    }
    
    SLOG(eInfo1, "Trying second initialization with 2 channels, should FAIL");
    apoParams.u32SamplesPerFrame   = ui32Channels;
    apoParams.u32FramesPerAverage  = ui32FramesPerAvg; 
    apoParams.u32NumAverages	   = ui32NumAvgs; 
    apoParams.u32FramesPerQuantum  = ui32FramesPerQuant; 
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams );
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: Double initialization SUCCEEDED, 0x%X", hr);
        dw = FNS_FAIL;
    }

    m_pMeter = NULL; m_pBaseAPO = NULL; m_pAPO = NULL;
    hr = CreateObjects();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Meter Object Creation FAILED", dw, FNS_FAIL);

    SLOG(eInfo1, "Checking if APO can be initialized after LockForProcess");
    SLOG(eInfo1, "Initializing with 2 channels");
    ui32Channels = 2;
    apoParams.u32SamplesPerFrame   = ui32Channels;
    apoParams.u32FramesPerAverage  = ui32FramesPerAvg; 
    apoParams.u32NumAverages	   = ui32NumAvgs; 
    apoParams.u32FramesPerQuantum  = ui32FramesPerQuant; 
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams );
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Initialize FAILED for 2 channels, 0x%X", hr);
        dw = FNS_FAIL;
    }    

    //Lock the APO
    IAudioMediaType* pInAudioMediaType = NULL;
    UNCOMPRESSEDAUDIOFORMAT uncompAF;
    BUILD_UNCOMP_FORMAT(&uncompAF, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 2, 4, 32, 48000);
    CAudioMediaType::UNCOMPAF2AMT(uncompAF, &pInAudioMediaType);    
    SLOG(eInfo1, "Locking the APO for Processing");
    hr = LockAPO(&pInAudioMediaType);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: LockForProcess FAILED on the APO", dw, FNS_FAIL);

    SLOG(eInfo1, "Calling Initialize after LockForProcess");
    ui32Channels = 2;
    apoParams.u32SamplesPerFrame   = ui32Channels;
    apoParams.u32FramesPerAverage  = ui32FramesPerAvg; 
    apoParams.u32NumAverages	   = ui32NumAvgs; 
    apoParams.u32FramesPerQuantum  = ui32FramesPerQuant; 
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams );
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: Initialize FAILED for 2 channels, 0x%X", hr);
        dw = FNS_FAIL;
    }

exit:
    return dw;
}

//-------------------------------------------------------------------------
// Class: CAPOMeterTest
// Method: APOMeterProcess
//
// Description:
//   
//   
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOMeterTest::APOTestMeter()
{
    DWORD dw = FNS_PASS;
    HRESULT hr = S_OK;
    IAudioProcessingObjectRTPtr pAPORT = NULL;
    FLOAT32 fRMSValue[2] = {0.0, 0.0};
    WAVEFORMATEX wfx;
    CComPtr<IBasicLog> pBasicLog;
    
    wfx.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = 48000;
    wfx.wBitsPerSample = sizeof(FLOAT32) * 8;
    wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
    wfx.cbSize = 0;

    _ASSERT(m_pMeter != NULL);
    _ASSERT(m_pBaseAPO != NULL);

    //Allocate memory for sine wave, 10msec 1KHz FLOAT32 Stereo sinewave
    FLOAT32* pfSineWave = (FLOAT32*)LocalAlloc(LMEM_FIXED, sizeof(FLOAT32) * 
                                                           2/*Num of channels*/ * 
                                                           480/* 10msec of 48Khz data*/);
    if(!pfSineWave)
    {
        SLOG(eError, "ERROR: Unable to allocate memory for sine data");
        dw = FNS_FAIL;
        goto exit;
    }
    SLOG(eInfo1, "Generating 48Khz FLOAT32 sine wave (10 ms)");
    hr = g_IShell->GetBasicLog(&pBasicLog);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Could not get a basic logger", dw, FNS_FAIL);
 
    hr = FillBufferWithSineSignal(
        pBasicLog, XFAIL,
        &wfx,
        1.0, // amplitude
        1000.0, // frequency
        0.0, // initial phase,
        0.0, // dc
        Method_NoDithering,
        reinterpret_cast<PBYTE>(pfSineWave),
        480, // samples in 10 ms @ 48000 samples/sec
        480 * wfx.nBlockAlign // bytes
    );
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Could not fill buffer with sine", dw, FNS_FAIL);
    
    // Allocate memory for input Connection, to be used with APOProcess call
    //This APO doesn't require an APO output connection
    APO_CONNECTION_PROPERTY* pInputConn = new APO_CONNECTION_PROPERTY;    
    if (!pInputConn)
    {
        SLOG(eError, "ERROR: Allocating memory for APO Connection");
        dw = FNS_FAIL;
        goto exit;
    }
    // Initialize Input Connection
    // Number of input frames is 480 (10msecs of data at 48K)
    BUILD_CONNECTION(pInputConn, pfSineWave, 480, 0, BUFFER_VALID);
    //Initialize the APO for 2 channels
    SLOG(eInfo1, "Initializing Meter APO for 2 channels");
 	apoParams.u32SamplesPerFrame   = 2;
	apoParams.u32FramesPerAverage  = 480; 
	apoParams.u32NumAverages	   = 1; 
	apoParams.u32FramesPerQuantum  = 480; 
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams );
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Initialization for 2 channels FAILED, 0x%X", hr);
        dw = FNS_FAIL;
        goto exit;
    }
    SLOG(eInfo1, "Querying IAPORT from Meter APO");
    hr = m_pMeter.QueryInterface(__uuidof(IAudioProcessingObjectRT), &pAPORT);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: QueryInterface for IAPORT FAILED, 0x%X", hr);
        dw = FNS_FAIL;
        goto exit;
    }
    UNCOMPRESSEDAUDIOFORMAT uncompAudioFloatFormat;
    BUILD_UNCOMP_FORMAT(&uncompAudioFloatFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 2, 4, 32, 48000);    
    IAudioMediaType* pAudioMediaType;
    CAudioMediaType::UNCOMPAF2AMT(uncompAudioFloatFormat, &pAudioMediaType);
    SLOG(eInfo1, "Locking APO for Processing");
    hr = LockAPO(&pAudioMediaType);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Lock for Process on the APO FAILED",dw, FNS_FAIL);    
    SLOG(eInfo1, "Calling APO Process on the APO");
    pAPORT->APOProcess( 1, /*1 input connection*/
                        &pInputConn, /*Input Connection*/
                        0, /*No output connection*/
                        NULL /*No output Connection*/);
    SLOG(eInfo1, "UnLocking APO for Processing");
    hr = m_pBaseAPO->UnlockForProcess();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr,"ERROR: UnLock for Process on the APO FAILED, dw, FNS_FAIL", dw, FNS_FAIL);
    AUDIO_METER_DATA audMeterData[2];
    SLOG(eInfo1, "Calling GetMeterData on the APO");
    hr = m_pMeter->GetMeteringData(audMeterData, 2);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: GetMeterData call FAILED, 0x%X", hr);
        dw = FNS_FAIL;
        goto exit;
    }
    //Check if the values are OK
    //The max value should be 1.0f
    if( (audMeterData[0].f32PeakLevel != (FLOAT32)1.0)||
        (audMeterData[1].f32PeakLevel != (FLOAT32)1.0) )
    {
        SLOG(eError, "ERROR: Max value mismatch, expected 1.0, actual %f, channel 0", audMeterData[0].f32PeakLevel);
        SLOG(eError, "ERROR: Max value mismatch, expected 1.0, actual %f, channel 1", audMeterData[1].f32PeakLevel);
        dw = FNS_FAIL;        
    }
    //The overcount should be zero
    if( (audMeterData[0].u32OverCount != 0) ||
        (audMeterData[1].u32OverCount != 0) )

    {
        SLOG(eError, "ERROR: OverCount value mismatch, expected 0, actual %d, channel 0", audMeterData[0].u32OverCount);
        SLOG(eError, "ERROR: OverCount value mismatch, expected 0, actual %d, channel 1", audMeterData[1].u32OverCount);
        dw = FNS_FAIL;        
    }

    //Calculate the rms value    
    for(UINT32 u32ValidFrames = 0; u32ValidFrames < 480; u32ValidFrames++)
    {
        fRMSValue[0] += pfSineWave[2*u32ValidFrames + 0] * pfSineWave[2*u32ValidFrames + 0]; //first channel
        fRMSValue[1] += pfSineWave[2*u32ValidFrames + 1] * pfSineWave[2*u32ValidFrames + 1]; //second channel
    }
    fRMSValue[0] /=  (480);
    fRMSValue[1] /=  (480);
    fRMSValue[0] = (FLOAT32)sqrt(fRMSValue[0]);
    fRMSValue[1] = (FLOAT32)sqrt(fRMSValue[1]);
    if( (!CCheck::InRange(fRMSValue[0], audMeterData[0].f32RMSLevel, 0.1)) ||
        (!CCheck::InRange(fRMSValue[1], audMeterData[1].f32RMSLevel, 0.1)) )
    {
        SLOG(eError, "ERROR: RMS value mismatch, expected %f, actual %f, channel 0", fRMSValue[0], audMeterData[0].f32PeakLevel);
        SLOG(eError, "ERROR: RMS value mismatch, expected %f, actual %f, channel 1", fRMSValue[1], audMeterData[1].f32PeakLevel);
        dw = FNS_FAIL;        
    }

    //fudge some of the values with a value greater than 1.0 and check if the overcount is OK
    for(UINT32 i = 0; i < 10; i++)
    {
        pfSineWave[2*i + 0] = (FLOAT32)2.0;
        pfSineWave[2*i + 1] = (FLOAT32)-2.0;
    }
    SLOG(eInfo1, "Locking APO for Processing");
    hr = LockAPO(&pAudioMediaType);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Lock for Process on the APO FAILED",dw, FNS_FAIL);    
    //Process again and see if the OverCount is 20
    pAPORT->APOProcess( 1, /*1 input connection*/
                        &pInputConn, /*Input Connection*/
                        0, /*No output connection*/
                        NULL /*No output Connection*/);
    SLOG(eInfo1, "UnLocking APO for Processing");
    hr = m_pBaseAPO->UnlockForProcess();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr,"ERROR: UnLock for Process on the APO FAILED", dw, FNS_FAIL);
    SLOG(eInfo1, "Calling GetMeterData on the APO");
    hr = m_pMeter->GetMeteringData(audMeterData, 2);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: GetMeterData call FAILED, 0x%X", hr);
        dw = FNS_FAIL;
        goto exit;
    }
     //The max value should now be 2.0f
    if( (audMeterData[0].f32PeakLevel != (FLOAT32)2.0) ||
        (audMeterData[1].f32PeakLevel != (FLOAT32)2.0) )
    {
        SLOG(eError, "ERROR: Max value mismatch, expected 2.0, actual %f", audMeterData[0].f32PeakLevel);
        SLOG(eError, "ERROR: Max value mismatch, expected 2.0, actual %f", audMeterData[1].f32PeakLevel);
        dw = FNS_FAIL;        
    }
    //The overcount should be twenty, 10 values at 2.0 and 10 values at -2.0
    if( (audMeterData[0].u32OverCount != 10) ||
        (audMeterData[1].u32OverCount != 10) )
    {
        SLOG(eError, "ERROR: OverCount value mismatch, expected 10, actual %d", audMeterData[0].u32OverCount);
        SLOG(eError, "ERROR: OverCount value mismatch, expected 10, actual %d", audMeterData[1].u32OverCount);
        dw = FNS_FAIL;        
    }

exit:
    SAFE_FREE_POINTER(pfSineWave);
    return dw;
}

//-------------------------------------------------------------------------
// Class: CAPOMeterTest
// Method: APOCheckAllocation
//
// Description:
//   this is check the allocation and deallocation of memory and structures in the Meter APO
//   Initialize the APO with a very large values, this should cause the memory allocation routines in the APO
//   to fail, At the end the APO should handle the non allocation case properly
//   Bug # 868493 PS, database Windows bugs
//   
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOMeterTest::APOCheckAllocation()
{
    DWORD dw = FNS_PASS;
    HRESULT hr = S_OK;


    _ASSERT(m_pMeter != NULL);
    _ASSERT(m_pBaseAPO != NULL);


    //Initialize the APO with very large values for the arguments
    // The value of Num Averages is adjusted so that a memory allocation happens for 0xFFFFFFFF
 	apoParams.u32SamplesPerFrame   = 2;
	apoParams.u32FramesPerAverage  = 0xFFFFFFFF; 
	apoParams.u32NumAverages	   = (0xFFFFFFFF-0x3); 
	apoParams.u32FramesPerQuantum  = 0xFFFFFFFF; 
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams );
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Initialize SUCCEEDED with very large values for the arguments", dw, FNS_FAIL);    
    
    
    return dw;
}

//-------------------------------------------------------------------------
// Class: CAPOMeterTest
// Method: LockAPO
//
// Description:
//   Helper routine to Lock the APO, given the number of Input Connection descriptors and Input Media type
//   This function is called from a number of places in order to Lock the APO
//   
// Parameters:
//    UINT32    ui32NumInputConnDesc
//      The number of input Connection Descriptors
//    IAudioMediaType**   ppInputAudioMediaType
//      An Array of IAudioMediatypes corresponding to the input descriptors
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
HRESULT CAPOMeterTest::LockAPO(IAudioMediaType** ppInputAudioMediaType)
{
    HRESULT hr = S_OK;
    APO_CONNECTION_DESCRIPTOR InputConnection;
    APO_CONNECTION_DESCRIPTOR *pInputConnDesc = NULL;    
    // The call below uses uses a pBuffer value of 0x1, since the API only checks for NULL
    // Max frames is set to 480, since all functions that call into this, use a max num of frames = 480
    BUILD_DESCRIPTOR(&InputConnection, *ppInputAudioMediaType, 
                     APO_CONNECTION_BUFFER_TYPE_EXTERNAL, 
                     0x1, 480);
    pInputConnDesc = &InputConnection;    
    hr = m_pBaseAPO->LockForProcess(1, &pInputConnDesc, 0, NULL);

    return hr;
}


//-------------------------------------------------------------------------
// Class: CAPOMeterTest
// Method: CreateObjects
//
// Description:
//   Internal method used to initialize internal variables. These methods are used by all other test methods
// Parameters:
//
// Return values (HRESULT):
//     Return S_OK on Success, E_FAIL or some other HRESULT error code on Failure
//
HRESULT CAPOMeterTest::CreateObjects()
{
    HRESULT hr = S_OK;
    if(m_pMeter == NULL)
    {
        SLOG(eInfo1, "Creating Meter Object");
        m_pMeter.CreateInstance(__uuidof(AudioMeter));
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Creating Meter Object, 0x%X", hr);
            goto exit;
        }
    }
    if(m_pBaseAPO == NULL)
    {
        SLOG(eInfo1, "Creating APOConfig Object");
        hr = m_pMeter.QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), &m_pBaseAPO);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Creating APOConfig Object, 0x%X", hr);
            goto exit;
        }
    }
    if(m_pAPO == NULL)
    {
        SLOG(eInfo1, "Creating APO Object");
        hr = m_pMeter.QueryInterface(__uuidof(IAudioProcessingObject), &m_pAPO);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Creating APO Object, 0x%X", hr);
        }
    }
exit:
    return hr;
}
