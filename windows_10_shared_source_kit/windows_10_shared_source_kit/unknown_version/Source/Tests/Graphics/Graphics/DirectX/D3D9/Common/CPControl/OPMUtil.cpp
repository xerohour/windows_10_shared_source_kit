//---------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------

#include "OPMControl.h"
#include <tchar.h>
#include <strsafe.h>
#include "OPMUtil.h"

TCHAR*   g_szOPMTestWindowClass = TEXT("OPM Test Window Class");

ATOM 
RegisterOPMTestWindowClass
(
    HINSTANCE hInstance
)
{
    WNDCLASSEX wcex = {0};

    wcex.cbSize = sizeof(WNDCLASSEX); 

    wcex.style			= CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc	= (WNDPROC)WndProc;
    wcex.cbClsExtra		= 0;
    wcex.cbWndExtra		= 0;
    wcex.hInstance		= hInstance;
    //wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_VAMTEST);
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszClassName	= g_szOPMTestWindowClass;
    //wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

    ATOM retVal = RegisterClassEx(&wcex);
    if (0 == retVal)
    {
        DWORD dwLastError = GetLastError();
        if (ERROR_CLASS_ALREADY_EXISTS == dwLastError)
            return 1;
    }
    return retVal;
}   //  RegisterVAMTestWndClass

DWORD
CreateOPMTestWindow
(
    HINSTANCE   hInstance, 
    TCHAR*      szWindowTitle,
    RECT*       pRect,
    HWND*       phWnd
)
{
    HWND        hWnd = 0;
    DWORD       dwErr = 0;

    if (0 == pRect)
    {
        //hWnd = CreateWindow(g_szOPMTestWindowClass, szWindowTitle, 
        //                    WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 
        //                    0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
        hWnd =CreateWindowExW
            (
                0,
                L"OPM Test Window Class",
                L"OPM Test window",
                WS_OVERLAPPEDWINDOW,
                0,
                0,
                640,
                480,
                NULL,
                NULL,
                hInstance,
                NULL
            );
    }
    if (0 == hWnd)
    {
        dwErr = GetLastError();
        return dwErr;
    }

    if (0 != phWnd)
        *phWnd = hWnd;

   ShowWindow(hWnd, SW_SHOWDEFAULT);
   UpdateWindow(hWnd);

   return dwErr;
}   //  CreateTestWindow

LRESULT CALLBACK 
WndProc
(
    HWND        hWnd, 
    UINT        message, 
    WPARAM      wParam, 
    LPARAM      lParam
)
{
    int         wmId = 0, wmEvent = 0;
    PAINTSTRUCT ps = {0};
    HDC         hdc = 0;

    return DefWindowProc(hWnd, message, wParam, lParam);
}   //  WndProc


