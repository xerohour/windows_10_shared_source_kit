//****************************************************************************
// File:  DCompIsolation.cpp
//
// Purpose:
// Isolates all DirectComposition calls to a separate library so that test
// code for other features (such as graphics) can make DirectComposition
// calls on Windows 8 and above but can still be built for earlier operating
// systems.
//
// Presenting through DComp is referred to as "Windowless" presentation, but
// DComp still ultimately needs a window for final composition.
//
// Copyright (c) Microsoft Corporation
//****************************************************************************

//
// Win32 headers
//

#include <dxgi.h>
#include <dxgi1_2.h>
#include <dcomp.h>

#include <strsafe.h>


//
// C runtime headers
//

#include <assert.h>


//
// STL headers
//

#include <new>


//
// Project headers
//

#include "DCOMPIsolation.h"


typedef HRESULT (STDAPICALLTYPE * PDCOMPCREATEDEVICE)
(
	_In_ IDXGIDevice * dxgiDevice,
	_In_ REFIID iid,
	_Outptr_ void ** dcompositionDevice
);

typedef HRESULT (STDAPICALLTYPE * PDCOMPCREATESURFACEHANDLE)
(
    _In_ DWORD desiredAccess,
    _In_opt_ SECURITY_ATTRIBUTES *securityAttributes,
    _Out_ HANDLE *surfaceHandle
);


//*****************************************************************************
// Function:  CDCompIsolation
//
// Purpose:
// Constructor.
//*****************************************************************************
CDCompIsolation::CDCompIsolation()
:
	m_hDCompLibrary( nullptr ),
	m_pDefaultDeviceWrapper( nullptr ),
	m_pDefaultTargetWrapper( nullptr ),
	m_pLogger( nullptr )
{
}

//*****************************************************************************
// Function:  ~CDCompIsolation
//
// Purpose:
// Destructor.
//*****************************************************************************
CDCompIsolation::~CDCompIsolation()
{
	assert( m_pDefaultTargetWrapper == nullptr );

	assert( m_pDefaultDeviceWrapper == nullptr );

	assert( m_hDCompLibrary == nullptr );

	if ( m_pLogger != nullptr )
	{
		m_pLogger->Release();
		m_pLogger = nullptr;
	};
}


//*****************************************************************************
// Function:  CreateDevice
//
// Purpose:
// Creates a wrapped DirectComposition device object.
//*****************************************************************************
HRESULT CDCompIsolation::CreateDevice
(
	__out CDCompDeviceWrapper ** ppDeviceWrapper
)
{
	//
	// Initialize locals.
	//

	HRESULT hr = E_FAIL;


	//
	// Validate parameters.
	//

	if ( ppDeviceWrapper == nullptr )
	{
		hr = E_POINTER;
		LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"ppDeviceWrapper must be non-NULL."
		);

		goto Cleanup;
	}

	*ppDeviceWrapper = nullptr;


	//
	// Create a DirectComposition (DComp) device wrapper
	//

	*ppDeviceWrapper = new (std::nothrow) CDCompDeviceWrapper( *this );

	if ( *ppDeviceWrapper == nullptr )
	{
		hr = E_OUTOFMEMORY;
		LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"Failed to allocate DirectComposition device wrapper object."
		);

		goto Cleanup;
	};


	//
	// Initialize the device wrapper.
	//

	hr = (*ppDeviceWrapper)->Initialize();
	if ( FAILED( hr ) )
	{
		LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"Initialize on device wrapper failed."
		);

		goto Cleanup;
	}


	hr = S_OK;


Cleanup:
	if ( FAILED( hr ) )
	{
		if ( *ppDeviceWrapper != nullptr )
		{
			delete (*ppDeviceWrapper);
			*ppDeviceWrapper = nullptr;
		}
	}


	return hr;
}


