// Copyright (C) Microsoft Corporation. All rights reserved.

#include <windows.h>
//#include <collection.h>
#include <roapi.h>
//#include <ppltasks.h>
#include <stdio.h>
#include <windows.h>
// Test Support Headers
#include <ILog.h>
#include <BasicLogHelper.h>
#include "WexTestClass.h"
#include "basiclog.h"
using namespace WEX::Logging;
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <strsafe.h>

#define LOG_ERROR(fmt, ...) Log::Error(WEX::Common::String().Format(fmt, __VA_ARGS__))


// 
// Managed
//




#define LOG(format, ...) wprintf(format L"\n", __VA_ARGS__)

class RoUninitializeOnExit {
public:
    RoUninitializeOnExit() {}
    ~RoUninitializeOnExit() { RoUninitialize(); }
};

class CoTaskMemFreeOnExit {
public:
    CoTaskMemFreeOnExit(PVOID p) : m_p(p) {}
    ~CoTaskMemFreeOnExit() { CoTaskMemFree(m_p); }
private:
    PVOID m_p;
};

