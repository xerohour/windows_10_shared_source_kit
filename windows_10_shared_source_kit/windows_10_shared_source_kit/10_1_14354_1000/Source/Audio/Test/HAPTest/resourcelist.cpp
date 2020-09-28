// Copyright(C) Microsoft.All rights reserved.

// resourcelist.cpp

#include <atlbase.h>
#include <atlstr.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <strsafe.h>
#include <functiondiscoverykeys_devpkey.h>

#include <basiclog.h>
#include <BasicLogHelper.h>
#include <basicprintf.h>

#include <ITestResource.h>
#include <ResourceList.h>
#include <WexTestClass.h>
#include <devicetopology.h>

#include <debug.h>
#include <doonexit.h>
#include <offloadpath.h>
#include <taefdevice.h>
#include <taefdevicehelper.h>

#include "haptest.h"
#include "log.h"

using namespace WEX::Logging;
using namespace WEX::TestExecution;

class CCoUninitializeOnExit {
public:
    CCoUninitializeOnExit() {}
    ~CCoUninitializeOnExit() { CoUninitialize(); }
};

HRESULT __cdecl BuildResourceList(ResourceList& resourceList) {
    Log::Comment(L"Begin BuildResourceList");

    HRESULT hr = S_OK;

    // don't want to see VERIFY spew
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    if (!VERIFY_SUCCEEDED(hr = CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return hr; }
    CCoUninitializeOnExit cou;
 
    if (!VERIFY_SUCCEEDED(hr = 
        AddAllAudioMMDevices(eAll, TRUE, resourceList))) { return hr; }

    Log::Comment(L"End BuildResourceList");

    return S_OK;
}
