//--------------------------------------------------------------------------
//
// Copyright(C) Microsoft.All rights reserved.
//
// File: Lullaby.h
//
// Abstract:
//      Header for all the Shell98 definitions.
//
//--------------------------------------------------------------------------
#pragma once

// ------------------------------------------------------------------------------
// Device definition
class CLullabyDevice : public CDeviceDescriptor
{
public:
    CLullabyDevice(LPCWSTR pszName, LPCWSTR pszId, LPARAM lpType, HRESULT &hr);
    CLullabyDevice(IMMDevice *pIEndpoint, HRESULT &hr);
    ~CLullabyDevice();
    LPCWSTR GetName() { return (&(m_szRep[0])); }
    HRESULT GetEndpoint(IMMDevice** ppDevice);
    BOOL DoesDeviceMatch(const WCHAR * szSelectedId); 
    BOOL IsBluetoothDevice();

private:
    LPWSTR m_pwszEndpointId;
};

// ----------------------------------------------------------
// Test Module Class declaration
class CLullabyDeviceTestModule: public CDeviceTestModule
{
public:
#ifndef BUILD_TAEF
    CLullabyDeviceTestModule(CTestShell* pShell, HINSTANCE hInstDLL);
#else
    CLullabyDeviceTestModule(DV_TCINFO * ptcInfo);
#endif
    virtual ~CLullabyDeviceTestModule(VOID);

    // CTestModule virtuals
    virtual DWORD   OnInitialUpdate(VOID);
    virtual VOID    SetTestDevice(CDeviceDescriptor* pDevice);
    virtual DWORD   EndTestRun(VOID) { return (FNS_PASS); }
    virtual DWORD   StartTestRun(VOID) { return (FNS_PASS); }
    virtual DWORD   RunTest(DWORD dwTestCase);

    CLullabyDevice *GetDevice()  const   { return m_pDevice;  }
    bool IsInternal() { return m_fInternal; }

private:
    CLullabyDevice   *m_pDevice; // current device under test
    bool m_fInternal;
    
    //power settings for wake timers
    DWORD m_dwSavedACValue;
    DWORD m_dwSavedDCValue;

    BOOL EnableWakeTimers();
    VOID RestoreWakeTimerSettings();
};


