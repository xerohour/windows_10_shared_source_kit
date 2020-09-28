/*++

    Copyright (C) Microsoft. All rights reserved.

Module Name:
   dumpextract32.cpp

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

BOOL 
OpenDumpFiles( 
    _In_ PCWSTR pszDmpOut, 
    _Out_ HANDLE *phDmpOut
 );
 
VOID 
TestDebugEngine(PDMP_CONTEXT Context);

NTSTATUS
GetDumpHeader32(
_Inout_ PDMP_CONTEXT Context
)
/*++

Routine Description:

This function uses variable services to get the OfflineMemoryDumpOsData value, retrieve the
DUMP_HEADER, and validate that the header is good.

The following items are checked.
1. DUMP_HEADER.Signature
2. DUMP_HEADER.ValidDump

Arguments:

Context - Dmp_CONTEXT

IsHeaderValid - Informs the caller whether to trust the header or not.

Return Value:

NT status code.

--*/
{
    NTSTATUS      status = STATUS_UNSUCCESSFUL;
    BOOL          result = FALSE;
    LARGE_INTEGER dumpHeaderAddress = Context->DumpHeaderAddress;

    if (0 == Context->DumpHeaderAddress.QuadPart){
        LogLibInfoPrintf(L"DumpHeader adress is invalid!");
        goto Exit;
    }

    LogLibInfoPrintf(L"DumpHeader address is 0x%x\r\n", Context->DumpHeaderAddress.QuadPart);

    //
    // Allocate a buffer to store the dump header. 
    //
    Context->DumpHeader = (PDUMP_HEADER32)malloc(sizeof(DUMP_HEADER));

    if (Context->DumpHeader == NULL) {
        LogLibInfoPrintf(L"Failed to allocate memory for DUMP_HEADER. Size: 0x%x\r\n",
            sizeof(DUMP_HEADER));
        status = STATUS_NO_MEMORY;
        goto Exit;
    }

    ZeroMemory(Context->DumpHeader, sizeof(DUMP_HEADER));

    result = ReadDisk(Context->hDisk,
        Context->DumpHeader,
        sizeof(DUMP_HEADER),
        &dumpHeaderAddress);

    if (FALSE == result){
        LogLibErrorPrintf(
            result,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__, L"Failed to read pre-built Dump header Status:0x%x\r\n", status);
        goto Exit;
    }

    if (Context->DumpHeader->Signature != DUMP_SIGNATURE32){
        LogLibInfoPrintf(L"Invalid DUMP_HEADER.Signature. Expected: 0x%x Actual:0x%x\r\n",
            DUMP_SIGNATURE32,
            Context->DumpHeader->Signature);
        status = STATUS_BAD_DATA;
        goto Exit;
    }

    if (Context->DumpHeader->ValidDump != DUMP_VALID_DUMP32) {
        LogLibInfoPrintf(L"Invalid DUMP_HEADER.ValidDump. Expected: 0x%x Actual:0x%x\r\n",
            DUMP_VALID_DUMP32,
            Context->DumpHeader->ValidDump);
        status = STATUS_BAD_DATA;
        goto Exit;
    }
    status = STATUS_SUCCESS;

Exit:

    return status;
}


