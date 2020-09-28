// Copyright(C) Microsoft. All rights reserved.

#include "stdafx.h"

CKSEndpointBaseTest::CKSEndpointBaseTest(bool bRender)
: m_pDevEndpoint(NULL), m_pIMMDevice(NULL)
{
    m_bRender = bRender;

    CreateEndpoint();
};


HRESULT CKSEndpointBaseTest::CreateEndpoint()
{
    HRESULT hr = S_OK;
    CComQIPtr<IMMEndpointInternal> spEndpointInternal;

    m_pIMMDevice = GetDeviceInfo(m_bRender)->GetIMMDevice();
    
    spEndpointInternal = m_pIMMDevice;
    if (spEndpointInternal == NULL)
    {
        SLOG(eError, "ERROR: Could not get IMMEndpointInternal interface from IMMDevice.");
    }

    SLOG(eInfo1, "Activating Device Endpoint");
    
    if (m_bRender)
    {
        CComPtr<IAudioOutputEndpointRT>         pIOutputEP = NULL;
        PAUDIO_ENDPOINT_SHARED_CREATE_PARAMS    pParams = NULL;
        PAUDIO_ENDPOINT_SHARED_CREATE_PARAMS_FOR_KS_ENDPOINTS    pKSParams = NULL;
        LPWAVEFORMATEX                          pwfx;
        PROPVARIANT                             pv;
        UINT                                    cbSize, cbwfx;
        
        pwfx   = GET_RENDER_WFX();
        cbSize = spEndpointInternal->GetClass() == eLocalEndpoint ? sizeof(AUDIO_ENDPOINT_SHARED_CREATE_PARAMS_FOR_KS_ENDPOINTS) : sizeof(AUDIO_ENDPOINT_SHARED_CREATE_PARAMS);
        cbwfx  = sizeof(WAVEFORMATEX);
        if (WAVE_FORMAT_PCM != pwfx->wFormatTag)
        {
            //  Technically if format is PCM cbSize MAY not be defined.
            cbSize += pwfx->cbSize;
            cbwfx  += pwfx->cbSize;
        }
        
        if (spEndpointInternal->GetClass() == eLocalEndpoint)
        {
            pKSParams = (PAUDIO_ENDPOINT_SHARED_CREATE_PARAMS_FOR_KS_ENDPOINTS)(new BYTE[cbSize]);

            if (NULL == pKSParams)
            {
                return (E_OUTOFMEMORY);
            }

            SecureZeroMemory(pKSParams, cbSize);
            CopyMemory(&(pKSParams->wfxDeviceFormat), pwfx, cbwfx);
        }
        else
        {
            pParams = (PAUDIO_ENDPOINT_SHARED_CREATE_PARAMS)(new BYTE[cbSize]);

            if (NULL == pParams)
            {
                return (E_OUTOFMEMORY);
            }

            SecureZeroMemory(pParams, cbSize);
            CopyMemory(&(pParams->wfxDeviceFormat), pwfx, cbwfx);
        }
        PropVariantInit(&pv);
        pv.vt = VT_BLOB;
        pv.blob.cbSize = cbSize;
        pv.blob.pBlobData = spEndpointInternal->GetClass() == eLocalEndpoint ? (BYTE*)pKSParams : (BYTE*)pParams;
        
        if (S_OK != (hr = m_pIMMDevice->Activate(__uuidof(IAudioOutputEndpointRT), CLSCTX_ALL, &pv, (void**)&pIOutputEP)))
        {
            SLOG(eError, "ERROR: Activate IAudioOutputEndpointRT returned error (0x%08lx).", hr);
        }
        else
        {
            if (S_OK != (hr = pIOutputEP->QueryInterface(__uuidof(IAudioDeviceEndpoint), (void**)&m_pDevEndpoint)))
            {
                SLOG(eError, "ERROR: QI for IAudioDeviceEndpoint returned error (0x%08lx).", hr);
            }
        }
        
        if (pParams) delete []pParams;
        if (pKSParams) delete []pKSParams;
    }
    else
    {
        CComPtr<IAudioInputEndpointRT>          pIInputEP = NULL;
        PAUDIO_ENDPOINT_SHARED_CREATE_PARAMS    pParams = NULL;
        PAUDIO_ENDPOINT_SHARED_CREATE_PARAMS_FOR_KS_ENDPOINTS    pKSParams = NULL;
        LPWAVEFORMATEX                          pwfx;
        PROPVARIANT                             pv;
        UINT                                    cbSize, cbwfx;
    
        pwfx   = GET_CAPTURE_WFX();
        cbSize = spEndpointInternal->GetClass() == eLocalEndpoint ? sizeof(AUDIO_ENDPOINT_SHARED_CREATE_PARAMS_FOR_KS_ENDPOINTS) : sizeof(AUDIO_ENDPOINT_SHARED_CREATE_PARAMS);
        cbwfx  = sizeof(WAVEFORMATEX);
        if (WAVE_FORMAT_PCM != pwfx->wFormatTag)
        {
            //  Technically if format is PCM cbSize MAY not be defined.
            cbSize += pwfx->cbSize;
            cbwfx  += pwfx->cbSize;
        }
    
        if (spEndpointInternal->GetClass() == eLocalEndpoint)
        {
            pKSParams = (PAUDIO_ENDPOINT_SHARED_CREATE_PARAMS_FOR_KS_ENDPOINTS)(new BYTE[cbSize]);

            if (NULL == pKSParams)
            {
                return (E_OUTOFMEMORY);
            }

            SecureZeroMemory(pKSParams, cbSize);
            CopyMemory(&(pKSParams->wfxDeviceFormat), pwfx, cbwfx);
        }
        else
        {
            pParams = (PAUDIO_ENDPOINT_SHARED_CREATE_PARAMS)(new BYTE[cbSize]);

            if (NULL == pParams)
            {
                return (E_OUTOFMEMORY);
            }

            SecureZeroMemory(pParams, cbSize);
            CopyMemory(&(pParams->wfxDeviceFormat), pwfx, cbwfx);
        }
        PropVariantInit(&pv);
        pv.vt = VT_BLOB;
        pv.blob.cbSize = cbSize;
        pv.blob.pBlobData = spEndpointInternal->GetClass() == eLocalEndpoint ? (BYTE*)pKSParams : (BYTE*)pParams;
    
        if (S_OK != (hr = m_pIMMDevice->Activate(__uuidof(IAudioInputEndpointRT), CLSCTX_ALL, &pv, (void**)&pIInputEP)))
        {
            SLOG(eError, "ERROR: Activate IAudioInputEndpointRT returned error (0x%08lx).", hr);
        }
        else
        {
            if (S_OK != (hr = pIInputEP->QueryInterface(__uuidof(IAudioDeviceEndpoint), (void**)&m_pDevEndpoint)))
            {
                SLOG(eError, "ERROR: QI for IAudioDeviceEndpoint returned error (0x%08lx).", hr);
            }
        }
    
        if (pParams) delete []pParams;
        if (pKSParams) delete []pKSParams;
    }
    
//    if (S_OK != (hr = pIMMEndpoint->Activate(__uuidof(IAudioDeviceEndpoint), CLSCTX_ALL, NULL, (void**)&m_pDevEndpoint)))
//    {
//        SLOG(eError, "ERROR: Device Endpoint Creation FAILED, 0x%X", hr);
//    }
    
    return (hr);
};

HRESULT CKSEndpointBaseTest::GetIMMDevice(IMMDevice **ppIMMDevice)
{
    if (NULL == m_pIMMDevice)
    {
        return (E_UNEXPECTED);
    }
    
    *ppIMMDevice = m_pIMMDevice;
    
    (*ppIMMDevice)->AddRef();
    
    return (S_OK);
}


