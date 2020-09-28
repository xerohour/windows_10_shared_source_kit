// Copyright(C) Microsoft.All rights reserved.

// Fancy allocator
LPVOID GuardPageAllocate(ULONG nBytes);
void GuardPageFree(LPVOID pBlock);

// Trivial with checks and protection
// of our code
BOOL StaticAllocatorInit();
LPVOID StaticMemoryAllocAligned(ULONG Alignment, ULONG cbSize, BOOL bUnaligned = FALSE);
LPVOID StaticMemoryAllocUnaligned(ULONG cbSize);
BOOL StaticMemoryCheckValidity(LPVOID lpBlock);
void StaticMemoryInit();
