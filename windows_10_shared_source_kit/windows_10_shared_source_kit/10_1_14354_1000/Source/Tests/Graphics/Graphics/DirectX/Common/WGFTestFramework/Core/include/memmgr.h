//////////////////////////////////////////////////////////////////////////
//  memmgr.h
//  created:	2004/10/06
//
//  purpose: Declares the memory manager and related memory functions.
//  NOTE: This header file must be included *before* any other header files
//  this may contain static objects.  This is the only way to ensure that
//	the memory manager is initialized before any memory is allocated
//  and that the memory manager is the last object deleted.
//////////////////////////////////////////////////////////////////////////
#pragma once

#include <stdlib.h>
#include <malloc.h>

#ifdef _DEBUG
#ifndef _WGFTESTMEM
#define _WGFTESTMEM
#endif // _WGFTESTMEM
#endif // _DEBUG

#ifdef _WGFTESTMEM

#define MM_HASH_TABLE_SIZE ( 1 + (unsigned short) -1 )

// Memory manager code should only be implemented
// in debug builds or builds that define _WGFTESTMEM explicitly.

extern void * __cdecl my_new(size_t size, const char *file, int line );
extern void * __cdecl my_new_array( size_t size, const char *file, int line );
extern void __cdecl my_delete( void *address );
extern void __cdecl my_delete_array( void *address );

#define __new new( __FILE__, __LINE__ )
#define __delete CMemoryManager::Instance().SetOwner( __FILE__, __LINE__), false ? CMemoryManager::Instance().SetOwner( 0, 0 ) : delete


#ifndef __MEMMGR__
#define malloc( sz ) CMemoryManager::Instance().AllocateMemory( __FILE__, __LINE__, sz, CMemoryManager::MM_MALLOC )
#define calloc( num, sz ) CMemoryManager::Instance().AllocateMemory( __FILE__, __LINE__, sz * num, CMemoryManager::MM_CALLOC )
#define realloc( ptr, sz ) CMemoryManager::Instance().AllocateMemory( __FILE__, __LINE__, sz, CMemoryManager::MM_REALLOC, ptr )
#define free( ptr ) CMemoryManager::Instance().FreeMemory( __FILE__, __LINE__, ptr, CMemoryManager::MM_FREE )

static void * __cdecl operator new(size_t size, const char *file, int line) { return my_new( size, file, line ); }
static void * __cdecl operator new[]( size_t size, const char *file, int line ) { return my_new_array( size, file, line ); }
static void __cdecl operator delete( void *address ) { return my_delete( address ); }
static void __cdecl operator delete[]( void *address ) { return my_delete_array( address ); }
#endif // __MEMMGR__


// Placement forms of operator delete in the event that operator new throws an exception.
static void __cdecl operator delete( void *address, const char *file, int line ) { return my_delete( address ); }
static void __cdecl operator delete[]( void *address, const char *file, int line ) { return my_delete_array( address ); }

class CMemoryManager
{
public:
	enum MMTYPE
	{
		MM_NEW,
		MM_NEW_ARRAY,
		MM_MALLOC,
		MM_CALLOC,
		MM_REALLOC,
		MM_FREE,
		MM_DELETE,
		MM_DELETE_ARRAY
	};

	//////////////////////////////////////////////////////////////////////////
	// Instance()
	//
	// Returns a reference to the global memory manager object.
	//////////////////////////////////////////////////////////////////////////
	static CMemoryManager &__stdcall Instance() { return me; }
	
	//////////////////////////////////////////////////////////////////////////
	// EnableMemoryLogging()
	//
	// Starts/stops logging memory allocations.  The return value
	// is the previous allocation logging state.  Use this to restore
	// memory logging state.
	//////////////////////////////////////////////////////////////////////////
	bool EnableMemoryLogging( bool bEnable );

	//////////////////////////////////////////////////////////////////////////
	// ClearMemoryLog()
	//
	// Clears currently logged memory allocations.  Does not stop memory
	// logging.
	//////////////////////////////////////////////////////////////////////////
	void ClearMemoryLog();

	//////////////////////////////////////////////////////////////////////////
	// DumpMemoryLeaks()
	//
	// Reports all currently unfreed memory allocations as leaks to the debug
	// console.
	//////////////////////////////////////////////////////////////////////////
	void DumpMemoryLeaks();

	//////////////////////////////////////////////////////////////////////////
	// BreakOnAllocationID()
	//
	// Schedules a break on the specified allocation id.  Subsequent calls
	// to BreakOnAllocationID() will override the previous call.
	//////////////////////////////////////////////////////////////////////////
	void BreakOnAllocationID( size_t nAllocationID ) { m_breakOnID = nAllocationID; }
	
	//
	// Memory allocation methods.
	//
	void *AllocateMemory( const char *file, int line, size_t size, MMTYPE type, void *ptr = 0 );
	void FreeMemory( const char *file, int line, void *ptr, MMTYPE type );
	void SetOwner( const char *file, int line );
	void GetOwner( const char **file, int *line );
	
private:
	CMemoryManager();
	~CMemoryManager();
	
	struct MemoryNode
	{
		size_t nActualSize;
		size_t nReportedSize;
		void *pActualAddress;
		void *pReportedAddress;
		char szSourceFile[_MAX_PATH];
		unsigned int nLineNumber;
		MMTYPE AllocationType;
		MemoryNode *pPrev, *pNext;
		size_t nAllocationID;
	};

	MemoryNode *m_MemNodeHashTable[MM_HASH_TABLE_SIZE];
	bool m_bStarted;
	size_t m_breakOnID;

private:
	MemoryNode *NewMemoryNode( void *pAddress );
	MemoryNode *FindMemoryNode( void *pAddress );
	MemoryNode *RemoveMemoryNode( void *pAddress );
	
	static CMemoryManager me;
};
#else // _WGFTESTMEM

// Stub out CMemoryManager class
class CMemoryManager
{
public:
	static CMemoryManager &__stdcall Instance() { return me; }
	
	inline bool EnableMemoryLogging( bool bEnable ) { return true; }
	inline void ClearMemoryLog() {}
	inline void DumpMemoryLeaks() {}	
	inline void BreakOnAllocationID( size_t nAllocationID ) {}

private:
	static CMemoryManager me;
};
#endif // _WGFTESTMEM