//-------------------------------------------------------------------------
// Class: CKSEndpointBaseTest
// Method: EndpointCreate
//
// Description:
//  This method creates an instance of the KSRenderEndpoint to check if the AudioKSRenderEndpoint
//  class is registered with COM.
//   
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FAIL on Failure
//
DWORD CKSEndpointBaseTest::EndpointCreate()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;
    IAudioEndpointPtr pEndpoint = NULL;
    IAudioEndpointRTPtr pEndpointRT = NULL;
    IAudioEndpointControlPtr pEndpointControl = NULL;
    //IAudioEndpointGlitchPtr pEndpointGlitch = NULL;
    IAudioClockPtr pAudClock = NULL;
    
    IAudioDeviceEndpointPtr pDevEndpoint = NULL;

    // Depending on whether this is the Input or Output Endpoint, create instance from the relevant class
    if(m_bRender)
    {   //This is Render Endpoint
        SLOG(eInfo1, "Creating KSRenderEndpoint");
        hr = pEndpoint.CreateInstance(__uuidof(AudioKSRenderEndpoint));
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:FAILED trying to create AudioKSRenderEndpoint", dw, FNS_FAIL);
    }
    else
    {   //This is Capture Endpoint         
        SLOG(eInfo1, "Creating KSCaptureEndpoint");
        hr = pEndpoint.CreateInstance(__uuidof(AudioKSCaptureEndpoint));
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:FAILED trying to create AudioKSRenderEndpoint", dw, FNS_FAIL);
    }    

    SLOG(eInfo1, "Querying IAudioEndpointRT from KSEndpoint");
    hr = pEndpoint.QueryInterface(__uuidof(IAudioEndpointRT), &pEndpointRT);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR:FAILED trying to Query for IAudioEndpointRT", dw, FNS_FAIL);

    SLOG(eInfo1, "Querying IAudioEndpointControl from KSEndpoint");
    hr = pEndpoint.QueryInterface(__uuidof(IAudioEndpointControl), &pEndpointControl);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR:FAILED trying to Query for IAudioEndpointControl", dw, FNS_FAIL);

    /*SLOG(eInfo1, "Querying IAudioEndpointGlitch from KSEndpoint");
    hr = pEndpoint.QueryInterface(__uuidof(IAudioEndpointGlitch), &pEndpointGlitch);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR:FAILED trying to Query for IAudioEndpointGlitch", dw, FNS_FAIL);*/

    SLOG(eInfo1, "Querying IAudioClock from KSEndpoint");
    hr = pEndpoint.QueryInterface(__uuidof(IAudioClock), &pAudClock);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR:FAILED trying to Query for IAudioClock", dw, FNS_FAIL);

    SLOG(eInfo1, "Querying IAudioDeviceEndpoint from KSEndpoint");
    hr = pEndpoint.QueryInterface(__uuidof(IAudioDeviceEndpoint), &pDevEndpoint);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR:FAILED trying to Query for IAudioDeviceEndpoint", dw, FNS_FAIL);

    // Depending on whether this is the Input or Output Endpoint, check for the relevant interface
    //if(IsEqualGUID(rclsID, __uuidof(AudioKSRenderEndpoint)))
    if(m_bRender)
    {
        //This is an Output Endpoint
        IAudioOutputEndpointRTPtr pOutEndpointRT = NULL;
        SLOG(eInfo1, "Querying IAudioOutputEndpointRT from KSEndpoint");
        hr = pEndpoint.QueryInterface(__uuidof(IAudioOutputEndpointRT), &pOutEndpointRT);
        METHOD_CHECKFAIL_SETVAL(hr, "ERROR:FAILED trying to Query for IAudioOutputEndpointRT", dw, FNS_FAIL);
    }
    else
    { 
        //This is an Input Endpoint
        IAudioInputEndpointRTPtr pInEndpointRT = NULL;
        SLOG(eInfo1, "Querying IAudioInputEndpointRT from KSEndpoint");
        hr = pEndpoint.QueryInterface(__uuidof(IAudioInputEndpointRT), &pInEndpointRT);
        METHOD_CHECKFAIL_SETVAL(hr, "ERROR:FAILED trying to Query for IAudioInputEndpointRT", dw, FNS_FAIL);
    }

    if(m_bRender)
    {  //Check for IAudioProtectedOutput on KSRenderEndpoint
        IAudioProtectedOutputPtr pProtectedOutput = NULL;
        SLOG(eInfo1, "Querying IAudioProtectedOutput from KSEndpoint");
        hr = pEndpoint.QueryInterface(__uuidof(IAudioProtectedOutput), &pProtectedOutput);
        METHOD_CHECKFAIL_SETVAL(hr, "ERROR:FAILED trying to Query for IAudioProtectedOutput", dw, FNS_FAIL);        
    }

   

exit:
    return dw;
}

//-------------------------------------------------------------------------
// Class: CKSEndpointBaseTest
// Method: EndpointGetFrequency
//
// Description:
//  TestCase for IAudioClock::GetFrequency API
//   
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FAIL on Failure
//
DWORD CKSEndpointBaseTest::EndpointGetFrequency()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;
    IAudioClockPtr AudClock = NULL;        
    UINT32 u32FramesPerCall = 0;
    UINT64 u64Frequency = 0;

    _ASSERT(m_pDevEndpoint != NULL);

    if (NULL == m_pDevEndpoint)
    {
        SLOG(eError, "ERROR: NULL == m_pDevEndpoint");
        return (FNS_FAIL);
    }
 
    //Get the IAudioClock interface
    SLOG(eInfo1, "Querying IAudioClock from Endpoint");
    hr = m_pDevEndpoint.QueryInterface(__uuidof(IAudioClock), &AudClock);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:QueryInterface for IAudioClock FAILED", dw, FNS_FAIL);
    
    SLOG(eInfo1, "Initializing Endpoint");
    hr = InitializeAndSetBuffer();// Initialize using the default Audio Device with a 10ms buffer
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:Endpoint Initialization FAILED", dw, FNS_FAIL);

    SLOG(eInfo1, "Calling GetFrequency with NULL/invalid argument");
#pragma prefast(suppress: __WARNING_PASSING_FUNCTION_UNEXPECTED_NULL, "Intentionally violating contract")
    hr = AudClock->GetFrequency(NULL);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR:GetFrequency SUCCEEDED with NULL/invalid argument", dw, FNS_FAIL);
    
    // Try a legit GetFrequency call and verify if the value returned
    // is the same as the values set
    SLOG(eInfo1, "Trying a legit GetFrequency call");
    hr = AudClock->GetFrequency(&u64Frequency);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:GetFrequency call on KSEndpoint FAILED", dw, FNS_FAIL);
    LPWAVEFORMATEX lpWfx = m_bRender ? GET_RENDER_WFX() : GET_CAPTURE_WFX();
    if(u64Frequency != lpWfx->nSamplesPerSec )
    {
        SLOG(eError, "ERROR:GetFrequency returned a different value from what was set");
        SLOG(eError, "Value returned is %I64d, Expected Frequency is %ld", u64Frequency, lpWfx->nSamplesPerSec );
        dw = FNS_FAIL;        
    }
    
exit:
    return dw;
}


//-------------------------------------------------------------------------
// Class: CKSEndpointBaseTest
// Method: EndpointGetPosition
//
// Description:
//  TestCase for IAudioClock::GetPosition API 
//
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CKSEndpointBaseTest::EndpointGetPosition()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;
    IAudioClockPtr AudClock = NULL;
    UINT64 u64Position = 0;
    UINT64 u64QPCPosition = 0;

    _ASSERT(m_pDevEndpoint != NULL);

    if (NULL == m_pDevEndpoint)
    {
        SLOG(eError, "ERROR: NULL == m_pDevEndpoint");
        return (FNS_FAIL);
    }
 
    //Get the IAudioClock interface
    SLOG(eInfo1, "Querying IAudioClock from KSEndpoint");
    hr = m_pDevEndpoint.QueryInterface(__uuidof(IAudioClock), &AudClock);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:QueryInterface for IAudioClock FAILED", dw, FNS_FAIL);
    
    SLOG(eInfo1, "Initializing Endpoint");
    hr = InitializeAndSetBuffer();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:Endpoint Initialization FAILED", dw, FNS_FAIL);

    SLOG(eInfo1, "Calling GetPosition with NULL/invalid argument");
#pragma prefast(suppress: __WARNING_PASSING_FUNCTION_UNEXPECTED_NULL, "Intentionally violating contract")
    hr = AudClock->GetPosition(NULL, NULL);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR:GetPosition SUCCEEDED with NULL/invalid argument", dw, FNS_FAIL);
    
    SLOG(eInfo1, "Making a Legit GetPosition call on the KSEndpoint");
    hr = AudClock->GetPosition(&u64Position, &u64QPCPosition);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: Legit GetPosition call FAILED", dw, FNS_FAIL);
    

exit:
    return dw;
}

