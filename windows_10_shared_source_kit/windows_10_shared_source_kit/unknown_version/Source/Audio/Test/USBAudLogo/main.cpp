// Copyright (C) Microsoft. All rights reserved.
#include "stdafx.h"
HRESULT __stdcall myInitializeTestModule( ITestRunner* );

HINSTANCE g_hInst = NULL;

extern "C" BOOL WINAPI DllMain(HINSTANCE    hInstance,
                                   DWORD    dwReason,
                                   LPVOID   /*lpReserved*/)
{
    g_hInst = hInstance;

    switch(dwReason)
    {
    case DLL_PROCESS_ATTACH:
        break;
    default:
        break;
    }

    return TRUE;
}

INITTESTFRAMEWORK __stdcall InitializeTestModule( PVOID /*pData*/ )
{
	INITTESTFRAMEWORK initFramework = { &myInitializeTestModule }; 
	return initFramework;
}
