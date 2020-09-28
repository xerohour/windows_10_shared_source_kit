// Copyright (C) Microsoft. All rights reserved.
#include "common.h"

float   gaMicArrayFrequency[] =
{
//    44100,
    48000
};

HRESULT GetImmediateConnector
(
    IMMDevice      *pIMMDevice,
    IConnector    **ppITargetConnector
)
{
    CComPtr<IConnector>         pIConnector = NULL;
    CComPtr<IDeviceTopology>    pIDevTopo = NULL;
    HRESULT                     hr;

    if (S_OK != (hr = pIMMDevice->Activate(__uuidof(IDeviceTopology), CLSCTX_INPROC_SERVER, NULL, (void**)&pIDevTopo)))
    {
        SLOG(eError, "  FAIL: IMMDevice::Activate for IDeviceTopology returned error (0x%08lx).", hr);
        return hr;
    }

    //  This is an Endpoint, it should only have one connector...
    if (S_OK != (hr = pIDevTopo->GetConnector(0, &pIConnector)))
    {
        SLOG(eError, "  FAIL: IDeviceTopology::GetConnector(0) returned error (0x%08lx).", hr);
        return hr;
    }
    
    if (S_OK != (hr = pIConnector->GetConnectedTo(ppITargetConnector)))
    {
//        SLOG(eError, "  FAIL: IConnector::GetConnectedTo returned error (0x%08lx).", hr);
        return hr;
    }
    
    return (S_OK);
}

UINT GetNumMicArrayChannels
(
    IMMDevice      *pIMMDevice
)
{
    CComPtr<IConnector>             pIConnector = NULL;
    CComPtr<IDeviceTopology>        pIDevTopo = NULL;
    CComPtr<IPart>                  pIPart = NULL;
    CComPtr<IMMDeviceEnumerator>    pIDeviceEnumerator = NULL;
    CComPtr<IMMDevice>              pIKsDevice = NULL;
    CComPtr<IKsControl>             pIKsControl = NULL;
    CCTMString                      strDeviceId = NULL;
    CAutoBuffer                     Buffer;
    KSAUDIO_MIC_ARRAY_GEOMETRY     *pMicArrayGeometry;
    UINT                            nPartId;
    KSP_PIN                         ksp;
    ULONG                           cbData;
    ULONG                           cbGeometry;
    GUID                            guidSubType;
    UINT                            cMicrophones;
    HRESULT                         hr;
    
    if (S_OK != (hr = GetImmediateConnector(pIMMDevice, &pIConnector)))
    {
        return (0);
    }
    
    if (S_OK != (hr = pIConnector->QueryInterface(__uuidof(IPart), (void**)&pIPart)))
    {
        SLOG(eError, "  FAIL: IConnector::QueryInterface for IPart returned error (0x%08lx).", hr);
        return (0);
    }
    
    if (S_OK != (hr = pIPart->GetSubType(&guidSubType)))
    {
        SLOG(eError, "  FAIL: IPart::GetSubType() returned error (0x%08lx).", hr);
        return (0);
    }
    
    if (!IsEqualGUID(guidSubType, KSNODETYPE_MICROPHONE_ARRAY))
    {
        return (0);
    }
    
    if (S_OK != (hr = pIPart->GetLocalId(&nPartId)))
    {
        SLOG(eError, "  FAIL: IPart::GetLocalId() returned error (0x%08lx).", hr);
        return (0);
    }
    
    if (S_OK != (hr = pIPart->GetTopologyObject(&pIDevTopo)))
    {
        SLOG(eError, "  FAIL: IPart::GetTopologyObject() returned error (0x%08lx).", hr);
        return (0);
    }
    
    if (S_OK != (hr = pIDevTopo->GetDeviceId(&strDeviceId)))
    {
        SLOG(eError, "  FAIL: IDeviceTopology::GetDeviceId() returned error (0x%08lx).", hr);
        return (0);
    }
    
    if (S_OK != (hr = pIDeviceEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator))))
    {
        SLOG(eError, "  FAIL: CoCreateInstance returned error (0x%08lx).", hr);
        return (0);
    }
 
    if (S_OK != (hr = pIDeviceEnumerator->GetDevice(strDeviceId, &pIKsDevice)))
    {
        SLOG(eError, "  FAIL: IMMDeviceEnumerator::GetDevice() returned error (0x%08lx).", hr);
        return (0);
    }
    
    if (S_OK != (hr = pIKsDevice->Activate(__uuidof(IKsControl), CLSCTX_INPROC_SERVER, NULL, (void**)&pIKsControl)))
    {
        SLOG(eError, "  FAIL: IMMDevice::Activate returned error (0x%08lx).", hr);
        return (0);
    }
    
    ZeroMemory(&ksp, sizeof(ksp));
    cbData             = 0;
    cbGeometry         = 0;
    ksp.Property.Set   = KSPROPSETID_Audio;
    ksp.Property.Id    = KSPROPERTY_AUDIO_MIC_ARRAY_GEOMETRY;
    ksp.Property.Flags = KSPROPERTY_TYPE_GET;
    ksp.PinId          = nPartId & PARTID_MASK;
    
    hr = pIKsControl->KsProperty((PKSPROPERTY)&ksp, sizeof(ksp), NULL, 0, &cbGeometry);
    
    if (S_OK != hr)
    {
        SLOG(eError, "  FAIL: IKsControl::KsProperty returned error (0x%08lx).", hr);
        return (0);
    }
    
    pMicArrayGeometry = (KSAUDIO_MIC_ARRAY_GEOMETRY*)Buffer.Allocate(cbGeometry);
    
    if (NULL == pMicArrayGeometry)
    {
        SLOG(eError, "  FAIL: Out of memory.");
        return (0);
    }
    
    hr = pIKsControl->KsProperty((PKSPROPERTY)&ksp, sizeof(ksp), pMicArrayGeometry, cbGeometry, &cbData);
    
    if (S_OK != hr)
    {
        SLOG(eError, "  FAIL: IKsControl::KsProperty returned error (0x%08lx).", hr);
        return (0);
    }

    cMicrophones = pMicArrayGeometry->usNumberOfMicrophones;
    
    return (cMicrophones);
}

