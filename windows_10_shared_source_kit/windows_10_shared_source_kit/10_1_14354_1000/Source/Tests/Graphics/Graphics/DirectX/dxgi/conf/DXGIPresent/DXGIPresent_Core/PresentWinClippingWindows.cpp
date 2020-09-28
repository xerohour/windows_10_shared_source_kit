///////////////////////////////////////////////////////////////////////////////
// File:  PresentWinClippingWindows.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of DXGI present windowed converting/stretching swap
// chain tests, with clipping windows on top
//
///////////////////////////////////////////////////////////////////////////////


#include "TestApp.h"

#include "DXGIBBFMT.H"
#include "DXGIValueSets.h"
#include "BBDims.h"
#include "DXGISwapEffects.h"
#include <parameterhelper.h>
#include <WindowTree.h>
#include <StrSafe.h>
using namespace testfactor;

///////////////////////////////////////////////////////////////////////////////
//
// Constructors / Destructor
//
///////////////////////////////////////////////////////////////////////////////
CPresentWinClippingWindows::CPresentWinClippingWindows()
{
    m_bStretchBB = false;
    m_fMaxNonConformanceFractionWin = 0.05f;
};


CPresentWinClippingWindows::~CPresentWinClippingWindows()
{
    //
    // Do nothing
    //
};


