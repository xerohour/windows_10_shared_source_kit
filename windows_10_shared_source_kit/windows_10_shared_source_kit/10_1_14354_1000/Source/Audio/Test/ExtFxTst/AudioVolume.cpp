//--------------------------------------------------------------------------
//
//  File: AudioVolume.cpp
//
//  Copyright (c) Microsoft Corporation.  All rights reserved
//
//  Abstract:
//      Implementation of tests for CAudioVolume and related interfaces.
//
//  Contents:
//      IsVolumeAPO
//      Log_SetVolumeCoefficients()
//      Log_SetMute()
//      Verify_NoRamp()
//      Verify_NoCurve()
//      Verify_WindowsFadeCurve()
//      VerifyCurve()
//      Volume_TestCurve()
//      Mute_TestCurve()
//      TC_CAudioVolume_VerifyRegistration()
//      TC_CAudioVolume_Initialize()
//      TC_CAudioVolume_States()
//      TC_IAudioVolume_SetVolumeCoefficients()
//      TC_IAudioVolume_GetVolumeCoefficients()
//      TC_IAudioVolume_CURVE_NONE()
//      TC_IAudioVolume_CURVE_WINDOWS_FADE()
//      TC_IAudioVolume_NoInitialRamp()
//      TC_IAudioMuteAPO_SetMute()
//      TC_IAudioMuteAPO_GetMute()
//      TC_IAudioMuteAPO_CURVE_NONE()
//      TC_IAudioMuteAPO_CURVE_WINDOWS_FADE()
//      TC_IAudioMuteAPO_NoInitialRamp()
//
//  History:
//      05/18/07    Fwong       Moving over tests from UMACore.
//
//--------------------------------------------------------------------------
#include "common.h"
#include <math.h>

using namespace NotANumber;

extern APOLIMITS gAPOLimits;

//  Right now we don't support delay, it must be zero.
const HNSTIME   hnst_Volume_DefaultDelay    = 0;
const HNSTIME   hnst_Volume_DefaultDuration = 10 * HNSTIME1MSEC;
const float     f_Volume_InitialVolume     = 1.0;
const float     f_Volume_TargetVolume      = 3.0;
const UINT      c_Volume_FailsBeforeAbort  = 10;

//--------------------------------------------------------------------------;
//
//  BOOL IsVolumeAPO
//
//  Description:
//      Given a device, determines if it's the Volume APO.
//
//  Arguments:
//      CAPODevice *pAPODevice: Device in question.
//
//  Return (BOOL):
//      TRUE if VolumeAPO, FALSE otherwise.
//
//  History:
//      05/18/07    Fwong       Moving over tests from UMACore.
//
//--------------------------------------------------------------------------;
BOOL IsVolumeAPO
(
    CAPODevice     *pAPODevice
)
{
    GUID            guidClassId;
    
    pAPODevice->GetClsID(&guidClassId);
    
    if (IsEqualGUID(guidClassId, __uuidof(AudioVolume)))
    {
        return (TRUE);
    }
    else
    {
        return (FALSE);
    }
}   //   IsVolumeAPO()

//--------------------------------------------------------------------------;
//
//  HRESULT Log_SetVolumeCoefficients
//
//  Description:
//      Logged call to SetVolumeCoefficients().
//
//  Arguments:
//      IAudioVolume *pIVol: IAudioVolume interface.
//
//      UINT32 cCoeffs: Same as method.
//
//      FLOAT32 *pfCoeffs: Same as method.
//
//      HNSTIME *phnstDelay: Same as method.
//
//      AUDIO_CURVE_TYPE u32Curve: Same as method.
//
//      HNSTIME *phnstDuration: Same as method.
//
//  Return (HRESULT):
//      Same as method.
//
//  History:
//      05/18/07    Fwong       Moving over tests from UMACore.
//
//--------------------------------------------------------------------------;
HRESULT Log_SetVolumeCoefficients
(
    IAudioVolume       *pIVol,
    UINT32              cCoeffs,
    FLOAT32            *pfCoeffs,
    HNSTIME            *phnstDelay,
    AUDIO_CURVE_TYPE    u32Curve,
    HNSTIME            *phnstDuration
)
{
    char                szOutput[256];
    char                szTemp[40];
    LPSTR               pszWrite;
    INT                 cchOutput;
    INT                 cchRemain;
    UINT                ii;
    char                szCalling[] = "Calling SetVolumeCoefficients(";
    
    cchOutput = ARRAYSIZE(szOutput);
    szOutput[0] = 0;
    
    //  Adding 'Calling...'
    cchRemain = cchOutput - lstrlen(szOutput) - 1;
    pszWrite  = &(szOutput[lstrlen(szOutput)]);
    if (cchRemain > lstrlen(szCalling))
    {
        lstrcpy(pszWrite, szCalling);
    }
    
    //  Adding cCoeffs
    cchRemain = cchOutput - lstrlen(szOutput) - 1;
    pszWrite  = &(szOutput[lstrlen(szOutput)]);
    sprintf_s(szTemp, "%d, [", cCoeffs);
    if (cchRemain > lstrlen(szTemp))
    {
        lstrcpy(pszWrite, szTemp);
    }
    
    //  Adding coefficients
    for (ii = 0; ii < cCoeffs; ++ii)
    {
        UINT    uUnits    = (UINT)pfCoeffs[ii];
        UINT    uFraction = ((UINT)(pfCoeffs[ii] * 10)) % 10;
        
        cchRemain = cchOutput - lstrlen(szOutput) - 1;
        pszWrite  = &(szOutput[lstrlen(szOutput)]);
        
        if (ii + 1 == cCoeffs)
        {
            sprintf_s(szTemp, "%d.%01d", uUnits, uFraction);
        }
        else
        {
            sprintf_s(szTemp, "%d.%01d,", uUnits, uFraction);
        }
        if (cchRemain > lstrlen(szTemp))
        {
            lstrcpy(pszWrite, szTemp);
        }
    }
    
    //  Adding delay...
    cchRemain = cchOutput - lstrlen(szOutput) - 1;
    pszWrite  = &(szOutput[lstrlen(szOutput)]);
    if (NULL == phnstDelay)
    {
        lstrcpy(szTemp, "], NULL, ");
    }
    else
    {
        sprintf_s(szTemp, "], %d ms, ", (UINT32)(*phnstDelay / HNSTIME1MSEC));
    }
    if (cchRemain > lstrlen(szTemp))
    {
        lstrcpy(pszWrite, szTemp);
    }
    
    //  Adding Curve...
    cchRemain = cchOutput - lstrlen(szOutput) - 1;
    pszWrite  = &(szOutput[lstrlen(szOutput)]);
    switch (u32Curve)
    {
        case AUDIO_CURVE_TYPE_NONE:
            lstrcpy(szTemp, "None, ");
            break;
            
        case AUDIO_CURVE_TYPE_WINDOWS_FADE:
            lstrcpy(szTemp, "Windows Fade, ");
            break;
            
        default:
            lstrcpy(szTemp, "Error, ");
            break;
    }
    if (cchRemain > lstrlen(szTemp))
    {
        lstrcpy(pszWrite, szTemp);
    }
    
    //  Adding duration...
    cchRemain = cchOutput - lstrlen(szOutput) - 1;
    pszWrite  = &(szOutput[lstrlen(szOutput)]);
    if (NULL == phnstDuration)
    {
        lstrcpy(szTemp, "NULL).");
    }
    else
    {
        sprintf_s(szTemp, "%d ms).", (UINT32)(*phnstDuration / HNSTIME1MSEC));
    }
    if (cchRemain > lstrlen(szTemp))
    {
        lstrcpy(pszWrite, szTemp);
    }
    
    SLOG(eInfo1, szOutput);

    return (pIVol->SetVolumeCoefficients(cCoeffs, pfCoeffs, phnstDelay, u32Curve, phnstDuration));
}   //   Log_SetVolumeCoefficients()

//--------------------------------------------------------------------------;
//
//  HRESULT Log_SetMute
//
//  Description:
//      Logged call to SetMute()
//
//  Arguments:
//      IAudioMuteAPO *pIMute: IAudioMuteAPO interface.
//
//      boolean bMute: Same as method.
//
//      HNSTIME *pTime: Same as method.
//
//      AUDIO_CURVE_TYPE u32CurveType: Same as method.
//
//      HNSTIME *pCurveDuration: Same as method.
//
//  Return (HRESULT):
//      Same as method.
//
//  History:
//      05/18/07    Fwong       Moving over tests from UMACore.
//
//--------------------------------------------------------------------------;
HRESULT Log_SetMute
(
    IAudioMuteAPO      *pIMute,
    boolean             bMute,
    HNSTIME            *pTime,
    AUDIO_CURVE_TYPE    u32CurveType,
    HNSTIME            *pCurveDuration
)
{
    char                szDelay[40];
    char                szDuration[40];
    char                szCurve[20];
    
    //  Formatting delay...
    if (NULL == pTime)
    {
        lstrcpy(szDelay, "NULL");
    }
    else
    {
        sprintf_s(szDelay, "%d ms", (UINT32)(*pTime / HNSTIME1MSEC));
    }
    
    //  Formatting duration...
    if (NULL == pCurveDuration)
    {
        lstrcpy(szDuration, "NULL");
    }
    else
    {
        sprintf_s(szDuration, "%d ms", (UINT32)(*pCurveDuration / HNSTIME1MSEC));
    }
    
    //  Formatting curve...
    switch (u32CurveType)
    {
        case AUDIO_CURVE_TYPE_NONE:
            lstrcpy(szCurve, "None");
            break;
            
        case AUDIO_CURVE_TYPE_WINDOWS_FADE:
            lstrcpy(szCurve, "Windows Fade");
            break;
            
        default:
            lstrcpy(szCurve, "Error");
            break;
    }
    
    SLOG(
        eInfo1,
        "Calling SetMute(%s, %s, %s, %s).",
        ((bMute)?"true":"false"),
        szDelay,
        szCurve,
        szDuration);
        
    return (pIMute->SetMute(bMute, pTime, u32CurveType, pCurveDuration));
}   //   Log_SetMute()

//--------------------------------------------------------------------------;
//
//  void Verify_NoRamp
//
//  Description:
//      Given the input and output buffers and specifics, verifies that
//          the 'no initial ramp' condition was met.
//
//  Arguments:
//      UNCOMPRESSEDAUDIOFORMAT uaf: Audio Format.
//
//      UINT iChannel: Channel to check.
//
//      float fTargetVolume: Target Volume.
//
//      float *pfInput: Input buffer.
//
//      UINT cbInput: Size of above buffer in bytes.
//
//      float *pfOutput: Output buffer.
//
//      UINT cbOutput: Size of above buffer in bytes.
//
//  Return (void):
//      None.
//
//  History:
//      05/18/07    Fwong       Moving over tests from UMACore.
//
//--------------------------------------------------------------------------;
void Verify_NoRamp
(
    UNCOMPRESSEDAUDIOFORMAT     uaf,
    UINT                        iChannel,
    float                       fTargetVolume,
    float                      *pfInput,
    UINT                        cbInput,
    float                      *pfOutput,
    UINT                        cbOutput
)
{
    float       fInput;
    float       fOutput;
    float       fExpected;
    UINT        cSamples;
    UINT        ii;
    UINT        uBlock;
    UINT        cChannels;
    const UINT  cFailsBeforeAbort = c_Volume_FailsBeforeAbort;
    UINT        cFails = cFailsBeforeAbort; 

    //  We only deal with floats...
    if (KSDATAFORMAT_SUBTYPE_IEEE_FLOAT != uaf.guidFormatType)
    {
        XLOG(XFAIL, eError, "FAIL: Format not float.");
        return;
    }
    
    //  Are the streams of different lengths?
    if (cbInput != cbOutput)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Streams are different lengths (%d bytes vs %d bytes).",
            cbInput,
            cbOutput);
        return;
    }
    
    if (0 != (cbInput % (uaf.dwBytesPerSampleContainer * uaf.dwSamplesPerFrame)))
    {
        XLOG(XFAIL, eError, "FAIL: Stream size not aligned to format type.");
        return;
    }
    
    uBlock    = uaf.dwBytesPerSampleContainer * uaf.dwSamplesPerFrame;
    cChannels = uaf.dwSamplesPerFrame;
    cSamples  = cbInput / uBlock;
    
    for (ii = 0; ii < cSamples; ++ii)
    {
        fInput    = pfInput[cChannels * ii + iChannel];
        fExpected = fTargetVolume * fInput;
        fOutput   = pfOutput[cChannels * ii + iChannel];
    
        if (!InRangeScalar(fExpected, fOutput, (float)0.001))
        {
            cFails--;
            XLOG(
                XFAIL,
                eError,
                "FAIL: Channel %d sample at %d does not compare "
                    "(Input: %.03f, Expected:%.03f, Actual:%.03f).",
                iChannel,
                ii,
                fInput,
                fExpected,
                fOutput);
        }
        
        if (0 == cFails)
        {
            SLOG(
                eError,
                "Already encountered %d errors, aborting...",
                cFailsBeforeAbort);
                
            break;
        }
    }
    
    if (cFailsBeforeAbort == cFails)
    {
        SLOG(
            eInfo1,
            "No initial ramp Verified for channel %d: %d samples compared.",
            iChannel,
            ii);
    }
}   //   Verify_NoRamp()

