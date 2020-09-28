//////////////////////////////////////////////////////////////////////
// File:  CSwapChainsTest.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for DXGI swap chain conformance test group class.
//
// History:
// 07 Nov 2013  SEdmison  Created.
// 12 Dec 2013  SEdmison  Revamped to work inside tailored process.
// 12 Aug 2015  SEdmison  Factored Direct3D scene renderer into D3D 11
//                        and 12 versions.
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

#include <Windows.h>
#include <DXGI1_4.h>
#include <D3D12.h>

#pragma warning( push )
#pragma warning( disable: 28922 )

#define TEMP NTDDI_VERSION
#undef NTDDI_VERSION
#define NTDDI_VERSION NTDDI_WIN8

#include <DComp.h>

#include <ShlObj.h>
#include <KnownFolders.h>

#undef NTDDI_VERSION
#define NTDDI_VERSION TEMP

#pragma warning( pop )


//
// C runtime headers
//

#include <assert.h>


//
// STL headers
//

#include <map>
#include <vector>


//
// Test infrastructure headers
//

#include <CSysInfo.h>

// Isolates DirectComposition references so that we can maintain
// the option to build tests for Windows 7 and run non-DComp
// variations down-level.
#include <DCompIsolation.h>

#if USE_DWMSLICER
#include <DWMSlicer.h>
#include <DirectX.h>
#endif

#if USE_SCREENCAPTURE
#include "CScreenCapture.hpp"
#endif


//
// Project headers
//

#define TEMP NTDDI_VERSION
#undef NTDDI_VERSION
#define NTDDI_VERSION NTDDI_WIN8
#include "CImmersiveWindowWrapper.hpp"
#undef NTDDI_VERSION
#define NTDDI_VERSION TEMP

#include "CSystemWindowWrapper.hpp"

#include "GraphicsTAEFTest.hpp"

#include "CD3D11SceneRenderer.hpp"
#include "CD3D12SceneRenderer.hpp"


/////////////////////////////////////////////////////////////////
//
// Type definitions
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Class:  ExpectedTexelColor
//
// Purpose:
// Expresses the expectation that a specific point in a
// captured frame be a specific color.
///////////////////////////////////////////////////////
class ExpectedTexelColor
{
	//
	// Construction/destruction
	//

	public:
		ExpectedTexelColor()
		:
			m_RelativeX( 0.f ),
			m_RelativeY( 0.f )
		{
			ZeroMemory( &m_ExpectedColor, sizeof( m_ExpectedColor ) );
		};


		ExpectedTexelColor
		(
			float RelativeX,
			float RelativeY,
			const FLOAT_COLOR & ExpectedColor
		)
		{
			assert( RelativeX >= 0.f );
			assert( RelativeX <= 1.f );
			assert( RelativeY >= 0.f );
			assert( RelativeY <= 1.f );

			m_RelativeX = RelativeX;
			m_RelativeY = RelativeY;
			m_ExpectedColor = ExpectedColor;
		};


		ExpectedTexelColor
		(
			float RelativeX,
			float RelativeY,
			float ExpectedColorRed,
			float ExpectedColorGreen,
			float ExpectedColorBlue,
			float ExpectedColorAlpha
		)
		{
			assert( RelativeX >= 0.f );
			assert( RelativeX <= 1.f );
			assert( RelativeY >= 0.f );
			assert( RelativeY <= 1.f );

			m_RelativeX = RelativeX;
			m_RelativeY = RelativeY;

			m_ExpectedColor.r = ExpectedColorRed;
			m_ExpectedColor.g = ExpectedColorGreen;
			m_ExpectedColor.b = ExpectedColorBlue;
			m_ExpectedColor.a = ExpectedColorAlpha;
		};


	//
	// Member data
	//

	public:
		// Coordinates as a fraction of scene size.
		float m_RelativeX;
		float m_RelativeY;

		FLOAT_COLOR m_ExpectedColor;
};


