// Copyright(C) Microsoft. All rights reserved.

#include "stdafx.h"

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOVolumeCreate
//
// Description:
//  TestCase to test if AudioVolume class is registered and IAudioVolume interface can be obtained from it
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOVolumeCreate()
{    
    CAPOVolumeTest APOVolumeTest;
    return APOVolumeTest.APOVolumeCreate();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOVolumeInitialize
//
// Description:
//  TestCase to test Initialize on IAudioVolume
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOVolumeInitialize()
{
    CAPOVolumeTest APOVolumeTest;
    return APOVolumeTest.APOInitialize();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOVolumeSetVolCoeffns
//
// Description:
//  TestCase to test SetVolumeCoeffecients on IAudioVolume
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOVolumeSetVolCoeffns()
{
    CAPOVolumeTest APOVolumeTest;
    return APOVolumeTest.APOSetVolCoeffns();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOVolumeGetVolCoeffns
//
// Description:
//  TestCase to test GetVolumeCoeffecients on IAudioVolume
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOVolumeGetVolCoeffns()
{
    CAPOVolumeTest APOVolumeTest;
    return APOVolumeTest.APOGetVolCoeffns();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOVolumeSetMute
//
// Description:
//  TestCase to test SetMute on IAudioVolume
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOVolumeSetMute()
{
    CAPOVolumeTest APOVolumeTest;
    return APOVolumeTest.APOSetMute();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOVolumeGetMute
//
// Description:
//  TestCase to test GetMute on IAudioVolume
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOVolumeGetMute()
{
    CAPOVolumeTest APOVolumeTest;
    return APOVolumeTest.APOGetMute();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOVolumeProcess
//
// Description:
//  Functional APO Volume test
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//

DWORD Tc_APOVolumeProcess()
{
    CAPOVolumeTest APOVolumeTest;
    return APOVolumeTest.APOTestVolume();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOVolumeParamCurveWindowsFade
//
// Description:
//  Functional APO Volume test for AUDIO_CURVE_TYPE_WINDOWS_FADE
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//

DWORD Tc_APOVolumeParamCurveWindowsFade()
{
    CAPOVolumeTest APOVolumeTest;
    return APOVolumeTest.APOTestParamCurveWindowsFade();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOVolumeNoInitialRamp
//
// Description:
//  Functional APO Volume test to verify initial volume settings are simply
//    applied, not ramped.
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOVolumeNoInitialRamp()
{
    CAPOVolumeTest APOVolumeTest;
    return APOVolumeTest.APOTestNoInitialRamp(AUDIO_CURVE_TYPE_WINDOWS_FADE);
}

//-------------------------------------------------------------------------
// Class: CAPOVolumeTest
// Method: CreateObjects
//
// Description:
//  Internal method used to create an instances of IAudioVolume and IAPO
// Parameters:
//
// Return values (HRESULT):
//     Return S_OK on Success, E_FAIL or some other HRESULT error code on Failure
//
HRESULT CAPOVolumeTest::CreateObjects()
{
    HRESULT hr = S_OK;
    

    if(m_pVolume == NULL)
    {
        SLOG(eInfo1, "Creating Volume Object");
        hr = m_pVolume.CreateInstance(__uuidof(AudioVolume));
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Creating Volume Object, 0x%X", hr);
            goto exit;
        }
    }
    if(m_pBaseAPO == NULL)
    {
        SLOG(eInfo1, "Creating APOConfig Object");
        hr = m_pVolume.QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), &m_pBaseAPO);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Creating APOConfig Object, 0x%X", hr);
            goto exit;
        }
    } 
    if(m_pAPO == NULL)
    {
        SLOG(eInfo1, "Creating APO Object");
        hr = m_pVolume.QueryInterface(__uuidof(IAudioProcessingObject), &m_pAPO);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Creating APO Object, 0x%X", hr);
            goto exit;
        }
    }

    if(m_pMute == NULL)
    {
        SLOG(eInfo1, "Creating APO Object");
        hr = m_pVolume.QueryInterface(__uuidof(IAudioMuteAPO), &m_pMute);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Query interface on APO Object, 0x%X", hr);            
        }
    }
    
exit:
    return hr;
}

//-------------------------------------------------------------------------
// Class: CAPOVolumeTest
// Method: Initialize
//
// Description:
//  Internal method used to intialize the VolumeAPO to 2 samples per frame
// Parameters:
//
// Return values (HRESULT):
//     Return S_OK on Success, E_FAIL or some other HRESULT error code on Failure
//
//HRESULT CAPOVolumeTest::Initialize()
//{
//    HRESULT hr = S_OK;
//    
//    // Initialize the Volume APO for 2 channels
//    if(m_pVolume == NULL)
//    {
//        SLOG(eError, "ERROR:No Volume APO available to initialize, bailing out");
//        return E_FAIL;
//    }
//
//    hr = m_pVolume->Initialize(2); // <--The number of channels is hardcoded here
//    if(FAILED(hr))
//    {
//        SLOG(eError, "ERROR: Trying to initialize APO to 2 frames");
//    }
//
//    return hr;
//}

//-------------------------------------------------------------------------
// Class: CAPOVolumeTest
// Method: LockAPO
//
// Description:
//  Internal method used to Lock the APO for Processing
// Parameters:
//  UINT32 u32SamplesPerFrame
//      The samples per frame that this is to be set up with, default = 1
//  FLOAT32* pfCoeffns
//      Array of Volume Coeffns for the individual channels, default = NULL
//      If it is NULL, the function allocates Coeffns for a single channel and initializes
//      it to 1.0
//  MFTIME* pmfTime
//      The time at which the changes need to be applied, default = NULL;
//  PLOCK_PARAMS pLockParams
//      Pointer to LOCK_PARAMS structure, to be used to Lock the APO for Processing
//
// Return values (HRESULT):
//     Return S_OK on Success, E_FAIL or some other HRESULT error code on Failure
//
HRESULT CAPOVolumeTest::LockAPO(UINT32 u32SamplesPerFrame, 
                                FLOAT32* pfCoeffns, 
                                MFTIME* pmfTime,
                                AUDIO_CURVE_TYPE ParamCurve,
                                MFTIME* pCurveDuration,
                                PLOCK_PARAMS pLockParams)
{
    HRESULT hr = S_OK;
    FLOAT32* pfVolCoeffns = NULL;
    UINT32 u32Channels = 0;    
    FLOAT32 f32FrameRate = 48000.0;

    _ASSERT( (m_pBaseAPO != NULL) && (m_pVolume != NULL) && (m_pAPO != NULL) );
    
    if(!pfCoeffns)
    {
        u32Channels = 1;
        pfVolCoeffns = new FLOAT32;
        *pfVolCoeffns = 1.0;
    }
    else
    {
        u32Channels = u32SamplesPerFrame;
        pfVolCoeffns = pfCoeffns;
    }

    if(!pCurveDuration)
    {
        ParamCurve = AUDIO_CURVE_TYPE_NONE;
    }

    // Set up the samples per frame for mono
    SLOG(eInfo1, "Setting Samples per Frame = %d", u32SamplesPerFrame);    
 	apoParams.u32SamplesPerFrame   = u32Channels;
	apoParams.f32FrameRate		   = f32FrameRate;
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: FAILED Trying to Initialize, 0x%X", hr);        
        goto exit;
    }    
    //Set up the Vol Coeffns as 1.0
    SLOG(eInfo1, "Setting Vol Coeffns");
    
    hr = m_pVolume->SetVolumeCoefficients(u32Channels,  // Channels 
                                          pfVolCoeffns, // the actual coeffns for each channel
                                          pmfTime, //MFTIME*    
                                          ParamCurve, //Curve to apply
                                          pCurveDuration); 
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: FAILED Trying to Set Vol Coeffns, 0x%X", hr);        
        goto exit;
    }
    // Now we can Lock the APO with one input and output connection each
    SLOG(eInfo1, "Locking APO for Processing");
    if(!pLockParams) // Caller did not provide a LOCK_PARAMS structure
    {
        AUDIO_FRAME_FORMAT AudioFormat;
        BUILD_FORMAT(&AudioFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, u32Channels, 4, 32, 48000);
        IAudioMediaType* pAudioMediaType = NULL;
        CAudioMediaType::AF2AMT(AudioFormat, &pAudioMediaType);
        APO_CONNECTION_DESCRIPTOR InputConnDesc, OutputConnDesc;
        APO_CONNECTION_DESCRIPTOR *pInputConnDesc = NULL, *pOutputConnDesc = NULL;
        // Descriptors below have 0x1 as the Buffer, because we don't care about the buffer,
        // and the API checks for NULL
        BUILD_DESCRIPTOR(&InputConnDesc, pAudioMediaType,
                         APO_CONNECTION_BUFFER_TYPE_EXTERNAL,
                         0x1, 480);
        BUILD_DESCRIPTOR(&OutputConnDesc, pAudioMediaType,
                         APO_CONNECTION_BUFFER_TYPE_EXTERNAL,
                         0x1, 480);
        pInputConnDesc = &InputConnDesc;
        pOutputConnDesc = &OutputConnDesc;
        hr = m_pBaseAPO->LockForProcess(1, &pInputConnDesc, 1, &pOutputConnDesc);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: FAILED Trying to Lock APO, 0x%X", hr);        
            goto exit;
        }
        pAudioMediaType->Release();
    }
    else //Use the values from the argument
    {
        hr = m_pBaseAPO->LockForProcess(pLockParams->u32NumInConn,
                                        pLockParams->ppInputConnections,
                                        pLockParams->u32NumOutConn,
                                        pLockParams->ppOutputConnections);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: FAILED Trying to Lock APO, 0x%X", hr);        
            goto exit;
        }

    }

exit:
    if(!pfCoeffns)// Coeffns are allocated, when not provided as argument
    {
        SAFE_DELETE_POINTER(pfVolCoeffns);
    }
    return hr;
}