//--------------------------------------------------------------------------;
//
//  void Verify_NoCurve
//
//  Description:
//      Given the input and output buffers and specifics, verifies that
//          the 'no curve' param curve condition was met.
//
//  Arguments:
//      UNCOMPRESSEDAUDIOFORMAT uaf: Audio format.
//
//      UINT iChannel: Channel to check.
//
//      float fInitialVolume: Initial Volume.
//
//      float fTargetVolume: Target Volume.
//
//      HNSTIME hnstDelay: Delay before applying curve.
//
//      HNSTIME hnstDuration: Duration of curve.
//
//      float *pfInput: Input buffer.
//
//      UINT cbInput: Size of above buffer in bytes.
//
//      float *pfOutput: Output buffer.
//
//      UINT cbOutput: Size of above buffer in bytes.
//
//  Return (void):
//      None.
//
//  History:
//      05/18/07    Fwong       Moving over tests from UMACore.
//
//--------------------------------------------------------------------------;
void Verify_NoCurve
(
    UNCOMPRESSEDAUDIOFORMAT     uaf,
    UINT                        iChannel,
    float                       fInitialVolume,
    float                       fTargetVolume,
    HNSTIME                     hnstDelay,
    HNSTIME                     /*hnstDuration*/,
    float                      *pfInput,
    UINT                        cbInput,
    float                      *pfOutput,
    UINT                        cbOutput
)
{
    float       fInput;
    float       fExpected;
    float       fOutput;
    UINT        cSamples;
    UINT        ii;
    UINT        uBlock;
    UINT        cChannels;
    UINT        iSampleDelay;
    const UINT  cFailsBeforeAbort = c_Volume_FailsBeforeAbort;
    UINT        cFails = cFailsBeforeAbort; 

    //  We only deal with floats...
    if (KSDATAFORMAT_SUBTYPE_IEEE_FLOAT != uaf.guidFormatType)
    {
        XLOG(XFAIL, eError, "FAIL: Format not float.");
        return;
    }
    
    //  Are the streams of different lengths?
    if (cbInput != cbOutput)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Streams are different lengths (%d bytes vs %d bytes).",
            cbInput,
            cbOutput);
        return;
    }
    
    if (0 != (cbInput % (uaf.dwBytesPerSampleContainer * uaf.dwSamplesPerFrame)))
    {
        XLOG(XFAIL, eError, "FAIL: Stream size not aligned to format type.");
        return;
    }
    
    uBlock           = uaf.dwBytesPerSampleContainer * uaf.dwSamplesPerFrame;
    cChannels        = uaf.dwSamplesPerFrame;
    cSamples         = cbInput / uBlock;
    
    iSampleDelay     = (UINT)((double)(uaf.fFramesPerSecond * hnstDelay)
                       / (1000 * HNSTIME1MSEC));
                       
    //  Verifying initial volume (during delay)
    if (0 != iSampleDelay)
    {
        SLOG(
            eInfo1,
            "Verifying initial volume (%.03f) (during delay)...",
            fInitialVolume);
        
        cFails = cFailsBeforeAbort;    
        for (ii = 0; ii <= iSampleDelay; ++ii)
        {
            fInput    = pfInput[cChannels * ii + iChannel];
            fExpected = fInitialVolume * fInput;
            fOutput   = pfOutput[cChannels * ii + iChannel];

            if (!InRangeScalar(fExpected, fOutput, (float)0.001))
            {
                cFails--;
                XLOG(
                    XFAIL,
                    eError,
                    "FAIL: Channel %d sample at %d does not compare "
                        "(Input:%.03f, Expected:%.03f, Actual:%.03f).",
                    iChannel,
                    ii,
                    fInput,
                    fExpected,
                    fOutput);
            }
    
            if (0 == cFails)
            {
                SLOG(
                    eError,
                    "Already encountered %d errors, aborting...",
                    cFailsBeforeAbort);
            
                break;
            }
        }
    
        if (cFailsBeforeAbort == cFails)
        {
            SLOG(
                eInfo1,
                "Initial volume verified for channel %d: %d samples compared.",
                iChannel,
                ii);
        }
    }
    
    //  Verifying eventual volume (after curve)
    SLOG(
        eInfo1,
        "Verifying eventual volume (%.03f) (after curve)...",
        fTargetVolume);
        
    cFails = cFailsBeforeAbort;    
    for (ii = iSampleDelay + 1; ii < cSamples; ++ii)
    {
        fInput    = pfInput[cChannels * ii + iChannel];
        fExpected = fTargetVolume * fInput;
        fOutput   = pfOutput[cChannels * ii + iChannel];

        if (!InRangeScalar(fExpected, fOutput, (float)0.001))
        {
            cFails--;
            XLOG(
                XFAIL,
                eError,
                "FAIL: Channel %d sample at %d does not compare (Input:%.03f, "
                    "Expected:%.03f, Actual:%.03f).",
                iChannel,
                ii,
                fInput,
                fExpected,
                fOutput);
        }
    
        if (0 == cFails)
        {
            SLOG(
                eError,
                "Already encountered %d errors, aborting...",
                cFailsBeforeAbort);
            
            break;
        }
    }
    
    if (cFailsBeforeAbort == cFails)
    {
        SLOG(
            eInfo1,
            "Eventual volume verified for channel %d: %d samples compared.",
            iChannel,
            ii - iSampleDelay);
    }
}   //   Verify_NoCurve()

//--------------------------------------------------------------------------;
//
//  void Verify_WindowsFadeCurve
//
//  Description:
//      Given the input and output buffers and specifics, verifies that
//          the 'windows fade curve' param curve condition was met.
//
//  Arguments:
//      UNCOMPRESSEDAUDIOFORMAT uaf: Audio format.
//
//      UINT iChannel: Channel to check.
//
//      float fInitialVolume: Initial Volume.
//
//      float fTargetVolume: Target Volume.
//
//      HNSTIME hnstDelay: Delay before applying curve.
//
//      HNSTIME hnstDuration: Duration of curve.
//
//      float *pfInput: Input buffer.
//
//      UINT cbInput: Size of above buffer in bytes.
//
//      float *pfOutput: Output buffer.
//
//      UINT cbOutput: Size of above buffer in bytes.
//
//  Return (void):
//      None.
//
//  History:
//      05/18/07    Fwong       Moving over tests from UMACore.
//
//--------------------------------------------------------------------------;
void Verify_WindowsFadeCurve
(
    UNCOMPRESSEDAUDIOFORMAT     uaf,
    UINT                        iChannel,
    float                       fInitialVolume,
    float                       fTargetVolume,
    HNSTIME                     hnstDelay,
    HNSTIME                     hnstDuration,
    float                      *pfInput,
    UINT                        cbInput,
    float                      *pfOutput,
    UINT                        cbOutput
)
{
    float       fInput;
    float       fExpected;
    float       fOutput;
    float       fDeltaVolume;
    float       fRampVolume;
    UINT        cSamples;
    UINT        cSamplesDuration;
    UINT        ii;
    UINT        uBlock;
    UINT        cChannels;
    UINT        iSampleDelay;
    UINT        iSampleDuration;
    UINT        iRampSample;
    const UINT  cFailsBeforeAbort = c_Volume_FailsBeforeAbort;
    UINT        cFails = cFailsBeforeAbort; 

    //  We only deal with floats...
    if (KSDATAFORMAT_SUBTYPE_IEEE_FLOAT != uaf.guidFormatType)
    {
        XLOG(XFAIL, eError, "FAIL: Format not float.");
        return;
    }
    
    //  Are the streams of different lengths?
    if (cbInput != cbOutput)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Streams are different lengths (%d bytes vs %d bytes).",
            cbInput,
            cbOutput);
        return;
    }
    
    if (0 != (cbInput % (uaf.dwBytesPerSampleContainer * uaf.dwSamplesPerFrame)))
    {
        XLOG(XFAIL, eError, "FAIL: Stream size not aligned to format type.");
        return;
    }
    
    uBlock           = uaf.dwBytesPerSampleContainer * uaf.dwSamplesPerFrame;
    cChannels        = uaf.dwSamplesPerFrame;
    cSamples         = cbInput / uBlock;
    
    iSampleDelay     = (UINT)((double)(uaf.fFramesPerSecond * hnstDelay)
                       / (1000 * HNSTIME1MSEC));
                       
    iSampleDuration  = (UINT)((double)(uaf.fFramesPerSecond *
                                      (hnstDelay + hnstDuration))
                       / (1000 * HNSTIME1MSEC));
                    
    cSamplesDuration = iSampleDuration - iSampleDelay;
    fDeltaVolume     = fTargetVolume - fInitialVolume;
    
    //  Verifying initial volume (during delay)
    SLOG(
        eInfo1,
        "Verifying initial volume (%.03f) (during delay)...",
        fInitialVolume);
        
    cFails = cFailsBeforeAbort;    
    for (ii = 0; ii <= iSampleDelay; ++ii)
    {
        fInput    = pfInput[cChannels * ii + iChannel];
        fExpected = fInitialVolume * fInput;
        fOutput   = pfOutput[cChannels * ii + iChannel];

        if (!InRangeScalar(fExpected, fOutput, (float)0.001))
        {
            cFails--;
            XLOG(
                XFAIL,
                eError,
                "FAIL: Channel %d sample at %d does not compare (Input:%.03f, "
                    "Expected:%.03f, Actual:%.03f).",
                iChannel,
                ii,
                fInput,
                fExpected,
                fOutput);
        }
    
        if (0 == cFails)
        {
            SLOG(
                eError,
                "Already encountered %d errors, aborting...",
                cFailsBeforeAbort);
            
            break;
        }
    }
    
    if (cFailsBeforeAbort == cFails)
    {
        SLOG(
            eInfo1,
            "Initial volume verified for channel %d: %d samples compared.",
            iChannel,
            ii);
    }
    
    //  Verifying ramping volume (through duration)
    SLOG(
        eInfo1,
        "Verifying ramping volume (%.03f to %.03f) (through duration)...",
        fInitialVolume,
        fTargetVolume);
        
    cFails = cFailsBeforeAbort;    
    for (ii = iSampleDelay + 1; ii <= iSampleDuration; ++ii)
    {
        //  Nth sample into duration...
        iRampSample  = ii - iSampleDelay;
        
        //  Getting the 'amount' of delta to apply...
        fRampVolume  = fDeltaVolume * iRampSample / cSamplesDuration;
        
        //  offsetting by Initial volume...
        fRampVolume += fInitialVolume;
        
        fInput       = pfInput[cChannels * ii + iChannel];
        fExpected    = fRampVolume * fInput;
        fOutput      = pfOutput[cChannels * ii + iChannel];

        if (!InRangeScalar(fExpected, fOutput, (float)0.001))
        {
            cFails--;
            XLOG(
                XFAIL,
                eError,
                "FAIL: Channel %d sample at %d does not compare (Input:%.03f, "
                    "Expected:%.03f, Actual:%.03f).",
                iChannel,
                ii,
                fInput,
                fExpected,
                fOutput);
        }
    
        if (0 == cFails)
        {
            SLOG(
                eError,
                "Already encountered %d errors, aborting...",
                cFailsBeforeAbort);
            
            break;
        }
    }
    
    if (cFailsBeforeAbort == cFails)
    {
        SLOG(
            eInfo1,
            "Ramping volume verified for channel %d: %d samples compared.",
            iChannel,
            ii - iSampleDelay);
    }
    
    //  Verifying eventual volume (after curve)
    SLOG(
        eInfo1,
        "Verifying eventual volume (%.03f) (after curve)...",
        fTargetVolume);
        
    cFails = cFailsBeforeAbort;    
    for (ii = iSampleDuration + 1; ii < cSamples; ++ii)
    {
        fInput    = pfInput[cChannels * ii + iChannel];
        fExpected = fTargetVolume * fInput;
        fOutput   = pfOutput[cChannels * ii + iChannel];

        if (!InRangeScalar(fExpected, fOutput, (float)0.001))
        {
            cFails--;
            XLOG(
                XFAIL,
                eError,
                "FAIL: Channel %d sample at %d does not compare (Input:%.03f, "
                    "Expected: %.03f, Actual:%.03f).",
                iChannel,
                ii,
                fInput,
                fExpected,
                fOutput);
        }
    
        if (0 == cFails)
        {
            SLOG(
                eError,
                "Already encountered %d errors, aborting...",
                cFailsBeforeAbort);
            
            break;
        }
    }
    
    if (cFailsBeforeAbort == cFails)
    {
        SLOG(
            eInfo1,
            "Eventual volume verified for channel %d: %d samples compared.",
            iChannel,
            ii - iSampleDuration);
    }
}   //   Verify_WindowsFadeCurve()

//--------------------------------------------------------------------------;
//
//  void VerifyCurve
//
//  Description:
//      Given the input and output buffers and specifics, verifies that
//          the param curve condition was met.
//
//  Arguments:
//      AUDIO_CURVE_TYPE u32Curve: Type of curve.
//
//      UNCOMPRESSEDAUDIOFORMAT uaf: Audio format.
//
//      UINT iChannel: Channel to check.
//
//      float fInitialVolume: Initial Volume.
//
//      float fTargetVolume: Target Volume.
//
//      HNSTIME hnstDelay: Delay before applying curve.
//
//      HNSTIME hnstDuration: Duration of curve.
//
//      float *pfInput: Input buffer.
//
//      UINT cbInput: Size of above buffer in bytes.
//
//      float *pfOutput: Output buffer.
//
//      UINT cbOutput: Size of above buffer in bytes.
//
//  Return (void):
//      None.
//
//  History:
//      05/18/07    Fwong       Moving over tests from UMACore.
//
//--------------------------------------------------------------------------;
void VerifyCurve
(
    AUDIO_CURVE_TYPE           u32Curve,
    UNCOMPRESSEDAUDIOFORMAT     uaf,
    UINT                        iChannel,
    float                       fInitialVolume,
    float                       fTargetVolume,
    HNSTIME                     hnstDelay,
    HNSTIME                     hnstDuration,
    float                      *pfInput,
    UINT                        cbInput,
    float                      *pfOutput,
    UINT                        cbOutput
)
{
    switch (u32Curve)
    {
        case AUDIO_CURVE_TYPE_NONE:
            Verify_NoCurve(
                uaf,
                iChannel,
                fInitialVolume,
                fTargetVolume,
                hnstDelay,
                hnstDuration,
                pfInput,
                cbInput,
                pfOutput,
                cbOutput);
            break;
            
        case AUDIO_CURVE_TYPE_WINDOWS_FADE:
            Verify_WindowsFadeCurve(
                uaf,
                iChannel,
                fInitialVolume,
                fTargetVolume,
                hnstDelay,
                hnstDuration,
                pfInput,
                cbInput,
                pfOutput,
                cbOutput);
            break;
            
        default:
            XLOG(XFAIL, eError, "FAIL: Undefined curve.");
            break;
    }
}   //   VerifyCurve()

