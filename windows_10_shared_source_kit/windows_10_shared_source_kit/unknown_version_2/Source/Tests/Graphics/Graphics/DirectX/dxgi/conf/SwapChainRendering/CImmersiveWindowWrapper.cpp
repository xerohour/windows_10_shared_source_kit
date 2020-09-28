//////////////////////////////////////////////////////////////////////
// File:  CImmersiveWindowWrapper.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a core window wrapper object.
//
// History:
// 12 Dec 2013  SEdmison  Imported from
//                        windowstest\DWM\Common\DXScene\DXSceneImmersiveWindow.cpp.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include "CImmersiveWindowWrapper.hpp"


/////////////////////////////////////////////////////////////////
//
// Constants
//
/////////////////////////////////////////////////////////////////

#define TIMEOUT_FOR_WAIT 10000


/////////////////////////////////////////////////////////////////
//
// Implementation
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// CImmersiveWindowWrapper implementation
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CImmersiveWindowWrapper
//
// Purpose:
// Constructor.
//
// Runs on:
// Parent thread.
///////////////////////////////////////////////////////
CImmersiveWindowWrapper::CImmersiveWindowWrapper()
:
	m_WindowLeft( 0 ),
	m_WindowTop( 0 ),
	m_WindowWidth( 100 ),
	m_WindowHeight( 100 ),
	m_pLogger( nullptr ),
	m_CoreWindow( nullptr ),
	m_CoreDispatcher( nullptr ),
	m_hWindowThread( nullptr ),
	m_hWindowLaunchedEvent( nullptr ),
	m_hWindowClosedEvent( nullptr ),
	m_pSceneRenderer( nullptr )
{
};


CImmersiveWindowWrapper::CImmersiveWindowWrapper
(
	UINT WindowWidth,
	UINT WindowHeight
)
:
	m_WindowLeft( 0 ),
	m_WindowTop( 0 ),
	m_WindowWidth( WindowWidth ),
	m_WindowHeight( WindowHeight ),
	m_pLogger( nullptr ),
	m_CoreWindow( nullptr ),
	m_CoreDispatcher( nullptr ),
	m_hWindowThread( nullptr ),
	m_hWindowLaunchedEvent( nullptr ),
	m_hWindowClosedEvent( nullptr ),
	m_pSceneRenderer( nullptr )
{
};


CImmersiveWindowWrapper::CImmersiveWindowWrapper
(
	LONG WindowLeft,
	LONG WindowTop,
	UINT WindowWidth,
	UINT WindowHeight
)
:
	m_WindowLeft( WindowLeft ),
	m_WindowTop( WindowTop ),
	m_WindowWidth( WindowWidth ),
	m_WindowHeight( WindowHeight ),
	m_pLogger( nullptr ),
	m_CoreWindow( nullptr ),
	m_CoreDispatcher( nullptr ),
	m_hWindowThread( nullptr ),
	m_hWindowLaunchedEvent( nullptr ),
	m_hWindowClosedEvent( nullptr ),
	m_pSceneRenderer( nullptr )
{
};


///////////////////////////////////////////////////////
// Function:  ~CImmersiveWindowWrapper
//
// Purpose:
// Destructor.
//
// Runs on:
// Parent thread.
///////////////////////////////////////////////////////
CImmersiveWindowWrapper::~CImmersiveWindowWrapper()
{
	//
	// Make sure that state has been cleaned up.
	//

	StopImmersiveWindowThread();

	assert( m_CoreWindow == nullptr );


	SetLogger( nullptr );
};


///////////////////////////////////////////////////////
// Function:  SetLogger
//
// Purpose:
// Attaches a logger.
//
// Runs on:
// Parent thread.
///////////////////////////////////////////////////////
void CImmersiveWindowWrapper::SetLogger
(
	_In_ ISimpleLog * pLogger
)
{
	SAFE_RELEASE( m_pLogger );

	m_pLogger = pLogger;

	if ( m_pLogger != nullptr )
	{
		m_pLogger->AddRef();
	};
};


