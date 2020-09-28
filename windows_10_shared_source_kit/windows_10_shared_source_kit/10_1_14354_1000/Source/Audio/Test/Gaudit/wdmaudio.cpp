//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
// Copyright(C) Microsoft.All rights reserved.
//
//  File:       wdmaudio.cpp
//
//
//--------------------------------------------------------------------------


#include "common.h"
#include "wdmaudio.h"

//@-----------------------------------------------------------------------------------------
//                                    IOCTL_WDMAUD_INIT
//@-----------------------------------------------------------------------------------------


// Function name   : Generic_Ioctl_IOCTL_WDMAUD_INIT
// Description     : Testcase to test IOCTL_WDMAUD_INIT with valid and invalid buffers
// Return type     : DWORD WINAPI 
// Argument        : PGAUDITPARAMS pParams
// History         : 2002     Created

DWORD WINAPI Generic_Ioctl_IOCTL_WDMAUD_INIT(PGAUDITPARAMS pParams)
{
    LPDEVICEINFO pDevInfo = NULL;
    DWORD cbDeviceInfo = 0;
    DWORD dwErr = 0;
    DWORD  dwResult = FNS_PASS;        

    // Check if this is wdmaudio.sys filter
    if(!(g_pGauditModule->GetDevice()->m_dwFlags & WDMAUD_DEVICE))
        return FNS_PASS;    

    // !!BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG
    // On win2k sizeof(DEVICEINFO) is 4 less,    
    // Do run time check on structure size and choose appropriately
    pDevInfo = (LPDEVICEINFO) LocalAlloc(LPTR, sizeof(DEVICEINFO));
    if(!pDevInfo)
    { 
        SLOG(LOG_LEVEL_ERROR, "Allocation Failed");
        return FNS_FAIL; /*spew some error*/ 
    }
    
    pDevInfo->OpenDone = 0;
    pDevInfo->DeviceType = AuxDevice; // defined in wdmaud.h
    pDevInfo->DataBuffer = NULL;
    pDevInfo->DataBufferSize = 0;        

    cbDeviceInfo = sizeof(DEVICEINFO);
    SLOG(eInfo2, "Sending Valid IOCTL");

    {        
        CGenericIoControl gnt( pParams->hFile, pDevInfo, 
                               cbDeviceInfo, cbDeviceInfo, 
                               sizeof(DEVICEINFO), sizeof(DEVICEINFO),                                
                               ALLOCATOR_STATIC, IOCTL_WDMAUD_INIT);
        dwErr = gnt.Test(FALSE, 0);    
        if (FAILED(dwErr))
        {
            // Should have passed
            SLOG(LOG_LEVEL_ERROR, "^^ERROR: Unable to init WDMAUD, Error returned is %d", dwErr);
        }
        if (gnt.GetFault() != FAULT_NO_FAULT)
            dwResult = FNS_FAIL;
    }

    SLOG(eInfo2, "Sending InValid IOCTL with Devcice Interface String NULL termination removed ");
    // Remove the NULL termination, with valid o/p buffer
    {
        pDevInfo->wstrDeviceInterface[0] = 'A';
        CGenericIoControl gnt( pParams->hFile, pDevInfo, 
                               cbDeviceInfo, cbDeviceInfo, 
                               sizeof(DEVICEINFO), sizeof(DEVICEINFO),                               
                               ALLOCATOR_STATIC, IOCTL_WDMAUD_INIT);
        gnt.Test(TRUE, 0);
        if (gnt.GetFault() != FAULT_NO_FAULT)
            dwResult = FNS_FAIL;
    }

    // The following has been commented out, because lying about i/p size only causes random failures
    // IO Manager is anyway going to buffer the "input buffer" since the IOCTL specifies METHOD_BUFFERED
    //SLOG(eInfo3, "Sending InValid IOCTL, lying about input buffer size");
    //// valid o/p buffer, lie about i/p buffer size
    //{
    //    
    //    CGenericIoControl gnt( pParams->hFile, pDevInfo, 
    //                           cbDeviceInfo, 2*cbDeviceInfo, 
    //                           sizeof(DEVICEINFO), sizeof(DEVICEINFO),                               
    //                           ALLOCATOR_STATIC, IOCTL_WDMAUD_INIT);
    //    gnt.Test(TRUE, 0);
    //    if (gnt.GetFault() != FAULT_NO_FAULT)
    //        dwResult = FNS_FAIL;
    //}

    SafeLocalFree(pDevInfo);     

    return dwResult;
}


//@-----------------------------------------------------------------------------------------
//                                    IOCTL_WDMAUD_ADD_DEVNODE
//@-----------------------------------------------------------------------------------------

// Function name   : Generic_Ioctl_IOCTL_WDMAUD_ADD_DEVNODE
// Description     : Test IOCTL_WDMAUD_ADD_DEVNODE and IOCTL_WDMAUD_REMOVE_DEVNODE 
//                   for WAVE, MIDI and MIXER devices, although this could be split into two test cases
//                   It is the exact same thing, just the IOCTL code changes, besides in order to ensure
//                   proper cleanup, every ADD should have a complementary REMOVE, and hence the integration
//                   of both cases here.
// Return type     : DWORD WINAPI 
// Argument        : PGAUDITPARAMS pParams
// History         : 2002     Created