HRESULT
ErrorCodeFromHResult
(
    const HRESULT   hr,
    TCHAR*          szErrorCode
)
{

    switch ( hr )
    {
    case DD_OK :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("S_OK"));
        break;

    case DDERR_DIRECTDRAWALREADYCREATED :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_DIRECTDRAWALREADYCREATED"));
        break;

    case DDERR_EXCLUSIVEMODEALREADYSET :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_EXCLUSIVEMODEALREADYSET"));
        break;

    case DDERR_GENERIC :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_GENERIC"));
        break;

    case DDERR_HWNDALREADYSET :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_HWNDALREADYSET"));
        break;

    case DDERR_HWNDSUBCLASSED :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_HWNDSUBCLASSED"));
        break;

    case DDERR_INVALIDDIRECTDRAWGUID :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_INVALIDDIRECTDRAWGUID"));
        break;

    case DDERR_INVALIDSURFACETYPE :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_INVALIDSURFACETYPE"));
        break;

    case DDERR_INCOMPATIBLEPRIMARY :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_INCOMPATIBLEPRIMARY"));
        break;

    case DDERR_INVALIDCAPS :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_INVALIDCAPS"));
        break;

    case DDERR_INVALIDOBJECT :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_INVALIDOBJECT"));
        break;

    case DDERR_INVALIDPARAMS :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_INVALIDPARAMS"));
        break;

    case DDERR_INVALIDPIXELFORMAT :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_INVALIDPIXELFORMAT"));
        break;

    case DDERR_INVALIDCLIPLIST :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_INVALIDCLIPLIST"));
        break;

    case DDERR_INVALIDRECT :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_INVALIDRECT"));
        break;

    case DDERR_NOALPHAHW :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_NOALPHAHW"));
        break;

    case DDERR_NOCLIPPERATTACHED :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_NOCLIPPERATTACHED"));
        break;

    case DDERR_NOCOOPERATIVELEVELSET :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_NOCOOPERATIVELEVELSET"));
        break;

    case DDERR_NODIRECTDRAWHW :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_NODIRECTDRAWHW"));
        break;

    case DDERR_NOEMULATION :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_NOEMULATION"));
        break;

    case DDERR_NOEXCLUSIVEMODE :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_NOEXCLUSIVEMODE"));
        break;

    case DDERR_NOFLIPHW :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_NOFLIPHW"));
        break;

    case DDERR_NOMIPMAPHW :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_NOMIPMAPHW"));
        break;

    case DDERR_NOOVERLAYHW :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_NOOVERLAYHW"));
        break;

    case DDERR_NOZBUFFERHW :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_NOZBUFFERHW"));
        break;

    case DDERR_NOBLTHW :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_NOBLTHW"));
        break;

    case DDERR_NOCLIPLIST :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_NOCLIPLIST"));
        break;

    case DDERR_NODDROPSHW :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_NODDROPSHW"));
        break;

    case DDERR_NOMIRRORHW :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_NOMIRRORHW"));
        break;

    case DDERR_NORASTEROPHW :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_NORASTEROPHW"));
        break;

    case DDERR_NOROTATIONHW :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_NOROTATIONHW"));
        break;

    case DDERR_NOSTRETCHHW :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_NOSTRETCHHW"));
        break;

    case DDERR_OUTOFMEMORY :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_OUTOFMEMORY"));
        break;

    case DDERR_OUTOFVIDEOMEMORY :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_OUTOFVIDEOMEMORY"));
        break;

    case DDERR_PRIMARYSURFACEALREADYEXISTS :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_PRIMARYSURFACEALREADYEXISTS"));
        break;

    case DDERR_SURFACEBUSY :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_SURFACEBUSY"));
        break;

    case DDERR_SURFACELOST :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_SURFACELOST"));
        break;

    case DDERR_UNSUPPORTED :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_UNSUPPORTED"));
        break;

    case DDERR_UNSUPPORTEDFORMAT :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_UNSUPPORTED"));
        break;

    case DDERR_UNSUPPORTEDMODE :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_UNSUPPORTEDMODE"));
        break;

    case DDERR_WASSTILLDRAWING :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_UNSUPPORTED"));
        break;

    case E_NOINTERFACE:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("E_NOINTERFACE"));
        break;

    case E_POINTER:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("E_POINTER"));
        break;

    case CO_E_NOTINITIALIZED:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("CO_E_NOTINITIALIZED"));
        break;

    case REGDB_E_CLASSNOTREG:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("REGDB_E_CLASSNOTREG"));
        break;

    case E_HANDLE:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("E_HANDLE"));
        break;

    case D3DERR_DRIVERINTERNALERROR:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("D3DERR_DRIVERINTERNALERROR"));
        break;

    case D3DERR_INVALIDCALL:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("D3DERR_INVALIDCALL"));
        break;

    case D3DERR_NOTAVAILABLE:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("D3DERR_NOTAVAILABLE"));
        break;

    case D3DERR_CANNOTPROTECTCONTENT:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("D3DERR_CANNOTPROTECTCONTENT"));
        break;

    case E_UNEXPECTED:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("E_UNEXPECTED"));
        break;


   // OPM - PVP etc
   case ERROR_GRAPHICS_OPM_NOT_SUPPORTED:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_NOT_SUPPORTED"));
        break;

   case ERROR_GRAPHICS_COPP_NOT_SUPPORTED:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_COPP_NOT_SUPPORTED"));
        break;

   case ERROR_GRAPHICS_UAB_NOT_SUPPORTED:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_UAB_NOT_SUPPORTED"));
        break;

   case ERROR_GRAPHICS_OPM_INVALID_ENCRYPTED_PARAMETERS:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_INVALID_ENCRYPTED_PARAMETERS"));
        break;

   case ERROR_GRAPHICS_PARAMETER_ARRAY_TOO_SMALL:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_PARAMETER_ARRAY_TOO_SMALL"));
        break;

   case ERROR_GRAPHICS_OPM_NO_VIDEO_OUTPUTS_EXIST:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_NO_VIDEO_OUTPUTS_EXIST"));
        break;

   case ERROR_GRAPHICS_NO_DISPLAY_DEVICE_CORRESPONDS_TO_NAME:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_NO_DISPLAY_DEVICE_CORRESPONDS_TO_NAME"));
        break;

   case ERROR_GRAPHICS_DISPLAY_DEVICE_NOT_ATTACHED_TO_DESKTOP:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_DISPLAY_DEVICE_NOT_ATTACHED_TO_DESKTOP"));
        break;

   case ERROR_GRAPHICS_MIRRORING_DEVICES_NOT_SUPPORTED:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_MIRRORING_DEVICES_NOT_SUPPORTED"));
        break;

   case ERROR_GRAPHICS_INVALID_POINTER:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_INVALID_POINTER"));
        break;

   case ERROR_GRAPHICS_OPM_INTERNAL_ERROR:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_INTERNAL_ERROR"));
        break;

   case ERROR_GRAPHICS_OPM_INVALID_HANDLE:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_INVALID_HANDLE"));
        break;
        
   case ERROR_GRAPHICS_NO_MONITORS_CORRESPOND_TO_DISPLAY_DEVICE:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_NO_MONITORS_CORRESPOND_TO_DISPLAY_DEVICE"));
        break;

   case ERROR_GRAPHICS_PVP_INVALID_CERTIFICATE_LENGTH:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_PVP_INVALID_CERTIFICATE_LENGTH"));
        break;
   
   case ERROR_GRAPHICS_OPM_SPANNING_MODE_ENABLED:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_SPANNING_MODE_ENABLED"));
        break;
   
   case ERROR_GRAPHICS_OPM_THEATER_MODE_ENABLED:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_THEATER_MODE_ENABLED"));
        break;
   
   case ERROR_GRAPHICS_PVP_HFS_FAILED:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_PVP_HFS_FAILED"));
        break;

   case ERROR_GRAPHICS_OPM_INVALID_SRM:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_INVALID_SRM"));
        break;

   case ERROR_GRAPHICS_OPM_OUTPUT_DOES_NOT_SUPPORT_HDCP:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_OUTPUT_DOES_NOT_SUPPORT_HDCP"));
        break;

   case ERROR_GRAPHICS_OPM_OUTPUT_DOES_NOT_SUPPORT_ACP:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_OUTPUT_DOES_NOT_SUPPORT_ACP"));
        break;

   case ERROR_GRAPHICS_OPM_OUTPUT_DOES_NOT_SUPPORT_CGMSA:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_OUTPUT_DOES_NOT_SUPPORT_CGMSA"));
        break;

   case ERROR_GRAPHICS_OPM_HDCP_SRM_NEVER_SET:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_HDCP_SRM_NEVER_SET"));
        break;

   case ERROR_GRAPHICS_OPM_RESOLUTION_TOO_HIGH:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_RESOLUTION_TOO_HIGH"));
        break;

   case ERROR_GRAPHICS_OPM_ALL_HDCP_HARDWARE_ALREADY_IN_USE:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_ALL_HDCP_HARDWARE_ALREADY_IN_USE"));
        break;

   case ERROR_GRAPHICS_OPM_VIDEO_OUTPUT_NO_LONGER_EXISTS:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_VIDEO_OUTPUT_NO_LONGER_EXISTS"));
        break;

   case ERROR_GRAPHICS_OPM_SESSION_TYPE_CHANGE_IN_PROGRESS:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_SESSION_TYPE_CHANGE_IN_PROGRESS"));
        break;

   case ERROR_GRAPHICS_OPM_DRIVER_INTERNAL_ERROR:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_DRIVER_INTERNAL_ERROR"));
        break;

   case ERROR_GRAPHICS_OPM_VIDEO_OUTPUT_DOES_NOT_HAVE_COPP_SEMANTICS:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_VIDEO_OUTPUT_DOES_NOT_HAVE_COPP_SEMANTICS"));
        break;

   case ERROR_GRAPHICS_OPM_INVALID_INFORMATION_REQUEST:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_INVALID_INFORMATION_REQUEST"));
        break;

   case ERROR_GRAPHICS_OPM_VIDEO_OUTPUT_DOES_NOT_HAVE_OPM_SEMANTICS:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_VIDEO_OUTPUT_DOES_NOT_HAVE_OPM_SEMANTICS"));
        break;

   case ERROR_GRAPHICS_OPM_SIGNALING_NOT_SUPPORTED:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_SIGNALING_NOT_SUPPORTED"));
        break;

   case ERROR_GRAPHICS_OPM_INVALID_CONFIGURATION_REQUEST:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_INVALID_CONFIGURATION_REQUEST"));
        break;

   default :
       //  attempt to get a message description from the system
       //  ugly solution, but works in a pinch; the error string has a \n
       //  at the end, which ruins our formatting. FormatMessage takes an
       //  escape sequence but the mechanism is very cumbersome and not
       //  platform independent (no 64 bit builds).
       DWORD dwcch = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, 
                                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                    szErrorCode, MAX_PATH, NULL);

       if (0 == dwcch)
           StringCchPrintf(szErrorCode, MAX_PATH, _T("unrecognized error code (0x%x)"), hr);
       else
       {
           //  dwcch is the number of chars, not including \0
           if(MAX_PATH - dwcch > 15)
               StringCchPrintf(szErrorCode+dwcch, MAX_PATH, _T("(0x%x)"), hr);
           else
               StringCchPrintf(szErrorCode+dwcch-15, MAX_PATH, _T("(0x%x)"), hr);

       }
        
    }   //  switch

    return hr;
}   //  ErrorCodeFromHResult

