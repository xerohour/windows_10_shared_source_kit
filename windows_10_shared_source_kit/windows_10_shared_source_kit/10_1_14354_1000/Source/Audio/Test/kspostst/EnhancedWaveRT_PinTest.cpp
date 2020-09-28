// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
// Module Name:
//
//  EnhancedWaveRT_PinTest.cpp
//
// Abstract:
//
//  Implementation of Enhanced WaveRT streaming position tests.
//



// -------------------------------------------------------------------------------

#include "PreComp.h"
#include <functional>
#include <waveio.h>
#include <KsPin.h>
#include <KsPinFactory.h>
#include <AudioOffload.h>

// If a hardware solution supports offloaded audio render processing, the startup latency must be less than:
//      - 80ms for compressed formats
//      - 15ms for PCM.
// If a hardware solution supports offloaded audio render processing, the pause or stop latency must be less than 10ms.

enum EPresentationPositionState
{
    PresentationPosition_NonZero,
    PresentationPosition_Paused,
    PresentationPosition_Stopped
};

// ------------------------------------------------------------------------------
// Declaration of Helper Functions
// ------------------------------------------------------------------------------
_Success_(return == FNS_PASS) DWORD PlayPCMSineWave( _In_ UINT msTimeForPlayWait, _Outptr_ KsRenderPin** ppKsRenderPin,
    _Out_ double* tPreRunState, _Out_ double* tPostRunState, _Out_ double* tPreGetPosition, _Out_ double* tPostGetPosition );
_Success_(return == FNS_PASS) DWORD PlayMP3SineWave( _In_ UINT msTimeForPlayWait, _Outptr_ KsRenderPin** ppKsRenderPin,
    _Out_ double* tPreRunState, _Out_ double* tPostRunState, _Out_ double* tPreGetPosition, _Out_ double* tPostGetPosition );
_Success_(return == FNS_PASS) DWORD WaitForPresentationPositionState( _In_ KsRenderPin* pKsRenderPin, _In_ UINT msTimeToWait,
    _In_ EPresentationPositionState ePresentationPositionState, _Out_ double* tPrePositionState, _Out_ double* tPostPositionState );
_Success_(return == FNS_PASS) DWORD GetEnhancedWaveRTRenderPin( KsRenderPin** ppKsRenderPin );
_Success_(return == FNS_PASS) DWORD GetMP3FileWfxAndBuffer( LPCWSTR mp3Filename, WAVEFORMATEX** ppWfx, BYTE** ppBuffer,
    size_t* pByteBufferSize );
_Success_(return == FNS_PASS) DWORD ValidateRequirement( LPCTSTR strWho, LPCTSTR strWhat, double msTimeAllowed, double msRequirement,
    double msActual );

