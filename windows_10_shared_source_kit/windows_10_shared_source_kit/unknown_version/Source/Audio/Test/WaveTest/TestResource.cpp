// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
// File Name:
//
//  TestResource.cpp
//
// Abstract:
//
//  TAEF Test Resource implementation
//



// -------------------------------------------------------------------------------

#include "PreComp.h"
#include <itestresource.h>
#include "halfdup.h"
#include "TestResource.h"

using namespace WEX::Logging;
using namespace WEX::TestExecution;

CPinTestResource::CPinTestResource():
    m_cRef(1)
{}

CPinTestResource::~CPinTestResource()
{}

HRESULT STDMETHODCALLTYPE
CPinTestResource::CreateInstance
(
    CHalfApp * pHalf,
    REFGUID guid,
    bool fLoopedStreaming,
    REFGUID guidMode,
    ITestResource ** ppOut
)
{
    HRESULT hr;
    CPinTestResource * pTestResource;
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    if (!VERIFY_IS_NOT_NULL(pHalf))
    {
        return E_INVALIDARG;
    }

    if (!VERIFY_IS_NOT_NULL(ppOut))
    {
        return E_POINTER;
    }

    pTestResource = new CPinTestResource();
    if (!VERIFY_IS_NOT_NULL(pTestResource))
    {
        return E_OUTOFMEMORY;
    }

    hr = VERIFY_SUCCEEDED_RETURN(pTestResource->Initialize(
            pHalf, guid, fLoopedStreaming, guidMode));
    if(FAILED(hr))
    {
        pTestResource->Release();
        return hr;
    }

    *ppOut = pTestResource;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE
CPinTestResource::Initialize
(
    CHalfApp * pHalf,
    REFGUID guid,
    bool fLoopedStreaming,
    REFGUID guidMode
)
{
    HRESULT hr = S_OK;
    CHAR szStr[MAX_PATH];
    LPCSTR szMode = ModeNameFromGuid(guidMode);
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    m_guid = guid;
    m_spHalfApp.Attach(pHalf);

    if(m_spHalfApp->m_flow == capture)
    {
        m_szType.Attach(W2BSTR(L"Capture"));
        if(!VERIFY_IS_NOT_NULL(m_szType)){return E_OUTOFMEMORY;}
    }
    else
    {
        m_szType.Attach(W2BSTR(L"Render"));
        if(!VERIFY_IS_NOT_NULL(m_szType)){return E_OUTOFMEMORY;}
    }

    if(fLoopedStreaming)
    {
        m_szStreaming.Attach(W2BSTR(L"Standard Looped"));
        if(!VERIFY_IS_NOT_NULL(m_szStreaming)){return E_OUTOFMEMORY;}
    }
    else
    {
        m_szStreaming.Attach(W2BSTR(L"Standard"));
        if(!VERIFY_IS_NOT_NULL(m_szStreaming)){return E_OUTOFMEMORY;}
    }

    // TODO:
    // possibly make this a combination of filter hw id, instance id, pin #
    hr = VERIFY_SUCCEEDED_RETURN(StringCchPrintfA(
        szStr, MAX_PATH, "%s#%d#%s",
        m_spHalfApp->m_pFilter->m_szFriendlyName,
        m_spHalfApp->m_pPin->m_nId,
        szMode));
    if(FAILED(hr))
    {
        return hr;
    }

    m_szId.Attach(A2WBSTR(szStr));

    if(!VERIFY_IS_NOT_NULL(m_szId)){return E_OUTOFMEMORY;}

    hr = VERIFY_SUCCEEDED_RETURN(StringCchPrintfA(
        szStr, MAX_PATH, " Pin # %d on %s (%s)",
        m_spHalfApp->m_pPin->m_nId,
        m_spHalfApp->m_pFilter->m_szFriendlyName,
        szMode));
    if(FAILED(hr))
    {
        return hr;
    }

    m_szName = m_szType;
    hr = VERIFY_SUCCEEDED_RETURN(m_szName.Append(szStr));
    if(FAILED(hr))
    {
        return hr;
    }

    hr = VERIFY_SUCCEEDED_RETURN(StringCchPrintfA(
        szStr, MAX_PATH, "%u",
        m_spHalfApp->m_pPin->m_nId));
    if(FAILED(hr))
    {
        return hr;
    }

    m_szPinId.Attach(A2WBSTR(szStr));
    if(!VERIFY_IS_NOT_NULL(m_szPinId)){return E_OUTOFMEMORY;}

    return hr;
}

STDMETHODIMP_(ULONG)
CPinTestResource::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG)
CPinTestResource::Release()
{
    ULONG cRef = InterlockedDecrement(&m_cRef);
    if (0 == cRef)
    {
        delete this;
    }

    return cRef;
}

