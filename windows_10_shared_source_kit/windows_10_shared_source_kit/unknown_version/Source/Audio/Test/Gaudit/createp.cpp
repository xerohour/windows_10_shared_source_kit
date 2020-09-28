//-------------------------------------------------------------------------
//
//  Microsoft Windows
//
// Copyright(C) Microsoft.All rights reserved.
//
//  File:       createp.cpp
//
//
//--------------------------------------------------------------------------

#include "common.h"
#define FILE_QUAD_ALIGNMENT 7
void FillPKsPinConnect(PKSPIN_CONNECT pconnect, ULONG PinId, HANDLE SinkHandle, KSPIN_INTERFACE *Interface, KSPIN_MEDIUM *Medium , KSPRIORITY *Priority)
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

    @TEST Creating pins
    ____________________________________________________________________ */


DWORD WINAPI  Generic_Create_Pin_1(PGAUDITPARAMS pParams)
// create pin with invalid size of KSDATAFORMAT_WAVEFORMATEX
{
    DWORD dwResult = FNS_FAIL;
    HANDLE hPin = NULL;
    DWORD dwError;

    REFER_FORMAL_PARAM(pParams);
    // We have a pin but the pin is not instantiated
    _ASSERT(PIN());
    _ASSERT(PIN()->m_handle == INVALID_HANDLE);


    KSDATAFORMAT_WAVEFORMATEX dfwfx = { 0 };

//    if (FNS_FAIL == GetPinPrefferedFormat(FILTER()->m_handle, &dfwfx.DataFormat, sizeof(dfwfx)))
    if (FNS_FAIL == GetPinPrefferedFormat(FILTER()->m_handle, &dfwfx, sizeof(dfwfx)))
        return FNS_PASS;
// we have to first make sure about wFormatTag. If the wFormatTag  is equalt to WAVE_FORMAT_PCM, then cbsize will be ignored and pin creation will success instead of failure and this is valid.
    if (dfwfx.WaveFormatEx.wFormatTag == WAVE_FORMAT_PCM)
        return FNS_PASS;
    // change the cbSize member
    dfwfx.WaveFormatEx.cbSize = 0x7000;


    // for creation of Source pin we will need to supply handle where as in case of sink pin it will be ingnored by KS
    CScratchHandle Handle (FILTER());

    PKSPIN_CONNECT pconnect = NULL;
    pconnect = (PKSPIN_CONNECT)new BYTE[sizeof(KSPIN_CONNECT) + sizeof(KSDATAFORMAT_WAVEFORMATEX)];
    if (!pconnect)
            goto cleanup;

    HANDLE NecessarySink;
    if (PIN()->m_Descriptor.Communication & KSPIN_COMMUNICATION_SINK)
        NecessarySink =  NULL;
    else {
        // we will need handle to supply for the creation of the pin
        if (Handle.Open ()) {
            SLOG(eError, " we are not able to get Handle for the temp file");
            goto cleanup;
        }   
        NecessarySink = Handle.GetHandle();
    }
    
     // fill connect structure with default params
     FillPKsPinConnect(pconnect, PIN()->m_nId,NecessarySink, (KSPIN_INTERFACE*)(PIN()->m_Descriptor.pmiInterfaces+1),(KSPIN_MEDIUM*)( PIN()->m_Descriptor.pmiMediums+1), NULL);


    // copy preffered format
    memcpy(pconnect + 1, &dfwfx, sizeof(dfwfx));

    dwError = g_pGauditModule->GetCreatePinFn()(FILTER()->m_handle,
                    pconnect,
                    PIN()-> m_Descriptor.DataFlow == KSPIN_DATAFLOW_OUT ? GENERIC_READ : GENERIC_WRITE,
                    &hPin);

    if (dwError != ERROR_SUCCESS)
        dwResult = FNS_PASS;
    else 
        SLOG(eError, "FAIL:CreatePin with invalid cbSize SUCCEEDED");

cleanup:
    SAFE_DELETEA(pconnect)
    SAFE_CLOSE_HANDLE(hPin)

       
    return dwResult;
}


DWORD WINAPI  Generic_Create_Pin_2(PGAUDITPARAMS pParams)
// creating pin with zero cb size for KSDATAFORMAT_WAVEFORMATEX    
{
    DWORD dwResult = FNS_FAIL;
    HANDLE hPin = NULL;

    REFER_FORMAL_PARAM(pParams);    
    // We have a pin but the pin is not instantiated
    _ASSERT(PIN());
    _ASSERT(PIN()->m_handle == INVALID_HANDLE);


    KSDATAFORMAT_WAVEFORMATEX dfwfx = { 0 };

    //    if (FNS_FAIL == GetPinPrefferedFormat(FILTER()->m_handle, &dfwfx.DataFormat, sizeof(dfwfx)))
           if (FNS_FAIL == GetPinPrefferedFormat(FILTER()->m_handle, &dfwfx, sizeof(dfwfx)))
        return FNS_PASS;


   //change size to zero
   if (dfwfx.WaveFormatEx.cbSize)
        dfwfx.WaveFormatEx.cbSize = 0x0;
   else
        return FNS_PASS;

    CScratchHandle Handle (FILTER());
    PKSPIN_CONNECT pconnect = NULL;

    pconnect = (PKSPIN_CONNECT)new BYTE[sizeof(KSPIN_CONNECT) + sizeof(KSDATAFORMAT_WAVEFORMATEX)];
    if (!pconnect)
            goto cleanup;

     HANDLE NecessarySink;
     if (PIN()->m_Descriptor.Communication & KSPIN_COMMUNICATION_SINK)
         NecessarySink =  NULL;
     else {
         // we will need handle to supply for the creation of the pin
         if (Handle.Open ()) {
             SLOG(eError, " we are not able to get Handle for the temp file");
             goto cleanup;
         }   
         NecessarySink = Handle.GetHandle();
     }
      // fill connect structure with default params
      FillPKsPinConnect(pconnect, PIN()->m_nId,NecessarySink, (KSPIN_INTERFACE*)(PIN()->m_Descriptor.pmiInterfaces+1),(KSPIN_MEDIUM*)( PIN()->m_Descriptor.pmiMediums+1), NULL);


    // copy preffered format
    memcpy(pconnect + 1, &dfwfx, sizeof(dfwfx));

    if (g_pGauditModule->GetCreatePinFn()(FILTER()->m_handle,
                    pconnect,
                    GENERIC_READ | GENERIC_WRITE,
                    &hPin))
        dwResult = FNS_PASS;
    else 
        SLOG(eError, "CreatePin with cbSize == 0 SUCCEEDED");

cleanup:
    SAFE_DELETEA(pconnect)
    SAFE_CLOSE_HANDLE(hPin)

       
    return dwResult;
}

