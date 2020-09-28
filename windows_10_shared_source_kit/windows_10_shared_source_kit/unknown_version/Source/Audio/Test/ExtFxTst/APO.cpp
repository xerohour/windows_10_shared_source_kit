// Copyright (C) Microsoft. All rights reserved.
#include "common.h"

typedef HRESULT (__stdcall IAudioProcessingObject:: *FORMAT_SUPPORT_PROC)(IAudioMediaType *pOpposite, IAudioMediaType *pRequested, IAudioMediaType **ppSupported);

BOOL IsFormatListed
(
    IAudioMediaType                    *pIFormat,
    IAudioSystemEffectsCustomFormats   *pICustomFormats
)
{
    HRESULT                 hr;
    BOOL                    fCompressed = FALSE;
    WAVEFORMATEX const     *pwfxSrc = NULL, *pwfxDst = NULL;
    UINT                    cbFormat;
    UINT                    iFormat;

    if (NULL == pIFormat)
    {
        //  WTF?!
        return (FALSE);
    }
    
    if (S_OK != (hr = pIFormat->IsCompressedFormat(&fCompressed)))
    {
        return (FALSE);
    }
    
    if (!fCompressed)
    {
        //  We generally like non-compressed formats...
        return (TRUE);
    }
    
    if (NULL == pICustomFormats)
    {
        //  We have a compressed format, but no interface to enumerate them?!
        return (FALSE);
    }
    
    pwfxSrc  = pIFormat->GetAudioFormat();
    cbFormat = sizeof(WAVEFORMATEX) + pwfxSrc->cbSize;
    
    if (S_OK != (hr = pICustomFormats->GetFormatCount(&iFormat)))
    {
        //  Could not get format count... FAIL!!!
        return (FALSE);
    }
    
    for (; iFormat; --iFormat)
    {
        CComPtr<IAudioMediaType>        pIEnumFormat = NULL;
        
        if (S_OK != (hr = pICustomFormats->GetFormat(iFormat - 1, &pIEnumFormat)))
        {
            return (FALSE);
        }
        
        pwfxDst  = pIEnumFormat->GetAudioFormat();
        
        if (0 == memcmp(pwfxSrc, pwfxDst, cbFormat))
        {
            //  Found a match!  Yay!
            return (TRUE);
        }
    }
    
    //  Went through loop and didn't find a match.  FAIL!
    return (FALSE);
}

DWORD WINAPI TC_General_VerifyInterfaceSupport()
{
    CAPODevice                                     *pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    IUnknown                                       *pIUnknown = NULL;
    CComPtr<IAudioProcessingObject>                 pIAPO = NULL;
    CComPtr<IAudioProcessingObjectRT>               pIAPORT = NULL;
    CComPtr<IAudioProcessingObjectConfiguration>    pIAPOConfig = NULL;
    CComPtr<IAudioSystemEffects>                    pISysFx = NULL;
    DWORD                                           dwRet = FNS_PASS;
    HRESULT                                         hr = S_OK;
    GUID                                            guidAPO;
    APOInitSystemEffects                            APOsystemeffects = {0};

    if (NULL == pCurrentDevice)
    {
        XLOG(XFAIL, eError, "FAIL: NULL == pCurrentDevice.");
        return (FNS_PASS);
    }
    
    if (NULL == (pIUnknown = pCurrentDevice->GetObject()))
    {
        XLOG(XFAIL, eError, "FAIL: Could not instantiate SysFx interface.");
        return (FNS_PASS);
    }
    
    if (S_OK != (hr = pIUnknown->QueryInterface(__uuidof(IAudioProcessingObject), (void**)&pIAPO)))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: SysFx does not support IAudioProcessingObject. Returned error (0x%08lx).", hr );
    }
    else
    {
        SLOG(eInfo1, "APO supports IAudioProcessingObject.");
    }
    
    if (S_OK != (hr = pIUnknown->QueryInterface(__uuidof(IAudioProcessingObjectRT), (void**)&pIAPORT)))
    {
        if (hr == E_NOINTERFACE && pCurrentDevice->IsProxyApo()) 
        {
            SLOG(eInfo1, "Proxy APO does not need to support IAudioProcessingObjectRT.");
        }
        else
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: SysFx does not support IAudioProcessingObjectRT. Returned error (0x%08lx).", hr );
        }
    }
    else
    {
        SLOG(eInfo1, "APO supports IAudioProcessingObjectRT.");
    }
    
    if (S_OK != (hr = pIUnknown->QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), (void**)&pIAPOConfig)))
    {
        if (hr == E_NOINTERFACE && pCurrentDevice->IsProxyApo()) 
        {
            SLOG(eInfo1, "Proxy APO does not need to support IAudioProcessingObjectConfiguration.");
        }
        else
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: SysFx does not support IAudioProcessingObjectConfiguration. Returned error (0x%08lx).", hr );
        }
    }
    else
    {
        SLOG(eInfo1, "APO supports IAudioProcessingObjectConfiguration.");
    }
    
    if (DT_SYSFX & pCurrentDevice->GetDeviceType())
    {
        if (S_OK != (hr = pIUnknown->QueryInterface(__uuidof(IAudioSystemEffects), (void**)&pISysFx)))
        {
            if (hr == E_NOINTERFACE && DT_SMEFX & pCurrentDevice->GetDeviceType()) 
            {
                 SLOG(eInfo1, "SME APO does not need to support APOInitSystemEffects.");
            }
            else
            {
                XLOG(
                    XFAIL,
                    eError,
                    "FAIL: SysFx does not support IAudioSystemEffects. Returned error (0x%08lx).", hr );
            }
        }
        else
        {
            SLOG(eInfo1, "SysFx supports IAudioSystemEffects.");

            pCurrentDevice->GetClsID(&guidAPO);

            //  Setup the Initialization parameters...    
            APOsystemeffects.APOInit.cbSize              = sizeof(APOInitSystemEffects);
            APOsystemeffects.APOInit.clsid               = guidAPO;
            APOsystemeffects.pAPOEndpointProperties      = pCurrentDevice->GetEndpointStore();
            APOsystemeffects.pAPOSystemEffectsProperties = pCurrentDevice->GetFxStore();
            APOsystemeffects.pDeviceCollection           = pCurrentDevice->GetDeviceCollection();
            APOsystemeffects.pReserved                   = NULL;

            if (S_OK != ( hr = pIAPO.CoCreateInstance(guidAPO)))
            {
                XLOG(
                    XFAIL,
                    eError,
                    "FAIL: CoCreateInstance for IAudioProcessingObject returned error (0x%081x).", hr);
            }			         
	    
            if (S_OK != ( hr = pIAPO->Initialize(APOsystemeffects.APOInit.cbSize, (BYTE*)(&APOsystemeffects))))
            {
                if (DT_SMEFX & pCurrentDevice->GetDeviceType()) 
                {
                    SLOG(eInfo1, "SME APO does not need to succeed initialization with APOInitSystemEffects.");
                }
                else
                {
                    XLOG(
                        XFAIL,
                        eError,
                        "FAIL: Initialize APO with APOInitSystemEffects failed. Returned error (0x%08lx).", hr );
                }
            }
        }
    }
    else
    {
        if (E_NOINTERFACE != (hr = pIUnknown->QueryInterface(__uuidof(IAudioSystemEffects), (void**)&pISysFx)))
        {
            SLOG(eError, "FAIL: QueryInterface for IAudioSystemEffects on APO should return E_NOINTERFACE. Returned error (0x%08lx).", hr );
            dwRet = FNS_FAIL;
        }
        else
        {
            SLOG(eInfo1, "APO does not support IAudioSystemEffects.");
        }
    }


    OSVERSIONINFO ver = {0};
    ver.dwOSVersionInfoSize = sizeof(ver);

    if (!GetVersionEx(&ver))
    {
        XLOG(
            XFAIL, 
            eFatalError, 
            "FAIL: GetVersionEx() failed: GetLastError() = %u", GetLastError());
    }

    SLOG(eInfo1, "Windows version %u.%u", ver.dwMajorVersion, ver.dwMinorVersion);

    if (ver.dwMajorVersion < 6 || (ver.dwMajorVersion == 6 && ver.dwMinorVersion < 3))
    {
        // If Version is 6.2 (Windows 8) or below then return here
        SLOG(eInfo1, "Windows version is below 6.3 skipping last tests");
        return (dwRet);
    }

    CComPtr<IAudioSystemEffects2>                   pISysFx2 = NULL;
    APOInitSystemEffects2                           APOsystemeffects2 = {0};
    LPGUID                                          pEffectsIds = NULL;
    UINT                                            cEffects = 0;

    if (DT_SYSFX & pCurrentDevice->GetDeviceType())
    {
        if (S_OK != (hr = pIUnknown->QueryInterface(__uuidof(IAudioSystemEffects2), (void**)&pISysFx2)))
        {
            if (hr == E_NOINTERFACE && DT_LGFX & pCurrentDevice->GetDeviceType())
            {
                SLOG(eInfo1, "LFX or GFX APO does not need to support IAudioSystemEffects2.");
            }
            else
            {
                XLOG(
                    XFAIL,
                    eError,
                    "FAIL: SysFx does not support IAudioSystemEffects2. Returned error (0x%08lx).", hr);
            }
        }
        else
        {
            SLOG(eInfo1, "SysFx supports IAudioSystemEffects2.");		

            if (S_OK != (hr = pISysFx2->GetEffectsList(&pEffectsIds, &cEffects , NULL)))
            {	
                XLOG(XFAIL, eError, "FAIL: IAudioSystemEffects2->GetEffectsList failed. Returned error (0x%08lx).", hr);
            }
            else
            {
                SLOG(eInfo1, "SysFX supports IAudioSystemEffects2->GetEffectsList.");
            }

            pCurrentDevice->GetClsID(&guidAPO);


            APOsystemeffects2.APOInit.cbSize                = sizeof(APOInitSystemEffects2);
            APOsystemeffects2.APOInit.clsid                 = guidAPO;
            APOsystemeffects2.pReserved                     = NULL;
            APOsystemeffects2.pAPOEndpointProperties        = pCurrentDevice->GetEndpointStore();
            APOsystemeffects2.pAPOSystemEffectsProperties   = pCurrentDevice->GetFxStore();
            APOsystemeffects2.pDeviceCollection             = pCurrentDevice->GetDeviceCollection();
            APOsystemeffects2.InitializeForDiscoveryOnly    = TRUE;
            APOsystemeffects2.AudioProcessingMode           = AUDIO_SIGNALPROCESSINGMODE_DEFAULT;
            APOsystemeffects2.nSoftwareIoDeviceInCollection = pCurrentDevice->GetSoftwareIoDeviceInCollection();
            APOsystemeffects2.nSoftwareIoConnectorIndex     = pCurrentDevice->GetSoftwareIoConnectorIndex();
            
            CComPtr<IAudioProcessingObject> pIAPODefaultForDiscovery = NULL;
            if (S_OK != ( hr = pIAPODefaultForDiscovery.CoCreateInstance(guidAPO)))
            {
                XLOG(
                    XFAIL,
                    eError,
                    "FAIL: CoCreateInstance for IAudioProcessingObject returned error (0x%081x).", hr);
            }


            if (S_OK != ( hr = pIAPODefaultForDiscovery->Initialize(APOsystemeffects2.APOInit.cbSize, (BYTE*)(&APOsystemeffects2))))
            {
                XLOG(
                    XFAIL,
                    eError,
                    "FAIL: Initialize APO with APOInitSystemEffects2 with processingmode default and for discovery. Returned error (0x%08lx).", hr);
            }

            // if APO is a Proxy APO it should support discovery for raw also
            if (pCurrentDevice->IsProxyApo())
            {
                APOsystemeffects2.APOInit.cbSize                = sizeof(APOInitSystemEffects2);
                APOsystemeffects2.APOInit.clsid                 = guidAPO;
                APOsystemeffects2.pReserved                     = NULL;
                APOsystemeffects2.pAPOEndpointProperties        = pCurrentDevice->GetEndpointStore();
                APOsystemeffects2.pAPOSystemEffectsProperties   = pCurrentDevice->GetFxStore();
                APOsystemeffects2.pDeviceCollection             = pCurrentDevice->GetDeviceCollection();
                APOsystemeffects2.InitializeForDiscoveryOnly    = TRUE;
                APOsystemeffects2.AudioProcessingMode           = AUDIO_SIGNALPROCESSINGMODE_RAW;
                APOsystemeffects2.nSoftwareIoDeviceInCollection = pCurrentDevice->GetSoftwareIoDeviceInCollection();
                APOsystemeffects2.nSoftwareIoConnectorIndex     = pCurrentDevice->GetSoftwareIoConnectorIndex();

                CComPtr<IAudioProcessingObject> pIAPORawForDiscovery = NULL;
                if (S_OK != ( hr = pIAPORawForDiscovery.CoCreateInstance(guidAPO)))
                {
                    XLOG(
                        XFAIL,
                        eError,
                        "FAIL: CoCreateInstance for IAudioProcessingObject returned error (0x%081x).", hr);
                }

                if (S_OK != ( hr = pIAPORawForDiscovery->Initialize(APOsystemeffects2.APOInit.cbSize, (BYTE*)(&APOsystemeffects2))))
                {
                    XLOG(
                        XFAIL,
                        eError,
                        "FAIL: Initialize APO with APOInitSystemEffects2 with processingmode raw and for discovery. Returned error (0x%08lx).", hr);
                }
            }

            // if APO is not a Proxy APO let's try to initialize it for default
            if (!pCurrentDevice->IsProxyApo())
            {
                APOsystemeffects2.APOInit.cbSize              = sizeof(APOInitSystemEffects2);
                APOsystemeffects2.APOInit.clsid               = guidAPO;
                APOsystemeffects2.pReserved                   = NULL;
                APOsystemeffects2.pAPOEndpointProperties      = pCurrentDevice->GetEndpointStore();
                APOsystemeffects2.pAPOSystemEffectsProperties = pCurrentDevice->GetFxStore();
                APOsystemeffects2.pDeviceCollection           = pCurrentDevice->GetDeviceCollection();
                APOsystemeffects2.InitializeForDiscoveryOnly  = FALSE;
                APOsystemeffects2.AudioProcessingMode         = AUDIO_SIGNALPROCESSINGMODE_DEFAULT;

                CComPtr<IAudioProcessingObject> pIAPODefault  = NULL;
                if (S_OK != ( hr = pIAPODefault.CoCreateInstance(guidAPO)))
                {
                    XLOG(
                        XFAIL,
                        eError,
                        "FAIL: CoCreateInstance for IAudioProcessingObject returned error (0x%081x).", hr);
                }

                if (S_OK != ( hr = pIAPODefault->Initialize(APOsystemeffects2.APOInit.cbSize, (BYTE*)(&APOsystemeffects2))))
                {
                    XLOG(
                        XFAIL,
                        eError,
                        "FAIL: Initialize APO with APOInitSystemEffects2 with processingmode default. Returned error (0x%08lx).", hr);
                }
            }
        }
    }
    else
    {
        if (E_NOINTERFACE != (hr = pIUnknown->QueryInterface(__uuidof(IAudioSystemEffects2), (void**)&pISysFx2)))
        {
            SLOG(eError, "FAIL: QUeryInterface for IAudioSystemEffects2 on APO should return E_NOINTERFACE. Returned error (0x%08lx).", hr);
            dwRet = FNS_FAIL;
        }
        else
        {
            SLOG(eInfo1, "APO does not support IAudioSystemEffects2.");
        }
    }

