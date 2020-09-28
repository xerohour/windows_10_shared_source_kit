// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
// File Name:
//
//  TestResourceBuild.cpp
//
// Abstract:
//
//  TAEF BuildResourceList implementation
//



// ------------------------------------------------------------------------------

#include "PreComp.h"
#include <itestresource.h>
#include "TestResource.h"

DEFINE_GUIDSTRUCT("84A3C1BD-88A1-40DD-8420-1D9C6442B524", KSNODETYPE_AUDIO_TELEPHONY);
#define KSNODETYPE_AUDIO_TELEPHONY DEFINE_GUIDNAMED(KSNODETYPE_AUDIO_TELEPHONY)

using namespace WEX::Common;
using namespace WEX::Logging;
using namespace WEX::TestExecution;

typedef struct _PinDeviceModel
{
    bool fIsStdStrmPin;
    bool fIsStdLoopedStrmPin;
    bool fIsWaveRtPin;
} PinDeviceModel;

// remove duplicates (most devices register under both _RENDER and _CAPTURE)
void RemoveDuplicateFilters(TTList<CKsFilter> &FilterList)
{
    TTNode<CKsFilter>*   pnFilter;
    
    pnFilter = FilterList.GetHead();
    while (pnFilter)
    {
        CKsFilter* pFilter = pnFilter->pData;
        TTNode<CKsFilter>* pnDup = FilterList.GetHead();
        while (pnDup)
        {
            CKsFilter* pDup = pnDup->pData;
            if(pDup == pFilter)
            {
                pnDup = FilterList.GetNext(pnDup);
                continue;
            }
            else
            {
                TTNode<CKsFilter>* pnDupNext = FilterList.GetNext(pnDup);
                if(strcmp(pDup->m_szFilterName, pFilter->m_szFilterName) == 0)
                {
                    FilterList.Remove(pnDup);
                    delete pDup;
                }
                pnDup = pnDupNext;
            }
        }

        pnFilter = FilterList.GetNext(pnFilter);
    }
}

void RemoveUnspecifiedInstanceIDFilters(TTList<CKsFilter> &FilterList)
{
    String targetDevice;

    // If we would like to run device specific test only, we will only keep that filter in the list.
    if (SUCCEEDED(RuntimeParameters::TryGetValue(L"InstanceId", targetDevice)))
    {
        if (targetDevice != "")
        {
            TTNode<CKsFilter>* pnFilter = FilterList.GetHead();
            while (pnFilter != NULL)
            {
                TTNode<CKsFilter>* pnNextFilter = FilterList.GetNext(pnFilter);
                if (pnFilter->pData != NULL)
                {
                    String szInstanceID(pnFilter->pData->m_szInstanceID);

                    if (szInstanceID.Compare(targetDevice) != 0)
                    {
                        delete pnFilter->pData;
                        FilterList.Remove(pnFilter);
                    }
                }
                pnFilter = pnNextFilter;
            }
        }
    }
}

void SetUserDefineFlagInFilter(TTList<CKsFilter> &FilterList)
{
    TTNode<CKsFilter>* pnFilter = FilterList.GetHead();
    while (pnFilter)
    {
        CKsFilter* pFilter = pnFilter->pData;
        TTNode<CKsFilter>* pnNext = FilterList.GetNext(pnFilter);

        if (nullptr != pFilter)
        {
            pFilter->m_UserDef = (0 == _tcsnicmp(pFilter->m_szHardwareID, _T("USB"), 3));
        }

        pnFilter = pnNext;
    }
}

CKsPin * FindPinIdInFilterCopy(CKsPin * pPin, CKsFilter * pFilter)
{
    TTList<CKsPin>* pPinList = &pFilter->m_listPins;
    TTNode<CKsPin>* pnPin = pPinList->GetHead();
    while(pnPin)
    {
        CKsPin * pCurrentPin = pnPin->pData;
        if(pPin->m_nId == pCurrentPin->m_nId)
        {
            return pCurrentPin;
        }
        pnPin = pPinList->GetNext(pnPin);
    }

    return NULL;
}

