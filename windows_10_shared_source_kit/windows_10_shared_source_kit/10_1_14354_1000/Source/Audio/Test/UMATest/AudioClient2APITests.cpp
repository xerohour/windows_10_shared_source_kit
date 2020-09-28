// Copyright(C) Microsoft. All rights reserved.

//
// AudioClient2APITests.cpp
//
// Description:
//
//  IAudioClient2 API Tests

#include <stdafx.h>
#include <AudioOffload.h>
#include <DeviceAccessHelper.h>

#include "Util.h"
#include "TestUtilities.h"
#include "AudioClient2APITests.h"

AUDIO_STREAM_CATEGORY g_offloadCategories[] = { AudioCategory_Movie, AudioCategory_Media };

AUDIO_STREAM_CATEGORY g_nonOffloadCategories[] = { AudioCategory_Other,       AudioCategory_Communications,
                                                   AudioCategory_Alerts,      AudioCategory_SoundEffects,
                                                   AudioCategory_GameEffects, AudioCategory_GameMedia,
                                                   AudioCategory_GameChat,    AudioCategory_Speech };

AUDIO_STREAM_CATEGORY g_captureCategories[] = { AudioCategory_Other,          AudioCategory_Communications,
                                                AudioCategory_Media,          AudioCategory_GameChat,
                                                AudioCategory_Speech };

AUDIO_STREAM_CATEGORY g_allCategories[] = { AudioCategory_Other,              AudioCategory_Movie,
                                            AudioCategory_Media,              AudioCategory_Communications,
                                            AudioCategory_Alerts,             AudioCategory_SoundEffects,
                                            AudioCategory_GameEffects,        AudioCategory_GameMedia,
                                            AudioCategory_GameChat,           AudioCategory_Speech};
                                            
// Catch changes to the AudioCategory Enum (audiosessiontypes.x)
static_assert(AudioCategory_enum_count == 12, "Enum values changed. Static arrays in " __FILE__ " have to be updated");                                            
                                            
LPCWSTR GetCategoryString( AUDIO_STREAM_CATEGORY category )
{
    switch( category )
    {
        case AudioCategory_Other:
            return L"AudioCategory_Other";

        case AudioCategory_Communications:
            return L"AudioCategory_Communications";

        case AudioCategory_Alerts:
            return L"AudioCategory_Alerts";

        case AudioCategory_SoundEffects:
            return L"AudioCategory_SoundEffects";

        case AudioCategory_GameEffects:
            return L"AudioCategory_GameEffects";

        case AudioCategory_GameMedia:
            return L"AudioCategory_GameMedia";        
            
        case AudioCategory_GameChat:
            return L"AudioCategory_GameChat";
            
        case AudioCategory_Speech:
            return L"AudioCategory_Speech";        
        
        case AudioCategory_Movie:
            return L"AudioCategory_Movie";
            
        case AudioCategory_Media:
            return L"AudioCategory_Media";

        default:
            return L"{NULL}";
    }
}

DWORD TC_AudioClient2_IsOffloadCapable_Invalid()
{
    AudioClient2TCPath testPaths = ISOFFLOADCAPABLE_INVALID_PARAM;
    return RunAudioClient2Test( testPaths );
}

DWORD TC_AudioClient2_SetClientProperties_Invalid()
{
    AudioClient2TCPath testPaths = SETCLIENTPROPERTIES_INVALID_PARAM | SETCLIENTPROPERTIES_INVALID_STATE;
    return RunAudioClient2Test( testPaths );
}

DWORD TC_AudioClient2_GetBufferSizeLimits_Invalid()
{
    AudioClient2TCPath testPaths = GETBUFFERSIZELIMITS_INVALID_PARAM | GETBUFFERSIZELIMITS_INVALID_STATE;
    return RunAudioClient2Test( testPaths );
}

DWORD TC_AudioClient2_IsOffloadCapable_Valid()
{
    CComPtr<IMMDevice> spMMDevice;
    GetEndpointUnderTest(&spMMDevice);

    CClassicOffloadEnabler cor;
    CHardwareAccelerationEnabler har(spMMDevice);

    AudioClient2TCPath testPaths = ISOFFLOADCAPABLE_VALID_PARAM;
    (void) RunAudioClient2Test( testPaths ); // pre-initialize path

    testPaths = INITIALIZE_IN_SHARED | ISOFFLOADCAPABLE_VALID_PARAM;
    (void) RunAudioClient2Test( testPaths );

    testPaths = INITIALIZE_IN_EXCLUSIVE | ISOFFLOADCAPABLE_VALID_PARAM;
    (void) RunAudioClient2Test( testPaths );

    return FNS_PASS;
}

DWORD TC_AudioClient2_SetClientProperties_Valid()
{
    CComPtr<IMMDevice> spMMDevice;
    GetEndpointUnderTest(&spMMDevice);

    CClassicOffloadEnabler cor;
    CHardwareAccelerationEnabler har(spMMDevice);

    AudioClient2TCPath testPaths = SETCLIENTPROPERTIES_VALID_PARAM;
    (void) RunAudioClient2Test( testPaths ); // pre-initialize path

    testPaths = INITIALIZE_IN_SHARED | SETCLIENTPROPERTIES_VALID_PARAM;
    (void) RunAudioClient2Test( testPaths );

    testPaths = INITIALIZE_IN_EXCLUSIVE | SETCLIENTPROPERTIES_VALID_PARAM;
    (void) RunAudioClient2Test( testPaths );

    return FNS_PASS;
}

DWORD TC_AudioClient2_GetBufferSizeLimits_Valid()
{
    CComPtr<IMMDevice> spMMDevice;
    GetEndpointUnderTest(&spMMDevice);

    CClassicOffloadEnabler cor;
    CHardwareAccelerationEnabler har(spMMDevice);

    AudioClient2TCPath testPaths = GETBUFFERSIZELIMITS_VALID_PARAM;
    (void) RunAudioClient2Test( testPaths ); // pre-initialize path

    testPaths = INITIALIZE_IN_SHARED | GETBUFFERSIZELIMITS_VALID_PARAM;
    (void) RunAudioClient2Test( testPaths );

    testPaths = INITIALIZE_IN_EXCLUSIVE | GETBUFFERSIZELIMITS_VALID_PARAM;
    (void) RunAudioClient2Test( testPaths );

    return FNS_PASS;
}

DWORD TC_AudioClient2_TimerDrivenOffload_Invalid()
{
    CComPtr<IMMDevice> spMMDevice;
    GetEndpointUnderTest(&spMMDevice);

    CClassicOffloadEnabler cor;
    CHardwareAccelerationEnabler har(spMMDevice);
    
    AudioClient2TCPath testPaths = TIMERDRIVENOFFLOAD_INVALID_STATE;
    return RunAudioClient2Test(testPaths);
}

//
//
//