//****************************************************************************
// CPresentWinClippingWindows::InitTestParameters
//
// Overloaded version of CPresentWinConvertStretch base class function.
//****************************************************************************
void CPresentWinClippingWindows::InitTestParametersImpl()
{
    RECT window = {0,0,800,600};
    CUserValueSet< RECT > *pClippingWindows = new CUserValueSet< RECT >();
    RECT testRect = {0,0,0,0};

    testRect.left = window.left;
    testRect.top = window.top;
    testRect.right = LONG((FLOAT)window.right *.5f);
    testRect.bottom = LONG((FLOAT)window.bottom *.5f);

    pClippingWindows->AddValue( testRect );

    testRect.left = window.left + (LONG((FLOAT)(window.right - window.left) *.5f));
    testRect.top = window.top + (LONG((FLOAT)(window.bottom - window.top) *.5f));
    testRect.right = testRect.left + (LONG((FLOAT)(window.right - window.left) *.5f));
    testRect.bottom = testRect.top + (LONG((FLOAT)(window.bottom - window.top) *.5f));

    pClippingWindows->AddValue( testRect );

    testRect.left = window.left + (LONG((FLOAT)(window.right - window.left) *.25f));
    testRect.top = window.top + (LONG((FLOAT)(window.bottom - window.top) *.25f));
    testRect.right = testRect.left +  LONG((FLOAT)window.right *.5f);
    testRect.bottom = testRect.top + LONG((FLOAT)window.bottom *.5f);

    pClippingWindows->AddValue( testRect );

    RFactor clippingWindows = AddParameter< RECT >( _T("clippingWindowRect"), &m_ClippingWindows, pClippingWindows);

    //
    // Need to maintain separate lists for blt and flip model swap effects.
    // Blt model does not support scrolling, dirty rects, nor DCOMP.
    //
    // Blt model Swap effect factors
    CUserValueSet< DXGISwapEffectRecord > *pSwapEffectBlt = new CUserValueSet< DXGISwapEffectRecord >();
    for( UINT i = 0; i < cuiNoOfSwapEffects; i++ )
    {
        if (!IsFlipModel(DXGISwapEffects[i].dxgiSwapEffect))
            pSwapEffectBlt->AddValue(DXGISwapEffects[i]);
    }
    RFactor SwapEffectBltFactors = AddParameter< DXGISwapEffectRecord >( _T("SwapEffect") , &m_SwapEffect, pSwapEffectBlt);

    // Flip model Swap effect factors
    CUserValueSet< DXGISwapEffectRecord > *pSwapEffectFlip = new CUserValueSet< DXGISwapEffectRecord >();
    for( UINT i = 0; i < cuiNoOfSwapEffects; i++ )
    {
        if (IsFlipModel(DXGISwapEffects[i].dxgiSwapEffect))
            pSwapEffectFlip->AddValue(DXGISwapEffects[i]);
    }
    RFactor SwapEffectFlipFactors = AddParameterValueSet< DXGISwapEffectRecord >( _T("SwapEffect"), pSwapEffectFlip);

    RFactor SwapEffectsCombined = SwapEffectFlipFactors + SwapEffectBltFactors;

    //
    // We need separate Test factors for choosing between DCOMP (Windowless) and regular Windowed Presents.
    // DCOMP is only supported for FlipModel when DWM is on.
    //
    // (Note that the name "windowless" is somewhat misleading since the visuals themselves are still contained with-in a Window.)
    //

    // Use DCOMP
    CUserValueSet< bool > *pUseDCOMP = new CUserValueSet< bool >();
    pUseDCOMP->AddValue(true);
    RFactor UseDCOMPFactor = AddParameter< bool >( _T("UseDCOMP") , &m_tp_bWindowless, pUseDCOMP);

    // Don't use DCOMP (use regular Windowed Presents)
    CUserValueSet< bool > *pDontUseDCOMP = new CUserValueSet< bool >();
    pDontUseDCOMP->AddValue(false);
    RFactor DontUseDCOMPFactor = AddParameterValueSet< bool >( _T("UseDCOMP"), pDontUseDCOMP);

    // DWM on/off Factors
    CUserValueSet< bool > *pDwmCompositionOn = new CUserValueSet< bool >();
    if ( g_Application.IsTerminalServices() == true ||
        GetSystemMetrics( SM_REMOTESESSION) != 0)
    {
        //
        // Dont add the toggle for DWM on and off in the remote case.
        //
    }
    else
    {
        pDwmCompositionOn->AddValue( false );
    }

    // Invlid dwm factor when in the remote case
    RFactor dwmCompositionOff_32bpp = AddParameter< bool >( _T("DWM"), &m_bDWM, pDwmCompositionOn);
    // Always valid dwm factors
    RFactor v_dwmCompositionOn = AddParameterValue< bool >( _T("DWM"), true);
    RFactor v_dwmCompositionOff = AddParameterValue< bool >( _T("DWM"), false);

    // Separate out Bpp32_DwmOn and DwmOff when adding test parameters together so that
    // the order of test cases are preserved.
    //
    // DWM Off - DCOMP Not Supported
    RFactor Bpp16_DwmOff = m_FeatureLevelFactor * m_globalRFactorDwmOffBpp * v_dwmCompositionOff * DontUseDCOMPFactor * SwapEffectsCombined *  clippingWindows;
    // DWM ON - DCOMP Supported, but only for FlipModel.  for Regular Windowed Presents test both blt and Flip model.
    RFactor Bpp32_DwmOn = m_FeatureLevelFactor  * m_globalRFactorDwmOnBpp * v_dwmCompositionOn * (DontUseDCOMPFactor*SwapEffectsCombined + UseDCOMPFactor*SwapEffectFlipFactors) *  clippingWindows;
    // DWM Off - DCOMP Not Supported
    RFactor Bpp32_DwmOff = m_FeatureLevelFactor  * m_globalRFactorDwmOnBpp * dwmCompositionOff_32bpp * DontUseDCOMPFactor * SwapEffectsCombined *  clippingWindows;

    // Only test dwm off cases on Win7+8IP and below
    OSVERSIONINFO versionInfo;
    versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
    GetVersionEx(&versionInfo);
    if(versionInfo.dwMajorVersion < 6 || (versionInfo.dwMajorVersion == 6 && versionInfo.dwMinorVersion < 2) )
    {
        // Check which set of enumerated mode set parameters are valid
        if( Bpp16_DwmOff.Valid() && Bpp32_DwmOn.Valid())
            SetRootTestFactor( Bpp32_DwmOn + Bpp32_DwmOff + Bpp16_DwmOff );
        else if( !Bpp16_DwmOff.Valid() && Bpp32_DwmOn.Valid())
            SetRootTestFactor(Bpp32_DwmOn + Bpp32_DwmOff);
        else if( Bpp16_DwmOff.Valid() && !Bpp32_DwmOn.Valid())
            SetRootTestFactor( Bpp16_DwmOff );
        else
        {
            SetRootTestFactor( m_FeatureLevelFactor );
            m_ParameterError = -1;
        }
        AddPriorityPatternFilter(FilterEqual< bool >(_T("DWM"), false ), WeightFactorVerySmall);
    }
    else
    {
        if( Bpp32_DwmOn.Valid())
            SetRootTestFactor( Bpp32_DwmOn );
    }

    DeclarePriorityLevelByPriorityValue(1, 3.0f);
	DeclarePriorityLevelByPriorityValue(2, 2.0f);

}

