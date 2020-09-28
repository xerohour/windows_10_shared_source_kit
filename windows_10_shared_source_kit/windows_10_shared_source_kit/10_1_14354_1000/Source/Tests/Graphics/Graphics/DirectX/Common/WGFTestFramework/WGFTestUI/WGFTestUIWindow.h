#pragma once

extern ATOM __stdcall WGFTestUIRegisterWndClass( LPCTSTR szClassName, UINT uStyle, HICON hIcon, HCURSOR hCursor, HBRUSH hbrBackground );


//////////////////////////////////////////////////////////////////////////
// WGFTestUIDrawText
//
// Same as normal DrawText with two exceptions:
// * A '$' character indicates that the next character(s) describe a color 
//   change.  The exception is "$$" which indicates a single '$' character
//   with no color change.
// * Only left aligned, single line text can be drawn.  Multilined, center aligned, and right aligned are ignored.
//////////////////////////////////////////////////////////////////////////
extern int __stdcall WGFTestUIDrawText( HDC hDC, LPCTSTR szText, int nCount, LPRECT lpRect, UINT uFormat );



//////////////////////////////////////////////////////////////////////////
// CWGFTestUIWindow
//////////////////////////////////////////////////////////////////////////
class CWGFTestUIWindow
{
public:
	CWGFTestUIWindow();
	virtual ~CWGFTestUIWindow();
	
	HWND GetHWnd() const { return m_hWnd; }
	HWND GetHWndParent() const { return ::GetParent( m_hWnd ); }
	CWGFTestUIWindow *GetParent();
	CWGFTestUIWindow *GetAncestor( UINT gaFlags );
	
	static CWGFTestUIWindow *__stdcall FromHWnd( HWND hWnd );
		// Returns a pointer to the CWGFTestUIWindow object attached to the window.  If the
		// window is not attached to a CWGFTestUIWindow object then it returns a pointer
		// to a temporary CWGFTestUIWindow object.  
		// NOTE: It is not safe to cache the return value of FromWHnd() for later use.
	
	virtual bool CreateEx(	DWORD dwExStyle, 
							LPCTSTR lpszClassName, 
							LPCTSTR lpszWindowName,
							DWORD dwStyle,
							int x,
							int y, 
							int nWidth,
							int nHeight, 
							CWGFTestUIWindow *pParent,
							HMENU hMenu,
							LPVOID lParam
							);
							
	bool Attach( HWND hWnd );
		// Attaches this CWGFTestUIWindow object to a window.
		
	HWND Detach();
		// Detaches this CWGFTestUIWindow object from the attached window.
	
	virtual void DestroyWindow();
	
	virtual bool DrawItemCallback( CWGFTestUIWindow *pParent, LPDRAWITEMSTRUCT lpdis ) { return false; }
	
	virtual bool NotifyCallback( int idCtrl, LPNMHDR pnmhdr ) { return false; }
		
	//
	// Wrapper Methods
	//
	CWGFTestUIWindow *SetParent( CWGFTestUIWindow *pNewParent ) { return FromHWnd( ::SetParent( GetHWnd(), pNewParent->GetHWnd() ) ); }
	
	BOOL AnimateWindow( DWORD dwTime, DWORD dwFlags ) { return ::AnimateWindow( m_hWnd, dwTime, dwFlags ); }
	UINT ArrangeIconicWindows() { return ::ArrangeIconicWindows( m_hWnd ); }
	BOOL BringWindowToTop() { return ::BringWindowToTop( m_hWnd ); }
	WORD CascadeWindows( UINT uHow, const RECT *lpRect, UINT cKids, const HWND *lpKids ) { return ::CascadeWindows( m_hWnd, uHow, lpRect, cKids, lpKids ); }
	
	HWND ChildWindowFromPoint( POINT pt ) { return ::ChildWindowFromPoint( m_hWnd, pt ); }
	HWND ChildWindowFromPointEx( POINT pt, UINT uFlags ) { return ::ChildWindowFromPointEx( m_hWnd, pt, uFlags ); }
	HWND GetLastActivePopup() { return ::GetLastActivePopup( m_hWnd ); }
	HWND GetTopWindow() { return ::GetTopWindow( m_hWnd ); }
	HWND GetWindow( UINT uCmd ) { return ::GetWindow( m_hWnd, uCmd ); }
	BOOL ShowOwnedPopups( BOOL bShow ) { return ::ShowOwnedPopups( m_hWnd, bShow ); }
	
	BOOL GetWindowInfo( PWINDOWINFO pwi ) { return ::GetWindowInfo( m_hWnd, pwi ); }
	UINT GetWindowModuleFileName( LPTSTR lpszFileName, UINT cchFileNameMax ) { return ::GetWindowModuleFileName( m_hWnd, lpszFileName, cchFileNameMax ); }
	DWORD GetWindowThreadProcessId( LPDWORD lpdwProcessId ) { return ::GetWindowThreadProcessId( m_hWnd, lpdwProcessId ); }
	
	BOOL CloseWindow() { return ::CloseWindow( m_hWnd ); }
	
