// Copyright (C) Microsoft. All rights reserved.
#include "common.h"

LPTSTR g_szKnownSystemFiles[] = 
{ 
    _T("drmk.sys"), 
    _T("swenum.sys"),
    _T("ks.sys"), 
    _T("kmixer.sys"), 
    _T("portcls.sys"), 
    _T("hdaudio.sys"),
    _T("hdaudbus.sys"),
    _T("usbaudio.sys"),
    _T("usbccgp.sys"),
    _T("openhci.sys"),
    _T("uhcd.sys"),   
    _T("usbohci.sys"),
    _T("usbuhci.sys"),
    _T("usbehci.sys"),
    _T("usbhub.sys"),
    _T("usbport.sys"),
    _T("usbd.sys"),
    _T("sbemul.sys"),
    _T("ntoskrnl.exe"),
    _T("ntkrnlpa.exe"),
    _T("usbhub3.sys"),
};

LPTSTR g_szPortClsFilesXP[] =    // no SWEnum or SBEmul
{
    _T("drmk.sys"), 
    _T("ks.sys"), 
    _T("kmixer.sys"), 
    _T("portcls.sys"),
};

LPTSTR g_szPortClsFilesVista[] =
{
    _T("drmk.sys"), 
// !!!!!!!! WORKAROUND for BUG 1824029 !!!!!!!!!!
// Bug 1824029 causes ks.sys not to show up in the list of drivers walked by DRMK.
// This only shows up in x64 since the problem is with ksthunk.sys. 
// Since that bug is a BIG security hole and until that it is fixed, DRMTest will
// not check for ks.sys and bring out this issue in front of any potential hackers.
// Once this issue is fixed, remove the #ifndef _WIN64
#ifdef WHQL 
#ifndef _WIN64
    _T("ks.sys"), 
#endif //_WIN64
#else
    _T("ks.sys"), 
#endif //WHQL
    _T("portcls.sys"),
};

LPTSTR g_szPortClsFilesWin7[] =
{
    _T("drmk.sys"), 
    _T("ks.sys"), 
    _T("portcls.sys"),
};

LPTSTR g_szPortClsFilesWin8[] =
{
    _T("drmk.sys"), 
    _T("ks.sys"), 
    _T("portcls.sys"),
};

LPTSTR g_szUsbAudioFilesXP[] =   // no SWEnum or SBEmul
{
    _T("drmk.sys"), 
    _T("ks.sys"), 
    _T("kmixer.sys"), 
    // Not required in cases of 3rd party USB driver
    //_T("usbaudio.sys"),
    _T("usbhub.sys"),
    _T("usbccgp.sys"),
    _T("usbport.sys"),
};

LPTSTR g_szUsbAudioFilesVista[] = // usbhub.sys
{
    _T("drmk.sys"), 
    _T("ks.sys"),
    // Not required in cases of 3rd party USB driver
    //_T("usbaudio.sys"),
    _T("usbccgp.sys"),
    _T("usbport.sys"),
};

LPTSTR g_szUsbAudioFilesWin7[] = 
{
    _T("drmk.sys"), 
    _T("ks.sys"),
    _T("usbccgp.sys"),
    // at this point one of two things happens
    //
    // either (1) the Microsoft stack continues with usbhub.sys
    // due to a Windows 7 bug, usbccgp.sys mistakenly fingered usbport.sys instead of usbhub.sys
    //
    // or (2) a third party USB stack like Intel's iusb3xhc.sys kicks in
    // core stacks are no longer required to be instrumented for Trusted Audio Drivers
    //
    // so we probe no further
};

LPTSTR g_szUsbAudioFilesWin8[] = 
{
    _T("drmk.sys"), 
    _T("ks.sys"),
    _T("usbccgp.sys"),
    // at this point one of two things happens
    //
    // either (1) the Microsoft stack continues with usbhub.sys or usbhub3.sys
    // usbhub.sys no longer fingers usbport.sys, so that is the last driver in the chain
    //
    // or (2) a third party USB stack like Intel's iusb3xhc.sys kicks in
    // core stacks are no longer required to be instrumented for Trusted Audio Drivers
    //
    // so we probe no further
};

LPTSTR g_szUsbHostController[] = 
{
    _T("openhci.sys"),
    _T("uhcd.sys"),   
    _T("usbohci.sys"),
    _T("usbuhci.sys"),
    _T("usbehci.sys"),
    _T("iusb3xhc.sys"), // Third-party Intel USB3 controller
};


#define E_UNSUPPORTED HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED)
// ------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------
int BinaryInAuthParamsList(LPTSTR strDriverSearch, AUTHPARAM* pAP)
{
    int     iRet = -1;
    CString szDriverName;

    for (int j = 0; j < (int)pAP->NumFailures; j++)
    {
        // verify that the driver that failed is in the list of drivers that 
        // we anticipate failing
        szDriverName = pAP->DriverFailures[j].path;
        szDriverName = szDriverName.Mid(szDriverName.ReverseFind(_T('\\'))+1);
        if (!szDriverName.IsEmpty())
        {
            if (0 == _tcsicmp(strDriverSearch, szDriverName))
            {
                iRet = j;
                break;
            }
        }
    }
    return iRet;
}

//--------------------------------------------------------------------------;
//
//  Authenticate (Vista and newer)
//
//  Description:
//      Creates a DRM stream and authenticates it.
//
//  Arguments:
//      
//  Return:
//      (BOOL)
//
//--------------------------------------------------------------------------;
HRESULT Authenticate(PCAudioDevice pDevice, PAUTHPARAM pAuthParam)
{
    HRESULT             hr = S_OK;
    BOOL                fAuthentication = TRUE;
    CKsTest             KsTest;
    CMemoryPropertyStore MemPropStore;
    DRMRIGHTS           DrmRights = { FALSE, FALSE, FALSE };
    DRMKHANDLE          DRMKHandle = NULL;
    STREAMHANDLE        StreamHandle = NULL;
    DWORD               StreamId = 0;
    DRM_STATUS          drmStatus = DRM_OK;
    
    if(!pDevice)    return E_FAIL;

    // Open an audio stream using DirectKS
    if(!KsTest.Initialize(pDevice->m_pFilter))
    {
        XLOG(XMSG, eError, _T("Failure condition: Unable to Initialize KSTest object"));
        hr = E_FAIL;
    }
    else if(!KsTest.CreateRender(TEST_BUFFER_MS))
    {
        XLOG(XMSG, eError, _T("Failure condition: Failed to CreateRender pin"));
        hr = E_FAIL;
    }

    // Setup a PropertyStore
    else if(MemPropStore.Initialize() != S_OK)
    {
        XLOG(XMSG, eError, _T("Failure condition: Failed to initialize PropertyStore"));
        hr = E_FAIL;
    }

    // Open a DRMK Stream
    else if(SXDRMKOpen(&DRMKHandle) != DRM_OK)
    {
        XLOG(XMSG, eError, _T("Failure condition: Unable to open DRMK handle"));
        hr = E_FAIL;
    }
    else if ((drmStatus = SXDRMKCreateStream(DRMKHandle, pDevice->m_pFilter->m_pRenderPin->m_handle,
                            (IPropertyStore*)&MemPropStore,  
                            &StreamId, &StreamHandle, &DrmRights))
                 != DRM_OK )
    {
        // on OneCore, the cryptcat service isn't running, so stream creation/initialization fails with a DRM sys error.
        // If this is hit, skip the test. This happens when DRMKRM service is running (the drmkopen above succeeds), but cryptcat is not.
        if (drmStatus == DRM_SYSERR)
        {
            XLOG(XMSG, eInfo1, _T("DRMKCreateStream failed, this can happen on SKU's that lack DRM support, did not authenticate."));
            hr = E_UNSUPPORTED;
        }
        else
        {
            XLOG(XMSG, eError, _T("Failure condition: DRMKCreateStream failed"));
            hr = E_FAIL;
        }
    }

    if (S_OK == hr)
    {
        // Authenticate
        fAuthentication = (SXDRMKAuthenticate(
                StreamHandle,                pAuthParam->ParseCatalogFile, 
                pAuthParam->MinDRMLevel,       pAuthParam->RequireSignedCatalog,
                pAuthParam->CheckDriverImages, pAuthParam->DriverFailures,
                &pAuthParam->NumFailures,      pAuthParam->MaxFailures,
                pAuthParam->AllowTestCertificate) == S_OK);
        if (!fAuthentication)
        {
            XLOG(XMSG, eError, _T("Authentication failed"));
            hr = E_FAIL;
        }
    }

    //Cleanup
    if (StreamHandle)
    {
        SXDRMKDestroyStream(StreamHandle);
    }
    if (DRMKHandle)
    {
        SXDRMKClose(DRMKHandle);
    }
    KsTest.DestroyRender();
    return hr;
} // Authenticate