DWORD WINAPI Generic_Ioctl_IOCTL_WDMAUD_ADD_DEVNODE(PGAUDITPARAMS pParams)
{
    // The name of this testcase is slightly wrong beacuse it tests ADD as well as REMOVE
    LPDEVICEINFO pDevInfo = NULL;
    DWORD cbDeviceInfo = 0;
    DWORD dwErr = 0;    
    DWORD dwResult = FNS_PASS;

    // Check if this is wdmaudio.sys filter
    if(!(g_pGauditModule->GetDevice()->m_dwFlags & WDMAUD_DEVICE))
        return FNS_PASS;

    // KslEnumerate KSCATEGORY_AUDIO devices
    TTList<CKsFilter> listFilters;
    TTNode<CKsFilter>* pNode = NULL;
    CKsFilter* pFilter = NULL;
    LPGUID guidAudio[] = { const_cast<LPGUID>(&KSCATEGORY_AUDIO) };

    if (!KslEnumFilters(&listFilters, ePCMAudio, guidAudio, 1, FALSE, FALSE, TRUE))
        return FNS_FAIL;

    // For each WAVE, MIDI or MIXER device, run tests
    FOR_EACH_FILTER(listFilters, pNode)
    {
        pFilter = pNode->pData;
        eDeviceType eDevType = IsValidAudioDevice(pFilter);        
        if ( pFilter && (BOOL)eDevType && (eDevType != DEVICE_USB))   // this filters out USB devices for now
        {
            SLOG(eInfo2, "Adding Device %s", pFilter->m_szFilterName);
            cbDeviceInfo = sizeof(DEVICEINFO)+(sizeof(WCHAR)*lstrlen(pFilter->m_szFilterName));
            pDevInfo = (LPDEVICEINFO) LocalAlloc(LPTR, cbDeviceInfo);
            if(!pDevInfo)
            {
                SLOG(LOG_LEVEL_ERROR, "Allocation Failed");
                return FNS_FAIL;
            }
            MultiByteToWideChar( GetACP(),
                                 MB_PRECOMPOSED,
                                 pFilter->m_szFilterName,
                                 -1, //since NULL terminated
                                 pDevInfo->wstrDeviceInterface,
                                 lstrlen(pFilter->m_szFilterName) + 1);    //+1 for NULL termination            
            // Test Out devices
            pDevInfo->DeviceType = (eDevType == DEVICE_WAVE) ? WaveOutDevice : 
                                 ( (eDevType == DEVICE_MIDI) ? MidiOutDevice :  MixerDevice);
            
            pDevInfo->DataBuffer = NULL;
            pDevInfo->DataBufferSize = 0;
            {
                CGenericIoControl gnt( pParams->hFile, pDevInfo, 
                                       cbDeviceInfo, cbDeviceInfo, 
                                       sizeof(DEVICEINFO), sizeof(DEVICEINFO),                                
                                       ALLOCATOR_STATIC, IOCTL_WDMAUD_ADD_DEVNODE);
                gnt.Test(FALSE,0);
                if ( gnt.GetFault() != FAULT_NO_FAULT )                                    
                    dwResult = FNS_FAIL;                                
            }
            // Now pDevInfo hasn't changed so REMOVE the device just added
            SLOG(eInfo2, "Removing Device %s", pFilter->m_szFilterName);
            pDevInfo->DataBuffer = NULL;
            pDevInfo->DataBufferSize = 0;
            {
                CGenericIoControl gnt( pParams->hFile, pDevInfo, 
                                       cbDeviceInfo, cbDeviceInfo, 
                                       sizeof(DEVICEINFO), sizeof(DEVICEINFO),                                
                                       ALLOCATOR_STATIC, IOCTL_WDMAUD_REMOVE_DEVNODE);
                gnt.Test(FALSE,0);
                if ( gnt.GetFault() != FAULT_NO_FAULT )
                    dwResult = FNS_FAIL;                
            }

            // Test In devices
            pDevInfo->DeviceType = (eDevType == DEVICE_WAVE) ? WaveInDevice : 
                                 ( (eDevType == DEVICE_MIDI) ? MidiInDevice :  MixerDevice);
            if (pDevInfo->DeviceType != MixerDevice) //Mixer Device has already been tested above
            {
                SLOG(eInfo2, "Adding Device %s", pFilter->m_szFilterName);
                pDevInfo->DataBuffer = NULL;
                pDevInfo->DataBufferSize = 0;
                {
                    CGenericIoControl gnt( pParams->hFile, pDevInfo, 
                        cbDeviceInfo, cbDeviceInfo, 
                        sizeof(DEVICEINFO), sizeof(DEVICEINFO),                                
                        ALLOCATOR_STATIC, IOCTL_WDMAUD_ADD_DEVNODE);
                    gnt.Test(FALSE,0);
                    if ( gnt.GetFault() != FAULT_NO_FAULT )
                        dwResult = FNS_FAIL;
                }
                // Now pDevInfo hasn't changed so REMOVE the device just added
                SLOG(eInfo2, "Removing Device %s", pFilter->m_szFilterName);
                pDevInfo->DataBuffer = NULL;
                pDevInfo->DataBufferSize = 0;
                {
                    CGenericIoControl gnt( pParams->hFile, pDevInfo, 
                        cbDeviceInfo, cbDeviceInfo, 
                        sizeof(DEVICEINFO), sizeof(DEVICEINFO),                                
                        ALLOCATOR_STATIC, IOCTL_WDMAUD_REMOVE_DEVNODE);
                    gnt.Test(FALSE,0);
                    if ( gnt.GetFault() != FAULT_NO_FAULT )
                        dwResult = FNS_FAIL;                
                }
            }

            SafeLocalFree(pDevInfo);
        }
    }		
    END_FOR_EACH();
        
    // Check 100 device limit for wdmaudio
    eDeviceType eDevType = DEVICE_UNKNOWN;
    pNode =  NULL;
    pFilter = NULL;
    FOR_EACH_FILTER(listFilters, pNode)
    {
        pFilter = pNode->pData;
        eDevType = IsValidAudioDevice(pFilter);
        if ( pFilter && (BOOL)eDevType && (eDevType != DEVICE_USB))   // this filters out USB devices for now 
        {
            // Found a valid device so break
            break;
        }
    }
    END_FOR_EACH();

    // Add pFilter 101 times, During 101st Addition it should return an error.
    if (pFilter)    {
        SLOG(eInfo2, "Adding Device %s 101 times", pFilter->m_szFilterName);
        cbDeviceInfo = sizeof(DEVICEINFO)+(sizeof(WCHAR)*lstrlen(pFilter->m_szFilterName));
        pDevInfo = (LPDEVICEINFO) LocalAlloc(LPTR, cbDeviceInfo);
    }
    if(!pDevInfo)
    {
        SLOG(LOG_LEVEL_ERROR, "Allocation Failed");
        return FNS_FAIL; 
    }
    MultiByteToWideChar( GetACP(),
                         MB_PRECOMPOSED,
                         pFilter->m_szFilterName,
                         -1, //since NULL terminated
                         pDevInfo->wstrDeviceInterface,
                         lstrlen(pFilter->m_szFilterName) + 1);    //+1 for NULL termination                
    pDevInfo->DeviceType = (eDevType == DEVICE_WAVE) ? WaveOutDevice : 
                         ( (eDevType == DEVICE_MIDI) ? MidiOutDevice :  MixerDevice);
    pDevInfo->DataBuffer = NULL;
    pDevInfo->DataBufferSize = 0;
    for (UINT uiAdd = 0; uiAdd < 101; uiAdd++)
    {
        CGenericIoControl gnt( pParams->hFile, pDevInfo, 
            cbDeviceInfo, cbDeviceInfo, 
            sizeof(DEVICEINFO), sizeof(DEVICEINFO),                                
            ALLOCATOR_STATIC, IOCTL_WDMAUD_ADD_DEVNODE);
        if (100 == uiAdd)
        {
            SLOG(1, "Adding device 101st time, expect Error");
            gnt.Test(TRUE,0);  // Adding device 101st time
        }
        else
        {    //Adding device 1-100 times 
            gnt.Test(FALSE,0);
        }
        
        if ( gnt.GetFault() != FAULT_NO_FAULT )                                    
        {
            if (100 != uiAdd)  // Failure is OK for 101st time.
                dwResult = FNS_FAIL;                                
        }
    }
    // Remove pFilter 101 times, During 101st Removal it should return an error.
    SLOG(eInfo2, "Removing Device %s 101 times", pFilter->m_szFilterName);
    for (UINT uiAdd = 0; uiAdd < 101; uiAdd++)
    {
        CGenericIoControl gnt( pParams->hFile, pDevInfo, 
                               cbDeviceInfo, cbDeviceInfo, 
                               sizeof(DEVICEINFO), sizeof(DEVICEINFO),                                
                               ALLOCATOR_STATIC, IOCTL_WDMAUD_REMOVE_DEVNODE);
        if (100 == uiAdd)
        {
            SLOG(1, "Removing device 101st time, expect Error");
            gnt.Test(TRUE,0);    // Adding device 101st time
        }
        else
        {
            // Adding device 1-100 times
            gnt.Test(FALSE,0);   
        }
        
        if ( gnt.GetFault() != FAULT_NO_FAULT )                                    
        {
            if (100 != uiAdd)  // Failure is OK for 101st time.
                dwResult = FNS_FAIL;                                
        }
    }
    SafeLocalFree(pDevInfo);

    // Add and Remove a non-existent device---------------------|
    //                                                          |
    // this testcase fails: Bug 701741                          V
    cbDeviceInfo = sizeof(DEVICEINFO)+(sizeof(WCHAR)*lstrlen("ABCDEFG"));    
    pDevInfo = (LPDEVICEINFO) LocalAlloc(LPTR, cbDeviceInfo);
    if(!pDevInfo)
    { return FNS_FAIL; /*spew some error*/ }
    MultiByteToWideChar( GetACP(),
                         MB_PRECOMPOSED,
                         "ABCDEFG", // Non-existent device.
                         -1, //since NULL terminated
                         pDevInfo->wstrDeviceInterface,
                         lstrlen(pFilter->m_szFilterName) + 1);    //+1 for NULL termination                
    pDevInfo->DeviceType = WaveOutDevice;   // doesn't really matter, since device does not exist
    SLOG(eInfo2, "Adding Non Existent device ");
    pDevInfo->DataBuffer = NULL;
    pDevInfo->DataBufferSize = 0;
    {
        CGenericIoControl gnt( pParams->hFile, pDevInfo, 
                               cbDeviceInfo, cbDeviceInfo, 
                               sizeof(DEVICEINFO), sizeof(DEVICEINFO),                                
                               ALLOCATOR_STATIC, IOCTL_WDMAUD_ADD_DEVNODE);
        gnt.Test(TRUE,0);
        if ( gnt.GetFault() != FAULT_NO_FAULT )
            dwResult = FNS_FAIL;
    }
    SLOG(eInfo2, "Removing Non Existent device");
    {
        CGenericIoControl gnt( pParams->hFile, pDevInfo, 
                               cbDeviceInfo, cbDeviceInfo, 
                               sizeof(DEVICEINFO), sizeof(DEVICEINFO),                                
                               ALLOCATOR_STATIC, IOCTL_WDMAUD_REMOVE_DEVNODE);
        gnt.Test(TRUE,0);
        if ( gnt.GetFault() != FAULT_NO_FAULT )
            dwResult = FNS_FAIL;
    }

    SafeLocalFree(pDevInfo);

    return dwResult;
}