return (dwRet);
}

DWORD WINAPI TC_IAudioProcessingObject_GetLatency()
{
    CAPODevice                                     *pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    CComPtr<IAudioProcessingObject>                 pIAPO = NULL;
    CComPtr<IAudioProcessingObjectRT>               pIAPORT = NULL;
    CComPtr<IAudioProcessingObjectConfiguration>    pIAPOConfig = NULL;
    APO_CONNECTION_DESCRIPTOR                      *pInputConnections = NULL;
    APO_CONNECTION_DESCRIPTOR                      *pOutputConnections = NULL;
    PAPO_REG_PROPERTIES                             pProperties;
    UINT                                            cConnections, cInputs, cOutputs;
    CTestFormats                                    TestFormats;
    UINT                                            cFormats;
    UINT                                            ii, jj;
    char                                            szFormat[256];
    HRESULT                                         hr;
    MFTIME                                          mfTime;
    
    if (NULL == pCurrentDevice)
    {
        XLOG(XFAIL, eError, "FAIL: Could not instantiate SysFx object.");
        return (FNS_PASS);
    }

    if (pCurrentDevice->IsProxyApo())
    {
        XLOG(
            XSKIP,
            eInfo1,
            "Skip: Testcase should not run on Proxy APO.");
        return (FNS_SKIPPED);
    }
    
    if (!pCurrentDevice->GetAPOInterfaces(&pIAPO, &pIAPORT, &pIAPOConfig))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObject interface.");
            
        return (FNS_PASS);
    }
    
    if (NULL == pIAPO)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObject interface.");
            
        return (FNS_PASS);
    }

    if (NULL == pIAPORT)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObjectRT interface.");
            
        return (FNS_PASS);
    }

    if (NULL == pIAPOConfig)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObjectConfiguration "
            "interface.");
            
        return (FNS_PASS);
    }




    if (NULL == (pProperties = pCurrentDevice->GetProperties()))
    {
        SLOG(eInfo1, "Skipping test - Could not get registration properties.");
        return (FNS_PASS);
    }
     
    cInputs      = pProperties->u32MinInputConnections;
    cOutputs     = pProperties->u32MinOutputConnections;
    cConnections = cInputs + cOutputs;
    
    //  Check for overflow...
    if ((cConnections < cInputs) || (cConnections < cOutputs))
    {
        XLOG(XFAIL, eError, "FAIL: We overflowed count of total connections?");
        return (FNS_PASS);
    }
    
    if (NULL == (pInputConnections = new APO_CONNECTION_DESCRIPTOR[cConnections]))
    {
        XLOG(XFAIL, eError, "FAIL: Insufficient memory");
        return (FNS_PASS);
    }

    pOutputConnections = &(pInputConnections[cInputs]);
    
    //  Note:  Ordinarily the last format is NULL...  However, we're special
    //         casing that format to be the mix format, so be very careful about
    //         tweaking the count here.
    cFormats = TestFormats.GetCount();
    for (ii = cFormats; ii; --ii)
    {
        CComPtr<IAudioMediaType>        pIRequestedFormat = NULL;
        CComPtr<IAudioMediaType>        pISupportedInputFormat = NULL;
        CComPtr<IAudioMediaType>        pISupportedOutputFormat = NULL;
        CBufferCollection               Buffers(cConnections);
        CAudioBuffer                   *pBuffer;
        FORMAT_SUPPORT_PROC             pfnFirst = NULL;
        FORMAT_SUPPORT_PROC             pfnSecond = NULL;
        char                            szFirst[25];
        char                            szSecond[25];
        IAudioMediaType               **ppIAMTFirst = NULL;
        IAudioMediaType               **ppIAMTSecond = NULL;
        
        if (!Buffers.IsValid())
        {
            XLOG(XFAIL, eError, "FAIL: Insufficient memory");
            continue;
        }
        
        //  Special casing this to get the mix format...
        if (cFormats == ii)
        {
            hr = pCurrentDevice->GetMixFormat(&pIRequestedFormat);
        }
        else
        {
            hr = TestFormats.GetAMTFormat(ii - 1, &pIRequestedFormat);
        }
        
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: Unable to get format.");
            continue;
        }
    
        //  Determine from which side to negotiate formats...
        if (pCurrentDevice->NegotiateFormatsFromInput())
        {
            pfnFirst     = &IAudioProcessingObject::IsInputFormatSupported;
            pfnSecond    = &IAudioProcessingObject::IsOutputFormatSupported;
            ppIAMTFirst  = &pISupportedInputFormat;
            ppIAMTSecond = &pISupportedOutputFormat;
            lstrcpy(szFirst,  "IsInputFormatSupported");
            lstrcpy(szSecond, "IsOutputFormatSupported");
        }
        else
        {
            pfnFirst     = &IAudioProcessingObject::IsOutputFormatSupported;
            pfnSecond    = &IAudioProcessingObject::IsInputFormatSupported;
            ppIAMTFirst  = &pISupportedOutputFormat;
            ppIAMTSecond = &pISupportedInputFormat;
            lstrcpy(szFirst,  "IsOutputFormatSupported");
            lstrcpy(szSecond, "IsInputFormatSupported");
        }
    
        //  Start format negotiation without opposed format.
        hr = (pIAPO->*pfnFirst)(NULL, pIRequestedFormat, ppIAMTFirst);
        
        //  Special casing the 'not supported' error...
        if (APOERR_FORMAT_NOT_SUPPORTED == hr)
        {
            if (cFormats == ii)
            {
                //  This is the mix format...  You MUST support this format!
                XLOG(
                    XFAIL,
                    eError,
                    "FAIL: With requested format of MixFormat [%s], %s "
                        "returned error (0x%08lx).",
                    GetFormatString(pIRequestedFormat, szFormat),
                    szFirst,
                    hr);
        
                continue;
            }
            else
            {
                //  APO does not support the format and it's not the mix format.
                //  This may be perfectly legit.
        
                SLOG(
                    eInfo1,
                    "With requested format of [%s], %s returned error "
                        "(0x%08lx).",
                    GetFormatString(pIRequestedFormat, szFormat),
                    szFirst,
                    hr);
                
                continue;
            }
        }
    
        //  Failing any other general error...
        if ((S_OK != hr) && (S_FALSE != hr))
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: With requested format of [%s], %s returned error "
                    "(0x%08lx).",
                GetFormatString(pIRequestedFormat, szFormat),
                szFirst,
                hr);
    
            continue;
        }
        
        //  At this point the only conditions should be S_FALSE and S_OK,
        //      and either one of those should suggest a format.
        hr = (pIAPO->*pfnSecond)(*ppIAMTFirst, pIRequestedFormat, ppIAMTSecond);
        
        if ((S_OK != hr) && (S_FALSE != hr))
        {
            char    szFormat2[256];
        
            XLOG(
                XFAIL,
                eError,
                "FAIL: With requested format of [%s] and opposed format "
                    "of [%s] %s returned error (0x%08lx).",
                 GetFormatString(pIRequestedFormat, szFormat),
                 GetFormatString(*ppIAMTFirst, szFormat2),
                 szSecond,
                 hr);
                 
            continue;
        }
    
        //  Okay at this point we have two formats that we KNOW work!!
        for (jj = cConnections; jj; --jj)
        {
            if (jj <= cInputs)
            {
                pBuffer = new CAudioBuffer(pISupportedInputFormat);
                Buffers.SetBuffer(jj - 1, pBuffer);
                if ((NULL == pBuffer) || (!pBuffer->GenerateBuffer(1.0)))
                {
                    break;
                }
            }
            else
            {
                pBuffer = new CAudioBuffer(pISupportedOutputFormat);
                Buffers.SetBuffer(jj - 1, pBuffer);
                if ((NULL == pBuffer) || (!pBuffer->AllocateBuffer(1.0)))
                {
                    break;
                }
            }
        }
        
        if (0 != jj)
        {
            XLOG(XFAIL, eError, "FAIL: Insufficient memory.");
            continue;
        }
        
        for (jj = cConnections; jj; --jj)
        {
            UNCOMPRESSEDAUDIOFORMAT   format;
        
            pBuffer = Buffers.GetBuffer(jj - 1);
            pInputConnections[jj - 1].Type              = APO_CONNECTION_BUFFER_TYPE_EXTERNAL;
            pInputConnections[jj - 1].pBuffer           = (UINT_PTR)pBuffer->GetAudioBuffer();
            pInputConnections[jj - 1].pFormat           = pBuffer->GetFormat();
            pInputConnections[jj - 1].u32Signature      = APO_CONNECTION_DESCRIPTOR_SIGNATURE;
            
            if (S_OK != (hr = (pInputConnections[jj - 1].pFormat)->GetUncompressedAudioFormat(&format)))
            {
                XLOG(
                    XFAIL,
                    eError,
                    "FAIL: IAudioMediaType::GetUncompressedAudioFormat "
                        "returned error (0x%08lx)",
                    hr);
                    
                break;
            }
            
            pInputConnections[jj - 1].u32MaxFrameCount  = pBuffer->GetBufferSize() / (format.dwSamplesPerFrame * format.dwBytesPerSampleContainer);
        }
        
        if (0 != jj)
        {
            continue;
        }
        
        if (S_OK != (hr = pIAPOConfig->LockForProcess(cInputs, &pInputConnections, cOutputs, &pOutputConnections)))
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: pIAudioProcessingObjectConfiguration::LockForProcess "
                    "returned error (0x%08lx).",
                hr);
                
            continue;
        }
        else
        {
            SLOG(eInfo1, "pIAudioProcessingObjectConfiguration::LockForProcess succeeds.");
            SLOG(eInfo1, "  Supported InputFormat: %s", GetFormatString(pISupportedInputFormat, szFormat));
            SLOG(eInfo1, "  Supported OutputFormat: %s", GetFormatString(pISupportedOutputFormat, szFormat));
        }

        if (S_OK != (hr = pIAPO->GetLatency(&mfTime)))
        {
            XLOG(XFAIL, eError, "FAIL: GetLatency returned error (0x%08lx).", hr);
        }
        else
        {
            SLOG(eInfo1, "Latency: %d", (DWORD)mfTime);
        }
        
        pIAPOConfig->UnlockForProcess();
    }
    
    delete []pInputConnections;
    return (FNS_PASS);
}

