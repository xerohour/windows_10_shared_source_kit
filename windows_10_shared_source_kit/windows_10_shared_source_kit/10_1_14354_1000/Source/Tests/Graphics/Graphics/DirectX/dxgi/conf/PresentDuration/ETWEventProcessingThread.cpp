//////////////////////////////////////////////////////////////////////
// File:  ETWEventProcessingThread.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of an ETW event processing thread.
//
// History:
// 09 Apr 2013  SEdmison  Created.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include "ETWEventProcessingThread.hpp"


/////////////////////////////////////////////////////////////////
//
// Implementation
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// CETWEventProcessingThread implementation
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CETWEventProcessingThread
//
// Purpose:
// Constructor.
//
// Runs on:
// Original thread.
///////////////////////////////////////////////////////
CETWEventProcessingThread::CETWEventProcessingThread
(
	__in PCWSTR SessionName,
	__in PCWSTR LogFileName,
	__in ISimpleLog * pLogger
)
:
	m_pLogger( nullptr ),
	m_hThread( nullptr ),
	m_ThreadID( 0 ),
	m_cEventsProcessed( 0 )
{
	if ( pLogger != nullptr )
	{
		m_pLogger = pLogger;
		m_pLogger->AddRef();
	};


	StringCchCopyW
	(
		m_SessionName,
		ARRAYSIZE( m_SessionName ),
		SessionName
	);

	StringCchCopyW
	(
		m_LogFileName,
		ARRAYSIZE( m_LogFileName ),
		LogFileName
	);
};


///////////////////////////////////////////////////////
// Function:  ~CETWEventProcessingThread
//
// Purpose:
// Destructor.
//
// Runs on:
// Original thread.
///////////////////////////////////////////////////////
CETWEventProcessingThread::~CETWEventProcessingThread()
{
	assert( m_hThread == nullptr );
	assert( m_ThreadID == 0 );

	assert( m_pLogger == nullptr );
	if ( m_pLogger != nullptr )
	{
		m_pLogger->Release();
		m_pLogger = nullptr;
	};
};


///////////////////////////////////////////////////////
// Function:  SetLogger
//
// Purpose:
// Attaches a logger to this object.
///////////////////////////////////////////////////////
void CETWEventProcessingThread::SetLogger
(
	__in ISimpleLog * pLogger
)
{
	if ( m_pLogger != nullptr )
	{
		m_pLogger->Release();
		m_pLogger = nullptr;
	};

	if ( pLogger != nullptr )
	{
		m_pLogger = pLogger;
		m_pLogger->AddRef();
	};
};


///////////////////////////////////////////////////////
// Function:  Cleanup
//
// Purpose:
// Destroys any resources created by Initialize and
// gets the object ready to be re-initialized or
// destroyed.
//
// Runs on:
// Original thread.
///////////////////////////////////////////////////////
HRESULT CETWEventProcessingThread::Cleanup()
{
	//
	// Wait for the thread to exit, and then close the handle.
	//

	if ( m_hThread != nullptr )
	{
		WaitForSingleObject
		(
			m_hThread,
			10 * 1000
		);

		CloseHandle( m_hThread );
		m_hThread = nullptr;
	};

	m_ThreadID = 0;
	m_cEventsProcessed = 0;


	//
	// Release the logger.
	//

	if ( m_pLogger != nullptr )
	{
		m_pLogger->Release();
		m_pLogger = nullptr;
	};


	return S_OK;
};


///////////////////////////////////////////////////////
// Function:  RunOnOriginalThread
//
// Purpose:
// Runs on the parent thread to create a child thread
// that will process ETW event records.
//
// Runs on:
// Original thread.
///////////////////////////////////////////////////////
DWORD CETWEventProcessingThread::RunOnOriginalThread
(
	__in LPTHREAD_START_ROUTINE pThreadProc
)
{
	//
	// Initialize locals.
	//

	DWORD OverallResult = 1;


	//
	// Validate parameters.
	//

	if ( pThreadProc == nullptr )
	{
		OverallResult = 2;
		goto Cleanup;
	};


	//
	// Create a child thread.
	//

	m_hThread = CreateThread
	(
		nullptr,
		0,
		pThreadProc,
		this,
		CREATE_SUSPENDED,
		&m_ThreadID
	);

	if ( m_hThread == nullptr )
	{
		DWORD CreateThreadError = GetLastError();

		LOG_ERROR
		(
			ERROR_TYPE_WIN32,
			CreateThreadError,
			L"CreateThread failed."
		);

		OverallResult = 3;
		goto Cleanup;
	};


	//
	// Resume the thread.
	//

	DWORD ResumeThreadResult = ResumeThread( m_hThread );

	if ( ResumeThreadResult != 1 )
	{
		DWORD ResumeThreadError = GetLastError();

		LOG_ERROR
		(
			ERROR_TYPE_WIN32,
			ResumeThreadError,
			L"ResumeThread failed."
		);

		OverallResult = 4;
		goto Cleanup;
	};


	OverallResult = 0;


Cleanup:
	return OverallResult;
};


