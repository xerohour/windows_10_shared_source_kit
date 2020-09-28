//////////////////////////////////////////////////////////////////////
// File:  InternalPointerAlias.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of Direct3D vertex buffer and index buffer tests.
//
// Additional information:
// This test triggers mode changes from a separate process while the
// main process is rendering a scene. The goal is to trigger the mode
// change at an inconvenient time, to verify that the runtime and
// driver handle it properly.  Each test case varies the POOL and
// USAGEs for all vertex buffers and index buffers created.  It also
// runs through each test case 5 times, varying when the mode change
// is triggered.
//
// History:
// ?? ??? ????  Darius Rad  Created.
// 25 Sep 2003  OlanH       Added comments added.  Re-worked to use
//                          framework's default 3D device.
// 28 Oct 2005  SEdmison    Cleaned up.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Build flags
//
/////////////////////////////////////////////////////////////////

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif


/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

#include "CVertexIndexBufferTest.h"
#include "cbuffers.h"
#include <d3dx8.h>
#include <windows.h>
#include <mmsystem.h>
#include <strsafe.h>


/////////////////////////////////////////////////////////////////
//
// Macros
//
/////////////////////////////////////////////////////////////////

//
// INTERNALPOINTERALIAS_TIMEOUT contains the maximum number of milliseconds to 
// sit in our render loop, waiting for a mode change to cause a DEVICELOST condition.
//
// Since we trigger 5 mode changes per VB/IB variation, and the mode changing thread
// steps up in 10ms increments, then the timeout must be at least 50ms.   To be safe,
// we should choose something much larger than that.   5 seconds seems reasonable. :-)
//

#define INTERNALPOINTERALIAS_TIMEOUT 5000  


/////////////////////////////////////////////////////////////////
//
// Globals and statics
//
/////////////////////////////////////////////////////////////////

// Test group object.
#ifndef FAST_TEST //Slow and not overly useful for drivers
InternalPointerAlias tstInternalPointerAlias;
#endif


const DWORD InternalPointerAlias::Usages[] =
{
	0,
	USAGE_WRITEONLY,
	//USAGE_POINTS,
	//USAGE_RTPATCHES,
	//USAGE_NPATCHES,
	USAGE_DYNAMIC,
	//USAGE_POINTS | USAGE_RTPATCHES | USAGE_NPATCHES,
	USAGE_WRITEONLY | USAGE_DYNAMIC,

	USAGE_SOFTWAREPROCESSING,
	USAGE_SOFTWAREPROCESSING | USAGE_WRITEONLY,
	//USAGE_SOFTWAREPROCESSING | USAGE_POINTS,
	//USAGE_SOFTWAREPROCESSING | USAGE_RTPATCHES,
	//USAGE_SOFTWAREPROCESSING | USAGE_NPATCHES,
	USAGE_SOFTWAREPROCESSING | USAGE_DYNAMIC,
	//USAGE_SOFTWAREPROCESSING | USAGE_POINTS | USAGE_RTPATCHES | USAGE_NPATCHES,
	USAGE_SOFTWAREPROCESSING | USAGE_WRITEONLY | USAGE_DYNAMIC,
};


const DWORD InternalPointerAlias::Pools[] =
{
	POOL_DEFAULT,
	POOL_MANAGED,
	POOL_SYSTEMMEM,
};


const DWORD InternalPointerAlias::LockFlags[] =
{
	0,
	LOCK_DISCARD,
	LOCK_NOSYSLOCK,
	LOCK_DISCARD | LOCK_NOSYSLOCK,
};


TCHAR InternalPointerAlias::s_ModeThreadError[ MAX_PATH ] = TEXT( "" );


static InternalPointerAliasTestCase s_InternalPointerAliasTestCases
[
	COUNTOF( InternalPointerAlias::Usages ) *
	COUNTOF( InternalPointerAlias::Pools ) *
	COUNTOF( InternalPointerAlias::LockFlags )
];


DEVMODE InternalPointerAlias::s_OriginalMode;
DEVMODE InternalPointerAlias::s_AlternateMode;


TCHAR InternalPointerAlias::s_DeviceName[ 32 ] = TEXT( "" );


