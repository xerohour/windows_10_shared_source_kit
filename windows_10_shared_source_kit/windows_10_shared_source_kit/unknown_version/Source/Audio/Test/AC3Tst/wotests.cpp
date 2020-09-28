// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
// File Name:
//
//  WoTests.cpp
//
// Abstract:
//
//  Implementation file for waveOut-related test cases
//



// -------------------------------------------------------------------------------

#include <ac3tst.h>
#include <tests.h>


// ------------------------------------------------------------------------------

LPCSTR GetMMErrorText
(
    MMRESULT    mmrError
)
{
    switch (mmrError)
    {
        case MMSYSERR_NOERROR:          return ("MMSYSERR_NOERROR");
        case MMSYSERR_ERROR:            return ("MMSYSERR_ERROR");
        case MMSYSERR_BADDEVICEID:      return ("MMSYSERR_BADDEVICEID");
        case MMSYSERR_NOTENABLED:       return ("MMSYSERR_NOTENABLED");
        case MMSYSERR_ALLOCATED:        return ("MMSYSERR_ALLOCATED");
        case MMSYSERR_INVALHANDLE:      return ("MMSYSERR_INVALHANDLE");
        case MMSYSERR_NODRIVER:         return ("MMSYSERR_NODRIVER");
        case MMSYSERR_NOMEM:            return ("MMSYSERR_NOMEM");
        case MMSYSERR_NOTSUPPORTED:     return ("MMSYSERR_NOTSUPPORTED");
        case MMSYSERR_BADERRNUM:        return ("MMSYSERR_BADERRNUM");
        case MMSYSERR_INVALFLAG:        return ("MMSYSERR_INVALFLAG");
        case MMSYSERR_INVALPARAM:       return ("MMSYSERR_INVALPARAM");
        case MMSYSERR_HANDLEBUSY:       return ("MMSYSERR_HANDLEBUSY");
        case MMSYSERR_INVALIDALIAS:     return ("MMSYSERR_INVALIDALIAS");
        case MMSYSERR_BADDB:            return ("MMSYSERR_BADDB");
        case MMSYSERR_KEYNOTFOUND:      return ("MMSYSERR_KEYNOTFOUND");
        case MMSYSERR_READERROR:        return ("MMSYSERR_READERROR");
        case MMSYSERR_WRITEERROR:       return ("MMSYSERR_WRITEERROR");
        case MMSYSERR_DELETEERROR:      return ("MMSYSERR_DELETEERROR");
        case MMSYSERR_VALNOTFOUND:      return ("MMSYSERR_VALNOTFOUND");
        case MMSYSERR_NODRIVERCB:       return ("MMSYSERR_NODRIVERCB");
		case WAVERR_BADFORMAT:          return ("WAVERR_BADFORMAT");
        case WAVERR_STILLPLAYING:       return ("WAVERR_STILLPLAYING");
        case WAVERR_UNPREPARED:         return ("WAVERR_UNPREPARED");
        case WAVERR_SYNC:               return ("WAVERR_SYNC");
        case MMIOERR_FILENOTFOUND:      return ("MMIOERR_FILENOTFOUND");
        case MMIOERR_OUTOFMEMORY:       return ("MMIOERR_OUTOFMEMORY");
        case MMIOERR_CANNOTOPEN:        return ("MMIOERR_CANNOTOPEN");
        case MMIOERR_CANNOTCLOSE:       return ("MMIOERR_CANNOTCLOSE");
        case MMIOERR_CANNOTREAD:        return ("MMIOERR_CANNOTREAD");
        case MMIOERR_CANNOTWRITE:       return ("MMIOERR_CANNOTWRITE");
        case MMIOERR_CANNOTSEEK:        return ("MMIOERR_CANNOTSEEK");
        case MMIOERR_CANNOTEXPAND:      return ("MMIOERR_CANNOTEXPAND");
        case MMIOERR_CHUNKNOTFOUND:     return ("MMIOERR_CHUNKNOTFOUND");
        case MMIOERR_UNBUFFERED:        return ("MMIOERR_UNBUFFERED");
        case MMIOERR_PATHNOTFOUND:      return ("MMIOERR_PATHNOTFOUND");
        case MMIOERR_ACCESSDENIED:      return ("MMIOERR_ACCESSDENIED");
        case MMIOERR_SHARINGVIOLATION:  return ("MMIOERR_SHARINGVIOLATION");
        case MMIOERR_NETWORKERROR:      return ("MMIOERR_NETWORKERROR");
        case MMIOERR_TOOMANYOPENFILES:  return ("MMIOERR_TOOMANYOPENFILES");
        case MMIOERR_INVALIDFILE:       return ("MMIOERR_INVALIDFILE");
        case MM_STREAM_ERROR:           return ("MM_STREAM_ERROR");
		default:                        return ("Undefined Error");
   }
}  // GetMMErrorText()

