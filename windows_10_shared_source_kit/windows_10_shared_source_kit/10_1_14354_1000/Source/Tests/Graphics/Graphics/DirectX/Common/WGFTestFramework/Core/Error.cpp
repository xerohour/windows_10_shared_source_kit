//////////////////////////////////////////////////////////////////////////
//  Error.cpp
//  created:	2004/10/01
//
//  purpose: Handles error output.
//////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "Shell.h"
#include "Framework.h"
#include "Error.h"

#include <strsafe.h>


void WGFTESTAPI WriteErrorA( const CHAR *szFormat, ... )
{
	// Format the error.
	CHAR szBuffer[ 4096 ] = "";

	va_list args;
	va_start( args, szFormat );
	StringCchVPrintfA
	(
		szBuffer,
		ARRAYSIZE( szBuffer ),
		szFormat,
		args
	);
	va_end( args );


	// Write the error to the shell (or to stderr if no shell is present).
	CShellBase *pShell = GetShell();
	
	if( pShell && (GetFramework()->IsMobile() == FALSE))
	{
		pShell->ErrorText( szBuffer );
	}
	else
	{
		fprintf( stderr, "%s\n", szBuffer );
	}


	// Write the error to the appropriate log targets.
	GetFramework()->WriteToLog( 1, szBuffer );
}


void WGFTESTAPI WriteErrorW( const WCHAR *szFormat, ... )
{
	// Format the error.
	WCHAR szBuffer[ 4096 ] = L"";

	va_list args;
	va_start( args, szFormat );
	StringCchVPrintfW
	(
		szBuffer,
		ARRAYSIZE( szBuffer ),
		szFormat,
		args
	);
	va_end( args );


	// Write the error to the shell (or to stderr if no shell is present).
	CShellBase *pShell = GetShell();

	if( pShell && (GetFramework()->IsMobile() == FALSE))
	{
		pShell->ErrorText( szBuffer );
	}
	else
	{
		fwprintf( stderr, L"%s\n", szBuffer );
	}


	// Write the error to the appropriate log targets.
	GetFramework()->WriteToLog( 1, szBuffer );
}
