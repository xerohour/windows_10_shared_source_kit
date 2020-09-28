/*++

    Copyright (C) Microsoft. All rights reserved.

Module Name:
   util.h

Environment:
   User Mode

Author:
   jinsh 10/3/2010
--*/

#pragma once 

#include "common.h"
//#include <nt.h>

#include <wdbgexts.h>
#include <ntiodump.h>


#define LARGE_INT_TO_PVOID(var)           ((UINTN*)(UINTN)(var).QuadPart)
#define GET_FLAG(flag)                    *LARGE_INT_TO_PVOID(flag)
#define SET_FLAG(flag, value)             *LARGE_INT_TO_PVOID(flag) = value

#define Add2Ptr(_Ptr, _Length) ((PVOID)((PUCHAR)(_Ptr) + (_Length)))

#define PAGES_TO_BYTES(Pages)             ((((UINT64)(Pages)) << (PAGE_SHIFT)))
#define PFN_TO_PHYSADDR(Pages)            PAGES_TO_BYTES(Pages)

#pragma pack(1)
//
// DDR Section Memory Map
// Based on DDR Seciton plus some 
// additional info to facilitate reading
// DDR sections
//
typedef struct _DDR_MEMORY_MAP
{
    UINT64      Base;

    //
    // End is Base + Size - 1
    //
    UINT64      End;
    UINT64      Size;

    UINT64      Offset;

    //
    // Contiguous tells us whether the current section is 
    // contiguous from previous section in terms of physical
    // addresses. What that means is that previous section's
    // End is just one byte less than this section's Base. 
    //
    BOOLEAN     Contiguous;
}DDR_MEMORY_MAP, *PDDR_MEMORY_MAP;

#pragma warning( push )
#pragma warning(disable : 4201) // nonstandard extension used : nameless struct/union
typedef struct _PAE_ADDRESS {
    union {
        struct {
            ULONG Offset : 12;                  // 0  .. 11
            ULONG Table : 9;                    // 12 .. 20
            ULONG Directory : 9;                // 21 .. 29
            ULONG DirectoryPointer : 2;         // 30 .. 31
        };
        struct {
            ULONG Offset : 21 ;
            ULONG Directory : 9 ;
            ULONG DirectoryPointer : 2;
        } LargeAddress;

        ULONG DwordPart;
    };
} PAE_ADDRESS, * PPAE_ADDRESS;
#pragma warning( pop )
//
// Global context struct. 
//
typedef struct _DMP_CONTEXT
{
    LARGE_INTEGER                                       ConfigTableAddress;
    HANDLE                                              hDisk;
    LARGE_INTEGER                                       diskoffset;
    LARGE_INTEGER                                       RawDumpPartitionLength;
    PRAW_DUMP_HEADER                                    RawDumpHeader;
    UINT32                                              RawDumpTableSize;
    UINT32                                              OfflineMemoryDumpUseCapability;
    PRAW_DUMP_SECTION_HEADER                            DDRSections;
    UINT32                                              DDRMemoryMapCount;
    PDDR_MEMORY_MAP                                     DDRMemoryMap;
    UINT64                                              TotalDDRSizeInBytes;
    UINT64                                              TotalSVSpecificSizeInBytes;
    UINT64                                              TotalCpuContextSizeInBytes;
    LARGE_INTEGER                                       CPUContextAddress;
    UINT32                                              DDRSectionCount;
    UINT32                                              SVSectionCount;
    UINT32                                              CPUContextSectionCount;
    UINT32                                              MissingFlagsCount;
    UINT32                                              InvalidSectionCount;
    UINT32                                              InsufficientStorageSectionsCount;
    UINT32                                              InvalidVersionCount;
    UINT32                                              DDRSectionsOverlapCount;
    UINT32                                              DDRSectionFragmentationCount;
    UINT64                                              LargestSVSpecificSectionSize;
    BOOL                                                Is64Bit;

    //
    // Dump file info
    //
    LPWSTR                                              DedicatedDumpFilePath;
    HANDLE                                              DedicatedDumpHandle;
    LARGE_INTEGER                                       DedicatedDumpFileOffset;

    LARGE_INTEGER                                       DDRFileOffset;
    LARGE_INTEGER                                       SecondaryDataOffset;
    LARGE_INTEGER                                       ActualDumpFileUsedInBytes;
    BOOL                                                DumpDDR;

    //
    // AP REG info
    //
    BOOL                                                IsAPREGRequested;
    LARGE_INTEGER                                       APRegAddress;
    PAP_REG_B_FAMILY_HEADER                             ApReg;
    LARGE_INTEGER                                       ArmContextPA;
    BOOL                                                HasValidAP_REG;
    BOOL                                                isAPREG64;
    
    // CPU Context
    LARGE_INTEGER                                       X86ContextPA;

    //
    // Data from Windows
    //
    LARGE_INTEGER                                       DumpHeaderPA;
    LARGE_INTEGER                                       DumpHeaderAddress;
    PDUMP_HEADER32                                      DumpHeader;
    PDUMP_HEADER64                                      DumpHeader64;
    PPHYSICAL_MEMORY_DESCRIPTOR32                       MemoryDescriptors;
    UINT64                                              SizeAccordingToMemoryDescriptors;
    UINT32                                              SecondaryDataBlobCount;

    //
    // Data to decode KdDebuggerDataBlock
    //
    UINT32                                              OfflineDumpKWA;
    UINT32                                              OfflineDumpKWN;
    UINT32                                              OfflineDumpKDBEP;
    UINT32                                              KDBEPValue;
    LARGE_INTEGER                                       KdDebuggerDataBlockPA;
    PKDDEBUGGER_DATA64                                  KdDebuggerDataBlock;
} DMP_CONTEXT, *PDMP_CONTEXT;

