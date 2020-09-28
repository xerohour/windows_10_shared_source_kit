//*****************************************************************************
// Copyright (C) Microsoft Corporation. All rights reserved.
//
// EffectsDiscoveryCPPTestCore.h
//*****************************************************************************
#pragma once
#pragma warning (disable: 4451)


#include <tchar.h>
#include <winstring.h>//
#include "mfapi.h"
#include <d3d11.h>
#include <dcomp.h>
#include <agile.h>
#include <ppltasks.h>
#include "stringify.h"
#include "enter.h"
// TAEF
#include <WexTestClass.h>
#include <wexstring.h>
#include <collection.h>
#include "CPropVariant.h"

// ATL stuff
#include <atlbase.h>
#include <atlcom.h>
#include <atlcoll.h>
#include <atlsync.h>
#include <atlconv.h>

//stl stuff for app launcher
#include <stdio.h>
#include <vector> 
#include <set>
#include <Tlhelp32.h>
#include <algorithm>
#include <iterator>



// Test Support Headers
#include <ILog.h>
#include <BasicLogHelper.h>
#include "WexTestClass.h"
#include "basiclog.h"


#include "ModernAppCommunications.h"
#include <shlobj.h>
#include <stdio.h>
#include <shobjidl.h>
#include <objbase.h>
#include <string>
#include <MmdeviceapiP.h>
#include <MMdeviceapi.h>
#include <AudioPolicyP.h>

// ATL stuff
#include <atlbase.h>
#include <atlcom.h>
#include <atlcoll.h>
#include <atlsync.h>
#include <atlconv.h>

//stl stuff for app launcher
#include <stdio.h>
#include <vector> 
#include <set>
#include <Tlhelp32.h>
#include <algorithm>
#include <iterator>



// Test Support Headers
#include <ILog.h>
#include <BasicLogHelper.h>
#include "WexTestClass.h"
#include "basiclog.h"

//Version Info stuff
#include <winver.h>
#include <windows.h>
#include <wrl\module.h>
#include <wrl\event.h>
#include "ModernAudioTestEnums.h"
#include <Strsafe.h>

using namespace WEX::Common;
using namespace WEX::Logging;
using namespace WEX::TestExecution;
using namespace Platform;
using namespace Platform::Collections;
using namespace Microsoft::WRL;

using namespace Windows::Media;
using namespace Windows::Media::Capture;
using namespace Windows::Media::Devices;
using namespace Windows::Media::MediaProperties;
using namespace Windows::Foundation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Storage::FileProperties;

using namespace Windows::Devices;
using namespace Windows::Devices::Enumeration;

using namespace Windows::Media::Effects;

using namespace Concurrency; // For async operations PPL


#define LOG_OUTPUT(fmt, ...) Log::Comment(WEX::Common::String().Format(fmt, __VA_ARGS__))
#define LOG_ERROR(fmt, ...) Log::Error(WEX::Common::String().Format(fmt, __VA_ARGS__))
#define PKeyAudioDeviceGUID L"{1da5d803-d492-4edd-8c23-e0c0ffee7f0e} 4"


class CClientLauncher
{
public:
    CClientLauncher();    
    HRESULT LaunchApp(LPWSTR pwszAppId);
    void TerminateAp(LPWSTR pwszAppId);
private:
    DWORD m_pid;    
    Microsoft::WRL::ComPtr<IApplicationActivationManager> m_pActivationManager; m_pActivationManager;
};

class CArrivalNotificationHandler: public IMACClientActivityNotify
{
public:
    CArrivalNotificationHandler::CArrivalNotificationHandler():m_cRef(1)
    {
	m_effectList = ref new Vector<Platform::String^>();
        m_hWatcherReady = CreateEvent(NULL, TRUE, FALSE, NULL);
       
        return;
    }
    
    //
    // IUnknown
    //
    ULONG STDMETHODCALLTYPE AddRef()
    {
        return InterlockedIncrement(&m_cRef);
    }

    ULONG STDMETHODCALLTYPE Release()
    {
        ULONG cTemp = InterlockedDecrement(&m_cRef);

        if (0 == cTemp)
        {
            delete this;
        }

        return cTemp;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid,
                                             VOID **ppvObject)
    {
        if (NULL == ppvObject)
        {
            return E_POINTER;
        }

        *ppvObject = NULL;

        if ((__uuidof(IUnknown) == iid)||
            (__uuidof(IMACClientActivityNotify) == iid))
        {
            *ppvObject = (IMACClientActivityNotify*)(this);
            AddRef();
            return S_OK;
        }
        else
        {
            return E_NOINTERFACE;
        }
    }

    //
    // IMACClientActivityNotify
    //
    HRESULT STDMETHODCALLTYPE OnClientArrival(LPCWSTR wszArrivedClient, UINT nActiveClients);
    
    HRESULT STDMETHODCALLTYPE OnClientRemoval(LPCWSTR wszRemovedClient, UINT nActiveClients);

    HRESULT STDMETHODCALLTYPE OnClientNotification(LPCWSTR wszSourceClient, LPCWSTR wszNotificationDescription, LPCWSTR wszNotificationContents);

