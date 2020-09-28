/////////////////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) Microsoft Corporation
//
//	Module Name:
//	   DXGIHybridPresent.cpp
//
//	Abstract:
//	This file contains implementation of DXGIHybridPresent tests
//
//	History:
//	11/14/2012	MarMel	Created.
//
//////////////////////////////////////////////////////////////////////////////////////////

#include "DXGIHybridPresent.h"
#include "TestApp.h"
#include "Direct3DFailureCategoryStrings.h"

#pragma warning(disable: 4355)

extern bool g_bSkipIfNotHybrid;

CHybridPresentRedirection::CHybridPresentRedirection()
:    m_bHybridMode(g_HybridMode),
     m_bIsWindowed(true),
     m_bFullScreenWindow(false)
{
    m_bStretchBB = false;
    m_MaxBufferCount = 3;
    m_BufferCount = 2;
    g_pTestWindow = &m_hwndDeviceWindow;

    // Handle high-dpi setting so we can successfully use GetFrontBuffer
    SetProcessDPIAware();
};


CHybridPresentRedirection::~CHybridPresentRedirection()
{
}

bool CHybridPresentRedirection::CheckSystemAdapters( bool &bIsHybridSystem, bool &bFoundBDAorBDDAdapter )
{
    // Local variables
    IDXGIFactory1 *pFactory1 = nullptr;
    IDXGIAdapter1 *pAdapter1 = nullptr;
    D3DKMT_DRIVERVERSION wddmDriverVersion = KMT_DRIVERVERSION_WDDM_1_0;
    DevX::GRFX::AdapterDescriptor *pAdapterDescriptor = nullptr;
    HRESULT hr = E_FAIL;
    UINT adapterIndex = 0; 
    bool bSuccess = false;
    bool bFoundHybridDiscrete = false;
    bool bFoundHybridIntegrated = false;
    
    //
    // Initalize state.
    //

    bIsHybridSystem = false;
    bFoundBDAorBDDAdapter = false;

    //
    // Enumerate all adapters.
    //

    if( FAILED( hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&pFactory1) ) ) )
    {
        LogError( __FILEW__, __LINE__, L"CHybridPresentRedirection::CheckSystemAdapters CreateDXGIFactory1 failed", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::DXGI::gFCCreateFactory );
        goto Cleanup;
    }

    while( pFactory1->EnumAdapters1(adapterIndex, &pAdapter1) != DXGI_ERROR_NOT_FOUND ) 
    { 
        // Get the driver model version
        assert( nullptr == pAdapterDescriptor );
        pAdapterDescriptor = DevX::GRFX::AdapterDescriptor::GetAdapterDescriptor( pAdapter1 );
        if( nullptr == pAdapterDescriptor )
        {
            LogError( __FILEW__, __LINE__, L"CHybridPresentRedirection::CheckSystemAdapters CreateDXGIFactory1 failed", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::gFCRequiredCapabilityNotSupported );
            goto Cleanup;
        }

        if( FAILED( hr = pAdapterDescriptor->GetDisplayDriverModel( wddmDriverVersion ) ) )
        {
            LogError( __FILEW__, __LINE__, L"CHybridPresentRedirection::CheckSystemAdapters GetDisplayDriverModel failed", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::gFCRequiredCapabilityNotSupported );
            goto Cleanup;
        }

        // If the adapter supports WDDM 1.3 or higher, see if it's a Hybrid iGPU, Hybrid dGPU, or neither
        if( wddmDriverVersion >= KMT_DRIVERVERSION_WDDM_1_3 )
        {
            bool bHybridDiscrete = false;
            bool bHybridIntegrated = false;
            bool bBasicDisplay = false;

            // Check if the adapter is a hybrid discrete (dGPU)
            if( FAILED( hr = pAdapterDescriptor->IsHybridDiscrete( bHybridDiscrete ) ) )
            {
                LogError( __FILEW__, __LINE__, L"CHybridPresentRedirection::CheckSystemAdapters IsHybridDiscrete failed", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::gFCRequiredCapabilityNotSupported );
                goto Cleanup;
            }

            // Check if the adapter is a hybrid integrated (iGPU)
            if( FAILED( hr = pAdapterDescriptor->IsHybridIntegrated( bHybridIntegrated ) ) )
            {
                LogError( __FILEW__, __LINE__, L"CHybridPresentRedirection::CheckSystemAdapters IsHybridIntegrated failed", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::gFCRequiredCapabilityNotSupported );
                goto Cleanup;
            }

            // Check if the adapter is BDA or BDD
            if( FAILED( hr = pAdapterDescriptor->IsBDAorBDD( bBasicDisplay ) ) )
            {
                LogError( __FILEW__, __LINE__, L"CHybridPresentRedirection::CheckSystemAdapters IsBDAorBDD failed", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::gFCRequiredCapabilityNotSupported );
                goto Cleanup;
            }

            if( bHybridDiscrete )
            {
                bFoundHybridDiscrete = true;
            }

            if( bHybridIntegrated )
            {
                bFoundHybridIntegrated = true;
            }

            if( bBasicDisplay )
            {
                bFoundBDAorBDDAdapter = true;
            }
        }

        // Cleanup the adapter descriptor
        if( nullptr != pAdapterDescriptor )
        {
            delete pAdapterDescriptor;
            pAdapterDescriptor = nullptr;
        }

        // Cleanup the adapter
        SAFE_RELEASE( pAdapter1 );

        // Increment the adapter index
        ++adapterIndex; 
    } 

    //
    // Determine if we are running on a hybrid system.
    //
    
    bIsHybridSystem = ( bFoundHybridDiscrete && bFoundHybridIntegrated );
    
    // If we got here, we passed.
    bSuccess = true;

