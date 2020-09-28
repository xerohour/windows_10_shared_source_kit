//////////////////////////////////////////////////////////////////////
// File:  PresentDurationTest.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for DXGI presentation duration conformance test group
// class.
//
// History:
// 23 Mar 2013  SEdmison  Created.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Pragmas
//
/////////////////////////////////////////////////////////////////

#pragma once


/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Win32 headers
//

#include <windows.h>
#include <dxgi1_3.h>
#include <d3d11_1.h>

#define TEMP NTDDI_VERSION
#undef NTDDI_VERSION
#define NTDDI_VERSION NTDDI_WIN8
#include <dcomp.h>
#undef NTDDI_VERSION
#define NTDDI_VERSION TEMP


//
// C runtime headers
//

#include <assert.h>


//
// Project headers
//

#include "GraphicsTAEFTest.hpp"
#include "CustomPresentDurationEventProcessingThread.hpp"


//
// Test infrastructure headers
//

#include <DCompIsolation.h> // Isolates Win8 DCOMP code so we can still compile the test to run on Win7.
#include <CSysInfo.h>


/////////////////////////////////////////////////////////////////
//
// Type definitions
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Enum:  SWAPCHAIN_CREATION_METHOD
//
// Purpose:
///////////////////////////////////////////////////////
enum SWAPCHAIN_CREATION_METHOD
{
	SWAPCHAIN_CREATION_METHOD_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACE,
	SWAPCHAIN_CREATION_METHOD_CREATESWAPCHAINFORCOMPOSITIONSURFACE,
	SWAPCHAIN_CREATION_METHOD_CREATESWAPCHAINFORHWND
};


