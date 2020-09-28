// Copyright(C) Microsoft. All rights reserved.

//
// ActivateAudioInterfaceTests.cpp
//
// Description:
//
//  Simple ActivateAudioInterface tests for IAudioClient for the purposes of AppContainer verification
//  Comprehensive ActivateAudioInterface tests can be found in DeviceAPI Test

#include <stdafx.h>
#include <windowsstringp.h>
#include <wrl\implements.h>
#include <winrt\gitptr.h>

#include <windows.media.devices.h>


#include <avendpointkeys.h>
#include <CPropVariant.h>
#include "TestPal.h"

using namespace Microsoft::WRL;

using namespace Windows::Foundation;
using namespace Windows::Internal;
using namespace Windows::Media::Devices;

const UINT32 ACTIVATION_TIMEOUT = 1000; // 1 second

class CloseHandleOnExit
{
public:
    CloseHandleOnExit(HANDLE h)
    : m_h(h)
    {}
    
    ~CloseHandleOnExit()
    {
        CloseHandle(m_h);
    }
    
private:
    HANDLE m_h;
};

//
// Async activation completion delegate
//
class CActivationCompletionDelegate : public RuntimeClass< RuntimeClassFlags< ClassicCom >, FtmBase, IActivateAudioInterfaceCompletionHandler >
{
private:
    HANDLE                                          m_hCompletionEvent;
    CComPtr<IActivateAudioInterfaceAsyncOperation>  m_spOperation;

public:
    CActivationCompletionDelegate(HANDLE hCompletedEvent):m_hCompletionEvent(hCompletedEvent) {}
    ~CActivationCompletionDelegate() {}

    //  IActivateAudioInterfaceCompletionHandler
    STDMETHODIMP            ActivateCompleted(IActivateAudioInterfaceAsyncOperation *operation);

    // Other
    STDMETHODIMP            GetOperationInterface(IActivateAudioInterfaceAsyncOperation** ppOperation);
};


//-----------------------------------------------------------------------------
// Method: CActivationCompletionDelegate::ActivateCompleted()
//
// Description:
//     Callback method
//
// Returns S_OK on success, the appropriate HRESULT on failure
//
// ----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CActivationCompletionDelegate::ActivateCompleted(IActivateAudioInterfaceAsyncOperation *operation)
{
    if (nullptr == operation)
    {
        XLOG(XFAIL, eError, "FAIL: IActivateAudioInterfaceCompletionHandler::ActivateCompleted provided with NULL operation");
        return E_POINTER;
    }

    // cache the returned interface.  The main test is guaranteed NOT to run on a UI thread in shell98.
    m_spOperation.Attach(operation);

    SetEvent(m_hCompletionEvent);
    return S_OK;
}

//-----------------------------------------------------------------------------
// Method: CActivationCompletionDelegate::Release()
//
// Description:
//     Remove a reference to this object
//
// Parameters: N/A
//
// Return values (ULONG): The number of references to this object
//
// ----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CActivationCompletionDelegate::GetOperationInterface(IActivateAudioInterfaceAsyncOperation** ppOperation)
{
    if (nullptr == ppOperation)
    {
        return E_POINTER;
    }

    if (m_spOperation == nullptr)
    {
        return E_FAIL;
    }
    else
    {
        *ppOperation = m_spOperation;
        m_spOperation.Detach();
    }

    return S_OK;
}