//--------------------------------------------------------------------------;
//
//  void Volume_TestCurve
//
//  Description:
//      Verifies setting volume coefficients with the given curve
//          parameters works.
//
//  Arguments:
//      AUDIO_CURVE_TYPE u32Curve: Specified curve parameter.
//
//  Return (void):
//      Nothing.  Failures are reported through XLOG.
//
//  History:
//      05/18/07    Fwong       Moving over tests from UMACore.
//
//--------------------------------------------------------------------------;
void Volume_TestCurve
(
    AUDIO_CURVE_TYPE   u32Curve
)
{
    //  Constants for this test case...
    HNSTIME hnstDelay          = hnst_Volume_DefaultDelay;
    HNSTIME hnstDuration       = hnst_Volume_DefaultDuration;
    const float fInitialVolume = f_Volume_InitialVolume;
    const float fTargetVolume  = f_Volume_TargetVolume;

    //  Variables...
    CAPODevice                                     *pCurrentDevice = NULL;
    CComPtr<IAudioProcessingObject>                 pIAPO = NULL;
    CComPtr<IAudioProcessingObjectRT>               pIAPORT = NULL;
    CComPtr<IAudioProcessingObjectConfiguration>    pIAPOConfig = NULL;
    CComPtr<IAudioVolume>                           pIVol = NULL;
    CComPtr<IAudioMuteAPO>                          pIMute = NULL;
    UINT                                            cChannels = gAPOLimits.Volume_u32SamplesPerFrame;
    UINT                                            ii;
    CAutoBuffer                                     Coefficients(2 * cChannels * sizeof(FLOAT32));
    FLOAT32                                        *pfCoefInitial;
    FLOAT32                                        *pfCoefTarget;
    UNCOMPRESSEDAUDIOFORMAT                         uaf;
    APO_CONNECTION_DESCRIPTOR                       InputConnection;
    APO_CONNECTION_DESCRIPTOR                       OutputConnection;
    APO_CONNECTION_PROPERTY                         InputProperty = {0};
    APO_CONNECTION_PROPERTY                         OutputProperty = {0};
    APO_CONNECTION_DESCRIPTOR                      *pInputConnection;
    APO_CONNECTION_DESCRIPTOR                      *pOutputConnection;
    APO_CONNECTION_PROPERTY                        *pInputProperty;
    APO_CONNECTION_PROPERTY                        *pOutputProperty;
    CComPtr<IAudioMediaType>                        pIInFormat = NULL;
    CComPtr<IAudioMediaType>                        pIOutFormat = NULL;
    CBufferCollection                               Buffers(2);     //  For input and output...
    CAudioBuffer                                   *pInBuffer = NULL;
    CAudioBuffer                                   *pOutBuffer = NULL;
    float                                           fBufferLength = (float)0.0;
    HRESULT                                         hr;
    
    pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    
    if (NULL == pCurrentDevice)
    {
        XLOG(XFAIL, eError, "FAIL: Could not instantiate APO object.");
        return;
    }
    
    if (!pCurrentDevice->GetAPOInterfaces(&pIAPO, &pIAPORT, &pIAPOConfig))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObject interface.");
        return;
    }

    hr = pIAPO->QueryInterface(__uuidof(IAudioVolume), (void**)&pIVol);

    if (E_NOINTERFACE == hr)
    {
        SLOG(eInfo1, "This APO does not implement IAudioVolume.");
        return;
    }
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: QueryInterface for IAudioVolume returns 0x%08lx.",
            hr);
        return;
    }

    hr = pIAPO->QueryInterface(__uuidof(IAudioMuteAPO), (void**)&pIMute);

    if (E_NOINTERFACE == hr)
    {
        SLOG(eInfo1, "This APO does not implement IAudioMuteAPO.");
        return;
    }
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: QueryInterface for IAudioMuteAPO returns 0x%08lx.",
            hr);
        return;
    }

    if (NULL == (pfCoefInitial = (FLOAT32*)Coefficients.GetBuffer()))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Insufficient memory allocating coefficient buffer.");
        return;
    }
    
    pfCoefTarget = &(pfCoefInitial[cChannels]);

    //  This is for both input and output formats...
    uaf.guidFormatType            = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
    uaf.dwSamplesPerFrame         = gAPOLimits.Volume_u32SamplesPerFrame;
    uaf.dwBytesPerSampleContainer = 4;
    uaf.dwValidBitsPerSample      = 32;
    uaf.fFramesPerSecond          = gAPOLimits.Volume_f32FrameRate;
    uaf.dwChannelMask             = 0;

    //  Getting actual format...
    hr = CreateAudioMediaTypeFromUncompressedAudioFormat(&uaf, &pIInFormat);
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioMediaType for format (0x%08lx).",
            hr);
        return;
    }
    
    //  Getting actual format...
    hr = CreateAudioMediaTypeFromUncompressedAudioFormat(&uaf, &pIOutFormat);
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioMediaType for format (0x%08lx).",
            hr);
        return;
    }
    
    pInBuffer = new CAudioBuffer(pIInFormat);
    Buffers.SetBuffer(0, pInBuffer);
    
    if (NULL == pInBuffer)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Insufficient memory allocating input buffer.");
        return;
    }
    
    //  Calculating buffer duration...
    fBufferLength  = (float)0.5;     //  Eventual duration is 1/2 second...
    fBufferLength += (float)(((double)hnstDelay) / (double)(HNSTIME1MSEC * 1000));
    fBufferLength += (float)(((double)hnstDuration) / (double)(HNSTIME1MSEC * 1000));
    
    SLOG(eInfo1, "Test Buffer duration: %.03f seconds.", fBufferLength);
    
    if (!pInBuffer->GenerateBuffer(fBufferLength))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to generate input buffer.");
        return;
    }
    
    pOutBuffer = new CAudioBuffer(pIOutFormat);
    Buffers.SetBuffer(1, pOutBuffer);
    
    if (NULL == pOutBuffer)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Insufficient memory allocating output buffer.");
        return;
    }
    
    if (!pOutBuffer->AllocateBuffer(fBufferLength))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to allocate output buffer.");
        return;
    }
   
    //  Setting up input connection...
    pInputConnection                   = &InputConnection;
    InputConnection.Type               = APO_CONNECTION_BUFFER_TYPE_EXTERNAL;
    InputConnection.pBuffer            = (UINT_PTR)pInBuffer->GetAudioBuffer();
    InputConnection.pFormat            = pIInFormat;
    InputConnection.u32MaxFrameCount   = pInBuffer->GetBufferSize() / 
                                           (uaf.dwSamplesPerFrame * 
                                                uaf.dwBytesPerSampleContainer);
    InputConnection.u32Signature       = APO_CONNECTION_DESCRIPTOR_SIGNATURE;
    
    //  Setting up output connection...
    pOutputConnection                  = &OutputConnection;
    OutputConnection.Type              = APO_CONNECTION_BUFFER_TYPE_EXTERNAL;
    OutputConnection.pBuffer           = (UINT_PTR)pOutBuffer->GetAudioBuffer();
    OutputConnection.pFormat           = pIOutFormat;
    OutputConnection.u32MaxFrameCount  = pOutBuffer->GetBufferSize() / 
                                         (uaf.dwSamplesPerFrame * 
                                             uaf.dwBytesPerSampleContainer);
    OutputConnection.u32Signature      = APO_CONNECTION_DESCRIPTOR_SIGNATURE;
    
    //  Setting up input properties...
    pInputProperty                     = &InputProperty;
    InputProperty.pBuffer              = InputConnection.pBuffer;
    InputProperty.u32ValidFrameCount   = InputConnection.u32MaxFrameCount;
    InputProperty.u32BufferFlags       = BUFFER_VALID;
    InputProperty.u32Signature         = APO_CONNECTION_PROPERTY_SIGNATURE;
    
    //  Setting up output properties...
    pOutputProperty                    = &OutputProperty;
    OutputProperty.pBuffer             = OutputConnection.pBuffer;
    OutputProperty.u32ValidFrameCount  = OutputConnection.u32MaxFrameCount;
    OutputProperty.u32BufferFlags      = BUFFER_INVALID;
    OutputProperty.u32Signature        = APO_CONNECTION_PROPERTY_SIGNATURE;

    //  Make sure we're not muted...  Also setting mute to take immediately.
    if (S_OK != (hr = Log_SetMute(pIMute, false, NULL, AUDIO_CURVE_TYPE_NONE, NULL)))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetMute to unmute returned error (0x%08lx).",
            hr);
        return;
    }
    
    //  Setting initial volume...
    for (ii = cChannels; ii; --ii)
    {
        pfCoefInitial[ii - 1] = fInitialVolume;
    }
    
    hr = Log_SetVolumeCoefficients(
        pIVol,
        cChannels,
        pfCoefInitial,
        NULL,
        AUDIO_CURVE_TYPE_WINDOWS_FADE,
        &hnstDuration);
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetVolumeCoefficients returned error (0x%08lx).",
            hr);
        return;
    }
    
    SLOG(
        eInfo1,
        "SetVolumeCoefficients before LockForProcess set to %.02f.",
        fInitialVolume);
    
    hr = pIAPOConfig->LockForProcess(
        1,
        &pInputConnection,
        1,
        &pOutputConnection);

    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: LockForProcess returned error (0x%08lx).", hr);
        return;
    }
    
    //  Calling process the first time to get rid of
    //      the "No initial ramp" condition...
    pIAPORT->APOProcess(1, &pInputProperty, 1, &pOutputProperty);
    
    //  Reset the status of the output buffer...
    OutputProperty.u32BufferFlags = BUFFER_INVALID;
    
    //  Set the coefficients for real now...
    for (ii = cChannels; ii; --ii)
    {
        pfCoefTarget[ii - 1] = fTargetVolume;
    }
    
    hr = Log_SetVolumeCoefficients(
        pIVol,
        cChannels,
        pfCoefTarget,
        &hnstDelay,
        u32Curve,
        &hnstDuration);
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetVolumeCoefficients returned error (0x%08lx).",
            hr);
    }
    else
    {
        SLOG(
            eInfo1,
            "SetVolumeCoefficients after LockForProcess set to %.02f.",
            fTargetVolume);
    }
    
    //  Now let's go for real.
    pIAPORT->APOProcess(1, &pInputProperty, 1, &pOutputProperty);
    
    if (S_OK != (hr = pIAPOConfig->UnlockForProcess()))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: LockForProcess returned error (0x%08lx).", hr);
        return;
    }
    
    //  In case buffer is marked as AUDCLNT_BUFFERFLAGS_SILENT
    if (S_OK != (hr = ApplySilenceFlag(pOutputConnection, pOutputProperty)))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: ApplySilenceFlag returned error (0x%08lx).",
            hr);
        return;
    }
    
    //  Verify calling SetVolumeCoefficients with windows fade curve works...
    for (ii = cChannels; ii; --ii)
    {
        VerifyCurve(
            u32Curve,
            uaf,
            ii - 1,
            pfCoefInitial[ii - 1],
            pfCoefTarget[ii - 1],
            hnstDelay,
            hnstDuration,
            (float*)pInBuffer->GetAudioBuffer(),
            pInBuffer->GetBufferSize(),
            (float*)pOutBuffer->GetAudioBuffer(),
            pOutBuffer->GetBufferSize());
    }
}   //   Volume_TestCurve()

//--------------------------------------------------------------------------;
//
//  void Mute_TestCurve
//
//  Description:
//      Verifies setting mute/unmute with the given curve parameters
//          works.
//
//  Arguments:
//      AUDIO_CURVE_TYPE u32Curve: Specified curve parameter.
//
//  Return (void):
//      Nothing.  Failures are reported through XLOG.
//
//  History:
//      05/18/07    Fwong       Moving over tests from UMACore.
//
//--------------------------------------------------------------------------;
void Mute_TestCurve
(
    AUDIO_CURVE_TYPE   u32Curve
)
{
    //  Constants for this test case...
    HNSTIME hnstDelay          = hnst_Volume_DefaultDelay;
    HNSTIME hnstDuration       = hnst_Volume_DefaultDuration;
    const float fInitialVolume = f_Volume_InitialVolume;

    //  Variables...
    CAPODevice                                     *pCurrentDevice = NULL;
    CComPtr<IAudioProcessingObject>                 pIAPO = NULL;
    CComPtr<IAudioProcessingObjectRT>               pIAPORT = NULL;
    CComPtr<IAudioProcessingObjectConfiguration>    pIAPOConfig = NULL;
    CComPtr<IAudioVolume>                           pIVol = NULL;
    CComPtr<IAudioMuteAPO>                          pIMute = NULL;
    UINT                                            cChannels = gAPOLimits.Volume_u32SamplesPerFrame;
    UINT                                            ii;
    CAutoBuffer                                     Coefficients(cChannels * sizeof(FLOAT32));
    FLOAT32                                        *pfCoefInitial;
    UNCOMPRESSEDAUDIOFORMAT                         uaf;
    APO_CONNECTION_DESCRIPTOR                       InputConnection;
    APO_CONNECTION_DESCRIPTOR                       OutputConnection;
    APO_CONNECTION_PROPERTY                         InputProperty = {0};
    APO_CONNECTION_PROPERTY                         OutputProperty = {0};
    APO_CONNECTION_DESCRIPTOR                      *pInputConnection;
    APO_CONNECTION_DESCRIPTOR                      *pOutputConnection;
    APO_CONNECTION_PROPERTY                        *pInputProperty;
    APO_CONNECTION_PROPERTY                        *pOutputProperty;
    CComPtr<IAudioMediaType>                        pIInFormat = NULL;
    CComPtr<IAudioMediaType>                        pIOutFormat = NULL;
    CBufferCollection                               Buffers(2);     //  For input and output...
    CAudioBuffer                                   *pInBuffer = NULL;
    CAudioBuffer                                   *pOutBuffer = NULL;
    float                                           fBufferLength = (float)0.0;
    HRESULT                                         hr;
    
    pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    
    if (NULL == pCurrentDevice)
    {
        XLOG(XFAIL, eError, "FAIL: Could not instantiate APO object.");
        return;
    }
    
    if (!pCurrentDevice->GetAPOInterfaces(&pIAPO, &pIAPORT, &pIAPOConfig))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObject interface.");
        return;
    }

    hr = pIAPO->QueryInterface(__uuidof(IAudioVolume), (void**)&pIVol);

    if (E_NOINTERFACE == hr)
    {
        SLOG(eInfo1, "This APO does not implement IAudioVolume.");
        return;
    }
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: QueryInterface for IAudioVolume returns 0x%08lx.",
            hr);
        return;
    }

    hr = pIAPO->QueryInterface(__uuidof(IAudioMuteAPO), (void**)&pIMute);

    if (E_NOINTERFACE == hr)
    {
        SLOG(eInfo1, "This APO does not implement IAudioMuteAPO.");
        return;
    }
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: QueryInterface for IAudioMuteAPO returns 0x%08lx.",
            hr);
        return;
    }

    if (NULL == (pfCoefInitial = (FLOAT32*)Coefficients.GetBuffer()))
    {
        XLOG(XFAIL, eError, "FAIL: Insufficient memory.");
        return;
    }
    
    //  This is for both input and output formats...
    uaf.guidFormatType            = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
    uaf.dwSamplesPerFrame         = gAPOLimits.Volume_u32SamplesPerFrame;
    uaf.dwBytesPerSampleContainer = 4;
    uaf.dwValidBitsPerSample      = 32;
    uaf.fFramesPerSecond          = gAPOLimits.Volume_f32FrameRate;
    uaf.dwChannelMask             = 0;

    //  Getting actual format...
    hr = CreateAudioMediaTypeFromUncompressedAudioFormat(&uaf, &pIInFormat);
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioMediaType for format (0x%08lx).",
            hr);
        return;
    }
    
    //  Getting actual format...
    hr = CreateAudioMediaTypeFromUncompressedAudioFormat(&uaf, &pIOutFormat);
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioMediaType for format (0x%08lx).",
            hr);
        return;
    }
    
    pInBuffer = new CAudioBuffer(pIInFormat);
    Buffers.SetBuffer(0, pInBuffer);
    
    if (NULL == pInBuffer)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Insufficient memory allocating input buffer.");
        return;
    }
    
    //  Calculating buffer duration...
    fBufferLength  = (float)0.5;     //  Eventual duration is 1/2 second...
    fBufferLength += (float)(((double)hnstDelay) / (double)(HNSTIME1MSEC * 1000));
    fBufferLength += (float)(((double)hnstDuration) / (double)(HNSTIME1MSEC * 1000));
    
    SLOG(eInfo1, "Test Buffer duration: %.03f seconds.", fBufferLength);
    
    if (!pInBuffer->GenerateBuffer(fBufferLength))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to generate input buffer.");
        return;
    }
    
    pOutBuffer = new CAudioBuffer(pIOutFormat);
    Buffers.SetBuffer(1, pOutBuffer);
    
    if (NULL == pOutBuffer)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Insufficient memory allocating output buffer.");
        return;
    }
    
    if (!pOutBuffer->AllocateBuffer(fBufferLength))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to allocate output buffer.");
        return;
    }
   
    //  Setting up input connection...
    pInputConnection                   = &InputConnection;
    InputConnection.Type               = APO_CONNECTION_BUFFER_TYPE_EXTERNAL;
    InputConnection.pBuffer            = (UINT_PTR)pInBuffer->GetAudioBuffer();
    InputConnection.pFormat            = pIInFormat;
    InputConnection.u32MaxFrameCount   = pInBuffer->GetBufferSize() / 
                                           (uaf.dwSamplesPerFrame * 
                                                uaf.dwBytesPerSampleContainer);
    InputConnection.u32Signature       = APO_CONNECTION_DESCRIPTOR_SIGNATURE;
    
    //  Setting up output connection...
    pOutputConnection                  = &OutputConnection;
    OutputConnection.Type              = APO_CONNECTION_BUFFER_TYPE_EXTERNAL;
    OutputConnection.pBuffer           = (UINT_PTR)pOutBuffer->GetAudioBuffer();
    OutputConnection.pFormat           = pIOutFormat;
    OutputConnection.u32MaxFrameCount  = pOutBuffer->GetBufferSize() / 
                                         (uaf.dwSamplesPerFrame * 
                                             uaf.dwBytesPerSampleContainer);
    OutputConnection.u32Signature      = APO_CONNECTION_DESCRIPTOR_SIGNATURE;
    
    //  Setting up input properties...
    pInputProperty                     = &InputProperty;
    InputProperty.pBuffer              = InputConnection.pBuffer;
    InputProperty.u32ValidFrameCount   = InputConnection.u32MaxFrameCount;
    InputProperty.u32BufferFlags       = BUFFER_VALID;
    InputProperty.u32Signature         = APO_CONNECTION_PROPERTY_SIGNATURE;
    
    //  Setting up output properties...
    pOutputProperty                    = &OutputProperty;
    OutputProperty.pBuffer             = OutputConnection.pBuffer;
    OutputProperty.u32ValidFrameCount  = OutputConnection.u32MaxFrameCount;
    OutputProperty.u32BufferFlags      = BUFFER_INVALID;
    OutputProperty.u32Signature        = APO_CONNECTION_PROPERTY_SIGNATURE;

    //  Make sure we're not muted...  Also setting mute to take immediately.
    if (S_OK != (hr = Log_SetMute(pIMute, false, NULL, AUDIO_CURVE_TYPE_NONE, NULL)))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetMute to unmute returned error (0x%08lx).",
            hr);
        return;
    }
    
    //  Setting initial volume...
    for (ii = cChannels; ii; --ii)
    {
        pfCoefInitial[ii - 1] = fInitialVolume;
    }
    
    hr = Log_SetVolumeCoefficients(
        pIVol,
        cChannels,
        pfCoefInitial,
        NULL,
        AUDIO_CURVE_TYPE_NONE,
        &hnstDuration);
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetVolumeCoefficients returned error (0x%08lx).",
            hr);
        return;
    }
    
    SLOG(
        eInfo1,
        "SetVolumeCoefficients before LockForProcess set to %.02f.",
        fInitialVolume);
    
    hr = pIAPOConfig->LockForProcess(
        1,
        &pInputConnection,
        1,
        &pOutputConnection);

    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: LockForProcess returned error (0x%08lx).", hr);
        return;
    }
    
    //  Calling process the first time to get rid of
    //      the "No initial ramp" condition...
    pIAPORT->APOProcess(1, &pInputProperty, 1, &pOutputProperty);
    
    //  Reset the status of the output buffer...
    OutputProperty.u32BufferFlags = BUFFER_INVALID;

    //  Muting...    
    hr = Log_SetMute(pIMute, true, &hnstDelay, u32Curve, &hnstDuration);
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetMute to muted returned error (0x%08lx).",
            hr);
    }
    else
    {
        SLOG(eInfo1, "SetMute to muted after LockForProcess.");
    }
    
    //  Now let's go for real.
    pIAPORT->APOProcess(1, &pInputProperty, 1, &pOutputProperty);

    //  In case buffer is marked as AUDCLNT_BUFFERFLAGS_SILENT
    if (S_OK != (hr = ApplySilenceFlag(pOutputConnection, pOutputProperty)))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: ApplySilenceFlag returned error (0x%08lx).",
            hr);
        return;
    }

    //  Verify calling SetVolumeCoefficients with windows fade curve works...
    SLOG(eInfo1, "Verifying the mute.");
    for (ii = cChannels; ii; --ii)
    {
        VerifyCurve(
            u32Curve,
            uaf,
            ii - 1,
            pfCoefInitial[ii - 1],
            0.0,
            hnstDelay,
            hnstDuration,
            (float*)pInBuffer->GetAudioBuffer(),
            pInBuffer->GetBufferSize(),
            (float*)pOutBuffer->GetAudioBuffer(),
            pOutBuffer->GetBufferSize());
    }
    
    //  Now, trying the unmute!
    
    //  Reset the status of the output buffer...
    OutputProperty.u32BufferFlags = BUFFER_INVALID;

    //  Unmuting...    
    hr = Log_SetMute(pIMute, false, &hnstDelay, u32Curve, &hnstDuration);
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetMute to unmuted returned error (0x%08lx).",
            hr);
    }
    else
    {
        SLOG(eInfo1, "SetMute to unmuted after LockForProcess.");
    }
    
    //  Now let's go for real.
    pIAPORT->APOProcess(1, &pInputProperty, 1, &pOutputProperty);

    //  In case buffer is marked as AUDCLNT_BUFFERFLAGS_SILENT
    if (S_OK != (hr = ApplySilenceFlag(pOutputConnection, pOutputProperty)))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: ApplySilenceFlag returned error (0x%08lx).",
            hr);
        return;
    }
    
    //  Verify calling SetVolumeCoefficients with windows fade curve works...
    SLOG(eInfo1, "Verifying the unmute.");
    for (ii = cChannels; ii; --ii)
    {
        VerifyCurve(
            u32Curve,
            uaf,
            ii - 1,
            0.0,
            pfCoefInitial[ii - 1],
            hnstDelay,
            hnstDuration,
            (float*)pInBuffer->GetAudioBuffer(),
            pInBuffer->GetBufferSize(),
            (float*)pOutBuffer->GetAudioBuffer(),
            pOutBuffer->GetBufferSize());
    }
    
    if (S_OK != (hr = pIAPOConfig->UnlockForProcess()))
    {
        XLOG(XFAIL, eError, "FAIL: LockForProcess returned error (0x%08lx).", hr);
        return;
    }
}   //   Mute_TestCurve()

