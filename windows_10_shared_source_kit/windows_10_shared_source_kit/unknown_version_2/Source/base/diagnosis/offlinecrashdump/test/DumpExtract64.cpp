/*++

    Copyright (C) Microsoft. All rights reserved.

Module Name:
   dumpextract64.cpp

Environment:
   User Mode

Author:
   radutta 
--*/
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <winioctl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strsafe.h>
#include "common.h"
#include "DbgClient.h"
#include "DiskUtil.h"
#include "DumpUtil.h"
#include "apreg64.h"

NTSTATUS
GetDumpHeader64(
_Inout_ PDMP_CONTEXT Context
)
/*++

Routine Description:

This function uses variable services to get the OfflineMemoryDumpOsData value, retrieve the
DUMP_HEADER64, and validate that the header is good.

The following items are checked.
1. DUMP_HEADER64.Signature
2. DUMP_HEADER64.ValidDump

Arguments:

Context - Dmp_CONTEXT

IsHeaderValid - Informs the caller whether to trust the header or not.

Return Value:

NT status code.

--*/
{
    NTSTATUS      status = STATUS_UNSUCCESSFUL;
    BOOL          result = FALSE;
    LARGE_INTEGER dumpHeaderAddress;

    if (0 == Context->DumpHeaderAddress.QuadPart){
        LogLibInfoPrintf(L"DumpHeader64 adress is invalid!");
        goto Exit;
    }

    LogLibInfoPrintf(L"DumpHeader64 address is 0x%llx\r\n", Context->DumpHeaderAddress.QuadPart);

    //
    // Allocate a buffer to store the dump header. 
    //
    Context->DumpHeader64 = (PDUMP_HEADER64)malloc(sizeof(DUMP_HEADER64));

    if (Context->DumpHeader64 == NULL) {
        LogLibInfoPrintf(L"Failed to allocate memory for DUMP_HEADER64. Size: 0x%llx\r\n",
            sizeof(DUMP_HEADER64));
        status = STATUS_NO_MEMORY;
        goto Exit;
    }

    ZeroMemory(Context->DumpHeader64, sizeof(DUMP_HEADER64));

    dumpHeaderAddress.QuadPart = Context->DumpHeaderAddress.QuadPart;
    result = ReadDisk(Context->hDisk,
        Context->DumpHeader64,
        sizeof(DUMP_HEADER64),
        &dumpHeaderAddress);

    if (FALSE == result){
        LogLibErrorPrintf(
            result,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__, L"Failed to read pre-built Dump header Status:0x%llx\r\n", status);
        goto Exit;
    }

    if (Context->DumpHeader64->Signature != DUMP_SIGNATURE64){
        LogLibInfoPrintf(L"Invalid DUMP_HEADER64.Signature. Expected: 0x%llx Actual:0x%llx\r\n",
            DUMP_SIGNATURE64,
            Context->DumpHeader64->Signature);
        status = STATUS_BAD_DATA;
        goto Exit;
    }

    if (Context->DumpHeader64->ValidDump != DUMP_VALID_DUMP64) {
        LogLibInfoPrintf(L"Invalid DUMP_HEADER64.ValidDump. Expected: 0x%llx Actual:0x%llx\r\n",
            DUMP_VALID_DUMP64,
            Context->DumpHeader64->ValidDump);
        status = STATUS_BAD_DATA;
        goto Exit;
    }
    status = STATUS_SUCCESS;

Exit:

    return status;
}


