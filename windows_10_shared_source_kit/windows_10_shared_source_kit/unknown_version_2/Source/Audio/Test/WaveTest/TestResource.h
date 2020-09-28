// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
// File Name:
//
//  TestResource.h
//
// Abstract:
//
//  TAEF Test Resource
//



// -------------------------------------------------------------------------------

#include "WaveTestTaef.h"

class CPinTestResource:
    public WEX::TestExecution::ITestResource,
    public IHalfAppContainer
{
public:
    //IUnknown
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();
    STDMETHODIMP QueryInterface(
        REFIID riid,
        __out VOID **ppvObject
        );
    // ITestResouce
    STDMETHODIMP GetGuid(GUID* pGuid);
    STDMETHODIMP SetGuid(GUID guid);
    STDMETHODIMP GetValue(BSTR name, BSTR* pValue);
    STDMETHODIMP SetValue(BSTR name, BSTR value);

    //IHalfAppContainer
    STDMETHODIMP GetHalfApp(CHalfApp ** ppHalfApp);

    static HRESULT STDMETHODCALLTYPE CreateInstance(
        CHalfApp * pHalf,
        REFGUID guid,
        bool fLoopedStreaming,
        REFGUID guidMode,
        WEX::TestExecution::ITestResource ** ppOut
        );

private:

    CPinTestResource();
    ~CPinTestResource();

    HRESULT STDMETHODCALLTYPE Initialize(
        CHalfApp * pHalf,
        REFGUID guid,
        bool fLoopedStreaming,
        REFGUID guidMode
        );

    LPCSTR ModeNameFromGuid(REFGUID guidMode);

    CComBSTR m_szName;
    CComBSTR m_szId;
    CComBSTR m_szType;
    CComBSTR m_szStreaming;
    CComBSTR m_szPinId;

    CAutoPtr<CHalfApp> m_spHalfApp;
    GUID m_guid;
    ULONG m_cRef;
    GUID m_guidMode;
};
