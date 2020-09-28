// CasesManager.cpp: implementation of the CCasesManager class.
//
//////////////////////////////////////////////////////////////////////

#include "CasesManager.h"

//////////////////////////////////////////////////////////////////////
// CCases
//////////////////////////////////////////////////////////////////////

CCase::CCase()
{
	m_dwValue = 0;
	m_szDesc = new char[1];
	strcpy(m_szDesc, "");
}

// ----------------------------------------------------------------------------

CCase::CCase(DWORD dwValue, char* szDesc)
{
	m_dwValue = dwValue;
	m_szDesc = new char[strlen(szDesc) + 1];
	strcpy(m_szDesc, szDesc);
}

// ----------------------------------------------------------------------------

CCase::~CCase()
{
	delete[] m_szDesc;
}

// ----------------------------------------------------------------------------

DWORD CCase::GetValue()
{
	return m_dwValue;
}

// ----------------------------------------------------------------------------

char* CCase::GetDesc()
{
	return m_szDesc;
}

//////////////////////////////////////////////////////////////////////
// CCasesList
//////////////////////////////////////////////////////////////////////


CCasesList::CCasesList()
{
}

// ----------------------------------------------------------------------------

CCasesList::~CCasesList()
{
	Empty();
}

// ----------------------------------------------------------------------------

void CCasesList::Empty()
{
	LISTCASE::iterator iCase;
	for (iCase = m_listCases.begin(); iCase!=m_listCases.end(); iCase++)
	{
		if (*iCase)
			delete (*iCase);
	}
	m_listCases.erase(m_listCases.begin(), m_listCases.end());
}

// ----------------------------------------------------------------------------

void CCasesList::AddCase(DWORD dwValue, char* szDesc)
{
	CCase* pCase = new CCase(dwValue, szDesc);
	CCasesList::AddCase(pCase);
}

// ----------------------------------------------------------------------------

void CCasesList::AddCase(CCase *pCase)
{
	m_listCases.push_back(pCase);
}

// ----------------------------------------------------------------------------

UINT CCasesList::GetNbCases()
{
	return m_listCases.size();
}

// ----------------------------------------------------------------------------

bool CCasesList::DoesCaseExist(DWORD dwValue)
{
	LISTCASE::iterator iCase;
	for (iCase = m_listCases.begin(); iCase!=m_listCases.end(); iCase++)
	{
		if ((*iCase)->GetValue() == dwValue)
			return true;
	}

	return false;
}
