//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft
// premium shared source license agreement under which you licensed
// this source code. If you did not accept the terms of the license
// agreement, you are not authorized to use this source code.
// For the terms of the license, please see the license agreement
// signed by you and Microsoft.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES OR INDEMNITIES.
//

#include "CallController.h"

HRESULT CallController::Initialize(IMMDevice *pEndpoint)
{
    HRESULT hr = S_OK;
    CComPtr<IMMDeviceEnumerator> spEnumerator;
    BOOL fTelephonyEndpointFound = FALSE;

    hr = spEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER);
    if (SUCCEEDED(hr))
    {
        CComPtr<IPart>           spTopologyFilterEndpointConnectorPart;
        CComPtr<IDeviceTopology> spEndpointTopology;
        CComPtr<IConnector>      spEndpointConnector;
        CComPtr<IConnector>      spTopologyFilterEndpointConnector;
        CComPtr<IDeviceTopology> spAdapterTopology;
        CComPtr<IKsControl>      spWaveKsControl;
        CComHeapPtr<wchar_t>     wszAdapterTopologyDeviceId;
        CComPtr<IMMDevice>       spAdapterDevice;
        GUID                     guidEndpointClass = GUID_NULL;
        CComPtr<IConnector>      spConnector;
        CComPtr<IConnector>      spConConnectedTo;
        CComPtr<IPart>           spConnectedPart;
        CComPtr<IDeviceTopology> spMiniportTopology;
        CComHeapPtr<wchar_t>     wszPhoneTopologyDeviceId;
        CComPtr<IMMDevice>       spWaveAdapterDevice;
        CComPtr<IPartsList>      spPartsList;
        CComPtr<IPart>           spPart;
        UINT                     providerId = 0;

      
        // Get the IDeviceTopology interface from the endpoint device.
        hr = pEndpoint->Activate(
                  __uuidof(IDeviceTopology),
                  CLSCTX_ALL,
                  NULL,
                  (VOID**)&spEndpointTopology);

        // Get the default connector for the endpoint device which is connected to
        // the connector on the topology filter.
        if (SUCCEEDED(hr))
        {
            hr = spEndpointTopology->GetConnector(0, &spEndpointConnector);
        }

        // Get the endpoint connector on the topology filter.
        if (SUCCEEDED(hr))
        {
            hr = spEndpointConnector->GetConnectedTo(&spTopologyFilterEndpointConnector);
        }

        // Get the endpoint connector part, endpoint class, and local ID.
        if (SUCCEEDED(hr))
        {
            hr = spTopologyFilterEndpointConnector.QueryInterface(&spTopologyFilterEndpointConnectorPart);
        }

        if (SUCCEEDED(hr))
        {
            hr = spTopologyFilterEndpointConnectorPart->GetSubType(&guidEndpointClass);
        }

        // We've only "found" the phone topology if we find the cellular BIDI endpoint
        if (SUCCEEDED(hr))
        {
            if (guidEndpointClass != KSNODETYPE_TELEPHONY_BIDI)
            {
                hr = E_FAIL;
            }
        }

        // Get the IDeviceTopology interface for the adapter device from the endpoint connector part.
        if (SUCCEEDED(hr))
        {
            fTelephonyEndpointFound = TRUE;

            hr = spTopologyFilterEndpointConnectorPart->GetTopologyObject(&spAdapterTopology);
        }

        // Activate the KSControl on topo filter
        if (NULL == m_spTopologyKsControl)
        {
            if (SUCCEEDED(hr))
            {
                 hr = spAdapterTopology->GetDeviceId(&wszAdapterTopologyDeviceId);
            }

            if (SUCCEEDED(hr))
            {
                 hr = spEnumerator->GetDevice(wszAdapterTopologyDeviceId, &spAdapterDevice);
            }

            if (SUCCEEDED(hr))
            {
                hr = spAdapterDevice->Activate(
                        __uuidof(IKsControl),
                        CLSCTX_ALL,
                        NULL,
                        (VOID**) &m_spTopologyKsControl);
            }
        }

        // Now proceed with enumerating the rest of the topology
        if (SUCCEEDED(hr))
        {
            hr = spTopologyFilterEndpointConnectorPart->EnumPartsOutgoing(&spPartsList);
        }

        if (SUCCEEDED(hr))
        {
            hr = spPartsList->GetPart(0, &spPart);
        }

        if (SUCCEEDED(hr))
        {
            hr = spPart.QueryInterface(&spConnector);
        }

        if (SUCCEEDED(hr))
        {
            hr = spConnector->GetConnectedTo(&spConConnectedTo);
        }

        // retrieve the part that this connector is on, which should be a part
        // on the miniport.
        if (SUCCEEDED(hr))
        {
            hr = spConConnectedTo.QueryInterface(&spConnectedPart);
        }

        if (SUCCEEDED(hr))
        {
             hr = spConnectedPart->GetTopologyObject(&spMiniportTopology);
        }

        if (SUCCEEDED(hr))
        {
            hr = spMiniportTopology->GetDeviceId(&wszPhoneTopologyDeviceId);
        }

        if (SUCCEEDED(hr))
        {
             hr = spEnumerator->GetDevice(wszPhoneTopologyDeviceId, &spWaveAdapterDevice);
        }

        if (SUCCEEDED(hr))
        {
            hr = spWaveAdapterDevice->Activate(
                    __uuidof(IKsControl),
                    CLSCTX_ALL,
                    NULL,
                    (VOID**) &spWaveKsControl);
        }

        if (SUCCEEDED(hr))
        {
            hr = GetProviderId(spWaveKsControl, &providerId);
        }

        if (SUCCEEDED(hr))
        {
            // Save provider Id to the list and save provider Id and KsControl
            // for the associated wave filter to the map
            m_mapProviderWaveKsControl.SetAt(providerId,spWaveKsControl);
            m_ValidTelephonyProviderIds.AddTail(providerId);
        }
    }

    if (!fTelephonyEndpointFound)
    {
        hr = E_NOTFOUND;
    }

    return hr;
}


