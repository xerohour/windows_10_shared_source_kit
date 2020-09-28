/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/

#include <windows.h>
#include <tchar.h>
#include <cfgmgr32.h>
#include <setupapi.h>
#include <shell98.h>
#include <module98.h>
#include <dvmodule.h>
#include <atlstr.h>
#include <strsafe.h>
#define USERMODE
#include <azcodec.h>

#include <privioct.h>
#include <basicprintf.h>
#include <debug.h>
#include <doonexit.h>
#include <controller-interface.h>

#include "install.h"
#include "uaatest.h"
#include "setupdi.h"
#include "inimgr.h"
#include "buffer.h"
#include "util.h"
#include "log.h"

// ISSUE - with hybrid devices we only end up running codec tests, 
//      we should figure out a way to run controller tests too

// TODO - need to requery driver (ctrl and codec) whenever an install/restore action is taken

// TODO [FEATURE] - add back in all-in-one (non-WTT) logic

// TODO - SetupDiGetClassDevs leaking Set handles?
// TODO - possibly make g_pUAATest->m_fNeedsReboot just be part of CINstaller directly? what about fWTT?
// TODO - all init should be in constructor and use EH
// TODO - no more reinterpret_casts
// TODO - use "const" where appropriate
// TODO - restrict class access as appropriate (tighten it down)
// TODO - modify UpdateDevice to use a string class instead of c-strings ; update szInf usage here to correspond
// TODO - notation: use b* for bool and f* for BOOL (by* for BYTE)

// utility class... call LogStartTest and LogEndTest in constructor and destructor
class CLogStartEndTest
{
private:
    CTestShell *m_pTestShell;
    LPCTSTR m_szTestCase;
    LPCTSTR m_szCaseID;
    GUID m_guidTUID;

public:
    CLogStartEndTest(CTestShell *pTestShell, LPCTSTR szTestCase, LPCTSTR szCaseID, GUID guidTUID)
    : m_pTestShell(pTestShell)
    , m_szTestCase(szTestCase)
    , m_szCaseID(szCaseID)
    , m_guidTUID(guidTUID)
    {
        m_pTestShell->LogStartTest(m_szTestCase, m_szCaseID, m_guidTUID);
    }

    ~CLogStartEndTest()
    {
        // let XLOG take care of failures
        m_pTestShell->LogEndTest(m_szTestCase, m_szCaseID, m_guidTUID, FNS_PASS);
    }
};

static const CString g_strTestFDName("t_hdaud.inf");
static const CString g_strClassFDName("hdaudio.inf");
static const CString g_strTestBDName("hdaudbustest.inf");
static const CString g_strClassBDName("hdaudbus.inf");

CInstaller::CInstaller()
{
    m_fRebooting = false;
    m_pWTT = NULL;
}
CInstaller::~CInstaller()
{
    delete m_pWTT;
}

bool 
CInstaller::DiscoverStartState()
{
    //establish environment
    SLOG(eInfo1, "UAATest %s coordinate configuration with WTT.", g_pUAATest->m_fWTT ? "will" : "will NOT");
    if (g_pUAATest->m_fWTT)
    {
        m_pWTT = new CWTT;
    }

    // dig up information about all HDAudio controllers on the system
    //  this will also enumerate all HDAudio codecs (children) on each respective controller    
    if (!EnumerateControllers())
    {
        SLOG(eError, "Error: Failed EnumerateControllers!");
        return false;
    }

    return true;
}

//
// This is where Setup/Restore Actions should take place
//
bool 
CInstaller::ConfigureDevices(bool* pfExiting)
{
/*
Actions:
    1) Install codec test drivers
    // replace codec (-h opt)
    2) Install controller test drivers
    // replace cnt (-h opt)    
    3) Install class drivers
    // replace class codec
    4) Restore drivers
    // restore (codec/cnt?) (-h opt?)
    5) All-in-one (non-WTT/non-class) run
    6) unsupported scenarios (??)
*/
    if (g_pUAATest->m_fReplace)
    {
        if (g_pUAATest->m_fUseClassDriver)
        {
            // #3
            SLOG(eInfo1, "Installing class drivers");
            if (!InstallClassDrivers())
            {
                SLOG(eError, "Error: Failed InstallClassDrivers!");
                return false;
            }
        }
        else if (g_pUAATest->m_fCodecTestOnly)
        {
            // #1
            SLOG(eInfo1, "Installing codec test drivers");
            if (!InstallCodecTestDrivers())
            {
                SLOG(eError, "Error: Failed InstallCodecTestDrivers!");
                return false;
            }
        }
        else if (g_pUAATest->m_fCntTestOnly)
        {
            // #2
            SLOG(eInfo1, "Installing controller test drivers");
            if (!InstallCtrlTestDrivers())
            {
                SLOG(eError, "Error: Failed InstallCtrlTestDrivers!");
                return false;
            }
        }
        else if (g_pUAATest->m_fRestore)
        {
            // #5
            SLOG(eError, "Error: Unsupported run option [All-in-one].");
            return false;
        }
        else
        {
            SLOG(eError, "Error: Unsupported run option.");
            return false;
        }
    }
    else if (g_pUAATest->m_fRestore)
    {
        // #4
        SLOG(eInfo1, "Restoring drivers");
        if (!RestoreDrivers())
        {
            SLOG(eError, "Error: Failed RestoreDrivers!");
            return false;
        }
    }
    else
    {
        // no config action requested
        return true;
    }

    *pfExiting = false;
    if (g_pUAATest->m_fNeedsReboot)
    {
        *pfExiting = true;
        XLOG(XMSG, eWarn1, "Warning: System needs to reboot for new driver(s) to be installed.");
        if (!Reboot(false))
        {
            SLOG(eError, "Error: Failed Reboot!");
            return false;
        }
    }    

    // for WTT, we need to force the app to bail out so that the next task can run
    if (g_pUAATest->m_fWTT)
    {
        *pfExiting = true;
        XLOG(XMSG, eError, "%s", "Exiting app so WTT can resume next task...");
        PostQuitMessage(0);
    }

    return true;
}

