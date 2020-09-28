#ifndef __CD3DTEST_H__
#define __CD3DTEST_H__

#include "Framework.h"
#include "CHandle.h"
#include "fpstate.h"


class CSurface;
class CTexture;


enum D3DTESTINIT
{
	D3DTESTINIT_RUN = 0,
	D3DTESTINIT_ABORT = 1,
	D3DTESTINIT_SKIPALL = 2,
};

enum D3DTESTFINISH
{
	D3DTESTFINISH_SKIPREMAINDER = 0,
	D3DTESTFINISH_ABORT = 1,
	D3DTESTFINISH_FAIL = 2,
};

enum D3DLOSEDEV
{
	D3DLOSEDEV_DEFAULT = 0,
	D3DLOSEDEV_BYOTHERPROCESS = 1,
	D3DLOSEDEV_BYFUS = 2,
	D3DLOSEDEV_BYTERMINALSERVER = 3,
	D3DLOSEDEV_BYTDR = 4,
	D3DLOSEDEV_BYSTANDBY = 5,
	D3DLOSEDEV_BYHIBERNATE = 6,
};


extern GETFUNC      g_DCTGet;
extern COMPAREFUNC  g_DCTCompare;
extern HINSTANCE    g_hDCTDLL;


const int   MODEOPT_BACKBUFFERWIDTH     = 320;
const int   MODEOPT_BACKBUFFERHEIGHT    = 280;
const int   MODEOPT_CURRENT             = -1;
const int   MODEOPT_WHQL                = -2;
const int   MODEOPT_MAXMODES        = 0x0FFFFFFF;

const UINT  MAX_DEVICES = 16;

const DWORD WINMODE_FULLSCREEN  = 1;
const DWORD WINMODE_WINDOWED    = 2;
const DWORD WINMODE_ALL         = WINMODE_FULLSCREEN | WINMODE_WINDOWED;

const DWORD TESTCAPS_RESIZEDEVICE                = 0x00000001;
const DWORD TESTCAPS_RESIZEWINDOW                = 0x00000002;
const DWORD TESTCAPS_RESIZEMODE                  = 0x00000004;
const DWORD TESTCAPS_SERIALEXECUTION             = 0x00000008;

const DWORD DXVERSION_6         = 0x00000001;
const DWORD DXVERSION_7         = 0x00000002;
const DWORD DXVERSION_8         = 0x00000004;
const DWORD DXVERSION_81        = 0x00000008;
const DWORD DXVERSION_8_OR_81   = 0x00000010;
const DWORD DXVERSION_9         = 0x00000020;
const DWORD DXVERSION_91        = 0x00000040;
const DWORD DXVERSION_9_AND_91  = DXVERSION_9 | DXVERSION_91;
const DWORD DXVERSION_9_OR_91   = 0x00000080;
const DWORD DXVERSION_LATEST    = 0x80000000;
const DWORD DXVERSION_ALL       = 0x0FFFFFFF;

DWORD GetDXVersionFlag(float fVersion);

typedef struct
{
	TESTTYPE    Type;                       // Test type
	bool        bCompareSrcRef;             // Compare Src Device vs Ref Device
	bool        bCompareFB;                 // Use the FB for comparisons
	bool        bSkipOnDeviceLost;          // Skip() vs Fail() when DisplayFrame() fails due to D3DERR_DEVICELOST
	DWORD       dwCaps;                     // Test caps
	bool        bIgnoreFPStateCheck;        // Ignore Floating point state
	bool        bDisableIdleDetection;      // Disable system idle detection via SetThreadExecutionState
	bool        bLogAppActivation;          // Log app activations (WM_ACTIVATEAPP)
	bool        bBreakOnCorruptionDetection;// DebugBreak() on Aperture Corruption detection.
	bool        bDisableCorruptionDetection;// Disable WDDM Aperture Corruption detection (overrides bBreakOnCorruptionDetection)
	bool        bDisableDWM;                // Disable DWM
} TESTOPTIONS, *PTESTOPTIONS;

typedef struct
{
	DISPLAYMODE DisplayMode;        // Display mode for tests(windowed or fullscreen)
	UINT        nBackBufferWidth;   // Sets the backbuffer width for the device
	UINT        nBackBufferHeight;  // Sets the backbuffer height for the device
	bool        bEnumReference;     // Enum the reference device
	bool        bReference;         // Does this test use a reference device
	bool        bGoldReference;     // Does this test use a golden image as reference
	bool        bRefEqualsSrc;      // Set the REF device equal to the SRC device
	bool        bZBuffer;           // Set a Z buffer for the render target
	bool        bCachedBackBuffer;  // Does the BackBuffer can be cached
	bool        bHintAntialias;     // Set the HINTANTIALIAS flag when creating primary(DX7/DX6)
	DWORD       dwRefDevType;       // Set the device type of the ref
	DWORD       dwRefBehaviors;     // Set the behaviors for the ref
	DWORD       dwDevTypes;         // Set different device types to run against
	DWORD       dwBehaviors;        // Set the creation options for the devices
	DWORD       dwPresentFlags;     // dwFlags in PRESENT_PARAMS struct
	DWORD       dwSwapEffect;       // SwapEffect in PRESENT_PARAMS struct
	DWORD       dwPresentInterval;  // dwPresentInterval in PRESENT_PARAMS struct
	DWORD       dwWinMode;          // Set the windowing modes
	DWORD       dwPSGPRegChange;    // Select DisablePSGP registry change 
	DWORD       dwRefPSGPRegChange; // Select DisbalePSGP registry change 
	DWORD       dwVPEmuRegChange;   // Select reg change for vertex pipeline emulation
	DWORD       dwRefVPEmuRegChange;// Select reg change for ref vertex pipeline emulation
	DWORD       dwPPEmuRegChange;   // Select reg change for pixel pipeline emulation
	DWORD       dwRefPPEmuRegChange;// Select reg change for ref vertex pipeline emulation
	float       fMaxDXVersion;      // Sets the max D3D version supported by the test
	float       fMinDXVersion;      // Sets the max D3D version supported by the test
	DWORD       dwIncVersions;      // Include specific DX versions
	bool        bD3DDebugHelper;    // CreateDevice with DebugHelper flag
} D3DOPTIONS, *PD3DOPTIONS;

typedef struct
{
	UINT                uMaxDisplayModes;   // Sets the max number of Display modes
	FMT                 TextureFmt;         // Sets the default texture format
	FMT                 ZBufferFmt;         // Sets the default ZBuffer format
	FMT                 CubeFmt;            // Sets the default Cube format
	FMT                 VolumeFmt;          // Sets the default Volume format
	FMT                 BackBufferFmt;      // Sets the default BackBuffer format
	MULTISAMPLE_TYPE    MultisampType;      // Sets the default Multisample Type

	DWORD       dwTexInclude,  dwTexExclude;    // Sets the inclusion/exclusions filters for textures(PF_*)
	DWORD       dwCubeInclude, dwCubeExclude;   // Sets the inclusion/exclusions filters for cubes(PF_*)
	DWORD       dwVolInclude,  dwVolExclude;    // Sets the inclusion/exclusions filters for volume(PF_*)
	DWORD       dwZInclude,    dwZExclude;      // Sets the inclusion/exclusions filters for ZBuffers(PF_*)
}MODEOPTIONS, *PMODEOPTIONS;