DWORD WINAPI  Generic_Create_Pin_3(PGAUDITPARAMS pParams)
{
    DWORD dwResult = FNS_FAIL;
    HANDLE hPin = NULL;
    
    // We have a pin but the pin is not instantiated
    _ASSERT(PIN());
    _ASSERT(PIN()->m_handle == INVALID_HANDLE);


    KSDATAFORMAT_WAVEFORMATEX dfwfx = { 0 };

    //    if (FNS_FAIL == GetPinPrefferedFormat(FILTER()->m_handle, &dfwfx.DataFormat, sizeof(dfwfx)))
           if (FNS_FAIL == GetPinPrefferedFormat(FILTER()->m_handle, &dfwfx, sizeof(dfwfx)))
        return FNS_PASS;


    // change the tag
    if (dfwfx.WaveFormatEx.wFormatTag != WAVE_FORMAT_EXTENSIBLE)
        dfwfx.WaveFormatEx.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
    else
        return FNS_PASS;
    
    CScratchHandle Handle (FILTER());
    PKSPIN_CONNECT pconnect = NULL;
    pconnect = (PKSPIN_CONNECT)new BYTE[sizeof(KSPIN_CONNECT) + sizeof(KSDATAFORMAT_WAVEFORMATEX)];
    if (!pconnect)
            goto cleanup;
    
    HANDLE NecessarySink;
    if (PIN()->m_Descriptor.Communication & KSPIN_COMMUNICATION_SINK)
        NecessarySink =  NULL;
    else {
        // we will need handle to supply for the creation of the pin
        if (Handle.Open ()) {
            SLOG(eError, " we are not able to get Handle for the temp file");
            goto cleanup;
        }   
        NecessarySink = Handle.GetHandle();
    }
     // fill connect structure with default params
     FillPKsPinConnect(pconnect, PIN()->m_nId,NecessarySink, (KSPIN_INTERFACE*)(PIN()->m_Descriptor.pmiInterfaces+1),(KSPIN_MEDIUM*)( PIN()->m_Descriptor.pmiMediums+1), NULL);


    // copy preffered format
    memcpy(pconnect + 1, &dfwfx, sizeof(dfwfx));

    if (g_pGauditModule->GetCreatePinFn()(pParams->hFile,
                    pconnect,
                    GENERIC_READ | GENERIC_WRITE,
                    &hPin))
        dwResult = FNS_PASS;
    else 
        SLOG(eError, "CreatePin with  changed FormatTag SUCCEEDED");


cleanup:
    SAFE_DELETEA(pconnect)
    SAFE_CLOSE_HANDLE(hPin)

       
    return dwResult;
}

DWORD WINAPI  Generic_Create_Pin_4(PGAUDITPARAMS pParams)
{
    DWORD dwResult = FNS_FAIL;
    HANDLE hPin = NULL;

    REFER_FORMAL_PARAM(pParams);
    // We have a pin but the pin is not instantiated
    _ASSERT(PIN());
    _ASSERT(PIN()->m_handle == INVALID_HANDLE);


    KSDATAFORMAT_WAVEFORMATEX dfwfx = { 0 };

    //    if (FNS_FAIL == GetPinPrefferedFormat(FILTER()->m_handle, &dfwfx.DataFormat, sizeof(dfwfx)))
           if (FNS_FAIL == GetPinPrefferedFormat(FILTER()->m_handle, &dfwfx, sizeof(dfwfx)))
        return FNS_PASS;

    if (dfwfx.DataFormat.FormatSize == sizeof(KSDATAFORMAT))
        return FNS_PASS;

    CScratchHandle Handle (FILTER());

    // allocate just  KSDATAFORMAT
    PKSPIN_CONNECT pconnect = NULL;
    pconnect = (PKSPIN_CONNECT)new BYTE[sizeof(KSPIN_CONNECT) + sizeof(KSDATAFORMAT)];
    if (!pconnect)
            goto cleanup;

     HANDLE NecessarySink;
     if (PIN()->m_Descriptor.Communication & KSPIN_COMMUNICATION_SINK)
         NecessarySink =  NULL;
     else {
         // we will need handle to supply for the creation of the pin
         if (Handle.Open ()) {
             SLOG(eError, " we are not able to get Handle for the temp file");
             goto cleanup;
         }   
         NecessarySink = Handle.GetHandle();
     }
      // fill connect structure with default params
      FillPKsPinConnect(pconnect, PIN()->m_nId,NecessarySink, (KSPIN_INTERFACE*)(PIN()->m_Descriptor.pmiInterfaces+1),(KSPIN_MEDIUM*)( PIN()->m_Descriptor.pmiMediums+1), NULL);

    // change format size but keep specifier
    dfwfx.DataFormat.FormatSize = sizeof(KSDATAFORMAT);

    // copy preffered format
    memcpy(pconnect + 1, &dfwfx, sizeof(KSDATAFORMAT));

    if (g_pGauditModule->GetCreatePinFn()(FILTER()->m_handle,
                    pconnect,
                    GENERIC_READ | GENERIC_WRITE,
                    &hPin))
        dwResult = FNS_PASS;
    else
        SLOG(eError, "CreatePin with Specifier Information Missing SUCCEEDED");

cleanup:
    SAFE_DELETEA(pconnect)
    SAFE_CLOSE_HANDLE(hPin)

        
    return dwResult;
}

