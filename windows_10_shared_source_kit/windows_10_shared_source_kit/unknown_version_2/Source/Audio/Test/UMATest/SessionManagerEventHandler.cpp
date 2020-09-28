// Copyright(C) Microsoft. All rights reserved.

//------------------------------------------------------------------------------
// File: SessionManagerEventHandler.cpp
//
// Desc: AudioSession Arrival Event Handler Implementation
//------------------------------------------------------------------------------

#pragma once

#include "stdafx.h"


#include "SessionManagerEventHandler.h"

// default constructor
CSessionManagerEventHandler::CSessionManagerEventHandler()
                                :m_cRef(1),
                                 m_bErrorReported(FALSE)
{
    return;
}

// class destructor
CSessionManagerEventHandler::~CSessionManagerEventHandler() 
{
    return;
}

// Get a cached audiosession by index
IAudioSessionControl* CSessionManagerEventHandler::GetAudioSession(DWORD SessionIndex) 
{
    m_SessionListLock.Enter();
    if (SessionIndex < m_SessionList.GetCount())
    {
        CComPtr<IAudioSessionControl> session = m_SessionList[SessionIndex];
        m_SessionListLock.Leave();
        return session;
    }

    m_SessionListLock.Leave();
    return NULL;
}

// Get a cached audiosession by process ID (pid)

IAudioSessionControl *CSessionManagerEventHandler::GetAudioSessionByPID(DWORD dwPID)
{
    HRESULT hr = S_OK;
    DWORD dwProcess;
    
    m_SessionListLock.Enter();
    for (UINT32 i = 0 ; i < m_SessionList.GetCount() ; i++)
    {
        LPWSTR sessionId = NULL;
        CComPtr<IAudioSessionControl> session = m_SessionList[i];
        CComPtr<IAudioSessionControl2> sessionEx;

        sessionEx = session;

        METHODCHECKRESULT("IAudioSessionControl::GetProcess",
                          hr,
                          sessionEx->GetProcessId(&dwProcess));                          
        if(FAILED(hr))
        {
            _FlagError();
            if(NULL != sessionId) { ::CoTaskMemFree(sessionId); }
            m_SessionListLock.Leave();
            return NULL;
        }        

        SLOG(eFatalError, "arrived: %d, created: %d", dwProcess, dwPID);
                
        if (dwProcess == dwPID)
        {  
            SLOG(eFatalError, "FOUND!");
            CoTaskMemFree(sessionId);
            m_SessionListLock.Leave();
            return session;
        }
        
        CoTaskMemFree(sessionId);
    }

    m_SessionListLock.Leave();
    return NULL;
}

// get a cached audiosession by sessionid
IAudioSessionControl* CSessionManagerEventHandler::GetAudioSession(LPCWSTR SessionId) 
{
    HRESULT hr;

    if(NULL == SessionId)
    {
        return NULL;
    }

    m_SessionListLock.Enter();
    for (UINT32 i = 0 ; i < m_SessionList.GetCount() ; i += 1)
    {
        CComPtr<IAudioSessionControl2> spAudSesCtrlEx = NULL;
        LPWSTR sessionId = NULL;        
        
        CComPtr<IAudioSessionControl> session = m_SessionList[i];

        METHODCHECKRESULT("IAudioSessionControl::QueryInterface",
                          hr,
                          session->QueryInterface(__uuidof(IAudioSessionControl2),
                          (void**)&spAudSesCtrlEx));

        if(FAILED(hr)) { return NULL; }

        METHODCHECKRESULT("IAudioSessionControl2::GetSessionInstanceIdentifier",
                          hr,
                          spAudSesCtrlEx->GetSessionInstanceIdentifier(&sessionId));                          
        if(FAILED(hr))
        {
            _FlagError();
            SAFE_COTASKMEMFREE(sessionId);
            return NULL;
        }

        SLOG(eBlab1, "string1: %ws", sessionId);
        SLOG(eBlab1, "string2: %ws", SessionId);
        
        if (0 == wcscmp(sessionId, SessionId))
        {  
            SAFE_COTASKMEMFREE(sessionId);
            m_SessionListLock.Leave();
            return session;
        }
        
        SAFE_COTASKMEMFREE(sessionId);
    }

    m_SessionListLock.Leave();
    return NULL;
}
   

// Indicate that an error has occurred within the object
void CSessionManagerEventHandler::_FlagError()
{
    m_ErrorLock.Enter();
        m_bErrorReported = TRUE;
    m_ErrorLock.Leave();

}

// IAudioSessionNotification AudioSession arrival event handler
// Cache new audiosession control for later use
STDMETHODIMP CSessionManagerEventHandler::OnSessionCreated(IAudioSessionControl *NewSession)
{   
    LPWSTR newSessionId = NULL;
    HRESULT hr = S_OK;
    CComPtr<IAudioSessionControl2> spAudSesCtrlEx = NULL;

    METHODCHECKRESULT("IAudioSessionControl::QueryInterface",
                      hr,
                      NewSession->QueryInterface(__uuidof(IAudioSessionControl2),
                         (void**)&spAudSesCtrlEx));
    if(FAILED(hr))
    {
        _FlagError();
        return hr;
    }

    
    METHODCHECKRESULT("IAudioSessionControl2::GetSessionInstanceIdentifier",
                      hr,
                      spAudSesCtrlEx->GetSessionInstanceIdentifier(&newSessionId));
    if(FAILED(hr)) 
    { 
        _FlagError();
        return hr; 
    }    

    SLOG(eFatalError, "AudioSession %ls has arrived!", newSessionId);

    // add the new audiosession
    m_SessionListLock.Enter();
        m_SessionList.Add(NewSession);
    m_SessionListLock.Leave();

    SAFE_COTASKMEMFREE(newSessionId);
    return hr;
}

BOOL CSessionManagerEventHandler::ErrorReported() 
{ 
    BOOL bRes = FALSE;
    
    m_ErrorLock.Enter();
        bRes = m_bErrorReported;
    m_ErrorLock.Leave();

    return bRes;
}

UINT32 CSessionManagerEventHandler::GetSessionCount() 
{ 
    return (UINT32)(m_SessionList.GetCount()); 
}

HRESULT CSessionManagerEventHandler::QueryInterface(REFIID iid, void **ppv)
{
    if (IID_IUnknown == iid)
    {
        this->AddRef();
        *ppv = (IUnknown *)(IAudioSessionNotification *)this;
    }
    else if (__uuidof(IAudioSessionNotification) == iid)
    {
        this->AddRef();
        *ppv = (IAudioSessionNotification *)this;
    }
    else
    {
        *ppv = NULL;
        return (E_NOINTERFACE);
    }

    return (S_OK);
}

ULONG CSessionManagerEventHandler::AddRef(void)
{
    return InterlockedIncrement(&m_cRef);
}

ULONG CSessionManagerEventHandler::Release(void)
{
    long RefCnt = InterlockedDecrement((long*)&m_cRef);
    if(RefCnt == 0)
    {
        delete this;
        return (0);
    }
    return (RefCnt);
}
