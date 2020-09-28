// Copyright(C) Microsoft. All rights reserved.

#include "StdAfx.h"

#include <audiostreaming.h>
#include <deviceaccesshelper.h>
#include <AudioOffload.h>
#include <OffloadPath.h>

#include "log_wfx.h"
#include "TestUtilities.h"
#include "KSInstance.h"

// Test cases
DWORD TC_KSInstance_MF() {
    HRESULT hr;
    CKsInstanceTest ks( api_MediaFoundation, hr );
    if (SUCCEEDED(hr)) {
        ks.Run();
    }
    return FNS_PASS;
}

DWORD TC_KSInstance_DSound() {
    HRESULT hr;
    CKsInstanceTest ks( api_DirectSound, hr );
    if (SUCCEEDED(hr)) {
        ks.Run();
    }
    return FNS_PASS;
}

DWORD TC_KSInstance_WinMM() {
    HRESULT hr;
    CKsInstanceTest ks( api_Wave, hr );
    if (SUCCEEDED(hr)) {
        ks.Run();
    }
    return FNS_PASS;
}

DWORD TC_KSInstance_WasapiShared() {
    HRESULT hr;
    CKsInstanceTest ks( api_WASAPI, hr );
    if (SUCCEEDED(hr)) {
        ks.Run();
    }
    return FNS_PASS;
}

DWORD TC_KSInstance_WasapiExclusive() {
    HRESULT hr;
    CKsInstanceTest ks( api_WASAPIExclusive, hr );
    if (SUCCEEDED(hr)) {
        ks.Run();
    }
    return FNS_PASS;
}