// ------------------------------------------------------------------------------
// Timer Macro
// ------------------------------------------------------------------------------
#define xGetTime    (g_pKsPosTst->m_pTimer->Proc)

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
DWORD Test_ERT_PCMStartLatency()
{
    DWORD dwReturn = FNS_PASS;
    KsRenderPin* pKsRenderPin = nullptr;
    double tPreRunState = 0.0;
    double tPostRunState = 0.0;
    double tPreGetPosition = 0.0;
    double tPostGetPosition = 0.0;

    double tStartupLatencyLimitInMS = 15.0;
    UINT uTimeForStartInMS = (UINT)tStartupLatencyLimitInMS * 10;
    NTSTATUS lTimerResStatus;

    lTimerResStatus = NtSetTimerResolution(timerResolution, TRUE, &actualTimerResolution);

    if (!NT_SUCCESS(lTimerResStatus))
    {
        XLOG(XSKIP, eInfo1, "Fail to set timer precision. Skip the test.");
        return FNS_SKIPPED;
    }

    // Step 1: Start streaming PCM data to the Enhanced WaveRT Pin
    //
    dwReturn = PlayPCMSineWave( uTimeForStartInMS, &pKsRenderPin, &tPreRunState, &tPostRunState, &tPreGetPosition, &tPostGetPosition );
    if (FNS_PASS != dwReturn)
    {
        // logging is completed in PlayPCMSineWave
        goto Exit;
    }

    // pin isn't needed anymore, free resource
    pKsRenderPin->Stop();

    delete pKsRenderPin;
    pKsRenderPin = nullptr;

    // Step 2: Validate it meets the Requirement
    //
    if (tPreGetPosition == 0.0 && tPostGetPosition == 0.0)
    {
        dwReturn = ValidateRequirement(TEXT("Offload Capable Pin"), TEXT("start streaming PCM content"), uTimeForStartInMS,
            tStartupLatencyLimitInMS, -1.0);
    }
    else
    {
        // stream started
        dwReturn = ValidateRequirement(TEXT("Offload Capable Pin"), TEXT("start streaming PCM content"), uTimeForStartInMS,
            tStartupLatencyLimitInMS, tPreGetPosition - tPostRunState);
    }

Exit:

    if (nullptr != pKsRenderPin)
    {
        pKsRenderPin->Stop();
        delete pKsRenderPin;
        pKsRenderPin = nullptr;
    }

    // Turn off the higher resolution timer
    NtSetTimerResolution(0 /*unused*/, FALSE, &actualTimerResolution); 

    return dwReturn;
}

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
DWORD Test_ERT_PauseLatency()
{
    HRESULT hr = S_OK;
    DWORD dwReturn = FNS_PASS;
    KsRenderPin* pKsRenderPin = nullptr;
    double tPreRunState = 0.0;
    double tPostRunState = 0.0;
    double tPostPauseState = 0.0;
    double tPreGetPosition = 0.0;
    double tPostGetPosition = 0.0;

    double tPauseLatencyLimitInMS = 10.0;
    UINT uTimeForPauseInMS = (UINT)tPauseLatencyLimitInMS * 10;
    UINT uTimeForStartupInMS = 160;
    NTSTATUS lTimerResStatus;

    lTimerResStatus = NtSetTimerResolution(timerResolution, TRUE, &actualTimerResolution);

    if (!NT_SUCCESS(lTimerResStatus)) 
    {
        XLOG(XSKIP, eInfo1, "Fail to set timer precision. Skip the test.");
        return FNS_SKIPPED;
    }

    // Step 1: Start streaming PCM data to the Enhanced WaveRT Pin
    //
    dwReturn = PlayPCMSineWave( uTimeForStartupInMS, &pKsRenderPin, &tPreRunState, &tPostRunState, &tPreGetPosition,
        &tPostGetPosition );
    if (FNS_PASS != dwReturn)
    {
        // logging is completed in PlayPCMSineWave
        goto Exit;
    }
    
    if (tPreGetPosition == 0 && tPostGetPosition == 0)
    {
        // stream position never moved
        XLOG(XFAIL, eError, "FAIL: Failed to start streaming PCM audio data to Enhanced WaveRT Pin within %ums.", uTimeForStartupInMS);
        dwReturn = FNS_FAIL;

        goto Exit;
    }

    // Step 2: Pause the stream
    //
    hr = pKsRenderPin->PausePin();
    tPostPauseState = xGetTime();

    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: Failed to Pause Pin.  (0x%08x)", hr);
        dwReturn = FNS_FAIL;

        goto Exit;
    }

    // Step 3: Check Presentation Position for paused position
    //
    dwReturn = WaitForPresentationPositionState(pKsRenderPin, uTimeForPauseInMS, PresentationPosition_Paused, &tPreGetPosition,
        &tPostGetPosition);
    if (FNS_PASS != dwReturn)
    {
        // logging is completed in WaitForPresentationPositionState
        goto Exit;
    }

    XLOG(XMSG, eInfo2, "PostPauseTime: %g PreGetPosition: %g", tPostPauseState, tPreGetPosition);

    // Step 4: Validate it meets the Requirement
    //
    if (tPreGetPosition == 0.0 && tPostGetPosition == 0.0)
    {
        dwReturn = ValidateRequirement(TEXT("Offload Capable Pin"), TEXT("pause streaming PCM content"), uTimeForPauseInMS,
            tPauseLatencyLimitInMS, -1.0);
    }
    else
    {
        // stream paused
        dwReturn = ValidateRequirement(TEXT("Offload Capable Pin"), TEXT("pause streaming PCM content"), uTimeForPauseInMS,
            tPauseLatencyLimitInMS, tPreGetPosition - tPostPauseState);
    }

