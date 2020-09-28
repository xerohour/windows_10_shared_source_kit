//////////////////////////////////////////////////////////////////////
// File:  D3D9CustomTestApp.cpp
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
// Helper functions
//
/////////////////////////////////////////////////////////////////

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


///////////////////////////////////////////////////////
// Function:  ToString( D3DDEVTYPE )
//
// Purpose:
// Converts a D3DDEVTYPE to a string.
///////////////////////////////////////////////////////
tstring __stdcall ToString
(
	const D3DDEVTYPE & DeviceType
)
{
	switch ( DeviceType )
	{
		case D3DDEVTYPE_HAL:
		{
			return tstring( _T("D3DDEVTYPE_HAL") );
		} break;


		case D3DDEVTYPE_NULLREF:
		{
			return tstring( _T("D3DDEVTYPE_NULLREF") );
		} break;


		case D3DDEVTYPE_REF:
		{
			return tstring( _T("D3DDEVTYPE_REF") );
		} break;


		case D3DDEVTYPE_SW:
		{
			return tstring( _T("D3DDEVTYPE_SW") );
		} break;


		default:
		{
			return tstring( _T( "" ) );
		} break;
	};
};


///////////////////////////////////////////////////////
// Function: FromString( D3DDEVTYPE *, const tstring )
//
// Purpose:
// Converts a string to a D3DDEVTYPE.
///////////////////////////////////////////////////////
bool __stdcall FromString
(
	__out D3DDEVTYPE * pType,
	const tstring &str
)
{
	if
	(
		tcistring( str ) == _T("D3DDEVTYPE_HAL") ||
		tcistring( str ) == _T("HW") ||
		tcistring( str ) == _T("HAL")
	)
	{
		*pType = D3DDEVTYPE_HAL;
		return true;
	};


	if
	(
		tcistring( str ) == _T("D3DDEVTYPE_NULLREF") ||
		tcistring( str ) == _T("NULLREF") ||
		tcistring( str ) == _T("NULL")
	)
	{
		*pType = D3DDEVTYPE_NULLREF;
		return true;
	};


	if
	(
		tcistring( str ) == _T("D3DDEVTYPE_REF") ||
		tcistring( str ) == _T("REF")
	)
	{
		*pType = D3DDEVTYPE_REF;
		return true;
	};


	if
	(
		tcistring( str ) == _T("D3DDEVTYPE_SW") ||
		tcistring( str ) == _T("SOFTWARE") ||
		tcistring( str ) == _T("SW")
	)
	{
		*pType = D3DDEVTYPE_SW;
		return true;
	};


	return false;
};
