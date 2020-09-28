// ============================================================================
//
//  CrossProcShareAPI.cpp
//
// ============================================================================
#include "CrossProcShareTest.h"
#include "CTextures.h"
#include <windows.h>

CCrossProcShareTest::CCrossProcShareTest()
{ 
    m_nClients = 1;
    m_dwRTYPE = 0;
    m_dwClientOpenedRTYPE = TYPE_NONE;
}

CCrossProcShareTest::~CCrossProcShareTest()
{
}

// Must be called after SetTestType()
void CCrossProcShareTest::SetTestOptions()
{
    if( KeySet(_T("stress")) )
    {
        SetTestType(TESTTYPE_STRESS);
        m_szTestName    =   _T("CrossProcShareStress");
        m_szLogName     =   _T("CrossProcShareStress");
        m_szCommandKey  =   _T("CrossProcShareStress");

        m_Options.ModeOptions.TextureFmt        = FMT_ANY;
        m_Options.ModeOptions.ZBufferFmt        = FMT_ANY;
        m_Options.ModeOptions.CubeFmt           = FMT_ANY;
        m_Options.ModeOptions.VolumeFmt         = FMT_ANY;
        m_Options.ModeOptions.BackBufferFmt     = FMT_ANY;
        m_Options.ModeOptions.MultisampType     = MULTISAMPLE_ANY;
        m_Options.ModeOptions.uMaxDisplayModes  = 1;
    }

    // Use these flags so that no backbuffer or DepthStencil surface are in D3DPOOL_DEFAULT for the reset
    m_Options.D3DOptions.bCachedBackBuffer  = false;
    m_Options.D3DOptions.bZBuffer           = false;

    m_Options.D3DOptions.dwWinMode  = WINMODE_WINDOWED;
    m_Options.D3DOptions.dwDevTypes = DEVICETYPE_HAL;
    m_Options.D3DOptions.bReference = false;

    m_Options.D3DOptions.fMinDXVersion = 9.1f;

    m_pFramework->GetImageCmp()->SetPassRequired(0.98f);
}

bool CCrossProcShareTest::CapsCheck()
{
	if( false == CD3DTest::CapsCheck() )
		return false;

	if( false == (m_pSrcDevice->GetCaps()->dwCaps2 & D3DCAPS2_CANSHARERESOURCE) )
		return false;

	return true;
}

bool CCrossProcShareTest::Setup(void)
{
    // Make sure we are on 9.L or up
    if( 9.1f > m_pD3D->GetVersion() )
    {
        WriteToLog(_T("Test needs DX9.L to run. Version = %f.\n"), m_pD3D->GetVersion());
        return false;
    }

    return true;
}

UINT CCrossProcShareTest::TestInitialize(void)
{
    if( false == m_bServer && true == g_bClientDone )
        return D3DTESTINIT_SKIPALL;

    UINT uResult = CD3DTest::TestInitialize();

	if( D3DTESTINIT_RUN != uResult )
		return uResult;

	// Check if more than one adapter in system
	m_nAdapters = m_pD3D->GetAdapterCount();
	if( false == m_bServer )
		m_bMultiheadAdapter = m_nAdapters > 1 && m_pSrcDevice->GetCaps()->NumberOfAdaptersInGroup > 1;
	else
		m_bMultiheadAdapter = m_pSrcDevice->GetCaps()->NumberOfAdaptersInGroup == 0;

	m_fDXVersion = m_DeviceList[0]->GetInterface();

    // Init all resource cases.
	m_EnumResource.Init(this);

    m_uCurrentDefaultCase   = -1;
    m_uCurrentSysMemCase    = -1;

    // Set test range
	if( true == m_bServer )
	{
		CreateTestFactors();
		SetTestFactors();

		if( false == m_rCasesManager->Valid() || 0 == m_rCasesManager->Count() )
		{
			SetTestRange(1,1);
            OutputDebugString( _T("TestInitialize: Error: Skiping all tests\n") );
            return D3DTESTINIT_SKIPALL;
		}

		SetTestRange(1, m_rCasesManager->Count());
	}
	else
	{
		SetTestRange(1, 1);
	}

    return uResult;
}

bool CCrossProcShareTest::TestTerminate(void)
{
	CCommunication::TestTerminate();

    ReleaseTest();

	m_EnumResource.Clean();

    g_bClientDone = true;

	return true;
}

