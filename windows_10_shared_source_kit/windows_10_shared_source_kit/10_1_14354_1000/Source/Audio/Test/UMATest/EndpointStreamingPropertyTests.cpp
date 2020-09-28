// Copyright(C) Microsoft. All rights reserved.

#include "StdAfx.h"

#include <CPropVariant.h>
#include <AvEndpointKeys.h>

#include <AudioOffload.h>
#include <OffloadPath.h>
#include <deviceaccesshelper.h>

#include "TestUtilities.h"
#include "EndpointStreamingPropertyTests.h"

DWORD TC_EndpointProperty_HardwareAcceleration()
{
    CComPtr<IMMDevice> spMMDevice;
    HRESULT hr = S_OK;

    hr = GetEndpointUnderTest(&spMMDevice);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: GetEndpointUnderTest HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );
        return FNS_FAIL;
    }

    CEndpointStreamingPropertyTests epStreamingPropTests( spMMDevice );

    (void) epStreamingPropTests.RunHardwareAccelerationTest_OFF();
    (void) epStreamingPropTests.RunHardwareAccelerationTest_ON();

    return FNS_PASS;
}

DWORD TC_EndpointProperty_DisableSystemEffects()
{
    CComPtr<IMMDevice> spMMDevice;
    HRESULT hr = S_OK;

    hr = GetEndpointUnderTest(&spMMDevice);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: GetEndpointUnderTest HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );
        return FNS_FAIL;
    }

    CEndpointStreamingPropertyTests epStreamingPropTests( spMMDevice );

    (void) epStreamingPropTests.RunSystemEffectsTest(false);
    (void) epStreamingPropTests.RunSystemEffectsTest(true);

    return FNS_PASS;
}

CEndpointStreamingPropertyTests::CEndpointStreamingPropertyTests( IMMDevice* pMMDevice )
 :  m_spMMDevice( pMMDevice )
{
    HRESULT hr = S_OK;

    if( m_spMMDevice == NULL )
    {
        return;
    }

    hr = m_spMMDevice->Activate( __uuidof(IAudioClient2), CLSCTX_ALL, NULL, (void **)&m_spAudioClient2 );
    if( FAILED(hr) )
    {
        return;
    }
}

DWORD CEndpointStreamingPropertyTests::RunHardwareAccelerationTest_OFF()
{
    HRESULT hr = S_OK;
    CComPtr<IBasicLog> spBasicLog;
    AudioClientProperties clientProperties = {0};
    bool bOffloadCapable = false;
    bool bClearKeyOnExit = false;
    bool bAccelerationCache = false;
    bool bAccelerationCheck = false;

    clientProperties.bIsOffload = TRUE;
    clientProperties.eCategory = AudioCategory_Media;
    clientProperties.cbSize = sizeof(clientProperties);

    if( m_spAudioClient2 == NULL )
    {
        XLOG( XFAIL, eError, "FAIL: Called RunHardwareAccelerationTest_OFF but m_spAudioClient2 is NULL" );

        return FNS_PASS;
    }

    // See if there is an offload endpoint on this device (device topology)
    hr = CheckDeviceSupportsOffload(m_spMMDevice, AudioOffload::MMDEV_API, bOffloadCapable );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eFatalError, "Error calling CheckDeviceSupportsOffload.  HRESULT 0x%08x [%s]", hr,
            GetHRString( hr ) );

        return FNS_PASS;
    }

    if( !bOffloadCapable )
    {
        bClearKeyOnExit = true;
    }

    hr = GetHardwareAccelerationSetting( bAccelerationCache );
    if( !bOffloadCapable && S_OK == hr )
    {
        // leave computer in state where the key exists
        XLOG( XFAIL, eError, "FAIL: Hardware Acceleration Key is present on an Endpoint that does NOT support Offload" );

        return FNS_PASS;
    }
    else if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Error Getting Hardware Acceleration Setting Value.  HRESULT 0x%08x [%s]",
            hr, GetHRString( hr ) );

        return FNS_PASS;
    }

    //
    // Test will do the following
    //  1. Set Hardware Acceleration to OFF
    //  2. Confirm setting is OFF
    //  3. SetClientProperties.bOffloadMode to TRUE -- confirm failure
    //  4. Restore Original value of Hardware Acceleration
    //

    hr = SetHardwareAccelerationSetting( false );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Error Setting Hardware Acceleration Setting Value to FALSE.  HRESULT 0x%08x [%s]",
            hr, GetHRString( hr ) );

        goto EXIT;
    }

    hr = GetHardwareAccelerationSetting( bAccelerationCheck );
    if( S_FALSE == hr )
    {
        XLOG( XFAIL, eError, "FAIL: Set the Hardware Acceleration Setting But Registry Key Does Not Exist." );

        goto EXIT;
    }
    else if( FAILED( hr ) )
    {
        XLOG( XFAIL, eError, "FAIL: Error Getting Hardware Acceleration Setting Value.  HRESULT 0x%08x [%s]",
            hr, GetHRString( hr ) );

        goto EXIT;
    }

    if( bAccelerationCheck != false )
    {
        XLOG( XFAIL, eError, "FAIL: Expected Hardware Acceleration Setting to be FALSE.  Actual TRUE." );

        goto EXIT;
    }

    hr = m_spAudioClient2->SetClientProperties( &clientProperties );
    if( AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE != hr )
    {
        XLOG( XFAIL, eError, "FAIL: Expected SetClientProperties to fail with AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE.  Actual 0x%08x [%s]",
            hr, GetHRString( hr ) );

        goto EXIT;
    }

