// Copyright(C) Microsoft. All rights reserved.

#include "stdafx.h"

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOFormatFl32Uint8Create
//
// Description:
//  TestCase to test if AudioFormatFl32Uint8 class is registered and the necessary interfaces can be obtained from it
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOFormatFl32Uint8Create()
{
    CAPOFormatFl32Uint8Test APOFormatTest;
    return APOFormatTest.APOTestCreate(__uuidof(AudioFormatConvert));
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOFormatFl32Uint8Process
//
// Description:
//  TestCase to test if APO AudioFormatFl32Uint8 behaves as expected
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOFormatFl32Uint8Process()
{    
    CAPOFormatFl32Uint8Test APOFormatTest;
    return APOFormatTest.APOFormatProcess();
}


//-------------------------------------------------------------------------
// Class: CAPOFormatFl32Uint8Test
// Method: APOFormatProcess
//
// Description:
//   Functional testcase for FormatFl32Uint8 APO
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOFormatFl32Uint8Test::APOFormatProcess()
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

    SLOG(eInfo1, "Creating FormatFl32Uint8 APO");
    hr = pAPORT.CreateInstance(__uuidof(AudioFormatConvert));
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: APORT Object creation from AudioFormatFl32Uint8 FAILED", dw, FNS_FAIL);
    
    SLOG(eInfo1, "Querying for IAPOConfig from FormatFl32Uint8");
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
        fInput[0] = (FLOAT32)(1.0 - 0.00393); //+ve max; clip it
        fInput[1] = (FLOAT32)0.0;
        fInput[2] = (FLOAT32)0.5;
        fInput[3] = (FLOAT32)-1.0; //-ve max
        fInput[4] = (FLOAT32)(1.0 - 0.00393); //+ve Clipping; clip it
        fInput[5] = (FLOAT32)-1.0; //-ve Clipping
        //fInput[6] = (FLOAT32)0.0078125; // 
        //fInput[7] = (FLOAT32)-0.0078125; // 
        fInput[6] = (FLOAT32)0.008; // 
        fInput[7] = (FLOAT32)-0.008; // 

        //2 frames of Output data
        UINT8 Output[8];
        ZeroMemory(Output, sizeof(UINT8) * 8);

        //Setup the AudioFormat for our test
        AUDIO_FRAME_FORMAT AudioFloatFormat;
        AUDIO_FRAME_FORMAT AudioIntFormat;
        BUILD_FORMAT(&AudioFloatFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 2, 4, 32, 48000);
        BUILD_FORMAT(&AudioIntFormat, KSDATAFORMAT_SUBTYPE_PCM, 2, 1, 8, 48000);

        //Setup input and output Connections
        SAFE_DELETE_POINTER(pInputConn);    
        pInputConn = new APO_CONNECTION_PROPERTY;
        SAFE_DELETE_POINTER(pOutputConn);
        pOutputConn = new APO_CONNECTION_PROPERTY;
        //_ASSERT( (pInputConn) && (pOutputConn) );
        if((NULL == pInputConn) || (NULL == pOutputConn))
        {
            SLOG(eError, "ERROR: Memory Allocation failure");
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

        SLOG(eInfo1, "Locking APO for Processing");
        hr = LockAPO(pAPORT, 1, pInAudioMediaType, 1, pOutAudioMediaType);
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Lock for Process on the APO FAILED", dw, FNS_FAIL);    

        SLOG(eInfo1, "Calling APO Process");
        pAPORT->APOProcess(1, &pInputConn, 1, &pOutputConn);    
        SLOG(eInfo1, "UnLocking APO");
        hr = pAPOConfig->UnlockForProcess();
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: UnLock for Process on the APO FAILED", dw, FNS_FAIL);

        //Check the output values
        if( Output[0] != UINT8(255) || 
            Output[1] != UINT8(128) || 
            Output[2] != UINT8(192) || 
            Output[3] != UINT8(0)   || 
            Output[4] != UINT8(255) || 
            Output[5] != UINT8(0)   ||
            Output[6] != UINT8(129) || 
            Output[7] != UINT8(127)
            )
        {
            SLOG(eError, "ERROR: APO Process did not behave as expected");
            SLOG(eError, "Input Float: %f        Output unsigned 8bit: %d", fInput[0], Output[0]);
            SLOG(eError, "Input Float: %f        Output unsigned 8bit: %d", fInput[1], Output[1]);
            SLOG(eError, "Input Float: %f        Output unsigned 8bit: %d", fInput[2], Output[2]);
            SLOG(eError, "Input Float: %f        Output unsigned 8bit: %d", fInput[3], Output[3]);
            SLOG(eError, "Input Float: %f        Output unsigned 8bit: %d", fInput[4], Output[4]);
            SLOG(eError, "Input Float: %f        Output unsigned 8bit: %d", fInput[5], Output[5]);
            SLOG(eError, "Input Float: %f        Output unsigned 8bit: %d", fInput[6], Output[6]);
            SLOG(eError, "Input Float: %f        Output unsigned 8bit: %d", fInput[7], Output[7]);
            dw = FNS_FAIL;       
        }

        //Start with a FLOAT32 SineWave, Convert it to INT16 and reconvert it to FLOAT32
        // The output should be exactly equal to the input
        SAFE_FREE_POINTER(pfSineWave);
        SAFE_FREE_POINTER(pfOutput);
        pfSineWave = (FLOAT32*)LocalAlloc(LMEM_FIXED, sizeof(FLOAT32) * 480);
        pfOutput = (FLOAT32*)LocalAlloc(LMEM_FIXED, sizeof(FLOAT32) * 480);        
        //_ASSERT( pfSineWave && pfOutput );
        if((NULL == pfSineWave) || (NULL == pfOutput))
        {
            SLOG(eError, "ERROR: Memory Allocation failure");
            dw = FNS_FAIL;
            goto exit;
        }

        ZeroMemory(pfOutput, sizeof(FLOAT32) * 480);   
        BUILD_FORMAT(&AudioFloatFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 1, 4, 32, 48000);
        BUILD_FORMAT(&AudioIntFormat, KSDATAFORMAT_SUBTYPE_PCM, 1, 1, 8, 48000);

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
            //if(! CCheck::InRange(pfOutput[i], pfSineWave[i],(FLOAT32)(0.5/128)))
            if(! CCheck::InRange(pfOutput[i], pfSineWave[i],(FLOAT32)0.01))
            {
                SLOG(eError, "ERROR: APO Process did not behave as expected");
                SLOG(eError, "Source Value%f            Dest Value%f,    Frame no: %d", pfSineWave[i], pfOutput[i], i);
                dw = FNS_FAIL;
            }
        }
    }//for(UINT ii = 0; ii < 3; ii++) For each dither type

exit:
    SAFE_DELETE_POINTER(pInputConn);
    SAFE_DELETE_POINTER(pOutputConn);
    SAFE_FREE_POINTER(pfSineWave);
    SAFE_FREE_POINTER(pfOutput);
    SAFE_RELEASE_POINTER(pInAudioMediaType);
    SAFE_RELEASE_POINTER(pOutAudioMediaType);
    return dw;
}