void CCrossProcShareTest::ReleaseTest()
{
	m_rCasesManager.Release();
}

bool CCrossProcShareTest::ExecuteTest(UINT uTestNumber)
{ 
    UINT uResult = TEST_SUCCESS;
    DWORD dwTestId = 0; // Test case id

    if( false == m_bServer )
    {
        int offset = 200 + (m_iClientId + 1) * 20;
        m_pFramework->Move(offset, offset);
        BeginTestCase(_T("ClientTest"), uTestNumber);
        WaitForMessagesFromServer();
        Skip();
        return false;
    }

    if( false == SetTestCase(uTestNumber) )
    {
        BeginTestCase(_T("SetTestCase_FAILD"), uTestNumber);
        Log(1, _T("ExecuteTest(): SetTestCase() Fail.\n") );
        Skip();
        return false;
    }

    // Get Test ID.
    dwTestId = m_listTestCases[ m_rTestCases->Value() ].dwTestID;

    // Make test case name
    sprintf(m_szFullName, _T("G%d T%d %s - %s"), m_pFramework->GetCurrentTestNum(), uTestNumber, m_listTestCases[ m_rTestCases->Value() ].szTestName,
        m_bReadOnlyCase ? _T("Read-Only") : _T("Read-Write") );

    // Set our test case
    RESOURCE_CASE defaultResCase;
    m_pDefaultResCase = &defaultResCase;
    m_pSysMemResCase  = NULL;
    if( false == GetCase(uTestNumber) )
    {
        BeginTestCase(m_szFullName, uTestNumber);
        Log(1, _T("ExecuteTest() GetNextCase() Fail.\n") );
        Fail();
        return false;
    }

    // Add resouce case to test name
    m_EnumResource.MakeCaseName(m_dwRTYPE, m_pDefaultResCase, m_szFullName);

    // Start the test case
    BeginTestCase(m_szFullName, uTestNumber);

    // Start the clients
    if( false == StartClients() )
    {
        Log(1, _T("StartClients() Failed.\n") );
        Skip();
        return false;
    }
    
    // Cleanup the client log and fail status
    ResetClientLog();
    ResetClientFailStatus();

    // Have main client log case
    SendMessageToClient( 0, MSG_ACT_LOGCASE );
    if( false == WaitForClientToBeDone( 0, 5000 ) )
    {
        Log(1, _T("ExecuteTest() Client 1 can not log.\n") );
        Skip();
        return false;
    }

    // Filter
	if( KeySet(_T("readonly")) && false == m_bReadOnlyCase )
	{
		Skip();
		Log(1, _T("ExecuteTest() Skiping Read-Write cases.\n") );
		goto Cleanup;
	}
	if( KeySet(_T("readwrite")) && true == m_bReadOnlyCase )
	{
		Skip();
		Log(1, _T("ExecuteTest() Skiping Read-Only cases.\n") );
		goto Cleanup;
	}

    // Run test
	uResult = ServerDispatchTest(dwTestId);

    // Log a fail if any of the clients failed
    if( false == DidAllClientsSucceeded() )
    {
        Log(1, _T("Client has failed\n") );
        uResult = TEST_FAIL;
    }

    // Check for client skips
    if( true == DidClientsSkip() )
    {
        Log(1, _T("Client has skiped test case\n") );
        uResult = TEST_SKIP;
    }

    // Log any client messages
    LogClientsMessages();

    if( uResult == TEST_FAIL )
        Fail();
    else if( uResult == TEST_SKIP )
        Skip();

Cleanup:
	
    // Client release resources
    SendMessageToClient(0, MSG_RES_RELEASE);
    WaitForClientToBeDone(0, 5000);

    // Server release
    RELEASE(m_pNonSharedResource);
	RELEASE(m_pResource);

	GetLastError();

	return false;
}

void CCrossProcShareTest::ProcessError(HRESULT hr)
{
    if( D3DERR_DRIVERINTERNALERROR != hr )
	    CD3DTest::ProcessError(hr);
}

