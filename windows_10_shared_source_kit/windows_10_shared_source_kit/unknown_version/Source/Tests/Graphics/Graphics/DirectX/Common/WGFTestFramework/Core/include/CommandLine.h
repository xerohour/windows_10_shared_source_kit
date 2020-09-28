//////////////////////////////////////////////////////////////////////////
//  CommandLine.h
//  created:	2004/09/24
//
//  purpose: Declares a simple command line interpreter class.
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <windows.h>
#include <vector>
#include <string>

class CCommandLine
{
public:
	const LPTSTR GetArgumentString() const { return m_pArgumentString; }
	const LPTSTR GetArgument( size_t nIndex ) const { return (const LPTSTR)m_Arguments[ nIndex ].c_str(); }
	size_t GetNumArguments() const { return m_Arguments.size(); }

private:	
	CCommandLine();
	~CCommandLine();
	
	bool FetchArgument( __in LPCTSTR szArgString, __out LPTSTR *ppArg, __out LPCTSTR *ppRemaining );
	
	void ParseString( __in LPCTSTR szArgs );
	void ParseFile( __in LPCTSTR szFile );
	
private:
	std::vector< std::basic_string<TCHAR> > m_Arguments;
	LPTSTR m_pBuffer;
	LPTSTR m_pArgumentString;
	
	friend class CWGFTestFramework;
};