EXIT:

    // restore original Hardware Acceleration value
    if( bClearKeyOnExit )
    {
        hr = ClearHardwareAccelerationSetting();
        if( FAILED(hr) )
        {
            XLOG( XFAIL, eError, "FAIL: Error Clearing Hardware Acceleration Setting.  HRESULT 0x%08x [%s]", hr,
                GetHRString( hr ) );
        }
    }
    else
    {
        hr = SetHardwareAccelerationSetting( bAccelerationCache );
        if( FAILED(hr) )
        {
            XLOG( XFAIL, eError, "FAIL: Error Restoring Hardware Acceleration Setting to %s.  HRESULT 0x%08x [%s]",
                (bAccelerationCache ? "TRUE" : "FALSE"), hr, GetHRString( hr ) );
        }
    }

    return FNS_PASS;
}

DWORD CEndpointStreamingPropertyTests::RunHardwareAccelerationTest_ON()
{
    HRESULT hr = S_OK;
    CComPtr<IBasicLog> spBasicLog;
    AudioClientProperties clientProperties = {0};
    bool bOffloadCapable = false;
    bool bClearKeyOnExit = false;
    bool bAccelerationCache = false;
    bool bAccelerationCheck = false;

    clientProperties.bIsOffload = TRUE;
    clientProperties.eCategory = AudioCategory_Media;
    clientProperties.cbSize = sizeof(clientProperties);

    if( m_spAudioClient2 == NULL )
    {
        XLOG( XFAIL, eError, "FAIL: Called RunHardwareAccelerationTest_ON but m_spAudioClient2 is NULL" );

        return FNS_PASS;
    }

    // Get an IBasicLog to pass to the AudioOffload methods to provide info on failures
    hr = g_IShell->GetBasicLog( &spBasicLog );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eFatalError, "Error calling ITestShell::GetBasicLog.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );

        return FNS_PASS;
    }

    // See if there is an offload endpoint on this device (device topology)
    hr = AudioOffload::DoesDeviceSupportOffload( spBasicLog, m_spMMDevice, AudioOffload::MMDEV_API, bOffloadCapable );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eFatalError, "Error calling AudioOffload::DoesDeviceSupportOffload.  HRESULT 0x%08x [%s]", hr,
            GetHRString( hr ) );

        return FNS_PASS;
    }

    if( !bOffloadCapable )
    {
        bClearKeyOnExit = true;
    }

    hr = GetHardwareAccelerationSetting( bAccelerationCache );
    if( !bOffloadCapable && S_OK == hr )
    {
        // leave computer in state where the key exists
        XLOG( XFAIL, eError, "FAIL: Hardware Acceleration Key is present on an Endpoint that does NOT support Offload" );

        return FNS_PASS;
    }
    else if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Error Getting Hardware Acceleration Setting Value.  HRESULT 0x%08x [%s]",
            hr, GetHRString( hr ) );

        return FNS_PASS;
    }

    //
    // Test will do the following
    //  1. Set Hardware Acceleration to ON
    //  2. Confirm setting is ON
    //  3. SetClientProperties.bOffloadMode to TRUE -- confirm success or failure
    //  4. Restore Original value of Hardware Acceleration
    //

    hr = SetHardwareAccelerationSetting( true );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Error Setting Hardware Acceleration Setting Value to FALSE.  HRESULT 0x%08x [%s]",
            hr, GetHRString( hr ) );

        goto EXIT;
    }

    hr = GetHardwareAccelerationSetting( bAccelerationCheck );
    if( S_FALSE == hr )
    {
        XLOG( XFAIL, eError, "FAIL: Set the Hardware Acceleration Setting But Registry Key Does Not Exist." );

        goto EXIT;
    }
    else if( FAILED( hr ) )
    {
        XLOG( XFAIL, eError, "FAIL: Error Getting Hardware Acceleration Setting Value.  HRESULT 0x%08x [%s]",
            hr, GetHRString( hr ) );

        goto EXIT;
    }

    if( bAccelerationCheck != true )
    {
        XLOG( XFAIL, eError, "FAIL: Expected Hardware Acceleration Setting to be TRUE.  Actual FALSE." );

        goto EXIT;
    }

    hr = m_spAudioClient2->SetClientProperties( &clientProperties );
    if( bOffloadCapable && FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Expected SetClientProperties to return S_OK when bOffloadMode set to TRUE on Offload Capable Endpoint.  Actual 0x%08x [%s]",
                hr, GetHRString( hr ) );

        goto EXIT;
    }
    else if( !bOffloadCapable && AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE != hr )
    {
        XLOG( XFAIL, eError, "FAIL: Expected SetClientProperties to return AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE when bOffloadMode set to TRUE on Endpoint NOT Capable of Offload.  Actual 0x%08x [%s]",
                hr, GetHRString( hr ) );

        goto EXIT;
    }

