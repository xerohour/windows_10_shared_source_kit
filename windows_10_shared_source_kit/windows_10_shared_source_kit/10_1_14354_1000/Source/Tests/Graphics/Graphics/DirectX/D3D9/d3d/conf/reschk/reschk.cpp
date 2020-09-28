//-----------------------------------------------------------------------------
// File: CReschk.cpp
//
// Desc: 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "reschk.h"

 DWORD dwPriority;
    BOOL  bUseContentProtection;
    D3DAUTHENTICATEDCHANNELTYPE AuthenticatedChannelType;
//                                    Priority | Use Content Protection | Authenticated Channel Type
RESCHK_TESTCASES g_MyTestCases[] = { {RESOURCE_PRIORITY_DISCARD, FALSE, D3DAUTHENTICATEDCHANNEL_D3D9},
                                     {RESOURCE_PRIORITY_LOW, FALSE, D3DAUTHENTICATEDCHANNEL_D3D9},
                                     {RESOURCE_PRIORITY_NORMAL, FALSE, D3DAUTHENTICATEDCHANNEL_D3D9},
                                     {RESOURCE_PRIORITY_HIGH, FALSE, D3DAUTHENTICATEDCHANNEL_D3D9},
                                     // {RESOURCE_PRIORITY_MAXIMUM, FALSE, D3DAUTHENTICATEDCHANNEL_D3D9}, Test with copy resource.
                                     // {RESOURCE_PRIORITY_NORMAL, TRUE, D3DAUTHENTICATEDCHANNEL_D3D9}, IV case is tested in API case
                                     {RESOURCE_PRIORITY_NORMAL, TRUE, D3DAUTHENTICATEDCHANNEL_DRIVER_SOFTWARE},
                                     {RESOURCE_PRIORITY_NORMAL, TRUE, D3DAUTHENTICATEDCHANNEL_DRIVER_HARDWARE} };
UINT cMyTestCases = sizeof(g_MyTestCases) / sizeof(g_MyTestCases[0]);

CReschk::CReschk(void)
{   
    if(KeySet(_T("stress")))
    {
		SetTestType(TESTTYPE_STRESS);
        m_nMaxPageOutRender     = 10;
        m_nMaxPageOutResources  = 1;
    }
    else
    {
        SetTestType(TESTTYPE_CONF);
        m_nMaxPageOutRender     = 5;
        m_nMaxPageOutResources  = 512;
    }

    m_Options.D3DOptions.bReference = false;
    m_Options.D3DOptions.fMinDXVersion = 9.1f;

    m_nMaxTestCases = 4;

    // Resources
    m_pTestResource = NULL;
    m_pCopyResource = NULL;

    // Content Protection helper
    m_pCPControl = NULL;

    // Timing
    m_dQPCFrequency = 0;
    m_dLoadTime     = 0;
    
    // TestPageOut settings private
    ReadInteger(_T("MaxSkip"),       1, (int*)&m_nMaxSkipPageOutTries, false);
    // MinPageOut is not used on EonE because we need to run on each encryption GUID... 
    // making the test run extremely long.
    ReadInteger(_T("MinPageOut"),    3, (int*)&m_nMinPageOutTries,     false);
    ReadInteger(_T("MaxFail"),       2, (int*)&m_nMaxFailPageOutTries, false);

    // TestPageOut settings public
    ReadInteger(_T("MaxRender"),    m_nMaxPageOutRender,    (int*)&m_nMaxPageOutRender,    false);
    ReadInteger(_T("MaxResources"), m_nMaxPageOutResources, (int*)&m_nMaxPageOutResources, false);

    if(KeySet(_T("EonE")))
    {
        // Add Encryption on Eviction test cases
        m_nMaxTestCases = cMyTestCases;

        // Force FlipEx
        m_Options.D3DOptions.dwSwapEffect = SWAPEFFECT_FLIPEX;
        m_Options.D3DOptions.DisplayMode.Format = FMT_X8R8G8B8; 
    }
}

CReschk::~CReschk(void)
{
}