// Returns KsControl for the wave filter matching the provider id
HRESULT CallController::ObtainKsControl(_In_ UINT providerId, _Outptr_ IKsControl** ppKsControl)
{
    HRESULT hr = S_OK;
    BOOL bFound;
    CComPtr<IKsControl> spWaveKsControl;

    bFound = m_mapProviderWaveKsControl.Lookup(providerId, spWaveKsControl);

    if (bFound)   
    {
        spWaveKsControl.CopyTo(ppKsControl);
    }

    return hr;
}

// Returns all supported provider Ids
HRESULT CallController::RetrieveProviderId(_Inout_ UINT ProviderId[], _Out_ UINT* pCount)
{
    HRESULT hr = S_OK;
    POSITION pos = m_ValidTelephonyProviderIds.GetHeadPosition();
    *pCount = 0;

    do
    {
        ProviderId[*pCount] = m_ValidTelephonyProviderIds.GetNext(pos);
        (*pCount)++;
    } while (pos != NULL && *pCount < MAX_PROVIDERIDS);

    return hr;
}

// Returns provider id for a wave filter.
HRESULT CallController::GetProviderId(_In_ IKsControl* pKsControl, _Out_ UINT* pProviderId)
{
    HRESULT hr = S_OK;
    KSPROPERTY providerIdProp;
    ULONG ulBytesReturned;
    UINT dwGetVal = 0;
 
    if (pProviderId == NULL || pKsControl == NULL)
    {
        hr = E_POINTER;
    }

    if (SUCCEEDED(hr))
    {
        providerIdProp.Set = KSPROPSETID_TelephonyControl;
        providerIdProp.Id = KSPROPERTY_TELEPHONY_PROVIDERID;
        providerIdProp.Flags = KSPROPERTY_TYPE_GET;

        hr = pKsControl->KsProperty(
                (PKSPROPERTY)&providerIdProp,
                sizeof(providerIdProp),
                &dwGetVal,
                sizeof(dwGetVal),
                &ulBytesReturned
        );
    }

    if (SUCCEEDED(hr))
    {
        *pProviderId = dwGetVal;
    }

    return hr;
}