/////////////////////////////////////////////////////////////////
//
// Implementation
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Construction / destruction
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  InternalPointerAlias
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
InternalPointerAlias::InternalPointerAlias()
:
	m_dwBehaviorOverride( 0 ),
	m_bSWVP( FALSE )
{
	m_szTestName = TEXT("VBIB Internal Pointer Alias");
	m_szCommandKey = TEXT("InternalPointerAlias");

	SetTestType( TESTTYPE_CONF );

	m_Options.D3DOptions.bZBuffer = false;
	m_Options.D3DOptions.bCachedBackBuffer	= false;
	m_Options.D3DOptions.bReference = false;
	m_Options.D3DOptions.fMinDXVersion = 8.0f;
	m_Options.D3DOptions.dwDevTypes = DEVICETYPE_HAL;

	m_Options.TestOptions.bCompareSrcRef = false;

	if (KeySet("WHQL"))
		m_Options.D3DOptions.dwIncVersions = DXVERSION_8_OR_81;
};


///////////////////////////////////////////////////////
// Function:  ~InternalPointerAlias
//
// Purpose:
// Destructor.
///////////////////////////////////////////////////////
InternalPointerAlias::~InternalPointerAlias()
{
};


///////////////////////////////////////////////////////
// Function:  TestInitialize
//
// Purpose:
// Test group initialization.
///////////////////////////////////////////////////////
UINT InternalPointerAlias::TestInitialize()
{
	//
	// Record the requested vertex processing type.
	//

	m_dwBehaviorOverride = m_pSrcDevice->GetBehaviors();
	if ( m_dwBehaviorOverride & DEVICECREATE_MIXED )
	{
		m_bSWVP = m_pSrcDevice->GetSoftwareVertexProcessing();
	};


	//
	// Count the number of valid scenarios, and build our
	// scenario data list.
	//

	UINT cTestCases = 0;
	for ( UINT iCurrentUsage = 0; iCurrentUsage < COUNTOF( Usages ); ++iCurrentUsage )
	{
		for ( UINT iCurrentPool = 0; iCurrentPool < COUNTOF( Pools ); ++iCurrentPool )
		{
			for ( UINT iCurrentLockFlag = 0; iCurrentLockFlag < COUNTOF( LockFlags ); ++iCurrentLockFlag )
			{
				// Skip invalid combinations of parameters:
				if
				(
					// Dynamic buffers can't be in POOL_MANAGED, and
					( ( Usages[ iCurrentUsage ] & USAGE_DYNAMIC ) && ( Pools[ iCurrentPool] == POOL_MANAGED ) ) ||

					// LOCK_DISCARD and LOCK_NOOVERWRITE can only be used on dynamic buffers.
					( !( Usages[ iCurrentUsage ] & USAGE_DYNAMIC ) && ( LockFlags[ iCurrentLockFlag ] & LOCK_DISCARD ) ) ||
					( !( Usages[ iCurrentUsage ] & USAGE_DYNAMIC ) && ( LockFlags[ iCurrentLockFlag ] & LOCK_NOOVERWRITE ) ) ||

					// Software usage is not valid on hardware device
					( ( Usages[ iCurrentUsage ] & USAGE_SOFTWAREPROCESSING ) && ( m_dwBehaviorOverride & DEVICECREATE_HWVP ) ) ||

					// Software usage is required on mixed device with SW vertex processing
					// when pool is not sysmem.
					(
						!( Usages[ iCurrentUsage ] & USAGE_SOFTWAREPROCESSING ) &&
						( m_dwBehaviorOverride & DEVICECREATE_MIXED ) &&
						m_bSWVP &&
						( Pools[ iCurrentPool ] != POOL_SYSTEMMEM )
					)
				)
				{
					continue;
				};

				ASSERT( cTestCases < COUNTOF( s_InternalPointerAliasTestCases ) );

				s_InternalPointerAliasTestCases[ cTestCases ].nTest = cTestCases;
				s_InternalPointerAliasTestCases[ cTestCases ].Usage = Usages[ iCurrentUsage ];
				s_InternalPointerAliasTestCases[ cTestCases ].Pool = Pools[ iCurrentPool ];
				s_InternalPointerAliasTestCases[ cTestCases ].LockFlag = LockFlags[ iCurrentLockFlag ];

				++cTestCases;
			};
		};
	};


	//
	// Set the test range.
	//

	SetTestRange
	(
		1,
		cTestCases
	);


	//
	// Initialize our shape.
	//

	m_shape.NewShape( CS_SPHERE, 50 );


	//
	// @REVIEW [SEdmison]:
	// Now that we've specified device type HAL in the D3DOptions,
	// the framework should only give us a HAL device.  Do we really
	// need this additional verification?
	//

	DEVICE_CREATION_PARAMETERS dcp;
	m_pSrcDevice->GetCreationParameters(&dcp);


	//
	// Unfortunately, this test app only runs properly on real hardware.
	// It won't run on REF or other pluggable software rasterizers that
	// don't have any real concept of a lost device condition.
	//

	if (dcp.DeviceType != D3DDEVTYPE_HAL)
	{
		WriteToLog("*** WARNING:  This test does not run against software devices. ***");
		return D3DTESTINIT_SKIPALL;
	};


	return D3DTESTINIT_RUN;
};