Cleanup:
    SAFE_RELEASE( pFactory1 );
    SAFE_RELEASE( pAdapter1 );

    if( nullptr != pAdapterDescriptor )
    {
        delete pAdapterDescriptor;
        pAdapterDescriptor = nullptr;
    }

    return bSuccess;
}

void CHybridPresentRedirection::CleanupETW()
{
    m_ETWMaster.WaitForEvents();

    CSLocker Lock(m_ETWMaster.m_CritSection);
    m_ETWMaster.m_CrossAdapterPresentsCount = 0;
    m_ETWMaster.m_RegularPresentsCount = 0;
    m_ETWMaster.CleanupAllocations();
    m_ETWMaster.m_PresentModel = D3DKMT_PM_UNINITIALIZED;
}

bool CHybridPresentRedirection::IsVistaBLT()
{
    if(m_ETWMaster.m_PresentModel == D3DKMT_PM_REDIRECTED_VISTABLT)
    {
        WriteToLog(_T("CHybridPresentRedirection::IsVistaBLT - VistaBLT presentation model detected"));

        // Check whether VistaBlt is expected. When swapchain is created with GDI-supported formats and we
        // are in windowed BLT presentation model, we shouldn't be seeing Vista BLTs
        if(m_SwapEffect.dxgiSwapEffect == DXGI_SWAP_EFFECT_SEQUENTIAL &&
           m_bIsWindowed == true &&
           (m_ModeInformation.ModeDesc.Format == DXGI_FORMAT_R8G8B8A8_UNORM ||
            m_ModeInformation.ModeDesc.Format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB ||
            m_ModeInformation.ModeDesc.Format == DXGI_FORMAT_B8G8R8A8_UNORM ||
            m_ModeInformation.ModeDesc.Format == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB ||
            m_ModeInformation.ModeDesc.Format == DXGI_FORMAT_B8G8R8X8_UNORM ||
            m_ModeInformation.ModeDesc.Format == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB
            ) &&
            m_bHybridMode)
        {
            LogError(__FILEW__, __LINE__, L"HybridPresentRedirection::IsVistaBLT() - Vista BLT is detected when RedirectedBlt is epxected", false, ERRORCODE_TYPE_BOOL);
        }

        return true;
    }
    else
        return false;
}