DWORD WINAPI TC_IAudioProcessingObject_GetRegistrationProperties()
{
    CAPODevice                         *pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    CComPtr<IAudioProcessingObject>     pIAPO = NULL;
    PAPO_REG_PROPERTIES                 pApoRegProperties;
    HRESULT                             hr;
    
    OSVERSIONINFO ver = {0};
    ver.dwOSVersionInfoSize = sizeof(ver);

    if (!GetVersionEx(&ver))
    {
        XLOG(
            XFAIL, 
            eFatalError, 
            "FAIL: GetVersionEx() failed: GetLastError() = %u", GetLastError());
    }

    if (NULL == pCurrentDevice)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate SysFx object.");
            
        return (FNS_PASS);
    }

    if (!pCurrentDevice->GetAPOInterfaces(&pIAPO, NULL, NULL))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObject interface.");
            
        return (FNS_PASS);
    }
    
    if (NULL == pIAPO)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObject interface.");
            
        return (FNS_PASS);
    }

    // GetRegistrationProperties is deprecated from Windows 8.1 
    if (S_OK != (hr = pIAPO->GetRegistrationProperties(&pApoRegProperties)))
    {
        if (ver.dwMajorVersion < 6 || (ver.dwMajorVersion == 6 && ver.dwMinorVersion < 3))
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: GetRegistrationProperties returned error (0x%08lx).",
                hr);
        } 
        else
        {
            SLOG(eInfo1, "IAPO::GetRegistrationProperties is deprecated in 6.3 and higher.");
        }
    }
    else
    {
        SLOG(eInfo1, "Call to IAPO::GetRegistrationProperties successful.");
    }
    Log_ApoRegProperties(pApoRegProperties);
    CoTaskMemFree(pApoRegProperties);

    return (FNS_PASS);
}

