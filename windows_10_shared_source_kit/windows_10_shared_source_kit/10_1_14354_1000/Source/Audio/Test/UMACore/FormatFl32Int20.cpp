// Copyright(C) Microsoft. All rights reserved.

#include "stdafx.h"

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOFormatFl32Int20Create
//
// Description:
//  TestCase to test if AudioFormatFl32Int20 class is registered and the necessary interfaces can be obtained from it
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOFormatFl32Int20Create()
{
    CAPOFormatFl32Int20Test APOFormatTest;
    return APOFormatTest.APOTestCreate(__uuidof(AudioFormatConvert));
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOFormatFl32Int20Process
//
// Description:
//  TestCase to test if APO AudioFormatFl32Int20 behaves as expected
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOFormatFl32Int20Process()
{    
    CAPOFormatFl32Int20Test APOFormatTest;
    return APOFormatTest.APOFormatProcess();
}

//-------------------------------------------------------------------------
// Class: CAPOFormatFl32Int20Test
// Method: APOFormatProcess
//
// Description:
//   Functional testcase for FormatFl32Int20 APO
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOFormatFl32Int20Test::APOFormatProcess()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;
    DWORD dwRet = FNS_FAIL;
    IAudioProcessingObjectRTPtr pAPORT = NULL;    
    IAudioProcessingObjectConfigurationPtr pAPOConfig = NULL;    
    CComPtr<IAudioDither> pAPODither = NULL;
    FLOAT32*  pSineWave = NULL;
    FLOAT32*  pOutput = NULL;
    IAudioMediaType* pInAudioMediaType = NULL;
    IAudioMediaType* pOutAudioMediaType = NULL;
    APO_CONNECTION_PROPERTY* pInputConn = NULL;
    APO_CONNECTION_PROPERTY* pOutputConn = NULL;
    const UINT32 TWENTY_BIT_ALIGN_SHIFT = 12;
    DITHER_STATE DitState = DITHER_NONE;

    SLOG(eInfo1, "Creating FormatFl32Int20 APO");
    hr = pAPORT.CreateInstance(__uuidof(AudioFormatConvert));
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: APORT Object creation from AudioFormatFl32Int20 FAILED", dw, FNS_FAIL);
    
    SLOG(eInfo1, "Querying for IAPOConfig from FormatFl32Int20");
    hr = pAPORT.QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), &pAPOConfig);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: QueryInterface for IAPOConfig from IAPORT FAILED", dw, FNS_FAIL);

    SLOG(eInfo1, "Querying for IAudioDither from FormatFl32Int16");
    hr = pAPORT.QueryInterface(__uuidof(IAudioDither), &pAPODither);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: QueryInterface for IAudioDither from IAPORT FAILED", dw, FNS_FAIL);

    //For each dither state, run the tests
    for(UINT ii = 0; ii < 3; ii++)
    {
        //Check how the boundary conditions are handled
        // i.e. does the dynamic range of 32 bit integer map the entire dynamic range for FLOAT32 from -1.0 to 1.0
        //Test input exteremes, 0.0 and Out of bounds
        // 4 frames of input data
        FLOAT32 fData[10];
        fData[0] = 0.0; 
        fData[1] = -0.0; 
        fData[2] = (FLOAT32)(1.0-1.91e-6); 
        fData[3] = -1.0; 
        fData[4] = 0.5; 
        fData[5] = -0.5; 
        //fData[6] = (FLOAT32)1.90734e-6; // This is 1.0 / (2^19)  
        //fData[7] = (FLOAT32)-1.90734e-6; 
        fData[6] = (FLOAT32)1.91e-6; // This is 1.0 / (2^19)  
        fData[7] = (FLOAT32)-1.91e-6; 
        fData[8] = (FLOAT32)(1.0-1.91e-6);
        fData[9] = (FLOAT32)-1.0;

        INT32 OutputData[10];
        ZeroMemory(OutputData, sizeof(INT32) * 10);

        //Setup the AudioFormat for our test    
        AUDIO_FRAME_FORMAT AudioIntFormat;
        AUDIO_FRAME_FORMAT AudioFloatFormat;    
        BUILD_FORMAT(&AudioIntFormat, KSDATAFORMAT_SUBTYPE_PCM, 2, 4, 20, 48000);
        BUILD_FORMAT(&AudioFloatFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 2, 4, 32, 48000);
        //Get IAudioMediaType for the current format
        SAFE_RELEASE_POINTER(pInAudioMediaType);
        SAFE_RELEASE_POINTER(pOutAudioMediaType);
        CAudioMediaType::AF2AMT(AudioFloatFormat, &pInAudioMediaType);
        CAudioMediaType::AF2AMT(AudioIntFormat, &pOutAudioMediaType);

        //Setup input and output Connections
        SAFE_DELETE_POINTER(pInputConn);
        pInputConn = new APO_CONNECTION_PROPERTY;
        SAFE_DELETE_POINTER(pOutputConn);
        pOutputConn = new APO_CONNECTION_PROPERTY;
        if((NULL == pInputConn) || (NULL == pOutputConn))
        {
            SLOG(eError, "ERROR: Memory Allocation failure");
            dw = FNS_FAIL;
            goto exit;
        }

        BUILD_CONNECTION(pInputConn, fData, 5, 0, BUFFER_VALID);
        BUILD_CONNECTION(pOutputConn, OutputData, 5, 0, BUFFER_SILENT);

        SLOG(eInfo1, "Locking APO for Processing");
        hr = LockAPO(pAPORT, 1, pInAudioMediaType, 1, pOutAudioMediaType);
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "", dw, FNS_FAIL);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Lock for Process on the APO FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        SLOG(eInfo1, "Calling APO Process");
        pAPORT->APOProcess(1, &pInputConn, 1, &pOutputConn);
        SLOG(eInfo1, "UnLocking APO");
        hr = pAPOConfig->UnlockForProcess();
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "", dw, FNS_FAIL);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: UnLock for Process on the APO FAILED, 0x%X", hr);
            dw = FNS_FAIL;
            goto exit;
        }
        //Verify the Output Values
        if( 
            ! CCheck::InRange(OutputData[0], INT32(0) << TWENTY_BIT_ALIGN_SHIFT, 128) ||
            ! CCheck::InRange(OutputData[1], INT32(0) << TWENTY_BIT_ALIGN_SHIFT, 128) || 
            ! CCheck::InRange(OutputData[2], INT32(524287) << TWENTY_BIT_ALIGN_SHIFT, 128) || 
            ! CCheck::InRange(OutputData[3], INT32(-524288) << TWENTY_BIT_ALIGN_SHIFT, 128) || 
            ! CCheck::InRange(OutputData[4], INT32(262144) << TWENTY_BIT_ALIGN_SHIFT, 128) || 
            ! CCheck::InRange(OutputData[5], INT32(-262144) << TWENTY_BIT_ALIGN_SHIFT, 128) || 
            ! CCheck::InRange(OutputData[6], INT32(1) << TWENTY_BIT_ALIGN_SHIFT, 128) || 
            ! CCheck::InRange(OutputData[7], INT32(-1) << TWENTY_BIT_ALIGN_SHIFT, 128) ||
            ! CCheck::InRange(OutputData[8], INT32(524287) << TWENTY_BIT_ALIGN_SHIFT, 128) ||
            ! CCheck::InRange(OutputData[9], INT32(-524288) << TWENTY_BIT_ALIGN_SHIFT, 128)
            )
        {
            SLOG(eError, "ERROR: APO Process did not behave as expected");
            SLOG(eError, "Input Float: %f        Output 20bit: %d ", fData[0], OutputData[0] >> TWENTY_BIT_ALIGN_SHIFT);
            SLOG(eError, "Input Float: %f        Output 20bit: %d ", fData[1], OutputData[1] >> TWENTY_BIT_ALIGN_SHIFT);
            SLOG(eError, "Input Float: %f        Output 20bit: %d ", fData[2], OutputData[2] >> TWENTY_BIT_ALIGN_SHIFT);
            SLOG(eError, "Input Float: %f        Output 20bit: %d ", fData[3], OutputData[3] >> TWENTY_BIT_ALIGN_SHIFT);
            SLOG(eError, "Input Float: %f        Output 20bit: %d ", fData[4], OutputData[4] >> TWENTY_BIT_ALIGN_SHIFT);
            SLOG(eError, "Input Float: %f        Output 20bit: %d ", fData[5], OutputData[5] >> TWENTY_BIT_ALIGN_SHIFT);
            SLOG(eError, "Input Float: %f        Output 20bit: %d ", fData[6], OutputData[6] >> TWENTY_BIT_ALIGN_SHIFT);
            SLOG(eError, "Input Float: %f        Output 20bit: %d ", fData[7], OutputData[7] >> TWENTY_BIT_ALIGN_SHIFT);
            SLOG(eError, "Input Float: %f        Output 20bit: %d ", fData[8], OutputData[8] >> TWENTY_BIT_ALIGN_SHIFT);
            SLOG(eError, "Input Float: %f        Output 20bit: %d ", fData[9], OutputData[9] >> TWENTY_BIT_ALIGN_SHIFT);
            dw = FNS_FAIL;
        }

        //Start with a FLOAT32 SineWave, Convert it to INT20 and reconvert it to FLOAT32
        // The output should be exactly equal to the input
        //Allocate Source and destination buffers
        SAFE_FREE_POINTER(pSineWave);
        SAFE_FREE_POINTER(pOutput);
        pSineWave = (FLOAT32*)LocalAlloc(LMEM_FIXED, sizeof(FLOAT32) * 480);
        pOutput = (FLOAT32*)LocalAlloc(LMEM_FIXED, sizeof(FLOAT32) * 480);
        //_ASSERT(pSineWave && pOutput);
        if((NULL == pSineWave) || (NULL == pOutput))
        {
            SLOG(eError, "ERROR: Memory Allocation failure");
            dw = FNS_FAIL;
            goto exit;
        }

        ZeroMemory(pOutput, sizeof(FLOAT32) * 480);
        // Mono 48K.
        BUILD_FORMAT(&AudioIntFormat, KSDATAFORMAT_SUBTYPE_PCM, 1, 4, 20, 48000);
        BUILD_FORMAT(&AudioFloatFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 1, 4, 32, 48000);
        // Call Fl32Int20 collect the data in a temp buffer, then call Fl32Int20 and collect the data in the 
        // Output buffer provided.
        dwRet = APOTestProcess( pSineWave, //Source data buffer
            AudioFloatFormat, //Source format
            __uuidof(AudioFormatConvert), //Source CLSID
            pOutput,  //destination buffer
            AudioIntFormat,  // Intermediate data format
            __uuidof(AudioFormatConvert), // Intermediate APO CLSID
            KSDATAFORMAT_SUBTYPE_IEEE_FLOAT //Source Data type
            );    
        if(FNS_PASS != dwRet)
        {
            SLOG(eError, "/ERROR: APOTestProcess FAILED, 0x%X", dw);            
            dw = FNS_FAIL;
            goto exit;
        }

        //At this point the sinewave has gone through a complete roundtrip from  to FLOAT32 to INT20 to FLOAT32
        //Verify if the Source and the final destination are the same within the small tolerance limit
        for (UINT32 i = 0; i < 480; i++)
        {        
            //if(! CCheck::InRange(pSineWave[i], pOutput[i], FLOAT32(0.5/524287)))
            if(! CCheck::InRange(pSineWave[i], pOutput[i], (FLOAT32)0.00001))
            {            
                SLOG(eError, "ERROR: APO Process did not behave as expected");
                SLOG(eError, "Source Value: %f            Dest Value: %f,    Frame no: %d", pSineWave[i], pOutput[i], i);
                dw = FNS_FAIL;
            }
        }
    }//for(UINT ii = 0; ii < 3; ii++) For each dither type

exit:
    SAFE_DELETE_POINTER(pInputConn);
    SAFE_DELETE_POINTER(pOutputConn);
    SAFE_RELEASE_POINTER(pInAudioMediaType);
    SAFE_RELEASE_POINTER(pOutAudioMediaType);
    SAFE_FREE_POINTER(pSineWave);
    SAFE_FREE_POINTER(pOutput);
    return dw;
}
