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

#include "WaveTestTaef.h"
#include <dithering.h>
#include <signal-source.h>
#include <sine-signal-source.h>
#include <tchar.h>
#include <dvmodule.h>


#include <math.h>
#include "whqllimits.h"
#include <DevTrans.h>
#include "tests.h"
using namespace DevTrans;
#include <CommDev.h>
#include <HRString.h>
#include <multisz-iterator.h>
#include <helperclasses/cptr.h>
#include <CPropvariant.h>
#include <avendpointkeys.h>
#include <EndpointBusType.h>
using namespace EndpointBusType;

#include <AudioOffload.h>

HRESULT IsFilterPoweredBy(_In_ CKsFilter* filter, LPCSTR szDriver, _Out_ bool *pbPoweredBy);

// ------------------------------------------------------------------------------
// Test_SetGetPosition - Function calls for Set position. then it call from get position. 
// then it checks for difference between position and time to see that get and set position works correctly
// This test is applicable to Render pin only
// ------------------------------------------------------------------------------
DWORD Test_SetGetPosition(void)
{
    DWORD           dwRes = FNS_PASS;
    BOOL            fRes = FALSE;
    CPCMAudioPin*   pPin = g_pWaveTest->m_pHalf->m_pPin;
    WAVEFORMATEX*   pwfx = &g_pWaveTest->m_pHalf->m_pFmtCurrent->wfxFormat.Format;
    BOOL            fLooped = g_pWaveTest->m_fLoop;
    bool            fRTPin = g_pWaveTest->m_pHalf->IsWaveRTPin();

    double          t1, t2;
    double          dToleranceBy;
    double          dBytesPerMs = (double)pwfx->nAvgBytesPerSec / 1000.;
    ULONG           nDelta;
    double          dDeltaMs;

    if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL))
        XLOG(XFAIL, eError, "SetThreadPriority returned error: %d", GetLastError());
    try
    {
        KSAUDIO_POSITION posSet;
        KSAUDIO_POSITION posGet;
        double           tRunPre, tRunPost;
        BOOL             fSignaled = FALSE;
        double           tEnd;
        double           dPosEnd;

        if (!g_pWaveTest->m_pHalf->IsTestSupportedForDeviceType(fLooped))
        {
            XLOG(XSKIP, eError, "Test is not supported for the current device type.");
            return FNS_SKIPPED;
        }

        fRes = g_pWaveTest->m_pHalf->PreparePin(fLooped, fRTPin);


        if (!fRes)
            throw("Failed to prepare pin for the streaming which is required for the test case");

        dPosEnd = (double)g_pWaveTest->m_pHalf->m_pPin->m_cbStreamData;

        SLOG(eInfo2, "Submitting %d bytes to the pin", pPin->m_cbStreamData);

        if (g_pWaveTest->m_pHalf->m_flow == render)
            fRes = pPin->WriteData();
        else
            throw("Pin has the wrong dataflow");

        if (!fRes)
            throw("Failed to submit buffer to pin");

        // mark the time
        tRunPre = tpQPC();

        // run the adapter sink pin
        fRes = pPin->SetState(KSSTATE_RUN);
        if (!fRes)
            throw("Failed to set state on pin to KSSTATE_RUN");

        // mark the time
        tRunPost = tpQPC();
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

            posSet.PlayOffset = (ULONGLONG)(s);

            // get the time before and after get/set positions
            t1 = tpQPC();

            fRes = pPin->SetPosition(&posSet);
            if (!fRes)
                throw("    Failure:  Call to KSPROPERTY_AUDIO_POSITION (_SET) failed");

            tRunPre = tpQPC();

            // Sleep(0);

            fRes = pPin->GetPosition(&posGet);
            if (!fRes)
                throw("    Failure:  Call to KSPROPERTY_AUDIO_POSITION (_GET) failed");
            tRunPost = tpQPC();

            t2 = tpQPC() - t1;

            // is the position that we got reasonable?
            dToleranceBy = dBytesPerMs * (t2 + POS_ERR_TOLERANCE_MS);
            nDelta = abs((LONG)(posGet.PlayOffset - posSet.PlayOffset));
            dDeltaMs = (double)nDelta / dBytesPerMs;

            SLOG(eInfo2, "%5.2f ms,  %5d bytes,  %6d bytes,  %6d bytes,  %4d bytes,    %6.2f ms,    %.03f ms ,   %.03f ms , %.03f ms ",
                t2, (ULONG)dToleranceBy, (ULONG)posSet.PlayOffset, (ULONG)posGet.PlayOffset, nDelta, dDeltaMs, tRunPre, tRunPost, (double)(tRunPost - tRunPre));

            if (posSet.PlayOffset > posGet.PlayOffset)
            {
                dwRes = FNS_FAIL;
                XLOG(XFAIL, eError, "    Failure: The value retrieved from KSPROPERTY_AUDIO_POSITION is less than the value set");
            }

            if (nDelta > (ULONG)dToleranceBy)
            {
                dwRes = FNS_FAIL;
                XLOG(XFAIL, eError, "    Failure: The value retrieved from KSPROPERTY_AUDIO_POSITION is not within %d bytes of the value set", (ULONG)dToleranceBy);
            }

            // fSignaled = pPin->IsSignaled();
            Sleep(200);
        } while ((t1 < tEnd) && !fSignaled);
    }

    // handle bad things
    catch (char* str)
    {
        XLOG(XFAIL, eError, "FAIL: %s", str);
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

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);

    if (!fRes)
        dwRes = FNS_FAIL;

    return dwRes;
}


// ------------------------------------------------------------------------------
// Test_SetInvalidPosition - Function tries to set position outside of the buffer, submitted for streaming to the pin
// ------------------------------------------------------------------------------

DWORD Test_SetInvalidPosition(ePOSTestCaseType SetPos)
{
    DWORD           dwRes = FNS_FAIL;
    BOOL            fRes = FALSE;
    CPCMAudioPin*   pPin = g_pWaveTest->m_pHalf->m_pPin;
    BOOL            fLooped = g_pWaveTest->m_fLoop;

    if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL))
        XLOG(XFAIL, eError, "SetThreadPriority returned error: %d", GetLastError());

    try
    {
        KSAUDIO_POSITION posSet;
        KSAUDIO_POSITION posGet;
        bool fRTPin = g_pWaveTest->m_pHalf->IsWaveRTPin();
        if (!g_pWaveTest->m_pHalf->IsTestSupportedForDeviceType(fLooped))
        {
            XLOG(XSKIP, eError, "Test is not supported for the current device type.");
            return FNS_SKIPPED;
        }

        fRes = g_pWaveTest->m_pHalf->PreparePin(fLooped, fRTPin);

        if (!fRes)
            throw("Failed to prepare pin for the streaming which is required for the test case");

        // submit data to the kmixer sink pin 
        SLOG(eInfo2, "Submitting %d bytes to the pin", pPin->m_cbStreamData);

        if (g_pWaveTest->m_pHalf->m_flow == render)
            fRes = pPin->WriteData();
        else
            // throw("Pin has the wrong dataflow");
            fRes = pPin->ReadData();

        if (!fRes)
            throw("Failed to submit buffer to pin");

        fRes = pPin->SetState(KSSTATE_RUN);
        if (!fRes)
            throw("Failed to set state on pin to KSSTATE_RUN");

        // ~~~~~~~~~~~~~~~~~~~
        //
        dwRes = FNS_PASS;
        int count = 0;
        posGet.PlayOffset = 0;
        while ((posGet.PlayOffset == 0) && (count < 100)) {
            Sleep(10);
            fRes = pPin->GetPosition(&posGet);
            if (!fRes)
                throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");
            count++;
        }
        fRes = pPin->GetPosition(&posGet);
        if (!fRes)
            throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");
        if (posGet.PlayOffset == 0)
            throw ("Position did not moved");
        // ~~~~~~~~~~~~~~~~~~~
        //
        posSet.PlayOffset = 0;
        posSet.WriteOffset = 0;
        dwRes = FNS_PASS;
        switch (SetPos) {
        case SetPOStoOutsideofBuffer:
            posSet.PlayOffset = (ULONGLONG)(pPin->m_cbStreamData + 100);
            posSet.WriteOffset = (ULONGLONG)(pPin->m_cbStreamData + 200);
            SLOG(eInfo2, "setting position Outside the buffer submitted to the pin");
            break;
        case SameWritePlayPOSEndOfBuffer:
            posSet.PlayOffset = pPin->m_cbStreamData;
            posSet.WriteOffset = pPin->m_cbStreamData;
            SLOG(eInfo2, "setting position at the end of the buffer");
            break;
        case SameWritePlayPOSzero:
            posSet.PlayOffset = 0;
            posSet.WriteOffset = 0;
            SLOG(eInfo2, "setting position at the begining of the buffer");
            break;
        case SameWritePlayPOSHalfBuffer:
            posSet.PlayOffset = (ULONG)(pPin->m_cbStreamData / 2);
            posSet.WriteOffset = posSet.PlayOffset;
            SLOG(eInfo2, "setting position at the midway of the buffer");
            break;
        case PlayPOSGreater:
            posSet.PlayOffset = posGet.WriteOffset;
            posSet.WriteOffset = ULONG(posSet.PlayOffset / 2);
            break;
        }
        SLOG(eInfo2, "Setting position to Play cursor = %6d bytes, Write Cursor = %6d Bytes", (ULONG)posSet.PlayOffset, (ULONG)posSet.WriteOffset);
        fRes = pPin->SetPosition(&posSet);
        if (fRes) {
            switch (SetPos) {
            case SetPOStoOutsideofBuffer:
                SLOG(eError, "we successed setting position Outside the buffer submitted to the pin");
                break;
            case PlayPOSGreater:
                SLOG(eError, "we successed setting position with write cursor < Play cursor ");
                break;
            }
            Sleep(10);          // wait for 10 ms to see that how pin is doing with new position  
            fRes = pPin->GetPosition(&posGet);
            if (!fRes)
                throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");

            SLOG(eInfo2, "Position set to Play cursor = %6d bytes, Write Cursor = %6d Bytes And Get position reported to Play Cursor %6d bytes, Write Cursor %d Bytes  ", (ULONG)posSet.PlayOffset, (ULONG)posSet.WriteOffset, (ULONG)posGet.PlayOffset, (ULONG)posGet.WriteOffset);
            if (posGet.WriteOffset < posGet.PlayOffset) {
                dwRes = FNS_FAIL;
                XLOG(XFAIL, eError, "write cursor < Play cursor ");
            }

            if ((posGet.PlayOffset > pPin->m_cbStreamData) || (posGet.WriteOffset > pPin->m_cbStreamData)) {
                dwRes = FNS_FAIL;
                XLOG(XFAIL, eError, "    Failure: Position reported by the driver is outside of the buffer submitted to pin.");
            }
        }
        else
            SLOG(eInfo2, "Set position is rejected - 0x%x", GetLastError());
    }


    // handle bad things
    catch (char* str)
    {
        XLOG(XFAIL, eError, "FAIL: %s", str);
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

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);

    return dwRes;
}

// ------------------------------------------------------------------------------
// Test_SetInvalidPosition - Function tries to set position outside of the buffer, submitted for streaming to the pin
// ------------------------------------------------------------------------------

DWORD Test_SetInvalidPosition1(void)
{
    return Test_SetInvalidPosition(SetPOStoOutsideofBuffer);
}


// ------------------------------------------------------------------------------
// Test_SetInvalidPosition - Function tries to set position with play cursor higher then write cursor
// ------------------------------------------------------------------------------

DWORD Test_SetInvalidPosition2(void)
{
    return Test_SetInvalidPosition(PlayPOSGreater);
}

// ------------------------------------------------------------------------------
// Test_SetInvalidPosition - Function tries to set position with play cursor equal to write cursor
// ------------------------------------------------------------------------------

DWORD Test_SetInvalidPosition3(void)
{
    DWORD dwRes = FNS_FAIL;
    dwRes = Test_SetInvalidPosition(SameWritePlayPOSzero);
    if (dwRes == FNS_PASS)
        dwRes = Test_SetInvalidPosition(SameWritePlayPOSHalfBuffer);
    if (dwRes == FNS_PASS)
        dwRes = Test_SetInvalidPosition(SameWritePlayPOSEndOfBuffer);
    return dwRes;
}

//set position once pin reached to end of the buffer
// this test is only for the standard streaming and not for the looped streaming
DWORD Test_SetValidPosition()
{
    DWORD           dwRes = FNS_FAIL;
    BOOL            fRes = FALSE;
    CPCMAudioPin*   pPin = g_pWaveTest->m_pHalf->m_pPin;
    BOOL            fLooped = g_pWaveTest->m_fLoop;
    if (fLooped)
        return FNS_SKIPPED;

    if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL))
        XLOG(XFAIL, eError, "SetThreadPriority returned error: %d", GetLastError());

    try
    {
        KSAUDIO_POSITION posSet;
        KSAUDIO_POSITION posGet;
        bool fRTPin = g_pWaveTest->m_pHalf->IsWaveRTPin();
        if (!g_pWaveTest->m_pHalf->IsTestSupportedForDeviceType(fLooped))
        {
            XLOG(XSKIP, eError, "Test is not supported for the current device type.");
            return FNS_SKIPPED;
        }

        fRes = g_pWaveTest->m_pHalf->PreparePin(fLooped, fRTPin, 200);

        if (!fRes)
            throw("Failed to prepare pin for the streaming which is required for the test case");

        // submit data to the kmixer sink pin 
        SLOG(eInfo2, "Submitting %d bytes to the pin", pPin->m_cbStreamData);

        if (g_pWaveTest->m_pHalf->m_flow == render)
            fRes = pPin->WriteData();
        else
            //            throw("Pin has the wrong dataflow");
            fRes = pPin->ReadData();

        if (!fRes)
            throw("Failed to submit buffer to pin");

        fRes = pPin->SetState(KSSTATE_RUN);
        if (!fRes)
            throw("Failed to set state on pin to KSSTATE_RUN");

        // ~~~~~~~~~~~~~~~~~~~
        //
        dwRes = FNS_PASS;
        int count = 0;
        posGet.PlayOffset = 0;
        while ((posGet.PlayOffset == 0) && (count < 100)) {
            Sleep(10);
            fRes = pPin->GetPosition(&posGet);
            if (!fRes)
                throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");
            count++;
        }
        fRes = pPin->GetPosition(&posGet);
        if (!fRes)
            throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");
        if (posGet.PlayOffset == 0)
            throw ("Position did not moved");

        while (posGet.PlayOffset < pPin->m_cbStreamData) {
            Sleep(100);
            SLOG(eInfo3, "Waiting till pin reached to the end of the buffer");
            fRes = pPin->GetPosition(&posGet);
            if (!fRes)
                throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");
        }
        SLOG(eInfo3, "We have reached end of the buffer");
        posSet.PlayOffset = (pPin->m_cbStreamData) / 2;
        fRes = pPin->SetPosition(&posSet);
        if (!fRes)
            throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");

        fRes = pPin->GetPosition(&posGet);
        if (!fRes)
            throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");

        SLOG(eInfo2, "Position set to Play cursor = %6d bytes And Get position reported to Play Cursor %6d bytes, Write Cursor %d Bytes  ", (ULONG)posSet.PlayOffset, (ULONG)posGet.PlayOffset, (ULONG)posGet.WriteOffset);
        if (posGet.WriteOffset < posGet.PlayOffset)
        {
            XLOG(XFAIL, eError, "write cursor < Play cursor ");
        }

        if (posGet.PlayOffset > pPin->m_cbStreamData)
        {
            XLOG(XFAIL, eError, "    Failure: Position reported by the driver is outside of the buffer submitted to pin.");
        }
        dwRes = FNS_PASS;
    }


    // handle bad things
    catch (char* str)
    {
        XLOG(XFAIL, eError, "FAIL: %s", str);
        dwRes = FNS_FAIL;
        XLOG(XFAIL, eError, "Error received from the get last error 0x%x", GetLastError());
    }

    //
    // clean up
    //
    if (pPin)
    {
        pPin->SetState(KSSTATE_STOP);
        pPin->ClosePin();
    }

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);

    if (!fRes)
        dwRes = FNS_FAIL;

    return dwRes;
}


// Checks if a pin is offload capable. Does not propagate back failure modes, rather fails the calling TC body
// When any of the required actions (acquiring a log and calling into offload library) fails we would log a TC failure
// because we can't establish if the pin is eligible to run the test
// TODO: Re-evaluate if we need to skip the offload pin forever