EXIT:

    // restore original Hardware Acceleration value
    if( bClearKeyOnExit )
    {
        hr = ClearHardwareAccelerationSetting();
        if( FAILED(hr) )
        {
            XLOG( XFAIL, eError, "FAIL: Error Clearing Hardware Acceleration Setting.  HRESULT 0x%08x [%s]", hr,
                GetHRString( hr ) );
        }
    }
    else
    {
        hr = SetHardwareAccelerationSetting( bAccelerationCache );
        if( FAILED(hr) )
        {
            XLOG( XFAIL, eError, "FAIL: Error Restoring Hardware Acceleration Setting to %s.  HRESULT 0x%08x [%s]",
                (bAccelerationCache ? "TRUE" : "FALSE"), hr, GetHRString( hr ) );
        }
    }

    return FNS_PASS;
}

DWORD CEndpointStreamingPropertyTests::RunSystemEffectsTest( bool bDisable )
{
    HRESULT hr = S_OK;
    CComPtr<IBasicLog> spBasicLog;
    CComPtr<IAudioClient2> spAudioClient2;
    CComPtr<IOffloadPathCollection> spOffloadPathCollection;
    CComPtr<IOffloadPath> spOffloadPath;
    CComHeapPtr<WCHAR> spAdapterId;
    bool bCacheDisableSetting = false;
    bool bClearOnExit = false;
    bool bInitialized = false;
    UINT cOffloadPath = 0;
    UINT uKsAENodeId = 0;
    bool bLfxEnable = false;
    bool bGfxEnable = false;

    if( m_spMMDevice == NULL )
    {
        XLOG( XFAIL, eError, "FAIL: Called RunSystemEffectsTest() but m_spMMDevice is NULL" );

        return FNS_PASS;
    }

    hr = m_spMMDevice->Activate( __uuidof(IAudioClient2), CLSCTX_ALL, NULL, (void **)&spAudioClient2 );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eFatalError, "Error calling IMMDevice::Activate( IAudioClient2 ).  HRESULT 0x%08x [%s]", hr,
            GetHRString( hr ) );

        return FNS_PASS;
    }

    // Get an IBasicLog to pass to the OffloadEnumerator methods to provide info on failures
    hr = g_IShell->GetBasicLog( &spBasicLog );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eFatalError, "Error calling ITestShell::GetBasicLog.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );

        return FNS_PASS;
    }

    hr = OffloadEnumerator::GetOffloadPath( spBasicLog, m_spMMDevice, &spOffloadPathCollection );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eFatalError, "Error calling OffloadEnumerator::GetOffloadPath.  HRESULT 0x%08x [%s]", hr,
            GetHRString( hr ) );

        return FNS_PASS;
    }

    hr = spOffloadPathCollection->GetCount( cOffloadPath );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eFatalError, "Error calling OffloadPathCollection::GetCount.  HRESULT 0x%08x [%s]", hr,
            GetHRString( hr ) );

        return FNS_PASS;
    }

    if( cOffloadPath == 0 )
    {
        XLOG( XWARN, eWarn1, "Audio Endpoint does not support Audio Offload.  Skipping test." );

        return FNS_PASS;
    }
    else if( 1 < cOffloadPath )
    {
        XLOG( XWARN, eWarn1, "Audio Endpoint supports %u Audio Offload Paths.  Only testing the first reported.", cOffloadPath );
    }

    hr = spOffloadPathCollection->Item( 0, &spOffloadPath );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eFatalError, "Error calling OffloadPathCollection::Item.  HRESULT 0x%08x [%s]", hr,
            GetHRString( hr ) );

        return FNS_PASS;
    }

    // neet to know the node id so we can get the lfx and gfx values from the driver
    hr = spOffloadPath->GetKsNodeId( &uKsAENodeId );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eFatalError, "Error calling OffloadPath::GetKsNodeId.  HRESULT 0x%08x [%s]", hr,
            GetHRString( hr ) );

        return FNS_PASS;
    }

    // also need to know the IMMDevice Id for the Adapter Filter to send commands through IKsControl
    hr = spOffloadPath->GetMMDeviceId( &spAdapterId );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eFatalError, "Error calling OffloadPath::GetMMEndpointId.  HRESULT 0x%08x [%s]", hr,
            GetHRString( hr ) );

        return FNS_PASS;
    }

    hr = GetDisableSystemEffectsSetting( bCacheDisableSetting );
    if( S_FALSE == hr )
    {
        bClearOnExit = true;
    }
    else if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Error Getting Disable System Effects Setting Value.  HRESULT 0x%08x [%s]",
            hr, GetHRString( hr ) );

        return FNS_PASS;
    }

    // if the registry key isn't set or isn't what we need for this pass, set it
    if( bClearOnExit || bCacheDisableSetting != bDisable )
    {
        hr = SetDisableSystemEffectsSetting( bDisable );
        if( FAILED(hr) )
        {
            XLOG( XFAIL, eError, "FAIL: Error Setting Disable System Effects Setting Value.  HRESULT 0x%08x [%s]",
                hr, GetHRString( hr ) );

            goto EXIT;
        }
    }

    // now that the registry key is set, initialize the endpoint.  On initialize, the lfx/gfx should be enabled
    // or disabled according to what the registry key is
    hr = InitializeInSharedMode( spAudioClient2, bInitialized );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Error Initializing the IAudioClient in Shared Mode.  HRESULT 0x%08x [%s]",
            hr, GetHRString( hr ) );

        goto EXIT;
    }

    // check to make sure that lfx is enabled -or- disabled
    hr = GetLfxEnable( spAudioClient2, uKsAENodeId, bLfxEnable );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Error Getting the LFX Enable Value.  HRESULT 0x%08x [%s]",
            hr, GetHRString( hr ) );

        goto EXIT;
    }

    if( bLfxEnable == bDisable )
    {
        XLOG( XFAIL, eError, "Expected the LFX Enable value to be %s (%u).  Actual %u.", (bDisable ? "FALSE" : "TRUE"), (bDisable ? FALSE : TRUE), bLfxEnable ? TRUE : FALSE );
    }

    // check to make sure that gfx is enabled -or- disabled
    hr = GetGfxEnable( spAdapterId, uKsAENodeId, bGfxEnable );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Error Getting the GFX Enable Value.  HRESULT 0x%08x [%s]",
            hr, GetHRString( hr ) );

        goto EXIT;
    }

    if( bGfxEnable == bDisable )
    {
        XLOG( XFAIL, eError, "Expected the GFX Enable value to be %s (%u).  Actual %u.", (bDisable ? "FALSE" : "TRUE"), (bDisable ? FALSE : TRUE), bLfxEnable ? TRUE : FALSE );
    }

