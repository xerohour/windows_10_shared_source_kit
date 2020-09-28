/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/

#ifdef TIME_BOMB
#define DAYS_UNTIL_EXPIRATION   120
#endif

//#define NTSTRSAFE_NO_DEPRECATE
#define STRSAFE_NO_DEPRECATE

#define INITGUID

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <tchar.h>
#include <mmsystem.h>
DEFINE_GUID(GUID_NULL,
    0x00000000L, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 );

#include <strsafe.h>

#include <shell98.h>
#include <module98.h>
#include <dvmodule.h>

#ifdef TIME_BOMB
#include <timebomb.h>
#endif

#include <stdio.h>
#include <setupapi.h>
#include <cfgmgr32.h>
#include <ks.h>
#include <ksmedia.h>
#include <process.h>
#include <atlstr.h>
#include <hitframework.h>

#define USERMODE // or azcodec.h includes wdm.h (kernel mode version of windows.h)
#include <azcodec.h>
#include <privioct.h>
#include <controller-interface.h>

#include "aznode.h"
#include "azverbutils.h"
#include "install.h"
#include "uaatest.h"
#include "power.h"
#include "codec.h"
#include "HdRegTest.h"
#include "HdHitLog.h"
#include "HdHitPlugin.h"
#include "hdaudiomemoryspace.h"
#include "controller.h"
#include "util.h"
#include "resource.h"
#include "testcase.h"
#include "buffer.h"
#include "SetupDi.h"
#include "inimgr.h"
#include "log.h"

BOOL fResetTest = FALSE;

VOID TestAPI (void);

#define NUMPOSSIBLECODECS 16
#define SHUTDOWN_TIMER 30

//
// globals
//
ITestShell* g_IShell = NULL;
CUAATestModule* g_pUAATest = NULL;
Hit::ILog * Log = NULL;                                                     // Must initialize before using!!
//
// Shell98 / ModMgr98 stuff
//
static LPTSTR  s_szModuleName = "UAA Test";
static DWORD   s_dwAppID       = 37;

//
// Prototypes / forward decls
//
//AzRirbEntry SendVerb(HANDLE hDriver, ULONG verb);

//==============================================================================
// NewTestModule
//
// Description:
//  Entry point.  Called as the shell loads to setup the derived test module 
//  class inside of the shell.
//
// Arguments:
//  IN  CTestShell  *pShell     - Pointer to shell for tests' access
//  IN  HINSTANCE   hinstDLL    - 
//
// Returns:
//  CTestModule*    - pointer to newly created test module object
//                  - NULL if out of memory
//
// History:
//  10/21/03 - jwexler - created
//==============================================================================
CTestModule* WINAPI 
NewTestModule
(
    IN CTestShell *pShell, 
    IN HINSTANCE hinstDLL
)
{
    g_IShell = (ITestShell*)pShell;
    g_pUAATest = new CUAATestModule(pShell, hinstDLL);
    return (CTestModule*)g_pUAATest;
} // NewTestModule

