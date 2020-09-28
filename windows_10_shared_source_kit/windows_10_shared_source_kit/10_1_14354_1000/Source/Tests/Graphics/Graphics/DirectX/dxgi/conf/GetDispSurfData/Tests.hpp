//
// Tests.hpp
//
// Test GetDisplaySurfaceData to ensure that pixel format conversion is done correctly.
//
// Class declarations: CTests

#pragma once


//
// Project headers
//

#include <initguid.h>
#include "GetDispSurfData.hpp"


// #defines converted to constants for easy debugging
const UINT  c_nBACK_BUFFER_COUNT = 2;   
const UINT  c_nFORMAT_MAX = 7;          // color formats to be tested
const UINT  c_nMODE_MAX = 2;            // we test two screen mode: lowest and highest testable resolutions
const UINT  c_nMAX_PIXEL_ERRORS = 10;

typedef struct tagLOCATION {
    UINT    i;
    UINT    j;
} LOCATION, * PLOCATION;

enum PIXEL_POSITION { EXACT, NEARBY };

tstring __stdcall ToString( const IID &iid );
bool __stdcall FromString( IID *piid, const tstring &iid );

enum TEST_DXGI_SWAP_CASE
{
    TEST_DXGI_SWAP_CASE_PRESENT,
    TEST_DXGI_SWAP_CASE_GETDISPSURFDATA,
    TEST_DXGI_SWAP_CASE_MAP,
    TEST_DXGI_SWAP_CASE_DEFAULT
};
//