//-------------------------------------------------------------------------
// Class: CAPOVolumeTest
// Method: APOVolumeCreate
//
// Description:
//   Actual testcase to check if AudioVilume is registered with COM and if IAudioVolume and  
//   IAudioProcessingObjetPtr can be successfully obtained from it.
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOVolumeTest::APOVolumeCreate()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;    
    IAudioVolumePtr                 pVol = NULL;    
    IAudioProcessingObjectPtr       pAPO = NULL;
    IAudioProcessingObjectRTPtr             pAPORT = NULL;
    IAudioProcessingObjectConfigurationPtr  pAPOConfig  = NULL;
    

    // Check if all the required APO interfaces can be queried for
    //Create Volume Object
    SLOG(eInfo1, "Creating Audio Volume object");
    hr = pVol.CreateInstance(__uuidof(AudioVolume));
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: FAILED to create Audio Volume object", dw, FNS_FAIL);
    
    SLOG(eInfo1, "Querying APO object from Volume");
    hr = pVol.QueryInterface(__uuidof(IAudioProcessingObject), &pAPO);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: FAILED to query for  APO object from Volume", dw, FNS_FAIL);
    
    SLOG(eInfo1, "Querying APORT object from Volume");
    hr = pVol.QueryInterface(__uuidof(IAudioProcessingObjectRT), &pAPORT);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: FAILED to query for  APORT object from Volume", dw, FNS_FAIL);
    
    SLOG(eInfo1, "Querying APOConfig object from Volume");
    hr = pVol.QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), &pAPOConfig);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: FAILED to query for  APO Config object from Volume", dw, FNS_FAIL);    
    
    return dw;
}

//-------------------------------------------------------------------------
// Class: CAPOVolumeTest
// Method: APOInitialize
//
// Description:
//   Actual testcase to test IAudioVolume->Initialize API call  
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOVolumeTest::APOInitialize()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;
    UINT32  ui32SamplesPerFrame = 0;
    FLOAT32 f32FrameRate = 0.0;
    APO_REG_PROPERTIES_FULL  regProp;
    APO_REG_PROPERTIES_FULL *pRegProp = &regProp;

    APORegistration::FillAllApoProperties( APO_VOLUME, regProp );

    _ASSERT( (m_pBaseAPO != NULL) && (m_pVolume != NULL) && (m_pAPO != NULL) );    

    /*if( (m_pVolume == NULL) || (m_pBaseAPO == NULL))
    {
    hr = CreateObjects();
    if(FAILED(hr))
    {
    SLOG(eError, "ERROR: FAILED Trying to create APO objects, 0x%X", hr);
    dw = FNS_FAIL;
    goto exit;
    }
    }*/
    // Try NULL/Invalid Params
    SLOG(eInfo1, "Trying Initialize with all NULL params");    
    apoParams.u32SamplesPerFrame   = ui32SamplesPerFrame;
    apoParams.f32FrameRate		   = f32FrameRate;
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Initializing with ALL NULL params SUCCEEDED", dw, FNS_FAIL);

    SLOG(eInfo1, "Trying Initialize channels =  NULL");
    f32FrameRate = 48000.00;
    apoParams.u32SamplesPerFrame   = ui32SamplesPerFrame;
    apoParams.f32FrameRate		   = f32FrameRate;
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Initializing with ALL NULL params SUCCEEDED", dw, FNS_FAIL);

    SLOG(eInfo1, "Trying Initialize with FrameRate = NULL");
    f32FrameRate = 0.0;
    ui32SamplesPerFrame = 1;
    apoParams.u32SamplesPerFrame   = ui32SamplesPerFrame;
    apoParams.f32FrameRate		   = f32FrameRate;
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Initializing with ALL NULL params SUCCEEDED", dw, FNS_FAIL);

    SLOG(eInfo1, "Initializing with channels = MAX_UINT");
    ui32SamplesPerFrame = 0xFFFFFFFF;
    apoParams.u32SamplesPerFrame   = ui32SamplesPerFrame;
    apoParams.f32FrameRate		   = f32FrameRate;
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Initializing with MAX_UINT channels SUCCEEDED", dw, FNS_FAIL);

    //SamplesPerFrame value beyond the allowed value.
    // Query for the APO object
    //SLOG(eInfo1, "Querying for APO from Volume object");
    //IAudioProcessingObjectPtr pAPO = NULL;
    //hr = m_pVolume.QueryInterface(__uuidof(IAudioProcessingObject), &pAPO);
    //METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR:Querying for APO Object FAILED", dw, FNS_FAIL);

    // Get the Registration Properties and set a value beyond the max(InProp.Max, Outprop.Max)
    //    SLOG(eInfo1, "Getting APO Registration Properties");        
    //    hr = pAPO->GetRegistrationProperties(&pRegProp);
    //    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: GetRegistrationProp call FAILED", dw, FNS_FAIL);

    SLOG(eInfo1, "Trying to Initialize with channel value higher than the allowed max");
    apoParams.u32SamplesPerFrame   = (max(pRegProp->InFormat.u32MaxSamplesPerFrame,
        pRegProp->OutFormat.u32MaxSamplesPerFrame)
        ) + 1;
    apoParams.f32FrameRate		   = pRegProp->InFormat.f32MinFramesPerSecond;
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Trying to Initialize with a channel value higher than the allowed max SUCCEEDED", dw, FNS_FAIL);

    SLOG(eInfo1, "Trying to Initialize with channel value lesser than the allowed min");
    apoParams.u32SamplesPerFrame   = (UINT32)((min(pRegProp->InFormat.u32MinSamplesPerFrame, 
        pRegProp->OutFormat.u32MinSamplesPerFrame)
        ) - 1);
    apoParams.f32FrameRate		   = pRegProp->InFormat.f32MinFramesPerSecond;
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Trying to Initialize with a channel value lower than the allowed max SUCCEEDED", dw, FNS_FAIL);

    SLOG(eInfo1, "Trying to Initialize with FrameRate value higher than the allowed max");
    apoParams.u32SamplesPerFrame   = pRegProp->InFormat.u32MinSamplesPerFrame;
    apoParams.f32FrameRate		   = pRegProp->InFormat.f32MaxFramesPerSecond + 1.0;
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Trying to Initialize with a FrameRate value higher than the allowed max SUCCEEDED", dw, FNS_FAIL);

    SLOG(eInfo1, "Trying to Initialize with FrameRate value lower than the allowed max");
    apoParams.u32SamplesPerFrame   =  pRegProp->InFormat.u32MinSamplesPerFrame;
    apoParams.f32FrameRate		   =  pRegProp->InFormat.f32MinFramesPerSecond - 1.0;
    hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: Trying to Initialize with a FrameRate value higher than the allowed max SUCCEEDED", dw, FNS_FAIL);

    SLOG(eInfo1, "Initializing after APO has been locked");
    // In order to Lock the APO, we need to do some required initialization 
    // which is setting up the SamplesPerFrame (num channels) and setting up the Vol Coeffns for
    // each of the samples (channels)
    SLOG(eInfo1, "Initializing APO and Locking it down");
    hr = LockAPO();
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Trying to Lock APO failed, 0x%X", hr);
        dw = FNS_FAIL;
        goto exit;
    }
    else
    {
        // APO has been locked, try to set samples per frame again
        SLOG(eInfo1, "Setting Samples per Frame = 1, mono @ 48K");
        apoParams.u32SamplesPerFrame   = 1;
        apoParams.f32FrameRate		   = 48000;
        hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams);
        if(SUCCEEDED(hr))
        {
            SLOG(eError, "ERROR:Initializing after Lock down SUCCEEDED, 0x%X", hr);
            dw = FNS_FAIL;            
        }
        hr = m_pBaseAPO->UnlockForProcess();
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Unlocking the APO FAILED, 0x%X", hr);
            dw = FNS_FAIL;            
        }
    }

exit:   
    return dw;
}

