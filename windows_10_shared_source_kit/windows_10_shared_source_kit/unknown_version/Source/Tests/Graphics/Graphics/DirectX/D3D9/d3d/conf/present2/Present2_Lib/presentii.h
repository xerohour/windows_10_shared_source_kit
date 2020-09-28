// ColorConvertingPresent.h: interface for the CPresentII class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <vector>
#include "resource.h"

#include "CSurface.h"
#include "CBuffers.h"
#include "CTextures.h"
#include "CSwapChain.h"
#include "CD3DTest.h"
#include "Node.h"

#include <dwmapi.h> 

using namespace std;
#pragma warning( disable : 4786 4503 )
typedef vector<int> VECINT;

//
//  Keep in sync with D3DKMT_MAX_PRESENT_HISTORY_RECTS in d3dkmthk.h
//
#define LOCAL_D3DKMT_MAX_PRESENT_HISTORY_RECTS 16

//
// Keep in sync with c_maxDirtyRegions in windownode.h 
//
#define LOCAL_c_maxDirtyRegions  8

#define DEFAULT_WINDOW_WIDTH    256
#define DEFAULT_WINDOW_HEIGHT    256
#define MAX_SWAPCHAINS 2

typedef enum
{
    TEST_PRESENT,
    TEST_COLOR_CONVERSION,
    TEST_DIRTYREGION,
    TEST_SETDIALOGBOXMODE,
    TEST_CLIPPINGWINDOWS,
    TEST_GAMMA,
    TEST_FULLSCREENPRESENT,
    TEST_DEVICECLIP,
} TEST_TYPE;

typedef enum
{
    ReleaseDevice,
    ReleaseSwapChains,
    NoRelease,
} DeviceReset;

const float    c_Scale[]    = { 0.0f, 1.0f, 0.5f, 2.0f, 0.66f, 1.5f, 0.1f };    // NULL, same, half, double, 1/3, 1.5, slice
const RECT    c_Offsets[]    = 
{
    // left        top            right        bottom
    { 25,        25,            75,            75 },         // centered
    {  0,         0,           100,            100 },        // origin
    {-25,         0,            75,            100 },        // offset left
    { 25,         0,           125,            100 },        // offset right
    {  0,       -25,           100,             75 },        // offset top
    {  0,        25,           100,            125 },        // offset bottom
    {-25,       -25,            75,             75 },        // offset top-left
    { 25,       -25,           125,             75 },        // offset top-right
    {-25,        25,            75,            125 },        // offset bottom-left
    { 25,        25,           125,            125 },        // offset bottom-right
};

typedef struct
{
    DWORD Style;
    DWORD RestoreStyle;
    DWORD ExStyle;
    DWORD RestoreExStyle;
    RECT RestoreRect;
    HWND Hwnd;
    bool IsTestWindow;
    bool IsRegionWindow;
    HRGN RestoreHrgn;
    bool IsLayeredWindow;
    bool CreateDevice;
    DWORD SwapEffect; 

    CDevice *pDevice;
} WindowData;


typedef struct
{
    bool bAbsoluteRect;    // true if using the rect member
    int indexXScale;    // index in the scale array
    int indexYScale;    // index in the scale array
    int indexOffset;    // index in the offset array
    RECT rect;
} RegionDescription;

typedef struct
{
    HWND hDlg;            // dialog handle
    bool bAbsolutePosition;    // true if using the rect member
    int indexXScale;    // index in the scale array
    int indexYScale;    // index in the scale array
    int indexOffset;    // index in the offset array
    RECT rect;
} DialogBoxDescription;

typedef union
{
    struct
    {
        BYTE blue;
        BYTE green;
        BYTE red;
        BYTE alpha;
    };
    DWORD color;
} MYD3DCOLOR;
const MYD3DCOLOR COLOR_ERASE                    = {0xde, 0xad, 0xbe, 0xef};
const MYD3DCOLOR COLOR_BGROUND                    = {0x80, 0xC0, 0x40, 0xFF};
const MYD3DCOLOR COLOR_BLACK                    = {0x00, 0x00, 0x00, 0x00};

typedef struct
{
    float x, y, z;    // The transformed position for the vertex.
    float tu, tv;    // Texture coordinates
} CCVERTEX;
#define D3DFVF_CCVERTEX ( D3DFVF_XYZ | D3DFVF_TEX1 )

// monitor / adapter information
#define MAXADAPTERS    9

typedef struct
{
    UINT uAdapter;
    HMONITOR hMonitor;
    RECT rcMonitor;
    bool bPrimaryMonitor;
    DISPLAYMODE m_modeDisplay;
    RECT m_rectFrontBuffer; // frontbuffer rect in the desktop
    RECT m_rectFrontBufferUnclipped; // the unclipped frontbuffer rect in the desktop; might span multiple monitors
    LPSURFACE m_pFrontBuffer; // the frontbuffer
    LPSURFACE m_pClearBuffer; // the frontbuffer clipped to the window, with clear only color
    CDevice *m_pDevice;
    CSwapChain *m_pSwapChain;
} ADAPTERMONITORINFO;

