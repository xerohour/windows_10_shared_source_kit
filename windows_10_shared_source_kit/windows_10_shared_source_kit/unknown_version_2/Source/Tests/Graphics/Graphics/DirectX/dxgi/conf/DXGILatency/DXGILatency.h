#ifndef DXGIPRESENT_H_INCLUDED
#define DXGIPRESENT_H_INCLUDED


//
// Win32 headers
//

#include <windows.h>
#include <basetsd.h>


//
// Direct3D headers
//

#include <d3d10_1.h>
#include <dxgi.h>
#include <d3dx10.h>


//
// STL headers
//

#include <vector>


//
// Framework headers
//

#include <WGFTestCore.h>
#include <WGFTestUI.h>
#include <DXGITest.h>


//
// Project headers
//
#include "DCOMPIsolation.h" // Isolates Win8 DCOMP code so we can still compile the test to run on Win7.

#include "DXGIBufferCompare.h"

#include "DXGIWndClass.h"
#include "DXGISurfUtils.h"
#include "D3D10RefUtil.h"

#include "D3D10SwapChain.h"

#include "GetDeviceInfo9.h"

#include "DXGIBBFMT.H"
#include "DXGIFMT.h"
#include "DXGIValueSets.h"
//#include "DXGISwapEffects.h"


#define MAX_BUFFERS 16
#define NUM_TEXTURES (MAX_BUFFERS + 1)

#define DEFAULT_FRAME_LATENCY 3

extern DXGI_MODE_DESC TempFSModes[];
extern const UINT cuiNoOfTempFSModes;

#define MINIMUM(a,b)  (a<b ? a : b)
#define MAXIMUM(a,b)  (a>b ? a : b)

enum TestType
{
    DEFAULT_TESTS,
    SET_TESTS,
    SECOND_DEVICE_CREATED_BEFORE_SET_TESTS,
    SECOND_DEVICE_CREATED_AFTER_SET_TESTS,
};


class clsTestableDevice
    :
    public clsD3D10Device
{
    //
    // Construction/destruction
    //

public:
    clsTestableDevice(CTest *pInitLoggingGroup)
        :
    clsD3D10Device(pInitLoggingGroup)
    {
    };

    virtual ~clsTestableDevice();


    //
    // Public methods
    //

public:
    bool GetDevicePtr(ID3D10Device **pDevicePointer);
};


class clsTestableDXGISwapChain
    :
    public clsDXGISwapChain
{
    //
    // Construction/destruction
    //

public:
    clsTestableDXGISwapChain
        (
        CTest * pInitLoggingGroup,
        clsD3D10Device * pInitParentD3D10Device
        )
        :
    clsDXGISwapChain
        (
        pInitLoggingGroup,
        pInitParentD3D10Device
        )
    {
    };

    virtual ~clsTestableDXGISwapChain();


    //
    // Public methods
    //

public:
    bool GetSwapChainPtr
        (
        IDXGISwapChain ** pSwapChainPointer
        );
};


class CDXGILatency
    :
    public CDXGITest,
    public clsBasicWindowClass,
    public CTestDXGISurfaceUtils,
    public clsD3D10Ref //, public CTestBasicD3D10
{
    //
    // Construction/destruction
    //

public:
    CDXGILatency();

    virtual ~CDXGILatency();


    //
    // Framework override methods
    //

protected:
    bool InitializeTestGroup();

    void CleanupTestGroup();

};


class CDXGILatencySingleSC
    :
    public CDXGILatency
{
    //
    // Construction/destruction
    //

public:
    CDXGILatencySingleSC();

    virtual ~CDXGILatencySingleSC();


protected:
    static const DXGI_FORMAT s_dxgifmtBBInitTexture;

protected:
    int m_iNumBuffersIndex;
    int m_iMaxFrameLatencyIndex;

    clsTestableDevice m_PrimaryAdapterDevice;
    ID3D10Device * m_pD3D10Device;
    HWND m_hwndFocusWindow;
    HWND m_hwndDeviceWindow;
    IDXGIFactory * m_pFactory;
    DXGI_SWAP_CHAIN_DESC m_SCDesc;
    clsTestableDXGISwapChain m_TestedSwapChain;
    IDXGISwapChain * m_pSwapChain;

    // Simplified information about the output and the relationship between the device
    // window and the output.
    //
    // To do: This will have to be revised substantially to accommodate test cases involving
    // multiple outputs and multiple adapters.

    IDXGIOutput * m_pDeviceWindowOutput;
    UINT m_uiDeviceOutputWidth;
    UINT m_uiDeviceOutputHeight;
    RECT m_rectDeviceWindow;

public:
    bool UpdateOutputWindowInformation();

    void ReleaseTestCaseObjects();
};


class CWindowedTestCases
    :
    public CDXGILatencySingleSC
{
    //
    // Construction/destruction
    //

public:
    CWindowedTestCases();

    virtual ~CWindowedTestCases();


    //
    // Helper methods
    //

protected:
    TEST_RESULT CreateDeviceAndSwapChain
        (
        bool bDWM,
        int iBBFormatIndexArg,
        int iSwapEffectIndexArg,
        int iNumBuffersIndexArg
        );
};


