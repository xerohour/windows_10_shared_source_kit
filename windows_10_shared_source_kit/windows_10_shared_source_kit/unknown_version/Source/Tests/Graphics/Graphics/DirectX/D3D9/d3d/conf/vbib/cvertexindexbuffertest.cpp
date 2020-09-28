//////////////////////////////////////////////////////////////////////
// File:  CVertexIndexBufferTest.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of Direct3D vertex buffer and index buffer tests.
//
// History:
// ?? ??? ????  Darius Rad  Created.
// 25 Sep 2003  OlanH       Added comments added.  Re-worked to use
//                          framework's default 3D device.
// 28 Oct 2005  SEdmison    Cleaned up.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

#include "CVertexIndexBufferTest.h"
#include <strsafe.h>


/////////////////////////////////////////////////////////////////
//
// Statics and globals
//
/////////////////////////////////////////////////////////////////

//
// Declare the framework
//
                   
CD3DWindowFramework	App;
CD3DWindowFramework *g_pD3DFramework = NULL;


//
// Define the groups here
//

NormalBuffer    tstNormalBuffer;

#if 0  // optimized vertex buffers were cut
OptimizedBuffer tstOptimizedBuffer;
#endif

LockActive      tstLockActive;

Priority        tstPriority;

SwitchRenderState tstSwitchRenderState;

/*MultipleLocks   tstMultipleLocks;*//* in case needed for repro */

#ifndef FAST_TEST //This test is slow and mosly tests the runtime
LostOnReset     tstLostOnReset;
#endif

HugeBuffer      tstHugeBuffer;

RelockDynamic   tstRelockDynamic;

DiscardDynamic  tstDiscardDynamic;


////////////////////////////////////////////////////////////
//
// Construction / destruction
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CVertexIndexBufferTest
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CVertexIndexBufferTest::CVertexIndexBufferTest()
{
	SetTestType( TESTTYPE_CONF );
//    m_Options.D3DOptions.dwDevTypes  = DEVICETYPE_HAL;
//    m_Options.D3DOptions.dwBehaviors = DEVICECREATE_FPU_PRESERVE | DEVICECREATE_ALL;
    m_Options.D3DOptions.bZBuffer	 		= false;
	m_Options.D3DOptions.fMinDXVersion		= 8.f;
	g_pD3DFramework = &App;

#if FAST_TEST
    // Add some more space to fit all test cases in one back buffer
    m_Options.D3DOptions.nBackBufferWidth = (DWORD)1024;
    m_Options.D3DOptions.nBackBufferHeight = (DWORD)1024;
#endif //FAST_TEST
};


////////////////////////////////////////////////////////////
//
// Helper methods
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  GetAvailMemory
//
// Purpose:
// Returns the amount of available virtual memory, as
// reported by GlobalMemoryStatus.
///////////////////////////////////////////////////////
SIZE_T CVertexIndexBufferTest::GetAvailMemory
(
    DWORD Pool
)
{
    // since there is no real way to get vertex and index buffer memory, assume it's in sysmem

    MEMORYSTATUS memstat;

    // check if really out of memory
    GlobalMemoryStatus( &memstat );

    return(memstat.dwAvailVirtual );
};


////////////////////////////////////////////////////////////
//
// Static helper methods
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  ExceptionFilter
//
// Purpose:
// Determines which types of exceptions are eligible to
// be handled in the application's __except block.
// Currently the only exception that will be caught is
// an access violation.
//
// Called by:
// __except block of various pieces of code.
//
// Returns:
// EXCEPTION_EXECUTE_HANDLER     Go to the body of the
//                               __except block.
// EXCEPTION_CONTINUE_EXECUTION  Ignore the exception
//                               and continue.
// EXCEPTION_CONTINUE_SEARCH     Bubble the exception
//                               up the stack.
///////////////////////////////////////////////////////
int CVertexIndexBufferTest::ExceptionFilter
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


///////////////////////////////////////////////////////
// Function:  GetDisplayString
//
// Purpose:
// Correlates a Direct3D adapter ordinal to a GDI
// display device, and returns the display device name.
//
// @REVIEW [SEdmison]:
// It looks as though the D3D8 version of the adapter
// identifier structure (which is apparently the
// version that one would get by calling the legacy
// framework's wrapped version of GetAdapterIdentifier)
// doesn't contain the device name.  Otherwise, I would
// expect that we'd just call GetAdapterIdentifier on
// the adapter ordinal, and then use the DeviceName
// field.  This method seems a bit dodgy.
///////////////////////////////////////////////////////
bool CVertexIndexBufferTest::GetDisplayString
(
	UINT n,
	LPTSTR szDisplayString,
	UINT nDisplayString
)
{
	UINT nAdapter = 0;

	DISPLAY_DEVICE dd;
	ZeroMemory(&dd, sizeof(dd));
	dd.cb = sizeof(dd);

	// Always make the first entry reflect the primary device
	if ( n == 0 )
	{
		for ( DWORD i = 0; EnumDisplayDevices(NULL, i, &dd, 0); i++)
		{
			//
			// Skip drivers that are not hardware devices.
			//

			if (dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER)
				continue;

			//
			// Skip drivers that are not attached.
			//

			if (!(dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP))
				continue;


			if (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
			{
				StringCchCopy( szDisplayString, nDisplayString, dd.DeviceName );
				return true;
			}
		}

		return false;
	}


	// Now get the info for the attached secondary devices
	nAdapter = 1;
	for ( DWORD i = 0; EnumDisplayDevices(NULL, i, &dd, 0); i++)
	{
		//
		// skip drivers that are not hardware devices
		//
		if (dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER)
			continue;
		//
		// Skip drivers that are not attached
		//
		if (!(dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP))
			continue;

		//
		// skip the primary - it's already accounted for
		//
		if (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE )
			continue;

 		if ( n == nAdapter )
		{
			StringCchCopy( szDisplayString, nDisplayString, dd.DeviceName );
			return true;
		}
		nAdapter++;
	};

	return false;
};