//--------------------------------------------------------------------------;
//
//  DWORD TC_CAudioVolume_VerifyRegistration
//
//  Description:
//      Verifies registration properties are ones that we expect.
//
//  Arguments:
//      None.
//
//  Return (DWORD):
//      Optional return code for test case.
//
//  History:
//      05/18/07    Fwong       Moving over tests from UMACore.
//
//--------------------------------------------------------------------------;
DWORD WINAPI TC_CAudioVolume_VerifyRegistration()
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
    
    if (!IsVolumeAPO(pCurrentDevice))
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
        if (0x0000000f == pApoRegProperties->Flags)
        {
            SLOG(eInfo1, "Flags value is 0x0000000f.");
        }
        else
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: Flags value is 0x%08lx not 0x0000000f.",
                pApoRegProperties->Flags);
        }
        
        //  Verify APO Interfaces.
        if (2 == pApoRegProperties->u32NumAPOInterfaces)
        {
            SLOG(eInfo1, "Number of APO interfaces is 2.");
        }
        else
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: Number of APO interfaces is %d; expecting 2.",
                pApoRegProperties->u32NumAPOInterfaces);
        }
        
        //  Looking for IAudioVolume
        for (ii = pApoRegProperties->u32NumAPOInterfaces; ii; --ii)
        {
            if (IsEqualGUID(
                pApoRegProperties->iidAPOInterfaceList[ii-1],
                __uuidof(IAudioVolume)))
            {
                CComPtr<IUnknown>   pIUnknown = NULL;
                
                SLOG(eInfo1, "IAudioVolume interface listed.");
                
                hr = pIAPO->QueryInterface(
                    pApoRegProperties->iidAPOInterfaceList[ii-1],
                    (void**)&pIUnknown);
            
                if (S_OK != hr)
                {
                    XLOG(
                        XFAIL,
                        eError,
                        "FAIL: QueryInterface for IAudioVolume returns 0x%08lx.",
                        hr);
                }
                else
                {
                    SLOG(eInfo1, "QueryInterface for IAudioVolume succeeds.");
                }
                
                break;
            }
        }
        if (0 == ii)
        {
            //  IAudioVolume not found...
            XLOG(
                XFAIL,
                eError,
                "FAIL: IAudioVolume interface not listed.");
        }
    
        //  Looking for IAudioMuteAPO    
        for (ii = pApoRegProperties->u32NumAPOInterfaces; ii; --ii)
        {
            if (IsEqualGUID(
                pApoRegProperties->iidAPOInterfaceList[ii-1],
                __uuidof(IAudioMuteAPO)))
            {
                CComPtr<IUnknown>   pIUnknown = NULL;
                
                SLOG(eInfo1, "IAudioMuteAPO interface listed.");
                
                hr = pIAPO->QueryInterface(
                    pApoRegProperties->iidAPOInterfaceList[ii-1],
                    (void**)&pIUnknown);
            
                if (S_OK != hr)
                {
                    XLOG(
                        XFAIL,
                        eError,
                        "FAIL: QueryInterface for IAudioMuteAPO returns 0x%08lx.",
                        hr);
                }
                else
                {
                    SLOG(eInfo1, "QueryInterface for IAudioMuteAPO succeeds.");
                }
                
                break;
            }
        }
        if (0 == ii)
        {
            //  IAudioMuteAPO not found...
            XLOG(
                XFAIL,
                eError,
                "FAIL: IAudioMuteAPO interface not listed.");
        }
    }

    return (FNS_PASS);
}   //   TC_CAudioVolume_VerifyRegistration()

//--------------------------------------------------------------------------;
//
//  DWORD TC_CAudioVolume_Initialize
//
//  Description:
//      Tests conditions surrounding Initialize()
//
//  Arguments:
//      None.
//
//  Return (DWORD):
//      Optional return code for test case.
//
//  History:
//      05/18/07    Fwong       Moving over tests from UMACore.
//
//--------------------------------------------------------------------------;
DWORD WINAPI TC_CAudioVolume_Initialize()
{
    CAPODevice                         *pCurrentDevice = NULL;
    CComPtr<IAudioProcessingObject>     pIAPO = NULL;
    APO_VOLUME_PARAMS                   APOParams = {0};
    GUID                                guidAPO;
    HRESULT                             hr;
    
    pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    
    if (NULL == pCurrentDevice)
    {
        XLOG(XFAIL, eError, "FAIL: Could not instantiate APO object.");
        return (FNS_PASS);
    }
    
    pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    
    if (!IsVolumeAPO(pCurrentDevice))
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
    APOParams.u32SamplesPerFrame = 0;
    APOParams.f32FrameRate       = 0.0;
    TestAPO_Initialize(
        guidAPO,
        sizeof(APOParams),
        (BYTE*)&APOParams,
        "with all zero params",
        APOERR_FORMAT_NOT_SUPPORTED);

    //  Initialize with channels = 0;
    APOParams.u32SamplesPerFrame = 0;
    APOParams.f32FrameRate       = gAPOLimits.Volume_f32FrameRate;
    TestAPO_Initialize(
        guidAPO,
        sizeof(APOParams),
        (BYTE*)&APOParams,
        "with channels = 0",
        APOERR_FORMAT_NOT_SUPPORTED);

    //  Initialize with framerate = 0.0;
    APOParams.u32SamplesPerFrame = gAPOLimits.Volume_u32SamplesPerFrame;
    APOParams.f32FrameRate       = 0.0;
    TestAPO_Initialize(
        guidAPO,
        sizeof(APOParams),
        (BYTE*)&APOParams,
        "with framerate = 0",
        APOERR_FORMAT_NOT_SUPPORTED);
    
    //  Initialize with channels = MAX_UINT32;
    APOParams.u32SamplesPerFrame = (UINT32)(-1);
    APOParams.f32FrameRate       = gAPOLimits.Volume_f32FrameRate;
    TestAPO_Initialize(
        guidAPO,
        sizeof(APOParams),
        (BYTE*)&APOParams,
        "with channels = MAX_UINT32",
        APOERR_FORMAT_NOT_SUPPORTED);

    //  channels > max;
    APOParams.u32SamplesPerFrame = AUDIO_MAX_CHANNELS + 1;
    APOParams.f32FrameRate       = gAPOLimits.Volume_f32FrameRate;
    TestAPO_Initialize(
        guidAPO,
        sizeof(APOParams),
        (BYTE*)&APOParams,
        "with channels > max",
        APOERR_FORMAT_NOT_SUPPORTED);
    
    //  channels < min;
    APOParams.u32SamplesPerFrame = AUDIO_MIN_CHANNELS - 1;
    APOParams.f32FrameRate       = gAPOLimits.Volume_f32FrameRate;
    TestAPO_Initialize(
        guidAPO,
        sizeof(APOParams),
        (BYTE*)&APOParams,
        "with channels < min",
        APOERR_FORMAT_NOT_SUPPORTED);

    //  framerate > max;
    APOParams.u32SamplesPerFrame = gAPOLimits.Volume_u32SamplesPerFrame;
    APOParams.f32FrameRate       = (float)(AUDIO_MAX_FRAMERATE + 1.0);
    TestAPO_Initialize(
        guidAPO,
        sizeof(APOParams),
        (BYTE*)&APOParams,
        "with framerate > max",
        APOERR_FORMAT_NOT_SUPPORTED);
    
    //  channels < min;
    APOParams.u32SamplesPerFrame = gAPOLimits.Volume_u32SamplesPerFrame;
    APOParams.f32FrameRate       = (float)(AUDIO_MIN_FRAMERATE - 1.0);
    TestAPO_Initialize(
        guidAPO,
        sizeof(APOParams),
        (BYTE*)&APOParams,
        "with framerate < min",
        APOERR_FORMAT_NOT_SUPPORTED);

    //  Calling initialize twice...  (First time when device was added)
    APOParams.u32SamplesPerFrame = gAPOLimits.Volume_u32SamplesPerFrame;
    APOParams.f32FrameRate       = gAPOLimits.Volume_f32FrameRate;
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
    
    return (FNS_PASS);
}   //   TC_CAudioVolume_Initialize()