///////////////////////////////////////////////////////////////////////////////
//
// Setup
//
///////////////////////////////////////////////////////////////////////////////
TEST_RESULT CPresentWinClippingWindows::Setup()
{
    return CPresentWinConvertStretch::Setup();
};

///////////////////////////////////////////////////////////////////////////////
//
// CreateDeviceAndSwapChain
//
///////////////////////////////////////////////////////////////////////////////
TEST_RESULT CPresentWinClippingWindows::CreateDeviceAndSwapChain()
{
    return CPresentWinConvertStretch::CreateDeviceAndSwapChain();
};

///////////////////////////////////////////////////////////////////////////////
//
// CleanupTestCase
//
///////////////////////////////////////////////////////////////////////////////
void CPresentWinClippingWindows::CleanupTestCase()
{
    CPresentWinConvertStretch::CleanupTestCase();
    m_WindowTree.CleanUpWindowTree();
};

///////////////////////////////////////////////////////////////////////////////
//
// ExecuteTestCase
//
///////////////////////////////////////////////////////////////////////////////
TEST_RESULT CPresentWinClippingWindows::ExecuteTestCase()
{
    return CPresentWinConvertStretch::ExecuteTestCase();
};

///////////////////////////////////////////////////////////////////////////////
//
// CreateTestWindows
//
// Must set m_hwndFocusWindow
//
///////////////////////////////////////////////////////////////////////////////
HRESULT CPresentWinClippingWindows::CreateTestWindows(
    UINT focusWindowLeft,
    UINT focusWindowTop,
    UINT focusWindowWidth,
    UINT focusWindowHeight)
{
    HRESULT hr = S_OK;

    hr = CPresentWinConvertStretch::CreateTestWindows(
        focusWindowLeft,
        focusWindowTop,
        focusWindowWidth,
        focusWindowHeight);

    if ( SUCCEEDED(hr) && m_hwndFocusWindow == NULL)
    {
        DWORD error = GetLastError();
        hr = (ERROR_SUCCESS == error) ? E_FAIL : HRESULT_FROM_WIN32(hr);
    }

    WindowData data = {0};
    if(SUCCEEDED(hr))
    {
        data.Style = WS_POPUP | WS_VISIBLE;
        data.X = m_ClippingWindows.left;
        data.Y = m_ClippingWindows.top;
        data.Width = m_ClippingWindows.right - m_ClippingWindows.left;
        data.Height = m_ClippingWindows.bottom - m_ClippingWindows.top;
        data.Color= m_ClippingColor;
        data.ExStyle = WS_EX_TOPMOST;
        data.IsTestWindow =false;
        data.IsRegionWindow =false;
        data.IsLayeredWindow =false;
        data.CreateDevice = false;
        Node<WindowData>* pChild = NULL;
        hr = m_WindowTree.InsertChild(NULL, data, &pChild);
    }

    return hr;
}

