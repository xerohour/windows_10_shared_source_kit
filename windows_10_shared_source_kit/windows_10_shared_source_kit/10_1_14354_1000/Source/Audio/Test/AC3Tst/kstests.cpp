// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
// File Name:
//
//  kstests.cpp
//
// Abstract:
//
//  Implementation for class
//



// -------------------------------------------------------------------------------

#define INIT_GUID
#include <ac3tst.h>
#include <tests.h>
#include <kslib.h>
#include <mmreg.h>

#include <debug.h>


/*  for reference

typedef struct {
    KSPIN_INTERFACE Interface;
    KSPIN_MEDIUM    Medium;
    ULONG           PinId;
    HANDLE          PinToHandle;
    KSPRIORITY      Priority;
} KSPIN_CONNECT;

typedef struct {
    ULONG   FormatSize;
    ULONG   Flags;
    ULONG   SampleSize;
    ULONG   Reserved;
    GUID    MajorFormat;
    GUID    SubFormat;
    GUID    Specifier;
} KSDATAFORMAT;

typedef struct tWAVEFORMATEX
{
    WORD        wFormatTag;     
    WORD        nChannels;      
    DWORD       nSamplesPerSec; 
    DWORD       nAvgBytesPerSec;
    WORD        nBlockAlign;    
    WORD        wBitsPerSample; 
    WORD        cbSize;         
} WAVEFORMATEX;

typedef struct {
    KSDATAFORMAT    DataFormat;
    WAVEFORMATEX    WaveFormatEx;
} KSDATAFORMAT_WAVEFORMATEX;

typedef struct
{
    KSPIN_CONNECT               Connect;
    KSDATAFORMAT_WAVEFORMATEX   Format;
} KSPIN_CONNECT_WFX;

typedef struct {
    ULONG               Flags;
    ULONG               Control;
    WAVEFORMATEX        WaveFormatEx;
} KSDSOUND_BUFFERDESC, *PKSDSOUND_BUFFERDESC;

typedef struct {
    KSDATAFORMAT        DataFormat;
    KSDSOUND_BUFFERDESC BufferDesc;
} KSDATAFORMAT_DSOUND, *PKSDATAFORMAT_DSOUND;
*/

typedef struct
{
    KSPIN_CONNECT       Connect;
    KSDATAFORMAT_DSOUND Format;
} KSPIN_CONNECT_DSOUND;

#define STATIC_KSDATAFORMAT_SUBTYPE_DOLBY_AC3_SPDIF\
    DEFINE_WAVEFORMATEX_GUID(WAVE_FORMAT_DOLBY_AC3_SPDIF)
DEFINE_GUIDSTRUCT("00000092-0000-0010-8000-00aa00389b71", KSDATAFORMAT_SUBTYPE_DOLBY_AC3_SPDIF);
#define KSDATAFORMAT_SUBTYPE_DOLBY_AC3_SPDIF DEFINE_GUIDNAMED(KSDATAFORMAT_SUBTYPE_DOLBY_AC3_SPDIF)

KSPIN_CONNECT_WFX   g_pinConnectWfx =
{
    {   /* Connect */

        {   STATIC_KSINTERFACESETID_Standard, KSINTERFACE_STANDARD_STREAMING, 0 },  // Interface
        {   STATIC_KSMEDIUMSETID_Standard, KSMEDIUM_STANDARD_DEVIO, 0 },            // Medium
        0,                                                                          // PinId            !!! Remember to set this
        NULL,                                                                       // PinToHandle
        {   KSPRIORITY_NORMAL, 1 },                                                 // Priority
    },

    {   /* Format */

        {   /* DataFormat */
            sizeof( KSDATAFORMAT_WAVEFORMATEX ),                                    // FormatSize;
            0,                                                                      // Flags;
            4,                                                                      // SampleSize;
            0,                                                                      // Reserved;
            STATIC_KSDATAFORMAT_TYPE_AUDIO,                                         // MajorFormat;
            STATIC_KSDATAFORMAT_SUBTYPE_DOLBY_AC3_SPDIF,                            // SubFormat;
            STATIC_KSDATAFORMAT_SPECIFIER_WAVEFORMATEX                              // Specifier;
        },

        {   /* WaveFormatEx */
            WAVE_FORMAT_DOLBY_AC3_SPDIF,                                            // wFormatTag;     
            2,                                                                      // nChannels;      
            48000,                                                                  // nSamplesPerSec; 
            192000,                                                                 // nAvgBytesPerSec;
            4,                                                                      // nBlockAlign;    
            16,                                                                     // wBitsPerSample; 
            0                                                                       // cbSize;         
        }
    }
};

