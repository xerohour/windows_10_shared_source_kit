#pragma once

#include "WGFTestUIWindow.h"
#include "WGFTestUIDeviceWindow.h"
#include "WGFTestUIStatus.h"
#include "WGFTestUILog.h"
#include "WGFTestUIGroups.h"
#include "WGFTestUITestCases.h"
#include "WGFTestUIGoToDialog.h"
#include "WGFTestUIFiltersDialog.h"
#include "WGFTestUIOptionsDialog.h"
#include "WGFTestUIParametersDialog.h"
#include "WGFTestUIHelpDialog.h"

#define DEFAULT_RENDER_WINDOW_WIDTH		320
#define DEFAULT_RENDER_WINDOW_HEIGHT	280
#define DEFAULT_NUM_DEVICE_WINDOWS		2
#define MAX_DEVICE_WINDOWS				8

enum WGFTESTUI_STATE
{
	STATE_PLAY,
	STATE_PLAY_ONCE,
	STATE_PAUSE,
	STATE_STEPFORWARD,
	STATE_STEPBACK,
	STATE_SKIPFORWARD,
	STATE_SKIPBACK,
	STATE_REPLAY,
	STATE_FINISHED,
	STATE_INITFAIL,
	STATE_EXECUTE,
};

// struct for communicating with U32OverCUI.dll
struct U32OverCUI_WindowOptions
{
	WNDPROC  wProc;
	LPCWSTR szEndpointName;
	bool bFullscreen;
	bool bDoInitialPresent;
};

typedef U32OverCUI_WindowOptions( WINAPI *U32OverCUI_GetDefaultWindowOptions )( void );
typedef HWND( WINAPI *U32OverCUI_CreateWindow )( U32OverCUI_WindowOptions );
typedef bool( WINAPI *U32OverCUI_DestroyWindow )( HWND );

//////////////////////////////////////////////////////////////////////////
// CWGFTestUIMessageDialog
//////////////////////////////////////////////////////////////////////////
class CWGFTestUIMessageDialog : public CWGFTestUIDialog
{
	tstring m_messageText;
	tstring m_messageTitle;
	
public:
	CWGFTestUIMessageDialog() {}
	INT_PTR Execute( CWGFTestUIWindow *pParent, const tstring &messageText, const tstring &messageTitle );
	void SetMessageText( const tstring &messageText );
	INT_PTR DialogProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
};


//////////////////////////////////////////////////////////////////////////
// CWGFTestUISplitter
//////////////////////////////////////////////////////////////////////////
class CWGFTestUISplitter : public CWGFTestUIWindow
{
public:
	CWGFTestUISplitter() : m_hbrPattern( NULL ), m_hbmp( NULL ) {}
	
	bool Create( UINT uID, CWGFTestUIWindow *pParent, int nMarginTop, int nMarginBottom );
	int GetDropPos() const { return m_iCursorY; }

private:
	LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	
	void DrawMoveRect( bool bEraseOld );
	void EraseMoveRect();
	
private:
	RECT m_rcDrag;
	int m_iCursorY;
	int m_nMarginTop;
	int m_nMarginBottom;
	HBITMAP m_hbmp;
	HBRUSH m_hbrPattern;
};

class CWGFTestUIMainStatusBar : public CWGFTestUIStatusBar
{
	bool DrawItemCallback( CWGFTestUIWindow *pParent, LPDRAWITEMSTRUCT lpdis );
	void DrawProgress( HDC hdc, LPCTSTR szText, COLORREF cr, LPRECT prc, size_t iValue, size_t iMax );
};

class CWGFTestUIShell : public CWGFTestUIWindow, public CShellBase
{
public:
	CWGFTestUIShell();
	~CWGFTestUIShell();
	
	//
	// CShellBase class overrides
	//
	bool Initialize();
	void Destroy();
	
	void InitOptionVariables();
	
	bool Update();
	//bool ProcessCommandLineArgument( const tstring &strArgument );
	HWND GetMainHWnd() const;
	void SetTitleText( const TCHAR *szText );
	void StatusText( const TCHAR *szKey, const TCHAR *szText );
	void DetailsText( const TCHAR * szHeader, const TCHAR * szKey, const TCHAR *szText );