// ----------------------------------------------------------------------------------
// TrapMMError
// return:          FALSE if it is an error, TRUE otherwise
// ----------------------------------------------------------------------------------
BOOL 
TrapMMError
(
    MMRESULT    mmRes,
    LPCSTR      szAPI,
    BOOL        fSuppressNoErrorLog = FALSE
)
{
    if(MMSYSERR_NOERROR == mmRes)
    {
        if (!fSuppressNoErrorLog)
            LOG(eInfo2, "%s returned \"MMSYSERR_NOERROR\"", szAPI);

        return TRUE;
    } 
    else
    {
        // log error message and return FALSE;
        LOG(eError, "ERROR: %s returned \"%s\"", szAPI, GetMMErrorText(mmRes));
        return FALSE;
    }
}


// ------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------
BOOL CreateAndFillWaveOutBuffer
(
    HWAVEOUT*   pwho,
    WAVEHDR*    pwhr,
    BOOL        fFill,
    DWORD       dwFlags = 0
)
{
    BOOL            fRes;
    CWinMMDev*      pMMDD = (CWinMMDev*)g_pCurDevice;

    // open sesame
    LogCall(waveOutOpen);
    fRes = TrapMMError(waveOutOpen(pwho, pMMDD->m_uDevID, g_wrAC3.pWfx, 0, 0, 0), "waveOutOpen");
    if (fRes)
    {
        if (fFill)
        {
            // Fill buffer
            ZeroMemory(pwhr, sizeof(WAVEHDR));

            pwhr->lpData = (char*)g_wrAC3.pData;
            pwhr->dwBufferLength = g_wrAC3.cbData;
            pwhr->dwFlags = dwFlags;
    
            LogCall(waveOutPrepareHeader);
            fRes = TrapMMError(waveOutPrepareHeader(*pwho, pwhr, sizeof(WAVEHDR)), "waveOutPrepareHeader");
        }
    }

    return fRes;
}


// ------------------------------------------------------------------------------
// Procedure
// Abstract:	
// ------------------------------------------------------------------------------
DWORD   Tc_woOpen(void)
{
    BOOL            fRes = TRUE;
    DWORD           dwRes = FNS_PASS;
    CWinMMDev*      pMMDev = (CWinMMDev*)g_pCurDevice;
    HWAVEOUT        hwo = NULL;

    // open sesame
    LogCall(waveOutOpen);
    fRes = TrapMMError(waveOutOpen(&hwo, pMMDev->m_uDevID, g_wrAC3.pWfx, 0, 0, 0), "waveOutOpen");

    if (hwo)
        waveOutClose(hwo);

    if (!fRes)
        dwRes = FNS_FAIL;

    return dwRes;
}