KSPIN_CONNECT_DSOUND   g_pinConnectDSound =
{
    {   /* Connect */

        {   STATIC_KSINTERFACESETID_Standard, KSINTERFACE_STANDARD_STREAMING, 0 },  // Interface
        {   STATIC_KSMEDIUMSETID_Standard, KSMEDIUM_STANDARD_DEVIO, 0 },            // Medium
        0,                                                                          // PinId            !!! Remember to set this
        NULL,                                                                       // PinToHandle
        {   KSPRIORITY_NORMAL, 1 },                                                 // Priority
    },

    {   /* Format */

        {   /* DataFormat */
            sizeof( KSDATAFORMAT_DSOUND ),                                          // FormatSize;
            0,                                                                      // Flags;
            4,                                                                      // SampleSize;
            0,                                                                      // Reserved;
            STATIC_KSDATAFORMAT_TYPE_AUDIO,                                         // MajorFormat;
            STATIC_KSDATAFORMAT_SUBTYPE_DOLBY_AC3_SPDIF,                            // SubFormat;
            STATIC_KSDATAFORMAT_SPECIFIER_DSOUND                                    // Specifier;
        },

        {   /* BufferDesc */

            0,                                                                      // Flags;
            0,                                                                      // Control;
            {   /* WaveFormatEx */

                WAVE_FORMAT_DOLBY_AC3_SPDIF,                                        // wFormatTag;     
                2,                                                                  // nChannels;      
                48000,                                                              // nSamplesPerSec; 
                192000,                                                             // nAvgBytesPerSec;
                4,                                                                  // nBlockAlign;    
                16,                                                                 // wBitsPerSample; 
                0                                                                   // cbSize;         
            }
        }
    }
};

// ------------------------------------------------------------------------------
void 
LogAudioDataRange
(
    ULONG               nOrdinal,
    PKSDATARANGE_AUDIO  pRangeAudio
)
{
    LOG(eWarn1, "DataRange[%d]:", nOrdinal);
    LOG(eWarn1, "    MaximumChannels        = %u", pRangeAudio->MaximumChannels);
    LOG(eWarn1, "    MaximumBitsPerSample   = %u", pRangeAudio->MaximumBitsPerSample);
    LOG(eWarn1, "    MinimumBitsPerSample   = %u", pRangeAudio->MinimumBitsPerSample);
    LOG(eWarn1, "    MaximumSampleFrequency = %u", pRangeAudio->MaximumSampleFrequency);
    LOG(eWarn1, "    MinimumSampleFrequency = %u", pRangeAudio->MinimumSampleFrequency);
}

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
// overcoming shortcomings of kslib...
BOOL
GetKsPosition
(
    CKsPin*             pPin,
    KSAUDIO_POSITION*   pPos
)
{
    if (! (pPos && pPin))
        return FALSE;

    return pPin->GetPropertySimple(KSPROPSETID_Audio, KSPROPERTY_AUDIO_POSITION, pPos, sizeof(KSAUDIO_POSITION));
}

BOOL
SetKsPosition
(
    CKsPin*             pPin,
    KSAUDIO_POSITION*   pPos
)
{
    if (! (pPos && pPin))
        return FALSE;

    return pPin->SetPropertySimple(KSPROPSETID_Audio, KSPROPERTY_AUDIO_POSITION, pPos, sizeof(KSAUDIO_POSITION));
}