bool IsPinOffloadCapable(CPCMAudioPin* pPin)
{
    HRESULT hr = S_OK;
    bool bOffloadSupported = false;
    CComPtr<IBasicLog> pLog;

    pLog = g_pBasicLog;

    hr = AudioOffload::DoesPinSupportOffload(pLog, pPin, AudioOffload::KS_API, bOffloadSupported);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "AudioOffload::DoesPinSupportOffload returned error (hr = %s)", GetHRString(hr));
    }

    return bOffloadSupported;
}

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
DWORD Test_ResetPin(void)
{
    DWORD           dwRes = FNS_FAIL;
    BOOL            fRes = FALSE;
    CPCMAudioPin*   pPin = g_pWaveTest->m_pHalf->m_pPin;
    CKsFilter*      pFilter = g_pWaveTest->m_pHalf->m_pFilter;      // used to find out that Filter is corresponding to USB or not
    BOOL            fLooped = g_pWaveTest->m_fLoop;
    bool            fRTPin = g_pWaveTest->m_pHalf->IsWaveRTPin();

    // ylsuen-2011-02-11:
    // IOCTL_KS_RESET_STATE is an optional control for miniport drivers. 
    // It is handled by KS, which calls into driver supplied optional reset routines. 
    // There is no way to tell if the callback of the control is implemented by the driver from user mode. 
    // Only KS can tell from the pin and filter descriptor.
    // KS also would not fail the control even the driver does not support this code.
    // We only know that portcls drivers would handle this properly.

    if (g_pWaveTest->m_pHalf->m_fIsKnownNonPortclsDriver || g_pWaveTest->m_pHalf->IsBTHPin())
    {
        XLOG(XSKIP, eError, "IOCTL_KS_RESET_STATE is not required to be supported by non-portcls drivers.");
        return FNS_SKIPPED;
    }

    if (!g_pWaveTest->m_pHalf->IsTestSupportedForDeviceType(fLooped))
    {
        XLOG(XSKIP, eError, "Test is not supported for the current device type.");
        return FNS_SKIPPED;
    }

    // Check to determine if the pin is offload.  If so, skip this test.
    if (IsPinOffloadCapable(pPin))
    {
        XLOG(XSKIP, eError, "Test is not supported on the offload pin.");
        return FNS_SKIPPED;
    }

    if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL))
    {
        XLOG(XFAIL, eError, "SetThreadPriority returned error: %d", GetLastError());
    }

    try
    {
        KSAUDIO_POSITION pos;

        // USB devices can only take 256ms at a time.  Limit buffer here if device is USB (m_UserDef = TRUE)
        fRes = g_pWaveTest->m_pHalf->PreparePin(fLooped, fRTPin, pFilter->m_UserDef ? USB_BUFLEN_MS : BUFLEN_MS);
        if (!fRes)
            throw("Failed to prepare pin for the streaming which is required for the test case");
        SLOG(eInfo2, "Submitting %d bytes to the pin", pPin->m_cbStreamData);

        if (g_pWaveTest->m_pHalf->IsWaveRTPacketStreamingSupported())
        {
            XLOG(XSKIP, eError, "The test doesn't support packet based pin.");
            dwRes = FNS_SKIPPED;
            goto Exit;
        }

        if (fRTPin)
        {   // in case of RT port, it does not support IOCTL_KS_WRITE_STREAM and IOCTL_KS_READ_STREAM
            fRes = pPin->SetState(KSSTATE_PAUSE);
        }
        else
        {
            fRes = ((g_pWaveTest->m_pHalf->m_flow == render) ? pPin->WriteData() : pPin->ReadData());
        }

        if (!fRes)
            throw("Failed to submit buffer to pin");

        fRes = pPin->SetState(KSSTATE_RUN);
        if (!fRes)
        {
            throw("Failure: Set state KSSTATE_RUN on pin failed");
        }

        int count = 0;
        pos.PlayOffset = 0;
        while ((pos.PlayOffset == 0) && (count < 100))
        {
            Sleep(10);
            fRes = pPin->GetPosition(&pos);
            if (!fRes)
            {
                throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");
            }
            count++;
        }
        fRes = pPin->GetPosition(&pos);
        if (!fRes)
        {
            throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");
        }
        if (pos.PlayOffset == 0)
        {
            throw ("Position did not moved");
        }
        SLOG(eInfo2, "Position reported before issuing KSRESET_BEGIN = %d bytes", (ULONG)pos.PlayOffset);

        fRes = pPin->SetState(fRTPin ? KSSTATE_PAUSE : KSSTATE_RUN);
        if (!fRes)
        {
            throw("Failure: Set state KSSTATE_PAUSE(RT)/KSSTATE_RUN(non-RT) failed");
        }

        // get position
        KSSTATE PinState;
        // first send KSRESET_BEGIN and see that Pin has responded it correctly
        fRes = pPin->Reset(KSRESET_BEGIN);
        if (!fRes)
            throw("Failed - Pin has not responded to IOCTL_KS_RESET_STATE command for KSRESET_BEGIN");
        fRes = pPin->GetPosition(&pos);
        if (!fRes)
            throw("Failed to get position");
        SLOG(eInfo2, "Position reported after issuing KSRESET_BEGIN = %d bytes", (ULONG)pos.PlayOffset);
        if (pos.PlayOffset != 0)
            throw("Pin has not responded to IOCTL_KS_RESET_STATE command for KSRESET_BEGIN correctly. Position after reset is not zero");
        fRes = pPin->GetState(&PinState);
        if (!fRes)
            throw("Failed to get Pin State");
        if (PinState != (fRTPin ? KSSTATE_PAUSE : KSSTATE_RUN))
            throw("Pin has not responded to IOCTL_KS_RESET_STATE command for KSRESET_BEGIN correctly. Pin State"
                " changed after reset command from %S to %d ", (fRTPin ? "KSSTATE_PAUSE" : "KSSTATE_RUN"), PinState);

        // submit IOCTL_KS_WRITE/READ again to see that pin position is moving or not
        if (g_pWaveTest->m_pHalf->m_flow == render)
        {
            fRes = pPin->WriteData(0, pPin->m_pbStreamData);
        }
        else
        {
            fRes = pPin->ReadData(0, pPin->m_pbStreamData);
        }
        if (!fRes)
        {
            throw("Failed WriteData(Render)/ReadData(Capture) to see if pin position is moving or not");
        }

        fRes = pPin->GetPosition(&pos);
        if (!fRes)
        {
            throw("Failed to get position");
        }

        SLOG(eInfo2, "Position reported after sending KSRESET_BEGIN and then sending IOCTL to read/write data to pin= %d bytes", (ULONG)pos.PlayOffset);
        if (pos.PlayOffset != 0)
        {
            if (fRTPin)
            {
                throw("RT Pin is in a paused state but the position changed after trying to play some buffers");
            }
            else
            {
                throw("Pin has not responded to IOCTL_KS_RESET_STATE command for KSRESET_BEGIN correctly. Position "
                    "started moving after sending IOCTL_KS_READ/WRITE Stream before sending KSRESET_END to pin");
            }
        }

        fRes = pPin->Reset(KSRESET_END);
        if (!fRes)
            throw("Failed - Pin has not responded to IOCTL_KS_RESET_STATE command for KSRESET_END");
        Sleep(5);
        fRes = pPin->GetPosition(&pos);
        if (!fRes)
            throw("Failed to get position");
        SLOG(eInfo2, "position reported after sending KSRESET_END= %d bytes", (ULONG)pos.PlayOffset);
        if (pos.PlayOffset != 0)
            throw("Pin has not responded to IOCTL_KS_RESET_STATE command for KSRESET_END correctly. Position after reset is not zero");
        fRes = pPin->GetState(&PinState);
        if (!fRes)
            throw("Failed to get Pin State");

        if (PinState != (fRTPin ? KSSTATE_PAUSE : KSSTATE_RUN))
        {
            throw("Pin has not responded to IOCTL_KS_RESET_STATE command for KSRESET_END correctly. "
                "Pin State changed after reset command from %s to %d ",
                (fRTPin ? "KSSTATE_PAUSE" : "KSSTATE_RUN"), PinState);
        }

        if (fRTPin)
        {
            fRes = pPin->SetState(KSSTATE_RUN);
            if (!fRes)
            {
                throw("Failed to set Pin state to KSSTATE_RUN");
            }
        }
        //send IOCTL READ/WRITE again to see that pin starts streaming or not
        if (g_pWaveTest->m_pHalf->m_flow == render)
        {
            fRes = pPin->WriteData(0, pPin->m_pbStreamData);
        }
        else
        {
            fRes = pPin->ReadData(0, pPin->m_pbStreamData);
        }
        if (!fRes)
        {
            throw("Failed WriteData(Render)/ReadData(Capture) to see that pin starts streaming or not");
        }

        for (int i = 0; i<25; i++) {
            fRes = pPin->GetPosition(&pos);
            if (!fRes)
                throw("Failed to get position");
            if (pos.PlayOffset == 0) {
                SLOG(eInfo2, "Giving one more chance - waiting for another 10 ms to see that position is changing or not");
                Sleep(10);
                if (fLooped)
                    Sleep(25); // we have noticed that in case of loop streaming, pin take long time to move from the start position
            }
            else
                break;
        }
        SLOG(eInfo2, "position reported after issuing KSRESET_END and then IOCTL to read/write data to pin= %d bytes", (ULONG)pos.PlayOffset);
        if (pos.PlayOffset == 0)
            throw("Pin has not responded to IOCTL_KS_RESET_STATE command for KSRESET_END correctly. Position did not move after sending IOCTL_KS_READ/WRITE Stream after sending KSRESET_END to pin");

        // TODO: Have KSRESET_END first and see what is the result
        dwRes = FNS_PASS;

    }
    // handle bad things
    catch (char* str)
    {
        XLOG(XFAIL, eError, "FAIL: %s", str);
        dwRes = FNS_FAIL;
    }

Exit:
    
    // clean up
    if (pPin)
    {
        pPin->SetState(KSSTATE_STOP);
        pPin->ClosePin();
    }
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);

    return dwRes;
}

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
DWORD Test_PinStateChanges(void)
{
    DWORD           dwRes = FNS_FAIL;
    BOOL            fRes = FALSE;
    CPCMAudioPin*   pPin = g_pWaveTest->m_pHalf->m_pPin;
    CKsFilter*      pFilter = g_pWaveTest->m_pHalf->m_pFilter;      // used to find out that Filter is corresponding to USB or not
    BOOL            fLooped = g_pWaveTest->m_fLoop;
    bool            fRTPin = g_pWaveTest->m_pHalf->IsWaveRTPin();

    // USB buffer too small. Need to have bigger buffer for this test
    if (pFilter && (pFilter->m_UserDef || g_pWaveTest->m_pHalf->IsBTHPin()))
    {
        return FNS_SKIPPED;
    }

    if (!g_pWaveTest->m_pHalf->IsTestSupportedForDeviceType(fLooped))
    {
        XLOG(XSKIP, eError, "Test is not supported for the current device type.");
        return FNS_SKIPPED;
    }

    // Check to determine if the pin is offload.  If so, skip this test.
    if (IsPinOffloadCapable(pPin))
    {
        XLOG(XSKIP, eError, "SKIP: Test is not supported on the offload pin.");
        return FNS_SKIPPED;
    }

    if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL))
    {
        XLOG(XFAIL, eError, "SetThreadPriority returned error: %d", GetLastError());
    }

    try
    {
        // USB devices can only take 256ms at a time.  Limit buffer here if device is USB (m_UserDef = TRUE)
        fRes = g_pWaveTest->m_pHalf->PreparePin(fLooped, fRTPin, pFilter->m_UserDef ? USB_BUFLEN_MS : BUFLEN_MS);
        if (!fRes)
        {
            throw("Failed to prepare pin for the streaming which is required for the test case");
        }

        if (g_pWaveTest->m_pHalf->IsWaveRTPacketStreamingSupported())
        {
            XLOG(XSKIP, eError, "The test doesn't support packet based pin.");
            dwRes = FNS_SKIPPED;
            goto Exit;
        }

        //submit data to the kmixer sink pin 
        SLOG(eInfo2, "Submitting %d bytes to the pin", pPin->m_cbStreamData);
        if (fRTPin)
        {   // in case of RT port, it does not support IOCTL_KS_WRITE_STREAM and IOCTL_KS_READ_STREAM
            fRes = pPin->SetState(KSSTATE_PAUSE);
        }
        else
        {
            fRes = ((g_pWaveTest->m_pHalf->m_flow == render) ? pPin->WriteData() : pPin->ReadData());
        }

        if (!fRes)
        {
            throw("Failed to submit buffer to pin");
        }

        // loop until we get a non-zero position
        KSSTATE PinState;
        KSAUDIO_POSITION pos = { 0 }, posRun = { 0 }, posPause = { 0 }, posStop = { 0 };

        // get state and set state are using SyncIOCTL. So we must get results of the position 
        // correctly as soon as we change the state of the pin
        fRes = pPin->SetState(KSSTATE_RUN);
        if (!fRes)
        {
            throw("changing pin state to KSSTATE_RUN Failed");
        }

        // Make sure the position does not stay 0
        for (int count = 0; (0 == pos.PlayOffset) && (count < 100); ++count)
        {
            Sleep(10);
            fRes = pPin->GetPosition(&pos);
            if (!fRes)
            {
                throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");
            }
        }

        // Now, pin starts to move. Record current position
        fRes = pPin->GetPosition(&pos);
        if (!fRes)
        {
            throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");
        }

        if (pos.PlayOffset == 0)
        {
            throw ("Position did not move");
        }

        // during the run condition make sure that pin position is moving
        // in the PAUSE state pin position should be same all time
        // in the STOP state pin position must be zero
        posRun.PlayOffset = pos.PlayOffset;
        UINT iCounter = 0;
        for (int nLoop = 0; nLoop < 10; nLoop++)
        {
            fRes = pPin->SetState(KSSTATE_RUN);
            if (!fRes)
            {
                throw("changing pin state to KSSTATE_RUN Failed");
            }

            Sleep(20);
            fRes = pPin->GetPosition(&pos);
            if (!fRes)
            {
                throw("failed to get position from the pin");
            }

            SLOG(eInfo2, "position reported after keeping pin into run state = %d bytes", (ULONG)pos.PlayOffset);
            // Assumption: Here I am assuming that postion will not move more then half of the buffer size betwwen 2 readings
            // practically we will have to find out the event that buffer is now doing looping
            // here we are assuning that we are suppling bug buffer to the pin so that it won't finsih buffer data with in 20 ms
            if ((fLooped) && ((ULONG)(abs((LONG)posRun.PlayOffset - (LONG)pos.PlayOffset)) > (pPin->m_cbStreamData / 2)))
            {
                SLOG(eInfo3, "We have looped so taking position reading again");
                posRun.PlayOffset = pos.PlayOffset;
                Sleep(20);
                fRes = pPin->GetPosition(&pos);
                if (!fRes)
                {
                    throw("failed to get position from the pin");
                }
            }

            if (posRun.PlayOffset >= pos.PlayOffset)
            {
                throw("Position is not moving when pin is into run state");
            }

            posRun.PlayOffset = pos.PlayOffset;
            // first set pin to PAUSE state
            fRes = pPin->SetState(KSSTATE_PAUSE);
            if (!fRes)
            {
                throw("changing pin state to KSSTATE_PAUSE Failed");
            }

            fRes = pPin->GetState(&PinState);
            if (!fRes)
            {
                throw("failed to get state from the pin");
            }
            SLOG(eInfo2, "Pin State = %d", (ULONG)PinState);

            Sleep(20); // why do we need this sleep? practically we should not have any sleep here. Confirm it with Frank
            fRes = pPin->GetPosition(&pos);
            if (!fRes)
            {
                throw("failed to get position from the pin");
            }
            SLOG(eInfo2, "position reported after keeping pin into pause state = %d bytes", (ULONG)pos.PlayOffset);
            posPause.PlayOffset = pos.PlayOffset;
            for (iCounter = 0; iCounter < 20; iCounter++)
            {
                fRes = pPin->GetPosition(&pos);
                if (!fRes)
                {
                    throw("Failed to get position on the pin");
                }

                if (pos.PlayOffset != posPause.PlayOffset)
                {
                    SLOG(eError, "position chaged from %I64d bytes to %I64d bytes while pin was in pause state", posPause.PlayOffset, pos.PlayOffset);
                    throw("Cursor postion changed when pin was in the pause state");
                }
            }
            if (pos.PlayOffset == pPin->m_cbStreamData)
            {
                SLOG(eInfo2, "We have reached to end of the buffer.");
                break;
            }
        }
        // now put pin into stop state
        fRes = pPin->SetState(KSSTATE_STOP);
        if (!fRes)
        {
            throw("changing pin state to KSSTATE_STOP Failed");
        }

        for (iCounter = 0; iCounter < 50; iCounter++)
        {
            fRes = pPin->GetPosition(&posStop);
            if (!fRes)
            {
                throw("Failed to get position after KSSTATE_STOP");
            }

            if (posStop.PlayOffset != 0)
            {
                throw("Cursor postion is not zero when we kept pin into stop position");
            }
        }
        dwRes = FNS_PASS;

    }
    // handle bad things
    catch (char* str)
    {
        XLOG(XFAIL, eError, "FAIL: %s", str);
        dwRes = FNS_FAIL;
    }

Exit:

    // clean up
    if (pPin)
    {
        pPin->SetState(KSSTATE_STOP);
        pPin->ClosePin();
    }

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);

    return dwRes;
}

