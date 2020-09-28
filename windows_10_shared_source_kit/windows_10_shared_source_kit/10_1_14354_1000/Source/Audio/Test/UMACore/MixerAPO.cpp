// Copyright(C) Microsoft. All rights reserved.

#include "stdafx.h"


extern const UINT32 g_nSystemAPOs;
extern LPCWSTR g_aSystemAposGuids[14];
//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOMixerCreate
//
// Description:
//  TestCase to test if AudioMixer class is registered and IAudioMixer interface can be obtained from it
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOMixerCreate()
{
    CAPOMixerTest APOMixerTest;
    return APOMixerTest.APOMixerCreate();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOMixerSetMix
//
// Description:
//  TestCase to test SetMix method on IAudioMixer
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
//DWORD Tc_APOMixerSetMix()
//{
//    CAPOMixerTest APOMixerTest;
//    return APOMixerTest.APOSetMix();
//}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOMixerGetMix
//
// Description:
//  TestCase to test GetMix method on IAudioMixer
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
//DWORD Tc_APOMixerGetMix()
//{
//    CAPOMixerTest APOMixerTest;
//    return APOMixerTest.APOGetMix();
//}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOMixerProcess
//
// Description:
//  TestCase to test APOProcess on Mixer
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//

DWORD Tc_APOMixerProcess()
{
    CAPOMixerTest APOMixerTest;
    return APOMixerTest.APOTestMixer();
}


//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOMixerProcesswMix_Copy
//
// Description:
//  TestCase to test APOProcess on Mixer with Processor and Pump for a MIX operation and then a COPY operation.
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOMixerProcesswProcMix_Copy()
{
    CAPOMixerTest APOMixerTest;
    return APOMixerTest.APOTestwProcMix_Copy();
}
//-------------------------------------------------------------------------
// Class: CAPOMixerTest
// Method: APOMixerCreate
//
// Description:
//   Actual testcase to check if AudioMixer is registered with COM and if IAudioMixer and  
//   other APO objects can be successfully obtained from it.
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOMixerTest::APOMixerCreate()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;    
    IAudioMixerPtr                          pMix = NULL;    
    IAudioProcessingObjectPtr               pAPO = NULL;
    IAudioProcessingObjectRTPtr             pAPORT = NULL;
    IAudioProcessingObjectConfigurationPtr  pAPOConfig  = NULL;
    
    
    //Check if we can get all the necessary Objects from AudioMixer
    SLOG(eInfo1, "Creating Audio Mixer object");
    hr = pMix.CreateInstance(__uuidof(AudioMixer));
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: FAILED to create Audio Mixer object, 0x%X", hr);
        dw = FNS_FAIL;
    }
    SLOG(eInfo1, "Querying APO object from Mixer");
    hr = pMix.QueryInterface(__uuidof(IAudioProcessingObject), &pAPO);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: FAILED to query for  APO object from Mixer, 0x%X", hr);
        dw = FNS_FAIL;
    }
    SLOG(eInfo1, "Querying APORT object from Mixer");
    hr = pMix.QueryInterface(__uuidof(IAudioProcessingObjectRT), &pAPORT);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: FAILED to query for  APORT object from Mixer, 0x%X", hr);
        dw = FNS_FAIL;
    }
    SLOG(eInfo1, "Querying APO Config object from Mixer");
    hr = pMix.QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), &pAPOConfig);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: FAILED to query for  APO config object from Mixer, 0x%X", hr);
        dw = FNS_FAIL;
    }

    return dw;
}

//-------------------------------------------------------------------------
// Class: CAPOMixerTest
// Method: APOSetMix
//
// Description:
//   Actual testcase to test IAudioMixer->SetMix API call 
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
//DWORD CAPOMixerTest::APOSetMix()
//{
//    DWORD dw = FNS_PASS;
//    HRESULT hr = S_OK;
//    AUDIO_MIXER_FLAGS audMixFlags;
//    AUDIO_MIXER_FLAGS audMixGetFlags;    
//
//    _ASSERT( (m_pMixer != NULL) && (m_pBaseAPO != NULL) );
//    //if ((m_pMixer == NULL) || (m_pBaseAPO == NULL))
//    //{
//    //    hr = CreateObjects();
//    //    if(FAILED(hr))
//    //    {
//    //        SLOG(eError, "ERROR: FAILED Trying to create APO objects, 0x%X", hr);
//    //        dw = FNS_FAIL;
//    //        goto exit;
//    //    }
//    //}
//    //testcase
//    SLOG(eInfo1, "Calling SetMix with Flags = 0xFFFF");
//    memset((LPVOID)&audMixFlags, 0xFF, sizeof(AUDIO_MIXER_FLAGS));
//    hr = m_pMixer->SetMix(audMixFlags);
//    if(SUCCEEDED(hr))
//    {
//        SLOG(eError, "SetMix SUCCEEDED for Mix Flags = 0xFFFF, 0x%X", hr);
//        dw = FNS_FAIL;
//    }
//    //testcase
//    SLOG(eInfo1, "Calling SetMix with Flags = AUDIO_MIXER_COPY");
//    audMixFlags = AUDIO_MIXER_COPY;
//    hr = m_pMixer->SetMix(audMixFlags);
//    if(FAILED(hr))
//    {
//        SLOG(eError, "SetMix FAILED for Mix Flags = AUDIO_MIXER_COPY, 0x%X", hr);
//        dw = FNS_FAIL;
//    }
//    else
//    {
//        SLOG(eInfo1, "Calling GetMix to verify what we just set");
//        hr = m_pMixer->GetMix(&audMixGetFlags);
//        if(FAILED(hr))
//        {
//            SLOG(eError, "GetMix FAILED for Mix Flags = AUDIO_MIXER_COPY, 0x%X", hr);
//            dw = FNS_FAIL;
//        }
//        if(audMixGetFlags != audMixFlags)
//        {
//            SLOG(eError, "ERROR:GetMix inconsistent with SetMix, 0x%X", hr);
//            dw = FNS_FAIL;
//        }
//    }
//    //testcase
//    SLOG(eInfo1, "Calling SetMix with Flags = AUDIO_MIXER_MIX");
//    audMixFlags = AUDIO_MIXER_MIX;
//    hr = m_pMixer->SetMix(audMixFlags);
//    if(FAILED(hr))
//    {
//        SLOG(eError, "SetMix FAILED for Mix Flags = AUDIO_MIXER_MIX, 0x%X", hr);
//        dw = FNS_FAIL;
//    }
//    else
//    {
//        SLOG(eInfo1, "Calling GetMix to verify what we just set");
//        hr = m_pMixer->GetMix(&audMixGetFlags);
//        if(FAILED(hr))
//        {
//            SLOG(eError, "GetMix FAILED for Mix Flags = AUDIO_MIXER_MIX, 0x%X", hr);
//            dw = FNS_FAIL;
//        }
//        if(audMixGetFlags != audMixFlags)
//        {
//            SLOG(eError, "ERROR:GetMix inconsistent with SetMix, 0x%X", hr);
//            dw = FNS_FAIL;
//        }
//    }
//    //testcase
//    // Vary the mixer flags after the APO has been locked.
//    // Set the Mixer for COPY, Lock and then set it for MIX
//    SLOG(eInfo1, "Trying to change Mixer Flags after it has been Locked");
//    SLOG(eInfo1, "Initializing the APO for COPY and Locking it");
//    hr =  LockAPO(AUDIO_MIXER_COPY);
//    if(FAILED(hr))
//    {
//        SLOG(eError, "ERROR:Trying to LockAPO, 0x%X", hr);
//        dw = FNS_FAIL;
//    }
//    else
//    {
//        SLOG(eInfo1, "Calling SetMix with Flags MIXER_MIX");
//        hr = m_pMixer->SetMix(AUDIO_MIXER_MIX);
//        if(FAILED(hr))
//        {
//            SLOG(eError, "ERROR:Trying to SetMix to MIXER_MIX after Locking the APO FAILED, 0x%X", hr);
//            dw = FNS_FAIL;
//        }
//        m_pBaseAPO->UnlockForProcess();
//    }
//    // Set the Mixer for MIX, Lock and then set it for COPY    
//    SLOG(eInfo1, "Trying to change Mixer Flags after it has been Locked");
//    SLOG(eInfo1, "Initializing the APO for MIX and Locking it");
//    hr =  LockAPO(AUDIO_MIXER_MIX);
//    if(FAILED(hr))
//    {
//        SLOG(eError, "ERROR:Trying to LockAPO, 0x%X", hr);
//        dw = FNS_FAIL;
//    }
//    else    
//    {
//        SLOG(eInfo1, "Calling SetMix with Flags MIXER_COPY");
//        hr = m_pMixer->SetMix(AUDIO_MIXER_COPY);
//        if(FAILED(hr))
//        {
//            SLOG(eError, "ERROR:Trying to SetMix to MIXER_COPY after Locking the APO FAILED, 0x%X", hr);
//            dw = FNS_FAIL;
//        }
//        m_pBaseAPO->UnlockForProcess();
//    }
//
//
//exit:
//    return dw;
//}


