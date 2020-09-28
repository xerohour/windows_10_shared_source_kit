// Copyright(C) Microsoft. All rights reserved.

#include "stdafx.h"

int& convert_from_24bit(int& Dst, s24Bit& Src)
{
    pair_of_numbers<s24Bit, int> Pair(0);
    Pair.Initialize(Src);
    Dst = Pair;    
    return Dst;
}
//-------------------------------------------------------------------------
// Class: CBaseFormatTest
// Method: APOTestCreate
//
// Description:
//   Base Method to test Format APO registration and if the necessary interfaces can be obtained
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CBaseFormatTest::APOTestCreate(REFCLSID rclsid)
{
    {
        DWORD dw = FNS_PASS;
        HRESULT hr = S_OK;
        IAudioFormatConverterPtr pAPOFormat= NULL;
        IAudioProcessingObjectPtr pAPO = NULL;
        IAudioProcessingObjectRTPtr pAPORT = NULL;
        IAudioProcessingObjectConfigurationPtr pAPOConfig= NULL;

        //Create APO Format instance
        SLOG(eInfo1, "Creating Format APO Object");
        hr = pAPOFormat.CreateInstance(rclsid);
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: CreateInstance for IAudioFormat FAILED", dw, FNS_FAIL);
        //Query for IAPO
        SLOG(eInfo1, "Querying for IAPO");
        hr = pAPOFormat.QueryInterface(__uuidof(IAudioProcessingObject), &pAPO);
        METHOD_CHECKFAIL_SETVAL_EXIT(hr , "ERROR: QueryInterface on AudioFormat for IAPO FAILED", dw , FNS_FAIL);
        //Query for IAPORT
        SLOG(eInfo1, "Querying for IAPORT");
        hr = pAPOFormat.QueryInterface(__uuidof(IAudioProcessingObjectRT), &pAPORT);
        METHOD_CHECKFAIL_SETVAL_EXIT(hr , "ERROR: QueryInterface on AudioFormat for IAPORT FAILED", dw , FNS_FAIL);
        //Query for IAPOConfig
        SLOG(eInfo1, "Querying for IAPOConfiguration");
        hr = pAPOFormat.QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), &pAPOConfig);
        METHOD_CHECKFAIL_SETVAL_EXIT(hr , "ERROR: QueryInterface on AudioFormat for IAPOConfig FAILED", dw , FNS_FAIL);

    exit:
        return dw;

    }
}


