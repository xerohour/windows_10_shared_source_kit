//////////////////////////////////////////////////////////////////////
// File:  GDIAdapterTopology.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a helper object that keeps track of the GDI
// adapter and output topology.
//
// History:
// 07 Jan 2007  SEdmison  Created.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

#include "GDIAdapterTopology.hpp"

#include "AdapterUtil.h"


/////////////////////////////////////////////////////////////////
//
// Macros
//
/////////////////////////////////////////////////////////////////
#define LOG_MESSAGE( MessageType, MessageText ) \
	if ( m_pLogger != NULL ) \
	{ \
		m_pLogger->LogMessage \
		( \
			MessageType, \
			MessageText \
		); \
	}


#define LOG_ERROR( ErrorType, ErrorCode, ErrorDescription ) \
	if ( m_pLogger != NULL ) \
	{ \
		m_pLogger->LogError \
		( \
			ErrorType, \
			ErrorCode, \
			ErrorDescription, \
			__FILEW__, \
			__LINE__, \
			__FUNCTION__ \
		); \
	} \


/////////////////////////////////////////////////////////////////
//
// Implementation
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CGDIAdapterTopology
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CGDIAdapterTopology::CGDIAdapterTopology()
:
	m_pLogger( NULL ),
	m_cDevices( 0 )
{
	ZeroMemory( m_DeviceDescriptors, sizeof( m_DeviceDescriptors ) );

	ZeroMemory( m_DeviceOrdinalForDeviceSlot, sizeof( m_DeviceOrdinalForDeviceSlot ) );
	ZeroMemory( m_DeviceSlotForMonitorSlot, sizeof( m_DeviceSlotForMonitorSlot ) );


	for
	(
		UINT iCurrentMonitorSlot = 0;
		iCurrentMonitorSlot < MONITOR_SLOTS;
		++iCurrentMonitorSlot
	)
	{
		m_DeviceSlotForMonitorSlot[ iCurrentMonitorSlot ] = 0xFFFFFFFF;
	};

	ZeroMemory( &m_DeviceModes, sizeof( m_DeviceModes ) );
};


///////////////////////////////////////////////////////
// Function:  ~CGDIAdapterTopology
//
// Purpose:
// Destructor.
///////////////////////////////////////////////////////
CGDIAdapterTopology::~CGDIAdapterTopology()
{
	AttachLogger( NULL );
};


///////////////////////////////////////////////////////
// Function:  AttachLogger
//
// Purpose:
// Attaches the object to a logger.
///////////////////////////////////////////////////////
void CGDIAdapterTopology::AttachLogger
(
	__in ISimpleLog * pLogger
)
{
	if ( m_pLogger != NULL )
	{
		m_pLogger->Release();
		m_pLogger = NULL;
	};

	if ( pLogger != NULL )
	{
		m_pLogger = pLogger;
		m_pLogger->AddRef();
	};
};


