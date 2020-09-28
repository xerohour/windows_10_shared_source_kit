#ifndef _CWINDOW_H_
#define _CWINDOW_H_

#include <windows.h>



#define MAX_CHILDREN 16

class CWindow
{
    public:
		
        HWND            	m_hWindow;
        WNDCLASS			m_WC;
		CWindow				*m_pParent;
		UINT				m_nChildren;						//  Number of child windows
		CWindow				*m_pChildren[MAX_CHILDREN];			//  List of child windows
		HFONT				m_hFont;							//  Window font - used for all text
		DWORD				m_dwStyle, m_dwStyleEx;				//  Current window styles
        UINT				m_nWidth, m_nHeight;				//  Width/Height of the window
		float				m_fWidth, m_fHeight;				//  Percent of parent's client window that this should cover
		bool				m_bFixed;							//  Fixed window size
		UINT_PTR			m_nID;								//  Window Identifier

        void				Init();
		virtual	void		UpdateChildStats();
        static LRESULT WINAPI StdWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

    public:

        CWindow();
        CWindow(UINT_PTR nID, HWND hWnd);
		virtual ~CWindow();

		virtual inline void SetDimensions(float fX, float fY){ m_fWidth = fX; m_fHeight = fY; }
		virtual inline void SetFixed(bool bFixed){ m_bFixed = bFixed; }
        virtual inline bool IsFixed(){ return m_bFixed; }

        virtual bool        InitializeWindow(DWORD dwStyleEx,LPTSTR sClassName, LPTSTR sWindowName,HMENU hMenu,HICON hIcon,DWORD dwStyle, int nPosX, int nPosY, int nWidth, int nHeight, HWND hParent);
        virtual void        Close(void);
        virtual void		SetStyle(DWORD dwStyle);
        virtual void		SetStyleEx(DWORD dwStyleEx);
        virtual void		Move(UINT nX, UINT nY, UINT nWidth, UINT nHeight);
		virtual void		Move(UINT nX, UINT nY);
        virtual void        Resize(UINT nWidth, UINT nHeight);
		virtual void  		Update(void);
        virtual LRESULT     WindowProc(UINT,WPARAM,LPARAM);
        CWindow*			GetParent(void);
        virtual CWindow*    GetWindow(UINT_PTR nID);
		static  CWindow*	GetWindowPointer(HWND hWnd);
        virtual LRESULT     OnCommand(WPARAM wParam, LPARAM lParam);
		virtual void    	OnNotify(WPARAM wParam, LPARAM lParam);
		virtual LRESULT		OnKeydown(WPARAM wParam, LPARAM lParam);
		virtual LRESULT		OnKeyup(WPARAM wParam, LPARAM lParam);
        virtual void        OnPaint(WPARAM wParam, LPARAM lParam);
        virtual void        OnSize(WPARAM wParam, LPARAM lParam);
		virtual LRESULT		OnMouse(UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual LRESULT		OnSetCursor(WPARAM wParam, LPARAM lParam);
        
		virtual bool 		AddChild(TCHAR *pcsName, CWindow *pChild);
		virtual void 		SetVisible(bool bVisible);
		virtual void		SetEnable(bool bEnable);
		static  HFONT 		MakeFont(HDC hDC, int nPoint, DWORD dwWeight, DWORD dwItalic, DWORD dwUnderline);
		virtual void		CalculateSize();
        virtual void		GetBorders(UINT &nWidth, UINT &nHeight);
};

class CLogWindow: public CWindow
{
    int                 m_nLogLevel;    
    UINT                m_nLogLength;
	UINT				m_nGrowSize;
    UINT                m_nLogAlloc;
	TCHAR				*m_sLog, *m_sRTFHeader;
	TCHAR				*m_sLineBuffer;
	UINT				m_nLineAlloc;

    virtual void 	Append(TCHAR cAppend, TCHAR **sOut, UINT &nLen, UINT &nAlloc);
    virtual void	Append(TCHAR* sAppend, TCHAR **sOut, UINT &nLen, UINT &nAlloc);
	virtual bool	AppendToLog(TCHAR *sEntry);
    virtual void	ConvertRTF(TCHAR *sIn, TCHAR **sOut, UINT &nAlloc);
	virtual void 	StripRTF(TCHAR *sString);

public:
    CLogWindow();
    virtual ~CLogWindow();

	inline  TCHAR* GetLog() { return m_sLog; }
    
    virtual bool    InitializeWindow(DWORD dwStyleEx,LPTSTR sClassName, LPTSTR sWindowName,HMENU hMenu,HICON hIcon,DWORD dwStyle, int nPosX, int nPosY, int nWidth, int nHeight, HWND hParent);
	virtual bool 	GetLogLine(UINT nLine, TCHAR *sBuffer, UINT nSize);
	virtual UINT	GetNumLines();
    virtual void  	Update(void);
    virtual void  	ClearEntries(void);
    virtual int   	GetLogLevel(void);
    virtual void  	SetLogLevel(int);
    virtual bool  	WriteToLog(const TCHAR *szFormat, ...);
    virtual bool  	WriteToLog(int nLevel, const TCHAR *szFormat, ...);
    virtual bool  	WriteStatus(const TCHAR *szKey, const TCHAR *szFormat,...);
    virtual bool  	WriteStatus(int nLevel, const TCHAR *szKey, const TCHAR *szFormat,...);
    virtual bool  	WriteToLogWhole(int nLevel, const TCHAR *szString);
    virtual bool  	WriteStatusWhole(int nLevel, const TCHAR *szKey, const TCHAR *szString);
};

class CTabWindow : public CWindow
{
	UINT		m_nSelectedTab;

public:
	CTabWindow();

    virtual bool 	AddChild(TCHAR *pcsName, CWindow *pChild);
    virtual void	SelectTab(UINT nIndex);
	virtual void	Resize(UINT nWidth, UINT nHeight);
    virtual void	Move(UINT nX, UINT nY);
    virtual void	Move(UINT nX, UINT nY, UINT nWidth, UINT nHeight);
    virtual void    OnNotify(WPARAM wParam, LPARAM lParam);

	inline CWindow*	GetSelectedTab() { return m_pChildren[m_nSelectedTab]; }
};

class CRebarWindow : public CWindow
{

public:
	CRebarWindow();
    virtual ~CRebarWindow();

    virtual bool 	AddBand(TCHAR *pcsText, DWORD dwStyle, int nWidth, HWND hChild, int dwLoc);
    virtual void    Close(void);
    virtual void	MoveWindow(int nX, int nY, UINT nWidth, UINT nHeight, bool bRepaint);
};

#endif

