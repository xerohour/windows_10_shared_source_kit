// Copyright(C) Microsoft. All rights reserved.

//
// TestUtilities.cpp
//
// Description:
//
//  Misc Utilities to support UMATest
//

#include <stdafx.h>
#include <ATLComTime.h>

#include <propkey.h>
#include <avendpointkeys.h>
#include <UMATestStreaming.h>
#include <AudioOffload.h>
#include <deviceaccesshelper.h>

#include "TestUtilities.h"
#include "AudioSessionEventHandler.h"
#include "EndpointVolumeEventHandler.h"
#include <CPropVariant.h>

#define REGKEY_AUDIOSERVICE "Software\\Microsoft\\Windows\\CurrentVersion\\Audio"
#define REGVALUE_ALLOW_CLASSIC_OFFLOAD "AllowClassicOffload"

const static UINT MAX_ARRAY_LENGTH = 256;
const static FLOAT DB_TOLER = .0001f;

//-----------------------------------------------------------------------------
// Method: LogDate
//
// Description:
//     Convert a DATE structure to string format, and log it to the console
//
//    
// Parameters:
//     [in] The DATE structure to be logged
//
// Return values (HRESULT):
//     S_OK on success, the appropriate HRESULT on failure
//
// ----------------------------------------------------------------------------
void LogDate(DATE date)
{
    HRESULT     hr = S_OK;
    SYSTEMTIME  stUTC;
    int         cwcBuffer;
    TCHAR       text[MAX_ARRAY_LENGTH];

    cwcBuffer = MAX_ARRAY_LENGTH - 1; ;

    if(!VariantTimeToSystemTime(date,&stUTC))
    {
        hr = OLE_E_CANTCONVERT;
        XLOG(XFAIL, 
             eError, 
             TEXT("FAIL: VariantTimeToSystemTime returned 0x%08x (%s)"),
             hr,
             GetHRString(hr));
    }

    if(SUCCEEDED(hr))
    {
        if(!GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&stUTC,NULL,text,cwcBuffer)) 
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
            XLOG(XFAIL, eError, TEXT("FAIL: GetDateFormat 0x%08x (%s)"), hr, GetHRString(hr));
        }
        else
        {
            SLOG(eInfo1, TEXT("  DATE: %s"),text);         
        }
    }

    return;
}


// ----------------------------------------------------------------------------
// Method: VerifyDate
//
// Description:
//  Verify a DATE parameter as valid
//    
// Parameters:
//     [in] date -- a DATE structure
//
// Return values (HRESULT):
//     S_OK if DATE represents a valid DATE structure, the appropriate
//      HRESULT on failure
//
// ----------------------------------------------------------------------------
HRESULT VerifyDate(DATE date)
{
    HRESULT     hr = S_OK;
    SYSTEMTIME  stUTC;
    int         cwcBuffer;
    TCHAR       text[MAX_ARRAY_LENGTH];
    TCHAR       now[MAX_ARRAY_LENGTH];

    cwcBuffer = MAX_ARRAY_LENGTH - 1;

    if(!VariantTimeToSystemTime(date,&stUTC))
    {
        hr = OLE_E_CANTCONVERT;
        XLOG(XFAIL, eError, TEXT("FAIL: VariantTimeToSystemTime failed: 0x%08x (%s)"), hr, GetHRString(hr));
    }

    if(SUCCEEDED(hr))
    {
        if(!GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&stUTC,NULL,text,cwcBuffer)) 
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
            XLOG(XFAIL, eError, TEXT("FAIL: GetDateFormat failed: 0x%08x (%s)"), hr, GetHRString(hr));
        }
        else
        {
            SLOG(eInfo1, TEXT("  DATE: %s"),text);
         
            FILETIME filetime;
            ZeroMemory(&filetime,sizeof(FILETIME));
            GetSystemTimeAsFileTime(&filetime);
            if(!FileTimeToLocalFileTime(&filetime, &filetime))
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                XLOG(XFAIL, eError, TEXT("FAIL: FileTimeToLocalFileTime failed: 0x%08x (%s)"), hr, GetHRString(hr));
            }
            else
            {

                COleDateTime lastTime(filetime);
                if(!VariantTimeToSystemTime(date,&stUTC))
                {
                    hr = OLE_E_CANTCONVERT;
                    XLOG(XFAIL, eError, TEXT("FAIL: VariantTimeToSystemTime failed: 0x%08x (%s)"), hr, GetHRString(hr));
                }

                if(SUCCEEDED(hr))
                {
                    if(!GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&stUTC,NULL,now,cwcBuffer)) 
                    {
                        hr = HRESULT_FROM_WIN32(GetLastError());
                        XLOG(XFAIL, eError, TEXT("FAIL: GetDateFormat failed: 0x%08x (%s)"),hr,GetHRString(hr));
                    }
                    else
                    {
                        if(strcmp(text,now))
                        {
                            XLOG(XFAIL, eError,"Dates do not match. This is NOT as expected.");
                            XLOG(XFAIL, eError, TEXT("  Returned DATE: -%s-"),text);
                            XLOG(XFAIL, eError, TEXT("  System   DATE: -%s-"),now);
                            hr = E_FAIL;
                        }
                    }
                }
            }
        }
    }
    return hr;
}