DWORD WINAPI TC_IAudioProcessingObject_VerifyInputFormatSupport()
{
    CAPODevice                                     *pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    CComPtr<IAudioProcessingObject>                 pIAPO = NULL;
    CComPtr<IAudioProcessingObjectRT>               pIAPORT = NULL;
    CComPtr<IAudioProcessingObjectConfiguration>    pIAPOConfig = NULL;
    CComPtr<IAudioSystemEffectsCustomFormats>       pICustomFormats = NULL;
    PAPO_REG_PROPERTIES                             pProperties;
    CTestFormats                                    TestFormats;
    UINT                                            ii, jj;
    char                                            szFormat[256];
    DWORD                                           dwDeviceType = pCurrentDevice->GetDeviceType();
    HRESULT                                         hr;
    UINT                                            uLog;
    
    if (NULL == pCurrentDevice)
    {
        XLOG(XFAIL, eError, "FAIL: Could not instantiate SysFx object.");
        return (FNS_PASS);
    }
    
    if (pCurrentDevice->IsProxyApo())
    {
        XLOG(
            XSKIP,
            eInfo1,
            "Skip: Testcase should not run on Proxy APO.");
        return (FNS_SKIPPED);
    }

    if (!pCurrentDevice->GetAPOInterfaces(&pIAPO, &pIAPORT, &pIAPOConfig))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObject interface.");
        
        return (FNS_PASS);
    }
    
    if (NULL == pIAPO)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObject interface.");
        
        return (FNS_PASS);
    }

    if (NULL == pIAPORT)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObjectRT interface.");
            
        return (FNS_PASS);
    }

    if (NULL == pIAPOConfig)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObjectConfiguration "
                "interface.");
                
        return (FNS_PASS);
    }
    



    if (NULL == (pProperties = pCurrentDevice->GetProperties()))
    {
        SLOG(eInfo1, "Skipping test - Could not get registration properties.");
        return (FNS_PASS);
    }
    
    //  Never check for an error condition you don't care about...
    pIAPO->QueryInterface(__uuidof(IAudioSystemEffectsCustomFormats), (void**)&pICustomFormats);
    
    //  Note:  We're spanning the entire count to include NULL formats.
    for (ii = TestFormats.GetCount(); ii; --ii)
    {
        CComPtr<IAudioMediaType>      pIOutputFormat = NULL;
        
        hr = TestFormats.GetAMTFormat(ii - 1, &pIOutputFormat);
        
        if (S_OK != hr)
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: Unable to find IAudioMediaType format (0x%08lx).",
                hr);
        
            continue;
        }
    
        //  Note:  We're omitting the last count to omit NULL formats.
        for (jj = TestFormats.GetCount() - 1; jj; --jj)
        {
            CComPtr<IAudioMediaType>      pIInputFormat = NULL;
            CComPtr<IAudioMediaType>      pISupportedInputFormat = NULL;
            
            hr = TestFormats.GetAMTFormat(jj - 1, &pIInputFormat);
            
            if (S_OK != hr)
            {
                XLOG(
                    XFAIL,
                    eError,
                    "FAIL: Unable to find IAudioMediaType format (0x%08lx).",
                    hr);
            
                continue;
            }
            
            hr   = pIAPO->IsInputFormatSupported(pIOutputFormat, pIInputFormat, &pISupportedInputFormat);
            uLog = eInfo1;
            
            if (S_OK == hr)
            {
                //  Succeeds with requested format.
                SLOG(eInfo1, "\r\nIAudioProcessingObject::IsInputFormatSupported succeeds (returns S_OK) with requested format:");
                
                //  Verify a format is returned.
                if (NULL == pISupportedInputFormat)
                {
                    XLOG(
                        XFAIL,
                        eError,
                        "FAIL: After returning S_OK, no supported format is "
                            "returned (NULL).");
                        
                    uLog  = eError;
                }
                else
                {
                    //  Verify requested and supported input formats are similar.
                    if (IsEqualFormat(pIInputFormat, pISupportedInputFormat))
                    {
                        SLOG(eInfo1, "Verified that requested and suggested formats are similar after returning S_OK.");
                    }
                    else
                    {
                        XLOG(
                            XFAIL,
                            eError,
                            "FAIL: After returning S_OK, requested and "
                                "suggested formats are different.");
                        
                        uLog  = eError;
                    }
                }
            }
            else if (S_FALSE == hr)
            {
                uLog = eInfo1;
            
                //  Succeeds with suggested format.
                SLOG(eInfo1, "\r\nIAudioProcessingObject::IsInputFormatSupported succeeds (returns S_FALSE) with suggested format:");
                
                //  Verify a format is returned.
                if (NULL == pISupportedInputFormat)
                {
                    XLOG(
                        XFAIL,
                        eError,
                        "FAIL: After returning S_FALSE, no supported format "
                            "is returned (NULL).");
                            
                    uLog  = eError;
                }
                else
                {
                    //  Verify requested and suggest input formats are different.
                    if (!IsEqualFormat(pIInputFormat, pISupportedInputFormat))
                    {
                        SLOG(eInfo1, "Verified that requested and suggested formats are different after returning S_FALSE.");
                    }
                    else
                    {
                        XLOG(
                            XFAIL,
                            eError,
                            "FAIL: After returning S_FALSE, requested and "
                                "suggested formats are similar.");
                                
                        uLog  = eError;
                    }
                }
            }
            else if (APOERR_FORMAT_NOT_SUPPORTED == hr)
            {
                SLOG(eInfo1, "IAudioProcessingObject::IsInputFormatSupported returns APOERR_FORMAT_NOT_SUPPORTED; legitimately does not support format.");
                SLOG(eInfo1, "  OutputFormat: %s", GetFormatString(pIOutputFormat, szFormat));
                SLOG(eInfo1, "  Requested InputFormat: %s", GetFormatString(pIInputFormat, szFormat));
                continue;
            }
            else
            {
                //  Returns error condition; we should never get here.
                XLOG(
                    XFAIL,
                    eError,
                    "\r\nFAIL: IAudioProcessingObject::IsInputFormatSupported "
                        "returned error (0x%08lx):",
                    hr);
                    
                uLog  = eError;
            }
            
            //  Verify format is either uncompressed or listed among custom formats...
            if ((S_OK == hr) || (S_FALSE == hr))
            {
                //  In either of these return values, we should have a return format.
                if (IsFormatListed(pISupportedInputFormat, pICustomFormats))
                {
                    SLOG(eInfo1, "Verified suggested format is either uncompressed or is listed among custom formats.");
                }
                else
                {
                    XLOG(
                        XFAIL,
                        eError,
                        "FAIL: Suggested format is compressed, but not listed "
                            "among custom formats.");
                            
                    uLog  = eError;
                }
            }
            
            //  Verify supported format correctly fills format structures.
            if (NULL != pISupportedInputFormat)
            {
                SLOG(eInfo1, "Verifying supported format sets all fields in UNCOMPRESSEDAUDIOFORMAT upon call to GetUncompressedAudioFormat.");
                if (!VerifyFormatFill(pISupportedInputFormat))
                {
                    //  Failure output in above function.
                    uLog  = eError;
                }
            }
            
            //  Verify matching flags from registration properties.
            if ((NULL != pIOutputFormat) && (NULL != pISupportedInputFormat))
            {
                WAVEFORMATEX const     *pwfxInput, *pwfxOutput;
                
                pwfxInput  = pISupportedInputFormat->GetAudioFormat();
                pwfxOutput = pIOutputFormat->GetAudioFormat();
            
                if (APO_FLAG_SAMPLESPERFRAME_MUST_MATCH & pProperties->Flags)
                {
                    if (pwfxInput->nChannels == pwfxOutput->nChannels)
                    {
                        SLOG(eInfo1, "Verified input and output channels match when APO_FLAG_SAMPLESPERFRAME_MUST_MATCH is set.");
                    }
                    else
                    {
                        XLOG(
                            XFAIL,
                            eError,
                            "FAIL: input and output channels differ when "
                                "APO_FLAG_SAMPLESPERFRAME_MUST_MATCH is set.");
                        
                        uLog  = eError;
                    }
                }
                
                if (APO_FLAG_FRAMESPERSECOND_MUST_MATCH & pProperties->Flags)
                {
                    if (pwfxInput->nSamplesPerSec == pwfxOutput->nSamplesPerSec)
                    {
                        SLOG(eInfo1, "Verified input and output samples per second match when APO_FLAG_FRAMESPERSECOND_MUST_MATCH is set.");
                    }
                    else
                    {
                        XLOG(
                            XFAIL,
                            eError,
                            "FAIL: input and output samples per second differ "
                                "when APO_FLAG_FRAMESPERSECOND_MUST_MATCH is "
                                "set.");
                        
                        uLog  = eError;
                    }
                }
                
                if (APO_FLAG_BITSPERSAMPLE_MUST_MATCH & pProperties->Flags)
                {
                    if (pwfxInput->wFormatTag == pwfxOutput->wFormatTag)
                    {
                        if (pwfxInput->wBitsPerSample == pwfxOutput->wBitsPerSample)
                        {
                            SLOG(eInfo1, "Verified input and output bits per sample match when APO_FLAG_BITSPERSAMPLE_MUST_MATCH is set.");
                        }
                        else
                        {
                            XLOG(
                                XFAIL,
                                eError,
                                "FAIL: input and output bits per sample "
                                    "differ when APO_FLAG_BITSPERSAMPLE_MUST_"
                                    "MATCH is set.");
                            
                            uLog  = eError;
                        }
                    }
                }
            }
            
            //  If GFX, verify suggested format is not compressed and float32...
            if ((DT_GFX == dwDeviceType) && (NULL != pISupportedInputFormat))
            {
                WAVEFORMATEX const     *pwfxInput;
                BOOL                    IsFloat = FALSE;
            
                pwfxInput  = pISupportedInputFormat->GetAudioFormat();
                
                if (WAVE_FORMAT_IEEE_FLOAT == pwfxInput->wFormatTag)
                {
                    IsFloat = TRUE;
                }
                else if (WAVE_FORMAT_EXTENSIBLE == pwfxInput->wFormatTag)
                {
                    WAVEFORMATEXTENSIBLE    *pwfext = (WAVEFORMATEXTENSIBLE*)pwfxInput;
                
                    if (IsEqualGUID(pwfext->SubFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT))
                    {
                        IsFloat = TRUE;
                    }
                }
            
                if (NULL == pIOutputFormat)
                {
                    //  If we don't have an opposite format, then we can be
                    //  very strict, it must be float32.
                
                    if (IsFloat)
                    {
                        SLOG(eInfo1, "Verified GFX suggested format is float32.");
                    }
                    else
                    {
                        XLOG(
                            XFAIL,
                            eError,
                            "FAIL: GFX suggested format is not float32.");
                            
                        uLog  = eError;
                    }
                }
                else if (0 == (APO_FLAG_BITSPERSAMPLE_MUST_MATCH & pProperties->Flags))
                {
                    //  If the SysFx doesn't require the bits per sample to
                    //  match, then they must return float32.
                
                    if (IsFloat)
                    {
                        SLOG(eInfo1, "Verified GFX suggested format is float32.");
                    }
                    else
                    {
                        XLOG(
                            XFAIL,
                            eError,
                            "FAIL: GFX suggested format is not float32.");
                            
                        uLog  = eError;
                    }
                }
                else
                {
                    SLOG(eInfo1, "Specified opposite format and bits per sample must match output format; non-float32 format allowed.");
                }
            }
            
            SLOG(uLog, "  OutputFormat: %s", GetFormatString(pIOutputFormat, szFormat));
            SLOG(uLog, "  Requested InputFormat: %s", GetFormatString(pIInputFormat, szFormat));
            SLOG(uLog, "  Suggested InputFormat: %s", GetFormatString(pISupportedInputFormat, szFormat));
        }
    }
    
    return (FNS_PASS);
}