// ------------------------------------------------------------------------------
// 
// ------------------------------------------------------------------------------
DWORD   
Tc_DataRanges
(
    void
)
{
    DWORD       dwRes = FNS_PASS;
    CPCMPinDev* pKsPinDev = (CPCMPinDev*)g_pCurDevice;

    __try
    {
        BOOL    fGotDSound = FALSE;
        BOOL    fGotWfx = FALSE;
        CKsPin* pPin = pKsPinDev->m_pPin;
        BOOL    fPinCanDoAC3 = FALSE;
        BOOL    fPinCanDo48kHzAC3 = FALSE;

        //
        // for pre-LH, we should have at least 2 dataranges, one for WAVEFORAMTEX and one for DSOUND
        // for LH, we only need WAVEFORMATEX
        //
        PBYTE               pbRange = (PBYTE)pPin->m_Descriptor.pDataRanges;
        PKSDATARANGE_AUDIO  pRangeAudio;
        ULONG               nRange;

        for (nRange = 0; nRange < pPin->m_Descriptor.cDataRanges; nRange ++)
        {
            BOOL fDataRangeIsAC3 = FALSE;

            // cast pbRange to make it useful
            pRangeAudio = (PKSDATARANGE_AUDIO)pbRange;

            // get next range
            pbRange += pRangeAudio->DataRange.FormatSize;

            // weed out undesirable ranges
            fDataRangeIsAC3 = 
                (
                    IsEqualGUIDAligned(pRangeAudio->DataRange.MajorFormat, KSDATAFORMAT_TYPE_AUDIO)                &&
                    IS_VALID_WAVEFORMATEX_GUID(&pRangeAudio->DataRange.SubFormat)                                  &&
                    (EXTRACT_WAVEFORMATEX_ID(&pRangeAudio->DataRange.SubFormat) == WAVE_FORMAT_DOLBY_AC3_SPDIF)
                );

            if (!fDataRangeIsAC3)
            {
                LOG(eInfo1, "DataRange[%d] is non-AC-3", nRange);
                continue;
            }

            // The AC-3 transport format is always "stereo" and "16-bit" per the IEC spec
            // The sample rate can vary though because it matches the encoded format
            //
            // An AC-3 data range should have MaximumChannels == 2 (there's no MinimumChannels)
            // and MinimumBitsPerSample == 16 and MaximumBitsPerSample == 16
            // the sample rate can vary

            if (pRangeAudio->MaximumChannels != 2)
            {
                LOG(eWarn1, "Failure:  AC-3 DataRange[%d] should have MaximumChannels = 2; instead it is %u", nRange, pRangeAudio->MaximumChannels);
                LogAudioDataRange(nRange, pRangeAudio);
                dwRes = FNS_FAIL;

                if (pRangeAudio->MaximumChannels < 2)
                {
                    // doesn't really count as AC3, sorry
                    continue;
                }
            }

            if (
                pRangeAudio->MinimumBitsPerSample != 16 ||
                pRangeAudio->MaximumBitsPerSample != 16
            )
            {
                LOG(eWarn1,
                    "Failure:  AC-3 DataRange[%d] should have "
                    "(MinimumBitsPerSample, MaximumBitsPerSample) = (16, 16); "
                    "instead it is (%u, %u)",
                     nRange,
                     pRangeAudio->MinimumBitsPerSample,
                     pRangeAudio->MaximumBitsPerSample);
                LogAudioDataRange(nRange, pRangeAudio);
                dwRes = FNS_FAIL;

                if (
                    pRangeAudio->MinimumBitsPerSample > 16 ||
                    pRangeAudio->MaximumBitsPerSample < 16
                )
                {
                    // doesn't really count as AC3, sorry
                    continue;
                }
            }

            // if we got this far then this is a reasonable AC3 data range
            LOG(eInfo2, "DataRange[%d] is viable for AC-3", nRange);
            fPinCanDoAC3 = TRUE;

            //
            // The vast majority of AC-3 encoded content is sampled at 48 kHz
            // So any pin that exposes an AC-3 data range should expose a 48 kHz-capable data range

            if (
                pRangeAudio->MinimumSampleFrequency <= 48000 &&
                pRangeAudio->MaximumSampleFrequency >= 48000
            )
            {
                LOG(eInfo2, "DataRange[%d] is viable for 48 kHz AC-3", nRange);
                fPinCanDo48kHzAC3 = TRUE;
            }

            // check for waveformatex and dsound specifiers
            if (IsEqualGUIDAligned(pRangeAudio->DataRange.Specifier, KSDATAFORMAT_SPECIFIER_WAVEFORMATEX))
            {
                LOG(eInfo2, " DataRange[%d] has a _WAVEFORMATEX specifier", nRange);
                fGotWfx = TRUE;
            }
            else if (IsEqualGUIDAligned(pRangeAudio->DataRange.Specifier, KSDATAFORMAT_SPECIFIER_DSOUND))
            {
                LOG(eInfo2, " DataRange[%d] has a _DSOUND specifier", nRange);
                fGotDSound = TRUE;
            }
            else
            {
                LOG(eWarn1, "Failure:  DataRange[%d] does not have a _WAVEFORMATEX or _DSOUND specifier", nRange);
                dwRes = FNS_FAIL;
            }            
        }

        if (fPinCanDoAC3 && !fPinCanDo48kHzAC3)
        {
                LOG(eWarn1, "Failure:  %s exposes AC-3 data ranges but none are compatible with 48 kHz content", pKsPinDev->m_szRep);
                dwRes = FNS_FAIL;
        }
        
        // LH does not require a DSOUND datarange
        if (IS_LONGHORN)
        {
            if (fGotWfx && (FNS_PASS == dwRes))
            {
                LOG(eInfo2, "%s is a viable AC-3 sink pin", pKsPinDev->m_szRep);
            }
            else
            {
                LOG(eWarn1, "Failure:  %s does not have a valid AC-3 datarange for WAVEFORMATEX", pKsPinDev->m_szRep);
                dwRes = FNS_FAIL;
            }
        }
        else
        {
            if (fGotWfx && fGotDSound && (FNS_PASS == dwRes))
            {
                LOG(eInfo2, "%s is a viable AC-3 sink pin", pKsPinDev->m_szRep);
            }
            else
            {
                LOG(eWarn1, "Failure:  %s does not have valid AC-3 dataranges for both WAVEFORMATEX and DSOUND", pKsPinDev->m_szRep);
                dwRes = FNS_FAIL;
            }
        }
    }

    __finally
    {
    }

    return dwRes;
}

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
DWORD   
Tc_KsCreatePin
(
    void
)
{
    DWORD       dwRes = FNS_PASS;
    CPCMPinDev* pKsPinDev = (CPCMPinDev*)g_pCurDevice;
    CKsPin*     pPin = pKsPinDev->m_pPin;

    __try
    {

        // try WAVEFORMATEX specifier

        pPin->m_pksPinCreate = (PKSPIN_CONNECT)LocalAlloc(LPTR, sizeof(g_pinConnectWfx));
        if (!pPin->m_pksPinCreate)
        {
            dwRes = FNS_FAIL;
            LOG(eError, "Failure: Failed to allocate memory for KSPIN_CONNECT(WAVEFORMATEX)");
            __leave;
        }

        CopyMemory(pPin->m_pksPinCreate, &g_pinConnectWfx, sizeof(g_pinConnectWfx));
        pPin->m_pksPinCreate->PinId = pPin->m_nId;

        LOG(eInfo2, "Attempting to instantiate the pin using the _WAVEFORMATEX specifier");
        BOOL fLooped = ((CPCMAudioFilter*)pPin->m_pFilter)->m_fRTPort;
        LOG(eInfo2, " This is an RTPort filter so fLooped == %u", fLooped);
        if (!pPin->Instantiate(fLooped))
        {
            dwRes = FNS_FAIL;
            LOG(eError, "Failure:  Failed to create AC-3 pin using _WAVEFORMATEX specifier");
        }
        else
        {
            pPin->ClosePin();
        }

        // Try DSound specifier
        // LH does not require a DSOUND datarange
        if (!IS_LONGHORN)
        {
            SafeLocalFree(pPin->m_pksPinCreate);
            pPin->m_pksPinCreate = (PKSPIN_CONNECT)LocalAlloc(LPTR, sizeof(g_pinConnectDSound));
            if (!pPin->m_pksPinCreate)
            {
                dwRes = FNS_FAIL;
                LOG(eError, "Failure: Failed to allocate memory for KSPIN_CONNECT(DSOUND)");
                __leave;
            }

            CopyMemory(pPin->m_pksPinCreate, &g_pinConnectDSound, sizeof(g_pinConnectDSound));
            pPin->m_pksPinCreate->PinId = pPin->m_nId;

            LOG(eInfo2, "Attempting to instantiate the pin using the _DSOUND specifier");
            if (!pPin->Instantiate(FALSE))
            {
                dwRes = FNS_FAIL;
                LOG(eError, "Failure:  Failed to create AC-3 pin using _DSOUND specifier");
            }
            else
            {
                pPin->ClosePin();
            }
        }
    }

    __finally
    {
        SafeLocalFree(pPin->m_pksPinCreate);
    }

    return dwRes;
}

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
DWORD   
Tc_KsCreatePinMulti
(
    void
)
{
    BOOL            fRes = TRUE;
    DWORD           dwRes = FNS_PASS;
    DWORD           errorCode = ERROR_SUCCESS;
    CPCMPinDev*     pKsPinDev = (CPCMPinDev*)g_pCurDevice;
    CKsPin*         pPin = pKsPinDev->m_pPin;
    CPCMAudioPin*   pPin2 = NULL;

    // create a deep copy of the pin
    // BUGBUG: why do I have to cast a CPCMAudioPin** to a CKsPin**?
    errorCode = pPin->CopyPin( pPin->m_pFilter, reinterpret_cast<CKsPin **>(&pPin2) );
    if( ERROR_SUCCESS != errorCode )
    {
        LOG( eError, "Failure:  Failed to make copy of pin.  (hr=0x%08x)", HRESULT_FROM_WIN32( errorCode ) );
        return FNS_FAIL;
    }

    try
    {

        // try WAVEFORMATEX specifier

        pPin->m_pksPinCreate = (PKSPIN_CONNECT)LocalAlloc(LPTR, sizeof(g_pinConnectWfx));
        if (!pPin->m_pksPinCreate)
        {
            dwRes = FNS_FAIL;
            throw("Failure: Failed to allocate memory for KSPIN_CONNECT(WAVEFORMATEX)");
        }

        CopyMemory(pPin->m_pksPinCreate, &g_pinConnectWfx, sizeof(g_pinConnectWfx));
        pPin->m_pksPinCreate->PinId = pPin->m_nId;

        LOG(eInfo2, "Attempting to instantiate the pin using the _WAVEFORMATEX specifier");
        BOOL fLooped = ((CPCMAudioFilter*)pPin->m_pFilter)->m_fRTPort;
        LOG(eInfo2, " This is an RTPort filter so fLooped == %u", fLooped);
        fRes = pPin->Instantiate(fLooped);
        if (!fRes)
        {
            throw("Failure:  Failed to create AC-3 pin using _WAVEFORMATEX specifier");
        }

        // whether we can duplicate the buffer or not depends on the number of possible instances
        ULONG   cPossible = pKsPinDev->m_pPin->m_Descriptor.CInstancesGlobal.PossibleCount;
        BOOL    fExpectDupSuccess = (cPossible > 1);

        LOG(eInfo1, "Given the possible instance count (%d) of the render pin, a second attempt to instantiate the pin should %s", 
            cPossible, fExpectDupSuccess ? "SUCCEED" : "FAIL");

        // make a copy
        BOOL    fSecondWorked;
        
        pPin2->m_pksPinCreate = (PKSPIN_CONNECT)LocalAlloc(LPTR, sizeof(g_pinConnectWfx));

        CopyMemory(pPin2->m_pksPinCreate, &g_pinConnectWfx, sizeof(g_pinConnectWfx));
        pPin2->m_pksPinCreate->PinId = pPin2->m_nId;

        LOG(eInfo2, "Attempting to instantiate the pin a second time using the _WAVEFORMATEX specifier");
        fSecondWorked = pPin2->Instantiate(FALSE);

        if (fSecondWorked)
        {
            if (fExpectDupSuccess)
            {
                LOG(eInfo1, "A second instantiation of the pin succeeded as expected.");
            }
            else
            {
                LOG(eError, "Failure:  A second instantiation of the pin unexpectedly succeeded.  The pin must be misrepresenting its instance capabilities.");
                fRes = FALSE;
            }
        }
        else
        {
            if (fExpectDupSuccess)
            {
                LOG(eError, "Failure:  A second instantiation of the pin unexpectedly failed.  The pin might be misrepresenting its instance capabilities.");
                fRes = FALSE;
            }
            else
            {
                LOG(eInfo1, "A second instantiation of the pin failed as expected.");
            }
        }
    }

    catch(LPSTR str)
    {
        fRes = FALSE;
        LOG(eError, str);
    }

    catch(...)
    {
        fRes = FALSE;
        LOG(eError, "Failure:  Handling exception");
    }

    pPin->ClosePin();
    if( NULL != pPin2 )
    {
        pPin2->ClosePin();
        delete pPin2;
    }

    SafeLocalFree(pPin->m_pksPinCreate);

    if( FALSE == fRes )
    {
        dwRes = FNS_FAIL;
    }

    return dwRes;
}

