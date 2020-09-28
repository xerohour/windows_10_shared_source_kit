//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
// Copyright(C) Microsoft.All rights reserved.
//
//  File:       pins.cpp
//
//
//--------------------------------------------------------------------------

#include "common.h"
#define TESTFLG_RANGE_SPECIFIER_MISSING            0x00000001
#define TESTFLG_BAD_LARGE_RANGE_SIZE                 0x00000002
#define TESTFLG_FORMAT_SPECIFIER_MISSING          0x00000004
#define TESTFLG_BAD_RANGE_SIZE                              0x00000008
#define TESTFLG_SMALL_RANGE_SIZE                          0x00000010
#define TESTFLG_AUDIO_FORMAT_BAD                        0x00000020
#define TESTFLG_INVALID_MULTIPLE_COUNT              0x00000040
#define TESTFLG_INVALID_PIN_ID                               0x00000080
#define TESTFLG_INVALID_PIN_ID_ULONG_MAX          0x00000100
#define TESTFLG_SMALL_OUTPUT_BUFFER                   0x00000200
#define TESTFLG_NULL_OUTPUT_BUFFER                     0x00000400

// for this test, feed each s_rgksDataRangeInvalid[i] into the dataintersection handler.  This should ALWAYS fail
KSDATARANGE_AUDIO s_rgksDataRangeInvalid[] =
{
    {
        {
            sizeof( KSDATARANGE_AUDIO ),
            0,
            0,
            0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_MUSIC),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_WAVEFORMATEX),
        },
        2,
        8,
        16,
        22050,
        22050
    },
    {
        {
            sizeof( KSDATARANGE_AUDIO ),
            0,
            0,
            0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_STANDARD_MPEG1_VIDEO),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_WAVEFORMATEX),
        },
        2,
        8,
        16,
        22050,
        22050
    },
    {
        {
            sizeof( KSDATARANGE_AUDIO ),
            0,
            0,
            0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_DIALECT_MPEG1_VIDEO),
        },
        2,
        8,
        16,
        22050,
        22050
    },
    {
        {
            sizeof( KSDATARANGE_AUDIO ),
            0,
            0,
            0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_WAVEFORMATEX),
        },
        0,
        8,
        16,
        22050,
        22050
    },
    {
        {
            sizeof( KSDATARANGE_AUDIO ),
            0,
            0,
            0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_WAVEFORMATEX),
        },
        2,
        0,
        2,
        22050,
        22050
    },
    {
        {
            sizeof( KSDATARANGE_AUDIO ),
            0,
            0,
            0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_WAVEFORMATEX),
        },
        2,
        44,
        53,
        22050,
        22050
    },
    {
        {
            sizeof( KSDATARANGE_AUDIO ),
            0,
            0,
            0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_WAVEFORMATEX),
        },
        2,
        8,
        16,
        1000000,
        100
    },
    {
        {
            sizeof( KSDATARANGE_AUDIO ),
            0,
            0,
            0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_WAVEFORMATEX),
        },
        2,
        8,
        16,
        19123,
        19123
    }
};

/* ____________________________________________________________________

    @TEST KSPROPERTY_PIN_DATAINTERSECTION
   ____________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_PIN_DATAINTERSECTION_1(PGAUDITPARAMS pParams)
//sending small input buffer - it will get rejected
{
	if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		return FNS_PASS;


    // Rejects input buffer too small
    // Out size is just enough for this property
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, 
                                          pParams->cbSize, pParams->cbSize, 
                                          sizeof(KSDATAFORMAT), sizeof(KSDATAFORMAT));

        gnt.Test(TRUE, 0);
        if (gnt.GetFault() != FAULT_NO_FAULT) 
	    return FNS_FAIL;
    }

	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_PIN_DATAINTERSECTION_BADAUDIORANGE(PGAUDITPARAMS pParams, ULONG TestFlag)
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
        return FNS_PASS;
    if (!PIN())
        return FNS_PASS;

    DWORD dwResult = FNS_PASS;
    // Get data ranges first and then try with all data ranges
    PKSMULTIPLE_ITEM pRanges = NULL;
    BOOL fRes;
    fRes = FILTER()->GetPinPropertyMulti(PIN()->m_nId, KSPROPSETID_Pin, KSPROPERTY_PIN_DATARANGES, &pRanges);
    if (!fRes)  {
        SLOG(eError, "ERROR: Failed to get pin %ld dataranges\n", PIN()->m_nId);
        dwResult = FNS_FAIL;
        return dwResult;
    }
    PKSDATARANGE pDataRange = (PKSDATARANGE) (pRanges + 1);
    pDataRange = (PKSDATARANGE)( ( ULONG_PTR(pDataRange) + (ULONG_PTR)7) & ~(ULONG_PTR)7);
    ULONG cbGoodSize = 0;
    
    bool thisIsAttributeList = false;
    bool nextIsAttributeList = false;

    for(UINT i=0; i < pRanges->Count; i++)  {
        PKSP_PIN pkspin = NULL;
        thisIsAttributeList = nextIsAttributeList;
        nextIsAttributeList = false;
        
        if(thisIsAttributeList)  {
            goto next_range;
        }
        
        nextIsAttributeList = ((pDataRange->Flags & KSDATARANGE_ATTRIBUTES) == KSDATARANGE_ATTRIBUTES);
        
        // sometine driver will report datarange with sub format and Specifier as Wild card. In this case we can not test it for bad data like max channels, etc
        if (pDataRange->FormatSize == sizeof(KSDATARANGE))  {
            // TODO: right now skip test for it - needs to be verified with developer for the same.
            goto next_range;
        }
    
        if ((pDataRange->MajorFormat == KSDATAFORMAT_TYPE_AUDIO) ||(pDataRange->MajorFormat == KSDATAFORMAT_TYPE_MUSIC))  {       // if the format is realted to AUDIO then only create pin
            cbGoodSize = sizeof(KSP_PIN) + sizeof(KSMULTIPLE_ITEM) + pDataRange->FormatSize; // since we can have differenct size then size of KSDATARANGE
            pkspin = (PKSP_PIN) new BYTE [cbGoodSize];
            if (!pkspin)    {
                SLOG(eError, "ERROR: Internal memory allocation failure");
                dwResult = FNS_FAIL;
                if (pRanges)
                    LocalFree(pRanges);
                return dwResult;
            }
            memset(pkspin,  0,  cbGoodSize);
            PKSMULTIPLE_ITEM   pksitems = (PKSMULTIPLE_ITEM)(pkspin + 1);
            PKSDATARANGE       pksrange = (PKSDATARANGE)(pksitems + 1);
            memcpy(pkspin, &pParams->Prop,  pParams->cbSize);
            pkspin->PinId = PIN()->m_nId;
            pkspin->Reserved = 0;
            pksitems->Count = 1;
            pksitems->Size  = sizeof(KSMULTIPLE_ITEM) + pDataRange->FormatSize;
            memcpy(pksrange, pDataRange,pDataRange->FormatSize);
            pksrange->Flags = 0;
            
            // first get the required buffersize by passing buffer size equal to zero
            ULONG OutputBufferSize = 0;
            CGenericIoControl gnt(pParams->hFile, pkspin,cbGoodSize, cbGoodSize, 0, 0);
            DWORD dwError;
            dwError = gnt.Test(TRUE, (ULONG)-1);
            OutputBufferSize = gnt.GetWritten();
            // now change the parameters as per test case - Pass Bad Audio Format
            PKSDATARANGE_AUDIO pKsDataAudio;        // for KSDATARNAGE_AUDIO
            PKSDATARANGE_MUSIC pKsDataMusic;        // for KSDATARANGE_MUSIC
            BOOL AudioRange =  FALSE;                            // if it is audio range then set it to true. if this is the Music range then set it to FALSE
            if ((pDataRange->MajorFormat == KSDATAFORMAT_TYPE_AUDIO))
                AudioRange =  TRUE;
            if ((pDataRange->MajorFormat == KSDATAFORMAT_TYPE_MUSIC))
                AudioRange =  FALSE;
            memcpy(pksrange, pDataRange,pDataRange->FormatSize); // copy data back - all valid value in the buffer and then change one of the filed as per testing
            pksrange->Flags = 0;

            // test for AUDIO Data Rannge
            if (AudioRange)    {
                pKsDataAudio = (PKSDATARANGE_AUDIO)pksrange;
                // test with maximum channels - on this case test case will successed. however check that driver reports format back with resonable values
// TODO: add test case for sending high value for maximum  channels and then verify the result retrived back from the driver               
/*                
                pKsDataAudio->MaximumChannels = 0xABCD;
                {
                    CGenericIoControl gnt1(pParams->hFile, pkspin, cbGoodSize, cbGoodSize, OutputBufferSize, OutputBufferSize);
                    dwError = gnt1.Test(FALSE, (ULONG)-1);
                    if (gnt1.GetFault() != FAULT_NO_FAULT)  {
                        SLOG (eError, "Passing Format with invalid MaximumChannels (0xABCD) successed instead of failure");
                        dwResult = FNS_FAIL;    //set test case as faliure and move on with fruther testing
                    }
                }
*/                
                memcpy(pksrange, pDataRange,pDataRange->FormatSize); // copy data back - all valid value in the buffer and then change one of the filed as per testing
                pksrange->Flags = 0;

            // test with MinimumBitsPerSample  > MaximumBitsPerSample 
                pKsDataAudio->MinimumBitsPerSample = 0xABCD;
                pKsDataAudio->MaximumBitsPerSample = 0x8;
                // now send request to the driver and it should get rejected.
                {
                    CGenericIoControl gnt1(pParams->hFile, pkspin, cbGoodSize, cbGoodSize, OutputBufferSize, OutputBufferSize);
                    dwError = gnt1.Test(TRUE, 0);
                    if (gnt1.GetFault() != FAULT_NO_FAULT)  {
                        SLOG (eError, "Passing Format with MinimumBitsPerSample > MaximumBitsPerSample successed instead of failure");
                        SLOG (eInfo2,  " here is the format used in the testing");
                        DisplayDataRangeAudio(pKsDataAudio);
                        dwResult = FNS_FAIL;    //set test case as faliure and move on with fruther testing
                    }
                }
                memcpy(pksrange, pDataRange,pDataRange->FormatSize); // copy data back - all valid value in the buffer and then change one of the filed as per testing
                pksrange->Flags = 0;
                
                // // test with MinimumBitsPerSample  > MaximumBitsPerSample
                pKsDataAudio->MinimumBitsPerSample = 0x8;
                pKsDataAudio->MaximumBitsPerSample = 0x4;
                {
                    CGenericIoControl gnt1(pParams->hFile, pkspin, cbGoodSize, cbGoodSize, OutputBufferSize, OutputBufferSize);
                    dwError = gnt1.Test(TRUE, 0);
                    if (gnt1.GetFault() != FAULT_NO_FAULT)  {
                        SLOG (eError, "Passing Format with MinimumBitsPerSample > MaximumBitsPerSample successed instead of failure");
                        SLOG (eInfo2,  " here is the format used in the testing");
                        DisplayDataRangeAudio(pKsDataAudio);
                        dwResult = FNS_FAIL;    //set test case as faliure and move on with fruther testing
                    }
                }
                memcpy(pksrange, pDataRange,pDataRange->FormatSize); // copy data back - all valid value in the buffer and then change one of the filed as per testing
                pksrange->Flags = 0;

                // // test with MinimumSampleFrequency   > MaximumSampleFrequency    
                pKsDataAudio->MinimumSampleFrequency  = 0xABCD;
                pKsDataAudio->MaximumSampleFrequency  = 0;
                {
                    CGenericIoControl gnt1(pParams->hFile, pkspin, cbGoodSize, cbGoodSize, OutputBufferSize, OutputBufferSize);
                    dwError = gnt1.Test(TRUE, 0);
                    if (gnt1.GetFault() != FAULT_NO_FAULT)  {
                        SLOG (eError, "Passing Format with MinimumSampleFrequency > MaximumSampleFrequency successed instead of failure");
                        SLOG (eInfo2,  " here is the format used in the testing");
                        DisplayDataRangeAudio(pKsDataAudio);
                        dwResult = FNS_FAIL;    //set test case as faliure and move on with fruther testing
                    }
                }
                memcpy(pksrange, pDataRange,pDataRange->FormatSize); // copy data back - all valid value in the buffer and then change one of the filed as per testing
                pksrange->Flags = 0;

                // test with the MaximumSampleFrequency     
                pKsDataAudio->MaximumSampleFrequency  = 0;
                {
                    CGenericIoControl gnt1(pParams->hFile, pkspin, cbGoodSize, cbGoodSize, OutputBufferSize, OutputBufferSize);
                    dwError = gnt1.Test(TRUE, 0);
                    if (gnt1.GetFault() != FAULT_NO_FAULT)  {
                        SLOG (eError, "Passing Format with invalid MaximumSampleFrequency  (0x0) successed instead of failure");
                        SLOG (eInfo2,  " here is the format used in the testing");
                        DisplayDataRangeAudio(pKsDataAudio);
                        dwResult = FNS_FAIL;    //set test case as faliure and move on with fruther testing
                    }
                }
            }
            // TODO: else test for ksdatarange_Music 
            memcpy(pksrange, pDataRange,pDataRange->FormatSize); // copy data back - all valid value in the buffer and then change one of the filed as per testing
            pksrange->Flags = 0;
            // now test realted to KSDATAFORMAT_WAVEFORMATEX 
        }
        
