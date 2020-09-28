// Copyright (C) Microsoft. All rights reserved.
#include "common.h"
#include <math.h>
#include <soundanalyze.h>

using namespace NotANumber;

extern APOLIMITS gAPOLimits;
typedef HRESULT (__stdcall IAudioProcessingObject:: *FORMAT_SUPPORT_PROC)(IAudioMediaType *pOpposite, IAudioMediaType *pRequested, IAudioMediaType **ppSupported);

// Establish the tolerance in Hz for loopback analysis
const static DOUBLE FREQ_TOLER_HZ = 10.0;

// define private engine error code
#define AEERR_APO_INVALIDARG 0x887C0048

////
// Routine Description:
//      Exercise a given SRC APO by processing a sine-tone buffer
// Arguments:
//      pguidAPOUnderTest   [in] -              CLSID of the APO under test
//      pwfxInputFormat     [in] -              Format of the input buffer used for processing
//      pwfxOutputFormat    [in] -              Format of the output buffer used for testing
//      nMillisInputBufferDuration [in] -       Duration of the input buffer used for testing
//      sAPOInitParams      [in] -              AUDIO_RATE_CONVERT_PARAMS struct used to initialized the APO
//      fRequestedDynamicInputFrameRate [in] -  Requested input dyn. frame rate
//      fRequestedDynamicOutputFrameRate [in] - Requested output dyn. frame rate
//      dTestToneFrequency  [in] -              Frequency of the test tone used to fill the input buffer
//      dTestToneAmplitude  [in] -              Amplitude of the test tone used to fill the input buffer
//      ppProcessorOutput   [out] -             Pointer to a buffer containing the output of the APO (free using delete[])
//      pcbProfessorOutput  [out] -             Number of bytes in the output buffer
//      pnProcessorOutputFrames [out] -         Number of frames in the output buffer
// Return value:
//      S_OK on success, the appropriate HRESULT on failure
////
HRESULT RateConverterFunctionalTest(LPCGUID pguidAPOUnderTest,
                                    const WAVEFORMATEXTENSIBLE* pwfxInputFormat,
                                    const WAVEFORMATEXTENSIBLE* pwfxOutputFormat,
                                    UINT32 nMillisInputBufferDuration,
                                    AUDIO_RATE_CONVERT_PARAMS sAPOInitParams,
                                    FLOAT32 fRequestedDynamicInputFrameRate,
                                    FLOAT32 fRequestedDynamicOutputFrameRate,
                                    DOUBLE dTestToneFrequency,
                                    DOUBLE dTestToneAmplitude,
                                    PBYTE* ppProcessorOutput,
                                    size_t* pcbProcessorOutput,
                                    UINT32* pnProcessorOutputFrames);

////
// Routine Description:
//      Given a buffer, return the fundamental frequency for each channel
// Arguments:
//      pBuffer             [in] -              Pointer to the buffer to be analyzed
//      pwfxBufferFormat    [in] -              Format of the buffer to be analyzed
//      nBufferFrames       [in] -              Number of frames in the buffer to be analyzed
//      pnChannelFrequencies [out] -            Number of output channel frequencies
//      padChannelFrequencies [out] -           Array containing the fundamental frequency for each channel
// Return value:
//      S_OK on success, the appropriate HRESULT on failure
////
HRESULT GetBufferFundamentalFrequency(PBYTE pBuffer,
                                      PWAVEFORMATEX pwfxBufferFormat,
                                      UINT32 nBufferFrames,
                                      UINT32* pnChannelFrequencies,
                                      DOUBLE** padChannelFrequencies);

BOOL IsRateConvertCMPT
(
    CAPODevice     *pAPODevice
)
{
    GUID            guidClassId;
    
    pAPODevice->GetClsID(&guidClassId);
    
    if (IsEqualGUID(guidClassId, __uuidof(AudioRateConvertCMPT)))
    {
        return (TRUE);
    }
    else
    {
        return (FALSE);
    }
}   //   IsRateConvertCMPT()


DWORD WINAPI TC_CAudioRateConvertCMPT_VerifyRegistration()
{
    CAPODevice                         *pCurrentDevice = NULL;
    CComPtr<IAudioProcessingObject>     pIAPO = NULL;
    PAPO_REG_PROPERTIES                 pApoRegProperties = NULL;
    UINT                                ii;
    HRESULT                             hr;
    
    pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    
    if (NULL == pCurrentDevice)
    {
        XLOG(XFAIL, eError, "FAIL: Could not instantiate APO object.");
        return (FNS_PASS);
    }
    
    if (!IsRateConvertCMPT(pCurrentDevice))
    {
        SLOG(eInfo1, "APO is not volume APO.  Skipping test.");
        return (FNS_PASS);
    }
    
    if (!pCurrentDevice->GetAPOInterfaces(&pIAPO, NULL, NULL))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObject interface.");
        return (FNS_PASS);
    }
    
    if (S_OK != (hr = pIAPO->GetRegistrationProperties(&pApoRegProperties)))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: GetRegistrationProperties returned error (0x%08lx).",
            hr);
        return (FNS_PASS);
    }
    
    {
        CCTMAutoFree    ApoReg(pApoRegProperties);
        
        Log_ApoRegProperties(pApoRegProperties);
        
        //  Verify flags.
        if (0x0000000a == pApoRegProperties->Flags)
        {
            SLOG(eInfo1, "Flags value is 0x0000000a.");
        }
        else
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: Flags value is 0x%08lx not 0x0000000a.",
                pApoRegProperties->Flags);
        }
        
        //  Verify APO Interfaces.
        if (3 == pApoRegProperties->u32NumAPOInterfaces)
        {
            SLOG(eInfo1, "Number of APO interfaces is 3.");
        }
        else
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: Number of APO interfaces is %d; expecting 3.",
                pApoRegProperties->u32NumAPOInterfaces);
        }
        
        //  Looking for IAudioRateConvert
        for (ii = pApoRegProperties->u32NumAPOInterfaces; ii; --ii)
        {
            if (IsEqualGUID(
                pApoRegProperties->iidAPOInterfaceList[ii-1],
                __uuidof(IAudioRateConvert)))
            {
                CComPtr<IUnknown>   pIUnknown = NULL;
                
                SLOG(eInfo1, "IAudioRateConvert interface listed.");
                
                hr = pIAPO->QueryInterface(
                    pApoRegProperties->iidAPOInterfaceList[ii-1],
                    (void**)&pIUnknown);
            
                if (S_OK != hr)
                {
                    XLOG(
                        XFAIL,
                        eError,
                        "FAIL: QueryInterface for IAudioRateConvert returns 0x%08lx.",
                        hr);
                }
                else
                {
                    SLOG(eInfo1, "QueryInterface for IAudioRateConvert succeeds.");
                }
                
                break;
            }
        }
        if (0 == ii)
        {
            //  IAudioRateConvert not found...
            XLOG(
                XFAIL,
                eError,
                "FAIL: IAudioRateConvert interface not listed.");
        }
        
        //  Looking for IAudioRateConvertRT
        for (ii = pApoRegProperties->u32NumAPOInterfaces; ii; --ii)
        {
            if (IsEqualGUID(
                pApoRegProperties->iidAPOInterfaceList[ii-1],
                __uuidof(IAudioRateConvertRT)))
            {
                CComPtr<IUnknown>   pIUnknown = NULL;
                
                SLOG(eInfo1, "IAudioRateConvertRT interface listed.");
                
                hr = pIAPO->QueryInterface(
                    pApoRegProperties->iidAPOInterfaceList[ii-1],
                    (void**)&pIUnknown);
            
                if (S_OK != hr)
                {
                    XLOG(
                        XFAIL,
                        eError,
                        "FAIL: QueryInterface for IAudioRateConvertRT returns 0x%08lx.",
                        hr);
                }
                else
                {
                    SLOG(eInfo1, "QueryInterface for IAudioRateConvertRT succeeds.");
                }
                
                break;
            }
        }
        if (0 == ii)
        {
            //  IAudioRateConvertRT not found...
            XLOG(
                XFAIL,
                eError,
                "FAIL: IAudioRateConvertRT interface not listed.");
        }
        
        //  Looking for IAudioProcessingObjectVBR
        for (ii = pApoRegProperties->u32NumAPOInterfaces; ii; --ii)
        {
            if (IsEqualGUID(
                pApoRegProperties->iidAPOInterfaceList[ii-1],
                __uuidof(IAudioProcessingObjectVBR)))
            {
                CComPtr<IUnknown>   pIUnknown = NULL;
                
                SLOG(eInfo1, "IAudioProcessingObjectVBR interface listed.");
                
                hr = pIAPO->QueryInterface(
                    pApoRegProperties->iidAPOInterfaceList[ii-1],
                    (void**)&pIUnknown);
            
                if (S_OK != hr)
                {
                    XLOG(
                        XFAIL,
                        eError,
                        "FAIL: QueryInterface for IAudioProcessingObjectVBR returns 0x%08lx.",
                        hr);
                }
                else
                {
                    SLOG(eInfo1, "QueryInterface for IAudioProcessingObjectVBR succeeds.");
                }
                
                break;
            }
        }
        if (0 == ii)
        {
            //  IAudioProcessingObjectVBR not found...
            XLOG(
                XFAIL,
                eError,
                "FAIL: IAudioProcessingObjectVBR interface not listed.");
        }
    }

    return (FNS_PASS);
}   //   TC_CAudioRateConvertCMPT_VerifyRegistration()


