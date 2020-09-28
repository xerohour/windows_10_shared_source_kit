//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
// Copyright(C) Microsoft.All rights reserved.
//
//  File:       helper.cpp
//
//
//--------------------------------------------------------------------------

#include "common.h"


/* ____________________________________________________________________

    @HELPER CreatePinInstance
   ____________________________________________________________________ */
BOOL CreatePinInstance(HANDLE hFilter, PKSDATARANGE prange, ULONG PinId, DWORD dwFilterFlags, HANDLE* phPin)
{
    PKSPIN_CONNECT pconnect = NULL;
    BOOL fResult = FALSE;
    REFER_FORMAL_PARAM(dwFilterFlags);

    if ((prange->Specifier == KSDATAFORMAT_SPECIFIER_WAVEFORMATEX) && (prange->FormatSize == sizeof (KSDATARANGE_AUDIO)))
    {
        pconnect = (PKSPIN_CONNECT)new BYTE[sizeof(KSPIN_CONNECT) + sizeof(KSDATAFORMAT_WAVEFORMATEX)];
        if (!pconnect)
            goto cleanup;

        PKSDATAFORMAT_WAVEFORMATEX pfmtwfx = PKSDATAFORMAT_WAVEFORMATEX(pconnect + 1);   
        PKSDATARANGE_AUDIO pDataRangeAudio = (PKSDATARANGE_AUDIO) prange;
        
        // for creation of Source pin we will need to supply handle where as in case of sink pin it will be ingnored by KS
        CScratchHandle Handle (FILTER());
        HANDLE NecessarySink;
        if (PIN()->m_Descriptor.Communication & KSPIN_COMMUNICATION_SINK)
            NecessarySink =  NULL;
        else {
            // we will need hanlde to supply for the creation of the pin
            if (Handle.Open ()) {
                SLOG(eError, " we are not able to get Handle for the temp file");
                goto cleanup;
            }   
            NecessarySink = Handle.GetHandle();
        }
        FillPKsPinConnect1(pconnect, PIN()->m_nId,NecessarySink, (KSPIN_INTERFACE*)(PIN()->m_Descriptor.pmiInterfaces+1),(KSPIN_MEDIUM*)( PIN()->m_Descriptor.pmiMediums+1), NULL);

        pfmtwfx->DataFormat.FormatSize = sizeof(KSDATAFORMAT_WAVEFORMATEX);
        pfmtwfx->DataFormat.Flags      = 0;
        pfmtwfx->DataFormat.SampleSize = 0;
        pfmtwfx->DataFormat.Reserved   = 0;
        pfmtwfx->DataFormat.MajorFormat = KSDATAFORMAT_TYPE_AUDIO;
        pfmtwfx->DataFormat.SubFormat   = KSDATAFORMAT_SUBTYPE_PCM;
        pfmtwfx->DataFormat.Specifier   = KSDATAFORMAT_SPECIFIER_WAVEFORMATEX;
 
        // allocate best found format
        pfmtwfx->WaveFormatEx.wFormatTag       = WAVE_FORMAT_PCM; 
        pfmtwfx->WaveFormatEx.nChannels        = (pDataRangeAudio->MaximumChannels < (ULONG)-1) ? (WORD)pDataRangeAudio->MaximumChannels : 2;
        pfmtwfx->WaveFormatEx.nSamplesPerSec   = min(pDataRangeAudio->MaximumSampleFrequency, 48000);
        pfmtwfx->WaveFormatEx.wBitsPerSample   = (WORD)min(pDataRangeAudio->MaximumBitsPerSample, 16);
        pfmtwfx->WaveFormatEx.nBlockAlign      = (WORD)(pfmtwfx->WaveFormatEx.nChannels * pfmtwfx->WaveFormatEx.wBitsPerSample / 8);
        pfmtwfx->WaveFormatEx.nAvgBytesPerSec  = pfmtwfx->WaveFormatEx.nBlockAlign * pfmtwfx->WaveFormatEx.nSamplesPerSec;
        pfmtwfx->WaveFormatEx.cbSize           = 0;
        // following lines are added as in case of PropsedDataformat test, we want to memorize dataformat of pin with which Pin is created.
        memcpy(&AudioPinFormat,(PVOID) pfmtwfx, sizeof(KSDATAFORMAT_WAVEFORMATEX));
        PinType = 1;
        DWORD dwerr = g_pGauditModule->GetCreatePinFn()(hFilter,
                    pconnect,
                    GENERIC_READ | GENERIC_WRITE,
                    phPin);

        if (dwerr)
        {
            goto cleanup;
        }
        fResult = TRUE;        
    }
	else
        if ((prange->Specifier == KSDATAFORMAT_SPECIFIER_DSOUND) && (prange->FormatSize == sizeof (KSDATARANGE_AUDIO)))
        {
            pconnect = (PKSPIN_CONNECT)new BYTE[sizeof(KSPIN_CONNECT) + sizeof(KSDATAFORMAT_DSOUND)];
            if (!pconnect)
                goto cleanup;

            PKSDATAFORMAT_DSOUND pfmtds = PKSDATAFORMAT_DSOUND(pconnect + 1);   
            PKSDATARANGE_AUDIO pDataRangeAudio = (PKSDATARANGE_AUDIO) prange;

            CScratchHandle Handle (FILTER());
            HANDLE NecessarySink;
            if (PIN()->m_Descriptor.Communication & KSPIN_COMMUNICATION_SINK)
                NecessarySink =  NULL;
            else {
               // we will need hanlde to supply for the creation of the pin
                if (Handle.Open ()) {
                    SLOG(eError, " we are not able to get Handle for the temp file");
                    goto cleanup;
                }   
                NecessarySink = Handle.GetHandle();
            }
            FillPKsPinConnect1(pconnect, PIN()->m_nId,NecessarySink, (KSPIN_INTERFACE*)(PIN()->m_Descriptor.pmiInterfaces+1),(KSPIN_MEDIUM*)( PIN()->m_Descriptor.pmiMediums+1), NULL);


            pfmtds->DataFormat.FormatSize = sizeof(KSDATAFORMAT_DSOUND);
            pfmtds->DataFormat.Flags      = 0;
            pfmtds->DataFormat.SampleSize = 0;
            pfmtds->DataFormat.Reserved   = 0;
            pfmtds->DataFormat.MajorFormat = KSDATAFORMAT_TYPE_AUDIO;
            pfmtds->DataFormat.SubFormat   = KSDATAFORMAT_SUBTYPE_PCM;
            pfmtds->DataFormat.Specifier      = KSDATAFORMAT_SPECIFIER_DSOUND;
            pfmtds->BufferDesc.Flags            = KSDSOUND_BUFFER_LOCSOFTWARE;
            pfmtds->BufferDesc.Control         = KSDSOUND_BUFFER_CTRL_3D | KSDSOUND_BUFFER_CTRL_HRTF_3D;
            // allocate best found format
            pfmtds->BufferDesc.WaveFormatEx.wFormatTag         = WAVE_FORMAT_PCM;  
            pfmtds->BufferDesc.WaveFormatEx.nChannels            = (pDataRangeAudio->MaximumChannels < (ULONG)-1) ? (WORD)pDataRangeAudio->MaximumChannels : 2;
            pfmtds->BufferDesc.WaveFormatEx.nSamplesPerSec   =  min(pDataRangeAudio->MaximumSampleFrequency, 48000);
            pfmtds->BufferDesc.WaveFormatEx.wBitsPerSample    = (WORD)min(pDataRangeAudio->MaximumBitsPerSample, 16);
            pfmtds->BufferDesc.WaveFormatEx.nBlockAlign           = (WORD)(pfmtds->BufferDesc.WaveFormatEx.nChannels * pfmtds->BufferDesc.WaveFormatEx.wBitsPerSample / 8);
            pfmtds->BufferDesc.WaveFormatEx.nAvgBytesPerSec  = pfmtds->BufferDesc.WaveFormatEx.nBlockAlign * pfmtds->BufferDesc.WaveFormatEx.nSamplesPerSec;
            pfmtds->BufferDesc.WaveFormatEx.cbSize                  = 0;
            // follwing lines are added as in case of PropsedDataformat test, we want to memorize dataforamt of pin with which Pin is created.
            memcpy(&DsoundPinFormat, (PVOID) pfmtds, sizeof(KSDATAFORMAT_DSOUND));
            PinType = 2;

            DWORD dwerr = g_pGauditModule->GetCreatePinFn()(hFilter,
                                                                                        pconnect,
                                                                                        GENERIC_READ | GENERIC_WRITE,
                                                                                         phPin);

            if (dwerr)
            {
                goto cleanup;
            }
            
            fResult = TRUE;
        }
    
        else if ((prange->Specifier == KSDATAFORMAT_SPECIFIER_NONE) && (prange->FormatSize == sizeof (KSDATARANGE_MUSIC)))
        {
            pconnect = (PKSPIN_CONNECT)new BYTE[sizeof(KSPIN_CONNECT) + sizeof(KSDATARANGE_MUSIC)];
            if (!pconnect)
                goto cleanup;
    
            PKSDATARANGE_MUSIC pfmusic = PKSDATARANGE_MUSIC(pconnect + 1);
            CScratchHandle Handle (FILTER());
            HANDLE NecessarySink;
            if (PIN()->m_Descriptor.Communication & KSPIN_COMMUNICATION_SINK)
                NecessarySink =  NULL;
            else {
                // we will need hanlde to supply for the creation of the pin
                if (Handle.Open ()) {
                    SLOG(eError, " we are not able to get Handle for the temp file");
                    goto cleanup;
                }   
                NecessarySink = Handle.GetHandle();
            }
            FillPKsPinConnect1(pconnect, PIN()->m_nId,NecessarySink, (KSPIN_INTERFACE*)(PIN()->m_Descriptor.pmiInterfaces+1),(KSPIN_MEDIUM*)( PIN()->m_Descriptor.pmiMediums+1), NULL);

            
            pfmusic->DataRange.FormatSize  = sizeof(KSDATARANGE);
            pfmusic->DataRange.Flags       = 0;
            pfmusic->DataRange.SampleSize  = 0;
            pfmusic->DataRange.Reserved    = 0;
            pfmusic->DataRange.MajorFormat = KSDATAFORMAT_TYPE_MUSIC;
            pfmusic->DataRange.SubFormat   = KSDATAFORMAT_SUBTYPE_DIRECTMUSIC;
            pfmusic->DataRange.Specifier   = KSDATAFORMAT_SPECIFIER_NONE;
    
            pfmusic->Technology             = KSMUSIC_TECHNOLOGY_WAVETABLE;
            pfmusic->Channels               = 0;
            pfmusic->Notes                  = 0;
            pfmusic->ChannelMask            = 0xFFFF;
            // follwing lines are added as in case of PropsedDataformat test, we want to memorize dataforamt of pin with which Pin is created.
            memcpy(&MidiPinFormat, (PVOID) (&pfmusic->DataRange), sizeof(KSDATAFORMAT));
            PinType = 3;
            DWORD dwerr = g_pGauditModule->GetCreatePinFn()(hFilter,
                                                            pconnect,
                                                            GENERIC_READ | GENERIC_WRITE,
                                                             phPin);
    
            if (dwerr)
            {
                goto cleanup;
            }
    
            fResult = TRUE;
        }

cleanup:
    SAFE_DELETEA(pconnect)

    return fResult;
}

