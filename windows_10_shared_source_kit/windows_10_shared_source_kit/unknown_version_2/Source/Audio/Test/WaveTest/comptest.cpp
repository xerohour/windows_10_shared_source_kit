// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft Corporation, 2006
//
// Module Name:
//  comptest.cpp
//
// Abstract:
//  Implementation of general compliance tests
//
// History:						
//  2006.08.24 - jwexler - created
//  2013.03.06 - mimcel - Header clean up
//
// -------------------------------------------------------------------------------

#include "PreComp.h"

#include "WaveTestTaef.h"

#include "tests.h"
#include <atlstr.h>
#include <endpointcount.h>
#include <powrprof.h>

#include <setupapi.h>
#include <devpkey.h>

#include <helperclasses/cptr.h>
#include <TestDevice.h>
#include <TestDevNode.h>
#include <AudioDevNodeSet.h>
#include <hrstring.h>
#include <CPropvariant.h>
#include <QueryMachineInfo.h>
using namespace QueryMachineInfo;

#include <DevTrans.h>

using namespace DevTrans;

using namespace AudioDevNode;

#include <AudioOffload.h>
#include <propkey.h>
#include <avendpointkeys.h>

using namespace WEX::Common;
using namespace WEX::TestExecution;

extern IBasicLog * g_pBasicLog;

class CPinInitialize
{
public:
    CPinInitialize
    (
        CPCMAudioPin       *pPin,
        WAVEFORMATEX       *pWfx,
        BOOL                fLooped,
        BOOL               &fRes
    )
        : m_pPin(pPin) 
    { 
        if (NULL == m_pPin) { fRes = FALSE; }
        fRes = InstantiatePinWfx(m_pPin, pWfx, fLooped); 
    }
    
    ~CPinInitialize()
    {
        m_pPin->SetState(KSSTATE_STOP);
        m_pPin->ClosePin();
    }
private: 
    CPCMAudioPin *m_pPin;
};

typedef LONG NTSTATUS;
#define STATUS_SUCCESS ((NTSTATUS)0)

HRESULT IsSystemAOAC(bool &bIsAOAC) 
{
    SYSTEM_POWER_CAPABILITIES systemPowerCapabilities = {0};
    
    NTSTATUS nt = CallNtPowerInformation
    (
        SystemPowerCapabilities,
        NULL,
        0,
        &systemPowerCapabilities,
        sizeof(SYSTEM_POWER_CAPABILITIES)
    );
    
    if (STATUS_SUCCESS != nt)
    { 
        XLOG(XFAIL, 1, "Failed CallNtPowerInformation");
        return HRESULT_FROM_NT(nt); 
    }
    
    bIsAOAC = (systemPowerCapabilities.AoAc != FALSE);
    return S_OK;
}

HRESULT FindInstanceIDInDeviceSet(ITestDeviceSet *pDeviceSet, LPCSTR szInstanceID, bool &fFound, bool &isBth)
{
    HRESULT hr = S_OK;
    if (NULL == pDeviceSet)
    {
        return E_POINTER;
    }

    UINT cDevices = 0;
    hr = pDeviceSet->GetCount(&cDevices);
    if (FAILED(hr)) { return hr; }

    fFound = false;
    isBth = false;
    for (DWORD dwIdx = 0; dwIdx < cDevices; dwIdx++)
    {   
        CComPtr<ITestDevice> pDevice;
        hr = pDeviceSet->GetAt(dwIdx, &pDevice);
        if (FAILED(hr))
        {
            XLOG(XFAIL, 1, "Failed getting device at index %d (hr = %s)", dwIdx, GetHRString(hr));
            return hr;
        }

        CComHeapPtr<WCHAR> szCurrentInstanceID;
        hr = pDevice->GetId(&szCurrentInstanceID);
        if (FAILED(hr))
        {
            XLOG(XFAIL, 1, "Failed getting device ID at index %d (hr = %s)", dwIdx, GetHRString(hr));
            return hr;
        }

        if (CStringA(szCurrentInstanceID) == szInstanceID)
        {
            fFound = true;
            XLOG(XMSG, 10, "The device:%s is found", szInstanceID);
            // Determine if device is Bluetooth...
            CComPtr<ITestDevNode> pAudioDevNode;
            hr = pDevice->QueryInterface(__uuidof(ITestDevNode), (void**)&pAudioDevNode);
            if (FAILED(hr))   
            {   
                XLOG(XFAIL, eError, "Failed QIing for AudioDevice with hr=0x%08x", hr);  
                return hr;   
            }     

            GUID guidAudioDevNodeType;            
            hr = pAudioDevNode->GetType(&guidAudioDevNodeType);
            if (FAILED(hr))   
            {   
                XLOG(XFAIL, eError, "Failed Getting DevNode type with hr=0x%08x", hr);  
                return hr;   
            }  
  
            if (GUID_AUDDEVNODE_BLUETOOTH == guidAudioDevNodeType)  
            {  
                isBth = true;  
            }  

            break; 
       }
    }
    return S_OK;
}