//-------------------------------------------------------------------------
// Class: CAPOMixerTest
// Method: APOGetMix
//
// Description:
//   Actual testcase to test IAudioMixer->GetMix API call 
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
//DWORD CAPOMixerTest::APOGetMix()
//{
//    DWORD dw = FNS_PASS;
//    HRESULT hr = S_OK;
//    AUDIO_MIXER_FLAGS audMixFlags;
//    AUDIO_MIXER_FLAGS audMixSetFlags;
//
//    _ASSERT( (m_pMixer != NULL) && (m_pBaseAPO != NULL) );
//
// /*   if ((m_pMixer == NULL) || (m_pBaseAPO == NULL))
//    {
//        hr = CreateObjects();
//        if(FAILED(hr))
//        {
//            SLOG(eError, "ERROR: FAILED Trying to create APO objects, 0x%X", hr);
//            dw = FNS_FAIL;
//            goto exit;
//        }
//    }*/
//    //testcase
//    SLOG(eInfo1, "Calling GetMix NULL ptr");    
//    hr = m_pMixer->GetMix(NULL);
//    if(SUCCEEDED(hr))
//    {
//        SLOG(eError, "ERROR: GetMix SUCCEEDED with NULL ptr, 0x%X", hr);
//        dw = FNS_FAIL;
//    }
//    //testcase
//    SLOG(eInfo1, "Verifying Set and GetMix");    
//    SLOG(eInfo1, "Calling SetMix with Flags = AUDIO_MIXER_COPY");
//    audMixSetFlags = AUDIO_MIXER_COPY;
//    hr = m_pMixer->SetMix(audMixSetFlags);
//    if(FAILED(hr))
//    {
//        SLOG(eError, "ERROR: SetMix FAILED for Mix Flags = AUDIO_MIXER_COPY, 0x%X", hr);
//        dw = FNS_FAIL;
//    }
//    else
//    {
//        SLOG(eInfo1, "Calling GetMix to verify what we just set");
//        hr = m_pMixer->GetMix(&audMixFlags);
//        if(FAILED(hr))
//        {
//            SLOG(eError, "ERROR: GetMix FAILED for Mix Flags = AUDIO_MIXER_COPY, 0x%X", hr);
//            dw = FNS_FAIL;
//        }
//        if(audMixSetFlags != audMixFlags)
//        {
//            SLOG(eError, "ERROR: GetMix inconsistent with SetMix, 0x%X", hr);
//            dw = FNS_FAIL;
//        }
//    }
//    //testcase
//    SLOG(eInfo1, "Calling SetMix with Flags = AUDIO_MIXER_MIX");
//    audMixSetFlags = AUDIO_MIXER_MIX;
//    hr = m_pMixer->SetMix(audMixSetFlags);
//    if(FAILED(hr))
//    {
//        SLOG(eError, "ERROR: SetMix FAILED for Mix Flags = AUDIO_MIXER_MIX, 0x%X", hr);
//        dw = FNS_FAIL;
//    }
//    else
//    {
//        SLOG(eInfo1, "Calling GetMix to verify what we just set");
//        hr = m_pMixer->GetMix(&audMixFlags);
//        if(FAILED(hr))
//        {
//            SLOG(eError, "ERROR: GetMix FAILED for Mix Flags = AUDIO_MIXER_MIX, 0x%X", hr);
//            dw = FNS_FAIL;
//        }
//        if(audMixSetFlags != audMixFlags)
//        {
//            SLOG(eError, "ERROR: GetMix inconsistent with SetMix, 0x%X", hr);
//            dw = FNS_FAIL;
//        }
//    }
//
//exit:
//    return dw;
//}


//-------------------------------------------------------------------------
// Class: CAPOMixerTest
// Method: APOTestMixer
//
// Description:
//   Functional testcase for Mixer APO
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOMixerTest::APOTestMixer()
{
    DWORD   dw = FNS_PASS;
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

    _ASSERT( (m_pMixer != NULL) && (m_pBaseAPO != NULL) );


    //Allocate memory for sine wave, 10msec 1KHz FLOAT32 Stereo sinewave at 48KHz sampling rate
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
    BUILD_CONNECTION(pOutputConn, pOutputBuffer, 480, 0, BUFFER_SILENT);

    //Audio format for our test
    AUDIO_FRAME_FORMAT AudioFormat;
    BUILD_FORMAT(&AudioFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 2, 4, 32, 48000);
    
    //Get IAudioMediaType for the current format
    IAudioMediaType* pAudioMediaType;
    CAudioMediaType::AF2AMT(AudioFormat, &pAudioMediaType);
    
    // Call the Test function and test for COPY
    hr = TestFunctional(1, /*Num In Connections*/
                        &pAudioMediaType,
                        pInputConn,
                        1, /*Num Out Connections*/
                        &pAudioMediaType,
                        pOutputConn);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Functional Test failed for COPY ");
        dw = FNS_FAIL;        
    }
    else //SUCCEEDED
    {
        // Check that the input channels are approp replicated on the output channels
        for(UINT32 u32Sample = 0; u32Sample < 480; u32Sample++)
        {
            if( (((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample]     != pfSineWave[2*u32Sample]) ||//Both channels are 
                (((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample + 1] != pfSineWave[2*u32Sample + 1]) )// replicated on the output
            {
                SLOG(eError, "ERROR: APOProcess FAILED to behave as expected ");
                dw = FNS_FAIL;
                goto exit;
            }
        }
    }
    // Set the Mixer to do a mix, and mix the sinewave into the output buffer
    // The resultant output should be 2 times the input, if the above test SUCCEEDED.
    // If the above test FAILED then , this test will FAIL too.
    // The earlier Process should have changed the OutputConnection Buffer flags to BUFFER_VALID
    // Hence on this pass the APO should do a Mix instead of a COPY
    // Call the Test function and test for MIX
    hr = TestFunctional(1, /*Num In Connections*/
                        &pAudioMediaType,
                        pInputConn,
                        1, /*Num Out Connections*/
                        &pAudioMediaType,
                        pOutputConn);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Functional Test failed for MIX ");
        dw = FNS_FAIL;        
    }
    else //SUCCEEDED
    {
        // Check that the input channels are approp replicated (2 times)  on the output channels
        for(UINT32 u32Sample = 0; u32Sample < 480; u32Sample++)
        {
            if( (((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample]     != 2 * pfSineWave[2*u32Sample]) ||//Both channels are 
                (((FLOAT32*)(pOutputConn->pBuffer))[2*u32Sample + 1] != 2 * pfSineWave[2*u32Sample + 1]) )// replicated on the output
            {
                SLOG(eError, "ERROR: APOProcess FAILED to behave as expected ");
                dw = FNS_FAIL;
                break;
            }
        }
    }

exit:
    SAFE_RELEASE_POINTER(pAudioMediaType);
    SAFE_DELETE_POINTER(pInputConn);
    SAFE_DELETE_POINTER(pOutputConn);
    SAFE_FREE_POINTER(pfSineWave);
    SAFE_FREE_POINTER(pOutputBuffer);
    

    return dw;

}

//-------------------------------------------------------------------------
// Class: CAPOMixerTest
// Method: Initialize
//
// Description:
//   Internal method used to initialize the APO
// Parameters:
//
// Return values (HRESULT):
//     Return S_OK on Success, E_FAIL or some other HRESULT error code on Failure
//
//HRESULT CAPOMixerTest::Initialize()
//{
//    HRESULT hr = S_OK;
//
//    hr = m_pMixer->SetMix(AUDIO_MIXER_COPY);
//    if(FAILED(hr))
//    {
//        SLOG(eError, "ERROR: SetMix FAILED for Mix Flags = AUDIO_MIXER_COPY, 0x%X", hr);       
//    }
//
//    return hr;
//}

//-------------------------------------------------------------------------
// Class: CAPOMixerTest
// Method: CreateObjects
//
// Description:
//  Internal method used to create an instances of IAudioMixer and IAPO
// Parameters:
//
// Return values (HRESULT):
//     Return S_OK on Success, E_FAIL or some other HRESULT error code on Failure
//
HRESULT CAPOMixerTest::CreateObjects()
{
    HRESULT hr = S_OK;

    if(m_pMixer == NULL)
    {
        SLOG(eInfo1, "Creating Mixer Object");
        m_pMixer.CreateInstance(__uuidof(AudioMixer));
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Creating Mixer Object, 0x%X", hr);
            goto exit;
        }
    }
    if(m_pBaseAPO == NULL)
    {
        SLOG(eInfo1, "Creating APO Object");
        hr = m_pMixer.QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), &m_pBaseAPO);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Creating APO Object, 0x%X", hr);
            goto exit;
        }
    }

