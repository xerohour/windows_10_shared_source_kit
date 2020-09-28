#include "pch.h"
#include "richedit.h"

#include "WGFTestUILog.h"

#define STRSAFE_NO_DEPRECATE
#include <strsafe.h>


void DPF( int i, LPCSTR szFormat, ... )
{
	CHAR szBuffer[ 4096 ] = "";

	va_list arglist;
	va_start( arglist, szFormat );
	StringCchVPrintfA
	(
		szBuffer,
		ARRAYSIZE( szBuffer ),
		szFormat,
		arglist
	);
	va_end( arglist );
	
	_DPF( "BUGBUG: FIX DPF - %s", szBuffer );
}

void DPF( int i, LPCWSTR szFormat, ... )
{
	WCHAR szBuffer[ 4096 ] = L"";

	va_list arglist;
	va_start( arglist, szFormat );
	StringCchVPrintfW
	(
		szBuffer,
		ARRAYSIZE( szBuffer ),
		szFormat,
		arglist
	);
	va_end( arglist );
	
	_DPF( L"BUGBUG: FIX DPF - %s", szBuffer );
}

bool CWGFTestUILogWindow::Create( UINT uID, CWGFTestUIWindow *pParent )
{
	UINT l = GetSystemDirectory( NULL, 0 );
	l += sizeof( _T( "riched32.dll" ) ) + 1;
	TCHAR *szPath = new TCHAR[ l ];
	if( !szPath )
		return false;
	UINT n = GetSystemDirectory( szPath, l );
	_tcscpy( szPath + n, _T( "\\riched32.dll" ) );
	szPath[l - 1] = 0;
	LoadLibrary( szPath );
	delete[] szPath;

	if( !CreateEx( 0, RICHEDIT_CLASS, NULL, WS_CHILD | ES_SUNKEN | ES_MULTILINE | ES_READONLY | ES_DISABLENOSCROLL | ES_AUTOVSCROLL | WS_HSCROLL | WS_VSCROLL | ES_WANTRETURN, 0, 0, 300, 128, pParent, (HMENU) (INT_PTR) uID, 0 ) )
		return false;

	// Initialize colors and font
	SendMessage( EM_SETBKGNDCOLOR, 0, (LPARAM) RGB( 0, 0, 0 ) );

	CHARFORMAT cf;
	cf.cbSize = sizeof( cf );
	cf.dwMask = CFM_COLOR | CFM_FACE | CFM_BOLD;
	cf.dwEffects = 0;
	_tcsncpy( cf.szFaceName, _T( "Courier New" ), sizeof( cf.szFaceName ) );
	cf.crTextColor = RGB( 191, 191, 191 );
	SendMessage( EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM) &cf );

	SendMessage( EM_SETSEL, (WPARAM) 0, (LPARAM) -1 );

	return true;
}

void CWGFTestUILogWindow::WriteText( const TCHAR *szText )
{
	// Make sure the string ends with a newline.  If not, append one.
	tstring text( szText );
	size_t l = _tcslen( szText );
	if( l > 0 && szText[l - 1] != '\n' )
		text.push_back( '\n' );


	// Get the edit control's current selection.
	DWORD SelectionStart = 0;
	DWORD SelectionEnd = 0;
	LRESULT lResult = SendMessage
	(
		EM_GETSEL,
		(WPARAM) &SelectionStart,
		(LPARAM) &SelectionEnd
	);

 
	// Clear the selection.		
	SendMessage
	(
		EM_SETSEL,
		(WPARAM) -1,
		(LPARAM) -1
	);


	// Replace the control's text.
	SendMessage
	(
		EM_REPLACESEL,
		0,
		(LPARAM) text.c_str()
	);


	// Restore the original selection.
	if ( lResult != -1 )
	{
		SendMessage
		(
			EM_SETSEL,
			(WPARAM) SelectionStart,
			(LPARAM) SelectionEnd
		);

#if 0
		// Scroll the caret into view.
		SendMessage
		(
			EM_SCROLLCARET,
			0,
			0
		);
#endif
	};
}

void CWGFTestUILogWindow::Clear()
{
	SetWindowText( _T( "" ) );
}

LRESULT CWGFTestUILogWindow::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case WM_SHOWWINDOW:
			if( wParam )
			{
				//SetFocus( GetHWnd() );
			}
			break;
			
		case WM_VSCROLL:
		{
			SCROLLINFO si;
			si.cbSize = sizeof( si );
			si.fMask = SIF_ALL;
			GetScrollInfo( GetHWnd(), SB_VERT, &si );
		}
	}
	
	return CWGFTestUIWindow::WindowProc( uMsg, wParam, lParam );
}


