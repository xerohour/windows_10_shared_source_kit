//////////////////////////////////////////////////////////////////////
// File:  GraphicsTAEFTest.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for a base class for TAEF-based graphics tests.
//
// History:
// 23 Mar 2013  SEdmison  Created, based on DImageTestBase.hpp.
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
#include <InitGUID.h>
#include <DXGIDebug.h>
#include <ShellScalingAPI.h>


//
// Test infrastructure headers
//

#include <DXGLogger.h>
#include <ISimpleLogController.h>
#include <StringConverters.hpp>


//
// TAEF headers
//

#include <WEXTestClass.h>


//
// Project headers
//

#include "HelperMacros.h"
#include "RenderingTypes.hpp"


/////////////////////////////////////////////////////////////////
//
// Macros
//
/////////////////////////////////////////////////////////////////

#if !defined(__FILEW__)
#define __FILEW__ UNICODIFY(__FILE__)
#endif


#undef LOG_ERROR
#define LOG_ERROR( ErrorType, ErrorCode, ErrorDescription ) \
	if ( m_pLogger != nullptr ) \
	{ \
		m_pLogger->LogError \
		( \
			ErrorType, \
			static_cast< UINT >( ErrorCode ), \
			ErrorDescription, \
			__FILEW__, \
			__LINE__, \
			__FUNCTION__ \
		); \
	}


#undef LOG_MESSAGE
#define LOG_MESSAGE( MessageType, MessageText ) \
	if ( m_pLogger != nullptr ) \
	{ \
		m_pLogger->LogMessage \
		( \
			(MessageType), \
			(MessageText) \
		); \
	}


/////////////////////////////////////////////////////////////////
//
// Type definitions
//
/////////////////////////////////////////////////////////////////

// DXGI debug function prototypes
typedef HRESULT (WINAPI * PFNDXGIGetDebugInterface)
(
	_In_ REFIID InterfaceID,
	_Out_ void ** ppObject_Interface
);

// Direct3D 11 function prototypes
typedef BOOL (WINAPI * PFNEnableFeatureLevelUpgrade)();


/////////////////////////////////////////////////////////////////
//
// Forward declarations
//
/////////////////////////////////////////////////////////////////

bool __stdcall OpenLog();
bool __stdcall CloseLog();


/////////////////////////////////////////////////////////////////
//
// Interfaces
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Helper functions
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  FromString( D3D_FEATURE_LEVEL )
//
// Purpose:
// Converts a string to a Direct3D feature level.
///////////////////////////////////////////////////////
bool __stdcall FromString
(
	_Out_ D3D_FEATURE_LEVEL * pFeatureLevel,
	const tstring & FeatureLevelString
);


///////////////////////////////////////////////////////
// Function:  ToString( D3D_DRIVER_TYPE )
//
// Purpose:
// Provides a string representation for a Direct3D
// driver type.
///////////////////////////////////////////////////////
tstring ToString( const D3D_DRIVER_TYPE & DriverType );


///////////////////////////////////////////////////////
// Function:  ToString( D3D_FEATURE_LEVEL )
//
// Purpose:
// Provides a string representation for a Direct3D
// feature level.
///////////////////////////////////////////////////////
tstring ToString( const D3D_FEATURE_LEVEL & FeatureLevel );


///////////////////////////////////////////////////////
// Function:  SetThreadName
//
// Purpose:
// Raises an exception to the debugger with a name to
// be associated with the current thread, making it
// easier to identify threads under the debugger.
///////////////////////////////////////////////////////
void SetThreadName
(
	DWORD ThreadID,
	_In_ PCSTR ThreadName
);


////////////////////////////////////////////////////////////
//
// Base class interfaces
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Class:  CGraphicsTAEFTest
//
// Purpose:
// Base class for graphics TAEF tests.  Handles mundane
// test tasks like parsing command-line arguments,
// selecting a logger, and setting environment
// variables.
///////////////////////////////////////////////////////
class CGraphicsTAEFTest
{
	//
	// Construction/destruction
	//

	public:
		CGraphicsTAEFTest();

		virtual ~CGraphicsTAEFTest();


	//
	// Public methods
	//

	public:
		HRESULT ComputeVariations( size_t cVariations );


		virtual bool SetupTestGroup();

		virtual bool CleanupTestGroup();


		virtual bool SetupTestCase();

		virtual bool CleanupTestCase();


	//
	// Helper methods
	//

	private:
		HRESULT InstallFeatureLevelPromotionDetour
		(
			TEST_API Direct3DVersion
		);

		HRESULT UninstallFeatureLevelPromotionDetour
		(
			TEST_API Direct3DVersion
		);


		void CreateDXGIDebugInterface();

		void ReportLiveDXGIObjects();


		HRESULT SetSymbolPath();


		void GetVariations();


	protected:
		bool IsOperatingSystemCheckedBuild() const;

		bool IsOperatingSystemCoverageBuild() const;


		void LogTestGroupOverallResult
		(
			TESTCASE_RESULT TestGroupOverallResult
		) const;



	//
	// Member data
	//

	protected:
		// Log wrapper object
		ISimpleLogController * m_pLogger;

		// Execution behavior
		bool m_bHaltOnFailure;

		// Variation parameters
		size_t m_VariationStartParam;
		size_t m_VariationEndParam;
		size_t m_VariationStartNumber;
		size_t m_VariationEndNumber;
		UINT32 m_VariationCount;

		// DXGI state
		HMODULE m_hDXGIDebugDLL;
		IDXGIDebug1 * m_pDXGIDebug;
		IDXGIInfoQueue * m_pDXGIInfoQueue;

		// Direct3D state
		bool m_bAllowFeatureLevelPromotion;
		PFNEnableFeatureLevelUpgrade m_pD3D11_EnableFeatureLevelUpgrade;
		PFNEnableFeatureLevelUpgrade m_pD3D12_EnableFeatureLevelUpgrade;
	};