DWORD WINAPI  Generic_Create_Pin_Invalid(PGAUDITPARAMS pParams, ULONG TestCase)
// create pin with invalid KSPRIORITY
{
    DWORD dwResult = FNS_FAIL;
    HANDLE hPin = NULL;
    DWORD dwError;

    REFER_FORMAL_PARAM(pParams);
    // We have a pin but the pin is not instantiated
    _ASSERT(PIN());
    _ASSERT(PIN()->m_handle == INVALID_HANDLE);


    KSDATAFORMAT_WAVEFORMATEX dfwfx = { 0 };

//    if (FNS_FAIL == GetPinPrefferedFormat(FILTER()->m_handle, &dfwfx.DataFormat, sizeof(dfwfx)))
    if (FNS_FAIL == GetPinPrefferedFormat(FILTER()->m_handle, &dfwfx, sizeof(dfwfx)))
        return FNS_PASS;
// we have to first make sure about wFormatTag. If the wFormatTag  is equalt to WAVE_FORMAT_PCM, then cbsize will be ignored and pin creation will success instead of failure and this is valid.
    if (dfwfx.WaveFormatEx.wFormatTag == WAVE_FORMAT_PCM)
        return FNS_PASS;

    // for creation of Source pin we will need to supply handle where as in case of sink pin it will be ingnored by KS
    CScratchHandle Handle (FILTER());

    PKSPIN_CONNECT pconnect = NULL;
    pconnect = (PKSPIN_CONNECT)new BYTE[sizeof(KSPIN_CONNECT) + sizeof(KSDATAFORMAT_WAVEFORMATEX)];
    if (!pconnect)
            goto cleanup;

    HANDLE NecessarySink;
    if (PIN()->m_Descriptor.Communication & KSPIN_COMMUNICATION_SINK)
        NecessarySink =  NULL;
    else {
        // we will need handle to supply for the creation of the pin
        if (Handle.Open ()) {
            SLOG(eError, " we are not able to get Handle for the temp file");
            goto cleanup;
        }   
        NecessarySink = Handle.GetHandle();
    }
    
     // fill connect structure with default params
    FillPKsPinConnect(pconnect, PIN()->m_nId,NecessarySink, (KSPIN_INTERFACE*)(PIN()->m_Descriptor.pmiInterfaces+1),(KSPIN_MEDIUM*)( PIN()->m_Descriptor.pmiMediums+1), NULL);
    if (TestCase == 1)  // test for invalid priorities
        pconnect->Priority.PriorityClass = ~(KSPRIORITY_LOW | KSPRIORITY_NORMAL | KSPRIORITY_HIGH);        //setting invalid priority. it should be rejected
    if (TestCase == 2)  // test for invalid pin medium         
        pconnect->Medium.Set  = KSPIN_INVALID_MEDIUM;        //setting invalid KsPin_Medium.Set. it should be rejected
    if (TestCase == 3)  // test for invalid pin medium         
        pconnect->Interface.Set  = KSPIN_INVALID_MEDIUM;        //setting invalid KsPin_Medium.Set. it should be rejected
        
    // copy preferred format
    memcpy(pconnect + 1, &dfwfx, sizeof(dfwfx));

    dwError = g_pGauditModule->GetCreatePinFn()(FILTER()->m_handle,
                    pconnect,
                    PIN()-> m_Descriptor.DataFlow == KSPIN_DATAFLOW_OUT ? GENERIC_READ : GENERIC_WRITE,
                    &hPin);

    if (dwError != ERROR_SUCCESS)
        dwResult = FNS_PASS;
    else 
        SLOG(eError, "FAIL:CreatePin with invalid cbSize SUCCEEDED");

cleanup:
    SAFE_DELETEA(pconnect)
    SAFE_CLOSE_HANDLE(hPin)

       
    return dwResult;
}


DWORD WINAPI  Generic_Create_Pin_5(PGAUDITPARAMS pParams)
// create pin with invalid KSPRIORITY
{
    return Generic_Create_Pin_Invalid(pParams, 1);
}

DWORD WINAPI  Generic_Create_Pin_6(PGAUDITPARAMS pParams)
// create pin with invalid KSPIN_MEDIUM
{
    return Generic_Create_Pin_Invalid(pParams, 2);
}

DWORD WINAPI  Generic_Create_Pin_7(PGAUDITPARAMS pParams)
// create pin with invalid KSPIN_INTERFACE
{
    return Generic_Create_Pin_Invalid(pParams, 3);
}


