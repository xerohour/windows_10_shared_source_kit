#ifndef _CD3DWINDOWFRAMEWORK_H_
#define _CD3DWINDOWFRAMEWORK_H_

#include "Framework.h"
#include "CImageCompare.h"
#include "TraceListenerThread.hpp"
#include "DXGLog.hpp"
#include "DWMController.hpp"


#if defined(_PREFAST_)
#pragma prefast(disable: 12109)
#pragma prefast(disable: 12110)
#pragma prefast(disable: 12111)
#pragma prefast(disable: 12112)
#endif

const UINT_PTR WND_DISPLAY      = 0x00000001;
const UINT_PTR WND_REBAR        = 0x00000002;
const UINT_PTR WND_TESTNUM      = 0x00000003;
const UINT_PTR WND_TOOLBAR      = 0x00000004;
const UINT_PTR WND_TAB          = 0x00000005;
const UINT_PTR WND_STATUS       = 0x00000006;
const UINT_PTR WND_LOG          = 0x00000007;
const UINT_PTR WND_MSG          = 0x00000008;
const UINT_PTR WND_STATUSBAR    = 0x00000009;
const UINT_PTR WND_HELP         = 0x00000010;

const DWORD EXITCLASS_STANDARD  = 0x00001000;
const DWORD EXITCLASS_STRESS    = 0x00002000;
const DWORD EXITCLASS_LOG       = 0x00004000;
const DWORD EXITCLASS_LOGDB     = 0x00080000;

const DWORD EXITCODE_SUCCESS    = 0x00000000;
const DWORD EXITCODE_ABORT      = 0x00000001;
const DWORD EXITCODE_SKIP       = 0x00000002;
const DWORD EXITCODE_FAIL       = 0x00000004;

enum DEVICETYPES
{
	HAL = 0,
	REF = 1,
	SW = 2,
	ENDDEVICE = 3
};

enum HYBRIDOVERRIDE
{
    NONE = 0,
    FORCEDISCRETE = 1,
    EMULATEHYBRID = 2,
    FORCEINTEGRATED = 3
};


typedef DWORD (WINAPI *PGETPROCESSIDOFTHREAD)
(
	HANDLE hThread
);


typedef DWORD (WINAPI *PGETPROCESSIMAGEFILENAMEW)
(
	HANDLE hProcess,
	LPWSTR pImageFileNameBuffer,
	DWORD nImageFileNameBuffer
);

typedef DWORD (WINAPI *PGETPROCESSIMAGEFILENAMEA)
(
	HANDLE hProcess,
	LPSTR pImageFileNameBuffer,
	DWORD nImageFileNameBuffer
);

#ifdef UNICODE
#define PGETPROCESSIMAGEFILENAME PGETPROCESSIMAGEFILENAMEW
#define GETPROCESSIMAGEFILENAME "GetProcessImageFileNameW"
#else
#define PGETPROCESSIMAGEFILENAME PGETPROCESSIMAGEFILENAMEA
#define GETPROCESSIMAGEFILENAME "GetProcessImageFileNameA"
#endif


typedef struct _DISPLAYMODELIST
{
	DISPLAYMODE			Mode;
	_DISPLAYMODELIST   *pNext;
}DISPLAYMODELIST, *PDISPLAYMODELIST;


class CD3DWindowFramework: public CWindow, public CTestManager
{
	friend class CD3DTest;
	friend class CManualTest;
	friend class CAPITest;
    
protected:
	float				m_fRunPerc; 			// Global test run percentage
	UINT				m_uLogLevel;
	bool				m_bMuteLog;
	bool				m_bCoverage;
	HINSTANCE			m_hCovDLL;
	DWORD				m_dwWindowWidth, m_dwWindowHeight;
	DWORD				m_dwWindowedStyle, m_dwFullscreenStyle;
	UINT				m_nCurrentAdapter;
	DWORD				m_dwCurrentDispWidth, m_dwCurrentDispHeight;
	bool				m_bCurrentDispWindowed;
	float				m_fGlobalPassRate;
	HACCEL				m_hAccel;
	HMENU				m_hMenu;
	HMODULE 			m_hRichEdit;
	HWND				m_hStatusWindow;
	SIZE				m_ClientSize;
	UINT				m_uExitCode;
	DXGLOG_INFO 		m_LogInfo;
	DXGLOG_DEVICEINFO	m_DeviceInfo;
	BYTE				m_bKey[256];
	bool				m_bExceptionHandling;
	bool				m_bActive;
	bool				m_bMaximizeDisplay;
	bool				m_bForceLog;
	bool				m_bDebug;
	time_t				m_tStartTime;
	bool				m_bLoseDeviceThenExit;
	UINT				m_uAdapterToLoseDevice;
	int					m_nAdapter;			   	// Adapter to run against
	UINT				m_nStressDuration;		// How long should we run stress for
	float				m_fMinDXVersion;		// Minimum version of DX requested
	float				m_fMaxDXVersion;		// Maximum version of DX requested	
	DEVICEMODE		   *m_pDeviceMode;			// List of devices to run against
	int					m_nDeviceMode;			// Device count of list
	DEVICEMODE		   *m_pCurrentDevMode;		// Ptr to the current device
	DWORD				m_dwCmdDevTypes;		// Cmdline overides for DEV type
	DWORD				m_dwCmdBehaviors;		// Cmdline overides for Behaviors
	DWORD				m_dwExcludeDevTypes; 	// Cmdline overides for DEV type exclusion(!REF)
	DWORD				m_dwExcludeBehaviors;   // Cmdline overides for DEV type exclusion(!PURE)
	bool				m_bShowCompareWindow;	// Is the compare window being shown;
	HFONT				m_hSmallFont;
	HFONT				m_hLargeFont;
	int					m_nAdapters;			// Number of adapters on this machine
	DEVMODE			   *m_pOrigModes;
	DISPLAYMODE		   *m_pCurModes;
	DISPLAYMODELIST	   *m_pDispModes;			// Display modes(windowed) that we want to run against
	PGETPROCESSIDOFTHREAD m_pGetProcessIdOfThread;

