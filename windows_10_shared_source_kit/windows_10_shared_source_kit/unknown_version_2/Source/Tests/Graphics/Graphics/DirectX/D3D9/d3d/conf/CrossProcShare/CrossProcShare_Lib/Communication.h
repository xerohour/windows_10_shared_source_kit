#ifndef _CCommunication_H_
#define _CCommunication_H_

#include "CD3DTest.h"
#include "EnumResource.h"
#include <vector>

const UINT MSG_EXIT	= 0xdeaddead;
const UINT MSG_PING	= 0xb33fb33f;

// Test results
const UINT TEST_SUCCESS							= 0;
const UINT TEST_FAIL							= 1;
const UINT TEST_SKIP							= 2;

struct SHAREDMEMORY
{
	DWORD dwMessage;
	bool bReadOnlyResource;
	HANDLE hShared;
	RESOURCE_CASE resCase;
};

struct CLIENTSHAREDMEMORY
{
	char szFailure[2048];
    UINT uTestResult;
};

struct PROCESSDATA
{
	HANDLE hProcess;
	HANDLE hMailbox;
	HANDLE hMailboxReceipt;
	HANDLE hFileMap;
	CLIENTSHAREDMEMORY *pSharedMemory;
};

class CCommunication : public CD3DTest
{
protected:
	std::vector <PROCESSDATA*>	m_listClientProcess;

	bool					m_bServer;
	int						m_iClientId;

	HANDLE					m_hFileMap;			//Handle to the shared memory
	HANDLE					m_hClientFileMap;	//Handle to the CLIENT shared memory
	HANDLE					m_hClientMailbox;	//Event sent to client to tell him he got a message
	HANDLE					m_hClientMailboxReceipt;	//Event sent to server to tell him we are done with message
	SHAREDMEMORY			*m_pSharedMemory;	//pointer to the shared memory of the file mapping
	CLIENTSHAREDMEMORY		*m_pClientSharedMemory;	//pointer to the CLIENT shared memory of the file mapping

public:
	CCommunication();
	~CCommunication();

	bool CreateClientProcess(UINT uAdapterForNewProcess = 0, bool bCreateProtected = false);
	bool InitializeIPC(); //Initialize Inter Process Communication
	bool SendMessageToServer(DWORD dwMessage);
	bool SendMessageToClient(UINT uClient, DWORD dwMessage);
	bool SendMessageToClients(DWORD dwMessage);
	bool WaitForClientToBeDone(UINT uClient, DWORD dwTimeout);
	bool WaitForAllClientsToBeDone(DWORD dwTimeout);
	bool WaitForOneMessageFromServer();
	bool WaitForMessagesFromServer();

	bool DidAllClientsSucceeded();
    bool DidClientsSkip();

    bool LogClientsMessages();
    void ResetClientLog();

	void Log(UINT uLogLevel, const TCHAR *ptcsFmt, ...);

	virtual UINT OnMessageFromServer(DWORD dwMessage) = 0;

    void ClenupClientData(void);
    void ResetClientFailStatus();

	// Framework overloads
	virtual bool TestTerminate(void);
};

#endif
