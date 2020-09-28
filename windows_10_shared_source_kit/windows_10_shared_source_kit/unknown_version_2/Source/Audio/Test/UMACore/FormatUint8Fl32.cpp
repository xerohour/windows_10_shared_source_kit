// Copyright(C) Microsoft. All rights reserved.

#include "stdafx.h"

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOFormatUint8Fl32Create
//
// Description:
//  TestCase to test if AudioFormatUint8Fl32 class is registered and the necessary interfaces can be obtained from it
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOFormatUint8Fl32Create()
{
    CAPOFormatUint8Fl32Test APOFormatTest;
    return APOFormatTest.APOTestCreate(__uuidof(AudioFormatConvert));
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOFormatUint8Fl32Process
//
// Description:
//  TestCase to test if APO AudioFormatUint8Fl32 behaves as expected
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOFormatUint8Fl32Process()
{    
    CAPOFormatUint8Fl32Test APOFormatTest;
    return APOFormatTest.APOFormatProcess();
}

//-------------------------------------------------------------------------
// Class: CAPOFormatUint8Fl32Test
// Method: APOFormatProcess
//
// Description:
//   Functional testcase for FormatUint8Fl32 APO
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOFormatUint8Fl32Test::APOFormatProcess()
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
    UINT8* pSineWave = NULL;
    UINT8* pOutput = NULL;
    
    SLOG(eInfo1, "Creating FormatUint8Fl32 APO");
    hr = pAPORT.CreateInstance(__uuidof(AudioFormatConvert));
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: APORT Object creation from AudioFormatUint8Fl32 FAILED", dw, FNS_FAIL);
    
    SLOG(eInfo1, "Querying for IAPOConfig from FormatUint8Fl32");
    hr = pAPORT.QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), &pAPOConfig);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: QueryInterface for IAPOConfig from IAPORT FAILED", dw, FNS_FAIL);
       
    //Check how the boundary conditions are handled
    // i.e. does the dynamic range of 0 to 255 map the entire dynamic range for FLOAT32 from -1.0 to 1.0
    //Test input exteremes, 0.0 and Out of bounds
    // 4 frames of input data
    UINT8 uiInputData[8];
    uiInputData[0] = 128;
    uiInputData[1] = 128;
    uiInputData[2] = 0;
    uiInputData[3] = 255;
    uiInputData[4] = 64;
    uiInputData[5] = 192;
    uiInputData[6] = 129;
    uiInputData[7] = 127;

    FLOAT32 fOutputData[8];
    ZeroMemory(fOutputData, sizeof(FLOAT32) * 8);

    //Setup the AudioFormat for our test    
    AUDIO_FRAME_FORMAT AudioIntFormat;
    AUDIO_FRAME_FORMAT AudioFloatFormat;    
    BUILD_FORMAT(&AudioIntFormat, KSDATAFORMAT_SUBTYPE_PCM, 2, 1, 8, 48000);
    BUILD_FORMAT(&AudioFloatFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 2, 4, 32, 48000);
    //Get IAudioMediaType for the current format    
    CAudioMediaType::AF2AMT(AudioIntFormat, &pInAudioMediaType);
    CAudioMediaType::AF2AMT(AudioFloatFormat, &pOutAudioMediaType);

    //Setup input and output Connections
    pInputConn = new APO_CONNECTION_PROPERTY;
    pOutputConn = new APO_CONNECTION_PROPERTY;
    _ASSERT( (pInputConn) && (pOutputConn) );
    BUILD_CONNECTION(pInputConn, uiInputData, 4, 0, BUFFER_VALID);
    BUILD_CONNECTION(pOutputConn, fOutputData, 4, 0, BUFFER_SILENT);

    SLOG(eInfo1, "Locking APO for Processing");
    hr = LockAPO(pAPORT, 1, pInAudioMediaType, 1, pOutAudioMediaType);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Lock for Process on the APO FAILED", dw, FNS_FAIL);    
    SLOG(eInfo1, "Calling APO Process");
    pAPORT->APOProcess(1, &pInputConn, 1, &pOutputConn);
    SLOG(eInfo1, "UnLocking APO");
    hr = pAPOConfig->UnlockForProcess();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: UnLock for Process on the APO FAILED", dw, FNS_FAIL);
    
    //Verify the Output Values
    if( ! CCheck::InRange(fOutputData[0], FLOAT32(0.0),0.999, 1.001) ||
        ! CCheck::InRange(fOutputData[1], FLOAT32(0.0),0.999, 1.001) || 
        ! CCheck::InRange(fOutputData[2], FLOAT32(-1.0),0.999, 1.001) || 
        ! CCheck::InRange(fOutputData[3], FLOAT32(1.0),0.99, 1.01) || 
        ! CCheck::InRange(fOutputData[4], FLOAT32(-0.5),0.999, 1.001) || 
        ! CCheck::InRange(fOutputData[5], FLOAT32(0.5),0.999, 1.001) || 
        ! CCheck::InRange(fOutputData[6], FLOAT32(7.8125e-3),0.999, 1.001) || 
        ! CCheck::InRange(fOutputData[7], FLOAT32(-7.8125e-3),0.999, 1.001)
      )
    {
        SLOG(eError, "ERROR: APO Process did not behave as expected");
        SLOG(eError, "Input Uint8: %d        Output Float: %f", uiInputData[0], fOutputData[0]);
        SLOG(eError, "Input Uint8: %d        Output Float: %f", uiInputData[1], fOutputData[1]);
        SLOG(eError, "Input Uint8: %d        Output Float: %f", uiInputData[2], fOutputData[2]);
        SLOG(eError, "Input Uint8: %d        Output Float: %f", uiInputData[3], fOutputData[3]);
        SLOG(eError, "Input Uint8: %d        Output Float: %f", uiInputData[4], fOutputData[4]);
        SLOG(eError, "Input Uint8: %d        Output Float: %f", uiInputData[5], fOutputData[5]);
        SLOG(eError, "Input Uint8: %d        Output Float: %f", uiInputData[6], fOutputData[6]);
        SLOG(eError, "Input Uint8: %d        Output Float: %f", uiInputData[7], fOutputData[7]);
        dw = FNS_FAIL;            
    }

    //Start with a UINT8 SineWave, Convert it to FLOAT32 and reconvert it to UINT8
    // The output should be exactly equal to the input
    //Allocate Source and destination buffers
    pSineWave = (UINT8*)LocalAlloc(LMEM_FIXED, sizeof(UINT8) * 480);
    pOutput = (UINT8*)LocalAlloc(LMEM_FIXED, sizeof(UINT8) * 480);
    _ASSERT(pSineWave && pOutput);

    ZeroMemory(pOutput, sizeof(UINT8) * 480);
    // Mono 48K.
    BUILD_FORMAT(&AudioIntFormat, KSDATAFORMAT_SUBTYPE_PCM, 1, 1, 8, 48000);
    BUILD_FORMAT(&AudioFloatFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 1, 4, 32, 48000);
    // Call Uint8Fl32 collect the data in a temp buffer, then call Fl32Int16 and collect the data in the 
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
        SLOG(eError, "/ERROR: APOTestProcess FAILED, 0x%X", dw);            
        dw = FNS_FAIL;
        goto exit;
    }

    //At this point the sinewave has gone through a complete roundtrip from  to FLOAT32 to INT16
    //Verify if the Source and the final destination are the same
    for (UINT32 i = 0; i < 480; i++)
    {
        if(! CCheck::InRange(pOutput[i], pSineWave[i],(UINT8)1, (UINT8)1))
        {
            SLOG(eError, "ERROR: APO Process did not behave as expected");
            SLOG(eError, "Source Value: %d            Dest Value: %d,    Frame no: %d", pSineWave[i], pOutput[i], i);
            dw = FNS_FAIL;
        }
    }

    // Check bit for bit transparency on boundary, Negative max, Negative max /2, Negative 0, 
    //                                             Positive Max, Positive Max/2, Positive 0
    SAFE_FREE_POINTER(pSineWave);
    SAFE_FREE_POINTER(pOutput);
    pSineWave = (UINT8*)LocalAlloc(LMEM_FIXED, sizeof(UINT8) * 256); //enitre 8 bit dynamic range
    pOutput = (UINT8*)LocalAlloc(LPTR, sizeof(UINT8) * 256); //Zero Out the destination space
    SLOG(eInfo1, "Testing bit for bit transparency for the entire dynamic range of INT16");    
    for (UINT i = 0x00; i <= 0xFF; i++)
    {
        pSineWave[i] = (UINT8)i;
    }
    dwRet = APOTestProcess( pSineWave, //Source data buffer
                         AudioIntFormat, //Source format
                         __uuidof(AudioFormatConvert), //Source CLSID
                         pOutput,  //destination buffer
                         AudioFloatFormat,  // Intermediate data format
                         __uuidof(AudioFormatConvert), // Intermediate APO CLSID
                         KSDATAFORMAT_SUBTYPE_PCM,
                         256,
                         FALSE
                         );    
    if(FNS_PASS != dwRet)
    {
        SLOG(eError, "/ERROR: APOTestProcess FAILED, 0x%X", dw);            
        dw = FNS_FAIL;
        goto exit;
    }

    //Verify if the Source and the final destination are the same
    for (UINT32 i = 0; i < 256; i++)
    {
        if(! CCheck::InRange(pOutput[i], pSineWave[i],(UINT8)1, (UINT8)1))
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