DWORD WINAPI TC_CAudioRateConvertCMPT_Initialize()
{
    CAPODevice                         *pCurrentDevice = NULL;
    CComPtr<IAudioProcessingObject>     pIAPO = NULL;
    AUDIO_RATE_CONVERT_PARAMS           APOParams = {0};
    GUID                                guidAPO;
    HRESULT                             hr;
    
    pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    
    if (NULL == pCurrentDevice)
    {
        XLOG(XFAIL, eError, "FAIL: Could not instantiate APO object.");
        return (FNS_PASS);
    }
    
    pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    
    if (!IsRateConvertCMPT(pCurrentDevice))
    {
        SLOG(eInfo1, "APO is not volume APO.  Skipping test.");
        return (FNS_PASS);
    }
    
    if (!pCurrentDevice->GetAPOInterfaces(&pIAPO, NULL, NULL))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObject interface.");
        return (FNS_PASS);
    }

    pCurrentDevice->GetClsID(&guidAPO);
    APOParams.APOInit.cbSize = sizeof(APOParams);
    APOParams.APOInit.clsid  = guidAPO;

    //  Initialize with all zero params...
    APOParams.fInputFrameRate         = 0.0;
    APOParams.fOutputFrameRate        = 0.0;
    APOParams.fMinInputFrameRate      = 0.0;
    APOParams.fMaxInputFrameRate      = 0.0;
    APOParams.fMinOutputFrameRate     = 0.0;
    APOParams.fMaxOutputFrameRate     = 0.0;
    APOParams.eAlgorithmType          = SRC_QUALITY_LINEAR; // 0
    APOParams.eFlowType               = AUDIO_FLOW_PULL; // 0
    APOParams.u32SamplesPerFrame      = 0;
    APOParams.u32HighQualityTaplength = 0;
    TestAPO_Initialize(
        guidAPO,
        sizeof(APOParams),
        (BYTE*)&APOParams,
        "with all zero params",
        APOERR_FORMAT_NOT_SUPPORTED);

    //  Add all kinds of other fun conditions here...

    //  Calling initialize twice...  (First time when device was added)
    APOParams.fInputFrameRate         = gAPOLimits.Rate_fInputFrameRate;
    APOParams.fOutputFrameRate        = gAPOLimits.Rate_fOutputFrameRate;
    APOParams.fMinInputFrameRate      = gAPOLimits.Rate_fMinInputFrameRate;
    APOParams.fMaxInputFrameRate      = gAPOLimits.Rate_fMaxInputFrameRate;
    APOParams.fMinOutputFrameRate     = gAPOLimits.Rate_fMinOutputFrameRate;
    APOParams.fMaxOutputFrameRate     = gAPOLimits.Rate_fMaxOutputFrameRate;
    APOParams.eAlgorithmType          = gAPOLimits.Rate_eAlgorithmType;
    APOParams.eFlowType               = gAPOLimits.Rate_eFlowType;
    APOParams.u32SamplesPerFrame      = gAPOLimits.Rate_u32SamplesPerFrame;
    APOParams.u32HighQualityTaplength = gAPOLimits.Rate_u32HighQualityTaplength;
    
    hr = pIAPO->Initialize(sizeof(APOParams), (BYTE*)&APOParams);
    if (APOERR_ALREADY_INITIALIZED == hr)
    {
        SLOG(
            eInfo1,
            "Calling Initialize second time returns "
                "APOERR_ALREADY_INITIALIZED.");
    }
    else
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Calling initialize second time returns error (0x%08lx).",
            hr);
    }

    //
    // Test with fixed input and output sample rates
    //
    APOParams.fInputFrameRate         = gAPOLimits.Rate_fInputFrameRate;
    APOParams.fOutputFrameRate        = gAPOLimits.Rate_fOutputFrameRate;
    APOParams.fMinInputFrameRate      = gAPOLimits.Rate_fInputFrameRate;
    APOParams.fMaxInputFrameRate      = gAPOLimits.Rate_fInputFrameRate;
    APOParams.fMinOutputFrameRate     = gAPOLimits.Rate_fOutputFrameRate;
    APOParams.fMaxOutputFrameRate     = gAPOLimits.Rate_fOutputFrameRate;
    APOParams.eAlgorithmType          = gAPOLimits.Rate_eAlgorithmType;
    APOParams.eFlowType               = gAPOLimits.Rate_eFlowType;
    APOParams.u32SamplesPerFrame      = gAPOLimits.Rate_u32SamplesPerFrame;
    APOParams.u32HighQualityTaplength = gAPOLimits.Rate_u32HighQualityTaplength;    

    TestAPO_Initialize(
        guidAPO,
        sizeof(APOParams),
        (BYTE*)&APOParams,
        "with fixed sample rates",
        S_OK);

    //
    // Test with really big sample rate ranges
    //
    APOParams.fInputFrameRate         = gAPOLimits.Rate_fInputFrameRate;
    APOParams.fOutputFrameRate        = gAPOLimits.Rate_fOutputFrameRate;
    APOParams.fMinInputFrameRate      = 1.0f;
    APOParams.fMaxInputFrameRate      = 10000000.0f;
    APOParams.fMinOutputFrameRate     = 1.0f;
    APOParams.fMaxOutputFrameRate     = 10000000.0f;
    APOParams.eAlgorithmType          = gAPOLimits.Rate_eAlgorithmType;
    APOParams.eFlowType               = gAPOLimits.Rate_eFlowType;
    APOParams.u32SamplesPerFrame      = gAPOLimits.Rate_u32SamplesPerFrame;
    APOParams.u32HighQualityTaplength = gAPOLimits.Rate_u32HighQualityTaplength;

    TestAPO_Initialize(
        guidAPO,
        sizeof(APOParams),
        (BYTE*)&APOParams,
        "with unreasonably large sample rate ranges",
        AEERR_APO_INVALIDARG); // AEEERR_APO_INVALIDARG


    //
    // Test with negative rate ranges
    //
    APOParams.fInputFrameRate         = gAPOLimits.Rate_fInputFrameRate;
    APOParams.fOutputFrameRate        = gAPOLimits.Rate_fOutputFrameRate;
    APOParams.fMinInputFrameRate      = -1.0f;
    APOParams.fMaxInputFrameRate      = gAPOLimits.Rate_fInputFrameRate;
    APOParams.fMinOutputFrameRate     = -1.0f;
    APOParams.fMaxOutputFrameRate     = gAPOLimits.Rate_fOutputFrameRate;
    APOParams.eAlgorithmType          = gAPOLimits.Rate_eAlgorithmType;
    APOParams.eFlowType               = gAPOLimits.Rate_eFlowType;
    APOParams.u32SamplesPerFrame      = gAPOLimits.Rate_u32SamplesPerFrame;
    APOParams.u32HighQualityTaplength = gAPOLimits.Rate_u32HighQualityTaplength;

    TestAPO_Initialize(
        guidAPO,
        sizeof(APOParams),
        (BYTE*)&APOParams,
        "with negative sample rate ranges",
        AEERR_APO_INVALIDARG); // AEEERR_APO_INVALIDARG

    //
    // Test with negative sample rates
    //
    APOParams.fInputFrameRate         = -1.0f;
    APOParams.fOutputFrameRate        = -1.0f;
    APOParams.fMinInputFrameRate      = gAPOLimits.Rate_fInputFrameRate;
    APOParams.fMaxInputFrameRate      = gAPOLimits.Rate_fInputFrameRate;
    APOParams.fMinOutputFrameRate     = gAPOLimits.Rate_fOutputFrameRate;
    APOParams.fMaxOutputFrameRate     = gAPOLimits.Rate_fOutputFrameRate;
    APOParams.eAlgorithmType          = gAPOLimits.Rate_eAlgorithmType;
    APOParams.eFlowType               = gAPOLimits.Rate_eFlowType;
    APOParams.u32SamplesPerFrame      = gAPOLimits.Rate_u32SamplesPerFrame;
    APOParams.u32HighQualityTaplength = gAPOLimits.Rate_u32HighQualityTaplength; 

    TestAPO_Initialize(
        guidAPO,
        sizeof(APOParams),
        (BYTE*)&APOParams,
        "with negative sample rates",
        APOERR_FORMAT_NOT_SUPPORTED);

    //
    // Test with MIN > MAX input sample rates
    //
    APOParams.fInputFrameRate         = gAPOLimits.Rate_fInputFrameRate;
    APOParams.fOutputFrameRate        = gAPOLimits.Rate_fOutputFrameRate;
    APOParams.fMinInputFrameRate      = gAPOLimits.Rate_fMaxInputFrameRate;
    APOParams.fMaxInputFrameRate      = gAPOLimits.Rate_fMinInputFrameRate;
    APOParams.fMinOutputFrameRate     = gAPOLimits.Rate_fMinOutputFrameRate;
    APOParams.fMaxOutputFrameRate     = gAPOLimits.Rate_fMaxOutputFrameRate;
    APOParams.eAlgorithmType          = gAPOLimits.Rate_eAlgorithmType;
    APOParams.eFlowType               = gAPOLimits.Rate_eFlowType;
    APOParams.u32SamplesPerFrame      = gAPOLimits.Rate_u32SamplesPerFrame;
    APOParams.u32HighQualityTaplength = gAPOLimits.Rate_u32HighQualityTaplength;

    TestAPO_Initialize(
        guidAPO,
        sizeof(APOParams),
        (BYTE*)&APOParams,
        "with max > min input sample rate",
        AEERR_APO_INVALIDARG); // AEEERR_APO_INVALIDARG

    APOParams.fInputFrameRate         = gAPOLimits.Rate_fInputFrameRate;
    APOParams.fOutputFrameRate        = gAPOLimits.Rate_fOutputFrameRate;
    APOParams.fMinInputFrameRate      = gAPOLimits.Rate_fMaxInputFrameRate;
    APOParams.fMaxInputFrameRate      = gAPOLimits.Rate_fMinInputFrameRate;
    APOParams.fMinOutputFrameRate     = gAPOLimits.Rate_fMaxOutputFrameRate;
    APOParams.fMaxOutputFrameRate     = gAPOLimits.Rate_fMinOutputFrameRate;
    APOParams.eAlgorithmType          = gAPOLimits.Rate_eAlgorithmType;
    APOParams.eFlowType               = gAPOLimits.Rate_eFlowType;
    APOParams.u32SamplesPerFrame      = gAPOLimits.Rate_u32SamplesPerFrame;
    APOParams.u32HighQualityTaplength = gAPOLimits.Rate_u32HighQualityTaplength;

    TestAPO_Initialize(
        guidAPO,
        sizeof(APOParams),
        (BYTE*)&APOParams,
        "with max > min output sample rate",
        AEERR_APO_INVALIDARG); // AEEERR_APO_INVALIDARG
        
    //
    // Test with input sample rate out of bounds
    //
    APOParams.fInputFrameRate         = gAPOLimits.Rate_fMinInputFrameRate - 100;
    APOParams.fOutputFrameRate        = gAPOLimits.Rate_fOutputFrameRate;
    APOParams.fMinInputFrameRate      = gAPOLimits.Rate_fMinInputFrameRate;
    APOParams.fMaxInputFrameRate      = gAPOLimits.Rate_fMaxInputFrameRate;
    APOParams.fMinOutputFrameRate     = gAPOLimits.Rate_fMinOutputFrameRate;
    APOParams.fMaxOutputFrameRate     = gAPOLimits.Rate_fMaxOutputFrameRate;
    APOParams.eAlgorithmType          = gAPOLimits.Rate_eAlgorithmType;
    APOParams.eFlowType               = gAPOLimits.Rate_eFlowType;
    APOParams.u32SamplesPerFrame      = gAPOLimits.Rate_u32SamplesPerFrame;
    APOParams.u32HighQualityTaplength = gAPOLimits.Rate_u32HighQualityTaplength;

    TestAPO_Initialize(
        guidAPO,
        sizeof(APOParams),
        (BYTE*)&APOParams,
        "with input sample rate below minimum",
        AEERR_APO_INVALIDARG); // AEEERR_APO_INVALIDARG

    APOParams.fInputFrameRate         = gAPOLimits.Rate_fMaxInputFrameRate + 100;
    APOParams.fOutputFrameRate        = gAPOLimits.Rate_fOutputFrameRate;
    APOParams.fMinInputFrameRate      = gAPOLimits.Rate_fMinInputFrameRate;
    APOParams.fMaxInputFrameRate      = gAPOLimits.Rate_fMaxInputFrameRate;
    APOParams.fMinOutputFrameRate     = gAPOLimits.Rate_fMinOutputFrameRate;
    APOParams.fMaxOutputFrameRate     = gAPOLimits.Rate_fMaxOutputFrameRate;
    APOParams.eAlgorithmType          = gAPOLimits.Rate_eAlgorithmType;
    APOParams.eFlowType               = gAPOLimits.Rate_eFlowType;
    APOParams.u32SamplesPerFrame      = gAPOLimits.Rate_u32SamplesPerFrame;
    APOParams.u32HighQualityTaplength = gAPOLimits.Rate_u32HighQualityTaplength;

    TestAPO_Initialize(
        guidAPO,
        sizeof(APOParams),
        (BYTE*)&APOParams,
        "with input sample rate above maximum",
        AEERR_APO_INVALIDARG); // AEEERR_APO_INVALIDARG

    //
    // Test with output sample rate out of bounds
    //
    APOParams.fInputFrameRate         = gAPOLimits.Rate_fInputFrameRate;
    APOParams.fOutputFrameRate        = gAPOLimits.Rate_fMinOutputFrameRate - 100;
    APOParams.fMinInputFrameRate      = gAPOLimits.Rate_fMinInputFrameRate;
    APOParams.fMaxInputFrameRate      = gAPOLimits.Rate_fMaxInputFrameRate;
    APOParams.fMinOutputFrameRate     = gAPOLimits.Rate_fMinOutputFrameRate;
    APOParams.fMaxOutputFrameRate     = gAPOLimits.Rate_fMaxOutputFrameRate;
    APOParams.eAlgorithmType          = gAPOLimits.Rate_eAlgorithmType;
    APOParams.eFlowType               = gAPOLimits.Rate_eFlowType;
    APOParams.u32SamplesPerFrame      = gAPOLimits.Rate_u32SamplesPerFrame;
    APOParams.u32HighQualityTaplength = gAPOLimits.Rate_u32HighQualityTaplength;

    TestAPO_Initialize(
        guidAPO,
        sizeof(APOParams),
        (BYTE*)&APOParams,
        "with output sample rate below minimum",
        AEERR_APO_INVALIDARG); // AEEERR_APO_INVALIDARG

    APOParams.fInputFrameRate         = gAPOLimits.Rate_fInputFrameRate;
    APOParams.fOutputFrameRate        = gAPOLimits.Rate_fMaxOutputFrameRate + 100;
    APOParams.fMinInputFrameRate      = gAPOLimits.Rate_fMinInputFrameRate;
    APOParams.fMaxInputFrameRate      = gAPOLimits.Rate_fMaxInputFrameRate;
    APOParams.fMinOutputFrameRate     = gAPOLimits.Rate_fMinOutputFrameRate;
    APOParams.fMaxOutputFrameRate     = gAPOLimits.Rate_fMaxOutputFrameRate;
    APOParams.eAlgorithmType          = gAPOLimits.Rate_eAlgorithmType;
    APOParams.eFlowType               = gAPOLimits.Rate_eFlowType;
    APOParams.u32SamplesPerFrame      = gAPOLimits.Rate_u32SamplesPerFrame;
    APOParams.u32HighQualityTaplength = gAPOLimits.Rate_u32HighQualityTaplength;

    TestAPO_Initialize(
        guidAPO,
        sizeof(APOParams),
        (BYTE*)&APOParams,
        "with output sample rate above maximum",
        AEERR_APO_INVALIDARG); // AEEERR_APO_INVALIDARG

    //
    // Test with input sample rate out of bounds
    //   - Use eAlgorithmType = SRC_QUALITY_SINC3
    //
    APOParams.fInputFrameRate         = gAPOLimits.Rate_fMinInputFrameRate - 100;
    APOParams.fOutputFrameRate        = gAPOLimits.Rate_fOutputFrameRate;
    APOParams.fMinInputFrameRate      = gAPOLimits.Rate_fMinInputFrameRate;
    APOParams.fMaxInputFrameRate      = gAPOLimits.Rate_fMaxInputFrameRate;
    APOParams.fMinOutputFrameRate     = gAPOLimits.Rate_fMinOutputFrameRate;
    APOParams.fMaxOutputFrameRate     = gAPOLimits.Rate_fMaxOutputFrameRate;
    APOParams.eAlgorithmType          = SRC_QUALITY_SINC3;
    APOParams.eFlowType               = gAPOLimits.Rate_eFlowType;
    APOParams.u32SamplesPerFrame      = gAPOLimits.Rate_u32SamplesPerFrame;
    APOParams.u32HighQualityTaplength = gAPOLimits.Rate_u32HighQualityTaplength;

    TestAPO_Initialize(
        guidAPO,
        sizeof(APOParams),
        (BYTE*)&APOParams,
        "with input sample rate below minimum",
        AEERR_APO_INVALIDARG); // AEEERR_APO_INVALIDARG

    //
    // Test with input sample rate out of bounds
    //   - Use eAlgorithmType = SRC_QUALITY_SINC3
    //
    APOParams.fInputFrameRate         = gAPOLimits.Rate_fMaxInputFrameRate + 100;
    APOParams.fOutputFrameRate        = gAPOLimits.Rate_fOutputFrameRate;
    APOParams.fMinInputFrameRate      = gAPOLimits.Rate_fMinInputFrameRate;
    APOParams.fMaxInputFrameRate      = gAPOLimits.Rate_fMaxInputFrameRate;
    APOParams.fMinOutputFrameRate     = gAPOLimits.Rate_fMinOutputFrameRate;
    APOParams.fMaxOutputFrameRate     = gAPOLimits.Rate_fMaxOutputFrameRate;
    APOParams.eAlgorithmType          = SRC_QUALITY_SINC3;
    APOParams.eFlowType               = gAPOLimits.Rate_eFlowType;
    APOParams.u32SamplesPerFrame      = gAPOLimits.Rate_u32SamplesPerFrame;
    APOParams.u32HighQualityTaplength = gAPOLimits.Rate_u32HighQualityTaplength;

    TestAPO_Initialize(
        guidAPO,
        sizeof(APOParams),
        (BYTE*)&APOParams,
        "with input sample rate above maximum",
        AEERR_APO_INVALIDARG); // AEEERR_APO_INVALIDARG

    //
    // Test with output sample rate out of bounds
    //   - Use eAlgorithmType = SRC_QUALITY_SINC3
    //
    APOParams.fInputFrameRate         = gAPOLimits.Rate_fInputFrameRate;
    APOParams.fOutputFrameRate        = gAPOLimits.Rate_fMinOutputFrameRate - 100;
    APOParams.fMinInputFrameRate      = gAPOLimits.Rate_fMinInputFrameRate;
    APOParams.fMaxInputFrameRate      = gAPOLimits.Rate_fMaxInputFrameRate;
    APOParams.fMinOutputFrameRate     = gAPOLimits.Rate_fMinOutputFrameRate;
    APOParams.fMaxOutputFrameRate     = gAPOLimits.Rate_fMaxOutputFrameRate;
    APOParams.eAlgorithmType          = SRC_QUALITY_SINC3;
    APOParams.eFlowType               = gAPOLimits.Rate_eFlowType;
    APOParams.u32SamplesPerFrame      = gAPOLimits.Rate_u32SamplesPerFrame;
    APOParams.u32HighQualityTaplength = gAPOLimits.Rate_u32HighQualityTaplength;

    TestAPO_Initialize(
        guidAPO,
        sizeof(APOParams),
        (BYTE*)&APOParams,
        "with output sample rate below minimum",
        AEERR_APO_INVALIDARG); // AEEERR_APO_INVALIDARG

    //
    // Test with output sample rate out of bounds
    //   - Use eAlgorithmType = SRC_QUALITY_SINC3
    //
    APOParams.fInputFrameRate         = gAPOLimits.Rate_fInputFrameRate;
    APOParams.fOutputFrameRate        = gAPOLimits.Rate_fMaxOutputFrameRate + 100;
    APOParams.fMinInputFrameRate      = gAPOLimits.Rate_fMinInputFrameRate;
    APOParams.fMaxInputFrameRate      = gAPOLimits.Rate_fMaxInputFrameRate;
    APOParams.fMinOutputFrameRate     = gAPOLimits.Rate_fMinOutputFrameRate;
    APOParams.fMaxOutputFrameRate     = gAPOLimits.Rate_fMaxOutputFrameRate;
    APOParams.eAlgorithmType          = SRC_QUALITY_SINC3;
    APOParams.eFlowType               = gAPOLimits.Rate_eFlowType;
    APOParams.u32SamplesPerFrame      = gAPOLimits.Rate_u32SamplesPerFrame;
    APOParams.u32HighQualityTaplength = gAPOLimits.Rate_u32HighQualityTaplength;

    TestAPO_Initialize(
        guidAPO,
        sizeof(APOParams),
        (BYTE*)&APOParams,
        "with output sample rate above maximum",
        AEERR_APO_INVALIDARG); // AEEERR_APO_INVALIDARG

    return (FNS_PASS);
}   //   TC_CAudioRateConvertCMPT_Initialize()

