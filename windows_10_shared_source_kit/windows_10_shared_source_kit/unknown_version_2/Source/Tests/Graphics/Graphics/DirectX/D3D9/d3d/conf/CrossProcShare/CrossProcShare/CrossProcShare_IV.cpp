// ============================================================================
//
//  CrossProcShare_Conf.cpp
//
// ============================================================================
#include "CrossProcShare_IV.h"
#include "CTextures.h"
#include <windows.h>

TESTCASE g_IVCases[] =
{
    { IV_DIFFERENTSIZE,     _T("DifferentSize") },
    { IV_DIFFERENTUSAGE,    _T("DifferentUsage") },
    { IV_DIFFERENTFORMAT,   _T("DifferentFormat") },
    { IV_DIFFERENTTYPE,     _T("DifferentType") },
    { IV_NULLRESOURCE,      _T("NullResource") },
    { IV_WRITEONREDONLY,    _T("WriteOnReadOnly") },
    { IV_CREATEDIFFERENTSIZE, _T("CreateDifferentSize") },
    { IV_CREATEDIFFERENTFORMAT, _T("CreateDifferentFormat") },
	{ IV_OPENCLOSEDRESOURCE, _T("OpenClosedResource") },
};
const UINT cIVCases = sizeof(g_IVCases) / sizeof(g_IVCases[0]);

CCrossProcShare_IV::CCrossProcShare_IV()
{ 
    m_szCommandKey  = _T("CrossProcShare_IV");
    m_szTestName    = _T("CrossProcShare_IV");
    m_szLogName     = _T("CrossProcShare");
    SetTestType(TESTTYPE_CONF);

    // Set CCrossProcShareTest options after SetTestType() set the default.
    SetTestOptions();

    m_nClients = 1;

    // Determine which tests to run from the command line params
    SetTestsToRun();
}

CCrossProcShare_IV::~CCrossProcShare_IV()
{
}

void CCrossProcShare_IV::CommandLineHelp(void)
{
	WriteCommandLineHelp("\n$gTest cases:");
	WriteCommandLineHelp("\n$y\t-IV:DifferentSize - Open a shared resource using different width and heigh.");
	WriteCommandLineHelp("\n$y\t-IV:DifferentUsage - Open a shared resource using different usages");
	WriteCommandLineHelp("\n$y\t-IV:DifferentFormat - Open a shared resource using different formats");
	WriteCommandLineHelp("\n$y\t-IV:DifferentType - Open a shared resource using a different resource type. For example, using a shared texture handle to create a vertex buffer");
	WriteCommandLineHelp("\n$y\t-IV:NullResource - Open a shared resource using a NULL resource and or a NULL shared handle");
	WriteCommandLineHelp("\n$y\t-IV:OpenClosedResource - Open a resource using a handle to a closed resource");
	WriteCommandLineHelp("\n$y\t-IV:WriteOnReadOnly - Write to a resource that was shared as read-only");
	WriteCommandLineHelp("\n$y\t-IV:CreateDifferentSize - Create a read-only shared resource using different width and heigh for the DEFAULT and SYSMEM resources");
	WriteCommandLineHelp("\n$y\t-IV:CreateDifferentFormat - Create a read-only shared resource using different format for the DEFAULT and SYSMEM resources");
}

void CCrossProcShare_IV::SetTestsToRun()
{
    TCHAR szOption[512] = _T("");
    DWORD dwIVTTests   = 0xffffffff;

    // Check if we override the IV tests
    if( ReadString(_T("IV"), szOption, ARRAYSIZE(szOption)) )
    {
        for( int iCase=0; iCase < cIVCases; iCase++ )
        {
            if( 0 == _tcsicmp(szOption, g_IVCases[iCase].szTestName))
            {
                dwIVTTests = g_IVCases[iCase].dwTestID;
            }
        }
    }

    for( int iCase=0; iCase < cIVCases; iCase++ )
    {
        if( dwIVTTests & g_IVCases[iCase].dwTestID )
        {
            m_listTestCases.push_back(g_IVCases[iCase]);
        }
    }
}