/* ____________________________________________________________________

    @HELPER IsPropertySupported
  ____________________________________________________________________ */
BOOL IsPropertySupported(HANDLE hFile, KSPROPERTY* pProperty, ULONG cbSize, ULONG Flags)
{
    DWORD dwOldFlags = pProperty->Flags;    //save old flags since we wanted to see that proeprty is supported on not p   
    pProperty->Flags = (dwOldFlags & KSPROPERTY_TYPE_TOPOLOGY);    // keep KSPROPERTY_TYPE_TOPOLOGY as we want to send KSPROPERTY_TYPE_BASICSUPPORT for Node / for filter
    pProperty->Flags |= KSPROPERTY_TYPE_BASICSUPPORT;

    DWORD dwSupport = 0;
    ULONG cboSize = sizeof(DWORD); 
    DWORD dwerr = ERROR_SUCCESS;
    ULONG cbWritten = 0;

    if (!DeviceIoControl(hFile,
                    IOCTL_KS_PROPERTY,
                    pProperty,
                    cbSize,
                    &dwSupport,
                    cboSize,
                    &cbWritten,
                    NULL
                    ))
    dwerr = GetLastError();

    pProperty->Flags = dwOldFlags;

    return (dwSupport & Flags); 
}

/* ____________________________________________________________________

    @HELPER IsPropertySetSupported
  ____________________________________________________________________ */