//*****************************************************************************
// Function:  Initialize
//
// Purpose:
// Creates a DirectComposition device.
//*****************************************************************************
HRESULT CDCompIsolation::Initialize()
{
	//
	// Initialize locals.
	//

	HRESULT hr = E_FAIL;

	PDCOMPCREATEDEVICE pDCompCreateDeviceFunc = nullptr;


	//
	// Load DComp.dll.
	//

	m_hDCompLibrary = LoadLibraryExW
	(
		L"DComp.dll",
		nullptr,
		0
	);

	if ( m_hDCompLibrary == nullptr )
	{
		// Don't log an error.  If we can't load DComp.dll, it's
		// likely by design (meaning that we're probably running
		// on Windows 7 or earlier).
		hr = E_NOINTERFACE;
		goto Cleanup;
	}


	//
	// Find the DCompositionCreateDevice entry point.
	//

	pDCompCreateDeviceFunc = (PDCOMPCREATEDEVICE) GetProcAddress
	(
		m_hDCompLibrary,
		"DCompositionCreateDevice"
	);

	if ( pDCompCreateDeviceFunc == nullptr )
	{
		LogError
		(
			ERROR_TYPE_WIN32,
			GetLastError(),
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"GetProcAddess was unable to locate DCompositionCreateDevice inside DComp.dll."
		);

		hr = HRESULT_FROM_WIN32( ERROR_PROC_NOT_FOUND );
		goto Cleanup;
	}


	//
	// Create a DirectComposition (DComp) device wrapper
	//

	hr = CreateDevice( &m_pDefaultDeviceWrapper );
	if ( FAILED( hr ) )
	{
		LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"CreateDevice failed."
		);

		goto Cleanup;
	}


	hr = S_OK;


Cleanup:
	// If the function was unsuccessful...
	if ( FAILED( hr ) )
	{
		// ...then release member objects, so as to keep the
		// isolation object in an uninitialized state.
		if ( m_pDefaultDeviceWrapper != nullptr )
		{
			delete m_pDefaultDeviceWrapper;
			m_pDefaultDeviceWrapper = nullptr;
		}


		if ( m_hDCompLibrary != nullptr )
		{
			FreeLibrary( m_hDCompLibrary );
			m_hDCompLibrary = nullptr;
		}
	}

	pDCompCreateDeviceFunc = nullptr;


	return hr;
}


//*****************************************************************************
// Function:  Cleanup
//
// Purpose:
// Destroys DComp resources and returns the object to the uninitialized state
// (so that it can be reused via a subsequent call to Initialize).
//*****************************************************************************
void CDCompIsolation::Cleanup()
{
	if ( m_pDefaultTargetWrapper != nullptr )
	{
		m_pDefaultTargetWrapper->Cleanup();

		delete m_pDefaultTargetWrapper;
		m_pDefaultTargetWrapper = nullptr;
	}


	if ( m_pDefaultDeviceWrapper != nullptr )
	{
		m_pDefaultDeviceWrapper->Cleanup();

		delete m_pDefaultDeviceWrapper;
		m_pDefaultDeviceWrapper = nullptr;
	}


	if ( m_hDCompLibrary != nullptr )
	{
		FreeLibrary( m_hDCompLibrary );
		m_hDCompLibrary = nullptr;
	}


	if ( m_pLogger != nullptr )
	{
		m_pLogger->Release();
		m_pLogger = nullptr;
	}
}


//*****************************************************************************
// Function:  SetTargetWindow
//
// Purpose:
// Specifies the target window to which DComp will direct the output of its
// composition.
//*****************************************************************************
HRESULT CDCompIsolation::SetTargetWindow
(
	__in HWND hTargetWindow
)
{
	//
	// Validate members and state.
	//

	if ( m_pDefaultDeviceWrapper == nullptr )
	{
		LogError
		(
			ERROR_TYPE_HRESULT,
			E_INVALIDARG,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"Default device wrapper must have been initialized via a successful call to Initialize before calling this method."
		);

		return E_INVALIDARG;
	}


	//
	// Hand off to the other specialization.
	//

	return SetTargetWindow
	(
		m_pDefaultDeviceWrapper,
		hTargetWindow
	);
}


