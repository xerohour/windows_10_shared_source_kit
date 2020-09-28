// Copyright(C) Microsoft. All rights reserved.

#include "StdAfx.h"


#ifdef TEST_LEAP_MATRIX
#endif
//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOVolumeCreate
//
// Description:
//  TestCase to test if AudioMatrix class is registered and IAudioMatrix interface can be obtained from it
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOMatrixCreate()
{
    CAPOMatrixTest APOMatrixTest;
    return APOMatrixTest.APOMatrixCreate();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOMatrixSetIOSamplesPerFrame
//
// Description:
//  TestCase to test SetIOSamplesPerFrame call on IAudioMatrix
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOMatrixInitialize()
{
    CAPOMatrixTest APOMatrixTest;
    return APOMatrixTest.APOInitialize();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOMatrixSetMatrixCoeffns
//
// Description:
//  TestCase to test SetMatrixCoeffecients call on IAudioMatrix
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOMatrixSetMatrixCoeffns()
{
    CAPOMatrixTest APOMatrixTest;
    return APOMatrixTest.APOSetMatrixCoeffns();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOMatrixGetMatrixCoeffns
//
// Description:
//  TestCase to test GetMatrixCoeffecients call on IAudioMatrix
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOMatrixGetMatrixCoeffns()
{
    CAPOMatrixTest APOMatrixTest;
    return APOMatrixTest.APOGetMatrixCoeffns();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOMatrixProcess
//
// Description:
//  Functional test case for Matrix APO
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOMatrixProcess()
{
    CAPOMatrixTest APOMatrixTest;
    return APOMatrixTest.APOTestMatrix();
}

#ifdef TEST_LEAP_MATRIX
//-------------------------------------------------------------------------
// Class: 
// Method: Tc_LEAPMatrixProcess
//
// Description:
//  Functional test case for LEAP Matrix APO
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_LEAPMatrixProcess()
{
    CAPOMatrixTest APOMatrixTest;
    return APOMatrixTest.LEAPTestMatrix();
}
#endif //TEST_LEAP_MATRIX

//-------------------------------------------------------------------------
// Class: CAPOMatrixTest
// Method: APOMatrixCreate
//
// Description:
//   Actual testcase to check if AudioMatrix is registered with COM and if IAudioMatrix and  
//   IAudioProcessingObjetPtr can be successfully obtained from it.
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOMatrixTest::APOMatrixCreate()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;    
    IAudioMatrixPtr                 pMatrix = NULL;    
    IAudioProcessingObjectPtr       pAPO = NULL;
    IAudioProcessingObjectRTPtr             pAPORT = NULL;
    IAudioProcessingObjectConfigurationPtr  pAPOConfig  = NULL;
    
    //First create Matrix, then QI APO from Matrix
    //Create Matrix Object
    SLOG(eInfo1, "Creating Audio Matrix object");
    hr = pMatrix.CreateInstance(__uuidof(AudioMatrix));
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: FAILED to create Audio Matrix object", dw, FNS_FAIL);
    
    // Create APO Object from Matrix 
    SLOG(eInfo1, "Querying APO object from Matrix");
    hr = pMatrix.QueryInterface(__uuidof(IAudioProcessingObject), &pAPO);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: FAILED to query for  APO object from Matrix", dw, FNS_FAIL);
    
    // Create APORT Object from Matrix 
    SLOG(eInfo1, "Querying APORT object from Matrix");
    hr = pMatrix.QueryInterface(__uuidof(IAudioProcessingObjectRT), &pAPORT);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: FAILED to query for  APORT object from Matrix", dw, FNS_FAIL);
    
    // Create APOConfig Object from Matrix 
    SLOG(eInfo1, "Querying APO config object from Matrix");
    hr = pMatrix.QueryInterface(__uuidof(IAudioProcessingObject), &pAPO);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: FAILED to query for  APO Config object from Matrix", dw, FNS_FAIL);
    
    return dw;
}

//-------------------------------------------------------------------------
// Class: CAPOMatrixTest
// Method: APOInitialize
//
// Description:
//   Actual testcase to test IAudioMatrix->Initialize
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOMatrixTest::APOInitialize()
{
    HRESULT hr = S_OK;
    DWORD   dw = FNS_PASS;
    UINT    u32InpSamplesPerFrame = 0;
    UINT    u32OutSamplesPerFrame = 0;
    FLOAT32 f32FrameRate = 0.0;
    //IAudioProcessingObjectPtr pAPO = NULL;

    _ASSERT( (m_pMatrix != NULL) && (m_pBaseAPO != NULL) && (m_pAPO != NULL) );
    /*if((m_pMatrix == NULL) || (m_pBaseAPO == NULL))
    {
    SLOG(eInfo1, "Creating Objects");
    hr = CreateObjects();
    if(FAILED(hr))
    {
    SLOG(eError, "ERROR: FAILED Trying to Create APO Objects, 0x%X", hr);
    dw = FNS_FAIL;
    goto exit;
    }
    }*/

    // Method call with NULL/Invalid Params
    SLOG(eInfo1, "Calling Initialize with NULL/Invalid Params");

    SLOG(eInfo1, "Calling with all params NULL");
    hr = m_pAPO->Initialize(0, NULL);   
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Method call with all params NULL SUCCEEDED", dw, FNS_FAIL);

    // Call method with InputSamples = NULL
    SLOG(eInfo1, "Calling with InputSamplesPerFrame = NULL");
    u32OutSamplesPerFrame = 2;
    f32FrameRate = 48000.00;
    apoParams.u32InputSamplesPerFrame	= u32InpSamplesPerFrame;
    apoParams.u32OutputSamplesPerFrame	= u32OutSamplesPerFrame;
    apoParams.f32FrameRate				= f32FrameRate;
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams);

    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Method call with InputSamples/Frame = NULL SUCCEEDED", dw, FNS_FAIL);

    // Call method with OutputSamples = NULL
    SLOG(eInfo1, "Calling with OutputSamplesPerFrame = NULL");
    u32InpSamplesPerFrame = 2;
    u32OutSamplesPerFrame = 0;
    apoParams.u32InputSamplesPerFrame	= u32InpSamplesPerFrame;
    apoParams.u32OutputSamplesPerFrame	= u32OutSamplesPerFrame;
    apoParams.f32FrameRate				= f32FrameRate;
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Method call with OutputSamples/Frame = NULL SUCCEEDED", dw, FNS_FAIL);

    // Call method with FrameRate = NULL
    SLOG(eInfo1, "Calling with FrameRate = NULL");
    u32InpSamplesPerFrame = 2;
    u32OutSamplesPerFrame = 2;
    f32FrameRate = 0.0;
    apoParams.u32InputSamplesPerFrame	= u32InpSamplesPerFrame;
    apoParams.u32OutputSamplesPerFrame	= u32OutSamplesPerFrame;
    apoParams.f32FrameRate				= f32FrameRate;
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Method call with FrameRate = NULL SUCCEEDED", dw, FNS_FAIL);

    //Trying to set input and output samples/frame lesser/greater that specified in the Reg Prop
    APO_REG_PROPERTIES_FULL  regProp;
    APO_REG_PROPERTIES_FULL* pAPORegProps = &regProp;

    APORegistration::FillAllApoProperties( APO_MATRIX, regProp );

    /*    if (!pAPORegProps)
    {
    SLOG(eError, "ERROR: Allocating memory for Registration Properties");
    dw = FNS_FAIL;
    goto exit;
    }
    SLOG(eInfo1, "Querying Matrix for APO object");    
    hr = m_pMatrix.QueryInterface(__uuidof(IAudioProcessingObject), &pAPO);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Querying for APO object FAILED", dw, FNS_FAIL);

    SLOG(eInfo1, "Getting APO Reg Propperties");
    hr = pAPO->GetRegistrationProperties(&pAPORegProps);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: GetRegistratioProperties FAILED", dw, FNS_FAIL);    
    */

    // Method call with input samples less than allowed
    SLOG(eInfo1, "Trying to set Inp samples/frame lesser than specified in RegProps");
    u32InpSamplesPerFrame = pAPORegProps->InFormat.u32MinSamplesPerFrame - 1;
    u32OutSamplesPerFrame = pAPORegProps->OutFormat.u32MinSamplesPerFrame; // valid
    f32FrameRate = pAPORegProps->InFormat.f32MinFramesPerSecond; //valid
    apoParams.u32InputSamplesPerFrame	= u32InpSamplesPerFrame;
    apoParams.u32OutputSamplesPerFrame	= u32OutSamplesPerFrame;
    apoParams.f32FrameRate				= f32FrameRate;
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Trying to set Inp samples/frame lesser than specified in RegProps SUCCEEDED", dw, FNS_FAIL);

    // Method call with inputsamples more than allowed
    SLOG(eInfo1, "Trying to set Inp samples/frame greater than specified in RegProps");
    u32InpSamplesPerFrame = pAPORegProps->InFormat.u32MaxSamplesPerFrame + 1;    
    apoParams.u32InputSamplesPerFrame	= u32InpSamplesPerFrame;
    apoParams.u32OutputSamplesPerFrame	= u32OutSamplesPerFrame;
    apoParams.f32FrameRate				= f32FrameRate;
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Trying to set Inp samples/frame greater than specified in RegProps SUCCEEDED", dw, FNS_FAIL);

    // Method call with output samples less than allowed
    SLOG(eInfo1, "Trying to set Out samples/frame lesser than specified in RegProps");
    u32InpSamplesPerFrame = pAPORegProps->InFormat.u32MinSamplesPerFrame;// valid
    u32OutSamplesPerFrame = pAPORegProps->OutFormat.u32MinSamplesPerFrame - 1; 
    apoParams.u32InputSamplesPerFrame	= u32InpSamplesPerFrame;
    apoParams.u32OutputSamplesPerFrame	= u32OutSamplesPerFrame;
    apoParams.f32FrameRate				= f32FrameRate;
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Trying to set Out samples/frame lesser than specified in RegProps SUCCEEDED", dw, FNS_FAIL);

    // Method call with Output samples more than allowed
    SLOG(eInfo1, "Trying to set Out samples/frame greater than specified in RegProps");    
    u32OutSamplesPerFrame = pAPORegProps->OutFormat.u32MaxSamplesPerFrame + 1;
    apoParams.u32InputSamplesPerFrame	= u32InpSamplesPerFrame;
    apoParams.u32OutputSamplesPerFrame	= u32OutSamplesPerFrame;
    apoParams.f32FrameRate				= f32FrameRate;
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Trying to set Out samples/frame greater than specified in RegProps SUCCEEDED", dw, FNS_FAIL);

    // Method call with FrameRate less than allowed
    SLOG(eInfo1, "Trying to set FrameRate lesser than specified in RegProps");
    u32InpSamplesPerFrame = pAPORegProps->InFormat.u32MinSamplesPerFrame;// valid
    u32OutSamplesPerFrame = pAPORegProps->OutFormat.u32MinSamplesPerFrame;//valid
    f32FrameRate = pAPORegProps->InFormat.f32MinFramesPerSecond - 1.0;
    apoParams.u32InputSamplesPerFrame	= u32InpSamplesPerFrame;
    apoParams.u32OutputSamplesPerFrame	= u32OutSamplesPerFrame;
    apoParams.f32FrameRate				= f32FrameRate;
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Trying to set FrameRate lesser than specified in RegProps SUCCEEDED", dw, FNS_FAIL);

    // Method call with FrameRate more than allowed
    SLOG(eInfo1, "Trying to set FrameRate greater than specified in RegProps");    
    f32FrameRate = pAPORegProps->InFormat.f32MaxFramesPerSecond + 1.0;
    apoParams.u32InputSamplesPerFrame	= u32InpSamplesPerFrame;
    apoParams.u32OutputSamplesPerFrame	= u32OutSamplesPerFrame;
    apoParams.f32FrameRate				= f32FrameRate;
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Trying to set FrameRate greater than specified in RegProps SUCCEEDED", dw, FNS_FAIL);

    // A valid method call
    SLOG(eInfo1, "Trying a valid call (mono to stereo matrix, 48K frame rate)");
    u32InpSamplesPerFrame = 1;
    u32OutSamplesPerFrame = 2;
    f32FrameRate = 48000.00;
    apoParams.u32InputSamplesPerFrame	= u32InpSamplesPerFrame;
    apoParams.u32OutputSamplesPerFrame	= u32OutSamplesPerFrame;
    apoParams.f32FrameRate				= f32FrameRate;
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: Valid call to Initialize FAILED", dw, FNS_FAIL);

    //If the above call succeeded then the APO is initialized.
    //irrespective, destroy the objects and create new ones for the following case.
    m_pBaseAPO = NULL; m_pMatrix = NULL; m_pAPO = NULL;
    hr = CreateObjects();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Matrix Object Creation FAILED", dw, FNS_FAIL);

    // Attempt to SetSamplesPerFrame after APO has been Locked    
    SLOG(eInfo1, "Trying to call Initialize after APO has been locked");
    hr = LockAPO();
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Trying to Lock APO FAILED, 0x%X (%s)", hr, GetHRString(hr));
        dw = FNS_FAIL;
        goto exit;
    }
    else // APO has been locked
    {
        SLOG(eInfo1, "Trying a valid call (mono to stereo matrix, 48K)");
        u32InpSamplesPerFrame = 1;
        u32OutSamplesPerFrame = 2;
        f32FrameRate = 48000.00;
        apoParams.u32InputSamplesPerFrame	= u32InpSamplesPerFrame;
        apoParams.u32OutputSamplesPerFrame	= u32OutSamplesPerFrame;
        apoParams.f32FrameRate				= f32FrameRate;
        hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams);
        METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Call SUCCEEDED even after APO has been locked", dw, FNS_FAIL);

        //Unlock APO before leaving
        hr = m_pBaseAPO->UnlockForProcess();
        METHOD_CHECKFAIL_SETVAL(hr, "ERROR: FAILED trying to Unlock the APO", dw, FNS_FAIL);
    }

exit:    
    return dw;
}