bool 
CInstaller::EnumerateControllers()
{    
    // enumerate all PCI devices
    HDEVINFO hPCISet = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES);
    if (!IsValidHandle(hPCISet))
    {
        SLOG(eError, "Error: Failed to get Device Information Set of PCI devices");
        return false;
    }

    
    // for each device
    // scan through all compatible IDs to see if we can find the HDAudio class codes:
    // PCI\CC_0403 or ACPI\CLS_0004&SUBCLS_0003
    // these will be the HDAudio controllers
    SP_DEVINFO_DATA devinfodata;
    for (ULONG ulMember = 0; ; ulMember++)
    {
        devinfodata.cbSize = sizeof(SP_DEVINFO_DATA);
        if (!SetupDiEnumDeviceInfo(hPCISet, ulMember, &devinfodata))
        {
            DWORD dwErrorCode = GetLastError();
            if (ERROR_NO_MORE_ITEMS == dwErrorCode)
                break;
            else
            {
                SLOG(eError, "Error: Failed SetupDiEnumDeviceInfo with error code = %u", dwErrorCode);
                return false;
            }
        }
        //SLOG(eInfo1, "devinfodata.DevInst == %u", devinfodata.DevInst);
        
        DWORD cbBufSize = 0;
        SetupDiGetDeviceRegistryProperty(hPCISet, 
                                         &devinfodata, 
                                         SPDRP_COMPATIBLEIDS, 
                                         NULL, 
                                         NULL, 
                                         0, 
                                         &cbBufSize);
        if (0 == cbBufSize)
            continue;

        // we shouldn't use auto_ptr for an array (since it uses delete instead of delete[]), but this is just a buffer
        auto_ptr<BYTE> pcBuf(new BYTE[cbBufSize]);
        if (!SetupDiGetDeviceRegistryProperty(hPCISet, 
                                              &devinfodata, 
                                              SPDRP_COMPATIBLEIDS, 
                                              NULL, 
                                              pcBuf.get(), 
                                              cbBufSize, 
                                              NULL))
        {
            SLOG(eError, "  Error: Failed to get Compatible IDs for device instance");
            SLOG(eError, "    GetLastError == %u", GetLastError());
            return false;
        }
        //SLOG(eInfo1, "  Checking Device Instance...");
        
        // can't find an API for working with REG_MULTI_SZ strings so I'll do it myself
        // BUGBUG - might want to change this to use a string class + functions as opposed to the old CRT stuff
        CHAR* szCompID = reinterpret_cast<CHAR*>(pcBuf.get());
        while (strcmp(szCompID, ""))
        {
            // check if this matches the HDAudio class code (CC_0403)
            // this is specified in a Microsoft white paper
            // http://www.microsoft.com/whdc/device/audio/HD-aud_PnP.mspx
            //
            // or the ACPI-enumerated compatible ID; this was added in Windows 8
            if (
                0 == _stricmp("PCI\\CC_0403", szCompID) ||
                0 == _stricmp("ACPI\\CLS_0004&SUBCLS_0003", szCompID)
            )
            {
                // Got an HDAudio controller
                SLOG(eInfo1, "HDAudio Controller: devinfodata.DevInst = %u; compat ID = %s", devinfodata.DevInst, szCompID);
                CControllerPDO NewController(devinfodata.DevInst);

                // get the Hardware ID                
                if (!NewController.QueryHardwareID(hPCISet, &devinfodata))
                {
                    SLOG(eError, "Error: Failed QueryHardwareID!");
                    return false;
                }

                // dig out information about the driver
                if (!NewController.QueryDriver(hPCISet, &devinfodata)) // [None, MSBus, ThirdParty, Test]
                {
                    SLOG(eError, "Error: Failed QueryDriver[%u]!", ulMember);
                    return false;
                }

                // BUGBUG - should I enumerate codecs automatically if the bus driver is loaded instead of calling it explicitly here?
                //  - alternatively, I could explicitly do this back in the DiscoverStartState routine
                if (eMicrosoft == NewController.m_eDriverType)
                {                    
                    if (!NewController.EnumerateCodecs())
                    {
                        SLOG(eError, "Error: Failed EnumerateCodecs!");
                        return false;
                    }
                }

                // add it to our vector and move on to the next device
                m_vControllers.push_back(NewController);
                break;
            }

            // get the next compatible ID
            szCompID += strlen(szCompID)+1;
        } // while

    } // for
    
    return true;
}

bool 
CPDO::QueryHardwareID(HDEVINFO hSet, SP_DEVINFO_DATA* pDevinfodata)
{

    // get the Hardware ID
    DWORD cbBufSize = 0;
    SetupDiGetDeviceRegistryProperty(hSet,
                                     pDevinfodata, 
                                     SPDRP_HARDWAREID, 
                                     NULL, 
                                     NULL, 
                                     0, 
                                     &cbBufSize);    
    // we shouldn't use auto_ptr for an array (since it uses delete instead of delete[]), but this is just a buffer
    auto_ptr<BYTE> szHardwareID(new BYTE[cbBufSize]);    
    if (!SetupDiGetDeviceRegistryProperty(hSet, 
                                          pDevinfodata, 
                                          SPDRP_HARDWAREID, 
                                          NULL, 
                                          szHardwareID.get(), 
                                          cbBufSize, 
                                          NULL))
    {
        SLOG(eError, "Error: Failed SetupDiGetDeviceRegistryProperty!");
        return false;
    }
    m_strHardwareID = reinterpret_cast<CHAR*>(szHardwareID.get());
    SLOG(eInfo1, " szHardwareID == %s", (LPCSTR)m_strHardwareID);

    return true;
}

