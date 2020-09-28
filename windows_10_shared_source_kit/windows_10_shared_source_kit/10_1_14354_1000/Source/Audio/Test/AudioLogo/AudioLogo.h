//--------------------------------------------------------------------------
//
//  File: AudioLogo.h
//
//  Copyright (C) Microsoft. All rights reserved.
//
//  Abstract:
//      Header for all the Shell98 definitions.
//
//
//--------------------------------------------------------------------------
#pragma once

// ------------------------------------------------------------------------------
// Device definition
class CAudioLogoDevice : public CDeviceDescriptor
{
public:
    CAudioLogoDevice(LPCWSTR pszName, LPCWSTR pszId, LPARAM lpType, HRESULT &hr);
    CAudioLogoDevice(IMMDevice *pIEndpoint, HRESULT &hr);
    ~CAudioLogoDevice(void) {}
    LPCWSTR GetName() { return (&(m_szRep[0])); }
    IMMDevice *GetEndpoint() { return ((IMMDevice*)(m_pIEndpoint)); }
    // TODO: ylsuen-2010-06-16: 
    // Ideally LPCWSTR would be preferred, but const WCHAR * is the definition in CDeviceDescriptor.
    // Update CDeviceDescriptor in the future to use LPCWSTR
    BOOL DoesDeviceMatch(const WCHAR * szSelectedId); 

private:
    CComPtr<IMMDevice>      m_pIEndpoint;
};

// ----------------------------------------------------------
// Test Module Class declaration
class CAudioLogoDeviceTestModule: public CDeviceTestModule
{
public:
#ifndef BUILD_TAEF
    CAudioLogoDeviceTestModule(CTestShell* pShell, HINSTANCE hInstDLL);
#else
    CAudioLogoDeviceTestModule(DV_TCINFO * ptcInfo);
#endif
    virtual ~CAudioLogoDeviceTestModule(VOID);

    // CTestModule virtuals
    virtual DWORD   OnInitialUpdate(VOID);
    virtual VOID    SetTestDevice(CDeviceDescriptor* pDevice);
    virtual DWORD   EndTestRun(VOID) { return (FNS_PASS); }
    virtual DWORD   StartTestRun(VOID) { return (FNS_PASS); }
    virtual DWORD   RunTest(DWORD dwTestCase);
    
    CAudioLogoDevice *GetDevice()  const   { return m_pDevice;  }

private:
    CAudioLogoDevice   *m_pDevice; // current device under test
};