BOOL AreAudioChannelsDifferent
(
    UINT        iSource,
    UINT        iDestination,
    UINT        cChannels,
    LPBYTE      pBuffer,
    UINT        cbSample,
    UINT        cSamples
)
{
    switch (cbSample)
    {
        case 8:     //  8-bit integers
        {
            BYTE       *pSrc = (BYTE*)pBuffer;
            BYTE       *pDst = (BYTE*)pBuffer;
    
            pSrc += iSource;
            pDst += iDestination;
    
            for (; cSamples; --cSamples)
            {
                if (*pSrc != *pDst)
                {
                    return (TRUE);
                }
        
                pSrc += cChannels;
                pDst += cChannels;
            }
        
            return (FALSE);
        }
        
        case 16:    //  16-bit integers
        {
            WORD       *pSrc = (WORD*)pBuffer;
            WORD       *pDst = (WORD*)pBuffer;
    
            pSrc += iSource;
            pDst += iDestination;
    
            for (; cSamples; --cSamples)
            {
                if (*pSrc != *pDst)
                {
                    return (TRUE);
                }
        
                pSrc += cChannels;
                pDst += cChannels;
            }
        
            return (FALSE);
        }
        
        case 32:    //  32-bit floats
        {
            float      *pSrc = (float*)pBuffer;
            float      *pDst = (float*)pBuffer;
        
            pSrc += iSource;
            pDst += iDestination;
        
            for (; cSamples; --cSamples)
            {
                if (*pSrc != *pDst)
                {
                    return (TRUE);
                }
            
                pSrc += cChannels;
                pDst += cChannels;
            }
            
            return (FALSE);
        }
        
        default:
            //  Failure condition...
            return (TRUE);
    }
}

