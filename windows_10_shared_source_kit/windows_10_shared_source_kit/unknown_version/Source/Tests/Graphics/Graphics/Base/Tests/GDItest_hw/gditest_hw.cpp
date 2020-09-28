// GDITEST_HW.cpp : Defines the entry point for the application.
//
#include "ntstatus.h"
#define WIN32_NO_STATUS
#include <stdlib.h>
#include "stdafx.h"
#include "stdio.h"
#include "strsafe.h"
#include "Gditest_hw.h"
#include "CCompDC.h"
#include <stddef.h>
#include <math.h>
#include <WttLogSimple.h>
#include <d3d9.h>
#include <dxgi.h>
#include <dxgiwmi.h>
#include <d3dkmthk.h>
#include <winuserp.h>
#include <dwmapi.h>

#define MAX_LOADSTRING 100
//#define BFT_BITMAP 0x4d42 // 'BM' 

typedef HRESULT (WINAPI * PDXGIREPORTADAPTERCONFIGURATION)
(
 IN CONST DXGI_ADAPTER_CONFIGURATION* pConfig
 );

int g_iRenderOption = -1;
int g_fType = -1;
BOOL g_bExit = FALSE;
BOOL g_bRunAll = FALSE;
int g_MonitorIndex = -1;
int g_NumMonitor = 1;
int g_iTolerance = -1;
float g_MonitorOffset = 0.0;
HMONITOR g_hMonitor = NULL;
BOOL g_bDebug = FALSE;
BOOL g_bTolerance = FALSE;
BOOL g_bClip = FALSE;
BOOL g_bNoCheck = FALSE;
BOOL g_bDeviceBitmap = FALSE;
BOOL g_bMonitorDC = FALSE;
int  g_iTestCase = -1;
CSimpleWTTLOG* g_pMyLog = NULL;
DEVMODE OrigDevMode = {0};

void GetMonitorInfo();

// Global Variables:
HINSTANCE hInst;					// current instance
TCHAR szTitle[MAX_LOADSTRING];				// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM			MyRegisterClass(HINSTANCE hInstance);
BOOL			InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);


//-----------------------------------------------------------------------------
void ChangeDisplayMode()
{
    DEVMODE DevMode = {0};
    DevMode.dmSize = sizeof(DevMode);
    OrigDevMode.dmSize = sizeof(OrigDevMode);
    EnumDisplaySettingsEx (NULL,
        ENUM_CURRENT_SETTINGS, 
        &OrigDevMode,
        0);
    DevMode = OrigDevMode;
    {
        DevMode.dmPelsHeight = 768;
        DevMode.dmPelsWidth = 1024;
        DevMode.dmBitsPerPel = 32;
    }
    DevMode.dmFields =  DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

    ChangeDisplaySettings(&DevMode, 0);
    Sleep(4000);
}
void RestoreDisplayMode()
{
    if(OrigDevMode.dmSize <= 0)
        return;
    if(OrigDevMode.dmPelsHeight < 600 
        || OrigDevMode.dmPelsWidth < 800)
        return;

    ChangeDisplaySettings(&OrigDevMode, 0);
    Sleep(1000);
}

//----------------------------------------------------------------------------
void LowerCase(__in_ecount(strLength) TCHAR* szString, size_t strLength)
{
    for(size_t i=0; i<strLength; i++)
    {
        szString[i] = (TCHAR)tolower(szString[i]);
    }
}

int StringToInteger(const TCHAR* strTemp)
{
    TCHAR   ch = 0;
    int     out = 0;

    if (*strTemp == ' ' && *strTemp != '\0')
    {
        ch = ' ';
        while (*strTemp == ' ')
        {
            strTemp++;
        }

        if (*strTemp == '"')    
        {
            strTemp ++;
            ch = '"';
        }

        out = atoi((char*)strTemp);
    }
    else
    {
        out = -1;
    }

    return out;
}

//Process command lines
void GetCmdLineArgText(__in_bcount(inSize) TCHAR* strTemp, DWORD inSize,
                       __out_bcount(outSize) TCHAR* strRet, DWORD outSize)
{
    TCHAR   ch = 0;
    int     i = 0;;

    if (*strTemp == ' ' && *strTemp != '\0')
    {
        ch = ' ';
        while (*strTemp == ' ')
        {
            strTemp++;
        }

        if(*strTemp == '-' || *strTemp == '\\')
        {
            strRet[0] = '\0';
            return;
        }

        if (*strTemp == '"')    
        {
            strTemp ++;
            ch = '"';
        }

        for(i = 0; (*strTemp != ch && *strTemp != '\0') ; i++)
        {
            strRet[i] = *strTemp;
            strTemp++;
        }
    }
    strRet[i] = '\0'; // terminate the string
}

BOOL ParseCommandLine(__in_bcount(cmdLineSize) TCHAR* lpCmdLine, DWORD cmdLineSize)
{
    TCHAR   strCpyCmdLine[MAX_PATH];
    TCHAR   strTemp[MAX_PATH];
    TCHAR   *pStrCpy;
    TCHAR   *pStrOrigCmdLine;
    TCHAR   szUsage[2000];
    BOOL    bReturn = FALSE;

    ZeroMemory(strCpyCmdLine, MAX_PATH*sizeof(TCHAR));
    ZeroMemory(strTemp, MAX_PATH*sizeof(TCHAR));
    ZeroMemory(szUsage, 2000*sizeof(TCHAR));

    //Make a copy of the original command line to convert to lower case
    //to preserve the case of the input from the user for the name 
    //of the files on the command line.
    
    StringCchPrintf(strCpyCmdLine, MAX_PATH, TEXT(" %s"),lpCmdLine); //put in space to handle matching below for first arg
    LowerCase(strCpyCmdLine, (UINT)MAX_PATH);
    if(strCpyCmdLine[0] == 0)
    {
        //regard the blank input as querying the usage
        StringCchPrintf(strCpyCmdLine, MAX_PATH, TEXT("/?"));
    }
    if ((pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" /tolerance"))) || (pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" -tolerance"))) )
    {
        g_bTolerance = TRUE;
        pStrOrigCmdLine = (lpCmdLine + (pStrCpy - strCpyCmdLine));
        GetCmdLineArgText(pStrOrigCmdLine+10, cmdLineSize, strTemp, MAX_PATH);
        char * TempStr;
        if(strTemp[0] != '\0')
        {
            TempStr = (char*) strTemp;
            int iTol = (int)atoi(strTemp);
            if(iTol > 0 && iTol < 18)
            {
                g_iTolerance = iTol;
            }
        }
    }
    if ((pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" /nocheck"))) || (pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" -nocheck"))) )
    {
        g_bNoCheck = TRUE;
    }
    if ((pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" /debug"))) || (pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" -debug"))) )
    {
        g_bDebug = TRUE;
    }
    if ((pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" /clipping"))) || (pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" -clipping"))) )
    {
        g_bClip = TRUE;
    }
    if ((pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" /devicebitmap"))) || (pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" -devicebitmap"))) )
    {
        g_bDeviceBitmap = TRUE;
    }
    if ((pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" /monitordc"))) || (pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" -monitordc"))) )
    {
        if( g_MonitorIndex >= 1 || g_MonitorIndex < 0)
        {
            g_bMonitorDC = TRUE;
        }
    }
    if ((pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" /m"))) || (pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" -m"))) )
    {
        pStrOrigCmdLine = (lpCmdLine + (pStrCpy - strCpyCmdLine));
        GetCmdLineArgText(pStrOrigCmdLine+2, cmdLineSize, strTemp, MAX_PATH);
        char * TempStr;
        if(strTemp[0] != '\0')
        {
            TempStr = (char*) strTemp;
            float tempf = (float)atof(strTemp);
            if(tempf >= 0.0 && tempf < g_NumMonitor)
            {
                g_MonitorIndex = (int)tempf;
                g_MonitorOffset = tempf - g_MonitorIndex;
                //make sure the test cases for MonitorDC only apply for extended monitor
                if(g_MonitorIndex < 1 && g_bMonitorDC)
                {
                    g_bMonitorDC = FALSE;
                }
            }
        }
        if(-1 == g_MonitorIndex)
        {
            bReturn = FALSE;
        }
    }
    if ( (pStrCpy = _tcsstr((_TCHAR *)strCpyCmdLine, _TEXT(" /bitblt"))) || (pStrCpy = _tcsstr((_TCHAR *)strCpyCmdLine, TEXT(" -bitblt"))))
    {
        g_iRenderOption = 6;
        pStrOrigCmdLine = (lpCmdLine + (pStrCpy - strCpyCmdLine));
        GetCmdLineArgText(pStrOrigCmdLine+7, cmdLineSize, strTemp, MAX_PATH);

        int i = 0;
        while(strTemp[i] == ' ')
            i++;

        TCHAR* pTempStr = NULL;
        if(strTemp[i] != '\0')
        {
            pTempStr = (strTemp+i);
            g_iTestCase = atoi(pTempStr);
        }

        return TRUE;
    }
    if ( (pStrCpy = _tcsstr((_TCHAR *)strCpyCmdLine, _TEXT(" /colorfill"))) || (pStrCpy = _tcsstr((_TCHAR *)strCpyCmdLine, TEXT(" -colorfill"))))
    {
        g_iRenderOption = 2;
        pStrOrigCmdLine = (lpCmdLine + (pStrCpy - strCpyCmdLine));
        GetCmdLineArgText(pStrOrigCmdLine+10, cmdLineSize, strTemp, MAX_PATH);

        int i = 0;
        while(strTemp[i] == ' ')
            i++;

        TCHAR* pTempStr = NULL;
        if(strTemp[i] != '\0')
        {
            pTempStr = (strTemp+i);
            g_iTestCase = atoi(pTempStr);
        }
        return TRUE;
    }
    if ((pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" /stretchblt"))) || (pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" -stretchblt"))))
    {
        g_iRenderOption = 8;
        pStrOrigCmdLine = (lpCmdLine + (pStrCpy - strCpyCmdLine));
        GetCmdLineArgText(pStrOrigCmdLine+11, cmdLineSize, strTemp, MAX_PATH);

        int i = 0;
        while(strTemp[i] == ' ')
            i++;

        TCHAR* pTempStr = NULL;
        if(strTemp[i] != '\0')
        {
            pTempStr = (strTemp+i);
            g_iTestCase = atoi(pTempStr);
        }
        return TRUE;
    }
    if ((pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" /transparentblt"))) || (pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" -transparentblt"))))
    {
        g_iRenderOption = 7;
        pStrOrigCmdLine = (lpCmdLine + (pStrCpy - strCpyCmdLine));
        GetCmdLineArgText(pStrOrigCmdLine+15, cmdLineSize, strTemp, MAX_PATH);

        int i = 0;
        while(strTemp[i] == ' ')
            i++;

        TCHAR* pTempStr = NULL;
        if(strTemp[i] != '\0')
        {
            pTempStr = (strTemp+i);
            g_iTestCase = atoi(pTempStr);
        }
        return TRUE;
    }
    if ((pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" /textoutall"))) || (pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" -textoutall"))))
    {
        g_iRenderOption = 11;
        g_fType = 0; //the type is used for indentifying the font: 0-all fonts;1-default font;2-antialias font;3-cleartype font
        bReturn = TRUE;
    }
    if ((pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" /textoutantialiast"))) || (pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" -textoutantialiast"))))
    {
        g_iRenderOption = 10;
        g_fType = 3;

        return TRUE;
    }
    if ((pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" /textoutcleartypet"))) || (pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" -textoutcleartypet"))))
    {
        g_iRenderOption = 5;
        g_fType = 5;

        pStrOrigCmdLine = (lpCmdLine + (pStrCpy - strCpyCmdLine));
        GetCmdLineArgText(pStrOrigCmdLine+18, cmdLineSize, strTemp, MAX_PATH);

        int i = 0;
        while(strTemp[i] == ' ')
            i++;

        TCHAR* pTempStr = NULL;
        if(strTemp[i] != '\0')
        {
            pTempStr = (strTemp+i);
            g_iTestCase = atoi(pTempStr);
        }
        return TRUE;
    }
    if ((pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" /alphablend"))) || (pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" -alphablend"))))
    {
        g_iRenderOption = 1;
        pStrOrigCmdLine = (lpCmdLine + (pStrCpy - strCpyCmdLine));
        GetCmdLineArgText(pStrOrigCmdLine+11, cmdLineSize, strTemp, MAX_PATH);

        int i = 0;
        while(strTemp[i] == ' ')
            i++;

        TCHAR* pTempStr = NULL;
        if(strTemp[i] != '\0')
        {
            pTempStr = (strTemp+i);
            g_iTestCase = atoi(pTempStr);
        }
        return TRUE;
    }
    if ( (pStrCpy = _tcsstr((_TCHAR *)strCpyCmdLine, _TEXT(" /lineto"))) || (pStrCpy = _tcsstr((_TCHAR *)strCpyCmdLine, TEXT(" -lineto"))))
    {
        g_iRenderOption = 9;
        pStrOrigCmdLine = (lpCmdLine + (pStrCpy - strCpyCmdLine));
        GetCmdLineArgText(pStrOrigCmdLine+7, cmdLineSize, strTemp, MAX_PATH);

        int i = 0;
        while(strTemp[i] == ' ')
            i++;

        TCHAR* pTempStr = NULL;
        if(strTemp[i] != '\0')
        {
            pTempStr = (strTemp+i);
            g_iTestCase = atoi(pTempStr);
        }
        return TRUE;
    }
    if ( (pStrCpy = _tcsstr((_TCHAR *)strCpyCmdLine, _TEXT(" /strokepath"))) || (pStrCpy = _tcsstr((_TCHAR *)strCpyCmdLine, TEXT(" -strokepath"))))
    {
        g_iRenderOption = 3;
        pStrOrigCmdLine = (lpCmdLine + (pStrCpy - strCpyCmdLine));
        GetCmdLineArgText(pStrOrigCmdLine+11, cmdLineSize, strTemp, MAX_PATH);

        int i = 0;
        while(strTemp[i] == ' ')
            i++;

        TCHAR* pTempStr = NULL;
        if(strTemp[i] != '\0')
        {
            pTempStr = (strTemp+i);
            g_iTestCase = atoi(pTempStr);
        }

        //StringCchPrintf(strCpyCmdLine, MAX_PATH, TEXT("The optional num is: %d"),g_iTestCase);
        //MessageBox(NULL, strCpyCmdLine, TEXT("GDI Functional Test"), MB_OK);
        return TRUE;
    }
    if ( (pStrCpy = _tcsstr((_TCHAR *)strCpyCmdLine, _TEXT(" /maskblt"))) || (pStrCpy = _tcsstr((_TCHAR *)strCpyCmdLine, TEXT(" -maskblt"))))
    {
        g_iRenderOption = 12;
        pStrOrigCmdLine = (lpCmdLine + (pStrCpy - strCpyCmdLine));
        GetCmdLineArgText(pStrOrigCmdLine+7, cmdLineSize, strTemp, MAX_PATH);

        int i = 0;
        while(strTemp[i] == ' ')
            i++;

        TCHAR* pTempStr = NULL;
        if(strTemp[i] != '\0')
        {
            pTempStr = (strTemp+i);
            g_iTestCase = atoi(pTempStr);
        }

        return TRUE;
    }
    if ((pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" /all"))) || (pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" -all"))) )
    {
        g_bRunAll = TRUE;
        bReturn = TRUE;
    }
    if(!bReturn) //if ((pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" /?"))) || (pStrCpy = _tcsstr(strCpyCmdLine, TEXT(" -?"))))
    {
        StringCchCopy(szUsage, 2000, TEXT("Usage:\n"));
        StringCchCat(szUsage, 2000, TEXT("-bitblt: Test pass for bitblt.\n"));
        StringCchCat(szUsage, 2000, TEXT("-colorfill: Test pass for colorfill.\n"));
        StringCchCat(szUsage, 2000, TEXT("-alphablend: Test pass for alphablend.\n"));
        StringCchCat(szUsage, 2000, TEXT("-transparentblt: Test pass for transparentblt.\n"));
        StringCchCat(szUsage, 2000, TEXT("-stretchblt: Test pass for stretchblt.\n"));
        StringCchCat(szUsage, 2000, TEXT("-textoutall: Test pass for output text with all three kinds of fonts.\n"));
        StringCchCat(szUsage, 2000, TEXT("-textoutdefaultT: Test pass for output text with TRANSPARENT mode and the default font.\n"));
        StringCchCat(szUsage, 2000, TEXT("-textoutantialiasT: Test pass for output text with TRANSPARENT mode and Anti-alias font.\n"));
        StringCchCat(szUsage, 2000, TEXT("-textoutcleartypeT: Test pass for output text with TRANSPARENT mode and Clear-type font.\n"));
        StringCchCat(szUsage, 2000, TEXT("-lineto: Test pass for drawing lines on the screen.\n"));
        StringCchCat(szUsage, 2000, TEXT("-strokepath: Test pass for doing strokepath with lines on the screen.\n"));
        StringCchCat(szUsage, 2000, TEXT("-maskblt: Test pass for maskblt.\n"));
        StringCchCat(szUsage, 2000, TEXT("-clipping: Using clipping.\n"));
        StringCchCat(szUsage, 2000, TEXT("-all: Test pass for all scenarios.\n"));
        StringCchCat(szUsage, 2000, TEXT("-debug: The GDI operations will be iteratively executed.\n"));
        StringCchCat(szUsage, 2000, TEXT("-tolerance: Adding tolerance, i.e. the error no more than 2 in each pixel is ignored.\n"));
        StringCchPrintf(strTemp, MAX_PATH, _T("-m [*.*](0=<m<%d): The float value for the position of this test on the indicated monitor.\n"), g_NumMonitor);
        StringCchCat(szUsage, 2000, strTemp);

        //TODO - put in a better usage message
        MessageBox(NULL,szUsage, TEXT("GDITEST_HW Options"), MB_OK);
        return FALSE;
    }

    return TRUE;
}

//Monitors

BOOL CALLBACK MonitorEnumProc(
                              HMONITOR hMonitor,  // handle to display monitor
                              HDC hdcMonitor,     // handle to monitor DC
                              LPRECT lprcMonitor, // monitor intersection rectangle
                              LPARAM dwData       // data
                              )
{
    UINT* pCounter = (UINT*)dwData;
    TCHAR s[256];

    MONITORINFOEX mi;
    ZeroMemory(&mi, sizeof(MONITORINFOEX));
    mi.cbSize = sizeof(mi);
    GetMonitorInfo(hMonitor, &mi);
    wprintf(L"Monitor %d: %s\n", *pCounter, mi.szDevice);
    (*pCounter)++;
    return TRUE;
}
//-----------------------------------------------------------------------------
BOOL CALLBACK MonitorEnumProcFind(
                                  HMONITOR hMonitor,  // handle to display monitor
                                  HDC hdcMonitor,     // handle to monitor DC
                                  LPRECT lprcMonitor, // monitor intersection rectangle
                                  LPARAM dwData       // data
                                  )
{
    UINT* pCounter = (UINT*)dwData;
    TCHAR s[256];
    static UINT Counter = 0;

    if (Counter == (UINT)dwData)
    {
        g_hMonitor = hMonitor;
        return FALSE;
    }
    Counter++;
    return TRUE;
}
struct VIDEO_INFO
{
    TCHAR	szDriverName[32];
    TCHAR	szDriverVersion[32];
    TCHAR	szDriverDate[32];
    TCHAR	szAdapterName[64];
    TCHAR	szDeviceName[32];
    TCHAR	szVendorId[2];
};
int GetVideoInfo(VIDEO_INFO* pVidInfo)
{
    HRESULT hr = S_OK;
    D3DADAPTER_IDENTIFIER9      Adapter;
    IDirect3D9*                 pD3D9 = NULL;
    HMODULE                     hD3DDLL = NULL;

    hD3DDLL = LoadLibrary(_T("D3D9.DLL"));
    if(!hD3DDLL)
    {
        return -1;
    }
    // Get the D3D creation entry point
    IDirect3D9 *(WINAPI *Direct3DCreate9)(UINT)  = (IDirect3D9*(WINAPI *)(UINT))GetProcAddress(hD3DDLL, "Direct3DCreate9");
    if (NULL == Direct3DCreate9)
    {
        OutputDebugString(_T("CMachineInfo::GetVidInfo() - Couldn't get Direct3DCreate9 address\n"));
        FreeLibrary(hD3DDLL);
        return -1;
    }

    // Create the D3D object
    pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
    if (NULL == pD3D9)
    {
        OutputDebugString(_T("CMachineInfo::GetVidInfo()- Direct3DCreate9() failed.  Header/Runtime mismatch likely.\n"));
        FreeLibrary(hD3DDLL);
        return -1;
    }

    // Get the number of adapters
    DWORD nAdapters = pD3D9->GetAdapterCount();
    if(0 == nAdapters)
    {
        OutputDebugString(_T("CMachineInfo::GetVidInfo() - GetAdapterCount() returned 0.  No display devices available.\n"));
        pD3D9->Release();
        FreeLibrary(hD3DDLL);

        return -1;
    }

    if(NULL == pVidInfo)
    {
        pD3D9->Release();
        return nAdapters;
    }

    // Enumerate the adapters
    DWORD dwAdapters;
    for (dwAdapters = 0; dwAdapters < nAdapters; dwAdapters++)
    {
        hr = pD3D9->GetAdapterIdentifier(dwAdapters, NULL, &Adapter);
        if (FAILED(hr))
        {
            OutputDebugString(_T("CMachineInfo::GetVidInfo() - GetAdapterIdentifier() failed."));

            pD3D9->Release();
            FreeLibrary(hD3DDLL);
            return dwAdapters;
        }

        // create the version string.
        {
            TCHAR tcsVer[32] = {0};
            memset( tcsVer, 0, sizeof tcsVer );

            StringCchPrintf(tcsVer, (sizeof tcsVer / sizeof tcsVer[0]) - 1, TEXT("%d.%d.%d.%d"),
                HIWORD(Adapter.DriverVersion.HighPart), // product
                LOWORD(Adapter.DriverVersion.HighPart), // version
                HIWORD(Adapter.DriverVersion.LowPart),  // sub-version
                LOWORD(Adapter.DriverVersion.LowPart)); // build

            StringCchPrintf(pVidInfo[dwAdapters].szDriverVersion, 32, tcsVer);
        }

        // get the adapter name
        StringCchPrintf(pVidInfo[dwAdapters].szAdapterName, 64, Adapter.Description);

        // get the driver name
        StringCchPrintf(pVidInfo[dwAdapters].szDriverName, 32, Adapter.Driver);

        // get the device name for the adapter
        StringCchPrintf(pVidInfo[dwAdapters].szDeviceName, 32, Adapter.DeviceName);

        // get the Vendor ID
        StringCchPrintf(pVidInfo[dwAdapters].szVendorId, 2, TEXT("%d"), Adapter.VendorId);
    }

    pD3D9->Release();
    FreeLibrary(hD3DDLL);
    return dwAdapters;
}

