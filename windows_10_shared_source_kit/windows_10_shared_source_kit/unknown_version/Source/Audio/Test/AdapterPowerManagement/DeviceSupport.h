//--------------------------------------------------------------------------
//
//  File: DeviceSupport.h
//
// Copyright (C) Microsoft. All rights reserved.
//
//  History:
//      02/14/2012    CaWerner       Initial implementation.
//
//--------------------------------------------------------------------------
#pragma once

static WEX::TestExecution::ResourceList* g_pResourceList;

class AudioDevnode : public ITestResource
{

public:

    AudioDevnode( IBasicLog* pBasicLog, LPCWSTR pszDeviceInstanceId, GUID guidModule, HRESULT& hr );
    ~AudioDevnode();

    HRESULT GetDevicePowerState( DEVICE_POWER_STATE* devicePowerState );
    HRESULT GetConservationIdleTime( ULONG* pSecsConservationIdleTime );
    HRESULT Disable();
    HRESULT Enable();
    HRESULT IsEnabled( bool* pIsEnabled );
    HRESULT IsDisabled( bool* pIsDisabled );
    HRESULT RestoreDevnodeState();

    // ITestResource methods
    HRESULT STDMETHODCALLTYPE GetGuid(GUID* pGuid);
    HRESULT STDMETHODCALLTYPE SetGuid(GUID guid);
    HRESULT STDMETHODCALLTYPE GetValue(BSTR name, BSTR* pValue);
    HRESULT STDMETHODCALLTYPE SetValue(BSTR name, BSTR value);

    // IUnknown methods
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();
    STDMETHOD(QueryInterface)(REFIID riid, void** pp);

private:

    LONG m_cRef;

    GUID m_guidModule;
    CComPtr<IBasicLog> m_spBasicLog;
    CComHeapPtr<WCHAR> m_spStrDeviceInstanceId;
    CComHeapPtr<WCHAR> m_spStrDriverDescription;
    CComHeapPtr<WCHAR> m_spStrPowerSupportType;
    CComHeapPtr<CComHeapPtr<WCHAR>> m_spArrayDeviceInstancePaths;
    UINT m_numDeviceInstancePaths;
    SetupApiContext m_SetupApiContext;
    bool m_disabledOnCreation;
};