VOID InitializeHdTable(Hit::CTable & GenTable )
{
    GenTable.Define(GeneralInformation);   
    GenTable["Global Capabilities"].Define (Global_Capabilities);
    GenTable["Global Control"].Define (Global_Control);
    GenTable["Wake Enable"].Define (Wake_Enable);
    GenTable["Wake Status"].Define (Wake_Status);
    GenTable["Global Status"].Define (Global_Status);
    
    GenTable["Interrupt Control"].Define (Interrupt_Control);
    GenTable["Interrupt Status"].Define (Interrupt_Status);
    GenTable["Stream Synchronization"].Define (Stream_Synchronization);
    GenTable["CORB Lower Base Address"].Define (CORBLBASE);
    GenTable["CORB Write Pointer"].Define (CORBWP);
    GenTable["CORB Read Pointer"].Define (CORBRP);
    GenTable["CORB Control"].Define (CORBCTL);
    GenTable["CORB Status"].Define (CORBSTS);
    GenTable["CORB Size"].Define (CORBSIZE);
    GenTable["RIRB Lower Base Address"].Define (RIRBLBASE);
    GenTable["RIRB Write Pointer"].Define (RIRBWP);
    GenTable["Response Interrupt Count"].Define (RINTCNT);
    GenTable["RIRB Control"].Define (RIRBCTL);
    GenTable["RIRB Status"].Define (RIRBSTS);
    GenTable["RIRB Size"].Define (RIRBSIZE);
    GenTable["DMA Position Buffer Lower Base"].Define (DPLBASE);
    GenTable["Immediate Command Status"].Define (Immediate_Command_Status);    

}
//
// Classes
//
//--------------------------------------------------------------------------
//  CGauditDevice constructor
CHdController::CHdController(HANDLE hControllerInterface)
{
    hController = hControllerInterface;	
    m_lpType = 0;
    m_fSelected = 1;
    Asrt.Initialize();
    pHdAudioCntrollerNameSpace = new Hit::CNamespace ("HdAudioContoller", 
                                                      Hit::HdCntRead, 
                                                      Hit::HdCntWrite, 
                                                      (PVOID)&hController, 
                                                      sizeof(HANDLE));
    pHdAudioCntrollerNameSpace->AddTable("General_Info");
    Hit::CTable & GenTable = pHdAudioCntrollerNameSpace->Tbl("General_Info");

    InitializeHdTable(GenTable);
    Hit::CRegister &RegGc = GenTable["Global Capabilities"];
    DWORD dGC = RegGc.Read() ; 
    if (dGC ==0)
        SLOG (eError, "Could not get Global Capabilities of the controller");
    // TODO:  here we want to exit test if dGC ==0

    // check for value of OSS, ISS, BSS and NSDO 
    Hit::CBitRange &OSS = RegGc["Number of Output Streams Supported -OSS"];    
    uOSS = (ULONG) OSS.Read();
    
    Hit::CBitRange &ISS = RegGc["Number of Input Streams Supported -ISS"];    
    uISS = (ULONG) ISS.Read();

    Hit::CBitRange &BSS = RegGc["Number of Bidirectional Streams Supported -BSS"];    
    uBSS = (ULONG) BSS.Read();

    Hit::CBitRange &NSDO = RegGc["Number of Serial Data Out Signals -NSDO"];    
    uNSDO = (ULONG) NSDO.Read();

    Hit::CBitRange &b640K = RegGc["64 Bit Address Supported -640K"];    
    if ((BYTE) b640K.Read())
        f64bit =  TRUE; // it supports 64 bit addressing
    else
        f64bit =  FALSE;    // it does not support 64 bit addressing


    for (ULONG i = 0; i<uISS; i++)  {
        pHdAudioCntrollerNameSpace->AddIndexedTable("Input_Stream_Descriptor", i, 0x80 +(i*0x20));
        Hit::CTable & InStreamTable = pHdAudioCntrollerNameSpace->Tbl("Input_Stream_Descriptor", i);    
        InStreamTable.Define(InputStreamDescriptor);
        InStreamTable["Input Stream Descriptor Control_1"].Define(SDControl_1);
        InStreamTable["Input Stream Descriptor Control_2"].Define(SDControl_2);
        InStreamTable["Input Stream Descriptor Control_3"].Define(SDControl_3);
        InStreamTable["Input Stream Descriptor Status"].Define(SDStatus);
        InStreamTable["Input Stream Descriptor Last Valid Index"].Define(SDLastValidIndex);
        InStreamTable["Input Stream Descriptor Format"].Define(SDFormat);
        InStreamTable["Input Stream Descriptor Buffer Descriptor List Pointer - Lower"].Define(SDLPBA);
    }
    for (ULONG i = 0; i<uOSS; i++)  {
        pHdAudioCntrollerNameSpace->AddIndexedTable("Output_Stream_Descriptor", i, 0x80 +(i*0x20)+(uISS* 0x20));
        Hit::CTable & InStreamTable = pHdAudioCntrollerNameSpace->Tbl("Output_Stream_Descriptor", i);    
        InStreamTable.Define(OutputStreamDescriptor);
        InStreamTable["Output Stream Descriptor Control_1"].Define(SDControl_1);
        InStreamTable["Output Stream Descriptor Control_2"].Define(SDControl_2);
        InStreamTable["Output Stream Descriptor Control_3"].Define(SDControl_3);
        InStreamTable["Output Stream Descriptor Status"].Define(SDStatus);
        InStreamTable["Output Stream Descriptor Last Valid Index"].Define(SDLastValidIndex);
        InStreamTable["Output Stream Descriptor Format"].Define(SDFormat);
        InStreamTable["Output Stream Descriptor Buffer Descriptor List Pointer - Lower"].Define(SDLPBA);
    }
    for (ULONG i = 0; i<uBSS; i++)  {
        pHdAudioCntrollerNameSpace->AddIndexedTable("BiDirectional_Stream_Descriptor", i, 0x80 +(i*0x20) + (uISS* 0x20) + (uOSS* 0x20));
        Hit::CTable & InStreamTable = pHdAudioCntrollerNameSpace->Tbl("BiDirectional_Stream_Descriptor", i);    
        InStreamTable.Define(BiDirectionalStreamDescriptor);
        InStreamTable["BiDirectional Stream Descriptor Control_1"].Define(SDControl_1);
        InStreamTable["BiDirectional Stream Descriptor Control_2"].Define(SDControl_2);
        InStreamTable["BiDirectional Stream Descriptor Control_3"].Define(SDControl_3);
        InStreamTable["BiDirectional Stream Descriptor Status"].Define(SDStatus);
        InStreamTable["BiDirectional Stream Descriptor Last Valid Index"].Define(SDLastValidIndex);
        InStreamTable["BiDirectional Stream Descriptor Format"].Define(SDFormat);
        InStreamTable["BiDirectional Stream Descriptor Buffer Descriptor List Pointer - Lower"].Define(SDLPBA);
    }


}

