/*
 *  file        :   Helpers
 *  description :   declarations
 *  history     :   06/04/2005	mehmetk		created 
 */
#pragma once

extern  TCHAR*  g_szOPMTestWindowClass;

#ifndef BREAK_ON_FAILURE
#define BREAK_ON_FAILURE(actual_code)               \
        if (TEST_RESULT_PASS != (actual_code))      \
        {                                           \
            break;                                  \
        }
#endif  //  BREAK_ON_FAILURE

//  other helpers
ATOM        RegisterOPMTestWindowClass(HINSTANCE);
DWORD
CreateOPMTestWindow
(
    HINSTANCE   hInstance, 
    TCHAR*      szWindowTitle,
    RECT*       pRect,
    HWND*       phWnd
);
HRESULT
ErrorCodeFromHResult
(
    const HRESULT   hr,
    TCHAR*          szErrorCode
);
IDirect3D9* CreateD3D9(UINT uSDKVersion);
HRESULT     CreateD3D9Device
(
    IDirect3D9*             pD3D9,
    const UINT              nIndex,
    HWND                    hDeviceWnd,
    const BOOL              bWindowed,
    IDirect3DDevice9**      ppD3D9Device
);

HRESULT CreateD3D9Ex
        (
            UINT            uSDKVersion,
            IDirect3D9Ex**   ppD3D
        );

HRESULT CreateD3D9DeviceEx
        (
            IDirect3D9Ex*             pD3D9,
            const UINT                nIndex,
            HWND                      hDeviceWnd,
            const BOOL                bWindowed,
            IDirect3DDevice9Ex**      ppD3D9Device
        );

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void Msg(TCHAR *szFormat, ...);

//  cleanup functions
#define Safe_Release(pT)    \
    if (NULL != (pT))       \
    {                       \
        (pT)->Release();    \
        (pT) = NULL;        \
    }

#define Safe_Delete(pT)     \
    if (NULL != (pT))       \
    {                       \
        delete (pT);        \
        (pT) = NULL;        \
    }

#define Safe_Delete_Array(pT)   \
    if (NULL != (pT))           \
    {                           \
        delete [] (pT);         \
        (pT) = NULL;            \
    }

#define Safe_Close(pH)      \
    if (NULL != (pH))       \
    {                       \
        CloseHandle(*(pH)); \
        *(pH) = NULL;       \
    }

#define Safe_CoTaskMemFree(pT)  \
    if (NULL != (pT))           \
    {                           \
        CoTaskMemFree(pT);      \
        (pT) = NULL;            \
    }

#define PROTECTION_TYPES    4

#define OPM_ProtectionType_Mask 0x8000000F
#define OPM_PROTTYPE_IDX_HDCP  0
#define OPM_PROTTYPE_IDX_ACP   1
#define OPM_PROTTYPE_IDX_CGMSA 2
#define OPM_PROTTYPE_IDX_COPP_COMPATIBLE_HDCP 3
#define OPM_PROTTYPE_IDX_Invalid   0xFF

ULONG   ProtTypeIdxFromType(const ULONG ProtType);
