#ifndef DXGIPRESENTRATE_H_INCLUDED
#define DXGIPRESENTRATE_H_INCLUDED


//
// Win32 headers
//

#include <windows.h>
#include <basetsd.h>
#include <mmsystem.h>


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
#include <D3DWrappers.h>


//
// Project headers
//

#include "DXGIBBFMT.H"
#include "DXGIFMT.h"
#include "DXGIWndClass.h"
#include "DXGISurfUtils.h"
#include "DXGIValueSets.h"
#include "DXGIScanoutFormats.h"
#include "DXGISwapEffects.h"
#include "D3D10RefUtil.h"
#include "D3D10SwapChain.h"
#include "DeviceWrapper10.h"
#include "DCOMPIsolation.h" // Isolates Win8 DCOMP code so we can still compile the test to run on Win7.

using namespace testfactor;

#define MAX_BUFFERS 4
#define MAX_DISPLAYMODES 255

extern DXGI_MODE_DESC TempFSModes[];
extern const UINT cuiNoOfTempFSModes;


typedef CDeviceWrapper10 CDeviceWrapper;
typedef CSwapChainWrapper10 CSwapChainWrapper;


class CDXGIPresentRate
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
    CDXGIPresentRate();

    virtual ~CDXGIPresentRate();


    //
    // Framework override methods
    //

protected:
    bool InitializeTestGroup();

    void CleanupTestGroup();
};


class CDXGIPresentRateSingleSC
    :
    public CDXGIPresentRate
{
    //
    // Construction/destruction
    //

public:
    CDXGIPresentRateSingleSC();

    virtual ~CDXGIPresentRateSingleSC();


protected:
    HRESULT GetRefreshRate(UINT *pRefreshRate, UINT uiDXGIAdapterOrdinal, UINT uiDXGIOutputOrdinal);
    HRESULT GetRefreshRateForDefault(UINT *pRefreshRate);
    HRESULT ConvertQpcToMs(
        __in LARGE_INTEGER &qpcTime,
        __out __int64* pTime);

    HRESULT GetPerformanceCounterTime(
        __out __int64* pTime);
    bool IsPixelFormatCompatibleWithSwapEffect
    (__in DXGI_SWAP_EFFECT swapeffect, 
     __in DXGI_FORMAT pixelformat, 
     __in TEST_FEATURE_LEVEL level);

protected:
    static const DXGI_FORMAT dxgifmtBBInitTexture;
    static const UINT cuiBBInitTextureWidth;
    static const UINT cuiBBInitTextureHeight;
    static const UINT cuiNoOfPresentOperations;
    static const UINT cuiSkipPresentsAtStart;
    static const float cfMaxFractionForImmediate;
    static const float cfMaxFractionalDeviationForNonImmediate;
    static const float cfPrecentLateFrames;

    DXGI_ADAPTER_POINTER m_AdapterPointer;
    DXGI_FACTORY_POINTER m_FactoryPointer;
    D3D10_DEVICE_POINTER m_DevicePointer;
    HWND m_hwndFocusWindow;
    HWND m_hwndDeviceWindow;

    CDeviceWrapper m_PrimaryAdapterDevice;
    CSwapChainWrapper m_TestedSwapChain;
    
    CDCompIsolation m_DCOMPIsolation;

    DXGI_SWAP_CHAIN_DESC SCDesc;
    IDXGISwapChain1 *m_pSwapChain;
    IDXGIDevice1 *m_pDXGIDevice;
    BOOL m_useQpcTimes;
    BOOL m_useAverages;

    ID3DTexture2D *pBBInitTextureSys[MAX_BUFFERS];
    ID3DTexture2D *pBBInitTextureVid[MAX_BUFFERS];
    ID3DShaderResourceView *pBBInitSRV[MAX_BUFFERS];

public:
    bool InitializeTexturePair(UINT uiIndex, const char * const szResourceName);
    bool CreateTextures();
    void ReleaseTextures();
    void ReleaseTestCaseObjects();
    HRESULT TestFullScreenSwapChain(UINT uiSyncInterval, UINT uiRefreshRateInHz, DXGI_SWAP_EFFECT swapEffect, UINT bufferCount, TEST_RESULT *pTestResult);
    HRESULT TestWindowedSwapChain(UINT uiSyncInterval, UINT uiRefreshRateInHz, DXGI_SWAP_EFFECT swapEffect, UINT bufferCount,
        DXGI_PRESENT_PARAMETERS* pPresentParameters, TEST_RESULT *pTestResult);
    HRESULT TestWindowedBltSwapChainPartialPresentation(UINT refreshRateInHz, UINT bufferCount,
        DXGI_PRESENT_PARAMETERS* pPresentParameters, TEST_RESULT * pTestresult);
    HRESULT TestVaryingSyncInterval
        (
        UINT baseSyncInterval,
        UINT varyingSyncInterval,
        UINT varyingInterval,
        UINT refreshRateInHz,
        DXGI_SWAP_EFFECT swapEffect,
        UINT bufferCount,
		bool bDoNotSequence,
        TEST_RESULT * pTestresult
        );

    TEST_RESULT VerifyPresentOnTimeFrames
        (
        __in_ecount(presentCount) __int64 *pFrametimes,
        __in UINT presentCount,
        __in UINT skipPresentsAtStart,
        __in UINT expectedFrameTime,
        __in UINT timeBuffer,
        __in FLOAT fractionalDeviation
        );

    TEST_RESULT  VerifyPresentAverageTime
        (
        __in_ecount(presentCount) __int64 *pFrametimes,
        __in UINT presentCount,
        __in UINT skipPresentsAtStart,
        __in UINT syncInterval,
        __in UINT refreshRateInHz,
        __in DXGI_SWAP_EFFECT swapeffect,
        __in UINT buffercount,
        __in bool fullscreen

        );

    TEST_RESULT  VerifyPresentAverageTime
        (
        __in_ecount(presentCount) __int64 *pFrametimes,
        __in_ecount(presentCount) __int64 *pExpectedFrametimes,
        __in UINT presentCount,
        __in UINT skipPresentsAtStart,
        __in UINT refreshRateInHz
        );
};