//------------------------------------------------------------------------------
// Name: AsyncActivateOnEndpointUnderTest
// Desc: Helper function to asynchronously activate an interface by IID
//
// Returns:
//  S_OK on success, the appropriate HRESULT on failures.
//  ppActivatedInterface will point to the requested interface, be NULL on failure.
//------------------------------------------------------------------------------
HRESULT AsyncActivateOnEndpointUnderTest(REFIID  iid, IUnknown** ppActivatedInterface)
{
    HRESULT hr = S_OK;
    CComPtr<IMMDevice> spDeviceUnderTest;
    CComPtr<IPropertyStore> spPropStore;
    CPropVariant spv;

    if (NULL == ppActivatedInterface)
    {
        return E_POINTER;
    }

    *ppActivatedInterface = NULL;

        // get the endpoint
    hr = GetEndpointUnderTest(&spDeviceUnderTest);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "Error calling GetEndpointUnderTest (0x%8x) [%s]", hr, GetHRString(hr));
        return hr;
    }

    hr = spDeviceUnderTest->OpenPropertyStore(STGM_READ, &spPropStore);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "Error calling IMMDevice::OpenPropertyStore (0x%8x) [%s]", hr, GetHRString(hr));
        return hr;
    }
    
    hr = spPropStore->GetValue(PKEY_SWD_DeviceInterfaceId, &spv);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error calling IPropertyStore::GetValue(PKEY_SWD_DeviceInterfaceId) (0x%8x) [%s]", hr, GetHRString(hr));
        return hr;
    }
    
    if (VT_LPWSTR != spv.vt)
    {
        XLOG(XFAIL, eError, "FAIL: PROPVAR associated with PKEY_SWD_DeviceInterfaceId was not of type VT_LPWSTR");
        return E_FAIL;
    }

    // create a completion delegate object
    HANDLE hCompletionEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (NULL == hCompletionEvent)
    {
        XLOG(XFAIL, eError, "FAIL: Unable to create an event object");
        return E_FAIL;
    }

    CloseHandleOnExit closeCompletionEvent(hCompletionEvent);
    
    ComPtr<IActivateAudioInterfaceAsyncOperation> spOperation;
    ComPtr<CActivationCompletionDelegate> spCompletionObject;
    ComPtr<IActivateAudioInterfaceCompletionHandler> spCompletionHandler;

    spCompletionObject = Make<CActivationCompletionDelegate>(hCompletionEvent);
    if (spCompletionObject == nullptr)
    {
        XLOG(XFAIL, eError, "FAIL: Unable to make activation completion delegate");
        return E_FAIL;
    }
    
    hr = spCompletionObject.As(&spCompletionHandler);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to register async completion handler (0x%8x) [%s]", hr, GetHRString(hr));
        return hr;
    }

    // Activate the interface
    XLOG(XMSG, eInfo1, "Preparing to activate requested interface on SWD [%ws]", spv.pwszVal);

    hr = ActivateAudioInterfaceAsync(spv.pwszVal, 
                                     iid,
                                     nullptr,
                                     spCompletionHandler.Get(),
                                     spOperation.GetAddressOf());
    if (FAILED(hr))
    {
       XLOG(XFAIL, eError, "FAIL: Error calling ActivateAudioInterfaceAsync (0x%8x) [%s]", hr, GetHRString(hr));
       return hr;
    }

    // wait for the async operation to complete
    DWORD dwRet = WaitForSingleObjectEx(hCompletionEvent, ACTIVATION_TIMEOUT, FALSE);
    if (WAIT_OBJECT_0 != dwRet)
    {
        if (WAIT_TIMEOUT == dwRet)
        {
            XLOG(XFAIL, eError, "FAIL: TIMEOUT waiting for async activation.");
        }
        else
        {
            XLOG(XFAIL, eError, "FAIL: WaitForSingleObjectEx returned 0x%08x", dwRet);
        }

        // don't pull the event out from under the async call
        WaitForSingleObjectEx(hCompletionEvent, INFINITE, FALSE);

        return E_UNEXPECTED;
    }

    SLOG(eInfo1, "Aysnc activation complete");

    // get the interface and make sure it works
    ComPtr<IUnknown> spUnknown;
    HRESULT hrActivateResult = S_OK;

    hr = spCompletionObject->GetOperationInterface(spOperation.GetAddressOf());
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IActivateAudioInterfaceAsyncOperation from delegate (0x%8x) [%s]", hr, GetHRString(hr));
        return hr;
    }
    
    hr = spOperation->GetActivateResult(&hrActivateResult, spUnknown.GetAddressOf());
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error calling GetActivateResult (0x%8x) [%s]", hr, GetHRString(hr));
        return hr;
    }
    else if (FAILED(hrActivateResult))
    {
        XLOG(XFAIL, eError, "FAIL: Error activating interface (0x%08x) [%s]", hrActivateResult, GetHRString(hrActivateResult));
        return hrActivateResult;
    }

    hr = spUnknown.CopyTo(ppActivatedInterface);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error copying activated interface (0x%08x) [%s]", hr, GetHRString(hr));
        return hr;
    }
    
    return hr;
}