DWORD CKSEndpointBaseTest::TestBaseEndPointInitialize
(
    UINT_PTR    hSharedMemHandle, 
    UINT32      u32MaxFramesPerCall,
    UINT32*     pu32SharedMemorySize,
    UINT_PTR*   phSharedMemory,
    BOOL        bWriteExclModeParams,
    BOOL        bValidTestCase
)
{
    HRESULT                             hr = S_OK;
    DWORD                               dw = FNS_PASS;
    IAudioCrossProcessClientEndpointPtr CPClient = NULL;    
    CComQIPtr<IAudioDeviceEndpoint2> spAudioDeviceEndpoint2;
    CHandle hPin;
    
    m_pDevEndpoint = NULL; //releasing the endpoint object, the pin to buffer, if any
    
    hr = CreateEndpoint(); //activate the graph again
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Unable to create end point", dw, FNS_FAIL);

    if (bWriteExclModeParams)
    {
        hr = m_pDevEndpoint->WriteExclusiveModeParametersToSharedMemory((UINT_PTR)GetCurrentProcess(),
                                                                10*MFTIME1MSEC, //10 msPeriod
                                                                2*10*MFTIME1MSEC, //Buffer is 20ms
                                                                1, //Latency
                                                                pu32SharedMemorySize,
                                                                phSharedMemory
                                                               );

        spAudioDeviceEndpoint2 = m_pDevEndpoint;
        VERIFY_IS_NOT_NULL(spAudioDeviceEndpoint2);

        VERIFY_SUCCEEDED(spAudioDeviceEndpoint2->GetPinHandle(&hPin.m_h));
    }
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: WriteExclModeParams FAILED", dw, FNS_FAIL);
    hr = CPClient.CreateInstance(m_bRender ? __uuidof(AudioKSRenderEndpoint) : __uuidof(AudioKSCaptureEndpoint));
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Unable to CoCreate AudioCPClientEndpt", dw, FNS_FAIL);    
    
    //Test with different parameters
    hr = CPClient->Initialize(*phSharedMemory, NULL, reinterpret_cast<UINT_PTR>(hPin.m_h), NULL, NULL, u32MaxFramesPerCall);
    
    if ((bValidTestCase && FAILED(hr)) || (!bValidTestCase && SUCCEEDED(hr)))
    {
        dw = FNS_FAIL;
    }
exit:
    CPClient = NULL;
    m_pDevEndpoint = NULL;
    return dw;
}

//-------------------------------------------------------------------------
// Class: CKSEndpointBaseTest
// Method: EndpointCPClientInitialize
//
// Description:
//  TestCase for IAudioCrossProcessClientEndpoint::Initialize API for KSEndpoints
//  Calling this API initializes the KSEndpoint in ExclusiveMode, so this test is only
//  for Exclusive Mode
//
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CKSEndpointBaseTest::EndpointCPClientInitialize()
{    
    HRESULT     hr = S_OK;
    DWORD       dw = FNS_PASS;        
    UINT_PTR    hSharedMemHandle = NULL;
    UINT32      u32SharedMemorySize = 0;
    
    // shared buffer event isn't used by ks endpoints (second parameter of CKSBaseEndpoint::Initialize)
    //Initialize(NULL, /, 0) - invalid test case
    SLOG(eInfo1, "Initialize(NULL, /, 0)");
    dw = TestBaseEndPointInitialize(NULL, 0, &u32SharedMemorySize, &hSharedMemHandle, FALSE, FALSE);
    
    //Initialize(0xff, /, 10) - invalid test case
    SLOG(eInfo1, "Initialize(0xff, /, 10)");
    dw = (dw == FNS_FAIL)? FNS_FAIL:
         TestBaseEndPointInitialize(0xFF, 10, &u32SharedMemorySize, &hSharedMemHandle, FALSE, FALSE);

    //Initialize(hSharedMemHandle, /, 0) - valid test case
    SLOG(eInfo1, "Initialize(hSharedMemHandle, /, 0)");
    dw = (dw == FNS_FAIL)? FNS_FAIL:
         TestBaseEndPointInitialize(NULL, 0, &u32SharedMemorySize, &hSharedMemHandle, TRUE, TRUE);

    //Initialize(hSharedMemHandle, /, 480 * sizeof(FLOAT32)) - valid test case
    SLOG(eInfo1, "Initialize(hSharedMemHandle, /, 480 * sizeof(FLOAT32))");
    dw = (dw == FNS_FAIL)? FNS_FAIL:
         TestBaseEndPointInitialize(NULL, 480 * sizeof(FLOAT32), &u32SharedMemorySize, &hSharedMemHandle, TRUE, TRUE);
    
    //abandon the remaining test case if the above did not succeed
    if (FNS_FAIL == dw)
    {
        goto exit;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    //Check that the client can specify the looped buffer size in Exclusive Mode
    //See bug 1036178    
    hr = CheckExclusiveLoopedBufferSize();
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR:Exclusive Mode Looped Buffer size test FAILED", dw, FNS_FAIL);        

exit:
    return dw;
}

//-------------------------------------------------------------------------
// Class: CKSEndpointBaseTest
// Method: EndpointSetBuffer
//
// Description:
//  TestCase for IAudioDeviceEndpoint::SetBuffer API 
//
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CKSEndpointBaseTest::EndpointSetBuffer()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;
    MFTIME mfMaxPeriod = 0;
    UINT32 u32LatencyCoeffn = 0;
    MFTIME mfLatency = 0;    
    IAudioEndpointPtr pEndpoint = NULL;

    _ASSERT(m_pDevEndpoint != NULL);   
    
    if (NULL == m_pDevEndpoint)
    {
        SLOG(eError, "ERROR: NULL == m_pDevEndpoint");
        return (FNS_FAIL);
    }

    SLOG(eInfo1, "Calling SetBuffer with NULL/invalid arguments");
    SLOG(eInfo1, "Calling SetBuffer with MaxPeriod = NULL");
    mfMaxPeriod = NULL;
    hr = m_pDevEndpoint->SetBuffer(mfMaxPeriod, u32LatencyCoeffn);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR:SetBuffer SUCCEEDED with MaxPeriod = NULL", dw, FNS_FAIL);
    SLOG(eInfo1, "Calling SetBuffer with MaxPeriod = MAX_UINT64");
    mfMaxPeriod = (UINT64)-1;
    hr = m_pDevEndpoint->SetBuffer(mfMaxPeriod, u32LatencyCoeffn);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR:SetBuffer SUCCEEDED with MaxPeriod = MAX_UINT64", dw, FNS_FAIL);
    SLOG(eInfo1, "Calling SetBuffer with a very large u32LatencyCoeffn( = 0x40000000)");    
    mfMaxPeriod = 10 * MFTIME1MSEC;
    u32LatencyCoeffn = (UINT32)0x40000000;
    //The following is a test to check that SetBuffer handles large values properly.
    //SLOG(eInfo1, "FOLLOWING FAILURE IS EXPECTED, SEE BUG 985418");
    //dw = FNS_FAIL; //hard code failure now, since this case causes a divide by 0 error
    hr = m_pDevEndpoint->SetBuffer(mfMaxPeriod, u32LatencyCoeffn);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR:SetBuffer SUCCEEDED with u32LatencyCoeffn = 0x40000000", dw, FNS_FAIL);

    //Enpoint is probably initialized by now and the buffer has been set, if the above call SUCCEEDED
    //Create a new Endpoint and re-initialize it
    m_pDevEndpoint = NULL;
    hr = CreateEndpoint();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:Device Endpoint creation FAILED", dw, FNS_FAIL);
    SLOG(eInfo1, "Making a Valid SetBuffer call");
    mfMaxPeriod = 10 * MFTIME1MSEC;
    u32LatencyCoeffn = 1; 
    hr = m_pDevEndpoint->SetBuffer(mfMaxPeriod, u32LatencyCoeffn);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: Legit SetBuffer call FAILED", dw, FNS_FAIL);

    //Check that specifying Latency=0 applies minimum latency
    //Enpoint is probably initialized by now and the buffer has been set, if the above call SUCCEEDED
    //Create a new Endpoint and re-initialize it
    m_pDevEndpoint = NULL;
    hr = CreateEndpoint();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:Device Endpoint creation FAILED", dw, FNS_FAIL);
    SLOG(eInfo1, "Check to see if specifying 0 latency applies minimum latency");
    mfMaxPeriod = 10 * MFTIME1MSEC;
    u32LatencyCoeffn = 0; 
    hr = m_pDevEndpoint->SetBuffer(mfMaxPeriod, u32LatencyCoeffn);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: Legit SetBuffer call FAILED", dw, FNS_FAIL);

    hr = m_pDevEndpoint.QueryInterface(__uuidof(IAudioEndpoint), &pEndpoint);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:QI on Device Endpoint for IAudioEndpoint FAILED", dw, FNS_FAIL);
    hr = pEndpoint->GetLatency(&mfLatency);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: GetLatency method call on IAudioEndpoint FAILED", dw, FNS_FAIL);
    if(0 == mfLatency)
    {
        SLOG(eError, "ERROR: SetBuffer did not set minimum latency when 0 is passed in");
        dw = FNS_FAIL;
    }
    else
    {
        SLOG(eInfo1, "SetBuffer set the latency to %d, when a value of 0 was passed in", mfLatency);
    }
    

exit:    
    return dw;
}