bool 
CPDO::QueryDriver(HDEVINFO hSet, SP_DEVINFO_DATA* pDevinfodata)
{

    SLOG(eInfo1, "  Examining currently installed driver");

    DWORD cchBufSize = 0;
    SetupDiGetDeviceInstanceId(hSet, 
                               pDevinfodata, 
                               NULL, 
                               0, 
                               &cchBufSize);
    BOOL bOK = SetupDiGetDeviceInstanceId(hSet, 
                                          pDevinfodata, 
                                          m_strInstanceID.GetBuffer(cchBufSize), 
                                          cchBufSize, 
                                          NULL);
    m_strInstanceID.ReleaseBuffer();
    if (!bOK)
    {
        SLOG(eError, "  Error: Failed SetupDiGetDeviceInstanceId!");
        SLOG(eError, "    GetLastError == %u", GetLastError());
        return false;
    }

    SP_DEVINSTALL_PARAMS deviceinstallparams;
    deviceinstallparams.cbSize = sizeof(SP_DEVINSTALL_PARAMS);
    if (!SetupDiGetDeviceInstallParams(hSet, pDevinfodata, &deviceinstallparams))
    {
        SLOG(eError, "  Error: Failed SetupDiGetDeviceInstallParams!");
        return false;
    }
    
    deviceinstallparams.FlagsEx |= DI_FLAGSEX_INSTALLEDDRIVER;
    if (!SetupDiSetDeviceInstallParams(hSet, pDevinfodata, &deviceinstallparams))
    {
        SLOG(eError, "  Error: Failed SetupDiSetDeviceInstallParams!");
        return false;
    }

    if (!SetupDiBuildDriverInfoList(hSet, pDevinfodata, SPDIT_COMPATDRIVER))
    {
        SLOG(eError, "  Error: Failed to build Driver Info List");
        SLOG(eError, "  GetLastError == %u", GetLastError());
        SLOG(eError, "  GetLastError == %08Xh", GetLastError());
        return false;
    }

    // this assumes only one driver is installed for the device
    SP_DRVINFO_DATA driverinfodata;
    driverinfodata.cbSize = sizeof(SP_DRVINFO_DATA);
    if (!SetupDiEnumDriverInfo(hSet, pDevinfodata, SPDIT_COMPATDRIVER, 0, &driverinfodata))
    {
        if (ERROR_NO_MORE_ITEMS != GetLastError())
        {
            SLOG(eError, "Error: Failed SetupDiEnumDriverInfo! Error code = %u", GetLastError());
            return false;
        }
        m_strInfFileName = "";
        m_eDriverType = eNone;
        SLOG(eInfo1, "   No driver is currently installed for this device.");
    }
    else
    {        
        SLOG(eInfo1, "   driverinfodata.Description == %s", driverinfodata.Description);                    

        DWORD cbBufSize = 0;
            SetupDiGetDriverInfoDetail(hSet, 
                                       pDevinfodata, 
                                   &driverinfodata, 
                                   NULL, 
                                   0, 
                                   &cbBufSize);
        //SLOG(eInfo1, "cbBufSize == %u", cbBufSize);
        // we shouldn't use auto_ptr for an array (since it uses delete instead of delete[]), but this is just a buffer
        auto_ptr<SP_DRVINFO_DETAIL_DATA> pdriverinfodetaildata((SP_DRVINFO_DETAIL_DATA*)new BYTE[cbBufSize]);
        pdriverinfodetaildata->cbSize = sizeof(SP_DRVINFO_DETAIL_DATA);
        if (!SetupDiGetDriverInfoDetail(hSet, 
                                        pDevinfodata, 
                                    &driverinfodata, 
                                    pdriverinfodetaildata.get(), 
                                    cbBufSize, 
                                    NULL))
        {
            SLOG(eError, "  Error: Could not get Driver Info Detail Data");
            SLOG(eError, "    GetLastError == %u", GetLastError());            
            return false;
        }
        
        SLOG(eInfo1, "   pdriverinfodetaildata->InfFileName == %s", pdriverinfodetaildata->InfFileName);
        m_strInfFileName = pdriverinfodetaildata->InfFileName;
                
        // Is it OS, Test, or ThirdParty driver?
        if (ExposesInterface(hSet, pDevinfodata, m_pguidTestInterface))
        {
            // only the test driver exposes the test interface
            SLOG(eInfo1, "  Microsoft Test Driver is loaded");
            m_eDriverType = eTest;
        }
        else if
        (
            StringEndsWith(pdriverinfodetaildata->InfFileName, "\\hdaudio.inf") ||
            StringEndsWith(pdriverinfodetaildata->InfFileName, "\\hdaudbus.inf")
        )
        {
            // only the Microsoft driver has an .inf name hdaudio.inf or hdaudbus.inf
            // (third-party drivers have their .inf name changed to oem##.inf)
            SLOG(eInfo1, "  Microsoft OS Driver is loaded");
            m_eDriverType = eMicrosoft;
        }
        else
        {
            SLOG(eInfo1, "  Third Party Driver is loaded");
            m_eDriverType = eThirdParty;
        }
    }

    return true;
}

// I intentionally made this take the interface guid as an argument instead of assuming m_pguidTestInterface.
// This way, if we ever want to look for different interfaces we can re-use this function.
bool 
CPDO::ExposesInterface(HDEVINFO hSet, SP_DEVINFO_DATA* pDevinfodata, const GUID* pGUIDInterface)
{
    
    //SLOG(eError, "Checking for %s", szDevInstanceID.get());
    hSet = SetupDiGetClassDevsEx(pGUIDInterface, 
                                 m_strInstanceID, 
                                 NULL, 
                                 DIGCF_PRESENT | DIGCF_DEVICEINTERFACE, 
                                 hSet, 
                                 NULL, 
                                 NULL);
    if (!IsValidHandle(hSet))
    {
        SLOG(eError, "Error: Failed SetupDiGetClassDevsEx with error code = %u", GetLastError());
        return false;
    }    

    // If the test driver is installed this will be the first and only device interface in the set
    //  Otherwise, SetupDiEnumDeviceInterfaces will fail with a cryptic GetLastError()==1447 (ERROR_NO_SCROLLBARS)
    SP_DEVICE_INTERFACE_DATA devintdata;
    devintdata.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    if (!SetupDiEnumDeviceInterfaces(hSet, 
                                     pDevinfodata, 
                                     pGUIDInterface, 
                                     0, 
                                     &devintdata))
    {
        return false;
    }
    else
    {
        // get the device path
        DWORD cbBufSize = 0;
        SetupDiGetDeviceInterfaceDetail(hSet, 
                                        &devintdata, 
                                        NULL, 
                                        0, 
                                        &cbBufSize, 
                                        NULL);
        auto_ptr<SP_DEVICE_INTERFACE_DETAIL_DATA> pdevintdetaildata((SP_DEVICE_INTERFACE_DETAIL_DATA*)new BYTE[cbBufSize]);
        pdevintdetaildata->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        if (!SetupDiGetDeviceInterfaceDetail(hSet, 
                                             &devintdata, 
                                             pdevintdetaildata.get(), 
                                             cbBufSize, 
                                             NULL, 
                                             NULL))
        {
            SLOG(eError, "Error: Could not get device interface details!");
            SLOG(eError, "  GetLastError == %u", GetLastError());
            return false;
        }
        SLOG(eInfo1, "   Path: %s", pdevintdetaildata->DevicePath);
        m_strDevicePath = pdevintdetaildata->DevicePath;
    }

    return true;
}

bool 
CControllerPDO::EnumerateCodecs()
{
    // get our working set of HDAudio\* devices
    HDEVINFO hHDAudioSet = SetupDiGetClassDevs(NULL, "HDAUDIO", NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES);
    if (!IsValidHandle(hHDAudioSet))
    {
        SLOG(eError, "Error: Failed to get Device Information Set for HDAUDIO enumerator devices");
        return false;
    }

    // enumerate all children using CM_* API
    DEVINST diChild = 0;
    CONFIGRET crRes = CM_Get_Child(&diChild, m_dwDevInst, 0);
    if (CR_SUCCESS != crRes)
    {
        if (CR_NO_SUCH_DEVNODE == crRes)
        {
            XLOG(XWARN, eWarn1, "Warning: Controller has no child devices.");
            return true;
        }
        else
        {
            SLOG(eError, "Error: Failed CM_Get_Child! [0x%08x]", crRes);
            return false;
        }
    }
    DEVINST diSibling = 0;
    while (CR_SUCCESS == crRes)
    {
        SLOG(eInfo1, " diChild == %u", diChild);
        CCodecPDO NewCodec(diChild, m_dwDevInst);

        // get the devinfodata object
        // the device instance handle is used to correlate the device between CM_API and SetupAPI
        SP_DEVINFO_DATA devinfodata;
        if (!NewCodec.GetDevInfoData(hHDAudioSet, &devinfodata))
        {
            SLOG(eError, "Error: Failed CCodecPDO::GetDevInfoData!");
            return false;
        }
        
        if (!NewCodec.QueryHardwareID(hHDAudioSet, &devinfodata))
        {
            SLOG(eError, "Error: Failed QueryHardwareID!");
            return false;
        }

        // dig out information about the driver
        if (!NewCodec.QueryDriver(hHDAudioSet, &devinfodata)) // [None, MSBus, ThirdParty, Test]
        {
            SLOG(eError, "Error: Failed QueryDriver!");
            return false;
        }
        
        // add it to our vector
        m_vCodecs.push_back(NewCodec);

        // get the next child of the bus driver
        crRes = CM_Get_Sibling(&diSibling, diChild, 0);
        diChild = diSibling;
    }
    if (CR_NO_SUCH_DEVNODE != crRes)
    {
        SLOG(eError, "Error: Failed CM_Get_Sibling! [0x%08x]", crRes);
        return false;
    }

    return true;
}