//-------------------------------------------------------------------------
// Class: CAPOVolumeTest
// Method: APOSetVolCoeffns
//
// Description:
//   Actual testcase to test IAudioVolume->SetVolumeCoeffecients API call  
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOVolumeTest::APOSetVolCoeffns()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;
    UINT32  ui32NumCoeffns = 0;
    FLOAT32  *pfCoeffns = NULL;
    MFTIME  mfTime = 0;
    AUDIO_CURVE_TYPE ParamCurve = AUDIO_CURVE_TYPE_NONE;
    MFTIME  CurveDuration = 0;

    _ASSERT( (m_pBaseAPO != NULL) && (m_pVolume != NULL) && (m_pAPO != NULL) );
    /*SLOG(eInfo1, "Creating APO and Volume Objects");
    if( (m_pVolume == NULL) || (m_pBaseAPO == NULL))
    {
        hr = CreateObjects();
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: FAILED Trying to create APO objects, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
    }*/
    SLOG(eInfo1, "Trying Set Vol Coeffns w/o initializing samples per frame");
    ui32NumCoeffns = 1;
    pfCoeffns = new FLOAT32(1.0);
    hr = m_pVolume->SetVolumeCoefficients(ui32NumCoeffns, pfCoeffns, &mfTime, ParamCurve, &CurveDuration);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: Setting Vol Coeffns SUCCEEDED, w/o initializing first, 0x%X", hr);
        dw = FNS_FAIL;
    }    
    //Set samples per frame
    SLOG(eInfo1, "Trying SetVolCoeffns with INVALID and NULL values");
    // Initializing APO with samples per frame = 1
    SLOG(eInfo1, "Trying to initialize with SamplesPerFrame = 1, (mono @ 48K)");    
 	apoParams.u32SamplesPerFrame   = 1;
	apoParams.f32FrameRate		   = 48000;
	hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Initializing APO with SamplesPerFrame = 1, 0x%X", hr);
        dw = FNS_FAIL;
    }
    // APO initialized with 1 sample per frame.
    SLOG(eInfo1, "Trying SetVolCoeffns with all NULL");    
    mfTime = 0;
    *pfCoeffns = 1.0;
    hr = m_pVolume->SetVolumeCoefficients(NULL, NULL, NULL, ParamCurve, NULL);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: SetVolCoeffns with all NULL SUCCEEDED, 0x%X", hr);
        dw = FNS_FAIL;
    }
    SLOG(eInfo1, "Trying SetVolCoeffns with NULL CountofCoeffns");
    hr = m_pVolume->SetVolumeCoefficients(NULL, pfCoeffns, &mfTime, ParamCurve, &CurveDuration);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: SetVolCoeffns with NULL CountofCoeffns SUCCEEDED, 0x%X", hr);
        dw = FNS_FAIL;
    }
    SLOG(eInfo1, "Trying SetVolCoeffns with NULL pfCoeffns");
    hr = m_pVolume->SetVolumeCoefficients(1, NULL, &mfTime, ParamCurve, &CurveDuration);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: SetVolCoeffns with NULL pfCoeffns SUCCEEDED, 0x%X", hr);
        dw = FNS_FAIL;
    }
    SLOG(eInfo1, "Trying SetVolCoeffns with NULL pTime");
    hr = m_pVolume->SetVolumeCoefficients(1, pfCoeffns, NULL, ParamCurve, &CurveDuration);
    if(FAILED(hr))    
    {
        SLOG(eError, "ERROR: SetVolCoeffns with NULL pTime FAILED, 0x%X", hr);        
        dw = FNS_FAIL;
    }
    SLOG(eInfo1, "Trying SetVolCoeffns with NULL Curve Duration");
    //Specify a valid Parameter curve and make the duration NULL
    ParamCurve = AUDIO_CURVE_TYPE_WINDOWS_FADE;
    hr = m_pVolume->SetVolumeCoefficients(1, pfCoeffns, &mfTime, ParamCurve, NULL);
    if(SUCCEEDED(hr))    
    {
        SLOG(eError, "ERROR: SetVolCoeffns with NULL Curve Duration SUCCEEDED, 0x%X", hr);        
        dw = FNS_FAIL;
    }
    ParamCurve = AUDIO_CURVE_TYPE_NONE;

    SLOG(eInfo1, "Trying SetVolCoeffns with samples per frame different from what the APO was initialized with");
    //SLOG(eInfo1, "Trying to set SamplesPerFrame = 1, mono @ 48K");    
    //hr = m_pVolume->Initialize(1, 48000.0); // Single sample per frame
    //if(FAILED(hr))
    //{
    //    SLOG(eError, "ERROR: Setting SamplePerFrame to 1, 0x%X", hr);
    //    dw = FNS_FAIL;
    //}
    SLOG(eInfo1, "Trying to call SetVolCoeffns with samplesperframe = 2");
    SAFE_DELETE_POINTER(pfCoeffns); //delete pfCoeffns and reallocate fro 2 floats
    pfCoeffns = new FLOAT32[2];
    pfCoeffns[0] = 1.0; pfCoeffns[1] = 1.0;
    hr = m_pVolume->SetVolumeCoefficients(2, pfCoeffns, &mfTime, ParamCurve, &CurveDuration); // two samples per frame
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: SetVolCoeffns with a diff samples per frame SUCCEEDED, 0x%X", hr);
        dw = FNS_FAIL;
    }
    SAFE_DELETE_ARRAY(pfCoeffns); //delete pfCoeffns
    pfCoeffns = new FLOAT32(1.0); //reallocate a single FLAOT32
    SLOG(eInfo1, "Trying to call SetVolCoeffns with fCoeffns = MAX_FLOAT");    
    // float format is 1bitSign 8bitExponent 23bitMantissa
    LPBYTE pby = (LPBYTE)pfCoeffns;
    memset(pby,0xFF,1);memset(pby+1,0xFF,1);memset(pby+2,0x7F,1);memset(pby+3,0x7F,1);
    hr = m_pVolume->SetVolumeCoefficients(1, pfCoeffns, &mfTime, ParamCurve, &CurveDuration); // one sample per frame
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: SetVolCoeffns with fCoeffns = MAX_FLOAT FAILED, 0x%X", hr);
        dw = FNS_FAIL;
    }
    SLOG(eInfo1, "Trying to call SetVolCoeffns with fCoeffns = NaN");    
    // float format is 1bitSign 8bitExponent 23bitMantissa
    memset(pfCoeffns, 0xFF, sizeof(FLOAT32));    
    hr = m_pVolume->SetVolumeCoefficients(1, pfCoeffns, &mfTime, ParamCurve, &CurveDuration); // one sample per frame
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: SetVolCoeffns with fCoeffns = NaN SUCCEEDED, 0x%X", hr);
        dw = FNS_FAIL;
    }
    SLOG(eInfo1, "Trying to call SetVolCoeffns with fCoeffns = -ve value");    
    // float format is 1bitSign 8bitExponent 23bitMantissa
    *pfCoeffns = (FLOAT32)-1.0;
    hr = m_pVolume->SetVolumeCoefficients(1, pfCoeffns, &mfTime, ParamCurve, &CurveDuration); // one sample per frame
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: SetVolCoeffns with -ve fCoeffns FAILED, 0x%X", hr);
        dw = FNS_FAIL;
    }
    //VERIFY SETVOLCOEFFNS BEHAVES AS EXPECTED FOR DIFFERENT VALUES OF
    // PARAM CURVE
    SLOG(eInfo1, "Trying to call SetVolCoeffns with ParamCurve = AUDIO_CURVE_TYPE_NONE");    
    *pfCoeffns = (FLOAT32)1.0;
    ParamCurve = AUDIO_CURVE_TYPE_NONE;
    hr = m_pVolume->SetVolumeCoefficients(1, pfCoeffns, &mfTime, ParamCurve, &CurveDuration); // one sample per frame
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: SetVolCoeffns with AUDIO_CURVE_TYPE_NONE FAILED, 0x%X", hr);
        dw = FNS_FAIL;
    }
    SLOG(eInfo1, "Trying to call SetVolCoeffns with ParamCurve = AUDIO_CURVE_TYPE_WINDOWS_FADE");    
    ParamCurve = AUDIO_CURVE_TYPE_WINDOWS_FADE;
    hr = m_pVolume->SetVolumeCoefficients(1, pfCoeffns, &mfTime, ParamCurve, &CurveDuration); // one sample per frame
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: SetVolCoeffns with AUDIO_CURVE_TYPE_WINDOWS_FADE FAILED, 0x%X", hr);
        dw = FNS_FAIL;
    }
    SLOG(eInfo1, "Trying to call SetVolCoeffns with Invalid ParamCurve");    
    memset(&ParamCurve, 0xFF, sizeof(ParamCurve));
    hr = m_pVolume->SetVolumeCoefficients(1, pfCoeffns, &mfTime, ParamCurve, &CurveDuration); // one sample per frame
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: SetVolCoeffns with Invalid ParamCurve SUCCEEDED, 0x%X", hr);
        dw = FNS_FAIL;
    }
    //Set Param Curve back to a valid value
    ParamCurve = AUDIO_CURVE_TYPE_NONE;
    SLOG(eInfo1, "Calling SetVolCoeffns followed by GetVolCoeffns");        
    *pfCoeffns = (FLOAT32)0.5;
    hr = m_pVolume->SetVolumeCoefficients(1, pfCoeffns, &mfTime, ParamCurve, &CurveDuration); // one sample per frame
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: SetVolCoeffns with fCoeffns = 0.5 FAILED, 0x%X", hr);
        dw = FNS_FAIL;
    }
    *pfCoeffns = (FLOAT32)0.0;
    hr = m_pVolume->GetVolumeCoefficients(1, pfCoeffns); // one sample per frame
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: GetVolCoeffns FAILED, 0x%X", hr);
        dw = FNS_FAIL;
    }
    if(*pfCoeffns != 0.5)
    {
        SLOG(eError, "ERROR: Coeffns do not match after a Set and Get");
        dw = FNS_FAIL;
    }

//exit:
    SAFE_DELETE_POINTER(pfCoeffns);    
    return dw;
}

//-------------------------------------------------------------------------
// Class: CAPOVolumeTest
// Method: APOGetVolCoeffns
//
// Description:
//   Actual testcase to test IAudioVolume->GetVolumeCoeffecients API call  
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOVolumeTest::APOGetVolCoeffns()
{
    DWORD dw = FNS_PASS;
    HRESULT hr = S_OK;
    float *pfCoeffns = NULL;
    UINT32 uCoeffnsCount = 0;
    AUDIO_CURVE_TYPE ParamCurve = AUDIO_CURVE_TYPE_NONE;
    MFTIME CurveDuration = 0;

    _ASSERT( (m_pBaseAPO != NULL) && (m_pVolume != NULL) && (m_pAPO != NULL) );
    /*SLOG(eInfo1, "Creating APO and Volume Objects");
    if( (m_pVolume == NULL) || (m_pBaseAPO == NULL))
    {
        hr = CreateObjects();
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: FAILED Trying to create APO objects, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
    }*/
    SLOG(eInfo1, "Trying to Get Voulme Coeffns from an uninitialized APO");
    uCoeffnsCount = 1;
    pfCoeffns = new float;
    hr = m_pVolume->GetVolumeCoefficients(uCoeffnsCount, pfCoeffns);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: GetVolCoeffns SUCCEEDED on an uninitialized APO, 0x%X", hr);
        dw = FNS_FAIL;
    }
    SAFE_DELETE_POINTER(pfCoeffns);    
 
    //Initialize APO and try to call with invalid/null data
    SLOG(eInfo1, "Initializing APO for 2 channels @ 48K");
 	apoParams.u32SamplesPerFrame   = 2;
	apoParams.f32FrameRate		   = 48000;
	hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: FAILED to initialize APO to 2 channels, 0x%X", hr);
        dw = FNS_FAIL;
        goto exit;
    }
    // Volume APO is now initialized
    SLOG(eInfo1, "Trying GetVolCoeffns with NULL/Invalid data");
    SLOG(eInfo1, "Trying GetVolCoeffns with both params = NULL");
    hr = m_pVolume->GetVolumeCoefficients(NULL, NULL);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: GetVolCoeffns SUCCEEDED with both params = NULL, 0x%X", hr);
        dw = FNS_FAIL;
    }
    SLOG(eInfo1, "Trying GetVolCoeffns with NULL count");
    pfCoeffns = new float[2];
    hr = m_pVolume->GetVolumeCoefficients(NULL, pfCoeffns);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: GetVolCoeffns SUCCEEDED with both NULL count, 0x%X", hr);
        dw = FNS_FAIL;
    }
    SAFE_DELETE_ARRAY(pfCoeffns);
    SLOG(eInfo1, "Trying GetVolCoeffns with NULL Coeffns ptr");
    hr = m_pVolume->GetVolumeCoefficients(uCoeffnsCount, NULL);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: GetVolCoeffns SUCCEEDED with NULL Coeffns ptr, 0x%X", hr);
        dw = FNS_FAIL;
    }
    SLOG(eInfo1, "Trying GetVolCoeffns with a different Coeffns Count than what the APO was initialized to");
    pfCoeffns = new float;
    uCoeffnsCount = 1;
    hr = m_pVolume->GetVolumeCoefficients(uCoeffnsCount, pfCoeffns);
    if(SUCCEEDED(hr))
    {
        SLOG(eError, "ERROR: GetVolCoeffns with a diff Coeffns count from what APO was initialized to, 0x%X", hr);
        dw = FNS_FAIL;
    }
    SAFE_DELETE_POINTER(pfCoeffns);
    // Trying to a valid Set and a Get
    // APO is initialized with 2 SamplesPerFrame
    pfCoeffns = new float[2]; // volume info for 2 samples
    uCoeffnsCount = 2; // 2 samples per frame
    pfCoeffns[0] = pfCoeffns[1] = 0.5; //set volume to 0.5 for both channels     
    hr = m_pVolume->SetVolumeCoefficients(uCoeffnsCount, pfCoeffns, NULL, ParamCurve, &CurveDuration);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Unable to SetVolCoeffns on the APO, 0x%X", hr);
        dw = FNS_FAIL;
    }
    else // SUCCEEDED
    {
        pfCoeffns[0] = pfCoeffns[1] = 0.0; 
        hr = m_pVolume->GetVolumeCoefficients(uCoeffnsCount, pfCoeffns);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Unable to GetVolCoeffns from the APO, 0x%X", hr);
            dw = FNS_FAIL;
        }
        if((pfCoeffns[0] != 0.5) || (pfCoeffns[1] != 0.5))
        {
            SLOG(eError, "ERROR: Set and GetVolCoeffns are different, 0x%X", hr);
            dw = FNS_FAIL;
        }
    }