//*****************************************************************************
// Function:  SetTargetWindow
//
// Purpose:
// Specifies the target window to which DComp will direct the output of its
// composition.
//*****************************************************************************
HRESULT CDCompIsolation::SetTargetWindow
(
	__in CDCompDeviceWrapper * pDeviceWrapper,
	__in HWND hTargetWindow
)
{
	//
	// Initialize locals.
	//

	HRESULT hr = E_FAIL;


	//
	// Validate parameters.
	//

	if ( pDeviceWrapper == nullptr )
	{
		hr = E_INVALIDARG;
		LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"pDeviceWrapper must be non-NULL."
		);

		goto Cleanup;
	}

	if ( hTargetWindow == nullptr )
	{
		hr = E_INVALIDARG;
		LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"hTargetWindow must be non-NULL."
		);

		goto Cleanup;
	}


	//
	// Free existing default target if needed.
	//

	if ( m_pDefaultTargetWrapper != nullptr )
	{
		m_pDefaultTargetWrapper->Cleanup();

		delete m_pDefaultTargetWrapper;
		m_pDefaultTargetWrapper = nullptr;
	}


	//
	// Create a default target.
	//

	assert( m_pDefaultTargetWrapper == nullptr );
	m_pDefaultTargetWrapper = new (std::nothrow) CDCompTargetWrapper
	(
		*m_pDefaultDeviceWrapper
	);

	if ( m_pDefaultTargetWrapper == nullptr )
	{
		hr = E_OUTOFMEMORY;
		LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"Failed to allocate default target wrapper."
		);

		goto Cleanup;
	};


	//
	// Initialize the default target.
	//

	hr = m_pDefaultTargetWrapper->Initialize
	(
		hTargetWindow,
		TRUE
	);

	if ( FAILED( hr ) )
	{
		LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"Initialize on default target wrapper failed."
		);

		delete m_pDefaultTargetWrapper;
		m_pDefaultTargetWrapper = nullptr;

		goto Cleanup;
	};


	hr = S_OK;


Cleanup:
	return hr;
}


//*****************************************************************************
// Function:  SetSourceSwapChain
//
// Purpose:
// Sets the specified swap chain as the content source for the DComp visual.
//*****************************************************************************
HRESULT CDCompIsolation::SetSourceSwapChain
(
	__in IDXGISwapChain * pSwapChain,
	bool bScaleToOutputWindow
)
{
	//
	// Validate members and state.
	//

	if
	(
		m_pDefaultTargetWrapper == nullptr ||
		m_pDefaultTargetWrapper->GetTarget() == nullptr
	)
	{
		LogError
		(
			ERROR_TYPE_HRESULT,
			E_INVALIDARG,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"Target must have been created via a successful call to SetTargetWindow before calling this method."
		);

		return E_INVALIDARG;
	}


	//
	// Hand off to the other specialization.
	//

	return SetSourceSwapChain
	(
		m_pDefaultTargetWrapper,
		pSwapChain,
		bScaleToOutputWindow
	);
}


HRESULT CDCompIsolation::SetSourceSwapChain
(
	__in CDCompTargetWrapper * pTargetWrapper,
	__in IDXGISwapChain * pSwapChain,
	bool bScaleToOutputWindow
)
{
	//
	// Initialize locals.
	//

	HRESULT hr = E_FAIL;


	//
	// Validate parameters.
	//

	if ( pTargetWrapper == nullptr )
	{
		hr = E_INVALIDARG;
		LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"pTargetWrapper must be non-NULL."
		);

		goto Cleanup;
	};


	if ( pSwapChain == nullptr )
	{
		hr = E_INVALIDARG;
		LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"pSwapChain must be non-NULL."
		);

		goto Cleanup;
	}


	//
	// Set the source swap chain on the target.
	//

	hr = pTargetWrapper->SetSourceSwapChain
	(
		pSwapChain,
		bScaleToOutputWindow
	);

	if ( FAILED( hr ) )
	{
		LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"SetSourceSwapChain on default target wrapper failed."
		);

		goto Cleanup;
	}


	hr = S_OK;


Cleanup:
	return hr;
}


//*****************************************************************************
// Function:  CreateSurfaceHandle
//
// Purpose:
// Sets the specified swap chain as the content source for the DComp visual.
//*****************************************************************************
HRESULT CDCompIsolation::CreateSurfaceHandle
(
	DWORD DesiredAccess,
	__in_opt SECURITY_ATTRIBUTES * pSecurityAttributes,
	__out HANDLE * phCompositionSurface
)
{
	//
	// Initialize locals.
	//

	HRESULT hr = E_FAIL;

	PDCOMPCREATESURFACEHANDLE pDCompCreateSurfaceHandleFunc = nullptr;


	//
	// Validate and initialize parameters.
	//

	if ( phCompositionSurface == nullptr )
	{
		hr = E_INVALIDARG;
		LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"phCompositionSurface must be non-NULL."
		);

		goto Cleanup;
	}

	*phCompositionSurface = nullptr;


	//
	// Load DComp.dll if needed.
	//

	if ( m_hDCompLibrary == nullptr )
	{
		m_hDCompLibrary = LoadLibraryExW
		(
			L"DComp.dll",
			nullptr,
			0
		);
	}

	if ( m_hDCompLibrary == nullptr )
	{
		// Don't log an error.  If we can't load DComp.dll, it's
		// likely by design (meaning that we're probably running
		// on Windows 7 or earlier).
		hr = E_NOINTERFACE;
		goto Cleanup;
	}


	//
	// Find the DCompositionCreateSurfaceHandle entry point.
	//

	pDCompCreateSurfaceHandleFunc = (PDCOMPCREATESURFACEHANDLE) GetProcAddress
	(
		m_hDCompLibrary,
		"DCompositionCreateSurfaceHandle"
	);

	if ( pDCompCreateSurfaceHandleFunc == nullptr )
	{
		LogError
		(
			ERROR_TYPE_WIN32,
			GetLastError(),
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"GetProcAddess was unable to locate DCompositionCreateSurfaceHandle inside DComp.dll."
		);

		hr = HRESULT_FROM_WIN32( ERROR_PROC_NOT_FOUND );
		goto Cleanup;
	}


	//
	// Create a surface handle.
	//

	hr = (*pDCompCreateSurfaceHandleFunc)
	(
		DesiredAccess,
		pSecurityAttributes,
		phCompositionSurface
	);

	if ( FAILED( hr ) )
	{
		LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"DCompCreateSurfaceHandle failed."
		);

		goto Cleanup;
	}


	hr = S_OK;