UINT CCrossProcShare_IV::ServerDispatchTest(DWORD dwTestID)
{
    UINT uResult = TEST_SUCCESS;

    if( KeySet(_T("stress")) )
    {
        return TEST_SKIP;
    }

    switch( dwTestID )
	{
		case IV_DIFFERENTSIZE:
			uResult = Test_Server_DifferentSize(m_dwRTYPE);
			break;

        case IV_DIFFERENTUSAGE:
			uResult = Test_Server_DifferentUsage(m_dwRTYPE);
			break;

        case IV_DIFFERENTFORMAT:
			uResult = Test_Server_DifferentFormat(m_dwRTYPE);
			break;
        
        case IV_DIFFERENTTYPE:
			uResult = Test_Server_DifferentType(m_dwRTYPE);
			break;

        case IV_NULLRESOURCE:
			uResult = Test_Server_NullResource(m_dwRTYPE);
			break;

        case IV_WRITEONREDONLY:
			uResult = Test_Server_WriteOnReadOnly(m_dwRTYPE);
			break;

        case IV_CREATEDIFFERENTSIZE:
			uResult = Test_Server_CreateDifferentSize(m_dwRTYPE);
			break;           

        case IV_CREATEDIFFERENTFORMAT:
			uResult = Test_Server_CreateDifferentFormat(m_dwRTYPE);
			break;

		case IV_OPENCLOSEDRESOURCE:
			uResult = Test_Server_OpenClosedResource(m_dwRTYPE);
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
UINT CCrossProcShare_IV::Test_Server_DifferentSize(DWORD dwRTYPE)
{   
    // Save shared case
    RESOURCE_CASE ResCaseSaved;
    memcpy( &ResCaseSaved, &m_pSharedMemory->resCase, sizeof(RESOURCE_CASE) );

    RESOURCE_CASE *pResCase = &m_pSharedMemory->resCase;

    // Create resource
    UINT uResult = DifferentCreate();
    if( TEST_SUCCESS != uResult )
        return uResult;
    
    // Width
    pResCase->uWidth = pResCase->uWidth == 64 ? 128 : 64;
    if( TEST_SUCCESS == ClientOpenResource_Test() )
    {
        Log(1, _T("Test_Server_DifferentSize() - Client was able to open the resource using a different width.\n"));
        memcpy( &m_pSharedMemory->resCase, &ResCaseSaved, sizeof(RESOURCE_CASE) );
        return TEST_FAIL;
    }

    // Height
    if( pResCase->dwRType == TYPE_OFFSCREENPLAIN || 
        pResCase->dwRType == TYPE_RENDERTARGET || 
        pResCase->dwRType == TYPE_DEPTHSTENCIL ||
        pResCase->dwRType == TYPE_TEXTURE ||
        pResCase->dwRType == TYPE_VOLUMETEXTURE )
    {
        pResCase->uHeight = pResCase->uHeight == 64 ? 128 : 64;
	    if( TEST_SUCCESS == ClientOpenResource_Test() )
        {
            Log(1, _T("Test_Server_DifferentSize() - Client was able to open the resource using a different height.\n"));
            memcpy( &m_pSharedMemory->resCase, &ResCaseSaved, sizeof(RESOURCE_CASE) );
            return TEST_FAIL;
        }
    }

    // Depth
    if( pResCase->dwRType == TYPE_VOLUMETEXTURE )
    {
	    pResCase->uDepth = pResCase->uDepth == 64 ? 128 : 64;	    
	    if( TEST_SUCCESS == ClientOpenResource_Test() )
        {
            Log(1, _T("Test_Server_DifferentSize() - Client was able to open the resource using a different depth.\n"));
            memcpy( &m_pSharedMemory->resCase, &ResCaseSaved, sizeof(RESOURCE_CASE) );
            return TEST_FAIL;
        }
    }
 
    memcpy( &m_pSharedMemory->resCase, &ResCaseSaved, sizeof(RESOURCE_CASE) );
    return TEST_SUCCESS;
}

UINT CCrossProcShare_IV::Test_Server_CreateDifferentSize(DWORD dwRTYPE)
{   
    // Create the system memory resource for the shared memory
    UINT uResult = DifferentReadOnlyCreate();
    if( TEST_SUCCESS != uResult )
        return uResult;

    // Save shared case
    RESOURCE_CASE ResCaseSaved;
    memcpy( &ResCaseSaved, &m_pSharedMemory->resCase, sizeof(RESOURCE_CASE) );

    // The resource case
    RESOURCE_CASE *pResCase = &m_pSharedMemory->resCase;
	pResCase->dwPool = POOL_DEFAULT;
    HANDLE hShared = NULL;

    // Test Width
    pResCase->uWidth = pResCase->uWidth == 64 ? 128 : 64;

    // Create read-only
    m_pResource = m_pNonSharedResource;
    HRESULT hr = m_EnumResource.CreateResource(pResCase->dwRType, pResCase, &m_pResource, &hShared);
    if( SUCCEEDED(hr) )
    {
        Log( 1, _T("Test_Server_CreateDifferentSize() - Create read-only resource using a different width passed.\n") );
        memcpy( &m_pSharedMemory->resCase, &ResCaseSaved, sizeof(RESOURCE_CASE) );
        RELEASE(m_pResource);
        return TEST_FAIL;
    }

    // Height
    if( pResCase->dwRType == TYPE_OFFSCREENPLAIN || 
        pResCase->dwRType == TYPE_RENDERTARGET || 
        pResCase->dwRType == TYPE_DEPTHSTENCIL ||
        pResCase->dwRType == TYPE_TEXTURE ||
        pResCase->dwRType == TYPE_VOLUMETEXTURE )
    {
        m_pResource = m_pNonSharedResource;
        hShared = NULL;
        pResCase->uHeight = pResCase->uHeight == 64 ? 128 : 64;
        hr = m_EnumResource.CreateResource(pResCase->dwRType, pResCase, &m_pResource, &hShared);
	    if( SUCCEEDED(hr) )
        {
            Log(1, _T("Test_Server_CreateDifferentSize() - Create read-only using a different height passed.\n"));
            memcpy( &m_pSharedMemory->resCase, &ResCaseSaved, sizeof(RESOURCE_CASE) );
            RELEASE(m_pResource);
            return TEST_FAIL;
        }
    }    

    // Depth
    if( pResCase->dwRType == TYPE_VOLUMETEXTURE )
    {
        m_pResource = m_pNonSharedResource;
        hShared = NULL;
	    pResCase->uDepth = pResCase->uDepth == 64 ? 128 : 64;	  
        hr = m_EnumResource.CreateResource(pResCase->dwRType, pResCase, &m_pResource, &hShared);
	    if( SUCCEEDED(hr) )
        {
            Log(1, _T("Test_Server_CreateDifferentSize() - Create read-only using a different depth passed.\n"));
            memcpy( &m_pSharedMemory->resCase, &ResCaseSaved, sizeof(RESOURCE_CASE) );
            RELEASE(m_pResource);
            return TEST_FAIL;
        }
    }

    // Levels
    if( pResCase->dwRType == TYPE_TEXTURE ||
        pResCase->dwRType == TYPE_VOLUMETEXTURE ||
        pResCase->dwRType == TYPE_CUBETEXTURE)
    {
        m_pResource = m_pNonSharedResource;
        hShared = NULL;
        UINT uLevels = pResCase->uLevels;
   
        if( uLevels > 1 )
        {
            // less levels should pass
            pResCase->uLevels = pResCase->uLevels - 1;
            hr = m_EnumResource.CreateResource(pResCase->dwRType, pResCase, &m_pResource, &hShared);
            if( FAILED(hr) )
            {
                Log(1, _T("Test_Server_CreateDifferentSize() - Create read-only using a smaller level failed.\n"));
                memcpy( &m_pSharedMemory->resCase, &ResCaseSaved, sizeof(RESOURCE_CASE) );
                RELEASE(m_pResource);
                return TEST_FAIL;
            }
        }        

        // more levels should fail
        if( uLevels < UINT_MAX - 1)
        {
            pResCase->uLevels = uLevels + 1;
            hr = m_EnumResource.CreateResource(pResCase->dwRType, pResCase, &m_pResource, &hShared);
            if( SUCCEEDED(hr) )
            {
                Log(1, _T("Test_Server_CreateDifferentSize() - Create read-only using a different level passed.\n"));
                memcpy( &m_pSharedMemory->resCase, &ResCaseSaved, sizeof(RESOURCE_CASE) );
                RELEASE(m_pResource);
                return TEST_FAIL;
            }
        }       
    }
 
    m_pResource = NULL;
    memcpy( &m_pSharedMemory->resCase, &ResCaseSaved, sizeof(RESOURCE_CASE) );
    return TEST_SUCCESS;
}

UINT CCrossProcShare_IV::Test_Server_DifferentUsage(DWORD dwRTYPE)
{
    // Save shared case
    RESOURCE_CASE ResCaseSaved;
    memcpy( &ResCaseSaved, &m_pSharedMemory->resCase, sizeof(RESOURCE_CASE) );

    RESOURCE_CASE *pResCase = &m_pSharedMemory->resCase;

    if (pResCase->dwRType != TYPE_TEXTURE &&
        pResCase->dwRType != TYPE_CUBETEXTURE &&
        pResCase->dwRType != TYPE_VOLUMETEXTURE )
    {
        return TEST_SKIP;
    }

	// Create resource
    UINT uResult = DifferentCreate();
    if( TEST_SUCCESS != uResult )
        return uResult;

    // Only try if there is another usage supported
    pResCase->dwUsage = m_EnumResource.GetAnotherValidUsageForFormat(pResCase->dwRType, pResCase->format, pResCase->dwUsage, false);
    if( pResCase->dwUsage == ResCaseSaved.dwUsage )
    {
        return TEST_SKIP;
    }

    // Test
    if( TEST_SUCCESS == ClientOpenResource_Test() )
    {
        Log( 1, _T("Test_Server_DifferentUsage() - Client was able to open the resource using a different usage: %s\n"), m_EnumResource.GetUsageString(pResCase->dwUsage) );
        uResult = TEST_FAIL;
    }

    memcpy( &m_pSharedMemory->resCase, &ResCaseSaved, sizeof(RESOURCE_CASE) );
    return uResult;
}

UINT CCrossProcShare_IV::Test_Server_DifferentFormat(DWORD dwRTYPE)
{ 
    // Save shared case
    RESOURCE_CASE ResCaseSaved;
    memcpy( &ResCaseSaved, &m_pSharedMemory->resCase, sizeof(RESOURCE_CASE) );

    RESOURCE_CASE *pResCase = &m_pSharedMemory->resCase;

	// Create resource
    UINT uResult = DifferentCreate();
    if( TEST_SUCCESS != uResult )
        return uResult;

    // Only try if there is another format supported
    pResCase->format = m_EnumResource.GetAnotherValidFormatForUsage(pResCase->dwRType, pResCase->dwUsage, pResCase->format);
    if( pResCase->format == ResCaseSaved.format )
    {
        return TEST_SKIP;
    }

    // Test
    if( TEST_SUCCESS == ClientOpenResource_Test() )
    {
        Log( 1, _T("Test_Server_DifferentUsage() - Client was able to open the resource using a different format: %s\n"), m_EnumResource.GetFormatString(pResCase->format) );
        uResult = TEST_FAIL;
    }

    memcpy( &m_pSharedMemory->resCase, &ResCaseSaved, sizeof(RESOURCE_CASE) );
    return uResult;
}

UINT CCrossProcShare_IV::Test_Server_CreateDifferentFormat(DWORD dwRTYPE)
{   
    if( false == m_bReadOnlyCase )
        return TEST_SKIP;

    // Save shared case
    RESOURCE_CASE ResCaseSaved;
    memcpy( &ResCaseSaved, &m_pSharedMemory->resCase, sizeof(RESOURCE_CASE) );

    // The resource case
    RESOURCE_CASE *pResCase = &m_pSharedMemory->resCase;
    HANDLE hShared = NULL;

	// Create the system memory resource for the shared memory
    UINT uResult = DifferentReadOnlyCreate();
    if( TEST_SUCCESS != uResult )
        return uResult;

    // Only try if there is another format supported
    pResCase->format = m_EnumResource.GetAnotherValidFormatForUsage(pResCase->dwRType, pResCase->dwUsage, pResCase->format);
    if( pResCase->format == ResCaseSaved.format )
    {
        return TEST_SKIP;
    }

    // Create
    pResCase->dwPool = POOL_DEFAULT;
    m_pResource = m_pNonSharedResource;
    HRESULT hr = m_EnumResource.CreateResource(pResCase->dwRType, pResCase, &m_pResource, &hShared);
    if( SUCCEEDED(hr) )
    {
        Log( 1, _T("Test_Server_CreateDifferentFormat() - Create read-only resource using a different format (%s) passed.\n"), m_EnumResource.GetFormatString(pResCase->format) );
        memcpy( &m_pSharedMemory->resCase, &ResCaseSaved, sizeof(RESOURCE_CASE) );
        RELEASE(m_pResource);
        return TEST_FAIL;
    }
    m_pResource = NULL;
    hShared = NULL;

    memcpy( &m_pSharedMemory->resCase, &ResCaseSaved, sizeof(RESOURCE_CASE) );
    return TEST_SUCCESS;
}

UINT CCrossProcShare_IV::Test_Server_DifferentType(DWORD dwRTYPE)
{ 
    RESOURCE_CASE *pResCase = &m_pSharedMemory->resCase;

    if( DEVICETYPE_REF == m_pSrcDevice->GetDevType() )
    {
        Log( 1, _T("Test_Server_DifferentType() - Test case can not run on a REF device.") );
        return TEST_SKIP;
    }

    // Create resource
    UINT uResult = DifferentCreate();
    if( TEST_SUCCESS != uResult )
        return uResult;

    // Resource names
    TCHAR szDiffRType[255];
    TCHAR szRType[255];
    m_EnumResource.GetTypeName(pResCase->dwRType, szRType);

    // Try to open with different resource type.
    for( int iType = 0; iType < TYPE_MAXTYPE; iType++ )
    {
        // If not different resource type skip.
        if( pResCase->dwRType == iType )
            continue;

        // Open the shared resource using a different resource with correct shared handle.	
		CObject* pOpen = NULL;
		HRESULT hr = m_EnumResource.CreateResource(iType, m_pDefaultResCase, &pOpen, &m_pSharedMemory->hShared);
		if( SUCCEEDED(hr) )
		{
			RELEASE(pOpen);
			m_EnumResource.GetTypeName(iType, szDiffRType);
            Log( 1, _T("Test_Server_DifferentUsage() - Client was able to open Opened a shared %s using a %s.\n"), szRType, szDiffRType);
            return TEST_FAIL;
		}
    }

    return TEST_SUCCESS;
}

UINT CCrossProcShare_IV::Test_Server_NullResource(DWORD dwRTYPE)
{
    if( false == m_pD3D->GetDebug() )
        return TEST_SKIP;
    // Log case info
    DumpCaseInfo(m_listTestCases[ m_rTestCases->Value() ].szTestName, m_pDefaultResCase, m_hShared, m_bReadOnlyCase );

    // Make sure we can creat the resource correctly
    if( TEST_SUCCESS != CreateResource_FromSharedMem() )
    {
        Log( 1, _T("Test_Server_NullResource() - Create of shared resource failed."));
        return TEST_SKIP;
    }

    // Test - pSharedHandle == NULL, ppTexture == NULL
    HRESULT hr = m_EnumResource.CreateResource(m_pDefaultResCase->dwRType, m_pDefaultResCase, NULL, NULL);
    if( SUCCEEDED(hr) )
    {
        Log( 1, _T("Test_Server_NullResource() - CreateResource with pSharedHandle == NULL, ppTexture == NULL passed.") );
        return TEST_FAIL;
    }

    // Test - pSharedHandle == NULL, ppTexture == NULL
    HANDLE hIV = NULL;
    hr = m_EnumResource.CreateResource(m_pDefaultResCase->dwRType, m_pDefaultResCase, NULL, &hIV);
    if( SUCCEEDED(hr) )
    {
        Log( 1, _T("Test_Server_NullResource() - CreateResource with *pSharedHandle == NULL, ppTexture == NULL passed.") );
        return TEST_FAIL;
    }
    
    return TEST_SUCCESS;
}

UINT CCrossProcShare_IV::Test_Server_OpenClosedResource(DWORD dwRTYPE)
{
    if( DEVICETYPE_REF == m_pSrcDevice->GetDevType() )
    {
        Log( 1, _T("Test_Server_DifferentType() - Test case can not run on a REF device.") );
        return TEST_SKIP;
    }

    // Log case info
    DumpCaseInfo(m_listTestCases[ m_rTestCases->Value() ].szTestName, m_pDefaultResCase, m_hShared, m_bReadOnlyCase );

    // Make sure we can create the resource correctly
    if( TEST_SUCCESS != CreateResource_FromSharedMem() )
    {
        Log( 1, _T("Test_Server_OpenClosedResource() - Create of shared resource failed.\n"));
        return TEST_SKIP;
    }

	// Close resource
    if( FALSE == ResetBind() )
    {
        Log( 1, _T("Test_Server_OpenClosedResource() - ResetBind() failed.\n"));
        return TEST_SKIP;
    }
	RELEASE(m_pResource);

	// Open resource with closed handle
    HRESULT hr = m_EnumResource.CreateResource(m_pDefaultResCase->dwRType, m_pDefaultResCase, &m_pResource, &m_pSharedMemory->hShared);
    if( SUCCEEDED(hr) )
    {
        Log( 1, _T("Test_Server_OpenClosedResource() - Open of resource with closed handle succeeded %s\n"), m_pD3D->HResultToString(hr) );
    }

    return TEST_SUCCESS;
}

UINT CCrossProcShare_IV::Test_Server_WriteOnReadOnly(DWORD dwRTYPE)
{
    RESOURCE_CASE *pResCase = &m_pSharedMemory->resCase;

    if( false == m_bReadOnlyCase || pResCase->dwUsage & USAGE_DYNAMIC )
        return TEST_SKIP;

    // Log case info
    DumpCaseInfo(m_listTestCases[ m_rTestCases->Value() ].szTestName, m_pDefaultResCase, m_hShared, m_bReadOnlyCase );

    // 1974715 - CDriverVertex/IndexBuffer::Lock() doesn’t check IsReadOnly() and allows lock without D3DLOCK_READONLY.
    if( TYPE_VERTEXBUFFER == pResCase->dwRType ||
        TYPE_INDEXBUFFER == pResCase->dwRType )
    {
        return TEST_SKIP;
    }

    // Create Read-Only resource
    if( TEST_SUCCESS != CreateResource_FromSharedMem() )
    {
        Log( 1, _T("Test_Server_WriteOnReadOnly() - Create of shared resource failed.") );
        return TEST_SKIP;
    }

    // Lock
	if( true == LockResource(m_pSharedMemory->resCase.dwRType, m_pResource) )
	{
		Log(1, _T("Test_Server_WriteOnReadOnly() - Locked a read-only resource without error.\n") );
		return TEST_FAIL;
	}

    // Client open resource
    SendMessageToClient( 0, MSG_RES_OPEN );
    WaitForClientToBeDone( 0, 50000 );

    // Client cant open. Move on.
    if( false == DidAllClientsSucceeded() || true == DidClientsSkip() )
    {
        ResetClientFailStatus();
        return TEST_SUCCESS;
    }

    // Client fill
    SendMessageToClient(0, MSG_RES_FILLONE);
    WaitForClientToBeDone(0, 50000);

    // Client fill should have failed
    if( false == DidAllClientsSucceeded() || true == DidClientsSkip() )
    {
        ResetClientFailStatus();
        return TEST_SUCCESS;
    }

    return TEST_FAIL;
}

UINT CCrossProcShare_IV::DifferentCreate()
{   
    // Log case info
    DumpCaseInfo(m_listTestCases[ m_rTestCases->Value() ].szTestName, m_pDefaultResCase, m_hShared, m_bReadOnlyCase );

    // Create
    if( TEST_SUCCESS != CreateResource_FromSharedMem() )
        return TEST_SKIP;

    // Make sure client can open
    if( TEST_SUCCESS != OpenResource_Test() )
        return TEST_SKIP;

    return TEST_SUCCESS;
}

UINT CCrossProcShare_IV::DifferentReadOnlyCreate()
{   
    if( false == m_bReadOnlyCase )
        return TEST_SKIP;

    // Log case info
    DumpCaseInfo(m_listTestCases[ m_rTestCases->Value() ].szTestName, m_pDefaultResCase, m_hShared, m_bReadOnlyCase );

    // Get case
    RESOURCE_CASE *pResCase = &m_pSharedMemory->resCase;

    // RT and DS can not be read-only
    if( TYPE_RENDERTARGET == pResCase->dwRType  || 
        TYPE_DEPTHSTENCIL == pResCase->dwRType )
        return TEST_SKIP;

    // Create a non shared resource
    pResCase->dwPool = POOL_SYSTEMMEM;
    HRESULT hr = m_EnumResource.CreateResource(pResCase->dwRType, pResCase, &m_pNonSharedResource,  NULL);
    if( FAILED(hr) )
    {
        Log( 1, _T("DifferentReadOnlyCreate() - Create SysMem resource for read-only case failed with %s\n"), m_pD3D->HResultToString(hr) );
        m_pNonSharedResource = NULL;
        return TEST_SKIP;
    }

    // Try to create a read-only resource
    /*
    m_pResource = m_pNonSharedResource;
    pResCase->dwPool = POOL_DEFAULT;
    hr = m_EnumResource.CreateResource(pResCase->dwRType, pResCase, &m_pResource, &m_pSharedMemory->hShared);
    if( FAILED(hr) )
    {
        Log( 1, _T("DifferentReadOnlyCreate() - Create default Read-Only resource failed with %s\n"), m_pD3D->HResultToString(hr) );
        RELEASE(m_pNonSharedResource);
        m_pResource = NULL;
        return TEST_SKIP;
    }

    RELEASE(m_pResource);*/

    return TEST_SUCCESS;
}

UINT CCrossProcShare_IV::ClientOpenResource_Test()
{
    UINT uResult = TEST_SUCCESS;

    // Client open resource
    SendMessageToClient( 0, MSG_RES_OPEN );
    WaitForClientToBeDone( 0, 50000 );

    // Client should not fail or skip
    if( false == DidAllClientsSucceeded() || true == DidClientsSkip() )
        uResult = TEST_FAIL;

    // Client release
    SendMessageToClient(0, MSG_RES_RELEASE);
    WaitForClientToBeDone(0, 50000);

    // Clear client status
    ResetClientLog();
    ResetClientFailStatus();

    return uResult;
}