//-------------------------------------------------------------------------
// Class: CAPOMatrixTest
// Method: CreateObjects
//
// Description:
//   Internal method used to initialize internal variables. These methods are used by all other test methods
// Parameters:
//
// Return values (HRESULT):
//     Return S_OK on Success, E_FAIL or some other HRESULT error code on Failure
//
HRESULT CAPOMatrixTest::CreateObjects()
{
    HRESULT hr = S_OK;
    if(m_pMatrix == NULL)
    {
        SLOG(eInfo1, "Creating Matrix Object");
        m_pMatrix.CreateInstance(__uuidof(AudioMatrix));
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Creating Matrix Object, 0x%X", hr);
            goto exit;
        }
    }
    if(m_pBaseAPO == NULL)
    {
        SLOG(eInfo1, "Creating APOConfig Object");
        hr = m_pMatrix.QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), &m_pBaseAPO);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Creating APOConfig Object, 0x%X", hr);
            goto exit;
        }
    }
    if(m_pAPO == NULL)
    {
        SLOG(eInfo1, "Creating APO Object");
        hr = m_pMatrix.QueryInterface(__uuidof(IAudioProcessingObject), &m_pAPO);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Creating APO Object, 0x%X", hr);
        }
    }
exit:
    return hr;
}


//-------------------------------------------------------------------------
// Class: CAPOMatrixTest
// Method: LockAPO
//
// Description:
//   Internal method to Lock the APO for Processing
// Parameters:
//
// Return values (HRESULT):
//     Return S_OK on Success, E_FAIL or some other HRESULT error code on Failure
//
HRESULT CAPOMatrixTest::LockAPO( UINT32 u32InChannels, UINT32 u32OutChannels, FLOAT32 f32FrameRate
                                ,FLOAT32* pfMatrixCoeffns, MFTIME* pMftime
                                ,AUDIO_CURVE_TYPE ParamCurve, MFTIME* pCurveDuration
                                ,PLOCK_PARAMS pLockParams)
{
    HRESULT hr = S_OK;
    _ASSERT( (m_pMatrix != NULL) && (m_pBaseAPO != NULL) && (m_pAPO != NULL) );
   /* if(m_pMatrix == NULL)
    {
        SLOG(eError, "ERROR: FATAL ERROR, Internal variable not initialized");
        hr = E_FAIL;
        goto exit;
    }*/

    // initialize the matrix APO
    SLOG(eInfo1, "Initializing Matrix APO");
    UINT32 u32InpSamplesPerFrame = u32InChannels;
    UINT32 u32OutSamplesPerFrame = u32OutChannels;
	apoParams.u32InputSamplesPerFrame	= u32InpSamplesPerFrame;
	apoParams.u32OutputSamplesPerFrame	= u32OutSamplesPerFrame;
	apoParams.f32FrameRate				= f32FrameRate;
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Valid call to SetIOSamplesPerFrame FAILED, 0x%X", hr);        
        goto exit;
    }
    // Set up the Matrix Coeffns
    if(pfMatrixCoeffns)
    {
        SLOG(eInfo1, "Setting up Matrix Coeffns");
        hr = m_pMatrix->SetMatrixCoefficients(u32InChannels, u32OutChannels
                                             , pfMatrixCoeffns);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Error settin Matrix Coeffns on the APO, 0x%X", hr);        
            goto exit;
        }
    }

    //Locking APO for Processing
    SLOG(eInfo1, "Locking APO for Processing");    
    if(!pLockParams)
    {
        // Set up the APO_CONNECTION_DESC's
        AUDIO_FRAME_FORMAT InAudioFormat, OutAudioFormat;
        APO_CONNECTION_DESCRIPTOR InputConnection, OutputConnection;
        APO_CONNECTION_DESCRIPTOR *pInConn = NULL, *pOutConn = NULL;
        BUILD_FORMAT(&InAudioFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, u32InChannels, 4, 32, 48000);
        BUILD_FORMAT(&OutAudioFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, u32OutChannels, 4, 32, 48000);        
        CAudioMediaType::AF2AMT(InAudioFormat, &InputConnection.pFormat);      
        CAudioMediaType::AF2AMT(OutAudioFormat, &OutputConnection.pFormat);
        InputConnection.Type = OutputConnection.Type = APO_CONNECTION_BUFFER_TYPE_EXTERNAL;
        // All the places that call into this function never do more than 480 frames.
        InputConnection.u32MaxFrameCount = OutputConnection.u32MaxFrameCount = 480;
        //InputConnection.u32ExtraFrameCount = OutputConnection.u32ExtraFrameCount = 0;

        // Since we don't know what the buffer will be in the future, use placeholder 
        // values for pBuffer. AudioEngine only checks for NULL and so will accept these.
        // Values need to be separated enough to ensure no overlap will happen, since
        // this isn't an inplace APO.
        InputConnection.pBuffer = 0x1000; OutputConnection.pBuffer = 0x2000;
        InputConnection.u32Signature = OutputConnection.u32Signature = 'ACDS';
        pInConn = &InputConnection;
        pOutConn = &OutputConnection;
        hr = m_pBaseAPO->LockForProcess(1, &pInConn, 1, &pOutConn);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: FAILED Trying to Lock APO, 0x%X", hr);        
            goto exit;
        }
        InputConnection.pFormat->Release();
        OutputConnection.pFormat->Release();
    }
    else
    {
        hr = m_pBaseAPO->LockForProcess(pLockParams->u32NumInConn, pLockParams->ppInputConnections,
            pLockParams->u32NumOutConn, pLockParams->ppOutputConnections);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: FAILED Trying to Lock APO, 0x%X", hr);
            goto exit;
        }
    }