//-----------------------------------------------------------------------------
// Method: LogGUID
//
// Description:
//     Convert a GUID structure to string format, and log it to the console
//
//    
// Parameters:
//     [in] The GUID structure to be logged
//
// Return values (HRESULT):
//     S_OK on success, the appropriate HRESULT on failure
//
// ----------------------------------------------------------------------------
void LogGUID(REFGUID Guid)
{
    OLECHAR        wszGuid[MAX_ARRAY_LENGTH];
    StringFromGUID2(Guid, wszGuid, MAX_ARRAY_LENGTH);
    SLOG(eInfo1, TEXT("  GUID: %ls"), wszGuid);
}

// Wait for an endpoint volume event
HRESULT WaitForEndpointEvent(CEndpointVolumeEventHandler* pEventHandler, ULONG nEventTimeoutMillis)
{
    ULONG nElapsedTime = 0;

    // validate params
    if(NULL == pEventHandler)
    {
        SLOG(eFatalError, "ERROR: NULL handler passed to WaitForEndpointEvent");
        return E_FAIL;
    }

    while(nElapsedTime < nEventTimeoutMillis)
    {
        if(pEventHandler->CheckEventReceived())
        {
            return S_OK;
        }

        // wait another 50 ms
        nElapsedTime += 50;
        Sleep(50);        
    }

    // no event
    SLOG(eFatalError, "<timeout waiting for endpoint event>");
    
    return E_FAIL;
}

// Wait for an audiosession volume/control event
HRESULT WaitForSessionEvent(CAudioSessionEventHandler* pEventHandler, ULONG nEventType, ULONG nEventTimeoutMillis)
{
    ULONG nElapsedTime = 0;

    // validate prams
    if(NULL == pEventHandler)
    {
        SLOG(eFatalError, "ERROR: NULL handler passed to WaitForAudioEvent");
        return E_FAIL;
    }

    while(nElapsedTime < nEventTimeoutMillis)
    {
        switch(nEventType)
        {
            case AUDIO_EVENT_SIMPLEVOLUME_CHANGED:
                if(pEventHandler->GetVolumeChangedEventCount() > 0)
                {
                    return S_OK;
                }
                break;                
            case AUDIO_EVENT_CHANNELVOLUME_CHANGED:
                if(pEventHandler->GetChannelVolumeChangedEventCount() > 0)
                {
                    return S_OK;
                }
                break;
            case AUDIO_EVENT_STATE_CHANGED:
                if(pEventHandler->GetStateChangedEventCount() > 0)
                {
                    return S_OK;
                }
                break;
            case AUDIO_EVENT_GROUPINGPARAM_CHANGED:
                if(pEventHandler->GetGroupingParamChangedEventCount() > 0)
                {
                    return S_OK;
                }
                break;
            case AUDIO_EVENT_ICONPATH_CHANGED:
                if(pEventHandler->GetIconPathChangedEventCount() > 0)
                {
                    return S_OK;
                }
                break;
            case AUDIO_EVENT_DISPLAYNAME_CHANGED:
                if(pEventHandler->GetDisplayNameChangedEventCount() > 0)
                {
                    return S_OK;
                }
                break;
            case AUDIO_EVENT_SESSION_DISCONNECTED:
                if(pEventHandler->GetSessionDisconnectedEventCount() > 0)
                {
                    return S_OK;
                }
                break;
            default:
                break;
        }        

        // wait another 50 ms
        nElapsedTime += 50;
        Sleep(50);
    }

    // no event
    SLOG(eFatalError, "<timeout waiting for session event>");
    
    return E_FAIL;
}