DWORD WINAPI TC_IAudioRateConvert_SetInputFrameRate()
{
    CAPODevice                                     *pCurrentDevice = NULL;
    GUID                                            guidAPO = GUID_NULL;
    CComPtr<IAudioProcessingObject>                 pIAPO = NULL;
    CComPtr<IAudioProcessingObjectRT>               pIAPORT = NULL;
    CComPtr<IAudioProcessingObjectConfiguration>    pIAPOConfig = NULL;
    CComPtr<IAudioRateConvert>                      pISRC = NULL;
    CComPtr<IAudioProcessingObject>                 pFreshAPO = NULL;
    CComPtr<IAudioRateConvert>                      pFreshSRC = NULL;
    HRESULT                                         hr = S_OK;
    
    pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    
    if (NULL == pCurrentDevice)
    {
        XLOG(XFAIL, eError, "FAIL: Could not instantiate APO object.");
        return (FNS_PASS);
    }
    
    if (pCurrentDevice->IsProxyApo())
    {
        XLOG(
            XSKIP,
            eInfo1,
            "Skip: Testcase should not run on Proxy APO.");
        return (FNS_SKIPPED);
    }

    if (!pCurrentDevice->GetAPOInterfaces(&pIAPO, &pIAPORT, &pIAPOConfig))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObject interface.");
        return (FNS_PASS);
    }

    hr = pIAPO->QueryInterface(__uuidof(IAudioRateConvert), (void**)&pISRC);

    if (E_NOINTERFACE == hr)
    {
        SLOG(eInfo1, "This APO does not implement IAudioRateConvert.");
        return (FNS_PASS);
    }
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: QueryInterface for IAudioRateConvert returns 0x%08lx.",
            hr);
        return (FNS_PASS);
    }

    // Create an uninitialized APO for additional testing
    
    pCurrentDevice->GetClsID(&guidAPO);

    //CoCreate a fresh instance of the APO under test
    hr = pFreshAPO.CoCreateInstance(guidAPO);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
            eError,
            "ERROR: Unable to CoCreate an instance of the APO under test (0x%08X)",
            hr);

        // Can't go any further
        return(FNS_PASS);
    }

    // Get the interface to test    
    hr = pFreshAPO->QueryInterface(__uuidof(IAudioRateConvert), (void**)&pFreshSRC);
    if(S_OK != hr)
    {
        SLOG(
            eInfo1,
            "INFO: This device does not support IAudioRateConvert (QI returned 0x%08X)",
            hr);

        // we can't go any further
        return (FNS_PASS);
    }

    // uninitialized case
    SLOG(eInfo1, "Testing IAudioRateConvert::SetInputFrameRate (uninitialized)");
    hr = pFreshSRC->SetInputFrameRate(gAPOLimits.Rate_fInputFrameRate);
    if(APOERR_NOT_INITIALIZED != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: IAudioRateConvert::SetInputFrameRate returned 0x%08X (expected APOERR_NOT_INITIALIZED)",
            hr);
    }                

    //
    // Set the rate to its current value
    //
    SLOG(eInfo1, "Testing IAudioRateConvert::SetInputFrameRate (no change)");
    hr = pISRC->SetInputFrameRate(gAPOLimits.Rate_fInputFrameRate);
    if(S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: IAudioRateConvert::SetInputFrameRate returned 0x%08X (expected S_OK)",
            hr);
    }

    //
    // Adjust the rate up
    //
    SLOG(eInfo1, "Testing IAudioRateConvert::SetInputFrameRate (adjust to original + 0.5f)");    
    hr = pISRC->SetInputFrameRate(gAPOLimits.Rate_fInputFrameRate + 0.5f);
    if(S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: IAudioRateConvert::SetInputFrameRate returned 0x%08X (expected S_OK)",
            hr);
    }

    //
    // Adjust the rate down
    //
    SLOG(eInfo1, "Testing IAudioRateConvert::SetInputFrameRate (adjust to original - 0.5f)");    
    hr = pISRC->SetInputFrameRate(gAPOLimits.Rate_fInputFrameRate - 0.5f);
    if(S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: IAudioRateConvert::SetInputFrameRate returned 0x%08X (expected S_OK)",
            hr);
    }

    //
    // Set the rate slightly below the min allowed value
    //
    SLOG(eInfo1, "Testing IAudioRateConvert::SetInputFrameRate (min - 0.1f)");
    hr = pISRC->SetInputFrameRate(gAPOLimits.Rate_fMinInputFrameRate - 0.1f);
    if ( (E_INVALIDARG != hr) && (AEERR_APO_INVALIDARG != hr) )
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: IAudioRateConvert::SetInputFrameRate returned 0x%08X (expected E_INVALIDARG or AEEERR_APO_INVALIDARG)",
            hr);
    }

    //
    // Set the rate slightly above the max allowed value
    //
    SLOG(eInfo1, "Testing IAudioRateConvert::SetInputFrameRate (max + 0.1f)");
    hr = pISRC->SetInputFrameRate(gAPOLimits.Rate_fMaxInputFrameRate + 0.1f);
    if ( (E_INVALIDARG != hr) && (AEERR_APO_INVALIDARG != hr) )
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: IAudioRateConvert::SetInputFrameRate returned 0x%08X (expected E_INVALIDARG or AEEERR_APO_INVALIDARG)",
            hr);
    }

    // Set the rate below zero
    hr = pISRC->SetInputFrameRate(-1.0f);
    if ( (E_INVALIDARG != hr) && (AEERR_APO_INVALIDARG != hr) )
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: IAudioRateConvert::SetInputFrameRate returned 0x%08X (expected E_INVALIDARG or AEEERR_APO_INVALIDARG)",
            hr);
    }
    
    return (FNS_PASS);
}   //   TC_IAudioRateConvert_SetInputFrameRate()