// used to fill data format  - KSDATAFORMAT_WAVEFORMATEX
VOID FillKSDATAFORMAT_WAVEFORMATEX(PKSDATAFORMAT_WAVEFORMATEX pFormat, DataRangeInfo* pDataFormatInfo)
{
    pFormat->DataFormat.FormatSize = sizeof(KSDATAFORMAT_WAVEFORMATEX);
    pFormat->DataFormat.Flags = 0;
    pFormat->DataFormat.Reserved = 0;
    pFormat->DataFormat.SampleSize = 0;
    pFormat->DataFormat.MajorFormat = KSDATAFORMAT_TYPE_AUDIO;
    pFormat->DataFormat.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
    pFormat->DataFormat.Specifier = KSDATAFORMAT_SPECIFIER_WAVEFORMATEX;

    pFormat->WaveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
    pFormat->WaveFormatEx.nChannels = (WORD)pDataFormatInfo->Channels;
    pFormat->WaveFormatEx.nSamplesPerSec = (DWORD)pDataFormatInfo->SampleFrequency;
    pFormat->WaveFormatEx.nAvgBytesPerSec = (DWORD)((pDataFormatInfo->SampleFrequency)* (pDataFormatInfo->Channels)*(pDataFormatInfo->BitsPerSample)) / 8;
    pFormat->WaveFormatEx.wBitsPerSample = (WORD)pDataFormatInfo->BitsPerSample;
    pFormat->WaveFormatEx.nBlockAlign = (WORD)(pDataFormatInfo->Channels * pDataFormatInfo->BitsPerSample / 8);
    pFormat->WaveFormatEx.cbSize = 0;
}
//==============================================
// pass the FormatFlag and it will fill channle, sample rate and other information into data range
void FormatFlagToFormatInfo(DWORD dwFormatFlag, DataRangeInfo* pDataFormatInfo) {

    switch (dwFormatFlag) {
    case    WAVE_FORMAT_1M08: //       0x00000001       // 11.025 kHz, Mono,   8-bit 
        pDataFormatInfo->BitsPerSample = 8;
        pDataFormatInfo->Channels = 1;
        pDataFormatInfo->SampleFrequency = 11025;
        break;

    case    WAVE_FORMAT_1S08: //       0x00000002       // 11.025 kHz, Stereo, 8-bit 
        pDataFormatInfo->BitsPerSample = 8;
        pDataFormatInfo->Channels = 2;
        pDataFormatInfo->SampleFrequency = 11025;
        break;

    case    WAVE_FORMAT_1M16: //       0x00000004       // 11.025 kHz, Mono,   16-bit
        pDataFormatInfo->BitsPerSample = 16;
        pDataFormatInfo->Channels = 1;
        pDataFormatInfo->SampleFrequency = 11025;
        break;

    case    WAVE_FORMAT_1S16: //       0x00000008       // 11.025 kHz, Stereo, 16-bit
        pDataFormatInfo->BitsPerSample = 16;
        pDataFormatInfo->Channels = 2;
        pDataFormatInfo->SampleFrequency = 11025;
        break;

    case    WAVE_FORMAT_2M08: //       0x00000010       // 22.05  kHz, Mono,   8-bit 
        pDataFormatInfo->BitsPerSample = 8;
        pDataFormatInfo->Channels = 1;
        pDataFormatInfo->SampleFrequency = 22050;
        break;

    case    WAVE_FORMAT_2S08: //       0x00000020       // 22.05  kHz, Stereo, 8-bit 
        pDataFormatInfo->BitsPerSample = 8;
        pDataFormatInfo->Channels = 2;
        pDataFormatInfo->SampleFrequency = 22050;
        break;

    case    WAVE_FORMAT_2M16: //       0x00000040       // 22.05  kHz, Mono,   16-bit
        pDataFormatInfo->BitsPerSample = 16;
        pDataFormatInfo->Channels = 1;
        pDataFormatInfo->SampleFrequency = 22050;
        break;

    case    WAVE_FORMAT_2S16: //       0x00000080       // 22.05  kHz, Stereo, 16-bit
        pDataFormatInfo->BitsPerSample = 16;
        pDataFormatInfo->Channels = 2;
        pDataFormatInfo->SampleFrequency = 22050;
        break;

        /*        case    WAVE_FORMAT_4M08: //       0x00000100       // 44.1   kHz, Mono,   8-bit
        pDataFormatInfo->BitsPerSample = 8;
        pDataFormatInfo->Channels = 1;
        pDataFormatInfo->SampleFrequency = 44100;
        break;

        case    WAVE_FORMAT_4S08: //      0x00000200       // 44.1   kHz, Stereo, 8-bit
        pDataFormatInfo->BitsPerSample = 8;
        pDataFormatInfo->Channels = 2;
        pDataFormatInfo->SampleFrequency = 44100;
        break;

        case    WAVE_FORMAT_4M16: //       0x00000400       // 44.1   kHz, Mono,   16-bit
        pDataFormatInfo->BitsPerSample = 16;
        pDataFormatInfo->Channels = 1;
        pDataFormatInfo->SampleFrequency = 44100;
        break;

        case    WAVE_FORMAT_4S16: //       0x00000800       // 44.1   kHz, Stereo, 16-bit
        pDataFormatInfo->BitsPerSample = 16;
        pDataFormatInfo->Channels = 2;
        pDataFormatInfo->SampleFrequency = 44100;
        break;
        */
    case    WAVE_FORMAT_44M08: //      0x00000100       // 44.1   kHz, Mono,   8-bit  
        pDataFormatInfo->BitsPerSample = 8;
        pDataFormatInfo->Channels = 1;
        pDataFormatInfo->SampleFrequency = 44100;
        break;

    case    WAVE_FORMAT_44S08: //      0x00000200       // 44.1   kHz, Stereo, 8-bit  
        pDataFormatInfo->BitsPerSample = 8;
        pDataFormatInfo->Channels = 2;
        pDataFormatInfo->SampleFrequency = 44100;
        break;

    case    WAVE_FORMAT_44M16: //      0x00000400       // 44.1   kHz, Mono,   16-bit 
        pDataFormatInfo->BitsPerSample = 16;
        pDataFormatInfo->Channels = 1;
        pDataFormatInfo->SampleFrequency = 44100;
        break;

    case    WAVE_FORMAT_44S16: //      0x00000800       // 44.1   kHz, Stereo, 16-bit 
        pDataFormatInfo->BitsPerSample = 16;
        pDataFormatInfo->Channels = 2;
        pDataFormatInfo->SampleFrequency = 44100;
        break;

    case    WAVE_FORMAT_48M08: //      0x00001000       // 48     kHz, Mono,   8-bit  
        pDataFormatInfo->BitsPerSample = 8;
        pDataFormatInfo->Channels = 1;
        pDataFormatInfo->SampleFrequency = 48000;
        break;

    case    WAVE_FORMAT_48S08: //      0x00002000       // 48     kHz, Stereo, 8-bit  
        pDataFormatInfo->BitsPerSample = 8;
        pDataFormatInfo->Channels = 2;
        pDataFormatInfo->SampleFrequency = 48000;
        break;

    case    WAVE_FORMAT_48M16: //      0x00004000       // 48     kHz, Mono,   16-bit 
        pDataFormatInfo->BitsPerSample = 16;
        pDataFormatInfo->Channels = 1;
        pDataFormatInfo->SampleFrequency = 48000;
        break;

    case    WAVE_FORMAT_48S16: //      0x00008000       // 48     kHz, Stereo, 16-bit 
        pDataFormatInfo->BitsPerSample = 16;
        pDataFormatInfo->Channels = 2;
        pDataFormatInfo->SampleFrequency = 48000;
        break;

    case    WAVE_FORMAT_96M08: //      0x00010000       // 96     kHz, Mono,   8-bit  
        pDataFormatInfo->BitsPerSample = 8;
        pDataFormatInfo->Channels = 1;
        pDataFormatInfo->SampleFrequency = 96000;
        break;

    case    WAVE_FORMAT_96S08: //      0x00020000       // 96     kHz, Stereo, 8-bit  
        pDataFormatInfo->BitsPerSample = 8;
        pDataFormatInfo->Channels = 2;
        pDataFormatInfo->SampleFrequency = 96000;
        break;

    case    WAVE_FORMAT_96M16: //      0x00040000       // 96     kHz, Mono,   16-bit 
        pDataFormatInfo->BitsPerSample = 16;
        pDataFormatInfo->Channels = 1;
        pDataFormatInfo->SampleFrequency = 96000;
        break;

    case    WAVE_FORMAT_96S16: //      0x00080000       // 96     kHz, Stereo, 16-bit         
        pDataFormatInfo->BitsPerSample = 16;
        pDataFormatInfo->Channels = 2;
        pDataFormatInfo->SampleFrequency = 96000;
        break;
        // TODO: we will have to add case for higher sampling rate too and nore channel number.


    }
}
BOOL CheckFormatForRange(DWORD dwFormat, PKSDATARANGE_AUDIO pKsDataRangeAudio, DataRangeInfo* pDataFormatInfo)
{
    BOOL fResult = FALSE;
    BOOL fChannel = FALSE;
    BOOL fFrequecy = FALSE;
    BOOL fnBitsperSample = FALSE;
    FormatFlagToFormatInfo(dwFormat, pDataFormatInfo);
    if ((pDataFormatInfo->BitsPerSample >= pKsDataRangeAudio->MinimumBitsPerSample) && (pDataFormatInfo->BitsPerSample <= pKsDataRangeAudio->MaximumBitsPerSample))
        fnBitsperSample = TRUE;
    if ((pDataFormatInfo->Channels <= pKsDataRangeAudio->MaximumChannels))
        fChannel = TRUE;
    if ((pDataFormatInfo->SampleFrequency >= pKsDataRangeAudio->MinimumSampleFrequency) && (pDataFormatInfo->SampleFrequency <= pKsDataRangeAudio->MaximumSampleFrequency))
        fFrequecy = TRUE;
    // in case of azalia drivr we found that driver reports max channel equal to 2 but they are not supporting mono format. so best way to find out is to call for KSPROPERTY_PIN_DATAINTERSECTION

    fResult = (fFrequecy & fChannel & fnBitsperSample);
    return fResult;
}

VOID DisplayDataRange(PKSDATARANGE pDataRange) {
    if (pDataRange) {
        SLOG(eInfo2, "Datarange->Formatsize = %d", pDataRange->FormatSize);
        SLOG(eInfo2, "Datarange->Flags = 0x%x", pDataRange->Flags);
        SLOG(eInfo2, "Datarange->SampleSize = 0x%x", pDataRange->SampleSize);
        SLOG(eInfo2, "Datarange->Reserved = 0x%x", pDataRange->Reserved);
        SLOG(eInfo2, "Datarange->MajorFormat = %x-%x-%x-%x%x-%x%x%x%x%x%x ", DisplayGUID(pDataRange->MajorFormat));
        SLOG(eInfo2, "Datarange->SubFormat = %x-%x-%x-%x%x-%x%x%x%x%x%x", DisplayGUID(pDataRange->SubFormat));
        SLOG(eInfo2, "Datarange->Specifier = %x-%x-%x-%x%x-%x%x%x%x%x%x ", DisplayGUID(pDataRange->Specifier));

    }
    else
        XLOG(XFAIL, eError, "DisplayDataRange not initialized properly.");
}

VOID DisplayWaveFormatEx(PWAVEFORMATEX pWfx) {
    if (pWfx) {
        SLOG(eInfo2, "WaveFormatEx->wFormatTag = %d", pWfx->wFormatTag);
        SLOG(eInfo2, "WaveFormatEx->nChannels   = %d", pWfx->nChannels);
        SLOG(eInfo2, "WaveFormatEx->nSamplesPerSec = %d", pWfx->nSamplesPerSec);
        SLOG(eInfo2, "WaveFormatEx->nAvgBytesPerSec = %d", pWfx->nAvgBytesPerSec);
        SLOG(eInfo2, "WaveFormatEx->nBlockAlign = %d", pWfx->nBlockAlign);
        SLOG(eInfo2, "WaveFormatEx->wBitsPerSample = %d", pWfx->wBitsPerSample);
        SLOG(eInfo2, "WaveFormatEx->cbSize = %d", pWfx->cbSize);
    }
    else
        XLOG(XFAIL, eError, "WaveFormat not initialized properly.");
}

// ------------------------------------------------------------------------------
// Test_ChangeAudioFormat -  tO change data format while pin is in streaming state
// ------------------------------------------------------------------------------
DWORD Test_ChangeAudioFormat(void)
{
    DWORD           dwRes = FNS_FAIL;
    BOOL            fRes = FALSE;
    CPCMAudioPin*   pPin = g_pWaveTest->m_pHalf->m_pPin;
    CKsFilter*      pFilter = g_pWaveTest->m_pHalf->m_pFilter;      // used to find out that Filter is corresponding to USB or not
    BOOL            fLooped = g_pWaveTest->m_fLoop;
    bool            fRTPin = g_pWaveTest->m_pHalf->IsWaveRTPin();

    //BUGBUG - can we do SetDataFormat on WaveRT when no buffer allocated?  is this even useful?
    if (fRTPin)
    {
        SLOG(eInfo1, "Rt port filter does not support SetDataFormat when buffer is allocated.");
        return FNS_SKIPPED;
    }

    if (!g_pWaveTest->m_pHalf->IsTestSupportedForDeviceType(fLooped) || g_pWaveTest->m_pHalf->IsBTHPin())
    {
        XLOG(XSKIP, eError, "Test is not supported for the current device type.");
        return FNS_SKIPPED;
    }

    // USB supports looping.
    if (fLooped && pFilter && pFilter->m_UserDef)
    {
        SLOG(eInfo1, "USB filter does not support looped streaming.");
        return FNS_SKIPPED;
    }

    if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL))
    {
        XLOG(XFAIL, eError, "SetThreadPriority returned error: %d", GetLastError());
    }

    try
    {
        KSAUDIO_POSITION pos;

        // USB devices can only take 256ms at a time.  Limit buffer here if device is USB (m_UserDef = TRUE)
        // Otherwise create large buffer so that we can get more time to change data format 
        fRes = g_pWaveTest->m_pHalf->PreparePin(fLooped, fRTPin, pFilter->m_UserDef ? USB_BUFLEN_MS : BUFLEN_MS);
        if (!fRes)
        {
            throw("Failed to prepare pin for the streaming which is required for the test case");
        }
        //submit data to the kmixer sink pin 
        SLOG(eInfo2, "Submitting %d bytes to the pin", pPin->m_cbStreamData);

        if (g_pWaveTest->m_pHalf->IsWaveRTPacketStreamingSupported())
        {
            XLOG(XSKIP, eError, "The test doesn't support packet based pin.");
            fRes = TRUE;
            dwRes = FNS_SKIPPED;
            goto Exit;
        }

        if (fRTPin)
        {   // in case of RT port, it does not support IOCTL_KS_WRITE_STREAM and IOCTL_KS_READ_STREAM
            fRes = pPin->SetState(KSSTATE_PAUSE);
        }
        else
        {
            fRes = ((g_pWaveTest->m_pHalf->m_flow == render) ? pPin->WriteData() : pPin->ReadData());
        }
        if (!fRes)
            throw("Failed to submit buffer to pin");

        fRes = pPin->SetState(KSSTATE_RUN);

        int count = 0;
        pos.PlayOffset = 0;
        while ((pos.PlayOffset == 0) && (count < 100))
        {
            Sleep(10);
            fRes = pPin->GetPosition(&pos);
            if (!fRes)
            {
                throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");
            }
            count++;
        }

        fRes = pPin->GetPosition(&pos);

        if (!fRes)
        {
            throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");
        }

        if (pos.PlayOffset == 0)
        {
            throw ("Position did not moved");
        }

        //        fRes = pPin->SetState(KSSTATE_STOP);
        // if the position is moving means we are streaming right now data. So send request for the change of the format on the pin.
        // now loop through the formatsupported by the driver
        PKSMULTIPLE_ITEM pRanges = NULL;
        pRanges = pPin->m_Descriptor.pmiDataRanges;
        if (!pRanges)
        {  // means we do not have any information about dataranges supported by the pin so we wil not run this test case.
            SLOG(eInfo1, "we do not have any information about dataranges supported by the pin so we wil not run this test case");
            goto EXIT;
        }
        // get the details of the dataranges form the pin descriptor
        PKSDATARANGE pDataRange = (PKSDATARANGE)(pPin->m_Descriptor.pmiDataRanges + 1);
        pDataRange = (PKSDATARANGE)((ULONG_PTR(pDataRange) + (ULONG_PTR)7) & ~(ULONG_PTR)7);
        PKSDATARANGE_AUDIO pKsDatarangeAudio;
        WAVEFORMATEX wfx;
        DataRangeInfo DataFormatInfo;
        BOOL fRange = FALSE;
        bool thisIsAttributeList = false;
        bool nextIsAttributeList = false;

        for (ULONG i = 0; i < pRanges->Count; i++)
        {
            thisIsAttributeList = nextIsAttributeList;
            nextIsAttributeList = false;

            // ignore attribute lists
            if (thisIsAttributeList)
            {
                goto next_range;
            }

            if (pDataRange->Flags & KSDATARANGE_ATTRIBUTES)
            {
                nextIsAttributeList = true;
            }

            // Chceking for the PCM Format first 
            if (IsEqualGUIDAligned(pDataRange->SubFormat, KSDATAFORMAT_SUBTYPE_PCM) &&
                IsEqualGUIDAligned(pDataRange->Specifier, KSDATAFORMAT_SPECIFIER_WAVEFORMATEX) &&
                (pDataRange->FormatSize == sizeof(KSDATARANGE_AUDIO))) {
                // range reported by the driver is of KSDATARANGE_AUDIO
                pKsDatarangeAudio = (PKSDATARANGE_AUDIO)pDataRange;
                // check that this format is supported by the driver. If so that try to chage existing format with this format
                for (DWORD dwFormat = MSB_FORMAT; dwFormat; dwFormat /= 2)
                {
                    fRange = CheckFormatForRange(dwFormat, pKsDatarangeAudio, &DataFormatInfo);
                    if (fRange)
                    {
                        wfx.nChannels = DataFormatInfo.Channels;
                        wfx.wBitsPerSample = DataFormatInfo.BitsPerSample;
                        wfx.nSamplesPerSec = DataFormatInfo.SampleFrequency;
                        if (pPin->DoesFormatIntersect(&wfx, FALSE))
                        {
                            SLOG(eInfo2, "Changing pin format to");
                            DisplayDataRange(g_pWaveTest->m_pDataFormat);
                            DisplayWaveFormatEx((PWAVEFORMATEX)(g_pWaveTest->m_pDataFormat + 1));
                            if (!fLooped)      // if pin had reached to end of the buffer then submit buffer again to pin
                            {
                                fRes = pPin->GetPosition(&pos);
                                if (!fRes)
                                {
                                    throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");
                                }
                                if (pos.PlayOffset == pPin->m_cbStreamData)
                                {
                                    if (g_pWaveTest->m_pHalf->m_flow == render)
                                    {
                                        fRes = pPin->WriteData();
                                    }
                                    else
                                    {
                                        fRes = pPin->ReadData();
                                    }
                                }
                            }

                            // need to pause the stream before Set Format on looped streaming
                            if (fLooped)
                            {
                                SLOG(eInfo2, "Pausing stream before SetDataFormat");
                                if (!pPin->SetState(KSSTATE_PAUSE))
                                {
                                    throw("    Failure:  SetState(KSSTATE_PAUSE) failed");
                                }
                            }

                            fRes = pPin->SetDataFormat(g_pWaveTest->m_pDataFormat, g_pWaveTest->m_DataFormatSize);

                            if (!fRes)
                            {
                                DWORD dwError = GetLastError();
                                SLOG(eInfo2, "dwError = 0x%x ", dwError);
                                if (dwError == 0x01 || dwError == 0x57 || dwError == 0xAA)     // 0x01 means incorrect function means driver might not be suporting change in the data format
                                {                                                              // 0xAA - means that requested resources is in use.  
                                    fRes = TRUE;
                                }
                                break;
                            }

                            // need to resume the stream after Set Format on looped streaming
                            if (fLooped)
                            {
                                SLOG(eInfo2, "Resuming stream after SetDataFormat");
                                if (!pPin->SetState(KSSTATE_RUN))
                                    throw("    Failure:  SetState(KSSTATE_RUN) failed");
                            }
                        }

                        if (!fRes)
                        {
                            throw("Failed to set pin data format");
                        }
                    } // if (fRange)
                      // if the pin reach to the end of the buffer then set poistion to the begining of the buffer and keep pin into run state
                      //fRes = pPin->GetPosition(&pos);
                      //SLOG (eInfo2, "getPosition returned %d", pos.PlayOffset);
                      //if (!fRes)
                      //    throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");
                      //if ((!fLooped) && (pPin->m_cbStreamData == pos.PlayOffset)) {
                      //    SLOG (eInfo2 ,  "We have reached at the end of the buffer");
                      //    goto EXIT;
                      //}
                } // for (dwFormat)
            } // if (PCM)

        next_range:
            ULONG size = (thisIsAttributeList ? ((PKSMULTIPLE_ITEM)pDataRange)->Size : pDataRange->FormatSize);
            pDataRange = (PKSDATARANGE)((ULONG_PTR(pDataRange) + size + (ULONG_PTR)7) & ~(ULONG_PTR)7);
        } // for each data range

    EXIT:
        dwRes = FNS_PASS;
    }
    // handle bad things
    catch (char* str)
    {
        XLOG(XFAIL, eError, "FAIL: %s", str);
        dwRes = FNS_FAIL;
    }