typedef struct
{
	TESTOPTIONS TestOptions;
	D3DOPTIONS  D3DOptions;
	MODEOPTIONS ModeOptions;
}D3DTEST_OPTIONS, *PD3DTEST_OPTIONS;

class CD3DTest: public CTest
{
	friend class CD3DWindowFramework;
	friend class CTestManager;

public:
	CD3DWindowFramework * m_pFramework;
	CWindow * m_pWindowMultiHead[MAX_DEVICES];
	PTESTMODE           m_pModeList;
	PTESTMODE           m_pCurrentMode;
	UINT                m_uCurrentMode;
	UINT                m_uTotalModes;
	bool                m_bShowCompareWindow;
	bool                m_bCompareEnabled;
	CDevice*            m_DeviceList[MAX_DEVICES];
	UINT                m_uDeviceList;
	DWORD               m_dwVersion;
	int                 m_nTagHeight;
	bool                m_bHandlePaintMsg;
	bool                m_bException;
	bool                m_bReverseMode;
	bool                m_bGenGolden;   // Generate golden images from the src device results
	float               m_fPrevPassRequired;

	LOGFONT             m_LogFont;
	LPTSTR              m_szNewLogName;
	bool                m_bCmdLineMode;
	bool                m_bTestTerminate;
	bool                m_bStretchPresent;
	bool                m_bHaltOnFail;
	float               m_fPassPercentage;              // For tests that do their own compare, use this to tell fmk results.

	SYSTEMTIME          m_TestCaseStartTime;

	FORMAT             *m_pCommonZBufferFormats;
	UINT                m_uCommonZBufferFormats;
	FORMAT             *m_pCommonTextureFormats;
	UINT                m_uCommonTextureFormats;
	FORMAT             *m_pCommonCubeFormats;
	UINT                m_uCommonCubeFormats;
	FORMAT             *m_pCommonVolumeFormats;
	UINT                m_uCommonVolumeFormats;
	FORMAT             *m_pCommonBackBufferFormats;
	UINT                m_uCommonBackBufferFormats;
	MULTISAMPLE_TYPE   *m_pCommonMultiSampleFormats;
	UINT                m_uCommonMultiSampleFormats;
	DISPLAYMODE        *m_pCommonDisplayModes;
	UINT                m_uCommonDisplayModes;

	// Wrapper vars
	CHandle             m_StateBlockHandles;
	CVertexShaderHandle m_VertexShaderHandles;
	CHandle             m_PixelShaderHandles;

	// D3D vars
	CD3D                *m_pD3D;
	CDevice             *m_pSrcDevice;
	CDevice             *m_pRefDevice;
	CSwapChain          *m_pCmpSwapChain;       // Used in windowed mode
	CTexture            *m_pCmpTexture;         // Used in fullscreen mode

	D3DTEST_OPTIONS     m_Options;

protected:
	virtual HRESULT ScanEventLogForEvents
	(
		__in_opt LPCTSTR pEventLogName,
		__in SYSTEMTIME * pScanStartSystemTime,
		UINT cEventIDs,
		__in_ecount( cEventIDs ) DWORD * pEventIDs,
		__out_opt DWORD * pMatchingEventID
	);

	// Internal to CD3DTest - should not be called by test
	virtual bool    CopyFormatList(FORMAT *pSrc, FORMAT **pFmtOut, UINT *uFmtOut);
	virtual bool    FindCommonFormats(FORMAT *pSrc, FORMAT *pRef, FORMAT **pOut, UINT *puOut);
	virtual bool    FindCommonFormats(MULTISAMPLE_TYPE *pSrc, UINT uSrc, MULTISAMPLE_TYPE *pRef, UINT uRef, MULTISAMPLE_TYPE **pOut, UINT *puOut);
	virtual bool    EnumCommonFormats(CDevice **pDevList, UINT uDevices);
	virtual bool    FilterCommonFormats();
	virtual void    ReleaseCommonFormats();

	virtual bool    EnumCommonDisplayModes(CDevice** pDevList, UINT uDevices);
	virtual bool    FindCommonDisplayModes(DISPLAYMODE *pMode0, UINT uMode0, DISPLAYMODE *pMode1, UINT uMode1, DISPLAYMODE **pOut, UINT *uOut);
	virtual bool    FilterDisplayModes(DISPLAYMODE *pModes, UINT uModes, DISPLAYMODE **pOut, UINT *uOut);
	virtual void    ReleaseCommonDisplayModes();

	virtual bool    ParseCmdLine();
	virtual bool    ParseResolution(LPCTSTR pcsRes, __out UINT *pWidth, __out UINT *pHeight, __out FMT *pFmt);

	virtual bool    AddMode(DISPLAYMODE *pDisplay, bool bWindowed, int nTexture, int nZBuffer, int nCube, int nVolume, int nMS, int nMSQ, int nBB);
	virtual bool    AddFormatModes(DISPLAYMODE *pDisplay, bool bWindowed);
	virtual bool    GetFormatIndex(FMT fmtTarget, FORMAT *pList, UINT *pIndex);
	virtual bool    GetZBufferFormatIndex(FMT fmt, FORMAT *pList, FORMAT *pRenderTarget, UINT *pIndex);
	virtual bool    GetBackBufferFormatIndex(FMT fmt, FORMAT *pList, FORMAT *pRenderTarget, UINT *pIndex);
	virtual bool    GetFormatIndex(MULTISAMPLE_TYPE MultiSamp, MULTISAMPLE_TYPE *pList, UINT uMS, UINT *pIndex);
	virtual void    ReleaseModeList();
	virtual bool    IsFullscreenAvailable(int nAdapter);
	virtual HRESULT LoadSaveGoldenImage(CSurface *pSurface, bool bLoadorSave);
	virtual HRESULT ProcessGoldenImage();
	virtual HRESULT DeviceCapture(DWORD dwFlags);

	virtual bool    SetupTest();
	virtual bool    CleanupTest();
	virtual bool    NextTestMode();
	virtual bool    PreviousGroup();
	virtual void    ExecuteOneTest();
	virtual HRESULT ExecuteTests();
	virtual void    UpdateControls();

	virtual HRESULT CreateObjects();
	virtual void    ReleaseObjects();

	virtual bool    IsDeviceLost();
	virtual bool    HandleDeviceLost();
	virtual bool    ResetTSConnect();
	virtual bool    ResetLostDevices();
	virtual DWORD   GetLostDevices();

	// Init/release/etc. generally called by the WindowFramework
	virtual bool    Initialize(CD3D *pD3D, bool bException);
	virtual void    ReleaseAll();

	virtual bool    ToggleCompareWindow();

