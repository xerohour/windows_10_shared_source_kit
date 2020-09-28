// Copyright(C) Microsoft.All rights reserved.

// aoac.cpp

#include <atlbase.h>
#include <atlstr.h>
#include <powrprof.h>
#include <strsafe.h>

#include <basiclog.h>
#include <BasicLogHelper.h>
#include <basicprintf.h>

#include <WexTestClass.h>

#include "log.h"
#include "aoac.h"

typedef LONG NTSTATUS;
#define STATUS_SUCCESS ((NTSTATUS)0)

using namespace WEX::Logging;
using namespace WEX::TestExecution;

HRESULT QueryAoAc(_Out_ bool *pbIsAoAc) {
    if (!VERIFY_IS_NOT_NULL(pbIsAoAc)) { return E_POINTER; }

    NTSTATUS nt;
    SYSTEM_POWER_CAPABILITIES systemPowerCapabilities = {0};
    
    if (!VERIFY_ARE_EQUAL(STATUS_SUCCESS, nt =
        CallNtPowerInformation(
            SystemPowerCapabilities,
            NULL,
            0,
            &systemPowerCapabilities,
            sizeof(SYSTEM_POWER_CAPABILITIES)
    ))) { return HRESULT_FROM_NT(nt); }
    
    *pbIsAoAc = (systemPowerCapabilities.AoAc != FALSE);
    return S_OK;
}
