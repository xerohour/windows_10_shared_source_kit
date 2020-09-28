//////////////////////////////////////////////////////////////////////////
//  memmgr.cpp
//  created:	2004/10/06
//
//  purpose: Memory allocation/tracking utilities.
//////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "new_off.h"

#pragma warning( push )
#pragma warning( disable: 4073 4068 )
#pragma init_seg(lib)

#include <cstdlib>
#include <new>

#ifdef malloc
#undef malloc
#endif

#ifdef calloc
#undef calloc
#endif

#ifdef realloc
#undef realloc
#endif

#ifdef free
#undef free
#endif

#define NUM_PAD_LONGS (8)
#define PAD_VALUE (0xDEADBEEF)

_C_LIB_DECL
extern int __cdecl _callnewh(size_t size) _THROW1(_STD bad_alloc);
_END_C_LIB_DECL

#pragma prefast( disable: 419, "There is no way to validate the size here." ) 

CMemoryManager CMemoryManager::me;

//////////////////////////////////////////////////////////////////////////
// Memory allocation code
//////////////////////////////////////////////////////////////////////////

#ifdef _WGFTESTMEM

void *__cdecl my_new( size_t size, const char *file, int line )
{
	return CMemoryManager::Instance().AllocateMemory( file, line, size, CMemoryManager::MM_NEW, NULL );
}

void *__cdecl my_new_array( size_t size, const char *file, int line )
{
	return CMemoryManager::Instance().AllocateMemory( file, line, size, CMemoryManager::MM_NEW_ARRAY, NULL );
}

void __cdecl my_delete( void *address )
{
	const char *file;
	int line;
	CMemoryManager::Instance().GetOwner( &file, &line );
	
	if( file == 0 )
		free( address );
	else
		CMemoryManager::Instance().FreeMemory( file, line, address, CMemoryManager::MM_DELETE );
}

void __cdecl my_delete_array( void *address )
{
	const char *file;
	int line;
	CMemoryManager::Instance().GetOwner( &file, &line );
	
	if( file == 0 )
		free( address );
	else
		CMemoryManager::Instance().FreeMemory( file, line, address, CMemoryManager::MM_DELETE_ARRAY );
}

//////////////////////////////////////////////////////////////////////////
// CMemoryManager implementation
//////////////////////////////////////////////////////////////////////////
bool CMemoryManager::EnableMemoryLogging( bool bEnable )
{
	bool bRet = m_bStarted;
	m_bStarted = bEnable;
	return bRet;
}

void CMemoryManager::ClearMemoryLog()
{
	for( unsigned int nIndex = 0; nIndex < MM_HASH_TABLE_SIZE; ++nIndex )
	{
		MemoryNode *pNode = m_MemNodeHashTable[nIndex];
		
		if( pNode )
		{
			MemoryNode *pNext = NULL;
			
			for( ; pNode != NULL; pNode = pNext )
			{
				pNext = pNode->pNext; 
				free( pNode ); 
			}
			
			m_MemNodeHashTable[nIndex] = NULL;
		}
	}
}

void CMemoryManager::DumpMemoryLeaks()
{
	bool bOldStarted = m_bStarted;
	m_bStarted = false;
	
	for( int nIndex = 0; nIndex < MM_HASH_TABLE_SIZE; ++nIndex )
	{
		for( MemoryNode *pNode = m_MemNodeHashTable[nIndex]; pNode != NULL; pNode = pNode->pNext )
		{
			_DPF( _T( "\n" ) );
			_DPF( _T( "%s(%i) : Memory Leak Detected\n" ), pNode->szSourceFile, pNode->nLineNumber );
			_DPF( _T( "    Allocation Address: 0x%8.8x\n" ), pNode->pReportedAddress );
			_DPF( _T( "    Allocation Size: %u\n" ), pNode->nReportedSize );
			_DPF( _T( "    Allocation ID: %u\n" ), pNode->nAllocationID );
			_DPF( _T( "\n" ) );
		}
	}
	
	m_bStarted = bOldStarted;
}

static unsigned short MMHash( void *pAddress )
{
	size_t n = reinterpret_cast< size_t >( pAddress );
	return (unsigned short) ( n ^ ( n >> 16 ) );
}

