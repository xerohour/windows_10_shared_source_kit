// Copyright(C) Microsoft. All rights reserved.

#include "stdafx.h"

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOFormatInt32Fl64Create
//
// Description:
//  TestCase to test if AudioFormatInt32Fl64 class is registered and the necessary interfaces can be obtained from it
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOFormatInt32Fl64Create()
{
    CAPOFormatInt32Fl64Test APOFormatTest;
    return APOFormatTest.APOTestCreate(__uuidof(AudioFormatConvert));
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOFormatInt32Fl64Process
//
// Description:
//  TestCase to test if APO AudioFormatInt32Fl64 behaves as expected
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOFormatInt32Fl64Process()
{    
    CAPOFormatInt32Fl64Test APOFormatTest;
    return APOFormatTest.APOFormatProcess();
}

//-------------------------------------------------------------------------
// Class: CAPOFormatInt32Fl64Test
// Method: APOFormatProcess
//
// Description:
//   Functional testcase for FormatInt32Fl64 APO
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOFormatInt32Fl64Test::APOFormatProcess()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;
    DWORD dwRet = FNS_FAIL;
    IAudioProcessingObjectRTPtr pAPORT = NULL;    
    IAudioProcessingObjectConfigurationPtr pAPOConfig = NULL;
    IAudioMediaType* pInAudioMediaType = NULL;
    IAudioMediaType* pOutAudioMediaType = NULL;
    APO_CONNECTION_PROPERTY* pInputConn = NULL;
    APO_CONNECTION_PROPERTY* pOutputConn = NULL;
    INT32* pSineWave = NULL;
    INT32* pOutput = NULL;

    SLOG(eInfo1, "Creating FormatInt32Fl64 APO");
    hr = pAPORT.CreateInstance(__uuidof(AudioFormatConvert));
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: APORT Object creation from AudioFormatInt32Fl64 FAILED, 0x%X", hr);
        dw = FNS_FAIL;
        goto exit;
    }
    SLOG(eInfo1, "Querying for IAPOConfig from FormatInt32Fl64");
    hr = pAPORT.QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), &pAPOConfig);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: QueryInterface for IAPOConfig from IAPORT FAILED, 0x%X", hr);
        dw = FNS_FAIL;
        goto exit;
    }    
    //Check how the boundary conditions are handled    
    //Test input exteremes, 0.0 and Out of bounds
    // 4 frames of input data
    INT32 iData[8];
    iData[0] = 0; 
    iData[1] = -0; 
    iData[2] = 2147483647; 
    iData[3] = 0x80000000;//-2147483648; 
    iData[4] = 1073741824; 
    iData[5] = -1073741824; 
    iData[6] = 1; 
    iData[7] = -1;     

    FLOAT64 OutputData[8];
    ZeroMemory(OutputData, sizeof(FLOAT64) * 8);

    //Setup the AudioFormat for our test    
    AUDIO_FRAME_FORMAT AudioIntFormat;
    AUDIO_FRAME_FORMAT AudioFloatFormat;    
    BUILD_FORMAT(&AudioIntFormat, KSDATAFORMAT_SUBTYPE_PCM, 2, 4, 32, 48000);
    BUILD_FORMAT(&AudioFloatFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 2, 8, 64, 48000);
    //Get IAudioMediaType for the current format    
    CAudioMediaType::AF2AMT(AudioIntFormat, &pInAudioMediaType);
    CAudioMediaType::AF2AMT(AudioFloatFormat, &pOutAudioMediaType);

    //Setup input and output Connections
    pInputConn = new APO_CONNECTION_PROPERTY;
    pOutputConn = new APO_CONNECTION_PROPERTY;
    _ASSERT( (pInputConn) && (pOutputConn) );
    BUILD_CONNECTION(pInputConn, iData, 4, 0, BUFFER_VALID);
    BUILD_CONNECTION(pOutputConn, OutputData, 4, 0, BUFFER_SILENT);

    SLOG(eInfo1, "Locking APO for Processing");
    hr = LockAPO(pAPORT, 1, pInAudioMediaType, 1, pOutAudioMediaType);
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
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: UnLock for Process on the APO FAILED, 0x%X", hr);
        dw = FNS_FAIL;
        goto exit;
    }
    //Verify the Output Values    
    if( 
        ! CCheck::InRange(OutputData[0], FLOAT64(0.0),0.999, 1.001) ||
        ! CCheck::InRange(OutputData[1], FLOAT64(0.0),0.999, 1.001) || 
        ! CCheck::InRange(OutputData[2], FLOAT64(1.0),0.999, 1.001) || 
        ! CCheck::InRange(OutputData[3], FLOAT64(-1.0),0.999, 1.001) || 
        ! CCheck::InRange(OutputData[4], FLOAT64(0.5),0.999, 1.001) || 
        ! CCheck::InRange(OutputData[5], FLOAT64(-0.5),0.999, 1.001) || 
        ! CCheck::InRange(OutputData[6], FLOAT64(4.65661e-10),0.999, 1.001) || 
        ! CCheck::InRange(OutputData[7], FLOAT64(-4.65661e-10),0.999, 1.001)
      )
    {
        SLOG(eError, "ERROR: APO Process did not behave as expected");
        SLOG(eError, "Input 32bit: %d        Output Float: %f ", iData[0], OutputData[0]);
        SLOG(eError, "Input 32bit: %d        Output Float: %f ", iData[1], OutputData[1]);
        SLOG(eError, "Input 32bit: %d        Output Float: %f ", iData[2], OutputData[2]);
        SLOG(eError, "Input 32bit: %d        Output Float: %f ", iData[3], OutputData[3]);
        SLOG(eError, "Input 32bit: %d        Output Float: %f ", iData[4], OutputData[4]);
        SLOG(eError, "Input 32bit: %d        Output Float: %f ", iData[5], OutputData[5]);
        SLOG(eError, "Input 32bit: %d        Output Float: %e ", iData[6], OutputData[6]);
        SLOG(eError, "Input 32bit: %d        Output Float: %e ", iData[7], OutputData[7]);
        dw = FNS_FAIL;            
    }

    //Start with a INT32 data, Convert it to FLOAT64 and reconvert it to INT32
    // Check the output sine data against the input data.
    pSineWave = (INT32*)LocalAlloc(LMEM_FIXED, sizeof(INT32) * 480);
    pOutput = (INT32*)LocalAlloc(LMEM_FIXED, sizeof(INT32) * 480);
    _ASSERT(pSineWave && pOutput);

    ZeroMemory(pOutput, sizeof(INT32) * 480);
    // Mono 48K.
    BUILD_FORMAT(&AudioIntFormat, KSDATAFORMAT_SUBTYPE_PCM, 1, 4, 32, 48000);
    BUILD_FORMAT(&AudioFloatFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 1, 8, 64, 48000);
    // Call Int32Fl64 collect the data in a temp buffer, then call Fl64Int32 and collect the data in the 
    // Output buffer provided.
    dwRet = APOTestProcess( pSineWave, //Source data buffer
                         AudioIntFormat, //Source format
                         __uuidof(AudioFormatConvert), //Source CLSID
                         pOutput,  //destination buffer
                         AudioFloatFormat,  // Intermediate data format
                         __uuidof(AudioFormatConvert) // Intermediate APO CLSID
                         );    
    if(FNS_PASS != dwRet)
    {
        SLOG(eError, "ERROR: APOTestProcess FAILED, 0x%X", dw);            
        dw = FNS_FAIL;
        goto exit;
    }

    //At this point the sinewave has gone through a complete roundtrip from  to FLOAT64 to INT32
    //Verify if the Source and the final destination are the same (with a tolerance of +/- 128)
    for (UINT32 i = 0; i < 480; i++)
    {
        if(! CCheck::InRange(pOutput[i], pSineWave[i], 128))
        {
            SLOG(eError, "ERROR: APO Process did not behave as expected");
            SLOG(eError, "Source Value: %d            Dest Value: %d,    Frame no: %d", pSineWave[i], pOutput[i], i);
            dw = FNS_FAIL;
        }
    }
  

exit:
    SAFE_DELETE_POINTER(pInputConn);
    SAFE_DELETE_POINTER(pOutputConn);
    SAFE_RELEASE_POINTER(pInAudioMediaType);
    SAFE_RELEASE_POINTER(pOutAudioMediaType);
    SAFE_FREE_POINTER(pSineWave);
    SAFE_FREE_POINTER(pOutput);
    return dw;

}