bool 
CPDO::GetDevInfoData(HDEVINFO hSet, SP_DEVINFO_DATA* pDevinfodata)
{
    SP_DEVINFO_DATA devinfodata;

    //
    // we need to correlate the Device Instance Handle with a SetupAPI enumerated devinfodata structure
    //
    DWORD dwErrorCode = ERROR_SUCCESS;
    bool fFound = false;
    for (ULONG ulMember = 0; !fFound && (ERROR_NO_MORE_ITEMS != dwErrorCode); ulMember++)
    {
        //SLOG(eInfo1, "ulMember == %u", ulMember);

        // get the device info structure
        devinfodata.cbSize = sizeof(SP_DEVINFO_DATA);
        if (!SetupDiEnumDeviceInfo(hSet, ulMember, &devinfodata))
        {
            dwErrorCode = GetLastError();
            if (ERROR_NO_MORE_ITEMS != dwErrorCode)
            {                
                SLOG(eError, "Error: Failed SetupDiEnumDeviceInfo with error code = %u", dwErrorCode);
                return false;
            }
        }

        if (devinfodata.DevInst == m_dwDevInst)
        {
            // found a match for the device using SetupAPI
            *pDevinfodata = devinfodata;
            fFound = true;
        }
    }

    return fFound;
}

bool 
CInstaller::InstallClassDrivers()
{
    // cycle through list of controllers and codecs
    //  if no class driver on controller, install it there
    //      is it safe to assume that the codec function driver will get installed automagically?
    //      what if only the codec test driver was previously installed? will it pick that one instead of the class driver?
    //  else install class driver on all codecs

    GUID guidInstallClassDrivers = {0x6e5e119f, 0x2c6, 0x4f0b, 0xb4, 0x5e, 0x0, 0x4e, 0xe3, 0x9d, 0xd4, 0x3c};

    CLogStartEndTest log(g_pUAATest->m_pShell, "Install class drivers", "", guidInstallClassDrivers);

    bool fReboot = false;
    vector<CControllerPDO>::iterator iControllers = m_vControllers.begin();
    for (;iControllers != m_vControllers.end(); iControllers++)
    {
        if (!iControllers->InstallClassDrivers(&fReboot))
        {
            SLOG(eError, "Error: Failed CControllerPDO::InstallClassDrivers!");
            return false;
        }
        g_pUAATest->m_fNeedsReboot = fReboot ? TRUE : g_pUAATest->m_fNeedsReboot;
    }

    return true;
}

bool 
CControllerPDO::InstallClassDrivers(bool* pfNeedsReboot)
{
    bool fReboot = false;    

    if (eMicrosoft == m_eDriverType)
    {
        vector<CCodecPDO>::iterator iCodecs = m_vCodecs.begin();
        for (;iCodecs != m_vCodecs.end(); iCodecs++)
        {
            if (!iCodecs->InstallClassDrivers(&fReboot))
            {
                SLOG(eError, "Error: Failed CCodecPDO::InstallClassDrivers!");
                return false;
            }
            *pfNeedsReboot = fReboot ? true : *pfNeedsReboot;
        }
    }
    else
    {
        // first need to install the Microsoft Bus Driver
        const CString strInf = MakeFullPath(g_strClassBDName);
        SLOG(eInfo1, "Installing Microsoft HD Audio bus driver (%s) on PCI\\CC_0403 device", static_cast<LPCSTR>(strInf));

        bool fShouldRunOnThisDevice = false;
        
        if (!InstallDriver(strInf, &fShouldRunOnThisDevice, &fReboot))
        {
            SLOG(eError, "Error: Failed InstallDriver!");
            return false;
        }
        *pfNeedsReboot = fReboot ? true : *pfNeedsReboot;
        
        if (fReboot)
        {
            SLOG(eError, " NYI: Hybrid-device requires reboot to install bus driver!");
            return false;            
            
            // HEREHERE
            // BUGBUG - we don't know how to set CInstaller::m_strAlternateHWIDValue, since the bus driver is not currently loaded
            // could set it to UNKNOWN
            // then take the -h pnpid (or current one if no -h) and write in it's child's hwid
            // assume that there is only 1 audio codec per hybrid device
            /*            
            if (!g_pUAATest->m_Installer.SetAlternateHWID("UNKNOWN"))
            {
                SLOG(eError, "Failed SetAlternateHWID!");
                return false;
            }
            
            return CInstaller::Reboot(true);
            */
        }
        else if (fShouldRunOnThisDevice)
        {
            if (!RequeryDriver())
            {
                SLOG(eError, "Error: Failed RequeryDriver!");
                return false;
            }

            if (0 == m_vCodecs.size()) {
                SLOG(eError,
                    "Error: No HD Audio codecs exposed "
                    "after installing Microsoft HD Audio bus driver on "
                    "PCI\\CC_0403 audio device! Device is not UAA-compliant."
                );
                return false;
            }

            if (g_pShell->m_pszHwIdTestDeviceSelect)
            {            
                // we need to notify the environment that the audio device's PNPID will be different now (codec instead of controller)
                // do this before the InstallClassDrivers call since -h parameter may change
                CString szAlternateHWID = GetAlternateHWID();
                SLOG(eInfo1, "Changing WDKDeviceId from (%s) to (%s)",
                    g_pShell->m_pszHwIdTestDeviceSelect,
                    static_cast<LPCSTR>(szAlternateHWID)
                );

                if (!g_pUAATest->m_Installer.SetAlternateHWID(szAlternateHWID))
                {
                    SLOG(eError, "Failed SetAlternateHWID!");
                    return false;
                }
            }

            // call ourself again - this will install the class driver on the codecs
            if (!InstallClassDrivers(pfNeedsReboot))
            {
                SLOG(eError, "Error: Failed InstallClassDrivers!");
                return false;
            }            
        }
    }

    return true;
}

CString 
CControllerPDO::GetAlternateHWID()
{
    if (0 == m_vCodecs.size())
    {
        SLOG(eError, "GetAlternateHWID called but there are no codecs exposed!");
        return "PCI\\CC_0403";
    }

    if (g_pUAATest->m_fDeviceIDIsAnInstanceID)
    {
        vector<CCodecPDO>::iterator iCodec = m_vCodecs.begin();

        for (; iCodec != m_vCodecs.end(); iCodec++)
        {
            CString szInstanceID = iCodec->GetInstanceID();

            if (-1 == szInstanceID.Find("HDAUDIO\\FUNC_01"))
            {
                SLOG(eError, "GetAlternateHWID, codec is not an HD audio codec! %s.",(LPCSTR)szInstanceID);
            }
            else
            {
                return iCodec->GetInstanceID();
            }
        }
    }

    return (m_vCodecs.begin())->GetHardwareID();
}