class CFSTestCases
    :
    public CDXGILatencySingleSC
{
    //
    // Construction/destruction
    //

public:
    CFSTestCases();
    virtual ~CFSTestCases();

protected:
    std::vector<DXGI_MODE_DESC> TestedModes;
    TestType m_TestType;
    DXGI_SWAP_EFFECT m_SwapEffect;
    DXGI_FORMAT m_PixelFormat;
    int m_nSyncInterval;  // Test Case Parameter - Sync interval

    int m_iModeIndex;
    int m_iLastModeIndex;

    TEST_RESULT CreateDeviceAndSwapChain
        (
        int iModeIndexArg,
        DXGI_SWAP_EFFECT swapEffect,
        int iNumBuffersIndexArg
        );

    HRESULT SetMaximumFrameLatency(
        ID3D10Device* pD3D10Device,
        UINT value);

    TEST_RESULT VerifyGetMaximumFrameLatency(
        ID3D10Device* pD3D10Device,
        UINT expectedValue);

public:
    TEST_RESULT TestLatency(UINT uiMaxLatency);
    virtual TEST_RESULT ExecuteTestCase();
};


class CDefaultLatencyFS
    :
    public CFSTestCases
{
    //
    // Construction/destruction
    //

public:
    CDefaultLatencyFS();
    virtual ~CDefaultLatencyFS();


    //
    // Framework override methods
    //

public:
    virtual TEST_RESULT Setup();

    virtual void InitTestParameters();
    virtual void Cleanup();
};

class CSetMaximumFrameLatencyFS
    :
    public CDefaultLatencyFS
{
    //
    // Construction/destruction
    //

public:
    CSetMaximumFrameLatencyFS();
    virtual ~CSetMaximumFrameLatencyFS();


public:
    virtual TEST_RESULT Setup();
    virtual void InitTestParameters();
};


class CVerifySecondDeviceUnaffected
    :
    public CFSTestCases
{
    //
    // Construction/destruction
    //

public:
    CVerifySecondDeviceUnaffected();
    virtual ~CVerifySecondDeviceUnaffected();


public:
    virtual TEST_RESULT Setup();
    virtual void InitTestParameters();
};

class CWinTestCases
    :
    public CDXGILatencySingleSC
{
    //
    // Construction/destruction
    //

public:
    CWinTestCases();
    virtual ~CWinTestCases();

protected:
    TestType m_TestType; // Test Case Parameter - Test Case type (DEFAULT Latency or Custom set)
    DXGI_SWAP_EFFECT m_SwapEffect; // Test Case Parameter - Swap Effect
    DXGI_FORMAT m_PixelFormat; // Test Case Parameter - Pixel Format
    int m_nSyncInterval;  // Test Case Parameter - Sync interval
    bool m_tp_bWindowless; // Test Case Parameter - Bool indicating whether we should use DCOMP (true) or traditional windowed presents (false).

    CDCompIsolation m_DCOMPIsolation;

    TEST_RESULT CreateDeviceAndSwapChain
        (
        DXGI_FORMAT,
        DXGI_SWAP_EFFECT swapEffect,
        unsigned int uNumBuffers
        );

    HRESULT SetMaximumFrameLatency(
        ID3D10Device* pD3D10Device,
        UINT value);

    TEST_RESULT VerifyGetMaximumFrameLatency(
        ID3D10Device* pD3D10Device,
        UINT expectedValue);

    void InitCommonTestParameters();
private:
    HRESULT PresentAndWaitForPresentCount(
        IDXGISwapChain *swapchain,
        UINT currentPresentCount,
        DXGI_FRAME_STATISTICS* outStats);

public:
    TEST_RESULT TestLatency(UINT uiMaxLatency);
    bool IsWindows8();
    virtual TEST_RESULT ExecuteTestCase();
};

class CDefaultLatencyWindowed
    :
    public CWinTestCases
{
    //
    // Construction/destruction
    //

public:
    CDefaultLatencyWindowed();
    virtual ~CDefaultLatencyWindowed();


    //
    // Framework override methods
    //

public:
    virtual TEST_RESULT Setup();

    virtual void InitTestParameters();
    virtual void Cleanup();
};

class CSetMaximumFrameLatencyWindowed
    :
    public CDefaultLatencyWindowed
{
    //
    // Construction/destruction
    //

public:
    CSetMaximumFrameLatencyWindowed();
    virtual ~CSetMaximumFrameLatencyWindowed();


public:
    virtual TEST_RESULT Setup();
    virtual void InitTestParameters();
};

class CDoNotWaitTest
    :
    public CDefaultLatencyWindowed
{
    //
    // Construction/destruction
    //

public:
    CDoNotWaitTest();
    virtual ~CDoNotWaitTest();


public:
    virtual TEST_RESULT Setup();
    virtual void InitTestParameters();
    TEST_RESULT ExecuteTestCase();
};

#endif