EXIT:

    // now put everything back the way we found it
    if( bClearOnExit )
    {
        hr = ClearDisableSystemEffectsSetting();
        if( FAILED(hr) )
        {
            XLOG( XFAIL, eError, "FAIL: Error Clearing Disable System Effects Setting Value.  HRESULT 0x%08x [%s]",
                hr, GetHRString( hr ) );
        }
    }
    else
    {
        hr = SetDisableSystemEffectsSetting( bCacheDisableSetting );
        if( FAILED(hr) )
        {
            XLOG( XFAIL, eError, "FAIL: Error Setting Disable System Effects Setting Value.  HRESULT 0x%08x [%s]",
                hr, GetHRString( hr ) );
        }
    }

    return FNS_PASS;
}

HRESULT CEndpointStreamingPropertyTests::GetHardwareAccelerationSetting( bool& bAllowed )
{
    HRESULT hr = S_OK;
    CComPtr<IPropertyStore> spPropertyStore;
    CPropVariant var;

    if( m_spMMDevice == NULL )
    {
        XLOG( XFAIL, eError, "FAIL: Called GetHardwareAccelerationSetting but m_spMMDevice is NULL" );

        return E_POINTER;
    }

    hr = m_spMMDevice->OpenPropertyStore( STGM_READ, &spPropertyStore );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Error Opening Property Store of IMMDevice.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );

        return hr;
    }

    hr = spPropertyStore->GetValue( PKEY_Endpoint_AllowOffloading, &var );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Error Getting 'AllowOffloading' Value.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );

        return hr;
    }

    if( VT_EMPTY == var.vt )
    {
        // if the key doesn't exist, this defaults to "offloading is allowed"
        SLOG( eInfo2, "PKEY_Endpoint_AllowOffloading Does Not Exist.  Defaults to TRUE." );
        bAllowed = true;

        return S_FALSE;
    }

    if( VT_BOOL != var.vt )
    {
        XLOG( XFAIL, eError, "FAIL: Value returned from Property Store was not a VT_BOOL.  Actual %hu", var.vt );

        return E_UNEXPECTED;
    }

    if( var.boolVal != VARIANT_TRUE && var.boolVal != VARIANT_FALSE )
    {
        XLOG( XFAIL, eError, "FAIL: Value returned from Property Store was not VARIANT_TRUE(%i) or VARIANT_FALSE(%i).  Actual %i",
            VARIANT_TRUE, VARIANT_FALSE, var.boolVal );

        return E_UNEXPECTED;
    }

    bAllowed = ( var.boolVal == VARIANT_TRUE ? true : false );

    return S_OK;
}