NTSTATUS
UpdateContextWithAPReg(
_Inout_ PDMP_CONTEXT Context
)
/*++

Routine Description:

This function updates CONTEXT with AP_REG.

The location of CONTEXT for a processor is stored at PRCB + OffsetPrcbContext.
To get to the above, we need to do the following:

1. Get the physical address of KdDebuggerDataBlock->KiProcessorBlock.
KiProcessorBlock is an array of pointers to the PRCB for each processor.
2. Calculate the offset to the PRCB address and read it.
3. Get the physical address of the PRCB
4. Calculate the physical address of PRCB + OffsetPrcbContext and read the value
which is a pointer to the CONTEXT.
5. Get the physical address of CONTEXT
6. Read CONTEXT.

Arguments:

Context - Pointer to PDMP_CONTEXT

Return Value:

NT status code.

--*/
{
    PARM_CONTEXT        armContext = NULL;
    UINT32              contextVA = 0;
    UINT32              dataSize = 0;
    UINT32              indexProcessor = 0;
    PKDDEBUGGER_DATA64  kdDebuggerDataBlock;
    UINT32              kiProcessorBlockVA = 0;
    UINT32              offsetPrcbContext = 0;
    UINT32              prcbAddressVA = 0;
    UINT32              processorCount = 0;
    PSECURE_CPU_CONTEXT secureContext = NULL;
    NTSTATUS            status = STATUS_UNSUCCESSFUL;
    UINT32              tempVA = 0;
    IO_STATUS_BLOCK     statusBlock;
    PCPU_STATUS         cpustatus;
    LARGE_INTEGER       armContextPA = Context->ArmContextPA;

    
    //
    // Check prereqs...
    //
    LogLibInfoPrintf(L"Checking prerequisites...\r\n");

    if (Context->KdDebuggerDataBlock == NULL) {
        LogLibInfoPrintf(L"Unable to update APREG. KdDebuggerDataBlock not available.\r\n");
        status = STATUS_UNSUCCESSFUL;
        goto Exit;
    }

    //
    // Allocate the space for context. 
    //
    dataSize = sizeof(ARM_CONTEXT);

    LogLibInfoPrintf(L"Allocating %u bytes for context.\r\n", dataSize);

    armContext = (ARM_CONTEXT*)malloc(dataSize);

    if (armContext == NULL) {
        LogLibInfoPrintf(L"Failed to allocate memory for CONTEXT. Size: %u bytes\r\n", dataSize);
        goto Exit;
    }

    ZeroMemory(armContext, dataSize);
    //
    // Get the addresses of various variables we will need.
    //
    kdDebuggerDataBlock = Context->KdDebuggerDataBlock;

    kiProcessorBlockVA = (UINT32)(kdDebuggerDataBlock->KiProcessorBlock);

    offsetPrcbContext = (UINT32)(kdDebuggerDataBlock->OffsetPrcbContext);

    processorCount = Context->DumpHeader->NumberProcessors;

    LogLibInfoPrintf(L"KiProcessorBlock: 0x%x  OffsetPrcbContext: 0x%x  ProcessorCount: %u\r\n",
        kiProcessorBlockVA,
        offsetPrcbContext,
        processorCount);

    //
    // Update the CONTEXT for each processor.
    //
    for (indexProcessor = 0; indexProcessor < processorCount; indexProcessor++) {
        //
        // Calculate the virtual address of the location containing the PRCB virtual address.
        //
        tempVA = kiProcessorBlockVA + (sizeof(UINT32)* indexProcessor);

        LogLibInfoPrintf(L"[%u] VA of PRCB address: 0x%x\r\n",
            indexProcessor,
            tempVA);

        status = ReadFromDDRSectionByVirtualAddress32(Context,
            tempVA,
            sizeof(prcbAddressVA),
            &prcbAddressVA,
            NULL);
        if (!NT_SUCCESS(status)) {
            LogLibInfoPrintf(L"Failed to read the address for PRCB. VA: 0x%x Status: 0x%x\r\n",
                tempVA,
                status);
            goto Exit;
        }

        if (prcbAddressVA == 0) {
            LogLibErrorPrintf(
                E_FAIL,
                __LINE__,
                WIDEN(__FUNCTION__),
                __WFILE__,
                L"PRCB VA is 0! Abort processing\n");
            status = STATUS_UNSUCCESSFUL;
            goto Exit;
        }
        //
        // Calculate the address of CONTEXT and read it.
        //
        tempVA = prcbAddressVA + offsetPrcbContext;

        LogLibInfoPrintf(L"[%u] VA Context address: 0x%x\r\n",
            indexProcessor,
            tempVA);

        status = ReadFromDDRSectionByVirtualAddress32(Context,
            tempVA,
            sizeof(contextVA),
            &contextVA,
            NULL);
        if (!NT_SUCCESS(status)) {
            LogLibInfoPrintf(L"Failed to read the address for CONTEXT. VA: 0x%x Status: 0x%x\r\n",
                tempVA,
                status);
            goto Exit;
        }

        if (contextVA == 0) {
            LogLibErrorPrintf(
                E_FAIL,
                __LINE__,
                WIDEN(__FUNCTION__),
                __WFILE__,
                L"Context VA is 0! Abort processing\n");
            status = STATUS_UNSUCCESSFUL;
            goto Exit;
        }

        //
        // Read the CONTEXT
        //
        RtlZeroMemory(armContext, dataSize);
        
        status = ReadFromDDRSectionByVirtualAddress32(Context,
            contextVA,
            dataSize,
            armContext,
            &armContextPA);
        
        Context->ArmContextPA = armContextPA;
        
        if (!NT_SUCCESS(status)) {
            LogLibInfoPrintf(L"Failed to read the address for CONTEXT. VA: 0x%x Status: 0x%x\r\n",
                tempVA,
                status);
            goto Exit;
        }

        LogLibInfoPrintf(L"[%u] armContextPA: 0x%x\r\n",
            indexProcessor,
            Context->ArmContextPA.QuadPart);

        cpustatus = (PCPU_STATUS)Add2Ptr(Context->ApReg, sizeof(AP_REG_B_FAMILY_HEADER)+indexProcessor * sizeof(CPU_STATUS));

        if (!(cpustatus->WarmBoot && cpustatus->SGI ||
            cpustatus->DBI_RSVD ||
            (cpustatus->NS || cpustatus->WDT || cpustatus->SGI || cpustatus->WarmBoot) == 0)) {

            if (Context->ApReg->Version == AP_REG_STRUCTURE_V4){
                LogLibInfoPrintf(L"Starting version 4 AP_REG validation...\r\n");

                //
                // For version 4 or higher, APREG is collected only if SC_STATUS_DBI_RSVD or SC_STATUS_CPU_CONTEXT 
                // is set. 
                //
                if (((cpustatus->DBI_RSVD) == 0) &&
                    ((cpustatus->CPU_Context) == 0)) {
                    LogLibInfoPrintf(L"[%u] Skipping AP_REG update. Neither SC_STATUS_DBI_RSVD nor SC_STATUS_CPU_CONTEXT are set.\r\n",
                        indexProcessor);
                    continue;
                }

            }

            //
            // Update CONTEXT
            //
            secureContext = (PSECURE_CPU_CONTEXT)Add2Ptr(Context->ApReg,
                (sizeof(AP_REG_B_FAMILY_HEADER)+
                (Context->ApReg->CPU_Count * sizeof(CPU_STATUS)) +
                (indexProcessor * sizeof(NON_SECURE_CPU_CONTEXT))));

            LogLibInfoPrintf(L"[%u] Updating context. SecureContext: 0x%p\r\n",
                indexProcessor,
                secureContext);

            status = TZBSPContextToNTContext(secureContext, armContext);

            if (!NT_SUCCESS(status)) {
                LogLibInfoPrintf(L"Update CONTEXT with AP Reg Failed\r\n");
                continue;
            }

            LogLibInfoPrintf(L"[%u] ARM_CONTEXT after the update.\r\n", indexProcessor);

        }

        //
        // Write the CONTEXT to dump file.
        //
        status = WriteToDumpByPhysicalAddress(Context,
            Context->ArmContextPA,
            dataSize,
            armContext);

        if (!NT_SUCCESS(status)) {
            LogLibInfoPrintf(L"[%u] Failed to update dump's CONTEXT. Status: 0x%x\r\n", indexProcessor, status);
            goto Exit;
        }

        NtFlushBuffersFile(Context->DedicatedDumpHandle, &statusBlock);

        LogLibInfoPrintf(L"[%u] Written updated CONTEXT to dump.\r\n", indexProcessor);
    } //for indexProcessor

    status = NtFlushBuffersFile(Context->DedicatedDumpHandle, &statusBlock);
    if (NT_SUCCESS(status) && (status != STATUS_PENDING))  {
        status = statusBlock.Status;
    }

Exit:

    if (armContext != NULL) {
        free(armContext);
    }

    if (Context->DedicatedDumpHandle != INVALID_HANDLE_VALUE){
        CloseHandle(Context->DedicatedDumpHandle);
        Context->DedicatedDumpHandle = INVALID_HANDLE_VALUE;
    }

    return status;
}

//































































































































































































































//


