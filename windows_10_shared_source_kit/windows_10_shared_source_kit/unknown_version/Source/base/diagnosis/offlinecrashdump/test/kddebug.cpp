/*++

    Copyright (C) Microsoft. All rights reserved.

Module Name:
   kddebug.cpp

Environment:
   User Mode

Author:
   radutta 
--*/

#include <atlbase.h>
#include <dbgeng.h>

#include <wdbgexts.h>
#include "dbgClient.h"
#include <initguid.h>
#include <WinEvt.h>
#include <objbase.h>


#define INITGUID
#include <guiddef.h>


//
// KiDecodePointer is copied from the NT kernel
//

ULONG64 KiWaitAlways;
ULONG64 KiWaitNever;

ULONG
RtlUlongByteSwap(
    __in  ULONG Source
    )
{  
    ULONG swapped;  

    swapped = ((Source)              << (8 * 3)) |
              ((Source & 0x0000FF00) << (8 * 1)) |
              ((Source & 0x00FF0000) >> (8 * 1)) |
              ((Source)              >> (8 * 3));
  
    return swapped;  
}  

ULONG
KiDecodePointer(
    _In_ ULONG Pointer,
    _In_ ULONG Salt
    )
{
    ULONG Value = Pointer;

    Value = RotateLeft32((ULONG)Value ^ (ULONG)KiWaitNever, (int)KiWaitNever);
    Value = RtlUlongByteSwap((ULONG)Value ^ (ULONG)Salt) ^ (ULONG)KiWaitAlways;

    return Value;
}

void
DecodeBlock(
    void *pvBlock,
    size_t cbBlock,
    ULONG Salt
    )
{
    ULONG *Cursor = (ULONG *)pvBlock;
    size_t Number = cbBlock / sizeof(ULONG);

    do {
        *Cursor = KiDecodePointer(*Cursor, Salt);
        Cursor += 1;
        Number -= 1;
    } while (Number != 0);
}


static KDDEBUGGER_DATA64 g_KdDebuggerDataBlock = { };

void SetDecodedKdDebuggerDataBlock(
    _In_ const KDDEBUGGER_DATA64 &KdDebuggerDataBlock)
{
    g_KdDebuggerDataBlock = KdDebuggerDataBlock;
}

const KDDEBUGGER_DATA64 *
GetDecodedKdDebuggerDataBlock(
    )
{
    if (g_KdDebuggerDataBlock.Header.Size > 0) {
        return &g_KdDebuggerDataBlock;
    }

    return NULL;
}

