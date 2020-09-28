// Copyright(C) Microsoft. All rights reserved.

//--------------------------------------------------------------------------
//
//  File: DummyDuckingClient.h
//
//  Abstract: Dummy IAudioDuckingEvent implementation for eventing testing
//
//  Notes:
//
//--------------------------------------------------------------------------

#pragma once

class CDummyDuckingNotificationClient: public IAudioVolumeDuckNotification
{
private:    
    ULONG m_cRef;
    ULONG m_cEvents;
public:

    CDummyDuckingNotificationClient() { m_cRef = 1; m_cEvents = 0; }
    ~CDummyDuckingNotificationClient(){};

    void ResetEvents() { m_cEvents = 0; }
    ULONG EventCount() { return m_cEvents; }

    //
    // IMMNotificationClient Implementation
    //

    STDMETHOD(OnVolumeDuckNotification)(LPCWSTR, UINT32) 
    { 
        SLOG(eInfo1, "DUCKING NOTIFICATION: OnVolumeDuckNotification");
        InterlockedIncrement(&m_cEvents);
        return S_OK; 
    }

    STDMETHOD(OnVolumeUnduckNotification)(LPCWSTR)
    { 
        SLOG(eInfo1, "DUCKING NOTIFICATION: OnVolumeUnduckNotification");
        InterlockedIncrement(&m_cEvents);
        return S_OK; 
    }


    //
    // IUnknownImplementation
    //
    STDMETHOD(QueryInterface)(REFIID iid, VOID** ppvObject)
    {
        if(NULL == ppvObject)
        {
            return E_POINTER;
        }

        *ppvObject = NULL;

        if((__uuidof(IUnknown) == iid)||
           (__uuidof(IAudioVolumeDuckNotification) == iid))
        {
            *ppvObject = (IAudioVolumeDuckNotification*)(this);
            AddRef();
            return S_OK;
        }
        else
        {
            return E_NOINTERFACE;
        }
    }

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return InterlockedIncrement(&m_cRef);
    }

    ULONG STDMETHODCALLTYPE Release()
    {
        ULONG cTemp = InterlockedDecrement(&m_cRef);

        if(0 == cTemp)
        {
            delete this;
        }

        return cTemp;
    }
};
