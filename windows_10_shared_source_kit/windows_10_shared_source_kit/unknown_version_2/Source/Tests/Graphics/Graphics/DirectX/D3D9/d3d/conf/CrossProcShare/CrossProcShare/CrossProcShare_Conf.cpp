// ============================================================================
//
//  CrossProcShare_Conf.cpp
//
// ============================================================================
#include "CrossProcShare_Conf.h"
#include "CTextures.h"
#include <windows.h>

TESTCASE g_CONFCases[] =
{
	{ CONF_CREATE,		            _T("Create") },			
	{ CONF_OPEN,                    _T("Open") },	
    { CONF_CLIENTOPEN,              _T("ClientOpen") },
	{ CONF_VERIFY,                  _T("Verify") },
    { CONF_FILL,                    _T("Fill") },
    { CONF_LOCK,                    _T("Lock") },
	{ CONF_RELEASESOURCEUSEOPENED,  _T("ReleaseSourceUseOpen") },
};
const UINT cCONFCases = sizeof(g_CONFCases) / sizeof(g_CONFCases[0]);

CCrossProcShare_Conf::CCrossProcShare_Conf()
{ 
    m_szCommandKey  = _T("CrossProcShare_Conf");
    m_szTestName    = _T("CrossProcShare_Conf");
    m_szLogName     = _T("CrossProcShare");
    SetTestType(TESTTYPE_CONF);

    // Let CCrossProcShareTest options after SetTestType() set the default.
    SetTestOptions();

    m_nClients = 1;

    // Determine which tests to run from the command line params
    SetTestsToRun();
}

CCrossProcShare_Conf::~CCrossProcShare_Conf()
{
}

void CCrossProcShare_Conf::CommandLineHelp(void)
{
    WriteCommandLineHelp("\n$gTest cases:");
	WriteCommandLineHelp("\n$y\t-CONF:Create - Create shared resource.");
	WriteCommandLineHelp("\n$y\t-CONF:Open - Create and open shared resource in same proccess.");
	WriteCommandLineHelp("\n$y\t-CONF:ClientOpen - Create shared in Proccess1 and open in Process2.");
	WriteCommandLineHelp("\n$y\t-CONF:Verify - Process2 verify that opened resource's content is correct.");
	WriteCommandLineHelp("\n$y\t-CONF:Fill - Process2 fills open resource and Process1 verify content.");
	WriteCommandLineHelp("\n$y\t-CONF:Lock - Process1 locks resource, Process2 locks (should fail), Process2 ");
	WriteCommandLineHelp("$y\t unlock (should fail), Process1 unlock, Process2 lock (should fail), Process2 unlock (should fail), Process1 unlock (should fail).");
	WriteCommandLineHelp("\n$y\t-CONF:ReleaseSourceUseOpen - Process1 create resource, Proccess2 open resource, ");
	WriteCommandLineHelp("\n$y\t Proccess2 verify content of resource, Proccess1 release resource, Process2 use resource, Process1 open resource, Process1 update resource content.");
	WriteCommandLineHelp("\n$gTo run with only one resource type use -ResourceType:<ResourceType>");
	WriteCommandLineHelp("\n$gResource types that can be used are:");
	WriteCommandLineHelp("\n$y\tOFFSCREENPLAIN");
	WriteCommandLineHelp("\n$y\tDEPTHSTENCIL");
	WriteCommandLineHelp("\n$y\tRENDERTARGET");
	WriteCommandLineHelp("\n$y\tTEXTURE");
	WriteCommandLineHelp("\n$y\tVOLUMETEXTURE");
	WriteCommandLineHelp("\n$y\tVERTEXBUFFER");
	WriteCommandLineHelp("\n$y\tINDEXBUFFER");
}