# pragma pack ()

static PWSTR RawDumpSectionTypeStringTable[] = {L"RESERVED", 
                                                L"DDR_RANGE",
                                                L"CPU_CONTEXT",
                                                L"SV_SPECIFIC"};

typedef struct _SV_SPECIFIC_NAME_TO_GUID
{
    GUID    Guid;
    UCHAR   Name[RAW_DUMP_SECTION_HEADER_NAME_LENGTH];
}SV_SPECIFIC_NAME_TO_GUID;

#define PAGE_SIZE                       0x1000

#define DUMP_HEADERSIG_LENGTH           8
#define RAW_DUMP_HEADER_SIGNATURE       0x21706d445f776152  // "Raw_Dmp!" - in hex, LittleEndian order
#define RAW_DUMP_SECTION_HEADER_VERSION 0x1000
#define ARM_VALID_PFN_MASK              0xFFFFF000
#define ARMV7_TTBCRN_MASK               0x7
#define INVALID_AP_REG                  0xFFFFFFFF
#define AMD64_BASE_VIRT                0xFFFFF6FB7DBED000UI64
#define AMD64_PAGE_SIZE                0x1000
#define AMD64_PAGE_SHIFT               12L
#define AMD64_MM_PTE_TRANSITION_MASK   0x800
#define AMD64_MM_PTE_PROTOTYPE_MASK    0x400
#define AMD64_VALID_PFN_MASK           0x0000FFFFFFFFF000UI64
#define AMD64_VALID_PFN_SHIFT          12
#define AMD64_USED_VA_BITS             48
#define AMD64_UNUSED_VA_MASK           0xffff
#define AMD64_PML4E_SHIFT              39
#define AMD64_PML4E_MASK               0x1ff
#define AMD64_PDPE_SHIFT               30
#define AMD64_PDPE_MASK                0x1ff
#define AMD64_PDE_SHIFT                21
#define AMD64_PDE_MASK                 0x1ff
#define AMD64_PTE_SHIFT                12
#define AMD64_PTE_MASK                 0x1ff
#define AMD64_LARGE_PAGE_MASK          0x80
#define AMD64_LARGE_PAGE_SIZE          (2 * 1024 * 1024)
#define AMD64_1GB_PAGE_MASK            0x80
#define AMD64_1GB_PAGE_SIZE            (1024 * 1024 * 1024)
#define AMD64_PDBR_MASK                AMD64_VALID_PFN_MASK


