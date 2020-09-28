// ============================================================================
//
//  CrossProcShareConf.cpp
//
// ============================================================================
#include "ProtectedProcess.h"
#include "CTextures.h"
#include <windows.h>

TESTCASE g_PROCases[] =
{
    { PRO_CREATE,		       _T("Create") },			
    { PRO_FAILOPEN,            _T("FailOpen") },			
    { PRO_VERIFYNONPRO,        _T("Verify_Non-Protected") },
    { PRO_VERIFYPRO,           _T("Verify_Protected") },
    { PRO_FILLNONPRO,          _T("Fill_Non-Protected") },
    { PRO_FILLPRO,             _T("Fill_Protected") },
};
const UINT cPROCases = sizeof(g_PROCases) / sizeof(g_PROCases[0]);

CProtectedProcess::CProtectedProcess()
{ 
    m_szCommandKey  = _T("CrossProcShare_PRO");
    m_szTestName    = _T("CrossProcShare_PRO");
    m_szLogName     = _T("CrossProcShare_PRO");
    SetTestType(TESTTYPE_CONF);

    // Let CCrossProcShareTest options after SetTestType() set the default.
    SetTestOptions();

    // Client 0 acts as a protected server for creating resources
    // Client 1 acts as a protected client
    // Server acts as a non-protected client
    m_nClients = 2;

    // Determine which tests to run from the command line params
    SetTestsToRun();
}

CProtectedProcess::~CProtectedProcess()
{
}

void CProtectedProcess::CommandLineHelp(void)
{
    //TODO: write correct command line options
    WriteCommandLineHelp("\n$gTEST CASES");
    WriteCommandLineHelp("\n$g\tFor all the following flag, if '!' is put before 'TEST', the test case will");
    WriteCommandLineHelp("\n$g\tbe excluded(eg. -!TEST_LOCKWHILELOST).  Many test cases can be excluded.  Otherwise,");
    WriteCommandLineHelp("\n$g\tONLY the test case of the TEST parameter will be executed.\n");
    WriteCommandLineHelp("$y-TEST_CREATIONWHILELOST\t$wTest case that creates a resource while the dev is lost");
}

void CProtectedProcess::SetTestsToRun()
{
    TCHAR	szOption[512] = _T("");
    DWORD dwPROTests = 0xffffffff;

    //C heck if we override the conf tests
    if( ReadString(_T("PRO"), szOption, ARRAYSIZE(szOption)) )
    {
        for( int iCase=0; iCase < cPROCases; iCase++ )
        {
            if( 0 == _tcsicmp(szOption, g_PROCases[iCase].szTestName))
            {
                dwPROTests = g_PROCases[iCase].dwTestID;
            }
        }
    }

    for( int iCase=0; iCase < cPROCases; iCase++ )
    {
        if( dwPROTests & g_PROCases[iCase].dwTestID )
        {
            m_listTestCases.push_back(g_PROCases[iCase]);
        }
    }
}

UINT CProtectedProcess::ServerDispatchTest(DWORD dwTestID)
{
    UINT uResult = TEST_SUCCESS;

    switch( dwTestID )
	{
		case PRO_CREATE:
			uResult = Test_Server_Create(m_dwRTYPE);
			break;

		case PRO_FAILOPEN:
			uResult = Test_Server_FailOpen(m_dwRTYPE);
			break;

		case PRO_VERIFYNONPRO:
			uResult = Test_Server_VerifyFill(m_dwRTYPE, false, false);
			break;

        case PRO_FILLNONPRO:
			uResult = Test_Server_VerifyFill(m_dwRTYPE, false, true);
			break;

        case PRO_VERIFYPRO:
			uResult = Test_Server_VerifyFill(m_dwRTYPE, true, false);
			break;

        case PRO_FILLPRO:
			uResult = Test_Server_VerifyFill(m_dwRTYPE, true, true);
			break;

		default :
			uResult = TEST_FAIL;
            Log( 1, _T("ServerDispatchTest() - Got unknown test ID.") );
	}

    return uResult;
}

bool CProtectedProcess::StartClients()
{
    if( 0 == m_listClientProcess.size() )
	{
        // Client 0 is protected and acts as the server for creating resources.
        if( false == CreateClientProcess(0,true) )
        {
            Log(1, _T("ExecuteTest::CreateClientProcess() 0 Failed.\n") );
            return false;
        }

        // Client 1 is protected and acts as a client for the protected to protected cases.
        if( false == CreateClientProcess(0,true) )
        {
            Log(1, _T("ExecuteTest::CreateClientProcess() 1 Failed.\n") );
            return false;
        }
	}

    return true;
}

// ----------------------------------------------------------------------------
//		The Tests
// ----------------------------------------------------------------------------
UINT CProtectedProcess::Test_Server_Create(DWORD dwRTYPE)
{
    // Protected client create protected resource
    SendMessageToClient( 0, MSG_RES_CREATE);
    WaitForClientToBeDone( 0, 50000 );

    // Client wants to skip the case...
    if( true == DidClientsSkip() )
        return TEST_SUCCESS;

    // Log what failed
    if( false == DidAllClientsSucceeded() )
        Log( 1, _T("Test_Server_Create() - Client failed create of protected resource.\n"));

    // Protected client create non-protected resource
    SendMessageToClient( 0, MSG_RES_CREATENONPRO);
    WaitForClientToBeDone(0, 50000);

    // Log what failed
    if( false == DidAllClientsSucceeded() )
        Log( 1, _T("Test_Server_Create() - Client failed create of non-protected resource.\n"));

    return TEST_SUCCESS;
}