//-------------------------------------------------------------------------
// Class: CBaseFormatTest
// Method: APOTestSetDitherState
//
// Description:
//   Base Method to test IAudioDither::SetDitherState on the FormatConverter APO
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CBaseFormatTest::APOTestSetDitherState(REFCLSID rclsid)
{
    {
        DWORD dw = FNS_PASS;
        HRESULT hr = S_OK;
        IAudioFormatConverterPtr pAPOFormat= NULL;
        IAudioProcessingObjectRTPtr pAPORT = NULL;    
        IAudioProcessingObjectConfigurationPtr pAPOConfig = NULL;
        APO_CONNECTION_PROPERTY* pInputConn = NULL;
        APO_CONNECTION_PROPERTY* pOutputConn = NULL;
        CComPtr<IAudioDither> pAPODither = NULL;
        DITHER_STATE DitherState = DITHER_NONE;
        IAudioMediaType* pInAudioMediaType = NULL;
        IAudioMediaType* pOutAudioMediaType = NULL;
        FLOAT32 fInput[8], fOutput[8];

        //Create APO Format instance
        SLOG(eInfo1, "Creating Format APO Object");
        hr = pAPOFormat.CreateInstance(rclsid);
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: CreateInstance for IAudioFormat FAILED", dw, FNS_FAIL);

        //Query for IAudioDither
        SLOG(eInfo1, "Querying for IAudioDither");
        hr = pAPOFormat.QueryInterface(__uuidof(IAudioDither), &pAPODither);
        METHOD_CHECKFAIL_SETVAL_EXIT(hr , "ERROR: QueryInterface on AudioFormat for IAudioDither FAILED", dw , FNS_FAIL);

        //Query for IAPOConfig
        SLOG(eInfo1, "Querying for IAPOConfig from FormatFl32Int16");
        hr = pAPOFormat.QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), &pAPOConfig);
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: QueryInterface for IAPOConfig from IAPOConfig FAILED", dw, FNS_FAIL);

        //Query for IAPORT
        SLOG(eInfo1, "Querying for IAPOConfig from FormatFl32Int16");
        hr = pAPOFormat.QueryInterface(__uuidof(IAudioProcessingObjectRT), &pAPORT);
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: QueryInterface for IAPORT from IAPOConfig FAILED", dw, FNS_FAIL);

        //Set an invalid Dither State
        DitherState = (DITHER_STATE)(UINT32)0xFFFFFFFF;
        SLOG(eInfo1, "Calling with invalid Dither State");
        hr = pAPODither->SetDitherState(DitherState);
        METHOD_CHECKSUCCESS_SETVAL(hr , "ERROR: SetDitherState with invalid DitherState SUCCEEDED", dw , FNS_FAIL);

        //Lock the APO
        //Setup the AudioFormat for our test
        AUDIO_FRAME_FORMAT AudioFloatFormat;
        AUDIO_FRAME_FORMAT AudioIntFormat;
        BUILD_FORMAT(&AudioFloatFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 2, 4, 32, 48000);
        BUILD_FORMAT(&AudioIntFormat, KSDATAFORMAT_SUBTYPE_PCM, 2, 2, 16, 48000);

        //Setup input and output Connections
        pInputConn = new APO_CONNECTION_PROPERTY;
        pOutputConn = new APO_CONNECTION_PROPERTY;
        //_ASSERT( (pInputConn) && (pOutputConn) );
        if( (NULL == pInputConn) || (NULL == pOutputConn))
        {
            SLOG(eError, "Memory allocation failure");
            dw = FNS_FAIL;
            goto exit;
        }
        BUILD_CONNECTION(pInputConn, fInput, 4, 0, BUFFER_VALID);
        BUILD_CONNECTION(pOutputConn, fOutput, 4, 0, BUFFER_SILENT);
        //Get IAudioMediaType for the current format    
        CAudioMediaType::AF2AMT(AudioFloatFormat, &pInAudioMediaType);
        CAudioMediaType::AF2AMT(AudioIntFormat, &pOutAudioMediaType);

        SLOG(eInfo1, "Locking APO for Processing");
        hr = LockAPO(pAPORT, 1, pInAudioMediaType, 1, pOutAudioMediaType);
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Lock for Process on the APO FAILED", dw, FNS_FAIL);

        //Set the Dither State
        DitherState = DITHER_NONE;
        SLOG(eInfo1, "Setting Dither state on a Locked APO");
        hr = pAPODither->SetDitherState(DitherState);
        METHOD_CHECKSUCCESS_SETVAL(hr , "ERROR: SetDitherState on Locked APO SUCCEEDED", dw , FNS_FAIL);

        SLOG(eInfo1, "UnLocking APO");
        hr = pAPOConfig->UnlockForProcess();
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: UnLock for Process on the APO FAILED", dw, FNS_FAIL);

        //Make valid calls
        //Set the Dither State to DITHER_NONE
        DitherState = DITHER_NONE;
        SLOG(eInfo1, "Setting Dither state to DITHER_NONE");
        hr = pAPODither->SetDitherState(DitherState);
        METHOD_CHECKFAIL_SETVAL(hr , "ERROR: SetDitherState FAILED for DITHER_NONE", dw , FNS_FAIL);

        //Set the Dither State
        DitherState = DITHER_TRIHP;
        SLOG(eInfo1, "Setting Dither state to DITHER_TRIHP");
        hr = pAPODither->SetDitherState(DitherState);
        METHOD_CHECKFAIL_SETVAL(hr , "ERROR: SetDitherState FAILED for DITHER_TRIHP", dw , FNS_FAIL);

        //Set the Dither State
        DitherState = DITHER_TRIHP_NOISE;
        SLOG(eInfo1, "Setting Dither state to DITHER_TRIHP_NOISE");
        hr = pAPODither->SetDitherState(DitherState);
        METHOD_CHECKFAIL_SETVAL(hr , "ERROR: SetDitherState FAILED for DITHER_TRIHP_NOISE", dw , FNS_FAIL);


        

    exit:
        SAFE_DELETE_POINTER(pInputConn);
        SAFE_DELETE_POINTER(pOutputConn);
        SAFE_RELEASE_POINTER(pInAudioMediaType);
        SAFE_RELEASE_POINTER(pOutAudioMediaType);

        return dw;

    }
}//DWORD CBaseFormatTest::APOTestSetDitherState(REFCLSID rclsid)