///////////////////////////////////////////////////////
// Function:  GetImmersiveWindow
//
// Purpose:
// Returns the wrapper object's core window.
///////////////////////////////////////////////////////
Windows::UI::Core::ICoreWindow * CImmersiveWindowWrapper::GetImmersiveWindow()
{
	return m_CoreWindow.Get();
};


///////////////////////////////////////////////////////
// Function:  GetSceneRenderer
//
// Purpose:
// Returns the core window object's associated scene
// renderer.
///////////////////////////////////////////////////////
CSceneRenderer * CImmersiveWindowWrapper::GetSceneRenderer()
{
	return m_pSceneRenderer;
};


///////////////////////////////////////////////////////
// Function:  StartImmersiveWindowThread
//
// Purpose:
// Starts a worker thread, which in turn creates a core
// window and begins processing window messages and
// rendering until the window-closed event becomes
// signalled.
//
// Runs on:
// Parent thread.
///////////////////////////////////////////////////////
HRESULT CImmersiveWindowWrapper::StartImmersiveWindowThread()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Create the window-launched event.
	//

	m_hWindowLaunchedEvent = CreateEventW
	(
		nullptr,
		FALSE,
		FALSE,
		nullptr
	);

	if ( m_hWindowLaunchedEvent == nullptr || m_hWindowLaunchedEvent == INVALID_HANDLE_VALUE )
	{
		hOverallResult = HRESULT_FROM_WIN32( GetLastError() );
		goto Cleanup;
	};


	//
	// Create the window-closed event.
	//

	m_hWindowClosedEvent = CreateEventW
	(
		nullptr,
		FALSE,
		FALSE,
		nullptr
	);

	if ( m_hWindowClosedEvent == nullptr || m_hWindowLaunchedEvent == INVALID_HANDLE_VALUE )
	{
		hOverallResult = HRESULT_FROM_WIN32( GetLastError() );
		goto Cleanup;
	};


	//
	// Create a worker thread to create and manage the window.
	//

	DWORD threadID = 0;
	m_hWindowThread = CreateThread
	(
		nullptr,
		0,
		&CImmersiveWindowWrapper::ThreadProc,
		this,
		0,
		&threadID
	);

	if ( m_hWindowThread == nullptr )
	{
		hOverallResult = HRESULT_FROM_WIN32( GetLastError() );
		goto Cleanup;
	};


	//
	// Wait for the window-launch event handle to be signalled,
	// indicating that window creation is complete.
	//

	DWORD WaitResult = WaitForSingleObject
	(
		m_hWindowLaunchedEvent,
		( IsDebuggerPresent() == TRUE ) ? ( TIMEOUT_FOR_WAIT * 3 ) : TIMEOUT_FOR_WAIT
	);

	if ( WaitResult != WAIT_OBJECT_0 )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"Window creation event was not signalled."
		);

		DWORD WaitForThreadResult = WaitForSingleObject
		(
			m_hWindowThread,
			( IsDebuggerPresent() == TRUE ) ? INFINITE : TIMEOUT_FOR_WAIT
		);

		if ( WaitForThreadResult == WAIT_OBJECT_0 )
		{
			DWORD ThreadExitCode = 0;
			BOOL bGetThreadExitCodeResult = GetExitCodeThread
			(
				m_hWindowThread,
				&ThreadExitCode
			);

			if ( bGetThreadExitCodeResult == TRUE )
			{
				LOG_ERROR
				(
					ERROR_TYPE_BOOL,
					false,
					WEX::Common::String().Format
					(
						L"Window launcher thread returned exit code %u.",
						ThreadExitCode
					)
				);
			};
		};

		hOverallResult = E_FAIL;
		goto Cleanup;
	};


	hOverallResult = S_OK;