CString 
CPDO::GetHardwareID()
{
    return m_strHardwareID;
}

CString 
CPDO::GetInstanceID()
{
    return m_strInstanceID;
}

bool 
CInstaller::SetAlternateHWID(CString& strAlternateHWIDValue)
{
    if (!g_pShell->m_pszHwIdTestDeviceSelect)
    {
        // no device specified on cmdline, so no need to change anything
        return true;
    }

    if (g_pUAATest->m_fNeedsReboot)
    {
        SLOG(eError, " NYI: Can not set AlternateHWID because bus driver needs a reboot!");
        return false;
    }
    
    if (g_pShell->m_pszHwIdTestDeviceSelect)
    {
        UINT nChars = strAlternateHWIDValue.GetLength() + 1;
        // device specified on cmdline, we need to use the new hwid now
        SafeLocalFree(g_pShell->m_pszHwIdTestDeviceSelect);
        g_pShell->m_pszHwIdTestDeviceSelect = (LPTSTR)LocalAlloc(LPTR, nChars * sizeof(TCHAR));
        strcpy_s(g_pShell->m_pszHwIdTestDeviceSelect, nChars, strAlternateHWIDValue);
    }

    SLOG(eInfo1, "Setting HWID to %s", (LPCSTR)strAlternateHWIDValue);
    if (g_pUAATest->m_fWTT)
    {
        if (!m_pWTT->SetAlternateHWID(strAlternateHWIDValue))
        {
            SLOG(eError, "Failed SetAlternateHWID!");
            return false;
        }
    }
    else
    {
        // no need to do anything else
    }
    return true;
}

bool 
CControllerPDO::RequeryDriver()
{
    // build a set and devinfodata structure representing this controller
    HDEVINFO hPCISet = SetupDiGetClassDevs(NULL, "PCI", NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES);
    if (!IsValidHandle(hPCISet))
    {
        SLOG(eError, "Error: Failed to get Device Information Set of System PCI devices");
        return false;
    }
    SP_DEVINFO_DATA devinfodata;
    if (!GetDevInfoData(hPCISet, &devinfodata))
    {
        SLOG(eError, "Error: Failed CCodecPDO::GetDevInfoData!");
        return false;
    }            

    // populate information about the newly installed driver
    if (!QueryDriver(hPCISet, &devinfodata))
    {
        SLOG(eError, "Error: Failed QueryDriver!");
        return false;
    }

    // enumerate the codecs now being created by the MSBD
    if (!EnumerateCodecs())
    {
        SLOG(eError, "Error: Failed EnumerateCodecs!");
        return false;
    }
    
    return true;
}

bool 
CCodecPDO::InstallClassDrivers(bool* pfNeedsReboot)
{
    if (eMicrosoft != m_eDriverType)
    {
        // only install on audio devices, not modem
        if (-1 == m_strHardwareID.Find("HDAUDIO\\FUNC_01"))
        {    
            SLOG(eInfo1, "   Skipping non-audio device");
        }
        else
        {
            const CString strInf = MakeFullPath(g_strClassFDName);
            bool fShouldRunOnThisDevice = false;
            if (!InstallDriver(strInf, &fShouldRunOnThisDevice, pfNeedsReboot))
            {
                SLOG(eError, "Error: Failed InstallDriver!");
                return false;
            }            
        }
    }

    return true;
}

bool 
CPDO::InstallDriver(const CString& strInf, bool *pfShouldRunOnThisDevice, bool *pfNeedsReboot)
{
    //SLOG(eInfo1, "m_strHardwareID == %s", m_strHardwareID);
    //SLOG(eInfo1, "g_pShell->m_pszHwIdTestDeviceSelect == %s", g_pShell->m_pszHwIdTestDeviceSelect);
    *pfShouldRunOnThisDevice = false;

    if (g_pShell->m_pszHwIdTestDeviceSelect)
    {
        // -h flag specified

        // does this exactly match?
        if (0 == m_strHardwareID.CompareNoCase(g_pShell->m_pszHwIdTestDeviceSelect))
        {
            SLOG(eInfo1, "Hardware ID (%s) matches -h flag (%s)", (LPCSTR)m_strHardwareID, g_pShell->m_pszHwIdTestDeviceSelect);
            // this device matches the -h flag
            *pfShouldRunOnThisDevice = true;
        }
        else if (0 == strncmp(m_strHardwareID, g_pShell->m_pszHwIdTestDeviceSelect, m_strHardwareID.GetLength()))
        {
            // m_strHardwareID is a prefix of the -h flag
            // this could happen if the -h flag is an Instance ID
            // (m_strHardwareID is always a Hardware ID)
            SLOG(eInfo1, "Hardware ID (%s) matches -h flag (%s)", (LPCSTR)m_strHardwareID, g_pShell->m_pszHwIdTestDeviceSelect);
            *pfShouldRunOnThisDevice = true;
            g_pUAATest->m_fDeviceIDIsAnInstanceID = TRUE;
        }
    }
    else
    {
        // no -h flag specified; all devices are interesting.
        *pfShouldRunOnThisDevice = true;
    }
    
    if (*pfShouldRunOnThisDevice)
    {
        if (eNone != m_eDriverType)
        {
            // Stash away the original driver info
            WriteIniString("OriginalDrivers", m_strHardwareID, m_strInfFileName);
        }

        SLOG(eInfo1, "   Installing %s on %s.", (LPCSTR)strInf, (LPCSTR)m_strHardwareID);
        BOOL fReboot = FALSE;
        if (!UpdateDevice(strInf, m_strHardwareID, &fReboot))
        {
            SLOG(eError, "Error: UpdateDevice failed to update the driver.");
            return false;
        }
        *pfNeedsReboot = fReboot ? true : false;
    }
    else
    {
        SLOG(eInfo1,
            "   Skipping installation on (%s) "
            "because it doesn't match -h flag (%s).",
            (LPCSTR)m_strHardwareID,
            g_pShell->m_pszHwIdTestDeviceSelect
        );
    }

    return true;
}

bool 
CInstaller::RestoreDrivers()
{
    bool fResult = true;

    GUID guidRestoreDrivers = {0xa451d41e, 0x9947, 0x43e0, 0x95, 0xdc, 0xa9, 0xb9, 0xc8, 0xc5, 0x64, 0x84};

    CLogStartEndTest log(g_pUAATest->m_pShell, "Restore original drivers", "", guidRestoreDrivers);

    XLOG(XMSG, eWarn1, "Warning: Restoring original drivers.");

    bool fReboot = false;
    vector<CControllerPDO>::iterator iControllers = m_vControllers.begin();
    for (;iControllers != m_vControllers.end(); iControllers++)
    {
        if (iControllers->RestoreDrivers(&fReboot))
        {
            g_pUAATest->m_fNeedsReboot = fReboot ? TRUE : g_pUAATest->m_fNeedsReboot;
        }
        else
        {
            SLOG(eError, "Error: Failed CControllerPDO::RestoreDrivers!");
            fResult = false; // soldier on
        }
    }

    if (!RemoveTestDriversFromSystem())
    {
        SLOG(eError, "Error: Failed CInstaller::RemoveTestDriversFromSystem!");
        fResult = false;
    }

    return fResult;
}