//@-----------------------------------------------------------------------------------------
//                                    IOCTL_WDMAUD_REMOVE_DEVNODE
//@-----------------------------------------------------------------------------------------

// Function name   : Generic_Ioctl_IOCTL_WDMAUD_REMOVE_DEVNODE
// Description     : Test IOCTL_WDMAUD_REMOVE_DEVNODE
// Return type     : DWORD WINAPI 
// Argument        : PGAUDITPARAMS pParams
// History         : 2002     Created

//DWORD WINAPI Generic_Ioctl_IOCTL_WDMAUD_REMOVE_DEVNODE(PGAUDITPARAMS pParams)
//{
//    LPDEVICEINFO pDevInfo = NULL;
//    DWORD cbDeviceInfo = 0;
//    DWORD dwErr = 0;    
//    DWORD dwResult = FNS_PASS;
//
//    // Check if this is wdmaudio.sys filter
//    if(!(g_pGauditModule->GetDevice()->m_dwFlags & WDMAUD_DEVICE))
//        return FNS_PASS;
//
//    // KslEnumerate KSCATEGORY_AUDIO devices
//    TTList<CKsFilter> listFilters;
//    TTNode<CKsFilter>* pNode = NULL;
//    CKsFilter* pFilter = NULL;
//    LPGUID guidAudio[] = { const_cast<LPGUID>(&KSCATEGORY_AUDIO) };
//
//    if (!KslEnumFilters(&listFilters, ePCMAudio, guidAudio, 1, FALSE, FALSE, TRUE))
//        return FNS_FAIL;
//
//    // For each WAVE, MIDI or MIXER device, run tests
//    FOR_EACH_FILTER(listFilters, pNode)
//    {
//        pFilter = pNode->pData;
//        UINT uiDeviceType = IsValidAudioDevice(pFilter);
//        // COMMENT: Use #defines for the devices (uiDeviceType)
//        // TODO: Figure out how to identify what a USB device supports
//        if ( pFilter && (BOOL)uiDeviceType && (uiDeviceType != 4))   // this filters out USB devices for now
//        {
//            SLOG(eInfo3, "Adding Device %s", pFilter->m_szFilterName);
//            cbDeviceInfo = sizeof(DEVICEINFO)+(sizeof(WCHAR)*lstrlen(pFilter->m_szFilterName));
//            pDevInfo = (LPDEVICEINFO) LocalAlloc(LPTR, cbDeviceInfo);
//            if(!pDevInfo)
//            { return FNS_FAIL; /*spew some error*/ }
//            MultiByteToWideChar( GetACP(),
//                                 MB_PRECOMPOSED,
//                                 pFilter->m_szFilterName,
//                                 -1, //since NULL terminated
//                                 pDevInfo->wstrDeviceInterface,
//                                 lstrlen(pFilter->m_szFilterName) + 1);    //+1 for NULL termination            
//            // Test Out devices
//            pDevInfo->DeviceType = (uiDeviceType == 1) ? WaveOutDevice : 
//                                 ( (uiDeviceType == 2) ? MidiOutDevice :  MixerDevice);
//            
//            pDevInfo->DataBuffer = NULL;
//            pDevInfo->DataBufferSize = 0;
//            {
//                CGenericIoControl gnt( pParams->hFile, pDevInfo, 
//                                       cbDeviceInfo, cbDeviceInfo, 
//                                       sizeof(DEVICEINFO), sizeof(DEVICEINFO),                                
//                                       ALLOCATOR_STATIC, IOCTL_WDMAUD_REMOVE_DEVNODE);
//                gnt.Test(FALSE,0);
//                if ( gnt.GetFault() != FAULT_NO_FAULT )
//                    dwResult = FNS_FAIL;                
//            }
//            // Test In devices
//            pDevInfo->DeviceType = (uiDeviceType == 1) ? WaveInDevice : 
//                                 ( (uiDeviceType == 2) ? MidiInDevice :  MixerDevice);
//            
//            pDevInfo->DataBuffer = NULL;
//            pDevInfo->DataBufferSize = 0;
//            if (pDevInfo->DeviceType != MixerDevice) //Mixer Device has already been tested above
//            {
//                CGenericIoControl gnt( pParams->hFile, pDevInfo, 
//                                       cbDeviceInfo, cbDeviceInfo, 
//                                       sizeof(DEVICEINFO), sizeof(DEVICEINFO),                                
//                                       ALLOCATOR_STATIC, IOCTL_WDMAUD_REMOVE_DEVNODE);
//                gnt.Test(FALSE,0);
//                if ( gnt.GetFault() != FAULT_NO_FAULT )
//                    dwResult = FNS_FAIL;
//            }
//
//            SafeLocalFree(pDevInfo);
//        }
//    }		
//    END_FOR_EACH()
//
//    return dwResult;
//}