//=============================================================
// used to fill data format  - KSDATAFORMAT_WAVEFORMATEX
VOID FillKSDATAFORMAT_WAVEFORMATEX ( PKSDATAFORMAT_WAVEFORMATEX pFormat, DataRangeInfo* pDataFormatInfo) 
{
    pFormat->DataFormat.FormatSize = sizeof(KSDATAFORMAT_WAVEFORMATEX);
    pFormat->DataFormat.Flags = 0;
    pFormat->DataFormat.Reserved = 0;
    pFormat->DataFormat.SampleSize = 0;
    pFormat->DataFormat.MajorFormat = KSDATAFORMAT_TYPE_AUDIO;
    pFormat->DataFormat.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
    pFormat->DataFormat.Specifier = KSDATAFORMAT_SPECIFIER_WAVEFORMATEX;

    pFormat->WaveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
    pFormat->WaveFormatEx.nChannels = (WORD) pDataFormatInfo->Channels;
    pFormat->WaveFormatEx.nSamplesPerSec = (DWORD) pDataFormatInfo->SampleFrequency;
    pFormat->WaveFormatEx.nAvgBytesPerSec = (DWORD) ((pDataFormatInfo->SampleFrequency)* (pDataFormatInfo->Channels)*(pDataFormatInfo->BitsPerSample))/8;
    pFormat->WaveFormatEx.wBitsPerSample = (WORD) pDataFormatInfo->BitsPerSample;
    pFormat->WaveFormatEx.nBlockAlign = (WORD)(pDataFormatInfo->Channels * pDataFormatInfo->BitsPerSample/8);
    pFormat->WaveFormatEx.cbSize = 0;
}
//===================================================================
// used to fill data format  - KSDATAFORMAT_DSOUND
VOID FillKSDATAFORMAT_DSOUND ( PKSDATAFORMAT_DSOUND pFormat, DataRangeInfo* pDataFormatInfo) 
{
    pFormat->DataFormat.FormatSize = sizeof(KSDATAFORMAT_DSOUND);
    pFormat->DataFormat.Flags = 0;
    pFormat->DataFormat.Reserved = 0;
    pFormat->DataFormat.SampleSize = 0;
    pFormat->DataFormat.MajorFormat = KSDATAFORMAT_TYPE_AUDIO;
    pFormat->DataFormat.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
    pFormat->DataFormat.Specifier = KSDATAFORMAT_SPECIFIER_DSOUND;

    pFormat->BufferDesc.WaveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
    pFormat->BufferDesc.WaveFormatEx.nChannels = (WORD) pDataFormatInfo->Channels;
    pFormat->BufferDesc.WaveFormatEx.nSamplesPerSec = (DWORD) pDataFormatInfo->SampleFrequency;
    pFormat->BufferDesc.WaveFormatEx.nAvgBytesPerSec = (DWORD) ((pDataFormatInfo->SampleFrequency)* (pDataFormatInfo->Channels)*(pDataFormatInfo->BitsPerSample))/8;
    pFormat->BufferDesc.WaveFormatEx.wBitsPerSample = (WORD) pDataFormatInfo->BitsPerSample;
    pFormat->BufferDesc.WaveFormatEx.nBlockAlign = (WORD)(pDataFormatInfo->Channels * pDataFormatInfo->BitsPerSample/8);
    pFormat->BufferDesc.WaveFormatEx.cbSize = 0;
    // TODO: needs to find out form the developer that follwing values are correct default value for the testing
    pFormat->BufferDesc.Flags = KSDSOUND_BUFFER_PRIMARY;
    pFormat->BufferDesc.Control = KSDSOUND_BUFFER_CTRL_FREQUENCY;
}
//=========================================================
// NOTE - This function can return true even though pin is not created. So before using phNewPin, check its contect first. If it is not NULL then only use it.
//=========================================================
BOOL CreateAudioPinInstance(DWORD dwFormat, PKSDATARANGE_AUDIO pKsDataRangeAudio, HANDLE* phNewPin)
{
    // check first that pin support this format. if so then go and create pin with this format
    BOOL fResult = TRUE;
    DWORD dwResult = FNS_FAIL;
    DWORD dwError = 0;
    BOOL fRange = FALSE;
    DataRangeInfo DataFormatInfo;
    KSDATARANGE_AUDIO DataRangeAudio;
    fRange = CheckFormatForRange(dwFormat, pKsDataRangeAudio, &DataFormatInfo);
    if (!fRange) {
        return fResult;
    }
    
    memcpy (&DataRangeAudio,pKsDataRangeAudio,sizeof(KSDATARANGE_AUDIO));
    DataRangeAudio.MaximumChannels = DataFormatInfo.Channels;
    DataRangeAudio.MaximumBitsPerSample = DataFormatInfo.BitsPerSample;
    DataRangeAudio.MaximumSampleFrequency = DataFormatInfo.SampleFrequency;
    DataRangeAudio.MinimumBitsPerSample = DataFormatInfo.BitsPerSample;
    DataRangeAudio.MinimumSampleFrequency = DataFormatInfo.SampleFrequency;
    SLOG (eInfo2, "Datarange should have support the format - Frequency = %d , Channels = %d , Bits per sample = %d. However checking the same with KSPROPERTY_PIN_DATAINTERSECTION  ", DataFormatInfo.SampleFrequency, DataFormatInfo.Channels, DataFormatInfo.BitsPerSample);
    // Now checking with KSPROPERTY_PIN_DATAINTERSECTION
    
    ULONG cbOutSize = 0;
    ULONG cbInSize = sizeof(KSP_PIN) + sizeof(KSMULTIPLE_ITEM) + sizeof(KSDATARANGE_AUDIO);
    PKSP_PIN pkspin = (PKSP_PIN) new BYTE[cbInSize];
    if (!pkspin)    {
        SLOG(eError, "ERROR: Internal memory allocation failure");
        return FALSE;
    }
 
    memset(pkspin, 0, cbInSize);

    PKSMULTIPLE_ITEM                    pksitems = (PKSMULTIPLE_ITEM)(pkspin + 1);
    PKSDATARANGE_AUDIO             pksrange = (PKSDATARANGE_AUDIO)(pksitems + 1);

    memset(pkspin, 0, cbInSize);
    memcpy(pksrange, &DataRangeAudio, sizeof (KSDATARANGE_AUDIO));
       // fill property request
    pkspin->Property.Set = KSPROPSETID_Pin;
    pkspin->Property.Id = KSPROPERTY_PIN_DATAINTERSECTION;
    pkspin->Property.Flags = KSPROPERTY_TYPE_GET;
    pkspin->PinId = PIN()->m_nId;

    // setup the multiple items struct
    pksitems->Count = 1;
    pksitems->Size = sizeof(KSMULTIPLE_ITEM) + sizeof(KSDATARANGE_AUDIO);

    CGenericIoControl gnt(FILTER()->m_handle, pkspin, 
                                      cbInSize, cbInSize, 
                                      cbOutSize, cbOutSize);
    gnt.Test(TRUE, (ULONG)-1);
    cbOutSize = gnt.GetWritten();
    BOOL fSizeError = FALSE;
    fSizeError = (cbOutSize == 0) || (cbOutSize < sizeof(KSDATAFORMAT));
    if (fSizeError) {
        SLOG (eInfo2, "We did not get the correct size from the property request -KSPROPERTY_PIN_DATAINTESTSECTION ");
        fResult = TRUE;
        if (pkspin)
            delete[] pkspin;
        return fResult;
    }
    SLOG(eInfo2, "Now sending correct buffer of size %d for result", cbOutSize);
    CGenericIoControl gnt1(FILTER()->m_handle, pkspin, 
                                      cbInSize, cbInSize, 
                                      cbOutSize, cbOutSize);
    dwError = gnt1.Test(-1, ULONG (-1));
    if ((dwError == ERROR_NO_MATCH) || (gnt1.GetFault() != FAULT_NO_FAULT) )
        dwResult =  FNS_FAIL;
    else     
        dwResult = FNS_PASS;
    if (dwResult)   {
        SLOG (eInfo2, "Pin does not support format with - Frequency = %d , Channels = %d , Bits per sample = %d. So pin Creation will not be possible. ", DataFormatInfo.SampleFrequency, DataFormatInfo.Channels, DataFormatInfo.BitsPerSample);           
        fResult = FALSE;
        if (dwError == ERROR_NO_MATCH)          // that means that we could not find the intersection for the above format for the pin
            fResult = TRUE;
        if (pkspin)
            delete[] pkspin;
        return fResult;
    }
    
    SLOG (eInfo2, "Pin must support format as we have verified the same by sending KSPROPERTY_PIN_DATARANGEINTERSECTION with - Frequency = %d , Channels = %d , Bits per sample = %d  ", DataFormatInfo.SampleFrequency, DataFormatInfo.Channels, DataFormatInfo.BitsPerSample);                      
    PKSPIN_CONNECT pconnect = NULL;
    PKSDATAFORMAT_WAVEFORMATEX pfmtwfx = NULL;
    CScratchHandle Handle (FILTER());
    
    pconnect = (PKSPIN_CONNECT)new BYTE[sizeof(KSPIN_CONNECT) + cbOutSize]; //sizeof(KSDATAFORMAT_WAVEFORMATEX)];
    if (!pconnect)
        goto cleanup;
    // copy format retruned by the pin during the Property_pin_DataIntersection
    memcpy ((PVOID)(pconnect + 1), gnt1.GetData(), cbOutSize);

    // here compare the result received back from the pin with original request.
    BOOL fBadFormat = FALSE;
    if (cbOutSize >= sizeof (KSDATAFORMAT_WAVEFORMATEX))    {
        pfmtwfx = PKSDATAFORMAT_WAVEFORMATEX(pconnect + 1);  
        if (DataRangeAudio.MaximumChannels <pfmtwfx->WaveFormatEx.nChannels)    {
            SLOG (eError, "FAIL: Driver has returned invalid format back (different no of channels). There is a bug in the driver Pin Dataintersection handler");
            fBadFormat = TRUE;
        }
        if ((DataRangeAudio.MaximumBitsPerSample<pfmtwfx->WaveFormatEx.wBitsPerSample) || (pfmtwfx->WaveFormatEx.wBitsPerSample< DataRangeAudio.MinimumBitsPerSample))    {
            SLOG (eError,"FAIL: Driver has returned invalid format back (different number of Bits per sample). There is a bug in the driver Pin Dataintersection handler");
            fBadFormat = TRUE;
        }
        if ((DataRangeAudio.MaximumSampleFrequency<pfmtwfx->WaveFormatEx.nSamplesPerSec) || (pfmtwfx->WaveFormatEx.nSamplesPerSec< DataRangeAudio.MinimumSampleFrequency))    {
            SLOG (eError,"FAIL: Driver has returned invalid format back (different sample rate). There is a bug in the driver Pin Dataintersection handler");
            fBadFormat = TRUE;
        }
    }
    if (fBadFormat) {
        // display data received back from the driver
        SLOG (eInfo2, "Data received back from the driver after KsProperty_pin_dataintersection request");
        SLOG (eInfo2, "Displaying KSDATAFORMAT_WAVEFORMATEX.Format");
        SLOG(eInfo2, "DataFormat.Formatsize = %d", pfmtwfx->DataFormat.FormatSize);
        SLOG(eInfo2, "DataFormat.Flags = 0x%x", pfmtwfx->DataFormat.Flags);
        SLOG(eInfo2, "DataFormat.SampleSize = 0x%x", pfmtwfx->DataFormat.SampleSize);
        SLOG(eInfo2, "DataFormat.Reserved = 0x%x", pfmtwfx->DataFormat.Reserved);
        SLOG(eInfo2, "DataFormat.MajorFormat = %x-%x-%x-%x%x-%x%x%x%x%x%x ", DisplayGUID(pfmtwfx->DataFormat.MajorFormat));
        SLOG(eInfo2, "DataFormat.SubFormat = %x-%x-%x-%x%x-%x%x%x%x%x%x ", DisplayGUID(pfmtwfx->DataFormat.SubFormat));
        SLOG(eInfo2, "DataFormat.Specifier = %x-%x-%x-%x%x-%x%x%x%x%x%x ", DisplayGUID(pfmtwfx->DataFormat.Specifier));

        SLOG (eInfo2, "Displaying KSDATAFORMAT_WAVEFORMATEX.WAVEFORMATEX");

        SLOG(eInfo2, "WaveFormatEx.wFormatTag = 0x%x", pfmtwfx->WaveFormatEx.wFormatTag);
        SLOG(eInfo2, "WaveFormatEx.nChannels = 0x%x", pfmtwfx->WaveFormatEx.nChannels);
        SLOG(eInfo2, "WaveFormatEx.nSamplesPerSec = 0x%x", pfmtwfx->WaveFormatEx.nSamplesPerSec);
        SLOG(eInfo2, "WaveFormatEx.nAvgBytesPerSec = 0x%x", pfmtwfx->WaveFormatEx.nAvgBytesPerSec);
        SLOG(eInfo2, "WaveFormatEx.nBlockAlign = 0x%x", pfmtwfx->WaveFormatEx.nBlockAlign);
        SLOG(eInfo2, "WaveFormatEx.wBitsPerSample= 0x%x", pfmtwfx->WaveFormatEx.wBitsPerSample);
        SLOG(eInfo2, "WaveFormatEx.cbSize = 0x%x", pfmtwfx->WaveFormatEx.cbSize);
        fResult = FALSE;
        goto cleanup;
    }
    
    HANDLE NecessarySink;

    if (PIN()->m_Descriptor.Communication & KSPIN_COMMUNICATION_SINK)
        NecessarySink =  NULL;
    else {
        // we will need handle to supply for the creation of the pin
        if (Handle.Open ()) {
            SLOG(eError, " we are not able to get Handle for the temp file");
            goto cleanup;
        }   
        NecessarySink = Handle.GetHandle();
    }

    FillPKsPinConnect(pconnect, PIN()->m_nId,NecessarySink, (KSPIN_INTERFACE*)(PIN()->m_Descriptor.pmiInterfaces+1),(KSPIN_MEDIUM*)( PIN()->m_Descriptor.pmiMediums+1), NULL);

//    FillKSDATAFORMAT_WAVEFORMATEX (pfmtwfx, pDataFormatInfo);
    dwError = g_pGauditModule->GetCreatePinFn()(FILTER()->m_handle,
                pconnect,
                GENERIC_READ | GENERIC_WRITE,
                phNewPin);

    if ((dwError) && (dwError != 0xaa)) //  which menas that requested resource is in use - ERROR_BUSY - so no need to fail test case for it
    {
        SLOG (eError, "Pin creation failed for PinId %d ERROR = 0x%x", PIN()->m_nId, dwError);
        LPVOID lpMsgBuf; // used for the displaying get lass error string
        FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                                NULL, dwError, 
                                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                (LPTSTR) &lpMsgBuf, 0, NULL );
        SLOG(eInfo2, "Error : %s", (LPTSTR)lpMsgBuf);
        LocalFree( (LPVOID)lpMsgBuf );
        phNewPin =  NULL;
        fResult =  FALSE;
        goto cleanup;
    }
    SLOG (eInfo1, "Pin creation successed for Pin id %d with format Frequency = %d , Channels = %d , Bits per sample = %d.  \n", PIN()-> m_nId, DataFormatInfo.SampleFrequency, DataFormatInfo.Channels, DataFormatInfo.BitsPerSample );    
	

