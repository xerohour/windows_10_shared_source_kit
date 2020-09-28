#include "CrossProcShareTest.h"

// ----------------------------------------------------------------------------

void CCrossProcShareTest::Client_ReleaseResource()
{
    RELEASE(m_pNonSharedResource);
	RELEASE(m_pResource);
    m_dwClientOpenedRTYPE = TYPE_NONE;
}

UINT CCrossProcShareTest::OnMessageFromServer(DWORD dwMessage)
{
	UINT uResult = TEST_SUCCESS;

    if( MSG_RES_OPEN != m_pSharedMemory->dwMessage &&
            MSG_RES_CREATE != m_pSharedMemory->dwMessage &&
            MSG_RES_CREATENONPRO != m_pSharedMemory->dwMessage &&
            MSG_RES_RELEASE != m_pSharedMemory->dwMessage &&
            MSG_ACT_LOGCASE != m_pSharedMemory->dwMessage &&
            (m_dwClientOpenedRTYPE != m_pSharedMemory->resCase.dwRType ||
            TYPE_NONE == m_dwClientOpenedRTYPE) )
        {
            Log( 1, _T("OnMessageFromServer(): Error: Performing action on wrong resource type.\n") );
            OutputDebugString( _T("OnMessageFromServer(): Error: Performing action on wrong resource type.\n") );
            return TEST_SKIP;
        }

	try 
	{
        switch( m_pSharedMemory->dwMessage )
        {
	        case MSG_RES_CREATE :
		        uResult = Client_CreateResource(true);
		        break;
            case MSG_RES_CREATENONPRO :
		        uResult = Client_CreateResource(false);
		        break;
            case  MSG_RES_OPEN :
		        uResult = Client_OpenResource();
		        break;
            case  MSG_RES_FILLZERO :
		        uResult = Client_FillResource(0);
		        break;
            case  MSG_RES_FILLONE :
		        uResult = Client_FillResource(1);
		        break;
            case  MSG_RES_VERIFYZERO :
		        uResult = Client_VerifyResource(0);
		        break;
            case  MSG_RES_VERIFYONE :
		        uResult = Client_VerifyResource(1);
		        break;
            case  MSG_RES_LOCK :
		        uResult = Client_LockResource();
		        break;
            case  MSG_RES_UNLOCK :
		        uResult = Client_UnLockResource();
		        break;
            case  MSG_RES_USE :
		        uResult = Client_UseResource();
		        break;
            case MSG_RES_RELEASE :
                Client_ReleaseResource();
                uResult = TEST_SUCCESS;
                break;
            case MSG_RES_DRAW :
                Client_DrawResource();
                uResult = TEST_SUCCESS;
                break;
            case MSG_ACT_LOGCASE :
                m_pSharedMemory->hShared = NULL;
                DumpCaseInfo(NULL, &m_pSharedMemory->resCase, m_pSharedMemory->hShared, m_pSharedMemory->bReadOnlyResource );
                uResult = TEST_SUCCESS;
                break;
            default:
                Log( 1, _T("Unknown message from server.\n") );
                uResult = TEST_FAIL;
                break;
        }
	}
	catch (...)
	{
		Log( 1, _T("Exception Caught in OnMessageFromServer()\n") );
		uResult = TEST_FAIL;
	}

	return uResult;
}

UINT CCrossProcShareTest::Client_FillResource(UINT uPattern)
{
	return FillResource_FromSharedMem(uPattern);
}

UINT CCrossProcShareTest::Client_VerifyResource(UINT uPattern)
{
	return VerifyResource_FromSharedMem(uPattern);
}

UINT CCrossProcShareTest::Client_CreateResource(bool bSecure)
{
    UINT uRT = 0;
    if( TYPE_NONE == m_dwClientOpenedRTYPE )
    {
        m_dwClientOpenedRTYPE = m_pSharedMemory->resCase.dwRType;
        uRT = CreateResource_FromSharedMem(bSecure);
        if( TEST_SUCCESS != uRT )
            Client_ReleaseResource();
    }
    else
    {
        Log( 1, _T("Client_CreateResource(): Error: Resouce not released before call to create\n") );
        OutputDebugString( _T("Client_CreateResource(): Error: Resouce not released before call to open\n") );
        uRT = TEST_SKIP;
    }

    return uRT;
}

UINT CCrossProcShareTest::Client_OpenResource()
{
    UINT uRT = 0;
    if( TYPE_NONE == m_dwClientOpenedRTYPE )
    {
        m_dwClientOpenedRTYPE = m_pSharedMemory->resCase.dwRType;
        uRT = OpenResource_FromSharedMem();
        if( TEST_SUCCESS != uRT )
            Client_ReleaseResource();
    }
    else
    {
        Log( 1, _T("Client_OpenResource(): Error: Resouce not released before call to open\n") );
        OutputDebugString( _T("Client_OpenResource(): Error: Resouce not released before call to open\n") );
        uRT = TEST_SKIP;
    }

    return uRT;
}

UINT CCrossProcShareTest::Client_LockResource()
{
    return LockResource_FromSharedMem();
}

UINT CCrossProcShareTest::Client_UnLockResource()
{
    return UnLockResource_FromSharedMem();
}
UINT CCrossProcShareTest::Client_UseResource()
{
    return UseResource_FromSharedMem();
}

UINT CCrossProcShareTest::Client_DrawResource()
{
    if( false == DrawResource(&m_pSharedMemory->resCase, m_pResource) )
        return TEST_FAIL;

    return TEST_SUCCESS;
}