///////////////////////////////////////////////////////
// Function:  StaticThreadProc
//
// Purpose:
// Thread procedure.
///////////////////////////////////////////////////////
DWORD WINAPI InternalPointerAlias::StaticThreadProc
(
	VOID * pContext
)
{
	if ( pContext == NULL )
	{
		return 1;
	};


	InternalPointerAlias * pThis = (InternalPointerAlias *) pContext;

	if ( pThis == NULL )
	{
		return 2;
	};

	return pThis->ThreadProc();
};


///////////////////////////////////////////////////////
// Function:  ThreadProc
//
// Purpose:
// Triggers a mode change by launching a child instance
// of our test app.  By doing this from a separate
// thread, it attempts to trigger a mode change in the
// middle of a rendering operation.
///////////////////////////////////////////////////////
DWORD InternalPointerAlias::ThreadProc()
{
	DWORD ThreadReturnCode = 0;


	DWORD SleepDuration = 0;
	while ( 1 )
	{
		// Signal the main thread to continue.
		SetEvent( m_phEvents[ 2 ] );

		// Wait until event 0 or 1 is triggered.  
		// Event 0 signals the thread to terminate.
		// Event 1 signals the thread to trigger a mode change.
		//OutputDebugString( TEXT( "ModeThread:  Waiting for events 0 or 1.\n" ) );
		DWORD dwObj = WaitForMultipleObjects
		(
			2,
			m_phEvents,
			FALSE,
			INFINITE
		);

		// If event 0 was triggered, then it's time to shutdown.
		if ( dwObj == WAIT_OBJECT_0 )
		{
			//OutputDebugString( TEXT( "ModeThread:  Event 0 signalled.  Time to go.\n" ) );
			break;
		};

		//OutputDebugString( TEXT( "ModeThread:  Event 1 signalled.  Time to do a mode change.\n" ) );

		Sleep( SleepDuration );
		SleepDuration += 10;

		// Pause the main thread in case it gets ahead of us.
		ResetEvent( m_phEvents[ 2 ] );

		// if the main thread is currently calling Reset, skip a mode change and wait again
		if ( TryEnterCriticalSection( &m_ResetLock ) )
		{
			//
			// Trigger a mode change, then switch back to the previous mode.
			// We're just doing this to put the device in a lost state.
			//

			bool bLoseDeviceResult = LoseTheDevice( D3DLOSEDEV_BYOTHERPROCESS );

			LeaveCriticalSection( &m_ResetLock );

			if ( bLoseDeviceResult != true )
			{
				WriteToLog
				(
					_T( "LoseTheDevice failed." )
				);

				goto Cleanup;
			};
		};
	};


	ThreadReturnCode = 0;


Cleanup:
	return ThreadReturnCode;
};