//--------------------------------------------------------------------------;
//
//  DWORD TC_CAudioVolume_States
//
//  Description:
//      Verifies that SetVolumeCoefficients() and SetMute() calls don't
//          affect the other's state.
//
//  Arguments:
//      None.
//
//  Return (DWORD):
//      Optional return code for test case.
//
//  History:
//      05/18/07    Fwong       Moving over tests from UMACore.
//
//--------------------------------------------------------------------------;
DWORD WINAPI TC_CAudioVolume_States()
{
    //  Constants for this test case...
    HNSTIME hnstDuration       = hnst_Volume_DefaultDuration;
    const float fTargetVolume  = f_Volume_TargetVolume;
    
    //  Variables...
    CAPODevice                                     *pCurrentDevice = NULL;
    CComPtr<IAudioProcessingObject>                 pIAPO = NULL;
    CComPtr<IAudioProcessingObjectRT>               pIAPORT = NULL;
    CComPtr<IAudioProcessingObjectConfiguration>    pIAPOConfig = NULL;
    CComPtr<IAudioVolume>                           pIVol = NULL;
    CComPtr<IAudioMuteAPO>                          pIMute = NULL;
    UINT                                            cChannels = gAPOLimits.Volume_u32SamplesPerFrame;
    UINT                                            ii;
    boolean                                         bMute;
    CAutoBuffer                                     Coefficients(2 * cChannels * sizeof(FLOAT32));
    FLOAT32                                        *pfCoef;
    FLOAT32                                        *pfCoefQuery;
    HRESULT                                         hr;
    
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

    hr = pIAPO->QueryInterface(__uuidof(IAudioVolume), (void**)&pIVol);

    if (E_NOINTERFACE == hr)
    {
        SLOG(eInfo1, "This APO does not implement IAudioVolume.");
        return (FNS_PASS);
    }
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: QueryInterface for IAudioVolume returns 0x%08lx.",
            hr);
        return (FNS_PASS);
    }

    hr = pIAPO->QueryInterface(__uuidof(IAudioMuteAPO), (void**)&pIMute);

    if (E_NOINTERFACE == hr)
    {
        SLOG(eInfo1, "This APO does not implement IAudioMuteAPO.");
        return (FNS_PASS);
    }
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: QueryInterface for IAudioMuteAPO returns 0x%08lx.",
            hr);
        return (FNS_PASS);
    }

    if (NULL == (pfCoef = (FLOAT32*)Coefficients.GetBuffer()))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Out of memory.");
        return (FNS_PASS);
    }
    
    //  Note:  Allocating one buffer for two sets of coefficients.
    pfCoefQuery = &(pfCoef[cChannels]);
    
    //  Verifying setting mute does not change volume values...
    for (ii = cChannels; ii; --ii)
    {
        pfCoef[ii - 1] = fTargetVolume;
    }
    
    hr = Log_SetVolumeCoefficients(
        pIVol,
        cChannels,
        pfCoef,
        NULL,
        AUDIO_CURVE_TYPE_NONE,
        &hnstDuration);
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetVolumeCoefficients returned error (0x%08lx).",
            hr);
        return (FNS_PASS);
    }
    
    hr = Log_SetMute(pIMute, true, NULL, AUDIO_CURVE_TYPE_NONE, &hnstDuration);
    
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: SetMute returned error (0x%08lx).", hr);
        return (FNS_PASS);
    }
    
    hr = pIVol->GetVolumeCoefficients(cChannels, pfCoefQuery);
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: GetVolumeCoefficients returned error (0x%08lx).",
            hr);
        return (FNS_PASS);
    }
    
    for (ii = cChannels; ii; --ii)
    {
        if (!InRangeScalar(pfCoef[ii - 1], pfCoefQuery[ii - 1], (float)0.00001))
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: SetMute affects the volume coefficients "
                    "(Expected: %.03f, Actual: %03f).",
                pfCoef[ii - 1],
                pfCoefQuery[ii - 1]);
        
            break;
        }
    }
    if (0 == ii)
    {
        SLOG(eInfo1, "SetMute does not affect the volume coefficients.");
    }
    
    //  Verifying setting volume to zero does not affect the 'unmuted' state...
    hr = Log_SetMute(pIMute, false, NULL, AUDIO_CURVE_TYPE_NONE, &hnstDuration);
    
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: SetMute returned error (0x%08lx).", hr);
        return (FNS_PASS);
    }
    
    for (ii = cChannels; ii; --ii)
    {
        pfCoef[ii - 1] = 0.0;
    }
    
    hr = Log_SetVolumeCoefficients(
        pIVol,
        cChannels,
        pfCoef,
        NULL,
        AUDIO_CURVE_TYPE_NONE,
        &hnstDuration);
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetVolumeCoefficients returned error (0x%08lx).",
            hr);
        return (FNS_PASS);
    }
    
    hr = pIMute->GetMute(&bMute);
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: GetMute returned error (0x%08lx).",
            hr);
        return (FNS_PASS);
    }
    
    if (bMute)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Setting volume to 0.0 sets the mute state to muted.");
    }
    else
    {
        SLOG(eInfo1, "Setting volume to 0.0 does not affect the mute state.");
    }
    
    return (FNS_PASS);
}   //   TC_CAudioVolume_States()

//--------------------------------------------------------------------------;
//
//  DWORD TC_IAudioVolume_SetVolumeCoefficients
//
//  Description:
//      Tests conditions surrounding SetVolumeCoefficients().
//
//  Arguments:
//      None.
//
//  Return (DWORD):
//      Optional return code for test case.
//
//  History:
//      05/18/07    Fwong       Moving over tests from UMACore.
//
//--------------------------------------------------------------------------;
DWORD WINAPI TC_IAudioVolume_SetVolumeCoefficients()
{
    //  Constants for this test case...
    HNSTIME hnstDelay          = hnst_Volume_DefaultDelay;
    HNSTIME hnstDuration       = hnst_Volume_DefaultDuration;
    const float fInitialVolume = f_Volume_InitialVolume;
    
    CAPODevice                         *pCurrentDevice = NULL;
    CComPtr<IAudioProcessingObject>     pIAPO = NULL;
    CComPtr<IAudioVolume>               pIVol = NULL;
    UINT                                cChannels = gAPOLimits.Volume_u32SamplesPerFrame;
    CAutoBuffer                         Coefficients(2 * cChannels * sizeof(FLOAT32));
    UINT                                ii, jj;
    FLOAT32                            *pfCoef;
    FLOAT32                            *pfCoefGet;
    HRESULT                             hr;
    
    pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    
    if (NULL == pCurrentDevice)
    {
        XLOG(XFAIL, eError, "FAIL: Could not instantiate APO object.");
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

    hr = pIAPO->QueryInterface(__uuidof(IAudioVolume), (void**)&pIVol);

    if (E_NOINTERFACE == hr)
    {
        SLOG(eInfo1, "This APO does not implement IAudioVolume.");
        return (FNS_PASS);
    }
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: QueryInterface for IAudioVolume returns 0x%08lx.",
            hr);
        return (FNS_PASS);
    }
    
    SLOG(eInfo1, "QueryInterface for IAudioVolume succeeds.");
    
    if (NULL == (pfCoef = (FLOAT32*)Coefficients.GetBuffer()))
    {
        XLOG(XFAIL, eError, "FAIL: Insufficient memory.");
        return (FNS_PASS);
    }
    
    //  Note:  Allocating one buffer for two sets of coefficients.
    pfCoefGet = &(pfCoef[cChannels]);

    //  Default values...
    for (ii = cChannels; ii; --ii)
    {
        pfCoef[ii - 1] = fInitialVolume;
    }
    
    //
    //  Listing failure cases first!!
    //

    //  Trying Set Vol Coeffns w/o initializing samples per frame...
    {
        CComPtr<IAudioProcessingObject>     pIAPOTest = NULL;
        GUID                                guidAPO;

        //  Note: Using new IAudioProcessingObject since 'current' one is
        //        Initialize'ed.
        
        pCurrentDevice->GetClsID(&guidAPO);
        
        if (S_OK != (hr = pIAPOTest.CoCreateInstance(guidAPO)))
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: CoCreateInstance returned error (0x%08lx).",
                hr);
        }
        else
        {
            CComPtr<IAudioVolume>               pIVolTest = NULL;
            
            hr = pIAPOTest->QueryInterface(
                __uuidof(IAudioVolume),
                (void**)&pIVolTest);
        
            if (S_OK != hr)
            {
                XLOG(
                    XFAIL,
                    eError,
                    "FAIL: QueryInterface for IAudioVolume returned "
                        "error (0x%08lx).",
                    hr);
            }
            else
            {
                hr = pIVolTest->SetVolumeCoefficients(
                    cChannels,
                    pfCoef,
                    NULL,
                    AUDIO_CURVE_TYPE_WINDOWS_FADE,
                    &hnstDuration);
            
                if (APOERR_NOT_INITIALIZED != hr)
                {
                    XLOG(
                        XFAIL,
                        eError,
                        "FAIL: SetVolumeCoefficients before Initialize "
                            "returned 0x%08lx.",
                        hr);
                }
                else
                {
                    SLOG(
                        eInfo1,
                        "SetVolumeCoefficients before Initialize returned "
                            "APOERR_NOT_INITIALIZED.");
                }
            }
        }
    }

    //  Invalid u32CountOfCoefficients
    hr = pIVol->SetVolumeCoefficients(
        cChannels - 1,
        pfCoef,
        NULL,
        AUDIO_CURVE_TYPE_WINDOWS_FADE,
        &hnstDuration);
        
    if (APOERR_INVALID_COEFFCOUNT != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetVolumeCoefficients with invalid u32CountOfCoefficients"
                " (channels-1) returned 0x%08lx.",
            hr);
    }
    else
    {
        SLOG(
            eInfo1,
            "SetVolumeCoefficients with invalid u32CountOfCoefficients "
                "(channels-1) returned APOERR_INVALID_COEFFCOUNT.");
    }

    hr = pIVol->SetVolumeCoefficients(
        cChannels + 1,
        pfCoef,
        NULL,
        AUDIO_CURVE_TYPE_WINDOWS_FADE,
        &hnstDuration);
        
    if (APOERR_INVALID_COEFFCOUNT != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetVolumeCoefficients with invalid u32CountOfCoefficients"
                " (channels+1) returned 0x%08lx.",
            hr);
    }
    else
    {
        SLOG(
            eInfo1,
            "SetVolumeCoefficients with invalid u32CountOfCoefficients "
                "(channels+1) returned APOERR_INVALID_COEFFCOUNT.");
    }

    //  NULL pf32Coefficients
#pragma prefast(suppress:6309, "Doing parameter validation.") 
    hr = pIVol->SetVolumeCoefficients(
        cChannels,
        NULL,
        NULL,
        AUDIO_CURVE_TYPE_WINDOWS_FADE,
        &hnstDuration);
        
    if (E_POINTER != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetVolumeCoefficients with NULL pf32Coefficients "
                "returned 0x%08lx.",
            hr);
    }
    else
    {
        SLOG(
            eInfo1,
            "SetVolumeCoefficients with NULL pf32Coefficients "
                "returned E_POINTER.");
    }
    
    //  Invalid Delay
    {
        HNSTIME     hnstBadDelay = -1;
    
        hr = pIVol->SetVolumeCoefficients(
            cChannels,
            pfCoef,
            &hnstBadDelay,
            AUDIO_CURVE_TYPE_WINDOWS_FADE,
            &hnstDuration);
    }
    
    if (APOERR_INVALID_CURVE_PARAM != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetVolumeCoefficients with negative delay (pTime) "
                "returned 0x%08lx.",
            hr);
    }
    else
    {
        SLOG(
            eInfo1,
            "SetVolumeCoefficients with negative delay (pTime) "
                "returned APOERR_INVALID_CURVE_PARAM.");
    }

    //  Invalid Curve Type
    {
        AUDIO_CURVE_TYPE   u32Curve;
        
        *((UINT32*)&u32Curve) = (UINT32)(-1);
        
        hr = pIVol->SetVolumeCoefficients(
            cChannels,
            pfCoef,
            NULL,
            u32Curve,
            &hnstDuration);
    }
        
    if (APOERR_INVALID_CURVE_PARAM != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetVolumeCoefficients with invalid u32CurveType "
                "returned 0x%08lx.",
            hr);
    }
    else
    {
        SLOG(
            eInfo1,
            "SetVolumeCoefficients with invalid u32CurveType "
                "returned APOERR_INVALID_CURVE_PARAM.");
    }

    //  NULL pCurveDuration
    hr = pIVol->SetVolumeCoefficients(
        cChannels,
        pfCoef,
        NULL,
        AUDIO_CURVE_TYPE_WINDOWS_FADE,
        NULL);
        
    if (APOERR_INVALID_CURVE_PARAM != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetVolumeCoefficients with NULL pCurveDuration "
                "returned 0x%08lx.",
            hr);
    }
    else
    {
        SLOG(
            eInfo1,
            "SetVolumeCoefficients with NULL pCurveDuration "
                "returned APOERR_INVALID_CURVE_PARAM.");
    }
    
    //  Negative curve duration.
    {
        HNSTIME     hnstBadDuration = -1 * hnstDuration;
    
        hr = pIVol->SetVolumeCoefficients(
            cChannels,
            pfCoef,
            NULL,
            AUDIO_CURVE_TYPE_WINDOWS_FADE,
            &hnstBadDuration);
    }
    
    if (APOERR_INVALID_CURVE_PARAM != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetVolumeCoefficients with negative pCurveDuration "
                "returned 0x%08lx.",
            hr);
    }
    else
    {
        SLOG(
            eInfo1,
            "SetVolumeCoefficients with negative pCurveDuration "
                "returned APOERR_INVALID_CURVE_PARAM.");
    }

    //  Trying limit and NaN values for coefficients...
    for (ii = 0; ; ++ii)
    {
        char        szDesc[NANNAME_SIZE];
        HRESULT     hr;
        
        for (jj = cChannels; jj; --jj)
        {
            //  Note: ii is the index of the NaN value, so all coefficients
            //        get the same value...
        
            hr = GetNextFloat32NaNWithLabel(
                ii,
                FLAG_NAN_ALL_BUT_ZERO,
                szDesc,
                ARRAYSIZE(szDesc),
                &(pfCoef[jj - 1]));
        
            if (S_OK != hr)
            {
                //  There are no more NaN values.
                break;
            }
        }
        
        if (0 != jj)
        {
            //  If we didn't cycle through all the channels, this means that
            //    we've cycled through all possible NaN values.
            break;
        }
        
        hr = pIVol->SetVolumeCoefficients(
            cChannels,
            pfCoef,
            NULL,
            AUDIO_CURVE_TYPE_WINDOWS_FADE,
            &hnstDuration);
    
        if (APOERR_INVALID_COEFFICIENT != hr)
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: SetVolumeCoefficients with special coefficient (%s) "
                    "returned 0x%08lx.",
                szDesc,
                hr);
        }
        else
        {
            SLOG(
                eInfo1,
                "SetVolumeCoefficients with special coefficient (%s) returned "
                    "APOERR_INVALID_COEFFICIENT.",
                szDesc);
        }
    }

    //
    //  Listing success cases next!!
    //

    //  Trying unity gain and no curve...
    for (ii = cChannels; ii; --ii)
    {
        pfCoef[ii - 1] = 1.0;
    }
    
    hr = pIVol->SetVolumeCoefficients(
        cChannels,
        pfCoef,
        NULL,
        AUDIO_CURVE_TYPE_NONE,
        &hnstDuration);

    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetVolumeCoefficients with AUDIO_CURVE_TYPE_NONE "
                "returned error (0x%08lx).",
            hr);
    }
    else
    {
        SLOG(eInfo1, "SetVolumeCoefficients with AUDIO_CURVE_TYPE_NONE succeeds.");
    }
    
    //  Trying unity gain and windows fade curve...
    for (ii = cChannels; ii; --ii)
    {
        pfCoef[ii - 1] = 1.0;
    }
    
    hr = pIVol->SetVolumeCoefficients(
        cChannels,
        pfCoef,
        NULL,
        AUDIO_CURVE_TYPE_WINDOWS_FADE,
        &hnstDuration);

    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetVolumeCoefficients with AUDIO_CURVE_TYPE_WINDOWS_FADE "
                "returned error (0x%08lx).",
            hr);
    }
    else
    {
        SLOG(eInfo1, "SetVolumeCoefficients with AUDIO_CURVE_TYPE_WINDOWS_FADE succeeds.");
    }
    
    //  Trying unity gain and delay
    hr = pIVol->SetVolumeCoefficients(
        cChannels,
        pfCoef,
        &hnstDelay,
        AUDIO_CURVE_TYPE_WINDOWS_FADE,
        &hnstDuration);

    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetVolumeCoefficients with pTime (Delay) of 10ms "
                "returned error (0x%08lx).",
            hr);
    }
    else
    {
        SLOG(
            eInfo1,
            "SetVolumeCoefficients with pTime (Delay) of 10ms succeeds.");
    }

    //  Trying negative values for coefficients...
    for (ii = cChannels; ii; --ii)
    {
        pfCoef[ii - 1] = -1.0;
    }
    
    hr = pIVol->SetVolumeCoefficients(
        cChannels,
        pfCoef,
        NULL,
        AUDIO_CURVE_TYPE_WINDOWS_FADE,
        &hnstDuration);

    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetVolumeCoefficients with negative coefficient (-1.0) "
                "returned error (0x%08lx).",
            hr);
    }
    else
    {
        SLOG(
            eInfo1,
            "SetVolumeCoefficients with negative coefficient (-1.0) succeeds.");
    }
    
    //  Trying large values for coefficients...
    for (ii = cChannels; ii; --ii)
    {
        pfCoef[ii - 1] = 10.0;
    }
    
    hr = pIVol->SetVolumeCoefficients(
        cChannels,
        pfCoef,
        NULL,
        AUDIO_CURVE_TYPE_WINDOWS_FADE,
        &hnstDuration);

    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetVolumeCoefficients with large coefficient (10.0) "
                "returned error (0x%08lx).",
            hr);
    }
    else
    {
        SLOG(
            eInfo1,
            "SetVolumeCoefficients with large coefficient (10.0) succeeds.");
    }

    //  Getting the values we just set...
    if (S_OK != (hr = pIVol->GetVolumeCoefficients(cChannels, pfCoefGet)))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: GetVolumeCoefficients returned error (0x%08lx).",
            hr);
    }
    else
    {
        BOOL    fSame = TRUE;
    
        for (ii = cChannels; ii; --ii)
        {
            if (pfCoefGet[ii - 1] != pfCoef[ii - 1])
            {
                XLOG(
                    XFAIL,
                    eError,
                    "FAIL: Coefficient for channel %d not same "
                        "(%.02f != %.02f).",
                    ii - 1,
                    pfCoef[ii - 1],
                    pfCoefGet[ii - 1]);
            
                fSame = FALSE;        
            }
        }
        
        if (fSame)
        {
            SLOG(eInfo1, "Queried coefficient values are the same as set.");
        }
    }
    
    return (FNS_PASS);
}   //   TC_IAudioVolume_SetVolumeCoefficients()