HRESULT CEndpointStreamingPropertyTests::SetHardwareAccelerationSetting( bool bAllowed )
{
    HRESULT hr = S_OK;
    CComPtr<IPropertyStore> spPropertyStore;
    CPropVariant var;

    if( m_spMMDevice == NULL )
    {
        XLOG( XFAIL, eError, "FAIL: Called SetHardwareAccelerationSetting but m_spMMDevice is NULL" );

        return E_POINTER;
    }

    var.vt = VT_BOOL;
    var.boolVal = ( bAllowed ? VARIANT_TRUE : VARIANT_FALSE );

    if (GetModule()->IsAppContainer())
    {
        CComPtr<IAudioDeviceAccessHelper> pHelper = GetModule()->GetDeviceAccessHelper();

        CComHeapPtr<WCHAR> pszId;
        hr = m_spMMDevice->GetId(&pszId);
        if ( FAILED(hr) )
        {
            XLOG( XFAIL, eFatalError, "Error calling IMMDevice::GetId (0x%8x) [%s]", hr, GetHRString( hr ) );
            return hr;
        }

        hr = pHelper->SetHardwareAcceleration(pszId, bAllowed? HARDWARE_ACCELERATION_SETTING_ALLOW : HARDWARE_ACCELERATION_SETTING_DISALLOW);
        if ( FAILED(hr) )
        {
            XLOG( XFAIL, eError, "FAIL: Error calling IAudioDeviceAccessHelper::SetHardwareAcceleration.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );

            return hr;
        }
    }
    else
    {
        hr = m_spMMDevice->OpenPropertyStore( STGM_WRITE, &spPropertyStore );
        if( FAILED(hr) )
        {
            XLOG( XFAIL, eError, "FAIL: Error Opening Property Store of IMMDevice.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );

            return hr;
        }
   

        hr = spPropertyStore->SetValue( PKEY_Endpoint_AllowOffloading, var );
        if( FAILED(hr) )
        {
            XLOG( XFAIL, eError, "FAIL: Error Setting 'AllowOffloading' Value.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );

            return hr;
        }

        hr = spPropertyStore->Commit();
        if( FAILED(hr) )
        {
            XLOG( XFAIL, eError, "FAIL: Error on Commit of Property Store Change.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );

            return hr;
        }
    }
    return S_OK;
}

HRESULT CEndpointStreamingPropertyTests::ClearHardwareAccelerationSetting()
{
    HRESULT hr = S_OK;
    
    CComPtr<IPropertyStore> spPropertyStore;
    CPropVariant var;

    if( m_spMMDevice == NULL )
    {
        XLOG( XFAIL, eError, "FAIL: Called ClearHardwareAccelerationSetting but m_spMMDevice is NULL" );

        return E_POINTER;
    }

    if (GetModule()->IsAppContainer())
    {
        CComPtr<IAudioDeviceAccessHelper> pHelper = GetModule()->GetDeviceAccessHelper();

        CComHeapPtr<WCHAR> pszId;
        hr = m_spMMDevice->GetId(&pszId);
        if ( FAILED(hr) )
        {
            XLOG( XFAIL, eFatalError, "Error calling IMMDevice::GetId (0x%8x) [%s]", hr, GetHRString( hr ) );
            return hr;
        }

        hr = pHelper->SetHardwareAcceleration(pszId, HARDWARE_ACCELERATION_SETTING_NONE);
        if ( FAILED(hr) )
        {
            XLOG( XFAIL, eError, "FAIL: Error calling IAudioDeviceAccessHelper::SetHardwareAcceleration.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );

            return hr;
        }
    }
    else
    {
        hr = m_spMMDevice->OpenPropertyStore( STGM_WRITE, &spPropertyStore );
        if( FAILED(hr) )
        {
            XLOG( XFAIL, eError, "FAIL: Error Opening Property Store of IMMDevice.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );

            return hr;
        }
    

        hr = spPropertyStore->SetValue( PKEY_Endpoint_AllowOffloading, var );
        if( FAILED(hr) )
        {
            XLOG( XFAIL, eError, "FAIL: Error Setting 'AllowOffloading' Value.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );

            return hr;
        }

        hr = spPropertyStore->Commit();
        if( FAILED(hr) )
        {
            XLOG( XFAIL, eError, "FAIL: Error on Commit of Property Store Change.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );

            return hr;
        }
    }
    return S_OK;
}

HRESULT CEndpointStreamingPropertyTests::GetDisableSystemEffectsSetting( bool& bDisableSysFx )
{
    HRESULT hr = S_OK;
    CComPtr<IMMEndpointInternal> spMMEndpointInternal;
    CComPtr<IPropertyStore> spFXPropertyStore;
    CPropVariant var;

    if( m_spMMDevice == NULL )
    {
        XLOG( XFAIL, eError, "FAIL: Called GetDisableSystemEffectsSetting but m_spMMDevice is NULL" );

        return E_POINTER;
    }

    hr = m_spMMDevice->QueryInterface( __uuidof(IMMEndpointInternal), (void**)&spMMEndpointInternal );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Error Querying for the Endpoint Internal Interface.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );

        return hr;
    }

    hr = spMMEndpointInternal->OpenFXPropertyStore( STGM_READ, &spFXPropertyStore );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Error Opening Effects Property Store of IMMDevice.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );

        return hr;
    }

    hr = spFXPropertyStore->GetValue( PKEY_AudioEndpoint_Disable_SysFx, &var );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Error Getting 'Disable System Effects' Value.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );

        return hr;
    }

    if( VT_EMPTY == var.vt )
    {
        // if the key doesn't exist, this defaults to "sys fx are NOT disabled"
        SLOG( eInfo2, "PKEY_AudioEndpoint_Disable_SysFx Does Not Exist.  Defaults to FALSE." );
        bDisableSysFx = false;

        return S_FALSE;
    }

    if( VT_UI4 != var.vt )
    {
        XLOG( XFAIL, eError, "FAIL: Value returned from Property Store was not a VT_UI4.  Actual %hu", var.vt );

        return E_UNEXPECTED;
    }

    if( var.ulVal != ENDPOINT_SYSFX_ENABLED && var.ulVal != ENDPOINT_SYSFX_DISABLED )
    {
        XLOG( XFAIL, eError, "FAIL: Value returned from Property Store was not ENDPOINT_SYSFX_ENABLED(%i) or ENDPOINT_SYSFX_ENABLED(%i).  Actual %i",
            ENDPOINT_SYSFX_ENABLED, ENDPOINT_SYSFX_ENABLED, var.boolVal );

        return E_UNEXPECTED;
    }

    SLOG( eInfo2, "PKEY_AudioEndpoint_Disable_SysFx is %u", var.boolVal );
    bDisableSysFx = ( var.boolVal == ENDPOINT_SYSFX_DISABLED ? true : false );

    return S_OK;
}

HRESULT CEndpointStreamingPropertyTests::SetDisableSystemEffectsSetting( bool bDisableSysFx )
{
    HRESULT hr = S_OK;
    CComPtr<IMMEndpointInternal> spMMEndpointInternal;
    CComPtr<IPropertyStore> spFXPropertyStore;
    CPropVariant var;

    if( m_spMMDevice == NULL )
    {
        XLOG( XFAIL, eError, "FAIL: Called SetDisableSystemEffectsSetting but m_spMMDevice is NULL" );

        return E_POINTER;
    }

    hr = m_spMMDevice->QueryInterface( __uuidof(IMMEndpointInternal), (void**)&spMMEndpointInternal );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Error Querying for the Endpoint Internal Interface.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );

        return hr;
    }

    hr = spMMEndpointInternal->OpenFXPropertyStore( STGM_WRITE, &spFXPropertyStore );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Error Opening Effects Property Store of IMMDevice.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );

        return hr;
    }

    var.vt = VT_UI4;
    var.ulVal = ( bDisableSysFx ? ENDPOINT_SYSFX_DISABLED : ENDPOINT_SYSFX_ENABLED );

    hr = spFXPropertyStore->SetValue( PKEY_AudioEndpoint_Disable_SysFx, var );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Error Setting 'Disable System Effects' Value.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );

        return hr;
    }

    hr = spFXPropertyStore->Commit();
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Error on Commit of Property Store Change.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );

        return hr;
    }

    SLOG( eInfo2, "Set PKEY_AudioEndpoint_Disable_SysFx to %u", var.boolVal );

    return S_OK;
}