//------------------------------------------------------------------------------
// Name: TC_AudioClient_ActivateViaAudioInterfaceAsync
// Desc: Validate simple asynchronious IAudioClient activation via ActivateAudioInterface
//
// Returns:
//              FNS_PASS (failure is signalled by XLOG)
//------------------------------------------------------------------------------
DWORD TC_AudioClient_ActivateViaAudioInterfaceAsync()
{
    HRESULT hr = S_OK;
    PWAVEFORMATEX pwfx = nullptr;
    ComPtr<IUnknown> spUnknown;
    ComPtr<IAudioClient> spAudioClient;

    hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "Error calling CoInitializeEx (0x%8x) [%s]", hr, GetHRString(hr));
        return FNS_FAIL;
    }

    hr = AsyncActivateOnEndpointUnderTest(__uuidof(IAudioClient), spUnknown.GetAddressOf());
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error asynchronously activating interface.");
        CoUninitialize();
        return FNS_PASS;
    }

    hr = spUnknown.As(&spAudioClient);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error obtaining IAudioClient from activate result (0x%08x) [%s]", hr, GetHRString(hr));
        CoUninitialize();
        return FNS_PASS;
    }

    // ensure the IAudioClient interface actually works.
    hr = spAudioClient->GetMixFormat(&pwfx);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: IAudioClient::Initialize returned (0x%08x) [%s]", hr, GetHRString(hr));
        CoUninitialize();
        return FNS_PASS;
    }

    hr = spAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 500*1000*10, 0, pwfx, NULL);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: IAudioClient::Initialize returned (0x%08x) [%s]", hr, GetHRString(hr));
    }

    SAFE_COTASKMEMFREE(pwfx);
    CoUninitialize();
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Name: TC_ActivateAsync_AllApplicable
// Desc: Attempt to asynchronously activate all applicable interface
//
// Returns:
//              FNS_PASS (failure is signalled by XLOG)
//------------------------------------------------------------------------------
DWORD TC_ActivateAsync_AllApplicable()
{
    HRESULT hr = S_OK;
    ComPtr<IUnknown> spUnknown;

    hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "Error calling CoInitializeEx (0x%8x) [%s]", hr, GetHRString(hr));
        return FNS_FAIL; // we can't go any further
    }

    // IAudioClient
    SLOG(eInfo1, "Asynchronously activating IAudioClient");

    hr = AsyncActivateOnEndpointUnderTest(__uuidof(IAudioClient), spUnknown.GetAddressOf());
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error asynchronously activating IAudioClient.");
    }

    spUnknown = nullptr;

    // IAudioClient2
    SLOG(eInfo1, "Asynchronously activating IAudioClient2");

    hr = AsyncActivateOnEndpointUnderTest(__uuidof(IAudioClient2), spUnknown.GetAddressOf());
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error asynchronously activating IAudioClient2.");
    }

    spUnknown = nullptr;

    // IAudioEndpointVolume
    SLOG(eInfo1, "Asynchronously activating IAudioEndpointVolume");

    hr = AsyncActivateOnEndpointUnderTest(__uuidof(IAudioEndpointVolume), spUnknown.GetAddressOf());
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error asynchronously activating IAudioEndpointVolume.");
    }

    spUnknown = nullptr;

    if(!IsRunningOnMobile())
    {
        // IAudioMeterInformation
        SLOG(eInfo1, "Asynchronously activating IAudioMeterInformation");
        
        hr = AsyncActivateOnEndpointUnderTest(__uuidof(IAudioMeterInformation), spUnknown.GetAddressOf());
        if (FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Error asynchronously activating IAudioMeterInformation.");
        }

        spUnknown = nullptr;

        // IAudioSessionManager
        SLOG(eInfo1, "Asynchronously activating IAudioSessionManager");
        
        hr = AsyncActivateOnEndpointUnderTest(__uuidof(IAudioSessionManager), spUnknown.GetAddressOf());
        if (FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Error asynchronously activating IAudioSessionManager.");
        }

        spUnknown = nullptr;
    }
    
    CoUninitialize();

    return FNS_PASS;
}