exit:
    return hr;
}


//-------------------------------------------------------------------------
// Class: CAPOMatrixTest
// Method: Initialize
//
// Description:
//   Internal method to Initialize the APO, called by methods that need to intialize the APO to a known state
//   This method sets up the APO to do a mono to stereo matrix, 48K
// Parameters:
//
// Return values (HRESULT):
//     Return S_OK on Success, E_FAIL or some other HRESULT error code on Failure
//
HRESULT CAPOMatrixTest::Initialize()
{
    HRESULT hr = S_OK;

    _ASSERT( (m_pMatrix != NULL) && (m_pBaseAPO != NULL) );
   /* if(m_pMatrix == NULL)
    {
        SLOG(eError, "ERROR: FATAL ERROR, Internal variable not initialized");
        return E_FAIL;
    }*/

    // Setup APO for 1 channel to 2 channel matrix
    SLOG(eInfo1, "Setting up APO for a mono to stereo matrix @ 48K");
    UINT32 u32InpSamplesPerFrame = 1;
    UINT32 u32OutSamplesPerFrame = 2;
    FLOAT32 f32FrameRate = 48000.00;
	apoParams.u32InputSamplesPerFrame	= u32InpSamplesPerFrame;
	apoParams.u32OutputSamplesPerFrame	= u32OutSamplesPerFrame;
	apoParams.f32FrameRate				= f32FrameRate;
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Valid call to Initialize FAILED, 0x%X", hr);        
    }

    return hr;
}