UINT CProtectedProcess::Test_Server_FailOpen(DWORD dwRTYPE)
{
    // Protected client create protected resource
    SendMessageToClient( 0, MSG_RES_CREATE);
    WaitForClientToBeDone(0, 50000);

    // Log case info
    DumpCaseInfo(m_listTestCases[ m_rTestCases->Value() ].szTestName, m_pDefaultResCase, m_hShared, m_bReadOnlyCase );

    // If a client failed/skiped there is nothing more we can do.
    if( false == DidAllClientsSucceeded() || true == DidClientsSkip() )
    {
        Log( 1, _T("Test_Server_FailOpen() - Client failed. Test case ending.\n"));
        return TEST_SUCCESS;
    }

    // Get the resource case
    RESOURCE_CASE *pResCase = &m_pSharedMemory->resCase;
    HANDLE hShared = m_pSharedMemory->hShared;
    CObject *pResource = NULL;
    if( NULL == pResCase || NULL == m_pSharedMemory->hShared )
    {
        // Fail - bad test case from client.
        Log( 1, _T("Test_Server_FailOpen() - Client did not enter a valid test case.\n"));
        return TEST_FAIL;
    }

    // Try to open. Should fail because we are not a protected process
    HRESULT hr = m_EnumResource.CreateResource(pResCase->dwRType, pResCase, &pResource, &hShared);
    if( FAILED(hr) )
    {
        pResource = NULL;
        return TEST_SUCCESS;
    }
    RELEASE(pResource);

    // Fail because we should not be able to open the resource
    Log( 1, _T("Test_Server_FailOpen() - Able to open a protected resource from a non-protected process.\n"));

    return TEST_FAIL;
}

UINT CProtectedProcess::Test_Server_VerifyFill(DWORD dwRTYPE, bool bProtected, bool bFill)
{
    // Let client create resource
    if( true == bProtected )
        SendMessageToClient( 0, MSG_RES_CREATE);
    else
        SendMessageToClient( 0, MSG_RES_CREATENONPRO);

    WaitForClientToBeDone(0, 50000);

    // Log case info
    DumpCaseInfo(m_listTestCases[ m_rTestCases->Value() ].szTestName, m_pDefaultResCase, m_hShared, m_bReadOnlyCase );

    // If a client failed/skiped there is nothing more we can do.
    if( false == DidAllClientsSucceeded() || true == DidClientsSkip() )
    {
        Log( 1, _T("Test_Server_VerifyFill() - Client failed. Test case ending.\n"));
        return TEST_SUCCESS;
    }

    // non-protected proccess try to open the resource
    if( false == bProtected )
    {
        // Get the resource case
        RESOURCE_CASE *pResCase = &m_pSharedMemory->resCase;
        HANDLE hShared = m_pSharedMemory->hShared;
        CObject *pResource = NULL;
        if( NULL == pResCase || NULL == m_pSharedMemory->hShared)
        {
            // Fail - bad test case from client.
            Log( 1, _T("Test_Server_VerifyFill() - Client did not enter a valid test case.\n"));
            return TEST_FAIL;
        }

        // Try to open
        HRESULT hr = m_EnumResource.CreateResource(pResCase->dwRType, pResCase, &pResource, &hShared);
        if( FAILED(hr) )
        {
            pResource = NULL;
            Log( 1, _T("Test_Server_VerifyFill() - Failed to open a resource created with D3DUSAGE_NONSECURE from a non-protected process. %s\n"), m_pD3D->HResultToString(hr));
	        return TEST_FAIL;
        }

        if( true == bFill && false == m_bReadOnlyCase ) // Fill test
        {
             // Verify content
            if( false == FillResource(pResCase, pResource, 1) )
            {
                Log( 1, _T("Test_Server_VerifyFill() - Resource from protected proccess failed VerifyContent(Pattern 1).\n") );
                RELEASE(pResource);
                return TEST_FAIL;
            }
            if( false == VerifyContent(pResCase, pResource, 1) )
            {
                Log( 1, _T("Test_Server_VerifyFill() - Resource from protected proccess failed VerifyContent(Pattern 1) after FillResource() passed.\n") );
                RELEASE(pResource);
                return TEST_FAIL;
            }
        }
        else // verify test
        {
            // Verify content
            if( false == VerifyContent(pResCase, pResource, 0) )
            {
                Log( 1, _T("Test_Server_VerifyFill() - Resource from protected proccess failed VerifyContent(Pattern 0).\n") );
                RELEASE(pResource);
                return TEST_FAIL;
            }
        }
        RELEASE(pResource);
    }

    // Client 1 has put its test case in the log yet. So so before testing with it.
    SendMessageToClient( 1, MSG_ACT_LOGCASE );
    WaitForClientToBeDone( 1, 50000);	

    // Let the client protected proccess open the resource
    SendMessageToClient( 1, MSG_RES_OPEN );
    WaitForClientToBeDone( 1, 50000 );

    // If a client failed/skiped there is nothing more we need to do.
    if( false == DidAllClientsSucceeded() || true == DidClientsSkip() )
    {
        Log( 1, _T("Test_Server_VerifyFill() - Client failed. Test case ending.\n"));
        return TEST_SUCCESS;
    }

    if( true == bFill && false == m_bReadOnlyCase ) // Fill test
    {
        // Let the client protected proccess fill test
        SendMessageToClient( 1, MSG_RES_FILLONE );
        WaitForClientToBeDone( 1, 50000 );
    }
    else
    {
        // Let the client protected proccess verify resource
        SendMessageToClient( 1, MSG_RES_VERIFYZERO );
        WaitForClientToBeDone( 1, 50000 );
    }

    // Let the client protected proccess release resource
    SendMessageToClient( 1, MSG_RES_RELEASE );
    WaitForClientToBeDone( 1, 50000 );

    return TEST_SUCCESS;
}