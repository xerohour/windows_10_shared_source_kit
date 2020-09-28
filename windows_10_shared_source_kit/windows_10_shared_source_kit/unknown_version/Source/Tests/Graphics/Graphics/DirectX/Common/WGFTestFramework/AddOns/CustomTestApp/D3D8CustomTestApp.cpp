//////////////////////////////////////////////////////////////////////
// File:  D3D8CustomTestApp.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of Direct3D 8 custom test app object.
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

#include "D3D8CustomTestApp.hpp"
#include "GDIAdapterTopology.hpp"


/////////////////////////////////////////////////////////////////
//
// Helper functions
//
/////////////////////////////////////////////////////////////////

/*
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
*/