Cleanup:
	pDCompCreateSurfaceHandleFunc = nullptr;


	return hr;
}


//*****************************************************************************
// Function:  CreateDCompResources
//
// Purpose:
// Creates a DComp device, a target for the specified output window, and a
// visual, and sets the content of the visual to be the specified swap chain.
// DXGI presentations will be directed to the visual.
//*****************************************************************************
HRESULT CDCompIsolation::CreateDCompResources
(
	__in IDXGISwapChain * pSwapChain,
	__in HWND hTargetWindow,
	bool bScaleToOutputWindow
)
{
	//
	// Initialize locals.
	//

	HRESULT hr = E_FAIL;

	PDCOMPCREATEDEVICE DCOMPCreateDeviceFunc = nullptr;

	DXGI_SWAP_CHAIN_DESC Desc;
	ZeroMemory( &Desc, sizeof( Desc ) );

	IDXGISwapChain1 * pSwapChain1 = nullptr;

	IDCompositionScaleTransform	* pScaleTransform = nullptr;


	//
	// Validate parameters.
	//

	if ( pSwapChain == nullptr )
	{
		hr = E_INVALIDARG;
		LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"pSwapChain must be non-NULL."
		);

		return hr;
	}

	if ( hTargetWindow == nullptr )
	{
		hr = E_INVALIDARG;
		LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"hTargetWindow must be non-NULL."
		);

		return hr;
	}


	//
	// Initialize the object.
	//

	hr = Initialize();

	if ( FAILED( hr ) )
	{
		LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"Initialize failed."
		);

		goto Cleanup;
	}


	//
	// Set the target window.
	//

	hr = SetTargetWindow( hTargetWindow );

	if ( FAILED( hr ) )
	{
		LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"SetTargetWindow failed."
		);

		goto Cleanup;
	}


	//
	// Set the source swap chain.
	//

	hr = SetSourceSwapChain( pSwapChain, bScaleToOutputWindow );

	if ( FAILED( hr ) )
	{
		LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"SetSourceSwapChain failed."
		);

		goto Cleanup;
	}


	hr = S_OK;


Cleanup:
	// If the function was unsuccessful...
	if ( FAILED( hr ) )
	{
		// ...then release member objects, so as to keep this
		// object in an uninitialized state.
		if ( m_pDefaultTargetWrapper != nullptr )
		{
			m_pDefaultTargetWrapper->Cleanup();

			delete m_pDefaultTargetWrapper;
			m_pDefaultTargetWrapper = nullptr;
		}

		if ( m_pDefaultDeviceWrapper != nullptr )
		{
			m_pDefaultDeviceWrapper->Cleanup();

			delete m_pDefaultDeviceWrapper;
			m_pDefaultDeviceWrapper = nullptr;
		}
	}


	return hr;
}


//*****************************************************************************
// Function:  SetLogger
//
// Purpose:
// Attaches a logger to this object.
//
// This call is optional.
//*****************************************************************************
void CDCompIsolation::SetLogger
(
	__in_opt ISimpleLog * pLogger
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
		pLogger->AddRef();
	};
}