ULONG
ProtTypeIdxFromType
(
    const ULONG ProtType 
)
{
    ULONG   idx = 0;

    switch (ProtType)
    {
        case OPM_PROTECTION_TYPE_HDCP:
        {
            idx = OPM_PROTTYPE_IDX_HDCP;
            break;
        }
        case OPM_PROTECTION_TYPE_ACP:  
        {
            idx = OPM_PROTTYPE_IDX_ACP;
            break;
        }
        case OPM_PROTECTION_TYPE_CGMSA:
        {
            idx = OPM_PROTTYPE_IDX_CGMSA;
            break;
        }
        case OPM_PROTECTION_TYPE_COPP_COMPATIBLE_HDCP:
        {
            idx = OPM_PROTTYPE_IDX_COPP_COMPATIBLE_HDCP;
            break;
        }
        default:
        {
            idx = OPM_PROTTYPE_IDX_Invalid;
            break;
        }
    }

    return idx;
}   


void Msg(TCHAR *szFormat, ...)
{
    TCHAR szBuffer[1024];  // Large buffer for long filenames or URLs
    const size_t NUMCHARS = sizeof(szBuffer) / sizeof(szBuffer[0]);
    const int LASTCHAR = NUMCHARS - 1;

    // Format the input string
    va_list pArgs;
    va_start(pArgs, szFormat);

    // Use a bounded buffer size to prevent buffer overruns.  Limit count to
    // character size minus one to allow for a NULL terminating character.
    (void)StringCchVPrintf(szBuffer, NUMCHARS - 1, szFormat, pArgs);
    va_end(pArgs);

    // Ensure that the formatted string is NULL-terminated
    szBuffer[LASTCHAR] = TEXT('\0');

    // Display a message box with the formatted string
    MessageBox(NULL, szBuffer, TEXT("OPM Manual Tool"), MB_OK);
}


