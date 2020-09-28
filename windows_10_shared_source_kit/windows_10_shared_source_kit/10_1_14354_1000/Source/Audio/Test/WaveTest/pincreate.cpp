#include "PreComp.h"
#include <dvmodule.h>
#include "WaveTestTaef.h"

//#ifdef BUILD_FOR_MOBILE_CORE

const unsigned PHONE_SIGNAL_PROCESSING_MODE_NUM = 4;
const GUID PHONE_SIGNAL_PROCESSING_MODES[PHONE_SIGNAL_PROCESSING_MODE_NUM] =
{
    AUDIO_SIGNALPROCESSINGMODE_COMMUNICATIONS, 
    AUDIO_SIGNALPROCESSINGMODE_SPEECH, 
    AUDIO_SIGNALPROCESSINGMODE_MEDIA,
    AUDIO_SIGNALPROCESSINGMODE_MOVIE, 
};

//#endif

//
// stolen from wdm.h
//
#define FILE_DEVICE_KS                  0x0000002f
#define METHOD_NEITHER                  3
#define FILE_ANY_ACCESS                 0

#define CTL_CODE( DeviceType, Function, Method, Access )    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method)

// ----------------------------------------------------------------------------------
// BitsSetInDWORD	- Finding no of Bits set into DWORD
// ----------------------------------------------------------------------------------
USHORT
BitsSetInDWORD
(
    DWORD   dw
)
{
    USHORT u;
    USHORT c = 0;

    for(u = 0; u < 32; u++)
    {
        if(dw & 0x00000001)
            c++;

        dw >>= 1;
    }

    return c;
}

// ----------------------------------------------------------------------------------
// KSPIN_CONNECT_Dump
// ----------------------------------------------------------------------------------
void
KSPIN_CONNECT_Dump
(
    PKSPIN_CONNECT_WAVEFORMAT_EXTENSIBLE    pkspc
)
{
    SLOG(eBlab2, "Creating Pin with format:");

    SLOG(eBlab2, "   Connect.nPinId                    = %d", pkspc->Connect.PinId );
    SLOG(eBlab2, "   Connect.PinToHandle               = 0x%p", pkspc->Connect.PinToHandle );
    SLOG(eBlab2, "   Format.FormatSize                 = %d", pkspc->DataFormat.FormatSize );
    SLOG(eBlab2, "   WaveFormat.Format.wFormatTag      = %d", pkspc->WaveFormat.Format.wFormatTag );
    SLOG(eBlab2, "   WaveFormat.Format.nChannels       = %d", pkspc->WaveFormat.Format.nChannels );
    SLOG(eBlab2, "   WaveFormat.Format.nSamplesPerSec  = %d", pkspc->WaveFormat.Format.nSamplesPerSec );
    SLOG(eBlab2, "   WaveFormat.Format.nAvgBytesPerSec = %d", pkspc->WaveFormat.Format.nAvgBytesPerSec );
    SLOG(eBlab2, "   WaveFormat.Format.nBlockAlign     = %d", pkspc->WaveFormat.Format.nBlockAlign );
    SLOG(eBlab2, "   WaveFormat.Format.wBitsPerSample  = %d", pkspc->WaveFormat.Format.wBitsPerSample );
    SLOG(eBlab2, "   WaveFormat.Format.cbSize          = %d", pkspc->WaveFormat.Format.cbSize );
    SLOG(eBlab2, "   WaveFormat.wValidBitsPerSample    = %d", pkspc->WaveFormat.Samples.wValidBitsPerSample );
    SLOG(eBlab2, "   WaveFormat.dwChannelMask          = %d", pkspc->WaveFormat.dwChannelMask);
}

