//
// Project headers
//

#include "GetDeviceInfo9.h"


//
// StrSafe.h has to come last, because it deprecates APIs that
// it doesn't like.
//

#include <strsafe.h>


/*
BEGIN_NAMED_VALUES( D3D10_DRIVER_TYPE )
    NAMED_VALUE( _T( "HW" ), D3D10_DRIVER_TYPE_HARDWARE )
    NAMED_VALUE( _T( "REF" ), D3D10_DRIVER_TYPE_REFERENCE )
    NAMED_VALUE( _T( "NULL" ), D3D10_DRIVER_TYPE_NULL )
    NAMED_VALUE( _T( "SW" ), D3D10_DRIVER_TYPE_SOFTWARE )
END_NAMED_VALUES( D3D10_DRIVER_TYPE )
*/


typedef HRESULT (WINAPI *PDIRECT3DCREATE9EX)
(
	UINT SDKVersion,
	IDirect3D9Ex**
);


clsGetDeviceInfo9::clsGetDeviceInfo9()
:
	m_DriverType( D3D10_DRIVER_TYPE_HARDWARE )
{
}

clsGetDeviceInfo9::clsGetDeviceInfo9( D3D10_DRIVER_TYPE DriverType )
:
	m_DriverType( DriverType )
{
}

clsGetDeviceInfo9::~clsGetDeviceInfo9()
{
}


///////////////////////////////////////////////////////
// Function:  GetDXBuild
//
// Purpose:
// Parses the DirectX version number from the registry
// and returns the build number.
///////////////////////////////////////////////////////
float GetDXBuild()
{
	float Version = 0;


	//
	// Open the DirectX registry key.
	//

	HKEY hKey = NULL;
	LONG OpenRegKeyResult = RegOpenKeyEx
	(
		HKEY_LOCAL_MACHINE,
		_T( "Software\\Microsoft\\DirectX" ),
		0,
		KEY_READ,
		&hKey
	);

	if ( OpenRegKeyResult != ERROR_SUCCESS )
	{
		goto Cleanup;
	};


	//
	// Look for the Version value.
	//

	CHAR pVersionBuffer[ 20 ] = "";
	DWORD ValueType = REG_SZ;
	DWORD ValueSize = sizeof( pVersionBuffer ) / sizeof( pVersionBuffer[ 0 ] ) - 1;
	LONG QueryValueResult = RegQueryValueEx
	(
		hKey,
		_T( "Version" ),
		NULL,
		&ValueType,
		(BYTE *) pVersionBuffer,
		&ValueSize
	);

	if ( QueryValueResult != ERROR_SUCCESS )
	{
		goto Cleanup;
	};


	//
	// Find the last segment in the version string, and assume
	// that that's the build number.
	//

	PSTR pFinalDot = strrchr( pVersionBuffer, '.' );
	if ( pFinalDot == NULL )
	{
		goto Cleanup;
	};


	//
	// Convert the build number to a float.
	//

	Version = (float) atof( pFinalDot + 1 );


Cleanup:
	if ( hKey != NULL )
	{
		RegCloseKey( hKey );
		hKey = NULL;
	};


	return Version;
};


void clsGetDeviceInfo9::InitOptionVariables()
{
	CWGFTestFramework::InitOptionVariables();

	int id = AddOptionCategory( "General" );
    RegisterOptionVariable
    (
        "BVT",
        "Run in BVT (short Build Verification Test) mode.",
        &m_bBVT,
        false,
        OPTION_VALUE_CONSTANT,
        id
    );
	id = AddOptionCategory( "D3D" );
	RegisterOptionVariable
	(
		"Src",
		"Source driver type",
		&m_DriverType,
		D3D10_DRIVER_TYPE_HARDWARE,
		0,
		id,
		_T( "-src:[REF|HW|SW|NULL]" )
	);
}