NTSTATUS
ValidateDDRAgaisntPhysicalMemoryBlock64(
_Inout_ PDMP_CONTEXT Context
)
/*++

Routine Description:

This function validates that the memory described in the DUMP_HEADER.PhysicalMemoryBlock
is contained within the DDR sections and checks that memory size described by PhysicalMemoryBlock
is at the most, the same as DDR sections.

The following are verified.

1. Total memory size described by the number of pages is the same or less than
the amount of memory described by the DDR sections.
2. Each memory run must be contained in contiguous DDR sections.

Arguments:

Context - PDMP_CONTEXT

Return Value:

NT status code.

--*/
{
    UINT64                          endPD;
    UINT32                          indexDDR;
    UINT32                          indexPD;
    UINT64                          startPD;
    UINT32                          spanCount;
    NTSTATUS                        status = STATUS_UNSUCCESSFUL;
    BOOLEAN                         startInSection;
    BOOLEAN                         endInSection;

    PDDR_MEMORY_MAP                 ddrMemoryMap = Context->DDRMemoryMap;

    PPHYSICAL_MEMORY_DESCRIPTOR64   physDesc = (PPHYSICAL_MEMORY_DESCRIPTOR64)(&(Context->DumpHeader64->PhysicalMemoryBlock));

    UINT64                          memorySizeFromPD = physDesc->NumberOfPages * PAGE_SIZE;

    Context->SizeAccordingToMemoryDescriptors = memorySizeFromPD;

    if (memorySizeFromPD > Context->TotalDDRSizeInBytes) {
        LogLibInfoPrintf(L"Memory size from memory block (0x%p) is larger than DDR size (0x%p)\r\n",
            memorySizeFromPD,
            Context->TotalDDRSizeInBytes);
        status = STATUS_BAD_DATA;
        //goto Exit;
    }

    for (indexPD = 0; indexPD < physDesc->NumberOfRuns; indexPD++) {
        startPD = (UINT64)(physDesc->Run[indexPD].BasePage * PAGE_SIZE);
        endPD = startPD + (UINT64)(physDesc->Run[indexPD].PageCount * PAGE_SIZE) - 1;
        spanCount = 0;
        startInSection = FALSE;
        endInSection = FALSE;

        LogLibInfoPrintf(L"Looking at descriptor 0x%llx\r\n", indexPD);

        for (indexDDR = 0; indexDDR < Context->DDRSectionCount; indexDDR++) {
            if ((spanCount > 0) &&
                (!ddrMemoryMap[indexDDR].Contiguous))
            {
                //
                // We got a problem. We got a run that spans 
                // DDR sections but we are hitting a DDR section 
                // which don't have memory that's contiguous with 
                // previous. 
                //
                LogLibInfoPrintf(L"Error verifying memory run. Run is spanning discontiguous DDR sections.\r\n");
                LogLibInfoPrintf(L"Run Index: 0x%llx\r\n");
                LogLibInfoPrintf(L"DDR Index: 0x%llx\r\n", indexDDR);
                LogLibInfoPrintf(L"Previous DDR Index: 0x%llx\r\n", (indexDDR - 1));

                status = STATUS_BAD_DATA;
                goto Exit;
            }

            //
            // Is startPD in this DDR section?
            //
            if ((startPD >= ddrMemoryMap[indexDDR].Base) &&
                (startPD <= ddrMemoryMap[indexDDR].End))
            {
                startInSection = TRUE;
                //
                // Does endPD also fall into this section?
                //
                if ((endPD >= ddrMemoryMap[indexDDR].Base) &&
                    (endPD <= ddrMemoryMap[indexDDR].End))
                {
                    //
                    // Range is within this section. 
                    //
                    endInSection = TRUE;
                    break;
                }
                else
                {
                    //
                    // Range is not contained in this section. 
                    // Move startPD to the end plus 1. 
                    //
                    startPD = ddrMemoryMap[indexDDR].End + 1;
                    spanCount++;
                }
            }
        } //indexDDR

        //
        // Check if the start or the end of a run fell within any DDR section.
        //
        if (!startInSection)
        {
            LogLibInfoPrintf(L"The start (0x%I64x) of run 0x%llx does not fall into any DDR section.\r\n",
                (UINT64)(physDesc->Run[indexPD].BasePage * PAGE_SIZE),
                indexPD);
            status = STATUS_BAD_DATA;
            goto Exit;
        }

        if (!endInSection)
        {
            LogLibInfoPrintf(L"The end (0x%I64x) of run 0x%llx does not fall into any DDR section.\r\n",
                endPD,
                indexPD);
            status = STATUS_BAD_DATA;
            goto Exit;
        }

        LogLibInfoPrintf(L"Descriptor 0x%llx checked out.\r\n", indexPD);
    }//indexPD

    status = STATUS_SUCCESS;
Exit:
    return status;
}


