/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/

#include "precomp.h"

HRESULT g_hrCoInit = E_FAIL;

BEGIN_MODULE()
    MODULE_PROPERTY(L"Feature", L"Voice Activation Manager")
END_MODULE()

MODULE_SETUP(ModuleSetup);
MODULE_CLEANUP(ModuleCleanup);

bool ModuleSetup()
{
    bool fRet = false;

    g_hrCoInit = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
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
