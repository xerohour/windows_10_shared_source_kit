// Copyright(C) Microsoft. All rights reserved.

#include "stdafx.h"

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOFormatFl32Int16Create
//
// Description:
//  TestCase to test if AudioFormatFl32Int16 class is registered and the necessary interfaces can be obtained from it
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOFormatFl32Int16Create()
{
    CAPOFormatFl32Int16Test APOFormatTest;
    return APOFormatTest.APOTestCreate(__uuidof(AudioFormatConvert));
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOFormatFl32Int16Process
//
// Description:
//  TestCase to test if APO AudioFormatFl32Int16 behaves as expected
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOFormatFl32Int16Process()
{    
    CAPOFormatFl32Int16Test APOFormatTest;
    return APOFormatTest.APOFormatProcess();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_BaseFormat_SetDitherState
//
// Description:
//  TestCase to test IAudioDither::SetDitherState
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_BaseFormat_SetDitherState()
{
    CAPOFormatFl32Int16Test APOFormatTest;
    return APOFormatTest.APOTestSetDitherState(__uuidof(AudioFormatConvert));
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_BaseFormat_GetDitherState
//
// Description:
//  TestCase to test IAudioDither::GetDitherState
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_BaseFormat_GetDitherState()
{
    CAPOFormatFl32Int16Test APOFormatTest;
    return APOFormatTest.APOTestGetDitherState(__uuidof(AudioFormatConvert));
}

//-------------------------------------------------------------------------
// Class: CAPOFormatFl32Int16Test
// Method: APOFormatProcess
//
// Description:
//   Functional testcase for FormatFl32Int16 APO
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOFormatFl32Int16Test::APOFormatProcess()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;
    DWORD dwRet = FNS_FAIL;
    IAudioProcessingObjectRTPtr pAPORT = NULL;    
    IAudioProcessingObjectConfigurationPtr pAPOConfig = NULL;
    CComPtr<IAudioDither> pAPODither = NULL;
    IAudioMediaType* pInAudioMediaType = NULL;
    IAudioMediaType* pOutAudioMediaType = NULL;
    APO_CONNECTION_PROPERTY* pInputConn = NULL;
    APO_CONNECTION_PROPERTY* pOutputConn = NULL;
    FLOAT32 *pfSineWave = NULL;
    FLOAT32 *pfOutput = NULL;
    DITHER_STATE DitState = DITHER_NONE;

    SLOG(eInfo1, "Creating FormatFl32Int16 APO");
    hr = pAPORT.CreateInstance(__uuidof(AudioFormatConvert));
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: APORT Object creation from AudioFormatFl32Int16 FAILED", dw, FNS_FAIL);
    
    SLOG(eInfo1, "Querying for IAPOConfig from FormatFl32Int16");
    hr = pAPORT.QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), &pAPOConfig);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: QueryInterface for IAPOConfig from IAPORT FAILED", dw, FNS_FAIL);

    SLOG(eInfo1, "Querying for IAudioDither from FormatFl32Int16");
    hr = pAPORT.QueryInterface(__uuidof(IAudioDither), &pAPODither);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: QueryInterface for IAudioDither from IAPORT FAILED", dw, FNS_FAIL);
        
    //For each dither state, run the tests
    for(UINT ii = 0; ii < 3; ii++)
    {
        //Check how the boundary conditions are handled
        // i.e. does the dynamic range of -1.0 to 1.0 map the entire dynamic range for 16 bit from -32768 to 32767
        //Test input exteremes, 0.0 and Out of bounds
        // 3 frames of input data        
        FLOAT32 fInput[8];
        fInput[0] = (FLOAT32)1.0; //+ve max
        fInput[1] = (FLOAT32)0.0;
        fInput[2] = (FLOAT32)0.5;
        fInput[3] = (FLOAT32)-1.0; //-ve max
        fInput[4] = (FLOAT32)1.01; //+ve Clipping
        fInput[5] = (FLOAT32)-1.01; //-ve Clipping
        fInput[6] = (FLOAT32)3.06e-5; // 1
        fInput[7] = (FLOAT32)-3.06e-5; // 1

        //2 frames of Output data
        INT16 Output[8];
        ZeroMemory(Output, sizeof(INT16) * 8);

        //Setup the AudioFormat for our test
        AUDIO_FRAME_FORMAT AudioFloatFormat;
        AUDIO_FRAME_FORMAT AudioIntFormat;
        BUILD_FORMAT(&AudioFloatFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 2, 4, 32, 48000);
        BUILD_FORMAT(&AudioIntFormat, KSDATAFORMAT_SUBTYPE_PCM, 2, 2, 16, 48000);

        //Setup input and output Connections
        SAFE_DELETE_POINTER(pInputConn);    
        pInputConn = new APO_CONNECTION_PROPERTY;
        SAFE_DELETE_POINTER(pOutputConn);
        pOutputConn = new APO_CONNECTION_PROPERTY;
        if((NULL == pInputConn) || (NULL == pOutputConn))
        {
            SLOG(eError, "ERROR: Memory Allocation Failure");            
            dw = FNS_FAIL;
            goto exit;
        }

        BUILD_CONNECTION(pInputConn, fInput, 4, 0, BUFFER_VALID);
        BUILD_CONNECTION(pOutputConn, Output, 4, 0, BUFFER_SILENT);
        //Get IAudioMediaType for the current format
        SAFE_RELEASE_POINTER(pInAudioMediaType);
        SAFE_RELEASE_POINTER(pOutAudioMediaType);
        CAudioMediaType::AF2AMT(AudioFloatFormat, &pInAudioMediaType);
        CAudioMediaType::AF2AMT(AudioIntFormat, &pOutAudioMediaType);

        DitState = (DITHER_STATE)ii;
        SLOG(eInfo1, "Setting DitherState to 0x%X", (UINT)DitState);
        hr = pAPODither->SetDitherState(DitState);
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Unable to SetDitherState on FormatConverter", dw, FNS_FAIL);

        SLOG(eInfo1, "Locking APO for Processing");
        hr = LockAPO(pAPORT, 1, pInAudioMediaType, 1, pOutAudioMediaType);
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Lock for Process on the APO FAILED", dw, FNS_FAIL);

        //UINT uiControl = 0;
        //uiControl = _controlfp(0, 0); //get the floating point control word
        //_controlfp(_RC_NEAR, _MCW_RC ); //set the control word to round 
        SLOG(eInfo1, "Calling APO Process");
        pAPORT->APOProcess(1, &pInputConn, 1, &pOutputConn);
        //_controlfp(uiControl, 0xFFFFFFFF); //reset it back to the original control word
        SLOG(eInfo1, "UnLocking APO");
        hr = pAPOConfig->UnlockForProcess();
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: UnLock for Process on the APO FAILED", dw, FNS_FAIL);

        //Check the output values
        if( Output[0] != INT16(32767) || Output[1] != INT16(0) || Output[2] != INT16(16384) || 
            Output[3] != INT16(-32768) || Output[4] != INT16(32767) || Output[5] != INT16(-32768) ||
            Output[6] != INT16(1) || Output[7] != INT16(-1)
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
            //dw = FNS_FAIL; //See bug 1348762
        }

        //Start with a FLOAT32 SineWave, Convert it to INT16 and reconvert it to FLOAT32
        // The output should be exactly equal to the input
        SAFE_FREE_POINTER(pfSineWave);
        SAFE_FREE_POINTER(pfOutput);
        pfSineWave = (FLOAT32*)LocalAlloc(LMEM_FIXED, sizeof(FLOAT32) * 480);
        pfOutput = (FLOAT32*)LocalAlloc(LMEM_FIXED, sizeof(FLOAT32) * 480);        
        if((NULL == pfSineWave) || (NULL == pfOutput))
        {
            SLOG(eError, "ERROR: Memory Allocation Failure");            
            dw = FNS_FAIL;
            goto exit;
        }

        ZeroMemory(pfOutput, sizeof(FLOAT32) * 480);   
        BUILD_FORMAT(&AudioFloatFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 1, 4, 32, 48000);
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
            //if(! CCheck::InRange(pfOutput[i], pfSineWave[i],(FLOAT32)(0.5/32767)))
            if(! CCheck::InRange(pfOutput[i], pfSineWave[i],(FLOAT32)0.001))        
            {
                SLOG(eError, "ERROR: APO Process did not behave as expected");
                SLOG(eError, "Source Value%f            Dest Value%f,    Frame no: %d", pfSineWave[i], pfOutput[i], i);
                dw = FNS_FAIL;
            }
        }
    }//for((UINT)(DITHER_STATE)DitState = DITHER_NONE; DitState <= DITHER_TRIHOP_NOISE; DitState++)

exit:
    SAFE_FREE_POINTER(pfSineWave);
    SAFE_FREE_POINTER(pfOutput);
    SAFE_DELETE_POINTER(pInputConn);
    SAFE_DELETE_POINTER(pOutputConn);
    SAFE_RELEASE_POINTER(pInAudioMediaType);
    SAFE_RELEASE_POINTER(pOutAudioMediaType);
    return dw;
}