next_range:
        if (pkspin)    
            delete[] pkspin;
        ULONG size = (thisIsAttributeList ? ((PKSMULTIPLE_ITEM)pDataRange)->Size : pDataRange->FormatSize);
        pDataRange = PKSDATARANGE(((LPBYTE)pDataRange) + size);
        pDataRange = PKSDATARANGE( (ULONG_PTR(pDataRange)  + (ULONG_PTR)7) & ~(ULONG_PTR)7);
    }
    if (pRanges)
            LocalFree(pRanges);
    return dwResult;
}


DWORD WINAPI Generic_Property_KSPROPERTY_PIN_DATAINTERSECTION_12(PGAUDITPARAMS pParams)
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
        return FNS_PASS;
    if (!PIN())
        return FNS_PASS;

    
    DWORD dwResult = FNS_PASS;
    PKSDATAFORMAT       pDataIntersection = NULL;
    ULONG               cbDataIntersection;
    PKSP_PIN            pksPPin;
    ULONG               cbKsPPin;
    BOOL                fRes = FALSE;
    BOOL fCorrectPinType = FALSE;
    CKsPin* pPinInst =  PIN();
    // we want the IRPS flowin' in
    if(pPinInst->m_Descriptor.Communication != KSPIN_COMMUNICATION_SINK)    {
        SLOG(eInfo2, "Not testing for this pin as it is not the pin with KSPIN_COMMUNICATION_SINK ");    
        return dwResult;
    }
    PKSDATAFORMAT pPinDataFormat = &MidiPinFormat;
    if (PinType ==1) 
        pPinDataFormat = & (AudioPinFormat.DataFormat);
    if (PinType ==2) 
        pPinDataFormat = & (DsoundPinFormat.DataFormat);
    // if this aint a PCM pin, then skip it
    fCorrectPinType = IsEqualGUID(pPinDataFormat->Specifier, KSDATAFORMAT_SPECIFIER_WAVEFORMATEX) ||
                                IsEqualGUID(pPinDataFormat->Specifier, KSDATAFORMAT_SPECIFIER_DSOUND);
    if(!fCorrectPinType)    {
        SLOG(eInfo2, "This is not PCM Pin");    
        return dwResult;
    }

    cbKsPPin = sizeof(KSP_PIN) + sizeof(KSMULTIPLE_ITEM) + s_rgksDataRangeInvalid[0].DataRange.FormatSize * 1;
    pksPPin = (PKSP_PIN) new BYTE [cbKsPPin];
    if (!pksPPin)    {
        SLOG(eError, "ERROR: Internal memory allocation failure");
        dwResult = FNS_FAIL;
        return dwResult;
    }
    memset(pksPPin,  0,  cbKsPPin);
    memcpy(pksPPin, &pParams->Prop,  pParams->cbSize);
    pksPPin->PinId = PIN()->m_nId;
    pksPPin->Reserved = 0;
    PKSMULTIPLE_ITEM pksmi = (PKSMULTIPLE_ITEM)(pksPPin + 1);
    pksmi->Count = 1;
    pksmi->Size = s_rgksDataRangeInvalid[0].DataRange.FormatSize + sizeof(KSMULTIPLE_ITEM);
    PKSDATARANGE pksdr = (PKSDATARANGE)(pksmi + 1);
    for(int i = 0; i < SIZEOF_ARRAY(s_rgksDataRangeInvalid); i++)   {
        memcpy(pksdr, &s_rgksDataRangeInvalid[i], s_rgksDataRangeInvalid[i].DataRange.FormatSize);
        pksdr->Flags = 0;
        cbDataIntersection = 0;
        SLOG(eInfo2, "    Submitting datarange:");
        CGenericIoControl gnt(pParams->hFile, pksPPin,cbKsPPin, cbKsPPin, 0, 0);
        DWORD dwError;
        dwError = gnt.Test(TRUE, (ULONG)-1);
        cbDataIntersection = gnt.GetWritten();
        if((!dwError) && (cbDataIntersection > 0))  {
            if(i < 3)   {
                SLOG(eError, "  Incorrectly succeeded call to KSPROPERTY_PIN_DATAINTERSECTION, querying size only");
                dwResult = FNS_FAIL;
            }
            CGenericIoControl gnt1(pParams->hFile, pksPPin, cbKsPPin, cbKsPPin, cbDataIntersection, cbDataIntersection);
            dwError = gnt1.Test(TRUE, (ULONG)-1);
            if (gnt1.GetFault() != FAULT_NO_FAULT)  {   
                SLOG(eError, "  Incorrectly succeeded call to KSPROPERTY_PIN_DATAINTERSECTION, querying for format");
                dwResult = FNS_FAIL;
            }
        }
    }
    if (pksPPin)    
        delete[] pksPPin;

    return dwResult;
}