//--------------------------------------------------------------------------;
//
//  DWORD TC_IAudioVolume_GetVolumeCoefficients
//
//  Description:
//      Tests conditions surrounding GetVolumeCoefficients().
//
//  Arguments:
//      None.
//
//  Return (DWORD):
//      Optional return code for test case.
//
//  History:
//      05/18/07    Fwong       Moving over tests from UMACore.
//
//--------------------------------------------------------------------------;
DWORD WINAPI TC_IAudioVolume_GetVolumeCoefficients()
{
    CAPODevice                         *pCurrentDevice = NULL;
    CComPtr<IAudioProcessingObject>     pIAPO = NULL;
    CComPtr<IAudioVolume>               pIVol = NULL;
    UINT                                cChannels = gAPOLimits.Volume_u32SamplesPerFrame;
    CAutoBuffer                         Coefficients((cChannels + 1) * sizeof(FLOAT32));
    FLOAT32                            *pfCoefGet;
    HRESULT                             hr;
    
    pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    
    if (NULL == pCurrentDevice)
    {
        XLOG(XFAIL, eError, "FAIL: Could not instantiate APO object.");
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

    hr = pIAPO->QueryInterface(__uuidof(IAudioVolume), (void**)&pIVol);

    if (E_NOINTERFACE == hr)
    {
        SLOG(eInfo1, "This APO does not implement IAudioVolume.");
        return (FNS_PASS);
    }
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: QueryInterface for IAudioVolume returns 0x%08lx.",
            hr);
        return (FNS_PASS);
    }
    
    SLOG(eInfo1, "QueryInterface for IAudioVolume succeeds.");

    if (NULL == (pfCoefGet = (FLOAT32*)Coefficients.GetBuffer()))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Insufficient memory.");
        return (FNS_PASS);
    }
    
    //
    //  Listing failure cases first!!
    //

    //  Trying Set Vol Coeffns w/o initializing samples per frame...
    {
        CComPtr<IAudioProcessingObject>     pIAPOTest = NULL;
        GUID                                guidAPO;

        //  Note: Using new IAudioProcessingObject since 'current' one is
        //        Initialize'ed.
        
        pCurrentDevice->GetClsID(&guidAPO);
        
        if (S_OK != (hr = pIAPOTest.CoCreateInstance(guidAPO)))
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: CoCreateInstance returned error (0x%08lx).",
                hr);
        }
        else
        {
            CComPtr<IAudioVolume>               pIVolTest = NULL;
            
            hr = pIAPOTest->QueryInterface(
                __uuidof(IAudioVolume),
                (void**)&pIVolTest);
        
            if (S_OK != hr)
            {
                XLOG(
                    XFAIL,
                    eError,
                    "FAIL: QueryInterface for IAudioVolume returned "
                        "error (0x%08lx).",
                    hr);
            }
            else
            {
                hr = pIVolTest->GetVolumeCoefficients(cChannels, pfCoefGet);
            
                if (APOERR_NOT_INITIALIZED != hr)
                {
                    XLOG(
                        XFAIL,
                        eError,
                        "FAIL: GetVolumeCoefficients before Initialize "
                            "returned 0x%08lx.",
                        hr);
                }
                else
                {
                    SLOG(
                        eInfo1,
                        "GetVolumeCoefficients before Initialize returned "
                            "APOERR_NOT_INITIALIZED.");
                }
            }
        }
    }
    
    //  Invalid u32CountOfCoefficients
    hr = pIVol->GetVolumeCoefficients(cChannels - 1, pfCoefGet);
    
    if (APOERR_INVALID_COEFFCOUNT != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: GetVolumeCoefficients with invalid u32CountOfCoefficients"
                " (channels-1) returned 0x%08lx.",
            hr);
    }
    else
    {
        SLOG(
            eInfo1,
            "SetVolumeCoefficients with invalid u32CountOfCoefficients "
                "(channels-1) returned APOERR_INVALID_COEFFCOUNT.");
    }
    
    hr = pIVol->GetVolumeCoefficients(cChannels + 1, pfCoefGet);
    
    if (APOERR_INVALID_COEFFCOUNT != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: GetVolumeCoefficients with invalid u32CountOfCoefficients"
                " (channels+1) returned 0x%08lx.",
            hr);
    }
    else
    {
        SLOG(
            eInfo1,
            "SetVolumeCoefficients with invalid u32CountOfCoefficients "
                "(channels+1) returned APOERR_INVALID_COEFFCOUNT.");
    }
    
    //  NULL pf32Coefficients
#pragma prefast(suppress:6309, "Doing parameter validation.") 
    hr = pIVol->GetVolumeCoefficients(cChannels, NULL);

    if (E_POINTER != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: GetVolumeCoefficients with NULL pf32Coefficients "
                "returned 0x%08lx.",
            hr);
    }
    else
    {
        SLOG(
            eInfo1,
            "GetVolumeCoefficients with NULL pf32Coefficients "
                "returned E_POINTER.");
    }
    
    return (FNS_PASS);
}   //   TC_IAudioVolume_GetVolumeCoefficients()

//--------------------------------------------------------------------------;
//
//  DWORD TC_IAudioVolume_CURVE_NONE
//
//  Description:
//      Verifies SetVolumeCoefficients() works with AUDIO_CURVE_TYPE_NONE.
//
//  Arguments:
//      None.
//
//  Return (DWORD):
//      Optional return code for test case.
//
//  History:
//      05/18/07    Fwong       Moving over tests from UMACore.
//
//--------------------------------------------------------------------------;
DWORD WINAPI TC_IAudioVolume_CURVE_NONE()
{
    Volume_TestCurve(AUDIO_CURVE_TYPE_NONE);
    
    //  Note:  All failures reported through XLOG...
    return (FNS_PASS);
}   //   TC_IAudioVolume_CURVE_NONE()

//--------------------------------------------------------------------------;
//
//  DWORD TC_IAudioVolume_CURVE_WINDOWS_FADE
//
//  Description:
//      Verifies SetVolumeCoefficients() works with AUDIO_CURVE_TYPE_WINDOWS_FADE.
//
//  Arguments:
//      None.
//
//  Return (DWORD):
//      Optional return code for test case.
//
//  History:
//      05/18/07    Fwong       Moving over tests from UMACore.
//
//--------------------------------------------------------------------------;
DWORD WINAPI TC_IAudioVolume_CURVE_WINDOWS_FADE()
{
    Volume_TestCurve(AUDIO_CURVE_TYPE_WINDOWS_FADE);
    
    //  Note:  All failures reported through XLOG...
    return (FNS_PASS);
}   //   TC_IAudioVolume_CURVE_WINDOWS_FADE()

//--------------------------------------------------------------------------;
//
//  DWORD TC_IAudioVolume_NoInitialRamp
//
//  Description:
//      Verifies 'No initial ramp' condition.
//
//      Note:  This APO was receiving many calls to SetVolumeCoefficents()
//             and initially it would simply cue them all up; this meant that
//             the start of each stream may be littered with many such ramps.
//             We changed the behavior such that any SetVolumeCoefficients()
//             calls are immediately set if they occur before the
//             LockForProcess().
//
//  Arguments:
//      None.
//
//  Return (DWORD):
//      Optional return code for test case.
//
//  History:
//      05/18/07    Fwong       Moving over tests from UMACore.
//
//--------------------------------------------------------------------------;
DWORD WINAPI TC_IAudioVolume_NoInitialRamp()
{
    //  Constants for this test case...
    HNSTIME hnstDelay          = hnst_Volume_DefaultDelay;
    HNSTIME hnstDuration       = hnst_Volume_DefaultDuration;
    const float fTargetVolume  = f_Volume_TargetVolume;

    //  Variables...
    CAPODevice                                     *pCurrentDevice = NULL;
    CComPtr<IAudioProcessingObject>                 pIAPO = NULL;
    CComPtr<IAudioProcessingObjectRT>               pIAPORT = NULL;
    CComPtr<IAudioProcessingObjectConfiguration>    pIAPOConfig = NULL;
    CComPtr<IAudioVolume>                           pIVol = NULL;
    CComPtr<IAudioMuteAPO>                          pIMute = NULL;
    UINT                                            cChannels = gAPOLimits.Volume_u32SamplesPerFrame;
    UINT                                            ii;
    CAutoBuffer                                     Coefficients(cChannels * sizeof(FLOAT32));
    FLOAT32                                        *pfCoef;
    UNCOMPRESSEDAUDIOFORMAT                         uaf;
    APO_CONNECTION_DESCRIPTOR                       InputConnection;
    APO_CONNECTION_DESCRIPTOR                       OutputConnection;
    APO_CONNECTION_PROPERTY                         InputProperty = {0};
    APO_CONNECTION_PROPERTY                         OutputProperty = {0};
    APO_CONNECTION_DESCRIPTOR                      *pInputConnection;
    APO_CONNECTION_DESCRIPTOR                      *pOutputConnection;
    APO_CONNECTION_PROPERTY                        *pInputProperty;
    APO_CONNECTION_PROPERTY                        *pOutputProperty;
    CComPtr<IAudioMediaType>                        pIInFormat = NULL;
    CComPtr<IAudioMediaType>                        pIOutFormat = NULL;
    CBufferCollection                               Buffers(2);     //  For input and output...
    CAudioBuffer                                   *pInBuffer = NULL;
    CAudioBuffer                                   *pOutBuffer = NULL;
    float                                           fBufferLength = (float)0.0;
    HRESULT                                         hr;
    
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

    hr = pIAPO->QueryInterface(__uuidof(IAudioVolume), (void**)&pIVol);

    if (E_NOINTERFACE == hr)
    {
        SLOG(eInfo1, "This APO does not implement IAudioVolume.");
        return (FNS_PASS);
    }
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: QueryInterface for IAudioVolume returns 0x%08lx.",
            hr);
        return (FNS_PASS);
    }
    
    hr = pIAPO->QueryInterface(__uuidof(IAudioMuteAPO), (void**)&pIMute);

    if (E_NOINTERFACE == hr)
    {
        SLOG(eInfo1, "This APO does not implement IAudioMuteAPO.");
        return (FNS_PASS);
    }

    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: QueryInterface for IAudioMuteAPO returns 0x%08lx.",
            hr);
        return (FNS_PASS);
    }

    if (NULL == (pfCoef = (FLOAT32*)Coefficients.GetBuffer()))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Out of memory.");
        return (FNS_PASS);
    }

    //  This is for both input and output formats...
    uaf.guidFormatType            = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
    uaf.dwSamplesPerFrame         = gAPOLimits.Volume_u32SamplesPerFrame;
    uaf.dwBytesPerSampleContainer = 4;
    uaf.dwValidBitsPerSample      = 32;
    uaf.fFramesPerSecond          = gAPOLimits.Volume_f32FrameRate;
    uaf.dwChannelMask             = 0;

    //  Getting actual format...
    hr = CreateAudioMediaTypeFromUncompressedAudioFormat(&uaf, &pIInFormat);
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioMediaType for format (0x%08lx).",
            hr);
    
        return (FNS_PASS);
    }
    
    //  Getting actual format...
    hr = CreateAudioMediaTypeFromUncompressedAudioFormat(&uaf, &pIOutFormat);
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioMediaType for format (0x%08lx).",
            hr);
    
        return (FNS_PASS);
    }
    
    pInBuffer = new CAudioBuffer(pIInFormat);
    Buffers.SetBuffer(0, pInBuffer);
    
    if (NULL == pInBuffer)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Insufficient memory allocating input buffer.");
        return (FNS_PASS);
    }
    
    //  Calculating buffer duration...
    fBufferLength  = (float)0.5;     //  Eventual duration is 1/2 second...
    fBufferLength += (float)(((double)hnstDelay) / (double)(HNSTIME1MSEC * 1000));
    fBufferLength += (float)(((double)hnstDuration) / (double)(HNSTIME1MSEC * 1000));
    
    SLOG(eInfo1, "Test Buffer duration: %.03f seconds.", fBufferLength);
    
    if (!pInBuffer->GenerateBuffer(fBufferLength))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to generate input buffer.");
        return (FNS_PASS);
    }
    
    pOutBuffer = new CAudioBuffer(pIOutFormat);
    Buffers.SetBuffer(1, pOutBuffer);
    
    if (NULL == pOutBuffer)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Insufficient memory allocating output buffer.");
        return (FNS_PASS);
    }
    
    if (!pOutBuffer->AllocateBuffer(fBufferLength))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to allocate output buffer.");
        return (FNS_PASS);
    }
   
    //  Setting up input connection...
    pInputConnection                   = &InputConnection;
    InputConnection.Type               = APO_CONNECTION_BUFFER_TYPE_EXTERNAL;
    InputConnection.pBuffer            = (UINT_PTR)pInBuffer->GetAudioBuffer();
    InputConnection.pFormat            = pIInFormat;
    InputConnection.u32MaxFrameCount   = pInBuffer->GetBufferSize() / 
                                           (uaf.dwSamplesPerFrame * 
                                                uaf.dwBytesPerSampleContainer);
    InputConnection.u32Signature       = APO_CONNECTION_DESCRIPTOR_SIGNATURE;
    
    //  Setting up output connection...
    pOutputConnection                  = &OutputConnection;
    OutputConnection.Type              = APO_CONNECTION_BUFFER_TYPE_EXTERNAL;
    OutputConnection.pBuffer           = (UINT_PTR)pOutBuffer->GetAudioBuffer();
    OutputConnection.pFormat           = pIOutFormat;
    OutputConnection.u32MaxFrameCount  = pOutBuffer->GetBufferSize() / 
                                         (uaf.dwSamplesPerFrame * 
                                             uaf.dwBytesPerSampleContainer);
    OutputConnection.u32Signature      = APO_CONNECTION_DESCRIPTOR_SIGNATURE;
    
    //  Setting up input properties...
    pInputProperty                     = &InputProperty;
    InputProperty.pBuffer              = InputConnection.pBuffer;
    InputProperty.u32ValidFrameCount   = InputConnection.u32MaxFrameCount;
    InputProperty.u32BufferFlags       = BUFFER_VALID;
    InputProperty.u32Signature         = APO_CONNECTION_PROPERTY_SIGNATURE;
    
    //  Setting up output properties...
    pOutputProperty                    = &OutputProperty;
    OutputProperty.pBuffer             = OutputConnection.pBuffer;
    OutputProperty.u32ValidFrameCount  = OutputConnection.u32MaxFrameCount;
    OutputProperty.u32BufferFlags      = BUFFER_INVALID;
    OutputProperty.u32Signature        = APO_CONNECTION_PROPERTY_SIGNATURE;

    //  Make sure we're not muted...  Also setting mute to take immediately.
    if (S_OK != (hr = Log_SetMute(pIMute, false, NULL, AUDIO_CURVE_TYPE_NONE, NULL)))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetMute to unmute returned error (0x%08lx).",
            hr);

        return (FNS_PASS);
    }
    
    //  Calling the setVolume BEFORE the LockForProcess...
    for (ii = cChannels; ii; --ii)
    {
        pfCoef[ii - 1] = fTargetVolume;
    }
    
    hr = Log_SetVolumeCoefficients(
        pIVol,
        cChannels,
        pfCoef,
        &hnstDelay,
        AUDIO_CURVE_TYPE_WINDOWS_FADE,
        &hnstDuration);

    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetVolumeCoefficients returned error (0x%08lx).",
            hr);
    
        return (FNS_PASS);
    }
    
    SLOG(eInfo1, "SetVolumeCoefficients set to %.02f.", fTargetVolume);
    
    hr = pIAPOConfig->LockForProcess(
        1,
        &pInputConnection,
        1,
        &pOutputConnection);

    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: LockForProcess returned error (0x%08lx).",
            hr);
        return (FNS_PASS);
    }
    
    pIAPORT->APOProcess(1, &pInputProperty, 1, &pOutputProperty);
    
    if (S_OK != (hr = pIAPOConfig->UnlockForProcess()))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: LockForProcess returned error (0x%08lx).",
            hr);
        return (FNS_PASS);
    }

    //  In case buffer is marked as AUDCLNT_BUFFERFLAGS_SILENT
    if (S_OK != (hr = ApplySilenceFlag(pOutputConnection, pOutputProperty)))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: ApplySilenceFlag returned error (0x%08lx).",
            hr);
        return (FNS_PASS);
    }

    //  Verify calling SetVolumeCoefficients before LockForProcess sets
    //      immediately...
    for (ii = cChannels; ii; --ii)
    {
        Verify_NoRamp(
            uaf,
            ii - 1,
            pfCoef[ii - 1],
            (float*)pInBuffer->GetAudioBuffer(),
            pInBuffer->GetBufferSize(),
            (float*)pOutBuffer->GetAudioBuffer(),
            pOutBuffer->GetBufferSize());
    }

    return (FNS_PASS);
}   //   TC_IAudioVolume_NoInitialRamp()