// Sets cellular volume. This is a property on topology filter.
HRESULT CallController::SetCellularVolume(_In_ LONG volume)
{
    HRESULT hr = S_OK;
    KSPROPERTY volumeProp;
    ULONG ulBytesReturned;
    KSPROPERTY_DESCRIPTION desc = {0};
    KSPROPERTY_DESCRIPTION * pdesc = NULL;
    PKSPROPERTY_MEMBERSHEADER pKsPropMembHead = NULL;
    PKSPROPERTY_STEPPING_LONG pKsPropStepLong = NULL;
 
    if (m_spTopologyKsControl == NULL)
    {
        hr = E_POINTER;
    }

    // Determine valid volume ranges
    if (SUCCEEDED(hr))
    {
        KSPROPERTY volumeSupportProp;
        ULONG cbSupportProp = 0;

        volumeSupportProp.Set = KSPROPSETID_TelephonyTopology;
        volumeSupportProp.Id = KSPROPERTY_TELEPHONY_VOLUME;
        volumeSupportProp.Flags = KSPROPERTY_TYPE_BASICSUPPORT;
        
        hr = m_spTopologyKsControl->KsProperty(
            (PKSPROPERTY)&volumeSupportProp,
            sizeof(volumeSupportProp),
            &desc,
            sizeof(desc),
            &cbSupportProp
        );
        
        if (SUCCEEDED(hr) && desc.DescriptionSize > sizeof(desc))
        {
            pdesc = (KSPROPERTY_DESCRIPTION*)CoTaskMemAlloc(desc.DescriptionSize);
            if (NULL == pdesc)
            {
                hr = E_OUTOFMEMORY;
            }

            if (SUCCEEDED(hr))
            {
                hr = m_spTopologyKsControl->KsProperty(
                    (PKSPROPERTY)&volumeSupportProp,
                    sizeof(volumeSupportProp),
                    pdesc,
                    desc.DescriptionSize,
                    &cbSupportProp
                );
            }

            if (SUCCEEDED(hr))
            {
                if (pdesc->PropTypeSet.Set != KSPROPTYPESETID_General ||
                    pdesc->PropTypeSet.Id != VT_I4 ||
                    pdesc->PropTypeSet.Flags != 0 ||
                    pdesc->MembersListCount < 1 ||
                    pdesc->Reserved != 0 ||
                    pdesc->DescriptionSize < (sizeof(KSPROPERTY_DESCRIPTION) + sizeof(KSPROPERTY_MEMBERSHEADER) + sizeof(KSPROPERTY_STEPPING_LONG)))
                {
                    hr = E_INVALIDARG;
                }
            }

            if (SUCCEEDED(hr))
            {
                pKsPropMembHead = reinterpret_cast<PKSPROPERTY_MEMBERSHEADER>(pdesc + 1);
                ULONG flags = pKsPropMembHead->Flags & (KSPROPERTY_MEMBER_FLAG_BASICSUPPORT_MULTICHANNEL | KSPROPERTY_MEMBER_FLAG_BASICSUPPORT_UNIFORM);
                if (pKsPropMembHead->MembersFlags != KSPROPERTY_MEMBER_STEPPEDRANGES ||
                    pKsPropMembHead->MembersSize < sizeof(KSPROPERTY_STEPPING_LONG) ||
                    pKsPropMembHead->MembersCount < 1 ||
                    (   flags != (KSPROPERTY_MEMBER_FLAG_BASICSUPPORT_MULTICHANNEL | KSPROPERTY_MEMBER_FLAG_BASICSUPPORT_UNIFORM)
                     && flags != 0))
                {
                    hr = E_INVALIDARG;
                }
            }
            
            if (SUCCEEDED(hr))
            {
                pKsPropStepLong = reinterpret_cast<PKSPROPERTY_STEPPING_LONG>(pKsPropMembHead + 1);
                
                // Round volume to nearest supported value
                volume = VALUE_NORMALIZE_IN_RANGE_EX(
                            volume, 
                            pKsPropStepLong->Bounds.SignedMinimum,
                            pKsPropStepLong->Bounds.SignedMaximum,
                            (LONG)(pKsPropStepLong->SteppingDelta)
                );
            }
        }
    } 

    if (SUCCEEDED(hr))
    {
        volumeProp.Set = KSPROPSETID_TelephonyTopology;
        volumeProp.Id = KSPROPERTY_TELEPHONY_VOLUME;
        volumeProp.Flags = KSPROPERTY_TYPE_SET;

        hr = m_spTopologyKsControl->KsProperty(
                (PKSPROPERTY)&volumeProp,
                sizeof(volumeProp),
                &volume,
                sizeof(LONG),
                &ulBytesReturned
        );
    }

    if (pdesc)
    {
        CoTaskMemFree(pdesc);
    }
    return hr;
}

