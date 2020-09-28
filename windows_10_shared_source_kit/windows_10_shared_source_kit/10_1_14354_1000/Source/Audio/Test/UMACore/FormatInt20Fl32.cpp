// Copyright(C) Microsoft. All rights reserved.

#include "stdafx.h"

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOFormatInt24Fl32Create
//
// Description:
//  TestCase to test if AudioFormatInt20Fl32 class is registered and the necessary interfaces can be obtained from it
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOFormatInt20Fl32Create()
{
    CAPOFormatInt20Fl32Test APOFormatTest;
    return APOFormatTest.APOTestCreate(__uuidof(AudioFormatConvert));
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOFormatInt20Fl32Process
//
// Description:
//  TestCase to test if APO AudioFormatInt20Fl32 behaves as expected
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOFormatInt20Fl32Process()
{    
    CAPOFormatInt20Fl32Test APOFormatTest;
    return APOFormatTest.APOFormatProcess();
}

//-------------------------------------------------------------------------
// Class: CAPOFormatInt20Fl32Test
// Method: APOFormatProcess
//
// Description:
//   Functional testcase for FormatInt20Fl32 APO
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOFormatInt20Fl32Test::APOFormatProcess()
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
    LPVOID  pSineWave = NULL;
    LPVOID  pOutput = NULL;
    const UINT32 TWENTY_BIT_ALIGN_SHIFT = 12;

    SLOG(eInfo1, "Creating FormatFl32Int24 APO");
    hr = pAPORT.CreateInstance(__uuidof(AudioFormatConvert));
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: APORT Object creation from AudioFormatInt20Fl32 FAILED", dw, FNS_FAIL);
    
    SLOG(eInfo1, "Querying for IAPOConfig from FormatInt20Fl32");
    hr = pAPORT.QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), &pAPOConfig);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: QueryInterface for IAPOConfig from IAPORT FAILED", dw, FNS_FAIL);
        
    //Check how the boundary conditions are handled    
    //Test input exteremes, 0.0 and Out of bounds
    // 4 frames of input data
    INT32 iInputData[8]; //20bit Audio uses 32 bit containers, data is in higher order bits
    iInputData[0] =  0 << TWENTY_BIT_ALIGN_SHIFT;
    iInputData[1] = -0 << TWENTY_BIT_ALIGN_SHIFT;
    iInputData[2] = -524288 << TWENTY_BIT_ALIGN_SHIFT;
    iInputData[3] = 524287 << TWENTY_BIT_ALIGN_SHIFT;
    iInputData[4] = -262144 << TWENTY_BIT_ALIGN_SHIFT;
    iInputData[5] = 262144 << TWENTY_BIT_ALIGN_SHIFT;
    iInputData[6] = 1 << TWENTY_BIT_ALIGN_SHIFT;
    iInputData[7] = -1 << TWENTY_BIT_ALIGN_SHIFT;

    FLOAT32 fOutputData[8];
    ZeroMemory(fOutputData, sizeof(FLOAT32) * 8);

    //Setup the AudioFormat for our test    
    AUDIO_FRAME_FORMAT AudioIntFormat;
    AUDIO_FRAME_FORMAT AudioFloatFormat;    
    BUILD_FORMAT(&AudioIntFormat, KSDATAFORMAT_SUBTYPE_PCM, 2, 4, 20, 48000);
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
    int temp = 0;
    if( ! CCheck::InRange(fOutputData[0], FLOAT32(0.0),0.999, 1.001) ||
        ! CCheck::InRange(fOutputData[1], FLOAT32(0.0), 0.999, 1.001) ||
        ! CCheck::InRange(fOutputData[2], FLOAT32(-1.0), 0.999, 1.001) ||
        ! CCheck::InRange(fOutputData[3], FLOAT32(1.0), 0.999, 1.001) ||
        ! CCheck::InRange(fOutputData[4], FLOAT32(-0.5), 0.999, 1.001) ||
        ! CCheck::InRange(fOutputData[5], FLOAT32(0.5), 0.999, 1.001) ||
        ! CCheck::InRange(fOutputData[6], FLOAT32(1.907342e-6), 0.999, 1.001) ||
        ! CCheck::InRange(fOutputData[7], FLOAT32(-1.907342e-6), 0.999, 1.001)
      )
    {
        SLOG(eError, "ERROR: APO Process did not behave as expected");
        SLOG(eError, "Input 20bit: %d        Output Float: %f", iInputData[0] >> TWENTY_BIT_ALIGN_SHIFT, fOutputData[0]);
        SLOG(eError, "Input 20bit: %d        Output Float: %f", iInputData[1] >> TWENTY_BIT_ALIGN_SHIFT, fOutputData[1]);
        SLOG(eError, "Input 20bit: %d        Output Float: %f", iInputData[2] >> TWENTY_BIT_ALIGN_SHIFT, fOutputData[2]);
        SLOG(eError, "Input 20bit: %d        Output Float: %f", iInputData[3] >> TWENTY_BIT_ALIGN_SHIFT, fOutputData[3]);
        SLOG(eError, "Input 20bit: %d        Output Float: %f", iInputData[4] >> TWENTY_BIT_ALIGN_SHIFT, fOutputData[4]);
        SLOG(eError, "Input 20bit: %d        Output Float: %f", iInputData[5] >> TWENTY_BIT_ALIGN_SHIFT, fOutputData[5]);
        SLOG(eError, "Input 20bit: %d        Output Float: %f", iInputData[6] >> TWENTY_BIT_ALIGN_SHIFT, fOutputData[6]);
        SLOG(eError, "Input 20bit: %d        Output Float: %f", iInputData[7] >> TWENTY_BIT_ALIGN_SHIFT, fOutputData[7]);
        dw = FNS_FAIL;            
    }

    //Start with a INT16 SineWave, Convert it to FLOAT32 and reconvert it to INT16
    // The output should be exactly equal to the input
    //Allocate Source and destination buffers
    pSineWave = (LPVOID)LocalAlloc(LMEM_FIXED, sizeof(INT32) * 480);
    pOutput = (LPVOID)LocalAlloc(LMEM_FIXED, sizeof(INT32) * 480);
    _ASSERT(pSineWave && pOutput);

    ZeroMemory(pOutput, sizeof(INT32) * 480);
    // Mono 48K.
    BUILD_FORMAT(&AudioIntFormat, KSDATAFORMAT_SUBTYPE_PCM, 1, 4, 20, 48000);
    BUILD_FORMAT(&AudioFloatFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 1, 4, 32, 48000);
    // Call Int20Fl32 collect the data in a temp buffer, then call Fl32Int20 and collect the data in the 
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

    //At this point the sinewave has gone through a complete roundtrip from  to FLOAT32 to INT20
    //Verify if the Source and the final destination are the same
    for (UINT32 i = 0; i < 480; i++)
    {        
        if(! CCheck::InRange(((INT32*)pSineWave)[i], ((INT32*)pOutput)[i],1, 1))
        {            
            SLOG(eError, "ERROR: APO Process did not behave as expected");
            SLOG(eError, "Source Value: %d            Dest Value: %d,    Frame no: %d", ((INT32*)pSineWave)[i], ((INT32*)pOutput)[i], i);
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