class CWindowedTestCases
    :
    public CDXGIPresentRateSingleSC
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
    bool m_tp_bPartialPresentation;
    DXGI_PRESENT_PARAMETERS m_PresentParams;
    RECT m_DirtyRects[ 9 ];


    TEST_RESULT CreateDeviceAndSwapChain
        (
        bool bDWM,
        bool bUseDCOMP,
        DXGI_FORMAT PixelFormat,
        DXGI_SWAP_EFFECT SwapEffectArg,
        int iNumBuffersIndexArg
        );
};


class CFSTestCases
    :
    public CDXGIPresentRateSingleSC
{
    //
    // Construction/destruction
    //

public:
    CFSTestCases();

    virtual ~CFSTestCases();


    //
    // Framework override methods
    //

public:
    virtual TEST_RESULT Setup();

    virtual void Cleanup();


    //
    // Helper methods
    //

protected:
    bool EnumerateAdapterModes
        (
        IDXGIAdapter * pDXGIAdapterEnumMode,
        UINT uiAdapterOrdinal
        );

    HRESULT EnumerateModes();

    RFactor InitModeParameters();

    TEST_RESULT CreateDeviceAndSwapChain
        (
        int iSwapEffectIndexArg,
        int iNumBuffersIndexArg
        );


    //
    // Member data
    //

protected:
    typedef struct _FullscreenTestMode
    {
        UINT uiDXGIAdapterOrdinal;
        UINT uiDXGIOutputOrdinal;
        DXGI_MODE_DESC ModeDesc;
    } FullscreenTestMode;

    std::map< HMONITOR, DEVMODEW > m_DesktopMode;    // original desktop mode, to be restored upon exit
    std::vector < DXGI_ADAPTER_DESC > m_vecAdapterDescriptors;

    FullscreenTestMode m_TestedModes[ MAX_DISPLAYMODES ];
    FullscreenTestMode m_FSModeInTest;

    BOOL m_bDWMState;
    BOOL m_bIgnoreRefresh;

    HRESULT m_hEnumerateModesResult;

    int m_iTotalModes;
};