bool CCrossProcShareTest::GetNextCase()
{
	if ( true == m_bReadOnlyCase) // read-only case
	{
        m_pSysMemResCase = m_EnumResource.GetNextCase(m_dwRTYPE, POOL_SYSTEMMEM, m_uCurrentSysMemCase);
		if( NULL == m_pSysMemResCase )
		{
			Log( 1, _T("GetNextCase() - Failed for Read-Only case.\n"));
            return false;
		}

        if( NULL == m_pDefaultResCase )
        {
            Log( 1, _T("GetNextCase() - Fail setting default case in read only case.\n"));
            return false;
        }
        
		memcpy(m_pDefaultResCase, m_pSysMemResCase, sizeof(RESOURCE_CASE));
		m_pDefaultResCase->dwPool = POOL_DEFAULT;

	}
	else  // read-write case
	{
        m_pSysMemResCase  = NULL;
        m_pDefaultResCase = m_EnumResource.GetNextCase(m_dwRTYPE, POOL_DEFAULT, m_uCurrentDefaultCase);
		if( NULL == m_pDefaultResCase )
		{
			Log( 1, _T("GetNextCase() - Failed for Read-Write case.\n"));
            return false;
		}
	}

    // Set the shared memory for the client
    memcpy(&m_pSharedMemory->resCase, m_pDefaultResCase, sizeof(RESOURCE_CASE));
    m_pSharedMemory->bReadOnlyResource = m_bReadOnlyCase;

    return true;
}

bool CCrossProcShareTest::GetCurrentCase()
{
	if ( true == m_bReadOnlyCase) // read-only case
	{
        m_pSysMemResCase = m_EnumResource.GetCurrentCase(m_dwRTYPE);
		if( NULL == m_pSysMemResCase )
		{
			Log( 1, _T("GetCurrentCase() - Failed for Read-Only case.\n"));
            return false;
		}

        if( NULL == m_pDefaultResCase )
        {
            Log( 1, _T("GetCurrentCase() - Fail setting default case in read only case.\n"));
            return false;
        }
        
		memcpy(m_pDefaultResCase, m_pSysMemResCase, sizeof(RESOURCE_CASE));
		m_pDefaultResCase->dwPool = POOL_DEFAULT;

	}
	else  // read-write case
	{
        m_pSysMemResCase  = NULL;
        m_pDefaultResCase = m_EnumResource.GetCurrentCase(m_dwRTYPE);
		if( NULL == m_pDefaultResCase )
		{
			Log( 1, _T("GetCurrentCase() - Failed for Read-Write case.\n"));
            return false;
		}
	}

    // Set the shared memory for the client
    memcpy(&m_pSharedMemory->resCase, m_pDefaultResCase, sizeof(RESOURCE_CASE));
    m_pSharedMemory->bReadOnlyResource = m_bReadOnlyCase;

    return true;
}

bool CCrossProcShareTest::GetCase(UINT nTestNum)
{
	bool bRT = false;
	static UINT nLastTestNum = 0;
	
	if( 1 == nTestNum ) // reset resource cases
	{
		for (int i=0; i<TYPE_MAXTYPE; i++)
		{
			m_EnumResource.ResetCurrentCase(i);
		}
		bRT = GetNextCase();
	}
	else if( nLastTestNum == nTestNum  ) // get current resource case
	{
		bRT = GetCurrentCase();
	}
	else if( nLastTestNum + 1 == nTestNum ) // get the next resource case
	{
		bRT = GetNextCase();
	}
	else // dont alow skiping of resource cases
	{
		Log( 1, _T("GetCase() - Failed. Cant skip test cases. Please go to test %d\n"), nLastTestNum);
        bRT = false;
	}

	if( true == bRT )
		nLastTestNum = nTestNum;

    return bRT;
}

bool CCrossProcShareTest::CreateTestFactors()
{
    if(	false == CHECKREF(m_rTestCases = NewFactor( m_listTestCases.size() )) )
    {
        Log(0, _T("CreateTestFactors() - ERROR creating m_rProtCases") );
        return false;
    }

    for( int iType = 0; iType < TYPE_MAXTYPE; iType++ )
    {
        if(	false == CHECKREF(m_rCasesDefault[iType] = NewFactor( m_EnumResource.GetNbCasesForPool(iType, POOL_DEFAULT) )))
        {
            Log(0, _T("CreateTestFactors() - ERROR creating m_rCasesDefault") );
            return false;
        }

        if(	false == CHECKREF(m_rCasesSysmem[iType] = NewFactor( m_EnumResource.GetNbCasesForPool(iType, POOL_SYSTEMMEM) )))
        {
            Log(0, _T("CreateTestFactors() - ERROR creating m_rCasesSysmem") );
            return false;
        }
    }

	return true;
}

