// Copyright(C) Microsoft. All rights reserved.

#include "stdafx.h"

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOFormatFl32Int32Create
//
// Description:
//  TestCase to test if AudioFormatFl32Int32 class is registered and the necessary interfaces can be obtained from it
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOFormatFl32Int32Create()
{
    CAPOFormatFl32Int32Test APOFormatTest;
    return APOFormatTest.APOTestCreate(__uuidof(AudioFormatConvert));
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOFormatFl32Int32Process
//
// Description:
//  TestCase to test if APO AudioFormatFl32Int32 behaves as expected
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOFormatFl32Int32Process()
{    
    CAPOFormatFl32Int32Test APOFormatTest;
    return APOFormatTest.APOFormatProcess();
}

//-------------------------------------------------------------------------
// Class: CAPOFormatFl32Int32Test
// Method: APOFormatProcess
//
// Description:
//   Functional testcase for FormatFl32Int32 APO
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOFormatFl32Int32Test::APOFormatProcess()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;
    DWORD dwRet = FNS_FAIL;
    IAudioProcessingObjectRTPtr pAPORT = NULL;    
    IAudioProcessingObjectConfigurationPtr pAPOConfig = NULL;    
    FLOAT32*  pSineWave = NULL;
    FLOAT32*  pOutput = NULL;
    IAudioMediaType* pInAudioMediaType = NULL;
    IAudioMediaType* pOutAudioMediaType = NULL;
    APO_CONNECTION_PROPERTY* pInputConn = NULL;
    APO_CONNECTION_PROPERTY* pOutputConn = NULL;

    SLOG(eInfo1, "Creating FormatFl32Int32 APO");
    hr = pAPORT.CreateInstance(__uuidof(AudioFormatConvert));
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: APORT Object creation from AudioFormatFl32Int32 FAILED", dw, FNS_FAIL);
    
    SLOG(eInfo1, "Querying for IAPOConfig from FormatFl32Int32");
    hr = pAPORT.QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), &pAPOConfig);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: QueryInterface for IAPOConfig from IAPORT FAILED", dw, FNS_FAIL);
        
    //Check how the boundary conditions are handled
    // i.e. does the dynamic range of 32 bit integer map the entire dynamic range for FLOAT32 from -1.0 to 1.0
    //Test input exteremes, 0.0 and Out of bounds
    // 4 frames of input data
    FLOAT32 fData[10];
    fData[0] = 0.0; 
    fData[1] = -0.0; 
    fData[2] = 1.0; 
    fData[3] = -1.0; 
    fData[4] = 0.5; 
    fData[5] = -0.5; 
    fData[6] = (FLOAT32)4.6566e-10; // This is 1.0 / (2^31)     
    fData[7] = (FLOAT32)-4.6566e-10;     
    fData[8] = (FLOAT32)1.01;
    fData[9] = (FLOAT32)-1.01;

    INT32 OutputData[10];
    ZeroMemory(OutputData, sizeof(INT32) * 10);

    //Setup the AudioFormat for our test    
    AUDIO_FRAME_FORMAT AudioIntFormat;
    AUDIO_FRAME_FORMAT AudioFloatFormat;    
    BUILD_FORMAT(&AudioIntFormat, KSDATAFORMAT_SUBTYPE_PCM, 2, 4, 32, 48000);
    BUILD_FORMAT(&AudioFloatFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 2, 4, 32, 48000);
    //Get IAudioMediaType for the current format
    CAudioMediaType::AF2AMT(AudioFloatFormat, &pInAudioMediaType);
    CAudioMediaType::AF2AMT(AudioIntFormat, &pOutAudioMediaType);

    //Setup input and output Connections
    pInputConn = new APO_CONNECTION_PROPERTY;
    pOutputConn = new APO_CONNECTION_PROPERTY;
    _ASSERT( (pInputConn) && (pOutputConn) );
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
    //Verify the Output Values, since this goes from a 32 bit integer to a 23 bit mantissa, there
    // is a tolerance of 128
    /*if( 
        ! CCheck::InRange(OutputData[0], INT32(0), 128) ||
        ! CCheck::InRange(OutputData[1], INT32(0), 128) || 
        ! CCheck::InRange(OutputData[2], INT32(2147483647), 128) || 
        ! CCheck::InRange(OutputData[3], INT32(0x80000000), 128) || 
        ! CCheck::InRange(OutputData[4], INT32(1073741824), 128) || 
        ! CCheck::InRange(OutputData[5], INT32(-1073741824), 128) || 
        ! CCheck::InRange(OutputData[6], INT32(1), 128) || 
        ! CCheck::InRange(OutputData[7], INT32(-1), 128) ||
        ! CCheck::InRange(OutputData[8], INT32(2147483647), 128) ||
        ! CCheck::InRange(OutputData[9], INT32(0x80000000), 128)
      )*/
    if( 
        ! CCheck::InRange(OutputData[0], INT32(0), 256) ||
        ! CCheck::InRange(OutputData[1], INT32(0), 256) || 
        ! CCheck::InRange(OutputData[2], INT32(2147483647), 256) || 
        ! CCheck::InRange(OutputData[3], INT32(0x80000000), 256) || 
        ! CCheck::InRange(OutputData[4], INT32(1073741824), 256) || 
        ! CCheck::InRange(OutputData[5], INT32(-1073741824), 256) || 
        ! CCheck::InRange(OutputData[6], INT32(1), 256) || 
        ! CCheck::InRange(OutputData[7], INT32(-1), 256) ||
        ! CCheck::InRange(OutputData[8], INT32(2147483647), 256) ||
        ! CCheck::InRange(OutputData[9], INT32(0x80000000), 256)
      )
    {
        SLOG(eError, "ERROR: APO Process did not behave as expected");
        SLOG(eError, "Input Float: %f        Output 32bit: %d ", fData[0], OutputData[0]);
        SLOG(eError, "Input Float: %f        Output 32bit: %d ", fData[1], OutputData[1]);
        SLOG(eError, "Input Float: %f        Output 32bit: %d ", fData[2], OutputData[2]);
        SLOG(eError, "Input Float: %f        Output 32bit: %d ", fData[3], OutputData[3]);
        SLOG(eError, "Input Float: %f        Output 32bit: %d ", fData[4], OutputData[4]);
        SLOG(eError, "Input Float: %f        Output 32bit: %d ", fData[5], OutputData[5]);
        SLOG(eError, "Input Float: %f        Output 32bit: %d ", fData[6], OutputData[6]);
        SLOG(eError, "Input Float: %f        Output 32bit: %d ", fData[7], OutputData[7]);
        SLOG(eError, "Input Float: %f        Output 32bit: %d ", fData[8], OutputData[8]);
        SLOG(eError, "Input Float: %f        Output 32bit: %d ", fData[9], OutputData[9]);
        dw = FNS_FAIL;            
    }

    //Start with a FLOAT32 SineWave, Convert it to INT32 and reconvert it to FLOAT32
    // The output should be exactly equal to the input
    //Allocate Source and destination buffers
    pSineWave = (FLOAT32*)LocalAlloc(LMEM_FIXED, sizeof(FLOAT32) * 480);
    pOutput = (FLOAT32*)LocalAlloc(LMEM_FIXED, sizeof(FLOAT32) * 480);
    _ASSERT(pSineWave && pOutput);

    ZeroMemory(pOutput, sizeof(FLOAT32) * 480);
    // Mono 48K.
    BUILD_FORMAT(&AudioIntFormat, KSDATAFORMAT_SUBTYPE_PCM, 1, 4, 32, 48000);
    BUILD_FORMAT(&AudioFloatFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 1, 4, 32, 48000);
    // Call Fl32Int32 collect the data in a temp buffer, then call Fl32Int32 and collect the data in the 
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

    //At this point the sinewave has gone through a complete roundtrip from  to FLOAT32 to INT32 to FLOAT32
    //Verify if the Source and the final destination are the same within the small tolerance limit
    for (UINT32 i = 0; i < 480; i++)
    {        
        //if(! CCheck::InRange(pSineWave[i], pOutput[i], 128))
        if(! CCheck::InRange(pSineWave[i], pOutput[i], 256))
        {            
            SLOG(eError, "ERROR: APO Process did not behave as expected");
            SLOG(eError, "Source Value: %f            Dest Value: %f,    Frame no: %d", pSineWave[i], pOutput[i], i);
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