//@-----------------------------------------------------------------------------------------
//                                    IOCTL_WDMAUD_GET_CAPABILITIES
//@-----------------------------------------------------------------------------------------


// Function name   : Generic_Ioctl_IOCTL_WDMAUD_GET_CAPABILITIES
// Description     : Test IOCTL_WDMAUD_GET_CAPABILITIES
// Return type     : DWORD  
// Argument        : PGAUDITPARAMS pParams
// History         : 5/22/2002     Created

DWORD WINAPI Generic_Ioctl_IOCTL_WDMAUD_GET_CAPABILITIES(PGAUDITPARAMS pParams)
{
    LPDEVICEINFO pDevInfo = NULL;
    DWORD cbDeviceInfo = 0;
    DWORD dwErr = 0;    
    DWORD dwResult = FNS_PASS;

    // Check if this is wdmaudio.sys filter
    if(!(g_pGauditModule->GetDevice()->m_dwFlags & WDMAUD_DEVICE))
        return FNS_PASS;    

    if (!WdmaudInit(pParams))
    {
        SLOG(eError, "Allocation Failed");
        return FNS_FAIL;  
    }

    // KslEnumerate KSCATEGORY_AUDIO devices
    TTList<CKsFilter> listFilters;
    TTNode<CKsFilter>* pNode = NULL;
    CKsFilter* pFilter = NULL;
    LPGUID guidAudio[] = { const_cast<LPGUID>(&KSCATEGORY_AUDIO) };

    if (!KslEnumFilters(&listFilters, ePCMAudio, guidAudio, 1, FALSE, FALSE, TRUE))
        return FNS_FAIL;

    // This testcase fails: Bug 701741, 701713
    // For each WAVE, MIDI or MIXER device, run tests    
    FOR_EACH_FILTER(listFilters, pNode)
    {
        pFilter = pNode->pData;
        eDeviceType eDevType = IsValidAudioDevice(pFilter);        
        if ( pFilter && (BOOL)eDevType && (eDevType != DEVICE_USB))   // this filters out USB devices for now
        {
            SLOG(eInfo2, "Getting Device Caps for %s", pFilter->m_szFilterName);            

            cbDeviceInfo = sizeof(DEVICEINFO)+(sizeof(WCHAR)*lstrlen(pFilter->m_szFilterName));
            pDevInfo = (LPDEVICEINFO) LocalAlloc(LPTR, cbDeviceInfo);
            if(!pDevInfo)
            {
                SLOG(eError, "Allocation Failed");
                return FNS_FAIL; 
            }
            MultiByteToWideChar( GetACP(),
                                 MB_PRECOMPOSED,
                                 pFilter->m_szFilterName,
                                 -1, //since NULL terminated
                                 pDevInfo->wstrDeviceInterface,
                                 lstrlen(pFilter->m_szFilterName) + 1);    //+1 for NULL termination 

            // Test Out devices
            pDevInfo->DeviceType = (eDevType == DEVICE_WAVE) ? WaveOutDevice : 
                                 ( (eDevType == DEVICE_MIDI) ? MidiOutDevice :  MixerDevice);
            //if (pDevInfo->DeviceType != MixerDevice)
           // { 

                SLOG(eInfo2, "Testing Out Device");
                if (!WdmaudAdd(pParams, pDevInfo))
                {
                    SLOG(eError, "Adding of device %s failed", pFilter->m_szFilterName);
                    dwResult = FNS_FAIL;
                    goto  EXIT;
                } 

                UINT cbDevCaps = 0;   
                LPVOID lpDevCaps = NULL;            
                // Should fail since data buffer is NULL
                pDevInfo->DataBuffer = lpDevCaps;
                pDevInfo->DataBufferSize = cbDevCaps;
                {
                    CGenericIoControl gnt( pParams->hFile, pDevInfo, 
                        cbDeviceInfo, cbDeviceInfo, 
                        sizeof(DEVICEINFO), sizeof(DEVICEINFO),                                
                        ALLOCATOR_STATIC, IOCTL_WDMAUD_GET_CAPABILITIES);
                    gnt.Test(TRUE, ULONG(-1));
                    if ( gnt.GetFault() != FAULT_NO_FAULT )                                    
                        dwResult = FNS_FAIL;                                
                }

                //Valid case should pass            
                // CAPS2W are extended UNICODE CAPS, in mmsystem.h
                cbDevCaps = (eDevType == DEVICE_WAVE) ? sizeof(WAVEOUTCAPS2W) : 
                ( (eDevType == DEVICE_MIDI) ? sizeof(MIDIOUTCAPS2W) :  sizeof(MIXERCAPS2W));
                lpDevCaps = (LPVOID)LocalAlloc(LPTR, cbDevCaps);
                if(!lpDevCaps)
                {
                    SLOG(eError, "Allocation failed");
                    dwResult = FNS_FAIL;
                    goto  EXIT;
                }
                pDevInfo->DataBuffer = lpDevCaps;
                pDevInfo->DataBufferSize = cbDevCaps;
                {
                    CGenericIoControl gnt( pParams->hFile, pDevInfo, 
                        cbDeviceInfo, cbDeviceInfo, 
                        sizeof(DEVICEINFO), sizeof(DEVICEINFO),                                
                        ALLOCATOR_STATIC, IOCTL_WDMAUD_GET_CAPABILITIES);
                    gnt.Test(FALSE,sizeof(DEVICEINFO)); 
                    if ( gnt.GetFault() != FAULT_NO_FAULT )                                    
                        dwResult = FNS_FAIL;                                
                }
                SafeLocalFree(lpDevCaps);
                // Pass in a huge buffer and check if we get back some kernel data.
                cbDevCaps = (eDevType == DEVICE_WAVE) ? sizeof(WAVEOUTCAPS2W) : 
                ( (eDevType == DEVICE_MIDI) ? sizeof(MIDIOUTCAPS2W) :  sizeof(MIXERCAPS2W));            
                UINT cbHugeDevCaps = 1024*1024; // 1MB 
                lpDevCaps = (LPVOID)LocalAlloc(LPTR, cbHugeDevCaps);
                if(!lpDevCaps)
                {
                    SLOG(eError, "Allocation failed");
                    dwResult = FNS_FAIL;
                    goto  EXIT;
                }
                LPVOID lpSig = (LPVOID)LocalAlloc(LMEM_FIXED, 256); //No need for zeroinit
                if(!lpSig)
                {
                    SLOG(eError, "Allocation failed");
                    dwResult = FNS_FAIL;
                    goto  EXIT;
                }
                // Fill the buffer with some signature and check upon return            
                // !!!~~~*******@@@@@@@@!~@@@@@@@!!!!!!!!!@@@@@@########$$$$$$$$&&&&&&&#
                // Only filling 256 bytes, based on the rational, that memory would be filled
                // sequentially, 
                // Also the chances that memory has been written again with the same bytes, are remote            
                FillMemory(lpSig, 256, 0xAB);
                CopyMemory( ((char*)lpDevCaps) + cbDevCaps, lpSig, 256);
                pDevInfo->DataBuffer = lpDevCaps;
                pDevInfo->DataBufferSize = cbHugeDevCaps; // Huge chunk of memory
                {
                    CGenericIoControl gnt( pParams->hFile, pDevInfo, 
                        cbDeviceInfo, cbDeviceInfo, 
                        sizeof(DEVICEINFO), sizeof(DEVICEINFO),                                
                        ALLOCATOR_STATIC, IOCTL_WDMAUD_GET_CAPABILITIES);
                    gnt.Test(FALSE,sizeof(DEVICEINFO)); 
                    if ( gnt.GetFault() != FAULT_NO_FAULT )                                    
                        dwResult = FNS_FAIL;                                
                }
                if(memcmp( ((char*)lpDevCaps) + cbDevCaps, lpSig, 256))            
                {
                    SLOG(eError, "FAIL: Kernel Memory Returned");
                    dwResult = FNS_FAIL;
                    goto  EXIT;
                }
                SafeLocalFree(lpSig);
                SafeLocalFree(lpDevCaps);

                if (!WdmaudRemove(pParams, pDevInfo))
                { 
                    SLOG(eError, "Removal of device %s failed", pFilter->m_szFilterName);
                    dwResult = FNS_FAIL;
                    goto  EXIT;
                }
           // }

            // Test In devices
            pDevInfo->DeviceType = (eDevType == DEVICE_WAVE) ? WaveInDevice : 
                                 ( (eDevType == DEVICE_MIDI) ? MidiInDevice :  MixerDevice);
            if (pDevInfo->DeviceType != MixerDevice) //Mixer Device has already been tested above
            {
                SLOG(eInfo2, "Testing In Device");
                if (!WdmaudAdd(pParams, pDevInfo))
                { 
                    SLOG(eError, "Adding of device %s failed", pFilter->m_szFilterName);
                    dwResult = FNS_FAIL;
                    goto  EXIT;
//                    return FNS_FAIL; /*spew some error*/ 
                } 

                UINT cbDevCaps1 = 0;   
                LPVOID lpDevCaps1 = NULL;            

                // Should fail since data buffer is NULL
                pDevInfo->DataBuffer = lpDevCaps1;
                pDevInfo->DataBufferSize = cbDevCaps1;
                {
                    CGenericIoControl gnt( pParams->hFile, pDevInfo, 
                                           cbDeviceInfo, cbDeviceInfo, 
                                           sizeof(DEVICEINFO), sizeof(DEVICEINFO),                                
                                           ALLOCATOR_STATIC, IOCTL_WDMAUD_GET_CAPABILITIES);
                    gnt.Test(TRUE, ULONG(-1)); 
                    if ( gnt.GetFault() != FAULT_NO_FAULT )                                    
                        dwResult = FNS_FAIL;                                
                }

                //Valid case should pass            
                // CAPS2W are extended UNICODE CAPS, in mmsystem.h
                cbDevCaps1 = (eDevType == DEVICE_WAVE) ? sizeof(WAVEINCAPS2W) : sizeof(MIDIINCAPS2W);                
                lpDevCaps1 = (LPVOID)LocalAlloc(LPTR, cbDevCaps1);
                if(!lpDevCaps1)
                {
                    SLOG(eError, "Allocation failed");
                    dwResult = FNS_FAIL;
                    goto  EXIT;
                }
                pDevInfo->DataBuffer = lpDevCaps1;
                pDevInfo->DataBufferSize = cbDevCaps1;
                {
                    CGenericIoControl gnt( pParams->hFile, pDevInfo, 
                                           cbDeviceInfo, cbDeviceInfo, 
                                           sizeof(DEVICEINFO), sizeof(DEVICEINFO),                                
                                           ALLOCATOR_STATIC, IOCTL_WDMAUD_GET_CAPABILITIES);
                    gnt.Test(FALSE,sizeof(DEVICEINFO)); 
                    if ( gnt.GetFault() != FAULT_NO_FAULT )                                    
                        dwResult = FNS_FAIL;                                
                }
                SafeLocalFree(lpDevCaps1);

                // Pass in a huge buffer and check if we get back some kernel data.
                cbDevCaps1 = (eDevType == DEVICE_WAVE) ? sizeof(WAVEINCAPS2W) : sizeof(MIDIINCAPS2W);
                cbHugeDevCaps = 1024*1024; // 1MB 
                lpDevCaps1 = (LPVOID)LocalAlloc(LPTR, cbHugeDevCaps);
                if(!lpDevCaps1)
                {
                    SLOG(eError, "Allocation failed");
                    dwResult = FNS_FAIL;
                    goto  EXIT;
                }
                LPVOID lpSig1 = (LPVOID)LocalAlloc(LMEM_FIXED, 256); //No need for zeroinit
                if(!lpSig1)
                {
                    SLOG(eError, "Allocation failed");
                    dwResult = FNS_FAIL;
                    goto  EXIT;
                }
                // Fill the buffer with some signature and check upon return            
                // !!!~~~*******@@@@@@@@!~@@@@@@@!!!!!!!!!@@@@@@########$$$$$$$$&&&&&&&#
                // Only filling 256 bytes, based on the rational, that memory would be filled
                // sequentially, 
                // Also the chances that memory has been written again with the same bytes, are remote            
                FillMemory(lpSig1, 256, 0xAB);
                CopyMemory( ((char*)lpDevCaps1) + cbDevCaps1, lpSig1, 256);
                pDevInfo->DataBuffer = lpDevCaps1;
                pDevInfo->DataBufferSize = cbHugeDevCaps; // Huge chunk of memory
                {
                    CGenericIoControl gnt( pParams->hFile, pDevInfo, 
                                           cbDeviceInfo, cbDeviceInfo, 
                                           sizeof(DEVICEINFO), sizeof(DEVICEINFO),                                
                                           ALLOCATOR_STATIC, IOCTL_WDMAUD_GET_CAPABILITIES);
                    gnt.Test(FALSE,sizeof(DEVICEINFO)); 
                    if ( gnt.GetFault() != FAULT_NO_FAULT )                                    
                        dwResult = FNS_FAIL;                                
                }                
                if(memcmp( ((char*)lpDevCaps1) + cbDevCaps1, lpSig1, 256))            
                {
                    SLOG(eError, "FAIL: Kernel Memory Returned");
                    dwResult = FNS_FAIL;
                    goto  EXIT;
                }
                SafeLocalFree(lpDevCaps1);
                SafeLocalFree(lpSig1);
                if (!WdmaudRemove(pParams, pDevInfo))
                {
                    SLOG(eError, "Removal of device %s failed", pFilter->m_szFilterName);
                    dwResult = FNS_FAIL;
                    goto  EXIT;
                }
            }
            SafeLocalFree(pDevInfo);
        }
    }		
    END_FOR_EACH();
    EXIT:
        SafeLocalFree(pDevInfo);
        return dwResult;
}