Exit:

    if( nullptr != pKsRenderPin )
    {
        pKsRenderPin->Stop();
        delete pKsRenderPin;
        pKsRenderPin = nullptr;
    }

    // Turn off the higher resolution timer
    NtSetTimerResolution(0 /*unused*/, FALSE, &actualTimerResolution); 

    return dwReturn;
}

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
DWORD Test_ERT_StopLatency()
{
    HRESULT hr = S_OK;
    DWORD dwReturn = FNS_PASS;
    KsRenderPin* pKsRenderPin = nullptr;
    double tPreRunState = 0.0;
    double tPostRunState = 0.0;
    double tPostStopState = 0.0;
    double tPreGetPosition = 0.0;
    double tPostGetPosition = 0.0;

    double tStopLatencyLimitInMS = 10.0;
    UINT uTimeForStopInMS = (UINT)tStopLatencyLimitInMS * 10;
    UINT uTimeForStartupInMS = 160;
    NTSTATUS lTimerResStatus;

    lTimerResStatus = NtSetTimerResolution(timerResolution, TRUE, &actualTimerResolution);

    if (!NT_SUCCESS(lTimerResStatus)) 
    {
        XLOG(XSKIP, eInfo1, "Fail to set timer precision. Skip the test.");
        return FNS_SKIPPED;
    }

    // Step 1: Start streaming PCM data to the Enhanced WaveRT Pin
    //
    dwReturn = PlayPCMSineWave( uTimeForStartupInMS, &pKsRenderPin, &tPreRunState, &tPostRunState, &tPreGetPosition,
        &tPostGetPosition );
    if (FNS_PASS != dwReturn)
    {
        // logging is completed in PlayPCMSineWave
        goto Exit;
    }

    if (tPreGetPosition == 0 && tPostGetPosition == 0)
    {
        // stream position never moved
        XLOG(XFAIL, eError, "FAIL: Failed to start streaming PCM audio data to Enhanced WaveRT Pin within %ums.", uTimeForStartupInMS);
        dwReturn = FNS_FAIL;

        goto Exit;
    }

    // Step 2: Stop the stream
    //
    hr = pKsRenderPin->StopPin();
    tPostStopState = xGetTime();

    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: Failed to Stop Pin.  (0x%08x)", hr);
        dwReturn = FNS_FAIL;

        goto Exit;
    }

    // Step 3: Check Presentation Position for stopped position
    //
    dwReturn = WaitForPresentationPositionState(pKsRenderPin, uTimeForStopInMS, PresentationPosition_Stopped, &tPreGetPosition,
        &tPostGetPosition);
    if (FNS_PASS != dwReturn)
    {
        // logging is completed in WaitForPresentationPositionState
        goto Exit;
    }

    // Step 4: Validate it meets the Requirement
    //
    if (tPreGetPosition == 0.0 && tPostGetPosition == 0.0)
    {
        dwReturn = ValidateRequirement(TEXT("Offload Capable Pin"), TEXT("stop streaming PCM content"), uTimeForStopInMS,
            tStopLatencyLimitInMS, -1.0);
    }
    else
    {
        // stream stopped
        dwReturn = ValidateRequirement(TEXT("Offload Capable Pin"), TEXT("stop streaming PCM content"), uTimeForStopInMS,
            tStopLatencyLimitInMS, tPreGetPosition - tPostStopState);
    }