DWORD WINAPI TC_IAudioRateConvert_SetOutputFrameRate()
{
    CAPODevice                                     *pCurrentDevice = NULL;
    GUID                                            guidAPO = GUID_NULL;
    CComPtr<IAudioProcessingObject>                 pIAPO = NULL;
    CComPtr<IAudioProcessingObjectRT>               pIAPORT = NULL;
    CComPtr<IAudioProcessingObjectConfiguration>    pIAPOConfig = NULL;
    CComPtr<IAudioRateConvert>                      pISRC = NULL;
    CComPtr<IAudioProcessingObject>                 pFreshAPO = NULL;
    CComPtr<IAudioRateConvert>                      pFreshSRC = NULL;
    HRESULT                                         hr = S_OK;
    
    pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    
    if (NULL == pCurrentDevice)
    {
        XLOG(XFAIL, eError, "FAIL: Could not instantiate APO object.");
        return (FNS_PASS);
    }
    
    if (pCurrentDevice->IsProxyApo())
    {
        XLOG(
            XSKIP,
            eInfo1,
            "Skip: Testcase should not run on Proxy APO.");
        return (FNS_SKIPPED);
    }

    if (!pCurrentDevice->GetAPOInterfaces(&pIAPO, &pIAPORT, &pIAPOConfig))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObject interface.");
        return (FNS_PASS);
    }

    hr = pIAPO->QueryInterface(__uuidof(IAudioRateConvert), (void**)&pISRC);

    if (E_NOINTERFACE == hr)
    {
        SLOG(eInfo1, "This APO does not implement IAudioRateConvert.");
        return (FNS_PASS);
    }
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: QueryInterface for IAudioRateConvert returns 0x%08lx.",
            hr);
        return (FNS_PASS);
    }

    // Create an uninitialized APO for additional testing
    
    pCurrentDevice->GetClsID(&guidAPO);

    //CoCreate a fresh instance of the APO under test
    hr = pFreshAPO.CoCreateInstance(guidAPO);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
            eError,
            "ERROR: Unable to CoCreate an instance of the APO under test (0x%08X)",
            hr);

        // Can't go any further
        return(FNS_PASS);
    }

    // Get the interface to test    
    hr = pFreshAPO->QueryInterface(__uuidof(IAudioRateConvert), (void**)&pFreshSRC);
    if(S_OK != hr)
    {
        SLOG(
            eInfo1,
            "INFO: This device does not support IAudioRateConvert (QI returned 0x%08X)",
            hr);

        // we can't go any further
        return (FNS_PASS);
    }

    // uninitialized case
    SLOG(eInfo1, "Testing IAudioRateConvert::SetOutputFrameRate (uninitialized)");
    hr = pFreshSRC->SetOutputFrameRate(gAPOLimits.Rate_fInputFrameRate);
    if(APOERR_NOT_INITIALIZED != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: IAudioRateConvert::SetOutputFrameRate returned 0x%08X (expected APOERR_NOT_INITIALIZED)",
            hr);
    }                

    //
    // Set the rate to its current value
    //
    SLOG(eInfo1, "Testing IAudioRateConvert::SetOutputFrameRate (no change)");
    hr = pISRC->SetOutputFrameRate(gAPOLimits.Rate_fOutputFrameRate);
    if(S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: IAudioRateConvert::SetOutputFrameRate returned 0x%08X (expected S_OK)",
            hr);
    }

    //
    // Adjust the rate up
    //
    SLOG(eInfo1, "Testing IAudioRateConvert::SetOutputFrameRate (adjust to original + 0.5f)");    
    hr = pISRC->SetOutputFrameRate(gAPOLimits.Rate_fOutputFrameRate + 0.5f);
    if(S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: IAudioRateConvert::SetOutputFrameRate returned 0x%08X (expected S_OK)",
            hr);
    }

    //
    // Adjust the rate down
    //
    SLOG(eInfo1, "Testing IAudioRateConvert::SetOutputFrameRate (adjust to original - 0.5f)");    
    hr = pISRC->SetOutputFrameRate(gAPOLimits.Rate_fOutputFrameRate - 0.5f);
    if(S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: IAudioRateConvert::SetOutputFrameRate returned 0x%08X (expected S_OK)",
            hr);
    }

    //
    // Set the rate slightly below the min allowed value
    //
    SLOG(eInfo1, "Testing IAudioRateConvert::SetOutputFrameRate (min - 0.1f)");
    hr = pISRC->SetOutputFrameRate(gAPOLimits.Rate_fMinOutputFrameRate - 0.1f);
    if ( (E_INVALIDARG != hr) && (AEERR_APO_INVALIDARG != hr) )
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: IAudioRateConvert::SetOutputFrameRate returned 0x%08X (expected E_INVALIDARG or AEEERR_APO_INVALIDARG)",
            hr);
    }

    SLOG(eInfo1, "Testing IAudioRateConvert::SetOutputFrameRate (min - 0.1f)");
    hr = pISRC->SetOutputFrameRate(gAPOLimits.Rate_fMinOutputFrameRate - 0.1f);
    if ( (E_INVALIDARG != hr) && (AEERR_APO_INVALIDARG != hr) )
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: IAudioRateConvert::SetOutputFrameRate returned 0x%08X (expected E_INVALIDARG or AEEERR_APO_INVALIDARG)",
            hr);
    }

    //
    // Set the rate slightly above the max allowed value
    //
    SLOG(eInfo1, "Testing IAudioRateConvert::SetOutputFrameRate (max + 0.1f)");
    hr = pISRC->SetOutputFrameRate(gAPOLimits.Rate_fMaxOutputFrameRate + 0.1f);
    if ( (E_INVALIDARG != hr) && (AEERR_APO_INVALIDARG != hr) )
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: IAudioRateConvert::SetOutputFrameRate returned 0x%08X (expected E_INVALIDARG or AEEERR_APO_INVALIDARG)",
            hr);
    }

    // Set the rate below zero
    hr = pISRC->SetOutputFrameRate(-1.0f);
    if ( (E_INVALIDARG != hr) && (AEERR_APO_INVALIDARG != hr) )
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: IAudioRateConvert::SetOutputFrameRate returned 0x%08X (expected E_INVALIDARG or AEEERR_APO_INVALIDARG)",
            hr);
    }
    
    return (FNS_PASS);
}   //   TC_IAudioRateConvert_SetOutputFrameRate()

DWORD WINAPI TC_IAudioRateConvertRT_GetInputFrameRate()
{
    CAPODevice                                     *pCurrentDevice = NULL;
    GUID                                            guidAPO;
    AUDIO_RATE_CONVERT_PARAMS                       APOParams = {0};
    CComPtr<IAudioProcessingObject>                 pIAPO = NULL;
    CComPtr<IAudioProcessingObjectRT>               pIAPORT = NULL;
    CComPtr<IAudioProcessingObjectConfiguration>    pIAPOConfig = NULL;
    CComPtr<IAudioRateConvert>                      pISRC = NULL;
    CComPtr<IAudioRateConvertRT>                    pISRC_RT = NULL;
    CComPtr<IAudioProcessingObject>                 pFreshAPO = NULL;

    HRESULT                                         hr = S_OK;
    FLOAT                                           fPreviousInputRate = 0.0f;
    FLOAT                                           fCurrentInputRate = 0.0f;
    
    pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    if (NULL == pCurrentDevice)
    {
        XLOG(XFAIL, eError, "FAIL: Could not instantiate APO object.");
        return (FNS_PASS);
    }
   
    if (pCurrentDevice->IsProxyApo())
    {
        XLOG(
            XSKIP,
            eInfo1,
            "Skip: Testcase should not run on Proxy APO.");
        return (FNS_SKIPPED);
    }

    if (!pCurrentDevice->GetAPOInterfaces(&pIAPO, &pIAPORT, &pIAPOConfig))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObject interface.");
        return (FNS_PASS);
    }       

    pCurrentDevice->GetClsID(&guidAPO);

    //CoCreate a fresh instance of the APO under test
    hr = pFreshAPO.CoCreateInstance(guidAPO);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
            eError,
            "ERROR: Unable to CoCreate and instance of the APO under test (0x%08X)",
            hr);

        // Can't go any further
        return(FNS_PASS);
    }

    // Get the interface to test    
    hr = pFreshAPO->QueryInterface(__uuidof(IAudioRateConvertRT), (void**)&pISRC_RT);
    if(S_OK != hr)
    {
        SLOG(
            eInfo1,
            "INFO: This device does not support IAudioRateConvertRT (QI returned 0x%08X)",
            hr);

        // we can't go any further
        return (FNS_PASS);
    }

    // Get IAudioRateConvert
    hr = pFreshAPO->QueryInterface(__uuidof(IAudioRateConvert), (void**)&pISRC);
    if(S_OK != hr)
    {
        SLOG(
            eInfo1,
            "INFO: This device does not support IAudioRateConvert (QI returned 0x%08X)",
            hr);

        // we can't go any further
        return (FNS_PASS);
    }

    // Set the APO init params to their default values
    APOParams.APOInit.cbSize = sizeof(APOParams);
    APOParams.APOInit.clsid  = guidAPO;
    APOParams.fInputFrameRate         = gAPOLimits.Rate_fInputFrameRate;
    APOParams.fOutputFrameRate        = gAPOLimits.Rate_fOutputFrameRate;
    APOParams.fMinInputFrameRate      = gAPOLimits.Rate_fMinInputFrameRate;
    APOParams.fMaxInputFrameRate      = gAPOLimits.Rate_fMaxInputFrameRate;
    APOParams.fMinOutputFrameRate     = gAPOLimits.Rate_fMinOutputFrameRate;
    APOParams.fMaxOutputFrameRate     = gAPOLimits.Rate_fMaxOutputFrameRate;
    APOParams.eAlgorithmType          = gAPOLimits.Rate_eAlgorithmType;
    APOParams.eFlowType               = gAPOLimits.Rate_eFlowType;
    APOParams.u32SamplesPerFrame      = gAPOLimits.Rate_u32SamplesPerFrame;
    APOParams.u32HighQualityTaplength = gAPOLimits.Rate_u32HighQualityTaplength;

    // Initialize the APO
    hr = pFreshAPO->Initialize(sizeof(APOParams), (BYTE*)&APOParams);
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not initialize IAudioProcessingObject with default init params (0x%08X)",
            hr);

        // we can't go any further
        return (FNS_PASS);
    }   

    //
    // Verify that the correct initial input frame rate is reported
    //
    SLOG(eInfo1, "Testing IAudioRateConvertRT::GetInputFrameRate (initial value)");
    fCurrentInputRate = pISRC_RT->GetInputFrameRate();

    if(gAPOLimits.Rate_fInputFrameRate != fCurrentInputRate)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Expected initial input frame rate of %f, observed %f",
            gAPOLimits.Rate_fInputFrameRate,
            fCurrentInputRate);        
    }

    fPreviousInputRate = fCurrentInputRate;
    

    //
    // Set the rate to its current value
    //
    SLOG(eInfo1, "Testing IAudioRateConvert::GetInputFrameRate (no change)");
    hr = pISRC->SetInputFrameRate(fPreviousInputRate);
    if(S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: IAudioRateConvert::SetInputFrameRate returned 0x%08X (expected S_OK)",
            hr);
    }

    // get the frame rate again
    fCurrentInputRate = pISRC_RT->GetInputFrameRate();    

    // make sure nothing changed
    if(fPreviousInputRate != fCurrentInputRate)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Set (%f) and retrieved (%f) frame rates do not match!",
            fPreviousInputRate,
            fCurrentInputRate);            
    }
    
    //
    // Verify that the rate doesn't change with an invalid set call
    //
    SLOG(eInfo1, "Testing IAudioRateConvert::GetInputFrameRate (invalid)");
    hr = pISRC->SetInputFrameRate(-1.0f);
    if ( (E_INVALIDARG != hr) && (AEERR_APO_INVALIDARG != hr) )
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: IAudioRateConvert::SetInputFrameRate returned 0x%08X (expected S_OK)",
            hr);
    }

    // make sure things didn't change...
    fCurrentInputRate = pISRC_RT->GetInputFrameRate();    

    // make sure nothing changed
    if(fPreviousInputRate != fCurrentInputRate)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Set (%f) and retrieved (%f) frame rates do not match!",
            fPreviousInputRate,
            fCurrentInputRate);            
    }
    
    return (FNS_PASS);
}   //   TC_IAudioRateConvertRT_GetInputFrameRate()