typedef struct
{
    LPSURFACE m_vecBB[D3DPRESENT_BACK_BUFFERS_MAX];
    LPSURFACE m_pSourceSurface;    // visible part of the backbuffer will be copied here, the rest filled with 0
    RECT m_rectSource; // the visible source surface rectangle
    RECT m_rectFrontBufferWindow;// only test window
    RECT m_rectPresentSource;
    RECT m_rectPresentDest;
    RECT m_rectDPIPresentDest;
    RECT m_rectDest; // the rectangle of the visible dest surface
    LPSURFACE m_pDestSurface; // the source surface will be copied here with stretch/colorconversion
    RECT m_rectDestVisible; // the dest rectangle on the screen
    RECT m_rectD3DXRef; // rectangle of the destination image in the D3DX ref surface, clipped
    LPSURFACE m_pD3DXRefBuffer;    // D3DX reference buffer; this will be compared to the frontbuffer
    LPSURFACE m_pDiffSurface; // erroneous pixels will be stored here
} SWAPCHAINDATA;

class CPresentII : public CD3DTest
{
public:
    CPresentII();
    virtual ~CPresentII();

    virtual void CommandLineHelp();                   // command line help for the test
    virtual bool CapsCheck();
    virtual UINT TestInitialize();                    // Init the testcase
    virtual bool TestTerminate();                     // cleanup
    virtual bool ExecuteTest(UINT nTest);             // Execute the testcase
    virtual bool ClearFrame(void) {return true;}      // DOES NOTHING! Clears the frame to the default color
    virtual bool IsDeviceLost();

protected:
    // general variables
    bool                    m_bDWMEnabled;             // true if DWM is enabled; checked after each device reset
    TEST_TYPE               m_TestType;                // test type
    UINT                    m_nTests;                  // total number of tests
    UINT                    m_nTestId;
    HWND                    m_hWndReset, m_hWndOverride, m_hWndPresent;
    LPVERTEXBUFFER          m_pVB;                     // vertex buffer
    CDevice                 *m_pRenderingDevice;       //deleteable clone of m_pSrcDevice
    CPShader                *m_pPShader;

    LPTEXTURE               m_pTexture;                // texture used to fill the back buffer
    bool                    m_bSnapshot;               // true if DisplayFrame is called by MY test
    bool                    m_bCanRender;              // true if drawing is possible (streams and textures set)
    DWORD                   m_SwapEffect;
    vector<DialogBoxDescription> m_vecWindowDescription;// clipper window descriptions
    MYD3DCOLOR                m_COLOR_ERASE, m_COLOR_WINDOW;    // color to erase background / color of clipping window
    float			m_fGamma;							// usually set to 2.2


    // swapchains
    UINT                    m_uNumberSwapChains;
    SWAPCHAINDATA           m_SwapChainData[MAX_SWAPCHAINS];

    // these indexes control the current test execution
    UINT                    m_nIndexPrivateData;
    UINT                    m_nIndexPresentParameters;
    UINT                    m_nIndexPresentWindow;
    UINT                    m_nIndexTexture;
    UINT                    m_nIndexRectSource;
    UINT                    m_nIndexRectDest;
    UINT                    m_nIndexDirtyRegion;
    UINT                    m_nIndexDialogs;
    UINT                    m_nIndexFlagsEx;
    UINT                    m_nIndexDeviceReleaseBeforeVerify;
    UINT                    m_nIndexWindowTrees;

    UINT                    m_nLastIndexPresentParameters;
    UINT                    m_nLastIndexTexture;
    DeviceReset             m_LastDeviceReleaseBeforeVerify;
    UINT                    m_nLastIndexWindowTrees;

    // front buffer variables
    vector<ADAPTERMONITORINFO> m_vecAdapterMonitors;    // store front buffer data for each adapter/monitor (usually just 1)
    DWORD                    m_dwWindowWidth;        // window width
    DWORD                    m_dwWindowHeight;        // window height
    
    // Present parameters
    RECT                     *m_prectPresentSource, *m_prectPresentDest;
    float                    m_fDPIFactor;
    RGNDATA                  *m_pDirtyRegion, *m_pDestDirtyRegion;

    UINT                    m_nIndexBB;

