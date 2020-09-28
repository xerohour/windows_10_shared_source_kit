#include "InfoQueueHelper.h"
#include "WGFTestCore.h"
#include "D3D11ValueSets.h"
#include "Strsafe.h"

#define RELEASE(a) if (!(a)){} else (a)->Release();

CInfoQueueHelper::~CInfoQueueHelper()
{
	ClearExpectedMessages();
}

void
CInfoQueueHelper::SetInfoQueue(ID3D11InfoQueue *pInfoQueue)
{
	m_pInfoQueue = pInfoQueue;
	if (m_pInfoQueue)
	{
		// InfoQueue has a default storage filter to deny SEVERITY_INFO.
		m_pInfoQueue->ClearStorageFilter();
	}
}

//TODO: might want to make sure an ID is not added twice
void
CInfoQueueHelper::AddExpectedMessage(D3D11_MESSAGE_ID ID,
									 D3D11_MESSAGE_CATEGORY Category,
									 D3D11_MESSAGE_SEVERITY Severity)
{
	FULL_MESSAGE_EXPECTED ExpectedMessage;
	ExpectedMessage.ID = ID;
	ExpectedMessage.Category = Category;
	ExpectedMessage.Severity = Severity;
	ExpectedMessage.matchResult = MessageMatch_NoMessage;
	ExpectedMessage.matchIndex = -1;
	ExpectedMessage.isCovered = false;
	ExpectedMessage.bCheckString = false;
	m_MsgExpectedList.push_back( ExpectedMessage );
}

void
CInfoQueueHelper::AddExpectedMessageString(D3D11_MESSAGE_ID ID,
										   D3D11_MESSAGE_CATEGORY Category,
										   D3D11_MESSAGE_SEVERITY Severity,
										   const char *pDescription,
										   ...)
{
	char localBuffer[2048];
	va_list args;
	va_start(args, pDescription);
	StringCchVPrintf(localBuffer, 2047, pDescription, args);
	va_end(args);
	localBuffer[2047] = 0;
	FULL_MESSAGE_EXPECTED ExpectedMessage;
	ExpectedMessage.ID = ID;
	ExpectedMessage.Category = Category;
	ExpectedMessage.Severity = Severity;
	ExpectedMessage.Description = localBuffer;
	ExpectedMessage.matchResult = MessageMatch_NoMessage;
	ExpectedMessage.matchIndex = -1;
	ExpectedMessage.isCovered = false;
	ExpectedMessage.bCheckString = true;
	m_MsgExpectedList.push_back( ExpectedMessage );
}

//Returns false if message is not found
bool
CInfoQueueHelper::MarkMessageAsCovered(D3D11_MESSAGE* d3dMsg, INT64 msgIndex)
{
	messageList_t::iterator vIter;
	FULL_MESSAGE_EXPECTED *pExpectedMsg = NULL;
	for ( vIter = m_MsgExpectedList.begin( ) ; vIter != m_MsgExpectedList.end( ) ; ++vIter )
	{
		pExpectedMsg = &(*vIter);
		if (!pExpectedMsg->isCovered)
		{
			int testMatch = CheckMessageMatches(d3dMsg, pExpectedMsg);
			if (MessageMatch_Match == testMatch)
			{
				pExpectedMsg->matchResult = testMatch;
				pExpectedMsg->matchIndex = msgIndex;
				pExpectedMsg->isCovered = true;
				return true;
			}
			if (   MessageMatch_MismatchString == testMatch
				|| (MessageMatch_MismatchID == testMatch && pExpectedMsg->bCheckString)
				|| MessageMatch_MismatchSeverity == testMatch
				|| MessageMatch_MismatchCategory == testMatch)
			{
				// record close-but-not-quite mismatches
				pExpectedMsg->matchResult = testMatch;
				pExpectedMsg->matchIndex = msgIndex;
			}
		}
	}

	return false;
}