NTSTATUS
TZBSPContextToNTContext(
_In_ PSECURE_CPU_CONTEXT tzctx,
_Out_ PARM_CONTEXT ntctx
)
{
    ULONG       mode;
    NTSTATUS    status = STATUS_SUCCESS;

    memset(ntctx, 0, sizeof *ntctx);

    //
    // The context is captured in monitor mode, so look at
    // mon_lr to see the value of pc at the time of the interrupt
    // and mon_spsr to see the value of cpsr
    //

    ntctx->ContextFlags = ARM_CONTEXT_INTEGER | ARM_CONTEXT_CONTROL;
    ntctx->R0 = tzctx->Usr_R0;
    ntctx->R1 = tzctx->Usr_R1;
    ntctx->R2 = tzctx->Usr_R2;
    ntctx->R3 = tzctx->Usr_R3;
    ntctx->R4 = tzctx->Usr_R4;
    ntctx->R5 = tzctx->Usr_R5;
    ntctx->R6 = tzctx->Usr_R6;
    ntctx->R7 = tzctx->Usr_R7;
    ntctx->R8 = tzctx->Usr_R8;
    ntctx->R9 = tzctx->Usr_R9;
    ntctx->R10 = tzctx->Usr_R10;
    ntctx->R11 = tzctx->Usr_R11;
    ntctx->R12 = tzctx->Usr_R12;
    ntctx->Pc = tzctx->Mon_Lr;
    ntctx->Cpsr = tzctx->Mon_Spsr;

    //
    // Read sp and lr from the banked registers of whichever mode we were
    // in (plus r8-r12 if it was fiq mode)
    //
    mode = tzctx->Mon_Spsr & 0x1f;

    switch (mode)
    {
    case usr_mode:
        ntctx->Sp = tzctx->Usr_R13;
        ntctx->Lr = tzctx->Usr_R14;
        break;

    case fiq_mode:
        ntctx->R8 = tzctx->Fiq_R8;
        ntctx->R9 = tzctx->Fiq_R9;
        ntctx->R10 = tzctx->Fiq_R10;
        ntctx->R11 = tzctx->Fiq_R11;
        ntctx->R12 = tzctx->Fiq_R12;
        ntctx->Sp = tzctx->Fiq_R13;
        ntctx->Lr = tzctx->Fiq_R14;
        break;

    case irq_mode:
        ntctx->Sp = tzctx->Irq_R13;
        ntctx->Lr = tzctx->Irq_R14;
        break;

    case svc_mode:
        ntctx->Sp = tzctx->Svc_R13;
        ntctx->Lr = tzctx->Svc_R14;
        break;

    case abt_mode:
        ntctx->Sp = tzctx->Abt_R13;
        ntctx->Lr = tzctx->Abt_R14;
        break;

    case und_mode:
        ntctx->Sp = tzctx->Und_R13;
        ntctx->Lr = tzctx->Und_R14;
        break;

    default:
        LogLibInfoPrintf(L"Unrecognized processor mode in CPU context. Mode: %u\r\n", mode);
        status = STATUS_UNSUCCESSFUL;
        break;
    }

    return status;
}

NTSTATUS
GetAPReg(
_Inout_ PDMP_CONTEXT Context,
_Out_ BOOL* IsApRegValid
)
/*++

Routine Description:

This function finds the physical address of AP_REG header and retrieve it
from disk.

The region is valid if:

1. Magic value match.
2. Version match.
3. CPU Count is non-zero.

Arguments:

Context - PDMP_CONTEXT

IsApRegValid - Informs the caller when a valid AP_REG was found.

Return Value:

NT status code.

--*/
{
    AP_REG_B_FAMILY_HEADER  apRegHeader;
    UINT32                  dataSize;
    NTSTATUS                status = STATUS_UNSUCCESSFUL;
    BOOL                    result = FALSE;
    LARGE_INTEGER           APRegAddress;

    if (0 == Context->APRegAddress.QuadPart) {
        LogLibInfoPrintf(L"AP_REG address is invalid!\n");
        goto Exit;
    }
    LogLibInfoPrintf(L"AP_REG address is 0x%x\r\n", Context->APRegAddress.QuadPart);

    APRegAddress.QuadPart = Context->APRegAddress.QuadPart;
    result = ReadDisk(Context->hDisk,
        &apRegHeader,
        sizeof(AP_REG_B_FAMILY_HEADER),
        &APRegAddress);

    if (result == FALSE) {
        LogLibInfoPrintf(L"Read AP_REG failed. Status:0x%x\r\n", status);
        goto Exit;
    }

    //
    // Check signature.
    //
    if (apRegHeader.Magic != AP_REG_STRUCTURE_MAGIC_VALUE) {
        LogLibInfoPrintf(L"Unexpected AP_REG magic value. Expected: 0x%x Actual: 0x%x\r\n",
            AP_REG_STRUCTURE_MAGIC_VALUE,
            apRegHeader.Magic);
        status = STATUS_BAD_DATA;
        goto Exit;
    }

    //
    // Check version.
    //
    if (apRegHeader.Version < AP_REG_STRUCTURE_V2 || apRegHeader.Version > AP_REG_STRUCTURE_V4) {
        LogLibInfoPrintf(L"Unexpected AP_REG version. Expected: 0x%x or 0x%x Actual: 0x%x\r\n",
            AP_REG_STRUCTURE_V2,
            AP_REG_STRUCTURE_V4,
            apRegHeader.Version);
        status = STATUS_BAD_DATA;
        goto Exit;
    }

    //
    // Check CPU count.
    //
    if ((apRegHeader.CPU_Count != 0) &&
        (apRegHeader.CPU_Count > AP_REG_MAX_CPUS)) {
        LogLibInfoPrintf(L"Unexpected AP_REG CPU count. Actual: 0x%x\r\n",
            apRegHeader.CPU_Count);
        status = STATUS_BAD_DATA;
        goto Exit;
    }

    //
    // Calculate and retrieve the rest of the content. 
    //

    dataSize = sizeof(AP_REG_B_FAMILY_HEADER)+(apRegHeader.CPU_Count *
        (sizeof(CPU_STATUS)+sizeof(NON_SECURE_CPU_CONTEXT)+sizeof(WDOG_STATUS))) +
        sizeof(SECURE_CPU_CONTEXT);

    //
    // Allocate the buffer...
    //
    Context->ApReg = (PAP_REG_B_FAMILY_HEADER)malloc(dataSize);

    if (Context->ApReg == NULL) {
        LogLibInfoPrintf(L"Failed to allocate memory for AP_REG. Size: 0x%x\r\n",
            dataSize);
        status = STATUS_NO_MEMORY;
        goto Exit;
    }

    ZeroMemory(Context->ApReg, dataSize);
    result = ReadDisk(Context->hDisk,
        Context->ApReg,
        dataSize,
        &APRegAddress);

    if (result == FALSE) {
        LogLibInfoPrintf(L"Read AP_REG failed. Status:0x%x\r\n", status);
        goto Exit;
    }

    Context->TotalCpuContextSizeInBytes = dataSize;

    *IsApRegValid = TRUE;
    status = STATUS_SUCCESS;

Exit:

    return status;
}