//----------------------------------------------------------------------------
// Name: CapsCheck()
// Desc: Check caps requred by test. 
//----------------------------------------------------------------------------
bool CReschk::CapsCheck(void)
{
	// Reschk test is not valid for the reference device
	if (m_pSrcDevice->GetDevType() == DEVICETYPE_REF)
	{
		WriteToLog("Reference device not supported.\n");
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------
// Name: Setup()
// Desc: Setup device specific settings, variables, ect.
// Called when devices are restored(group init, device lost, etc)
//----------------------------------------------------------------------------
bool CReschk::Setup(void)
{
    // Make sure we are on 9.L or up
    if( 9.1f > m_pD3D->GetVersion() )
    {
        WriteToLog(_T("Test needs DX9.L to run. Version = %f.\n"), m_pD3D->GetVersion());
        return false;
    }

    // Common vertices for tests
    float   fWidth  = (float)m_Options.D3DOptions.nBackBufferWidth  - 10.0f;
    float   fHeight = (float)m_Options.D3DOptions.nBackBufferHeight - 10.0f;
    m_Vertices[0] = DIFUSEVERTEX(10.0f,  fHeight, 0.9f, (1.0f / 0.9f), RGBA_MAKE(255,255,255,255));
    m_Vertices[1] = DIFUSEVERTEX(10.0f,    10.0f, 0.9f, (1.0f / 0.9f), RGBA_MAKE(255,255,255,255));
    m_Vertices[2] = DIFUSEVERTEX(fWidth, fHeight, 0.9f, (1.0f / 0.9f), RGBA_MAKE(255,255,255,255));
    m_Vertices[3] = DIFUSEVERTEX(fWidth,   10.0f, 0.9f, (1.0f / 0.9f), RGBA_MAKE(255,255,255,255));

    // Setup tests resources
    if( false == SetupResource() )
        WriteToLog(_T("SetupResource Failed"));

    return true;
}

//----------------------------------------------------------------------------
// Name: TestInitialize()
// Desc: Setup non-device specific settings, variables, ect.
// Called once for each group if CapsCheck succeeds.
//----------------------------------------------------------------------------
UINT CReschk::TestInitialize(void)
{
    if( false == SetTestRange(1, m_nMaxTestCases) )
        return D3DTESTINIT_ABORT;
    
	return D3DTESTINIT_RUN;
}

//----------------------------------------------------------------------------
// Name: ExecuteTest(UINT uTestNum)
// Desc: Called once for each test defined by SetTestRange()
//----------------------------------------------------------------------------
bool CReschk::ExecuteTest(UINT uTestNum)
{ 
    if( 0 == uTestNum || uTestNum > cMyTestCases )
    {
        WriteToLog(_T("Bad input uTestNum. uTestNum must be > 0 and <= %d"), cMyTestCases);
        return true;
    }

    HRESULT hr = E_FAIL;
    double  PagedInRenderTime   = 0; // Render time when in memory GPU-accessible mem
    double  PagedOutRenderTime  = 0; // Render time when not in memory GPU-accessible mem
    UINT    nTMPMinPageOutTries = m_nMinPageOutTries; // To increment based on skips
    UINT    nSkipedPageOutCount = 0; // Number of times TestPageOutResource skiped
    UINT    nFailedPageOutCount = 0; // Number of times TestPageOutResource failed
    UINT    uPagedOutRenderCount = 0; // Number of times
    DWORD   dwPriTest   = g_MyTestCases[uTestNum - 1].dwPriority;  // Applied to m_pTestResource
    DWORD   dwPriCopy   = dwPriTest + 0x28000000;       // PriTest + 1. Applied to m_pCopyResource 
    UINT    nNumEncryptionGuids = 1; // Default is one to insure we run once if protection is not being tested.
    GetLastError();

    // Start test case
    TCHAR szTestCaseName[256];
    if( FALSE == g_MyTestCases[uTestNum - 1].bUseContentProtection )
        sprintf(szTestCaseName, _T("Priority - %lu"), dwPriTest );
    else
    {
        sprintf(szTestCaseName, _T("Encryption On Eviction") );
    }
    BeginTestCase(szTestCaseName, uTestNum);

    // Make sure we have a test resource
    if( NULL == m_pTestResource || NULL == m_pCopyResource )
    {
        WriteToLog(_T("Resource not setup"));
        Skip();
        return false;
    }
    
    // Make sure we have work to do
    if( 0 == m_nMinPageOutTries )
    {
        WriteToLog(_T("Bad input for MinPageOutTries."));
        Fail();
        return false;
    }

    // Create content protection helper object
    ReleaseCPControl();
    if( TRUE == g_MyTestCases[uTestNum - 1].bUseContentProtection )
    {
        // Create
        if( false == CreateCPControl(g_MyTestCases[uTestNum - 1].AuthenticatedChannelType, TRUE) )
        {
            WriteToLog(_T("CreateCPControl failed."));
            Skip();
            return false;
        }
        
        // Get number of encryption guids to test... nNumEncryptionGuids
        hr = m_pCPControl->QueryEvictionEncryptionGuidCount(&nNumEncryptionGuids);
        if( FAILED(hr) )
        {
            WriteToLog(_T("QueryEvictionEncryptionGuidCount failed"));
            ReleaseCPControl();
            Fail();
            return false;
        }

        // Test encryption guids
        if( false == TestEncryptionGuids() )
        {
            WriteToLog(_T("TestEncryptionGuids failed"));
            ReleaseCPControl();
            Fail();
            return false;
        }
    }

    // SetPriority return test
    if( false == TestSetPriority(m_pTestResource, dwPriTest) )
    {
        Fail();
        return false;
    }

    if( false == TestSetPriority(m_pCopyResource, dwPriCopy) )
    {
        Fail();
        return false;
    }

    // GetPriority return test
    if( false == TestGetPriority(m_pTestResource, dwPriTest) )
    {
        Fail();
        return false;
    }

    if( false == TestGetPriority(m_pCopyResource, dwPriCopy) )
    {
        Fail();
        return false;
    }

    // Get resource into memory
    for(int i = 0; i < m_nMaxPageOutRender; i++)
    {
        if( S_OK != m_pSrcDevice->CheckResourceResidency(&m_pTestResource, 1) ||
            S_OK != m_pSrcDevice->CheckResourceResidency(&m_pCopyResource, 1) )
        {
            LoadResource(m_pTestResource);
            LoadResource(m_pCopyResource); // make sure copy resource is last used
        }
        else
            break;
    }

    hr = m_pSrcDevice->CheckResourceResidency(&m_pTestResource, 1);
    if( S_OK != hr && S_RESIDENT_IN_SHARED_MEMORY != hr )
    {
        WriteToLog(_T("Unable to get test resource into resident memory.\n"));
        WriteToLog(_T("CheckResourceResidency() must return S_RESIDENT_IN_SHARED_MEMORY or S_OK.\n"));
        Fail();
        return false;
    }
    hr = m_pSrcDevice->CheckResourceResidency(&m_pCopyResource, 1);
    if( S_OK != hr && S_RESIDENT_IN_SHARED_MEMORY != hr )
    {
        WriteToLog(_T("Unable to get copy resource into resident memory.\nCheckResourceResidency() FAILED.\n"));
        WriteToLog(_T("CheckResourceResidency() must return S_RESIDENT_IN_SHARED_MEMORY or S_OK.\n"));
        Fail();
        return false;
    }

    if( TRUE == g_MyTestCases[uTestNum - 1].bUseContentProtection )
    {
        // Cycle test for each EncryptionGuidCount
        for( UINT EncryptionGuidID = 0; EncryptionGuidID < nNumEncryptionGuids; EncryptionGuidID++ )
        {
            // Get/Set Encryption GUID for EncryptionGuidID
            if( false == SetEncryptionGuidById(EncryptionGuidID) )
            {
                WriteToLog(_T("SetEncryptionGuidById failed for EncryptionGuidID: %d"), EncryptionGuidID);
                ReleaseCPControl();
                Fail();
                return false;
            }
            // Evict Resource
            if( PAGEOUTRESOURCE_SKIP == TestPageOutResource() )
                Skip();
            else
            {
                // If a resource got evected verify we can load it again. 
                // Else, log a skip and move on to the next EncryptionGuidID.
                BOOL bEvictedResource = FALSE;
                hr = m_pSrcDevice->CheckResourceResidency(&m_pTestResource, 1);
                if( S_OK != hr && S_RESIDENT_IN_SHARED_MEMORY != hr )
                {
                    bEvictedResource = TRUE;
                    LoadResource(m_pTestResource);
                }
                hr = m_pSrcDevice->CheckResourceResidency(&m_pCopyResource, 1);
                if( S_OK != hr && S_RESIDENT_IN_SHARED_MEMORY != hr )
                {
                    bEvictedResource = TRUE;
                    LoadResource(m_pCopyResource);
                }

                if( FALSE == bEvictedResource )
                    Skip(); // No evicted resources...
                else 
                    Pass(); //Resource was evicted and loaded.       

            }
        } // End of cycle test for each EncryptionGuidCount
    }
    else if( KeySet(_T("Evict")) )
    {
        // Get render time for the resource in GPU-accessible memory
        for( int i = 0; i < m_nMinPageOutTries; i++ )
        {
            PagedInRenderTime = TimedLoadResource( m_pTestResource );
        }
        PagedInRenderTime /= m_nMinPageOutTries;


        // Does lower priority resource (m_pTestResource) page out
        // before higher priority resource (m_pCopyResource)?
        // - Test must run m_nMinPageOutTries without skipping 
        // m_nMaxSkipPageOutTries times, otherwise we skip the test.
        // - Test must not fail more then m_nMaxFailPageOutTries
        for( int i = 0; i < nTMPMinPageOutTries && 
            (i - nFailedPageOutCount - nSkipedPageOutCount) < (m_nMinPageOutTries - m_nMaxFailPageOutTries);
            i++)
        {
            UINT nPageOutResult = TestPageOutResource();
            if( PAGEOUTRESOURCE_FAIL == nPageOutResult )
            {
                nFailedPageOutCount++;
                if( m_nMaxFailPageOutTries < nFailedPageOutCount )
                {
                    WriteToLog(_T("Failing because max number of page out failes reached\n"));
                    WriteToLog(_T("Fails = %d, Skips = %d, NumTries = %d\n"), 
                                nFailedPageOutCount, nSkipedPageOutCount, i+1 );
                    Fail();
                    return false;
                }            
            }
            else if( PAGEOUTRESOURCE_SKIP == nPageOutResult )
            {
                nSkipedPageOutCount++;
                if( m_nMaxSkipPageOutTries <= nSkipedPageOutCount )
                {
                    WriteToLog(_T("Skipping because max number of page out skips reached\n"));
                    WriteToLog(_T("Fails = %d, Skips = %d, NumTries = %d\n"), 
                                nFailedPageOutCount, nSkipedPageOutCount, i+1 );
                    Skip();
                    return false;
                }
                nTMPMinPageOutTries++;
            }
            else
            {   // TestPageOutResource passed, get render time for resource well
                // its out of memory
                PagedOutRenderTime += TimedLoadResource(m_pTestResource);
                LoadResource(m_pCopyResource);
                uPagedOutRenderCount++;
            }
        }

        WriteToLog(_T("Fails = %d, Skips = %d, NumTries = %d\n"), 
            nFailedPageOutCount, nSkipedPageOutCount, nTMPMinPageOutTries );

        // Render time for paged out resource must be slower then paged in resource.
        if( 0 != uPagedOutRenderCount )
        {
            PagedOutRenderTime /= uPagedOutRenderCount;
            if( PagedInRenderTime > PagedOutRenderTime )
            {
                WriteToLog(_T("Failing because resource rendered slower when in GPU-accessible memory.\n"));
                Fail();
                return false;
            }
        }
    }
    
    return false;
} 

//----------------------------------------------------------------------------
// Name: TimedLoadResource()
// Desc: Time how long it takes to load pResource
//----------------------------------------------------------------------------
double CReschk::TimedLoadResource(LPRESOURCE pResource)
{
    LARGE_INTEGER Frequency;
    QueryPerformanceFrequency( &Frequency );
    m_dQPCFrequency = double(Frequency.QuadPart);
    m_dLoadTime = 0;

    ClearFrame();
    LoadResource(pResource);
    
    m_dQPCFrequency = 0;
    return m_dLoadTime;
}
void CReschk::StartTime() 
{ 
    QueryPerformanceCounter( &m_liStartTime ); 
}
void CReschk::EndTime()
{
    QueryPerformanceCounter( &m_liQPCEnd );
    
    if( 0 != m_dQPCFrequency )
        m_dLoadTime = double( m_liQPCEnd.QuadPart - m_liStartTime.QuadPart ) / m_dQPCFrequency;
}

//----------------------------------------------------------------------------
// Name: TestPageOutResource()
// Desc: Try to page out the test resource before the copy.
// PAGEOUTRESOURCE_SKIP = Unable to page out.
// PAGEOUTRESOURCE_PASS = Copy resource did not page out before test resource.
// PAGEOUTRESOURCE_FAIL = Other
//----------------------------------------------------------------------------
UINT CReschk::TestPageOutResource()
{
    UINT            nReturn = 0;
    HRESULT         hResult = E_FAIL;
    UINT            nNumTex = 0;
    list<CTexture*> lstpTexture;
    CTexture*       pTexture = NULL;
    CVertexBuffer*  pVB     = NULL;
    
    // Get resource into memory
    for(int i = 0; i < m_nMaxPageOutRender; i++)
    {
        if( S_OK != m_pSrcDevice->CheckResourceResidency(&m_pTestResource, 1) ||
            S_OK != m_pSrcDevice->CheckResourceResidency(&m_pCopyResource, 1) )
        {
            LoadResource(m_pTestResource);
            LoadResource(m_pCopyResource); // make sure copy resource is last used
        }
        else
            break;
    }

    if( S_OK != m_pSrcDevice->CheckResourceResidency(&m_pTestResource, 1) ||
        S_OK != m_pSrcDevice->CheckResourceResidency(&m_pCopyResource, 1) )
    {
        WriteToLog(_T("Unable to get resource into GPU-accessible memory.\n"));
        return PAGEOUTRESOURCE_SKIP;
    }

    // Create vertex buffer
    if( FAILED(CreateVBTEX1(&pVB)) )
    {
        WriteToLog(_T("TestPageOutResource() - Unablet to create vb.\n"));
        return PAGEOUTRESOURCE_FAIL;
    }
    
    // Create up to m_nMaxPageOutResources textures and render
    // with each up to m_nMaxPageOutRender times.
    for(nNumTex = 0; nNumTex < m_nMaxPageOutResources; nNumTex++)
    {
        if( FAILED(CreatePageOutTexture(&pTexture)) )
            break;
        
        lstpTexture.push_back(pTexture);
        pTexture->SetPriority(RESOURCE_PRIORITY_MAXIMUM);

        for(UINT nRenders = 0; nRenders < m_nMaxPageOutRender; nRenders++)
        {
            ClearFrame();
            m_pSrcDevice->SetTexture(0, pTexture);
            m_pSrcDevice->SetStreamSource( 0, pVB, sizeof( TEX1VERTEX ), 0 );
            m_pSrcDevice->SetFVF( D3DFVF_TEX1VERTEX );
            m_pSrcDevice->BeginScene();
            m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
            m_pSrcDevice->EndScene();
            m_pSrcDevice->Present(NULL, NULL, NULL, NULL);

            nReturn = TestResourceResidency();
            if( PAGEOUTRESOURCE_FAIL == nReturn )
            {
                WriteToLog(_T("Fail at - nNumTex: %lu, nRenders: %lu\n"), 
                    nNumTex, nRenders);
                goto exit;
            }
            else if( PAGEOUTRESOURCE_PASS == nReturn ) 
            {
                WriteToLog(_T("Pass at - nNumTex: %lu, nRenders: %lu\n"), 
                    nNumTex, nRenders);
                goto exit;
            }
        }
    }
    if( 0 == nNumTex )
    {
        WriteToLog(_T("TestPageOutResource() - Unable to create textures.\n"));
        return PAGEOUTRESOURCE_FAIL;
    }

    // Unable to get resource our of GPU-accessible memory
    WriteToLog(_T("TestPageOutResource() - Skipping.\n"));
    nReturn = PAGEOUTRESOURCE_SKIP;

exit:
    RELEASE(pVB);

    while (lstpTexture.size() > 0)
    {
        CTexture* pTex = lstpTexture.front();
        RELEASE(pTex);
        lstpTexture.pop_front();
    } 

    return nReturn;
}

//----------------------------------------------------------------------------
// Name: TestResourceResidency()
// Desc: Test CheckResourceResidency() succeeds and 
// PAGEOUTRESOURCE_SKIP = Resources not paged out
// PAGEOUTRESOURCE_PASS = Copy resource did not page out before test resource.
//                        or all resources paged out.
// PAGEOUTRESOURCE_FAIL = Copy resource paged out before test resource.
//----------------------------------------------------------------------------
UINT CReschk::TestResourceResidency()
{
    HRESULT hResult, hResultCopy;
    hResult = m_pSrcDevice->CheckResourceResidency(&m_pTestResource, 1);
    hResultCopy = m_pSrcDevice->CheckResourceResidency(&m_pCopyResource, 1);

    // copy in, test in
    if( S_OK == hResultCopy && S_OK == hResult ) 
        return PAGEOUTRESOURCE_SKIP;

    // copy in, test out
    if( S_OK == hResultCopy && S_OK != hResult ) 
    {
        WriteToLog(_T("TestResourceResidency() copy in, test out\n"));
        return PAGEOUTRESOURCE_PASS;
    }
    
    // copy out, test in
    if( S_OK != hResultCopy && S_OK == hResult ) 
    {
        WriteToLog(_T("TestResourceResidency() copy out, test in\n"));
        return PAGEOUTRESOURCE_FAIL;
    }

    // copy out, test out
    WriteToLog(_T("TestResourceResidency() copy out, test out\n"));
    return PAGEOUTRESOURCE_PASS;
}

//----------------------------------------------------------------------------
// Name: CreatePageOutTexture()
// Desc: 
//----------------------------------------------------------------------------
HRESULT CReschk::CreatePageOutTexture(CTexture** ppTexture)
{
    HRESULT hResult = E_FAIL;
    DWORD dwTextureWidth = 800;//m_pSrcDevice->GetCaps()->dwMaxTextureWidth;
	DWORD dwTextureHeight = 600;//m_pSrcDevice->GetCaps()->dwMaxTextureHeight;
    if(FAILED(hResult = m_pSrcDevice->CreateTexture(dwTextureWidth, dwTextureHeight,
      1, 0, m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat], POOL_DEFAULT , ppTexture)))
    {
        return hResult;
    }
    
    int fRanColor = (int)(256.0f * rand() / (RAND_MAX + 1.0f));
    D3DCOLOR d3dColorsStripe[] = {RGBA_MAKE(0,255,0,255), RGBA_MAKE(0,0,fRanColor,255)};
    CTexture* pTexture = *ppTexture;
    if( FAILED(hResult = pTexture->LoadStripes(1, dwTextureWidth, dwTextureHeight, 
        2, (DWORD*)d3dColorsStripe, false, false, TEXLOAD_UPDATETEXTURE)))
    {
        return hResult;
    }
    
    if(pTexture->IsPalettized())
		pTexture->SetPalette(NULL);
   
    return S_OK;
}

//----------------------------------------------------------------------------
// Name: TestSetPriority()
// Desc: Tests the return of SetPriority().
// If successful, the resource will have its priority set to
// dwPriority on return
//----------------------------------------------------------------------------
bool CReschk::TestSetPriority(LPRESOURCE pResource, DWORD dwPriority)
{
    DWORD dwPri = 0;
    pResource->SetPriority(RESOURCE_PRIORITY_DISCARD);
    if( RESOURCE_PRIORITY_DISCARD != (dwPri = pResource->SetPriority(dwPriority)) )
    {
        WriteToLog(_T("SetPriority() failed to return previous set priority. Expected %lu ; Got %lu\n"),
                    RESOURCE_PRIORITY_DISCARD, dwPri);
        return false;
    }
    if( dwPriority != (dwPri = pResource->SetPriority(dwPriority)) )
    {
        WriteToLog(_T("SetPriority() failed to return previous set priority. Expected %lu ; Got %lu\n"),
                    dwPriority, dwPri);
        return false;
    }
    
    return true;
}

//----------------------------------------------------------------------------
// Name: TestGetPriority()
// Desc: Tests the return of GetPriority().
//----------------------------------------------------------------------------
bool CReschk::TestGetPriority(LPRESOURCE pResource, DWORD dwPriority)
{
    DWORD dwPri = 0;

    if( true == KeySet(_T("NoGetPri") ) )
        return true;

    if( dwPriority != (dwPri = pResource->GetPriority()) )
    {
        WriteToLog(_T("GetPriority() failed. Expected %lu ; Got %lu\n"), dwPriority, dwPri);
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------
// Name: CreateVBTEX1()
// Desc:
//----------------------------------------------------------------------------
HRESULT CReschk::CreateVBTEX1(CVertexBuffer** ppVertexBuffer)
{
    HRESULT hResult = E_FAIL;

	DIFUSEVERTEX* pdifuseVertices = GetVertices();
    TEX1VERTEX Vertices[] = 
    {
        TEX1VERTEX(pdifuseVertices[0],0,0), 
        TEX1VERTEX(pdifuseVertices[1],1,0), 
        TEX1VERTEX(pdifuseVertices[2],0,1), 
        TEX1VERTEX(pdifuseVertices[3],1,1)
    };
    if( FAILED(hResult = m_pSrcDevice->CreateVertexBuffer(sizeof(Vertices), 
               USAGE_WRITEONLY, D3DFVF_TEX1VERTEX, POOL_DEFAULT, ppVertexBuffer)) )
    {
        WriteToLog(_T("CreateVertexBuffer() failed with HResult = %s.\n"),
                    m_pD3D->HResultToString(hResult));
        return hResult;
    }

    void *pVertices;
    CVertexBuffer* pVB = *ppVertexBuffer;
    if( FAILED(pVB->Lock(0, 0, &pVertices, 0)))
    {
        RELEASE(pVB);
        return hResult;
    }
    memcpy( pVertices, Vertices, sizeof( Vertices ) );
    pVB->Unlock();

    return hResult;
}

//----------------------------------------------------------------------------
// Name: ClearFrame()
// Desc: Clear
//----------------------------------------------------------------------------
bool CReschk::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,100,255), 1.0f, 0);
}