/*

A volume setting of 0 dB corresponds to a scalar value of 1 and a tapered value of 1.

A volume setting of (-infinity dB to 0 dB) is "attenuation."  This corresponds to scalar values of
(0 to 1) and tapered values of (0 to 1).

In this range, the scalar-to-taper curve is t = pow(s, 4/7).  This allows greater control over
settings close to 0 dB = (scalar)1 = (taper)1.

For capture, a volume setting of greater than 0 dB is allowed: (0 dB to +infinity dB) is
"amplification" or "positive gain."  This corresponds to scalar values of (1 to infinity and
tapered values of (1 to 2). In this range, the scalar-to-taper curve is t = 1 + (1 - pow(s, -4/7)).
This likewise allows greater control over settings close to 0 dB = (scalar)1 = (taper)1.

taper
        ^
        |
        |
        |
      2 + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - >
        |
        |
        |
        |
        |
        |                                                      ***************>
        |                                          ************
        |                                  ******** t = 1 + (1 - pow(s, -4/7))
      1 +                            **x***
        |                      ******
        |                 *****
        |             ****
        |         **** t = pow(s, 4/7)
        |     ****
        |   **
        | **
        |*
      0 *------------------------------+-------------------------------------->
        0        (attenuation)         1               (gain)            scalar

*/
FLOAT ConvertDbToWiper(FLOAT fDbVol, FLOAT fMinDbVolume, FLOAT fMaxDbVolume, DWORD dwDeviceType)
{
    FLOAT fRetVal = 0.0f;

    FLOAT fMinScalar = 0.0f;
    FLOAT fMaxScalar = 0.0f;    

    if(DEVICE_TYPE_CAPTURE == dwDeviceType || DEVICE_TYPE_RENDER == dwDeviceType)
    {
        FLOAT fMinTaper = 0.0f;
        FLOAT fMaxTaper = 0.0f;

        FLOAT fCurrScalar = 0.0f;
        FLOAT fCurrTaper = 0.0f;

        if(fMinDbVolume == fMaxDbVolume)
        {
            fRetVal = -1.0f;
            goto END;
        }

        fMinScalar = pow(10.0f, fMinDbVolume/20.0f);  
        fMaxScalar = pow(10.0f, fMaxDbVolume/20.0f);
        fCurrScalar = pow(10.0f, fDbVol/20.0f);

        fMinTaper = fMinScalar <= 1.0 ? (pow(fMinScalar, 4.0f/7.0f)) : (2 - pow(fMinScalar, -4.0f/7.0f));
        fMaxTaper = fMaxScalar <= 1.0 ? (pow(fMaxScalar, 4.0f/7.0f)) : (2 - pow(fMaxScalar, -4.0f/7.0f));

        fCurrTaper = fCurrScalar <= 1.0 ? (pow(fCurrScalar, 4.0f/7.0f)) : (2 - pow(fCurrScalar, -4.0f/7.0f));
                        
        fRetVal = (fCurrTaper-fMinTaper)/(fMaxTaper-fMinTaper);
        goto END;
    }
    else
    {
        // invalid dataflow direction
        // ensure failure
        fRetVal = -1.0f; 
        goto END;
    }

END:
    return fRetVal;
}