bool TryInstantiatePin(CPCMAudioPin * pPin, bool fLooped)
{
    CComHeapPtr<KSDATAFORMAT_WAVEFORMATEX> spKsDataFormatWaveFormatEx;

    // first, see if the pin provides a proposed format
    if(pPin->GetProposeFormat(&spKsDataFormatWaveFormatEx))
    {
        if(!VERIFY_IS_TRUE(FALSE != pPin->SetFormat((KSDATAFORMAT*)(spKsDataFormatWaveFormatEx.m_pData))))
        {
            return false;
        }
    }
    // if not, do the old data ranges / data intersection approach
    else
    {
        CComPtr<IWaveFormatEnum> pWFE;
        if(!VERIFY_SUCCEEDED(CreateWaveFormatEnum(&pWFE)))
        {
            return false;
        }

        if(!VERIFY_SUCCEEDED(pWFE->AddPCMFormats(
                                IWaveFormatEnum::WF_BIT_ALL,
                                IWaveFormatEnum::WF_SAMPLE_ALL,
                                IWaveFormatEnum::WF_CHANNEL_ALL,
                                IWaveFormatEnum::use_both)))
        {
            return false;
        }

        if(!VERIFY_IS_TRUE(FALSE != pPin->SetViablePCMWfxt(pWFE)))
        {
            return false;
        }
    }

    Log::Comment(L"Wave Format used for instantiation:");
    LogWaveFormat(g_pBasicLog, pPin->GetFormat());

    return FALSE != pPin->Instantiate(fLooped);
}

bool InitializePinDeviceModel
(
    CPCMAudioPin * pPin,
    PinDeviceModel * pPinModel
)
{
    // check pin property to see if it supports standard streaming and/or standard looped streaming
    CHeapPtr<KSMULTIPLE_ITEM, CLocalAllocator> spMultiItem;
    if(!VERIFY_IS_TRUE(FALSE != pPin->m_pFilter->GetPinPropertyMulti(
                pPin->m_nId,
                KSPROPSETID_Pin,
                KSPROPERTY_PIN_INTERFACES,
                &spMultiItem
            )))
    {
        return false;
    }

    if(!VERIFY_IS_NOT_NULL(spMultiItem)){return false;}

    PKSPIN_INTERFACE pInterfaces = (PKSPIN_INTERFACE)(&(spMultiItem[1]));
    ULONG  cInterfaces = spMultiItem->Count;

    pPinModel->fIsStdLoopedStrmPin = false;
    pPinModel->fIsStdStrmPin = false;
    pPinModel->fIsWaveRtPin= false;

    for (ULONG iInterface = 0; iInterface < cInterfaces; iInterface++)
    {
        if (IsEqualGUID(KSINTERFACESETID_Standard, pInterfaces[iInterface].Set))
        {
            if (KSINTERFACE_STANDARD_STREAMING == pInterfaces[iInterface].Id)
            {
                Log::Comment(L"The current device supports standard streaming.");
                pPinModel->fIsStdStrmPin = true;
            }

            if (KSINTERFACE_STANDARD_LOOPED_STREAMING == pInterfaces[iInterface].Id)
            {
                Log::Comment(L"The current device supports standard looped streaming.");
                pPinModel->fIsStdLoopedStrmPin = true;
            }
        }
    }

    // check for standard streaming support
    if (pPinModel->fIsStdStrmPin)
    {
        if(!VERIFY_IS_TRUE(
                TryInstantiatePin(pPin, FALSE),
                L"The current device supports standard streaming but failed to instantiate."))
        {
            return true;
        }
        pPin->ClosePin();
    }

    // check for standard looped streaming support
    if (pPinModel->fIsStdLoopedStrmPin)
    {
        if(!VERIFY_IS_TRUE(
                TryInstantiatePin(pPin, TRUE),
                L"The current device supports standard looped streaming but failed to instantiate."))
        {
            return true;
        }
        
        // Checking for KSPROPERTY_RTAUDIO_BUFFER property basic support on the pin, i.e. WaveRT
        KSPROPERTY Property;
        DWORD AccessFlag = 0;

        Property.Set   = KSPROPSETID_RtAudio;
        Property.Id    = KSPROPERTY_RTAUDIO_BUFFER;
        Property.Flags = KSPROPERTY_TYPE_BASICSUPPORT;

        // find out support for RT pin
        BOOL fBasicSupport = pPin->PropertyBasicSupport
        (
            KSPROPSETID_RtAudio, 
            KSPROPERTY_RTAUDIO_BUFFER, 
            &AccessFlag
        );
        if((fBasicSupport) && (AccessFlag & KSPROPERTY_TYPE_GET))
        {
            pPinModel->fIsWaveRtPin = true;
            Log::Comment(L"This is a WaveRT Pin.");
        }

        pPin->ClosePin();
    }

    return true;
}

