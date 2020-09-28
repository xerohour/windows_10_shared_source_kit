/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/

#ifndef _INSTALL_H_
#define _INSTALL_H_

#ifndef __ATLSTR_H__
#error install.h requires atlstr.h
#endif

#include <vector>
using namespace std;

#ifndef _CFGMGR32_H_
#error install.h requires cfgmgr32.h
#endif

#ifndef _INC_SETUPAPI
#error install.h requires setupapi.h
#endif

#ifndef _HDAudioBus_Priv_Ioctl_H_Included_
#error install.h requires privioct.h
#endif

//
// types
//
enum enumDriverType { eNone, eMicrosoft, eThirdParty, eTest, eUnknown };

//
// class definitions
//
// WTT SDK interface class
// wttsdk.dll functions don't seem to work in DTM
// so call CreateProcess(wttcmd.exe /arg)
class CWTT
{
public:
    CWTT();
    ~CWTT();

public:
    bool Reboot(const bool fRestartTaskAfterReboot);
    bool SetAlternateHWID(CString& strAlternateHWIDValue);

private:
    bool CallWttCmd(LPCSTR szParameter);
};

// abstract base class for devices
class CPDO
{    
public:
    CPDO(DWORD dwDevInst, const GUID* pguidTestInterface) : 
      m_dwDevInst(dwDevInst), 
      m_strHardwareID(""), 
      m_strInstanceID(""),
      m_eDriverType(eNone),       
      m_strInfFileName(""), 
      m_pguidTestInterface(pguidTestInterface), 
      m_strDevicePath("")
      {};

public:
    bool InstallDriver(const CString& strInf, bool *pfShouldRunOnThisDevice, bool* pfNeedsReboot);
    CString GetHardwareID();
    CString GetInstanceID();

public:
      bool QueryHardwareID(HDEVINFO hSet, SP_DEVINFO_DATA* pDevinfodata);
      bool QueryDriver(HDEVINFO hSet, SP_DEVINFO_DATA* pDevinfodata);
protected:
    virtual bool InstallClassDrivers(bool* pfNeedsReboot) = 0;
    bool GetDevInfoData(HDEVINFO hSet, SP_DEVINFO_DATA* pDevinfodata);
    bool RestoreDriver(bool* pfNeedsReboot);
    virtual bool PopulateShellDeviceList() = 0;
private:
    bool ExposesInterface(HDEVINFO hSet, SP_DEVINFO_DATA* pDevinfodata, const GUID* pGUIDInterface);
      
// BUGBUG - this really should not be public
public:
    enumDriverType m_eDriverType;
protected:
    DWORD m_dwDevInst;
    CString m_strHardwareID;
    CString m_strInstanceID;
    CString m_strDevicePath;
private:    
    CString m_strInfFileName;
    const GUID* m_pguidTestInterface;
};

class CCodecPDO : public CPDO
{
    friend class CControllerPDO;

public:
    CCodecPDO(DWORD dwDevInst, DWORD dwControllerDevInst) :
        CPDO(dwDevInst, &GUID_HDAUDIO_DEBUG_UAATEST),
        m_dwControllerDevInst(dwControllerDevInst) {};

    bool InstallClassDrivers(bool* pfNeedsReboot);
    bool PopulateShellDeviceList();    

private:
    DWORD m_dwControllerDevInst;
};

class CControllerPDO : public CPDO
{
public:
    CControllerPDO(DWORD dwDevInst) : CPDO(dwDevInst, &GUID_HDCONTROLLER_TEST_INTERFACE_UAATEST) {};

    bool InstallClassDrivers(bool* pfNeedsReboot);
    bool InstallCodecTestDrivers(bool* pfNeedsReboot);
    bool RestoreDrivers(bool* pfNeedsReboot);
    bool PopulateShellDeviceList();

    bool EnumerateCodecs();

private:
    bool RequeryDriver();
    CString GetAlternateHWID();
private:    
    vector<CCodecPDO> m_vCodecs;
};

// this is the app's interface into the setup routines
class CInstaller
{
public:
    CInstaller();
    ~CInstaller();

public:
    bool DiscoverStartState();
    bool ConfigureDevices(bool* pfExiting);
    bool PopulateShellDeviceList();
    bool SetAlternateHWID(CString& strAlternateHWIDValue);

private:
    bool EnumerateControllers();
    bool InstallClassDrivers();
    bool InstallCodecTestDrivers();
    bool InstallCtrlTestDrivers();
    bool RestoreDrivers();
    bool Reboot(const bool fRestartTaskAfterReboot);
    bool RemoveTestDriversFromSystem();

public:
    enum { eShutDownTimer = 30 };
    static const CStringW s_wstrAlternateHWIDKey;

private:
    vector<CControllerPDO> m_vControllers;
    CWTT* m_pWTT;
    bool m_fRebooting;    
};

//
// helpers
//
CString MakeFullPath(const CString& strInf);

#endif _INSTALL_H_