void CCrossProcShare_Conf::SetTestsToRun()
{
    TCHAR szOption[512] = _T("");
    DWORD dwCONFTTests   = 0xffffffff;

    // Check if we override the conf tests
    if( ReadString(_T("CONF"), szOption, ARRAYSIZE(szOption)) )
    {
        for( int iCase=0; iCase < cCONFCases; iCase++ )
        {
            if( 0 == _tcsicmp(szOption, g_CONFCases[iCase].szTestName))
            {
                dwCONFTTests = g_CONFCases[iCase].dwTestID;
            }
        }
    }

    for( int iCase=0; iCase < cCONFCases; iCase++ )
    {
        if( dwCONFTTests & g_CONFCases[iCase].dwTestID )
        {
            m_listTestCases.push_back(g_CONFCases[iCase]);
        }
    }
}

UINT CCrossProcShare_Conf::ServerDispatchTest(DWORD dwTestID)
{
    UINT uResult = TEST_SUCCESS;

    switch( dwTestID )
	{
		case CONF_CREATE:
			uResult = Test_Server_Create(m_dwRTYPE);
			break;
        
        case CONF_OPEN:
			uResult = Test_Server_Open(m_dwRTYPE);
			break;

        case CONF_CLIENTOPEN:
			uResult = Test_Server_ClientOpen(m_dwRTYPE);
			break;

        case CONF_VERIFY:
			uResult = Test_Server_Verify(m_dwRTYPE);
			break;

        case CONF_FILL:
			uResult = Test_Server_Fill(m_dwRTYPE);
			break;

        case CONF_LOCK:
			uResult = Test_Server_Lock(m_dwRTYPE);
			break;

        case CONF_RELEASESOURCEUSEOPENED:
			uResult = Test_Server_ReleaseSourceUseOpen(m_dwRTYPE);
			break;

		default :
			uResult = TEST_FAIL;
            Log( 1, _T("ServerDispatchTest() - Got unknown test ID.") );
	}

    return uResult;
}

// ----------------------------------------------------------------------------
//		The Tests
// ----------------------------------------------------------------------------
UINT CCrossProcShare_Conf::Test_Server_Create(DWORD dwRTYPE)
{
    UINT uResult = TEST_SUCCESS;
    // Log case info
    DumpCaseInfo(m_listTestCases[ m_rTestCases->Value() ].szTestName, m_pDefaultResCase, m_hShared, m_bReadOnlyCase );

    // Create resource.
    uResult = CreateResource_FromSharedMem();
    if( TEST_SUCCESS != uResult )
    {
        Log( 1, _T("Test_Server_Create() - Create of shared resource failed.") );
        return uResult;
    }

    return uResult;
}

UINT CCrossProcShare_Conf::Test_Server_Open(DWORD dwRTYPE)
{
    // Log case info
    DumpCaseInfo(m_listTestCases[ m_rTestCases->Value() ].szTestName, m_pDefaultResCase, m_hShared, m_bReadOnlyCase );

    // Create resource.
    UINT uResult = TEST_SUCCESS;
    uResult = CreateResource_FromSharedMem();
    if( TEST_SUCCESS != uResult )
    {
        // Cant open if we cant create. note that the Test_Server_Open case will fail if this happens.
        Log( 1, _T("Test_Server_Open() - Create of shared resource failed.") );
        return TEST_SKIP;
    }
 
    return OpenResource_Test();
}

UINT CCrossProcShare_Conf::Test_Server_ClientOpen(DWORD dwRTYPE)
{
    // Log case info
    DumpCaseInfo(m_listTestCases[ m_rTestCases->Value() ].szTestName, m_pDefaultResCase, m_hShared, m_bReadOnlyCase );

    // Cant share on REF
    if( DEVICETYPE_REF == m_pSrcDevice->GetDevType() )
    {
        Log( 1, _T("Test_Server_ClientOpen() - Test case can not run on a REF device.") );
        return TEST_SKIP;
    }

    // Create resource.
    UINT uResult = TEST_SUCCESS;
    uResult = CreateResource_FromSharedMem();
    if( TEST_SUCCESS != uResult )
    {
        // Cant open if we cant create. note that the Test_Server_Open case will fail if this happens.
        Log( 1, _T("Test_Server_ClientOpen() - Create of shared resource failed.") );
        return TEST_SKIP;
    }

    // Open the resource.
    SendMessageToClient( 0, MSG_RES_OPEN );
    WaitForClientToBeDone( 0, 50000 );
 
    return TEST_SUCCESS;
}