exit:
    return hr;
}


//-------------------------------------------------------------------------
// Class: CAPOMixerTest
// Method: LockAPO
//
// Description:
//  Internal method to Lock the Mixer APO for Processing
//  It does the necessary initialization and calls LockForProcess
// Parameters:
//
// Return values (HRESULT):
//     Return S_OK on Success, E_FAIL or some other HRESULT error code on Failure
//
//HRESULT CAPOMixerTest::LockAPO(/*AUDIO_MIXER_FLAGS audMixFlags*/)
//{
//    HRESULT hr = S_OK;    
//
//    //SLOG(eInfo1, "Setting Mix Flag to %s", 
//    //                    (audMixFlags==AUDIO_MIXER_COPY) ?  "AUDIO_MIXER_COPY" : "AUDIO_MIXER_MIX");
//    //hr = m_pMixer->SetMix(audMixFlags);
//    //if(FAILED(hr))
//    //{
//    //    SLOG(eError, "ERROR: FAILED Trying to Set Mix Flags, 0x%X", hr);        
//    //    goto exit;
//    //}
//
//    SLOG(eInfo1, "Locking APO for Processing");    
//    AUDIO_FRAME_FORMAT *pInConns = NULL, *pOutConns = NULL;
//    pInConns = new AUDIO_FRAME_FORMAT;
//    pOutConns = new AUDIO_FRAME_FORMAT;  
//    INITIALIZE_FRAME_FORMAT(pInConns, const_cast<LPGUID>(&KSDATAFORMAT_SUBTYPE_IEEE_FLOAT), 1, 4, 32, 44100);
//    INITIALIZE_FRAME_FORMAT(pOutConns, const_cast<LPGUID>(&KSDATAFORMAT_SUBTYPE_IEEE_FLOAT), 1, 4, 32, 44100);
//    hr = m_pBaseAPO->LockForProcess(1, &pInConns, 1, &pOutConns);
//    if(FAILED(hr))
//    {
//        SLOG(eError, "ERROR: FAILED Trying to Lock APO, 0x%X", hr);        
//        goto exit;
//    }    
//
//exit:
//    return hr;
//}



//-------------------------------------------------------------------------
// Class: CAPOMixerTest
// Method: TestFunctional
//
// Description:
//   Internal method that Locks the APO for Processing, 
//   Calls APO Process and then UnLocks it.
// Parameters:
//      UINT32 u32NumInConn
//          The number of Input Connections
//      IAudioMediaType** ppInputAudioType
//          Array of InputAudioMediaType pointers, i.e the Input Format
//      APO_CONNECTION_PROPERTY* pInConn
//         The Input Connections passed to APOProcess
//      UINT32 u32NumOutConn
//          The number of Output Connections
//      IAudioMediaType** ppOutputAudioType
//          Array of InputAudioMediaType pointers, i.e the Output Format
//      APO_CONNECTION_PROPERTY* pOutConn
//         The output Connections passed to APOProcess
//
// Return values (HRESULT):
//     Return S_OK on Success, appropriate error code on Failure
//
HRESULT CAPOMixerTest::TestFunctional( UINT32 u32NumInConn
                                      ,IAudioMediaType** ppInputAudioType
                                      ,APO_CONNECTION_PROPERTY* pInConn
                                      ,UINT32 u32NumOutConn
                                      ,IAudioMediaType** ppOutputAudioType
                                      ,APO_CONNECTION_PROPERTY* pOutConn
                                     )
{
    HRESULT hr = S_OK;

    IAudioProcessingObjectRTPtr pAPORT = NULL;

    _ASSERT((m_pMixer != NULL) && (m_pBaseAPO != NULL));

    SLOG(eInfo1, "Creating APORT Object");
    hr = m_pMixer.QueryInterface(__uuidof(IAudioProcessingObjectRTPtr), &pAPORT);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Creation of APORT Object FAILED, 0x%X", hr);        
        goto exit;
    } 

    //Lock the APO
    APO_CONNECTION_DESCRIPTOR InputConnection, OutputConnection;
    APO_CONNECTION_DESCRIPTOR *pInputConnDesc = NULL, *pOutputConnDesc = NULL;    
    BUILD_DESCRIPTOR(&InputConnection, *ppInputAudioType, 
                     APO_CONNECTION_BUFFER_TYPE_EXTERNAL, 
                     pInConn->pBuffer, pInConn->u32ValidFrameCount);
    BUILD_DESCRIPTOR(&OutputConnection, *ppOutputAudioType,
                     APO_CONNECTION_BUFFER_TYPE_EXTERNAL,
                     pOutConn->pBuffer, pOutConn->u32ValidFrameCount);

    pInputConnDesc = &InputConnection;
    pOutputConnDesc = &OutputConnection;
    hr = m_pBaseAPO->LockForProcess(1, &pInputConnDesc, 1, &pOutputConnDesc);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Locking APO FAILED, 0x%X", hr);        
        goto exit;
    }
    //Call APOProcess... 
    SLOG(eInfo1, "Calling APO Process");
    pAPORT->APOProcess(u32NumInConn, &pInConn, u32NumOutConn, &pOutConn);
    
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