/////////////////////////////////////////////////////////////////
//
// Interfaces
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Test class interfaces
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Class:  CSwapChainsTest
//
// Purpose:
// Test group class for swap chain conformance tests.
///////////////////////////////////////////////////////
class CSwapChainsTest
:
	public CGraphicsTAEFTest
{
	//
	// Construction/destruction
	//

	public:
		// Constructor.
		CSwapChainsTest();

		// Destructor.
		virtual ~CSwapChainsTest();


	//
	// Public methods
	//

	public:
		// Declare this class as a test group.
		BEGIN_TEST_CLASS( CSwapChainsTest )
			// Logo/certification kit metadata
			TEST_CLASS_PROPERTY( L"Kits.Specification", L"Device.Graphics.WDDM13.DisplayRender.MultiPlaneOverlaySupport" )
		END_TEST_CLASS()


		//
		// Test-group-wide setup and cleanup fixtures
		//

		virtual bool SetupTestGroup();
		TEST_CLASS_SETUP( SetupTestGroup );

		virtual bool CleanupTestGroup();
		TEST_CLASS_CLEANUP( CleanupTestGroup );


		//
		// Per-test-case setup and cleanup fixtures
		//

		virtual bool SetupTestCase();
		TEST_METHOD_SETUP( SetupTestCase );

		virtual bool CleanupTestCase();
		TEST_METHOD_CLEANUP( CleanupTestCase );


		//
		// Test methods
		//

		BEGIN_TEST_METHOD( TestCreateDecodeSwapChainForCompositionSurfaceHandle_Native )
#if TEST_DECODE_SWAP_CHAINS_UNDER_D3D12
			TEST_METHOD_PROPERTY( L"Data:RenderingAPI", L"{TEST_API_D3D11, TEST_API_D3D12}" )
#else
			TEST_METHOD_PROPERTY( L"Data:RenderingAPI", L"{TEST_API_D3D11}" )
#endif
			TEST_METHOD_PROPERTY( L"Data:SceneType", L"{SCENE_TYPE_CUBE_SHADED, SCENE_TYPE_PLANE_TEXTURED, SCENE_TYPE_SWATCH_FIXEDCOLOR, SCENE_TYPE_PATTERN}" )

			// Test method general/descriptive metadata
			TEST_METHOD_PROPERTY( L"Kits.Description", L"This test case verifies that content rendered by a swap chain created with CreateDecodeSwapChainForCompositionSurfaceHandle is correct." )

			// Test method logo/certification kit job metadata
			TEST_METHOD_PROPERTY( L"Kits.TestName", L"Test CreateDecodeSwapChain For CompositionSurfaceHandle (Native)" )
			TEST_METHOD_PROPERTY( L"Kits.ExpectedRuntime", L"2" )
			TEST_METHOD_PROPERTY( L"Kits.IsManual", L"false" )

			TEST_METHOD_PROPERTY( L"Kits.ProvidesAssessmentResult", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.HasSupplementalContent", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.DevelopmentPhase", L"Tuning and Validation" )
			TEST_METHOD_PROPERTY( L"Kits.PublishingOrganization", L"Microsoft Corporation" )
			TEST_METHOD_PROPERTY( L"Kits.TestType", L"Development" )

			TEST_METHOD_PROPERTY( L"Kits.TestId", L"df4356cf-8556-46ac-965c-d6a63f1d9665" )
			TEST_METHOD_PROPERTY( L"Kits.TestId2", L"505650ba-3f64-400d-ad77-eca811f8eced" )
			TEST_METHOD_PROPERTY( L"Kits.TimeoutInMinutes", L"4" )

			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Client x86" )
			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Client x64" )
			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Server x64" )
			TEST_METHOD_PROPERTY( L"Kits.MinRelease", L"TH1" )
			TEST_METHOD_PROPERTY( L"Kits.CorePackageComposition", L"Full" )
		END_TEST_METHOD();


		BEGIN_TEST_METHOD( TestCreateDecodeSwapChainForCompositionSurfaceHandle_WoW64 )
#if TEST_DECODE_SWAP_CHAINS_UNDER_D3D12
			TEST_METHOD_PROPERTY( L"Data:RenderingAPI", L"{TEST_API_D3D11, TEST_API_D3D12}" )
#else
			TEST_METHOD_PROPERTY( L"Data:RenderingAPI", L"{TEST_API_D3D11}" )
#endif
			TEST_METHOD_PROPERTY( L"Data:SceneType", L"{SCENE_TYPE_CUBE_SHADED, SCENE_TYPE_PLANE_TEXTURED, SCENE_TYPE_SWATCH_FIXEDCOLOR, SCENE_TYPE_PATTERN}" )

			// Test method general/descriptive metadata
			TEST_METHOD_PROPERTY( L"Kits.Description", L"This test case verifies that content rendered by a swap chain created with CreateDecodeSwapChainForCompositionSurfaceHandle is correct." )

			// Test method logo/certification kit job metadata
			TEST_METHOD_PROPERTY( L"Kits.TestName", L"Test CreateDecodeSwapChain For CompositionSurfaceHandle (WoW64)" )
			TEST_METHOD_PROPERTY( L"Kits.ExpectedRuntime", L"2" )
			TEST_METHOD_PROPERTY( L"Kits.IsManual", L"false" )

			TEST_METHOD_PROPERTY( L"Kits.ProvidesAssessmentResult", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.HasSupplementalContent", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.DevelopmentPhase", L"Tuning and Validation" )
			TEST_METHOD_PROPERTY( L"Kits.PublishingOrganization", L"Microsoft Corporation" )
			TEST_METHOD_PROPERTY( L"Kits.TestType", L"Development" )

			TEST_METHOD_PROPERTY( L"Kits.TestId", L"bcc341aa-f043-4cfd-b417-8ffaae7168f3" )
			TEST_METHOD_PROPERTY( L"Kits.TestId2", L"c91e8bfb-c2ad-455b-a588-e9cf6d4d014b" )
			TEST_METHOD_PROPERTY( L"Kits.TimeoutInMinutes", L"4" )

			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Client x64" )
			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Server x64" )
			TEST_METHOD_PROPERTY( L"Kits.MinRelease", L"TH1" )
			TEST_METHOD_PROPERTY( L"Kits.CorePackageComposition", L"Full" )

			TEST_METHOD_PROPERTY( L"Kits.IsWoW64Test", L"true" )
		END_TEST_METHOD();


		BEGIN_TEST_METHOD( TestCreateSwapChain_Native )
			TEST_METHOD_PROPERTY( L"Data:RenderingAPI", L"{TEST_API_D3D11, TEST_API_D3D12}" )
			TEST_METHOD_PROPERTY( L"Data:SceneType", L"{SCENE_TYPE_CUBE_SHADED, SCENE_TYPE_PLANE_TEXTURED, SCENE_TYPE_SWATCH_FIXEDCOLOR, SCENE_TYPE_PATTERN}" )

			// Test method general/descriptive metadata
			TEST_METHOD_PROPERTY( L"Kits.Description", L"This test case verifies that content rendered by a swap chain created with CreateSwapChain is correct." )

			// Test method logo/certification kit job metadata
			TEST_METHOD_PROPERTY( L"Kits.TestName", L"Test CreateSwapChain (Native)" )
			TEST_METHOD_PROPERTY( L"Kits.ExpectedRuntime", L"2" )
			TEST_METHOD_PROPERTY( L"Kits.IsManual", L"false" )

			TEST_METHOD_PROPERTY( L"Kits.ProvidesAssessmentResult", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.HasSupplementalContent", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.DevelopmentPhase", L"Tuning and Validation" )
			TEST_METHOD_PROPERTY( L"Kits.PublishingOrganization", L"Microsoft Corporation" )
			TEST_METHOD_PROPERTY( L"Kits.TestType", L"Development" )

			TEST_METHOD_PROPERTY( L"Kits.TestId", L"85978b6f-70ce-4ff1-8c53-825f1fe54608" )
			TEST_METHOD_PROPERTY( L"Kits.TestId2", L"aa9379d2-c6e5-416d-909c-9db743ddbe42" )
			TEST_METHOD_PROPERTY( L"Kits.TimeoutInMinutes", L"4" )

			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Client x86" )
			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Client x64" )
			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Server x64" )
			TEST_METHOD_PROPERTY( L"Kits.MinRelease", L"TH1" )
			TEST_METHOD_PROPERTY( L"Kits.CorePackageComposition", L"Full" )
		END_TEST_METHOD();


		BEGIN_TEST_METHOD( TestCreateSwapChain_WoW64 )
			TEST_METHOD_PROPERTY( L"Data:RenderingAPI", L"{TEST_API_D3D11, TEST_API_D3D12}" )
			TEST_METHOD_PROPERTY( L"Data:SceneType", L"{SCENE_TYPE_CUBE_SHADED, SCENE_TYPE_PLANE_TEXTURED, SCENE_TYPE_SWATCH_FIXEDCOLOR, SCENE_TYPE_PATTERN}" )

			// Test method general/descriptive metadata
			TEST_METHOD_PROPERTY( L"Kits.Description", L"This test case verifies that content rendered by a swap chain created with CreateSwapChain is correct." )

			// Test method logo/certification kit job metadata
			TEST_METHOD_PROPERTY( L"Kits.TestName", L"Test CreateSwapChain (WoW64)" )
			TEST_METHOD_PROPERTY( L"Kits.ExpectedRuntime", L"2" )
			TEST_METHOD_PROPERTY( L"Kits.IsManual", L"false" )

			TEST_METHOD_PROPERTY( L"Kits.ProvidesAssessmentResult", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.HasSupplementalContent", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.DevelopmentPhase", L"Tuning and Validation" )
			TEST_METHOD_PROPERTY( L"Kits.PublishingOrganization", L"Microsoft Corporation" )
			TEST_METHOD_PROPERTY( L"Kits.TestType", L"Development" )

			TEST_METHOD_PROPERTY( L"Kits.TestId", L"89c9f115-3218-4009-8c91-aa8881825c78" )
			TEST_METHOD_PROPERTY( L"Kits.TestId2", L"41a575a4-e717-47d6-b56b-1a8204c12453" )
			TEST_METHOD_PROPERTY( L"Kits.TimeoutInMinutes", L"4" )

			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Client x64" )
			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Server x64" )
			TEST_METHOD_PROPERTY( L"Kits.MinRelease", L"TH1" )
			TEST_METHOD_PROPERTY( L"Kits.CorePackageComposition", L"Full" )

			TEST_METHOD_PROPERTY( L"Kits.IsWoW64Test", L"true" )
		END_TEST_METHOD();


		BEGIN_TEST_METHOD( TestCreateSwapChainForCompositionSurfaceHandle_Native )
			TEST_METHOD_PROPERTY( L"Data:RenderingAPI", L"{TEST_API_D3D11, TEST_API_D3D12}" )
			TEST_METHOD_PROPERTY( L"Data:SceneType", L"{SCENE_TYPE_CUBE_SHADED, SCENE_TYPE_PLANE_TEXTURED, SCENE_TYPE_SWATCH_FIXEDCOLOR, SCENE_TYPE_PATTERN}" )

			// Test method general/descriptive metadata
			TEST_METHOD_PROPERTY( L"Kits.Description", L"This test case verifies that content rendered by a swap chain created with CreateSwapChainForCompositionSurfaceHandle is correct." )

			// Test method logo/certification kit job metadata
			TEST_METHOD_PROPERTY( L"Kits.TestName", L"Test CreateSwapChain For CompositionSurfaceHandle (Native)" )
			TEST_METHOD_PROPERTY( L"Kits.ExpectedRuntime", L"2" )
			TEST_METHOD_PROPERTY( L"Kits.IsManual", L"false" )

			TEST_METHOD_PROPERTY( L"Kits.ProvidesAssessmentResult", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.HasSupplementalContent", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.DevelopmentPhase", L"Tuning and Validation" )
			TEST_METHOD_PROPERTY( L"Kits.PublishingOrganization", L"Microsoft Corporation" )
			TEST_METHOD_PROPERTY( L"Kits.TestType", L"Development" )

			TEST_METHOD_PROPERTY( L"Kits.TestId", L"48efe820-9334-4676-9504-b02ca99d8427" )
			TEST_METHOD_PROPERTY( L"Kits.TestId2", L"3c031c8a-327b-4307-8d4c-904571dd7eae" )
			TEST_METHOD_PROPERTY( L"Kits.TimeoutInMinutes", L"4" )

			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Client x86" )
			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Client x64" )
			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Server x64" )
			TEST_METHOD_PROPERTY( L"Kits.MinRelease", L"TH1" )
			TEST_METHOD_PROPERTY( L"Kits.CorePackageComposition", L"Full" )
		END_TEST_METHOD();


		BEGIN_TEST_METHOD( TestCreateSwapChainForCompositionSurfaceHandle_WoW64 )
			TEST_METHOD_PROPERTY( L"Data:RenderingAPI", L"{TEST_API_D3D11, TEST_API_D3D12}" )
			TEST_METHOD_PROPERTY( L"Data:SceneType", L"{SCENE_TYPE_CUBE_SHADED, SCENE_TYPE_PLANE_TEXTURED, SCENE_TYPE_SWATCH_FIXEDCOLOR, SCENE_TYPE_PATTERN}" )

			// Test method general/descriptive metadata
			TEST_METHOD_PROPERTY( L"Kits.Description", L"This test case verifies that content rendered by a swap chain created with CreateSwapChainForCompositionSurfaceHandle is correct." )

			// Test method logo/certification kit job metadata
			TEST_METHOD_PROPERTY( L"Kits.TestName", L"Test CreateSwapChain For CompositionSurfaceHandle (WoW64)" )
			TEST_METHOD_PROPERTY( L"Kits.ExpectedRuntime", L"2" )
			TEST_METHOD_PROPERTY( L"Kits.IsManual", L"false" )

			TEST_METHOD_PROPERTY( L"Kits.ProvidesAssessmentResult", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.HasSupplementalContent", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.DevelopmentPhase", L"Tuning and Validation" )
			TEST_METHOD_PROPERTY( L"Kits.PublishingOrganization", L"Microsoft Corporation" )
			TEST_METHOD_PROPERTY( L"Kits.TestType", L"Development" )

			TEST_METHOD_PROPERTY( L"Kits.TestId", L"79419659-4ab2-4f95-97c2-585ff47bbab5" )
			TEST_METHOD_PROPERTY( L"Kits.TestId2", L"23868a2a-498a-4a00-a8cb-2c245ba4815a" )
			TEST_METHOD_PROPERTY( L"Kits.TimeoutInMinutes", L"4" )

			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Client x64" )
			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Server x64" )
			TEST_METHOD_PROPERTY( L"Kits.MinRelease", L"TH1" )
			TEST_METHOD_PROPERTY( L"Kits.CorePackageComposition", L"Full" )

			TEST_METHOD_PROPERTY( L"Kits.IsWoW64Test", L"true" )
		END_TEST_METHOD();


		BEGIN_TEST_METHOD( TestCreateSwapChainForHwnd_Native )
			TEST_METHOD_PROPERTY( L"Data:RenderingAPI", L"{TEST_API_D3D11, TEST_API_D3D12}" )
			TEST_METHOD_PROPERTY( L"Data:SceneType", L"{SCENE_TYPE_CUBE_SHADED, SCENE_TYPE_PLANE_TEXTURED, SCENE_TYPE_SWATCH_FIXEDCOLOR, SCENE_TYPE_PATTERN}" )

			// Test method general/descriptive metadata
			TEST_METHOD_PROPERTY( L"Kits.Description", L"This test case verifies that content rendered by a swap chain created with CreateSwapChainForHwnd is correct." )

			// Test method logo/certification kit job metadata
			TEST_METHOD_PROPERTY( L"Kits.TestName", L"Test CreateSwapChainForHwnd (Native)" )
			TEST_METHOD_PROPERTY( L"Kits.ExpectedRuntime", L"2" )
			TEST_METHOD_PROPERTY( L"Kits.IsManual", L"false" )

			TEST_METHOD_PROPERTY( L"Kits.ProvidesAssessmentResult", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.HasSupplementalContent", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.DevelopmentPhase", L"Tuning and Validation" )
			TEST_METHOD_PROPERTY( L"Kits.PublishingOrganization", L"Microsoft Corporation" )
			TEST_METHOD_PROPERTY( L"Kits.TestType", L"Development" )

			TEST_METHOD_PROPERTY( L"Kits.TestId", L"84d3569c-71d5-4598-a29d-9d98308373d1" )
			TEST_METHOD_PROPERTY( L"Kits.TestId2", L"fcf14bb4-c853-4e72-a627-23f714136e2b" )
			TEST_METHOD_PROPERTY( L"Kits.TimeoutInMinutes", L"4" )

			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Client x86" )
			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Client x64" )
			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Server x64" )
			TEST_METHOD_PROPERTY( L"Kits.MinRelease", L"TH1" )
			TEST_METHOD_PROPERTY( L"Kits.CorePackageComposition", L"Full" )
		END_TEST_METHOD();


		BEGIN_TEST_METHOD( TestCreateSwapChainForHwnd_WoW64 )
			TEST_METHOD_PROPERTY( L"Data:RenderingAPI", L"{TEST_API_D3D11, TEST_API_D3D12}" )
			TEST_METHOD_PROPERTY( L"Data:SceneType", L"{SCENE_TYPE_CUBE_SHADED, SCENE_TYPE_PLANE_TEXTURED, SCENE_TYPE_SWATCH_FIXEDCOLOR, SCENE_TYPE_PATTERN}" )

			// Test method general/descriptive metadata
			TEST_METHOD_PROPERTY( L"Kits.Description", L"This test case verifies that content rendered by a swap chain created with CreateSwapChainForHwnd is correct." )

			// Test method logo/certification kit job metadata
			TEST_METHOD_PROPERTY( L"Kits.TestName", L"Test CreateSwapChainForHwnd (WoW64)" )
			TEST_METHOD_PROPERTY( L"Kits.ExpectedRuntime", L"2" )
			TEST_METHOD_PROPERTY( L"Kits.IsManual", L"false" )

			TEST_METHOD_PROPERTY( L"Kits.ProvidesAssessmentResult", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.HasSupplementalContent", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.DevelopmentPhase", L"Tuning and Validation" )
			TEST_METHOD_PROPERTY( L"Kits.PublishingOrganization", L"Microsoft Corporation" )
			TEST_METHOD_PROPERTY( L"Kits.TestType", L"Development" )

			TEST_METHOD_PROPERTY( L"Kits.TestId", L"9ebb8466-c491-44cf-8dbc-7c5b8bebbef3" )
			TEST_METHOD_PROPERTY( L"Kits.TestId2", L"6a3f002c-fa13-4fb6-a93d-e4dcea230300" )
			TEST_METHOD_PROPERTY( L"Kits.TimeoutInMinutes", L"4" )

			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Client x64" )
			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Server x64" )
			TEST_METHOD_PROPERTY( L"Kits.MinRelease", L"TH1" )
			TEST_METHOD_PROPERTY( L"Kits.CorePackageComposition", L"Full" )

			TEST_METHOD_PROPERTY( L"Kits.IsWoW64Test", L"true" )
		END_TEST_METHOD();


		BEGIN_TEST_METHOD( TestCreateSwapChainForCoreWindow_Overlapped_Native )
			TEST_METHOD_PROPERTY( L"Data:RenderingAPI", L"{TEST_API_D3D11, TEST_API_D3D12}" )
			TEST_METHOD_PROPERTY( L"Data:SceneType", L"{SCENE_TYPE_CUBE_SHADED, SCENE_TYPE_PLANE_TEXTURED, SCENE_TYPE_SWATCH_FIXEDCOLOR, SCENE_TYPE_PATTERN}" )

			// Test method execution metadata
			TEST_METHOD_PROPERTY( L"RunAs", L"Tailored" )

			// Test method general/descriptive metadata
			TEST_METHOD_PROPERTY( L"Kits.Description", L"This test verifies that content rendered by overlapped swap chains is correctly blended by DWM or overlay hardware." )

			// Test method logo/certification kit job metadata
			TEST_METHOD_PROPERTY( L"Kits.TestName", L"Test for Overlapped Swap Chain Blending (Native)" )
			TEST_METHOD_PROPERTY( L"Kits.ExpectedRuntime", L"2" )
			TEST_METHOD_PROPERTY( L"Kits.IsManual", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.Usage", L"This test renders content from multiple overlapping swap chains in order to verify that the fully composed output is correctly blended." )

			TEST_METHOD_PROPERTY( L"Kits.ProvidesAssessmentResult", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.HasSupplementalContent", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.DevelopmentPhase", L"Tuning and Validation" )
			TEST_METHOD_PROPERTY( L"Kits.PublishingOrganization", L"Microsoft Corporation" )
			TEST_METHOD_PROPERTY( L"Kits.TestType", L"Development" )

			TEST_METHOD_PROPERTY( L"Kits.TestId", L"142a4031-33f9-4399-ad7d-63baa530d190" )
			TEST_METHOD_PROPERTY( L"Kits.TestId2", L"98d46db5-3c17-45f9-8e50-add9756dcc8e" )
			TEST_METHOD_PROPERTY( L"Kits.TimeoutInMinutes", L"4" )

			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Client x86" )
			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Client x64" )
			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Server x64" )
			TEST_METHOD_PROPERTY( L"Kits.MinRelease", L"TH1" )
			TEST_METHOD_PROPERTY( L"Kits.CorePackageComposition", L"Full" )
		END_TEST_METHOD();


		BEGIN_TEST_METHOD( TestCreateSwapChainForCoreWindow_Overlapped_WoW64 )
			TEST_METHOD_PROPERTY( L"Data:RenderingAPI", L"{TEST_API_D3D11, TEST_API_D3D12}" )
			TEST_METHOD_PROPERTY( L"Data:SceneType", L"{SCENE_TYPE_CUBE_SHADED, SCENE_TYPE_PLANE_TEXTURED, SCENE_TYPE_SWATCH_FIXEDCOLOR, SCENE_TYPE_PATTERN}" )

			// Test method execution metadata
			TEST_METHOD_PROPERTY( L"RunAs", L"Tailored" )

			// Test method general/descriptive metadata
			TEST_METHOD_PROPERTY( L"Kits.Description", L"This test verifies that content rendered by overlapped swap chains is correctly blended by DWM or overlay hardware." )

			// Test method logo/certification kit job metadata
			TEST_METHOD_PROPERTY( L"Kits.TestName", L"Test for Overlapped Swap Chain Blending (WoW64)" )
			TEST_METHOD_PROPERTY( L"Kits.ExpectedRuntime", L"2" )
			TEST_METHOD_PROPERTY( L"Kits.IsManual", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.Usage", L"This test renders content from multiple overlapping swap chains in order to verify that the fully composed output is correctly blended." )

			TEST_METHOD_PROPERTY( L"Kits.ProvidesAssessmentResult", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.HasSupplementalContent", L"false" )
			TEST_METHOD_PROPERTY( L"Kits.DevelopmentPhase", L"Tuning and Validation" )
			TEST_METHOD_PROPERTY( L"Kits.PublishingOrganization", L"Microsoft Corporation" )
			TEST_METHOD_PROPERTY( L"Kits.TestType", L"Development" )

			TEST_METHOD_PROPERTY( L"Kits.TestId", L"c3f9d0be-c329-44db-8bf8-7b8b6114688e" )
			TEST_METHOD_PROPERTY( L"Kits.TestId2", L"7458e0ce-9553-41c1-bbe2-5351a5b8eb83" )
			TEST_METHOD_PROPERTY( L"Kits.TimeoutInMinutes", L"4" )

			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Client x64" )
			TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Server x64" )
			TEST_METHOD_PROPERTY( L"Kits.MinRelease", L"TH1" )
			TEST_METHOD_PROPERTY( L"Kits.CorePackageComposition", L"Full" )

			TEST_METHOD_PROPERTY( L"Kits.IsWoW64Test", L"true" )
		END_TEST_METHOD();


	//
	// Verification-related helper methods
	//

	protected:
#if USE_DWMSLICER
		// Retrieve a texel value as a FLOAT_COLOR.  Used in presentation validation.
		virtual bool GetTexel
		(
			UINT X,
			UINT Y,
			DXGI_FORMAT Format,
			_In_ const D3D11_MAPPED_SUBRESOURCE & MappedSubresource,	
			_Out_ FLOAT_COLOR * pFloatColor
		);

		virtual bool GetTexel
		(
			UINT iCurrentFrame,
			UINT FrameWidth,
			UINT FrameHeight,
			UINT X,
			UINT Y,
			_Out_ FLOAT_COLOR * pFloatColor
		);

		virtual HRESULT IsAtlasNonEmpty
		(
			UINT FrameWidth,
			UINT FrameHeight
		);

		virtual HRESULT IsFrameNonEmpty
		(
			UINT iCurrentFrame,
			UINT FrameWidth,
			UINT FrameHeight,
			DXGI_FORMAT FrameFormat,
			const D3D11_MAPPED_SUBRESOURCE & MappedSubresource
		);
#endif


		virtual void SetExpectedResults();


	//
	// Miscellaneous helper methods
	//

	protected:
		HRESULT GetDesktopCoordinatesForDefaultOutput
		(
			_Out_ LONG * pDefaultOutputLeft,
			_Out_ LONG * pDefaultOutputTop,
			_Out_ LONG * pDefaultOutputRight,
			_Out_ LONG * pDefaultOutputBottom
		);


		void LogTestCaseParameters();


		void TestSwapChainRendering
		(
			bool bRunUnderWoW64,
			SWAPCHAIN_TYPE SwapChainType
		);


	//
	// Static helper methods
	//

	private:
		static BOOL CALLBACK MonitorEnumProc
		(
			_In_ HMONITOR /* hMonitor */,
			_In_ HDC /* hMonitorDC */,
			_In_ LPRECT pMonitorRectangle,
			_In_ LPARAM dwData
		);


	//
	// Member data
	//

	private:
		CSceneRenderer * m_ppSceneRenderers[ TEST_API_COUNT ];

		CImmersiveWindowWrapper * m_pCoreWindowWrapper;

		CSystemWindowWrapper * m_pSystemWindowWrapper;


		// DirectComposition members and state
		CDCompIsolation m_DCompIsolation;

		HANDLE m_hCompositionSurface;

#if defined(USE_ADVANCED_DCOMPISOLATION_MODEL)
		CDCompDeviceWrapper * m_pDCompDeviceWrapper;
		CDCompTargetWrapper * m_pDCompTargetWrapper;
#endif

		// SysInfo object
		CSysInfo m_SysInfo;

#if USE_DWMSLICER
		// DWMSlicer-related members
		ImagePipeline::DwmSlicer * m_pDWMSlicer;
		UINT m_cCapturedFrames;
		SmartPtr< ImagePipeline::Texture > m_CapturedFrameAtlas;
#endif

#if USE_SCREENCAPTURE
		CScreenCapture * m_pScreenCapture;
#endif

		// Reference data
		typedef std::vector< ExpectedTexelColor > t_ExpectedColorVector;
		typedef std::map< SCENE_TYPE, t_ExpectedColorVector > t_SceneTypeToExpectedColorVectorMap;
		typedef std::map< SWAPCHAIN_TYPE, t_SceneTypeToExpectedColorVectorMap > t_SwapChainTypeToSceneTypeMap;
		typedef std::map< TEST_API, t_SwapChainTypeToSceneTypeMap > t_RenderingAPIToSwapChainTypeMap;

		t_RenderingAPIToSwapChainTypeMap m_ExpectedResults;
};
