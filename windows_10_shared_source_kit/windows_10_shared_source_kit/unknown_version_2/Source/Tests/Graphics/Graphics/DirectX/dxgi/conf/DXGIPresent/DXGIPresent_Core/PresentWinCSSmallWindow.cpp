//////////////////////////////////////////////////////////////////////
// File:  PresentWinCSSmallWindows.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of DXGI present windowed converting/stretching swap
// chain tests, for small window sizes.  Meant to a user simulate resizing
// the window.
//
//
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include "TestApp.h"

#include "DXGIBBFMT.H"
#include "DXGIValueSets.h"
#include "BBDims.h"
#include "DXGISwapEffects.h"

#include <StrSafe.h>
#include <mmsystem.h>

#pragma warning(disable: 4355)

/////////////////////////////////////////////////////////////////
//
// PresentWinCSSmallWindows implementation
//
/////////////////////////////////////////////////////////////////

PresentWinCSSmallWindow::PresentWinCSSmallWindow():
m_nPresents( 60 ),
m_bWin8OrHigher( false )
{

	m_DesktopModes[0].uiWidth = 1024;
	m_DesktopModes[0].uiHeight = 768;
	m_DesktopModes[0].uiBPP = 32;

	m_numberOfModes = 1;

	m_numberOfWindowsToTest = 2;
	m_NumberOfPresents = 1;


}


PresentWinCSSmallWindow::~PresentWinCSSmallWindow()
{
};