// ------------------------------------------------------------------------------
// Test_EnumerateAudioEndpoint()
// ------------------------------------------------------------------------------
DWORD 
Test_EnumerateAudioEndpoint() // find all audio devices and count the number of active render/capture endpoints on the current device
{
//#ifndef BUILD_TAEF //we're originally not building this test for mc
//however with this implementation it might be possible to actually build it
//We'll leave this to skip for now to get the FI resolved and will attempt to
//build after the changes FI
    HRESULT hr = S_OK;
    LPCSTR szInstanceID = g_pWaveTest->m_pHalf->m_pFilter->m_szInstanceID;

    CComPtr<IMMDevice> pDevnode;
    hr = InstanceIdToDevnodeA(szInstanceID, &pDevnode);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "Failed to create devnode object from device instance ID %s (hr = %s)", szInstanceID, GetHRString(hr));
        return FNS_FAIL;
    }

    CComPtr<IMMDeviceCollection> pCollection;
    hr = GetDevnodeEndpoints(pDevnode, DEVICE_STATE_ACTIVE, &pCollection);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "Failed to create active endpoint collection object from devnode object with instance ID %s (hr = %s)", szInstanceID, GetHRString(hr));
        return FNS_FAIL;
    }

    UINT cDevices = 0;
    hr = pCollection->GetCount(&cDevices);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "Failed to get count of active endpoint collection object from devnode object with instance ID %s (hr = %s)", szInstanceID, GetHRString(hr));
        return FNS_FAIL;
    }

    if (0 == cDevices) 
    {
        XLOG(XFAIL, 1, "There are zero active endpoint devices on the system with instance ID %s, while there should be at least one", szInstanceID);
        return FNS_FAIL;
    }
    else
    {
        XLOG(XMSG, 1, "There %s %d active render device%s on the system for instance ID %s", 
                                ((cDevices > 1)? "are" : "is"), 
                                cDevices, 
                                ((cDevices > 1)? "s" : ""), 
                                szInstanceID);
    }
//#endif

    return FNS_PASS;
}   //  Test_EnumerateAudioEndpoint