Cleanup:
	if ( FAILED( hOverallResult ) )
	{
		if ( m_hWindowClosedEvent != nullptr )
		{
			CloseHandle( m_hWindowClosedEvent );
			m_hWindowClosedEvent = nullptr;
		};

		if ( m_hWindowLaunchedEvent != nullptr )
		{
			CloseHandle( m_hWindowLaunchedEvent );
			m_hWindowLaunchedEvent = nullptr;
		};

		if ( m_hWindowThread != nullptr )
		{
			CloseHandle( m_hWindowThread );
			m_hWindowThread = nullptr;
		};
	};


	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  StopImmersiveWindowThread
//
// Purpose:
// Closes the core window and sets the window-closed
// event to cause the worker thread to exit.
//
// Runs on:
// Parent thread.
///////////////////////////////////////////////////////
HRESULT CImmersiveWindowWrapper::StopImmersiveWindowThread()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Clean up the worker thread if needed.
	//

	if ( m_hWindowThread != nullptr )
	{
		//
		// Close the core window.
		//

		if ( m_CoreWindow != nullptr )
		{
			m_CoreWindow->Close();
		};


		//
		// Set the window-closed event.
		//

		BOOL bSetEventResult = SetEvent( m_hWindowClosedEvent );

		if ( bSetEventResult != TRUE )
		{
			DWORD SetEventError = GetLastError();

			hOverallResult = HRESULT_FROM_WIN32( SetEventError );
			goto Cleanup;
		};


		//
		// Wait for the worker thread to exit.
		//

		DWORD waitResult = WaitForSingleObject
		(
			m_hWindowThread,
			( IsDebuggerPresent() == TRUE ) ? INFINITE : TIMEOUT_FOR_WAIT
		);

		switch ( waitResult )
		{
			case WAIT_OBJECT_0:
			{
				LOG_MESSAGE
				(
					MESSAGE_TYPE_INFORMATIONAL,
					L"CImmersiveWindowWrapper thread exited successfully."
				);
			} break;


			case WAIT_TIMEOUT:
			{
				LOG_MESSAGE
				(
					MESSAGE_TYPE_WARNING,
					L"CImmersiveWindowWrapper thread timed out."
				);

#pragma warning( suppress:  6258 )
				TerminateThread
				(
					m_hWindowThread,
					1
				);

				goto Cleanup;
			} break;


			default:
			{
				LOG_ERROR
				(
					ERROR_TYPE_BOOL,
					false,
					WEX::Common::String().Format
					(
						L"WaitForSingoeObject on CImmersiveWindowWrapper thread handle returned unexpected result %u.",
						waitResult
					)
				);

				goto Cleanup;
			} break;
		};
	};


	hOverallResult = S_OK;


Cleanup:
	if ( m_hWindowThread != nullptr )
	{
		CloseHandle( m_hWindowThread );
		m_hWindowThread = nullptr;
	};

	if ( m_hWindowLaunchedEvent != nullptr )
	{
		CloseHandle( m_hWindowLaunchedEvent );
		m_hWindowLaunchedEvent = nullptr;

	};

	if ( m_hWindowClosedEvent != nullptr )
	{
		CloseHandle( m_hWindowClosedEvent );
		m_hWindowClosedEvent = nullptr;
	};


	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  ThreadProc