//-------------------------------------------------------------------------
// Class: CAPOMatrixTest
// Method: APOSetMatrixCoeffns
//
// Description:
//   Actual test for IAudioMatrix->SetMatrixCoefficients
// Parameters:
//
// Return values (HRESULT):
//     Return S_OK on Success, E_FAIL or some other HRESULT error code on Failure
//
DWORD CAPOMatrixTest::APOSetMatrixCoeffns()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;
    UINT32 u32InChannelCount = 0;
    UINT32 u32OutChannelCount = 0;    
    MFTIME CurveDuration = 0;
    FLOAT32* pfCoeffnMtrx = NULL;    

    _ASSERT( (m_pMatrix != NULL) && (m_pBaseAPO != NULL) );
    // Instantiate the required Objects.
    /*  if((m_pMatrix == NULL) || (m_pBaseAPO == NULL))
    {
    SLOG(eInfo1, "Creating Objects");
    hr = CreateObjects();
    if(FAILED(hr))
    {
    SLOG(eError, "ERROR: FAILED Trying to Create APO Objects, 0x%X", hr);
    dw = FNS_FAIL;
    goto exit;
    }
    }*/
    //Testcase
    // Check if API can be called on an uninitialized Matrix APO
    SLOG(eInfo1, "Calling SetCoeffns on an uninitialized Matrix APO");
    pfCoeffnMtrx = new FLOAT32[2];
    if(!pfCoeffnMtrx)
    {
        SLOG(eError, "ERROR: Memory allocation for Coeffecient Matrix FAILED");
        dw = FNS_FAIL;
        goto exit;
    }
    pfCoeffnMtrx[0] = 1.0;
    pfCoeffnMtrx[1] = 0.0;
    hr = m_pMatrix->SetMatrixCoefficients(1, 2, pfCoeffnMtrx);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: SetMatrixCoeffns call SUCCEEDED on an uninitialized Matrix APO, 0x%X", hr);
        dw = FNS_FAIL;
    }
    SAFE_DELETE_ARRAY(pfCoeffnMtrx);
    // Try out NULL/Invalid Params
    // ALL input Params are NULL
    SLOG(eInfo1, "Calling SetCoeffns with ALL NULL params");
    // APO needs to be initialized first in order to call this method
    hr = Initialize();
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Trying to initialize APO FAILED, 0x%X", hr);
        dw = FNS_FAIL;
        goto exit;
    }

    hr = m_pMatrix->SetMatrixCoefficients(u32InChannelCount, u32OutChannelCount, NULL);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: Calling SetCoeffns with all NULL params SUCCEEDED, 0x%X", hr);
        dw = FNS_FAIL;
    }

    // Testcase
    // Input Channel Count is 0
    SLOG(eInfo1, "Calling SetCoeffns with 0 Inp Channel Count");
    u32InChannelCount = 0;
    u32OutChannelCount = 2;
    pfCoeffnMtrx = new FLOAT32[2];
    if(!pfCoeffnMtrx)
    {
        SLOG(eError, "ERROR: Memory allocation for Coeffecient Matrix FAILED");
        goto exit;
    }
    pfCoeffnMtrx[0] = 1.0;
    pfCoeffnMtrx[1] = 0.0;
    hr = m_pMatrix->SetMatrixCoefficients(u32InChannelCount, u32OutChannelCount, pfCoeffnMtrx);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: Calling SetCoeffns with 0 Inp Channels SUCCEEDED, 0x%X", hr);
        dw = FNS_FAIL;
    }
    SAFE_DELETE_ARRAY(pfCoeffnMtrx);

    //Testcase
    //Output Channel Count is 0
    SLOG(eInfo1, "Calling SetCoeffns with 0 Out Channel Count");
    u32InChannelCount = 1;
    u32OutChannelCount = 0;
    pfCoeffnMtrx = new FLOAT32[2];
    if(!pfCoeffnMtrx)
    {
        SLOG(eError, "ERROR: Memory allocation for Coeffecient Matrix FAILED");
        goto exit;
    }
    pfCoeffnMtrx[0] = 1.0;
    pfCoeffnMtrx[1] = 0.0;
    hr = m_pMatrix->SetMatrixCoefficients(u32InChannelCount, u32OutChannelCount, pfCoeffnMtrx);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: Calling SetCoeffns with 0 Inp Channels SUCCEEDED, 0x%X", hr);
        dw = FNS_FAIL;
    }
    SAFE_DELETE_ARRAY(pfCoeffnMtrx);

    //Testcase
    //Valid input and output channel count, but Coeffn matrix is NULL
    SLOG(eInfo1, "Calling SetCoeffns with NULL Coeffn matrix");
    u32InChannelCount = 1;
    u32OutChannelCount = 2;        
    hr = m_pMatrix->SetMatrixCoefficients(u32InChannelCount, u32OutChannelCount, NULL);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: Calling SetCoeffns with NULL Coeffn Matrix SUCCEEDED, 0x%X", hr);
        dw = FNS_FAIL;
    }
    
    //TestCase
    // Try input Channel Count different from what the APO is initialized with
    SLOG(eInfo1, "Calling SetCoeffns with a mismatch in Input Channel Count");
    u32InChannelCount = 2; //Initialized to 1 above
    u32OutChannelCount = 2;
    pfCoeffnMtrx = new FLOAT32[4]; // right size 2*2        
    if(!pfCoeffnMtrx)
    {
        SLOG(eError, "ERROR: Memory allocation for Coeffecient Matrix FAILED");
        goto exit;
    }
    memset((void*)pfCoeffnMtrx, 0, sizeof(FLOAT32) * 4);
    hr = m_pMatrix->SetMatrixCoefficients(u32InChannelCount, u32OutChannelCount, pfCoeffnMtrx);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: Calling SetCoeffns with mismatch in Inp Channel Count SUCCEEDED, 0x%X", hr);
        dw = FNS_FAIL;
    }
    SAFE_DELETE_ARRAY(pfCoeffnMtrx);

    //TestCase
    // Try output Channel Count different from what the APO is initialized with

    SLOG(eInfo1, "Calling SetCoeffns with a mismatch in Output Channel Count");
    u32InChannelCount = 1; 
    u32OutChannelCount = 4; //Initialized to 2 above
    pfCoeffnMtrx = new FLOAT32[4]; // right size 1*4        
    if(!pfCoeffnMtrx)
    {
        SLOG(eError, "ERROR: Memory allocation for Coeffecient Matrix FAILED");
        goto exit;
    }
    memset((void*)pfCoeffnMtrx, 0, sizeof(FLOAT32) * 4);
    hr = m_pMatrix->SetMatrixCoefficients(u32InChannelCount, u32OutChannelCount, pfCoeffnMtrx);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: Calling SetCoeffns with mismatch in Output Channel Count SUCCEEDED, 0x%X", hr);
        dw = FNS_FAIL;
    }
    SAFE_DELETE_ARRAY(pfCoeffnMtrx);

    //Testcase
    // Try with pfCoeffns pointing to less than required memory

    // We allocate 2 pages of memory and mark the other page NOACCESS,
    // The Coeffn matrix needs to point to an array of two FLOAT32, we pass in a pointer
    // that is one FLOAT32 inside the first page, so that trying to access the next value would incur a
    // fault.
    // THE FOLLOWING CASE HAS BEEN COMMENTED OUT, SINCE ENGINE HAS CODE THAT ASSERTS IF MEMORY CANNOT BE READ
    // IN RETAIL BUILDS IT'LL A/V AND IN CHECKED BUILDS IT'LL INT 3
    // THERE ISN'T AN ERROR CODE OUT OF IT, SO COMPILING THE CODE BELOW IS A DEFINITE A/V IN ENGINE
    // IN USER SPACE.

    //SLOG(eInfo1, "Calling SetCoeffns with with pfCoeffns pointing to less than required memory");
    //SLOG(eError, "This Test currently blocked on 790379 for checked build of engine core");        
    // UNCOMMENT THE FOLLOWING ONCE 790379 GETS FIXED.
        //u32InChannelCount = 1; 
        //u32OutChannelCount = 2;
        //UINT32 PAGESIZE = 4096;
        //LPVOID lpMem = VirtualAlloc(NULL, 2*PAGESIZE, MEM_COMMIT, PAGE_READONLY);
        //if(!lpMem)
        //{
        //    SLOG(eError, "ERROR: Could not allocate memory for Coeffn Matrix");
        //    dw = FNS_FAIL;
        //    goto exit;
        //}
        //// Get the pointer just one FLOAT32 inside the first PAGE
        //pfCoeffnMtrx = reinterpret_cast<FLOAT32*>((UINT32)lpMem + PAGESIZE - sizeof(FLOAT32));        
        //DWORD dwOldProtect = 0;
        //if (!VirtualProtect((LPVOID)((UINT32)lpMem + PAGESIZE), PAGESIZE, PAGE_NOACCESS, &dwOldProtect))
        //{
        //    DWORD dwErr = GetLastError();
        //    SLOG(eError, "ERROR: Trying to mark memory NOACCESS FAILED, ErrorCode 0x%X", dwErr);
        //    dw = FNS_FAIL;
        //    goto exit;
        //}
        //hr = m_pMatrix->SetMatrixCoefficients(u32InChannelCount, u32OutChannelCount, pfCoeffnMtrx, &mfTime, ParamCurve, &CurveDuration);
        //if(SUCCEEDED(hr))
        //{
        //    SLOG(eError, "ERROR: Calling SetCoeffns less than expected memory SUCCEEDED, 0x%X", hr);
        //    dw = FNS_FAIL;
        //}
        //SAFE_DELETE_ARRAY(pfCoeffnMtrx);
        //if(!VirtualFree(lpMem, 0, MEM_RELEASE))
        //{
        //    SLOG(eError, "ERROR: Trying to FREE memory");
        //    dw = FNS_FAIL;
        //    goto exit;
        //}

    //Testcase
    // Verify if the API behaves as expected
    // We first initialize the APO for a mono to stereo matrix

    SLOG(eInfo1, "Making a valid call to SetMatrixCoeffns");
    u32InChannelCount = 1; 
    u32OutChannelCount = 2; 
    pfCoeffnMtrx = new FLOAT32[2];
    if(!pfCoeffnMtrx)
    {
        SLOG(eError, "ERROR: Memory allocation for Coeffecient Matrix FAILED");
        dw = FNS_FAIL;
        goto exit;
    }
    // Initialize Coeffns
    pfCoeffnMtrx[0] = 2.0;
    pfCoeffnMtrx[1] = 2.0;        
    hr = m_pMatrix->SetMatrixCoefficients(u32InChannelCount, u32OutChannelCount, pfCoeffnMtrx);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Valid call to SetCoeffns FAILED, 0x%X", hr);
        dw = FNS_FAIL;
    }
    else
    {
        FLOAT32* pfOutCoeffnMtrx = new FLOAT32[2];
        if(!pfOutCoeffnMtrx)
        {
            SLOG(eError, "ERROR: Memory allocation for Coeffecient Matrix FAILED");
            dw = FNS_FAIL;
            goto exit;
        }
        ZeroMemory(pfOutCoeffnMtrx, 2 * sizeof(FLOAT32));
        hr = m_pMatrix->GetMatrixCoefficients(u32InChannelCount, u32OutChannelCount, pfOutCoeffnMtrx);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Valid call to GetCoeffns FAILED, 0x%X", hr);
            dw = FNS_FAIL;
        }
        if( (pfCoeffnMtrx[0] != pfOutCoeffnMtrx[0]) ||
            (pfCoeffnMtrx[1] != pfOutCoeffnMtrx[1]) )
        {
            SLOG(eError, "ERROR: Coeffecient data different from what was Set");
            dw = FNS_FAIL;
        }
        SAFE_DELETE_ARRAY(pfOutCoeffnMtrx);
    }
    SAFE_DELETE_ARRAY(pfCoeffnMtrx);

exit:    
    return dw;
}