//-------------------------------------------------------------------------
// Class: CKSEndpointBaseTest
// Method: EndpointWriteExclusiveModeParametersToSharedMemory
//
// Description:
//  TestCase for IAudioDeviceEndpoint::WriteExclusiveModeParametersToSharedMemory API 
//
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CKSEndpointBaseTest::EndpointWriteExclusiveModeParametersToSharedMemory()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;
    HANDLE hProcess = NULL;
    UINT32 ui32SharedMemorySize = 0;
    UINT_PTR uiSharedMemory = NULL;
    MFTIME mfPeriod = 0;
    MFTIME mfBufferDuration = 0;
    UINT32 u32LatencyCoeffn = 0;

    _ASSERT(m_pDevEndpoint != NULL);

    if (NULL == m_pDevEndpoint)
    {
        SLOG(eError, "ERROR: NULL == m_pDevEndpoint");
        return (FNS_FAIL);
    }

    SLOG(eInfo1, "Calling WriteExclusiveModeParametersToSharedMemory with NULL/invalid arguments");
    SLOG(eInfo1, "Calling with all params = NULL");
    hr = m_pDevEndpoint->WriteExclusiveModeParametersToSharedMemory((UINT_PTR)NULL, NULL, NULL, NULL, NULL, NULL);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR:WriteExclusiveModeParametersToSharedMemory SUCCEEDED with all NULL params", dw, FNS_FAIL);

    SLOG(eInfo1, "Calling with NULL hTargetProcess ");
    hProcess = NULL;
    hr = m_pDevEndpoint->WriteExclusiveModeParametersToSharedMemory((UINT_PTR)hProcess, mfPeriod, mfBufferDuration,
                                                    u32LatencyCoeffn, &ui32SharedMemorySize, &uiSharedMemory);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR:WriteExclusiveModeParametersToSharedMemory FAILED with NULL hTargetProcess", dw, FNS_FAIL);

    SLOG(eInfo1, "Calling with NULL sharedmemorysize ptr");
    hProcess = GetCurrentProcess();
    hr = m_pDevEndpoint->WriteExclusiveModeParametersToSharedMemory((UINT_PTR)hProcess, mfPeriod, mfBufferDuration,
                                                    u32LatencyCoeffn, NULL, &uiSharedMemory);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR:WriteExclusiveModeParametersToSharedMemory SUCCEEDED with NULL sharedmemorysize ptr", dw, FNS_FAIL);

    SLOG(eInfo1, "Calling with NULL sharedmemory ptr");
    hProcess = GetCurrentProcess();
    hr = m_pDevEndpoint->WriteExclusiveModeParametersToSharedMemory((UINT_PTR)hProcess, mfPeriod, mfBufferDuration,
                                                    u32LatencyCoeffn, &ui32SharedMemorySize, NULL);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR:WriteExclusiveModeParametersToSharedMemory SUCCEEDED with NULL sharedmemory ptr", dw, FNS_FAIL);

    SLOG(eInfo1, "Making a valid WriteExclusiveModeParametersToSharedMemory call");
    hr = m_pDevEndpoint->WriteExclusiveModeParametersToSharedMemory((UINT_PTR)hProcess, mfPeriod, mfBufferDuration,
                                                    u32LatencyCoeffn, &ui32SharedMemorySize, &uiSharedMemory);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR:Valid WriteExclusiveModeParametersToSharedMemory call FAILED", dw, FNS_FAIL);

    // After a valid WriteExclusiveModeParamsToSharedMemory(...) call, we now have to close the duplicate pin handle held by the KS endpoint.  Failure to
    // do this will monopolize the endpoint for the lifetime of this process.
    if (SUCCEEDED(hr))
    {
        PVOID pSharedMemoryBlock = NULL;

        if ((0 == ui32SharedMemorySize)||(NULL == uiSharedMemory))
        {
            SLOG(eError, "ERROR: WriteExclusiveModeParametersToSharedMemory succeeded, but returned an invalid shared memory handle!");
            dw = FNS_FAIL;
            goto exit;
        }
        
        if (ui32SharedMemorySize < sizeof(AUDIO_ENDPOINT_EXCLUSIVE_CREATE_PARAMS))
        {
            SLOG(eError, "ERROR: WriteExclusiveModeParametersToSharedMemory succeeded, but returned too small of a shared memory buffer!");
            dw = FNS_FAIL;
            goto exit;
        }
    
        pSharedMemoryBlock = MapViewOfFile(reinterpret_cast<HANDLE>(uiSharedMemory),
                                           FILE_MAP_ALL_ACCESS,                         // write access necessary to zero the handle
                                           0,
                                           0,
                                           ui32SharedMemorySize);
        if (!pSharedMemoryBlock)
        {
            SLOG(eError, "ERROR: MapViewOfFile signalled error 0x%08X when attempting to access shared memory buffer returned by WriteExclusiveModeParametersToSharedMemory", GetLastError());
            dw = FNS_FAIL;
            goto exit;
        }
    
        // close the pin handle
        SAFE_CLOSE_HANDLE(reinterpret_cast<HANDLE&>(reinterpret_cast<PAUDIO_ENDPOINT_EXCLUSIVE_CREATE_PARAMS>(pSharedMemoryBlock)->hConnection));

        if (!UnmapViewOfFile(pSharedMemoryBlock))
        {
            SLOG(eError, "ERROR: UnmapViewOfFile signalled error 0x%08X", GetLastError());
            dw = FNS_FAIL;
            goto exit;
        }
    
    }

exit:
    return dw;
}

//-------------------------------------------------------------------------
// Class: CKSEndpointBaseTest
// Method: EndpointGetRTCaps
//
// Description:
//  TestCase for IAudioDeviceEndpoint::GetRTCaps API 
//
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CKSEndpointBaseTest::EndpointGetRTCaps()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;
    BOOL bIsRTCapable = FALSE;
    _ASSERT(m_pDevEndpoint != NULL);

    if (NULL == m_pDevEndpoint)
    {
        SLOG(eError, "ERROR: NULL == m_pDevEndpoint");
        return (FNS_FAIL);
    }

    SLOG(eInfo1, "Calling GetRTCaps with NULL Param");
    hr = m_pDevEndpoint->GetRTCaps(NULL);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR:GetRTCaps SUCCEEDED with NULL Param", dw, FNS_FAIL);

    //Make a valid call and check that it succeeds
    SLOG(eInfo1, "Making a valid GetRTCaps call");
    hr = m_pDevEndpoint->GetRTCaps(&bIsRTCapable);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR:Valid GetRTCaps call FAILED", dw, FNS_FAIL);
    
    return dw;
}

//-------------------------------------------------------------------------
// Class: CKSEndpointBaseTest
// Method: EndpointGetFrameFormat
//
// Description:
//  TestCase for IAudioEndpoint::GetFrameFormat API 
//
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CKSEndpointBaseTest::EndpointGetFrameFormat()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;
    IAudioEndpointPtr pEndpoint = NULL;
    WAVEFORMATEX* pWfx = NULL;
    
    _ASSERT(m_pDevEndpoint != NULL);

    if (NULL == m_pDevEndpoint)
    {
        SLOG(eError, "ERROR: NULL == m_pDevEndpoint");
        return (FNS_FAIL);
    }

    SLOG(eInfo1, "QI for IAudioEndpoint");
    hr = m_pDevEndpoint.QueryInterface(__uuidof(IAudioEndpoint), &pEndpoint);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:QI for IAudioEndpoint FAILED", dw, FNS_FAIL);

    SLOG(eInfo1, "Calling GetFrameFormat with NULL param");
    hr = pEndpoint->GetFrameFormat(NULL);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR:GetFrameFormat SUCCEEDED with NULL", dw, FNS_FAIL);

    SLOG(eInfo1, "Making a valid GetFrameFormat call");
    hr = pEndpoint->GetFrameFormat(&pWfx);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR:Valid GetFrameFormat call FAILED", dw, FNS_FAIL);

exit:
    return dw;
}