DWORD WINAPI TC_IAudioRateConvertRT_GetOutputFrameRateRT()
{
    CAPODevice                                     *pCurrentDevice = NULL;
    GUID                                            guidAPO;
    AUDIO_RATE_CONVERT_PARAMS                       APOParams = {0};
    CComPtr<IAudioProcessingObject>                 pIAPO = NULL;
    CComPtr<IAudioProcessingObjectRT>               pIAPORT = NULL;
    CComPtr<IAudioProcessingObjectConfiguration>    pIAPOConfig = NULL;
    CComPtr<IAudioRateConvert>                      pISRC = NULL;
    CComPtr<IAudioRateConvertRT>                    pISRC_RT = NULL;
    CComPtr<IAudioProcessingObject>                 pFreshAPO = NULL;

    HRESULT                                         hr = S_OK;
    FLOAT                                           fPreviousOutputRate = 0.0f;
    FLOAT                                           fCurrentOutputRate = 0.0f;
    
    pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    if (NULL == pCurrentDevice)
    {
        XLOG(XFAIL, eError, "FAIL: Could not instantiate APO object.");
        return (FNS_PASS);
    }
    
    if (pCurrentDevice->IsProxyApo())
    {
        XLOG(
            XSKIP,
            eInfo1,
            "Skip: Testcase should not run on Proxy APO.");
        return (FNS_SKIPPED);
    }

    if (!pCurrentDevice->GetAPOInterfaces(&pIAPO, &pIAPORT, &pIAPOConfig))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObject interface.");
        return (FNS_PASS);
    }       

    pCurrentDevice->GetClsID(&guidAPO);

    //CoCreate a fresh instance of the APO under test
    hr = pFreshAPO.CoCreateInstance(guidAPO);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
            eError,
            "ERROR: Unable to CoCreate and instance of the APO under test (0x%08X)",
            hr);

        // Can't go any further
        return(FNS_PASS);
    }

    // Get the interface to test    
    hr = pFreshAPO->QueryInterface(__uuidof(IAudioRateConvertRT), (void**)&pISRC_RT);
    if(S_OK != hr)
    {
        SLOG(
            eInfo1,
            "INFO: This device does not support IAudioRateConvertRT (QI returned 0x%08X)",
            hr);

        // we can't go any further
        return (FNS_PASS);
    }

    // Get IAudioRateConvert
    hr = pFreshAPO->QueryInterface(__uuidof(IAudioRateConvert), (void**)&pISRC);
    if(S_OK != hr)
    {
        SLOG(
            eInfo1,
            "INFO: This device does not support IAudioRateConvert (QI returned 0x%08X)",
            hr);

        // we can't go any further
        return (FNS_PASS);
    }

    // Set the APO init params to their default values
    APOParams.APOInit.cbSize = sizeof(APOParams);
    APOParams.APOInit.clsid  = guidAPO;
    APOParams.fInputFrameRate         = gAPOLimits.Rate_fInputFrameRate;
    APOParams.fOutputFrameRate        = gAPOLimits.Rate_fOutputFrameRate;
    APOParams.fMinInputFrameRate      = gAPOLimits.Rate_fMinInputFrameRate;
    APOParams.fMaxInputFrameRate      = gAPOLimits.Rate_fMaxInputFrameRate;
    APOParams.fMinOutputFrameRate     = gAPOLimits.Rate_fMinOutputFrameRate;
    APOParams.fMaxOutputFrameRate     = gAPOLimits.Rate_fMaxOutputFrameRate;
    APOParams.eAlgorithmType          = gAPOLimits.Rate_eAlgorithmType;
    APOParams.eFlowType               = gAPOLimits.Rate_eFlowType;
    APOParams.u32SamplesPerFrame      = gAPOLimits.Rate_u32SamplesPerFrame;
    APOParams.u32HighQualityTaplength = gAPOLimits.Rate_u32HighQualityTaplength;

    // Initialize the APO
    hr = pFreshAPO->Initialize(sizeof(APOParams), (BYTE*)&APOParams);
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not initialize IAudioProcessingObject with default init params (0x%08X)",
            hr);

        // we can't go any further
        return (FNS_PASS);
    }   

    //
    // Verify that the correct initial output frame rate is reported
    //
    SLOG(eInfo1, "Testing IAudioRateConvertRT::GetOutputFrameRate (initial value)");
    fCurrentOutputRate = pISRC_RT->GetOutputFrameRate();

    if(gAPOLimits.Rate_fOutputFrameRate != fCurrentOutputRate)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Expected initial output frame rate of %f, observed %f",
            gAPOLimits.Rate_fOutputFrameRate,
            fCurrentOutputRate);        
    }

    fPreviousOutputRate = fCurrentOutputRate;
    

    //
    // Set the rate to its current value
    //
    SLOG(eInfo1, "Testing IAudioRateConvert::GetOutputFrameRate (no change)");
    hr = pISRC->SetOutputFrameRate(fPreviousOutputRate);
    if(S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: IAudioRateConvert::SetOutputFrameRate returned 0x%08X (expected S_OK)",
            hr);
    }

    // get the frame rate again
    fCurrentOutputRate = pISRC_RT->GetOutputFrameRate();    

    // make sure nothing changed
    if(fPreviousOutputRate != fCurrentOutputRate)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Set (%f) and retrieved (%f) frame rates do not match!",
            fPreviousOutputRate,
            fCurrentOutputRate);            
    }
    
    //
    // Verify that the rate doesn't change with an invalid set call
    //
    SLOG(eInfo1, "Testing IAudioRateConvert::GetOutputFrameRate (invalid)");
    hr = pISRC->SetOutputFrameRate(-1.0f);
    if ( (E_INVALIDARG != hr) && (AEERR_APO_INVALIDARG != hr) )
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: IAudioRateConvert::SetInputFrameRate returned 0x%08X (expected S_OK)",
            hr);
    }

    // make sure things didn't change...
    fCurrentOutputRate = pISRC_RT->GetOutputFrameRate();    

    // make sure nothing changed
    if(fPreviousOutputRate != fCurrentOutputRate)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Set (%f) and retrieved (%f) frame rates do not match!",
            fPreviousOutputRate,
            fCurrentOutputRate);            
    }
    
    return (FNS_PASS);
}   //   TC_IAudioRateConvertRT_GetOutputFrameRateRT()

DWORD WINAPI TC_IAudioProcessingObjectVBR_CalcMaxInputFrames()
{
    CAPODevice                                     *pCurrentDevice = NULL;
    GUID                                            guidAPO;
    CComPtr<IAudioProcessingObject>                 pIAPO = NULL;
    CComPtr<IAudioProcessingObjectRT>               pIAPORT = NULL;
    CComPtr<IAudioProcessingObjectConfiguration>    pIAPOConfig = NULL;
    CComPtr<IAudioRateConvert>                      pISRC = NULL;
    CComPtr<IAudioRateConvertRT>                    pISRC_RT = NULL;
    CComPtr<IAudioProcessingObjectVBR>              pIAPO_VBR = NULL;
    CComPtr<IAudioProcessingObject>                 pFreshAPO = NULL;
    CComPtr<IAudioProcessingObjectVBR>              pFreshAPO_VBR = NULL;
    HRESULT                                         hr;
    UINT32                                          nMaxInputFrames = 0;

    
    pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    
    if (NULL == pCurrentDevice)
    {
        XLOG(XFAIL, eError, "FAIL: Could not instantiate APO object.");
        return (FNS_PASS);
    }
    
    if (pCurrentDevice->IsProxyApo())
    {
        XLOG(
            XSKIP,
            eInfo1,
            "Skip: Testcase should not run on Proxy APO.");
        return (FNS_SKIPPED);
    }

    if (!pCurrentDevice->GetAPOInterfaces(&pIAPO, &pIAPORT, &pIAPOConfig))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObject interface.");
        return (FNS_PASS);
    }

    hr = pIAPO->QueryInterface(__uuidof(IAudioProcessingObjectVBR), (void**)&pIAPO_VBR);

    if (E_NOINTERFACE == hr)
    {
        SLOG(eInfo1, "This APO does not implement IAudioProcessingObjectVBR.");
        return (FNS_PASS);
    }
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: QueryInterface for IAudioProcessingObjectVBR returns 0x%08lx.",
            hr);
        return (FNS_PASS);
    }

    // allocate a new APO to tets the uninitialized case
    pCurrentDevice->GetClsID(&guidAPO);

    hr = pFreshAPO.CoCreateInstance(guidAPO);
    if(FAILED(hr))
    {
        if(FAILED(hr))
        {
            XLOG(XFAIL, 
                eError, 
                "ERROR: Unable to CoCreate a new instance of the APO under test! (hr = 0x%08X)",
                hr);

            // can't go any further
            return (FNS_PASS);
        }
    }

    // QI for another IAudioProcessingObjectVBR interface
    hr = pFreshAPO->QueryInterface(__uuidof(IAudioProcessingObjectVBR),
                                   (void**)&pFreshAPO_VBR);
    if (E_NOINTERFACE == hr)
    {
        SLOG(eInfo1, "This APO does not implement IAudioProcessingObjectVBR.");
        return (FNS_PASS);
    }
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: QueryInterface for IAudioProcessingObjectVBR returns 0x%08lx.",
            hr);
        return (FNS_PASS);
    }

    // uninitialized case
    SLOG(eInfo1, "Testing IAudioProcessingObjectVBR::CalcMaxInputFrames (uninitialized)...");
    hr = pFreshAPO_VBR->CalcMaxInputFrames(1000, &nMaxInputFrames);
    if(APOERR_NOT_INITIALIZED != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: IAudioProcessingObjectVBR::CalcMaxInputFrames returned 0x%08X (expected APOERR_NOT_INITIALIZED)",
            hr);                    
    }


    // 0 output frame case
    SLOG(eInfo1, "Testing IAudioProcessingObjectVBR::CalcMaxInputFrames w/ 0 input frame count...");
    hr = pIAPO_VBR->CalcMaxInputFrames(0, &nMaxInputFrames);
    if((AEERR_APO_INVALIDARG != hr) && (E_INVALIDARG != hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: IAudioProcessingObjectVBR::CalcMaxInputFrames should have returned  AEERR_APO_INVALIDARG or E_INVALIDARG (hr = 0x%08X)",
             hr);
    }

    // NULL param case
    SLOG(eInfo1, "Testing IAudioProcessingObjectVBR w/ 0 NULL output param...");   
#pragma prefast(suppress:6309, "Disable parameter checking")
    hr = pIAPO_VBR->CalcMaxInputFrames(1000, NULL);    
    if(E_POINTER != hr)
    {
        XLOG(XFAIL,
             eError,
             "FAIL: IAudioProcessingObjectVBR::CalcMaxInputFrames should have returned  AEERR_APO_INVALIDARG or E_INVALIDARG (hr = 0x%08X)",
             hr);
    }

    // the resampler is tested elsewhere...
    
    return (FNS_PASS);
}   //   TC_IAudioProcessingObjectVBR_CalcMaxInputFrames()