IDirect3D9 *CreateD3D9
(
    UINT uSDKVersion
)
{
    const WCHAR szD3D9[] = L"D3D9.DLL";
    const CHAR szDirect3DCreate[] = "Direct3DCreate9";

    typedef IDirect3D9* (WINAPI* LPDIRECT3DCREATE9)(UINT SDKVersion);

    static HINSTANCE hlibD3D9 = NULL;
    static LPDIRECT3DCREATE9 lpfnDirect3DCreate9 = NULL;

    if (lpfnDirect3DCreate9 == NULL) 
    {
        if (hlibD3D9 == NULL) 
        {
            // BUGBUG [SEdmison]:  This library reference is leaked, but I'm
            // not sure what the ramifications would be if I were to add a
            // FreeLibrary call within this function, so I'm leaving it alone
            // for the time being.
            hlibD3D9 = LoadLibraryExW(szD3D9, NULL, 0);
            if (hlibD3D9 == NULL) 
            {
                return NULL;
            }
        }
 
        lpfnDirect3DCreate9 = (LPDIRECT3DCREATE9)GetProcAddress(hlibD3D9, szDirect3DCreate);
        if (lpfnDirect3DCreate9 == NULL) 
        {
            return NULL;
        }
    }

    return (*lpfnDirect3DCreate9)(uSDKVersion);
}   //  CreateD3D9


