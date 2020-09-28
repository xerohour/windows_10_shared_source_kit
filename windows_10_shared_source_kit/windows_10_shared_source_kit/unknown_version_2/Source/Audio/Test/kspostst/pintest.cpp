// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
// Module Name:
//
//  tests.cpp
//
// Abstract:
//
//  Implementation file for test cases
//




// -------------------------------------------------------------------------------



#include "PreComp.h"
#include <devioctl.h>
#include <intsafe.h>
#include <bestfit.h>
#include <math.h>
#include <debug.h>
#include "locallimits.h"
#include "usbstream.h"
#include <cptr.h>
#include <log_wfx.h>
#include <AudioOffload.h>
#include <avrt.h>
#include <keyworddetectoroemadapter.h>
#include <waveio.h>

#define xGetTime    g_pKsPosTst->m_pTimer->Proc

// 
// QpcToRelativeMs
//
//  A simple utility function to convert from a performance counter value to
//  milliseconds relative to some baseline performance counter reference value.
//
// Returns
//  LONGLONG - Number of milliseconds from the qpc reference value to the qpc
//      value.
//
__inline LONGLONG QpcToRelativeMs
(
    LONGLONG qpc,                   // The qpc value
    LARGE_INTEGER qpcReference,     // The reference time, in qpc terms
    LARGE_INTEGER qpcFrequency      // The qpc frequency
)
{
    return (qpc - qpcReference.QuadPart) * 1000 / qpcFrequency.QuadPart;
}

// Checks if a pin is offload capable. Does not propagate back failure modes, rather fails the calling TC body
// When any of the required actions (acquiring a log and calling into offload library) fails we would log a TC failure
// because we can't establish if the pin is eligible to run the test
// TODO: Re-evaluate if we need to skip the offload pin forever

bool IsPinOffloadCapable(CPCMAudioPin* pPin)
{

    HRESULT hr = S_OK;

    CComPtr<IBasicLog> spBasicLog;
    bool bOffloadSupported = false;

    spBasicLog = g_pBasicLog;

    hr = AudioOffload::DoesPinSupportOffload( spBasicLog, pPin, AudioOffload::KS_API, bOffloadSupported );
    if( S_OK != hr )
    {
        XLOG( XFAIL, eError, "BLOCK: Unable to determine if pin supports offload.  (0x%08x)", hr );
    }

    return bOffloadSupported;
}