Exit:

    if( nullptr != pKsRenderPin )
    {
        pKsRenderPin->Stop();
        delete pKsRenderPin;
        pKsRenderPin = nullptr;
    }

    // Turn off the higher resolution timer
    NtSetTimerResolution(0 /*unused*/, FALSE, &actualTimerResolution); 

    return dwReturn;
}

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
_Success_(return == FNS_PASS) DWORD PlayPCMSineWave( _In_ UINT msTimeForPlayWait, _Outptr_ KsRenderPin** ppKsRenderPin,
    _Out_ double* tPreRunState, _Out_ double* tPostRunState, _Out_ double* tPreGetPosition, _Out_ double* tPostGetPosition )
{
    HRESULT hr = S_OK;
    DWORD dwReturn = FNS_PASS;
    KsRenderPin* pKsRenderPin = nullptr;
    WAVEFORMATEX* pWfx = &g_pKsPosTst->m_pHalf->m_pFmtCurrent->wfxFormat.Format;

    *ppKsRenderPin = nullptr;
    *tPreRunState = 0.0;
    *tPostRunState = 0.0;
    *tPreGetPosition = 0.0;
    *tPostGetPosition = 0.0;

    dwReturn = GetEnhancedWaveRTRenderPin( &pKsRenderPin );
    if( FNS_PASS != dwReturn )
    {
        // logging is completed in GetEnhancedWaveRTRenderPin
        return dwReturn;
    }

    // Step 1: Initialize the KS Render Pin
    //    a. KsCreatePin for a looped pin with the provided format
    hr = pKsRenderPin->InitializePin( pWfx, 10, KSLIB_AUDIOPROCESSINGMODE_AUTO );
    if( S_OK != hr )
    {
        XLOG( XFAIL, eError, "FAIL: Failed to Initialize KS Render Pin.  (0x%08x)", hr );

        delete pKsRenderPin;
        pKsRenderPin = nullptr;

        return FNS_FAIL;
    }

    // Step 2: Play the sine wave to the pin
    //    a: Allocate the DMA buffer
    //    b: set pin state to stop, then pause
    //    c: create render thread
    //        i: pre-rolls audio data
    //       ii: set pin state to run
    *tPreRunState = xGetTime();
    hr = pKsRenderPin->PlaySineWaveAsync( 220.0f, 0.1f, Method_NoDithering );
    *tPostRunState = xGetTime();

    if( S_OK != hr )
    {
        XLOG( XFAIL, eError, "FAIL: Failed to Play the Sine Wave Asynchronously.  (0x%08x)", hr );

        pKsRenderPin->Stop();
        delete pKsRenderPin;
        pKsRenderPin = nullptr;

        return FNS_FAIL;
    }

    // Step 3: Check Presentation Position for first non-zero
    //
    dwReturn = WaitForPresentationPositionState( pKsRenderPin, msTimeForPlayWait, PresentationPosition_NonZero, tPreGetPosition,
        tPostGetPosition );
    if( FNS_PASS != dwReturn )
    {
        // logging is completed in WaitForPresentationPositionState
        pKsRenderPin->Stop();
        delete pKsRenderPin;
        pKsRenderPin = nullptr;

        return dwReturn;
    }

    *ppKsRenderPin = pKsRenderPin;

    return FNS_PASS;
}

_Success_(return == FNS_PASS) DWORD WaitForPresentationPositionState( _In_ KsRenderPin* pKsRenderPin, _In_ UINT msTimeToWait,
    _In_ EPresentationPositionState ePresentationPositionState, _Out_ double* tPrePositionState, _Out_ double* tPostPositionState )
{
    HRESULT hr = S_OK;
    UINT64 u64PausedPositionInBlocks = 0;
    UINT64 u64PositionInBlocks = 0;
    UINT64 u64QPCPosition = 0;
    double tPrePresentationPosition = 0.0;
    double tPostPresentationPosition = 0.0;
    UINT matchCount = 0;

    *tPrePositionState = 0.0;
    *tPostPositionState = 0.0;

    switch( ePresentationPositionState )
    {
        case PresentationPosition_NonZero:
            // Check Presentation Position for first non-zero
            //
            for( UINT tick = 0; tick < msTimeToWait; tick++ )
            {
                tPrePresentationPosition = xGetTime();
                hr = pKsRenderPin->GetPresentationPosition( &u64PositionInBlocks, &u64QPCPosition );
                tPostPresentationPosition = xGetTime();

                if( S_OK != hr )
                {
                    XLOG( XFAIL, eError, "FAIL: Failed to Get Presentation Position.  (0x%08x)", hr );

                    return FNS_FAIL;
                }

                if( u64PositionInBlocks != 0 )
                {
                    *tPrePositionState = tPrePresentationPosition;
                    *tPostPositionState = tPostPresentationPosition;

                    break;
                }

                Sleep(1);
            }
            break;

        case PresentationPosition_Paused:
        {
            // Check Presentation Position for paused position
            //
            int array_index = 0;
            UINT64 position_array[100];
            double time_array[100];
            ZeroMemory( position_array, sizeof(UINT64) * 100 );
            ZeroMemory( time_array, sizeof(double) * 100 );

            for( UINT tick = 0; tick < msTimeToWait; tick++, array_index++ )
            {
                double tPreCurrGetPosition = xGetTime();
                hr = pKsRenderPin->GetPresentationPosition( &u64PositionInBlocks, &u64QPCPosition );
                double tPostCurrGetPosition = xGetTime();

                if( S_OK != hr )
                {
                    XLOG( XFAIL, eError, "FAIL: Failed to Get Presentation Position.  (0x%08x)", hr );

                    return FNS_FAIL;
                }

                time_array[array_index] = tPreCurrGetPosition;
                position_array[array_index] = u64PositionInBlocks;

                if( u64PositionInBlocks != u64PausedPositionInBlocks )
                {
                    tPrePresentationPosition = tPreCurrGetPosition;
                    tPostPresentationPosition = tPostCurrGetPosition;
                    u64PausedPositionInBlocks = u64PositionInBlocks;
                    matchCount = 0;
                }
                else
                {
                    matchCount++;
                }

                if( matchCount > 20 )
                {
                    *tPrePositionState = tPrePresentationPosition;
                    *tPostPositionState = tPostPresentationPosition;

                    break;
                }

                Sleep(1);
            }

            XLOG( XMSG, eInfo2, "** Done collecting positions **" );
            XLOG( XMSG, eInfo2, "Total positions collected: %d", array_index );
            XLOG( XMSG, eInfo2, "Output format: index #: time the position was read, presentation position read" );
            for( int i = 0; i < array_index; i++ )
            {
                XLOG( XMSG, eInfo2, "Index %d: %g, %I64u", i, time_array[i], position_array[i] );
            }
            XLOG( XMSG, eInfo2, "** **" );

            break;
        }
        case PresentationPosition_Stopped:
            // Check Presentation Position for stopped position
            //
            for( UINT tick = 0; tick < msTimeToWait; tick++ )
            {
                tPrePresentationPosition = xGetTime();
                hr = pKsRenderPin->GetPresentationPosition( &u64PositionInBlocks, &u64QPCPosition );
                tPostPresentationPosition = xGetTime();

                if( S_OK != hr )
                {
                    XLOG( XFAIL, eError, "FAIL: Failed to Get Presentation Position.  (0x%08x)", hr );

                    return FNS_FAIL;
                }

                if( 0 == u64PositionInBlocks )
                {
                    *tPrePositionState = tPrePresentationPosition;
                    *tPostPositionState = tPostPresentationPosition;

                    break;
                }

                Sleep(1);
            }
            break;

        default:
            return FNS_FAIL;
    }

    return FNS_PASS;
}