// ------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------
BOOL
InstantiateFillAndWriteToPin
(
    CKsPin* pPin
)
{
    BOOL    fRes = TRUE;

    __try
    {
        pPin->m_pksPinCreate = (PKSPIN_CONNECT)LocalAlloc(LPTR, sizeof(g_pinConnectWfx));
        if (!pPin->m_pksPinCreate)
        {
            fRes = FALSE;
            LOG(eError, "Failure: Failed to allocate memory for KSPIN_CONNECT(WAVEFORMATEX)");
            __leave;
        }

        CopyMemory(pPin->m_pksPinCreate, &g_pinConnectWfx, sizeof(g_pinConnectWfx));
        pPin->m_pksPinCreate->PinId = pPin->m_nId;

        LOG(eInfo2, "Attempting to instantiate the pin using the _WAVEFORMATEX specifier");
        fRes = pPin->Instantiate(FALSE);
        if (!fRes)
        {
            LOG(eError, "Failure:  Failed to create AC-3 pin using _WAVEFORMATEX specifier");
            __leave;
        }

        pPin->m_pbStreamData = (PBYTE)LocalAlloc(LPTR, g_wrAC3.cbData);
        if (!pPin->m_pbStreamData)
        {
            fRes = FALSE;
            LOG(eError, "Failure: Failed to allocate memory for AC3 Stream Data");
            __leave;
        }

        CopyMemory(pPin->m_pbStreamData, g_wrAC3.pData, g_wrAC3.cbData);

        pPin->m_cbStreamData = g_wrAC3.cbData;

        fRes = pPin->SetState(KSSTATE_ACQUIRE);
        if (!fRes)
            __leave;

        fRes = pPin->SetState(KSSTATE_PAUSE);
        if (!fRes)
            __leave;

        LOG(eInfo2, "Submitting %d bytes to render pin...", g_wrAC3.cbData);
        fRes = pPin->WriteData();
        if (!fRes)
        {
            LOG(eError, "Failure:  Failed to create AC-3 pin using _WAVEFORMATEX specifier");
            __leave;
        }
    }

    __finally
    {
    }

    return fRes;
}