DWORD WINAPI TC_VerifyMicArrayRaw()
{
    CAPODevice                                     *pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    CComPtr<IMMDevice>                              pIEndpoint = NULL;
    CComPtr<IAudioProcessingObject>                 pIAPO = NULL;
    CComPtr<IAudioProcessingObjectRT>               pIAPORT = NULL;
    CComPtr<IAudioProcessingObjectConfiguration>    pIAPOConfig = NULL;
    APO_CONNECTION_DESCRIPTOR                       InputConnection, *pInputConnection = &InputConnection;
    APO_CONNECTION_DESCRIPTOR                       OutputConnection, *pOutputConnection = &OutputConnection;
    APO_CONNECTION_PROPERTY                         InputProperty, *pInputProperty = &InputProperty;
    APO_CONNECTION_PROPERTY                         OutputProperty, *pOutputProperty = &OutputProperty;
    UINT                                            cMicrophones = 0;
    UINT                                            cSamples;
    UNCOMPRESSEDAUDIOFORMAT                         Format;
    HRESULT                                         hr;
    UINT                                            iFreq;
    UINT                                            ii, jj;
    BOOL                                            fChannelsCheck;
    char                                            szFormat[256];
    
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
    
    pIEndpoint  = pCurrentDevice->GetEndpoint();
            
    if (0 == (cMicrophones = GetNumMicArrayChannels(pIEndpoint)))
    {
        SLOG(eInfo1, "Device does not contain microphone array.");
        return (FNS_PASS);
    }
    
    if (1 == cMicrophones)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Device claims to be a one element microphone array.");
            
        return (FNS_PASS);
    }
    
    SLOG(eInfo1, "Device contains microphone array.  Verifying non-beam mode.");
    
    iFreq = sizeof(gaMicArrayFrequency)/sizeof(gaMicArrayFrequency[0]);
    for (; iFreq; --iFreq)
    {
        CComPtr<IAudioMediaType>        pIInputFormat = NULL;
        CComPtr<IAudioMediaType>        pIOutputFormat = NULL;
        CAudioBuffer                    InputBuffer;
        CAudioBuffer                    OutputBuffer;
    
        ZeroMemory(&Format, sizeof(Format));
        
        Format.guidFormatType            = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
        Format.dwSamplesPerFrame         = cMicrophones;
        Format.dwBytesPerSampleContainer = 4;    // float32...
        Format.dwValidBitsPerSample      = 32;   // float32...
        Format.fFramesPerSecond          = gaMicArrayFrequency[iFreq - 1];
        Format.dwChannelMask             = 0;
        
        //  Input and output formats should be the same in Raw Mode...
        hr = CreateAudioMediaTypeFromUncompressedAudioFormat(
                &Format,
                &pIInputFormat);
        
        if (S_OK != hr)
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: Unable to instantiate IAudioMediaType format.");
                
            break;
        }
        
        hr = CreateAudioMediaTypeFromUncompressedAudioFormat(
                &Format,
                &pIOutputFormat);
        
        if (S_OK != hr)
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: Unable to instantiate IAudioMediaType format.");
                
            break;
        }

        //  Verifying IsXFormatSupported() work properly...        
        {
            CComPtr<IAudioMediaType>      pISupportedOutputFormat = NULL;
            CComPtr<IAudioMediaType>      pISupportedInputFormat = NULL;
            
            //  Make sure that we approve on the input query...
            hr = pIAPO->IsInputFormatSupported(pIOutputFormat, pIInputFormat, &pISupportedInputFormat);
        
            if (S_OK != hr)
            {
                XLOG(
                    XFAIL,
                    eError,
                    "FAIL: Microphone Array SysFx does not natively support "
                        "[%s] for input and output with "
                        "IsInputFormatSupported().",
                    GetFormatString(pIInputFormat, szFormat));
                    
                break;
            }
            else
            {
                SLOG(eInfo1, "Microphone Array SysFx supports [%s] for input and output with IsInputFormatSupported().", GetFormatString(pIInputFormat, szFormat));
            }
            
            //  Make sure that we approve on the output query...
            hr = pIAPO->IsOutputFormatSupported(pIInputFormat, pIOutputFormat, &pISupportedOutputFormat);
            
            if (S_OK != hr)
            {
                XLOG(
                    XFAIL,
                    eError,
                    "FAIL: Microphone Array SysFx does not natively support "
                        "[%s] for input and output with "
                        "IsOutputFormatSupported().",
                    GetFormatString(pIInputFormat, szFormat));
                        
                break;
            }
            else
            {
                SLOG(eInfo1, "Microphone Array SysFx supports [%s] for input and output with IsOutputFormatSupported().", GetFormatString(pIInputFormat, szFormat));
            }
        }
        
        InputBuffer.SetFormat(pIInputFormat);
        OutputBuffer.SetFormat(pIOutputFormat);
        
        if (!InputBuffer.GenerateOffsetBuffer(1.0))
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: Unable to generate test buffer.");
                
            break;
        }
        
        if (!OutputBuffer.AllocateBuffer(1.0))
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: Unable to allocate test buffer.");
                
            break;
        }
        
        ZeroMemory(&InputConnection, sizeof(InputConnection));
        InputConnection.Type              = APO_CONNECTION_BUFFER_TYPE_EXTERNAL;
        InputConnection.pBuffer           = (UINT_PTR)InputBuffer.GetAudioBuffer();
        InputConnection.pFormat           = InputBuffer.GetFormat();
        InputConnection.u32MaxFrameCount  = InputBuffer.GetBufferSize() / (Format.dwSamplesPerFrame * Format.dwBytesPerSampleContainer);
        InputConnection.u32Signature      = APO_CONNECTION_DESCRIPTOR_SIGNATURE;
        
        ZeroMemory(&InputProperty, sizeof(InputProperty));
        InputProperty.pBuffer             = (UINT_PTR)InputBuffer.GetAudioBuffer();
        InputProperty.u32ValidFrameCount  = InputConnection.u32MaxFrameCount;
        InputProperty.u32BufferFlags      = BUFFER_VALID;
        InputProperty.u32Signature        = APO_CONNECTION_PROPERTY_SIGNATURE;
        
        ZeroMemory(&OutputConnection, sizeof(OutputConnection));
        OutputConnection.Type             = APO_CONNECTION_BUFFER_TYPE_EXTERNAL;
        OutputConnection.pBuffer          = (UINT_PTR)OutputBuffer.GetAudioBuffer();
        OutputConnection.pFormat          = OutputBuffer.GetFormat();
        OutputConnection.u32MaxFrameCount = OutputBuffer.GetBufferSize() / (Format.dwSamplesPerFrame * Format.dwBytesPerSampleContainer);
        OutputConnection.u32Signature     = APO_CONNECTION_DESCRIPTOR_SIGNATURE;
        
        ZeroMemory(&OutputProperty, sizeof(OutputProperty));
        OutputProperty.pBuffer            = (UINT_PTR)OutputBuffer.GetAudioBuffer();
        OutputProperty.u32ValidFrameCount = OutputConnection.u32MaxFrameCount;
        OutputProperty.u32BufferFlags     = BUFFER_INVALID;
        OutputProperty.u32Signature       = APO_CONNECTION_PROPERTY_SIGNATURE;
        
        if (S_OK != (hr = pIAPOConfig->LockForProcess(1, &pInputConnection, 1, &pOutputConnection)))
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: pIAudioProcessingObjectConfiguration::LockForProcess "
                    "returned error (0x%08lx).",
                hr);
                
            break;
        }
        else
        {
            SLOG(eInfo1, "pIAudioProcessingObjectConfiguration::LockForProcess succeeds.");
        }
        
        //  No return value...  Crossing fingers?!
        pIAPORT->APOProcess(1, &pInputProperty, 1, &pOutputProperty);
        
        if (BUFFER_VALID != OutputProperty.u32BufferFlags)
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: OutputBuffer not marked as BUFFER_VALID after call to "
                    "APOProcess.");
                    
            break;
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
                    "returned error (0x%08lx).",
                hr);
                
            break;
        }
        
        SLOG(eInfo1, "Verifying channels on output buffer are different.");
        fChannelsCheck = TRUE;
        cSamples       = InputBuffer.GetBufferSize() / (Format.dwSamplesPerFrame * Format.dwBytesPerSampleContainer);
        for (ii = cMicrophones; ii > 1; --ii)
        {
            for (jj = ii - 1; jj; --jj)
            {
                BOOL    fDifferent;
            
                fDifferent = AreAudioChannelsDifferent(
                                ii - 1,
                                jj - 1,
                                cMicrophones,
                                (LPBYTE)OutputConnection.pBuffer,
                                Format.dwBytesPerSampleContainer,
                                cSamples);
                        
                if (!fDifferent)
                {
                    XLOG(
                        XFAIL,
                        eError,
                        "FAIL: Output buffer channels %d and %d are identical.",
                        ii - 1,
                        jj - 1);
                        
                    fChannelsCheck = FALSE;
                    break;
                }
            }
            
            if (!fChannelsCheck)
            {
                break;
            }
        }
        
        if (fChannelsCheck)
        {
            SLOG(eInfo1, "Channels on output buffer are different.");
        }
    }
    
    return (FNS_PASS);
}