HRESULT
CurrentPinShouldSupportWaveRT(bool &fShouldSupportWaveRT)
{
    HRESULT hr = S_OK;
    
    LPCSTR szInstanceID = g_pWaveTest->m_pHalf->m_pFilter->m_szInstanceID;
    CComPtr<IBasicLog> pLog;

    pLog = g_pBasicLog;

    //  First check if pin supports offload...
    {
        CPCMAudioPin* pPin = g_pWaveTest->m_pHalf->m_pPin;
        bool bOffloadSupported = false;
    
        hr = AudioOffload::DoesPinSupportOffload( pLog, pPin, AudioOffload::KS_API, bOffloadSupported);
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "AudioOffload::DoesPinSupportOffload returned error (hr = %s)", GetHRString(hr));
            return (hr);
        }
        
        if (bOffloadSupported)
        {
            fShouldSupportWaveRT = true;
            return S_OK;
        }
    }

    // ylsuen-2009/03/05: the original test implements the following:
    // - 'pass condition: if portcls then must be RT (portcls -> RT, or "not portcls OR RT")'
    // However, this is not exactly the requirement AUDIO-0001. The requirement specifies that
    // - Integrated or discrete audio device driver must be based on the Microsoft Windows WaveRT 
    //   miniport WDM driver model.Requirement details are defined in the white paper titled 
    //   "A Wave Port Driver for Real-Time Audio Streaming." 
    //   The legacy portsWaveCyclic or WavePCI are not used to support the audio device on Premium 
    //   Windows Vista systems.
    //   For device technologies such as USB Audio 1.0 based devices where the audio driver model 
    //   is not specifically called out, any WDM audio driver model is allowed.
    // Note that device such as USB and Bluetooth do not have to support WaveRT.
    // Hence the test is modified to only require WaveRT support on HD Audio with the following
    // assumptions:
    // - HD Audio devices are the only (primary) bus type that's internal. This is not much different from 
    //   what we were previously enforcing, e.g. internal USB devices that are installed with class 
    //   driver for example would not fullfill the WaveRT requirement.
    // - All HD Audio devices has PnP Id prefixed with HDAUDIO.

    // search for if the device is an HD Audio device. Note that some PCI devices are integrated
    // and considered to be HD Audio when installed with class driver. 

    CComPtr<ITestDeviceSet> pDevnodeSet;
    hr = CreateHDAudioDevNodeSet(pLog, &pDevnodeSet);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "Failed to create a list of integrated device devnodes (hr = %s)", GetHRString(hr));
        return hr;
    }

    bool fIsIntegrated = false; 
    bool isBth = false;
    hr = FindInstanceIDInDeviceSet(pDevnodeSet, szInstanceID, fIsIntegrated, isBth);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "Encountered failure when looking for hardware ID %s in the list of devnodes on the system (hr = %s)", szInstanceID, GetHRString(hr));
        return hr;
    }

    if (fIsIntegrated)
    {
        XLOG(XMSG, eInfo1, "The device %s is an integraded device", szInstanceID);
    }
    else
    {
        XLOG(XMSG, eError, "The device %s is NOT an integraded device", szInstanceID);
    }

    fShouldSupportWaveRT = fIsIntegrated;
    return S_OK;
}

// ------------------------------------------------------------------------------
// Test_VerifyPinIsWaveRT -  (self-explanatory)
// ------------------------------------------------------------------------------
DWORD 
Test_VerifyPinIsWaveRT()
{
    
    HRESULT hr = S_OK;
    
    bool fCurrPinShouldSupportWaveRT = true;
    hr = CurrentPinShouldSupportWaveRT(fCurrPinShouldSupportWaveRT);
    if (FAILED(hr))
    {
        return FNS_FAIL;
    }

    if (!fCurrPinShouldSupportWaveRT)
    {
        return FNS_SKIPPED;
    }

    DWORD dwRet = FNS_FAIL;
    BOOL fIsRTPin = g_pWaveTest->m_pHalf->IsWaveRTPin();
    if (fIsRTPin)
    {
        SLOG(eInfo1, "The device driver is WaveRT, as expected.");
        dwRet = FNS_PASS;
    }
    else if (!fCurrPinShouldSupportWaveRT)
    {
        SLOG(eInfo1, "The device driver does not need to support WaveRT. Test skipped.");
        dwRet = FNS_SKIPPED;
    }
    else
    {
        XLOG(XFAIL, eError, "The current device driver is not based on the WaveRT miniport model.");
    }
    
    return dwRet;
}   //  Test_VerifyPinIsWaveRT