HRESULT CPresentWinClippingWindows::UpdateReferenceTexture()
{
    HRESULT hr = S_OK;

    //
    // for  LinearToGamma conversion.
    //
    CPresentWinConvertStretch::UpdateReferenceTexture();

    RECT intersection = { 0 };

    //find where the clipping window intersects the client area
    if ( SUCCEEDED(hr))
    {
        if (!IntersectRect(&intersection, &m_ClippingWindows, &rectDeviceWindow))
        {
            DWORD error = GetLastError();
            hr = (ERROR_SUCCESS == error) ? E_FAIL : HRESULT_FROM_WIN32(hr);
        }
    }

    //move rect back to 0,0 in reference texture space
    if ( SUCCEEDED(hr))
    {
        if (!OffsetRect(&intersection, -rectDeviceWindow.left, -rectDeviceWindow.top))
        {
            DWORD error = GetLastError();
            hr = (ERROR_SUCCESS == error) ? E_FAIL : HRESULT_FROM_WIN32(hr);
        }
    }


    MYD3DCOLOR color;
    color.red = GetRValue(m_ClippingColor);
    color.green = GetGValue(m_ClippingColor);
    color.blue = GetBValue(m_ClippingColor);
    color.alpha = 255;

    if ( SUCCEEDED(hr))
    {
        hr = DrawRect(
            m_pBackBufferRendered,
            intersection.left,
            intersection.top,
            intersection.right - intersection.left,
            intersection.bottom - intersection.top,
            color);

        if ( FAILED(hr))
        {
            WriteToLog(_T("DrawRect failed. 0x%x"), hr);
        };
    }

    return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Constructors / Destructor
//
///////////////////////////////////////////////////////////////////////////////
CPresentWinChildWindows::CPresentWinChildWindows()
{
    m_bStretchBB = false;
    m_fMaxNonConformanceFractionWin = 0.05f;
};


CPresentWinChildWindows::~CPresentWinChildWindows()
{
    //
    // Do nothing
    //
};


///////////////////////////////////////////////////////////////////////////////
//
// Setup
//
///////////////////////////////////////////////////////////////////////////////
TEST_RESULT CPresentWinChildWindows::Setup()
{
    return CPresentWinConvertStretch::Setup();
};

///////////////////////////////////////////////////////////////////////////////
//
// CreateDeviceAndSwapChain
//
///////////////////////////////////////////////////////////////////////////////
TEST_RESULT CPresentWinChildWindows::CreateDeviceAndSwapChain()
{
    return CPresentWinConvertStretch::CreateDeviceAndSwapChain();
};

///////////////////////////////////////////////////////////////////////////////
//
// CleanupTestCase
//
///////////////////////////////////////////////////////////////////////////////
void CPresentWinChildWindows::CleanupTestCase()
{
    CPresentWinConvertStretch::CleanupTestCase();
    m_WindowTree.CleanUpWindowTree();
};

///////////////////////////////////////////////////////////////////////////////
//
// ExecuteTestCase
//
///////////////////////////////////////////////////////////////////////////////
TEST_RESULT CPresentWinChildWindows::ExecuteTestCase()
{
    return CPresentWinConvertStretch::ExecuteTestCase();
};




//****************************************************************************
// CPresentWinChildWindows::InitTestParameters
//
// Overloaded version of CPresentWinConvertStretch base class function.
//****************************************************************************
void CPresentWinChildWindows::InitTestParametersImpl()
{
    CUserValueSet< DXGISwapEffectRecord > *pSwapEffect = new CUserValueSet< DXGISwapEffectRecord >();
    for( UINT i = 0; i < cuiNoOfSwapEffects; i++ )
    {
        pSwapEffect->AddValue(DXGISwapEffects[i]);
    }

    RFactor swapEffect = AddParameter< DXGISwapEffectRecord >( _T("SwapEffect") , &m_SwapEffect, pSwapEffect);

    RECT window = {0,0,800,600};
    CUserValueSet< RECT > *pChildWindows = new CUserValueSet< RECT >();
    RECT testRect = {0,0,0,0};

    testRect.left = window.left;
    testRect.top = window.top;
    testRect.right = LONG((FLOAT)window.right *.5f);
    testRect.bottom = LONG((FLOAT)window.bottom *.5f);

    pChildWindows->AddValue( testRect );

    testRect.left = window.left + (LONG((FLOAT)(window.right - window.left) *.5f));
    testRect.top = window.top + (LONG((FLOAT)(window.bottom - window.top) *.5f));
    testRect.right = testRect.left + (LONG((FLOAT)(window.right - window.left) *.5f));
    testRect.bottom = testRect.top + (LONG((FLOAT)(window.bottom - window.top) *.5f));

    pChildWindows->AddValue( testRect );

    testRect.left = window.left + (LONG((FLOAT)(window.right - window.left) *.25f));
    testRect.top = window.top + (LONG((FLOAT)(window.bottom - window.top) *.25f));
    testRect.right = testRect.left +  LONG((FLOAT)window.right *.5f);
    testRect.bottom = testRect.top + LONG((FLOAT)window.bottom *.5f);

    pChildWindows->AddValue( testRect );

    RFactor childWindows = AddParameter< RECT >( _T("childWindowRect"), &m_ChildWindows, pChildWindows);

    //
    // Test a single BBDimension.  This is the default value used even if we
    // didn't cycle on it, but adding it makes the test parameters common
    // across more test groups, thus preventing dialog pop-ups when you set a
    // test parameter that doesn't exist for some test groups.
    //
    CUserValueSet<int> *pBBDimensions = new CUserValueSet<int>();
    pBBDimensions->AddValue(0);
    RFactor rBBDimension = AddParameter<int>( _T("BBDimensions"), &m_iBBDimIndex, pBBDimensions );


    //
    // Test both Windowed and Windowless (DCOMP Visual) Presents when DWM is
    // on.  (Note that "windowless" is somewhat misleading since the visuals
    // themselves are still contained with-in a Window.)
    //
    CUserValueSet< bool > *pWindowlessDWMOn = new CUserValueSet< bool >();
    pWindowlessDWMOn->AddValue(false);
    pWindowlessDWMOn->AddValue(true);
    RFactor DWMOnWindowlessFactors = AddParameter< bool >( _T("UseDCOMP") , &m_tp_bWindowless, pWindowlessDWMOn);

    // Windowless (DCOMP) is NOT compatible with DWM off.  Test Window Presents only.
    CUserValueSet< bool > *pWindowlessDWMOff = new CUserValueSet< bool >();
    pWindowlessDWMOff->AddValue(false);
    RFactor DWMOffWindowlessFactors = AddParameterValueSet< bool >( _T("UseDCOMP"), pWindowlessDWMOff);


    CUserValueSet< bool > *pDwmCompositionOn = new CUserValueSet< bool >();
    if ( g_Application.IsTerminalServices() == true ||
        GetSystemMetrics( SM_REMOTESESSION) != 0)
    {
        //
        // Dont add the toggle for DWM on and off in the remote case.
        //
    }
    else
    {
        pDwmCompositionOn->AddValue( false );
    }

    // Invlid dwm factor when in the remote case
    RFactor dwmCompositionOff_32bpp = AddParameter< bool >( _T("DWM"), &m_bDWM, pDwmCompositionOn);
    // Always valid dwm factors
    RFactor v_dwmCompositionOn = AddParameterValue< bool >( _T("DWM"), true);
    RFactor v_dwmCompositionOff = AddParameterValue< bool >( _T("DWM"), false);

    // Separate out Bpp32_DwmOn and DwmOff when adding test parameters together so that
    // the order of test cases are preserved (
    RFactor Bpp16_DwmOff = m_FeatureLevelFactor * m_globalRFactorDwmOffBpp * v_dwmCompositionOff * DWMOffWindowlessFactors * swapEffect * rBBDimension * childWindows ;
    RFactor Bpp32_DwmOn_NoDComp = m_FeatureLevelFactor  * m_globalRFactorDwmOnBpp * v_dwmCompositionOn * DWMOffWindowlessFactors * swapEffect * rBBDimension * childWindows ;
    RFactor Bpp32_DwmOn = m_FeatureLevelFactor  * m_globalRFactorDwmOnBpp * v_dwmCompositionOn * DWMOnWindowlessFactors * swapEffect * rBBDimension * childWindows ;
    RFactor Bpp32_DwmOff = m_FeatureLevelFactor  * m_globalRFactorDwmOnBpp * dwmCompositionOff_32bpp * DWMOffWindowlessFactors * swapEffect * rBBDimension * childWindows ;

    // Only test dwm off cases on Win7+8IP and below
    OSVERSIONINFO versionInfo;
    versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
    GetVersionEx(&versionInfo);
    if(versionInfo.dwMajorVersion < 6 || (versionInfo.dwMajorVersion == 6 && versionInfo.dwMinorVersion < 2) )
    {
        // Check which set of enumerated mode set parameters are valid
        if( Bpp16_DwmOff.Valid() && Bpp32_DwmOn_NoDComp.Valid())
            SetRootTestFactor( Bpp32_DwmOn_NoDComp + Bpp32_DwmOff + Bpp16_DwmOff );
        else if( !Bpp16_DwmOff.Valid() && Bpp32_DwmOn_NoDComp.Valid())
            SetRootTestFactor(Bpp32_DwmOn_NoDComp + Bpp32_DwmOff);
        else if( Bpp16_DwmOff.Valid() && !Bpp32_DwmOn_NoDComp.Valid())
            SetRootTestFactor( Bpp16_DwmOff );
        else
        {
            SetRootTestFactor( m_FeatureLevelFactor );
            m_ParameterError = -1;
        }

        // Make all "DWM off" cases lower priority
        AddPriorityPatternFilter(FilterEqual< bool >(_T("DWM"), false ), WeightFactorVerySmall);
    }
    else
    {
        if( Bpp32_DwmOn.Valid())
            SetRootTestFactor( Bpp32_DwmOn );
    }

    DeclarePriorityLevelByPriorityValue(1, 3.0f);
	DeclarePriorityLevelByPriorityValue(2, 2.0f);

}

///////////////////////////////////////////////////////////////////////////////
//
// CreateTestWindows
//
// Must set m_hwndFocusWindow
//
///////////////////////////////////////////////////////////////////////////////
HRESULT CPresentWinChildWindows::CreateTestWindows(
    UINT focusWindowLeft,
    UINT focusWindowTop,
    UINT focusWindowWidth,
    UINT focusWindowHeight)
{
    HRESULT hr = S_OK;

    hr = CPresentWinConvertStretch::CreateTestWindows(
        focusWindowLeft,
        focusWindowTop,
        focusWindowWidth,
        focusWindowHeight);

    if ( SUCCEEDED(hr) && m_hwndFocusWindow == NULL)
    {
        DWORD error = GetLastError();
        hr = (ERROR_SUCCESS == error) ? E_FAIL : HRESULT_FROM_WIN32(hr);
    }

    if (SUCCEEDED(hr))
    {

        WindowData parentData, data = {0};

        parentData.Style = WS_POPUP| WS_VISIBLE|WS_CLIPCHILDREN| WS_CLIPSIBLINGS;
        parentData.X = focusWindowLeft;
        parentData.Y = focusWindowTop;
        parentData.Width = focusWindowWidth;
        parentData.Height = focusWindowHeight;
        parentData.Color= m_ClippingColor;
        parentData.ExStyle = WS_EX_TOPMOST;
        parentData.IsTestWindow =false;
        parentData.IsRegionWindow =false;
        parentData.IsLayeredWindow =false;
        parentData.CreateDevice = false;
        parentData.IsTestWindow = false;
        parentData.ExStyle =0;

        Node<WindowData>* pParent = NULL;
        if ( !SUCCEEDED(m_WindowTree.InsertChild(NULL, parentData, &pParent)))
            return hr;

        data.IsTestWindow = true;
        data.Style = WS_POPUP | WS_VISIBLE | WS_CHILD;

        data.Hwnd = m_hwndFocusWindow;
        data.ExStyle = WS_EX_TOPMOST;

        Node<WindowData>* pChild = NULL;
        hr = m_WindowTree.InsertChild(pParent, data, &pChild);

        if( SUCCEEDED(hr))
        {
            if( m_hwndFocusWindow == NULL )
            {
                DWORD error = GetLastError();
                hr = (ERROR_SUCCESS == error) ? E_FAIL : HRESULT_FROM_WIN32(hr);
            }
            else
            {
                m_hwndFocusWindow= pChild->Data.Hwnd;
                WriteToLog
                    (
                    "CPresentWinChildWindows::CreateTestWindows : Successfully created WS_CHILD style focus window."
                    );

            }

        }
    }

    return hr;
}