//-------------------------------------------------------------------------
// Method: CreateEmptySessionInProcess
//
// Description:
//  Create an audiosession with no associated stream in the current process
//    
// Parameters:
//      pAudSesMgr         - reference to the session manager to use 
//                           for creating the new audiosession
//      wszNewSessionID - id of the newly created audiosession
//
// Return values (HRESULT):
//     S_OK on success, the appropriate HRESULT on failure
HRESULT CreateEmptySessionInProcess(IAudioSessionManager* pAudSesMgr, __out LPWSTR& wszNewSessionID)
{
    HRESULT hr = S_OK;
    GUID newGuid = GUID_NULL;
    CComPtr<IAudioSessionControl> spAudSessCtrl = NULL;
    CComPtr<IAudioSessionControl2> spAudSessCtrlEx = NULL;

    wszNewSessionID = NULL;

    // we need a GUID
    METHODCHECKRESULT("::CoCreateGuid",
                      hr,
                      ::CoCreateGuid(&newGuid));
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Error creating new guid");
        goto END;
    }
    
    METHODCHECKRESULT("IAudioSessionManager::GetAudioSession",
                      hr,
                      pAudSesMgr->GetAudioSessionControl(&newGuid, FALSE, &spAudSessCtrl));
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Unable to create new audiosession");       
        goto END;
    }

    METHODCHECKRESULT("IAudioSessionControl::QueryInterface",
                        hr,
                        spAudSessCtrl->QueryInterface(__uuidof(IAudioSessionControl2),
                                                        (void**)&spAudSessCtrlEx));

    if(FAILED(hr)) { 
        SLOG(eError, "ERROR: Unable to obtain IAudioSessionControl2 interface");
        goto END; 
    }

    // get the ID of the audiosession
    METHODCHECKRESULT("IAudioSessionControl2::GetSessionInstanceIdentifier",
                      hr,
                      spAudSessCtrlEx->GetSessionInstanceIdentifier(&wszNewSessionID));
    if(FAILED(hr))
    {
        SLOG(eInfo1, "ERROR: Unable to obtain audiosession ID");
        goto END;
    }
    else
    {
        SLOG(eInfo1, "New AudioSession ID: %ws", wszNewSessionID);
    }

END:
       return hr;
}

