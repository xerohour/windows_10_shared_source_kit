//--------------------------------------------------------------------------;
//
//  File: DeviceSupport.cpp
//
// Copyright (C) Microsoft. All rights reserved.
//
//  Abstract:
//      Test class for HDAudioPowerTest.
//
//  Contents:
//      BuildResourceList
//
//  History:
//      02/14/2012    CaWerner       Initial implementation.
//
//--------------------------------------------------------------------------;
#include "common.h"
#include <initguid.h>
#include <devpkey.h>

#include <resourcelist.h>
#include <itestresource.h>
#include <wextestclass.h>

using namespace WEX::Logging;
using namespace WEX::TestExecution;

#include "SetupApiHelper.h"
#include "DeviceSupport.h"

HRESULT __cdecl BuildResourceList( ResourceList& resourceList )
{
    HRESULT hr = S_OK;
    CComPtr<IBasicLog> spBasicLog;
    CComHeapPtr<CComHeapPtr<WCHAR>> spArrayDeviceInstanceIds;
    UINT numDeviceInstanceIds = 0;
    GUID guidModule = GUID_NULL;

    g_pResourceList = nullptr;

    // don't want to see VERIFY spew
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    if( !VERIFY_SUCCEEDED(hr = CreateWexBasicLog( &spBasicLog )) ) { return hr; }

    // Enumerate the Device Instance Ids for Audio Devnodes
    if( !VERIFY_SUCCEEDED(
        hr = SetupApiHelper::EnumerateAudioDeviceInstanceIds( spBasicLog, &spArrayDeviceInstanceIds, &numDeviceInstanceIds ) ) )
    { return hr; }

    if( !VERIFY_SUCCEEDED( hr = CoCreateGuid( &guidModule ) ) ) { return hr; }

    LOG( spBasicLog, L"BuildResourceList: Number of Audio Devnodes: %u", numDeviceInstanceIds );

    for( UINT i = 0; i < numDeviceInstanceIds; i++ )
    {
        CComPtr<ITestResource> spTestResource;

        // create a new AudioDevnode
        spTestResource.Attach( new AudioDevnode( spBasicLog, spArrayDeviceInstanceIds[i], guidModule, hr ) );
        if( !spTestResource )
        {
            LOG( spBasicLog, L"BuildResourceList: Failed to Allocate a New AudioDevnode for %s.",
                spArrayDeviceInstanceIds[i].m_pData );
            hr = E_OUTOFMEMORY;

            return hr;
        }

        if( S_OK != hr )
        {
            LOG( spBasicLog, L"BuildResourceList: Failed to Create a New AudioDevnode for %s.  (HRESULT:0x%08x)",
                spArrayDeviceInstanceIds[i].m_pData, hr );

            return hr;
        }

        // add it to the resource list
        hr = resourceList.Add( spTestResource );
        if( S_OK != hr )
        {
            LOG( spBasicLog, L"Failed to add new AudioDevnode to Resource List" );

            return hr;
        }

        LOG( spBasicLog, L"Added Audio Devnode: %s", spArrayDeviceInstanceIds[i].m_pData );
    }

    g_pResourceList = &resourceList;

    return S_OK;
}

AudioDevnode::AudioDevnode( IBasicLog* pBasicLog, LPCWSTR pszDeviceInstanceId, GUID guidModule, HRESULT& hr )
 :  m_cRef(1), m_guidModule(guidModule), m_numDeviceInstancePaths(0), m_SetupApiContext(), m_disabledOnCreation(false)
{
    size_t cchDeviceInstanceId = 0;
    UINT cbDriverDescription = 0;

    m_spBasicLog = pBasicLog;

    if( !VERIFY_SUCCEEDED( hr = StringCchLength( pszDeviceInstanceId, STRSAFE_MAX_CCH, &cchDeviceInstanceId ) ) ) { return; }

    if( !m_spStrDeviceInstanceId.Allocate( cchDeviceInstanceId + 1 ) )
    {
        LOG( pBasicLog, L"Failed to allocate the device instance id." );
        hr = E_OUTOFMEMORY;

        return;
    }

    if( !VERIFY_SUCCEEDED( hr = StringCchCopy( m_spStrDeviceInstanceId.m_pData, cchDeviceInstanceId + 1, pszDeviceInstanceId ) ) )
    {
        return;
    }

    if( !VERIFY_SUCCEEDED( hr = SetupApiHelper::GetSetupApiContext( pBasicLog, m_spStrDeviceInstanceId, &m_SetupApiContext ) ) )
    {
        return;
    }

    // get the driver description string
    if( !VERIFY_SUCCEEDED( hr = SetupApiHelper::GetDeviceProperty( pBasicLog,
                                                                   &m_SetupApiContext,
                                                                   &DEVPKEY_Device_DriverDesc,
                                                                   (PBYTE*)&m_spStrDriverDescription.m_pData,
                                                                   &cbDriverDescription ) ) )
    {
        return;
    }

    if( !m_spStrPowerSupportType.AllocateBytes( (ARRAYSIZE(L"D3Cold") + 1) * sizeof(WCHAR) ) )
    {
        Log::Comment( L"Failed to allocate the power support type string." );
        hr = E_OUTOFMEMORY;

        return;
    }

    StringCbCopy( m_spStrPowerSupportType.m_pData, (ARRAYSIZE(L"D3Cold") + 1) * sizeof(WCHAR), L"D3Cold" );

    if( !VERIFY_SUCCEEDED( hr = IsDisabled( &m_disabledOnCreation ) ) ) { return; }

    hr = S_OK;
}

