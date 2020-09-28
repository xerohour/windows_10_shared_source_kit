// Copyright (C) Microsoft. All rights reserved.
#pragma once

#include <windows.h>
#include <atlbase.h>
#include <atlstr.h>
#include <vector>
#include <list>
#include <strsafe.h>
#include <mmdeviceapi.h>

extern "C" 
{
#include <HidSdi.h>
#include <setupapi.h>
#include <SetupAPI_DevObj_DL.h>
}

#include <FormattedStr.h>
typedef CFormattedString CFS;
#include <HRString.h>
#include <BasicLog.h>

#include <BasicLogHelper.h>
#include <basicprintf.h>
#include <debug.h>
#include <helperclasses\cptr.h>

#include <mmdeviceapi.h>
#include <StressSafeFormattedString.h>


#define IF_FAILED_RETURN(logger, hr, msg, res) \
    if (FAILED(hr)) \
    { \
        LogMessageWithHr(logger, msg, hr); \
        return res; \
    } else { /*no-op*/ }