NTSTATUS
WriteAPREG(
_Inout_ PDMP_CONTEXT Context)
{
    LPWSTR              Filename = L"APREG.bin";
    DWORD               flag= 0;
    LARGE_INTEGER       fileoffset;
    HANDLE              hFile = INVALID_HANDLE_VALUE;
    IO_STATUS_BLOCK     statusBlock;
    NTSTATUS            status = STATUS_SUCCESS;


    fileoffset.QuadPart = 0;
    flag = FILE_ATTRIBUTE_NORMAL;
    flag |= FILE_FLAG_OVERLAPPED;

    hFile = CreateFileW(Filename,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_ALWAYS,
        flag,
        NULL);
    if (hFile == INVALID_HANDLE_VALUE){
        LogLibErrorPrintf(
            E_FAIL,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__,
            L" Create File Failed. INVALID_HANDLE_VALUE. %s\n", Filename);
        goto Exit;
    }

    status = NtWriteFile(hFile,
        NULL,
        NULL,
        NULL,
        &statusBlock,
        Context->ApReg,
        (ULONG)Context->TotalCpuContextSizeInBytes,
        &fileoffset,
        NULL);
    if (!NT_SUCCESS(status)) {
        LogLibInfoPrintf(L"Failed to write SV section to dump file. Status: 0x%x\r\n",
            status);
        goto Exit;
    }

Exit:
    if (hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(hFile);
    }

    return status;
}

NTSTATUS
GetX86CPUContext(
_Inout_ PDMP_CONTEXT Context
)
/*++

Routine Description:

This function writes CPU Context Specific sections based on the DUMP_HEADER's physical memory descriptor
to dump file.

Arguments:

Context - Pointer to the global context structure.

Return Value:

NT status code.

--*/
{

    UINT32                   sectionsCount = 0;
    UINT32                   sectionIndex = 0;
    PRAW_DUMP_SECTION_HEADER section = NULL;
    NTSTATUS                status = STATUS_UNSUCCESSFUL;

    sectionsCount = Context->RawDumpHeader->SectionsCount;

    for (sectionIndex = 0; sectionIndex < sectionsCount; sectionIndex++) {
        section = &(Context->RawDumpHeader->SectionTable[sectionIndex]);
        if (section->Type == RAW_DUMP_SECTION_TYPE_CPU_CONTEXT)  {
            Context->CPUContextAddress.QuadPart = section->Offset;
            Context->TotalCpuContextSizeInBytes = section->Size;
            status = STATUS_SUCCESS;
        } // RAW_DUMP_SECTION_TYPE_CPU_CONTEXT
    }//for sectionindex

    return status;
}


BOOL
ISContainAPREG(
BYTE* buffer,
LONG buffersize,
LONG* offset)
{
    BOOL    foundAPRG = FALSE;
    LONG    j = 0;
    UCHAR AP_REG[] = { // String --> "QACD"
        0x51, 0x41, 0x43, 0x44
    };

    for (j = 0; j <(LONG)(buffersize - sizeof(AP_REG)); j++){
        if (memcmp((buffer + j), AP_REG, sizeof(AP_REG)) == 0) {
            *offset = j;
            foundAPRG = TRUE;
            goto EXIT;
        }
    }

EXIT:
    return foundAPRG;

}

