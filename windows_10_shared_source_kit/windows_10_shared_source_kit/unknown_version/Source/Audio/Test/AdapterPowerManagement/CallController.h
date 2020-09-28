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
#pragma once

#include "common.h"

// From sysvad.h
#define VALUE_NORMALIZE_P(v, step) \
    ((((v) + (step)/2) / (step)) * (step))

#define VALUE_NORMALIZE(v, step) \
    ((v) > 0 ? VALUE_NORMALIZE_P((v), (step)) : -(VALUE_NORMALIZE_P(-(v), (step))))
    
#define VALUE_NORMALIZE_IN_RANGE_EX(v, min, max, step) \
    ((v) > (max) ? (max) : \
     (v) < (min) ? (min) : \
     VALUE_NORMALIZE((v), (step)))

#define MAX_PROVIDERIDS 4

class CallController
{

public:
    HRESULT Initialize(IMMDevice *pMMDevice);
    HRESULT RetrieveProviderId(_Inout_ UINT ProviderId[], _Out_ UINT* pCount);
    HRESULT GetCallInfo(_In_ UINT providerId,  _Out_ PKSTELEPHONY_CALLINFO pCallInfo);
    HRESULT EnableCellular(_In_ UINT providerId, _In_ TELEPHONY_CALLTYPE callType);
    HRESULT DisableCellular(_In_ UINT providerId, _In_ TELEPHONY_CALLTYPE callType);
    HRESULT BeginProviderChange(_In_ UINT providerId, _In_ TELEPHONY_CALLTYPE callType);
    HRESULT EndProviderChange(_In_ UINT providerId, _In_ TELEPHONY_CALLTYPE callType);
    HRESULT CancelProviderChange(_In_ UINT providerId, _In_ TELEPHONY_CALLTYPE callType);
    HRESULT SetCellularHold(_In_ UINT providerId, _In_ BOOL bHold);
    HRESULT GetCellularHold(_In_ UINT providerId, _Out_ BOOL* pbHold);
    HRESULT SetCellularTxMute(_In_ UINT providerId, _In_ BOOL bMute);
    HRESULT GetCellularTxMute(_In_ UINT providerId, _Out_ BOOL* pbMute);
    HRESULT SetCellularVolume(_In_ LONG volume);
    HRESULT GetCellularVolume(_Out_ LONG *plVolume);
    HRESULT GetCellularRouting(_Out_ PKSTOPOLOGY_ENDPOINTIDPAIR pRoutingInfo);
    HRESULT SetCellularRouting(_In_ KSTOPOLOGY_ENDPOINTIDPAIR RoutingInfo);

private:
    CAtlList<UINT>                        m_ValidTelephonyProviderIds;  // List of all the valid provider Ids supported by the driver
    CAtlMap<UINT, CComPtr<IKsControl>>    m_mapProviderWaveKsControl;   // Map of provider id and KsControl for its associated wave filter
    CComPtr<IKsControl>                   m_spTopologyKsControl;        // KsControl for topology filter

    HRESULT HandleCallControl(_In_ IKsControl *pKsControl, _In_ KSTELEPHONY_CALLCONTROL callControl);
    HRESULT HandleProviderChange(_In_ IKsControl *pKsControl, _In_ KSTELEPHONY_PROVIDERCHANGE providerChange);
    HRESULT GetProviderId(_In_ IKsControl *pKsControl, _Out_ UINT* pProviderId);
    HRESULT ObtainKsControl(_In_ UINT providerId, _Outptr_ IKsControl** ppKsControl);
};