cleanup:
    if (pkspin)
        delete[] pkspin;

    SAFE_DELETEA(pconnect)

    return fResult;
}

//==============================================
// pass the FormatFlag and it will fill channle, sample rate and other information into data range
void FormatFlagToFormatInfo(DWORD dwFormatFlag, DataRangeInfo* pDataFormatInfo)  {
    
    switch (dwFormatFlag)   {
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

    }      
}
    // to check that given format is within thge range or not
    // if format is within the range then funtion will return TRUE otherwise FALSE
    // right now we are using the format as defined into mmsystem.h

BOOL CheckFormatForRange(DWORD dwFormat, PKSDATARANGE_AUDIO pKsDataRangeAudio, DataRangeInfo* pDataFormatInfo) 
{
    BOOL fResult = FALSE;
    BOOL fChannel = FALSE;
    BOOL fFrequecy = FALSE;
    BOOL fnBitsperSample = FALSE;
    FormatFlagToFormatInfo(dwFormat, pDataFormatInfo);
    if ((pDataFormatInfo->BitsPerSample >= pKsDataRangeAudio->MinimumBitsPerSample) && (pDataFormatInfo->BitsPerSample <= pKsDataRangeAudio->MaximumBitsPerSample))
        fnBitsperSample = TRUE;
    if ((pDataFormatInfo->Channels <= pKsDataRangeAudio->MaximumChannels) )
        fChannel = TRUE;
    if ((pDataFormatInfo->SampleFrequency >= pKsDataRangeAudio->MinimumSampleFrequency) && (pDataFormatInfo->SampleFrequency <= pKsDataRangeAudio->MaximumSampleFrequency))
        fFrequecy = TRUE;
    // in case of azalia drivr we found that driver reports max channel equal to 2 but they are not supporting mono format. so best way to find out is to call for KSPROPERTY_PIN_DATAINTERSECTION
    
    fResult = (fFrequecy & fChannel & fnBitsperSample);
    return fResult;
}

