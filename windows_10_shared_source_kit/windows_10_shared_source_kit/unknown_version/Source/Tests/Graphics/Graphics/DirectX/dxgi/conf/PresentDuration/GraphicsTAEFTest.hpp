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

#include <windows.h>
#include <DXGI1_3.h>
#include <initguid.h>
#include <DXGIDebug.h>


//
// Test infrastructure headers
//

#include <WEXTestClass.h>
#include <DXGLogger.h>
#include <ISimpleLogController.h>
#include <StringConverters.hpp>


//
// Project headers
//

#include "HelperMacros.h"


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
	__in REFIID InterfaceID,
	__out void ** ppObject_Interface
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
		bool InstallFeatureLevelPromotionDetour();

		bool UninstallFeatureLevelPromotionDetour();


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

		// Direct3D device creation flags
		bool m_bAllowFeatureLevelPromotion;
		PFNEnableFeatureLevelUpgrade m_pEnableFeatureLevelUpgrade;
	};