int CheckPerfStatus(HWND hWnd)
{
    HDC hDC = GetDC(hWnd);
    if (hDC == NULL)
    {
        OutputDebugString(_T("DC is NULL\n"));
        return 0;
    }
    HDC hMemoryDC = CreateCompatibleDC(hDC);
    if (hMemoryDC == NULL)
    {
        OutputDebugString(_T("DC is NULL\n"));
        return 0;
    }

    // Obtain the window's client rectangle 
    RECT r;
    GetClientRect(hWnd, &r); 
    UINT Width = r.right - r.left;
    UINT Height = r.bottom - r.top;

    UINT BitmapSizeX = (r.right - r.left);
    UINT BitmapSizeY = (r.bottom - r.top);

    HBITMAP hBitmap = CreateCompatibleBitmap (hDC, BitmapSizeX, BitmapSizeY);
    if (hBitmap == NULL)
    {
        ReleaseDC(hWnd, hDC);
        OutputDebugString(_T("DC is NULL\n"));
        return 0;
    }

    HBRUSH hBrush = CreateSolidBrush(0x00FFFFFF);
    if (hBrush == NULL)
    {
        OutputDebugString(_T("hBrush is NULL\n"));
        return 0;
    }
    {
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hMemoryDC, hBrush);
        // Fill the background of the bitmap
        if (!BitBlt(hMemoryDC, 0, 0, BitmapSizeX, BitmapSizeY, NULL, 0, 0, PATCOPY))
        {
            OutputDebugString(_T("BitBlt failed\n"));
        }
        SelectObject(hMemoryDC, hOldBrush);
    }

    LARGE_INTEGER freq, Time1, Time2;
    QueryPerformanceFrequency(&freq);
    double TimeSpan = 1000.0/freq.LowPart;      // To convert to milliseconds

    TCHAR s[256];
    double time, totaltime;
    totaltime = 0;

    //Screen-to-screen BitBlt
    for (UINT i = 0; i < 10; i++)
    {
        QueryPerformanceCounter(&Time1);
        for (UINT j = 0; j < 6; j++){
            for (UINT k = 0; k < 6; k++){
                //timer.MarkTime();
                // Do BLT from bitmap to screen
                BitBlt(hDC, 0, 0, BitmapSizeX, BitmapSizeY, hDC, 0, 0, SRCCOPY);
            }
        }
        QueryPerformanceCounter(&Time2);
        time = (Time2.QuadPart - Time1.QuadPart)*TimeSpan;
        totaltime += time;
        Sleep(160);
    }

    StringCchPrintf(s, 256, _T("Elapsed time for 360 same-surface Screen-to-Screen BitBlt: %lf\n"), totaltime);
    //g_pMyLog->Message(s);

    double totaltime1 = 0;

    //Separate BitBlt with readback
    QueryPerformanceCounter(&Time1);
    for (UINT i = 0; i < 360; i++)
    {
        // Do BLT from Screen to bitmap
        BitBlt(hMemoryDC, 0, 0, BitmapSizeX, BitmapSizeY, hDC, 0, 0, SRCCOPY);
        //color = GetPixel(hDC, x,y);
    }
    QueryPerformanceCounter(&Time2);
    totaltime1 = (Time2.QuadPart - Time1.QuadPart)*TimeSpan;

    StringCchPrintf(s, 256, _T("Elapsed time for 360 Screen-to-System BitBlt: %lf\n"), totaltime1);
    //g_pMyLog->Message(s);

    DeleteObject(hBrush);
    DeleteDC(hMemoryDC);
    DeleteObject(hBitmap);
    ReleaseDC(hWnd, hDC);

    if(totaltime1 > 6.0*totaltime)
        return 1;
    else
        return -1;

    /*Because of the optimization for drawing lines, the perf numbers for Lineto is
    no appropriate for checking the status of GDI HW ACC

    //Use LineTo to draw horizontal and vertical lines on the screen
    HPEN hPen = (HPEN)SelectObject(hDC,GetStockObject(WHITE_PEN));

    for(UINT row=50; row<500; row+=10)
    {
    //MoveTo(row, 500, NULL);
    QueryPerformanceCounter(&Time1);
    for(UINT collum=500; collum>50; collum-=10)
    {
    LineTo(hDC, 50+row, 550-collum);
    LineTo(hDC, 50+row, 50+collum);
    LineTo(hDC, 550-row, 50+collum);
    LineTo(hDC, 550-row, 550-collum);
    }
    // Flush the internal buffer
    //COLORREF color = GetPixel(hDC, 1,1);

    QueryPerformanceCounter(&Time2);
    time = (Time2.QuadPart - Time1.QuadPart)*TimeSpan;
    totaltime += time;
    }

    StringCchPrintf(s, 256, _T("Elapsed time for using LineTo to draw horizontal and vertical lines on the screen: %lf\n"), totaltime);
    OutputDebugString(s);
    //g_pMyLog->Message(s);

    //Use LineTo to draw lines with 45 degree  on the screen
    double totaltime1 = 0;
    for(UINT row=50; row<500; row+=5)
    {
    QueryPerformanceCounter(&Time1);
    for(UINT collum=500; collum>50; collum-=5)
    {
    LineTo(hDC, row, collum);
    LineTo(hDC, collum, row);
    }
    // Flush the internal buffer
    //COLORREF color = GetPixel(hDC, 1,1);

    QueryPerformanceCounter(&Time2);
    time = (Time2.QuadPart - Time1.QuadPart)*TimeSpan;
    totaltime1 += time;
    }

    StringCchPrintf(s, 256, _T("Elapsed time for using LineTo to draw lines with different directions on the screen: %lf\n"), totaltime1);
    //g_pMyLog->Message(s);
    OutputDebugString(s);

    SelectObject(hDC,hPen);
    DeleteObject(hBitmap);
    DeleteDC(hMemoryDC);
    ReleaseDC(hWnd, hDC);

    if(totaltime1 > 1.8*totaltime)
    return 1;
    else
    return -1;
    */
}

//Modified from DX test framework
int CheckDriverModel()
{
    HMODULE hDXGI = NULL;
    PDXGIREPORTADAPTERCONFIGURATION pDXGIReportAdapterConfiguration = NULL;
    HDC hDC = NULL;
    HMODULE hGDI32 = NULL;

    //firstly load DXGI
    hDXGI = LoadLibraryA( "DXGI.dll" );
    if(NULL != hDXGI)
    {
        pDXGIReportAdapterConfiguration = (PDXGIREPORTADAPTERCONFIGURATION)
            GetProcAddress(hDXGI, "DXGIReportAdapterConfiguration");

        if ( NULL != pDXGIReportAdapterConfiguration != NULL )
        {
            DXGI_ADAPTER_CONFIGURATION DriverModelConfiguration;
            ZeroMemory( &DriverModelConfiguration, sizeof( DriverModelConfiguration ) );

            DXGI_DRIVERMODELINFO DriverModelInfo;
            ZeroMemory( &DriverModelInfo, sizeof( DriverModelInfo ) );

            UINT iAdapter = 0;

            DriverModelConfiguration.AdapterIndex = iAdapter;
            DriverModelConfiguration.Type = DXGIACTYPE_DRIVERMODEL;
            DriverModelConfiguration.pPrivateDriverData = &DriverModelInfo;
            DriverModelConfiguration.PrivateDriverDataSize = sizeof( DriverModelInfo );

            HRESULT hGetDriverModelInfoResult = (*pDXGIReportAdapterConfiguration)
                (
                &DriverModelConfiguration
                );

            if ( SUCCEEDED( hGetDriverModelInfoResult ) )
            {
                if( DriverModelInfo.Version > KMT_DRIVERVERSION_WDDM_1_0)
                {
                    pDXGIReportAdapterConfiguration = NULL;
                    FreeLibrary( hDXGI );
                    hDXGI = NULL;
                    return 1;
                }
                else
                {
                    switch ( DriverModelInfo.Version )
                    {
                    case KMT_DRIVERVERSION_WDDM_1_0:
                        g_pMyLog->Message(TEXT("WDDMv1.0 driver is used"));
                        break;
                        //case KMT_DRIVERVERSION_XPDM:
                        //	g_pMyLog->Message(TEXT("XPDM driver is used"));
                        //	break;
                    default:
                        //g_pMyLog->Message(TEXT("UNKNOWN driver is used"));
                        break;
                    }
                    pDXGIReportAdapterConfiguration = NULL;
                    FreeLibrary( hDXGI );
                    hDXGI = NULL;
                    return -1;
                }
            }
        }
    }

    //In case the above fails, the thunk of QueryAdapterInfo needs to be directly called
    int iResult = 0;

    //Get the DC on the primary device
    DISPLAY_DEVICE dd;
    memset(&dd, 0, sizeof (dd));
    dd.cb = sizeof dd;

    for (int i = 0; EnumDisplayDevices(NULL, i, &dd, 0); ++i)
    {
        if (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
        {
            break;
        }
    }

    hDC = CreateDC (NULL, dd.DeviceName, NULL, NULL);

    if(NULL == hDC)
    {
        g_pMyLog->Message(TEXT("CreateDC failed"));
        return iResult; //this function return void result;
    }

    //load gdi libary
    hGDI32 = LoadLibrary(_T("gdi32.dll"));

    if ( hGDI32 == NULL )
    {
        g_pMyLog->Message(TEXT("load gdi32.dll failed"));
        DeleteDC(hDC);
        return iResult;
    }

    PFND3DKMT_OPENADAPTERFROMHDC pfnOsThunkDDIOpenAdapterFromHdc = 
        (PFND3DKMT_OPENADAPTERFROMHDC) GetProcAddress(hGDI32, "D3DKMTOpenAdapterFromHdc");

    if(NULL == pfnOsThunkDDIOpenAdapterFromHdc)
    {
        g_pMyLog->Message(TEXT("GetProcAddress for D3DKMTOpenAdapterFromHdc failed"));
        DeleteDC(hDC);
        FreeLibrary( hGDI32 );
        return iResult;
    }

    PFND3DKMT_CLOSEADAPTER pfnOsThunkDDICloseAdapter = 
        (PFND3DKMT_CLOSEADAPTER) GetProcAddress(hGDI32, "D3DKMTCloseAdapter");

    if(NULL == pfnOsThunkDDICloseAdapter)
    {
        g_pMyLog->Message(TEXT("GetProcAddress for D3DKMTCloseAdapter failed"));
        pfnOsThunkDDIOpenAdapterFromHdc = NULL;
        DeleteDC(hDC);
        FreeLibrary( hGDI32 );
        return iResult;
    }

    D3DKMT_OPENADAPTERFROMHDC OpenAdapterFromHdc;
    OpenAdapterFromHdc.hDc = hDC;

    NTSTATUS OpenAdapterResult = (*pfnOsThunkDDIOpenAdapterFromHdc)
        (
        &OpenAdapterFromHdc
        );

    if ( OpenAdapterResult != STATUS_SUCCESS )
    {
        g_pMyLog->Message(TEXT("pfnOsThunkDDIOpenAdapterFromHdc failed"));
        pfnOsThunkDDIOpenAdapterFromHdc = NULL;
        pfnOsThunkDDICloseAdapter = NULL;
        DeleteDC(hDC);
        FreeLibrary( hGDI32 );
        return iResult;
    }

    PFND3DKMT_QUERYADAPTERINFO pfnOsThunkDDIQueryAdapterInfo = 
        (PFND3DKMT_QUERYADAPTERINFO)GetProcAddress(
        hGDI32, 
        "D3DKMTQueryAdapterInfo" );

    if(NULL != pfnOsThunkDDIQueryAdapterInfo)
    {
        D3DKMT_QUERYADAPTERINFO QueryAdapterInfo;
        D3DKMT_DRIVERVERSION  DriverVersion;

        ZeroMemory(&QueryAdapterInfo, sizeof(QueryAdapterInfo));
        ZeroMemory(&DriverVersion, sizeof(DriverVersion));

        QueryAdapterInfo.hAdapter = OpenAdapterFromHdc.hAdapter;
        QueryAdapterInfo.Type =  KMTQAITYPE_DRIVERVERSION;
        QueryAdapterInfo.pPrivateDriverData = &DriverVersion;
        QueryAdapterInfo.PrivateDriverDataSize = sizeof(DriverVersion);

        if (STATUS_SUCCESS == ((*pfnOsThunkDDIQueryAdapterInfo)(&QueryAdapterInfo)))
        {
            if ( DriverVersion > 1000 ) // > KMT_DRIVERVERSION_WDDM_1_0
            {
                iResult = 1;
            }
            else if ( DriverVersion == 1000 ) // KMT_DRIVERVERSION_WDDM_1_0
            {
                iResult = -1;
                g_pMyLog->Message(TEXT("WDDMv1.0 driver is used"));
            }
            else
            {
                iResult = 0;
            }
        }
    }

    D3DKMT_CLOSEADAPTER CloseAdapter;
    CloseAdapter.hAdapter = OpenAdapterFromHdc.hAdapter;

    (*pfnOsThunkDDICloseAdapter)( &CloseAdapter	);

    pfnOsThunkDDIOpenAdapterFromHdc = NULL;
    pfnOsThunkDDICloseAdapter = NULL;
    pfnOsThunkDDIQueryAdapterInfo = NULL;
    if(NULL != hDC)
        DeleteDC(hDC);
    if(NULL != hGDI32)
        FreeLibrary( hGDI32 );

    return iResult;
}
BOOL LogTestResults(TCHAR* TestName, TCHAR* TestCaseName, ERROR_INFO* ErrorInfo, UINT TestCaseNum, BOOL bUseDeviceBitmap)
{
    TCHAR s[256];
    TCHAR ScenarioName[256];
    BOOL bNoFail = TRUE;

    if( bUseDeviceBitmap )
    {
        StringCchPrintf(ScenarioName, 256, TEXT("DeviceBitmap-%s"), TestName);
    }
    else
    {
        StringCchPrintf(ScenarioName, 256, TEXT("%s"), TestName);
    }
    if(ErrorInfo->NumofColorError > 0)
    {
        bNoFail = FALSE;
        StringCchPrintf(s, sizeof s/sizeof s[0], _T("The test for %s failed\n"), ScenarioName);
        g_pMyLog->Fail(s);
        StringCchPrintf(s, sizeof s/sizeof s[0], _T("Total Number of mis-matched pixels: %d; Average difference in Red-Channel: %d, Green-Channel: %d, Blue-Channel: %d\n"), 
            ErrorInfo->NumofColorError, ErrorInfo->AveRedError, ErrorInfo->AveGreenError, ErrorInfo->AveBlueError);
        g_pMyLog->Message(s);
        StringCchPrintf(s, sizeof s/sizeof s[0], _T("The max difference in mis-matched pixels: Red-Channel: %d, Green-Channel: %d, Blue-Channel: %d\n"), 
            ErrorInfo->MaxRedError, ErrorInfo->MaxGreenError, ErrorInfo->MaxBlueError);
        g_pMyLog->Message(s);
        StringCchPrintf(s, sizeof s/sizeof s[0], _T("The coordinate of pixel with max mis-matched error: (%d, %d)\n"), 
            ErrorInfo->MaxPixelX, ErrorInfo->MaxPixelY);
        g_pMyLog->Message(s);
        StringCchPrintf(s, sizeof s/sizeof s[0], _T("The RGB value of pixel with max mis-matched error: RGB(%d, %d %d)\n"), 
            ErrorInfo->RedValue, ErrorInfo->GreenValue, ErrorInfo->BlueValue);
        g_pMyLog->Message(s);
        StringCchPrintf(s, sizeof s/sizeof s[0], TEXT("The images were generated with names of %s*.bmp\n"), ScenarioName);
        g_pMyLog->Message(s);
        if(TestCaseNum < 9999)
        {
            StringCchPrintf(s, sizeof s/sizeof s[0], TEXT("Repro: Gditest_hw.exe -tolerance -%s %d\n"), TestCaseName, TestCaseNum);
        }
        else
        {
            StringCchPrintf(s, sizeof s/sizeof s[0], TEXT("Repro: Gditest_hw.exe -tolerance -%s\n"), TestCaseName);
        }
        g_pMyLog->Message(s);
    }
    else
    {
        StringCchPrintf(s, sizeof s/sizeof s[0], _T("PASS: %s passed\n"), ScenarioName);
        g_pMyLog->Message(s);
        StringCchPrintf(s, sizeof s/sizeof s[0], _T("The max difference in mis-matched pixels: Red-Channel: %d, Green-Channel: %d, Blue-Channel: %d\n"), 
            ErrorInfo->MaxRedError, ErrorInfo->MaxGreenError, ErrorInfo->MaxBlueError);
        g_pMyLog->Message(s);
    }
    return bNoFail;
}
//-----------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
    bool bSkipped = false;
    if(g_pMyLog == NULL)
    {
        g_pMyLog = new CSimpleWTTLOG(TEXT("GDITEST_HW.wtl"));
    }

    if(NULL == g_pMyLog)
    {
        OutputDebugString(_T("Failed to create WttLog\n"));
        return -1;
    }
    if(!g_pMyLog->IsValid())
    {
        OutputDebugString(_T("Failed to create valid WttLog\n"));
        return -1;
    }

    if(!EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&g_NumMonitor))
    {
        g_pMyLog->Warn(TEXT("The EnumDisplayMonitors did not succeed"));
    }		

    if(lpCmdLine!=NULL)
    {
        if(!ParseCommandLine((TCHAR*)lpCmdLine, MAX_PATH))
        {
            g_pMyLog->Fail(TEXT("The parsing command lines failed"));
            if(NULL != g_pMyLog)
                delete g_pMyLog;
            return -1;
        }
    }
    else
    {
        g_pMyLog->Warn(TEXT("No command line option for rendering"));
        TCHAR   strCpyCmdLine[MAX_PATH];
        StringCchPrintf(strCpyCmdLine, MAX_PATH, TEXT("/?"));
        ParseCommandLine(strCpyCmdLine, MAX_PATH);
        if(NULL != g_pMyLog)
            delete g_pMyLog;
        return -1;
    }

    //check if the driver >= 1.1
    g_pMyLog->StartTest(TEXT("Check Driver Version"));
    int iRes = CheckDriverModel();
    if(iRes < 0) //driver version < WDDMv1.1
    {
        g_pMyLog->Skip();
        bSkipped = true;
        if(!g_bNoCheck)
        {
            if(NULL != g_pMyLog)
                delete g_pMyLog;
            return -1;
        }
    }
    else if(iRes > 0)
    {
        g_pMyLog->Message(TEXT("Driver version used is greater than WDDMv1.1"));
    }
    else
    {
        g_pMyLog->Message(TEXT("UNKNOWN driver (most likely XDDM driver) is used"));//wait later for checking GDI HW ACC
    }
    if(bSkipped == false)
    {
        g_pMyLog->EndTest();
    }
    bSkipped = false;

    //get info of display card and video driver
    g_pMyLog->StartTest(TEXT("Log Adapter and Video Driver Info"));
    int nAdapters = GetVideoInfo(NULL);
    if(nAdapters < 0)
    {
        g_pMyLog->Warn(TEXT("Failed to get video driver info"));
        g_pMyLog->Skip();
        bSkipped = true;
    }
    else if(0 == nAdapters)
    {
        g_pMyLog->Warn(TEXT("Failed to get video driver info with zero adapter"));
        g_pMyLog->Skip();
        bSkipped = true;
    }
    else
    {
        VIDEO_INFO* pVidInfo = (VIDEO_INFO*)new VIDEO_INFO[nAdapters];
        if(NULL == pVidInfo)
        {
            g_pMyLog->Warn(TEXT("Out of Memory and thus failed to get video info"));
            g_pMyLog->Skip();
            bSkipped = true;
        }
        else
        {
            memset(pVidInfo, 0, sizeof(VIDEO_INFO)*nAdapters);

            nAdapters = GetVideoInfo(pVidInfo);
            if(nAdapters <= 0 || NULL == pVidInfo)
            {
                g_pMyLog->Warn(TEXT("Failed to get video driver info"));
                g_pMyLog->Skip();
                bSkipped = true;
            }
            else
            {
                for(int i = 0; i < nAdapters; i++)
                {
                    TCHAR tempText[128];
                    StringCchPrintf(tempText, 128, TEXT("Display Card Description: %s"), pVidInfo[i].szAdapterName);
                    g_pMyLog->Message(tempText);

                    StringCchPrintf(tempText, 64, TEXT("Driver Name for the Adapter: %s"), pVidInfo[i].szDriverName); 
                    g_pMyLog->Message(tempText);

                    StringCchPrintf(tempText, 64, TEXT("Driver Version for the Adapter: %s"), pVidInfo[i].szDriverVersion); 
                    g_pMyLog->Message(tempText);

                    StringCchPrintf(tempText, 64, TEXT("Device Name for the Adapter: %s"), pVidInfo[i].szDeviceName); 
                    g_pMyLog->Message(tempText);

                    StringCchPrintf(tempText, 32, TEXT("VendorId for the Adapter: %s"), pVidInfo[i].szVendorId); 
                    g_pMyLog->Message(tempText);
                }
            }
        }
        if(NULL != pVidInfo)
            delete pVidInfo;
    }
    if(bSkipped == false)
    {
        g_pMyLog->EndTest();			
    }
    bSkipped = false;

    //Make the mode to be default one with 1280x1024x32
    //ChangeDisplayMode();

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    MSG msg;
    HACCEL hAccelTable;

    // Initialize global strings
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_GDITEST_HW, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        g_pMyLog->Message(TEXT("InitInstance failed"));
        if(NULL != g_pMyLog)
        {
            delete g_pMyLog;
        }
        RestoreDisplayMode();
        return -1;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GDITEST_HW));

    // Main message loop:
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (g_bExit)
        {
            //ChangeDisplayMode();
            g_bExit = FALSE;
        }
    }

    if(NULL != g_pMyLog)
    {
        delete g_pMyLog;
    }
    //RestoreDisplayMode();

    return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style		= CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc	= WndProc;
    wcex.cbClsExtra		= 0;
    wcex.cbWndExtra		= 0;
    wcex.hInstance		= hInstance;
    wcex.hIcon		= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GDITEST_HW));
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_GDITEST_HW);
    wcex.lpszClassName	= szWindowClass;
    wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}


//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
    BOOL bResult;

    hInst = hInstance; // Store instance handle in our global variable

    int PosX = 0;
    int PosY = 0;
    if (g_MonitorIndex != -1)
    {
        bResult = EnumDisplayMonitors(NULL, NULL, MonitorEnumProcFind, (LPARAM)g_MonitorIndex);
        if (g_hMonitor != NULL)
        {
            //Move the winwod on the indicated monitor
            MONITORINFOEX mi;
            mi.cbSize = sizeof(mi);
            GetMonitorInfo(g_hMonitor, &mi);
            PosX = mi.rcMonitor.left;
            PosY = mi.rcMonitor.top;
            if((mi.rcWork.right - mi.rcWork.left) > 0)
            {
                PosX += (int)(g_MonitorOffset*(mi.rcWork.right - mi.rcWork.left));
            }
            if((mi.rcWork.bottom - mi.rcWork.top) > 0)
            {
                PosY += (mi.rcWork.bottom - mi.rcWork.top)/3;
            }
        }
    }
    //put the window on the toppest in case there are other apps 
    DWORD dwExStyle = WS_EX_TOPMOST;
    hWnd = CreateWindowEx(dwExStyle, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW  | WS_CLIPCHILDREN,
        PosX, PosY, 
        330, 330, 
        NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}
//-----------------------------------------------------------------------------
UINT GetRandomNumber(UINT MaxValue)
{
    return (UINT)((rand() / (float)RAND_MAX) * MaxValue);
}

//-----------------------------------------------------------------------------