DWORD WINAPI Generic_Property_KSPROPERTY_PIN_DATAINTERSECTION(PGAUDITPARAMS pParams, ULONG TestFlag)
// Correct Range but invalid buffer / invalid multiple count
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
        return FNS_PASS;
    DWORD dwResult = FNS_PASS;
    // Get data ranges first and then try with all data ranges
    PKSMULTIPLE_ITEM pRanges = NULL;
    BOOL fRes;
    fRes = FILTER()->GetPinPropertyMulti(PIN()->m_nId, KSPROPSETID_Pin, KSPROPERTY_PIN_DATARANGES, &pRanges);
    if (!fRes)  {
        SLOG(eError, "ERROR: Failed to get pin %ld dataranges\n", PIN()->m_nId);
        dwResult = FNS_FAIL;
        return dwResult;
    }
    PKSDATARANGE pDataRange = (PKSDATARANGE) (pRanges + 1);
    pDataRange = (PKSDATARANGE)( ( ULONG_PTR(pDataRange) + (ULONG_PTR)7) & ~(ULONG_PTR)7);
    ULONG cbGoodSize = 0;
    
    bool thisIsAttributeList = false;
    bool nextIsAttributeList = false;

    for(UINT i=0; i < pRanges->Count; i++)  {
        PKSP_PIN pkspin = NULL;
        thisIsAttributeList = nextIsAttributeList;
        nextIsAttributeList = false;
        
        if(!thisIsAttributeList)  {
            nextIsAttributeList = ((pDataRange->Flags & KSDATARANGE_ATTRIBUTES) == KSDATARANGE_ATTRIBUTES);

            cbGoodSize = sizeof(KSP_PIN) + sizeof(KSMULTIPLE_ITEM) + pDataRange->FormatSize; // since we can have differenct size then size of KSDATARANGE
            pkspin = (PKSP_PIN) new BYTE [cbGoodSize];
            if (!pkspin)    {
                SLOG(eError, "ERROR: Internal memory allocation failure");
                dwResult = FNS_FAIL;
                if (pRanges)
                    LocalFree(pRanges);
                return dwResult;
            }
            memset(pkspin,  0,  cbGoodSize);
            PKSMULTIPLE_ITEM   pksitems = (PKSMULTIPLE_ITEM)(pkspin + 1);
            PKSDATARANGE       pksrange = (PKSDATARANGE)(pksitems + 1);
            memcpy(pkspin, &pParams->Prop,  pParams->cbSize);
            pkspin->PinId = PIN()->m_nId;
            pkspin->Reserved = 0;
            pksitems->Count = 1;
            pksitems->Size  = sizeof(KSMULTIPLE_ITEM) + pDataRange->FormatSize;
            memcpy(pksrange, pDataRange,pDataRange->FormatSize);
            pksrange->Flags = 0;
            // first get the required buffersize by passing buffer size equal to zero
            ULONG OutputBufferSize = 0;
            // the FormatSize is correct but we are missing the specifier
            // also the out size is correct
            CGenericIoControl gnt(pParams->hFile, pkspin, 
                                          cbGoodSize, cbGoodSize, 
                                          0, 
                                          0);
            DWORD dwError;
            dwError = gnt.Test(TRUE, (ULONG)-1);
            // Now  there are chances that driver may wants to return more data then size of (KSDATAFORMAT).
            // so first get the data size and then allocate buffer long enough and then send request again.
            if ((dwError == ERROR_INSUFFICIENT_BUFFER) || (dwError == ERROR_MORE_DATA))
                    OutputBufferSize = gnt.GetWritten();
            // TODO: in this case dirve is faulty as it has to send result ERROR_INSUFFICIENT_BUFFER or ERROR_MORE_DATA
            else
                goto NEXTRANGE;
            // now change the parameters as per test case
            if (TestFlag & TESTFLG_INVALID_PIN_ID)
                pkspin->PinId = FILTER()->m_PinFactoriesCount +1;    
            if (TestFlag & TESTFLG_INVALID_PIN_ID_ULONG_MAX)
                pkspin->PinId = ULONG_MAX;    
            if (TestFlag == TESTFLG_SMALL_OUTPUT_BUFFER)    {
                // some driver returns outputbufferszie (required buffer) equal to zero. - it is a driver bug. In this case we shoudl fail test case.
                if (OutputBufferSize== 0)   {
                    SLOG (eError, "Driver has returned zero buffer required on passing null buffer for output buffer which is wrong");
                    SLOG (eInfo2,  "Here is the format used in the testing");
                    DisplayDataRange(pksrange);            
                    dwResult = FNS_FAIL;
                    goto NEXTRANGE; // we will continue testing with next range.
                }
                OutputBufferSize = OutputBufferSize - 1;
            }
            if (TestFlag == TESTFLG_NULL_OUTPUT_BUFFER)
                OutputBufferSize = 0;
            if (TestFlag == TESTFLG_INVALID_MULTIPLE_COUNT)
                pksitems->Count = 0x80000000;
            {
                CGenericIoControl gnt1(pParams->hFile, pkspin, 
                                              cbGoodSize, cbGoodSize, 
                                              OutputBufferSize, 
                                              OutputBufferSize);
                dwError = gnt1.Test(TRUE, 0);
                if (gnt1.GetFault() != FAULT_NO_FAULT)  {
                    dwResult = FNS_FAIL;
                    goto NEXTRANGE; // we will continue testing with next range.
                }
            }   
        }
NEXTRANGE:
        ULONG size = (thisIsAttributeList ? ((PKSMULTIPLE_ITEM)pDataRange)->Size : pDataRange->FormatSize);
        pDataRange = PKSDATARANGE(((LPBYTE)pDataRange) + (size) ) ;  //since formatsize may be different then size of KSDATARANGE or KSDATARNAGE_AUDIO
        pDataRange = PKSDATARANGE( (ULONG_PTR(pDataRange)  + (ULONG_PTR)7) & ~(ULONG_PTR)7);
        if (pkspin)    
            delete[] pkspin;
    }
    if (pRanges)
        LocalFree(pRanges);
    return dwResult;
}

