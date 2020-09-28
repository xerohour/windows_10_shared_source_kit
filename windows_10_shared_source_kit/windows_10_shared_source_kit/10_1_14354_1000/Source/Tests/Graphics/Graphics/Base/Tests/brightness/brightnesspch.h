#pragma once
#define _WIN32_DCOM

#include <windows.h>
#include <initguid.h>
#include <stdio.h>
#include <stdarg.h>
#include <tchar.h>
#include <wbemcli.h>
#include <Wbemidl.h>
#include <atlbase.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <xstring>
#include <strsafe.h>
#include <list>
#include <string.h>
#include <limits.h>
#include <wmidata.h>
#include <powrprof.h>
/*
#include <windows.h>
#include <wbemcli.h>
#include <atlbase.h>
#include <comdef.h>
#include <initguid.h>
#include <stdio.h>
#include <stdarg.h>
#include <tchar.h>
#include <Wbemidl.h>
#include <atlbase.h>
#include <wmidata.h>
#include <atlbase.h>
//#include <iomanip>
#include <vector>
#include <string>
#include <list>
#include <string.h>
*/

#define OFFSET_TO_PTR(Base, Offset) ((PBYTE)((PBYTE)Base + Offset))

using namespace std;

typedef vector<string> string_vector;

#include "CVidWttLog.h"
#include "BrightnessTest.h"
#include "dlist.h"

#define MAX_LOGLINE 1024
enum LOG_LEVEL {LOG_MSG = 1, LOG_WARNING, LOG_ERROR, LOG_KNOWN_BUG, LOG_START_TEST, LOG_END_TEST, LOG_SKIP_TEST};

enum TEST_TYPE {TT_UNINITIALIZED = 0, TT_WBEM, TT_CAPI, TT_LEGACY};

HRESULT Log(DWORD dwLevel, LPCTSTR szFormat, ... );