// ----------------------------------------------------------------------------------
// KSPIN_CONNECT_Init	- Intialize Pin
// ----------------------------------------------------------------------------------
void
KSPIN_CONNECT_Init
(
    PKSPIN_CONNECT_WAVEFORMAT_EXTENSIBLE    pkspc, 
    UINT                                    nPinId
)
{
    pkspc->Connect.Interface.Set = KSINTERFACESETID_Standard;
    pkspc->Connect.Interface.Id = KSINTERFACE_STANDARD_STREAMING;
    pkspc->Connect.Interface.Flags = 0;
    pkspc->Connect.Medium.Set = KSMEDIUMSETID_Standard;
    pkspc->Connect.Medium.Id = KSMEDIUM_TYPE_ANYINSTANCE;
    pkspc->Connect.Medium.Flags = 0;
    pkspc->Connect.PinId = nPinId;
    pkspc->Connect.PinToHandle = NULL;			// NULL menas that we r requesting connection to itself.
    pkspc->Connect.Priority.PriorityClass = KSPRIORITY_NORMAL;
    pkspc->Connect.Priority.PrioritySubClass = 1;
    pkspc->DataFormat.FormatSize = sizeof(KSDATAFORMAT) + sizeof(WAVEFORMATEX);
    pkspc->DataFormat.Flags = 0;
    pkspc->DataFormat.SampleSize = 0;
    pkspc->DataFormat.Reserved = 0;
    pkspc->DataFormat.MajorFormat = KSDATAFORMAT_TYPE_AUDIO;
    pkspc->DataFormat.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
    pkspc->DataFormat.Specifier = KSDATAFORMAT_SPECIFIER_WAVEFORMATEX;
}

// ----------------------------------------------------------------------------------
// KSPIN_CONNECT_SetPinToHandle	- In the case of a source destination, this contains the handle of the pin instance to establish a connection to. In the case of a sink destination, this field contains NULL, and is not otherwise used
// ----------------------------------------------------------------------------------
void
KSPIN_CONNECT_SetPinToHandle
(
    PKSPIN_CONNECT_WAVEFORMAT_EXTENSIBLE    pkspc, 
    HANDLE                                  hPin
)
{
    pkspc->Connect.PinToHandle = hPin;
}

// ----------------------------------------------------------------------------------
// KSPIN_CONNECT_SetWaveFormat	- Setting WAVEFormat for Pin
// ----------------------------------------------------------------------------------
void
KSPIN_CONNECT_SetWaveFormat
(
    PKSPIN_CONNECT_WAVEFORMAT_EXTENSIBLE    pkspc, 
    WAVEFORMATEX                            wfx
)
{
    CopyMemory(&pkspc->WaveFormat, &wfx, sizeof(WAVEFORMATEX));
}

// ----------------------------------------------------------------------------------
// CreatePin_WAVEFORMATEXTENSIBLE
// ----------------------------------------------------------------------------------
BOOL
CreatePin_WAVEFORMATEXTENSIBLE
(
    CKsPin*                 pKsPin,
    CKsPin*                 pKsPinConnectTo,    // Sink pin to connect.  Non-NULL implies that we are creating a source pin.
    PWAVEFORMATEXTENSIBLE   pWFX,
    BOOL                    fLooped,
    REFGUID                 guidMode
)
{
    BOOL                                    fRes = FALSE;
    PKSPIN_CONNECT_WAVEFORMAT_EXTENSIBLE    pPinConnect = NULL;
    
    pPinConnect = (PKSPIN_CONNECT_WAVEFORMAT_EXTENSIBLE)LocalAlloc(LPTR, sizeof(KSPIN_CONNECT_WAVEFORMAT_EXTENSIBLE));
    if(!pPinConnect)
        return FALSE;

    // stuff that never changes
    KSPIN_CONNECT_Init(pPinConnect, pKsPin->m_nId);
    
    if(pKsPinConnectTo)
        KSPIN_CONNECT_SetPinToHandle(pPinConnect, pKsPinConnectTo->m_handle);

    // copy supplied WAVEFORMATEXTENSIBLE to the KSPIN_CONNECT structure
    CopyMemory(&pPinConnect->WaveFormat, pWFX, sizeof(WAVEFORMATEXTENSIBLE));

    // make sure nobody's home before we move in
    SafeLocalFree(pKsPin->m_pksPinCreate);

    pKsPin->m_pksPinCreate = (PKSPIN_CONNECT)pPinConnect;
    pKsPin->m_pksDataFormat = (PKSDATAFORMAT)&pPinConnect->DataFormat;

    if (!IsEqualGUID(GUID_NULL, guidMode))
    {
        pPinConnect->DataFormat.Flags |= KSDATAFORMAT_ATTRIBUTES;
        pPinConnect->MultipleItem.Count = 1;
        pPinConnect->MultipleItem.Size = sizeof(pPinConnect->MultipleItem) + sizeof(pPinConnect->ModeAttribute);
        pPinConnect->ModeAttribute.AttributeHeader.Size = sizeof(KSATTRIBUTE_AUDIOSIGNALPROCESSING_MODE);
        pPinConnect->ModeAttribute.AttributeHeader.Flags = 0;
        pPinConnect->ModeAttribute.AttributeHeader.Attribute = KSATTRIBUTEID_AUDIOSIGNALPROCESSING_MODE;
        pPinConnect->ModeAttribute.SignalProcessingMode = guidMode;
    }

    // 
    pPinConnect->DataFormat.FormatSize = sizeof(KSDATAFORMAT) + sizeof(WAVEFORMATEXTENSIBLE);
    pPinConnect->WaveFormat.Format.cbSize = 0;
    pPinConnect->WaveFormat.Format.wFormatTag = WAVE_FORMAT_PCM;


    // spew
    KSPIN_CONNECT_Dump(pPinConnect);

    // make it so
    fRes = pKsPin->Instantiate(fLooped);

    return fRes;
}