DWORD WINAPI Generic_Property_KSPROPERTY_PIN_DATAINTERSECTION_RANGE(PGAUDITPARAMS pParams, ULONG TestFlag)
// Invalid Range Specifier
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
        return FNS_PASS;
    DWORD dwResult = FNS_PASS;
    // Get data ranges first and then try with all data ranges
    PKSMULTIPLE_ITEM pRanges = NULL;
    PKSDATARANGE       pksrange = NULL; 
    BOOL fRes;
    fRes = FILTER()->GetPinPropertyMulti(PIN()->m_nId, KSPROPSETID_Pin, KSPROPERTY_PIN_DATARANGES, &pRanges);
    if (!fRes)  {
        SLOG(eError, "ERROR: Failed to get pin %ld dataranges\n", PIN()->m_nId);
        dwResult = FNS_FAIL;
        return dwResult;
    }
    PKSDATARANGE pDataRange = (PKSDATARANGE) (pRanges + 1);
    pDataRange = (PKSDATARANGE)( ( ULONG_PTR(pDataRange) + (ULONG_PTR)7) & ~(ULONG_PTR)7);

    ULONG cbGoodSize = 0;

    //Before we try with the test make sure that datarange.specifier is not GUID_NULL or not equal to KSDATAFORMAT_SPECIFIER_NONE
    
    bool thisIsAttributeList = false;
    bool nextIsAttributeList = false;

    for(UINT i=0; i < pRanges->Count; i++)  {
        PKSP_PIN pkspin = NULL;
        
        thisIsAttributeList = nextIsAttributeList;
        nextIsAttributeList = false;
        
        if(thisIsAttributeList)  {
            goto NextRange;
        }
        
        nextIsAttributeList = ((pDataRange->Flags & KSDATARANGE_ATTRIBUTES) == KSDATARANGE_ATTRIBUTES);

        if ((pDataRange->Specifier != GUID_NULL) && (pDataRange->Specifier != KSDATAFORMAT_SPECIFIER_NONE))  {       // if the format is realted to AUDIO then only create pin
            // test with missing specifier
            cbGoodSize = sizeof(KSP_PIN) + sizeof(KSMULTIPLE_ITEM) + pDataRange->FormatSize; // since we can have differenct size then size of KSDATARANGE
            pkspin = (PKSP_PIN) new BYTE[cbGoodSize];
            if (!pkspin)    {
                SLOG(eError, "ERROR: Internal memory allocation failure");
                dwResult = FNS_FAIL;
                if (pRanges)
                    LocalFree(pRanges);
                return dwResult;
            }
            memset(pkspin,  0,  cbGoodSize);
            PKSMULTIPLE_ITEM   pksitems = (PKSMULTIPLE_ITEM)(pkspin + 1);
            pksrange = (PKSDATARANGE)(pksitems + 1);
            memcpy(pkspin, &pParams->Prop,  pParams->cbSize);
            pkspin->PinId = PIN()->m_nId;
            pkspin->Reserved = 0;
            pksitems->Count = 1;
            pksitems->Size  = sizeof(KSMULTIPLE_ITEM) + pDataRange->FormatSize;
            memcpy(pksrange, pDataRange,pDataRange->FormatSize);
            pksrange->Flags = 0;
            // first get the required buffersize by passing buffer size equal to zero
            ULONG OutputBufferSize = 0;
            // the FormatSize is correct but we are missing the specifier
            // also the out size is correct
            CGenericIoControl gnt(pParams->hFile, pkspin, 
                                          cbGoodSize, cbGoodSize, 
                                          0, 
                                          0);
            DWORD dwError;
            dwError = gnt.Test(TRUE, (ULONG)-1);
            OutputBufferSize = gnt.GetWritten();
            //We have bug in sysaudio / port calss  for the same 761926            
            if ((dwError == 0x01) || (dwError == 0x490) || (dwError == 0x491) || (dwError == 0x32)) {
                if (pkspin)    
                    delete[] pkspin;
                goto NextRange;
            }
            
            // some driver returns outputbufferszie (required buffer) equal to zero. - it is a driver bug. In this case we should fail test case.
            if (OutputBufferSize== 0)   {
                SLOG (eError, "Driver has returned zero buffer required on passing null buffer for output buffer which is wrong");
                dwResult = FNS_FAIL;
                if (pkspin)    
                    delete[] pkspin;
                goto EXIT;
            }
            
            // now change the parameters as per test case
            if (TestFlag & TESTFLG_RANGE_SPECIFIER_MISSING)
                pksrange->FormatSize = sizeof(KSDATARANGE);
            if (TestFlag & TESTFLG_BAD_LARGE_RANGE_SIZE) 
                pksrange->FormatSize = 1024 * 1024;
            if (TestFlag & TESTFLG_BAD_RANGE_SIZE) 
                pksrange->FormatSize = (pDataRange->FormatSize) * 8; 
            if (TestFlag & TESTFLG_SMALL_RANGE_SIZE) 
                pksrange->FormatSize = (pDataRange->FormatSize) - 8; 
            
            CGenericIoControl gnt1(pParams->hFile, pkspin, 
                                          cbGoodSize, cbGoodSize, 
                                          OutputBufferSize, 
                                          OutputBufferSize);
            dwError = gnt1.Test(TRUE, 0);
            if (gnt1.GetFault() != FAULT_NO_FAULT)  {
                dwResult = FNS_FAIL;
                SLOG (eInfo2,  "Here is the format used in the testing");
                DisplayDataRange(pksrange);            
                
                if (pkspin)    
                    delete[] pkspin;
                goto EXIT;
            }
            if (pkspin)    
                delete[] pkspin;

        }
NextRange:
        ULONG size = (thisIsAttributeList ? ((PKSMULTIPLE_ITEM)pDataRange)->Size : pDataRange->FormatSize);
        pDataRange = PKSDATARANGE(((LPBYTE)pDataRange) + (size) ) ;  //since formatsize may be different then size of KSDATARANGE or KSDATARNAGE_AUDIO
        pDataRange = PKSDATARANGE( (ULONG_PTR(pDataRange)  + (ULONG_PTR)7) & ~(ULONG_PTR)7);
        
    }
EXIT:    
    if (pRanges)
        LocalFree(pRanges);
    return dwResult;
}

DWORD WINAPI Generic_Property_KSPROPERTY_PIN_DATAINTERSECTION_2(PGAUDITPARAMS pParams)
// Missing specifier
{
    return ( Generic_Property_KSPROPERTY_PIN_DATAINTERSECTION_RANGE(pParams, TESTFLG_RANGE_SPECIFIER_MISSING));
}

DWORD WINAPI Generic_Property_KSPROPERTY_PIN_DATAINTERSECTION_3(PGAUDITPARAMS pParams)
// sending very big format size buffer - it must fail   
{
    return ( Generic_Property_KSPROPERTY_PIN_DATAINTERSECTION_RANGE(pParams, TESTFLG_BAD_LARGE_RANGE_SIZE));
}
DWORD WINAPI Generic_Property_KSPROPERTY_PIN_DATAINTERSECTION_8(PGAUDITPARAMS pParams)
// sending very big format size buffer - it must fail   
{
    return ( Generic_Property_KSPROPERTY_PIN_DATAINTERSECTION_RANGE(pParams, TESTFLG_BAD_RANGE_SIZE));
}
DWORD WINAPI Generic_Property_KSPROPERTY_PIN_DATAINTERSECTION_9(PGAUDITPARAMS pParams)
// sending very big format size buffer - it must fail   
{
    return ( Generic_Property_KSPROPERTY_PIN_DATAINTERSECTION_RANGE(pParams, TESTFLG_SMALL_RANGE_SIZE));
}
DWORD WINAPI Generic_Property_KSPROPERTY_PIN_DATAINTERSECTION_4(PGAUDITPARAMS pParams)
//  Out size less than specifier required size  
{
    return ( Generic_Property_KSPROPERTY_PIN_DATAINTERSECTION(pParams, TESTFLG_SMALL_OUTPUT_BUFFER));
}

DWORD WINAPI Generic_Property_KSPROPERTY_PIN_DATAINTERSECTION_10(PGAUDITPARAMS pParams)
//  Out size less than specifier required size  
{
    return ( Generic_Property_KSPROPERTY_PIN_DATAINTERSECTION(pParams, TESTFLG_NULL_OUTPUT_BUFFER));
}


DWORD WINAPI Generic_Property_KSPROPERTY_PIN_DATAINTERSECTION_5(PGAUDITPARAMS pParams)
//sends 0x80000000 for count of pksmultiple item    (Multiple items count overflow check)
{
    return ( Generic_Property_KSPROPERTY_PIN_DATAINTERSECTION(pParams, TESTFLG_INVALID_MULTIPLE_COUNT));
}

DWORD WINAPI Generic_Property_KSPROPERTY_PIN_DATAINTERSECTION_6(PGAUDITPARAMS pParams)
// passing invalid id for pin factory    
{
    return ( Generic_Property_KSPROPERTY_PIN_DATAINTERSECTION(pParams, TESTFLG_INVALID_PIN_ID));
}

DWORD WINAPI Generic_Property_KSPROPERTY_PIN_DATAINTERSECTION_7(PGAUDITPARAMS pParams)
// passing invalid id for pin factory  - ULONG_MAX  
{
    return ( Generic_Property_KSPROPERTY_PIN_DATAINTERSECTION(pParams, TESTFLG_INVALID_PIN_ID_ULONG_MAX));
}

DWORD WINAPI Generic_Property_KSPROPERTY_PIN_DATAINTERSECTION_11(PGAUDITPARAMS pParams)
// Passing invalid Adio Format  
{
    return ( Generic_Property_KSPROPERTY_PIN_DATAINTERSECTION_BADAUDIORANGE(pParams, TESTFLG_AUDIO_FORMAT_BAD));
}