AudioDevnode::~AudioDevnode()
{
}

HRESULT AudioDevnode::GetDevicePowerState( DEVICE_POWER_STATE* devicePowerState )
{
    HRESULT hr = S_OK;
    CComHeapPtr<CM_POWER_DATA> spCmPowerData;
    UINT cbCmPowerData = 0;

    *devicePowerState = PowerDeviceUnspecified;

    if( !VERIFY_SUCCEEDED( hr = SetupApiHelper::GetDeviceProperty( m_spBasicLog,
                                                                   &m_SetupApiContext,
                                                                   &DEVPKEY_Device_PowerData,
                                                                   (PBYTE*)&spCmPowerData.m_pData,
                                                                   &cbCmPowerData ) ) )
    {
        return hr;
    }

    *devicePowerState = spCmPowerData->PD_MostRecentPowerState;

    return S_OK;
}

HRESULT AudioDevnode::GetConservationIdleTime( ULONG* pSecsConservationIdleTime )
{
    HRESULT hr = S_OK;

    *pSecsConservationIdleTime = 0;

    if( !VERIFY_SUCCEEDED( hr = SetupApiHelper::GetConservationIdleTime( m_spBasicLog,
                                                                         &m_SetupApiContext,
                                                                         pSecsConservationIdleTime ) ) )
    {
        return hr;
    }

    if( *pSecsConservationIdleTime == 0 )
    {
        LOG( m_spBasicLog, L"Conservation Idle Disabled for Devnode" );

        return S_FALSE;
    }

    return S_OK;
}

HRESULT AudioDevnode::Disable()
{
    HRESULT hr = S_OK;

    if( !VERIFY_SUCCEEDED( hr = SetupApiHelper::DisableDevnode( m_spBasicLog, &m_SetupApiContext ) ) ) { return hr; }

    return hr;
}

HRESULT AudioDevnode::Enable()
{
    HRESULT hr = S_OK;

    if( !VERIFY_SUCCEEDED( hr = SetupApiHelper::EnableDevnode( m_spBasicLog, &m_SetupApiContext ) ) ) { return hr; }

    return hr;
}

HRESULT AudioDevnode::RestoreDevnodeState()
{
    HRESULT hr = S_OK;
    bool isDisabled = true;

    if( !VERIFY_SUCCEEDED( hr = IsDisabled( &isDisabled ) ) ) { return hr; }

    if( m_disabledOnCreation == isDisabled )
    {
        return S_OK;
    }

    if( m_disabledOnCreation )
    {
        if( !VERIFY_SUCCEEDED( hr = Disable() ) ) { return hr; }
    }
    else
    {
        if( !VERIFY_SUCCEEDED( hr = Enable() ) ) { return hr; }
    }

    return S_OK;
}

HRESULT AudioDevnode::IsEnabled( bool* pIsEnabled )
{
    HRESULT hr = S_OK;
    ULONG status = 0;
    ULONG problem = 0;

    if( !VERIFY_SUCCEEDED( hr = SetupApiHelper::GetDevnodeStatus( m_spBasicLog, &m_SetupApiContext, &status, &problem ) ) ) { return hr; }

    *pIsEnabled = (problem != CM_PROB_DISABLED);

    return S_OK;
}

HRESULT AudioDevnode::IsDisabled( bool* pIsDisabled )
{
    HRESULT hr = S_OK;
    ULONG status = 0;
    ULONG problem = 0;

    if( !VERIFY_SUCCEEDED( hr = SetupApiHelper::GetDevnodeStatus( m_spBasicLog, &m_SetupApiContext, &status, &problem ) ) ) { return hr; }

    *pIsDisabled = (problem == CM_PROB_DISABLED);

    return S_OK;
}

// IUnknown methods
ULONG STDMETHODCALLTYPE AudioDevnode::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