// CKsInstanceTest constructor
_Success_( hr >= 0 )
CKsInstanceTest::CKsInstanceTest( ApiEnum api, _Out_ HRESULT &hr )
: m_api( api )
, m_deviceType( GetDeviceTypeUnderTest() )
, m_closeKsLib( false )
{
    hr = GetEndpointUnderTest( &m_spMMDevice );
    if(FAILED(hr)) {
         XLOG( XFAIL, eFatalError, "FAIL: Unable to obtain IMMDevice interface for the endpoint under test (0x%8x)", hr);
         return;
    }
    
    hr = g_IShell->GetBasicLog( &m_spBasicLog );
    if( FAILED( hr ) ) {
        XLOG( XFAIL, eFatalError, "FAIL: Error calling ITestShell::GetBasicLog.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );
        return;
    }
    
    if ( KslInitKsLib() ) {
        m_closeKsLib = true;
    } else {
        XLOG( XFAIL, eFatalError, "FAIL: Error calling KslInitKsLib; GetLastError is %u", GetLastError() );
        hr = E_UNEXPECTED;
    }
}

CKsInstanceTest::~CKsInstanceTest() {
    if ( m_closeKsLib ) {
        KslCloseKsLib();
    }
}

void CKsInstanceTest::Run() {
    switch ( m_deviceType ) {
        case DEVICE_TYPE_CAPTURE:
            Capture();
            break;
            
        case DEVICE_TYPE_RENDER:
            Render( false );
            break;
            
        case DEVICE_TYPE_RENDEROFFLOAD:
            Render( true );
            break;
        
        default:
            XLOG( XFAIL, 1, "FAIL: CKsInstanceTest::Run called on an unexpected device type %u", m_deviceType );
    }
}

void CKsInstanceTest::Capture() {
    // instance count should start at 0
    if ( ! ValidatePinCountsTraditional( false ) ) {
        return;
    }
    
    CComPtr<IAudioStreamingCapture> spCapture;
    HRESULT hr = CreateAudioStreamingCapture(
        m_api,
        m_spMMDevice,
        m_spBasicLog,
        &spCapture
    );
    // BUGBUG: remove this check when MF capture is implemented in audiostreaming.dll
    if ( E_NOTIMPL == hr && api_MediaFoundation == m_api ) {
        XLOG( XSKIP, 1, "SKIP: Media Foundation capture is not yet implemented in audiostreaming.dll" );
        return;
    } else if ( FAILED( hr ) ) {
        XLOG( XFAIL, 1, "FAIL: CreateAudioStreamingCapture failed with hr = 0x%08x (%s)", hr, GetHRStringA(hr) );
        return;
    }
    
    hr = spCapture->Initialize( NULL ); // any format will do
    if ( FAILED( hr ) ) {
        XLOG( XFAIL, 1, "FAIL: IAudioStreamingCapture::Initialize failed with hr = 0x%08x (%s)", hr, GetHRStringA(hr) );
        return;
    }
    
    CComHeapPtr<WAVEFORMATEX> pWfx;
    hr = spCapture->GetWaveFormat(&pWfx);
    if ( FAILED( hr ) ) {
        XLOG( XFAIL, 1, "FAIL: IAudioStreamingCapture::GetWaveFormat failed with hr = 0x%08x (%s)", hr, GetHRStringA(hr) );
        return;
    }
    log_wfx(m_spBasicLog, pWfx);
    
    // we're just going to drop the frames on the floor
    SLOG( 3, "Starting capture..." );
    hr = spCapture->CaptureWithoutBufferAsync();
    if ( FAILED( hr ) ) {
        XLOG( XFAIL, 1, "FAIL: IAudioStreamingCapture::CaptureWithoutBufferAsync failed with hr = 0x%08x (%s)", hr, GetHRStringA(hr) );
        return;
    }
    
    // from this point on we want to make absolutely sure that Stop() gets called
    CStopOnExit<IAudioStreamingCapture> stop( spCapture );

    // then it should go up to 1
    if ( ! ValidatePinCountsTraditional( true ) ) {
        return;
    }
    
    hr = stop.Now();
    SLOG( 3, "Stopping capture..." );
    if ( FAILED( hr ) ) {
        XLOG( XFAIL, 1, "FAIL: IAudioStreamingCapture::Stop failed with hr = 0x%08x (%s)", hr, GetHRStringA(hr) );
        return;
    }

    // tear down the streaming object too
    // it's not enough to stop, we also have to tear down the IAudioClient
    spCapture.Release();

    // and now it should go back to 0
    ValidatePinCountsTraditional( false );
}

void CKsInstanceTest::Render( bool offload ) {
    // does this endpoint support offload?
    // this is important to know whether we stream in offload or not
    // because offload-capable endpoints have three pin factories
    // whereas non-offload-capable endpoints have only one
    bool offloadCapable;
    HRESULT hr = S_OK;

    hr = CheckDeviceSupportsOffload(m_spMMDevice, AudioOffload::MMDEV_API, offloadCapable);
    if ( FAILED( hr ) ) {
        XLOG( XFAIL, 1, "FAIL: CheckDeviceSupportsOffload failed with hr = 0x%08x (%s)", hr, GetHRStringA(hr) );
        return;
    }
    
    // the device is giving mixed messages as to whether it supports offload or not
    if ( offload && !offloadCapable ) {
        XLOG( XFAIL, 1, "FAIL: Offload streaming was requested but this device is not offload-capable" );
        return;
    }
    
    if (offloadCapable) {
        RenderToOffloadCapable( offload );
    } else {
        RenderToTraditional();
    }
}
    
void CKsInstanceTest::RenderToOffloadCapable( bool offload ) {

    // validate the combination of API and offload
    switch ( m_api ) {
        case api_MediaFoundation:
            if ( !offload ) {
                XLOG( XSKIP, 1, "SKIP: Media Foundation playback is always offloaded on offload-capable pins" );
                return;
            }
            break;
            
        case api_WASAPI:
            // WASAPI shared mode can be played in both offloaded and non-offloaded modes
            break;

        case api_Wave:
        case api_DirectSound:
        case api_WASAPIExclusive:
            if ( offload ) {
                XLOG( XFAIL, 1, "FAIL: Wave, DSound and WASAPI exclusive playback are never offloaded" );
                return;
            }
            break;
            
        default:
            XLOG( XFAIL, 1, "Unrecognized API: %u", m_api );
            return;
    }

    // the pin count should start at 0
    if ( ! ValidatePinCountsOffloadCapable( false, offload ) ) {
        return;
    }
        
    SLOG( 3, "Starting stream..." );
    CComPtr<IAudioStreamingRender> spRender;
    HRESULT hr = CreateAudioStreamingRender(
        m_api,
        m_spMMDevice,
        m_spBasicLog,
        &spRender
    );
    if ( FAILED( hr ) ) {
        XLOG( XFAIL, 1, "FAIL: CreateAudioStreamingRender failed with hr = 0x%08x (%s)", hr, GetHRStringA(hr) );
        return;
    }

    CComHeapPtr<WAVEFORMATEX> spWfx;
    if ( offload ) {
        // some APIs require handling
        AudioClientProperties clientProperties = {0};
        clientProperties.bIsOffload = TRUE;
        clientProperties.eCategory = AudioCategory_Media;
        clientProperties.cbSize = sizeof(clientProperties);
        
        if ( api_WASAPI == m_api) {
            // explicitly put the streaming engine into "offload mode"
            CComPtr<IAudioStreamingControl> spAudioStreamingControl;

            hr = spRender->QueryInterface( __uuidof( IAudioStreamingControl ), reinterpret_cast<void **>( &spAudioStreamingControl ) );
            if ( FAILED( hr ) ) {
                XLOG( XFAIL, 1, "FAIL: IAudioStreamingRender::QueryInterface(IAudioStreamingControl) failed with hr = 0x%08x (%s)", hr, GetHRStringA(hr) );
                return;
            }
            
            CComPtr<IAudioStreamingWasapiControl> spAudioStreamingWasapiControl;
            hr = spAudioStreamingControl->GetService( __uuidof( IAudioStreamingWasapiControl ), reinterpret_cast<void **>( &spAudioStreamingWasapiControl ) );
            if ( FAILED( hr ) ) {
                XLOG( XFAIL, 1, "FAIL: IAudioStreamingControl::GetService(IAudioStreamingWasapiControl) failed with hr = 0x%08x (%s)", hr, GetHRStringA(hr) );
                return;
            }
            
            hr = spAudioStreamingWasapiControl->SetClientProperties( &clientProperties );
            if ( FAILED( hr ) ) {
                XLOG( XFAIL, 1, "FAIL: IAudioStreamingWasapiControl::SetClientProperties failed with hr = 0x%08x (%s)", hr, GetHRStringA(hr) );
                return;
            }
        }
        
        if ( api_MediaFoundation == m_api ) {
            SLOG( 3, "Querying the offload mix format..." );
            
            CComPtr<IAudioClient2> spAudioClient2;
            hr = m_spMMDevice->Activate(
                __uuidof( IAudioClient2 ),
                CLSCTX_ALL,
                NULL,
                reinterpret_cast< void** >( &spAudioClient2 )
            );
            if ( FAILED( hr ) ) {
                XLOG( XFAIL, 1, "FAIL: IMMDevice::Activate(IAudioClient2) failed with hr = 0x%08x (%s)", hr, GetHRStringA(hr) );
                return;
            }
            
            hr = spAudioClient2->SetClientProperties( &clientProperties );
            if ( FAILED( hr ) ) {
                XLOG( XFAIL, 1, "FAIL: IAudioClient2::SetClientProperties failed with hr = 0x%08x (%s)", hr, GetHRStringA(hr) );
                return;
            }
            
            hr = spAudioClient2->GetMixFormat( &spWfx );
            if ( FAILED( hr ) ) {
                XLOG( XFAIL, 1, "FAIL: IAudioClient2::GetMixFormat failed with hr = 0x%08x (%s)", hr, GetHRStringA(hr) );
                return;
            }
        }
    }
    
    hr = spRender->Initialize( spWfx );
    if ( FAILED( hr ) ) {
        XLOG( XFAIL, 1, "FAIL: IAudioStreamingRender::Initialize failed with hr = 0x%08x (%s)", hr, GetHRStringA(hr) );
        return;
    }
    
    spWfx.Free();
    hr = spRender->GetWaveFormat( &spWfx );
    if ( FAILED( hr ) ) {
        XLOG( XFAIL, 1, "FAIL: IAudioStreamingRender::GetWaveFormat failed with hr = 0x%08x (%s)", hr, GetHRStringA(hr) );
        return;
    }
    log_wfx( m_spBasicLog, spWfx );

    // intentionally picking a frequency that doesn't match 10ms buffer size
    SLOG( 3, "Starting playback..." );
    hr = spRender->PlaySineWaveAsync( 103.2781f, 0.01f, Method_TPD );
    if ( FAILED( hr ) ) {
        XLOG( XFAIL, 1, "FAIL: IAudioStreamingCapture::CaptureWithoutBufferAsync failed with hr = 0x%08x (%s)", hr, GetHRStringA(hr) );
        return;
    }

    // from this point on we want to make absolutely sure that Stop() gets called
    CStopOnExit<IAudioStreamingRender> stop( spRender );

    // DirectSound render is hosted in a different thread
    // give it time to catch up to the command
    if ( api_DirectSound == m_api ) {
        Sleep( 200 );
    }
    
    // the pin count should go up to 1 on an appropriate pin
    if ( ! ValidatePinCountsOffloadCapable( true, offload ) ) {
        return;
    }
    
    // stop the stream
    SLOG( 3, "Stopping playback..." );
    hr = stop.Now();
    if ( FAILED( hr ) ) {
        XLOG( XFAIL, 1, "FAIL: IAudioStreamingRender::Stop failed with hr = 0x%08x (%s)", hr, GetHRStringA(hr) );
        return;
    }

    // tear down the streaming object too
    // it's not enough to stop, we also have to tear down the IAudioClient
    spRender.Release();

    // DirectSound render is hosted in a different thread
    // give it time to catch up to the command
    if ( api_DirectSound == m_api ) {
        Sleep( 200 );
    }

    // the pin count should go back down to 1 again
    ValidatePinCountsOffloadCapable( false, offload );
}

void CKsInstanceTest::RenderToTraditional() {
    // the pin count should start at 0
    if ( ! ValidatePinCountsTraditional( false ) ) {
        return;
    }
    
    CComPtr<IAudioStreamingRender> spRender;
    HRESULT hr = CreateAudioStreamingRender(
        m_api,
        m_spMMDevice,
        m_spBasicLog,
        &spRender
    );
    if ( FAILED( hr ) ) {
        XLOG( XFAIL, 1, "FAIL: CreateAudioStreamingRender failed with hr = 0x%08x (%s)", hr, GetHRStringA(hr) );
        return;
    }
    
    hr = spRender->Initialize( NULL ); // any format will do
    if ( FAILED( hr ) ) {
        XLOG( XFAIL, 1, "FAIL: IAudioStreamingRender::Initialize failed with hr = 0x%08x (%s)", hr, GetHRStringA(hr) );
        return;
    }
    
    CComHeapPtr<WAVEFORMATEX> pWfx;
    hr = spRender->GetWaveFormat(&pWfx);
    if ( FAILED( hr ) ) {
        XLOG( XFAIL, 1, "FAIL: IAudioStreamingRender::GetWaveFormat failed with hr = 0x%08x (%s)", hr, GetHRStringA(hr) );
        return;
    }
    log_wfx(m_spBasicLog, pWfx);

    // intentionally picking a frequency that doesn't match 10ms buffer size
    SLOG( 3, "Starting playback..." );
    hr = spRender->PlaySineWaveAsync( 103.2781f, 0.01f, Method_TPD );
    if ( FAILED( hr ) ) {
        XLOG( XFAIL, 1, "FAIL: IAudioStreamingCapture::CaptureWithoutBufferAsync failed with hr = 0x%08x (%s)", hr, GetHRStringA(hr) );
        return;
    }
    
    // from this point on we want to make absolutely sure that Stop() gets called
    CStopOnExit<IAudioStreamingRender> stop( spRender );

    // DirectSound render is hosted in a different thread
    // give it time to catch up to the command
    if ( api_DirectSound == m_api ) {
        Sleep( 200 );
    }

    // the pin count should have gone up to 1
    if ( ! ValidatePinCountsTraditional( true ) ) {
        return;
    }

    // stop the stream
    SLOG( 3, "Stopping playback..." );
    hr = stop.Now();
    if ( FAILED( hr ) ) {
        XLOG( XFAIL, 1, "FAIL: IAudioStreamingRender::Stop failed with hr = 0x%08x (%s)", hr, GetHRStringA(hr) );
        return;
    }

    // tear down the streaming object too
    // it's not enough to stop, we also have to tear down the IAudioClient
    spRender.Release();
    
    // DirectSound render is hosted in a different thread
    // give it time to catch up to the command
    if ( api_DirectSound == m_api ) {
        Sleep( 200 );
    }
    
    // the current instance count should drop to 0 again
    ValidatePinCountsTraditional( false );
}


bool CKsInstanceTest::ValidatePinCountsOffloadCapable( bool streaming, bool offload ) {
    // this is an offload capable pin
    // so we need to monitor all three pin factories
    // regardless of whether we actually play in offload mode or not
    CComPtr<IOffloadPathCollection> spOffloadPathCollection;
    HRESULT hr = OffloadEnumerator::GetOffloadPath(
        m_spBasicLog,
        m_spMMDevice,
        &spOffloadPathCollection
    );
    if ( FAILED( hr ) ) {
        XLOG( XFAIL, 1, "FAIL: GetOffloadPath failed with hr = 0x%08x (%s)", hr, GetHRStringA(hr) );
        return false;
    }
    UINT nPaths;
    hr = spOffloadPathCollection->GetCount( nPaths );
    if ( FAILED( hr ) ) {
        XLOG( XFAIL, 1, "FAIL: IOffloadPathCollection::GetCount failed with hr = 0x%08x (%s)", hr, GetHRStringA(hr) );
        return false;
    }

    // there should be exactly one path
    if (1 != nPaths) {
        XLOG( XFAIL, 1, "Unexpected number of offload paths: %u", nPaths );
        return false;
    }
    
    CComPtr<IOffloadPath> spOffloadPath;
    hr = spOffloadPathCollection->Item( 0, &spOffloadPath );
    if ( FAILED( hr ) ) {
        XLOG( XFAIL, 1, "FAIL: IOffloadPathCollection::Item failed with hr = 0x%08x (%s)", hr, GetHRStringA(hr) );
        return false;
    }
    
    PinInstances offloadPin = { 0 };
    PinInstances hostPin = { 0 };
    PinInstances loopbackPin = { 0 } ;
    
    // get the ids for each pin
    hr = spOffloadPath->GetOffloadPinFactoryId( &offloadPin.pinId );
    if ( FAILED( hr ) ) {
        XLOG( XFAIL, 1, "FAIL: IOffloadPath::GetOffloadPinFactoryId failed with hr = 0x%08x (%s)", hr, GetHRStringA(hr) );
        return false;
    }
    hr = spOffloadPath->GetHostPinFactoryId( &hostPin.pinId );
    if ( FAILED( hr ) ) {
        XLOG( XFAIL, 1, "FAIL: IOffloadPath::GetHostPinFactoryId failed with hr = 0x%08x (%s)", hr, GetHRStringA(hr) );
        return false;
    }
    hr = spOffloadPath->GetLoopbackPinFactoryId( &loopbackPin.pinId );
    if ( FAILED( hr ) ) {
        XLOG( XFAIL, 1, "FAIL: IOffloadPath::GetLoopbackPinFactoryId failed with hr = 0x%08x (%s)", hr, GetHRStringA(hr) );
        return false;
    }

    // get the instance counts for each pin
    hr = spOffloadPath->GetPinInstanceCounts( offloadPin.pinId, &offloadPin.possible, &offloadPin.current );
    if ( FAILED( hr ) ) {
        XLOG( XFAIL, 1, "FAIL: IOffloadPath::GetPinInstanceCounts failed on offload pin %u with hr = 0x%08x (%s)", offloadPin.pinId, hr, GetHRStringA(hr) );
        return false;
    }
    hr = spOffloadPath->GetPinInstanceCounts( hostPin.pinId, &hostPin.possible, &hostPin.current );
    if ( FAILED( hr ) ) {
        XLOG( XFAIL, 1, "FAIL: IOffloadPath::GetPinInstanceCounts failed on host pin %u with hr = 0x%08x (%s)", hostPin.pinId, hr, GetHRStringA(hr) );
        return false;
    }
    hr = spOffloadPath->GetPinInstanceCounts( loopbackPin.pinId, &loopbackPin.possible, &loopbackPin.current );
    if ( FAILED( hr ) ) {
        XLOG( XFAIL, 1, "FAIL: IOffloadPath::GetPinInstanceCounts failed on loopback pin %u with hr = 0x%08x (%s)", loopbackPin.pinId, hr, GetHRStringA(hr) );
        return false;
    }

    SLOG( 3,
        "Offload pin instances: possible %d; current %d\n"
        "Host pin instances: possible %d; current %d\n"
        "Loopback pin instances: possible %d; current %d",
        offloadPin.possible, offloadPin.current,
        hostPin.possible, hostPin.current,
        loopbackPin.possible, loopbackPin.current
    );
    
    if ( streaming ) {
        if ( offload ) {
            // offload mode (Media Foundation or WASAPI shared mode) goes to the offload pin
            if ( ! ( 1 == offloadPin.current && 0 == hostPin.current && 0 == loopbackPin.current ) ) {
                XLOG( XFAIL, 1,
                    "FAIL: Expected offload (%d) to have 1 instance, host (%d) to have 0, and loopback (%d) to have 0",
                    offloadPin.current, hostPin.current, loopbackPin.current
                );
                return false;
            }
        } else if ( api_WASAPIExclusive == m_api ) {
            // WASAPI exclusive mode could go to either the offload or the host pin (either is allowed)
            if ( ! ( 1 == offloadPin.current + hostPin.current && 0 == loopbackPin.current ) ) {
                XLOG( XFAIL, 1,
                    "FAIL: Expected offload (%d) and host (%d) to have 1 instance combined, and loopback (%d) to have 0",
                    offloadPin.current, hostPin.current, loopbackPin.current
                );
                return false;
            }
        } else {
            // all other scenarios must go to the host pin
            if ( ! ( 0 == offloadPin.current && 1 == hostPin.current && 0 == loopbackPin.current ) ) {
                XLOG( XFAIL, 1,
                    "FAIL: Expected offload (%d) to have 1 instance, host (%d) to have 0, and loopback (%d) to have 0",
                    offloadPin.current, hostPin.current, loopbackPin.current
                );
                return false;
            }
        }
    } else {
        // the current instances should all be 0
        if ( ! ( 0 == offloadPin.current && 0 == hostPin.current && 0 == loopbackPin.current ) ) {
            XLOG( XFAIL, 1,
                "FAIL: Expected offload (%d), host (%d), and loopback (%d) pins to have 0 instances each",
                offloadPin.current, hostPin.current, loopbackPin.current
            );
            return false;
        }
    }
    
    return true;
}

bool CKsInstanceTest::ValidatePinCountsTraditional( bool streaming ) {
    ULONG possible;
    ULONG current;
    ULONG expected = ( streaming ? 1 : 0 );
    
    HRESULT hr = GetMMDeviceKSInstanceInformation(
        m_spMMDevice,
        possible,
        current
    );
    if ( HRESULT_FROM_WIN32( ERROR_NOT_FOUND ) == hr ) {
        XLOG( XSKIP, 1, "SKIP: This audio driver seems to not support KSPROPERTY_PIN_GLOBALCINSTANCES" );
        return false;
    } else if ( FAILED( hr ) ) {
        XLOG( XFAIL, 1, "FAIL: GetMMDeviceKSInstanceInformation failed with hr = 0x%08x (%s)", hr, GetHRStringA(hr) );
        return false;
    }
    SLOG( 3, "Instances: possible %d; current %d", possible, current );
    if ( current != expected ) {
        XLOG( XFAIL, 1, "FAIL: Current instances is %d; expected %d", current, expected );
        return false;
    }
    
    return true;
}