int TestCase_BitBlt(HWND hWnd, HDC hMonitorDC, BOOL bClipping)
{
    TCHAR s[1000];
    HBRUSH hBrush = NULL;

    DWORD BitBltROPs[] =  { (DWORD) ( BLACKNESS ), (DWORD) ( DSTINVERT ),
        (DWORD) ( MERGECOPY ) , (DWORD) ( MERGEPAINT ),
        (DWORD) ( NOTSRCCOPY ), (DWORD) ( NOTSRCERASE ),
        (DWORD) ( SRCCOPY )   , (DWORD) ( SRCERASE ),
        (DWORD) ( SRCINVERT ) , (DWORD) ( SRCPAINT ),
        (DWORD) ( WHITENESS ) , (DWORD) ( SRCAND )
    };
    TCHAR* BitBltROPsName[] =  { TEXT("BLACKNESS"), TEXT("DSTINVERT"),
        TEXT("MERGECOPY") , TEXT("MERGEPAINT"),
        TEXT("NOTSRCCOPY"), TEXT("NOTSRCERASE"),
        TEXT("SRCCOPY")   , TEXT("SRCERASE"),
        TEXT("SRCINVERT") , TEXT("SRCPAINT"),
        TEXT("WHITENESS") , TEXT("SRCAND")
    };
    int NumofBitBltROPs = sizeof(BitBltROPs)/sizeof(DWORD);

    if(bClipping)
        g_pMyLog->StartTest(TEXT("BitBlt test with Clips"));
    else
        g_pMyLog->StartTest(TEXT("BitBlt test without Clips"));

    HDC hDC = GetDC(hWnd);
    if (hDC == NULL)
    {
        OutputDebugString(_T("DC is NULL\n"));
        g_pMyLog->Fail(TEXT("DC is NULL"));
        g_pMyLog->EndTest();
        return 1;
    }
    HDC hTempDC = NULL;
    HBITMAP hTempBitmap = NULL;

    CCompatibleDC TestDC(hWnd, hMonitorDC, g_bDeviceBitmap);

    if( !TestDC.IsValid() )
    {
        StringCchPrintf(s, 256, _T("The creation of CCompatibleDC failed\n"));
        OutputDebugString(s);
        g_pMyLog->Fail(s);
        g_pMyLog->EndTest();
        ReleaseDC(hWnd, hDC);
        return 1;
    }

    BOOL bUseDeviceBitmap = TestDC.IsUsingDeviceBitmap();
    if(!bUseDeviceBitmap && g_bDeviceBitmap)
    {
        StringCchPrintf(s, 256, _T("CreateBitmapFromDxSurface failed\n"));
        OutputDebugString(s);
        g_pMyLog->Warn(s);
    }

    RECT r;
    GetClientRect(hWnd, &r);
    UINT Width = (r.right - r.left);
    UINT Height = (r.bottom - r.top);
    UINT BitmapSizeX = Width/4;
    UINT BitmapSizeY = Height/4;

    if(BitmapSizeX <= 0 || BitmapSizeY <= 0)
    {
        OutputDebugString(_T("DC is with abnormal client area\n"));
        g_pMyLog->Fail(_T("DC is with abnormal client area"));
        goto exit;
    }

    hTempDC = CreateCompatibleDC(hDC);
    if (hTempDC == NULL)
    {
        OutputDebugString(_T("DC is NULL\n"));
        g_pMyLog->Fail(_T("DC is NULL"));
        goto exit;
    }

    hTempBitmap = CreateCompatibleBitmap (hDC, BitmapSizeX, BitmapSizeY);
    if (hTempBitmap == NULL)
    {
        OutputDebugString(_T("DC is NULL\n"));
        g_pMyLog->Fail(_T("DC is NULL"));
        goto exit;
    }

    hBrush = CreateSolidBrush(0x00800000);
    if (hBrush == NULL)
    {
        OutputDebugString(_T("hBrush is NULL\n"));
        g_pMyLog->Fail(_T("Brush is NULL"));
        goto exit;
    }
    HBRUSH hBrush1 = CreateSolidBrush(0x00008000);
    if (hBrush1 == NULL)
    {
        OutputDebugString(_T("hBrush is NULL\n"));
        g_pMyLog->Fail(_T("Brush is NULL"));
        goto exit;
    }
    {
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hTempDC, hTempBitmap);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hTempDC, hBrush);
        // Fill the background of the bitmap
        if (!BitBlt(hTempDC, 0, 0, BitmapSizeX, BitmapSizeY, NULL, 0, 0, PATCOPY))
        {
            DeleteObject(hBrush);
            hBrush = NULL;
            DeleteObject(hBrush1);
            hBrush1 = NULL;
            OutputDebugString(_T("BitBlt failed\n"));
            g_pMyLog->Fail(_T("BitBlt failed"));
            g_pMyLog->EndTest();
            goto exit;
        }
        SelectObject(hTempDC, hOldBrush);
        hOldBrush = (HBRUSH)SelectObject(hTempDC, hBrush1);
        BitBlt(hTempDC, BitmapSizeX / 4, BitmapSizeY / 4, 
            BitmapSizeX / 4, BitmapSizeY / 4, NULL, 0, 0, PATCOPY);
        BitBlt(hTempDC, BitmapSizeX / 2, BitmapSizeY / 2, 
            BitmapSizeX / 4, BitmapSizeY / 4, NULL, 0, 0, PATCOPY);
        SelectObject(hTempDC, hOldBrush);
    }
    DeleteObject(hBrush);
    hBrush = NULL;
    DeleteObject(hBrush1);
    hBrush1 = NULL;

    hBrush = CreateSolidBrush(0x00805060);
    if (hBrush == NULL)
    {
        OutputDebugString(_T("hBrush is NULL\n"));
        g_pMyLog->Fail(_T("Brush is NULL"));
        goto exit;
    }

    //Firstly render to hDC so that the redirection surface for this window DC is created before starting the test
    if( !TestDC.Initialize(hBrush) )
    {
        g_pMyLog->Fail(_T("Warning: the Init of CCompatibleDC failed"));
        goto exit;
    }

    BOOL bTestResult;
    int NumFailures = 0;
    int TestCaseNum = -1;
    for(int i=0; i<NumofBitBltROPs; i++)
    {
        bTestResult = FALSE;
        TestCaseNum++;
        if(g_iTestCase >= 0)
        {
            if(g_iTestCase != TestCaseNum)
            {
                continue;
            }
        }

        // Remove the clips and fill the background with the brown color
        if( !TestDC.SelectRgn(NULL) || !TestDC.Initialize(hBrush) )
        {
            g_pMyLog->Fail(_T("The SelectClipRgn or Init of CCompatibleDC failed"));
            goto exit;
        }

        if(bClipping)
        {
            HRGN hRgn = CreateRectRgn(r.left+(int)(0.1f*Width),r.top+(int)(0.1f*Height),
                r.right-(int)(0.1f*Width),r.bottom-(int)(0.1f*Height));
            if(NULL == hRgn)
            {
                g_pMyLog->Fail(TEXT("CreateRectRgn failed."));
                goto exit;
            }
            if(!TestDC.SelectRgn(hRgn))
            {
                g_pMyLog->Fail(TEXT("SelectClipRgn failed."));
                goto exit;
            }
            if(!TestDC.ExcludeRect(r.left+(int)(0.4f*Width),r.top+(int)(0.4f*Height),
                r.right-(int)(0.4f*Width),r.bottom-(int)(0.4f*Height)))
            {
                g_pMyLog->Fail(TEXT("ExcludeClipRgn failed."));
                goto exit;
            }
            DeleteObject(hRgn);
        }

        DWORD Rop = BitBltROPs[i];
        UINT x = GetRandomNumber((r.right - r.left)/2);
        float factor = (float)x / (float)(r.right - r.left);

        // Run test case
        BOOL result;
        UINT MaxIter = 1;
        if(g_bDebug)
            MaxIter = 3000;
        for(UINT iter = 0; iter < MaxIter; iter++) //for debug purpose 
        {
            result = TestDC.DoBitBlt(hTempDC, BitmapSizeX, BitmapSizeY, Rop, factor);
        }
        if( result )
        {
            StringCchPrintf(s, 256, TEXT("BitBlt-%s"), BitBltROPsName[i]);
            if( TestDC.CompareDC( (TCHAR*)s, g_bTolerance ) )
            {
                if(!bClipping)
                {
                    bTestResult = LogTestResults(s, "bitblt", &TestDC.ErrorInfo, TestCaseNum, FALSE);
                }
                else
                {
                    bTestResult = LogTestResults(s, "bitblt", &TestDC.ErrorInfo, TestCaseNum+1000, FALSE);
                }
                if(!bTestResult)
                {
                    StringCchCat(s, 256, TEXT(" failed"));
                    g_pMyLog->Fail(s);
                }
            }
            else
            {
                StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                OutputDebugString(s);
                g_pMyLog->Fail(s);
            }

            if(bUseDeviceBitmap)
            {
                StringCchPrintf(s, 256, TEXT("BitBlt-%s"), BitBltROPsName[i]);
                TestDC.SyncBitmapDC(TRUE);
                if( TestDC.CompareDC( (TCHAR*)s, g_bTolerance ) )
                {
                    if(!bClipping)
                    {
                        bTestResult = LogTestResults(s, "bitblt", &TestDC.ErrorInfo, TestCaseNum, TRUE);
                    }
                    else
                    {
                        bTestResult = LogTestResults(s, "bitblt", &TestDC.ErrorInfo, TestCaseNum+1000, TRUE);
                    }
                    if(!bTestResult)
                    {
                        StringCchCat(s, 256, TEXT("-devicebitmap failed"));
                        g_pMyLog->Fail(s);
                    }
                }
                else
                {
                    StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                    OutputDebugString(s);
                    g_pMyLog->Fail(s);
                }
            }
        }
        else
        {
            StringCchPrintf(s, 256, _T("The BitBlt in the compatible DC failed\n"));
            OutputDebugString(s);
            g_pMyLog->Fail(s);
        }
        if( !bTestResult )
        {
            NumFailures++;
        }
    }

exit:
    if(NULL != hBrush )
        DeleteObject(hBrush);
    g_pMyLog->EndTest();
    if(hTempBitmap)
        DeleteObject(hTempBitmap);
    if(hTempDC)
        DeleteDC(hTempDC);
    ReleaseDC(hWnd, hDC);

    return NumFailures;//wprintf(L"The test cases for BitBlt failed\n");
}
int TestCase_ColorFill(HWND hWnd, HDC hMonitorDC, BOOL bClipping)
{
    TCHAR s[1000];
    HBRUSH hBrush = NULL;
    HBRUSH hBrush1 = NULL;

    DWORD ColorFillRops[] = { (DWORD) ( PATCOPY )  ,
        (DWORD) ( PATINVERT ),
        (DWORD) ( PATINVERT )
    };
    TCHAR* ColorFillROPsName[] =  { TEXT("PATCOPY"), TEXT("PATINVERT"),
        TEXT("PATINVERT") 
    };

    int NumofColorFillROPs = sizeof(ColorFillRops)/sizeof(DWORD);

    if(bClipping)
        g_pMyLog->StartTest(TEXT("ColorFill test with Clips"));
    else
        g_pMyLog->StartTest(TEXT("ColorFill test without Clips"));

    HDC hDC = GetDC(hWnd);
    if (hDC == NULL)
    {
        OutputDebugString(_T("DC is NULL\n"));
        g_pMyLog->Fail(TEXT("DC is NULL"));
        g_pMyLog->EndTest();
        return 1;
    }

    CCompatibleDC TestDC(hWnd, hMonitorDC, g_bDeviceBitmap);

    if( !TestDC.IsValid() )
    {
        StringCchPrintf(s, 256, _T("The creation of CCompatibleDC failed\n"));
        OutputDebugString(s);
        g_pMyLog->Fail(s);
        g_pMyLog->EndTest();
        ReleaseDC(hWnd, hDC);
        return 1;
    }

    BOOL bUseDeviceBitmap = TestDC.IsUsingDeviceBitmap();
    if(!bUseDeviceBitmap && g_bDeviceBitmap)
    {
        StringCchPrintf(s, 256, _T("CreateBitmapFromDxSurface failed\n"));
        OutputDebugString(s);
        g_pMyLog->Warn(s);
    }

    RECT r;
    GetClientRect(hWnd, &r);
    UINT Width = (r.right - r.left);
    UINT Height = (r.bottom - r.top);
    UINT BitmapSizeX = Width/2;
    UINT BitmapSizeY = Height/2;

    if(BitmapSizeX <= 0 || BitmapSizeY <= 0)
    {
        OutputDebugString(_T("DC is with abnormal client area\n"));
        g_pMyLog->Fail(_T("DC is with abnormal client area"));
        goto exit;
    }

    hBrush = CreateSolidBrush(0x00805060);
    if (hBrush == NULL)
    {
        OutputDebugString(_T("hBrush is NULL\n"));
        g_pMyLog->Fail(TEXT("hBrush is NULL\n"));
        goto exit;
    }
    hBrush1 = CreateSolidBrush(0x00005000);
    if (hBrush1 == NULL)
    {
        OutputDebugString(_T("hBrush is NULL\n"));
        g_pMyLog->Fail(TEXT("hBrush is NULL\n"));
        goto exit;
    }

    BOOL bTestResult;
    int NumFailures = 0;
    int TestCaseNum = -1;
    for(int i=0; i<NumofColorFillROPs; i++)
    {
        bTestResult = FALSE;
        TestCaseNum++;
        if(g_iTestCase >= 0)
        {
            if(g_iTestCase != TestCaseNum)
                continue;
        }

        DWORD Rop = ColorFillRops[i];

        if( !TestDC.SelectRgn(NULL) )  //remove clip region firstly
        {
            OutputDebugString(_T("SelectClipRgn failed in TestDC\n"));
            g_pMyLog->Fail(TEXT("SelectClipRgn failed in TestDC\n"));
            goto exit;
        }
        // Fill the background with the brown color
        if( !TestDC.Initialize(hBrush) )
        {
            g_pMyLog->Fail(_T("The Init of CCompatibleDC failed"));
        }
        else
        {
            if(bClipping)
            {
                if( !TestDC.SelectRgn(NULL) )  //remove clip region firstly
                {
                    OutputDebugString(_T("SelectClipRgn failed in TestDC\n"));
                    g_pMyLog->Fail(TEXT("SelectClipRgn failed in TestDC\n"));
                    goto exit;
                }

                HRGN hRgn;
                hRgn = CreateRectRgn(0, 0, Width, Height);
                if(NULL == hRgn)
                {
                    OutputDebugString(_T("CreateRectRgn failed\n"));
                    g_pMyLog->Fail(TEXT("CreateRectRgn failed\n"));
                    goto exit;
                }
                if(!TestDC.SelectRgn(hRgn))
                {
                    OutputDebugString(_T("SelectClipRgn failed\n"));
                    g_pMyLog->Fail(TEXT("SelectClipRgn failed\n"));
                    goto exit;
                }
                for(int i = 0; i < (int)Width; i += 8)
                {
                    for(int j = 0; j < (int)Height; j += 8)
                    {
                        if(!TestDC.ExcludeRect(i, j, i+4, j+4))
                        {
                            OutputDebugString(_T("ExcludeClipRect failed\n"));
                            g_pMyLog->Fail(TEXT("ExcludeClipRect failed\n"));
                            goto exit;
                        }
                    }
                }
                DeleteObject(hRgn);
            }
            UINT x = GetRandomNumber((r.right - r.left)/2);
            float factor = (float)x / (float)(r.right - r.left);

            // Run test case
            TestDC.Select(hBrush1);
            BOOL result;
            UINT MaxIter = 1;
            if(g_bDebug)
                MaxIter = 3000;

            for(UINT iter = 0; iter < MaxIter; iter++) //for debug purpose 
            {
                result = TestDC.DoColorFill(BitmapSizeX, BitmapSizeY, Rop, factor);
            }
            if( result )
            {
                StringCchPrintf(s, 256, TEXT("ColorFill-%s"), ColorFillROPsName[i]);
                if( TestDC.CompareDC( (TCHAR*)s, g_bTolerance ) )
                {
                    if(!bClipping)
                    {
                        bTestResult = LogTestResults(s, "colorfill", &TestDC.ErrorInfo, TestCaseNum, FALSE);
                    }
                    else
                    {
                        bTestResult = LogTestResults(s, "colorfill", &TestDC.ErrorInfo, TestCaseNum+1000, FALSE);
                    }
                    if(!bTestResult)
                    {
                        StringCchCat(s, 256, TEXT(" failed"));
                        g_pMyLog->Fail(s);
                    }
                }
                else
                {
                    StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                    OutputDebugString(s);
                    g_pMyLog->Fail(s);
                }

                if(bUseDeviceBitmap)
                {
                    StringCchPrintf(s, 256, TEXT("ColorFill-%s"), ColorFillROPsName[i]);
                    TestDC.SyncBitmapDC(TRUE);
                    if( TestDC.CompareDC( (TCHAR*)s, g_bTolerance ) )
                    {
                        if(!bClipping)
                        {
                            bTestResult = LogTestResults(s, "colorfill", &TestDC.ErrorInfo, TestCaseNum, TRUE);
                        }
                        else
                        {
                            bTestResult = LogTestResults(s, "colorfill", &TestDC.ErrorInfo, TestCaseNum+1000, TRUE);
                        }
                        if(!bTestResult)
                        {
                            StringCchCat(s, 256, TEXT("-devicebitmap failed"));
                            g_pMyLog->Fail(s);
                        }
                    }
                    else
                    {
                        StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                        OutputDebugString(s);
                        g_pMyLog->Fail(s);
                    }
                }
            }
            else
            {
                StringCchPrintf(s, 256, _T("The ColorFill in the compatible DC failed\n"));
                OutputDebugString(s);
                g_pMyLog->Fail(s);
            }
            if( !bTestResult )
            {
                NumFailures++;
            }
        }
    }

exit:
    g_pMyLog->EndTest();
    if(hBrush)
        DeleteObject(hBrush);
    if(hBrush1)
        DeleteObject(hBrush1);
    ReleaseDC(hWnd, hDC);

    return NumFailures;//wprintf(L"The test for ColorFill failed\n");
}

int TestCase_AlphaBlend(HWND hWnd, HDC hMonitorDC, BOOL bClipping)
{
    TCHAR s[1000];
    if(bClipping)
        g_pMyLog->StartTest(TEXT("AlphaBlend Test with Clips"));
    else
        g_pMyLog->StartTest(TEXT("AlphaBlend Test without Clips"));

    HDC hDC = GetDC(hWnd);
    if (hDC == NULL)
    {
        OutputDebugString(_T("DC is NULL\n"));
        g_pMyLog->Fail(_T("DC is NULL"));
        g_pMyLog->EndTest();
        return 1;
    }
    HDC hTempDC = NULL;
    HBITMAP hTempBitmap = NULL;

    CCompatibleDC TestDC(hWnd, hMonitorDC, g_bDeviceBitmap);

    if( !TestDC.IsValid() )
    {
        StringCchPrintf(s, 256, _T("The creation of CCompatibleDC failed\n"));
        OutputDebugString(s);
        g_pMyLog->Fail(s);
        g_pMyLog->EndTest();
        ReleaseDC(hWnd, hDC);
        return 1;
    }

    BOOL bUseDeviceBitmap = TestDC.IsUsingDeviceBitmap();
    if(!bUseDeviceBitmap && g_bDeviceBitmap)
    {
        StringCchPrintf(s, 256, _T("CreateBitmapFromDxSurface failed\n"));
        OutputDebugString(s);
        g_pMyLog->Warn(s);
    }

    // Obtain the window's client rectangle 
    RECT r;
    GetClientRect(hWnd, &r); 
    UINT Width = (r.right - r.left);
    UINT Height = (r.bottom - r.top);

    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader); 
    bmi.bmiHeader.biWidth  = Width; 
    bmi.bmiHeader.biHeight = -(LONG)Height; 
    bmi.bmiHeader.biPlanes = 1; 
    bmi.bmiHeader.biBitCount = 32; 
    bmi.bmiHeader.biCompression = BI_RGB; 
    bmi.bmiHeader.biSizeImage = 0; 
    bmi.bmiHeader.biXPelsPerMeter = 96; 
    bmi.bmiHeader.biYPelsPerMeter = 96; 
    bmi.bmiHeader.biClrUsed = 0; 
    bmi.bmiHeader.biClrImportant = 0; 

    VOID* pBits;
    hTempBitmap = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    if (hTempBitmap == NULL)
    {
        OutputDebugString(_T("hTempBitmap is NULL\n"));
        g_pMyLog->Fail(_T("gTempBitmap is NULL"));
        goto exit;
    }

    //Fill the bitmap with foreground colors for AlphaBlend
    {
        DWORD* pColors = (DWORD*)pBits;
        BYTE*  pColor = (BYTE*)pBits;
        for (UINT i=0; i < Height; i++)
        {
            for (UINT j=0; j < Width; j++)
            {
                //put varying alpha values in pixels
                if (i >= 40 && i <= 60 &&
                    j >= 100 && j <= 140)
                {
                    *pColors++ = 0x0000FF00 | ((j % 255) << 24);
                }
                else if(i >= 140 && i <= 160
                    && j >= 200 && j <= 240)
                {
                    *pColors++ = 0x00805060 | ((j % 255) << 24);
                }
                else
                {
                    *pColors++ = 0x00080808 | ((j % 255) << 24);
                }
            }
        }
    }

    hTempDC = CreateCompatibleDC(hDC);
    if (hTempDC == NULL)
    {
        OutputDebugString(_T("DC is NULL\n"));
        g_pMyLog->Fail(_T("DC is NULL"));
        goto exit;
    }
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hTempDC, hTempBitmap);

    // Set Blending function
    BLENDFUNCTION Blend;
    {
        Blend.AlphaFormat = 0;
        Blend.BlendFlags = 0;
        Blend.BlendOp = AC_SRC_OVER;
        Blend.SourceConstantAlpha = 64;
    }

    //firstly initialize the background areas to be black, i.e. 0
    if( !TestDC.SelectRgn(NULL) )  //remove clip region
    {
        OutputDebugString(_T("SelectClipRgn failed in TestDC\n"));
        g_pMyLog->Fail(TEXT("SelectClipRgn failed in TestDC\n"));
        goto exit;
    }
    //Do a warm-up
    TestDC.DoAlphaBlend(hTempDC, Width, Height, Blend, FALSE);
    //Then fill the background with the black color
    {
        HBRUSH hBrush = CreateSolidBrush(0x00000000);
        if (hBrush == NULL)
        {
            OutputDebugString(_T("hBrush is NULL\n"));
            g_pMyLog->Fail(_T("Brush is NULL"));
            goto exit;
        }
        if( !TestDC.Initialize(hBrush) )
        {
            DeleteObject(hBrush);
            StringCchPrintf(s, 256, _T("The Initialization of Compatible DC failed\n"));
            g_pMyLog->Fail(s);
            goto exit;
        }
        DeleteObject(hBrush);
        hBrush = NULL;
    }

    if(bClipping)
    {
        HRGN hRgn = CreateRectRgn(r.left+(int)(0.25f*Width),r.top+(int)(0.2f*Height),
            r.right-(int)(0.25f*Width),r.bottom-(int)(0.2f*Height));
        if(NULL == hRgn)
        {
            g_pMyLog->Fail(TEXT("CreateRectRgn failed."));
            goto exit;
        }
        if(!TestDC.SelectRgn(hRgn))
        {
            g_pMyLog->Fail(TEXT("SelectClipRgn failed."));
            goto exit;
        }
        if(!TestDC.ExcludeRect(r.left+(int)(0.45f*Width),r.top+(int)(0.45f*Height),
            r.right-(int)(0.45f*Width),r.bottom-(int)(0.45f*Height)))
        {
            g_pMyLog->Fail(TEXT("ExcludeClipRgn failed."));
            goto exit;
        }
        DeleteObject(hRgn);
    }

    //then fill the brown color in clipped regions 
    {
        HBRUSH hBrush = CreateSolidBrush(0x12805060);
        if (hBrush == NULL)
        {
            OutputDebugString(_T("hBrush is NULL\n"));
            g_pMyLog->Fail(_T("Brush is NULL"));
            goto exit;
        }
        if( !TestDC.Initialize(hBrush) )
        {
            DeleteObject(hBrush);
            StringCchPrintf(s, 256, _T("The Initialization of Compatible DC failed\n"));
            g_pMyLog->Fail(s);
            goto exit;
        }
        DeleteObject(hBrush);
        hBrush = NULL;
    }
    // Run test case
    BOOL bTestResult;
    int NumFailures = 0;
    int TestCaseNum = -1;
    BOOL result;
    UINT MaxIter = 1;
    if(g_bDebug)
        MaxIter = 3000;

    bTestResult = FALSE;
    TestCaseNum++;
    if(g_iTestCase > 0)
    {
        goto case2;
    }

    for(UINT iter = 0; iter < MaxIter; iter++) //for debug purpose 
    {
        result = TestDC.DoAlphaBlend(hTempDC, Width, Height, Blend, FALSE);
    }
    if( result )
    {
        StringCchPrintf(s, 256, TEXT("AlphaBlend-Non-PerPixel"));
        if( TestDC.CompareDC( (TCHAR*)s, g_bTolerance ) )
        {
            if(!bClipping)
            {
                bTestResult = LogTestResults(s, "alphablend", &TestDC.ErrorInfo, TestCaseNum, FALSE);
            }
            else
            {
                bTestResult = LogTestResults(s, "alphablend", &TestDC.ErrorInfo, TestCaseNum+1000, FALSE);
            }
            if(!bTestResult)
            {
                StringCchCat(s, 256, TEXT(" failed"));
                g_pMyLog->Fail(s);
            }
        }
        else
        {
            StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
            OutputDebugString(s);
            g_pMyLog->Fail(s);
        }

        if(bUseDeviceBitmap)
        {
            StringCchPrintf(s, 256, TEXT("AlphaBlend-Non-PerPixel"));
            TestDC.SyncBitmapDC(TRUE);
            if( TestDC.CompareDC( (TCHAR*)s, g_bTolerance ) )
            {
                if(!bClipping)
                {
                    bTestResult = LogTestResults(s, "alphablend", &TestDC.ErrorInfo, TestCaseNum, TRUE);
                }
                else
                {
                    bTestResult = LogTestResults(s, "alphablend", &TestDC.ErrorInfo, TestCaseNum+1000, TRUE);
                }
                if(!bTestResult)
                {
                    StringCchCat(s, 256, TEXT("-devicebitmap failed"));
                    g_pMyLog->Fail(s);
                }
            }
            else
            {
                StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                OutputDebugString(s);
                g_pMyLog->Fail(s);
            }
        }
    }
    else
    {
        StringCchPrintf(s, 256, _T("The AlphaBlend in the compatible DC failed\n"));
        OutputDebugString(s);
        g_pMyLog->Fail(s);
    }
    if( !bTestResult )
    {
        NumFailures++;
    }
    if(g_iTestCase >= 0)
    {
        //just for repro case and thus stop running the left cases
        goto exit;
    }