	void LogText( const CHAR *szText );
	void LogText( const WCHAR *szText );

	void MessageText( const CHAR *szText );
	void MessageText( const WCHAR *szText );

	void ErrorText( const CHAR *szText );
	void ErrorText( const WCHAR *szText );

	virtual HWND GetDeviceRenderWindow( int nIndex );
	virtual void SetDeviceTitle( int nIndex, LPCTSTR szTitle, LPCTSTR szDesc );
	virtual void SetDeviceWindowsResizeable( bool bResizeable );
	virtual bool SetNumDeviceWindows( int n );

	virtual HBITMAP SetDeviceWindowBitmap( int nIndex, HBITMAP hBmp );
	
	virtual bool AllowDebugOnFail();

	virtual bool IsMobile() { return m_bMobile; }
	
	//
	// CWGFTestUIWindow class overrides
	//
	LRESULT WindowProc(	UINT uMsg, WPARAM wParam, LPARAM lParam );
	
	//
	// Methods
	//
	WGFTESTUI_STATE GetState() const { return m_State; }
	void SetState( WGFTESTUI_STATE State );
	
private:
	void InitDevWindowPositions( bool bResize );
	
	enum RecalcMethod
	{
		RECALC_TO_FIT_DEVICE_WINDOWS,
		RECALC_FROM_USER_ADJUSTMENT,
	};
	void RecalcLayout( RecalcMethod rm );
	CWGFTestUIWindow *GetTabbedWindow();
	void AdjustTabbedControls();
	
	void DrawStatusItem( LPDRAWITEMSTRUCT pdis );
	void DrawProgress( HDC hdc, LPCTSTR szText, COLORREF cr, LPRECT prc, size_t iValue, size_t iMax );
	
	void FlushPaintMessages();
	bool FlushMessages();
	
protected:
	bool m_bLaunchInStepMode;
	bool m_bHaltOnFail;
	bool m_bHaltOnFinish;
	bool m_bStartup;
	bool m_bMobile;
	
	WGFTESTUI_STATE m_State;
	HACCEL m_hAccelerators;
	bool m_bLockDevWindowSize;
	bool m_bLockDevWindowAspect;

private:
	CWGFTestUIToolbar m_toolbar;
	CWGFTestUIMainStatusBar m_statusbar;
	CWGFTestUITabControl m_tabControl;
	CWGFTestUISplitter m_splitter;
	CWGFTestUIMessageDialog m_errorDialog;
	int m_nNumDeviceWindows;
	CWGFTestUIDeviceWindow m_DeviceWindows[MAX_DEVICE_WINDOWS];
	HBITMAP m_hDevWndBmp[MAX_DEVICE_WINDOWS];
	int m_iDeviceWindowAspect;
	int m_iRenderWindowWidth;
	int m_iRenderWindowHeight;
	int m_iSplitterHeight;
	HMODULE m_hU32OverCUI;
	
	// Tabbed windows
	CWGFTestUIStatusWindow    m_StatusWindow;
	CWGFTestUIGroupsWindow    m_GroupsWindow;
	CWGFTestUITestCasesWindow m_TestCasesWindow;
	CWGFTestUILogWindow       m_LogWindow;
	CWGFTestUILogWindow       m_MessageWindow;
	
	CWGFTestUIGoToDialog m_DlgGoTo;
	CWGFTestUIFiltersDialog m_DlgFilters;
	CWGFTestUIOptionsDialog m_DlgOptions;
	CWGFTestUIParametersDialog m_DlgParams;
	CWGFTestUIHelpDialog m_DlgHelp;

	size_t m_nLastTestCase;
	TEST_RESULT m_LastResult;
	bool m_bUpdateStatusControl;
	bool m_bUpdateStatusBar;
	bool m_bUpdateStatusBarPath;
	
	int m_iStatusTabIndex;
	int m_iGroupsTabIndex;
	int m_iCasesTabIndex;
	int m_iLogTabIndex;
	int m_iMessageTabIndex;
	
	int m_nDelay;

	bool m_bExiting;
};



