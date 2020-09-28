// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft Corporation, 1996 - 2000.  All rights reserved.
//
// File Name:
//
//  TestResource.h
//
// Abstract:
//
//  TAEF Test Resource
//
// @@BEGIN_MSINTERNAL
//      minkom  02/19/2012 - created
// @@END_MSINTERNAL
// -------------------------------------------------------------------------------


class CPinTestResource:
    public WEX::TestExecution::ITestResource,
    public IHalfAppContainer
{
public:
    //IUnknown
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();
    STDMETHODIMP QueryInterface(
        __in REFIID riid,
        __deref_out void **ppvObject
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
        WEX::TestExecution::ITestResource ** ppOut
        );

private:

    CPinTestResource();
    ~CPinTestResource();

    HRESULT STDMETHODCALLTYPE Initialize(
        CHalfApp * pHalf,
        REFGUID guid,
        bool fLoopedStreaming
        );

    CComBSTR m_szName;
    CComBSTR m_szId;
    CComBSTR m_szType;
    CComBSTR m_szStreaming;
    CComBSTR m_szPinId;

    CAutoPtr<CHalfApp> m_spHalfApp;
    GUID m_guid;
    ULONG m_cRef;
};