	BOOL GetClientRect( LPRECT lpRect ) { return ::GetClientRect( m_hWnd, lpRect ); }
	BOOL GetWindowRect( LPRECT lpRect ) { return ::GetWindowRect( m_hWnd, lpRect ); }
	BOOL GetWindowPlacement( WINDOWPLACEMENT *lpwndpl ) { return ::GetWindowPlacement( m_hWnd, lpwndpl ); }
	BOOL SetWindowPlacement( WINDOWPLACEMENT *lpwndpl ) { return ::SetWindowPlacement( m_hWnd, lpwndpl ); }
	BOOL SetWindowPos( HWND hWndInsertAfter, int x, int y, int cx, int cy, UINT uFlags ) { return ::SetWindowPos( m_hWnd, hWndInsertAfter, x, y, cx, cy, uFlags ); }
	BOOL MoveWindow( int x, int y, int nWidth, int nHeight, BOOL bRepaint ) { return ::MoveWindow( m_hWnd, x, y, nWidth, nHeight, bRepaint ); }
	BOOL ShowWindow( int nCmdShow ) { return ::ShowWindow( m_hWnd, nCmdShow ); }
	BOOL ScreenToClient( LPPOINT pt ) { return ::ScreenToClient( m_hWnd, pt ); }
	BOOL ClientToScreen( LPPOINT pt ) { return ::ClientToScreen( m_hWnd, pt ); }
	BOOL ScreenToClient( LPRECT prc ) { return ::ScreenToClient( m_hWnd, (LPPOINT) &prc->left ) && ::ScreenToClient( m_hWnd, (LPPOINT) &prc->right ); }
	BOOL ClientToScreen( LPRECT prc ) { return ::ClientToScreen( m_hWnd, (LPPOINT) &prc->left ) && ::ClientToScreen( m_hWnd, (LPPOINT) &prc->right ); }
	
	BOOL SetWindowText( LPCTSTR lpString ) { return ::SetWindowText( m_hWnd, lpString ); }
	int GetWindowText( LPTSTR lpString, int nMaxCount ) { return ::GetWindowText( m_hWnd, lpString, nMaxCount ); }
	int GetWindowTextLength() { return ::GetWindowTextLength( m_hWnd ); }
	
	BOOL GetTitleBarInfo( PTITLEBARINFO pti ) { return ::GetTitleBarInfo( m_hWnd, pti ); }

	BOOL IsIconic() { return ::IsIconic( m_hWnd ); }
	BOOL IsWindowVisible() { return ::IsWindowVisible( m_hWnd ); }
	BOOL IsWindowEnabled() { return ::IsWindowEnabled( m_hWnd ); }	
	BOOL IsZoomed() { return ::IsZoomed( m_hWnd ); }
	BOOL OpenIcon() { return ::OpenIcon( m_hWnd ); }
	BOOL SetForegroundWindow() { return ::SetForegroundWindow( m_hWnd ); }
	
	// Painting methods.
	HDC GetDC() { return ::GetDC( m_hWnd ); }
	HDC BeginPaint( LPPAINTSTRUCT lpPaint ) { return ::BeginPaint( m_hWnd, lpPaint ); }
	BOOL EndPaint( CONST PAINTSTRUCT *lpPaint ) { return ::EndPaint( m_hWnd, lpPaint ); }
	BOOL DrawCaption( HDC hdc, LPCRECT lprc, UINT uFlags ) { return ::DrawCaption( m_hWnd, hdc, lprc, uFlags ); }
	BOOL GetUpdateRect( LPRECT lpRect, BOOL bErase ) { return ::GetUpdateRect( m_hWnd, lpRect, bErase ); }
	int GetUpdateRgn( HRGN hRgn, BOOL bErase ) { return ::GetUpdateRgn( m_hWnd, hRgn, bErase ); }
	HDC GetWindowDC() { return ::GetWindowDC( m_hWnd ); }
	int ReleaseDC( HDC hDC ) { return (int) ::ReleaseDC( m_hWnd, hDC ); }
	int GetWindowRgn( HRGN hRgn ) { return ::GetWindowRgn( m_hWnd, hRgn ); }
	BOOL InvalidateRect( CONST RECT *lprc, BOOL bErase ) { return ::InvalidateRect( m_hWnd, lprc, bErase ); }
	BOOL InvalidateRgn( HRGN hRgn, BOOL bErase ) { return ::InvalidateRgn( m_hWnd, hRgn, bErase ); }
	BOOL LockWindowUpdate() { return ::LockWindowUpdate( m_hWnd ); }
	BOOL RedrawWindow( CONST RECT *lprcUpdate, HRGN hrgnUpdate, UINT flags ) { return ::RedrawWindow( m_hWnd, lprcUpdate, hrgnUpdate, flags ); }
	int SetWindowRgn( HRGN hRgn, BOOL bRedraw ) { return ::SetWindowRgn( m_hWnd, hRgn, bRedraw ); }
	BOOL UpdateWindow() { return ::UpdateWindow( m_hWnd ); }
	BOOL ValidateRect( CONST RECT *lprc ) { return ::ValidateRect( m_hWnd, lprc ); }
	BOOL ValidateRgn( HRGN hRgn ) { return ::ValidateRgn( m_hWnd, hRgn ); }
	
	// Message methods.
	BOOL PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam ) { return ::PostMessage( m_hWnd, uMsg, wParam, lParam ); }
	LRESULT SendMessage( UINT uMsg, WPARAM wParam, LPARAM lParam ) { return ::SendMessage( m_hWnd, uMsg, wParam, lParam ); }

protected:	
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	
protected:
	HWND m_hWnd;	
	
private:
	WNDPROC m_pDefaultWndProc;

private:	
	static LRESULT CALLBACK ProxyWindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	static HHOOK s_hhook;
	static CWGFTestUIWindow *s_pHookWindow;
	static LRESULT CALLBACK WndProcHookFn( int nCode, WPARAM wParam, LPARAM lParam );
};


