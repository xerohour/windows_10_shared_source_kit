// ============================================================================
//
//  Communication.cpp
//
// ============================================================================
#include "Communication.h"
#include <process.h>

CCommunication::CCommunication()
{ 
  	m_szCommandKey=	_T("CrossProcShare");
	m_szLogName = _T("CrossProcShare");
	m_szTestName=	_T("CrossProcShare");

	m_iClientId = 0xffffffff;
	m_bServer = !(ReadInteger(_T("Client"), &m_iClientId));

	// Initialize all the Inter Process communication for server or client
	InitializeIPC();
}


// ----------------------------------------------------------------------------

CCommunication::~CCommunication()
{
	TestTerminate();
}

bool CCommunication::TestTerminate(void)
{
	SendMessageToClients(MSG_EXIT);

    ClenupClientData();

	return true;
}

void CCommunication::ClenupClientData(void)
{
    //Close the process handles
    std::vector <PROCESSDATA*>::iterator caseIter;

    for (caseIter = m_listClientProcess.begin(); caseIter != m_listClientProcess.end(); caseIter++)
    {
        PROCESSDATA *pProcessData = (PROCESSDATA*)*caseIter;
        HANDLE hProcess = pProcessData->hProcess;
        CloseHandle(pProcessData->hProcess);
        CloseHandle(pProcessData->hMailbox);
        CloseHandle(pProcessData->hMailboxReceipt);
        CloseHandle(pProcessData->hFileMap);
        delete pProcessData;
    }
    m_listClientProcess.clear();
}

bool CCommunication::InitializeIPC()
{
	char szTemp[MAX_PATH];
	if( true == m_bServer )
	{
		//Create the file mapping to get shared memory
		m_hFileMap = ::CreateFileMapping(	INVALID_HANDLE_VALUE , 
											NULL, 
											PAGE_READWRITE, 
											0,
											sizeof(SHAREDMEMORY),
											_T("CrossProcShareTestSharedMem") );
        
        if( NULL == m_hFileMap )
            return false;
	}
	else //Client
	{
		//Open the file mapping to access the shared memory
	    m_hFileMap = ::OpenFileMapping(	FILE_MAP_WRITE, 
										FALSE, 
										_T("CrossProcShareTestSharedMem") );

        if( NULL == m_hFileMap )
            return false;

		//Open the CLIENT file mapping to access the shared memory
		_snprintf(szTemp, MAX_PATH-1, _T("CrossProcShareTestClientSharedMem%d"), m_iClientId);
		szTemp[MAX_PATH-1] = '\0';
	    m_hClientFileMap = ::OpenFileMapping(	FILE_MAP_WRITE, 
												FALSE, 
												szTemp);

		// Obtain a pointer of CLIENTSHAREDMEMORY from the handle to file mapping object
		m_pClientSharedMemory = (CLIENTSHAREDMEMORY*)::MapViewOfFile(	m_hClientFileMap,
																		FILE_MAP_WRITE,
																		0,
																		0,
																		sizeof(CLIENTSHAREDMEMORY));

		//Open a mailbox (event) for this client to receive message from server
		_snprintf(szTemp, MAX_PATH-1, _T("CrossProcShareTestMailbox%d"), m_iClientId);
		szTemp[MAX_PATH-1] = '\0';
		m_hClientMailbox = ::CreateEvent(NULL, FALSE, FALSE, szTemp);

		//Open a mailbox receipt (event) for this client to tell the server we finished this request
		_snprintf(szTemp, MAX_PATH-1, _T("CrossProcShareTestMailboxReceipt%d"), m_iClientId);
		szTemp[MAX_PATH-1] = '\0';
		m_hClientMailboxReceipt = ::CreateEvent(NULL, FALSE, FALSE, szTemp);
	}

	// Obtain a pointer of SHAREDMEMORY from the handle to file mapping object
	m_pSharedMemory = (SHAREDMEMORY*)::MapViewOfFile(	m_hFileMap,
														FILE_MAP_WRITE,
														0,
														0,
														sizeof(SHAREDMEMORY));

	return true;
}

// ----------------------------------------------------------------------------