CInfoQueueHelper::MessageMatchResult_t
CInfoQueueHelper::CheckMessageMatches(D3D11_MESSAGE* d3dMsg,
									  const FULL_MESSAGE_EXPECTED* testMsg)
{
	if (!d3dMsg)
		return MessageMatch_NoMessage;
	int retval = MessageMatch_Match;
	if (d3dMsg->Category != testMsg->Category)
		retval |= MessageMatch_MismatchCategory;
	if (d3dMsg->Severity != testMsg->Severity)
		retval |= MessageMatch_MismatchSeverity;
	if (d3dMsg->ID != testMsg->ID)
		retval |= MessageMatch_MismatchID;
	if (testMsg->bCheckString)
	{
		size_t numchars = testMsg->Description.size();
		if (numchars > d3dMsg->DescriptionByteLength)
			numchars = d3dMsg->DescriptionByteLength;
		if (0 != _strnicmp(d3dMsg->pDescription, testMsg->Description.c_str(), numchars))
			retval |= MessageMatch_MismatchString;
	}
	return (MessageMatchResult_t)retval;
}

bool
CInfoQueueHelper::FindCoveredMessages(bool bIgnoreUnexpected)
{
	bool result = true;
	D3D11_MESSAGE *pMsg;
	SIZE_T sizeBlob = 1024;
	SIZE_T sizeMsg = sizeBlob;
	ID3D10Blob *blob = NULL;

	m_InfoQueueMatches.resize((size_t)m_pInfoQueue->GetNumStoredMessagesAllowedByRetrievalFilter());

	if (FAILED( D3D10CreateBlob((UINT)sizeMsg, &blob) ) ) 
	{
        result = false;
		goto exit;
    }

	pMsg = (D3D11_MESSAGE *)blob->GetBufferPointer();

	for (UINT64 i=0; i<m_pInfoQueue->GetNumStoredMessagesAllowedByRetrievalFilter(); i++)
	{
		//Get message size
		sizeMsg = 0;
		if (FAILED( m_pInfoQueue->GetMessage( i, NULL, &sizeMsg ) ) )
		{
   //         result = false;
			//goto exit;
		}

		if (sizeBlob < sizeMsg)
		{
			RELEASE( blob );
			if (FAILED( D3D10CreateBlob((UINT)sizeMsg, &blob) ) ) 
			{
				result = false;
				goto exit;
			}

			sizeBlob = sizeMsg;
			pMsg = (D3D11_MESSAGE *)blob->GetBufferPointer();
		}

		if (FAILED( m_pInfoQueue->GetMessage( i, pMsg, &sizeMsg ) ) )
		{
            result = false;
			goto exit;
		}

		if ( !MarkMessageAsCovered( pMsg, i ) )
		{
			m_InfoQueueMatches[(size_t)i] = false;
			if (!bIgnoreUnexpected)
				result = false;
		}
		else
			m_InfoQueueMatches[(size_t)i] = true;
	}

	{
		messageList_t::iterator vIter;
		FULL_MESSAGE_EXPECTED *pExpectedMsg = NULL;
		for ( vIter = m_MsgExpectedList.begin( ) ; vIter != m_MsgExpectedList.end( ) ; ++vIter )
		{
			pExpectedMsg = &(*vIter);
			if ( pExpectedMsg->isCovered == false )
			{
				result = false;
			}
		}
	}
exit:
	RELEASE( blob );

	return result;
}

//Returns D3D11_MESSAGE_ID_UNKNOWN if no more uncovered message id
//Otherwise returns the first uncovered id and mark it as covered
D3D11_MESSAGE_ID
CInfoQueueHelper::GetNextUncoveredMessageID()
{
	messageList_t::iterator vIter;
	FULL_MESSAGE_EXPECTED *pExpectedID = NULL;
	for ( vIter = m_MsgExpectedList.begin( ) ; vIter != m_MsgExpectedList.end( ) ; ++vIter )
	{
		pExpectedID = &(*vIter);
		if ( pExpectedID->isCovered == false )
		{
			pExpectedID->isCovered = true;
			return pExpectedID->ID;
		}
	}

	return D3D11_MESSAGE_ID_UNKNOWN;
}