//-------------------------------------------------------------------------
// Class: CAPOMatrixTest
// Method: APOGetMatrixCoeffns
//
// Description:
//   Actual test for IAudioMatrix->GetMatrixCoefficients
// Parameters:
//
// Return values (HRESULT):
//     Return S_OK on Success, E_FAIL or some other HRESULT error code on Failure
//
DWORD CAPOMatrixTest::APOGetMatrixCoeffns()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;
    UINT32 u32InChannelCount = 0;
    UINT32 u32OutChannelCount = 0;
    FLOAT32* pfCoeffnMtrx = NULL;

    // Instantiate the required Objects.
    if((m_pMatrix == NULL) || (m_pBaseAPO == NULL))
    {
        SLOG(eInfo1, "Creating Objects");
        hr = CreateObjects();
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: FAILED Trying to Create APO Objects, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
    }
    //Testcase
    // Check if API can be called on an uninitialized Matrix APO
    SLOG(eInfo1, "Calling GetCoeffns on an uninitialized Matrix APO");
    pfCoeffnMtrx = new FLOAT32[2];
    if(!pfCoeffnMtrx)
    {
        SLOG(eError, "ERROR: Memory allocation for Coeffecient Matrix FAILED");
        dw = FNS_FAIL;
        goto exit;
    }    
    hr = m_pMatrix->GetMatrixCoefficients(1, 2, pfCoeffnMtrx);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: GetMatrixCoeffns call SUCCEEDED on an uninitialized Matrix APO, 0x%X", hr);
        dw = FNS_FAIL;
    }
    SAFE_DELETE_ARRAY(pfCoeffnMtrx);
    //Testcase
    // Try out NULL/Invalid Params
    // ALL input Params are NULL
    SLOG(eInfo1, "Calling GetCoeffns with ALL NULL params");
    // APO needs to be initialized first in order to call this method
    hr = Initialize();
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Trying to initialize APO FAILED, 0x%X", hr);
        dw = FNS_FAIL;
    }
    else
    {
        hr = m_pMatrix->GetMatrixCoefficients(u32InChannelCount, u32OutChannelCount, NULL);
        if(SUCCEEDED(hr))
        {
            SLOG(eError, "ERROR: Calling GetCoeffns with all NULL params SUCCEEDED, 0x%X", hr);
            dw = FNS_FAIL;
        }
    }
    // Testcase
    // Input Channel Count is 0
    SLOG(eInfo1, "Calling GetCoeffns with 0 Inp Channel Count");
    u32InChannelCount = 0;
    u32OutChannelCount = 2;
    pfCoeffnMtrx = new FLOAT32[2];
    if(!pfCoeffnMtrx)
    {
        SLOG(eError, "ERROR: Memory allocation for Coeffecient Matrix FAILED");
        dw = FNS_FAIL;
        goto exit;
    }        
    hr = m_pMatrix->GetMatrixCoefficients(u32InChannelCount, u32OutChannelCount, pfCoeffnMtrx);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: Calling GetCoeffns with 0 Inp Channels SUCCEEDED, 0x%X", hr);
        dw = FNS_FAIL;
    }
    SAFE_DELETE_ARRAY(pfCoeffnMtrx);

    //Testcase
    //Output Channel Count is 0
    SLOG(eInfo1, "Calling GetCoeffns with 0 Out Channel Count");
    u32InChannelCount = 1;
    u32OutChannelCount = 0;
    pfCoeffnMtrx = new FLOAT32[2];
    if(!pfCoeffnMtrx)
    {
        SLOG(eError, "ERROR: Memory allocation for Coeffecient Matrix FAILED");
        dw = FNS_FAIL;
        goto exit;
    }
    hr = m_pMatrix->GetMatrixCoefficients(u32InChannelCount, u32OutChannelCount, pfCoeffnMtrx);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: Calling GetCoeffns with 0 Out Channels SUCCEEDED, 0x%X", hr);
        dw = FNS_FAIL;
    }
    SAFE_DELETE_ARRAY(pfCoeffnMtrx);

    //Testcase
    //Valid input and output channel count, but Coeffn matrix is NULL
    SLOG(eInfo1, "Calling GetCoeffns with NULL Coeffn Matrix");
    u32InChannelCount = 1;
    u32OutChannelCount = 2;       
    hr = m_pMatrix->GetMatrixCoefficients(u32InChannelCount, u32OutChannelCount, NULL);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: Calling GetCoeffns with 0 Inp Channels SUCCEEDED, 0x%X", hr);
        dw = FNS_FAIL;
    }

    //TestCase
    // Try input Channel Count different from what the APO is initialized with
    SLOG(eInfo1, "Initializing APO to do a mono to stereo Matrix");

    SLOG(eInfo1, "Calling GetCoeffns with a mismatch in Input Channel Count");
    u32InChannelCount = 2; //Initialized to 1 above
    u32OutChannelCount = 2;
    pfCoeffnMtrx = new FLOAT32[4]; // right size 2*2        
    if(!pfCoeffnMtrx)
    {
        SLOG(eError, "ERROR: Memory allocation for Coeffecient Matrix FAILED");
        dw = FNS_FAIL;
        goto exit;
    }
    memset((void*)pfCoeffnMtrx, 0, sizeof(FLOAT32) * 4);
    hr = m_pMatrix->GetMatrixCoefficients(u32InChannelCount, u32OutChannelCount, pfCoeffnMtrx);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: Calling GetCoeffns with mismatch in Inp Channel Count SUCCEEDED, 0x%X", hr);
        dw = FNS_FAIL;
    }
    SAFE_DELETE_ARRAY(pfCoeffnMtrx);

    //TestCase
    // Try output Channel Count different from what the APO is initialized with
    SLOG(eInfo1, "Initializing APO to do a mono to stereo Matrix");

    SLOG(eInfo1, "Calling GetCoeffns with a mismatch in Output Channel Count");
    u32InChannelCount = 1; 
    u32OutChannelCount = 4; //Initialized to 2 above
    pfCoeffnMtrx = new FLOAT32[4]; // right size 1*4        
    if(!pfCoeffnMtrx)
    {
        SLOG(eError, "ERROR: Memory allocation for Coeffecient Matrix FAILED");
        dw = FNS_FAIL;
        goto exit;
    }
    memset((void*)pfCoeffnMtrx, 0, sizeof(FLOAT32) * 4);
    hr = m_pMatrix->GetMatrixCoefficients(u32InChannelCount, u32OutChannelCount, pfCoeffnMtrx);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: Calling GetCoeffns with mismatch in Output Channel Count SUCCEEDED, 0x%X", hr);
        dw = FNS_FAIL;
    }
    SAFE_DELETE_ARRAY(pfCoeffnMtrx);

    //Testcase
    // Try with pfCoeffns pointing to less than required memory
    // We allocate 2 pages of memory and mark the other page NOACCESS,
    // The Coeffn matrix needs to point to an array of two FLOAT32, we pass in a pointer
    // that is one FLOAT32 inside the first page, so that trying to access the next value would incur a
    // fault.
    SLOG(eInfo1, "Calling SetCoeffns with with pfCoeffns pointing to less than required memory");
    SLOG(eError, "This Test currently blocked on 790379 for checked build of engine core");
    // UNCOMMENT THE FOLLOWING ONCE 790379 GETS FIXED.
    //u32InChannelCount = 1; 
    //u32OutChannelCount = 2;
    //UINT32 PAGESIZE = 4096; //each page is 4K
    //LPVOID lpMem = VirtualAlloc(NULL, 2*PAGESIZE, MEM_COMMIT, PAGE_READONLY);
    //if(!lpMem)
    //{
    //    SLOG(eError, "ERROR: Could not allocate memory for Coeffn Matrix");
    //    dw = FNS_FAIL;
    //    goto exit;
    //}
    //// Get the pointer just one FLOAT32 inside the first PAGE
    //pfCoeffnMtrx = reinterpret_cast<FLOAT32*>((UINT32)lpMem + PAGESIZE - sizeof(FLOAT32));        
    //DWORD dwOldProtect = 0;
    //if (!VirtualProtect((LPVOID)((UINT32)lpMem + PAGESIZE), PAGESIZE, PAGE_READONLY, &dwOldProtect))
    //{
    //    SLOG(eError, "ERROR: Trying to mark memory NOACCESS FAILED");
    //    dw = FNS_FAIL;
    //    goto exit;
    //}
    //hr = m_pMatrix->GetMatrixCoefficients(u32InChannelCount, u32OutChannelCount, pfCoeffnMtrx);
    //if(SUCCEEDED(hr))
    //{
    //    SLOG(eError, "ERROR: Calling GetCoeffns less than expected memory SUCCEEDED, 0x%X", hr);
    //    dw = FNS_FAIL;
    //}
    //SAFE_DELETE_ARRAY(pfCoeffnMtrx);
    //if(!VirtualFree(lpMem, 0, MEM_RELEASE))
    //{
    //    SLOG(eError, "ERROR: Trying to FREE memory");
    //    dw = FNS_FAIL;
    //    goto exit;
    //}   

exit: 
    return dw;
}

