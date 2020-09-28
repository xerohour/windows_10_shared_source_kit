// Copyright(C) Microsoft.All rights reserved.

// registry.cpp

#include <atlbase.h>
#include <atlstr.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <avendpointkeys.h>
#include <endpointvolume.h>
#include <strsafe.h>

#include <basiclog.h>
#include <BasicLogHelper.h>
#include <basicprintf.h>
#include <WexTestClass.h>

#include <debug.h>
#include <doonexit.h>

#include "haptest.h"
#include "log.h"

using namespace WEX::Logging;
using namespace WEX::TestExecution;

void CHapTest::OffloadFormatControl() {
    // don't want to see VERIFY spew
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    CComPtr<IBasicLog> pLogger;
    if (!VERIFY_SUCCEEDED(CreateWexBasicLog(&pLogger))) { return; }

    CComPtr<IMMDevice> spMMDevice;
    if (!DeviceUnderTest(pLogger, &spMMDevice)) { return; }

    CComPtr<IPropertyStore> spPropertyStore;
    if (!VERIFY_SUCCEEDED(spMMDevice->OpenPropertyStore(STGM_READ, &spPropertyStore) ) ) { return; }

    PROPVARIANT var; PropVariantInit(&var);
    DoOnExit<PROPVARIANT *> clearVar(PropVariantClear, Success_NonNegative, &var, pLogger, L"clear PROPVARIANT");
    if (!VERIFY_SUCCEEDED(spPropertyStore->GetValue(PKEY_Endpoint_OffloadFormatControl, &var))) { return; }

    // the key should not even exist
    if (var.vt != VT_EMPTY) {
        ERR(L"Format Control Key should not be set.  Variant type: %u.", var.vt);

        if (var.vt == VT_UI4) {
            LOG(L"Value of Offload Format Control key: 0x%08x", var.ulVal);
        }
    }
}