NTSTATUS
VirtualToPhysical32(
_In_ PDMP_CONTEXT Context,
_In_ UINT32 VirtualAddress,
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
    NTSTATUS            status = STATUS_UNSUCCESSFUL;
    UINT32              directoryTableBase = 0;
    UINT32              pdeAddress;
    LARGE_INTEGER       temp;
    UINT32              pdi;
    UINT32              PPE;
    UINT32              pde;
    UINT32              pdeoffset = 0;
    UINT32              pageTableAddress;
    UINT32              pteAddress;
    UINT32              pte;
    UINT32              pteoffset = 0;
    UCHAR               PaeEnabled;
    PAE_ADDRESS         virtualaddinPAE;
    PHARDWARE_PTE       PointerPte;

    PhysicalAddress->QuadPart = (ULONGLONG)-1;

    //
    // ARM_VALID_PFN_MASK           0xFFFFF000
    // X86_VALID_PFN_MASK           0xFFFFF000
    directoryTableBase = (UINT32)(Context->DumpHeader->DirectoryTableBase & ARM_VALID_PFN_MASK);
    if (directoryTableBase == 0) {
        LogLibInfoPrintf(L"Directory table base is NULL.\r\n");
        status = STATUS_BAD_DATA;
        goto Exit;
    }

    PaeEnabled = Context->DumpHeader->PaeEnabled;      
    if (PaeEnabled != TRUE) {    
        LogLibInfoPrintf(L"PAE Disabled: VirtualAddress : 0x%x  directory table base is: 0x%x \r\n", VirtualAddress, directoryTableBase);
        pdeoffset = VirtualAddress;
        pdeoffset = (pdeoffset >> 22)*sizeof (UINT32);
        pdeAddress = directoryTableBase + pdeoffset;
        temp.QuadPart = pdeAddress;
        status = ReadFromDDRSectionByPhysicalAddress(Context, temp, sizeof(UINT32), &pde);

        if (!NT_SUCCESS(status)) {
            LogLibInfoPrintf(L"Failed to read PDE. Status: 0x%x\r\n", status);
            goto Exit;
        }

        LogLibInfoPrintf(L"Physical PDE Address:0x%x     offset:0x%x     PDE Content: 0x%x  VirtualAddress:0x%x \r\n", pdeAddress, pdeoffset, pde, VirtualAddress);
        
        // 
        // Checking PDE attributes.
        //
        PointerPte = (PHARDWARE_PTE) &pde;
        if(PointerPte->LargePage) {
            // 
            // we have a 4 MB page. Dont go to any more levels from here. Least 22 bits will be the offset.
            //
            PhysicalAddress->QuadPart = (UINT32)(pde & ARM_PDE_MASK) + (UINT32)(VirtualAddress & ARM_LARGE_PAGE_ADDR_OFFSET_MASK);
            LogLibInfoPrintf(L"LAGE PAGE: Decoded Physical Address: 0x%x    PDE: 0x%x   Virtual address: 0x%x\r\n", PhysicalAddress->QuadPart, pde, VirtualAddress);            
        }

        else {
        pageTableAddress = pde & ARM_VALID_PFN_MASK;
        pteoffset = VirtualAddress;
        pteoffset = ((pteoffset & 0x003FF000) >> 12) * sizeof(UINT32);
        pteAddress = pageTableAddress + pteoffset;
        temp.QuadPart = pteAddress;
        status = ReadFromDDRSectionByPhysicalAddress(Context, temp, sizeof(UINT32), &pte);

        LogLibInfoPrintf(L"Physical PTE Address: 0x%x    offset = 0x%x  PTE Content= 0x%x   PageTableAddress = 0x%x\r\n", pteAddress, pteoffset, pte, pageTableAddress);

        if (!NT_SUCCESS(status)) {
            LogLibInfoPrintf(L"Failed to read PTE. Status: 0x%x\r\n", status);
            goto Exit;
        }

        if (pte == 0) {
            LogLibErrorPrintf(
                E_FAIL,
                __LINE__,
                WIDEN(__FUNCTION__),
                __WFILE__,
                L"Error: PTE is zero\n");
            status = STATUS_UNSUCCESSFUL;
            goto Exit;
        }

        PhysicalAddress->QuadPart = (UINT32)(pte & 0xFFFFF000) + (UINT32)(VirtualAddress & 0x00000FFF);
        LogLibInfoPrintf(L"Decoded Physical Address: 0x%x    PTE: 0x%x   Virtual address: 0x%x\r\n", PhysicalAddress->QuadPart, pte, VirtualAddress);
        }
        status = STATUS_SUCCESS;
    }
    else {
        LogLibInfoPrintf(L"PAE enabled system, we will use PDI->PDE->PTE->physic address decoding");

        //
        // Page Directory Index (PDI)->Page Directory Entries (PDE) ->PTE->Physic address
        //
        //  highest 2bit are PDI, 00,01,10,11 are valid
        //
        //
        virtualaddinPAE.DwordPart = VirtualAddress;
        pdi = virtualaddinPAE.DirectoryPointer;

        pageTableAddress = (directoryTableBase & 0xffffffe0) + pdi*sizeof(ULONGLONG);
        temp.QuadPart = pageTableAddress;
        status = ReadFromDDRSectionByPhysicalAddress(Context, temp, sizeof(UINT32), &PPE);
        if (!NT_SUCCESS(status)) {
            LogLibInfoPrintf(L"Failed to read PPE. Status: 0x%x\r\n", status);
            goto Exit;
        }

        LogLibInfoPrintf(L"VirtualAddress: 0x%x PPE:0x%x\r\n", VirtualAddress, PPE);

        pdeoffset = virtualaddinPAE.Directory;
        pdeAddress = (PPE & 0xFFFFF000) + pdeoffset*sizeof(ULONGLONG);
        temp.QuadPart = pdeAddress;
        status = ReadFromDDRSectionByPhysicalAddress(Context, temp, sizeof(UINT32), &pde);
        if (!NT_SUCCESS(status)) {
            LogLibInfoPrintf(L"Failed to read PDE. Status: 0x%x\r\n", status);
            goto Exit;
        }

        LogLibInfoPrintf(L"Physical PDE Address:0x%x     offset:0x%x     PDE Content: 0x%x  VirtualAddress:0x%x \r\n", pdeAddress, pdeoffset, pde, VirtualAddress);

        pteoffset = virtualaddinPAE.Table;
        pteAddress = (pde & 0xFFFFF000) + pteoffset*sizeof(ULONGLONG);
        temp.QuadPart = pteAddress;
        status = ReadFromDDRSectionByPhysicalAddress(Context, temp, sizeof(UINT32), &pte);

        LogLibInfoPrintf(L"Physical PTE Address: 0x%x    offset = 0x%x  PTE Content= 0x%x   \r\n", pteAddress, pteoffset, pte);
        if (!NT_SUCCESS(status)) {
            LogLibInfoPrintf(L"Failed to read PTE. Status: 0x%x\r\n", status);
            goto Exit;
        }

        PhysicalAddress->QuadPart = (UINT32)(pte & 0xFFFFF000) + (UINT32)(VirtualAddress & 0x00000FFF);
        LogLibInfoPrintf(L"Decoded Physical Address: 0x%x    PTE: 0x%x   Virtual address= 0x%x\r\n", PhysicalAddress->QuadPart, pte, VirtualAddress);
        status = STATUS_SUCCESS;

    }

Exit:

    return status;
}