DWORD RunAudioClient2Test( AudioClient2TCPath tc )
{
    HRESULT hr = S_OK;
    CComPtr<IMMDevice> spMMDevice;
    AUDIO_STREAM_CATEGORY* categories = nullptr;
    UINT countCategories = 0;
    bool isInitialized = false;

    AudioClient2TCPath localTC = tc & TESTCASE_MASK;

    if( 0 == localTC )
    {
        // we are all done -- no test case path
        return FNS_PASS;
    }
    else if( 0 < (INITIALIZE_IN_SHARED & localTC) && 0 < (INITIALIZE_IN_EXCLUSIVE & localTC) )
    {
        XLOG( XFAIL, eFatalError, "Both Shared and Exclusive Mode Initialization were requested." );
        return FNS_PASS;
    }

    if( !GetDeviceUnderTestAllowsExclusiveMode() &&
        (localTC & INITIALIZE_IN_EXCLUSIVE))
    {
       SLOG( eInfo2, "Skipping Exclusive mode portion of test on Endpoint that doesn't allow exclusive mode" );
        return FNS_PASS;
    }

    hr = GetEndpointUnderTest(&spMMDevice);
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eFatalError, "Error calling GetEndpointUnderTest (0x%8x) [%s]", hr, GetHRString( hr ) );
        return FNS_PASS;
    }

    if( 0 == (INITIALIZE_IN_SHARED & localTC) && 0 == (INITIALIZE_IN_EXCLUSIVE & localTC) )
    {
        // only going to do one loop, stream category doesn't really matter
        categories = g_nonOffloadCategories;

        countCategories = 1;
    }
    else if( DEVICE_TYPE_RENDEROFFLOAD == GetDeviceTypeUnderTest() )
    {
        // Setup valid stream categories for offload
        categories = g_offloadCategories;

        countCategories = ARRAYSIZE(g_offloadCategories);
    }
    else if( DEVICE_TYPE_CAPTURE == GetDeviceTypeUnderTest() )
    {
        // Setup valid stream categories for capture
        categories = g_captureCategories;

        countCategories = ARRAYSIZE(g_captureCategories);
    }
    else
    {
        // fill the array with all stream types
        categories = g_allCategories;

        countCategories = ARRAYSIZE(g_allCategories);
    }

    for( UINT i = 0; i < countCategories; i++ )
    {
        AUDIO_STREAM_CATEGORY category = categories[i];
        localTC = tc & TESTCASE_MASK;
        CComPtr<IAudioClient2> spAudioClient2;
        
        hr = spMMDevice->Activate( __uuidof(IAudioClient2), CLSCTX_ALL, NULL, (void **)&spAudioClient2 );
        if( S_OK != hr )
        {
            XLOG( XFAIL, eFatalError, "Error calling IMMDevice::Activate(IAudioClient2) (0x%8x) [%s]", hr, GetHRString( hr ) );

            return FNS_PASS;
        }

        // Run the tests
        for( int currentTest = 0; localTC > 0; localTC = localTC >> 1, currentTest++ )
        {
            if( 0 < (localTC & 0x1) )
            {
                switch( 1 << currentTest )
                {
                    case INITIALIZE_IN_SHARED:
                        (void) InitializeInSharedMode( spAudioClient2, (AUDIO_STREAM_CATEGORY)category, isInitialized );
                        break;

                    case INITIALIZE_IN_EXCLUSIVE:
                        (void) InitializeInExclusiveMode( spAudioClient2, spMMDevice, (AUDIO_STREAM_CATEGORY)category, isInitialized );
                        break;

                    case ISOFFLOADCAPABLE_INVALID_PARAM:
                        (void) IsOffloadCapable_InvalidParamsTest( spAudioClient2, spMMDevice );
                        break;

                    case ISOFFLOADCAPABLE_VALID_PARAM:
                        (void) IsOffloadCapable_ValidParamsTest( spAudioClient2, spMMDevice );
                        break;

                    case SETCLIENTPROPERTIES_INVALID_PARAM:
                        (void) SetClientProperties_InvalidParamsTest( spAudioClient2, spMMDevice );
                        break;

                    case SETCLIENTPROPERTIES_INVALID_STATE:
                        (void) SetClientProperties_InvalidStateTest( spAudioClient2 );
                        break;

                    case SETCLIENTPROPERTIES_VALID_PARAM:
                        (void) SetClientProperties_ValidParamsTest( spAudioClient2, isInitialized, spMMDevice );
                        break;

                    case GETBUFFERSIZELIMITS_INVALID_PARAM:
                        (void) GetBufferSizeLimits_InvalidParamsTest( spAudioClient2 );
                        break;

                    case GETBUFFERSIZELIMITS_INVALID_STATE:
                        (void) GetBufferSizeLimits_InvalidStateTest( spAudioClient2 );
                        break;

                    case GETBUFFERSIZELIMITS_VALID_PARAM:
                        (void) GetBufferSizeLimits_ValidParamsTest( tc, spAudioClient2, isInitialized );
                        break;

                    case TIMERDRIVENOFFLOAD_INVALID_STATE:
                        (void) TimerDrivenOffload_InvalidStateTest( spAudioClient2 );
                        break;

                    default:
                        // using TESTCASE_MASK ensures tests are within range
                        break;

                }
            }
        }
    }

    return FNS_PASS;
}

//
//
//

DWORD InitializeInSharedMode( IAudioClient2* pAudioClient2, AUDIO_STREAM_CATEGORY streamCategory, bool& isInitialized )
{
    HRESULT hr = S_OK;
    CComHeapPtr<WAVEFORMATEX> pWfx;
    const REFERENCE_TIME HNS_TWENTY_MS = 200000;
    REFERENCE_TIME hnsBufferDuration = HNS_TWENTY_MS;
    REFERENCE_TIME hnsPeriodicity = 0;
    AudioClientProperties clientProperties = {0};
    HRESULT hrExpectedSetClientProperties = S_OK;
    DWORD dwStreamFlags = 0x0;

    isInitialized = false;

    clientProperties.eCategory = streamCategory;
    clientProperties.cbSize = sizeof( clientProperties );

    if( DEVICE_TYPE_RENDEROFFLOAD == GetDeviceTypeUnderTest() )
    {
        // Setup for the offload path
        clientProperties.bIsOffload = TRUE;
        dwStreamFlags = AUDCLNT_STREAMFLAGS_EVENTCALLBACK;
        hrExpectedSetClientProperties = S_OK;
        hnsPeriodicity = hnsBufferDuration;
    }
    else
    {
        clientProperties.bIsOffload = FALSE;
    }

    METHODCHECKRESULT( "IAudioClient2::SetClientProperties", hr, pAudioClient2->SetClientProperties( &clientProperties ) );
    if( hrExpectedSetClientProperties != hr )
    {
        XLOG( XFAIL, eError, "FAIL: Expected SetClientProperties to return 0x%08x [%s].  Actual 0x%08x [%s]",
            hrExpectedSetClientProperties, GetHRString(hrExpectedSetClientProperties), hr, GetHRString( hr ) );

        return FNS_PASS;
    }

    // Get the mix format to use for initialize
    METHODCHECKRESULT( "IAudioClient2::GetMixFormat", hr, pAudioClient2->GetMixFormat( &pWfx ) );
    if( S_OK != hr )
    {
        XLOG( XFAIL, eError, "FAIL: Expected GetMixFormat to return 0x%08x [S_OK].  Actual 0x%08x [%s]",
            S_OK, hr, GetHRString( hr ) );

        return FNS_PASS;
    }

    // Now, Initialize a shared mode stream (20ms buffer)
    METHODCHECKRESULT( "IAudioClient2::Initialize", hr,
        pAudioClient2->Initialize( AUDCLNT_SHAREMODE_SHARED, dwStreamFlags, hnsBufferDuration, hnsPeriodicity, pWfx, NULL ) );
    if( S_OK != hr )
    {
        XLOG( XFAIL, eError, "FAIL: Expected Initialize to return 0x%08x [S_OK].  Actual 0x%08x [%s]",
            S_OK, hr, GetHRString( hr ) );

        return FNS_PASS;
    }
    else
    {
       SLOG( eInfo2, "IAudioClient2 Initialized in Shared Mode (bIsOffload: %s, category: %S)",
            clientProperties.bIsOffload == FALSE ? "FALSE" : "TRUE", GetCategoryString( clientProperties.eCategory ) );

        isInitialized = true;
    }

    return FNS_PASS;
}

