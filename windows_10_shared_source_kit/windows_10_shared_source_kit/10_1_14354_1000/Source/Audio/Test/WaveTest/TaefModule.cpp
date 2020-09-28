/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/
#include "PreComp.h"

#include <windows.h>
#include <wextestclass.h>

using namespace WEX::Logging;
using namespace WEX::Common;

HRESULT g_hrCoInit = E_FAIL;

BEGIN_MODULE()
    MODULE_PROPERTY(L"Feature", L"Wave Test")
END_MODULE()

MODULE_SETUP(ModuleSetup);
MODULE_CLEANUP(ModuleCleanup);

bool ModuleSetup()
{
    bool fRet = false;

    g_hrCoInit = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if(FAILED(g_hrCoInit))
    {
        Log::Comment(String().Format(L"CoInitialize failed. Error = 0x%08x", g_hrCoInit));
        goto Exit;
    }

    fRet = true;
Exit:
    return fRet;
}

bool ModuleCleanup()
{
    if (g_hrCoInit != E_FAIL)
    {
        CoUninitialize();
    }
    return true;
}