DWORD WINAPI TC_IAudioProcessingObject_VerifyOutputFormatSupport()
{
    CAPODevice                                     *pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    CComPtr<IAudioProcessingObject>                 pIAPO = NULL;
    CComPtr<IAudioProcessingObjectRT>               pIAPORT = NULL;
    CComPtr<IAudioProcessingObjectConfiguration>    pIAPOConfig = NULL;
    CComPtr<IAudioSystemEffectsCustomFormats>       pICustomFormats = NULL;
    PAPO_REG_PROPERTIES                             pProperties;
    CTestFormats                                    TestFormats;
    DWORD                                           dwDeviceType = pCurrentDevice->GetDeviceType();
    UINT                                            ii, jj;
    char                                            szFormat[256];
    HRESULT                                         hr;
    UINT                                            uLog;
    
    if (NULL == pCurrentDevice)
    {
        XLOG(XFAIL, eError, "FAIL: Could not instantiate SysFx object.");
        return (FNS_PASS);
    }
    
    if (pCurrentDevice->IsProxyApo())
    {
        XLOG(
            XSKIP,
            eInfo1,
            "Skip: Testcase should not run on Proxy APO.");
        return (FNS_SKIPPED);
    } 
    
    if (!pCurrentDevice->GetAPOInterfaces(&pIAPO, &pIAPORT, &pIAPOConfig))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObject interface.");
            
        return (FNS_PASS);
    }
    
    if (NULL == pIAPO)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObject interface.");
            
        return (FNS_PASS);
    }

    if (NULL == pIAPORT)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObjectRT interface.");
            
        return (FNS_PASS);
    }

    if (NULL == pIAPOConfig)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObjectConfiguration "
                "interface.");
                
        return (FNS_PASS);
    }
    



    if (NULL == (pProperties = pCurrentDevice->GetProperties()))
    {
        SLOG(eInfo1, "Skipping test - Could not get registration properties.");
        return (FNS_PASS);
    }
    
    //  Never check for an error condition you don't care about...
    pIAPO->QueryInterface(__uuidof(IAudioSystemEffectsCustomFormats), (void**)&pICustomFormats);
    
    //  Note:  We're spanning the entire count to include NULL formats.
    for (ii = TestFormats.GetCount(); ii; --ii)
    {
        CComPtr<IAudioMediaType>      pIInputFormat = NULL;
        
        hr = TestFormats.GetAMTFormat(ii - 1, &pIInputFormat);
        
        if (S_OK != hr)
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: Unable to find IAudioMediaType format (0x%08lx).",
                hr);
        
            continue;
        }
        
        //  Note:  We're omitting the last count to omit NULL formats.
        for (jj = TestFormats.GetCount() - 1; jj; --jj)
        {
            CComPtr<IAudioMediaType>      pIOutputFormat = NULL;
            CComPtr<IAudioMediaType>      pISupportedOutputFormat = NULL;
            
            hr = TestFormats.GetAMTFormat(jj - 1, &pIOutputFormat);
            
            if (S_OK != hr)
            {
                XLOG(
                    XFAIL,
                    eError,
                    "FAIL: Unable to get IAudioMediaType format (0x%08lx).",
                    hr);
            
                continue;
            }
            
            hr   = pIAPO->IsOutputFormatSupported(pIInputFormat, pIOutputFormat, &pISupportedOutputFormat);
            uLog = eInfo1;
            
            if (S_OK == hr)
            {
                //  Succeeds with requested format.
                SLOG(eInfo1, "\r\nIAudioProcessingObject::IsOutputFormatSupported succeeds (returns S_OK) with requested format:");
                
                //  Verify a format is returned.
                if (NULL == pISupportedOutputFormat)
                {
                    XLOG(
                        XFAIL,
                        eError,
                        "FAIL: After returning S_OK, no supported format is "
                            "returned (NULL).");
                            
                    uLog  = eError;
                }
                else
                {
                    //  Verify requested and supported input formats are similar.
                    if (IsEqualFormat(pIOutputFormat, pISupportedOutputFormat))
                    {
                        SLOG(eInfo1, "Verified that requested and suggested formats are similar after returning S_OK.");
                    }
                    else
                    {
                        XLOG(
                            XFAIL,
                            eError,
                            "FAIL: After returning S_OK, requested and "
                                "suggested formats are different.");
                                
                        uLog  = eError;
                    }
                }
            }
            else if (S_FALSE == hr)
            {
                uLog = eInfo1;
            
                //  Succeeds with suggested format.
                SLOG(eInfo1, "\r\nIAudioProcessingObject::IsOutputFormatSupported succeeds (returns S_FALSE) with suggested format:");
                
                //  Verify a format is returned.
                if (NULL == pISupportedOutputFormat)
                {
                    XLOG(
                        XFAIL,
                        eError,
                        "FAIL: After returning S_OK, no supported format is "
                            "returned (NULL).");
                            
                    uLog  = eError;
                }
                else
                {
                    //  Verify requested and suggest input formats are different.
                    if (!IsEqualFormat(pIOutputFormat, pISupportedOutputFormat))
                    {
                        SLOG(eInfo1, "Verified that requested and suggested formats are different after returning S_FALSE.");
                    }
                    else
                    {
                        XLOG(
                            XFAIL,
                            eError,
                            "FAIL: After returning S_FALSE, requested and "
                                "suggested formats are similar.");
                                
                        uLog  = eError;
                    }
                }
            }
            else if (APOERR_FORMAT_NOT_SUPPORTED == hr)
            {
                SLOG(eInfo1, "IAudioProcessingObject::IsOutputFormatSupported returns APOERR_FORMAT_NOT_SUPPORTED; legitimately does not support format.");
                SLOG(eInfo1, "  OutputFormat: %s", GetFormatString(pIOutputFormat, szFormat));
                SLOG(eInfo1, "  Requested InputFormat: %s", GetFormatString(pIInputFormat, szFormat));
                continue;
            }
            else
            {
                //  Returns error condition; we should never get here.
                XLOG(
                    XFAIL,
                    eError,
                    "\r\nFAIL: IAudioProcessingObject::IsOutputFormatSupported "
                        "returned error (0x%08lx):",
                    hr);
                    
                uLog  = eError;
            }
            
            //  Verify format is either uncompressed or listed among custom formats...
            if ((S_OK == hr) || (S_FALSE == hr))
            {
                //  In either of these return values, we should have a return format.
                if (IsFormatListed(pISupportedOutputFormat, pICustomFormats))
                {
                    SLOG(eInfo1, "Verified suggested format is either uncompressed or is listed among custom formats.");
                }
                else
                {
                    XLOG(
                        XFAIL,
                        eError,
                        "FAIL: Suggested format is compressed, but not listed "
                            "among custom formats.");
                        
                    uLog  = eError;
                }
            }
            
            //  Verify supported format correctly fills format structures.
            if (NULL != pISupportedOutputFormat)
            {
                SLOG(eInfo1, "Verifying supported format sets all fields in UNCOMPRESSEDAUDIOFORMAT upon call to GetUncompressedAudioFormat.");
                if (!VerifyFormatFill(pISupportedOutputFormat))
                {
                    //  Failure output in above function.
                    uLog  = eError;
                }
            }
            
            //  Verify matching flags from registration properties.
            if ((NULL != pIInputFormat) && (NULL != pISupportedOutputFormat))
            {
                WAVEFORMATEX const     *pwfxInput, *pwfxOutput;
                
                pwfxInput  = pIInputFormat->GetAudioFormat();
                pwfxOutput = pISupportedOutputFormat->GetAudioFormat();
            
                if (APO_FLAG_SAMPLESPERFRAME_MUST_MATCH & pProperties->Flags)
                {
                    if (pwfxInput->nChannels == pwfxOutput->nChannels)
                    {
                        SLOG(eInfo1, "Verified input and output channels match when APO_FLAG_SAMPLESPERFRAME_MUST_MATCH is set.");
                    }
                    else
                    {
                        XLOG(
                            XFAIL,
                            eError,
                            "FAIL: input and output channels differ when "
                                "APO_FLAG_SAMPLESPERFRAME_MUST_MATCH is set.");
                        
                        uLog  = eError;
                    }
                }

                if (APO_FLAG_FRAMESPERSECOND_MUST_MATCH & pProperties->Flags)
                {
                    if (pwfxInput->nSamplesPerSec == pwfxOutput->nSamplesPerSec)
                    {
                        SLOG(eInfo1, "Verified input and output samples per second match when APO_FLAG_FRAMESPERSECOND_MUST_MATCH is set.");
                    }
                    else
                    {
                        XLOG(
                            XFAIL,
                            eError,
                            "FAIL: input and output samples per second differ "
                                "when APO_FLAG_FRAMESPERSECOND_MUST_MATCH "
                                "is set.");
                        
                        uLog  = eError;
                    }
                }
                
                if (APO_FLAG_BITSPERSAMPLE_MUST_MATCH & pProperties->Flags)
                {
                    if (pwfxInput->wFormatTag == pwfxOutput->wFormatTag)
                    {
                        if (pwfxInput->wBitsPerSample == pwfxOutput->wBitsPerSample)
                        {
                            SLOG(eInfo1, "Verified input and output bits per sample match when APO_FLAG_BITSPERSAMPLE_MUST_MATCH is set.");
                        }
                        else
                        {
                            XLOG(
                                XFAIL,
                                eError,
                                "FAIL: input and output bits per sample "
                                    "differ when APO_FLAG_BITSPERSAMPLE_"
                                    "MUST_MATCH is set.");
                            
                            uLog  = eError;
                        }
                    }
                }
            }
            
            //  If LFX, verify suggested format is not compressed and float32...
            if ((DT_LFX == dwDeviceType) && (NULL != pISupportedOutputFormat))
            {
                WAVEFORMATEX const     *pwfxOutput;
                BOOL                    IsFloat = FALSE;
            
                pwfxOutput  = pISupportedOutputFormat->GetAudioFormat();
                
                if (WAVE_FORMAT_IEEE_FLOAT == pwfxOutput->wFormatTag)
                {
                    IsFloat = TRUE;
                }
                else if (WAVE_FORMAT_EXTENSIBLE == pwfxOutput->wFormatTag)
                {
                    WAVEFORMATEXTENSIBLE    *pwfext = (WAVEFORMATEXTENSIBLE*)pwfxOutput;
                    
                    if (IsEqualGUID(pwfext->SubFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT))
                    {
                        IsFloat = TRUE;
                    }
                }
                
                if (NULL == pIInputFormat)
                {
                    //  If we don't have an opposite format, then we can be
                    //  very strict, it must be float32.
                
                    if (IsFloat)
                    {
                        SLOG(eInfo1, "Verified GFX suggested format is float32.");
                    }
                    else
                    {
                        XLOG(
                            XFAIL,
                            eError,
                            "FAIL: GFX suggested format is not float32.");
                        
                        uLog  = eError;
                    }
                }
                else if (0 == (APO_FLAG_BITSPERSAMPLE_MUST_MATCH & pProperties->Flags))
                {
                    //  If the SysFx doesn't require the bits per sample to
                    //  match, then they must return float32.
                
                    if (IsFloat)
                    {
                        SLOG(eInfo1, "Verified GFX suggested format is float32.");
                    }
                    else
                    {
                        XLOG(
                            XFAIL,
                            eError,
                            "FAIL: GFX suggested format is not float32.");
                            
                        uLog  = eError;
                    }
                }
                else
                {
                    SLOG(eInfo1, "Specified opposite format and bits per sample must match output format; non-float32 format allowed.");
                }
            }
            
            SLOG(uLog, "  InputFormat: %s", GetFormatString(pIInputFormat, szFormat));
            SLOG(uLog, "  Requested OutputFormat: %s", GetFormatString(pIOutputFormat, szFormat));
            SLOG(uLog, "  Suggested OutputFormat: %s", GetFormatString(pISupportedOutputFormat, szFormat));
        }
    }
    
    return (FNS_PASS);
}