DWORD InitializeInExclusiveMode( IAudioClient2* pAudioClient2, IMMDevice* pMMDevice, AUDIO_STREAM_CATEGORY streamCategory,
    bool& isInitialized )
{
    HRESULT hr = S_OK;
    CComHeapPtr<WAVEFORMATEX> spWfx;
    const REFERENCE_TIME HNS_TWENTY_MS = 200000;
    AudioClientProperties clientProperties = {0};

    isInitialized = false;

    clientProperties.eCategory = streamCategory;
    clientProperties.cbSize = sizeof(clientProperties);

    // offload flag must be FALSE for initializing an exclusive mode stream
    clientProperties.bIsOffload = FALSE;

    METHODCHECKRESULT( "IAudioClient2::SetClientProperties", hr, pAudioClient2->SetClientProperties( &clientProperties ) );
    if( S_OK != hr )
    {
        XLOG( XFAIL, eError, "FAIL: Expected SetClientProperties to return 0x%08x [S_OK].  Actual 0x%08x [%s]",
            S_OK, hr, GetHRString( hr ) );

        return FNS_PASS;
    }

    hr = GetExclusiveModeFormat( pAudioClient2, pMMDevice, &spWfx );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eError, "FAIL: Unable to get a format for exclusive mode.  HRESULT: 0x%08x", hr );

        return FNS_PASS;
    }

    // Now, Initialize an exclusive mode stream (20ms buffer)
    METHODCHECKRESULT( "IAudioClient2::Initialize", hr,
        pAudioClient2->Initialize( AUDCLNT_SHAREMODE_EXCLUSIVE, 0, HNS_TWENTY_MS, 0, spWfx, NULL ) );
    if( S_OK != hr )
    {
        XLOG( XFAIL, eError, "FAIL: Expected Initialize to return 0x%08x [S_OK].  Actual 0x%08x [%s]",
            S_OK, hr, GetHRString( hr ) );

        return FNS_PASS;
    }
    else
    {
       SLOG(eInfo2, "IAudioClient2 Initialized in Exclusive Mode (bIsOffload: %s, category: %S)",
            clientProperties.bIsOffload == FALSE ? "FALSE" : "TRUE", GetCategoryString( clientProperties.eCategory ) );

        isInitialized = true;
    }

    return FNS_PASS;
}

DWORD IsOffloadCapable_InvalidParamsTest( IAudioClient2* pAudioClient2, IMMDevice* pMMDevice )
{
    HRESULT hr = S_OK;
    bool doesDeviceSupportOffload = false;
    BOOL closeKsLib = FALSE;
    BOOL isOffloadCapable = FALSE;

    closeKsLib = KslInitKsLib();
    if( FALSE == closeKsLib )
    {
        XLOG( XFAIL, eFatalError, "Error calling KslInitKsLib" );

        goto END;
    }

    hr = CheckDeviceSupportsOffload( pMMDevice, AudioOffload::MMDEV_API, doesDeviceSupportOffload );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eFatalError, "Error calling CheckDeviceSupportsOffload with MMDEV_API (0x%8x) [%s]", hr, GetHRString( hr ) );

        goto END;
    }

    // set the BOOL passed to IsOffloadCapable to the opposite
    // value we would expect when called correctly
    if( doesDeviceSupportOffload )
    {
        isOffloadCapable = FALSE;
    }
    else
    {
        isOffloadCapable = TRUE;
    }

    // nullptr parameter
#pragma prefast( suppress:__WARNING_PASSING_FUNCTION_UNEXPECTED_NULL, "Testing API parameter handling; Intentionally disabling prefast parameter checking" )
    METHODCHECKRESULT( "IAudioClient2::IsOffloadCapable", hr, pAudioClient2->IsOffloadCapable( AudioCategory_Media,
        nullptr ) );
    if( E_INVALIDARG != hr ) 
    {
        XLOG( XFAIL, eError, "FAIL: Expected IsOffloadCapable to return 0x%08x [E_INVALIDARG] when called with nullptr.  Actual 0x%08x [%s]",
            E_INVALIDARG, hr, GetHRString( hr ) );
    }

    // Stream Category of -1
    METHODCHECKRESULT( "IAudioClient2::IsOffloadCapable", hr, pAudioClient2->IsOffloadCapable( (AUDIO_STREAM_CATEGORY)-1,
        &isOffloadCapable ) );
    if( E_INVALIDARG != hr ) 
    {
        XLOG( XFAIL, eError, "FAIL: Expected IsOffloadCapable() to return 0x%08x [E_INVALIDARG] when called with AUDIO_STREAM_CATEGORY of -1.  Actual 0x%08x [%s]",
            E_INVALIDARG, hr, GetHRString( hr ) );
    }

    // ensure the BOOL was not modified
    if( doesDeviceSupportOffload && FALSE != isOffloadCapable )
    {
        XLOG( XFAIL, eError, "FAIL: Expected IsOffloadCapable(category=-1) to not modify the value of the BOOL when passed invalid parameters.  BOOL was FALSE(%u), now (%u)",
            FALSE, isOffloadCapable );

        // reset for next test
        isOffloadCapable = FALSE;
    }
    else if( !doesDeviceSupportOffload && FALSE == isOffloadCapable )
    {
        XLOG( XFAIL, eError, "FAIL: Expected IsOffloadCapable(category=-1) to not modify the value of the BOOL when passed invalid parameters.  BOOL was TRUE(%u), now (%u)",
            TRUE, isOffloadCapable );

        // reset for next test
        isOffloadCapable = TRUE;
    }

    // Stream Category of (one over last enumerated type value)
    METHODCHECKRESULT( "IAudioClient2:IsOffloadCapable", hr, pAudioClient2->IsOffloadCapable(
        (AUDIO_STREAM_CATEGORY)AudioCategory_enum_count, &isOffloadCapable ) );
    if( E_INVALIDARG != hr ) 
    {
        XLOG( XFAIL, eError, "FAIL: Expected IsOffloadCapable to return 0x%08x [E_INVALIDARG] when called with AUDIO_STREAM_CATEGORY of LAST_CATEGORY+1.  Actual 0x%08x [%s]",
            E_INVALIDARG, hr, GetHRString( hr ) );
    }

    // ensure the BOOL was not modified
    if( doesDeviceSupportOffload && FALSE != isOffloadCapable )
    {
        XLOG( XFAIL, eError, "FAIL: Expected IsOffloadCapable(category=LAST_CATEGORY+1) to not modify the value of the BOOL when passed invalid parameters.  BOOL was FALSE(%u), now (%u)",
            FALSE, isOffloadCapable );
    }
    else if( !doesDeviceSupportOffload && FALSE == isOffloadCapable )
    {
        XLOG( XFAIL, eError, "FAIL: Expected IsOffloadCapable(category=LAST_CATEGORY+1) to not modify the value of the BOOL when passed invalid parameters.  BOOL was TRUE(%u), now (%u)",
            TRUE, isOffloadCapable );
    }

END:

    if( FALSE != closeKsLib )
    {
        (void) KslCloseKsLib();
    }

    return FNS_PASS;
}