bool CCommunication::CreateClientProcess(UINT uAdapterForNewProcess, bool bCreateProtected)
{
    STARTUPINFO si;
    PROCESS_INFORMATION ProcInf;
	UINT nClients = m_listClientProcess.size();
    ZeroMemory( &ProcInf, sizeof(ProcInf) );
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
	char szExe[MAX_PATH];
	char szExe2[MAX_PATH];
	memset(szExe2, 0, sizeof(char) * MAX_PATH);
	if (!GetModuleFileName( NULL, szExe2, MAX_PATH-1 ))
	{
		WriteToLog(0, _T("CreateClientProcess() - couldn't GetModuleFileName"));
		return false;
	}
    if( true == KeySet("WINDBG") )
	    _snprintf(szExe, MAX_PATH-1, _T("c:\\debuggers\\windbg.exe -g %s -Client:%d -M:%d -NoLog"), szExe2, nClients, uAdapterForNewProcess);
    else
        _snprintf(szExe, MAX_PATH-1, _T("%s -Client:%d -M:%d -NoLog"), szExe2, nClients, uAdapterForNewProcess);

	szExe[MAX_PATH-1] = '\0';

	PROCESSDATA *pProcessData = new PROCESSDATA;

	// Create a mailbox (event) so that the server can send message to this client
	char szTemp[MAX_PATH];
	_snprintf(szTemp, MAX_PATH-1, _T("CrossProcShareTestMailbox%d"), nClients);
	szTemp[MAX_PATH-1] = '\0';
	pProcessData->hMailbox = ::CreateEvent(NULL, FALSE, FALSE, szTemp);

	// Create a mailbox (event) so that the server knows when this client is done with message
	_snprintf(szTemp, MAX_PATH-1, _T("CrossProcShareTestMailboxReceipt%d"), nClients);
	szTemp[MAX_PATH-1] = '\0';
	pProcessData->hMailboxReceipt = ::CreateEvent(NULL, FALSE, FALSE, szTemp);

	// Create the file mapping to get the shared memory from the client
	_snprintf(szTemp, MAX_PATH-1, _T("CrossProcShareTestClientSharedMem%d"), nClients);
	szTemp[MAX_PATH-1] = '\0';
	pProcessData->hFileMap = ::CreateFileMapping(	INVALID_HANDLE_VALUE , 
													NULL, 
													PAGE_READWRITE, 
													0,
													sizeof(CLIENTSHAREDMEMORY),
													szTemp);

	// Obtain a pointer of CLIENTSHAREDMEMORY from the handle to file mapping object
	pProcessData->pSharedMemory = (CLIENTSHAREDMEMORY*)::MapViewOfFile(	pProcessData->hFileMap,
																	FILE_MAP_WRITE,
																	0,
																	0,
																	sizeof(CLIENTSHAREDMEMORY));
	// Start the child process
    DWORD dwCreateFlag = true == bCreateProtected ? CREATE_PROTECTED_PROCESS: 0;
    if( false == CreateProcess(	NULL, szExe, NULL, NULL, FALSE, dwCreateFlag, NULL, NULL, &si, &ProcInf ) ) 
	{
        CloseHandle(ProcInf.hProcess);
        CloseHandle(ProcInf.hThread);
		WriteToLog(0, _T("CreateClientProcess() - couldn't CreateProcess.\n") );
		return false;
	}

	if( WAIT_OBJECT_0 != ::WaitForSingleObject(pProcessData->hMailboxReceipt, 50000) ) 
	{
		//Timeout occurs
        CloseHandle(ProcInf.hProcess);
        CloseHandle(ProcInf.hThread);
		WriteToLog(0, _T("CreateClientProcess() - Timeout waiting on client.") );
        return false;
	}

	pProcessData->hProcess = ProcInf.hProcess;
	
	m_listClientProcess.push_back(pProcessData);

	return true;
}

bool CCommunication::SendMessageToClient(UINT uClient, DWORD dwMessage)
{
   if( uClient > m_listClientProcess.size() )
        return false;

	m_pSharedMemory->dwMessage = dwMessage;

	::SetEvent(m_listClientProcess[ uClient ]->hMailbox);

	return true;
}

bool CCommunication::SendMessageToClients(DWORD dwMessage)
{
    if( 0 == m_listClientProcess.size() )
        return false;

	m_pSharedMemory->dwMessage = dwMessage;

	for (int iClient=0; iClient<m_listClientProcess.size(); iClient++)
	{
		::SetEvent(m_listClientProcess[ iClient ]->hMailbox);
	}

	return true;
}

bool CCommunication::WaitForClientToBeDone(UINT uClient, DWORD dwTimeout)
{
    DWORD dwRT = 0;

    dwRT = ::WaitForSingleObject(m_listClientProcess[ uClient ]->hMailboxReceipt, dwTimeout);

    if( WAIT_TIMEOUT == dwRT || 
        WAIT_ABANDONED == dwRT ||
        WAIT_FAILED == dwRT )
		return false;

	return true;
}