    HANDLE GetWatcherHandle();

    Vector<Platform::String^>^ GetEffectsListInNotification();
	
    void ClearNotifications() { m_effectList->Clear(); };


    
private:
    LONG    m_cRef;
    HANDLE  m_hWatcherReady;
	Vector<Platform::String^>^ m_effectList;   

};

class CEffectsDiscoveryCPPTestCore
{
public:
    enum Verification
    {
        VerifyLoudnessEq = 0,
        VerifyDisabled
    };

    CEffectsDiscoveryCPPTestCore();
    ~CEffectsDiscoveryCPPTestCore();
    void EnumerateCaptureDevices(BOOL bVideo, DeviceInformationCollection^* ppList);
    void EnumerateRenderDevices(DeviceInformationCollection^* ppList);
    bool ReadRawSupportSettings(Platform::String ^audioDeviceID);


    IVectorView<AudioEffect^>^ ListEffectsCapture(
    Windows::Media::Effects::AudioCaptureEffectsManager ^manager,
    Windows::Devices::Enumeration::DeviceInformation ^device,
    Windows::Media::Capture::MediaCategory category,
    Windows::Media::AudioProcessing mode);

    
    IVectorView<AudioEffect^>^ ListEffectsRender(
    Windows::Media::Effects::AudioRenderEffectsManager ^manager,
    Windows::Devices::Enumeration::DeviceInformation ^device,
    Windows::Media::Render::AudioRenderCategory category,
    Windows::Media::AudioProcessing mode);

    void ListEffectsAndVerifyCapture(
    Windows::Media::Effects::AudioCaptureEffectsManager ^manager,
    Windows::Devices::Enumeration::DeviceInformation ^device,
    Windows::Media::Capture::MediaCategory category,
    Windows::Media::AudioProcessing mode);

    void ListEffectsAndVerifyRender(
    Windows::Media::Effects::AudioRenderEffectsManager ^manager,
    Windows::Devices::Enumeration::DeviceInformation ^device,
    Windows::Media::Render::AudioRenderCategory category,
    Windows::Media::AudioProcessing mode);

    void VerifyEffectListReturned(IVectorView<AudioEffect^> ^list, Windows::Media::AudioProcessing mode);
    void VerifyEffectListOnDisable(IVectorView<AudioEffect^> ^list);    
    void VerifyEffectListOnDisable(Platform::Collections::Vector<AudioEffectType> ^list);
    void VerifyRawSupport(DeviceInformation ^pDevice);

    void LogException(Platform::Exception^ e);
    bool SetDisableSystemEffectsSetting( bool bDisableSysFx, EDataFlow flow, LPCWSTR  deviceId,CPropVariant* storevar);
    bool RestoreSystemEffectsSetting(CPropVariant var, EDataFlow flow,  LPCWSTR  deviceId);   
    bool NameContainsHF(const wchar_t *pDeviceName);    
    bool RestoreLoudnessEq(CPropVariant storevar, EDataFlow flow, LPCWSTR  deviceId);
    bool ChangeLoudnessEq(EDataFlow flow, LPCWSTR  deviceId,CPropVariant* storevar);
    bool CheckforPresenceOfWMALFXGFXDSP(EDataFlow flow, LPCWSTR  deviceId);    
    bool GetIMMDevice(LPCWSTR  deviceId, EDataFlow flow, IMMDevice **ppIMMDevice);
    bool LookForWMALFXGFX(LPWSTR pstrId, REFPROPERTYKEY key);
    void ProcessCaptureEffectsList(const  wchar_t* effectsList);
    void ProcessRenderEffectsList(const  wchar_t* effectsList, CPropVariant var, Verification eVerType);
    bool SetupCommunications(EDataFlow flow);
    bool TeardownCommunications();
    CArrivalNotificationHandler* GetNotificationHandler();
    void StopListening(EDataFlow flow);
    void VerifyNotificationsLoudnessEq(Platform::Collections::Vector<AudioEffectType> ^list, 
        Windows::Media::Render::AudioRenderCategory category,
        Windows::Media::AudioProcessing mode,
        CPropVariant var
        );
    HRESULT RestartMACService();
    void VerifyNotificationsReceived();
    void AddtoNotificationRenderCategory(Windows::Media::Render::AudioRenderCategory category);
    void ClearPresentCategories() { m_presentCategories->Clear(); };
private:
	//Communications variables
    CComPtr<IModernAppCommunications> m_spCommunicationsFramework;
    CComPtr<IMACServer> m_spCommServer;
    CComPtr<IRpcBasicLog> m_spRemoteLogger;
    CComPtr<IMACClientActivityNotify> m_spClientNotify;
    CArrivalNotificationHandler *m_pNotificationHandler;
    CComPtr<IBasicLog> m_spLog;  
    CClientLauncher *m_pLauncher;
    HANDLE m_hHandle;
    Platform::Collections::Vector<Windows::Media::Render::AudioRenderCategory> ^m_presentCategories;
};