Exit:

    // clean up
    if (pPin)
    {
        pPin->SetState(KSSTATE_STOP);
        pPin->ClosePin();
    }

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);

    if (!fRes)
        dwRes = FNS_FAIL;

    return dwRes;
}

BOOL TestThread1(struct ThreadProcData *pTPData)
{
    BOOL                fRes = FALSE;
    KSAUDIO_POSITION    Pos, tPos;
    BOOL                fLooped = g_pWaveTest->m_fLoop;
    CPCMAudioPin       *pPin = g_pWaveTest->m_pHalf->m_pPin;

    // wait till main thread started streaming data
    WaitForSingleObject(pTPData->ahSemaphore[0], INFINITE);
    if (pTPData->TestCase != StreamingGetPosition)
    {
        if (g_pWaveTest->m_pHalf->m_flow == render)
            fRes = pPin->WriteData(0, pPin->m_pbStreamData);
        else
            fRes = pPin->ReadData(0, pPin->m_pbStreamData);
        Sleep(10);
    }
    if (pTPData->TestCase == CancelIO)
    {
        ReleaseSemaphore(pTPData->ahSemaphore[1], 1, NULL);     // signal that thread 1 has finish submitting data
        WaitForSingleObject(pTPData->ahSemaphore[2], INFINITE); // wait till thread 2 finish submitting data
        fRes = CancelIo(pPin->m_handle);
        Sleep(10);
    }
    if (pTPData->TestCase == StreamingGetPosition)
    {
        Pos.PlayOffset = 0;
        tPos.PlayOffset = 0;
        while (Pos.PlayOffset < pPin->m_cbStreamData)
        {
            fRes = pPin->GetPosition(&Pos);
            if (!fRes)
                throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");
            if (fLooped && ((ULONG)(abs((LONG)Pos.PlayOffset - (LONG)tPos.PlayOffset)) >(pPin->m_cbStreamData / 2)))
                break;
            tPos.PlayOffset = Pos.PlayOffset;
        }
    }
    if (pTPData->TestCase != StreamingGetPosition)
        ReleaseSemaphore(pTPData->ahSemaphore[3], 1, NULL);     // signal that thread 1 has finish Cancelling IO
    return fRes;
}

BOOL TestThread2(struct ThreadProcData *pTPData)
{
    BOOL fRes = FALSE;
    CPCMAudioPin*   pPin = g_pWaveTest->m_pHalf->m_pPin;
    WaitForSingleObject(pTPData->ahSemaphore[1], INFINITE); // wit till thread 1 finish submitting data
    if (g_pWaveTest->m_pHalf->m_flow == render)
        fRes = pPin->WriteData(0, pPin->m_pbStreamData);
    else
        fRes = pPin->ReadData(0, pPin->m_pbStreamData);
    Sleep(10);
    ReleaseSemaphore(pTPData->ahSemaphore[2], 1, NULL);     // signaled that this thread 2 has finished submitting data
    return fRes;
}
// ------------------------------------------------------------------------------
// Test_CancelIO -  to cancel Io during the streaming  
// creates 3 threads. In first thread submit buffer and send IOCTL. Second thread 
// ------------------------------------------------------------------------------
// TODO: This test case is written for only standard streaming and needs to be modify for the loop streaming
DWORD Test_CancelIO(void)
{
    DWORD           dwRes = FNS_FAIL;
    BOOL            fRes = FALSE;
    CPCMAudioPin*   pPin = g_pWaveTest->m_pHalf->m_pPin;
    BOOL            fLooped = g_pWaveTest->m_fLoop;
    bool            fRTPin = g_pWaveTest->m_pHalf->IsWaveRTPin();
    HANDLE          hThread1 = NULL, hThread2 = NULL;
    struct ThreadProcData   TPData = { 0 };         // Used to communicate messages between threads

    if (!g_pWaveTest->m_pHalf->IsTestSupportedForDeviceType(fLooped) || g_pWaveTest->m_pHalf->IsBTHPin())
    {
        XLOG(XSKIP, eError, "Test is not supported for the current device type.");
        return FNS_SKIPPED;
    }

    // Check to determine if the pin is offload.  If so, skip this test.
    if (IsPinOffloadCapable(pPin))
    {
        XLOG(XSKIP, eError, "SKIP: Test is not supported on the offload pin.");
        return FNS_SKIPPED;
    }

    try
    {
        KSAUDIO_POSITION pos1, pos2;

        fRes = g_pWaveTest->m_pHalf->PreparePin(fLooped, fRTPin, 200);
        if (!fRes)
            throw("Failed to prepare pin for the streaming which is required for the test case");
        //submit data to the kmixer sink pin 
        SLOG(eInfo2, "Submitting %d bytes to the pin", pPin->m_cbStreamData);

        if (g_pWaveTest->m_pHalf->IsWaveRTPacketStreamingSupported())
        {
            XLOG(XSKIP, eError, "The test doesn't support packet based pin.");
            dwRes = FNS_SKIPPED;
            goto Exit;
        }

        if (fRTPin)
        {   // in case of RT port, it does not support IOCTL_KS_WRITE_STREAM and 	IOCTL_KS_READ_STREAM
            fRes = pPin->SetState(KSSTATE_PAUSE);
        }
        else
        {
            fRes = ((g_pWaveTest->m_pHalf->m_flow == render) ? pPin->WriteData() : pPin->ReadData());
        }

        if (!fRes)
            throw("Failed to submit buffer to pin");

        SLOG(eInfo2, "Creating semaphore");
        TPData.ahSemaphore[0] = CreateSemaphoreW(0, 0, 1, NULL);
        if (TPData.ahSemaphore[0] == NULL)
            throw ("'Failed to create Semaphore");
        TPData.ahSemaphore[1] = CreateSemaphoreW(0, 0, 1, NULL);
        if (TPData.ahSemaphore[1] == NULL)
            throw ("'Failed to create Semaphore");
        TPData.ahSemaphore[2] = CreateSemaphoreW(0, 0, 1, NULL);
        if (TPData.ahSemaphore[2] == NULL)
            throw ("'Failed to create Semaphore");
        TPData.ahSemaphore[3] = CreateSemaphoreW(0, 0, 1, NULL);
        if (TPData.ahSemaphore[3] == NULL)
            throw ("'Failed to create Semaphore");
        SLOG(eInfo2, "Created semaphore");

        fRes = g_pWaveTest->m_pHalf->PrepareBuffer(fRTPin, 200);    // prepare buffer for submitting data in thread 1
        if (!fRes)
            throw("Failed to submit buffer to pin for thread 1");

        TPData.TestCase = CancelIO;
        SLOG(eInfo2, "Creating thread 1");
        hThread1 = CreateThread(NULL, 0, reinterpret_cast <LPTHREAD_START_ROUTINE> (TestThread1), &TPData, 0, NULL);
        if (hThread1 == NULL)
            throw("Failed to create thread 1");

        fRes = g_pWaveTest->m_pHalf->PrepareBuffer(fRTPin, 200);    // prepare buffer to submit into thread 2
        if (!fRes)
            throw("Failed to submit buffer to pin");

        SLOG(eInfo2, "Creating thread 2");
        hThread2 = CreateThread(NULL, 0, reinterpret_cast <LPTHREAD_START_ROUTINE> (TestThread2), &TPData, 0, NULL);
        if (hThread2 == NULL)
            throw("Failed to create thread 2");

        SLOG(eInfo2, "thread created");
        // set pininto run state so that main thread start streaming data 
        fRes = pPin->SetState(KSSTATE_RUN);
        if (!fRes)
            throw("Failed to set pin into run state");

        int count = 0;
        pos1.PlayOffset = 0;
        pos2.PlayOffset = 0;
        while (pos1.PlayOffset == 0)
        {
            Sleep(10);
            fRes = pPin->GetPosition(&pos1);
            if (!fRes)
                throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");
            count++;
            if (count > 100)
                break;
        }
        fRes = pPin->GetPosition(&pos1);
        if (!fRes)
            throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");
        if (pos1.PlayOffset == 0)
            throw ("Position did not moved");

        // pin has started streaming data so signaled thread 1 for the same
        ReleaseSemaphore(TPData.ahSemaphore[0], 1, NULL);
        WaitForSingleObject(TPData.ahSemaphore[3], INFINITE); // wait till both thread finish submitting data and then thread 1 issue cancel IO command
        while (pos1.PlayOffset != pos2.PlayOffset) {
            if (fLooped && ((ULONG)(abs((LONG)pos1.PlayOffset - (LONG)pos2.PlayOffset)) > (pPin->m_cbStreamData / 2)))
            {
                SLOG(eInfo2, "For loop streaming we have detected loop");
                break;
            }
            SLOG(eInfo2, "pin position pos2.playoffset = %d , pos1.playOffset = %d ", (ULONG)pos2.PlayOffset, (ULONG)pos1.PlayOffset);
            pos2.PlayOffset = pos1.PlayOffset;
            fRes = pPin->GetPosition(&pos1);
            if (!fRes)
                throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");
            Sleep(10);
        }

        WaitForSingleObject(hThread1, INFINITE);
        WaitForSingleObject(hThread2, INFINITE);

        dwRes = FNS_PASS;

    }
    // handle bad things
    catch (char* str)
    {
        XLOG(XFAIL, eError, "FAIL: %s", str);
        dwRes = FNS_FAIL;
    }

    SafeCloseHandle(hThread1);
    SafeCloseHandle(hThread2);

    for (int i = 0; i < MAX_NUM_SEMAPHORES; i++) {
        SafeCloseHandle(TPData.ahSemaphore[i]);
    }

Exit:

    // clean up
    if (pPin)
    {
        pPin->SetState(KSSTATE_STOP);
        pPin->ClosePin();
    }

    return dwRes;
}
// ------------------------------------------------------------------------------
// Test_StreamingMultiThread -  Streaming through different threads
// ------------------------------------------------------------------------------
DWORD Test_StreamingMultiThread(void)
{
    DWORD           dwRes = FNS_FAIL;
    BOOL            fRes = FALSE;
    CPCMAudioPin*   pPin = g_pWaveTest->m_pHalf->m_pPin;
    CKsFilter*      pFilter = g_pWaveTest->m_pHalf->m_pFilter;      // used to find out that Filter is corresponding to USB or not
    BOOL            fLooped = g_pWaveTest->m_fLoop;
    bool            fRTPin = g_pWaveTest->m_pHalf->IsWaveRTPin();
    HANDLE          hThread1 = NULL;
    struct ThreadProcData   TPData = { 0 };         // Used to communicate messages between threads

    if (fLooped || !g_pWaveTest->m_pHalf->IsTestSupportedForDeviceType(fLooped))
    {
        // TODO: This test case is written for only standard streaming 
        // and needs to be modify for the loop streaming
        XLOG(XSKIP, eError, "This test does not run in standard looped streaming mode.");
        return FNS_SKIPPED;
    }

    // USB buffer too small. Need to have bigger buffer for this test
    if (pFilter && (pFilter->m_UserDef || g_pWaveTest->m_pHalf->IsBTHPin()))
    {
        return FNS_SKIPPED;
    }

    try
    {
        KSAUDIO_POSITION pos1, pos2;

        fRes = g_pWaveTest->m_pHalf->PreparePin(fLooped, fRTPin, 200);
        if (!fRes)
            throw("Failed to prepare pin for the streaming which is required for the test case");
        //submit data to the kmixer sink pin 
        SLOG(eInfo2, "Submitting %d bytes to the pin", pPin->m_cbStreamData);

        if (g_pWaveTest->m_pHalf->IsWaveRTPacketStreamingSupported())
        {
            XLOG(XSKIP, eError, "The test doesn't support packet based pin.");
            dwRes = FNS_SKIPPED;
            goto Exit;
        }

        if (fRTPin)
        {   // in case of RT port, it does not support IOCTL_KS_WRITE_STREAM and 	IOCTL_KS_READ_STREAM
            fRes = pPin->SetState(KSSTATE_PAUSE);
        }
        else
        {
            fRes = ((g_pWaveTest->m_pHalf->m_flow == render) ? pPin->WriteData() : pPin->ReadData());
        }
        if (!fRes)
        {
            throw("Failed to submit buffer to pin");
        }

        SLOG(eInfo2, "Creating semaphore");
        TPData.ahSemaphore[0] = CreateSemaphoreW(0, 0, 1, NULL);
        if (TPData.ahSemaphore[0] == NULL)
            throw ("'Failed to create Semaphore");
        TPData.ahSemaphore[1] = CreateSemaphoreW(0, 0, 1, NULL);
        if (TPData.ahSemaphore[1] == NULL)
            throw ("'Failed to create Semaphore");
        TPData.ahSemaphore[2] = CreateSemaphoreW(0, 0, 1, NULL);
        if (TPData.ahSemaphore[2] == NULL)
            throw ("'Failed to create Semaphore");
        TPData.ahSemaphore[3] = CreateSemaphoreW(0, 0, 1, NULL);
        if (TPData.ahSemaphore[3] == NULL)
            throw ("'Failed to create Semaphore");
        SLOG(eInfo2, "Created semaphore");

        fRes = g_pWaveTest->m_pHalf->PrepareBuffer(fRTPin, 200);    // prepare buffer for submitting data in thread 1
        if (!fRes)
            throw("Failed to submit buffer to pin for thread 1");

        SLOG(eInfo2, "Creating thread 1");
        TPData.TestCase = StreamingMultiThread;
        hThread1 = CreateThread(NULL, 0, reinterpret_cast <LPTHREAD_START_ROUTINE> (TestThread1), &TPData, 0, NULL);
        if (hThread1 == NULL)
            throw("Failed to create thread 1");

        SLOG(eInfo2, "thread created");
        // set pininto run state so that main thread start streaming data 
        fRes = pPin->SetState(KSSTATE_RUN);
        if (!fRes)
            throw("Failed to set pin into run state");

        int count = 0;
        pos1.PlayOffset = 0;
        pos2.PlayOffset = 0;
        while (pos1.PlayOffset == 0)
        {
            Sleep(10);
            fRes = pPin->GetPosition(&pos1);
            if (!fRes)
                throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");
            count++;
            if (count > 10)
                break;
        }
        fRes = pPin->GetPosition(&pos1);
        if (!fRes)
            throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");
        if (pos1.PlayOffset == 0)
            throw ("Position did not moved");

        // pin has started streaming data so signaled thread 1 for the same
        ReleaseSemaphore(TPData.ahSemaphore[0], 1, NULL);
        WaitForSingleObject(TPData.ahSemaphore[3], INFINITE); // wait till both thread finish submitting data and then thread 1 issue cancel IO command
        while (pos1.PlayOffset != pos2.PlayOffset)
        {
            if (fLooped && ((ULONG)(abs((LONG)pos1.PlayOffset - (LONG)pos2.PlayOffset)) > (pPin->m_cbStreamData / 2)))
            {
                SLOG(eInfo2, "For loop streaming we have detected loop");
                break;
            }
            SLOG(eInfo2, "pin position pos2.playoffset = %d , pos1.playOffset = %d ", (ULONG)pos2.PlayOffset, (ULONG)pos1.PlayOffset);
            pos2.PlayOffset = pos1.PlayOffset;
            fRes = pPin->GetPosition(&pos1);
            if (!fRes)
                throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");
            Sleep(10);
        }
        WaitForSingleObject(hThread1, INFINITE);
        dwRes = ((pos1.PlayOffset < (ULONGLONG)(pPin->m_cbStreamData) * 2) ? FNS_FAIL : FNS_PASS);
        if (FNS_FAIL == dwRes)
        {
            SLOG(eError, "FAIL: Position (%I64d) is less than the amount of data streamed (%I64d).",
                pos1.PlayOffset, (ULONGLONG)(pPin->m_cbStreamData) * 2);
        }
    }
    // handle bad things
    catch (char* str)
    {
        XLOG(XFAIL, eError, "FAIL: %s", str);
        dwRes = FNS_FAIL;
    }

    SafeCloseHandle(hThread1);

    for (int i = 0; i < MAX_NUM_SEMAPHORES; i++)
    {
        SafeCloseHandle(TPData.ahSemaphore[i]);
    }

Exit:

    // clean up
    if (pPin)
    {
        pPin->SetState(KSSTATE_STOP);
        pPin->ClosePin();
    }

    //SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
    return dwRes;
}