//--------------------------------------------------------------------------
//  CGauditDevice desstructor
CHdController::~CHdController()
{
       delete pHdAudioCntrollerNameSpace;
	if (hController)
        CloseHandle(hController);
}


BOOL InitializeHdHit ()  
{
        //initialize Log object used by HIT frame work
    Log = new HdHitLog ();
    if (Log == NULL || !Log->Initialize()) {		
        SLOG(eError, "Error: Log creation/initialization failed\n");
        Log = NULL;
        return FALSE;
    }	
    return TRUE;
    // Must initialize assertion objects
//    Asrt.Initialize();

}

//==============================================================================
// CUAATestModule::CUAATestModule
//
// Description:
//  Constructor
//  ***Additional initialization for this module should _NOT_ be put here.
//
// Arguments:
//  IN  CTestShell  *pShell     - Pointer to shell for tests' access
//  IN  HINSTANCE   hinstDLL    - 
//
// Returns:
//  CUAATestModule* (implicit) - pointer to new CSampleModule object
//                              - NULL if out of memory
//
// History:
//  10/21/03 - jwexler - created
//==============================================================================
CUAATestModule::CUAATestModule
(
    IN CTestShell *pShell, 
    IN HINSTANCE hinstDLL
) : CDeviceTestModule(pShell, hinstDLL), 
    m_pCurDevice(NULL)
{
    m_dwAppID           = s_dwAppID;  
    m_pstrModuleName    = s_szModuleName;
    m_rgTestCaseInfo    = rgTestCaseInfo;
    m_cTestCases        = ARRAY_ELEMENTS(rgTestCaseInfo);
    m_nIconID           = IDI_UAATEST;

    m_fDeviceIDIsAnInstanceID = FALSE;
    m_fUseClassDriver = FALSE;
	m_fWTT = FALSE;
    m_fReplace = FALSE;
    m_fRestart = FALSE;
    m_fRestore = FALSE;
    m_fNeedsReboot = FALSE;
    m_fCodecTesting = FALSE;	    
    m_fCodecTestOnly = FALSE;
    m_fCntTestOnly = FALSE;    
    m_liQpcPerSecond.QuadPart = 0;

    timeBeginPeriod(1);
} // CUAATestModule::CUAATestModule