TEST_RESULT CHybridPresentRedirection::ExecuteTestCase()
{
    if( g_bSkipVistaBlt &&
        m_SwapEffect.dxgiSwapEffect == DXGI_SWAP_EFFECT_SEQUENTIAL &&
        (m_ModeInformation.ModeDesc.Format != DXGI_FORMAT_R8G8B8A8_UNORM &&
        m_ModeInformation.ModeDesc.Format != DXGI_FORMAT_R8G8B8A8_UNORM_SRGB &&
        m_ModeInformation.ModeDesc.Format != DXGI_FORMAT_B8G8R8A8_UNORM &&
        m_ModeInformation.ModeDesc.Format != DXGI_FORMAT_B8G8R8A8_UNORM_SRGB &&
        m_ModeInformation.ModeDesc.Format != DXGI_FORMAT_B8G8R8X8_UNORM &&
        m_ModeInformation.ModeDesc.Format != DXGI_FORMAT_B8G8R8X8_UNORM_SRGB) 
        )
    {
        m_ETWMaster.LogTestETW(L"TEST: Skipping Vista BLT test case");
        WriteToLog(_T("VistaBLT, skipping"));
        return RESULT_SKIP;
    }

    m_NumberOfPresents = m_BufferCount;

    if(m_StartFullScreen)
        m_NumberOfPresents *= 2;

    m_TexturesToLoad = m_BufferCount;

    m_dwPresentTime = g_TimeToExecute;

    // we always start up as windowed and might change to fullscreen window depending on test case parameters
    m_bIsWindowed = true;

    // Set gApplication.m_DFlip to true for DFlip test cases
    // CPresentWinConvertStretch::ExecuteTestCase() will create DFlip window if this is true
    bool bSetDFlipToTrue = false;
    if(m_tp_bWindowless == false && 
       m_SwapEffect.dxgiSwapEffect == (DXGI_SWAP_EFFECT)DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL && 
       m_bFullScreenWindow == true && 
       g_Application.m_bDFlip == false)
    {
        g_Application.m_bDFlip = true;
        bSetDFlipToTrue = true;
        WriteToLog(_T("CHybridPresentRedirection::ExecuteTestCase() - This test will run in DirectFlip mode"));
    }

    CleanupETW();
    TEST_RESULT tr = CPresentWinConvertStretch::ExecuteTestCase();

    if(bSetDFlipToTrue)
    {
        g_Application.m_bDFlip = false;
        bSetDFlipToTrue = false;
    }

    if(tr != RESULT_PASS)
        return tr;

    // Wait for events to drain
    m_ETWMaster.WaitForEvents();

    TEST_RESULT trCA = ValidateCrossAdapterSurfaces();

    // Validate Presents
    TEST_RESULT trPresent = ValidatePresents();

    if(tr == RESULT_FAIL || trCA != RESULT_PASS || trPresent != RESULT_PASS)
        return RESULT_FAIL;
    else
        return tr;
};