	HMODULE             m_hPSAPI;
	PGETPROCESSIMAGEFILENAME m_pGetProcessImageFileName;

	CTraceListenerThread * m_pTraceListenerThread;

	HDESK               m_hOriginalThreadDesktop;
	TCHAR               m_pOriginalThreadDesktopName[ MAX_PATH ];
	HDESK               m_hOriginalInputDesktop;
	TCHAR               m_pOriginalInputDesktopName[ MAX_PATH ];
	HDESK               m_hCustomDesktop;
	TCHAR               m_pCustomDesktopName[ MAX_PATH ];

	bool                m_bReenableDWM;
	HYBRIDOVERRIDE		m_uHybridOverride;

	virtual bool		LogDeviceInfo(CDevice* pDevice);
	virtual bool		OpenLogFile();
	virtual bool		CheckD3DVersion();
	virtual HRESULT 	InitializeTests();
	virtual void		ReleaseTests();
	virtual void		ParseCmdline();
	virtual void		LoadRegistrySettings();
	virtual void		ReleaseAll();
	virtual LRESULT 	WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam);
	virtual void		CommandLineHelp();
	virtual void		ApertureCorruptionDebugBreak();
	virtual bool		ExecuteTests();
	virtual bool		BeginTestExecution();
	virtual bool		EndTestExecution();
	virtual HRESULT 	NextTest(DEVICEMODE *pMode);
	virtual HRESULT 	LoadNextTest(DEVICEMODE *pMode);
	virtual DEVICEMODE* GetNextDevice();
	virtual HRESULT 	LoadDevice(__in DEVICEMODE *pMode, __out CDevice **pSrcDev);
	virtual bool		AddDeviceMode(DISPLAYMODE *pDispMode, int nAdapter, float fVersion, DWORD dwDevType, DWORD dwBehaviors, DWORD dwPSGPRegChange);
	virtual bool		ValidateDeviceMode(int nAdapter, float fVersion, DWORD dwDevType, DWORD dwBehaviors, DWORD dwPSGPRegChange);
	virtual HRESULT 	ChangeDesktopMode(DEVICEMODE *pMode);
	virtual bool		AddDisplayMode(DISPLAYMODE *pMode);

	virtual bool        ChangeDeviceMode( __in_opt LPCTSTR szDeviceName, __out_opt DEVMODE * pOriginalMode );

	virtual HRESULT     SwitchToDesktop( __in_opt LPCTSTR pDesktopName );
	virtual HRESULT     RevertToOriginalDesktop();

	virtual void		OnPaint(WPARAM wParam, LPARAM lParam);
	virtual void		OnSize(WPARAM wParam, LPARAM lParam);
	virtual LRESULT 	OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void		OnNotify(WPARAM wParam, LPARAM lParam);
	virtual LRESULT 	OnMouse(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT 	OnKeydown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT 	OnKeyup(WPARAM wParam, LPARAM lParam);
	virtual LRESULT 	OnSetCursor(WPARAM wParam, LPARAM lParam);

	virtual bool        EnablePrivilege( __in LPCTSTR PrivilegeName );

	static INT_PTR CALLBACK HelpProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK ImageCompareProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK DevOptionsProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK ConfigureProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

	virtual HRESULT 	Initialize();
	virtual bool		PreWindowCreate();
	virtual bool		WindowCreate();
	virtual bool		CreateTabControl();
	virtual bool		CreateRebarControl();
	virtual bool		PostWindowCreate();
	virtual void		ShowHelp();

	virtual bool		ToggleFullscreen(bool bWindowed);
	virtual bool		ToggleSizeable(bool bSize);
	virtual void		Shutdown();
	virtual void		SetAdapter(UINT nAdapter, PADAPTER pAdapter, bool bForce = false);
	virtual void		ResizeWindows(UINT nWidth, UINT nHeight);
	virtual bool		QueryTestRange(CTest * pTest);
	virtual bool		TestSelected(CTest *pTest);
	virtual bool		KeyPress(WPARAM wKey, bool bOnce);
	virtual bool		HasStressTimedOut();

	inline	bool		ShowCompareWnd() { return m_bShowCompareWindow; }



public:
	HINSTANCE           m_hInstance;       // Application HINSTANCE
	CCommandLine        m_CmdLine;         // Commandline object
	CDXG9Logger         m_DXGLog;          // Log object
	ISimpleLog *        m_pSimpleLog;      // Log adapter object for shared code
	CD3D *              m_pD3D;            // D3D wrapper
	CImageCompare	    m_ImageCmp;        // Image Compare object
	bool                m_bStressMode;     // Are we in stress mode?
	bool                m_bWHQL;           // Are we running in WHQL mode?

	CWindow *           m_pDisplayWindow;
	CLogWindow *        m_pLogWindow;
	CLogWindow *        m_pStatWindow;
	CLogWindow *        m_pMsgWindow;
	CLogWindow *        m_pDetailsWindow;
	CTabWindow *        m_pTabWindow;
	CRebarWindow *      m_pRebar;
	CWindow *           m_pStatusBar;
	CLogWindow *        m_pHelpWindow;


public:
	CD3DWindowFramework();
	~CD3DWindowFramework();

	virtual void			SetLogLevel(UINT uLevel);
	virtual UINT			GetLogLevel() { return m_uLogLevel; }

	inline	CD3D*			GetD3D() { return m_pD3D; }
	inline	CImageCompare*	GetImageCmp() { return &m_ImageCmp; }
	virtual int 			GetAdapterCount();
	virtual bool			GetDisplayString( UINT n, __out_ecount(nDisplayMaxPath) LPTSTR szDisplayString, UINT nDisplayMaxPath );
	inline	HFONT			GetSmallFont() { return m_hSmallFont; }
	inline	HFONT			GetLargeFont() { return m_hLargeFont; }
	inline  HYBRIDOVERRIDE	GetHybridOverride() const { return m_uHybridOverride; };

	virtual void			WriteStringToLog(UINT uLevel, LPCTSTR pcsMsg);
	virtual void			WriteToLog(UINT uLogLevel, LPCTSTR ptcsFmt, ...);
	virtual void			WriteMessage(UINT nLevel, LPCTSTR pcsMsg);
	virtual void			WriteDetails(UINT nLevel, LPCTSTR pcsMsg);
	virtual void			WriteStatusBar(int nPart, LPCTSTR szFormat, ...);
	virtual void			WriteCommandLineHelp(LPCTSTR szFormat, ...);
	virtual void			DebugPrint(UINT uLevel, LPCTSTR sErr, ...);
	virtual int 			MsgBox(UINT uType, LPCTSTR sMsg, ...);
	virtual void			MuteLog();
	virtual void			UnMuteLog();
	inline	bool			IsLogMute() { return m_bMuteLog; }

	virtual bool			ProcessMessages();
	virtual int 			Execute(int nShow);
	virtual void			UpdateWindows();
	virtual void			ClearControls();
	virtual void			UpdateToolbar();
	virtual void			UpdateControls(CD3DTest *pTest);
	virtual void			SetDisplaySize(int nWidth, int nHeight, bool bWindowed=true, bool bForce=false);
	virtual void			SuggestExitCode(UINT);

	virtual bool			LoseDeviceDefault(UINT uAdapterToLoseDevice);
	virtual bool			LoseDeviceByOtherProcess(UINT uAdapterToLoseDevice);
	virtual bool			LoseDeviceByTDR(UINT uAdapterToLoseDevice);
	virtual bool			LoseDeviceByFUS();
	virtual bool			LoseDeviceByTerminalServer();

	virtual bool			SuspendSystem(IN bool bHibernate);

	static int				ExceptionFilter(unsigned int ExceptionCode, _EXCEPTION_POINTERS * pExceptionDetails);
};


void DeclareFramework(CD3DWindowFramework *);
extern	CD3DWindowFramework* g_pFramework;
typedef DWORD (*COVERAGEINITIALIZE)(DWORD options);
typedef DWORD (*COVERAGERESET)(const TCHAR* machine);
typedef DWORD (*COVERAGESAVE)(const TCHAR* traceName, const TCHAR* machine);
typedef DWORD (*COVERAGEUNINITIALIZE)();
#define DPF 		if(g_pFramework)g_pFramework->DebugPrint
#define MSGBOX 		if(g_pFramework)g_pFramework->MsgBox
#define STATUS(x)	if(g_pFramework)g_pFramework->WriteStatusBar(0, x)

#endif // _CD3DWINDOWFRAMEWORK_H_ 