// ------------------------------------------------------------------------------
// Procedure
// Abstract:	
// ------------------------------------------------------------------------------
DWORD   Tc_woOpenMulti(void)
{
    BOOL            fRes = TRUE;
    DWORD           dwRes = FNS_PASS;
    MMRESULT        mmRes;
    CWinMMDev*      pMMDev = (CWinMMDev*)g_pCurDevice;
    HWAVEOUT        hwo = NULL;
    HWAVEOUT        hwo2 = NULL;

    // open sesame
    LogCall(waveOutOpen);
    fRes = TrapMMError(waveOutOpen(&hwo, pMMDev->m_uDevID, g_wrAC3.pWfx, 0, 0, 0), "waveOutOpen");
    if (fRes)
    {
        // whether we can duplicate the buffer or not depends on the number of possible instances
        BOOL fExpectDupSuccess = (pMMDev->m_cPossibleInstances > 1);

        LogCall(waveOutOpen);
        LOG(eInfo1, "Given the possible instance count (%d) of the render pin, a second call to waveOutOpen should %s", 
            pMMDev->m_cPossibleInstances, fExpectDupSuccess ? "SUCCEED" : "FAIL");
    
        mmRes = waveOutOpen(&hwo2, pMMDev->m_uDevID, g_wrAC3.pWfx, 0, 0, 0);
        TrapMMError(mmRes, "waveOutOpen (2nd call)");

        if (mmRes != MMSYSERR_NOERROR)
        {
            if (fExpectDupSuccess)
                dwRes = FNS_FAIL;
            else
                LOG(eInfo1, "waveOutOpen (2nd call) failed (as expected)");
        }
        else
        {
            if (fExpectDupSuccess)
                LOG(eInfo1, "waveOutOpen (2nd call) succeeded (as expected)");
            else
            {
                LOG(eInfo1, "waveOutOpen (2nd call) succeeded incorrectly.");
                dwRes = FNS_FAIL;
            }
        }
    }

    if (hwo)
        waveOutClose(hwo);

    if (hwo2)
        waveOutClose(hwo2);

    if (!fRes)
        dwRes = FNS_FAIL;

    return dwRes;
}

// ------------------------------------------------------------------------------
// Procedure
// Abstract:	
// ------------------------------------------------------------------------------
DWORD   Tc_woWrite(void)
{
    BOOL            fRes = TRUE;
    DWORD           dwRes = FNS_PASS;
    HWAVEOUT        hwo = NULL;
    WAVEHDR         whdr;

    __try
    {
        // common stuff
        fRes = CreateAndFillWaveOutBuffer(&hwo, &whdr, TRUE);
        if (!fRes)
            __leave;

        // play it
        LogCall(waveOutWrite);
        fRes = TrapMMError(waveOutWrite(hwo, &whdr, sizeof(whdr)), "waveOutWrite");
        if (!fRes)
            __leave;

        // WHDR_DONE should be clear
        fRes = ((whdr.dwFlags & WHDR_DONE) == 0);
        if (!fRes)
        {
            LOG(eError, "Failure:  WHDR_DONE bit set too early");
            __leave;
        }

        // wait
        DWORD   dwTimeStart = timeGetTime();

        while ((whdr.dwFlags & WHDR_DONE) == 0)
        {
            if (timeGetTime() - dwTimeStart > g_wrAC3.cmsData + 100)
            {
                LOG(eError, "Failure:  waveOut device didn't stop playing within 100 ms of the expected time");
                __leave;
            }
        }
    }

    __finally
    {
        if (hwo)
        {
            waveOutReset(hwo);  // should always do this before unpreparing
            waveOutUnprepareHeader(hwo, &whdr, sizeof(whdr));
            waveOutClose(hwo);
        }
    }

    if (!fRes)
        dwRes = FNS_FAIL;

    return dwRes;
}