TEST_RESULT CHybridPresentRedirection::ValidatePresents(bool bEnableNoPresentsValidation)
{
    if(g_bNoETWValidation)
        return RESULT_PASS;

    TEST_RESULT tr = RESULT_PASS;
    UINT CrossAdapterPresentsCount = 0;
    UINT RegularPresentsCount = 0;

    {
    CSLocker Lock(m_ETWMaster.m_CritSection);
    CrossAdapterPresentsCount = m_ETWMaster.m_CrossAdapterPresentsCount;
    RegularPresentsCount = m_ETWMaster.m_RegularPresentsCount;
    }

    WriteToLog(_T("CHybridPresentRedirection::ValidatePresents() - detected %d CrossAdapter Presents and "
                  "%d Regular Presents"),
                  CrossAdapterPresentsCount,
                  RegularPresentsCount);

    UINT TotalPresentCount = CrossAdapterPresentsCount + RegularPresentsCount;
    if (TotalPresentCount > 0 || bEnableNoPresentsValidation)
    {
        if(m_bHybridMode)
        {
            if(IsVistaBLT())
            {
                if(CrossAdapterPresentsCount)
                {
                    WriteToLog(_T("CHybridPresentRedirection::ValidatePresents() - ERROR: Don't expect to see optimized presents when using VistaBLT"));
                    tr = RESULT_FAIL;
                }
            }
            else if(!m_bIsWindowed)
            {
                if(CrossAdapterPresentsCount)
                {
                    WriteToLog(_T("CHybridPresentRedirection::ValidatePresents() - ERROR: Don't expect to see optimized presents when running in Fullscreen mode"));
                    tr = RESULT_FAIL;
                }
            }
            else
            {
                if(RegularPresentsCount || !CrossAdapterPresentsCount)
                {
                    WriteToLog(_T("CHybridPresentRedirection::ValidatePresents() - ERROR: We only expect "
                                  "CrossAdapter presents when running in Hybrid mode"));
                    tr = RESULT_FAIL;
                }
            }
        }
        else
        {
            if(!IsVistaBLT())
            {
                if(!RegularPresentsCount || CrossAdapterPresentsCount)
                {
                    WriteToLog(_T("CHybridPresentRedirection::ValidatePresents() - ERROR: We only expect "
                                  "Regular presents when NOT running in Hybrid mode"));
                    tr = RESULT_FAIL;
                }
            }
        }
    }

    m_ETWMaster.ResetPresentCounters();

    return tr;
}

TEST_RESULT CHybridPresentRedirection::ValidateCrossAdapterSurfaces()
{
    if(g_bNoETWValidation)
        return RESULT_PASS;

    TEST_RESULT tr = RESULT_PASS;

    CSLocker Lock(m_ETWMaster.m_CritSection);

    for(AdapterList::iterator it = m_ETWMaster.m_Adapters.begin();
        it != m_ETWMaster.m_Adapters.end();
        it++)
    {
        CAdapter* pAdapter = *it;

        if(pAdapter->IsBDD() && !g_bBDDHybrid)
            continue;

        if(pAdapter->m_CrossAdapterAllocationList.empty())
        {
            bool bCheckForAllocations = true;

            // In BDD Hybrid configs, all devices and all allocations will be created on Discrete adapter
            if(g_bBDDHybrid && !pAdapter->IsDiscrete())
                bCheckForAllocations = false;

            if(bCheckForAllocations)
            {
                if(m_bHybridMode && !IsVistaBLT())
                {
                    WriteToLog(_T("CHybridPresentRedirection::ValidateCrossAdapterSurfaces() - ERROR:Running in HybridMode, but didn't find \
                                  any CrossAdapter surfaces on adapter 0x%p (bDiscrete=%s)"),
                                  pAdapter->m_hAdapter,
                                  (pAdapter->IsDiscrete()) ? _T("TRUE") : _T("FALSE"));
                    tr = RESULT_FAIL;
                }
            }
        }
        else
        {
            UINT Count = pAdapter->m_CrossAdapterAllocationList.size();

            WriteToLog(_T("CHybridPresentRedirection::ValidateCrossAdapterSurfaces() - Found %d \
                            CrossAdapter surfaces on adapter 0x%p (bDiscrete=%s)"),
                            Count,
                            pAdapter->m_hAdapter,
                            (pAdapter->IsDiscrete()) ? _T("TRUE") : _T("FALSE"));

            if(!m_bHybridMode)
            {
                WriteToLog(_T("CHybridPresentRedirection::ValidateCrossAdapterSurfaces() - ERROR:Not expecting CrossAdapter surfaces as we are not running in HybridMode"));
                tr = RESULT_FAIL;
            }
            else
            {
                for(CrossAdapterAllocationList::iterator it = pAdapter->m_CrossAdapterAllocationList.begin();
                    it != pAdapter->m_CrossAdapterAllocationList.end();
                    it++)
                {
                    CCrossAdapterAllocation* pAllocation = *it;

                    // Check if allocation is Aperture-Only
                    if(!pAdapter->IsApertureOnly(pAllocation->m_SupportedSegmentSet))
                    {
                        WriteToLog(_T("CHybridPresentRedirection::ValidateCrossAdapterSurfaces() - ERROR:CrossAdapter allocation 0x%p is not Aperture-Only"));
                        tr = RESULT_FAIL;
                    }

                    // Check that allocation is Shared
                    if(!pAllocation->m_Flags.Shareable)
                    {
                        WriteToLog(_T("CHybridPresentRedirection::ValidateCrossAdapterSurfaces() - ERROR:CrossAdapter allocation 0x%p is not Shared"));
                        tr = RESULT_FAIL;
                    }

                    // Check that allocation is not swizzled
                    if(pAllocation->m_Flags.Swizzled)
                    {
                        WriteToLog(_T("CHybridPresentRedirection::ValidateCrossAdapterSurfaces() - ERROR:CrossAdapter allocation 0x%p is Swizzled"));
                        tr = RESULT_FAIL;
                    }
                }
            }
        }
    }

    return tr;
}