//-------------------------------------------------------------------------
// Class: CAPOMatrixTest
// Method: APOTestMatrix
//
// Description:
//   Actual functional test for Matrix APO to see if it behaves as expected.
// Parameters:
//
// Return values (HRESULT):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOMatrixTest::APOTestMatrix()
{
    DWORD dw = FNS_PASS;
    HRESULT hr = S_OK;    
    WAVEFORMATEX wfx;
    CComPtr<IBasicLog> pBasicLog;
    
    wfx.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = 48000;
    wfx.wBitsPerSample = sizeof(FLOAT32) * 8;
    wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
    wfx.cbSize = 0;

    // Check if the necessary internal variables have been instantiated
    _ASSERT((m_pMatrix != NULL) && (m_pBaseAPO != NULL));
    
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

    // Allocate memory for input and output Connections, to be used with APOProcess call
    APO_CONNECTION_PROPERTY* pInputConn = NULL;
    APO_CONNECTION_PROPERTY* pOutputConn = NULL;
    pInputConn = new APO_CONNECTION_PROPERTY;
    pOutputConn = new APO_CONNECTION_PROPERTY;
    if (!pInputConn || !pOutputConn)
    {
        SLOG(eError, "ERROR: Allocating memory for APO Connections");
        dw = FNS_FAIL;
        goto exit;
    }
    // Initialize Input Connection
    // Number of input frames is 480 (10msecs of data at 48K)
    BUILD_CONNECTION(pInputConn, pfSineWave, 480, 0, BUFFER_VALID);

    // First off we try the default case, with Stereo in and Stereo out.
    // In this case, left in goes to left out and right in goes to right out (unity matrix)
    //Allocate Output buffer for 10msec of 48k stereo  FLOAT32 data
    FLOAT32* pOutputBuffer = (FLOAT32*)LocalAlloc(LMEM_FIXED, 480 * 2 * sizeof(FLOAT32));
    if(!pOutputBuffer)
    {
        SLOG(eError, "ERROR: Allocating memory for Output Buffer");
        dw = FNS_FAIL;
        goto exit;
    }
    // Initialize Output Connection   
    BUILD_CONNECTION(pOutputConn, pOutputBuffer, 480, 0, BUFFER_SILENT);

    SLOG(eInfo1, "Functional Test for Stereo in, Stereo Out with default Coeffns ");
    //Check if the default operation is as expected
    hr = TestFunctional(2, 2, /* Stereo input and output*/
                        NULL, /*default coeffns*/
                        1, /*Num In Connections*/
                        pInputConn,
                        1, /*Num Out Connections*/
                        pOutputConn);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Functional Test failed for Stereo in, Stereo Out with default Coeffns ");
        dw = FNS_FAIL;        
    }
    else
    {
        FLOAT32* pfOutBuffer = reinterpret_cast<FLOAT32*>(pOutputConn->pBuffer);
        UINT cwf = _clearfp();
        _controlfp(_MCW_RC, _RC_NEAR); //Set the Rounding control to NEAR
        // Check that the input channels are approp replicated on the output channels
        for(UINT32 u32ValidFrames = 0; u32ValidFrames < 480; u32ValidFrames++)
        {
            if( ( pfOutBuffer[2*u32ValidFrames]     != pfSineWave[2*u32ValidFrames]) ||//Both channels are 
                ( pfOutBuffer[2*u32ValidFrames + 1] != pfSineWave[2*u32ValidFrames + 1]) )// replicated on the output
            {
                SLOG(eError, "ERROR: APOProcess FAILED to behave as expected ");
                dw = FNS_FAIL;
                break;
            }
        }
        _controlfp(_MCW_RC, (cwf & _MCW_RC)); //Set it back to whatever it was
    }
    
    // Matrix from 2 to 6 channels with the following coeffns
    //  |     |____In___|
    //  |_Out_| 1  | 2  |
    //  | 1   |1.0 |0.0 |
    //  | 2   |0.0 |1.0 |
    //  | 3   |-1.0|1.0 |
    //  | 4   |0.0 |-1.0|
    //  | 5   |0.8 |0.0 |
    //  | 6   |0.0 |0.8 |
    // Each input channel goes to a specific output channel weighted approp.
    // Free the existing output buffer
    SAFE_FREE_POINTER(pOutputBuffer);    
    //Allocate memory for 6 channel 10msec blah blah output,
    pOutputBuffer = (FLOAT32*)LocalAlloc(LMEM_FIXED, 480 * 6 * sizeof(FLOAT32));
    if(!pOutputBuffer)
    {
        SLOG(eError, "ERROR: Allocating memory for Output Buffer");
        dw = FNS_FAIL;
        goto exit;
    }
    // Initialize Output Connection   
    BUILD_CONNECTION(pOutputConn, pOutputBuffer, 480, 0, BUFFER_SILENT);
    {
        //TestFunctional implicitly Initializes the APO, so we need to destroy the objects and recreate them
        //since the APO can be initialized only once
        m_pBaseAPO = NULL; m_pMatrix = NULL; m_pAPO = NULL;
        hr = CreateObjects();
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:Matrix APO Object Creation FAILED", dw, FNS_FAIL);
        
        //Initialize the Matrix Coeffns
        FLOAT32 fArrMatrixCoeffns[6][2];        
        memset((LPVOID)fArrMatrixCoeffns, 0, sizeof(FLOAT32)*12);
        // initialize the matrix coeffns of interest, all others are 0.0
        //   Channel Out---|  |--- Channel In
        fArrMatrixCoeffns[0][0] = 1.0;
        fArrMatrixCoeffns[1][1] = 1.0;
        fArrMatrixCoeffns[2][0] = (FLOAT32)-1.0;
        fArrMatrixCoeffns[3][1] = (FLOAT32)-1.0;
        fArrMatrixCoeffns[4][0] = (FLOAT32)0.8;
        fArrMatrixCoeffns[5][1] = (FLOAT32)0.8;

        SLOG(eInfo1, "Functional Test for Stereo in, 6 channel Out ");
        //Check if the default operation is as expected
        hr = TestFunctional(2, /* Stereo input */
            6, /* 6 channel output*/
            (FLOAT32*)fArrMatrixCoeffns, /*Matrix coeffns*/
            1, /*Num In Connections*/
            pInputConn,
            1, /*Num Out Connections*/
            pOutputConn);
    }
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Functional Test failed for Stereo in, 6 channel Out ");
        dw = FNS_FAIL;        
    }
    else
    {
        FLOAT32* pfOutBuffer = reinterpret_cast<FLOAT32*>(pOutputConn->pBuffer);        
        // Check that the input channels are approp weighted on each of the 6 output channels
        UINT cwf = _clearfp();
        _controlfp(_MCW_RC, _RC_NEAR); //Set the Rounding control to NEAR        
        for(UINT32 u32ValidFrames = 0; u32ValidFrames < 480; u32ValidFrames++)                   
        {
            // The funtion below checks if the the two values below are within +/- 1%
            // This is to avoid the differences between FLOAT32 and the internal 80 bit format of the FPU
            // and 1% seems like a safe enough margin
            if(
                !CCheck::InRange(pfOutBuffer[6*u32ValidFrames + 0], 
                                 pfSineWave[2*u32ValidFrames], 
                                 (FLOAT32)0.99, (FLOAT32)1.01) ||
                !CCheck::InRange(pfOutBuffer[6*u32ValidFrames + 1],
                                 pfSineWave[2*u32ValidFrames + 1], 
                                 (FLOAT32)0.99, (FLOAT32)1.01) ||
                !CCheck::InRange(pfOutBuffer[6*u32ValidFrames + 2],
                                 -1.0 * pfSineWave[2*u32ValidFrames],
                                 (FLOAT32)0.99, (FLOAT32)1.01) ||
                !CCheck::InRange(pfOutBuffer[6*u32ValidFrames + 3],
                                 -1.0 * pfSineWave[2*u32ValidFrames + 1],
                                 (FLOAT32)0.99, (FLOAT32)1.01) ||
                !CCheck::InRange(pfOutBuffer[6*u32ValidFrames + 4],
                                 (FLOAT32)0.8 * pfSineWave[2*u32ValidFrames],
                                 (FLOAT32)0.99, (FLOAT32)1.01) ||
                !CCheck::InRange(pfOutBuffer[6*u32ValidFrames + 5],
                                 (FLOAT32)0.8 * pfSineWave[2*u32ValidFrames + 1],
                                 (FLOAT32)0.99, (FLOAT32)1.01)
              )
            {
                SLOG(eError, "ERROR: APOProcess FAILED to behave as expected ");
                dw = FNS_FAIL;
                break;
            }
        }
        _controlfp(_MCW_RC, (cwf & _MCW_RC)); //Set it back to whatever it was
    }

    // Matrix from 2 to 1 channel with 1.0 and -1.0 to get complete silence    
    // Free the existing output buffer
    SAFE_FREE_POINTER(pOutputBuffer);    
    //Allocate memory for 1 channel 10msec blah blah output,
    pOutputBuffer = (FLOAT32*)LocalAlloc(LMEM_FIXED, 480 * 1 * sizeof(FLOAT32));
    if(!pOutputBuffer)
    {
        SLOG(eError, "ERROR: Allocating memory for Output Buffer");
        dw = FNS_FAIL;
        goto exit;
    }
    
    // Initialize Output Connection   
    BUILD_CONNECTION(pOutputConn, pOutputBuffer, 480, 0, BUFFER_SILENT);
    //Initialize the Matrix Coeffns
    {
        FLOAT32 fArrMatrixCoeffns[1][2];
        //           Out---|  |---In    
        fArrMatrixCoeffns[0][0] = 1.0;
        fArrMatrixCoeffns[0][1] = -1.0;
        SLOG(eInfo1, "Functional Test for Stereo in, Mono Out with 1 channel inverted ");
        //TestFunctional implicitly Initializes the APO, so we need to destroy the objects and recreate them
        //since the APO can be initialized only once
        m_pBaseAPO = NULL; m_pMatrix = NULL; m_pAPO = NULL;
        hr = CreateObjects();
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:Matrix APO Object Creation FAILED", dw, FNS_FAIL);
        //Check if the default operation is as expected
        hr = TestFunctional(2, /* Stereo input */
            1, /* 1 channel output*/
            (FLOAT32*)fArrMatrixCoeffns, /*Matrix coeffns*/
            1, /*Num In Connections*/
            pInputConn,
            1, /*Num Out Connections*/
            pOutputConn);
    }
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Functional Test failed for Stereo in, Mono Out with 1 channel inverted ");
        dw = FNS_FAIL;        
    }
    else
    {
        FLOAT32* pfOutBuffer = reinterpret_cast<FLOAT32*>(pOutputConn->pBuffer);
        UINT cwf = _clearfp();
        _controlfp(_MCW_RC, _RC_NEAR); //Set the Rounding control to NEAR
        // Check that the input channels are approp weighted
        for(UINT32 u32ValidFrames = 0; u32ValidFrames < 480; u32ValidFrames++)
        {
            if( ( pfOutBuffer[u32ValidFrames] != 0.0) )
            {
                SLOG(eError, "ERROR: APOProcess FAILED to behave as expected ");
                dw = FNS_FAIL;
                break;
            }
        }
        _controlfp(_MCW_RC, (cwf & _MCW_RC)); //Set it back to whatever it was
    }