HRESULT CEndpointStreamingPropertyTests::ClearDisableSystemEffectsSetting()
{
    HRESULT hr = S_OK;
    CComPtr<IPropertyStore> spPropertyStore;
    CPropVariant var;

    if( m_spMMDevice == NULL )
    {
        XLOG( XFAIL, eError, "FAIL: Called ClearDisableSystemEffectsSetting but m_spMMDevice is NULL" );

        return E_POINTER;
    }

    hr = m_spMMDevice->OpenPropertyStore( STGM_WRITE, &spPropertyStore );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Error Opening Property Store of IMMDevice.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );

        return hr;
    }

    hr = spPropertyStore->SetValue( PKEY_AudioEndpoint_Disable_SysFx, var );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Error Setting 'Disable System Effects' Value.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );

        return hr;
    }

    hr = spPropertyStore->Commit();
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Error on Commit of Property Store Change.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );

        return hr;
    }

    return S_OK;
}

HRESULT CEndpointStreamingPropertyTests::InitializeInSharedMode( IAudioClient2* pAudioClient2, bool& isInitialized )
{
    HRESULT hr = S_OK;
    CComHeapPtr<WAVEFORMATEX> pWfx;
    const REFERENCE_TIME hnsTwentyMS = 200000;
    AudioClientProperties clientProperties = {0};

    isInitialized = false;

    clientProperties.eCategory = AudioCategory_Other;
    clientProperties.cbSize = sizeof(clientProperties);

    if( DEVICE_TYPE_RENDEROFFLOAD == GetDeviceTypeUnderTest() )
    {
        // Setup for the offload path
        clientProperties.eCategory = AudioCategory_Media;
        clientProperties.bIsOffload = TRUE;
    }
    else
    {
        // setup for the software path
        clientProperties.bIsOffload = FALSE;
    }

    METHODCHECKRESULT( "IAudioClient2::SetClientProperties", hr, pAudioClient2->SetClientProperties( &clientProperties ) );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Expected SetClientProperties to return 0x%08x [S_OK].  Actual 0x%08x [%s]",
            S_OK, hr, GetHRString( hr ) );

        return hr;
    }

    // Get the mix format to use for initialize
    METHODCHECKRESULT( "IAudioClient2::GetMixFormat", hr, pAudioClient2->GetMixFormat( &pWfx ) );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Expected GetMixFormat to return 0x%08x [S_OK].  Actual 0x%08x [%s]",
            S_OK, hr, GetHRString( hr ) );

        return hr;
    }

    // Now, Initialize a shared mode stream (20ms buffer)
    METHODCHECKRESULT( "IAudioClient2::Initialize", hr,
        pAudioClient2->Initialize( AUDCLNT_SHAREMODE_SHARED, 0, hnsTwentyMS, 0, pWfx, NULL ) );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Expected Initialize to return 0x%08x [S_OK].  Actual 0x%08x [%s]",
            S_OK, hr, GetHRString( hr ) );

        return hr;
    }
    else
    {
        isInitialized = true;
    }

    return S_OK;
}