//==============================================================================
// CUAATestModule::~CUAATestModule
//
// Description:
//  Destructor
//  ***Any cleanup from Initialize should be put here.
//
// Arguments:
//  None
//
// Returns:
//  Nothing
//
// History:
//  10/21/03 - jwexler - created
//==============================================================================
CUAATestModule::~CUAATestModule()
{
    timeEndPeriod(1);
} // CUAATestModule::~CUAATestModule

//==============================================================================
// CUAATestModule::OnInitialUpdate
//
// Description:
//  This is called by the shell the first time we paint the app window.
//
// Arguments:
//  None
//
// Returns:
//  DWORD   -   FNS_PASS indicates success
//              Anything else indicates failure
//
// History:
//  10/21/03 - jwexler - created
//==============================================================================
DWORD 
CUAATestModule::OnInitialUpdate()
{
    DWORD   dwRes = FNS_PASS;

    // Log some stuff
    XLOG(XMSG, eError, "%s", m_pstrModuleName);
    XLOG(XMSG, eError, "AppID = %d", m_dwAppID);
    XLOG(XMSG, eError, "%s", "");
    if(!InitIniFile(NULL, "uaatest.ini"))
    {
        XLOG(XWARN, eWarn1, "Warning: Initializing IniFile failed.");
    }
    m_fCodecTesting = TRUE; // UAATEst will norally test Codecs only.
    if (! InitializeHdHit ())
        return FNS_FAIL;

    // Parse UAATest specific cmdline
    if (g_pShell->m_pszAppCmdLine)
    {
        _strlwr(g_pShell->m_pszAppCmdLine);
        SLOG(eInfo1, "UAATest CmdLine == %s", g_pShell->m_pszAppCmdLine);
		m_fUseClassDriver = strstr(g_pShell->m_pszAppCmdLine, "class") ? TRUE : FALSE; // install the class driver instead of the test driver
        m_fWTT = strstr(g_pShell->m_pszAppCmdLine, "wtt") ? TRUE : FALSE; // is the app being invoked under wtt?
        m_fReplace = strstr(g_pShell->m_pszAppCmdLine, "replace") ? TRUE : FALSE; // replace the existing driver
        m_fRestart = strstr(g_pShell->m_pszAppCmdLine, "restart") ? TRUE : FALSE;   // restart machine if required
        m_fRestore = strstr(g_pShell->m_pszAppCmdLine, "restore") ? TRUE : FALSE; // restore original driver
        m_fCodecTestOnly= strstr(g_pShell->m_pszAppCmdLine, "codec") ? TRUE : FALSE;    //specify  codec in the command line to test codecs
        m_fCntTestOnly = strstr(g_pShell->m_pszAppCmdLine, "cnt") ? TRUE : FALSE; // Sepcify cnt for the  controller testing
        m_fCodecTesting = strstr(g_pShell->m_pszAppCmdLine, "controller") ? FALSE: TRUE; // this is used internelly by  uaa test to have controller testing after the codec testing is done
    }

    if (m_fCodecTestOnly && m_fCntTestOnly)  // if both cnt and codecs are specified into command line then we will test first codecs and then controller.
    {
        m_fCodecTestOnly = FALSE;
        m_fCntTestOnly = FALSE;
    }

    if (m_fCodecTestOnly) // that means we will test codecs only. If Codec is specified into command line argument then it will overied all other aruuments
    {
        m_fCodecTesting = TRUE;
        SLOG(eInfo1, "Testing only codecs.");
    }
    else if (m_fCntTestOnly) // that means we will test codecs only. If Codec is specified into command line argument then it will overied all other aruuments
    {
        m_fCodecTestOnly = FALSE;
        m_fCodecTesting = FALSE;
        SLOG(eInfo1, "Testing only Controller.");
    }
    
    if (!QueryPerformanceFrequency(&m_liQpcPerSecond))
    {
        SLOG(eError, "Error: QueryPerformanceFrequency failed: GetLastError() = %u", GetLastError());
        dwRes = FNS_FAIL;
        goto DONE;
    }

    // Gather information about environment, devices, and drivers
    if (!m_Installer.DiscoverStartState())
    {
        SLOG(eError, "Error: Failed DiscoverStartState!");
        dwRes = FNS_FAIL;
        goto DONE;
    }
    
    bool fExiting = false;
    if (!m_Installer.ConfigureDevices(&fExiting))
    {
        SLOG(eError, "Error: Failed ConfigureDevices!");
        dwRes = FNS_FAIL;
        goto DONE;
    }

    // only populate when ConfigureDevices does not require an app/system restart
    if (!fExiting)
    {
        if (!m_Installer.PopulateShellDeviceList())
        {
            SLOG(eError, "Error: Failed PopulateShellDeviceList!");
            dwRes = FNS_FAIL;
            goto DONE;
        }
    }

DONE:
    return dwRes;

} // CUAATestModule::OnInitialUpdate