exit:
    SAFE_DELETE_ARRAY(pfCoeffns);    
    return dw;
}



//-------------------------------------------------------------------------
// Class: CAPOVolumeTest
// Method: APOSetMute
//
// Description:
//   Actual testcase to test IAudioVolume->SetMute API call  
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOVolumeTest::APOSetMute()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;
    boolean bMute = FALSE;    
    MFTIME  mfTime = 0;
    AUDIO_CURVE_TYPE ParamCurve = AUDIO_CURVE_TYPE_NONE;
    MFTIME  CurveDuration = 0;

    _ASSERT( (m_pBaseAPO != NULL) && (m_pVolume != NULL) && (m_pAPO != NULL) );
    
    SLOG(eInfo1, "Trying SetMute on an uninitialized APO");    
    bMute = TRUE;
    hr = m_pMute->SetMute(bMute, &mfTime, ParamCurve, &CurveDuration);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: SetMute SUCCEEDED on an unintialized APO", dw, FNS_FAIL);

    //Set samples per frame
    SLOG(eInfo1, "Trying SetMute with INVALID and NULL values");
    
    // Initializing APO with samples per frame = 1
    SLOG(eInfo1, "Trying to Initialize APO with SamplesPerFrame = 1, mono@48K");    
 	apoParams.u32SamplesPerFrame   = 1;
	apoParams.f32FrameRate		   = 48000;
	hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Initializing APO with  SamplesPerFrame to 1", dw, FNS_FAIL);
    
    // Trying ALL NULL doesn't make sense, because it just looks like a valid call to 
    // Mute the APO with NULL pTime and No param Curve.

    // APO initialized with 1 sample per frame.
    //SLOG(eInfo1, "Trying SetMute with all NULL");        
    //hr = m_pVolume->SetMute(NULL, NULL, ParamCurve, NULL);
    //METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: SetMute SUCCEEDED with all arguments = NULL", dw, FNS_FAIL);
        
    SLOG(eInfo1, "Trying SetMute with NULL pTime");
    hr = m_pMute->SetMute(bMute, NULL, ParamCurve, &CurveDuration);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: SetMute with NULL pTime FAILED", dw, FNS_FAIL);
    
    SLOG(eInfo1, "Trying SetMute with NULL Curve Duration");
    //Specify a valid Parameter curve and make the duration NULL
    ParamCurve = AUDIO_CURVE_TYPE_WINDOWS_FADE;
    hr = m_pMute->SetMute(bMute, &mfTime, ParamCurve, NULL);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: SetMute SUCCEEDED with NULL CurveDuration", dw, FNS_FAIL);

    ParamCurve = AUDIO_CURVE_TYPE_NONE;
    
    SLOG(eInfo1, "Trying to Mute the APO");
    bMute = TRUE;
    hr = m_pMute->SetMute(bMute, &mfTime, ParamCurve, &CurveDuration);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: Trying to Mute the APO FAILED", dw, FNS_FAIL);
    
    SLOG(eInfo1, "Trying to UnMute the APO");
    bMute = FALSE;
    hr = m_pMute->SetMute(bMute, &mfTime, ParamCurve, &CurveDuration);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: Trying to UnMute the APO FAILED", dw, FNS_FAIL);
    
    
    //VERIFY SETVOLCOEFFNS BEHAVES AS EXPECTED FOR DIFFERENT VALUES OF
    // PARAM CURVE
    SLOG(eInfo1, "Trying to call SetMute with ParamCurve = AUDIO_CURVE_TYPE_NONE");
    ParamCurve = AUDIO_CURVE_TYPE_NONE;
    bMute = TRUE;
    hr = m_pMute->SetMute(bMute, &mfTime, ParamCurve, &CurveDuration); 
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: SetMute with AUDIO_CURVE_TYPE_NONE FAILED", dw, FNS_FAIL);
    
    SLOG(eInfo1, "Trying to call SetVolCoeffns with ParamCurve = AUDIO_CURVE_TYPE_WINDOWS_FADE");    
    ParamCurve = AUDIO_CURVE_TYPE_WINDOWS_FADE;
    hr = m_pMute->SetMute(bMute, &mfTime, ParamCurve, &CurveDuration); 
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: SetMute with AUDIO_CURVE_TYPE_WINDOWS_FADE FAILED", dw, FNS_FAIL);
    
    SLOG(eInfo1, "Trying to call SetVolCoeffns with Invalid ParamCurve");    
    memset(&ParamCurve, 0xFF, sizeof(ParamCurve));
    hr = m_pMute->SetMute(bMute, &mfTime, ParamCurve, &CurveDuration);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: SetMute with Invalid Param Curve SUCCEEDED", dw, FNS_FAIL);
    
    SLOG(eInfo1, "Verifying SetMute operation");
    bMute = FALSE;
    ParamCurve = AUDIO_CURVE_TYPE_NONE;
    
    //Try both values for SetMute
    for (UINT i = 0; i < 2; i++)
    {
        SLOG(eInfo1, "Calling SetMute with bMute = %s", bMute ? "TRUE" : "FALSE");
        hr = m_pMute->SetMute(bMute, &mfTime, ParamCurve, &CurveDuration);
        METHOD_CHECKFAIL_SETVAL(hr, "ERROR: Legit SetMute call FAILED", dw, FNS_FAIL);
        boolean bRet = FALSE;
        SLOG(eInfo1, "Calling GetMute");
        hr = m_pMute->GetMute(&bRet);
        METHOD_CHECKFAIL_SETVAL(hr, "ERROR: Legit GetMute call FAILED", dw, FNS_FAIL);            
        if(bRet != bMute)
        {
            SLOG(eError, "ERROR: Mute Values do not match after a Set and Get");
            SLOG(eError, "Set value: 0x%X, Get value: 0x%X", bMute, bRet);
            dw = FNS_FAIL;            
        }
        bMute = !bMute;        
    }
exit:    
    return dw;
}


//-------------------------------------------------------------------------
// Class: CAPOVolumeTest
// Method: APOGetMute
//
// Description:
//   Actual testcase to test IAudioVolume->GetMute API call  
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOVolumeTest::APOGetMute()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;
    boolean bMute = FALSE;    
    MFTIME  mfTime = 0;
    AUDIO_CURVE_TYPE ParamCurve = AUDIO_CURVE_TYPE_NONE;
    MFTIME  CurveDuration = 0;

    _ASSERT( (m_pBaseAPO != NULL) && (m_pVolume != NULL) && (m_pAPO != NULL) );

    SLOG(eInfo1, "Trying GetMute on an uninitialized APO");    
    hr = m_pMute->GetMute(&bMute);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: GetMute SUCCEEDED on an uninitialized APO", dw, FNS_FAIL);
    
    SLOG(eInfo1, "Trying GetMute with INVALID and NULL values");
    
    // Initializing APO with samples per frame = 1
    SLOG(eInfo1, "Trying to Initialize APO with SamplesPerFrame = 1, mono @48K");    
 	apoParams.u32SamplesPerFrame   = 1;
	apoParams.f32FrameRate		   = 48000;
	hr = m_pAPO->Initialize( sizeof(apoParams), (LPBYTE)&apoParams);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Initializing APO with  SamplesPerFrame to 1", dw, FNS_FAIL);
    
    // APO initialized with 1 sample per frame.
    SLOG(eInfo1, "Trying GetMute with all NULL");
    hr = m_pMute->GetMute(NULL);
    METHOD_CHECKSUCCESS_SETVAL(hr, "ERROR: GetMute SUCCEEDED with all NULL argument", dw, FNS_FAIL);

    SLOG(eInfo1, "Trying a legit GetMute call");
    hr = m_pMute->GetMute(&bMute);
    METHOD_CHECKFAIL_SETVAL(hr, "ERROR: Legit GetMute call FAILED", dw, FNS_FAIL);

exit:
    return dw;
}

