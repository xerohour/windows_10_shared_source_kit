//////////////////////////////////////////////////////////////////////
// File:  CSystemWindowWrapper.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a Win32 window thread object.
//
// History:
// 14 Jan 2014  SEdmison  Created.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include "CSystemWindowWrapper.hpp"


/////////////////////////////////////////////////////////////////
//
// Constants
//
/////////////////////////////////////////////////////////////////

#define TIMEOUT_FOR_WAIT 10000

#define WINDOW_CLASS_NAME L"SwapChains"
#define WINDOW_CAPTION L"Swap chain rendering test"

// Quick reference to the top of the containing binary
EXTERN_C IMAGE_DOS_HEADER __ImageBase;


/////////////////////////////////////////////////////////////////
//
// Implementation
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// CSystemWindowWrapper implementation
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CSystemWindowWrapper
//
// Purpose:
// Constructor.
//
// Runs on:
// Parent thread.
///////////////////////////////////////////////////////
CSystemWindowWrapper::CSystemWindowWrapper()
:
	m_pLogger( nullptr ),
	m_WindowLeft( 0 ),
	m_WindowTop( 0 ),
	m_WindowRight( 1920 ),
	m_WindowBottom( 1080 ),
	m_hWindow( nullptr ),
	m_hWindowThread( nullptr ),
	m_hWindowLaunchedEvent( nullptr ),
	m_hWindowClosedEvent( nullptr ),
	m_pSceneRenderer( nullptr )
{
};


CSystemWindowWrapper::CSystemWindowWrapper
(
	UINT WindowWidth,
	UINT WindowHeight
)
:
	m_pLogger( nullptr ),
	m_WindowLeft( 0 ),
	m_WindowTop( 0 ),
	m_WindowRight( WindowWidth ),
	m_WindowBottom( WindowHeight ),
	m_hWindow( nullptr ),
	m_hWindowThread( nullptr ),
	m_hWindowLaunchedEvent( nullptr ),
	m_hWindowClosedEvent( nullptr ),
	m_pSceneRenderer( nullptr )
{
};


CSystemWindowWrapper::CSystemWindowWrapper
(
	LONG WindowLeft,
	LONG WindowTop,
	LONG WindowRight,
	LONG WindowBottom
)
:
	m_pLogger( nullptr ),
	m_WindowLeft( WindowLeft ),
	m_WindowTop( WindowTop ),
	m_WindowRight( WindowRight ),
	m_WindowBottom( WindowBottom ),
	m_hWindow( nullptr ),
	m_hWindowThread( nullptr ),
	m_hWindowLaunchedEvent( nullptr ),
	m_hWindowClosedEvent( nullptr ),
	m_pSceneRenderer( nullptr )
{
};


///////////////////////////////////////////////////////
// Function:  ~CSystemWindowWrapper
//
// Purpose:
// Destructor.
//
// Runs on:
// Parent thread.
///////////////////////////////////////////////////////
CSystemWindowWrapper::~CSystemWindowWrapper()
{
	//
	// Make sure that state has been cleaned up.
	//

	StopSystemWindowThread();

	assert( m_hWindow == nullptr );


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
void CSystemWindowWrapper::SetLogger
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
// Function:  GetSystemWindow
//
// Purpose:
// Returns the wrapper window handle.
///////////////////////////////////////////////////////
HWND CSystemWindowWrapper::GetSystemWindow()
{
	return m_hWindow;
};


///////////////////////////////////////////////////////
// Function:  GetSceneRenderer
//
// Purpose:
// Returns the window object's scene renderer.
///////////////////////////////////////////////////////
CSceneRenderer * CSystemWindowWrapper::GetSceneRenderer()
{
	return m_pSceneRenderer;
};


///////////////////////////////////////////////////////
// Function:  StartSystemWindowThread
//
// Purpose:
// Starts a worker thread, which in turn creates a
// window and begins processing window messages and
// rendering until the window-closed event becomes
// signalled.
//
// Runs on:
// Parent thread.
///////////////////////////////////////////////////////
HRESULT CSystemWindowWrapper::StartSystemWindowThread()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Register the window class.
	//

	HRESULT hRegisterWindowClassResult = RegisterWindowClass();

	if ( FAILED( hRegisterWindowClassResult ) )
	{
		hOverallResult = hRegisterWindowClassResult;
		goto Cleanup;
	};


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
		&CSystemWindowWrapper::ThreadProc,
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
// Function:  StopSystemWindowThread
//
// Purpose:
// Closes the window and sets the window-closed
// event to cause the worker thread to exit.
//
// Runs on:
// Parent thread.
///////////////////////////////////////////////////////
HRESULT CSystemWindowWrapper::StopSystemWindowThread()
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
		// Close the window.
		//

		if ( m_hWindow != nullptr )
		{
			DestroyWindow( m_hWindow );
			m_hWindow = nullptr;
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
					L"CSystemWindowWrapper thread exited successfully."
				);
			} break;


			case WAIT_TIMEOUT:
			{
				LOG_MESSAGE
				(
					MESSAGE_TYPE_WARNING,
					L"CSystemWindowWrapper thread timed out."
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
						L"WaitForSingoeObject on CSystemWindowWrapper thread handle returned unexpected result %u.",
						waitResult
					)
				);

				goto Cleanup;
			} break;
		};
	};


	hOverallResult = S_OK;