BOOL 
CUAATestModule::CreateCodecDevice(DWORD dwControllerDevInst, LPCSTR szDevicePath, LPCSTR szInstanceID)
{
    HRESULT hr = S_OK;
    BOOL fRes = FALSE;    
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;

    // get a handle to the driver                    
    HANDLE hFilter = CreateFile(szDevicePath, 
                                GENERIC_READ | GENERIC_WRITE, 
                                0, 
                                NULL, 
                                OPEN_EXISTING, 
                                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 
                                NULL);
    if (!IsValidHandle(hFilter))
    {
        SLOG(eError, "Error: Could not open filter");
        SLOG(eError, " GetLastError == %u", GetLastError());
        fRes = FALSE;
        goto DONE;
    }

    // get SDI and starting node info
    // for this function group
    ResourceInfoPacket functionGroup;
    DWORD nBytesReturned;
    fRes = DeviceIoControl(
        hFilter,
        IOCTL_AZALIABUS_GETRESOURCEINFORMATION,
        NULL, 0, // no input buffer
        &functionGroup, sizeof(functionGroup),
        &nBytesReturned,
        NULL // no OVERLAPPED
    );

    if (fRes)
    {
        SLOG(eInfo1,
            "Function group is SDI line %u, starting node is %u",
            functionGroup.SdiCodecAddress,
            functionGroup.NodeFunctionGroupStart
        );
    }
    else
    {
        SLOG(eError, "Error: GetResourceInformation failed. GetLastError == %u", GetLastError());
        goto DONE;
    }
    
    // only add unique codecs
    // we'll re-parse all the function groups in CCodec::Parse
    if (!IsKnownCodec(
        dwControllerDevInst,
        functionGroup.SdiCodecAddress))
    {
        CCodec* pCodec = new CCodec(
            szDevicePath,
            dwControllerDevInst,
            functionGroup.SdiCodecAddress,
            hFilter);
        hr = StringCbPrintf(
            pCodec->m_szRep,
            sizeof(pCodec->m_szRep), // defined in multimediatest/published/cshell.w as 256
            "SDI line %u on controller device instance 0x%x",
                functionGroup.SdiCodecAddress, dwControllerDevInst
        );

        if (FAILED(hr))
        {
            SLOG(eError,
                "Error: StringCbPrintf failed to build friendly name of codec: hr = 0x%08x",
                hr
            );
            fRes = FALSE;
            delete pCodec;
            goto DONE;
        }
        
        _tcscpy(pCodec->m_szPnPId, szInstanceID);
        pCodec->m_fSelected = TRUE;
        pCodec->m_lpType = DT_CODEC;

        fRes = pCodec->Parse();
        if (!fRes)
        {
            SLOG(eError,
                "Error: Failure parsing codec attached to SDI# %u",
                functionGroup.SdiCodecAddress
            );
            delete pCodec;
            goto DONE;
        }
        if (!g_IShell->AddDevice(pCodec))
        {
            //delete pCodec;
        }
    }
    else
    {
        SLOG(eInfo1, "Codec has already been added to device list");
    }
    
    /*
    // always add as a function driver
    CAFD* pAFD = new CAFD(szDevicePath, pcDevID, hFilter);
    // BUGBUG - need to get the friendly name somehow
    _tcscpy(pAFD->m_szRep, "HDAudio Function Driver");
    _tcscpy(pAFD->m_szPnPId, szInstanceID);
    pAFD->m_fSelected = TRUE;
    pAFD->m_lpType = DT_AFD;
    if (!g_IShell->AddDevice(pAFD))
    {
        delete pAFD;
    }
    */

    fRes = TRUE;

DONE:
    //CloseHandle(hFilter);
    return fRes;
}