// ------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------
DWORD   
Tc_KsWriteStream
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

        fRes = InstantiateFillAndWriteToPin(pPin);
        if (!fRes)
            __leave;

        fRes = pPin->SetState(KSSTATE_RUN);
        if (!fRes)
            __leave;

        pPin->Wait(g_wrAC3.cmsData + 100);

        fRes = pPin->SetState(KSSTATE_PAUSE);
        if (!fRes)
            __leave;

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

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
DWORD   
Tc_KsState
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

        fRes = InstantiateFillAndWriteToPin(pPin);
        if (!fRes)
            __leave;

        // Play the data
        ULONG   msSleep = g_wrAC3.cmsData / 10;
        int     i;

        for (i = 0; i < 10; i++)
        {
            LOG(eInfo2, "Setting pin to KSSTATE_RUN");
            fRes = pPin->SetState(KSSTATE_RUN);
            if (!fRes)
                __leave;

            Sleep(msSleep);

            if (i == 10)
                break;

            LOG(eInfo2, "Setting pin to KSSTATE_PAUSE");
            fRes = pPin->SetState(KSSTATE_PAUSE);
            if (!fRes)
                __leave;
        }
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

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
DWORD   
Tc_KsGetPosition
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

        fRes = InstantiateFillAndWriteToPin(pPin);
        if (!fRes)
            __leave;

        fRes = pPin->SetState(KSSTATE_RUN);
        if (!fRes)
            __leave;

        // Spin
        KSAUDIO_POSITION    pos;
        KSAUDIO_POSITION    posLast;
        double              tStop = qpcGetTime() + g_wrAC3.cmsData + 100;

        ZeroMemory(&pos, sizeof(pos));
        ZeroMemory(&posLast, sizeof(posLast));

        LOG(eInfo1, "KSPROPERTY_AUDIO_POSITION handler will be called repeatedly.  The value should increase.");

        ASSERT(g_wrAC3.cbData);

        LOG(eInfo1, "g_wrAC3.cbData = %d", g_wrAC3.cbData);
        LOG(eInfo1, "g_wrAC3.cmsData = %d", g_wrAC3.cmsData);

        while 
        (
            (pos.PlayOffset < g_wrAC3.cbData)    &&
            (qpcGetTime() < tStop)
        )
        {
            fRes = GetKsPosition(pPin, &pos);
            if (!fRes)
            {
                LOG(eError, "Failure:  Call to KSPROPERTY_AUDIO_POSITION failed");
                __leave;
            }

            if (g_fLogPositions)
                LOG(eInfo2, "KSPROPERTY_AUDIO_POSITION: %5d (play cursor), %5d (write cursor)", (ULONG)pos.PlayOffset, (ULONG)pos.WriteOffset);

            if (pos.PlayOffset < posLast.PlayOffset)
            {
                if (pos.PlayOffset == 0)
                    break;

                LOG(eError, "Failure: The play cursor appears to have moved backward (from %d to %d)", posLast.PlayOffset, pos.PlayOffset);
                dwRes = FNS_FAIL;
                __leave;
            }

            posLast.PlayOffset = pos.PlayOffset;
            Sleep(2);
        }

        fRes = pPin->SetState(KSSTATE_PAUSE);
        if (!fRes)
            __leave;

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