BOOL IsPropertySetSupported(HANDLE hFile, KSPROPERTY* pProperty, ULONG cbSize)
{
    DWORD KillFlags = (pProperty->Flags & ~(KSPROPERTY_TYPE_TOPOLOGY));
    pProperty->Flags &= KSPROPERTY_TYPE_TOPOLOGY;
    pProperty->Flags |= KSPROPERTY_TYPE_SETSUPPORT;

    DWORD dwSupport = 0;
    ULONG cboSize = sizeof(DWORD); 
    DWORD dwerr;
    ULONG cbWritten;

    DeviceIoControl(hFile,
                    IOCTL_KS_PROPERTY,
                    pProperty,
                    cbSize,
                    NULL,
                    0,
                    &cbWritten,
                    NULL
                    );
    dwerr = GetLastError();

    pProperty->Flags &= ~KSPROPERTY_TYPE_SETSUPPORT;
    pProperty->Flags |= KillFlags;

    return (dwerr == 0); 
}

/* ____________________________________________________________________

    @HELPER CopyGuid
   ____________________________________________________________________ */
void CopyGuid(GUID& dest, const GUID& src)
{
    memcpy(&dest, &src, sizeof(dest));
}

/* ____________________________________________________________________

    @HELPER CreateClock
   ____________________________________________________________________ */
