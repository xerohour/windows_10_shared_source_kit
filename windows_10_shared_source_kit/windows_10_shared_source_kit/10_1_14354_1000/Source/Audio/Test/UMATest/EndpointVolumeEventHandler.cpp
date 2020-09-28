// Copyright(C) Microsoft. All rights reserved.

//------------------------------------------------------------------------------
// File: AudioSessionEventHandler.cpp
//
// Desc: AudioSessionEvent Test Handler Implementation
//------------------------------------------------------------------------------

#pragma once

#include <stdafx.h>

#include <UMATestStreaming.h>

#include "TestUtilities.h"
#include "EndpointVolumeEventHandler.h"


// {D4178DBB-389E-4ea0-9986-03AC6D672830}
extern const GUID IID_IAudioEndpointVolumeCallback = 
{ 0xd4178dbb, 0x389e, 0x4ea0, { 0x99, 0x86, 0x3, 0xac, 0x6d, 0x67, 0x28, 0x30 } };

HRESULT CEndpointVolumeEventHandler::QueryInterface
(
    REFIID          iid,
    void            **ppv
)
{
    if (IID_IAudioEndpointVolumeCallback == iid)
    {
        this->AddRef();
        *ppv = (IAudioEndpointVolumeCallback *)this;
    }
    else
    {
        *ppv = NULL;
        return (E_NOINTERFACE);
    }

    return (S_OK);
}

STDMETHODIMP CEndpointVolumeEventHandler::OnNotify(AUDIO_VOLUME_NOTIFICATION_DATA* pData)
{      
    HRESULT hr = S_OK;
 
    SLOG(eInfo1, "##############################");
    SLOG(eInfo1, "Endpoint Volume Event Received");
    LogGUID(pData->guidEventContext);
    SLOG(eInfo1, "##############################");
    
    // store the data, and signal that an event was received
    _EventDataLock.Enter();
        // filter the event if a filter guid is set and the incoming event doesn't match.
        if((NULL != m_pguidEventFilter)&&
           (!IsEqualGUID(pData->guidEventContext, *m_pguidEventFilter)))
        {
            SLOG(eInfo1, "<INCOMINT EVENT FILTERED>");
            hr = S_FALSE;
        }
        else
        {
            CopyMemory(&m_EventData,pData,sizeof(AUDIO_VOLUME_NOTIFICATION_DATA));
            bEventReceived = TRUE;
        }
    _EventDataLock.Leave();
    
    return S_OK;
}
