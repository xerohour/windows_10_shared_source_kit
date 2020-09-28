//////////////////////////////////////////////////////////////////////
// File:  CustomTestApp.inl
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of custom test app object.
//
// History:
// 10 Mar 2006  SEdmison  Factored from other tests into a shared lib.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Constants
//
/////////////////////////////////////////////////////////////////

EXTERN_C const GUID FAR GUID_VIDEO_SUBGROUP;
EXTERN_C const GUID FAR GUID_VIDEO_POWERDOWN_TIMEOUT;


/////////////////////////////////////////////////////////////////
//
// Implementation
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CMyTestApp
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
template < typename ParentClass >
CCustomTestApp< ParentClass >::CCustomTestApp< ParentClass >()
:
	m_OriginalForegroundWindowLockTimeout( 0 ),
	m_bCompositionUIValueFound( FALSE ),
	m_CompositionUIValue( 0 )
{
};


///////////////////////////////////////////////////////
// Function:  Setup
//
// Purpose:
// Performs app-wide initialization.
///////////////////////////////////////////////////////
template < typename ParentClass >
bool CCustomTestApp< ParentClass >::Setup()
{
	//
	// Initialize locals.
	//

	bool bOverallResult = false;

	//
	// Turn off DWM composition UI, which otherwise would jump up
	// at inopportune times, thus stealing the foreground from the
	// windows created by our tests.
	//

    // warning 28931: Unused assignment of variable hToggleCompositionUIResult
	HRESULT hToggleCompositionUIResult = ToggleDWMCompositionUI( FALSE );

    if (FAILED(hToggleCompositionUIResult))
    {
		WriteToLog
		(
			_T( "Setup:  ToggleDWMCompositionUI( FALSE ) failed." )
		);

		goto Cleanup;
    }

	//
	// Enable the system shutdown privilege.
	//

	HRESULT hEnableShutdownResult = EnablePrivilege( SE_SHUTDOWN_NAME );

	if ( FAILED( hEnableShutdownResult ) )
	{
		WriteToLog
		(
			_T( "Setup:  EnablePrivilege( SE_SHUTDOWN_NAME ) failed." )
		);

		goto Cleanup;
	};


	//
	// Retrieve the original foreground lock timeout.
	//

	SystemParametersInfo
	(
		SPI_GETFOREGROUNDLOCKTIMEOUT,
		0,
		&m_OriginalForegroundWindowLockTimeout,
		0
	);


	//
	// Override that timeout with zero to unlock the foreground
	// window (meaning that we can freely SetForegroundWindow).
	//

	BOOL bOverrideForegroundTimeoutResult = SystemParametersInfo
	(
		SPI_SETFOREGROUNDLOCKTIMEOUT,
		0,
		NULL,
		SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE
	);

	if ( bOverrideForegroundTimeoutResult != TRUE )
	{
		DWORD OverrideForegroundTimeoutResult = GetLastError();

		WriteToLogF
		(
			_T( "Setup:  SystemParametersInfo( SPI_SETFOREGROUNDLOCKTIMEOUT ) ")
			_T( "failed with error %u.  This may mean that the test is" )
			_T( "occluded by another window, and may cause failures." ),
			OverrideForegroundTimeoutResult
		);

		//goto Cleanup;
	};


	bOverallResult = true;


Cleanup:
	return bOverallResult;
};


///////////////////////////////////////////////////////
// Function:  Cleanup
//
// Purpose:
// Application-wide cleanup.
///////////////////////////////////////////////////////
template < typename ParentClass >
void CCustomTestApp< ParentClass >::Cleanup()
{
	//
	// Restore the original foreground window lock timeout.
	//

	SystemParametersInfo
	(
		SPI_SETFOREGROUNDLOCKTIMEOUT,
		0,
		(PVOID) (DWORD_PTR) m_OriginalForegroundWindowLockTimeout,
		SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE
	);


	//
	// Restore prior DWM composition UI setting.
	//

	/* HRESULT hToggleCompositionUIResult = */ ToggleDWMCompositionUI( TRUE );
};


