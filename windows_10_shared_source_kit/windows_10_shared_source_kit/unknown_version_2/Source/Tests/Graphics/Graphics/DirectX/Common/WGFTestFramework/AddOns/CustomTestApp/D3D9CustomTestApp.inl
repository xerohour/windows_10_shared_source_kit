//////////////////////////////////////////////////////////////////////
// File:  D3D9CustomTestApp.inl
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of Direct3D 9.L custom test app object.
//
// History:
// 10 Mar 2006  SEdmison  Factored from other tests into a shared lib.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include "D3D9CustomTestApp.hpp"
#include "GDIAdapterTopology.hpp"


/////////////////////////////////////////////////////////////////
//
// Implementation
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CD3D9CustomTestApp
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CD3D9CustomTestApp::CD3D9CustomTestApp()
:
	m_bDebugHelper( false ),
	m_DeviceType( D3DDEVTYPE_HAL )
{
};


///////////////////////////////////////////////////////
// Function:  InitOptionVariables
//
// Purpose:
// Framework override method for registering option
// variables.
///////////////////////////////////////////////////////
void CD3D9CustomTestApp::InitOptionVariables()
{
	int CategoryDebug = AddOptionCategory( _T( "Debugging" ) );

	int CategoryD3D = AddOptionCategory( _T( "D3D" ) );


	//
	// Register framework option variables for the values that
	// we need or expect to receive from the command line.
	//

	RegisterOptionVariable< bool >
	(
		_T( "DebugHelper" ),
		_T( "Enable D3D debug helper spew" ),
		&m_bDebugHelper,
		false,
		OPTION_VALUE_CONSTANT,
		CategoryDebug
	);

	RegisterOptionVariable< D3DDEVTYPE >
	(
		_T( "Src" ),
		_T( "Source driver type" ),
		&m_DeviceType,
		D3DDEVTYPE_HAL,
		0,
		CategoryD3D,
		_T( "-src:[REF|HAL|SW|NULLREF]" )
	);
};