/* ____________________________________________________________________

    @TEST KSPROPERTY_PIN_CINSTANCES
   ____________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_PIN_CINSTANCES_1(PGAUDITPARAMS pParams)
//passing invalid pin factory ID =0x10000000
{
	if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		return FNS_PASS;


    // Rejects invalid pin id
    {
        pParams->PinProp.PinId = 0x10000000;
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, sizeof(KSPIN_CINSTANCES), sizeof(KSPIN_CINSTANCES));

        gnt.Test(TRUE, 0);
        if (gnt.GetFault() != FAULT_NO_FAULT) 
	    return FNS_FAIL;
    }
    
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_PIN_CINSTANCES_2(PGAUDITPARAMS pParams)
// passing invalid id for pin factory    
{
	if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		return FNS_PASS;
    CKsFilter* TestFilter = FILTER();   
    ULONG pinFactCount = TestFilter->m_PinFactoriesCount;
    SLOG (eInfo2, "value fo pin factory count %d", pinFactCount);
    pParams->PinProp.PinId = pinFactCount +1;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, sizeof(KSPIN_CINSTANCES), sizeof(KSPIN_CINSTANCES));
    gnt.Test(TRUE, 0);
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    else 
        return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_PIN_CINSTANCES_3(PGAUDITPARAMS pParams)
// passing invalid id for pin factory  - ULONG_MAX  
{
	if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		return FNS_PASS;
    pParams->PinProp.PinId = ULONG_MAX;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, sizeof(KSPIN_CINSTANCES), sizeof(KSPIN_CINSTANCES));
    gnt.Test(TRUE, 0);
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    else 
        return FNS_PASS;
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_CINSTANCES_4(PGAUDITPARAMS pParams)
// passing NULL buffer 
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;

    pParams->PinProp.PinId = PIN()->m_nId;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, 0, 0);
    gnt.Test(TRUE, 0);
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    else 
        return FNS_PASS;
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_CINSTANCES_5(PGAUDITPARAMS pParams)
// passing small buffer 
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;

    pParams->PinProp.PinId = PIN()->m_nId;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, sizeof(KSPIN_CINSTANCES) -1, sizeof(KSPIN_CINSTANCES)-1);
    gnt.Test(TRUE, 0);
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    else 
        return FNS_PASS;
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_CINSTANCES_6(PGAUDITPARAMS pParams)
// passing Large buffer 
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;

    pParams->PinProp.PinId = PIN()->m_nId;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, sizeof(KSPIN_CINSTANCES) +1, sizeof(KSPIN_CINSTANCES)+1);
    gnt.Test(FALSE, sizeof(KSPIN_CINSTANCES));
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    
    //make sure that data recevied are correct - since we han't created any pin yet on the filter, possible count >= current count must be the same
    KSPIN_CINSTANCES pinCInstances = *(KSPIN_CINSTANCES*) (gnt.GetData());
    if (pinCInstances.PossibleCount < pinCInstances.CurrentCount)  {
        SLOG (eError, "Possible Count (%d) and Current Count (%d) are not same for the pin factory (%d) ", pinCInstances.PossibleCount , pinCInstances.CurrentCount, PIN()->m_nId );
        return FNS_FAIL;
    }
    return FNS_PASS;
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_CINSTANCES_7(PGAUDITPARAMS pParams)
// passing valid data
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;;

    pParams->PinProp.PinId = PIN()->m_nId;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, sizeof(KSPIN_CINSTANCES) , sizeof(KSPIN_CINSTANCES));
    gnt.Test(FALSE, sizeof(KSPIN_CINSTANCES));
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    //make sure that data recevied are correct - since we han't created any pin yet on the filter, possible count >= current count 
    KSPIN_CINSTANCES pinCInstances = *(KSPIN_CINSTANCES*) (gnt.GetData());
    if (pinCInstances.PossibleCount < pinCInstances.CurrentCount)  {
        SLOG (eError, "Possible Count (%d) and Current Count (%d) are not same for the pin factory (%d) ", pinCInstances.PossibleCount , pinCInstances.CurrentCount, PIN()->m_nId );
        return FNS_FAIL;
    }
// TODO: need test case in which we wanted to create pin first then call this property to find out result are matching or not
// TODO: needs to take decision what to do if filter reports possible count = -1??
// TODO: We should also compare results of KSPROPERTY_PIN_CINSTANCES with result of KSPROPERTY_PIN_GLOBALCINSTANCES.
// TODO: Value from KSPROPERTY_PIN_GLOBALCINSTANCES> that of KSPROPERTY_PIN_CINSTANCES - needs to confrim with the developer

     return FNS_PASS;
}


/* ____________________________________________________________________

    @TEST KSPROPERTY_PIN_CATEGORY
   ____________________________________________________________________ */
DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_CATEGORY_1(PGAUDITPARAMS pParams)
// passing invalid pin Factory 
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;

    CKsFilter* TestFilter = FILTER();   
    ULONG pinFactCount = TestFilter->m_PinFactoriesCount;
    SLOG (eInfo2, "value fo pin factory count %d", pinFactCount);
    pParams->PinProp.PinId = pinFactCount +1;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, sizeof(GUID), sizeof(GUID));
    gnt.Test(TRUE, 0);
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    else 
        return FNS_PASS;
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_CATEGORY_2(PGAUDITPARAMS pParams)
// passing invalid pin Factory - ULONG_MAX
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;
    pParams->PinProp.PinId = ULONG_MAX;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, sizeof(GUID), sizeof(GUID));
    gnt.Test(TRUE, 0);
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    else 
        return FNS_PASS;
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_CATEGORY_3(PGAUDITPARAMS pParams)
// passing NULL buffer 
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;

    pParams->PinProp.PinId = PIN()->m_nId;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, 0, 0);
    gnt.Test(TRUE, 0);
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    else
        return FNS_PASS;
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_CATEGORY_4(PGAUDITPARAMS pParams)
// passing small buffer 
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;

    pParams->PinProp.PinId = PIN()->m_nId;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, sizeof(GUID) -1, sizeof(GUID)-1);
    gnt.Test(TRUE, 0);
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    else
        return FNS_PASS;
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_CATEGORY_5(PGAUDITPARAMS pParams)
// passing Large buffer 
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;

    pParams->PinProp.PinId = PIN()->m_nId;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, sizeof(GUID) +1, sizeof(GUID)+1);
    gnt.Test(FALSE, sizeof(GUID));
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    else    
        return FNS_PASS;
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_CATEGORY_6(PGAUDITPARAMS pParams)
// passing valid data
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;;

    pParams->PinProp.PinId = PIN()->m_nId;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, sizeof(GUID) , sizeof(GUID));
    gnt.Test(FALSE, sizeof(GUID));
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    else
        return FNS_PASS;
}

/* ____________________________________________________________________

    @TEST KSPROPERTY_PIN_COMMUNICATION
   ____________________________________________________________________ */
DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_COMMUNICATION_1(PGAUDITPARAMS pParams)
// passing invalid pin Factory 
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;

    CKsFilter* TestFilter = FILTER();   
    ULONG pinFactCount = TestFilter->m_PinFactoriesCount;
    SLOG (eInfo2, "value fo pin factory count %d", pinFactCount);
    pParams->PinProp.PinId = pinFactCount +1;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, sizeof(KSPIN_COMMUNICATION), sizeof(KSPIN_COMMUNICATION));
    gnt.Test(TRUE, 0);
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    else 
        return FNS_PASS;
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_COMMUNICATION_2(PGAUDITPARAMS pParams)
// passing invalid pin Factory - ULONG_MAX
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;

    pParams->PinProp.PinId = ULONG_MAX;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, sizeof(KSPIN_COMMUNICATION), sizeof(KSPIN_COMMUNICATION));
    gnt.Test(TRUE, 0);
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    else 
        return FNS_PASS;
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_COMMUNICATION_3(PGAUDITPARAMS pParams)
// passing NULL buffer 
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;

    pParams->PinProp.PinId = PIN()->m_nId;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, 0, 0);
    gnt.Test(TRUE, 0);
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    else    
        return FNS_PASS;
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_COMMUNICATION_4(PGAUDITPARAMS pParams)
// passing small buffer 
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;

    pParams->PinProp.PinId = PIN()->m_nId;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, sizeof(KSPIN_COMMUNICATION) -1, sizeof(KSPIN_COMMUNICATION)-1);
    gnt.Test(TRUE, 0);
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    else
        return FNS_PASS;
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_COMMUNICATION_5(PGAUDITPARAMS pParams)
// passing Large buffer 
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;

    pParams->PinProp.PinId = PIN()->m_nId;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, sizeof(KSPIN_COMMUNICATION) +1, sizeof(KSPIN_COMMUNICATION)+1);
    gnt.Test(FALSE, sizeof(KSPIN_COMMUNICATION));
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    //verify the result data
    LONG CommData = *(LONG*)gnt.GetData();
    if ((CommData < KSPIN_COMMUNICATION_NONE )||(CommData > KSPIN_COMMUNICATION_BRIDGE )) {
        SLOG (eError, "KSPIN_COMMUNICATION = %d - which is not defined in ks.h", CommData);
        return FNS_FAIL;
    }     
    return FNS_PASS;
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_COMMUNICATION_6(PGAUDITPARAMS pParams)
// passing valid data
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;

    pParams->PinProp.PinId = PIN()->m_nId;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, sizeof(KSPIN_COMMUNICATION) , sizeof(KSPIN_COMMUNICATION));
    gnt.Test(FALSE, sizeof(KSPIN_COMMUNICATION));
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    LONG CommData = *(LONG*)gnt.GetData();
    if ((CommData < KSPIN_COMMUNICATION_NONE )||(CommData > KSPIN_COMMUNICATION_BRIDGE )) {
        SLOG (eError, "KSPIN_COMMUNICATION = %d - which is not defined in ks.h", CommData);
        return FNS_FAIL;
    }     
    return FNS_PASS;
}