Cleanup:
	//
	// Unregister the window class.
	//

	UnregisterWindowClass();


	//
	// Close the window thread handle.
	//

	if ( m_hWindowThread != nullptr )
	{
		CloseHandle( m_hWindowThread );
		m_hWindowThread = nullptr;
	};


	//
	// Close the window-launched event handle.
	//

	if ( m_hWindowLaunchedEvent != nullptr )
	{
		CloseHandle( m_hWindowLaunchedEvent );
		m_hWindowLaunchedEvent = nullptr;
	};


	//
	// Close the window-closed event handle.
	//

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
DWORD WINAPI CSystemWindowWrapper::ThreadProc
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

	SetThreadName( 0xFFFFFFFF, "System window wrapper thread" );


	//
	// Interpret the context parameter as a this pointer.
	//

	CSystemWindowWrapper * pWindowWrapper = (CSystemWindowWrapper *) pContext;

#pragma warning( suppress:  28922 )
	if ( pWindowWrapper == nullptr )
	{
		OverallResult = 1;
		goto Cleanup;
	};


	//
	// Have the window wrapper object create a window.
	//

	wprintf
	(
		L"CSystemWindowWrapper::ThreadProc:  Creating window.\n"
	);

	HRESULT hCreateWindowResult = pWindowWrapper->CreateSystemWindow();

	if ( FAILED( hCreateWindowResult ) )
	{
		fwprintf
		(
			stderr,
			L"CreateSystemWindow failed.\n"
		);

		OverallResult = 3;
		goto Cleanup;
	};


	//
	// Have the window wrapper object render and process events
	// until the window-closed event is signalled.
	//

	wprintf
	(
		L"CSystemWindowWrapper::ThreadProc:  Entering message loop.\n"
	);

	HRESULT hProcessEventsResult = pWindowWrapper->RenderAndProcessWindowEvents();

	if ( FAILED( hProcessEventsResult ) )
	{
		fwprintf
		(
			stderr,
			L"RenderAndProcessWindowEvents failed.\n"
		);

		OverallResult = 4;
		goto Cleanup;
	};


	//
	// Have the window wrapper object clean up.
	//

	wprintf
	(
		L"CSystemWindowWrapper::ThreadProc:  Destroying window.\n"
	);

	pWindowWrapper->DestroySystemWindow();


	OverallResult = 0;


Cleanup:
	return OverallResult;
};


///////////////////////////////////////////////////////
// Function:  CreateSystemWindow
//
// Purpose:
// Helper method that creates the window.
//
// Runs on:
// Child thread.
///////////////////////////////////////////////////////
HRESULT CSystemWindowWrapper::CreateSystemWindow()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Create a window.
	//

	DWORD ExtendedStyles = IsDebuggerPresent() == TRUE ? 0 : WS_EX_TOPMOST;

	assert( m_WindowRight - m_WindowLeft > 0 );
	assert( m_WindowBottom - m_WindowTop > 0 );
	WCHAR WindowCaption[ 256 ] = WINDOW_CAPTION;
	m_hWindow = CreateWindowExW
	(
		ExtendedStyles,
		WINDOW_CLASS_NAME,
		WindowCaption,
		WS_POPUP | WS_VISIBLE,
		m_WindowLeft,
		m_WindowTop,
		m_WindowRight - m_WindowLeft,
		m_WindowBottom - m_WindowTop,
		nullptr,
		nullptr,
		nullptr,
		static_cast< void * >( this )
	);

	if ( m_hWindow == nullptr )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"CreateWindowEx failed."
		);

		goto Cleanup;
	};

	assert( IsWindow( m_hWindow ) == TRUE );


	//
	// Show the window.
	//

	ShowWindow
	(
		m_hWindow,
		SW_SHOWNORMAL
	);


	//
	// Bring the window to the foreground.
	//

	BOOL bSetForegroundWindowResult = SetForegroundWindow( m_hWindow );

	if ( bSetForegroundWindowResult != TRUE )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_WARNING,
			L"SetForegroundWindow failed."
		);
	};


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


	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  DestroySystemWindow
