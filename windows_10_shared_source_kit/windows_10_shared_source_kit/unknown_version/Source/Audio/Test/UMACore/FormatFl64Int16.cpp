// Copyright(C) Microsoft. All rights reserved.

#include "stdafx.h"

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOFormatFl64Int16Create
//
// Description:
//  TestCase to test if AudioFormatFl64Int16 class is registered and the necessary interfaces can be obtained from it
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOFormatFl64Int16Create()
{
    CAPOFormatFl64Int16Test APOFormatTest;
    return APOFormatTest.APOTestCreate(__uuidof(AudioFormatConvert));
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOFormatFl64Int16Process
//
// Description:
//  TestCase to test if APO AudioFormatFl64Int16 behaves as expected
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOFormatFl64Int16Process()
{    
    CAPOFormatFl64Int16Test APOFormatTest;
    return APOFormatTest.APOFormatProcess();
}


//-------------------------------------------------------------------------
// Class: CAPOFormatFl64Int16Test
// Method: APOFormatProcess
//
// Description:
//   Functional testcase for FormatFl64Int16 APO
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOFormatFl64Int16Test::APOFormatProcess()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;
    DWORD dwRet = FNS_FAIL;
    IAudioProcessingObjectRTPtr pAPORT = NULL;    
    IAudioProcessingObjectConfigurationPtr pAPOConfig = NULL;
    APO_CONNECTION_PROPERTY* pInputConn = NULL;
    APO_CONNECTION_PROPERTY* pOutputConn = NULL;
    IAudioMediaType* pInAudioMediaType = NULL;
    IAudioMediaType* pOutAudioMediaType = NULL;
    FLOAT64 *pfSineWave = NULL;
    FLOAT64 *pfOutput = NULL; 

    SLOG(eInfo1, "Creating FormatFl64Int16 APO");
    hr = pAPORT.CreateInstance(__uuidof(AudioFormatConvert));
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: APORT Object creation from AudioFormatFl64Int16 FAILED", dw, FNS_FAIL);
    
    SLOG(eInfo1, "Querying for IAPOConfig from FormatFl64Int16");
    hr = pAPORT.QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), &pAPOConfig);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: QueryInterface for IAPOConfig from IAPORT FAILED", dw, FNS_FAIL);
    
    //Check how the boundary conditions are handled
    // i.e. does the dynamic range of -1.0 to 1.0 map the entire dynamic range for 16 bit from -32768 to 32767
    //Test input exteremes, 0.0 and Out of bounds
    // 3 frames of input data
    FLOAT64 fInput[8];    
    fInput[0] = (FLOAT64)0.0;
    fInput[1] = (FLOAT64)-0.0;
    fInput[2] = (FLOAT64)(1.0 - 3.0e-5); //+ve max; clip it
    fInput[3] = (FLOAT64)-1.0; //-ve max
    fInput[4] = (FLOAT64)0.5;
    fInput[5] = (FLOAT64)-0.5;
    fInput[6] = (FLOAT64)(1.0 - 3.0e-5); //+ve max; clip it
    fInput[7] = (FLOAT64)-1.0; //-ve Clipping

    //2 frames of Output data
    INT16 Output[8];
    ZeroMemory(Output, sizeof(INT16) * 8);

    //Setup the AudioFormat for our test
    AUDIO_FRAME_FORMAT AudioFloatFormat;
    AUDIO_FRAME_FORMAT AudioIntFormat;
    BUILD_FORMAT(&AudioFloatFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 2, 8, 64, 48000);
    BUILD_FORMAT(&AudioIntFormat, KSDATAFORMAT_SUBTYPE_PCM, 2, 2, 16, 48000);

    //Setup input and output Connections
    pInputConn = new APO_CONNECTION_PROPERTY;
    pOutputConn = new APO_CONNECTION_PROPERTY;
    _ASSERT( (pInputConn) && (pOutputConn) );
    BUILD_CONNECTION(pInputConn, fInput, 4, 0, BUFFER_VALID); //4 frames of input data
    BUILD_CONNECTION(pOutputConn, Output, 4, 0, BUFFER_SILENT); // 4 frames of output data
    //Get IAudioMediaType for the current format    
    CAudioMediaType::AF2AMT(AudioFloatFormat, &pInAudioMediaType);
    CAudioMediaType::AF2AMT(AudioIntFormat, &pOutAudioMediaType);

    SLOG(eInfo1, "Locking APO for Processing");
    hr = LockAPO(pAPORT, 1, pInAudioMediaType, 1, pOutAudioMediaType);
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Lock for Process on the APO FAILED, 0x%X", hr);
        dw = FNS_FAIL;
        goto exit;
    }
    //UINT uiControl = 0;
    //uiControl = _controlfp(0, 0); //get the floating point control word
    //_controlfp(_RC_NEAR, _MCW_RC ); //set the control word to round 
    SLOG(eInfo1, "Calling APO Process");
    pAPORT->APOProcess(1, &pInputConn, 1, &pOutputConn);
    //_controlfp(uiControl, 0xFFFFFFFF); //reset it back to the original control word
    SLOG(eInfo1, "UnLocking APO");
    hr = pAPOConfig->UnlockForProcess();
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: UnLock for Process on the APO FAILED, 0x%X", hr);
        dw = FNS_FAIL;
        goto exit;
    }        
    //Check the output values
    if( Output[0] != INT16(0) ||        Output[1] != INT16(0) ||
        Output[2] != INT16(32767) ||    Output[3] != INT16(-32768) ||
        Output[4] != INT16(16384) ||    Output[5] != INT16(-16384) ||
        Output[6] != INT16(32767) ||    Output[7] != INT16(-32768)
      )
    {
        SLOG(eError, "ERROR: APO Process did not behave as expected");
        SLOG(eError, "Input Float: %f        Output 16bit: %d", fInput[0], Output[0]);
        SLOG(eError, "Input Float: %f        Output 16bit: %d", fInput[1], Output[1]);
        SLOG(eError, "Input Float: %f        Output 16bit: %d", fInput[2], Output[2]);
        SLOG(eError, "Input Float: %f        Output 16bit: %d", fInput[3], Output[3]);
        SLOG(eError, "Input Float: %f        Output 16bit: %d", fInput[4], Output[4]);
        SLOG(eError, "Input Float: %f        Output 16bit: %d", fInput[5], Output[5]);
        SLOG(eError, "Input Float: %f        Output 16bit: %d", fInput[6], Output[6]);
        SLOG(eError, "Input Float: %f        Output 16bit: %d", fInput[7], Output[7]);
        dw = FNS_FAIL;            
    }

    //Start with a FLOAT64 SineWave, Convert it to INT16 and reconvert it to FLOAT32
    // The output should be exactly equal to the input
    pfSineWave = (FLOAT64*)LocalAlloc(LMEM_FIXED, sizeof(FLOAT64) * 480);
    pfOutput = (FLOAT64*)LocalAlloc(LMEM_FIXED, sizeof(FLOAT64) * 480);        
    _ASSERT( pfSineWave && pfOutput );

    ZeroMemory(pfOutput, sizeof(FLOAT64) * 480);   
    BUILD_FORMAT(&AudioFloatFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 1, 8, 64, 48000);
    BUILD_FORMAT(&AudioIntFormat, KSDATAFORMAT_SUBTYPE_PCM, 1, 2, 16, 48000);

    // This will create the necessary source data (1Khz sinewave) and push it through both the APO's and finally
    // collect in the Output buffer
    dwRet = APOTestProcess(pfSineWave, //Source data
        AudioFloatFormat, //Source format
        __uuidof(AudioFormatConvert), //Source CLSID
        pfOutput, //Destination buffer
        AudioIntFormat, //Intermediate Format INT16
        __uuidof(AudioFormatConvert), //Intermediate CLSID
        KSDATAFORMAT_SUBTYPE_IEEE_FLOAT //Source Data type
        );

    if(FNS_PASS != dwRet)
    {
        SLOG(eError, "ERROR: APOTestProcess FAILED, 0x%X", dw);            
        dw = FNS_FAIL;
        goto exit;
    }
    //At this point the sinewave has gone through a complete roundtrip from FLOAT32 to INT16 to FLOAT32
    //Verify if the Source and the final destination are the same
    for (UINT32 i = 0; i < 480; i++)
    {
        //if(! CCheck::InRange(pfOutput[i], pfSineWave[i],(FLOAT64)(0.5/32767)))
        if(! CCheck::InRange(pfOutput[i], pfSineWave[i],(FLOAT64)0.001))
        {
            SLOG(eError, "ERROR: APO Process did not behave as expected");
            SLOG(eError, "Source Value%f            Dest Value%f,    Frame no: %d", pfSineWave[i], pfOutput[i], i);
            dw = FNS_FAIL;
        }
    }

exit:
    SAFE_FREE_POINTER(pfSineWave);
    SAFE_FREE_POINTER(pfOutput);
    SAFE_RELEASE_POINTER(pInAudioMediaType);
    SAFE_RELEASE_POINTER(pOutAudioMediaType);
    return dw;
}