//////////////////////////////////////////////////////////////////////////
// CWGFTestUIDialog
//////////////////////////////////////////////////////////////////////////
class CWGFTestUIDialog : public CWGFTestUIWindow
{
public:
	virtual bool Create( HINSTANCE hInstance, LPCTSTR lpTemplate, CWGFTestUIWindow *pParent );
	virtual bool CreateParam( HINSTANCE hInstance, LPCTSTR lpTemplate, CWGFTestUIWindow *pParent, LPARAM lParam );
	
	virtual INT_PTR DoModal( HINSTANCE hInstance, LPCTSTR lpTemplate, CWGFTestUIWindow *pParent ); // BUGBUG - TODO
	virtual INT_PTR DoModalParam( HINSTANCE hInstance, LPCTSTR lpTemplate, CWGFTestUIWindow *pParent, LPARAM lParam ); // BUGBUG - TODO
	
	virtual INT_PTR DialogProc( UINT uMsg, WPARAM wParam, LPARAM lParam ) { return false; }
	

	//
	// Wrapper methods.
	//
	BOOL EndDialog( INT_PTR nResult ) { return ::EndDialog( m_hWnd, nResult ); }
	
	HWND GetDlgItem( int nIDDlgItem ) { return ::GetDlgItem( m_hWnd, nIDDlgItem ); }
	UINT GetDlgItemInt( int nIDDlgItem, BOOL *lpTranslated, BOOL bSigned ) { return ::GetDlgItemInt( m_hWnd, nIDDlgItem, lpTranslated, bSigned ); }
	UINT GetDlgItemText( int nIDDlgItem, LPTSTR lpString, int nMaxCount ) { return ::GetDlgItemText( m_hWnd, nIDDlgItem, lpString, nMaxCount ); }
	BOOL SetDlgItemInt( int nIDDlgItem, UINT uValue, BOOL bSigned ) { return ::SetDlgItemInt( m_hWnd, nIDDlgItem, uValue, bSigned ); }
	BOOL SetDlgItemText( int nIDDlgItem, LPCTSTR lpString ) { return ::SetDlgItemText( m_hWnd, nIDDlgItem, lpString ); }
	LRESULT SendDlgItemMessage( int nIDDlgItem, UINT uMsg, WPARAM wParam, LPARAM lParam ) { return ::SendDlgItemMessage( m_hWnd, nIDDlgItem, uMsg, wParam, lParam ); }

	BOOL IsDialogMessage( LPMSG lpMsg ) { return ::IsDialogMessage( m_hWnd, lpMsg ); }
	
	BOOL MapDialogRect( LPRECT lprc ) { return ::MapDialogRect( m_hWnd, lprc ); }

private:
	static INT_PTR CALLBACK CWGFTestUIDialog::ProxyDialogProc( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
};



//////////////////////////////////////////////////////////////////////////
// CWGFTestUIToolbar 
//////////////////////////////////////////////////////////////////////////
class CWGFTestUIToolbar : public CWGFTestUIWindow
{
public:
	bool Create( UINT uID, CWGFTestUIWindow *pParent, DWORD dwTBStyle );
	bool CreateSimpleBitmapButtons( UINT uID, CWGFTestUIWindow *pParent, int *pButtonIDs, DWORD dwTBStyle = 0 );
	