//
// Purpose:
// Helper method that destroys the window.
//
// Runs on:
// Child thread.
///////////////////////////////////////////////////////
void CSystemWindowWrapper::DestroySystemWindow()
{
	//
	// Release connection to scene renderer.
	//

	m_pSceneRenderer = nullptr;


	//
	// Destroy the window.
	//

	if ( m_hWindow != nullptr )
	{
		DestroyWindow( m_hWindow );
		m_hWindow = nullptr;
	};
};


///////////////////////////////////////////////////////
// Function:  OnWindowClosed
//
// Purpose:
// Window-closed event handler callback.
//
// Runs on:
// Child thread.
///////////////////////////////////////////////////////
HRESULT CSystemWindowWrapper::OnWindowClosed()
{
	//
	// Set the window-closed event.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"CSystemWindowWrapper::OnWindowClosed:  Setting window-closed event."
	);

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
// Child thread.
///////////////////////////////////////////////////////
HRESULT CSystemWindowWrapper::OnWindowSizeChanged
(
	UINT NewWidth,
	UINT NewHeight
)
{
	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		WEX::Common::String().Format
		(
			L"CSystemWindowWrapper::OnWindowSizeChanged:  New window size:  %u x %u.",
			NewWidth,
			NewHeight
		)
	);


	//
	// Re-create size-dependent resources.
	//

	RecreateSizeDependentResources
	(
		NewWidth,
		NewHeight
	);


	return S_OK;
};


