//////////////////////////////////////////////////////////////////////
// File:  CustomTestApp.cpp
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
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include "CustomTestApp.hpp"


/////////////////////////////////////////////////////////////////
//
// Helper functions
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  DecodeGDIDeviceStateFlags
//
// Purpose:
//
///////////////////////////////////////////////////////
HRESULT DecodeGDIDeviceStateFlags
(
	DWORD StateFlags,
	LPCWSTR Padding,
	LPCWSTR Delimiter,
	__out_ecount(nBuffer) LPWSTR Buffer,
	UINT nBuffer
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	BOOL bFirstFlag = TRUE;


	//
	// Validate parameters.
	//

	if ( Buffer == NULL )
	{
		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};

	if ( nBuffer == 0 )
	{
		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	//
	// Initialize parameters.
	//

	ZeroMemory( Buffer, nBuffer * sizeof( Buffer[ 0 ] ) );


	//
	// Decode state flags.
	//

	DECODE_FLAG( StateFlags, DISPLAY_DEVICE_PRIMARY_DEVICE, Padding, Delimiter );
	DECODE_FLAG( StateFlags, DISPLAY_DEVICE_ATTACHED_TO_DESKTOP, Padding, Delimiter );
	DECODE_FLAG( StateFlags, DISPLAY_DEVICE_MIRRORING_DRIVER, Padding, Delimiter );
	DECODE_FLAG( StateFlags, DISPLAY_DEVICE_VGA_COMPATIBLE, Padding, Delimiter );
	DECODE_FLAG( StateFlags, DISPLAY_DEVICE_MODESPRUNED, Padding, Delimiter );
	DECODE_FLAG( StateFlags, DISPLAY_DEVICE_REMOVABLE, Padding, Delimiter );


	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  GetWTSStateName
//
// Purpose:
// Helper function that retrieves a stringized version
// of a WTS_CONNECTSTATE_CLASS value.
///////////////////////////////////////////////////////
LPCSTR GetWTSStateName
(
	WTS_CONNECTSTATE_CLASS State
)
{
	//
	// Look up the state, and return the stringized
	// version.
	//

	switch ( State )
	{
		HANDLE_ENUM_A( WTSActive );
		HANDLE_ENUM_A( WTSConnected );
		HANDLE_ENUM_A( WTSConnectQuery );
		HANDLE_ENUM_A( WTSShadow );
		HANDLE_ENUM_A( WTSDisconnected );
		HANDLE_ENUM_A( WTSIdle );
		HANDLE_ENUM_A( WTSListen );
		HANDLE_ENUM_A( WTSReset );
		HANDLE_ENUM_A( WTSDown );
		HANDLE_ENUM_A( WTSInit );

		default:
		{
			return "[Unknown]";
		} break;
	};
};


//////////////////////////////////////////////////////////////////////
// Function:  ExceptionFilter
//
// Purpose:
// Determines which types of exceptions are eligible by handling in
// the application's __except block.  Currently the only exception
// that will be caught is an access violation.
//
// Called by:
// __except block of various pieces of code.
//
// Returns:
// EXCEPTION_EXECUTE_HANDLER     Go to the body of the __except block.
// EXCEPTION_CONTINUE_EXECUTION  Ignore the exception and continue.
// EXCEPTION_CONTINUE_SEARCH     Bubble the exception up the stack.
//////////////////////////////////////////////////////////////////////
int ExceptionFilter
(
	unsigned int ExceptionCode,
	_EXCEPTION_POINTERS * pExceptionDetails
)
{
	switch ( ExceptionCode )
	{
		case EXCEPTION_ACCESS_VIOLATION:
		{
			//OutputDebugString( "Caught access violation.\n" );
			return EXCEPTION_EXECUTE_HANDLER;
		};

		default:
		{
			//OutputDebugString( "Caught other exception.\n" );
			return EXCEPTION_CONTINUE_SEARCH;
		};
	};
};