// rsaad - 2006-06-08
// This test has been deprecated because SetPosition() is not being supported anymore
// by all the following drivers: Wave RT, WavePCI, USB Audio
//
//// ------------------------------------------------------------------------------
//// ------------------------------------------------------------------------------
//DWORD   
//Tc_KsGetSetPosition
//(
//    void
//)
//{
//    DWORD       dwRes = FNS_PASS;
//    BOOL        fRes = TRUE;
//    CPCMPinDev* pKsPinDev = (CPCMPinDev*)g_pCurDevice;
//    CKsPin*     pPin = pKsPinDev->m_pPin;
//    double      t1, t2, tStart, tStop;
//    ULONG       nToleranceBy;
//    ULONG       nBytesPerMs;
//    double      dMsPerByte;
//    ULONG       nDelta;
//    double      dDeltaMs;
//
//    if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL))
//        SLOG(eError, "SetThreadPriority failed");
//
//    __try
//    {
//        // TODO - Implement RT-style streaming for RTPort filters
//        if (((CPCMAudioFilter*)pPin->m_pFilter)->m_fRTPort)
//        {
//            LOG(eInfo1, " Skipping: KSWriteStream not used for RTPort filters");
//            __leave;
//        }
//
//        nBytesPerMs = g_wrAC3.pWfx->nAvgBytesPerSec / 1000;    // should = 192
//        dMsPerByte = 1000. / (double)g_wrAC3.pWfx->nAvgBytesPerSec;
//
//        fRes = InstantiateFillAndWriteToPin(pPin);
//        if (!fRes)
//            __leave;
//
//        fRes = pPin->SetState(KSSTATE_RUN);
//        if (!fRes)
//            __leave;
//
//        // Spin
//        KSAUDIO_POSITION    posSet;
//        KSAUDIO_POSITION    posGet;
//        
//        tStop = qpcGetTime() + ((double)g_wrAC3.cmsData * 4.);
//
//        posSet.PlayOffset = 0;
//
//        LOG(eInfo1, "KSPROPERTY_AUDIO_POSITION handler will be called repeatedly, and the values of the play cursor will be compared");
//        LOG(eInfo1, "\"IOCTLs duration\" refers to the delta between 2 time stamps: one taken before the \"Set\" call and one taken after the \"Get\" call.");
//
//        LOG(eInfo2, "\n  IOCTLs,      Allowed,  SetPosition,  GetPosition,           ,    Delta ms,");
//        LOG(eInfo2, "duration,    Tolerance,        Value,        Value,     Delta ,  Equivalent,\n");
//
//        while (posSet.PlayOffset < g_wrAC3.cbData)
//        {
//            t1 = qpcGetTime();
//            if (t1 > tStop)
//                break;
//
//            fRes = SetKsPosition(pPin, &posSet);
//            if (!fRes)
//            {
//                LOG(eError, "    Failure:  Call to KSPROPERTY_AUDIO_POSITION (_SET) failed");
//                __leave;
//            }
//
//            Sleep(0);
//
//            fRes = GetKsPosition(pPin, &posGet);
//            if (!fRes)
//            {
//                LOG(eError, "    Failure:  Call to KSPROPERTY_AUDIO_POSITION (_GET) failed");
//                __leave;
//            }
//
//            t2 = qpcGetTime() - t1;
//            nToleranceBy = (ULONG)((double)nBytesPerMs * (t2 + POS_ERR_TOLERANCE_MS) );
//            nDelta = abs((ULONG)(posGet.PlayOffset - posSet.PlayOffset));
//            dDeltaMs = (double)nDelta * dMsPerByte;
//
//            LOG(eInfo2, "%5.2f ms,  %5d bytes,  %5d bytes,  %5d bytes,  %3d bytes,    %5.2f ms,",
//                          t2,   nToleranceBy,  (ULONG)posSet.PlayOffset, (ULONG)posGet.PlayOffset,   nDelta, dDeltaMs);
//
//            if ((posSet.PlayOffset > posGet.PlayOffset) || (nDelta > nToleranceBy))
//            {
//                dwRes = FNS_FAIL;
//                LOG(eError, "    Failure: The value retrieved from KSPROPERTY_AUDIO_POSITION does not agree with the value set");
//                if (nDelta > nToleranceBy)
//                    LOG(eError, "    Failure: The value retrieved from KSPROPERTY_AUDIO_POSITION is not within %d bytes of the value set", nToleranceBy);
//            }
//
//            posSet.PlayOffset += 6144;  // 32 ms
//            Sleep(200);
//        }
//
//        fRes = pPin->SetState(KSSTATE_PAUSE);
//        if (!fRes)
//            __leave;
//
//        fRes = pPin->SetState(KSSTATE_ACQUIRE);
//        if (!fRes)
//            __leave;
//    }
//
//    __finally
//    {
//    }
//
//    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
//
//    SafeLocalFree(pPin->m_pbStreamData);
//
//    pPin->ClosePin();
//
//    if (!fRes)
//        dwRes = FNS_FAIL;
//
//    return dwRes;
//}


// ------------------------------------------------------------------------------
// returns time in ms using QueryPerformanceTime.
// precision is 1 us
// ------------------------------------------------------------------------------
static LARGE_INTEGER liFreq = { 0, 0 };

double qpcGetTime(void)
{
    LARGE_INTEGER liCount;

    if ((liFreq.QuadPart == 0) && (!QueryPerformanceFrequency(&liFreq)))
        return (ULONG)-1;

    QueryPerformanceCounter(&liCount);

    // convert to us
    liCount.QuadPart = liCount.QuadPart * 1000000 / liFreq.QuadPart;

    // convert to ms
    double dResult = (double)liCount.QuadPart / 1000.; 

    return dResult;
}