//--------------------------------------------------------------------------;
//
//  DWORD TC_IAudioMuteAPO_SetMute
//
//  Description:
//      Tests conditions surrounding SetMute().
//
//  Arguments:
//      None.
//
//  Return (DWORD):
//      Optional return code for test case.
//
//  History:
//      05/18/07    Fwong       Moving over tests from UMACore.
//
//--------------------------------------------------------------------------;
DWORD WINAPI TC_IAudioMuteAPO_SetMute()
{
    //  Constants for this test case...
    HNSTIME hnstDelay          = hnst_Volume_DefaultDelay;
    HNSTIME hnstDuration       = hnst_Volume_DefaultDuration;
    
    //  Variables...
    CAPODevice                         *pCurrentDevice = NULL;
    CComPtr<IAudioProcessingObject>     pIAPO = NULL;
    CComPtr<IAudioMuteAPO>              pIMute = NULL;
    boolean                             fMute;
    HRESULT                             hr;
    
    pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    
    if (NULL == pCurrentDevice)
    {
        XLOG(XFAIL, eError, "FAIL: Could not instantiate APO object.");
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

    hr = pIAPO->QueryInterface(__uuidof(IAudioMuteAPO), (void**)&pIMute);

    if (E_NOINTERFACE == hr)
    {
        SLOG(eInfo1, "This APO does not implement IAudioMuteAPO.");
        return (FNS_PASS);
    }
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: QueryInterface for IAudioMuteAPO returns 0x%08lx.",
            hr);
        return (FNS_PASS);
    }
    
    SLOG(eInfo1, "QueryInterface for IAudioMuteAPO succeeds.");

    //
    //  Listing failure cases first!!
    //

    //  Trying SetMute before Initialize...
    {
        CComPtr<IAudioProcessingObject>     pIAPOTest = NULL;
        GUID                                guidAPO;

        //  Note: Using new IAudioProcessingObject since 'current' one is
        //        Initialize'ed.
        
        pCurrentDevice->GetClsID(&guidAPO);
        
        if (S_OK != (hr = pIAPOTest.CoCreateInstance(guidAPO)))
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: CoCreateInstance returned error (0x%08lx).",
                hr);
        }
        else
        {
            CComPtr<IAudioMuteAPO>      pIMuteTest = NULL;
            
            hr = pIAPOTest->QueryInterface(
                __uuidof(IAudioMuteAPO),
                (void**)&pIMuteTest);
        
            if (S_OK != hr)
            {
                XLOG(
                    XFAIL,
                    eError,
                    "FAIL: QueryInterface for IAudioMuteAPO returned "
                        "error (0x%08lx).",
                    hr);
            }
            else
            {
                hr = pIMuteTest->SetMute(TRUE, NULL, AUDIO_CURVE_TYPE_NONE, NULL);
            
                if (APOERR_NOT_INITIALIZED != hr)
                {
                    XLOG(
                        XFAIL,
                        eError,
                        "FAIL: SetMute before Initialize returned 0x%08lx.",
                        hr);
                }
                else
                {
                    SLOG(
                        eInfo1,
                        "SetMute before Initialize returned "
                            "APOERR_NOT_INITIALIZED.");
                }
            }
        }
    }
    
    //
    //  Listing failure cases first!!
    //

    //  Invalid Delay time
    {
        HNSTIME     hnstBadDelay = -1;
        
        hr = pIMute->SetMute(TRUE, &hnstBadDelay, AUDIO_CURVE_TYPE_NONE, NULL);
    }
    
    if (APOERR_INVALID_CURVE_PARAM != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetMute with negative delay (pTime) returned 0x%08lx.",
            hr);
    }
    else
    {
        SLOG(
            eInfo1,
            "SetMute with negative delay (pTime) returned "
                "APOERR_INVALID_CURVE_PARAM.");
    }
    
    //  Invalid Curve Type
    {
        AUDIO_CURVE_TYPE   u32Curve;
        
        *((UINT32*)&u32Curve) = (UINT32)(-1);
        
        hr = pIMute->SetMute(TRUE, NULL, u32Curve, &hnstDuration);
    }
    
    if (APOERR_INVALID_CURVE_PARAM != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetMute with invalid curve type returned 0x%08lx.",
            hr);
    }
    else
    {
        SLOG(
            eInfo1,
            "SetMute with invalid curve type returned "
                "APOERR_INVALID_CURVE_PARAM.");
    }
    
    //  Negative Curve Duration
    {
        HNSTIME     hnstBadDuration = -1 * hnstDuration;
        
        hr = pIMute->SetMute(TRUE, NULL, AUDIO_CURVE_TYPE_WINDOWS_FADE, &hnstBadDuration);
    }
    
    if (APOERR_INVALID_CURVE_PARAM != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetMute with negative curve duration returned 0x%08lx.",
            hr);
    }
    else
    {
        SLOG(
            eInfo1,
            "SetMute with negative curve duration returned "
                "APOERR_INVALID_CURVE_PARAM.");
    }
    
    //  NULL Curve Duration and PARAM_CURVE != NONE.
    hr = pIMute->SetMute(TRUE, NULL, AUDIO_CURVE_TYPE_WINDOWS_FADE, NULL);

    if (APOERR_INVALID_CURVE_PARAM != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetMute with NULL duration and AUDIO_CURVE_TYPE_WINDOWS_FADE "
                "returned 0x%08lx.",
            hr);
    }
    else
    {
        SLOG(
            eInfo1,
            "SetMute with NULL duration and AUDIO_CURVE_TYPE_WINDOWS_FADE returned "
                "APOERR_INVALID_CURVE_PARAM.");
    }
    
    //
    //  Listing success cases next!!
    //

    //  NULL Delay
    hr = pIMute->SetMute(TRUE, NULL, AUDIO_CURVE_TYPE_WINDOWS_FADE, &hnstDuration);
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetMute with NULL Delay returned error (0x%08lx).",
            hr);
    }
    else
    {
        SLOG(eInfo1, "SetMute with NULL Delay succeeds.");
    }
    
    //  10ms Delay
    hr = pIMute->SetMute(TRUE, &hnstDelay, AUDIO_CURVE_TYPE_WINDOWS_FADE, &hnstDuration);
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetMute with 10ms delay returned error (0x%08lx).",
            hr);
    }
    else
    {
        SLOG(eInfo1, "SetMute with 10ms delay succeeds.");
    }

    //  Muting
    hr = pIMute->SetMute(TRUE, NULL, AUDIO_CURVE_TYPE_NONE, NULL);
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetMute returned error (0x%08lx).",
            hr);
    }
    else
    {
        //  Verifying...
        hr = pIMute->GetMute(&fMute);
        
        if (S_OK != hr)
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: GetMute returned error (0x%08lx).",
                hr);
        }
        else
        {
            if (!fMute)
            {
                XLOG(
                    XFAIL,
                    eError,
                    "FAIL: GetMute returned unmuted after muting.");
            }
            else
            {
                SLOG(eInfo1, "SetMute returned muted after muting.");
            }
        }
    }
    
    //  Unmuting
    hr = pIMute->SetMute(FALSE, NULL, AUDIO_CURVE_TYPE_NONE, NULL);
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetMute returned error (0x%08lx).",
            hr);
    }
    else
    {
        //  Verifying...
        hr = pIMute->GetMute(&fMute);
        
        if (S_OK != hr)
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: GetMute returned error (0x%08lx).",
                hr);
        }
        else
        {
            if (fMute)
            {
                XLOG(
                    XFAIL,
                    eError,
                    "FAIL: GetMute returned muted after unmuting.");
            }
            else
            {
                SLOG(eInfo1, "SetMute returned unmuted after unmuting.");
            }
        }
    }
    
    //  Using AUDIO_CURVE_TYPE_NONE
    hr = pIMute->SetMute(TRUE, NULL, AUDIO_CURVE_TYPE_NONE, &hnstDuration);
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetMute with AUDIO_CURVE_TYPE_NONE returned error (0x%08lx).",
            hr);
    }
    else
    {
        SLOG(eInfo1, "SetMute with AUDIO_CURVE_TYPE_NONE succeeds.");
    }

    //  Using AUDIO_CURVE_TYPE_WINDOWS_FADE
    hr = pIMute->SetMute(TRUE, NULL, AUDIO_CURVE_TYPE_WINDOWS_FADE, &hnstDuration);
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetMute with AUDIO_CURVE_TYPE_WINDOWS_FADE returned error (0x%08lx).",
            hr);
    }
    else
    {
        SLOG(eInfo1, "SetMute with AUDIO_CURVE_TYPE_WINDOWS_FADE succeeds.");
    }

    return (FNS_PASS);
}   //   TC_IAudioMuteAPO_SetMute()

//--------------------------------------------------------------------------;
//
//  DWORD TC_IAudioMuteAPO_GetMute
//
//  Description:
//      Tests conditions surrounding GetMute().
//
//  Arguments:
//      None.
//
//  Return (DWORD):
//      Optional return code for test case.
//
//  History:
//      05/18/07    Fwong       Moving over tests from UMACore.
//
//--------------------------------------------------------------------------;
DWORD WINAPI TC_IAudioMuteAPO_GetMute()
{
    CAPODevice                         *pCurrentDevice = NULL;
    CComPtr<IAudioProcessingObject>     pIAPO = NULL;
    CComPtr<IAudioMuteAPO>              pIMute = NULL;
    boolean                             fMute;
    HRESULT                             hr;
    
    pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    
    if (NULL == pCurrentDevice)
    {
        XLOG(XFAIL, eError, "FAIL: Could not instantiate APO object.");
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

    hr = pIAPO->QueryInterface(__uuidof(IAudioMuteAPO), (void**)&pIMute);

    if (E_NOINTERFACE == hr)
    {
        SLOG(eInfo1, "This APO does not implement IAudioMuteAPO.");
        return (FNS_PASS);
    }
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: QueryInterface for IAudioMuteAPO returns 0x%08lx.",
            hr);
        return (FNS_PASS);
    }
    
    SLOG(eInfo1, "QueryInterface for IAudioMuteAPO succeeds.");

    //
    //  Listing failure cases first!!
    //

    //  Trying GetMute before Initialize...
    {
        CComPtr<IAudioProcessingObject>     pIAPOTest = NULL;
        GUID                                guidAPO;

        //  Note: Using new IAudioProcessingObject since 'current' one is
        //        Initialize'ed.
        
        pCurrentDevice->GetClsID(&guidAPO);
        
        if (S_OK != (hr = pIAPOTest.CoCreateInstance(guidAPO)))
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: CoCreateInstance returned error (0x%08lx).",
                hr);
        }
        else
        {
            CComPtr<IAudioMuteAPO>      pIMuteTest = NULL;
            
            hr = pIAPOTest->QueryInterface(
                __uuidof(IAudioMuteAPO),
                (void**)&pIMuteTest);
        
            if (S_OK != hr)
            {
                XLOG(
                    XFAIL,
                    eError,
                    "FAIL: QueryInterface for IAudioMuteAPO returned "
                        "error (0x%08lx).",
                    hr);
            }
            else
            {
                hr = pIMuteTest->GetMute(&fMute);
            
                if (APOERR_NOT_INITIALIZED != hr)
                {
                    XLOG(
                        XFAIL,
                        eError,
                        "FAIL: GetMute before Initialize returned 0x%08lx.",
                        hr);
                }
                else
                {
                    SLOG(
                        eInfo1,
                        "GetMute before Initialize returned "
                            "APOERR_NOT_INITIALIZED.");
                }
            }
        }
    }

    //  NULL pbMuted
