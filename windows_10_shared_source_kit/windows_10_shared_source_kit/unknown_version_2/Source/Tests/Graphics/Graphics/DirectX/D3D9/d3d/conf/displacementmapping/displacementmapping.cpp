ncryptionGuids);
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
}#include "include.fx"
$include "PShader.prep"
${##################################################################################################

# Factors to take into account:
# 	comparison					- lstComparisons (OpMap maps comparison ops to functions that perform the comparison)
#	write mask					- lstWriteMasks
#	src reg type
#	input value/sign			- lstInputCombinations (ValueMap maps symbols to values)
#	source modifiers: neg, abs
#	component swizzles			- lstSimpleSwizzles, lstArbitrarySwizzles, lstRandomSwizzles
#								- lstUsedSwizzles: set of above three

# Comparisons (6):
# _gt Greater than 
# _lt Less than 
# _ge Greater than or equal 
# _le Less than or equal 
# _eq Equal to 
# _ne Not equal to
lstComparisons = 'gt,lt,ge,le,eq,ne'.split(',')

### index the following with the comparison index and the result should be all true, used to test mask
##lstInputsForTrueResults = [(a*4,b*4) for a,b in zip('000000', '-+000+')]
##
### Input Value (20):
### Semi-hand crafted comparison sets
###		|   |   |   |   |   |   |   |
##pat = ['++00+-0--+0-', '+0+0-+-0-00--+']
##lstInputCombinations = []
##for n in range(20):
##	i = (n*4) % (len(pat[0])-3)
##	j = (n*4) % (len(pat[1])-3)
##	lstInputCombinations.append((pat[0][i:i+4], pat[1][j:j+4]))
##
### mapping between codes and values:
##ValueMap = dict(zip('-0+', (-1,0,1)))

# Map comparison operation names to functions that perform the comparison
OpMap = {
	'gt': lambda a,b: a>b,
	'lt': lambda a,b: a<b,
	'ge': lambda a,b: a>=b,
	'le': lambda a,b: a<=b,
	'eq': lambda a,b: a==b,
	'ne': lambda a,b: a!=b }

values = [20, 0, 32, -12, -20, 0, 32, 16]
fValues = [n/16.0 for n in values]
src_defaults = fValues[:4], fValues[4:]
fValues = [abs(n)/32.0 for n in values]
color_defaults = fValues[:4], fValues[4:]
del fValues

default_result = 0

if ps_3_0:
	regcombos = [ 'rcv',
				  'rvc' ]
else:
	regcombos = [ 'rcvt',
				  'rvcr' ]
regcombos = [c for c in apply(zip, regcombos)]

SetupDefaults(src_defaults, color_defaults)

def ExpandPredSrc(src):
	if type(src) is not str:
		return src
	else:
		return [ValueMap[c] for c in src]

class Op_Setp(Operation):
	def __init__(s, cmpop):
		s.cmpop = cmpop
		Operation.__init__(s, 'setp')
		s.desc = 'setp_%s'
	def __str__(s):
		return s.desc % s.cmpop
	def Apply(s, src0, src1):
		result = [int(OpMap[s.cmpop](v0,v1)) for v0,v1 in zip(src0.Transform(), src1.Transform())]
		return result

class Inst_Setp(Instruction):
	def __init__(s, op, dest, sources, result, verifymask):
		Instruction.__init__(s, op, Predication(), dest, sources, result)
		s.verifymask = verifymask
		s.DeclareConstantsPerTestcase = True
	def ClearOutputReg(s):
		if s.verifymask:
			s.write('CLEAR_P')
	def SetEpsReg(s):
		pass
	def ResultConst(s):
		pass
	def VerifyResult(s):
		s.write('mov r2, c0.y')
		s.write('mov r2.gw, c0.x')
		if s.verifymask:
			checkresult = ['%s%s' % (c,r) for c,r in zip('XYZW', s.maskedresult)]
		else:
			checkresult = ['%s%s' % ('XYZW'[n],s.result[n]) for n in range(4) if s.outputmask[n]]
		s.write(' '.join(checkresult))

def WriteTestCase(
	sat=0, pp=0, op=0, mask=0,						# instruction & destination modifiers
	reg0='r', src0=None, neg0=0, abs0=0, swz0=0,	# source 0
	reg1='r', src1=None, neg1=0, abs1=0, swz1=0,	# source 1
	verifymask=0,									# whether to verify a masked result
	**extra_params):
	global curCount

	assert op, 'must specify cmp operation to perform'

	# Initialize sources:
	sources = CollapseSources(locals(), 2)
	sources = [apply(Source, src) for src in sources]
	for src in sources:
		src.value = ExpandPredSrc(src.value)

	# Initialize other objects:
	dest = Destination('p0', mask)
	op = Op_Setp(op)
	result = apply(op.Apply, sources)

	# Instruction take