DWORD SetClientProperties_InvalidParamsTest( IAudioClient2* pAudioClient2, IMMDevice* pMMDevice )
{
    HRESULT hr = S_OK;
    BOOL isOffloadCapable = FALSE;
    AudioClientProperties clientProperties = {0};
    clientProperties.bIsOffload = FALSE;
    clientProperties.eCategory = AudioCategory_Media;
    clientProperties.cbSize = sizeof(clientProperties);

    METHODCHECKRESULT( "IAudioClient2::IsOffloadCapable", hr, pAudioClient2->IsOffloadCapable( AudioCategory_Media,
        &isOffloadCapable ) );
    if( S_OK != hr )
    {
        XLOG( XFAIL, eError, "FAIL: Expected IsOffloadCapable to return 0x%08x [S_OK].  Actual 0x%08x [%s]",
            S_OK, hr, GetHRString(hr) );

        return FNS_PASS;
    }

    // nullptr parameter
#pragma prefast( suppress:__WARNING_PASSING_FUNCTION_UNEXPECTED_NULL, "Testing API parameter handling; Intentionally disabling prefast parameter checking" )
    METHODCHECKRESULT( "IAudioClient2::IsOffloadCapable", hr, pAudioClient2->SetClientProperties( nullptr ) );
    if( E_POINTER != hr ) 
    {
        XLOG( XFAIL, eError, "FAIL: Expected SetClientProperties to return 0x%08x [E_POINTER] when called with nullptr.  Actual 0x%08x [%s]",
            E_POINTER, hr, GetHRString( hr ) );
    }

    // AudioClientProperties.cbSize of 0
    clientProperties.cbSize = 0;
    METHODCHECKRESULT( "IAudioClient2::SetClientProperties", hr, pAudioClient2->SetClientProperties( &clientProperties ) );
    if( E_INVALIDARG != hr )
    {
        XLOG( XFAIL, eError, "FAIL: Expected SetClientProperties() to return 0x%08x [E_INVALIDARG] when called with AudioClientProperties Size of 0.  Actual 0x%08x [%s]",
            E_INVALIDARG, hr, GetHRString( hr ) );
    }

    // AudioClientProperties.cbSize of sizeof(AudioClientProperties) + 1
    clientProperties.cbSize = sizeof(clientProperties) + 1;
    METHODCHECKRESULT( "IAudioClient2::SetClientProperties", hr, pAudioClient2->SetClientProperties( &clientProperties ) );
    if( E_INVALIDARG != hr ) 
    {
        XLOG( XFAIL, eError, "FAIL: Expected SetClientProperties() to return 0x%08x [E_INVALIDARG] when called with AudioClientProperties Size of AudioClientProperties + 1.  Actual 0x%08x [%s]",
            E_INVALIDARG, hr, GetHRString( hr ) );
    }

    // AudioClientProperties.cbSize of sizeof(AudioClientProperties) - 1
    clientProperties.cbSize = sizeof(clientProperties) - 1;
    METHODCHECKRESULT( "IAudioClient2::SetClientProperties", hr, pAudioClient2->SetClientProperties( &clientProperties ) );
    if( E_INVALIDARG != hr ) 
    {
        XLOG( XFAIL, eError, "FAIL: Expected SetClientProperties() to return 0x%08x [E_INVALIDARG] when called with AudioClientProperties Size of AudioClientProperties - 1.  Actual 0x%08x [%s]",
            E_INVALIDARG, hr, GetHRString( hr ) );
    }

    clientProperties.cbSize = sizeof(clientProperties);

    // AudioClientProperties.eCategory of -1
    clientProperties.eCategory = (AUDIO_STREAM_CATEGORY)-1;
    METHODCHECKRESULT( "IAudioClient2::SetClientProperties", hr, pAudioClient2->SetClientProperties( &clientProperties ) );
    if( E_INVALIDARG != hr ) 
    {
        XLOG( XFAIL, eError, "FAIL: Expected SetClientProperties() to return 0x%08x [E_INVALIDARG] when called with AudioClientProperties Category of -1.  Actual 0x%08x [%s]",
            E_INVALIDARG, hr, GetHRString( hr ) );
    }

    // AudioClientProperties.eCategory of LAST_CATEGORY + 1
    clientProperties.eCategory = (AUDIO_STREAM_CATEGORY)AudioCategory_enum_count;
    METHODCHECKRESULT( "IAudioClient2::SetClientProperties", hr, pAudioClient2->SetClientProperties( &clientProperties ) );
    if( E_INVALIDARG != hr ) 
    {
        XLOG( XFAIL, eError, "FAIL: Expected SetClientProperties() to return 0x%08x [E_INVALIDARG] when called with AudioClientProperties Category of LAST_CATEGORY + 1.  Actual 0x%08x [%s]",
            E_INVALIDARG, hr, GetHRString( hr ) );
    }

    // AudioClientProperties.bIsOffload to TRUE with non-Offload Capable eCategory
    if( TRUE == isOffloadCapable )
    {
        clientProperties.bIsOffload = TRUE;
        for( UINT i = 0; i < ARRAYSIZE(g_nonOffloadCategories); i++ )
        {
            clientProperties.eCategory = g_nonOffloadCategories[i];
            METHODCHECKRESULT( "IAudioClient2::SetClientProperties", hr, pAudioClient2->SetClientProperties( &clientProperties ) );
            if( AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE != hr )
            {
                XLOG( XFAIL, eError, "FAIL: Expected SetClientProperties() to return 0x%08x [AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE] when called with Is Offload TRUE and a Category (%S) that does not support Audio Offload.  Actual 0x%08x [%s]",
                    AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE, GetCategoryString( g_nonOffloadCategories[i] ), hr, GetHRString( hr ) );
            }
        }
    }

// TODO: Change to _WIN32_WINNT_X when it's available
#if (WINVER < _WIN32_WINNT_WIN8)     
    return FNS_PASS;
#else
    BOOL bRawModeSupported = FALSE;

    // Check if device supports Raw Mode
    hr = CheckDeviceSupportsRawMode(pMMDevice, &bRawModeSupported);
    if( S_OK != hr )
    {
        XLOG( XFAIL, eError, "FAIL: Expected CheckDeviceSupportsRawMode to return 0x%08x [S_OK].  Actual 0x%08x [%s]", S_OK, hr, GetHRString(hr) );
    }

    if( FALSE == bRawModeSupported )
    { 
        // Set it to Raw mode even that is not supported
        clientProperties.Options = AUDCLNT_STREAMOPTIONS_RAW;

        // Test with offload off    
        clientProperties.bIsOffload = FALSE;
        clientProperties.eCategory = g_nonOffloadCategories[0];
        METHODCHECKRESULT( "IAudioClient2::SetClientProperties", hr, pAudioClient2->SetClientProperties( &clientProperties ) );
        if( AUDCLNT_E_RAW_MODE_UNSUPPORTED != hr )
        {
            XLOG( XFAIL, eError, "FAIL: Expected SetClientProperties() to return 0x%08x [AUDCLNT_E_RAW_MODE_UNSUPPORTED] when called with Option set to AUDCLNT_STREAMOPTIONS_RAW when the driver does not support raw mode.  Actual 0x%08x [%s]",
                AUDCLNT_E_RAW_MODE_UNSUPPORTED, hr, GetHRString( hr ) );
        }

        if( TRUE == isOffloadCapable )
        {
            // If device supports offload test here also
            clientProperties.bIsOffload = TRUE;
            clientProperties.eCategory = g_offloadCategories[0];
            METHODCHECKRESULT( "IAudioClient2::SetClientProperties", hr, pAudioClient2->SetClientProperties( &clientProperties ) );
            if( AUDCLNT_E_RAW_MODE_UNSUPPORTED != hr )
            {
                XLOG( XFAIL, eError, "FAIL: Expected SetClientProperties() to return 0x%08x [AUDCLNT_E_RAW_MODE_UNSUPPORTED] when called with Option set to AUDCLNT_STREAMOPTIONS_RAW when the driver does not support raw mode.  Actual 0x%08x [%s]",
                    AUDCLNT_E_RAW_MODE_UNSUPPORTED, hr, GetHRString( hr ) );
            }
        }
    }
    return FNS_PASS;
#endif
}