UINT CCrossProcShare_Conf::Test_Server_Verify(DWORD dwRTYPE)
{
    UINT uResult = TEST_SUCCESS;

    // Create a resource and have the client open it
    uResult = Test_Server_ClientOpen(dwRTYPE);
    if( TEST_SUCCESS != uResult )
    {
        Log( 1, _T("Test_Server_Verify() - Create/Open of shared resource failed.") );
        return uResult;
    }

    // Did client fail open
    if( false == DidAllClientsSucceeded() || true == DidClientsSkip() )
    {
        // Cant verify if we cant open. note that the Test_Server_Open case will fail if this happens.
        Log(1, _T("Test_Server_Verify() - Client has failed.\n") );
        ResetClientFailStatus(); // dont let the client fail the case
        return TEST_SKIP;
    }

	// Verify fill works on non-shared.
	if( false == FillVerifyNonShared_FromSharedMem(0) )
	{
		Log(1, _T("Test_Server_Verify() - Unable to fill verify on non-shared resource.\n") );
        ResetClientFailStatus(); // dont let the client fail the case
        return TEST_SKIP;
	}

    // Client verify pattern 0
    SendMessageToClient( 0, MSG_RES_VERIFYZERO );
    WaitForClientToBeDone( 0, 50000 );

    return TEST_SUCCESS;
}

UINT CCrossProcShare_Conf::Test_Server_Fill(DWORD dwRTYPE)
{
    // Cant fill read-only cases
    if( true == m_bReadOnlyCase )
        return TEST_SKIP;

    UINT uResult = TEST_SUCCESS;

    // Create a resource, have the client open it, and verify patter 0
    uResult = Test_Server_Verify(dwRTYPE);
    if( TEST_SUCCESS != uResult )
    {
        Log( 1, _T("Test_Server_Fill() - Create/Open of shared resource failed.") );
        return uResult;
    }

    // Did client fail
    if( false == DidAllClientsSucceeded() || true == DidClientsSkip() )
    {
        // The Verify case will fail this... dont duplicate failures
        Log(1, _T("Test_Server_Fill() - Client has failed.\n") );
        ResetClientFailStatus(); // dont let the client fail the case
        return TEST_SKIP;
    }
 
	// Verify fill works on non-shared.
	if( false == FillVerifyNonShared_FromSharedMem(1) )
	{
		Log(1, _T("Test_Server_Fill() - Unable to fill verify on non-shared resource.\n") );
        ResetClientFailStatus(); // dont let the client fail the case
        return TEST_SKIP;
	}

    // Fill patterh 1
    uResult = FillResource_FromSharedMem(1);
    if( TEST_SUCCESS != uResult )
    {
        Log( 1, _T("Test_Server_Fill() - Fill of shared resource pattern 1 failed.") );
        return uResult;
    }
    
    // Verify pattern 1
    SendMessageToClient( 0, MSG_RES_VERIFYONE );
    WaitForClientToBeDone( 0, 50000 );    

    return TEST_SUCCESS;
}

