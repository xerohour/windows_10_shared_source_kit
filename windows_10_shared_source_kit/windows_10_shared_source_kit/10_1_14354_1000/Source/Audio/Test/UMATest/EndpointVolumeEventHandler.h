// Copyright(C) Microsoft. All rights reserved.

//------------------------------------------------------------------------------
// File: EndpointVolumeEventHandler.h
//
// Desc: Inteface definition for IAudioEndpointVolume Test Handler
//------------------------------------------------------------------------------

#pragma once

class CEndpointVolumeEventHandler: public IAudioEndpointVolumeCallback
{
    private:       
        long       m_cref;
        CCriticalSection _EventDataLock;

        AUDIO_VOLUME_NOTIFICATION_DATA m_EventData;     // guarded by _EventDataLock
        BOOL bEventReceived;                            // guarded by _EventDataLock

        // event guid to listen for
        // NULL == listen for all events
        LPCGUID m_pguidEventFilter;
         
    public:
        // default constructor
        CEndpointVolumeEventHandler()
        {            
            m_cref = 1;
            m_pguidEventFilter = NULL;
            bEventReceived = FALSE;            
        }

        // set the event context filter and reset the event received flag (thread safe)
        void SetEventContextFilterAndReset(LPCGUID pguidEventFilter)
        {
            _EventDataLock.Enter();
                m_pguidEventFilter = pguidEventFilter;
                bEventReceived = FALSE;
            _EventDataLock.Leave();
        }

        // set the event context filter without resetting the event received flag (thread safe)
        void SetEventContextFilterNoReset(LPCGUID pguidEventFilter)
        {
             m_pguidEventFilter = pguidEventFilter;
        }


        // reset the event-received flag (thread safe)
        void Reset()
        {
            bEventReceived = FALSE;
        }

        // check to see if an event has been receieved since the last reset
        // (thread safe)
        BOOL CheckEventReceived()
        {
            return bEventReceived;
        }

        // Get the data associated with the last VolumeChanged event
        // Note: This data is not valid if CheckEventReceived returns
        // false
        AUDIO_VOLUME_NOTIFICATION_DATA GetLastVolumeData()
        {
            AUDIO_VOLUME_NOTIFICATION_DATA copy;
            _EventDataLock.Enter();
                if (bEventReceived)
                {
                    copy = m_EventData;
                }
                else
                {
                    ZeroMemory(&copy, sizeof(AUDIO_VOLUME_NOTIFICATION_DATA));
                }                   
            _EventDataLock.Leave();
            return copy;
        }
     
        // IAudioClientPlayerCallback methods
        STDMETHOD(OnNotify)(AUDIO_VOLUME_NOTIFICATION_DATA* pData);


        // IUnknown Methods
        STDMETHODIMP            QueryInterface(REFIID  iid, void **ppv);

        STDMETHODIMP_(ULONG) AddRef(void)
        {
            return InterlockedIncrement(&m_cref);
        }
        STDMETHODIMP_(ULONG)Release(void)
        {
            LONG lRef = InterlockedDecrement(&m_cref);
            if (lRef == 0)
            {           
                delete this;
            }
            return lRef;
        }
};