/* ____________________________________________________________________

    @TEST KSPROPERTY_PIN_DATAFLOW
   ____________________________________________________________________ */
DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_DATAFLOW_1(PGAUDITPARAMS pParams)
// passing invalid pin Factory 
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;

    CKsFilter* TestFilter = FILTER();   
    ULONG pinFactCount = TestFilter->m_PinFactoriesCount;
    SLOG (eInfo2, "value fo pin factory count %d", pinFactCount);
    pParams->PinProp.PinId = pinFactCount +1;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, sizeof(KSPIN_DATAFLOW), sizeof(KSPIN_DATAFLOW));
    gnt.Test(TRUE, 0);
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    else 
        return FNS_PASS;
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_DATAFLOW_2(PGAUDITPARAMS pParams)
// passing invalid pin Factory - ULONG_MAX
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;
    pParams->PinProp.PinId = ULONG_MAX;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, sizeof(KSPIN_DATAFLOW), sizeof(KSPIN_DATAFLOW));
    gnt.Test(TRUE, 0);
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    else 
        return FNS_PASS;
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_DATAFLOW_3(PGAUDITPARAMS pParams)
// passing NULL buffer 
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;

    pParams->PinProp.PinId = PIN()->m_nId;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, 0, 0);
    gnt.Test(TRUE, 0);
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    else
        return FNS_PASS;
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_DATAFLOW_4(PGAUDITPARAMS pParams)
// passing small buffer 
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;

    pParams->PinProp.PinId = PIN()->m_nId;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, sizeof(KSPIN_DATAFLOW) -1, sizeof(KSPIN_DATAFLOW)-1);
    gnt.Test(TRUE, 0);
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    else
        return FNS_PASS;
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_DATAFLOW_5(PGAUDITPARAMS pParams)
// passing Large buffer 
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;

    pParams->PinProp.PinId = PIN()->m_nId;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, sizeof(KSPIN_DATAFLOW) +1, sizeof(KSPIN_DATAFLOW)+1);
    gnt.Test(FALSE, sizeof(KSPIN_DATAFLOW));
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    //verify the result data
    LONG CommData = *(LONG*)gnt.GetData();
    if ((CommData < KSPIN_DATAFLOW_IN )||(CommData > KSPIN_DATAFLOW_OUT )) {
        SLOG (eError, "KSPIN_DATAFLOW = %d - which is not defined in ks.h", CommData);
        return FNS_FAIL;
    }     
    return FNS_PASS;
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_DATAFLOW_6(PGAUDITPARAMS pParams)
// passing valid data
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;

    pParams->PinProp.PinId = PIN()->m_nId;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, sizeof(KSPIN_DATAFLOW) , sizeof(KSPIN_DATAFLOW));
    gnt.Test(FALSE, sizeof(KSPIN_DATAFLOW));
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    LONG CommData = *(LONG*)gnt.GetData();
    if ((CommData < KSPIN_DATAFLOW_IN )||(CommData > KSPIN_DATAFLOW_OUT )) {
        SLOG (eError, "KSPIN_DATAFLOW = %d - which is not defined in ks.h", CommData);
        return FNS_FAIL;
    }     
    return FNS_PASS;
}
/* ____________________________________________________________________

    @TEST KSPROPERTY_PIN_NECESSARYINSTANCES
   ____________________________________________________________________ */
DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_NECESSARYINSTANCES_1(PGAUDITPARAMS pParams)
// passing invalid pin Factory 
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;

    CKsFilter* TestFilter = FILTER();   
    ULONG pinFactCount = TestFilter->m_PinFactoriesCount;
    SLOG (eInfo2, "value fo pin factory count %d", pinFactCount);
    pParams->PinProp.PinId = pinFactCount +1;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, sizeof(ULONG), sizeof(ULONG));
    gnt.Test(TRUE, 0);
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    else 
        return FNS_PASS;
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_NECESSARYINSTANCES_2(PGAUDITPARAMS pParams)
// passing invalid pin Factory - ULONG_MAX
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;

    pParams->PinProp.PinId = ULONG_MAX;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, sizeof(ULONG), sizeof(ULONG));
    gnt.Test(TRUE, 0);
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    else 
        return FNS_PASS;
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_NECESSARYINSTANCES_3(PGAUDITPARAMS pParams)
// passing NULL buffer 
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;

    pParams->PinProp.PinId = PIN()->m_nId;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, 0, 0);
    gnt.Test(TRUE, 0);
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    else
        return FNS_PASS;
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_NECESSARYINSTANCES_4(PGAUDITPARAMS pParams)
// passing small buffer 
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;

    pParams->PinProp.PinId = PIN()->m_nId;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, sizeof(ULONG) -1, sizeof(ULONG)-1);
    gnt.Test(TRUE, 0);
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    else
        return FNS_PASS;
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_NECESSARYINSTANCES_5(PGAUDITPARAMS pParams)
// passing Large buffer 
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;

    pParams->PinProp.PinId = PIN()->m_nId;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, sizeof(ULONG) +1, sizeof(ULONG)+1);
    gnt.Test(FALSE, sizeof(ULONG));
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    else
        return FNS_PASS;
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_NECESSARYINSTANCES_6(PGAUDITPARAMS pParams)
// passing valid data
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;

    pParams->PinProp.PinId = PIN()->m_nId;
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, sizeof(ULONG) , sizeof(ULONG));
    gnt.Test(FALSE, sizeof(ULONG));
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    else    
        return FNS_PASS;
}

/* ____________________________________________________________________

    @TEST KSPROPERTY_PIN_CONSTRAINEDDATARANGES
   ____________________________________________________________________ */

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_TESTDATARANGES(PGAUDITPARAMS pParams, GUID guidPropertySet, ULONG nProperty, eTestCaseType TestCase)
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;
    BOOL bExpectedTestResult = TRUE;    // as normally test case must fail as we are passing invalid parameters 
    ULONG cbExpectedWritten = 0;    //as normally test case must fail as we are passing invalid parameters 
    CKsFilter* TestFilter = FILTER();   
    ULONG pinFactCount = TestFilter->m_PinFactoriesCount;
    BOOL fRes;    
    PKSMULTIPLE_ITEM pPinDataRanges = NULL;
    
    //get first the size of the header of KSMULTIPLE_ITEM and property data that follows (KSMULTILPE_ITEM.SIZE)
    fRes = TestFilter->GetPinPropertyMulti(PIN()->m_nId, guidPropertySet, nProperty, &pPinDataRanges);
    if (!fRes)  {
        SLOG(eError, "ERROR: Failed to get pin %ld properties\n", PIN()->m_nId);
        return FNS_FAIL;
    }            
    ULONG OutBufferSize = pPinDataRanges->Size;
    // now do test as per test case
    
    switch (TestCase)   {
        case PinFactory_ULONG_MAX:
            pParams->PinProp.PinId = ULONG_MAX;  //setting pin id to ULONG_MAX and passing all other parameters correct        
            break;
        case PinFactory_BAD:
            pParams->PinProp.PinId = pinFactCount+1;  //setting pin id to invalid pin id and passing all other parameters correct        
            break;    
        case NULL_Buffer:
            OutBufferSize = 0;  //setting channel as invalid.   
            break;    
        case Small_Buffer:
            OutBufferSize = OutBufferSize -1;  //setting channel as invalid.   
            break;    
        case Large_Buffer:
            cbExpectedWritten = OutBufferSize;   
            OutBufferSize = OutBufferSize + 1;  //sending small buffer then required one
            bExpectedTestResult = FALSE;        // in case of Large buffer - Get/set request will Pass. In case of set request check for overwrite.
            break;
        case Valid_Test:
            cbExpectedWritten = OutBufferSize;
            OutBufferSize = OutBufferSize;  //
            bExpectedTestResult = FALSE;        // in case of Large buffer - Get/set request will Pass. In case of set request check for overwrite. result will be success but needs to varify value of lpBytesReturned of DeviceIoControl
            break;

        default:
            _ASSERT(FALSE);
    }
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, OutBufferSize, OutBufferSize);
    gnt.Test(bExpectedTestResult, cbExpectedWritten);
    if (gnt.GetFault() != FAULT_NO_FAULT) 
	return FNS_FAIL;
    else 
       return FNS_PASS; 
}