bool CreatePinTestResource
(
    CKsPin * pPin,
    STACKWISE_DATAFLOW DataFlow,
    ResourceList &resourceList,
    REFGUID ResourceGUID,
    PinDeviceModel * pPinModel,
    bool fLooped
)
{
    CPCMAudioPin * pPinFromCopy;
    CAutoPtr<CKsFilter> spFilterCopy;
    CAutoPtr<CHalfApp> spHalfApp;
    CComPtr<ITestResource> spTestResource;
    ULONG cModes = 0;
    CComHeapPtr<GUID> spModes;
    CPCMAudioPin * pPCMPin = reinterpret_cast<CPCMAudioPin*>(pPin);

    if (!pPCMPin->GetAllProcessingModes(&cModes, &spModes))
    {
        cModes = 0;
    }

    // Iterate through all supported modes, plus one to run without any mode
    for (ULONG i = 0; i < cModes + 1; ++i)
    {
        GUID mode = GUID_NULL;
        if (i < cModes)
        {
            mode = spModes[i];
        }
        // make a copy of the filter and find the pin in it
        if (!VERIFY_WIN32_SUCCEEDED(pPin->m_pFilter->CopyFilter(&spFilterCopy.m_p)))
        {
            return false;
        }

        pPinFromCopy = reinterpret_cast<CPCMAudioPin *> (FindPinIdInFilterCopy(pPin, spFilterCopy));
        if (!VERIFY_IS_NOT_NULL(pPinFromCopy))
        { 
            return false; 
        }

        spHalfApp.Attach(new CHalfApp());
        if (!VERIFY_IS_NOT_NULL(spHalfApp))
        { 
            return false; 
        }

        spHalfApp->m_flow = DataFlow;
        spHalfApp->m_pFilter = spFilterCopy;
        spHalfApp->m_pPin = reinterpret_cast<CPCMAudioPin *>(pPinFromCopy);
        spHalfApp->m_fIsWaveRTPin = pPinModel->fIsWaveRtPin;
        spHalfApp->m_fIsStdLoopedStrmPin = pPinModel->fIsStdLoopedStrmPin;
        spHalfApp->m_fIsStdStrmPin = pPinModel->fIsStdStrmPin;
        spHalfApp->m_Mode = mode;

        if (!VERIFY_SUCCEEDED(CPinTestResource::CreateInstance(
            spHalfApp, ResourceGUID, fLooped, mode, &spTestResource)))
        {
            return false;
        }

        // Test resource now owns the HalfApp instance;
        spHalfApp.Detach();

        if (!VERIFY_SUCCEEDED(resourceList.Add(spTestResource)))
        {
            return false;
        }
    }

    return true;
}

bool
CheckPinExcluded
(
    CKsPin * pPin
)
{
    static int rgExcludedPins[32];
    static int cExcludedPins = 0;
    static bool fExcludedChecked = false;
    int pinId = pPin->m_nId;
    String excludedPins;

    if (!fExcludedChecked)
    {
        fExcludedChecked = true;
        if (FAILED(RuntimeParameters::TryGetValue(L"ExcludedPins", excludedPins)))
        {
            return false;
        }

        bool fFoundComma = true;
        int numberStart = 0;
        int numberEnd = 0;
        do
        {
            int nextExclusion = 0;
            numberEnd = excludedPins.Find(L",", numberStart);
            if (numberEnd < 0)
            {
                numberEnd = excludedPins.GetLength();
                fFoundComma = false;
            }
            String number = excludedPins.Mid(numberStart, numberEnd - numberStart);
            if (1 == swscanf((const WCHAR*)number, L"%d", &nextExclusion))
            {
                rgExcludedPins[cExcludedPins] = nextExclusion;
                cExcludedPins += 1;
                if (cExcludedPins >= _countof(rgExcludedPins))
                {
                    break;
                }
            }

            numberStart = numberEnd + 1;
        } while (fFoundComma);
    }

    for (int i = 0; i < cExcludedPins; ++i)
    {
        if (rgExcludedPins[i] == pinId)
        {
            Log::Comment(String().Format(L"Excluding Pin #%d from testing", pPin->m_nId));
            return true;
        }
    }
    
    return false;
}