///////////////////////////////////////////////////////
// Function:  UpdateCurrentDeviceInfo
//
// Purpose:
// Updates device information for the log.
///////////////////////////////////////////////////////
bool CD3D9CustomTestApp::UpdateCurrentDeviceInfo
(
	__in DXGLOG_DEVICEINFO * pDeviceInfo
)
{
	//
	// Validate parameters.
	//

	if ( pDeviceInfo == NULL )
	{
		return false;
	};


	//
	// Enumerate user/GDI display devices.
	//

	CGDIAdapterTopology AdapterTopology;

	AdapterTopology.EnumerateAdaptersAndOutputs();


	//
	// Initialize locals.
	//

	bool bOverallResult = false;

	IDirect3D9 * pD3D9 = Direct3DCreate9
	(
		D3D_SDK_VERSION
	);

	if ( pD3D9 == NULL )
	{
		goto Cleanup;
	};


	//
	// Get an adapter identifier for the default device.
	//

	D3DADAPTER_IDENTIFIER9 AdapterIdentifier;
	ZeroMemory( &AdapterIdentifier, sizeof( AdapterIdentifier ) );

	HRESULT hGetIdentifierResult = pD3D9->GetAdapterIdentifier
	(
		D3DADAPTER_DEFAULT,
		0,
		&AdapterIdentifier
	);

	if ( FAILED( hGetIdentifierResult ) )
	{
		goto Cleanup;
	};


	//
	// Fill in DXGLOG parameters.
	//

	pDeviceInfo->fMinDXVersion = 9.1f;
	pDeviceInfo->fMaxDXVersion = 9.1f;
	pDeviceInfo->bPSGP = false;

	pDeviceInfo->sDisplay = new TCHAR[ MAX_DEVICE_IDENTIFIER_STRING ];
	if ( pDeviceInfo->sDisplay == NULL )
	{
		goto Cleanup;
	};
#ifdef UNICODE
	int cCharactersConverted = MultiByteToWideChar
	(
		CP_ACP,
		0,
		AdapterIdentifier.Description,
		-1,
		pDeviceInfo->sDevice,
		MAX_DEVICE_IDENTIFIER_STRING
	);
#else
	StringCchCopy
	(
		pDeviceInfo->sDisplay,
		MAX_DEVICE_IDENTIFIER_STRING,
		AdapterIdentifier.Description
	);
#endif


	pDeviceInfo->sDriver = new TCHAR[ MAX_DEVICE_IDENTIFIER_STRING ];
	if ( pDeviceInfo->sDriver == NULL )
	{
		goto Cleanup;
	};
#ifdef UNICODE
	cCharactersConverted = MultiByteToWideChar
	(
		CP_ACP,
		0,
		AdapterIdentifier.Driver,
		-1,
		pDeviceInfo->sDriver,
		MAX_DEVICE_IDENTIFIER_STRING
	);
#else
	StringCchCopy
	(
		pDeviceInfo->sDriver,
		MAX_DEVICE_IDENTIFIER_STRING,
		AdapterIdentifier.Driver
	);
#endif


	pDeviceInfo->sDriverVersion = new TCHAR[ 256 ];
	if ( pDeviceInfo->sDriverVersion == NULL )
	{
		goto Cleanup;
	};
	StringCchPrintf
	(
		pDeviceInfo->sDriverVersion,
		256,
		_T( "%d.%d.%d.%d" ),
		HIWORD( AdapterIdentifier.DriverVersion.HighPart ), // product
		LOWORD( AdapterIdentifier.DriverVersion.HighPart ), // version
		HIWORD( AdapterIdentifier.DriverVersion.LowPart ),  // sub-version
		LOWORD( AdapterIdentifier.DriverVersion.LowPart ) // build
	);

	pDeviceInfo->bDriverDebug = false;
	pDeviceInfo->dwVendorID = AdapterIdentifier.VendorId;
	pDeviceInfo->dwDeviceID = AdapterIdentifier.DeviceId;


	pDeviceInfo->sPnPID = new TCHAR[ 48 ];
	if ( pDeviceInfo->sPnPID == NULL )
	{
		goto Cleanup;
	};
	StringCchPrintf
	(
		pDeviceInfo->sPnPID,
		48,
		_T("VEN_%04X&DEV_%04X&SUBSYS_%08X&REV_%02X"),
		AdapterIdentifier.VendorId,
		AdapterIdentifier.DeviceId,
		AdapterIdentifier.SubSysId,
		AdapterIdentifier.Revision
	);


	pDeviceInfo->sResolution= new TCHAR[ 32 ];
	if ( pDeviceInfo->sResolution == NULL )
	{
		goto Cleanup;
	};
	StringCchPrintf
	(
		pDeviceInfo->sResolution,
		32,
		_T( "%u x %u x %s @ %u Hz" ),
		1024,
		768,
		_T( "X8R8G8B8" ),
		60
	);

	pDeviceInfo->nDDI = 9;
	pDeviceInfo->fVersion = 9.1f;
	pDeviceInfo->fBuild = GetDXBuild();
	pDeviceInfo->bDebug = false;
	pDeviceInfo->fInterface = 9.1f;


	pDeviceInfo->sDevice = new TCHAR[ 256 ];
	if ( pDeviceInfo->sDevice == NULL )
	{
		goto Cleanup;
	};
	ZeroMemory( pDeviceInfo->sDevice, 256 * sizeof( pDeviceInfo->sDevice[ 0 ] ) );


	pDeviceInfo->dwDispMode = 0;
	pDeviceInfo->wDesktopBPP = 32;


	//
	// Get device capabilities.
	//

	D3DCAPS9 DeviceCaps;
	ZeroMemory( &DeviceCaps, sizeof( DeviceCaps ) );
	HRESULT hGetDeviceCapsResult = pD3D9->GetDeviceCaps
	(
		D3DADAPTER_DEFAULT,
		m_DeviceType,
		&DeviceCaps
	);

	if ( FAILED( hGetDeviceCapsResult ) )
	{
		WriteToLogF
		(
			_T( "UpdateCurrentDeviceInfo:  GetDeviceCaps failed with error 0x%x." ),
			hGetDeviceCapsResult
		);

		//goto Cleanup;
	};


	switch ( m_DeviceType )
	{
		case D3DDEVTYPE_HAL:
		{
			StringCchPrintf
			(
				pDeviceInfo->sDevice,
				256,
				_T( "DX9.1 Pure HAL, %s VP" ),
				(
					( DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) == 0
					? _T( "SW" )
					: _T( "HW" )
				)
			);
		} break;


		case D3DDEVTYPE_NULLREF:
		{
			StringCchPrintf
			(
				pDeviceInfo->sDevice,
				256,
				_T( "DX9.1 NULLREF, %s VP" ),
				(
					( DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) == 0
					? _T( "SW" )
					: _T( "HW" )
				)
			);
		} break;


		case D3DDEVTYPE_REF:
		{
			StringCchPrintf
			(
				pDeviceInfo->sDevice,
				256,
				_T( "DX9.1 REF, %s VP" ),
				(
					( DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) == 0
					? _T( "SW" )
					: _T( "HW" )
				)
			);
		} break;


		case D3DDEVTYPE_SW:
		{
			StringCchPrintf
			(
				pDeviceInfo->sDevice,
				256,
				_T( "DX9.1 SW, %s VP" ),
				(
					( DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) == 0
					? _T( "SW" )
					: _T( "HW" )
				)
			);
		} break;


		default:
		{
			assert( FALSE );
		} break;
	};


	//
	// If we made it this far, we're good to go.
	//

	bOverallResult = true;


Cleanup:
	if ( pD3D9 != NULL )
	{
		pD3D9->Release();
		pD3D9 = NULL;
	};

	return true;
};