NTSTATUS
WriteDumpHeader64(
_Inout_ PDMP_CONTEXT Context
)
/*++

Routine Description:

This function writes DUMP_HEADER to dump file.

Arguments:

Context - Pointer to the global context structure.

Return Value:

NT status code.

--*/
{
    UINT32                  dataSize = 0;
    NTSTATUS                status = STATUS_UNSUCCESSFUL;
	IO_STATUS_BLOCK         statusBlock = { 0 };
	LARGE_INTEGER           dedicatedDumpFileOffset = { 0 };

    Context->DedicatedDumpFileOffset.QuadPart = 0;
    
    //
    // Update the DUMP_HEADER64
    //
	LogLibInfoPrintf(L"Updating DUMP_HEADER64 before it is written to dump.\r\n");
	
	LogLibInfoPrintf(L"Changing value of \"Context->DumpHeader64->RequiredDumpSpace\" from 0x%llx to 0x%llx\n", Context->DumpHeader64->RequiredDumpSpace, Context->ActualDumpFileUsedInBytes);
    Context->DumpHeader64->RequiredDumpSpace = Context->ActualDumpFileUsedInBytes;

    if ((Context->SVSectionCount > 0) || (Context->CPUContextSectionCount > 0)) {
		LogLibInfoPrintf(L"Changing value of \"Context->DumpHeader64->SecondaryDataState\" from 0x%llx to 0x%llx\n", Context->DumpHeader64->SecondaryDataState, STATUS_SUCCESS);
		Context->DumpHeader64->SecondaryDataState = STATUS_SUCCESS;
    }

    //
    // Write the header to dump.
    //
    dataSize = sizeof(DUMP_HEADER64);
    dedicatedDumpFileOffset.QuadPart = Context->DedicatedDumpFileOffset.QuadPart;

	LogLibInfoPrintf(L"   dataSize=0x%x (%d)", dataSize, dataSize);
	LogLibInfoPrintf(L"   dedicatedDumpFileOffset=0x%x", dedicatedDumpFileOffset);
	LogLibInfoPrintf(L"   dedicatedDumpFileOffset.QuadPart=0x%x (%d)", dedicatedDumpFileOffset.QuadPart, dedicatedDumpFileOffset.QuadPart);

    status = NtWriteFile(Context->DedicatedDumpHandle,
        NULL,
        NULL,
        NULL,
        &statusBlock,
        Context->DumpHeader64,
        dataSize,
        &dedicatedDumpFileOffset,
        NULL);
	LogLibInfoPrintf(L"   NtWriteFile() returned status 0x%x (%d)", status, status);

    if (!NT_SUCCESS(status)) {
        LogLibErrorPrintf(
            E_FAIL,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__,
            L"Error:  Failed to write DumpHeader64%d \n",
            GetLastError());
        goto Exit;
    }

    NtFlushBuffersFile(Context->DedicatedDumpHandle, &statusBlock);

    Context->DedicatedDumpFileOffset.QuadPart += dataSize;

    //
    // Save the starting offset for DDR for easier future access.
    //
    Context->DDRFileOffset = Context->DedicatedDumpFileOffset;

    status = STATUS_SUCCESS;

Exit:

    return status;
}