//-------------------------------------------------------------------------
// Class: CBaseFormatTest
// Method: APOTestSetDitherState
//
// Description:
//   Base Method to test IAudioDither::GetDitherState on the FormatConverter APO
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CBaseFormatTest::APOTestGetDitherState(REFCLSID rclsid)
{
    {
        DWORD dw = FNS_PASS;
        HRESULT hr = S_OK;
        IAudioFormatConverterPtr pAPOFormat= NULL;
        CComPtr<IAudioDither> pAPODither = NULL;
        DITHER_STATE DitherState = DITHER_NONE;

        //Create APO Format instance
        SLOG(eInfo1, "Creating Format APO Object");
        hr = pAPOFormat.CreateInstance(rclsid);
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: CreateInstance for IAudioFormat FAILED", dw, FNS_FAIL);

        //Query for IAudioDither
        SLOG(eInfo1, "Querying for IAudioDither");
        hr = pAPOFormat.QueryInterface(__uuidof(IAudioDither), &pAPODither);
        METHOD_CHECKFAIL_SETVAL_EXIT(hr , "ERROR: QueryInterface on AudioFormat for IAudioDither FAILED", dw , FNS_FAIL);

        //Call with NULL/Invalid arguments
        SLOG(eInfo1, "Calling GetDitherState with NULL/Invalid arguments");
        hr = pAPODither->GetDitherState(NULL);
        METHOD_CHECKSUCCESS_SETVAL(hr , "ERROR: GetDitherState with NULL argument SUCCEEDED", dw , FNS_FAIL);

        //Make a valid/legit call
        SLOG(eInfo1, "Calling GetDitherState with valid arguments");
        hr = pAPODither->GetDitherState(&DitherState);
        METHOD_CHECKFAIL_SETVAL(hr , "ERROR: GetDitherState with valid args FAILED", dw , FNS_FAIL);

        if( ((UINT32)DITHER_NONE != (UINT32)DitherState) && 
            ((UINT32)DITHER_TRIHP != (UINT32)DitherState) &&
            ((UINT32)DITHER_TRIHP_NOISE != (UINT32)DitherState)
          )
        {
            SLOG(eError, "ERROR: GetDitherState returned an invalid DITHER_STATE value 0x%X", (UINT32)DitherState);
            dw = FNS_FAIL;            
        }
        

    exit:
        return dw;

    }
}//DWORD CBaseFormatTest::APOTestGetDitherState(REFCLSID rclsid)