//
// Purpose:
// Thread procedure for the window-creator worker
// thread.
//
// Runs on:
// Child thread.
///////////////////////////////////////////////////////
DWORD WINAPI CImmersiveWindowWrapper::ThreadProc
(
	_In_ void * pContext
)
{
	//
	// Initialize locals.
	//

	DWORD OverallResult = 999;


	//
	// Set the thread name.
	//

	SetThreadName( 0xFFFFFFFF, "Core window wrapper thread" );


	//
	// Interpret the context parameter as a this pointer.
	//

	CImmersiveWindowWrapper * window = (CImmersiveWindowWrapper *) pContext;

#pragma prefast( suppress: 28922 )
	if ( window == nullptr )
	{
		OverallResult = 1;
		goto Cleanup;
	};


	//
	// Initialize the Windows Runtime to single-threaded operation.
	//

	HRESULT hr = RoInitialize( RO_INIT_SINGLETHREADED );

	if ( FAILED( hr ) )
	{
		OverallResult = 2;
		goto Cleanup;
	};


	//
	// Have the window wrapper object create a core window.
	//

	HRESULT hCreateWindowResult = window->CreateImmersiveWindow();

	if ( FAILED( hCreateWindowResult ) )
	{
		OverallResult = 3;
		goto Cleanup;
	};


	//
	// Have the window wrapper object render and process events
	// until the window-closed event is signalled.
	//

	HRESULT hProcessEventsResult = window->RenderAndProcessWindowEvents();

	if ( FAILED( hProcessEventsResult ) )
	{
		OverallResult = 4;
		goto Cleanup;
	};


	//
	// Have the window wrapper object clean up.
	//

	window->DestroyImmersiveWindow();


	OverallResult = 0;


Cleanup:
	if ( OverallResult == 0 || OverallResult > 2 )
	{
		RoUninitialize();
	};

	return OverallResult;
};


///////////////////////////////////////////////////////
// Function:  CreateImmersiveWindow
//
// Purpose:
// Helper method that creates the core window.
//
// Runs on:
// Child thread.
///////////////////////////////////////////////////////
HRESULT CImmersiveWindowWrapper::CreateImmersiveWindow()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Create a core window.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		WEX::Common::String().Format
		(
			L"Creating core window of size (%u, %u) at position (%li, %li).",
			m_WindowWidth,
			m_WindowHeight,
			m_WindowLeft,
			m_WindowTop
		)
	);

	HRESULT hCreateCoreWindowResult = CreateCoreWindow
	(
		IMMERSIVE_BODY,
		L"Swap chain rendering test",
		static_cast< int >( m_WindowLeft ),
		static_cast< int >( m_WindowTop ),
		static_cast< int >( m_WindowWidth ),
		static_cast< int >( m_WindowHeight ),
		nullptr,
		IID_PPV_ARGS( &m_CoreWindow )
	);

	if ( FAILED( hCreateCoreWindowResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hCreateCoreWindowResult,
			L"CreateCoreWindow failed."
		);

		hOverallResult = hCreateCoreWindowResult;
		goto Cleanup;
	};


	//
	// Set the app ID.
	//

	HRESULT hSetAppIDResult = SetAppUserModelID();

	if ( FAILED( hSetAppIDResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hSetAppIDResult,
			L"SetAppID failed."
		);

		hOverallResult = hSetAppIDResult;
		goto Cleanup;
	};


	//
	// Get the dispatcher associated with this window.
	//

	m_CoreWindow->get_Dispatcher( &m_CoreDispatcher );
	if ( m_CoreDispatcher == nullptr )
	{
		goto Cleanup;
	};


	//
	// Register event handlers.
	//

	m_CoreWindow->add_SizeChanged
	(
		Microsoft::WRL::Callback< Windows::UI::Core::IWindowSizeChangedEventHandler >
		(
			this,
			&CImmersiveWindowWrapper::OnWindowSizeChanged
		).Get(),
		&m_WindowSizeChangedToken
	);


	m_CoreWindow->add_Closed
	(
		Microsoft::WRL::Callback< Windows::UI::Core::IWindowClosedEventHandler >
		(
			this,
			&CImmersiveWindowWrapper::OnWindowClosed
		).Get(),
		&m_WindowClosedToken
	);


	//
	// Set the cursor type.
	//

	HRESULT hSetCursorResult = SetImmersiveWindowCursor();

	if ( FAILED( hSetCursorResult ) )
	{
		hOverallResult = hSetCursorResult;
		goto Cleanup;
	};


	//
	// Activate the window so that it will be made visible.
	//

	hOverallResult = m_CoreWindow->Activate();


	//
	// Set the window-launched event.
	//

	BOOL bSetEventResult = SetEvent( m_hWindowLaunchedEvent );

	if ( bSetEventResult != TRUE )
	{
		DWORD SetEventError = GetLastError();

		hOverallResult = HRESULT_FROM_WIN32( SetEventError );
		goto Cleanup;
	};


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  DestroyImmersiveWindow
//
// Purpose:
// Helper method that destroys the core window.
//
// Runs on:
// Child thread.
///////////////////////////////////////////////////////
void CImmersiveWindowWrapper::DestroyImmersiveWindow()
{
	//
	// Release connection to scene renderer.
	//

	m_pSceneRenderer = nullptr;


	//
	// Unregister event handlers.
	//

	if ( m_CoreWindow != nullptr )
	{
		m_CoreWindow->remove_Closed( m_WindowClosedToken );
		m_CoreWindow->remove_SizeChanged( m_WindowSizeChangedToken );
	};


	//
	// Release the core window.
	//

	m_CoreWindow = nullptr;
};