	//
	// Wrapper Methods
	//
	int AddBitmap( int nButtons, const TBADDBITMAP *pAddBitmap ) { return (int) SendMessage( TB_ADDBITMAP, (WPARAM) nButtons, (LPARAM) pAddBitmap ); }
	bool AddButtons( UINT nButtons, const TBBUTTON *pButtons ) { return 0 != SendMessage( TB_ADDBUTTONS, (WPARAM) nButtons, (LPARAM) pButtons ); }
	int AddString( HINSTANCE hInstance, UINT nIDString ) { return (int) SendMessage( TB_ADDSTRING, (WPARAM) hInstance, (LPARAM) nIDString ); }
	int AddString( LPCTSTR szString ) { return (int) SendMessage( TB_ADDSTRING, (WPARAM) 0, (LPARAM) szString ); }
	int AddStrings( LPCTSTR szString ) { return (int) SendMessage( TB_ADDSTRING, 0, (LPARAM) szString ); }
	bool AutoSize() { return 0 != SendMessage( TB_AUTOSIZE, 0, 0 ); }
	int ButtonCount() { return (int) SendMessage( TB_BUTTONCOUNT, 0, 0 ); }
	int CommandToIndex( int nIDButton ) { return (int) SendMessage( TB_COMMANDTOINDEX, (WPARAM) nIDButton, 0 ); }
	bool Customize() { return 0 != SendMessage( TB_CUSTOMIZE, 0, 0 ); }
	int HitTest( LPPOINT lpPoint ) { return (int) SendMessage( TB_HITTEST, 0, (LPARAM) lpPoint ); }
	bool InsertButton( int nButtonIndex, LPTBBUTTON lpButton ) { return 0 != SendMessage( TB_INSERTBUTTON, (WPARAM) nButtonIndex, (LPARAM) lpButton); }
	bool InsertMarkHitTest( LPPOINT lpPoint, LPTBINSERTMARK lpMark ) { return 0 != SendMessage( TB_INSERTMARKHITTEST, (WPARAM) lpPoint, (LPARAM) lpMark ); }
	bool IsButtonChecked( int nIDButton ) { return 0 != SendMessage( TB_ISBUTTONCHECKED, (WPARAM) nIDButton, 0 ); }
	bool IsButtonEnabled( int nIDButton ) { return 0 != SendMessage( TB_ISBUTTONENABLED, (WPARAM) nIDButton, 0 ); }
	bool IsButtonHidden( int nIDButton ) { return 0 != SendMessage( TB_ISBUTTONHIDDEN, (WPARAM) nIDButton, 0 ); }
	bool IsButtonHighlighted( int nIDButton ) { return 0 != SendMessage( TB_ISBUTTONHIGHLIGHTED, (WPARAM) nIDButton, 0 ); }
	bool IsButtonIndeterminate( int nIDButton ) { return 0 != SendMessage( TB_ISBUTTONINDETERMINATE, (WPARAM) nIDButton, 0 ); }
	bool IsButtonPressed( int nIDButton ) { return 0 != SendMessage( TB_ISBUTTONPRESSED, (WPARAM) nIDButton, 0 ); }
	int LoadSystemImages( int iBitmapID ) { return (int) SendMessage( TB_LOADIMAGES, (WPARAM) iBitmapID, (LPARAM) HINST_COMMCTRL ); }
	bool MapAccelerator( TCHAR csAccel, UINT *pIDButton ) { return 0 != SendMessage( TB_MAPACCELERATOR, (WPARAM) csAccel, (LPARAM) pIDButton ); }
	bool ChangeBitmap( int nIDButton, int nImageListIndex ) { return 0 != SendMessage( TB_CHANGEBITMAP, (WPARAM) nIDButton, (LPARAM) nImageListIndex ); }
	bool CheckButton( int nIDButton, bool bCheck ) { return 0 != SendMessage( TB_CHECKBUTTON, (WPARAM) nIDButton, (LPARAM) bCheck ); }
	bool DeleteButton( int nButtonIndex ) { return 0 != SendMessage( TB_DELETEBUTTON, (WPARAM) nButtonIndex, 0 ); }
	bool EnableButton( int nIDButton, bool bEnable ) { return 0 != SendMessage( TB_ENABLEBUTTON, (WPARAM) nIDButton, (LPARAM) bEnable ); }
	bool HideButton( int nIDButton, bool bHide ) { return 0 != SendMessage( TB_HIDEBUTTON, (WPARAM) nIDButton, (LPARAM) bHide ); }
	bool MarkButton( int nIDButton, bool bMark ) { return 0 != SendMessage( TB_MARKBUTTON, (WPARAM) nIDButton, (LPARAM) bMark ); }
	bool MoveButton( int nButtonIndexOld, int nButtonIndexNew ) { return 0 != SendMessage( TB_MOVEBUTTON, (WPARAM) nButtonIndexOld, (LPARAM) nButtonIndexNew ); }
	bool PressButton( int nIDButton, bool bPress ) { return 0 != SendMessage( TB_PRESSBUTTON, (WPARAM) nIDButton, (LPARAM) bPress ); }
	bool Indeterminate( int nIDButton, bool bIndeterminate ) { return 0 != SendMessage( TB_INDETERMINATE, (WPARAM) nIDButton, (LPARAM) bIndeterminate ); }
	bool ReplaceBitmap( LPTBREPLACEBITMAP ptbrb ) { return 0 != SendMessage( TB_REPLACEBITMAP, 0, (LPARAM) ptbrb ); }
	void SaveRestore( bool bSave, TBSAVEPARAMS *ptbsp ) { SendMessage( TB_SAVERESTORE, (WPARAM) bSave, (LPARAM) ptbsp ); }
	bool SetAnchorHighlight( bool bHighlight ) { return 0 != SendMessage( TB_SETANCHORHIGHLIGHT, (WPARAM) bHighlight, 0 ); }
	bool SetBitmapSize( int width, int height ) { return 0 != SendMessage( TB_SETBITMAPSIZE, 0, (LPARAM) MAKELONG( width, height ) ); }
	bool SetButtonInfo( int nIDButton, LPTBBUTTONINFO lptbbi ) { return 0 != SendMessage( TB_SETBUTTONINFO, (WPARAM) nIDButton, (LPARAM) lptbbi ); }
	bool SetButtonSize( int width, int height ) { return 0 != SendMessage( TB_SETBUTTONSIZE, 0, (LPARAM) MAKELONG( width, height ) ); }
	bool SetButtonWidth( int nMinWidth, int nMaxWidth ) { return 0 != SendMessage( TB_SETBUTTONWIDTH, 0, (LPARAM) MAKELONG( nMinWidth, nMaxWidth ) ); }
	bool SetCmdID( int nButtonIndex, int nIDButton ) { return 0 != SendMessage( TB_SETCMDID, (WPARAM) nButtonIndex, (LPARAM) nIDButton ); }
	void SetColorScheme( LPCOLORSCHEME lpcs ) { SendMessage( TB_SETCOLORSCHEME, 0, (LPARAM) lpcs ); }
	HIMAGELIST SetDisabledImageList( HIMAGELIST hNewImageList ) { return (HIMAGELIST) SendMessage( TB_SETDISABLEDIMAGELIST, 0, (LPARAM) hNewImageList ); }
	DWORD SetDrawTextFlags( DWORD dwMask, DWORD dwDTFlags ) { return (DWORD) SendMessage( TB_SETDRAWTEXTFLAGS, (WPARAM) dwMask, (LPARAM) dwDTFlags ); }
	DWORD SetExtendedStyle( DWORD dwExStyle ) { return (DWORD) SendMessage( TB_SETEXTENDEDSTYLE, 0, (LPARAM) dwExStyle ); }
	HIMAGELIST SetHotImageList( HIMAGELIST hNewImageList ) { return (HIMAGELIST) SendMessage( TB_SETHOTIMAGELIST, 0, (LPARAM) hNewImageList ); }
	int SetHotItem( int iHot ) { return (int) SendMessage( TB_SETHOTITEM, (WPARAM) iHot, 0 ); }
	HIMAGELIST SetImageList( HIMAGELIST hNewImageList, int iImageID ) { return (HIMAGELIST) SendMessage( TB_SETIMAGELIST, (WPARAM) iImageID, (LPARAM) hNewImageList ); }
	bool SetIndent( int iIndent ) { return 0 != SendMessage( TB_SETINDENT, (WPARAM) iIndent, 0 ); }
	bool SetInsertMark( LPTBINSERTMARK lptbim ) { return 0 != SendMessage( TB_SETINSERTMARK, 0, (LPARAM) lptbim ); }
	COLORREF SetInsertMarkColor( COLORREF clrInsertMark ) { return (COLORREF) SendMessage( TB_SETINSERTMARKCOLOR, 0, (LPARAM) clrInsertMark ); }
	bool SetMaxTextRows( int iMaxRows ) { return 0 != SendMessage( TB_SETMAXTEXTROWS, (WPARAM) iMaxRows, 0 ); }
	DWORD SetPadding( LPARAM Padding ) { return (DWORD) SendMessage( TB_SETPADDING, 0, Padding ); }
	void SetRows( int nRows, bool bLarger, LPRECT lprc )	 { SendMessage( TB_SETROWS, MAKEWPARAM( nRows, bLarger ), (LPARAM) lprc ); }
	bool SetState( int nIDButton, DWORD dwState ) { return 0 != SendMessage( TB_SETSTATE, (WPARAM) nIDButton, (LPARAM) MAKELONG( dwState, 0 ) ); }
	void SetStyle( DWORD dwStyle ) { SendMessage( TB_SETSTYLE, 0, (LPARAM) dwStyle ); }
	void SetToolTips( HWND hWndToolTips ) { SendMessage( TB_SETTOOLTIPS, (WPARAM) hWndToolTips, 0 ); }
	bool SetUnicodeFormat( bool bUnicode ) { return 0 != SendMessage( TB_SETUNICODEFORMAT, (WPARAM) bUnicode, 0 ); }
	bool GetAnchorHighlight() { return 0 != SendMessage( TB_GETANCHORHIGHLIGHT, 0, 0 ); }
	int GetBitmap( int nIDButton ) { return (int) SendMessage( TB_GETBITMAP, (WPARAM) nIDButton, 0 ); }
	DWORD GetBitmapFlags() { return (DWORD) SendMessage( TB_GETBITMAPFLAGS, 0, 0 ); }
	bool GetButton( int nButtonIndex, TBBUTTON *pButton ) { return 0 != SendMessage( TB_GETBUTTON, (WPARAM) nButtonIndex, (LPARAM) pButton ); }
	bool GetButtonInfo( int nIDButton, TBBUTTONINFO *pButtonInfo ) { return 0 != SendMessage( TB_GETBUTTONINFO, (WPARAM) nIDButton, (LPARAM) pButtonInfo ); }
	DWORD GetButtonSize() { return (DWORD) SendMessage( TB_GETBUTTONSIZE, 0, 0 ); }
	int GetButtonText( int nIDButton, LPTSTR szBuffer ) { return (int) SendMessage( TB_GETBUTTONTEXT, (WPARAM) nIDButton, (LPARAM) szBuffer ); }
	bool GetColorScheme( LPCOLORSCHEME pScheme ) { return 0 != SendMessage( TB_GETCOLORSCHEME, 0, (LPARAM) pScheme ); }
	HIMAGELIST GetDisabledImageList() { return (HIMAGELIST) SendMessage( TB_GETDISABLEDIMAGELIST, 0, 0 ); }
	DWORD GetExtendedStyle() { return (DWORD) SendMessage( TB_GETEXTENDEDSTYLE, 0, 0 ); }
	HIMAGELIST GetHotImageList() { return (HIMAGELIST) SendMessage( TB_GETHOTIMAGELIST, 0, 0 ); }
	int GetHotItem() { return (int) SendMessage( TB_GETHOTITEM, 0, 0 ); }
	HIMAGELIST GetImageList() { return (HIMAGELIST) SendMessage( TB_GETIMAGELIST, 0, 0 ); }
	void GetInsertMark( LPTBINSERTMARK lptbim ) { SendMessage( TB_GETINSERTMARK, 0, (LPARAM) lptbim ); }
	COLORREF GetInsertMarkColor() { return (COLORREF) SendMessage( TB_GETINSERTMARKCOLOR, 0, 0 ); }
	bool GetItemRect( int nButtonIndex, LPRECT lprc ) { return 0 != SendMessage( TB_GETITEMRECT, (WPARAM) nButtonIndex, (LPARAM) lprc ); }
	bool GetMaxSize( LPSIZE lpsize ) { return 0 != SendMessage( TB_GETMAXSIZE, 0, (LPARAM) lpsize ); }
	DWORD GetPadding() { return (DWORD) SendMessage( TB_GETPADDING, 0, 0 ); }
	bool GetRect( int nIDButton, LPRECT lprc ) { return 0 != SendMessage( TB_GETRECT, (WPARAM) nIDButton, (LPARAM) lprc ); }
	UINT GetRows() { return (UINT) SendMessage( TB_GETROWS, 0, 0 ); }
	int GetState( int nIDButton ) { return (int) SendMessage( TB_GETSTATE, (WPARAM) nIDButton, 0 ); }
	int GetString( int iString, size_t nLength, LPTSTR pszString ) { return (int) SendMessage( TB_GETSTRING, MAKEWPARAM( nLength, iString ), (LPARAM) pszString ); }
	DWORD GetStyle() { return (DWORD) SendMessage( TB_GETSTYLE, 0, 0 ); }
	int GetTextRows() { return (int) SendMessage( TB_GETROWS, 0, 0 ); }
	HWND GetToolTips() { return (HWND) SendMessage( TB_GETTOOLTIPS, 0, 0 ); }
	bool GetUnicodeFormat() { return 0 != SendMessage( TB_GETUNICODEFORMAT, 0, 0 ); }
};

//////////////////////////////////////////////////////////////////////////
// CWGFTestUIStatusBar
//////////////////////////////////////////////////////////////////////////
class CWGFTestUIStatusBar : public CWGFTestUIWindow
{
public:
	bool Create( UINT uID, CWGFTestUIWindow *pParent, DWORD dwStyle, LPCTSTR lpszText );
	
