/******************************Module*Header*******************************
* Module Name: WGFPayloadTDR.h
*
* D3D11 Graphics and Compute TDR workload test
*
* Author: David Flenniken [davidfl]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/
#pragma once

// The ActionMacros.h file requires the following three headers
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
                
// Included for friendly macros
#include <ActionMacros.h>

#include <tchar.h>
#include <dxgi.h>
#include <D3D11Test.h>
#include <WGFTestCore.h>
#include "PresentHelper.h"
#include "CrossProcSharing.h"

#include <windows.h>
#include <wmistr.h>
#include <evntrace.h>
#include <strsafe.h>
#include <Evntcons.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

//-------------------------------------------------------------
// Definitions for D3D kernel mode DDI
//
typedef struct _MDL {
    struct _MDL *MdlNext;
    short MdlSize;
    short MdlFlags;
    struct _EPROCESS *Process;
    ULONG *lpMappedSystemVa;
    ULONG *lpStartVa;
    ULONG ByteCount;
    ULONG ByteOffset;
} MDL;
typedef MDL *PMDL;

typedef LONG NTSTATUS;

#ifndef PHYSICAL_ADDRESS
typedef LARGE_INTEGER PHYSICAL_ADDRESS;
#endif

#include <d3dkmddi.h>
#include <d3dkmthk.h>
#include <d3dkmthk_p.h>
#include <dxgkdrt.h>

#include <wbemcli.h>  
#include <Wbemidl.h>  
#include "debugmon.h"

// All the base classes derive off the CConsumer class.
#include <Consumer.h>
#include <Controller.h>

// For crimson event decoding we need the tdh stuff and our etw EventHelper header.
#include <tdh.h>
#include <EventHelper.h>
// the etw VersionHelper header is used if a filter needs to destiguish between different
// OS builds (the common practice is to issue a different provider GUID based on the OS.
#include <VersionHelper.h>


// main compute entry point.
int __cdecl mainFuncCompute();