	virtual void    OnPaint();
	virtual LRESULT OnCommand(WPARAM wParam, LPARAM lParam);

	virtual HRESULT PulseScreensaverTimeout();

public:

	CD3DTest();
	~CD3DTest();

	// Standard
	void            SetTestType(TESTTYPE Type);     // REQUIRED - Called in the constructor, sets defaults by type

	void            Pass();
	UINT            GetPass();
	void            Fail();
	UINT            GetFail();
	void            Abort();
	UINT            GetAbort();
	void            Skip();
	UINT            GetSkip();
	void            Warn();
	UINT            GetWarn();
	void            Other(LPTSTR szComment);
	void            SetResult(DWORD Result);
	virtual void    BeginTestCase(LPCTSTR, UINT uTestNumber=0xFFFFFFFF);
	virtual void    EndTestCase();
	virtual void    ProcessError(HRESULT hr);
	virtual bool    SkipTests(UINT uCount);
	virtual bool    SkipTestRange(UINT uStart, UINT uStop);
	virtual HRESULT SetLastError(HRESULT hr);
	virtual HRESULT GetLastError();
	virtual CDevice*    GetDevice(UINT nDevice);
	inline  FORMAT*     GetCurrentTextureFormat() { return m_pCurrentMode->nTextureFormat < (int)m_uCommonTextureFormats ? &(m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat]) : NULL; }
	inline  FORMAT*     GetCurrentCubeFormat() { return m_pCurrentMode->nCubeFormat < (int)m_uCommonCubeFormats ? &(m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat]) : NULL; }
	inline  FORMAT*     GetCurrentVolumeFormat() { return m_pCurrentMode->nVolumeFormat < (int)m_uCommonVolumeFormats ? &(m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat]) : NULL; }
	inline  FORMAT*     GetCurrentZBufferFormat() { return m_pCurrentMode->nZBufferFormat < (int)m_uCommonZBufferFormats ? &(m_pCommonZBufferFormats[m_pCurrentMode->nZBufferFormat]) : NULL; }
	inline  bool    IsCompareEnabled() { return m_bCompareEnabled; }

	// Command-line interaction with window framework
	virtual void    ReadString(LPCTSTR pKey, LPCTSTR pDefault, __out_ecount(nBuffer) LPTSTR pBuffer, int nBuffer, bool bRemove = false);
	virtual bool    ReadString(LPCTSTR pKey, __out_ecount(nBuffer) LPTSTR pBuffer, int nBuffer, bool bRemove = false);
	virtual void    ReadInteger(LPCTSTR pKey, int iDefault, __out int * pInt, bool bRemove = false);
	virtual bool    ReadInteger(LPCTSTR pKey, __out int * pInt, bool bRemove = false);
	virtual void    ReadFloat(LPCTSTR pKey, float fDefault, __out float * pFloat, bool bRemove = false);
	virtual bool    ReadFloat(LPCTSTR pKey, __out float * pFloat, bool bRemove = false);
	virtual bool    KeySet(LPCTSTR pKey, bool bRemove=false);
	virtual void    AddKeys(LPCTSTR pKey);
	virtual bool    IsWHQL();       //Is this test running as a WHQL test?

	// Logging functions routed to framework
	virtual void    WriteMessage(UINT uLogLevel, const TCHAR *ptcsFmt, ...);
	virtual void    WriteDetails(UINT uLogLevel, const TCHAR *ptcsFmt, ...);
	virtual void    WriteToLog(UINT uLogLevel, const TCHAR *ptcsFmt, ...);
	virtual void    WriteToLog(const TCHAR *ptcsFmt, ...);
	virtual void    WriteStatus(const TCHAR *ptcsKey, const TCHAR *ptcsFmt,...);
	virtual void    WriteStatus(int nLevel, const TCHAR *ptcsKey, const TCHAR *ptcsFmt,...);
	virtual void    WriteStatusBar(int nField, const TCHAR *ptcsFmt, ...);
	virtual void    MuteLog();      //After this call, nothing is written to the log
	virtual void    UnMuteLog();

	// Process window messages
	virtual bool    ProcessMessages();
    
	// Hooks to override settings before/after modes are setup
	virtual bool    PreModeSetup();
	virtual bool    PostModeSetup();

	// Devce management
	virtual bool    AddDeviceToList(CDevice *pDevice);
	virtual HRESULT CreateDevices(DEVICEMODE *pMode);
	virtual void    ReleaseDevices();
	virtual HRESULT UpdateDevices(PRESENT_PARAMETERS *pParam);
	virtual HRESULT ValidateDevices(DEVICEMODE *pMode);
	virtual bool    ModeSupported(__in DEVICEMODE *pMode);
	virtual HRESULT CreateImageCompare(CSwapChain **ppSwapChain);
	virtual HRESULT CreatePresentParams(TESTMODE *pMode, PRESENT_PARAMETERS **ppParams);
	virtual HRESULT Resize(UINT uWidth, UINT uHeight);

	// Lost Devices
	virtual bool    LoseTheDevice(DWORD dwLostType = D3DLOSEDEV_DEFAULT);


	// Mode management
	virtual HRESULT SetupTestMode(TESTMODE *pMode);
	virtual bool    ReleaseTestMode();
	virtual HRESULT SetupDeviceMode(DEVICEMODE *pMode);
	virtual bool    ConstructModeList(DEVICEMODE *pMode);
	virtual void    ReleaseDevMode();

	// Test management
	virtual void    TestFinish(UINT uState);

	// RegKey management
	virtual bool    EnumReference();
	virtual bool    RemoveReference();

	// Windowing support
	virtual void    SetCompareSrcRef(bool bEnable);
	virtual void    SetupDisplay(TESTMODE *pMode, bool bWindowed);

	virtual void    WriteCommandLineHelp(LPCTSTR,...);
	virtual void    CommandLineHelp();
	virtual void    ConstructTestCaption(__out_ecount(nBuffer) LPTSTR pBuffer, int nBuffer);
	virtual PTESTMODE GetModeList() { return m_pModeList; }
	virtual PTESTMODE GetCurrentMode() { return m_pCurrentMode; }

	//
	// Common Test Functions/Overrides - these should be defined/called by most tests
	//

	virtual void    SetTestOptions(DWORD dwTestScenarios, float fMinVersion, bool bPure, bool bRequireRefRast); // PSGP extension of SetTestType
	virtual bool    CapsCheck();            // Called once for each group before any test initialization has been done
	virtual UINT    TestInitialize();   // Called once for each group if CapsCheck succeeds
	virtual bool    TestTerminate();    // Called once for each group if TestInitialize succeeds
	virtual bool    Setup();                // Called everytime devices are restored(group init, device lost, etc)
	virtual void    Cleanup();              // Called everytime devices are restored(group init, device lost, etc)
	virtual bool    ExecuteTest(UINT);      // Called once for each test defined by SetTestRange()
	virtual void    UpdateStatus();         // Called once each frame to update status window text
	virtual void    UpdateDetails();        // Called once each frame to update details window text

	//
	// Less Common Test Overrides - these are overridden by some tests
	//

	virtual HRESULT GetFrontBuffers( CSurface ** pSurface, DWORD dwFlags ); //Takes a point to an array of surfaces, and the flags indicating which devices to capture
	virtual bool    DisplayFrame();     // Presents the results from the rendering
	virtual bool    ProcessFrame();     // Compares SRC/REF if requested
	virtual bool    ClearFrame();       // Clears the frame to the default color
	virtual void    OnDeviceLost();         // Called after DEVICELOST.  Test should release all D3D objects created in POOL_DEFAULT
	virtual void    OnReset();              // Called after Reset() succeeds.  Test should re-create all D3D objects released in OnDeviceLost
											// and refresh all data within existing objects.
	virtual bool    DrawStats(int nHeight, int nWidth, CDevice *pDevice);   // Draw the stats in fullscreen mode
	virtual bool    FilterFormats(FORMAT *pFmtIn, DWORD dwInclFlags, DWORD dwExclFlags, FORMAT **pFmtOut, UINT *uCount); // Returns a new list based on the filters specified
	virtual bool    FilterResolution(PDISPLAYMODE pMode);   // Override to filter fullscreen resolutions - return true if you want to add this mode

	//
	// Legacy hooks - only older apps should use these
	//

	virtual void    SceneRefresh();
	virtual bool    SetDefaultMaterials();
	virtual bool    SetDefaultTextures();
	virtual bool    SetDefaultViewport();
	virtual bool    SetDefaultLights();
	virtual bool    SetDefaultRenderStates();
	virtual bool    SetDefaultMatrices();

	// Device wrappers - wrap all registered devices

	virtual bool    Clear(DWORD Count,D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil, DWORD dwFlags = CD3D_ALL);
	virtual bool    Present(RECT* pSourceRect,RECT* pDestRect,HWND hDestWindowOverride,RGNDATA* pDirtyRegion, DWORD dwFlags = CD3D_ALL);
	virtual bool    SetTransform(D3DTRANSFORMSTATETYPE Type, LPD3DMATRIX Matrix, DWORD dwFlags = CD3D_ALL);

	virtual bool    CreateStateBlock(D3DSTATEBLOCKTYPE eType, DWORD *dwHandle, DWORD dwFlags = CD3D_ALL);
	virtual bool    CaptureStateBlock(DWORD dwHandle, DWORD dwFlags = CD3D_ALL);
	virtual bool    ApplyStateBlock(DWORD dwHandle, DWORD dwFlags = CD3D_ALL);
	virtual bool    DeleteStateBlock(DWORD dwHandle, DWORD dwFlags = CD3D_ALL);
	virtual bool    BeginStateBlock(DWORD dwFlags = CD3D_ALL);
	virtual bool    EndStateBlock(DWORD *dwHandle, DWORD dwFlags = CD3D_ALL);

	virtual bool    BeginScene(DWORD dwFlags = CD3D_ALL);
	virtual bool    EndScene(DWORD dwFlags = CD3D_ALL);
	virtual bool    LightEnable(DWORD dwLight, BOOL bOn, DWORD dwFlags = CD3D_ALL);
	virtual bool    SetLightState(D3DLIGHTSTATETYPE dwState, DWORD dwValue, DWORD dwFlags = CD3D_ALL);
	virtual bool    SetRenderState(D3DRENDERSTATETYPE dwState, DWORD dwValue, DWORD dwFlags = CD3D_ALL);
	virtual bool    SetMaterial(MATERIAL *pMat, DWORD dwFlags = CD3D_ALL);
	virtual bool    CreateRenderTarget(UINT Width,UINT Height,FORMAT Format,MULTISAMPLE_TYPE MultiSample,BOOL Lockable,LPSURFACES* ppSurface,DWORD dwFlags = CD3D_ALL);
	virtual bool    CreateRenderTarget(UINT Width,UINT Height,FORMAT Format,MULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,LPSURFACES* ppSurface,DWORD dwFlags = CD3D_ALL);
	virtual bool    CreateDepthStencilSurface(UINT Width,UINT Height,FORMAT Format,MULTISAMPLE_TYPE MultiSample,LPSURFACES *ppSurface,DWORD dwFlags = CD3D_ALL);
	virtual bool    CreateDepthStencilSurface(UINT Width,UINT Height,FORMAT Format,MULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,LPSURFACES *ppSurface,DWORD dwFlags = CD3D_ALL);
	virtual bool    CreateImageSurface(UINT Width,UINT Height,FORMAT Format,LPSURFACES* ppSurface, DWORD dwFlags = CD3D_ALL);
	virtual bool    CreateOffscreenPlainSurface(UINT Width,UINT Height,FORMAT Format,DWORD dwPool, LPSURFACES* ppSurface, DWORD dwFlags = CD3D_ALL);
	virtual bool    SetRenderTarget(LPSURFACES pRenderTarget, LPSURFACES pNewZStencil, DWORD dwFlags = CD3D_ALL);
	virtual bool    SetRenderTarget(DWORD RenderTargetIndex, LPSURFACES pRenderTarget, DWORD dwFlags = CD3D_ALL);
	virtual bool    SetDepthStencilSurface(LPSURFACES pNewZStencil, DWORD dwFlags = CD3D_ALL);
	virtual bool    GetRenderTarget(LPSURFACES *ppRenderTarget, DWORD dwFlags = CD3D_ALL);
	virtual bool    GetDepthStencilSurface(LPSURFACES *ppZStencilSurface, DWORD dwFlags = CD3D_ALL);
	virtual bool    GetBackBuffer(UINT BackBuffer,DWORD Type,LPSURFACES* ppBackBuffer, DWORD dwFlags = CD3D_ALL); 
	virtual bool    GetBackBuffer(UINT iSwapChain,UINT BackBuffer,DWORD Type,LPSURFACES* ppBackBuffer, DWORD dwFlags = CD3D_ALL); 
	virtual bool    GetFrontBuffer(LPSURFACES pDestSurface, DWORD dwFlags = CD3D_ALL);
	virtual bool    GetFrontBufferData(UINT iSwapChain,LPSURFACES pDestSurface, DWORD dwFlags = CD3D_ALL);
	virtual bool    GetRenderTargetData(LPSURFACES pSrcRenderTarget, LPSURFACES pDestSurface, DWORD dwFlags = CD3D_ALL);
	virtual bool    GetTexture(int nIndex, LPBASETEXTURES* ppTextures, DWORD dwFlags = CD3D_ALL);
	virtual bool    SetTexture(DWORD dwStage, LPBASETEXTURES pTexture, DWORD dwFlags = CD3D_ALL);
	virtual bool    SetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE Type, DWORD dwValue, DWORD dwFlags = CD3D_ALL);
	virtual bool    GetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE Type, DWORD *pdwValue, DWORD dwFlags = CD3D_SRC);
	virtual bool    SetLight(DWORD dwStage, LIGHT *pLight, DWORD dwFlags = CD3D_ALL);
	virtual bool    SetViewport(LPVIEWPORT pViewport, DWORD dwFlags = CD3D_ALL);
	virtual bool    SetClipPlane(DWORD dwIndex,D3DVALUE* pPlaneEquation, DWORD dwFlags = CD3D_ALL);
	virtual UINT    GetAvailableTextureMem(DWORD dwFlags = CD3D_SRC); 
	virtual bool    EvictManagedResources(DWORD dwFlags = CD3D_ALL);
	virtual bool    ResourceManagerDiscardBytes(DWORD Bytes, DWORD dwFlags = CD3D_ALL);
	virtual bool    GetDisplayMode(DISPLAYMODE* pMode, DWORD dwFlags = CD3D_SRC);
	virtual bool    GetCreationParameters(DEVICE_CREATION_PARAMETERS *pParameters, DWORD dwFlags = CD3D_SRC);
	virtual bool    SetCursorProperties(UINT XHotSpot,UINT YHotSpot,LPSURFACES pCursorBitmap, DWORD dwFlags = CD3D_ALL);
	virtual void    SetCursorPosition(UINT XScreenSpace,UINT YScreenSpace,DWORD Flags, DWORD dwFlags = CD3D_SRC);
	virtual void    ShowCursor(BOOL bShow, DWORD dwFlags = CD3D_ALL);
	virtual bool    Reset(PRESENT_PARAMETERS* pPresentationParameters, DWORD dwFlags = CD3D_ALL);
	virtual bool    GetRasterStatus(RASTER_STATUS* pRasterStatus, DWORD dwFlags = CD3D_SRC);
	virtual void    SetGammaRamp(DWORD Flags,GAMMARAMP* pRamp, DWORD dwFlags = CD3D_ALL);
	virtual void    GetGammaRamp(GAMMARAMP* pRamp, DWORD dwFlags = CD3D_SRC);
	virtual bool    CopyRects(LPSURFACES pSourceSurface,RECT* pSourceRectsArray,UINT cRects,LPSURFACES pDestinationSurface,POINT* pDestPointsArray, DWORD dwFlags = CD3D_ALL);
	virtual bool    StretchRect(LPSURFACES pSourceSurface,RECT* pSourceRect,LPSURFACES pDestinationSurface,RECT* pDestRect, DWORD dwFilter, DWORD dwFlags = CD3D_ALL);
	virtual bool    UpdateSurface(LPSURFACES pSourceSurface,RECT* pSourceRects,LPSURFACES pDestinationSurface,POINT* pDestPoint, DWORD dwFlags = CD3D_ALL);
	virtual bool    GetTransform(D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix, DWORD dwFlags = CD3D_SRC);
	virtual bool    MultiplyTransform(D3DTRANSFORMSTATETYPE Type,D3DMATRIX* Matrix, DWORD dwFlags = CD3D_ALL);
	virtual bool    GetViewport(LPVIEWPORT pViewport, DWORD dwFlags = CD3D_SRC);
	virtual bool    GetMaterial(MATERIAL *pMaterial, DWORD dwFlags = CD3D_SRC);
	virtual bool    GetLight(DWORD dwIndex, LIGHT *pLight, DWORD dwFlags = CD3D_SRC);
	virtual bool    GetLightEnable(DWORD dwLightNum, BOOL* pEnable, DWORD dwFlags = CD3D_SRC);
	virtual bool    GetClipPlane(DWORD dwIndex, float* pPlane, DWORD dwFlags = CD3D_SRC);
	virtual bool    GetLightState(D3DLIGHTSTATETYPE dwState, DWORD *pdwValue, DWORD dwFlags = CD3D_SRC);
	virtual bool    GetRenderState(D3DRENDERSTATETYPE dwState, DWORD *pdwValue, DWORD dwFlags = CD3D_SRC);
	virtual bool    SetClipStatus(LPCLIPSTATUS pClipStatus, DWORD dwFlags = CD3D_ALL);
	virtual bool    GetClipStatus(LPCLIPSTATUS pClipStatus, DWORD dwFlags = CD3D_SRC);
	virtual bool    ValidateDevice(DWORD *pdwPasses, DWORD dwFlags = CD3D_SRC);
	virtual bool    GetInfo(DWORD dwDevInfoID,LPVOID pDevInfoStruct,DWORD dwSize, DWORD dwFlags = CD3D_SRC);
	virtual bool    GetVerticalBlankStatus(LPBOOL lpbIsInVB, DWORD dwFlags = CD3D_SRC);
	virtual bool    SetPaletteEntries(UINT PaletteNumber,PALETTEENTRY* pEntries, DWORD dwFlags = CD3D_ALL);
	virtual bool    GetPaletteEntries(UINT PaletteNumber,PALETTEENTRY* pEntries, DWORD dwFlags = CD3D_SRC);
	virtual bool    SetCurrentTexturePalette(UINT PaletteNumber, DWORD dwFlags = CD3D_ALL);
	virtual bool    GetCurrentTexturePalette(UINT *PaletteNumber, DWORD dwFlags = CD3D_SRC);
	virtual bool    CreateVertexDeclaration(VERTEXELEMENT* pVE, CnVertexDeclaration** ppDecl, DWORD dwFlags = CD3D_ALL);
	virtual bool    CreateVertexShader(LPDWORD pdwDeclaration, LPDWORD pdwFunction, LPDWORD pdwHandle, DWORD dwUsage, DWORD dwFlags = CD3D_ALL);
	virtual bool    SetVertexDeclaration(CnVertexDeclaration* pDecl, DWORD dwFlags = CD3D_ALL);
	virtual bool    SetVertexShader(DWORD dwHandle, DWORD dwFlags = CD3D_ALL);
	virtual bool    GetVertexShader(DWORD *dwHandle, DWORD dwFlags = CD3D_ALL);
	virtual bool    SetFVF(DWORD dwFVF, DWORD dwFlags = CD3D_ALL);
	virtual bool    DeleteVertexShader(DWORD dwHandle, DWORD dwFlags = CD3D_ALL);

	// 9 style vertex shader calls:
	virtual bool    CreateVertexShader(LPDWORD pdwFunction, CnVertexShader **ppVShaders, DWORD dwFlags = CD3D_ALL);
	virtual bool    SetVertexShader(CnVertexShader* pShader, DWORD dwFlags = CD3D_ALL);
	virtual bool    GetVertexShader(CnVertexShader** ppShader, DWORD dwFlags = CD3D_SRC);
	virtual bool    GetFVF(DWORD *pHandle, DWORD dwFlags = CD3D_SRC);

	virtual bool    SetVertexShaderConstant(UINT dwRegister, void* lpvConstantData, UINT dwConstantCount, DWORD dwFlags = CD3D_ALL)
						{ return SetVertexShaderConstantF(dwRegister, (float*)lpvConstantData, dwConstantCount, dwFlags); }
	virtual bool    GetVertexShaderConstant(UINT Register,void* pConstantData,UINT ConstantCount, DWORD dwFlags = CD3D_SRC)
						{ return GetVertexShaderConstantF(Register,(float*)pConstantData,ConstantCount,dwFlags); } 
	virtual bool    SetVertexShaderConstantF(UINT dwRegister, CONST float* pConstantData, UINT dwConstantCount, DWORD dwFlags = CD3D_ALL);
	virtual bool    GetVertexShaderConstantF(UINT Register,float* pConstantData,UINT ConstantCount, DWORD dwFlags = CD3D_SRC);
	virtual bool    SetVertexShaderConstantI(UINT dwRegister, CONST int* pConstantData, UINT dwConstantCount, DWORD dwFlags = CD3D_ALL);
	virtual bool    GetVertexShaderConstantI(UINT Register,int* pConstantData,UINT ConstantCount, DWORD dwFlags = CD3D_SRC);
	virtual bool    SetVertexShaderConstantB(UINT StartRegister,CONST BOOL* pConstantData, UINT BoolCount, DWORD dwFlags = CD3D_ALL);
	virtual bool    GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount, DWORD dwFlags = CD3D_SRC);
	virtual bool    GetVertexShaderDeclaration(DWORD Handle,void* pData,DWORD* pSizeOfData, DWORD dwFlags = CD3D_SRC);
	virtual bool    GetVertexShaderFunction(DWORD Handle,void* pData,DWORD* pSizeOfData, DWORD dwFlags = CD3D_SRC);
	virtual bool    SetStreamSource(UINT streamNumber, CnVertexBuffer  *pStreamData,  UINT  stride,  UINT offset = 0, DWORD dwFlags = CD3D_ALL);
	virtual bool    GetStreamSource(UINT streamNumber, CnVertexBuffer **ppStreamData, UINT *pStride, DWORD dwFlags = CD3D_SRC);
	virtual bool    SetIndices(CnIndexBuffer  *pIndexData,   UINT  baseVertexIndex,  DWORD dwFlags = CD3D_ALL);
	virtual bool    GetIndices(CnIndexBuffer **ppIndexData,  UINT *pBaseVertexIndex, DWORD dwFlags = CD3D_SRC);

	// 9 style pixel shader calls:
	virtual bool    CreatePixelShader(LPDWORD pdwFunction, CnPixelShader **ppPShaders, DWORD dwFlags = CD3D_ALL);
	virtual bool    SetPixelShader(CnPixelShader* pShader, DWORD dwFlags = CD3D_ALL);
	virtual bool    GetPixelShader(CnPixelShader** ppShader, DWORD dwFlags = CD3D_SRC);

	virtual bool    CreatePixelShader(LPDWORD pdwFunction, LPDWORD pdwHandle, DWORD dwFlags = CD3D_ALL);
	virtual bool    SetPixelShader(DWORD dwHandle, DWORD dwFlags = CD3D_ALL);
	virtual bool    GetPixelShader(DWORD* pHandle, DWORD dwFlags = CD3D_SRC);
	virtual bool    DeletePixelShader(DWORD dwHandle, DWORD dwFlags = CD3D_ALL);
	virtual bool    SetPixelShaderConstant(DWORD dwRegisterAddress, LPVOID lpvConstantData, DWORD dwConstantCount, DWORD dwFlags = CD3D_ALL)
						{ return SetPixelShaderConstantF(dwRegisterAddress, (float*)lpvConstantData, dwConstantCount, dwFlags); }
	virtual bool    GetPixelShaderConstant(DWORD Register,void* pConstantData,DWORD ConstantCount, DWORD dwFlags = CD3D_SRC)
						{ return GetPixelShaderConstantF(Register,(float*)pConstantData,ConstantCount,dwFlags); } 
	virtual bool    SetPixelShaderConstantF(UINT StartRegister,const float* pConstantData,UINT Vector4fCount, DWORD dwFlags = CD3D_ALL);
	virtual bool    GetPixelShaderConstantF(UINT StartRegister,float* pConstantData,UINT Vector4fCount, DWORD dwFlags = CD3D_SRC);
	virtual bool    SetPixelShaderConstantI(UINT StartRegister,const int* pConstantData,UINT Vector4iCount, DWORD dwFlags = CD3D_ALL);
	virtual bool    GetPixelShaderConstantI(UINT StartRegister,int* pConstantData,UINT Vector4iCount, DWORD dwFlags = CD3D_SRC);
	virtual bool    SetPixelShaderConstantB(UINT StartRegister,const BOOL* pConstantData,UINT BoolCount, DWORD dwFlags = CD3D_ALL);
	virtual bool    GetPixelShaderConstantB(UINT StartRegister,BOOL* pConstantData,UINT BoolCount, DWORD dwFlags = CD3D_SRC);
	virtual bool    GetPixelShaderFunction(DWORD Handle,void* pData,DWORD* pSizeOfData, DWORD dwFlags = CD3D_SRC);
	virtual bool    DrawRectPatch(UINT uHandle, float *pSegments, RECTPATCH_INFO *pInfo, DWORD dwFlags = CD3D_ALL);
	virtual bool    DrawTriPatch(UINT uHandle, float *pSegments, TRIPATCH_INFO *pInfo, DWORD dwFlags = CD3D_ALL);
	virtual bool    DeletePatch(UINT uHandle, DWORD dwFlags = CD3D_ALL);
	virtual bool    SetNPatchMode(float nSegments, DWORD dwFlags = CD3D_ALL);
	virtual float   GetNPatchMode(DWORD dwFlags = CD3D_ALL);
	virtual bool    GetDirect3D(CD3D **ppD3D, DWORD dwFlags = CD3D_ALL);
	virtual bool    CreateTexture(UINT Width,UINT Height,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,LPTEXTURES* ppTexture, DWORD dwFlags = CD3D_ALL);
	virtual bool    CreateVolumeTexture(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,LPVOLUMETEXTURES* ppVolumeTexture, DWORD dwFlags = CD3D_ALL);
	virtual bool    CreateCubeTexture(UINT EdgeLength,UINT Levels,DWORD Usage,FORMAT Format, DWORD Pool,LPCUBETEXTURES* ppCubeTexture, DWORD dwFlags = CD3D_ALL);
	virtual bool    UpdateTexture(LPBASETEXTURES pSourceTexture, LPBASETEXTURES pDestinationTexture, DWORD dwFlags = CD3D_ALL);
	virtual bool    CreateAdditionalSwapChain(PRESENT_PARAMETERS* pParams, LPSWAPCHAINS* ppSwapChain, DWORD dwFlags = CD3D_ALL);
	virtual bool    CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, DWORD Pool, LPVERTEXBUFFERS* ppVB, DWORD dwFlags = CD3D_ALL);
	virtual bool    CreateIndexBuffer(UINT Length, DWORD Usage, FMT Format, DWORD Pool, LPINDEXBUFFERS* ppIB, DWORD dwFlags = CD3D_ALL);
	virtual bool    ProcessVertices(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,LPVERTEXBUFFERS pDestBuffer,DWORD dwPVFlags, DWORD dwDeviceFlags = CD3D_ALL);
	virtual bool    ProcessVertices(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,LPVERTEXBUFFERS pDestBuffer, CnVertexDeclaration *pDestDecl, DWORD dwPVFlags, DWORD dwDeviceFlags = CD3D_ALL);
	virtual bool    DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount, DWORD dwDeviceFlags = CD3D_ALL);
	virtual bool    DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT MinIndex,UINT NumVertices,UINT StartIndex,UINT PrimitiveCount,INT BaseVertexIndex = 0, DWORD dwDeviceFlags = CD3D_ALL);
	virtual bool    DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,void* pVertexStreamZeroData,UINT VertexStreamZeroStride,DWORD dwDeviceFlags = CD3D_ALL);
	virtual bool    DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertexIndices,UINT PrimitiveCount,void* pIndexData,FORMAT IndexDataFormat,void* pVertexStreamZeroData,UINT VertexStreamZeroStride,DWORD dwDeviceFlags = CD3D_ALL);
	virtual bool    DrawPrimitiveUserMemory(D3DPRIMITIVETYPE PrimitiveType, DWORD dwVertexTypeDesc,LPVOID lpVertices, DWORD  dwVertexCount, LPVOID lpIndices, DWORD  dwIndexCount, DWORD  dwFlags, bool bIs32BitIndices = false, DWORD dwDeviceFlags = CD3D_ALL);
	virtual bool    DrawPrimitiveVertexBuffer(D3DPRIMITIVETYPE PrimitiveType,CnVertexBuffer *pSrcBuffer, DWORD dwStartVertex, DWORD dwNumVertices, LPWORD lpwIndices, DWORD dwIndexCount, DWORD dwFlags, DWORD dwUsage=0, DWORD dwPool=0, bool bIs32BitIndices = false, DWORD dwDeviceFlags = CD3D_ALL);
	virtual bool    DrawPrimitiveStridedData(D3DPRIMITIVETYPE PrimitiveType,DWORD dwVertexTypeDesc,LPDRAWPRIMITIVESTRIDEDDATA lpVertexArray,DWORD dwVertexCount,LPWORD lpwIndices,DWORD dwIndexCount,DWORD dwFlags,DWORD dwUsage=0,DWORD dwPool=0,bool bIs32BitIndices = false, DWORD dwDeviceFlags = CD3D_ALL);
	virtual bool    RenderPrimitive(D3DPRIMITIVETYPE PrimType, DWORD dwFVF,
									LPVOID pVertices, DWORD dwVertexCount, LPVOID pIndices, DWORD dwIndexCount, 
									RPOPTIONS *pOptions, DWORD dwFlags = CD3D_ALL);
	virtual bool    SetScissorRect(CONST RECT *pRect, DWORD dwFlags = CD3D_ALL);
	virtual bool    GetScissorRect(RECT *pRect, DWORD dwFlags = CD3D_ALL);
	virtual bool    SetSamplerState(DWORD Sampler, SAMPLERSTATETYPE Type, DWORD Value, DWORD dwFlags = CD3D_ALL);
	virtual bool    GetSamplerState(DWORD Sampler, SAMPLERSTATETYPE Type, DWORD *Value, DWORD dwFlags = CD3D_ALL);
	virtual bool    CreateQuery(QUERYTYPE Type, LPQUERYS* ppQuery, DWORD dwFlags = CD3D_SRC);
	virtual bool    SetSoftwareVertexProcessing(BOOL bSoftware, DWORD dwFlags = CD3D_ALL);
	virtual BOOL    GetSoftwareVertexProcessing(DWORD dwFlags = CD3D_ALL);
	virtual UINT    GetNumberOfSwapChains(DWORD dwFlags = CD3D_SRC);
	virtual bool    GetSwapChain(UINT iSwapChain,LPSWAPCHAINS* ppSwapChain, DWORD dwFlags = CD3D_ALL);
	virtual bool    SetStreamSourceFreq(UINT nStreamNumber, UINT nDivider, DWORD dwFlags = CD3D_ALL);
	virtual bool    GetStreamSourceFreq(UINT nStreamNumber, UINT* pDividerOut, DWORD dwFlags = CD3D_ALL);
	virtual bool    D3DXCreateTextureFromResourceA(HMODULE hSrcModule, LPCSTR pSrcResource, LPTEXTURES* ppTextures, DWORD dwFlags = CD3D_ALL);
	virtual bool    D3DXCreateTextureFromResourceExA(HMODULE hSrcModule, LPCSTR pSrcResource, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pScrInfo, PALETTEENTRY* pPalette, LPTEXTURES* ppTextures, DWORD dwFlags = CD3D_ALL);
	virtual bool    D3DXLoadSurfaceFromSurface(CnSurface* pDestSurface,CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, CnSurface* pSrcSurface, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey, DWORD dwFlags = CD3D_ALL);
	virtual bool    CreateEffectFromFile (LPCTSTR pSrcFile, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXEFFECTPOOL pPool, LPEFFECTS* ppEffects, LPD3DXBUFFER* ppCompilationErrors, DWORD dwDevFlags = CD3D_ALL);
	virtual bool    CreateEffectFromResource (HMODULE hSrcModule, LPCTSTR pSrcresource, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXEFFECTPOOL pPool, LPEFFECTS* ppEffects, LPD3DXBUFFER* ppCompilationErrors, DWORD dwDevFlags = CD3D_ALL);

	// LONGHORN ONLY wrappers
	virtual bool    ComposeRects( LPSURFACES ppSrc, LPSURFACES ppDst, LPVERTEXBUFFERS ppSrcRectDescs, UINT NumRects, LPVERTEXBUFFERS ppDstRectDescs, UINT Operation, int Xoffset, int Yoffset, DWORD dwFlags = CD3D_ALL );
	virtual bool    SetConvolutionMonoKernel( UINT Width, UINT Height, float* RowWeights, float* ColumnWeights, DWORD dwFlags = CD3D_ALL );


	// Device wrapper helper functions
	virtual bool    SkipDevice(UINT uDevice, DWORD dwFlags);
	virtual void    SetFont( LOGFONT lf, DWORD dwFlags=CD3D_ALL);

	//
	// API cycling control functions
	//

	virtual void        SetAPI(RENDERPRIMITIVEAPI dwAPI);
	virtual void        SetAPI(PRENDERPRIMITIVEAPI pAPIList, DWORD dwAPIListSize, 
							   DWORD dwAPICyclingFrequency = 3);
	virtual RENDERPRIMITIVEAPI  GetAPI();
	virtual void        SetDestinationFVF(DWORD dwDestinationFVF);
	virtual DWORD       GetDestinationFVF();
	virtual void        SwitchAPI(DWORD dwTestNumber);
	virtual DWORD       GetVertexSize(DWORD dwFVF);
	virtual TCHAR*      RPModeToString();
	virtual TCHAR*      TexLoadToString(DWORD dwTexLoad);