NTSTATUS
ReadFromDDRSectionByVirtualAddress32(
_In_ PDMP_CONTEXT Context,
_In_ UINT32 VirtualAddress,
_In_ UINT32 Length,
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
    UINT32              endVA;
    UINT32              nextPageVA;
    LARGE_INTEGER       physicalAddress;
    NTSTATUS            status = STATUS_UNSUCCESSFUL;

    //
    // Validate if we are crossing page boundary. If we are
    // return STATUS_INVALID_PARAMETER. 
    //
    nextPageVA = (VirtualAddress & 0xFFFFF000) + 0x1000;
    endVA = VirtualAddress + Length - 1;

    if (endVA >= nextPageVA) {
        //
        // This address could be in a Large Page (4MB)
        //
        nextPageVA = (VirtualAddress & 0xFF000000) + 0x1000000;
        endVA = VirtualAddress + Length - 1;
        
        if (endVA >= nextPageVA) {
            status = STATUS_INVALID_PARAMETER;
            LogLibInfoPrintf(L"The read crosses page boundary %d", status);
            goto Exit;
        }
    }
    

    //
    // Get the physical address.
    //
    status = VirtualToPhysical32(Context,
        VirtualAddress,
        &physicalAddress);

    if (!NT_SUCCESS(status)) {
        LogLibInfoPrintf(L"Failed to convert virtual address 0x%x to physical address. Status: 0x%x\r\n",
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
        LogLibInfoPrintf(L"Failed to read %u bytes from physical address 0x%I64x. Status: 0x%x\r\n",
            Length,
            physicalAddress.QuadPart,
            status);

        goto Exit;
    }

    LogLibInfoPrintf(L"Read %u bytes from virtual address 0x%x (PA: 0x%I64x).\r\n",
        Length,
        VirtualAddress,
        physicalAddress.QuadPart);
    status = STATUS_SUCCESS;

Exit:

    return status;
}

NTSTATUS
ValidateDDRAgaisntPhysicalMemoryBlock32(
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
    PDDR_MEMORY_MAP               ddrMemoryMap = NULL;
    UINT64                        endPD;
    UINT32                        indexDDR;
    UINT32                        indexPD;
    UINT64                        startPD;
    UINT32                        spanCount;
    UINT32                        memorySizeFromPD = 0;
    PPHYSICAL_MEMORY_DESCRIPTOR32 physDesc = NULL;
    NTSTATUS                      status = STATUS_UNSUCCESSFUL;
    BOOLEAN                       startInSection;
    BOOLEAN                       endInSection;

    ddrMemoryMap = Context->DDRMemoryMap;
    physDesc = &(Context->DumpHeader->PhysicalMemoryBlock);

    memorySizeFromPD = physDesc->NumberOfPages * PAGE_SIZE;

    Context->SizeAccordingToMemoryDescriptors = memorySizeFromPD;

    if (memorySizeFromPD > Context->TotalDDRSizeInBytes) {
        LogLibInfoPrintf(L"Memory size from memory block (0x%p) is larger than DDR size (0x%p)\r\n",
            memorySizeFromPD,
            Context->TotalDDRSizeInBytes);
        status = STATUS_BAD_DATA;
        goto Exit;
    }
	
	for (UINT32 i = 0; i <  Context->DDRSectionCount; i ++) {
		LogLibInfoPrintf(L" Context->DDRMemoryMap[%d].Size        0x%I64x", i, Context->DDRMemoryMap[i].Size);
		LogLibInfoPrintf(L" Context->DDRMemoryMap[%d].Offset      0x%I64x", i, Context->DDRMemoryMap[i].Offset);
		LogLibInfoPrintf(L" Context->DDRMemoryMap[%d].End         0x%I64x", i, Context->DDRMemoryMap[i].End);
		LogLibInfoPrintf(L" Context->DDRMemoryMap[%d].Contiguous  0x%I64x", i, Context->DDRMemoryMap[i].Contiguous);
		LogLibInfoPrintf(L" Context->DDRMemoryMap[%d].Base        0x%I64x\n", i, Context->DDRMemoryMap[i].Base);
	}
		
    for (indexPD = 0; indexPD < physDesc->NumberOfRuns; indexPD++) {
        startPD = (UINT64)(physDesc->Run[indexPD].BasePage * PAGE_SIZE);
        endPD = startPD + (UINT64)(physDesc->Run[indexPD].PageCount * PAGE_SIZE) - 1;
        spanCount = 0;
        startInSection = FALSE;
        endInSection = FALSE;

        LogLibInfoPrintf(L"Looking at descriptor 0x%x\r\n", indexPD);

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
                LogLibInfoPrintf(L"Run Index: 0x%x\r\n");
                LogLibInfoPrintf(L"DDR Index: 0x%x\r\n", indexDDR);
                LogLibInfoPrintf(L"Previous DDR Index: 0x%x\r\n", (indexDDR - 1));

                status = STATUS_BAD_DATA;
                goto Exit;
            }

            //
            // Is startPD in this DDR section?
            //
            if ((startPD >= ddrMemoryMap[indexDDR].Base) &&
                (startPD <= (ddrMemoryMap[indexDDR].Base + ddrMemoryMap[indexDDR].Size)))
            {
                startInSection = TRUE;
                //
                // Does endPD also fall into this section?
                //
                if ((endPD >= ddrMemoryMap[indexDDR].Base) &&
                    (endPD <= (ddrMemoryMap[indexDDR].Base + ddrMemoryMap[indexDDR].Size)))
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
            LogLibInfoPrintf(L"The start (0x%I64x) of run 0x%x does not fall into any DDR section.\r\n",
                (UINT64)(physDesc->Run[indexPD].BasePage * PAGE_SIZE),
                indexPD);
            status = STATUS_BAD_DATA;
            goto Exit;
        }

        if (!endInSection)
        {
            LogLibInfoPrintf(L"The end (0x%I64x) of run 0x%x does not fall into any DDR section.\r\n",
                endPD,
                indexPD);
            status = STATUS_BAD_DATA;
            goto Exit;
        }

        LogLibInfoPrintf(L"Descriptor 0x%x checked out.\r\n", indexPD);
    }//indexPD

    status = STATUS_SUCCESS;
Exit:
    return status;
}