_Success_(return == FNS_PASS) DWORD GetEnhancedWaveRTRenderPin( KsRenderPin** ppKsRenderPin )
{
    HRESULT hr = S_OK;
    CKsFilter* pFilter = g_pKsPosTst->m_pHalf->m_pFilter;
    CPCMAudioPin* pPin = g_pKsPosTst->m_pHalf->m_pPin;
    CComPtr<IBasicLog> spBasicLog;
    bool bOffloadSupported = false;
    DWORD dwPinCapabilities = 0;
    KsPin::PinType ksPinType = KsPin::UNKNOWN_PIN_TYPE;
    KsRenderPin* pKsRenderPin = nullptr;

    if( nullptr == ppKsRenderPin )
    {
        XLOG( XBLOCK, eError, "BLOCK: KS Render Pin passed to GetEnhancedWaveRTRenderPing is NULL." );

        return FNS_FAIL;
    }

    *ppKsRenderPin = nullptr;

    spBasicLog = g_pBasicLog;

    hr = AudioOffload::DoesPinSupportOffload( spBasicLog, pPin, AudioOffload::KS_API, bOffloadSupported );
    if( S_OK != hr )
    {
        XLOG( XBLOCK, eError, "BLOCK: Unable to determine if pin supports offload.  (0x%08x)", hr );

        return FNS_FAIL;
    }

    if( !bOffloadSupported )
    {
        XLOG( XSKIP, eError, "SKIP: Pin does not support Audio Offload." );

        return FNS_SKIPPED;
    }

    hr = KsPinFactory::GetKsPinCapabilities( (CPCMAudioFilter *)pFilter, pPin->m_nId, &dwPinCapabilities, spBasicLog );
    if( S_OK != hr )
    {
        XLOG( XFAIL, eError, "FAIL: Failed to Get KS Pin Capabilities.  (0x%08x)", hr );

        return FNS_FAIL;
    }

    if( 0 < (dwPinCapabilities & STREAM_CAP_ENHANCEDRTNOTIFICATION) )
    {
        ksPinType = KsPin::SINK_ENHANCED_RT_NOTIFICATION_PIN_TYPE;
    }
    else if( 0 < (dwPinCapabilities & STREAM_CAP_ENHANCEDREALTIME) )
    {
        ksPinType = KsPin::SINK_ENHANCED_RT_PIN_TYPE;
    }
    else
    {
        XLOG( XSKIP, eError, "SKIP: Pin does not support Enhanced WaveRT Streaming." );

        return FNS_SKIPPED;
    }

    pKsRenderPin = (KsRenderPin *)KsPinFactory::GetKsPin( (CPCMAudioFilter *)pFilter, pPin->m_nId, ksPinType, spBasicLog );
    if( nullptr == pKsRenderPin )
    {
        XLOG( XFAIL, eError, "FAIL: Unable to Get KS Pin." );

        return FNS_FAIL;
    }

    *ppKsRenderPin = pKsRenderPin;

    return FNS_PASS;
}