// Returns current cellular volume. This is a property on topology filter.
HRESULT CallController::GetCellularVolume(_Out_ LONG* plVolume)
{
    HRESULT hr = S_OK;
    KSPROPERTY volumeProp;
    ULONG ulBytesReturned;
    LONG vol = 0;
 
    if (plVolume == NULL || m_spTopologyKsControl == NULL)
    {
        hr = E_POINTER;
    }

    if (SUCCEEDED(hr))
    {
        volumeProp.Set = KSPROPSETID_TelephonyTopology;
        volumeProp.Id = KSPROPERTY_TELEPHONY_VOLUME;
        volumeProp.Flags = KSPROPERTY_TYPE_GET;

        hr = m_spTopologyKsControl->KsProperty(
            (PKSPROPERTY)&volumeProp,
            sizeof(volumeProp),
            &vol,
            sizeof(LONG),
            &ulBytesReturned
        );
    }

    if (SUCCEEDED(hr))
    {
        *plVolume = vol;
    }

    return hr;
}

// Returns current cellular routing. This is a property on topology filter.
HRESULT CallController::GetCellularRouting(_Out_ PKSTOPOLOGY_ENDPOINTIDPAIR pRoutingInfo)
{
    HRESULT hr = S_OK;
    KSPROPERTY routingProp;
    KSTOPOLOGY_ENDPOINTIDPAIR cellularRouting = {};
    ULONG ulBytesReturned;
 
    if (pRoutingInfo == NULL || m_spTopologyKsControl == NULL)
    {
        hr = E_POINTER;
    }

    if (SUCCEEDED(hr))
    {
        routingProp.Set = KSPROPSETID_TelephonyTopology;
        routingProp.Id = KSPROPERTY_TELEPHONY_ENDPOINTIDPAIR;
        routingProp.Flags = KSPROPERTY_TYPE_GET;

        hr = m_spTopologyKsControl->KsProperty(
                (PKSPROPERTY)&routingProp,
                sizeof(routingProp),
                &cellularRouting,
                sizeof(KSTOPOLOGY_ENDPOINTIDPAIR),
                &ulBytesReturned
        );
    }

    if (SUCCEEDED(hr))
    {
        memcpy(pRoutingInfo, &cellularRouting, sizeof(cellularRouting));
    }

    return hr;
}