case2:
    bTestResult = FALSE;
    TestCaseNum++;
    if(g_iTestCase > 1)
    {
        //it must be a reproed case for overflowed cases
        g_bRunAll = FALSE;
        goto case3;
    }

    //set params for Per-Pixel alphablend
    {
        Blend.AlphaFormat = AC_SRC_ALPHA;
        Blend.BlendFlags = 0;
        Blend.BlendOp = AC_SRC_OVER;
        Blend.SourceConstantAlpha = 200;
    }

    SelectObject(hTempDC, hOldBitmap);
    //update the values in the source bitmap with multiplying alpha value in each RBG channel
    {
        DWORD* pColors = (DWORD*)pBits;
        DWORD aColor, rColor, bColor, gColor;
        DWORD Value;
        float Alpha, fColor;
        for (UINT i=0; i < Height; i++)
        {
            for (UINT j=0; j < Width; j++)
            {
                Value = (*pColors & 0xFF000000);
                Value = Value >> 24; //Alpha cahnnel
                Alpha = ((float)(Value))/255.0f;

                bColor = (*pColors & 0x00FF0000) >> 16; //Blue channel
                fColor = ((float)bColor)*Alpha;
                bColor = (DWORD)fColor;

                gColor = (*pColors & 0x0000FF00) >> 8; //Green channel
                fColor = ((float)gColor)*Alpha;
                gColor = (DWORD)fColor;

                rColor = (*pColors & 0x000000FF); //Red channel
                fColor = ((float)rColor)*Alpha;
                rColor = (DWORD)fColor;

                Value = (((Value << 24) & 0xFF000000)|
                    ((bColor << 16) & 0x00FF0000)|
                    ((gColor << 8) & 0x0000FF00) |
                    (rColor & 0x000000FF));

                *pColors++ = Value;
            }
        }
    }
    SelectObject(hTempDC, hTempBitmap);

    // Fill the background with the brown color with destination alpha
    {
        HBRUSH hBrush = CreateSolidBrush(0x12805060);
        if (hBrush == NULL)
        {
            OutputDebugString(_T("hBrush is NULL\n"));
            g_pMyLog->Fail(_T("Brush is NULL"));
            goto exit;
        }
        if( !TestDC.Initialize(hBrush) )
        {
            DeleteObject(hBrush);
            StringCchPrintf(s, 256, _T("The Initialization of Compatible DC failed\n"));
            g_pMyLog->Fail(s);
            goto exit;
        }
        DeleteObject(hBrush);
        hBrush = NULL;
    }

    // Run test case
    for(UINT iter = 0; iter < MaxIter; iter++) //for debug purpose 
    {
        result = TestDC.DoAlphaBlend(hTempDC, Width, Height, Blend, FALSE);
    }
    if( result )
    {
        StringCchPrintf(s, 256, TEXT("AlphaBlend-PerPixel"));
        if( TestDC.CompareDC( (TCHAR*)s, g_bTolerance ) )
        {
            if(!bClipping)
            {
                bTestResult = LogTestResults(s, "alphablend", &TestDC.ErrorInfo, TestCaseNum, FALSE);
            }
            else
            {
                bTestResult = LogTestResults(s, "alphablend", &TestDC.ErrorInfo, TestCaseNum+1000, FALSE);
            }
            if(!bTestResult)
            {
                StringCchCat(s, 256, TEXT(" failed"));
                g_pMyLog->Fail(s);
            }
        }
        else
        {
            StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
            OutputDebugString(s);
            g_pMyLog->Fail(s);
        }

        if(bUseDeviceBitmap)
        {
            StringCchPrintf(s, 256, TEXT("AlphaBlend-PerPixel"));
            TestDC.SyncBitmapDC(TRUE);
            if( TestDC.CompareDC( (TCHAR*)s, g_bTolerance ) )
            {
                if(!bClipping)
                {
                    bTestResult = LogTestResults(s, "alphablend", &TestDC.ErrorInfo, TestCaseNum, TRUE);
                }
                else
                {
                    bTestResult = LogTestResults(s, "alphablend", &TestDC.ErrorInfo, TestCaseNum+1000, TRUE);
                }
                if(!bTestResult)
                {
                    StringCchCat(s, 256, TEXT("-devicebitmap failed"));
                    g_pMyLog->Fail(s);
                }
            }
            else
            {
                StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                OutputDebugString(s);
                g_pMyLog->Fail(s);
            }
        }
    }
    else
    {
        StringCchPrintf(s, 256, _T("The AlphaBlend in the compatible DC failed\n"));
        OutputDebugString(s);
        g_pMyLog->Fail(s);
    }
    if( !bTestResult )
    {
        NumFailures++;
    }
    if(g_iTestCase >= 0)
    {
        //just for repro case and thus stop running the left cases
        goto exit;
    }

case3:
    //Test cases for overflowed ones 
    if(!g_bRunAll) //Only apply for single test run so far
    {
        TestCaseNum++;
        if(g_iTestCase < 0 || TestCaseNum == g_iTestCase)
        {
            SelectObject(hTempDC, hOldBitmap);
            //fill the source bitmap with color values that could incur overflow
            DWORD* pColors = (DWORD*)pBits;
            for (UINT i=0; i < Height; i++)
            {
                for (UINT j=0; j < Width; j++)
                {
                    //put max alpha values in pixels
                    if (i >= 40 && i <= 60 &&
                        j >= 100 && j <= 104)
                    {
                        *pColors++ = 0x8800FF00;
                    }
                    else if(i >= 140 && i <= 144
                        && j >= 233 && j <= 237)
                    {
                        *pColors++ = 0x88FF0000;
                    }
                    else
                    {
                        *pColors++ = 0x88080808;
                    }
                }
            }
            SelectObject(hTempDC, hTempBitmap);

            //set params for Per-Pixel alphablend
            {
                Blend.AlphaFormat = AC_SRC_ALPHA;
                Blend.BlendFlags = 0;
                Blend.BlendOp = AC_SRC_OVER;
                Blend.SourceConstantAlpha = 255;
            }

            // Run test case
            bTestResult = FALSE;
            result = TRUE;
            for(UINT iter = 0; iter < MaxIter; iter++) //for debug purpose 
            {
                // Fill the background with blue color and destination alpha
                {
                    HBRUSH hBrush = CreateSolidBrush(0x88880808);
                    if (hBrush == NULL)
                    {
                        OutputDebugString(_T("hBrush is NULL\n"));
                        g_pMyLog->Fail(_T("Brush is NULL"));
                        goto exit;
                    }
                    if( !TestDC.Initialize(hBrush) )
                    {
                        DeleteObject(hBrush);
                        StringCchPrintf(s, 256, _T("The Initialization of Compatible DC failed\n"));
                        g_pMyLog->Fail(s);
                        goto exit;
                    }
                    DeleteObject(hBrush);
                    hBrush = NULL;
                }
                result &= TestDC.DoAlphaBlend(hTempDC, Width, Height, Blend, TRUE);
            }
            if( result )
            {
                StringCchPrintf(s, 256, TEXT("AlphaBlend-PerPixel-Overflow"));
                if( TestDC.CompareDC( (TCHAR*)s, g_bTolerance ) )
                {
                    if(!bClipping)
                    {
                        bTestResult = LogTestResults(s, "alphablend", &TestDC.ErrorInfo, TestCaseNum, FALSE);
                    }
                    else
                    {
                        bTestResult = LogTestResults(s, "alphablend", &TestDC.ErrorInfo, TestCaseNum+1000, FALSE);
                    }
                    if(!bTestResult)
                    {
                        StringCchCat(s, 256, TEXT(" failed"));
                        g_pMyLog->Fail(s);
                    }
                }
                else
                {
                    StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                    OutputDebugString(s);
                    g_pMyLog->Fail(s);
                }

                if(bUseDeviceBitmap)
                {
                    StringCchPrintf(s, 256, TEXT("AlphaBlend-PerPixel-Overflow"));
                    TestDC.SyncBitmapDC(TRUE);
                    if( TestDC.CompareDC( (TCHAR*)s, g_bTolerance ) )
                    {
                        if(!bClipping)
                        {
                            bTestResult = LogTestResults(s, "alphablend", &TestDC.ErrorInfo, TestCaseNum, TRUE);
                        }
                        else
                        {
                            bTestResult = LogTestResults(s, "alphablend", &TestDC.ErrorInfo, TestCaseNum+1000, TRUE);
                        }
                        if(!bTestResult)
                        {
                            StringCchCat(s, 256, TEXT("-devicebitmap failed"));
                            g_pMyLog->Fail(s);
                        }
                    }
                    else
                    {
                        StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                        OutputDebugString(s);
                        g_pMyLog->Fail(s);
                    }
                }
            }
            else
            {
                StringCchPrintf(s, 256, _T("The AlphaBlend in the compatible DC failed\n"));
                OutputDebugString(s);
                g_pMyLog->Fail(s);
            }
            if( !bTestResult )
            {
                NumFailures++;
            }
            if(g_iTestCase >= 0)
            {
                //just for repro case and thus stop running the left case
                goto exit;
            }
        }

        //for another case with source, destination and constand alpha
        bTestResult = FALSE;
        TestCaseNum++;
        if(g_iTestCase >= 0 && g_iTestCase != TestCaseNum)
        {
            //the repro case number does not match, so exit this test
            if(bClipping)
            {
                g_iTestCase += 1000;
            }
            StringCchPrintf(s, 256, _T("The repro case, %d, for AlphaBlend is not existing\n"), g_iTestCase);
            OutputDebugString(s);
            g_pMyLog->Warn(s);
            goto exit;
        }

        //set params for Per-Pixel alphablend
        {
            Blend.AlphaFormat = AC_SRC_ALPHA;
            Blend.BlendFlags = 0;
            Blend.BlendOp = AC_SRC_OVER;
            Blend.SourceConstantAlpha = 250;
        }

        // Run test case
        result = TRUE;
        for(UINT iter = 0; iter < MaxIter; iter++) //for debug purpose 
        {
            // Fill the background with the green color and destination alpha
            {
                HBRUSH hBrush = CreateSolidBrush(0x88088808);
                if (hBrush == NULL)
                {
                    OutputDebugString(_T("hBrush is NULL\n"));
                    g_pMyLog->Fail(_T("Brush is NULL"));
                    goto exit;
                }
                if( !TestDC.Initialize(hBrush) )
                {
                    DeleteObject(hBrush);
                    StringCchPrintf(s, 256, _T("The Initialization of Compatible DC failed\n"));
                    g_pMyLog->Fail(s);
                    goto exit;
                }
                DeleteObject(hBrush);
                hBrush = NULL;
            }
            result &= TestDC.DoAlphaBlend(hTempDC, Width, Height, Blend, TRUE);
        }
        if( result )
        {
            StringCchPrintf(s, 256, TEXT("AlphaBlend-PerPixel-Constant-Overflow"));
            if( TestDC.CompareDC( (TCHAR*)s, g_bTolerance ) )
            {
                if(!bClipping)
                {
                    bTestResult = LogTestResults(s, "alphablend", &TestDC.ErrorInfo, TestCaseNum, FALSE);
                }
                else
                {
                    bTestResult = LogTestResults(s, "alphablend", &TestDC.ErrorInfo, TestCaseNum+1000, FALSE);
                }
                if(!bTestResult)
                {
                    StringCchCat(s, 256, TEXT(" failed"));
                    g_pMyLog->Fail(s);
                }
            }
            else
            {
                StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                OutputDebugString(s);
                g_pMyLog->Fail(s);
            }

            if(bUseDeviceBitmap)
            {
                StringCchPrintf(s, 256, TEXT("AlphaBlend-PerPixel-Constant-Overflow"));
                TestDC.SyncBitmapDC(TRUE);
                if( TestDC.CompareDC( (TCHAR*)s, g_bTolerance ) )
                {
                    if(!bClipping)
                    {
                        bTestResult = LogTestResults(s, "alphablend", &TestDC.ErrorInfo, TestCaseNum, TRUE);
                    }
                    else
                    {
                        bTestResult = LogTestResults(s, "alphablend", &TestDC.ErrorInfo, TestCaseNum+1000, TRUE);
                    }
                    if(!bTestResult)
                    {
                        StringCchCat(s, 256, TEXT("-devicebitmap failed"));
                        g_pMyLog->Fail(s);
                    }
                }
                else
                {
                    StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                    OutputDebugString(s);
                    g_pMyLog->Fail(s);
                }
            }
        }
        else
        {
            StringCchPrintf(s, 256, _T("The AlphaBlend in the compatible DC failed\n"));
            OutputDebugString(s);
            g_pMyLog->Fail(s);
        }
        if( !bTestResult )
        {
            NumFailures++;
        }
    }

exit:
    g_pMyLog->EndTest();
    if(hTempBitmap)
        DeleteObject(hTempBitmap);
    if(hTempDC)
        DeleteDC(hTempDC);
    ReleaseDC(hWnd, hDC);

    return NumFailures;//wprintf(L"The test for Alphablend failed\n");
}

int TestCase_TransparentBlt(HWND hWnd, HDC hMonitorDC, BOOL bClipping)
{
    TCHAR s[1000];
    HBRUSH hBrush = NULL;

    if(bClipping)
        g_pMyLog->StartTest(TEXT("TransparentBlt Test with Clips"));
    else
        g_pMyLog->StartTest(TEXT("TransparentBlt Test"));

    HDC hDC = GetDC(hWnd);
    if (hDC == NULL)
    {
        OutputDebugString(_T("DC is NULL\n"));
        g_pMyLog->Fail(_T("DC is NULL"));
        g_pMyLog->EndTest();
        return 1;
    }
    HDC hTempDC = NULL;
    HBITMAP hTempBitmap = NULL;
    UINT ColorKey = RGB(0, 0, 0xFF);

    CCompatibleDC TestDC(hWnd, hMonitorDC, g_bDeviceBitmap);

    if( !TestDC.IsValid() )
    {
        StringCchPrintf(s, 256, _T("The creation of CCompatibleDC failed\n"));
        OutputDebugString(s);
        g_pMyLog->Fail(s);
        g_pMyLog->EndTest();
        return 1;
    }

    BOOL bUseDeviceBitmap = TestDC.IsUsingDeviceBitmap();
    if(!bUseDeviceBitmap && g_bDeviceBitmap)
    {
        StringCchPrintf(s, 256, _T("CreateBitmapFromDxSurface failed\n"));
        OutputDebugString(s);
        g_pMyLog->Warn(s);
    }

    // Obtain the window's client rectangle 
    RECT r;
    GetClientRect(hWnd, &r); 
    UINT Width = (r.right - r.left);
    UINT Height = (r.bottom - r.top);

    // Create a DC to work with bitmap
    //
    hTempDC = CreateCompatibleDC(hDC);
    if (hTempDC == NULL)
    {
        OutputDebugString(_T("DC is NULL\n"));
        g_pMyLog->Fail(_T("DC is NULL"));
        goto exit;
    }
    // Create a bitmap with source color key
    //
    hTempBitmap = CreateCompatibleBitmap (hDC, Width, Height);
    if (hTempBitmap == NULL)
    {
        OutputDebugString(_T("DC is NULL\n"));
        g_pMyLog->Fail(_T("DC is NULL"));
        goto exit;
    }
    SelectObject(hTempDC, hTempBitmap);
    {
        // Fill the bitmap with GREEN color
        //
        HBRUSH hBrush = CreateSolidBrush(RGB(0, 0xFF, 0));
        if (hBrush == NULL)
        {
            OutputDebugString(_T("hBrush is NULL\n"));
            g_pMyLog->Fail(_T("Brush is NULL"));
            goto exit;
        }
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hTempDC, hBrush);
        if (!BitBlt(hTempDC, 0, 0, Width, Height, NULL, 0, 0, PATCOPY))
        {
            OutputDebugString(_T("BitBlt failed\n"));
            g_pMyLog->Fail(_T("BitBlt failed"));
            DeleteObject(hBrush);
            goto exit;
        }
        SelectObject(hTempDC, hOldBrush);
        DeleteObject(hBrush);
        hBrush = NULL;
    }
    {
        // Fill the area of bitmap with color key color
        //
        HBRUSH hBrush = CreateSolidBrush(ColorKey);
        if (hBrush == NULL)
        {
            OutputDebugString(_T("hBrush is NULL\n"));
            g_pMyLog->Fail(_T("Brush is NULL"));
            goto exit;
        }
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hTempDC, hBrush);
        if (!BitBlt(hTempDC, 10, 20, 50, 40, NULL, 0, 0, PATCOPY))
        {
            OutputDebugString(_T("BitBlt failed\n"));
            g_pMyLog->Fail(_T("BitBlt failed"));
            DeleteObject(hBrush);
            goto exit;
        }
        if (!BitBlt(hTempDC, 110, 120, 50, 40, NULL, 0, 0, PATCOPY))
        {
            OutputDebugString(_T("BitBlt failed\n"));
            g_pMyLog->Fail(_T("BitBlt failed"));
            DeleteObject(hBrush);
            goto exit;
        }
        if (!BitBlt(hTempDC, 210, 220, 50, 40, NULL, 0, 0, PATCOPY))
        {
            OutputDebugString(_T("BitBlt failed\n"));
            g_pMyLog->Fail(_T("BitBlt failed"));
            DeleteObject(hBrush);
            goto exit;
        }
        SelectObject(hTempDC, hOldBrush);
        DeleteObject(hBrush);
        hBrush = NULL;
    }

    hBrush = CreateSolidBrush(RGB(0, 0, 0xFF));
    if (hBrush == NULL)
    {
        OutputDebugString(_T("hBrush is NULL\n"));
        g_pMyLog->Fail(_T("Brush is NULL"));
        goto exit;
    }

    // Fill the window background with blue color.
    if( !TestDC.Initialize(hBrush) )
    {
        StringCchPrintf(s, 256, _T("The Initialization of CCompatibleDC failed\n"));
        OutputDebugString(s);
        g_pMyLog->Fail(s);
        DeleteObject(hBrush);
        goto exit;
    }
    DeleteObject(hBrush);
    hBrush = NULL;

    if(bClipping)
    {
        HRGN hRgn = CreateRectRgn(r.left+(int)(0.2f*Width),r.top+(int)(0.2f*Height),
            r.right-(int)(0.2f*Width),r.bottom-(int)(0.2f*Height));
        if(NULL == hRgn)
        {
            g_pMyLog->Fail(TEXT("CreateRectRgn failed."));
            goto exit;
        }
        if(!TestDC.SelectRgn(hRgn))
        {
            g_pMyLog->Fail(TEXT("SelectClipRgn failed."));
            goto exit;
        }
        if(!TestDC.ExcludeRect(r.left+(int)(0.4f*Width),r.top+(int)(0.4f*Height),
            r.right-(int)(0.4f*Width),r.bottom-(int)(0.4f*Height)))
        {
            g_pMyLog->Fail(TEXT("ExcludeClipRgn failed."));
            goto exit;
        }
        DeleteObject(hRgn);
    }

    // Run test case
    BOOL bTestResult;
    int NumFailures = 0;
    int TestCaseNum = -1;
    BOOL result;
    UINT MaxIter = 1;
    if(g_bDebug)
        MaxIter = 3000;

    bTestResult = FALSE;
    TestCaseNum++;
    if(g_iTestCase >= 0)
    {
        if(g_iTestCase != TestCaseNum)
            goto exit;
    }

    for(UINT iter = 0; iter < MaxIter; iter++) //for debug purpose 
    {
        result = TestDC.DoTransparentBlt(hTempDC, Width, Height, ColorKey);
    }
    if( result )
    {
        StringCchPrintf(s, 256, TEXT("TransparentBlt"));
        if( TestDC.CompareDC( (TCHAR*)s, g_bTolerance ) )
        {
            if(!bClipping)
            {
                bTestResult = LogTestResults(s, "transparentblt", &TestDC.ErrorInfo, TestCaseNum, FALSE);
            }
            else
            {
                bTestResult = LogTestResults(s, "transparentblt", &TestDC.ErrorInfo, TestCaseNum+1000, FALSE);
            }
            if(!bTestResult)
            {
                StringCchCat(s, 256, TEXT(" failed"));
                g_pMyLog->Fail(s);
            }
        }
        else
        {
            StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
            OutputDebugString(s);
            g_pMyLog->Fail(s);
        }

        if(bUseDeviceBitmap)
        {
            StringCchPrintf(s, 256, TEXT("TransparentBlt"));
            TestDC.SyncBitmapDC(TRUE);
            if( TestDC.CompareDC( (TCHAR*)s, g_bTolerance ) )
            {
                if(!bClipping)
                {
                    bTestResult = LogTestResults(s, "transparentblt", &TestDC.ErrorInfo, TestCaseNum, TRUE);
                }
                else
                {
                    bTestResult = LogTestResults(s, "transparentblt", &TestDC.ErrorInfo, TestCaseNum+1000, TRUE);
                }
                if(!bTestResult)
                {
                    StringCchCat(s, 256, TEXT("-devicebitmap failed"));
                    g_pMyLog->Fail(s);
                }
            }
            else
            {
                StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                OutputDebugString(s);
                g_pMyLog->Fail(s);
            }
        }
    }
    else
    {
        StringCchPrintf(s, 256, _T("The TransparentBlt in the compatible DC failed\n"));
        OutputDebugString(s);
        g_pMyLog->Fail(s);
    }
    if( !bTestResult )
    {
        NumFailures++;
    }

exit:
    if( NULL != hBrush )
        DeleteObject(hBrush);
    g_pMyLog->EndTest();
    if(hTempBitmap)
        DeleteObject(hTempBitmap);
    if(hTempDC)
        DeleteDC(hTempDC);
    ReleaseDC(hWnd, hDC);

    return NumFailures;//wprintf(L"The test for TransparentBlt failed\n");
}