///////////////////////////////////////////////////////
// Function:  EnumerateAdaptersAndOutputs
//
// Purpose:
// Enumerates GDI adapters and outputs.
///////////////////////////////////////////////////////
HRESULT CGDIAdapterTopology::EnumerateAdaptersAndOutputs
(
	BOOL bIncludeDetachedDevices
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	DISPLAY_DEVICEW TemporaryDeviceList[ 30 ];
	ZeroMemory( TemporaryDeviceList, sizeof( TemporaryDeviceList ) );

	DISPLAY_DEVICEW TemporaryMonitorList[ 10 ];
	ZeroMemory( TemporaryMonitorList, sizeof( TemporaryMonitorList ) );

	UINT cDevicesFound = 0;

	UINT PrimaryDeviceSlot = 0xFFFFFFFF;


	//
	// Initialize members.
	//

	m_cDevices = 0;


	for
	(
		UINT iCurrentAttempt = 0;
		iCurrentAttempt < 10;
		++iCurrentAttempt
	)
	{
		//
		// Enumerate display devices.
		//

		cDevicesFound = 0;
		for
		(
			DWORD CurrentDeviceOrdinal = 0;
			CurrentDeviceOrdinal < 30 &&
				cDevicesFound < ARRAYSIZE( TemporaryDeviceList );
			++CurrentDeviceOrdinal
		)
		{
			TemporaryDeviceList[ cDevicesFound ].cb = sizeof( TemporaryDeviceList[ 0 ] );
			BOOL bEnumerateDeviceResult = EnumDisplayDevicesW
			(
				NULL,
				CurrentDeviceOrdinal,
				&TemporaryDeviceList[ cDevicesFound ],
				0
			);

			if ( bEnumerateDeviceResult != TRUE )
			{
				if ( cDevicesFound == 0 )
				{
					GetFramework()->WriteToLogF
					(
						_T( "CGDIAdapterTopology::EnumerateAdaptersAndOutputs:  EnumDisplayDevicesW failed without having found any devices." )
					);

					goto Cleanup;
				};

				break;
			};

			++cDevicesFound;
		};


		//
		// Find the primary device.
		//
		bool bFoundPrimary = false;

		for
		(
			DWORD iCurrentDevice = 0;
			iCurrentDevice < cDevicesFound;
			++iCurrentDevice
		)
		{
			if
			(
				( TemporaryDeviceList[ iCurrentDevice ].StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE ) != 0
			)
			{
				PrimaryDeviceSlot = iCurrentDevice;
				bFoundPrimary = true;
				break;
			};
		};

		//
		// Sanity-check: Was there a primary device?
		//

		if ( bFoundPrimary == false )
		{
			GetFramework()->WriteToLogF
			(
				L"CGDIAdapterTopology::EnumerateAdaptersAndOutputs:  No primary display device found!"
			);

			goto Cleanup;
		}

		if ( ( TemporaryDeviceList[ PrimaryDeviceSlot ].StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) != 0 )
		{
			break;
		};


		Sleep( 1000 );
	};


	//
	// Sanity-check:  Is the primary device attached to the
	// desktop?
	//

	//assert( ( TemporaryDeviceList[ PrimaryDeviceSlot ].StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) != 0 );
	if ( ( TemporaryDeviceList[ PrimaryDeviceSlot ].StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) == 0 )
	{
		GetFramework()->WriteToLogF
		(
			L"CGDIAdapterTopology::EnumerateAdaptersAndOutputs:  Device %u %s (%s) is the primary device, but is not attached to the desktop!",
			PrimaryDeviceSlot,
			TemporaryDeviceList[ PrimaryDeviceSlot ].DeviceName,
			TemporaryDeviceList[ PrimaryDeviceSlot ].DeviceString
		);

		goto Cleanup;
	};


	//
	// Determine the primary device's current mode.
	//

	DEVMODEW PrimaryDeviceMode;
	ZeroMemory( &PrimaryDeviceMode, sizeof( PrimaryDeviceMode ) );
	PrimaryDeviceMode.dmSize = sizeof( PrimaryDeviceMode );
	PrimaryDeviceMode.dmSpecVersion = DM_SPECVERSION;

	BOOL bGetPrimaryDeviceModeResult = EnumDisplaySettingsExW
	(
		TemporaryDeviceList[ PrimaryDeviceSlot ].DeviceName,
		ENUM_CURRENT_SETTINGS,
		&PrimaryDeviceMode,
		0
	);

	if ( bGetPrimaryDeviceModeResult != TRUE )
	{
		goto Cleanup;
	};


	//
	// Find the left device.
	//

	UINT LeftDeviceSlot = 0xFFFFFFFF;
	for
	(
		DWORD iCurrentDevice = 0;
		iCurrentDevice < cDevicesFound;
		++iCurrentDevice
	)
	{
		if
		(
			( TemporaryDeviceList[ iCurrentDevice ].StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE ) != 0 ||
			( TemporaryDeviceList[ iCurrentDevice ].StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) == 0
		)
		{
			continue;
		};

		DEVMODEW CurrentMode;
		ZeroMemory( &CurrentMode, sizeof( CurrentMode ) );
		CurrentMode.dmSize = sizeof( CurrentMode );
		CurrentMode.dmSpecVersion = DM_SPECVERSION;

		BOOL bGetDeviceModeResult = EnumDisplaySettingsExW
		(
			TemporaryDeviceList[ iCurrentDevice ].DeviceName,
			ENUM_CURRENT_SETTINGS,
			&CurrentMode,
			0
		);

		if ( bGetDeviceModeResult != TRUE )
		{
			continue;
		};

		if
		(
			CurrentMode.dmPosition.x + CurrentMode.dmPelsWidth == PrimaryDeviceMode.dmPosition.x &&
			(
				CurrentMode.dmPosition.y >= PrimaryDeviceMode.dmPosition.y &&
				CurrentMode.dmPosition.y < PrimaryDeviceMode.dmPosition.y + (LONG) PrimaryDeviceMode.dmPelsHeight ||
				PrimaryDeviceMode.dmPosition.y >= CurrentMode.dmPosition.y &&
				PrimaryDeviceMode.dmPosition.y < CurrentMode.dmPosition.y + (LONG) CurrentMode.dmPelsHeight
			)
		)
		{
			LeftDeviceSlot = iCurrentDevice;
			break;
		};
	};


	//
	// Find the right device.
	//

	UINT RightDeviceSlot = 0xFFFFFFFF;
	for
	(
		DWORD iCurrentDevice = 0;
		iCurrentDevice < cDevicesFound;
		++iCurrentDevice
	)
	{
		if
		(
			( TemporaryDeviceList[ iCurrentDevice ].StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE ) != 0 ||
			( TemporaryDeviceList[ iCurrentDevice ].StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) == 0
		)
		{
			continue;
		};

		DEVMODEW CurrentMode;
		ZeroMemory( &CurrentMode, sizeof( CurrentMode ) );
		CurrentMode.dmSize = sizeof( CurrentMode );
		CurrentMode.dmSpecVersion = DM_SPECVERSION;

		BOOL bGetDeviceModeResult = EnumDisplaySettingsExW
		(
			TemporaryDeviceList[ iCurrentDevice ].DeviceName,
			ENUM_CURRENT_SETTINGS,
			&CurrentMode,
			0
		);

		if ( bGetDeviceModeResult != TRUE )
		{
			continue;
		};

		if
		(
			CurrentMode.dmPosition.x == PrimaryDeviceMode.dmPosition.x + PrimaryDeviceMode.dmPelsWidth &&
			(
				CurrentMode.dmPosition.y >= PrimaryDeviceMode.dmPosition.y &&
				CurrentMode.dmPosition.y < PrimaryDeviceMode.dmPosition.y + (LONG) PrimaryDeviceMode.dmPelsHeight ||
				PrimaryDeviceMode.dmPosition.y >= CurrentMode.dmPosition.y &&
				PrimaryDeviceMode.dmPosition.y < CurrentMode.dmPosition.y + (LONG) CurrentMode.dmPelsHeight
			)
		)
		{
			RightDeviceSlot = iCurrentDevice;
			break;
		};
	};


	//
	// Determine which devices to return to the caller.
	//

	if ( bIncludeDetachedDevices == TRUE )
	{
		m_cDevices = cDevicesFound;
		if ( m_cDevices > ARRAYSIZE( m_DeviceDescriptors ) )
		{
			m_cDevices = ARRAYSIZE( m_DeviceDescriptors );
		};

		for
		(
			UINT iCurrentDevice = 0;
			iCurrentDevice < m_cDevices;
			++iCurrentDevice
		)
		{
			memcpy
			(
				&m_DeviceDescriptors[ iCurrentDevice ],
				&TemporaryDeviceList[ iCurrentDevice ],
				sizeof( m_DeviceDescriptors[ iCurrentDevice ] )
			);

			m_DeviceOrdinalForDeviceSlot[ iCurrentDevice ] = iCurrentDevice;

			if ( PrimaryDeviceSlot != 0xFFFFFFFF )
			{
				m_DeviceSlotForMonitorSlot[ MONITOR_CENTER ] = PrimaryDeviceSlot;
			};

			if ( LeftDeviceSlot != 0xFFFFFFFF )
			{
				m_DeviceSlotForMonitorSlot[ MONITOR_LEFT ] = LeftDeviceSlot;
			};

			if ( RightDeviceSlot != 0xFFFFFFFF )
			{
				m_DeviceSlotForMonitorSlot[ MONITOR_RIGHT ] = RightDeviceSlot;
			};
		};
	}
	else
	{
		memcpy
		(
			&m_DeviceDescriptors[ m_cDevices ],
			&TemporaryDeviceList[ PrimaryDeviceSlot ],
			sizeof( m_DeviceDescriptors[ 0 ] )
		);

		m_DeviceOrdinalForDeviceSlot[ m_cDevices ] = PrimaryDeviceSlot;
		PrimaryDeviceSlot = 0;
		m_DeviceSlotForMonitorSlot[ MONITOR_CENTER ] = 0;

		m_cDevices = 1;


		if ( LeftDeviceSlot != 0xFFFFFFFF )
		{
			memcpy
			(
				&m_DeviceDescriptors[ m_cDevices ],
				&TemporaryDeviceList[ LeftDeviceSlot ],
				sizeof( m_DeviceDescriptors[ 0 ] )
			);

			m_DeviceOrdinalForDeviceSlot[ m_cDevices ] = LeftDeviceSlot;
			LeftDeviceSlot = m_cDevices;
			m_DeviceSlotForMonitorSlot[ MONITOR_LEFT ] = m_cDevices;

			++m_cDevices;
		};


		if ( RightDeviceSlot != 0xFFFFFFFF )
		{
			memcpy
			(
				&m_DeviceDescriptors[ m_cDevices ],
				&TemporaryDeviceList[ RightDeviceSlot ],
				sizeof( m_DeviceDescriptors[ m_cDevices ] )
			);

			m_DeviceOrdinalForDeviceSlot[ m_cDevices ] = RightDeviceSlot;
			RightDeviceSlot = m_cDevices;
			m_DeviceSlotForMonitorSlot[ MONITOR_RIGHT ] = m_cDevices;

			++m_cDevices;
		};
	};


	//
	// Now capture the mode list for the set of devices that
	// we have chosen to return.
	//

	for
	(
		DWORD iCurrentDevice = 0;
		iCurrentDevice < m_cDevices;
		++iCurrentDevice
	)
	{
		// Attempt to determine the monitor's current mode...
		m_DeviceModes[ iCurrentDevice ].dmSize = sizeof( m_DeviceModes[ 0 ] );
		m_DeviceModes[ iCurrentDevice ].dmSpecVersion = DM_SPECVERSION;

		BOOL bGetMonitorModeResult = EnumDisplaySettingsExW
		(
			m_DeviceDescriptors[ iCurrentDevice ].DeviceName,
			ENUM_CURRENT_SETTINGS,
			&m_DeviceModes[ iCurrentDevice ],
			0
		);

		// ...and if we can't...
		assert( bGetMonitorModeResult == TRUE );
		if ( bGetMonitorModeResult != TRUE )
		{
			// ...then find out what went wrong...
			DWORD GetMonitorModeError = GetLastError();

			if ( GetMonitorModeError != ERROR_SUCCESS )
			{
				hOverallResult = HRESULT_FROM_WIN32( GetMonitorModeError );
			};

			// ...and bail.
			goto Cleanup;
		};

		assert( m_DeviceModes[ iCurrentDevice ].dmPelsWidth > 0 );
		assert( m_DeviceModes[ iCurrentDevice ].dmPelsHeight > 0 );
	};


	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  CauseModeChange
//
// Purpose:
// Causes a mode change for all devices attached to
// the desktop (that is, all adapters onto which the
// desktop extends).
///////////////////////////////////////////////////////
HRESULT CGDIAdapterTopology::CauseModeChange
(
	IN UINT Monitor,
	IN DWORD Width,
	IN DWORD Height,
	IN DWORD BitDepth,
	IN DWORD RefreshRate,
	IN DWORD Orientation,
	IN DWORD Flags
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = S_OK;
	DevX::GRFX::AdapterUtil * pAdapterUtil = DevX::GRFX::AdapterUtil::CreateAdapterUtil();
	DWORD FixedRefreshRate = 0;


	//
	// Walk the list of adapters that we built previously,
	// causing a mode change on each.
	//

	for
	(
		DWORD iCurrentAdapter = 0;
		iCurrentAdapter < m_cDevices;
		++iCurrentAdapter
	)
	{
		//
		// Skip over empty or detached adapter slots.
		//

		if
		(
			iCurrentAdapter > 0 &&
			( m_DeviceDescriptors[ iCurrentAdapter ].StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) == 0
		)
		{
			continue;
		};


		//
		// If the caller asked for a mode change only on a specific
		// adapter, then skip the others.
		//

		if
		(
			Monitor != MONITOR_UNSPECIFIED &&
			m_DeviceSlotForMonitorSlot[ Monitor ] != iCurrentAdapter
		)
		{
			continue;
		};


		//
		// If this adapter is BDD/BDA, modify the refresh rate to be
		// 64Hz (because that's the only supported refresh rate on
		// MSBDD/MSBDA).
		//
        
		bool IsBddOrBda = false;

		if ( pAdapterUtil != NULL )
		{
			DevX::GRFX::AdapterDescriptor * pAdapterDesc = pAdapterUtil->GetAdapterDrivingDisplayDevice( m_DeviceDescriptors[iCurrentAdapter] );
			if ( pAdapterDesc != NULL )
			{
				pAdapterDesc->IsBDAorBDD( IsBddOrBda );
			}
		}

		if ( IsBddOrBda )
		{
			// Fix the refresh rate to 64Hz on BDD/BDA adapters
			FixedRefreshRate = 64;
		}
		else
		{
			// On all other adapters, leave the refresh rate unchanged from the requested one
			FixedRefreshRate = RefreshRate;
		};


		//
		// Attempt to change the current monitor to the specified mode.
		//

		DEVMODEW DeviceMode;
		ZeroMemory( &DeviceMode, sizeof( DeviceMode ) );
		DeviceMode.dmSpecVersion = DM_SPECVERSION;
		DeviceMode.dmSize = sizeof( DeviceMode );

		if ( Width == 0 )
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"CauseModeChange:  Getting current mode."
			);

			DEVMODEW CurrentDeviceMode;
			ZeroMemory( &CurrentDeviceMode, sizeof( CurrentDeviceMode ) );
			CurrentDeviceMode.dmSpecVersion = DM_SPECVERSION;
			CurrentDeviceMode.dmSize = sizeof( CurrentDeviceMode );

			BOOL bGetCurrentDeviceModeResult = EnumDisplaySettingsW
			(
				m_DeviceDescriptors[ iCurrentAdapter ].DeviceName,
				ENUM_CURRENT_SETTINGS,
				&CurrentDeviceMode
			);

			if ( bGetCurrentDeviceModeResult != TRUE )
			{
				DWORD GetCurrentDeviceModeError = GetLastError();
				LOG_ERROR
				(
					ERROR_TYPE_WIN32,
					GetCurrentDeviceModeError,
					L"EnumDisplaySettings failed."
				);

				if ( GetCurrentDeviceModeError != ERROR_SUCCESS )
				{
					hOverallResult = HRESULT_FROM_WIN32( GetCurrentDeviceModeError );
				}
				else
				{
					hOverallResult = E_FAIL;
				};

				goto Cleanup;
			};


			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"CauseModeChange:  Enumerating modes in search of a mode sufficiently different from the current mode."
			);

			bool bFoundSuitableMode = false;
			for
			(
				DWORD iCurrentMode = 0;
				iCurrentMode < UINT_MAX;
				++iCurrentMode
			)
			{
				ZeroMemory( &DeviceMode, sizeof( DeviceMode ) );
				DeviceMode.dmSpecVersion = DM_SPECVERSION;
				DeviceMode.dmSize = sizeof( DeviceMode );

				BOOL bEnumerateModeResult = EnumDisplaySettingsW
				(
					m_DeviceDescriptors[ iCurrentAdapter ].DeviceName,
					iCurrentMode,
					&DeviceMode
				);

				if ( bEnumerateModeResult != TRUE )
				{
					DWORD EnumerateModeError = GetLastError();
					LOG_ERROR
					(
						ERROR_TYPE_WIN32,
						EnumerateModeError,
						L"EnumDisplaySettings failed."
					);

					if ( EnumerateModeError != ERROR_SUCCESS )
					{
						hOverallResult = HRESULT_FROM_WIN32( EnumerateModeError );
					}
					else
					{
						hOverallResult = E_FAIL;
					};

					goto Cleanup;
				};

				if
				(
					DeviceMode.dmPelsWidth != CurrentDeviceMode.dmPelsWidth ||
					DeviceMode.dmPelsHeight != CurrentDeviceMode.dmPelsHeight
				)
				{
					bFoundSuitableMode = true;

					DeviceMode.dmFields =
						DM_BITSPERPEL |
						DM_PELSWIDTH |
						DM_PELSHEIGHT |
						DM_DISPLAYFREQUENCY |
						DM_DISPLAYORIENTATION;

					WCHAR MessageBuffer[ 1024 ] = L"";
					StringCchPrintfW
					(
						MessageBuffer,
						ARRAYSIZE( MessageBuffer ),
						L"CauseModeChange:  Calling ChangeDisplaySettings to change mode on device %s to %u x %u x %u bpp at %u Hz.",
						m_DeviceDescriptors[ iCurrentAdapter ].DeviceName,
						DeviceMode.dmPelsWidth,
						DeviceMode.dmPelsHeight,
						DeviceMode.dmBitsPerPel,
						DeviceMode.dmDisplayFrequency
					);

					LOG_MESSAGE
					(
						MESSAGE_TYPE_INFORMATIONAL,
						MessageBuffer
					);

					LONG ChangeDisplayModeResult = ChangeDisplaySettingsExW
					(
						m_DeviceDescriptors[ iCurrentAdapter ].DeviceName,
						&DeviceMode,
						NULL,
						Flags,
						NULL
					);

					if ( ChangeDisplayModeResult == DISP_CHANGE_SUCCESSFUL )
					{
						hOverallResult = S_OK;
						break;
					};
				};
			};

			if ( bFoundSuitableMode == true )
			{
				LOG_MESSAGE
				(
					MESSAGE_TYPE_INFORMATIONAL,
					L"CauseModeChange found at least one suitable mode but failed to trigger a mode change."
				);

				goto Cleanup;
			}
			else
			{
				LOG_MESSAGE
				(
					MESSAGE_TYPE_INFORMATIONAL,
					L"CauseModeChange:  Mode list did not include any modes that differed enough from the current mode to use in triggering a mode change."
				);

				hOverallResult = S_FALSE;
				goto Cleanup;
			};
		}
		else
		{
			DeviceMode.dmPelsWidth = Width;
			DeviceMode.dmPelsHeight = Height;
			DeviceMode.dmBitsPerPel = BitDepth;
			DeviceMode.dmDisplayFrequency = FixedRefreshRate;
			DeviceMode.dmDisplayOrientation = Orientation;

			DeviceMode.dmFields =
				DM_BITSPERPEL |
				DM_PELSWIDTH |
				DM_PELSHEIGHT |
				DM_DISPLAYFREQUENCY |
				DM_DISPLAYORIENTATION;

			WCHAR MessageBuffer[ 1024 ] = L"";
			StringCchPrintfW
			(
				MessageBuffer,
				ARRAYSIZE( MessageBuffer ),
				L"CauseModeChange:  Calling ChangeDisplaySettings to change mode on device %s to %u x %u x %u bpp at %u Hz.",
				m_DeviceDescriptors[ iCurrentAdapter ].DeviceName,
				DeviceMode.dmPelsWidth,
				DeviceMode.dmPelsHeight,
				DeviceMode.dmBitsPerPel,
				DeviceMode.dmDisplayFrequency
			);

			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				MessageBuffer
			);

			LONG ChangeDisplayModeResult = ChangeDisplaySettingsExW
			(
				m_DeviceDescriptors[ iCurrentAdapter ].DeviceName,
				&DeviceMode,
				NULL,
				Flags,
				NULL
			);

			if ( ChangeDisplayModeResult != DISP_CHANGE_SUCCESSFUL )
			{
				WCHAR ErrorMessage[ 1024 ] = L"";
				StringCchPrintfW
				(
					ErrorMessage,
					ARRAYSIZE( ErrorMessage ),
					L"ChangeDisplaySettingsEx failed with error %d.",
					ChangeDisplayModeResult
				);

				LOG_MESSAGE
				(
					MESSAGE_TYPE_INFORMATIONAL,
					ErrorMessage
				);

				hOverallResult = E_FAIL;
				goto Cleanup;
			};
		};
	};


	hOverallResult = S_OK;