TEST_RESULT CHybridPresentRedirection::Setup()
{
    TEST_RESULT tResult = RESULT_FAIL;
    HRESULT hResult = S_OK;
    bool bHybridSystem = false;
    bool bFoundBDAOrBDDAdapter = false;

    //
    // Check if we are running on a hybrid system or Terminal Service.
    //

    if( !CHybridPresentRedirection::CheckSystemAdapters( bHybridSystem, bFoundBDAOrBDDAdapter ) )
    {
        WriteToLog(_T( "Setup:  CheckSystemAdapters() failed." ));
        tResult = RESULT_FAIL;
        goto Cleanup;
    }

    if( !bHybridSystem)
    {
        if(g_bSkipIfNotHybrid)
        {
            // SKIP when running with "-skipifnothybrid" parameter - we expect this test to be run on non-hybrid configurations
            WriteToLog(_T( "Setup:  CHybridPresentRedirection is being run on a non-hybrid system. Skipping group." ));
            tResult = RESULT_SKIP;
        }
        else
        {
            // FAIL when running without "-skipifnothybrid" parameter is not specified - we don't expect this test to be run on non-hybrid
            // configurations (i.e. in WTT environment (Hybrid support is determined by job constraint) or LOGO environment (Hybrid support is
            // determined by DisplayGatherer)
            WriteToLog(_T( "Setup:  CHybridPresentRedirection is being run on a non-hybrid system. Failing group." ));
            tResult = RESULT_FAIL;
        }
        goto Cleanup;
    }

    if( bFoundBDAOrBDDAdapter )
    {
        g_bBDDHybrid = true;	// WinBlue: 195417
    }

    if(g_Application.IsTerminalServices() == true ||
       GetSystemMetrics( SM_REMOTESESSION ) != 0)
    {
        WriteToLog(_T( "Setup:  CHybridPresentRedirection not run TerminalServices." ));
        tResult = RESULT_SKIP;
        goto Cleanup;
    }
    
    //
    // Call the parent Setup and start ETW tracing.
    //

    tResult = CPresentWinConvertStretch::Setup();
    if(tResult == RESULT_PASS && !g_bNoETWValidation)
    {
        m_ETWMaster.StartListening();
    }

Cleanup:
    return tResult;
};


void CHybridPresentRedirection::Cleanup()
{
    if(!g_bNoETWValidation)
    {
        m_ETWMaster.StopListening();
        m_ETWMaster.Cleanup();
    }

    CPresentWinConvertStretch::Cleanup();
}