BOOL CreateClock(HANDLE hFile, HANDLE* phClock)
{
    KSCLOCK_CREATE clockcreate = { 0 };
    BOOL fResult = FALSE;

    DWORD dwerr = g_pGauditModule->GetCreateClockFn()(hFile,
                                                                                     &clockcreate,
                                                                                     phClock);
    if (dwerr == S_OK)
        fResult = TRUE;

    return fResult;
}


/* ____________________________________________________________________

    @HELPER GetPinFormat
   ____________________________________________________________________ */

DWORD GetPinFormat(PGAUDITPARAMS pParams, PKSDATAFORMAT_WAVEFORMATEX pWfx, PKSDATAFORMAT_DSOUND pDS, BOOL& fIsWfx)
{
    // get current dataformat for the pin
    DWORD dwResult = FNS_FAIL; 
    KSPROPERTY Property = { 0 };
    
    Property.Set = KSPROPSETID_Connection;
    Property.Id  = KSPROPERTY_CONNECTION_DATAFORMAT;
    Property.Flags = KSPROPERTY_TYPE_GET;

    // we can only have dsound or waveformat in this version
/*    CGenericIoControl gnt(pParams->hFile, &Property, 
	                      sizeof(Property), sizeof(Property), 
		   		          sizeof(KSDATAFORMAT_WAVEFORMATEX), sizeof(KSDATAFORMAT_WAVEFORMATEX));
		   		          */
    // call with zero buffer and this will tell us how many bytes are need to have ksdataformat.
    
    CGenericIoControl gnt(pParams->hFile, &Property, 
                             sizeof(Property), sizeof(Property), 0, 0);

    gnt.Test(TRUE, (ULONG) -1);
    SLOG(eInfo3, "required bytes for getting dataformat is %d", gnt.GetWritten());
    DWORD dwDataLength = gnt.GetWritten();
    CGenericIoControl gnt1(pParams->hFile, &Property, 
                                 sizeof(Property), sizeof(Property), dwDataLength, dwDataLength);
    SLOG(eInfo3, "dwDataLength = %d", dwDataLength);

    //  it is waveformatex
    if (!gnt1.Test(-1, (ULONG) -1))
   {
        dwDataLength = gnt1.GetWritten();
        
        SLOG(eInfo3, "required bytes for getting dataformat is %d", gnt1.GetWritten());
        if (dwDataLength == sizeof(KSDATAFORMAT_WAVEFORMATEX))  {
            fIsWfx = TRUE;
            memcpy(pWfx, gnt1.GetData(), sizeof(KSDATAFORMAT_WAVEFORMATEX));
            dwResult = FNS_PASS;
        }
        else if (dwDataLength == sizeof(KSDATAFORMAT_DSOUND))    {
            fIsWfx = FALSE;
            memcpy(pDS, gnt1.GetData(), sizeof(KSDATAFORMAT_DSOUND));
            dwResult = FNS_PASS;
        }
        else 
            SLOG(eInfo3, "Pin %ld data format is different then KSDATAFORMAT_WAVEFORMATEX or  KSDATAFORMAT_DSOUND", PIN()->m_nId);

    }

    if (dwResult != FNS_PASS)
        SLOG(eInfo2, "Could not get current format for pin %ld", PIN()->m_nId);

    return dwResult;
}