DWORD WINAPI TC_IAudioProcessingObjectVBR_CalcMaxOutputFrames()
{
    CAPODevice                                     *pCurrentDevice = NULL;
    GUID                                            guidAPO;
    CComPtr<IAudioProcessingObject>                 pIAPO = NULL;
    CComPtr<IAudioProcessingObjectRT>               pIAPORT = NULL;
    CComPtr<IAudioProcessingObjectConfiguration>    pIAPOConfig = NULL;
    CComPtr<IAudioRateConvert>                      pISRC = NULL;
    CComPtr<IAudioRateConvertRT>                    pISRC_RT = NULL;
    CComPtr<IAudioProcessingObjectVBR>              pIAPO_VBR = NULL;
    CComPtr<IAudioProcessingObject>                 pFreshAPO = NULL;
    CComPtr<IAudioProcessingObjectVBR>              pFreshAPO_VBR = NULL;

    HRESULT                                         hr;
    UINT32                                          nMaxOutputFrames = 0;

    
    pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    
    if (NULL == pCurrentDevice)
    {
        XLOG(XFAIL, eError, "FAIL: Could not instantiate APO object.");
        return (FNS_PASS);
    }
    
    if (pCurrentDevice->IsProxyApo())
    {
        XLOG(
            XSKIP,
            eInfo1,
            "Skip: Testcase should not run on Proxy APO.");
        return (FNS_SKIPPED);
    }

    if (!pCurrentDevice->GetAPOInterfaces(&pIAPO, &pIAPORT, &pIAPOConfig))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObject interface.");
        return (FNS_PASS);
    }

    hr = pIAPO->QueryInterface(__uuidof(IAudioProcessingObjectVBR), (void**)&pIAPO_VBR);

    if (E_NOINTERFACE == hr)
    {
        SLOG(eInfo1, "This APO does not implement IAudioProcessingObjectVBR.");
        return (FNS_PASS);
    }
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: QueryInterface for IAudioProcessingObjectVBR returns 0x%08lx.",
            hr);
        return (FNS_PASS);
    }

    // allocate a new APO to tets the uninitialized case
    pCurrentDevice->GetClsID(&guidAPO);

    hr = pFreshAPO.CoCreateInstance(guidAPO);
    if(FAILED(hr))
    {
        if(FAILED(hr))
        {
            XLOG(XFAIL, 
                eError, 
                "ERROR: Unable to CoCreate a new instance of the APO under test! (hr = 0x%08X)",
                hr);

            // can't go any further
            return (FNS_PASS);
        }
    }

    // QI for another IAudioProcessingObjectVBR interface
    hr = pFreshAPO->QueryInterface(__uuidof(IAudioProcessingObjectVBR),
                                   (void**)&pFreshAPO_VBR);
    if (E_NOINTERFACE == hr)
    {
        SLOG(eInfo1, "This APO does not implement IAudioProcessingObjectVBR.");
        return (FNS_PASS);
    }
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: QueryInterface for IAudioProcessingObjectVBR returns 0x%08lx.",
            hr);
        return (FNS_PASS);
    }

    // uninitialized case
    SLOG(eInfo1, "Testing IAudioProcessingObjectVBR::CalcMaxOutputFrames (uninitialized)...");
    hr = pFreshAPO_VBR->CalcMaxOutputFrames(1000, &nMaxOutputFrames);
    if(APOERR_NOT_INITIALIZED != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: IAudioProcessingObjectVBR::CalcMaxOutputFrames returned 0x%08X (expected APOERR_NOT_INITIALIZED)",
            hr);                    
    }

    // NULL param case 
    SLOG(eInfo1, "Testing IAudioProcessingObjectVBR::CalcMaxOutputFrames w/ NULL output param...");   
#pragma prefast(suppress:6309, "Disable parameter checking")
    hr = pIAPO_VBR->CalcMaxOutputFrames(1000, NULL);
    if((AEERR_APO_INVALIDARG != hr) && (E_POINTER != hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: IAudioProcessingObjectVBR::CalcMaxOutputFrames should have returned  AEERR_APO_INVALIDARG or E_INVALIDARG (hr = 0x%08X)",
             hr);
    }

    // the resampler is tested elsewhere...
    
    return (FNS_PASS);
}   //   TC_IAudioProcessingObjectVBR_CalcMaxOutputFrames()

HRESULT RateConverterFunctionalTest(LPCGUID pguidAPOUnderTest,
                                    const WAVEFORMATEXTENSIBLE* pwfxInputFormat,
                                    const WAVEFORMATEXTENSIBLE* pwfxOutputFormat,
                                    UINT32 nMillisInputBufferDuration,
                                    AUDIO_RATE_CONVERT_PARAMS sAPOInitParams,
                                    FLOAT32 fRequestedDynamicInputFrameRate,
                                    FLOAT32 fRequestedDynamicOutputFrameRate,
                                    DOUBLE dTestToneFrequency,
                                    DOUBLE dTestToneAmplitude,
                                    PBYTE* ppProcessorOutput,
                                    size_t* pcbProcessorOutput,
                                    UINT32* pnProcessorOutputFrames)
{
    HRESULT hr = S_OK;

    //
    // Standard APO Interfaces
    //
    CComPtr<IAudioProcessingObject>                 spAudioProcessingObject = NULL;
    CComPtr<IAudioProcessingObjectRT>               spAudioProcessingObjectRT = NULL;
    CComPtr<IAudioProcessingObjectConfiguration>    spAudioProcessingObjectConfiguration = NULL;
    CComPtr<IAudioProcessingObjectVBR>              spAudioProcessingObjectVBR = NULL;

    //
    // SRC APO Specific Interfaces
    //
    CComPtr<IAudioRateConvert>                      spAudioRateConvert = NULL;
    CComPtr<IAudioRateConvertRT>                    spAudioRateConvertRT = NULL;

    // Input and Output Formats
    CComPtr<IAudioMediaType>                        spInputFormatType;
    CComPtr<IAudioMediaType>                        spRequestedInputFormatType;

    CComPtr<IAudioMediaType>                        spOutputFormatType;
    CComPtr<IAudioMediaType>                        spRequestedOutputFormatType;
    
    //
    // Input and Output Connect Descriptors
    //
    APO_CONNECTION_DESCRIPTOR*                      pInputConnections = NULL;
    APO_CONNECTION_DESCRIPTOR*                      pOutputConnections = NULL;
    
    //
    // Input and Output Connection Properties
    //
    APO_CONNECTION_PROPERTY*                        pInputProperties = NULL;
    APO_CONNECTION_PROPERTY*                        pOutputProperties = NULL;

    //
    // Max Frame Counts
    //
    UINT32                                          nMaxInputFrameCount = NULL;
    UINT32                                          nMaxOutputFrameCount = NULL; 

    //
    // Expected Output Frame Count
    //
    UINT32                                          nOutputFrameCount = 0;

    //
    // Buffer Sizes
    //
    size_t                                          cbInputBufferSize = 0;
    size_t                                          cbOutputBufferSize = 0;

    //
    // Input/Output Buffers
    //
    PBYTE                                           pInputBuffer = NULL;
    PBYTE                                           pOutputBuffer = NULL;

    //
    // Logger
    // 
    CComPtr<IBasicLog> pBasicLog;    

    // Validate params
    if((NULL == pguidAPOUnderTest) ||
       (NULL == pwfxInputFormat) ||
       (NULL == pwfxOutputFormat) ||
       (NULL == ppProcessorOutput) ||
       (NULL == pcbProcessorOutput) ||
       (NULL == pnProcessorOutputFrames))
    {
        hr = E_POINTER;
        goto Exit;
    }

    // Co-Create the APO under test
    hr = spAudioProcessingObject.CoCreateInstance(*pguidAPOUnderTest);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
            eError,
            "FAIL: Unable to CoCreate the APO under test (0x%08X)",
            hr);
        goto Exit;
    }

    // QI for all the relevant interfaces supported by this COM object

    hr = spAudioProcessingObject->QueryInterface(__uuidof(IAudioProcessingObjectRT),
                                                 (void**)(&spAudioProcessingObjectRT));
    if(FAILED(hr))
    {
        XLOG(XFAIL,
            eError,
            "FAIL: Unable to QI the APO under test for IAudioProcessingObjectRT (0x%08X)",
            hr);
        goto Exit;
    }
   
    hr = spAudioProcessingObject->QueryInterface(__uuidof(IAudioProcessingObjectConfiguration),
                                                 (void**)(&spAudioProcessingObjectConfiguration));
    if(FAILED(hr))
    {
        XLOG(XFAIL,
            eError,
            "FAIL: Unable to QI the APO under test for IAudioProcessingObjectConfiguration (0x%08X)",
            hr);
        goto Exit;
    }

    hr = spAudioProcessingObject->QueryInterface(__uuidof(IAudioProcessingObjectVBR),
                                                 (void**)(&spAudioProcessingObjectVBR));
    if(FAILED(hr))
    {
        XLOG(XFAIL,
            eError,
            "FAIL: Unable to QI the APO under test for IAudioProcessingObjectVBR (0x%08X)",
            hr);
        goto Exit;
    }

    hr = spAudioProcessingObject->QueryInterface(__uuidof(IAudioRateConvert),
                                                 (void**)(&spAudioRateConvert));
    if(FAILED(hr))
    {
        XLOG(XFAIL,
            eError,
            "FAIL: Unable to QI the APO under test for IAudioRateConvert (0x%08X)",
            hr);
        goto Exit;
    }

    hr = spAudioProcessingObject->QueryInterface(__uuidof(IAudioRateConvertRT),
                                                 (void**)(&spAudioRateConvertRT));
    if(FAILED(hr))
    {
        XLOG(XFAIL,
            eError,
            "FAIL: Unable to QI the APO under test for IAudioRateConvertRT (0x%08X)",
            hr);
        goto Exit;
    }

    // Initialize the APO using the given parameter structure

    SLOG(eInfo1,
        "Initializing the CMPT Format Converter APO with the following parameters:");
    SLOG(eInfo1,
        "Input Frame Rate: %.1f Hz",
        sAPOInitParams.fInputFrameRate);
    SLOG(eInfo1,
        "Output Frame Rate: %.1f Hz",
        sAPOInitParams.fOutputFrameRate);
    SLOG(eInfo1,
        "Minimum Input Frame Rate: %.1f Hz",
        sAPOInitParams.fMinInputFrameRate);
    SLOG(eInfo1,
        "Maximum Input Frame Rate: %.1f Hz",
        sAPOInitParams.fMaxInputFrameRate);
    SLOG(eInfo1,
        "Minimum Output Frame Rate: %.1f Hz",
        sAPOInitParams.fMinOutputFrameRate);
    SLOG(eInfo1,
        "Maximum Output Frame Rate: %.1f Hz",
        sAPOInitParams.fMaxOutputFrameRate);
    
    hr = spAudioProcessingObject->Initialize(sizeof(sAPOInitParams),
                                             (PBYTE)(&sAPOInitParams));
    if(FAILED(hr))
    {
        XLOG(XFAIL,
            eError,
            "FAIL: Unable to initialize the APO under test: 0x%08X",
            hr);        
    }

    // Allocate space for input and ouput connections and properties
    if(NULL == (pInputConnections = new APO_CONNECTION_DESCRIPTOR[2]))
    {
        hr = E_OUTOFMEMORY;
        XLOG(XFAIL,
            eError,
            "FAIL: Out of memory!");
        goto Exit;
    }

    pOutputConnections = &(pInputConnections[1]); // Store input and output connenctions in the same buffer
        
    if(NULL == (pInputProperties = new APO_CONNECTION_PROPERTY[2]))
    {
        hr = E_OUTOFMEMORY;
        XLOG(XFAIL,
            eError,
            "FAIL: Out of memory!");
        goto Exit;
    }

    pOutputProperties = &(pInputProperties[1]); // Store input and output properties in the same buffer

    // Create IAudioMediaType from the given input and output formats and validate them.
    hr = CreateAudioMediaType((PWAVEFORMATEX)pwfxInputFormat,
                              sizeof(WAVEFORMATEXTENSIBLE),
                              &spRequestedInputFormatType);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
            eError,
            "FAIL: Unable to create IAudioMediaType interface from given input format (0x%08X)",
            hr);
        goto Exit;
    }

    hr = CreateAudioMediaType((PWAVEFORMATEX)pwfxOutputFormat,
                              sizeof(WAVEFORMATEXTENSIBLE),
                              &spRequestedOutputFormatType);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
            eError,
            "FAIL: Unable to create IAudioMediaType interface from given output format (0x%08X)",
            hr);
        goto Exit;
    }

    hr = spAudioProcessingObject->IsInputFormatSupported(spRequestedOutputFormatType,   // opposing format
                                                         spRequestedInputFormatType,    // requested format
                                                         &spInputFormatType);           // suggested format
    // accept only S_OK, as we want only exact matches
    if(S_OK != hr)
    {
        XLOG(XFAIL,
            eError,
            "FAIL: IAudioProcessingObject::IsInputFormatSupported did not return S_OK requested format (0x%08X)",
            hr);
        goto Exit;
    }

    hr = spAudioProcessingObject->IsOutputFormatSupported(spRequestedInputFormatType,    // opposing format
                                                          spRequestedOutputFormatType,   // requested format
                                                          &spOutputFormatType);          // suggested format
    // accept only S_OK, as we want only exact matches
    if(S_OK != hr)
    {
        XLOG(XFAIL,
            eError,
            "FAIL: IAudioProcessingObject::IsOutputFormatSupported did not return S_OK requested format (0x%08X)",
            hr);
        goto Exit;
    }

    // calculate the maximum input and output frame counts, as well as the corresponding buffer sizes

    // base the input buffer frame count off of the requested buffer duration
    nMaxInputFrameCount = (UINT32)(( ((FLOAT)(nMillisInputBufferDuration))/1000.0f ) * pwfxInputFormat->Format.nSamplesPerSec);
    cbInputBufferSize = nMaxInputFrameCount * pwfxInputFormat->Format.nBlockAlign;

    if(NULL == (pInputBuffer = new BYTE[cbInputBufferSize]))
    {
        hr = E_OUTOFMEMORY;
        XLOG(XFAIL,
            eError,
            "FAIL: Out of memory!");
        goto Exit;
    }

    SLOG(eInfo1,
        "Max Input Frame Count: %d (%d bytes)",
        nMaxInputFrameCount,
        cbInputBufferSize);


    // base the output buffer off of the worst-case considering the max/min input and output frame rates
    // supported by the SRC:
    // This is equal to (Max Input Frame Count) * ((Max Output Frame Rate) / (Min Input Frame Rate))
    nMaxOutputFrameCount = (UINT32)( (FLOAT32)(nMaxInputFrameCount) * 
                                   (  sAPOInitParams.fMaxOutputFrameRate) / (sAPOInitParams.fMinInputFrameRate) + 1) + 1;
    cbOutputBufferSize = nMaxOutputFrameCount * pwfxOutputFormat->Format.nBlockAlign;    

    if(NULL == (pOutputBuffer = new BYTE[cbOutputBufferSize]))
    {
        hr = E_OUTOFMEMORY;
        XLOG(XFAIL,
            eError,
            "FAIL: Out of memory!");
        goto Exit;
    }

    SLOG(eInfo1,
        "Max Output Frame Count: %d (%d bytes)",
        nMaxOutputFrameCount,
        cbOutputBufferSize);
   
    // build the input and output connection descriptors
    pInputConnections[0].Type = APO_CONNECTION_BUFFER_TYPE_EXTERNAL;
    pInputConnections[0].pFormat = spInputFormatType;
    pInputConnections[0].u32MaxFrameCount = nMaxInputFrameCount;
    pInputConnections[0].u32Signature = APO_CONNECTION_DESCRIPTOR_SIGNATURE;
    pInputConnections[0].pBuffer = (UINT_PTR)pInputBuffer;

    pOutputConnections[0].Type = APO_CONNECTION_BUFFER_TYPE_EXTERNAL;
    pOutputConnections[0].pFormat = spOutputFormatType;
    pOutputConnections[0].u32MaxFrameCount = nMaxOutputFrameCount;
    pOutputConnections[0].u32Signature = APO_CONNECTION_DESCRIPTOR_SIGNATURE;
    pOutputConnections[0].pBuffer = (UINT_PTR)pOutputBuffer;

    // build the input and output connection properties
    pInputProperties[0].pBuffer = pInputConnections[0].pBuffer;
    pInputProperties[0].u32BufferFlags = BUFFER_VALID;
    pInputProperties[0].u32ValidFrameCount = nMaxInputFrameCount;
    pInputProperties[0].u32Signature = APO_CONNECTION_PROPERTY_SIGNATURE;

    pOutputProperties[0].pBuffer = pOutputConnections[0].pBuffer;
    pOutputProperties[0].u32BufferFlags = BUFFER_VALID;
    pOutputProperties[0].u32ValidFrameCount = 0;
    pOutputProperties[0].u32Signature = APO_CONNECTION_PROPERTY_SIGNATURE;

    // get a logger
    hr = g_IShell->GetBasicLog(&pBasicLog);
    
    if (S_OK != hr)
    {
        XLOG(XFAIL,
            eError,
            "FAIL: Error getting basic log object: 0x%08x", hr);
        goto Exit;
    }
    
    // Fill the input buffer with a sine tone
    hr = FillBufferWithSineSignal(
        pBasicLog, XFAIL,
        &pwfxInputFormat->Format,
        dTestToneAmplitude,
        dTestToneFrequency,
        0.0, // initial phase,
        0.0, // dc
        Method_NoDithering,
        pInputBuffer,
        nMaxInputFrameCount,
        static_cast<UINT32>(cbInputBufferSize)
    );

    if (S_OK != hr)
    {
        XLOG(XFAIL,
            eError,
            "FAIL: Error filling buffer: 0x%08x", hr);
        goto Exit;
    }

    // Lock our "mini-graph" for processing
    hr = spAudioProcessingObjectConfiguration->LockForProcess(1,
                                                              &pInputConnections,
                                                              1,
                                                              &pOutputConnections);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
            eError,
            "FAIL: IAudioProcessingObject::LockForProcess returned error (hr = 0x%08X)",
            hr);
        goto Exit;
    }
    
    // Set the input dynamic frame rate as necessary
    if(0 != fRequestedDynamicInputFrameRate)
    {
        hr = spAudioRateConvert->SetInputFrameRate(fRequestedDynamicInputFrameRate);
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                eError,
                "FAIL: IAudioRateConvert::SetInputFrameRate returned error (hr = 0x%08X)",
                hr);
            goto Exit;
        }
    }
    
    // Set the output dynamic frame rate as necessary
    if(0 != fRequestedDynamicOutputFrameRate)
    {
        hr = spAudioRateConvert->SetOutputFrameRate(fRequestedDynamicOutputFrameRate);
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                eError,
                "FAIL: IAudioRateConvert::SetOutputFrameRate returned error (hr = 0x%08X)",
                hr);
            goto Exit;
        }
    }

    SLOG(eInfo1, "Processing...");

    // calculate the expected number of output frames
    nOutputFrameCount =  spAudioProcessingObjectRT->CalcOutputFrames(pInputProperties[0].u32ValidFrameCount);

    SLOG(eInfo1, "Output frame count: %d", nOutputFrameCount);

    // Process
    spAudioProcessingObjectRT->APOProcess(1,
                                          &pInputProperties,
                                          1,
                                          &pOutputProperties);
    if(BUFFER_VALID != pOutputProperties[0].u32BufferFlags)
    {
        XLOG(XFAIL,
            eError,
            "FAIL: Output buffer not marked as VALID after call to IAudioProcessingObject::APOProcess");
        goto Exit;
    }
    
    //  In case buffer is marked as AUDCLNT_BUFFERFLAGS_SILENT
    hr = ApplySilenceFlag(pOutputConnections, pOutputProperties);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
            eError,
            "FAIL: ApplySilenceFlag returned error (hr = 0x%08X).",
            hr);
        goto Exit;
    }

    // Unlock the graph
    hr = spAudioProcessingObjectConfiguration->UnlockForProcess();
    if(FAILED(hr))
    {
        XLOG(XFAIL,
            eError,
            "FAIL: IAudioProcessingObjectConfiguration::UnlockForProcess returned error (hr = 0x%08X)",
            hr);
        goto Exit;
    }