bool
CheckTelephonyPin
(
    CKsFilter * pFilter,
    CKsPin * pPin
)
{
    TTNode<CKsNode> * pNode = NULL;
    TTNode<CKsConnection> * pConnection = NULL;
    ULONG ulTelephonyNodeId = KSFILTER_NODE;

    pNode = pFilter->m_listNodes.GetHead();
    while (pNode)
    {
        if (IsEqualGUID(pNode->pData->m_guidType, KSNODETYPE_AUDIO_TELEPHONY))
        {
            ulTelephonyNodeId = pNode->pData->m_nId;
            break;
        }
        pNode = pFilter->m_listNodes.GetNext(pNode);
    }

    if (KSFILTER_NODE == ulTelephonyNodeId)
    {
        return false;
    }

    pConnection = pFilter->m_listConnections.GetHead();
    while (pConnection)
    {
        if ((pConnection->pData->FromNode() == KSFILTER_NODE && pConnection->pData->FromNodePin() == pPin->m_nId) &&
            (pConnection->pData->ToNode() == ulTelephonyNodeId))
        {
            // From is the pin we're inspecting, To is the Telephony engine node
            return true;
        }
        if ((pConnection->pData->FromNode() == ulTelephonyNodeId) &&
            (pConnection->pData->ToNode() == KSFILTER_NODE && pConnection->pData->ToNodePin() == pPin->m_nId))
        {
            return true;
        }
        pConnection = pFilter->m_listConnections.GetNext(pConnection);
    }

    return false;
}

bool
AddPinTestResources
(
    CKsPin * pPin,
    STACKWISE_DATAFLOW DataFlow,
    ResourceList &resourceList,
    REFGUID ResourceGUID
)
{
    String szInstanceID(pPin->m_pFilter->m_szFilterName);
    PinDeviceModel PinModel = {0};
    Log::Comment(String().Format(L"Wave - [%s] Adding Pin Test Resource for Pin #%d", (PCWSTR)szInstanceID, pPin->m_nId));

    if(CheckPinExcluded(pPin))
    {
        // This isn't an error condition - return true to indicate we succeeded,
        // though we didn't add the pin to the list of resources
        Log::Comment(L"Pin is in excluded list");
        return true;
    }

    // determine if the supperted streaming and wavert stuff
    if(!VERIFY_IS_TRUE(InitializePinDeviceModel(
        reinterpret_cast<CPCMAudioPin *>(pPin), &PinModel)))
    {
        return false;
    }

    if(PinModel.fIsStdStrmPin)
    {
        // create a test resource for STD streaming pin
        if(!VERIFY_IS_TRUE(CreatePinTestResource(
                pPin, DataFlow, resourceList, ResourceGUID, &PinModel, false)))
        {
            return false;
        }
    }

    if(PinModel.fIsStdLoopedStrmPin)
    {
        // create a test resource for STD looped streaming pin
        if(!VERIFY_IS_TRUE(CreatePinTestResource(
                pPin, DataFlow, resourceList, ResourceGUID, &PinModel, true)))
        {
            return false;
        }
    }

    return true;
}