BOOL 
CUAATestModule::CreateControllerDevice(LPCSTR szDevicePath, LPCSTR szInstanceID)
{
    BOOL fRes = FALSE;
    HANDLE hInterface = NULL;

    TTNode<CDeviceDescriptor>* pttnodeDev = g_pShell->m_listDevices.GetHead();
    if (!pttnodeDev)
    {
        hInterface = CreateFile(szDevicePath, 
                                GENERIC_READ | GENERIC_WRITE, 
                                0, 
                                NULL, 
                                OPEN_EXISTING, 
                                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 
                                NULL);

        if (!IsValidHandle(hInterface))
        {
            SLOG(eError, "Error: Could not open filter");
            SLOG(eError, " GetLastError == %u", GetLastError());
            fRes = FALSE;
            return fRes;
        }    

        CHdController * pController = new CHdController (hInterface);
        if (!pController)
        {
            SLOG(eError, "Error: Could not Create Test Controller Object");
            fRes = FALSE;
            return fRes;
        }    

        pController->m_lpType = DT_CTRL;
        pController->m_fSelected = TRUE;
        _tcscpy(pController->m_szRep, "HDAudio Controller");
        _tcscpy(pController->m_szPnPId, szInstanceID);
        g_IShell->AddDevice(pController);
    }
    else
    {
        while (pttnodeDev)
        {
            CDeviceDescriptor* pDevice = pttnodeDev->pData;
            if (strcmp (pDevice->m_szPnPId, szInstanceID))
            {
                hInterface = CreateFile(szDevicePath, 
                                        GENERIC_READ | GENERIC_WRITE, 
                                        0, 
                                        NULL, 
                                        OPEN_EXISTING, 
                                        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 
                                        NULL);

                if (!IsValidHandle(hInterface)) {
                    SLOG(eError, "Error: Could not open filter");
                    SLOG(eError, " GetLastError == %u", GetLastError());
                    fRes = FALSE;
                    return fRes;
                }    

                CHdController * pController = new CHdController (hInterface);
                if (!pController)   {
                    SLOG(eError, "Error: Could not Create Test Controller Object");
                    fRes = FALSE;
                    return fRes;
                }    

                pController->m_lpType = DT_CTRL;
                pController->m_fSelected = TRUE;
                _tcscpy(pController->m_szRep, "HDAudio Controller");
                _tcscpy(pController->m_szPnPId, szInstanceID);
                g_IShell->AddDevice(pController);

            }
            pttnodeDev = g_pShell->m_listDevices.GetNext(pttnodeDev);
        }
    }


    fRes = TRUE;

    return fRes;
}    

void    
CUAATestModule::SetTestDevice
(
    CDeviceDescriptor* pDevice
)
{
    m_pCurDevice = pDevice;
}

BOOL 
CUAATestModule::IsKnownCodec(DWORD dwControllerDevInst, USHORT usSDI)
{
    BOOL fKnown = FALSE;

    for (
        TTNode<CDeviceDescriptor>* pttnodeDev = g_pShell->m_listDevices.GetHead();
        pttnodeDev && !fKnown;
        pttnodeDev = g_pShell->m_listDevices.GetNext(pttnodeDev)
    )
    {
        CDeviceDescriptor* pDevice = pttnodeDev->pData;
        if (DT_CODEC == pDevice->m_lpType)
        {
            CCodec *pCodec = (CCodec*)pDevice;
            if (
                pCodec->m_dwControllerDevInst == dwControllerDevInst && 
                pCodec->m_usSDI == usSDI
            )
            {
                fKnown = TRUE;
            }
        }
    }
    
    return fKnown;
}

