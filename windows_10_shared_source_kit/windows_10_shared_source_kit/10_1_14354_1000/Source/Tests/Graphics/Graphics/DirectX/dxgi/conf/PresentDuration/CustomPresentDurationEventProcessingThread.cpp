//////////////////////////////////////////////////////////////////////
// File:  CustomPresentDurationEventProcessingThread.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a thread to process ETW events pertaining to
// custom presentation durations.
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

#include "CustomPresentDurationEventProcessingThread.hpp"


/////////////////////////////////////////////////////////////////
//
// Implementation
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// CCustomPresentDurationEventProcessingThread implementation
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CCustomPresentDurationEventProcessingThread
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CCustomPresentDurationEventProcessingThread::CCustomPresentDurationEventProcessingThread
(
	__in ISimpleLog * pLogger
)
:
	CETWEventProcessingThread
	(
		L"CustomPresentDurationSession",
		L"Microsoft.DXGI.PresentDuration.Tests.etw",
		pLogger
	),
	m_bIndependentFlipWasAchieved( false )
{
};


///////////////////////////////////////////////////////
// Function:  ~CCustomPresentDurationEventProcessingThread
//
// Purpose:
// Destructor.
///////////////////////////////////////////////////////
CCustomPresentDurationEventProcessingThread::~CCustomPresentDurationEventProcessingThread()
{
};


///////////////////////////////////////////////////////
// Function:  Initialize
//
// Purpose:
// Gets the object ready for use.
///////////////////////////////////////////////////////
HRESULT CCustomPresentDurationEventProcessingThread::Initialize()
{
	return S_OK;
};


///////////////////////////////////////////////////////
// Function:  Cleanup
//
// Purpose:
// Frees resources allocated in Initialize.
///////////////////////////////////////////////////////
HRESULT CCustomPresentDurationEventProcessingThread::Cleanup()
{
	//
	// Have the base class clean up.
	//

	return __super::Cleanup();
};


///////////////////////////////////////////////////////
// Function:  ProcessEventRecord
//
// Purpose:
// Processes the specified ETW event record.  Currently
// just keeps track of whether we've seen independent
// flip events from DWM.
//
// To do:
// Crack open the payload for DXGI Present events and
// vsync events to analyze the timing to make sure that
// both types of events are happening at a steady and
// precise cadence consistent with our specified custom
// presentation duration.
///////////////////////////////////////////////////////
void CCustomPresentDurationEventProcessingThread::ProcessEventRecord
(
	__in EVENT_RECORD * pEventRecord
)
{
	//
	// Initialize locals.
	//

	BYTE * pTraceEventBufferAllocation = nullptr;
	DWORD nTraceEventBufferAllocation = 0;

	TRACE_EVENT_INFO * pTraceEventBuffer = nullptr;


	//
	// Skip events other than the specific ones we care about.
	//

	if
	(
		!(
			pEventRecord->EventHeader.ProviderId == DXGIEtwProviderGuid &&
			pEventRecord->EventHeader.EventDescriptor.Id != IDXGISwapChain_Present_Start_value &&
			pEventRecord->EventHeader.ProcessId == GetCurrentProcessId()
		) &&
		!(
			pEventRecord->EventHeader.ProviderId == DxgkControlGuid &&
			pEventRecord->EventHeader.EventDescriptor.Id == EventIndependentFlip_value
		)
	)
	{
		return;
	};


	//
	// Keep a running tally of events processed.
	//

	++m_cEventsProcessed;


	//
	// Look for a DXGKrnl event indicating that DWM is doing
	// an independent flip.
	//

	if
	(
		pEventRecord->EventHeader.ProviderId == DxgkControlGuid &&
		pEventRecord->EventHeader.EventDescriptor.Id == EventIndependentFlip_value
	)
	{
		m_bIndependentFlipWasAchieved = true;
	};


#if 0
	//
	// Get information about the event.
	//

	DWORD GetTraceEventInformationResult = TdhGetEventInformation
	(
		pEventRecord,
		0,
		nullptr,
		nullptr,
		&nTraceEventBufferAllocation
	);

	if ( GetTraceEventInformationResult == ERROR_INSUFFICIENT_BUFFER )
	{
		assert( nTraceEventBufferAllocation > 0 );
		pTraceEventBufferAllocation = new( std::nothrow ) BYTE[ nTraceEventBufferAllocation ];

		if ( pTraceEventBufferAllocation == nullptr )
		{
			// @REVIEW:  Trigger an error here?
			goto Cleanup;
		};

		pTraceEventBuffer = (TRACE_EVENT_INFO *) pTraceEventBufferAllocation;

		GetTraceEventInformationResult = TdhGetEventInformation
		(
			pEventRecord,
			0,
			nullptr,
			pTraceEventBuffer,
			&nTraceEventBufferAllocation
		);
	};

	if ( GetTraceEventInformationResult != ERROR_SUCCESS )
	{
		// @REVIEW:  Trigger an error here?
		goto Cleanup;
	};


	//
	// Analyze the event information.
	//
	// @TODO


Cleanup:
#endif
	pTraceEventBuffer = nullptr;

#if 0
	if ( pTraceEventBufferAllocation != nullptr )
	{
		delete [] pTraceEventBufferAllocation;
		pTraceEventBufferAllocation = nullptr;
	};
#endif
};