///////////////////////////////////////////////////////
// Function:  ExecuteTest
//
// Purpose:
// Runs a test case.
///////////////////////////////////////////////////////
bool InternalPointerAlias::ExecuteTest
(
	IN UINT nTest
)
{
	//
	// Validate the test case ordinal.
	//

	if ( nTest > COUNTOF( s_InternalPointerAliasTestCases ) )
	{
		WriteToLog
		(
			TEXT( "Invalid test ordinal %u." ),
			nTest
		);

		return false;
	};


	//
	// Build the test case name.
	//

	TCHAR sz[ 256 ] = TEXT( "" );
	StringCchPrintf
	(
		sz,
		256,
		TEXT("Usage = 0x%x, Pool = %d, LockFlags = 0x%x"),
		s_InternalPointerAliasTestCases[ nTest - 1 ].Usage,
		s_InternalPointerAliasTestCases[ nTest - 1 ].Pool,
		s_InternalPointerAliasTestCases[ nTest - 1 ].LockFlag
	);


	//
	// Begin the test case.
	//

	BeginTestCase( sz, nTest );


	//
	// Clear the frame in preparation for rendering.
	//

	ClearFrame();


	//
	// Initialize local variables for use later.
	//

	HRESULT hr = E_FAIL;
	LPVERTEXBUFFER pVB = NULL;
	LPINDEXBUFFER pIB = NULL;
	HANDLE hThread = NULL;

	TCHAR ThisCall[ MAX_PATH ] = TEXT( "" );

	ZeroMemory( m_phEvents, sizeof( m_phEvents ) );


	//
	// Determine whether software vertex processing was specified.
	//

	BOOL fSoftwareUsage = FALSE;
	if ( m_dwBehaviorOverride & DEVICECREATE_MIXED )
	{
		fSoftwareUsage = FALSE;
		if ( FAILED(hr = m_pSrcDevice->SetSoftwareVertexProcessing( m_bSWVP ) ) )
		{
			WriteToLog
			(
				TEXT("SetSoftwareVertexProcessing returned %s (0x%x)."),
				m_pD3D->HResultToString(hr),
				hr
			);

			Abort();

			goto Cleanup;
		};
	};


	//
	// Fill in a context information structure for our helper thread.
	//

	WriteToLog
	(
		_T( "Initializing synchronization objects." )
	);

	InitializeCriticalSection(&m_ResetLock);

	m_phEvents[ 0 ] = CreateEvent( NULL, FALSE, FALSE, NULL );
	if ( m_phEvents[ 0 ] == NULL )
	{
		WriteToLog
		(
			TEXT( "CreateEvent failed." )
		);

		Abort();

		goto Cleanup;
	};

	m_phEvents[ 1 ] = CreateEvent( NULL, FALSE, FALSE, NULL );
	if ( m_phEvents[ 1 ] == NULL )
	{
		WriteToLog
		(
			TEXT( "CreateEvent failed." )
		);

		Abort();

		goto Cleanup;
	};

	m_phEvents[ 2 ] = CreateEvent( NULL, TRUE, TRUE, NULL );
	if ( m_phEvents[ 2 ] == NULL )
	{
		WriteToLog
		(
			TEXT( "CreateEvent failed." )
		);

		Abort();

		goto Cleanup;
	};


	//
	// Launch the mode-switching thread.
	//

	WriteToLog
	(
		_T( "Launching mode-switching thread." )
	);

	DWORD ThreadID = 0;
	hThread = CreateThread
	(
		NULL,
		0,
		StaticThreadProc,
		this,
		0,
		&ThreadID
	);

	if ( hThread == NULL )
	{
		WriteToLog
		(
			TEXT("CreateThread failed with error %u."),
			::GetLastError()
		);

		Abort();

		goto Cleanup;
	};


	//
	// Disable framework painting.  
	// Our device may be lost, and the framework will trigger a
	// failure if we allow it to paint.
	//

	m_bHandlePaintMsg = FALSE; 


	WriteToLog
	(
		_T( "Rendering." )
	);

	__try
	{
		hr = D3DERR_DEVICENOTRESET;

		//
		// Loop 5 times.  Our mode-changing thread varies its timing each time through.
		//

		for ( INT iLoop = 0; iLoop < 5; iLoop++ )
		{
			// Pause the mode-switching thread.
			ResetEvent( m_phEvents[ 1 ] );


			//
			// Wait until last mode switch is complete (just in case this 
			// thread is a little ahead).
			//

			//OutputDebugString( TEXT( "ExecuteTest:  Waiting for event 2.\n" ) );
			if ( WaitForSingleObject( m_phEvents[ 2 ], 90000 ) == WAIT_TIMEOUT )
			{
				DPF
				(
					1,
					TEXT("ExecuteTest:  WaitForSingleObject timed out.  Something is wrong with the mode-changing thread.")
				);

				Fail();

				goto Cleanup;
			};

			//OutputDebugString( TEXT( "ExecuteTest:  Event 2 signalled.\n" ) );

			// If the device is lost...
			if ( m_pSrcDevice->IsDeviceLost() )
			{
				// ...then release our objects...
				ReleaseObjects();
				RELEASE(pVB);
				RELEASE(pIB);

				// ...and wait for it to become ready for reset...
				StringCchPrintf
				(
					ThisCall,
					MAX_PATH,
					TEXT("IsDeviceReadyForReset()")
				);

				for
				(
					int iCurrentAttempt = 0;
					iCurrentAttempt < 30000;
					++iCurrentAttempt
				)
				{
					Sleep( 1 );
					if ( m_pSrcDevice->IsDeviceReadyForReset() == true )
					{
						hr = D3DERR_DEVICENOTRESET;
						break;
					};
				};

				// ...and if it's still lost...
				if ( m_pSrcDevice->IsDeviceReadyForReset() == false )
				{
					// ...then fail.
					WriteToLog(TEXT("Device continues to be lost; test cannot continue."));

					Fail();

					goto Cleanup;
				};

				GetLastError();
			};


			// If the device is lost and ready for reset
			// (or this is the first time through the loop)...
			if ( hr == D3DERR_DEVICENOTRESET )
			{
				// ...then we need to reset devices, and re-create
				// our objects.


				//
				// Reset device(s) if necessary.
				//
				// N.B. [SEdmison]:
				// We use a critical section to keep the mode-switching
				// thread from initiating a mode change while we're in
				// the middle of resetting.
				//

				StringCchPrintf
				(
					ThisCall,
					MAX_PATH,
					TEXT("EnterCriticalSection()")
				);
				EnterCriticalSection( &m_ResetLock );

				if
				(
					m_pSrcDevice->IsDeviceLost() == true &&
					m_pSrcDevice->IsDeviceReadyForReset() == true
				)
				{
					StringCchPrintf
					(
						ThisCall,
						MAX_PATH,
						TEXT("HandleDeviceLost()")
					);
					if (!m_pSrcDevice->HandleDeviceLost())
					{
  						WriteToLog(TEXT("HandleDeviceLost failed!  Failing this test case!"));

     					LeaveCriticalSection( &m_ResetLock );

						Fail();

						goto Cleanup;
					};
                };

				StringCchPrintf
				(
					ThisCall,
					MAX_PATH,
					TEXT("LeaveCriticalSection()")
				);

				LeaveCriticalSection( &m_ResetLock );


				//
				// Turn on clipping.
				//

				StringCchPrintf
				(
					ThisCall,
					MAX_PATH,
					TEXT("SetRenderState(D3DRS_CLIPPING,TRUE)")
				);
				hr = m_pSrcDevice->SetRenderState( D3DRS_CLIPPING, TRUE );
				if ( FAILED( hr ) )
				{
					WriteToLog
					(
						TEXT("%s returned %s (0x%x)."),
						ThisCall,
						m_pD3D->HResultToString(hr),
						hr
					);

					Fail();

					goto Cleanup;
				};


				//
				// Create the vertex buffer.
				//

				StringCchPrintf
				(
					ThisCall,
					MAX_PATH,
					TEXT("CreateVertexBuffer(%d,0x%x,0x%x,%d,)"),
					m_shape.m_nVertices * sizeof(D3DVERTEX),
					s_InternalPointerAliasTestCases[ nTest - 1 ].Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : NULL),
					D3DFVF_VERTEX,
					s_InternalPointerAliasTestCases[ nTest - 1 ].Pool
				);

				hr = m_pSrcDevice->CreateVertexBuffer
				(
					m_shape.m_nVertices * sizeof(D3DVERTEX),
					s_InternalPointerAliasTestCases[ nTest - 1 ].Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : NULL),
					D3DFVF_VERTEX,
					s_InternalPointerAliasTestCases[ nTest - 1 ].Pool,
					&pVB
				);

				if ( FAILED( hr ) )
				{
					WriteToLog
					(
						TEXT("%s returned %s (0x%x)."),
						ThisCall,
						m_pD3D->HResultToString(hr),
						hr
					);

					Fail();

					goto Cleanup;
				};


				//
				// Create the index buffer.
				//

				StringCchPrintf
				(
					ThisCall,
					MAX_PATH,
					TEXT("CreateIndexBuffer(%d,0x%x,%d,%d,)"),
					m_shape.m_nIndices * sizeof(WORD),
					s_InternalPointerAliasTestCases[ nTest - 1 ].Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : NULL),
					FMT_INDEX16,
					s_InternalPointerAliasTestCases[ nTest - 1 ].Pool
				);

				hr = m_pSrcDevice->CreateIndexBuffer
				(
					m_shape.m_nIndices * sizeof(WORD),
					s_InternalPointerAliasTestCases[ nTest - 1 ].Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : NULL),
					FMT_INDEX16,
					s_InternalPointerAliasTestCases[ nTest - 1 ].Pool,
					&pIB
				);

				if ( FAILED( hr ) )
				{
					WriteToLog
					(
						TEXT("%s returned %s (0x%x)."),
						ThisCall,
						m_pD3D->HResultToString(hr),
						hr
					);

					Fail();

					goto Cleanup;
				};


				//
				// Lock the vertex buffer.
				//

				StringCchPrintf
				(
					ThisCall,
					MAX_PATH,
					TEXT("VB::Lock(0,0,,0x%x)"),
					s_InternalPointerAliasTestCases[ nTest - 1 ].LockFlag
				);

				BYTE * pVertices = NULL;
				hr = pVB->Lock
				(
					0,
					0,
					&pVertices,
					s_InternalPointerAliasTestCases[ nTest - 1 ].LockFlag
				);

				if ( FAILED( hr ) )
				{
					WriteToLog
					(
						TEXT("%s returned %s (0x%x)."),
						ThisCall,
						m_pD3D->HResultToString(hr),
						hr
					);

					Fail();

					goto Cleanup;
				};


				//
				// Fill the vertex buffer.
				//

				StringCchPrintf
				(
					ThisCall,
					MAX_PATH,
					TEXT("CopyMemory(vertices)")
				);
				CopyMemory
				(
					pVertices,
					m_shape.m_pVertices,
					m_shape.m_nVertices * sizeof(D3DVERTEX)
				);


				//
				// Unlock the vertex buffer.
				//

				StringCchPrintf
				(
					ThisCall,
					MAX_PATH,
					TEXT("VB::Unlock()")
				);
				hr = pVB->Unlock();
				if ( FAILED( hr ) )
				{
					WriteToLog
					(
						TEXT("%s returned %s (0x%x)."),
						ThisCall,
						m_pD3D->HResultToString(hr),
						hr
					);

					Fail();

					goto Cleanup;
				};


				//
				// Lock the index buffer.
				//

				StringCchPrintf
				(
					ThisCall,
					MAX_PATH,
					TEXT("IB::Lock(0,0,,0x%x)"),
					s_InternalPointerAliasTestCases[ nTest - 1 ].LockFlag
				);

				BYTE * pIndices = NULL;
				hr = pIB->Lock
				(
					0,
					0,
					&pIndices,
					s_InternalPointerAliasTestCases[ nTest - 1 ].LockFlag
				);

				if ( FAILED( hr ) )
				{
					WriteToLog
					(
						TEXT("%s returned %s (0x%x)."),
						ThisCall,
						m_pD3D->HResultToString(hr),
						hr
					);

					Fail();

					goto Cleanup;
				};


				//
				// Fill the index buffer.
				//

				StringCchPrintf
				(
					ThisCall,
					MAX_PATH,
					TEXT("CopyMemory(indices)")
				);
				CopyMemory
				(
					pIndices,
					m_shape.m_pIndices,
					m_shape.m_nIndices * sizeof(WORD)
				);


				//
				// Unlock the index buffer.
				//

				StringCchPrintf
				(
					ThisCall,
					MAX_PATH,
					TEXT("IB::Unlock()")
				);
				hr = pIB->Unlock();

				if ( FAILED( hr ) )
				{
					WriteToLog
					(
						TEXT("%s returned %s (0x%x)."),
						ThisCall,
						m_pD3D->HResultToString(hr),
						hr
					);

					Fail();

					goto Cleanup;
				};


				//
				// Set the stream source to the vertex buffer.
				//

				StringCchPrintf
				(
					ThisCall,
					MAX_PATH,
					TEXT("SetStreamSource()")
				);
				hr = m_pSrcDevice->SetStreamSource
				(
					0,
					pVB,
					sizeof(D3DVERTEX)
				);

				if ( FAILED( hr ) )
				{
					WriteToLog
					(
						TEXT("%s returned %s (0x%x)."),
						ThisCall,
						m_pD3D->HResultToString(hr),
						hr
					);

					Fail();

					goto Cleanup;
				};


				//
				// Set indices to the index buffer.
				//

				StringCchPrintf
				(
					ThisCall,
					MAX_PATH,
					TEXT("SetIndices()")
				);
				hr = m_pSrcDevice->SetIndices
				(
					pIB,
					0
				);

				if ( FAILED( hr ) )
				{
					WriteToLog
					(
						TEXT("%s returned %s (0x%x)."),
						ThisCall,
						m_pD3D->HResultToString(hr),
						hr
					);

					Fail();

					goto Cleanup;
				};


				StringCchPrintf
				(
					ThisCall,
					MAX_PATH,
					TEXT("SetVertexShader(D3DFVF_VERTEX)")
				);
				hr = m_pSrcDevice->SetVertexShader( D3DFVF_VERTEX );

				if ( FAILED( hr ) )
				{
					WriteToLog
					(
						TEXT("%s returned %s (0x%x)."),
						ThisCall,
						m_pD3D->HResultToString(hr),
						hr
					);

					Fail();

					goto Cleanup;
				};
			};


			//
			// Signal mode changing thread to do its thing.
			//

			SetEvent( m_phEvents[ 1 ] ); 


			//
			// Repeat until we detect a device loss condition
			// or we time out.
			//

			DWORD dwStartTime = timeGetTime();
			hr = S_OK;
			for
			(
				DWORD Interval = timeGetTime() - dwStartTime;
				Interval < INTERNALPOINTERALIAS_TIMEOUT &&
					SUCCEEDED( hr );
				Interval = timeGetTime() - dwStartTime
			)
			{
				//
				// Draw the scene.
				//

				StringCchPrintf
				(
					ThisCall,
					MAX_PATH,
					TEXT("Clear()")
				);
				hr = m_pSrcDevice->Clear
				(
					0,
					NULL,
					D3DCLEAR_TARGET,
					D3DCOLOR_XRGB(0x00, 0x00, 0x00),
					0.0f,
					0
				);

				if
				(
					FAILED( hr ) &&
					hr != D3DERR_DEVICELOST &&
					hr != D3DERR_DEVICENOTRESET
				)
				{
					WriteToLog
					(
						TEXT("%s returned %s (0x%x)."),
						ThisCall,
						m_pD3D->HResultToString(hr),
						hr
					);

					Fail();

					goto Cleanup;
				};


				StringCchPrintf
				(
					ThisCall,
					MAX_PATH,
					TEXT("BeginScene()")
				);
				hr = m_pSrcDevice->BeginScene( );
				if
				(
					FAILED( hr ) &&
					hr != D3DERR_DEVICELOST &&
					hr != D3DERR_DEVICENOTRESET
				)
				{
					WriteToLog
					(
						TEXT("%s returned %s (0x%x)."),
						ThisCall,
						m_pD3D->HResultToString(hr),
						hr
					);

					Fail();

					goto Cleanup;
				};


				StringCchPrintf
				(
					ThisCall,
					MAX_PATH,
					TEXT("DrawIndexedPrimitive()")
				);
				hr = m_pSrcDevice->DrawIndexedPrimitive
				(
					D3DPT_TRIANGLELIST,
					0,
					m_shape.m_nVertices,
					0,
					m_shape.m_nIndices / 3
				);
				if
				(
					FAILED( hr ) &&
					hr != D3DERR_DEVICELOST &&
					hr != D3DERR_DEVICENOTRESET
				)
				{
					WriteToLog
					(
						TEXT("%s returned %s (0x%x)."),
						ThisCall,
						m_pD3D->HResultToString(hr),
						hr
					);

					Fail();

					goto Cleanup;
				};


				StringCchPrintf
				(
					ThisCall,
					MAX_PATH,
					TEXT("EndScene()")
				);
				hr = m_pSrcDevice->EndScene();
				if
				(
					FAILED( hr ) &&
					hr != D3DERR_DEVICELOST &&
					hr != D3DERR_DEVICENOTRESET
				)
				{
					WriteToLog
					(
						TEXT("%s returned %s (0x%x)."),
						ThisCall,
						m_pD3D->HResultToString(hr),
						hr
					);

					Fail();

					goto Cleanup;
				};


				//
				// Present the scene.
				//
				// N.B.:
				// Only present will actually report the device loss error code.
				// all previous functions should handle it silently.
				//

				StringCchPrintf
				(
					ThisCall,
					MAX_PATH,
					TEXT("Present()")
				);
				hr = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );
				if
				(
					hr == E_INVALIDARG ||
					hr == E_OUTOFMEMORY
				)
				{
					// Have the framework ignore this error.
					GetLastError();

					Skip();

					goto Cleanup;
				};


				if
				(
					FAILED( hr ) &&
					hr != D3DERR_DEVICELOST &&
					hr != D3DERR_DEVICENOTRESET
				)
				{
					WriteToLog
					(
						TEXT("%s returned %s (0x%x)."),
						ThisCall,
						m_pD3D->HResultToString(hr),
						hr
					);

					Fail();

					goto Cleanup;
				};
			};


			//
			// Eat device loss error codes so that the framework
			// doesn't catch them and log a failure.
			//

			if ( hr == D3DERR_DEVICELOST )
			{
				GetLastError();
			};
		};
	}
	__except
	(
		ExceptionFilter
		(
			GetExceptionCode(),
			GetExceptionInformation()
		)
	)
	{
		WriteToLog
		(
			TEXT("%s caused an unhandled exception."),
			ThisCall
		);

		Fail();
	};


