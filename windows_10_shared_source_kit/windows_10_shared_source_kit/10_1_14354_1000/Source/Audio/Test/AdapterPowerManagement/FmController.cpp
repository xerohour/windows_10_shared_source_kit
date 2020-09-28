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

#include "common.h"
#include "FmController.h"

HRESULT FmController::Initialize(IMMDevice *pMMDevice)
{
    HRESULT hr = S_OK;
    CComPtr<IMMDeviceEnumerator> spEnumerator;

    hr = spEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER);

    if (SUCCEEDED(hr))
    {
        CComPtr<IMMDevice>       spDevice;
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
      
        // Get the IDeviceTopology interface from the endpoint device.
        if (SUCCEEDED(hr))
        {
            hr = pMMDevice->Activate(
                  __uuidof(IDeviceTopology),
                  CLSCTX_ALL,
                  NULL,
                  (VOID**)&spEndpointTopology);
        }

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

        // If this is the FM_RX node we've found the FM topology
        if (SUCCEEDED(hr))
        {
            if (guidEndpointClass != KSNODETYPE_FM_RX)
            {
                return E_NOTFOUND;
            }
        }

        // Get the IDeviceTopology interface for the adapter device from the endpoint connector part.
        if (SUCCEEDED(hr))
        {
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
            m_spWaveKsControl = spWaveKsControl;
        }
    }

    return hr;
}

// Sets fm volume. This is a property on topology filter.
HRESULT FmController::SetFmVolume(_In_ LONG volume)
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

        volumeSupportProp.Set = KSPROPSETID_FMRXTopology;
        volumeSupportProp.Id = KSPROPERTY_FMRX_VOLUME;
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
        volumeProp.Set = KSPROPSETID_FMRXTopology;
        volumeProp.Id = KSPROPERTY_FMRX_VOLUME;
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

// Returns current fm volume. This is a property on topology filter.
HRESULT FmController::GetFmVolume(_Out_ LONG* plVolume)
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
        volumeProp.Set = KSPROPSETID_FMRXTopology;
        volumeProp.Id = KSPROPERTY_FMRX_VOLUME;
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

// Sets fm endpoint id. Takes index
HRESULT FmController::SetFmEndpointId(_In_ LONG index)
{
    HRESULT hr = S_OK;
    KSPROPERTY endpointIdProp;
    ULONG ulBytesReturned;
    KSPROPERTY_DESCRIPTION desc = { 0 };
    KSPROPERTY_DESCRIPTION * pdesc = NULL;
    PKSPROPERTY_MEMBERSHEADER pKsPropMembHead = NULL;
    PKSTOPOLOGY_ENDPOINTID pKsPropEndpointId= NULL;
    ULONG endpointCount = 0;

    if (m_spTopologyKsControl == NULL)
    {
        hr = E_POINTER;
    }

    // Determine valid endpoints
    if (SUCCEEDED(hr))
    {
        KSPROPERTY endpointIdSupportProp;
        ULONG cbSupportProp = 0;

        endpointIdSupportProp.Set = KSPROPSETID_FMRXTopology;
        endpointIdSupportProp.Id = KSPROPERTY_FMRX_ENDPOINTID;
        endpointIdSupportProp.Flags = KSPROPERTY_TYPE_BASICSUPPORT;

        hr = m_spTopologyKsControl->KsProperty(
            (PKSPROPERTY)&endpointIdSupportProp,
            sizeof(endpointIdSupportProp),
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
                    (PKSPROPERTY)&endpointIdSupportProp,
                    sizeof(endpointIdSupportProp),
                    pdesc,
                    desc.DescriptionSize,
                    &cbSupportProp
                    );
            }

            if (SUCCEEDED(hr))
            {
                if (pdesc->PropTypeSet.Set != KSPROPSETID_FMRXTopology ||
                    pdesc->PropTypeSet.Id != KSPROPERTY_FMRX_ENDPOINTID ||
                    pdesc->PropTypeSet.Flags != 0 ||
                    pdesc->MembersListCount < 1 ||
                    pdesc->Reserved != 0 ||
                    pdesc->DescriptionSize < (sizeof(KSPROPERTY_DESCRIPTION)+sizeof(KSPROPERTY_MEMBERSHEADER)+sizeof(KSTOPOLOGY_ENDPOINTID)))
                {
                    hr = E_INVALIDARG;
                }
            }

            if (SUCCEEDED(hr))
            {
                pKsPropMembHead = reinterpret_cast<PKSPROPERTY_MEMBERSHEADER>(pdesc + 1);
                if (pKsPropMembHead->MembersFlags != KSPROPERTY_MEMBER_VALUES||
                    pKsPropMembHead->MembersSize < sizeof(KSTOPOLOGY_ENDPOINTID) ||
                    pKsPropMembHead->MembersCount < 1 ||
                    pKsPropMembHead->Flags != 0)
                {
                    hr = E_INVALIDARG;
                }
                endpointCount = pKsPropMembHead->MembersCount;
            }
        }
    }

    if ((ULONG)(index) >= endpointCount)
    {
        hr = E_INVALIDARG;
    }

    if (SUCCEEDED(hr))
    {
        endpointIdProp.Set = KSPROPSETID_FMRXTopology;
        endpointIdProp.Id = KSPROPERTY_FMRX_ENDPOINTID;
        endpointIdProp.Flags = KSPROPERTY_TYPE_SET;

        hr = m_spTopologyKsControl->KsProperty(
            (PKSPROPERTY)&endpointIdProp,
            sizeof(endpointIdProp),
            &pKsPropEndpointId[index],
            sizeof(KSTOPOLOGY_ENDPOINTID),
            &ulBytesReturned
            );
    }

    if (pdesc)
    {
        CoTaskMemFree(pdesc);
    }
    return hr;
}