ULONG STDMETHODCALLTYPE AudioDevnode::Release()
{
    LONG retval = InterlockedDecrement(&m_cRef);

    if( 0 >= retval )
    {
        delete this;
    }

    return retval;
}

HRESULT STDMETHODCALLTYPE AudioDevnode::QueryInterface( REFIID riid, void** pp )
{
    if( nullptr == pp)
    {
        return E_POINTER;
    }

    *pp = nullptr;

    if( __uuidof(IUnknown) == riid || __uuidof(ITestResource) == riid )
    {
        *pp = (ITestResource *)(this);
        AddRef();

        return S_OK;
    }

    return E_NOINTERFACE;
}

// ITestResource methods
HRESULT STDMETHODCALLTYPE AudioDevnode::GetGuid( GUID* pGuid )
{
    if( !VERIFY_IS_NOT_NULL(pGuid) )
    {
        return E_POINTER;
    }

    *pGuid = m_guidModule;

    return S_OK;
}

HRESULT STDMETHODCALLTYPE AudioDevnode::SetGuid(GUID guid) {
    m_guidModule = guid;

    return S_OK;
}

HRESULT STDMETHODCALLTYPE AudioDevnode::GetValue(BSTR name, BSTR* pValue) {
    // don't want to see VERIFY spew
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    if (!VERIFY_IS_NOT_NULL(name)) { return E_POINTER; }
    if (!VERIFY_IS_NOT_NULL(pValue)) { return E_POINTER; }

    if (0 == wcscmp(name, TestResourceProperty::c_szName)) {
        if (!VERIFY_IS_NOT_NULL(*pValue = SysAllocString(m_spStrDriverDescription))) { return E_OUTOFMEMORY; }
        return S_OK;
    }

    if (0 == wcscmp(name, TestResourceProperty::c_szId)) {
        if (!VERIFY_IS_NOT_NULL(*pValue = SysAllocString(m_spStrDeviceInstanceId))) { return E_OUTOFMEMORY; }
        return S_OK;
    }

    if (0 == wcscmp(name, TestResourceProperty::c_szType)) {
        if (!VERIFY_IS_NOT_NULL(*pValue = SysAllocString(m_spStrPowerSupportType))) { return E_OUTOFMEMORY; }
        return S_OK;
    }

    LOG( m_spBasicLog, L"AudioDevnode::GetValue name not found: %s", name );
    return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
}

HRESULT STDMETHODCALLTYPE AudioDevnode::SetValue(BSTR name, BSTR value) {
    // don't want to see VERIFY spew
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    if (!VERIFY_IS_NOT_NULL(name)) { return E_POINTER; }
    if (!VERIFY_IS_NOT_NULL(value)) { return E_POINTER; }

    HRESULT hr;

    if (0 == wcscmp(name, TestResourceProperty::c_szName)) {
        size_t cbStrDriverDescription = 0;

        if( !VERIFY_SUCCEEDED(
            hr = StringCbLength( m_spStrDriverDescription, STRSAFE_MAX_CCH * sizeof(WCHAR), &cbStrDriverDescription ) ) )
        { return hr; }

        if (!VERIFY_SUCCEEDED(
            hr = StringCbCopy(m_spStrDriverDescription, cbStrDriverDescription + sizeof(WCHAR), value)))
        { return hr; }

        return S_OK;
    }

    if (0 == wcscmp(name, TestResourceProperty::c_szId)) {
        size_t cbStrDeviceInstanceId = 0;

        if( !VERIFY_SUCCEEDED(
            hr = StringCbLength( m_spStrDeviceInstanceId, STRSAFE_MAX_CCH * sizeof(WCHAR), &cbStrDeviceInstanceId ) ) )
        { return hr; }

        if (!VERIFY_SUCCEEDED(
            hr = StringCbCopy(m_spStrDeviceInstanceId, cbStrDeviceInstanceId + sizeof(WCHAR), value)))
        { return hr; }

        return S_OK;
    }

    if (0 == wcscmp(name, TestResourceProperty::c_szId)) {
        size_t cbStrPowerSupportType = 0;

        if( !VERIFY_SUCCEEDED(
            hr = StringCbLength( m_spStrPowerSupportType, STRSAFE_MAX_CCH * sizeof(WCHAR), &cbStrPowerSupportType ) ) )
        { return hr; }

        if (!VERIFY_SUCCEEDED(
            hr = StringCbCopy(m_spStrPowerSupportType, cbStrPowerSupportType + sizeof(WCHAR), value)))
        { return hr; }

        return S_OK;
    }

    LOG( m_spBasicLog, L"AudioDevnode::SetValue name not found: %s", name );
    return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
}