exit:
    SAFE_FREE_POINTER(pfSineWave);
    SAFE_FREE_POINTER(pOutputBuffer);
    SAFE_DELETE_POINTER(pInputConn);
    SAFE_DELETE_POINTER(pOutputConn);    
    return dw;
}

//-------------------------------------------------------------------------
// Class: CAPOVolumeTest
// Method: TestFunctional
//
// Description:
//   Internal method that Locks the APO for Processing, 
//   Calls APO Process and then UnLocks it.
// Parameters:
//      UINT32 u32InChannels
//          The number of input channels or SamplesPerFrame
//      UINT32 u32OutChannels
//          The number of output channels or SamplesPerFrame
//      FLOAT32* pfMatrixCoeffns
//          Matrix coeffns for the number of channels specified above
//      UINT32 u32NumInConn
//          The number of Input Connections
//      APO_CONNECTION_PROPERTY* pInConn
//         The Input Connections passed to APOProcess
//      UINT32 u32NumOutConn
//          The number of Output Connections
//      APO_CONNECTION_PROPERTY* pOutConn
//         The output Connections passed to APOProcess
//
// Return values (HRESULT):
//     Return S_OK on Success, appropriate error code on Failure
//
HRESULT CAPOMatrixTest::TestFunctional( UINT32 u32InChannels
                            ,UINT32 u32OutChannels                            
                            ,FLOAT32* pfMatrixCoeffns
                            ,UINT32 u32NumInConn
                            ,APO_CONNECTION_PROPERTY* pInputConn
                            ,UINT32 u32NumOutConn
                            ,APO_CONNECTION_PROPERTY* pOutputConn
                            )
{
    HRESULT hr = S_OK;

    IAudioProcessingObjectRTPtr pAPORT = NULL;

    _ASSERT((m_pMatrix != NULL) && (m_pBaseAPO != NULL));

    SLOG(eInfo1, "Creating APORT Object");
    hr = m_pMatrix.QueryInterface(__uuidof(IAudioProcessingObjectRT), &pAPORT);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Creating APORT Object, 0x%X", hr);        
        goto exit;
    }    
    //At the moment, the frame rate is fixed at 48K, don't see any need to make it an input
    // argument, since it does not control anything.
    FLOAT32 f32FrameRate = 48000.00; 
    // Lock the APO for Processing with the approp Matrix Coeffns
    hr = LockAPO(u32InChannels, u32OutChannels, f32FrameRate, pfMatrixCoeffns, NULL);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Locking APO for Processing FAILED 0x%X", hr);
        goto exit;
    }    
    //Call APOProcess... 
    SLOG(eInfo1, "Calling APO Process");
    pAPORT->APOProcess(u32NumInConn, &pInputConn, u32NumOutConn, &pOutputConn);
    //Unlock APO
    SLOG(eInfo1, "UnLocking APO....");
    hr = m_pBaseAPO->UnlockForProcess();
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: UnLocking APO FAILED 0x%X", hr);
        goto exit;
    }

exit:
    return hr;
}