HRESULT FmController::GetFmEndpointId(_Out_ PKSTOPOLOGY_ENDPOINTID pEndpointId)
{
    HRESULT hr = S_OK;
    KSPROPERTY endpointIdProp;
    ULONG ulBytesReturned;
    KSTOPOLOGY_ENDPOINTID ksEndpointId = { 0 };


    if (pEndpointId == NULL || m_spTopologyKsControl == NULL)
    {
        hr = E_POINTER;
    }

    if (SUCCEEDED(hr))
    {
        endpointIdProp.Set = KSPROPSETID_FMRXTopology;
        endpointIdProp.Id = KSPROPERTY_FMRX_ENDPOINTID;
        endpointIdProp.Flags = KSPROPERTY_TYPE_GET;

        hr = m_spTopologyKsControl->KsProperty(
            (PKSPROPERTY)&endpointIdProp,
            sizeof(endpointIdProp),
            &ksEndpointId,
            sizeof(KSTOPOLOGY_ENDPOINTID),
            &ulBytesReturned
            );
    }

    if (SUCCEEDED(hr))
    {
        memcpy(pEndpointId, &ksEndpointId, sizeof(*pEndpointId));
    }

    return hr;
}

HRESULT FmController::GetFmAntennaEndpointId(_Out_ PKSTOPOLOGY_ENDPOINTID pEndpointId)
{
    HRESULT hr = S_OK;
    KSPROPERTY endpointIdProp;
    ULONG ulBytesReturned;
    KSTOPOLOGY_ENDPOINTID ksEndpointId = { 0 };


    if (pEndpointId == NULL || m_spTopologyKsControl == NULL)
    {
        hr = E_POINTER;
    }

    if (SUCCEEDED(hr))
    {
        endpointIdProp.Set = KSPROPSETID_FMRXTopology;
        endpointIdProp.Id = KSPROPERTY_FMRX_ANTENNAENDPOINTID;
        endpointIdProp.Flags = KSPROPERTY_TYPE_GET;

        hr = m_spTopologyKsControl->KsProperty(
            (PKSPROPERTY)&endpointIdProp,
            sizeof(endpointIdProp),
            &ksEndpointId,
            sizeof(KSTOPOLOGY_ENDPOINTID),
            &ulBytesReturned
            );
    }

    if (SUCCEEDED(hr))
    {
        memcpy(pEndpointId, &ksEndpointId, sizeof(*pEndpointId));
    }

    return hr;
}

HRESULT FmController::SetFmState(_In_ BOOL state)
{
    HRESULT hr = S_OK;
    KSPROPERTY stateProp;
    ULONG ulBytesReturned;

    if (m_spTopologyKsControl == NULL)
    {
        hr = E_POINTER;
    }

    if (SUCCEEDED(hr))
    {
        stateProp.Set = KSPROPSETID_FMRXControl;
        stateProp.Id = KSPROPERTY_FMRX_STATE;
        stateProp.Flags = KSPROPERTY_TYPE_SET;

        hr = m_spWaveKsControl->KsProperty(
            (PKSPROPERTY)&stateProp,
            sizeof(stateProp),
            &state,
            sizeof(BOOL),
            &ulBytesReturned
            );
    }

    return hr;
}
HRESULT FmController::GetFmState(_Out_ BOOL * pbState)
{
    HRESULT hr = S_OK;
    KSPROPERTY stateProp;
    ULONG ulBytesReturned;
    BOOL state = 0;

    if (pbState== NULL || m_spWaveKsControl == NULL)
    {
        hr = E_POINTER;
    }

    if (SUCCEEDED(hr))
    {
        stateProp.Set = KSPROPSETID_FMRXControl;
        stateProp.Id = KSPROPERTY_FMRX_STATE;
        stateProp.Flags = KSPROPERTY_TYPE_GET;

        hr = m_spWaveKsControl->KsProperty(
            (PKSPROPERTY)&stateProp,
            sizeof(stateProp),
            &state,
            sizeof(BOOL),
            &ulBytesReturned
            );
    }

    if (SUCCEEDED(hr))
    {
        *pbState = state;
    }

    return hr;
}