//-------------------------------------------------------------------------
// Class: CBaseFormatTest
// Method: APOTestProcess
//
// Description:
//   Base Method to test Format APO Process
//   This method starts with a source format, converts to intermediate and reconverts to source format
//   It then checks to see if the source data is exactly equal to the destination data
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CBaseFormatTest::APOTestProcess(LPVOID lpSource,
                                      AUDIO_FRAME_FORMAT& SourceFormat,
                                      REFCLSID  SourceClassID,
                                      LPVOID lpDest,
                                      AUDIO_FRAME_FORMAT& IntermediateFormat,
                                      REFCLSID  IntermediateClassID,
                                      GUID SampleDataType,
                                      UINT32  NumFrames,
                                      BOOL   bGenerateSineWave
                                      )
{    
    DWORD dw = FNS_PASS;
    LPVOID lpIntermediate = NULL;
    HRESULT hr = S_OK;
    IAudioProcessingObjectRTPtr pAPOSrcRT = NULL;    
    IAudioProcessingObjectConfigurationPtr pAPOSrcConfig = NULL;
    IAudioProcessingObjectRTPtr pAPOIntermediateRT = NULL;
    IAudioProcessingObjectConfigurationPtr pAPOIntermediateConfig = NULL;
    IAudioMediaType* pInAudioMediaType = NULL;
    IAudioMediaType* pOutAudioMediaType = NULL;
    APO_CONNECTION_PROPERTY *pInConn = NULL;
    APO_CONNECTION_PROPERTY *pOutConn = NULL;
    APO_CONNECTION_DESCRIPTOR InputConnection, OutputConnection;
    APO_CONNECTION_DESCRIPTOR *pInputConnDesc = NULL, *pOutputConnDesc = NULL;
    WAVEFORMATEXTENSIBLE wfxEx;
    
    _ASSERT(lpSource);
    _ASSERT(SourceFormat.u32SamplesPerFrame == IntermediateFormat.u32SamplesPerFrame);

    if(bGenerateSineWave) //Generate the sine wave only if test data is not provided
    {
        CComPtr<IBasicLog> pBasicLog;
        
        SLOG(eInfo1, "Generating 1Khz PCM Test tone");
    
        // get a logger
        hr = g_IShell->GetBasicLog(&pBasicLog);
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FAILED to get basic log object", dw, FNS_FAIL);

        FORMAT2WAVEXT(&SourceFormat, &wfxEx);

        // Fill the buffer with a sine wave
        hr = FillBufferWithSineSignal(
            pBasicLog, XFAIL,
            &wfxEx.Format,
            1.0, // amplitude
            1000.0, // frequency
            0.0, // initial phase,
            0.0, // dc
            Method_NoDithering,
            reinterpret_cast<PBYTE>(lpSource),
            NumFrames,
            NumFrames * wfxEx.Format.nBlockAlign
        );
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FAILED to fill buffer with sine data", dw, FNS_FAIL);
    }
    //Get Source APO interfaces
    SLOG(eInfo1, "Creating APO instance");
    hr = pAPOSrcRT.CreateInstance(SourceClassID);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FAILED to CreateInstance of Source APORT", dw, FNS_FAIL);
    
    SLOG(eInfo1, "Querying for IAPOConfig from APO instance");
    hr = pAPOSrcRT.QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), &pAPOSrcConfig);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FAILED to Query for IAPO Object from Source APORT", dw, FNS_FAIL);
    
    //Get Intermediate Interfaces
    SLOG(eInfo1, "Creating APO instance");
    hr = pAPOIntermediateRT.CreateInstance(IntermediateClassID);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FAILED to CreateInstance of Intermediate APORT", dw, FNS_FAIL);
    
    SLOG(eInfo1, "Querying for IAPOConfig from APO instance");
    hr = pAPOIntermediateRT.QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), &pAPOIntermediateConfig);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FAILED to Query for IAPO Object from Intermediate APORT", dw, FNS_FAIL);
    
    //Create Input and Output Connections    
    lpIntermediate = (LPVOID)LocalAlloc(LMEM_FIXED, NumFrames * 
                                                    IntermediateFormat.u32BytesPerSampleContainer *
                                                    IntermediateFormat.u32SamplesPerFrame );
    _ASSERT(lpIntermediate);
    pInConn = new APO_CONNECTION_PROPERTY;
    pOutConn = new APO_CONNECTION_PROPERTY;
    _ASSERT( pInConn && pOutConn );
    BUILD_CONNECTION(pInConn, lpSource, NumFrames, 0, BUFFER_VALID);
    BUILD_CONNECTION(pOutConn, lpIntermediate, NumFrames, 0, BUFFER_SILENT);

    //Get IAudioMediaType for the current format    
    CAudioMediaType::AF2AMT(SourceFormat, &pInAudioMediaType);
    CAudioMediaType::AF2AMT(IntermediateFormat, &pOutAudioMediaType);
    SLOG(eInfo1, "Locking APO for Processing");        
    BUILD_DESCRIPTOR(&InputConnection, pInAudioMediaType, 
                     APO_CONNECTION_BUFFER_TYPE_EXTERNAL, 
                     pInConn->pBuffer, pInConn->u32ValidFrameCount);
    BUILD_DESCRIPTOR(&OutputConnection, pOutAudioMediaType,
                     APO_CONNECTION_BUFFER_TYPE_EXTERNAL,
                     pOutConn->pBuffer, pOutConn->u32ValidFrameCount);
    pInputConnDesc = &InputConnection;
    pOutputConnDesc = &OutputConnection;
    hr = pAPOSrcConfig->LockForProcess(1, &pInputConnDesc, 1, &pOutputConnDesc);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Lock for Process on Source APO FAILED", dw, FNS_FAIL);
    
    //Convert the Sinewave from FLOAT32 to INT16
    SLOG(eInfo1, "Calling APO Process");
    pAPOSrcRT->APOProcess(1, &pInConn, 1, &pOutConn);
    SLOG(eInfo1, "Unlocking APO");
    hr = pAPOSrcConfig->UnlockForProcess();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: UnLock for Process on theSource APO FAILED", dw, FNS_FAIL);
    
    // Now the output Connection should have the data in the intermediate format, 
    // Call APOProcess again on the other APO and convert the data back to Source format
    // in destination buffer.
    //Create Input and Output Connections        
    BUILD_CONNECTION(pInConn, lpIntermediate, NumFrames, 0, BUFFER_VALID);
    BUILD_CONNECTION(pOutConn, lpDest, NumFrames, 0, BUFFER_SILENT);

    //Input and Output formats get reversed now.
    SLOG(eInfo1, "Locking APO for Processing");
    BUILD_DESCRIPTOR(&InputConnection, pOutAudioMediaType, 
                     APO_CONNECTION_BUFFER_TYPE_EXTERNAL, 
                     pInConn->pBuffer, pInConn->u32ValidFrameCount);
    BUILD_DESCRIPTOR(&OutputConnection, pInAudioMediaType,
                     APO_CONNECTION_BUFFER_TYPE_EXTERNAL,
                     pOutConn->pBuffer, pOutConn->u32ValidFrameCount);
    pInputConnDesc = &InputConnection;
    pOutputConnDesc = &OutputConnection;
    hr = pAPOIntermediateConfig->LockForProcess(1, &pInputConnDesc, 1, &pOutputConnDesc);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Lock for Process on Intermediate APO FAILED", dw, FNS_FAIL);
    
    //Convert the Sinewave from FLOAT32 to INT16
    SLOG(eInfo1, "Calling APO Process");
    pAPOIntermediateRT->APOProcess(1, &pInConn, 1, &pOutConn);
    SLOG(eInfo1, "Unlocking APO");
    hr = pAPOIntermediateConfig->UnlockForProcess();
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: UnLock for Process on the Intermediate APO FAILED", dw, FNS_FAIL);
    