	bool GetBorders( LPINT pBorderElements ) { return 0 != SendMessage( SB_GETBORDERS, 0, (LPARAM) pBorderElements ); }
	HICON GetIcon( int iPart ) { return (HICON) SendMessage( SB_GETICON, iPart, 0 ); }
	int GetParts( int nParts, int *pCoordArray ) { return (int) SendMessage( SB_GETPARTS, nParts, (LPARAM) pCoordArray ); }
	bool GetRect( int nPart, LPRECT lprc ) { return 0 != SendMessage( SB_GETRECT, nPart, (LPARAM) lprc ); }
	DWORD GetText( int nPart, LPTSTR szText ) { return (DWORD) SendMessage( SB_GETTEXT, nPart, (LPARAM) szText ); }
	DWORD GetTextLength( int nPart ) { return (DWORD) SendMessage( SB_GETTEXTLENGTH, nPart, 0 ); }
	void GetTipText( int nPart, int nSize, LPCTSTR lpszToolTip ) { SendMessage( SB_GETTIPTEXT, MAKEWPARAM( nPart, nSize ), (LPARAM) lpszToolTip ); }
	bool IsSimple() { return 0 != SendMessage( SB_ISSIMPLE, 0, 0 ); }
	
	COLORREF SetBkColor( COLORREF cr ) { return (COLORREF) SendMessage( SB_SETBKCOLOR, 0, (LPARAM) cr ); }
	bool SetIcon( int nPart, HICON hIcon ) { return 0 != SendMessage( SB_SETICON, nPart, (LPARAM) hIcon ); }
	void SetMinHeight( int nMinHeight ) { SendMessage( SB_SETMINHEIGHT, nMinHeight, 0 ); }
	bool SetParts( int nParts, int *pCoordArray ) { return 0 != SendMessage( SB_SETPARTS, nParts, (LPARAM) pCoordArray ); }
	bool SetText( UINT uPart, LPCTSTR szText ) { return 0 != SendMessage( SB_SETTEXT, uPart, (LPARAM) szText ); }
	void SetTipText( int nPart, LPCTSTR szToolTip ) { SendMessage( SB_SETTIPTEXT, nPart, (LPARAM) szToolTip ); }
	void Simple( bool bSimple ) { SendMessage( SB_SIMPLE, (WPARAM) bSimple, 0 ); }	
};


//////////////////////////////////////////////////////////////////////////
// CWGFTestUITabControl
//////////////////////////////////////////////////////////////////////////
class CWGFTestUITabControl : public CWGFTestUIWindow
{
public:
	bool Create( UINT uID, CWGFTestUIWindow *pParent, DWORD dwStyle , int x, int y, int width, int height );
	
	LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	
	//
	// Wrapper Methods
	//
	void AdjustRect( bool bLarger, LPRECT lprc ) { SendMessage( TCM_ADJUSTRECT, bLarger, (LPARAM) lprc ); }
	bool DeleteAllItems() { return 0 != SendMessage( TCM_DELETEALLITEMS, 0, 0 ); }
	bool DeleteItem( int iItem ) { return 0 != SendMessage( TCM_DELETEITEM, iItem, 0 ); }
	void DeselectAll( DWORD dwExcludeFocus ) { SendMessage( TCM_DESELECTALL, dwExcludeFocus, 0 ); }
	int GetCurFocus() { return (int) SendMessage( TCM_GETCURFOCUS, 0, 0 ); }
	int GetCurSel() { return (int) SendMessage( TCM_GETCURSEL, 0, 0 ); }
	DWORD GetExtendedStyle() { return (DWORD) SendMessage( TCM_GETEXTENDEDSTYLE, 0, 0 ); }
	HIMAGELIST GetImageList() { return (HIMAGELIST) SendMessage( TCM_GETIMAGELIST, 0, 0 ); }
	bool GetItem( int iItem, TCITEM *lptci ) { return 0 != SendMessage( TCM_GETITEM, iItem, (LPARAM) lptci ); }
	int GetItemCount() { return (int) SendMessage( TCM_GETITEMCOUNT, 0, 0 ); }
	bool GetItemRect( int iItem, LPRECT lprc ) { return 0 != SendMessage( TCM_GETITEMRECT, iItem, (LPARAM) lprc ); }
	int GetRowCount() { return (int) SendMessage( TCM_GETROWCOUNT, 0, 0 ); }
	HWND GetToolTips() { return (HWND) SendMessage( TCM_GETTOOLTIPS, 0, 0 ); }
	bool HighlightItem( int iItem, bool bHighlight ) { return 0 != SendMessage( TCM_HIGHLIGHTITEM, iItem, (LPARAM) bHighlight ); }
	int HitTest( LPTCHITTESTINFO pinfo ) { return (int) SendMessage( TCM_HITTEST, 0, (LPARAM) pinfo ); }
	int InsertItem( int iItem, const TCITEM *pitem ) { return (int) SendMessage( TCM_INSERTITEM, iItem, (LPARAM) pitem ); }
	void RemoveImage( int iImage ) { SendMessage( TCM_REMOVEIMAGE, iImage, 0 ); }
	void SetCurFocus( int iItem ) { SendMessage( TCM_SETCURFOCUS, iItem, 0 ); }
	int SetCurSel( int iItem ) { return (int) SendMessage( TCM_SETCURSEL, iItem, 0 ); }
	DWORD SetExtendedStyle( DWORD dwExMask, DWORD dwExStyle ) { return (DWORD) SendMessage( TCM_SETEXTENDEDSTYLE, dwExMask, (LPARAM) dwExStyle ); }
	HIMAGELIST SetImageList( HIMAGELIST hil ) { return (HIMAGELIST) SendMessage( TCM_SETIMAGELIST, 0, (LPARAM) hil ); }
	bool SetItem( int iItem, const TCITEM * pitem ) { return 0 != SendMessage( TCM_SETITEM, iItem, (LPARAM) pitem ); }
	bool SetItemExtra( int cb ) { return 0 != SendMessage( TCM_SETITEMEXTRA, cb, 0 ); }
	DWORD SetItemSize( int cx, int cy ) { return (DWORD) SendMessage( TCM_SETITEMSIZE, 0, (LPARAM) MAKELPARAM( cx, cy ) ); }
	int SetMinTabWidth( int cx ) { return (int) SendMessage( TCM_SETMINTABWIDTH, 0, (LPARAM) cx ); }
	void SetPadding( int cx, int cy ) { SendMessage( TCM_SETPADDING, 0, MAKELPARAM( cx, cy ) ); }
	void SetToolTips( HWND hWndTT ) { SendMessage( TCM_SETTOOLTIPS, (WPARAM) hWndTT, 0 ); }
	
protected:
	HFONT m_hFont;
};

//////////////////////////////////////////////////////////////////////////
// CWGFTestUIRichEditControl
//////////////////////////////////////////////////////////////////////////
class CWGFTestUIRichEditControl : public CWGFTestUIWindow
{
public:
	bool Create( UINT uID, CWGFTestUIWindow *pParent, DWORD dwStyle , int x, int y, int width, int height );
	bool Create( HWND hWnd );
	
	LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool EnableURLDetection( BOOL bYes ) { return FALSE == SendMessage( EM_AUTOURLDETECT, bYes, NULL ); }
	bool GetURLDetection( ) { return TRUE == SendMessage( EM_GETAUTOURLDETECT, NULL, NULL ); }

	LRESULT SetSelection( CHARRANGE * pRange ) { return SendMessage( EM_EXSETSEL, NULL, (LPARAM) pRange ); }
	void GetSelection( CHARRANGE * pRange ) { SendMessage( EM_EXGETSEL, NULL, (LPARAM) pRange ); }
	LRESULT GetSelText( void * pBuf ) { return SendMessage( EM_GETSELTEXT, NULL, (LPARAM) pBuf ); }
	void HideSelection( BOOL bHide ) { SendMessage( EM_HIDESELECTION, bHide, NULL ); }
	LRESULT GetSelectionType( ) { return SendMessage( EM_SELECTIONTYPE, NULL, NULL ); }

	LRESULT GetText( GETTEXTEX * pGetText, char * szBuf ) { return SendMessage( EM_GETTEXTEX, (WPARAM) pGetText, (LPARAM) szBuf ); }
	LRESULT SetText( SETTEXTEX * pSetText, const char * szText ) { return SendMessage( EM_SETTEXTEX, (WPARAM) pSetText, (LPARAM) szText ); }
	LRESULT GetTextLength( GETTEXTLENGTHEX * pLength ) { return SendMessage( EM_GETTEXTLENGTHEX, (WPARAM) pLength, NULL ); }
	LRESULT GetTextMode( ) { return SendMessage( EM_GETTEXTMODE, NULL, NULL ); }
	bool SetTextMode( DWORD dwMode ) { return FALSE == SendMessage( EM_SETTEXTMODE, dwMode, NULL ); }

	LRESULT GetTextRange( TEXTRANGE * pRange ) { return SendMessage( EM_GETTEXTRANGE, NULL, (LPARAM) pRange ); }	

	void LimitText( long Limit ) { SendMessage( EM_EXLIMITTEXT, NULL, Limit ); }
	LRESULT LineFromCharIndex( long lIndex ) { return SendMessage( EM_EXLINEFROMCHAR, NULL, lIndex ); }
	