///////////////////////////////////////////////////////
// Function:  OnWindowClosed
//
// Purpose:
// Window-closed event handler callback.
//
// Runs on:
// Event handler callback thread.
///////////////////////////////////////////////////////
HRESULT CImmersiveWindowWrapper::OnWindowClosed
(
	_In_ Windows::UI::Core::ICoreWindow * /*sender*/,
	_In_ Windows::UI::Core::ICoreWindowEventArgs * e
)
{
	//
	// Allow this event to continue to other handlers.
	//

	e->put_Handled( FALSE );


	//
	// Set the thread synchronization event to signal window closing.
	//

	BOOL bSetEventResult = SetEvent( m_hWindowClosedEvent );

	if ( bSetEventResult != TRUE )
	{
		DWORD SetEventError = GetLastError();

		LOG_ERROR
		(
			ERROR_TYPE_WIN32,
			SetEventError,
			L"SetEvent on window-closed event failed."
		);

		return HRESULT_FROM_WIN32( SetEventError );
	};


	return S_OK;
};


///////////////////////////////////////////////////////
// Function:  OnWindowSizeChanged
//
// Purpose:
// Window-size-changed event handler callback.
//
// Runs on:
// Event handler callback thread.
///////////////////////////////////////////////////////
HRESULT CImmersiveWindowWrapper::OnWindowSizeChanged
(
	_In_ Windows::UI::Core::ICoreWindow * /*sender*/,
	_In_ Windows::UI::Core::IWindowSizeChangedEventArgs * e
)
{
	//
	// Get the new window dimensions.
	//

	Windows::Foundation::Size windowSize;
	e->get_Size( &windowSize );


	//
	// Re-create size-dependent resources.
	//

	RecreateSizeDependentResources
	(
		static_cast< UINT >( windowSize.Width ),
		static_cast< UINT >( windowSize.Height )
	);


	return S_OK;
};


///////////////////////////////////////////////////////
// Function:  RecreateSizeDependentResources
//
// Purpose:
// Re-creates any resources that are dependent on the
// size of the core window.
//
// Runs on:
// Event handler callback thread.
///////////////////////////////////////////////////////
HRESULT CImmersiveWindowWrapper::RecreateSizeDependentResources
(
	UINT NewWidth,
	UINT NewHeight
)
{
	if
	(
		NewWidth == 0 ||
		NewHeight > 0
	)
	{
		return S_FALSE;
	};

	if
	(
		m_pSceneRenderer == nullptr
	)
	{
		return S_FALSE;
	};


	//
	// Have the scene object re-create size-dependent resources.
	//

	return m_pSceneRenderer->RecreateSizeDependentResources
	(
		NewWidth,
		NewHeight
	);
};