#pragma prefast(suppress:6309, "Doing parameter validation.") 
    if (E_POINTER != (hr = pIMute->GetMute(NULL)))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: GetMute with NULL pbMuted returned 0x%08lx.",
            hr);
    }
    else
    {
        SLOG(eInfo1, "GetMute with NULL pbMuted returned E_POINTER.");
    }
    
    return (FNS_PASS);
}   //   TC_IAudioMuteAPO_GetMute()

//--------------------------------------------------------------------------;
//
//  DWORD TC_IAudioMuteAPO_CURVE_NONE
//
//  Description:
//      Verifies SetMute() works with AUDIO_CURVE_TYPE_NONE.
//
//  Arguments:
//      None.
//
//  Return (DWORD):
//      Optional return code for test case.
//
//  History:
//      05/18/07    Fwong       Moving over tests from UMACore.
//
//--------------------------------------------------------------------------;
DWORD WINAPI TC_IAudioMuteAPO_CURVE_NONE()
{
    Mute_TestCurve(AUDIO_CURVE_TYPE_NONE);
    
    //  Note:  All failures reported through XLOG...
    return (FNS_PASS);
}   //   TC_IAudioMuteAPO_CURVE_NONE()

//--------------------------------------------------------------------------;
//
//  DWORD TC_IAudioMuteAPO_CURVE_WINDOWS_FADE
//
//  Description:
//      Verifies SetMute() works with AUDIO_CURVE_TYPE_WINDOWS_FADE.
//
//  Arguments:
//      None.
//
//  Return (DWORD):
//      Optional return code for test case.
//
//  History:
//      05/18/07    Fwong       Moving over tests from UMACore.
//
//--------------------------------------------------------------------------;
DWORD WINAPI TC_IAudioMuteAPO_CURVE_WINDOWS_FADE()
{
    Mute_TestCurve(AUDIO_CURVE_TYPE_WINDOWS_FADE);
    
    //  Note:  All failures reported through XLOG...
    return (FNS_PASS);
}   //   TC_IAudioMuteAPO_CURVE_WINDOWS_FADE()

//--------------------------------------------------------------------------;
//
//  DWORD TC_IAudioMuteAPO_NoInitialRamp
//
//  Description:
//      Verifies 'No initial ramp' condition occurs with mute as well.
//
//      Note:  This APO was receiving many calls to SetVolumeCoefficents()
//             and initially it would simply cue them all up; this meant that
//             the start of each stream may be littered with many such ramps.
//             We changed the behavior such that any SetVolumeCoefficients()
//             calls are immediately set if they occur before the
//             LockForProcess().
//
//  Arguments:
//      None.
//
//  Return (DWORD):
//      Optional return code for test case.
//
//  History:
//      05/18/07    Fwong       Moving over tests from UMACore.
//
//--------------------------------------------------------------------------;
DWORD WINAPI TC_IAudioMuteAPO_NoInitialRamp()
{
    //  Constants for this test case...
    HNSTIME hnstDelay          = hnst_Volume_DefaultDelay;
    HNSTIME hnstDuration       = hnst_Volume_DefaultDuration;
    const float fTargetVolume  = f_Volume_TargetVolume;

    //  Variables...
    CAPODevice                                     *pCurrentDevice = NULL;
    CComPtr<IAudioProcessingObject>                 pIAPO = NULL;
    CComPtr<IAudioProcessingObjectRT>               pIAPORT = NULL;
    CComPtr<IAudioProcessingObjectConfiguration>    pIAPOConfig = NULL;
    CComPtr<IAudioVolume>                           pIVol = NULL;
    CComPtr<IAudioMuteAPO>                          pIMute = NULL;
    UINT                                            cChannels = gAPOLimits.Volume_u32SamplesPerFrame;
    UINT                                            ii;
    CAutoBuffer                                     Coefficients(cChannels * sizeof(FLOAT32));
    FLOAT32                                        *pfCoef;
    UNCOMPRESSEDAUDIOFORMAT                         uaf;
    APO_CONNECTION_DESCRIPTOR                       InputConnection;
    APO_CONNECTION_DESCRIPTOR                       OutputConnection;
    APO_CONNECTION_PROPERTY                         InputProperty = {0};
    APO_CONNECTION_PROPERTY                         OutputProperty = {0};
    APO_CONNECTION_DESCRIPTOR                      *pInputConnection;
    APO_CONNECTION_DESCRIPTOR                      *pOutputConnection;
    APO_CONNECTION_PROPERTY                        *pInputProperty;
    APO_CONNECTION_PROPERTY                        *pOutputProperty;
    CComPtr<IAudioMediaType>                        pIInFormat = NULL;
    CComPtr<IAudioMediaType>                        pIOutFormat = NULL;
    CBufferCollection                               Buffers(2);     //  For input and output...
    CAudioBuffer                                   *pInBuffer = NULL;
    CAudioBuffer                                   *pOutBuffer = NULL;
    float                                           fBufferLength = (float)0.0;
    HRESULT                                         hr;
    
    pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    
    if (NULL == pCurrentDevice)
    {
        XLOG(XFAIL, eError, "FAIL: Could not instantiate APO object.");
        return (FNS_PASS);
    }
    
    if (!pCurrentDevice->GetAPOInterfaces(&pIAPO, &pIAPORT, &pIAPOConfig))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObject interface.");
        return (FNS_PASS);
    }

    hr = pIAPO->QueryInterface(__uuidof(IAudioVolume), (void**)&pIVol);

    if (E_NOINTERFACE == hr)
    {
        SLOG(eInfo1, "This APO does not implement IAudioVolume.");
        return (FNS_PASS);
    }
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: QueryInterface for IAudioVolume returns 0x%08lx.",
            hr);
        return (FNS_PASS);
    }
    
    hr = pIAPO->QueryInterface(__uuidof(IAudioMuteAPO), (void**)&pIMute);

    if (E_NOINTERFACE == hr)
    {
        SLOG(eInfo1, "This APO does not implement IAudioMuteAPO.");
        return (FNS_PASS);
    }
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: QueryInterface for IAudioMuteAPO returns 0x%08lx.",
            hr);
        return (FNS_PASS);
    }

    if (NULL == (pfCoef = (FLOAT32*)Coefficients.GetBuffer()))
    {
        XLOG(XFAIL, eError, "FAIL: Insufficient memory.");
        return (FNS_PASS);
    }

    //  This is for both input and output formats...
    uaf.guidFormatType            = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
    uaf.dwSamplesPerFrame         = gAPOLimits.Volume_u32SamplesPerFrame;
    uaf.dwBytesPerSampleContainer = 4;
    uaf.dwValidBitsPerSample      = 32;
    uaf.fFramesPerSecond          = gAPOLimits.Volume_f32FrameRate;
    uaf.dwChannelMask             = 0;

    //  Getting actual format...
    hr = CreateAudioMediaTypeFromUncompressedAudioFormat(&uaf, &pIInFormat);
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioMediaType for format (0x%08lx).",
            hr);
    
        return (FNS_PASS);
    }
    
    //  Getting actual format...
    hr = CreateAudioMediaTypeFromUncompressedAudioFormat(&uaf, &pIOutFormat);
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioMediaType for format (0x%08lx).",
            hr);
    
        return (FNS_PASS);
    }
    
    pInBuffer = new CAudioBuffer(pIInFormat);
    Buffers.SetBuffer(0, pInBuffer);
    
    if (NULL == pInBuffer)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Insufficient memory allocating input buffer.");
        return (FNS_PASS);
    }
    
    //  Calculating buffer duration...
    fBufferLength  = (float)0.5;     //  Eventual duration is 1/2 second...
    fBufferLength += (float)(((double)hnstDelay) / (double)(HNSTIME1MSEC * 1000));
    fBufferLength += (float)(((double)hnstDuration) / (double)(HNSTIME1MSEC * 1000));
    
    SLOG(eInfo1, "Test Buffer duration: %.03f seconds.", fBufferLength);
    
    if (!pInBuffer->GenerateBuffer(fBufferLength))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to generate input buffer.");
        return (FNS_PASS);
    }
    
    pOutBuffer = new CAudioBuffer(pIOutFormat);
    Buffers.SetBuffer(1, pOutBuffer);
    
    if (NULL == pOutBuffer)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Insufficient memory allocating output buffer.");
        return (FNS_PASS);
    }
    
    if (!pOutBuffer->AllocateBuffer(fBufferLength))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to allocate output buffer.");
        return (FNS_PASS);
    }
   
    //  Setting up input connection...
    pInputConnection                   = &InputConnection;
    InputConnection.Type               = APO_CONNECTION_BUFFER_TYPE_EXTERNAL;
    InputConnection.pBuffer            = (UINT_PTR)pInBuffer->GetAudioBuffer();
    InputConnection.pFormat            = pIInFormat;
    InputConnection.u32MaxFrameCount   = pInBuffer->GetBufferSize() / 
                                           (uaf.dwSamplesPerFrame * 
                                                uaf.dwBytesPerSampleContainer);
    InputConnection.u32Signature       = APO_CONNECTION_DESCRIPTOR_SIGNATURE;
    
    //  Setting up output connection...
    pOutputConnection                  = &OutputConnection;
    OutputConnection.Type              = APO_CONNECTION_BUFFER_TYPE_EXTERNAL;
    OutputConnection.pBuffer           = (UINT_PTR)pOutBuffer->GetAudioBuffer();
    OutputConnection.pFormat           = pIOutFormat;
    OutputConnection.u32MaxFrameCount  = pOutBuffer->GetBufferSize() / 
                                         (uaf.dwSamplesPerFrame * 
                                             uaf.dwBytesPerSampleContainer);
    OutputConnection.u32Signature      = APO_CONNECTION_DESCRIPTOR_SIGNATURE;
    
    //  Setting up input properties...
    pInputProperty                     = &InputProperty;
    InputProperty.pBuffer              = InputConnection.pBuffer;
    InputProperty.u32ValidFrameCount   = InputConnection.u32MaxFrameCount;
    InputProperty.u32BufferFlags       = BUFFER_VALID;
    InputProperty.u32Signature         = APO_CONNECTION_PROPERTY_SIGNATURE;
    
    //  Setting up output properties...
    pOutputProperty                    = &OutputProperty;
    OutputProperty.pBuffer             = OutputConnection.pBuffer;
    OutputProperty.u32ValidFrameCount  = OutputConnection.u32MaxFrameCount;
    OutputProperty.u32BufferFlags      = BUFFER_INVALID;
    OutputProperty.u32Signature        = APO_CONNECTION_PROPERTY_SIGNATURE;
    
    //  Make sure we're not muted...  Also setting mute to take immediately.
    if (S_OK != (hr = Log_SetMute(pIMute, false, NULL, AUDIO_CURVE_TYPE_NONE, NULL)))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetMute to unmute returned error (0x%08lx).",
            hr);

        return (FNS_PASS);
    }
    
    for (ii = cChannels; ii; --ii)
    {
        pfCoef[ii - 1] = fTargetVolume;
    }
    
    //  Setting volume so we know how samples should be scaled.
    hr = Log_SetVolumeCoefficients(
        pIVol,
        cChannels,
        pfCoef,
        &hnstDelay,
        AUDIO_CURVE_TYPE_WINDOWS_FADE,
        &hnstDuration);

    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetVolumeCoefficients returned error (0x%08lx).",
            hr);
    
        return (FNS_PASS);
    }
    
    //  Calling the SetMute BEFORE the LockForProcess...
    hr = Log_SetMute(
            pIMute,
            true,
            &hnstDelay,
            AUDIO_CURVE_TYPE_WINDOWS_FADE,
            &hnstDuration);

    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetMute returned error (0x%08lx).",
            hr);
    
        return (FNS_PASS);
    }
    
    SLOG(eInfo1, "SetMute set to muted.");
    
    hr = pIAPOConfig->LockForProcess(
        1,
        &pInputConnection,
        1,
        &pOutputConnection);

    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: LockForProcess returned error (0x%08lx).",
            hr);
        return (FNS_PASS);
    }
    
    pIAPORT->APOProcess(1, &pInputProperty, 1, &pOutputProperty);
    
    if (S_OK != (hr = pIAPOConfig->UnlockForProcess()))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: LockForProcess returned error (0x%08lx).",
            hr);
        return (FNS_PASS);
    }
    
    //  In case buffer is marked as AUDCLNT_BUFFERFLAGS_SILENT
    if (S_OK != (hr = ApplySilenceFlag(pOutputConnection, pOutputProperty)))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: ApplySilenceFlag returned error (0x%08lx).",
            hr);
        return (FNS_PASS);
    }

    //  Verify calling SetMute to muted before LockForProcess sets
    //      immediately...
    for (ii = cChannels; ii; --ii)
    {
        Verify_NoRamp(
            uaf,
            ii - 1,
            0.0,
            (float*)pInBuffer->GetAudioBuffer(),
            pInBuffer->GetBufferSize(),
            (float*)pOutBuffer->GetAudioBuffer(),
            pOutBuffer->GetBufferSize());
    }
    
    OutputProperty.u32BufferFlags = BUFFER_INVALID;
    
    //  Calling the SetMute BEFORE the LockForProcess...
    hr = Log_SetMute(
            pIMute,
            false,
            &hnstDelay,
            AUDIO_CURVE_TYPE_WINDOWS_FADE,
            &hnstDuration);

    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SetMute returned error (0x%08lx).",
            hr);
    
        return (FNS_PASS);
    }
    
    SLOG(eInfo1, "SetMute set to unmuted.");
    
    hr = pIAPOConfig->LockForProcess(
        1,
        &pInputConnection,
        1,
        &pOutputConnection);

    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: LockForProcess returned error (0x%08lx).",
            hr);
        return (FNS_PASS);
    }
    
    pIAPORT->APOProcess(1, &pInputProperty, 1, &pOutputProperty);
    
    if (S_OK != (hr = pIAPOConfig->UnlockForProcess()))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: LockForProcess returned error (0x%08lx).",
            hr);
        return (FNS_PASS);
    }
    
    //  In case buffer is marked as AUDCLNT_BUFFERFLAGS_SILENT
    if (S_OK != (hr = ApplySilenceFlag(pOutputConnection, pOutputProperty)))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: ApplySilenceFlag returned error (0x%08lx).",
            hr);
        return (FNS_PASS);
    }
    
    //  Verify calling SetMute to unmuted before LockForProcess sets
    //      immediately...
    for (ii = cChannels; ii; --ii)
    {
        Verify_NoRamp(
            uaf,
            ii - 1,
            pfCoef[ii - 1],
            (float*)pInBuffer->GetAudioBuffer(),
            pInBuffer->GetBufferSize(),
            (float*)pOutBuffer->GetAudioBuffer(),
            pOutBuffer->GetBufferSize());
    }

    return (FNS_PASS);
}   //   TC_IAudioMuteAPO_NoInitialRamp()