//-------------------------------------------------------------------------
// Class: CKSEndpointBaseTest
// Method: EndpointGetLatency
//
// Description:
//  TestCase for IAudioEndpoint::GetLatency API 
//
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CKSEndpointBaseTest::EndpointGetLatency()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;
    IAudioEndpointPtr pEndpoint = NULL;
    MFTIME mfLatency = 0;
    
    _ASSERT(m_pDevEndpoint != NULL);

    if (NULL == m_pDevEndpoint)
    {
        SLOG(eError, "ERROR: NULL == m_pDevEndpoint");
        return (FNS_FAIL);
    }

    SLOG(eInfo1, "QI for IAudioEndpoint");
    hr = m_pDevEndpoint.QueryInterface(__uuidof(IAudioEndpoint), &pEndpoint);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:QI for IAudioEndpoint FAILED", dw, FNS_FAIL);

    //Initialize the Endpoint
    hr = InitializeAndSetBuffer();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:Initialization on the Endpoint FAILED", dw, FNS_FAIL);

    SLOG(eInfo1, "Calling GetLatency with NULL param");
    hr = pEndpoint->GetLatency(NULL);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR:GetLatency SUCCEEDED with NULL", dw, FNS_FAIL);

    SLOG(eInfo1, "Making a valid GetLatency call");
    hr = pEndpoint->GetLatency(&mfLatency);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR:Valid GetLatency call FAILED", dw, FNS_FAIL);
    //This is a hardware endpoint, so latency can't be 0
    if(0 == mfLatency)
    {
        SLOG(eError, "Hardware Endpoint returned 0 latency");
        dw = FNS_FAIL;
    }
    //Depending on what type of Endpoint this is, i.e. Render/Capture....Standard/RT the latency values are different
    //Make sure the right values are returned
    if(GetModule()->IsRt(m_bRender))
    { //Device is RT
        if(m_bRender)
        { //Render endpoint
        }
        else
        { //Capture endpoint
        }
    }
    else
    { //Device is not RT
        if(m_bRender)
        { //Render endpoint
        }
        else
        { //Capture endpoint
        }
    }

exit:
    return dw;
}

//-------------------------------------------------------------------------
// Class: CKSEndpointBaseTest
// Method: EndpointGetFramesPerPacket
//
// Description:
//  TestCase for IAudioEndpoint::GetFramesPerPacket API 
//
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CKSEndpointBaseTest::EndpointGetFramesPerPacket()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;
    IAudioEndpointPtr pEndpoint = NULL;
    UINT32 u32FramesPerPacket = 0;
    
    _ASSERT(m_pDevEndpoint != NULL);

    if (NULL == m_pDevEndpoint)
    {
        SLOG(eError, "ERROR: NULL == m_pDevEndpoint");
        return (FNS_FAIL);
    }

    SLOG(eInfo1, "QI for IAudioEndpoint");
    hr = m_pDevEndpoint.QueryInterface(__uuidof(IAudioEndpoint), &pEndpoint);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:QI for IAudioEndpoint FAILED", dw, FNS_FAIL);

    //Initialize the Endpoint
    hr = InitializeAndSetBuffer();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:Initialization on the Endpoint FAILED", dw, FNS_FAIL);

    SLOG(eInfo1, "Calling GetFramesPerPacket with NULL param");
    hr = pEndpoint->GetFramesPerPacket(NULL);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR:GetFramesPerPacket SUCCEEDED with NULL", dw, FNS_FAIL);

    SLOG(eInfo1, "Making a valid GetFramesPerPacket call");
    hr = pEndpoint->GetFramesPerPacket(&u32FramesPerPacket);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR:Valid GetFramesPerPacket call FAILED", dw, FNS_FAIL);    

exit:
    return dw;
}




//-------------------------------------------------------------------------
// Class: CKSEndpointBaseTest
// Method: EndpointTestPositions
//
// Description:
//  This is the testcase for verifying functional behavior of GetPosition and the KSEndpoint
//  The test sets up the capture/render graph and continually keeps stepping through the processing pass
//  until there is no more data in the Memory Endpoint.
//
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CKSEndpointBaseTest::EndpointTestPositions()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;
    FLOAT32* pfBuffer = NULL;
    HANDLE hTimer = NULL;
    IAudioEndpointRTPtr InputEndpoint = NULL;
    IAudioEndpointRTPtr OutputEndpoint = NULL;
    CComObject<CMemoryAudioEndpoint>* pMemEndpoint = new CComObject<CMemoryAudioEndpoint>();
    //IAudioEndpointControlPtr DevControl = NULL;
    //IAudioEndpointControlPtr MemControl = NULL;
    CGraphRepresentation* pGraph = NULL;
    IAudioClock*  AudioClock = NULL;
    AUDIO_FRAME_FORMAT af;
    LPDWORD* ppVoid = NULL;

    _ASSERT(m_pDevEndpoint != NULL);

    if (NULL == m_pDevEndpoint)
    {
        SLOG(eError, "ERROR: NULL == m_pDevEndpoint");
        return (FNS_FAIL);
    }

    //Initialize the Endpoint with the Default device and 10ms period.
    SLOG(eInfo1, "Initializing Endpoint with default device");
    hr = InitializeAndSetBuffer();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:Endpoint Initialization FAILED", dw, FNS_FAIL);

    //Allocate memory for 500ms of the sinewave at the Device's default format.
    //Get the device's default 16bit format
    LPWAVEFORMATEX lpwfx = GET_RENDER_WFX();    
    
    // half a second of FLOAT32data at devices Sample Rate
    DWORD cbBufferSize = sizeof(FLOAT32) * lpwfx->nChannels * lpwfx->nSamplesPerSec * 0.5;
    pfBuffer = (FLOAT32*)LocalAlloc(LMEM_FIXED, cbBufferSize);
    _ASSERT(pfBuffer != 0);
    
    //If Render, then Generate sine wave else just zero out buffer memory
    if(m_bRender)
    { //Render Endpoint, generate sine wave
        CComPtr<IBasicLog> pBasicLog;
        WAVEFORMATEX wfxFloat;

        SLOG(eInfo1, "Generating Input SineWave");
        wfxFloat.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
        wfxFloat.nChannels = lpwfx->nChannels;
        wfxFloat.nSamplesPerSec = lpwfx->nSamplesPerSec;
        wfxFloat.wBitsPerSample = sizeof(FLOAT32) * 8;
        wfxFloat.nBlockAlign = wfxFloat.nChannels * wfxFloat.wBitsPerSample / 8;
        wfxFloat.nAvgBytesPerSec = wfxFloat.nBlockAlign * wfxFloat.nSamplesPerSec;
        wfxFloat.cbSize = 0;

        hr = g_IShell->GetBasicLog(&pBasicLog);
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Could not get a basic logger", dw, FNS_FAIL);
     
        hr = FillBufferWithSineSignal(
            pBasicLog, XFAIL,
            &wfxFloat,
            1.0, // amplitude
            1000.0, // frequency
            0.0, // initial phase,
            0.0, // dc
            Method_NoDithering,
            reinterpret_cast<PBYTE>(pfBuffer),
            cbBufferSize / wfxFloat.nBlockAlign,
            cbBufferSize
        );
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Could not fill buffer", dw, FNS_FAIL);
    }
    else
    { //Capture Endpoint, Zero out buffer memory
        ZeroMemory(pfBuffer, cbBufferSize);
    }

    //Initialize MemoryEndpoint
    //Memory Endpoint is FLOAT format
    BUILD_FORMAT(&af, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, lpwfx->nChannels, 4, 32, lpwfx->nSamplesPerSec);    
    SLOG(eInfo1, "Initializing Memory Endpoint");
    hr = pMemEndpoint->Initialize(&af, pfBuffer, cbBufferSize, (lpwfx->nSamplesPerSec / 1000) * 5, m_bRender);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:Initializing Memory Endpoint FAILED", dw, FNS_FAIL);
    //AddRef the Memory Endpoint for local use. This is just a safeguard, since CGraphRep will AddRef the Endpoints, 
    // and since the Graph is destroyed in the cleanup, we are assured that MemEndpoint will always be
    // available for our use, but nevertheless
    pMemEndpoint->AddRef();
    
    // Get the respective IAudioEndpointRT* for the Input and Output Endpoints
    if(m_bRender)
    { //Input and Output Endpoints reverse depending on whether this is a KSRenderEndpoint or KSCaptureEndpoint
        SLOG(eInfo1, "Querying for IAudioEndpointRT from KSEndpoint");
        hr = m_pDevEndpoint.QueryInterface(__uuidof(IAudioEndpointRT), (void**)&OutputEndpoint);
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:Query for IAudioEndpointRT from KSEndpoint FAILED", dw, FNS_FAIL);
        SLOG(eInfo1, "Querying for IAudioEndpointRT from MemEndpoint");
        hr = pMemEndpoint->QueryInterface(__uuidof(IAudioEndpointRT), (void**)&InputEndpoint);
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:Query for IAudioEndpointRT from MemEndpoint FAILED", dw, FNS_FAIL);
    }
    else
    {
        SLOG(eInfo1, "Querying for IAudioEndpointRT from KSEndpoint");
        hr = m_pDevEndpoint.QueryInterface(__uuidof(IAudioEndpointRT), (void**)&InputEndpoint);
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:Query for IAudioEndpointRT from KSEndpoint FAILED", dw, FNS_FAIL);
        SLOG(eInfo1, "Querying for IAudioEndpointRT from MemEndpoint");
        hr = pMemEndpoint->QueryInterface(__uuidof(IAudioEndpointRT), (void**)&OutputEndpoint);
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:Query for IAudioEndpointRT from MemEndpoint FAILED", dw, FNS_FAIL);
    }
    //Create the Graphs, Initialize Pumps with NON_RT, Period 10ms and Budget 4ms
    SLOG(eInfo1, "Creating %s Graph", m_bRender ? "Render" : "Capture");
    hr = m_bRender ? CreateRenderGraph(&pGraph, InputEndpoint, OutputEndpoint)
                   : CreateCaptureGraph(&pGraph, InputEndpoint, OutputEndpoint);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:Graph Creation FAILED", dw, FNS_FAIL);       
    
    // Start the Pump, with a Period of 10msec.
    // Memory Endpoint has 500msec of data. While the Pump is running,
    // Keep making GetPosition calls on the Device Endpoint till the MemoryEndpoint runs out of data 
    //     
    MFTIME mfPumpPeriod = 10 * MFTIME1MSEC;
    MFTIME mfStartTime = 0;
    hr = pGraph->StartPump(&mfStartTime, &mfPumpPeriod);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Starting Audio Pump FAILED", dw, FNS_FAIL);      
    
    SLOG(eInfo1, "Querying KS Endpoint for AudioClock");
    hr = m_bRender ? OutputEndpoint->QueryInterface(__uuidof(IAudioClock), (void**)&AudioClock)
                   : InputEndpoint->QueryInterface(__uuidof(IAudioClock), (void**)&AudioClock);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: QueryInterface for IAudioClock FAILED on the KSEndpoint", dw, FNS_FAIL);      
    //// Need to pass in the Graph as well as the dw,
    //// hence the array below of LPVOID's
     ppVoid = new LPDWORD[4];
    _ASSERT(ppVoid != NULL);
    BOOL bStop = FALSE;
    UINT64 u64LastPosition = 0;
    ppVoid[0] = reinterpret_cast<LPDWORD>(AudioClock);
    ppVoid[1] = reinterpret_cast<LPDWORD>(&dw);
    ppVoid[2] = reinterpret_cast<LPDWORD>(&bStop);
    ppVoid[3] = reinterpret_cast<LPDWORD>(&u64LastPosition);
    //SLOG(eInfo1, "Creating TimerQueueTimer");
    SLOG(eInfo1, "Creating TimerQueueTimer");
    if(!CreateTimerQueueTimer(&hTimer, NULL, TimerCallback, (LPVOID)ppVoid, 0, 10, WT_EXECUTEINTIMERTHREAD))
    { //Timer creation failed
        SLOG(eError, "ERROR: TimerQueueTimer creation FAILED, %d", GetLastError());
        dw = FNS_FAIL;
        goto exit;
    }

    // Check if the MemEndpoint is done, we continuously poll for the Overflow flag to see if all the data has
    // been read from the Memory Endpoint
    bool bOverflow = false;
    bOverflow = pMemEndpoint->GetOverflow();
    // while the MemEndpoint has space and nothing has failed
    while( (!bOverflow) && (FNS_PASS == dw))
    {
        Sleep(50);
        bOverflow = pMemEndpoint->GetOverflow();
    }
    //MemEndpoint is done, The graph will no longer process data,