///////////////////////////////////////////////////////
// Function:  RenderAndProcessWindowEvents
//
// Purpose:
// Helper method to process events and render until the
// window-closed event is signalled.
//
// Runs on:
// Event handler callback thread.
///////////////////////////////////////////////////////
HRESULT CImmersiveWindowWrapper::RenderAndProcessWindowEvents()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Render and process all available events until we receive
	// the signal to stop.
	//

	while ( true )
	{
		//
		// Process all window events that are available.
		//

		m_CoreDispatcher->ProcessEvents
		(
			Windows::UI::Core::CoreProcessEventsOption_ProcessAllIfPresent
		);


		//
		// Check whether the window-closed event has been signalled.
		//

		DWORD WaitResult = WaitForSingleObject
		(
			m_hWindowClosedEvent,
			0
		);

		if ( WaitResult == WAIT_OBJECT_0 )
		{
			break;
		};


		//
		// Render and present a frame.
		//

		if ( m_pSceneRenderer != nullptr )
		{
			HRESULT hRenderOneFrameResult = m_pSceneRenderer->RenderAndPresentOnce();

			if ( FAILED( hRenderOneFrameResult ) )
			{
				hOverallResult = hRenderOneFrameResult;
				goto Cleanup;
			};
		};
	};


	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  SetAppUserModelID
//
// Purpose:
// Sets the application user model ID.
//
// Runs on:
// Child thread.
///////////////////////////////////////////////////////
HRESULT CImmersiveWindowWrapper::SetAppUserModelID()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	Microsoft::WRL::ComPtr< ICoreWindowInterop > coreWindowInterop;

	HWND hWindow = nullptr;

	WCHAR appIdStr[ MAX_PATH ] = L"";

	Microsoft::WRL::ComPtr< IPropertyStore > coreWindowProperties;


	//
	// Set application id so that explorer would start interacting with the window (which would make it full screen)
	//

	HRESULT hQueryForICoreWindowInteropResult = m_CoreWindow.Get()->QueryInterface( IID_PPV_ARGS( &coreWindowInterop ) );

	if ( FAILED( hQueryForICoreWindowInteropResult ) )
	{
		hOverallResult = hQueryForICoreWindowInteropResult;
		goto Cleanup;
	};


	//
	// Get a Win32 window handle for the core window.
	//

	HRESULT hGetWindowHandleResult = coreWindowInterop->get_WindowHandle( &hWindow );

	if ( FAILED( hGetWindowHandleResult ) )
	{
		hOverallResult = hGetWindowHandleResult;
		goto Cleanup;
	};


	//
	// Build an application ID string.
	//

	HRESULT hFormatAppIDResult = StringCchPrintf
	(
		appIdStr,
		ARRAYSIZE( appIdStr ),
		L"CImmersiveWindowWrapper.%p",
		hWindow
	);

	if ( FAILED( hFormatAppIDResult ) )
	{
		hOverallResult = hFormatAppIDResult;
		goto Cleanup;
	};


	//
	// Get the property store for the window.
	//

	HRESULT hGetWindowPropertyStoreResult = SHGetPropertyStoreForWindow
	(
		hWindow,
		IID_PPV_ARGS( &coreWindowProperties )
	);

	if ( FAILED( hGetWindowPropertyStoreResult ) )
	{
		hOverallResult = hGetWindowPropertyStoreResult;
		goto Cleanup;
	};


	//
	// Set the app user model ID property.
	//

	PROPVARIANT vtAppID;
	vtAppID.vt = VT_LPWSTR;
	vtAppID.pwszVal = appIdStr;

	HRESULT hSetAppUserModelIDResult = coreWindowProperties->SetValue
	(
		PKEY_AppUserModel_ID,
		vtAppID
	);

	if ( FAILED( hSetAppUserModelIDResult ) )
	{
		hOverallResult = hSetAppUserModelIDResult;
		goto Cleanup;
	};


	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  SetImmersiveWindowCursor