//----------------------------------------------------------------------------
// Name: UpdateStatus()
// Desc: Called once each frame to update status window text
//----------------------------------------------------------------------------
void CReschk::UpdateStatus(void)
{
	// Get the default status
    CD3DTest::UpdateStatus();
}

//----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Cleanup device specific settings, variables, ect. 
// Tied with Setup()
//----------------------------------------------------------------------------
void CReschk::Cleanup(void)
{
    ReleaseCPControl();
}

//----------------------------------------------------------------------------
// Name: CreateCPControl()
// Desc: Create the content protection helper object
//----------------------------------------------------------------------------
bool CReschk::CreateCPControl(D3DAUTHENTICATEDCHANNELTYPE Channel, BOOL bEnable)
{
    HRESULT hr = S_OK;

    // Release
    ReleaseCPControl();

    // Create
    m_pCPControl = new CPControl((LPDIRECT3DDEVICE9EX)m_pSrcDevice->GetDevicePointer9(), Channel, &hr);
    if( NULL == m_pCPControl || FAILED(hr) )
    {
        WriteToLog(_T("Unable to create CPControl"));
        return false;
    }
    
    // Initialize
    hr = m_pCPControl->Initialize(FALSE, FALSE);
    if( FAILED(hr) )
    {
        WriteToLog(_T("m_pCPControl->Initialize(FALSE, FALSE) failed"));
        ReleaseCPControl();
        return false;
    }

    // Configure Protection (bEnable)
    hr = m_pCPControl->ConfigureProtection(bEnable, FALSE);
    if( FAILED(hr) )
    {
        WriteToLog(_T("Failed ConfigureProtection"));
        ReleaseCPControl();
        return false;
    }
    
    return true;
}

