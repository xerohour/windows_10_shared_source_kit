//////////////////////////////////////////////////////////////////////////
//  Shell.h
//  created:	2004/09/22
//
//  purpose: Provides a base class for implementing a shell.
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "Test.h"

//////////////////////////////////////////////////////////////////////////
// CShellBase 
//
// Intended as a base class for a shell for the
// framework.
//////////////////////////////////////////////////////////////////////////
class CShellBase
{
public:
	CShellBase();
	virtual ~CShellBase() {}
	
	//
	// Overridable Methods
	//
	virtual bool Initialize() { return true; }
		// Creates the shell.
		// Called by the framework prior to application initialization.
		// Returns true if successful.
	
	virtual void Destroy() {}
		// Destroys the shell.
		
	virtual bool Update() { return true; }
		// Called by the framework each "frame" to facilitate
		// synchronous execution of the shell.
		// 
		// This is the correct place to implement the
		// message pump for window-based shells.
		//
		// Update() should return false if the framework
		// needs to terminate the application.

	virtual bool ProcessCommandLineArgument( const tstring &strArgument ) { return false; }
		// Called by the framework prior to initialization
		// to allow the shell an opportunity to manually process
		// specific command line arguments.  If possible
		// use InitOptionVariables() instead of manually processing
		// command line options.
		
	virtual void InitOptionVariables() {}
		// Called by the framework prior to initialization
		// to allow the shell to register custom shell options.
		// Using registered options provides automatic
		// display of options in the help system and
		// can be used to provide a way to modify
		// option values at execution time.
		
	virtual HWND GetMainHWnd() const { return NULL; }
	
	virtual void SetTitleText( const TCHAR *szText ) {}
	
	// Device window methods.
	virtual HWND GetDeviceRenderWindow( int nIndex ) { return NULL; }
	virtual void SetDeviceTitle( int nIndex, LPCTSTR szTitle, LPCTSTR szDesc ) {}
	virtual void SetDevicesWindowResizeable( bool bResizeable ) {}
	virtual bool SetNumDeviceWindows( int n ) { return true; }
	
	virtual bool AllowDebugOnFail() { return true; }

	virtual bool IsMobile() { return false; }
	virtual bool IsConsole() { return false; }
	
	virtual HBITMAP SetDeviceWindowBitmap( int nIndex, HBITMAP hBmp ) { return NULL; };
	
	//
	// Text Methods
	//

	virtual void StatusText( const TCHAR *szKey, const TCHAR *szText ) {}
		// Used to send status text to the shell.
		
	virtual void DetailsText( const TCHAR * szHeader, const TCHAR * szKey, const TCHAR *szText ) {}
		// Used to send message text to the shell.
		
	virtual void LogText( const CHAR *szText ) {}
	virtual void LogText( const WCHAR *szText ) {}
		// Used to send logger text to the shell.
		
	virtual void MessageText( const CHAR *szText ) {}
	virtual void MessageText( const WCHAR *szText ) {}
		// Used to send message text to the shell.
		
	virtual void ErrorText( const CHAR *szText ) {}
	virtual void ErrorText( const WCHAR *szText ) {}
		// Used to send error text to the shell.
};