class CWinSequential
    :
    public CWindowedTestCases
{
    //
    // Construction/destruction
    //

public:
    CWinSequential();

    virtual ~CWinSequential();


    //
    // Framework override methods
    //

public:
    virtual void InitTestParameters();

    virtual TEST_RESULT ExecuteTestCase();

    virtual void Cleanup();


    //
    // Member data
    //

protected:
    bool m_bDWM;
    DXGI_SWAP_EFFECT m_SwapEffect;
    DXGI_FORMAT m_PixelFormat;
    int iNumBuffersIndex;
    int iSyncInterval;
    bool m_tp_bWindowless; // Test Case Parameter - Bool indicating whether we should sse DCOMP (true) or traditional windowed presents (false).

};

class CWinSequentialVaryingSyncInterval
    :
    public CWindowedTestCases
{
    //
    // Construction/destruction
    //

public:
    CWinSequentialVaryingSyncInterval();

    virtual ~CWinSequentialVaryingSyncInterval();


    //
    // Framework override methods
    //

public:
    virtual void InitTestParameters();

    virtual TEST_RESULT ExecuteTestCase();

    virtual void Cleanup();


    //
    // Member data
    //

protected:
    DXGI_SWAP_EFFECT m_SwapEffect;
    bool m_bDWM;
    DXGI_FORMAT m_PixelFormat;
    int iNumBuffersIndex;
    int iSyncInterval1;
    int iSyncInterval2;
	int iIntervalDivisor;
	bool m_bDoNotSequence;   // Used to cycle use of DXGI_PRESENT_DO_NOT_SEQUENCE on syncinterval2 frames.
    bool m_tp_bWindowless; // Test Case Parameter - Bool indicating whether we should sse DCOMP (true) or traditional windowed presents (false).

};

class CFSSequential
    :
    public CFSTestCases
{
    //
    // Construction/destruction
    //

public:
    CFSSequential();

    virtual ~CFSSequential();


    //
    // Framework override methods
    //

public:
    virtual void InitTestParameters();

    virtual TEST_RESULT ExecuteTestCase();

    virtual void Cleanup();


    //
    // Member data
    //

protected:
    int iNumBuffersIndex;
    int iSyncInterval;
};


class CWinDiscard
    :
    public CWindowedTestCases
{
    //
    // Construction/destruction
    //

public:
    CWinDiscard();

    virtual ~CWinDiscard();


    //
    // Framework override methods
    //

public:
    virtual void InitTestParameters();

    virtual TEST_RESULT ExecuteTestCase();

    virtual void Cleanup();


    //
    // Member data
    //

protected:
    bool m_bDWM;
    DXGI_SWAP_EFFECT m_SwapEffect;
    DXGI_FORMAT m_PixelFormat;
    int iNumBuffersIndex;
    int iSyncInterval;
    bool m_tp_bWindowless; // Test Case Parameter - Bool indicating whether we should sse DCOMP (true) or traditional windowed presents (false).

};


class CFSDiscard
    :
    public CFSTestCases
{
    //
    // Construction/destruction
    //

public:
    CFSDiscard();

    virtual ~CFSDiscard();


    //
    // Framework override methods
    //

public:
    virtual void InitTestParameters();

    virtual TEST_RESULT ExecuteTestCase();

    virtual void Cleanup();


    //
    // Member data
    //

protected:
    int iNumBuffersIndex;
    int iSyncInterval;
};

#endif