DWORD GetBufferSizeLimits_InvalidParamsTest( IAudioClient2* pAudioClient2 )
{
    HRESULT hr = S_OK;
    BOOL isOffloadCapable = FALSE;
    AudioClientProperties clientProperties = { 0 };
    REFERENCE_TIME refTime = 0;
    WAVEFORMATEX wfx = { 0 };

    clientProperties.bIsOffload = TRUE;
    clientProperties.eCategory = AudioCategory_Media;
    clientProperties.cbSize = sizeof(AudioClientProperties);

    METHODCHECKRESULT( "IAudioClient2::IsOffloadCapable", hr, pAudioClient2->IsOffloadCapable( AudioCategory_Media,
        &isOffloadCapable ) );
    if( S_OK != hr )
    {
        XLOG( XFAIL, eError, "FAIL: Expected IsOffloadCapable to return 0x%08x [S_OK].  Actual 0x%08x [%s]",
            S_OK, hr, GetHRString(hr) );

        return FNS_PASS;
    }

    if( FALSE == isOffloadCapable )
    {
        // will only validate parameters on an offload capable endpoint
        // otherwise, the method will just return AUDCLNT_E_OFFLOAD_MODE_ONLY
        // that case is tested in GetBufferSizeLimits_InvalidStateTest
        return FNS_PASS;
    }

    METHODCHECKRESULT( "IAudioClient2::SetClientProperties", hr, pAudioClient2->SetClientProperties( &clientProperties ) );
    if( S_OK != hr )
    {
        XLOG( XFAIL, eError, "FAIL: Expected SetClientProperties to return 0x%08x [S_OK].  Actual 0x%08x [%s]",
            S_OK, hr, GetHRString(hr) );

        return FNS_PASS;
    }

    // nullptr format parameter
#pragma prefast( suppress:__WARNING_PASSING_FUNCTION_UNEXPECTED_NULL, "Testing API parameter handling; Intentionally disabling prefast parameter checking" )
    METHODCHECKRESULT( "IAudioClient2::GetBufferSizeLimits", hr,
        pAudioClient2->GetBufferSizeLimits( nullptr, false, &refTime, &refTime ) );
    if( E_POINTER != hr )
    {
        XLOG( XFAIL, eError, "FAIL: Expected GetBufferSizeLimits to return 0x%08x [E_POINTER] when called with a nullptr format.  Actual 0x%08x [%s]",
            E_POINTER, hr, GetHRString( hr ) );
    }

    // nullptr min buffer parameter
#pragma prefast( suppress:__WARNING_PASSING_FUNCTION_UNEXPECTED_NULL, "Testing API parameter handling; Intentionally disabling prefast parameter checking" )
    METHODCHECKRESULT( "IAudioClient2::GetBufferSizeLimits", hr,
        pAudioClient2->GetBufferSizeLimits( &wfx, false, nullptr, &refTime ) );
    if( E_POINTER != hr )
    {
        XLOG( XFAIL, eError, "FAIL: Expected GetBufferSizeLimits to return 0x%08x [E_POINTER] when called with a nullptr min buffer.  Actual 0x%08x [%s]",
            E_POINTER, hr, GetHRString( hr ) );
    }

    // nullptr max buffer parameter
#pragma prefast( suppress:__WARNING_PASSING_FUNCTION_UNEXPECTED_NULL, "Testing API parameter handling; Intentionally disabling prefast parameter checking" )
    METHODCHECKRESULT( "IAudioClient2::GetBufferSizeLimits", hr,
        pAudioClient2->GetBufferSizeLimits( &wfx, false, &refTime, nullptr ) );
    if( E_POINTER != hr )
    {
        XLOG( XFAIL, eError, "FAIL: Expected GetBufferSizeLimits to return 0x%08x [E_POINTER] when called with a nullptr max buffer.  Actual 0x%08x [%s]",
            E_POINTER, hr, GetHRString( hr ) );
    }

    // reset client properties
    clientProperties.bIsOffload = FALSE;
    clientProperties.eCategory = AudioCategory_Other;
    METHODCHECKRESULT( "IAudioClient2::SetClientProperties", hr, pAudioClient2->SetClientProperties( &clientProperties ) );
    if( S_OK != hr )
    {
        XLOG( XFAIL, eError, "FAIL: Expected SetClientProperties to return 0x%08x [S_OK].  Actual 0x%08x [%s]",
            S_OK, hr, GetHRString(hr) );

        return FNS_PASS;
    }

    return FNS_PASS;
}

DWORD SetClientProperties_InvalidStateTest( IAudioClient2* pAudioClient2 )
{
    HRESULT hr = S_OK;
    BOOL isOffloadCapable = FALSE;
    AudioClientProperties clientProperties = {0};
    clientProperties.bIsOffload = TRUE;
    clientProperties.cbSize = sizeof(clientProperties);

    // Make sure that there is no offload endpoint on this device
    METHODCHECKRESULT( "IAudioClient2::IsOffloadCapable", hr, pAudioClient2->IsOffloadCapable( AudioCategory_Media, &isOffloadCapable ) );
    if( S_OK != hr )
    {
        XLOG( XFAIL, eFatalError, "Error calling IAudioClient2::IsOffloadCapable (0x%8x) [%s]", hr, GetHRString( hr ) );

        return FNS_PASS;
    }

    // Set bIsOffload TRUE for Capture and non-offload Render Endpoints
    if( FALSE == isOffloadCapable )
    {
        for( UINT i = 0; i < ARRAYSIZE(g_allCategories); i++ )
        {
            clientProperties.eCategory = g_allCategories[i];
            METHODCHECKRESULT( "IAudioClient2::SetClientProperties", hr, pAudioClient2->SetClientProperties( &clientProperties ) );
            if( AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE != hr )
            {
                XLOG( XFAIL, eFatalError, "FAIL: Expected SetClientProperties(category=%S) to return 0x%08x [AUDCLNT_E_ENDPOINT_NOT_CAPABLE].  Actual (0x%8x) [%s]",
                    GetCategoryString( g_allCategories[i] ), AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE, hr, GetHRString( hr ) );

                return FNS_PASS;
            }
        }
    }

    return FNS_PASS;
}

DWORD GetBufferSizeLimits_InvalidStateTest( IAudioClient2* pAudioClient2 )
{
    HRESULT hr = S_OK;
    BOOL isOffloadCapable = FALSE;
    CComHeapPtr<WAVEFORMATEX> spWfx;
    REFERENCE_TIME minRefTime = 0;
    REFERENCE_TIME maxRefTime = 0;

    // See if there is an offload endpoint on this device (device topology)
    METHODCHECKRESULT( "IAudioClient2::IsOffloadCapable", hr, pAudioClient2->IsOffloadCapable( AudioCategory_Media,
        &isOffloadCapable ) );
    if( S_OK != hr )
    {
        XLOG( XFAIL, eFatalError, "Error calling IAudioClient2::IsOffloadCapable (0x%8x) [%s]", hr, GetHRString( hr ) );

        return FNS_PASS;
    }

    // Get buffer sizes for Capture and non-offload Render Endpoints
    if( FALSE == isOffloadCapable )
    {
        METHODCHECKRESULT( "IAudioClient2::GetMixFormat", hr, pAudioClient2->GetMixFormat( &spWfx ) );
        if( S_OK != hr )
        {
            XLOG( XFAIL, eFatalError, "Error calling IAudioClient2::GetMixFormat (0x%8x) [%s]", hr, GetHRString( hr ) );

            return FNS_PASS;
        }

        METHODCHECKRESULT( "IAudioClient2::GetBufferSizeLimits", hr,
            pAudioClient2->GetBufferSizeLimits( spWfx, false, &minRefTime, &maxRefTime ) );
        if( AUDCLNT_E_OFFLOAD_MODE_ONLY != hr )
        {
            XLOG( XFAIL, eError, "FAIL: Expected GetBufferSizeLimits to return 0x%08x [AUDCLNT_E_OFFLOAD_MODE_ONLY].  Actual 0x%08x [%s]",
                AUDCLNT_E_OFFLOAD_MODE_ONLY, hr, GetHRString( hr ) );

            return FNS_PASS;
        }
    }

    return FNS_PASS;
}

