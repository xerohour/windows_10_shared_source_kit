// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
// File Name:
//
//  DSTests.cpp
//
// Abstract:
//
//  Implementation file for regression tests for specific bugs found elsewhere
//



// -------------------------------------------------------------------------------

#include <ac3tst.h>
#include <tests.h>
#include <dserr.h>

#define SafeRelease(p)  if (p)  (p)->Release(); (p) = NULL

// ------------------------------------------------------------------------------
// see kstests.cpp
extern BOOL InstantiateFillAndWriteToPin(CKsPin* pPin);

#define BytesIn10MsAC3  960
#define BytesIn32MsAC3  6144
// ------------------------------------------------------------------------------
// Procedure
// Abstract:	Regression test for RAID 183493
// ------------------------------------------------------------------------------
DWORD   
Tc_Regression183493
(
    void
)
{
    DWORD       dwRes = FNS_PASS;
    BOOL        fRes = TRUE;
    CPCMPinDev* pKsPinDev = (CPCMPinDev*)g_pCurDevice;
    CKsPin*     pPin = pKsPinDev->m_pPin;

    __try
    {
        // TODO - Implement RT-style streaming for RTPort filters
        if (((CPCMAudioFilter*)pPin->m_pFilter)->m_fRTPort)
        {
            LOG(eInfo1, " Skipping: KSWriteStream not used for RTPort filters");
            __leave;
        }

        KSAUDIO_POSITION    pos1, pos2, pos3;
        double              t1, t2, t3, t4;

        fRes = InstantiateFillAndWriteToPin(pPin);
        if (!fRes)
            __leave;

        fRes = pPin->SetState(KSSTATE_RUN);
        if (!fRes)
            __leave;

        Sleep(2000);

        // get time before 1st getposition
        t1 = qpcGetTime();

        SLOG(eInfo2, "Making sure that call to KSPROPERTY_AUDIO_POSITION succeeds when pin is in KSSTATE_RUN...");
        fRes = pPin->GetPropertySimple(KSPROPSETID_Audio, KSPROPERTY_AUDIO_POSITION, &pos1, sizeof(pos1));
        if (!fRes)
        {
            SLOG(eError, "Failure condition:  Call to KSPROPERTY_AUDIO_POSITION failed");
            __leave;
        }

        // get time after 1st getposition and before transition to PAUSE
        t2 = qpcGetTime();

        if (g_fLogPositions)
            SLOG(eInfo2, "Position.PlayOffset = %5d, Position.WriteOffset = %5d", (ULONG)pos1.PlayOffset, (ULONG)pos1.WriteOffset);

        fRes = pPin->SetState(KSSTATE_PAUSE);
        if (!fRes)
            __leave;

        Sleep(0);

        // get time after transition to PAUSE
        t3 = qpcGetTime();

        SLOG(eInfo2, "Making sure that call to KSPROPERTY_AUDIO_POSITION returns reasonable value when pin is in KSSTATE_PAUSE...");
        fRes = pPin->GetPropertySimple(KSPROPSETID_Audio, KSPROPERTY_AUDIO_POSITION, &pos2, sizeof(pos2));
        if (!fRes)
        {
            SLOG(eError, "Failure condition:  Call to KSPROPERTY_AUDIO_POSITION failed");
            __leave;
        }

        // get time after 2nd getposition
        t4 = qpcGetTime();

        if (g_fLogPositions)
            SLOG(eInfo2, "Position.PlayOffset = %5d, Position.WriteOffset = %5d", (ULONG)pos2.PlayOffset, (ULONG)pos2.WriteOffset);

        // position should be about what it was before we paused the pin.  960 is the number of bytes in 5 ms of AC3 data
        double nTimeMin, nTimeMax;
        nTimeMin = t3-t2;
        nTimeMax = t4-t1;

        g_IShell->IncrementIndent();
        SLOG(eInfo2, "State transition (KSSTATE_RUN -> KSSTATE_PAUSE) took %.02f ms", nTimeMin);
        SLOG(eInfo2, "The time from the first KSPROPERTY_AUDIO_POSITION to the second was somewhere between %.02f and %.02f ms...", nTimeMin, nTimeMax);

        int nPosExpectedMin = (int)pos1.PlayOffset; // + (int)(nTimeMin * (double)g_wrAC3.pWfx->nAvgBytesPerSec / 1000.);
        int nPosExpectedMax = (int)pos1.PlayOffset + (int)(nTimeMax * (double)g_wrAC3.pWfx->nAvgBytesPerSec / 1000.);

        SLOG(eInfo2, "...which implies an expected position of %d to %d", nPosExpectedMin, nPosExpectedMax);

        if (nPosExpectedMax % BytesIn32MsAC3)
            nPosExpectedMax = (nPosExpectedMax / BytesIn32MsAC3 + 1) * BytesIn32MsAC3;

        SLOG(eInfo2, "Rounding up to the neares 32 ms boundary, the upperbound is %d", nPosExpectedMax);

        nPosExpectedMax += BytesIn10MsAC3;
        SLOG(eInfo2, "Furthermore, 10 ms (960 bytes) of tolerance is allowed for an upperbound of %d", nPosExpectedMax);
        g_IShell->DecrementIndent();

        SLOG(eInfo2, "GET KSPROPERTY_AUDIO_POSITION returned a value of %d (bytes).  Position is expected to be between %d and %d.", 
            (ULONG)pos2.PlayOffset, nPosExpectedMin, nPosExpectedMax);

        fRes = ((int)pos2.PlayOffset < (int)pPin->m_cbStreamData);
        if (!fRes)
        {
            SLOG(eError, "Failure condition:  Position reported is larger than the size of the buffer submitted to the pin!");
            dwRes = FNS_FAIL;
        }
// chage due to bug 1028149.
        fRes = ((pos2.PlayOffset >= nPosExpectedMin) && (pos2.PlayOffset < nPosExpectedMax));
        if (!fRes)
        {
            SLOG(eError, "Failure condition:  Position reported is outside of the range of expected positions.");
            dwRes = FNS_FAIL;
        }

        Sleep(100);

        // make sure the thing aint moving while paused
        SLOG(eInfo2, "Making sure that play cursor does not move when pin is in KSSTATE_PAUSE...");
        fRes = pPin->GetPropertySimple(KSPROPSETID_Audio, KSPROPERTY_AUDIO_POSITION, &pos3, sizeof(pos3));
        if (!fRes)
        {
            SLOG(eError, "Failure condition:  Call to KSPROPERTY_AUDIO_POSITION failed");
            __leave;
        }

        fRes = (pos3.PlayOffset == pos2.PlayOffset);
        if (!fRes)
        {
            SLOG(eError, "Failure condition:  Position changed from %d to %d while in the paused state.", (ULONG)pos2.PlayOffset, (ULONG)pos3.PlayOffset);
            dwRes = FNS_FAIL;
        }

        SLOG(eInfo2, "Setting state to KSSTATE_RUN for audible confirmation of the position...");
        fRes = pPin->SetState(KSSTATE_RUN);
        if (!fRes)
            __leave;

        pPin->Wait(4000);

        fRes = pPin->SetState(KSSTATE_ACQUIRE);
        if (!fRes)
            __leave;
    }

    __finally
    {
    }

    SafeLocalFree(pPin->m_pbStreamData);

    pPin->ClosePin();

    if (!fRes)
        dwRes = FNS_FAIL;

    return dwRes;
}