void CHybridPresentRedirection::InitTestParametersImpl()
{
    //
    // We'll need to create different test factors for Blt vs. Flip Model.   Flip-Model does not support the 1 buffer swapchain case.
    //

    // Blt-Model Back Buffer Counts
    RFactor rfBltModelBufferCount = AddParameter
        (
        _T("BufferCount"),
        &m_BufferCount,
        RangeValueSet
        (
        (UINT)1,
        (UINT)m_MaxBufferCount,
        (UINT)1
        )
        );

    // Flip-Model Back Buffer Counts
    RFactor rfFlipModelBufferCount = AddParameterValueSet
        (
        _T("BufferCount"),
        RangeValueSet
        (
        (UINT)2,
        (UINT)m_MaxBufferCount,
        (UINT)1
        )
        );

    // Blt-Model Swap Effects
    CUserValueSet< DXGISwapEffectRecord > *pBltModelSwapEffect = new CUserValueSet< DXGISwapEffectRecord >();
    for( UINT i = 0; i < cuiNoOfSwapEffects; i++ )
    {
        if(DXGISwapEffects[i].dxgiSwapEffect == DXGI_SWAP_EFFECT_SEQUENTIAL)
        {
            pBltModelSwapEffect->AddValue(DXGISwapEffects[i]);
        }
    }

    RFactor rfBltModelSwapEffect =  AddParameter< DXGISwapEffectRecord >( _T("SwapEffect") , &m_SwapEffect, pBltModelSwapEffect);


    // Flip-Model Swap Effects
    CUserValueSet< DXGISwapEffectRecord > *pFlipModelSwapEffect = new CUserValueSet< DXGISwapEffectRecord >();
    for( UINT i = 0; i < cuiNoOfSwapEffects; i++ )
    {
        if(DXGISwapEffects[i].dxgiSwapEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL)
        {
            pFlipModelSwapEffect->AddValue(DXGISwapEffects[i]);
        }
    }

    RFactor rfFlipModelSwapEffect =  AddParameterValueSet< DXGISwapEffectRecord >( _T("SwapEffect") , pFlipModelSwapEffect);

    RFactor rfBltModelCombo =  (rfBltModelSwapEffect * rfBltModelBufferCount);
    RFactor rfFlipModelCombo =  (rfFlipModelSwapEffect * rfFlipModelBufferCount);

    //
    // We need separate Test factors for choosing between DCOMP (Windowless) and regular Windowed Presents.
    // DCOMP is only supported for FlipModel when DWM is on.
    //
    // (Note that the name "windowless" is somewhat misleading since the visuals themselves are still contained with-in a Window.)
    //
    // Use DCOMP
    RFactor UseDCOMPFactor = AddParameter< bool >( _T("UseDCOMP") , &m_tp_bWindowless, true);

    // Don't use DCOMP (use regular Windowed Presents)
    RFactor DontUseDCOMPFactor = AddParameterValue< bool >( _T("UseDCOMP"), false);

    // Full screen vs. Windowed.
    // DCOMP can't be in Full screen mode
    RFactor FullScreen = AddParameter<bool>(_T("FullScreen"), &m_StartFullScreen, true);
    RFactor Windowed = AddParameterValue<bool>(_T("FullScreen"), false);

    // Full screen window vs. regular window
    // Use this for Flip only - when we have full screen window we are running in DFlip mode
    RFactor FullScreenWindow = AddParameter<bool>(_T("FullScreenWindow"), &m_bFullScreenWindow, true);
    RFactor RegularWindow = AddParameter<bool>(_T("FullScreenWindow"), &m_bFullScreenWindow, false);

    RFactor DontUseDCOMPBltFactor = (Windowed+FullScreen)*rfBltModelCombo*DontUseDCOMPFactor;
    RFactor DontUseDCOMPFlipFactor = (DontUseDCOMPFactor*Windowed*(FullScreenWindow+RegularWindow)*rfFlipModelCombo) + (DontUseDCOMPFactor*FullScreen*rfFlipModelCombo);

    // DWM ON - DCOMP Supported, but only for FlipModel.  for Regular Windowed Presents test both blt and Flip model.
    RFactor Bpp32_DwmOn = m_FeatureLevelFactor  * m_globalRFactorDwmOnBpp * (DontUseDCOMPBltFactor + DontUseDCOMPFlipFactor + (UseDCOMPFactor*rfFlipModelCombo*Windowed));

    SetRootTestFactor( Bpp32_DwmOn );

    DeclarePriorityLevelByPriorityValue(1, 5.0f);
    DeclarePriorityLevelByPriorityValue(2, 1.0f);
}