// Function name   : IsValidAudioDevice
// Description     : Checks if the filter is a valid Audio Device, and returns an approp value
// Return type     : UINT 
// Argument        : CKsFilter* pFilter
// History         : 2002     Created

eDeviceType IsValidAudioDevice(CKsFilter* pFilter)
{
    if (!pFilter)
        return DEVICE_UNKNOWN;
    
   // It is a wave device                     
   if (strstr(pFilter->m_szFilterName, "\\wave" )) 
       return DEVICE_WAVE;
   // It is an External MIDI device      
   if ( strstr(pFilter->m_szFilterName, "\\uart" ) || 
        strstr(pFilter->m_szFilterName, "\\fmsynth" ) ) 
       return DEVICE_MIDI;
   // It is a mixer device
   if (strstr(pFilter->m_szFilterName, "\\topology"))
       return DEVICE_MIXER;
   // It is a USB device
   if (strstr(pFilter->m_szFilterName, "\\global" )) 
       return DEVICE_USB;
        
   return DEVICE_UNKNOWN;
}

// Function name   : WdmaudInit
// Description     : Initialize wdmaud, reqd for sending all other IOCTLS
// Return type     : BOOL 
// Argument        : void
BOOL WdmaudInit(PGAUDITPARAMS pParams)
{
    BOOL fResult = TRUE;
    LPDEVICEINFO pDevInfo = NULL;
    pDevInfo = (LPDEVICEINFO) new BYTE [sizeof(DEVICEINFO)];
    if (!pDevInfo)
    {
        return FALSE;
    }
    {        
        CGenericIoControl gnt( pParams->hFile, pDevInfo, 
                               sizeof(DEVICEINFO), sizeof(DEVICEINFO), 
                               sizeof(DEVICEINFO), sizeof(DEVICEINFO),                                
                               ALLOCATOR_STATIC, IOCTL_WDMAUD_INIT);
        gnt.Test(FALSE, 0);    
        if (gnt.GetFault() != FAULT_NO_FAULT) 
        {
            fResult =FALSE;
        }        
    }
    if (pDevInfo)
        delete[] pDevInfo;
    // Initialize wdmaud, 
    return fResult;
}

