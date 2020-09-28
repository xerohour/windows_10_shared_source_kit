// Copyright(C) Microsoft. All rights reserved.

#include "stdafx.h"

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOFormatInt16Fl32Create
//
// Description:
//  TestCase to test if AudioFormatInt16Fl32 class is registered and the necessary interfaces can be obtained from it
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOFormatInt16Fl32Create()
{
    CAPOFormatInt16Fl32Test APOFormatTest;
    return APOFormatTest.APOTestCreate(__uuidof(AudioFormatConvert));
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOFormatInt16Fl32Process
//
// Description:
//  TestCase to test if APO AudioFormatInt16Fl32 behaves as expected
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOFormatInt16Fl32Process()
{    
    CAPOFormatInt16Fl32Test APOFormatTest;
    return APOFormatTest.APOFormatProcess();
}

//-------------------------------------------------------------------------
// Class: CAPOFormatInt16Fl32Test
// Method: APOFormatProcess
//
// Description:
//   Functional testcase for FormatInt16Fl32 APO
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOFormatInt16Fl32Test::APOFormatProcess()
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
    INT16* pSineWave = NULL;
    INT16* pOutput = NULL;
    
    SLOG(eInfo1, "Creating FormatFl32Int16 APO");
    hr = pAPORT.CreateInstance(__uuidof(AudioFormatConvert));
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: APORT Object creation from AudioFormatInt16Fl32 FAILED", dw, FNS_FAIL);
    
    SLOG(eInfo1, "Querying for IAPOConfig from FormatInt16Fl32");
    hr = pAPORT.QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), &pAPOConfig);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: QueryInterface for IAPOConfig from IAPORT FAILED", dw, FNS_FAIL);
       
    //Check how the boundary conditions are handled
    // i.e. does the dynamic range of -32768 to 32767 map the entire dynamic range for FLOAT32 from -1.0 to 1.0
    //Test input exteremes, 0.0 and Out of bounds
    // 4 frames of input data
    INT16 iInputData[8];
    iInputData[0] = 0;
    iInputData[1] = -0;
    iInputData[2] = -32767;
    iInputData[3] = 32767;
    iInputData[4] = -16383;
    iInputData[5] = 16383;
    iInputData[6] = 1;
    iInputData[7] = -1;

    FLOAT32 fOutputData[8];
    ZeroMemory(fOutputData, sizeof(FLOAT32) * 8);

    //Setup the AudioFormat for our test    
    AUDIO_FRAME_FORMAT AudioIntFormat;
    AUDIO_FRAME_FORMAT AudioFloatFormat;    
    BUILD_FORMAT(&AudioIntFormat, KSDATAFORMAT_SUBTYPE_PCM, 2, 2, 16, 48000);
    BUILD_FORMAT(&AudioFloatFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 2, 4, 32, 48000);
    //Get IAudioMediaType for the current format    
    CAudioMediaType::AF2AMT(AudioIntFormat, &pInAudioMediaType);
    CAudioMediaType::AF2AMT(AudioFloatFormat, &pOutAudioMediaType);

    //Setup input and output Connections
    pInputConn = new APO_CONNECTION_PROPERTY;
    pOutputConn = new APO_CONNECTION_PROPERTY;
    _ASSERT( (pInputConn) && (pOutputConn) );
    BUILD_CONNECTION(pInputConn, iInputData, 4, 0, BUFFER_VALID);
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
        ! CCheck::InRange(fOutputData[3], FLOAT32(1.0),0.999, 1.001) || 
        ! CCheck::InRange(fOutputData[4], FLOAT32(-0.5),0.999, 1.001) || 
        ! CCheck::InRange(fOutputData[5], FLOAT32(0.5),0.999, 1.001) || 
        ! CCheck::InRange(fOutputData[6], FLOAT32(3.05e-5),0.999, 1.001) || 
        ! CCheck::InRange(fOutputData[7], FLOAT32(-3.05e-5),0.999, 1.001)
      )
    {
        SLOG(eError, "ERROR: APO Process did not behave as expected");
        SLOG(eError, "Input 16bit: %d        Output Float: %f", iInputData[0], fOutputData[0]);
        SLOG(eError, "Input 16bit: %d        Output Float: %f", iInputData[1], fOutputData[1]);
        SLOG(eError, "Input 16bit: %d        Output Float: %f", iInputData[2], fOutputData[2]);
        SLOG(eError, "Input 16bit: %d        Output Float: %f", iInputData[3], fOutputData[3]);
        SLOG(eError, "Input 16bit: %d        Output Float: %f", iInputData[4], fOutputData[4]);
        SLOG(eError, "Input 16bit: %d        Output Float: %f", iInputData[5], fOutputData[5]);
        SLOG(eError, "Input 16bit: %d        Output Float: %f", iInputData[6], fOutputData[6]);
        SLOG(eError, "Input 16bit: %d        Output Float: %f", iInputData[7], fOutputData[7]);
        dw = FNS_FAIL;            
    }

    //Start with a INT16 SineWave, Convert it to FLOAT32 and reconvert it to INT16
    // The output should be exactly equal to the input
    //Allocate Source and destination buffers
    pSineWave = (INT16*)LocalAlloc(LMEM_FIXED, sizeof(INT16) * 480);
    pOutput = (INT16*)LocalAlloc(LMEM_FIXED, sizeof(INT16) * 480);
    _ASSERT(pSineWave && pOutput);

    ZeroMemory(pOutput, sizeof(INT16) * 480);
    // Mono 48K.
    BUILD_FORMAT(&AudioIntFormat, KSDATAFORMAT_SUBTYPE_PCM, 1, 2, 16, 48000);
    BUILD_FORMAT(&AudioFloatFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 1, 4, 32, 48000);
    // Call Int16Fl32 collect the data in a temp buffer, then call Fl32Int16 and collect the data in the 
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
        if(! CCheck::InRange(pOutput[i], pSineWave[i],(INT16)1, (INT16)1))
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
    pSineWave = (INT16*)LocalAlloc(LMEM_FIXED, sizeof(INT16) * 65536); //enitre 16 but dynamic range..2^16
    pOutput = (INT16*)LocalAlloc(LPTR, sizeof(INT16) * 65536); //Zero Out the destination space
    SLOG(eInfo1, "Testing bit for bit transparency for the entire dynamic range of INT16");    
    for (UINT i = 0x0000; i <= 0xFFFF; i++)
    {
        pSineWave[i] = (INT16)i;
    }
    dwRet = APOTestProcess( pSineWave, //Source data buffer
                         AudioIntFormat, //Source format
                         __uuidof(AudioFormatConvert), //Source CLSID
                         pOutput,  //destination buffer
                         AudioFloatFormat,  // Intermediate data format
                         __uuidof(AudioFormatConvert), // Intermediate APO CLSID
                         KSDATAFORMAT_SUBTYPE_PCM,
                         65536,
                         FALSE
                         );    
    if(FNS_PASS != dwRet)
    {
        SLOG(eError, "/ERROR: APOTestProcess FAILED, 0x%X", dw);            
        dw = FNS_FAIL;
        goto exit;
    }

    //Verify if the Source and the final destination are the same
    for (UINT32 i = 0; i < 65536; i++)
    {
        if(! CCheck::InRange(pOutput[i], pSineWave[i],(INT16)1, (INT16)1))
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