bool CCommunication::WaitForAllClientsToBeDone(DWORD dwTimeout)
{
	UINT nClients = m_listClientProcess.size();
    if( 0 == nClients )
        return false;

    HANDLE *pHandles = new HANDLE[nClients];
    DWORD dwRT = 0;


	for (int iClient=0; iClient<nClients; iClient++)
	{
		pHandles[iClient] = m_listClientProcess[ iClient ]->hMailboxReceipt;
	}

    dwRT = ::WaitForMultipleObjects(nClients, pHandles, TRUE, dwTimeout);
    
    if( dwRT != WAIT_TIMEOUT &&  dwRT != WAIT_OBJECT_0 ) // error
    {
        ClenupClientData();
        delete[] pHandles;
		return false;
    }

	delete[] pHandles;
    return dwRT == WAIT_OBJECT_0 ? true: false;
}

bool CCommunication::WaitForOneMessageFromServer()
{
	//Tell the server we are ready to receive messages
	::SetEvent(m_hClientMailboxReceipt);

	//wait for one message
	if (::WaitForSingleObject(m_hClientMailbox, 120000) == WAIT_OBJECT_0) 
	{
        // Time out
		return true;
	}

	//Timeout occured
	Log( 1, _T("Cient %d : WaitForMessagesFromServer() : Timeout occured"), m_iClientId );
	return false;
}

bool CCommunication::WaitForMessagesFromServer()
{
	//Tell the server we are ready to receive messages
	::SetEvent(m_hClientMailboxReceipt);

	//Loop on waiting for messages
	while( WAIT_OBJECT_0 == ::WaitForSingleObject(m_hClientMailbox, INFINITE) )
	{
		if( MSG_EXIT == m_pSharedMemory->dwMessage ) 
		{
			return true;
		}
        else if( MSG_PING == m_pSharedMemory->dwMessage )
        {
            ::SetEvent(m_hClientMailboxReceipt);
        }
		else
		{
			// Reset log string
			ResetClientLog();

			// Process message
			m_pClientSharedMemory->uTestResult = OnMessageFromServer(m_pSharedMemory->dwMessage);

			// Tell the server we are done with this message
			::SetEvent(m_hClientMailboxReceipt);
		}
	};
    
    // server has quit without us
	return false;
}

void CCommunication::ResetClientLog()
{
    for( int iClient = 0; iClient < m_listClientProcess.size(); iClient++ )
    {
        _tcscpy(m_listClientProcess[ iClient ]->pSharedMemory->szFailure, "");
    }
}

void CCommunication::ResetClientFailStatus()
{
    for( int iClient = 0; iClient < m_listClientProcess.size(); iClient++ )
    {
        m_listClientProcess[ iClient ]->pSharedMemory->uTestResult = TEST_SUCCESS;
    }
}

void CCommunication::Log(UINT uLogLevel, const TCHAR *ptcsFmt, ...)
{
	TCHAR *sTemp = NULL;
	va_list	argList;

	va_start(argList, ptcsFmt);
	sTemp = GetVarArgString(ptcsFmt, argList);
	va_end(argList);

	if( true == m_bServer )
	{
		DPF(uLogLevel, sTemp);
	}
	else
	{
		_stprintf(m_pClientSharedMemory->szFailure, _T("%s %s"), m_pClientSharedMemory->szFailure, sTemp);
	}

	SAFEDELETEA(sTemp);
}

bool CCommunication::DidAllClientsSucceeded()
{
    // any clients open?
	if( 0 == m_listClientProcess.size() )
		return true;

	for( int iClient=0; iClient<m_listClientProcess.size(); iClient++ )
	{
		if( m_listClientProcess[ iClient ]->pSharedMemory->uTestResult == TEST_FAIL )
		{
			return false;
		}
	}

	return true;
}

bool CCommunication::DidClientsSkip()
{
    // any clients open?
	if( 0 == m_listClientProcess.size() )
		return true;

	for( int iClient=0; iClient<m_listClientProcess.size(); iClient++ )
	{
		if( m_listClientProcess[ iClient ]->pSharedMemory->uTestResult == TEST_SKIP )
		{
			return true;
		}
	}

	return false;
}

// Logs messages the client reported with Log().
bool CCommunication::LogClientsMessages()
{
    // any clients open?
    if( 0 == m_listClientProcess.size() )
		return true;
    
    // for all open clients get messaged they Log()'ed.
    for( int iClient = 0; iClient < m_listClientProcess.size(); iClient++ )
	{
        Log(1, _T("\n-- Message From Client %d:\n"), iClient);
        Log(1, m_listClientProcess[ iClient ]->pSharedMemory->szFailure); 
        Log(1, _T("\n-- End of message from client %d.\n"), iClient);
	}

	return true;
}
