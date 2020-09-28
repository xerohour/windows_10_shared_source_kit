/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/


#include <windows.h>
#include <tchar.h>
#include <shell98.h>
#include <module98.h>
#include <dvmodule.h>
#include <cfgmgr32.h>
#include <setupapi.h>
#include <atlstr.h>
#include <stdio.h>
#include <debug.h>
#define USERMODE // or azcodec.h includes wdm.h (kernel mode version of windows.h)
#include <azcodec.h>
#include <privioct.h>
#include <controller-interface.h>

#include "install.h"
#include "uaatest.h"
#include "buffer.h"
#include "log.h"

void SLOG(UINT eLevel, __format_string LPCTSTR ptszFormat, ...) {

    // boring stuff to transform the format string and the args into a message...
    va_list arg_ptr;
    va_start(arg_ptr, ptszFormat);

    int iLen = _vsctprintf(ptszFormat, arg_ptr); // doesn't include _T('\0')
    if (iLen < 0 || iLen + 1 <= 0) { // watch out for the wraparound...
        ASSERT(FALSE); // boom!
        return;
    }

    // CBuffer will self-allocate the memory, and free it when it goes out of scope
    CBuffer<TCHAR> ptszMessage(iLen + 1); // iLen + 1 because we need space for _T('\0')
    if (NULL == ptszMessage) { // out of memory
        ASSERT(FALSE); // boom!
        return;
    }

    int iCopied = _vstprintf_s(ptszMessage, iLen + 1, ptszFormat, arg_ptr); // length of buffer = length of string + 1
    if (iCopied != iLen) {
        ASSERT(FALSE); // boom!
        return;
    }
    // ok, boring stuff over

    // TODO: This app needs to be updated to use XLOGTYPE enumerations, rather than eError/eBlah
    if (LogLevel_Result == eLevel) {
        // eError and eFatalError map to this
        g_IShell->XLog(XFAIL, eLevel, _T("%s"), ptszMessage);
    }
    else {
        // eInfo*, eBlab* map to this.
        // eWarn* will also end up here, so if you want to log a warning use XLOG instead of SLOG
        g_IShell->Log(eLevel, _T("%s"), ptszMessage);
    }
}