class CTests
    :
    public CDXGITest,
    public clsBasicWindowClass
{
public:
    CTests();
    ~CTests();

protected:
    // WGFramework overrides
    //
    bool  InitializeTestGroup();
    void  CleanupTestGroup();
    void InitTestParameters();
    TEST_RESULT Setup();
    TEST_RESULT  ExecuteTestCase();
    void Cleanup();

private:
    // Internal Methods
    //

    // Enumerate all the display devices
    TEST_RESULT  EnumerateDisplayDevices();

    // For each display device and each display format, enumerate all the display modes 
    TEST_RESULT
        DXGIStoreSelectedModes(
        __in    IDXGIOutput*    pTempDXGIOutput,
        __in    UINT            nAdapter,
        __in    UINT            nMonitor
        );

    // Before running a test case, create the D3D9 and DXGI objects
    TEST_RESULT  CreateInitialDXObjects();


    TEST_RESULT
        GetDXGIAdapterPtr( 
        __inout     IDXGIAdapter**   ppAdapter
        );

    TEST_RESULT
        GetDXGIOutputPtr( 
        __inout     IDXGIOutput** ppOutput
        );

    // Extract display device name and mode info
    TEST_RESULT
        GetMiscOutputData(
        __in        IDXGIOutput*    pOutput,
        __inout     DXGI_OUTPUT_DESC&  outputDesc,  // by ref
        __inout_ecount(nDevNameLen)  PCHAR  pszDevName,
        __in        UINT            nDevNameLen,
        __inout     DEVMODE&        devMode         // by ref
        );

    // Set up the swap chain for the device
    TEST_RESULT
        DXGISetupSwapChain(
        __in    UINT            nAdapter,
        __in    UINT            nMonitor,
        __in    DEVMODE&        devMode,            // by ref
        __in __nullterminated  PCHAR  pszDevName
        );

    TEST_RESULT
        PresentAndValidate(
        __in    UINT            nAdapter,
        __in    UINT            nMonitor
        );

    HRESULT
        CreateManualSysmemTexture(
        __in    UINT        nAdapter,
        __in    UINT        nMonitor,
        __in    UINT        width,
        __in    UINT        height,
        __in    DXGI_FORMAT nFormat,
        __in    float       flColorIntensity,
        __inout ID3D10Texture2D **ppManualValidationSysMemTex
        );

    TEST_RESULT
        FuzzyCompareDXGISurfaces(
        __in    const DXGI_FORMAT   nFormat,
        __in    const DXGI_SURFACE_DESC&  testSurfDesc,
        __in	const DXGI_SWAP_CHAIN_DESC& scDesc, 
        __in	const DXGI_MAPPED_RECT mappedTestBits,
        __in	const D3D10_MAPPED_TEXTURE2D mappedSysMemTex2D
        );

    TEST_RESULT
        CompareDXGIPixel(
        __in    PIXEL_POSITION  npixelPos,
        __in    const void*     pTestPixel,
        __in    const void*     pValidationPixel,
        __in    const DXGI_FORMAT  nFormat,
        __in    const DXGI_FORMAT  nBackBufferFormat,
        __inout_ecount(4) char  szMsg[4][c_nMSG_LEN]
    );

    TEST_RESULT
        CompareDXGIPixel(
        __in    PIXEL_POSITION  npixelPos,
        __in    const void*     pTestPixel,
        __in	const BYTE** pValidationPixels,
        __in	UINT			nBytesPerPixel,
        __in	UINT			nNumValidationPixels,
        __in    const DXGI_FORMAT  nFormat,
        __in    const DXGI_FORMAT  nBackBufferFormat,
        __inout_ecount(4) char  szMsg[4][c_nMSG_LEN]
    );

    BOOL  ReleaseLocalObjects(
        __inout  IDXGISurface*&             pTestSurface,
        __inout  ID3D10Resource*&           pValidationVidMemRes,
        __inout  ID3D10Resource*&           pValidationSysMemRes,
        __inout  ID3D10Texture2D*&          pValidationVidMemTex, 
        __inout  ID3D10Texture2D*&          pValidationSysMemTex,
        __inout  ID3D10Texture2D*&          pManualValidationSysMemTex
        );

    void  DeleteD3D10SceneObjects();
    BOOL  ReleaseTestCaseObjects();
    BOOL  ReleaseDXGITestCaseObjects();

    // Framework combinatorial vars
    int     m_nPrimaryOutputNum;
    int     m_nModeIndex;
    bool    m_bUseDWM;
    bool    m_bUseFullscreen;
    IID m_EmulatedInterface;
    TEST_DXGI_SWAP_CASE m_case;


    // Variables
    int             m_nNumMonitors;
    BOOL            m_fGotOriginalDevMode[MONITORS_MAX];
    DISPLAY_DEVICE  m_originalDispDev[MONITORS_MAX];
    DEVMODE         m_originalDevMode[MONITORS_MAX];
    BOOL            m_bOriginalDWMState;

    DXGI_FORMAT     m_driverSupportedFormats[c_nFORMAT_MAX];
    int             m_nNoOfDriverSupportedFormats;

    DXGI_FACTORY_POINTER  m_FactoryPointer;

    // There shouldn't be more adapters than monitors.
    IDXGIAdapter   *m_pAdapter[MONITORS_MAX];

    // There might be 10 adapters with one monitor each, or theoretically one adapter with 10 monitors.
    // Allow for both extremes.
    IDXGIOutput    *m_pOutput[MONITORS_MAX][MONITORS_MAX];

    // Cache the location (i, j) of each output.
    LOCATION        m_location[MONITORS_MAX];

    // Store up to c_nMODE_MAX modes for each attached monitor or Output.
    // c_nMODE_MAX is a small selection of commonly-used and edge-case modes.
    DXGI_MODE_DESC  m_ModeToTest[MONITORS_MAX][MONITORS_MAX][c_nFORMAT_MAX * c_nMODE_MAX];

    HWND            m_hwndDevWnd[MONITORS_MAX][MONITORS_MAX];

    DXGI_SWAP_CHAIN_DESC    m_swapChainDesc[MONITORS_MAX][MONITORS_MAX];
    ID3D10Device   *m_pD3D10Device[MONITORS_MAX][MONITORS_MAX];
    IDXGIDevice    *m_pDXGIDevice[MONITORS_MAX][MONITORS_MAX];
    IDXGISwapChain *m_pSwapChain[MONITORS_MAX][MONITORS_MAX];
    int             m_nWarningsCount;

    // Displays a D3D10 2D image using 3D on each monitor.
    D3D10Scene     *m_pD3D10Scene[MONITORS_MAX][MONITORS_MAX];

    // Draws a D3D10 2D image using 3D on a VidMem Surface.
    D3D10VidSurface     *m_pD3D10VidSurface[MONITORS_MAX][MONITORS_MAX];
};