HRESULT
CreateD3D9Device
(
    IDirect3D9*             pD3D9,
    const UINT              nIndex,
    HWND                    hDeviceWnd,
    const BOOL              bWindowed,
    IDirect3DDevice9**      ppD3D9Device
)
{
    HRESULT                 hr = S_OK;
    D3DPRESENT_PARAMETERS   PresentParams = {0};

    do
    {
        //  verify params
        if (NULL == pD3D9)
        {
            hr = E_INVALIDARG;
            break;
        }
        if (NULL == ppD3D9Device)
        {
            hr = E_POINTER;
            break;
        }

        //  check device type
        hr = pD3D9->CheckDeviceType(nIndex, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8,
                                    D3DFMT_X8R8G8B8, bWindowed);
        if (D3D_OK != hr)
            break;

        //  create device
        PresentParams.BackBufferWidth = 640;
        PresentParams.BackBufferHeight = 480;
        PresentParams.BackBufferFormat = D3DFMT_UNKNOWN;
        PresentParams.BackBufferCount = 0;
        PresentParams.MultiSampleType = D3DMULTISAMPLE_NONE;
        PresentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
        PresentParams.hDeviceWindow = hDeviceWnd;
        PresentParams.Windowed = bWindowed;
        PresentParams.Flags = 0;
        PresentParams.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

        hr = pD3D9->CreateDevice(nIndex, D3DDEVTYPE_HAL, NULL, 
                                D3DCREATE_MULTITHREADED | D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                &PresentParams, ppD3D9Device);

        if (FAILED(hr))
        {
            //  attempt a subsequent call, as some incorrect params might be corrected
            hr = pD3D9->CreateDevice(nIndex, D3DDEVTYPE_HAL, NULL, 
                                    D3DCREATE_MULTITHREADED | D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                    &PresentParams, ppD3D9Device);
        }

    }   while (FALSE);

    return hr;
}   //  CreateD3D9Device


/*
 *  function    :
 *  description :
 *  arguments   :
 *  returns     :
 *  notes       :
 */
HRESULT
CreateD3D9Ex
(
    UINT            uSDKVersion,
    IDirect3D9Ex**   ppD3D
)
{
    const WCHAR szD3D9[] = L"D3D9.DLL";
    const CHAR szDirect3DCreate[] = "Direct3DCreate9Ex";

    typedef HRESULT (WINAPI* LPDIRECT3DCREATE9EX)(UINT SDKVersion, IDirect3D9Ex** ppD3D);

    static HINSTANCE hlibD3D9 = NULL;
    static LPDIRECT3DCREATE9EX lpfnDirect3DCreate9Ex = NULL;

    if (lpfnDirect3DCreate9Ex == NULL)
    {
        if (hlibD3D9 == NULL)
        {
            // BUGBUG [SEdmison]:  This library reference is leaked, but I'm
            // not sure what the ramifications would be if I were to add a
            // FreeLibrary call within this function, so I'm leaving it alone
            // for the time being.
            hlibD3D9 = LoadLibraryExW(szD3D9, NULL, 0);
            if (hlibD3D9 == NULL)
            {
                DWORD LoadLibraryError = GetLastError();
                return HRESULT_FROM_WIN32(LoadLibraryError);
            }
        }

        lpfnDirect3DCreate9Ex = (LPDIRECT3DCREATE9EX)GetProcAddress(hlibD3D9, szDirect3DCreate);
        if (lpfnDirect3DCreate9Ex == NULL)
        {
            DWORD GetProcAddressError = GetLastError();
            return HRESULT_FROM_WIN32(GetProcAddressError);
        }
    }

    return (*lpfnDirect3DCreate9Ex)(uSDKVersion, ppD3D);
}   //  CreateD3D9Ex