/*****************************************************************************
 * Tst_KCP_AllPinsValid ()
 *****************************************************************************
 * Instantiates all instantiatable pins on the currently instantiated filters
 */
DWORD   Tst_KCP_AllPinsValid (PGAUDITPARAMS pParams)
{
    if(PIN()->m_Descriptor.Communication == KSPIN_COMMUNICATION_NONE)   {
        SLOG(eInfo1, "Pin %d does not support instantiation", PIN()->m_nId);
        return FNS_PASS;
    }

    REFER_FORMAL_PARAM(pParams);
    PKSMULTIPLE_ITEM pRanges = NULL;   
    BOOL fResult = FNS_PASS;
    CKsPin* pTestPin = PIN();

    // allocate an appropriate KSDATAFORMAT based on the first KSDATARANGE in MIDataRanges
    pRanges = pTestPin->m_Descriptor.pmiDataRanges;
    if (!pRanges)  {  // means we do not have any information about dataranges supported by the pin so we wil not run this test case.
        SLOG(eInfo1, "Pin %d  - we do not have any information about dataranges supported by the pin so we wil not run this test case", PIN()->m_nId);
        return fResult;        
    }

    // get the details of the dataranges form the pin descriptor
    PKSDATARANGE pDataRange = (PKSDATARANGE) (pTestPin->m_Descriptor.pmiDataRanges + 1);
    pDataRange = (PKSDATARANGE)( ( ULONG_PTR(pDataRange) + (ULONG_PTR)7) & ~(ULONG_PTR)7);
    PKSDATARANGE_AUDIO pKsDatarangeAudio;
    
    SLOG(eInfo1, "Test case for creating Pin having id %d  with possible format supported by it", PIN()->m_nId);
    // here we will try to create pin with only audio format
    // TODO: For analog Audio, we need to find out how can we create pin
    bool thisIsAttributeList = false;
    bool nextIsAttributeList = false;
    
    for(ULONG i=0; i < pRanges->Count; i++)  {
        thisIsAttributeList = nextIsAttributeList;
        nextIsAttributeList = false;
        
        // ignore attribute lists
        if(thisIsAttributeList)  {
            goto next_range;
        }
        
        if(pDataRange->Flags & KSDATARANGE_ATTRIBUTES)  {
            nextIsAttributeList = true;
        }
        
        // Chceking for the PCM Format first 
        if(IsEqualGUID(pDataRange->SubFormat, KSDATAFORMAT_SUBTYPE_PCM)) {
            if(IsEqualGUID(pDataRange->Specifier, KSDATAFORMAT_SPECIFIER_WAVEFORMATEX))  {
                if (pDataRange->FormatSize != sizeof (KSDATARANGE_AUDIO))   {
                    SLOG (eError, "Datarange reported by the driver is not correct one" );
                    SLOG (eInfo2,  "Here is the reange reported by the driver");
                    DisplayDataRange(pDataRange);            
                    fResult = FNS_FAIL;
                    break;
                }
                
                // range reported by the driver is of KSDATARANGE_AUDIO
                pKsDatarangeAudio = (PKSDATARANGE_AUDIO) pDataRange;
                // test for all the format supported by this range
                for (DWORD dwFormat = MSB_FORMAT; dwFormat; dwFormat /= 2) {
                        BOOL fCreatedPin = FALSE;
                        fCreatedPin = CreateAudioPinInstance(dwFormat, pKsDatarangeAudio, &pTestPin->m_handle);
                        if (!fCreatedPin )  {
                            //need to display the format for which pin creation failed
                            fResult = FNS_FAIL;
                        } // for fCreatePin
                        else    {
                            // TODO: we should have all other kind of testing here - like reading Pin_CInstance,etc
                            SAFE_CLOSE_HANDLE(pTestPin->m_handle)
                        }
                     // for the Range and dataformat match
                } // to create pin with all possbile format
            }
            else if(IsEqualGUID(pDataRange->Specifier, KSDATAFORMAT_SPECIFIER_DSOUND))   {
    //            pKsPinConnect = AllocPinConnectDSound( pTestPin->m_nId, NULL, 22050, 2, 16);
    //            LogEx(eSevInfoL, "      Using KSDATAFORMAT_DSOUND\n");
            }
        }

next_range:
        ULONG size = (thisIsAttributeList ? ((PKSMULTIPLE_ITEM)pDataRange)->Size : pDataRange->FormatSize);
        pDataRange = PKSDATARANGE(((LPBYTE)pDataRange) + size ) ;  //since formatsize may be different then size of KSDATARANGE or KSDATARNAGE_AUDIO
        pDataRange = PKSDATARANGE( (ULONG_PTR(pDataRange)  + (ULONG_PTR)7) & ~(ULONG_PTR)7);
    }
    
/*    if(IsEqualGUID(pDataRange->SubFormat, KSDATAFORMAT_SUBTYPE_MIDI))   {
        pKsPinConnect = AllocPinConnectMIDI(pTestPin->m_nId, NULL);
        LogEx(eSevInfoL, "      Using KSDATAFORMAT\n");
    }
*/
    return fResult;
}