#pragma prefast(suppress: __WARNING_UNUSED_SCALAR_ASSIGNMENT, "for the benefit of hTimer")
    bStop = TRUE;    

    //We're done, so stop the pump, and the endpoints.
    hr = pGraph->Stop();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Stopping Audio Pump FAILED", dw, FNS_FAIL);

    //// The following Wait is to know when the Processing has been done.
    //DWORD dwWait = WaitForSingleObjectEx(m_hTimerEvent, INFINITE, FALSE);
    //if(WAIT_OBJECT_0 != dwWait)
    //{ //Timer creation failed
    //    SLOG(eError, "ERROR: Wait for Timer Event FAILED");
    //    dw = FNS_FAIL;
    //    goto exit;
    //}
    //Delete the timer, wait for the callback to finish processing if the period has already expired
    SLOG(eInfo1, "Deleting Timer");
    if(!DeleteTimerQueueTimer(NULL, hTimer, INVALID_HANDLE_VALUE))
    { //Timer deletion failed
        SLOG(eError, "ERROR: Timer deletion FAILED, %d", GetLastError());
        dw = FNS_FAIL;
        goto exit;
    }

exit:
    InputEndpoint = NULL;
    OutputEndpoint = NULL;   
    SAFE_DELETE_ARRAY(ppVoid);
    SAFE_RELEASE_POINTER(AudioClock);    
    SAFE_FREE_POINTER(pfBuffer);    
    SAFE_DELETE_POINTER(pGraph);
    SAFE_RELEASE_POINTER(pMemEndpoint);
    return dw;
}

//-------------------------------------------------------------------------
// Class: CKSEndpointBaseTest
// Method: EndpointGetPadding
//
// Description:
//  TestCase for IAudioEndpointRT::GetPadding API 
//
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CKSEndpointBaseTest::EndpointGetCurrentPadding()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;    
    IAudioEndpointRTPtr pEndpointRT;    
    MFTIME mfPadding = 0;
    AE_CURRENT_POSITION aeCurrentPosition;

    _ASSERT(m_pDevEndpoint != NULL);
    
    if (NULL == m_pDevEndpoint)
    {
        SLOG(eError, "ERROR: NULL == m_pDevEndpoint");
        return (FNS_FAIL);
    }

    SLOG(eInfo1, "Querying KS Endpoint for IAudioEndpointRT");
    hr = m_pDevEndpoint.QueryInterface(__uuidof(IAudioEndpointRT), &pEndpointRT);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:Query for IAudioEndpointRT FAILED", dw, FNS_FAIL);

    //Initialize the Endpoint with the Default device.
    SLOG(eInfo1, "Initializing Endpoint with default device");
    hr = InitializeAndSetBuffer();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:Endpoint Initialization FAILED", dw, FNS_FAIL);        

    //Try method with NULL parameter
    SLOG(eInfo1, "Trying a legit GetCurrentPadding call");
    pEndpointRT->GetCurrentPadding(&mfPadding, &aeCurrentPosition);
    
exit:    
    return dw;
}


//-------------------------------------------------------------------------
// Class: CKSEndpointBaseTest
// Method: EndpointGetTimeStamp
//
// Description:
//  TestCase for IAudioEndpointRT::GetCurrentTimeStamp API 
//
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CKSEndpointBaseTest::EndpointGetTimeStamp()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_FAIL;
    _ASSERT(m_pDevEndpoint != NULL);
    
    if (NULL == m_pDevEndpoint)
    {
        SLOG(eError, "ERROR: NULL == m_pDevEndpoint");
        return (FNS_FAIL);
    }
    
    SLOG(eError, "Test NOT IMPLEMENTED");

    return dw;
}