//
// KdDebuggerDataBlock is a structure maintained by the NT kernel and read by
// debuggers.  It's sometimes stored encoded in memory.  The bugcheck code
// would decode it before generating a dump, but in offline dumps we don't get
// a chance to execute the bugcheck code.  So raw2dump has to do the decoding
// itself.
//
BOOL
DecodeDebuggerDataBlock(
    _In_ HANDLE hDmpOut  
    )
{
    HRESULT hr;
    ULONG KdpDataBlockEncodedVirtualAddr = 0;
    BOOLEAN KdpDataBlockEncoded = FALSE;
    ULONG KiWaitAlwaysVirtualAddr = 0;
    ULONG KiWaitNeverVirtualAddr = 0;
    ULONG KdDebuggerDataBlockVirtualAddr = 0;
    ULONG64 KdDebuggerDataBlockPhysAddr = 0;
    ULONGLONG KdDebuggerDataBlockOffset = 0;
    KDDEBUGGER_DATA64 KdDebuggerDataBlock;
    ULONG64 Offset;

    IDebugSymbols3 *DebugSymbols = DbgClient::GetSymbols();
    IDebugDataSpaces2 *DebugDataSpaces = DbgClient::GetDataSpaces();

    if ((DebugSymbols == NULL) || (DebugDataSpaces == NULL)) {
        wprintf(L"Failure in dbgeng, KdDebuggerDataBlock may be unreadable\n\r");
        return TRUE;
    }

    if (FAILED(hr = DebugSymbols->Reload("nt"))) {
        wprintf(L"Failed (0x%x) to load kernel symbols, KdDebuggerDataBlock may be unreadable", hr);
        return TRUE;
    }

    //
    // Read KdpDataBlockEncoded to determine if KdDebuggerDataBlock is in fact encoded
    //
    
    #define MAX_ENTRIES 1000
    DEBUG_MODULE_AND_ID ids[MAX_ENTRIES];
    ULONG Flags = 0;
    ULONG IdsCount = MAX_ENTRIES;
    ULONG Entries = 0;
    char Buffer[100];
    ULONG StringSize = 0;
    
    if (FAILED(hr = DebugSymbols->GetSymbolEntriesByName("nt!KPR*", Flags, ids, IdsCount, &Entries))) {
         wprintf(L" not working !! \n\r");         
    }
    
    for (ULONG i = 0; i< min(Entries, MAX_ENTRIES) ; i++) {
        wprintf(L"----------------------------\n\r");
        if (FAILED(hr = DebugSymbols->GetSymbolEntryString(&(ids[i]), 0, Buffer, sizeof Buffer, &StringSize))){    
            wprintf(L" not working  2 !! \n\r");
            continue;
        }
        Buffer[StringSize] = '\0';
        printf("SYMBOL: %s ", Buffer);
        if (FAILED(hr = DebugSymbols->GetOffsetByName(Buffer, &Offset))) {
            wprintf(L"Cannot get the offset for %s", Buffer);
            continue;
        }
        wprintf(L" Offset : 0x%x\n\r", Offset);
    }
        

    if (FAILED(hr = DebugSymbols->GetOffsetByName("nt!KdDebuggerDataBlock", &Offset))) {
        wprintf(L"Can't evaluate nt!KdDebuggerDataBlock, symbols may be missing 0x%x\n\r", hr);
        wprintf(L"KdDebuggerDataBlock may be unreadable as a result\n\r");
        return TRUE;
    }

    KdpDataBlockEncodedVirtualAddr = (ULONG)Offset;

    hr = DebugDataSpaces->ReadVirtual(
            KdpDataBlockEncodedVirtualAddr,
            &KdpDataBlockEncoded,
            sizeof KdpDataBlockEncoded,
            NULL);

    if (FAILED(hr)) {
        wprintf(L"Failed to read KdpDataBlockEncoded from dump, KdDebuggerDataBlock may be unreadable Error 0x%x\n\r", hr);
        return TRUE;
    }

    if (!KdpDataBlockEncoded) {
        // Nothing to do here
        return TRUE;
    }

    //
    // Read KiWaitAlways and KiWaitNever for use in the decode routine
    //

    if (FAILED(hr = DebugSymbols->GetOffsetByName("nt!KiWaitAlways", &Offset))) {
        wprintf(L"Can't evaluate nt!KiWaitAlways Error 0x%x\n\r", hr);
        return FALSE;
    }

    KiWaitAlwaysVirtualAddr = (ULONG)Offset;

    hr = DebugDataSpaces->ReadVirtual(KiWaitAlwaysVirtualAddr, &KiWaitAlways, sizeof KiWaitAlways, NULL);

    if (FAILED(hr)) {
        wprintf(L"Failed to read nt!KiWaitAlways Error 0x%x\n\r", hr);
        return FALSE;
    }

    if (FAILED(hr = DebugSymbols->GetOffsetByName("nt!KiWaitNever", &Offset))) {
        wprintf(L"Can't evaluate nt!KiWaitNever Error 0x%x\n\r", hr);
        return FALSE;
    }

    KiWaitNeverVirtualAddr = (ULONG)Offset;

    hr = DebugDataSpaces->ReadVirtual(KiWaitNeverVirtualAddr, &KiWaitNever, sizeof KiWaitNever, NULL);

    if (FAILED(hr)) {
        wprintf(L"Failed to read nt!KiWaitNever Error 0x%x\n\r", hr);
        return FALSE;
    }

    //
    // Read KdDebuggerDataBlock into a local buffer
    //

    if (FAILED(hr = DebugSymbols->GetOffsetByName("nt!KdDebuggerDataBlock", &Offset))) {
        wprintf(L"Can't evaluate nt!KdDebuggerDataBlock Error 0x%x\n\r", hr);
        return FALSE;
    }

    KdDebuggerDataBlockVirtualAddr = (ULONG)Offset;

    hr = DebugDataSpaces->ReadVirtual(KdDebuggerDataBlockVirtualAddr, &KdDebuggerDataBlock, sizeof KdDebuggerDataBlock, NULL);
    if (FAILED(hr)) {
        wprintf(L"Failed to read nt!KdDebuggerDataBlock Error 0x%x\n\r", hr);
        return FALSE;
    }

    //
    // Decode KdDebuggerDataBlock
    //
    DecodeBlock(
        &KdDebuggerDataBlock,
        sizeof KdDebuggerDataBlock,       // sizeof(KDDEBUGGER_DATA64)
        KdpDataBlockEncodedVirtualAddr);

    // Stash this for later
    SetDecodedKdDebuggerDataBlock(KdDebuggerDataBlock);

    //
    // Write the decoded KdDebuggerDataBlock back into the dump file
    //

    hr = DebugDataSpaces->VirtualToPhysical(KdDebuggerDataBlockVirtualAddr, &KdDebuggerDataBlockPhysAddr);

    if (FAILED(hr)) {
        wprintf(L"Failed to find KdDebuggerDataBlock physical address Error 0x%x\n\r", hr);
        return FALSE;
    }

    LARGE_INTEGER p;
    p.QuadPart = KdDebuggerDataBlockPhysAddr;

 //   if (!Phys2Offset::Convert(p, &KdDebuggerDataBlockOffset)) {
 //       return FALSE;
 //   }
 // TODO: Fix the above.

    LARGE_INTEGER li;
    li.QuadPart = KdDebuggerDataBlockOffset;

    if (!SetFilePointerEx(hDmpOut, li, NULL, FILE_BEGIN)) {
        wprintf(L"Unable to write to output file 0x%x\n\r", GetLastError());
        return FALSE;
    }

  //  if (!WriteStruct(hDmpOut, KdDebuggerDataBlock)) {
  //      return FALSE;
  //  }
  // TODO: Fix the above error

    return TRUE;
}