BOOL 
InstantiatePinWfx
(
    CPCMAudioPin   *pPin,
    WAVEFORMATEX   *pWfx,
    BOOL            fLooped,
    BOOL            bRawSupport,
    BOOL            bDefaultSupport
)
{
    GUID mode =
    (
        bRawSupport     ? AUDIO_SIGNALPROCESSINGMODE_RAW     :
        bDefaultSupport ? AUDIO_SIGNALPROCESSINGMODE_DEFAULT :
        KSLIB_AUDIOPROCESSINGMODE_AUTO
    );

    if (!IsEqualGUID(g_pWaveTest->m_pHalf->m_Mode, GUID_NULL))
    {
        mode = g_pWaveTest->m_pHalf->m_Mode;
    }

    if (pWfx)
    {
        // we've been handed a format; use that
        if(!pPin->SetFormat(pWfx, mode))
        {
            XLOG(XMSG, XFAIL, "Setting the specified format on the pin failed");
            return FALSE;
        }
    }
    else
    {
        // let the pin choose the format
        if(!pPin->SetViablePCMWfxt(NULL, mode))
        {
            XLOG(XFAIL, eError, "Unable to find a viable format");
            return FALSE;
        }
    }

    XLOG(XMSG, eInfo1, "Wave Format used for instantiation:");
    CComPtr<IBasicLog> pShellBasicLog;

    pShellBasicLog = g_pBasicLog;
    LogWaveFormat(pShellBasicLog, pPin->GetFormat());

    BOOL fRtn = pPin->Instantiate(fLooped);

//#ifdef BUILD_FOR_MOBILE_CORE
    if (!fRtn && pWfx)
    {
        // Try the phone only modes
        for (unsigned i = 0; (i < PHONE_SIGNAL_PROCESSING_MODE_NUM) && !fRtn; i++)
        {
            if(!pPin->SetFormat(pWfx, PHONE_SIGNAL_PROCESSING_MODES[i]))
            {
                OLECHAR wszProcessingMode[40] = {};
                CHAR szProcessingMode[40] = {};
                StringFromGUID2(PHONE_SIGNAL_PROCESSING_MODES[i], wszProcessingMode, ARRAYSIZE(wszProcessingMode));
                WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, wszProcessingMode, (int)ARRAYSIZE(wszProcessingMode),
                    szProcessingMode, (int)ARRAYSIZE(szProcessingMode), NULL, NULL);

                XLOG(XMSG, XFAIL, "Setting the specified format on the pin failed with processing mode %s", szProcessingMode);
                return FALSE;
            }
            fRtn = pPin->Instantiate(fLooped);
        }
    }
//#endif

    return fRtn;
}

