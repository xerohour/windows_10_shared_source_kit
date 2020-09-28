// Copyright(C) Microsoft. All rights reserved.

// ksinstance.h

class CKsInstanceTest {
public:
    _Success_( hr >= 0 )
    CKsInstanceTest( ApiEnum api, _Out_ HRESULT &hr );
    ~CKsInstanceTest();

    void Run();
   
private:
    ApiEnum m_api;
    CComPtr<IBasicLog> m_spBasicLog;
    CComPtr<IMMDevice> m_spMMDevice;
    DWORD m_deviceType;
    bool m_closeKsLib;
    
    void Capture();
    void Render(bool offload);
    void RenderToOffloadCapable( bool offload );
    void RenderToTraditional();

    bool ValidatePinCountsOffloadCapable( bool streaming, bool offload );
    bool ValidatePinCountsTraditional( bool streaming );
};

template<typename T> class CStopOnExit {
public:
    CStopOnExit(T *pStopMe) : m_p( pStopMe ) {}
    
    ~CStopOnExit() {
        if ( NULL != m_p ) {
            HRESULT hr = m_p->Stop();
            if ( FAILED( hr ) ) {
                XLOG( XFAIL, eError, "FAIL: Stop() failed.  HRESULT 0x%08x [%s]", hr, GetHRString( hr ) );
            }
        }
    }
    
    HRESULT Now() {
        if ( NULL == m_p ) { return E_POINTER; }
        HRESULT hr = m_p->Stop();
        m_p = NULL;
        return hr;
    };
    
private:
    CComPtr<T> m_p;
};

struct PinInstances {
    UINT  pinId;
    ULONG possible;
    ULONG current;
};
