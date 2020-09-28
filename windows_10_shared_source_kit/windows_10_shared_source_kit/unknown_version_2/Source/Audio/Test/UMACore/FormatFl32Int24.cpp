// Copyright(C) Microsoft. All rights reserved.

#include "stdafx.h"

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOFormatFl32Int24Create
//
// Description:
//  TestCase to test if AudioFormatFl32Int24 class is registered and the necessary interfaces can be obtained from it
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOFormatFl32Int24Create()
{
    CAPOFormatFl32Int24Test APOFormatTest;
    return APOFormatTest.APOTestCreate(__uuidof(AudioFormatConvert));
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOFormatFl32Int24Process
//
// Description:
//  TestCase to test if APO AudioFormatFl32Int24 behaves as expected
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOFormatFl32Int24Process()
{    
    CAPOFormatFl32Int24Test APOFormatTest;
    return APOFormatTest.APOFormatProcess();
}

//-------------------------------------------------------------------------
// Class: CAPOFormatFl32Int24Test
// Method: APOFormatProcess
//
// Description:
//   Functional testcase for FormatFl32Int24 APO
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOFormatFl32Int24Test::APOFormatProcess()
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
    pair_of_numbers<s24Bit, int> Pair(0); 

    SLOG(eInfo1, "Creating FormatFl32Int24 APO");
    hr = pAPORT.CreateInstance(__uuidof(AudioFormatConvert));
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: APORT Object creation from AudioFormatFl32Int24 FAILED", dw, FNS_FAIL);
    
    SLOG(eInfo1, "Querying for IAPOConfig from FormatFl32Int24");
    hr = pAPORT.QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), &pAPOConfig);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: QueryInterface for IAPOConfig from IAPORT FAILED", dw, FNS_FAIL);
        
    //Check how the boundary conditions are handled
    // i.e. does the dynamic range of 24bit map the entire dynamic range for FLOAT32 from -1.0 to 1.0
    //Test input exteremes, 0.0 and Out of bounds
    // 4 frames of input data
    FLOAT32 fData[8];
    fData[0] =  0.0; 
    fData[1] = -0.0; 
    fData[2] =  (FLOAT32)(1.0 - 1.2e-7);  // clip it
    fData[3] = -1.0; 
    fData[4] =  0.5; 
    fData[5] = -0.5; 
    //fData[6] = (FLOAT32)1.1920e-7; 
    //fData[7] = (FLOAT32)-1.1920e-7; 
    fData[6] = (FLOAT32)1.2e-7; 
    fData[7] = (FLOAT32)-1.2e-7; 

    s24Bit OutputData[8];
    ZeroMemory(OutputData, sizeof(s24Bit) * 8);

    //Setup the AudioFormat for our test    
    AUDIO_FRAME_FORMAT AudioIntFormat;
    AUDIO_FRAME_FORMAT AudioFloatFormat;    
    BUILD_FORMAT(&AudioIntFormat, KSDATAFORMAT_SUBTYPE_PCM, 2, 3, 24, 48000);
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
    s24Bit iInputData[8];
    Pair.Initialize((int)0);
    iInputData[0] = Pair;
    Pair.Initialize((int)-0);
    iInputData[1] = Pair;    
    Pair.Initialize((int)8388607);
    iInputData[2] = Pair;
    Pair.Initialize((int)-8388608);
    iInputData[3] = Pair;
    Pair.Initialize((int)4194304);
    iInputData[4] = Pair;
    Pair.Initialize((int)-4194304);
    iInputData[5] = Pair;    
    Pair.Initialize((int)1);
    iInputData[6] = Pair;
    Pair.Initialize((int)-1);
    iInputData[7] = Pair;
    int temp;
    if( ( memcmp(&OutputData[0],&iInputData[0], 3) != 0)||
        ( memcmp(&OutputData[1],&iInputData[1], 3) != 0)||
        ( memcmp(&OutputData[2],&iInputData[2], 3) != 0)||
        ( memcmp(&OutputData[3],&iInputData[3], 3) != 0)||
        ( memcmp(&OutputData[4],&iInputData[4], 3) != 0)||
        ( memcmp(&OutputData[5],&iInputData[5], 3) != 0)||
        ( memcmp(&OutputData[6],&iInputData[6], 3) != 0)||
        ( memcmp(&OutputData[7],&iInputData[7], 3) != 0)          
      )
    {
        SLOG(eError, "ERROR: APO Process did not behave as expected");
        SLOG(eError, "Input Float: %f        Output 24bit: %d ", fData[0], convert_from_24bit(temp, OutputData[0]));
        SLOG(eError, "Input Float: %f        Output 24bit: %d ", fData[1], convert_from_24bit(temp, OutputData[1]));
        SLOG(eError, "Input Float: %f        Output 24bit: %d ", fData[2], convert_from_24bit(temp, OutputData[2]));
        SLOG(eError, "Input Float: %f        Output 24bit: %d ", fData[3], convert_from_24bit(temp, OutputData[3]));
        SLOG(eError, "Input Float: %f        Output 24bit: %d ", fData[4], convert_from_24bit(temp, OutputData[4]));
        SLOG(eError, "Input Float: %f        Output 24bit: %d ", fData[5], convert_from_24bit(temp, OutputData[5]));
        SLOG(eError, "Input Float: %f        Output 24bit: %d ", fData[6], convert_from_24bit(temp, OutputData[6]));
        SLOG(eError, "Input Float: %f        Output 24bit: %d ", fData[7], convert_from_24bit(temp, OutputData[7]));
        dw = FNS_FAIL;            
    }

    //Start with a FLOAT32 SineWave, Convert it to 24 bit INT and reconvert it to FLOAT32
    // The output should be exactly equal to the input
    //Allocate Source and destination buffers
    pSineWave = (FLOAT32*)LocalAlloc(LMEM_FIXED, sizeof(FLOAT32) * 480);
    pOutput = (FLOAT32*)LocalAlloc(LMEM_FIXED, sizeof(FLOAT32) * 480);
    _ASSERT(pSineWave && pOutput);

    ZeroMemory(pOutput, sizeof(FLOAT32) * 480);
    // Mono 48K.
    BUILD_FORMAT(&AudioIntFormat, KSDATAFORMAT_SUBTYPE_PCM, 1, 3, 24, 48000);
    BUILD_FORMAT(&AudioFloatFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 1, 4, 32, 48000);
    // Call Fl32Int24 collect the data in a temp buffer, then call Fl32Int24 and collect the data in the 
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

    //At this point the sinewave has gone through a complete roundtrip from  to FLOAT32 to 24bit INT to FLOAT32
    //Verify if the Source and the final destination are the same
    for (UINT32 i = 0; i < 480; i++)
    {        
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