// ------------------------------------------------------------------------------
// Test_GetPositionMultiThread -  Streaming from main thread then call for get position from different threads
// ------------------------------------------------------------------------------
// TODO: This test case is written for only standard streaming and needs to be modify for the loop streaming
DWORD Test_GetPositionMultiThread(void)
{
    DWORD           dwRes = FNS_FAIL;
    BOOL            fRes = FALSE;
    CPCMAudioPin*   pPin = g_pWaveTest->m_pHalf->m_pPin;
    CKsFilter*      pFilter = g_pWaveTest->m_pHalf->m_pFilter;      // used to find out that Filter is corresponding to USB or not
    BOOL            fLooped = g_pWaveTest->m_fLoop;
    bool            fRTPin = g_pWaveTest->m_pHalf->IsWaveRTPin();
    HANDLE          hThread1 = NULL;
    BOOL            hTResult = FALSE;
    struct ThreadProcData   TPData = { 0 };         // Used to communicate messages between threads

                                                    //if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL))
                                                    //    SLOG(eError, "SetThreadPriority failed");

                                                    // TODO: Need to re-work this test case for USB, because the USB bus cannot take more than about 200ms buffer,
                                                    // and the test has race conditions on that buffer.
    if (pFilter && (pFilter->m_UserDef || g_pWaveTest->m_pHalf->IsBTHPin()))
    {
        return FNS_SKIPPED;
    }

    if (fLooped || !g_pWaveTest->m_pHalf->IsTestSupportedForDeviceType(fLooped))
    {
        XLOG(XSKIP, eError, "Test is not supported for the current device type.");
        return FNS_SKIPPED;
    }

    try
    {
        KSAUDIO_POSITION pos1, pos2;

        // USB devices can only take 256ms at a time.  Limit buffer here if device is USB (m_UserDef = TRUE)
        fRes = g_pWaveTest->m_pHalf->PreparePin(fLooped, fRTPin, pFilter->m_UserDef ? USB_BUFLEN_MS : BUFLEN_MS);
        if (!fRes)
            throw("Failed to prepare pin for the streaming which is required for the test case");
        //submit data to the kmixer sink pin 
        SLOG(eInfo2, "Submitting %d bytes to the pin", pPin->m_cbStreamData);

        if (g_pWaveTest->m_pHalf->IsWaveRTPacketStreamingSupported())
        {
            XLOG(XSKIP, eError, "The test doesn't support packet based pin.");
            dwRes = FNS_SKIPPED;
            goto Exit;
        }

        if (fRTPin)
        {   // in case of RT port, it does not support IOCTL_KS_WRITE_STREAM and 	IOCTL_KS_READ_STREAM
            fRes = pPin->SetState(KSSTATE_PAUSE);
        }
        else
        {
            fRes = ((g_pWaveTest->m_pHalf->m_flow == render) ? pPin->WriteData() : pPin->ReadData());
        }
        if (!fRes)
        {
            throw("Failed to submit buffer to pin");
        }

        SLOG(eInfo2, "Creating semaphore");
        TPData.ahSemaphore[0] = CreateSemaphoreW(0, 0, 1, NULL);
        if (TPData.ahSemaphore[0] == NULL)
            throw ("'Failed to create Semaphore");
        TPData.ahSemaphore[1] = CreateSemaphoreW(0, 0, 1, NULL);
        if (TPData.ahSemaphore[1] == NULL)
            throw ("'Failed to create Semaphore");
        TPData.ahSemaphore[2] = CreateSemaphoreW(0, 0, 1, NULL);
        if (TPData.ahSemaphore[2] == NULL)
            throw ("'Failed to create Semaphore");
        TPData.ahSemaphore[3] = CreateSemaphoreW(0, 0, 1, NULL);
        if (TPData.ahSemaphore[3] == NULL)
            throw ("'Failed to create Semaphore");
        SLOG(eInfo2, "Created semaphore");

        SLOG(eInfo2, "Creating thread 1");
        TPData.TestCase = StreamingGetPosition;
        hThread1 = CreateThread(NULL, 0, reinterpret_cast <LPTHREAD_START_ROUTINE> (TestThread1), &TPData, 0, NULL);
        if (hThread1 == NULL)
            throw("Failed to create thread 1");

        SLOG(eInfo2, "thread created");
        // set pininto run state so that main thread start streaming data 
        fRes = pPin->SetState(KSSTATE_RUN);
        if (!fRes)
            throw("Failed to set pin into run state");

        int count = 0;
        pos1.PlayOffset = 0;
        pos2.PlayOffset = 0;
        while (pos1.PlayOffset == 0)
        {
            Sleep(10);
            fRes = pPin->GetPosition(&pos1);
            if (!fRes)
                throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");
            count++;
            if (count > 50)
                break;
        }
        fRes = pPin->GetPosition(&pos1);
        if (!fRes)
            throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");
        if (pos1.PlayOffset == 0)
            throw ("Position did not moved");

        // pin has started streaming data so signaled thread 1 for the same
        ReleaseSemaphore(TPData.ahSemaphore[0], 1, NULL);
        BOOL fLoopStream = FALSE;
        //        WaitForSingleObject(TPData.ahSemaphore[3], INFINITE); // wait till both thread finish submitting data and then thread 1 issue cancel IO command
        while (pos1.PlayOffset != pos2.PlayOffset)
        {
            fLoopStream = (fLooped && ((ULONG)(abs((LONG)pos1.PlayOffset - (LONG)pos2.PlayOffset)) > (pPin->m_cbStreamData / 2)));
            if (fLoopStream) {
                SLOG(eInfo2, "For loop streaming we have detected loop");
                break;
            }
            SLOG(eInfo2, "pin position pos2.playoffset = %d , pos1.playOffset = %d ", (ULONG)pos2.PlayOffset, (ULONG)pos1.PlayOffset);
            pos2.PlayOffset = pos1.PlayOffset;
            Sleep(10);
            fRes = pPin->GetPosition(&pos1);
            if (!fRes)
                throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");

        }
        WaitForSingleObject(hThread1, INFINITE);
        if (pos1.PlayOffset < (ULONGLONG)(pPin->m_cbStreamData))
        {
            XLOG(XFAIL, eError, "pos1.PlayOffset < pPin->m_cbStreamData");
            dwRes = FNS_FAIL;
        }
        else
            dwRes = FNS_PASS;
        fRes = GetExitCodeThread(hThread1, (LPDWORD)&hTResult);
        if (!fRes)
            throw ("Fail in getting result back from Thread");
        if (!hTResult)
        {
            XLOG(XFAIL, eError, "thread did not returned success. 0x%x", GetLastError());
            dwRes = FNS_FAIL;
        }
    }
    // handle bad things
    catch (char* str)
    {
        XLOG(XFAIL, eError, "FAIL: %s", str);
        dwRes = FNS_FAIL;
    }


    SafeCloseHandle(hThread1);

    for (int i = 0; i < MAX_NUM_SEMAPHORES; i++)
    {
        SafeCloseHandle(TPData.ahSemaphore[i]);
    }

Exit:

    // clean up
    if (pPin)
    {
        pPin->SetState(KSSTATE_STOP);
        pPin->ClosePin();
    }

    //SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
    return dwRes;
}
// ------------------------------------------------------------------------------
// Test_StreamingSmallBuffer -  Streaming Small Buffer
// ------------------------------------------------------------------------------
// TODO: This test case is written for only standard streaming and needs to be modify for the loop streaming
DWORD Test_StreamingSmallBuffer(void)
{
    DWORD           dwRes = FNS_FAIL;
    BOOL            fRes = FALSE;
    CPCMAudioPin*   pPin = g_pWaveTest->m_pHalf->m_pPin;
    BOOL            fLooped = g_pWaveTest->m_fLoop;
    bool            fRTPin = g_pWaveTest->m_pHalf->IsWaveRTPin();

    if (g_pWaveTest->m_pHalf->IsBTHPin())
    {
        return FNS_SKIPPED;
    }

    // USB doesn't support looping.
    if (fLooped || !g_pWaveTest->m_pHalf->IsTestSupportedForDeviceType(fLooped))
    {
        SLOG(eInfo1, "Test is not supported for the current device type..");
        return FNS_SKIPPED;
    }

    try
    {
        KSAUDIO_POSITION pos1, pos2;

        fRes = g_pWaveTest->m_pHalf->PreparePin(fLooped, fRTPin, 1);
        if (!fRes)
            throw("Failed to prepare pin for the streaming which is required for the test case");
        //submit data to the kmixer sink pin 
        SLOG(eInfo2, "Submitting %d bytes to the pin", pPin->m_cbStreamData);

        if (g_pWaveTest->m_pHalf->IsWaveRTPacketStreamingSupported())
        {
            XLOG(XSKIP, eError, "The test doesn't support packet based pin.");
            dwRes = FNS_SKIPPED;
            goto Exit;
        }

        if (fRTPin)
        {   // in case of RT port, it does not support IOCTL_KS_WRITE_STREAM and 	IOCTL_KS_READ_STREAM
            fRes = pPin->SetState(KSSTATE_PAUSE);
        }
        else
        {
            fRes = ((g_pWaveTest->m_pHalf->m_flow == render) ? pPin->WriteData() : pPin->ReadData());
        }
        if (!fRes)
            throw("Failed to submit buffer to pin");

        // set pininto run state so that main thread start streaming data 
        fRes = pPin->SetState(KSSTATE_RUN);
        if (!fRes)
            throw("Failed to set pin into run state");

        int count = 0;
        pos1.PlayOffset = 0;
        pos2.PlayOffset = 0;
        while (pos1.PlayOffset == 0) {
            Sleep(10);
            fRes = pPin->GetPosition(&pos1);
            if (!fRes)
                throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");
            count++;
            if (count > 10)
                break;
        }
        fRes = pPin->GetPosition(&pos1);
        if (!fRes)
            throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");
        if (pos1.PlayOffset == 0)
            throw ("Position did not moved");
        while (pos1.PlayOffset != pos2.PlayOffset) {
            SLOG(eInfo2, "pin position pos2.playoffset = %d , pos1.playOffset = %d ", (ULONG)pos2.PlayOffset, (ULONG)pos1.PlayOffset);
            pos2.PlayOffset = pos1.PlayOffset;
            Sleep(5);  // we have small sleep time since we are supling very small buffer
            fRes = pPin->GetPosition(&pos1);
            if (!fRes)
                throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");
            count++;
            if ((count >5) && (fLooped))        // in case of looped buffer just come out after 5 loop
                break;
        }
        if (pos1.PlayOffset < (ULONGLONG)(pPin->m_cbStreamData))
            dwRes = FNS_FAIL;
        else
            dwRes = FNS_PASS;
    }
    // handle bad things
    catch (char* str)
    {
        XLOG(XFAIL, eError, "FAIL: %s", str);
        dwRes = FNS_FAIL;
    }

Exit:

    // clean up
    if (pPin)
    {
        pPin->SetState(KSSTATE_STOP);
        pPin->ClosePin();
    }

    //SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
    return dwRes;
}