// Sets cellular routing. This is a property on topology filter.
HRESULT CallController::SetCellularRouting(_In_ KSTOPOLOGY_ENDPOINTIDPAIR RoutingInfo)
{
    HRESULT hr = S_OK;
    KSPROPERTY routingProp;
    ULONG ulBytesReturned;
 
    if (m_spTopologyKsControl == NULL)
    {
        hr = E_POINTER;
    }

    if (SUCCEEDED(hr))
    {
        routingProp.Set = KSPROPSETID_TelephonyTopology;
        routingProp.Id = KSPROPERTY_TELEPHONY_ENDPOINTIDPAIR;
        routingProp.Flags = KSPROPERTY_TYPE_SET;

        hr = m_spTopologyKsControl->KsProperty(
                (PKSPROPERTY)&routingProp,
                sizeof(routingProp),
                &RoutingInfo,
                sizeof(KSTOPOLOGY_ENDPOINTIDPAIR),
                &ulBytesReturned
        );
    }

    return hr;
}

// Enables cellular call for a given provider id and calltype.
HRESULT CallController::EnableCellular(_In_ UINT providerId, _In_ TELEPHONY_CALLTYPE callType)
{
    HRESULT hr = S_OK;
    KSTELEPHONY_CALLCONTROL callControl = {};
    CComPtr<IKsControl> spWaveKsControl;
    
    hr = ObtainKsControl(providerId, &spWaveKsControl);
    
    if (SUCCEEDED(hr))
    {
        callControl.CallType = callType;
        callControl.CallControlOp = TELEPHONY_CALLCONTROLOP_ENABLE;

        hr = HandleCallControl(spWaveKsControl, callControl);
    }

    return hr;
}

// Disables cellular call for a given provider id and calltype.
HRESULT CallController::DisableCellular(_In_ UINT providerId, _In_ TELEPHONY_CALLTYPE callType)
{
    HRESULT hr = S_OK;
    KSTELEPHONY_CALLCONTROL callControl = {};
    CComPtr<IKsControl> spWaveKsControl;
    
    hr = ObtainKsControl(providerId, &spWaveKsControl);

    if (SUCCEEDED(hr))
    {
        callControl.CallType = callType;
        callControl.CallControlOp = TELEPHONY_CALLCONTROLOP_DISABLE;

        hr = HandleCallControl(spWaveKsControl, callControl);
    }

    return hr;
}

HRESULT CallController::HandleCallControl(_In_ IKsControl* pKsControl, _In_ KSTELEPHONY_CALLCONTROL callControl)
{
    HRESULT hr = S_OK;
    KSPROPERTY callControlProp;
    ULONG ulBytesReturned;

    if (pKsControl == NULL)
    {
        hr = E_POINTER;
    }

    if (SUCCEEDED(hr))
    {
        callControlProp.Set = KSPROPSETID_TelephonyControl;
        callControlProp.Id = KSPROPERTY_TELEPHONY_CALLCONTROL;
        callControlProp.Flags = KSPROPERTY_TYPE_SET;

        hr = pKsControl->KsProperty(
                (PKSPROPERTY)&callControlProp,
                sizeof(callControlProp),
                &callControl,
                sizeof(callControl),
                &ulBytesReturned
        );
    }

    return hr;
}

// Begins provider change for a given provider id and calltype.
HRESULT CallController::BeginProviderChange(_In_ UINT providerId, _In_ TELEPHONY_CALLTYPE callType)
{
    HRESULT hr = S_OK;
    KSTELEPHONY_PROVIDERCHANGE providerChange = {};
    CComPtr<IKsControl> spWaveKsControl;

    hr = ObtainKsControl(providerId, &spWaveKsControl);

    if (SUCCEEDED(hr))
    {
        providerChange.CallType = callType;
        providerChange.ProviderChangeOp = TELEPHONY_PROVIDERCHANGEOP_BEGIN;

        hr = HandleProviderChange(spWaveKsControl, providerChange);
    }

    return hr;
}