DWORD TimerDrivenOffload_InvalidStateTest( IAudioClient2* pAudioClient2 ) {
    HRESULT hr = S_OK;
    CComHeapPtr<WAVEFORMATEX> pWfx;
    AudioClientProperties clientProperties = {};
    const REFERENCE_TIME HNS_TWENTY_MS = 200000;
    REFERENCE_TIME hnsBufferDuration = HNS_TWENTY_MS;

    clientProperties.cbSize = sizeof(clientProperties);
    clientProperties.bIsOffload = TRUE;
    clientProperties.eCategory = AudioCategory_Media;

    // opt into offload mode
    METHODCHECKRESULT("IAudioClient2::SetClientProperties", hr, pAudioClient2->SetClientProperties(&clientProperties));
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: Expected IAudioClient2::SetClientProperties() to return S_OK.  Actual 0x%08x [%s]", hr, GetHRString(hr));
        return FNS_PASS;
    }

    // Get the mix format to use for initialize
    METHODCHECKRESULT("IAudioClient2::GetMixFormat", hr, pAudioClient2->GetMixFormat(&pWfx));
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, "FAIL: Expected GetMixFormat to return S_OK.  Actual 0x%08x [%s]",
            hr, GetHRString(hr));

        return FNS_PASS;
    }

    // timer-driven initialize
    METHODCHECKRESULT(
        "IAudioClient2::Initialize",
        hr,
        pAudioClient2->Initialize(
            AUDCLNT_SHAREMODE_SHARED,
            0, // intentionally not setting event-driven flag
            hnsBufferDuration,
            0,
            pWfx,
            nullptr
        )
    );
    if (AUDCLNT_E_NONOFFLOAD_MODE_ONLY != hr)
    {
        XLOG(XFAIL, eError, "FAIL: Expected IAudioClient::Initialize() to return AUDCLNT_E_NONOFFLOAD_MODE_ONLY.  Actual 0x%08x [%s]", hr, GetHRString(hr));
        return FNS_PASS;
    }

    return FNS_PASS;
}

DWORD IsOffloadCapable_ValidParamsTest( IAudioClient2* pAudioClient2, IMMDevice* pMMDevice )
{
    HRESULT hr = S_OK;
    CComPtr<IBasicLog> pBasicLog;
    bool doesDeviceSupportOffload = false;
    bool doesKsSupportOffload = false;
    BOOL closeKsLib = FALSE;
    BOOL isOffloadCapable = (BOOL)-1;

    closeKsLib = KslInitKsLib();
    if( FALSE == closeKsLib )
    {
        XLOG( XFAIL, eFatalError, "Error calling KslInitKsLib" );

        goto END;
    }

    // Get an IBasicLog to pass to the AudioOffload methods to provide info on failures
    hr = g_IShell->GetBasicLog( &pBasicLog );
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eFatalError, "Error calling ITestShell::GetBasicLog (0x%8x) [%s]", hr, GetHRString( hr ) );

        goto END;
    }

    hr = CheckDeviceSupportsOffload(pMMDevice, AudioOffload::MMDEV_API, doesDeviceSupportOffload);
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eFatalError, "Error calling CheckDeviceSupportsOffload with MMDEV_API (0x%8x) [%s]", hr, GetHRString( hr ) );

        goto END;
    }

    hr = CheckDeviceSupportsOffload(pMMDevice, AudioOffload::KS_API, doesKsSupportOffload);
    if( FAILED(hr) )
    {
        XLOG( XFAIL, eFatalError, "Error calling CheckDeviceSupportsOffload with KS_API (0x%8x) [%s]", hr, GetHRString( hr ) );

        goto END;
    }
    
    // validate device topology
    if( doesDeviceSupportOffload && !doesKsSupportOffload )
    {
        XLOG( XFAIL, eError, "FAIL: Device Topology shows support for Offload, but KS Topology does not" );

        goto END;
    }
    else if( !doesDeviceSupportOffload && doesKsSupportOffload )
    {
        XLOG( XWARN, LogLevel_Info, "WARN: KS Topology shows support for Offload, but Device Topology does not.  Check driver/hardware." );
    }

    for( UINT catIndex = 0; catIndex < ARRAYSIZE(g_allCategories); catIndex++ )
    {
        isOffloadCapable = (BOOL)-1;
        METHODCHECKRESULT( "IAudioClient2::IsOffloadCapable", hr, pAudioClient2->IsOffloadCapable(
            g_allCategories[catIndex], &isOffloadCapable ) );
        if( S_OK != hr )
        {
            XLOG( XFAIL, eError, "FAIL: Expected IsOffloadCapable(category=%S) to return 0x%08x [S_OK].  Actual 0x%08x [%s]",
                GetCategoryString( g_allCategories[catIndex] ), S_OK, hr, GetHRString(hr) );

            goto END;
        }

        if( TRUE != isOffloadCapable && FALSE != isOffloadCapable )
        {
            XLOG( XFAIL, eError, "FAIL: Expected IsOffloadCapable(category=%S) to assign BOOL value of TRUE(%u) or FALSE(%u).  Actual %u",
                GetCategoryString( g_allCategories[catIndex] ), TRUE, FALSE, isOffloadCapable );

            continue;
        }

        // See if IsOffloadCapable returned the correct value
        if( doesDeviceSupportOffload )
        {
            switch( g_allCategories[catIndex] )
            {
                // BOOL should be TRUE
                case AudioCategory_Movie:
                case AudioCategory_Media:
                    if( TRUE != isOffloadCapable )
                    {
                        XLOG( XFAIL, eError, "FAIL: Expected IsOffloadCapable(category=%S) to assign BOOL value of TRUE (%u).  Actual %u",
                            GetCategoryString( g_allCategories[catIndex] ), TRUE, isOffloadCapable );
                    }
                    break;

                // BOOL should be FALSE
                case AudioCategory_Other:
                case AudioCategory_Communications:
                case AudioCategory_Alerts:
                case AudioCategory_SoundEffects:
                case AudioCategory_GameEffects:
                case AudioCategory_GameMedia:
                case AudioCategory_GameChat:
                case AudioCategory_Speech:

                    if( FALSE != isOffloadCapable )
                    {
                        XLOG( XFAIL, eError, "FAIL: Expected IsOffloadCapable(category=%S) to assign BOOL value of FALSE (%u).  Actual %u",
                            GetCategoryString( g_allCategories[catIndex] ), FALSE, isOffloadCapable );
                    }
                    break;

                default:
                    // in theory, we will never get here
                    XLOG( XFAIL, eError, "FAIL: Reached Unknown Audio Stream Category %u", g_allCategories[catIndex] );
                    break;

            }
        }
        else if( FALSE != isOffloadCapable )
        {
            XLOG( XFAIL, eError, "FAIL: Expected IsOffloadCapable(category=%S) to assign BOOL value of FALSE (%u) when Endpoint does not support hardware offload.  Actual %u",
                GetCategoryString( g_allCategories[catIndex] ), FALSE, isOffloadCapable );
        }
    }

