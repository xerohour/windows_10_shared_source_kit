// Copyright (C) Microsoft. All rights reserved.
#include "common.h"

BOOL CheckFaultAPOInitialize
(
    IAudioProcessingObject     *pIAPO,
    UINT                        cbBuffer,
    BYTE                       *pBuffer
)
{
    BOOL        fFault = FALSE;
    
    __try
    {
        pIAPO->Initialize(cbBuffer, pBuffer);
    }
    __except ( (EXCEPTION_ACCESS_VIOLATION == GetExceptionCode())?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH )
    {
        fFault = TRUE;
    }
    
    return (fFault);
}

DWORD WINAPI TC_External_NULL_DeviceCollection()
{                                                                                           
    CAPODevice                         *pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    CComPtr<IUnknown>                   pIUnknown = NULL;
    CComPtr<IAudioProcessingObject>     pIAPO = NULL;
    GUID                                gClsId;
    APOInitSystemEffects                sysfxInitParams = {0};
    HRESULT                             hr;

    pCurrentDevice->GetClsID(&gClsId);
    
    //  Get the General Interface...
    SLOG(eInfo1, "CoCreating the APO object.");
    if (S_OK != (hr = pIUnknown.CoCreateInstance(gClsId)))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: CoCreateInstance for IUnknown returned error (0x%08lx).",
            hr);
            
        return (FNS_PASS);
    }
    
    //  Get the APO Interface...
    SLOG(eInfo1, "Querying for the APO interface.");
    if (S_OK != (hr = pIUnknown->QueryInterface(__uuidof(IAudioProcessingObject), (void**)&pIAPO)))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: CoCreateInstance for IAudioProcessingObject returned error "
                "(0x%08lx).",
            hr);
            
        return (FNS_PASS);
    }

    //  Setup the Initialization parameters...    
    sysfxInitParams.APOInit.cbSize              = sizeof(sysfxInitParams);
    sysfxInitParams.APOInit.clsid               = gClsId;
    sysfxInitParams.pAPOEndpointProperties      = pCurrentDevice->GetEndpointStore();
    sysfxInitParams.pAPOSystemEffectsProperties = pCurrentDevice->GetFxStore();
    sysfxInitParams.pDeviceCollection           = NULL;     //  Note:  This is the offending param.
    sysfxInitParams.pReserved                   = NULL;
    
    if (CheckFaultAPOInitialize(pIAPO, sizeof(sysfxInitParams), (BYTE*)&sysfxInitParams))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: APO::Initialize() with (NULL == APOInitSystemEffects."
                "pDeviceCollection) causes an exception "
                "(EXCEPTION_ACCESS_VIOLATION).");
    
        return (FNS_PASS);
    }
    else
    {
        SLOG(eInfo1, "APO::Initialize() with (NULL == "
                     "APOInitSystemEffects.pDeviceCollection) does not cause "
                     "an exception (EXCEPTION_ACCESS_VIOLATION).");
    }
    
    return (FNS_PASS);
}

DWORD WINAPI TC_External_NULL_EndpointProperties()
{                                                                                           
    CAPODevice                         *pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    CComPtr<IUnknown>                   pIUnknown = NULL;
    CComPtr<IAudioProcessingObject>     pIAPO = NULL;
    GUID                                gClsId;
    APOInitSystemEffects                sysfxInitParams = {0};
    HRESULT                             hr;

    pCurrentDevice->GetClsID(&gClsId);

    //  Get the General Interface...
    SLOG(eInfo1, "CoCreating the APO object.");
    if (S_OK != (hr = pIUnknown.CoCreateInstance(gClsId)))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: CoCreateInstance for IUnknown returned error (0x%08lx).",
            hr);

        return (FNS_PASS);
    }

    //  Get the APO Interface...
    SLOG(eInfo1, "Querying for the APO interface.");
    if (S_OK != (hr = pIUnknown->QueryInterface(__uuidof(IAudioProcessingObject), (void**)&pIAPO)))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: CoCreateInstance for IAudioProcessingObject returned error "
            "(0x%08lx).",
            hr);

        return (FNS_PASS);
    }

    //  Setup the Initialization parameters...    
    sysfxInitParams.APOInit.cbSize              = sizeof(sysfxInitParams);
    sysfxInitParams.APOInit.clsid               = gClsId;
    sysfxInitParams.pAPOEndpointProperties      = NULL;     //  Note:  This is the offending param.
    sysfxInitParams.pAPOSystemEffectsProperties = pCurrentDevice->GetFxStore();
    sysfxInitParams.pDeviceCollection           = pCurrentDevice->GetDeviceCollection();
    sysfxInitParams.pReserved                   = NULL;

    if (CheckFaultAPOInitialize(pIAPO, sizeof(sysfxInitParams), (BYTE*)&sysfxInitParams))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: APO::Initialize() with (NULL == APOInitSystemEffects."
            "pApoEndpointProperties) causes an exception "
            "(EXCEPTION_ACCESS_VIOLATION).");

        return (FNS_PASS);
    }
    else
    {
        SLOG(eInfo1, "APO::Initialize() with (NULL == "
            "APOInitSystemEffects.pApoEndpointProperties) does not cause "
            "an exception (EXCEPTION_ACCESS_VIOLATION).");
    }

    return (FNS_PASS);
}