//-------------------------------------------------------------------------
// Method: CreateStreamSessionInProcess
//
// Description:
//  Create an audiosession with a single associated stream in the current process
//    
// Parameters:
//      pStreamingEngine        - Pointer to the streaming engine associated with the
//                                new session.  
//
//                                Must be released by the client.
//
//      pRenderStreamingEngine  - Pointer to the render streaming engine associated
//                                with the new session.  
//                                
//                                NULL if the flow direction of the current endpoint 
//                                under test is CAPTURE.  
//
//                                Must be released by the client.
//
//      pCaptureStreamingEngine - Pointer to the capture streaming engine associated
//                                with the new session.  
//                                
//                                NULL if the flow direction of the current endpoint 
//                                under test is RENDER.  
//
//                                Must be released by the client.
//
//      wszNewSessionID         - session instance id of the newly created audiosession 
//                                (Client must call CoTaskMemFree on this string)
//
// Return values (HRESULT):
//     S_OK on success, the appropriate HRESULT on failure
HRESULT CreateStreamSessionInProcess(ITestStreamingEngine** ppStreamingEngine, 
                                     IRenderTestStreamingEngine** ppRenderStreamingEngine,
                                     ICaptureTestStreamingEngine** ppCaptureStreamingEngine,
                                     __out LPWSTR& wszNewSessionID)
{
    HRESULT hr = S_OK;
    AudioClientProperties clientProperties = {0};

    LPCWSTR pwszEndpointUnderTest = GetEndpointIDUnderTest();
    GUID guidNewSessionGuid = GUID_NULL;

    CComPtr<ITestStreamingEngine> spStreamingEngine = NULL;
    CComPtr<IRenderTestStreamingEngine> spRenderStreamingEngine = NULL;
    CComPtr<ICaptureTestStreamingEngine> spCaptureStreamingEngine = NULL;
    CComPtr<IAudioSessionControl> spSessionControl = NULL;
    CComPtr<IAudioSessionControl2> spSessionControlEx = NULL;

    clientProperties.bIsOffload = FALSE;
    clientProperties.eCategory = AudioCategory_Other;
    clientProperties.cbSize = sizeof(clientProperties);

    // Create a guid for the new session
    hr = CoCreateGuid(&guidNewSessionGuid);
    if(FAILED(hr))
    {
        SLOG(eError, "FAIL: Unable to create new GUID");
        goto END;
    }

    if((DEVICE_TYPE_RENDER == GetDeviceTypeUnderTest())||(DEVICE_TYPE_RENDEROFFLOAD == GetDeviceTypeUnderTest()))
    {
        // Create the streaming engine
        METHODCHECKRESULT("CreateTestStreamingEngineOnEndpointId",
                          hr,
                          CreateTestStreamingEngineOnEndpointId(IID_IRenderTestStreamingEngine,
                                                    pwszEndpointUnderTest,
                                                    (DEVICE_TYPE_RENDEROFFLOAD == GetDeviceTypeUnderTest() ? EVENT_PULL_RENDER : TIMER_PUSH_RENDER),
                                                    NULL,
                                                    (void**)(&spRenderStreamingEngine)));
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Unable to create test streaming engine");
            goto END;
        }

        // Get the ITestStreamingEngine interface
        METHODCHECKRESULT("IRenderTestStreamingEngine::QueryInterface",
                          hr,
                          spRenderStreamingEngine->QueryInterface(IID_ITestStreamingEngine,
                                                                  (void**)&spStreamingEngine));
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Unable to QI for ITestStreamingEngine");   
            goto END;
        }

        if (DEVICE_TYPE_RENDEROFFLOAD == GetDeviceTypeUnderTest())
        {
            clientProperties.bIsOffload = TRUE;
            clientProperties.eCategory = AudioCategory_Media;
        }
    }
    else if(DEVICE_TYPE_CAPTURE == GetDeviceTypeUnderTest())
    {
        // Create the streaming engine
        METHODCHECKRESULT("CreateTestStreamingEngineOnEndpointId",
                          hr,
                          CreateTestStreamingEngineOnEndpointId(IID_ICaptureTestStreamingEngine,
                                                    pwszEndpointUnderTest,
                                                    TIMER_PUSH_CAPTURE,
                                                    NULL,
                                                    (void**)(&spCaptureStreamingEngine)));
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Unable to create test streaming engine");
            goto END;
        }

        // Get the ITestStreamingEngine interface
        METHODCHECKRESULT("ICaptureTestStreamingEngine::QueryInterface",
                          hr,
                          spCaptureStreamingEngine->QueryInterface(IID_ITestStreamingEngine,
                                                                   (void**)&spStreamingEngine));
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Unable to QI for ITestStreamingEngine");   
            goto END;
        }
    }
    else
    {
        SLOG(eError, "ERROR: Endpoint under test does not appear to support streaming!");
        hr = E_FAIL;
        goto END;
    }

    // Use a unique session ID so that a new session will appear
    METHODCHECKRESULT("ITestStreamingEngine::SetRequestedSessionGUID",
                      hr,
                      spStreamingEngine->SetRequestedSessionGUID(guidNewSessionGuid));
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: unable to set session GUID for streaming engine");
        goto END;
    }

    // Initialize the render engine, creating a stream
    METHODCHECKRESULT("ITestStreamingEngine::Initialize()",
                      hr,
                      spStreamingEngine->Initialize(&clientProperties));

    if(hr == UTS_E_OFFLOAD_NOT_SUPPORTED)
    {
        XLOG(XSKIP, eError, "SKIP: Render Endpoint does not support offload");
        goto SKIPPED;
    }
    
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Unable to initialize streaming engine");
        goto END;
    }

    // Get the SessionID of the new stream
    //
    METHODCHECKRESULT("ITestStreamingEngine::GetAudioSessionControl",
                      hr,
                      spStreamingEngine->GetSessionControl(&spSessionControl));
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Unable to obtain IAudioSessionControl");
        goto END;
    }

    METHODCHECKRESULT("IAudioSession::QueryInterface",
                      hr,
                      spSessionControl->QueryInterface(__uuidof(IAudioSessionControl2),
                                                       (void**)&spSessionControlEx));
    if(FAILED(hr))
    {

        SLOG(eError, "ERROR: Unable to obtain IAudioSessionControl2 interface");
    }

    METHODCHECKRESULT("IAudioSessionControl2::GetSessionInstanceIdentifier",
                      hr,
                      spSessionControlEx->GetSessionInstanceIdentifier(&wszNewSessionID));
    if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Unable to obtain Session Instance ID for newly created audiosession");
    }
    else
    {
        SLOG(eInfo1, "INFO: Created audiosession %ws", wszNewSessionID);        
    }
            
    *ppStreamingEngine = spStreamingEngine;
    if((*ppStreamingEngine) != NULL)
    {
        (*ppStreamingEngine)->AddRef();
    }

    *ppRenderStreamingEngine = spRenderStreamingEngine;
    if((*ppRenderStreamingEngine) != NULL)
    {
        (*ppRenderStreamingEngine)->AddRef();
    }

    *ppCaptureStreamingEngine = spCaptureStreamingEngine;
    if((*ppCaptureStreamingEngine) != NULL)
    {
        (*ppCaptureStreamingEngine)->AddRef();
    }