#define ARM64_BASE_VIRT                0xFFFFF6FB7DBED000UI64
#define ARM64_PAGE_SIZE                0x1000
#define ARM64_PAGE_SHIFT               12L
#define ARM64_MM_PTE_TRANSITION_MASK   0x800
#define ARM64_MM_PTE_PROTOTYPE_MASK    0x400
#define ARM64_VALID_PFN_MASK           0x0000FFFFFFFFF000UI64
#define ARM64_VALID_PFN_SHIFT          12
#define ARM64_USED_VA_BITS             48
#define ARM64_UNUSED_VA_MASK           0xffff
#define ARM64_PML4E_SHIFT              39
#define ARM64_PML4E_MASK               0x1ff
#define ARM64_PDPE_SHIFT               30
#define ARM64_PDPE_MASK                0x1ff
#define ARM64_PDE_SHIFT                21
#define ARM64_PDE_MASK                 0x1ff
#define ARM64_PTE_SHIFT                12
#define ARM64_PTE_MASK                 0x1ff
#define ARM64_LARGE_PAGE_MASK          0x02
#define ARM64_LARGE_PAGE_SIZE          (2 * 1024 * 1024)
#define ARM64_PTE_VALID_MASK           1
#define ARM64_PTE_IS_VALID             1
#define ARM64_1GB_PAGE_MASK            0x02
#define ARM64_1GB_PAGE_SIZE            (1024 * 1024 * 1024)

#define X86_VALID_PFN_MASK_PAE          0x0000001FFFFFF000UI64
#define SECUREBOOT_DEBUG_GUID \
           {0x0cdad82e, 0xd839, 0x4754, \
           {0x89, 0xa1, 0x84, 0x4a, 0xb2, 0x82, 0x31, 0x2b}};


//
// Copied from other headers.
//

#define ADDRESS_NOT_PRESENT ((UINT64)-1)

//
// ----------------------------------------------------------------- Prototypes
//

NTSTATUS
ExtractRawDumpPartitionToFiles(
    PDMP_CONTEXT Context 
    );

VOID
DumpRawDumpHeader(
    _In_ PRAW_DUMP_HEADER Header
    );

HRESULT 
FindDumpPartition ( 
    PDMP_CONTEXT Context);

NTSTATUS
WriteRAWDDRToBin(
    _Inout_ PDMP_CONTEXT Context
    );

NTSTATUS
ExtractRawDumpToFiles(
        PDMP_CONTEXT Context );

BOOL 
ReadRawDumpPartitionToFile( PDMP_CONTEXT Context,
                            WCHAR* FilePath);

NTSTATUS
ExtractRawDumpFileToFiles(
            PDMP_CONTEXT Context, 
            LPCWSTR FileName );
HRESULT
ExtractSecondaryDataFromDumpFile(
                                LPWSTR dumpfile);

HRESULT
ReconstructDDRSectionsFromDumpFile(
                                LPWSTR dumpfile);

NTSTATUS
ExtractWindowsDumpFromDDR( PDMP_CONTEXT Context, 
                           LPWSTR FileName
                          );

BOOL MergeFile(
    HANDLE hTo, 
    HANDLE hFrom);

BOOL MergeDDRFiles(PDMP_CONTEXT Context, 
                _Inout_ PCOMMAND_LINE_ARGS arguments,
                LPWSTR mergedfile );

BOOL CheckDebugPolicyEnabled();

BOOL
QueryAbnromalResetEvents(
    VOID
    );

BOOL
WipeRawDumpHeader( 
    PDMP_CONTEXT Context);

BOOL
ExistValidOfflineDump(
    PDMP_CONTEXT Context);

NTSTATUS
UpdateContextX86(
_Inout_ PDMP_CONTEXT Context
);

NTSTATUS
TZBSPContextToNTContext(
_In_ PSECURE_CPU_CONTEXT tzctx,
_Out_ PARM_CONTEXT ntctx
);

NTSTATUS
UpdateContextWithAPReg(
_Inout_ PDMP_CONTEXT Context
);

NTSTATUS
WriteAPREG(
_Inout_ PDMP_CONTEXT Context);


NTSTATUS
VerifyRawDumpHeader(
PDMP_CONTEXT Context,
BOOL* IsRawDumpHeaderValid
);

NTSTATUS
VerifyRawDumpSectionTable(
PDMP_CONTEXT Context,
BOOL* IsRawDumpSectionTableValid
);