DWORD WINAPI  Generic_Create_Pin_Mode(PGAUDITPARAMS pParams, GUID guidProcessingMode, ULONG TestCase)
{
    DWORD dwResult = FNS_FAIL;
    HANDLE hPin = NULL;

    // We have a pin but the pin is not instantiated
    _ASSERT(PIN());
    _ASSERT(PIN()->m_handle == INVALID_HANDLE);
    UINT AttributeOffset = 0;
    UINT AttributeSize = 0;
    KSDATAFORMAT_WAVEFORMATEX dfwfx = { 0 };
    BOOL bMode = FALSE;

    if (FNS_FAIL == GetPinPrefferedFormat(FILTER()->m_handle, &dfwfx, sizeof(dfwfx)))
        return FNS_PASS;

    if (guidProcessingMode != GUID_NULL)
    {
        AttributeOffset = (sizeof(dfwfx) + dfwfx.WaveFormatEx.cbSize + FILE_QUAD_ALIGNMENT) & ~FILE_QUAD_ALIGNMENT;
        AttributeSize = sizeof(KSMULTIPLE_ITEM) + sizeof(KSATTRIBUTE) + sizeof(GUID);
        dfwfx.DataFormat.Flags |= KSDATAFORMAT_ATTRIBUTES;
        bMode = TRUE;
    }

    CScratchHandle Handle (FILTER());
    PKSPIN_CONNECT pconnect = NULL;
    pconnect = (PKSPIN_CONNECT)new BYTE[sizeof(KSPIN_CONNECT) + (bMode ? AttributeOffset + AttributeSize: sizeof(dfwfx)) ];
    if (!pconnect)
        goto cleanup;

    HANDLE NecessarySink;
    if (PIN()->m_Descriptor.Communication & KSPIN_COMMUNICATION_SINK)
        NecessarySink =  NULL;
    else {
        // we will need handle to supply for the creation of the pin
        if (Handle.Open ()) {
            SLOG(eError, " we are not able to get Handle for the temp file");
            goto cleanup;
        }   
        NecessarySink = Handle.GetHandle();
    }

    // fill connect structure with default params
    FillPKsPinConnect(pconnect, PIN()->m_nId,NecessarySink, (KSPIN_INTERFACE*)(PIN()->m_Descriptor.pmiInterfaces+1),(KSPIN_MEDIUM*)( PIN()->m_Descriptor.pmiMediums+1), NULL);

    // copy preferred format
    memcpy(pconnect + 1, &dfwfx, sizeof(dfwfx));

    if (bMode)
    {
        PKSMULTIPLE_ITEM pKsMi = (PKSMULTIPLE_ITEM)((PBYTE)(pconnect+1) + AttributeOffset);
        pKsMi->Size = AttributeSize;
        pKsMi->Count = 1;

        PKSATTRIBUTE_AUDIOSIGNALPROCESSING_MODE pKsAttribute = (PKSATTRIBUTE_AUDIOSIGNALPROCESSING_MODE)(pKsMi + 1);
        pKsAttribute->AttributeHeader.Size = sizeof(KSATTRIBUTE) + sizeof(GUID);
        pKsAttribute->AttributeHeader.Flags = 0;
        pKsAttribute->AttributeHeader.Attribute = KSATTRIBUTEID_AUDIOSIGNALPROCESSING_MODE;
        pKsAttribute->SignalProcessingMode = guidProcessingMode;
    }

    if (TestCase == 1)
    {
        pconnect->PinId = ULONG_MAX;
    }

    if (g_pGauditModule->GetCreatePinFn()(pParams->hFile,
        pconnect,
        GENERIC_READ | GENERIC_WRITE,
        &hPin))
        dwResult = FNS_PASS;
    else 
        SLOG(eError, "CreatePin with  changed FormatTag SUCCEEDED");

cleanup:
    SAFE_DELETEA(pconnect)
    SAFE_CLOSE_HANDLE(hPin)
    return dwResult;
}

DWORD WINAPI Create_Mode_Pin_ULONG_PINID_DEFAULT(PGAUDITPARAMS pParams)
{
    return Generic_Create_Pin_Mode(pParams, AUDIO_SIGNALPROCESSINGMODE_DEFAULT, 1);
}

DWORD WINAPI Create_Mode_Pin_ULONG_PINID_RAW(PGAUDITPARAMS pParams)
{
    return Generic_Create_Pin_Mode(pParams, AUDIO_SIGNALPROCESSINGMODE_RAW, 1);
}

DWORD WINAPI Create_Mode_Pin_GUIDNULL(PGAUDITPARAMS pParams)
{
    return Generic_Create_Pin_Mode(pParams, GUID_NULL, 0);
}

DWORD WINAPI Create_Mode_Pin_GUID_RANDOM(PGAUDITPARAMS pParams)
{
    GUID Guid;
    IIDFromString(L"{7856912A-D5F4-4B7A-A4D9-8B3FC87265D0}", &Guid);
    return Generic_Create_Pin_Mode(pParams, Guid, 0);
}