Exit:
    SAFE_DELETE_ARRAY(pInputConnections);
    SAFE_DELETE_ARRAY(pInputProperties);

    SAFE_DELETE_ARRAY(pInputBuffer);

    if(FAILED(hr))
    {
        SAFE_DELETE_ARRAY(pOutputBuffer);
    }
    else
    {
        // pass the output buffer back to the caller
        *ppProcessorOutput = pOutputBuffer;
        *pcbProcessorOutput = cbOutputBufferSize;
        *pnProcessorOutputFrames = nOutputFrameCount;
    }
    
    return hr;
}

HRESULT GetBufferFundamentalFrequency(PBYTE pBuffer,
                                      PWAVEFORMATEX pwfxBufferFormat,
                                      UINT32 nBufferFrames,
                                      UINT32* pnChannelFrequencies,
                                      DOUBLE** padChannelFrequencies)
{
    HRESULT                                         hr = S_OK;

    CComPtr<ISoundAnalyze>                          spSndAnlyz = NULL;

    DOUBLE*                                         adFrequencies = NULL;
    UINT32                                          nFreqs = 0;

    if((NULL == pBuffer)||
       (NULL == pwfxBufferFormat)||
       (NULL == pnChannelFrequencies)||
       (NULL == padChannelFrequencies))
    {
        hr = E_POINTER;
        goto Exit;
    }

    // get the sound analyzer interface
    hr = CreateSoundAnalyze(&spSndAnlyz);
    if(FAILED(hr))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Unable to obtain SoundAnalyzer interface! (0x%08X)",
            hr);
        goto Exit;
    }

    // Initialize the Sound Analyzer
    hr = spSndAnlyz->InitializeSoundAnalyzer((LPCWAVEFORMATEX)(pwfxBufferFormat),
                                             pBuffer,
                                             nBufferFrames,
                                             RECTANGLE,
                                             FALSE);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
            eError,
            "FAIL: Unable to initialize Sound Analyzer (0x%08X)",
            hr);

        if(HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND) == hr)
        {
            // give the user a hint
            SLOG(eError, "--->Is SndAnlyz.dll in your path?");
        }
    }

    // allocate an array of doubles to store the fundamental frequency for each channel
    nFreqs = pwfxBufferFormat->nChannels;

    if(NULL == (adFrequencies = new DOUBLE[nFreqs]))
    {
        hr = E_OUTOFMEMORY;
        XLOG(XFAIL,
            eError,
            "FAIL: Out of memory!");
        goto Exit;
    }

    for(UINT32 i=0;i<nFreqs;i++)
    {            
        // Get the base frequency
        // channels are 1-based in SoundAnalyze
        hr = spSndAnlyz->GetBaseFrequency(i+1, adFrequencies[i]);
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: ISoundAnalyze returned error (hr = 0x%08X)",
                 hr);
            goto Exit;
        }
    }

Exit:
    if(FAILED(hr))
    {
        SAFE_DELETE_ARRAY(adFrequencies);
    }
    else
    {
        // only update the out-params when everything succeeded
        *pnChannelFrequencies = nFreqs;
        *padChannelFrequencies = adFrequencies;
    }

    return hr;
}

void 
LogTestCAudioRateConvertCMPT
(
    LPCSTR              pszBufferFlowLabel,
    FLOAT               fChangePercent,
    SRC_ALGORITHM_TYPE  srcAlgorithm
)
{
    const UINT  BUFF_SIZE = 300;
    char        pszBuffer[BUFF_SIZE] = "INFO: Testing SRC APO with ";
    size_t      uCount = BUFF_SIZE - strlen(pszBuffer);
    LPCSTR      pszAlgorithm = NULL;

    switch(srcAlgorithm)
    {
        case SRC_QUALITY_LINEAR:
            pszAlgorithm = "SRC_QUALITY_LINEAR";
            break;
        case SRC_QUALITY_SINC1:
            pszAlgorithm = "SRC_QUALITY_SINC1";
            break;
        case SRC_QUALITY_SINC2:
            pszAlgorithm = "SRC_QUALITY_SINC2";
            break;
        case SRC_QUALITY_SINC3:
            pszAlgorithm = "SRC_QUALITY_SINC3";
            break;
        default:
            pszAlgorithm = "!!UNKNOWN!!";
            break;
    }

    if(fChangePercent != 0)
    {
        _snprintf_s(pszBuffer + strlen(pszBuffer), uCount*sizeof(char), uCount - 1,
                    "an %s frame rate %s of %f%% and %s quality flag.\n", 
                    pszBufferFlowLabel,
                    ((fChangePercent > 0) ? "increase" : "decrease"),
                    abs(fChangePercent), 
                    pszAlgorithm);
    }
    else
    {
        _snprintf_s(pszBuffer + strlen(pszBuffer), uCount*sizeof(char), uCount - 1,
                    "no frame rate change and %s quality flag.\n",
                    pszAlgorithm);
    }

    SLOG(1, "%s", pszBuffer);
}
                                                                                                          
