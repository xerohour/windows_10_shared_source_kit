// Copyright (C) Microsoft. All rights reserved.
#include "common.h"

//--------------------------------------------------------------------------;
//
//  FindPin
//
//  Description:
//      Returns the Pin which supports the required format.
//
//  Arguments:
//      PCKsFilter              : pFilter
//      TTList <CKsPin> pPinList : List of pins.
//      PCKsPin *pKsDRMPin      : Returned pin.
//      PCKsPin pKsPStart
//      LPGUID  guidSubFormat
//      LPGUID  guidSpecifier
//
//  Return:
//      (BOOL)
//
//--------------------------------------------------------------------------;
BOOL
FindPin
(
    PCKsFilter                  pFilter,
    TTList <CKsPin>             *pPinList,
    PCKsPin                    *ppKsDRMPin,
    PCKsPin                     pKsPStart, /* = NULL */
    LPCGUID                     guidSubFormat, /* = NULL */
    LPCGUID                     guidSpecifier /* = NULL */
)
{
    BOOL                        fResult = TRUE;
    BOOL                        fFound = FALSE;
    TTNode <CKsPin>             *pNodePin;
    PCKsPin                     pKsPin;
    PKSMULTIPLE_ITEM            pMIDataRanges;
    ULONG                       i;

    try
    {
        *ppKsDRMPin = NULL;
        
        pNodePin = pPinList->GetHead();
        while (pNodePin && !fFound)
        {
            pKsPin = pNodePin->pData;
            
            fResult = pFilter->GetPinPropertyMulti(
                pKsPin->m_nId,
                KSPROPSETID_Pin,
                KSPROPERTY_PIN_DATARANGES,
                &pMIDataRanges);
            if (fResult)
            {
                PKSDATARANGE pKsDataRange = (PKSDATARANGE)(pMIDataRanges + 1);
                bool thisIsAttributeList = false;
                bool nextIsAttributeList = false;

                for (i = 0; i < pMIDataRanges->Count && !fFound; i++)
                {
                    thisIsAttributeList = nextIsAttributeList;
                    nextIsAttributeList = false;
                    
                    if(thisIsAttributeList)  {
                        goto NextRange;
                    }

                    nextIsAttributeList = ((pKsDataRange->Flags & KSDATARANGE_ATTRIBUTES) == KSDATARANGE_ATTRIBUTES);

                    if (IsEqualGUIDAligned
                            (pKsDataRange->MajorFormat, KSDATAFORMAT_TYPE_AUDIO) &&
                        pKsPin != pKsPStart)
                    {
                        fFound = TRUE;
                        
                        if (guidSubFormat)
                        {
                            if (IsEqualGUIDAligned
                                    (pKsDataRange->SubFormat, *guidSubFormat))
                            {
                                fFound = TRUE;
                            }
                            else
                            {
                                fFound = FALSE;
                            }
                        }

                        if (guidSpecifier && fFound)
                        {
                            if (IsEqualGUIDAligned
                                    (pKsDataRange->Specifier, *guidSpecifier))
                            {
                                fFound = TRUE;
                            }
                            else
                            {
                                fFound = FALSE;
                            }
                        }
                    }

                    if (fFound)
                    {
                        *ppKsDRMPin = pKsPin;
                    }
NextRange:
                    ULONG size = (thisIsAttributeList ? ((PKSMULTIPLE_ITEM)pKsDataRange)->Size : pKsDataRange->FormatSize);
                    pKsDataRange = (PKSDATARANGE) 
                        ((PBYTE) pKsDataRange + ((size + 7) & ~7));
                }
            }

            SafeLocalFree(pMIDataRanges);
            
            pNodePin = pPinList->GetNext(pNodePin);
        }
    }
    catch (LPSTR szError)
    {
        fResult = FALSE;
    }

    return fFound && fResult;
} // FindPin

//--------------------------------------------------------------------------;
//
//  FindDRMPin
//
//  Description:
//      Returns the Pin which supports DRM data format in the list of pins.
//
//  Arguments:
//      PCKsFilter              : pFilter
//      TTList <CKsPin> pPinList : List of pins.
//      PCKsPin *pKsDRMPin      : Returned pin.
//
//  Return:
//      (BOOL)
//
//--------------------------------------------------------------------------;
BOOL 
FindDRMPin
(
    PCKsFilter                  pFilter,
    TTList <CKsPin>             *pPinList,
    PCKsPin                    *ppKsDRMPin,
    PCKsPin                     pKsPStart
)
{
    return 
        FindPin(
            pFilter,
            pPinList,
            ppKsDRMPin,
            pKsPStart,
            &KSDATAFORMAT_SUBTYPE_DRM);
} // FindDRMPin

//--------------------------------------------------------------------------;
//
//  FindPCM
//
//  Description:
//      Returns the Pin which supports PCM data format in the list of pins.
//
//  Arguments:
//      PCKsFilter              : pFilter
//      TTList <CKsPin> pPinList : List of pins.
//      PCKsPin *pKsPCMPin      : Returned pin.
//
//  Return:
//      (BOOL)
//
//--------------------------------------------------------------------------;
BOOL
FindPCMPin
(
    PCKsFilter                  pFilter,
    TTList <CKsPin>             *pPinList,
    PCKsPin                    *ppKsPCMPin
)
{
    return 
        FindPin(
            pFilter,
            pPinList,
            ppKsPCMPin,
            NULL,
            &KSDATAFORMAT_SUBTYPE_PCM);
} // FindPCMPin