void
CInfoQueueHelper::LogUncoveredMessages(CTest* test)
{
	messageList_t::iterator vIter;
	// this loop covers the expected messages that were not found
	for ( vIter = m_MsgExpectedList.begin( ) ; vIter != m_MsgExpectedList.end( ) ; ++vIter )
	{
		FULL_MESSAGE_EXPECTED *pExpectedMsg = NULL;
		pExpectedMsg = &(*vIter);
		if ( pExpectedMsg->isCovered == false )
		{
			tstring strCategory = ToString(pExpectedMsg->Category);
			tstring strSeverity = ToString(pExpectedMsg->Severity);
			tstring strID = ToString(pExpectedMsg->ID);
			if (pExpectedMsg->bCheckString)
			{
				test->WriteToLog( _T("[InfoQueueHelper] Unmatched Expected Message: Category=%s Severity=%s ID=%s Description='%s'\n"),
					strCategory.c_str(),
					strSeverity.c_str(),
					strID.c_str(),
					pExpectedMsg->Description.c_str());
			}
			else
			{
				test->WriteToLog( _T("[InfoQueueHelper] Unmatched Expected Message: Category=%s Severity=%s ID=%s NoDescription\n"),
					strCategory.c_str(),
					strSeverity.c_str(),
					strID.c_str());
			}
			if (pExpectedMsg->matchIndex >= 0)
			{
				tstring matchDesc;
				switch (pExpectedMsg->matchResult)
				{
				default:
					matchDesc = "Unknown";
					break;
				case MessageMatch_MismatchString:
					matchDesc = "Description";
					break;
				case MessageMatch_MismatchID:
					matchDesc = "ID";
					break;
				case MessageMatch_MismatchSeverity:
					matchDesc = "Severity";
					break;
				case MessageMatch_MismatchCategory:
					matchDesc = "Category";
					break;
				}
				test->WriteToLog( _T("[InfoQueueHelper] Message was close to matching with index %I64d, mismatch only by %s\n"),
					pExpectedMsg->matchIndex,
					matchDesc.c_str());
			}
		}
	}
	// this loop covers the messages that were found but not expected
	for (UINT64 i = 0; i < m_InfoQueueMatches.size(); ++i)
	{
		if (!m_InfoQueueMatches[(size_t)i])
		{
			SIZE_T messageSize = 0;
			HRESULT hr = m_pInfoQueue->GetMessage(i, NULL, &messageSize);
			ID3D10Blob *blob = NULL;
			if (FAILED(hr) || FAILED( D3D10CreateBlob((UINT)messageSize, &blob) ) ) 
			{
				test->WriteToLog( _T("Could not retrieve info queue message at index %d\n"), (int)i);
				continue;
			}
			D3D11_MESSAGE* pMsg = (D3D11_MESSAGE *)blob->GetBufferPointer();
			hr = m_pInfoQueue->GetMessage(i, pMsg, &messageSize);
			if (FAILED(hr)) 
			{
				test->WriteToLog( _T("Could not retrieve info queue message at index %d\n"), (int)i);
			}
			else
			{
				tstring strCategory = ToString(pMsg->Category);
				tstring strSeverity = ToString(pMsg->Severity);
				tstring strID = ToString(pMsg->ID);
				test->WriteToLog( _T("[InfoQueueHelper] Unexpected Message @ %I64d: Category=%s Severity=%s ID=%s Description='%s'\n"),
					i,
					strCategory.c_str(),
					strSeverity.c_str(),
					strID.c_str(),
					pMsg->pDescription);
			}
			blob->Release();
		}
	}
}

void
CInfoQueueHelper::ClearMatchedMessages()
{
	m_InfoQueueMatches.clear();
}

void
CInfoQueueHelper::ClearExpectedMessages()
{
	m_MsgExpectedList.clear();
	m_InfoQueueMatches.clear(); //These are no longer valid
}