CMemoryManager::CMemoryManager()
{
	EnableMemoryLogging( true );
	m_breakOnID = 0;
}

CMemoryManager::~CMemoryManager()
{
	EnableMemoryLogging( false );
	DumpMemoryLeaks();
}

CMemoryManager::MemoryNode *CMemoryManager::NewMemoryNode( void *pAddress )
{
	bool bOldStarted = m_bStarted;
	m_bStarted = false;
	
	unsigned short hash = MMHash( pAddress );
	
	MemoryNode *pNode = (MemoryNode *) malloc( sizeof( MemoryNode ) );
	if( pNode )
	{
		if( ( pNode->pNext = m_MemNodeHashTable[hash] ) != NULL )
			pNode->pNext->pPrev = pNode;
		m_MemNodeHashTable[hash] = pNode;
		pNode->pPrev = NULL;
				
		m_bStarted = bOldStarted;
	}
		
	return pNode;
}

CMemoryManager::MemoryNode *CMemoryManager::FindMemoryNode( void *pAddress )
{
	MemoryNode *pNode = NULL;
	unsigned short hash = MMHash( pAddress );
	
	// Find the address in the hash table
	for( pNode = m_MemNodeHashTable[hash]; pNode != NULL && pNode->pReportedAddress != pAddress; pNode = pNode->pNext ) {}
	
	return pNode;
}

CMemoryManager::MemoryNode *CMemoryManager::RemoveMemoryNode( void *pAddress )
{
	MemoryNode **ppNode = NULL;
	unsigned short hash = MMHash( pAddress );
	
	// Find the dead node in the hash table
	for( ppNode = &m_MemNodeHashTable[hash]; *ppNode != NULL; ppNode = &( (*ppNode)->pNext ) )
	{
		if( (*ppNode)->pReportedAddress == pAddress )
			break;
	}
	
	MemoryNode *pDeadNode = *ppNode;
	
	if( pDeadNode != NULL )
	{
		// Remove the dead node from the table
		if( ( *ppNode = pDeadNode->pNext ) != NULL )
		{
			pDeadNode->pNext->pPrev = pDeadNode->pPrev;
		}
	}		
	
	// Return a pointer to the dead node.
	// It is the responsibility of the calling function to free the node.
	return pDeadNode;
}
void *CMemoryManager::AllocateMemory( const char *file, int line, size_t size, MMTYPE type, void *ptr /* = NULL  */)
{
	void *pAddress;
	const size_t nPaddingSize = NUM_PAD_LONGS * sizeof( unsigned long );
	size_t nActualSize = size;
	static size_t nAllocationID = 0;
	
	if( m_bStarted )
		nActualSize += 2 * nPaddingSize;
	
	switch( type )
	{
		case MM_MALLOC:
		case MM_CALLOC:
			pAddress = malloc( nActualSize );
			break;
			
		case MM_NEW:
		case MM_NEW_ARRAY:
			while( ( pAddress = malloc( nActualSize ) ) == 0 )
				if( _callnewh( size ) == 0 )
					throw _STD bad_alloc();	//_STD _Nomemory();
			break;
			
		case MM_REALLOC:
			pAddress = malloc( nActualSize );
			break;
			
		default:
			return NULL;
			break;			
	}
	
    if ( pAddress == NULL )
    {
        return NULL;
    }

	if( m_bStarted )
	{
		void *pReportedAddress = reinterpret_cast< void *>( reinterpret_cast< size_t >( pAddress ) + nPaddingSize );
		
		// Create a memory node and add it to the hash table
		MemoryNode *pMemoryNode = NewMemoryNode( pReportedAddress );
		
		if ( pMemoryNode )
        {		
		    pMemoryNode->AllocationType = type;
		    pMemoryNode->nActualSize = nActualSize;
		    pMemoryNode->nReportedSize = size;
		    pMemoryNode->pReportedAddress = pReportedAddress;
		    pMemoryNode->pActualAddress = pAddress;
		    nAllocationID++;
		    if( m_breakOnID == nAllocationID )
			    DebugBreak();
		    pMemoryNode->nAllocationID = nAllocationID;
		    
		    pMemoryNode->nLineNumber = line;
		    strncpy( pMemoryNode->szSourceFile, file, sizeof( pMemoryNode->szSourceFile ) - 1 );
		    pMemoryNode->szSourceFile[ sizeof( pMemoryNode->szSourceFile ) - 1] = 0;
		 }   

		// Initialize leading and trailing padding with
		// a known value.
		unsigned long *pPad = (unsigned long *) pAddress;
		for( int i = 0; i < NUM_PAD_LONGS; ++i )
			pPad[i] = PAD_VALUE;
		
		pPad = (unsigned long *) ( (char *) pAddress + nActualSize ) - NUM_PAD_LONGS;
		for( int i = 0; i < NUM_PAD_LONGS; ++i )
			pPad[i] = PAD_VALUE;
		
		pAddress = pReportedAddress;
	}

	return pAddress;		
}