//*****************************************************************************
// Function:  LogError
//
// Purpose:
// If a logger has been set with the SetLogger method, this function has the
// logger issue an error.
//*****************************************************************************
void CDCompIsolation::LogError
(
	ERROR_TYPE ErrorType,
	UINT ErrorCode,
	__in const WCHAR * SourceFileName,
	ULONG SourceFileLine,
	__in const CHAR * SourceFunction,
	__in const WCHAR * ErrorDescription
)
{
	//
	// Log the error.
	//

	if ( m_pLogger != NULL )
	{
		m_pLogger->LogError
		(
			ErrorType,
			ErrorCode,
			ErrorDescription,
			SourceFileName,
			SourceFileLine,
			SourceFunction
		);
	};
};


//*****************************************************************************
// Function:  LogMessage
//
// Purpose:
// If a logger has been set with the SetLogger method, this function has the
// logger issue a message.
//*****************************************************************************
void CDCompIsolation::LogMessage
(
	__in const WCHAR * szFormat,
	...
)
{
	//
	// Format the message.
	//

	WCHAR szBuffer[ 4096 ] = L"";
	va_list args;

	va_start( args, szFormat );
	StringCchVPrintfW
	(
		szBuffer,
		ARRAYSIZE( szBuffer ),
		szFormat,
		args
	);
	va_end( args );


	//
	// Log the message.
	//

	if ( m_pLogger != NULL )
	{
		m_pLogger->LogMessage
		(
			MESSAGE_TYPE_INFORMATIONAL,
			szBuffer
		);
	};
};


//*****************************************************************************
// Function:  CDCompDeviceWrapper
//
// Purpose:
// Constructor.
//*****************************************************************************
CDCompDeviceWrapper::CDCompDeviceWrapper
(
	__in CDCompIsolation & Parent
)
:
	m_pParent( &Parent ),
	m_hDCompLibrary( nullptr ),
	m_pDCompDevice( nullptr )
{
	assert( m_pParent != nullptr );
}


//*****************************************************************************
// Function:  ~CDCompDeviceWrapper
//
// Purpose:
// Destructor.
//*****************************************************************************
CDCompDeviceWrapper::~CDCompDeviceWrapper()
{
	assert( m_pDCompDevice == nullptr );

	assert( m_hDCompLibrary == nullptr );
}


//*****************************************************************************
// Function:  Initialize
//
// Purpose:
// Creates a DirectComposition device.
//*****************************************************************************
HRESULT CDCompDeviceWrapper::Initialize()
{
	//
	// Initialize locals.
	//

	HRESULT hr = E_FAIL;

	PDCOMPCREATEDEVICE pDCompCreateDeviceFunc = nullptr;


	//
	// Load DComp.dll.
	//

	m_hDCompLibrary = LoadLibraryExW
	(
		L"DComp.dll",
		nullptr,
		0
	);

	if ( m_hDCompLibrary == nullptr )
	{
		// Don't log an error.  If we can't load DComp.dll, it's
		// likely by design (meaning that we're probably running
		// on Windows 7 or earlier).
		hr = E_NOINTERFACE;
		goto Cleanup;
	}


	//
	// Find the DCompositionCreateDevice entry point.
	//

	pDCompCreateDeviceFunc = (PDCOMPCREATEDEVICE) GetProcAddress
	(
		m_hDCompLibrary,
		"DCompositionCreateDevice"
	);

	if ( pDCompCreateDeviceFunc == nullptr )
	{
		m_pParent->LogError
		(
			ERROR_TYPE_WIN32,
			GetLastError(),
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"GetProcAddess was unable to locate DCompositionCreateDevice inside DComp.dll."
		);

		hr = HRESULT_FROM_WIN32( ERROR_PROC_NOT_FOUND );
		goto Cleanup;
	}


	//
	// Create a DirectComposition (DComp) device
	//

	assert( m_pDCompDevice == nullptr );
	hr = (*pDCompCreateDeviceFunc)
	(
		nullptr,
		__uuidof( IDCompositionDevice ),
		(void **) &m_pDCompDevice
	 );

	if ( FAILED( hr ) )
	{
		m_pParent->LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"DCompositionCreateDevice failed."
		);

		goto Cleanup;
	}

	if ( m_pDCompDevice == nullptr )
	{
		hr = E_POINTER;
		m_pParent->LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"DCompositionCreateDevice succeeded but returned NULL device pointer."
		);

		goto Cleanup;
	}


	hr = S_OK;