HRESULT
CAudioRateConvertCMPT_Functional_ChangePercent
(
    FLOAT               fChangePercent,
    SRC_ALGORITHM_TYPE  srcAlgorithm
)
{
    HRESULT                     hr = S_OK;

    CAPODevice*                 pCurrentDevice = NULL;
    GUID                        guidAPOUnderTest = GUID_NULL;

    WAVEFORMATEXTENSIBLE        wfxInputFormat = {0};
    WAVEFORMATEXTENSIBLE        wfxOutputFormat = {0};
    
    UINT32                      nMillisInputBufferDuration = 1000;
    const DOUBLE                dTestToneFrequency = 10000.0; // 10K Hz
    const DOUBLE                dTestToneAmplitude = 1.0;
    FLOAT                       fFullChangeFraction = (1.0f + (fChangePercent / 100.0f));

    AUDIO_RATE_CONVERT_PARAMS   sAPOInitParams = {0};

    PBYTE                       pProcessorOutput = NULL;
    size_t                      cbProcessorOutput = 0;
    UINT32                      nProcessorOutputFrames = 0;

    UINT32                      nOutputFrequencies = 0;
    DOUBLE*                     adOutputFrequencies = NULL;

    FLOAT                       fRequestedInputDynamicFrameRate = 0.0f;
    FLOAT                       fRequestedOutputDynamicFrameRate = 0.0f;

    pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    if (NULL == pCurrentDevice)
    {
        goto Exit;
    }
    
    if (!IsRateConvertCMPT(pCurrentDevice))
    {
        SLOG(eInfo1, "APO is not RateConvertCMPT APO.  Skipping test.");
        goto Exit;
    }

    pCurrentDevice->GetClsID(&guidAPOUnderTest);

    // Populate the APO Init params and input/ouput formats
    sAPOInitParams.APOInit.cbSize                   = sizeof(sAPOInitParams);
    sAPOInitParams.APOInit.clsid                    = guidAPOUnderTest;    
    sAPOInitParams.fInputFrameRate                  = 48000.0;
    sAPOInitParams.fOutputFrameRate                 = 48000.0;
    sAPOInitParams.fMinInputFrameRate               = 22050.0;
    sAPOInitParams.fMaxInputFrameRate               = 128000.0;
    sAPOInitParams.fMinOutputFrameRate              = 22050.0;
    sAPOInitParams.fMaxOutputFrameRate              = 128000.0;
    sAPOInitParams.eAlgorithmType                   = srcAlgorithm;
    sAPOInitParams.eFlowType                        = AUDIO_FLOW_PUSH;
    sAPOInitParams.u32SamplesPerFrame               = 2;
    sAPOInitParams.u32HighQualityTaplength          = 0L;

    // Input WFX
    wfxInputFormat.Format.wFormatTag                = WAVE_FORMAT_EXTENSIBLE;
    wfxInputFormat.Format.nChannels                 = 2;
    wfxInputFormat.Format.nSamplesPerSec            = 48000;
    wfxInputFormat.Format.wBitsPerSample            = 32;
    wfxInputFormat.Format.nAvgBytesPerSec           = wfxInputFormat.Format.nChannels *
                                                      (wfxInputFormat.Format.wBitsPerSample / 8) *
                                                      wfxInputFormat.Format.nSamplesPerSec;
    wfxInputFormat.Format.nBlockAlign               = wfxInputFormat.Format.nChannels *
                                                      (wfxInputFormat.Format.wBitsPerSample /8);
    wfxInputFormat.Format.cbSize                    = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
    wfxInputFormat.Samples.wValidBitsPerSample      = 32; // valid bits per sample
    wfxInputFormat.dwChannelMask                    = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
    wfxInputFormat.SubFormat                        = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;

    // Output WFX
    memcpy(&wfxOutputFormat, &wfxInputFormat, sizeof(WAVEFORMATEXTENSIBLE));

    if(fChangePercent != 0)
    {
        // Change Input Rate
        fRequestedInputDynamicFrameRate = 48000.0f * fFullChangeFraction;
        fRequestedOutputDynamicFrameRate = 0.0f;
    }
    LogTestCAudioRateConvertCMPT("input", fChangePercent, srcAlgorithm);

    //
    // Run the APO
    //
    hr = RateConverterFunctionalTest(&guidAPOUnderTest,
                                     &wfxInputFormat,
                                     &wfxOutputFormat,
                                     nMillisInputBufferDuration,
                                     sAPOInitParams,
                                     fRequestedInputDynamicFrameRate,
                                     fRequestedOutputDynamicFrameRate,
                                     dTestToneFrequency,
                                     dTestToneAmplitude,
                                     &pProcessorOutput,
                                     &cbProcessorOutput,
                                     &nProcessorOutputFrames);
    if(S_OK != hr)
    {
        XLOG(XFAIL,
            eError,
            "FAIL: Error encountered while exercising the SRC (hr = 0x%08X)",
            hr);
        goto Exit;
    }

    //
    // Analyze the Results
    //
    hr  = GetBufferFundamentalFrequency(pProcessorOutput,
                                        (PWAVEFORMATEX)(&wfxOutputFormat),
                                        nProcessorOutputFrames,
                                        &nOutputFrequencies,
                                        &adOutputFrequencies);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
            eError,
            "FAIL: Error analyzing output buffer (0x%08X)",
            hr);
        goto Exit;
    }

    // verify that we're in the expected range
    for(UINT32 i=0; i < nOutputFrequencies; i++)
    {
        if(FREQ_TOLER_HZ < abs(adOutputFrequencies[i] - dTestToneFrequency * fFullChangeFraction))
        {
            XLOG(XFAIL,
                eError,
                "FAIL: Frequency for channel %d (%.2f Hz) is outside of the expected range (%.2f +/ %.2f Hz)",
                i,
                adOutputFrequencies[i],
                dTestToneFrequency * fFullChangeFraction,
                FREQ_TOLER_HZ);                
        }
        else
        {
            SLOG(eInfo1, "Channel %d Frequency: %.2f Hz [OK]", i, adOutputFrequencies[i]);
        }
    }

    // release the output buffer and output frequencies so we can re-use them
    SAFE_DELETE_ARRAY(pProcessorOutput);
    SAFE_DELETE_ARRAY(adOutputFrequencies);

    if(fChangePercent != 0)
    {
        // Change Output Rate, leave the input rate alone
        fRequestedInputDynamicFrameRate = 0.0f;
        fRequestedOutputDynamicFrameRate = 48000.0f * fFullChangeFraction;
    }
    LogTestCAudioRateConvertCMPT("output", fChangePercent, srcAlgorithm);


    //
    // Run the APO
    //
    hr = RateConverterFunctionalTest(&guidAPOUnderTest,
                                     &wfxInputFormat,
                                     &wfxOutputFormat,
                                     nMillisInputBufferDuration,
                                     sAPOInitParams,
                                     fRequestedInputDynamicFrameRate,
                                     fRequestedOutputDynamicFrameRate,
                                     dTestToneFrequency,
                                     dTestToneAmplitude,
                                     &pProcessorOutput,
                                     &cbProcessorOutput,
                                     &nProcessorOutputFrames);
    if(S_OK != hr)
    {
        XLOG(XFAIL,
            eError,
            "ERROR: Error encountered while exercising the SRC (hr = 0x%08X)",
            hr);
        goto Exit;
    }

    //
    // Analyze the Results
    //
    hr  = GetBufferFundamentalFrequency(pProcessorOutput,
                                        (PWAVEFORMATEX)(&wfxOutputFormat),
                                        nProcessorOutputFrames,
                                        &nOutputFrequencies,
                                        &adOutputFrequencies);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
            eError,
            "FAIL: Error analyzing output buffer (0x%08X)",
            hr);
        goto Exit;
    }

    // verify that we're in the expected range
    for(UINT32 i=0; i < nOutputFrequencies; i++)
    {
        if(FREQ_TOLER_HZ < abs(adOutputFrequencies[i] - dTestToneFrequency / fFullChangeFraction))
        {
            XLOG(XFAIL,
                eError,
                "FAIL: Frequency for channel %d (%.2f Hz) is outside of the expected range (%.2f +/ %.2f Hz)",
                i,
                adOutputFrequencies[i],
                dTestToneFrequency / fFullChangeFraction,
                FREQ_TOLER_HZ);                
        }
        else
        {
            SLOG(eInfo1, "Channel %d Frequency: %.2f Hz [OK]", i, adOutputFrequencies[i]);
        }
    }
                
Exit:    
    SAFE_DELETE_ARRAY(pProcessorOutput);
    SAFE_DELETE_ARRAY(adOutputFrequencies);
    return hr;
}


// SRC_QUALITY_LINEAR Test cases
DWORD WINAPI TC_CAudioRateConvertCMPT_Functional_NoChange_Linear()
{
    // Failure results will be triggered via XLOG, that is why HRESULT can be ignored. 
#pragma prefast(suppress:28931, "Ignore hr")
    CAudioRateConvertCMPT_Functional_ChangePercent(0.0f, SRC_QUALITY_LINEAR);
    return (FNS_PASS);
}

DWORD WINAPI TC_CAudioRateConvertCMPT_Functional_Increase1Percent_Linear()
{
    // Failure results will be triggered via XLOG, that is why HRESULT can be ignored. 
#pragma prefast(suppress:28931, "Ignore hr")
    CAudioRateConvertCMPT_Functional_ChangePercent(+1.0f, SRC_QUALITY_LINEAR);
    return (FNS_PASS);
}

DWORD WINAPI TC_CAudioRateConvertCMPT_Functional_Increase20Percent_Linear()
{
    // Failure results will be triggered via XLOG, that is why HRESULT can be ignored. 
#pragma prefast(suppress:28931, "Ignore hr")
    CAudioRateConvertCMPT_Functional_ChangePercent(+20.0f, SRC_QUALITY_LINEAR);
    return (FNS_PASS);
}

DWORD WINAPI TC_CAudioRateConvertCMPT_Functional_Decrease1Percent_Linear()
{
    // Failure results will be triggered via XLOG, that is why HRESULT can be ignored. 
#pragma prefast(suppress:28931, "Ignore hr")
    CAudioRateConvertCMPT_Functional_ChangePercent(-1.0f, SRC_QUALITY_LINEAR);
    return (FNS_PASS);
}

DWORD WINAPI TC_CAudioRateConvertCMPT_Functional_Decrease20Percent_Linear()
{
    // Failure results will be triggered via XLOG, that is why HRESULT can be ignored. 
#pragma prefast(suppress:28931, "Ignore hr")
    CAudioRateConvertCMPT_Functional_ChangePercent(-20.0f, SRC_QUALITY_LINEAR);
    return (FNS_PASS);
}

// SRC_QUALITY_SINC3 Test cases
DWORD WINAPI TC_CAudioRateConvertCMPT_Functional_NoChange_SINC3()
{
    // Failure results will be triggered via XLOG, that is why HRESULT can be ignored. 
#pragma prefast(suppress:28931, "Ignore hr")
    CAudioRateConvertCMPT_Functional_ChangePercent(0.0f, SRC_QUALITY_SINC3);
    return (FNS_PASS);
}

DWORD WINAPI TC_CAudioRateConvertCMPT_Functional_Increase1Percent_SINC3()
{
    // Failure results will be triggered via XLOG, that is why HRESULT can be ignored. 
#pragma prefast(suppress:28931, "Ignore hr")
    CAudioRateConvertCMPT_Functional_ChangePercent(+1.0f, SRC_QUALITY_SINC3);
    return (FNS_PASS);
}

DWORD WINAPI TC_CAudioRateConvertCMPT_Functional_Increase20Percent_SINC3()
{
    // Failure results will be triggered via XLOG, that is why HRESULT can be ignored. 
#pragma prefast(suppress:28931, "Ignore hr")
    CAudioRateConvertCMPT_Functional_ChangePercent(+20.0f, SRC_QUALITY_SINC3);
    return (FNS_PASS);
}

DWORD WINAPI TC_CAudioRateConvertCMPT_Functional_Decrease1Percent_SINC3()
{
    // Failure results will be triggered via XLOG, that is why HRESULT can be ignored. 
#pragma prefast(suppress:28931, "Ignore hr")
    CAudioRateConvertCMPT_Functional_ChangePercent(-1.0f, SRC_QUALITY_SINC3);
    return (FNS_PASS);
}

DWORD WINAPI TC_CAudioRateConvertCMPT_Functional_Decrease20Percent_SINC3()
{
    // Failure results will be triggered via XLOG, that is why HRESULT can be ignored. 
#pragma prefast(suppress:28931, "Ignore hr")
    CAudioRateConvertCMPT_Functional_ChangePercent(-20.0f, SRC_QUALITY_SINC3);
    return (FNS_PASS);
}