/* ____________________________________________________________________

    @HELPER GetPinPrefferedFormat 

    ____________________________________________________________________ */

DWORD GetPinPrefferedFormat(HANDLE hFile, PKSDATAFORMAT_WAVEFORMATEX pFmt, ULONG cbOutSize)
{
        DWORD dwResult = FNS_FAIL;
        
        ULONG cbInSize = sizeof(KSP_PIN) + sizeof(KSMULTIPLE_ITEM) + sizeof(KSDATARANGE_AUDIO);
        PKSP_PIN pkspin = (PKSP_PIN) new BYTE[cbInSize];
        if (!pkspin)
	 {
    	     SLOG(eError, "ERROR: Internal memory allocation failure");
            return FNS_FAIL;
	 }
 
	memset(pkspin, 0, cbInSize);

       PKSMULTIPLE_ITEM                    pksitems = (PKSMULTIPLE_ITEM)(pkspin + 1);
       PKSDATARANGE_AUDIO             pksrange = (PKSDATARANGE_AUDIO)(pksitems + 1);

       memset(pkspin, 0, cbInSize);
       
       // fill property request
       pkspin->Property.Set = KSPROPSETID_Pin;
       pkspin->Property.Id = KSPROPERTY_PIN_DATAINTERSECTION;
       pkspin->Property.Flags = KSPROPERTY_TYPE_GET;
       pkspin->PinId = PIN()->m_nId;

        // setup the multiple items struct
        pksitems->Count = 1;
        pksitems->Size = sizeof(KSMULTIPLE_ITEM) + sizeof(KSDATARANGE_AUDIO);

        // setup the universal WFX range
        pksrange->DataRange.FormatSize = sizeof(KSDATARANGE_AUDIO); 
        pksrange->DataRange.MajorFormat = KSDATAFORMAT_TYPE_AUDIO ;
        pksrange->DataRange.SubFormat =  KSDATAFORMAT_SUBTYPE_PCM ;
        pksrange->DataRange.Specifier = KSDATAFORMAT_SPECIFIER_WAVEFORMATEX ;
        pksrange->MaximumChannels = 2;
        pksrange->MinimumBitsPerSample = 8;
        pksrange->MaximumBitsPerSample = 24;
        pksrange->MinimumSampleFrequency = 100;
        pksrange->MaximumSampleFrequency = 100000;

        CGenericIoControl gnt(hFile, pkspin, 
                                          cbInSize, cbInSize, 
                                          cbOutSize, cbOutSize);

        if (!gnt.Test(-1, (ULONG)-1))
        {
            // copy data back
            memcpy(pFmt, gnt.GetData(), cbOutSize);
            dwResult = FNS_PASS;
        }
        if (pkspin)
            delete[] pkspin;

        return dwResult;
}
/* ____________________________________________________________________

    @HELPER GetPinDataInterSection
// right now no one use this function
    ____________________________________________________________________ */