Cleanup:
	// If the function was unsuccessful...
	if ( FAILED( hr ) )
	{
		// ...then release member objects, so as to keep the
		// isolation object in an uninitialized state.
		SAFE_RELEASE( m_pDCompDevice );
		m_pDCompDevice = nullptr;

		if ( m_hDCompLibrary != nullptr )
		{
			FreeLibrary( m_hDCompLibrary );
			m_hDCompLibrary = nullptr;
		}
	}

	pDCompCreateDeviceFunc = nullptr;


	return hr;
}


//*****************************************************************************
// Function:  Cleanup
//
// Purpose:
// Destroys DirectComposition resources and returns the object to the
// uninitialized state (so that it can be reused via a subsequent call to
// Initialize).
//*****************************************************************************
void CDCompDeviceWrapper::Cleanup()
{
	if ( m_pDCompDevice != nullptr )
	{
		m_pDCompDevice->Release();
		m_pDCompDevice = nullptr;
	}

	if ( m_hDCompLibrary != nullptr )
	{
		FreeLibrary( m_hDCompLibrary );
		m_hDCompLibrary = nullptr;
	}
}


//*****************************************************************************
// Function:  GetDevice
//
// Purpose:
// Returns a pointer to the DirectComposition device being wrapped.
//*****************************************************************************
IDCompositionDevice * CDCompDeviceWrapper::GetDevice() const
{
	return m_pDCompDevice;
}


//*****************************************************************************
// Function:  GetParent
//
// Purpose:
// Returns a pointer to the DirectComposition isolation object.
//*****************************************************************************
CDCompIsolation * CDCompDeviceWrapper::GetParent() const
{
	return m_pParent;
}


//*****************************************************************************
// Function:  CreateTargetForWindow
//
// Purpose:
// Creates and initializes a DirectComposition target wrapper object for the
// specified window.
//*****************************************************************************
HRESULT CDCompDeviceWrapper::CreateTargetForWindow
(
	__in HWND hWindow,
	__out CDCompTargetWrapper ** ppTargetWrapper
)
{
	return CreateTargetForWindow
	(
		hWindow,
		TRUE,
		ppTargetWrapper
	);
}


HRESULT CDCompDeviceWrapper::CreateTargetForWindow
(
	__in HWND hTargetWindow,
	BOOL bTopmost,
	__out CDCompTargetWrapper ** ppTargetWrapper
)
{
	//
	// Initialize locals.
	//

	HRESULT hr = E_FAIL;


	//
	// Validate parameters.
	//

	if ( hTargetWindow == nullptr )
	{
		hr = E_INVALIDARG;
		m_pParent->LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"hTargetWindow must be non-NULL."
		);

		goto Cleanup;
	}

	if ( ppTargetWrapper == nullptr )
	{
		hr = E_POINTER;
		m_pParent->LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"ppTargetWrapper must be non-NULL."
		);

		goto Cleanup;
	}


	//
	// Allocate a target wrapper object.
	//

	*ppTargetWrapper = new (std::nothrow) CDCompTargetWrapper( *this );
	if ( *ppTargetWrapper == nullptr )
	{
		m_pParent->LogError
		(
			ERROR_TYPE_HRESULT,
			E_OUTOFMEMORY,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"Failed to allocated DirectComposition target wrapper."
		);

		goto Cleanup;
	}


	//
	// Initialize the target wrapper.
	//

	hr = (*ppTargetWrapper)->Initialize
	(
		hTargetWindow,
		bTopmost
	);

	if ( FAILED( hr ) )
	{
		m_pParent->LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"Initialize on target wrapper failed."
		);

		goto Cleanup;
	}


	hr = S_OK;


Cleanup:
	if ( FAILED( hr ) )
	{
		if ( *ppTargetWrapper != nullptr )
		{
			delete (*ppTargetWrapper);
			*ppTargetWrapper = nullptr;
		}
	}


	return hr;
}


//*****************************************************************************
// Function:  CDCompTargetWrapper
//
// Purpose:
// Constructor.
//*****************************************************************************
CDCompTargetWrapper::CDCompTargetWrapper
(
	__in CDCompDeviceWrapper & pParentDevice
)
:
	m_pParentDevice( &pParentDevice ),
	m_pDCompTarget( nullptr ),
	m_pDCompRootVisual( nullptr ),
	m_hTargetWindow( nullptr )
{
	assert( m_pParentDevice != nullptr );
}


//*****************************************************************************
// Function:  ~CDCompTargetWrapper
//
// Purpose:
// Destructor.
//*****************************************************************************
CDCompTargetWrapper::~CDCompTargetWrapper()
{
	assert( m_pDCompRootVisual == nullptr );

	assert( m_pDCompTarget == nullptr );

	m_hTargetWindow = nullptr;
}