#ifdef TEST_LEAP_MATRIX
//-------------------------------------------------------------------------
// Class: CAPOMatrixTest
// Method: LEAPTestMatrix
//
// Description:
//   Test the optimized LEAP Matrix funtions
// Parameters:
//
// Return values (HRESULT):
//     Return S_OK on Success, E_FAIL or some other HRESULT error code on Failure
//
DWORD CAPOMatrixTest::LEAPTestMatrix()
{
    DWORD dw = FNS_PASS;
    struct SMatrixParameters MatrixParam;
    
    //Allocate memory for sine wave, 10msec 1KHz FLOAT32 Stereo sinewave 
    FLOAT32* pfSineWave = (FLOAT32*)LocalAlloc(LMEM_FIXED, sizeof(FLOAT32) * 
                                                           2/*Num of channels*/ * 
                                                           480/* 10msec of 48Khz data*/); 
    FLOAT32* pfOutput = (FLOAT32*)LocalAlloc(LMEM_FIXED, sizeof(FLOAT32) * 
                                                           2/*Num of channels*/ * 
                                                           480/* 10msec of 48Khz data*/);
    for(UINT32 i = 0; i < 480 * 2; i++)
    { //Initialize Output Buffer to contain all 1.0, this is to verify if Mix succeeds
        pfOutput[i] = 1.0;
    }
    if(!pfSineWave || !pfOutput)
    {
        SLOG(eError, "ERROR: Unable to allocate memory for sine data");
        dw = FNS_FAIL;
        goto exit;
    }
    //Create a stereo test tone 
    //Generate a 10msec 1Khz FLOAT32 Stereo sine wave, routine is called from tonegen.lib
    SLOG(eInfo1, "Generating Input Sine Wave");
    GeneratePCMTestTone((void*)pfSineWave, /*Buffer*/
                        sizeof(FLOAT32)*2*480, /*Sizeof buffer*/
                        48000, /*Sampling frequency*/
                        2,/*Stereo*/
                        32, /*Bits per sample*/
                        1000, /*Frequency of the sine wave*/
                        1.0,/*Amplification factor is unity*/ 
                        32,/*Container is 32 bits*/    
                        KSDATAFORMAT_SUBTYPE_IEEE_FLOAT);

    //Setup Matrix Coeffns for each of the channels with No Ramping
    //Test Matrix Universal Safe
    {
        FLOAT32 fMatrix[2][2];
        fMatrix[0][0] = 0.75;        
        fMatrix[0][1] = -0.5;
        fMatrix[1][0] = -0.75;
        fMatrix[1][1] =  0.5;
        //Initialize the Matrix Param Structure
        MatrixParam.pfl32MatrixCoefficients = (FLOAT32*)fMatrix; MatrixParam.pfl32RampCoefficients = NULL;
        MatrixParam.pvSrc = pfSineWave; MatrixParam.pfl32Dst = pfOutput;
        MatrixParam.fMixWithDestination = TRUE; MatrixParam.uSrcChannelCount = 2;
        MatrixParam.uDstChannelCount = 2; MatrixParam.uSrcSampleCount = 480 * 2;
        
        //Call the Matrix function and mix to destination
        SLOG(eInfo1, "Calling Matrix function GeneralSafe");
        MatrixFromFl32GeneralSafe(&MatrixParam);
        //Verify operation
        SLOG(eInfo1, "Verifying Matrix GeneralSafe Operation");
        for(UINT32 u32ValidFrames = 0; u32ValidFrames < 480; u32ValidFrames++)
        {
            if( 
                !CCheck::InRange(pfOutput[2*u32ValidFrames + 0], 
                                (0.75 * pfSineWave[2*u32ValidFrames+ 0 ]) + (-0.5 * pfSineWave[2*u32ValidFrames+ 1 ]) + 1.0, 
                                (FLOAT32)0.99, (FLOAT32)1.01) ||
                !CCheck::InRange(pfOutput[2*u32ValidFrames + 1], 
                                (-0.75 * pfSineWave[2*u32ValidFrames+ 0 ]) + (0.5 * pfSineWave[2*u32ValidFrames+ 1 ]) + 1.0, 
                                (FLOAT32)0.99, (FLOAT32)1.01)                                
              )
            {
                SLOG(eError, "ERROR: Matrix function FAILED to behave as expected , %d", u32ValidFrames);
                dw = FNS_FAIL;
                break;
            }
        }        
    }

    //Setup Matrix Coeffns for each of the channels with No Ramping
    //Test Matrix One to One Safe
    {
        FLOAT32 fMatrix[2][2];
        //Setup diagonal elements
        fMatrix[0][0] = 0.75;        
        fMatrix[1][0] = fMatrix[0][1] = 0.0;
        fMatrix[1][1] =  -0.5;
        for(UINT32 i = 0; i < 480 * 2; i++)
        { //Initialize Output Buffer to contain all 1.0, this is to verify if Mix succeeds
            pfOutput[i] = 1.0;
        }
        //Initialize the Matrix Param Structure
        MatrixParam.pfl32MatrixCoefficients = (FLOAT32*)fMatrix; MatrixParam.pfl32RampCoefficients = NULL;
        MatrixParam.pvSrc = pfSineWave; MatrixParam.pfl32Dst = pfOutput;
        MatrixParam.fMixWithDestination = TRUE; MatrixParam.uSrcChannelCount = 2;
        MatrixParam.uDstChannelCount = 2; MatrixParam.uSrcSampleCount = 480 * 2;
        
        //////////////////////////// TRY THE SAFE FUNCTION ////////////////////////////////////
        //Call the Matrix function and mix to destination
        SLOG(eInfo1, "Calling Matrix function Diagonal Safe");
        MatrixFromFl32DiagonalSafe(&MatrixParam);
        //Verify operation
        SLOG(eInfo1, "Verifying Matrix DiagonalSafe Operation");
        for(UINT32 u32ValidFrames = 0; u32ValidFrames < 480; u32ValidFrames++)
        {
            if( 
                !CCheck::InRange(pfOutput[2*u32ValidFrames + 0], 
                                (0.75 * pfSineWave[2*u32ValidFrames+ 0 ]) + 1.0, 
                                (FLOAT32)0.99, (FLOAT32)1.01) ||
                !CCheck::InRange(pfOutput[2*u32ValidFrames + 1], 
                                (-0.5 * pfSineWave[2*u32ValidFrames+ 1 ]) + 1.0, 
                                (FLOAT32)0.99, (FLOAT32)1.01)
              )
            {
                SLOG(eError, "ERROR: Matrix function FAILED to behave as expected , %d", u32ValidFrames);
                dw = FNS_FAIL;
                break;
            }
        }

        for(UINT32 i = 0; i < 480 * 2; i++)
        { //Initialize Output Buffer to contain all 1.0, this is to verify if Mix succeeds
            pfOutput[i] = 1.0;
        }
        //Initialize the Matrix Param Structure
        MatrixParam.pfl32MatrixCoefficients = (FLOAT32*)fMatrix; MatrixParam.pfl32RampCoefficients = NULL;
        MatrixParam.pvSrc = pfSineWave; MatrixParam.pfl32Dst = pfOutput;
        MatrixParam.fMixWithDestination = TRUE; MatrixParam.uSrcChannelCount = 2;
        MatrixParam.uDstChannelCount = 2; MatrixParam.uSrcSampleCount = 480 * 2;
        
        //////////////////////////// TRY THE SSE FUNCTION ////////////////////////////////////
        //Call the Matrix function and mix to destination
        SLOG(eInfo1, "Calling Matrix function DiagonalSse");
        MatrixFromFl32DiagonalSse(&MatrixParam);
        //Verify operation
        SLOG(eInfo1, "Verifying Matrix DiagonalSse Operation");
        for(UINT32 u32ValidFrames = 0; u32ValidFrames < 480; u32ValidFrames++)
        {
            if( 
                !CCheck::InRange(pfOutput[2*u32ValidFrames + 0],  //first o/p gets data from first i/p channel
                                (0.75 * pfSineWave[2*u32ValidFrames+ 0 ]) + 1.0, 
                                (FLOAT32)0.99, (FLOAT32)1.01) ||
                !CCheck::InRange(pfOutput[2*u32ValidFrames + 1], //2nd o/p channed gets data from 2nd i/p channel
                                (-0.5 * pfSineWave[2*u32ValidFrames+ 1 ]) + 1.0, 
                                (FLOAT32)0.99, (FLOAT32)1.01)
              )
            {
                SLOG(eError, "ERROR: Matrix function FAILED to behave as expected , %d", u32ValidFrames);
                dw = FNS_FAIL;
                break;
            }
        }
    }

    FLOAT32* pfMonoSineWave = (FLOAT32*)LocalAlloc(LMEM_FIXED, sizeof(FLOAT32)*480);
    for(UINT32 i = 0; i < 480; i++);
    {//Grab every other sample in the original sinewave and assign it to the mono sinewave
        pfMonoSineWave[i] = pfSineWave[2*i];
    }
    SAFE_FREE_POINTER(pfSineWave);
    //TEST MONO TO STEREO
    //Setup Matrix Coeffns for each of the channels with No Ramping
    //Test Matrix One to One Safe
    {
        FLOAT32 fMatrix[2];
        //Setup diagonal elements
        fMatrix[0] = 0.75;        
        fMatrix[1] = -0.75;        
        for(UINT32 i = 0; i < 480 * 2; i++)
        { //Initialize Output Buffer to contain all 1.0, this is to verify if Mix succeeds
            pfOutput[i] = 1.0;
        }
        //Initialize the Matrix Param Structure
        MatrixParam.pfl32MatrixCoefficients = (FLOAT32*)fMatrix; MatrixParam.pfl32RampCoefficients = NULL;
        MatrixParam.pvSrc = pfMonoSineWave; MatrixParam.pfl32Dst = pfOutput;
        MatrixParam.fMixWithDestination = TRUE; MatrixParam.uSrcChannelCount = 1;
        MatrixParam.uDstChannelCount = 2; MatrixParam.uSrcSampleCount = 480;
        
        //////////////////////////// TRY THE SAFE FUNCTION ////////////////////////////////////
        //Call the Matrix function and mix to destination
        SLOG(eInfo1, "Calling Matrix function MonoToStereoSafe");
        MatrixFromFl32MonoToStereoSafe(&MatrixParam);
        //Verify operation      
        SLOG(eInfo1, "Verifying Matrix MonoToStereoSafe Operation");
        for(UINT32 u32ValidFrames = 0; u32ValidFrames < 480; u32ValidFrames++)
        {
            if( 
                !CCheck::InRange(pfOutput[2*u32ValidFrames + 0], 
                                (0.75 * pfMonoSineWave[u32ValidFrames])+ 1.0, 
                                (FLOAT32)0.99, (FLOAT32)1.01) ||
                !CCheck::InRange(pfOutput[2*u32ValidFrames + 1], 
                                (-0.75 * pfMonoSineWave[u32ValidFrames])+ 1.0, 
                                (FLOAT32)0.99, (FLOAT32)1.01)
              )
            {
                SLOG(eError, "ERROR: Matrix function FAILED to behave as expected , %d", u32ValidFrames);
                dw = FNS_FAIL;
                break;
            }
        }

        fMatrix[0] = 0.75;        
        fMatrix[1] = -0.75;
        for(UINT32 i = 0; i < 480 * 2; i++)
        { //Initialize Output Buffer to contain all 1.0, this is to verify if Mix succeeds
            pfOutput[i] = 1.0;
        }
        //Initialize the Matrix Param Structure
        MatrixParam.pfl32MatrixCoefficients = (FLOAT32*)fMatrix; MatrixParam.pfl32RampCoefficients = NULL;
        MatrixParam.pvSrc = pfMonoSineWave; MatrixParam.pfl32Dst = pfOutput;
        MatrixParam.fMixWithDestination = TRUE; MatrixParam.uSrcChannelCount = 1;
        MatrixParam.uDstChannelCount = 2; MatrixParam.uSrcSampleCount = 480;
        
        //////////////////////////// TRY THE SSE FUNCTION ////////////////////////////////////
        //Call the Matrix function and mix to destination
        SLOG(eInfo1, "Calling Matrix  MonoToStereoSsefunction");
        MatrixFromFl32MonoToStereoSse(&MatrixParam);
        //Verify operation
        SLOG(eInfo1, "Verifying Matrix MonoToStereoSse Operation");
        for(UINT32 u32ValidFrames = 0; u32ValidFrames < 480; u32ValidFrames++)
        {
            if( 
                !CCheck::InRange(pfOutput[2*u32ValidFrames + 0], 
                                (0.75 * pfMonoSineWave[u32ValidFrames])+ 1.0, 
                                (FLOAT32)0.99, (FLOAT32)1.01) ||
                !CCheck::InRange(pfOutput[2*u32ValidFrames + 1], 
                                (-0.75 * pfMonoSineWave[u32ValidFrames])+ 1.0, 
                                (FLOAT32)0.99, (FLOAT32)1.01)
              )
            {
                SLOG(eError, "ERROR: Matrix function FAILED to behave as expected, %d", u32ValidFrames);
                dw = FNS_FAIL;
                break;
            }
        }
    }



exit:
    return dw;


}
#endif //TEST_LEAP_MATRIX