// Ends provider change for a given provider id and calltype.
HRESULT CallController::EndProviderChange(_In_ UINT providerId, _In_ TELEPHONY_CALLTYPE callType)
{
    HRESULT hr = S_OK;
    KSTELEPHONY_PROVIDERCHANGE providerChange = {};
    CComPtr<IKsControl> spWaveKsControl;
    
    hr = ObtainKsControl(providerId, &spWaveKsControl);

    if (SUCCEEDED(hr))
    {
        providerChange.CallType = callType;
        providerChange.ProviderChangeOp = TELEPHONY_PROVIDERCHANGEOP_END;

        hr = HandleProviderChange(spWaveKsControl, providerChange);
    }

    return hr;
}

// Cancels provider change for a given provider id and calltype.
HRESULT CallController::CancelProviderChange(_In_ UINT providerId, _In_ TELEPHONY_CALLTYPE callType)
{
    HRESULT hr = S_OK;
    KSTELEPHONY_PROVIDERCHANGE providerChange = {};
    CComPtr<IKsControl> spWaveKsControl;

    hr = ObtainKsControl(providerId, &spWaveKsControl);

    if (SUCCEEDED(hr))
    {
        providerChange.CallType = callType;
        providerChange.ProviderChangeOp = TELEPHONY_PROVIDERCHANGEOP_CANCEL;

        hr = HandleProviderChange(spWaveKsControl, providerChange);
    }

    return hr;
}

HRESULT CallController::HandleProviderChange(_In_ IKsControl* pKsControl, _In_ KSTELEPHONY_PROVIDERCHANGE providerChange)
{
    HRESULT hr = S_OK;
    KSPROPERTY providerChangeProp;
    ULONG ulBytesReturned;
 
    if (pKsControl == NULL)
    {
        hr = E_POINTER;
    }

    if (SUCCEEDED(hr))
    {
        providerChangeProp.Set = KSPROPSETID_TelephonyControl;
        providerChangeProp.Id = KSPROPERTY_TELEPHONY_PROVIDERCHANGE;
        providerChangeProp.Flags = KSPROPERTY_TYPE_SET;

        hr = pKsControl->KsProperty(
                (PKSPROPERTY)&providerChangeProp,
                sizeof(providerChangeProp),
                &providerChange,
                sizeof(providerChange),
                &ulBytesReturned
        );
    }

    return hr;
}

// Sets call hold/unhold for a given provider id.
HRESULT CallController::SetCellularHold(_In_ UINT providerId, _In_ BOOL bHold)
{
    HRESULT hr = S_OK;
    CComPtr<IKsControl> spWaveKsControl;
    KSPROPERTY holdProp;
    ULONG ulBytesReturned;
    
    hr = ObtainKsControl(providerId, &spWaveKsControl);

    if (SUCCEEDED(hr))
    {
        holdProp.Set = KSPROPSETID_TelephonyControl;
        holdProp.Id = KSPROPERTY_TELEPHONY_CALLHOLD;
        holdProp.Flags = KSPROPERTY_TYPE_SET;

        hr = spWaveKsControl->KsProperty(
                (PKSPROPERTY)&holdProp,
                sizeof(holdProp),
                &bHold,
                sizeof(bHold),
                &ulBytesReturned
        );
    }

    return hr;
}

// Returns call hold value for a given provider id.
HRESULT CallController::GetCellularHold(_In_ UINT providerId, _Out_ BOOL* pbHold)
{
    HRESULT hr = S_OK;
    CComPtr<IKsControl> spWaveKsControl;
    KSPROPERTY holdProp;
    ULONG ulBytesReturned;
    BOOL bHold = FALSE;
    
    if (pbHold == NULL)
    {
        hr = E_POINTER;
    }

    if (SUCCEEDED(hr))
    {
        hr = ObtainKsControl(providerId, &spWaveKsControl);
    }

    if (SUCCEEDED(hr))
    {
        holdProp.Set = KSPROPSETID_TelephonyControl;
        holdProp.Id = KSPROPERTY_TELEPHONY_CALLHOLD;
        holdProp.Flags = KSPROPERTY_TYPE_GET;

        hr = spWaveKsControl->KsProperty(
                (PKSPROPERTY)&holdProp,
                sizeof(holdProp),
                &bHold,
                sizeof(bHold),
                &ulBytesReturned
        );
    }

    if (SUCCEEDED(hr))
    {
        *pbHold = bHold;
    }

    return hr;
}