//
// Purpose:
// Sets the cursor type for the core window to be an
// arrow.
//
// Runs on:
// Child thread.
///////////////////////////////////////////////////////
HRESULT CImmersiveWindowWrapper::SetImmersiveWindowCursor()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	Microsoft::WRL::ComPtr< Windows::UI::Core::ICoreCursorFactory > spCoreCursorFactory;
	Microsoft::WRL::ComPtr< Windows::UI::Core::ICoreCursor > spCursor;


	//
	// Get a core cursor activation factory.
	//

	HRESULT hGetCoreCursorActivationFactoryResult = Windows::Foundation::GetActivationFactory
	(
		Windows::Internal::StringReference
		(
			RuntimeClass_Windows_UI_Core_CoreCursor
		).Get(),
		&spCoreCursorFactory
	);

	if ( FAILED( hGetCoreCursorActivationFactoryResult ) )
	{
		hOverallResult = hGetCoreCursorActivationFactoryResult;
		goto Cleanup;
	};


	//
	// Create an arrow cursor.
	//

	HRESULT hCreateCursorResult = spCoreCursorFactory->CreateCursor
	(
		Windows::UI::Core::CoreCursorType_Arrow,
		0,
		&spCursor
	);

	if ( FAILED( hCreateCursorResult ) )
	{
		hOverallResult = hCreateCursorResult;
		goto Cleanup;
	};


	//
	// Set the core window's cursor to the arrow we've created.
	//

	m_CoreWindow->put_PointerCursor( spCursor.Get() );


	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  SetSceneRenderer
//
// Purpose:
// Associates the core window wrapper object with a
// scene renderer object.
//
// Runs on:
// Parent thread.
///////////////////////////////////////////////////////
void CImmersiveWindowWrapper::SetSceneRenderer
(
	_In_ CSceneRenderer * pSceneRenderer
)
{
	m_pSceneRenderer = pSceneRenderer;
};


///////////////////////////////////////////////////////
// Function:  NotifyWindowResizeManagerOfLayoutCompletion
//
// Purpose:
// Notifies the core window resize manager for our core
// window that we have completed layout.
///////////////////////////////////////////////////////
HRESULT CImmersiveWindowWrapper::NotifyWindowResizeManagerOfLayoutCompletion()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	Microsoft::WRL::ComPtr< Windows::UI::Core::ICoreWindowResizeManagerStatics > spResizeManagerStatics;
	Microsoft::WRL::ComPtr< Windows::UI::Core::ICoreWindowResizeManager > spResizeManager;


	//
	// Get the core window resize manager runtime class name.
	//

	Windows::Internal::StringReference CoreWindowResizeManagerID( RuntimeClass_Windows_UI_Core_CoreWindowResizeManager );


	//
	// Get a core window resize manager activation factory.
	//

	HRESULT hGetActivationFactoryResult = Windows::Foundation::GetActivationFactory
	(
		CoreWindowResizeManagerID.Get(),
		&spResizeManagerStatics
	);

	if ( FAILED( hGetActivationFactoryResult ) )
	{
		hOverallResult = hGetActivationFactoryResult;
		goto Cleanup;
	};


	//
	// Get the core window resize manager associated with the current window.
	//

	HRESULT hGetResizeManagerStaticsResult = spResizeManagerStatics->GetForCurrentView( &spResizeManager );

	if ( FAILED( hGetResizeManagerStaticsResult ) )
	{
		hOverallResult = hGetResizeManagerStaticsResult;
		goto Cleanup;
	};


	//
	// Notify the window resize manager that we're through with layout.
	//

	HRESULT hNotifyLayoutCompletedResult = spResizeManager->NotifyLayoutCompleted();

	if ( FAILED( hNotifyLayoutCompletedResult ) )
	{
		hOverallResult = hNotifyLayoutCompletedResult;
		goto Cleanup;
	};


	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};