UINT CCrossProcShare_Conf::Test_Server_Lock(DWORD dwRTYPE)
{
    if( true == m_bReadOnlyCase || false == m_pDefaultResCase->bLockable )
        return TEST_SKIP;

    UINT uResult = TEST_SUCCESS;

    // Create a resource and have the client open it
    uResult = Test_Server_ClientOpen(dwRTYPE);
    if( TEST_SUCCESS != uResult )
    {
        Log( 1, _T("Test_Server_Lock() - Create/Open of shared resource failed.") );
        return uResult;
    }

    // Did client fail open
    if( false == DidAllClientsSucceeded() || true == DidClientsSkip() )
    {
        // Cant Lock if we cant open. note that the Test_Server_Open case will fail if this happens.
        Log(1, _T("Test_Server_Lock() - Client has failed.\n") );
        ResetClientFailStatus(); // dont let the client fail the case
        return TEST_SKIP;
    }
 
    // Lock
    uResult = LockResource_FromSharedMem();
    if( TEST_SUCCESS != uResult )
    {
        Log( 1, _T("Test_Server_Lock() - Lock failed on shared resource from owner processes.") );
        return uResult;
    }

    // Client lock (should fail)
    SendMessageToClient( 0, MSG_RES_LOCK );
    WaitForClientToBeDone( 0, 50000 );
    if( true == DidAllClientsSucceeded() )
    {
        SendMessageToClient( 0, MSG_RES_UNLOCK );
        WaitForClientToBeDone( 0, 50000 );
        ResetClientFailStatus();
        UnLockResource_FromSharedMem();
        Log(1, _T("Test_Server_Lock() - Client has locked a locked shared resource.\n") );
        return TEST_FAIL;
    }
 
    // Unlock
    uResult = UnLockResource_FromSharedMem();
    if( TEST_SUCCESS != uResult )
    {
        Log( 1, _T("Test_Server_Lock() - UnLock failed on shared resource from owner processes when resource should be locked") );
        return uResult;
    }

    // Client lock (should fail)
    SendMessageToClient( 0, MSG_RES_LOCK );
    WaitForClientToBeDone( 0, 50000 );
    if( true == DidAllClientsSucceeded() )
    {
        SendMessageToClient( 0, MSG_RES_UNLOCK );
        WaitForClientToBeDone( 0, 50000 );
        ResetClientFailStatus();
        Log(1, _T("Test_Server_Lock() - Client has locked a shared resource.\n") );
        return TEST_FAIL;
    }

	SendMessageToClient( 0, MSG_RES_UNLOCK );
    WaitForClientToBeDone( 0, 50000 );
    ResetClientFailStatus();

    return TEST_SUCCESS;
}

// - Client open resource
// - Client verify resource is correct
// - Server release resource
// - Client verify and use resource
UINT CCrossProcShare_Conf::Test_Server_ReleaseSourceUseOpen(DWORD dwRTYPE)
{
    UINT uResult = TEST_SUCCESS;

    // Create a resource and have the client open it
    uResult = Test_Server_Verify(dwRTYPE);
    if( TEST_SUCCESS != uResult )
    {
        Log( 1, _T("Test_Server_ReleaseSourceUseOpen() - Create/Open of shared resource failed.") );
        return uResult;
    }

    // Did client fail open
    if( false == DidAllClientsSucceeded() || true == DidClientsSkip() )
    {
        // Cant Lock if we cant open. note that the Test_Server_Open case will fail if this happens.
        Log(1, _T("Test_Server_ReleaseSourceUseOpen() - Client has failed.\n") );
        ResetClientFailStatus(); // dont let the client fail the case
        return TEST_SKIP;
    }

	// Verify we can fill pattern 1
    uResult = FillResource_FromSharedMem(1);
    if( TEST_SUCCESS != uResult )
    {
        Log( 1, _T("Test_Server_ReleaseSourceUseOpen() - Fill of shared resource pattern 1 failed.") );
        return TEST_SKIP;
    }

	// fill with pattern 0 for the client to verify
    uResult = FillResource_FromSharedMem(0);
    if( TEST_SUCCESS != uResult )
    {
        Log( 1, _T("Test_Server_ReleaseSourceUseOpen() - Fill of shared resource pattern 0 failed.") );
        return TEST_SKIP;
    }

    // Relase
    if( FALSE == ResetBind() )
    {
        Log( 1, _T("Test_Server_OpenClosedResource() - ResetBind() failed.\n"));
        return TEST_SKIP;
    }
    RELEASE(m_pNonSharedResource);
	RELEASE(m_pResource);

    // Client should have a resource that works after our release
    SendMessageToClient( 0, MSG_RES_VERIFYZERO );
    WaitForClientToBeDone( 0, 50000 );
    if( false == DidAllClientsSucceeded() || true == DidClientsSkip() )
    {
        Log(1, _T("Test_Server_ReleaseSourceUseOpen() - Client has failed verify patter 0 after relase of server resource.\n") );
        return TEST_FAIL;
    }

    // Client uses resrouce
    SendMessageToClient( 0, MSG_RES_DRAW );
    WaitForClientToBeDone( 0, 50000 );
	ResetClientFailStatus(); 

    return TEST_SUCCESS;
}