protected:
	//---------------------------------------------------------------------
	// Internal variables for API wrapper and cycling. (RP prefix)

	float                   m_fSkipsPerTest;                // Number of skips to do between running tests
	float                   m_fSkipCount;                   // Skip rate counter;

	DWORD                   m_dwRPAPICyclingFrequency;      // 0 in this field will switch off cycling
	RENDERPRIMITIVEAPI      m_dwRPCurrentAPI;               // Current API    
	PRENDERPRIMITIVEAPI     m_pRPAPIList;                   // Pointer to an array of APIs to cycle
	DWORD                   m_dwRPAPIListSize;              // Size of this array
	bool                    m_bRPCommandLineOverwrite;      // true if the application in running with -api command line
	DWORD                   m_dwRPAPICyclingIndex;          // Internal index for API cycling

	DWORD                   m_dwDestinationFVF;             // Destination vertex format for ProcessVertices calls

	// Static data for API cycling
	static RENDERPRIMITIVEAPI   m_pRPDefaultAPIList[6];
	static RENDERPRIMITIVEAPI   m_pRPFullAPIList[16];
	static RENDERPRIMITIVEAPI   m_pRPFullDX5APIList[4];
	static RENDERPRIMITIVEAPI   m_pRPFullDX6APIList[14];
	static RENDERPRIMITIVEAPI   m_pRPFullDX7APIList[14];
	static RENDERPRIMITIVEAPI   m_pRPFullDX8APIList[6];
	static DWORD                m_dwRPDefaultAPIListSize;
	static DWORD                m_dwRPFullAPIListSize;
	static DWORD                m_dwRPFullDX5APIListSize;
	static DWORD                m_dwRPFullDX6APIListSize;
	static DWORD                m_dwRPFullDX7APIListSize;
	static DWORD                m_dwRPFullDX8APIListSize;

	//---------------------------------------------------------------------
	// API cycling internal functions

	virtual void        InitializeAPICycling();
};