NTSTATUS
SearchDumperHeaderandAPREG(
_Inout_ PDMP_CONTEXT Context);

NTSTATUS
GetDumpHeader32(
_Inout_ PDMP_CONTEXT Context
);

NTSTATUS
GetDumpHeader64(
_Inout_ PDMP_CONTEXT Context
);

NTSTATUS
BuildDDRMemoryMap(
PDMP_CONTEXT Context
);

NTSTATUS
ReadFromDDRSectionByPhysicalAddress(
_In_ PDMP_CONTEXT Context,
_In_ LARGE_INTEGER PhysicalAddress,
_In_ UINT64 Length,
_Out_ PVOID Buffer
);

NTSTATUS
ValidateDDRAgaisntPhysicalMemoryBlock32(
_Inout_ PDMP_CONTEXT Context
);

NTSTATUS
ValidateDDRAgaisntPhysicalMemoryBlock64(
_Inout_ PDMP_CONTEXT Context
);

NTSTATUS
WriteDumpHeader32(
_Inout_ PDMP_CONTEXT Context
);

NTSTATUS
VirtualToPhysical32(
_In_ PDMP_CONTEXT Context,
_In_ UINT32 VirtualAddress,
_Out_ PLARGE_INTEGER PhysicalAddress
);

NTSTATUS
VirtualToPhysical64(
_In_ PDMP_CONTEXT Context,
_In_ UINT64 VirtualAddress,
_Out_ PLARGE_INTEGER PhysicalAddress
);

HRESULT
InitDumpFile(
_Inout_ PDMP_CONTEXT Context);

NTSTATUS
WriteDDR32(
_Inout_ PDMP_CONTEXT Context
);

NTSTATUS
WriteDDR64(
_Inout_ PDMP_CONTEXT Context
);

NTSTATUS
WriteSVSpecific(
_Inout_ PDMP_CONTEXT Context
);

NTSTATUS
GetKdDebuggerDataBlock32(
_Inout_ PDMP_CONTEXT Context
);

NTSTATUS
WriteToDumpByPhysicalAddress(
_Inout_ PDMP_CONTEXT Context,
_In_ LARGE_INTEGER PhysicalAddress,
_In_ UINT32 Size,
_Inout_ PVOID Buffer
);


NTSTATUS
ExtractWindowsDumpFile32(
PDMP_CONTEXT Context
);

NTSTATUS
ExtractWindowsDumpFile64(
PDMP_CONTEXT Context
);

NTSTATUS
ReadFromDDRSectionByVirtualAddress(
_In_ PDMP_CONTEXT Context,
_In_ UINT64 VirtualAddress,
_In_ UINT64 Length,
_Out_ PVOID Buffer,
_Out_opt_ PLARGE_INTEGER PhysicalAddress
);

NTSTATUS
ReadFromDDRSectionByVirtualAddress32(
_In_ PDMP_CONTEXT Context,
_In_ UINT32 VirtualAddress,
_In_ UINT32 Length,
_Out_ PVOID Buffer,
_Out_opt_ PLARGE_INTEGER PhysicalAddress
);

NTSTATUS
ReadFromDDRSectionByVirtualAddress64(
_In_ PDMP_CONTEXT Context,
_In_ UINT64 VirtualAddress,
_In_ UINT64 Length,
_Out_ PVOID Buffer,
_Out_opt_ PLARGE_INTEGER PhysicalAddress
);

BOOL ReadDisk(
    __in HANDLE hDisk,
    __out_bcount(Length) PVOID Buffer,
    __in ULONG Length,
    __in_opt PLARGE_INTEGER ByteOffset);

NTSTATUS
GetX86CPUContext(
_Inout_ PDMP_CONTEXT Context
);

NTSTATUS
GetAPReg(
_Inout_ PDMP_CONTEXT Context,
_Out_ BOOL* IsApRegValid
);

BOOL
ISContainAPREG(
BYTE* buffer,
LONG buffersize,
LONG* offset);

BOOL
ValidateKdDebuggerDataBlock(
_In_ PDBGKD_DEBUG_DATA_HEADER64 Header
);


BOOL
DecodeDebuggerDataBlock(
    _In_     HANDLE hDmpOut
 );


//#endif // __DUMPUTIL_H__