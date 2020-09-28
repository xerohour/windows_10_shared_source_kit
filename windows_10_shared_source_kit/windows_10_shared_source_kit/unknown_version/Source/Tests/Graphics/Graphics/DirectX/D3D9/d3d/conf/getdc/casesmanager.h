// CasesManager.h: interface for the CCasesManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CASESMANAGER_H__17E83D28_EF72_4797_A7FD_A2E5A555BF21__INCLUDED_)
#define AFX_CASESMANAGER_H__17E83D28_EF72_4797_A7FD_A2E5A555BF21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <windows.h>

class CCase;

typedef std::vector<class CCase*>			LISTCASE;

//CCase could be overloaded so that Value is something different than DWORD
class CCase
{
protected:
	DWORD	m_dwValue;
	char*	m_szDesc;
	UINT	m_uRefCount;

public:
	CCase();
	CCase(DWORD dwValue, char* szDesc);
	virtual ~CCase();
	void IncRefCount();
	UINT Release();

	UINT  GetRefCount();
	DWORD GetValue();
	char* GetDesc();
};

/* Exemple of overloading CCase to have other value than DWORD
class CCHARCase : public CCase
{
protected:
	char m_cValue;
public:
	CCHARCase(char cValue, char* szDesc) {m_cValue = cValue;}
	char GetValue() {return m_cValue;}
};
*/

class CSizeCase : public CCase
{
protected:
	UINT m_uWidth;
	UINT m_uHeight;

public:
	CSizeCase(DWORD dwValue, char* szDesc, UINT uWidth, UINT uHeight) : CCase(dwValue, szDesc)
	{
		m_uWidth = uWidth;
		m_uHeight = uHeight;
	};

	UINT GetWidth() { return m_uWidth; }
	UINT GetHeight() { return m_uHeight; }
};

// ----------------------------------------------------------------------------

class CCasesList
{
protected:
	LISTCASE		m_listCases;

public:
	CCasesList();
	virtual ~CCasesList();

	void AddCase(DWORD dwValue, char* szDesc);
	void AddCase(CCase *pCase);

	UINT GetNbCases();
	bool DoesCaseExist(DWORD dwValue);
	void Empty();

	CCase* operator[](UINT iCaseIndex) { return (iCaseIndex < GetNbCases())? m_listCases[iCaseIndex] : NULL; }
};

// ----------------------------------------------------------------------------

#endif // !defined(AFX_CASESMANAGER_H__17E83D28_EF72_4797_A7FD_A2E5A555BF21__INCLUDED_)