exit:
    SAFE_RELEASE_POINTER(pInAudioMediaType);
    SAFE_RELEASE_POINTER(pOutAudioMediaType);
    SAFE_FREE_POINTER(lpIntermediate);
    SAFE_DELETE_POINTER(pInConn);
    SAFE_DELETE_POINTER(pOutConn);
    return dw;
}


//-------------------------------------------------------------------------
// Class: CBaseFormatTest
// Method: LockAPO
//
// Description:
//   Base Method to test Format APO Process
//   This method starts with a source format, converts to intermediate and reconverts to source format
//   It then checks to see if the source data is exactly equal to the destination data
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//

HRESULT CBaseFormatTest::LockAPO
( IAudioProcessingObjectRT* pAPO,
  UINT32 ui32NumInConnDesc,
  IAudioMediaType* pInAudioMediaType,
  UINT32 ui32NumOutConnDesc,
  IAudioMediaType* pOutAudioMediaType
)
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;
    IAudioProcessingObjectRTPtr pAPORT = pAPO;
    IAudioProcessingObjectConfigurationPtr pAPOConfig = NULL;
    APO_CONNECTION_PROPERTY *pInConn = NULL;
    APO_CONNECTION_PROPERTY *pOutConn = NULL;
    APO_CONNECTION_DESCRIPTOR InputConnection, OutputConnection;
    APO_CONNECTION_DESCRIPTOR *pInputConnDesc = NULL, *pOutputConnDesc = NULL;

    SLOG(eInfo1, "Querying for IAPOConfig from APO instance");
    hr = pAPORT.QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), &pAPOConfig);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: FAILED to Query for IAPOConfig from APORT", dw, FNS_FAIL);

    // The Descriptors below have a pBuffer as 0x1000 and 0x4000, because the API checks for NULL, and also checks
    // that there is no overlap in the buffers since format converters are not inplace.
    // and the buffer value is not needed.
    // NumMaxFrames is 480, because none of the functions that call into this method
    // use more thatn 480 frames at a time.
    SLOG(eInfo1, "Locking APO for Processing");
    BUILD_DESCRIPTOR(&InputConnection, pInAudioMediaType, 
                     APO_CONNECTION_BUFFER_TYPE_EXTERNAL, 
                     0x1000, 480);
    BUILD_DESCRIPTOR(&OutputConnection, pOutAudioMediaType,
                     APO_CONNECTION_BUFFER_TYPE_EXTERNAL,
                     0x4000, 480);
    pInputConnDesc = &InputConnection;
    pOutputConnDesc = &OutputConnection;
    hr = pAPOConfig->LockForProcess(1, &pInputConnDesc, 1, &pOutputConnDesc);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Lock for Process on Intermediate APO FAILED", dw, FNS_FAIL);

exit:
    return hr;
}