/////////////////////////////////////////////////////////////////
//
// Interfaces
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Class:  CPresentDurationTest
//
// Purpose:
// Test group class for presentation duration
// conformance tests.
///////////////////////////////////////////////////////
class CPresentDurationTest
:
	public CGraphicsTAEFTest
{
	//
	// Construction/destruction
	//

	public:
		// Constructor.
		CPresentDurationTest();

		// Destructor.
		virtual ~CPresentDurationTest();


	//
	// Public methods
	//

	public:
		// Declare this class as a test group.
		BEGIN_TEST_CLASS( CPresentDurationTest )
			// Test group execution metadata
			TEST_CLASS_PROPERTY( L"Kits.RunElevated", L"true" )

			// Logo/certification kit metadata
			TEST_CLASS_PROPERTY( L"Kits.Specification", L"Device.Graphics.WDDM13.DisplayRender.48HzVideoPlayback" )
		END_TEST_CLASS()

		virtual bool SetupTestGroup();
		TEST_CLASS_SETUP( SetupTestGroup );

		virtual bool CleanupTestGroup();
		TEST_CLASS_CLEANUP( CleanupTestGroup );

		virtual bool SetupTestCase();
		TEST_METHOD_SETUP( SetupTestCase );

		virtual bool CleanupTestCase();
		TEST_METHOD_CLEANUP( CleanupTestCase );


		BEGIN_TEST_METHOD( TestPresentDurationSupport_Native )
			// Test method general/descriptive metadata
			TEST_METHOD_PROPERTY( L"Kits.Description", L"This test verifies whether the system reports support for custom presentation durations." )

			// Test method logo/certification kit job metadata
			TEST_METHOD_PROPERTY( L"Kits.TestName", L"Test for Custom Presentation Duration Support (Native)" )
			TEST_METHOD_PROPERTY( L"Kits.ExpectedRuntime", L"2" )
			TEST_METHOD_PROPERTY( L"Kits.IsManual", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.Usage", L"This test verifies whether a WDDM 1.3 graphics driver reports support for custom presentation durations if and only if there are no connected external display devices." )

			TEST_METHOD_PROPERTY( L"Kits.ProvidesAssessmentResult", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.HasSupplementalContent", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.DevelopmentPhase", L"Tuning and Validation" )
			TEST_METHOD_PROPERTY( L"Kits.PublishingOrganization", L"Microsoft Corporation" )
			TEST_METHOD_PROPERTY( L"Kits.TestType", L"Development" )

			TEST_METHOD_PROPERTY( L"Kits.TestId", L"d14cbdb9-bb12-42f6-80bd-1a98a7793742" )
			TEST_METHOD_PROPERTY( L"Kits.TestId2", L"9f3d5fed-2606-4bc6-a48a-0d561258ca08" )
			TEST_METHOD_PROPERTY( L"Kits.TimeoutInMinutes", L"4" )

			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Client x86" )
			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Client x64" )
			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Server x64" )
			TEST_METHOD_PROPERTY( L"Kits.MinRelease", L"TH1" )
			TEST_METHOD_PROPERTY( L"Kits.CorePackageComposition", L"Full" )

			//TEST_METHOD_PROPERTY( L"DeploymentItem", L"NTTEST\\windowstest\\graphics\\foo\\bar\\baz.dll" )
		END_TEST_METHOD();


		BEGIN_TEST_METHOD( TestPresentDurationSupport_WoW64 )
			// Test method general/descriptive metadata
			TEST_METHOD_PROPERTY( L"Kits.Description", L"This test verifies whether the system reports support for custom presentation durations." )

			// Test method logo/certification kit job metadata
			TEST_METHOD_PROPERTY( L"Kits.TestName", L"Test for Custom Presentation Duration Support (WoW64)" )
			TEST_METHOD_PROPERTY( L"Kits.ExpectedRuntime", L"2" )
			TEST_METHOD_PROPERTY( L"Kits.IsManual", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.Usage", L"This test verifies whether a WDDM 1.3 graphics driver reports support for custom presentation durations if and only if there are no connected external display devices." )

			TEST_METHOD_PROPERTY( L"Kits.ProvidesAssessmentResult", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.HasSupplementalContent", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.DevelopmentPhase", L"Tuning and Validation" )
			TEST_METHOD_PROPERTY( L"Kits.PublishingOrganization", L"Microsoft Corporation" )
			TEST_METHOD_PROPERTY( L"Kits.TestType", L"Development" )

			TEST_METHOD_PROPERTY( L"Kits.TestId", L"62cbf58c-c18f-499c-af47-8261aab8f4e1" )
			TEST_METHOD_PROPERTY( L"Kits.TestId2", L"debf7dd1-0450-41db-87cc-e9e02f5a6a8a" )
			TEST_METHOD_PROPERTY( L"Kits.TimeoutInMinutes", L"4" )

			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Client x64" )
			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Server x64" )
			TEST_METHOD_PROPERTY( L"Kits.MinRelease", L"TH1" )
			TEST_METHOD_PROPERTY( L"Kits.CorePackageComposition", L"Full" )

			//TEST_METHOD_PROPERTY(L"DeploymentItem", L"NTTEST\\windowstest\\graphics\\foo\\bar\\baz.dll")
		END_TEST_METHOD();


		BEGIN_TEST_METHOD( TestPresentDurationPrecision_Native )
			// Test method general/descriptive metadata
			TEST_METHOD_PROPERTY( L"Kits.Description", L"This test verifies that a system which supports custom presentation durations precisely honors a specified duration." )

			// Test method logo/certification kit job metadata
			TEST_METHOD_PROPERTY( L"Kits.TestName", L"Test for Custom Presentation Duration Precision (Native)" )
			TEST_METHOD_PROPERTY( L"Kits.ExpectedRuntime", L"2" )
			TEST_METHOD_PROPERTY( L"Kits.IsManual", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.Usage", L"This test verifies whether a WDDM 1.3 graphics driver that support custom presentation durations accurately and precisely honors a specified duration." )

			TEST_METHOD_PROPERTY( L"Kits.ProvidesAssessmentResult", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.HasSupplementalContent", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.DevelopmentPhase", L"Tuning and Validation" )
			TEST_METHOD_PROPERTY( L"Kits.PublishingOrganization", L"Microsoft Corporation" )
			TEST_METHOD_PROPERTY( L"Kits.TestType", L"Development" )

			TEST_METHOD_PROPERTY( L"Kits.TestId", L"6a433077-8026-4feb-9287-9fc02cdff78a" )
			TEST_METHOD_PROPERTY( L"Kits.TestId2", L"c53c05a4-3f95-445d-9c78-360222dd90b2" )
			TEST_METHOD_PROPERTY( L"Kits.TimeoutInMinutes", L"4" )

			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Client x86" )
			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Client x64" )
			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Server x64" )
			TEST_METHOD_PROPERTY( L"Kits.MinRelease", L"TH1" )
			TEST_METHOD_PROPERTY( L"Kits.CorePackageComposition", L"Full" )

			//TEST_METHOD_PROPERTY( L"DeploymentItem", L"NTTEST\\windowstest\\graphics\\foo\\bar\\baz.dll" )
		END_TEST_METHOD();


		BEGIN_TEST_METHOD( TestPresentDurationPrecision_WoW64 )
			// Test method general/descriptive metadata
			TEST_METHOD_PROPERTY( L"Kits.Description", L"This test verifies that a system which supports custom presentation durations precisely honors a specified duration." )

			// Test method logo/certification kit job metadata
			TEST_METHOD_PROPERTY( L"Kits.TestName", L"Test for Custom Presentation Duration Precision (WoW64)" )
			TEST_METHOD_PROPERTY( L"Kits.ExpectedRuntime", L"2" )
			TEST_METHOD_PROPERTY( L"Kits.IsManual", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.Usage", L"This test verifies whether a WDDM 1.3 graphics driver that support custom presentation durations accurately and precisely honors a specified duration." )

			TEST_METHOD_PROPERTY( L"Kits.ProvidesAssessmentResult", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.HasSupplementalContent", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.DevelopmentPhase", L"Tuning and Validation" )
			TEST_METHOD_PROPERTY( L"Kits.PublishingOrganization", L"Microsoft Corporation" )
			TEST_METHOD_PROPERTY( L"Kits.TestType", L"Development" )

			TEST_METHOD_PROPERTY( L"Kits.TestId", L"fcf3e81e-8fdc-4d46-80e0-2195ac218938" )
			TEST_METHOD_PROPERTY( L"Kits.TestId2", L"da8d14fc-811f-49dc-87e5-571255aa32fe" )
			TEST_METHOD_PROPERTY( L"Kits.TimeoutInMinutes", L"4" )

			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Client x64" )
			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Server x64" )
			TEST_METHOD_PROPERTY( L"Kits.MinRelease", L"TH1" )
			TEST_METHOD_PROPERTY( L"Kits.CorePackageComposition", L"Full" )

			//TEST_METHOD_PROPERTY(L"DeploymentItem", L"NTTEST\\windowstest\\graphics\\foo\\bar\\baz.dll")
		END_TEST_METHOD();


	//
	// Helper methods
	//

	protected:
		void LogTestCaseParameters();


		LRESULT MsgProc
		(
			__in HWND hWindow,
			UINT Message,
			WPARAM wParam,
			LPARAM lParam
		);


		void TestPresentDurationPrecision
		(
			bool bRunUnderWoW64
		);

		void TestPresentDurationSupport
		(
			bool bRunUnderWoW64
		);


		HRESULT BeginETWTrace();

		HRESULT EndETWTrace();


		bool VerifySupportForCustomPresentDuration
		(
			UINT RequestedCustomPresentDuration
		);


	//
	// Static methods
	//

	protected:
		static LRESULT WINAPI StaticMsgProc
		(
			__in HWND hWindow,
			UINT Message,
			WPARAM wParam,
			LPARAM lParam
		);


	//
	// Member data
	//

	private:
		// GDI members and state
		ATOM m_WindowClassAtom;
		HWND m_hWindow;

		// DXGI/Direct3D 11 members and state
        IDXGIFactory3 * m_pFactory;
        IDXGIFactoryMedia * m_pFactoryMedia;

		SWAPCHAIN_CREATION_METHOD m_SwapChainCreationMethod;
		IDXGISwapChain1 * m_pSwapChain_IDXGISwapChain1;
		IDXGIDecodeSwapChain * m_pSwapChain_IDXGIDecodeSwapChain;
		IDXGISwapChainMedia * m_pSwapChain_IDXGISwapChainMedia;

		ID3D11Device * m_pDevice;
		ID3D11DeviceContext * m_pImmediateContext;

		ID3D11Texture2D * m_pYUVDecodingBufferArray;

		UINT m_SwapChainBufferWidth;
		UINT m_SwapChainBufferHeight;
		DXGI_FORMAT m_SwapChainBufferFormat;
		UINT m_SwapChainBufferBindFlags;
		const UINT m_SwapChainBufferArraySize;
		UINT m_SwapChainBufferMiscFlags;

		// DirectComposition members and state
		HANDLE m_hCompositionSurface;

		// DirectComposition isolation object
		CDCompIsolation m_DCompIsolation;

		// SysInfo object
		CSysInfo m_SysInfo;

		// ETW event processing thread
		CCustomPresentDurationEventProcessingThread m_EventProcessingThread;
		TRACEHANDLE m_hEventTracingSession;
};
