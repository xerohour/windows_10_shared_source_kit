//////////////////////////////////////////////////////////////////////////
//  CommandLine.cpp
//  created:	2004/09/24
//
//  purpose: Implements a simple command line interpreter class.
//////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "CommandLine.h"

static TCHAR s_szNullString[] = _T( "" );

CCommandLine::CCommandLine()
{
	LPTSTR szArgs = GetCommandLine();
	
	size_t nLength = _tcslen( szArgs );
	
	// Duplicate the command line.
	m_pBuffer = new TCHAR[ nLength + 1];
	if( m_pBuffer == NULL )
		throw _T( "Out of memory" );
	_tcsncpy( m_pBuffer, szArgs, nLength );
	m_pBuffer[nLength] = 0;

	// Product studio will convert hyphens into 'soft hyphens' - You can see these using 'Alt 0150'
    // bogus warning: warning 6054: String 'argument 1' might not be zero-terminated
#pragma warning( push )
#pragma warning( disable : 6054 )
	while( LPTSTR foundHyphen = _tcschr( m_pBuffer, 150 ) )
	{
		*foundHyphen = _T('-');
	}
#pragma warning( pop )

	LPCTSTR pTemp = m_pBuffer;
	
	// Find the first argument by skipping executable file name
	FetchArgument( m_pBuffer, NULL, &pTemp );
	size_t len = _tcslen( pTemp );
	m_pArgumentString = new TCHAR[ len + 1];
	m_pArgumentString[len] = 0;
	if( len )
	{
		_tcsncpy( m_pArgumentString, pTemp, len );
		m_pArgumentString[len] = 0;
		ParseString( pTemp );
	}
}

bool CCommandLine::FetchArgument( __in LPCTSTR szArgString, __out LPTSTR *ppArg, __out LPCTSTR *ppRemaining )
{
	LPTSTR pArg = NULL;
	LPTSTR z = NULL;
	LPTSTR c = const_cast< LPTSTR >( szArgString );
	
	// Ignore leading whitespace.
	for( ; *c && ( *c == _T( ' ' ) || *c == _T( '\t' ) || *c == _T( '\r' ) || *c == _T( '\n' ) ); ++c )
	{
	}
	
	if( *c == 0 )
		return false;
	
	pArg = c;
	
	// Check for quoted strings.	
	if( *c == _T( '\"' ) )
	{
		*c =_T( ' ' );
		pArg = c = c + 1;
		
		// Scan to the end of the quoted string.
		for( z = c; *z && *z != _T( '\"' ); ++z )
		{
		}
		
		if( *z == _T( '\"' ) )
			*z = _T( ' ' );
	}
	else
	{
		// Scan to end of argument
		for( z = c + 1; *z && *z != _T( ' ' ) && *z != _T( '\t' ) && *z != _T( '\r' ) && *z != _T( '\n' ); ++z )
		{
		}
	}
	
	TCHAR last = *z;
	
	if( last != 0 )
	{
		// Terminate the argument.
		c = z + 1;
		*z = 0;
	}
	else
	{
		c = z;
	}
	
	if( ppArg )
		*ppArg = pArg;
	
	if( ppRemaining )	
		*ppRemaining = c;
	
	return true;
}

void CCommandLine::ParseString( __in LPCTSTR szArgs )
{
	// Scan the string for delimiters
	LPTSTR pArg = NULL;
	
	for( ; ; )
	{
		if( !FetchArgument( szArgs, &pArg, &szArgs ) )
			break;
		
		if( pArg[0] == '@' )
		{
			// The argument is a filename containing more arguments.
			// Load the file and parse arguments from the file.
			ParseFile( &pArg[1] );
		}
		else
		{
			m_Arguments.push_back( std::basic_string<TCHAR>(pArg) );
		}
	}
}

void CCommandLine::ParseFile( __in LPCTSTR szFile )
{
	FILE *pFile = _tfopen( szFile, _T( "rt" ) );
	
    // warning 6262: Function uses '65560' bytes of stack: exceeds /analyze:stacksize'16384'. 
    // 16000 characters is enough for command line. @jianchen, 10/14/2009
	if( pFile )
	{
        TCHAR buffer[16000] = _T( "" );
		fread( (void *) buffer, 1, 15999, pFile );
		buffer[15999] = 0;
		fclose( pFile );
		ParseString( buffer );
	}
}

CCommandLine::~CCommandLine()
{
	delete[] m_pArgumentString;
	m_pArgumentString = NULL;

	delete[] m_pBuffer;
	m_pBuffer = NULL;
}