//-------------------------------------------------------------------------
// Class: CKSEndpointBaseTest
// Method: TimerCallback
//
// Description:
//  This is the Timer Callback that does all the processing for the GetCurrentPadding call.
//  Basically it gets called at a Periodic Rate and on every pass it steps through one Processing Pass
//  on the graph, making sure that the Paddings returned are OK.
//  At the end of all the processing, it set an event which causes the Timer
//
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
void CKSEndpointBaseTest::TimerCallback
(
 LPVOID lpParam,
 BOOLEAN TimerOrWaitFired
)
{    
    LPDWORD* pTemp = reinterpret_cast<LPDWORD*>(lpParam);    
    IAudioClock* AudClock = reinterpret_cast<IAudioClock*>(pTemp[0]);
    DWORD* pdw  = reinterpret_cast<PDWORD>(pTemp[1]); // The second value in the array is PDWORD, return code
    PBOOL pbStop = reinterpret_cast<PBOOL>(pTemp[2]); // Third value is BOOL flag to stop processing
    UINT64* pu64LastPosition = reinterpret_cast<UINT64*>(pTemp[3]); //Third value holds the last position of the stream
    UINT64 u64CurrPosition = 0;
    UINT64 u64QPCPosition = 0;
    HRESULT hr = S_OK;

    SLOG(eInfo3, "Timer Callback");
    if(!*pbStop) //if we have not been instructed to stop.
    {          
        SLOG(eInfo3, "Calling GetPosition on KS Endpoint");
        hr = AudClock->GetPosition(&u64CurrPosition, &u64QPCPosition);
        if(FAILED(hr))
        {
            //This thing shouldn't have failed, log an error, Set the event and bail
            SLOG(eError, "ERROR: GetPosition on KS Endpoint FAILED, 0x%X", hr);        
            *pdw = FNS_FAIL;
            goto exit;
        }
        SLOG(eInfo3, "Position reported is %I64d", u64CurrPosition);
        if(*pu64LastPosition > u64CurrPosition)
        {
            //This shouldn't have happened, log an error and bail
            SLOG(eInfo1, "ERROR: Position going backwards"); 
            SLOG(eInfo1, "LastPosition: %I64d, Current Position: %I64d", *pu64LastPosition, u64CurrPosition); 
            *pdw = FNS_FAIL;
        }
        /*if(u64Position == u64CurrPosition)
        {
            SLOG(eError, "WARNING: Position appears to be stuck"); 
            SLOG(eError, "LastPosition: %I64d, Current Position: %I64d", u64Position, u64CurrPosition);             
        }*/
        *pu64LastPosition = u64CurrPosition;
    }
    //else
    //{
    //    // The test is done and we have been instructed to Stop
    //    // re-initialize the static variable, incase this test case is run again
    //    u64Position = 0;
    //}
exit:
    return;
}

//-------------------------------------------------------------------------
// Class: CKSEndpointBaseTest
// Method: SetupRenderGraph
//
// Description:
//  This method sets up the render graph a
//  
//  
//  
// Parameters:
//      CGraphRepresentation** ppGraph
//              This is the entire graph, on successful return it contains a valid, CGraphRep,
//              that has one format APO to go from FLOAT32 to int16,
//              The graph is built and intialized here
//
// Return values (HRESULT):
//     Return S_OK on Success, appropriate error on Failure
//
HRESULT 
CKSEndpointBaseTest::CreateRenderGraph
(
 CGraphRepresentation** ppGraph,
 IAudioEndpointRT* InputEndpoint,
 IAudioEndpointRT* OutputEndpoint
)
{
    HRESULT hr = S_OK;
    CGraphRepresentation* pGraph = NULL;
    UINT32 u32MaxFramesPerCall = 0;

    _ASSERT(InputEndpoint != NULL);
    _ASSERT(OutputEndpoint != NULL);

    // The following are used for building Local Graph
    // Local graph descriptors used apo's
    GRAPH_APO_DESCRIPTOR aApoDescriptors[] = { { APO_FMT_f32i16, NULL } };	//Format converter
    // Bindings between apo's and connections use only one connection
    GRAPH_BINDING_DESCRIPTOR aBindingDescriptors[] = { { 0, 0, true  }, { 0, 1, false } };   	
	// Connection descriptors
    GRAPH_CONNECTION_DESCRIPTOR aConnectionDescriptors[2]; 
    ZeroMemory(aConnectionDescriptors, sizeof(aConnectionDescriptors[0])*2);

    LPWAVEFORMATEX lpWfx = GET_RENDER_WFX();
    
    //Build the format for APO Input Connection, FLOAT32 format
    BUILD_FORMAT(&aConnectionDescriptors[0].FrameFormat,
                 KSDATAFORMAT_SUBTYPE_IEEE_FLOAT,
                 lpWfx->nChannels,
                 sizeof(FLOAT32),
                 sizeof(FLOAT32) * 8,
                 lpWfx->nSamplesPerSec);
    //Build the format for APO Output Connection, Device's preferred 16 bit format
    AUDIO_FRAME_FORMAT af;    
    WEX2AF(GET_RENDER_WFX(), &af);
    CopyMemory(&aConnectionDescriptors[1].FrameFormat, &af, sizeof(AUDIO_FRAME_FORMAT));

    //Calculate the MaxNumber of frames for 10ms period at the Device's preferred sample rate
    u32MaxFramesPerCall = (lpWfx->nSamplesPerSec / 1000) * 10;
    aConnectionDescriptors[0].ConnectionDescriptor.u32MaxFrameCount =  u32MaxFramesPerCall;
    aConnectionDescriptors[1].ConnectionDescriptor.u32MaxFrameCount =  u32MaxFramesPerCall;
    pGraph = new CGraphRepresentation(aConnectionDescriptors, 2, //2 connection descriptors
                                        aBindingDescriptors, 2, //2 binding descriptors
                                        aApoDescriptors, 1, //1 Apo descriptor
                                        u32MaxFramesPerCall, //Input buffer in frames
                                        u32MaxFramesPerCall, //Output buffer in frames
                                        OutputEndpoint, //Output endpoint
                                        1000, //Optional sine freq, will not be used
                                        InputEndpoint //Input endpoint
                                        );
    _ASSERT(pGraph != NULL);
    SLOG(eInfo1, "Building Graph");
    hr = pGraph->BuildGraph(); //This call connects the Endpoints, Connections and the respective APO's
    METHOD_CHECKFAIL_EXIT(hr, "ERROR:Build Graph on GraphRepresentation FAILED");

    *ppGraph = pGraph;

exit:
    if(FAILED(hr))
    {
        SAFE_DELETE_POINTER(pGraph);
    }

    return hr;
}

//-------------------------------------------------------------------------
// Class: CKSEndpointBaseTest
// Method: SetupCaptureGraph
//
// Description:
//  This method sets up the capture graph a
//  
//  
//  
// Parameters:
//      CGraphRepresentation** ppGraph
//              This is the entire graph, on successful return it contains a valid, CGraphRep,
//              that has one format APO to go from int16 to FLOAT32,
//              The graph is built and intialized here
//
// Return values (HRESULT):
//     Return S_OK on Success, appropriate error on Failure
//
HRESULT 
CKSEndpointBaseTest::CreateCaptureGraph
(
 CGraphRepresentation** ppGraph,
 IAudioEndpointRT* InputEndpoint,
 IAudioEndpointRT* OutputEndpoint
)
{
    HRESULT hr = S_OK;
    CGraphRepresentation* pGraph = NULL;
    UINT32 u32MaxFramesPerCall = 0;

    _ASSERT(InputEndpoint != NULL);
    _ASSERT(OutputEndpoint != NULL);

    // The following are used for building Local Graph
    // Local graph descriptors used apo's
    GRAPH_APO_DESCRIPTOR aApoDescriptors[] = { { APO_FMT_i16f32, NULL } };	//Format converter
    // Bindings between apo's and connections use only one connection
    GRAPH_BINDING_DESCRIPTOR aBindingDescriptors[] = { { 0, 0, true  }, { 0, 1, false } };   	
	// Connection descriptors
    GRAPH_CONNECTION_DESCRIPTOR aConnectionDescriptors[2]; 
    ZeroMemory(aConnectionDescriptors, sizeof(aConnectionDescriptors[0])*2);

    LPWAVEFORMATEX lpWfx = GET_CAPTURE_WFX();
    
    //Build the format for APO Input Connection, Device's preferred 16 bit format
    AUDIO_FRAME_FORMAT af;    
    WEX2AF(GET_RENDER_WFX(), &af);
    CopyMemory(&aConnectionDescriptors[0].FrameFormat, &af, sizeof(AUDIO_FRAME_FORMAT));
    //Build the format for APO Output Connection, FLOAT32 format
    BUILD_FORMAT(&aConnectionDescriptors[1].FrameFormat,
                 KSDATAFORMAT_SUBTYPE_IEEE_FLOAT,
                 lpWfx->nChannels,
                 sizeof(FLOAT32),
                 sizeof(FLOAT32) * 8,
                 lpWfx->nSamplesPerSec);
    //Build the format for APO Output Connection, Device's preferred 16 bit format    
    WEX2AF(GET_CAPTURE_WFX(), &af);
    CopyMemory(&aConnectionDescriptors[1].FrameFormat, &af, sizeof(AUDIO_FRAME_FORMAT));

    //Calculate the MaxNumber of frames for 10ms period at the Device's preferred sample rate
    u32MaxFramesPerCall = (lpWfx->nSamplesPerSec / 1000) * 10;
    aConnectionDescriptors[0].ConnectionDescriptor.u32MaxFrameCount =  u32MaxFramesPerCall;
    aConnectionDescriptors[1].ConnectionDescriptor.u32MaxFrameCount =  u32MaxFramesPerCall;
    pGraph = new CGraphRepresentation(aConnectionDescriptors, 2, //2 connection descriptors
                                        aBindingDescriptors, 2, //2 binding descriptors
                                        aApoDescriptors, 1, //1 Apo descriptor
                                        u32MaxFramesPerCall, //Input buffer in frames
                                        u32MaxFramesPerCall, //Output buffer in frames
                                        OutputEndpoint, //Output endpoint
                                        1000, //Optional sine freq, will not be used
                                        InputEndpoint //Input endpoint
                                        );
    _ASSERT(pGraph != NULL);
    hr = pGraph->BuildGraph(); //This call connects the Endpoints, Connections and the respective APO's
    METHOD_CHECKFAIL_EXIT(hr, "ERROR:Build Graph on GraphRepresentation FAILED");

    *ppGraph = pGraph;

exit:
    if(FAILED(hr))
    {
        SAFE_DELETE_POINTER(pGraph);
    }
    return hr;
}