DWORD
Test_Latency
(
    void
)
{
    DWORD           dwRes = FNS_FAIL;
    BOOL            fRes = FALSE;
    CPCMAudioPin*   pPin = g_pKsPosTst->m_pHalf->m_pPin;
    WAVEFORMATEX*   pwfx = &g_pKsPosTst->m_pHalf->m_pFmtCurrent->wfxFormat.Format;
    BOOL            fTestingLoopedDevice = g_pKsPosTst->m_fLoop;
    CKsFilter*      pFilter = g_pKsPosTst->m_pHalf->m_pFilter;      // used to find out that Filter is corresponding to USB or not
    bool            fRTPin = g_pKsPosTst->m_pHalf->IsWaveRTPin();
    ULONG           ulRun = 0;   
    NTSTATUS        lTimerResStatus;

    if(!pFilter)
    {
        SLOG(eError, "Unable to find a CKsFilter for current device");
        return FNS_FAIL;
    }

    if(!g_pKsPosTst->m_pHalf->IsTestSupportedForDeviceType(fTestingLoopedDevice) || g_pKsPosTst->m_pHalf->IsBTHPin())
    {
        XLOG(XSKIP, eError, "Test is not supported for the current device type.");
        return FNS_SKIPPED;
    }

    if(IsPinOffloadCapable(pPin))
    {
        XLOG( XSKIP, eError, "SKIP: Test is not supported on the offload pin." );
        return FNS_SKIPPED;
    }

    lTimerResStatus = NtSetTimerResolution(timerResolution, TRUE, &actualTimerResolution);

    if (!NT_SUCCESS(lTimerResStatus))
    {
        XLOG(XSKIP, eInfo1, "Fail to set timer precision. Skip the test.");
        return FNS_SKIPPED;
    }

    try
    {
        double           tLatency;
        
        fRes = g_pKsPosTst->m_pHalf->PreparePinConsiderUSB(pwfx, fTestingLoopedDevice, FALSE);
        if (!fRes)
        {
            throw("Failed to prepare pin for testing");
        }


        if (g_pKsPosTst->m_pHalf->IsWaveRTPacketStreamingSupported())
        {
            XLOG(XSKIP, eError, "The test doesn't support packet based pin.");

            fRes = TRUE;
            dwRes = FNS_SKIPPED;
            goto Exit;
        }

        // Submit data to the sink pin 
        SLOG(eInfo2, "Submitting %d bytes to the pin", pPin->m_cbStreamData);
        if (fRTPin) 
        {   // in case of RT port, it does not support IOCTL_KS_WRITE_STREAM and IOCTL_KS_READ_STREAM
            fRes = pPin->SetState(KSSTATE_PAUSE);
        }
        else    
        {            
            if (g_pKsPosTst->m_pHalf->m_flow == render)
            {
                fRes = pPin->WriteData();
            }
            else
            {
                fRes = pPin->ReadData();
            }
        }
        if (!fRes)
        {
            throw("Failed to submit buffer to pin");
        }

        // mark the time
        double tRunPre = xGetTime();

        // run the adapter sink pin
        fRes = pPin->SetState(KSSTATE_RUN);
        if (!fRes)
        {
            throw("Failed to set state on pin to KSSTATE_RUN");
        }
        
        // mark the time
        double tRunPost = xGetTime();
        SLOG(eInfo2, "Setting pin to KSSTATE_RUN took %.03f ms", (tRunPost - tRunPre));

        KSAUDIO_POSITION    pos = {0,0};
        double              tPosPre = 0.0;      
        double              tPosPost = 0.0;
        
        // loop until we get a non-zero position
        while (pos.PlayOffset <= 0)
        {
            ulRun++;
            Sleep(1);
            
            // timer before position call
            tPosPre = xGetTime();

            // get position
            fRes = pPin->GetPosition(&pos);

            // timer after position call
            tPosPost = xGetTime();
            SLOG(eInfo2, "Testing  First non-zero position reported = %d bytes, (%.03f ms)", (ULONG)pos.PlayOffset, tPosPost);           
            
            if (pos.PlayOffset > 0)
            {
                if (g_pKsPosTst->m_fLogHistograms)
                {
                    SLOG(eInfo2, "Getting KSPROPERTY_AUDIO_POSITION took %.03f ms", (tPosPost - tPosPre));
                }

                KSAUDIO_POSITION posCheaters = {0,0};

                Sleep(1);
                pPin->GetPosition(&posCheaters);
                fRes = (posCheaters.PlayOffset != pos.PlayOffset);
                if (!fRes)
                {
                    throw ("Non-zero position is being reported, but the cursor is not moving!");
                }

                break;
            }

            fRes = ((tPosPost - tRunPost) < 500.);
            if (!fRes)
            {
                throw("Position didn't move within 500 ms");
            }
        }
        
        // the uncertainty in time is the max possible value minus the min possible value divided by 2 (for +/- purposes)
        double tMax = tPosPost - tRunPre;
        double tMin = tPosPre - tRunPost;
        double tAve = (tMax + tMin) / 2.;
        double tUncertainty = (tMax - tMin) / 2.;

        ULONG ulOffsetBytes = (ULONG)pos.PlayOffset;
        ULONG ulOffsetSamples = ulOffsetBytes / pwfx->nBlockAlign;
        double tOffset = 1000. * (double)ulOffsetSamples / (double)pwfx->nSamplesPerSec;

        SLOG(eInfo2, "First non-zero position reported = %d bytes, (%d samples), (%.03f ms)", ulOffsetBytes, ulOffsetSamples, tOffset);
        SLOG(eInfo2, "Time of first non-zero position  = %g ms (+/- %g ms)", tAve, tUncertainty);
                
        // if the reported position is outside the max time for this call (see tUncertainty calculation above), then fail
        // temp fix until we know how far can be the data in front of the 
        // real time because of buffering
        if ((ULONG)tOffset > (ULONG)tMax)
        {
            SLOG(eWarn1, "Time equivalent of first non-zero position (%.03f ms) is greater than the time the pin has been in the run state (%.03f ms)", tOffset, tMax);
            throw("First non-zero position is too high.");
        }

        tLatency = tAve - tOffset;
        SLOG(eInfo2, "Adjusted latency                 = %.03f ms (+/- %.3f ms)", tLatency, tUncertainty);
        
        if (g_pKsPosTst->m_pHalf->m_pFilter->m_UserDef == FILTERTYPE_SYSAUDIO)
        {
            if (tLatency <= LATENCY_THRESHOLD_SYSAUDIO) // ms
            {
                SLOG(eInfo1, "PASS: The (adjusted) latency for this format is no more than %.03f ms, and is therefore acceptable", LATENCY_THRESHOLD_SYSAUDIO);
                dwRes = FNS_PASS;
            }
            else
            {
                SLOG(eWarn1, "FAIL: The (adjusted) latency for this format exceeds the acceptable limit of %.03f ms", LATENCY_THRESHOLD_SYSAUDIO);
                SLOG(eWarn1, "!!!!  NOTE THAT THIS FAILURE IS THROUGH SYSAUDIO AND IS THEREFORE LIKELY DUE TO");
                SLOG(eWarn1, "      EXCESSIVE LATENCY INCURRED BY KMIXER");
            }
        }
        else
        {
            if (tLatency <= LATENCY_THRESHOLD)         // ms
            {
                SLOG(eInfo1, "PASS: The (adjusted) latency for this format is no more than %.03f ms, and is therefore acceptable", LATENCY_THRESHOLD);
                dwRes = FNS_PASS;
            }
            else
            {
                SLOG(eWarn1, "FAIL: The (adjusted) latency for this format exceeds the acceptable limit of %.03f ms", LATENCY_THRESHOLD);
            }
        }
    }
    // handle bad things
    catch (char* str)
    {
        SLOG(eError, "FAIL: %s", str);
        SLOG(eInfo1, "Number of runs %ld", ulRun);
        dwRes = FNS_FAIL;
    }

Exit:

    // Clean up
    if (pPin)
    {
        pPin->SetState(KSSTATE_STOP);
        pPin->ClosePin();
    }
    dwRes = (fRes ? dwRes : FNS_FAIL);

    // Turn off the higher resolution timer
    NtSetTimerResolution(0 /*unused*/, FALSE, &actualTimerResolution); 

    return dwRes;
}

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
DWORD
Test_DurationLength
(
    void
)
{
    DWORD           dwRes = FNS_PASS;
    BOOL            fRes = FALSE;
    CPCMAudioPin*   pPin = g_pKsPosTst->m_pHalf->m_pPin;
    CKsFilter*      pFilter = g_pKsPosTst->m_pHalf->m_pFilter;
    WAVEFORMATEX*   pwfx = &g_pKsPosTst->m_pHalf->m_pFmtCurrent->wfxFormat.Format;
    BOOL            fTestingLoopedDevice = g_pKsPosTst->m_fLoop;
    bool            fIsUSBPin = g_pKsPosTst->m_pHalf->IsUSBPin();
    bool            fRTPin = g_pKsPosTst->m_pHalf->IsWaveRTPin();
    double          DurationThreshold = DURATION_THRESHOLD;
    double          LengthThreshold = 0.5;
    const double    dWaitFactor = 10.0;

    if(!pFilter)
    {
        SLOG(eError, "Unable to find a CKsFilter for current device");
        return FNS_FAIL;
    }

    if(!g_pKsPosTst->m_pHalf->IsTestSupportedForDeviceType(fTestingLoopedDevice) || g_pKsPosTst->m_pHalf->IsBTHPin())
    {
        XLOG(XSKIP, eError, "Test is not supported for the current device type.");
        return FNS_SKIPPED;
    }

    HANDLE  hWaitForFinish = CreateEvent(NULL, TRUE, FALSE, NULL);
    if(NULL == hWaitForFinish)
    {
        SLOG(eError, "FAIL: Unable to create an event");
        return FNS_FAIL;
    }

    try
    {
        KSAUDIO_POSITION pos = {0};
        ULONG       ulLen=0, ulLenExpected=0;
        LONG        lLenError=0;
        double      tStart=0, tEnd=0, tLen=0, tExpected=0;
        double      tError=0, tErrorPerc=0;
        
        tExpected = BUFLEN_MS;
        ulLenExpected = ((ULONG)(tExpected) * pwfx->nSamplesPerSec / 1000) * pwfx->nBlockAlign ;

        if (fRTPin)
        {
            // Allocate a larger buffer to make sure the position would not reach the end and restart from 0
            // Test will get notification once the data in the larger buffer ((tExpected)* 2) got consumed.
            fRes = g_pKsPosTst->m_pHalf->PreparePin(pwfx, fTestingLoopedDevice, (ULONG)(tExpected)* 2);
        }
        else
        {
            // For non-RT pin, the render/capture time will be based on how much buffer we allocated (KsRend.Play() or KsCap.Capture).
            // So the test result will show us tExpected ms to comsume the buffer. If we allocate a larger buffer (tExpected)* 2, 
            // it will make render/capture time double even we only fill out tExpected data
            fRes = g_pKsPosTst->m_pHalf->PreparePin(pwfx, fTestingLoopedDevice, (ULONG)(tExpected));
        }

        if (!fRes)
        {
            throw("PreparePin failed.");
        }

        if (g_pKsPosTst->m_pHalf->IsWaveRTPacketStreamingSupported())
        {

            XLOG(XSKIP, eError, "The test doesn't support packet based pin.");
            fRes = TRUE;
            dwRes = FNS_SKIPPED;
            goto Exit;
        }

        fRes = pPin->SetState(KSSTATE_PAUSE);
        if(!fRes)                       
        { 
            throw("Unable to set pin to KSSTATE_PAUSE"); 
        }

        bool    fState = true;
        CKsStreamRender KsRend(fState, pPin);
        if(!fState) 
        { 
            throw("Failed to create a CKsStreamRender object."); 
        }

        CKsStreamCapture KsCap(fState, pPin);
        if(!fState) 
        { 
            throw("Failed to create a CKsStreamCapture object."); 
        }

        // submit data to the kmixer sink pin 
        if (!fRTPin) 
        {            
            if(render == g_pKsPosTst->m_pHalf->m_flow)
            {   
                SLOG(eInfo1, "Non RT render pin");

                if(!KsRend.Play(pwfx, hWaitForFinish)) 
                {
                    throw("Standard streaming render failed."); 
                }
            }
            else
            {
                SLOG(eInfo1, "Non RT capture pin");

                fRes = pPin->SetState(KSSTATE_RUN);
                if (!fRes) 
                {
                    throw("Failed to set state on pin to KSSTATE_RUN");
                }

                if(!KsCap.Capture(pwfx, hWaitForFinish))    
                { 
                    throw("Standard streaming capture failed."); 
                }
            }
        }

        // Purpose of this test is to render/capture a specific amount of bytes and then once that amount
        // has been streamed/collected see how much time it took.

        // run the adapter sink pin
        fRes = pPin->SetState(KSSTATE_RUN);
        if (!fRes)
        { 
            throw("Failed to set state on pin to KSSTATE_RUN");
        }

        // mark the time
        tStart = xGetTime();

        if(fRTPin)
        {
            SLOG(eInfo1, "Sleep %lg msec for RT pin", tExpected);

            // Wait for tExpected sec and later check if the position move to here as well
            Sleep((ULONG)(tExpected));
        }
        else
        {
            SLOG(eInfo1, "Wait %lg msec for Non RT pin", tExpected * dWaitFactor);
            SLOG(eInfo1, "Time start");

            DWORD waitResult = WaitForSingleObject(hWaitForFinish, (ULONG)(tExpected * dWaitFactor));

            SLOG(eInfo1, "Time end");

            if( WAIT_TIMEOUT == waitResult )
            {
                throw("Timed out waiting for pin data IRPs to complete");
            }
            else if( WAIT_FAILED == waitResult )
            {
                SLOG(eWarn1, "FAIL: Error %u occurred while waiting for pin data IRPs to complete", GetLastError());
                throw("Failed on wait for pin data IRPs to complete");
            }
        }

        // get position
        fRes = pPin->GetPosition(&pos);
        if (!fRes)
        {
            throw("Failed to get pin stream position");
        }

        // get the finish time
        tEnd = xGetTime();

#ifdef USB_BUG
        DurationThreshold = ( fIsUSBPin ? DURATION_THRESHOLD_USB : DURATION_THRESHOLD );
        if(fIsUSBPin && (capture == g_pKsPosTst->m_pHalf->m_flow))
        {
            // Capture for USB does not peg the play offset to the write offset.
            // Until it is resolved of what exactly is to be expected, fudge the
            // threshold by 500 ms, which is 25% error on time.
            LengthThreshold = (500 * dWaitFactor * pwfx->nSamplesPerSec / 1000) * pwfx->nBlockAlign;
            DurationThreshold = 25;
        }
#endif

        ulLen = (ULONG)pos.PlayOffset;
        lLenError = ulLen - ulLenExpected;
        if ((labs(lLenError)/ulLenExpected) <= LengthThreshold)
        {
            SLOG(eInfo1, "PASS: Length difference is no more than %.05f %% and was reported correctly.", (labs(lLenError) / (double)ulLenExpected));
        }
        else
        {
            SLOG(eWarn1, "FAIL: Driver streamed %d bytes instead of %d.", ulLen, ulLenExpected);
            dwRes = FNS_FAIL;
        }
        // spew
        SLOG(eInfo2, "Length:");
        SLOG(eInfo2, "   Measured: %d bytes", ulLen);
        SLOG(eInfo2, "   Expected: %d bytes", ulLenExpected);
        SLOG(eInfo2, "   Error:    %d bytes", lLenError);

        SLOG(eInfo2, "   Time reading before starting of wait preiod: %.03f ms", tStart);
        SLOG(eInfo2, "   Time reading at the end of wait preiod: %.03f ms", tEnd);

        tLen = tEnd - tStart;
        tError = tLen - tExpected;
        tErrorPerc = (tError / tExpected) * 100.;
        SLOG(eInfo2, "Duration:");
        SLOG(eInfo2, "   Measured: %.03f ms", tLen);
        SLOG(eInfo2, "   Expected: %.03f ms", tExpected);
        SLOG(eInfo2, "   Error:    %.03f ms (%.3f %%)", tError, tErrorPerc);

        if (fabs(tErrorPerc) <= DurationThreshold)
        {
            SLOG(eInfo1, "PASS: The duration error for this format is no more than %.03f %%, and is therefore acceptable", DurationThreshold);
        }
        else
        {
            SLOG(eWarn1, "FAIL: The duration error for this format exceeds the acceptable limit of %.03f %%", DurationThreshold);
            dwRes = FNS_FAIL;
        }
    }
    // handle bad things
    catch (char* str)
    {
        SLOG(eError, "FAIL: %s", str);
        dwRes = FNS_FAIL;
    }

Exit:

    // Clean up
    if (pPin)
    {
        pPin->SetState(KSSTATE_STOP);
        pPin->ClosePin();
    }

    // hWaitForFinish could not be NULL, given we would have checked that and exited above.
    CloseHandle(hWaitForFinish); 
    dwRes = (fRes ? dwRes : FNS_FAIL);
    return dwRes;
}

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
DWORD
Test_Progression
(
    void
)
{
    DWORD           dwRes = FNS_PASS;
    BOOL            fRes = FALSE;
    CPCMAudioPin*   pPin = g_pKsPosTst->m_pHalf->m_pPin;
    WAVEFORMATEX*   pwfx = &g_pKsPosTst->m_pHalf->m_pFmtCurrent->wfxFormat.Format;
    BOOL            fTestingLoopedDevice = g_pKsPosTst->m_fLoop;
    CKsFilter*      pFilter = g_pKsPosTst->m_pHalf->m_pFilter;
    bool            fRTPin = g_pKsPosTst->m_pHalf->IsWaveRTPin();
    
    if(!pFilter)
    {
        SLOG(eError, "Unable to find a CKsFilter for current device");
        return FNS_FAIL;
    }

    if(!g_pKsPosTst->m_pHalf->IsTestSupportedForDeviceType(fTestingLoopedDevice) || g_pKsPosTst->m_pHalf->IsBTHPin())
    {
        XLOG(XSKIP, eError, "Test is not supported for the current device type.");
        return FNS_SKIPPED;
    }

    try
    {
        KSAUDIO_POSITION    pos;
        KSAUDIO_POSITION    posLast = { 0, 0 };
        ULONG               ulPosEnd = 0;
        BOOL                fSignaled = FALSE;
        ULONG               cSame = 0;
        BOOL                fWriteWrapped = FALSE;

        fRes = g_pKsPosTst->m_pHalf->PreparePin(pwfx, fTestingLoopedDevice);
        if (!fRes)
        {
            throw("");
        }
        SLOG(eInfo2, "Submitting %d bytes to the pin", pPin->m_cbStreamData);

        if (g_pKsPosTst->m_pHalf->IsWaveRTPacketStreamingSupported())
        {
            XLOG(XSKIP, eError, "The test doesn't support packet based pin.");
            fRes = TRUE;
            dwRes = FNS_SKIPPED;
            goto Exit;
        }

        // Unlooped Case: we will play the buffer and wait on IRP completion
        ulPosEnd = g_pKsPosTst->m_pHalf->m_pPin->m_cbStreamData;
        // Looping Case: then we will play the buffer a little less than one time
        if (fTestingLoopedDevice)
        {
            ulPosEnd -= 2000;
        }
        SLOG(eInfo2, " We will wait till cursor will reach to ulPosEnd = %d ", ulPosEnd);

        // submit data to the sink pin 
        if (fRTPin) 
        {   // in case of RT port, it does not support IOCTL_KS_WRITE_STREAM and     IOCTL_KS_READ_STREAM
            fRes = pPin->SetState(KSSTATE_PAUSE);
        }
        else    
        {            
            if (g_pKsPosTst->m_pHalf->m_flow == render)
                fRes = pPin->WriteData();
            else
                fRes = pPin->ReadData();
        }
        if (!fRes)
            throw("Failed to submit buffer to pin");

        // run the adapter sink pin
        fRes = pPin->SetState(KSSTATE_RUN);
        if (!fRes)
            throw("Failed to set state on pin to KSSTATE_RUN");

        // wait for the pin to play all of the data
        do
        {
            Sleep(1);

            // get position
            fRes = pPin->GetPosition(&pos);
            SLOG(eInfo2, "Last Play offset = (%d), Current Play offset = (%d)", (ULONG)posLast.PlayOffset, (ULONG)pos.PlayOffset);

            if (!fRes)
                throw("Failed to get pin stream position");

            if (g_pKsPosTst->m_fLogHistograms)
                SLOG(eBlab1, "Play cursor (%d) Write cursor (%d)", (ULONG)pos.PlayOffset, (ULONG)pos.WriteOffset);

            if (posLast.PlayOffset == pos.PlayOffset)
            {
                cSame++;
                Sleep(2);
            }
            else
            {
                cSame = 0;

                if (posLast.PlayOffset > pos.PlayOffset)
                {
                    SLOG(eWarn1, "Play position went backward from %d to %d bytes", (ULONG)posLast.PlayOffset, (ULONG)pos.PlayOffset);
                    dwRes = FNS_FAIL;
                }
            }

            if (cSame > 100)
            {
                SLOG(eWarn1, "Play cursor is stuck at %d bytes", (ULONG)posLast.PlayOffset);
                throw ("Play cursor is stuck.");
            }

            if (pos.WriteOffset < posLast.WriteOffset)
            {
                SLOG(eBlab1, "Write Cursor wrapped around");
                fWriteWrapped = TRUE;
            }

            if (g_pKsPosTst->m_pHalf->m_flow == render)
            {
                if (pos.PlayOffset > pos.WriteOffset)
                {
                    // in non-looped case, the Write cursor should always be ahead of the play cursor
                    // in looped case, it's okay if the write cursor looped back to the begining
                    if (    (!fTestingLoopedDevice)  ||
                            (fTestingLoopedDevice && (!fWriteWrapped)) )
                    {
                        SLOG(eWarn1, "The Play cursor (%d) is ahead of the Write cursor (%d)", (ULONG)pos.PlayOffset, (ULONG)pos.WriteOffset);
                        dwRes = FNS_FAIL;
                    }
                }
            }

            CopyMemory(&posLast, &pos, sizeof(posLast));

            fSignaled = pPin->IsSignaled();
            SLOG(eInfo2, "pos.PlayOffset = %d,  ulPosEnd = %d  ", (ULONG)pos.PlayOffset, ulPosEnd);      // added for more logging information.

            } while ((!fSignaled) && ((ULONG)pos.PlayOffset < ulPosEnd));

        if (FNS_FAIL != dwRes)
        {
            if (g_pKsPosTst->m_pHalf->m_flow == render)
                SLOG(eInfo1, "PASS: The Play cursor consistently increased over time and stayed behind the Write cursor");
            else
                SLOG(eInfo1, "PASS: The Record cursor consistently increased over time");
        }
        else
        {
            if (g_pKsPosTst->m_pHalf->m_flow == render)
                SLOG(eWarn1, "FAIL: The Play cursor did not consistently increase over time and stay behind the Write cursor");
            else
                SLOG(eWarn1, "FAIL: The Record cursor did not consistently increase over time");
        }
    }

    // handle bad things
    catch (char* str)
    {
        SLOG(eError, "FAIL: %s", str);
        dwRes = FNS_FAIL;
    }

Exit:

    if (pPin)
    {
        pPin->SetState(KSSTATE_STOP);
        pPin->ClosePin();
    }
    dwRes = (fRes ? dwRes : FNS_FAIL);
    return dwRes;
}

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
BOOL CollectSampleData
(
    CHalfApp*   pHalf,
    DWORD*      pdwRes,
    bool        fRTPin,
    bool        fIsUSBPin
)
{
    BOOL            fRes = FALSE;
    WAVEFORMATEX*   pwfx = &pHalf->m_pFmtCurrent->wfxFormat.Format;
    PFORMAT_ENTRY   pFormatEntry = pHalf->m_pFmtCurrent;

    if (!pFormatEntry)
    {
        SLOG(eError, "FAIL: Current Format Entry is NULL.");
        return FALSE;
    }

    PPERF_RESULTS   pResults = &pFormatEntry->perfResults;
    CPCMAudioPin*   pPin = pHalf->m_pPin;
    BOOL            fTestingLoopedDevice = g_pKsPosTst->m_fLoop;

    SLOG(eInfo2, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    SLOG(eInfo2, "Building time vs. position histogram.  Please stand by...");

    pResults->argPosSets = (PPOSITION_SET)LocalAlloc(LPTR, DATA_POINTS * sizeof(POSITION_SET));

    try
    {
        KSAUDIO_POSITION pos;
        double           tStart = 0, tPre = 0, tPost = 0;
        UINT             i = 0;
        ULONG            ulTimeStop = 0, ulLenStop = 0;

        ulTimeStop = (fIsUSBPin) ? USB_BUFLEN_MS : BUFLEN_MS;
        fRes = g_pKsPosTst->m_pHalf->PreparePinConsiderUSB(pwfx, fTestingLoopedDevice, FALSE);
        if (!fRes)
        {
            throw("Failed to prepare pin for testing");
        }

        // exceeding this will stop data collection
        ulLenStop = ulTimeStop * pwfx->nAvgBytesPerSec / 1000 - 200;

        SLOG(eInfo2, "Submitting %d bytes to the pin", pPin->m_cbStreamData);        // added for more logging information.

        if (g_pKsPosTst->m_pHalf->IsWaveRTPacketStreamingSupported())
        {
            XLOG(XSKIP, eError, "The test doesn't support packet based pin.");
            *pdwRes = FNS_SKIPPED;
            fRes = FALSE;
            goto Exit;
        }

        // submit data to the kmixer sink pin 
        if (fRTPin) {   // in case of RT port, it does not support IOCTL_KS_WRITE_STREAM and     IOCTL_KS_READ_STREAM
            fRes = pPin->SetState(KSSTATE_PAUSE);
        }
        else    {
            if (g_pKsPosTst->m_pHalf->m_flow == render)
                fRes = pPin->WriteData();
            else
                fRes = pPin->ReadData();
        }

        if (!fRes)
            throw("Failed to Write Data to Renderer sink pin");

        // run the adapter sink pin
        fRes = pPin->SetState(KSSTATE_RUN);
        if (!fRes)
            throw("Failed to set state on Renderer sink pin to KSSTATE_RUN");

        tStart = xGetTime();

        // make sure the position-fetching code is all paged in by exercising that code path
        pPin->GetPosition(&pos);
        Sleep(40);
        int nLoop = 0;       // to keep track of which loop is streaming - first time, second time etc... At every end of buffer we will increament it by one
        double posDiff = 0;    // position difference between current and previous position 

        // loop until we get a non-zero position
        for (i = 0; i < DATA_POINTS;)
        {
            Sleep(1);

            tPre = xGetTime();

            fRes = pPin->GetPosition(&pos);
            if (!fRes)
                throw("GetPosition failed");

            tPost = xGetTime();

            if (pos.PlayOffset > 0)
            {
                // only count this result if the GetPosition and GetTime happened relatively atomically
                if ((tPost - tPre) < 1.)
                {
                    if (i){    // Detect loop. if we are again at the begining of buffer after finishing buffer data then increment nLoop and add buffer lenght into postion.
                        // Here we are assuming that difference between 2 successive position will not be more then half of buffer length
                        posDiff = abs((double)((ULONG)pos.PlayOffset) - (pResults->argPosSets[i - 1].dPos - ((double)(pPin->m_cbStreamData)*nLoop)));
                        if ((posDiff) >(double)((pPin->m_cbStreamData) / 2))    {
                            nLoop++;
                            SLOG(eInfo2, "running %d loop", nLoop + 1);
                        }

                    }
                    pResults->argPosSets[i].dTime = (double)((tPost + tPre) / 2 - tStart);
                    pResults->argPosSets[i].dPos = (double)((ULONG)pos.PlayOffset) + ((double)(pPin->m_cbStreamData)*nLoop);        // for bug 788024
                    //                    pResults->argPosSets[i].dPos = (double)((ULONG)pos.PlayOffset);

                    // detect if we have stopped moving.  If so, we are done collecting data
                    if
                        (
                        (i > 0) &&
                        (pResults->argPosSets[i - 1].dPos == pResults->argPosSets[i].dPos)
                        )
                    {
                        // try to detect false alarms
                        if (abs(tPost - tStart - (double)BUFLEN_MS) < 500.)
                        {
                            // decrement the count by one, since the last two values are 
                            //  identical and we don't want to artificially weight the end of the buffer
                            SLOG(eBlab1, "Position stopped moving");
                            break;
                        }
                        else
                        {
                            SLOG(eInfo2, "Warning:  Position appears to be stalled at t = %d ms, pos = %d bytes",
                                (ULONG)pResults->argPosSets[i].dTime, (ULONG)pResults->argPosSets[i].dPos);
                        }
                    }

                    i++;
                }
                else
                {
                    SLOG(eBlab2, "GetPosition function too slow");
                }

                if (pos.PlayOffset > ulLenStop)
                {
                    SLOG(eBlab2, "Exceeded position data collection range...");
                    break;
                }
            }
            else
            {
                if ((tPost - tStart) > 1000)
                {
                    throw("Position didn't move within 1 second");
                }
            }

            pResults->cPosSets = i;
        }
    }
    // handle bad things
    catch (char* str)
    {
        *pdwRes = FNS_FAIL;
        fRes = FALSE;
        SLOG(eError, "FAIL: %s", str);
    }

    //  Did we get enough data?
    //
    if (pResults->cPosSets < 30)
    {
        *pdwRes = FNS_FAIL;
        fRes = FALSE;
        SLOG(eWarn1, "FAIL: not enough position data for a valid test.");
        SLOG(eWarn1, "       Actual number of collected data points = %u.", pResults->cPosSets);
        SLOG(eWarn1, "       The value of the final data point collected is [ time = %d ms, Play position = %d bytes ]",
            (ULONG)pResults->argPosSets[pResults->cPosSets].dTime,
            (ULONG)pResults->argPosSets[pResults->cPosSets].dPos);
    }

Exit:

    //
    // clean up
    //
    if (pPin)
    {
        pPin->SetState(KSSTATE_STOP);
        pPin->ClosePin();
    }

    pFormatEntry->fPerfDataCollected = fRes;

    return fRes;
}

// --------------------------------------------------------------------------------------------------
// CalculateSampleRate
// --------------------------------------------------------------------------------------------------
BOOL CalculateSampleRate
(
    CHalfApp*   pHalf,
    DWORD*      pdwRes
)
{
    BOOL            fRes = TRUE;
    WAVEFORMATEX*   pwfx = &pHalf->m_pFmtCurrent->wfxFormat.Format;
    PFORMAT_ENTRY   pFormatEntry = pHalf->m_pFmtCurrent;
    PPERF_RESULTS   pResults = &pFormatEntry->perfResults;

    double dError, dErrorPercent;
    double dSampleRateExpected = (double)(pwfx->nSamplesPerSec * pwfx->nBlockAlign);

    PDPOINT pArray = (PDPOINT)pResults->argPosSets;         // array o' points
    ULONG   cArray = pResults->cPosSets;                    // number of points

    // calculate the slope and intercept (sample rate and latency)
    pResults->dSampleRate = SLOPE(pArray, cArray);
    pResults->dOffset     = INTERCEPT(pArray, cArray, pResults->dSampleRate);

    // Convert from bytes/ms to bytes/sec
    pResults->dSampleRate *= 1000.;

    // calculate errors
    dError = pResults->dSampleRate - dSampleRateExpected;
    dErrorPercent = dError / dSampleRateExpected * 100.;
    
    SLOG(eInfo2, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    SLOG(eInfo2, "SampleRate calculated from histogram (method of least squares)\n");

    SLOG(eInfo2, "Collected %d data points", pResults->cPosSets);
    SLOG(eInfo2, "Specified sample rate  = %6.3f bytes/sec, %6.3f samples/sec", (double)(pwfx->nBlockAlign * pwfx->nSamplesPerSec), (double)pwfx->nSamplesPerSec);
    SLOG(eInfo2, "Calculated sample rate = %6.3f bytes/sec, %6.3f samples/sec", pResults->dSampleRate, pResults->dSampleRate / (double)pwfx->nBlockAlign);
    SLOG(eInfo2, "Error = %.3f %%", dErrorPercent);

    // evaluate the results
    if (abs(dErrorPercent) <= SAMPLERATE_THRESHOLD)
    {
        SLOG(eInfo1, "PASS: The sample rate drift for this format is no more than %.03f %%, and is therefore acceptable", SAMPLERATE_THRESHOLD);
    }
    else
    {
        SLOG(eWarn1, "FAIL: The sample rate drift for this format exceeds the acceptable limit of %.03f %%", SAMPLERATE_THRESHOLD);
        *pdwRes = FNS_FAIL;
    }

    return fRes;
}

inline void MakeStatDump
(
    PSTAT_DUMP  pSD,
    double      dBytes,
    ULONG       nBytesPerSample,
    double      dSamplesPerSecond
)
{
    pSD->b = dBytes;
    pSD->s = pSD->b / (double)nBytesPerSample;
    pSD->t = (double)pSD->s * 1000. / dSamplesPerSecond;
}

// --------------------------------------------------------------------------------------------------
// CalculateJitter
// --------------------------------------------------------------------------------------------------
BOOL CalculateJitter
(
    CHalfApp*   pHalf,
    DWORD*      pdwRes
)
{
    BOOL            fRes = TRUE;
    WAVEFORMATEX*   pwfx = &pHalf->m_pFmtCurrent->wfxFormat.Format;
    PFORMAT_ENTRY   pFormatEntry = pHalf->m_pFmtCurrent;
    PPERF_RESULTS   pResults = &pFormatEntry->perfResults;

    double          dPosExpected, dError, dErrorAve, dErrorMax, dErrorMS;
    double          dStdDeviation;
    double          dUnshifted;
    ULONG           i;
    PPOSITION_SET   prgData = pResults->argPosSets;

    dErrorAve = 0.;
    dErrorMax = 0.;
    dStdDeviation = 0.;

    if (g_pKsPosTst->m_fLogHistograms)
    {
        SLOG(eBlab1, "");
        SLOG(eBlab1, "                      Latency-");
        SLOG(eBlab1, "          Reported    compensated Expected");
        SLOG(eBlab1, "Time      position    position    position    Error       Error");
        SLOG(eBlab1, "(ms)      (bytes)     (bytes)     (bytes)     (bytes)     (ms)");
        SLOG(eBlab1, "========  ==========  ==========  ==========  ==========  ==========");
    }

    for (i = 0; i < pResults->cPosSets; i++)
    {
        dPosExpected = prgData[i].dTime * pResults->dSampleRate / 1000.;

        dUnshifted = (ULONG)prgData[i].dPos;
        prgData[i].dPos = dUnshifted - pResults->dOffset;

        dError = dPosExpected - prgData[i].dPos;
        dErrorAve += dError;

        dErrorMS = dError * 1000. / pResults->dSampleRate;

        if (g_pKsPosTst->m_fLogHistograms)
        {
            SLOG(eBlab1, "%8.3f  %10d  %10.2f  %10.2f  %10.2f  %10.3f", prgData[i].dTime, (ULONG)dUnshifted, prgData[i].dPos, dPosExpected, dError, dErrorMS);
        }

        if (dError < 0.)
            dError = -dError;
        if (dError > dErrorMax)
            dErrorMax = dError;
    }

    dErrorAve /= (double)pResults->cPosSets;

    ASSERT((ULONG)dErrorAve < 2);      // if this is not true then, there's something wrong with our compensation...

    MakeStatDump(&pResults->sdMax, dErrorMax, pwfx->nBlockAlign, pResults->dSampleRate);

    SLOG(eInfo2, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    SLOG(eInfo2, "Jitter calculated from histogram (with (samplerate drift and latency) compensation)\n");


    SLOG(eInfo2, "Max deviation      = %6.2f bytes, %6.2f samples, %6.3f ms", pResults->sdMax.b, pResults->sdMax.s, pResults->sdMax.t);

    for (i = 0; i < pResults->cPosSets; i++)
    {
        dPosExpected = prgData[i].dTime * pResults->dSampleRate / 1000.;
        dError = dPosExpected - prgData[i].dPos;
        dStdDeviation += ((dError - dErrorAve) * (dError - dErrorAve));
    }

    dStdDeviation /= (double)pResults->cPosSets;
    dStdDeviation = sqrt(dStdDeviation);

    MakeStatDump(&pResults->sdStdDev, dStdDeviation, pwfx->nBlockAlign, pResults->dSampleRate);

    SLOG(eInfo2, "Standard deviation = %6.2f bytes, %6.2f samples, %6.3f ms", pResults->sdStdDev.b, pResults->sdStdDev.s, pResults->sdStdDev.t);

    // evaluate the results ~~~~~~~~~~~~~

    if (abs(pResults->sdMax.t) >= JITTER_MAX_THRESHOLD)
    {
        SLOG(eWarn1, "FAIL: The maximum deviation for position error for this format exceeds the acceptable limit of %.03f ms", JITTER_MAX_THRESHOLD);
        *pdwRes = FNS_FAIL;
    }
    else
    {
        SLOG(eInfo1, "PASS: The maximum deviation for position error for this format is no more than %.03f ms, and is therefore acceptable", JITTER_MAX_THRESHOLD);
    }

    if (abs(pResults->sdStdDev.t) >= JITTER_STD_THRESHOLD)
    {
        SLOG(eWarn1, "FAIL: The standard deviation for position error for this format exceeds the acceptable limit of %.03f ms", JITTER_STD_THRESHOLD);
        *pdwRes = FNS_FAIL;
    }
    else
    {
        SLOG(eInfo1, "PASS: The standard deviation for position error for this format is no more than %.03f ms, and is therefore acceptable", JITTER_STD_THRESHOLD);
    }

    return fRes;
}
/*
// --------------------------------------------------------------------------------------------------
// SearchForNegProg
//   Searches the histogram for negative position changes
//   Position should ALWAYS increase
// --------------------------------------------------------------------------------------------------
BOOL SearchForNegProg
(
    CHalfApp*   pHalf,
    DWORD*      pdwRes
)
{
    PPERF_RESULTS   pResults = &pHalf->m_pFmtCurrent->perfResults;
    PPOSITION_SET   prgData = pResults->argPosSets;

    ULONG           i;
    BOOL            fRes = TRUE;        // assume there are no backwards movements

    SLOG(eInfo2, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    SLOG(eInfo2, "Making sure stream Play cursor never decreases\n");

    for (i = 1; i < pResults->cPosSets; i++)
    {
        if (prgData[i].dPos < prgData[i - 1].dPos)
        {
            SLOG
            (
                eWarn1, 
                "FAIL: At sample %d of %d, the Play cursor moved backwards from %d to %d",
                i, 
                pResults->cPosSets,
                (ULONG)prgData[i - 1].dPos,
                (ULONG)prgData[i].dPos
            );
            *pdwRes = FNS_FAIL;
        }
    }

    if (PASSED(*pdwRes))
        SLOG(eInfo1, "PASS: No backwards movement was detected.");

    return fRes;
}
*/

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
DWORD
Test_ReadPacketMain
(
    _In_ BOOL CheckPacketSignature, // Whether to check that the audio samples actually contain specific data
    _In_opt_ LPCWSTR Filename       // Filename to use to save captured data in WAV format
)
{
    DWORD           dwRes = FNS_PASS;
    CKsFilter*      pFilter = g_pKsPosTst->m_pHalf->m_pFilter;
    CPCMAudioPin*   pPin = g_pKsPosTst->m_pHalf->m_pPin;
    PPERF_RESULTS   pResults = &g_pKsPosTst->m_pHalf->m_pFmtCurrent->perfResults;
    WAVEFORMATEX*   pwfx = &g_pKsPosTst->m_pHalf->m_pFmtCurrent->wfxFormat.Format;
    HANDLE          bufferEvent = NULL;
    LARGE_INTEGER   qpcBaseline;
    LARGE_INTEGER   qpcFrequency;
    ULONG           packetSize;
    int             i;
    const ULONG     nNotifications = 2;

    CComPtr<IWaveFileSink> spWaveFileSink;
    KSRTAUDIO_GETREADPACKET_INFO rpi;

    // This structure contains all interesting data gathered from one test loop iteration
    typedef struct
    {
        LARGE_INTEGER                   qpc;
        KSRTAUDIO_GETREADPACKET_INFO    ReadPacketInfo;
        DWORD                           BufferEventWaitResult;
        KSAUDIO_POSITIONEX              AudioPosition;
        union
        {
            UINT16                      AsUint16[8];
            LONGLONG                    AsLonglong[2];
        } Samples;
    } DATAPOINT;

    DATAPOINT data[DATA_POINTS+1];    // Collection of all data gathered from the main test loop

    if (!pFilter)
    {
        SLOG(eError, "Unable to find a CKsFilter for current device");
        return FNS_FAIL;
    }

    if (!g_pKsPosTst->m_pHalf->IsWaveRTPin())
    {
        XLOG(XSKIP, eError, "Test is not supported for the current device type. Not a WaveRT pin.");
        return FNS_SKIPPED;
    }

    if (!g_pKsPosTst->m_pHalf->IsTestSupportedForDeviceType(g_pKsPosTst->m_fLoop))
    {
        XLOG(XSKIP, eError, "Test is not supported for the current device type.");
        return FNS_SKIPPED;
    }

    if (IsPinOffloadCapable(pPin))
    {
        XLOG(XSKIP, eError, "SKIP: Test is not supported on the offload pin.");
        return FNS_SKIPPED;
    }

    // Instantiate the KS pin
    VERIFY_WIN32_BOOL_SUCCEEDED(g_pKsPosTst->m_pHalf->PreparePin(&g_pKsPosTst->m_pHalf->m_pFmtCurrent->wfxFormat.Format, g_pKsPosTst->m_fLoop, 20, 0, nNotifications));

    if (!g_pKsPosTst->m_pHalf->m_fIsWaveRTPinWithReadPacketSupport)
    {
        XLOG(XSKIP, eError, "Test is not supported for the current device type - KSPROPERTY_RTAUDIO_GETREADPACKET not supported.");
        return FNS_SKIPPED;
    }

    // Register buffer notification event
    KSRTAUDIO_NOTIFICATION_EVENT_PROPERTY notificationEventProperty;
    bufferEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    notificationEventProperty.NotificationEvent = bufferEvent;
    VERIFY_WIN32_BOOL_SUCCEEDED(pPin->SetPropertySimple(KSPROPSETID_RtAudio, KSPROPERTY_RTAUDIO_REGISTER_NOTIFICATION_EVENT, NULL, 0, &notificationEventProperty.NotificationEvent, sizeof(notificationEventProperty) - sizeof(KSPROPERTY)));

    // The packet size
    packetSize = pPin->m_cbStreamData / nNotifications;

    // Verify correct error if called while stopped
    VERIFY_ARE_EQUAL((DWORD)WAIT_TIMEOUT, WaitForSingleObject(bufferEvent, 0));
    VERIFY_WIN32_BOOL_FAILED(pPin->GetPropertySimple(KSPROPSETID_RtAudio, KSPROPERTY_RTAUDIO_GETREADPACKET, &rpi, sizeof(rpi)));
    VERIFY_ARE_EQUAL((DWORD)ERROR_BAD_COMMAND, GetLastError());

    // Verify correct error if called while paused
    VERIFY_WIN32_BOOL_SUCCEEDED(pPin->SetState(KSSTATE_PAUSE));

    // ISSUE-2014/10/18-frankye Contract? Should buffered packets be readable during KSSTATE_PAUSE? Perhaps yes but flush on transition to KSSTATE_RUN.
    //// Verify event not signaled
    //VERIFY_ARE_EQUAL((DWORD)WAIT_TIMEOUT, WaitForSingleObject(bufferEvent, 0));
    //VERIFY_WIN32_BOOL_FAILED(pPin->GetPropertySimple(KSPROPSETID_RtAudio, KSPROPERTY_RTAUDIO_GETREADPACKET, &readPacketInfo, sizeof(readPacketInfo)));
    //VERIFY_ARE_EQUAL((DWORD)ERROR_NOT_READY, GetLastError());

    // Snap a reference time, just to make displayed times smaller and more human readable.
    // Make the reference time 2 seconds ago, since timestamps on burst reads may be in the past.
    QueryPerformanceFrequency(&qpcFrequency);
    QueryPerformanceCounter(&qpcBaseline);
    qpcBaseline.QuadPart -= (2 * qpcFrequency.QuadPart);    // Minus 2 seconds

    // Start running
    VERIFY_WIN32_BOOL_SUCCEEDED(pPin->SetState(KSSTATE_RUN));

    // Normally data is not ready immediately after going to KSSTATE_RUN. But if this thread is
    // preempted just after setting KSSTATE_RUN, or if data is bursting from a earlier keyword
    // detection, then data may in fact be available. So there is no simple way to test that the
    // driver hasn't prematurely indicated that data is available.
    //VERIFY_ARE_EQUAL((DWORD)WAIT_TIMEOUT, WaitForSingleObject(bufferEvent, 0));
    //VERIFY_WIN32_BOOL_FAILED(pPin->GetPropertySimple(KSPROPSETID_RtAudio, KSPROPERTY_RTAUDIO_GETREADPACKET, &readPacketInfo, sizeof(readPacketInfo)));
    //VERIFY_ARE_EQUAL((DWORD)ERROR_NOT_READY, GetLastError());

    pResults->argPosSets = (PPOSITION_SET)LocalAlloc(LPTR, DATA_POINTS * sizeof(POSITION_SET));

    if (Filename != nullptr)
    {
        VERIFY_SUCCEEDED(CreateWaveFileSink(g_pBasicLog, XFAIL, Filename, pwfx, &spWaveFileSink));
    }

    // Main test loop. Gather data into an array of datapoints from each iteration.
    i = 0;
    do
    {
        BOOL moreData;
        DWORD waitResult;

        // Wait for captured packet
        VERIFY_ARE_EQUAL((DWORD)WAIT_OBJECT_0, WaitForSingleObject(bufferEvent, INFINITE));

        do
        {
            DATAPOINT *dp = &data[i];

            QueryPerformanceCounter(&dp->qpc);

            // ISSUE-2014/10/18-frankye Ensure driver signals only when packet is ready after bursting
            //VERIFY_WIN32_BOOL_SUCCEEDED(pPin->GetPropertySimple(KSPROPSETID_RtAudio, KSPROPERTY_RTAUDIO_GETREADPACKET, &dp->ReadPacketInfo, sizeof(dp->ReadPacketInfo)));
            if (!pPin->GetPropertySimple(KSPROPSETID_RtAudio, KSPROPERTY_RTAUDIO_GETREADPACKET, &dp->ReadPacketInfo, sizeof(dp->ReadPacketInfo)))
            {
                break;
            }

            waitResult = WaitForSingleObject(bufferEvent, 0);
            dp->BufferEventWaitResult = waitResult;

            // A short sleep gives time for buffer position to advance a little. This helps to visually
            // check whether the buffer position appears snapped to packet boundaries. This shouldn't
            // be the case for unbuffered capture.
            //Sleep(1);
            VERIFY_WIN32_BOOL_SUCCEEDED(pPin->GetPropertySimple(KSPROPSETID_Audio, KSPROPERTY_AUDIO_POSITIONEX, &dp->AudioPosition, sizeof(dp->AudioPosition)));

            // Save the first several bytes of audio sample data and accumulate file to write
            PBYTE packetAsBytePtr = pPin->m_pbStreamData + ((dp->ReadPacketInfo.PacketNumber * packetSize) % pPin->m_cbStreamData);
            RtlCopyMemory(&dp->Samples.AsLonglong[0], packetAsBytePtr, sizeof(dp->Samples.AsLonglong));
            if (spWaveFileSink)
            {
                VERIFY_SUCCEEDED(spWaveFileSink->ConsumeBuffer(packetAsBytePtr, packetSize / pwfx->nBlockAlign, packetSize));
            }

            moreData = dp->ReadPacketInfo.MoreData;
            i++;

        } while ((i < DATA_POINTS) && (moreData || waitResult == WAIT_OBJECT_0));

    } while (i < DATA_POINTS);

    if (spWaveFileSink)
    {
        VERIFY_SUCCEEDED(spWaveFileSink->Finalize());
    }

    // View and process the collected data_
    for (i = 0; i < DATA_POINTS; i++)
    {
        DATAPOINT *dp = &data[i];

        // Extrapolate WaveRT buffer write offset to the time the read packet was captured
        LONGLONG bufferOffsetPerformanceCount = (dp->AudioPosition.TimeStamp1.QuadPart + dp->AudioPosition.TimeStamp2.QuadPart) / 2;
        LONGLONG deltaBufferOffset = (bufferOffsetPerformanceCount - dp->ReadPacketInfo.PerformanceCounterValue) * pwfx->nAvgBytesPerSec / qpcFrequency.QuadPart;
        LONGLONG writeOffsetWhenPacketCaptured = (dp->AudioPosition.Position.WriteOffset - deltaBufferOffset) % pPin->m_cbStreamData;

        // Initialize these just to result in a displayed value of "0" if the test doesn't actually use the audio data
        LONGLONG   embeddedPacketNumber = 0;
        LONGLONG   embeddedTimeCapture = qpcBaseline.QuadPart;

        if (CheckPacketSignature)
        {
            // The sysvad Contoso keyword detector embeds some test data into the audio sample data
            embeddedPacketNumber = dp->Samples.AsLonglong[0];
            embeddedTimeCapture = dp->Samples.AsLonglong[1];
        }

        SLOG(eInfo1, "t=%6d | pn=%5d pt=%6d pp=%6d mo=%1d | e=%1d | bt=%6d wo=%5d po=%5d wo[pt]=%5d | epn=%5d ept=%6d",
            (int)QpcToRelativeMs(dp->qpc.QuadPart, qpcBaseline, qpcFrequency),                              // The time the loop started
            (int)dp->ReadPacketInfo.PacketNumber,                                                           // The read packet number
            (int)QpcToRelativeMs(dp->ReadPacketInfo.PerformanceCounterValue, qpcBaseline, qpcFrequency),    // The timestamp on the packet info
            (int)(dp->ReadPacketInfo.PacketNumber * packetSize),                                            // The stream sample position derived from packet number
            (int)dp->ReadPacketInfo.MoreData,                                                               // Whether the driver indicated more data available
            (int)(dp->BufferEventWaitResult == WAIT_OBJECT_0),                                              // Whether the buffer event was signaled just after reading the packet
            (int)QpcToRelativeMs(bufferOffsetPerformanceCount, qpcBaseline, qpcFrequency),                  // The timestamp on the buffer position data
            (int)dp->AudioPosition.Position.WriteOffset,                                                    // The buffer write offset
            (int)dp->AudioPosition.Position.PlayOffset,                                                     // The buffer play offset
            (int)writeOffsetWhenPacketCaptured,                                                             // Extrapolation of buffer write offset to packet's timestamp
            (int)embeddedPacketNumber,                                                                      // The packet number embedded in the audio sample data
            (int)QpcToRelativeMs(embeddedTimeCapture, qpcBaseline, qpcFrequency)                            // The packet timestamp embedded in the audio sample data
            );

        // ISSUE-2014/10/18-frankye Verify driver signals buffer event from its
        // property handler if and only if returning MoreData
        //VERIFY_IS_FALSE((dp->ReadPacketInfo.MoreData) && (dp->BufferEventWaitResult != WAIT_OBJECT_0));
        //VERIFY_ARE_EQUAL(!!(dp->ReadPacketInfo.MoreData), !!(dp->BufferEventWaitResult == WAIT_OBJECT_0));

        // ISSUE-2014/10/18-frankye verify write offset is consistent with
        // packet position within tolerance of each other (ideally equal)

        if (CheckPacketSignature)
        {
            VERIFY_ARE_EQUAL(dp->ReadPacketInfo.PacketNumber, (ULONG)embeddedPacketNumber);
            VERIFY_ARE_EQUAL(dp->ReadPacketInfo.PerformanceCounterValue, (ULONG64)embeddedTimeCapture);
        }

        // Collect data for further analysis
        pResults->argPosSets[i].dTime = (double)QpcToRelativeMs(dp->ReadPacketInfo.PerformanceCounterValue, qpcBaseline, qpcFrequency);
        pResults->argPosSets[i].dPos = (double)(dp->ReadPacketInfo.PacketNumber * packetSize);
    }

    pResults->cPosSets = i;
    
    // Unregister buffer notification
    VERIFY_WIN32_BOOL_SUCCEEDED(pPin->SetPropertySimple(KSPROPSETID_RtAudio, KSPROPERTY_RTAUDIO_UNREGISTER_NOTIFICATION_EVENT, NULL, 0, &notificationEventProperty.NotificationEvent, sizeof(notificationEventProperty) - sizeof(KSPROPERTY)));
    
    g_pKsPosTst->m_pHalf->m_pPin->ClosePin();

    // calculate drift ~~~~~~~~~~~~~~~~~~
    VERIFY_WIN32_BOOL_SUCCEEDED(CalculateSampleRate(g_pKsPosTst->m_pHalf, &dwRes));
    VERIFY_ARE_EQUAL((DWORD)FNS_PASS, dwRes);

    // calculate Jitter ~~~~~~~~~~~~~~~~~
    VERIFY_WIN32_BOOL_SUCCEEDED(CalculateJitter(g_pKsPosTst->m_pHalf, &dwRes));
    VERIFY_ARE_EQUAL((DWORD)FNS_PASS, dwRes);

    return FNS_PASS;
}

DWORD
Test_ReadPacket(void)
{
    WCHAR filename[100];
    StringCbPrintfW(filename, sizeof(filename), L"Test_ReadPacket#%i.wav", g_pKsPosTst->m_uTestDataIndex);
    return Test_ReadPacketMain(FALSE, filename);
}

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
//
// ISSUE-2014/10/18-frankye Move to a fictitious "Contoso voice activation kit" header
//
// Identifier for Contoso keyword configuration data. Other hardware would
// define a different identifier for their configuration data.
//
// {6F7DBCC1-202E-498D-99C5-61C36C4EB2DC}
static const GUID CONTOSO_KEYWORDCONFIGURATION_IDENTIFIER =
{ 0x6f7dbcc1, 0x202e, 0x498d, { 0x99, 0xc5, 0x61, 0xc3, 0x6c, 0x4e, 0xb2, 0xdc } };

//
// The format of the Contoso keyword data.
//
// Hardware manufacturers define formats specific to their keyword detection
// technology to pass voice models, keyword data, speaker data, or any other
// data relevant to their technology.
//
typedef struct
{
    SOUNDDETECTOR_PATTERNHEADER Header;
    LONGLONG                    ContosoDetectorData;
} CONTOSO_KEYWORDCONFIGURATION;

DWORD
Test_KeywordBurstRead
(
void
)
{
    CKsFilter*                                  pFilter = g_pKsPosTst->m_pHalf->m_pFilter;
    CPCMAudioPin*                               pPin = g_pKsPosTst->m_pHalf->m_pPin;
    GUID                                        pinCategory;
    KSMULTIPLE_ITEM*                            patternsMultipleItem;
    GUID                                        patternType = GUID_NULL;
    BOOL                                        arm = TRUE;
    CComHeapPtr<SOUNDDETECTOR_PATTERNHEADER>    spPatternData;
    CComHeapPtr<KSMULTIPLE_ITEM>                spPatterns;
    CComPtr<IKeywordDetectorOemAdapter>         spAdapter;
    CComPtr<IStream>                            spStream;
    WCHAR                                       filename[100];

    KEYWORDSELECTOR keywordSelector = {
        KwVoiceAssistant,
        0x0409,         // Overwritten below by GetUserDefaultUILanguage
    };

    keywordSelector.LangId = GetUserDefaultUILanguage();

    VERIFY_WIN32_BOOL_SUCCEEDED(pFilter->GetPinPropertySimple(pPin->m_nId, KSPROPSETID_Pin, KSPROPERTY_PIN_CATEGORY, &pinCategory, sizeof(pinCategory)));
    if (pinCategory != KSNODETYPE_AUDIO_KEYWORDDETECTOR)
    {
        return FNS_SKIPPED;
    }

    VERIFY_WIN32_BOOL_SUCCEEDED(pFilter->GetPropertyMulti(KSPROPSETID_SoundDetector, KSPROPERTY_SOUNDDETECTOR_SUPPORTEDPATTERNS, &patternsMultipleItem));

    VERIFY_IS_GREATER_THAN(patternsMultipleItem->Count, (unsigned)0);
    VERIFY_IS_GREATER_THAN_OR_EQUAL(patternsMultipleItem->Size, sizeof(KSMULTIPLE_ITEM) + patternsMultipleItem->Count*sizeof(GUID));

    patternType = *(GUID*)(patternsMultipleItem + 1);

    LocalFree(patternsMultipleItem);

    VERIFY_SUCCEEDED(CoCreateInstance(patternType, nullptr, CLSCTX_ALL, __uuidof(IKeywordDetectorOemAdapter), (void**)&spAdapter));

    VERIFY_SUCCEEDED(CreateStreamOnHGlobal(NULL, TRUE, &spStream)); 
    VERIFY_SUCCEEDED(spAdapter->BuildArmingPatternData(spStream, &keywordSelector, 1, &spPatternData));
    
    VERIFY_IS_TRUE(spPatterns.AllocateBytes(sizeof(KSMULTIPLE_ITEM) + spPatternData->Size));

    spPatterns->Size = sizeof(KSMULTIPLE_ITEM) + spPatternData->Size;
    spPatterns->Count = 1;
    CopyMemory(spPatterns.m_pData + 1, spPatternData, spPatternData->Size);

    VERIFY_WIN32_BOOL_SUCCEEDED(pFilter->SetPropertySimple(KSPROPSETID_SoundDetector, KSPROPERTY_SOUNDDETECTOR_PATTERNS, spPatterns, spPatterns->Size));
    
    KSEVENT event;
    KSEVENTDATA eventData = { 0 };
    event.Set = KSEVENTSETID_SoundDetector;
    event.Id = KSEVENT_SOUNDDETECTOR_MATCHDETECTED;
    event.Flags = KSEVENT_TYPE_ENABLE;
    eventData.NotificationType = KSEVENTF_EVENT_HANDLE;
    eventData.EventHandle.Event = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (!(eventData.EventHandle.Event))
    {
        VERIFY_WIN32_BOOL_SUCCEEDED(GetLastError());
    }
    
    ULONG bytesReturned;
    VERIFY_WIN32_BOOL_SUCCEEDED(DeviceIoControl(pFilter->m_handle, IOCTL_KS_ENABLE_EVENT, &event, sizeof(event), &eventData, sizeof(eventData), &bytesReturned, nullptr));
    
    VERIFY_WIN32_BOOL_SUCCEEDED(pFilter->SetPropertySimple(KSPROPSETID_SoundDetector, KSPROPERTY_SOUNDDETECTOR_ARMED, &arm, sizeof(arm)));
    
    DWORD waitResult = WaitForSingleObject(eventData.EventHandle.Event, 10000);
    VERIFY_ARE_EQUAL(waitResult, (DWORD)WAIT_OBJECT_0);

    // If this is the Contoso keyword detector, pass CheckPacketSignatures = TRUE
    // to verify that correct packets are available in the WaveRT buffer
    StringCbPrintfW(filename, sizeof(filename), L"Test_KeywordBurstRead#%i.wav", g_pKsPosTst->m_uTestDataIndex);
    return Test_ReadPacketMain(patternType == CONTOSO_KEYWORDCONFIGURATION_IDENTIFIER, filename);
}

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
DWORD
Test_DriftAndJitter
(
    void
)
{
    DWORD           dwRes = FNS_PASS;
    BOOL            fRes = FALSE;
    BOOL            fTestingLoopedDevice = g_pKsPosTst->m_fLoop;    
    CKsFilter      *pFilter = g_pKsPosTst->m_pHalf->m_pFilter;
    bool            fRTPin = g_pKsPosTst->m_pHalf->IsWaveRTPin();
    bool            fIsUSBPin = g_pKsPosTst->m_pHalf->IsUSBPin();

    if (!pFilter)
    {
        SLOG(eError, "Unable to find a CKsFilter for current device");
        return FNS_FAIL;
    }

    if (!g_pKsPosTst->m_pHalf->IsTestSupportedForDeviceType(fTestingLoopedDevice) || g_pKsPosTst->m_pHalf->IsBTHPin())
    {
        XLOG(XSKIP, eError, "Test is not supported for the current device type.");
        return FNS_SKIPPED;
    }

    if (IsPinOffloadCapable(g_pKsPosTst->m_pHalf->m_pPin))
    {
        XLOG(XSKIP, eError, "SKIP: Test is not supported on the offload pin.");
        return FNS_SKIPPED;
    }

#ifdef USB_BUG
    // TEMPORARY FIX: USB has too small a sample played to allow for this test
    // to pass. Sample played must exceed 250 ms which will require
    // special functionality to play multiple buffers.
    if (fIsUSBPin)
    {
        XLOG(XSKIP, eError, "USB devices not supported by this test. ");
        return FNS_SKIPPED;
    }
#endif

    try
    {
        // collect the data ~~~~~~~~~~~~~~~~~
        fRes = CollectSampleData(g_pKsPosTst->m_pHalf, &dwRes, fRTPin, fIsUSBPin);

        if (FNS_SKIPPED == dwRes)
            return FNS_SKIPPED;

        if (!fRes)
            throw("");

        // calculate drift ~~~~~~~~~~~~~~~~~~
        fRes = CalculateSampleRate(g_pKsPosTst->m_pHalf, &dwRes);
        if (!fRes)
            throw("");

        // calculate Jitter ~~~~~~~~~~~~~~~~~
        fRes = CalculateJitter(g_pKsPosTst->m_pHalf, &dwRes);
        if (!fRes)
            throw("");
    }
    // handle bad things
    catch (char* str)
    {
        SLOG(eError, "FAIL: %s", str);
    }

    dwRes = (fRes ? dwRes : FNS_FAIL);
    return dwRes;
}



// ------------------------------------------------------------------------------
double fmax(double d1, double d2)
{
    if (d1 > d2)
        return d1;
    return d2;
}

double fmin(double d1, double d2)
{
    if (d1 < d2)
        return d1;
    return d2;
}

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
DWORD
Test_GetPosition
(
    void
)
{
    DWORD           dwRes = FNS_FAIL;
    BOOL            fRes = FALSE;
    CPCMAudioPin*   pPin = g_pKsPosTst->m_pHalf->m_pPin;
    WAVEFORMATEX*   pwfx = &g_pKsPosTst->m_pHalf->m_pFmtCurrent->wfxFormat.Format;
    BOOL            fTestingLoopedDevice = g_pKsPosTst->m_fLoop;
    CKsFilter*      pFilter = g_pKsPosTst->m_pHalf->m_pFilter;
    bool            fRTPin = g_pKsPosTst->m_pHalf->IsWaveRTPin();

    if(!pFilter)
    {
        SLOG(eError, "Unable to find a CKsFilter for current device");
        return FNS_FAIL;
    }

    if(!g_pKsPosTst->m_pHalf->IsTestSupportedForDeviceType(fTestingLoopedDevice))
    {
        XLOG(XSKIP, eError, "Test is not supported for the current device type.");
        return FNS_SKIPPED;
    }

	try
    {
        KSAUDIO_POSITION pos;
        double           tRunPre=0, tRunPost=0;
        double           tPosPre=0 , tPosPost=0;
        double           tDuration =0, tMax=0, tMin=0, tAve=0;
        int              cLoops=0;
        
        fRes = g_pKsPosTst->m_pHalf->PreparePinConsiderUSB(pwfx, fTestingLoopedDevice, FALSE);
        if (!fRes)
        {
            throw("Failed to prepare pin for testing");
        }

        // submit data to the kmixer sink pin 
        SLOG(eInfo2, "Submitting %d bytes to the pin", pPin->m_cbStreamData);

        if (g_pKsPosTst->m_pHalf->IsWaveRTPacketStreamingSupported())
        {
            XLOG(XSKIP, eError, "The test doesn't support packet based pin.");
            dwRes = FNS_SKIPPED;
            fRes = TRUE;
            goto Exit;
        }

        if (fRTPin) {   // in case of RT port, it does not support IOCTL_KS_WRITE_STREAM and IOCTL_KS_READ_STREAM
            fRes = pPin->SetState(KSSTATE_PAUSE);
        }
        else    {            
            if (g_pKsPosTst->m_pHalf->m_flow == render)
                fRes = pPin->WriteData();
            else
                fRes = pPin->ReadData();
        }


        // mark the time
        tRunPre = xGetTime();

        // run the adapter sink pin
        fRes = pPin->SetState(KSSTATE_RUN);
        if (!fRes)
            throw("Failed to set state on pin to KSSTATE_RUN");

        // mark the time
        tRunPost = xGetTime();
        SLOG(eInfo2, "Setting pin to KSSTATE_RUN took %.03f ms", (tRunPost - tRunPre));

        // loop until we get a non-zero position
        while (fRes)
        {
            // timer before position call
            tPosPre = xGetTime();

            // get position
            fRes = pPin->GetPosition(&pos);

            // timer after position call
            tPosPost = xGetTime();

            tDuration = (tPosPost - tPosPre);

            tMax = fmax(tMax, tDuration);
            tMin = fmin(tMin, tDuration);
            tAve += tDuration;
            cLoops++;
            
            if ((tPosPost - tRunPost) > BUFLEN_MS)
                break;

            if (g_pKsPosTst->m_fLogHistograms)
                SLOG(eInfo2, "Getting KSPROPERTY_AUDIO_POSITION took %.03f ms", tDuration);
        }

        tAve = tAve / (double)cLoops;

        SLOG(eInfo1, "\nStatistics:");
        SLOG(eInfo1, "  Ave = %.03f ms, Max = %.03f ms, Min = %.03f ms", tAve, tMax, tMin);

        if (g_pKsPosTst->m_pHalf->m_pFilter->m_UserDef == FILTERTYPE_SYSAUDIO)
        {
            if (tAve <= GETPOS_THRESHOLD_SYSAUDIO) // ms
            {
                SLOG(eInfo1, "PASS: The average time needed to get KSPROPERTY_AUDIO_POSITION for this format is no more than %.03f ms, and is therefore acceptable", GETPOS_THRESHOLD_SYSAUDIO);
                dwRes = FNS_PASS;
            }
            else
            {
                SLOG(eWarn1, "FAIL: The average time needed to get KSPROPERTY_AUDIO_POSITION for this format ");
                SLOG(eWarn1, "exceeds the acceptable limit of %.03f ms", GETPOS_THRESHOLD_SYSAUDIO);
                SLOG(eWarn1, "!!!!  NOTE THAT THIS FAILURE IS THROUGH SYSAUDIO AND IS THEREFORE LIKELY DUE TO");
                SLOG(eWarn1, "      EXCESSIVE LATENCY INCURRED BY KMIXER");
            }
        }
        else
        {
            if (tAve <= GETPOS_THRESHOLD)         // ms
            {
                SLOG(eInfo1, "PASS: The average time needed to get KSPROPERTY_AUDIO_POSITION for this format is no more than %.03f ms, and is therefore acceptable", GETPOS_THRESHOLD);
                dwRes = FNS_PASS;
            }
            else
            {
                SLOG(eWarn1, "FAIL: The average time needed to get KSPROPERTY_AUDIO_POSITION for this format exceeds the acceptable limit of %.03f ms", GETPOS_THRESHOLD);
            }
        }
    }

    // handle bad things
    catch (char* str)
    {
        SLOG(eError, "FAIL: %s", str);
        dwRes = FNS_FAIL;
    }


Exit:
    // Clean up
    if (pPin)
    {
        pPin->SetState(KSSTATE_STOP);
        pPin->ClosePin();
    }
    dwRes = (fRes ? dwRes : FNS_FAIL);
    return dwRes;
}


// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
DWORD   
Test_SetGetPosition
(
    void
)
{
    DWORD           dwRes = FNS_FAIL;
    BOOL            fRes = FALSE;
    CPCMAudioPin*   pPin = g_pKsPosTst->m_pHalf->m_pPin;
    WAVEFORMATEX*   pwfx = &g_pKsPosTst->m_pHalf->m_pFmtCurrent->wfxFormat.Format;
    BOOL            fTestingLoopedDevice = g_pKsPosTst->m_fLoop;

    double          t1, t2;
    double          dToleranceBy;
    double          dBytesPerMs = (double)pwfx->nAvgBytesPerSec / 1000.;
    ULONG           nDelta;
    double          dDeltaMs;

    try
    {
        KSAUDIO_POSITION posSet;
        KSAUDIO_POSITION posGet;
        double           tRunPre, tRunPost;
        BOOL             fSignaled = FALSE;
        double           tEnd;
        double           dPosEnd;

        bool             fRTPin = g_pKsPosTst->m_pHalf->IsWaveRTPin();

        if ((!fTestingLoopedDevice) && (fRTPin)) {
            SLOG (eInfo1, "Rt port filter does not support standard streaming pin. ");
            return FNS_PASS;            
        }
            
        fRes = g_pKsPosTst->m_pHalf->PreparePin(pwfx, fTestingLoopedDevice);


        if (!fRes)
            throw("");

        dPosEnd = (double)g_pKsPosTst->m_pHalf->m_pPin->m_cbStreamData;

        // submit data to the kmixer sink pin 
        SLOG(eInfo2, "Submitting %d bytes to the pin", pPin->m_cbStreamData);

        if (g_pKsPosTst->m_pHalf->m_flow == render)
            fRes = pPin->WriteData();
        else
            throw("Pin has the wrong dataflow");

        if (!fRes)
            throw("Failed to submit buffer to pin");

        // mark the time
        tRunPre = xGetTime();

        // run the adapter sink pin
        fRes = pPin->SetState(KSSTATE_RUN);
        if (!fRes)
            throw("Failed to set state on pin to KSSTATE_RUN");

        // mark the time
        tRunPost = xGetTime();
        SLOG(eInfo2, "Setting pin to KSSTATE_RUN took %.03f ms", (tRunPost - tRunPre));

        // we want to play with the position for about 5 seconds
        tEnd = tRunPost + 5000.;

        //
        // ~~~~~~~~~~~~~~~~~~~
        //
        posSet.PlayOffset = 0;

        SLOG(eInfo1, "KSPROPERTY_AUDIO_POSITION handler will be called repeatedly, and the values of the play cursor will be compared");
        SLOG(eInfo1, "\"IOCTLs duration\" refers to the delta between 2 time stamps: one taken before the \"Set\" call and one taken after the \"Get\" call.");

        SLOG(eInfo2, "");
        SLOG(eInfo2, "  IOCTLs,      Allowed,   SetPosition,   GetPosition,       Delta,   Delta time,");
        SLOG(eInfo2, "duration,    Tolerance,         Value,         Value,            ,   Equivalent,");
        SLOG(eInfo2, "    (ms),      (bytes),       (bytes),       (bytes),     (bytes),         (ms),\n");

        double cycles = 2.;
        double _2pi = 6.283185307;
        double _2pi_cycles_div_5seconds = cycles * _2pi / 5000.;
        double t = 0.;

        // mess around with the play position for 5 seconds
        do
        {
            // make the play position set a sinusoidal function of time
            t += 200.;

            double s1 = _2pi_cycles_div_5seconds * t;
            double s2 = sin(s1);
            double s = dPosEnd * s2 / 3. + (dPosEnd / 2.);

            posSet.PlayOffset = (ULONGLONG) (s);

            // get the time before and after get/set positions
            t1 = xGetTime();

            fRes = pPin->SetPosition(&posSet);
            if (!fRes)
                throw("    Failure:  Call to KSPROPERTY_AUDIO_POSITION (_SET) failed");

            tRunPre = xGetTime();
            
            fRes = pPin->GetPosition(&posGet);
            if (!fRes)
                throw("    Failure:  Call to KSPROPERTY_AUDIO_POSITION (_GET) failed");
            tRunPost =  xGetTime();

            t2 = xGetTime() - t1;

            // is the position that we got reasonable?
            dToleranceBy = dBytesPerMs * (t2 + POS_ERR_TOLERANCE_MS);
            nDelta = abs((LONG)(posGet.PlayOffset - posSet.PlayOffset));
            dDeltaMs = (double)nDelta / dBytesPerMs;

            SLOG(eInfo2, "%5.2f ms,  %5d bytes,  %6d bytes,  %6d bytes,  %4d bytes,    %6.2f ms,    %.03f ms ,   %.03f ms , %.03f ms ",
                          t2,   (ULONG)dToleranceBy,  (ULONG)posSet.PlayOffset, (ULONG)posGet.PlayOffset,   nDelta, dDeltaMs, tRunPre , tRunPost, (double) (tRunPost-tRunPre));

            if (posSet.PlayOffset > posGet.PlayOffset)
            {
                dwRes = FNS_FAIL;
                SLOG(eError, "    Failure: The value retrieved from KSPROPERTY_AUDIO_POSITION is less than the value set");
            }

            if (nDelta > (ULONG)dToleranceBy)
            {
                dwRes = FNS_FAIL;
                SLOG(eError, "    Failure: The value retrieved from KSPROPERTY_AUDIO_POSITION is not within %d bytes of the value set", (ULONG)dToleranceBy);
            }

            Sleep(200);
        }
        while ((t1 < tEnd) && !fSignaled);
    }

    // handle bad things
    catch (char* str)
    {
        SLOG(eError, "FAIL: %s", str);
        dwRes = FNS_FAIL;
    }

    //
    // clean up
    //
    if (pPin)
    {
        pPin->SetState(KSSTATE_STOP);
        pPin->ClosePin();
    }

    return dwRes;
}

// ------------------------------------------------------------------------------


BOOL CreateSecondInstaceOfPin(CPCMAudioPin* pPin2, BOOL bDefaultSupport, BOOL fLooped)
{
    BOOL           fRes = true;
    const ULONG    BUFFER_TIME_IN_MS = 220;

    fRes = InstantiatePinWfx(pPin2, NULL, fLooped, false, bDefaultSupport);

    if (fRes)   
    {
        // prepare second pin        
        WAVEFORMATEX*               pwfx;
        PKSDATAFORMAT_WAVEFORMATEX  pFormatWfx = (PKSDATAFORMAT_WAVEFORMATEX)pPin2->m_pksDataFormat;
        CComPtr<IBasicLog>          pBasicLog;
        HRESULT                     hr = S_OK;

        pwfx = &pFormatWfx->WaveFormatEx;

        // allocate data
        fRes = pPin2->CreateDataBuffer(BUFFER_TIME_IN_MS, pwfx->nBlockAlign, 16);

        if (!fRes) 
        {
            DWORD dwErr = GetLastError();
            SLOG(eError, "Could not create data buffer for pin - GetLastError() returned %d (0x%08x)", dwErr, dwErr);
            throw("Could not create data buffer for pin");
        }
            
        // make a sine tone
        // get a logger
        pBasicLog = g_pBasicLog;
        
        hr = FillBufferWithSineSignal(
            pBasicLog,
            XFAIL,
            pwfx,
            1.0, // amplitude
            200.0, // frequency
            0.0, // initial phase,
            0.0, // dc
            Method_NoDithering,
            pPin2->m_pbStreamData,
            pPin2->m_cbStreamData / pwfx->nBlockAlign,
            pPin2->m_cbStreamData
        );

        if (S_OK != hr)
        {
            fRes = FALSE;
            throw("Error filling buffer");
        }
    }        

    return fRes;
}

#define LOOPED_THRESHOLD    10000   // BUGBUG - might have to base this of the format?

DWORD
Test_GetPositionMult
(
    void
)
{
    DWORD                   errorCode = ERROR_SUCCESS;
    DWORD                   dwRes = FNS_PASS;
    BOOL                    fRes = FALSE;
    WAVEFORMATEX*           pwfx = &g_pKsPosTst->m_pHalf->m_pFmtCurrent->wfxFormat.Format;
    CPCMAudioPin*           pPin = g_pKsPosTst->m_pHalf->m_pPin;
    BOOL                    fTestingLoopedDevice = g_pKsPosTst->m_fLoop;
    CKsFilter*              pFilter = g_pKsPosTst->m_pHalf->m_pFilter; 
    CPCMAudioPin            *pPin2 = ((CPCMAudioFilter*)pFilter)->BuildPin(pPin->m_nId);
    CComPtr<IBasicLog>      pBasicLog;
    BOOL                    fLooped = g_pKsPosTst->m_fLoop;
    const ULONG             BUFFER_TIME_IN_MS = 220; // USB and non-USB device are allocating the same buffer size. 
                                                     // Using 220 ms because we always allocate this size for USB in the test
                                                     // since long time ago in the whole test. 

    if( ERROR_SUCCESS != errorCode )
    {
        SLOG( eError, "Creating a copy of pin failed.  (error=%u)", errorCode );
        return FNS_FAIL;
    }

    if(!pFilter)
    {
        SLOG(eError, "Unable to find a CKsFilter for current device");
        return FNS_FAIL;
    }

    if(!g_pKsPosTst->m_pHalf->IsTestSupportedForDeviceType(fTestingLoopedDevice))
    {
        XLOG(XSKIP, eError, "Test is not supported for the current device type.");
        return FNS_SKIPPED;
    }

    try
    {
        double           tRunPre = 0, tRunPost = 0;
        KSAUDIO_POSITION pos  = {0, 0}, prev  = {0, 0};
        KSAUDIO_POSITION pos2 = {0, 0}, prev2 = {0, 0};
        BOOL bRawSupport=0, bDefaultSupport=0;

       fRes= pPin->CheckProcessingModes(&bRawSupport, &bDefaultSupport);
       SLOG(eInfo1, "Going to prepare pin. Pin mode support:%s, %s", bRawSupport == TRUE? "RAW  MODE" : "NO RAW" , bDefaultSupport == TRUE ? "DEFAULT MODE" : "NO DEFAULT");      

       if (!fRes)
       {
           //CheckProcessing modes did not succeed; Ignore and just instantiate pins- We don't have modes supported info
           //But that is ok
           fRes = TRUE;
           XLOG(XMSG,eInfo1,"CheckProcessing Modes failed");
       }

       // In this test, we will create two data buffers on the same pin with 220 ms size.
       // Once we got two buffers, we set both pins to RUN state to make sure the pins move in 2 seconds time duration.
        XLOG(XMSG,eInfo1, "Going to prepare pin");
        fRes = g_pKsPosTst->m_pHalf->PreparePin(pwfx, fTestingLoopedDevice, BUFFER_TIME_IN_MS, bRawSupport);
        if (!fRes)
        {
            throw("Failed to prepare pin for testing");
        }

        if (g_pKsPosTst->m_pHalf->IsWaveRTPacketStreamingSupported())
        {
            XLOG(XSKIP, eError, "The test doesn't support packet based pin.");
            fRes = TRUE;
            dwRes = FNS_SKIPPED;
            goto EXIT;
        }

        ULONG   cPossible = pPin->m_Descriptor.CInstancesGlobal.PossibleCount;
        BOOL    fExpectDupSuccess = (cPossible > 1);

        SLOG(eInfo1, "Given the possible instance count (%d) of the render pin, a 2nd attempt to instantiate the pin should %s", 
            cPossible, fExpectDupSuccess ? "SUCCEED" : "FAIL");

        if (!fExpectDupSuccess) 
        { 
            SLOG (eInfo1,"Second instance of the pin is not possible. So exiting test case");
            dwRes = FNS_PASS;
            goto EXIT;

        }

        // create second pin

        // TODO: for RT port this will be different as we have to send property request to have buffer for the pin
                
        fRes = CreateSecondInstaceOfPin(pPin2, bDefaultSupport, fLooped);
        if (!fRes)
        {
            throw("Failed to create second instance of pin");
        }

        if (fRes)
        {
            // submit data to the pins
            SLOG(eInfo2, "Submitting %d bytes to the pins", pPin->m_cbStreamData);

            fRes = pPin->WriteData();
            if (!fRes)
            {
                throw("Failed to submit buffer to first pin");
            }

            fRes = pPin2->WriteData();
            if (!fRes)
            {
                throw("Failed to submit buffer to second pin");
            }

            // run the pins
            fRes = pPin->SetState(KSSTATE_RUN);
            if (!fRes)
            {
                throw("Failed to set state on first pin to KSSTATE_RUN");
            }

            fRes = pPin2->SetState(KSSTATE_RUN);
            if (!fRes)
            {
                throw("Failed to set state on second pin to KSSTATE_RUN");
            }

            // mark the time
            tRunPost = tRunPre = xGetTime();

            // loop until buffer is completely played
            while (fRes && (BUFLEN_MS >= (tRunPost - tRunPre)))
            {
                // get position
                fRes = pPin->GetPosition(&pos);
                if (!fRes)
                {
                    throw("Failed to get first buffer's position!");
                }

                fRes = pPin2->GetPosition(&pos2);
                if (!fRes)
                {
                    throw("Failed to get second buffer's position!");
                }

                // mark the time
                tRunPost = xGetTime();

                // compare prev and pos - depending on looped need to have threshold???
                if ((pos.PlayOffset < prev.PlayOffset) &&
                    (!fTestingLoopedDevice || ((prev.PlayOffset - pos.PlayOffset) < LOOPED_THRESHOLD)))
                {
                    fRes = FALSE;
                    SLOG(eError, "FAIL: First buffer's play position went backwards!");
                }
                if ((pos.PlayOffset > pos.WriteOffset) && 
                    (!fTestingLoopedDevice || ((pos.PlayOffset - pos.WriteOffset) < LOOPED_THRESHOLD)))
                {
                    fRes = FALSE;
                    SLOG(eError, "FAIL: First buffer's position overran the write position");
                }
                if ((pos.WriteOffset < prev.WriteOffset) && 
                    (!fTestingLoopedDevice || ((prev.WriteOffset - pos.WriteOffset) < LOOPED_THRESHOLD)))
                {
                    fRes = FALSE;
                    SLOG(eError, "FAIL: First buffer's write position went backwards!");
                }

                if ((pos2.PlayOffset < prev2.PlayOffset) && 
                    (!fTestingLoopedDevice || ((prev2.PlayOffset - pos2.PlayOffset) < LOOPED_THRESHOLD)))
                {
                    fRes = FALSE;
                    SLOG(eError, "FAIL: Second buffer's play position went backwards!");
                }
                if ((pos2.PlayOffset > pos2.WriteOffset) && 
                    (!fTestingLoopedDevice || ((pos2.PlayOffset - pos2.WriteOffset) < LOOPED_THRESHOLD)))
                {
                    fRes = FALSE;
                    SLOG(eError, "FAIL: Second buffer's position overran the write position");
                }
                if ((pos2.WriteOffset < prev2.WriteOffset) && 
                    (!fTestingLoopedDevice || ((prev2.WriteOffset - pos2.WriteOffset) < LOOPED_THRESHOLD)))
                {
                    fRes = FALSE;
                    SLOG(eError, "FAIL: Second buffer's write position went backwards!");
                }

                if (!fRes)
                {
                    SLOG(eInfo1, "  pos.PlayOffset == %I64d", pos.PlayOffset);
                    SLOG(eInfo1, "  pos.WriteOffset == %I64d", pos.WriteOffset);
                    SLOG(eInfo1, "  prev.PlayOffset == %I64d", prev.PlayOffset);
                    SLOG(eInfo1, "  prev.WriteOffset == %I64d", prev.WriteOffset);
                    SLOG(eInfo1, "  pos2.PlayOffset == %I64d", pos2.PlayOffset);
                    SLOG(eInfo1, "  pos2.WriteOffset == %I64d", pos2.WriteOffset);
                    SLOG(eInfo1, "  prev2.PlayOffset == %I64d", prev2.PlayOffset);
                    SLOG(eInfo1, "  prev2.WriteOffset == %I64d", prev2.WriteOffset);
                }

                prev = pos;
                prev2 = pos2;
            }
        }
        else
        {
            SLOG(eInfo1, "Failed to prepare second pin.  This is OK on non-hwaccel devices");
            dwRes = FNS_SKIPPED;
            fRes = TRUE;
        }
    }

    // handle bad things
    catch (char* str)
    {
        SLOG(eError, "FAIL: %s", str);
        dwRes = FNS_FAIL;
    }

    //
    // clean up
    //

    pPin->SetState(KSSTATE_STOP); 
    pPin2->SetState(KSSTATE_STOP);


EXIT:
    if (pPin)
    {
        pPin->ClosePin();
    }
    if (pPin2 != NULL)
    {        
        pPin2->ClosePin();
        delete pPin2;
        pPin2 = NULL;
    }
    dwRes = (fRes ? dwRes : FNS_FAIL);
    return dwRes;
}

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
DWORD   
Test_WritePacket
(
    void
)
{
    CKsFilter*                      pFilter = g_pKsPosTst->m_pHalf->m_pFilter;
    CPCMAudioPin*                   pPin = g_pKsPosTst->m_pHalf->m_pPin;
    WAVEFORMATEX*                   pwfx = &g_pKsPosTst->m_pHalf->m_pFmtCurrent->wfxFormat.Format;
    BOOL                            fTestingLoopedDevice = g_pKsPosTst->m_fLoop;
    HANDLE                          bufferEvent = NULL;
    LARGE_INTEGER                   qpcBaseline;
    LARGE_INTEGER                   qpcFrequency;
    LARGE_INTEGER                   qpcLastEvent;
    ULONG                           packetSize;
    const ULONG                     nNotifications = 2;
    KSRTAUDIO_SETWRITEPACKET_INFO   wpi = {0};
    KSAUDIO_PRESENTATION_POSITION   pos;
    ULONG                           tPacketCount;
    const ULONG                     BUFFER_TIME = 20;
    ULONG                           PacketTime = BUFFER_TIME / nNotifications;
    LONGLONG                        PacketTimeInQpc;
    HANDLE                          hAvRtMmcssService = NULL;
    DWORD                           dwMmcssTaskIndex = 0;
    
    // This structure contains all interesting data gathered from one test loop iteration
    typedef struct
    {
        LARGE_INTEGER                   qpcBefore;
        LARGE_INTEGER                   qpcAfter;
        KSRTAUDIO_SETWRITEPACKET_INFO   WritePacketInfo;
        DWORD                           BufferEventWaitResult;
        KSAUDIO_PRESENTATION_POSITION   PresentationPosition;
        ULONG                           PacketCount;
    } DATAPOINT;

    DATAPOINT data[DATA_POINTS];    // Collection of all data gathered from the main test loop

    if (!pFilter)
    {
        SLOG(eError, "Unable to find a CKsFilter for current device");
        return FNS_FAIL;
    }
    
    if (!g_pKsPosTst->m_pHalf->IsWaveRTPin())
    {
        XLOG(XSKIP, eError, "Test is not supported for the current device type. Not a WaveRT pin.");
        return FNS_SKIPPED;
    }

    if (!g_pKsPosTst->m_pHalf->IsTestSupportedForDeviceType(fTestingLoopedDevice))
    {
        XLOG(XSKIP, eError, "Test is not supported for the current device type.");
        return FNS_SKIPPED;
    }

    VERIFY_WIN32_BOOL_SUCCEEDED(g_pKsPosTst->m_pHalf->PreparePin(pwfx, fTestingLoopedDevice, BUFFER_TIME, FALSE, nNotifications));
    
    if (!g_pKsPosTst->m_pHalf->m_fIsWaveRTPinWithWritePacketSupport)
    {
        XLOG(XSKIP, eError, "Test is not supported for the current device type - KSPROPERTY_RTAUDIO_SETWRITEPACKET not supported.");
        return FNS_SKIPPED;
    }
    
    // Register buffer notification event
    KSRTAUDIO_NOTIFICATION_EVENT_PROPERTY notificationEventProperty;
    bufferEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    notificationEventProperty.NotificationEvent = bufferEvent;
    VERIFY_WIN32_BOOL_SUCCEEDED(pPin->SetPropertySimple(KSPROPSETID_RtAudio, KSPROPERTY_RTAUDIO_REGISTER_NOTIFICATION_EVENT, NULL, 0, &notificationEventProperty.NotificationEvent, sizeof(notificationEventProperty) - sizeof(KSPROPERTY)));

    // The packet size
    packetSize = pPin->m_cbStreamData / nNotifications;
    
    // Verify GetPacketCount returns 0 at stopped
    // Verify GetOutputStreamPresentationPosition return 0 at stopped
    VERIFY_WIN32_BOOL_SUCCEEDED(pPin->GetPropertySimple(KSPROPSETID_RtAudio, KSPROPERTY_RTAUDIO_PRESENTATION_POSITION, &pos, sizeof(pos)));
    VERIFY_WIN32_BOOL_SUCCEEDED(pPin->GetPropertySimple(KSPROPSETID_RtAudio, KSPROPERTY_RTAUDIO_PACKETCOUNT, &tPacketCount, sizeof(tPacketCount)));
    VERIFY_ARE_EQUAL(0U, tPacketCount);
    VERIFY_ARE_EQUAL(0U, pos.u64PositionInBlocks);
    
    VERIFY_WIN32_BOOL_SUCCEEDED(pPin->SetState(KSSTATE_PAUSE));
    
    // Snap a reference time, just to make displayed times smaller and more human readable.
    QueryPerformanceFrequency(&qpcFrequency);
    QueryPerformanceCounter(&qpcBaseline);
    
    // Packet interval, in terms of performance counter ticks
    PacketTimeInQpc = PacketTime * qpcFrequency.QuadPart / 1000;

    // Verify correct error for out of bounds SetWritePacket call
    wpi.PacketNumber = nNotifications;
    VERIFY_WIN32_BOOL_FAILED(pPin->SetPropertySimple(KSPROPSETID_RtAudio, KSPROPERTY_RTAUDIO_SETWRITEPACKET, &wpi, sizeof(wpi)));
    VERIFY_ARE_EQUAL((DWORD)ERROR_IO_DEVICE, GetLastError());
    
    // Write a packet before going into a loop
    wpi.PacketNumber = 0;
    VERIFY_WIN32_BOOL_SUCCEEDED(pPin->SetPropertySimple(KSPROPSETID_RtAudio, KSPROPERTY_RTAUDIO_SETWRITEPACKET, &wpi, sizeof(wpi)));
    
    // Higher Thread priority
    hAvRtMmcssService = AvSetMmThreadCharacteristics( _T("Audio"), &dwMmcssTaskIndex );
    if (NULL != hAvRtMmcssService)
    {
        VERIFY_WIN32_BOOL_SUCCEEDED(AvSetMmThreadPriority( hAvRtMmcssService, AVRT_PRIORITY_HIGH ));
    }
    
    VERIFY_WIN32_BOOL_SUCCEEDED(pPin->SetState(KSSTATE_RUN));

    // Time of transition to RUN.
    QueryPerformanceCounter(&qpcLastEvent);
    
    // SetWritePacket for DATA_POINTS loop. Record every GetPacketCount and GetOutputStream results(Last Datapoint with the EOS flag)
    for(int i = 0; (i < DATA_POINTS); i++)
    {
        DATAPOINT *dp = &data[i];
        ULONG packetNumber = i + 1;     // Starting with packet 1, because packet 0 was written before this loop.

        dp->WritePacketInfo = { 0 };

        dp->WritePacketInfo.PacketNumber = packetNumber;
        
        // On last packet, set EOS
        if (i == DATA_POINTS - 1)
        {
            // Define Flags
            dp->WritePacketInfo.Flags = KSSTREAM_HEADER_OPTIONSF_ENDOFSTREAM;
            
            // Verify correct error for calling SetWritePacket with invalid EoSPacketLength parameter.
            dp->WritePacketInfo.EosPacketLength = packetSize + 1;
            VERIFY_WIN32_BOOL_FAILED(pPin->SetPropertySimple(KSPROPSETID_RtAudio, KSPROPERTY_RTAUDIO_SETWRITEPACKET, &dp->WritePacketInfo, sizeof(dp->WritePacketInfo)));
            VERIFY_ARE_EQUAL((DWORD)ERROR_INVALID_PARAMETER, GetLastError());
            
            // In-bound EoSPacketLength
            dp->WritePacketInfo.EosPacketLength = packetSize / 2;
        }
        
        VERIFY_WIN32_BOOL_SUCCEEDED(pPin->SetPropertySimple(KSPROPSETID_RtAudio, KSPROPERTY_RTAUDIO_SETWRITEPACKET, &dp->WritePacketInfo, sizeof(dp->WritePacketInfo)));
        
        QueryPerformanceCounter(&dp->qpcBefore);
    
        // 150% tolerance for first buffer event after RUN, 50% tolerance between each buffer event after that
        LONGLONG maxTimeToNextEventInQpc = (i == 0) ? (PacketTimeInQpc * 5 / 2) : (PacketTimeInQpc * 3 / 2);

        // Wait for the buffer event (for up to 5 seconds- much longer than needed)
        dp->BufferEventWaitResult = WaitForSingleObject(bufferEvent, 5000);
                
        // Snap current time to compare with the qpc from KSPROPERTY_RTAUDIO_PRESENTATION_POSITION call
        QueryPerformanceCounter(&dp->qpcAfter);
        
        // Confirm the event occured within the required time
        VERIFY_IS_LESS_THAN(dp->qpcAfter.QuadPart - qpcLastEvent.QuadPart, maxTimeToNextEventInQpc);

        // Remember when this last buffer event occured in order to confirm the next event occurs soon enough
        qpcLastEvent = dp->qpcAfter;

        // get position and packet count, just for logging purposes for now
        VERIFY_WIN32_BOOL_SUCCEEDED(pPin->GetPropertySimple(KSPROPSETID_RtAudio, KSPROPERTY_RTAUDIO_PRESENTATION_POSITION, &dp->PresentationPosition, sizeof(dp->PresentationPosition)));
        VERIFY_WIN32_BOOL_SUCCEEDED(pPin->GetPropertySimple(KSPROPSETID_RtAudio, KSPROPERTY_RTAUDIO_PACKETCOUNT, &dp->PacketCount, sizeof(dp->PacketCount)));
    }

    // View the collected data
    for (int i = 0; (i < DATA_POINTS); i++)
    {
        DATAPOINT *dp = &data[i];

        SLOG(eInfo1, "pp=%6d | dp=%6d | qt=%6d | wtb=%6d | wta=%6d | p=%3d | c=%3d | e=%1d | s=%5d",
            (int)(dp->PresentationPosition.u64PositionInBlocks * pwfx->nBlockAlign),                    // Presentation position in bytes
            (int)(dp->PacketCount * packetSize),                                                        // Derived presentation position from packet count
            (int)QpcToRelativeMs(dp->PresentationPosition.u64QPCPosition, qpcBaseline, qpcFrequency),   // Query timestamp
            (int)QpcToRelativeMs(dp->qpcBefore.QuadPart, qpcBaseline, qpcFrequency),                    // Before wait timestamp
            (int)QpcToRelativeMs(dp->qpcAfter.QuadPart, qpcBaseline, qpcFrequency),                     // After wait timestamp
            (int)dp->WritePacketInfo.PacketNumber,                                                      // Last set PacketNumber
            (int)dp->PacketCount,                                                                       // Packet count
            (int)(dp->BufferEventWaitResult == WAIT_OBJECT_0),                                          // Waitresult
            (int)packetSize                                                                             // Packet Size
            );
    }
    
    // Verify correct error for calling SetWritePacket after EOS was set
    wpi.PacketNumber = data[DATA_POINTS - 1].WritePacketInfo.PacketNumber + 1;      // Packet after the last one written in the loop above
    VERIFY_WIN32_BOOL_FAILED(pPin->SetPropertySimple(KSPROPSETID_RtAudio, KSPROPERTY_RTAUDIO_SETWRITEPACKET, &wpi, sizeof(wpi)));
    VERIFY_ARE_EQUAL((DWORD)ERROR_BAD_COMMAND, GetLastError());
    
    // Clean up
    // Unregister buffer notification
    VERIFY_WIN32_BOOL_SUCCEEDED(pPin->SetPropertySimple(KSPROPSETID_RtAudio, KSPROPERTY_RTAUDIO_UNREGISTER_NOTIFICATION_EVENT, NULL, 0, &notificationEventProperty.NotificationEvent, sizeof(notificationEventProperty) - sizeof(KSPROPERTY)));
    
    // Normalize thread priority
    if (NULL != hAvRtMmcssService)
    {
        VERIFY_WIN32_BOOL_SUCCEEDED(AvRevertMmThreadCharacteristics( hAvRtMmcssService ));
        hAvRtMmcssService = NULL;
    }
    
    // Close pin
    if (pPin)
    {
        pPin->SetState(KSSTATE_STOP);
        pPin->ClosePin();
    }
    
    return FNS_PASS;
}


// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
// 
#define STREAMING_SECONDS_AEC 81
#define MAX_TS_POS_PAIRS (STREAMING_SECONDS_AEC * 100) // we sleep 10ms every time.

typedef struct _stLinearRegression
{
    int bReady;

    int iNominalSamplRate;

    UINT64  iFirstSamplePos;
    __int64 iFirstTimestamp;
    UINT64  iPrevSamplePos;
    __int64 iPrevTimestamp;

    int     N;
    UINT64 iTimeLength; 
    __int64 rgiSamplePos[MAX_TS_POS_PAIRS];
    __int64 rgiTimestamp[MAX_TS_POS_PAIRS];

    int    NBest;
    UINT64 iTimeLengthBest; 
    __int64 rgiSamplePosBest[MAX_TS_POS_PAIRS];
    __int64 rgiTimestampBest[MAX_TS_POS_PAIRS];

    double fSlope;
    double fIntercept;
    double fMSError;
    double fAvgErrorMag;
    int    iHistgram[6]; //histgram of noise in 0-0.05-0.1-0.2-0.4-0.8-1.6ms

    int    iIndexOf2Sec;
    double fAvgErrorPer2Sec[STREAMING_SECONDS_AEC/2+5]; 
    double fMaxAvgErrorPer2Sec;
    int    iPointsPer2Sec[STREAMING_SECONDS_AEC/2+5]; 
    int    iTtlGlitches;
}LRModel;

void LRModelInit(LRModel *pLR, int iNominalSamplRate)
{
    memset(pLR, 0, sizeof(LRModel));
    pLR->iNominalSamplRate = iNominalSamplRate;
    pLR->bReady = 0;
}

void LRModelResetAfterGlitch(LRModel *pLR)
{
    pLR->N = 0;
    pLR->iFirstSamplePos = 0;
    pLR->iFirstTimestamp = 0;
    pLR->iPrevSamplePos = 0;
    pLR->iPrevTimestamp = 0;

    pLR->bReady = 0;
    pLR->iTimeLength = 0; 

    pLR->iTtlGlitches++;
    if (pLR->iTimeLength > pLR->iTimeLengthBest)
    {
        pLR->NBest = pLR->N;
        pLR->iTimeLengthBest = pLR->iTimeLength;
        memcpy(pLR->rgiSamplePosBest, pLR->rgiSamplePos, sizeof(__int64)*pLR->NBest);
        memcpy(pLR->rgiTimestampBest, pLR->rgiTimestamp, sizeof(__int64)*pLR->NBest);
    }
}

void LRModelAddDataPair(LRModel  *pLR, UINT64 iSamplePos, __int64 iTimestamp)
{
    static iCount = 0;
    __int64 iAdjustedTimestamp;
    __int64 iAdjustedSamplePos;

    // skip first 100ms
    if (iCount < 10)
    {
        iCount++;
        return;
    }

    if (pLR->N >= MAX_TS_POS_PAIRS)
    {
        return;
    }

    // don't use more than 100s data to avoid computation overflow.
    if (pLR->iTimeLength >= 10000000 * 100)  
    {
        return;
    }

    //SLOG(eInfo2, "%I64d %I64d", iTimestamp, iSamplePos);

    if (0 == pLR->bReady)
    {
        pLR->bReady = 1;
        pLR->iFirstTimestamp = iTimestamp;
        pLR->iFirstSamplePos = iSamplePos;
    }
    else
    {
        __int64 iInstNoise = (pLR->iPrevSamplePos + (iTimestamp - pLR->iPrevTimestamp)* pLR->iNominalSamplRate / 10000000) - iSamplePos;
        if (iInstNoise > pLR->iNominalSamplRate*5/1000 || iInstNoise < -pLR->iNominalSamplRate*5/1000)
        {   // if off by > 5ms we consider glitch happens
            SLOG(eInfo2,
                "Detected a glitch at %I64d; position advanced %I64d samples in %I64d hns",
                iTimestamp + pLR->iFirstTimestamp,
                iSamplePos - pLR->iPrevSamplePos,
                iTimestamp - pLR->iPrevTimestamp
            );
            LRModelResetAfterGlitch(pLR);            
            return;
        }
    }

    pLR->iPrevSamplePos = iSamplePos;
    pLR->iPrevTimestamp = iTimestamp;

    iAdjustedTimestamp = iTimestamp - pLR->iFirstTimestamp;
    iAdjustedSamplePos = iSamplePos - pLR->iFirstSamplePos - iAdjustedTimestamp * pLR->iNominalSamplRate / 10000000;

    pLR->rgiSamplePos[pLR->N] = iAdjustedSamplePos;
    pLR->rgiTimestamp[pLR->N] = iAdjustedTimestamp;

    pLR->N = pLR->N + 1;

    pLR->iTimeLength = iAdjustedTimestamp;

}

void LRModelGetResult(LRModel *pLR)
{   
    int i;
    int N;
    double  Sx = 0.0;
    double  Sy = 0.0;
    double  Sxx = 0.0;
    double  Syy = 0.0;
    double  Sxy = 0.0;    

    if (pLR->iTimeLength > pLR->iTimeLengthBest)
    {
        pLR->NBest = pLR->N;
        pLR->iTimeLengthBest = pLR->iTimeLength;
        memcpy(pLR->rgiSamplePosBest, pLR->rgiSamplePos, sizeof(__int64)*pLR->NBest);
        memcpy(pLR->rgiTimestampBest, pLR->rgiTimestamp, sizeof(__int64)*pLR->NBest);
    }

    N = pLR->NBest;

    bool bVerbose = false;
    WEX::TestExecution::RuntimeParameters::TryGetValue<bool>(L"Verbose", bVerbose);

    for (i = 0; i < N; i++)
    {
        __int64 iAdjustedSamplePos = pLR->rgiSamplePosBest[i];
        __int64 iAdjustedTimestamp = pLR->rgiTimestampBest[i];

        if (bVerbose)
        {
            //Logging this for all samples causes the log to get too big, causing crashes in WTT.
            SLOG(10, "SamplePosition[%d] = %I64d", i, iAdjustedSamplePos);
            SLOG(10, "Timestamp[%d] = %I64d", i, iAdjustedTimestamp);
        }

        Sx = Sx + iAdjustedTimestamp;
        Sy = Sy + iAdjustedSamplePos;
        Sxx = Sxx + iAdjustedTimestamp * iAdjustedTimestamp;
        Syy = Syy + iAdjustedSamplePos  * iAdjustedSamplePos;
        Sxy = Sxy + iAdjustedTimestamp * iAdjustedSamplePos;   
    }

    pLR->fSlope = (N * Sxy - Sx * Sy) / (N * Sxx - Sx * Sx);
    pLR->fIntercept = (Sy - pLR->fSlope * Sx) / N;
    pLR->fMSError = (Syy + (pLR->fIntercept*pLR->fIntercept*N) + (pLR->fSlope * pLR->fSlope) * Sxx 
        - 2 * pLR->fIntercept * Sy + 2 * pLR->fIntercept * pLR->fSlope * Sx - 2 * pLR->fSlope * Sxy);
    pLR->fMSError = pLR->fMSError / N;

    pLR->iIndexOf2Sec = 0;
    for (i = 0; i < N; i++)
    {
        double fError = 0.0f;
        fError = pLR->rgiSamplePosBest[i] - (pLR->fIntercept + pLR->fSlope * pLR->rgiTimestampBest[i]);
        //SLOG(eInfo2, "%I64d %I64d", pLR->rgiTimestampBest[i], pLR->rgiSamplePosBest[i]);
        pLR->iIndexOf2Sec = (int)(pLR->rgiTimestampBest[i] / 10000000) / 2;
        if (pLR->iIndexOf2Sec < 0)
        {
            SLOG(eError, "FAIL: Timestamp run backward");
            pLR->NBest = 0;
            return;
        }
        if (pLR->iIndexOf2Sec >= STREAMING_SECONDS_AEC/2 + 5)
        {
            SLOG(eError, "FAIL: Timestamp run too fast");
            pLR->NBest = 0;
            return;
        }
        pLR->fAvgErrorPer2Sec[pLR->iIndexOf2Sec] += fError; 
        pLR->iPointsPer2Sec[pLR->iIndexOf2Sec]++; 

        fError = (fError >=  0 ? fError : -fError);
        pLR->fAvgErrorMag += fError;
        
        fError = (fError / pLR->iNominalSamplRate) * 1000;        
        if (fError > 0.8f)
            pLR->iHistgram[5]++;
        else if (fError > 0.4f)
            pLR->iHistgram[4]++;
        else if (fError > 0.2f)
            pLR->iHistgram[3]++;
        else if (fError > 0.1f)
            pLR->iHistgram[2]++;
        else if (fError > 0.05f)
            pLR->iHistgram[1]++;
        else
            pLR->iHistgram[0]++;
    }
    pLR->fAvgErrorMag = pLR->fAvgErrorMag / N;

    if (pLR->iIndexOf2Sec > 2)
    {
        pLR->iIndexOf2Sec--;
        for (i = 0; i < pLR->iIndexOf2Sec; i++)
        {
            pLR->fAvgErrorPer2Sec[i] /= pLR->iPointsPer2Sec[i];
            // convert to ms.
            pLR->fAvgErrorPer2Sec[i] /= 10000;
            if (fabs(pLR->fAvgErrorPer2Sec[i]) > pLR->fMaxAvgErrorPer2Sec)
            {      
                pLR->fMaxAvgErrorPer2Sec = fabs(pLR->fAvgErrorPer2Sec[i]);
            }
        }
    }
    else
    {
        SLOG(eError, "FAIL: Don't have enough captured samples");
        pLR->NBest = 0;
        return;
    }

    // convert
    pLR->fSlope = pLR->fSlope * 10000000 + pLR->iNominalSamplRate; // sampling rate
    pLR->fIntercept += pLR->iFirstTimestamp;
    pLR->fIntercept /= 10000; //convert to ms
    pLR->fMSError = pLR->fMSError * 1000 * 1000 / pLR->iNominalSamplRate / pLR->iNominalSamplRate; // convert to ms
    pLR->fAvgErrorMag = pLR->fAvgErrorMag / pLR->iNominalSamplRate * 1000; //convert to ms
}

BOOL CollectSampleData_ForAEC
(
    CHalfApp       *pHalf,
    WAVEFORMATEX   *pwfx,
    bool            fRTPin,
    LRModel        *pLRModel,
    DWORD          *dwResult
)
{
    BOOL            fRes = FALSE;
    PFORMAT_ENTRY   pFormatEntry = pHalf->m_pFmtCurrent;
    CPCMAudioPin*   pPin = pHalf->m_pPin;
    BOOL            fTestingLoopedDevice = g_pKsPosTst->m_fLoop;

    *dwResult = FNS_FAIL;

    SLOG(eInfo2, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    SLOG(eInfo2, "Building time vs. position histogram.  Please stand by...");

    try
    {
        KSAUDIO_POSITION pos;
        double           tStart, tPre, tPost;
        UINT             i;
        bool             fState = true;

        if(fRTPin)
        {
            fRes = g_pKsPosTst->m_pHalf->PreparePin(pwfx, fTestingLoopedDevice);
        }
        else
        {
            fRes = g_pKsPosTst->m_pHalf->PreparePin(pwfx, fTestingLoopedDevice, (STREAMING_SECONDS_AEC+10)*1000);
        }

        if (!fRes)
        {
            throw("PreparePin failed");
        }
        SLOG(eInfo2, "Submitting %d bytes to the pin", pPin->m_cbStreamData);

        if (g_pKsPosTst->m_pHalf->IsWaveRTPacketStreamingSupported())
        {
            *dwResult = FNS_SKIPPED;
            throw("The test doesn't support packet based pin.");
        }

        // Set state to pause and stream if non-RT pin
        fRes = pPin->SetState(KSSTATE_PAUSE);
        if (!fRes)
        {
            throw("Failed to set pin to pause state.");
        }

        CKsStreamRender KsRend(fState, pPin);
        if(!fState) { throw("Failed to create a CKsStreamRender object."); }
        CKsStreamCapture KsCap(fState, pPin);
        if(!fState) { throw("Failed to create a CKsStreamCapture object."); }

        if(!fRTPin)
        {   
            if(render == g_pKsPosTst->m_pHalf->m_flow)
            {   
                if(!KsRend.Play(pwfx))
                {
                    throw("USB rendering failed.");
                }
            }
            else
            {
                fRes = pPin->SetState(KSSTATE_RUN);
                if (!fRes)
                {
                    throw( "Failed to set pin to run state.");
                }

                if(!KsCap.Capture(pwfx))
                {
                    throw("USB capture failed.");
                }
            }
        }

        // run the adapter sink pin
        fRes = pPin->SetState(KSSTATE_RUN);
        if (!fRes)
        {         
            throw("Failed to set state on Renderer sink pin to KSSTATE_RUN");
        }

        tStart = xGetTime();

        // make sure the position-fetching code is all paged in by exercising that code path
        pPin->GetPosition(&pos);

        Sleep(40);
        int nLoop = 0;       // to keep track of which loop is streaming - first time, second time etc... At every end of buffer we will increament it by one
        double posDiff = 0;    // position difference between current and previous position 

        __int64 iBytePos = 0;
        __int64 iSamplePos = 0;
        __int64 iTimestamp = 0;
        __int64 iBytePosPrev = 0;
        __int64 iSamplePosPrev = 0;

        // Try to stream for 60 seconds.
        for (i = 0; i < STREAMING_SECONDS_AEC * 100; )
        {
            Sleep(10);

            tPre = xGetTime();

            fRes = pPin->GetPosition(&pos);

            if (!fRes) 
            {
                throw("GetPosition failed");
            }
            tPost = xGetTime();

            if (pos.PlayOffset > 0)
            {
                // only count this result if the GetPosition and GetTime happened relatively atomically
                if ((tPost - tPre) < 1.)
                {
                    if (i) 
                    {    
                        // Detect loop. if we are again at the begining of buffer after finishing 
                        // buffer data then increment nLoop and add buffer lenght into postion.
                        // Here we are assuming that difference between 2 successive position will 
                        // not be more then half of buffer length
                        posDiff = abs( (double)((ULONG)pos.PlayOffset) - 
                                        (iBytePosPrev -((double)(pPin->m_cbStreamData)*nLoop)));
                        if ((posDiff) >(double) ((pPin->m_cbStreamData)/2))    
                        {
                            nLoop++;
                            SLOG(eInfo2, "running %d loop", nLoop+1);
                        }
                        
                    }

                    // convert ms to 100 nanosec.
                    iTimestamp = (__int64)(((tPost + tPre) / 2 - tStart) * 10000);
                    iBytePos = (__int64)(pos.PlayOffset + ((__int64)(pPin->m_cbStreamData)*nLoop));        // for bug 788024
                    iSamplePos = iBytePos / pwfx->nBlockAlign;

                    LRModelAddDataPair(pLRModel, iSamplePos, iTimestamp);

                    // detect if we have stopped moving.  If so, we are done collecting data
                    if 
                    (
                        (i > 0) && 
                        (iSamplePos == iSamplePosPrev)
                    )
                    {
                        // try to detect false alarms
                        if (abs(tPost - tStart - STREAMING_SECONDS_AEC*1000) < 500.)
                        {
                            // decrement the count by one, since the last two values are 
                            //  identical and we don't want to artificially weight the end of the buffer
                            SLOG(eBlab1, "Position stopped moving");
                            break;
                        }
                        else
                        {
                            SLOG(eInfo2, "Warning:  Position appears to be stalled at t = %d ms, pos = %I64d bytes", 
                                (ULONG)iTimestamp / 10000, iSamplePos);
                        }
                    }

                    iBytePosPrev = iBytePos;
                    iSamplePosPrev = iSamplePos;
                }
                else
                {
                    SLOG(eBlab2, "GetPosition function too slow");
                }

            }
            else
            {
                if ( (fRTPin && 0 == nLoop) && ((tPost - tStart) > 1000) )
                {
                    throw("Position didn't move within 1 second");
                }
            }

            if (iSamplePos >= STREAMING_SECONDS_AEC * pwfx->nSamplesPerSec ||
                tPost - tStart > STREAMING_SECONDS_AEC * 1000)
            {
                // Stop after streaming 60 seconds data. 
                break;
            }
            i++;
        }
    }

    // handle bad things
    catch (char* str)
    {
        fRes = FALSE;
        SLOG(eError, "FAIL: %s", str);
    }

    //
    // clean up
    //
    if (pPin)
    {
        pPin->SetState(KSSTATE_STOP);
        pPin->ClosePin();
    }

    if (pFormatEntry)
    {
        pFormatEntry->fPerfDataCollected = fRes;
    }

    return fRes;
}


bool GenerateAECWaveFormats(IWaveFormatEnum *pWFE)
{
    DWORD BM = IWaveFormatEnum::WF_BIT_16 | IWaveFormatEnum::WF_BIT_20 |
               IWaveFormatEnum::WF_BIT_24 | IWaveFormatEnum::WF_BIT_32;
    DWORD CM_Multi = IWaveFormatEnum::WF_CHANNEL_4 | IWaveFormatEnum::WF_CHANNEL_6;
    IWaveFormatEnum::WF_Struct_Type WF_st = IWaveFormatEnum::use_both;

    // Create a list of AEC formats to check for in this order:
    //  1. 44.1 KHz with channel numbers of 2, 1 and multi in this order
    //  2. 48 KHz with channel numbers of 2, 1 and multi in this order
    //  3. 16 KHz with channel numbers of 2, 1 and multi in this order
    //  4. 22.05 KHz with channel numbers of 2, 1 and multi in this order
    //  5. All possible formats

    HRESULT hr = S_OK;
    hr = pWFE->AddPCMFormats(BM, IWaveFormatEnum::WF_SAMPLE_44100,
                            IWaveFormatEnum::WF_CHANNEL_2, WF_st);
    if(FAILED(hr))  { return FALSE; }
    hr = pWFE->AddPCMFormats(BM, IWaveFormatEnum::WF_SAMPLE_44100,
                            IWaveFormatEnum::WF_CHANNEL_1, WF_st);
    if(FAILED(hr))  { return FALSE; }
    hr = pWFE->AddPCMFormats(BM, IWaveFormatEnum::WF_SAMPLE_44100, CM_Multi, WF_st);
    if(FAILED(hr))  { return FALSE; }
    
    hr = pWFE->AddPCMFormats(BM, IWaveFormatEnum::WF_SAMPLE_48000,
                            IWaveFormatEnum::WF_CHANNEL_2, WF_st);
    if(FAILED(hr))  { return FALSE; }
    hr = pWFE->AddPCMFormats(BM, IWaveFormatEnum::WF_SAMPLE_48000,
                            IWaveFormatEnum::WF_CHANNEL_1, WF_st);
    if(FAILED(hr))  { return FALSE; }
    hr = pWFE->AddPCMFormats(BM, IWaveFormatEnum::WF_SAMPLE_48000, CM_Multi, WF_st);
    if(FAILED(hr))  { return FALSE; }

    hr = pWFE->AddPCMFormats(BM, IWaveFormatEnum::WF_SAMPLE_16000,
                            IWaveFormatEnum::WF_CHANNEL_2, WF_st);
    if(FAILED(hr))  { return FALSE; }
    hr = pWFE->AddPCMFormats(BM, IWaveFormatEnum::WF_SAMPLE_16000,
                            IWaveFormatEnum::WF_CHANNEL_1, WF_st);
    if(FAILED(hr))  { return FALSE; }
    hr = pWFE->AddPCMFormats(BM, IWaveFormatEnum::WF_SAMPLE_16000, CM_Multi, WF_st);
    if(FAILED(hr))  { return FALSE; }

    hr = pWFE->AddPCMFormats(BM, IWaveFormatEnum::WF_SAMPLE_22050,
                            IWaveFormatEnum::WF_CHANNEL_2, WF_st);
    if(FAILED(hr))  { return FALSE; }
    hr = pWFE->AddPCMFormats(BM, IWaveFormatEnum::WF_SAMPLE_22050,
                            IWaveFormatEnum::WF_CHANNEL_1, WF_st);
    if(FAILED(hr))  { return FALSE; }
    hr = pWFE->AddPCMFormats(BM, IWaveFormatEnum::WF_SAMPLE_22050, CM_Multi, WF_st);
    if(FAILED(hr))  { return FALSE; }

    hr = pWFE->AddPCMFormats(IWaveFormatEnum::WF_BIT_ALL, IWaveFormatEnum::WF_SAMPLE_ALL,
                             IWaveFormatEnum::WF_CHANNEL_ALL, WF_st);
    if(FAILED(hr))  { return FALSE; }

    return true;
}

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
DWORD
Test_DriftAndJitter_ForAEC
(
    void
)
{
    DWORD           dwRes = FNS_PASS;
    BOOL            fRes = FALSE;
    CKsFilter      *pFilter = g_pKsPosTst->m_pHalf->m_pFilter;
    CPCMAudioPin   *pPin = g_pKsPosTst->m_pHalf->m_pPin;
    WAVEFORMATEX   *pwfx = NULL;
    BOOL            fTestingLoopedDevice = g_pKsPosTst->m_fLoop;
    bool            fRTPin = g_pKsPosTst->m_pHalf->IsWaveRTPin();
    CPtr<LRModel>   pLrModel;

    if(!pFilter || !pPin)
    {
        SLOG(eError, "Unable to find a CKsFilter or Pin for current device");
        return FNS_FAIL;
    }

    if(!g_pKsPosTst->m_pHalf->IsTestSupportedForDeviceType(fTestingLoopedDevice) || g_pKsPosTst->m_pHalf->IsBTHPin())
    {
        XLOG(XSKIP, eError, "Test is not supported for the current device type.");
        return FNS_SKIPPED;
    }

	if(IsPinOffloadCapable(pPin))
    {
        XLOG(XSKIP, eError, "SKIP: Test is not supported on the offload pin." );
        return FNS_SKIPPED;
    }

    // Set a viable format on the pin
    CComPtr<IWaveFormatEnum>    pWfe = NULL;
    IWaveFormatEnum            *pWfeFinal = NULL;
    WAVEFORMATEXTENSIBLE        wfxe = {0};

    if(FAILED(CreateWaveFormatEnum(&pWfe)))
    {
        XLOG(XFAIL, 1, "Unable to create a WaveFormatEnum");
        goto exit;
    }
    if(GenerateAECWaveFormats(pWfe))    { pWfeFinal = pWfe; }
    if(!pPin->SetViablePCMWfxt(pWfeFinal))
    {
        XLOG(XFAIL, 1, "Unable to set a viable format on pin for streaming");
        goto exit;
    }
    pwfx = pPin->GetFormat();

    UINT cbWfx = sizeof(WAVEFORMATEX) + pwfx->cbSize;
    ASSERT(sizeof(wfxe) >= cbWfx); 
    if (sizeof(wfxe) < cbWfx)
    {
        XLOG(XFAIL, 1, "Found a WAVEFORMATEX description that cannot be fit into WAVEFORMATEXTENSIBLE.");

        log_wfx(g_pBasicLog, eError, pwfx);

        goto exit;
    }
    
    ZeroMemory(&wfxe, sizeof(wfxe));
    memcpy(&wfxe, pwfx, cbWfx);
    pwfx = (WAVEFORMATEX*)&wfxe;

    pLrModel = new LRModel(); // create in heap to prevent stack overflow
    if (NULL == pLrModel)
    {
        XLOG(XFAIL, 1, "Failed to allocate a LRModel object.");
        dwRes = FNS_FAIL;
        goto exit;
    }

    LRModelInit(pLrModel, pwfx->nSamplesPerSec);

    DWORD           dwRes2;

    // collect the data ~~~~~~~~~~~~~~~~~
    fRes = CollectSampleData_ForAEC(g_pKsPosTst->m_pHalf, pwfx, fRTPin, pLrModel, &dwRes2);
    if (!fRes)
    {
        SLOG(eError, "CollectSampleData_ForAEC failed");

        // We only care about if dwRes2 is FNS_SKIPPED, if not, fail this test.
        if (FNS_SKIPPED == dwRes2)
            dwRes = FNS_SKIPPED;
        else
            dwRes = FNS_FAIL;
        goto exit;
    }

    // calculate drift ~~~~~~~~~~~~~~~~~~
    LRModelGetResult(pLrModel);
    SLOG(eInfo2, "Results:");
    SLOG(eInfo2, "Collected %d good data points which covers %6.3f ms", pLrModel->NBest, ((double)pLrModel->iTimeLengthBest/ 10000));
    SLOG(eInfo2, "Detected %d glitches in %d seconds", pLrModel->iTtlGlitches, STREAMING_SECONDS_AEC);
    SLOG(eInfo2, "Specified sample rate  = %6.3f bytes/sec, %6.3f samples/sec", (double)(pwfx->nBlockAlign * pwfx->nSamplesPerSec), (double)pwfx->nSamplesPerSec);
    SLOG(eInfo2, "Calculated sample rate = %6.3f bytes/sec, %6.3f samples/sec", (double)(pwfx->nBlockAlign * pLrModel->fSlope), (double)pLrModel->fSlope);
    SLOG(eInfo2, "Sample rate drifting ratio = %f %%", (pLrModel->fSlope - pwfx->nSamplesPerSec) / pwfx->nSamplesPerSec * 100);
    SLOG(eInfo2, "Average position noise magnitude = %f ms", pLrModel->fAvgErrorMag);
    SLOG(eInfo2, "STD of position noise = %f ms", sqrt(pLrModel->fMSError));
    
    if (pLrModel->NBest > 0)
    {
        SLOG(eInfo2, "%f%% of position noise is larger than 0.8 ms", ((double)pLrModel->iHistgram[5]) * 100.0 / pLrModel->NBest); 
        SLOG(eInfo2, "Largest short term (2 seconds) position noise average is %fms", pLrModel->fMaxAvgErrorPer2Sec);
    }

    // evaluate the results

    if ((0 == pLrModel->NBest) || (((double)pLrModel->iTimeLengthBest / 10000.0) < 40 * 1000))
    {
        SLOG(eWarn1, "WARNING: Could not find 40 seconds continous glitch free samples in %d seconds test. Detected %d glitches", STREAMING_SECONDS_AEC, pLrModel->iTtlGlitches);
    }

    if (pLrModel->NBest > 0)
    {    
        if (fabs((pLrModel->fSlope - pwfx->nSamplesPerSec) / pwfx->nSamplesPerSec) > 0.001)
        {
            SLOG(eError, "FAIL: Sample rate drifting ratio is %f which is"
                         " larger than threshold of 0.001", 
                         (pLrModel->fSlope - pwfx->nSamplesPerSec) / pwfx->nSamplesPerSec);
            dwRes = FNS_FAIL;
        }
        
        if (pLrModel->fAvgErrorMag > 0.3)
        {
            SLOG(eError, "FAIL: average position noise magnitude is %fms which"
                         " is larger than 0.3ms", pLrModel->fAvgErrorMag);
            dwRes = FNS_FAIL;
        }

        if  (sqrt(pLrModel->fMSError) > 0.3)
        {
            SLOG(eError, "FAIL: STD of position noise is %fms which is larger "
                          "than 0.3ms", sqrt(pLrModel->fMSError));
            dwRes = FNS_FAIL;
        }

        if (((double)pLrModel->iHistgram[5] / pLrModel->NBest) > 0.01)
        {
            SLOG(eError, "FAIL: %f %% of points have larger than 0.8ms position"
                         " noise. Threshold is 1%%", 
                         ((double)pLrModel->iHistgram[5]/pLrModel->NBest)*100 );
            dwRes = FNS_FAIL;
        }
        
        for (int i = 0; i < pLrModel->iIndexOf2Sec; i++)
        {
            if (fabs(pLrModel->fAvgErrorPer2Sec[i]) > 0.1)
            {
                SLOG(eError, "FAIL: Short term (2 seconds) position noise average "
                             "is %fms around %d second into the data which is larger "
                             "than 0.1ms threshold", pLrModel->fAvgErrorPer2Sec[i], i*2);
                dwRes = FNS_FAIL;
            }
        }
    }
    
exit:
    if (dwRes == FNS_FAIL)
    {
        bool bVerbose = false;
        WEX::TestExecution::RuntimeParameters::TryGetValue<bool>(L"Verbose", bVerbose);
        if (!bVerbose)
        {
            SLOG(eInfo2, "To log the SamplePosition and Timestamp of all the samples, run the test with \"/p:Verbose=true\" ");
        }
    }
    
    return dwRes;
}

OACR_WARNING_POP