// ------------------------------------------------------------------------------
// Test_VerifyPinSupportsPullMode -  verify if the pin supports pull mode
// ------------------------------------------------------------------------------
DWORD 
Test_VerifyPinSupportsPullMode(void)
{
    HRESULT hr = S_OK;
    BOOL fRes = FALSE;
    bool bPinIsLoopback = FALSE;
    CComPtr<IBasicLog> pLog;

    BOOL fIsRTPin = g_pWaveTest->m_pHalf->IsWaveRTPin();
    if (!fIsRTPin)
    {
        SLOG(eInfo1, "The current device driver is not based on the WaveRT miniport model.");
        return FNS_SKIPPED;
    }

    CPCMAudioPin *pPin = static_cast<CPCMAudioPin*>(g_pWaveTest->m_pHalf->m_pPin);
    if (pPin == NULL)
    {
        XLOG(XFAIL, eError, "Graph not complete");
        return FNS_FAIL;
    }

    pLog = g_pBasicLog;

    hr = AudioOffload::DoesPinSupportLoopback( pLog, pPin, bPinIsLoopback);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "ERROR in AudioOffload::DoesPinSupportLoopback (hr = %s)", GetHRString(hr));
        return FNS_FAIL;
    }

    if (bPinIsLoopback)
    {
        SLOG(eInfo1, "Current pin is loopback - skipping test.");
        return FNS_SKIPPED;
    }

    // verify that the endpoint is not a WaveCyclic PortCls driver
    // If bluetooth, the skip over this test...
    CComPtr<ITestDeviceSet> pAudioDevNodeSet;
    LPCSTR szInstanceID = g_pWaveTest->m_pHalf->m_pFilter->m_szInstanceID;

    // BUGBUG: SideBand BT devices are not discovered and skipped against.  
    // BUGBUG: For the time being if the filter's szFriendlyName contains "bthhfenum.sys" then we'll assume its sideband BT, and skip test.
    // BUGBUG: Revisit when WinBlue bug 89559 is fixed.
    LPCSTR sidebandBTStr = "bthhfenum.sys";
    LPCSTR szFriendlyName = CharLower(g_pWaveTest->m_pHalf->m_pFilter->m_szFriendlyName);
    if (strstr (szFriendlyName, sidebandBTStr))
    {
        SLOG(eInfo1, "Current endpoint is BlueTooth, skipping test.");
        return FNS_SKIPPED;
    }
    
    bool isBth = false;
    bool isIntegrated = false;
    hr = CreateAudioDevNodeSet(pLog, &pAudioDevNodeSet);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "Failed CreateHidDeviceSet (hr = %s)", GetHRString(hr));
        return FNS_FAIL;
    }

    hr = FindInstanceIDInDeviceSet(pAudioDevNodeSet, 
                                   szInstanceID, 
                                   isIntegrated, 
                                   isBth);
    if (FAILED(hr))
    {
       XLOG(XFAIL, 
            eError, 
            "Encountered failure when looking for hardware ID %s in the list of devnodes on the system (hr = %s)", 
            szInstanceID, 
            GetHRString(hr));
       return FNS_FAIL;
    }
    
    if (isBth)
    {
       SLOG(eInfo1, "Current Endpoint is Bluetooth, skipping test.");

       return FNS_SKIPPED;
    }
  
    // verify that the endpoints have PKEY_AudioEndpoint_Supports_EventDriven_Mode
    CComPtr<IMMDeviceCollection> pEndpointCollection;
    hr = GetCurrentEndpointsUnderTest(&pEndpointCollection);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "Failed to get current endpoints under test (hr = %s)", GetHRString(hr));
        return FNS_FAIL;
    }

    UINT cEndpoints = 0;
    hr = pEndpointCollection->GetCount(&cEndpoints);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "Failed to get number of endpoints in the collection (hr = %s)", GetHRString(hr));
        return FNS_FAIL;
    }

    XLOG(XMSG, eInfo1, "There is a total of %d endpoints for the current pin", cEndpoints);
    if (0 == cEndpoints)
    {
        XLOG(XFAIL, eError, "Didn't find any proper endpoints for the current pin. Current pin is neither host, offload, nor keyword detection pin");
        return FNS_FAIL;
    }

    // loop through all the endpoints and make sure that each of them have the endpoint association to the pull mode key
    for (UINT idxEP = 0; idxEP < cEndpoints; idxEP++)
    {
        CComPtr<IMMDevice> pEndpoint;

        hr = pEndpointCollection->Item(idxEP, &pEndpoint);
        if (FAILED(hr))
        {
            XLOG(XFAIL, eError, "Failed to get endpoint at index %d", idxEP);
            return FNS_FAIL;
        }

        CComHeapPtr<WCHAR> szId; // use the ID for logging purpose
        hr = pEndpoint->GetId(&szId);
        if (FAILED(hr))
        {
            XLOG(XFAIL, eError, "Failed to get endpoint id at index %d", idxEP);
            return FNS_FAIL;
        }

        XLOG(XMSG, eInfo2, "Audio endpoint device under test has ID: %ls", S_W(szId));
        
        CComPtr<IPropertyStore> pPropertyStore;
        hr = pEndpoint->OpenPropertyStore(STGM_READ, &pPropertyStore);
        if (FAILED(hr))
        {
            XLOG(XFAIL, eError, "Failed to open property store on endpoint (%ls) to read (IMMDevice::OpenPropertyStore) (hr = %s)", S_W(szId), GetHRString(hr));
            return FNS_FAIL;
        }

        CPropVariant varPullMode;
        hr = pPropertyStore->GetValue(PKEY_AudioEndpoint_Supports_EventDriven_Mode, &varPullMode);
        if (FAILED(hr))
        {
            XLOG(XFAIL, eError, "Failed to get PKEY_AudioEndpoint_Supports_EventDriven_Mode on endpoint (%ls) (IPropertyStore::GetValue) (hr = %s)", S_W(szId), GetHRString(hr));
            return FNS_FAIL;
        }
        
        if (VT_EMPTY == varPullMode.vt)
        {
            XLOG(XFAIL, eError, "PKEY_AudioEndpoint_Supports_EventDriven_Mode not defined on endpoint (%ls). Please update the INF AddReg section to include this key for this endpoint.", S_W(szId));
            continue; 
        }
        
        if (VT_UI4 != varPullMode.vt)
        {
            XLOG(XFAIL, eError, "PKEY_AudioEndpoint_Supports_EventDriven_Mode found but is not of type VT_UI4 on endpoint (%ls). Please update the INF AddReg section for this endpoint.", S_W(szId));
            continue;
        }

        bool bPullModeSet = (varPullMode.ulVal ? TRUE:FALSE);
        if (!bPullModeSet)
        {
            XLOG(XFAIL, eError, "PKEY_AudioEndpoint_Supports_EventDriven_Mode found but is not set to true on endpoint ret(%ls). Please update the INF AddReg section for this endpoint.", S_W(szId));
            continue;
        }

        SLOG(eInfo1, "Endpoint (%ls) is associated with PKEY_AudioEndpoint_Supports_EventDriven_Mode in INF.", S_W(szId));
    }

    // should we continue the rest?    
    bool fCurrPinShouldSupportWaveRT = true;
    hr = CurrentPinShouldSupportWaveRT(fCurrPinShouldSupportWaveRT);
    if (FAILED(hr))
    {
        return FNS_FAIL;
    }

    if (!fCurrPinShouldSupportWaveRT)
    {
        return FNS_PASS; // we skip the rest if the device does not need to support wave RT
    }

    // RAII for creating the render pin on the audio adapter filter and stop and close the pin at exit
    // This is necessary for querying the KS properties.
    fRes = FALSE;
    CPinInitialize pinInitialize(pPin, NULL, TRUE, fRes);
    if (!fRes)
    {
        XLOG(XFAIL, eError, "Unable to create looped streaming pin on a WaveRT device, this is unexpected.");
        return FNS_FAIL;
    }

    // check for RT Property support
    SLOG(eInfo2, "Checking for KSPROPERTY_RTAUDIO_BUFFER_WITH_NOTIFICATION property support on the pin");
    DWORD AccessFlag = 0;
    fRes = pPin->PropertyBasicSupport(KSPROPSETID_RtAudio, KSPROPERTY_RTAUDIO_BUFFER_WITH_NOTIFICATION, &AccessFlag);
    
    if (FALSE == fRes)
    {
        XLOG(XFAIL, eError, "Failed to query for basic support (KSPROPERTY_RTAUDIO_BUFFER_WITH_NOTIFICATION)");
    }
    else if (0 == (AccessFlag & KSPROPERTY_TYPE_GET))
    {
        XLOG(XFAIL, eError, "This pin does NOT support pull mode (KSPROPERTY_RTAUDIO_BUFFER_WITH_NOTIFICATION)");            
    } 
    else
    {
        SLOG(eInfo1, "This pin does support pull mode (KSPROPERTY_RTAUDIO_BUFFER_WITH_NOTIFICATION)");            
    }

    SLOG(eInfo2, "Checking for KSPROPERTY_RTAUDIO_QUERY_NOTIFICATION_SUPPORT property support on the pin");
    fRes = pPin->PropertyBasicSupport(KSPROPSETID_RtAudio, KSPROPERTY_RTAUDIO_QUERY_NOTIFICATION_SUPPORT, &AccessFlag);

    if (FALSE == fRes)
    {
        XLOG(XFAIL, eError, "Failed to query for basic support (KSPROPERTY_RTAUDIO_QUERY_NOTIFICATION_SUPPORT)");
    } 
    else if (0 == (AccessFlag & KSPROPERTY_TYPE_GET))
    {
        XLOG(XFAIL, eError, "This pin does NOT support pull mode (KSPROPERTY_RTAUDIO_QUERY_NOTIFICATION_SUPPORT)");            
    }
    else
    {
        SLOG(eInfo1, "This pin does support pull mode (KSPROPERTY_RTAUDIO_QUERY_NOTIFICATION_SUPPORT)");            
    }
    
    return FNS_PASS;
}