// ----------------------------------------------------------------------------
// API test framework used only by API tests

#ifndef GROUP_DECLARE
#define GROUP_DECLARE(classname, superclass) \
class classname : public superclass\
{\
public:\
	classname();\
	bool ExecuteTest(UINT);\
}
#endif

#ifndef GROUP_CREATE
#define GROUP_CREATE(classname)\
classname tst##classname
#endif

#ifndef GROUP_BEGIN
#define GROUP_BEGIN(classname, groupname, commandkey)\
classname::classname()\
{\
	m_szTestName = groupname;\
	m_szCommandKey = commandkey;\
	m_nTestCounter = 0;\
}\
GROUP_BEGIN_EXEC(classname)
#endif

#ifndef GROUP_BEGIN_EXEC
#define GROUP_BEGIN_EXEC(classname)\
bool classname::ExecuteTest(UINT nTest)\
{\
	m_nTest = 0;\
	bool bDetectLeaks = true;\
	DWORD dwMemUsage, dwPostMemUsage;
#endif

#ifndef TEST_BEGIN
#define TEST_BEGIN(str, bRun)\
	if(nTest == 0)\
		m_nTestCounter++;\
	else\
	{\
		if(nTest == ++m_nTest)\
		{\
			BeginTestCase(str, m_nTest);\
			if(bRun != false)\
			{
#endif

#ifndef TEST_END
#define TEST_END \
			}\
			else\
				SetResult(SKIP);\
		}\
	}
#endif

#ifndef GOTO_CLEANUP
#define GOTO_CLEANUP \
				goto e_GroupCleanup;
#endif

#ifndef GROUP_CLEANUP
#define GROUP_CLEANUP \
e_GroupCleanup:
#endif

#ifndef GROUP_END
#define GROUP_END  \
	return true;\
}
#endif