DWORD WINAPI TC_IAudioProcessingObject_VerifyProcess()
{
    CAPODevice                                     *pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    CComPtr<IAudioProcessingObject>                 pIAPO = NULL;
    CComPtr<IAudioProcessingObjectRT>               pIAPORT = NULL;
    CComPtr<IAudioProcessingObjectConfiguration>    pIAPOConfig = NULL;
    APO_CONNECTION_DESCRIPTOR                      *pInputConnections = NULL;
    APO_CONNECTION_DESCRIPTOR                      *pOutputConnections = NULL;
    APO_CONNECTION_PROPERTY                        *pInputProperties = NULL;
    APO_CONNECTION_PROPERTY                        *pOutputProperties = NULL;
    PAPO_REG_PROPERTIES                             pProperties;
    UINT                                            cConnections, cInputs, cOutputs;
    CTestFormats                                    TestFormats;
    UINT                                            cFormats;
    UINT                                            ii, jj;
    char                                            szFormat[256];
    char                                            szFormat2[256];
    HRESULT                                         hr;
    
    if (NULL == pCurrentDevice)
    {
        XLOG(XFAIL, eError, "FAIL: Could not instantiate SysFx object.");
        return (FNS_PASS);
    }
    
    if (pCurrentDevice->IsProxyApo())
    {
        XLOG(
            XSKIP,
            eInfo1,
            "Skip: Testcase should not run on Proxy APO.");
        return (FNS_SKIPPED);
    }

    if (!pCurrentDevice->GetAPOInterfaces(&pIAPO, &pIAPORT, &pIAPOConfig))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObject interface.");
            
        return (FNS_PASS);
    }
    
    if (NULL == pIAPO)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObject interface.");
            
        return (FNS_PASS);
    }

    if (NULL == pIAPORT)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObjectRT interface.");
            
        return (FNS_PASS);
    }

    if (NULL == pIAPOConfig)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObjectConfiguration "
                "interface.");
                
        return (FNS_PASS);
    }
    



    if (NULL == (pProperties = pCurrentDevice->GetProperties()))
    {
        SLOG(eInfo1, "Skipping test - Could not get registration properties.");
        return (FNS_PASS);
    }
    
    cInputs      = pProperties->u32MinInputConnections;
    cOutputs     = pProperties->u32MinOutputConnections;
    cConnections = cInputs + cOutputs;
    
    //  Check for overflow...
    if ((cConnections < cInputs) || (cConnections < cOutputs))
    {
        XLOG(XFAIL, eError, "FAIL: We overflowed count of total connections?");
        return (FNS_PASS);
    }
    
    if (NULL == (pInputConnections = new APO_CONNECTION_DESCRIPTOR[cConnections]))
    {
        XLOG(XFAIL, eError, "FAIL: Insufficient memory");
        return (FNS_PASS);
    }

    pOutputConnections = &(pInputConnections[cInputs]);
    
    if (NULL == (pInputProperties = new APO_CONNECTION_PROPERTY[cConnections]))
    {
        delete []pInputConnections;
        XLOG(XFAIL, eError, "FAIL: Insufficient memory");
        return (FNS_PASS);
    }

    pOutputProperties = &(pInputProperties[cInputs]);
    
    //  Note:  Ordinarily the last format is NULL...  However, we're special
    //         casing that format to be the mix format, so be very careful about
    //         tweaking the count here.
    cFormats = TestFormats.GetCount();
    for (ii = cFormats; ii; --ii)
    {
        CComPtr<IAudioMediaType>        pIRequestedFormat = NULL;
        CComPtr<IAudioMediaType>        pISupportedInputFormat = NULL;
        CComPtr<IAudioMediaType>        pISupportedOutputFormat = NULL;
        CBufferCollection               Buffers(cConnections);
        CAudioBuffer                   *pBuffer;
        FORMAT_SUPPORT_PROC             pfnFirst = NULL;
        FORMAT_SUPPORT_PROC             pfnSecond = NULL;
        char                            szFirst[25];
        char                            szSecond[25];
        IAudioMediaType               **ppIAMTFirst = NULL;
        IAudioMediaType               **ppIAMTSecond = NULL;
        
        if (!Buffers.IsValid())
        {
            XLOG(XFAIL, eError, "FAIL: Insufficient memory");
            continue;
        }
        
        //  Special casing this to get the mix format...
        if (cFormats == ii)
        {
            hr = pCurrentDevice->GetMixFormat(&pIRequestedFormat);
        }
        else
        {
            hr = TestFormats.GetAMTFormat(ii - 1, &pIRequestedFormat);
        }
        
        if (S_OK != hr)
        {
            //  Unable to get format.
            XLOG(XFAIL, eError, "FAIL: Unable to get format.");
            continue;
        }
        
        //  Determine from which side to negotiate formats...
        if (pCurrentDevice->NegotiateFormatsFromInput())
        {
            pfnFirst     = &IAudioProcessingObject::IsInputFormatSupported;
            pfnSecond    = &IAudioProcessingObject::IsOutputFormatSupported;
            ppIAMTFirst  = &pISupportedInputFormat;
            ppIAMTSecond = &pISupportedOutputFormat;
            lstrcpy(szFirst,  "IsInputFormatSupported");
            lstrcpy(szSecond, "IsOutputFormatSupported");
        }
        else
        {
            pfnFirst     = &IAudioProcessingObject::IsOutputFormatSupported;
            pfnSecond    = &IAudioProcessingObject::IsInputFormatSupported;
            ppIAMTFirst  = &pISupportedOutputFormat;
            ppIAMTSecond = &pISupportedInputFormat;
            lstrcpy(szFirst,  "IsOutputFormatSupported");
            lstrcpy(szSecond, "IsInputFormatSupported");
        }
    
        //  Start format negotiation without opposed format.
        hr = (pIAPO->*pfnFirst)(NULL, pIRequestedFormat, ppIAMTFirst);
        
        //  Special casing the 'not supported' error...
        if (APOERR_FORMAT_NOT_SUPPORTED == hr)
        {
            if (cFormats == ii)
            {
                //  This is the mix format...  You MUST support this format!
                XLOG(
                    XFAIL,
                    eError,
                    "FAIL: With requested format of MixFormat [%s], %s "
                        "returned error (0x%08lx).",
                    GetFormatString(pIRequestedFormat, szFormat),
                    szFirst,
                    hr);
        
                continue;
            }
            else
            {
                //  APO does not support the format and it's not the mix format.
                //  This may be perfectly legit.
        
                SLOG(
                    eInfo1,
                    "With requested format of [%s], %s returned error "
                        "(0x%08lx).",
                    GetFormatString(pIRequestedFormat, szFormat),
                    szFirst,
                    hr);
                
                continue;
            }
        }
    
        //  Failing any other general error...
        if ((S_OK != hr) && (S_FALSE != hr))
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: With requested format of [%s], %s returned error "
                    "(0x%08lx).",
                GetFormatString(pIRequestedFormat, szFormat),
                szFirst,
                hr);
    
            continue;
        }
        
        //  At this point the only conditions should be S_FALSE and S_OK,
        //      and either one of those should suggest a format.
        if (NULL == *ppIAMTFirst)
        {
            if (S_OK == hr)
            {
                XLOG(
                    XFAIL,
                    eError,
                    "\r\nFAIL: With a return code of S_OK, %s returned a NULL "
                        "pSupportedFormat (pOpposed = NULL, pRequested = %s).",
                    szFirst,
                    GetFormatString(pIRequestedFormat, szFormat));
            }
            else
            {
                //  At this point it can ONLY be S_FALSE...
                XLOG(
                    XFAIL,
                    eError,
                    "\r\nFAIL: with a return code of S_FALSE, %s returned a "
                        "NULL pSupportedFormat (pOpposed = NULL, "
                        "pRequested = %s).",
                    szFirst,
                    GetFormatString(pIRequestedFormat, szFormat));
            }
            continue;
        }
        
        hr = (pIAPO->*pfnSecond)(*ppIAMTFirst, pIRequestedFormat, ppIAMTSecond);
        
        if ((S_OK != hr) && (S_FALSE != hr))
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: With requested format of [%s] and opposed format "
                    "of [%s] %s returned error (0x%08lx).",
                 GetFormatString(pIRequestedFormat, szFormat),
                 GetFormatString(*ppIAMTFirst, szFormat2),
                 szSecond,
                 hr);
                 
            continue;
        }
        
        if (NULL == *ppIAMTSecond)
        {
            if (S_OK == hr)
            {
                XLOG(
                    XFAIL,
                    eError,
                    "\r\nFAIL: With a return code of S_OK, %s returned a NULL "
                        "pSupportedFormat (pOpposed = %s, pRequested = %s).",
                    szSecond,
                    GetFormatString(*ppIAMTFirst, szFormat),
                    GetFormatString(pIRequestedFormat, szFormat2));
            }
            else
            {
                //  At this point it can ONLY be S_FALSE...
                XLOG(
                    XFAIL,
                    eError,
                    "\r\nFAIL: with a return code of S_FALSE, %s returned a "
                        "NULL pSupportedFormat (pOpposed = %s, "
                        "pRequested = %s).",
                    szSecond,
                    GetFormatString(*ppIAMTFirst, szFormat),
                    GetFormatString(pIRequestedFormat, szFormat2));
            }
            continue;
        }
        
        if (S_FALSE == hr)
        {
            SLOG(
                eInfo1,
                "Specifying an opposed format of: %s",
                GetFormatString(*ppIAMTFirst, szFormat));
                
            SLOG(
                eInfo1,
                "  and a requested format of: %s",
                GetFormatString(pIRequestedFormat, szFormat));
                
            SLOG(
                eInfo1,
                "  %s supports a format of: %s.",
                szSecond,
                GetFormatString(*ppIAMTSecond, szFormat));
        }
        
        //  Okay at this point we have two formats that we KNOW work!!
        
        for (jj = cConnections; jj; --jj)
        {
            //  Note:  Setting up both input and output buffers.
            //   Arrays are consecutive!!
            
            if (jj <= cInputs)
            {
                pBuffer = new CAudioBuffer(pISupportedInputFormat);
                Buffers.SetBuffer(jj - 1, pBuffer);
                if ((NULL == pBuffer) || (!pBuffer->GenerateBuffer(1.0)))
                {
                    break;
                }
            }
            else
            {
                pBuffer = new CAudioBuffer(pISupportedOutputFormat);
                Buffers.SetBuffer(jj - 1, pBuffer);
                if ((NULL == pBuffer) || (!pBuffer->AllocateBuffer(1.0)))
                {
                    break;
                }
            }
        }
        
        if (0 != jj)
        {
            XLOG(XFAIL, eError, "FAIL: Insufficient memory");
            continue;
        }
        
        for (jj = cConnections; jj; --jj)
        {
            UNCOMPRESSEDAUDIOFORMAT   format;
            
            //  Note:  Setting up both input and output structures.
            //   Arrays are consecutive!!
        
            //  Set up connections...
            pBuffer = Buffers.GetBuffer(jj - 1);
            pInputConnections[jj - 1].Type              = APO_CONNECTION_BUFFER_TYPE_EXTERNAL;
            pInputConnections[jj - 1].pBuffer           = (UINT_PTR)pBuffer->GetAudioBuffer();
            pInputConnections[jj - 1].pFormat           = pBuffer->GetFormat();
            
            if (S_OK != (hr = (pInputConnections[jj - 1].pFormat)->GetUncompressedAudioFormat(&format)))
            {
                XLOG(
                    XFAIL,
                    eError,
                    "FAIL: IAudioMediaType::GetUncompressedAudioFormat "
                        "returned error (0x%08lx)",
                    hr);
                    
                break;
            }
            
            pInputConnections[jj - 1].u32MaxFrameCount  = pBuffer->GetBufferSize() / (format.dwSamplesPerFrame * format.dwBytesPerSampleContainer);
            pInputConnections[jj - 1].u32Signature      = APO_CONNECTION_DESCRIPTOR_SIGNATURE;
            
            //  Set up properties...
            pInputProperties[jj - 1].pBuffer            = (UINT_PTR)pBuffer->GetAudioBuffer();
            pInputProperties[jj - 1].u32ValidFrameCount = pInputConnections[jj - 1].u32MaxFrameCount;
            pInputProperties[jj - 1].u32BufferFlags     = ((jj <= cInputs)?BUFFER_VALID:BUFFER_INVALID);
            pInputProperties[jj - 1].u32Signature       = APO_CONNECTION_PROPERTY_SIGNATURE;
        }
        
        if (0 != jj)
        {
            continue;
        }
        
        if (S_OK != (hr = pIAPOConfig->LockForProcess(cInputs, &pInputConnections, cOutputs, &pOutputConnections)))
        {
            char    szFormat2[256];
        
            XLOG(
                XFAIL,
                eError,
                "FAIL: pIAudioProcessingObjectConfiguration::LockForProcess "
                    "returned error (0x%08lx)."
                    "\r\n  Input: %d connection%s, format: %s"
                    "\r\n  Output: %d connection%s, format: %s",
                hr,
                cInputs,
                ((1 == cInputs)?"":"s"),
                GetFormatString(pISupportedInputFormat, szFormat),
                cOutputs,
                ((1 == cOutputs)?"":"s"),
                GetFormatString(pISupportedOutputFormat, szFormat2));
                
            continue;
        }
        else
        {
            SLOG(eInfo1, "pIAudioProcessingObjectConfiguration::LockForProcess succeeds.");
            SLOG(eInfo1, "  Supported InputFormat: %s", GetFormatString(pISupportedInputFormat, szFormat));
            SLOG(eInfo1, "  Supported OutputFormat: %s", GetFormatString(pISupportedOutputFormat, szFormat));
        }

        //  No return value...  Crossing fingers?!
        pIAPORT->APOProcess(cInputs, &pInputProperties, cOutputs, &pOutputProperties);
        
        for (jj = cOutputs; jj; --jj)
        {
            if (BUFFER_VALID != pOutputProperties[jj - 1].u32BufferFlags)
            {
                XLOG(
                    XFAIL,
                    eError,
                    "FAIL: OutputBuffer not marked as BUFFER_VALID after "
                        "call to APOProcess.");
            }
            else
            {
                SLOG(eInfo1, "OutputBuffer marked as BUFFER_VALID after call to APOProcess.");
            }
        }
        
        if (S_OK != (hr = pIAPOConfig->UnlockForProcess()))
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: pIAudioProcessingObjectConfiguration::UnlockForProcess "
                    "returned error (0x%08lx).",
                hr);
        }
        
        if (0 == (APO_FLAG_INPLACE & pProperties->Flags))
        {
            SLOG(eInfo1, "APO does not claim to support APO_FLAG_INPLACE.");
            continue;
        }
        
        SLOG(eInfo1, "APO supports APO_FLAG_INPLACE.  Doing in place processing...");
        
        if (1 != pProperties->u32MinInputConnections)
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: u32MinInputConnections is > 1 (%d) when specifying "
                    "APO_FLAG_INPLACE.",
                pProperties->u32MinInputConnections);
                
            continue;
        }
        else
        {
            SLOG(eInfo1, "u32MinInputConnections == 1 when specifying APO_FLAG_INPLACE.");
        }
        
        if (1 != pProperties->u32MaxInputConnections)
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: u32MaxInputConnections is > 1 (%d) when specifying "
                    "APO_FLAG_INPLACE.",
                pProperties->u32MaxInputConnections);
                
            continue;
        }
        else
        {
            SLOG(eInfo1, "u32MaxInputConnections == 1 when specifying APO_FLAG_INPLACE.");
        }
        
        if (1 != pProperties->u32MinOutputConnections)
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: u32MinOutputConnections is > 1 (%d) when specifying "
                    "APO_FLAG_INPLACE.",
                pProperties->u32MinOutputConnections);
                
            continue;
        }
        else
        {
            SLOG(eInfo1, "u32MinOutputConnections == 1 when specifying APO_FLAG_INPLACE.");
        }
        
        if (1 != pProperties->u32MaxOutputConnections)
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: u32MaxOutputConnections is > 1 (%d) when specifying "
                    "APO_FLAG_INPLACE.",
                pProperties->u32MaxOutputConnections);
                
            continue;
        }
        else
        {
            SLOG(eInfo1, "u32MaxOutputConnections == 1 when specifying APO_FLAG_INPLACE.");
        }
        
        {
            UNCOMPRESSEDAUDIOFORMAT     formatInput;
            UNCOMPRESSEDAUDIOFORMAT     formatOutput;
            UINT                        uInputBlockAlign;
            UINT                        uOutputBlockAlign;
            FLOAT                       fOutputBytesPerSec;
            FLOAT                       fInputBytesPerSec;
            
            if (S_OK != (hr = (pInputConnections[0].pFormat)->GetUncompressedAudioFormat(&formatInput)))
            {
                XLOG(
                    XFAIL,
                    eError,
                    "FAIL: IAudioMediaType::GetUncompressedAudioFormat "
                        "returned error (0x%08lx)",
                    hr);
                    
                continue;
            }
            
            if (S_OK != (hr = (pOutputConnections[0].pFormat)->GetUncompressedAudioFormat(&formatOutput)))
            {
                XLOG(
                    XFAIL,
                    eError,
                    "FAIL: IAudioMediaType::GetUncompressedAudioFormat "
                        "returned error (0x%08lx)",
                    hr);
                    
                continue;
            }
            
            uOutputBlockAlign  = formatOutput.dwSamplesPerFrame * formatOutput.dwBytesPerSampleContainer;
            fOutputBytesPerSec = uOutputBlockAlign * formatOutput.fFramesPerSecond;
            uInputBlockAlign   = formatInput.dwSamplesPerFrame * formatInput.dwBytesPerSampleContainer;
            fInputBytesPerSec  = uInputBlockAlign * formatInput.fFramesPerSecond;

            //  Can we even do in place processing?            
            if (fOutputBytesPerSec > fInputBytesPerSec)
            {
                SLOG(eInfo1, "Output rate higher than input rate.  Not performing IN PLACE processing.");
                continue;
            }
            
            //  Set up output connection...
            pBuffer = Buffers.GetBuffer(cInputs);   //  This should be the first output buffer...
            pOutputConnections[0].Type              = APO_CONNECTION_BUFFER_TYPE_EXTERNAL;
            pOutputConnections[0].pFormat           = pBuffer->GetFormat();
            pOutputConnections[0].u32Signature      = APO_CONNECTION_DESCRIPTOR_SIGNATURE;
            
            //  Set up output property...
            pOutputProperties[0].u32ValidFrameCount = 0;
            pOutputProperties[0].u32BufferFlags     = BUFFER_INVALID;
            pOutputProperties[0].u32Signature       = APO_CONNECTION_PROPERTY_SIGNATURE;

            //  Set up input connection...
            pBuffer = Buffers.GetBuffer(0);         //  This should be the first input buffer...
            pInputConnections[0].Type              = APO_CONNECTION_BUFFER_TYPE_EXTERNAL;
            pInputConnections[0].pBuffer           = (UINT_PTR)pBuffer->GetAudioBuffer();
            pInputConnections[0].pFormat           = pBuffer->GetFormat();
            pInputConnections[0].u32MaxFrameCount  = pBuffer->GetBufferSize() / (uInputBlockAlign);
            pInputConnections[0].u32Signature      = APO_CONNECTION_DESCRIPTOR_SIGNATURE;
            
            //  Set up input property...
            pInputProperties[0].pBuffer            = (UINT_PTR)pBuffer->GetAudioBuffer();
            pInputProperties[0].u32ValidFrameCount = pInputConnections[0].u32MaxFrameCount;
            pInputProperties[0].u32BufferFlags     = BUFFER_VALID;
            pInputProperties[0].u32Signature       = APO_CONNECTION_PROPERTY_SIGNATURE;

            //  Since we're using the input buffer for 'in place'...
            pOutputConnections[0].pBuffer           = (UINT_PTR)pBuffer->GetAudioBuffer();
            pOutputConnections[0].u32MaxFrameCount  = pBuffer->GetBufferSize() / (uOutputBlockAlign);
            pOutputProperties[0].pBuffer            = (UINT_PTR)pBuffer->GetAudioBuffer();
        }
        
        if (S_OK != (hr = pIAPOConfig->LockForProcess(cInputs, &pInputConnections, cOutputs, &pOutputConnections)))
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: pIAudioProcessingObjectConfiguration::LockForProcess "
                    "returned error (0x%08lx).",
                hr);
                
            continue;
        }
        else
        {
            SLOG(eInfo1, "pIAudioProcessingObjectConfiguration::LockForProcess succeeds.");
            SLOG(eInfo1, "  Supported InputFormat: %s", GetFormatString(pISupportedInputFormat, szFormat));
            SLOG(eInfo1, "  Supported OutputFormat: %s", GetFormatString(pISupportedOutputFormat, szFormat));
        }

        //  No return value...  Crossing fingers?!
        pIAPORT->APOProcess(cInputs, &pInputProperties, cOutputs, &pOutputProperties);
        
        if (BUFFER_VALID != pOutputProperties[0].u32BufferFlags)
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: OutputBuffer not marked as BUFFER_VALID after call "
                    "to APOProcess.");
        }
        else
        {
            SLOG(eInfo1, "OutputBuffer marked as BUFFER_VALID after call to APOProcess.");
        }
        
        if (S_OK != (hr = pIAPOConfig->UnlockForProcess()))
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: pIAudioProcessingObjectConfiguration::UnlockForProcess "
                    "returned error (0x%08lx).", hr);
        }
        
        //  Should we compare the 'inplace' output buffer vs. the conventional output buffer?
    }
    
    delete []pInputProperties;
    delete []pInputConnections;
    return (FNS_PASS);
}

