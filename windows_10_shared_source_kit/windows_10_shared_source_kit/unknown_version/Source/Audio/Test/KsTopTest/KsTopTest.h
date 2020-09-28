// -----------------------------------------------------------------------------
// File:    KSTopTest.h
//
// Desc:    This file contains the definition of the classes needed to run 
//          shell98 and test cases.
//
//
// Copyright (C) Microsoft Corporation. All rights reserved.
// -----------------------------------------------------------------------------
#pragma once

// -----------------------------------------------------------------------------
// Device definition
class CKsDevice : public CDeviceDescriptor
{
    public:
        CKsDevice(CDevice *pDevice);
        ~CKsDevice(void) {}
        CDevice *GetTopoDevice() { return (m_pDevice); }
        
    private:
        CDevice    *m_pDevice;  //  CDevice definition from TopoLib.
};


// ----------------------------------------------------------
// Test Module Class declaration
class CKsDeviceTestModule: public CDeviceTestModule
{
    public:
#ifndef BUILD_TAEF
        CKsDeviceTestModule(CTestShell* pShell, HINSTANCE hInstDLL);
#else //#ifndef BUILD_TAEF
        CKsDeviceTestModule(DV_TCINFO * ptcInfo);
#endif //#ifndef BUILD_TAEF
        virtual ~CKsDeviceTestModule();

        // CTestModule virtuals
        virtual DWORD   OnInitialUpdate(VOID);
        virtual VOID    SetTestDevice(CDeviceDescriptor* pDevice);
        virtual DWORD   EndTestRun(VOID) { return (FNS_PASS); }
        virtual DWORD   StartTestRun(VOID) { return (FNS_PASS); }
        virtual DWORD   RunTest(DWORD dwTestID);
        CKsDevice      *GetDevice()  const   { return m_pDevice;  }
        CDevice        *GetKsDevice() { return (m_pDevice->GetTopoDevice()); }
        CEnumDevice    *GetEnumDevice() {return m_pDevices;}
        
    private:
        CKsDevice      *m_pDevice; // current device under test
        CEnumDevice    *m_pDevices;
};

BOOL ValidEndpoints();

size_t GetAudioDeviceCount();
