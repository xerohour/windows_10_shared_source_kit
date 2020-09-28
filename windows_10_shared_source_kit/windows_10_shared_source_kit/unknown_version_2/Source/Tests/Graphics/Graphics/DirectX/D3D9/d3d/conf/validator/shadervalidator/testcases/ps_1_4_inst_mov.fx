PS_173_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_173_Desc = "ps_1_4 : mad write mask .r is allowed";
string PS_173 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mad r0.r, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_174_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_174_Desc = "ps_1_4 : mad write mask .g is allowed";
string PS_174 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mad r0.g, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_175_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_175_Desc = "ps_1_4 : mad write mask .b is allowed";
string PS_175 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mad r0.b, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_176_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_176_Desc = "ps_1_4 : mad write mask .a is allowed";
string PS_176 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mad r0.a, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_177_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_177_Desc = "ps_1_4 : mad write mask .rg is allowed";
string PS_177 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mad r0.rg, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_178_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_178_Desc = "ps_1_4 : mad write mask .gb is allowed";
string PS_178 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mad r0.gb, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_179_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_179_Desc = "ps_1_4 : mad write mask .ba is allowed";
string PS_179 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mad r0.ba, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_180_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_180_Desc = "ps_1_4 : mad write mask .rb is allowed";
string PS_180 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mad r0.rb, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_181_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_181_Desc = "ps_1_4 : mad write mask .ra is allowed";
string PS_181 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mad r0.ra, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_182_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_182_Desc = "ps_1_4 : mad write mask .ga is allowed";
string PS_182 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mad r0.ga, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_183_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_183_Desc = "ps_1_4 : mad write mask .rgb is allowed";
string PS_183 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mad r0.rgb, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_184_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_184_Desc = "ps_1_4 : mad write mask .gba is allowed";
string PS_184 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mad r0.gba, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_185_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_185_Desc = "ps_1_4 : mad write mask .rba is allowed";
string PS_185 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mad r0.rba, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_186_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_186_Desc = "ps_1_4 : mad write mask .rgba is allowed";
string PS_186 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mad r0.rgba, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_187_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_187_Desc = "ps_1_4 : mad write mask .yx is NOT allowed";
string PS_187 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mad r0.yx, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_188_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_188_Desc = "ps_1_4 : mad write mask .zx is NOT allowed";
string PS_188 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mad r0.zx, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_189_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_189_Desc = "ps_1_4 : mad write mask .zy is NOT allowed";
string PS_189 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mad r0.zy, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_190_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_190_Desc = "ps_1_4 : mad write mask .wx is NOT allowed";
string PS_190 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mad r0.wx, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_191_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_191_Desc = "ps_1_4 : mad write mask .wz is NOT allowed";
string PS_191 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mad r0.wz, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_192_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_192_Desc = "ps_1_4 : mad write mask .wy is NOT allowed";
string PS_192 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mad r0.wy, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_193_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_193_Desc = "ps_1_4 : mad write mask .zyx is NOT allowed";
string PS_193 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mad r0.zyx, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_194_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_194_Desc = "ps_1_4 : mad write mask .wzy is NOT allowed";
string PS_194 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mad r0.wzy, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_195_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_195_Desc = "ps_1_4 : mad write mask .wzx is NOT allowed";
string PS_195 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mad r0.wzx, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_196_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_196_Desc = "ps_1_4 : mad write mask .wyx is NOT allowed";
string PS_196 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mad r0.wyx, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_197_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_197_Desc = "ps_1_4 : mad write mask .yxw is NOT allowed";
string PS_197 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mad r0.yxw, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_198_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_198_Desc = "ps_1_4 : mad write mask .wzyx is NOT allowed";
string PS_198 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mad r0.wzyx, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_199_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_199_Desc = "ps_1_4 : mad write mask .zxwy is NOT allowed";
string PS_199 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mad r0.zxwy, v0, v1, r0 "
	"mov r0, c0 ";


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
    