AzRirbEntry SendVerb(HANDLE hDriver, ULONG verb)
{
  AzRirbEntry azrResponse;
  UserModeCodecCommandPacket CPacket;
  UserModeCodecResponsePacket RPacket;
  DWORD dwBytesReturned = 0;

  ZeroMemory(&CPacket, sizeof(CPacket));
  ZeroMemory(&RPacket, sizeof(RPacket));
  CPacket.NumCommands = 1;
  CPacket.Command[0] = verb;
  BOOL fRes = DeviceIoControl(hDriver, IOCTL_AZALIABUS_SENDVERBS, &CPacket, sizeof(CPacket), &RPacket, sizeof(RPacket), &dwBytesReturned, 0);
  //SLOG(eInfo1, "    fRes == %u", fRes);
  if (!fRes) {
      // not necessarily a failure, let the caller decide that
      XLOG(XMSG, eError, "    [DeviceIoControl:SendVerb] GetLastError() == %u", GetLastError());
  }
  //SLOG(eInfo1, "    CPacket.NumCommands == %u", CPacket.NumCommands);
  //SLOG(eInfo1, "    CPacket.Command[0] == 0x%08x", (ULONG)CPacket.Command[0]);
  //SLOG(eInfo1, "    RPacket.NumResponses == %u", RPacket.NumResponses);
  //SLOG(eInfo1, "    RPacket.Response[0] == 0x%I64x", (ULONGLONG)(RPacket.Response[0]));
  //SLOG(eInfo1, "    RPacket.Response[0].Valid == %u", RPacket.Response[0].Valid);
  //SLOG(eInfo1, "    RPacket.Response[0].Sdi == %u", RPacket.Response[0].Sdi);
  //SLOG(eInfo1, "    RPacket.Response[0].Unsolicited == %u", RPacket.Response[0].Unsolicited);
  //SLOG(eInfo1, "    RPacket.Response[0].Reserved0 == %u", RPacket.Response[0].Reserved0);
  azrResponse = RPacket.Response[0];
/*
  if (!fRes)
  {
      ulRes = 0;
  }
  else
  {
      
  }
  */
  
  return azrResponse;
}

BOOL
CUAATestModule::SleepAtLeast(DWORD dwMilliseconds)
{
    LARGE_INTEGER liStart = {0};
    LARGE_INTEGER liNow = {0};
    BOOL fRes = TRUE;
    DWORD dwSlept = 0;

    SLOG(eBlab1, "Attempting to sleep at least %d ms", dwMilliseconds);
    
    if (!QueryPerformanceCounter(&liStart))
    {
        SLOG(eError, "Error: QueryPerformanceCounter failed: GetLastError = %u", GetLastError());
        fRes = FALSE;
        goto DONE;
    }

    do
    {
        SLOG(eBlab2, "Sleeping %u ms...", dwMilliseconds - dwSlept);
        Sleep(dwMilliseconds - dwSlept);
        
        if (!QueryPerformanceCounter(&liNow))
        {
            SLOG(eError, "Error: QueryPerformanceCounter failed: GetLastError = %u", GetLastError());
            fRes = FALSE;
            goto DONE;
        }

        dwSlept = (DWORD)(1000.0 * (liNow.QuadPart - liStart.QuadPart) / m_liQpcPerSecond.QuadPart);

        if (dwSlept < dwMilliseconds)
        {
            SLOG(eBlab2, "Slept %u ms so far...", dwSlept);
        }
        else
        {
            SLOG(eBlab2, "Ended up sleeping a total of %u ms.", dwSlept);
        }
    } while (dwSlept < dwMilliseconds);

DONE:
    return fRes;
}