///////////////////////////////////////////////////////
// Function:  EnablePrivilege
//
// Purpose:
// Asserts the specified security privilege for the
// process.
///////////////////////////////////////////////////////
template < typename ParentClass >
HRESULT CCustomTestApp< ParentClass >::EnablePrivilege
(
	IN LPCTSTR PrivilegeName
)
{
	//
	// Validate parameters.
	//

	if ( PrivilegeName == NULL )
	{
		return E_INVALIDARG;
	};


	//
	// Open the process token.
	//

	HANDLE hToken = NULL;
	BOOL bOpenTokenResult = OpenProcessToken
	(
		GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
		&hToken
	);

	if
	(
		bOpenTokenResult != TRUE ||
		hToken == NULL ||
		hToken == INVALID_HANDLE_VALUE
	)
	{
		return HRESULT_FROM_WIN32( GetLastError() );
	};


	TOKEN_PRIVILEGES tkp; // pointer to token structure
	ZeroMemory( &tkp, sizeof( tkp ) );


	//
	// Lookup the shutdown privilege.
	//

	LookupPrivilegeValue
	(
		NULL,
		PrivilegeName,
		&tkp.Privileges[0].Luid
	);


	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;   


	//
	// Enable the privilege for the process.
	//

	BOOL bAdjustPrivilegesResult = AdjustTokenPrivileges
	(
		hToken,
		FALSE,
		&tkp,
		sizeof( tkp ),
		(PTOKEN_PRIVILEGES) NULL,
		0
	);

	if ( bAdjustPrivilegesResult != TRUE )
	{
		DWORD AdjustPrivilegesError = GetLastError();

		if ( AdjustPrivilegesError != ERROR_SUCCESS )
		{
			return HRESULT_FROM_WIN32( AdjustPrivilegesError );
		};
	};


	return S_OK;
};