END:

    if( FALSE != closeKsLib )
    {
        (void) KslCloseKsLib();
    }

    return FNS_PASS;
}

DWORD SetClientProperties_ValidParamsTest( IAudioClient2* pAudioClient2, bool isInitialized, IMMDevice* pMMDevice )
{
    HRESULT hr = S_OK;
    BOOL isOffloadCapable = FALSE;
    AudioClientProperties clientProperties = {0};

    // See if IAudioClient2 says the endpoint is offload capable
    METHODCHECKRESULT( "IAudioClient2::IsOffloadCapable", hr, pAudioClient2->IsOffloadCapable( AudioCategory_Media,
        &isOffloadCapable ) );
    if( S_OK != hr )
    {
        XLOG( XFAIL, eError, "FAIL: Expected IsOffloadCapable to return 0x%08x [S_OK].  Actual 0x%08x [%s]", S_OK, hr, GetHRString(hr) );
    }

    // Test Is Offload False with each stream category
    clientProperties.bIsOffload = FALSE;
    clientProperties.cbSize = sizeof(clientProperties);
    for( UINT i = 0; i < ARRAYSIZE(g_allCategories); i++ )
    {
        clientProperties.eCategory = g_allCategories[i];

        METHODCHECKRESULT( "IAudioClient2::SetClientProperties", hr, pAudioClient2->SetClientProperties( &clientProperties ) );
        if( isInitialized && AUDCLNT_E_ALREADY_INITIALIZED != hr )
        {
            XLOG( XFAIL, eError, "FAIL: Expected SetClientProperties(category=%S) to return 0x%08x [AUDCLNT_E_ALREADY_INITIALIZED].  Actual 0x%08x [%s]",
                GetCategoryString( g_allCategories[i] ), AUDCLNT_E_ALREADY_INITIALIZED, hr, GetHRString(hr) );
        }
        else if( !isInitialized && S_OK != hr )
        {
            XLOG( XFAIL, eError, "FAIL: Expected SetClientProperties(category=%S) to return 0x%08x [S_OK].  Actual 0x%08x [%s]",
                GetCategoryString( g_allCategories[i] ), S_OK, hr, GetHRString(hr) );
        }
    }

    // Test Is Offload True with each capable stream category
    if( TRUE == isOffloadCapable )
    {
        clientProperties.bIsOffload = TRUE;
        for( UINT i = 0; i < ARRAYSIZE(g_offloadCategories); i++ )
        {
            clientProperties.eCategory = g_offloadCategories[i];

            METHODCHECKRESULT( "IAudioClient2::SetClientProperties", hr, pAudioClient2->SetClientProperties( &clientProperties ) );
            if( isInitialized && AUDCLNT_E_ALREADY_INITIALIZED != hr )
            {
                XLOG( XFAIL, eError, "FAIL: Expected SetClientProperties(category=%S) to return 0x%08x [AUDCLNT_E_ALREADY_INITIALIZED].  Actual 0x%08x [%s]",
                    GetCategoryString( g_offloadCategories[i] ), AUDCLNT_E_ALREADY_INITIALIZED, hr, GetHRString(hr) );
            }
            else if( !isInitialized && S_OK != hr )
            {
                XLOG( XFAIL, eError, "FAIL: Expected SetClientProperties(category=%S) to return 0x%08x [S_OK].  Actual 0x%08x [%s]",
                    GetCategoryString( g_offloadCategories[i] ), S_OK, hr, GetHRString(hr) );
            }
        }
    }

// TODO: Change to _WIN32_WINNT_X when it's available
#if (WINVER < _WIN32_WINNT_WIN8)     
    if ( ! ( sizeof(AudioClientProperties) == 12 ) )
    {
        XLOG( XFAIL, eError, "FAIL: Expected AudioClientProperties.cbSize to be 12. Actual %u", sizeof(AudioClientProperties) ); 
    }
    return FNS_PASS;
#else
    BOOL bRawModeSupported = FALSE;

    if ( ! ( sizeof(AudioClientProperties) == 16 ) )
    {
        XLOG( XFAIL, eError, "FAIL: Expected size of AudioClientsProperties to be 16. Actual %u", sizeof(AudioClientProperties) ); 
    }

    // Check if device supports Raw Mode
    hr = CheckDeviceSupportsRawMode(pMMDevice, &bRawModeSupported);
    if( S_OK != hr )
    {
        XLOG( XFAIL, eError, "FAIL: Expected CheckDeviceSupportsRawMode to return 0x%08x [S_OK].  Actual 0x%08x [%s]", S_OK, hr, GetHRString(hr) );
    }

    if( TRUE == bRawModeSupported )
    { 
        clientProperties.Options = AUDCLNT_STREAMOPTIONS_RAW;
        
        clientProperties.bIsOffload = FALSE;
        clientProperties.eCategory = g_nonOffloadCategories[0];
        METHODCHECKRESULT( "IAudioClient2::SetClientProperties", hr, pAudioClient2->SetClientProperties( &clientProperties ) );
        if( isInitialized && AUDCLNT_E_ALREADY_INITIALIZED != hr )
        {
            XLOG( XFAIL, eError, "FAIL: Expected SetClientProperties(category=%S) to return 0x%08x [AUDCLNT_E_ALREADY_INITIALIZED].  Actual 0x%08x [%s]",
                GetCategoryString( clientProperties.eCategory ), AUDCLNT_E_ALREADY_INITIALIZED, hr, GetHRString(hr) );
        }
        else if( !isInitialized && S_OK != hr )
        {
            XLOG( XFAIL, eError, "FAIL: Expected SetClientProperties(category=%S) to return 0x%08x [S_OK].  Actual 0x%08x [%s]",
                GetCategoryString( clientProperties.eCategory ), S_OK, hr, GetHRString(hr) );
        }

        if ( TRUE == isOffloadCapable )
        { 
            clientProperties.bIsOffload = TRUE;
            clientProperties.eCategory = g_offloadCategories[0];
            METHODCHECKRESULT( "IAudioClient2::SetClientProperties", hr, pAudioClient2->SetClientProperties( &clientProperties ) );
            if( isInitialized && AUDCLNT_E_ALREADY_INITIALIZED != hr )
            {
                XLOG( XFAIL, eError, "FAIL: Expected SetClientProperties(category=%S) to return 0x%08x [AUDCLNT_E_ALREADY_INITIALIZED].  Actual 0x%08x [%s]",
                    GetCategoryString( clientProperties.eCategory ), AUDCLNT_E_ALREADY_INITIALIZED, hr, GetHRString(hr) );
            }
            else if( !isInitialized && S_OK != hr )
            {
                XLOG( XFAIL, eError, "FAIL: Expected SetClientProperties(category=%S) to return 0x%08x [S_OK].  Actual 0x%08x [%s]",
                    GetCategoryString( clientProperties.eCategory ), S_OK, hr, GetHRString(hr) );
            }
        }
    }
    return FNS_PASS;

#endif
}