HRESULT CEndpointStreamingPropertyTests::GetLfxEnable( IAudioClient2* pAudioClient2, UINT uKsAENodeId, bool& bLfxEnable )
{
    HRESULT hr = S_OK;
    CComPtr<IKsControl> spKsControl;
    KSNODEPROPERTY ksNodeProperty = {0};
    ULONG cBytesReturned = 0;
    BOOL isLfxEnabled = FALSE;

    bLfxEnable = false;

    hr = pAudioClient2->GetService( __uuidof(IKsControl), (void **)&spKsControl );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Error Getting the IKsControl Service.  HRESULT 0x%08x [%s]",
            hr, GetHRString( hr ) );

        return hr;
    }

    ksNodeProperty.NodeId = uKsAENodeId;
    ksNodeProperty.Property.Set = KSPROPSETID_AudioEngine;
    ksNodeProperty.Property.Id = KSPROPERTY_AUDIOENGINE_LFXENABLE;
    ksNodeProperty.Property.Flags = KSPROPERTY_TYPE_TOPOLOGY | KSPROPERTY_TYPE_GET;

    hr = spKsControl->KsProperty( &ksNodeProperty.Property, sizeof(ksNodeProperty), &isLfxEnabled, sizeof(isLfxEnabled),
        &cBytesReturned );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Error Getting the LFX ENABLE KS Value with IKsControl::KsProperty().  HRESULT 0x%08x [%s]",
            hr, GetHRString( hr ) );

        return hr;
    }

    if( cBytesReturned != sizeof(BOOL) )
    {
        XLOG( XFAIL, eError, "FAIL: Expected the Bytes Returned from KsProperty to be sizeof(BOOL) (%Iu).  Actual %u",
            sizeof(BOOL), cBytesReturned );

        return E_UNEXPECTED;
    }

    SLOG( eInfo2, "LFX Enable is %u", isLfxEnabled );
    bLfxEnable = (isLfxEnabled != FALSE);

    return S_OK;
}