void CCrossProcShareTest::SetTestFactors()
{
    RFactor rCasesReadOnly;
    RFactor rCasesReadWrite;

    rCasesReadWrite =   m_rCasesDefault[TYPE_TEXTURE] +
                        m_rCasesDefault[TYPE_VERTEXBUFFER] + 
                        m_rCasesDefault[TYPE_OFFSCREENPLAIN] +
                        m_rCasesDefault[TYPE_RENDERTARGET] + 
                        m_rCasesDefault[TYPE_DEPTHSTENCIL] +
                        m_rCasesDefault[TYPE_CUBETEXTURE] +
                        m_rCasesDefault[TYPE_VOLUMETEXTURE] +
                        m_rCasesDefault[TYPE_INDEXBUFFER];

    rCasesReadOnly =    m_rCasesSysmem[TYPE_TEXTURE] + 
                        m_rCasesSysmem[TYPE_VERTEXBUFFER] +
                        m_rCasesSysmem[TYPE_OFFSCREENPLAIN] + 
                        m_rCasesSysmem[TYPE_CUBETEXTURE] + 
                        m_rCasesSysmem[TYPE_VOLUMETEXTURE] + 
                        m_rCasesSysmem[TYPE_INDEXBUFFER];

	m_rCasesManager =	m_rTestCases * (rCasesReadWrite + rCasesReadOnly);
}

bool CCrossProcShareTest::StartClients()
{
    if( 0 == m_listClientProcess.size() )
    {
        for( int i = 0; i < m_nClients; i++ )
        {
            if( false == CreateClientProcess(0, false) )
            {
                Log(1, _T("StartClients() Client 0 failed to start.\n") );
                return false;
            }
        }
    }
    
    // clients ready?
    if( 0 == m_listClientProcess.size() )
    {
        Log(1, _T("ExecuteTest() no clients started.\n") );
        return false;
    }
    for( int i = 0; i < m_nClients; i++ )
    {
        SendMessageToClient( i, MSG_PING );
        if( false == WaitForClientToBeDone( i, 1000) )
        {
            Log(1, _T("ExecuteTest() Clients are not ready.\n") );
            return false;
        }
    }   
    return true;
}

bool CCrossProcShareTest::SetTestCase(UINT uTestNumber)
{
    m_rCasesManager->Set(uTestNumber - 1);
    m_bReadOnlyCase = false;

    for( int iType=0; iType < TYPE_MAXTYPE; iType++ )
    {
        if( m_rCasesDefault[iType]->Active() )
        {
            // if we are switching types reset
            if( m_dwRTYPE != iType )
                m_EnumResource.ResetCurrentCase(iType);

            m_dwRTYPE = iType;
            if (m_uCurrentDefaultCase+1 == m_EnumResource.GetLastCaseForPool(iType, POOL_DEFAULT))
                m_uCurrentDefaultCase = -1;
        }

        if( m_rCasesSysmem[iType]->Active() )
        {
            // if we are switching types reset
            if( m_dwRTYPE != iType )
                m_EnumResource.ResetCurrentCase(iType);

            m_dwRTYPE = iType;
            if (m_uCurrentSysMemCase+1 == m_EnumResource.GetLastCaseForPool(iType, POOL_SYSTEMMEM))
                m_uCurrentSysMemCase = -1;
            m_bReadOnlyCase = true;
        }
    }
    return true;
}

void CCrossProcShareTest::DumpCaseInfo(const TCHAR *szTestName, RESOURCE_CASE *pResCase, HANDLE hShared, bool bReadOnly)
{
    TCHAR szTestCaseInfo[2048];

    _stprintf(szTestCaseInfo, _T("\n%s Test Case:\n"), m_bServer ? _T("Server") : _T("Client") );
    
    if( NULL != szTestName )
        _stprintf(szTestCaseInfo, _T("%s  Test Case: %s\n"), szTestCaseInfo, szTestName);

    _stprintf(szTestCaseInfo, _T("%s  Read-Only : %s\n"), szTestCaseInfo, bReadOnly ? _T("True") : _T("False") );
    m_EnumResource.DumpCaseDetails(pResCase->dwRType, pResCase, szTestCaseInfo);
    _stprintf(szTestCaseInfo, _T("%s *pSharedHandle = NULL: %s\n\n"), szTestCaseInfo, NULL==hShared ? _T("True") : _T("False") );
    Log(1, szTestCaseInfo);
}