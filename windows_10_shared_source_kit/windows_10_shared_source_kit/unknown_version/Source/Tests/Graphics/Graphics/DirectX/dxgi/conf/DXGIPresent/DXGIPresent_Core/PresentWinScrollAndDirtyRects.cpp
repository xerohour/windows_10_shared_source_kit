///////////////////////////////////////////////////////////////////////////////
// File:  PresentWinScrollAndDirtyRects.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
//  Houses test cases for DXGI's Present1 Dirty Rects and Scrolling features
//  introduced in Windows 8.
//  Based on CPresentWinClippingWindows class, but not derived from it.
//  Current implementation is a super set of CPresentWinClippingWindows, but is
//  implemented seperately to allow for future differentiation.
//
// History:
// August 3,2010 - olanh - Initial Creation
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
CPresentWinScrollAndDirtyRects::CPresentWinScrollAndDirtyRects()
{
	m_bStretchBB = false;
	m_fMaxNonConformanceFractionWin = 0.05f;
};


CPresentWinScrollAndDirtyRects::~CPresentWinScrollAndDirtyRects()
{
	//
	// Do nothing
	//
};


///////////////////////////////////////////////////////////////////////////////
// InitTestParameters
//
// Replaces the CPresentWinConvertStretch base class InitTestParameters with
// this version.   This is temporary code to restrict dirty rects and scrolling
// to Flip Model with DWM on.   This restriction will go away with Win8 M2.
///////////////////////////////////////////////////////////////////////////////
void CPresentWinScrollAndDirtyRects::InitTestParametersImpl()
{
	m_adapterNoOutputs = false;

	//
	// Add clipping window test factors
	//
	RECT window = {0,0,800,600};
	CUserValueSet< RECT > *pClippingWindows = new CUserValueSet< RECT >();
	RECT testRect = {0,0,0,0};

	pClippingWindows->AddValue( testRect );

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
        // Add Back Buffer count cycling
        //
        // Flip_Sequential & Sequential require 2, Discard requires 1
        //
	// See DXGIPresentDirtyRects.exe for a more exhaustive back buffer count focuxesd
        // test app.
        //
        CUserValueSet<UINT> *pBackBuffer1 = new CUserValueSet<UINT>();
        pBackBuffer1->AddValue(1);
        RFactor BackBuffer1 = AddParameter<UINT>(_T("BackBuffers"), &m_BufferCount, pBackBuffer1);

        CUserValueSet<UINT> *pBackBuffer2 = new CUserValueSet<UINT>();
        pBackBuffer2->AddValue(2);
        RFactor BackBuffer2 = AddParameterValueSet<UINT>(_T("BackBuffers"), pBackBuffer2);

	//
	// Add Dirty Rect cycling.
	//
	// For the main present test, it's overkill to cycle on a variety of dirty
	// rect cases, we just want to explore a single case in combination with
	// other test factors.   Therefore make this a bool to enable or disable
	// dirty rects.
	//
	// See DXGIPresentDirtyRects.exe for a more exhaustive dirty rects focused
	// test app.
	//
	CUserValueSet<bool> *pDirtyRectsEnabled= new CUserValueSet<bool>();
	pDirtyRectsEnabled->AddValue(true); // Window mode support added in Windows 8 (At least for Flip model swap effects).
	pDirtyRectsEnabled->AddValue(false);
	RFactor DirtyRects = AddParameter<bool>(_T("DirtyRects"), &m_bUseDirtyRects, pDirtyRectsEnabled);

	//
	// Add Scrolling
	//
	// We'll cycle on a very limited set of rects and cardinal scrolling directions.
	// Add all elements from the global table defined at the start of this source module.
	//
	// The first element is all zeros which represents the "no scrolling" case.
	//
	CUserValueSet< SCROLLINGINFO > *pScrollInfoValues = new CUserValueSet< SCROLLINGINFO >();
	for (unsigned int u=0; u<g_uNumOfScrollInfoElements; u++)
		pScrollInfoValues->AddValue(g_ScrollingInfo[u]);
	RFactor Scrolling = AddParameter<SCROLLINGINFO>(_T("ScrollInfo"), &m_ScrollInfo, pScrollInfoValues);
	//
	// For Blt Model we don't support Scrolling.   Add just the first element from the scrolling
	// table which translates to "no scrolling".
	//
	CUserValueSet< SCROLLINGINFO > *pScrollInfoValuesBltModel = new CUserValueSet< SCROLLINGINFO >();
	pScrollInfoValuesBltModel->AddValue(g_ScrollingInfo[0]);
	RFactor ScrollingBltModel = AddParameterValueSet<SCROLLINGINFO>(_T("ScrollInfo"), pScrollInfoValuesBltModel);


	//
	// Need to maintain seperate lists for blt_discard, blt_sequential and flip model swap effects.
	// Blt model does not support scrolling nor DCOMP.
        // Blt discard not supported at all.
	//
	// Blt model Swap effect factors
        // Sequential(Blt) swap effect factors
	CUserValueSet< DXGISwapEffectRecord > *pSwapEffectBltSequential = new CUserValueSet< DXGISwapEffectRecord >();
	for( UINT i = 0; i < cuiNoOfSwapEffects; i++ )
	{
		if (DXGISwapEffects[i].dxgiSwapEffect == DXGI_SWAP_EFFECT_SEQUENTIAL)
			pSwapEffectBltSequential->AddValue(DXGISwapEffects[i]);
	}
	RFactor SwapEffectBltSequentialFactors = AddParameter< DXGISwapEffectRecord >( _T("SwapEffect"), &m_SwapEffect, pSwapEffectBltSequential);

	// Flip model Swap effect factors
	CUserValueSet< DXGISwapEffectRecord > *pSwapEffectFlip = new CUserValueSet< DXGISwapEffectRecord >();
	for( UINT i = 0; i < cuiNoOfSwapEffects; i++ )
	{
		if (DXGISwapEffects[i].dxgiSwapEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL)
			pSwapEffectFlip->AddValue(DXGISwapEffects[i]);
	}
	RFactor SwapEffectFlipFactors = AddParameterValueSet< DXGISwapEffectRecord >( _T("SwapEffect"), pSwapEffectFlip);

	RFactor SwapEffectsCombined = SwapEffectFlipFactors + SwapEffectBltSequentialFactors;

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

        //
        // Create aggregate factors for blt and flip model
        //
        RFactor rfFlipFactors = (DontUseDCOMPFactor + UseDCOMPFactor) * SwapEffectFlipFactors * DirtyRects * Scrolling * BackBuffer2;
        RFactor rfBltSequential = DontUseDCOMPFactor * SwapEffectBltSequentialFactors * DirtyRects * ScrollingBltModel * (BackBuffer1 + BackBuffer2);

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
	//RFactor Bpp16_DwmOff = m_FeatureLevelFactor * m_globalRFactorDwmOffBpp * v_dwmCompositionOff * DontUseDCOMPFactor * SwapEffectFlipFactors * DirtyRects * Scrolling * clippingWindows;
	// DWM ON - DCOMP Supported, but only for FlipModel.  for Regular Windowed Presents test both blt and Flip model.
	RFactor Bpp32_DwmOn =
                m_FeatureLevelFactor  *
                m_globalRFactorDwmOnBpp *
                clippingWindows *
                v_dwmCompositionOn *
                ( rfFlipFactors + rfBltSequential );
	// DWM Off - DCOMP Not Supported
	//RFactor Bpp32_DwmOff = m_FeatureLevelFactor  * m_globalRFactorDwmOnBpp * dwmCompositionOff_32bpp * DontUseDCOMPFactor * SwapEffectFlipFactors * DirtyRects * Scrolling * clippingWindows;


	//AddPriorityPatternFilter(FilterEqual< bool >(_T("DWM"), false ), WeightFactorVerySmall);

	// Give priority to the no scrolling and first scrolling rect.
	AddPriorityPatternFilter(FilterEqual< SCROLLINGINFO >(_T("ScrollInfo"), g_ScrollingInfo[0] ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual< SCROLLINGINFO >(_T("ScrollInfo"), g_ScrollingInfo[1] ), WeightFactorLarge);

	// Give priority to the "no clipping" case.
	RECT testRectHighPriority = {0,0,0,0};
	AddPriorityPatternFilter(FilterEqual< RECT >(_T("clippingWindowRect"), testRectHighPriority ), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue(1, 5.0f);
	DeclarePriorityLevelByPriorityValue(2, 2.0f);

	//    if( Bpp16_DwmOff.Valid() && Bpp32_DwmOn.Valid())
	//        SetRootTestFactor( Bpp32_DwmOn + Bpp32_DwmOff + Bpp16_DwmOff );
	//    else if( !Bpp16_DwmOff.Valid() && Bpp32_DwmOn.Valid())
	//        SetRootTestFactor(Bpp32_DwmOn + Bpp32_DwmOff);
	//    else if( Bpp16_DwmOff.Valid() && !Bpp32_DwmOn.Valid())
	//        SetRootTestFactor( Bpp16_DwmOff );
	//    else
	//    {
	//        SetRootTestFactor( m_FeatureLevelFactor );
	//        m_ParameterError = -1;
	//    }

	if( Bpp32_DwmOn.Valid() )
		SetRootTestFactor(Bpp32_DwmOn);
	else
	{
		SetRootTestFactor( m_FeatureLevelFactor );
		m_ParameterError = -1;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Setup
//
///////////////////////////////////////////////////////////////////////////////
TEST_RESULT CPresentWinScrollAndDirtyRects::Setup()
{
	return CPresentWinConvertStretch::Setup();
};

///////////////////////////////////////////////////////////////////////////////
//
// CreateDeviceAndSwapChain
//
///////////////////////////////////////////////////////////////////////////////
TEST_RESULT CPresentWinScrollAndDirtyRects::CreateDeviceAndSwapChain()
{
	return CPresentWinConvertStretch::CreateDeviceAndSwapChain();
};

///////////////////////////////////////////////////////////////////////////////
//
// CleanupTestCase
//
///////////////////////////////////////////////////////////////////////////////
void CPresentWinScrollAndDirtyRects::CleanupTestCase()
{
	CPresentWinConvertStretch::CleanupTestCase();
	m_WindowTree.CleanUpWindowTree();
};

///////////////////////////////////////////////////////////////////////////////
//
// ExecuteTestCase
//
///////////////////////////////////////////////////////////////////////////////
TEST_RESULT CPresentWinScrollAndDirtyRects::ExecuteTestCase()
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
HRESULT CPresentWinScrollAndDirtyRects::CreateTestWindows(
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

HRESULT CPresentWinScrollAndDirtyRects::UpdateReferenceTexture()
{
	HRESULT hr = S_OK;

	//
	// for  LinearToGamma conversion.
	//
	CPresentWinConvertStretch::UpdateReferenceTexture();

	RECT intersection = { 0 };

	//fIf the clipping window overlaps our main window, then update update the reference image to match.
	if (IntersectRect(&intersection, &m_ClippingWindows, &rectDeviceWindow))
	{

		//move rect back to 0,0 in reference texture space
		if (!OffsetRect(&intersection, -rectDeviceWindow.left, -rectDeviceWindow.top))
		{
			DWORD error = GetLastError();
			hr = (ERROR_SUCCESS == error) ? E_FAIL : HRESULT_FROM_WIN32(hr);
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
	}
	return hr;
}