//--------------------------------------------------------------------------;
//
//  IsDRMServiceRunning (Mobile vs desktop)
//
//  Description:
//      Finds out if the DRMK service is running. If it is not running, then
//      this is likely running on a OneCore based system. If so, the test
//      should skip
//
//  Arguments:
//      
//  Return:
//      (BOOL)
//
//--------------------------------------------------------------------------;
BOOL IsDRMServiceRunning()
{
    BOOL                fRet = TRUE;
    DRMKHANDLE          DRMKHandle = NULL;

    // Attempt to open DRMK. If we get a not running error,
    // then the service isn't running, which is an indication
    // this is running on onecore.
    if(SXDRMKOpen(&DRMKHandle) == DRM_KRM_NOT_RUNNING)
    {
        fRet = FALSE;
    }

    if (DRMKHandle)
    {
        SXDRMKClose(DRMKHandle);
    }

    return fRet;
}


//--------------------------------------------------------------------------;
//
//  IsDrmInterfaceImplemented (Vista and newer)
//
//  Description:
//      Finds out if the driver has implemented Drm interfaces or not.
//
//  Arguments:
//      
//  Return:
//      (BOOL)
//
//--------------------------------------------------------------------------;
HRESULT IsDrmInterfaceImplemented(PCAudioDevice pDevice)
{
    HRESULT             hr = S_OK;
    CKsTest             KsTest;
    CMemoryPropertyStore MemPropStore;
    AUTHPARAM           authParam = {0};
    DRMKHANDLE          DRMKHandle = NULL;
    STREAMHANDLE        StreamHandle = NULL;
    DWORD               StreamId = 0;
    DRMRIGHTS           DrmRights = { FALSE, TRUE, FALSE };
    DRM_STATUS          drmStatus = DRM_OK;

    if(!pDevice)    return E_FAIL;

    authParam.MaxFailures           = MAX_FAILURES;
    authParam.RequireSignedCatalog  = TRUE;
    authParam.AllowTestCertificate  = (g_pDRMTest->m_fDriversRealSigned ? FALSE : TRUE);
    authParam.CheckDriverImages     = TRUE;
    authParam.MinDRMLevel           = 0xFFFF;
    authParam.ParseCatalogFile      = TRUE;
    // Authentication cannot succeed with the above settings.
    // They are simply too restrictive.
    // If the drivers have not implemented the Drm interface,
    // there should not be any drivers in the Failure list.
    
    // Open an audio stream using DirectKS
    if(!KsTest.Initialize(pDevice->m_pFilter))
    {
        XLOG(XMSG, eError, _T("Failure condition: Unable to Initialize KSTest object"));
        hr = E_FAIL;
    }   
    else if(!KsTest.CreateRender(500))
    {
        XLOG(XMSG, eError, _T("Failure condition: Failed to CreateRender pin"));
        hr = E_FAIL;
    }

    // Setup a PropertyStore
    else if(MemPropStore.Initialize() != S_OK)
    {
        XLOG(XMSG, eError, _T("Failure condition: Failed to initialize PropertyStore"));
        hr = E_FAIL;
    }

    // Open a DRMK Stream
    else if(SXDRMKOpen(&DRMKHandle) != DRM_OK)
    {
        XLOG(XMSG, eError, _T("Failure condition: Unable to open DRMK handle"));
        hr = E_FAIL;
    }
    else if ((drmStatus = SXDRMKCreateStream(DRMKHandle, pDevice->m_pFilter->m_pRenderPin->m_handle,
                            (IPropertyStore*)&MemPropStore,  
                            &StreamId, &StreamHandle, &DrmRights))
                 != DRM_OK )
    {
        // on OneCore, the cryptcat service isn't running, so stream creation/initialization fails with a DRM sys error.
        // If this is hit, skip the test. This happens when DRMKRM service is running (the drmkopen above succeeds), but cryptcat is not.
        if (drmStatus == DRM_SYSERR)
        {
            XLOG(XMSG, eInfo1, _T("DRMKCreateStream failed, this can happen on some SKU's and DRM is not supported, did not authenticate."));
            hr = E_UNSUPPORTED;
        }
        else
        {
            XLOG(XMSG, eError, _T("Failure condition: Creation of the DRM stream failed"));
            hr = E_FAIL;
        }
    }
    // The idea here is that SXDRMKAuthenticate should fail due to the overly restrictive 
    // parameters.  If DRM is implemented, then the DriverFailures array should be populated.
    // If this array is not populated (i.e. if NumFailures == 0), then we can assume that 
    // DRM is not implemented

    // !!!! TODO:  Figure out under what conditions this returns 0xC000000D (inval param)
    else if( SXDRMKAuthenticate(
            StreamHandle,                authParam.ParseCatalogFile, 
            authParam.MinDRMLevel,       authParam.RequireSignedCatalog,
            authParam.CheckDriverImages, authParam.DriverFailures,
            &authParam.NumFailures,      authParam.MaxFailures,
            authParam.AllowTestCertificate)
                            == DRM_OK )
    {
        // this should always fail since we specified CheckCat = TRUE and we 
        // have an impossibly high MinDrmLevel.
        XLOG(XMSG, eError, _T("\nFailure condition: WARNING ::::: VERY VERY UNEXPECTED. CONTACT WHQL :::::\n"));
        hr = E_FAIL;
    }

    if (SUCCEEDED(hr) && (authParam.NumFailures == 0))
    {
        // If no drivers are returned from SXDRMKAuthenticate (with overly restrictive
        // parameters), that means the driver is not implementing Drm interfaces.
        XLOG(XMSG, eInfo1, _T("Driver does not implement the DRM interfaces"));
        hr = E_FAIL;
    }

    //Cleanup
    if (StreamHandle)
    {
        SXDRMKDestroyStream(StreamHandle);
    }
    if (DRMKHandle)
    {
        SXDRMKClose(DRMKHandle);
    }
    KsTest.DestroyRender();
    return hr;
}