DWORD GetBufferSizeLimits_ValidParamsTest( AudioClient2TCPath Flags, IAudioClient2* pAudioClient2, bool isInitialized )
{
    HRESULT hr = S_OK;
    BOOL isOffloadCapable = FALSE;
    bool IsExclusiveMode = ((0 != (INITIALIZE_IN_EXCLUSIVE & Flags))?true:false);
    CComHeapPtr<WAVEFORMATEX> spWfx;
    REFERENCE_TIME minRefTime = 0;
    REFERENCE_TIME maxRefTime = 0;

    // See if IAudioClient2 says the endpoint is offload capable
    METHODCHECKRESULT( "IAudioClient2::IsOffloadCapable", hr, pAudioClient2->IsOffloadCapable( AudioCategory_Media,
        &isOffloadCapable ) );
    if( S_OK != hr )
    {
        XLOG( XFAIL, eError, "FAIL: Expected IsOffloadCapable to return 0x%08x [S_OK].  Actual 0x%08x [%s]",
            S_OK, hr, GetHRString(hr) );

        return FNS_PASS;
    }

    // Test GetBufferSizeLimits w/ Mix Format
    if( TRUE == isOffloadCapable )
    {
        HRESULT     hrExpectedGetBufferLimits = ((IsExclusiveMode)?AUDCLNT_E_OFFLOAD_MODE_ONLY:S_OK);
    
        if( !isInitialized )
        {
            AudioClientProperties clientProperties = {0};
            clientProperties.bIsOffload = TRUE;
            clientProperties.eCategory = AudioCategory_Media;
            clientProperties.cbSize = sizeof(AudioClientProperties);

            METHODCHECKRESULT( "IAudioClient2::SetClientProperties", hr, pAudioClient2->SetClientProperties( &clientProperties ) );
            if( S_OK != hr )
            {
                XLOG( XFAIL, eError, "FAIL: Expected SetClientProperties to return 0x%08x [S_OK].  Actual 0x%08x [%s]",
                    S_OK, hr, GetHRString(hr) );

                return FNS_PASS;
            }
        }

        // Get the mix format
        METHODCHECKRESULT( "IAudioClient2::GetMixFormat", hr, pAudioClient2->GetMixFormat( &spWfx ) );
        if( S_OK != hr )
        {
            XLOG( XFAIL, eError, "FAIL: Expected GetMixFormat to return 0x%08x [S_OK].  Actual 0x%08x [%s]",
                S_OK, hr, GetHRString(hr) );

            return FNS_PASS;
        }

        // push-mode buffer size limits
        METHODCHECKRESULT( "IAudioClient2::GetBufferSizeLimits", hr,
            pAudioClient2->GetBufferSizeLimits( spWfx, false, &minRefTime, &maxRefTime ) );
        if ( hrExpectedGetBufferLimits != hr )
        {
            XLOG( XFAIL, eError, "FAIL: Expected GetBufferSizeLimits(push-mode) to return 0x%08x [%s].  Actual 0x%08x [%s]",
                hrExpectedGetBufferLimits, GetHRString(hrExpectedGetBufferLimits), hr, GetHRString(hr) );

            return FNS_PASS;
        }
    
        // pull-mode buffer size limits
        METHODCHECKRESULT( "IAudioClient2::GetBufferSizeLimits", hr,
            pAudioClient2->GetBufferSizeLimits( spWfx, true, &minRefTime, &maxRefTime ) );
        if( hrExpectedGetBufferLimits != hr )
        {
            XLOG( XFAIL, eError, "FAIL: Expected GetBufferSizeLimits(pull-mode) to return 0x%08x [%s].  Actual 0x%08x [%s]",
                hrExpectedGetBufferLimits, GetHRString(hrExpectedGetBufferLimits), hr, GetHRString(hr) );

            return FNS_PASS;
        }

        if( !isInitialized )
        {
            AudioClientProperties clientProperties = {0};
            clientProperties.bIsOffload = FALSE;
            clientProperties.eCategory = AudioCategory_Other;
            clientProperties.cbSize = sizeof(AudioClientProperties);

            METHODCHECKRESULT( "IAudioClient2::SetClientProperties", hr, pAudioClient2->SetClientProperties( &clientProperties ) );
            if( S_OK != hr )
            {
                XLOG( XFAIL, eError, "FAIL: Expected SetClientProperties to return 0x%08x [S_OK].  Actual 0x%08x [%s]",
                    S_OK, hr, GetHRString(hr) );

                return FNS_PASS;
            }
        }
    }

    return FNS_PASS;
}

//
//
//

HRESULT GetExclusiveModeFormat( IAudioClient2* pAudioClient2, IMMDevice* pMMDevice, PWAVEFORMATEX* ppWfx )
{
    HRESULT hr = S_OK;
    CComHeapPtr<WAVEFORMATEX> spWfx;
    CComHeapPtr<WAVEFORMATEX> spClosestWfx;
    PROPVARIANT varDeviceFormat;
    CComPtr<IPropertyStore> pPropertyStore;

    PropVariantInit( &varDeviceFormat );

    // Will look at three possible formats:
    //  1. Mix Format
    //  2. Closest Format
    //  3. Device Format

    // Get the mix format to use for initialize
    METHODCHECKRESULT( "IAudioClient2::GetMixFormat", hr, pAudioClient2->GetMixFormat( &spWfx ) );
    if( S_OK != hr )
    {
        XLOG( XFAIL, eError, "FAIL: Expected GetMixFormat to return 0x%08x [S_OK].  Actual 0x%08x [%s]",
            S_OK, hr, GetHRString( hr ) );

        goto END;
    }

    // See if the format is supported in exclusive mode
    // if not, get the "closest match"
    METHODCHECKRESULT( "IAudioClient2::IsFormatSupported", hr,
        pAudioClient2->IsFormatSupported( AUDCLNT_SHAREMODE_EXCLUSIVE, spWfx, &spClosestWfx ) );
    if( AUDCLNT_E_UNSUPPORTED_FORMAT == hr )
    {
        if( NULL == spClosestWfx.m_pData )
        {
            // ok, last chance, get the device format
            hr = pMMDevice->OpenPropertyStore( STGM_READ, &pPropertyStore );
            if( S_OK != hr )
            {
                XLOG( XFAIL, eError, "FAIL: Expected OpenPropertyStore to return 0x%08x [S_OK].  Actual 0x%08x [%s]",
                    S_OK, hr, GetHRString( hr ) );

                goto END;
            }

            hr = pPropertyStore->GetValue( PKEY_AudioEngine_DeviceFormat, &varDeviceFormat );
            if( S_OK != hr )
            {
                XLOG( XFAIL, eError, "FAIL: Expected GetValue to return 0x%08x [S_OK].  Actual 0x%08x [%s]",
                    S_OK, hr, GetHRString( hr ) );

                goto END;
            }
            else if( VT_BLOB != varDeviceFormat.vt )
            {
                XLOG( XFAIL, eError, "FAIL: Expected PROPVARIANT type to be %u [VT_BLOB].  Actual %u",
                    VT_BLOB, varDeviceFormat.vt );
                hr = E_FAIL;

                goto END;
            }

            *ppWfx = (PWAVEFORMATEX)CoTaskMemAlloc( varDeviceFormat.blob.cbSize );
            if( NULL == *ppWfx )
            {
                XLOG( XFAIL, eError, "FAIL: Unable to allocate memory for WAVEFORMATEX" );
                hr = E_OUTOFMEMORY;

                goto END;
            }

            memcpy( *ppWfx, varDeviceFormat.blob.pBlobData, varDeviceFormat.blob.cbSize );
        }
        else
        {
            *ppWfx = spClosestWfx.Detach();
        }
    }
    else if( S_OK != hr )
    {
        XLOG( XFAIL, eError, "FAIL: Expected IsFormatSupported to return 0x%08x [S_OK].  Actual 0x%08x [%s]",
            S_OK, hr, GetHRString( hr ) );

        goto END;
    }
    else
    {
        *ppWfx = spWfx.Detach();
    }

END:

    PropVariantClear( &varDeviceFormat );

    return hr;
}
