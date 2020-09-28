/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <tchar.h>
#include <atlstr.h>
#include <shell98.h>
#include <module98.h>
#include <dvmodule.h>
#include <cfgmgr32.h>
#include <setupapi.h>
#include <hitframework.h>

// the Azalia includes (privioct.h, azcodec.h, et al.)
// will "helpfully" include windows.h for you (fine)
// ... but only if you #define USERMODE before #include them
// ... if you don't #define USERMODE,
// they will "helpfully" include wdm.h
// which is for kernel-mode things
// so you get doubly-defined compilation errors
#define USERMODE
#include <azcodec.h>
#include <privioct.h>
#include <controller-interface.h>

#include "aznode.h"
#include "azverbutils.h"
#include "install.h"
#include "uaatest.h"
#include "HdRegTest.h"
#include "HdHitLog.h"
#include "HdHitPlugin.h"
#include "hdaudiomemoryspace.h"
#include "controller.h"
#include "power.h"
#include "codec.h"
#include "util.h"
#include "log.h"
#include "HDAudioDev.h"

DWORD CollectAFGInfo()
{
    WCHAR LogFullFileName[MAX_PATH];
    WCHAR FileName[MAX_PATH];
    CCodec* pCodec = (CCodec*)(g_pUAATest->m_pCurDevice);
    USHORT sdi = pCodec->m_usSDI;

    if (DT_CODEC == pCodec->m_lpType)
    {
        MultiByteToWideChar(CP_ACP, 0, g_pShell->m_szSourcePath, -1, LogFullFileName, MAX_PATH);
        swprintf_s(FileName, MAX_PATH, L"\\AzaliaSDI%d.dat", sdi);
        wcscat_s( LogFullFileName, MAX_PATH, FileName );

        DumpFunctionGroupInfoForSDI(sdi, LogFullFileName, pCodec->m_hDriver);
    }

    return FNS_PASS;
}

bool StringEndsWith(LPCSTR haystack, LPCSTR needle)
{
    if (strlen(haystack) < strlen(needle))
    {
        return false;
    }
    
    return (0 == _stricmp(haystack + strlen(haystack) - strlen(needle), needle));
}