/*
 *  function    :
 *  description :
 *  arguments   :
 *  returns     :
 *  notes       :
 */
HRESULT CreateD3D9DeviceEx
(
    IDirect3D9Ex*             pD3D9Ex,
    const UINT                nIndex,
    HWND                      hDeviceWnd,
    const BOOL                bWindowed,
    IDirect3DDevice9Ex**      ppD3D9DeviceEx
)
{
    HRESULT                 hr = S_OK;
    DWORD                   dwBehavior = 0;

    //  verify params
    if ((NULL == pD3D9Ex) || (NULL == ppD3D9DeviceEx))
    {
        hr = E_POINTER;
        return hr;
    }
    
    dwBehavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE;

    if ( bWindowed == TRUE )
    {
        D3DPRESENT_PARAMETERS WindowedPresentParameters;
        ZeroMemory( &WindowedPresentParameters, sizeof( WindowedPresentParameters ) );

        WindowedPresentParameters.Windowed = TRUE;
        WindowedPresentParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
        WindowedPresentParameters.SwapEffect =  D3DSWAPEFFECT_FLIPEX ;
        WindowedPresentParameters.EnableAutoDepthStencil = TRUE;
        WindowedPresentParameters.AutoDepthStencilFormat = D3DFMT_D16;
        WindowedPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
        WindowedPresentParameters.Flags = D3DPRESENTFLAG_RESTRICTED_CONTENT ;

        hr = pD3D9Ex->CreateDeviceEx
        (
            nIndex,
            D3DDEVTYPE_HAL,
            hDeviceWnd,
            dwBehavior,
            &WindowedPresentParameters,
            NULL,
            ppD3D9DeviceEx
        );

    }
    else
    {
        D3DDISPLAYMODEEX DisplayMode;
        DisplayMode.Size = sizeof(DisplayMode);
        if( FAILED( hr = pD3D9Ex->GetAdapterDisplayModeEx(0, &DisplayMode, NULL)) )
            return hr;        

        D3DPRESENT_PARAMETERS FullscreenPresentParameters;
        ZeroMemory( &FullscreenPresentParameters, sizeof( FullscreenPresentParameters ) );
        
        FullscreenPresentParameters.BackBufferWidth = DisplayMode.Width;
        FullscreenPresentParameters.BackBufferHeight = DisplayMode.Height;
        FullscreenPresentParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
        FullscreenPresentParameters.BackBufferCount = 0;
        FullscreenPresentParameters.MultiSampleType =  D3DMULTISAMPLE_NONE;
        FullscreenPresentParameters.SwapEffect = D3DSWAPEFFECT_FLIPEX ;
        FullscreenPresentParameters.hDeviceWindow = hDeviceWnd;
        FullscreenPresentParameters.Windowed = FALSE;		
        FullscreenPresentParameters.EnableAutoDepthStencil = TRUE;
        FullscreenPresentParameters.AutoDepthStencilFormat = D3DFMT_D16;        
        FullscreenPresentParameters.Flags = D3DPRESENTFLAG_RESTRICTED_CONTENT;
        FullscreenPresentParameters.FullScreen_RefreshRateInHz = 60;
        FullscreenPresentParameters.PresentationInterval = 0;//D3DPRESENT_INTERVAL_IMMEDIATE;

        D3DDISPLAYMODEEX FullscreenMode;
        ZeroMemory( &FullscreenMode, sizeof( FullscreenMode ) );

        FullscreenMode.Size = sizeof( FullscreenMode );
        FullscreenMode.Width = DisplayMode.Width;
        FullscreenMode.Height = DisplayMode.Height;
        FullscreenMode.RefreshRate = 60;
        FullscreenMode.Format = DisplayMode.Format;
        FullscreenMode.ScanLineOrdering = DisplayMode.ScanLineOrdering;

        hr = pD3D9Ex->CreateDeviceEx
        (
            nIndex,
            D3DDEVTYPE_HAL,
            hDeviceWnd,
            dwBehavior,
            &FullscreenPresentParameters,
            &FullscreenMode,//&m_DisplayMode,//
            ppD3D9DeviceEx
        );
    }

    return hr;
} //CreateD3D9DeviceEx