// Copyright (C) Microsoft. All rights reserved.
#pragma once

#include "buttonstate.h"

enum EHidControl
{
    EHidControl_Mute,
    EHidControl_VolUp,
    EHidControl_VolDown,
    EHidControl_Undefined
};

struct __declspec(uuid("AE14A29E-F31E-4D4D-AB25-65F782DC0EA2")) IAudioHidPerfTestClient abstract : public IUnknown 
{
    virtual HRESULT RegisterNotification() = 0;
    virtual HRESULT UnregisterNotification() = 0;
    virtual HRESULT Test(EHidControl hidControl) = 0;
};

HRESULT CreateAudioHidPerfTestClient
(
    IBasicLog *pLog, 
    CLatencyDataset *pStartSet, 
    CLatencyDataset *pEndSet, 
    IAudioHidPerfTestClient **ppClient
);
