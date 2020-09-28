//
// Class:  CCommon
//         Contains commonly-used methods, especially for DXGI Mode Enumeration.
//

#pragma once

#include <d3d9.h>


// #defines converted to constants for easy debugging
static const UINT  c_nFILEIDENTITYINFOSTRINGLEN = 256;
static const UINT  c_nMSG_LEN = 1024;
static const UINT  c_nDEV_NAME_LEN = 32;

#ifndef MONITORS_MAX
#define MONITORS_MAX 16
#endif

typedef struct tagFILEIDENTITYINFO
{
	// Add any additional file info you want to retrieve here
	bool	bDebug;
	DWORD	dwFileVersionMS;
	DWORD	dwFileVersionLS;
    WORD    wVersion[4];
    TCHAR   tszFileIdentityInfo[c_nFILEIDENTITYINFOSTRINGLEN];     // string version, date/time and size converted to a string
} FILEIDENTITYINFO, *PFILEIDENTITYINFO;


//////////////////////////////////////////////////////////////////////////////
//
// Global Function Declarations
//
//////////////////////////////////////////////////////////////////////////////

int  __cdecl  Compare_DXGI_MODE_DESC( const void* pModeDesc1, const void* pModeDesc2 );
BOOL  GetFileInfo( const CHAR *pcsFilename, FILEIDENTITYINFO *pFileInfo );


//////////////////////////////////////////////////////////////////////////////
//
// Class Declarations
//
//////////////////////////////////////////////////////////////////////////////

class CCommon
{
public:
    CCommon()  {}
    ~CCommon()  {}

    static void  AttachScreensAndSetRotationsToZero();

    static void  GetFocusForFullscreen( __in HWND hwnd );

    static BOOL  GetGDIDevMode( DWORD dwDevNum, DISPLAY_DEVICE& currentDispDev, DEVMODE& currentDevMode );

    static BOOL
    GetGDIDevMode(
        __in_ecount(nDevNameLen)  __nullterminated const char*  pszDevName,
        __in    UINT            nDevNameLen,
        __inout DEVMODE&        currentDevMode  // by ref
       );

    static int  SetGDIDevMode( DISPLAY_DEVICE& newDispDev, DEVMODE& newDevMode );

    static BOOL
    IsFullscreen(
        __in    HWND            hwndDeviceWindow,
        __in    D3DDISPLAYMODEEX&  dispModeEx    // by ref
      );

    static void  LogTestAppInfo();

    static void
    PumpWindowMsgs(
        __in    HWND            hwnd,
        __in    BOOL            fCallSleep,
        __in    UINT            nSleepTime  
       );

    static void
    SendAltTab(
        __in    HWND            hwndDevWnd
        );

    static void
    SendAltEnter(
        __in    HWND            hwndDevWnd
        );

    static BOOL
    SetPrimaryMonitor(
        __in_ecount(nDevNameLen)  LPCSTR  pszDevName,
        __in    UINT            nDevNameLen,
        __in    DEVMODE&        devMode
       );

    static void
    WriteMsgs(
        __in_ecount(nMsgLen)  const PCHAR  pszMsg,
        __in    UINT            nMsgLen
       );
};