// ------------------------------------------------------------------------------
// Procedure
// Abstract:	
// ------------------------------------------------------------------------------
DWORD   Tc_woWriteLooped(void)
{
    BOOL            fRes = TRUE;
    DWORD           dwRes = FNS_PASS;
    MMRESULT        mmRes;
    HWAVEOUT        hwo = NULL;
    WAVEHDR         whdr;

    __try
    {
        CWinMMDev*   pMMDD = (CWinMMDev*)g_pCurDevice;

        // open sesame
        LogCall(waveOutOpen);
        fRes = TrapMMError(waveOutOpen(&hwo, pMMDD->m_uDevID, g_wrAC3.pWfx, 0, 0, 0), "waveOutOpen");
        if (!fRes)
            __leave;

        // Fill buffer
        ZeroMemory(&whdr, sizeof(WAVEHDR));

        whdr.lpData = (char*)g_wrAC3.pData;
        whdr.dwBufferLength = g_wrAC3.cbData;
        whdr.dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP;
        whdr.dwLoops = 5;
        
        LOG(eInfo1, "Setting WHDR_BEGINLOOP and WHDR_ENDLOOP bits in the WAVEHDR");

        LogCall(waveOutPrepareHeader);
        fRes = TrapMMError(waveOutPrepareHeader(hwo, &whdr, sizeof(WAVEHDR)), "waveOutPrepareHeader");
        if (!fRes)
            __leave;
        
        /*
        fRes = (mmRes == MMSYSERR_INVALPARAM);
        if (!fRes)
        {
            LOG(eError, "Failure:  Calling waveOutPrepareHeader on a WAVEHDR with WHDR_BEGINLOOP | WHDR_ENDLOOP should return MMSYSERR_NOTSUPPORTED");
//            __leave;
        }
*/
        // play it
        LogCall(waveOutWrite);
        mmRes = waveOutWrite(hwo, &whdr, sizeof(whdr));
        //
        // 20051013 - jwexler - According to MSDN, this *IS* supported
        //  So, I've updated the code to respect that.
        //
        fRes = TrapMMError(mmRes, "waveOutWrite");
        if (!fRes)
        {
            LOG(eError, "Failure:  Calling waveOutWrite on a WAVEHDR with WHDR_BEGINLOOP | WHDR_ENDLOOP failed");
            __leave;
        }

        /*        
        TrapMMError(mmRes, "waveOutWrite");
        fRes = (mmRes == MMSYSERR_NOTSUPPORTED);
        if (!fRes)
        {
            LOG(eError, "Failure:  Calling waveOutWrite on a WAVEHDR with WHDR_BEGINLOOP | WHDR_ENDLOOP should return MMSYSERR_NOTSUPPORTED");
            __leave;
        }
        */
    }

    __finally
    {
        if (hwo)
        {
            waveOutReset(hwo);  // should always do this before unpreparing
            waveOutUnprepareHeader(hwo, &whdr, sizeof(whdr));
            waveOutClose(hwo);
        }
    }

    if (!fRes)
        dwRes = FNS_FAIL;

    return dwRes;
}

// ------------------------------------------------------------------------------
// Procedure
// Abstract:	
// ------------------------------------------------------------------------------
DWORD   Tc_woPause(void)
{
    BOOL            fRes = TRUE;
    DWORD           dwRes = FNS_PASS;
    HWAVEOUT        hwo = NULL;
    WAVEHDR         whdr;

    __try
    {
        // common stuff
        fRes = CreateAndFillWaveOutBuffer(&hwo, &whdr, TRUE);
        if (!fRes)
            __leave;

        // play it
        LogCall(waveOutWrite);
        fRes = TrapMMError(waveOutWrite(hwo, &whdr, sizeof(whdr)), "waveOutWrite");
        if (!fRes)
            __leave;

        // pause and restart the stream
        ULONG   msSleep = g_wrAC3.cmsData / 10;
        int     i;

        for (i = 0; i < 10; i++)
        {
            LogCall(waveOutPause);
            fRes = TrapMMError(waveOutPause(hwo), "waveOutPause");
            if (!fRes)
                __leave;

            Sleep(msSleep);

            if (i == 10)
                break;

            LogCall(waveOutRestart);
            fRes = TrapMMError(waveOutRestart(hwo), "waveOutRestart");
            if (!fRes)
                __leave;
        }
    }

    __finally
    {
        if (hwo)
        {
            waveOutReset(hwo);  // should always do this before unpreparing
            waveOutUnprepareHeader(hwo, &whdr, sizeof(whdr));
            waveOutClose(hwo);
        }
    }

    if (!fRes)
        dwRes = FNS_FAIL;

    return dwRes;
}