// Sets cellular TxMute for a given provider id.
HRESULT CallController::SetCellularTxMute(_In_ UINT providerId, _In_ BOOL bMute)
{
    HRESULT hr = S_OK;
    CComPtr<IKsControl> spWaveKsControl;
    KSPROPERTY txMuteProp;
    ULONG ulBytesReturned;
    
    hr = ObtainKsControl(providerId, &spWaveKsControl);

    if (SUCCEEDED(hr))
    {
        txMuteProp.Set = KSPROPSETID_TelephonyControl;
        txMuteProp.Id = KSPROPERTY_TELEPHONY_MUTE_TX;
        txMuteProp.Flags = KSPROPERTY_TYPE_SET;

        hr = spWaveKsControl->KsProperty(
                (PKSPROPERTY)&txMuteProp,
                sizeof(txMuteProp),
                &bMute,
                sizeof(bMute),
                &ulBytesReturned
        );
    }

    return hr;
}

// Returns cellular TxMute value for a given provider id.
HRESULT CallController::GetCellularTxMute(_In_ UINT providerId, _Out_ BOOL* pbMute)
{
    HRESULT hr = S_OK;
    CComPtr<IKsControl> spWaveKsControl;
    KSPROPERTY txMuteProp;
    ULONG ulBytesReturned;
    BOOL bMute = FALSE;

    if (pbMute == NULL)
    {
        hr = E_POINTER;
    }

    if (SUCCEEDED(hr))
    {
        hr = ObtainKsControl(providerId, &spWaveKsControl);
    }

    if (SUCCEEDED(hr))
    {
        txMuteProp.Set = KSPROPSETID_TelephonyControl;
        txMuteProp.Id = KSPROPERTY_TELEPHONY_MUTE_TX;
        txMuteProp.Flags = KSPROPERTY_TYPE_GET;

        hr = spWaveKsControl->KsProperty(
                (PKSPROPERTY)&txMuteProp,
                sizeof(txMuteProp),
                &bMute,
                sizeof(bMute),
                &ulBytesReturned
        );
    }

    if (SUCCEEDED(hr))
    {
        *pbMute = bMute;
    }

    return hr;
}

// Returns call info for a given provider id.
HRESULT CallController::GetCallInfo(_In_ UINT providerId, _Out_ PKSTELEPHONY_CALLINFO pCallInfo)
{
    HRESULT hr = S_OK;
    KSPROPERTY callInfoProp;
    KSTELEPHONY_CALLINFO callInfo = {};
    ULONG ulBytesReturned;
    CComPtr<IKsControl> spWaveKsControl;

    if (pCallInfo == NULL)
    {
        hr = E_POINTER;
    }

    if (SUCCEEDED(hr))
    {
        hr = ObtainKsControl(providerId, &spWaveKsControl);
    }

    if (SUCCEEDED(hr))
    {
        callInfoProp.Set = KSPROPSETID_TelephonyControl;
        callInfoProp.Id = KSPROPERTY_TELEPHONY_CALLINFO;
        callInfoProp.Flags = KSPROPERTY_TYPE_GET;

        hr = spWaveKsControl->KsProperty(
                (PKSPROPERTY)&callInfoProp,
                sizeof(callInfoProp),
                &callInfo,
                sizeof(callInfo),
                &ulBytesReturned
        );
    }

    if(SUCCEEDED(hr))
    {
        memcpy(pCallInfo, &callInfo, sizeof(KSTELEPHONY_CALLINFO));
    }

    return hr;
}

