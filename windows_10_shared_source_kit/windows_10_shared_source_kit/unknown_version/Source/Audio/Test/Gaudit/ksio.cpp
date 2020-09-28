//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
// Copyright(C) Microsoft.All rights reserved.
//
//  File:       ksio.cpp
//
//
//--------------------------------------------------------------------------

#include "common.h"

#define BUFFER_SIZE        256
#define SIZE_OF_RT_AUDIO_BUFFER_IN_MILLISECONDS 32
/* __________________________________________________________________________

    @TEST IOCTL_KS_WRITE
   ___________________________________________________________________________ */

// test case function
DWORD WINAPI Generic_Ioctl_IOCTL_KS_WRITE_1(PGAUDITPARAMS pParams)
{

    CKsPin *pPin = PIN();
    CKsFilter *pFilter = FILTER();
    bool bRT = false;
    
    // test only on pins
    if (!pPin) {
        return FNS_PASS;
    }

    // test only on data sinks
    if (KSPIN_DATAFLOW_OUT == pPin->m_Descriptor.DataFlow) {
        // can't write to a source
        return FNS_PASS;
    }    
    
    SLOG(eInfo1,
        "Testing pin %u: %s",
        pPin->m_nId,
        pPin->m_szFriendlyName
    );
    
    // brace is for scope
    {

        // PCM (Pulse Code Modulation) pins that are also RT (Real Time) need a buffer
        // before they can be set to the "Pause" state
        if (pPin->m_eType == ePCMAudio || pFilter->m_eType == ePCMAudio) {

            // sanity check... pin and filter should be of the same type
            if (pPin->m_eType != pFilter->m_eType) {
                SLOG(eError, "ERROR: pin's m_eType (%d) and filter's m_eType (%d) don't match", pPin->m_eType, pFilter->m_eType);
                return FNS_FAIL;
            }

            CPCMAudioPin *pPCMPin = reinterpret_cast<CPCMAudioPin*>(pPin);
            CPCMAudioFilter *pPCMFilter = reinterpret_cast<CPCMAudioFilter*>(pFilter);

            // this is a PCM pin on a PCM filter
            // check if this is a RT filter
            if (pPCMFilter->m_fRTPort) {
                bRT = true;
            
                // it's an RT PCM audio pin
                // so it needs a buffer, By Design, to be anything-but-stopped
                // the buffer in turn needs a WaveFormatEx

                // Make sure the wave format is set
				// Call the SetViablePCMWfxt with defaults
				if (pPCMPin->SetViablePCMWfxt())
				{
					PKSDATAFORMAT_WAVEFORMATEX pKSDFWFX = reinterpret_cast<PKSDATAFORMAT_WAVEFORMATEX>(pPCMPin->m_pksDataFormat);
				}
				else 
                {
                    SLOG(eError, "ERROR: Unable to set viable data format on Pin %d .", pPin->m_nId);
                    return FNS_FAIL;
                }
                              
                // check to see if a buffer has already been assigned
                if (pPCMPin->m_pbStreamData == NULL) {

                    // nope, doesn't have one... have the pin create its buffer.
                    BOOL bBufferSucceeded = 
                        pPCMPin->CreateDataBuffer(SIZE_OF_RT_AUDIO_BUFFER_IN_MILLISECONDS);
                    if (!bBufferSucceeded) {
                        SLOG(eError, "ERROR: Could not allocate %d-millisecond buffer for RT audio pin", SIZE_OF_RT_AUDIO_BUFFER_IN_MILLISECONDS);
                        return FNS_FAIL;
                    } // buffer succeeded
                } // needs buffer
            } // Real Time
        } // PCM

        // set pin state to pause
        if (!pPin->SetState(KSSTATE_PAUSE))
        {
            SLOG(eError, "ERROR: Failed to set pin %ld state to pause", pPin->m_nId);
            return FNS_FAIL;
        }

        // RT drivers don't need or want an IOCTL_KS_WRITE_STREAM
        // they just party on the one-shared-buffer-to-rule-them-all
        if (bRT) {
            // nothing very exciting here...
        
            // set pin state to start
            if (!pPin->SetState(KSSTATE_RUN)) {
                SLOG(eError, "ERROR: Failed to set pin %ld state to start", pPin->m_nId);
                return FNS_FAIL;
            }

            // cancel IO
            BOOL bCancelSucceeded = CancelIo(pParams->hFile);

            if (!bCancelSucceeded) {
                DWORD dwErr = GetLastError();
                SLOG(eError, "ERROR: CancelIo failed: GetLastError returns 0x%08x", dwErr);
                return FNS_FAIL;
            }

            if (!pPin->SetState(KSSTATE_STOP)) {
                SLOG(eError, "ERROR: Failed to set pin %ld state to stop", pPin->m_nId);
                return FNS_FAIL;
            }

        } else {
            // send an audio packet down
            CGenericIoControl gnt(pParams->hFile, NULL,
                                  0, 0, 
                                  sizeof(KSSTREAM_HEADER) + BUFFER_SIZE, sizeof(KSSTREAM_HEADER),
                                  ALLOCATOR_STATIC,
                                  pParams->Ioctl);

            PKSSTREAM_HEADER pFrame = (PKSSTREAM_HEADER)gnt.GetData();
            pFrame->Size = sizeof(KSSTREAM_HEADER);
            pFrame->Data = pFrame + 1;
            pFrame->FrameExtent = BUFFER_SIZE;
            pFrame->DataUsed = BUFFER_SIZE;
            pFrame->PresentationTime.Time          = 0;
            pFrame->PresentationTime.Numerator     = 1;
            pFrame->PresentationTime.Denominator   = 1;
            pFrame->OptionsFlags =  KSSTREAM_HEADER_OPTIONSF_ENDOFSTREAM;

            // run async
            DWORD dwResult = gnt.Test(FALSE, 0, TRUE);

            if (ERROR_IO_PENDING != dwResult) {
                SLOG(eError,
                    "ERROR: attempt to prime data returned %u instead of ERROR_IO_PENDING",
                    dwResult,
                    ERROR_IO_PENDING
                );
                return FNS_FAIL;
            }

            // set pin state to start
            if (!pPin->SetState(KSSTATE_RUN)) {
                SLOG(eError, "ERROR: Failed to set pin %ld state to start", pPin->m_nId);
                return FNS_FAIL;
            }

            // cancel IO
            BOOL bCancelSucceeded = CancelIo(pParams->hFile);

            if (!bCancelSucceeded) {
                DWORD dwErr = GetLastError();
                SLOG(eError, "ERROR: CancelIo failed: GetLastError returns 0x%08x", dwErr);
                return FNS_FAIL;
            }
            
            SLOG(eInfo1, "CancelIo succeeded.");

            if (!pPin->SetState(KSSTATE_STOP)) {
                SLOG(eError, "ERROR: Failed to set pin %ld state to stop", pPin->m_nId);
                return FNS_FAIL;
            }

            if (gnt.GetFault() != FAULT_NO_FAULT) {
                SLOG(eError, "ERROR: GetFault returned 0x%x", gnt.GetFault());
                return FNS_FAIL;
            }
        } // non-waveRT
      } // scope

    return FNS_PASS;
}