    // testcase input parameters
    vector<int>                 m_vecPrivateData;         // gives the test a chance to add custom parameters
    vector<PRESENT_PARAMETERS>  m_vecPresentParameters;   // present parameters
    vector<UINT>  m_vecPresentParametersRotation; //rotation info for present parameters
    vector<HWND>                m_vecPresentWindow;       // present window
    vector<string>              m_vecStringTexture;       // texture path (if any)
    vector<RECT>                m_vecRectPresentSource, m_vecRectPresentDest;    // present rectangles
    vector<RGNDATA*>            m_vecRegData;
    vector<VECINT>              m_vecIntWindows;    // clip windows
    vector<DWORD>               m_vecFlagsEx;
    vector<DeviceReset>         m_vecDeviceReleaseBeforeVerify;
    vector<Node<WindowData>>    m_vecWindowTrees;
    
    // presentation parameters

    // test config parameters
    int                      m_iConformance;            // conformance percentage
    DWORD                    m_d3dxFilter;            // d3dx filter
    
    // testcase presentation parameters
    FORMAT                    m_fmtFrontBuffer;        // frontbuffer format

    // testcase output parameters
    MYD3DCOLOR              m_ColorBalance;                // makes sure the image is balanced, low tolerance
    float                   m_fLocalConformance;        // local conformance, measured by the errors between the same pixels
    float                   m_fOverallRedConformance;    // RED conformace percentage between front buffer and D3DX ref surface
    float                   m_fOverallGreenConformance;    // GREEN conformace percentage between front buffer and D3DX ref surface
    float                   m_fOverallBlueConformance;    // BLUE conformace percentage between front buffer and D3DX ref surface
    bool                    m_bPreserveBackground;        // true if Present preserved the background outside RectDest; if false, behavior is wrong
	float                   m_fEraseRedConformance;    // RED conformace percentage between front buffer and clear surface
    float                   m_fEraseGreenConformance;    // GREEN conformace percentage between front buffer and clear surface
    float                   m_fEraseBlueConformance;    // BLUE conformace percentage between front buffer and clear surface

    // misc parameters
    bool                    m_bDontHideCursor;            // don't hide the mouse cursor
    bool                    m_bQuitOnFailure;             // quit if the failure algorithm fails
    bool                    m_bSaveAll;                   // save bitmap images for all failures.
    bool                    m_bTestFailed;                // one test has failed
    CAPS                    m_deviceCaps;                 // device caps
    OSVERSIONINFOEX         m_osVersionInfoEx;            // OS Version info

    // virtual functions to be implemented by each testcase
    virtual char*            Name( char* szName ) = 0;       // return current test name
    virtual void            InitPrivateData();               // build the vector of special data
    virtual void            InitDirtyRegions();              // build the vector of regions to test
    virtual void            InitWindows();                   // build the vector of dialog boxes to test
    virtual void            InitTextures();                  // build the vector of textures to test
    virtual void            InitPresentParameters();         // build the vector of present parameters to test
    virtual void            InitPresentWindows();            // build the vector of Present windows to test
    virtual void            InitPresentSourceRect();         // build the vector of present source rects to test
    virtual void            InitPresentDestRect();           // build the vector of present dest rects to test
    virtual void            InitFlagsEx();                   // LONGHORN ONLY: build the vector of Ex flags to test
    virtual void            InitDeviceReleaseBeforeVerify(); // Used by FlipEx Tests to validate DWM frame reuse after device change.
    virtual void            InitWindowTrees();
    virtual void            Present();                       // implementation of Present (either using the device's shortcut or swapchain's method)
    virtual void            ProcessREFBuffer( UINT uSwapChain);    // Do any postprocessing to the REF buffer (like gamma correct it)
    virtual bool            PixelInsideDialogs( POINT Pixel );        // true if the pixel is inside Dialogs
    virtual void            PrivateDataProcessing();    // process private data case before rendering
    virtual void            AfterReleaseDeviceBeforeVerify();    // called by test, so it can force a dwm repaint
    virtual void            ChangeDisplaySettings();    // change the mode if necessary
    virtual void            FilterLowResolutionModes();
    virtual void            GetHighLowResolutionModes();
    virtual DWORD           GetDebugPause();


    
    void ClearAdapterMonitors();
    UINT InitAdapterMonitors();
    
    // virtual rendering methods
    virtual bool            ClearBackBuffers();            // true if the backbuffer(s) should be cleared
    virtual bool            UpdateBackBuffer();            // true if the backbuffer contents should be updated in the REF image
    virtual void            RenderScene( UINT uSwapChain, int indexBB );    // renders the scene

    // overwritten
    virtual bool    ResetLostDevices() { return false;};            // don't handle lost device; fmwk gets stuck in HandleLostDevice is TestCooperativeLevel doesn't return the expected code
    bool            DisplayFrame();                // override present call
    bool            ProcessFrame();                // process the frame
    LRESULT         OnSetCursor(WPARAM wParam, LPARAM lParam);    // prevent mouse cursor

