/******************************Module*Header*******************************
* Module Name: VidMMWDDM1_2.h
*
* D3D-based tests that verifies UMD WDDM1.2 VidMM requirements
*
* Author: Marina Meleshenko [marmel]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/
#pragma once

#include "wgfvidmm.h"

enum eRESIDENCY
{
    RESIDENCY_NOT_RESIDENT = 0,
    RESIDENCY_RESIDENT_VIDEO = 1,
    RESIDENCY_RESIDENT_APERTURE = 2
};

DECLARE_NAMED_VALUES(eWGFVIDMM_RESOURCE_TYPE);

BEGIN_NAMED_VALUES(eWGFVIDMM_RESOURCE_TYPE)
    NAMED_VALUE(_T("WGFVIDMM_TEXTURE"), WGFVIDMM_TEXTURE)
    NAMED_VALUE(_T("WGFVIDMM_BUFFER"), WGFVIDMM_BUFFER)
END_NAMED_VALUES(eWGFVIDMM_RESOURCE_TYPE)


class CVidMMUMDTest : public CWGFOfferReclaimTest
{
public:
    CVidMMUMDTest();
    ~CVidMMUMDTest(){OutputDebugString("~CVidMMUMDTest()\n");};

protected:
    bool LogListenerErrors();

    eRESIDENCY GetAllocationResidency(ID3D11Texture2D* pTex);
    eALLOCATION_OFFER_STATE GetOfferState(HANDLE pAllocHandle);
    eALLOCATION_OFFER_PRIORITY GetAllocationOfferPriority(HANDLE pAllocHandle);

    bool                    m_bVerifyUMDAllocInfo;
};

////////////////////// Listeners /////////////////////////////////

class CUMDLoggingInternalSurfaceETW : public CVidMmEtwMaster
{
public:
    CUMDLoggingInternalSurfaceETW()
        : m_bNewStagingBuffer(false),
          m_bReclaimedStagingBuffer(false),
          m_bReclaimedWrongSemantic(false),
          m_bNewWrongSemantic(false),
          m_bReadyForCustomETWHandler(false)
    { 
        m_tstrError = _T("");
        OutputDebugString("CUMDLoggingInternalSurfaceETW()\n");
    }

    ~CUMDLoggingInternalSurfaceETW(){OutputDebugString("~CUMDLoggingInternalSurfaceETW()\n");}

    bool IsNewStagingBuffer(){return m_bNewStagingBuffer;}
    bool IsReclaimedStagingBuffer(){return m_bReclaimedStagingBuffer;}
    bool IsNewWrongSemantic(){return m_bNewWrongSemantic;}
    bool IsReclaimedWrongSemantic(){return m_bReclaimedWrongSemantic;}
    void ReadyForCustomETWHandler(bool bReady) {m_bReadyForCustomETWHandler = bReady;}
    tstring& GetErrorString(){return m_tstrError;}

private:
    bool m_bNewStagingBuffer;
    bool m_bReclaimedStagingBuffer;
    bool m_bNewWrongSemantic;
    bool m_bReclaimedWrongSemantic;
    bool m_bReadyForCustomETWHandler;
    tstring m_tstrError;

    virtual void HandleUMDAllocation(PEVENT_RECORD pEvent);
    virtual void HandleReclaimAllocation(PEVENT_RECORD pEvent);
};


class COfferReclaimPendingETW : public CVidMmEtwMaster
{
public:
    COfferReclaimPendingETW()
        : m_numOffers(0),
          m_hD3D11AppResource(NULL)
    {OutputDebugString("COfferReclaimPendingETW()\n");}

    ~COfferReclaimPendingETW(){OutputDebugString("~COfferReclaimPendingETW()\n");}

    UINT GetOfferCount(){return m_numOffers;}
    void SetResourceToWatch(HANDLE hResource){m_hD3D11AppResource = hResource;}
    void CleanupCounters();

private:
    UINT m_numOffers;
    HANDLE m_hD3D11AppResource;

    virtual void HandleOfferAllocation(PEVENT_RECORD pEvent);
};


class COfferReclaimDefferedDeletionETW : public CVidMmEtwMaster
{
public:
    COfferReclaimDefferedDeletionETW()
        : m_numOffers(0),
          m_numTerminates(0),
          m_numKernelDestroys(0),
          m_hD3D11AppResource(NULL),
          m_hGlobalAlloc(NULL),
          m_numD3D11Destroys(0)
    {OutputDebugString("COfferReclaimDefferedDeletionETW()\n");}

    ~COfferReclaimDefferedDeletionETW(){OutputDebugString("~COfferReclaimDefferedDeletionETW()\n");}

    UINT GetOfferCount(){return m_numOffers;}
    UINT GetTerminateCount(){return m_numTerminates;}
    UINT GetD3D11DestroyCount(){return m_numD3D11Destroys;}
    UINT GetKernelDestroyCount() {return m_numKernelDestroys;}
    void SetResourceToWatch(HANDLE hResource);
    void CleanupCounters();

private:
    UINT m_numOffers;
    UINT m_numTerminates;
    UINT m_numKernelDestroys;
    HANDLE m_hD3D11AppResource;
    HANDLE m_hGlobalAlloc;
    UINT m_numD3D11Destroys;

    virtual void HandleOfferAllocation(PEVENT_RECORD pEvent);
    virtual void HandleTerminateAllocation(PEVENT_RECORD pEvent);
    virtual void HandleAdapterAllocation(PEVENT_RECORD pEvent);
    virtual void HandleD3D11Texture2D(PEVENT_RECORD pEvent);
    virtual void HandleD3D11Buffer(PEVENT_RECORD pEvent);
};


///////////////////////////////////////////// Internal Surfaces //////////////////////////////////////////
class CInternalSurfaceTest : public CVidMMUMDTest
{
public:
    CInternalSurfaceTest();
    ~CInternalSurfaceTest(){OutputDebugString("~CInternalSurfaceTest()\n");};

    virtual void InitTestParameters();
    virtual TEST_RESULT SetupTestCase();
    virtual TEST_RESULT ExecuteTestCase();
    virtual TEST_RESULT VerifyResults(){return RESULT_BLOCKED;} // child classes should implement this method

protected:
    ID3D11Texture2D*        m_pVidMemTex;
    ID3D11Texture2D*        m_pStagingTex;
};

class CUMDLoggingInternalSurfaceTest : public CInternalSurfaceTest
{
public: 
    CUMDLoggingInternalSurfaceTest()
    {
        m_pETWMaster = &m_CustomETWMaster;
        m_bVerifyUMDAllocInfo = true;

        OutputDebugString("CUMDLoggingInternalSurfaceTest()\n");
    }

    ~CUMDLoggingInternalSurfaceTest()
    {
        OutputDebugString("~CUMDLoggingInternalSurfaceTest()\n");
    }

    virtual TEST_RESULT VerifyResults();
    virtual TEST_RESULT ExecuteTestCase();

private:
    CUMDLoggingInternalSurfaceETW m_CustomETWMaster;
};

class COfferReclaimStagingSurfaceTest : public CInternalSurfaceTest
{
public:
    COfferReclaimStagingSurfaceTest(){OutputDebugString("COfferReclaimStagingSurfaceTest()\n");}
    ~COfferReclaimStagingSurfaceTest(){OutputDebugString("~COfferReclaimStagingSurfaceTest()\n");}

    virtual TEST_RESULT VerifyResults();
};

////////////////////////////////////////////// Packed allocations ///////////////////////////////////////////////
class CPackedSurfaceTest : public CVidMMUMDTest
{
public:
    CPackedSurfaceTest();
    ~CPackedSurfaceTest();

    virtual void InitTestParameters();
    virtual TEST_RESULT SetupTestCase();
    virtual TEST_RESULT ExecuteTestCase();
    virtual TEST_RESULT VerifyResults(){return RESULT_BLOCKED;} // child classes should implement this method
    virtual void CleanupTestCase();
    virtual int GetNumRenderWindows() const { return 1;} // this will result in rendering window being created. This is needed for CPresentHelper

protected:
    HANDLE			 		m_pPackedAllocs[MAX_RESOURCE_COUNT];		// these will point to textures in m_pTex, which are marked as Packed
    IDXGIResource*          m_pPackedResources[MAX_RESOURCE_COUNT];		// these will point to resources in m_pResources, which are marked as Packed
    UINT                    m_PackedCount;
    bool                    m_bNoAllocationLogging;
    eWGFVIDMM_RESOURCE_TYPE m_ResourceType;
    testfactor::RFactor     m_Factor;
    bool                    m_bSharedProcessStarted;
    CPresentHelper          m_PresentHelper;
};

class CUMDLoggingPackedSurfaceTest : public CPackedSurfaceTest
{
public: 
    CUMDLoggingPackedSurfaceTest()
    {
        m_bVerifyUMDAllocInfo = true;
        OutputDebugString("CUMDLoggingPackedSurfaceTest()\n");
    }

    ~CUMDLoggingPackedSurfaceTest(){OutputDebugString("~CUMDLoggingPackedSurfaceTest()\n");}

    virtual TEST_RESULT VerifyResults();
};

enum eOFFER_RECLAIM_PACKED_SURFACE_TEST
{
    OFFER_RECLAIM_PACKED_SURFACE_TEST_SINGLE = 0,
    OFFER_RECLAIM_PACKED_SURFACE_TEST_MULTIPLE = 1,
    OFFER_RECLAIM_PACKED_SURFACE_TEST_ALL = 2
};
DECLARE_NAMED_VALUES(eOFFER_RECLAIM_PACKED_SURFACE_TEST);

class COfferReclaimPackedSurfaceTest : public CPackedSurfaceTest
{
public:
    COfferReclaimPackedSurfaceTest(){OutputDebugString("COfferReclaimPackedSurfaceTest()\n");}
    ~COfferReclaimPackedSurfaceTest(){OutputDebugString("~COfferReclaimPackedSurfaceTest()\n");}

    virtual void InitTestParameters();
    virtual TEST_RESULT ExecuteTestCase();
    virtual TEST_RESULT VerifyResults();

private:
    eOFFER_RECLAIM_PACKED_SURFACE_TEST        m_TestType;

    TEST_RESULT ExecuteTestCaseSingle();
    TEST_RESULT ExecuteTestCaseMultiple();
    TEST_RESULT ExecuteTestCaseAll();
};

/////////////////////////////////////////// Renaming ////////////////////////////////////////////////
class CRenamedSurfaceTest : public CVidMMUMDTest
{
public:
    CRenamedSurfaceTest();
    ~CRenamedSurfaceTest();

    virtual void InitTestParameters();
    virtual TEST_RESULT SetupTestCase();
    virtual TEST_RESULT ExecuteTestCase();
    virtual TEST_RESULT VerifyResults(){return RESULT_BLOCKED;} // child classes should implement this method
    virtual int GetNumRenderWindows() const { return 1;} // this will result in rendering window being created. This is needed for CPresentHelper


protected:
    UINT                    m_TimesToRename;
    CPresentHelper          m_PresentHelper;
    eWGFVIDMM_RESOURCE_TYPE m_ResourceType;
    testfactor::RFactor     m_Factor;

    HRESULT RenameTexture(ID3D11Texture2D* pTex, ID3D11ShaderResourceView* pShaderView);
    HRESULT RenameBuffer(ID3D11Buffer* pBuf);
};

class CUMDLoggingRenamedSurfaceTest : public CRenamedSurfaceTest
{
public: 
    CUMDLoggingRenamedSurfaceTest()
    {
        m_bVerifyUMDAllocInfo = true;
        OutputDebugString("CUMDLoggingRenamedSurfaceTest()\n");
    }
    ~CUMDLoggingRenamedSurfaceTest(){OutputDebugString("~CUMDLoggingRenamedSurfaceTest()\n");};

    virtual TEST_RESULT ExecuteTestCase();
    virtual TEST_RESULT VerifyResults();
};

enum eOFFER_RECLAIM_RENAMED_SURFACE_TEST
{
    OFFER_RECLAIM_RENAMED_SURFACE_TEST_OFFERED = 0,
    OFFER_RECLAIM_RENAMED_SURFACE_TEST_RECLAIMED = 1,
    OFFER_RECLAIM_RENAMED_SURFACE_TEST_RETIRED = 2,
    OFFER_RECLAIM_RENAMED_SURFACE_TEST_RETIRED_OFFERED = 3
};
DECLARE_NAMED_VALUES(eOFFER_RECLAIM_RENAMED_SURFACE_TEST);

class COfferReclaimRenamedSurfaceTest : public CRenamedSurfaceTest
{
public:
    COfferReclaimRenamedSurfaceTest(){OutputDebugString("COfferReclaimRenamedSurfaceTest()\n");}
    ~COfferReclaimRenamedSurfaceTest(){OutputDebugString("~COfferReclaimRenamedSurfaceTest()\n");}

    virtual void InitTestParameters();
    virtual TEST_RESULT SetupTestCase();
    virtual TEST_RESULT ExecuteTestCase();
    virtual TEST_RESULT VerifyResults();

private:
    eOFFER_RECLAIM_RENAMED_SURFACE_TEST m_TestType;
};

//////////////////////////////////////////////////////////////////////////////////////////
enum eCREATE_DESTROY_TEST
{
    CREATE_DESTROY_TEST_BASIC = 0,
    CREATE_DESTROY_TEST_RECYCLE = 1,
    CREATE_DESTROY_TEST_REUSE = 2
};
DECLARE_NAMED_VALUES(eCREATE_DESTROY_TEST);

class CUMDLoggingCreateDestroyTest : public CVidMMUMDTest
{
public:
    CUMDLoggingCreateDestroyTest();
    ~CUMDLoggingCreateDestroyTest();

    virtual void InitTestParameters();
    virtual TEST_RESULT SetupTestCase();
    virtual TEST_RESULT ExecuteTestCase();
    virtual TEST_RESULT VerifyResults();

protected:
    eCREATE_DESTROY_TEST      m_TestType;
};

//////////////////////////////////////////////////////////////////////////////
DECLARE_NAMED_VALUES(eALLOCATION_OFFER_PRIORITY);

class COfferReclaimCreateDestroyTest : public CVidMMUMDTest
{
public:
    COfferReclaimCreateDestroyTest(){OutputDebugString("COfferReclaimCreateDestroyTest()\n");}
    ~COfferReclaimCreateDestroyTest(){OutputDebugString("~COfferReclaimCreateDestroyTest()\n");}
    virtual void InitTestParameters();
    virtual TEST_RESULT SetupTestCase();
    virtual TEST_RESULT ExecuteTestCase();
    TEST_RESULT VerifyAllocationsNotOffered();
    TEST_RESULT VerifyAllocationsOffered();
    TEST_RESULT VerifyAllocationNotOffered(UINT Index);
    TEST_RESULT VerifyAllocationOffered(UINT Index);
    TEST_RESULT ExecuteBasicTestCase();
    TEST_RESULT ExecuteRecycleTestCase();
    TEST_RESULT ExecuteReuseTestCase();
    virtual int GetNumRenderWindows() const { return 1;} // this will result in rendering window being created. This is needed for CPresentHelper

protected:
    eALLOCATION_OFFER_PRIORITY   m_OfferPriority;
    CPresentHelper          m_PresentHelper;
    eCREATE_DESTROY_TEST      m_TestType;
    bool                      m_bReclaim;
};

class COfferReclaimCreateDestroyReuseChild : public CWGFVidMMResourceChild
{
public:
    COfferReclaimCreateDestroyReuseChild(){m_bNoKeyedMutex = false;}
    TEST_RESULT ExecuteTestCase();
};

//////////////////////////////////////////////////////////////////////////////
class COfferReclaimDeferredDeletionsTest : public CVidMMUMDTest
{
public:
    COfferReclaimDeferredDeletionsTest() 
    {
        m_pETWMaster = &m_CustomETWMaster;
        m_ResourceType = WGFVIDMM_TEXTURE;
        m_bShared = false;
        OutputDebugString("COfferReclaimDeferredDeletionsTest()\n");
    }
    ~COfferReclaimDeferredDeletionsTest(){OutputDebugString("~COfferReclaimDeferredDeletionsTest()\n");}
    virtual void InitTestParameters();
    virtual TEST_RESULT SetupTestCase();
    virtual TEST_RESULT ExecuteTestCase();
    virtual int GetNumRenderWindows() const { return 1;} // this will result in rendering window being created. This is needed for CPresentHelper

protected:
    CPresentHelper          m_PresentHelper;
    COfferReclaimDefferedDeletionETW         m_CustomETWMaster;
    eWGFVIDMM_RESOURCE_TYPE      m_ResourceType;
    UINT GetOfferCount();
    UINT GetTerminateCount();
    UINT GetD3D11DestroyCount();
    UINT GetKernelDestroyCount();

};

//////////////////////////////////////////////////////////////////////////////
enum eOFFER_RECLAIM_PENDING_TEST
{
    OFFER_RECLAIM_PENDING_OFFER_TEST = 0,
    OFFER_RECLAIM_PENDING_CANCEL_TEST = 1
};
DECLARE_NAMED_VALUES(eOFFER_RECLAIM_PENDING_TEST);

class COfferReclaimPendingTest : public CVidMMUMDTest
{
public:
    COfferReclaimPendingTest()
    {
        m_ValidpDiscarded = true;
        m_ResourceType = WGFVIDMM_TEXTURE;
        m_bShared = false;
        m_OfferPriority = OFFER_PRIORITY_NORMAL;
        m_pETWMaster = &m_CustomETWMaster;
        OutputDebugString("COfferReclaimPendingTest()\n");
    }
    ~COfferReclaimPendingTest(){OutputDebugString("~COfferReclaimPendingTest()\n");}
    virtual void InitTestParameters();
    virtual TEST_RESULT SetupTestCase();
    virtual TEST_RESULT ExecuteTestCase();
    virtual int GetNumRenderWindows() const { return 1;} // this will result in rendering window being created. This is needed for CPresentHelper

protected:
    CPresentHelper          m_PresentHelper;
    eOFFER_RECLAIM_PENDING_TEST     m_TestType;
    COfferReclaimPendingETW         m_CustomETWMaster;
    eWGFVIDMM_RESOURCE_TYPE         m_ResourceType;
    eALLOCATION_OFFER_PRIORITY      m_OfferPriority;
    bool                            m_ValidpDiscarded;

    UINT GetOfferCount();
};



////////////////////////////// Coherency //////////////////////////////////
class CCoherencyTest : public CVidMMUMDTest
{
public:
    CCoherencyTest();
    ~CCoherencyTest();

    virtual void InitTestParameters();
    virtual TEST_RESULT SetupTestCase();
    virtual void CleanupTestCase();
    virtual TEST_RESULT ExecuteTestCase();

    TEST_RESULT ResourceContentAndValidation(ID3D11Texture2D* pTex, BYTE Pattern, bool bFill);
    TEST_RESULT ValidateAllocationCreationParameters();

protected:
    ID3D11Texture2D*        m_pStagingReadOnly;
    ID3D11Texture2D*        m_pStagingReadWrite;
    ID3D11Texture2D*        m_pStagingWriteOnly;
    ID3D11Texture2D*        m_pTexture;
    bool                    m_bHWCacheCoherencyAvailable;
};



//////////////////////////////////////////////////////////////////////////////
class CVidMMUMDTestApp : public CD3D11TestFramework
{
public:
    CVidMMUMDTestApp() : m_bPresent( false ) {}
    void Present() { if ( m_bPresent ) CD3D11TestFramework::Present( 0, 0, 0, 0, 0, 0 );	}
    bool Present(RECT *pSrc, RECT *pDest, void *pSourceDirtyRegion, void *pDestClipRegion, UINT SyncInterval, UINT dwFlags)
    {
        if ( m_bPresent )
            return CD3D11TestFramework::Present( pSrc, pDest, pSourceDirtyRegion, pDestClipRegion, SyncInterval, dwFlags );

        return true;
    }
    bool getPresent() { return m_bPresent; }
    void setPresent(bool bPresent) {m_bPresent = bPresent;}
protected:
    TESTTYPE GetTestType() const { return TESTTYPE_CONF; }

    virtual void InitOptionVariables();
    virtual bool InitTestGroups();

    bool m_bPresent;
};