DWORD Test_VerifyRawOrDefaultProcessingSupported(_In_ EDataFlow dataFlow)
{
    HRESULT         hr              = S_OK;
    BOOL            fRes            = FALSE;
    BOOL            bRawSupport     = FALSE;
    BOOL            bDefaultSupport = FALSE;
    bool            bPinIsLoopback  = false;
    CPCMAudioPin*   pPin            = g_pWaveTest->m_pHalf->m_pPin;
    CComPtr<IBasicLog> pLog;
    String          OldRequirement="";

    pLog = g_pBasicLog;

    hr = AudioOffload::DoesPinSupportLoopback(pLog, pPin, bPinIsLoopback);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: in AudioOffload::DoesPinSupportLoopback (hr = %s)", GetHRString(hr));
        return FNS_FAIL;
    }

    if (bPinIsLoopback)
    {
        SLOG(eInfo1, "Current pin is loopback - skipping test.");
        return FNS_SKIPPED;
    }

    fRes = pPin->CheckProcessingModes(&bRawSupport, &bDefaultSupport);
    if (false == fRes)
    {
        XLOG(XFAIL, eError, "FAIL: The pin %d doesn't support KSPROPERTY_AUDIOSIGNALPROCESSING_MODES", pPin->m_nId);
        return FNS_FAIL;
    }

    // For threshold, all the pins should support "either" default or raw mode
    if (SUCCEEDED(RuntimeParameters::TryGetValue(L"OldRequirement", OldRequirement)) && 
        !OldRequirement.CompareNoCase(L"Threshold"))
    {
        SLOG(eInfo1, "Applying Threshold requirement");

        if (false == bRawSupport && false == bDefaultSupport)
        {
            XLOG(XFAIL, eError, "FAIL: Pin %d does not support Raw or Default mode; Support for Raw or Default Mode is required.", pPin->m_nId);
            return FNS_FAIL;
        }

        SLOG(eInfo1, "Pin %d supports Raw or Default mode as required (Raw %d; Default %d)", pPin->m_nId, bRawSupport, bDefaultSupport);
    }
    else
    {
        // For Redstone, "capture" pin should at least support raw
        if (eCapture == dataFlow)
        {
            if (false == bRawSupport)
            {
                XLOG(XFAIL, eError, "FAIL: Capture Pin %d does not support Raw mode; Capture supports for Raw Mode is required.", pPin->m_nId);
                return FNS_FAIL;
            }
            SLOG(eInfo1, "Capture pin %d supports at least Raw mode as required (Raw %d; Default %d)", pPin->m_nId, bRawSupport, bDefaultSupport);
        }
        else
        {
            if (false == bRawSupport && false == bDefaultSupport)
            {
                XLOG(XFAIL, eError, "FAIL: Pin %d does not support Raw or Default mode; Support for Raw or Default Mode is required.", pPin->m_nId);
                return FNS_FAIL;
            }
            SLOG(eInfo1, "Render pin %d supports Raw or Default mode as required (Raw %d; Default %d)", pPin->m_nId, bRawSupport, bDefaultSupport);
        }
    }

    return FNS_PASS;
}

DWORD Test_VerifyRenderRawOrDefaultProcessingSupported(void)
{
    return Test_VerifyRawOrDefaultProcessingSupported(eRender);
}

DWORD Test_VerifyCaptureRawOrDefaultProcessingSupported(void)
{
    return Test_VerifyRawOrDefaultProcessingSupported(eCapture);
}