bool 
CControllerPDO::RestoreDrivers(bool* pfNeedsReboot)
{
    bool fReboot = false;

    // restore codec drivers FIRST!
    // otherwise we run into a problem with hybrid devices
    vector<CCodecPDO>::iterator iCodecs = m_vCodecs.begin();
    for (;iCodecs != m_vCodecs.end(); iCodecs++)
    {
        if (!iCodecs->RestoreDriver(&fReboot))
        {
            SLOG(eError, "Error: Failed RestoreDriver!");
            return false;
        }
        *pfNeedsReboot = fReboot ? true : *pfNeedsReboot;
    }

    if (!RestoreDriver(&fReboot))
    {
        SLOG(eError, "Error: Failed RestoreDriver!");
        return false;
    }
    *pfNeedsReboot = fReboot ? true : *pfNeedsReboot;

    return true;
}

bool 
CPDO::RestoreDriver(bool* pfNeedsReboot)
{
    BOOL fReboot = FALSE;
    CString strInf;
    
    if (GetIniString("OriginalDrivers", m_strHardwareID, strInf.GetBuffer(MAX_PATH), MAX_PATH))
    {
        strInf.ReleaseBuffer();
        SLOG(eInfo1, " Restoring %s", (LPCSTR)m_strHardwareID);
        SLOG(eInfo1, "  using %s", (LPCSTR)strInf);
        if (!UpdateDevice(CString(strInf).GetBuffer(), CString(m_strHardwareID).GetBuffer(), &fReboot))
        {
            SLOG(eError, "Error: UpdateDevice failed to update the driver.");
            SLOG(eError, "GetLastError == %u", GetLastError());
            return false;
        }
        *pfNeedsReboot = fReboot ? true : false;
    }
    else
    {
        // not going to fail if we couldn't find it
    }

    return true;
}

bool 
CInstaller::InstallCodecTestDrivers()
{
    GUID guidInstallCodecTestDrivers = {0x91b09477, 0x6d53, 0x4eda, 0x92, 0x62, 0x8e, 0x6b, 0xb7, 0x84, 0x2a, 0x71};

    CLogStartEndTest log(g_pUAATest->m_pShell, "Install codec test drivers", "", guidInstallCodecTestDrivers);

    bool fReboot = false;
    vector<CControllerPDO>::iterator iControllers = m_vControllers.begin();
    for (;iControllers != m_vControllers.end(); iControllers++)
    {
        if (!iControllers->InstallCodecTestDrivers(&fReboot))
        {
            SLOG(eError, "Error: Failed CControllerPDO::InstallCodecTestDrivers!");
            return false;
        }
        g_pUAATest->m_fNeedsReboot = fReboot ? TRUE : g_pUAATest->m_fNeedsReboot;
    }    

    return true;
}

bool 
CControllerPDO::InstallCodecTestDrivers(bool* pfNeedsReboot)
{
    bool fReboot = false;

    if (eMicrosoft == m_eDriverType)
    {
        vector<CCodecPDO>::iterator iCodecs = m_vCodecs.begin();
        for (;iCodecs != m_vCodecs.end(); iCodecs++)
        {
            if (eTest == iCodecs->m_eDriverType)
            {
                SLOG(eInfo1, "Skipping: Test driver already installed for this device.");
            }
            else
            {
                bool fShouldRunOnThisDevice = false;
                if (!iCodecs->InstallDriver(g_strTestFDName, &fShouldRunOnThisDevice, &fReboot))
                {
                    SLOG(eError, "Error: Failed CCodecPDO::InstallDriver!");
                    return false;
                }
                *pfNeedsReboot = fReboot ? true : *pfNeedsReboot;                
            }
        }        
    }
    else
    {
        // first need to install the Microsoft Bus Driver
        const CString strInf = MakeFullPath(g_strClassBDName);
        bool fShouldRunOnThisDevice = false;
        if (!InstallDriver(strInf, &fShouldRunOnThisDevice, &fReboot))
        {
            SLOG(eError, "Error: Failed InstallDriver!");
            return false;
        }
        *pfNeedsReboot = fReboot ? true : *pfNeedsReboot;
        
        if (fReboot)
        {
            SLOG(eError, " NYI: Hybrid-device requires reboot to install bus driver!");
            return false;

            // HEREHERE
            // BUGBUG - we don't know how to set CInstaller::m_strAlternateHWIDValue, since the bus driver is not currently loaded
            // could set it to UNKNOWN
            // then take the -h pnpid (or current one if no -h) and write in it's child's hwid
            // assume that there is only 1 audio codec per hybrid device
            /*
            if (!g_pUAATest->m_Installer.SetAlternateHWID("UNKNOWN"))
            {
                SLOG(eError, "Failed SetAlternateHWID!");
                return false;
            }
            
            return CInstaller::Reboot(true);
            */
        }
        else if (fShouldRunOnThisDevice)
        {
            if (!RequeryDriver())
            {
                SLOG(eError, "Error: Failed RequeryDriver!");
                return false;
            }            

            // we need to notify the environment that the audio device's PNPID will be different now (codec instead of controller)
            // do this before the InstallClassDrivers call since -h parameter may change
            CString szAlternateHWID = GetAlternateHWID();
            if (!g_pUAATest->m_Installer.SetAlternateHWID(szAlternateHWID))
            {
                SLOG(eError, "Failed SetAlternateHWID!");
                return false;
            }

            // call ourself again - this will install the test driver on the codecs
            if (!InstallCodecTestDrivers(pfNeedsReboot))
            {
                SLOG(eError, "Error: Failed InstallCodecTestDrivers!");
                return false;
            }            
        }
    }

    return true;
}

bool 
CInstaller::InstallCtrlTestDrivers()
{

    GUID guidInstallCtrlTestDrivers = {0x5ed3305f, 0xcfff, 0x420f, 0xa8, 0x21, 0xa1, 0xca, 0x42, 0x8f, 0x1e, 0xb3};

    CLogStartEndTest log(g_pUAATest->m_pShell, "Install controller test drivers", "", guidInstallCtrlTestDrivers);

    bool fReboot = false;
    vector<CControllerPDO>::iterator iControllers = m_vControllers.begin();
    for (;iControllers != m_vControllers.end(); iControllers++)
    {
        if (eTest == iControllers->m_eDriverType)
        {
            SLOG(eInfo1, "Skipping: Test driver already installed for this device.");
        }
        else
        {
            bool fShouldRunOnThisDevice = false;
            if (!iControllers->InstallDriver(g_strTestBDName, &fShouldRunOnThisDevice, &fReboot))
            {
                SLOG(eError, "Error: Failed CControllerPDO::InstallDriver!");
                return false;
            }
            g_pUAATest->m_fNeedsReboot = fReboot ? TRUE : g_pUAATest->m_fNeedsReboot;            
        }
    }    

    return true;
}