NTSTATUS
WriteDDR64(
_Inout_ PDMP_CONTEXT Context
)
/*++

Routine Description:

This function writes DDR sections based on the DUMP_HEADER64's physical memory descriptor
to dump file.

Arguments:

Context - Pointer to the global context structure.

Return Value:

NT status code.

--*/
{
	LARGE_INTEGER   basePA = { 0 };
    ULONG64         PageRemain = 0;
    LARGE_INTEGER   bytesWritten = { 0 };
    UINT32          io = 0;
    UINT32          ioCountPerRun = 0;
	LARGE_INTEGER   runSize = { 0 };
	LARGE_INTEGER   startPA = { 0 };
    UINT32          ioSize = 0;
    ULONG           buffersize = DEFAULT_DMP_BUF_SZ;
    NTSTATUS        status = STATUS_UNSUCCESSFUL;
	IO_STATUS_BLOCK statusBlock = { 0 };
	LARGE_INTEGER   dedicatedDumpFileOffset = { 0 };


    //
    // Allocate the intermediate buffer to read memory from DDR section
    // to the dump file.
    //
    PVOID           tempBuffer = malloc(buffersize);
	if (tempBuffer == nullptr) {
        LogLibInfoPrintf(L"Unable to allocate 0x%llx bytes buffer for writing DDR memory to dump.\r\n", buffersize);
        status = STATUS_NO_MEMORY;
        goto Exit;
    }
	LogLibInfoPrintf(L"[INFO] Successfully allocated 0x%llx bytes of buffer to write DDR memory to dump.\r\n", buffersize);

    ZeroMemory(tempBuffer, buffersize);

    for (UINT32 index = 0; index < Context->DumpHeader64->PhysicalMemoryBlock.NumberOfRuns; index++)  {
		LogLibInfoPrintf(L"=== === === ===  Run %d  === === ===", index);

        basePA.QuadPart = (Context->DumpHeader64->PhysicalMemoryBlock.Run[index].BasePage * PAGE_SIZE);
        PageRemain = Context->DumpHeader64->PhysicalMemoryBlock.Run[index].PageCount;
        runSize.QuadPart = (Context->DumpHeader64->PhysicalMemoryBlock.Run[index].PageCount *PAGE_SIZE);
        ioCountPerRun = (UINT32)(runSize.QuadPart / buffersize);

        if ((runSize.QuadPart % buffersize) > 0) {
            ioCountPerRun++;
        }

		LogLibInfoPrintf(L"0x%x IOs will be required for this run.", ioCountPerRun);
		LogLibInfoPrintf(L"     basePA.QuadPart=0x%llx",  basePA.QuadPart );
		LogLibInfoPrintf(L"          PageRemain=0x%08x",  PageRemain );
		LogLibInfoPrintf(L"    runSize.QuadPart=0x%08x",  runSize.QuadPart );
		LogLibInfoPrintf(L"       ioCountPerRun=0x%08x",  ioCountPerRun );

        for (io = 0; io < ioCountPerRun; io++) {
            startPA.QuadPart = basePA.QuadPart + io * ioSize;

            ioSize = (UINT32)((PageRemain * PAGE_SIZE)< buffersize ? PageRemain * PAGE_SIZE : buffersize);
			LogLibInfoPrintf(L"  -- I/O 0x%08x: startPA.QuadPart=0x%016x, ioSize=0x%016x", startPA.QuadPart, ioSize );

            status = ReadFromDDRSectionByPhysicalAddress(Context,
                                                        startPA,
                                                        ioSize,
                                                        tempBuffer);
            if (!NT_SUCCESS(status)) {
                LogLibInfoPrintf(L"[0x%llx] Failed to read from DDR sections. Status: 0x%llx\r\n",
                    io,
                    status);
                goto Exit;
            }

            dedicatedDumpFileOffset.QuadPart = Context->DedicatedDumpFileOffset.QuadPart;
            status = NtWriteFile(Context->DedicatedDumpHandle,
                NULL,
                NULL,
                NULL,
                &statusBlock,
                tempBuffer,
                ioSize,
                &dedicatedDumpFileOffset,
                NULL);
            if (!NT_SUCCESS(status)) {
                LogLibInfoPrintf(L"Failed to Write to file! ");
                goto Exit;

            }
            NtFlushBuffersFile(Context->DedicatedDumpHandle, &statusBlock);
            bytesWritten.QuadPart += ioSize;
            Context->DedicatedDumpFileOffset.QuadPart += ioSize;
            PageRemain -= ioSize / PAGE_SIZE;
			LogLibInfoPrintf(L"  	            bytesWritten=0x%llx, Context->DedicatedDumpFileOffset.QuadPart=0x%lld, 0x%llx Pages remain\n", bytesWritten, Context->DedicatedDumpFileOffset.QuadPart, PageRemain);
        }// for io

        LogLibInfoPrintf(L"**** Finished writing run 0x%llx. ****\r\n", index);
    }//for index

    if ((ULONG64)(bytesWritten.QuadPart / PAGE_SIZE) != Context->DumpHeader64->PhysicalMemoryBlock.NumberOfPages) {
        LogLibErrorPrintf(
            E_FAIL,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__,
            L" Wrote Page not match as expected\n");
        goto Exit;
    }
    status = STATUS_SUCCESS;

Exit:

    if (tempBuffer != nullptr) {
        free(tempBuffer);
        tempBuffer = nullptr;
    }

    return status;
}