static const TCHAR *MMTYPEToString( CMemoryManager::MMTYPE type )
{
	const TCHAR *pStrings[] =
	{
		_T( "new" ),
		_T( "new[]" ),
		_T( "malloc" ),
		_T( "calloc" ),
		_T( "realloc" ),
		_T( "free" ),
		_T( "delete" ),
		_T( "delete[]" ),
	};
	
	return pStrings[type]; 
};

void CMemoryManager::FreeMemory( const char *file, int line, void *ptr, MMTYPE type )
{
	MMTYPE typeExpected;
	if( ptr )
	{
		MemoryNode *pNode = RemoveMemoryNode( ptr );
		
		if( pNode )
		{
			switch( pNode->AllocationType )
			{
				case MM_NEW:
					typeExpected = MM_DELETE;
					break;
					
				case MM_NEW_ARRAY:
					typeExpected = MM_DELETE_ARRAY;
					break;
					
				case MM_MALLOC:
				case MM_CALLOC:
					typeExpected = MM_FREE;
					break;
					
				default:
					typeExpected = MM_FREE;
					break;			
			}
			
			if( typeExpected != type )
			{
				m_bStarted = false;
				_DPF( _T( "\n" ) );
				_DPF( _T( "%s(%i) : Deallocation Type Error.\n" ), file, line );
				_DPF( _T( "    Address allocated with %s being freed using %s.  Expected %s\n" ), MMTYPEToString( pNode->AllocationType ), MMTYPEToString( type ), MMTYPEToString( typeExpected ) );
				_DPF( _T( "\n" ) );
				m_bStarted = true;
			}
			
			// Set ptr to the actual address.	
			ptr = pNode->pActualAddress;
			
			// CHECK FOR BOUNDARY VIOLATIONS HERE
			unsigned long *pBoundsCheck = (unsigned long *) ptr;
			for( int i = 0; i < NUM_PAD_LONGS; ++i )
			{
				if( pBoundsCheck[i] != PAD_VALUE )
				{
					_DPF( _T( "\n" ) );
					_DPF( _T( "%s(%i) : Boundary Violation, Damage before memory block 0x%8.8x\n" ), file, line, pNode->pReportedAddress );
					_DPF( _T( "    Allocation Size: %u\n" ), pNode->nReportedSize );
					_DPF( _T( "    \n" ), ptr );
					break;
				}
			}
			
			pBoundsCheck = (unsigned long *) ( (char *) ptr + pNode->nActualSize ) - NUM_PAD_LONGS;
			for( int i = 0; i < NUM_PAD_LONGS; ++i )
			{
				if( pBoundsCheck[i] != PAD_VALUE )
				{
					_DPF( _T( "\n" ) );
					_DPF( _T( "%s(%i) : Boundary Violation, Damage after memory block 0x%8.8x\n" ), file, line, pNode->pReportedAddress );
					_DPF( _T( "    Allocation Size: %u\n" ), pNode->nReportedSize );
					_DPF( _T( "\n" ) );
					break;
				}
			}
		}
			
		// Free the memory
		free( ptr );
		
		// Delete the memory node
		free( pNode );
	}
}

static const char *__file = 0;
static int __line = 0;

void CMemoryManager::SetOwner( const char *file, int line )
{
	__file = file;
	__line = line;
}

void CMemoryManager::GetOwner( const char **file, int *line )
{
	*file = __file;
	*line = __line;
}
#endif // _DEBUG

#pragma warning( pop )