//////////////////////////////////////////////////////////////////////
// File:  ETWEventProcessingThread.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for an ETW event processing thread.
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
// Win32 public headers
//

#include <Windows.h>
#include <D3D11.h>
#include <DXGI.h>
#include <EvntCons.h>
#include <EvnTrace.h>
#include <TDH.h>
#include <strsafe.h>


//
// C runtime headers
//

#include <assert.h>


//
// STL headers
//

#include <algorithm>
#include <string>
#include <vector>


//
// Test infrastructure headers
//

#include <ISimpleLog.h>


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


class CCustomPresentDurationEventProcessingThread;


/////////////////////////////////////////////////////////////////
//
// Interfaces
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Class:  CETWEventProcessingThread
//
// Purpose:
// Object that encapsulates a worker thread that opens
// an ETW session, processes the trace, and processes
// the events of interest.
///////////////////////////////////////////////////////
class CETWEventProcessingThread
{
	//
	// Creation/destruction
	//

	public:
		// Constructor.
		CETWEventProcessingThread
		(
			__in PCWSTR SessionName,
			__in PCWSTR LogFileName,
			__in ISimpleLog * pLogger
		);

		// Destructor.
		virtual ~CETWEventProcessingThread();


	//
	// Public methods
	//

	public:
		virtual HRESULT Cleanup();


		DWORD RunOnOriginalThread
		(
			__in LPTHREAD_START_ROUTINE pThreadProc
		);

		DWORD RunOnNewThread();


		void SetLogger
		(
			__in ISimpleLog * pLogger
		);


	//
	// Static public methods
	//

	public:
		static DWORD WINAPI ThreadProc
		(
			__in void * pContext
		);


		static void WINAPI EventRecordCallback
		(
			__in EVENT_RECORD * pEventRecord
		);


	//
	// Helper methods
	//

	protected:
		virtual void ProcessEventRecord
		(
			__in EVENT_RECORD * pEventRecord
		) PURE;


	//
	// Member data
	//

	protected:
		ISimpleLog * m_pLogger;

		HANDLE m_hThread;
		DWORD m_ThreadID;

	public:
		WCHAR m_SessionName[ 32 ];
		WCHAR m_LogFileName[ MAX_PATH ];

		UINT m_cEventsProcessed;
};