//-------------------------------------------------------------------------
// Class: CKSEndpointBaseTest
// Method: EndpointGetGlitchCount
//
// Description:
//  TestCase for IAudioEndpointGlitch::EndpointGetGlitchCount API 
//
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CKSEndpointBaseTest::EndpointGetGlitchCount()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_FAIL;
    _ASSERT(m_pDevEndpoint != NULL);
    
    if (NULL == m_pDevEndpoint)
    {
        SLOG(eError, "ERROR: NULL == m_pDevEndpoint");
        return (FNS_FAIL);
    }

    SLOG(eError, "Test NOT IMPLEMENTED");

    return dw;
}

//-------------------------------------------------------------------------
// Class: CKSEndpointBaseTest
// Method: EndpointStart
//
// Description:
//  TestCase for IAudioEndpointControl::Start API 
//
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CKSEndpointBaseTest::EndpointStart()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_FAIL;
    _ASSERT(m_pDevEndpoint != NULL);
    
    if (NULL == m_pDevEndpoint)
    {
        SLOG(eError, "ERROR: NULL == m_pDevEndpoint");
        return (FNS_FAIL);
    }

    SLOG(eError, "Test NOT IMPLEMENTED");

    return dw;
}

//-------------------------------------------------------------------------
// Class: CKSEndpointBaseTest
// Method: EndpointStop
//
// Description:
//  TestCase for IAudioEndpointControl::Stop API 
//
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CKSEndpointBaseTest::EndpointStop()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_FAIL;
    _ASSERT(m_pDevEndpoint != NULL);
    
    if (NULL == m_pDevEndpoint)
    {
        SLOG(eError, "ERROR: NULL == m_pDevEndpoint");
        return (FNS_FAIL);
    }

    SLOG(eError, "Test NOT IMPLEMENTED");

    return dw;
}

//-------------------------------------------------------------------------
// Class: CKSEndpointBaseTest
// Method: EndpointReset
//
// Description:
//  TestCase for IAudioEndpointControl::Reset API 
//
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CKSEndpointBaseTest::EndpointReset()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_FAIL;
    _ASSERT(m_pDevEndpoint != NULL);
    
    if (NULL == m_pDevEndpoint)
    {
        SLOG(eError, "ERROR: NULL == m_pDevEndpoint");
        return (FNS_FAIL);
    }

    SLOG(eError, "Test NOT IMPLEMENTED");

    return dw;
}

//-------------------------------------------------------------------------
// Class: CKSEndpointBaseTest
// Method: SetDeviceBuffer
//
// Description:
//  Helper function to prepare device buffer. Attempt setting the requested time
//  then attempt a time that should work for most devices
//
// Parameters:
//
// Return values (HRESULT):
//     Return S_OK on Success, Failure result on Failure
//
HRESULT CKSEndpointBaseTest::SetDeviceBuffer(MFTIME mfRequestedBufferTime, WAVEFORMATEX * lpWfx)
{
    HRESULT hr = S_OK;
    // First, try the requested buffer time. This will be 10ms in most cases
    hr = m_pDevEndpoint->SetBuffer(mfRequestedBufferTime, 0);

    if (FAILED(hr))
    {
        SLOG(eWarn1, "SetBuffer with %lld returned %#08x", (MFTIME) 10 * MFTIME1MSEC, hr);

        // If that doesn't work, adjust 10ms to nearest multiple of full PCM sample and 256 bytes
        // This covers:
        //    1. Drivers that require 256-byte WaveRT buffer size alignment (like HDAudio)
        //    2. Drivers that have PCM sample size that doesn't align perfectly with 10ms
        MFTIME mfAlignedBufferTime = 10 * MFTIME1MSEC;

        // Determine the smallest common aligned duration for 256 bytes and PCM size
        size_t cbPcmSize = lpWfx->nBlockAlign;
        size_t cbSmallestAligned = 0;

        for (size_t i = 1; i <= cbPcmSize; ++i)
        {
            if ((256 * i) % cbPcmSize == 0)
            {
                cbSmallestAligned = 256 * i;
                break;
            }
        }
        _ASSERT(cbSmallestAligned > 0);
        
        MFTIME mfSmallestAlignedDuration = ((MFTIME)cbSmallestAligned) * 1000 * MFTIME1MSEC / lpWfx->nAvgBytesPerSec;
        MFTIME mfDurationMultiple = (mfAlignedBufferTime - 1) / mfSmallestAlignedDuration + 1;
        MFTIME mfChosenBuffer = mfDurationMultiple * mfSmallestAlignedDuration;

        hr = m_pDevEndpoint->SetBuffer(mfChosenBuffer, 0);
        SLOG(eInfo1, "SetBuffer with %lld returns %#08x", mfChosenBuffer, hr);
    }

    return hr;
}

//-------------------------------------------------------------------------
// Class: CKSEndpointBaseTest
// Method: GetEndpointType
//
// Description:
//  Returns the EndpointType for the PnpID passed in 
//
// Parameters:
//  LPWSTR szPnpID
//      The PnpID of the Endppoint whose type is to be returned
//  PULONG  pulEndpointType
//      The Endpoint Type of the Endpoint passed in
//
// Return values (HRESULT):
//     Return S_OK on Success, approp Error Code on Failure
//
//-------------------------------------------------------------------------
//HRESULT CKSEndpointBaseTest::GetEndpointType(LPWSTR szPnpID, PULONG pulEndpointType)
//{
//    HRESULT hr = S_OK;
//    CComPtr<IMMDeviceEnumerator>    MMDeviceEnum = NULL;
//    CComPtr<IMMDevice>              MMDevice     = NULL;    
//    CComPtr<IPropertyStore>         EndpointProperties = NULL;
//    PROPVARIANT                     var;
//
//    PropVariantInit(&var);
//
//    SLOG(eInfo1, "Create MMDeviceEnumerator");
//    hr = MMDeviceEnum.CoCreateInstance(__uuidof(IMMDeviceEnumerator));
//    METHOD_CHECKFAIL_EXIT(hr, "ERROR: Unable to create IMMDeviceEnumerator instance");
//  	
//    SLOG(eInfo1, "GetDevice from MMDeviceEnumerator");
//    hr = MMDeviceEnum->GetDevice(szPnpID, &MMDevice);
//    METHOD_CHECKFAIL_EXIT(hr, "ERROR: Unable to GetDevice from Enumerator");
//    
//    SLOG(eInfo1, "Open PropertyStore for Endpoint");
//    hr = MMDevice->OpenPropertyStore(STGM_READWRITE, &EndpointProperties);
//    METHOD_CHECKFAIL_EXIT(hr, "ERROR: Unable to Open PropertyStore for MMEndpoint");
//        
//    hr = EndpointProperties->GetValue(PKEY_Endpoint_Type, &var);
//    if(SUCCEEDED(hr) && (VT_UI4 == var.vt))
//    {
//        *pulEndpointType = var.ulVal;
//    }
//    else
//    {
//        *pulEndpointType = 0;
//    }
//
//exit:
//    PropVariantClear(&var);
//    return hr;
//}//CKSEndpointBaseTest::GetEndpointType