END:    
    // release these interfaces so they no longer use the logger object
    spCaptureStreamingEngine = NULL;
    spRenderStreamingEngine = NULL;
    spStreamingEngine = NULL;
    
    return hr;

SKIPPED:
    return FNS_SKIPPED;
}


HRESULT CheckDeviceSupportsOffload(IMMDevice *pMMDevice, AudioOffload::EApiLevel api, bool& fSupported)
{
    HRESULT hr = S_OK;
    if (NULL == pMMDevice)
    {
        XLOG( XFAIL, eFatalError, "Input IMMDevie pointer is NULL");
        return E_POINTER;
    }

    if (GetModule()->IsAppContainer())
    {
        CComPtr<IAudioDeviceAccessHelper> pHelper = GetModule()->GetDeviceAccessHelper();

        CComHeapPtr<WCHAR> pszId;
        hr = pMMDevice->GetId(&pszId);
        if (FAILED(hr))
        {
            XLOG( XFAIL, eFatalError, "Error calling IMMDevice::GetId (0x%8x) [%s]", hr, GetHRString( hr ) );
            return hr;
        }

        BOOL bSupported = FALSE;
        // See if there is an offload endpoint on this device (device topology)
        switch (api)
        {
        case AudioOffload::MMDEV_API:
            hr = pHelper->IsAudioOffloadSupported(pszId, &bSupported);
            if( FAILED(hr) )
            {
                XLOG( XFAIL, eFatalError, "Error calling IAudioDeviceAccessHelper::IsAudioOffloadSupported (0x%8x) [%s]", hr, GetHRString( hr ) );
                return hr;
            }
            break;
        case AudioOffload::KS_API:
            hr = pHelper->IsAudioOffloadSupportedFromKernelStreamingAPI(pszId, &bSupported);
            if( FAILED(hr) )
            {
                XLOG( XFAIL, eFatalError, "Error calling IAudioDeviceAccessHelper::IsAudioOffloadSupportedFromKernelStreamingAPI (0x%8x) [%s]", hr, GetHRString( hr ) );
                return hr;
            }
            break;
        default:
            XLOG( XFAIL, eFatalError, "Unknown api requested %d", api);
            return E_INVALIDARG;
        }
        fSupported = (TRUE == bSupported);
    }
    else
    {
        // See if there is an offload endpoint on this device (device topology)
        hr = AudioOffload::DoesDeviceSupportOffload( GetModule()->GetBasicLogger(), pMMDevice, api, fSupported );
        if( FAILED(hr) )
        {
            XLOG( XFAIL, eFatalError, "Error calling AudioOffload::DoesDeviceSupportOffload (0x%8x) [%s]", hr, GetHRString( hr ) );
            return hr;
        }
    }

    return S_OK;
}


