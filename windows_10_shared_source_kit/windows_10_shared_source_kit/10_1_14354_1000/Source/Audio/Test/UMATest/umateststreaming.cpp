// Copyright(C) Microsoft. All rights reserved.

#include "StdAfx.h"

#include <umateststreaming.h>


//
// these test cases call an asynchronous render or capture function
// and then waitson the "streaming completed" event
// repeat several times in quick succession
//
// the idea is to flush out race conditions and deadlocks
// between setting the "streaming completed" event and being ready for streaming again
//
DWORD TC_UmaTestStreaming_RenderLoop()
{
    CComPtr< IMMDevice > pMMDevice;
    HRESULT hr = GetEndpointUnderTest( &pMMDevice );
    if( FAILED( hr ) )
    {
        XLOG( XFAIL, 1, "FAIL: Error calling GetEndpointUnderTest.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );
        return FNS_PASS;
    }
    
    // get a render test streaming engine
    CComPtr< IRenderTestStreamingEngine > pRender;
    hr = CreateTestStreamingEngine(
        __uuidof( IRenderTestStreamingEngine ),
        pMMDevice,
        EVENT_PULL_RENDER,
        nullptr,
        reinterpret_cast< void** >( &pRender )
    );
    if( FAILED( hr ) )
    {
        XLOG( XFAIL, 1, "FAIL: Error calling CreateTestStreamingEngine.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );
        return FNS_PASS;
    }

    // get the common test streaming interface
    CComPtr< ITestStreamingEngine > pEngine;
    hr = pRender->QueryInterface( __uuidof(ITestStreamingEngine), reinterpret_cast< void** >( &pEngine ) );
    if( FAILED( hr ) )
    {
        XLOG( XFAIL, 1, "FAIL: Could not QI for ITestStreamingEngine.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );
        return FNS_PASS;
    }
    
    // initialize
    hr = pEngine->Initialize( nullptr );
    if( FAILED( hr ) )
    {
        XLOG( XFAIL, 1, "FAIL: ITestStreamingEngine::Initialize failed.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );
        return FNS_PASS;
    }
    
    CHandle hDone;
    hDone.Attach( CreateEvent( nullptr, FALSE, FALSE, nullptr ) );
    if ( nullptr == hDone )
    {
        DWORD dwErr = GetLastError();
        XLOG( XFAIL, 1, "FAIL: CreateEvent failed.  GetLastError %d [%s]", dwErr, GetHRString( HRESULT_FROM_WIN32( dwErr ) ) );
        return FNS_PASS;
    }
    
    const int PASSES = 100;
    SLOG( 3, "Attempting %d passes...", PASSES );
    for ( int i = 0; i < PASSES; i++ ) {
        // SLOG( 3, "Pass %d of %d", i + 1, PASSES );
        
        hr = pRender->RenderSineTone(
            372.387, // frequency (Hz)
            0.01,  // amplitude (0 to 1)
            147, // duration (ms)
            DITHER_TYPE_TPD,
            hDone
        );
        if( FAILED( hr ) )
        {
            XLOG(
                XFAIL, 1,
                "FAIL: IRenderTestStreamingEngine::RenderSineTone failed on pass %d of %d; aborting.  HRESULT 0x%08x [%s]",
                i + 1, PASSES, hr, GetHRString( hr )
            );
            return FNS_PASS;
        }
        
        WaitForSingleObject( hDone, INFINITE );
        hr = pRender->GetLastStreamingError();
        if( FAILED( hr ) )
        {
            XLOG(
                XFAIL, 1,
                "FAIL: IRenderTestStreamingEngine::GetLastStreamingError indicates an error on pass %d of %d; aborting.  HRESULT 0x%08x [%s]",
                i + 1, PASSES, hr, GetHRString( hr )
            );
            return FNS_PASS;
        }
    }
    
    return FNS_PASS;
}

DWORD TC_UmaTestStreaming_CaptureLoop()
{
    CComPtr< IMMDevice > pMMDevice;
    HRESULT hr = GetEndpointUnderTest( &pMMDevice );
    if( FAILED( hr ) )
    {
        XLOG( XFAIL, 1, "FAIL: Error calling GetEndpointUnderTest.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );
        return FNS_PASS;
    }
    
    // get a capture test streaming engine
    CComPtr< ICaptureTestStreamingEngine > pCapture;
    hr = CreateTestStreamingEngine(
        __uuidof( ICaptureTestStreamingEngine ),
        pMMDevice,
        EVENT_PULL_CAPTURE,
        nullptr,
        reinterpret_cast< void** >( &pCapture )
    );
    if( FAILED( hr ) )
    {
        XLOG( XFAIL, 1, "FAIL: Error calling CreateTestStreamingEngine.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );
        return FNS_PASS;
    }

    // get the common test streaming interface
    CComPtr< ITestStreamingEngine > pEngine;
    hr = pCapture->QueryInterface( __uuidof(ITestStreamingEngine), reinterpret_cast< void** >( &pEngine ) );
    if( FAILED( hr ) )
    {
        XLOG( XFAIL, 1, "FAIL: Could not QI for ITestStreamingEngine.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );
        return FNS_PASS;
    }
    
    // initialize
    hr = pEngine->Initialize( nullptr );
    if( FAILED( hr ) )
    {
        XLOG( XFAIL, 1, "FAIL: ITestStreamingEngine::Initialize failed.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );
        return FNS_PASS;
    }
    
    CHandle hDone;
    hDone.Attach( CreateEvent( nullptr, FALSE, FALSE, nullptr ) );
    if ( nullptr == hDone )
    {
        DWORD dwErr = GetLastError();
        XLOG( XFAIL, 1, "FAIL: CreateEvent failed.  GetLastError %d [%s]", dwErr, GetHRString( HRESULT_FROM_WIN32( dwErr ) ) );
        return FNS_PASS;
    }
    
    const int PASSES = 100;
    SLOG( 3, "Attempting %d passes...", PASSES );
    for ( int i = 0; i < PASSES; i++ ) {
        // SLOG( 3, "Pass %d of %d", i + 1, PASSES );
        
        hr = pCapture->NullCapture( 147, hDone );
        if( FAILED( hr ) )
        {
            XLOG(
                XFAIL, 1,
                "FAIL: ICaptureTestStreamingEngine::NullCapture failed on pass %d of %d; aborting. HRESULT 0x%08x [%s]",
                i + 1, PASSES, hr, GetHRString( hr )
            );
            return FNS_PASS;
        }
        
        WaitForSingleObject( hDone, INFINITE );
        hr = pCapture->GetLastStreamingError();
        if( FAILED( hr ) )
        {
            XLOG(
                XFAIL, 1, "FAIL: ICaptureTestStreamingEngine::GetLastStreamingError indicates an error on pass %d of %d; aborting.  HRESULT 0x%08x [%s]",
                i + 1, PASSES, hr, GetHRString( hr )
            );
            return FNS_PASS;
        }
    }
    
    return FNS_PASS;
}