int TestCase_StretchBlt(HWND hWnd, HDC hMonitorDC, BOOL bClipping)
{
    TCHAR s[1000];
    HDC hScreenDC = NULL;
    HBRUSH hBrush = NULL;

    if(bClipping)
        g_pMyLog->StartTest(TEXT("StretchBlt Test with Clips"));
    else
        g_pMyLog->StartTest(TEXT("StretchBlt Test without Clips"));

    //the tolerance only applies for all cases (single -StretchBlt) run
    BOOL bTolerance = FALSE;
    if(g_bTolerance) // && !g_bRunAll)
    {
        bTolerance = TRUE;
    }

    HDC hDC = GetDC(hWnd);
    if (hDC == NULL)
    {
        OutputDebugString(_T("DC is NULL\n"));
        g_pMyLog->Fail(_T("DC is NULL"));
        g_pMyLog->EndTest();
        return 1;
    }
    HDC hTempDC = NULL;
    HBITMAP hTempBitmap = NULL;

    CCompatibleDC TestDC(hWnd, hMonitorDC, g_bDeviceBitmap);

    if( !TestDC.IsValid() )
    {
        StringCchPrintf(s, 256, _T("The creation of CCompatibleDC failed\n"));
        OutputDebugString(s);
        g_pMyLog->Fail(s);
        g_pMyLog->EndTest();
        goto exit;
    }

    BOOL bUseDeviceBitmap = TestDC.IsUsingDeviceBitmap();
    if(!bUseDeviceBitmap && g_bDeviceBitmap)
    {
        StringCchPrintf(s, 256, _T("CreateBitmapFromDxSurface failed\n"));
        OutputDebugString(s);
        g_pMyLog->Warn(s);
    }

    // Obtain the window's client rectangle 
    RECT r;
    GetClientRect(hWnd, &r); 
    UINT Width = (r.right - r.left);
    UINT Height = (r.bottom - r.top);

    HWND hScreenWnd = GetDesktopWindow();
    hScreenDC = GetDC(NULL);
    if (hScreenDC == NULL)
    {
        OutputDebugString(_T("DC is NULL\n"));
        g_pMyLog->Fail(_T("DC is NULL"));
        goto exit;
    }
    RECT ScreenRect;
    if( !GetWindowRect(hScreenWnd, &ScreenRect) )
    {
        OutputDebugString(_T("GetWindowRect failed\n"));
        g_pMyLog->Fail(_T("GetWindowRect failed"));
        goto exit;
    }
    UINT ScreenWidth = ScreenRect.right - ScreenRect.left;
    UINT ScreenHeight = ScreenRect.bottom - ScreenRect.top;

    // BLT desktop to the window

    //	SetStretchBltMode(hDC, HALFTONE);
    //	SetStretchBltMode(hScreenDC, HALFTONE);

    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader); 
    bmi.bmiHeader.biWidth  = Width; 
    bmi.bmiHeader.biHeight = Height; 
    bmi.bmiHeader.biPlanes = 1; 
    bmi.bmiHeader.biBitCount = 32; 
    bmi.bmiHeader.biCompression = BI_RGB; 
    bmi.bmiHeader.biSizeImage = 0; 
    bmi.bmiHeader.biXPelsPerMeter = 96; 
    bmi.bmiHeader.biYPelsPerMeter = 96; 
    bmi.bmiHeader.biClrUsed = 0; 
    bmi.bmiHeader.biClrImportant = 0; 

    VOID* pBits;
    hTempBitmap = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    if (hTempBitmap == NULL)
    {
        OutputDebugString(_T("hMemoryBitmap is NULL\n"));
        g_pMyLog->Fail(_T("MemoryBitmap is NULL"));
        goto exit;
    }

    DWORD* pColors = (DWORD*)pBits;
    BYTE*  pColor = (BYTE*)pBits;
    for (UINT i=0; i < Height; i++)
    {
        for (UINT j=0; j < Width; j++)
        {
            if (bmi.bmiHeader.biBitCount == 24)
            {
                *pColor++ = 0x00;
                *pColor++ = 0xFF;
                *pColor++ = 0xFF;
            }
            else
            {
                *pColors++ = 0xFF00FFFF;
            }
        }
    }

    hTempDC = CreateCompatibleDC(hDC);
    if (hTempDC == NULL)
    {
        OutputDebugString(_T("DC is NULL\n"));
        g_pMyLog->Fail(_T("DC is NULL"));
        goto exit;
    }

    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hTempDC, hTempBitmap);
    {
        // Fill some areas in bitmap with brown color
        //
        HBRUSH hBrush = CreateSolidBrush(0x00805060);
        if (hBrush == NULL)
        {
            OutputDebugString(_T("hBrush is NULL\n"));
            g_pMyLog->Fail(_T("Brush is NULL"));
            goto exit;
        }
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hTempDC, hBrush);

        if (!BitBlt(hTempDC, 10, 20, 80, 60, NULL, 0, 0, PATCOPY))
        {
            OutputDebugString(_T("BitBlt failed\n"));
            g_pMyLog->Fail(_T("BitBlt failed"));
            DeleteObject(hBrush);
            goto exit;
        }
        if (!BitBlt(hTempDC, 120, 130, 80, 60, NULL, 0, 0, PATCOPY))
        {
            OutputDebugString(_T("BitBlt failed\n"));
            g_pMyLog->Fail(_T("BitBlt failed"));
            DeleteObject(hBrush);
            goto exit;
        }
        SelectObject(hTempDC, hOldBrush);
        DeleteObject(hBrush);
        hBrush = NULL;
    }
    //use stretchblt to copy the source of screenDC to hTempDC
    if(g_bDebug) //not used and it can be added for specific option (8 == g_Option)
    {
        int result = StretchBlt(hTempDC,         // handle to destination DC
            0,           // x-coord of destination upper-left corner
            0,           // y-coord of destination upper-left corner
            Width,       // width of destination rectangle
            Height,      // height of destination rectangle
            hScreenDC,   // handle to source DC
            0,           // x-coord of source upper-left corner
            0,           // y-coord of source upper-left corner
            ScreenWidth, // width of source rectangle
            ScreenHeight,// height of source rectangle
            SRCCOPY      // raster operation code
            );
        if(!result)
        {
            OutputDebugString(_T("StretchBlt failed\n"));
            g_pMyLog->Fail(_T("StretchBlt failed"));
            goto exit;
        }
    }
    //the other option for source is using window DC
    //when there is already content on window DC
    if(0)//not used
    {
        int result = BitBlt(hTempDC,         // handle to destination DC
            0,           // x-coord of destination upper-left corner
            0,           // y-coord of destination upper-left corner
            Width,       // width of destination rectangle
            Height,      // height of destination rectangle
            hDC,   // handle to source DC
            0,           // x-coord of source upper-left corner
            0,           // y-coord of source upper-left corner
            SRCCOPY      // raster operation code
            );
        if(!result)
        {
            OutputDebugString(_T("BitBlt failed\n"));
            g_pMyLog->Fail(_T("BitBlt failed"));
            goto exit;
        }
    }

    hBrush = CreateSolidBrush(RGB(0, 0, 0xFF));
    if (hBrush == NULL)
    {
        OutputDebugString(_T("hBrush is NULL\n"));
        g_pMyLog->Fail(_T("Brush is NULL"));
        goto exit;
    }

    // Fill the window background with blue color.
    if( !TestDC.Initialize(hBrush) )
    {
        StringCchPrintf(s, 256, _T("The Initialization of CCompatibleDC failed\n"));
        OutputDebugString(s);
        g_pMyLog->Fail(s);
        goto exit;
    }

    UINT x = GetRandomNumber(r.right - r.left);
    float factor = (float)x / (float)(r.right - r.left);
    UINT MaxIter = 1;
    if(g_bDebug)
        MaxIter = 3000;

    BOOL result;
    if(bClipping)
    {
        HRGN hRgn = CreateRectRgn(r.left+(int)(0.2f*Width),r.top+(int)(0.2f*Height),
            r.right-(int)(0.2f*Width),r.bottom-(int)(0.2f*Height));
        if(NULL == hRgn)
        {
            g_pMyLog->Fail(TEXT("CreateRectRgn failed."));
            goto exit;
        }
        if(!TestDC.SelectRgn(hRgn))
        {
            g_pMyLog->Fail(TEXT("SelectClipRgn failed."));
            goto exit;
        }
        if(!TestDC.ExcludeRect(r.left+(int)(0.4f*Width),r.top+(int)(0.4f*Height),
            r.right-(int)(0.4f*Width),r.bottom-(int)(0.4f*Height)))
        {
            g_pMyLog->Fail(TEXT("ExcludeClipRgn failed."));
            goto exit;
        }
        DeleteObject(hRgn);
    }

    BOOL bTestResult;
    int NumFailures = 0;
    int TestCaseNum = -1;

    //StretchBlt for zoom-in
    for(int k = 1; k < 11; k++)
    {
        bTestResult = FALSE;
        TestCaseNum++;
        if(g_iTestCase >= 0)
        {
            if(g_iTestCase != TestCaseNum)
                continue;
        }
        // Fill the window background with blue color.
        if( !TestDC.Initialize(hBrush) )
        {
            StringCchPrintf(s, 256, _T("The Initialization of CCompatibleDC failed\n"));
            OutputDebugString(s);
            g_pMyLog->Fail(s);
            goto exit;
        }
        factor = 0.1f*(float)k;

        // Fill the background with the brown color
        for(UINT iter = 0; iter < MaxIter; iter++) //for debug purpose 
        {
            result = TestDC.DoStretchBlt(hTempDC, Width, Height, factor);
        }
        if( result )
        {
            StringCchPrintf(s, 256, TEXT("StretchBlt-Shrink-%f"), factor);
            if( TestDC.CompareDC( (TCHAR*)s, bTolerance ) )
            {
                if(!bClipping)
                {
                    bTestResult = LogTestResults(s, "stretchblt", &TestDC.ErrorInfo, TestCaseNum, FALSE);
                }
                else
                {
                    bTestResult = LogTestResults(s, "stretchblt", &TestDC.ErrorInfo, TestCaseNum+1000, FALSE);
                }
                if(!bTestResult)
                {
                    StringCchCat(s, 256, TEXT(" failed"));
                    g_pMyLog->Fail(s);
                }
            }
            else
            {
                StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                OutputDebugString(s);
                g_pMyLog->Fail(s);
            }

            if(bUseDeviceBitmap)
            {
                StringCchPrintf(s, 256, TEXT("StretchBlt-Shrink-%f"), factor);
                TestDC.SyncBitmapDC(TRUE);
                if( TestDC.CompareDC( (TCHAR*)s, bTolerance ) )
                {
                    if(!bClipping)
                    {
                        bTestResult = LogTestResults(s, "stretchblt", &TestDC.ErrorInfo, TestCaseNum, TRUE);
                    }
                    else
                    {
                        bTestResult = LogTestResults(s, "stretchblt", &TestDC.ErrorInfo, TestCaseNum+1000, TRUE);
                    }
                    if(!bTestResult)
                    {
                        StringCchCat(s, 256, TEXT("-devicebitmap failed"));
                        g_pMyLog->Fail(s);
                    }
                }
                else
                {
                    StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                    OutputDebugString(s);
                    g_pMyLog->Fail(s);
                }
            }
        }
        else
        {
            StringCchPrintf(s, 256, _T("The StretchBlt in the compatible DC failed\n"));
            OutputDebugString(s);
            g_pMyLog->Fail(s);
        }
        if( !bTestResult )
        {
            NumFailures++;
        }
    }

    x = GetRandomNumber(r.right - r.left);
    factor = 1.0f + (float)x / (float)(r.right - r.left);

    //StretchBlt for zoom-out
    for(int k = 2; k < 10; k += 2) //No need for many stretch ratios
    {
        bTestResult = FALSE;
        TestCaseNum++;
        if(g_iTestCase >= 0)
        {
            if(g_iTestCase != TestCaseNum)
                continue;
        }
        // Fill the window background with blue color.
        if( !TestDC.Initialize(hBrush) )
        {
            StringCchPrintf(s, 256, _T("The Initialization of CCompatibleDC failed\n"));
            OutputDebugString(s);
            g_pMyLog->Fail(s);
            DeleteObject(hBrush);
            goto exit;
        }

        factor = 1.0f+ 0.1f*(float)k;

        for(UINT iter = 0; iter < MaxIter; iter++) //for debug purpose 
        {
            result = TestDC.DoStretchBlt(hTempDC, Width, Height, factor);
        }
        if( result )
        {
            StringCchPrintf(s, 256, TEXT("StretchBlt-Stretch-%f"), factor);
            if( TestDC.CompareDC( (TCHAR*)s, bTolerance ) )
            {
                if(!bClipping)
                {
                    bTestResult = LogTestResults(s, "stretchblt", &TestDC.ErrorInfo, TestCaseNum, FALSE);
                }
                else
                {
                    bTestResult = LogTestResults(s, "stretchblt", &TestDC.ErrorInfo, TestCaseNum+1000, FALSE);
                }
                if(!bTestResult)
                {
                    StringCchCat(s, 256, TEXT(" failed"));
                    g_pMyLog->Fail(s);
                }
            }
            else
            {
                StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                OutputDebugString(s);
                g_pMyLog->Fail(s);
            }

            if(bUseDeviceBitmap)
            {
                StringCchPrintf(s, 256, TEXT("StretchBlt-Stretch-%f"), factor);
                TestDC.SyncBitmapDC(TRUE);
                if( TestDC.CompareDC( (TCHAR*)s, bTolerance ) )
                {
                    if(!bClipping)
                    {
                        bTestResult = LogTestResults(s, "stretchblt", &TestDC.ErrorInfo, TestCaseNum, TRUE);
                    }
                    else
                    {
                        bTestResult = LogTestResults(s, "stretchblt", &TestDC.ErrorInfo, TestCaseNum+1000, TRUE);
                    }
                    if(!bTestResult)
                    {
                        StringCchCat(s, 256, TEXT("-devicebitmap failed"));
                        g_pMyLog->Fail(s);
                    }
                }
                else
                {
                    StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                    OutputDebugString(s);
                    g_pMyLog->Fail(s);
                }
            }
        }
        else
        {
            StringCchPrintf(s, 256, _T("The StretchBlt in the compatible DC failed\n"));
            OutputDebugString(s);
            g_pMyLog->Fail(s);
        }
        if( !bTestResult )
        {
            NumFailures++;
        }
    }

    //Mirror StretchBlt for zoom-out and In
    for(int k = 1; k < 12; k++) 
    {
        bTestResult = FALSE;
        TestCaseNum++;
        if(g_iTestCase >= 0)
        {
            if(g_iTestCase != TestCaseNum)
                continue;
        }
        // Fill the window background with blue color.
        if( !TestDC.Initialize(hBrush) )
        {
            StringCchPrintf(s, 256, _T("The Initialization of CCompatibleDC failed\n"));
            OutputDebugString(s);
            g_pMyLog->Fail(s);
            DeleteObject(hBrush);
            goto exit;
        }

        factor = 0.1f*(float)k - 1.6f;

        for(UINT iter = 0; iter < MaxIter; iter++) //for debug purpose 
        {
            result = TestDC.DoStretchBlt(hTempDC, Width, Height, factor);
        }
        if( result )
        {
            StringCchPrintf(s, 256, TEXT("StretchBlt-Mirror-%f"), factor);
            if( TestDC.CompareDC( (TCHAR*)s, bTolerance ) )
            {
                if(!bClipping)
                {
                    bTestResult = LogTestResults(s, "stretchblt", &TestDC.ErrorInfo, TestCaseNum, FALSE);
                }
                else
                {
                    bTestResult = LogTestResults(s, "stretchblt", &TestDC.ErrorInfo, TestCaseNum+1000, FALSE);
                }
                if(!bTestResult)
                {
                    StringCchCat(s, 256, TEXT(" failed"));
                    g_pMyLog->Fail(s);
                }
            }
            else
            {
                StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                OutputDebugString(s);
                g_pMyLog->Fail(s);
            }

            if(bUseDeviceBitmap)
            {
                StringCchPrintf(s, 256, TEXT("StretchBlt-Mirror-%f"), factor);
                TestDC.SyncBitmapDC(TRUE);
                if( TestDC.CompareDC( (TCHAR*)s, bTolerance ) )
                {
                    if(!bClipping)
                    {
                        bTestResult = LogTestResults(s, "stretchblt", &TestDC.ErrorInfo, TestCaseNum, TRUE);
                    }
                    else
                    {
                        bTestResult = LogTestResults(s, "stretchblt", &TestDC.ErrorInfo, TestCaseNum+1000, TRUE);
                    }
                    if(!bTestResult)
                    {
                        StringCchCat(s, 256, TEXT("-devicebitmap failed"));
                        g_pMyLog->Fail(s);
                    }
                }
                else
                {
                    StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                    OutputDebugString(s);
                    g_pMyLog->Fail(s);
                }
            }
        }
        else
        {
            StringCchPrintf(s, 256, _T("The StretchBlt in the compatible DC failed\n"));
            OutputDebugString(s);
            g_pMyLog->Fail(s);
        }
        if( !bTestResult )
        {
            NumFailures++;
        }
    }

exit:
    g_pMyLog->EndTest();
    if( NULL != hBrush )
        DeleteObject(hBrush);
    if(hTempBitmap)
        DeleteObject(hTempBitmap);
    if(hTempDC)
        DeleteDC(hTempDC);
    if(hScreenDC)
        ReleaseDC(NULL, hScreenDC);
    ReleaseDC(hWnd, hDC);

    return NumFailures;//wprintf(L"The test for StretchBlt failed\n");
}

