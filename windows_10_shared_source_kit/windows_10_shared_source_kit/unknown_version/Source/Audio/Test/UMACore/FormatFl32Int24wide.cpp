// Copyright(C) Microsoft. All rights reserved.

#include "stdafx.h"

// SOURCE FILE FOR 24bit wide Format Converters
// 24 bit wide data is 24 bit data in 32 bit containers (data is contained in the higher bits)


//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOFormatFl32Int24wideCreate
//
// Description:
//  TestCase to test if AudioFormatFl32Int24wide class is registered and the necessary interfaces can be obtained from it
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOFormatFl32Int24wideCreate()
{
    CAPOFormatFl32Int24wideTest APOFormatTest;
    return APOFormatTest.APOTestCreate(__uuidof(AudioFormatConvert));
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOFormatFl32Int24wideProcess
//
// Description:
//  TestCase to test if APO AudioFormatFl32Int24wide behaves as expected
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOFormatFl32Int24wideProcess()
{    
    CAPOFormatFl32Int24wideTest APOFormatTest;
    return APOFormatTest.APOFormatProcess();
}

//-------------------------------------------------------------------------
// Class: CAPOFormatFl32Int24wideTest
// Method: APOFormatProcess
//
// Description:
//   Functional testcase for FormatFl32Int24wide APO
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOFormatFl32Int24wideTest::APOFormatProcess()
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
    FLOAT32*  pSineWave = NULL;
    FLOAT32*  pOutput = NULL;    
    const UINT BYTE_SHIFT_COUNT = 8;

    SLOG(eInfo1, "Creating FormatFl32Int24wide APO");
    hr = pAPORT.CreateInstance(__uuidof(AudioFormatConvert));
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: APORT Object creation from AudioFormatFl32Int24wide FAILED", dw, FNS_FAIL);
    
    SLOG(eInfo1, "Querying for IAPOConfig from FormatFl32Int24wide");
    hr = pAPORT.QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), &pAPOConfig);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: QueryInterface for IAPOConfig from IAPORT FAILED", dw, FNS_FAIL);
        
    //Check how the boundary conditions are handled
    // i.e. does the dynamic range of 24bit map the entire dynamic range for FLOAT32 from -1.0 to 1.0
    //Test input exteremes, 0.0 and Out of bounds
    // 4 frames of input data
    FLOAT32 fData[8];
    fData[0] = 0.0; 
    fData[1] = -0.0; 
    fData[2] = (FLOAT32)(1.0-1.2e-7);  // clip it
    fData[3] = -1.0; 
    fData[4] = 0.5; 
    fData[5] = -0.5; 
    //fData[6] = (FLOAT32)1.1920e-7; 
    //fData[7] = (FLOAT32)-1.1920e-7; 
    fData[6] = (FLOAT32)1.2e-7; 
    fData[7] = (FLOAT32)-1.2e-7; 

    INT32 OutputData[8]; //24 bit data is contained in 32 bit containers
    ZeroMemory(OutputData, sizeof(INT32) * 8);

    //Setup the AudioFormat for our test    
    AUDIO_FRAME_FORMAT AudioIntFormat;
    AUDIO_FRAME_FORMAT AudioFloatFormat;    
    BUILD_FORMAT(&AudioIntFormat, KSDATAFORMAT_SUBTYPE_PCM, 2, 4, 24, 48000);
    BUILD_FORMAT(&AudioFloatFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 2, 4, 32, 48000);
    //Get IAudioMediaType for the current format    
    CAudioMediaType::AF2AMT(AudioFloatFormat, &pInAudioMediaType);
    CAudioMediaType::AF2AMT(AudioIntFormat, &pOutAudioMediaType);

    //Setup input and output Connections
    pInputConn = new APO_CONNECTION_PROPERTY;
    pOutputConn = new APO_CONNECTION_PROPERTY;
    _ASSERT( (pInputConn) && (pOutputConn) );
    BUILD_CONNECTION(pInputConn, fData, 4, 0, BUFFER_VALID);
    BUILD_CONNECTION(pOutputConn, OutputData, 4, 0, BUFFER_SILENT);

    SLOG(eInfo1, "Locking APO for Processing");
    hr = LockAPO(pAPORT, 1, pInAudioMediaType, 1, pOutAudioMediaType);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Lock for Process on the APO FAILED", dw, FNS_FAIL);
    
    SLOG(eInfo1, "Calling APO Process");
    pAPORT->APOProcess(1, &pInputConn, 1, &pOutputConn);
    SLOG(eInfo1, "UnLocking APO");
    hr = pAPOConfig->UnlockForProcess();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: UnLock for Process on the APO FAILED", dw, FNS_FAIL);
    
    //Verify the Output Values
    INT32 iInputData[8];
    iInputData[0] = 0 << BYTE_SHIFT_COUNT;
    iInputData[1] = -0 << BYTE_SHIFT_COUNT;   
    iInputData[2] = 8388607 << BYTE_SHIFT_COUNT;
    iInputData[3] = -8388608 << BYTE_SHIFT_COUNT;
    iInputData[4] = 4194304 << BYTE_SHIFT_COUNT;
    iInputData[5] = -4194304 << BYTE_SHIFT_COUNT;
    iInputData[6] = 1 << BYTE_SHIFT_COUNT;
    iInputData[7] = -1 << BYTE_SHIFT_COUNT;
    int temp;
    if( ( OutputData[0] != iInputData[0]) ||
        ( OutputData[1] != iInputData[1])||
        ( OutputData[2] != iInputData[2])||
        ( OutputData[3] != iInputData[3])||
        ( OutputData[4] != iInputData[4])||
        ( OutputData[5] != iInputData[5])||
        ( OutputData[6] != iInputData[6])||
        ( OutputData[7] != iInputData[7])        
      )
    {
        SLOG(eError, "ERROR: APO Process did not behave as expected");
        SLOG(eError, "Input Float: %e        Output 24bit: %d ", fData[0], OutputData[0] >> BYTE_SHIFT_COUNT);
        SLOG(eError, "Input Float: %e        Output 24bit: %d ", fData[1], OutputData[1] >> BYTE_SHIFT_COUNT);
        SLOG(eError, "Input Float: %e        Output 24bit: %d ", fData[2], OutputData[2] >> BYTE_SHIFT_COUNT);
        SLOG(eError, "Input Float: %e        Output 24bit: %d ", fData[3], OutputData[3] >> BYTE_SHIFT_COUNT);
        SLOG(eError, "Input Float: %e        Output 24bit: %d ", fData[4], OutputData[4] >> BYTE_SHIFT_COUNT);
        SLOG(eError, "Input Float: %e        Output 24bit: %d ", fData[5], OutputData[5] >> BYTE_SHIFT_COUNT);
        SLOG(eError, "Input Float: %e        Output 24bit: %d ", fData[6], OutputData[6] >> BYTE_SHIFT_COUNT);
        SLOG(eError, "Input Float: %e       Output 24bit: %d ", fData[7], OutputData[7] >> BYTE_SHIFT_COUNT);
        //SLOG(eError, "Input Float: %f        Output 24bit: %d ", fData[7], convert_from_24bit(temp, OutputData[7]));
        dw = FNS_FAIL;
        goto exit;
    }

    //Start with a FLOAT32 SineWave, Convert it to INT24wide and reconvert it to FLOAT32
    // The output should be exactly equal to the input
    //Allocate Source and destination buffers
    pSineWave = (FLOAT32*)LocalAlloc(LMEM_FIXED, sizeof(FLOAT32) * 480);
    pOutput = (FLOAT32*)LocalAlloc(LMEM_FIXED, sizeof(FLOAT32) * 480);
    _ASSERT(pSineWave && pOutput);

    ZeroMemory(pOutput, sizeof(FLOAT32) * 480);
    // Mono 48K.
    BUILD_FORMAT(&AudioIntFormat, KSDATAFORMAT_SUBTYPE_PCM, 1, 4, 24, 48000);
    BUILD_FORMAT(&AudioFloatFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 1, 4, 32, 48000);
    // Call Fl32Int24wide collect the data in a temp buffer, then call Fl32Int24wide and collect the data in the 
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

    //At this point the sinewave has gone through a complete roundtrip from  to FLOAT32 to 24bitwide INT to FLOAT32
    //Verify if the Source and the final destination are the same
    for (UINT32 i = 0; i < 480; i++)
    {        
        //if(! CCheck::InRange(pSineWave[i], pOutput[i],(FLOAT32)0.5/(0x7FFFFF)))
        if(! CCheck::InRange(pSineWave[i], pOutput[i],(FLOAT32)0.00001))
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