//****************************************************************************
// PresentWinCSSmallWindow::InitTestParameters
//
// Overloaded version of CPresentWinConvertStretch base class function.
//****************************************************************************
void PresentWinCSSmallWindow::InitTestParametersImpl()
{
	RFactor numberOfPresents = AddParameter
		(
		_T("NumberOfPresents"),
		&m_nPresents,
		RangeValueSet
		(
		(UINT)60,
		(UINT)60,
		1
		)
		);

	RFactor window = AddParameter
		(
		_T("Window"),
		&m_iWindow,
		RangeValueSet
		(
		4,
		5,
		1
		)
		);

	RFactor rfCombinedBasicFactors =  window * numberOfPresents;

	//
	// Need to maintain seperate lists for blt and flip model swap effects.
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
	//
	// Don't test DCOMP (use regular Windowed Presents only).  This test
	// explores buffer sizes of zero width and or height, which is not supported by DCOMP.
	//
	CUserValueSet< bool > *pDCOMPUsage = new CUserValueSet< bool >();
	pDCOMPUsage->AddValue(false);
	RFactor DontUseDCOMPFactor = AddParameter< bool >( _T("UseDCOMP") , &m_tp_bWindowless, pDCOMPUsage);

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
	RFactor Bpp16_DwmOff = m_FeatureLevelFactor * m_globalRFactorDwmOffBpp * v_dwmCompositionOff     * DontUseDCOMPFactor * SwapEffectsCombined * rfCombinedBasicFactors ;
	RFactor Bpp32_DwmOn = m_FeatureLevelFactor  * m_globalRFactorDwmOnBpp  * v_dwmCompositionOn      * DontUseDCOMPFactor * SwapEffectsCombined * rfCombinedBasicFactors ;
	RFactor Bpp32_DwmOff = m_FeatureLevelFactor * m_globalRFactorDwmOnBpp  * dwmCompositionOff_32bpp * DontUseDCOMPFactor * SwapEffectsCombined  * rfCombinedBasicFactors ;

	// Only test dwm off cases on Win7+8IP and below
	OSVERSIONINFO versionInfo;
	versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
	GetVersionEx(&versionInfo);
	m_bWin8OrHigher = false;
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
		m_bWin8OrHigher = true;
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
HRESULT PresentWinCSSmallWindow::CreateTestWindows(
	UINT focusWindowLeft,
	UINT focusWindowTop,
	UINT focusWindowWidth,
	UINT focusWindowHeight)
{
	HRESULT hr = S_OK;
	DWORD dwStyleEx = 0;

	if ( g_Application.IsRTL())
		dwStyleEx |= WS_EX_LAYOUTRTL;


	//
	// Create a window some client area. The purpose of the
	// test is to simulate user resizing to some small size and some client area is required.
	//


	m_hwndFocusWindow = CreateWindowEx
		(
		dwStyleEx,
		m_szFocusWindowClassName,
		"Focus Window",
		WS_POPUP | WS_VISIBLE,
		focusWindowLeft,
		focusWindowTop,
		focusWindowWidth,
		focusWindowHeight,
		NULL,
		NULL,
		m_hProgInst,
		NULL
		);

	if (m_hwndFocusWindow == NULL)
	{
		DWORD error = GetLastError();
		hr = (ERROR_SUCCESS == error) ? E_FAIL : HRESULT_FROM_WIN32(hr);
	}

	ProcessMessages();

	//
	// Some small dimensions cant be create with WS_OVERLAPPEDWINDOW.  Set it after creatation
	//
	DWORD dwStyle  = (DWORD)GetWindowLongPtr ( m_hwndFocusWindow, GWL_STYLE);

	dwStyle &= ~WS_POPUP;
	dwStyle |= WS_OVERLAPPEDWINDOW;
	SetWindowLongPtr ( m_hwndFocusWindow, GWL_STYLE, dwStyle );
	SetWindowPos
		(
		m_hwndFocusWindow,
		HWND_TOP,
		0,
		0,
		0,
		0,
		SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED
		);
	ProcessMessages();
	return hr;
}

bool PresentWinCSSmallWindow::PerformPresent(
	IDXGISwapChain *pSwapChain,
	UINT SyncInterval,
	UINT Flags )
{
	HRESULT hresult = E_FAIL;

	ActivateWindow
		(
		m_hwndDeviceWindow
		);

	bool bPresentSucceeded = true;

	__int64 start = 0, stop = 0;
	start = timeGetTime();

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory( &swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC) );
	if( FAILED( hresult = pSwapChain->GetDesc(&swapChainDesc) ) )
	{
		WriteToLog
				(
				"SwapChain GetDesc() failed with error 0x%x.",
				hresult
				);

		bPresentSucceeded = false;
		return bPresentSucceeded;	// Don't bother presenting. We don't know how to determine success without the desc.
	}

	// Win8: 733172: FLIP_SEQUENTIAL should never return DXGI_STATUS_OCCLUDED
	// Win8: 805721: 8IP can return DXGI_STATUS_OCCLUDED when Win7 + DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL + DWM is off
	bool bOccludedExpected = true;
	if(IsFlipModel(swapChainDesc.SwapEffect))
	{
		// Flip model swapchains should never return DXGI_STATUS_OCCLUDED...
		bOccludedExpected = false;

		// ...unless Win7 + DWM is off
		if( !m_bWin8OrHigher && !m_bDWM )
		{
			// Without DWM, Flip present falls back to a blit model, and is able to return occlusion results
			bOccludedExpected = true;
		}
	}

	for( UINT i = 0; i <m_nPresents; i++)
	{
		hresult = pSwapChain->Present(SyncInterval, Flags);
		DebugOut.Write("+++++ Present called. hresult=0x%x +++++\n", hresult);
		if( FAILED(hresult) )
		{
			WriteToLog
				(
				"Present failed with error 0x%x.",
				hresult
				);

			bPresentSucceeded = false;
			break;
		}
		else if ( bOccludedExpected &&
			(hresult != DXGI_STATUS_OCCLUDED) )
		{
			WriteToLog
				(
				"Present returned 0x%x when DXGI_STATUS_OCCLUDED was expected for 0 width or height windows",
				hresult
				);

			bPresentSucceeded = false;
			break;
		}
		else if ( !bOccludedExpected &&
			(hresult == DXGI_STATUS_OCCLUDED) )
		{
			WriteToLog
				(
				"Present returned 0x%x when DXGI_STATUS_OCCLUDED wasn't expected for 0 width or height windows",
				hresult
				);

			bPresentSucceeded = false;
			break;
		}
	}
	stop = timeGetTime();
	if ( bPresentSucceeded && stop >= start )
	{
		WriteToLog
				(
				"PresentWinCSSmallWindow::PerformPresent: %d Presents took %I64u", m_nPresents,
				stop - start
				);
	}
	return bPresentSucceeded;
}