_Success_(return == FNS_PASS) DWORD GetMP3FileWfxAndBuffer( LPCWSTR mp3Filename, WAVEFORMATEX** ppWfx, BYTE** ppBuffer,
    size_t* pByteBufferSize )
{
    HRESULT hr = S_OK;
    CComPtr<IBasicLog> spBasicLog;
    CComPtr<IWaveFileSource> spWfs;
    CComHeapPtr<BYTE> spBuffer;
    CComHeapPtr<WAVEFORMATEX> spWfx;
    UINT32 nFrames = 0;
    UINT32 nBytes = 0;

    spBasicLog = g_pBasicLog;

    hr = CreateWaveFileSource( spBasicLog, XBLOCK, mp3Filename, 0, &spWfs );
    if( S_OK != hr )
    {
        XLOG( XBLOCK, eError, "BLOCK: GetMP3FileBuffer() Failed CreateWaveFileSource(%S) with HRESULT 0x%08x", mp3Filename, hr );

        return FNS_FAIL;
    }

    hr = spWfs->GetWaveFormat( &spWfx );
    if( S_OK != hr )
    {
        XLOG( XBLOCK, eError, "BLOCK: GetMP3FileBuffer() Failed IWaveFileSource::GetWaveFormat() with HRESULT 0x%08x", hr );

        return FNS_FAIL;
    }

    hr = spWfs->GetFrameCount( &nFrames, &nBytes );
    if( S_OK != hr )
    {
        XLOG( XBLOCK, eError, "BLOCK: GetMP3FileBuffer() Failed IWaveFileSource::GetFrameCount() with HRESULT 0x%08x", hr );

        return FNS_FAIL;
    }

    if( !spBuffer.AllocateBytes( nBytes ) )
    {
        DWORD error = GetLastError();
        XLOG( XBLOCK, eError, "BLOCK: GetMP3FileBuffer() Failed to allocate %u bytes for the MP3 Buffer with Error %d",
            nBytes, error );

        return FNS_FAIL;
    }

    hr = spWfs->FillBuffer( spBuffer, nFrames, nBytes );
    if( S_OK != hr )
    {
        XLOG( XBLOCK, eError, "BLOCK: GetMP3FileBuffer() Failed IWaveFileSource::FillBuffer(*, %d, %d) with HRESULT 0x%08x",
            nFrames, nBytes, hr);

        return FNS_FAIL;
    }

    *ppWfx = spWfx.Detach();
    *ppBuffer = spBuffer.Detach();
    *pByteBufferSize = nBytes;

    return FNS_PASS;
}

_Success_(return == FNS_PASS) DWORD ValidateRequirement( LPCTSTR strWho, LPCTSTR strWhat, double msTimeAllowed, double msRequirement,
    double msActual )
{
    if( msActual == -1.0 )
    {
        // position state was never reached
        XLOG( XFAIL, eError, "FAIL: A %s must %s within %.2fms.  After %.2fms, the %s did not %s.",
            strWho, strWhat, msRequirement, msTimeAllowed, strWho, strWhat );

        return FNS_FAIL;
    }
    else
    {
        // position state reached, now check to see if it did so within the requirement limit
        if( msActual > msRequirement )
        {
            XLOG( XFAIL, eError, "FAIL: A %s must %s within %.2fms.  However, the %s took %.4fms to %s.",
                strWho, strWhat, msRequirement, strWho, msActual, strWhat );

            return FNS_FAIL;
        }
        else
        {
            XLOG( XMSG, eInfo1, "PASS: A %s must %s within %.2fms.  The %s did %s in %.4fms.",
                strWho, strWhat, msRequirement, strWho, strWhat, msActual );

            return FNS_PASS;
        }
    }
}

OACR_WARNING_POP