//*****************************************************************************
// Function:  Initialize
//
// Purpose:
// Initializes the target wrapper.
//*****************************************************************************
HRESULT CDCompTargetWrapper::Initialize
(
	__in HWND hTargetWindow,
	BOOL bTopmost
)
{
	//
	// Initialize locals.
	//

	HRESULT hr = E_FAIL;


	//
	// Create a DComp target for the specified window.
	//

	assert( m_pDCompTarget == nullptr );
	hr = m_pParentDevice->GetDevice()->CreateTargetForHwnd
	(
		hTargetWindow,
		bTopmost,
		&m_pDCompTarget
	);

	if ( FAILED( hr ) )
	{
		m_pParentDevice->GetParent()->LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"CreateTargetForHwnd failed."
		);

		goto Cleanup;
	}

	if ( m_pDCompTarget == nullptr )
	{
		hr = E_POINTER;
		m_pParentDevice->GetParent()->LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"CreateTargetForHwnd succeeded but returned NULL target pointer."
		);

		goto Cleanup;
	}


	m_hTargetWindow = hTargetWindow;


	hr = S_OK;


Cleanup:
	if ( FAILED( hr ) )
	{
		SAFE_RELEASE( m_pDCompTarget );
	}


	return hr;
}


//*****************************************************************************
// Function:  Cleanup
//
// Purpose:
// Cleans up resources held by the target wrapper.
//*****************************************************************************
void CDCompTargetWrapper::Cleanup()
{
	if ( m_pDCompRootVisual != nullptr )
	{
		m_pDCompRootVisual->Release();
		m_pDCompRootVisual = nullptr;
	}


	if ( m_pDCompTarget != nullptr )
	{
		m_pDCompTarget->Release();
		m_pDCompTarget = nullptr;
	}


	m_hTargetWindow = nullptr;
}


//*****************************************************************************
// Function:  GetTarget
//
// Purpose:
// Returns a pointer to the underlying DirectComposition target.
//*****************************************************************************
IDCompositionTarget * CDCompTargetWrapper::GetTarget() const
{
	return m_pDCompTarget;
}


//*****************************************************************************
// Function:  GetTargetWindow
//
// Purpose:
// Returns a handle to the target window.
//*****************************************************************************
HWND CDCompTargetWrapper::GetTargetWindow() const
{
	return m_hTargetWindow;
}