DWORD GetPinDataintersection(HANDLE hFile, PKSDATARANGE_AUDIO pAudioFmt)
{
        DWORD dwResult = FNS_FAIL;
        DWORD dwError = 0;
        ULONG cbOutSize = 0;
        ULONG cbInSize = sizeof(KSP_PIN) + sizeof(KSMULTIPLE_ITEM) + sizeof(KSDATARANGE_AUDIO);
        PKSP_PIN pkspin = (PKSP_PIN) new BYTE[cbInSize];
        if (!pkspin)
	 {
    	     SLOG(eError, "ERROR: Internal memory allocation failure");
            return FNS_FAIL;
	 }
 
	memset(pkspin, 0, cbInSize);

       PKSMULTIPLE_ITEM                    pksitems = (PKSMULTIPLE_ITEM)(pkspin + 1);
       PKSDATARANGE_AUDIO             pksrange = (PKSDATARANGE_AUDIO)(pksitems + 1);

       memset(pkspin, 0, cbInSize);
       memcpy(pksrange, pAudioFmt, sizeof (KSDATARANGE_AUDIO));
       // fill property request
       pkspin->Property.Set = KSPROPSETID_Pin;
       pkspin->Property.Id = KSPROPERTY_PIN_DATAINTERSECTION;
       pkspin->Property.Flags = KSPROPERTY_TYPE_GET;
       pkspin->PinId = PIN()->m_nId;

        // setup the multiple items struct
        pksitems->Count = 1;
        pksitems->Size = sizeof(KSMULTIPLE_ITEM) + sizeof(KSDATARANGE_AUDIO);

        CGenericIoControl gnt(hFile, pkspin, 
                                          cbInSize, cbInSize, 
                                          cbOutSize, cbOutSize);
        gnt.Test(TRUE, (ULONG)-1);
        cbOutSize = gnt.GetWritten();
        SLOG(eInfo2, "Now sending correct buffer of size %d for result", cbOutSize);
        CGenericIoControl gnt1(hFile, pkspin, 
                                          cbInSize, cbInSize, 
                                          cbOutSize, cbOutSize);
        dwError = gnt1.Test(-1, ULONG (-1));
        if ((dwError == ERROR_NO_MATCH) || (gnt1.GetFault() != FAULT_NO_FAULT) )
            dwResult =  FNS_FAIL;
        else     
            
            dwResult = FNS_PASS;
        if (dwResult == FNS_PASS)   // Here we want to copy data back as we are passing only one format and if property successed that mean pin supoorts Data Format
        
        if (pkspin)
            delete[] pkspin;

        return dwResult;
}
// Fill PKSPIN_CONNECT as per the MEDIUM  / INTERFACE and PRIORITY supplied

void FillPKsPinConnect1(PKSPIN_CONNECT pconnect, ULONG PinId, HANDLE SinkHandle, KSPIN_INTERFACE *Interface, KSPIN_MEDIUM *Medium , KSPRIORITY *Priority)
{
    if (!Interface) {
        pconnect -> Interface.Set = KSINTERFACESETID_Standard;
        pconnect -> Interface.Id = KSINTERFACE_STANDARD_STREAMING;
        pconnect -> Interface.Flags = 0;
    } else {
        pconnect -> Interface = *Interface;
    }
    
    if (!Medium) {
        pconnect -> Medium.Set = KSMEDIUMSETID_Standard;
        pconnect -> Medium.Id = KSMEDIUM_TYPE_ANYINSTANCE;
        pconnect -> Medium.Flags = 0;
    } else {
        pconnect -> Medium = *Medium;
    }
    pconnect->PinId                           = PinId;
    pconnect->PinToHandle                 = SinkHandle;

    if (!Priority) {
        pconnect -> Priority.PriorityClass = KSPRIORITY_NORMAL;
        pconnect -> Priority.PrioritySubClass = KSPRIORITY_NORMAL;
    } else {
        pconnect -> Priority = *Priority;
    }
    
}


/* ____________________________________________________________________

    @HELPER FillStandardConnect 

    ____________________________________________________________________ */


__inline void FillStandardConnect(PKSPIN_CONNECT pconnect, ULONG PinId, HANDLE hPin )
{
    pconnect->Interface.Set               = KSINTERFACESETID_Standard;
    pconnect->Interface.Id                 = KSINTERFACE_STANDARD_STREAMING;
    pconnect->Interface.Flags             = 0;
    pconnect->Medium.Set                 = KSMEDIUMSETID_Standard;
    pconnect->Medium.Id                   = KSMEDIUM_TYPE_ANYINSTANCE;
    pconnect->Medium.Flags               = 0;
    pconnect->PinId                           = PinId;
    pconnect->PinToHandle                 = hPin;
    pconnect->Priority.PriorityClass      = KSPRIORITY_NORMAL;
    pconnect->Priority.PrioritySubClass  = 1;
}