//-------------------------------------------------------------------------
// Class: CAPOVolumeTest
// Method: APOTestVolume
//
// Description:
//   Funtional Test to check that the APO behaves as expected  
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOVolumeTest::APOTestVolume()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;
    FLOAT32* pfVolCoeffns = NULL;
    IAudioProcessingObjectRTPtr pAPORT = NULL;
    WAVEFORMATEX wfx;
    CComPtr<IBasicLog> pBasicLog;
    
    wfx.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = 48000;
    wfx.wBitsPerSample = sizeof(FLOAT32) * 8;
    wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
    wfx.cbSize = 0;
    
    _ASSERT( (m_pBaseAPO != NULL) && (m_pVolume != NULL) && (m_pAPO != NULL) );    

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
    // This is the test tone and is used for all Functional tests here
    //Generate a 10msec 1Khz FLOAT32 Stereo sine wave
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
    APO_CONNECTION_PROPERTY* pInputConn = new APO_CONNECTION_PROPERTY;
    APO_CONNECTION_PROPERTY* pOutputConn = new APO_CONNECTION_PROPERTY;    
    if (!pInputConn || !pOutputConn)
    {
        SLOG(eError, "ERROR: Allocating memory for APO Connections");
        dw = FNS_FAIL;
        goto exit;
    }
    // Initialize Input Connection
    // Number of input frames is 480 (10msecs of data at 48K)
    BUILD_CONNECTION(pInputConn, pfSineWave, 480, 0, BUFFER_VALID);

    //Allocate Output buffer for 10msec of 48k stereo  FLOAT32 data
    FLOAT32* pOutputBuffer = (FLOAT32*)LocalAlloc(LMEM_FIXED, 480 * 2 * sizeof(FLOAT32));
    if(!pOutputBuffer)
    {
        SLOG(eError, "ERROR: Allocating memory for Output Buffer");
        dw = FNS_FAIL;
        goto exit;
    }
    // Initialize Output Connection   
    BUILD_CONNECTION(pOutputConn, pOutputBuffer, 480, 0, BUFFER_SILENT );
    
    //Testcase
    // Initialize the APO to do VolumeControl on a Stereo stream
    pfVolCoeffns = new FLOAT32[2]; // Stereo stream
    pfVolCoeffns[0] = pfVolCoeffns[1] = 0.0; // Volume is zero
    SLOG(eInfo1, "Testing APOProcess with Volume Coeffns = 0.0");
    hr = TestFunctional(2 /*Stereo*/
                        ,pfVolCoeffns
                        ,1 /* Single Input Conn*/
                        ,pInputConn
                        ,1 /*Single Out Conn*/
                        ,pOutputConn);
    
    if(SUCCEEDED(hr))
    {
        // Check if the APO did the right thing, the output should be silence
        for(UINT32 u32Sample = 0; u32Sample < 480; u32Sample++)
        {
            if( (((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample]     != 0.0) ||  // first channel
                (((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample + 1] != 0.0) )//second channel
            {
                SLOG(eError, "ERROR: APOProcess did not silence the buffer");
                SLOG(eError, "Expected Value: 0.0, Actual Value: %f, Channel 0, Sample: %d", ((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample], u32Sample);
                SLOG(eError, "Expected Value: 0.0, Actual Value: %f, Channel 1, Sample: %d", ((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample+1], u32Sample);
                dw = FNS_FAIL;
                break;
            }
        }
    }
    else
    {
        SLOG(eError, "ERROR: Functional Test FAILED 0x%X", hr);
    }
    m_pVolume = NULL; m_pBaseAPO = NULL; m_pAPO = NULL; m_pMute = NULL; //TestFunctional implicitly Initializes the APO.
    hr = CreateObjects();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Object creation for Volume APO FAILED", dw, FNS_FAIL);
    
    //Set the Coeffecients to some value i.e. 2.5, but Mute the APO
    // The resultant output should be Muted (silence)
    //Zero out the output Memory
    ZeroMemory(pOutputBuffer, sizeof(FLOAT32) * 2 * 480);
    //Set the Volume Coeffns
    pfVolCoeffns[0] = 2.5; pfVolCoeffns[1] = 2.5;
    SLOG(eInfo1, "Testing APOProcess with APO Muted");
    SLOG(eInfo1, "Creating APORT Object");
    hr = m_pVolume.QueryInterface(__uuidof(IAudioProcessingObjectRT), &pAPORT);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Creating APORT Object, 0x%X", hr);        
        goto exit;
    }

    SLOG(eInfo1, "Locking APO for Processing");
    // Lock the APO for Processing with the approp Volume Coeffns
    hr = LockAPO(2, pfVolCoeffns);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Locking APO for Processing FAILED 0x%X", hr);
        goto exit;
    }    
    boolean bMute = TRUE; //Mute the APO
    MFTIME mfTime = 0, mfCurveDuration = 0;
    hr = m_pMute->SetMute(bMute, &mfTime, AUDIO_CURVE_TYPE_NONE, &mfCurveDuration);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: SetMute call FAILED", dw, FNS_FAIL);
    //Call APOProcess... 
    SLOG(eInfo1, "Calling APO Process");
    pAPORT->APOProcess(1, &pInputConn, 1, &pOutputConn);
    //Unlock APO
    SLOG(eInfo1, "UnLocking APO....");
    hr = m_pBaseAPO->UnlockForProcess();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: UnLocking APO FAILED ", dw, FNS_FAIL);    
    bMute = FALSE; //UnMute
    hr = m_pMute->SetMute(bMute, &mfTime, AUDIO_CURVE_TYPE_NONE, &mfCurveDuration);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: SetMute call FAILED", dw, FNS_FAIL);
    
    if(SUCCEEDED(hr))
    {
        // Check if the APO did the right thing, the output should be silence on one channel
        for(UINT32 u32Sample = 0; u32Sample < 480; u32Sample++)
        {
            if( (((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample]     != 0.0) ||//channel 0 is muted
                (((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample + 1] != 0.0) )//channel 1 is muted
            {
                SLOG(eError, "ERROR: APOProcess FAILED to behave as expected ");
                SLOG(eError, "Expected Value: 0.0, Actual Value: %f, Channel 0, Sample: %d", ((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample], u32Sample);
                SLOG(eError, "Expected Value: 0.0, Actual Value: %f, Channel 1, Sample: %d", ((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample+1], u32Sample);
                dw = FNS_FAIL;
                break;
            }
        }
    }
    else
    {
        SLOG(eError, "ERROR: Functional Test FAILED 0x%X", hr);
    }
    m_pVolume = NULL; m_pBaseAPO = NULL; m_pAPO = NULL; //TestFunctional implicitly Initializes the APO.
    hr = CreateObjects();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Object creation for Volume APO FAILED", dw, FNS_FAIL);
    
    // Selectively mute one of the channels
    //TestCase
    // Mute Channel 0, leave the other at 1.0
    pfVolCoeffns[0] = 0.0; pfVolCoeffns[1] = 1.0;
    SLOG(eInfo1, "Testing APOProcess with Channel 0 Volume Coeffn = 0.0");
    hr = TestFunctional(2 /*Stereo*/
                        ,pfVolCoeffns
                        ,1 /* Single Input Conn*/
                        ,pInputConn
                        ,1 /*Single Out Conn*/
                        ,pOutputConn);
    
    if(SUCCEEDED(hr))
    {
        // Check if the APO did the right thing, the output should be silence on one channel
        for(UINT32 u32Sample = 0; u32Sample < 480; u32Sample++)
        {
            if( (((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample]     != 0.0) ||//channel 0 is muted
                (((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample + 1] != pfSineWave[2*u32Sample+1]) )//channel 1 is untouched
            {
                SLOG(eError, "ERROR: APOProcess FAILED to behave as expected ");
                SLOG(eError, "Expected Value: 0.0, Actual Value: %f, Channel 0, Sample: %d", ((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample], u32Sample);
                SLOG(eError, "Expected Value: %f, Actual Value: %f, Channel 1, Sample: %d", pfSineWave[2*u32Sample+1], ((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample+1], u32Sample);
                dw = FNS_FAIL;
                break;
            }
        }
    }
    else
    {
        SLOG(eError, "ERROR: Functional Test FAILED 0x%X", hr);
    }
    m_pVolume = NULL; m_pBaseAPO = NULL; m_pAPO = NULL;//TestFunctional implicitly Initializes the APO.
    hr = CreateObjects();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Object creation for Volume APO FAILED", dw, FNS_FAIL);
    // Mute Channel 1, leave the other at 1.0
    pfVolCoeffns[0] = 1.0; pfVolCoeffns[1] = 0.0;
    SLOG(eInfo1, "Testing APOProcess with Channel 1 Volume Coeffn = 0.0");
    hr = TestFunctional(2 /*Stereo*/
                        ,pfVolCoeffns
                        ,1 /* Single Input Conn*/
                        ,pInputConn
                        ,1 /*Single Out Conn*/
                        ,pOutputConn);
    
    if(SUCCEEDED(hr))
    {
        // Check if the APO did the right thing, the output should be silence on one channel
        for(UINT32 u32Sample = 0; u32Sample < 480; u32Sample++)
        {
            if( (((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample]     != pfSineWave[2*u32Sample]) ||//channel 0 is intouched
                (((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample + 1] != 0.0) )//channel 1 is mute
            {
                SLOG(eError, "ERROR: APOProcess FAILED to behave as expected ");
                SLOG(eError, "Expected Value: %f, Actual Value: %f, Channel 0, Sample: %d", pfSineWave[2*u32Sample], ((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample], u32Sample);
                SLOG(eError, "Expected Value: 0.0, Actual Value: %f, Channel 1, Sample: %d", ((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample+1], u32Sample);
                dw = FNS_FAIL;
                break;
            }
        }
    }
    else
    {
        SLOG(eError, "ERROR: Functional Test FAILED 0x%X", hr);
    }
    m_pVolume = NULL; m_pBaseAPO = NULL; m_pAPO = NULL;//TestFunctional implicitly Initializes the APO.
    hr = CreateObjects();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Object creation for Volume APO FAILED", dw, FNS_FAIL);
    //Check to see if the APO amplifies by 2.0 correctly
    pfVolCoeffns[0] = 2.0; pfVolCoeffns[1] = 2.0;
    SLOG(eInfo1, "Testing APOProcess with both Channel Volume Coeffns = 2.0");
    hr = TestFunctional(2 /*Stereo*/
                        ,pfVolCoeffns
                        ,1 /* Single Input Conn*/
                        ,pInputConn
                        ,1 /*Single Out Conn*/
                        ,pOutputConn);
    
    if(SUCCEEDED(hr))
    {
        // Check if the APO did the right thing, the output should be 2 times the input
        for(UINT32 u32Sample = 0; u32Sample < 480; u32Sample++)
        {
            if( (((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample]     != 2.0 * pfSineWave[2*u32Sample]) || // Both channels are
                (((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample + 1] != 2.0 * pfSineWave[2*u32Sample]) ) // 2 times the input
            {
                SLOG(eError, "ERROR: APOProcess FAILED to behave as expected ");
                SLOG(eError, "Expected Value: %f, Actual Value: %f, Channel 0, Sample: %d", 2.0 * pfSineWave[2*u32Sample], ((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample], u32Sample);
                SLOG(eError, "Expected Value: %f, Actual Value: %f, Channel 1, Sample: %d", 2.0 * pfSineWave[2*u32Sample + 1], ((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample+1], u32Sample);
                dw = FNS_FAIL;
                break;
            }
        }
    }
    else
    {
        SLOG(eError, "ERROR: Functional Test FAILED 0x%X", hr);
    }
    m_pVolume = NULL; m_pBaseAPO = NULL; m_pAPO = NULL;//TestFunctional implicitly Initializes the APO.
    hr = CreateObjects();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Object creation for Volume APO FAILED", dw, FNS_FAIL);
    //Check to see if the APO attenuates by 2.0 correctly
    pfVolCoeffns[0] = 0.5; pfVolCoeffns[1] = 0.5;
    SLOG(eInfo1, "Testing APOProcess with both Channel Volume Coeffn = 0.5");
    hr = TestFunctional(2 /*Stereo*/
                        ,pfVolCoeffns
                        ,1 /* Single Input Conn*/
                        ,pInputConn
                        ,1 /*Single Out Conn*/
                        ,pOutputConn);
    
    if(SUCCEEDED(hr))
    {
        // Check if the APO did the right thing, the output should be 0.5 times the input
        for(UINT32 u32Sample = 0; u32Sample < 480; u32Sample++)
        {
            if( (((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample]     != 0.5 * pfSineWave[2*u32Sample]) ||// Both channels are
                (((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample + 1] != 0.5 * pfSineWave[2*u32Sample]) )// 0.5 times the input
            {
                SLOG(eError, "ERROR: APOProcess FAILED to behave as expected ");
                SLOG(eError, "Expected Value: %f, Actual Value: %f, Channel 0, Sample: %d", 2.0 * pfSineWave[2*u32Sample], ((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample], u32Sample);
                SLOG(eError, "Expected Value: %f, Actual Value: %f, Channel 1, Sample: %d", 2.0 * pfSineWave[2*u32Sample + 1], ((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample+1], u32Sample);
                dw = FNS_FAIL;
                break;
            }
        }
    }
    else
    {
        SLOG(eError, "ERROR: Functional Test FAILED 0x%X", hr);
    }
    m_pVolume = NULL; m_pBaseAPO = NULL; m_pAPO = NULL;//TestFunctional implicitly Initializes the APO.
    hr = CreateObjects();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Object creation for Volume APO FAILED", dw, FNS_FAIL);
    // Check if the APO can handle 8 channels
    // Need to generate a sine wave for 8 channels
    SAFE_FREE_POINTER(pfSineWave);
    //Allocate memory for sine wave, 10msec 1KHz FLOAT32 8 channel sinewave
    pfSineWave = (FLOAT32*)LocalAlloc(LMEM_FIXED, sizeof(FLOAT32) * 
                                                           8/*Num of channels*/ * 
                                                           480/* 10msec of 48Khz data*/);    
    if(!pfSineWave)
    {
        SLOG(eError, "ERROR: Unable to allocate memory for sine data");
        dw = FNS_FAIL;
        goto exit;
    }

    // This is the test tone and is used for all Functional tests here
    //Generate a 10msec 1Khz FLOAT32 8 channel sine wave
    
    SLOG(eInfo1, "Generating 8 channel sinewave");
    wfx.nChannels = 8; // eight-channel this time
    wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
    
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
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Could not fill buffer", dw, FNS_FAIL);

    //Reinitialize the input and output, buffer/connection 
    // Initialize Input Connection
    // Number of input frames is 480 (10msecs of data at 48K)
    BUILD_CONNECTION(pInputConn, pfSineWave, 480, 0, BUFFER_VALID);
    //Allocate Output buffer for 10msec of 48k 8 channels  FLOAT32 data
    SAFE_FREE_POINTER(pOutputBuffer);
    pOutputBuffer = (FLOAT32*)LocalAlloc(LMEM_FIXED, 480 * 8 * sizeof(FLOAT32));
    if(!pOutputBuffer)
    {
        SLOG(eError, "ERROR: Allocating memory for Output Buffer");
        dw = FNS_FAIL;
        goto exit;
    }
    // Initialize Output Connection   
    BUILD_CONNECTION(pOutputConn, pOutputBuffer, 480, 0, BUFFER_SILENT);
    // Allocate volume coeffns for 8 channels
    SAFE_DELETE_ARRAY(pfVolCoeffns);
    pfVolCoeffns = new FLOAT32[8];
    // Initialize the volume coeffns to 2.0 for all 8 channels
    for(UINT i = 0; i < 8; i++)
    {
        pfVolCoeffns[i] = 2.0;
    }
    SLOG(eInfo1, "Testing APOProcess with 8 Channel data, Vol Coeffns = 2.0");
    // Call the Test function
    hr = TestFunctional(8 /*Num Channels*/
                        ,pfVolCoeffns
                        ,1 /* Single Input Conn*/
                        ,pInputConn
                        ,1 /*Single Out Conn*/
                        ,pOutputConn);
    
    if(SUCCEEDED(hr))
    {
        BOOL fFail = FALSE;
        // Check if the APO did the right thing, the output should be twice the input on each channel
        for(UINT32 u32Sample = 0; u32Sample < 480; u32Sample++)
        {
            for (UINT i = 0; i < 8; i++)
            {//For each of the channel check if the output is 2 times the input
                if(((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample + i]!= 2.0 * pfSineWave[2*u32Sample + i])                    
                {
                    SLOG(eError, "ERROR: APOProcess FAILED to behave as expected ");
                    dw = FNS_FAIL;
                    fFail = TRUE;
                    break;
                }
            }
            if(fFail) //If we failed above,break out of the outer loop too
                break;
        }
    }
    else
    {
        SLOG(eError, "ERROR: Functional Test FAILED 0x%X", hr);
    }
    m_pVolume = NULL; m_pBaseAPO = NULL; m_pAPO = NULL; m_pMute = NULL; //TestFunctional implicitly Initializes the APO.
    hr = CreateObjects();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Object creation for Volume APO FAILED", dw, FNS_FAIL);

exit:
    SAFE_FREE_POINTER(pfSineWave);
    SAFE_FREE_POINTER(pOutputBuffer);
    SAFE_DELETE_POINTER(pInputConn);
    SAFE_DELETE_POINTER(pOutputConn);
    SAFE_DELETE_ARRAY(pfVolCoeffns);
    return dw;
}

//-------------------------------------------------------------------------
// Class: CAPOVolumeTest
// Method: TestFunctional
//
// Description:
//   Internal method that Locks the Volume APO for Processing, Calls APO Process
//   and then UnLocks it
// Parameters:
//      UINT32 u32Channels
//          The number of channels or SamplesPerFrame
//      FLOAT32* pfVolCoeffns
//          Volume coeffns for the number of channels specified above
//      UINT32 u32NumInConn
//          The number of Input Connections
//      APO_CONNECTION_PROPERTY* pInConn
//         The input Connections passed to APOProcess
//      UINT32 u32NumOutConn
//          The number of Output Connections
//      APO_CONNECTION_PROPERTY* pInConn
//         The output Connections passed to APOProcess
//
// Return values (HRESULT):
//     Return S_OK on Success, appropriate error code on Failure
//
HRESULT CAPOVolumeTest::TestFunctional(UINT32 u32Channels, 
                           FLOAT32* pfVolCoeffns,
                           UINT32 u32NumInConn,
                           APO_CONNECTION_PROPERTY* pInputConn,
                           UINT32 u32NumOutConn,
                           APO_CONNECTION_PROPERTY* pOutputConn,
                           AUDIO_CURVE_TYPE ParamCurve,
                           MFTIME* pCurveDuration)
{
    HRESULT     hr = S_OK;
    UINT32      ii;

    IAudioProcessingObjectRTPtr pAPORT = NULL;

    SLOG(eInfo1, "Creating APORT Object");
    hr = m_pVolume.QueryInterface(__uuidof(IAudioProcessingObjectRT), &pAPORT);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Creating APORT Object, 0x%X", hr);        
        goto exit;
    }

    //  Bug#1871057, rigging this guy to work.
    {
        FLOAT32    *pfVolUnity = new FLOAT32[u32Channels];
   
        //Set up the Vol Coeffns as 1.0
        if (NULL == pfVolUnity)
        {
            SLOG(eError, "ERROR: Insufficient memory.");
            hr = E_OUTOFMEMORY;
            goto exit;
        }
        for (ii = u32Channels; ii; --ii)
        {
            pfVolUnity[ii - 1] = 1.0;
        }
        
        SLOG(eInfo1, "Locking APO for Processing");
        // Lock the APO for Processing with the approp Volume Coeffns
        hr = LockAPO(u32Channels, pfVolUnity, NULL, ParamCurve, pCurveDuration);
        delete [] pfVolUnity;
        
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Locking APO for Processing FAILED 0x%X", hr);
            goto exit;
        }    
    }
    
    //Call APOProcess the first time to clear its 'initial' flag.
    pAPORT->APOProcess(u32NumInConn, &pInputConn, u32NumOutConn, &pOutputConn);
    for (ii = u32NumOutConn; ii; --ii)
    {
        pOutputConn[ii - 1].u32BufferFlags = BUFFER_SILENT;
    }
    
    SLOG(eInfo1, "Setting Vol Coeffns");
    
    hr = m_pVolume->SetVolumeCoefficients(u32Channels,  // Channels 
                                          pfVolCoeffns, // the actual coeffns for each channel
                                          NULL, //MFTIME*    
                                          ParamCurve, //Curve to apply
                                          pCurveDuration); 
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: FAILED Trying to Set Vol Coeffns, 0x%X", hr);        
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


//-------------------------------------------------------------------------
// Class: CAPOVolumeTest
// Method: APOTestManual
//
// Description:
//   Funtional Test to check that the APO behaves as expected.
//   This test will create the entire Engine Graph with a Volume APO (and a Format Converter)in it 
//   writing to the preferred Audio Device. Also it'll have a normal thread  running from which 
//   it'll try to change the volume and verify that the values are consistent.
//   
//   The Graph should look like 
//   InputEndpt-->Conn-->VolumeAPO-->Conn-->FormatAPO-->Conn-->OutputEndpoint
//
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOVolumeTest::APOTestManual()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;

    //IAPO for Volume, we'll need this to query for IAudVol to get/set vol coeffns
    IAudioProcessingObjectPtr pAPOVol = NULL; 
    // two APO's, Volume and FormatConverter
    // Format Conv does not need any init, so we don't need an IAPO for it
    GRAPH_APO_DESCRIPTOR GraphAPO[] = { {APO_FMT_f32i16, NULL}, {APO_VOLUME, pAPOVol} }; 
    //3 Connection , 4 Descriptors
    GRAPH_BINDING_DESCRIPTOR GraphBinding[] = { 
                                              { 0, 0, true  }, { 0, 1, false },
                                              { 1, 1, true  }, { 1, 2, false } 
                                              }; 
    //3 Connection Descriptors for the three connections
    GRAPH_CONNECTION_DESCRIPTOR GraphConnection[3];
    ZeroMemory(GraphConnection, sizeof(GRAPH_CONNECTION_DESCRIPTOR) * 3);
    // The first two connections have float format

    return dw;

}

//-------------------------------------------------------------------------
// Class: CAPOVolumeTest
// Method: APOTestParamCurveWindowsFade
//
// Description:
//   Funtional Test to check that WINDOWS_FADE ParamCurve behaves as expected
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOVolumeTest::APOTestParamCurveWindowsFade()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;
    FLOAT32* pfVolCoeffns = NULL;
    MFTIME CurveDuration = 0;
    IAudioProcessingObjectRTPtr pAPORT = NULL;
    WAVEFORMATEX wfx;
    CComPtr<IBasicLog> pBasicLog;
    
    wfx.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = 48000;
    wfx.wBitsPerSample = sizeof(FLOAT32) * 8;
    wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
    wfx.cbSize = 0;

    _ASSERT( (m_pBaseAPO != NULL) && (m_pVolume != NULL) && (m_pAPO != NULL) );

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
    // This is the test tone and is used for all Functional tests here
    //Generate a 10msec 1Khz FLOAT32 Stereo sine wave, routine is called from tonegen.lib
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
    APO_CONNECTION_PROPERTY* pInputConn = new APO_CONNECTION_PROPERTY;
    APO_CONNECTION_PROPERTY* pOutputConn = new APO_CONNECTION_PROPERTY;    
    if (!pInputConn || !pOutputConn)
    {
        SLOG(eError, "ERROR: Allocating memory for APO Connections");
        dw = FNS_FAIL;
        goto exit;
    }
    // Initialize Input Connection
    // Number of input frames is 480 (10msecs of data at 48K)
    BUILD_CONNECTION(pInputConn, pfSineWave, 480, 0, BUFFER_VALID);

    //Allocate Output buffer for 10msec of 48k stereo  FLOAT32 data
    FLOAT32* pOutputBuffer = (FLOAT32*)LocalAlloc(LMEM_FIXED, 480 * 2 * sizeof(FLOAT32));
    if(!pOutputBuffer)
    {
        SLOG(eError, "ERROR: Allocating memory for Output Buffer");
        dw = FNS_FAIL;
        goto exit;
    }
    // Initialize Output Connection   
    BUILD_CONNECTION(pOutputConn, pOutputBuffer, 480, 0, BUFFER_SILENT );
    
    // Do a Volume transition from 1.0 to 2.0, over different durations for the Param Curve
    
    // Volume change, Param Curve WindowsFade, duration = entire quantum.
    // CurveDuration = 10ms, since that is what our quantum is
    CurveDuration = 10 * MFTIME1MSEC;
    pfVolCoeffns = new FLOAT32[2];
    if(NULL == pfVolCoeffns)
    {
        SLOG(eError, "ERROR: Memory Allocation FAILED");
        dw = FNS_FAIL;
        goto exit;
    }
    pfVolCoeffns[0] = pfVolCoeffns[1] = 2.0;
    SLOG(eInfo1, "Testing APOProcess with CurveDuration = Quantum");
    hr = TestFunctional(2 /*Stereo*/
                        ,pfVolCoeffns
                        ,1 /* Single Input Conn*/
                        ,pInputConn
                        ,1 /*Single Out Conn*/
                        ,pOutputConn
                        ,AUDIO_CURVE_TYPE_WINDOWS_FADE
                        , &CurveDuration);
    
    if(SUCCEEDED(hr))
    {
        // Check if the APO did the right thing.        
        // Calculate the ramp increment
        // The ramp takes the volume from 1.0 to 2.0 over 480 samples
        FLOAT32 f32RampIncrement = (FLOAT32)(2.0 - 1.0)/480;
        FLOAT32 f32Coeffn = 1.0;
        for(UINT32 u32Sample = 0; u32Sample < 480; u32Sample++)
        {   //Check if the values are within .1% of each other
            if( 
                !CCheck::InRange(((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample], f32Coeffn * pfSineWave[2*u32Sample], 0.999, 1.001) ||
                !CCheck::InRange(((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample + 1], f32Coeffn * pfSineWave[2*u32Sample + 1], 0.999, 1.001)
              )
            {
                SLOG(eError, "ERROR: APOProcess did not behave as expected");
                SLOG(eError, "Expected Value: %f, Actual Value: %f, Channel 0, Sample: %d",f32Coeffn * pfSineWave[2*u32Sample], ((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample], u32Sample);
                SLOG(eError, "Expected Value: %f, Actual Value: %f, Channel 1, Sample: %d",f32Coeffn * pfSineWave[2*u32Sample + 1], ((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample+1], u32Sample);
                dw = FNS_FAIL;
                break;
            }
            f32Coeffn += f32RampIncrement;
        }        
    }
    else
    {
        SLOG(eError, "ERROR: Parameter Curve Test FAILED 0x%X, %s", hr, GetHRString(hr));
    }
    m_pVolume = NULL; m_pBaseAPO = NULL; m_pAPO = NULL; m_pMute = NULL; //TestFunctional implicitly Initializes the APO.
    hr = CreateObjects();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Object creation for Volume APO FAILED", dw, FNS_FAIL);

    // Volume change, Param Curve WindowsFade, duration = half quantum.
    // CurveDuration = 5ms, quantum is 10 ms, so the ramp should be over half the frames and 
    // other half should be at a constant value of 2.0 times the source samples
    CurveDuration = 5 * MFTIME1MSEC;
    pfVolCoeffns[0] = pfVolCoeffns[1] = 2.0;
    SLOG(eInfo1, "Testing APOProcess with CurveDuration < Quantum");
    hr = TestFunctional(2 /*Stereo*/
                        ,pfVolCoeffns
                        ,1 /* Single Input Conn*/
                        ,pInputConn
                        ,1 /*Single Out Conn*/
                        ,pOutputConn
                        ,AUDIO_CURVE_TYPE_WINDOWS_FADE
                        , &CurveDuration);
    
    if(SUCCEEDED(hr))
    {
        // Check if the APO did the right thing.        
        // Calculate the ramp increment
        // The ramp takes the volume from 1.0 to 2.0 over 240 samples
        FLOAT32 f32RampIncrement = (FLOAT32)(2.0 - 1.0)/240;
        FLOAT32 f32Coeffn = 1.0;
        for(UINT32 u32Sample = 0; u32Sample < 240; u32Sample++)
        {
            if( 
                !CCheck::InRange(((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample], f32Coeffn * pfSineWave[2*u32Sample], 0.999, 1.001) ||
                !CCheck::InRange(((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample + 1], f32Coeffn * pfSineWave[2*u32Sample + 1], 0.999, 1.001)
              )
            {
                SLOG(eError, "ERROR: APOProcess did not behave as expected");
                SLOG(eError, "Expected Value: %f, Actual Value: %f, Channel 0, Sample: %d",f32Coeffn * pfSineWave[2*u32Sample], ((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample], u32Sample);
                SLOG(eError, "Expected Value: %f, Actual Value: %f, Channel 1, Sample: %d",f32Coeffn * pfSineWave[2*u32Sample + 1], ((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample+1], u32Sample);
                dw = FNS_FAIL;
                break;
            }
            f32Coeffn += f32RampIncrement;
        }        
        for(UINT32 u32Sample = 240; u32Sample < 480; u32Sample++)
        {
            if( 
                !CCheck::InRange(((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample], f32Coeffn * pfSineWave[2*u32Sample], 0.999, 1.001) ||
                !CCheck::InRange(((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample + 1], f32Coeffn * pfSineWave[2*u32Sample + 1], 0.999, 1.001)
              )
            //if( (((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample]     != f32Coeffn * pfSineWave[2*u32Sample]) ||
            //    (((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample + 1] != f32Coeffn * pfSineWave[2*u32Sample + 1]) )
            {
                SLOG(eError, "ERROR: APOProcess did not behave as expected");
                SLOG(eError, "Expected Value: %f, Actual Value: %f, Channel 0, Sample: %d",f32Coeffn * pfSineWave[2*u32Sample], ((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample], u32Sample);
                SLOG(eError, "Expected Value: %f, Actual Value: %f, Channel 1, Sample: %d",f32Coeffn * pfSineWave[2*u32Sample + 1], ((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample+1], u32Sample);
                dw = FNS_FAIL;
                break;
            }            
        }
    }
    else
    {
        SLOG(eError, "ERROR: Parameter Curve Test FAILED 0x%X, %s", hr, GetHRString(hr));
    }
    m_pVolume = NULL; m_pBaseAPO = NULL; m_pAPO = NULL; m_pMute = NULL; //TestFunctional implicitly Initializes the APO.
    hr = CreateObjects();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Object creation for Volume APO FAILED", dw, FNS_FAIL);


    // Volume change, Param Curve WindowsFade, duration = One and a half quantum.
    // CurveDuration = 15ms, quantum is 10 ms, so the ramp would be for more than one quantum
    // Since each call to APOProcess processes 1 quantum worth of data, we'll have to call
    // APOProcess two times for the volume change to fully take effect

    CurveDuration = 15 * MFTIME1MSEC;    
    pfVolCoeffns[0] = pfVolCoeffns[1] = 2.0;
    SLOG(eInfo1, "Testing APOProcess with CurveDuration > Quantum");
    //Call APOProcess with 480 frames
    // the ramp should be over all the samples during this processing pass
    hr = TestFunctional(2 /*Stereo*/
                        ,pfVolCoeffns
                        ,1 /* Single Input Conn*/
                        ,pInputConn
                        ,1 /*Single Out Conn*/
                        ,pOutputConn
                        ,AUDIO_CURVE_TYPE_WINDOWS_FADE
                        , &CurveDuration); 
    // Calculate the ramp increment
    FLOAT32 f32RampIncrement = (FLOAT32)((2.0 - 1.0)/(480 * 1.5));
    FLOAT32 f32Coeffn = 1.0;
    if(SUCCEEDED(hr))
    {
        // Check if the APO did the right thing.
        // Volume ramps up over all the samples in this processing pass
        for(UINT32 u32Sample = 0; u32Sample < 480; u32Sample++)
        {
            if( 
                !CCheck::InRange(((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample], f32Coeffn * pfSineWave[2*u32Sample], 0.999, 1.001) ||
                !CCheck::InRange(((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample + 1], f32Coeffn * pfSineWave[2*u32Sample + 1], 0.999, 1.001)
              )
            //if( (((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample]     != f32Coeffn * pfSineWave[2*u32Sample]) ||
            //    (((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample + 1] != f32Coeffn * pfSineWave[2*u32Sample + 1]) )
            {
                SLOG(eError, "ERROR: APOProcess did not behave as expected");
                SLOG(eError, "Expected Value: %f, Actual Value: %f, Channel 0, Sample: %d",f32Coeffn * pfSineWave[2*u32Sample], ((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample], u32Sample);
                SLOG(eError, "Expected Value: %f, Actual Value: %f, Channel 1, Sample: %d",f32Coeffn * pfSineWave[2*u32Sample + 1], ((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample+1], u32Sample);
                dw = FNS_FAIL;
                break;
            }
            f32Coeffn += f32RampIncrement;
        }        
    }
    else
    {
        SLOG(eError, "ERROR: Parameter Curve Test FAILED 0x%X, %s", hr, GetHRString(hr));
        dw = FNS_FAIL;
        goto exit;
    }
   
    //Call APOProcess with 480 frames, for the next time
    // the ramp should be over half of all samples and the other half should be 2 times the input samples
    
    //Lock the APO, Call APO Process and Unlock the APO    
    SLOG(eInfo1, "Locking APO for Processing");
    AUDIO_FRAME_FORMAT AudioFormat;         
    BUILD_FORMAT(&AudioFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 2/* stereo*/, 4, 32, 48000);
    IAudioMediaType* pAudioMediaType = NULL;
    CAudioMediaType::AF2AMT(AudioFormat, &pAudioMediaType);
    APO_CONNECTION_DESCRIPTOR InputConnDesc, OutputConnDesc;
    APO_CONNECTION_DESCRIPTOR *pInputConnDesc = NULL, *pOutputConnDesc = NULL;
    // Descriptors below have 0x1 as the Buffer, because we don't care about the buffer,
    // and the API checks for NULL
    BUILD_DESCRIPTOR(&InputConnDesc, pAudioMediaType,
                     APO_CONNECTION_BUFFER_TYPE_EXTERNAL,
                     0x1, 480);
    BUILD_DESCRIPTOR(&OutputConnDesc, pAudioMediaType,
                     APO_CONNECTION_BUFFER_TYPE_EXTERNAL,
                     0x1, 480);
    pInputConnDesc = &InputConnDesc;
    pOutputConnDesc = &OutputConnDesc;
    hr = m_pBaseAPO->LockForProcess(1, &pInputConnDesc, 1, &pOutputConnDesc);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "Error Locking the APO for Processing", dw, FNS_FAIL);
    pAudioMediaType->Release();         
    SLOG(eInfo1, "Creating APORT Object");
    hr = m_pVolume.QueryInterface(__uuidof(IAudioProcessingObjectRT), &pAPORT);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "Error Querying APORT object", dw, FNS_FAIL);    
    //Call APOProcess... 
    SLOG(eInfo1, "Calling APO Process");
    pAPORT->APOProcess(1, &pInputConn, 1, &pOutputConn);
    //Unlock APO
    SLOG(eInfo1, "UnLocking APO....");
    hr = m_pBaseAPO->UnlockForProcess();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "Error UnLocking the APO for Processing", dw, FNS_FAIL);
    if(SUCCEEDED(hr))
    {
        // Check if the APO did the right thing.                
        for(UINT32 u32Sample = 0; u32Sample < 240; u32Sample++)
        {
            if( 
                !CCheck::InRange(((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample], f32Coeffn * pfSineWave[2*u32Sample], 0.999, 1.001) ||
                !CCheck::InRange(((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample + 1], f32Coeffn * pfSineWave[2*u32Sample + 1], 0.999, 1.001)
              )
            //if( (((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample]     != f32Coeffn * pfSineWave[2*u32Sample]) ||
            //    (((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample + 1] != f32Coeffn * pfSineWave[2*u32Sample + 1]) )
            {
                SLOG(eError, "ERROR: APOProcess did not behave as expected");
                SLOG(eError, "Expected Value: %f, Actual Value: %f, Channel 0, Sample: %d",f32Coeffn * pfSineWave[2*u32Sample], ((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample], u32Sample);
                SLOG(eError, "Expected Value: %f, Actual Value: %f, Channel 1, Sample: %d",f32Coeffn * pfSineWave[2*u32Sample + 1], ((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample+1], u32Sample);
                dw = FNS_FAIL;
                break;
            }
            f32Coeffn += f32RampIncrement;
        }
        for(UINT32 u32Sample = 240; u32Sample < 480; u32Sample++)
        {
            if( 
                !CCheck::InRange(((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample], f32Coeffn * pfSineWave[2*u32Sample], 0.999, 1.001) ||
                !CCheck::InRange(((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample + 1], f32Coeffn * pfSineWave[2*u32Sample + 1], 0.999, 1.001)
              )
            //if( (((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample]     != f32Coeffn * pfSineWave[2*u32Sample]) ||
            //    (((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample + 1] != f32Coeffn * pfSineWave[2*u32Sample + 1]) )
            {
                SLOG(eError, "ERROR: APOProcess did not behave as expected");
                SLOG(eError, "Expected Value: %f, Actual Value: %f, Channel 0, Sample: %d",f32Coeffn * pfSineWave[2*u32Sample], ((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample], u32Sample);
                SLOG(eError, "Expected Value: %f, Actual Value: %f, Channel 1, Sample: %d",f32Coeffn * pfSineWave[2*u32Sample + 1], ((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample+1], u32Sample);
                dw = FNS_FAIL;
                break;
            }            
        }
    }
    else
    {
        SLOG(eError, "ERROR: Parameter Curve Test FAILED 0x%X, %s", hr, GetHRString(hr));
        dw = FNS_FAIL;
    }
    m_pVolume = NULL; m_pBaseAPO = NULL; m_pAPO = NULL; m_pMute = NULL; //TestFunctional implicitly Initializes the APO.
    hr = CreateObjects();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Object creation for Volume APO FAILED", dw, FNS_FAIL);
    
exit:
    SAFE_FREE_POINTER(pfSineWave);
    SAFE_FREE_POINTER(pOutputBuffer);
    SAFE_DELETE_POINTER(pInputConn);
    SAFE_DELETE_POINTER(pOutputConn);
    SAFE_DELETE_ARRAY(pfVolCoeffns);
    return dw;
}//CAPOVolumeTest::APOTestParamCurveWindowsFade

DWORD CAPOVolumeTest::APOTestNoInitialRamp
(
    AUDIO_CURVE_TYPE                       ParamCurve
)
{
    HRESULT                                 hr = S_OK;
    DWORD                                   dw = FNS_PASS;
    FLOAT32                                *pfVolCoeffns = NULL;
    MFTIME                                  CurveDuration = 0;
    FLOAT32                                 fTargetVolume = 2.0;
    CComPtr<IAudioProcessingObjectRT>       pIAPORT = NULL;
    WAVEFORMATEX                            wfx;
    CComPtr<IBasicLog>                      pBasicLog;
    
    wfx.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = 48000;
    wfx.wBitsPerSample = sizeof(FLOAT32) * 8;
    wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
    wfx.cbSize = 0;

    _ASSERT( (m_pBaseAPO != NULL) && (m_pVolume != NULL) && (m_pAPO != NULL) );

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
    // This is the test tone and is used for all Functional tests here
    //Generate a 10msec 1Khz FLOAT32 Stereo sine wave, routine is called from tonegen.lib
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
    APO_CONNECTION_PROPERTY* pInputConn = new APO_CONNECTION_PROPERTY;
    APO_CONNECTION_PROPERTY* pOutputConn = new APO_CONNECTION_PROPERTY;    
    if (!pInputConn || !pOutputConn)
    {
        SLOG(eError, "ERROR: Allocating memory for APO Connections");
        dw = FNS_FAIL;
        goto exit;
    }
    // Initialize Input Connection
    // Number of input frames is 480 (10msecs of data at 48K)
    BUILD_CONNECTION(pInputConn, pfSineWave, 480, 0, BUFFER_VALID);

    //Allocate Output buffer for 10msec of 48k stereo  FLOAT32 data
    FLOAT32* pOutputBuffer = (FLOAT32*)LocalAlloc(LMEM_FIXED, 480 * 2 * sizeof(FLOAT32));
    if(!pOutputBuffer)
    {
        SLOG(eError, "ERROR: Allocating memory for Output Buffer");
        dw = FNS_FAIL;
        goto exit;
    }
    // Initialize Output Connection   
    BUILD_CONNECTION(pOutputConn, pOutputBuffer, 480, 0, BUFFER_SILENT );
    
    // Do a Volume transition from 1.0 to 2.0, over different durations for the Param Curve
    
    // Volume change, Param Curve WindowsFade, duration = entire quantum.
    // CurveDuration = 10ms, since that is what our quantum is
    CurveDuration = 10 * MFTIME1MSEC;
    pfVolCoeffns = new FLOAT32[2];
    if(NULL == pfVolCoeffns)
    {
        SLOG(eError, "ERROR: Memory Allocation FAILED");
        dw = FNS_FAIL;
        goto exit;
    }
    SLOG(eInfo1, "Testing initial APOProcess will immediately set the volume without ramping.");
    if (S_OK != (hr = m_pVolume.QueryInterface(__uuidof(IAudioProcessingObjectRT), &pIAPORT)))
    {
        SLOG(eError, "ERROR: QueryInterface for IAudioProcesingObjectRT returned error (0x%08lx).", hr);
        dw = FNS_FAIL;
        goto exit;
    }
    
    pfVolCoeffns[0] = pfVolCoeffns[1] = 1.0;
    if (S_OK != (hr = LockAPO(2, pfVolCoeffns, NULL, ParamCurve, &CurveDuration)))
    {
        SLOG(eError, "ERROR: LockAPO returned error (0x%08lx).", hr);
        dw = FNS_FAIL;
        goto exit;
    }
    
    pfVolCoeffns[0] = pfVolCoeffns[1] = fTargetVolume;
    hr = m_pVolume->SetVolumeCoefficients(2, pfVolCoeffns, NULL, ParamCurve, &CurveDuration);
    if (S_OK != hr)
    {
        SLOG(eError, "ERROR: SetVolumeCoefficients returned error (0x%08lx).", hr);
        dw = FNS_FAIL;
        goto exit;
    }
    
    pIAPORT->APOProcess(1, &pInputConn, 1, &pOutputConn);
    
    if (S_OK != (hr = m_pBaseAPO->UnlockForProcess()))
    {
        SLOG(eError, "ERROR: SetVolumeCoefficients returned error (0x%08lx).", hr);
        dw = FNS_FAIL;
        goto exit;
    }

    for(UINT32 u32Sample = 0; u32Sample < 480; u32Sample++)
    {   //Check if the values are within .1% of each other
        if ( 
            !CCheck::InRange(((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample],  fTargetVolume * pfSineWave[2*u32Sample], 0.999, 1.001) ||
            !CCheck::InRange(((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample + 1], fTargetVolume * pfSineWave[2*u32Sample + 1], 0.999, 1.001)
           )
        {
            SLOG(eError, "ERROR: APOProcess did not behave as expected");
            SLOG(eError, "Expected Value: %f, Actual Value: %f, Channel 0, Sample: %d",fTargetVolume * pfSineWave[2*u32Sample], ((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample], u32Sample);
            SLOG(eError, "Expected Value: %f, Actual Value: %f, Channel 1, Sample: %d",fTargetVolume * pfSineWave[2*u32Sample + 1], ((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample+1], u32Sample);
            dw = FNS_FAIL;
            break;
        }
    }        
    
exit:
    SAFE_FREE_POINTER(pfSineWave);
    SAFE_FREE_POINTER(pOutputBuffer);
    SAFE_DELETE_POINTER(pInputConn);
    SAFE_DELETE_POINTER(pOutputConn);
    SAFE_DELETE_ARRAY(pfVolCoeffns);
    return dw;
}