#ifndef LEAK_BEGIN
#define LEAK_BEGIN(bLeak) \
			bDetectLeaks = bLeak;\
			dwMemUsage = GetMemoryUsage();
#endif

#ifndef LEAK_END
#define LEAK_END \
			dwPostMemUsage = GetMemoryUsage();\
			if(bDetectLeaks && (dwMemUsage < dwPostMemUsage) )\
			{\
				WriteToLog(_T("Memory leak detected! %d percentage leaked\n"), dwPostMemUsage - dwMemUsage );\
				Fail();\
			}
#endif

#ifndef CHECKLEAKS
#define CHECKLEAKS(x)
#endif

#define CHECKRESULT(x,y) \
	if((x) != (y))\
	{\
		WriteToLog(_T("Returned HR = %s\n"), m_pD3D->HResultToString(x));\
		Fail();\
	}\

class CApiTest: public CD3DTest
{
protected:
	bool    m_bRunInvalid;          // Indicates whether to execute invalid param tests
	bool    m_bRunCrash;            // Indicates whether to execute crashing tests
	UINT    m_nTest;                // Current test case used by macros
	UINT    m_nTestCounter;         // Counter used to total individual test cases

	DWORD           GetMemoryUsage();
	virtual UINT    CountTestCases();
	virtual bool    ParseCmdLine();
	virtual UINT    TestInitialize();

public:
	CApiTest();