// ------------------------------------------------------------------------------
// Procedure
// Abstract:	
// ------------------------------------------------------------------------------
DWORD   Tc_woReset(void)
{
    BOOL            fRes = TRUE;
    DWORD           dwRes = FNS_PASS;
    HWAVEOUT        hwo = NULL;
    WAVEHDR         whdr;

    __try
    {
        // common stuff
        fRes = CreateAndFillWaveOutBuffer(&hwo, &whdr, TRUE);
        if (!fRes)
            __leave;

        // play it
        LogCall(waveOutWrite);
        fRes = TrapMMError(waveOutWrite(hwo, &whdr, sizeof(whdr)), "waveOutWrite");
        if (!fRes)
            __leave;

        // sleep a bit, then reset the buffer and check it
        Sleep(g_wrAC3.cmsData / 2);

        LogCall(waveOutReset);
        fRes = TrapMMError(waveOutReset(hwo), "waveOutReset");
        if (!fRes)
            __leave;

        fRes = (whdr.dwFlags & WHDR_DONE);
        if (!fRes)
        {
            LOG(eError, "Failure:  After resetting the waveOut device, the WHDR_DONE bit is not set");
            __leave;
        }

        // the position should be 0
        MMTIME mmt;
        mmt.wType = TIME_BYTES;

        LogCall(waveOutGetPosition);
        fRes = TrapMMError(waveOutGetPosition(hwo, &mmt, sizeof(MMTIME)), "waveOutGetPosition", TRUE);
        if (!fRes)
            __leave;

        fRes = (mmt.u.cb == 0);
        if (!fRes)
        {
            LOG(eError, "Failure:  After resetting the waveOut device, the position (TIME_BYTES) is not 0");
            __leave;
        }
    }

    __finally
    {
        if (hwo)
        {
            waveOutReset(hwo);  // should always do this before unpreparing
            waveOutUnprepareHeader(hwo, &whdr, sizeof(whdr));
            waveOutClose(hwo);
        }
    }

    if (!fRes)
        dwRes = FNS_FAIL;

    return dwRes;
}


// ------------------------------------------------------------------------------
// Procedure
// Abstract:	
// ------------------------------------------------------------------------------
DWORD   Tc_woGetPosition(void)
{
    BOOL            fRes = TRUE;
    DWORD           dwRes = FNS_PASS;
    HWAVEOUT        hwo = NULL;
    WAVEHDR         whdr;

    __try
    {
        // common stuff
        fRes = CreateAndFillWaveOutBuffer(&hwo, &whdr, TRUE);
        if (!fRes)
            __leave;

        // play it
        LogCall(waveOutWrite);
        fRes = TrapMMError(waveOutWrite(hwo, &whdr, sizeof(whdr)), "waveOutWrite");
        if (!fRes)
            __leave;

        // spin.
        DWORD   dwTimeStart = timeGetTime(); 
        UINT    cbLast = 0;
        MMTIME  mmt;

        ZeroMemory(&mmt, sizeof(mmt));
        mmt.wType = TIME_BYTES;

        LOG(eInfo1, "waveOutGetPosition will be called repeatedly.  The value should increase.");

        while 
        (
            (mmt.u.cb < g_wrAC3.cbData)    &&
            (timeGetTime() - dwTimeStart < g_wrAC3.cmsData + 100)
        )
        {
            fRes = TrapMMError(waveOutGetPosition(hwo, &mmt, sizeof(mmt)), "waveOutGetPosition", TRUE);
            if (!fRes)
                __leave;

            if (g_fLogPositions)
                LOG(eInfo2, "waveOutGetPosition: %5d", mmt.u.cb);

            fRes = (mmt.u.cb >= cbLast);
            if (!fRes)
            {
                LOG(eError, "Failure: The play cursor appears to have moved backward (from %d to %d)", cbLast, mmt.u.cb);
                __leave;
            }

            cbLast = mmt.u.cb;
            Sleep(2);
        }
    }

    __finally
    {
        if (hwo)
        {
            waveOutReset(hwo);  // should always do this before unpreparing
            waveOutUnprepareHeader(hwo, &whdr, sizeof(whdr));
            waveOutClose(hwo);
        }
    }

    if (!fRes)
        dwRes = FNS_FAIL;

    return dwRes;
}