HRESULT CEndpointStreamingPropertyTests::GetGfxEnable( LPCWSTR pwszAdapter, UINT uKsAENodeId, bool& bGfxEnable )
{
    HRESULT hr = S_OK;
    CComPtr<IMMDevice> spAdapter;
    CComPtr<IMMDeviceEnumerator> spDeviceEnumerator;
    CComPtr<IKsControl> spKsControl;
    KSNODEPROPERTY ksNodeProperty = {0};
    ULONG cBytesReturned = 0;
    BOOL isGfxEnabled = FALSE;

    hr = spDeviceEnumerator.CoCreateInstance( __uuidof(MMDeviceEnumerator) );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Error Creating MMDeviceEnumerator Instance.  HRESULT 0x%08x [%s]",
            hr, GetHRString( hr ) );

        return hr;
    }

    hr = spDeviceEnumerator->GetDevice( pwszAdapter, &spAdapter );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Error Getting Device %S.  HRESULT 0x%08x [%s]",
            pwszAdapter, hr, GetHRString( hr ) );

        return hr;
    }

    hr = spAdapter->Activate( __uuidof(IKsControl), CLSCTX_ALL, NULL, (void**)&spKsControl );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Error Activating IKsControl on %S.  HRESULT 0x%08x [%s]",
            pwszAdapter, hr, GetHRString( hr ) );

        return hr;
    }

    ksNodeProperty.NodeId = uKsAENodeId;
    ksNodeProperty.Property.Set = KSPROPSETID_AudioEngine;
    ksNodeProperty.Property.Id = KSPROPERTY_AUDIOENGINE_GFXENABLE;
    ksNodeProperty.Property.Flags = KSPROPERTY_TYPE_TOPOLOGY | KSPROPERTY_TYPE_GET;

    hr = spKsControl->KsProperty( &ksNodeProperty.Property, sizeof(ksNodeProperty), &isGfxEnabled, sizeof(isGfxEnabled), &cBytesReturned );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Error Getting the GFX ENABLE KS Value with IKsControl::KsProperty().  HRESULT 0x%08x [%s]",
            hr, GetHRString( hr ) );

        return hr;
    }

    if( cBytesReturned != sizeof(BOOL) )
    {
        XLOG( XFAIL, eError, "FAIL: Expected the Bytes Returned from KsProperty to be sizeof(BOOL) (%Iu).  Actual %u",
            sizeof(BOOL), cBytesReturned );

        return E_UNEXPECTED;
    }

    SLOG( eInfo2, "GFX Enable is %u", isGfxEnabled );
    bGfxEnable = (isGfxEnabled != FALSE);

    return S_OK;
}