	// inlining here would add dependancies to CD3D.h and CDevice.h.
	// there is no reason to make these virtual, either.
	LPDIRECT3D8 GetD3D8();
	LPDIRECT3D9 GetD3D9();
	LPDIRECT3D9EX GetD3D9Ex();
	// Default of Device n=0 maps to SrcDevice
	LPDIRECT3DDEVICE8 GetDevice8(UINT n=0);
	LPDIRECT3DDEVICE9 GetDevice9(UINT n=0);
	LPDIRECT3DDEVICE9EX GetDevice9Ex(UINT n=0);
	// Unwrap various FMWK objects
	LPDIRECT3DSURFACE9 GetResource9(CSurface *pResource);
	LPDIRECT3DTEXTURE9 GetResource9(CTexture *pResource);
	LPDIRECT3DCUBETEXTURE9 GetResource9(CCubeTexture *pResource);
	LPDIRECT3DVOLUMETEXTURE9 GetResource9(CVolTexture *pResource);
	LPDIRECT3DVOLUME9 GetResource9(CVolume *pResource);
};

// ----------------------------------------------------------------------------
// Manual test framework used mostly by D3DX

class CManualTest : public CApiTest
{
	static INT_PTR CALLBACK TestFailureProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

protected:  
	virtual void    SetTestType(TESTTYPE Type);
	virtual void    ExecuteOneTest();
	virtual HRESULT ExecuteTests();
	virtual HRESULT Render();
	virtual bool    PostModeSetup();
	virtual bool    SetupArcBall();
	virtual LRESULT OnMouse(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void    OnPaint();
	virtual HRESULT Resize(UINT uWidth, UINT uHeight);
	virtual void    BeginTestCase(LPCTSTR szTestCase, UINT uTestNumber);

public:
	CD3DArcBall     m_ArcBall;
	bool            m_bRenderOnPaint;

	CManualTest() { m_bRenderOnPaint = true; }
};

#endif