	LRESULT FindText( WPARAM Params, FINDTEXTEX * pFindText ) { return SendMessage( EM_FINDTEXTEX, Params, (LPARAM) pFindText ); }
	LRESULT FindTextW( WPARAM Params, FINDTEXTEXW * pFindText ) { return SendMessage( EM_FINDTEXTEXW, Params, (LPARAM) pFindText ); }
	LRESULT FindWordBreak( WPARAM Params, unsigned long Start ) { return SendMessage( EM_FINDWORDBREAK, Params, Start ); }

	LRESULT GetCharFormat( WPARAM Params, CHARFORMAT * pFormat ) { return SendMessage( EM_GETCHARFORMAT, Params, (LPARAM) pFormat ); }
	bool SetCharFormat( WPARAM Params, CHARFORMAT * pFormat ) { return FALSE != SendMessage( EM_SETCHARFORMAT, Params, (LPARAM) pFormat ); }
	bool SetFontSize( int iSizeChange ) { return TRUE == SendMessage( EM_SETFONTSIZE, iSizeChange, NULL ); }

	LRESULT GetEditStyle( ) { return SendMessage( EM_GETEDITSTYLE, NULL, NULL ); }
	LRESULT SetEditStyle( DWORD dwValues, DWORD dwMask ) { return SendMessage( EM_SETEDITSTYLE, dwValues, dwMask ); }

	LRESULT GetOptions( ) { return SendMessage( EM_GETOPTIONS, NULL, NULL ); }
	LRESULT SetOptions( DWORD dwOp, DWORD dwValues ) { return SendMessage( EM_SETOPTIONS, dwOp, dwValues ); }
	LRESULT SetBackgroundColor( BOOL bUseSystem, COLORREF Color ) { return SendMessage( EM_SETBKGNDCOLOR, bUseSystem, Color ); }

	void SetBidirectionalOptions( BIDIOPTIONS * pOptions ) { SendMessage( EM_SETBIDIOPTIONS, NULL, (LPARAM) pOptions ); }
	void GetBidirectionalOptions( BIDIOPTIONS * pOptions ) { SendMessage( EM_GETBIDIOPTIONS, NULL, (LPARAM) pOptions ); }

	void GetScrollPos( POINT * pPoint ) { SendMessage( EM_GETSCROLLPOS, NULL, (LPARAM) pPoint ); }
	void SetScrollPosition( POINT * pPoint ) { SendMessage( EM_SETSCROLLPOS, NULL, (LPARAM) pPoint ); }
	void ShowScrollBar( BOOL bSetVertical, BOOL bShow ) { SendMessage( EM_SHOWSCROLLBAR, bSetVertical, bShow ); }

//Messages that are being ignored for now
	//EM_CANPASTE
	//EM_CANREDO
	//EM_DISPLAYBAND
	//EM_FORMATRANGE
	//EM_GETCTFMODEBIAS
	//EM_GETCTFOPENSTATUS
	//EM_GETEVENTMASK
	//EM_GETHYPHENATEINFO
	//EM_GETIMECOLOR
	//EM_GETIMECOMPMODE
	//EM_GETIMECOMPTEXT
	//EM_GETIMEMODEBIAS
	//EM_GETIMEOPTIONS
	//EM_GETIMEPROPERTY
	//EM_GETLANGOPTIONS
	//EM_GETOLEINTERFACE
	//EM_GETPAGEROTATE
	//EM_GETPARAFORMAT
	//EM_GETPUNCTUATION
	//EM_GETREDONAME
	//EM_GETTYPOGRAPHYOPTIONS
	//EM_GETUNDONAME
	//EM_GETWORDBREAKPROCEX
	//EM_GETWORDWRAPMODE
	//EM_GETZOOM
	//EM_ISIME
	//EM_PASTESPECIAL
	//EM_RECONVERSION
	//EM_REDO
	//EM_REQUESTRESIZE
	//EM_SETCTFMODEBIAS
	//EM_SETCTFOPENSTATUS
	//EM_SETEVENTMASK
	//EM_SETHYPHENATEINFO
	//EM_SETIMECOLOR
	//EM_SETIMEMODEBIAS
	//EM_SETIMEOPTIONS
	//EM_SETLANGOPTIONS
	//EM_SETOLECALLBACK
	//EM_SETPAGEROTATE
	//EM_SETPALETTE
	//EM_SETPARAFORMAT
	//EM_SETPUNCTUATION
	//EM_SETTARGETDEVICE
	//EM_SETTYPOGRAPHYOPTIONS
	//EM_SETUNDOLIMIT
	//EM_SETWORDBREAKPROCEX
	//EM_SETWORDWRAPMODE
	//EM_SETZOOM
	//EM_STOPGROUPTYPING
};

//////////////////////////////////////////////////////////////////////////
// CWGFTestUIGroupTreeControl
//////////////////////////////////////////////////////////////////////////
class CWGFTestUIGroupTreeControl : public CWGFTestUIWindow
{
public:	
	CWGFTestUIGroupTreeControl();
	bool Create( UINT uID, CWGFTestUIWindow *pParent );
	bool Create( HWND hWnd );

	LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	
	const CTestGroupPath &PathFromCurSel();
	const CTestGroupPath &PathFromItem( HTREEITEM hItem );
	virtual void Initialize( );
	
private:
	void AddBranch( HTREEITEM hParent, const CTestGroupNode *pNode );	
	
protected:	
	void AddGroups( HTREEITEM hParent );
	HIMAGELIST m_hImageList;
};