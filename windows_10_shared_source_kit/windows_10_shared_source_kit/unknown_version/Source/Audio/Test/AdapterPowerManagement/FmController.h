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

class FmController
{

public:
    HRESULT Initialize(IMMDevice *pMMDevice);
    HRESULT SetFmVolume(_In_ LONG volume);
    HRESULT GetFmVolume(_Out_ LONG * plVolume);
    HRESULT SetFmEndpointId(_In_ LONG index);
    HRESULT GetFmEndpointId(_Out_ PKSTOPOLOGY_ENDPOINTID pEndpointId);
    HRESULT GetFmAntennaEndpointId(_Out_ PKSTOPOLOGY_ENDPOINTID pEndpointId);
    HRESULT SetFmState(_In_ BOOL state);
    HRESULT GetFmState(_Out_ BOOL * pbState);

private:
    CComPtr<IKsControl>     m_spWaveKsControl;      // KsControl for wave filter
    CComPtr<IKsControl>     m_spTopologyKsControl;  // KsControl for topology filter
};