NTSTATUS
WriteDumpHeader32(
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
    IO_STATUS_BLOCK         statusBlock;
    LARGE_INTEGER           dedicatedDumpFileOffset;

    Context->DedicatedDumpFileOffset.QuadPart = 0;

    //
    // Update the DUMP_HEADER32
    //
    LogLibInfoPrintf(L"Changing value of \"Context->DumpHeader->RequiredDumpSpace\" from 0x%llx to 0x%llx\n", Context->DumpHeader->RequiredDumpSpace, Context->ActualDumpFileUsedInBytes);
    Context->DumpHeader->RequiredDumpSpace = Context->ActualDumpFileUsedInBytes;

    if ((Context->SVSectionCount > 0) || (Context->CPUContextSectionCount > 0)) {
        Context->DumpHeader->SecondaryDataState = STATUS_SUCCESS;
    }

    //
    // Write the header to dump.
    //
    dataSize = sizeof(DUMP_HEADER32);
    dedicatedDumpFileOffset.QuadPart = Context->DedicatedDumpFileOffset.QuadPart;
    status = NtWriteFile(Context->DedicatedDumpHandle,
        NULL,
        NULL,
        NULL,
        &statusBlock,
        Context->DumpHeader,
        dataSize,
        &dedicatedDumpFileOffset,
        NULL);

    if (!NT_SUCCESS(status)) {
        LogLibErrorPrintf(
            E_FAIL,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__,
            L"Error:  Failed to write DumpHeader%d \n",
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
WriteDDR32(
_Inout_ PDMP_CONTEXT Context
)
/*++

Routine Description:

This function writes DDR sections based on the DUMP_HEADER's physical memory descriptor
to dump file.

Arguments:

Context - Pointer to the global context structure.

Return Value:

NT status code.

--*/
{
    LARGE_INTEGER   basePA;
    ULONG           PageRemain = 0;
    LARGE_INTEGER   bytesWritten = { 0 };
    UINT32          io = 0;
    UINT32          index = 0;
    UINT32          ioCountPerRun = 0;
    LARGE_INTEGER   runSize;
    LARGE_INTEGER   startPA;
    ULONG           ioSize = 0;
    ULONG           buffersize = DEFAULT_DMP_BUF_SZ;
    NTSTATUS        status = STATUS_UNSUCCESSFUL;
    PVOID           tempBuffer = NULL;
    IO_STATUS_BLOCK statusBlock;
    LARGE_INTEGER   dedicatedDumpFileOffset;


    //
    // Allocate the intermediate buffer to read memory from DDR section
    // to the dump file.
    //
    tempBuffer = malloc(buffersize);
    if (tempBuffer == NULL) {
        LogLibInfoPrintf(L"Unable to allocate 0x%x bytes buffer for writing DDR memory to dump.\r\n",
            PAGE_SIZE);
        status = STATUS_NO_MEMORY;
        goto Exit;
    }

    ZeroMemory(tempBuffer, buffersize);

    for (index = 0; index < Context->DumpHeader->PhysicalMemoryBlock.NumberOfRuns; index++)  {

        basePA.QuadPart = (Context->DumpHeader->PhysicalMemoryBlock.Run[index].BasePage * PAGE_SIZE);
        PageRemain = Context->DumpHeader->PhysicalMemoryBlock.Run[index].PageCount;
        runSize.QuadPart = (Context->DumpHeader->PhysicalMemoryBlock.Run[index].PageCount *PAGE_SIZE);
        ioCountPerRun = (UINT32)(runSize.QuadPart / buffersize);

        if ((runSize.QuadPart % buffersize) > 0) {
            ioCountPerRun++;
        }

        LogLibInfoPrintf(L"0x%x IOs will be required for this run.\r\n", ioCountPerRun);

        for (io = 0; io < ioCountPerRun; io++) {
            startPA.QuadPart = basePA.QuadPart + io * ioSize;

            ioSize = (PageRemain * PAGE_SIZE)< buffersize ?
                PageRemain * PAGE_SIZE :
                buffersize;

            status = ReadFromDDRSectionByPhysicalAddress(Context,
                startPA,
                ioSize,
                tempBuffer);
            if (!NT_SUCCESS(status)) {
                LogLibInfoPrintf(L"[0x%x] Failed to read from DDR sections. Status: 0x%x\r\n",
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
            LogLibInfoPrintf(L"[%d]Run 0x%x Pages remain.\r\n", index, PageRemain);
        }// for io

        LogLibInfoPrintf(L"**** Finished writing run 0x%x. ****\r\n", index);
    }//for index

    if ((bytesWritten.QuadPart / PAGE_SIZE) != Context->DumpHeader->PhysicalMemoryBlock.NumberOfPages) {
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

    if (tempBuffer != NULL) {
        free(tempBuffer);
        tempBuffer = NULL;
    }

    return status;
}

NTSTATUS
GetKdDebuggerDataBlock32(
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
    PKDDEBUGGER_DATA64          dbgDataBlock = nullptr;
    LARGE_INTEGER               dbgDataHeaderPA;
    LARGE_INTEGER               decodedBlockPA;
    UINT32                      dbgDataHeaderVA;
    NTSTATUS                    status = STATUS_UNSUCCESSFUL;
    BOOL                        result = FALSE;
    
    LogLibInfoPrintf(L"Checking DBGKD_DEBUG_DATA_HEADER64.Size\r\n");
    Context->KdDebuggerDataBlockPA.QuadPart = ADDRESS_NOT_PRESENT;

    //
    // Read DBGKD_DEBUG_DATA_HEADER64
    // 
    dbgDataHeaderVA = Context->DumpHeader->KdDebuggerDataBlock;
    dataSize = sizeof(KDDEBUGGER_DATA64);
    dbgDataBlock = (PKDDEBUGGER_DATA64)malloc(dataSize);
    if (dbgDataBlock == nullptr) {
        goto Exit;
    }
    ZeroMemory(dbgDataBlock, dataSize);
        
  
    status = ReadFromDDRSectionByVirtualAddress32(Context,
                dbgDataHeaderVA,
                dataSize,
                dbgDataBlock,
                &dbgDataHeaderPA);

    if (!NT_SUCCESS(status)) {
        LogLibInfoPrintf(L"Failed to read the address for CONTEXT. VA: 0x%x Status: 0x%x\r\n",
        dbgDataHeaderVA,
        status);
        goto Exit;
    }   
       
    if(dbgDataBlock->Header.Size > 0x10000  || !ValidateKdDebuggerDataBlock(&(dbgDataBlock->Header))) {    
    
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
    else {
           LogLibInfoPrintf(L"DBGKD_DEBUG_DATA_HEADER64.Size is: 0x%x\r\n", dbgDataBlock->Header.Size);
           LogLibInfoPrintf(L"KdDebuggerDataBlock appears to be valid and decoded. Read it in to memory.\r\n");
    }
    
    Context->KdDebuggerDataBlock = dbgDataBlock;   
    Context->KdDebuggerDataBlockPA.QuadPart = dbgDataHeaderPA.QuadPart;
    if (!ValidateKdDebuggerDataBlock(&(Context->KdDebuggerDataBlock->Header))) {
        status = STATUS_UNSUCCESSFUL;
        LogLibInfoPrintf(L"The KdDebuggerDataBlock is not valid.\r\n");
        goto Exit;
    }

    LogLibInfoPrintf(L"KdDebuggerDataBlock has been read into memory.\r\n");
    status = WriteToDumpByPhysicalAddress(Context,
        Context->KdDebuggerDataBlockPA,
        dataSize,
        Context->KdDebuggerDataBlock);
   
    if (!NT_SUCCESS(status)) {
        LogLibInfoPrintf(L"Failed write the decoded copy of KdDebuggerData to dump. Status: 0x%x\r\n", status);
        goto Exit;
    }
     status = STATUS_SUCCESS;
    LogLibInfoPrintf(L"KdDebuggerDataBlock Writen to 0x%I64x Size:%d been read into memory.\r\n", Context->KdDebuggerDataBlockPA, dataSize);
Exit:
    return status;
}

NTSTATUS
ExtractWindowsDumpFile32(PDMP_CONTEXT Context)
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
    BOOL        ValidateResult = FALSE;
    HRESULT     result = ERROR_SUCCESS; 
    
     LogLibInfoPrintf(L"=========== Getting the pre-built DUMP_HEADER. ===========\r\n");
    status = GetDumpHeader32(Context);
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
        Context->DumpHeader->BugCheckParameter2 = INVALID_AP_REG;
    }

    LogLibInfoPrintf(L"=========== Validating DUMP_HEADER's memory descriptors against DDR sections. ===========\r\n");
    status = ValidateDDRAgaisntPhysicalMemoryBlock32(Context);

    if (FAILED(status)) {
        LogLibErrorPrintf(
            E_FAIL,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__,
            L"Error: Failed ValidateDDRAgaisntPhysicalMemoryBlock32 \n");
        goto Exit;
    }

    LogLibInfoPrintf(L"=========== Init the Windows 32 dump file ===========\r\n");
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

    LogLibInfoPrintf(L"=========== Writing DUMP_HEADER 32 to the dump file. ===========\r\n");
    status = WriteDumpHeader32(Context);
    if (FAILED(status)) {
        LogLibErrorPrintf(
            E_FAIL,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__,
            L"Error: Failed WriteDumpHeader32 \n");
        goto Exit;
    }

    LogLibInfoPrintf(L"=========== Writing DDR section to the dump file. ===========\r\n");
    status = WriteDDR32(Context);
    if (FAILED(status)) {
        LogLibErrorPrintf(
            E_FAIL,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__,
            L"Error: Failed WriteDDR32 \n");
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
    status = GetKdDebuggerDataBlock32(Context);
    if (!NT_SUCCESS(status)) {
        LogLibErrorPrintf(
            E_FAIL,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__,
            L"Error:GetKdDebuggerDataBlock32 \n");
        goto Exit;
    }

    if(Context->IsAPREGRequested != FALSE) {
        LogLibInfoPrintf(L"=========== Update CPU context ==============================================\r\n");
        switch (Context->DumpHeader->MachineImageType){

        case IMAGE_FILE_MACHINE_I386:





            break;
        case IMAGE_FILE_MACHINE_ARM:
        case IMAGE_FILE_MACHINE_ARMNT:
            if (Context->HasValidAP_REG == TRUE) {
                
                if(Context->isAPREG64 == FALSE && Context->IsAPREGRequested == TRUE) {
                    //
                    // Get the AP_REG data from memory and do some validation.
                    //
                    LogLibInfoPrintf(L" Try getting AP_REG. ===========\r\n");

                    status = GetAPReg(Context, &ValidateResult);
                    if (!NT_SUCCESS(status)) {
                        LogLibErrorPrintf(
                            E_FAIL,
                            __LINE__,
                            WIDEN(__FUNCTION__),
                            __WFILE__,
                            L"Error:Failed to read AP_REG \n");
                        goto Exit;
                    }
                    status = WriteAPREG(Context);
                    if (!NT_SUCCESS(status)) {
                        LogLibErrorPrintf(
                            E_FAIL,
                            __LINE__,
                            WIDEN(__FUNCTION__),
                            __WFILE__,
                            L"Error:WriteAPREG \n");
                        goto Exit;
                    }
                 
                    status = UpdateContextWithAPReg(Context);
                    if (FAILED(status)) {
                        LogLibErrorPrintf(
                            E_FAIL,
                            __LINE__,
                            WIDEN(__FUNCTION__),
                            __WFILE__,
                            L"Error: Failed UpdateContextWithAPReg \n");
                        goto Exit;
                    }
                }
                else {
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




BOOL 
OpenDumpFiles( 
    _In_ PCWSTR pszDmpOut, 
    _Out_ HANDLE *phDmpOut
 )
{
    HANDLE hDmpOut =
        CreateFileW(
            pszDmpOut,
            FILE_GENERIC_READ,
            FILE_SHARE_READ | FILE_SHARE_DELETE,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

    if (hDmpOut == INVALID_HANDLE_VALUE) {
        LogLibInfoPrintf(L"Unable to open output file, Error 0x%x\n\r", GetLastError());        
        return FALSE;
    }

    *phDmpOut = hDmpOut;
    return TRUE;
}

VOID 
TestDebugEngine(PDMP_CONTEXT Context)
{
   // Test Code.
    HRESULT hr = E_FAIL;
    KDDEBUGGER_DATA64 KdDebuggerDataBlock;
    UINT32 dbgDataHeaderVA;
      
    if (!DbgClient::Initialize(Context->DedicatedDumpFilePath, L"cache*c:\\symbols;srv*http://symweb;")) {        
        // Press on without dbgeng
        wprintf(L"Cant Initialize Debug Engine \n\r");
    }
    wprintf(L"Initialized ... opening the file handle now\n\r");
    
     
    IDebugSymbols3 *DebugSymbols = DbgClient::GetSymbols();
    IDebugDataSpaces2 *DebugDataSpaces = DbgClient::GetDataSpaces();
    
    
    wprintf(L"Opened the file handle, trying to decode nt!KdDebuggerDataBlock\n\r");
    
    if ((DebugSymbols == NULL) || (DebugDataSpaces == NULL)) {
        wprintf(L"Failure in dbgeng, KdDebuggerDataBlock may be unreadable\n\r");
        return ;
    }

    if (FAILED(hr = DebugSymbols->Reload("nt"))) {
        wprintf(L"Failed (0x%x) to load kernel symbols, KdDebuggerDataBlock may be unreadable", hr);
        return ;
    }
    
    dbgDataHeaderVA = Context->DumpHeader->KdDebuggerDataBlock;
    
    hr = DebugDataSpaces->ReadVirtual(dbgDataHeaderVA, &KdDebuggerDataBlock, sizeof KdDebuggerDataBlock, NULL);
    if (FAILED(hr)) {
        wprintf(L"Failed to read nt!KdDebuggerDataBlock Error 0x%x\n\r", hr);
        return;
    }
}