DWORD WINAPI TC_IAudioProcessingObject_VerifyNoSRC()
{
    CAPODevice                                     *pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    PAPO_REG_PROPERTIES                             pProperties = NULL;
    UINT                                            ii;
    BOOL                                            fSupportSRC = FALSE;
    BOOL                                            fSupportSRC_RT = FALSE;

    if (NULL == pCurrentDevice)
    {
        XLOG(XFAIL, eError, "FAIL: Could not instantiate SysFx object.");
        return (FNS_PASS);
    }
    



    if (NULL == (pProperties = pCurrentDevice->GetProperties()))
    {
        SLOG(eInfo1, "Skipping test - Could not get registration properties.");
        return (FNS_PASS);
    }
    
    if (APO_FLAG_FRAMESPERSECOND_MUST_MATCH & pProperties->Flags)
    {
        SLOG(eInfo1, "  Verified pre-mix SysFx must have matching frames per second on input and output.");
    }
    else
    {
        XLOG(
            XFAIL,
            eError,
            "  FAIL: Pre-mix SysFx does not stipulate matching frames per "
                "second on input and output; performing sample rate conversion.");
    }
    
    for (ii = pProperties->u32NumAPOInterfaces; ii; --ii)
    {
        if (IsEqualGUID(__uuidof(IAudioRateConvert), pProperties->iidAPOInterfaceList[ii - 1]))
        {
            XLOG(
                XFAIL,
                eError,
                "  FAIL: Pre-mix SysFx supports the IAudioRateConvert "
                    "interface.");
                    
            fSupportSRC = TRUE;
        }
        
        if (IsEqualGUID(__uuidof(IAudioRateConvertRT), pProperties->iidAPOInterfaceList[ii - 1]))
        {
            XLOG(
                XFAIL,
                eError,
                "  FAIL: Pre-mix SysFx supports the IAudioRateConvertRT "
                    "interface.");
                    
            fSupportSRC_RT = TRUE;
        }
    }
    
    if ((!fSupportSRC) && (!fSupportSRC_RT))
    {
        SLOG(eInfo1, "  Verified pre-mix SysFx does not support sample rate conversion interfaces.");
    }
    
    return (FNS_PASS);
}

