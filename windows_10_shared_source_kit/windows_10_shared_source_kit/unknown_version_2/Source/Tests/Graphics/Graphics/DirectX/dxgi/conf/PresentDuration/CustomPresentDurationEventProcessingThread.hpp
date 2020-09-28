//////////////////////////////////////////////////////////////////////
// File:  CustomPresentDurationEventProcessingThread.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for a thread to process ETW events pertaining to custom
// presentation durations.
//
// History:
// 09 Apr 2013  SEdmison  Created.
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
// Project headers
//

#include "ETWEventProcessingThread.hpp"


//
// Win32 internal headers
//

#include <DCompEvents.h>
#include <DXGIEvents.h>

// Windows Blue Bugs bug 215134:  DXGIEvents.h and LDDMCoreEventDefs.h can't be #include'd from the same compiland
// We can't #include LDDMCoreEventDefs.h, so for now just
// declare/define what we need.
EXTERN_C __declspec(selectany) const GUID DxgkControlGuid = {0x802ec45a, 0x1e99, 0x4b83, {0x99, 0x20, 0x87, 0xc9, 0x82, 0x77, 0xba, 0x9d}};
#define EventIndependentFlipTransition_value 0x10b
#define EventIndependentFlip_value 0x10a


/////////////////////////////////////////////////////////////////
//
// Interfaces
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Class:  CCustomPresentDurationEventProcessingThread
//
// Purpose:
// Object that processes ETW events pertaining to
// custom presentation durations for DXGI swap chains.
///////////////////////////////////////////////////////
class CCustomPresentDurationEventProcessingThread
:
	public CETWEventProcessingThread
{
	//
	// Creation/destruction
	//

	public:
		// Constructor.
		CCustomPresentDurationEventProcessingThread
		(
			__in ISimpleLog * pLogger
		);

		// Destructor.
		virtual ~CCustomPresentDurationEventProcessingThread();


	//
	// Public methods
	//

	public:
		HRESULT Initialize();

		virtual HRESULT Cleanup();


	//
	// Helper methods
	//

	protected:
		virtual void ProcessEventRecord
		(
			__in EVENT_RECORD * pEventRecord
		);


	//
	// Member data
	//

	public:
		bool m_bIndependentFlipWasAchieved;
};