//*****************************************************************************
// Function:  SetSourceSwapChain
//
// Purpose:
// Creates a visual graph like the following:
//   swap chain -> root visual -> target -> HWND
//*****************************************************************************
HRESULT CDCompTargetWrapper::SetSourceSwapChain
(
	__in IDXGISwapChain * pSwapChain,
	bool bScaleToOutputWindow
)
{
	//
	// Initialize locals.
	//

	HRESULT hr = E_FAIL;

	IDXGISwapChain1 * pSwapChain1 = nullptr;

	DXGI_SWAP_CHAIN_DESC SwapChainDescriptor;
	ZeroMemory( &SwapChainDescriptor, sizeof( SwapChainDescriptor ) );

	IDCompositionScaleTransform	* pScaleTransform = nullptr;


	//
	// Validate parameters.
	//

	if ( pSwapChain == nullptr )
	{
		hr = E_INVALIDARG;
		m_pParentDevice->GetParent()->LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"pSwapChain must be non-NULL."
		);

		goto Cleanup;
	}


	//
	// Query the swap chain for its IDXGISwapChain1 interface.
	//

	hr = pSwapChain->QueryInterface
	(
		IID_IDXGISwapChain1,
		(void **) &pSwapChain1
	);

	if ( FAILED( hr ) )
	{
		m_pParentDevice->GetParent()->LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"QueryInterface( IID_IDXGISwapChain1 ) failed."
		);

		goto Cleanup;
	}

	if ( pSwapChain1 == nullptr )
	{
		hr = E_POINTER;
		m_pParentDevice->GetParent()->LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"QueryInterface( IID_IDXGISwapChain1 ) succeeded but returned NULL swap chain pointer."
		);

		goto Cleanup;
	}


	//
	// Create a DComp visual if needed.
	//

	if ( m_pDCompRootVisual == nullptr )
	{
		//
		// Create the visual.
		//

		hr = m_pParentDevice->GetDevice()->CreateVisual( &m_pDCompRootVisual );
		if ( FAILED( hr ) )
		{
			m_pParentDevice->GetParent()->LogError
			(
				ERROR_TYPE_HRESULT,
				hr,
				__FILEW__,
				__LINE__,
				__FUNCTION__,
				L"CreateVisual failed."
			);

			goto Cleanup;
		}

		if ( m_pDCompRootVisual == nullptr )
		{
			hr = E_POINTER;
			m_pParentDevice->GetParent()->LogError
			(
				ERROR_TYPE_HRESULT,
				hr,
				__FILEW__,
				__LINE__,
				__FUNCTION__,
				L"CreateVisual succeeded but returned NULL visual pointer."
			);

			hr = E_POINTER;
			goto Cleanup;
		}


		//
		// Make the visual the root of the DComp visual tree.
		//

		hr = m_pDCompTarget->SetRoot( m_pDCompRootVisual );
		if ( FAILED( hr ) )
		{
			m_pParentDevice->GetParent()->LogError
			(
				ERROR_TYPE_HRESULT,
				hr,
				__FILEW__,
				__LINE__,
				__FUNCTION__,
				L"SetRoot on DirectComposition target failed."
			);

			goto Cleanup;
		}
	}


	//
	// Scale down the visual to fit the final composition
	// window if requested.
	//

	if ( bScaleToOutputWindow == true )
	{
		//
		// Create a scale transform.
		//

		hr = m_pParentDevice->GetDevice()->CreateScaleTransform( &pScaleTransform );
		if ( FAILED( hr ) )
		{
			m_pParentDevice->GetParent()->LogError
			(
				ERROR_TYPE_HRESULT,
				hr,
				__FILEW__,
				__LINE__,
				__FUNCTION__,
				L"CreateScaleTransform failed."
			);

			goto Cleanup;
		}

		if ( pScaleTransform == nullptr )
		{
			hr = E_POINTER;
			m_pParentDevice->GetParent()->LogError
			(
				ERROR_TYPE_HRESULT,
				hr,
				__FILEW__,
				__LINE__,
				__FUNCTION__,
				L"CreateScaleTransform succeeded but returned NULL transform pointer."
			);

			goto Cleanup;
		}


		//
		// Get the swap chain descriptor (from which we can get the
		// swap chain width and height).
		//

		pSwapChain1->GetDesc( &SwapChainDescriptor );


		//
		// Get the rectangle for the target window.
		//

		RECT TargetWindowRectangle;
		ZeroMemory( &TargetWindowRectangle, sizeof( TargetWindowRectangle ) );

		GetWindowRect
		(
			m_hTargetWindow,
			&TargetWindowRectangle
		);


		//
		// Set parameters on the scale transform.
		//

		hr = pScaleTransform->SetScaleX( (float) ( TargetWindowRectangle.right - TargetWindowRectangle.left ) / (float) SwapChainDescriptor.BufferDesc.Width );
		hr = pScaleTransform->SetScaleY( (float) ( TargetWindowRectangle.bottom - TargetWindowRectangle.top ) / (float) SwapChainDescriptor.BufferDesc.Height );

		hr = pScaleTransform->SetCenterX( 0.0 );
		hr = pScaleTransform->SetCenterX( 0.0 );


		//
		// Set the scale transformation on the root visual.
		//

		assert( m_pDCompRootVisual != nullptr );
		hr = m_pDCompRootVisual->SetTransform( pScaleTransform );
	}


	//
	// Set the swap chain as the content source for the root visual.
	//

	assert( m_pDCompRootVisual != nullptr );
	hr = m_pDCompRootVisual->SetContent( pSwapChain1 );
	if ( FAILED( hr ) )
	{
		m_pParentDevice->GetParent()->LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"SetContent failed."
		);

		goto Cleanup;
	}


	//
	// Have DComp commit the batch.
	//

	hr = m_pParentDevice->GetDevice()->Commit();

	if ( FAILED( hr ) )
	{
		m_pParentDevice->GetParent()->LogError
		(
			ERROR_TYPE_HRESULT,
			hr,
			__FILEW__,
			__LINE__,
			__FUNCTION__,
			L"Commit on DirectComposition device failed."
		);

		goto Cleanup;
	}


	//
	// Work around an early DComp bug where Commit is deferred
	// to the next VSync when DWM wakes up.
	//

	Sleep( 250 );


	hr = S_OK;


Cleanup:
	SAFE_RELEASE( pScaleTransform );
	pScaleTransform = nullptr;

	SAFE_RELEASE( pSwapChain1 );
	pSwapChain1 = nullptr;

	return hr;
}
