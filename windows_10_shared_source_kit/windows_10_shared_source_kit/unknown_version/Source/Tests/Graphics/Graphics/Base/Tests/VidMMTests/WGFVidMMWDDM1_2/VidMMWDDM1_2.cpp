/******************************Module*Header*******************************
* Module Name: VidMMWDDM1_2.cpp
*
* D3D-based tests that verifies UMD WDDM1.2 VidMM requirements
*
* Author: Marina Meleshenko [marmel]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/
#include "VidMMWDDM1_2.h"
#include "locker.h"
#include "AdapterUtil.h"

CVidMMUMDTestApp g_App;

bool CVidMMUMDTestApp::InitTestGroups()
{	
    if(m_D3DOptions.SrcDriverType == D3D_DRIVER_TYPE_REFERENCE)
    {
        WriteError(_T("This test doesn't support running on REF"));
        Sleep(5000);
        return false;
    }

    // We have a separate job in WHQL for FL9, so we want to limit this one to 10 and up (if "-whql" switch is specified)
    if( strstr( GetFramework()->GetCommandLine()->GetArgumentString(), _T("-whql")) )
        m_D3DOptions.wMinFeatureLevel = D3D_FEATURE_LEVEL_10_0;

    if( strstr( GetFramework()->GetCommandLine()->GetArgumentString(), _T("-childBasic")) )
    {
        if( !ADD_TEST( "BasicChild", CWGFVidMMResourceChild ) ) return false;
    }
    else if(strstr( GetFramework()->GetCommandLine()->GetArgumentString(), _T("-childCreateDestroyReuse")) )
    {
        if( !ADD_TEST( "CreateDestroyReuseChild", COfferReclaimCreateDestroyReuseChild ) ) return false;
    }
    else
    {
        // WLK UMD Logging requirements
        if( !ADD_TEST( _T("UMDLogging\\UMDLoggingInternalSurface"),		CUMDLoggingInternalSurfaceTest ) ) return false;
        if( !ADD_TEST( _T("UMDLogging\\UMDLoggingPackedSurface"),		CUMDLoggingPackedSurfaceTest ) ) return false;
        if( !ADD_TEST( _T("UMDLogging\\UMDLoggingRenamedSurface"),		CUMDLoggingRenamedSurfaceTest ) ) return false;
        if( !ADD_TEST( _T("UMDLogging\\UMDLoggingCreateDestroyTest"),		CUMDLoggingCreateDestroyTest ) ) return false;

        // WLK Offer/Reclaim requirements
        if( !ADD_TEST( _T("OfferReclaim\\OfferReclaimStagingSurface"),		COfferReclaimStagingSurfaceTest ) ) return false;
        if( !ADD_TEST( _T("OfferReclaim\\OfferReclaimPackedSurface"),		COfferReclaimPackedSurfaceTest ) ) return false;
        if( !ADD_TEST( _T("OfferReclaim\\OfferReclaimRenamedSurface"),		COfferReclaimRenamedSurfaceTest ) ) return false;
        if( !ADD_TEST( _T("OfferReclaim\\OfferReclaimCreateDestroy"),		COfferReclaimCreateDestroyTest ) ) return false;
        if( !ADD_TEST( _T("OfferReclaim\\OfferReclaimDeferredDeletions"),	COfferReclaimDeferredDeletionsTest ) ) return false;
        if( !ADD_TEST( _T("OfferReclaim\\OfferReclaimPending"),		COfferReclaimPendingTest ) ) return false;

        // WLK Software Cache Coherency requirements
        if( !ADD_TEST( _T("Coherency\\CoherencyBasic"),		CCoherencyTest ) ) return false;
    }

    return true;
}


void CVidMMUMDTestApp::InitOptionVariables()
{
    CD3D11TestFramework::InitOptionVariables();

    UnRegisterOptionVariable( _T("SrcOnly") );
    m_D3DOptions.SrcOnly = true;
}




CVidMMUMDTest::CVidMMUMDTest()
: m_bVerifyUMDAllocInfo(false)
{
    OutputDebugString("CVidMMUMDTest()\n");
}

bool CVidMMUMDTest::LogListenerErrors()
{
    bool bErrorsLogged = false;

    if(m_pETWMaster)
    {
        std::string strError = "";
        
        while(m_pETWMaster->GetListenerError(strError))
        {
            strError += "\n";
            WriteToLog(_T(strError.c_str()));
            bErrorsLogged = true;
        }
    }

    return bErrorsLogged;
}


eRESIDENCY CVidMMUMDTest::GetAllocationResidency(ID3D11Texture2D* pTex)
{
    assert(m_pETWMaster);
    eRESIDENCY retRes = RESIDENCY_NOT_RESIDENT;

    m_pETWMaster->WaitForEvents(5000);

    sALLOC_INFO* pAllocInfo = m_pETWMaster->GetAllocInfo(pTex);

    if(pAllocInfo)
    {
        if(pAllocInfo->SegmentID)
        {
            sSEGMENT_INFO* pSegment = m_pETWMaster->SysInfo()->GetSegment(pAllocInfo->SegmentID);

            assert(pSegment);

            if(pSegment->Flags.Aperture)
            {
                retRes = RESIDENCY_RESIDENT_APERTURE;
            }
            else
            {
                retRes = RESIDENCY_RESIDENT_VIDEO;
            }
        }
    }

    return retRes;
}

eALLOCATION_OFFER_STATE CVidMMUMDTest::GetOfferState(HANDLE hAllocHandle)
{
    assert(m_pETWMaster);

    sALLOC_INFO* pAllocInfo = m_pETWMaster->GetAllocInfo(hAllocHandle);

    assert(pAllocInfo);

    if(pAllocInfo)
        return pAllocInfo->OfferState;
    else
        return OFFER_STATE_NOT_OFFERED;
}

eALLOCATION_OFFER_PRIORITY CVidMMUMDTest::GetAllocationOfferPriority(HANDLE hAllocHandle)
{
    assert(m_pETWMaster);

    sALLOC_INFO* pAllocInfo = m_pETWMaster->GetAllocInfo(hAllocHandle);

    assert(pAllocInfo);

    if(pAllocInfo)
        return pAllocInfo->OfferPriority;
    else
        return OFFER_PRIORITY_NONE;
}

/////////////////////////////////////////////////////////////////////////////

CInternalSurfaceTest::CInternalSurfaceTest()
: m_pVidMemTex(NULL),
  m_pStagingTex(NULL)
{
    OutputDebugString("CInternalSurfaceTest()\n");
}

void CInternalSurfaceTest::InitTestParameters()
{
    CVidMMUMDTest::InitTestParameters();
    
    // For this test we need large texture. Copying data from system memory
    // to Video Memory for large texture might result in driver creating new 
    // (or reclaiming existing) staging surfaces. These dimensions should result 
    // in 16 MB surface
    m_Width = 2048;
    m_Height = 2048;

    // For this test, we'll need 2 surfaces, source in system memory and destination 
    // in video memory
    m_Count = 2;
}

TEST_RESULT CInternalSurfaceTest::SetupTestCase()
{
    TEST_RESULT tr = CVidMMUMDTest::SetupTestCase();

    if(tr != RESULT_PASS)
    {
        CVidMMUMDTest::CleanupTestCase();
        return tr;
    }

    HRESULT hr = S_OK;
    const UINT DestIndex = 0;
    const UINT SrcIndex = 1;

    hr = Create2DTexture(DestIndex, D3D11_USAGE_DEFAULT);

    if(SUCCEEDED(hr))
    {
        hr = Create2DTexture(SrcIndex, D3D11_USAGE_STAGING);

        if(FAILED(hr))
        {
            WriteToLog(_T("CInternalSurfaceTest::SetupTestCase() - Failed to create system memory texture, hr = 0x%X"), hr);
            tr = RESULT_BLOCKED;
        }
    }
    else
    {
        WriteToLog(_T("CInternalSurfaceTest::SetupTestCase() - Failed to create video memory texture, hr = 0x%X"), hr);
        tr = RESULT_BLOCKED;
    }

    m_pVidMemTex = m_pTex[DestIndex];
    m_pStagingTex = m_pTex[SrcIndex];

    return tr;
}


TEST_RESULT CInternalSurfaceTest::ExecuteTestCase()
{
    TEST_RESULT tr = RESULT_PASS;

    StartETWTracking();

    GetEffectiveContext()->Flush();
    // wait for all UMD events that have nothing to do with uploading to come in
    m_pETWMaster->WaitForEvents(5000);

    GetEffectiveContext()->CopyResource(m_pVidMemTex, m_pStagingTex);
    GetEffectiveContext()->Flush();

    // Check that resource has been uploaded to memory
    if(GetAllocationResidency(m_pVidMemTex) == RESIDENCY_NOT_RESIDENT)
    {
        WriteToLog(_T("CInternalSurfaceTest::ExecuteTestCase() - VidMemTex has not been made resident"));
        tr = RESULT_SKIP;
    }

    StopETWTracking(false);

    if(tr == RESULT_PASS)
        tr = VerifyResults();

    return tr;
}

//++++++++++++++++++++++ UMDLoggingInternalSurfaceTest ++++++++++++++++++++++++++++++
TEST_RESULT CUMDLoggingInternalSurfaceTest::ExecuteTestCase()
{
    TEST_RESULT tr = RESULT_PASS;

    StartETWTracking();

    GetEffectiveContext()->Flush();
    // wait for all UMD events that have nothing to do with uploading to come in
    m_CustomETWMaster.WaitForEvents(5000);

    m_CustomETWMaster.ReadyForCustomETWHandler(true);
    GetEffectiveContext()->CopyResource(m_pVidMemTex, m_pStagingTex);
    GetEffectiveContext()->Flush();

    // Wait for all events to come in
    m_CustomETWMaster.WaitForEvents(5000);
    m_CustomETWMaster.ReadyForCustomETWHandler(false);

    // Check that resource has been uploaded to memory
    if(GetAllocationResidency(m_pVidMemTex) == RESIDENCY_NOT_RESIDENT)
    {
        WriteToLog(_T("CInternalSurfaceTest::ExecuteTestCase() - VidMemTex has not been made resident"));
        tr = RESULT_SKIP;
    }

    StopETWTracking(false);

    if(tr == RESULT_PASS)
        tr = VerifyResults();

    return tr;
}

TEST_RESULT CUMDLoggingInternalSurfaceTest::VerifyResults()
{
    TEST_RESULT tr = RESULT_PASS;
    bool bFound = false;

    if(m_CustomETWMaster.IsNewStagingBuffer())
    {
        WriteToLog(_T("CUMDLoggingInternalSurfaceTest::VerifyResult() - new staging buffer detected"));
        bFound = true;
    }

    if(m_CustomETWMaster.IsReclaimedStagingBuffer())
    {
        WriteToLog(_T("CUMDLoggingInternalSurfaceTest::VerifyResult() - reclaimed staging buffer detected"));
        bFound = true;
    }

    if(m_CustomETWMaster.IsNewWrongSemantic())
    {
        WriteToLog(m_CustomETWMaster.GetErrorString().c_str());
        WriteToLog(_T("CUMDLoggingInternalSurfaceTest::VerifyResult() - new internal allocation with unexpected semantic is detected"));
        bFound = true;
        tr = RESULT_FAIL;
    }

    if(m_CustomETWMaster.IsReclaimedWrongSemantic())
    {
        WriteToLog(m_CustomETWMaster.GetErrorString().c_str());
        WriteToLog(_T("CUMDLoggingInternalSurfaceTest::VerifyResult() - reclaimed internal allocation with unexpected semantic is detected"));
        bFound = true;
        tr = RESULT_FAIL;
    }

    if(!bFound)
    {
        // Drivers are not required to use staging buffers
        WriteToLog(_T("CUMDLoggingInternalSurfaceTest::VerifyResult() - didn't detect any new or reclaimed internal allocations"));
    }
    

    if(LogListenerErrors())
    {
        WriteToLog(_T("CUMDLoggingInternalSurfaceTest::VerifyResults() - UMD logging errors detected"));
        tr = RESULT_FAIL;
    }

    return tr;
}


/////////////////////////////////////////////// Listeners //////////////////////////////////////////////////////////

void CUMDLoggingInternalSurfaceETW::HandleUMDAllocation(PEVENT_RECORD pEvent)
{
    bool bNewStagingBuffer = false;
    bool bNewDMABuffer = false;
    bool bNewOther = false;

    if(m_bReadyForCustomETWHandler)
    {
        CDisplayUMDEventHelper EH(pEvent);

        // We might be getting this event for allocation beloging to process that we 
        // are not tracking. Check if we are tracking this allocation
        HANDLE hDxgAllocation = (HANDLE)EH.GetProperty<ULONGLONG>(L"hDxgAllocation");

        AllocationTrackerMap::iterator it;

        CCSLocker Lock(m_CSAllocationMap);

        it = m_AllocationMap.find(hDxgAllocation);

        if(it == m_AllocationMap.end())
        {
            return;
        }

        CAllocationTracker* pAllocation = it->second;

        // We don't care about special handling of Report or Unmap
        if(EH.IsStart())
        {
            HANDLE hD3DResource = (HANDLE)EH.GetProperty<ULONGLONG>(L"hD3DAllocation");

            // Look at internal allocations only
            if(!hD3DResource)
            {
                eUMD_ALLOC_SEMANTIC Semantic = EH.GetProperty<eUMD_ALLOC_SEMANTIC>(L"Semantic");

                if((Semantic == UMDAllocSemantic_UploadStaging) || (Semantic == UMDAllocSemantic_DownloadStaging))
                {
                    m_bNewStagingBuffer = true;
                    bNewStagingBuffer = true;
                }
                else if(Semantic == UMDAllocSemantic_DMA_Buffer)
                {
                    // some drivers might create new DMA buffers in upload scenarios
                    // this is allowed
                    bNewDMABuffer = true;
                }
                else
                {
                    // we don't expect any other type of internal allocations, but we won't fail for small allocations
                    if(EH.GetProperty<ULONGLONG>(L"Size") > (ULONGLONG)(4 * 1024))
                    {
                        m_bNewWrongSemantic = true;
                        bNewOther = true;
                        tstring strSemantic = (Semantic == UMDAllocSemantic_ContextSave) ? _T("ContextSave") : _T("None");
                        m_tstrError = (_T("CUMDLoggingInternalSurfaceETW::HandleUMDAllocation() - mapping internal allocation with unexpected semantic %s"), strSemantic.c_str());
                    }
                }
            }
        }
    }

    CVidMmEtwMaster::HandleUMDAllocation(pEvent);

    if(bNewStagingBuffer)
    {
        DBGPrint(L"CUMDLoggingInternalSurfaceETW::HandleUMDAllocation - new STAGING buffer is detected");
    }
    
    if(bNewDMABuffer)
    {
        DBGPrint(L"CUMDLoggingInternalSurfaceETW::HandleUMDAllocation - new DMA buffer is detected");
    }

    if(bNewOther)
    {
        DBGPrint(L"CUMDLoggingInternalSurfaceETW::HandleUMDAllocation - new allocation with unexpected semantic is detected");
    }
}

void CUMDLoggingInternalSurfaceETW::HandleReclaimAllocation(PEVENT_RECORD pEvent)
{
    bool bReclaimedStagingBuffer = false;
    bool bReclaimedDMABuffer = false;
    bool bReclaimedOther = false;

    if(m_bReadyForCustomETWHandler)
    {
        CDxgKernelEventHelper EH(pEvent);

        HANDLE hAlloc = EH.GetProperty<HANDLE>(L"hAllocationHandle");

        AllocationTrackerMap::iterator it;

        CCSLocker Lock(m_CSAllocationMap);

        it = m_AllocationMap.find(hAlloc);

        if(it == m_AllocationMap.end())
        {
            // We only keep track of allocations from certain devices (as determined by the user)
            // This event doesn't have hDevice member, so we'll get these events for
            // allocations from all devices, which we might not be tracking
            return;
        }

        CAllocationTracker* pAllocation = it->second;
        sUMD_ALLOC_INFO* pUMDAllocInfo = pAllocation->GetUMDAllocInfo(NULL); // pass NULL handle for internal allocation

        if(pUMDAllocInfo)
        {
            // We only care about internal allocations, if this wasn't an internal allocation,
            // pUMDAllocInfo would be NULL. If we got here, then this allocation is internal
            assert(!pUMDAllocInfo->hD3DResource);

            // check semantic
            if((pUMDAllocInfo->Semantic == UMDAllocSemantic_UploadStaging) || (pUMDAllocInfo->Semantic == UMDAllocSemantic_DownloadStaging))
            {
                bReclaimedStagingBuffer = true;
                m_bReclaimedStagingBuffer = true;
            }
            else if(pUMDAllocInfo->Semantic == UMDAllocSemantic_DMA_Buffer)
            {
                // some drivers might reclaim DMA buffers in upload scenarios
                // this is allowed
                bReclaimedDMABuffer = true;
            }
            else
            {
                // we don't expect any other type of internal allocations, but we won't fail for small allocations
                if(pUMDAllocInfo->Size > (ULONGLONG)(4 * 1024))
                {
                    bReclaimedOther = true;
                    m_bReclaimedWrongSemantic = true;
                    tstring strSemantic = (pUMDAllocInfo->Semantic == UMDAllocSemantic_ContextSave) ? _T("ContextSave") : _T("None");
                    m_tstrError = (_T("CUMDLoggingInternalSurfaceETW::HandleUMDAllocation() - reclaiming internal allocation with unexpected semantic %s"), strSemantic.c_str());
                }
            }
        }
    }

    CVidMmEtwMaster::HandleReclaimAllocation(pEvent);

    if(bReclaimedStagingBuffer)
    {
        DBGPrint(L"CUMDLoggingInternalSurfaceETW::HandleReclaimAllocation - reclaimed STAGING buffer is detected");
    }
    
    if(bReclaimedDMABuffer)
    {
        DBGPrint(L"CUMDLoggingInternalSurfaceETW::HandleReclaimAllocation - reclaimed DMA buffer is detected");
    }

    if(bReclaimedOther)
    {
        DBGPrint(L"CUMDLoggingInternalSurfaceETW::HandleReclaimAllocation - reclaimed allocation with unexpected semantic is detected");
    }
}


//++++++++++++++++++++++++++++++++++++++++++++ COfferReclaimStagingSurfaceTest ++++++++++++++++++++++++++++++++++++++++++
TEST_RESULT COfferReclaimStagingSurfaceTest::VerifyResults()
{
    TEST_RESULT tr = RESULT_PASS;
    AllocationTrackerList AllocList;
    m_pETWMaster->GetAllocationListSnapshot(AllocList);
    const ULONGLONG MaxSizeNotOffered = 1024 * 1024;	// We allow a total of 1MB of staging resources to not be Offered
    ULONGLONG SizeNotOffered = 0;

    // go through all allocations, find all marked as Staging, make sure they are all offered
    AllocationTrackerList::iterator it;
    bool bFoundStagingBuffer = false;

    for(it = AllocList.begin(); it != AllocList.end(); it++)
    {
        CAllocationTracker* pAllocation = *it;
        assert(pAllocation);

        CCSLocker Lock(pAllocation->GetAllockTrackerCritSection());

        std::list<sUMD_ALLOC_INFO*> UMDAllocList = pAllocation->GetUMDAllocInfoList();
        std::list<sUMD_ALLOC_INFO*>::iterator UMDAllocListIt;

        for(UMDAllocListIt = UMDAllocList.begin(); UMDAllocListIt != UMDAllocList.end(); UMDAllocListIt++)
        {
            sUMD_ALLOC_INFO* pUMDAlloc = *UMDAllocListIt;

            if(pUMDAlloc)
            {
                if((pUMDAlloc->Semantic == UMDAllocSemantic_UploadStaging) || (pUMDAlloc->Semantic == UMDAllocSemantic_DownloadStaging))
                {
                    bFoundStagingBuffer = true;

                    // Make sure that this allocation is in one of the offer states
                    sALLOC_INFO* pAllocInfo = pAllocation->AllocInfo();

                    assert(pAllocInfo);

                    if((pAllocInfo->OfferState == OFFER_STATE_NOT_OFFERED) || (pAllocInfo->OfferPriority == OFFER_PRIORITY_NONE))
                    {
                        // Add size of this buffer to SizeNotOffered
                        SizeNotOffered += pAllocInfo->Size;
                        WriteToLog(_T("COfferReclaimStagingSurfaceTest::VerifyResults() - Staging buffer (hD3DAppResource == 0x%p, GlobalAlloc == 0x%p) of size %d hasn't been offered"),
                                    pUMDAlloc->hD3DAppResource,
                                    pAllocation->GlobalAlloc(),
                                    pAllocInfo->Size);

                        DBGPrint(L"COfferReclaimStagingSurfaceTest::VerifyResults() - Staging buffer (hD3DAppResource == 0x%p, GlobalAlloc == 0x%p) of size %d hasn't been offered",
                                    pUMDAlloc->hD3DAppResource,
                                    pAllocation->GlobalAlloc(),
                                    pAllocInfo->Size);
                    }
                }
            }
        }
    }

    // See if total size of not-offered staging resources exceeds the maximum allowed
    if(SizeNotOffered > MaxSizeNotOffered)
    {
        WriteToLog(_T("COfferReclaimStagingSurfaceTest::VerifyResults() - Total size (%I64u) of all non-Offered Staging buffers excceeds maximum allowed (%I64u)"), 
                      SizeNotOffered,
                      MaxSizeNotOffered);

        DBGPrint(L"COfferReclaimStagingSurfaceTest::VerifyResults() - Total size (%I64u) of all non-Offered Staging buffers excceeds maximum allowed (%I64u)", 
                      SizeNotOffered,
                      MaxSizeNotOffered);

        tr = RESULT_FAIL;
    }

    if(!bFoundStagingBuffer)
    {
        WriteToLog(_T("COfferReclaimStagingSurfaceTest::VerifyResults() - Didn't find any staging buffers"));
        tr = RESULT_SKIP;
    }

    // Log listener errors, but don't fail, because we don't want to fail on UMDLogging errors
    // or other errors not related to Offer/Reclaim
    LogListenerErrors();

    return tr;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CPackedSurfaceTest::CPackedSurfaceTest()
{
    m_PackedCount = 0;
    m_bNoAllocationLogging = false;
    ZeroMemory(m_pPackedAllocs, sizeof(m_pPackedAllocs[0]) * MAX_RESOURCE_COUNT);
    ZeroMemory(m_pPackedResources, sizeof(m_pPackedResources[0]) * MAX_RESOURCE_COUNT);
    m_bShared = false;
    m_ResourceType = WGFVIDMM_TEXTURE;
    m_bSharedProcessStarted = false;

    OutputDebugString("CPackedSurfaceTest()\n");
}

CPackedSurfaceTest::~CPackedSurfaceTest()
{
    m_pETWMaster = NULL;

    OutputDebugString("~CPackedSurfaceTest()\n");
}


void CPackedSurfaceTest::InitTestParameters()
{
    CVidMMUMDTest::InitTestParameters();
    
    // For this test we'll need a lot of small
    // resources (< 64K), which potentially
    // can be packed into one allocation
    m_Count = MAX_RESOURCE_COUNT;

    CTestCaseParameter<UINT> *pWidth = AddParameter<UINT>(_T("Width"), &m_Width);
    CTestCaseParameter<UINT> *pHeight = AddParameter<UINT>(_T("Height"), &m_Height);
    CTestCaseParameter<eWGFVIDMM_RESOURCE_TYPE> *pResourceType = AddParameter<eWGFVIDMM_RESOURCE_TYPE>(_T("ResourceType"), &m_ResourceType);

    CUserValueSet<UINT> *pWidthTexture = new CUserValueSet<UINT>();
    pWidthTexture->AddValue(64);

    CUserValueSet<UINT> *pWidthBuffer = new CUserValueSet<UINT>();
    pWidthBuffer->AddValue(16*1024);

    CUserValueSet<UINT> *pHeightTexture = new CUserValueSet<UINT>();
    pHeightTexture->AddValue(64);

    CUserValueSet<UINT> *pHeightBuffer = new CUserValueSet<UINT>();
    pHeightBuffer->AddValue(1);

    CTestCaseParameter<bool> *pShared = AddParameter<bool>(_T("Shared"), &m_bShared);
    CUserValueSet<bool> * pSharedValues = new CUserValueSet<bool>();
    pSharedValues->AddValue(false);
    pSharedValues->AddValue(true);

    testfactor::RFactor TextureFactor = AddParameterValue<eWGFVIDMM_RESOURCE_TYPE>(pResourceType, WGFVIDMM_TEXTURE)
                                        * AddParameterValueSet<bool>(pShared, pSharedValues)		
                                        * AddParameterValueSet<UINT>(pWidth, pWidthTexture)
                                        * AddParameterValueSet<UINT>(pHeight, pHeightTexture);


    testfactor::RFactor BufferFactor = AddParameterValue<eWGFVIDMM_RESOURCE_TYPE>(pResourceType, WGFVIDMM_BUFFER)
                                       * AddParameterValueSet<bool>(pShared, pSharedValues)		
                                       * AddParameterValueSet<UINT>(pWidth, pWidthBuffer)
                                       * AddParameterValueSet<UINT>(pHeight, pHeightBuffer);

    m_Factor = (TextureFactor + BufferFactor);
                        
    SetRootTestFactor(m_Factor);
}

void CPackedSurfaceTest::CleanupTestCase()
{
    TCHAR strError[ERROR_SIZE];	 // this string is used in macros
    ZeroMemory(strError, sizeof(TCHAR) * ERROR_SIZE);
    TEST_RESULT tr = RESULT_PASS; // this variable is used in macros

    m_PackedCount = 0;
    ZeroMemory(m_pPackedAllocs, sizeof(m_pPackedAllocs[0]) * MAX_RESOURCE_COUNT);
    ZeroMemory(m_pPackedResources, sizeof(m_pPackedResources[0]) * MAX_RESOURCE_COUNT);

    if(m_pManagerSync && m_bSharedProcessStarted)
    {
        m_bSharedProcessStarted = false;
        MANAGER_SIGNAL_AND_WAIT
        MANAGER_SIGNAL_TO_TERMINATE
MacroFailed:
        WriteToLog(strError);
    }

    SAFE_DELETE(m_pManagerSync);

    CVidMMUMDTest::CleanupTestCase();
}



TEST_RESULT CPackedSurfaceTest::SetupTestCase()
{
    if(m_bShared)
    {
        if(g_App.m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_10_1)
        {
            m_bNoKeyedMutex = true;
        }
    }

    TEST_RESULT tr = CVidMMUMDTest::SetupTestCase();

    if(tr != RESULT_PASS)
    {
        CVidMMUMDTest::CleanupTestCase();
        return tr;
    }

    TCHAR strError[ERROR_SIZE];	 // this string is used in macros
    ZeroMemory(strError, sizeof(TCHAR) * ERROR_SIZE);

    if(tr == RESULT_PASS)
    {
        if(!m_PresentHelper.Setup(this))
        {
            WriteToLog(_T("CPackedSurfaceTest::SetupTestCase() - failed to setup PresentHelper"));
            tr = RESULT_BLOCKED;
        }
    }

    if(m_bShared)
    {
        if(!m_pManagerSync)
        {
            m_pManagerSync = new CXProcSyncManager(WGFVIDMM_MAPPING_NAME, WGFVIDMM_TOTAL_SHARERS, sizeof(sWGFVIDMM_SHARED_DATA));

            if(!m_pManagerSync)
            {
                WriteToLog(_T("CPackedSurfaceTest::SetupTestCase() - out of memory"));
                return RESULT_BLOCKED;
            }
        }
    }

    return tr;
}

TEST_RESULT CPackedSurfaceTest::ExecuteTestCase()
{
    TEST_RESULT tr = RESULT_PASS;

    TCHAR strError[ERROR_SIZE];	 // this string is used in macros
    ZeroMemory(strError, sizeof(TCHAR) * ERROR_SIZE);

    StartETWTracking();

    HRESULT hr = S_OK;

    for(UINT i = 0; i < m_Count; i++)
    {
         if(m_ResourceType == WGFVIDMM_TEXTURE)
        {
            hr = Create2DTexture(i, D3D11_USAGE_DEFAULT, RESOURCE_PATTERN_1, m_bShared);
        }
        else if (m_ResourceType == WGFVIDMM_BUFFER)
        {
            D3D11_USAGE Usage = D3D11_USAGE_DEFAULT;

            hr = CreateBuffer(i, Usage, m_bShared, m_pBufferData1);
        }

        if(FAILED(hr))
        {
            WriteToLog(_T("CPackedSurfaceTest::SetupTestCase() - failed to create resource %d, hr = 0x%X"), i, hr);
            tr = RESULT_BLOCKED;
            break;
        }

        if(tr == RESULT_PASS)
        {
            // Fill resources
            ACQUIRE_MUTEX_FOR_SINGLE_RESOURCE(i)
            if(m_ResourceType == WGFVIDMM_TEXTURE)
            {
                hr = Fill2DTexture(i, RESOURCE_PATTERN_1);
            }
            else if (m_ResourceType == WGFVIDMM_BUFFER)
            {
                hr = FillBuffer(i, m_pBufferData1);
            }
            RELEASE_MUTEX_FOR_SINGLE_RESOURCE(i)

            if(FAILED(hr))
            {
                WriteToLog(_T("CPackedSurfaceTest::ExecuteTestCase() - failed to fill resource %d, hr = 0x%X"), i, hr);
                tr = RESULT_BLOCKED;
                break;
            }
        }

        if(tr == RESULT_PASS)
        {
            bool bPresent = false;
            g_App.setPresent(true);

            ACQUIRE_MUTEX_FOR_SINGLE_RESOURCE(i)
            if(m_ResourceType == WGFVIDMM_TEXTURE)
            {
                bPresent = m_PresentHelper.PresentTexture2D(m_pTex[i], m_pShaderResourceViews[i]);
            }
            else if (m_ResourceType == WGFVIDMM_BUFFER)
            {
                bPresent = m_PresentHelper.PresentBuffer(m_pBuf[i]);
            }
            RELEASE_MUTEX_FOR_SINGLE_RESOURCE(i)

            if(!bPresent)
            {
                WriteToLog(_T("CPackedSurfaceTest::ExecuteTestCase() - Failed to present iteration %d"), i);
                tr = RESULT_FAIL;
                break;
            }
        }
    }

    if(tr == RESULT_PASS && m_bShared)
    {
        // Start our client app
        tr = StartChildProcess("-childBasic");

        if(tr != RESULT_PASS)
        {
            WriteToLog(_T("CPackedSurfaceTest::SetupTestCase() - failed to start child process"));
        }
        else
        {
            m_bSharedProcessStarted = true;
            Sleep(1000); // give it a chance to start up
            OutputDebugString("CPackedSurfaceTest::SetupTestCase() - successfully started child process, moving child to Running state\n");
            MANAGER_SIGNAL_TO_RUN
        }
    }

    m_pETWMaster->WaitForEvents(10000);

    // See if any of the resources are packed into one allocation
    for(UINT i = 0; i < m_Count; i++)
    {
        HANDLE hAllocHandle = NULL;

        if(m_ResourceType == WGFVIDMM_TEXTURE)
        {
            hAllocHandle = (HANDLE)m_pTex[i];
        }
        else if(m_ResourceType == WGFVIDMM_BUFFER)
        {
            hAllocHandle = (HANDLE)m_pBuf[i];
        }

        CAllocationTracker* pAllocation = m_pETWMaster->GetAllocationTracker(hAllocHandle);
        
        if(!pAllocation)
        {
            WriteToLog(_T("CPackedSurfaceTest::ExecuteTestCase() - Failed to get AllocationTracker for D3D resource %p"), hAllocHandle);
            m_bNoAllocationLogging = true;
            break;
        }

        sUMD_ALLOC_INFO* pUMDAllocInfo = pAllocation->GetUMDAllocInfo(hAllocHandle);
        
        if(!pUMDAllocInfo)
        {
            WriteToLog(_T("CPackedSurfaceTest::ExecuteTestCase() - Failed to get UMDAllocInfo for D3D resource %p"), hAllocHandle);
            m_bNoAllocationLogging = true;
            break;
        }

        if(pUMDAllocInfo->Usage.Packed)
        {
            CCSLocker Lock(pAllocation->GetAllockTrackerCritSection());

            std::list<sUMD_ALLOC_INFO*> UMDInfoList = pAllocation->GetUMDAllocInfoList();
            std::list<sUMD_ALLOC_INFO*>::iterator it;

            for(it = UMDInfoList.begin(); it != UMDInfoList.end(); it++)
            {
                sUMD_ALLOC_INFO* pTemp = *it;
                assert(pTemp);

                // Find this allocation and fill out the m_pPacked* arrays to point to it
                bool bFound = false;

                for(UINT y = 0; y != m_Count; y++)
                {
                    HANDLE hAllocHandleTemp = NULL;

                    if(m_ResourceType == WGFVIDMM_TEXTURE)
                    {
                        hAllocHandleTemp = (HANDLE)m_pTex[y];
                    }
                    else if(m_ResourceType == WGFVIDMM_BUFFER)
                    {
                        hAllocHandleTemp = (HANDLE)m_pBuf[y];
                    }

                    if(hAllocHandleTemp == pTemp->hD3DAppResource)
                    {
                        m_pPackedAllocs[m_PackedCount] = hAllocHandleTemp;
                        m_pPackedResources[m_PackedCount] = m_pResources[y];
                        m_PackedCount++;	
                        char str[256] = {0};
                        sprintf(str, "packed count == %d\n", m_PackedCount);
                        OutputDebugString(str);
                        bFound = true;
                        break;
                    }
                }


                if(!bFound)
                {
                    // At least one of packed resources don't belong to us, skip this allocation
                    m_PackedCount = 0;
                    ZeroMemory(m_pPackedAllocs, sizeof(m_pPackedAllocs[0]) * m_Count);
                    ZeroMemory(m_pPackedResources, sizeof(m_pPackedResources[0]) * m_Count);

                    break;
                }

            }


            if(m_PackedCount > MAX_RESOURCE_COUNT)
            {
                WriteToLog(_T("CPackedSurfaceTest::ExecuteTestCase() - Packed count (%d) excceeded maximum expected (%d)"), m_PackedCount, m_Count);
                tr = RESULT_BLOCKED;
            }

            if(m_PackedCount)
                break;
        }
    }

    if(m_bNoAllocationLogging)
    {
        WriteToLog(_T("CPackedSurfaceTest::ExecuteTestCase() - no allocation logging"));
    }

    tr = VerifyResults();

    StopETWTracking(true);

    return tr;

MacroFailed:
    WriteToLog(strError);
    WriteToLog(_T("CPackedSurfaceTest::SetupTestCase() - there was a failure in one of the macros"));
    return tr;
}

//+++++++++++++++++++++++++++++++++++++++++++ CUMDLoggingPackedSurfaceTest +++++++++++++++++++++++++++++++++++++
TEST_RESULT CUMDLoggingPackedSurfaceTest::VerifyResults()
{
    TEST_RESULT tr = RESULT_PASS;

    if(m_bNoAllocationLogging)
        return RESULT_FAIL;

    if(m_PackedCount <= 1)
    {
        WriteToLog(_T("CUMDLoggingPackedSurfaceTest::ExecuteTestCase() - didn't detect any packed allocations"));
        tr = RESULT_SKIP;
    }

    // Most validation will be done by VidMMEtwListener in real time
    // Validate here that allocation size reported by UMD is same as what test has specified
    for(UINT i = 0; i < m_Count; i++)
    {
        HANDLE hAllocHandle = NULL;

        if(m_ResourceType == WGFVIDMM_TEXTURE)
        {
            hAllocHandle = (HANDLE)m_pTex[i];
        }
        else if(m_ResourceType == WGFVIDMM_BUFFER)
        {
            hAllocHandle = (HANDLE)m_pBuf[i];
        }

        CAllocationTracker* pAllocation = m_pETWMaster->GetAllocationTracker(hAllocHandle);
        assert(pAllocation);

        if(!pAllocation)
        {
            WriteToLog(_T("CUMDLoggingPackedSurfaceTest::VerifyResults() - Failed to get AllocationTracker for D3D resource %p"), hAllocHandle);
            m_bNoAllocationLogging = true;
            break;
        }

        sUMD_ALLOC_INFO* pUMDAllocInfo = pAllocation->GetUMDAllocInfo(hAllocHandle);
        assert(pUMDAllocInfo);

        if(pUMDAllocInfo && pUMDAllocInfo->Size != m_SizeInBytes)
        {
            // TODO: Test Bug 147608. Disabling this check until this bug is fixed
            //WriteToLog(_T("CUMDLoggingPackedSurfaceTest::VerifyResults() - UMD reported allocation size is not what's expected (%d, %d)"), pUMDAllocInfo->Size, m_SizeInBytes);
            //tr = RESULT_FAIL;
        }
    }

    if(LogListenerErrors())
    {
        WriteToLog(_T("CUMDLoggingPackedSurfaceTest::VerifyResults() - UMD logging errors detected"));
        tr = RESULT_FAIL;
    }


    return tr;
}

//+++++++++++++++++++++++++++++++++++++++++++ COfferReclaimPackedSurfaceTest +++++++++++++++++++++++++++++++++++++
BEGIN_NAMED_VALUES(eOFFER_RECLAIM_PACKED_SURFACE_TEST)
    NAMED_VALUE(_T("OFFER_RECLAIM_PACKED_SURFACE_TEST_SINGLE"), OFFER_RECLAIM_PACKED_SURFACE_TEST_SINGLE)
    NAMED_VALUE(_T("OFFER_RECLAIM_PACKED_SURFACE_TEST_MULTIPLE"), OFFER_RECLAIM_PACKED_SURFACE_TEST_MULTIPLE)
    NAMED_VALUE(_T("OFFER_RECLAIM_PACKED_SURFACE_TEST_ALL"), OFFER_RECLAIM_PACKED_SURFACE_TEST_ALL)
END_NAMED_VALUES(eOFFER_RECLAIM_PACKED_SURFACE_TEST)

void COfferReclaimPackedSurfaceTest::InitTestParameters()
{
    CPackedSurfaceTest::InitTestParameters();

    CTestCaseParameter<eOFFER_RECLAIM_PACKED_SURFACE_TEST> *pTestType = AddParameter<eOFFER_RECLAIM_PACKED_SURFACE_TEST>(_T("TestType"), &m_TestType);
    CUserValueSet<eOFFER_RECLAIM_PACKED_SURFACE_TEST> *pTestTypeValues = new CUserValueSet<eOFFER_RECLAIM_PACKED_SURFACE_TEST>();
    pTestTypeValues->AddValue( OFFER_RECLAIM_PACKED_SURFACE_TEST_SINGLE );
    pTestTypeValues->AddValue( OFFER_RECLAIM_PACKED_SURFACE_TEST_MULTIPLE );
    pTestTypeValues->AddValue( OFFER_RECLAIM_PACKED_SURFACE_TEST_ALL );

    m_Factor = m_Factor * AddParameterValueSet<eOFFER_RECLAIM_PACKED_SURFACE_TEST>(pTestType, pTestTypeValues);

    SetRootTestFactor(m_Factor);
}

TEST_RESULT COfferReclaimPackedSurfaceTest::ExecuteTestCase()
{
    TEST_RESULT tr = CPackedSurfaceTest::ExecuteTestCase();

    if(tr != RESULT_PASS)
        return tr;

    if(m_bNoAllocationLogging)
        return RESULT_BLOCKED;

    assert(m_pDXGIDevice2);

    StartETWTracking();

    switch(m_TestType)
    {
    case OFFER_RECLAIM_PACKED_SURFACE_TEST_SINGLE:
        tr = ExecuteTestCaseSingle();
        break;
    case OFFER_RECLAIM_PACKED_SURFACE_TEST_MULTIPLE:
        tr = ExecuteTestCaseMultiple();
        break;
    case OFFER_RECLAIM_PACKED_SURFACE_TEST_ALL:
        tr = ExecuteTestCaseAll();
        break;
    default:
        assert(0);
    }


    if(tr == RESULT_PASS)
    {
        tr = VerifyResults();

        // Log listener errors, but don't fail, because we don't want to fail on UMDLogging errors
        // or other errors not related to Offer/Reclaim
        LogListenerErrors();
    }


    StopETWTracking(true);

    return tr;
}

TEST_RESULT COfferReclaimPackedSurfaceTest::VerifyResults()
{
    TEST_RESULT tr = RESULT_PASS;

    if(m_PackedCount <= 1)
    {
        WriteToLog(_T("COfferReclaimPackedSurfaceTest::ExecuteTestCase() - didn't detect any packed allocations"));
        tr = RESULT_SKIP;
    }

    return tr;
}

TEST_RESULT COfferReclaimPackedSurfaceTest::ExecuteTestCaseSingle()
{
    TEST_RESULT tr = RESULT_PASS;
    HRESULT hr = S_OK;
    eALLOCATION_OFFER_STATE OfferState;
    eALLOCATION_OFFER_PRIORITY OfferPriority;
    TCHAR strError[ERROR_SIZE]; // this string is use in macros
    ZeroMemory(strError, sizeof(TCHAR) * ERROR_SIZE);


    // Start Offering packed resources one by one
    // Make sure allocation is not offered until all
    // resources have been offered
    for(UINT i = 0; i < m_PackedCount; i++)
    {
        DXGI_OFFER_RESOURCE_PRIORITY DXGIOfferPriority = DXGI_OFFER_RESOURCE_PRIORITY_LOW;

        if(i == (m_PackedCount-1)) // the last one in the pack
        {
            DXGIOfferPriority = DXGI_OFFER_RESOURCE_PRIORITY_NORMAL;
        }

        ACQUIRE_MUTEX
        hr = m_pDXGIDevice2->OfferResources(1, &m_pPackedResources[i], DXGIOfferPriority);
        RELEASE_MUTEX

        if(FAILED(hr))
        {
            tr = RESULT_FAIL;
            WriteToLog(_T("COfferReclaimPackedSurfaceTest::ExecuteTestCaseSingle() - OfferResources() failed, i = %d, hr = 0x%X"), i, hr);
        }
        else
        {
            GetEffectiveContext()->Flush();
            m_pETWMaster->WaitForEvents(5000);

            OfferState = GetOfferState(m_pPackedAllocs[i]);

            if(i == (m_PackedCount-1)) // the last one in the pack
            {
                // When last resouce in a pack is offered, we expect Allocation to be Offered with highest priority (NORMAL in this case)
                if(OfferState == OFFER_STATE_NOT_OFFERED)
                {
                    tr = RESULT_FAIL;
                    WriteToLog(_T("COfferReclaimPackedSurfaceTest::ExecuteTestCaseSingle() - All resources in a pack got offered, but allocation has not been Offered"));
                }
                else
                {
                    OfferPriority = GetAllocationOfferPriority(m_pPackedAllocs[i]);

                    if(OfferPriority != OFFER_PRIORITY_NORMAL)
                    {
                        tr = RESULT_FAIL;
                        WriteToLog(_T("COfferReclaimPackedSurfaceTest::ExecuteTestCase() - All resources in a pack got offered, but Priority is unexpected (%d, %d)"), 
                                    OfferPriority,
                                    OFFER_PRIORITY_NORMAL);
                    }
                }
            }
            else
            {
                // We don't expect allocation to be Offered when not all resources in the pack have been offered
                if(OfferState != OFFER_STATE_NOT_OFFERED)
                {
                    tr = RESULT_FAIL;
                    WriteToLog(_T("COfferReclaimPackedSurfaceTest::ExecuteTestCaseSingle() - Not all resources in a pack got offered, but allocation is already Offered"));
                }
            }
        }
    }

    if(tr == RESULT_PASS)
    {
        // We expect allocation to be reclaimed as soon as one resource is reclaimed
        ACQUIRE_MUTEX
        hr = m_pDXGIDevice2->ReclaimResources(1, &m_pPackedResources[0], NULL);
        RELEASE_MUTEX

        if(FAILED(hr))
        {
            tr = RESULT_FAIL;
            WriteToLog(_T("COfferReclaimPackedSurfaceTest::ExecuteTestCaseSingle() - ReclaimResources() failed, hr = 0x%X"), hr);
        }
        else
        {
            GetEffectiveContext()->Flush();
            m_pETWMaster->WaitForEvents(5000);

            OfferState = GetOfferState(m_pPackedAllocs[0]);

            if(OfferState != OFFER_STATE_NOT_OFFERED)
            {
                tr = RESULT_FAIL;
                WriteToLog(_T("COfferReclaimPackedSurfaceTest::ExecuteTestCaseSingle() - One of the resource has been Reclaimed, but allocation is still Offered"));
            }
        }
    }

    return tr;

MacroFailed:
    WriteToLog(strError);
    WriteToLog(_T("COfferReclaimPackedSurfaceTest::ExecuteTestCaseAll() - There was a failure in one of the macros"));

    return tr;
}

TEST_RESULT COfferReclaimPackedSurfaceTest::ExecuteTestCaseMultiple()
{
    TEST_RESULT tr = RESULT_PASS;
    HRESULT hr = S_OK;
    eALLOCATION_OFFER_STATE OfferState;
    eALLOCATION_OFFER_PRIORITY OfferPriority;
    TCHAR strError[ERROR_SIZE];	// this sting is used by macros
    ZeroMemory(strError, sizeof(TCHAR) * ERROR_SIZE);


    // Offer all but one, make sure allocation is not offered
    ACQUIRE_MUTEX
    hr = m_pDXGIDevice2->OfferResources((m_PackedCount - 1), m_pPackedResources, DXGI_OFFER_RESOURCE_PRIORITY_NORMAL);
    RELEASE_MUTEX

    if(FAILED(hr))
    {
        tr = RESULT_FAIL;
        WriteToLog(_T("COfferReclaimPackedSurfaceTest::ExecuteTestCaseMultiple() - OfferResources() failed, hr = 0x%X"), hr);
    }
    else
    {
        GetEffectiveContext()->Flush();
        m_pETWMaster->WaitForEvents(5000);

        OfferState = GetOfferState(m_pPackedAllocs[0]);

        if(OfferState != OFFER_STATE_NOT_OFFERED)
        {
            tr = RESULT_FAIL;
            WriteToLog(_T("COfferReclaimPackedSurfaceTest::ExecuteTestCaseMultiple() - Not all resources in a pack got offered, but allocation is already Offered"));
        }
    }

    // Offer last resource and verify that allocation is now Offered
    ACQUIRE_MUTEX
    hr = m_pDXGIDevice2->OfferResources(1, &m_pPackedResources[m_PackedCount - 1], DXGI_OFFER_RESOURCE_PRIORITY_LOW);
    RELEASE_MUTEX

    if(FAILED(hr))
    {
        tr = RESULT_FAIL;
        WriteToLog(_T("COfferReclaimPackedSurfaceTest::ExecuteTestCaseMultiple() - OfferResources() failed for last resource, hr = 0x%X"), hr);
    }
    else
    {
        GetEffectiveContext()->Flush();
        m_pETWMaster->WaitForEvents(5000);

        OfferState = GetOfferState(m_pPackedAllocs[0]);

        if(OfferState == OFFER_STATE_NOT_OFFERED)
        {
            tr = RESULT_FAIL;
            WriteToLog(_T("COfferReclaimPackedSurfaceTest::ExecuteTestCaseMultiple() - All resources in a pack got offered, but allocation has not been Offered"));
        }
        else
        {
            OfferPriority = GetAllocationOfferPriority(m_pPackedAllocs[0]);

            if(OfferPriority != OFFER_PRIORITY_NORMAL)
            {
                tr = RESULT_FAIL;
                WriteToLog(_T("COfferReclaimPackedSurfaceTest::ExecuteTestCaseMultiple() - Allocation is offered, but Priority is unexpected (%d, %d)"), 
                            OfferPriority,
                            OFFER_PRIORITY_NORMAL);
            }
        }
    }


    if(tr == RESULT_PASS)
    {
        // We expect allocation to be reclaimed as soon as one resource is reclaimed
        ACQUIRE_MUTEX
        hr = m_pDXGIDevice2->ReclaimResources(1, &m_pPackedResources[0], NULL);
        RELEASE_MUTEX

        if(FAILED(hr))
        {
            tr = RESULT_FAIL;
            WriteToLog(_T("COfferReclaimPackedSurfaceTest::ExecuteTestCaseMultiple() - ReclaimResources() failed, hr = 0x%X"), hr);
        }
        else
        {
            GetEffectiveContext()->Flush();
            m_pETWMaster->WaitForEvents(5000);

            OfferState = GetOfferState(m_pPackedAllocs[0]);

            if(OfferState != OFFER_STATE_NOT_OFFERED)
            {
                tr = RESULT_FAIL;
                WriteToLog(_T("COfferReclaimPackedSurfaceTest::ExecuteTestCaseMultiple() - One of the resource has been Reclaimed, but allocation is still Offered"));
            }
        }
    }

    return tr;

MacroFailed:
    WriteToLog(strError);
    WriteToLog(_T("COfferReclaimPackedSurfaceTest::ExecuteTestCaseAll() - There was a failure in one of the macros"));

    return tr;
}

TEST_RESULT COfferReclaimPackedSurfaceTest::ExecuteTestCaseAll()
{
    TEST_RESULT tr = RESULT_PASS;
    HRESULT hr = S_OK;
    eALLOCATION_OFFER_STATE OfferState;
    eALLOCATION_OFFER_PRIORITY OfferPriority;
    TCHAR strError[ERROR_SIZE]; // this string is used in macros
    ZeroMemory(strError, sizeof(TCHAR) * ERROR_SIZE);


    // Offer all resources, make sure allocation is Offered
    ACQUIRE_MUTEX
    hr = m_pDXGIDevice2->OfferResources(m_PackedCount, m_pPackedResources, DXGI_OFFER_RESOURCE_PRIORITY_NORMAL);
    RELEASE_MUTEX

    if(FAILED(hr))
    {
        tr = RESULT_FAIL;
        WriteToLog(_T("COfferReclaimPackedSurfaceTest::ExecuteTestCaseAll() - OfferResources() failed, hr = 0x%X"), hr);
    }
    else
    {
        GetEffectiveContext()->Flush();
        m_pETWMaster->WaitForEvents(5000);

        OfferState = GetOfferState(m_pPackedAllocs[0]);

        if(OfferState == OFFER_STATE_NOT_OFFERED)
        {
            tr = RESULT_FAIL;
            WriteToLog(_T("COfferReclaimPackedSurfaceTest::ExecuteTestCaseAll() - All resources in a pack got offered, but allocation has not been Offered"));
        }
    }

    if(tr == RESULT_PASS)
    {
        ACQUIRE_MUTEX
        hr = m_pDXGIDevice2->ReclaimResources(m_PackedCount, m_pPackedResources, NULL);
        RELEASE_MUTEX

        if(FAILED(hr))
        {
            tr = RESULT_FAIL;
            WriteToLog(_T("COfferReclaimPackedSurfaceTest::ExecuteTestCaseAll() - ReclaimResources() failed, hr = 0x%X"), hr);
        }
        else
        {
            GetEffectiveContext()->Flush();
            m_pETWMaster->WaitForEvents(5000);

            OfferState = GetOfferState(m_pPackedAllocs[0]);

            if(OfferState != OFFER_STATE_NOT_OFFERED)
            {
                tr = RESULT_FAIL;
                WriteToLog(_T("COfferReclaimPackedSurfaceTest::ExecuteTestCaseAll() - Allocation is still Offered"));
            }
        }
    }

    return tr;

MacroFailed:
    WriteToLog(strError);
    WriteToLog(_T("COfferReclaimPackedSurfaceTest::ExecuteTestCaseAll() - There was a failure in one of the macros"));

    return tr;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CRenamedSurfaceTest::CRenamedSurfaceTest()
{
    m_TimesToRename = 0;
    OutputDebugString("CRenamedSurfaceTest()\n");
}

CRenamedSurfaceTest::~CRenamedSurfaceTest()
{
    m_pETWMaster = NULL;
    OutputDebugString("~CRenamedSurfaceTest()\n");
}


void CRenamedSurfaceTest::InitTestParameters()
{
    CVidMMUMDTest::InitTestParameters();
    
    // For this test we'll need a medium size allocation
    // Large enough so that it's not packed, but small 
    // enough that there are enough resources to rename it
    // few times
    m_Count = 2;
    m_TimesToRename = 4;

    CTestCaseParameter<eWGFVIDMM_RESOURCE_TYPE> *pResourceType = AddParameter<eWGFVIDMM_RESOURCE_TYPE>(_T("ResourceType"), &m_ResourceType);
    CTestCaseParameter<UINT> *pWidth = AddParameter<UINT>(_T("Width"), &m_Width);
    CTestCaseParameter<UINT> *pHeight = AddParameter<UINT>(_T("Height"), &m_Height);

    CUserValueSet<UINT> *pWidthTexture = new CUserValueSet<UINT>();
    pWidthTexture->AddValue(512);

    CUserValueSet<UINT> *pHeightTexture = new CUserValueSet<UINT>();
    pHeightTexture->AddValue(512);

    CUserValueSet<UINT> *pWidthBuffer = new CUserValueSet<UINT>();
    pWidthBuffer->AddValue(512 * 1024);

    CUserValueSet<UINT> *pHeightBuffer = new CUserValueSet<UINT>();
    pHeightBuffer->AddValue(1);

    testfactor::RFactor TextureFactor = AddParameterValue<eWGFVIDMM_RESOURCE_TYPE>(pResourceType, WGFVIDMM_TEXTURE)	
                                        * AddParameterValueSet<UINT>(pWidth, pWidthTexture)
                                        * AddParameterValueSet<UINT>(pHeight, pHeightTexture);


    testfactor::RFactor BufferFactor = AddParameterValue<eWGFVIDMM_RESOURCE_TYPE>(pResourceType, WGFVIDMM_BUFFER)	
                                       * AddParameterValueSet<UINT>(pWidth, pWidthBuffer)
                                       * AddParameterValueSet<UINT>(pHeight, pHeightBuffer);

    m_Factor = (TextureFactor + BufferFactor);
                        
    SetRootTestFactor(m_Factor);

}


TEST_RESULT CRenamedSurfaceTest::SetupTestCase()
{
    TEST_RESULT tr = CVidMMUMDTest::SetupTestCase();
    HRESULT hr = S_OK;

    if(tr != RESULT_PASS)
    {
        CVidMMUMDTest::CleanupTestCase();
        return tr;
    }

    if(!m_PresentHelper.Setup(this))
    {
        WriteToLog(_T("CRenamedSurfaceTest::SetupTestCase() - failed to setup PresentHelper"));
        tr = RESULT_BLOCKED;
    }

    for(UINT i = 0; i < m_Count; i++)
    {
        if(m_ResourceType == WGFVIDMM_TEXTURE)
        {
            hr = Create2DTexture(i, D3D11_USAGE_DYNAMIC);
        }
        else if(m_ResourceType == WGFVIDMM_BUFFER)
        {
            hr = CreateBuffer(i, D3D11_USAGE_DYNAMIC, false, m_pBufferData1);
        }

        if(FAILED(hr))
        {
            WriteToLog(_T("CRenamedSurfaceTest::SetupTestCase() - Failed to create resource, i = %d, hr = 0x%X"), i, hr);
            tr = RESULT_BLOCKED;
            break;
        }

        if(tr == RESULT_PASS)
        {
            if(m_ResourceType == WGFVIDMM_TEXTURE)
            {
                hr = Fill2DTexture(i, RESOURCE_PATTERN_1);
            }
            else if (m_ResourceType == WGFVIDMM_BUFFER)
            {
                hr = FillBuffer(i, m_pBufferData1);
            }

            if(FAILED(hr))
            {
                WriteToLog(_T("CRenamedSurfaceTest::SetupTestCase() - Failed to fill resource with pattern, i = %d, hr = 0x%X"), i, hr);
                tr = RESULT_BLOCKED;
                break;
            }
        }
    }

    return tr;
}

TEST_RESULT CRenamedSurfaceTest::ExecuteTestCase()
{
    TEST_RESULT tr = RESULT_PASS;

    StartETWTracking();

    HRESULT hr = S_OK;

    if(m_ResourceType == WGFVIDMM_TEXTURE)
    {
        hr = RenameTexture(m_pTex[0], m_pShaderResourceViews[0]);
    }
    else if (m_ResourceType == WGFVIDMM_BUFFER)
    {
        hr = RenameBuffer(m_pBuf[0]);
    }

    if(FAILED(hr))
    {
        tr = RESULT_BLOCKED;
    }

    StopETWTracking(false);
    
    if(tr == RESULT_PASS)
    {
        tr = VerifyResults();
    }
    
    return tr;
}


HRESULT CRenamedSurfaceTest::RenameTexture(ID3D11Texture2D* pTex, ID3D11ShaderResourceView* pShaderView)
{
    HRESULT hr = S_OK;
    g_App.setPresent(false);

    for(UINT i = 0; i < m_TimesToRename; i++)
    {
        // Draw with texture, but don't present
        if(!m_PresentHelper.PresentTexture2D(pTex, pShaderView))
        {
            WriteToLog(_T("CRenamedSurfaceTest::RenameTexture() - Present() failed, which may indicate incorrect driver behavior. Please check dxgkdx log for any possible errors"));
            hr = E_FAIL;
            break;
        }
        else
        {
            // Map allocation with DISCARD flag, this will
            // hopefully result in Renaming
            D3D11_MAPPED_SUBRESOURCE MappedResource;
            hr = GetEffectiveContext()->Map(pTex, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

            if(FAILED(hr))
            {
                WriteToLog(_T("CRenamedSurfaceTest::RenameTexture() - Map() failed, i = %d, hr = 0x%X"), i, hr);
                break;
            }
            // write a little
            ULONG* puData = reinterpret_cast<ULONG*>( MappedResource.pData );
        
            puData[0] = RESOURCE_PATTERN_1;

            GetEffectiveContext()->Unmap(pTex, 0);
        }
    }

    // Draw with texture to retire previous instances
    if(SUCCEEDED(hr))
    {
        if(!m_PresentHelper.PresentTexture2D(pTex, pShaderView))
        {
            WriteToLog(_T("CRenamedSurfaceTest::RenameTexture() - Present() failed, which may indicate incorrect driver behavior. Please check dxgkdx log for any possible errors"));
            hr = E_FAIL;
        }
    }

    return hr;
}


HRESULT CRenamedSurfaceTest::RenameBuffer(ID3D11Buffer* pBuf)
{
    HRESULT hr = S_OK;
    g_App.setPresent(false);

    for(UINT i = 0; i < m_TimesToRename; i++)
    {
        // Draw with buffer, but don't present
        if(!m_PresentHelper.PresentBuffer(pBuf))
        {
            WriteToLog(_T("CRenamedSurfaceTest::RenameTexture() - RenameBuffer() failed, which may indicate incorrect driver behavior. Please check dxgkdx log for any possible errors"));
            hr = E_FAIL;
            break;
        }
        else
        {
            // Map allocation with DISCARD flag, this will
            // hopefully result in Renaming
            D3D11_MAPPED_SUBRESOURCE MappedResource;
            hr = GetEffectiveContext()->Map(pBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

            if(FAILED(hr))
            {
                WriteToLog(_T("CRenamedSurfaceTest::RenameBuffer() - Map() failed, i = %d, hr = 0x%X"), i, hr);
                break;
            }
            // write a little
            float* puData = reinterpret_cast<float*>( MappedResource.pData );
        
            puData[0] = m_pBufferData1[0];

            GetEffectiveContext()->Unmap(pBuf, 0);
        }
    }

    if(SUCCEEDED(hr))
    {
        // Draw with texture to retire previous instances
        if(!m_PresentHelper.PresentBuffer(pBuf))
        {
            WriteToLog(_T("CRenamedSurfaceTest::RenameTexture() - RenameBuffer() failed, which may indicate incorrect driver behavior. Please check dxgkdx log for any possible errors"));
            hr = E_FAIL;
        }
    }

    return hr;
}

//++++++++++++++++++++++++++++++++++++++++++ CUMDLoggingRenamedSurfaceTest ++++++++++++++++++++++++++++++++++++
TEST_RESULT CUMDLoggingRenamedSurfaceTest::ExecuteTestCase()
{
    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;

    StartETWTracking();
    if(m_ResourceType == WGFVIDMM_TEXTURE)
    {
        hr = RenameTexture(m_pTex[0], m_pShaderResourceViews[0]);
    }
    else if (m_ResourceType == WGFVIDMM_BUFFER)
    {
        hr = RenameBuffer(m_pBuf[0]);
    }

    if(FAILED(hr))
    {
        tr = RESULT_BLOCKED;
    }
    else
    {
        // Present to have renamed instances of Allocation A retired
        g_App.setPresent(true);
        if(!GetFramework()->Present(NULL, NULL, NULL, NULL, 0, 0))
        {
            WriteToLog(_T("CUMDLoggingRenamedSurfaceTest::ExecuteTestCase() - Present() failed, which may indicate incorrect driver behavior. Please check dxgkdx log for any possible errors"));
            tr = RESULT_FAIL;
            StopETWTracking(true);
        }
        else
        {
            // Rename Allocation B
            if(m_ResourceType == WGFVIDMM_TEXTURE)
            {
                hr = RenameTexture(m_pTex[1], m_pShaderResourceViews[1]);
            }
            else if (m_ResourceType == WGFVIDMM_BUFFER)
            {
                hr = RenameBuffer(m_pBuf[1]);
            }
            if(FAILED(hr))
            {
                tr = RESULT_BLOCKED;
            }
        }
    }

    if(tr == RESULT_PASS)
    {
        tr = VerifyResults();
    }

    StopETWTracking(true);

    return tr;
}

TEST_RESULT CUMDLoggingRenamedSurfaceTest::VerifyResults()
{
    TEST_RESULT tr = RESULT_PASS;
    CAllocationTracker *pAllocationA = NULL, *pAllocationB = NULL;
    sUMD_ALLOC_INFO *pUMDAllocInfoA = NULL, *pUMDAllocInfoB = NULL;
    sRENAMED_ALLOC  *pRenamedAllocA = NULL, *pRenamedAllocB = NULL;
    bool bAllocARenamedByUMD = false, bAllocBRenamedByUMD = false;
    HANDLE hAllocA = NULL, hAllocB = NULL;

    if(m_ResourceType == WGFVIDMM_TEXTURE)
    {
        hAllocA = (HANDLE)m_pTex[0];
        hAllocB = (HANDLE)m_pTex[1];
    }
    else
    {
        hAllocA = (HANDLE)m_pBuf[0];
        hAllocB = (HANDLE)m_pBuf[1];
    }

    m_pETWMaster->WaitForEvents(5000);

    // Check to see if AllocationA has been renamed by UMD driver
    pAllocationA = m_pETWMaster->GetAllocationTracker(hAllocA);
    
    if(!pAllocationA)
    {
        WriteToLog(_T("CUMDLoggingRenamedSurfaceTest::VerifyResults() - Can't map D3D resource %p (resource A) to kernel mode allocation"), hAllocA);
        return RESULT_FAIL;
    }

    if(pAllocationA->RenamedAlloc())
    {
        pUMDAllocInfoA = pAllocationA->GetUMDAllocInfo(hAllocA);
        assert(pUMDAllocInfoA);

        if(pAllocationA->RenamedAlloc() && !pAllocationA->RenamedAlloc()->bUMDRenaming)
        {
            WriteToLog(_T("CUMDLoggingRenamedSurfaceTest::VerifyResults() - Allocation A (D3D resource %p) wasn't renamed by UMD"), hAllocA);
            tr = RESULT_SKIP;
        }
        else
        {
            bAllocARenamedByUMD = true;
            // Make sure number of instances is not greater then TimesToRename+1
            pRenamedAllocA = pAllocationA->RenamedAlloc();
            assert(pRenamedAllocA);

            if(pRenamedAllocA && pRenamedAllocA->Count > (m_TimesToRename + 2)) // 1 initial allocation + 1 for fill with CPU (DISCARD) + Times to rename
            {
                WriteToLog(_T("CUMDLoggingRenamedSurfaceTest::VerifyResults() - Too many instances of Allocation A (%d) (D3D resource %p)"), pRenamedAllocA->Count, hAllocA);
                tr = RESULT_FAIL;
            }
        }
    }

    if(tr == RESULT_PASS)
    {
        // Check if Allocation B has been renamed by UMD driver
        pAllocationB = m_pETWMaster->GetAllocationTracker(hAllocB);
        
        if(!pAllocationB)
        {
            WriteToLog(_T("CUMDLoggingRenamedSurfaceTest::VerifyResults() - Can't map D3D resource %p (resource B) to kernel mode allocation"), hAllocB);
            tr == RESULT_FAIL;
        }

        if(pAllocationB && pAllocationB->RenamedAlloc())
        {
            pUMDAllocInfoB = pAllocationB->GetUMDAllocInfo(hAllocB);
            assert(pUMDAllocInfoB);
            pRenamedAllocB = pAllocationB->RenamedAlloc();
            assert(pRenamedAllocB);

            if(pRenamedAllocB && !pRenamedAllocB->bUMDRenaming)
            {
                WriteToLog(_T("CUMDLoggingRenamedSurfaceTest::VerifyResults() - Allocation B (D3D resource %p) wasn't renamed by UMD"), hAllocB);
                tr = RESULT_SKIP;
            }
            else
            {
                bAllocBRenamedByUMD = true;

                if(pRenamedAllocB && pRenamedAllocB->Count > (m_TimesToRename + 2)) // 1 initial allocation + 1 for fill with CPU (DISCARD) + Times to rename
                {
                    WriteToLog(_T("CUMDLoggingRenamedSurfaceTest::VerifyResults() - Too many instances of Allocation B (%d) (D3D resource %p)"), pRenamedAllocB->Count, hAllocB);
                    tr = RESULT_FAIL;
                }
            }
        }
    }

    if(tr == RESULT_PASS)
    {
        if(bAllocARenamedByUMD && bAllocBRenamedByUMD)
        {
            // Check instances of A and B to make sure UMD didn't reuse retired istances of A for B
            // without unmapping them first
            std::list<CAllocationTracker*>::iterator itA, itB;

            for(itA = pRenamedAllocA->pInstances.begin(); itA != pRenamedAllocA->pInstances.end(); itA++)
            {
                CAllocationTracker* pAllocA = *itA;

                for(itB = pRenamedAllocB->pInstances.begin(); itB != pRenamedAllocB->pInstances.end(); itB++)
                {
                    CAllocationTracker* pAllocB = *itB;

                    if(pAllocA->GlobalAlloc() == pAllocB->GlobalAlloc())
                    {
                        WriteToLog(_T("CUMDLoggingRenamedSurfaceTest::VerifyResults() - found same allocation in AllocA and AllocB instances, GlobalAlloc = 0x%X"), pAllocA->GlobalAlloc());
                        tr = RESULT_FAIL;
                    }
                }
            }
        }
    }

    if(!bAllocARenamedByUMD && !bAllocBRenamedByUMD)
    {
        WriteToLog(_T("CUMDLoggingRenamedSurfaceTest::VerifyResults() - no UMD renaming"));
        tr = RESULT_SKIP;
    }


    if(LogListenerErrors())
    {
        WriteToLog(_T("CUMDLoggingRenamedSurfaceTest::VerifyResults() - UMD logging errors detected"));
        tr = RESULT_FAIL;
    }


    return tr;
}

//+++++++++++++++++++++++++++++++++++++ COfferReclaimRenamedSurfaceTest ++++++++++++++++++++++++++++++++++
BEGIN_NAMED_VALUES(eOFFER_RECLAIM_RENAMED_SURFACE_TEST)
    NAMED_VALUE(_T("OFFER_RECLAIM_RENAMED_SURFACE_TEST_OFFERED"), OFFER_RECLAIM_RENAMED_SURFACE_TEST_OFFERED)
    NAMED_VALUE(_T("OFFER_RECLAIM_RENAMED_SURFACE_TEST_RECLAIMED"), OFFER_RECLAIM_RENAMED_SURFACE_TEST_RECLAIMED)
    NAMED_VALUE(_T("OFFER_RECLAIM_RENAMED_SURFACE_TEST_RETIRED"), OFFER_RECLAIM_RENAMED_SURFACE_TEST_RETIRED)
    NAMED_VALUE(_T("OFFER_RECLAIM_RENAMED_SURFACE_TEST_RETIRED_OFFERED"), OFFER_RECLAIM_RENAMED_SURFACE_TEST_RETIRED_OFFERED)
END_NAMED_VALUES(eOFFER_RECLAIM_RENAMED_SURFACE_TEST)

void COfferReclaimRenamedSurfaceTest::InitTestParameters()
{
    CRenamedSurfaceTest::InitTestParameters();

    CTestCaseParameter<eOFFER_RECLAIM_RENAMED_SURFACE_TEST> *pTestType = AddParameter<eOFFER_RECLAIM_RENAMED_SURFACE_TEST>(_T("TestType"), &m_TestType);
    CUserValueSet<eOFFER_RECLAIM_RENAMED_SURFACE_TEST> *pTestTypeValues = new CUserValueSet<eOFFER_RECLAIM_RENAMED_SURFACE_TEST>();
    pTestTypeValues->AddValue( OFFER_RECLAIM_RENAMED_SURFACE_TEST_OFFERED );
    pTestTypeValues->AddValue( OFFER_RECLAIM_RENAMED_SURFACE_TEST_RECLAIMED );
    pTestTypeValues->AddValue( OFFER_RECLAIM_RENAMED_SURFACE_TEST_RETIRED );
    pTestTypeValues->AddValue( OFFER_RECLAIM_RENAMED_SURFACE_TEST_RETIRED_OFFERED );

    testfactor::RFactor TestTypeFactor = AddParameterValueSet<eOFFER_RECLAIM_RENAMED_SURFACE_TEST>(pTestType, pTestTypeValues);
    m_Factor = m_Factor * TestTypeFactor;

    SetRootTestFactor(m_Factor);
}

TEST_RESULT COfferReclaimRenamedSurfaceTest::SetupTestCase()
{
    D3DKMT_DRIVERVERSION wddmDriverVersion = KMT_DRIVERVERSION_WDDM_1_2;

    DevX::GRFX::AdapterDescriptor *pAdapterDescriptor = DevX::GRFX::AdapterDescriptor::GetAdapterDescriptor(m_pDXGIAdapter1);
    if (nullptr == pAdapterDescriptor)
    {
        WriteToLog(_T("COfferReclaimRenamedSurfaceTest::SetupTestCase() - GetAdapterDescriptor failed"));
        return RESULT_BLOCKED;
    }

    if (FAILED(pAdapterDescriptor->GetDisplayDriverModel(wddmDriverVersion)))
    {
        WriteToLog(_T("COfferReclaimRenamedSurfaceTest::SetupTestCase() - GetDisplayDriverModel failed"));
        delete pAdapterDescriptor;
        return RESULT_BLOCKED;
    }

    //
    // This test is skipped on WDDM2.0 adapters and above due to relaxed Offer/Reclaim requirements for the driver.
    //
    if (wddmDriverVersion >= KMT_DRIVERVERSION_WDDM_2_0)
    {
        WriteToLog(_T("COfferReclaimRenamedSurfaceTest::SetupTestCase() - Test skipped on WDDM 2.0 and above"));
        delete pAdapterDescriptor;
        return RESULT_SKIP;
    }

    TEST_RESULT tr = CVidMMUMDTest::SetupTestCase();

    if (tr != RESULT_PASS)
    {
        CVidMMUMDTest::CleanupTestCase();
        return tr;
    }

    return tr;
}

TEST_RESULT COfferReclaimRenamedSurfaceTest::ExecuteTestCase()
{
    TEST_RESULT tr = RESULT_PASS;
    HRESULT hr = S_OK;

    StartETWTracking();

    if(m_ResourceType == WGFVIDMM_TEXTURE)
    {
        hr = RenameTexture(m_pTex[0], m_pShaderResourceViews[0]);
    }
    else if (m_ResourceType == WGFVIDMM_BUFFER)
    {
        hr = RenameBuffer(m_pBuf[0]);
    }

    if(FAILED(hr))
    {
        tr = RESULT_BLOCKED;
    }

    switch(m_TestType)
    {
    case OFFER_RECLAIM_RENAMED_SURFACE_TEST_RETIRED:
        // Present to make renamed instances of Allocation non-referenced
        g_App.setPresent(true);
        if(!GetFramework()->Present(NULL, NULL, NULL, NULL, 0, 0))
        {
            WriteToLog(_T("COfferReclaimRenamedSurfaceTest::ExecuteTestCase() - Present() (1) failed, which may indicate incorrect driver behavior. Please check dxgkdx log for any possible errors"));
            tr = RESULT_FAIL;
        }
        break;
    case OFFER_RECLAIM_RENAMED_SURFACE_TEST_RETIRED_OFFERED:
        // Present to have renamed instances of Allocation retired, then Offer.
        g_App.setPresent(true);
        if(!GetFramework()->Present(NULL, NULL, NULL, NULL, 0, 0))
        {
            WriteToLog(_T("COfferReclaimRenamedSurfaceTest::ExecuteTestCase() - Present() failed, which may indicate incorrect driver behavior. Please check dxgkdx log for any possible errors"));
            tr = RESULT_FAIL;
        }
        else
        {
            HRESULT hr = m_pDXGIDevice2->OfferResources(1, &m_pResources[0], DXGI_OFFER_RESOURCE_PRIORITY_NORMAL);
            if(FAILED(hr))
            {
                tr = RESULT_FAIL;
                WriteToLog(_T("COfferReclaimRenamedSurfaceTest::ExecuteTestCaseAll() - OfferResources() failed, hr = 0x%X"), hr);
            }
            // Flush here to make sure all offers get to kernel (to avoid UMD pipelining offers)
            GetEffectiveContext()->Flush();
        }
        break;
    case OFFER_RECLAIM_RENAMED_SURFACE_TEST_OFFERED:
        {
            HRESULT hr = m_pDXGIDevice2->OfferResources(1, &m_pResources[0], DXGI_OFFER_RESOURCE_PRIORITY_NORMAL);
            if(FAILED(hr))
            {
                tr = RESULT_FAIL;
                WriteToLog(_T("COfferReclaimRenamedSurfaceTest::ExecuteTestCaseAll() - OfferResources() failed, hr = 0x%X"), hr);
            }

            // Flush here to make sure all offers get to kernel (to avoid UMD pipelining offers)
            GetEffectiveContext()->Flush();
        }
        break;
    case OFFER_RECLAIM_RENAMED_SURFACE_TEST_RECLAIMED:
        {
            HRESULT hr = m_pDXGIDevice2->OfferResources(1, &m_pResources[0], DXGI_OFFER_RESOURCE_PRIORITY_NORMAL);
            if(FAILED(hr))
            {
                tr = RESULT_FAIL;
                WriteToLog(_T("COfferReclaimRenamedSurfaceTest::ExecuteTestCaseAll() - OfferResources() failed, hr = 0x%X"), hr);
            }

            // Flush here to make sure all offers get to kernel (to avoid UMD pipelining offers)
            GetEffectiveContext()->Flush();

            hr = m_pDXGIDevice2->ReclaimResources(1, &m_pResources[0], NULL); // don't check conent, since this is a dynamic allocation
            if(FAILED(hr))
            {
                tr = RESULT_FAIL;
                WriteToLog(_T("COfferReclaimRenamedSurfaceTest::ExecuteTestCaseAll() - ReclaimResources() failed, hr = 0x%X"), hr);
            }
        }
        break;
    }
    
    if(tr == RESULT_PASS)
        tr = VerifyResults();

    StopETWTracking(true);

    // We might fail VerifyResult due to failure to map allocation. In such case, we still want
    // to make sure we can use allocation after it was offered.
    TEST_RESULT trUseAfterOffer = RESULT_PASS;

    if(m_TestType != OFFER_RECLAIM_RENAMED_SURFACE_TEST_RECLAIMED && m_TestType != OFFER_RECLAIM_RENAMED_SURFACE_TEST_RETIRED)
    {
        hr = m_pDXGIDevice2->ReclaimResources(1, &m_pResources[0], NULL); // don't check conent, since this is a dynamic allocation
        if(FAILED(hr))
        {
            trUseAfterOffer = RESULT_FAIL;
            WriteToLog(_T("COfferReclaimRenamedSurfaceTest::ExecuteTestCaseAll() - ReclaimResources() failed, hr = 0x%X"), hr);
        }
    }

    if(trUseAfterOffer == RESULT_PASS)
    {
        // make sure we can use resources
        if(m_ResourceType == WGFVIDMM_TEXTURE)
        {
            hr = Fill2DTexture(0, RESOURCE_PATTERN_1);
        }
        else if (m_ResourceType == WGFVIDMM_BUFFER)
        {
            hr = FillBuffer(0, m_pBufferData1);
        }
        if(FAILED(hr))
        {
            trUseAfterOffer = RESULT_FAIL;
            WriteToLog(_T("COfferReclaimRenamedSurfaceTest::ExecuteTestCaseAll() - FillBuffer after reaclaim failed, hr = 0x%X"), hr);
        }
        else
        {
            bool bPresent = false;
            g_App.setPresent(true);

            if(m_ResourceType == WGFVIDMM_TEXTURE)
            {
                bPresent = m_PresentHelper.PresentTexture2D(m_pTex[0], m_pShaderResourceViews[0]);
            }
            else if (m_ResourceType == WGFVIDMM_BUFFER)
            {
                bPresent = m_PresentHelper.PresentBuffer(m_pBuf[0]);
            }

            if(!bPresent)
            {
                WriteToLog(_T("COfferReclaimRenamedSurfaceTest::ExecuteTestCaseAll() - Failed to present after Reclaim"));
                trUseAfterOffer = RESULT_FAIL;
            }
        }
    }

    // Log listener errors, but don't fail, because we don't want to fail on UMDLogging errors
    // or other errors not related to Offer/Reclaim
    LogListenerErrors();

    if(tr != RESULT_FAIL && trUseAfterOffer == RESULT_FAIL)
        tr = RESULT_FAIL;

    return tr;
}

TEST_RESULT COfferReclaimRenamedSurfaceTest::VerifyResults()
{
    TEST_RESULT tr = RESULT_PASS;

    CAllocationTracker *pAllocation = NULL, *pLatest = NULL;
    sUMD_ALLOC_INFO *pUMDAllocInfo = NULL, *pLatestUMDInfo = NULL;
    sRENAMED_ALLOC  *pRenamedAlloc = NULL;
    std::list<CAllocationTracker*>::iterator it;
    HANDLE hAlloc = NULL;

    if(m_ResourceType == WGFVIDMM_TEXTURE)
    {
        hAlloc = (HANDLE)m_pTex[0];
    }
    else
    {
        hAlloc = (HANDLE)m_pBuf[0];
    }

    m_pETWMaster->WaitForEvents(5000);

    // Check if Allocation has been renamed by UMD driver
    pAllocation = m_pETWMaster->GetAllocationTracker(hAlloc);
    
    if(!pAllocation)
    {
        WriteToLog(_T("COfferReclaimRenamedSurfaceTest::VerifyResults - Can't map D3D resource %p to kernel allocation"), hAlloc);
        return RESULT_BLOCKED;
    }

    pUMDAllocInfo = pAllocation->GetUMDAllocInfo(hAlloc);
    
    if(!pUMDAllocInfo)
    {
        // The reason we might end up here is if UMD did some incorrect remapping during renaming
        // if this is the case, we won't be able to perform verification of Offer/Reclaim behavior
        WriteToLog(_T("COfferReclaimRenamedSurfaceTest::VerifyResults - Can't find UMDAllocInfo for D3D resource %p"), hAlloc);
        tr = RESULT_BLOCKED;
    }
    else
    {
        if(!pAllocation->RenamedAlloc())
        {
            WriteToLog(_T("COfferReclaimRenamedSurfaceTest::VerifyResults - D3D resource %p hasn't been renamed"), hAlloc);
            tr = RESULT_SKIP;
        }
        else
        {
            if(pAllocation->RenamedAlloc()->bUMDRenaming)
            {
                WriteToLog(_T("COfferReclaimRenamedSurfaceTest::VerifyResults - D3D resource %p has been renamed by UMD driver"), hAlloc);
            }
            else if(pAllocation->RenamedAlloc()->bVidMMRenaming)
            {
                WriteToLog(_T("COfferReclaimRenamedSurfaceTest::VerifyResults - D3D resource %p has been renamed by VidMM"), hAlloc);
            }

            pRenamedAlloc = pAllocation->RenamedAlloc();
            assert(pRenamedAlloc);

            pLatest = pRenamedAlloc->pLatest;
            
            if(pLatest)
            {
                pLatestUMDInfo = pLatest->GetUMDAllocInfo(NULL);
                assert(pLatestUMDInfo);
            }
            else
            {
                // in case of UMD renaming we won't always know Latest instance
            }

            switch(m_TestType)
            {
            case OFFER_RECLAIM_RENAMED_SURFACE_TEST_RECLAIMED:
            case OFFER_RECLAIM_RENAMED_SURFACE_TEST_RETIRED:
                {
                    if(pLatest)
                    {
                        // all instances, except Latest, should be Destroyed or Offered (TODO: Or Terminated?)
                        if(pLatest->AllocInfo() && pLatest->AllocInfo()->OfferState != OFFER_STATE_NOT_OFFERED)
                        {
                            WriteToLog(_T("COfferReclaimRenamedSurfaceTest::VerifyResults() - Latest instance of GlobalAlloc %p is in Offer state (%d), which is not expected"), pLatest->GlobalAlloc(), pLatest->AllocInfo()->OfferState);
                            tr = RESULT_FAIL;
                        }
            
                        for(it = pRenamedAlloc->pInstances.begin(); it != pRenamedAlloc->pInstances.end(); it++)
                        {
                            CAllocationTracker* pAlloc = *it;

                            if(pAlloc->GlobalAlloc() != pLatest->GlobalAlloc())
                            {
                                if(pAlloc->AllocInfo() && pAlloc->AllocInfo()->OfferState == OFFER_STATE_NOT_OFFERED)
                                {
                                    WriteToLog(_T("COfferReclaimRenamedSurfaceTest::VerifyResults() - retired isntance of GlobalAlloc %p is not offered"), pLatest->GlobalAlloc());
                                    tr = RESULT_FAIL;
                                }
                            }
                        }
                    }
                    else
                    {
                        // make sure that cInstances-1 are offered
                        UINT cOfferedInstances = 0;
                        for(it = pRenamedAlloc->pInstances.begin(); it != pRenamedAlloc->pInstances.end(); it++)
                        {
                            CAllocationTracker* pAlloc = *it;

                            if(pAlloc->AllocInfo() && pAlloc->AllocInfo()->OfferState == OFFER_STATE_NOT_OFFERED)
                            {
                                WriteToLog(_T("INFO - COfferReclaimRenamedSurfaceTest::VerifyResults() - retired isntance of GlobalAlloc %p is not offered"), pAlloc->GlobalAlloc());
                            }
                            else
                                cOfferedInstances++;
                        }

                        if(cOfferedInstances != (pRenamedAlloc->Count - 1))
                        {
                            WriteToLog(_T("COfferReclaimRenamedSurfaceTest::VerifyResults() - Not all instances expected to be offered are offered (%d != %d)"), (pRenamedAlloc->Count - 1), cOfferedInstances);
                            tr = RESULT_FAIL;
                        }
                    }
                }
                break;
            case OFFER_RECLAIM_RENAMED_SURFACE_TEST_RETIRED_OFFERED:
            case OFFER_RECLAIM_RENAMED_SURFACE_TEST_OFFERED:
                {
                    // Resource has been offered, all instances should be offered
                    for(it = pRenamedAlloc->pInstances.begin(); it != pRenamedAlloc->pInstances.end(); it++)
                    {
                        CAllocationTracker* pAlloc = *it;

                        if(pAlloc->AllocInfo() && pAlloc->AllocInfo()->OfferState == OFFER_STATE_NOT_OFFERED)
                        {
                            WriteToLog(_T("COfferReclaimRenamedSurfaceTest::VerifyResults() - isntance of GlobalAlloc %p is not offered"), pAlloc->GlobalAlloc());
                            tr = RESULT_FAIL;
                        }
                    }
                }
                break;
            default:
                assert(0);
            }
        }
    }

    return tr;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//++++++++++++++++++++++++++++ CUMDLoggingCreateDestroyTest +++++++++++++++++++++++++++++++++++++
BEGIN_NAMED_VALUES(eCREATE_DESTROY_TEST)
    NAMED_VALUE(_T("CREATE_DESTROY_TEST_BASIC"), CREATE_DESTROY_TEST_BASIC)
    NAMED_VALUE(_T("CREATE_DESTROY_TEST_RECYCLE"), CREATE_DESTROY_TEST_RECYCLE)
    NAMED_VALUE(_T("CREATE_DESTROY_TEST_REUSE"), CREATE_DESTROY_TEST_REUSE)
END_NAMED_VALUES(eCREATE_DESTROY_TEST)

CUMDLoggingCreateDestroyTest::CUMDLoggingCreateDestroyTest()
{
    m_bVerifyUMDAllocInfo = true;
    OutputDebugString("CUMDLoggingCreateDestroyTest()\n");
}

CUMDLoggingCreateDestroyTest::~CUMDLoggingCreateDestroyTest()
{
    m_pETWMaster = NULL;
    OutputDebugString("~CUMDLoggingCreateDestroyTest()\n");
}


void CUMDLoggingCreateDestroyTest::InitTestParameters()
{
    CVidMMUMDTest::InitTestParameters();
    
    CTestCaseParameter<eCREATE_DESTROY_TEST> *pTestType = AddParameter<eCREATE_DESTROY_TEST>(_T("TestType"), &m_TestType);
    CTestCaseParameter<UINT> *pWidth = AddParameter<UINT>(_T("Width"), &m_Width);
    CTestCaseParameter<UINT> *pHeight = AddParameter<UINT>(_T("Height"), &m_Height);

    CUserValueSet<UINT> *pWidthBasic = new CUserValueSet<UINT>();
    pWidthBasic->AddValue(1);
    pWidthBasic->AddValue(220);
    pWidthBasic->AddValue(2048);

    CUserValueSet<UINT> *pWidthRecycle = new CUserValueSet<UINT>();
    pWidthRecycle->AddValue(64);
    pWidthRecycle->AddValue(512);

    CUserValueSet<UINT> *pHeightBasic = new CUserValueSet<UINT>();
    pHeightBasic->AddValue(1234);
    pHeightBasic->AddValue(1024);

    CUserValueSet<UINT> *pHeightRecycle = new CUserValueSet<UINT>();
    pHeightRecycle->AddValue(64);
    pHeightRecycle->AddValue(1024);

    testfactor::RFactor BasicFactor
        = AddParameterValue<eCREATE_DESTROY_TEST>(pTestType, CREATE_DESTROY_TEST_BASIC)
        * AddParameterValueSet<UINT>(pWidth, pWidthBasic)
        * AddParameterValueSet<UINT>(pHeight, pHeightBasic);

    testfactor::RFactor RecycleFactor
        = AddParameterValue<eCREATE_DESTROY_TEST>(pTestType, CREATE_DESTROY_TEST_RECYCLE)
        * AddParameterValueSet<UINT>(pWidth, pWidthRecycle)
        * AddParameterValueSet<UINT>(pHeight, pHeightRecycle);

    testfactor::RFactor AllFactor = BasicFactor + RecycleFactor;

    SetRootTestFactor(AllFactor);
}


TEST_RESULT CUMDLoggingCreateDestroyTest::SetupTestCase()
{
    TEST_RESULT tr = CVidMMUMDTest::SetupTestCase();

    if(tr != RESULT_PASS)
    {
        CVidMMUMDTest::CleanupTestCase();
        return tr;
    }

    switch(m_TestType)
    {
    case CREATE_DESTROY_TEST_BASIC: m_Count = 1; break;
    case CREATE_DESTROY_TEST_RECYCLE: m_Count = MAX_RESOURCE_COUNT; break;
    default: assert(0);
    }

    return tr;
}

TEST_RESULT CUMDLoggingCreateDestroyTest::ExecuteTestCase()
{
    TEST_RESULT tr = RESULT_PASS;

    StartETWTracking();

    for(UINT i = 0; i < m_Count; i++)
    {
        if(m_TestType == CREATE_DESTROY_TEST_BASIC)
        {
            HRESULT hr = Create2DTexture(i, D3D11_USAGE_DEFAULT);

            if(FAILED(hr))
            {
                WriteToLog(_T("CUMDLoggingCreateDestroyTest::ExecuteTestCase() - Failed to create texture, i = %d, hr = 0x%X"), i, hr);
                tr = RESULT_BLOCKED;
            }
        }
        else if(m_TestType == CREATE_DESTROY_TEST_RECYCLE)
        {
            HRESULT hr = Create2DTexture(0, D3D11_USAGE_DEFAULT);

            if(FAILED(hr))
            {
                WriteToLog(_T("CUMDLoggingCreateDestroyTest::ExecuteTestCase() - Failed to create texture, i = %d, hr = 0x%X"), i, hr);
                tr = RESULT_BLOCKED;
            }
            else
            {
                // Release texture after it's created. In case of "Recycle"
                // we might get into situation where UMD recycles the allocations
                // VidMmEtwListener checks in real time to make sure allocations are
                // not mapped multiple times without being unmapped first
                ClearIndex(0);
            }
        }
    }	

    StopETWTracking(false);
    
    if(tr == RESULT_PASS)
        tr = VerifyResults();

    if(LogListenerErrors())
    {
        WriteToLog(_T("CUMDLoggingCreateDestroyTest::VerifyResults() - UMD logging errors detected"));
        tr = RESULT_FAIL;
    }
    
    return tr;
}


TEST_RESULT CUMDLoggingCreateDestroyTest::VerifyResults()
{
    TEST_RESULT tr = RESULT_PASS;

    CAllocationTracker *pAllocation = NULL;
    sUMD_ALLOC_INFO *pUMDAllocInfo = NULL;

    for(UINT i = 0; i < m_Count; i++)
    {
        if(m_pTex[i])
        {
            pAllocation = m_pETWMaster->GetAllocationTracker(m_pTex[i]);
            if(!pAllocation)
            {
                WriteToLog(_T("CUMDLoggingCreateDestroyTest::VerifyResults() - No Allocation Logging"));
                return RESULT_FAIL;
            }

            pUMDAllocInfo = pAllocation->GetUMDAllocInfo(m_pTex[i]);
            assert(pAllocation);

            // Verify size 
            if(pUMDAllocInfo && pUMDAllocInfo->Size != m_SizeInBytes)
            {
                // TODO: Test Bug 147608. Disabling this check until this bug is fixed
                //WriteToLog(_T("CUMDLoggingCreateDestroyTest::VerifyResults() - allocation size specified by UMD doesn't match expected size (%d, %d)"),
                //                pUMDAllocInfo->Size,
                //                m_SizeInBytes);
                //tr = RESULT_FAIL;
            }
        }
    }

    return tr;
}



/////////////////////////////////////////////////////////////////////////////////////////////////
//++++++++++++++++++++++++++++ COfferReclaimCreateDestroyTest +++++++++++++++++++++++++++++++++++++
BEGIN_NAMED_VALUES(eALLOCATION_OFFER_PRIORITY)
    NAMED_VALUE(_T("OFFER_PRIORITY_LOW"), OFFER_PRIORITY_LOW)
    NAMED_VALUE(_T("OFFER_PRIORITY_NORMAL"), OFFER_PRIORITY_NORMAL)
    NAMED_VALUE(_T("OFFER_PRIORITY_HIGH"), OFFER_PRIORITY_HIGH)
END_NAMED_VALUES(eALLOCATION_OFFER_PRIORITY)


void COfferReclaimCreateDestroyTest::InitTestParameters()
{
    CVidMMUMDTest::InitTestParameters();

    CTestCaseParameter<eCREATE_DESTROY_TEST> *pTestType = AddParameter<eCREATE_DESTROY_TEST>(_T("TestType"), &m_TestType);
    CTestCaseParameter<eWGFVIDMM_RESOURCE_TYPE> *pResourceType = AddParameter<eWGFVIDMM_RESOURCE_TYPE>(_T("ResourceType"), &m_ResourceType);
    CTestCaseParameter<UINT> *pWidth = AddParameter<UINT>(_T("Width"), &m_Width);
    CTestCaseParameter<UINT> *pHeight = AddParameter<UINT>(_T("Height"), &m_Height);

    CUserValueSet<UINT> *pWidthBasic = new CUserValueSet<UINT>();
    pWidthBasic->AddValue(4);
    pWidthBasic->AddValue(390);
    pWidthBasic->AddValue(2048);

    CUserValueSet<UINT> *pWidthRecycle = new CUserValueSet<UINT>();
    pWidthBasic->AddValue(32);
    pWidthRecycle->AddValue(512);

    CUserValueSet<UINT> *pHeightBasic = new CUserValueSet<UINT>();
    pHeightBasic->AddValue(64);
    pHeightBasic->AddValue(1234);

    CUserValueSet<UINT> *pHeightRecycle = new CUserValueSet<UINT>();
    pHeightRecycle->AddValue(32);
    pHeightRecycle->AddValue(1024);

    CUserValueSet<UINT> *pWidthBufferBasic = new CUserValueSet<UINT>();
    pWidthBufferBasic->AddValue(16);
    pWidthBufferBasic->AddValue(1024);
    pWidthBufferBasic->AddValue(1234567);

    CUserValueSet<UINT> *pHeightBuffer = new CUserValueSet<UINT>();
    pHeightBuffer->AddValue(1);

    CUserValueSet<UINT> *pWidthBufferRecycle = new CUserValueSet<UINT>();
    pWidthBufferRecycle->AddValue(70000);
    pWidthBufferRecycle->AddValue(4 * 1024 * 1024);

    CTestCaseParameter<UINT> *pMipCount = AddParameter<UINT>(_T("MipLevels"), &m_MipCount);
    CUserValueSet<UINT> * pMipCountValues = new CUserValueSet<UINT>();
    pMipCountValues->AddValue(1);
    pMipCountValues->AddValue(4);

    CTestCaseParameter<bool> *pShared = AddParameter<bool>(_T("Shared"), &m_bShared);
    CUserValueSet<bool> * pSharedValues = new CUserValueSet<bool>();
    pSharedValues->AddValue(false);
    pSharedValues->AddValue(true);
    
    CTestCaseParameter<eALLOCATION_OFFER_PRIORITY> *pOfferPriority = AddParameter<eALLOCATION_OFFER_PRIORITY>(_T("OfferPriority"), &m_OfferPriority);
    CUserValueSet<eALLOCATION_OFFER_PRIORITY> *pOfferPriorityValues = new CUserValueSet<eALLOCATION_OFFER_PRIORITY>();
    pOfferPriorityValues->AddValue( OFFER_PRIORITY_LOW );
    pOfferPriorityValues->AddValue( OFFER_PRIORITY_NORMAL );
    pOfferPriorityValues->AddValue( OFFER_PRIORITY_HIGH );

    CTestCaseParameter<bool> *pReclaim = AddParameter<bool>(_T("Reclaim"), &m_bReclaim);
    CUserValueSet<bool> * pReclaimValues = new CUserValueSet<bool>();
    pReclaimValues->AddValue(false);
    pReclaimValues->AddValue(true);



    testfactor::RFactor BasicTextureFactor = (AddParameterValue<eWGFVIDMM_RESOURCE_TYPE>(pResourceType, WGFVIDMM_TEXTURE)
                                            * AddParameterValue<eCREATE_DESTROY_TEST>(pTestType, CREATE_DESTROY_TEST_BASIC)
                                            * AddParameterValueSet<bool>(pReclaim, pReclaimValues)
                                            * AddParameterValueSet<bool>(pShared, pSharedValues))
                                            % AddParameterValueSet<eALLOCATION_OFFER_PRIORITY>(pOfferPriority, pOfferPriorityValues)
                                            % AddParameterValueSet<UINT>(pWidth, pWidthBasic)
                                            % AddParameterValueSet<UINT>(pHeight, pHeightBasic)
                                            % AddParameterValueSet<UINT>(pMipCount, pMipCountValues);


    testfactor::RFactor RecycleTextureFactor = AddParameterValue<eWGFVIDMM_RESOURCE_TYPE>(pResourceType, WGFVIDMM_TEXTURE)
                                            * AddParameterValue<eCREATE_DESTROY_TEST>(pTestType, CREATE_DESTROY_TEST_RECYCLE)
                                            * AddParameterValueSet<bool>(pReclaim, pReclaimValues)
                                            * AddParameterValueSet<bool>(pShared, pSharedValues)
                                            % AddParameterValueSet<eALLOCATION_OFFER_PRIORITY>(pOfferPriority, pOfferPriorityValues) 
                                            % AddParameterValueSet<UINT>(pWidth, pWidthRecycle)
                                            % AddParameterValueSet<UINT>(pHeight, pHeightRecycle)
                                            % AddParameterValueSet<UINT>(pMipCount, pMipCountValues);

    testfactor::RFactor ReuseTextureFactor = AddParameterValue<eWGFVIDMM_RESOURCE_TYPE>(pResourceType, WGFVIDMM_TEXTURE)
                                            * AddParameterValue<eCREATE_DESTROY_TEST>(pTestType, CREATE_DESTROY_TEST_REUSE)
                                            * AddParameterValueSet<bool>(pShared, pSharedValues)
                                            % AddParameterValueSet<eALLOCATION_OFFER_PRIORITY>(pOfferPriority, pOfferPriorityValues) 
                                            % AddParameterValueSet<UINT>(pWidth, pWidthRecycle)
                                            % AddParameterValueSet<UINT>(pHeight, pHeightRecycle)
                                            % AddParameterValueSet<UINT>(pMipCount, pMipCountValues);


    testfactor::RFactor BasicBufferFactor = AddParameterValue<eWGFVIDMM_RESOURCE_TYPE>(pResourceType, WGFVIDMM_BUFFER)
                                            * AddParameterValue<eCREATE_DESTROY_TEST>(pTestType, CREATE_DESTROY_TEST_BASIC)
                                            * AddParameterValueSet<bool>(pReclaim, pReclaimValues)
                                            * AddParameterValueSet<bool>(pShared, pSharedValues)
                                            % AddParameterValueSet<eALLOCATION_OFFER_PRIORITY>(pOfferPriority, pOfferPriorityValues) 
                                            % AddParameterValueSet<UINT>(pWidth, pWidthBufferBasic)
                                            % AddParameterValueSet<UINT>(pHeight, pHeightBuffer);


    testfactor::RFactor RecycleBufferFactor = AddParameterValue<eWGFVIDMM_RESOURCE_TYPE>(pResourceType, WGFVIDMM_BUFFER)
                                            * AddParameterValue<eCREATE_DESTROY_TEST>(pTestType, CREATE_DESTROY_TEST_RECYCLE)
                                            * AddParameterValueSet<bool>(pReclaim, pReclaimValues)
                                            * AddParameterValueSet<bool>(pShared, pSharedValues)
                                            % AddParameterValueSet<eALLOCATION_OFFER_PRIORITY>(pOfferPriority, pOfferPriorityValues) 
                                            % AddParameterValueSet<UINT>(pWidth, pWidthBufferRecycle)
                                            % AddParameterValueSet<UINT>(pHeight, pHeightBuffer);

    testfactor::RFactor ReuseBufferFactor = AddParameterValue<eWGFVIDMM_RESOURCE_TYPE>(pResourceType, WGFVIDMM_BUFFER)
                                            * AddParameterValue<eCREATE_DESTROY_TEST>(pTestType, CREATE_DESTROY_TEST_REUSE)
                                            * AddParameterValueSet<bool>(pShared, pSharedValues)
                                            % AddParameterValueSet<eALLOCATION_OFFER_PRIORITY>(pOfferPriority, pOfferPriorityValues) 
                                            % AddParameterValueSet<UINT>(pWidth, pWidthBufferRecycle)
                                            % AddParameterValueSet<UINT>(pHeight, pHeightBuffer);

    testfactor::RFactor Factor = ReuseTextureFactor + ReuseBufferFactor + BasicTextureFactor + BasicBufferFactor + RecycleTextureFactor + RecycleBufferFactor;
    SetRootTestFactor(Factor);
}

TEST_RESULT COfferReclaimCreateDestroyTest::SetupTestCase()
{
    m_Count = MAX_RESOURCE_COUNT;

    if(m_bShared)
    {
        if(g_App.m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_10_1)
        {
            m_bNoKeyedMutex = true;
        }
    }

    TEST_RESULT tr = CVidMMUMDTest::SetupTestCase();

    if(tr != RESULT_PASS)
    {
        CVidMMUMDTest::CleanupTestCase();
        return tr;
    }

    if(tr == RESULT_PASS)
    {
        if(!m_PresentHelper.Setup(this))
        {
            WriteToLog(_T("CWGFOfferReclaimBasicTextureTest::SetupTestCase() - failed to setup PresentHelper"));
            tr = RESULT_BLOCKED;
        }
    }

    if(m_TestType == CREATE_DESTROY_TEST_RECYCLE || m_TestType == CREATE_DESTROY_TEST_REUSE)
        m_Count = 1;

    if((m_MipCount > 1) && (g_App.m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_10_0))
    {
        // Check if Width and Height are pow2, otherwise mip levels are not supported on FL9
        if(!IsPow2(m_Width) || !IsPow2(m_Height))
            m_MipCount = 1;
    }

    return tr;
}

TEST_RESULT COfferReclaimCreateDestroyTest::ExecuteReuseTestCase()
{
    TEST_RESULT tr = RESULT_PASS;
    HRESULT hr = S_OK;
    TCHAR strError[ERROR_SIZE];
    ZeroMemory(strError, sizeof(TCHAR) * ERROR_SIZE);
    bool bSharedProcessStarted = false;
    bool bRegenerateContent = false;

    StartETWTracking();

    if(m_bShared)
    {
        if(!m_pManagerSync)
        {
            m_pManagerSync = new CXProcSyncManager(WGFVIDMM_MAPPING_NAME, WGFVIDMM_TOTAL_SHARERS, sizeof(sWGFVIDMM_SHARED_DATA));

            if(!m_pManagerSync)
            {
                WriteToLog(_T("COfferReclaimCreateDestroyTest::ExecuteReuseTestCase - out of memory"));
                tr = RESULT_BLOCKED;
            }
        }
    }

    if(tr == RESULT_PASS)
    {
        // 1. Create resources
        if(m_ResourceType == WGFVIDMM_TEXTURE)
        {
            hr = Create2DTexture((UINT)0, D3D11_USAGE_DEFAULT, RESOURCE_PATTERN_1, m_bShared);
        }
        else if (m_ResourceType == WGFVIDMM_BUFFER)
        {
            hr = CreateBuffer((UINT)0, D3D11_USAGE_DEFAULT, m_bShared, m_pBufferData1);
        }

        if(FAILED(hr))
        {
            WriteToLog(_T("COfferReclaimCreateDestroyTest::ExecuteReuseTestCase() - failed to create resource, hr = 0x%X"), hr);
            tr = RESULT_BLOCKED;
        }
        else
        {
            // Fill resources
            ACQUIRE_MUTEX
            if(m_ResourceType == WGFVIDMM_TEXTURE)
            {
                hr = Fill2DTexture(0, RESOURCE_PATTERN_1);
            }
            else if (m_ResourceType == WGFVIDMM_BUFFER)
            {
                hr = FillBuffer(0, m_pBufferData1);
            }
            RELEASE_MUTEX

            if(FAILED(hr))
            {
                WriteToLog(_T("COfferReclaimCreateDestroyTest::ExecuteReuseTestCase() - failed to fill resource, hr = 0x%X"), hr);
                tr = RESULT_BLOCKED;
            }
        }
    }

    if(tr == RESULT_PASS && m_bShared)
    {
        if(!m_bNoKeyedMutex)
        {
            // Start app which will be Reclaiming resource Offered by Owner
            tr = StartChildProcess("-childCreateDestroyReuse");
        }
        else
        {
            tr = StartChildProcess("-childBasic");
        }

        if(tr != RESULT_PASS)
        {
            WriteToLog(_T("COfferReclaimCreateDestroyTest::ExecuteReuseTestCase() - failed to start child process"));
        }
        else
        {
            bSharedProcessStarted = true;
            Sleep(1000); // give it a chance to start up
            OutputDebugString("COfferReclaimCreateDestroyTest::ExecuteReuseTestCase() - successfully started child process, moving child to Running state\n");
            MANAGER_SIGNAL_TO_RUN
        }
    }

    if(tr == RESULT_PASS)
    {
        for(UINT i = 0; i < 4; i++)
        {
            if(bRegenerateContent)
            {
                // Fill resources
                ACQUIRE_MUTEX
                if(m_ResourceType == WGFVIDMM_TEXTURE)
                {
                    hr = Fill2DTexture(0, RESOURCE_PATTERN_1);
                }
                else if (m_ResourceType == WGFVIDMM_BUFFER)
                {
                    hr = FillBuffer(0, m_pBufferData1);
                }
                RELEASE_MUTEX

                if(FAILED(hr))
                {
                    WriteToLog(_T("COfferReclaimCreateDestroyTest::ExecuteReuseTestCase() - failed to fill resource, iteraton %d, hr = 0x%X"), i, hr);
                    tr = RESULT_BLOCKED;
                    break;
                }
            }


            // 2. Use resource to get it into memory
            bool bPresent = false;
            g_App.setPresent(true);

            ACQUIRE_MUTEX
            if(m_ResourceType == WGFVIDMM_TEXTURE)
            {
                bPresent = m_PresentHelper.PresentTexture2D(m_pTex[0], m_pShaderResourceViews[0]);
            }
            else if (m_ResourceType == WGFVIDMM_BUFFER)
            {
                bPresent = m_PresentHelper.PresentBuffer(m_pBuf[0]);
            }
            RELEASE_MUTEX

            if(!bPresent)
            {
                WriteToLog(_T("COfferReclaimCreateDestroyTest::ExecuteReuseTestCase() - Failed to present iteration %d"), i);
                tr = RESULT_FAIL;
                break;
            }
            OutputDebugString("COfferReclaimCreateDestroyTest::ExecuteReuseTestCase() - Used resources\n");			


            // 3. Offer/Reclaim resource
            DXGI_OFFER_RESOURCE_PRIORITY DXGIOfferPriority;

            switch(m_OfferPriority)
            {
            case OFFER_PRIORITY_LOW: DXGIOfferPriority = DXGI_OFFER_RESOURCE_PRIORITY_LOW; break;
            case OFFER_PRIORITY_NORMAL: DXGIOfferPriority = DXGI_OFFER_RESOURCE_PRIORITY_NORMAL; break;
            case OFFER_PRIORITY_HIGH: DXGIOfferPriority = DXGI_OFFER_RESOURCE_PRIORITY_HIGH; break;
            default: assert(0);
            }

            ACQUIRE_MUTEX
            hr = m_pDXGIDevice2->OfferResources(1, &m_pResources[0], DXGIOfferPriority);
            RELEASE_MUTEX

            if(FAILED(hr))
            {
                WriteToLog(_T("COfferReclaimCreateDestroyTest::ExecuteReuseTestCase() - failed to OfferResources(), iteration %d, hr = 0x%X"), i, hr);
                tr = RESULT_FAIL;
                break;
            }
            else
            {
                GetEffectiveContext()->Flush();

                // Validate that resources were offered with correct priorities
                tr = VerifyAllocationOffered(0);

                if(tr != RESULT_PASS)
                {
                    // Intentionally don't stop here as we might fail for allocation logging issues, in that case
                    // we'll at least get some stability testing
                }



                BOOL* pDiscarded = NULL;

                if(!!i)
                    pDiscarded = &m_pDiscarded[0];
                
                if(m_bShared && !m_bNoKeyedMutex)
                {
                    // Allow sharer to reclaim resource
                    DBGPrint(L"COfferReclaimCreateDestroyTest::ExecuteReuseTestCase() - Waiting for Client to Reclaim resources");
                    // Let sharer Offer/Reclaim
                    MANAGER_SIGNAL_AND_WAIT
                    DBGPrint(L"COfferReclaimCreateDestroyTest::ExecuteReuseTestCase() - Successfully waited for Client to Reclaim resources");

                    // assume that we need to regenerate content
                    bRegenerateContent = true;
                }
                else
                {
                    ACQUIRE_MUTEX
                    hr = m_pDXGIDevice2->ReclaimResources(1, &m_pResources[0], pDiscarded);
                    RELEASE_MUTEX

                    if(FAILED(hr))
                    {
                        WriteToLog(_T("COfferReclaimCreateDestroyTest::ExecuteReuseTestCase() - ReclaimResources failed, iteration %d, hr = 0x%X"), i, hr);
                        tr = RESULT_FAIL;
                        break;
                    }
 
                    if(pDiscarded && *pDiscarded == FALSE)
                    {
                        ACQUIRE_MUTEX
                        if(m_ResourceType == WGFVIDMM_TEXTURE)
                        {
                            hr = Check2DTexture((UINT)0, RESOURCE_PATTERN_1);
                        }
                        else if (m_ResourceType == WGFVIDMM_BUFFER)
                        {
                            hr = CheckBuffer((UINT)0, m_pBufferData1);
                        }
                        RELEASE_MUTEX

                        if(FAILED(hr))
                        {
                            WriteToLog(_T("COfferReclaimCreateDestroyTest::ExecuteReuseTestCase() - Content check failed, iteration %d, hr = 0x%X"), i, hr);
                            tr = RESULT_FAIL;
                            bRegenerateContent = true;
                            break;
                        }
                        else
                            bRegenerateContent = false;
                    }

                    if(!pDiscarded || *pDiscarded == TRUE)
                        bRegenerateContent = true;
                }

                // Try using allocation after reclaim if content is still valid
                if(tr == RESULT_PASS && !bRegenerateContent)
                {
                    GetEffectiveContext()->Flush();

                    tr = VerifyAllocationNotOffered(0);

                    if(tr != RESULT_PASS)
                    {
                        // Intentionally don't stop here as we might fail for allocation logging issues, in that case
                        // we'll at least get some stability testing
                    }

                    // Use resource after reclaim if possible
                    bool bPresent = true;
                    g_App.setPresent(true);

                    ACQUIRE_MUTEX
                    if(m_ResourceType == WGFVIDMM_TEXTURE)
                    {
                        bPresent = m_PresentHelper.PresentTexture2D(m_pTex[0], m_pShaderResourceViews[0]);
                    }
                    else if (m_ResourceType == WGFVIDMM_BUFFER)
                    {
                        bPresent = m_PresentHelper.PresentBuffer(m_pBuf[0]);
                    }
                    RELEASE_MUTEX

                    if(!bPresent)
                    {
                        WriteToLog(_T("COfferReclaimCreateDestroyTest::ExecuteReuseTestCase() - Failed to present after reclaim, iteration %d"), i);
                        tr = RESULT_FAIL;
                        break;
                    }				
                }

                OutputDebugString("COfferReclaimCreateDestroyTest::ExecuteReuseTestCase() - Offered/Reclaimed resources\n");
            }
        }
    }

    StopETWTracking(true);

    // 4. Destroy resource
    ClearIndex(0);
    GetEffectiveContext()->Flush();

    if(bSharedProcessStarted)
    {
        OutputDebugString("COfferReclaimCreateDestroyTest::ExecuteReuseTestCase() - Signaled client, waiting for response\n");	
        MANAGER_SIGNAL_AND_WAIT
        OutputDebugString("COfferReclaimCreateDestroyTest::ExecuteReuseTestCase() - Telling client to terminate\n");
        MANAGER_SIGNAL_TO_TERMINATE
    }

    if(m_bShared)
    {
        if(m_pManagerSync)
        {
            delete m_pManagerSync;
            m_pManagerSync = NULL;
        }

        bSharedProcessStarted = false;
    }

    // log listener errors to help with debugging issues
    LogListenerErrors();
  
    if(m_pManagerSync && bSharedProcessStarted)
    {
        MANAGER_SIGNAL_AND_WAIT
        MANAGER_SIGNAL_TO_TERMINATE
    }

    return tr;

MacroFailed:
    // We can only get here if ACQUIRE_MUTEX or RELEASE_MUTEX produced a failure.
    // log the failure and return the error
    if(m_pManagerSync && bSharedProcessStarted)
    {
        MANAGER_SIGNAL_TO_TERMINATE
    }

    WriteToLog(strError);

    return tr;
}

TEST_RESULT COfferReclaimCreateDestroyReuseChild::ExecuteTestCase()
{
    TEST_RESULT tr = RESULT_PASS;
    STATE_TYPE state = eStateUnknown;
    HRESULT hr = S_OK;
    TCHAR strError[ERROR_SIZE];
    ZeroMemory(strError, sizeof(TCHAR) * ERROR_SIZE);

    DBGPrint(L"COfferReclaimCreateDestroyReuseChild::ExecuteTestCase() - Entering\n");

    DBGPrint(L"COfferReclaimCreateDestroyReuseChild::ExecuteTestCase() - Client is waiting for Manager's signal\n");
    CLIENT_WAIT

    for(UINT i = 0; i < 4; i++)
    {
        BOOL bDiscarded = false;

        DBGPrint(L"COfferReclaimCreateDestroyReuseChild::ExecuteTestCase() - Reclaiming Resource");
        ACQUIRE_MUTEX
        hr = m_pDXGIDevice2->ReclaimResources(1, &m_pResources[0], &bDiscarded);
        RELEASE_MUTEX

        if(FAILED(hr))
        {
            _stprintf_s(strError, ERROR_SIZE, _T("COfferReclaimCreateDestroyReuseChild::ExecuteReuseTestCase() - ReclaimResources failed, iteration %d, hr = 0x%X"), i, hr);
            tr = RESULT_FAIL;
            goto MacroFailed;
        }	

        DBGPrint(L"COfferReclaimCreateDestroyReuseChild::ExecuteTestCase() - signaling Owner");
        CLIENT_SIGNAL
        DBGPrint(L"COfferReclaimCreateDestroyReuseChild::ExecuteTestCase() - Waiting on Owner's signal");
        CLIENT_WAIT
    }

MacroFailed:
    DBGPrint(L"COfferReclaimCreateDestroyReuseChild::ExecuteTestCase() - Client is done and is signalign the Manager\n");
    CLIENT_SIGNAL

    // This is our normal exit
    if(tr != RESULT_PASS)
    {
        DBGPrint(L"COfferReclaimCreateDestroyReuseChild::ExecuteTestCase() - Acquiring lock to write our status\n");
        if(m_pClientSync->AcquireLock())
        {
            sWGFVIDMM_SHARED_DATA *pSharedData = (sWGFVIDMM_SHARED_DATA*)m_pClientSync->GetExtraDataPtr();
            memcpy(pSharedData->strError, strError, sizeof(pSharedData->strError));
            pSharedData->tr = tr;
            m_pClientSync->ReleaseLock();
            DBGPrint(L"COfferReclaimCreateDestroyReuseChild::ExecuteTestCase() - Released lock after writing our status\n");
        }
        else
            assert(0);
    }

    DBGPrint(L"COfferReclaimCreateDestroyReuseChild::ExecuteTestCase() - Exiting\n");
    return tr;
}

TEST_RESULT COfferReclaimCreateDestroyTest::ExecuteRecycleTestCase()
{
    TEST_RESULT tr = RESULT_PASS;
    HRESULT hr = S_OK;
    TCHAR strError[ERROR_SIZE];
    ZeroMemory(strError, sizeof(TCHAR) * ERROR_SIZE);
    bool bSharedProcessStarted = false;

    StartETWTracking();

    for(UINT i = 0; i < 4; i++)
    {

        if(m_bShared)
        {
            if(!m_pManagerSync)
            {
                m_pManagerSync = new CXProcSyncManager(WGFVIDMM_MAPPING_NAME, WGFVIDMM_TOTAL_SHARERS, sizeof(sWGFVIDMM_SHARED_DATA));

                if(!m_pManagerSync)
                {
                    WriteToLog(_T("COfferReclaimCreateDestroyTest::ExecuteRecycleTestCase - out of memory"));
                    tr = RESULT_BLOCKED;
                    break;
                }
            }
        }

        // 1. Create resources
        if(m_ResourceType == WGFVIDMM_TEXTURE)
        {
            hr = Create2DTexture((UINT)0, D3D11_USAGE_DEFAULT, RESOURCE_PATTERN_1, m_bShared);
        }
        else if (m_ResourceType == WGFVIDMM_BUFFER)
        {
            hr = CreateBuffer((UINT)0, D3D11_USAGE_DEFAULT, m_bShared, m_pBufferData1);
        }

        if(FAILED(hr))
        {
            WriteToLog(_T("COfferReclaimCreateDestroyTest::ExecuteRecycleTestCase() - failed to create resource %d, hr = 0x%X"), i, hr);
            tr = RESULT_BLOCKED;
            break;
        }
        else
        {
            // Fill resources
            ACQUIRE_MUTEX
            if(m_ResourceType == WGFVIDMM_TEXTURE)
            {
                hr = Fill2DTexture(0, RESOURCE_PATTERN_1);
            }
            else if (m_ResourceType == WGFVIDMM_BUFFER)
            {
                hr = FillBuffer(0, m_pBufferData1);
            }
            RELEASE_MUTEX

            if(FAILED(hr))
            {
                WriteToLog(_T("COfferReclaimCreateDestroyTest::ExecuteRecycleTestCase() - failed to fill resource %d, hr = 0x%X"), i, hr);
                tr = RESULT_BLOCKED;
                break;
            }
        }


        if(tr == RESULT_PASS && m_bShared)
        {
            // Start our client app
            tr = StartChildProcess("-childBasic");

            if(tr != RESULT_PASS)
            {
                WriteToLog(_T("COfferReclaimCreateDestroyTest::ExecuteRecycleTestCase() - failed to start child process"));
            }
            else
            {
                bSharedProcessStarted = true;
                Sleep(1000); // give it a chance to start up
                OutputDebugString("COfferReclaimCreateDestroyTest::ExecuteRecycleTestCase() - successfully started child process, moving child to Running state\n");
                MANAGER_SIGNAL_TO_RUN
            }
        }

        // 2. Use resource to get it into memory
        if(tr == RESULT_PASS)
        {
            bool bPresent = false;
            g_App.setPresent(true);

            ACQUIRE_MUTEX
            if(m_ResourceType == WGFVIDMM_TEXTURE)
            {
                bPresent = m_PresentHelper.PresentTexture2D(m_pTex[0], m_pShaderResourceViews[0]);
            }
            else if (m_ResourceType == WGFVIDMM_BUFFER)
            {
                bPresent = m_PresentHelper.PresentBuffer(m_pBuf[0]);
            }
            RELEASE_MUTEX

            if(!bPresent)
            {
                WriteToLog(_T("COfferReclaimCreateDestroyTest::ExecuteRecycleTestCase() - Failed to present iteration %d"), i);
                tr = RESULT_FAIL;
                break;
            }
            OutputDebugString("COfferReclaimCreateDestroyTest::ExecuteRecycleTestCase() - Used resources\n");			
        }


        // 3. Offer/Reclaim resource
        if(tr == RESULT_PASS)
        {
            // Before offering, make sure allocations are not already offered. We might catch driver recycling
            // our allocations from previous iterations without reclaiming them first.
            tr = VerifyAllocationNotOffered(0);

            if(tr != RESULT_PASS)
            {
                // Intentionally don't stop here as we might fail for allocation logging issues, in that case
                // we'll at least get some stability testing
            }
            DXGI_OFFER_RESOURCE_PRIORITY DXGIOfferPriority;

            switch(m_OfferPriority)
            {
            case OFFER_PRIORITY_LOW: DXGIOfferPriority = DXGI_OFFER_RESOURCE_PRIORITY_LOW; break;
            case OFFER_PRIORITY_NORMAL: DXGIOfferPriority = DXGI_OFFER_RESOURCE_PRIORITY_NORMAL; break;
            case OFFER_PRIORITY_HIGH: DXGIOfferPriority = DXGI_OFFER_RESOURCE_PRIORITY_HIGH; break;
            default: assert(0);
            }

            ACQUIRE_MUTEX
            hr = m_pDXGIDevice2->OfferResources(1, &m_pResources[0], DXGIOfferPriority);
            RELEASE_MUTEX

            if(FAILED(hr))
            {
                WriteToLog(_T("COfferReclaimCreateDestroyTest::ExecuteRecycleTestCase() - failed to OfferResources(), hr = 0x%X"), hr);
                tr = RESULT_FAIL;
            }
            else
            {
                GetEffectiveContext()->Flush();

                // Validate that resources were offered with correct priorities
                tr = VerifyAllocationOffered(0);

                if(tr != RESULT_PASS)
                {
                    // Intentionally don't stop here as we might fail for allocation logging issues, in that case
                    // we'll at least get some stability testing
                }

                if(m_bReclaim)
                {
                    ACQUIRE_MUTEX
                    if(m_ResourceType == WGFVIDMM_TEXTURE)
                    {
                        hr = Reclaim2DResource(0, true, RESOURCE_PATTERN_1);
                    }
                    else if (m_ResourceType == WGFVIDMM_BUFFER)
                    {
                        hr = ReclaimBuffer(0, true, m_pBufferData1);
                    }
                    RELEASE_MUTEX	

                    if(FAILED(hr))
                    {
                        WriteToLog(_T("COfferReclaimCreateDestroyTest::ExecuteRecycleTestCase() - failed to ReclaimResources(), hr = 0x%X"), hr);
                        tr = RESULT_FAIL;
                    }
                    else
                    {
                        GetEffectiveContext()->Flush();

                        if(tr == RESULT_PASS)
                        {
                            tr = VerifyAllocationNotOffered(0);

                            if(tr != RESULT_PASS)
                            {
                                // Intentionally don't stop here as we might fail for allocation logging issues, in that case
                                // we'll at least get some stability testing
                            }
                        }

                        // Use resource after reclaim if possible
                        bool bPresent = true;;
                        g_App.setPresent(true);

                        ACQUIRE_MUTEX
                        if(m_ResourceType == WGFVIDMM_TEXTURE)
                        {
                            if(!m_pDiscarded[0])
                                bPresent = m_PresentHelper.PresentTexture2D(m_pTex[0], m_pShaderResourceViews[0]);
                        }
                        else if (m_ResourceType == WGFVIDMM_BUFFER)
                        {
                            if(!m_pDiscarded[0])
                                bPresent = m_PresentHelper.PresentBuffer(m_pBuf[0]);
                        }
                        RELEASE_MUTEX

                        if(!bPresent)
                        {
                            WriteToLog(_T("COfferReclaimCreateDestroyTest::ExecuteRecycleTestCase() - Failed to present after reclaim, iteration %d"), i);                  
                            tr = RESULT_FAIL;
                            break;
                        }				
                    }
                }
            }

            OutputDebugString("COfferReclaimCreateDestroyTest::ExecuteRecycleTestCase() - Offered/Reclaimed resources\n");	
        }

        // 4. Destroy resource
        ClearIndex(0);
        GetEffectiveContext()->Flush();

        if(bSharedProcessStarted)
        {
            OutputDebugString("COfferReclaimCreateDestroyTest::ExecuteRecycleTestCase() - Signaled client, waiting for response\n");	
            MANAGER_SIGNAL_AND_WAIT
            OutputDebugString("COfferReclaimCreateDestroyTest::ExecuteRecycleTestCase() - Telling client to terminate\n");
            MANAGER_SIGNAL_TO_TERMINATE
            // Sleep a little but to give Child process chance to clean up shared memory
            Sleep(3000);
        }

        if(m_bShared)
        {
            if(m_pManagerSync)
            {
                delete m_pManagerSync;
                m_pManagerSync = NULL;
            }

            bSharedProcessStarted = false;
        }

    }

    StopETWTracking(true);

    // log listener errors to help with debugging issues
    LogListenerErrors();
  
    if(m_pManagerSync && bSharedProcessStarted)
    {
        MANAGER_SIGNAL_AND_WAIT
        MANAGER_SIGNAL_TO_TERMINATE
    }

    return tr;

MacroFailed:
    // We can only get here if ACQUIRE_MUTEX or RELEASE_MUTEX produced a failure.
    // log the failure and return the error
    if(m_pManagerSync && bSharedProcessStarted)
    {
        MANAGER_SIGNAL_TO_TERMINATE
    }

    WriteToLog(strError);

    return tr;
}

TEST_RESULT COfferReclaimCreateDestroyTest::ExecuteBasicTestCase()
{
    TEST_RESULT tr = RESULT_PASS;
    HRESULT hr = S_OK;
    bool bSharedProcessStarted = false;
    TCHAR strError[ERROR_SIZE];
    ZeroMemory(strError, sizeof(TCHAR) * ERROR_SIZE);

    StartETWTracking();

    // 1. Create resources
    for(UINT i = 0; i < m_Count; i++)
    {
        if(m_ResourceType == WGFVIDMM_TEXTURE)
        {
            hr = Create2DTexture(i, D3D11_USAGE_DEFAULT, RESOURCE_PATTERN_1, m_bShared);
        }
        else if (m_ResourceType == WGFVIDMM_BUFFER)
        {
            hr = CreateBuffer(i, D3D11_USAGE_DEFAULT, m_bShared, m_pBufferData1);
        }

        if(FAILED(hr))
        {
            WriteToLog(_T("COfferReclaimCreateDestroyTest::ExecuteBasicTestCase() - failed to create resource %d, hr = 0x%X"), i, hr);
            tr = RESULT_BLOCKED;
            break;
        }
        else
        {
            // Fill resources
            ACQUIRE_MUTEX_FOR_SINGLE_RESOURCE(i)
            if(m_ResourceType == WGFVIDMM_TEXTURE)
            {
                hr = Fill2DTexture(i, RESOURCE_PATTERN_1);
            }
            else if (m_ResourceType == WGFVIDMM_BUFFER)
            {
                hr = FillBuffer(i, m_pBufferData1);
            }
            RELEASE_MUTEX_FOR_SINGLE_RESOURCE(i)

            if(FAILED(hr))
            {
                WriteToLog(_T("COfferReclaimCreateDestroyTest::ExecuteBasicTestCase() - failed to fill resource %d, hr = 0x%X"), i, hr);
                tr = RESULT_BLOCKED;
                break;
            }
        }
    }

    OutputDebugString("COfferReclaimCreateDestroyTest::ExecuteBasicTestCase() - Created resources\n");

    //OutputDebugString("!!!!!!!!!!!!!!! Alloc list After creation !!!!!!!!!!!!!!!!!!!!!!!!!!");
    //m_pETWMaster->WaitForEvents(5000);
    //m_pETWMaster->PrintAllocationMap();	

    if(tr == RESULT_PASS && m_bShared)
    {
        OutputDebugString("COfferReclaimCreateDestroyTest::ExecuteBasicTestCase() - starting child process\n");
        // Start our client app
        tr = StartChildProcess("-childBasic");

        if(tr != RESULT_PASS)
        {
            WriteToLog(_T("COfferReclaimCreateDestroyTest::ExecuteBasicTestCase() - failed to start child process"));
            OutputDebugString("COfferReclaimCreateDestroyTest::ExecuteBasicTestCase() - Failed to start Child process\n");
        }
        else
        {
            bSharedProcessStarted = true;
            Sleep(1000); // give it a chance to start up
            OutputDebugString("COfferReclaimCreateDestroyTest::ExecuteBasicTestCase() - successfully started child process, moving child to Running state\n");
            MANAGER_SIGNAL_TO_RUN
        }
    }

    // 2. Use resources to get them into memory
    if(tr == RESULT_PASS)
    {
        for(UINT i = 0; i < m_Count; i++)
        {
            bool bPresent = false;
            g_App.setPresent(true);

            ACQUIRE_MUTEX
            if(m_ResourceType == WGFVIDMM_TEXTURE)
            {
                bPresent = m_PresentHelper.PresentTexture2D(m_pTex[i], m_pShaderResourceViews[i]);
            }
            else if (m_ResourceType == WGFVIDMM_BUFFER)
            {
                bPresent = m_PresentHelper.PresentBuffer(m_pBuf[i]);
            }
            RELEASE_MUTEX

            if(!bPresent)
            {
                WriteToLog(_T("COfferReclaimCreateDestroyTest::ExecuteBasicTestCase() - Failed to present iteration %d"), i);
                tr = RESULT_FAIL;
                break;
            }
        }
        OutputDebugString("COfferReclaimCreateDestroyTest::ExecuteBasicTestCase() - Used resources\n");
    }

        //OutputDebugString("!!!!!!!!!!!!!!! Alloc list After Use !!!!!!!!!!!!!!!!!!!!!!!!!!");
        //m_pETWMaster->WaitForEvents(5000);
        //m_pETWMaster->PrintAllocationMap();

    // 3. Offer/Reclaim resources
    if(tr == RESULT_PASS)
    {
        // Before offering, make sure allocations are not already offered. We might catch driver recycling
        // our allocations from previous iterations without reclaiming them first.
        tr = VerifyAllocationsNotOffered();

        if(tr != RESULT_PASS)
        {
            // Intentionally don't stop here as we might fail for allocation logging issues, in that case
            // we'll at least get some stability testing
        }
        DXGI_OFFER_RESOURCE_PRIORITY DXGIOfferPriority;

        switch(m_OfferPriority)
        {
        case OFFER_PRIORITY_LOW: DXGIOfferPriority = DXGI_OFFER_RESOURCE_PRIORITY_LOW; break;
        case OFFER_PRIORITY_NORMAL: DXGIOfferPriority = DXGI_OFFER_RESOURCE_PRIORITY_NORMAL; break;
        case OFFER_PRIORITY_HIGH: DXGIOfferPriority = DXGI_OFFER_RESOURCE_PRIORITY_HIGH; break;
        default: assert(0);
        }

        ACQUIRE_MUTEX
        hr = m_pDXGIDevice2->OfferResources(m_Count, m_pResources, DXGIOfferPriority);
        RELEASE_MUTEX

        if(FAILED(hr))
        {
            WriteToLog(_T("COfferReclaimCreateDestroyTest::ExecuteBasicTestCase() - failed to OfferResources(), hr = 0x%X"), hr);
            tr = RESULT_FAIL;
        }
        else
        {
            GetEffectiveContext()->Flush();
        //OutputDebugString("!!!!!!!!!!!!!!! Alloc list After Offer !!!!!!!!!!!!!!!!!!!!!!!!!!");
        //m_pETWMaster->WaitForEvents(5000);
        //m_pETWMaster->PrintAllocationMap();

            // Validate that resources were offered with correct priorities
            tr = VerifyAllocationsOffered();

            if(tr != RESULT_PASS)
            {
                // Intentionally don't stop here as we might fail for allocation logging issues, in that case
                // we'll at least get some stability testing
            }

            if(m_bReclaim)
            {
                ACQUIRE_MUTEX
                if(m_ResourceType == WGFVIDMM_TEXTURE)
                {
                    hr = Reclaim2DResources(true, RESOURCE_PATTERN_1);
                }
                else if (m_ResourceType == WGFVIDMM_BUFFER)
                {
                    hr = ReclaimBuffers(true, m_pBufferData1);
                }
                RELEASE_MUTEX		

                if(FAILED(hr))
                {
                    WriteToLog(_T("COfferReclaimCreateDestroyTest::ExecuteBasicTestCase() - failed to ReclaimResources(), hr = 0x%X"), hr);
                    tr = RESULT_FAIL;
                }
                else
                {
                    GetEffectiveContext()->Flush();

                    if(tr == RESULT_PASS)
                    {
                        tr = VerifyAllocationsNotOffered();

                        if(tr != RESULT_PASS)
                        {
                            // Intentionally don't stop here as we might fail for allocation logging issues, in that case
                            // we'll at least get some stability testing
                        }
                    }

                    // Use resources after reclaim when possible
                    for(UINT i = 0; i < m_Count; i++)
                    {
                        bool bPresent = true;
                        g_App.setPresent(true);

                        ACQUIRE_MUTEX
                        if(m_ResourceType == WGFVIDMM_TEXTURE)
                        {
                            if(!m_pDiscarded[0])
                                bPresent = m_PresentHelper.PresentTexture2D(m_pTex[i], m_pShaderResourceViews[i]);
                        }
                        else if (m_ResourceType == WGFVIDMM_BUFFER)
                        {
                            if(!m_pDiscarded[0])
                                bPresent = m_PresentHelper.PresentBuffer(m_pBuf[i]);
                        }
                        RELEASE_MUTEX

                        if(!bPresent)
                        {
                            WriteToLog(_T("COfferReclaimCreateDestroyTest::ExecuteBasicTestCase() - Failed to present after reclaim, iteration %d"), i);                  
                            tr = RESULT_FAIL;
                            break;
                        }
                    }					
                }
            }
        }
        OutputDebugString("COfferReclaimCreateDestroyTest::ExecuteBasicTestCase() - Offered/Reclaimed resources\n");
    }

    // 4. Destroy resources
    for(UINT i = 0; i < m_Count; i++)
    {
        ClearIndex(i);
    }
    GetEffectiveContext()->Flush();
    OutputDebugString("COfferReclaimCreateDestroyTest::ExecuteBasicTestCase() - Destroyed resources\n");


    StopETWTracking(true);

    // log listener errors to help with debugging issues
    LogListenerErrors();

    if(bSharedProcessStarted)
    {
        OutputDebugString("COfferReclaimCreateDestroyTest::ExecuteBasicTestCase() - Signaling client and waiting\n");
        MANAGER_SIGNAL_AND_WAIT
        MANAGER_SIGNAL_TO_TERMINATE
    }

    return tr;

MacroFailed:
    // We can only get here if ACQUIRE_MUTEX or RELEASE_MUTEX produced a failure.
    // log the failure and return the error
    if(bSharedProcessStarted)
    {
        OutputDebugString("COfferReclaimCreateDestroyTest::ExecuteBasicTestCase() - Moving Client to Terminate state\n");
        MANAGER_SIGNAL_TO_TERMINATE
    }

    WriteToLog(strError);

    return tr;
}


TEST_RESULT COfferReclaimCreateDestroyTest::ExecuteTestCase()
{
    TEST_RESULT tr = RESULT_PASS;
  
    switch(m_TestType)
    {
    case CREATE_DESTROY_TEST_BASIC: tr = ExecuteBasicTestCase(); break;
    case CREATE_DESTROY_TEST_RECYCLE: tr = ExecuteRecycleTestCase(); break;
    case CREATE_DESTROY_TEST_REUSE: tr = ExecuteReuseTestCase(); break;
    default: assert(0);
    }

    return tr;
}


TEST_RESULT COfferReclaimCreateDestroyTest::VerifyAllocationNotOffered(UINT Index)
{
    TEST_RESULT tr = RESULT_PASS;
    CAllocationTracker *pAllocation = NULL;

    m_pETWMaster->WaitForEvents(5000);

    if(m_ResourceType == WGFVIDMM_TEXTURE)
        pAllocation = m_pETWMaster->GetAllocationTracker(m_pTex[Index]);
    else if (m_ResourceType == WGFVIDMM_BUFFER)
        pAllocation = m_pETWMaster->GetAllocationTracker(m_pBuf[Index]);

    if(!pAllocation)
    {
        WriteToLog(_T("COfferReclaimCreateDestroyTest::VerifyAllocationsNotOffered() - Can't find allocation, possibly due to no allocation logging"));
        return RESULT_BLOCKED;
    }

    // Verify that allocation is not offered
    if(pAllocation->AllocInfo()->OfferPriority != 0 || pAllocation->AllocInfo()->OfferState != OFFER_STATE_NOT_OFFERED)
    {
        WriteToLog(_T("COfferReclaimCreateDestroyTest::VerifyAllocationsNotOffered() - allocation (index %d) is offered when it is not expected to be offered: OfferState = %d, OfferPriority = %d"), 
                    Index, 
                    pAllocation->AllocInfo()->OfferState, 
                    pAllocation->AllocInfo()->OfferPriority);
        tr = RESULT_FAIL;
    }

    return tr;
}

TEST_RESULT COfferReclaimCreateDestroyTest::VerifyAllocationsNotOffered()
{
    TEST_RESULT tr = RESULT_PASS;
    CAllocationTracker *pAllocation = NULL;

    m_pETWMaster->WaitForEvents(5000);

    for(UINT i = 0; i < m_Count; i++)
    {
        if(m_ResourceType == WGFVIDMM_TEXTURE)
            pAllocation = m_pETWMaster->GetAllocationTracker(m_pTex[i]);
        else if (m_ResourceType == WGFVIDMM_BUFFER)
            pAllocation = m_pETWMaster->GetAllocationTracker(m_pBuf[i]);

        if(!pAllocation)
        {
            WriteToLog(_T("COfferReclaimCreateDestroyTest::VerifyAllocationsNotOffered() - Can't find allocation, possibly due to no allocation logging"));
            return RESULT_BLOCKED;
        }

        // Verify that allocation is not offered
        if(pAllocation->AllocInfo()->OfferPriority != 0 || pAllocation->AllocInfo()->OfferState != OFFER_STATE_NOT_OFFERED)
        {
            WriteToLog(_T("COfferReclaimCreateDestroyTest::VerifyAllocationsNotOffered() - allocation (index %d) is offered when it is not expected to be offered: OfferState = %d, OfferPriority = %d"), 
                        i, 
                        pAllocation->AllocInfo()->OfferState, 
                        pAllocation->AllocInfo()->OfferPriority);
            tr = RESULT_FAIL;
            break;
        }
    }

    return tr;
}


TEST_RESULT COfferReclaimCreateDestroyTest::VerifyAllocationOffered(UINT Index)
{
    TEST_RESULT tr = RESULT_PASS;
    CAllocationTracker *pAllocation = NULL;
    sUMD_ALLOC_INFO* pUMDInfo = NULL;

    m_pETWMaster->WaitForEvents(5000);

    HANDLE hAlloc = NULL;

    if(m_ResourceType == WGFVIDMM_TEXTURE)
    {
        hAlloc = m_pTex[Index];
        pAllocation = m_pETWMaster->GetAllocationTracker(m_pTex[Index]);
    }
    else if (m_ResourceType == WGFVIDMM_BUFFER)
    {
        hAlloc = m_pBuf[Index];
        pAllocation = m_pETWMaster->GetAllocationTracker(m_pBuf[Index]);
    }

    if(!pAllocation)
    {
        WriteToLog(_T("COfferReclaimCreateDestroyTest::VerifyAllocationOffered() - Can't find allocation 0x%p, possibly due to no allocation logging"), hAlloc);
        return RESULT_BLOCKED;
    }

    if(m_ResourceType == WGFVIDMM_TEXTURE)
    {
        pUMDInfo = pAllocation->GetUMDAllocInfo(m_pTex[Index]);
    }
    else if (m_ResourceType == WGFVIDMM_BUFFER)
    {
        pUMDInfo = pAllocation->GetUMDAllocInfo(m_pBuf[Index]);
    }

    // Verify offer priority
    if(pAllocation->AllocInfo()->OfferPriority != m_OfferPriority)
    {
        // See if this is packed allocation. In case of packed allocation it wouldn't be offered unless
        // every other resource in the pack would also be offered
        if(pUMDInfo && !pUMDInfo->Usage.Packed)
        {
            WriteToLog(_T("COfferReclaimCreateDestroyTest::VerifyAllocationOffered() - allocation's offer priority doesn't match expected offer priority (%d, %d), (D3D Resource == 0x%p)"),
                            pAllocation->AllocInfo()->OfferPriority,
                            m_OfferPriority,
                            hAlloc);
            tr = RESULT_FAIL;
        }
    }

    return tr;
}

TEST_RESULT COfferReclaimCreateDestroyTest::VerifyAllocationsOffered()
{
    TEST_RESULT tr = RESULT_PASS;
    CAllocationTracker *pAllocation = NULL;
    sUMD_ALLOC_INFO* pUMDInfo = NULL;

    m_pETWMaster->WaitForEvents(5000);

    for(UINT i = 0; i < m_Count; i++)
    {
        HANDLE hAlloc = NULL;

        if(m_ResourceType == WGFVIDMM_TEXTURE)
        {
            hAlloc = m_pTex[i];
            pAllocation = m_pETWMaster->GetAllocationTracker(hAlloc);
        }
        else if (m_ResourceType == WGFVIDMM_BUFFER)
        {
            hAlloc = m_pBuf[i];
            pAllocation = m_pETWMaster->GetAllocationTracker(hAlloc);
        }

        if(!pAllocation)
        {
            WriteToLog(_T("COfferReclaimCreateDestroyTest::VerifyAllocationsOffered() - Can't find allocation 0x%p, possibly due to no allocation logging"), hAlloc);
            return RESULT_BLOCKED;
        }

        if(m_ResourceType == WGFVIDMM_TEXTURE)
        {
            pUMDInfo = pAllocation->GetUMDAllocInfo(m_pTex[i]);
        }
        else if (m_ResourceType == WGFVIDMM_BUFFER)
        {
            pUMDInfo = pAllocation->GetUMDAllocInfo(m_pBuf[i]);
        }

        // Verify offer priority
        if(pAllocation->AllocInfo()->OfferPriority != m_OfferPriority)
        {
            if(pUMDInfo && !pUMDInfo->Usage.Packed)
            {
                WriteToLog(_T("COfferReclaimCreateDestroyTest::VerifyAllocationsOffered() - allocation's offer priority doesn't match expected offer priority (%d, %d), (D3D Resource == 0x%p)"),
                                pAllocation->AllocInfo()->OfferPriority,
                                m_OfferPriority,
                                hAlloc);
                tr = RESULT_FAIL;
                break;
            }
        }
    }

    return tr;
}



/////////////////////////////////////////////////////////////////////////////////////////////////
//++++++++++++++++++++++++++++ COfferReclaimDeferredDeletionsTest +++++++++++++++++++++++++++++++++++++

void COfferReclaimDeferredDeletionsTest::InitTestParameters()
{
    CVidMMUMDTest::InitTestParameters();

    m_Count = 1;

    CTestCaseParameter<UINT> *pWidth = AddParameter<UINT>(_T("Width"), &m_Width);
    CTestCaseParameter<UINT> *pHeight = AddParameter<UINT>(_T("Height"), &m_Height);
    CTestCaseParameter<eWGFVIDMM_RESOURCE_TYPE> *pResourceType = AddParameter<eWGFVIDMM_RESOURCE_TYPE>(_T("ResourceType"), &m_ResourceType);

    CUserValueSet<UINT> *pWidthTexture = new CUserValueSet<UINT>();
    pWidthTexture->AddValue(512);

    CUserValueSet<UINT> *pWidthBuffer = new CUserValueSet<UINT>();
    pWidthBuffer->AddValue(1024 * 1024);

    CUserValueSet<UINT> *pHeightTexture = new CUserValueSet<UINT>();
    pHeightTexture->AddValue(512);

    CUserValueSet<UINT> *pHeightBuffer = new CUserValueSet<UINT>();
    pHeightBuffer->AddValue(1);

    CTestCaseParameter<bool> *pShared = AddParameter<bool>(_T("Shared"), &m_bShared);
    CUserValueSet<bool> * pSharedValues = new CUserValueSet<bool>();
    pSharedValues->AddValue(false);
    pSharedValues->AddValue(true);

    testfactor::RFactor TextureFactor = AddParameterValue<eWGFVIDMM_RESOURCE_TYPE>(pResourceType, WGFVIDMM_TEXTURE)
                                        * AddParameterValueSet<bool>(pShared, pSharedValues)		
                                        * AddParameterValueSet<UINT>(pWidth, pWidthTexture)
                                        * AddParameterValueSet<UINT>(pHeight, pHeightTexture);


    testfactor::RFactor BufferFactor = AddParameterValue<eWGFVIDMM_RESOURCE_TYPE>(pResourceType, WGFVIDMM_BUFFER)
                                       * AddParameterValueSet<bool>(pShared, pSharedValues)		
                                       * AddParameterValueSet<UINT>(pWidth, pWidthBuffer)
                                       * AddParameterValueSet<UINT>(pHeight, pHeightBuffer);

    testfactor::RFactor Factor = (TextureFactor + BufferFactor);
                        
    SetRootTestFactor(Factor);

}


TEST_RESULT COfferReclaimDeferredDeletionsTest::SetupTestCase()
{
    // Local variables
    D3DKMT_DRIVERVERSION wddmDriverVersion = KMT_DRIVERVERSION_WDDM_1_2;

    DevX::GRFX::AdapterDescriptor *pAdapterDescriptor = DevX::GRFX::AdapterDescriptor::GetAdapterDescriptor( m_pDXGIAdapter1 );
    if( nullptr == pAdapterDescriptor )
    {
        WriteToLog(_T("COfferReclaimDeferredDeletionsTest::SetupTestCase() - GetAdapterDescriptor failed"));
        return RESULT_BLOCKED;
    }

    if( FAILED( pAdapterDescriptor->GetDisplayDriverModel( wddmDriverVersion ) ) )
    {
        WriteToLog(_T("COfferReclaimDeferredDeletionsTest::SetupTestCase() - GetDisplayDriverModel failed"));
        delete pAdapterDescriptor;
        return RESULT_BLOCKED;
    }

    //
    // This test is skipped on WDDM2.0 adapters and above due to relaxed Offer/Reclaim requirements for the driver.
    //
    if( wddmDriverVersion >= KMT_DRIVERVERSION_WDDM_2_0 )
    {
        WriteToLog(_T("COfferReclaimDeferredDeletionsTest::SetupTestCase() - Test skipped on WDDM 2.0 and above"));
        delete pAdapterDescriptor;
        return RESULT_SKIP;
    }
            
    if(m_bShared)
    {
        if(g_App.m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_10_1)
        {
            m_bNoKeyedMutex = true;
        }
    }

    TEST_RESULT tr = CVidMMUMDTest::SetupTestCase();
    TCHAR strError[ERROR_SIZE];	 // this string is used in macros
    ZeroMemory(strError, sizeof(TCHAR) * ERROR_SIZE);

    if(tr != RESULT_PASS)
    {
        CVidMMUMDTest::CleanupTestCase();
        return tr;
    }

    if(!m_PresentHelper.Setup(this))
    {
        WriteToLog(_T("COfferReclaimDeferredDeletionsTest::SetupTestCase() - failed to setup PresentHelper"));
        return RESULT_BLOCKED;
    }

    return tr;
}



TEST_RESULT COfferReclaimDeferredDeletionsTest::ExecuteTestCase()
{
    TEST_RESULT tr = RESULT_PASS;
    TCHAR strError[ERROR_SIZE];		// this string is used by macros
    ZeroMemory(strError, sizeof(TCHAR) * ERROR_SIZE);
    bool bSharedProcessStarted = false;

    StartETWTracking();

    TEST_RESULT trLogging = RESULT_PASS;
    UINT TotalDestroyCount = 0;

    for(UINT i = 0; i < 2; i++)
    {
        HRESULT hr = S_OK;

        if(tr == RESULT_PASS && m_bShared)
        {
            if(!m_pManagerSync)
            {
                m_pManagerSync = new CXProcSyncManager(WGFVIDMM_MAPPING_NAME, WGFVIDMM_TOTAL_SHARERS, sizeof(sWGFVIDMM_SHARED_DATA));

                if(!m_pManagerSync)
                {
                    WriteToLog(_T("CPackedSurfaceTest::SetupTestCase() - out of memory"));
                    tr = RESULT_BLOCKED;
                    break;
                }
            }
        }

        // 1. Create resources
        if(m_ResourceType == WGFVIDMM_TEXTURE)
        {
            hr = Create2DTexture((UINT)0, D3D11_USAGE_DEFAULT, RESOURCE_PATTERN_1, m_bShared);
        }
        else if (m_ResourceType == WGFVIDMM_BUFFER)
        {
            hr = CreateBuffer((UINT)0, D3D11_USAGE_DEFAULT, m_bShared, m_pBufferData1);
        }

        if(FAILED(hr))
        {
            WriteToLog(_T("COfferReclaimDeferredDeletionsTest::SetupTestCase() - Failed to create resource, iteration %d, hr = 0x%X"), i, hr);
            tr = RESULT_BLOCKED;
        }

        if(tr == RESULT_PASS && m_bShared)
        {
            // Start our client app
            tr = StartChildProcess("-childBasic");

            if(tr != RESULT_PASS)
            {
                WriteToLog(_T("COfferReclaimDeferredDeletionsTest::ExecuteTestCase() - failed to start child process"));
            }
            else
            {
                bSharedProcessStarted = true;
                Sleep(1000); // give it a chance to start up
                OutputDebugString("COfferReclaimDeferredDeletionsTest::ExecuteTestCase() - successfully started child process, moving child to Running state\n");
                MANAGER_SIGNAL_TO_RUN
            }
        }

        // drain all events to this point
        m_CustomETWMaster.WaitForEvents(5000);

        // Make ETW listener aware of which resource to track
        HANDLE hAlloc = NULL;

        if(m_ResourceType == WGFVIDMM_TEXTURE)
        {
            hAlloc = m_pTex[0];
        }
        else if (m_ResourceType == WGFVIDMM_BUFFER)
        {
            hAlloc = m_pBuf[0];
        }

        m_CustomETWMaster.SetResourceToWatch(hAlloc);

        // Check if ETW data for this event got logged. If not, the test is blocked, but we'll continue to run it for stability validation
        sALLOC_INFO* pAllocInfo = m_CustomETWMaster.GetAllocInfo(hAlloc);

        if(!pAllocInfo)
        {
            WriteToLog(_T("COfferReclaimDeferredDeletionsTest::ExecuteTestCase() - can't map D3D resource %p to kernel allocation"), hAlloc);
            trLogging = RESULT_BLOCKED; // allow to continue for stability testing
        }

        g_App.setPresent(false);

        for(UINT y = 0; y < 10; y++)
        {
            bool bPresent = true;
            // Draw with texture, but don't present
            ACQUIRE_MUTEX
            if(m_ResourceType == WGFVIDMM_TEXTURE)
            {
                bPresent = m_PresentHelper.PresentTexture2D(m_pTex[0], m_pShaderResourceViews[0]);
            }
            else if (m_ResourceType == WGFVIDMM_BUFFER)
            {
                bPresent = m_PresentHelper.PresentBuffer(m_pBuf[0]);
            }
            RELEASE_MUTEX

            if(!bPresent)
            {
                WriteToLog(_T("COfferReclaimDeferredDeletionsTest::ExecuteTestCase() - Present() failed, which may indicate incorrect driver behavior. Please check dxgkdx log for any possible errors"));
                tr = RESULT_FAIL;
                break;
            }	
        }

        if(bSharedProcessStarted)
        {
            OutputDebugString("COfferReclaimDeferredDeletionsTest::ExecuteRecycleTestCase() - Signaled client, waiting for response\n");	
            MANAGER_SIGNAL_AND_WAIT
            OutputDebugString("COfferReclaimDeferredDeletionsTest::ExecuteRecycleTestCase() - Telling client to terminate\n");
            MANAGER_SIGNAL_TO_TERMINATE
        }

        if(m_bShared)
        {
            SAFE_DELETE(m_pManagerSync);
            bSharedProcessStarted = false;
        }

        if(tr != RESULT_FAIL)
        {
            ClearIndex(0);

            // call Flush() again
            GetEffectiveContext()->Flush();

            if(tr != RESULT_FAIL)
            {
                m_CustomETWMaster.WaitForEvents(10000);

                UINT OfferCount = GetOfferCount();
                UINT TerminateCount = GetTerminateCount();
                UINT DestroyCount = GetD3D11DestroyCount();
                UINT KernelDestroyCount = GetKernelDestroyCount();

                if(DestroyCount)
                {
                    TotalDestroyCount++;

                    WriteToLog(_T("COfferReclaimDeferredDeletionsTest::ExecuteTestCase() - D3D11 resource %p had been destroyed. OfferCount = %d, TeminateCount = %d, KernelDestroyCount = %d"),
                                hAlloc,        
                                OfferCount,
                                TerminateCount,
                                KernelDestroyCount);

                    if(m_bShared)
                    {
                        // UMDs shouldn't defer deletion of shared allocations.
                        if(!KernelDestroyCount && !TerminateCount)
                        {
                            WriteToLog(_T("COfferReclaimDeferredDeletionsTest::ExecuteTestCase() - Shared allocation %p hasn't been destroyed by UMD"), hAlloc);
                            tr = RESULT_FAIL;
                            m_CustomETWMaster.CleanupCounters(); //clear out the counters
                            break;
                        }
                    }
                    else
                    {
                        if(!KernelDestroyCount && !OfferCount && !TerminateCount)
                        {
                            WriteToLog(_T("COfferReclaimDeferredDeletionsTest::ExecuteTestCase() - Deferred-deleted allocation %p hasn't been terminated or offered"), hAlloc);
                            tr = RESULT_FAIL;
                            m_CustomETWMaster.CleanupCounters(); //clear out the counters
                            break;
                        }
                    }
                }
            }
        }

        m_CustomETWMaster.CleanupCounters(); //clear out the counters
    }

    StopETWTracking(true);
    
    if(!TotalDestroyCount)
    {
        WriteToLog(_T("COfferReclaimDeferredDeletionsTest::ExecuteTestCase() - D3D resource haven't been destroyed by the D3D Runtime"));
        tr = RESULT_BLOCKED;
    }

    if(tr != RESULT_FAIL && trLogging == RESULT_BLOCKED)
        tr = RESULT_BLOCKED;

    
    // log listener errors to help with debugging issues
    LogListenerErrors();
  
    if(m_pManagerSync && bSharedProcessStarted)
    {
        MANAGER_SIGNAL_AND_WAIT
        MANAGER_SIGNAL_TO_TERMINATE
        SAFE_DELETE(m_pManagerSync);
    }

    return tr;

MacroFailed:
    // We can only get here if ACQUIRE_MUTEX or RELEASE_MUTEX produced a failure.
    // log the failure and return the error
    if(m_pManagerSync && bSharedProcessStarted)
    {
        MANAGER_SIGNAL_TO_TERMINATE
        SAFE_DELETE(m_pManagerSync);
    }

    WriteToLog(strError);

    return tr;
}


UINT COfferReclaimDeferredDeletionsTest::GetOfferCount()
{
    return m_CustomETWMaster.GetOfferCount();
}

UINT COfferReclaimDeferredDeletionsTest::GetTerminateCount()
{
    return m_CustomETWMaster.GetTerminateCount();
}

UINT COfferReclaimDeferredDeletionsTest::GetD3D11DestroyCount()
{
    return m_CustomETWMaster.GetD3D11DestroyCount();
}

UINT COfferReclaimDeferredDeletionsTest::GetKernelDestroyCount()
{
    return m_CustomETWMaster.GetKernelDestroyCount();
}


//////////////////////////////////////// Listeners ///////////////////////////////////////////
void COfferReclaimDefferedDeletionETW::CleanupCounters()
{
    m_numOffers = 0;
    m_hD3D11AppResource = 0;
    m_numTerminates = 0;
    m_numD3D11Destroys = 0;
    m_numKernelDestroys = 0;
}

void COfferReclaimDefferedDeletionETW::SetResourceToWatch(HANDLE hResource)
{
    m_hD3D11AppResource = hResource;

    // If we already processed event for creation of this allocation, find it in the map and get m_hGlobalAlloc
    CCSLocker Lock(m_CSAllocationMap);

    AllocationTrackerMap::iterator it;
    it = m_AllocationMap.find(m_hD3D11AppResource);
    if(it != m_AllocationMap.end())
    {
        CAllocationTracker* pTrackedAllocation = it->second;

        m_hGlobalAlloc = pTrackedAllocation->GlobalAlloc();
    }
    else
    {
        // we might not have received the event to process this allocation yet
    }
}

void COfferReclaimDefferedDeletionETW::HandleOfferAllocation(PEVENT_RECORD pEvent)
{
    if(m_hGlobalAlloc)
    {
        CDxgKernelEventHelper EH(pEvent);

        if(EH.IsStart())
        {
            CCSLocker Lock(m_CSAllocationMap);

            AllocationTrackerMap::iterator it;
            it = m_AllocationMap.find(m_hGlobalAlloc);
            if(it != m_AllocationMap.end())
            {
                 CAllocationTracker* pTrackedAllocation = it->second;

                 it = m_AllocationMap.find(EH.GetProperty<HANDLE>(L"hAllocationHandle"));
                 if(it != m_AllocationMap.end())
                 {
                    CAllocationTracker* pThisAllocation = it->second;

                    if(pTrackedAllocation == pThisAllocation)
                    {
                        m_numOffers++;
                    }
                 }
            }
            else
            {
                DBGPrint(L"COfferReclaimDefferedDeletionETW::HandleOfferAllocation() - failed to find allocation GlobalAlloc == 0x%p", m_hGlobalAlloc);
            }
        }
    }

    CVidMmEtwMaster::HandleOfferAllocation(pEvent);
}

void COfferReclaimDefferedDeletionETW::HandleTerminateAllocation(PEVENT_RECORD pEvent)
{
    if(m_hGlobalAlloc)
    {
        CDxgKernelEventHelper EH(pEvent);

        CCSLocker Lock(m_CSAllocationMap);

        AllocationTrackerMap::iterator it;
        it = m_AllocationMap.find(m_hGlobalAlloc);
        if(it != m_AllocationMap.end())
        {
            CAllocationTracker* pTrackedAllocation = it->second;

            it = m_AllocationMap.find(EH.GetProperty<HANDLE>(L"hVidMmAlloc"));

            if(it != m_AllocationMap.end())
            {
                CAllocationTracker* pThisAllocation = it->second;

                if(pTrackedAllocation == pThisAllocation)
                {
                    m_numTerminates++;
                }
            }
        }
        else
        {
            DBGPrint(L"COfferReclaimDefferedDeletionETW::HandleTerminateAllocation() - failed to find allocation GlobalAlloc == 0x%p", m_hGlobalAlloc);
        }
    }

    CVidMmEtwMaster::HandleTerminateAllocation(pEvent);
}

void COfferReclaimDefferedDeletionETW::HandleAdapterAllocation(PEVENT_RECORD pEvent)
{
    CDxgKernelEventHelper EH(pEvent);

    if(EH.IsStop())
    {
        if(m_hGlobalAlloc)
        {
            CCSLocker Lock(m_CSAllocationMap);

            AllocationTrackerMap::iterator it;
            it = m_AllocationMap.find(m_hGlobalAlloc);
            if(it != m_AllocationMap.end())
            {
                CAllocationTracker* pTrackedAllocation = it->second;

                if(m_hGlobalAlloc == EH.GetProperty<HANDLE>(L"hVidMmGlobalAlloc"))
                {
                    m_numKernelDestroys++;
                    m_hGlobalAlloc = NULL;
                }
            }
            else
            {
                DBGPrint(L"COfferReclaimDefferedDeletionETW::HandleDestroyAllocation() - failed to find allocation GlobalAlloc == 0x%p", m_hGlobalAlloc);
            }
        }
    }

    CVidMmEtwMaster::HandleAdapterAllocation(pEvent);
}

void COfferReclaimDefferedDeletionETW::HandleD3D11Texture2D(PEVENT_RECORD pEvent)
{
    CVidMmEtwMaster::HandleD3D11Texture2D(pEvent);

    if(m_hD3D11AppResource)
    {
        CD3DEventHelper EH(pEvent);

        if(EH.IsStop())
        {
            if(EH.GetProperty<HANDLE>(L"pID3D11Resource") == m_hD3D11AppResource)
            {
                m_numD3D11Destroys++;
            }
        }
        else if(EH.IsStart() || EH.IsDCStart())
        {
            if(EH.GetProperty<HANDLE>(L"pID3D11Resource") == m_hD3D11AppResource)
            {
                // Get kernel allocation handle associated with this resource
                CCSLocker Lock(m_CSAllocationMap);

                AllocationTrackerMap::iterator it;
                it = m_AllocationMap.find(m_hD3D11AppResource);
                if(it != m_AllocationMap.end())
                {
                    CAllocationTracker* pTrackedAllocation = it->second;

                    m_hGlobalAlloc = pTrackedAllocation->GlobalAlloc();
                }
                else
                {
                    DBGPrint(L"COfferReclaimDefferedDeletionETW::HandleD3D11Texture2D() - failed to find allocation hD3DResources = 0x%p", m_hD3D11AppResource);
                }
            }
        }
    }
}


void COfferReclaimDefferedDeletionETW::HandleD3D11Buffer(PEVENT_RECORD pEvent)
{
    CVidMmEtwMaster::HandleD3D11Buffer(pEvent);

    if(m_hD3D11AppResource)
    {
        CD3DEventHelper EH(pEvent);

        if(EH.IsStop())
        {
            if(EH.GetProperty<HANDLE>(L"pID3D11Resource") == m_hD3D11AppResource)
            {
                m_numD3D11Destroys++;
            }
        }
        else if(EH.IsStart() || EH.IsDCStart())
        {
            if(EH.GetProperty<HANDLE>(L"pID3D11Resource") == m_hD3D11AppResource)
            {
                // Get kernel allocation handle associated with this resource
                CCSLocker Lock(m_CSAllocationMap);

                AllocationTrackerMap::iterator it;
                it = m_AllocationMap.find(m_hD3D11AppResource);
                if(it != m_AllocationMap.end())
                {
                    CAllocationTracker* pTrackedAllocation = it->second;

                    m_hGlobalAlloc = pTrackedAllocation->GlobalAlloc();
                }
                else
                {
                    DBGPrint(L"COfferReclaimDefferedDeletionETW::HandleD3D11Buffer() - failed to find allocation hD3DResources = 0x%p", m_hD3D11AppResource);
                }
            }
        }
    }
}



/////////////////////////////////////////////////////////////////////////////////////////////////
//++++++++++++++++++++++++++++ COfferReclaimPendingTest +++++++++++++++++++++++++++++++++++++
BEGIN_NAMED_VALUES(eOFFER_RECLAIM_PENDING_TEST)
    NAMED_VALUE(_T("OFFER_RECLAIM_PENDING_OFFER_TEST"), OFFER_RECLAIM_PENDING_OFFER_TEST)
    NAMED_VALUE(_T("OFFER_RECLAIM_PENDING_CANCEL_TEST"), OFFER_RECLAIM_PENDING_CANCEL_TEST)
END_NAMED_VALUES(eOFFER_RECLAIM_PENDING_TEST)


void COfferReclaimPendingTest::InitTestParameters()
{
    CVidMMUMDTest::InitTestParameters();

    CTestCaseParameter<eOFFER_RECLAIM_PENDING_TEST> *pTestType = AddParameter<eOFFER_RECLAIM_PENDING_TEST>(_T("TestType"), &m_TestType);
    CUserValueSet<eOFFER_RECLAIM_PENDING_TEST> *pTestTypeValues = new CUserValueSet<eOFFER_RECLAIM_PENDING_TEST>();
    pTestTypeValues->AddValue( OFFER_RECLAIM_PENDING_OFFER_TEST );
    pTestTypeValues->AddValue( OFFER_RECLAIM_PENDING_CANCEL_TEST );

    CTestCaseParameter<eWGFVIDMM_RESOURCE_TYPE> *pResourceType = AddParameter<eWGFVIDMM_RESOURCE_TYPE>(_T("ResourceType"), &m_ResourceType);
    CUserValueSet<eWGFVIDMM_RESOURCE_TYPE> *pResourceTypeValues = new CUserValueSet<eWGFVIDMM_RESOURCE_TYPE>();
    pResourceTypeValues->AddValue(WGFVIDMM_TEXTURE);
    pResourceTypeValues->AddValue(WGFVIDMM_BUFFER);

    CTestCaseParameter<eALLOCATION_OFFER_PRIORITY> *pOfferPriority = AddParameter<eALLOCATION_OFFER_PRIORITY>(_T("OfferPriority"), &m_OfferPriority);
    CUserValueSet<eALLOCATION_OFFER_PRIORITY> *pOfferPriorityValues = new CUserValueSet<eALLOCATION_OFFER_PRIORITY>();
    pOfferPriorityValues->AddValue( OFFER_PRIORITY_LOW );
    pOfferPriorityValues->AddValue( OFFER_PRIORITY_NORMAL );
    pOfferPriorityValues->AddValue( OFFER_PRIORITY_HIGH );

    CTestCaseParameter<bool> *pShared = AddParameter<bool>(_T("Shared"), &m_bShared);
    CUserValueSet<bool> * pSharedValues = new CUserValueSet<bool>();
    pSharedValues->AddValue(false);
    pSharedValues->AddValue(true);

    CTestCaseParameter<bool> *ValidpDiscarded = AddParameter<bool>(_T("ValidpDiscarded"), &m_ValidpDiscarded);
    CUserValueSet<bool> * ValidpDiscardedValues = new CUserValueSet<bool>();
    ValidpDiscardedValues->AddValue(true);
    ValidpDiscardedValues->AddValue(false);

    testfactor::RFactor Factor = (AddParameterValueSet<eOFFER_RECLAIM_PENDING_TEST>(pTestType, pTestTypeValues)
                               * AddParameterValueSet<eWGFVIDMM_RESOURCE_TYPE>(pResourceType, pResourceTypeValues)
                               * AddParameterValueSet<bool>(pShared, pSharedValues)
                               * AddParameterValueSet<bool>(ValidpDiscarded, ValidpDiscardedValues))
                               % AddParameterValueSet<eALLOCATION_OFFER_PRIORITY>(pOfferPriority, pOfferPriorityValues);

    SetRootTestFactor(Factor);
}

TEST_RESULT COfferReclaimPendingTest::SetupTestCase()
{
    m_Width = 1024;
    m_Height = 1024;
    m_Count = 1;

    if(m_ResourceType == WGFVIDMM_BUFFER)
    {
        m_Width = m_Width * m_Height;
        m_Height = 1;
    }

    if(m_bShared)
    {
        if(g_App.m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_10_1)
        {
            m_bNoKeyedMutex = true;
        }
    }

    TEST_RESULT tr = CVidMMUMDTest::SetupTestCase();

    if(tr != RESULT_PASS)
    {
        CVidMMUMDTest::CleanupTestCase();
        return tr;
    }

    HRESULT hr = S_OK;

    if(m_ResourceType == WGFVIDMM_TEXTURE)
    {
        hr = Create2DTexture((UINT)0, D3D11_USAGE_DEFAULT, RESOURCE_PATTERN_1, m_bShared);
    }
    else if (m_ResourceType == WGFVIDMM_BUFFER)
    {
        hr = CreateBuffer((UINT)0, D3D11_USAGE_DEFAULT, m_bShared, m_pBufferData1);
    }

    if(FAILED(hr))
    {
        WriteToLog(_T("COfferReclaimPendingTest::SetupTestCase() - failed to create resource, hr = 0x%X"), hr);
        tr = RESULT_BLOCKED;
    }

    if(tr == RESULT_PASS)
    {
        if(m_ResourceType == WGFVIDMM_TEXTURE)
        {
            hr = Fill2DTexture(0, RESOURCE_PATTERN_1);
        }
        else if (m_ResourceType == WGFVIDMM_BUFFER)
        {
            hr = FillBuffer(0, m_pBufferData1);
        }

        if(FAILED(hr))
        {
            WriteToLog(_T("COfferReclaimPendingTest::SetupTestCase() - failed to fill resource, hr = 0x%X"), hr);
            tr = RESULT_BLOCKED;
        }
    }

    if(tr == RESULT_PASS)
    {
        if(!m_PresentHelper.Setup(this))
        {
            WriteToLog(_T("COfferReclaimPendingTest::SetupTestCase() - failed to setup PresentHelper"));
            tr = RESULT_BLOCKED;
        }
    }

    return tr;
}


TEST_RESULT COfferReclaimPendingTest::ExecuteTestCase()
{
    TEST_RESULT tr = RESULT_PASS;
    TEST_RESULT trLogging = RESULT_PASS;
    HRESULT hr = S_OK;
    TCHAR strError[ERROR_SIZE];	// macros are using this string
    ZeroMemory(strError, sizeof(TCHAR) * ERROR_SIZE);

    StartETWTracking();

    if(m_bShared)
    {
        if(!m_pManagerSync)
        {
            m_pManagerSync = new CXProcSyncManager(WGFVIDMM_MAPPING_NAME, WGFVIDMM_TOTAL_SHARERS, sizeof(sWGFVIDMM_SHARED_DATA));

            if(!m_pManagerSync)
            {
                WriteToLog(_T("COfferReclaimPendingTest::ExecuteTestCase() - out of memory"));
                tr = RESULT_BLOCKED;
            }
        }

        // Start our client app
        tr = StartChildProcess("-childBasic");

        if(tr != RESULT_PASS)
        {
            WriteToLog(_T("COfferReclaimPendingTest::ExecuteTestCase() - failed to start child process"));
        }
        else
        {
            Sleep(1000); // give it a chance to start up
            OutputDebugString("COfferReclaimPendingTest::ExecuteTestCase() - successfully started child process, moving child to Running state\n");
            MANAGER_SIGNAL_TO_RUN
        }
    }

    // drain all events to this point
    m_CustomETWMaster.WaitForEvents(5000);

    HANDLE hAllocation = NULL;

    if(m_ResourceType == WGFVIDMM_TEXTURE)
        hAllocation = (HANDLE)m_pTex[0];
    else if (m_ResourceType == WGFVIDMM_BUFFER)
        hAllocation = (HANDLE)m_pBuf[0];

    // Make ETW listener aware of which resource to trace
    m_CustomETWMaster.SetResourceToWatch(hAllocation);

    // Check if ETW data for this event got logged. If not, the test is blocked, but we'll continue to run it for stability validation
    sALLOC_INFO* pAllocInfo = m_CustomETWMaster.GetAllocInfo(hAllocation);

    if(!pAllocInfo)
    {
        WriteToLog(_T("COfferReclaimPendingTest::ExecuteTestCase() - can't map D3D resource %p to kernel allocation"), hAllocation);
        trLogging = RESULT_BLOCKED; // allow to continue for stability testing
    }

    if(tr == RESULT_PASS)
    {
        bool bPresent = false;
        g_App.setPresent(false);


        for(UINT i = 0; i < 10; i++)
        {
            ACQUIRE_MUTEX
            if(m_ResourceType == WGFVIDMM_TEXTURE)
            {
                bPresent = m_PresentHelper.PresentTexture2D(m_pTex[0], m_pShaderResourceViews[0]);
            }
            else if (m_ResourceType == WGFVIDMM_BUFFER)
            {
                bPresent = m_PresentHelper.PresentBuffer(m_pBuf[0]);
            }
            RELEASE_MUTEX

            if(!bPresent)
            {
                WriteToLog(_T("COfferReclaimPendingTest::ExecuteTestCase() - Present() failed (iteration %d), which may indicate incorrect driver behavior. Please check dxgkdx log for any possible errors"), i);
                tr = RESULT_FAIL;
                break;
            }
        }
    }

    if(tr == RESULT_PASS)
    {
        DXGI_OFFER_RESOURCE_PRIORITY DXGIOfferPriority;

        switch(m_OfferPriority)
        {
        case OFFER_PRIORITY_LOW: DXGIOfferPriority = DXGI_OFFER_RESOURCE_PRIORITY_LOW; break;
        case OFFER_PRIORITY_NORMAL: DXGIOfferPriority = DXGI_OFFER_RESOURCE_PRIORITY_NORMAL; break;
        case OFFER_PRIORITY_HIGH: DXGIOfferPriority = DXGI_OFFER_RESOURCE_PRIORITY_HIGH; break;
        default: assert(0);
        }

        // Offer this resource
        ACQUIRE_MUTEX
        HRESULT hr = m_pDXGIDevice2->OfferResources(1, &m_pResources[0], DXGIOfferPriority);
        RELEASE_MUTEX

        if(FAILED(hr))
        {
            tr = RESULT_FAIL;
            WriteToLog(_T("COfferReclaimPendingTest::ExecuteTestCase() - OfferResources() failed, hr = 0x%X"), hr);
        }

        if(tr == RESULT_PASS)
        {
            // At this point we don't expect allocation to be in any offer state, because we expect driver to pipeline offers
            // In case when driver flushes all the work above, the offer won't be pipelined in the buffer, but it could still
            // be pipelined in kernel. We don't have enough information to know this. However, it is ok, because the purpose
            // of this test is to make sure that driver pipelines Offer correctly and if it doesn't, there is nothing to 
            // validate. If it doesn't pipeline Offer and Offers and allocation before all refereces to it are retires, 
            // kernel will detect it and we'll see Device Removed errors later in the test. So, it's OK to continue.
            UINT OfferCount = GetOfferCount();
            bool bPipelinedOffer = true;

            if(OfferCount)
            {
                bPipelinedOffer = false;
                WriteToLog(_T("COfferReclaimPendingTest::ExecuteTestCase() - D3D resource 0x%p is offered; offer doesn't appear to be pipelined"), hAllocation);
                DBGPrint(L"COfferReclaimPendingTest::ExecuteTestCase() - D3D resource 0x%p is offered; offer doesn't appear to be pipelined", hAllocation);
            }
            else
            {
                if(m_TestType == OFFER_RECLAIM_PENDING_CANCEL_TEST)
                {
                    // Reclaim before Offer has a chance to get to the kernel
                    BOOL* pDiscarded = NULL;

                    if(m_ValidpDiscarded)
                        pDiscarded = &m_pDiscarded[0];

                    ACQUIRE_MUTEX
                    hr = m_pDXGIDevice2->ReclaimResources(1, &m_pResources[0], pDiscarded);
                    RELEASE_MUTEX

                    if(FAILED(hr))
                    {
                        tr = RESULT_FAIL;
                        WriteToLog(_T("COfferReclaimPendingTest::ExecuteTestCase() - ReclaimResources() failed, hr = 0x%X"), hr);
                    }
                    else
                    {
                        if(m_ValidpDiscarded)
                        {
                            // Allocation shouldn't be discarded since offer never went through
                            if(m_pDiscarded[0])
                            {
                                tr = RESULT_FAIL;
                                WriteToLog(_T("COfferReclaimPendingTest::ExecuteTestCase() - Cancelled pending Offer resulted in Discareded resource 0x%p"), hAllocation);
                            }
                        }
                    }
                }

                if(tr != RESULT_FAIL)
                {
                    // After we VidMM don't have reference to our allocation it should be offered		
                    g_App.setPresent(true);

                    for(UINT i = 0; i < 5; i++)
                    {
                        if(!GetFramework()->Present(NULL, NULL, NULL, NULL, 0, 0))
                        {
                            WriteToLog(_T("COfferReclaimPendingTest::ExecuteTestCase() - Present() failed, which may indicate incorrect driver behavior. Please check dxgkdx log for any possible errors"));
                            tr = RESULT_FAIL;
                        }
                    }

                    if(tr != RESULT_FAIL)
                    {
                        OfferCount = GetOfferCount();

                        if(m_TestType == OFFER_RECLAIM_PENDING_CANCEL_TEST)
                        {
                            // we don't expect to see any OFFERS, because they should have been cancelled before it has a chance to 
                            // get flushed to the kernel
                            if(OfferCount)
                            {
                                tr = RESULT_FAIL;
                                WriteToLog(_T("COfferReclaimPendingTest::ExecuteTestCase() - cancelled offer on D3D Resource 0x%p was flushed to kernel, OfferCount = %d"), hAllocation, OfferCount);
                                DBGPrint(L"COfferReclaimPendingTest::ExecuteTestCase() - cancelled offer on D3D Resource 0x%p was flushed to kernel, OfferCount = %d", hAllocation, OfferCount);
                            }
                        }
                        else
                        {
                            // We expect offer to be flushed to kernel
                            if(OfferCount != 1)
                            {
                                tr = RESULT_FAIL;
                                WriteToLog(_T("COfferReclaimPendingTest::ExecuteTestCase() - expected to see 1 offer event for D3D resource 0x%p, saw %d"), hAllocation, OfferCount);
                                DBGPrint(L"COfferReclaimPendingTest::ExecuteTestCase() - expected to see 1 offer event for D3D resource 0x%p, saw %d", hAllocation, OfferCount);
                            }
                        }
                    }
                }
            }

            TEST_RESULT trUseResource = RESULT_PASS;

            // reclaim, re-generate content and try to use (even if we failed cancelled offer validation)
            if(bPipelinedOffer == false || m_TestType != OFFER_RECLAIM_PENDING_CANCEL_TEST)
            {
                ACQUIRE_MUTEX
                hr = m_pDXGIDevice2->ReclaimResources(1, &m_pResources[0], NULL);
                RELEASE_MUTEX

                if(FAILED(hr))
                {
                    trUseResource = RESULT_FAIL;
                    WriteToLog(_T("COfferReclaimPendingTest::ExecuteTestCase() - ReclaimResources() failed, hr = 0x%X"), hr);
                }
            }

            if(trUseResource == RESULT_PASS)
            {
                ACQUIRE_MUTEX
                if(m_ResourceType == WGFVIDMM_TEXTURE)
                {
                    hr = Fill2DTexture(0, RESOURCE_PATTERN_1);
                }
                else if (m_ResourceType == WGFVIDMM_BUFFER)
                {
                    hr = FillBuffer(0, m_pBufferData1);
                }
                RELEASE_MUTEX

                if(FAILED(hr))
                {
                    WriteToLog(_T("COfferReclaimPendingTest::ExecuteTestCase() - failed to fill resource 0x%p after reclaiming, hr = 0x%X"), hAllocation, hr);
                    trUseResource = RESULT_BLOCKED;
                }
            }

            if(trUseResource == RESULT_PASS)
            {
                bool bPresent = true;
                g_App.setPresent(true);

                ACQUIRE_MUTEX
                if(m_ResourceType == WGFVIDMM_TEXTURE)
                {
                    bPresent = m_PresentHelper.PresentTexture2D(m_pTex[0], m_pShaderResourceViews[0]);
                }
                else if (m_ResourceType == WGFVIDMM_BUFFER)
                {
                    bPresent = m_PresentHelper.PresentBuffer(m_pBuf[0]);
                }
                RELEASE_MUTEX

                if(!bPresent)
                {
                    WriteToLog(_T("COfferReclaimPendingTest::ExecuteTestCase() - Present() failed after Offer/Reclaim, which may indicate incorrect driver behavior. Please check dxgkdx log for any possible errors"));
                    trUseResource = RESULT_FAIL;
                }
            }

            if(trUseResource == RESULT_FAIL)
                tr = RESULT_FAIL;
        }
    }

    StopETWTracking(true);
    m_CustomETWMaster.CleanupCounters(); //clear out the Offer count and tracked allocation

    // log listener errors to help with debugging issues
    LogListenerErrors();

    if(m_bShared)
    {
        OutputDebugString("COfferReclaimPendingTest::ExecuteTestCase() - Signaling client and waiting\n");
        MANAGER_SIGNAL_AND_WAIT
        MANAGER_SIGNAL_TO_TERMINATE
    }

    if(tr != RESULT_FAIL && trLogging == RESULT_BLOCKED)
        tr = RESULT_BLOCKED;

    return tr;

MacroFailed:
    // We can only get here if ACQUIRE_MUTEX or RELEASE_MUTEX produced a failure.
    // log the failure and return the error
    if(m_bShared)
    {
        OutputDebugString("COfferReclaimPendingTest::ExecuteTestCase() - Moving Client to Terminate state\n");
        MANAGER_SIGNAL_TO_TERMINATE
    }

    WriteToLog(strError);
    return tr;
}

UINT COfferReclaimPendingTest::GetOfferCount()
{
    m_CustomETWMaster.WaitForEvents(5000);
    return m_CustomETWMaster.GetOfferCount();
}

//////////////////////////////////////// Listeners ///////////////////////////////////////////
void COfferReclaimPendingETW::CleanupCounters()
{
    CVidMmEtwMaster::Cleanup();
    m_numOffers = 0;
    m_hD3D11AppResource = 0;
}

void COfferReclaimPendingETW::HandleOfferAllocation(PEVENT_RECORD pEvent)
{
    if(m_hD3D11AppResource)
    {
        CDxgKernelEventHelper EH(pEvent);

        if(EH.IsStart())
        {
            AllocationTrackerMap::iterator it;
            it = m_AllocationMap.find(m_hD3D11AppResource);
            if(it != m_AllocationMap.end())
            {
                 CCSLocker Lock(m_CSAllocationMap);
                 CAllocationTracker* pTrackedAllocation = it->second;

                 it = m_AllocationMap.find(EH.GetProperty<HANDLE>(L"hAllocationHandle"));
                 if(it != m_AllocationMap.end())
                 {
                    CAllocationTracker* pThisAllocation = it->second;

                    if(pTrackedAllocation == pThisAllocation)

                    {
                        m_numOffers++;
                    }
                }
            }
        }
    }

    CVidMmEtwMaster::HandleOfferAllocation(pEvent);
}



////////////////////////////////////////////////////////////////////////////////////////////////
//+++++++++++++++++++++++++++++++++++++ CCoherencyTest +++++++++++++++++++++++++++++++++++++++++

CCoherencyTest::CCoherencyTest()
: m_pStagingReadOnly(NULL),
  m_pStagingReadWrite(NULL),
  m_pStagingWriteOnly(NULL),
  m_pTexture(NULL),
  m_bHWCacheCoherencyAvailable(false)
{
    OutputDebugString("CCoherencyTest()\n");
}

CCoherencyTest::~CCoherencyTest()
{
    m_pETWMaster = NULL;
    OutputDebugString("~CCoherencyTest()\n");
}


void CCoherencyTest::InitTestParameters()
{
    CVidMMUMDTest::InitTestParameters();

    m_Width = 1024;
    m_Height = 1024;
}


TEST_RESULT CCoherencyTest::SetupTestCase()
{
    assert(m_Width);
    assert(m_Height);

    // Create resources
    HRESULT hr = S_OK;

    // pStagingReadOnly
    D3D11_TEXTURE2D_DESC tex2DDesc = {0};
    tex2DDesc.ArraySize			= 1;
    tex2DDesc.Format			= m_DXGIFormat;
    tex2DDesc.Height			= m_Height;
    tex2DDesc.Width				= m_Width;
    tex2DDesc.MipLevels			= 1;
    tex2DDesc.MiscFlags			= 0;
    tex2DDesc.SampleDesc.Count	= 1;
    tex2DDesc.SampleDesc.Quality= 0;
    tex2DDesc.BindFlags			= 0;
    tex2DDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_READ;
    tex2DDesc.Usage				= D3D11_USAGE_STAGING;

    hr = GetDevice()->CreateTexture2D(&tex2DDesc, NULL, &m_pStagingReadOnly);

    if(FAILED(hr))
    {
        WriteToLog(_T("CCoherencyTest::SetupTestCase() - Failed to create read-only staging surface, hr = 0x%X"), hr);
        return RESULT_BLOCKED;
    }

    // pStagingReadWrite
    tex2DDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;

    hr = GetDevice()->CreateTexture2D(&tex2DDesc, NULL, &m_pStagingReadWrite);

    if(FAILED(hr))
    {
        WriteToLog(_T("CCoherencyTest::SetupTestCase() - Failed to create read-write staging surface, hr = 0x%X"), hr);
        return RESULT_BLOCKED;
    }

    // pStagingWriteOnly
    tex2DDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;

    hr = GetDevice()->CreateTexture2D(&tex2DDesc, NULL, &m_pStagingWriteOnly);

    if(FAILED(hr))
    {
        WriteToLog(_T("CCoherencyTest::SetupTestCase() - Failed to create write-only staging surface, hr = 0x%X"), hr);
        return RESULT_BLOCKED;
    }

    // Texture
    tex2DDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
    tex2DDesc.CPUAccessFlags	= 0;
    tex2DDesc.Usage				= D3D11_USAGE_DEFAULT;
    
    hr = GetDevice()->CreateTexture2D(&tex2DDesc, NULL, &m_pTexture);

    if(FAILED(hr))
    {
        WriteToLog(_T("CCoherencyTest::SetupTestCase() - Failed to create texture, hr = 0x%X"), hr);
        return RESULT_BLOCKED;
    }

    return RESULT_PASS;
}


void CCoherencyTest::CleanupTestCase()
{
    StopETWTracking(true);

    // Flush references to our resources
    if(m_pEffectiveContext)
    {
        GetEffectiveContext()->Flush();
    }

    SAFE_RELEASE(m_pStagingReadOnly);
    SAFE_RELEASE(m_pStagingReadWrite);
    SAFE_RELEASE(m_pStagingWriteOnly);
    SAFE_RELEASE(m_pTexture);
}


TEST_RESULT CCoherencyTest::ResourceContentAndValidation(ID3D11Texture2D* pTex, BYTE Pattern, bool bFill)
{
    assert(pTex);
    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;

    // Map resource
    D3D11_MAPPED_SUBRESOURCE MappedResource;
    ZeroMemory(&MappedResource, sizeof(MappedResource));
    D3D11_MAP MapType = D3D11_MAP_READ;

    if(bFill)
    {
        MapType = D3D11_MAP_WRITE;
    }

    hr = GetEffectiveContext()->Map(pTex, 0, MapType, 0, &MappedResource);

    if(FAILED(hr))
    {
        WriteToLog(_T("CCoherencyTest::ResourceContentAndValidation() - Map() failed, hr = 0x%X"), hr);
        tr = RESULT_BLOCKED;
    }
    else
    {
        if(!m_bHWCacheCoherencyAvailable)
        {
            // Validate that pointer retuned to use from Map is cacheable
            MEMORY_BASIC_INFORMATION memInfo;
            ZeroMemory(&memInfo, sizeof(memInfo));

            if(!VirtualQuery(MappedResource.pData, &memInfo, sizeof(memInfo)))
            {
                WriteToLog(_T("CCoherencyTest::ResourceContentAndValidation() - VirtualQuery() failed, error = %d"), GetLastError());
                tr = RESULT_BLOCKED;
            }
            else
            {
                // double check that all of this allocation is withing the memory range returned by VirtualQuery
                UINT AllocSize = m_Height * MappedResource.RowPitch;
            
                if(AllocSize > memInfo.RegionSize)
                {
                    WriteToLog(_T("CCoherencyTest::ResourceContentAndValidation() - RegionSize returned by VirtualQuery is less then Allocation size (%d < %d)"), 
                               memInfo.RegionSize,
                               AllocSize);
                    tr = RESULT_FAIL;
                }
                else
                {
                    // Check that neither PAGE_NOCACHE nor PAGE_WRITECOMBINE access flags are set for our allocation pointer
                    if(memInfo.AllocationProtect & (PAGE_NOCACHE || PAGE_WRITECOMBINE))
                    {
                        WriteToLog(_T("CCoherencyTest::ResourceContentAndValidation() - allocation pointer has unexpected protection modifyer: (%s%s%s)"),
                                   ((memInfo.AllocationProtect & PAGE_NOCACHE)?L"PAGE_NOCACHE":L""),
                                   ((memInfo.AllocationProtect & (PAGE_NOCACHE && PAGE_WRITECOMBINE))?L", ":L""),
                                   ((memInfo.AllocationProtect & PAGE_WRITECOMBINE)?L"PAGE_WRITECOMBINE":L""));
                        tr = RESULT_FAIL;
                    }
                }
            }
        }

        if(tr == RESULT_PASS)
        {
            if(bFill)
            {
                for(UINT i = 0; i < m_Height; i++)
                {
                    for(UINT y = 0; y < MappedResource.RowPitch; y++)
                    {
                        ((BYTE*)(MappedResource.pData))[i*m_Height + y] = Pattern;
                    }
                }
            }
            else
            {
                // Check the pattern
                for(UINT i = 0; i < m_Height; i++)
                {
                    for(UINT y = 0; y < MappedResource.RowPitch; y++)
                    {
                        if(((BYTE*)(MappedResource.pData))[i*m_Height + y] != Pattern)
                        {
                            WriteToLog(_T("CCoherencyTest::ResourceContentAndValidation() - Resource pattern doesn't match: (%c, %c)"),
                                       ((BYTE*)(MappedResource.pData))[i*m_Height + y],
                                       Pattern);
                            tr = RESULT_FAIL;
                        }
                    }
                }
            }
        }

        GetEffectiveContext()->Unmap(pTex, 0);
    }

    return tr;
}


TEST_RESULT CCoherencyTest::ValidateAllocationCreationParameters()
{
    TEST_RESULT tr = RESULT_PASS;

    // Get information about HW Cache Coherency support and allocations from ETW listener
    StartETWTracking();
    m_pETWMaster->WaitForEvents(5000);
    StopETWTracking(false);

    // log listener errors, but don't fail
    LogListenerErrors();

    UINT SegmentCount = 0;
    sSEGMENT_INFO* pSegments = NULL;

    m_pETWMaster->GetSegmentInfo(&SegmentCount, &pSegments);

    for(UINT i = 0; i < SegmentCount; i++)
    {
        if(pSegments[i].Flags.Aperture && pSegments[i].Flags.CacheCoherent)
        {
            m_bHWCacheCoherencyAvailable = true;
        }
    }

    // 10level9 staging surfaces will end up being created in D3DDDIPOOL_SYSMEM. These allocation won't have kernel resources
    // therefore, these check is not necessary.
    // UMD will create an intermediate surface to copy from system memory to video memory. This intermediate surface would need
    // to follow same requirements as staging surface, but we don't have a way to validate this.
    if(g_App.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_10_0)
    {
        sALLOC_INFO* pStagingReadOnlyInfo = m_pETWMaster->GetAllocInfo(m_pStagingReadOnly);
        sALLOC_INFO* pStagingReadWriteInfo = m_pETWMaster->GetAllocInfo(m_pStagingReadWrite);
        sALLOC_INFO* pStagingWriteOnlyInfo = m_pETWMaster->GetAllocInfo(m_pStagingWriteOnly);

        if(!pStagingReadOnlyInfo || !pStagingReadWriteInfo || !pStagingWriteOnlyInfo)
        {
            WriteToLog(_T("CCoherencyTest::ValidateAllocationCreationParameters() - UMD Logging is required to match allocations to ETW trackers"));
            return RESULT_BLOCKED;		
        }

        bool bFail = false;

        // validate allocation flags
        if(!(pStagingReadOnlyInfo->Flags.Cached))
        {
            WriteToLog(_T("CCoherencyTest::ValidateAllocationCreationParameters() - Staging read-only surface must be cached"));
            bFail = true;
        }

        if(!(pStagingReadWriteInfo->Flags.Cached))
        {
            WriteToLog(_T("CCoherencyTest::ValidateAllocationCreationParameters() - Staging read-write surface must be cached"));
            bFail = true;
        }


        if(!m_bHWCacheCoherencyAvailable)
        {
            // cached surfaces can be placed into Aperture only on HW that doesn't have cached coherency support
            for(UINT i = 0; i < SegmentCount; i++)
            {
                UINT SegmentMask = 1 << i;
                bool IsAperture = (pSegments[i].Flags.Aperture) ? true : false;

                if(pStagingReadOnlyInfo->SupportedSegmentSet & SegmentMask)
                {
                    if(!IsAperture)
                    {
                        WriteToLog(_T("CCoherencyTest::ValidateAllocationCreationParameters() - read-only cached surfaces must be placed into aperture segment when cache-coherent segment is not available"));
                        bFail = true;
                    }
                }

                if(pStagingReadWriteInfo->SupportedSegmentSet & SegmentMask)
                {
                    if(!IsAperture)
                    {
                        WriteToLog(_T("CCoherencyTest::ValidateAllocationCreationParameters() - read-write cached surfaces must be placed into aperture segment when cache-coherent segment is not available"));
                        bFail = true;
                    }
                }			
            }
        }
        else
        {
            // if HW supports cache coherency, cached surfaces must go into cache-coherent segments
            for(UINT i = 0; i < SegmentCount; i++)
            {
                UINT SegmentMask = 1 << i;
                bool IsCacheCoherent = (pSegments[i].Flags.CacheCoherent) ? true : false;

                if(pStagingReadOnlyInfo->SupportedSegmentSet & SegmentMask)
                {
                    if(!IsCacheCoherent)
                    {
                        WriteToLog(_T("CCoherencyTest::ValidateAllocationCreationParameters() - read-only cached surfaces must be placed into cache-coherent segment when one is available"));
                        bFail = true;
                    }
                }

                if(pStagingReadWriteInfo->SupportedSegmentSet & SegmentMask)
                {
                    if(!IsCacheCoherent)
                    {
                        WriteToLog(_T("CCoherencyTest::ValidateAllocationCreationParameters() - read-write cached surfaces must be placed into cache-coherent segment when one is available"));
                        bFail = true;
                    }
                }			
            }
        }

        if(bFail)
            tr = RESULT_FAIL;
    }

    return tr;
}


TEST_RESULT CCoherencyTest::ExecuteTestCase()
{
    assert(m_pStagingReadOnly);
    assert(m_pStagingWriteOnly);
    assert(m_pStagingReadWrite);
    assert(m_pTexture);

    srand(100);

    TEST_RESULT tr = RESULT_PASS;

    TEST_RESULT trAllocationCreation = ValidateAllocationCreationParameters();

    // Check for coherency issues
    for(UINT i = 0; i < 500; i++)
    {
        BYTE Pattern = (rand() % 256) + 1;

        // 1. fill read-write surface with pattern (CPU-touch)
        tr = ResourceContentAndValidation(m_pStagingReadWrite, Pattern, true);

        if(tr == RESULT_PASS)
        {
            // 2. Copy pattern from read-write surface to texture
            GetEffectiveContext()->CopyResource(m_pTexture, m_pStagingReadWrite);

            // 3. Copy pattern from texture to read-only surface (GPU touch)
            GetEffectiveContext()->CopyResource(m_pStagingReadOnly, m_pTexture);

            // 4. Read pattern from read-only surface (CPU touch)
            tr = ResourceContentAndValidation(m_pStagingReadOnly, Pattern, false);

            if(tr != RESULT_PASS)
            {
                WriteToLog(_T("CCoherencyTest::ExecuteTestCase() - ResourceContentAndValidation() didn't succeed for read-only surface"));
                break;
            }
        }
        else
        {
            WriteToLog(_T("CCoherencyTest::ExecuteTestCase() - ResourceContentAndValidation() didn't succeed for read-write surface"));
            break;
        }
    }

    if(trAllocationCreation == RESULT_FAIL)
    {
        tr = RESULT_FAIL;
    }
    else if(trAllocationCreation == RESULT_BLOCKED && tr != RESULT_FAIL)
    {
        tr = RESULT_BLOCKED;
    }

    return tr;
}