///////////////////////////////////////////////////////
// Function:  RecreateSizeDependentResources
//
// Purpose:
// Re-creates any resources that are dependent on the
// size of the window.
//
// Runs on:
// Child thread.
///////////////////////////////////////////////////////
HRESULT CSystemWindowWrapper::RecreateSizeDependentResources
(
	UINT NewWidth,
	UINT NewHeight
)
{
	if
	(
		NewWidth == 0 ||
		NewHeight == 0
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
// Function:  RegisterWindowClass
//
// Purpose:
// Registers the class for the window.
//
// Runs on:
// Child thread.
///////////////////////////////////////////////////////
HRESULT CSystemWindowWrapper::RegisterWindowClass()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Fill out our window class struct.
	//

	WNDCLASSEXW WindowClass;
	ZeroMemory( &WindowClass, sizeof( WindowClass ) );
	WindowClass.cbSize = sizeof( WindowClass );
	WindowClass.style = CS_DBLCLKS;
	WindowClass.lpfnWndProc = StaticMsgProc;
	WindowClass.cbClsExtra = 0;
	WindowClass.cbWndExtra = sizeof( PVOID );
	WindowClass.hInstance = nullptr; // (HINSTANCE) &__ImageBase;
	WindowClass.hbrBackground = (HBRUSH) COLOR_WINDOW + 1;
	WindowClass.lpszClassName = WINDOW_CLASS_NAME;


	//
	// Register our window class.
	//

	m_WindowClassAtom = RegisterClassExW( &WindowClass );

	if ( m_WindowClassAtom == 0 )
	{
		DWORD RegisterClassError = GetLastError();

		if ( RegisterClassError != ERROR_CLASS_ALREADY_EXISTS )
		{
			LOG_ERROR
			(
				ERROR_TYPE_WIN32,
				RegisterClassError,
				L"RegisterClassEx failed."
			);

			goto Cleanup;
		};
	};


	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  UnregisterWindowClass
//
// Purpose:
// Unregisters the window class atom.
//
// Runs on:
// Child thread.
///////////////////////////////////////////////////////
HRESULT CSystemWindowWrapper::UnregisterWindowClass()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Unregister the window class.
	//

	BOOL bUnregisterClassResult = UnregisterClassW
	(
		WINDOW_CLASS_NAME,
		GetModuleHandle( nullptr )
	);

	if ( bUnregisterClassResult != TRUE )
	{
		DWORD UnregisterClassError = GetLastError();

		if ( UnregisterClassError != ERROR_CLASS_DOES_NOT_EXIST )
		{
			LOG_ERROR
			(
				ERROR_TYPE_WIN32,
				UnregisterClassError,
				L"UnregisterClass failed"
			);

			hOverallResult = HRESULT_FROM_WIN32( UnregisterClassError );
			goto Cleanup;
		};
	};


	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  RenderAndProcessWindowEvents
//
// Purpose:
// Helper method to process events and render until the
// window-closed event is signalled.
//
// Runs on:
// Child thread.
///////////////////////////////////////////////////////
HRESULT CSystemWindowWrapper::RenderAndProcessWindowEvents()
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
		// Process messages if there are any.
		//

		MSG msg;
		if
		(
			PeekMessage
			(
				&msg,
				nullptr,
				0,
				0,
				PM_REMOVE
			)
		)
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		};


		//
		// Check whether the window-closed event has been
		// signalled.
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
// Function:  SetSceneRenderer
//
// Purpose:
// Associates the window wrapper object with a scene
// renderer object.
//
// Runs on:
// Parent thread.
///////////////////////////////////////////////////////
void CSystemWindowWrapper::SetSceneRenderer
(
	_In_ CSceneRenderer * pSceneRenderer
)
{
	m_pSceneRenderer = pSceneRenderer;
};


///////////////////////////////////////////////////////
// Function:  MsgProc
//
// Purpose:
// Message handler routine.
//
// Called by:
// StaticMsgProc
//
// Runs on:
// Child thread.
///////////////////////////////////////////////////////
LRESULT CSystemWindowWrapper::MsgProc
(
	_In_ HWND hWindow,
	UINT Message,
	WPARAM wParam,
	LPARAM lParam
)
{
	//
	// Validate parameters.
	//

	if
	(
		m_hWindow != nullptr &&
		hWindow == m_hWindow
	)
	{
		//
		// Handle expected messages.
		//

		switch ( Message )
		{
			case WM_CLOSE:
			{
				OnWindowClosed();

				return 0;
			} break;


			case WM_SIZE:
			{
				RECT WindowRectangle;
				ZeroMemory( &WindowRectangle, sizeof( WindowRectangle ) );
				BOOL bGetWindowRectangleResult = GetWindowRect
				(
					hWindow,
					&WindowRectangle
				);

				if ( bGetWindowRectangleResult != TRUE )
				{
					DWORD GetWindowRectangleError = GetLastError();

					LOG_ERROR
					(
						ERROR_TYPE_WIN32,
						GetWindowRectangleError,
						L"GetWindowRect failed."
					);

					return 0;
				};


				OnWindowSizeChanged
				(
					static_cast< UINT >( WindowRectangle.right - WindowRectangle.left ),
					static_cast< UINT >( WindowRectangle.bottom - WindowRectangle.top )
				);

				return 0;
			} break;
		};
	};


	//
	// Do default handling of unrecognized messages.
	//

	return DefWindowProc
	(
		hWindow,
		Message,
		wParam,
		lParam
	);
};


///////////////////////////////////////////////////////
// Function:  StaticMsgProc
//
// Purpose:
// Static message handler routine.
//
// Called by:
// [System]
//
// Runs on:
// Child thread.
///////////////////////////////////////////////////////
LRESULT WINAPI CSystemWindowWrapper::StaticMsgProc
(
	_In_ HWND hWindow,
	UINT Message,
	WPARAM wParam,
	LPARAM lParam
)
{
	//
	// Stash away our this pointer during handling of
	// the WM_NCCREATE message.
	//

	if ( Message == WM_NCCREATE )
	{
		SetWindowLongPtr
		(
			hWindow,
			GWLP_USERDATA,
			reinterpret_cast< LONG_PTR >( ( reinterpret_cast< LPCREATESTRUCT >( lParam ) )->lpCreateParams )
		);

		return 1;
	};


	//
	// Retrieve our this pointer.
	//

	LONG_PTR pContext = GetWindowLongPtr
	(
		hWindow,
		GWLP_USERDATA
	);


	//
	// Do default handling in the absence of a this pointer.
	//

	if ( pContext == 0 )
	{
		return DefWindowProc
		(
			hWindow,
			Message,
			wParam,
			lParam
		);
	};


	//
	// Cast the pointer that we stashed away previously.
	//

	CSystemWindowWrapper * pThis = (CSystemWindowWrapper *) pContext;

#pragma warning( suppress:  28922 )
	assert( pThis != nullptr );


	//
	// Hand off to the member method.
	//

	return pThis->MsgProc
	(
		hWindow,
		Message,
		wParam,
		lParam
	);
};