int TestCase_Trans_Textout_ClearType(HWND hWnd, HDC hMonitorDC)
{
    TCHAR s[1000];
    BOOL result;
    COLORREF TextCol;

    g_pMyLog->StartTest(TEXT("Transparent-Textout-Cleartype Test"));

    CCompatibleDC TestDC(hWnd, hMonitorDC, g_bDeviceBitmap);

    if( !TestDC.IsValid() )
    {
        StringCchPrintf(s, 256, _T("The creation of CCompatibleDC failed\n"));
        OutputDebugString(s);
        g_pMyLog->Fail(s);
        g_pMyLog->EndTest();
        return 1;
    }

    BOOL bUseDeviceBitmap = TestDC.IsUsingDeviceBitmap();
    if(!bUseDeviceBitmap && g_bDeviceBitmap)
    {
        StringCchPrintf(s, 256, _T("CreateBitmapFromDxSurface failed\n"));
        OutputDebugString(s);
        g_pMyLog->Warn(s);
    }

    HFONT fClearType = CreateFont(-20, 0, 0, 0, FW_NORMAL, 0, 0, 0,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH, TEXT("Tahoma"));
    HFONT OldFont = (HFONT)TestDC.Select((HGDIOBJ)fClearType);

    //Set the template memory for indicating interior pixels in cleartype textout
    {
        HBRUSH hBrush;
        TestDC.SetBKMode(TRANSPARENT);

        //Only use the case with black background and white text color
        hBrush = CreateSolidBrush(0x00000000);
        if (hBrush == NULL)
        {
            OutputDebugString(_T("hBrush is NULL\n"));
            g_pMyLog->Fail(_T("Brush is NULL"));
            g_pMyLog->EndTest();
            return 1;
        }
        TestDC.SetTextCol(RGB(255,255,255));
        TestDC.SetBKCol(RGB(0,0,0));
        COLORREF TextCol = RGB(255,255,255);
        if( !TestDC.Initialize(hBrush) )
        {
            StringCchPrintf(s, 256, _T("The Initialization of Compatible DC failed\n"));
            g_pMyLog->Fail(s);
            g_pMyLog->EndTest();
            DeleteObject(hBrush);
            return 1;
        }
        TestDC.DoTextout();
        DeleteObject(hBrush);
        hBrush = NULL;

        if( !TestDC.GetInteriorPixels(TextCol) )
        {
            StringCchPrintf(s, 256, _T("The method for retriving interior pixels failed\n"));
            g_pMyLog->Fail(s);
            g_pMyLog->EndTest();
            DeleteObject(hBrush);
            return 1;
        }
    }
    //set the system parameters for cleartype text
    DWORD SystemParam;
    BOOL bSetSystemParam = FALSE;
    if(!SystemParametersInfo(SPI_GETFONTSMOOTHINGCONTRAST, 0, &SystemParam, 0))
    {
        SystemParam = 1200; //default one.
        OutputDebugString(_T("Get system parameter info failed\n"));
        g_pMyLog->Warn(_T("Get system parameter info failed"));
    }
    else
    {
        StringCchPrintf(s, 256, TEXT("The original value of FONTSMOOTHINGCONTRAST is %d\n"), SystemParam);
        g_pMyLog->Message(s);
    }
    SystemParametersInfo(SPI_SETFONTSMOOTHING,TRUE, 0,
        SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
    SystemParametersInfo(SPI_SETFONTSMOOTHINGTYPE,
        FE_FONTSMOOTHINGCLEARTYPE, 0,
        SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);

    BOOL bTestResult = FALSE;
    int NumFailures = 0, TestCaseNum = -1;
    UINT MaxIter = 1;
    UINT mGama = 2200, nGama = 0;
    if(g_bDebug && !g_bRunAll)
    {
        mGama = 0;
        nGama = 2200;
    }
    UINT iTol = 0;
    if(g_bTolerance)
    {
        iTol = 4;//set bigger tolerance for applying Gamma
    }
    if(g_iTolerance > 0)
    {
        iTol = g_iTolerance;
    }

    for(DWORD iGama = 1000; iGama <= mGama; iGama+=100)
    {
        if(g_bTolerance && iGama >= 2200 && g_iTolerance < 0)
        {
            iTol = 16;
        }
        //Set BGR in FONTSMOOTHINGORIENTATION
        if(!SystemParametersInfo(SPI_SETFONTSMOOTHINGORIENTATION, 0, (DWORD*)FE_FONTSMOOTHINGORIENTATIONBGR, 0))
        {
            StringCchPrintf(s, 256, TEXT("Set SPI_SETFONTSMOOTHINGORIENTATION to BGR failed\n"));
            g_pMyLog->Message(s);
        }
        //Set the FONTSMOOTHINGCONTRAST to different values
        if(!SystemParametersInfo(SPI_SETFONTSMOOTHINGCONTRAST, 0, (DWORD*)iGama, 0))
        {
            StringCchPrintf(s, 256, TEXT("Set FONTSMOOTHINGCONTRST to %d failed\n"), iGama);
            g_pMyLog->Message(s);
        }

        if(g_iTolerance > 0)
        {
            TestDC.SetMaxErrorValue( g_iTolerance );
        }
        else if (g_bTolerance)
        {
            TestDC.SetMaxErrorValue( 4 );
        }
        else
        {
            TestDC.SetMaxErrorValue( 0 );
        }
        //run with different text colors
        for(UINT iCol = 4; iCol < 276; iCol += 10)
        {
            bTestResult = FALSE;
            TestCaseNum++;
            if(g_iTestCase >= 0)
            {
                if(g_iTestCase != TestCaseNum)
                    continue;
            }

            HBRUSH hBrush;
            TestDC.SetBKMode(TRANSPARENT);

            if(iCol > 265)
            {
                //for the case with black background and grey text color
                hBrush = CreateSolidBrush(0x00000000);
                if (hBrush == NULL)
                {
                    OutputDebugString(_T("hBrush is NULL\n"));
                    g_pMyLog->Fail(_T("Brush is NULL"));
                    g_pMyLog->EndTest();
                    return 1;
                }
                TestDC.SetTextCol(RGB(255,255,255));
                TestDC.SetBKCol(RGB(0,0,0));
                TextCol = RGB(255,255,255);
            }
            else if(iCol > 255)
            {
                //for the case with white background and black text color
                hBrush = CreateSolidBrush(0x00FFFFFF);
                if (hBrush == NULL)
                {
                    OutputDebugString(_T("hBrush is NULL\n"));
                    g_pMyLog->Fail(_T("Brush is NULL"));
                    g_pMyLog->EndTest();
                    return 1;
                }
                TestDC.SetTextCol(RGB(0,0,0));
                TestDC.SetBKCol(RGB(255,255,255));
                TextCol = RGB(0,0,0);
            }
            else
            {
                //for other cases
                hBrush = CreateSolidBrush(0x00805060);
                if (hBrush == NULL)
                {
                    OutputDebugString(_T("hBrush is NULL\n"));
                    g_pMyLog->Fail(_T("Brush is NULL"));
                    g_pMyLog->EndTest();
                    return 1;
                }
                TestDC.SetTextCol(RGB(iCol,128,128));
                TestDC.SetBKCol(RGB(128,128,128));
                TextCol = RGB(iCol,128,128);
            }
            // run the test case
            for(UINT iter = 0; iter < MaxIter; iter++) //for debug purpose
            {
                if( !TestDC.Initialize(hBrush) )
                {
                    StringCchPrintf(s, 256, _T("The Initialization of Compatible DC failed\n"));
                    g_pMyLog->Fail(s);
                    g_pMyLog->EndTest();
                    DeleteObject(hBrush);
                    return 1;
                }

                result = TestDC.DoTextout();
            }
            DeleteObject(hBrush);
            hBrush = NULL;

            if( result )
            {
                StringCchPrintf(s, 256, TEXT("Transparent-ClearTypeTextout-BGR-%d"), iGama);
                if( TestDC.CompareDC( (TCHAR*)s , g_bTolerance, TextCol, iTol ) )
                {
                    bTestResult = LogTestResults(s, "textoutcleartypet", &TestDC.ErrorInfo, TestCaseNum, FALSE);
                    if(!bTestResult)
                    {
                        StringCchCat(s, 256, TEXT(" failed"));
                        g_pMyLog->Fail(s);
                    }
                    if(TestDC.ErrorInfo.NumofInteriorColorError > 0)
                    {
                        bTestResult = FALSE;
                        StringCchPrintf(s, 1000, _T("Failed: The test for Transparent-ClearTypeTextout-BGR-%d failed with mismatched interior pixels\n"), iGama);
                        g_pMyLog->Fail(s);
                        StringCchPrintf(s, 256, TEXT("Repro: Gditest_hw.exe -tolerance -textoutcleartypet %d\n"), TestCaseNum);
                        g_pMyLog->Message(s);
                        StringCchPrintf(s, 1000, _T("Total Number of mis-matched interior pixels: %d; the max error in a color channel for all interior pixels: %d\n"), 
                            TestDC.ErrorInfo.NumofInteriorColorError, TestDC.ErrorInfo.MaxInteriorError);
                        g_pMyLog->Message(s);
                    }
                    else
                    {
                        StringCchPrintf(s, 1000, _T("Pass: No mismatched interior pixels for Transparent-ClearTypeTextout-BGR-%d, passed\n"), iGama);
                        g_pMyLog->Message(s);
                    }
                    StringCchPrintf(s, 1000, _T("Total Number of pixels with text color of RGB(%d, %d, %d): %d\n"), 
                        GetRValue(TextCol), GetGValue(TextCol), GetBValue(TextCol), TestDC.ErrorInfo.NumofPixelwithTextCol);
                    g_pMyLog->Message(s);
                }
                else
                {
                    StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                    OutputDebugString(s);
                    g_pMyLog->Fail(s);
                }

                if(bUseDeviceBitmap)
                {
                    StringCchPrintf(s, 256, TEXT("Transparent-ClearTypeTextout-BGR-%d"), iGama);
                    TestDC.SyncBitmapDC(TRUE);
                    if( TestDC.CompareDC( (TCHAR*)s , g_bTolerance, TextCol, iTol ) )
                    {
                        bTestResult = LogTestResults(s, "textoutcleartypet", &TestDC.ErrorInfo, TestCaseNum, TRUE);
                        if(!bTestResult)
                        {
                            StringCchCat(s, 256, TEXT("-devicebitmap failed"));
                            g_pMyLog->Fail(s);
                        }
                        if(TestDC.ErrorInfo.NumofInteriorColorError > 0)
                        {
                            bTestResult = FALSE;
                            StringCchPrintf(s, 1000, _T("Failed: The test for Transparent-DeviceBitmap-ClearTypeTextout-BGR-%d failed with mismatched interior pixels\n"), iGama);
                            g_pMyLog->Fail(s);
                            StringCchPrintf(s, 256, TEXT("Repro: Gditest_hw.exe -tolerance -devicebitmap -textoutcleartypet %d\n"), TestCaseNum);
                            g_pMyLog->Message(s);
                            StringCchPrintf(s, 1000, _T("Total Number of mis-matched interior pixels: %d; the max error in a color color channel for all interior pixels: %d\n"), 
                                TestDC.ErrorInfo.NumofInteriorColorError, TestDC.ErrorInfo.MaxInteriorError);
                            g_pMyLog->Message(s);
                        }
                        else
                        {
                            StringCchPrintf(s, 1000, _T("PASS: No mismatched interior pixels for Transparent-DeviceBitmap-ClearTypeTextout-BGR-%d, passed\n"), iGama);
                            g_pMyLog->Message(s);
                        }
                    }
                    else
                    {
                        StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                        OutputDebugString(s);
                        g_pMyLog->Fail(s);
                    }
                }
            }
            else
            {
                StringCchPrintf(s, 256, _T("The Textout in the compatible DC failed\n"));
                OutputDebugString(s);
                g_pMyLog->Fail(s);
            }
            if( !bTestResult )
            {
                NumFailures++;
            }
        }

        //Set RGB in FONTSMOOTHINGORIENTATION
        if(!SystemParametersInfo(SPI_SETFONTSMOOTHINGORIENTATION, 0, (DWORD*)FE_FONTSMOOTHINGORIENTATIONRGB, 0))
        {
            StringCchPrintf(s, 256, TEXT("Set SPI_SETFONTSMOOTHINGORIENTATION to RGB failed\n"));
            g_pMyLog->Message(s);
        }
        //Set the FONTSMOOTHINGCONTRAST to different values
        if(!SystemParametersInfo(SPI_SETFONTSMOOTHINGCONTRAST, 0, (DWORD*)iGama, 0))
        {
            StringCchPrintf(s, 256, TEXT("Set FONTSMOOTHINGCONTRST to %d failed\n"), iGama);
            g_pMyLog->Message(s);
        }

        if(g_iTolerance > 0)
        {
            TestDC.SetMaxErrorValue( g_iTolerance );
        }
        else if( g_bTolerance )
        {
            TestDC.SetMaxErrorValue( 4 );
        }
        else
        {
            TestDC.SetMaxErrorValue( 0 );
        }
        for(UINT iCol = 4; iCol < 276; iCol += 10)
        {
            bTestResult = FALSE;
            TestCaseNum++;
            if(g_iTestCase >= 0)
            {
                if(g_iTestCase != TestCaseNum)
                    continue;
            }

            HBRUSH hBrush;
            TestDC.SetBKMode(TRANSPARENT);

            if(iCol > 265)
            {
                //for the case with black background and grey text color
                hBrush = CreateSolidBrush(0x00000000);
                if (hBrush == NULL)
                {
                    OutputDebugString(_T("hBrush is NULL\n"));
                    g_pMyLog->Fail(_T("Brush is NULL"));
                    g_pMyLog->EndTest();
                    return 1;
                }
                TestDC.SetTextCol(RGB(255,255,255));
                TestDC.SetBKCol(RGB(0,0,0));
                TextCol = RGB(255,255,255);
            }
            else if(iCol > 255)
            {
                //for the case with white background and black text color
                hBrush = CreateSolidBrush(0x00FFFFFF);
                if (hBrush == NULL)
                {
                    OutputDebugString(_T("hBrush is NULL\n"));
                    g_pMyLog->Fail(_T("Brush is NULL"));
                    g_pMyLog->EndTest();
                    return 1;
                }
                TestDC.SetTextCol(RGB(0,0,0));
                TestDC.SetBKCol(RGB(255,255,255));
                TextCol = RGB(0,0,0);
            }
            else
            {
                //for other cases
                hBrush = CreateSolidBrush(0x00805060);
                if (hBrush == NULL)
                {
                    OutputDebugString(_T("hBrush is NULL\n"));
                    g_pMyLog->Fail(_T("Brush is NULL"));
                    g_pMyLog->EndTest();
                    return 1;
                }
                TestDC.SetTextCol(RGB(iCol,128,128));
                TestDC.SetBKCol(RGB(128,128,128));
                TextCol = RGB(iCol,128,128);
            }
            // run the test case
            for(UINT iter = 0; iter < MaxIter; iter++) //for debug purpose
            {
                if( !TestDC.Initialize(hBrush) )
                {
                    StringCchPrintf(s, 256, _T("The Initialization of Compatible DC failed\n"));
                    g_pMyLog->Fail(s);
                    g_pMyLog->EndTest();
                    DeleteObject(hBrush);
                    return 1;
                }

                result = TestDC.DoTextout();
            }
            DeleteObject(hBrush);
            hBrush = NULL;

            if( result )
            {
                StringCchPrintf(s, 256, TEXT("Transparent-ClearTypeTextout-RGB-%d"), iGama);
                if( TestDC.CompareDC( (TCHAR*)s , g_bTolerance, TextCol, iTol ) )
                {
                    bTestResult = LogTestResults(s, "textoutcleartypet", &TestDC.ErrorInfo, TestCaseNum, FALSE);
                    if(!bTestResult)
                    {
                        StringCchCat(s, 256, TEXT(" failed"));
                        g_pMyLog->Fail(s);
                    }
                    if(TestDC.ErrorInfo.NumofInteriorColorError > 0)
                    {
                        bTestResult = FALSE;
                        StringCchPrintf(s, 1000, _T("Warn: The test for Transparent-ClearTypeTextout-RGB-%d failed with mismatched interior pixels\n"), iGama);
                        g_pMyLog->Fail(s);
                        StringCchPrintf(s, 256, TEXT("Repro: Gditest_hw.exe -tolerance -textoutcleartypet %d\n"), TestCaseNum);
                        g_pMyLog->Message(s);
                        StringCchPrintf(s, 1000, _T("Total Number of mis-matched interior pixels: %d; the max error in a color channel for all interior pixels: %d\n"), 
                            TestDC.ErrorInfo.NumofInteriorColorError, TestDC.ErrorInfo.MaxInteriorError);
                        g_pMyLog->Message(s);
                    }
                    else
                    {
                        StringCchPrintf(s, 1000, _T("Pass: No mismatched interior pixels for Transparent-ClearTypeTextout-RGB-%d, passed\n"), iGama);
                        g_pMyLog->Message(s);
                    }
                    StringCchPrintf(s, 1000, _T("Total Number of pixels with text color of RGB(%d, %d, %d): %d\n"), 
                        GetRValue(TextCol), GetGValue(TextCol), GetBValue(TextCol), TestDC.ErrorInfo.NumofPixelwithTextCol);
                    g_pMyLog->Message(s);
                }
                else
                {
                    StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                    OutputDebugString(s);
                    g_pMyLog->Fail(s);
                }

                if(bUseDeviceBitmap)
                {
                    StringCchPrintf(s, 256, TEXT("Transparent-ClearTypeTextout-RGB-%d"), iGama);
                    TestDC.SyncBitmapDC(TRUE);
                    if( TestDC.CompareDC( (TCHAR*)s , g_bTolerance, TextCol, iTol ) )
                    {
                        bTestResult = LogTestResults(s, "textoutcleartypet", &TestDC.ErrorInfo, 10000, TRUE);
                        if(!bTestResult)
                        {
                            StringCchCat(s, 256, TEXT("-devicebitmap failed"));
                            g_pMyLog->Fail(s);
                        }
                        if(TestDC.ErrorInfo.NumofInteriorColorError > 0)
                        {
                            bTestResult = FALSE;
                            StringCchPrintf(s, 1000, _T("Warn: The test for Transparent-DeviceBitmap-ClearTypeTextout-RGB-%d failed with mismatched interior pixels\n"), iGama);
                            g_pMyLog->Fail(s);
                            StringCchPrintf(s, 256, TEXT("Repro: Gditest_hw.exe -tolerance -devicebitmap -textoutcleartypet %d\n"), TestCaseNum);
                            g_pMyLog->Message(s);
                            StringCchPrintf(s, 1000, _T("Total Number of mis-matched interior pixels: %d; the max error in a color color channel for all interior pixels: %d\n"), 
                                TestDC.ErrorInfo.NumofInteriorColorError, TestDC.ErrorInfo.MaxInteriorError);
                            g_pMyLog->Message(s);
                        }
                        else
                        {
                            StringCchPrintf(s, 1000, _T("PASS: No mismatched interior pixels for Transparent-DeviceBitmap-ClearTypeTextout-RGB-%d, passed\n"), iGama);
                            g_pMyLog->Message(s);
                        }
                    }
                    else
                    {
                        StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                        OutputDebugString(s);
                        g_pMyLog->Fail(s);
                    }
                }
            }
            else
            {
                StringCchPrintf(s, 256, _T("The Textout in the compatible DC failed\n"));
                OutputDebugString(s);
                g_pMyLog->Fail(s);
            }
            if( !bTestResult )
            {
                NumFailures++;
            }
        }
    }

    //These cases are not used in most applications,
    //so are not counted in and logged as warnings.
    int MaxEdgeError, MaxInternalError;
    UINT ERvalue = 0, EGvalue = 0, EBvalue = 0;
    UINT IRvalue = 0, IGvalue = 0, IBvalue = 0;
    int MaxValue;
    iTol = 4;
    if(g_iTolerance > 0)
    {
        iTol = g_iTolerance;
    }
    for(DWORD iGama = 1000; iGama <= nGama; iGama+=100)
    {
        MaxEdgeError = MaxInternalError = 0;
        MaxValue = 0;

        //Set the FONTSMOOTHINGCONTRAST to different values
        if(!SystemParametersInfo(SPI_SETFONTSMOOTHINGCONTRAST, 0, (DWORD*)iGama, 0))
        {
            StringCchPrintf(s, 256, TEXT("Set FONTSMOOTHINGCONTRST to %d failed\n"), iGama);
            g_pMyLog->Message(s);
        }

        //Set BGR in FONTSMOOTHINGORIENTATION
        if(!SystemParametersInfo(SPI_SETFONTSMOOTHINGORIENTATION, 0, (DWORD*)FE_FONTSMOOTHINGORIENTATIONBGR, 0))
        {
            StringCchPrintf(s, 256, TEXT("Set SPI_SETFONTSMOOTHINGORIENTATION to BGR failed\n"));
            g_pMyLog->Message(s);
        }
        for(UINT iCol = 0; iCol < 256; iCol+=3)
        {
            // run the test case
            for(UINT iSue = 0; iSue < 256; iSue+=4)
            {
                HBRUSH hBrush = CreateSolidBrush(0x00ffffff);
                if (hBrush == NULL)
                {
                    OutputDebugString(_T("hBrush is NULL\n"));
                    g_pMyLog->Fail(_T("Brush is NULL"));
                    g_pMyLog->EndTest();
                    return 1;
                }
                if( !TestDC.Initialize(hBrush) )
                {
                    StringCchPrintf(s, 256, _T("The Initialization of Compatible DC failed\n"));
                    g_pMyLog->Fail(s);
                    g_pMyLog->EndTest();
                    DeleteObject(hBrush);
                    return 1;
                }
                DeleteObject(hBrush);
                hBrush = NULL;

                TestDC.SetTextCol(RGB(iCol,iSue,255));
                TestDC.SetBKCol(RGB(128,228,128));
                TestDC.SetBKMode(TRANSPARENT);
                TextCol = RGB(iCol,iSue,255);

                result = TestDC.DoTextout();

                if( result )
                {
                    StringCchPrintf(s, 256, TEXT("Transparent-ClearTypeTextout-%d"), iGama);
                    if( TestDC.CompareDC( (TCHAR*)s , g_bTolerance, TextCol, iTol ) )
                    {
                        if(TestDC.ErrorInfo.NumofColorError > 0)
                        {
                            MaxValue = max(TestDC.ErrorInfo.MaxRedError, TestDC.ErrorInfo.MaxGreenError);
                            MaxValue = max(TestDC.ErrorInfo.MaxBlueError, MaxValue);
                            if(MaxValue > MaxEdgeError)
                            {
                                MaxEdgeError = MaxValue;
                                ERvalue = iCol;
                                EGvalue = iSue;
                                EBvalue = 255;
                            }
                        }
                        if(TestDC.ErrorInfo.NumofInteriorColorError > 0)
                        {
                            if(TestDC.ErrorInfo.MaxInteriorError > MaxInternalError)
                            {
                                MaxInternalError = TestDC.ErrorInfo.MaxInteriorError;
                                IRvalue = iCol;
                                IGvalue = iSue;
                                IBvalue = 255;
                            }
                        }
                    }
                    else
                    {
                        StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                        OutputDebugString(s);
                        //g_pMyLog->Fail(s);
                    }
                }
            }
        }
        for(UINT iCol = 0; iCol < 256; iCol+=3)
        {
            // run the test case
            for(UINT iSue = 0; iSue < 256; iSue+=4)
            {
                HBRUSH hBrush = CreateSolidBrush(0x00ffffff);
                if (hBrush == NULL)
                {
                    OutputDebugString(_T("hBrush is NULL\n"));
                    g_pMyLog->Fail(_T("Brush is NULL"));
                    g_pMyLog->EndTest();
                    return 1;
                }
                if( !TestDC.Initialize(hBrush) )
                {
                    StringCchPrintf(s, 256, _T("The Initialization of Compatible DC failed\n"));
                    g_pMyLog->Fail(s);
                    g_pMyLog->EndTest();
                    DeleteObject(hBrush);
                    return 1;
                }
                DeleteObject(hBrush);
                hBrush = NULL;

                TestDC.SetTextCol(RGB(255,iCol,iSue));
                TestDC.SetBKCol(RGB(128,228,128));
                TestDC.SetBKMode(TRANSPARENT);
                TextCol = RGB(255,iCol,iSue);

                result = TestDC.DoTextout();

                if( result )
                {
                    StringCchPrintf(s, 256, TEXT("Transparent-ClearTypeTextout-%d"), iGama);
                    if( TestDC.CompareDC( (TCHAR*)s , g_bTolerance, TextCol, iTol ) )
                    {
                        if(TestDC.ErrorInfo.NumofColorError > 0)
                        {
                            MaxValue = max(TestDC.ErrorInfo.MaxRedError, TestDC.ErrorInfo.MaxGreenError);
                            MaxValue = max(TestDC.ErrorInfo.MaxBlueError, MaxValue);
                            if(MaxValue > MaxEdgeError)
                            {
                                MaxEdgeError = MaxValue;
                                EGvalue = iCol;
                                ERvalue = 255;
                                EBvalue = iSue;
                            }
                        }
                        if(TestDC.ErrorInfo.NumofInteriorColorError > 0)
                        {
                            if(TestDC.ErrorInfo.MaxInteriorError > MaxInternalError)
                            {
                                MaxInternalError = TestDC.ErrorInfo.MaxInteriorError;
                                IGvalue = iCol;
                                IRvalue = 255;
                                IBvalue = iSue;
                            }
                        }
                    }
                    else
                    {
                        StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                        OutputDebugString(s);
                        //g_pMyLog->Fail(s);
                    }
                }
            }
        }
        for(UINT iCol = 0; iCol < 256; iCol+=3)
        {
            // run the test case
            for(UINT iSue = 0; iSue < 256; iSue+=4)
            {
                HBRUSH hBrush = CreateSolidBrush(0x00ffffff);
                if (hBrush == NULL)
                {
                    OutputDebugString(_T("hBrush is NULL\n"));
                    g_pMyLog->Fail(_T("Brush is NULL"));
                    g_pMyLog->EndTest();
                    return 1;
                }
                if( !TestDC.Initialize(hBrush) )
                {
                    StringCchPrintf(s, 256, _T("The Initialization of Compatible DC failed\n"));
                    g_pMyLog->Fail(s);
                    g_pMyLog->EndTest();
                    DeleteObject(hBrush);
                    return 1;
                }
                DeleteObject(hBrush);
                hBrush = NULL;

                TestDC.SetTextCol(RGB(iSue,255,iCol));
                TestDC.SetBKCol(RGB(128,228,128));
                TestDC.SetBKMode(TRANSPARENT);
                TextCol = RGB(iSue,255,iCol);

                result = TestDC.DoTextout();

                if( result )
                {
                    StringCchPrintf(s, 256, TEXT("Transparent-ClearTypeTextout-%d"), iGama);
                    if( TestDC.CompareDC( (TCHAR*)s , g_bTolerance, TextCol, iTol ) )
                    {
                        if(TestDC.ErrorInfo.NumofColorError > 0)
                        {
                            MaxValue = max(TestDC.ErrorInfo.MaxRedError, TestDC.ErrorInfo.MaxGreenError);
                            MaxValue = max(TestDC.ErrorInfo.MaxBlueError, MaxValue);
                            if(MaxValue > MaxEdgeError)
                            {
                                MaxEdgeError = MaxValue;
                                EBvalue = iCol;
                                EGvalue = 255;
                                ERvalue = iSue;
                            }
                        }
                        if(TestDC.ErrorInfo.NumofInteriorColorError > 0)
                        {
                            if(TestDC.ErrorInfo.MaxInteriorError > MaxInternalError)
                            {
                                MaxInternalError = TestDC.ErrorInfo.MaxInteriorError;
                                IBvalue = iCol;
                                IGvalue = 255;
                                IRvalue = iSue;
                            }
                        }
                    }
                    else
                    {
                        StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                        OutputDebugString(s);
                        //g_pMyLog->Fail(s);
                    }
                }
            }
        }

        //Set RGB in FONTSMOOTHINGORIENTATION
        if(!SystemParametersInfo(SPI_SETFONTSMOOTHINGORIENTATION, 0, (DWORD*)FE_FONTSMOOTHINGORIENTATIONRGB, 0))
        {
            StringCchPrintf(s, 256, TEXT("Set SPI_SETFONTSMOOTHINGORIENTATION to BGR failed\n"));
            g_pMyLog->Message(s);
        }
        for(UINT iCol = 0; iCol < 256; iCol+=3)
        {
            // run the test case
            for(UINT iSue = 0; iSue < 256; iSue+=4)
            {
                HBRUSH hBrush = CreateSolidBrush(0x00ffffff);
                if (hBrush == NULL)
                {
                    OutputDebugString(_T("hBrush is NULL\n"));
                    g_pMyLog->Fail(_T("Brush is NULL"));
                    g_pMyLog->EndTest();
                    return 1;
                }
                if( !TestDC.Initialize(hBrush) )
                {
                    StringCchPrintf(s, 256, _T("The Initialization of Compatible DC failed\n"));
                    g_pMyLog->Fail(s);
                    g_pMyLog->EndTest();
                    DeleteObject(hBrush);
                    return 1;
                }
                DeleteObject(hBrush);
                hBrush = NULL;

                TestDC.SetTextCol(RGB(iCol,iSue,255));
                TestDC.SetBKCol(RGB(128,228,128));
                TestDC.SetBKMode(TRANSPARENT);
                TextCol = RGB(iCol,iSue,255);

                result = TestDC.DoTextout();

                if( result )
                {
                    StringCchPrintf(s, 256, TEXT("Transparent-ClearTypeTextout-%d"), iGama);
                    if( TestDC.CompareDC( (TCHAR*)s , g_bTolerance, TextCol, iTol ) )
                    {
                        if(TestDC.ErrorInfo.NumofColorError > 0)
                        {
                            MaxValue = max(TestDC.ErrorInfo.MaxRedError, TestDC.ErrorInfo.MaxGreenError);
                            MaxValue = max(TestDC.ErrorInfo.MaxBlueError, MaxValue);
                            if(MaxValue > MaxEdgeError)
                            {
                                MaxEdgeError = MaxValue;
                                ERvalue = iCol;
                                EGvalue = iSue;
                                EBvalue = 255;
                            }
                        }
                        if(TestDC.ErrorInfo.NumofInteriorColorError > 0)
                        {
                            if(TestDC.ErrorInfo.MaxInteriorError > MaxInternalError)
                            {
                                MaxInternalError = TestDC.ErrorInfo.MaxInteriorError;
                                IRvalue = iCol;
                                IGvalue = iSue;
                                IBvalue = 255;
                            }
                        }
                    }
                    else
                    {
                        StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                        OutputDebugString(s);
                        //g_pMyLog->Fail(s);
                    }
                }
            }
        }
        for(UINT iCol = 0; iCol < 256; iCol+=3)
        {
            // run the test case
            for(UINT iSue = 0; iSue < 256; iSue+=4)
            {
                HBRUSH hBrush = CreateSolidBrush(0x00ffffff);
                if (hBrush == NULL)
                {
                    OutputDebugString(_T("hBrush is NULL\n"));
                    g_pMyLog->Fail(_T("Brush is NULL"));
                    g_pMyLog->EndTest();
                    return 1;
                }
                if( !TestDC.Initialize(hBrush) )
                {
                    StringCchPrintf(s, 256, _T("The Initialization of Compatible DC failed\n"));
                    g_pMyLog->Fail(s);
                    g_pMyLog->EndTest();
                    DeleteObject(hBrush);
                    return 1;
                }
                DeleteObject(hBrush);
                hBrush = NULL;

                TestDC.SetTextCol(RGB(255,iCol,iSue));
                TestDC.SetBKCol(RGB(128,228,128));
                TestDC.SetBKMode(TRANSPARENT);
                TextCol = RGB(255,iCol,iSue);

                result = TestDC.DoTextout();

                if( result )
                {
                    StringCchPrintf(s, 256, TEXT("Transparent-ClearTypeTextout-%d"), iGama);
                    if( TestDC.CompareDC( (TCHAR*)s , g_bTolerance, TextCol, iTol ) )
                    {
                        if(TestDC.ErrorInfo.NumofColorError > 0)
                        {
                            MaxValue = max(TestDC.ErrorInfo.MaxRedError, TestDC.ErrorInfo.MaxGreenError);
                            MaxValue = max(TestDC.ErrorInfo.MaxBlueError, MaxValue);
                            if(MaxValue > MaxEdgeError)
                            {
                                MaxEdgeError = MaxValue;
                                EGvalue = iCol;
                                ERvalue = 255;
                                EBvalue = iSue;
                            }
                        }
                        if(TestDC.ErrorInfo.NumofInteriorColorError > 0)
                        {
                            if(TestDC.ErrorInfo.MaxInteriorError > MaxInternalError)
                            {
                                MaxInternalError = TestDC.ErrorInfo.MaxInteriorError;
                                IGvalue = iCol;
                                IRvalue = 255;
                                IBvalue = iSue;
                            }
                        }
                    }
                    else
                    {
                        StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                        OutputDebugString(s);
                        //g_pMyLog->Fail(s);
                    }
                }
            }
        }
        for(UINT iCol = 0; iCol < 256; iCol+=3)
        {
            // run the test case
            for(UINT iSue = 0; iSue < 256; iSue+=4)
            {
                HBRUSH hBrush = CreateSolidBrush(0x00ffffff);
                if (hBrush == NULL)
                {
                    OutputDebugString(_T("hBrush is NULL\n"));
                    g_pMyLog->Fail(_T("Brush is NULL"));
                    g_pMyLog->EndTest();
                    return 1;
                }
                if( !TestDC.Initialize(hBrush) )
                {
                    StringCchPrintf(s, 256, _T("The Initialization of Compatible DC failed\n"));
                    g_pMyLog->Fail(s);
                    g_pMyLog->EndTest();
                    DeleteObject(hBrush);
                    return 1;
                }
                DeleteObject(hBrush);
                hBrush = NULL;

                TestDC.SetTextCol(RGB(iSue,255,iCol));
                TestDC.SetBKCol(RGB(128,228,128));
                TestDC.SetBKMode(TRANSPARENT);
                TextCol = RGB(iSue,255,iCol);

                result = TestDC.DoTextout();

                if( result )
                {
                    StringCchPrintf(s, 256, TEXT("Transparent-ClearTypeTextout-%d"), iGama);
                    if( TestDC.CompareDC( (TCHAR*)s , g_bTolerance, TextCol, iTol ) )
                    {
                        if(TestDC.ErrorInfo.NumofColorError > 0)
                        {
                            MaxValue = max(TestDC.ErrorInfo.MaxRedError, TestDC.ErrorInfo.MaxGreenError);
                            MaxValue = max(TestDC.ErrorInfo.MaxBlueError, MaxValue);
                            if(MaxValue > MaxEdgeError)
                            {
                                MaxEdgeError = MaxValue;
                                EBvalue = iCol;
                                EGvalue = 255;
                                ERvalue = iSue;
                            }
                        }
                        if(TestDC.ErrorInfo.NumofInteriorColorError > 0)
                        {
                            if(TestDC.ErrorInfo.MaxInteriorError > MaxInternalError)
                            {
                                MaxInternalError = TestDC.ErrorInfo.MaxInteriorError;
                                IBvalue = iCol;
                                IGvalue = 255;
                                IRvalue = iSue;
                            }
                        }
                    }
                    else
                    {
                        StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                        OutputDebugString(s);
                        //g_pMyLog->Fail(s);
                    }
                }
            }
        }

        StringCchPrintf(s, 256, _T("The max error in edge pixels with gamma, %d, at RGB value (%d, %d, %d) is: %d\n"),
            iGama, ERvalue, EGvalue, EBvalue, MaxEdgeError);
        g_pMyLog->Message(s);
        StringCchPrintf(s, 256, _T("The max error in interior pixels with gamma, %d, at RGB value (%d, %d, %d) is: %d\n"),
            iGama, IRvalue, IGvalue, IBvalue, MaxInternalError);
        g_pMyLog->Message(s);
    }

    TestDC.Select((HGDIOBJ)OldFont);
    if(fClearType)
    {
        DeleteObject(fClearType);
    }

    if(!SystemParametersInfo(SPI_SETFONTSMOOTHINGCONTRAST, 0, (DWORD*)SystemParam, 0))
    {
        StringCchPrintf(s, 256, _T("Set original FONTSMOOTHINGCONTRAST failed\n"));
        g_pMyLog->Warn(s);
    }
    g_pMyLog->EndTest();

    return NumFailures;//printf("The test for transparent Textout with clearType font failed\n");
}

int TestCase_Trans_Textout_Antialiased(HWND hWnd, HDC hMonitorDC)
{
    TCHAR s[1000];
    BOOL result;

    CCompatibleDC TestDC(hWnd, hMonitorDC, g_bDeviceBitmap);

    g_pMyLog->StartTest(TEXT("Transparent_TextoutAntialiased Test"));

    if( !TestDC.IsValid() )
    {
        StringCchPrintf(s, 256, _T("The creation of CCompatibleDC failed\n"));
        OutputDebugString(s);
        g_pMyLog->Fail(s);
        g_pMyLog->EndTest();
        return 1;
    }

    BOOL bUseDeviceBitmap = TestDC.IsUsingDeviceBitmap();
    if(!bUseDeviceBitmap && g_bDeviceBitmap)
    {
        StringCchPrintf(s, 256, _T("CreateBitmapFromDxSurface failed\n"));
        OutputDebugString(s);
        g_pMyLog->Warn(s);
    }

    HFONT fAntialiased = CreateFont(-20, 0, 0, 0, FW_NORMAL, 0, 0, 0,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, DEFAULT_PITCH, TEXT("Tahoma"));
    HFONT OldFont = (HFONT)TestDC.Select((HGDIOBJ)fAntialiased);

    // run the test case
    BOOL bTestResult = FALSE;
    int NumFailures = 0;
    UINT MaxIter = 1;
    if(g_bDebug)
        MaxIter = 3000;
    for(UINT iCol = 5; iCol < 256; iCol +=10)
    {
        for(UINT iter = 0; iter < MaxIter; iter++) //for debug purpose 
        {
            HBRUSH hBrush = CreateSolidBrush(0x00805060);
            if (hBrush == NULL)
            {
                OutputDebugString(_T("hBrush is NULL\n"));
                g_pMyLog->Fail(_T("Brush is NULL"));
                g_pMyLog->EndTest();
                return 1;
            }
            if( !TestDC.Initialize(hBrush) )
            {
                StringCchPrintf(s, 256, _T("The Initialization of Compatible DC failed\n"));
                g_pMyLog->Fail(s);
                g_pMyLog->EndTest();
                DeleteObject(hBrush);
                return 1;
            }
            DeleteObject(hBrush);
            hBrush = NULL;

            TestDC.SetTextCol(RGB(128,iCol,128));
            TestDC.SetBKCol(RGB(128,128,128));
            TestDC.SetBKMode(TRANSPARENT);

            result = TestDC.DoTextout();
        }

        if( result )
        {
            StringCchPrintf(s, 256, TEXT("Transparent-TextoutAntialiased"));
            if( TestDC.CompareDC( (TCHAR*) s, g_bTolerance))
            {
                bTestResult = LogTestResults(s, "textoutantialiast", &TestDC.ErrorInfo, 10000, FALSE);
                if(!bTestResult)
                {
                    StringCchCat(s, 256, TEXT(" failed"));
                    g_pMyLog->Fail(s);
                }
            }
            else
            {
                StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                OutputDebugString(s);
                g_pMyLog->Fail(s);
            }

            if(bUseDeviceBitmap)
            {
                StringCchPrintf(s, 256, TEXT("Transparent-TextoutAntialiased"));
                TestDC.SyncBitmapDC(TRUE);
                if( TestDC.CompareDC( (TCHAR*) s, g_bTolerance) )
                {
                    bTestResult = LogTestResults(s, "textoutantialiast", &TestDC.ErrorInfo, 10000, TRUE);
                    if(!bTestResult)
                    {
                        StringCchCat(s, 256, TEXT("-devicebitmap failed"));
                        g_pMyLog->Fail(s);
                    }
                }
                else
                {
                    StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                    OutputDebugString(s);
                    g_pMyLog->Fail(s);
                }
            }
        }
        else
        {
            StringCchPrintf(s, 256, _T("The Textout in the compatible DC failed\n"));
            OutputDebugString(s);
            g_pMyLog->Fail(s);
        }
    }

    if( !bTestResult )
    {
        NumFailures++;
    }

    TestDC.Select((HGDIOBJ)OldFont);
    if(fAntialiased)
    {
        DeleteObject(fAntialiased);
    }
    g_pMyLog->EndTest();

    return NumFailures;//printf("The test for transparent Textout with antialiased font failed\n");
}
int TestCase_LineTo(HWND hWnd, HDC hMonitorDC, BOOL bClipping)
{
    TCHAR s[1000];
    TCHAR tClip[10];
    BOOL result;

    CCompatibleDC TestDC(hWnd, hMonitorDC, g_bDeviceBitmap);

    if( !TestDC.IsValid() )
    {
        StringCchPrintf(s, 256, _T("The creation of CCompatibleDC failed\n"));
        OutputDebugString(s);
        g_pMyLog->Fail(s);
        g_pMyLog->EndTest();
        return 1;
    }

    if(bClipping)
        StringCchPrintf(tClip, 10, _T("Clip"));
    else
        StringCchPrintf(tClip, 10, _T("noClip"));

    StringCchPrintf(s, 256, TEXT("Lineto test with %s"), tClip);
    g_pMyLog->StartTest(s);

    HDC hDC = GetDC(hWnd);
    if (hDC == NULL)
    {
        OutputDebugString(_T("DC is NULL\n"));
        g_pMyLog->Fail(TEXT("DC is NULL"));
        g_pMyLog->EndTest();
        return 1;
    }
    RECT r;
    GetClientRect(hWnd, &r);
    UINT Width = (r.right - r.left);
    UINT Height = (r.bottom - r.top);

    int PenStyles[] = { (int) ( PS_SOLID ), (int) ( PS_NULL ),
        (int) ( PS_INSIDEFRAME ), (int) ( PS_DASH ),
        (int) ( PS_DOT ), (int) ( PS_DASHDOT ),
        (int) ( PS_DASHDOTDOT )
    };
    TCHAR* PenStyleName[] =  {TEXT("SOLID"), TEXT("INVISIBLE"),
        TEXT("NSIDEFRAME"), TEXT("DASH"),
        TEXT("DOT"), TEXT("DASHDOT"),
        TEXT("DASHDOTDOT")
    };
    UINT NumofPenStyles = sizeof(PenStyles)/sizeof(int);

    TCHAR* ThemeName[] =  { TEXT("LineTo-Horizon-Vetical"),
        TEXT("LineTo-Diagnal"),
        TEXT("LineTo-Mixed")
    };

    BOOL bUseDeviceBitmap = TestDC.IsUsingDeviceBitmap();
    if(!bUseDeviceBitmap && g_bDeviceBitmap)
    {
        StringCchPrintf(s, 256, _T("CreateBitmapFromDxSurface failed\n"));
        OutputDebugString(s);
        g_pMyLog->Warn(s);
    }

    // Fill the background with the brown color
    HBRUSH hBrush = CreateSolidBrush(0x00805060);
    if (hBrush == NULL)
    {
        OutputDebugString(_T("hBrush is NULL\n"));
        g_pMyLog->Fail(_T("Brush is NULL"));
        g_pMyLog->EndTest();
        return 1;
    }
    //run the test cases
    BOOL bTestResult;
    int NumFailures = 0;
    int TestCaseNum = -1;
    UINT MaxIter = 1;
    if(g_bDebug)
        MaxIter = 1000;

    COLORREF crColor = RGB(128, 128, 128); //white pen;
    HPEN hPen = NULL, hOldPen = NULL;
    for(UINT nTheme = 0; nTheme < 3; nTheme++)
    {
        for(UINT nPenStyle = 0; nPenStyle < NumofPenStyles; nPenStyle++)
        {
            for(UINT nWidth = 1; nWidth < 5; nWidth++)
            {
                bTestResult = FALSE;
                TestCaseNum++;
                if(g_iTestCase >= 0)
                {
                    if(g_iTestCase != TestCaseNum)
                        continue;
                }
                if( !TestDC.SelectRgn(NULL) )  //remove clip region firstly
                {
                    OutputDebugString(_T("SelectClipRgn failed in TestDC\n"));
                    g_pMyLog->Fail(TEXT("SelectClipRgn failed in TestDC\n"));
                    DeleteObject(hBrush);
                    if(hPen)
                        DeleteObject(hPen);
                    ReleaseDC(hWnd, hDC);
                    g_pMyLog->EndTest();
                    return 1;
                }
                if( !TestDC.Initialize(hBrush) )
                {
                    StringCchPrintf(s, 256, _T("The Initialization of Compatible DC failed\n"));
                    g_pMyLog->Fail(s);
                    g_pMyLog->EndTest();
                    DeleteObject(hBrush);
                    ReleaseDC(hWnd, hDC);
                    if(NULL != hPen)
                        DeleteObject(hPen);
                    return 1;
                }

                if(bClipping)
                {
                    HRGN hRgn;
                    hRgn = CreateRectRgn(0, 0, Width, Height);
                    if(NULL == hRgn)
                    {
                        g_pMyLog->Fail(TEXT("CreateRectRgn failed."));
                        DeleteObject(hBrush);
                        if(hPen)
                            DeleteObject(hPen);
                        ReleaseDC(hWnd, hDC);
                        g_pMyLog->EndTest();
                        return 1;
                    }

                    if( !TestDC.SelectRgn(hRgn))
                    {
                        OutputDebugString(_T("SelectClipRgn failed\n"));
                        g_pMyLog->Fail(TEXT("SelectClipRgn failed\n"));
                        DeleteObject(hBrush);
                        if(hPen)
                            DeleteObject(hPen);
                        ReleaseDC(hWnd, hDC);
                        g_pMyLog->EndTest();
                        return 1;
                    }
                    for(int i = 0; i < (int)Width; i += 16)
                    {
                        for(int j = 0; j < (int)Height; j += 16)
                        {
                            if( !TestDC.ExcludeRect(i, j, i+8, j+8) )
                            {
                                OutputDebugString(_T("ExcludeClipRect failed\n"));
                                g_pMyLog->Fail(TEXT("ExcludeClipRect failed\n"));
                                DeleteObject(hBrush);
                                if(hPen)
                                    DeleteObject(hPen);
                                ReleaseDC(hWnd, hDC);
                                g_pMyLog->EndTest();
                                return 1;
                            }
                        }
                    }
                    DeleteObject(hRgn);
                }

                //Delete the pen if it exists in case of the leaking
                if(hPen)
                {
                    DeleteObject(hPen);
                    hPen = NULL;
                }
                hPen = CreatePen(PenStyles[nPenStyle], nWidth, crColor);
                if(NULL != hPen)
                {
                    hOldPen = (HPEN)TestDC.Select(hPen);
                }
                else
                {
                    StringCchPrintf(s, 256, _T("Waring: CreatePen failed for style %s, Width %d. The default white pen was insteadly used.\n"), PenStyleName[nPenStyle], nWidth);
                    g_pMyLog->Warn(s);
                    hOldPen = (HPEN)TestDC.Select(GetStockObject(WHITE_PEN));
                }

                for(UINT iter = 0; iter < MaxIter; iter++) //for debug purpose
                {
                    result = TestDC.DoLineTo(nTheme+1);
                }
                TestDC.Select(hOldPen);
                if( result )
                {
                    StringCchPrintf(s, 1000, TEXT("%s-%s-%d-%s"),ThemeName[nTheme], PenStyleName[nPenStyle], nWidth, tClip);
                    if( TestDC.CompareDC( (TCHAR*)s , g_bTolerance) )
                    {
                        if(!bClipping)
                        {
                            bTestResult = LogTestResults(s, "lineto", &TestDC.ErrorInfo, TestCaseNum, FALSE);
                        }
                        else
                        {
                            bTestResult = LogTestResults(s, "lineto", &TestDC.ErrorInfo, TestCaseNum+1000, FALSE);
                        }
                        if(!bTestResult)
                        {
                            StringCchCat(s, 256, TEXT(" failed"));
                            g_pMyLog->Fail(s);
                        }
                    }
                    else
                    {
                        StringCchPrintf(s, 1000, _T("The opreation of compare in the compatible DC failed\n"));
                        OutputDebugString(s);
                        g_pMyLog->Fail(s);
                    }

                    if(bUseDeviceBitmap)
                    {
                        StringCchPrintf(s, 1000, TEXT("%s-%s-%d-%s"),ThemeName[nTheme], PenStyleName[nPenStyle], nWidth, tClip);
                        TestDC.SyncBitmapDC(TRUE);
                        if( TestDC.CompareDC( (TCHAR*)s , g_bTolerance) )
                        {
                            if(!bClipping)
                            {
                                bTestResult = LogTestResults(s, "lineto", &TestDC.ErrorInfo, TestCaseNum, TRUE);
                            }
                            else
                            {
                                bTestResult = LogTestResults(s, "lineto", &TestDC.ErrorInfo, TestCaseNum+1000, TRUE);
                            }
                            if(!bTestResult)
                            {
                                StringCchCat(s, 256, TEXT("-devicebitmap failed"));
                                g_pMyLog->Fail(s);
                            }
                        }
                        else
                        {
                            StringCchPrintf(s, 1000, _T("The opreation of compare in the compatible DC failed\n"));
                            OutputDebugString(s);
                            g_pMyLog->Fail(s);
                        }
                    }
                }
                else
                {
                    StringCchPrintf(s, 1000, _T("The LineTo in the compatible DC failed\n"));
                    OutputDebugString(s);
                    g_pMyLog->Fail(s);
                }
                if( !bTestResult )
                {
                    NumFailures++;
                }
            }
        }
    }

    g_pMyLog->EndTest();
    if(hBrush)
        DeleteObject(hBrush);
    if(hPen)
        DeleteObject(hPen);
    ReleaseDC(hWnd, hDC);

    return NumFailures;//wprintf(L"The test for LineTo failed\n");
}
int TestCase_StrokePath(HWND hWnd, HDC hMonitorDC, BOOL bClipping)
{
    TCHAR s[1000];
    TCHAR tClip[10];
    BOOL result;

    CCompatibleDC TestDC(hWnd, hMonitorDC, g_bDeviceBitmap);

    if( !TestDC.IsValid() )
    {
        StringCchPrintf(s, 256, _T("The creation of CCompatibleDC failed\n"));
        OutputDebugString(s);
        g_pMyLog->Fail(s);
        g_pMyLog->EndTest();
        return 1;
    }

    if(bClipping)
        StringCchPrintf(tClip, 10, _T("Clip"));
    else
        StringCchPrintf(tClip, 10, _T("noClip"));

    StringCchPrintf(s, 256, TEXT("StrokePath test with %s"), tClip);
    g_pMyLog->StartTest(s);

    HDC hDC = GetDC(hWnd);
    if (hDC == NULL)
    {
        OutputDebugString(_T("DC is NULL\n"));
        g_pMyLog->Fail(TEXT("DC is NULL"));
        g_pMyLog->EndTest();
        return 1;
    }
    RECT r;
    GetClientRect(hWnd, &r);
    UINT Width = (r.right - r.left);
    UINT Height = (r.bottom - r.top);

    int PenStyles[] = { (int) ( PS_SOLID ), (int) ( PS_NULL ),
        (int) ( PS_INSIDEFRAME ), (int) ( PS_DASH ),
        (int) ( PS_DOT ), (int) ( PS_DASHDOT ),
        (int) ( PS_DASHDOTDOT )
    };
    TCHAR* PenStyleName[] =  {TEXT("SOLID"), TEXT("INVISIBLE"),
        TEXT("NSIDEFRAME"), TEXT("DASH"),
        TEXT("DOT"), TEXT("DASHDOT"),
        TEXT("DASHDOTDOT")
    };
    UINT NumofPenStyles = sizeof(PenStyles)/sizeof(int);

    TCHAR* ThemeName[] =  { TEXT("StrokePath-Curves"),
        TEXT("StrokePath-Ellipse")
    };

    BOOL bUseDeviceBitmap = TestDC.IsUsingDeviceBitmap();
    if(!bUseDeviceBitmap && g_bDeviceBitmap)
    {
        StringCchPrintf(s, 256, _T("CreateBitmapFromDxSurface failed\n"));
        OutputDebugString(s);
        g_pMyLog->Warn(s);
    }

    // Fill the background with the brown color
    HBRUSH hBrush = CreateSolidBrush(0x00805060);
    if (hBrush == NULL)
    {
        OutputDebugString(_T("hBrush is NULL\n"));
        g_pMyLog->Fail(_T("Brush is NULL"));
        g_pMyLog->EndTest();
        return 1;
    }
    //run the test cases
    BOOL bTestResult;
    int NumFailures = 0;
    int TestCaseNum = -1;
    UINT MaxIter = 1;
    if(g_bDebug)
        MaxIter = 1000;

    COLORREF crColor = RGB(128, 128, 128); //white pen;
    HPEN hPen = NULL, hOldPen = NULL;
    for(UINT nTheme = 0; nTheme < 2; nTheme++)
    {
        for(UINT nPenStyle = 0; nPenStyle < NumofPenStyles; nPenStyle++)
        {
            for(UINT nWidth = 1; nWidth < 5; nWidth++)
            {
                bTestResult = FALSE;
                TestCaseNum++;
                if(g_iTestCase >= 0)
                {
                    if(g_iTestCase != TestCaseNum)
                        continue;
                }
                if( !TestDC.SelectRgn(NULL) )  //remove clip region firstly
                {
                    OutputDebugString(_T("SelectClipRgn failed in TestDC\n"));
                    g_pMyLog->Fail(TEXT("SelectClipRgn failed in TestDC\n"));
                    DeleteObject(hBrush);
                    if(hPen)
                        DeleteObject(hPen);
                    ReleaseDC(hWnd, hDC);
                    g_pMyLog->EndTest();
                    return 1;
                }
                if( !TestDC.Initialize(hBrush) )
                {
                    StringCchPrintf(s, 256, _T("The Initialization of Compatible DC failed\n"));
                    g_pMyLog->Fail(s);
                    g_pMyLog->EndTest();
                    DeleteObject(hBrush);
                    ReleaseDC(hWnd, hDC);
                    if(NULL != hPen)
                        DeleteObject(hPen);
                    return 1;
                }

                if(bClipping)
                {
                    HRGN hRgn = CreateRectRgn(0,0,Width,Height);
                    if(NULL == hRgn)
                    {
                        g_pMyLog->Fail(TEXT("CreateRectRgn failed."));
                        g_pMyLog->EndTest();
                        DeleteObject(hBrush);
                        ReleaseDC(hWnd, hDC);
                        if(NULL != hPen)
                            DeleteObject(hPen);
                        return 1;
                    }

                    if(!TestDC.SelectRgn(hRgn))
                    {
                        g_pMyLog->Fail(TEXT("SelectClipRgn failed."));
                        g_pMyLog->EndTest();
                        DeleteObject(hBrush);
                        ReleaseDC(hWnd, hDC);
                        if(NULL != hPen)
                            DeleteObject(hPen);
                        return 1;
                    }
                    if(!TestDC.ExcludeRect((int)(0.2f*Width),(int)(0.2f*Height),
                        (int)(0.3f*Width),(int)(0.7f*Height)))
                    {
                        g_pMyLog->Fail(TEXT("ExcludeClipRgn failed."));
                        g_pMyLog->EndTest();
                        DeleteObject(hBrush);
                        ReleaseDC(hWnd, hDC);
                        if(NULL != hPen)
                            DeleteObject(hPen);
                        return 1;
                    }
                    if(!TestDC.ExcludeRect((int)(0.7f*Width),(int)(0.2f*Height),
                        (int)(0.8f*Width),(int)(0.7f*Height)))
                    {
                        g_pMyLog->Fail(TEXT("ExcludeClipRgn failed."));
                        g_pMyLog->EndTest();
                        DeleteObject(hBrush);
                        ReleaseDC(hWnd, hDC);
                        if(NULL != hPen)
                            DeleteObject(hPen);
                        return 1;
                    }
                    DeleteObject(hRgn);
                }

                //Delete the pen if it already exists
                if(hPen)
                {
                    DeleteObject(hPen);
                    hPen = NULL;
                }
                hPen = CreatePen(PenStyles[nPenStyle], nWidth, crColor);
                if(NULL != hPen)
                {
                    hOldPen = (HPEN)TestDC.Select(hPen);
                }
                else
                {
                    StringCchPrintf(s, 256, _T("Waring: CreatePen failed for style %s, Width %d. The default white pen was insteadly used.\n"), PenStyleName[nPenStyle], nWidth);
                    g_pMyLog->Warn(s);
                    hOldPen = (HPEN)TestDC.Select(GetStockObject(WHITE_PEN));
                }

                for(UINT iter = 0; iter < MaxIter; iter++) //for debug purpose
                {
                    result = TestDC.DoStrokePath(nTheme+1);
                }
                TestDC.Select(hOldPen);
                if( result )
                {
                    StringCchPrintf(s, 1000, TEXT("%s-%s-%d-%s"),ThemeName[nTheme], PenStyleName[nPenStyle], nWidth, tClip);
                    if( TestDC.CompareDC( (TCHAR*)s , g_bTolerance) )
                    {
                        if(!bClipping)
                        {
                            bTestResult = LogTestResults(s, "strokepath", &TestDC.ErrorInfo, TestCaseNum, FALSE);
                        }
                        else
                        {
                            bTestResult = LogTestResults(s, "strokepath", &TestDC.ErrorInfo, TestCaseNum+1000, FALSE);
                        }
                        if(!bTestResult)
                        {
                            StringCchCat(s, 256, TEXT(" failed"));
                            g_pMyLog->Fail(s);
                        }
                    }
                    else
                    {
                        StringCchPrintf(s, 1000, _T("The opreation of compare in the compatible DC failed\n"));
                        OutputDebugString(s);
                        g_pMyLog->Fail(s);
                    }

                    if(bUseDeviceBitmap)
                    {
                        StringCchPrintf(s, 1000, TEXT("%s-%s-%d-%s"),ThemeName[nTheme], PenStyleName[nPenStyle], nWidth, tClip);
                        TestDC.SyncBitmapDC(TRUE);
                        if( TestDC.CompareDC( (TCHAR*)s , g_bTolerance) )
                        {
                            if(!bClipping)
                            {
                                bTestResult = LogTestResults(s, "strokepath", &TestDC.ErrorInfo, TestCaseNum, TRUE);
                            }
                            else
                            {
                                bTestResult = LogTestResults(s, "strokepath", &TestDC.ErrorInfo, TestCaseNum+1000, TRUE);
                            }
                            if(!bTestResult)
                            {
                                StringCchCat(s, 256, TEXT("-devicebitmap failed"));
                                g_pMyLog->Fail(s);
                            }
                        }
                        else
                        {
                            StringCchPrintf(s, 1000, _T("The opreation of compare in the compatible DC failed\n"));
                            OutputDebugString(s);
                            g_pMyLog->Fail(s);
                        }
                    }
                }
                else
                {
                    StringCchPrintf(s, 1000, _T("The StrokePath in the compatible DC failed\n"));
                    OutputDebugString(s);
                    g_pMyLog->Fail(s);
                }
                if( !bTestResult )
                {
                    NumFailures++;
                }
            }
        }
    }
    g_pMyLog->EndTest();

    if(hBrush)
        DeleteObject(hBrush);
    if(hPen)
        DeleteObject(hPen);
    ReleaseDC(hWnd, hDC);

    return NumFailures;//wprintf(L"The test for StrokePath failed\n");
}
int TestCase_MaskBlt(HWND hWnd, HDC hMonitorDC, BOOL bClipping)
{
    TCHAR s[1000];
    HBRUSH hBrush = NULL;
    HBRUSH hBrush1 = NULL;

    DWORD MaskBltROPs[] =  { (DWORD) ( BLACKNESS ), (DWORD) ( DSTINVERT ),
        (DWORD) ( MERGECOPY ) , (DWORD) ( MERGEPAINT ),
        (DWORD) ( NOTSRCCOPY ), (DWORD) ( NOTSRCERASE ),
        (DWORD) ( SRCCOPY )   , (DWORD) ( SRCERASE ),
        (DWORD) ( SRCINVERT ) , (DWORD) ( SRCPAINT ),
        (DWORD) ( WHITENESS ) , (DWORD) ( SRCAND )
    };
    TCHAR* MaskBltROPsName[] =  { TEXT("BLACKNESS"), TEXT("DSTINVERT"),
        TEXT("MERGECOPY") , TEXT("MERGEPAINT"),
        TEXT("NOTSRCCOPY"), TEXT("NOTSRCERASE"),
        TEXT("SRCCOPY")   , TEXT("SRCERASE"),
        TEXT("SRCINVERT") , TEXT("SRCPAINT"),
        TEXT("WHITENESS") , TEXT("SRCAND")
    };
    int NumofMaskBltROPs = sizeof(MaskBltROPs)/sizeof(DWORD);

    if(bClipping)
        g_pMyLog->StartTest(TEXT("MaskBlt test with Clips"));
    else
        g_pMyLog->StartTest(TEXT("MaskBlt test without Clips"));

    HDC hDC = GetDC(hWnd);
    if (hDC == NULL)
    {
        OutputDebugString(_T("DC is NULL\n"));
        g_pMyLog->Fail(TEXT("DC is NULL"));
        g_pMyLog->EndTest();
        return 1;
    }
    HDC hTempDC = NULL;
    HBITMAP hTempBitmap = NULL;

    CCompatibleDC TestDC(hWnd, hMonitorDC, g_bDeviceBitmap);

    if( !TestDC.IsValid() )
    {
        StringCchPrintf(s, 256, _T("The creation of CCompatibleDC failed\n"));
        OutputDebugString(s);
        g_pMyLog->Fail(s);
        g_pMyLog->EndTest();
        ReleaseDC(hWnd, hDC);
        return 1;
    }

    BOOL bUseDeviceBitmap = TestDC.IsUsingDeviceBitmap();
    if(!bUseDeviceBitmap && g_bDeviceBitmap)
    {
        StringCchPrintf(s, 256, _T("CreateBitmapFromDxSurface failed\n"));
        OutputDebugString(s);
        g_pMyLog->Warn(s);
    }

    RECT r;
    GetClientRect(hWnd, &r);
    UINT Width = (r.right - r.left);
    UINT Height = (r.bottom - r.top);

    UINT BitmapSizeX = 16;
    UINT BitmapSizeY = 16;

    //Create Mask Bitmap
    WORD bitPattern[] = {0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa,
        0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa};
    HBITMAP hMask = CreateBitmap(16, 16, 1, 1, bitPattern);

    if(NULL == hMask)
    {
        StringCchPrintf(s, 256, _T("The creation of mask bitmap failed\n"));
        OutputDebugString(s);
        g_pMyLog->Fail(s);
        g_pMyLog->EndTest();
        ReleaseDC(hWnd, hDC);
        return 1;
    }

    hTempDC = CreateCompatibleDC(hDC);
    if (hTempDC == NULL)
    {
        OutputDebugString(_T("DC is NULL\n"));
        g_pMyLog->Fail(_T("DC is NULL"));
        goto exit;
    }

    hTempBitmap = CreateCompatibleBitmap (hDC, BitmapSizeX, BitmapSizeY);
    if (hTempBitmap == NULL)
    {
        OutputDebugString(_T("DC is NULL\n"));
        g_pMyLog->Fail(_T("DC is NULL"));
        goto exit;
    }

    hBrush = CreateSolidBrush(0x00800000);
    if (hBrush == NULL)
    {
        OutputDebugString(_T("hBrush is NULL\n"));
        g_pMyLog->Fail(_T("Brush is NULL"));
        goto exit;
    }
    hBrush1 = CreateSolidBrush(0x00008000);
    if (hBrush1 == NULL)
    {
        OutputDebugString(_T("hBrush is NULL\n"));
        g_pMyLog->Fail(_T("Brush is NULL"));
        goto exit;
    }

    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hTempDC, hTempBitmap);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hTempDC, hBrush);
    // Fill the background of the bitmap
    if (!BitBlt(hTempDC, 0, 0, BitmapSizeX, BitmapSizeY, NULL, 0, 0, PATCOPY))
    {
        DeleteObject(hBrush);
        hBrush = NULL;
        DeleteObject(hBrush1);
        hBrush1 = NULL;
        OutputDebugString(_T("BitBlt failed\n"));
        g_pMyLog->Fail(_T("BitBlt failed"));
        g_pMyLog->EndTest();
        goto exit;
    }
    SelectObject(hTempDC, hOldBrush);
    hOldBrush = (HBRUSH)SelectObject(hTempDC, hBrush1);
    BitBlt(hTempDC, BitmapSizeX / 4, BitmapSizeY / 4, 
        BitmapSizeX / 4, BitmapSizeY / 4, NULL, 0, 0, PATCOPY);
    BitBlt(hTempDC, BitmapSizeX / 2, BitmapSizeY / 2, 
        BitmapSizeX / 4, BitmapSizeY / 4, NULL, 0, 0, PATCOPY);
    SelectObject(hTempDC, hOldBrush);
    DeleteObject(hBrush);
    hBrush = NULL;
    DeleteObject(hBrush1);
    hBrush1 = NULL;

    hBrush = CreateSolidBrush(0x00805060);
    if (hBrush == NULL)
    {
        OutputDebugString(_T("hBrush is NULL\n"));
        g_pMyLog->Fail(_T("Brush is NULL"));
        goto exit;
    }

    //Firstly render to hDC so that the redirection surface for this window DC is created before starting the test
    if( !TestDC.Initialize(hBrush) )
    {
        g_pMyLog->Fail(_T("Warning: the Init of CCompatibleDC failed"));
    }

    BOOL bTestResult;
    int NumFailures = 0;
    int TestCaseNum = -1;
    for(int i=0; i<NumofMaskBltROPs; i++)
    {
        for(int j=0; j<NumofMaskBltROPs; j++)
        {
            bTestResult = FALSE;
            TestCaseNum++;
            if(g_iTestCase >= 0)
            {
                if(g_iTestCase != TestCaseNum)
                {
                    continue;
                }
            }

            // Remove the clips and fill the background with the brown color
            if( !TestDC.SelectRgn(NULL) || !TestDC.Initialize(hBrush) )
            {
                g_pMyLog->Fail(_T("The SelectClipRgn or Init of CCompatibleDC failed"));
                goto exit;
            }

            if(bClipping)
            {
                HRGN hRgn = CreateRectRgn(r.left+(int)(0.1f*Width),r.top+(int)(0.1f*Height),
                    r.right-(int)(0.1f*Width),r.bottom-(int)(0.1f*Height));
                if(NULL == hRgn)
                {
                    g_pMyLog->Fail(TEXT("CreateRectRgn failed."));
                    goto exit;
                }
                if(!TestDC.SelectRgn(hRgn))
                {
                    g_pMyLog->Fail(TEXT("SelectClipRgn failed."));
                    goto exit;
                }
                if(!TestDC.ExcludeRect(r.left+(int)(0.2f*Width),r.top+(int)(0.2f*Height),
                    r.left+(int)(0.5f*Width),r.top+(int)(0.5f*Height)))
                {
                    g_pMyLog->Fail(TEXT("ExcludeClipRgn failed."));
                    goto exit;
                }
                DeleteObject(hRgn);
            }

            DWORD fRop = MaskBltROPs[i];
            DWORD bRop = MaskBltROPs[j];

            // Run test case
            BOOL result;
            UINT MaxIter = 1;
            if(g_bDebug)
                MaxIter = 3000;
            for(UINT iter = 0; iter < MaxIter; iter++) //for debug purpose 
            {
                result = TestDC.DoMaskBlt(hTempDC, BitmapSizeX, BitmapSizeY, hMask, fRop, bRop);
            }
            if( !result )
            {
                //the failure could be caused by Non-NULL source DC for the rop that does not need the source, then try again
                result = TestDC.DoMaskBlt(hTempDC, BitmapSizeX, BitmapSizeY, NULL, fRop, bRop);
            }
            if( result )
            {
                StringCchPrintf(s, 256, TEXT("MaskBlt-%s-%s"), MaskBltROPsName[i], MaskBltROPsName[j]);
                if( TestDC.CompareDC( (TCHAR*)s, g_bTolerance ) )
                {
                    if(!bClipping)
                    {
                        bTestResult = LogTestResults(s, "MaskBlt", &TestDC.ErrorInfo, TestCaseNum, FALSE);
                    }
                    else
                    {
                        bTestResult = LogTestResults(s, "MaskBlt", &TestDC.ErrorInfo, TestCaseNum+1000, FALSE);
                    }
                    if(!bTestResult)
                    {
                        StringCchCat(s, 256, TEXT(" failed"));
                        g_pMyLog->Fail(s);
                    }
                }
                else
                {
                    StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                    OutputDebugString(s);
                    g_pMyLog->Fail(s);
                }

                if(bUseDeviceBitmap)
                {
                    StringCchPrintf(s, 256, TEXT("MaskBlt-%s-%s"), MaskBltROPsName[i], MaskBltROPsName[j]);
                    TestDC.SyncBitmapDC(TRUE);
                    if( TestDC.CompareDC( (TCHAR*)s, g_bTolerance ) )
                    {
                        if(!bClipping)
                        {
                            bTestResult = LogTestResults(s, "MaskBlt", &TestDC.ErrorInfo, TestCaseNum, TRUE);
                        }
                        else
                        {
                            bTestResult = LogTestResults(s, "MaskBlt", &TestDC.ErrorInfo, TestCaseNum+1000, TRUE);
                        }
                        if(!bTestResult)
                        {
                            StringCchCat(s, 256, TEXT("-devicebitmap failed"));
                            g_pMyLog->Fail(s);
                        }
                    }
                    else
                    {
                        StringCchPrintf(s, 256, _T("The opreation of compare in the compatible DC failed\n"));
                        OutputDebugString(s);
                        g_pMyLog->Fail(s);
                    }
                }
            }
            else
            {
                StringCchPrintf(s, 256, _T("The MaskBlt failed\n"));
                OutputDebugString(s);
                g_pMyLog->Fail(s);
            }
            if( !bTestResult )
            {
                NumFailures++;
            }
        }
    }
    SelectObject(hTempDC, hOldBitmap);