Cleanup:
	SAFE_DELETE( pAdapterUtil );

	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  RestoreOriginalMode
//
// Purpose:
// Restores the device to the mode that it enumerated
// originally.
///////////////////////////////////////////////////////
HRESULT CGDIAdapterTopology::RestoreOriginalMode
(
	IN UINT Monitor
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = S_OK;


	//
	// Walk the list of monitors, setting them each
	// to the mode that it enumerated previously.
	//

	C_ASSERT( ARRAYSIZE( m_DeviceSlotForMonitorSlot ) == MONITOR_SLOTS );
	for
	(
		DWORD iCurrentMonitor = 0;
		iCurrentMonitor < MONITOR_SLOTS;
		++iCurrentMonitor
	)
	{
		//
		// If the caller asked for a mode change only on a specific
		// adapter, then skip the others.
		//

		if
		(
			Monitor != MONITOR_UNSPECIFIED &&
			Monitor != iCurrentMonitor ||
			m_DeviceSlotForMonitorSlot[ iCurrentMonitor ] == 0xFFFFFFFF
		)
		{
			continue;
		};


		//
		// Attempt to change the current monitor to its original mode.
		//

		HRESULT hCauseModeChangeResult = CauseModeChange
		(
			iCurrentMonitor,
			m_DeviceModes[ m_DeviceSlotForMonitorSlot[ iCurrentMonitor ] ].dmPelsWidth,
			m_DeviceModes[ m_DeviceSlotForMonitorSlot[ iCurrentMonitor ] ].dmPelsHeight,
			m_DeviceModes[ m_DeviceSlotForMonitorSlot[ iCurrentMonitor ] ].dmBitsPerPel,
			m_DeviceModes[ m_DeviceSlotForMonitorSlot[ iCurrentMonitor ] ].dmDisplayFrequency,
			m_DeviceModes[ m_DeviceSlotForMonitorSlot[ iCurrentMonitor ] ].dmDisplayOrientation,
			CDS_UPDATEREGISTRY
		);

		if ( FAILED( hCauseModeChangeResult ) )
		{
			hOverallResult = hCauseModeChangeResult;
		};
	};


	return hOverallResult;
};