NTSTATUS
GetKdDebuggerDataBlock64(
_Inout_ PDMP_CONTEXT Context
)
/*++

Routine Description:

This function attempts to determine if KdDebuggerDataBlock is encoded or not by
performing the following checks.

1. Read the value pointed to by Context->OfflineDumpKDBEP
2. Or check the DBGKD_DEBUG_DATA_HEADER64.Size value.

Arguments:

Context - PDMP_CONTEXT

Return Value:

NT status code.

--*/
{
    UINT32                      dataSize;
    PKDDEBUGGER_DATA64          dbgDataBlock;
    LARGE_INTEGER               decodedBlockPA;
    ULONG64                     dbgDataHeaderVA;
    NTSTATUS                    status = STATUS_UNSUCCESSFUL;
    BOOL                        result = FALSE;
    HRESULT                     hr = E_FAIL;

    LogLibInfoPrintf(L"Checking DBGKD_DEBUG_DATA_HEADER64.Size\r\n");
    Context->KdDebuggerDataBlockPA.QuadPart = ADDRESS_NOT_PRESENT;

    //
    // Read DBGKD_DEBUG_DATA_HEADER64
    // 
    dbgDataHeaderVA = Context->DumpHeader64->KdDebuggerDataBlock;
    dataSize = sizeof(KDDEBUGGER_DATA64);
    dbgDataBlock = (PKDDEBUGGER_DATA64)malloc(dataSize);
    if (dbgDataBlock == nullptr) {
        LogLibInfoPrintf(L"Failed to allocated 0x%llx bytes for KdDebuggerDataBlock\r\n", dataSize);
        status = STATUS_NO_MEMORY;
        goto Exit;
    }
    ZeroMemory(dbgDataBlock, dataSize);

    IDebugSymbols3 *DebugSymbols = DbgClient::GetSymbols();
    IDebugDataSpaces2 *DebugDataSpaces = DbgClient::GetDataSpaces();

    if ((DebugSymbols == nullptr) || (DebugDataSpaces == nullptr))
    {
        LogLibInfoPrintf(L"Could not get GetDataSpaces, KdDebuggerDataBlock may be unreadable\n\r");
        goto Exit;
    }        
    
    hr = DebugDataSpaces->ReadVirtual(dbgDataHeaderVA, dbgDataBlock, sizeof(KDDEBUGGER_DATA64), NULL);
    if (FAILED(hr)) {
        LogLibInfoPrintf(L"Failed to read nt!KdDebuggerDataBlock Error 0x%x\n\r", hr);
         goto Exit;
    }
    
    //
    // Check the size.
    //
    LogLibInfoPrintf(L"DBGKD_DEBUG_DATA_HEADER64.Size is: 0x%llx\r\n", dbgDataBlock->Header.Size);

    if (dbgDataBlock->Header.Size < 0x10000 || ValidateKdDebuggerDataBlock(&(dbgDataBlock->Header))) {
        LogLibInfoPrintf(L"KdDebuggerDataBlock appears to be valid and decoded. Read it in to memory.\r\n");
    }

    else {
        LogLibInfoPrintf(L"The KdDebuggerDataBlock appears to be encoded \r\n");

        //
        // As ARM platform DumperHeader size is PageSize so 
        // directly use PAGE_SIZE
        //
        decodedBlockPA.QuadPart = Context->DumpHeaderAddress.QuadPart + PAGE_SIZE;
        LogLibInfoPrintf(L"Decoded version of KdDebuggerDataBlock should be at 0x%I64x \r\n", decodedBlockPA.QuadPart);

        //directly read to buffer
        result = ReadDisk(Context->hDisk,
            dbgDataBlock,
            dataSize,
            &decodedBlockPA);
        if (result == FALSE) {
            LogLibInfoPrintf(L"Read Decoded Debug Block failed. Status:0x%x\r\n", status);
            goto Exit;
        }
    }

    Context->KdDebuggerDataBlock = dbgDataBlock;
    status = STATUS_SUCCESS;
    
Exit:
    return status;
}

