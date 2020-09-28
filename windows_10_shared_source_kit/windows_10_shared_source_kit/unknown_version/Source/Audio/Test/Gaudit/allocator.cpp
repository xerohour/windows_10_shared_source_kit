//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
// Copyright(C) Microsoft.All rights reserved.
//
//  File:       allocator.cpp
//
//
//--------------------------------------------------------------------------

#include "common.h"


// That's a nice allocator like the page heap feature
// in avrf it can be used to catch all sorts of buffer
// problems 

// Allocate 
//-------------------------------------------------------------------------
// Method: GuardPageAllocate
//
// Description:
//      it allocates memory of the asked bytes. It also make sure that next page of allocated memory must be read/write protected so that we can catch buffer problems.
//      it also stores length of allocated bytes which is used in case of reeing guard page  
// Parameters:
//
//      ULONG nBytes
//          No of bytes allocated
//
// Return values (LPVOID):
//          Pointer to allocated memory
//
LPVOID GuardPageAllocate(ULONG nBytes)
{
    if (nBytes >= PAGE)
        return NULL;
    //allocate 2 page of memory with read write access. Fill this bolck (2 page) with 0xBA. Then disable access to second Page. 
    // retrun pointer which points to asked block in the first page starting at PAGE-asked bytes
    LPVOID lpBlock = VirtualAlloc(NULL, 2 * PAGE, MEM_COMMIT, PAGE_READWRITE);
    if (!lpBlock)
        return NULL;
    memset(lpBlock, 0xBA, 2 * PAGE);
    DWORD dwoldprotect;
    VirtualProtect((LPBYTE)lpBlock + PAGE, PAGE, PAGE_NOACCESS, &dwoldprotect);
    LPVOID  lpBlockStart  = (LPBYTE)lpBlock + PAGE - nBytes;
    LPDWORD lpBlockLength = (LPDWORD)((LPBYTE)lpBlockStart - sizeof(DWORD));
    *lpBlockLength = nBytes;        //it is used in case of freeing guard page

    return lpBlockStart;
}

// Free
//-------------------------------------------------------------------------
// Method: GuardPageFree
//
// Description:
//      it frees the memory allocated by GuardPageAllocate function.
//
// Parameters:
//
//      LPVOID pBlock
//          Block starting address to be freed
//
// Return values (VOID):
//          

void GuardPageFree(LPVOID pBlock)
{
    LPDWORD lpBlockLength = (LPDWORD)((LPBYTE)pBlock - sizeof(DWORD));  //get the length of the block - stored during GuardPageAllocate
    LPVOID  pBlockStart   = (LPBYTE)pBlock + *lpBlockLength - PAGE;                 // get pointer to to starting of page in which this block was allocated
    DWORD dwoldprotect;
    VirtualProtect((LPBYTE)pBlockStart, 2*PAGE, PAGE_READWRITE, &dwoldprotect);
    VirtualFree(pBlockStart, 0, MEM_RELEASE);
}


// That is the easiest allocator that I can think of
// such that we don't allocate memory all the time
// and we catch over/under buffer 
LPVOID gpHeapMemory = NULL;

// Call once
//-------------------------------------------------------------------------
// Method: StaticAllocatorInit
//
// Description:
//      Allocated 2 pages block memory if it is not allocated.
//      If memory is allocated then it stoes starting pointer at gpHeapMemory - which is global variable for allocator.cpp
//
// Parameters:
//
//      
//
// Return values (BOOL):
//      TRUE - if memory already allocated or memory allocated successfuly
//      FALSE - if memory can not be allocated
//
BOOL StaticAllocatorInit()
{
    if (gpHeapMemory)
        return TRUE;
    return ((gpHeapMemory = LocalAlloc(0, 2 * PAGE))!=NULL); 
}

// Call before every allocation
//-------------------------------------------------------------------------
// Method: StaticMemoryInit
//
// Description:
//      Fill 2 page memory block allocated by StaticAllocatorInit with 0xBA.
//
// Parameters:
//
// Return values (void):
//
void StaticMemoryInit()
{
    memset(gpHeapMemory, 0xBA, 2 * PAGE); 
}

// Allocate aligned buffer
//-------------------------------------------------------------------------
// Method: StaticMemoryAllocAligned
//
// Description:
//
// Parameters:
//
//      ULONG nBytes
//          No of bytes allocated
//
// Return values (LPVOID):
//          Pointer to allocated memory
//
LPVOID StaticMemoryAllocAligned(ULONG Alignment, ULONG cbSize, BOOL bUnaligned )
{
  
    if ( !cbSize  || Alignment < 4 )
    {
        return NULL;
    }


    ULONG   cbAlignSize          = ( cbSize + Alignment ) & ~Alignment;
    ULONG   cbExtraSize         = (bUnaligned) ? 1 : 0;
    ULONG   cbTotalSize          = cbAlignSize + cbExtraSize;
    

    //
    // the one byte is to account for the memory unaligned case in which
    // we need a byte to unalign the buffer!
    if ( cbTotalSize + sizeof(ULONG)   >  PAGE )
    {
        DebugBreak();
        return NULL;
    }
    
    LPBYTE  lpBlock          = (LPBYTE)gpHeapMemory + PAGE - cbTotalSize ;
    LPBYTE  lpBlockSize   = lpBlock - sizeof(ULONG);
    
     *(ULONG*)lpBlockSize = cbSize;

    if ( bUnaligned )
    {
        *lpBlock = 0xBB;
          lpBlock++;
    }
    
    return lpBlock;
    
}

// Allocate unaligned buffer
LPVOID StaticMemoryAllocUnaligned(ULONG cbSize)
{
    return StaticMemoryAllocAligned(7, cbSize, TRUE);
}


// Check memory valid for given block allocation
// Consider this as a replacement of free 
BOOL StaticMemoryCheckValidity(LPVOID lpBlock)
{
    if ( lpBlock == 0 || (LPBYTE)lpBlock - 1 == 0)
        return TRUE;

    LPBYTE lpInternalBlock = (LPBYTE)lpBlock;

    if (*((LPBYTE)lpBlock - 1) == 0xBB) // unaligned request
        lpInternalBlock--;
    
    ULONG cbSize = *(LPDWORD)(lpInternalBlock - sizeof(ULONG));

    if (*((LPBYTE)lpBlock - 1) == 0xBB) // unaligned request
        cbSize++;

	LPBYTE p = NULL;
    for(p = LPBYTE(gpHeapMemory); p < lpInternalBlock - sizeof(ULONG); p++)
        if (*p != 0xBA)
            return FALSE;

    for(p = lpInternalBlock + cbSize; p < (LPBYTE)gpHeapMemory + 2 * PAGE; p++)
        if (*p != 0xBA)
            return FALSE;

    return TRUE;
}