///////////////////////////////////////////////////////
// Function:  ToggleDWMCompositionUI
//
// Purpose:
// Disables or restores DWM composition UI via a
// registry hack.
///////////////////////////////////////////////////////
template < typename ParentClass >
HRESULT CCustomTestApp< ParentClass >::ToggleDWMCompositionUI
(
	IN BOOL bRestore
)
{
	HRESULT hOverallResult = E_FAIL;


	//
	// Open or create the parent registry key.
	//

	HKEY hParentKey = NULL;
	LONG CreateParentKeyResult = RegCreateKeyEx
	(
		HKEY_CURRENT_USER,
		_T( "Software\\Microsoft\\Windows\\DWM" ),
		0,
		NULL,
		0,
		KEY_READ | KEY_WRITE,
		NULL,
		&hParentKey,
		NULL
	);

	if ( CreateParentKeyResult != ERROR_SUCCESS )
	{
		hOverallResult = HRESULT_FROM_WIN32( CreateParentKeyResult );
		goto Cleanup;
	};


	//
	// Set the value thus suppressing the behavior, or restore
	// the system to its prior state.
	//

	// If we're restoring the state of the system...
	if ( bRestore == TRUE )
	{
		// ...and the value wasn't set originally...
		if ( m_bCompositionUIValueFound == FALSE )
		{
			// ...then delete it...
/*
			WriteToLog
			(
				_T( "ToggleDWMCompositionUI:  Removing SuppressDisableCompositionUI value from the registry." )
			);
*/

			LONG DeleteValueResult = RegDeleteValue
			(
				hParentKey,
				_T( "SuppressDisableCompositionUI" )
			);

			if ( DeleteValueResult != ERROR_SUCCESS )
			{
				hOverallResult = HRESULT_FROM_WIN32( DeleteValueResult );
				goto Cleanup;
			};
		}
		// ...otherwise, it was set, ...
		else
		{
			// ...so restore it to its original value...
/*
			WriteToLogF
			(
				_T( "ToggleDWMCompositionUI:  Restoring SuppressDisableCompositionUI value to %u in the registry." ),
				m_CompositionUIValue
			);
*/

			LONG SetValueResult = RegSetValueEx
			(
				hParentKey,
				_T( "SuppressDisableCompositionUI" ),
				0,
				REG_DWORD,
				(LPBYTE) &m_CompositionUIValue,
				sizeof( m_CompositionUIValue )
			);

			if ( SetValueResult != ERROR_SUCCESS )
			{
				hOverallResult = HRESULT_FROM_WIN32( SetValueResult );
				goto Cleanup;
			};
		};
	}
	// ...otherwise, if we're overriding the state of the system...
	else
	{
		// ...then query the value...
		DWORD ValueType = REG_DWORD;
		DWORD ValueSize = sizeof( DWORD );
		LONG QueryValueResult = RegQueryValueEx
		(
			hParentKey,
			_T( "SuppressDisableCompositionUI" ),
			NULL,
			&ValueType,
			(LPBYTE) &m_CompositionUIValue,
			&ValueSize
		);

		// ...and if we found it...
		if ( QueryValueResult == ERROR_SUCCESS )
		{
			// ...then sanity-check it...
			assert( ValueType == REG_DWORD );
			assert( ValueSize == sizeof( DWORD ) );

			// ...and stash it for restoration at cleanup time...
			m_bCompositionUIValueFound = TRUE;
		}
		else
		{
			// ...otherwise, make a note that we didn't find it.
			m_bCompositionUIValueFound = FALSE;
		};


		//
		// Override the value.
		//

/*
		WriteToLog
		(
			_T( "ToggleDWMCompositionUI:  Overriding SuppressDisableCompositionUI value in the registry." )
		);
*/

		DWORD OverrideData = 1;
		LONG SetValueResult = RegSetValueEx
		(
			hParentKey,
			_T( "SuppressDisableCompositionUI" ),
			0,
			REG_DWORD,
			(LPBYTE) &OverrideData,
			sizeof( OverrideData )
		);

		if ( SetValueResult != ERROR_SUCCESS )
		{
			hOverallResult = HRESULT_FROM_WIN32( SetValueResult );
			goto Cleanup;
		};
	};

			
	//
	// If we made it this far, then we're good to go.
	//

	hOverallResult = S_OK;


Cleanup:
	//
	// Close the parent key.
	//

	if ( hParentKey != NULL )
	{
		RegCloseKey( hParentKey );
		hParentKey = NULL;
	};

	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  DetectDriverModelForDevice
//
// Purpose:
// Determines whether the specified device supports the
// Longhorn device driver model (LDDM).
//
// Returns:
// DRIVERMODEL_XPDM  for Windows XP devices
// DRIVERMODEL_LDDM  for Longhorn (LDDM) devices
///////////////////////////////////////////////////////
template < typename ParentClass >
DRIVERMODEL CCustomTestApp< ParentClass >::DetectDriverModelForDevice
(
	IN LPCWSTR pDeviceName
)
{
	DRIVERMODEL DriverModel = DRIVERMODEL_XPDM;

	HMODULE hGDI = NULL;

	HDC hDC = NULL;
	if ( pDeviceName != NULL )
	{
		hDC = CreateDCW
		(
			pDeviceName,
			NULL,
			NULL,
			NULL
		);
	}
	else
	{
		hDC = CreateDCW
		(
			L"DISPLAY",
			NULL,
			NULL,
			NULL
		);
	};

	if ( hDC == NULL )
	{
		goto Cleanup;
	};


	//
	// Load GDI.
	//

	hGDI = LoadLibrary(_T("gdi32.dll"));
	if ( hGDI == NULL )
	{
		goto Cleanup;
	};


	//
	// Get pointers to our GDI kernel-mode thunks.
	//

	PFND3DKMT_OPENADAPTERFROMHDC pfnOsThunkDDIOpenAdapterFromHdc = (PFND3DKMT_OPENADAPTERFROMHDC)GetProcAddress
	(
		hGDI,
		"D3DKMTOpenAdapterFromHdc"
	);
	if (pfnOsThunkDDIOpenAdapterFromHdc == NULL)
	{
		goto Cleanup;
	};


	PFND3DKMT_CLOSEADAPTER pfnOsThunkDDICloseAdapter = (PFND3DKMT_CLOSEADAPTER)GetProcAddress
	(
		hGDI,
		"D3DKMTCloseAdapter"
	);
	if (pfnOsThunkDDICloseAdapter == NULL)
	{
		goto Cleanup;
	};


	//
	// Open the adapter.
	//

	D3DKMT_OPENADAPTERFROMHDC OpenAdapterFromHdc;
	OpenAdapterFromHdc.hDc = hDC;
	if
	(
		(*pfnOsThunkDDIOpenAdapterFromHdc)(&OpenAdapterFromHdc) != STATUS_SUCCESS
	)
	{
		goto Cleanup;
	};


	//
	// Close the adapter.
	//

	D3DKMT_CLOSEADAPTER CloseAdapter;

	CloseAdapter.hAdapter = OpenAdapterFromHdc.hAdapter;
	(*pfnOsThunkDDICloseAdapter)(&CloseAdapter);


	DriverModel = DRIVERMODEL_LDDM;


Cleanup:
	if ( hDC != NULL )
	{
		DeleteDC( hDC );
		hDC = NULL;
	};

	if ( hGDI != NULL )
	{
		FreeLibrary( hGDI );
		hGDI = NULL;
	};


	return DriverModel;
};