    // protected methods
    HRESULT         GetFrontBufferRect(); // get window and monitor info to determine the backbuffer position on the desktop
    HRESULT         CompareSurfaces( UINT uSwapChain, LPRGNDATA pRegionData );    // compare the front buffer with the reference d3dx surface
    void            HideWindow( CWindow* pWindow, HWND donotHideThisWindow );    // hide the window
    void            PrepareNextTest();

    // helper methods
    bool            IsDWMEnabled();                                            // returns true if DWM is enabled, false otherwise
    void            Check_HRFail( HRESULT hResult, const char* sMethod );    // if hResult is not D3D_OK, throws an exception
    HRESULT         HandlePresentReturnCode( HRESULT hrPresent );            // handle the results from Present and PresentEx
    FORMAT          SuppressAlphaChannel( FORMAT fmtWithAlpha );            // converts alpha formats into non-alpha formats
    bool            FailTestCase();                                            // fail & end the testcase
    RECT            CreateRect( int nXScale, int nYScale, int nOffset );    // build a rect from scale and offset constants
    RECT            CreateRect( int left, int top, int right, int bottom );    // build a rect from the parameters
    RECT            CreateRect( RECT rectScale, int nWidth, int nHeight );    // scale the width and height and offset by the rect parameter
    bool            CheckDeviceFormat( FORMAT fmt );                        // check if the adapter supports backbuffer fmt
    RGNDATA*        CreateDirtyRegion( vector<RegionDescription>& vecRegions );    // create a region from the parameters
    void            InitPresentRects( vector<RECT>& vecRects );                // build the vector of rects to test
    void            InitBackBufferRects( vector<RECT>& vecRects, int nWidth, int nHeight );
    void            Dump();                                                    // dump the current config (with results)
    char*           FORMAT2STRING( D3DFORMAT fmt );                            // convert D3Dformat 2 string
    HRESULT         SWAPEFFECT2STRING( DWORD swapEffect, __in_ecount(size) PTSTR s, UINT size);
    HRESULT         D3DX_FILTER2STRING( DWORD swapEffect, __in_ecount(size) PTSTR s, UINT size);
    HRESULT         DeviceReset2String( DWORD swapEffect, __in_ecount(size) PTSTR s, UINT size);
    int             round( float f );                                        // rounds the float to the closest int
    bool            PixelInsideRegion( UINT uSwapChain, POINT Pixel );        // true if the pixel is inside region (or present dest if region is null)
    bool            PixelInside(POINT Pixel, LPRGNDATA pRegionData ); // true if the pixel is inside region
    bool            PixelOutsideRegion( UINT uSwapChain, POINT Pixel );        // true if the pixel is outside region (or present dest if region is null)
    bool            GrabEraseColor();
    bool            FillAdapterMonitorInfo( CDevice *pDevice, UINT uAdapterOffset, ADAPTERMONITORINFO *pAdapterMonitorInfo );    // gathers adapter information
    DWORD           GetGDIDisplayOrientation();
    DWORD          GetGDIDisplayOrientation(UINT adapterID);
    HRESULT         SetTaskBarState(bool alwaysOnTop);
    bool            IsTaskBarAlwaysOnTop();
    bool            CanUseFlipEx();
    HRESULT         CreateAndCopySurfaceFromSurface(LPSURFACE pSourceSurface, CDevice *pDestinationDevice, LPSURFACE *ppDestinationSurface);
    HRESULT         BuildWindowTree(HWND root,HWND testWindow, Node<WindowData>*);
    HRESULT         ResetWindowTreeRegions(Node<WindowData>* pNode);
    HRESULT         MoveWindowTree(Node<WindowData>* pNode, int, int);
    HRESULT         PumpMessagesWindowTree(Node<WindowData>* pNode);
    Node<WindowData>* FindTestWindowInWindowTree(Node<WindowData>* pNode, bool *pFound);
    HRESULT         CleanUpWindowTree(Node<WindowData>*);
    HRESULT         ResetTestWindow(Node<WindowData>*);
    HRESULT         ResetPosition(Node<WindowData>*);
    HRESULT         CleanUpDevicesAndWindows(Node<WindowData>*);
    HRESULT         CopyClippedRegion(RGNDATA* pSrcRgn, RECT srcRect,RECT srcSubRect, LPSURFACE pSrcSurface, RECT destRect, LPSURFACE pDstSurface, DWORD d3dxFliter);
    bool            IsRestrictToMonitor(UINT swapChain, UINT adapter, UINT indexPresentParameters);
    bool            IsDeviceClip(UINT swapChain, UINT adapter, UINT indexPresentParameters);
    HRESULT         FillSubRectColor(LPSURFACE pDstSurface, RECT textureSize, RECT subRect, DWORD color);
    HRESULT         GetWindowRegionData(HWND hwnd, HRGN *pRgn, LPRGNDATA *pRegionData);
    HRESULT         SaveBuffers();
    void            AddGammaToRef(UINT uSwapChain);

};