CString MakeFullPath(const CString& strInf)
{
    CString strInfFullPath;
    if (!GetWindowsDirectory(strInfFullPath.GetBuffer(MAX_PATH), MAX_PATH))
    {
        SLOG(eError, "Error: GetWindowsDirectory failed: GetLastError = %u", GetLastError());
        return strInf;
    }
    strInfFullPath.ReleaseBuffer();
    strInfFullPath += "\\inf\\" + strInf;

    return strInfFullPath;
}

bool 
CInstaller::PopulateShellDeviceList()
{
    // TODO - Should I populate device list with all enumerated devices, regardless of drivertype?
    // if so, I could just give them an appropriate DT_* type and match to test cases
    // right now, we only will populate eTest devices

    vector<CControllerPDO>::iterator iControllers = m_vControllers.begin();
    for (;iControllers != m_vControllers.end(); iControllers++)
    {
        if (!iControllers->PopulateShellDeviceList())
        {
            SLOG(eError, "Error: Failed CControllerPDO::PopulateShellDeviceList!");
            return false;
        }
    }

    return true;
}


//const CStringW CInstaller::s_wstrAlternateHWIDKey = L"AlternateHWID";
const CStringW CInstaller::s_wstrAlternateHWIDKey = L"WDKDeviceID";

bool
CInstaller::RemoveTestDriversFromSystem() {
    bool fResult = true;
    
    CComPtr<IBasicLog> pBasicLog;
    HRESULT hr = g_IShell->GetBasicLog(&pBasicLog);
    if (FAILED(hr))
    {
        SLOG(eError, "Error: ITestShell::GetBasicLog failed: hr = 0x%08x", hr);
        return false;
    }

    // troll %windir%\inf directory for oem*.inf files
    CString strInfDirectory;
    if (!GetWindowsDirectory(strInfDirectory.GetBuffer(MAX_PATH), MAX_PATH))
    {
        SLOG(eError, "Error: GetWindowsDirectory failed: GetLastError() = %u", GetLastError());
        return false;
    }
    strInfDirectory.ReleaseBuffer();
    strInfDirectory += "\\inf";

    WIN32_FIND_DATA wfdFound = {0};
    HANDLE hFind = FindFirstFile(strInfDirectory + "\\oem*.inf", &wfdFound);
    if (INVALID_HANDLE_VALUE == hFind)
    {
        DWORD dwErr = GetLastError();

        if (ERROR_FILE_NOT_FOUND == dwErr)
        {
            SLOG(eInfo1, "There are no third-party drivers installed on this system.");
            return true;
        }
        
        SLOG(eError, "Error: FindFirstFile %%windir%%\\oem*.inf failed unexpectedly: GetLastError = %d", dwErr);
        return false;
    }

    DoOnExit<HANDLE> closeFind(
        FindClose, Success_NonZero, hFind, pBasicLog, "closing FindFirstFile handle"
    );

    do
    {
        CString strOemInf = strInfDirectory + "\\" + wfdFound.cFileName;
        SLOG(eInfo1, "Examining %s", static_cast<LPCSTR>(strOemInf));

        // get the original .inf name

        // need to call SetupGetInformation twice
        // once with a NULL output buffer
        // to get the required buffer size
        // and again with an allocated buffer of that size
        DWORD cbInfInfo = 0;
        if (!SetupGetInfInformation(
            static_cast<LPCSTR>(strOemInf),
            INFINFO_INF_NAME_IS_ABSOLUTE,
            NULL, 0, &cbInfInfo
        ))
        {
            SLOG(eError,
                "Error: SetupGetInfInformation(%s, ... NULL, ...) failed: GetLastError = %u",
                static_cast<LPCSTR>(strOemInf),
                GetLastError()
            );
            fResult = false;
            continue;
        }

        CBuffer<BYTE> bufInfInfo(cbInfInfo);
        PSP_INF_INFORMATION pInfInfo = (PSP_INF_INFORMATION)(BYTE*)bufInfInfo;
        if (!SetupGetInfInformation(
            static_cast<LPCSTR>(strOemInf),
            INFINFO_INF_NAME_IS_ABSOLUTE,
            pInfInfo, cbInfInfo, NULL
        ))
        {
            SLOG(eError,
                "Error: SetupGetInfInformation(%s, ... buffer, ...) failed: GetLastError = %u",
                static_cast<LPCSTR>(strOemInf),
                GetLastError()
            );
            fResult = false;
            continue;
        }

        bool bIsTestDriver = false;
        for (UINT i = 0; i < pInfInfo->InfCount; i++)
        {
            SP_ORIGINAL_FILE_INFO infoOriginalFile = {0};
            infoOriginalFile.cbSize = sizeof(infoOriginalFile);

            if (!SetupQueryInfOriginalFileInformation(
                pInfInfo,
                i,
                NULL, // no alternate platform
                &infoOriginalFile
            ))
            {
                SLOG(eError,
                    "Error: SetupQueryInfOriginalFileInformation(%s, %u ...) failed: GetLastError = %u",
                    static_cast<LPCSTR>(strOemInf),
                    i,
                    GetLastError()
                );
                
                fResult = false;
                continue; // try the next inf index in this inf information
            }

            SLOG(eInfo1,
                "    OriginalInfName: \"%s\"; OriginalCatalogName: \"%s\"",
                infoOriginalFile.OriginalInfName,
                infoOriginalFile.OriginalCatalogName
            );

            if (
                0 == g_strTestBDName.CompareNoCase(infoOriginalFile.OriginalInfName) ||
                0 == g_strTestFDName.CompareNoCase(infoOriginalFile.OriginalInfName)
            )
            {
                bIsTestDriver = true;
                SLOG(eInfo1, "*** Removing OEM driver \"%s\" (%u) which is really test driver \"%s\" ***",
                    wfdFound.cFileName,
                    i,
                    infoOriginalFile.OriginalInfName
                );
                break; // don't need to check the rest of the inf indexes in this inf information
            }
        } // inf index in this inf information

        // if the original .inf name is t_hdaud.inf or hdaudbustest.inf
        // then force-uninstall it

        if (bIsTestDriver)
        {
            if (!SetupUninstallOEMInf(
                wfdFound.cFileName, // need name w/o path
                SUOI_FORCEDELETE, // Deletes .inf even if the driver's loaded (shouldn't be)
                NULL // reserved
            ))
            {
                SLOG(eError,
                    "Error: SetupUninstallOEMInf(%s) failed: GetLastError = %u",
                    wfdFound.cFileName,
                    GetLastError()
                );
                
                fResult = false;
                continue; // try next .inf
            }

            SLOG(eInfo1, "*** Removal of %s was successful. ***", wfdFound.cFileName);
        }

    } while (FindNextFile(hFind, &wfdFound));

    DWORD dwErr = GetLastError();
    if (ERROR_NO_MORE_FILES != dwErr)
    {
        SLOG(eError, "Error: FindNextFile %%windir%%\\oem*.inf failed unexpectedly: GetLastError = %d", dwErr);
        return false;
    }

    return fResult;
}