///////////////////////////////////////////////////////
// Function:  RunOnNewThread
//
// Purpose:
// Opens an ETW event record tracing session and
// processes incoming ETW events with the callback
// EventRecordCallback until all events have been
// processed or the callback returns FALSE.
//
// Runs on:
// ETW event processing thread.
///////////////////////////////////////////////////////
DWORD CETWEventProcessingThread::RunOnNewThread()
{
	//
	// Initialize locals.
	//

	DWORD OverallResult = 1;


	//
	// Initialize properties for the tracing session to be processed.
	//

	EVENT_TRACE_LOGFILEW EventTraceLogFile;
	ZeroMemory( &EventTraceLogFile, sizeof( EventTraceLogFile ) );

	EventTraceLogFile.LogFileName = nullptr;
	EventTraceLogFile.LoggerName = const_cast< PWSTR >( m_SessionName );
	EventTraceLogFile.ProcessTraceMode = PROCESS_TRACE_MODE_EVENT_RECORD | PROCESS_TRACE_MODE_REAL_TIME;
	EventTraceLogFile.EventRecordCallback = EventRecordCallback;
	EventTraceLogFile.Context = this;


	//
	// Open the tracing session.
	//

	TRACEHANDLE hEventTracingSession = OpenTraceW
	(
		&EventTraceLogFile
	);

	if ( hEventTracingSession == INVALID_PROCESSTRACE_HANDLE )
	{
		DWORD OpenTraceError = GetLastError();

		/*
		LOG_ERROR
		(
			ERROR_TYPE_WIN32,
			OpenTraceError,
			L"OpenTraceW failed."
		);
		*/

		OverallResult = 2;
		goto Cleanup;
	};


	//
	// Process events until the session is closed.
	//

	TRACEHANDLE EventTracingSessionsToProcess[ 1 ] = { hEventTracingSession };
	ULONG ProcessTraceResult = ERROR_WMI_INSTANCE_NOT_FOUND;
	DWORD StartTickCount = GetTickCount();
	for
	(
		DWORD CurrentTickCount = StartTickCount;
		CurrentTickCount <= StartTickCount + 10 * 1000;
		CurrentTickCount = GetTickCount()
	)
	{
		ProcessTraceResult = ProcessTrace
		(
			EventTracingSessionsToProcess,
			ARRAYSIZE( EventTracingSessionsToProcess ),
			nullptr,
			nullptr
		);

		if ( ProcessTraceResult != ERROR_WMI_INSTANCE_NOT_FOUND )
		{
			break;
		};

		Sleep( 1 * 1000 );
	};

	if ( ProcessTraceResult != ERROR_SUCCESS )
	{
		/*
		LOG_ERROR
		(
			ERROR_TYPE_WIN32,
			ProcessTraceResult,
			L"ProcessTrace failed."
		);
		*/

		OverallResult = 3;
		goto Cleanup;
	};


	OverallResult = 0;


Cleanup:
	//
	// Close the tracing session.
	//

	if ( hEventTracingSession != INVALID_PROCESSTRACE_HANDLE )
	{
		CloseTrace( hEventTracingSession );
		hEventTracingSession = 0;
	};


	return OverallResult;
};


///////////////////////////////////////////////////////
// Function:  ThreadProc
//
// Purpose:
// Thread procedure that calls RunOnNewThread.
//
// Runs on:
// ETW event processing thread.
///////////////////////////////////////////////////////
DWORD WINAPI CETWEventProcessingThread::ThreadProc
(
	void * pContext
)
{
	//
	// Initialize locals.
	//

	DWORD OverallResult = 1;


	//
	// Validate parameters.
	//

	if ( pContext == nullptr )
	{
		OverallResult = 2;
		goto Cleanup;
	};


	//
	// Cast the context pointer to a thread pointer.
	//

	CETWEventProcessingThread * pThread = reinterpret_cast< CETWEventProcessingThread * >( pContext );
	if ( pThread == nullptr )
	{
		OverallResult = 3;
		goto Cleanup;
	};


	//
	// Call the thread object's RunOnNewThread method.
	//

	OverallResult = pThread->RunOnNewThread();


Cleanup:
	return OverallResult;
};


///////////////////////////////////////////////////////
// Function:  EventRecordCallback
//
// Purpose:
// Callback that calls ProcessEventRecord.
//
// Runs on:
// ETW event processing thread.
///////////////////////////////////////////////////////
void WINAPI CETWEventProcessingThread::EventRecordCallback
(
	__in EVENT_RECORD * pEventRecord
)
{
	//
	// Validate parameters.
	//

	assert( pEventRecord != nullptr );
	if ( pEventRecord == nullptr )
	{
		return;
	};


	//
	// Cast the context pointer to a thread pointer.
	//

	CETWEventProcessingThread * pThread = reinterpret_cast< CETWEventProcessingThread * >( pEventRecord->UserContext );
	assert( pThread != nullptr );
	if ( pThread == nullptr )
	{
		return;
	};


	//
	// Have the thread process the event record.
	//

	pThread->ProcessEventRecord( pEventRecord );
};