// Add pins to resource list from wave filters
bool AddPinTestResourcesFromFilter
(
    ResourceList& resourceList,
    REFGUID ResourceGUID,
    CKsFilter * pFilter
)
{
    if(!VERIFY_IS_NOT_NULL(pFilter))
    {
        return false;
    }

    TTList<CKsPin>* pPinList = &pFilter->m_listRenderSinkPins;
    TTNode<CKsPin>* pnPin = pPinList->GetHead();

    // Add all the render pins
    while(pnPin)
    {
        CKsPin * pPin = pnPin->pData;
        if(pPin)
        {
            if(CheckTelephonyPin(pFilter, pPin))
            {
                Log::Comment(String().Format(L"Skipping Telephony Render Pin #%d", pPin->m_nId));
            }
            else if(!VERIFY_IS_TRUE(AddPinTestResources(
                    pPin, render, resourceList, ResourceGUID)))
            {
                return false;
            }
        }
        pnPin = pPinList->GetNext(pnPin);
    }

    // Add all the capture pins
    pPinList = &pFilter->m_listCaptureSinkPins;
    pnPin = pPinList->GetHead();
    while(pnPin)
    {
        CKsPin * pPin = pnPin->pData;
        if(pPin)
        {
            if(CheckTelephonyPin(pFilter, pPin))
            {
                Log::Comment(String().Format(L"Skipping Telephony Capture Pin #%d", pPin->m_nId));
            }
            else if(!VERIFY_IS_TRUE(AddPinTestResources(
                    pPin, capture, resourceList, ResourceGUID)))
            {
                return false;
            }
        }
        pnPin = pPinList->GetNext(pnPin);
    }

    return true;
}

HRESULT __cdecl BuildResourceList(ResourceList& resourceList)
{
    HRESULT hr = S_OK;
    GUID ResourceGUID;
    TTList<CKsFilter> FilterList;
    TTNode<CKsFilter>* pNodeFilter;
    LPGUID arpguidCategories[2];
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);
    DisableVerifyExceptions disable;

    Log::Comment(L"In BuildResourceList");

    if(NULL == g_pBasicLog)
    {
        if(!VERIFY_SUCCEEDED(CreateWexBasicLog(&g_pBasicLog)))
        {
            goto Error;
        }
    }

    if(!VERIFY_IS_TRUE(FALSE != KslInitKsLib()))
    {
        hr = E_FAIL;
        goto Error;
    }
    KslRegisterLog(g_pBasicLog);

    arpguidCategories[0] = (LPGUID)&KSCATEGORY_AUDIO;
    arpguidCategories[1] = (LPGUID)&KSCATEGORY_RENDER;

    if(!VERIFY_IS_TRUE(FALSE != KslEnumFilters(&FilterList, ePCMAudio, arpguidCategories, 2)))
    {
        hr = E_FAIL;
        goto Error;
    }

    arpguidCategories[1] = (LPGUID)&KSCATEGORY_CAPTURE;
    if(!VERIFY_IS_TRUE(FALSE != KslEnumFilters(&FilterList, ePCMAudio, arpguidCategories, 2)))
    {
        hr = E_FAIL;
        goto Error;
    }

    RemoveDuplicateFilters(FilterList);

    RemoveUnspecifiedInstanceIDFilters(FilterList);

    SetUserDefineFlagInFilter(FilterList);

    if(!VERIFY_IS_TRUE(FALSE == FilterList.IsEmpty()))
    {
        hr = E_FAIL;
        goto Error;
    }

    KslLogEnumResults(&FilterList, NULL);

    hr = VERIFY_SUCCEEDED_RETURN(CoCreateGuid(&ResourceGUID));
    if(FAILED(hr))
    {
        goto Error;
    }

    pNodeFilter = FilterList.GetHead();
    while(pNodeFilter)
    {
        pNodeFilter->pData->EnumerateConnections();
        if(!VERIFY_IS_TRUE(AddPinTestResourcesFromFilter(
            resourceList, ResourceGUID, pNodeFilter->pData)))
        {
            hr = E_FAIL;
            break;
        }
        pNodeFilter = FilterList.GetNext(pNodeFilter);
    }

    Log::Comment(L"Resource enumeration complete.");
    Log::Comment(String().Format(L"Enumerated %u resources", resourceList.Count()));

Error:

    pNodeFilter = FilterList.GetHead();
    while(pNodeFilter)
    {
        TTNode<CKsFilter>* pNodeNextFilter = FilterList.GetNext(pNodeFilter);
        if(pNodeFilter->pData)
        {
            delete pNodeFilter->pData;
        }
            
        FilterList.Remove(pNodeFilter);
        pNodeFilter = pNodeNextFilter;
    }
    return hr;
}