HRESULT GetMMDeviceKSInstanceInformation(IMMDevice *pMMDevice, ULONG &cPossibleInstance, ULONG &cCurrentInstances)
{
    HRESULT hr = S_OK;
    if (NULL == pMMDevice)
    {
        XLOG( XFAIL, eFatalError, "Input IMMDevie pointer is NULL");
        return E_POINTER;
    }

    if (GetModule()->IsAppContainer())
    {
        CComPtr<IAudioDeviceAccessHelper> pHelper = GetModule()->GetDeviceAccessHelper();

        CComHeapPtr<WCHAR> pszId;
        hr = pMMDevice->GetId(&pszId);
        if (FAILED(hr))
        {
            XLOG( XFAIL, eFatalError, "Error calling IMMDevice::GetId (0x%8x) [%s]", hr, GetHRString( hr ) );
            return hr;
        }

        // Get KS instance information
        hr = pHelper->GetEndpointKSInstanceInformation(pszId, (UINT*)&cPossibleInstance, (UINT*)&cCurrentInstances);
        if( FAILED(hr) )
        {
            XLOG( XFAIL, eFatalError, "Error calling IAudioDeviceAccessHelper::GetMMDeviceKSInstanceInformation (0x%8x) [%s]", hr, GetHRString( hr ) );
            return hr;
        }
    }
    else
    {
        // Get KS instance information
        hr = AudioOffload::GetInstanceInformation( GetModule()->GetBasicLogger(), pMMDevice, cPossibleInstance, cCurrentInstances);
        if( FAILED(hr) )
        {
            XLOG( XFAIL, eFatalError, "Error calling AudioOffload::GetInstanceInformation (0x%8x) [%s]", hr, GetHRString( hr ) );
            return hr;
        }
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Method: CheckDeviceSupportsORawMode 
//
// Description:
//       Get the value from the property key for the device 
//    
// Parameters:
//     [in] IMMDevice: Device for which the property key should be set
//     [out] BOOL: TRUE for raw mode enabled and FALSE for raw mode disabled 
//
// Return values (HRESULT):
//     S_OK on success, the appropriate HRESULT on failure
//
// ----------------------------------------------------------------------------
HRESULT CheckDeviceSupportsRawMode(IMMDevice* pMMDevice, BOOL * pbRawModeSupported)
{
    HRESULT hr = S_OK;
    PROPVARIANT var;
    CComPtr<IPropertyStore> pPropertyStore;

    if( pMMDevice == NULL )
    {
        XLOG( XFAIL, eFatalError, "pMMDevice is NULL");
        return S_FALSE;
    }
    *pbRawModeSupported = FALSE;

    PropVariantInit(&var);

    hr = pMMDevice->OpenPropertyStore( STGM_READ, &pPropertyStore );
    if( S_OK != hr )
    {
        XLOG( XFAIL, eFatalError, "FAIL: Expected OpenPropertyStore to return 0x%08x [S_OK].  Actual 0x%08x [%s]",
            S_OK, hr, GetHRString( hr ) );
    }
    else
    {
        VERIFY_SUCCEEDED(hr = pPropertyStore->GetValue(PKEY_Devices_AudioDevice_RawProcessingSupported, &var));
        if ( FAILED(hr) )
        {    
            XLOG( XFAIL, eFatalError, "Error calling PropertyStore::GetValue (0x%8x) [%s]", hr, GetHRString( hr ) );
        }
        else
        {
            *pbRawModeSupported = var.boolVal ? TRUE : FALSE;
        }
    }

    PropVariantClear(&var);

    return hr;
}

FLOAT rand_float()
{
    static_assert(RAND_MAX == 0x7fff, "Unexpected RAND_MAX value");

    // rand() returns a number from 0 to RAND_MAX
    // this has 15 random bits
    // an IEEE single-precision float has 24 bits of mantissa (and 8 bits of exponent)
    // so calling rand() twice suffices; more is overkill
    //
    // to adjust this method for a different RAND_MAX
    // or to return a double instead
    // adjust the "2" below so that count * (bits in RAND_MAX) >= bits needed
    const int count = 2;
    
    float x = 0.0f;
    float max = 0.0f;

    float m = RAND_MAX;
    
    for (int i = 0; i < count; i++) {
        // invariant
        // x is a random integer between 0 and max inclusive
        // max = pow(m + 1, i) - 1
        x = (x * (m + 1)) + rand();
        
        // now x is a random integer between 0 and max * (m + 1) + m inclusive
        // note max * (m + 1) = (pow(m + 1, i) - 1) * (m + 1) + m = pow(m + 1, i + 1) - (m + 1) + m
        // so the new max is pow(m + 1, i + 1) - 1
        // thus preserving the invariant
        max = (max * (m + 1)) + m;
    }
    
    return x / max;
}