exit:
    if( NULL != hBrush )
        DeleteObject(hBrush);
    if( NULL != hBrush1 )
        DeleteObject(hBrush1);
    g_pMyLog->EndTest();
    if(hMask)
        DeleteObject(hMask);
    if(hTempBitmap)
        DeleteObject(hTempBitmap);
    if(hTempDC)
        DeleteDC(hTempDC);
    ReleaseDC(hWnd, hDC);

    return NumFailures;//wprintf(L"The test cases for MaskBlt failed\n");
}
//-----------------------------------------------------------------------------
void DoRender(HWND hWnd, HDC hMonitorDC)
{
    BOOL bClipping = FALSE;
    if(g_iTestCase >= 1000)
    {
        bClipping = TRUE;
        g_bClip = FALSE;
        g_iTestCase -= 1000;
    }

    int NumFailures = 0;
    int NumFailedTests = 0;
    int PreNumFailures = 0;
    if(!g_bRunAll)
    {
        switch (g_iRenderOption)
        {
        case 1:
            NumFailures += TestCase_AlphaBlend(hWnd, hMonitorDC, bClipping);
            if(g_bClip)
            {
                NumFailures += TestCase_AlphaBlend(hWnd, hMonitorDC, TRUE);
            }
            break;
        case 2:
            NumFailures += TestCase_ColorFill(hWnd, hMonitorDC, bClipping);
            if(g_bClip)
                NumFailures += TestCase_ColorFill(hWnd, hMonitorDC, TRUE);
            break;
        case 3:
            NumFailures += TestCase_StrokePath(hWnd, hMonitorDC, bClipping);
            if(g_bClip)
                NumFailures += TestCase_StrokePath(hWnd, hMonitorDC, TRUE);
            break;
        case 4:
            //Render(hDC);
            break;
        case 5:
            NumFailures += TestCase_Trans_Textout_ClearType(hWnd, hMonitorDC);
            break;
        case 6:
            NumFailures += TestCase_BitBlt(hWnd, hMonitorDC, bClipping);
            if(g_bClip)
                NumFailures += TestCase_BitBlt(hWnd, hMonitorDC, TRUE);
            break;
        case 7:
            NumFailures += TestCase_TransparentBlt(hWnd, hMonitorDC, bClipping);
            if(g_bClip)
                NumFailures += TestCase_TransparentBlt(hWnd, hMonitorDC, TRUE);
            break;
        case 8:
            NumFailures += TestCase_StretchBlt(hWnd, hMonitorDC, bClipping);
            if(g_bClip)
                NumFailures += TestCase_StretchBlt(hWnd, hMonitorDC, TRUE);
            break;
        case 9:
            NumFailures += TestCase_LineTo(hWnd, hMonitorDC, bClipping);
            if(g_bClip)
                NumFailures += TestCase_LineTo(hWnd, hMonitorDC, TRUE);
            break;
        case 10:
            NumFailures += TestCase_Trans_Textout_Antialiased(hWnd, hMonitorDC);
            break;
        case 11:
            NumFailures += TestCase_Trans_Textout_Antialiased(hWnd, hMonitorDC);
            NumFailures += TestCase_Trans_Textout_ClearType(hWnd, hMonitorDC);
            break;
        case 12:
            NumFailures += TestCase_MaskBlt(hWnd, hMonitorDC, bClipping);
            if(g_bClip)
                NumFailures += TestCase_MaskBlt(hWnd, hMonitorDC, TRUE);
            break;
        default:
            OutputDebugString(TEXT("No option in ExecuteRender"));
            g_pMyLog->Message(TEXT("No option in ExecuteRender\n"));
            break;
        }
        if(NumFailures > PreNumFailures)
        {
            NumFailedTests++;
            PreNumFailures = NumFailures;
        }
    }
    else{
        NumFailures += TestCase_AlphaBlend(hWnd, hMonitorDC, bClipping);
        if(NumFailures > PreNumFailures)
        {
            NumFailedTests++;
            PreNumFailures = NumFailures;
        }
        Sleep(1000);
        NumFailures += TestCase_ColorFill(hWnd, hMonitorDC, bClipping);
        if(NumFailures > PreNumFailures)
        {
            NumFailedTests++;
            PreNumFailures = NumFailures;
        }
        Sleep(1000);
        NumFailures += TestCase_BitBlt(hWnd, hMonitorDC, bClipping);
        if(NumFailures > PreNumFailures)
        {
            NumFailedTests++;
            PreNumFailures = NumFailures;
        }
        Sleep(1000);
        NumFailures += TestCase_StretchBlt(hWnd, hMonitorDC, bClipping);
        if(NumFailures > PreNumFailures)
        {
            NumFailedTests++;
            PreNumFailures = NumFailures;
        }
        Sleep(1000);
        NumFailures += TestCase_TransparentBlt(hWnd, hMonitorDC, bClipping);
        if(NumFailures > PreNumFailures)
        {
            NumFailedTests++;
            PreNumFailures = NumFailures;
        }
        Sleep(1000);
        NumFailures += TestCase_Trans_Textout_ClearType(hWnd, hMonitorDC);
        if(NumFailures > PreNumFailures)
        {
            NumFailedTests++;
            PreNumFailures = NumFailures;
        }
        Sleep(1000);
        NumFailures += TestCase_Trans_Textout_Antialiased(hWnd, hMonitorDC);
        if(NumFailures > PreNumFailures)
        {
            NumFailedTests++;
            PreNumFailures = NumFailures;
        }
        Sleep(1000);
        NumFailures += TestCase_LineTo(hWnd, hMonitorDC, bClipping);
        if(NumFailures > PreNumFailures)
        {
            NumFailedTests++;
            PreNumFailures = NumFailures;
        }
        Sleep(1000);
        NumFailures += TestCase_StrokePath(hWnd, hMonitorDC, bClipping);
        if(NumFailures > PreNumFailures)
        {
            NumFailedTests++;
            PreNumFailures = NumFailures;
        }
        Sleep(1000);
        NumFailures += TestCase_MaskBlt(hWnd, hMonitorDC, bClipping);
        if(NumFailures > PreNumFailures)
        {
            NumFailedTests++;
            PreNumFailures = NumFailures;
        }
        if(g_bClip)
        {
            Sleep(1000);
            NumFailures += TestCase_AlphaBlend(hWnd, hMonitorDC, TRUE);
            if(NumFailures > PreNumFailures)
            {
                NumFailedTests++;
                PreNumFailures = NumFailures;
            }
            Sleep(1000);
            NumFailures += TestCase_ColorFill(hWnd, hMonitorDC, TRUE);
            if(NumFailures > PreNumFailures)
            {
                NumFailedTests++;
                PreNumFailures = NumFailures;
            }
            Sleep(1000);
            NumFailures += TestCase_BitBlt(hWnd, hMonitorDC, TRUE);
            if(NumFailures > PreNumFailures)
            {
                NumFailedTests++;
                PreNumFailures = NumFailures;
            }
            Sleep(1000);
            NumFailures += TestCase_StretchBlt(hWnd, hMonitorDC, TRUE);
            if(NumFailures > PreNumFailures)
            {
                NumFailedTests++;
                PreNumFailures = NumFailures;
            }
            Sleep(1000);
            NumFailures += TestCase_TransparentBlt(hWnd, hMonitorDC, TRUE);
            if(NumFailures > PreNumFailures)
            {
                NumFailedTests++;
                PreNumFailures = NumFailures;
            }
            Sleep(1000);
            NumFailures += TestCase_LineTo(hWnd, hMonitorDC, TRUE);
            if(NumFailures > PreNumFailures)
            {
                NumFailedTests++;
                PreNumFailures = NumFailures;
            }
            Sleep(1000);
            NumFailures += TestCase_StrokePath(hWnd, hMonitorDC, TRUE);
            if(NumFailures > PreNumFailures)
            {
                NumFailedTests++;
                PreNumFailures = NumFailures;
            }
            Sleep(1000);
            NumFailures += TestCase_MaskBlt(hWnd, hMonitorDC, TRUE);
            if(NumFailures > PreNumFailures)
            {
                NumFailedTests++;
                PreNumFailures = NumFailures;
            }
        }
    }

    //output the total num of failures on the window
    TCHAR text[100];
    memset(text, 0, 100*sizeof(TCHAR));
    StringCchPrintf(text, 100, TEXT("Totally %d test(s) failed and %d test case(s) failed"), NumFailedTests, NumFailures);

    HDC hTempDC = GetDC(hWnd);
    if(NULL != hTempDC)
    {
        RECT r;
        GetClientRect(hWnd, &r);
        UINT Width = r.right - r.left;
        UINT Height = r.bottom - r.top;

        HBRUSH hBrush, hOldBrush = NULL;
        hBrush = CreateSolidBrush(0x00805060);
        if(hBrush)
        {
            hOldBrush = (HBRUSH)SelectObject(hTempDC, hBrush);
        }
        BitBlt(hTempDC, 0, 0, Width, Height, NULL, 0, 0, PATCOPY);

        TextOut(hTempDC, 1, r.top/2+r.bottom/2, text, ARRAYSIZE(text) - 1);
        Sleep(4000);

        if(hOldBrush)
            SelectObject(hTempDC, hOldBrush);
        if(hBrush)
            DeleteObject(hBrush);
        ReleaseDC(hWnd, hTempDC);
    }
}
BOOL CALLBACK MonitorEnumProcExtended(
                                      HMONITOR hMonitor,  // handle to display monitor
                                      HDC hDCMonitor,     // handle to monitor DC
                                      LPRECT lprcMonitor, // monitor intersection rectangle
                                      LPARAM dwData)      // data
{
    if (hDCMonitor == NULL)
    {
        wprintf(L"hdcMonitor == NULL\n");
        return FALSE;
    }
    HWND hWnd = (HWND)dwData;

    DoRender(hWnd, hDCMonitor);
    return TRUE;
}
void ExecuteRender(HWND hWnd)
{
    //firstly check if device bitmaps are used 
    g_pMyLog->StartTest(TEXT("Check Status"));

    HRESULT hr = S_OK;
    BOOL bCompositionEnabled = FALSE; 
    hr = DwmIsCompositionEnabled(&bCompositionEnabled);
    bool bSkipped = false;

    if(SUCCEEDED(hr) && bCompositionEnabled)
    {
        HANDLE hDxSharedSurface = NULL;
        DXGI_FORMAT dxgiFormat = DXGI_FORMAT_UNKNOWN;
        UINT uiPresentFlags = 0;
        BOOL bDeviceBitmapsEnabled = FALSE;

        //we block the call and assume not to check the status of device bitmaps
        //The checking work is deferred later in CheckStatus()
        bDeviceBitmapsEnabled = TRUE;

        if (!bDeviceBitmapsEnabled)
        {
            g_pMyLog->Fail(TEXT("DeviceBitmap is not applied"));
            if(!g_bNoCheck)
            {
                g_pMyLog->EndTest();
                PostQuitMessage(0);
                return;
            }
        }

        int iResult = CheckPerfStatus(hWnd);
        if(iResult < 0)
        {
            g_pMyLog->Warn(_T("The perf number for BitBlt is low. Please double-check if device bitmaps are disabled."));
            if(!g_bNoCheck)
            {
                //PostQuitMessage(0);
                //return;
            }
        }
        else if(0 == iResult)
        {
            g_pMyLog->Warn(_T("The function for checking status failed. No performance numbers are available."));
        }
        else //perf number meet the expection for GDI HW ACC
        {
            g_pMyLog->Message(_T("DeviceBitmap is applied and GDI HW ACC is used."));
        }
    }
    else
    {
        g_pMyLog->Message(TEXT("DWM is OFF"));
        g_pMyLog->Skip();
        bSkipped = true;
        if(!g_bNoCheck)
        {
            PostQuitMessage(0);
            return;
        }
    }
    if(bSkipped == false)
    {
        g_pMyLog->EndTest();
    }
    bSkipped = false;


    //check if device bitmaps are used firstly
    g_pMyLog->StartTest(TEXT("Get Window DC"));
    HDC hDC = GetDC(hWnd);
    if (hDC == NULL)
    {
        g_pMyLog->Fail(TEXT("DC is NULL"));
        g_pMyLog->EndTest();
        PostQuitMessage(0);
        return;
    }

    g_pMyLog->EndTest();

    if(g_MonitorIndex >= 1 && g_bMonitorDC)
    {
        g_pMyLog->StartTest(TEXT("Running test on extended monitor"));
        if( !EnumDisplayMonitors(hDC, NULL, MonitorEnumProcExtended, (LPARAM)hWnd) )
        {
            g_pMyLog->Fail(TEXT("EnumDisplayMonitors failed to run the test cases"));
        }
        g_pMyLog->EndTest();
    }
    else
    {
        DoRender(hWnd, NULL);
    }

    if(NULL != hDC)
    {
        ReleaseDC(hWnd, hDC);
    }
    PostQuitMessage(0);
    return;

}
//-----------------------------------------------------------------------------
//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_COMMAND:
        wmId    = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_KEYDOWN:
        if (wParam == VK_DOWN || wParam == VK_UP)
        {
            hdc = BeginPaint(hWnd, &ps);
            ExecuteRender(hWnd);
            EndPaint(hWnd, &ps);
            break;
        }
        else
            if (wParam == VK_END)
            {
                g_bExit = TRUE;
            }
            break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        ExecuteRender(hWnd);
        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        if(NULL != g_pMyLog)
        {
            delete g_pMyLog;
        }
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