STDMETHODIMP
CPinTestResource::QueryInterface(
    __in REFIID riid,
    __deref_out void ** ppvObject
    )
{
    HRESULT hr = S_OK;

    //IF_FALSE_EXIT_HR( (NULL != ppvObject), E_POINTER);

    if (IsEqualIID(riid, __uuidof(IUnknown)))
    {
        *ppvObject = static_cast<void *>(this);
    }
    else if (IsEqualIID(riid, __uuidof(ITestResource)))
    {
        *ppvObject = static_cast<ITestResource *>(this);
    }
    else if (IsEqualIID(riid, __uuidof(IHalfAppContainer)))
    {
        *ppvObject = static_cast<IHalfAppContainer *>(this);
    }
    else
    {
        *ppvObject = NULL;
        hr = E_NOINTERFACE;
        goto Exit;
    }

    AddRef();

Exit:
    return hr;
}

STDMETHODIMP
CPinTestResource::GetGuid(GUID* pGuid) {
    // don't want to see VERIFY spew
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    if (!VERIFY_IS_NOT_NULL(pGuid)) { return E_POINTER; }
    
    *pGuid = m_guid;
    return S_OK;
}

STDMETHODIMP
CPinTestResource::SetGuid(GUID guid) {
    m_guid = guid;
    return S_OK;
}

STDMETHODIMP
CPinTestResource::GetValue(BSTR name, BSTR* pValue) {
    // don't want to see VERIFY spew
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    if (!VERIFY_IS_NOT_NULL(name)) { return E_POINTER; }
    if (!VERIFY_IS_NOT_NULL(pValue)) { return E_POINTER; }

    if (0 == wcscmp(name, TestResourceProperty::c_szName)) {
        if (!VERIFY_IS_NOT_NULL(*pValue = SysAllocString(m_szName))) { return E_OUTOFMEMORY; }
        return S_OK;
    }

    if (0 == wcscmp(name, TestResourceProperty::c_szId)) {
        if (!VERIFY_IS_NOT_NULL(*pValue = SysAllocString(m_szId))) { return E_OUTOFMEMORY; }
        return S_OK;
    }

    if (0 == wcscmp(name, TestResourceProperty::c_szType)) {
        if (!VERIFY_IS_NOT_NULL(*pValue = SysAllocString(m_szType))) { return E_OUTOFMEMORY; }
        return S_OK;
    }

    if (0 == wcscmp(name, TestResourceProperty::c_szStreaming)) {
        if (!VERIFY_IS_NOT_NULL(*pValue = SysAllocString(m_szStreaming))) { return E_OUTOFMEMORY; }
        return S_OK;
    }

    if (0 == wcscmp(name, TestResourceProperty::c_szPinId)) {
        if (!VERIFY_IS_NOT_NULL(*pValue = SysAllocString(m_szPinId))) { return E_OUTOFMEMORY; }
        return S_OK;
    }

    Log::Error(L"CDevice::GetValue name not found");
    return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
}

STDMETHODIMP
CPinTestResource::SetValue(BSTR name, BSTR value) {
    // don't want to see VERIFY spew
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    if (!VERIFY_IS_NOT_NULL(name)) { return E_POINTER; }
    if (!VERIFY_IS_NOT_NULL(value)) { return E_POINTER; }

    if (0 == wcscmp(name, TestResourceProperty::c_szName)) {
        return m_szName.AssignBSTR(value);
    }

    if (0 == wcscmp(name, TestResourceProperty::c_szId)) {
        return m_szId.AssignBSTR(value);
    }
    
    if (0 == wcscmp(name, TestResourceProperty::c_szType)) {
        return m_szType.AssignBSTR(value);
    }

    if (0 == wcscmp(name, TestResourceProperty::c_szStreaming)) {
        return m_szStreaming.AssignBSTR(value);
    }

    if (0 == wcscmp(name, TestResourceProperty::c_szPinId)) {
        return m_szPinId.AssignBSTR(value);
    }

    Log::Error(L"CDevice::SetValue name not found");
    return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
}

STDMETHODIMP
CPinTestResource::GetHalfApp(CHalfApp ** ppHalfApp)
{
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    if (!VERIFY_IS_NOT_NULL(ppHalfApp)) { return E_POINTER; }

    *ppHalfApp = m_spHalfApp;
    return S_OK;
}

struct MAP
{
    GUID    Guid;
    LPCSTR  Name;
};
#define MAP_ENTRY(x) {AUDIO_SIGNALPROCESSINGMODE_##x, #x}
LPCSTR CPinTestResource::ModeNameFromGuid(REFGUID guidMode)
{

    MAP modes[] = {
        { GUID_NULL, "NO_MODE" },
        MAP_ENTRY(RAW),
        MAP_ENTRY(DEFAULT),
        MAP_ENTRY(COMMUNICATIONS),
        MAP_ENTRY(SPEECH),
        MAP_ENTRY(NOTIFICATION),
        MAP_ENTRY(MEDIA),
        MAP_ENTRY(MOVIE)
    };

    for (int i = 0; i < _countof(modes); ++i)
    {
        if (IsEqualGUID(modes[i].Guid, guidMode))
        {
            return modes[i].Name;
        }
    }

    return "UnknownMode";
}
