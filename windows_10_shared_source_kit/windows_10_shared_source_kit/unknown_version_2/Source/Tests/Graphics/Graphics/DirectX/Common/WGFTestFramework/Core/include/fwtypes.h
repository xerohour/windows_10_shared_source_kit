//////////////////////////////////////////////////////////////////////////
//  fwtypes.h
//  created:	2004/09/18
//
//  purpose: Common typedefs used throughout the framework
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>

#include "StringConverters.hpp"
#include "stringutils.h"
#include "assert.h"
#include "tchar.h"
#include "float.h"
#include "basetsd.h"

const float pi = 3.141592654f;

//////////////////////////////////////////////////////////////////////////
// MACROS
//////////////////////////////////////////////////////////////////////////
#define WGFTESTAPI __stdcall

#ifndef SAFE_RELEASE
#define SAFE_RELEASE( ptr ) if( ptr ) { ptr->Release(); ptr = NULL; }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE( ptr ) { delete( ptr ); ptr = NULL; }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY( ptr ) { delete[]( ptr ); ptr = NULL; }
#endif

#ifndef SAFE_FREE
#define SAFE_FREE( ptr ) if( ptr ) { free( ptr ); ptr = NULL; }
#endif

//////////////////////////////////////////////////////////////////////////
// TYPEDEFS
//////////////////////////////////////////////////////////////////////////
typedef unsigned long DWORD;

typedef int INT;
typedef char CHAR;
typedef short SHORT;

typedef unsigned int UINT;
typedef unsigned char UCHAR;
typedef unsigned short USHORT;

enum GRAPHICSBUSTYPES { BADBUS, UNKNOWNBUS, PCIBUS, AGPBUS };



//////////////////////////////////////////////////////////////////////////
// Structures
//////////////////////////////////////////////////////////////////////////
struct D3DFWException
{
	D3DFWException( const TCHAR *Message ) { _tcsncpy( szMessage, Message, sizeof( szMessage ) - 1 ); szMessage[sizeof( szMessage ) - 1] = 0; }
	
	TCHAR szMessage[1024];
};