// ------------------------------------------------------------------------------
// Procedure
// Abstract:	
// ------------------------------------------------------------------------------
DWORD   Tc_woOpenAC3thenPCM(void)
{
    BOOL            fRes = TRUE;
    DWORD           dwRes = FNS_PASS;
    HWAVEOUT        hwoAC3 = NULL;
    WAVEHDR         whdrAC3;
    HWAVEOUT        hwoPCM = NULL;
    WAVEHDR         whdrPCM;
    MMRESULT        mmErr;

    CWinMMDev*      pMMDD = (CWinMMDev*)g_pCurDevice;

    __try
    {
        // open AC3 device
        LOG(eInfo1, "Calling waveOutOpen with an AC-3 format");
        fRes = TrapMMError(waveOutOpen(&hwoAC3, pMMDD->m_uDevID, g_wrAC3.pWfx, 0, 0, 0), "waveOutOpen");
        if (!fRes)
            __leave;

        // query for PCM support
        LOG(eInfo1, "Calling waveOutOpen (WAVE_FORMAT_QUERY) with a PCM format");
        fRes = TrapMMError(waveOutOpen(&hwoPCM, pMMDD->m_uDevID, g_wrPCM.pWfx, 0, 0, WAVE_FORMAT_QUERY), "waveOutOpen");
        if (!fRes)
            __leave;

        // open PCM device
        // this may fail if exclusive-mode streams have priority on this device
        // (which is the default)
        //
        // if it succeeds, the AC3 stream will be dropped on the floor.
        LOG(eInfo1, "Calling waveOutOpen with a PCM format");
        mmErr = waveOutOpen(&hwoPCM, pMMDD->m_uDevID, g_wrPCM.pWfx, 0, 0, 0);
        switch (mmErr)
        {
            case MMSYSERR_NOERROR:
                break;

            case MMSYSERR_ALLOCATED:
                LOG(eInfo1,
                    "waveOutOpen(PCM) returned MMSYSERR_ALLOCATED; this is consistent with "
                    "exclusive-mode streams having priority"
                );
                __leave; // nothing to do
                
            default:
                fRes = TrapMMError(mmErr, "waveOutOpen");
                __leave;
        }

        // Fill AC3 buffer
        ZeroMemory(&whdrAC3, sizeof(WAVEHDR));

        whdrAC3.lpData = (char*)g_wrAC3.pData;
        whdrAC3.dwBufferLength = g_wrAC3.cbData;
    
        LogCall(waveOutPrepareHeader);
        fRes = TrapMMError(waveOutPrepareHeader(hwoAC3, &whdrAC3, sizeof(WAVEHDR)), "waveOutPrepareHeader");
        if (!fRes)
            __leave;

        // Fill PCM buffer
        ZeroMemory(&whdrPCM, sizeof(WAVEHDR));

        whdrPCM.lpData = (char*)g_wrPCM.pData;
        whdrPCM.dwBufferLength = g_wrPCM.cbData;
    
        LogCall(waveOutPrepareHeader);
        fRes = TrapMMError(waveOutPrepareHeader(hwoPCM, &whdrPCM, sizeof(WAVEHDR)), "waveOutPrepareHeader");
        if (!fRes)
            __leave;

        // play it
        SLOG(eInfo1, "Calling waveOutWrite on AC3 waveOut handle");
        fRes = TrapMMError(waveOutWrite(hwoAC3, &whdrAC3, sizeof(whdrAC3)), "waveOutWrite");
        if (!fRes)
            __leave;

        // play it
        SLOG(eInfo1, "Calling waveOutWrite on PCM waveOut handle");
        fRes = TrapMMError(waveOutWrite(hwoPCM, &whdrPCM, sizeof(whdrPCM)), "waveOutWrite");
        if (!fRes)
            __leave;

        // spin.
        DWORD   dwTimeStart = timeGetTime();
        UINT    cbLast = 0;
        MMTIME  mmt;

        ZeroMemory(&mmt, sizeof(mmt));
        mmt.wType = TIME_BYTES;

        LOG(eInfo1, "waveOutGetPosition will be called repeatedly.  The value should increase.");

        while 
        (
            (mmt.u.cb < g_wrAC3.cbData)    &&
            (timeGetTime() - dwTimeStart < g_wrAC3.cmsData + 100)
        )
        {
            fRes = TrapMMError(waveOutGetPosition(hwoAC3, &mmt, sizeof(mmt)), "waveOutGetPosition", TRUE);
            if (!fRes)
                __leave;

            if (g_fLogPositions)
                LOG(eInfo2, "waveOutGetPosition: %5d", mmt.u.cb);

            fRes = (mmt.u.cb >= cbLast);
            if (!fRes)
            {
                LOG(eError, "Failure: The play cursor appears to have moved backward (from %d to %d)", cbLast, mmt.u.cb);
                __leave;
            }

            cbLast = mmt.u.cb;
            Sleep(2);
        }
    }

    __finally
    {
        if (hwoAC3)
        {
            waveOutReset(hwoAC3);  // should always do this before unpreparing
            waveOutUnprepareHeader(hwoAC3, &whdrAC3, sizeof(whdrAC3));
            waveOutClose(hwoAC3);
        }

        if (hwoPCM)
        {
            waveOutReset(hwoPCM);  // should always do this before unpreparing
            waveOutUnprepareHeader(hwoPCM, &whdrPCM, sizeof(whdrPCM));
            waveOutClose(hwoPCM);
        }
    }

    if (!fRes)
        dwRes = FNS_FAIL;

    return dwRes;
}