bool clsGetDeviceInfo9::UpdateCurrentDeviceInfo(DXGLOG_DEVICEINFO * pDeviceInfo)
{
	//
	// Validate parameters.
	//

	if ( pDeviceInfo == NULL )
	{
		return false;
	};


	bool bOverallResult = false;


	//
	// Initialize locals.
	//

	HMODULE hD3D9 = NULL;
	PDIRECT3DCREATE9EX pDirect3DCreate9Ex = NULL;
	IDirect3D9Ex * pD3D = NULL;


	//
	// Load D3D.
	//

	hD3D9 = LoadLibraryExW( L"d3d9.dll", NULL, 0 );

	if ( hD3D9 == NULL )
	{
		goto Cleanup;
	};


	//
	// Get a pointer to our entry point.
	//

	pDirect3DCreate9Ex = (PDIRECT3DCREATE9EX) GetProcAddress
	(
		hD3D9,
		"Direct3DCreate9Ex"
	);

	if ( pDirect3DCreate9Ex == NULL )
	{
		goto Cleanup;
	};


	//
	// Initialize Direct3D.
	//

	HRESULT hInitializeD3DResult = (*pDirect3DCreate9Ex)
	(
		D3D_SDK_VERSION,
		&pD3D
	);

	if ( FAILED( hInitializeD3DResult ) )
	{
		goto Cleanup;
	};

	if ( pD3D == NULL )
	{
		goto Cleanup;
	};


	//
	// Get an adapter identifier for the default device.
	//

	D3DADAPTER_IDENTIFIER9 AdapterIdentifier;
	ZeroMemory( &AdapterIdentifier, sizeof( AdapterIdentifier ) );

	HRESULT hGetIdentifierResult = pD3D->GetAdapterIdentifier
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

	pDeviceInfo->fMinDXVersion = 10.0f;
	pDeviceInfo->fMaxDXVersion = 10.0f;
	pDeviceInfo->bPSGP = false;

	pDeviceInfo->sDisplay = new TCHAR[ MAX_DEVICE_IDENTIFIER_STRING ];
	if ( pDeviceInfo->sDisplay == NULL )
	{
		return false;
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
		return false;
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
		return false;
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
		return false;
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
		return false;
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
		return false;
	};

	/*
	StringCchPrintf
	(
		pDeviceInfo->sDevice,
		256,
		_T( "DX9.1 Pure HAL, %s VP" ),
		(
			( m_D3DAdapterCaps[ MONITOR_CENTER ].DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) == 0
			? _T( "SW" )
			: _T( "HW" )
		)
	);
	*/

	switch ( m_DriverType )
	{
		case D3D10_DRIVER_TYPE_REFERENCE:
		{
			StringCchPrintf
			(
				pDeviceInfo->sDevice,
				256,
				_T( "REF" )
			);
		} break;

		case D3D10_DRIVER_TYPE_NULL:
		{
			StringCchPrintf
			(
				pDeviceInfo->sDevice,
				256,
				_T( "NULL" )
			);
		} break;

		case D3D10_DRIVER_TYPE_SOFTWARE:
		{
			StringCchPrintf
			(
				pDeviceInfo->sDevice,
				256,
				_T( "SW" )
			);
		} break;

		case D3D10_DRIVER_TYPE_HARDWARE:
		{
			StringCchPrintf
			(
				pDeviceInfo->sDevice,
				256,
				_T( "HW" )
			);
		} break;

		default:
		{
			StringCchPrintf
			(
				pDeviceInfo->sDevice,
				256,
				_T( "[Unknown]" )
			);
		} break;
	};


	pDeviceInfo->dwDispMode = 0;
	pDeviceInfo->wDesktopBPP = 32;


	//
	// If we made it this far, we're good to go.
	//

	bOverallResult = true;


Cleanup:
	if ( pD3D != NULL )
	{
		pD3D->Release();
		pD3D = NULL;
	};

	if ( hD3D9 != NULL )
	{
		FreeLibrary( hD3D9 );
		hD3D9 = NULL;

		pDirect3DCreate9Ex = NULL;
	};


	return bOverallResult;
}
