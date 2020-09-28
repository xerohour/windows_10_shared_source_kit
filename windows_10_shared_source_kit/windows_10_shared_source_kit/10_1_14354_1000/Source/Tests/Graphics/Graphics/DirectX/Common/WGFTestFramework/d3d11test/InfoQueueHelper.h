#pragma once

#include <d3d11.h>
#include <vector>
#include <list>
#include <string>

class CTest;

class CInfoQueueHelper
{
public:
	enum MessageMatchResult_t
	{
		MessageMatch_Match				= 0x00,
		MessageMatch_NoMessage			= 0x01,
		MessageMatch_OutOfMemory		= 0x02,
		MessageMatch_MismatchCategory	= 0x04,
		MessageMatch_MismatchSeverity	= 0x08,
		MessageMatch_MismatchID			= 0x10,
		MessageMatch_MismatchString		= 0x20,
	};

	~CInfoQueueHelper();
	void SetInfoQueue(ID3D11InfoQueue *pInfoQueue);
	void AddExpectedMessage(D3D11_MESSAGE_ID ID, D3D11_MESSAGE_CATEGORY Category, D3D11_MESSAGE_SEVERITY Severity);
	void AddExpectedMessageString(D3D11_MESSAGE_ID ID, D3D11_MESSAGE_CATEGORY Category, D3D11_MESSAGE_SEVERITY Severity, const char *pDescription, ...);
	void ClearExpectedMessages();
	void ClearMatchedMessages();
	bool FindCoveredMessages(bool bIgnoreUnexpected = false);
	D3D11_MESSAGE_ID GetNextUncoveredMessageID();
	void LogUncoveredMessages(CTest* test);

protected:
	struct FULL_MESSAGE_EXPECTED
	{
		D3D11_MESSAGE_CATEGORY Category;
		D3D11_MESSAGE_SEVERITY Severity;
		D3D11_MESSAGE_ID ID;
		std::	string Description;
		int matchResult;
		INT64 matchIndex;
		bool isCovered;
		bool bCheckString;
	};

	MessageMatchResult_t CheckMessageMatches(D3D11_MESSAGE* d3dMsg, const FULL_MESSAGE_EXPECTED* testMsg);
	bool MarkMessageAsCovered(D3D11_MESSAGE* d3dMsg, INT64 msgIndex);

	ID3D11InfoQueue*	m_pInfoQueue;
	typedef std::list<FULL_MESSAGE_EXPECTED> messageList_t;
	messageList_t		m_MsgExpectedList;
	std::vector<bool>	m_InfoQueueMatches;
};