DWORD WINAPI TC_VerifyCustomFormats()
{
    CAPODevice                                     *pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    CComPtr<IAudioProcessingObject>                 pIAPO = NULL;
    CComPtr<IAudioSystemEffectsCustomFormats>       pICustomFormats = NULL;
    CComPtr<IAudioMediaType>                        pIMediaTypeTemp = NULL;
    CCTMString                                      strNameTemp = NULL;
    HRESULT                                         hr;
    UINT                                            iFormat;

    if (NULL == pCurrentDevice)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate SysFx object.");
        
        return (FNS_PASS);
    }
    
    if (!pCurrentDevice->GetAPOInterfaces(&pIAPO, NULL, NULL))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObject interface.");
            
        return (FNS_PASS);
    }
    
    if (NULL == pIAPO)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObject interface.");
            
        return (FNS_PASS);
    }
    
    hr = pIAPO->QueryInterface(__uuidof(IAudioSystemEffectsCustomFormats), (void**)&pICustomFormats);
    
    if (E_NOINTERFACE == hr)
    {
        //  This GFX does not support custom formats...
        SLOG(
            eInfo1,
            "SysFx does not support custom formats (QueryInterface for "
                "IAudioSystemEffectsCustomFormats returns E_NOINTERFACE).");
            
        return (FNS_PASS);
    }
    else if (S_OK != hr)
    {
        //  Any other non-S_OK result -> FAIL!!!
        XLOG(
            XFAIL,
            eError,
            "FAIL: QueryInterface for IAudioSystemEffectsCustomFormats "
                "returned 0x%08lx.",
            hr);
            
        return (FNS_PASS);
    }
    else
    {
        //  S_OK...
        SLOG(eInfo1, "IAudioSystemEffectsCustomFormats available on SysFx.");
    }
    
    if (DT_LFX & pCurrentDevice->GetDeviceType() ||
        DT_SFX & pCurrentDevice->GetDeviceType())
    {
        //  This check is only valid for GFX's, MFX or EFX...
        XLOG(
            XFAIL,
            eError,
            "FAIL: IAudioSystemEffectsCustomFormats available on "
                "pre-mix SysFx or SFX.");
    }
    
    //  Test for E_POINTER??
    
    if (S_OK != (hr = pICustomFormats->GetFormatCount(&iFormat)))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Call to IAudioSystemEffectsCustomFormats::GetFormatCount "
                "returned 0x%08lx.",
            hr);
            
        return (FNS_PASS);
    }

#if 0    //  Bug#1856666
    if (0 == iFormat)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: IAudioSystemEffectsCustomFormats::GetFormatCount returned "
                "zero custom formats.");
    }
#endif    
    
    //  Test for E_POINTER??
    
    //  Verify using index out of range bounces...
    if (E_INVALIDARG != (hr = pICustomFormats->GetFormat(iFormat, &pIMediaTypeTemp)))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Call to GetFormat with index out of range (%d) returns "
                "error (0x%08lx).",
            iFormat,
            hr);
    }
    else
    {
        SLOG(eInfo1, "GetFormat with index out of range returned E_INVALIDARG.");
    }
    
    if (E_INVALIDARG != (hr = pICustomFormats->GetFormatRepresentation(iFormat, &strNameTemp)))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: GetFormatRepresentation with index out of range (%d) "
                "returns 0x%08lx.",
            iFormat,
            hr);
    }
    else
    {
        SLOG(eInfo1, "GetFormatRepresentation with index out of range returned E_INVALIDARG.");
    }
    
    for (; iFormat; --iFormat)
    {
        CComPtr<IAudioMediaType>        pIFormat = NULL;
        CCTMString                      strFormatName = NULL;
        
        if (S_OK != (hr = pICustomFormats->GetFormat(iFormat - 1, &pIFormat)))
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: IAudioSystemEffectsCustomFormats::GetFormat returned "
                    "error (0x%08lx).",
                hr);
        }
        else
        {
            SLOG(eInfo1, "IAudioSystemEffectsCustomFormats::GetFormat successful.");
        }
        
        if (S_OK != (hr = pICustomFormats->GetFormatRepresentation(iFormat - 1, &strFormatName)))
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: IAudioSystemEffectsCustomFormats::"
                    "GetFormatRepresentation returned error (0x%08lx).",
                hr);
        }
        else
        {
            SLOG(eInfo1, "IAudioSystemEffectsCustomFormats::GetFormatRepresentation successful.");
            SLOG(eInfo1, "  Format: [%ls]", strFormatName);
        }
    }
    
    return (FNS_PASS);
}

DWORD WINAPI TC_VerifyCustomFormatSupport()
{
    CAPODevice                                     *pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    CComPtr<IAudioProcessingObject>                 pIAPO = NULL;
    CComPtr<IAudioSystemEffectsCustomFormats>       pICustomFormats = NULL;
    CComPtr<IEndpointUtility>                       pIEPUtility = NULL;
    IMMDevice                                      *pIMMDevice = NULL;
    HRESULT                                         hr;
    UINT                                            iFormat;

    if (NULL == pCurrentDevice)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate SysFx object.");
            
        return (FNS_PASS);
    }
    
    if (NULL == (pIMMDevice = pCurrentDevice->GetEndpoint()))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Unable to get endpoint for device.");
            
        return (FNS_PASS);
    }
    
    if (S_OK != (hr = pIMMDevice->Activate(__uuidof(IEndpointUtility), CLSCTX_INPROC_SERVER, NULL, (void**)&pIEPUtility)))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Unable activate utility interface.");
            
        return (FNS_PASS);
    }
    
    if (!pCurrentDevice->GetAPOInterfaces(&pIAPO, NULL, NULL))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObject interface.");
            
        return (FNS_PASS);
    }
    
    if (NULL == pIAPO)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Could not instantiate IAudioProcessingObject interface.");
            
        return (FNS_PASS);
    }
    
    hr = pIAPO->QueryInterface(__uuidof(IAudioSystemEffectsCustomFormats), (void**)&pICustomFormats);
    
    if (E_NOINTERFACE == hr)
    {
        //  This GFX does not support custom formats...
        SLOG(
            eInfo1,
            "SysFx does not support custom formats (QueryInterface for "
                "IAudioSystemEffectsCustomFormats returns E_NOINTERFACE).");
            
        return (FNS_PASS);
    }
    else if (S_OK != hr)
    {
        //  Any other non-S_OK result -> FAIL!!!
        XLOG(
            XFAIL,
            eError,
            "FAIL: QueryInterface for IAudioSystemEffectsCustomFormats "
                "returned 0x%08lx.",
            hr);
        
        return (FNS_PASS);
    }
    else
    {
        //  S_OK...
        SLOG(eInfo1, "IAudioSystemEffectsCustomFormats available on SysFx.");
    }

    if (S_OK != (hr = pICustomFormats->GetFormatCount(&iFormat)))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Call to IAudioSystemEffectsCustomFormats::GetFormatCount "
                "returned 0x%08lx.",
            hr);
            
        return (FNS_PASS);
    }
    
    for (; iFormat; --iFormat)
    {
        CComPtr<IAudioMediaType>        pIFormat = NULL;
        CComPtr<IAudioMediaType>        pIFormatSuggested = NULL;
        CCTMString                      strFormatName = NULL;
        CComPtr<IPartsList>             pIParts = NULL;
        KSDATAFORMAT_WAVEFORMATEX      *pKsFormat = NULL;
        
        if (S_OK != (hr = pICustomFormats->GetFormat(iFormat - 1, &pIFormat)))
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: IAudioSystemEffectsCustomFormats::GetFormat returned "
                    "error (0x%08lx).",
                hr);
                
            continue;
        }
        
        if (S_OK != (hr = pICustomFormats->GetFormatRepresentation(iFormat - 1, &strFormatName)))
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: IAudioSystemEffectsCustomFormats::"
                    "GetFormatRepresentation returned error (0x%08lx).",
                hr);
                
            continue;
        }
        
        SLOG(eInfo1, "Verifying format support for: [%ls]...", strFormatName);
        
        //  Custom format, must be supported on the output side of SysFx...
        hr = pIAPO->IsOutputFormatSupported(NULL, pIFormat, &pIFormatSuggested);
        
        if (S_OK != hr)
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: IsOutputFormatSupported returns error (0x%08lx).",
                hr);
        }
        else
        {
            if (NULL == pIFormatSuggested)
            {
                XLOG(
                    XFAIL,
                    eError,
                    "FAIL: IsOutputFormatSupported returns S_OK, but does not "
                        "suggest format.");
            }
            else
            {
                if (!IsEqualFormat(pIFormat, pIFormatSuggested))
                {
                    XLOG(
                        XFAIL,
                        eError,
                        "FAIL: IsOutputFormatSupported returns S_OK, but "
                            "returns a different format.");
                }
                else
                {
                    SLOG(eInfo1, "IsOutputFormatSupported supports custom format.");
                }
            }
        }
        
        pKsFormat = CreateKSDataFromWFX((WAVEFORMATEX*)pIFormat->GetAudioFormat());
        
        if (NULL == pKsFormat)
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: Unable to create KSDATAFORMAT from PWAVEFORMATEX.");
                
            continue;
        }
        
        {
            CCTMAutoFree    KSFormatBuffer((LPVOID)pKsFormat);
            
            //  Custom format, must be supported by endpoint...
            hr = pIEPUtility->FindHostConnector(
                    (KSDATAFORMAT*)pKsFormat,
                    pKsFormat->DataFormat.FormatSize,
                    FALSE,
                    &pIParts);
            
            if (S_OK != hr)
            {
                XLOG(
                   XFAIL,
                   eError,
                   "FAIL: Endpoint does not support custom format.");
            }
            else
            {
                SLOG(eInfo1, "Endpoint supports custom format.");
            }
        }
    }
    
    return (FNS_PASS);
}
