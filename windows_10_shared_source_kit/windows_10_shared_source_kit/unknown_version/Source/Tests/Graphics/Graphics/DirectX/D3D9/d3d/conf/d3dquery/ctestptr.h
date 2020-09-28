/************************************************************
 *															*
 *   Copyright (c) Microsoft Corporation, 2001				*
 *															*
 *   Program:   Header for CTestPtr API utility				*
 *   Created:   12/14/2001									*
 *    Author:   Daniel Roman (danielro)						*
 *															*
 ************************************************************/

#ifndef __CTESTPTR_H__
#define __CTESTPTR_H__

#define TESTDXVER 0x0900

#define STRICT
#include <tchar.h>
#include <windows.h>
#include "TestUtils.h"

// STL
#include <vector>
using namespace std;
#pragma warning( disable : 4786 4503 )


//
// handy stuff
//
#define COUNTOF(a)  (sizeof(a)/sizeof((a)[0]))
#define THIS_AINT_A_POINTER (LPVOID)((UINT_PTR)(-1))

typedef enum
{
	PTR_OK = 1,
	PTR_NULL = 2,
	PTR_INVALID = 3
} PTR_TYPE;

typedef enum
{
	RET_UNKNOWN = 0,
	RET_VALID   = 1,
	RET_INVALID = 2
} RET_TYPE;

// helpers

UINT GetAvailMemory();

//-----------------------------------------------------------------------------
// This helper class is used to create and test unaligned structures against
// buffer overruns from D3D
//-----------------------------------------------------------------------------

#define PTR_PAD_SIG (BYTE)0xbe
class CTestPtr
{
protected:
	LPVOID m_Ptr;			// Address of the whole memory chunk
	LPVOID m_PtrData;		// Address of the structure within the chunk
	UINT   m_nBeginPad;		// Number of bytes to pad onto the beginning of the struct
	UINT   m_nEndPad;       // Number of bytes to pad onto the end of the struct
	UINT   m_nSize;         // size of the structure within the chunk
public:
	CTestPtr(UINT nSize, UINT nBeginPad = 4, UINT nEndPad = 4);
	~CTestPtr();
	void   Clear();
	bool   IsValid();
	bool   IsZeroed();
	inline LPVOID GetPtr()	{ return m_PtrData; }
};

#endif