/* ____________________________________________________________________

    @TEST KSPROPERTY_PIN_CONSTRAINEDDATARANGES
   ____________________________________________________________________ */

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_CONSTRAINEDDATARANGES_1(PGAUDITPARAMS pParams)
// passing Invalid PinFactory 
{
    return Generic_Property_KSPROPERTY_PIN_TESTDATARANGES (pParams, KSPROPSETID_Pin, KSPROPERTY_PIN_CONSTRAINEDDATARANGES, PinFactory_BAD);
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_CONSTRAINEDDATARANGES_2(PGAUDITPARAMS pParams)
// passing Invalid PinFactory - ULONG_MAX
{
    return Generic_Property_KSPROPERTY_PIN_TESTDATARANGES (pParams, KSPROPSETID_Pin, KSPROPERTY_PIN_CONSTRAINEDDATARANGES, PinFactory_ULONG_MAX);
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_CONSTRAINEDDATARANGES_3(PGAUDITPARAMS pParams)
// passing null buffer
{
    return Generic_Property_KSPROPERTY_PIN_TESTDATARANGES (pParams, KSPROPSETID_Pin, KSPROPERTY_PIN_CONSTRAINEDDATARANGES, NULL_Buffer);
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_CONSTRAINEDDATARANGES_4(PGAUDITPARAMS pParams)
// passing small buffer
{
    return Generic_Property_KSPROPERTY_PIN_TESTDATARANGES (pParams, KSPROPSETID_Pin, KSPROPERTY_PIN_CONSTRAINEDDATARANGES, Small_Buffer);
}
DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_CONSTRAINEDDATARANGES_5(PGAUDITPARAMS pParams)
// passing Large buffer
{
    return Generic_Property_KSPROPERTY_PIN_TESTDATARANGES (pParams, KSPROPSETID_Pin, KSPROPERTY_PIN_CONSTRAINEDDATARANGES, Large_Buffer);
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_CONSTRAINEDDATARANGES_6(PGAUDITPARAMS pParams)
// passing valid test data 
{
    return Generic_Property_KSPROPERTY_PIN_TESTDATARANGES (pParams, KSPROPSETID_Pin, KSPROPERTY_PIN_CONSTRAINEDDATARANGES, Valid_Test);
}

/* ____________________________________________________________________

    @TEST KSPROPERTY_PIN_PROPOSEDATAFORMAT
    This property is write only property
   ____________________________________________________________________ */

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_PROPOSEDATAFORMAT(PGAUDITPARAMS pParams, eTestCaseType TestCase)
// passing Invalid PinFactory 
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
        return FNS_PASS;
     // we need a valid pin from the framework
    if (!PIN() || PIN()->m_handle == INVALID_HANDLE)
        return FNS_PASS;
    ULONG pinFactCount = FILTER()->m_PinFactoriesCount;
    ULONG BufferSize = 0;
    if (PinType == 1)
        BufferSize = sizeof(KSDATAFORMAT_WAVEFORMATEX);
    if (PinType == 2)
        BufferSize = sizeof(KSDATAFORMAT_DSOUND);
    if (PinType == 3)
        BufferSize = sizeof(KSDATAFORMAT);
    BOOL bExpectedTestResult = TRUE;
    // now do the test
    switch (TestCase)   {
        case PinFactory_ULONG_MAX:
            pParams->PinProp.PinId = ULONG_MAX;  //setting pin id to ULONG_MAX and passing all other parameters correct        
            break;
        case PinFactory_BAD:
            pParams->PinProp.PinId = pinFactCount+1;  //setting pin id to invalid pin id and passing all other parameters correct        
            break;    
        case NULL_Buffer:
            BufferSize = 0;  //setting channel as invalid.   
            break;    
        case Small_Buffer:
            BufferSize = BufferSize -1;  //setting channel as invalid.   
            break;    
        case Large_Buffer:
            BufferSize = BufferSize + 1;  //sending small buffer then required one
            bExpectedTestResult = FALSE;        // in case of Large buffer - Get/set request will Pass. In case of set request check for overwrite.
            break;
        case Valid_Test:
            bExpectedTestResult = FALSE;        // in case of Large buffer - Get/set request will Pass. In case of set request check for overwrite. result will be success but needs to varify value of lpBytesReturned of DeviceIoControl
            break;
        default:
            _ASSERT(FALSE);
    }
    
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, BufferSize, BufferSize);
    if (BufferSize == 0)    // menas we have null buffer test
        goto TEST;
    if (PinType == 1) { // pin was created with KSDATAFORMAT_WAVEFORMATEX
        memcpy( gnt.GetData(),  (PVOID)  &AudioPinFormat, BufferSize);
    }
    if (PinType == 2) { // pin was created with KSDATAFORMAT_DSOUND
        memcpy( gnt.GetData(),  (PVOID)  &DsoundPinFormat, BufferSize);
    }
    if (PinType == 3) { // pin was created with KSDATAFORMAT_WAVEFORMATEX
        memcpy( gnt.GetData(),  (PVOID)  &MidiPinFormat, BufferSize);
    }
    
TEST:    
    gnt.Test(bExpectedTestResult, 0);
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    else 
       return FNS_PASS; 

}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_PROPOSEDATAFORMAT_1(PGAUDITPARAMS pParams)    
{
    return Generic_Property_KSPROPERTY_PIN_PROPOSEDATAFORMAT (pParams, PinFactory_BAD);
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_PROPOSEDATAFORMAT_2(PGAUDITPARAMS pParams)
// passing Invalid PinFactory - ULONG_MAX
{
    return Generic_Property_KSPROPERTY_PIN_PROPOSEDATAFORMAT (pParams,  PinFactory_ULONG_MAX);
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_PROPOSEDATAFORMAT_3(PGAUDITPARAMS pParams)
// passing null buffer
{
    return Generic_Property_KSPROPERTY_PIN_PROPOSEDATAFORMAT (pParams,  NULL_Buffer);
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_PROPOSEDATAFORMAT_4(PGAUDITPARAMS pParams)
// passing small buffer
{
    return Generic_Property_KSPROPERTY_PIN_PROPOSEDATAFORMAT (pParams,  Small_Buffer);
}
DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_PROPOSEDATAFORMAT_5(PGAUDITPARAMS pParams)
// passing Large buffer
{
    return Generic_Property_KSPROPERTY_PIN_PROPOSEDATAFORMAT (pParams,  Large_Buffer);
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_PROPOSEDATAFORMAT_6(PGAUDITPARAMS pParams)
// passing valid test data 
{
    return Generic_Property_KSPROPERTY_PIN_PROPOSEDATAFORMAT (pParams,  Valid_Test);
}

/* ____________________________________________________________________

    @TEST KSPROPERTY_PIN_DATARANGES
   ____________________________________________________________________ */

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_DATARANGES_1(PGAUDITPARAMS pParams)
// passing Invalid PinFactory 
{
    return Generic_Property_KSPROPERTY_PIN_TESTDATARANGES (pParams, KSPROPSETID_Pin, KSPROPERTY_PIN_DATARANGES, PinFactory_BAD);
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_DATARANGES_2(PGAUDITPARAMS pParams)
// passing Invalid PinFactory - ULONG_MAX
{
    return Generic_Property_KSPROPERTY_PIN_TESTDATARANGES (pParams, KSPROPSETID_Pin, KSPROPERTY_PIN_DATARANGES, PinFactory_ULONG_MAX);
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_DATARANGES_3(PGAUDITPARAMS pParams)
// passing null buffer
{
    return Generic_Property_KSPROPERTY_PIN_TESTDATARANGES (pParams, KSPROPSETID_Pin, KSPROPERTY_PIN_DATARANGES, NULL_Buffer);
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_DATARANGES_4(PGAUDITPARAMS pParams)
// passing small buffer
{
    return Generic_Property_KSPROPERTY_PIN_TESTDATARANGES (pParams, KSPROPSETID_Pin,KSPROPERTY_PIN_DATARANGES, Small_Buffer);
}
DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_DATARANGES_5(PGAUDITPARAMS pParams)
// passing Large buffer
{
    return Generic_Property_KSPROPERTY_PIN_TESTDATARANGES (pParams, KSPROPSETID_Pin, KSPROPERTY_PIN_DATARANGES, Large_Buffer);
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_DATARANGES_6(PGAUDITPARAMS pParams)
// passing valid test data 
{
    return Generic_Property_KSPROPERTY_PIN_TESTDATARANGES (pParams, KSPROPSETID_Pin, KSPROPERTY_PIN_DATARANGES, Valid_Test);
}

/* ____________________________________________________________________

    @TEST KSPROPERTY_PIN_MEDIUMS
   ____________________________________________________________________ */

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_MEDIUMS_1(PGAUDITPARAMS pParams)
// passing Invalid PinFactory 
{
    return Generic_Property_KSPROPERTY_PIN_TESTDATARANGES (pParams, KSPROPSETID_Pin, KSPROPERTY_PIN_MEDIUMS, PinFactory_BAD);
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_MEDIUMS_2(PGAUDITPARAMS pParams)
// passing Invalid PinFactory - ULONG_MAX
{
    return Generic_Property_KSPROPERTY_PIN_TESTDATARANGES (pParams, KSPROPSETID_Pin, KSPROPERTY_PIN_MEDIUMS, PinFactory_ULONG_MAX);
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_MEDIUMS_3(PGAUDITPARAMS pParams)
// passing null buffer
{
    return Generic_Property_KSPROPERTY_PIN_TESTDATARANGES (pParams, KSPROPSETID_Pin, KSPROPERTY_PIN_MEDIUMS, NULL_Buffer);
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_MEDIUMS_4(PGAUDITPARAMS pParams)
// passing small buffer
{
    return Generic_Property_KSPROPERTY_PIN_TESTDATARANGES (pParams, KSPROPSETID_Pin, KSPROPERTY_PIN_MEDIUMS, Small_Buffer);
}
DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_MEDIUMS_5(PGAUDITPARAMS pParams)
// passing Large buffer
{
    return Generic_Property_KSPROPERTY_PIN_TESTDATARANGES (pParams, KSPROPSETID_Pin, KSPROPERTY_PIN_MEDIUMS, Large_Buffer);
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_MEDIUMS_6(PGAUDITPARAMS pParams)
// passing valid test data 
{
    return Generic_Property_KSPROPERTY_PIN_TESTDATARANGES (pParams, KSPROPSETID_Pin, KSPROPERTY_PIN_MEDIUMS, Valid_Test);
}

/* ____________________________________________________________________

    @TEST KSPROPERTY_PIN_INTERFACE
   ____________________________________________________________________ */

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_INTERFACES_1(PGAUDITPARAMS pParams)
// passing Invalid PinFactory 
{
    return Generic_Property_KSPROPERTY_PIN_TESTDATARANGES (pParams,  KSPROPSETID_Pin,  KSPROPERTY_PIN_INTERFACES, PinFactory_BAD);
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_INTERFACES_2(PGAUDITPARAMS pParams)
// passing Invalid PinFactory - ULONG_MAX
{
    return Generic_Property_KSPROPERTY_PIN_TESTDATARANGES (pParams, KSPROPSETID_Pin, KSPROPERTY_PIN_INTERFACES, PinFactory_ULONG_MAX);
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_INTERFACES_3(PGAUDITPARAMS pParams)
// passing null buffer
{
    return Generic_Property_KSPROPERTY_PIN_TESTDATARANGES (pParams, KSPROPSETID_Pin, KSPROPERTY_PIN_INTERFACES, NULL_Buffer);
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_INTERFACES_4(PGAUDITPARAMS pParams)
// passing small buffer
{
    return Generic_Property_KSPROPERTY_PIN_TESTDATARANGES (pParams, KSPROPSETID_Pin, KSPROPERTY_PIN_INTERFACES, Small_Buffer);
}
DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_INTERFACES_5(PGAUDITPARAMS pParams)
// passing Large buffer
{
    return Generic_Property_KSPROPERTY_PIN_TESTDATARANGES (pParams, KSPROPSETID_Pin, KSPROPERTY_PIN_INTERFACES, Large_Buffer);
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_INTERFACES_6(PGAUDITPARAMS pParams)
// passing valid test data 
{
    return Generic_Property_KSPROPERTY_PIN_TESTDATARANGES (pParams, KSPROPSETID_Pin, KSPROPERTY_PIN_INTERFACES, Valid_Test);
}


/* ____________________________________________________________________

    @TEST KSPROPERTY_AUDIOSIGNALPROCESSING_MODES
   ____________________________________________________________________ */
DWORD WINAPI  Generic_Property_KSPROPERTY_AUDIOSIGNALPROCESSING_MODES_1(PGAUDITPARAMS pParams)
    // passing Invalid PinFactory 
{
    return Generic_Property_KSPROPERTY_PIN_TESTDATARANGES (pParams, KSPROPSETID_AudioSignalProcessing, KSPROPERTY_AUDIOSIGNALPROCESSING_MODES, PinFactory_BAD);
}

DWORD WINAPI  Generic_Property_KSPROPERTY_AUDIOSIGNALPROCESSING_MODES_2(PGAUDITPARAMS pParams)
    // passing Invalid PinFactory - ULONG_MAX
{
    return Generic_Property_KSPROPERTY_PIN_TESTDATARANGES (pParams, KSPROPSETID_AudioSignalProcessing, KSPROPERTY_AUDIOSIGNALPROCESSING_MODES, PinFactory_ULONG_MAX);
}


/* ____________________________________________________________________

    @TEST KSPROPERTY_PIN_NAME
   ____________________________________________________________________ */

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_NAME(PGAUDITPARAMS pParams,eTestCaseType TestCase)
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		    return FNS_PASS;
    BOOL bExpectedTestResult = TRUE;    // as normally test case must fail as we are passing invalid parameters 
    ULONG cbExpectedWritten = 0;    //as normally test case must fail as we are passing invalid parameters 
    CKsFilter* TestFilter = FILTER();   
    ULONG pinFactCount = TestFilter->m_PinFactoriesCount;
    ULONG OutBufferSize;
    //first get the buffer size by passing null buffer
    CGenericIoControl gnt1(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize,0, 0);
    DWORD dwError;
    dwError = gnt1.Test(TRUE, (ULONG)-1);
    // Get the required buffer size
    OutBufferSize = gnt1.GetWritten();
    // now do test as per test case
    switch (TestCase)   {
        case PinFactory_ULONG_MAX:
            pParams->PinProp.PinId = ULONG_MAX;  //setting pin id to ULONG_MAX and passing all other parameters correct        
            break;
        case PinFactory_BAD:
            pParams->PinProp.PinId = pinFactCount+1;  //setting pin id to invalid pin id and passing all other parameters correct        
            break;    
        case NULL_Buffer:
            OutBufferSize = 0;  //setting channel as invalid.   
            break;    
        case Small_Buffer:
            if (OutBufferSize == 0)
            {
                SLOG(eInfo1, "Buffer size is 0, can't make smaller buffer than that.");
                return FNS_SKIPPED;
            }
            OutBufferSize = OutBufferSize -1;  //setting channel as invalid.   
            break;    
        case Large_Buffer:
            cbExpectedWritten = OutBufferSize;   
            OutBufferSize = OutBufferSize + 1;  //sending small buffer then required one
            bExpectedTestResult = FALSE;        // in case of Large buffer - Get/set request will Pass. In case of set request check for overwrite.
            break;
        case Valid_Test:
            cbExpectedWritten = OutBufferSize;
            OutBufferSize = OutBufferSize;  //
            bExpectedTestResult = FALSE;        // in case of Large buffer - Get/set request will Pass. In case of set request check for overwrite. result will be success but needs to varify value of lpBytesReturned of DeviceIoControl
            break;

        default:
            _ASSERT(FALSE);
    }
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, OutBufferSize, OutBufferSize);
    gnt.Test(bExpectedTestResult, cbExpectedWritten);
    if (gnt.GetFault() != FAULT_NO_FAULT) 
	return FNS_FAIL;
    else 
       return FNS_PASS; 
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_NAME_1(PGAUDITPARAMS pParams)
// passing Invalid PinFactory 
{
    return Generic_Property_KSPROPERTY_PIN_NAME(pParams,  PinFactory_BAD);
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_NAME_2(PGAUDITPARAMS pParams)
// passing Invalid PinFactory - ULONG_MAX
{
    return Generic_Property_KSPROPERTY_PIN_NAME (pParams,  PinFactory_ULONG_MAX);
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_NAME_3(PGAUDITPARAMS pParams)
// passing null buffer
{
    return Generic_Property_KSPROPERTY_PIN_NAME (pParams,  NULL_Buffer);
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_NAME_4(PGAUDITPARAMS pParams)
// passing small buffer
{
    return Generic_Property_KSPROPERTY_PIN_NAME (pParams,  Small_Buffer);
}
DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_NAME_5(PGAUDITPARAMS pParams)
// passing Large buffer
{
    return Generic_Property_KSPROPERTY_PIN_NAME (pParams,  Large_Buffer);
}

DWORD WINAPI  Generic_Property_KSPROPERTY_PIN_NAME_6(PGAUDITPARAMS pParams)
// passing valid test data 
{
    return Generic_Property_KSPROPERTY_PIN_NAME (pParams,  Valid_Test);
}