//----------------------------------------------------------------------------
// Name: ReleaseCPControl()
// Desc: Cleanup the content protection helper object
//----------------------------------------------------------------------------
bool CReschk::ReleaseCPControl(void)
{
    if( NULL != m_pCPControl )
    {
        delete m_pCPControl;
        m_pCPControl = NULL;
    }
    return true;
}

//----------------------------------------------------------------------------
// Name: SetEncryptionGuidById()
// Desc: Set a SetEncryption GUID by ID
//----------------------------------------------------------------------------
bool CReschk::SetEncryptionGuidById(UINT nEncryptionGuidID)
{
    HRESULT hr = E_FAIL;
    GUID EncryptionGuid;
    if( NULL == m_pCPControl )
    {
        return false;
    }

    // Get Encryption GUID for EncryptionGuidID
    hr = m_pCPControl->QueryEvictionEncryptionGuid(nEncryptionGuidID, &EncryptionGuid);
    if( FAILED(hr) )
    {
        WriteToLog(_T("QueryEvictionEncryptionGuid failed for EncryptionGuidID: %d"), nEncryptionGuidID);
        return false;
    }
    // Set Encryption GUID for EncryptionGuidID
    hr = m_pCPControl->ConfigureEvictionCryption(EncryptionGuid);
    if( FAILED(hr) )
    {
        WriteToLog(_T("ConfigureEvictionCryption failed for EncryptionGuidID: %d"), nEncryptionGuidID);
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------
// Name: TestEncryptionGuids
// Desc: Runs a set of API test on the EncryptionGuids
//----------------------------------------------------------------------------
bool CReschk::TestEncryptionGuids()
{
    bool bRT = false;
    HRESULT hr = E_FAIL;
    UINT nNumEncryptionGuids = 0;
    GUID EncryptionGuid;
    GUID* pGuids = NULL;
    D3DBUSTYPE BusType;
    BOOL AccessNonContigBlocks = FALSE;
    BOOL AccessContigBlocks = FALSE;

    if( NULL == m_pCPControl )
    {
        WriteToLog(_T("TestEncryptionGuids: m_pCPControl is NULL"));
        goto cleanup;
    }

    // Get number of encryption guids to test... nNumEncryptionGuids
    hr = m_pCPControl->QueryEvictionEncryptionGuidCount(&nNumEncryptionGuids);
    if( FAILED(hr) )
    {
        WriteToLog(_T("QueryEvictionEncryptionGuidCount failed"));
        ReleaseCPControl();
        Fail();
        goto cleanup;
    }
    if( 0 == nNumEncryptionGuids )
    {
        WriteToLog(_T("QueryEvictionEncryptionGuidCount returned a count of 0"));
        ReleaseCPControl();
        Fail();
        goto cleanup;
    }

    pGuids = new GUID[nNumEncryptionGuids];
    if( NULL == pGuids )
    {
        WriteToLog(_T("out of memory"));
        goto cleanup;
    }
    ZeroMemory(pGuids, sizeof(pGuids[0]) * nNumEncryptionGuids);

    // Get/Set and save each nNumEncryptionGuids reported
    for( UINT nEncryptionGuidID = 0; nEncryptionGuidID < nNumEncryptionGuids; nEncryptionGuidID++ )
    {
        // Get Encryption GUID for EncryptionGuidID
        hr = m_pCPControl->QueryEvictionEncryptionGuid(nEncryptionGuidID, &EncryptionGuid);
        if( FAILED(hr) )
        {
            WriteToLog(_T("QueryEvictionEncryptionGuid failed for EncryptionGuidID: %d"), nEncryptionGuidID);
            goto cleanup;
        }

        // Set Encryption GUID for EncryptionGuidID
        hr = m_pCPControl->ConfigureEvictionCryption(EncryptionGuid);
        if( FAILED(hr) )
        {
            WriteToLog(_T("ConfigureEvictionCryption failed for EncryptionGuidID: %d"), nEncryptionGuidID);
            goto cleanup;
        }
        
        // Check for Duplicate GUID
        for( UINT i = 0; i < nEncryptionGuidID; i++ )
        {
            if( pGuids[i] == EncryptionGuid )
            {
                WriteToLog(_T("Duplicate GUIDs found"));
                goto cleanup;
            }

        }
        // Save GUID
        pGuids[nEncryptionGuidID] = EncryptionGuid;
    }

    // QueryBusData
    hr = m_pCPControl->QueryBusData(&BusType, &AccessNonContigBlocks, &AccessContigBlocks);
    if( FAILED(hr) )
    {
        WriteToLog(_T("QueryBusData failed"));
        goto cleanup;
    }
    // Until WDDM 2.0, all hw will be accesible in contigious blocks 
    if( FALSE == AccessContigBlocks )
    {
        WriteToLog(_T("AccessContigBlocks is not set"));
        goto cleanup;
    }

    // IV Test
    // Get Encryption GUID for IV EncryptionGuidID
    if( 99 > nNumEncryptionGuids )
    {
        for( UINT i = nNumEncryptionGuids; i < 100; i++)
        {
            hr = m_pCPControl->QueryEvictionEncryptionGuid(i, &EncryptionGuid);
            if( SUCCEEDED(hr) )
            {
                WriteToLog(_T("QueryEvictionEncryptionGuid passed for ID = %d, NumEncryptionGuids = %d"), i, nNumEncryptionGuids);
                goto cleanup;
            }
        }
    }

    bRT = true;
cleanup:
    if( NULL != pGuids ) 
    {
        delete[] pGuids;
        pGuids = NULL;
    }
    return bRT;
}

//----------------------------------------------------------------------------
// Name: SetTestResources()
// Desc: 
//----------------------------------------------------------------------------
HRESULT CReschk::SetTestResources(LPRESOURCE pTest, LPRESOURCE pCopy)
{
    if( NULL == pTest ) return E_FAIL;
    if( NULL == pCopy ) return E_FAIL;

    m_pTestResource = pTest;
    m_pCopyResource = pCopy;

    return S_OK;
}