/*
//-------------------------------------------------------------------------
// Class: CAPOMixerTest
// Method: APOTestwProcCopy
//
// Description:
//  Test case to test Mixer COPY functionality with Processor
// This is a functional testcase and uses the Pump, Processor and the whole shebang
// Also it uses a Volume APO just for kicks at the output of the Mixer APO.
// Parameters:
//
// Return values (HRESULT):
//     Return FNS_PASS on Success, FNS_FAIL
//
DWORD CAPOMixerTest::APOTestwProcCopy()
{
    DWORD dw = FNS_PASS;
    HRESULT hr = S_OK;
    IAudioProcessorPtr AudioProc;
    IAudioPumpPtr AudPump;    
    FLOAT32* pfSineWave = NULL;
    FLOAT32* pfOutput = NULL; //Ouput data
    CComObject<CMemoryAudioEndpoint>* pInAudioEndpoint = new CComObject<CMemoryAudioEndpoint>();;
    CComObject<CMemoryAudioEndpoint>* pOutAudioEndpoint = new CComObject<CMemoryAudioEndpoint>();;
    IAudioProcessingObject* pAPOMixer = NULL;
    IAudioProcessingObject* pAPOVolume = NULL;
    IAudioVolume* pVol = NULL;
    UINT_PTR hTransaction = NULL;
    UINT_PTR hAPOInputConnection = NULL;
    UINT_PTR hAPOMiddleConnection = NULL;
    UINT_PTR hAPOOutputConnection = NULL;
    UINT32 u32NumFrames = 1440; //480 * 3 = 30 ms of 48K data
    WAVEFORMATEX wfx;
    CComPtr<IBasicLog> pBasicLog;
    
    wfx.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = 48000;
    wfx.wBitsPerSample = sizeof(FLOAT32) * 8;
    wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
    wfx.cbSize = 0;

    _ASSERT( (m_pMixer != NULL) && (m_pBaseAPO != NULL) );
    _ASSERT( (pInAudioEndpoint) && (pOutAudioEndpoint) );
    pInAudioEndpoint->AddRef();
    pOutAudioEndpoint->AddRef();

    {
        IAudioEndpointPtr pInEndpoint = NULL;
        IAudioEndpointPtr pOutEndpoint = NULL;
        IAudioEndpointRTPtr pOutEndpointRT = NULL;
        IAudioProcessRTPtr AudioProcRT = NULL;
        //Audio Processor Creation
        SLOG(eInfo1, "Creating Audio Processor");
        hr = AudioProc.CreateInstance(__uuidof(AudioProcessor));
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Processor Creation FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }

        //AudioPump Creation
        SLOG(eInfo1, "Creating Audio Pump");
        hr = AudPump.CreateInstance(__uuidof(AudioPump));
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Pump Creation FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }   

        //Allocate Input and Output data memory, 30 msec 48Khz, FLOAT32 stereo i.e. 1440 frames
        UINT32 cbBufferSize = sizeof(FLOAT32) * 2 * u32NumFrames; //2 is for Stereo data.
        pfSineWave = (FLOAT32*) LocalAlloc(LMEM_FIXED, cbBufferSize);        
        pfOutput = (FLOAT32*) LocalAlloc(LMEM_FIXED, cbBufferSize);
        _ASSERT(pfSineWave && pfOutput);

        //Generate 30msec 1Khz FLOAT32 Stereo sine wave
        SLOG(eInfo1, "Generating Input SineWave");

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
            u32NumFrames,
            cbBufferSize
        );
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Could not fill buffer with sine", dw, FNS_FAIL);

        //Create and Initialize APO
        //Create AudioMixer        
        
        SLOG(eInfo1, "Creating Volume APO");
        hr = AudioProc->CreateAPO(__uuidof(AudioVolume), &pAPOVolume);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Volume APO Creation FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        SLOG(eInfo1, "Creating Mixer APO");
        hr = AudioProc->CreateAPO(__uuidof(AudioMixer), &pAPOMixer);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Mixer APO Creation FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Initialize Endpoints
        AUDIO_FRAME_FORMAT AudioFormat;
        BUILD_FORMAT(&AudioFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 2, 4, 32, 48000);
        // Although the Endpoints have storage for 30 ms worth of data, the graph is being run at
        // 10ms, so the endpoints should never have to deal with more than 10ms of data at any time
        UINT32 u32MaxEndpointNumFrames = u32NumFrames / 3;
        // Initialize InputEndpoint
        SLOG(eInfo1, "Initializing Input Endpoint");
        hr = pInAudioEndpoint->Initialize(&AudioFormat, pfSineWave, cbBufferSize, u32MaxEndpointNumFrames, TRUE);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Input Endpoint Initialization FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        // Initialize OutputEndpoint
        SLOG(eInfo1, "Initializing Output Endpoint");
        hr = pOutAudioEndpoint->Initialize(&AudioFormat, pfOutput, cbBufferSize, u32MaxEndpointNumFrames, FALSE);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Output Endpoint Initialization FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Create APO Connections
        APO_CONNECTION_DESCRIPTOR ApoConnDesc;
        IAudioMediaTypePtr pMediaType;
        CAudioMediaType::AF2AMT(AudioFormat, &pMediaType);
        //Create Connection Heap
        SLOG(eInfo1, "Creating Connection Heap");
        hr = AudioProc->CreateConnectionHeap(MFTIME1MSEC * 10, 48000, 2);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Connection Heap Creation FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }

        // Initialize Descriptor for 10 msec Stereo data (480 )
        BUILD_DESCRIPTOR(&ApoConnDesc, pMediaType, APO_CONNECTION_BUFFER_TYPE_ALLOCATED, NULL, 480);
        //Create Input Connection
        SLOG(eInfo1, "Creating Input Connection");
        hr = AudioProc->CreateConnection(&ApoConnDesc, &hAPOInputConnection);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: APO Input Connection Creation FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Create Middle Connection
        SLOG(eInfo1, "Creating Middle Connection");
        hr = AudioProc->CreateConnection(&ApoConnDesc, &hAPOMiddleConnection);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: APO Middle Connection Creation FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }

        //Create Output Connection
        SLOG(eInfo1, "Creating Output Connection");
        hr = AudioProc->CreateConnection(&ApoConnDesc, &hAPOOutputConnection);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: APO Output Connection Creation FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }

        //Initialize APO's
        //Initialize Mixer APO
        //  No Initialization for Mixer APO
        //Initialize Volume APO        
        hr = pAPOVolume->QueryInterface(__uuidof(IAudioVolume), (void**)&pVol);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Querying for IAudioVolume from IAPO for Audio Volume FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
		APO_VOLUME_PARAMS apoParams;

		apoParams.APOInit.cbSize = sizeof(apoParams);
		apoParams.APOInit.clsid  = __uuidof(AudioVolume);
		apoParams.u32SamplesPerFrame   = 2;
		apoParams.f32FrameRate		   = 48000;

        hr = pAPOVolume->Initialize( sizeof(apoParams), (LPBYTE)&apoParams); //Set it up for 2 channels, defaults will set up for 1.0 @ 48K sample rate
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Initializing Volume APO FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        SAFE_RELEASE_POINTER(pVol);
        //Make Graph Changes
        SLOG(eInfo1, "Begining Graph Changes on Processor");
        hr = AudioProc->BeginGraphChanges(&hTransaction);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: AudioProcessor BeginGraphChanges FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Add Input Conn to Mixer APO
        SLOG(eInfo1, "Adding Input Connection to Mixer APO");
        hr = AudioProc->AddAPOInputConnection(hTransaction, pAPOMixer, hAPOInputConnection, NULL);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Adding APO Input Connection FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Add Output Conn to Mixer APO
        SLOG(eInfo1, "Adding Output Connection to Mixer APO");
        hr = AudioProc->AddAPOOutputConnection(hTransaction, pAPOMixer, hAPOMiddleConnection, NULL);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Adding APO Output Connection FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Add Input Conn to Volume APO
        SLOG(eInfo1, "Adding Input Connection to Volume APO");
        hr = AudioProc->AddAPOInputConnection(hTransaction, pAPOVolume, hAPOMiddleConnection, NULL);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Adding APO Input Connection FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Add Output Conn to Volume APO
        SLOG(eInfo1, "Adding Output Connection to Volume APO");
        hr = AudioProc->AddAPOOutputConnection(hTransaction, pAPOVolume, hAPOOutputConnection, NULL);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Adding APO Output Connection FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        
        //Attach Endpoints
        //Query Input Endpoint
        hr = pInAudioEndpoint->QueryInterface(__uuidof(IAudioEndpoint), (void**)&pInEndpoint);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Querying for EndpointRT from Input Endpoint FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Query Output Endpoint
        hr = pOutAudioEndpoint->QueryInterface(__uuidof(IAudioEndpoint), (void**)&pOutEndpoint);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Querying for Endpoint from Output Endpoint FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }

        //Query Output Endpoint
        hr = pOutAudioEndpoint->QueryInterface(__uuidof(IAudioEndpointRT), (void**)&pOutEndpointRT);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Querying for EndpointRT from Output Endpoint FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }

        //Attach Input Endpoint to APO Input Connection
        SLOG(eInfo1, "Attaching Input Endpoint to Input Connection");
        hr = AudioProc->AttachInputEndpointToConnection(hTransaction, hAPOInputConnection, pInEndpoint);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Attaching Input Endpoint to APO Input Connection FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Attach Output Endpoint to APO Output Connection
        SLOG(eInfo1, "Attaching Output Endpoint to Output Connection");
        hr = AudioProc->AttachOutputEndpointToConnection(hTransaction, hAPOOutputConnection, pOutEndpoint);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Attaching Output Endpoint to APO Output Connection FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Activate APO's
        SLOG(eInfo1, "Activating Mixer APO");
        hr = AudioProc->ActivateAPO(hTransaction, pAPOMixer, NULL, APO_ACTIVATE_AFTER);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Activating Mixer APO FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        SLOG(eInfo1, "Activating Volume APO");
        hr = AudioProc->ActivateAPO(hTransaction, pAPOVolume, pAPOMixer, APO_ACTIVATE_AFTER);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Activating Volume APO FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Commit all Processor Graph changes
        SLOG(eInfo1, "Commiting Graph Changes to Processor");
        hr = AudioProc->CommitGraphChanges(hTransaction);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Commit for graph changes FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Initialize Pump 
        DWORD dwPumpPeriod = 10 * MFTIME1MSEC; //10msec		
        AudioProc.QueryInterface(__uuidof(IAudioProcessRT), &AudioProcRT);
        SLOG(eInfo1, "Initializing Audio Pump");
        hr =  AudPump->Initialize(AUDIOPUMP_ENDPOINT_OUTPUT, dwPumpPeriod, GUID_NULL, AudioProcRT, pOutEndpointRT);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Initialization of Audio Pump FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Start Processing on the Endpoint and the Pump
        MFTIME mfStartTime = 0; //Time NOW
        SLOG(eInfo1, "Starting Input Endpoint");
        hr = pInAudioEndpoint->Start();
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: FAILED to Start Input Endpoint, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        SLOG(eInfo1, "Starting Audio Pump");
        hr = AudPump->Start(&mfStartTime, NULL);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: FAILED to Start Audio Pump, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //While the Output data buffer is not written to entirely
        //Sleep
        Sleep(3000);
        MFTIME mfPadding = 0;
        AE_CURRENT_POSITION aeCurrentPosition;
        pOutEndpointRT->GetCurrentPadding(&mfPadding, &aeCurrentPosition);
        while(mfPadding == 0)
        {
            Sleep(1000);
            pOutEndpointRT->GetCurrentPadding(&mfPadding, &aeCurrentPosition);
        }
        //Stop the Pump
        SLOG(eInfo1, "Stopping Audio Pump");
        hr = AudPump->Stop();
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: FAILED to Stop Audio Pump, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Stop the Input Endpoint
        SLOG(eInfo1, "Stopping Input Endpoint");
        hr = pInAudioEndpoint->Stop();
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: FAILED to Stop Input Audio Endpoint, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Verify if we got the right data in the Output buffer.
        // This should have been just a COPY operation and the output should
        // be exactly equal to the input
        SLOG(eInfo1, "Verifying Output for Mixer APO COPY operation");
        for (UINT32 u32Frame = 0; u32Frame < u32NumFrames; u32Frame++)
        {
            if( (pfOutput[2*u32Frame + 0] != pfSineWave[2*u32Frame + 0]) ||
                (pfOutput[2*u32Frame + 1] != pfSineWave[2*u32Frame + 1]) )
            {
                SLOG(eError, "ERROR: APO Processing for Mixer FAILED for Frame Number %d", u32Frame);
                dw = FNS_FAIL;
                break;
            }
        }

        //CleanUp
        SLOG(eInfo1, "Cleaning Up");
        hr = AudioProc->BeginGraphChanges(&hTransaction);    
        hr = AudioProc->DeactivateAPO(hTransaction, pAPOMixer);
        hr = AudioProc->DetachEndpointFromConnection(hTransaction, pInEndpoint);
        hr = AudioProc->DetachEndpointFromConnection(hTransaction, pOutEndpoint);
        hr = AudioProc->RemoveAPOInputConnection(hTransaction, pAPOMixer, hAPOInputConnection);
        hr = AudioProc->RemoveAPOOutputConnection(hTransaction, pAPOMixer, hAPOMiddleConnection);
        hr = AudioProc->RemoveAPOInputConnection(hTransaction, pAPOVolume, hAPOMiddleConnection);
        hr = AudioProc->RemoveAPOOutputConnection(hTransaction, pAPOVolume, hAPOOutputConnection);
        hr = AudioProc->CommitGraphChanges(hTransaction);
        hr = AudioProc->ReleaseAPO(pAPOMixer);
        hr = AudioProc->ReleaseAPO(pAPOVolume);
        hr = AudioProc->ReleaseConnection(hAPOInputConnection);
        hr = AudioProc->ReleaseConnection(hAPOOutputConnection);
        hr = AudioProc->ReleaseConnection(hAPOMiddleConnection);
    }

    pInAudioEndpoint->Release();
    pOutAudioEndpoint->Release();

exit:
    SAFE_FREE_POINTER(pfSineWave);
    SAFE_FREE_POINTER(pfOutput);      
    return dw;
} 

//-------------------------------------------------------------------------
// Class: CAPOMixerTest
// Method: APOTestwProcCopy
//
// Description:
//  Test case to test Mixer COPY functionality with Processor
//  The following method creates two endpoints and two Mixer APO's. The Mixer APO's are
//  connected to a common Output Connection for the Mix operation
//                                             ------
//                                             |      |
//  InEndpoint1--->InConnection--->MixerAPO--->| O/p  |
//                                             | Conn |---->OutputEndpoint
//                                             |      |
//  InEndpoint2--->InConnection--->MixerAPO--->|      |
//                                             |      |
//                                              ------ 
// Parameters:
//
// Return values (HRESULT):
//     Return FNS_PASS on Success, FNS_FAIL
//
DWORD CAPOMixerTest::APOTestwProcMix()
{
    DWORD dw = FNS_PASS;
    HRESULT hr = S_OK;
    IAudioProcessorPtr AudioProc;
    IAudioPumpPtr AudPump;    
    FLOAT32* pfSineWave1 = NULL;
    FLOAT32* pfSineWave2 = NULL;
    FLOAT32* pfOutput = NULL; //Ouput data
    CComObject<CMemoryAudioEndpoint>* pInAudioEndpoint1 = new CComObject<CMemoryAudioEndpoint>();
    CComObject<CMemoryAudioEndpoint>* pInAudioEndpoint2 = new CComObject<CMemoryAudioEndpoint>();
    CComObject<CMemoryAudioEndpoint>* pOutAudioEndpoint = new CComObject<CMemoryAudioEndpoint>();
    IAudioProcessingObject* pAPOMixer1 = NULL;
    IAudioProcessingObject* pAPOMixer2 = NULL;
    IAudioProcessRTPtr AudioProcRT;
    UINT_PTR hTransaction = NULL;
    UINT_PTR hAPOInputConnection1 = NULL;
    UINT_PTR hAPOInputConnection2 = NULL;
    UINT_PTR hAPOOutputConnection = NULL;
    UINT32 u32NumFrames = 1440; //480 * 3 = 30 ms of 48K data
    WAVEFORMATEX wfx;
    CComPtr<IBasicLog> pBasicLog;
    
    wfx.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = 48000;
    wfx.wBitsPerSample = sizeof(FLOAT32) * 8;
    wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
    wfx.cbSize = 0;

    _ASSERT( (m_pMixer != NULL) && (m_pBaseAPO != NULL) );    
    _ASSERT( (pInAudioEndpoint1) && (pInAudioEndpoint2) && (pOutAudioEndpoint) );
    pInAudioEndpoint1->AddRef();
    pInAudioEndpoint2->AddRef();
    pOutAudioEndpoint->AddRef();

    {
        IAudioEndpointPtr pInEndpoint1;
        IAudioEndpointPtr pInEndpoint2;
        IAudioEndpointPtr pOutEndpoint;
        IAudioEndpointRTPtr pOutEndpointRT;

        //Audio Processor Creation
        SLOG(eInfo1, "Creating Audio Processor");
        hr = AudioProc.CreateInstance(__uuidof(AudioProcessor));
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Processor Creation FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }

        //AudioPump Creation
        SLOG(eInfo1, "Creating Audio Pump");
        hr = AudPump.CreateInstance(__uuidof(AudioPump));
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Pump Creation FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }   

        //Allocate Input and Output data memory, 30 msec 48Khz, FLOAT32 stereo i.e. 1440 frames
        UINT32 cbBufferSize = sizeof(FLOAT32) * 2 * u32NumFrames; //Stereo data.
        pfSineWave1 = (FLOAT32*) LocalAlloc(LMEM_FIXED, cbBufferSize);
        pfSineWave2 = (FLOAT32*) LocalAlloc(LMEM_FIXED, cbBufferSize);
        pfOutput = (FLOAT32*) LocalAlloc(LMEM_FIXED, cbBufferSize);

        _ASSERT(pfSineWave1 && pfSineWave2 && pfOutput);
        //Generate 10msec 1Khz FLOAT32 Stereo sine wave, routine is called from tonegen.lib
        // Both sinewave have amplitude of 0.5, so the mix should have an amplitude of 1
        SLOG(eInfo1, "Generating Input Sinewaves");
        hr = g_IShell->GetBasicLog(&pBasicLog);
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Could not get a basic logger", dw, FNS_FAIL);
     
        hr = FillBufferWithSineSignal(
            pBasicLog, XFAIL,
            &wfx,
            0.5, // amplitude
            1000.0, // frequency
            0.0, // initial phase,
            0.0, // dc
            Method_NoDithering,
            reinterpret_cast<PBYTE>(pfSineWave1),
            u32NumFrames,
            cbBufferSize
        );
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Could not fill buffer with sine", dw, FNS_FAIL);
        
        //The second sine wave is an exact copy of the first one
        CopyMemory(pfSineWave2, pfSineWave1, cbBufferSize);

        //Create and Initialize APO
        //Create AudioMixer        
        SLOG(eInfo1, "Creating 1st Mixer APO");
        hr = AudioProc->CreateAPO(__uuidof(AudioMixer), &pAPOMixer1);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Mixer APO1 Creation FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        SLOG(eInfo1, "Creating 2nd Mixer APO");
        hr = AudioProc->CreateAPO(__uuidof(AudioMixer), &pAPOMixer2);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Mixer APO2 Creation FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Initialize Endpoints
        AUDIO_FRAME_FORMAT AudioFormat;
        BUILD_FORMAT(&AudioFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 2, 4, 32, 48000);
        // Although the Endpoints have storage for 30 ms worth of data, the graph is being run at
        // 10ms, so the endpoints should never have to deal with more than 10ms of data at any time
        UINT32 u32MaxEndpointNumFrames = u32NumFrames / 3;
        // Initialize InputEndpoint1
        SLOG(eInfo1, "Initializing Input Endpoint1");
        hr = pInAudioEndpoint1->Initialize(&AudioFormat, pfSineWave1, cbBufferSize, u32MaxEndpointNumFrames, TRUE);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Input Endpoint1 Initialization FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        // Initialize InputEndpoint2
        SLOG(eInfo1, "Initializing Input Endpoint2");
        hr = pInAudioEndpoint2->Initialize(&AudioFormat, pfSineWave2, cbBufferSize, u32MaxEndpointNumFrames, TRUE);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Input Endpoint2 Initialization FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        // Initialize OutputEndpoint
        SLOG(eInfo1, "Initializing Output Endpoint");
        hr = pOutAudioEndpoint->Initialize(&AudioFormat, pfOutput, cbBufferSize, u32MaxEndpointNumFrames, FALSE);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Output Endpoint Initialization FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Create APO Connections
        APO_CONNECTION_DESCRIPTOR ApoConnDesc;
        IAudioMediaTypePtr pMediaType;
        CAudioMediaType::AF2AMT(AudioFormat, &pMediaType);
        //Create Connection Heap
        SLOG(eInfo1, "Creating Connection Heap");
        hr = AudioProc->CreateConnectionHeap(MFTIME1MSEC * 10, 48000, 2);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Connection Heap Creation FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        // Initialize Descriptor for 10 msec (480 frames)
        BUILD_DESCRIPTOR(&ApoConnDesc, pMediaType, APO_CONNECTION_BUFFER_TYPE_ALLOCATED, NULL, 480);
        //Create Input Connection1
        SLOG(eInfo1, "Creating Input Connection1");
        hr = AudioProc->CreateConnection(&ApoConnDesc, &hAPOInputConnection1);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: APO Input Connection1 Creation FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Create Input Connection2
        SLOG(eInfo1, "Creating Input Connection2");
        hr = AudioProc->CreateConnection(&ApoConnDesc, &hAPOInputConnection2);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: APO Input Connection2 Creation FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Create Output Connection
        SLOG(eInfo1, "Creating Output Connection");
        hr = AudioProc->CreateConnection(&ApoConnDesc, &hAPOOutputConnection);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: APO Output Connection Creation FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        
        //Initialize APO's

        //Make Graph Changes
        SLOG(eInfo1, "Begining Graph Changes on Processor");
        hr = AudioProc->BeginGraphChanges(&hTransaction);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: AudioProcessor BeginGraphChanges FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Add Input Conn to Mixer APO1
        SLOG(eInfo1, "Adding InputConnection1 to APOMixer1");
        hr = AudioProc->AddAPOInputConnection(hTransaction, pAPOMixer1, hAPOInputConnection1, NULL);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Adding APO InputConnection1 FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Add Output Conn to Mixer APO1
        SLOG(eInfo1, "Adding OutputConnection to APOMixer1");
        hr = AudioProc->AddAPOOutputConnection(hTransaction, pAPOMixer1, hAPOOutputConnection, NULL);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Adding APO Output Connection FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Add Input Conn to Mixer APO2
        SLOG(eInfo1, "Adding InputConnection2 to APOMixer2");
        hr = AudioProc->AddAPOInputConnection(hTransaction, pAPOMixer2, hAPOInputConnection2, NULL);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Adding APO InputConnection2 FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Add Output Conn to Mixer APO2
        SLOG(eInfo1, "Adding OutputConnection to APOMixer2");
        hr = AudioProc->AddAPOOutputConnection(hTransaction, pAPOMixer2, hAPOOutputConnection, NULL);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Adding APO Output Connection FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Attach Endpoints
        //Query Input Endpoint1
        hr = pInAudioEndpoint1->QueryInterface(__uuidof(IAudioEndpoint), (void**)&pInEndpoint1);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Querying for EndpointRT from Input Endpoint1 FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Query Input Endpoint2
        hr = pInAudioEndpoint2->QueryInterface(__uuidof(IAudioEndpoint), (void**)&pInEndpoint2);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Querying for EndpointRT from Input Endpoint2 FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Query Output Endpoint
        hr = pOutAudioEndpoint->QueryInterface(__uuidof(IAudioEndpoint), (void**)&pOutEndpoint);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Querying for Endpoint from Output Endpoint FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Query Output Endpoint
        hr = pOutAudioEndpoint->QueryInterface(__uuidof(IAudioEndpointRT), (void**)&pOutEndpointRT);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Querying for EndpointRT from Output Endpoint FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Attach Input Endpoint1 to APO Input Connection1
        SLOG(eInfo1, "Attaching InputConnection1 to InputEndpoint1");
        hr = AudioProc->AttachInputEndpointToConnection(hTransaction, hAPOInputConnection1, pInEndpoint1);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Attaching Input Endpoint1 to APO Input Connection1 FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Attach Input Endpoint2 to APO Input Connection2
        SLOG(eInfo1, "Attaching InputConnection2 to InputEndpoint2");
        hr = AudioProc->AttachInputEndpointToConnection(hTransaction, hAPOInputConnection2, pInEndpoint2);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Attaching Input Endpoint2 to APO Input Connection2 FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Attach Output Endpoint to APO Output Connection
        SLOG(eInfo1, "Attaching OutputConnection to OutputEndpoint");
        hr = AudioProc->AttachOutputEndpointToConnection(hTransaction, hAPOOutputConnection, pOutEndpoint);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Attaching Output Endpoint to APO Output Connection FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Activate APO's
        SLOG(eInfo1, "Activating 1st Mixer APO");
        hr = AudioProc->ActivateAPO(hTransaction, pAPOMixer1, NULL,APO_ACTIVATE_AFTER);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Activating Mixer APO1 FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        SLOG(eInfo1, "Activating 2nd Mixer APO");
        hr = AudioProc->ActivateAPO(hTransaction, pAPOMixer2, NULL,APO_ACTIVATE_AFTER);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Activating Mixer APO2 FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Commit all Processor Graph changes
        SLOG(eInfo1, "Commiting Graph Changes");
        hr = AudioProc->CommitGraphChanges(hTransaction);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Commit for graph changes FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Initialize Pump 
        DWORD dwPumpPeriod = 100000; //10msec
        AudioProc.QueryInterface(__uuidof(IAudioProcessRT), &AudioProcRT);
        SLOG(eInfo1, "Initializing Audio Pump");
        hr =  AudPump->Initialize(AUDIOPUMP_ENDPOINT_OUTPUT, dwPumpPeriod, GUID_NULL, AudioProcRT, pOutEndpointRT);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Initialization of Audio Pump FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }

        //Start Processing on the Endpoint and the Pump
        MFTIME mfStartTime = 0; //Time NOW
        SLOG(eInfo1, "Starting InputEndpoint1");
        hr = pInAudioEndpoint1->Start();
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: FAILED to Start Input Endpoint1, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        SLOG(eInfo1, "Starting InputEndpoint2");
        hr = pInAudioEndpoint2->Start();
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: FAILED to Start Input Endpoint2, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        SLOG(eInfo1, "Starting Audio Pump");
        hr = AudPump->Start(&mfStartTime, NULL);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: FAILED to Start Audio Pump, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //While the Output data buffer is not written to entirely
        //Sleep
        Sleep(3000);
        MFTIME mfPadding = 0;
        AE_CURRENT_POSITION aeCurrentPosition;
        pOutEndpointRT->GetCurrentPadding(&mfPadding, &aeCurrentPosition);
        while(mfPadding == 0)
        {
            Sleep(1000);
            pOutEndpointRT->GetCurrentPadding(&mfPadding, &aeCurrentPosition);
        }
        //Stop the Pump
        SLOG(eInfo1, "Stoping Audio Pump");
        hr = AudPump->Stop();
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: FAILED to Stop Audio Pump, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Stop the Input Endpoint
        SLOG(eInfo1, "Stoping Input Endpoint2");
        hr = pInAudioEndpoint2->Stop();
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: FAILED to Stop Input Audio Endpoint2, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        SLOG(eInfo1, "Stoping Input Endpoint1");
        hr = pInAudioEndpoint1->Stop();
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: FAILED to Stop Input Audio Endpoint1, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Verify if we got the right data in the Output buffer.
        // This should have been just a COPY operation and the output should
        // be exactly equal to the input
        SLOG(eInfo1, "Verifying Output for Mixer APO MIX operation");
        for (UINT32 u32Frame = 0; u32Frame < u32NumFrames; u32Frame++)
        {
            if( (pfOutput[2*u32Frame + 0] != 2 * pfSineWave1[2*u32Frame + 0] ) ||
                (pfOutput[2*u32Frame + 1] != 2 * pfSineWave1[2*u32Frame + 1]))
            {
                SLOG(eError, "ERROR: APO Processing for Mixer FAILED for Frame Number %d", u32Frame);
                dw = FNS_FAIL;
                break;
            }
        }

        //CleanUp
        SLOG(eInfo1, "Cleaning Up");
        hr = AudioProc->BeginGraphChanges(&hTransaction);    
        hr = AudioProc->DeactivateAPO(hTransaction, pAPOMixer1);
        hr = AudioProc->DeactivateAPO(hTransaction, pAPOMixer2);
        hr = AudioProc->DetachEndpointFromConnection(hTransaction, pInEndpoint1);
        hr = AudioProc->DetachEndpointFromConnection(hTransaction, pInEndpoint2);
        hr = AudioProc->DetachEndpointFromConnection(hTransaction, pOutEndpoint);
        hr = AudioProc->RemoveAPOInputConnection(hTransaction, pAPOMixer1, hAPOInputConnection1);
        hr = AudioProc->RemoveAPOInputConnection(hTransaction, pAPOMixer2, hAPOInputConnection2);
        hr = AudioProc->RemoveAPOOutputConnection(hTransaction, pAPOMixer1, hAPOOutputConnection);
        hr = AudioProc->RemoveAPOOutputConnection(hTransaction, pAPOMixer2, hAPOOutputConnection);
        hr = AudioProc->CommitGraphChanges(hTransaction);
        hr = AudioProc->ReleaseAPO(pAPOMixer1);
        hr = AudioProc->ReleaseAPO(pAPOMixer2);
        hr = AudioProc->ReleaseConnection(hAPOInputConnection1);
        hr = AudioProc->ReleaseConnection(hAPOInputConnection2);
        hr = AudioProc->ReleaseConnection(hAPOOutputConnection);
    }

    pInAudioEndpoint1->Release();
    pInAudioEndpoint2->Release();
    pOutAudioEndpoint->Release();

exit:
    SAFE_FREE_POINTER(pfSineWave1);
    SAFE_FREE_POINTER(pfSineWave2);
    SAFE_FREE_POINTER(pfOutput);    
    return dw;
}
*/