NTSTATUS
ReadFromDDRSectionByVirtualAddress64(
_In_ PDMP_CONTEXT Context,
_In_ UINT64 VirtualAddress,
_In_ UINT64 Length,
_Out_ PVOID Buffer,
_Out_opt_ PLARGE_INTEGER PhysicalAddress
)
/*++

Routine Description:

This function reads the contents of memory in DDR sections based on a virtual address.

Arguments:

Context - DMP_CONTEXT

VirtualAddress - Virtual address of memory in DDR sections which we want
to read from.

Length - Number of bytes to read.

Buffer - Buffer holding the contents of the read.

PhysicalAddress - Optional. Returns the physical address.

Return Value:

NT status code.

--*/
{
    UINT64              endVA;
    UINT64              nextPageVA;
    LARGE_INTEGER       physicalAddress;
    NTSTATUS            status = STATUS_UNSUCCESSFUL;

    //
    // Validate if we are crossing page boundary. If we are
    // return STATUS_INVALID_PARAMETER. 
    //
    nextPageVA = VirtualAddress + 0x1000;
    endVA = VirtualAddress + Length - 1;

    if (PhysicalAddress != NULL) {
        PhysicalAddress->QuadPart = ADDRESS_NOT_PRESENT;
    }

    if (endVA >= nextPageVA) {
        LogLibInfoPrintf(L"The read crosses page boundary. EndVA: 0x%llx\r\n",
            endVA);
        status = STATUS_INVALID_PARAMETER;
        goto Exit;
    }

    //
    // Get the physical address.
    //
    status = VirtualToPhysical64(Context,
        VirtualAddress,
        &physicalAddress);

    if (!NT_SUCCESS(status)) {
        LogLibInfoPrintf(L"Failed to convert virtual address 0x%llx to physical address. Status: 0x%llx\r\n",
            VirtualAddress,
            status);

        goto Exit;
    }

    if (PhysicalAddress != NULL) {
        PhysicalAddress->QuadPart = physicalAddress.QuadPart;
    }

    //
    // Read this from DDR
    //
    status = ReadFromDDRSectionByPhysicalAddress(Context,
        physicalAddress,
        Length,
        Buffer);

    if (!NT_SUCCESS(status)) {
        LogLibInfoPrintf(L"Failed to read %u bytes from physical address 0x%I64x. Status: 0x%llx\r\n",
            Length,
            physicalAddress.QuadPart,
            status);

        goto Exit;
    }

    LogLibInfoPrintf(L"Read %u bytes from virtual address 0x%llx (PA: 0x%I64x).\r\n",
        Length,
        VirtualAddress,
        physicalAddress.QuadPart);
    status = STATUS_SUCCESS;

Exit:

    return status;
}