HRESULT CHybridPresentRedirection::TransitionBeforePresent()
{
    HRESULT hr = S_OK;
    IDXGIDevice *pDXGIDevice = NULL;
    IDXGIAdapter *pAdapter = NULL;
    IDXGIOutput* pDXGIOutput = NULL;
    DXGI_MODE_DESC  matchDesc = { 0 };
    DXGI_MODE_DESC  outDesc = { 0 };

    if(!m_StartFullScreen)
        return S_OK;

    DBGPrint(L"TransitionBeforePresent() - Enter");

    if(!m_adapterNoOutputs) // does our Adapter have outputs?  If not, we can't switch to fullscreen mode.
    {
        // We are about to switch to Full Screen mode where we don't expect any CrossAdapter Presents. Reset our counters so that we don't count previous CrossAdapter Presents
        m_ETWMaster.WaitForEvents();
        m_ETWMaster.ResetPresentCounters();

        hr = m_pSwapChain->GetDevice(IID_PPV_ARGS(&pDXGIDevice));
        if(FAILED(hr))
        {
            LogError( __FILEW__, __LINE__, L"CHybridPresentRedirection::TransitionBeforePresent() - ERROR: Unable to get output for fullscreen transition", hr, ERRORCODE_TYPE_HRESULT);
            goto Cleanup;
        }
        hr = pDXGIDevice->GetParent(IID_PPV_ARGS(&pAdapter));
        if(FAILED(hr))
        {
            LogError( __FILEW__, __LINE__, L"CHybridPresentRedirection::TransitionBeforePresent() - ERROR: Unable to get output for fullscreen transition", hr, ERRORCODE_TYPE_HRESULT);
            goto Cleanup;
        }
        hr = pAdapter->EnumOutputs(0, &pDXGIOutput);
        if(FAILED(hr))
        {
            LogError( __FILEW__, __LINE__, L"CHybridPresentRedirection::TransitionBeforePresent() - ERROR: Unable to get output for fullscreen transition", hr, ERRORCODE_TYPE_HRESULT);
            goto Cleanup;
        }

        hr = m_pSwapChain->SetFullscreenState( TRUE, pDXGIOutput );
        if(FAILED(hr))
        {
            if( !(m_bExpectDeviceLoss && IsDeviceLost(hr)))
            {
                LogError( __FILEW__, __LINE__, L"CHybridPresentRedirection::TransitionBeforePresent() - ERROR: SetFullscreenState failed", hr, ERRORCODE_TYPE_HRESULT);
            }

            goto Cleanup;
        }

        m_bIsWindowed = false;

        hr = m_pSwapChain->ResizeBuffers(m_BufferCount,
                                         GetSystemMetrics(SM_CXSCREEN),
                                         GetSystemMetrics(SM_CYSCREEN),
                                         m_ModeInformation.ModeDesc.Format,
                                         m_SCDesc.Flags | DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
        if(FAILED(hr))
        {
            if( !(m_bExpectDeviceLoss && IsDeviceLost(hr)))
            {
                LogError(__FILEW__, __LINE__, L"CHybridPresentRedirection::TransitionBeforePresent() - ERROR: ResizeBuffers failed", hr, ERRORCODE_TYPE_HRESULT);
            }
            goto Cleanup;
        }
    }
    else
    {
        WriteToLog(_T("Fullscreen on adapter with no outputs not implemented in CHybridPresentRedirection"));
        hr = E_NOTIMPL;
    }

Cleanup:
    SAFE_RELEASE(pDXGIOutput);
    SAFE_RELEASE(pAdapter);
    SAFE_RELEASE(pDXGIDevice);

    DBGPrint(L"TransitionBeforePresent() - Exit");

    return hr;
}

HRESULT CHybridPresentRedirection::TransitionAfterPresent()
{
    HRESULT hr = S_OK;

    if(!m_StartFullScreen)
        return S_OK;

    DBGPrint(L"TransitionAfterPresent() - Enter");

    //
    // Test that buffers are not disturbed after transitioning from fullscreen.
    //
    BOOL bDWMState = FALSE;
    DwmIsCompositionEnabled( &bDWMState );

    // Check Presents before we switch back to Windowed mode. While in Full Screen mode we don't expect any CrossAdapter Presents
    m_ETWMaster.WaitForEvents();
    TEST_RESULT tr = ValidatePresents(false);

    hr = m_pSwapChain->SetFullscreenState( FALSE, NULL );
    if ( FAILED( hr ) )
    {
        if( !(m_bExpectDeviceLoss && IsDeviceLost(hr)))
        {
            LogError(__FILEW__, __LINE__, L"CFSWinTransitionSequential::TransitionAfterPresent() - ERROR: SetFullscreenState failed", hr, ERRORCODE_TYPE_HRESULT);
        }
        return hr;
    };

    if(SUCCEEDED(hr))
    {
        m_bIsWindowed = true;

        Sleep(100);
        ProcessMessages();
        Sleep(bDWMState ? 2500 : 1500);
        ProcessMessages();
    }

    //
    // Need to ResizeBuffers after transitioning from Full Screen. Get the current desc and reapply it.
    //
    DXGI_SWAP_CHAIN_DESC scd;
    m_pSwapChain->GetDesc(&scd);

    //
    // Call ResizeBuffers with the current values.
    //
    hr = m_pSwapChain->ResizeBuffers
        (
        scd.BufferCount,
        scd.BufferDesc.Width,
        scd.BufferDesc.Height,
        scd.BufferDesc.Format,
        scd.Flags | DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
        );
    if ( FAILED( hr ) )
    {
        if( !(m_bExpectDeviceLoss && IsDeviceLost(hr)))
        {
            LogError(__FILEW__, __LINE__, L"CHybridPresentRedirection::TransitionAfterPresent() - ERROR: ResizeBuffers failed with error 0x%x.", hr, ERRORCODE_TYPE_HRESULT);
        }

        return hr;
    };

    // Reinitialize buffer content after ResizeBuffers
    // TransitionAfterPresent is only on the first Present loop. ie: always start on the texture after texture 0.
    for
        (
        UINT uiCount = 1;
    uiCount <= m_BufferCount;
    ++uiCount
        )
    {
        HRESULT hrInitializeBackBufferResult = InitializeBackBuffer(
            &TestedSwapChain,
            ( uiCount ) % m_TexturesToLoad );

        if ( FAILED( hrInitializeBackBufferResult ) )
        {
            if( !(m_bExpectDeviceLoss && IsDeviceLost(hr)))
            {
                WriteToLog(_T("CHybridPresentRedirection::TransitionAfterPresent: InitializeBackBuffer failed."));
                if( hrInitializeBackBufferResult == E_OUTOFMEMORY && m_bLargeMemoryUsage)
                        WriteToLog( _T("CHybridPresentRedirection::TransitionAfterPresent: Test case using large amount of memory. InitializeBackBuffer failure expected. " ));
            }

            return hrInitializeBackBufferResult;
        };


        if ( !PerformPresent( m_pSwapChain, 0, 0 ) )
        {
            if( !(m_bExpectDeviceLoss && IsDeviceLost(hr)))
            {
                WriteToLog(_T("CHybridPresentRedirection::TransitionAfterPresent: PerformPresent on swap chain failed."));
                return E_FAIL;
            }
            else
                return DXGI_ERROR_DEVICE_RESET;
        }

    }

    if(tr == RESULT_FAIL)
        hr = E_FAIL;

    DBGPrint(L"TransitionAfterPresent() - Exit");

    return hr;
}