//--------------------------------------------------------------------------;
//
//  Test_Miniport_SetContentId
//
//  Description:
//      - For each render device 
//       - Create a PCM pin
//        - Set ContentId on the pin   
//
//  Arguments:
//      
//  Return:
//      (DWORD)
//
//--------------------------------------------------------------------------;
DWORD 
Test_Miniport_SetContentId()
{
    PCAudioDevice               pDevice = g_pDRMTest->m_pDeviceUnderTest;
    DWORD                       dwResult = FNS_PASS;
    HRESULT                     hr = S_OK;

    if (g_pDRMTest->m_listRender.IsEmpty())
    {
        XLOG(XSKIP, eInfo2, _T("Skipping because there is no rendering device when running the device level test"));
        return FNS_SKIPPED;
    }

    if( !IsDRMServiceRunning())
    {
        XLOG(XSKIP, eInfo2, _T("Skipping because the DRMK service is not running, indicating this is a SKU that lacks DRM support."));
        return FNS_SKIPPED;
    }
#ifdef WHQL
    if (SUCCEEDED(hr = IsDrmInterfaceImplemented(pDevice)))
    {
        XLOG(XMSG, eInfo1, _T("Device has implemented DRM interface."));
    }
    else if (E_UNSUPPORTED == hr)
    {
        XLOG(XSKIP, eInfo2, _T("Skipping because the cryptcat service is not running, indicating this is a SKU that lacks DRM support."));
        return FNS_SKIPPED;
    }
    else
    {
        XLOG(XFAIL, eError, _T("FAIL: Device has NOT implemented DRM interface."));
        dwResult = FNS_FAIL;
    }
#else //WHQL
    if (SUCCEEDED(IsDrmInterfaceImplemented(pDevice)))
    {
        if (pDevice->IsDRMCompliant())
            XLOG(XMSG, eInfo1, _T("Device has implemented DRM interface and is in list of known DRM devices."));
        else
        {
            dwResult = FNS_FAIL;
            g_IShell->IncrementIndent();
            XLOG(XMSG, eFatalError, _T(""));
            XLOG(XMSG, eFatalError, _T("~ Failure condition ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
            XLOG(XMSG, eFatalError, _T("Device has implemented DRM interface but is not in list of known DRM drivers"));
            XLOG(XMSG, eFatalError, _T("If this is an inbox driver (i.e. ships with the OS you are testing), please email"));
            XLOG(XMSG, eFatalError, _T("the owner of this test with the name of this device: \"%s\""), pDevice->GetName());
            XLOG(XMSG, eFatalError, _T("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
            g_IShell->DecrementIndent();

        }
    }
    else
    {
        if (pDevice->IsDRMCompliant())
        {
            XLOG(XFAIL, eError, _T("Failure condition:  Device is listed as a known DRM-compliant device, but the DRM Interface does not appear to be implemented."));
            dwResult = FNS_FAIL;
        }
        else
            XLOG(XMSG, eInfo1, _T("Device has not implemented DRM interface, but is not in list of known DRM devices.  This is okay."));
    }
#endif //WHQL

    return dwResult;
} // Test_Miniport_SetContentId

//--------------------------------------------------------------------------;
//
//  Test_Authentication_Image
//
//  Description:
//       Call authenticate with CheckImage = TRUE.
//
//  Arguments:
//      
//  Return:
//      (DWORD)
//
//--------------------------------------------------------------------------;
DWORD
Test_Authentication_Image()
{
    DWORD           dwResult = FNS_PASS;
    HRESULT         hr = S_OK;
    AUTHPARAM       authParam = {0};
    PCAudioDevice   pDevice = g_pDRMTest->m_pDeviceUnderTest;

    if (g_pDRMTest->m_listRender.IsEmpty())
    {
        XLOG(XSKIP, eInfo2, _T("Skipping because there is no rendering device when running the device level test"));
        return FNS_SKIPPED;
    }

    //ZeroMemory(&authParam, sizeof(authParam));
    authParam.MaxFailures = MAX_FAILURES;
    authParam.DriverFileCache = g_pDRMTest->m_DefaultAuthenticate.DriverFileCache;
    authParam.RequireSignedCatalog = TRUE;

    authParam.CheckDriverImages = TRUE;


    if( !IsDRMServiceRunning())
    {
        XLOG(XSKIP, eInfo2, _T("Skipping because the DRMK service is not running, indicating this is a SKU that lacks DRM support."));
        dwResult = FNS_SKIPPED;
    }
    else if (FAILED(hr = Authenticate(pDevice, &authParam)))
    {
        if (E_UNSUPPORTED == hr)
        {
            XLOG(XSKIP, eInfo2, _T("Skipping because creation of a DRMK stream failed, likely due to this being on a SKU that lacks DRM support."));
            dwResult = FNS_SKIPPED;
        }
        else
        {
            XLOG(XFAIL, eError, _T("\nFAIL: Authentication (IMAGE) failed"));
            dwResult = FNS_FAIL;
        }
    }

    return dwResult;
} // Test_Authentication_Image

//--------------------------------------------------------------------------;
//
//  Test_Authentication_Authenticate
//
//  Description:
//      Call authenticate with everything set to TRUE.
//
//  Arguments:
//      
//  Return:
//      (DWORD)
//
//--------------------------------------------------------------------------;
DWORD 
Test_Authentication_Authenticate()
{
    DWORD           dwResult = FNS_PASS;
    HRESULT         hr = S_OK;
    AUTHPARAM       authParam;
    PCAudioDevice   pDevice = g_pDRMTest->m_pDeviceUnderTest;

    if (g_pDRMTest->m_listRender.IsEmpty())
    {
        XLOG(XSKIP, eInfo2, _T("Skipping because there is no rendering device when running the device level test"));
        return FNS_SKIPPED;
    }

    ZeroMemory(&authParam, sizeof(authParam));
    authParam.MaxFailures = MAX_FAILURES;
    authParam.DriverFileCache = g_pDRMTest->m_DefaultAuthenticate.DriverFileCache;
    authParam.RequireSignedCatalog = TRUE;

    authParam.AllowTestCertificate  = FALSE;
    authParam.CheckDriverImages = TRUE;
    authParam.MinDRMLevel = g_dwDRMLevelThisOS;
    authParam.ParseCatalogFile = TRUE;

#ifdef WHQL
    if (FAILED(hr = Authenticate(pDevice, &authParam)))
    {
        if (E_UNSUPPORTED == hr)
        {
            XLOG(XSKIP, eInfo2, _T("Skipping because DRMK stream creation failed due to a DRM system error, likely due to running on a SKU that lacks DRM support."));
            return FNS_SKIPPED;
        }
        else
        {
            XLOG(XFAIL, eError, _T("\nFAIL: Authentication (ALL) failed"));
            dwResult = FNS_FAIL;
        }
    }
#else //WHQL
    //
    // Some comments here (PLEASE READ) ....
    // For release version of Millennium:
    //    The drivers are real signed.
    //    Driver cache does not allow test signatures.
    //    So both of the calls should pass.
    // For early builds of Millennium.
    //    Drivers are test signed.
    //    Driver cache allows test signatures.
    //    The first call should fail, but the seconds should
    //    pass.

    // For early versions of XP:
    //    The drivers are test signed.
    //    Driver cache allows test signatures.
    //    So first call should fail and second should pass

    // Please adjust the following values accordingly.
    //      m_fDriversRealSigned
    //      m_fCacheAllowsTestCert
    //
/*
    XLOG(XMSG, eInfo2, 
        "\nDriversRealSigned : %s \nDriverCacheAllowsTest : %s\n",
        g_pDRMTest->m_fDriversRealSigned ? "TRUE" : "FALSE",
        g_pDRMTest->m_fCacheAllowsTestCert ? "TRUE" : "FALSE");

    authParam.AllowTestCertificate = FALSE; // ~~~~~~~~

    if (SUCCEEDED(Authenticate(pDevice, &authParam)))
    {
        if (!g_pDRMTest->m_fDriversRealSigned && pDevice->IsDRMCompliant())
        {
            XLOG(XFAIL, eError, 
                "\nAuthentication succeeded. "
                "This should always fail for Millennium or Whistler Internal builds.");
            dwResult = FNS_FAIL;
        }
    }

    authParam.AllowTestCertificate = TRUE; // ~~~~~~~~

    if (SUCCEEDED(Authenticate(pDevice, &authParam)))
    {
        if (!g_pDRMTest->m_fDriversRealSigned && !g_pDRMTest->m_fCacheAllowsTestCert)
        {
            XLOG(XFAIL, eError, 
                "\nAuthentication succeeded. "
                "This should fail with test signed drivers if driver cache "
                "disallows test signatures.");
            dwResult = FNS_FAIL;
        }
    }
    else
    {
        if (g_pDRMTest->m_fDriversRealSigned && pDevice->IsDRMCompliant())
        {
            XLOG(XFAIL, eError,
                "\n Authentication failed. "
                "This should pass with real signed drivers");
            dwResult = FNS_FAIL;
        }

        if (!g_pDRMTest->m_fDriversRealSigned && g_pDRMTest->m_fCacheAllowsTestCert && pDevice->IsDRMCompliant()) 
        {
            XLOG(XFAIL, eError, 
                "\nAuthentication failed.  This should always succeed at this level with AllowTestCertificate "
                "= TRUE for Millennium or Whistler Internal builds.");
            dwResult = FNS_FAIL;
        }
    }
*/

    // This is a rewrite of the above.
    {
        authParam.AllowTestCertificate = FALSE; // ~~~~~~~~

        if (SUCCEEDED(Authenticate(pDevice, &authParam)))
        {
            if (!g_pDRMTest->m_fDriversRealSigned)
            {
                XLOG(XFAIL, eError, _T("\nFailure condition:  Authentication succeeded on TestSigned driver even though AllowTestCertificate = FALSE."));
                dwResult = FNS_FAIL;
            }
        }
        else
        {
            if (pDevice->IsDRMCompliant())
            {
                if (g_pDRMTest->m_fDriversRealSigned)
                {
                    XLOG(XFAIL, eError, _T("\nFailure condition:  Authentication failed on RealSigned driver."));
                    dwResult = FNS_FAIL;
                }
                else
                {
                    XLOG(XMSG, eInfo1, _T("\nAuthentication failed on TestSigned driver.  This is expected with AllowTestCertificate = FALSE."));
                }
            }
        }

        authParam.AllowTestCertificate = TRUE; // ~~~~~~~~

        if (SUCCEEDED(Authenticate(pDevice, &authParam)))
        {
            if (!pDevice->IsDRMCompliant())
            {
                XLOG(XFAIL, eError, _T("\nFailure condition:  Authentication succeeded on non-DRM compliant driver."));
                dwResult = FNS_FAIL;
            }
        }
        else
        {
            if (pDevice->IsDRMCompliant())
            {
                XLOG(XFAIL, eError, _T("\nFAIL: Authentication failed.  This should always succeed at this level with AllowTestCertificate ")
                             _T("= TRUE for Millennium or Whistler Internal builds."));
                dwResult = FNS_FAIL;
            }
        }
    }

#endif //WHQL

    return dwResult;
} // Test_Authentication_Authenticate

//--------------------------------------------------------------------------;
//
//  Test_Authentication_MinDrmLevel
//
//  Description:
//     Call authenticate with everything set to TRUE and different drmLevels.
//
//  Arguments:
//      
//  Return:
//      (DWORD)
//
//--------------------------------------------------------------------------;
DWORD 
Test_Authentication_MinDrmLevel()
{
    DWORD           dwResult = FNS_PASS;
    AUTHPARAM       authParam;
    PCAudioDevice   pDevice = g_pDRMTest->m_pDeviceUnderTest;
    HRESULT hr = S_OK;

    if (g_pDRMTest->m_listRender.IsEmpty())
    {
        XLOG(XSKIP, eInfo2, _T("Skipping because there is no rendering device when running the device level test"));
        return FNS_SKIPPED;
    }

    ZeroMemory(&authParam, sizeof(authParam));
    authParam.MaxFailures = MAX_FAILURES;
    authParam.DriverFileCache = g_pDRMTest->m_DefaultAuthenticate.DriverFileCache;
    authParam.RequireSignedCatalog = TRUE;

    authParam.AllowTestCertificate = TRUE;
    authParam.CheckDriverImages = TRUE;
    authParam.ParseCatalogFile = TRUE;


    authParam.MinDRMLevel = g_dwDRMLevelThisOS;
    if (FAILED(hr = Authenticate(pDevice, &authParam)))
    {
        if (E_UNSUPPORTED == hr)
        {
            XLOG(XSKIP, eInfo2, _T("DRM system reported unsupported, running on a SKU that lacks DRM support/catalogging."));
            return FNS_SKIPPED;
        }
        else
        {
            XLOG(XFAIL, eError, _T("\nFAIL: Authentication (%d) failed"), g_dwDRMLevelThisOS);
            dwResult = FNS_FAIL;
        }
    }
    else
    {
        XLOG(XMSG, eInfo1, _T("\nAuthentication (%d) succeeded"), g_dwDRMLevelThisOS);
    }

    authParam.MinDRMLevel = g_dwDRMLevelThisOS - 100;
    if (FAILED(hr = Authenticate(pDevice, &authParam)))
    {
        XLOG(XFAIL, eError, _T("\nFAIL: Authentication (%d) failed"), g_dwDRMLevelThisOS - 100);
        dwResult = FNS_FAIL;
    }
    else
    {
        XLOG(XMSG, eInfo1, _T("\nAuthentication (%d) succeeded"), g_dwDRMLevelThisOS - 100);
    }

    authParam.MinDRMLevel = 0;
    if (FAILED(hr = Authenticate(pDevice, &authParam)))
    {
        XLOG(XFAIL, eError, _T("\nFAIL: Authentication (0) failed"));
        dwResult = FNS_FAIL;
    }
    else
    {
        XLOG(XMSG, eInfo1, _T("\nAuthentication (%d) succeeded"), 0);
    }

    authParam.MinDRMLevel = g_dwDRMLevelThisOS / 2;
    if (FAILED(hr = Authenticate(pDevice, &authParam)))
    {
        XLOG(XFAIL, eError, _T("\nAuthentication (%d) failed"), g_dwDRMLevelThisOS / 2);
        dwResult = FNS_FAIL;
    }
    else
    {
        XLOG(XMSG, eInfo1, _T("\nAuthentication (%d) succeeded"), g_dwDRMLevelThisOS / 2);
    }

#ifndef WHQL
    authParam.MinDRMLevel = (DWORD) -1;
    if (SUCCEEDED(hr = Authenticate(pDevice, &authParam)))
    {
        XLOG(XFAIL, eError, _T("\nFAIL: Authentication with MinDrmLevel (DWORD) -1 succeeded; wasn't supposed to"));
        dwResult = FNS_FAIL;
    }
    else
    {
        XLOG(XMSG, eInfo1, _T("\nAuthentication (%d) failed; this is correct behavior"), (DWORD)-1);
    }
#endif //WHQL
    return dwResult;
} // Test_Authentication_MinDrmLevel

//--------------------------------------------------------------------------;
//
//  Test_Authentication_CertList
//
//  Description:
//      Call authenticate with everything set to TRUE. Generate a list
//      of files that require Drm attribure.
//
//  Arguments:
//      
//  Return:
//      (DWORD)
//
//--------------------------------------------------------------------------;
DWORD 
Test_Authentication_CertList()
{
    DWORD           dwResult = FNS_PASS;
#ifdef WHQL
    AUTHPARAM       authParam;
    PCAudioDevice   pDevice = g_pDRMTest->m_pDeviceUnderTest;

    if (g_pDRMTest->m_listRender.IsEmpty())
    {
        XLOG(XSKIP, eInfo2, _T("Skipping because there is no rendering device when running the device level test"));
        return FNS_SKIPPED;
    }

    if (pDevice->GetAllowedDRMLevel() == (ULONG)-1)
    {
        XLOG(XFAIL, eFatalError, _T("FAIL: Certification list cannot be determined if")
                    _T(" test case 2 has not been run.  Please put case 2")
                    _T(" in the profile preceeding this test case"));
        return FNS_FAIL;
    }

    ZeroMemory(&authParam, sizeof(authParam));
    authParam.MaxFailures = MAX_FAILURES;
    authParam.DriverFileCache = g_pDRMTest->m_DefaultAuthenticate.DriverFileCache;
    authParam.RequireSignedCatalog = TRUE;

    authParam.AllowTestCertificate  = (g_pDRMTest->m_fDriversRealSigned ? FALSE : TRUE);
    authParam.CheckDriverImages = TRUE;
    authParam.MinDRMLevel = g_dwDRMLevelThisOS;
    authParam.ParseCatalogFile = TRUE;

    // If the device implemented the Drm interface.
    if (pDevice->IsDRMCompliant() && FAILED(Authenticate(pDevice, &authParam)))
    {
        // Generate the attribute list for WHQL tools.
        // Format:
        //   Start DRM Files
        //      <driver name>.sys:DRMLevel:g_dwDRMLevelThisOS
        //   End DRM Files
        XLOG(XMSG, eInfo2, _T("\n================================================"));
        XLOG(XMSG, eInfo2, _T("%d Files Rejected by Drm Authentication"), authParam.NumFailures);

        if (authParam.NumFailures)
        {
            BOOL  fNeedAttribute;
            CString szName;
    
            XLOG(XMSG, eInfo2, _T("Start DRM Files"));
            for (UINT i = 0; i < authParam.NumFailures; i++)
            {
                fNeedAttribute = TRUE;
                szName.Empty();
                szName = authParam.DriverFailures[i].path;
                szName = szName.Mid(szName.ReverseFind(_T('\\'))+1);
                if (!szName.IsEmpty())
                {
                    // check if the driver name matches one of the known
                    // file names.
                    // If authentication fails on one of the millennium
                    // audio components, than bail out.
                    for (UINT j = 0; j < ARRAY_ELEMENTS(g_szKnownSystemFiles); j++)
                    {
                        if (0 == _tcsicmp(g_szKnownSystemFiles[j], szName))
                        {
                            dwResult = FNS_FAIL;
                            fNeedAttribute = FALSE;
                        }
                    }
                    
                    if (fNeedAttribute)
                    {
                        // This portion generates the log that
                        // will be used by HCT tools to generate the DRM cat
                        // automatically. 
                        // The level is set to 1100 as of 05/18/2000. 
                        // The level may change depending on Drm policy.

                        // 2001-- The level in Windows XP is 1100 if driver uses , else 1200
                        // This is determined in case 2
                        XLOG(XMSG, eInfo2, _T("%s:DRMLevel:%d"), (LPCTSTR)szName, pDevice->GetAllowedDRMLevel());
                    }
                }
            }
            XLOG(XMSG, eInfo2, _T("End DRM Files"));

            if (!PASSED(dwResult))
            {
                XLOG(XMSG, eError, _T("Failure condition:  One of the OS")
                             _T("(Millennium, XP, Vista) Core Audio components")
                             _T(" is not signed properly"));
            }
        }

        XLOG(XMSG, eInfo2, _T("================================================\n"));
    }
#endif //WHQL

    return dwResult;
} // Test_Authentication_CertList

//--------------------------------------------------------------------------;
//
//  Test_Authentication_FindAllDrivers
//
//  Description:
//        Call authenticate with a very high drmlevel. Compare the list of
//        returned drivers to the actual list of drivers that needs to be
//        signed.
//
//  Arguments:
//      
//  Return:
//      (DWORD)
//
//--------------------------------------------------------------------------;
DWORD
Test_Authentication_FindAllDrivers()
{
    DWORD           dwResult = FNS_PASS;
    AUTHPARAM       authParam;
    PCAudioDevice   pDevice = g_pDRMTest->m_pDeviceUnderTest;

    if (g_pDRMTest->m_listRender.IsEmpty())
    {
        XLOG(XSKIP, eInfo2, _T("Skipping because there is no rendering device when running the device level test"));
        return FNS_SKIPPED;
    }

    ZeroMemory(&authParam, sizeof(authParam));
    authParam.MaxFailures = MAX_FAILURES;
    authParam.DriverFileCache = g_pDRMTest->m_DefaultAuthenticate.DriverFileCache;
    authParam.RequireSignedCatalog = TRUE;

#ifdef WHQL
    authParam.AllowTestCertificate  = FALSE;
#else //WHQL
    authParam.AllowTestCertificate  = (g_pDRMTest->m_fDriversRealSigned ? FALSE : TRUE);
#endif //WHQL
    authParam.CheckDriverImages = TRUE;
    authParam.MinDRMLevel = 100000;
    authParam.ParseCatalogFile = TRUE;

    // If the device implemented the Drm interface.
    if (pDevice->IsDRMCompliant())
    {
        // Check if all the known system files have been found depending
        // on device type.
        LPTSTR* szDriverList = NULL;
        UINT    listCount = 0;
        UINT    cFailuresExpected;
        LPTSTR  szDriverName;
        BOOL    fFound;

        // Note that this should always fail because none of the 
        // drivers are signed with 100000 level.
        Authenticate(pDevice, &authParam);

        //
        // determine the expected array of failed drivers generated by the previous Authenticate call
        //
        if (pDevice->IsUsb())
        {
            switch(g_OS)
            {
                case eOSWindows8:
                    listCount = ARRAY_ELEMENTS(g_szUsbAudioFilesWin8);
                    szDriverList = g_szUsbAudioFilesWin8;
                    break;
                case eOSWindows7:
                    listCount = ARRAY_ELEMENTS(g_szUsbAudioFilesWin7);
                    szDriverList = g_szUsbAudioFilesWin7;
                    break;
                case eOSWindowsVista:
                    listCount = ARRAY_ELEMENTS(g_szUsbAudioFilesVista);
                    szDriverList = g_szUsbAudioFilesVista;
                    break;
                case eOSWindowsXP:
                    listCount = ARRAY_ELEMENTS(g_szUsbAudioFilesXP);
                    szDriverList = g_szUsbAudioFilesXP;
                    break;
                default:
                    XLOG(XFAIL, eError, _T("FAIL: Unexpected OS enum %u"), g_OS);
                    dwResult = FNS_FAIL;
            }
            cFailuresExpected = listCount;
        }
        else
        {
            switch(g_OS)
            {
                case eOSWindows8:
                    listCount = ARRAY_ELEMENTS(g_szPortClsFilesWin8);
                    szDriverList = g_szPortClsFilesWin8;
                    break;
                case eOSWindows7:
                    listCount = ARRAY_ELEMENTS(g_szPortClsFilesWin7);
                    szDriverList = g_szPortClsFilesWin7;
                    break;
                case eOSWindowsVista:
                    listCount = ARRAY_ELEMENTS(g_szPortClsFilesVista);
                    szDriverList = g_szPortClsFilesVista;
                    break;
                case eOSWindowsXP:
                    listCount = ARRAY_ELEMENTS(g_szPortClsFilesXP);
                    szDriverList = g_szPortClsFilesXP;
                    break;
                default:
                    XLOG(XFAIL, eError, _T("FAIL: Unexpected OS enum %u"), g_OS);
                    dwResult = FNS_FAIL;
            }
            cFailuresExpected = listCount + 1;
        }

        if (FNS_PASS == dwResult)
        {
            // NOTE:  If this is failing because NumFailures = 1, make sure that KRMPRoxy 
            //        was built without EXIT_ON_FIRST_FAILURE defined
            XLOG(XMSG, eInfo2, _T("\nauthParam.NumFailures = %d, expected at least %d"), 
                                authParam.NumFailures, cFailuresExpected);

            // make sure each expected system binary is in the list
            for (UINT i = 0; i < listCount; i++)
            {
                szDriverName = szDriverList[i];
                fFound = (-1 != BinaryInAuthParamsList(szDriverName, &authParam));

                if (!fFound)
                {
                    dwResult = FNS_FAIL;
                    XLOG(XFAIL, eError, _T("\n Failure condition:  %s could not be detected in the ")
                                 _T("list of expected failed drivers"), szDriverName);

                    if (_tcscmp(szDriverName, _T("portcls.sys")) == 0)
                        XLOG(XFAIL, eError, _T("It is likely that the adapter driver reset the dispatch")
                                     _T(" table and therefore must use DrmAddContentHandler to")
                                     _T(" make Portcls.sys visible to the secure audio system."));
                }
            }
        }

        // and make sure the adapter driver is in the list
        if (dwResult == FNS_PASS)
        {
            szDriverName = _tcsrchr(pDevice->m_pFilter->m_szBinary, _T('\\'));
            if (szDriverName++)
            {
                int nDriverIndex = BinaryInAuthParamsList(szDriverName, &authParam);

                if (nDriverIndex == -1) // -1 means not found
                {
                    XLOG(XFAIL, eError, _T("\n Failure condition:  %s could not be detected in the list of expected failed drivers"), szDriverName);
                    dwResult = FNS_FAIL;
                }
                else if (   authParam.DriverFailures[nDriverIndex].type != AuthInadequateDRMLevel
                         && authParam.DriverFailures[nDriverIndex].type != AuthNoDrmAttrib
                         && authParam.DriverFailures[nDriverIndex].type != AuthNoCert)
                {
                    XLOG(XFAIL, eError, _T("Failure condition:  Expected error code should be one ")
                                 _T("of AuthNoDrmAttrib, AuthInadequateDRMLevel or AuthNoCert."));
                    dwResult = FNS_FAIL;
                }
            }
        }

        // if this is a USB device, then...
        if ((dwResult == FNS_PASS) && pDevice->IsUsb())
        {
            NtVersion os(g_OSVersionInfo.dwMajorVersion, g_OSVersionInfo.dwMinorVersion);
            
            // on Windows 8 and up builds, make sure the last item in the list is usbhub.sys
            if (NtVersion(6, 2) <= os)
            {
                int     nUSBHubDriverIndex = 0; 
                int     nUSBHub3DriverIndex = 0;
                
                XLOG(XMSG, eInfo2, _T("Looking for usbhub.sys or usbhub3.sys..."));
                nUSBHubDriverIndex = BinaryInAuthParamsList(_T("usbhub.sys"), 
                                                      &authParam);

                nUSBHub3DriverIndex = BinaryInAuthParamsList(_T("usbhub3.sys"), 
                                                      &authParam);
                                                      
                // usbhub.sys or usbhub3.sys must be in the list, as either the last driver
                // or the next-to-last driver.
                //
                // If there are no portcls-based drivers in the system, it will be the last driver.
                //
                // If the USB audio driver itself is portcls-based, it will likely also be last in the list.
                //
                // If the USB audio driver is not portcls-based, but there are other portcls-based drivers,
                // it will be the next-to-last driver, and the last driver will be portcls.sys.
                if (-1 == nUSBHubDriverIndex && -1 == nUSBHub3DriverIndex) {
                    XLOG(XFAIL, eError, _T("FAIL: Could not detect usbhub(3).sys"));
                    dwResult = FNS_FAIL;
                }
                else if (authParam.NumFailures - 1 == nUSBHubDriverIndex ||
                         authParam.NumFailures - 1 == nUSBHub3DriverIndex)
                {
                    // fine, last in the list
                    XLOG(XMSG, eInfo1, _T("usbhub(3).sys is last in the list as expected"));
                }
                else if (authParam.NumFailures - 2 == nUSBHubDriverIndex ||
                         authParam.NumFailures - 2 == nUSBHub3DriverIndex)
                {
                    BOOL bFound = FALSE;
                    
                    // if it is the next-to-last, the last must be portcls.sys
                    CString szUSBName;
                    szUSBName = authParam.DriverFailures[nUSBHubDriverIndex + 1].path;
                    szUSBName = szUSBName.Mid(szUSBName.ReverseFind(_T('\\')) + 1);
                    
                    if (0 == _tcsicmp(_T("portcls.sys"), szUSBName))
                    {
                        XLOG(XMSG, eInfo1, _T("usbhub.sys is followed only by portcls.sys as expected"));
                        bFound = TRUE;
                    }

                    CString szUSB3Name;
                    szUSB3Name = authParam.DriverFailures[nUSBHub3DriverIndex + 1].path;
                    szUSB3Name = szUSB3Name.Mid(szUSB3Name.ReverseFind(_T('\\')) + 1);

                    if (0 == _tcsicmp(_T("portcls.sys"), szUSB3Name))
                    {
                        XLOG(XMSG, eInfo1, _T("usbhub3.sys is followed only by portcls.sys as expected"));
                        bFound = TRUE;
                    }

                    if (FALSE == bFound)
                    {
                        XLOG(XFAIL, eError, _T("FAIL: usbhub(3).sys should be last, or followed only by portcls.sys"));
                        dwResult = FNS_FAIL;
                    }
                    
                }
                else
                {
                    XLOG(XFAIL, eError, _T("FAIL: Unexpected drivers after usbhub(3).sys"));
                    dwResult = FNS_FAIL;
                }
            }
            else
            {
                // on pre-Windows 8 builds, make sure the host controller is in the list
                int     nDriverIndex = 0;
                BOOL    fHostControllerInList = FALSE;
                UINT    cKnownControllers = ARRAY_ELEMENTS(g_szUsbHostController);
                UINT    nController = 0;

                // One of these drivers must exist on a machine.
                for (nController = 0; nController < cKnownControllers; nController++)
                {
                    XLOG(XMSG, eInfo2, _T("Looking for %s..."), g_szUsbHostController[nController]);
                    nDriverIndex = BinaryInAuthParamsList(g_szUsbHostController[nController], 
                                                          &authParam);
                    fHostControllerInList = (nDriverIndex != -1);
                    if (fHostControllerInList)
                    {
                        XLOG(XMSG, eInfo2, _T("Found %s"), g_szUsbHostController[nController]);
                        if (   authParam.DriverFailures[nDriverIndex].type != AuthInadequateDRMLevel
                            && authParam.DriverFailures[nDriverIndex].type != AuthNoDrmAttrib
                            && authParam.DriverFailures[nDriverIndex].type != AuthNoCert)
                        {
                            XLOG(XFAIL, eError, _T("Failure condition:  Expected error code should be one ")
                                         _T("of AuthNoDrmAttrib, AuthInadequateDRMLevel or AuthNoCert."));
                            dwResult = FNS_FAIL;
                        }
                        break;
                    }
                }

                // NOTE:  If this is failing, make sure that KRMPRoxy was built 
                //        without EXIT_ON_FIRST_FAILURE defined
                if (!fHostControllerInList)
                {
                    XLOG(XFAIL, eError, _T("FAIL: Could not detect USB Host Controller"));
                    dwResult = FNS_FAIL;
                }
            }
        }
        
        // if the portcls driver is LAST in the list,
        // this might be an AVStream driver (not a portcls miniport)
        // and DRMKAuthenticate is just being overly cautious.
        //
        // this is not a bug.
        if (BinaryInAuthParamsList(_T("portcls.sys"), &authParam) == authParam.NumFailures - 1)
        {
            XLOG(XMSG, eInfo2,
                _T("portcls.sys is the last driver in the list. ")
                _T("It is possible that this is an AVStream driver (not a portcls miniport) ")
                _T("and that DRMKAuthenticate is manually adding portcls.sys ")
                _T("to the list; this would not be a bug in the third-party driver."));
        }
    }

    return dwResult;
} // Test_Authentication_FindAllDrivers




// --------------------------------------------------------------------------------------------------
// BinaryContainsBytes
// --------------------------------------------------------------------------------------------------
BOOL BinaryContainsBytes(LPCTSTR  strBinary, BYTE* pbLookFor, UINT cbLookFor) 
{
    BOOL    fFound = FALSE;
    HANDLE  hFile = NULL;
    ULONG   cbFile = 0, cbFileRead = 0;
    BYTE   *pbFileBuffer = NULL, 
           *pbFileEnd = NULL, 
           *pbFileTemp = NULL;

    try
    {
        hFile = CreateFile( strBinary, GENERIC_READ, 0, NULL,
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (!hFile || (INVALID_HANDLE_VALUE == hFile))         
        {   
            throw(_T("Could not open file\n")); 
        }

        cbFile = GetFileSize(hFile, NULL);
        if ((0 == cbFile) || (INVALID_FILE_SIZE == cbFile))
        {   
            throw(_T("Error retrieving file size\n")); 
        }

        pbFileBuffer = new BYTE[cbFile];
        if (!pbFileBuffer)  { throw(_T("Out of Memory!\n")); }

        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

        // read cbFile bytes from the file into pbFileBuffer
        if (!ReadFile(hFile, pbFileBuffer, cbFile, &cbFileRead, NULL))
        {   throw(_T("Error reading binary image\n"));  }
        if( (0 == cbFileRead) || (cbFileRead > cbFile) )
        {   throw(_T("Error reading binary image\n"));  }
        
        // The end of the file buffer that can be safely compared to
        // the LookFor buffer would be hit when we reach the size of
        // the LookFor buffer plus one.
        pbFileEnd = (pbFileBuffer + cbFileRead - 1) - (cbLookFor - 1) + 1;
        for(pbFileTemp = pbFileBuffer; pbFileTemp < pbFileEnd; ++pbFileTemp)
        {
            if(0 == memcmp(pbFileTemp, pbLookFor, cbLookFor))
            {
                fFound = TRUE;
                break;
            }
        }
    }
    catch (LPCTSTR str)
    {
        XLOG(XMSG, eError, _T("Internal error in BinaryContainsGUID: %s"), str);
    }

    if (hFile)
    {
        CloseHandle(hFile);
    }

    if (pbFileBuffer)
    {
        delete[] pbFileBuffer;
    }

    return fFound;
}

// ------------------------------------------------------------------------------
//
//  Test_DrmForwardContentToFileObject
//
//  Description:
//      For each binary used by this device, look for DrmForwardContentToFileObject
//      in the imports. This is considered an unsafe function
// ------------------------------------------------------------------------------
DWORD
Test_DrmForwardContentToFileObject()
{
    DWORD           dwRes = FNS_PASS;
    BOOL            fContainsExport = FALSE;
    int             i = 0;
    PCAudioDevice   pDevice = g_pDRMTest->m_pDeviceUnderTest;
    LPTSTR          strBinary = pDevice->m_pFilter->m_szBinary;
    BYTE           *pbImports[] = { (BYTE*)"PcForwardContentToFileObject", 
                                    (BYTE*)"DRMForwardContentToFileObject",
                                    (BYTE*)L"PcForwardContentToFileObject", 
                                    (BYTE*)L"DRMForwardContentToFileObject"};
    // Remove the NULL terminator from the size of the strings
    BYTE            cbImports[] = { sizeof("PcForwardContentToFileObject")   - sizeof(CHAR), 
                                    sizeof("DRMForwardContentToFileObject")  - sizeof(CHAR),
                                    sizeof(L"PcForwardContentToFileObject")  - sizeof(WCHAR), 
                                    sizeof(L"DRMForwardContentToFileObject") - sizeof(WCHAR)};
    LPCTSTR         strImports[] = { _T("PcForwardContentToFileObject"), _T("DRMForwardContentToFileObject") };

    if (g_pDRMTest->m_listRender.IsEmpty())
    {
        XLOG(XSKIP, eInfo2, _T("Skipping because there is no rendering device when running the device level test"));
        return FNS_SKIPPED;
    }

    for (i = 0; i < 4; i++)
    {
        XLOG(XMSG, eInfo2, _T("Searching binary %s for import %s"), strBinary, strImports[i%2]);
        fContainsExport = BinaryContainsBytes(strBinary, pbImports[i], cbImports[i]);
        if(fContainsExport) { break; }
    }

    if (fContainsExport)
    {
        XLOG(XFAIL, eError, _T("FAIL: The binary for this adapter driver %s imports the function %s."), strBinary, strImports[i%2]);
        XLOG(XFAIL, eError, _T("The driver is using deprecated functions and can not be granted a DRM Level of 1200. ")
                     _T("To get a higher security level the driver should be altered to use the ")
                     _T("use the more secure ForwardContentToDeviceObject rather than ForwardContentToFileObject."));

        dwRes = FNS_FAIL;
    }
    else
    {
        XLOG(XMSG, eInfo2, _T("Import not found.  This driver will be given the maximum DRMLevel for this Operating system (%d)"), g_dwDRMLevelThisOS);
        pDevice->SetAllowedDRMLevel(g_dwDRMLevelThisOS);
    }

    return dwRes;
}

// ------------------------------------------------------------------------------
//
//  Test_CheckDisableProtectedAudioDG
//
//  Description:
//      Check registry to verify that DisableProtectedAudioDG is set to 0 or not present
// ------------------------------------------------------------------------------

// Registry key location and value name
#define REGKEY_AUDIOSERVICE_PARENTKEY L"Software\\Microsoft\\Windows\\CurrentVersion"
#define REGKEY_AUDIOSERVICE_SUBKEY L"Audio"
#define REGKEY_AUDIOSERVICE REGKEY_AUDIOSERVICE_PARENTKEY L"\\" REGKEY_AUDIOSERVICE_SUBKEY
#define REGVALUE_DISABLE_PROTECTED_AUDIODG L"DisableProtectedAudioDG"

DWORD 
Test_CheckDisableProtectedAudioDG()
{
    DWORD dwRes = FNS_PASS;
    DWORD dwSuccess = ERROR_SUCCESS;
    DWORD dwValue, cbData = sizeof(dwValue);
    
    if (g_pDRMTest->m_listRender.IsEmpty())
    {
        XLOG(XSKIP, eInfo2, _T("Skipping because there is no rendering device when running the device level test"));
        return FNS_SKIPPED;
    }

    //read the ProtectedAudioDG Registry key
    dwSuccess = RegGetValueW(HKEY_LOCAL_MACHINE, REGKEY_AUDIOSERVICE, REGVALUE_DISABLE_PROTECTED_AUDIODG, RRF_RT_DWORD, NULL, &dwValue, &cbData);

    if (ERROR_FILE_NOT_FOUND == dwSuccess)
    {
        //it is best if the registry key is not present at all
        XLOG(XMSG, eInfo2, _T("DisableProtectedAudioDG Registry key not present on system."));
        return FNS_PASS;
    }
    else if(ERROR_SUCCESS != dwSuccess)
    {
        XLOG(XFAIL, eError, _T("Failed Querying DisableProtectedAudioDG registry key (%d)"), dwSuccess);
        return FNS_FAIL;
    }

    //now that we know the key is present and that we have read the value, we need to see if the setting is disabled.
    if (0 == dwValue)
    {
        //this is also ok, but it would be better if the key werent there at all
        XLOG(XMSG, eInfo2, _T("DisableProtectedAudioDG registry key is set to 0. "));
        XLOG(XMSG, eWarn1, _T("Recommend removing DisableProtectedAudioDG registry key from system all together."));
        return FNS_PASS;
    }
    else
    {
        //this is the bad one. fail here
        XLOG(XFAIL, eError, _T("DisableProtectedAudioDG registry key is set to %d."), dwValue);
        XLOG(XFAIL, eError, _T("DisableProtectedAudioDG registry key should be not present or set to 0 on production systems."));
        return FNS_FAIL;
    }

    return dwRes;
}

// ------------------------------------------------------------------------------
//
//  Test_NoTestSignedDrivers
//
//  Description:
//      Verify that no TestSigned drivers are present on the system.
// ------------------------------------------------------------------------------
DWORD
Test_NoTestSignedDrivers()
{
    DWORD dwRes = FNS_PASS;

    if (g_pDRMTest->m_listRender.IsEmpty())
    {
        XLOG(XSKIP, eInfo2, _T("Skipping because there is no rendering device when running the device level test"));
        return FNS_SKIPPED;
    }

    //check for system submission
    if (TRUE == g_pDRMTest->m_bSystemTest)
    {
        //system kit
        DWORD dwRes = FNS_PASS;

        if( !IsDRMServiceRunning())
        {
            XLOG(XSKIP, eInfo2, _T("Skipping because the DRMK service is not running, indicating this is a SKU that lacks DRM support."));
            dwRes = FNS_SKIPPED;
        }
        else
        {
            dwRes = Test_Authentication_Authenticate();
            if (FNS_FAIL == dwRes)
            {
                XLOG(XFAIL, eInfo2, _T("Failed Authenticating Driver list against Non-TestSigned and DRMLevel = %d"), g_dwDRMLevelThisOS);
            }
        }
    }
    else
    {
        //device kit, skip
        XLOG(XSKIP, eInfo2, _T("Device Kit submission, skipping since device kit can have test signed drivers"));
        dwRes = FNS_SKIPPED;
    }

    return dwRes;

}