Cleanup:
	//
	// Re-enable framework handling of paint messages.
	//

	m_bHandlePaintMsg = TRUE;


	//
	// Clean up our worker thread.
	//

	if ( hThread != NULL )
	{
		if ( m_phEvents[ 0 ] != NULL )
		{
			// Tell the mode changing thread to terminate...
			SetEvent( m_phEvents[ 0 ] );
		};

		// ...and wait for it to do so.
		//OutputDebugString( TEXT( "ExecuteTest:  Waiting for worker thread to exit.\n" ) );
		DWORD WaitResult = WaitForSingleObject
		(
			hThread,
			35000
		);

		if ( WaitResult == WAIT_TIMEOUT )
		{
			DPF
			(
				1,
				TEXT( "ExecuteTest:  Mode-switching thread failed to exit." )
			);
		}
		else
		{
			DWORD dwExitCode = 0;
			BOOL bGetThreadResult = GetExitCodeThread
			(
				hThread,
				&dwExitCode
			);

			if ( bGetThreadResult != TRUE )
			{
				WriteToLog
				(
					TEXT( "ExecuteTest:  GetExitCodeThread failed." )
				);
			};

			if ( dwExitCode != 0 )
			{
				WriteToLog
				(
					TEXT( "ExecuteTest:  Mode-switching thread failed:  %s." ),
					s_ModeThreadError
				);

				Fail();
			};
		};
	};


	//
	// Release our buffers.
	//

	RELEASE( pVB );
	RELEASE( pIB );


	//
	// Close our worker thread handle.
	//

	CLOSEHANDLE( hThread );


	//
	// Close our event handles.
	//

	CLOSEHANDLE( m_phEvents[ 0 ] );
	CLOSEHANDLE( m_phEvents[ 1 ] );
	CLOSEHANDLE( m_phEvents[ 2 ] );


	//
	// Delete our critical section.
	//

	DeleteCriticalSection( &m_ResetLock );


	//
	// Handle any remaining device-loss conditions, since the
	// framework expects tests to clean up after themselves.
	//

	if ( !m_pSrcDevice->HandleDeviceLost() )
	{
		WriteToLog
		(
			TEXT("Reset failed as test was exiting!")
		);
	};


	//
	// Make sure we don't have any residual DEVICELOST or DEVICENOTRESET errors. 
	// The framework has its own checks and will trigger a failure.
	//

	GetLastError(); 

	return false;
};