BOOL CreateSecondInstanceOfPin(CPCMAudioPin*   pPin2, BOOL bDefaultSupport, BOOL fLooped)
{

    BOOL fRes = true;

    fRes = InstantiatePinWfx(pPin2, NULL, fLooped, 0, bDefaultSupport);
    if (fRes) {
        // prepare second pin        
        WAVEFORMATEX*               pwfx;
        PKSDATAFORMAT_WAVEFORMATEX  pFormatWfx = (PKSDATAFORMAT_WAVEFORMATEX)pPin2->m_pksDataFormat;
        CComPtr<IBasicLog>          pBasicLog;
        HRESULT                     hr = S_OK;

        pwfx = &pFormatWfx->WaveFormatEx;

        // allocate data
        fRes = pPin2->CreateDataBuffer(BUFLEN_MS, pwfx->nBlockAlign, 16);

        if (!fRes) {
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
            TEST_AMPLITUDE,
            TEST_FREQUENCY,
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

DWORD StreamMultiPin(CPCMAudioPin*   pPin1, CPCMAudioPin*   pPin2)
{
    int count = 0;
    BOOL fLooped = g_pWaveTest->m_fLoop;
    BOOL fRes, fLoop1, fLoop2;
    KSAUDIO_POSITION tpos1, pos1, pos2, tpos2;
    tpos1.PlayOffset = 0;
    tpos2.PlayOffset = 0;
    pos1.PlayOffset = 0;
    pos2.PlayOffset = 0;
    fLoop1 = fLoop2 = FALSE;


    while ((pos1.PlayOffset == 0) || (pos2.PlayOffset == 0))
    {
        Sleep(10);
        fRes = pPin1->GetPosition(&pos1);
        if (!fRes)
        {
            throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION on the first pin failed");
        }

        fRes = pPin2->GetPosition(&pos2);
        if (!fRes)
        {
            throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION on the second pin failed");
        }

        count++;
        if (count > 50)
        {
            throw("   One of the position for Pin1 or Pin2  is still not moving");
            break;
        }
        SLOG(eInfo2, "Pin1 = %d,  Pin2 = %d", pPin1->m_cbStreamData, pPin2->m_cbStreamData);
        SLOG(eInfo2, "Pin1 PlayOffset = %I64d,  Pin2  PlayOffset= %I64d", pos1.PlayOffset, pos2.PlayOffset);
        SLOG(eInfo2, "Pin1 WriteOffset = %I64d,  Pin2  WriteOffset= %I64d", pos1.WriteOffset, pos2.WriteOffset);

    }
    // play upto the end of the buffer
    SLOG(eInfo2, "Pin1 = %d,  Pin2 = %d", pPin1->m_cbStreamData, pPin2->m_cbStreamData);

    while ((pos1.PlayOffset != pPin1->m_cbStreamData) && (pos2.PlayOffset != pPin2->m_cbStreamData))
    {
        tpos1.PlayOffset = pos1.PlayOffset;
        tpos2.PlayOffset = pos2.PlayOffset;

        Sleep(10);
        fRes = pPin1->GetPosition(&pos1);
        if (!fRes)
        {
            throw("    Failure:  Failed to get position on the first pin");
        }

        fRes = pPin2->GetPosition(&pos2);
        if (!fRes)
        {
            throw("    Failure:  Failed to get position on the second pin");
        }

        SLOG(eInfo2, "pin position pos1.playoffset = %d , pos2.playOffset = %d ", (ULONG)pos1.PlayOffset, (ULONG)pos2.PlayOffset);
        if (((tpos1.PlayOffset == pos1.PlayOffset) && (pos1.PlayOffset != pPin1->m_cbStreamData)) || ((tpos2.PlayOffset == pos2.PlayOffset) && (pos2.PlayOffset != pPin2->m_cbStreamData)))
        {
            SLOG(eError, "Position same as previous position recorded Pos1 = %I64d,  Pos2 = %I64d", pos1.PlayOffset, pos2.PlayOffset);
            break;

        }
        if (!fRes)
        {
            throw("    Failure:  Call for getting KSPROPERTY_AUDIO_POSITION failed");
        }
        if (fLooped)
        {
            if ((ULONG)(abs((LONG)pos1.PlayOffset - (LONG)tpos1.PlayOffset)) > (pPin1->m_cbStreamData / 2))
                fLoop1 = TRUE;
            if ((ULONG)(abs((LONG)pos2.PlayOffset - (LONG)tpos2.PlayOffset)) > (pPin2->m_cbStreamData / 2))
                fLoop2 = TRUE;
            if (fLoop1 || fLoop2)
            {
                SLOG(eInfo2, "For loop streaming we have detected loop");
                break;
            }
        }

    }

    if ((fLooped) && (fLoop1 || fLoop2))
        return FNS_PASS;
    if ((pos1.PlayOffset < (ULONGLONG)(pPin1->m_cbStreamData)) || (pos2.PlayOffset < (ULONGLONG)(pPin2->m_cbStreamData))) {
        SLOG(eError, "PlayOffset is < PinData");
        SLOG(eError, "Pos1 = %I64d,  Pos2 = %I64d", pos1.PlayOffset, pos2.PlayOffset);
        throw("    Failure:  Error in streaming position");
        return FNS_FAIL;
    }
    else
        return FNS_PASS;
}

// ------------------------------------------------------------------------------
// Test_StreamingMultiPin -  Creating 2 instances of the pin and then stream data - this test case is for render pin only 
// ------------------------------------------------------------------------------
// TODO: This test case is written for only standard streaming and needs to be modify for the loop streaming
DWORD
Test_StreamingMultiPin(void)
{
    DWORD           dwRes = FNS_FAIL;
    BOOL            fRes = FALSE;
    CPCMAudioPin*   pPin = g_pWaveTest->m_pHalf->m_pPin;
    CKsFilter*      pFilter = g_pWaveTest->m_pHalf->m_pFilter;      // used to find out that Filter is corresponding to USB or not
    BOOL            fLooped = g_pWaveTest->m_fLoop;
    bool            fRTPin = g_pWaveTest->m_pHalf->IsWaveRTPin();
    WAVEFORMATEX*   pWfxIn = &g_pWaveTest->m_pHalf->m_pFmtCurrent->wfxFormat.Format;
    CPCMAudioPin*   pPin2 = NULL;

    pPin2 = ((CPCMAudioFilter*)pFilter)->BuildPin(pPin->m_nId);

    if (!g_pWaveTest->m_pHalf->IsTestSupportedForDeviceType(fLooped))
    {
        XLOG(XSKIP, eError, "Test is not supported for the current device type. (fLooped = %d)", fLooped);
        return FNS_SKIPPED;
    }

    if (fLooped && IsPinOffloadCapable(pPin))
    {
        XLOG(XSKIP, eError, "Test is not supported for the current device type. (Looped test doesn't support offload pin)");
        return FNS_SKIPPED;
    }

    if (!IsEqualGUID(g_pWaveTest->m_pHalf->m_Mode, GUID_NULL))
    {
        XLOG(XSKIP, eError, "Test is not supported for the current device type. (MultiPin test doesn't support Modes)");
        return FNS_SKIPPED;
    }

    // USB doesn't support looping.
    if (fLooped && pFilter && pFilter->m_UserDef)
    {
        SLOG(eInfo1, "USB filter does not support looped streaming.");
        return FNS_SKIPPED;
    }

    // (Pin2) Failed creating pPin2
    if (pPin2 == NULL)
    {
        SLOG(eError, "Failed to create of pPin2 for 2nd Instance.");
        return FNS_FAIL;
    }

    try
    {
        BOOL bRawSupport = false;
        BOOL bDefaultSupport = false;

        //CheckProcessing modes may not succeed; Ignore and just instantiate pins- We don't have modes supported info
        //But that is ok
        fRes = pPin->CheckProcessingModes(&bRawSupport, &bDefaultSupport);
        SLOG(eInfo1, "Going to prepare pin. Pin mode support:%s, %s", bRawSupport == TRUE ? "RAW  MODE" : "NO RAW", bDefaultSupport == TRUE ? "DEFAULT MODE" : "NO DEFAULT");

        fRes = g_pWaveTest->m_pHalf->PreparePin(pWfxIn, fLooped, fRTPin, BUFLEN_MS, bRawSupport);
        if (!fRes)
            throw("Failed to prepare pin for the streaming which is required for the test case");

        if (g_pWaveTest->m_pHalf->IsWaveRTPacketStreamingSupported())
        {
            XLOG(XSKIP, eError, "The test doesn't support packet based pin.");
            dwRes = FNS_SKIPPED;
            goto EXIT;
        }

        ULONG   cPossible = pPin->m_Descriptor.CInstancesGlobal.PossibleCount;
        BOOL    fExpectDupSuccess = (cPossible > 1);
        SLOG(eInfo1, "Given the possible instance count (%d) of the render pin, a second attempt to instantiate the pin should %s",
            cPossible, fExpectDupSuccess ? "SUCCEED" : "FAIL");
        if (!fExpectDupSuccess) {
            SLOG(eInfo1, "Second instance of the pin is not possible. So exiting test case");
            dwRes = FNS_PASS;
            goto EXIT;
        }

        BOOL fRes2 = FALSE;
        // create second pin
        // TODO: for RT port this will be different as we have to send property request to have buffer for the pin

        fRes = CreateSecondInstanceOfPin(pPin2, bDefaultSupport, fLooped);
        if (!fRes)
        {
            throw("Failed to create second instance of pin");
        }

        //submit data to the kmixer sink pin 
        SLOG(eInfo2, "Submitting %d bytes to the pin1", pPin->m_cbStreamData);
        SLOG(eInfo2, "Submitting %d bytes to the pin2", pPin2->m_cbStreamData);
        if (fRTPin)
        {
            // in case of RT port, it does not support IOCTL_KS_WRITE_STREAM and 	IOCTL_KS_READ_STREAM
            fRes = pPin->SetState(KSSTATE_PAUSE);
            fRes2 = pPin2->SetState(KSSTATE_PAUSE);
        }
        else
        {
            fRes = pPin->WriteData();
            fRes2 = pPin2->WriteData();
        }

        // first pin
        if (!fRes)
        {
            throw("Failed to submit buffer to pin");
        }

        // second pin
        if (!fRes2)
        {
            throw("Failed to submit buffer to the second pin");
        }

        // set pin into run state so that main thread start streaming data 
        fRes = pPin->SetState(KSSTATE_RUN);
        if (!fRes)
            throw("Failed to set pin into run state");

        // set pin into run state so that main thread start streaming data 
        fRes = pPin2->SetState(KSSTATE_RUN);
        if (!fRes)
            throw("Failed to set pin into run state");

        dwRes = StreamMultiPin(pPin, pPin2);

    }
    // handle bad things
    catch (char* str)
    {
        XLOG(XFAIL, eError, "FAIL: %s", str);
        dwRes = FNS_FAIL;
    }

EXIT:
    // clean up
    if (pPin)
    {
        pPin->SetState(KSSTATE_STOP);
        pPin->ClosePin();
    }
    if (pPin2)
    {
        pPin2->SetState(KSSTATE_STOP);
        pPin->ClosePin();
        delete pPin2;
    }
    //SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
    return dwRes;
}

// ------------------------------------------------------------------------------
// Test_StreamingMultiPin -  Creating 2 instances of the pin and then stream data - this test case is for render pin only 
// ------------------------------------------------------------------------------
// TODO: This test case is written for only standard streaming and needs to be modify for the loop streaming
DWORD Test_StreamingMultiPinDifferentFormat(void)
{
    HRESULT         hr = S_OK;
    DWORD           errorCode = ERROR_SUCCESS;
    DWORD           dwRes = FNS_FAIL;
    BOOL            fRes = FALSE;
    CPCMAudioPin*   pPin = g_pWaveTest->m_pHalf->m_pPin;
    CKsFilter*      pFilter = g_pWaveTest->m_pHalf->m_pFilter;      // used to find out that Filter is corresponding to USB or not
    WAVEFORMATEX*   pwfx = &g_pWaveTest->m_pHalf->m_pFmtCurrent->wfxFormat.Format;
    BOOL            fLooped = g_pWaveTest->m_fLoop;
    bool            fRTPin = g_pWaveTest->m_pHalf->IsWaveRTPin();
    WAVEFORMATEX*   pWfxIn = &g_pWaveTest->m_pHalf->m_pFmtCurrent->wfxFormat.Format;
    CPCMAudioPin*   pPin2 = NULL;
    BOOL            fAtLeastOneFormatFound = FALSE;

    //Check error code
    CComPtr<IBasicLog> pBasicLog;

    if (fLooped && IsPinOffloadCapable(pPin))
    {
        XLOG(XSKIP, eError, "Test is not supported for the current device type.");
        return FNS_SKIPPED;
    }

    if (!IsEqualGUID(g_pWaveTest->m_pHalf->m_Mode, GUID_NULL))
    {
        XLOG(XSKIP, eError, "Test is not supported for the current device type. (MultiPin test doesn't support Modes)");
        return FNS_SKIPPED;
    }

    // USB doesn't support looping.
    if (fLooped && pFilter && pFilter->m_UserDef)
    {
        SLOG(eInfo1, "USB filter does not support looped streaming.");
        return FNS_SKIPPED;
    }

    if (!g_pWaveTest->m_pHalf->IsTestSupportedForDeviceType(fLooped))
    {
        XLOG(XSKIP, eError, "Test is not supported for the current device type.");
        return FNS_SKIPPED;
    }

    if (ERROR_SUCCESS != errorCode)
    {
        XLOG(XFAIL, eError, "Failed creating pin. (error=%u)", errorCode);
        return FNS_FAIL;
    }

    try
    {
        BOOL bRawSupport = false;
        BOOL bDefaultSupport = false;
        XLOG(XMSG, eInfo1, "Going to check processing modes");
        //CheckProcessing modes may not succeed; Ignore and just instantiate pins- We don't have modes supported info
        //But that is ok
        fRes = pPin->CheckProcessingModes(&bRawSupport, &bDefaultSupport);
        SLOG(eInfo1, "Going to prepare pin. Pin mode support:%s, %s", bRawSupport == TRUE ? "RAW  MODE" : "NO RAW", bDefaultSupport == TRUE ? "DEFAULT MODE" : "NO DEFAULT");

        fRes = g_pWaveTest->m_pHalf->PreparePin(pWfxIn, fLooped, fRTPin, BUFLEN_MS, bRawSupport);
        if (!fRes)
            throw("Failed to prepare pin for the streaming which is required for the test case");

        if (g_pWaveTest->m_pHalf->IsWaveRTPacketStreamingSupported())
        {
            XLOG(XSKIP, eError, "The test doesn't support packet based pin.");
            dwRes = FNS_SKIPPED;
            goto EXIT;
        }

        ULONG   cPossible = pPin->m_Descriptor.CInstancesGlobal.PossibleCount;
        BOOL    fExpectDupSuccess = (cPossible > 1);
        SLOG(eInfo1, "Given the possible instance count (%d) of the render pin, a second attempt to instantiate the pin should %s", cPossible, fExpectDupSuccess ? "SUCCEED" : "FAIL");
        if (!fExpectDupSuccess) {
            SLOG(eInfo1, "Second instance of the pin is not possible. So exiting test case");
            dwRes = FNS_PASS;
            goto EXIT;
        }

        PKSMULTIPLE_ITEM pRanges = NULL;
        pRanges = pPin->m_Descriptor.pmiDataRanges;
        if (!pRanges) {  // means we do not have any information about dataranges supported by the pin so we wil not run this test case.
            SLOG(eInfo1, "we do not have any information about dataranges supported by the pin so we wil not run this test case");
            goto EXIT;
        }
        // get the details of the dataranges form the pin descriptor
        PKSDATARANGE pDataRange = (PKSDATARANGE)(pPin->m_Descriptor.pmiDataRanges + 1);
        pDataRange = (PKSDATARANGE)((ULONG_PTR(pDataRange) + (ULONG_PTR)7) & ~(ULONG_PTR)7);
        PKSDATARANGE_AUDIO pKsDatarangeAudio;
        WAVEFORMATEXTENSIBLE wfxExt = g_pWaveTest->m_pHalf->m_pFmtCurrent->wfxFormat;
        DataRangeInfo DataFormatInfo;
        BOOL fRange = FALSE;
        bool thisIsAttributeList = false;
        bool nextIsAttributeList = false;
        for (ULONG i = 0; i < pRanges->Count; i++) { //
            thisIsAttributeList = nextIsAttributeList;
            nextIsAttributeList = false;

            // ignore attribute lists
            if (thisIsAttributeList) {
                goto next_range;
            }

            if (pDataRange->Flags & KSDATARANGE_ATTRIBUTES) {
                nextIsAttributeList = true;
            }

            // Chceking for the PCM Format first 
            if (IsEqualGUIDAligned(pDataRange->SubFormat, KSDATAFORMAT_SUBTYPE_PCM) &&
                IsEqualGUIDAligned(pDataRange->Specifier, KSDATAFORMAT_SPECIFIER_WAVEFORMATEX) &&
                (pDataRange->FormatSize == sizeof(KSDATARANGE_AUDIO))) {
                // range reported by the driver is of KSDATARANGE_AUDIO
                pKsDatarangeAudio = (PKSDATARANGE_AUDIO)pDataRange;
                // check that this format is supported by the driver. If so that try to chage existing format with this format
                for (DWORD dwFormat = MSB_FORMAT; dwFormat; dwFormat /= 2) {
                    fRange = CheckFormatForRange(dwFormat, pKsDatarangeAudio, &DataFormatInfo);
                    if (fRange)
                    {
                        wfxExt.Format.nChannels = DataFormatInfo.Channels;
                        wfxExt.Format.wBitsPerSample = DataFormatInfo.BitsPerSample;
                        wfxExt.Format.nSamplesPerSec = DataFormatInfo.SampleFrequency;
                        if (pPin->DoesFormatIntersect((WAVEFORMATEX*)&wfxExt, FALSE))
                        {
                            // Workaround for HFP. Turns out that in the format enumeration we don't have any formats less than 11kHz
                            fAtLeastOneFormatFound = TRUE;
                            //// create second instance of the pin with this format
                            // create second pin
                            // TODO: for RT port this will be different as we have to send proeprty request to have buffer for the pin
                            if (pPin2) delete pPin2;
                            pPin2 = ((CPCMAudioFilter*)pFilter)->BuildPin(pPin->m_nId);
                            SLOG(eInfo2, "Attempting new format");
                            //fRes = CreatePin_WAVEFORMATEXTENSIBLE(pPin2,NULL,&wfxExt,fLooped);
                            fRes = InstantiatePinWfx(pPin2, (WAVEFORMATEX*)&wfxExt, fLooped, 0, bDefaultSupport);
                            if (fRes) {
                                // prepare second pin
                                // allocate data
                                //Lst parameter to CreateDataBuffer is Address alignment
                                fRes = pPin2->CreateDataBuffer(BUFLEN_MS, wfxExt.Format.nBlockAlign, 16);
                                if (!fRes) {
                                    DWORD dwErr = GetLastError();
                                    SLOG(eError, "Could not create data buffer for pin - GetLastError() returned %d (0x%08x)", dwErr, dwErr);
                                    throw("Could not create data buffer for pin");
                                }
                                if (!pPin2->m_pbStreamData)
                                    throw("CreateDataBuffer Failed for pin2.");
                                // make a sine tone
                                // get a logger
                                pBasicLog = g_pBasicLog;

                                hr = FillBufferWithSineSignal(
                                    pBasicLog,
                                    XFAIL,
                                    pwfx,
                                    TEST_AMPLITUDE,
                                    TEST_FREQUENCY,
                                    0.0, // initial phase
                                    0.0, // dc
                                    Method_NoDithering,
                                    pPin2->m_pbStreamData,
                                    pPin2->m_cbStreamData / pwfx->nBlockAlign,
                                    pPin2->m_cbStreamData
                                    );
                                if (FAILED(hr))
                                    throw("FillBufferWithSineSignal failed");

                                // we have now created 2 instances of the pin and now set pin to run state
                                //submit data to the kmixer sink pin 
                                SLOG(eInfo2, "Submitting %d bytes to the 1st intance of pin and %d bytes to 2nd instance of the pin", pPin->m_cbStreamData, pPin2->m_cbStreamData);
                                BOOL fRes2 = FALSE;
                                if (fRTPin)
                                {
                                    // in case of RT port, it does not support IOCTL_KS_WRITE_STREAM and 	IOCTL_KS_READ_STREAM
                                    fRes = pPin->SetState(KSSTATE_PAUSE);
                                    fRes2 = pPin2->SetState(KSSTATE_PAUSE);
                                }
                                else
                                {
                                    fRes = pPin->WriteData();
                                    fRes2 = pPin2->WriteData();
                                }

                                // first pin
                                if (!fRes)
                                {
                                    throw("Failed to submit buffer to pin");
                                }

                                // second pin
                                if (!fRes2)
                                {
                                    throw("Failed to submit buffer to the second pin");
                                }

                                // set pin into run state so that main thread start streaming data 
                                fRes = pPin->SetState(KSSTATE_RUN);
                                if (!fRes)
                                    throw("Failed to set pin into run state");

                                // set pin into run state so that main thread start streaming data 
                                fRes = pPin2->SetState(KSSTATE_RUN);
                                if (!fRes)
                                    throw("Failed to set pin into run state");

                                dwRes = StreamMultiPin(pPin, pPin2);
                                if (dwRes == FNS_FAIL)
                                    goto EXIT;
                                pPin->SetState(KSSTATE_STOP);
                                pPin2->SetState(KSSTATE_STOP);
                                delete pPin2;
                                pPin2 = NULL;
                            } // if res
                        } // if intersect
                    } // if range
                } // for format
            } // if PCM
        next_range:
            ULONG size = (thisIsAttributeList ? ((PKSMULTIPLE_ITEM)pDataRange)->Size : pDataRange->FormatSize);
            pDataRange = (PKSDATARANGE)((ULONG_PTR(pDataRange) + size + (ULONG_PTR)7) & ~(ULONG_PTR)7);
        } // for format
    } // try

      // handle bad things
    catch (char* str)
    {
        XLOG(XFAIL, eError, "FAIL: %s", str);
        dwRes = FNS_FAIL;
    }
EXIT:
    // Test gets a free pass if we did not find a format to try on the second pin
    if (!fAtLeastOneFormatFound)
    {
        dwRes = FNS_SKIPPED;
        SLOG(eInfo1, "No format was found suitable for the second pin.");
    }
    // clean up
    if (pPin)
    {
        pPin->SetState(KSSTATE_STOP);
        pPin->ClosePin();
    }
    if (pPin2)
    {
        pPin2->SetState(KSSTATE_STOP);
        pPin2->ClosePin();
        delete pPin2;
    }
    //SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
    return dwRes;
}

//-------------------------------------------------------------------------
// Class: 
// Method: Test_PinInstantiateCheck
//
// Description:
//  Check if maximum number of pins instantiated, the appropriate error
//  values are returned for more instantiations
//
// Parameters:
//
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//-------------------------------------------------------------------------
DWORD Test_PinInstantiateCheck()
{
    DWORD           dwRet = FNS_PASS;
    CPCMAudioPin   *pPin = g_pWaveTest->m_pHalf->m_pPin;
    CPCMAudioFilter*pFilter = NULL;
    HANDLE         *pPinHandles = NULL;
    BOOL            fLooped = FALSE;
    ULONG           uPossInstances = 0, //Possible number of instances on pin
        uCurrInstances = 0, //Current  number of instances on pin
        ii = 0;

    if (!IsEqualGUID(g_pWaveTest->m_pHalf->m_Mode, GUID_NULL))
    {
        XLOG(XSKIP, eError, "Test is not supported for different modes.");
        return FNS_SKIPPED;
    }

    if (pPin)
    {
        pFilter = reinterpret_cast<CPCMAudioFilter*>(pPin->m_pFilter);
    }
    if (!pFilter)
    {
        XLOG(XFAIL, eError, "Unable to get pointer to Pin or Filter structures.");
        return FNS_FAIL;
    }

    BOOL bOffload = IsPinOffloadCapable(pPin);
    // Check that the number of possible and current instances are logical
    if (pPin->m_Descriptor.CInstancesGlobal.PossibleCount >= pPin->m_Descriptor.CInstances.PossibleCount)
    {
        uPossInstances = pPin->m_Descriptor.CInstancesGlobal.PossibleCount;
        uCurrInstances = pPin->m_Descriptor.CInstancesGlobal.CurrentCount;
    }
    else
    {
        uPossInstances = pPin->m_Descriptor.CInstances.PossibleCount;
        uCurrInstances = pPin->m_Descriptor.CInstances.CurrentCount;
    }
    if (0 == uPossInstances)
    {
        XLOG(XFAIL, eError, "ERROR: Pin %d advertises 0 possible instance(s)."
            " There must be at least one possible instance for the pin.",
            pPin->m_nId);
        dwRet = FNS_FAIL;
        goto exit;
    }
    if (uPossInstances < uCurrInstances)
    {
        XLOG(XFAIL, eError, "ERROR: Pin %d advertises %d possible instance(s) "
            "and has %d current instances.", pPin->m_nId, uPossInstances, uCurrInstances);
        dwRet = FNS_FAIL;
        goto exit;
    }

    // Create array to track pin handles
    // uPossInstances - uCurrPins + 1 (should always be at least 1)
    pPinHandles = new HANDLE[uPossInstances - uCurrInstances + 1];
    if (NULL == pPinHandles)
    {
        XLOG(XFAIL, eError, "Unable to allocate memory for array of Pin Handles.");
        dwRet = FNS_FAIL;
        goto exit;
    }
    for (ii = 0; ii < (uPossInstances - uCurrInstances + 1); ++ii)
    {
        pPinHandles[ii] = NULL;
    }

    // Setup pin instantiation
    fLooped = pFilter->m_fRTPort; //In case of RT port, we always specify Looped streaming

                                  //check for processing modes
    BOOL bRawSupport = FALSE;
    BOOL bDefaultSupport = FALSE;
    pPin->CheckProcessingModes(&bRawSupport, &bDefaultSupport);

    //NON-OFFLOAD with Mode Support
    if (FALSE == bOffload && (TRUE == bDefaultSupport || TRUE == bRawSupport))
    {
        HANDLE hDefaultPinHandle, hDefaultErrorHandle;
        HANDLE hRawHandle, hRawErrorHandle;

        // Instantiate two of each mode supported, second should always fail
        if (TRUE == bDefaultSupport)
        {
            XLOG(XMSG, eInfo1, "Instantiating instance 1 of DEFAULT mode on Pin %d",
                pPin->m_nId);
            if (!InstantiatePinWfx(pPin, NULL, fLooped, FALSE, bDefaultSupport))
            {
                XLOG(XFAIL, eError, "ERROR: Non-Offload Pin %d should have 1 instance of a DEFAULT mode, but has 0.", pPin->m_nId);
                dwRet = FNS_FAIL;
                goto exit;
            }
            hDefaultPinHandle = pPin->m_handle;

            //open second pin instance, it should fail with insufficient resources
            XLOG(XMSG, eInfo1, "Instantiating instance 2 of DEFAULT mode on Pin %d",
                pPin->m_nId);
            if (InstantiatePinWfx(pPin, NULL, fLooped, FALSE, bDefaultSupport))
            {
                hDefaultErrorHandle = pPin->m_handle;
                XLOG(XFAIL, eError, "ERROR: Non-Offload Pin %d has more than 1 instance available of DEFAULT mode",
                    pPin->m_nId);
                dwRet = FNS_FAIL;
                CloseHandle(hDefaultErrorHandle);
            }
            else
            {
                XLOG(XMSG, eInfo1, "The in-ability to instantiate 2 DEFAULT instances is expected and valid.");
            }
            CloseHandle(hDefaultPinHandle);
        }

        if (TRUE == bRawSupport)
        {
            XLOG(XMSG, eInfo1, "Instantiating instance 1 of RAW mode on Pin %d",
                pPin->m_nId);
            if (!InstantiatePinWfx(pPin, NULL, fLooped, bRawSupport, FALSE))
            {
                XLOG(XFAIL, eError, "ERROR: Non-Offload Pin %d should have 1 instance of a RAW mode, but has 0.", pPin->m_nId);
                dwRet = FNS_FAIL;
                goto exit;
            }
            hRawHandle = pPin->m_handle;

            //open second pin instance, it should fail with insufficient resources
            XLOG(XMSG, eInfo1, "Instantiating instance 2 of RAW mode on Pin %d",
                pPin->m_nId);
            if (InstantiatePinWfx(pPin, NULL, fLooped, bRawSupport, FALSE))
            {
                hRawErrorHandle = pPin->m_handle;
                XLOG(XFAIL, eError, "ERROR: Non-Offload Pin %d has more than 1 instance available of RAW mode",
                    pPin->m_nId);
                dwRet = FNS_FAIL;
                CloseHandle(hRawErrorHandle);
            }
            else
            {
                XLOG(XMSG, eInfo1, "The in-ability to instantiate 2 RAW instances is expected and valid.");
            }
            CloseHandle(hRawHandle);
        }

        if (TRUE == bRawSupport && TRUE == bDefaultSupport)
        {
            //open two of each
            XLOG(XMSG, eInfo1, "Instantiating instance 1 of DEFAULT mode and instance 1 of RAW mode on Pin %d",
                pPin->m_nId);
            if (!InstantiatePinWfx(pPin, NULL, fLooped, FALSE, bDefaultSupport))
            {
                XLOG(XFAIL, eError, "ERROR: Pin %d should have 1 instance of a DEFAULT mode, but has 0.", pPin->m_nId);
                dwRet = FNS_FAIL;
                goto exit;
            }
            hDefaultPinHandle = pPin->m_handle;

            if (!InstantiatePinWfx(pPin, NULL, fLooped, bRawSupport, FALSE))
            {
                XLOG(XFAIL, eError, "ERROR: Pin %d should have 1 instance of a RAW mode, but has 0.", pPin->m_nId);
                dwRet = FNS_FAIL;
                goto exit;
            }
            hRawHandle = pPin->m_handle;

            //open second pin instance, it should fail with insufficient resources
            XLOG(XMSG, eInfo1, "Instantiating instance 2 of DEFAULT mode and instance 2 of RAW mode on Pin %d",
                pPin->m_nId);
            if (InstantiatePinWfx(pPin, NULL, fLooped, FALSE, bDefaultSupport))
            {
                hDefaultErrorHandle = pPin->m_handle;
                XLOG(XFAIL, eError, "ERROR: Pin %d has more than 1 instance available of DEFAULT mode",
                    pPin->m_nId);
                dwRet = FNS_FAIL;
                CloseHandle(hDefaultErrorHandle);
            }
            else
            {
                XLOG(XMSG, eInfo1, "The in-ability to instantiate 2 DEFAULT instances is expected and valid.");
            }

            if (InstantiatePinWfx(pPin, NULL, fLooped, bRawSupport, FALSE))
            {
                hRawErrorHandle = pPin->m_handle;
                XLOG(XFAIL, eError, "ERROR: Pin %d has more than 1 instance available of RAW mode",
                    pPin->m_nId);
                dwRet = FNS_FAIL;
                CloseHandle(hRawErrorHandle);
            }
            else
            {
                XLOG(XMSG, eInfo1, "The in-ability to instantiate 2 RAW instances is expected and valid.");
            }
            CloseHandle(hDefaultPinHandle);
            CloseHandle(hRawHandle);
        }

        return dwRet;
    }
    else if (FALSE == bOffload) //NON-OFFLOAD
    {
        // Instantiate as many as possible pins
        XLOG(XMSG, eInfo1, "Non-Offload,Non-Mode-Aware Pin %d advertises %d possible instance(s) "
            "and currently has %d open instance(s).", pPin->m_nId, uPossInstances, uCurrInstances);

        for (ii = 0; ii < (uPossInstances - uCurrInstances); ++ii)
        {
            if (!InstantiatePinWfx(pPin, NULL, fLooped))
            {
                XLOG(XFAIL, eError, "ERROR: Pin %d advertises %d possible instance(s)"
                    " but actually only allows %d.", pPin->m_nId, uPossInstances, ii);
                dwRet = FNS_FAIL;
                goto exit;
            }
            pPinHandles[ii] = pPin->m_handle;
        }
        XLOG(XMSG, eInfo1, "Testing extra pin instantiation on NON-OFFLOAD PIN.");
        // Instantiate an extra pin to see if it generates the correct error
        if (InstantiatePinWfx(pPin, NULL, fLooped))
        {
            pPinHandles[uPossInstances - uCurrInstances] = pPin->m_handle;
            XLOG(XFAIL, eError, "ERROR: Pin %d advertises %d possible instance(s)"
                " but actually allows more instances.", pPin->m_nId, uPossInstances);
            dwRet = FNS_FAIL;
            goto exit;
        }
        else
        {
            XLOG(XMSG, eInfo1, "The in-ability to instantiate NON-OFFLOAD pin beyon max instances is expected and valid.");
        }
    }
    else //OFFLOAD
    {
        // Instantiate as many as possible pins
        XLOG(XMSG, eInfo1, "Offload Pin %d advertises %d possible instance(s) "
            "and currently has %d open instance(s).", pPin->m_nId, uPossInstances, uCurrInstances);

        for (ii = 0; ii < (uPossInstances - uCurrInstances); ++ii)
        {
            if (!InstantiatePinWfx(pPin, NULL, fLooped))
            {
                XLOG(XFAIL, eError, "ERROR: Pin %d advertises %d possible instance(s)"
                    " but actually only allows %d.", pPin->m_nId, uPossInstances, ii);
                dwRet = FNS_FAIL;
                goto exit;
            }
            pPinHandles[ii] = pPin->m_handle;
        }
        // OFFLOAD PIN
        XLOG(XMSG, eInfo1, "Testing extra pin instantiation on OFFLOAD PIN");

        // we expect that the device will not find a format
        DWORD dwIOCTLRet = 0;
        if (!pFilter->SetPinProposedFormat(pPin->m_nId, pPin->m_pksDataFormat, pPin->GetKSDataFormatSize(), &dwIOCTLRet))
        {
            //this is what we want, make sure the value is proper
            if (ERROR_NO_SYSTEM_RESOURCES == dwIOCTLRet)
            {
                goto exit;
            }
            else
            {
                XLOG(XFAIL, eError, "ERROR: Pin %d failed SetProposedFormat with error: %d. Expected ERROR_NO_SYSTEM_RESOURCES(1450)",
                    pPin->m_nId, dwIOCTLRet);
                dwRet = FNS_FAIL;
                goto exit;
            }

        }
        else
        {
            XLOG(XFAIL, eError, "SetProposedFormat succeeded on Pin: %d unexpectedly on instance:(%d). Advertised instance count:(%d)",
                pPin->m_nId, uPossInstances + 1, uPossInstances);

            XLOG(XMSG, eInfo1, "Wave Format used for instantiation:");
            CComPtr<IBasicLog> pShellBasicLog;

            pShellBasicLog = g_pBasicLog;
            LogWaveFormat(pShellBasicLog, pPin->GetFormat());

            if (TRUE == pPin->Instantiate(fLooped))
            {
                pPinHandles[uPossInstances - uCurrInstances] = pPin->m_handle;
                XLOG(XFAIL, eError, "ERROR: Pin %d advertises %d possible instance(s)"
                    " but actually allows more instances.", pPin->m_nId, uPossInstances);
            }
            else
            {
                XLOG(XFAIL, eError, "ERROR: Pin %d instance(%d) passed format negotiation"
                    " but failed pin instantiation.", pPin->m_nId, uPossInstances + 1);
            }
            dwRet = FNS_FAIL;
            goto exit;
        }
    }

exit:
    if (NULL != pPinHandles)
    {
        for (ii = 0; ii < uPossInstances - uCurrInstances + 1; ++ii)
        {
            if (NULL != pPinHandles[ii])
            {
                CloseHandle(pPinHandles[ii]);
            }
        }
        delete[] pPinHandles;
    }

    return dwRet;
}


//-------------------------------------------------------------------------
// Class: 
// Method: RenderCaptureDataIntersection
//
// Description:
//  Test RenderCaptureDataIntersection on Render/Capture pin
//
// Parameters:
//      fRender: TRUE for render test.
//               FALSE for capture test.
//      fCommin: TRUE for communication device
//               FALSE for other device
//      fBth : TRUE for bluetooth device
//
// Return values (DWORD):
//     Return FNS_PASS. The return value is irrelevant is errors are
//     roled up using XLOG.
//-------------------------------------------------------------------------
DWORD RenderCaptureDataIntersection(BOOL fRender, BOOL fCommun, BOOL fBth = FALSE)
{
    BOOL                        fRes = TRUE;
    UINT                        ii = 0, cWF = 0;
    CPCMAudioPin               *pPin = g_pWaveTest->m_pHalf->m_pPin;
    bool                        fIsWaveRTPin = g_pWaveTest->m_pHalf->m_fIsWaveRTPin;
    bool                        fIsStdStrmPin = g_pWaveTest->m_pHalf->m_fIsStdStrmPin;
    bool                        fIsStdLoopedStrmPin = g_pWaveTest->m_pHalf->m_fIsStdLoopedStrmPin;
    CComPtr<IWaveFormatEnum>    pWF;
    CDataIntersectionTest       Test;

    XLOG(XMSG, eInfo1, "---------------------------------------------------------------------\n"
        "Running test for all combinations of: \n"
        "  1. Sample Rates: 8, 11.025 16, 22.1, 32, 44.1, 48, 96, 192, 384 KHz\n"
        "  2. Sample Sizes: 8, 16, 20, 24, 32 bits\n"
        "  3. Channel count: 1 till 12 channels\n"
        "---------------------------------------------------------------------\n");

    // Enumerate Wave Formats
    if (FAILED(CreateWaveFormatEnum(&pWF)) ||
        FAILED(pWF->AddPCMFormats(IWaveFormatEnum::WF_BIT_ALL, IWaveFormatEnum::WF_SAMPLE_ALL,
            IWaveFormatEnum::WF_CHANNEL_ALL, IWaveFormatEnum::use_both)))
    {
        fRes = false;
        XLOG(XFAIL, eError, "FAIL: Unable to enumerate Wave Formats.");
        goto exit;
    }

    if (!Test.Initialize(pPin, fIsWaveRTPin, fIsStdStrmPin, fIsStdLoopedStrmPin))
    {
        XLOG(XFAIL, eError, "FAIL: Failed to Initialize Test object");
        goto exit;
    }

    //For each PCM format
    for (ii = 0, cWF = pWF->GetCount(); ii < cWF; ++ii)
    {
        //Create render/capture Intersection for current format
        if (fRender)
        {
            fRes = Test.CreateRender_Intersection(pWF->GetAt(ii));
            if (fRes)
            {
                fRes = Test.Render(); //Render audio
                if (!fRes)
                {
                    XLOG(XFAIL, eError, "FAIL: Failed to render audio on an instantiated pin.");
                    continue;
                }
                Test.DestroyRender(); //Cleanup
            }
        }
        else
        {
            fRes = Test.CreateCapture_Intersection(pWF->GetAt(ii));
            if (fRes)
            {
                fRes = Test.Capture(); //Capture audio
                if (!fRes)
                {
                    XLOG(XFAIL, eError, "FAIL: Failed to capture audio on an instantiated pin.");
                    continue;
                }
                Test.DestroyCapture(); //Cleanup
            }
        }

        if (!fRes)
        {
            //Could not find intersection
            if (Test.DoesIntersect())
            {
                XLOG(XFAIL, eError, "FAIL: KSPROPERTY_PIN_DATAINTERSECTION Succeeded, Pin Creation Failed");
            }//if(!fRes), could not find intersection
        }

    }//for(UINT ii = 0; ii < uiCountPCMFormats; ii++)

exit:
    return FNS_PASS; // The return value is irrelevant

}//DWORD RenderCaptureDataIntersection()

 // Get the collection of endpoints associated with the current device (pin) under test
HRESULT GetCurrentEndpointsUnderTest(IMMDeviceCollection **ppEndpointCollection)
{
    HRESULT hr = S_OK;
    if (NULL == ppEndpointCollection) { return E_POINTER; }

    CPCMAudioPin *pPin = g_pWaveTest->m_pHalf->m_pPin; // current pin under test
    CKsFilter *pFilter = g_pWaveTest->m_pHalf->m_pFilter; // current filter under test

    CComPtr<IMMDevice> pDevInterface;
    hr = DevicePathToInterfaceA(pFilter->m_szFilterName, &pDevInterface);
    if (FAILED(hr))
    {
        XLOG(XWARN, eError, "Failed to get Interface from device path %s (hr = %s)", pFilter->m_szFilterName, GetHRString(hr));
        return hr;
    }

    // - We should NOT enumerate endpoints that are removed, i.e. DEVICE_STATE_NOTPRESENT. Those are not valid for testing.
    // - We should enumerate disabled devices such as Stereo Mix, CD In, Aux, etc.
    // - We theoretically should not see unplugged endpoint on the system because wave test would have failed earlier before
    // loading a test case. But it doesn't hurt to add it here.
    // - We are always interested in active endpoints. 
    hr = KSStreamPinToEndpoints(pDevInterface, pPin->m_nId, DEVICE_STATE_ACTIVE | DEVICE_STATE_UNPLUGGED | DEVICE_STATE_DISABLED, ppEndpointCollection, true);
    if (FAILED(hr))
    {
        XLOG(XWARN, eError, "Failed to get endpoints from pin %d on device path %s (hr = %s)", pPin->m_nId, pFilter->m_szFilterName, GetHRString(hr));
        return hr;
    }

    return S_OK;
}

// Check if a device is a commnunication device, because it has different requirement.
// Howerer, each pin can map to more than one endpoint. 
// In this case, we would consider it not a communication device
// unless all of the endpoints for the same pin are communication endpoints.
// Assumption: endpoints on the same pin have a single SRC, which means their supported formats would be the same.
// In the case where endpoints for the same pin are not all declared to be communication devices, they should all
// either pass the test or fail the data intersection test. If they all fail due to lack of format support, 
// then these devices should probably be all declared to be communication devices to pass the test.
// Note, since there are cases where the pin-to-endpoint translation might not be correct and were allowed due
// to system error - Database:DTM Errata, bug:645. 
HRESULT IsTestCommunicationDevice(bool &fComm, bool &fBth)
{
    HRESULT hr = S_OK;
    fComm = false;
    fBth = false;
    CComPtr<IMMDeviceCollection> pEndpointCollection;
    hr = GetCurrentEndpointsUnderTest(&pEndpointCollection);
    if (FAILED(hr))
    {
        XLOG(XWARN, eError, "Failed to get current endpoints under test (hr = %s)", GetHRString(hr));
        return hr;
    }

    UINT cEndpoints = 0;
    hr = pEndpointCollection->GetCount(&cEndpoints);
    if (FAILED(hr))
    {
        XLOG(XWARN, eError, "Failed to get number of endpoints in the collection (hr = %s)", GetHRString(hr));
        return hr;
    }
    XLOG(XMSG, eInfo1, "There is a total of %d endpoints for the current pin", cEndpoints);

    if (0 == cEndpoints)
    {
        XLOG(XWARN, eError, "Failed to find any endpoints for the current pin");
        return E_UNEXPECTED;
    }

    CComPtr<IBasicLog> pLog;
    pLog = g_pBasicLog;

    fComm = true;
    fBth = true;
    for (UINT idxEndpoint = 0; idxEndpoint < cEndpoints; idxEndpoint++)
    {
        CComPtr<IMMDevice> pCurrEndpoint;
        hr = pEndpointCollection->Item(idxEndpoint, &pCurrEndpoint);
        if (FAILED(hr))
        {
            XLOG(XWARN, eError, "Failed to get current endpoint at index %d (hr = %s)", idxEndpoint, GetHRString(hr));
            fComm = false;
            fBth = false;
            return hr;
        }

        bool fIsCommunicationEndpoint = false;
        hr = IsCommunicationEndpoint(pLog, pCurrEndpoint, fIsCommunicationEndpoint);
        if (FAILED(hr))
        {
            XLOG(XWARN, eError, "Failed to get communication endpoint at index %d (hr = %s)", idxEndpoint, GetHRString(hr));
            fComm = false;
            fBth = false;
            return hr;
        }
        fComm = fComm && fIsCommunicationEndpoint;

        bool fIsBluetoothEndpoint = false;
        EndpointBusType::eBusType eBus = EndpointBusType::eUnknown;
        hr = GetEndpointBusType(pLog, pCurrEndpoint, eBus);
        if (FAILED(hr))
        {
            XLOG(XWARN, eError, "Failed to get bus type of endpoint at index %d (hr = %s)", idxEndpoint, GetHRString(hr));
            fComm = false;
            fBth = false;
            return hr;
        }
        if (EndpointBusType::eBluetooth == eBus)
        {
            fIsBluetoothEndpoint = true;
        }
        fBth = fBth && fIsBluetoothEndpoint;
    }
    if (fComm)
    {
        XLOG(XMSG, eInfo1, "The pin under test are connected to communication endpoints");
    }
    if (fBth)
    {
        XLOG(XMSG, eInfo1, "The pin under test are connected to bluetooth endpoints");
    }

    return S_OK;
}

//IsTestCommunicationDevice actually tells if the device is a commnunication device, also if it is a bluetooth device.
// New requirement is that commnunication device need to support 16kHz sample rate, so no longer skip the tests.
DWORD Test_RenderDataIntersection()
{
    bool fComm = false;
    bool fBth = false;
    HRESULT hr = IsTestCommunicationDevice(fComm, fBth);
    if (FAILED(hr))
    {
        XLOG(XWARN, eError, "Failed to decide if current data intersection test should be skipped");
        fComm = false;
    }

    return RenderCaptureDataIntersection(TRUE, fComm, fBth);
}

DWORD Test_CaptureDataIntersection()
{
    bool fComm = false;
    bool fBth = false;
    HRESULT hr = IsTestCommunicationDevice(fComm, fBth);
    if (FAILED(hr))
    {
        XLOG(XWARN, eError, "Failed to decide if current data intersection test should be skipped");
        fComm = false;
    }

    return RenderCaptureDataIntersection(FALSE, fComm, fBth);
}

//-------------------------------------------------------------------------
// Class: CDataIntersectionTest
// Method: Initialize
//
// Description:
//  Initialize the object
//  
// Parameters:
//  CKsPin* pKsPin
//      The Pin that this test is going to use
// Return values (BOOL):
//     TRUE on success, FALSE on Failure
//-------------------------------------------------------------------------
BOOL CDataIntersectionTest::Initialize
(
    CPCMAudioPin* pKsPin,
    bool fIsWaveRTPin,
    bool fIsStdStrmPin,
    bool fIsStdLoopedStrmPin
    )
{
    m_pPCMAudioPin = pKsPin;
    m_fIsWaveRTPin = fIsWaveRTPin;
    m_fIsStdStrmPin = fIsStdStrmPin;
    m_fIsStdLoopedStrmPin = fIsStdLoopedStrmPin;

    m_dwBufferMS = 100; //100 msec
    return (m_pPCMAudioPin != NULL);
} //BOOL CDataIntersectionTest::Initialize()

  //-------------------------------------------------------------------------
  // Class: CDataIntersectionTest
  // Method: CreateRender_Intersection
  //
  // Description:
  //      Calls internal method to check for intersection and instantiate the pin
  //      This method also prepares the pin by generating sine wave data to render
  //  
  // Parameters:
  //  WAVEFORMATEX* pWfx
  //      The format for which the data intersection is to be checked
  //
  // Return values (BOOL):
  //     TRUE on success, FALS on Failure
  //-------------------------------------------------------------------------
BOOL CDataIntersectionTest::CreateRender_Intersection(WAVEFORMATEX* pWfx)
{
    BOOL        fRes = TRUE;
    CComPtr<IBasicLog> pBasicLog = NULL;
    HRESULT     hr;

    fRes = CreateIntersection(pWfx);
    if (!fRes)
    {
        goto exit;
    }

    // get a logger
    pBasicLog = g_pBasicLog;

    hr = FillBufferWithSineSignal(
        pBasicLog,
        XFAIL,
        pWfx,
        TEST_AMPLITUDE,
        TEST_FREQUENCY,
        0.0, // initial phase
        0.0, // dc
        Method_NoDithering,
        m_pPCMAudioPin->m_pbStreamData,
        m_pPCMAudioPin->m_cbStreamData / pWfx->nBlockAlign,
        m_pPCMAudioPin->m_cbStreamData
        );
    if (FAILED(hr))
    {
        fRes = FALSE;
        XLOG(XMSG, eError, "FAIL: Unable to generate PCM Test tone");
        goto exit;
    }

exit:

    return fRes;
}//BOOL    CreateRender_Intersection(WAVEFORMATEX* pWfx)

 //-------------------------------------------------------------------------
 // Class: CDataIntersectionTest
 // Method: CreateCapture_Intersection
 //
 // Description:
 //      Calls internal method to check for intersection and instantiate the pin
 //  
 // Parameters:
 //  WAVEFORMATEX* pWfx
 //      The format for which the data intersection is to be checked
 //
 // Return values (BOOL):
 //     TRUE on success, FALS on Failure
 //-------------------------------------------------------------------------
BOOL CDataIntersectionTest::CreateCapture_Intersection(WAVEFORMATEX* pWfx)
{
    return CreateIntersection(pWfx);
}//BOOL    CreateCapture_Intersection(WAVEFORMATEX* pWfx)

 //-------------------------------------------------------------------------
 // Class: CDataIntersectionTest
 // Method: CreateIntersection
 //
 // Description:
 //  Checks for intersection with the format passed in on the render pin
 //  If there is an intersection, this method than sets the format
 //  on the pin and instantiates it
 //  
 // Parameters:
 //  WAVEFORMATEX* pWfx
 //      The format for which the data intersection is to be checked
 //
 // Return values (BOOL):
 //     TRUE on success, FALS on Failure
 //-------------------------------------------------------------------------
BOOL CDataIntersectionTest::CreateIntersection
(
    WAVEFORMATEX* pWfx
    )
{
    BOOL fRet = TRUE, fRes = TRUE;

    //Argument check
    if ((NULL == m_pPCMAudioPin) || (NULL == pWfx))
    {
        XLOG(XMSG, eError, "FAIL: Invalid arguments");
        fRet = FALSE;
        goto exit;
    }

    // Check that the format intersects with pin
    m_bIntersect = m_pPCMAudioPin->DoesFormatIntersect(pWfx, TRUE);
    if (!m_bIntersect)
    {
        fRet = FALSE;
        goto exit;
    }

    // This sets the pWfx as the format and instantiates the pin
    if (!InstantiatePinWfx(m_pPCMAudioPin, pWfx, m_fIsStdLoopedStrmPin))
    {
        fRet = FALSE;
        goto exit;
    }

    //Create a data buffer for dealing with this pin
    fRes = m_pPCMAudioPin->CreateDataBuffer(m_dwBufferMS, pWfx->nBlockAlign, 0x10);
    if (!fRes)
    {
        XLOG(XMSG, eError, "FAIL: Failed to create data buffer");
        fRet = FALSE;
        goto exit;
    }

exit:
    return fRet;
}//BOOL    CDataIntersectionTest::CreateIntersection(WAVEFORMATEX* pWfx)


 //-------------------------------------------------------------------------
 // Class: CDataIntersectionTest
 // Method: Render
 //
 // Description:
 //  Method renders the data on the render pin (WriteData, SetState etc...)
 //
 // Parameters:
 //
 // Return values (BOOL):
 //     TRUE on success, FALSE on Failure
 //-------------------------------------------------------------------------
BOOL CDataIntersectionTest::Render()
{
    BOOL fRes = TRUE;

    //Write data to the pin
    fRes = m_pPCMAudioPin->WriteData();
    if (!fRes)
    {
        SLOG(eError, "FAIL: Failed writing to render pin");
        goto exit;
    }

    //Set the capture pin to RUN 
    fRes = m_pPCMAudioPin->SetState(KSSTATE_RUN);
    if (!fRes)
    {
        SLOG(eError, "FAIL: Failed to set pin to KSSTATE_RUN");
        goto exit;
    }

    //Wait till data is rendered
    if (m_fIsWaveRTPin)
    {
        //If RT port, just sleep
        Sleep(m_dwBufferMS);
    }
    else
    {
        //if not RT, then wait for the pin to consume all the data
        //Ideally we would wait for m_dwBufferMS, but we add a small tolerance here
        //This is what it was in PCAudReq, so this is what it is here.
        WaitForSingleObject(m_pPCMAudioPin->GetOverlapped().hEvent, (DWORD)(m_dwBufferMS * STREAMING_EVENT_TOLERANCE));
    }

    // stop render pin.
    m_pPCMAudioPin->SetState(KSSTATE_STOP);

exit:
    return fRes;
}//BOOL    CDataIntersectionTest::Render()

 //-------------------------------------------------------------------------
 // Class: CDataIntersectionTest
 // Method: Capture
 //
 // Description:
 //  Method captures the data on the capture pin (ReadData, SetState etc...)
 //
 // Parameters:
 //
 // Return values (BOOL):
 //     TRUE on success, FALSE on Failure
 //-------------------------------------------------------------------------
BOOL CDataIntersectionTest::Capture()
{
    BOOL fRes = TRUE;

    //Read data from the capture pin
    fRes = m_pPCMAudioPin->ReadData();
    if (!fRes)
    {
        SLOG(eError, "FAIL: Failed reading data from capture pin");
        goto exit;
    }

    //Set the capture pin to RUN
    fRes = m_pPCMAudioPin->SetState(KSSTATE_RUN);
    if (!fRes)
    {
        SLOG(eError, "FAIL: Failed to set pin to KSSTATE_RUN");
        goto exit;
    }

    //Wait till some data is captured
    if (m_fIsWaveRTPin)
    {
        //If RT port, just sleep
        Sleep(m_dwBufferMS);
    }
    else
    {
        //if not RT, then wait for the pin to consume all the data
        //Ideally we would wait for m_dwBufferMS, but we add a small tolerance here
        //This is what it was in PCAudReq, so this is what it is here.
        WaitForSingleObject(m_pPCMAudioPin->GetOverlapped().hEvent, (DWORD)(m_dwBufferMS * STREAMING_EVENT_TOLERANCE));
    }

    //Set the pin to STOP
    m_pPCMAudioPin->SetState(KSSTATE_STOP);

exit:
    return fRes;
}//BOOL    CDataIntersectionTest::Capture()


 //-------------------------------------------------------------------------
 // Class: CDataIntersectionTest
 // Method: DestroyRender
 //
 // Description:
 //  Destroy (Cleanup) render pin
 //
 // Parameters:
 //
 // Return values (BOOL):
 //     TRUE on success, FALSE on Failure
 //-------------------------------------------------------------------------
BOOL CDataIntersectionTest::DestroyRender()
{
    m_pPCMAudioPin->ClosePin();

    return TRUE;
}//BOOL    CDataIntersectionTest::DestroyRender()

 //-------------------------------------------------------------------------
 // Class: CDataIntersectionTest
 // Method: DestroyCapture
 //
 // Description:
 //  Destroy (Cleanup) capture pin
 //
 // Parameters:
 //
 // Return values (BOOL):
 //     TRUE on success, FALSE on Failure
 //-------------------------------------------------------------------------
BOOL CDataIntersectionTest::DestroyCapture()
{
    m_pPCMAudioPin->ClosePin();

    return TRUE;
}//BOOL    CDataIntersectionTest::DestroyCapture()

 //-------------------------------------------------------------------------
 // Method: IsFilterPoweredBy
 //
 // Description:
 //  Determines if the given filter is powered by the given driver
 //  The driver in question should be a bare string: e.g., "usbaudio.sys"
 //  The function will look for strings of the form "usbaudio.sys" as well as "C:\windows\system32\drivers\usbaudio.sys"
 //-------------------------------------------------------------------------
HRESULT IsFilterPoweredBy(_In_ CKsFilter* filter, LPCSTR szDriver, _Out_ bool *pbPoweredBy)
{
    *pbPoweredBy = false;

    CComHeapPtr<CHAR> driverFiles;
    HRESULT hr = InstanceIdToDriverFileList(&driverFiles, filter->m_szInstanceID);

    if (HRESULT_FROM_SETUPAPI(ERROR_IN_WOW64) == hr)
    {
        // can't tell
        return S_FALSE;
    }

    if (FAILED(hr))
    {
        XLOG(XFAIL,
            eError,
            "IsFilterPoweredBy() encountered an error calling InstanceIdToDriverFileList( id = %s ) [hr = 0x%X ]",
            filter->m_szInstanceID,
            hr);
        return hr;
    }

    MultiStringIterator driverStrIter((LPCSTR)driverFiles);
    for (LPCTSTR str = driverStrIter.Next(); NULL != str; str = driverStrIter.Next())
    {
        // see if it matches directly
        if (0 == StrCmpI(str, szDriver))
        {
            *pbPoweredBy = true;
            return S_OK;
        }

        // see if str is of the form "some-path\strDriver"
        LPCTSTR lastBackslash = StrRChr(str, NULL, '\\');

        // [1] because we want to start at the character after the backslash
        // there's no risk of going off the end of the string
        // because even if the backslash is the last character (which would be weird)
        // [1] will point to the NULL terminator
        if (NULL != lastBackslash && 0 == StrCmpI(&lastBackslash[1], szDriver))
        {
            *pbPoweredBy = true;
            return S_OK;
        }
    }

    return S_OK;
}