bool 
CInstaller::Reboot(const bool fRestartTaskAfterReboot)
{    
    if (m_fRebooting)
    {
        // reboot already scheduled - do nothing
        return true;
    }
    else
    {
        m_fRebooting = true;
    }

    XLOG(XMSG, eWarn1, "Warning: Machine will be rebooted.");
    // flush all logs
    g_pShell->CommitLog();

    if (g_pUAATest->m_fWTT)
    {        
        return m_pWTT->Reboot(fRestartTaskAfterReboot);
    }
    else
    {
        if (fRestartTaskAfterReboot)
        {
            // BUGBUG - Not Yet Implemented            
            // use runonce to rerun the same cmdline
            XLOG(XWARN, eWarn1, "NYI: Auto-reboot/resume outside of WTT environment!");
            XLOG(XWARN, eWarn1, "User will need to manually reboot and restart the test.");
            return true;            
        }

        //
        // enable shutdown privilege for this process
        //
        HANDLE hToken = NULL;
        TOKEN_PRIVILEGES tkp;             
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        {
            SLOG(eError, "Error: Failed OpenProcessToken!");
            return false;
        }
        LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
        tkp.PrivilegeCount = 1;  // one privilege to set    
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0))
        {
            SLOG(eError, "Error: Failed AdjustTokenPrivileges!");
            return false;
        }
        // and reboot!
#pragma prefast(suppress: __WARNING_USE_OTHER_FUNCTION, "OACR suggests finding a design alternative to rebooting")
        if (!InitiateSystemShutdownEx(NULL, 
                                      "UAATest application is rebooting the system", 
                                      CInstaller::eShutDownTimer, 
                                      TRUE, // force apps to close, so we don't get the End Program dialog
                                      TRUE, 
                                      SHTDN_REASON_MAJOR_APPLICATION | SHTDN_REASON_MINOR_HARDWARE_DRIVER))
        {
            SLOG(eError, "Error: Failed InitiateSystemShutdownEx!");
            return false;
        }
    }    

    return true;
}

bool 
CControllerPDO::PopulateShellDeviceList()
{
    if (eTest == m_eDriverType)
    {
        // leverage existing populate code
        if (!g_pUAATest->CreateControllerDevice(m_strDevicePath.GetBuffer(), m_strInstanceID.GetBuffer()))
        {
            SLOG(eError, "Error: Failed CUAATestModule::CreateDevice!");
            return false;
        }
    }
    else if (eMicrosoft == m_eDriverType)
    {
        // do the codecs
        vector<CCodecPDO>::iterator iCodecs = m_vCodecs.begin();
        for (;iCodecs != m_vCodecs.end(); iCodecs++)
        {
            if (!iCodecs->PopulateShellDeviceList())
            {
                SLOG(eError, "Error: Failed CCodecPDO::PopulateShellDeviceList!");
                return false;
            }
        }
    }
    else
    {
        // can't populate shell device list
    }

    return true;
}

bool 
CCodecPDO::PopulateShellDeviceList()
{
    if (eTest == m_eDriverType)
    {
        // leverage existing populate code
        if (
            !g_pUAATest->CreateCodecDevice(
                m_dwControllerDevInst,
                m_strDevicePath.GetBuffer(),
                m_strInstanceID.GetBuffer()
            )
        )
        {
            SLOG(eError, "Error: Failed CUAATestModule::CreateDevice!");
            return false;
        }
    }

    return true;
}

CWTT::CWTT()
{
}

CWTT::~CWTT()
{
}

bool 
CWTT::Reboot(const bool /*fRestartTaskAfterReboot*/)
{
    return CallWttCmd("/EAReboot");
}

bool 
CWTT::SetAlternateHWID(CString& strAlternateHWIDValue)
{

    CString szParameter;
    szParameter.Format(
        _T("/SysInitKey /Key:%ls /Value:%s"),
        static_cast<LPCWSTR>(CInstaller::s_wstrAlternateHWIDKey),
        static_cast<LPCTSTR>(strAlternateHWIDValue)
    );

    return CallWttCmd(szParameter);
}

bool
CWTT::CallWttCmd(LPCSTR szParameter)
{
    if (NULL == szParameter) 
    { 
        XLOG(XFAIL, eError, "Error: CallWttCmd input param is NULL");
        return false; 
    } 
    
    char szWttCmdPath[MAX_PATH] = {0};
#ifdef EXTERNAL
    if (0 == GetEnvironmentVariableA("DTMBIN", szWttCmdPath, MAX_PATH))
    {
        DWORD dwErr = GetLastError();
        XLOG(XFAIL, eError, "Error: GetEnvironmentVariable(DTMBIN, ...) failed: GetLastError = %u", dwErr);
        return false;
    }
#else
    if (0 == GetEnvironmentVariableA("WTTBIN", szWttCmdPath, MAX_PATH))
    {
        DWORD dwErr = GetLastError();
        XLOG(XFAIL, eError, "Error: GetEnvironmentVariable(WTTBIN, ...) failed: GetLastError = %u", dwErr);
        return false;
    }
#endif

    CStringA szCommand;
    szCommand.Format("\"%swttcmd\" %s", szWttCmdPath, szParameter); 

    STARTUPINFO si = { 0 }; si.cb = sizeof(si);
    PROCESS_INFORMATION pi = { 0 };

    BOOL bOK = CreateProcess(
        NULL,
        szCommand.GetBuffer(), // needs a writable string
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi
    );

    if (!bOK)
    {
        DWORD dwErr = GetLastError();
        SLOG(eError, "Error: CreateProcess(\"%s\") failed: GetLastError = %u", (LPCSTR)szCommand, dwErr);
        return false;
    }

    bool bRet = true;
    DWORD dwWaitResult = WaitForSingleObject(pi.hProcess, INFINITE);

    if (WAIT_OBJECT_0 != dwWaitResult)
    {
        DWORD dwErr = GetLastError();
        SLOG(eError,
            "Error: WaitForSingleObject on command \"%s\" returned %u: GetLastError = %u",
            (LPCSTR)szCommand,
            dwWaitResult,
            dwErr
        );
        bRet = false;
    }

    DWORD dwExitCode = 0;
    bOK = GetExitCodeProcess(pi.hProcess, &dwExitCode);

    if (!bOK)
    {
        DWORD dwErr = GetLastError();
        SLOG(eError,
            "Error: Could not get exit code for process \"%s\": GetLastError = %u",
            (LPCSTR)szCommand,
            dwErr
        );
        bRet = false;
    }

    if (STILL_ACTIVE == dwExitCode)
    {
        SLOG(eError, "Error: Process \"%s\" exit code is STILL_ACTIVE (%u)", (LPCSTR)szCommand, STILL_ACTIVE);
        bRet = false;
    }
    else if (0 != dwExitCode)
    {
        SLOG(eError, "Error: Process \"%s\" returned failing exit code %u", (LPCSTR)szCommand, dwExitCode);
        bRet = false;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return bRet;
}