//-------------------------------------------------------------------------
// Class: CAPOMixerTest
// Method: APOTestwProcMix_Copy
//
// Description:
//  Test case to set up Mixer so that it mixes two streams, verify its behavior then remove one of the streams
//  So that the mixer is only doing a copy now.
//  Setup a local graph as follows, 
//                                                   ------
//                                                  |      |
//  InEndpoint1--->InConnection (0) --->MixerAPO--->| O/p  |
//                                                  | Conn |---->OutputEndpoint
//                                                  | (1)  |
//  InEndpoint2--->InConnection (2) --->MixerAPO--->|      |
//                                                  |      |
//                                                   ------ 
//  Setup Endpoint1 Buffer 1.5 times Endpoint2, Run the graph and verify that the mixer mixes the data. 
//  Once Endpoint2 runs out of data, Remove the second Endpoint from the graph, and signal Endpoint 1 to change its data
//  once Endpoint1 has played all data, verify the output.
// 
// Parameters:
//
// Return values (HRESULT):
//     Return FNS_PASS on Success, FNS_FAIL
//
DWORD CAPOMixerTest::APOTestwProcMix_Copy()
{
    DWORD dw = FNS_PASS;
    HRESULT hr = S_OK;
    MFTIME mfLocalPumpPeriod = 20 * MFTIME1MSEC;    
    AUDIO_FRAME_FORMAT frameFormatLocal;
    FLOAT32* pfSineWave1 = NULL;
    FLOAT32* pfSineWave2 = NULL;
    FLOAT32* pfOutput = NULL; //Ouput data
    CComObject<CMemoryAudioEndpoint>* pInMemEndpoint1 = new CComObject<CMemoryAudioEndpoint>();
    CComObject<CMemoryAudioEndpoint>* pInMemEndpoint2 = new CComObject<CMemoryAudioEndpoint>();
    CComObject<CMemoryAudioEndpoint>* pOutMemEndpoint = new CComObject<CMemoryAudioEndpoint>();
    LPVOID lpInBuffer1 = NULL, lpInBuffer2 = NULL, lpOutBuffer = NULL;    

    IAudioEndpointPtr pInEndpoint1 = NULL, pInEndpoint2 = NULL, pOutEndpoint = NULL;
    IAudioEndpointRTPtr pPumpEndpoint = NULL;

    CGraphRepresentation graph; // Local graph

    UINT32 u32NumFrames = 960; // 480 * 2, 20 ms of 48K data
    CComPtr<IBasicLog> pBasicLog;
    WAVEFORMATEXTENSIBLE wfxExt;

    _ASSERT( (m_pMixer != NULL) && (m_pBaseAPO != NULL) );    
    _ASSERT( (pInMemEndpoint1) && (pInMemEndpoint2) && (pOutMemEndpoint) );
    pInMemEndpoint1->AddRef();
    pInMemEndpoint2->AddRef();
    pOutMemEndpoint->AddRef();    

    //
    // Get local graph format
    BUILD_FORMAT(&frameFormatLocal, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 1/* mono*/, 4, 32, 48000);
    
    //
    // Local graph descriptors
    // Used apo's
    GRAPH_APO_DESCRIPTOR aApoDescriptors[] = { { APO_MIXER, NULL }, {APO_MIXER, NULL} };
	//
    // Bindings between apo's and connections
    // use only one connection
    GRAPH_BINDING_DESCRIPTOR aBindingDescriptors[] = { { 0, 0, true  }, { 0, 1, false }  //for APO 0
                                                      ,{ 1, 2, true  }, { 1, 1, false} }; //for APO 1
	//
    // Connection descriptors
    GRAPH_CONNECTION_DESCRIPTOR aConnectionDescriptors[3]; 
    ZeroMemory(aConnectionDescriptors, sizeof(aConnectionDescriptors[0])*3);
    CopyMemory(&aConnectionDescriptors[0].FrameFormat, &frameFormatLocal, sizeof(frameFormatLocal));
    CopyMemory(&aConnectionDescriptors[1].FrameFormat, &frameFormatLocal, sizeof(frameFormatLocal));
    CopyMemory(&aConnectionDescriptors[2].FrameFormat, &frameFormatLocal, sizeof(frameFormatLocal));

    //
    // Calculate connection max number of frames
    aConnectionDescriptors[0].ConnectionDescriptor.u32MaxFrameCount =  UINT32(((mfLocalPumpPeriod/MFTIME1MSEC)*frameFormatLocal.f32FramesPerSecond)/1000.00); 
    aConnectionDescriptors[1].ConnectionDescriptor.u32MaxFrameCount =  UINT32(((mfLocalPumpPeriod/MFTIME1MSEC)*frameFormatLocal.f32FramesPerSecond)/1000.00);
    aConnectionDescriptors[2].ConnectionDescriptor.u32MaxFrameCount =  UINT32(((mfLocalPumpPeriod/MFTIME1MSEC)*frameFormatLocal.f32FramesPerSecond)/1000.00); 

    GRAPH_ENDPOINT_DESCRIPTOR aEndpointDesc[3]; //3 Endpoint descriptors, 2 input 1 output
    
    hr = pInMemEndpoint1->QueryInterface(&pInEndpoint1);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Unable to QI IAudioEndpoint from MemEndpoint", dw, FNS_FAIL);
    INITIALIZE_ENDPOINT_DESCRIPTOR(aEndpointDesc[0], true, 0, aConnectionDescriptors[0].ConnectionDescriptor.u32MaxFrameCount, pInEndpoint1);

    hr = pInMemEndpoint2->QueryInterface(&pInEndpoint2);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Unable to QI IAudioEndpoint from MemEndpoint", dw, FNS_FAIL);
    INITIALIZE_ENDPOINT_DESCRIPTOR(aEndpointDesc[1], true, 2, aConnectionDescriptors[2].ConnectionDescriptor.u32MaxFrameCount, pInEndpoint2);

    hr = pOutMemEndpoint->QueryInterface(&pOutEndpoint);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Unable to QI IAudioEndpoint from MemEndpoint", dw, FNS_FAIL);
    INITIALIZE_ENDPOINT_DESCRIPTOR(aEndpointDesc[2], false, 1, aConnectionDescriptors[1].ConnectionDescriptor.u32MaxFrameCount, pOutEndpoint); 


    //Allocate Buffers for Input and Endpoints
    UINT32 u32BufferSize = aEndpointDesc[0].u32NumFrames * /* Number of Frames, same for all endpoints */
                           frameFormatLocal.u32BytesPerSampleContainer * /* Bytes per sample for the format of the Endpoint */
                           frameFormatLocal.u32SamplesPerFrame; /* Number of samples per frame for Endpoint Format */

    lpInBuffer1 = (LPVOID)LocalAlloc(LPTR, u32BufferSize);
    lpInBuffer2 = (LPVOID)LocalAlloc(LPTR, u32BufferSize);
    lpOutBuffer = (LPVOID)LocalAlloc(LPTR, u32BufferSize);
    if(NULL == lpInBuffer1 || NULL == lpInBuffer2 || NULL == lpOutBuffer)
    {
        SLOG(eError, "Memory allocation for Endpoint Buffers FAILED");
        dw = FNS_FAIL;
        goto exit;
    }

    //Initialize the Endpoints, all Endpoints have the same number of Frames.
    hr = pInMemEndpoint1->Initialize(&frameFormatLocal, lpInBuffer1, u32BufferSize, aEndpointDesc[0].u32NumFrames, true);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Unable to Initialize first Input Endpoint", dw, FNS_FAIL);
    hr = pInMemEndpoint2->Initialize(&frameFormatLocal, lpInBuffer2, u32BufferSize, aEndpointDesc[0].u32NumFrames, true);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Unable to Initialize second Input Endpoint", dw, FNS_FAIL);
    hr = pOutMemEndpoint->Initialize(&frameFormatLocal, lpOutBuffer, u32BufferSize, aEndpointDesc[0].u32NumFrames, false);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Unable to Initialize Output Endpoint", dw, FNS_FAIL);

    hr = g_IShell->GetBasicLog(&pBasicLog);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Could not get a basic logger", dw, FNS_FAIL);

    FORMAT2WAVEXT(&frameFormatLocal, &wfxExt);

    //Generate the Input PCM tone for the Input Endpoints
    for(UINT32 u = 0; u < 2; u++)
    {
        hr = FillBufferWithSineSignal(
            pBasicLog, XFAIL,
            &wfxExt.Format,
            1.0, // amplitude, this is 1.0 so that when added the o/p will cover the full dyn range
            u ? 2000 : 1000, // frequency of the PCM wave
            0.0, // initial phase,
            0.0, // dc
            Method_NoDithering,
            reinterpret_cast<PBYTE>(u ? lpInBuffer2 : lpInBuffer1),
            aEndpointDesc[0].u32NumFrames,
            u32BufferSize // bytes
        );
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Could not fill buffer", dw, FNS_FAIL);
    }
    

    //Build the Local Graph
    hr = pOutMemEndpoint->QueryInterface(&pPumpEndpoint);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Unable to QI IAudioEndpointRT from MemEndpoint", dw, FNS_FAIL);
    graph.Initialize(aConnectionDescriptors, 3,
                     aBindingDescriptors, 4,
                     aApoDescriptors, 2,
                     aEndpointDesc, 3,
                     pPumpEndpoint);

    //Build the Graph
    hr = graph.BuildGraph();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Unable to build Local Graph", dw, FNS_FAIL);        

    //The Graph is now connected 
    //Start the Endpoints

    //Start the output Endpoint
    hr = graph.StartEndpoints();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Unable to Start Output Endpoints", dw, FNS_FAIL);

    //Start the Input Endpoint
    graph.StartEndpoints(NULL, true);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Unable to Start Input Endpoints", dw, FNS_FAIL);

    //We are trying to verify MIX functionality of the mixer
    MFTIME mfStartTime = 0;
    hr = graph.Step(&mfStartTime, &mfLocalPumpPeriod);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Unable to Step through a processing cycle in the Local graph", dw, FNS_FAIL);

    //Run through each value in the output Endpoint and verify that the data has been mixed
    FLOAT32* lpIn1 = ((FLOAT32*)(lpInBuffer1));
    FLOAT32* lpIn2 = ((FLOAT32*)(lpInBuffer2));
    FLOAT32* lpOut = ((FLOAT32*)(lpOutBuffer));
    // Verify if the APO did what it was supposed to do
    for(UINT32 n = 0; n < u32NumFrames; n++)
    {
        if( 
            !CCheck::InRange(lpOut[n], (lpIn1[n] + lpIn2[n]), 0.999, 1.001)
          )  /* This is MIX operation, output should be sum*/            
        {
            SLOG(eError, "ERROR: MIX operation FAILED to behave as expected ");
            SLOG(eError, "Expected Value: %f, Actual Value: %f, Channel 0, Sample: %d", lpIn1[2*n] + lpIn2[2*n], lpOut[2*n], n);
            dw = FNS_FAIL;
            break;
        }
    }

    // Reset the Endpoints, so that Endpoints reset the positions on their internal
    // Buffers.
    pInMemEndpoint1->Reset();
    pInMemEndpoint2->Reset();
    pOutMemEndpoint->Reset();   

    // Delete the 2nd APO, so that there is only 1 in the Graph.
    // The Second Memory Endpoint is not called at all to provide Data.    
    APO_DELETE_INFO ApoDelete;
    ApoDelete.u32ApoReference = 1;
    hr = graph.DeleteFromGraph(&ApoDelete, 1);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Unable to delete an APO from the Local graph", dw, FNS_FAIL);

    //We are trying to verify COPY functionality of the mixer    
    hr = graph.Step(&mfStartTime, &mfLocalPumpPeriod);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Unable to Step through a processing cycle in the Local graph", dw, FNS_FAIL);

    lpIn1 = ((FLOAT32*)(lpInBuffer1));    
    lpOut = ((FLOAT32*)(lpOutBuffer));
    //Verify if the APO did what it was supposed to do
    for(UINT32 n = 0; n < u32NumFrames; n++)
    {
        if(
            !CCheck::InRange(lpOut[n], lpIn1[n], 0.999, 1.001)
          ) /* This is COPY operation*/            
        {
            SLOG(eError, "ERROR: COPY operation FAILED to behave as expected ");
            SLOG(eError, "Expected Value: %f, Actual Value: %f, Channel 0, Sample: %d", lpIn1[2*n], lpOut[2*n], n);            
            dw = FNS_FAIL;
            break;
        }
    }

exit:

    SAFE_FREE_POINTER(lpInBuffer1);
    SAFE_FREE_POINTER(lpInBuffer2);
    SAFE_FREE_POINTER(lpOutBuffer);

    pInMemEndpoint1->Release();
    pInMemEndpoint2->Release();
    pOutMemEndpoint->Release();

    return dw;
}