// Function name   : WdmaudAdd
// Description     : Adds the device specified
// Return type     : BOOL 
// Argument        : PGAUDITPARAMS pParams
// Argument        : LPDEVICEINFO pDevInfo
BOOL WdmaudAdd(PGAUDITPARAMS pParams, LPDEVICEINFO pDevInfo)
{
    // Add the device just passed in
    BOOL fResult = TRUE;
    UINT cbInputBuff = sizeof(WCHAR) * wcslen(pDevInfo->wstrDeviceInterface);
    pDevInfo->DataBuffer = NULL;
    pDevInfo->DataBufferSize = 0;
    {
        CGenericIoControl gnt( pParams->hFile, pDevInfo, 
                               sizeof(DEVICEINFO) + cbInputBuff, sizeof(DEVICEINFO) + cbInputBuff , 
                               sizeof(DEVICEINFO), sizeof(DEVICEINFO),                                
                               ALLOCATOR_STATIC, IOCTL_WDMAUD_ADD_DEVNODE);
        gnt.Test(FALSE,0);
        if ( gnt.GetFault() != FAULT_NO_FAULT )                                    
            fResult = FALSE;
    }
    return fResult;
}

// Function name   : WdmaudRemove
// Description     : Remove the specified device
// Return type     : BOOL 
// Argument        : PGAUDITPARAMS pParams
// Argument        : LPDEVICEINFO pDevInfo
BOOL WdmaudRemove(PGAUDITPARAMS pParams, LPDEVICEINFO pDevInfo)
{    
    // Remove the device just passed in 
    BOOL fResult = TRUE;
    UINT cbInputBuff = sizeof(WCHAR) * wcslen(pDevInfo->wstrDeviceInterface);
    pDevInfo->DataBuffer = NULL;
    pDevInfo->DataBufferSize = 0;
    {
        CGenericIoControl gnt( pParams->hFile, pDevInfo, 
                               sizeof(DEVICEINFO) + cbInputBuff, sizeof(DEVICEINFO) + cbInputBuff , 
                               sizeof(DEVICEINFO), sizeof(DEVICEINFO),                                
                               ALLOCATOR_STATIC, IOCTL_WDMAUD_REMOVE_DEVNODE);
        gnt.Test(FALSE,0);
        if ( gnt.GetFault() != FAULT_NO_FAULT )                                    
            fResult = FALSE;
    }
    return fResult;
}
