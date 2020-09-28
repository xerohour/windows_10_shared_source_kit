/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/

#ifndef UAATEST_H
#define UAATEST_H

// SetupAPI stuff
#ifndef _CFGMGR32_H_
#error uaatest.h requires cfgmgr32.h
#endif

#ifndef _INC_SETUPAPI
#error uaatest.h requires setupapi.h
#endif

// shell98 stuff
#ifndef _shell98_h
#error uaatest.h requires shell98.h
#endif

#ifndef _MODULE98_H
#error uaatest.h requires module98.h
#endif

#ifndef _DVMODULE_H
#error uaatest.h requires dvmodule.h
#endif

// local stuff
#ifndef _AzCodec_h_Included_
#error uaatest.h requires AzCodec.h
#endif

#ifndef _INSTALL_H_
#error uaatest.h requires install.h
#endif

//
// Prototypes
//
//DWORD Tc_NotImplemented();
CTestModule* WINAPI NewTestModule(IN CTestShell *pShell, IN HINSTANCE hinstDLL);

AzRirbEntry SendVerb(HANDLE hDriver, ULONG verb);

// ------------------------------------------------------------------------------
//
// Class declaration
//
class CUAATestModule : public CDeviceTestModule
{
    friend class CInstaller;
    friend class CControllerPDO;
    friend class CCodecPDO;

public:
    CUAATestModule(IN CTestShell *pShell, IN HINSTANCE hinstDLL);
    ~CUAATestModule();

public:
    DWORD OnInitialUpdate();
	//void EndApp();

    //DWORD StartTestCase();
    //DWORD EndTestCase();
    BOOL EnumerateDevices(BOOL fHDAudioCodecs);
    void SetTestDevice(CDeviceDescriptor* pDevice);
    BOOL CUAATestModule::ChangeLogProfileSetting();
    BOOL CUAATestModule::UndoLogProfileSetting();
    BOOL SleepAtLeast(DWORD dwMilliseconds);
    //DWORD GetFinalResult (DWORD dwResult);
    //VOID FinalResult ();
    //DWORD EndTestRun;

private:
    BOOL InstallClassDriver(LPCSTR szHardwareID);
    BOOL EnumerateChild(DEVINST diChild);
    BOOL EnumerateController(SP_DEVINFO_DATA devInfoData, HDEVINFO hSystemSet);
    BOOL CreateCodecDevice(DWORD dwControllerDevInst, LPCSTR szDevicePath, LPCSTR szInstanceID);
    BOOL CreateControllerDevice(LPCSTR szDevicePath, LPCSTR szInstanceID);
    BOOL IsKnownCodec(DWORD dwControllerDevInst, USHORT usSDI);
    BOOL Reboot();

public:
    CDeviceDescriptor* m_pCurDevice;
    BOOL m_fDeviceIDIsAnInstanceID;
    LARGE_INTEGER m_liQpcPerSecond;

private:
	BOOL m_fUseClassDriver;
    BOOL m_fWTT;
    BOOL m_fReplace;
    BOOL m_fRestart;
    BOOL m_fRestore;
    BOOL m_fNeedsReboot;
    BOOL m_fCodecTesting;	
    BOOL m_fCodecTestOnly;           // to test only codec
    BOOL m_fCntTestOnly;           // to test only Contorller
    DWORD dTestResult;              // to store result of the test - if dtestcycle  is zero then rest its value. if dTestCycle =1 then store value in ini
    // dTestResult will same as CtestShell::m_dwResult
    CInstaller m_Installer;
};


class CAFD : public CDeviceDescriptor
{    
public:
    CAFD(LPCSTR szDevicePath, LPCSTR szDevInstID)
    {
        strncpy(m_szDevicePath, szDevicePath, MAX_PATH);
        strncpy(m_szDevInstID, szDevInstID, MAX_PATH);
    };
public:
    CHAR m_szDevInstID[MAX_PATH];
private:    
    CHAR m_szDevicePath[MAX_PATH];
};

extern CUAATestModule* g_pUAATest;

#define DT_CODEC    0x1 // codec
#define DT_CTRL     0x2 // controller
#define DT_AFD      0x3 // audio function driver
#define DT_GEN      0x4 // non-device specific test

#endif // UAATEST_H
