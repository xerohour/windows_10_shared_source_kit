#include "pch.h"

#include "Framework.h"
#include "Shell.h"
#include "Logger.h"

CDXGTestLogger::CDXGTestLogger()
{
	m_uSpacesInResultTextLine = 2;
}

// --------------------------------------------------------------------------------------

HRESULT CDXGTestLogger::TraceToWindow
(
	UINT uLevel,
	__in const PCSTR sMsg
)
{
	// Make sure the string ends with a newline.  If not, append one.
	string text( sMsg );
	size_t l = strlen( sMsg );
	if( l > 0 && sMsg[l - 1] != '\n' )
		text.push_back( '\n' );
		

	if( GetShell() )
	{
		GetShell()->LogText( text.c_str() );
	}

	return S_OK;
}

// --------------------------------------------------------------------------------------

HRESULT CDXGTestLogger::TraceToWindow
(
	UINT uLevel,
	__in const PCWSTR sMsg
)
{
	// Make sure the string ends with a newline.  If not, append one.
	wstring text( sMsg );
	size_t l = wcslen( sMsg );
	if( l > 0 && sMsg[l - 1] != L'\n' )
		text.push_back( L'\n' );


	if( GetShell() )
	{
		GetShell()->LogText( text.c_str() );
	}

	return S_OK;
}


// --------------------------------------------------------------------------------------

PCTSTR CDXGTestLogger::GenerateCmdline
(
	__out_ecount( MAX_PATH ) PTSTR sBuffer
)
{
	_tcsncpy
	(
		sBuffer,
		GetFramework()->GetCommandLine()->GetArgumentString(),
		MAX_PATH - 1
	);

	sBuffer[MAX_PATH - 1] = _T('\0'); // PreFast likes this

	return sBuffer;
}