NTSTATUS
ExtractWindowsDumpFile64(PDMP_CONTEXT Context)
/*--
Extract Windows Dump file
it will
1. search in memory Dump header
2. validate DDR
3. Init Dump file
4. get debug block data
5. write dump header to dump file
6. write DDR to dump file
7. update dump file will cpu context
++*/
{
    NTSTATUS    status = STATUS_UNSUCCESSFUL;
    HRESULT     result = ERROR_SUCCESS;

    LogLibInfoPrintf(L"=========== Getting the pre-built DUMP_HEADER64. ===========\r\n");
    status = GetDumpHeader64(Context);
    if (FAILED(status)) {
        LogLibErrorPrintf(
            E_FAIL,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__,
            L"Error: Failed to Get Dump Header\n");
        goto Exit;
    }
    
    //
    // Change Bugcheck Parameter 2 to 0xFFFFFFFF to indicate the dump have no AP_REG
    //
    if (Context->HasValidAP_REG != TRUE) {
        Context->DumpHeader64->BugCheckParameter2 = INVALID_AP_REG;
    }
    
    LogLibInfoPrintf(L"=========== Validating DUMP_HEADER64's memory descriptors against DDR sections. ===========\r\n");
    status = ValidateDDRAgaisntPhysicalMemoryBlock64(Context);

    if (FAILED(status)) {
        LogLibErrorPrintf(
            E_FAIL,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__,
            L"Error: Failed ValidateDDRAgaisntPhysicalMemoryBlock64 \n");
        goto Exit;
    }

    LogLibInfoPrintf(L"=========== Init the Windows 64 dump file ===========\r\n");
    result = InitDumpFile(Context);
    if (result != ERROR_SUCCESS){
        LogLibErrorPrintf(
            result,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__,
            L"Error: Failed to InitDumpFile : %d\n",
            GetLastError());
        goto Exit;
    }

    LogLibInfoPrintf(L"=========== Writing DUMP_HEADER 64 to the dump file. ===========\r\n");
    status = WriteDumpHeader64(Context);
    if (FAILED(status)) {
        LogLibErrorPrintf(
            E_FAIL,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__,
            L"Error: Failed WriteDumpHeader64 \n");
        goto Exit;
    }

    LogLibInfoPrintf(L"=========== Writing DDR section to the dump file. ===========\r\n");
    status = WriteDDR64(Context);
    if (FAILED(status)) {
        LogLibErrorPrintf(
            E_FAIL,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__,
            L"Error: Failed WriteDDR64 \n");
        goto Exit;
    }
  LogLibInfoPrintf(L"=========== Trying to load DbgEng.dll to understand virtual memory. ===========\r\n");
    
    if (!DbgClient::Initialize(Context->DedicatedDumpFilePath, L"cache*c:\\symbols;srv*http://symweb;")) {
        LogLibErrorPrintf(
            E_FAIL,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__,
            L"Error: Failed to initialize Debug Client \n");
            goto Exit;
    }        
    
    LogLibInfoPrintf(L"=========== Try and determine if KdDebuggerDataBlock is encoded. ===========\r\n");
    status = GetKdDebuggerDataBlock64(Context);
    if (!NT_SUCCESS(status)) {
        LogLibErrorPrintf(
            E_FAIL,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__,
            L"Error:GetKdDebuggerDataBlock64 \n");
   //     goto Exit;
    }

    if(Context->IsAPREGRequested != FALSE) {
        LogLibInfoPrintf(L"=========== Update CPU context ==============================================\r\n");
        switch (Context->DumpHeader64->MachineImageType) {
           case IMAGE_FILE_MACHINE_ARM:
            case IMAGE_FILE_MACHINE_ARM64:
                if (Context->HasValidAP_REG == TRUE) {                
                    if(Context->isAPREG64 == TRUE && Context->IsAPREGRequested == TRUE) {                    
                          LogLibInfoPrintf(L" ====== Updating context with APREG64 . ===========\r\n");
                        result = UpdateContextWithAPReg64(Context);
                        if (FAILED(result)) {
                            LogLibErrorPrintf(
                                E_FAIL,
                                __LINE__,
                                WIDEN(__FUNCTION__),
                                __WFILE__,
                                L"Error: Failed UpdateContextWithAPReg 64 \n");
                            goto Exit;
                        }
                    }
                }
                break;
            default:
                LogLibErrorPrintf(
                        E_FAIL,
                        __LINE__,
                        WIDEN(__FUNCTION__),
                        __WFILE__,
                        L"Error:Unsupported CPU Context tyep \n");
        }
    }
    LogLibInfoPrintf(L"Write to Dump file successfully.\n");

Exit:
    return status;

}
#define MI_IS_NATIVE_PTE_VALID(PTE) ((PTE)->Valid == 1)

NTSTATUS
VirtualToPhysical64(
_In_ PDMP_CONTEXT Context,
_In_ UINT64 VirtualAddress,
_Out_ PLARGE_INTEGER PhysicalAddress
)
/*++

Routine Description:

This function determines the physical address given a virtual address.
This only works for 4K pages.

Arguments:

Context - DMP_CONTEXT

VirtualAddress - The virtual address of some memory in the DDR sections.

PhysicalAddress - Physical address of memory in DDR sections which we want
to read from.

Return Value:

NT status code.

--*/
{  
    NTSTATUS      status = STATUS_UNSUCCESSFUL;
    











































































































    
    return status;
}