// ------------------------------------------------------------------------------
// Procedure
// Abstract:	
// ------------------------------------------------------------------------------
DWORD   Tc_woOpenPCMthenAC3(void)
{
    BOOL            fRes = TRUE;
    DWORD           dwRes = FNS_PASS;
    HWAVEOUT        hwoAC3 = NULL;
    WAVEHDR         whdrAC3;
    HWAVEOUT        hwoPCM = NULL;
    WAVEHDR         whdrPCM;
    MMRESULT        mmErr;

    CWinMMDev*      pMMDD = (CWinMMDev*)g_pCurDevice;

    __try
    {
        // open PCM device
        LOG(eInfo1, "Calling waveOutOpen with a PCM format");
        fRes = TrapMMError(waveOutOpen(&hwoPCM, pMMDD->m_uDevID, g_wrPCM.pWfx, 0, 0, 0), "waveOutOpen");
        if (!fRes)
            __leave;

        // query for AC3 support
        LOG(eInfo1, "Calling waveOutOpen (WAVE_FORMAT_QUERY) with a AC3 format");
        fRes = TrapMMError(waveOutOpen(&hwoAC3, pMMDD->m_uDevID, g_wrAC3.pWfx, 0, 0, WAVE_FORMAT_QUERY), "waveOutOpen");
        if (!fRes)
            __leave;

        // open AC3 device
        // this may fail if shared-mode streams have priority on this device
        // (not the default, but possible)
        //
        // if it succeeds, the PCM stream will be dropped on the floor.
        LOG(eInfo1, "Calling waveOutOpen with a AC3 format");
        mmErr = waveOutOpen(&hwoAC3, pMMDD->m_uDevID, g_wrAC3.pWfx, 0, 0, 0);
        switch (mmErr)
        {
            case MMSYSERR_NOERROR:
                break;

            case MMSYSERR_ALLOCATED:
                LOG(eInfo1,
                    "waveOutOpen(AC3) returned MMSYSERR_ALLOCATED; this is consistent with "
                    "shared-mode streams having priority"
                );
                __leave; // nothing to do
                
            default:
                fRes = TrapMMError(mmErr, "waveOutOpen");
                __leave;
        }

        // Fill PCM buffer
        ZeroMemory(&whdrPCM, sizeof(WAVEHDR));

        whdrPCM.lpData = (char*)g_wrPCM.pData;
        whdrPCM.dwBufferLength = g_wrPCM.cbData;
    
        LogCall(waveOutPrepareHeader);
        fRes = TrapMMError(waveOutPrepareHeader(hwoPCM, &whdrPCM, sizeof(WAVEHDR)), "waveOutPrepareHeader");
        if (!fRes)
            __leave;

        // Fill AC3 buffer
        ZeroMemory(&whdrAC3, sizeof(WAVEHDR));

        whdrAC3.lpData = (char*)g_wrAC3.pData;
        whdrAC3.dwBufferLength = g_wrAC3.cbData;
    
        LogCall(waveOutPrepareHeader);
        fRes = TrapMMError(waveOutPrepareHeader(hwoAC3, &whdrAC3, sizeof(WAVEHDR)), "waveOutPrepareHeader");
        if (!fRes)
            __leave;

        // play it
        SLOG(eInfo1, "Calling waveOutWrite on PCM waveOut handle");
        fRes = TrapMMError(waveOutWrite(hwoPCM, &whdrPCM, sizeof(whdrPCM)), "waveOutWrite");
        if (!fRes)
            __leave;

        // play it
        SLOG(eInfo1, "Calling waveOutWrite on AC3 waveOut handle");
        fRes = TrapMMError(waveOutWrite(hwoAC3, &whdrAC3, sizeof(whdrAC3)), "waveOutWrite");
        if (!fRes)
            __leave;

        // spin.
        DWORD   dwTimeStart = timeGetTime();
        UINT    cbLast = 0;
        MMTIME  mmt;

        ZeroMemory(&mmt, sizeof(mmt));
        mmt.wType = TIME_BYTES;

        LOG(eInfo1, "waveOutGetPosition will be called repeatedly.  The value should increase.");

        while 
        (
            (mmt.u.cb < g_wrAC3.cbData)    &&
            (timeGetTime() - dwTimeStart < g_wrAC3.cmsData + 100)
        )
        {
            fRes = TrapMMError(waveOutGetPosition(hwoAC3, &mmt, sizeof(mmt)), "waveOutGetPosition", TRUE);
            if (!fRes)
                __leave;

            if (g_fLogPositions)
                LOG(eInfo2, "waveOutGetPosition: %5d", mmt.u.cb);

            fRes = (mmt.u.cb >= cbLast);
            if (!fRes)
            {
                LOG(eError, "Failure: The play cursor appears to have moved backward (from %d to %d)", cbLast, mmt.u.cb);
                __leave;
            }

            cbLast = mmt.u.cb;
            Sleep(2);
        }
    }

    __finally
    {
        if (hwoAC3)
        {
            waveOutReset(hwoAC3);  // should always do this before unpreparing
            waveOutUnprepareHeader(hwoAC3, &whdrAC3, sizeof(whdrAC3));
            waveOutClose(hwoAC3);
        }

        if (hwoPCM)
        {
            waveOutReset(hwoPCM);  // should always do this before unpreparing
            waveOutUnprepareHeader(hwoPCM, &whdrPCM, sizeof(whdrPCM));
            waveOutClose(hwoPCM);
        }
    }

    if (!fRes)
        dwRes = FNS_FAIL;

    return dwRes;
}

