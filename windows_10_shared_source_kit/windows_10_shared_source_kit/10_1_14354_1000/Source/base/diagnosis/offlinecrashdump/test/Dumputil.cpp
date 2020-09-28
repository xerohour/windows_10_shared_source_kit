/*++

Copyright (c) April 2013 Microsoft Corporation, All Rights Reserved

Module Name:
   Dumputil.cpp

Environment:
   User Mode

Author:
   Jinsheng Shi (jinsh)  04/10/2013
   Jose Pagan (JoPagan)  01/06/2016

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
#include <initguid.h>
#include <WinEvt.h>
#include <wchar.h>
#include "common.h"
#include "DbgClient.h"
#include "DiskUtil.h"
#include "DumpUtil.h"
#include "apreg64.h"

BOOL ReadDisk(
    __in HANDLE hDisk, 
    __out_bcount(Length) PVOID Buffer,
    __in ULONG Length,
    __in_opt PLARGE_INTEGER ByteOffset)
/*++

Routine Description:

    This function Read the content from the disk

Arguments:

    Context - PDMP_CONTEXT
    Buffer - output buffer
    Length - the size of buffer
    ByteOffeset the absolute offset to the disk

Return Value:

    BOOL 

--*/
{
    BYTE*                       tempbuffer;
    LONG                        buffersize;
    LONG                        sector_offset;
    BOOL                        result = FALSE;
    IO_STATUS_BLOCK             statusBlock;
    NTSTATUS                    status = E_FAIL;
	LARGE_INTEGER               alignedoffset = { 0 };

    sector_offset = ByteOffset->QuadPart%DEFUALT_SECTOR_SZ;
    alignedoffset.QuadPart = ByteOffset->QuadPart - sector_offset;
    buffersize = ((Length+sector_offset)/DEFUALT_SECTOR_SZ +1)*DEFUALT_SECTOR_SZ;

    //
    // Allocate the buffer.
    //
    tempbuffer = (BYTE*) malloc( buffersize);
    if (!tempbuffer) {
        LogLibInfoPrintf(L"Could not allocate memory ");
        goto Exit;
    }

    ZeroMemory(tempbuffer, buffersize);
    status = NtReadFile( hDisk,
                        NULL,
                        NULL,
                        NULL,
                        &statusBlock,
                        tempbuffer,
                        buffersize,
                        &alignedoffset,
                        NULL );

    if (!NT_SUCCESS(status)) {
           LogLibErrorPrintf(
            result,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__,
            L"NtReadFile failed. Status:0x%x\r\n",
            status);
        goto Exit;
    }
   
    memcpy(Buffer, tempbuffer + sector_offset, Length);
    result = TRUE;

 Exit:
    if(tempbuffer!= NULL) {
        free(tempbuffer);
    }
    
    return result;
}

BOOL CheckDebugPolicyEnabled()
/*++

Routine Description:

  This routine fetch current SecureBoot policy and compare to the production policy.
  Return true if the current policy is production, 
  else return false.

Arguments:

Return Value:

    BOOL.

--*/
{
    NTSTATUS status;
    SYSTEM_SECUREBOOT_POLICY_INFORMATION policyInfo = {0};

    status = NtQuerySystemInformation(SystemSecureBootPolicyInformation,
                                      &policyInfo,
                                      sizeof(policyInfo),
                                      NULL);

    if (NT_SUCCESS(status)) {

        //Compare current GUID with Production GUID
        GUID SecBootPolicyPublisherDebug = SECUREBOOT_DEBUG_GUID;
        if (memcmp(&SecBootPolicyPublisherDebug, &policyInfo.PolicyPublisher, sizeof(GUID)) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL
QueryAbnromalResetEvents(
    VOID
    )
/*++

Routine Description:

    Check whether abnormal reset event (Windows Kernel Power 142) logged or not

Arguments:

Return Value:

    BOOL

--*/
{
    EVT_HANDLE QueryHandle;
    EVT_HANDLE EventHandle;
    DWORD      Returned;
    BOOL       FoundEvent;

    FoundEvent = FALSE;

    //
    // Obtain a query handle to the system event log
    //

    QueryHandle = EvtQuery(
                    NULL, 
                    L"System", 
                    L"*[System[EventID=142][Provider[@Name=\"Microsoft-Windows-Kernel-Power\"]]]",
                    EvtQueryChannelPath | EvtQueryForwardDirection);

    if (QueryHandle != NULL) {

        //
        // Get the next watchdog timer event
        //

        while (EvtNext(
                QueryHandle,
                1,
                &EventHandle,
                (DWORD)-1,
                0,
                &Returned)) {

            FoundEvent = TRUE;
            EvtClose(EventHandle);
            break;
        }

        EvtClose(QueryHandle);
    }

    return FoundEvent;
}

HRESULT FindDumpPartition ( PDMP_CONTEXT Context)
/*++

Routine Description:

    This function found the raw dump partition on the disk.
    it will enumraete all GPT partitions and compare the GUID RAWDUMP_GUID

Arguments:

     Context - Pointer to DmpContext

Return Value:

    HRESULT.

--*/
{
    PVOID AllocTemp;
    WCHAR DevicePath[MAX_PATH];
    SYSTEM_DEVICE_INFORMATION DeviceInfo;
    PDRIVE_LAYOUT_INFORMATION_EX DriveLayout;
    ULONG DriveLayoutSize;
    HANDLE hDevice;
    ULONG i;
    UINT32 DiskIndex;
    PPARTITION_INFORMATION_EX PartitionEntry;
    ULONG PartitionId;
    ULONG ReturnedLength;
    NTSTATUS Status;
    HRESULT result = E_FAIL;
    hDevice = INVALID_HANDLE_VALUE;
    DriveLayout = NULL;

    //
    // Get the number of Disks in the system
    //
    Status = NtQuerySystemInformation( SystemDeviceInformation, &DeviceInfo, sizeof(SYSTEM_DEVICE_INFORMATION), 0 );
    if ( Status != ERROR_SUCCESS){
         LogLibErrorPrintf(
                result,
                __LINE__,
                WIDEN(__FUNCTION__),
                __WFILE__,
                L" NtQuerySystemInformation. STATUS: 0x%08x", Status);
        goto EXIT;
    }

    //
    // Loop through the disks
    //
    DiskIndex = 0;
    while ( DiskIndex < DeviceInfo.NumberOfDisks ) {

        //
        // Create the Device Path used for getting the handle
        //
        StringCchPrintfW(DevicePath,
                         MAX_PATH,
                         L"\\\\.\\PhysicalDrive%d",
                         DiskIndex);

        //
        // Create a Handle to the Hard Disk
        //
        hDevice = CreateFileW(DevicePath,
                              FILE_GENERIC_READ | FILE_GENERIC_WRITE,
                              FILE_SHARE_READ | FILE_SHARE_WRITE,
                              NULL,
                              OPEN_EXISTING,
                              0,
                              NULL);

        if (hDevice == INVALID_HANDLE_VALUE){
            LogLibErrorPrintf(
                result,
                __LINE__,
                WIDEN(__FUNCTION__),
                __WFILE__,
                L" Create File Failed. INVALID_HANDLE_VALUE. %s\n", DevicePath);
            goto EXIT;
        }

        //
        // Get the Disk Layout Structure.  This doesn't return needed buffer size, so keep trying.
        // Starting with size of disk and 16 partitions.
        // Don't care about skipping offline Dynamic disks.
        //
        ReturnedLength = 0;
        DriveLayoutSize = sizeof(DRIVE_LAYOUT_INFORMATION_EX) + (PARTITION_INFORMATION_SECTION_COUNT * sizeof(PARTITION_INFORMATION_EX));;
        DriveLayout = (PDRIVE_LAYOUT_INFORMATION_EX) malloc(DriveLayoutSize);

        if (DriveLayout == NULL){
            result = E_FAIL;
            LogLibInfoPrintf (L"PartitionInfo: Allocation failed");
            goto EXIT;
        }

        for (i=0; i < 20; i++){

            if (DeviceIoControl ( hDevice,
                                  IOCTL_DISK_GET_DRIVE_LAYOUT_EX,
                                  NULL,
                                  0,
                                  DriveLayout,
                                  DriveLayoutSize,
                                  &ReturnedLength,
                                  NULL) == FALSE ) {

                if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {

                    DriveLayoutSize = DriveLayoutSize * 2;
                    AllocTemp = realloc(DriveLayout, DriveLayoutSize);
                    if (AllocTemp == NULL){
                        result = E_FAIL;
                        LogLibInfoPrintf (L"PartitionInfo: Memory reallocation failed");
                        goto EXIT;
                    }

                    DriveLayout = (PDRIVE_LAYOUT_INFORMATION_EX) AllocTemp;
                    AllocTemp = NULL;
                    ZeroMemory(DriveLayout, DriveLayoutSize);
                
                }else{

                    LogLibInfoPrintf (L"PartitionInfo Get Drive Layout IOCTL failed: %#x", GetLastError());
                    CloseHandle(hDevice);
                    hDevice = INVALID_HANDLE_VALUE;
                    break;
                }

            }else{

                // IOCTL Success
                break;
            }
        }

        // IOCTL failed, try next disk
        if (hDevice == INVALID_HANDLE_VALUE){
            DiskIndex++;
            continue;
        }

        //
        // Only interested in GPT disk, if not the loop again.
        //
        if (DriveLayout->PartitionStyle != PARTITION_STYLE_GPT){
            LogLibInfoPrintf (L"Partition  type not GPT, 0x%x", DriveLayout->PartitionStyle);
            CloseHandle(hDevice);
            hDevice = INVALID_HANDLE_VALUE;
            DiskIndex++;
            continue;
        }

        //
        // Loop through the partitions
        //
        for (PartitionId = 0; PartitionId < DriveLayout->PartitionCount; PartitionId++){
            PartitionEntry = &DriveLayout->PartitionEntry[PartitionId];
            LogLibInfoPrintf(L" Disk Index = %d  Partition number = %d Partition GUID is: \n", DiskIndex, PartitionEntry->PartitionNumber);
            DumpGUID( &PartitionEntry->Gpt.PartitionType);
            

            //
            // Check for the RAW_DUMP partition type
            //
            if (IsEqualGUID(&PartitionEntry->Gpt.PartitionType, &RAWDUMP_GUID)){
                LogLibInfoPrintf(L" Found the RAW_DUMP Partition \n");

                Context->hDisk = hDevice;
                Context->diskoffset = PartitionEntry->StartingOffset;
                Context->RawDumpPartitionLength = PartitionEntry->PartitionLength;
                result = ERROR_SUCCESS;

                goto EXIT;
            }
        }

        CloseHandle(hDevice);
        hDevice = INVALID_HANDLE_VALUE;
        DiskIndex++;

    }

EXIT:
    if (DriveLayout)
        free (DriveLayout);
    return result ;
}

NTSTATUS
VerifyRawDumpHeader(
    PDMP_CONTEXT Context,
    BOOL* IsRawDumpHeaderValid
    )
/*++

Routine Description:

    This function validates the RAW_DUMP_HEADER. The following are checked.

    1. RAW_DUMP_HEADER.Signature
    2. RAW_DUMP_HEADER.Version
    3. RAW_DUMP_HEADER.Flags
    4. RAW_DUMP_HEADER.DumpSize
    5. RAW_DUMP_HEADER.SectionCount

    If things check out, it will allocate and read the complete dump table
    to memory.

Arguments:

    Context - Pointer to DmpContext

    IsRawDumpHeaderValid - Returned to caller. TRUE if RAW_DUMP_HEADER is valid. FALSE if 
                           no valid dump found.

Return Value:

    NT status code.

--*/
{
    UINT32                      expectedBits = 0;
    NTSTATUS                    status = E_FAIL;
    RAW_DUMP_HEADER             dumpHeader;
    LARGE_INTEGER               curoffset = Context->diskoffset;
    BOOL                        result = FALSE;

    Context->RawDumpHeader = NULL;
    Context->RawDumpTableSize = 0;

    *IsRawDumpHeaderValid = FALSE;

    //
    // Read in the header and validate the fields.
    //

    result= ReadDisk(Context->hDisk,
                    &dumpHeader,
                    sizeof(RAW_DUMP_HEADER),
                    &curoffset);

    if (FALSE == result) {
        LogLibInfoPrintf(L"Read RAW_Dump Partition failed. Status:0x%x\r\n", status);
        goto Exit;
    }

    //
    // Check the signature.
    //
    if (*(PUINT64)dumpHeader.Signature != RAW_DUMP_HEADER_SIGNATURE) {
        LogLibInfoPrintf(L"Signature validation failed. Actual: 0x%I64x Expected:0x%I64x\r\n", 
                 *(PUINT64)dumpHeader.Signature,
                 RAW_DUMP_HEADER_SIGNATURE);
        goto Exit;
    }

    //
    // Check the version.
    //
    if (dumpHeader.Version != RAW_DUMP_HEADER_VERSION) {
        LogLibInfoPrintf(L"Version validation failed. Actual: 0x%x Expected: 0x%x\r\n", 
                  dumpHeader.Version,
                  RAW_DUMP_HEADER_VERSION);
        goto Exit;
    }

    //
    // Check for expected flags.
    //
    expectedBits = RAW_DUMP_HEADER_FLAGS_VALID | 
                   RAW_DUMP_HEADER_FLAGS_INSUFFICIENT_STORAGE;

    if ((dumpHeader.Flags & expectedBits) == 0) {
        LogLibInfoPrintf(L"Invalid flags. Flags: 0x%x\r\n", dumpHeader.Flags);
        goto Exit;
    }


    //
    // Check dump size. We don't know before hand the size of the dump
    // so we'll check for a non-zero value.
    //
    if (dumpHeader.DumpSize == 0) {
        LogLibInfoPrintf(L"DumpSize is 0.\r\n");
        goto Exit;
    }

    //
    // There should be at least one section.
    //
    if (dumpHeader.SectionsCount == 0) {
        LogLibInfoPrintf(L"SectionsCount is 0.\r\n");
        goto Exit;
    }

    //
    // Header looks good and looks like we have a dump. 
    // Read the complete table.
    //
    Context->RawDumpTableSize =  sizeof(RAW_DUMP_HEADER) + 
                                 ((dumpHeader.SectionsCount - 1) * 
                                  (sizeof(RAW_DUMP_SECTION_HEADER)));

    //
    // Need to free up this space when we exit. 
    //
    Context->RawDumpHeader = (PRAW_DUMP_HEADER )malloc(Context->RawDumpTableSize);
    if (Context->RawDumpHeader == NULL) {
        LogLibInfoPrintf(L"Failed to allocate 0x%x bytes of memory for dump table. \r\n",
                  Context->RawDumpTableSize);
        goto Exit;
    }
    ZeroMemory(Context->RawDumpHeader, Context->RawDumpTableSize);

    result= ReadDisk(Context->hDisk,
                    Context->RawDumpHeader,
                    Context->RawDumpTableSize,
                    &curoffset);
   
    if (FALSE == result) {
         LogLibErrorPrintf(
            result,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__,L"Read RAW_Dump Partition failed. Status:0x%x\r\n", status);
        goto Exit;
    }

    *IsRawDumpHeaderValid = TRUE;

    LogLibInfoPrintf(L"Dump header validated.\r\n");

    DumpRawDumpHeader(Context->RawDumpHeader);

Exit:
    return status;
}

NTSTATUS
VerifyRawDumpSectionTable(
    PDMP_CONTEXT Context,
    BOOL* IsRawDumpSectionTableValid
    )
/*++

Routine Description:

    This function validates the RAW_DUMP_HEADER.SectionTable. The following are checked.

    1. At least one DDR section.
    2. DDR sections are adjacent to each other?
    3. DDR sections are in ascending order.
    4. No sections with unrecognized section type.
    5. SectionsCount matches number of valid sections.
    6. Only the last section should have the RAW_DUMP_HEADER_FLAGS_INSUFFICIENT_STORAGE flag.

Arguments:

    Context - Pointer to DmpContext

    IsRawDumpSectionTableValid - Returned to the caller to indicate whether we have a good 
                                 section table or not.

Return Value:

    NT status code.

--*/
{
    UINT32                   index = 0;
    PRAW_DUMP_SECTION_HEADER sectionTable = NULL;
    PRAW_DUMP_HEADER         dumpHeader = NULL;
    NTSTATUS                 status = STATUS_UNSUCCESSFUL;
	UINT64                   allocatedDDRStart = 0;
	UINT64                   allocatedDDREnd = 1;
	UINT64                   currentDDRStart = 0;
    UINT64                   currentDDREnd = 0;
    UINT32                   expectedFlagBits = 0;
    UINT32                   lastDDRSectionIndex = 0;

    *IsRawDumpSectionTableValid = FALSE;

    //
    // Initialize the various counters.
    //
    Context->DDRSectionCount = 0;
    Context->SVSectionCount = 0;
    Context->CPUContextSectionCount = 0;
    Context->InvalidSectionCount = 0;
    Context->DDRSectionFragmentationCount = 0;
    Context->DDRSectionsOverlapCount = 0;
    Context->MissingFlagsCount = 0;
    Context->InsufficientStorageSectionsCount = 0;
    Context->InvalidVersionCount = 0;
    Context->LargestSVSpecificSectionSize = 0;

    dumpHeader = Context->RawDumpHeader;
    sectionTable = (PRAW_DUMP_SECTION_HEADER)&dumpHeader->SectionTable[0];

    expectedFlagBits = RAW_DUMP_HEADER_FLAGS_VALID | 
                       RAW_DUMP_HEADER_FLAGS_INSUFFICIENT_STORAGE;

    for (index = 0; index < dumpHeader->SectionsCount; index++)  {
        //
        // Check version
        //
        if (sectionTable[index].Version != RAW_DUMP_SECTION_HEADER_VERSION) {
            LogLibInfoPrintf(L"Section 0x%x has invalid version. Version: 0x%x\r\n",
                      index, 
                      sectionTable[index].Version);

            Context->InvalidVersionCount++;
        }

        //
        // Has the right flags been set?
        //
        if ((sectionTable[index].Flags & expectedFlagBits) == 0) {
            LogLibInfoPrintf(L"Expected flags are not set. Flags: 0x%x\r\n", 
                      sectionTable[index].Flags);
            Context->MissingFlagsCount++;
        }

        //
        // Check if RAW_DUMP_HEADER_FLAGS_INSUFFICIENT_STORAGE flag is 
        // set. If so, it should only be set for the last section.
        //
        if ((sectionTable[index].Flags & RAW_DUMP_HEADER_FLAGS_INSUFFICIENT_STORAGE) ==
            RAW_DUMP_HEADER_FLAGS_INSUFFICIENT_STORAGE) {
            if (index != (dumpHeader->SectionsCount - 1)) {
                LogLibInfoPrintf(L"Section 0x%x has an insufficient storage flag and it is not the last section!\r\n",
                          &sectionTable[index]);

                Context->InsufficientStorageSectionsCount++;
            }
            else  {
                LogLibInfoPrintf(L"The last section has an incomplete flag. Index: 0x%x\r\n", index);
            }
        }

        //
        // Section type-specific checks.
        //
        switch (sectionTable[index].Type) {
            case RAW_DUMP_SECTION_TYPE_DDR_RANGE:
                //
                // Save the pointer to the first DDR section.
                // This will make it easier to access stuff 
                // in DDR sections.
                //
                if (Context->DDRSectionCount == 0) {
                    Context->DDRSections = &sectionTable[index];
                }

                //
                // Do some checking to see if 
                // all DDR sections are adjacent to 
                // each other as called for by the spec.
                //
                if ((Context->DDRSectionCount != 0) &&
                    ((index - lastDDRSectionIndex) != 1)) {
                    Context->DDRSectionFragmentationCount++;
                    LogLibInfoPrintf(L"Discontiguous DDR section found. Index: 0x%x\r\n", 
                              index);
                }

                //
                // Check for overlap.
                // Assume Random layout of DDR secetions, just get the block...
				//   CANNOT assume each subsequent section is ascending
                //   from previous.
                //
                currentDDRStart = sectionTable[index].u.DDRInformation.Base;
                currentDDREnd = currentDDRStart + sectionTable[index].Size - 1;

                //
                // Check for an obvious DDR section errors, exit if any are found...
                //
                if( currentDDREnd < currentDDRStart )
                { // FAILURE: bad DDR section information, likely an invalid dump file...
                    LogLibInfoPrintf(L"ERROR: block 0x%x information is invalid, Start/End of block incorrect.\r\n", index);
                    LogLibInfoPrintf(L"    Start: 0x%x\r\n", currentDDRStart );
                    LogLibInfoPrintf(L"      End: 0x%x\r\n", currentDDREnd );
                    LogLibInfoPrintf(L"     Size: 0x%x\r\n", sectionTable[index].Size );
                    status = STATUS_BAD_DATA;
                    goto Exit;
                }
                else if( sectionTable[index].Size == 0 )
                { // FAILUER: empty DDR section, likely an invalid dump file...
                    LogLibInfoPrintf(L"ERROR: Current block information is invalid, block size is zero.\r\n");
                    status = STATUS_BAD_DATA;
                    goto Exit;
                }

                //
                // Checking for overlap doesn't make sense for the very first DDR 
                // section so simply lay it out as the inital allocation.
                //
				if (Context->DDRSectionCount == 0) {
					allocatedDDRStart = currentDDRStart;
					allocatedDDREnd   = currentDDREnd;
				}
				else if (Context->DDRSectionCount > 0)
				{
					if (currentDDRStart == (allocatedDDREnd + 1))
					{ // Non-Overlap: case is where the current block starts Just after the allocated block
						allocatedDDREnd = currentDDREnd;
					}
					else if (currentDDREnd == (allocatedDDRStart - 1))
					{ // Non-Overlap: case is where the current block starts Just before the allocated block
						allocatedDDRStart = currentDDRStart;
					}
					else if( (currentDDRStart >= allocatedDDRStart) &&
						     (currentDDRStart <= allocatedDDREnd))
					{ // Overlap: case where the current block starts inside the allocated block
						Context->DDRSectionsOverlapCount++;
						LogLibInfoPrintf(L"DDR section 0x%x overlap with already allocated block.\r\n   Currenet block Starts at [0x%I64x] which is inside the existing DRDR memory block:  Start[0x%I64x]  End[0x%I64x]\r\n",
								index,
								currentDDRStart,
								allocatedDDRStart,
								allocatedDDREnd);
					}
					else if( (currentDDREnd >= allocatedDDRStart) &&
							 (currentDDREnd <= allocatedDDREnd) )
					{ // Overlap: case where the current block ends inside the allocated block
						Context->DDRSectionsOverlapCount++;
						LogLibInfoPrintf(L"DDR section 0x%x overlap with already allocated block.\r\n   Currenet block Ends at [0x%I64x] which is inside the existing DRDR memory block:  Start[0x%I64x]  End[0x%I64x]\r\n",
							index,
							currentDDREnd,
							allocatedDDRStart,
							allocatedDDREnd);
					}
					else if (currentDDRStart > (allocatedDDREnd + 1))
					{ // HOLE: case is where the current block starts some distance after the allocated block
						Context->DDRSectionFragmentationCount++;
						LogLibInfoPrintf(L"HOLE at the end of the allocated DDR section found.\r\n   Index: 0x%x\r\n   From end of Allocated [0x%I64x] to beginning of new section [0x%I64x]\r\n",
							index,
							allocatedDDREnd, 
							currentDDRStart);
					}
					else if (currentDDREnd < (allocatedDDRStart - 1))
					{ // HOLE: case is where the current block starts some distance before the allocated block
						Context->DDRSectionFragmentationCount++;
						LogLibInfoPrintf(L"HOLE at the beginning of the allocated DDR section found.\r\n   Index: 0x%x\r\n   From end of new section [0x%I64x] to beginning of allocated section [0x%I64x]\r\n",
							index,
							currentDDREnd,
							allocatedDDRStart);
					}
					else
					{ // Something I did not consider - continue processing because the output should provide as much info as can be gathered from this dump.
						LogLibInfoPrintf(L"An unexpected DDR section found.\r\n   Index: 0x%x\r\n", index );
					}

				}

                lastDDRSectionIndex = index;
                Context->DDRSectionCount++;

                break;

            case RAW_DUMP_SECTION_TYPE_CPU_CONTEXT:
                Context->CPUContextSectionCount++;
                Context->TotalCpuContextSizeInBytes += sectionTable[index].Size;

                switch(sectionTable[index].u.CPUContextInformation.Architecture) {
                    case ARCH_X86:
                                LogLibInfoPrintf(L"This is a x86 machine Dump\n"); 
                                break;
                    case ARCH_ARM:
                                LogLibInfoPrintf(L"This is a ARM machine Dump\n"); 
                                break;
                    case ARCH_X64:
                                LogLibInfoPrintf(L"This is a x64 machine Dump\n"); 
                                break;
                    default:
                                LogLibInfoPrintf(L"This is a unknown type machine Dump\n"); 
                }
                break;

            case RAW_DUMP_SECTION_TYPE_SV_SPECIFIC:
                Context->SVSectionCount++;
                Context->TotalSVSpecificSizeInBytes += sectionTable[index].Size;

                if (Context->LargestSVSpecificSectionSize < sectionTable[index].Size) {
                    Context->LargestSVSpecificSectionSize = sectionTable[index].Size;
                }  
 
                break;

            default:
                Context->InvalidSectionCount++;
                break;
        } //switch (sectionTable[index].Type)
    } //for dumpHeader->SectionsCount

    LogLibInfoPrintf(L"DDR Sections        : 0x%x\r\n",
                     Context->DDRSectionCount);

    LogLibInfoPrintf(L"SV Specific Sections: 0x%x\r\n", 
                     Context->SVSectionCount);

    LogLibInfoPrintf(L"CPU Context Sections: 0x%x\r\n",
                     Context->CPUContextSectionCount);

    if (Context->InvalidVersionCount > 0) {
        LogLibInfoPrintf(L"Sections with invalid versions detected. Count: 0x%x\r\n",
                         Context->InvalidVersionCount);
        goto Exit;
    }

    if (Context->MissingFlagsCount > 0) {
        LogLibInfoPrintf(L"Sections with no flags set detected. Count: 0x%x\r\n",
                         Context->MissingFlagsCount);
        goto Exit;
    }

    if (Context->InsufficientStorageSectionsCount > 0)  {
        LogLibInfoPrintf(L"Unexpected sections with insufficient storage flag. Count: 0x%x\r\n",
                         Context->InsufficientStorageSectionsCount);
        goto Exit;
    }

    if (Context->InvalidSectionCount > 0) {
        LogLibInfoPrintf(L"InvalidSectionCount is not zero. Count: 0x%x\r\n", 
                         Context->InvalidSectionCount);
        goto Exit;
    }

    if (Context->DDRSectionsOverlapCount > 0) {
        LogLibInfoPrintf(L"DDR section overlap detected. Count: 0x%x\r\n", 
                         Context->DDRSectionsOverlapCount);
        goto Exit;
    }

    if (Context->DDRSectionFragmentationCount > 0) {
        LogLibInfoPrintf(L"DDRSections are not adjacent to each other. Count: 0x%x\r\n",
                         Context->DDRSectionFragmentationCount);
        // Not an error --> goto Exit;
    }

    LogLibInfoPrintf(L"Section table validated...\r\n");

    *IsRawDumpSectionTableValid = TRUE;
    status = STATUS_SUCCESS;

Exit:
    return status;
}


NTSTATUS
BuildDDRMemoryMap(
    PDMP_CONTEXT Context
    )
/*++

Routine Description:

   This routine builds a DDRMemoryMap table. 

Arguments:

    Context - Pointer to DmpContext

Return Value:

    NT status code.

--*/
{
    UINT32              allocationSize = 0;
    UINT64              difference = 0;
    UINT32              index;
    NTSTATUS            status = STATUS_UNSUCCESSFUL;

    //
    // We should have aborted if the DDR assumptions were violated.
    // Let's make sure.
    //

    if ((Context->DDRSectionFragmentationCount != 0) &&
        (Context->DDRSectionsOverlapCount != 0)) {
        LogLibInfoPrintf(L"Section table violates assumptions about DDR sections!\r\n");
        goto Exit;
    }

    Context->TotalDDRSizeInBytes = 0;
    Context->DDRMemoryMapCount = 0;
    Context->DDRMemoryMap = NULL;
    allocationSize = sizeof(DDR_MEMORY_MAP) * Context->DDRSectionCount;

    Context->DDRMemoryMap = (PDDR_MEMORY_MAP)malloc(allocationSize);

    if (Context->DDRMemoryMap == NULL) {
        LogLibInfoPrintf(L"Failed to allocate memory for DDR memory map.\r\n");
        status = STATUS_NO_MEMORY;
        goto Exit;
    }

    ZeroMemory(Context->DDRMemoryMap, allocationSize);

    //
    // Fill in the table.
    //
    for (index = 0; index < Context->DDRSectionCount; index++)  {
        if (Context->DDRSections[index].Type != RAW_DUMP_SECTION_TYPE_DDR_RANGE) {
            LogLibInfoPrintf(L"ERROR!!! DDR section 0x%p does not have correct type. Type: 0x%x\r\n",
                      &Context->DDRSections[index],
                      Context->DDRSections[index].Type);
            status = STATUS_BAD_DATA;
            goto Exit;
        }

        Context->DDRMemoryMapCount++;
        Context->DDRMemoryMap[index].Base = Context->DDRSections[index].u.DDRInformation.Base;
        Context->DDRMemoryMap[index].Size = Context->DDRSections[index].Size;
        Context->DDRMemoryMap[index].Offset = Context->DDRSections[index].Offset;
        Context->DDRMemoryMap[index].End =  Context->DDRMemoryMap[index].Base + 
                                            Context->DDRMemoryMap[index].Size - 
                                            1;
        Context->TotalDDRSizeInBytes += Context->DDRMemoryMap[index].Size;
    }

    //
    // Need to sort the DDR Memory maps before processing
    //
    {
        BOOLEAN swapped = TRUE;
        DDR_MEMORY_MAP tmpDdrRecord = {0};
        
		for (unsigned int idx = 0; swapped && (idx < Context->DDRMemoryMapCount - 1); idx++)
		{
			swapped = FALSE;

			for (unsigned int i = idx + 1; (i < Context->DDRMemoryMapCount); i++)
			{
				if (Context->DDRMemoryMap[i].Base < Context->DDRMemoryMap[idx].Base)
				{
					memcpy(&tmpDdrRecord, &Context->DDRMemoryMap[idx], sizeof(tmpDdrRecord));
					memcpy(&Context->DDRMemoryMap[idx], &Context->DDRMemoryMap[i], sizeof(tmpDdrRecord));
					memcpy(&Context->DDRMemoryMap[i], &tmpDdrRecord, sizeof(tmpDdrRecord));
					swapped = TRUE;
				}
			}
		}

		LogLibInfoPrintf(L"-  -  -  -  Sorted DDR Memory  -  -  -  -");
		for (unsigned int idx = 0; (idx < Context->DDRMemoryMapCount); idx++)
		{
			LogLibInfoPrintf(L"\r\n  Section %d:", idx);
			LogLibInfoPrintf(L"      Context->DDRMemoryMap[%02d].Base: 0x%08x", idx, Context->DDRMemoryMap[idx].Base);
			LogLibInfoPrintf(L"      Context->DDRMemoryMap[%02d].Size: 0x%08x", idx, Context->DDRMemoryMap[idx].Size);
			LogLibInfoPrintf(L"    Context->DDRMemoryMap[%02d].Offset: 0x%08x", idx, Context->DDRMemoryMap[idx].Offset);
			LogLibInfoPrintf(L"       Context->DDRMemoryMap[%02d].End: 0x%08x", idx, Context->DDRMemoryMap[idx].End);
		}
		LogLibInfoPrintf(L"\r\n-  -  -  -  -  -  -  -  -  -  -  -  -\r\n");
    }

    //
    // Check address contiguity.
    //
    for (index = 0; index < Context->DDRMemoryMapCount; index++) {
        if (index > 0) {
            difference = Context->DDRMemoryMap[index].Base - 
                         Context->DDRMemoryMap[index - 1].End - 1;

            if (difference == 0) {
                Context->DDRMemoryMap[index].Contiguous = TRUE;
    
                LogLibInfoPrintf(L"Section 0x%x is contiguous to previous.\r\n",
                      difference);
            }
            else {
                LogLibInfoPrintf(L"Section %d is not contiguous to previous. Difference: 0x%x\r\n",
                          index,
                          difference);
            }
        }
    }
    status = STATUS_SUCCESS;
Exit:
    return status;
}

NTSTATUS
ReadFromDDRSectionByPhysicalAddress(
    _In_ PDMP_CONTEXT Context,
    _In_ LARGE_INTEGER PhysicalAddress,
    _In_ UINT64 Length,
    _Out_ PVOID Buffer
    )
/*++

Routine Description:

    This function reads the contents of memory in DDR sections.

Arguments:

    Context - Dmp_CONTEXT

    PhysicalAddress - Physical address of memory in DDR sections which we want
                      to read from.

    Length - Number of bytes to read.

    Buffer - Buffer holding the contents of the read. 

Return Value:

    NT status code.

--*/
{
    UINT64              addressStart = 0; 
    UINT64              addressEnd = 0;
    UINT32              bytesToRead = 0;
    UINT64              bytesRemain = 0;
    UINT32              ddrSectionsCount = 0;
	LARGE_INTEGER       offset = { 0 };
    PDDR_MEMORY_MAP		ddrMap = NULL;
    UINT32              index = 0;
    UINT64              sectionStart = 0;
    UINT64              sectionEnd = 0;
    UINT32              sectionSpanCount = 0;
    NTSTATUS            status = STATUS_UNSUCCESSFUL;;
    PVOID               temp = 0;
    BOOL				result = FALSE;

    ddrSectionsCount = Context->DDRMemoryMapCount;
    ddrMap = Context->DDRMemoryMap;
    addressStart = PhysicalAddress.QuadPart;
    addressEnd = addressStart + Length - 1;
    bytesRemain = Length;
    temp = Buffer;

    //
    // Determine the right section.
    //
    for (index = 0; index < ddrSectionsCount; index++) {
		
		sectionStart = ddrMap[index].Base;
        sectionEnd = ddrMap[index].Base + ddrMap[index].Size -1 ;       

        if ((sectionSpanCount != 0) &&
            (bytesRemain != 0) &&
            (!ddrMap[index].Contiguous)) {
            LogLibInfoPrintf(L"Attempting to read spanning discontiguous sections. "
                      L"Section: 0x%x BytesRemain: 0x%x\r\n",
                      index,
                      bytesRemain);
            status = STATUS_INVALID_PARAMETER;
            goto Exit;
        }

        //
        // Determine if address falls in to this section.
        //
        if ((sectionStart <= addressStart) &&
            (sectionEnd >= addressStart)) {
            LogLibInfoPrintf(L"Section %d:    sectionStart=0x%llx    sectionEnd=0x%llx", index, sectionStart, sectionEnd);
            LogLibInfoPrintf(L"  Memory foind in ddrmap[%d]:", index);
            LogLibInfoPrintf(L"      Base          0x%llx (sectionStart)", ddrMap[index].Base);
            LogLibInfoPrintf(L"      End           0x%llx", ddrMap[index].End);
            LogLibInfoPrintf(L"      Size          0x%llx", ddrMap[index].Size);
            LogLibInfoPrintf(L"      Offset        0x%llx", ddrMap[index].Offset);
            LogLibInfoPrintf(L"      Contiguous    %s", ddrMap[index].Contiguous ? L"True" : L"False");

            //
            // Figure out how many bytes to read.
            // Need to figure this out because we may 
            // cross section boundaries.
            //
            bytesToRead = (sectionEnd >= addressEnd) ? 
                          (UINT32)(addressEnd - addressStart + 1) : 
                          (UINT32)(sectionEnd - addressStart + 1);

            offset.QuadPart  = Context->diskoffset.QuadPart + (addressStart - sectionStart) + ddrMap[index].Offset;

            LogLibInfoPrintf(L"Context->diskoffset.QuadPart=0x%I64x, addressStart=0x%I64x, sectionStart=0x%I64x, ddrMap[%d].Offset=0x%I64x\r\n",
                Context->diskoffset.QuadPart, addressStart, sectionStart, index, ddrMap[index].Offset); 
            LogLibInfoPrintf(L"Reading 0x%x bytes at offset 0x%I64x\r\n", bytesToRead, offset.QuadPart);
            
            result = ReadDisk ( Context->hDisk,
                        temp,
                        bytesToRead,
                        &offset);
            if (result == FALSE) {
                LogLibInfoPrintf(L"[ERROR] ReadDisk Failed with result=FALSE");
                goto Exit;
            }
                
                        
            //
            // Update bytesRemain.
            //
            bytesRemain = bytesRemain - bytesToRead;

            // LogLibInfoPrintf(L"0x%x bytes remain to be read.\r\n", bytesRemain);

            if (bytesRemain != 0)
            {
                //
                // Time to move to next section.
                // Update temp.
                //
                Buffer = (PVOID)((UINT32)(Buffer) + bytesToRead);

                //
                // Update addressStart.
                // AddressEnd should not be updated. 
                //
                addressStart = addressStart + bytesToRead;
                sectionSpanCount++;
            }
            else
            {
                //
                // No more bytes to read. Time to leave.
                //
                status = STATUS_SUCCESS;
                break;
            }
        }//(sectionStart <= addressStart)
    }//for (index = 0; index < ddrSectionsCount; index++)

    //
    // We end up here either because bytesRemain is zero or 
    // we went through all the DDR sections and still couldn't 
    // read all the bytes. 
    //
    if ((index == ddrSectionsCount) &&
        (bytesRemain != 0))
    {
        LogLibInfoPrintf(L"Read incomplete. 0x%x bytes not read out of 0x%x bytes.\r\n", 
                  bytesRemain,
                  Length);

        status = STATUS_UNSUCCESSFUL;
        goto Exit;
    }

Exit: 

    return status;
}



NTSTATUS
ReadFromDDRSectionByVirtualAddress(
    _In_ PDMP_CONTEXT Context,
    _In_ UINT64 VirtualAddress,
    _In_ UINT64 Length,
    _Out_ PVOID Buffer,
    _Out_opt_ PLARGE_INTEGER PhysicalAddress
)
{
    NTSTATUS status;
    ULONG64 PA64;
    IDebugDataSpaces2 *DebugDataSpaces = DbgClient::GetDataSpaces();
    if (DebugDataSpaces == nullptr) {
        LogLibInfoPrintf(L"Failure in dbgeng, KdDebuggerDataBlock may be unreadable");
        return STATUS_UNSUCCESSFUL;
    }
        
    if (Context->Is64Bit) {
//        status = ReadFromDDRSectionByVirtualAddress64(Context,
//                    VirtualAddress,
//                    Length,
//                    Buffer,
//                    PhysicalAddress);

        HRESULT hr = DebugDataSpaces->VirtualToPhysical(VirtualAddress, &PA64);
        if (FAILED(hr)) {
            LogLibInfoPrintf(L"Failed to find physical address Error %x", hr);
            status =  STATUS_UNSUCCESSFUL;
        }
        else{
            status =  STATUS_SUCCESS;
            if (PhysicalAddress != nullptr)
                PhysicalAddress->QuadPart = PA64;
        }
    }
    else {
         status = ReadFromDDRSectionByVirtualAddress32(Context,
                    (UINT32)VirtualAddress,
                    (UINT32)Length,
                    Buffer,
                    PhysicalAddress);
    }
    
    return status;
}


BOOL
ISContainDumpHeader(
                    _In_ PDMP_CONTEXT Context,
                    BYTE* buffer, 
                    LONG buffersize, 
                    LONG* offset)
{
    BOOL                foundDumpHeader = FALSE;
    LONG                i =0;
    LONG                magicstringlength = 0;

    UCHAR PAGEDUMP[] = { // Spells --> PAGEDUMP
    0x50, 0x41, 0x47, 0x45, 0x44, 0x55, 0x4D, 0x50
    };
    
    UCHAR PAGEDUMP64[] = { // Spells --> PAGEDU64
    0x50, 0x41, 0x47, 0x45, 0x44, 0x55, 0x36, 0x34
    };

    // Magic String --> ;ISS”E.0ÔËÚ—ñµè6aˆp›
    UCHAR InMemoryDumpHeaderMagicString[] = {
    0x3B,0x49,0x53,0x53,0x94,0x45,0x2E,0x30,
    0xD4,0xCB,0xDA,0x97,0xF1,0x11,0x02,0xB5,
    0xE8,0x36,0x08,0x61,0x88,0x70,0x9B, 0x19
    };
    
    magicstringlength = sizeof (InMemoryDumpHeaderMagicString) + sizeof (PAGEDUMP);

    for(i =0 ; i<buffersize - magicstringlength; i++)
    {
        if( memcmp((buffer + i), InMemoryDumpHeaderMagicString, sizeof(InMemoryDumpHeaderMagicString)) == 0)
        {
            *offset = i + sizeof(InMemoryDumpHeaderMagicString);
            foundDumpHeader = TRUE;
            LogLibInfoPrintf(L"Found dump header magic string at offset: 0x%llx", i);
            LogLibInfoPrintf(L"      dump header begins at offset: 0x%llx", *offset);

            if( memcmp((buffer + *offset), PAGEDUMP, sizeof(PAGEDUMP)) == 0 )
            {
                LogLibInfoPrintf(L"Dump header type is PAGEDUMP");
                goto EXIT;
            }
            else if (memcmp((buffer + *offset), PAGEDUMP64, sizeof(PAGEDUMP64)) == 0)
            {
                Context->Is64Bit = TRUE;
                LogLibInfoPrintf(L"Dump header type is PAGEDU64");
                goto EXIT;
            }
            else
            { // Invalid Header - continue looking...
                TCHAR szDisplay[max(sizeof(PAGEDUMP), sizeof(PAGEDUMP64)) + 1] = { 0 };
                
                memcpy(szDisplay, (buffer + *offset), sizeof(szDisplay) - 1);

                LogLibInfoPrintf(L"Failed to find dump header type, expected string PAGEDUMP or PAGEDU64");
                LogLibInfoPrintf(L"   Actual: %S", szDisplay);
                foundDumpHeader = FALSE;

            }
        }
    }

EXIT:
    return foundDumpHeader;
}

NTSTATUS
SearchDumperHeaderandAPREG(_Inout_ PDMP_CONTEXT Context)
{
    ULONG               index = 0;
    LONG                offset = 0;
    LONG                buffersize = PAGE_SIZE*1024*8;
    LONGLONG            totalread =0; 
    LARGE_INTEGER       curoffset = Context->diskoffset;
    BOOL                result = FALSE;
    BYTE*               buffer;       
    IO_STATUS_BLOCK     statusBlock;
    NTSTATUS            status = E_FAIL;
    BOOL                foundDumpHeader = FALSE;
    BOOL                foundAPRG;
    
    Context->Is64Bit = FALSE;    
    //
    // if this is 64 bit APREG, dont attempt to find th eAPREG in the memory
    //
    foundAPRG = (Context->isAPREG64 ||  !Context->IsAPREGRequested) ? TRUE : FALSE; 
    
    //
    // calculate need read how many times 
    //
    totalread  =  Context->RawDumpPartitionLength.QuadPart/buffersize;
    //
    // Allocate the buffer.
    //
    buffer = (BYTE*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, buffersize);
    if (!buffer) {
        LogLibInfoPrintf(L"Could not allocate memory for 4MB ");
        goto EXIT;
    }

    for(index = 0; index < totalread; index++) {
        status = NtReadFile( Context->hDisk,
                            NULL,
                            NULL,
                            NULL,
                            &statusBlock,
                            buffer,
                            buffersize,
                            &curoffset,
                            NULL );

        if(!NT_SUCCESS(status)) {
            LogLibInfoPrintf(L"Failed to Read partition! ");
            goto EXIT;
        }

        if(!foundDumpHeader){
            foundDumpHeader = ISContainDumpHeader(Context, buffer, buffersize, &offset);
            if (foundDumpHeader) {
                Context->DumpHeaderAddress.QuadPart = curoffset.QuadPart + offset;
                
                if(foundAPRG) {
                    LogLibInfoPrintf(L"We found the dumpheader and other sections!");
                    goto EXIT;
                }
            }
        }

        if(!foundAPRG){
            foundAPRG = ISContainAPREG(buffer, buffersize, &offset);
            if(foundAPRG) {
	            Context->APRegAddress.QuadPart = curoffset.QuadPart + offset;
		        LogLibInfoPrintf(L"we found the AP_REG at 0x%llx (%lld)",  Context->APRegAddress.QuadPart, Context->APRegAddress.QuadPart);
            
			    if(foundDumpHeader) {
                    LogLibInfoPrintf(L"We found both dumpheader and AP_REG!");
                    goto EXIT;
                }
            }
        }

		curoffset.QuadPart += buffersize;
		wprintf(L"      ");
		switch (index%4)
		{
			case 0:
				wprintf(L"-");
				break;
			case 1:
				wprintf(L"\\");
				break;
			case 2:
				wprintf(L"|");
				break;
			case 3:
				wprintf(L"/");
				break;
			default:
				wprintf(L"*");
				break;
		}
        wprintf(L" we searched %ld MB   \r", (index*buffersize)/(1024*1024));
    }
	status = STATUS_SUCCESS;

EXIT:
	LogLibInfoPrintf(L"       Completed search %ld MB ", (index*buffersize) / (1024 * 1024));
	if(!foundDumpHeader){
        status = E_FAIL;
        LogLibErrorPrintf(
                result,
                __LINE__,
                WIDEN(__FUNCTION__),
                __WFILE__, 
                L"We Searched the whole memory could not find pre-built Dump header Status:0x%x\r\n", status);
    }

    if (foundAPRG !=TRUE) {
        Context ->HasValidAP_REG = FALSE;
        LogLibInfoPrintf(L"Error:Failed to find APReg, looks like this offline dump not triggered by secure watchdog! \n"); 
    }

    if (buffer != NULL) {
        HeapFree(GetProcessHeap(), 0, buffer);
    }  
    return status;
}

VOID
DumpRawDumpHeader(
    _In_ PRAW_DUMP_HEADER Header
    )
{
    ULONG index;
    UINT32 sectionCount; 
    UINT32 namelength;
    PRAW_DUMP_SECTION_HEADER    sectionTable = NULL;
    
    CHAR sig[9];
    WCHAR signature[RAW_DUMP_HEADER_SIGNATURE_LENGTH + 1];

    CHAR name[RAW_DUMP_SECTION_HEADER_NAME_LENGTH];    
    WCHAR sectionName[RAW_DUMP_SECTION_HEADER_NAME_LENGTH+1];

    sectionName[RAW_DUMP_SECTION_HEADER_NAME_LENGTH] = 0;
    signature[RAW_DUMP_HEADER_SIGNATURE_LENGTH] = 0;
    
    LogLibInfoPrintf(L"*******RAW_DUMP_HEADER: 0x%p*******\r\n", Header);
    LogLibInfoPrintf(L"\tSignature: ");
    memset(sig, 0, RAW_DUMP_HEADER_SIGNATURE_LENGTH);
    memcpy(sig, (void*) &(Header->Signature), RAW_DUMP_HEADER_SIGNATURE_LENGTH);
    mbstowcs(signature, sig, RAW_DUMP_HEADER_SIGNATURE_LENGTH);

    LogLibInfoPrintf(L"%s", signature);
    LogLibInfoPrintf(L"\r\n");

    LogLibInfoPrintf(L"\tVersion: 0x%x\r\n", Header->Version);
    LogLibInfoPrintf(L"\tFlags: 0x%x\r\n", Header->Flags);
    LogLibInfoPrintf(L"\tOsData: 0x%I64x\r\n", Header->OsData);
    LogLibInfoPrintf(L"\tCpuContext: 0x%I64x\r\n", Header->CpuContext);
    LogLibInfoPrintf(L"\tResetTrigger: 0x%x\r\n", Header->ResetTrigger);
    LogLibInfoPrintf(L"\tDumpSize: 0x%I64x\r\n", Header->DumpSize);
    LogLibInfoPrintf(L"\tTotalDumpSizeRequire: 0x%I64x\r\n", Header->TotalDumpSizeRequired);
    LogLibInfoPrintf(L"\tSectionsCount: 0x%x\r\n", Header->SectionsCount);

    sectionCount = Header->SectionsCount;

    sectionTable = (PRAW_DUMP_SECTION_HEADER)&Header->SectionTable[0];

    for (index = 0; index < sectionCount; index++)
    {
        LogLibInfoPrintf(L"Section[%d]: 0x%p\r\n", index, &sectionTable[index]);
        LogLibInfoPrintf(L"\t\tFlags: 0x%x\r\n", sectionTable[index].Flags);
        LogLibInfoPrintf(L"\t\tVersion: 0x%x\r\n", sectionTable[index].Version);
        
        if (sectionTable[index].Type <  RAW_DUMP_SECTION_TYPE_Maximum)
        {
            LogLibInfoPrintf(L"\t\tType: 0x%x => %s\r\n", 
                     sectionTable[index].Type,
                     RawDumpSectionTypeStringTable[sectionTable[index].Type]);
        }
        else
        {
            LogLibInfoPrintf(L"\t\tType: 0x%x => UNKNOWN\r\n", sectionTable[index].Type);
        }

        LogLibInfoPrintf(L"\t\tOffset: 0x%I64x\r\n", sectionTable[index].Offset);
        LogLibInfoPrintf(L"\t\tSize: 0x%I64x\r\n", sectionTable[index].Size);

        switch (sectionTable[index].Type)
        {
            case RAW_DUMP_SECTION_TYPE_Reserved:
                LogLibInfoPrintf(L"\t\tType is reserved!!\r\n");
                break;

            case RAW_DUMP_SECTION_TYPE_DDR_RANGE:
                LogLibInfoPrintf(L"\t\tDumping DDR_INFORMATION...\r\n");
                LogLibInfoPrintf(L"\t\tBase: 0x%I64x\r\n", sectionTable[index].u.DDRInformation.Base);
                break;

            case RAW_DUMP_SECTION_TYPE_CPU_CONTEXT:
                LogLibInfoPrintf(L"\t\tDumping CPU_CONTEXT_INFORMATION...\r\n");
                break;

            case RAW_DUMP_SECTION_TYPE_SV_SPECIFIC:
                LogLibInfoPrintf(L"\t\tDumping SV_SPECIFIC_INFORMATION...\r\n");
                LogLibInfoPrintf(L"\t\t");
                DumpGUID((GUID*)sectionTable[index].u.SVSpecificInformation.SVSpecificData);
                break;
            default:
                LogLibInfoPrintf(L"\t\tUNKNOWN section type. 0x%x\r\n", sectionTable[index].Type);
                break;
        }
        
        memset(name, 0, RAW_DUMP_SECTION_HEADER_NAME_LENGTH);
        memcpy(name, (void*) &(sectionTable[index].Name), RAW_DUMP_SECTION_HEADER_NAME_LENGTH);
        namelength = (UINT32)strlen(name);
        sectionName[namelength] = 0;
        mbstowcs(sectionName, name, namelength);
        LogLibInfoPrintf(L"\t\tName: %s\r\n", sectionName);
    }

}

HRESULT 
InitDumpFile(
        _Inout_ PDMP_CONTEXT Context)
{
    LARGE_INTEGER    fileoffset;
    DWORD            flag = 0;
    HRESULT          result = E_FAIL;
    Context->DedicatedDumpHandle = INVALID_HANDLE_VALUE;
    Context->DedicatedDumpFilePath = L"raw_dump.dmp";

    fileoffset.QuadPart = 0;
    flag = FILE_ATTRIBUTE_NORMAL;
    flag |= FILE_FLAG_OVERLAPPED;

    Context->DedicatedDumpHandle  = CreateFileW( Context->DedicatedDumpFilePath,
                                                    GENERIC_READ | GENERIC_WRITE,
                                                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                                    NULL,
                                                    OPEN_ALWAYS,
                                                    flag,
                                                    NULL );

    if (Context->DedicatedDumpHandle  == INVALID_HANDLE_VALUE) {
        LogLibErrorPrintf(
            result,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__,L"Failed to Create entire file!\n");
        goto Exit;
    }
	wprintf(L"Created Dedicated Dump File \'%s\', Context->DedicatedDumpHandle=0x%x!\n", Context->DedicatedDumpFilePath, (UINT32)Context->DedicatedDumpHandle);

    //
    // Get the sum of storage requirements for data.
    //
    Context->ActualDumpFileUsedInBytes.QuadPart = Context->SizeAccordingToMemoryDescriptors + Context->TotalCpuContextSizeInBytes;

    //
    // Get the sum of storage requiremetns for metadata.
    //
    if (Context->Is64Bit==TRUE)
    {
        Context->ActualDumpFileUsedInBytes.QuadPart += sizeof(DUMP_HEADER64);
    }
    else
    {
        Context->ActualDumpFileUsedInBytes.QuadPart += sizeof(DUMP_HEADER32);
    }

    LogLibInfoPrintf(L"  0x%llx (%llu) bytes are required to store the dump contents.\r\n", 
              Context->ActualDumpFileUsedInBytes, Context->ActualDumpFileUsedInBytes);
    
    result = ERROR_SUCCESS;

Exit:
    return result;
}

NTSTATUS
WriteRAWDDRToBinAndSearchHeaders(
    _Inout_ PDMP_CONTEXT Context
    )
/*++

Routine Description:

    This function writes RAW DDR sections to bin files 

Arguments:

    Context - Pointer to the global context structure.

Return Value:

    NT status code.

--*/
{
    UINT32                  index = 0;
    UINT32                  counter = 0;
    NTSTATUS                status = STATUS_UNSUCCESSFUL;
    ULONG                   bufferSize = DEFAULT_DMP_BUF_SZ; //4MB
    ULONG                   readbufferSize = 0;
    LONGLONG                remainsize = 0; //4MB
    PVOID                   tempBuffer = NULL;
    LARGE_INTEGER           offset;
    LARGE_INTEGER           sectionEnd;
    LARGE_INTEGER           fileoffset;
    DWORD                   flag = 0;
    HRESULT                 result = E_FAIL;
    HANDLE                  hFile = INVALID_HANDLE_VALUE;
    IO_STATUS_BLOCK         statusBlock;
    WCHAR                   Filename[MAX_PATH];
    LONG                    internalOffset = 0;
    BOOL                    foundDumpHeader = FALSE;
    BOOL                    foundAPRG;

    Context->Is64Bit = FALSE;
    //
    // if this is 64 bit APREG, dont attemp to find th eAPREG in the memory
    //
    foundAPRG = (Context->isAPREG64 || !Context->IsAPREGRequested) ? TRUE : FALSE; 
     
    tempBuffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bufferSize);
    if (tempBuffer == NULL) {
        LogLibInfoPrintf(L"Failed to allocate intermediate buffer for writing SV specific sections. Size: %u\r\n", 
                  Context->LargestSVSpecificSectionSize);
        goto Exit;
    }

    for (index = 0; index < Context->DDRSectionCount; index++)  {
        if (Context->DDRSections[index].Type != RAW_DUMP_SECTION_TYPE_DDR_RANGE) {
            LogLibInfoPrintf(L"ERROR!!! DDR section 0x%p does not have correct type. Type: 0x%x\r\n",
                        &Context->DDRSections[index],
                        Context->DDRSections[index].Type);
            status = STATUS_BAD_DATA;
            goto Exit;
        }

        //
        // Only include DDR sections that are valid.
        //
        if ((Context->DDRSections[index].Flags & RAW_DUMP_HEADER_FLAGS_VALID) != 
            RAW_DUMP_HEADER_FLAGS_VALID) {
            LogLibInfoPrintf(L"Skipping DDR section with incomplete memory.\r\n");
           //continue;
        }

        if(Context->DumpDDR == TRUE){
            wsprintf(Filename, L"DDRSection_%d.bin", index);
            fileoffset.QuadPart = 0;
            flag = FILE_ATTRIBUTE_NORMAL;
            flag |= FILE_FLAG_OVERLAPPED;
            hFile = CreateFileW( Filename,
                                GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                NULL,
                                OPEN_ALWAYS,
                                flag,
                                NULL );
            if (hFile == INVALID_HANDLE_VALUE){
                    LogLibErrorPrintf(
                    E_FAIL,
                    __LINE__,
                    WIDEN(__FUNCTION__),
                    __WFILE__,
                    L" Create File Failed. INVALID_HANDLE_VALUE. %ls\n", Filename);
                goto Exit;
            }
        }
        
        offset.QuadPart = Context->diskoffset.QuadPart + Context->DDRSections[index].Offset;
        sectionEnd.QuadPart = Context->diskoffset.QuadPart + Context->DDRSections[index].Offset + Context->DDRSections[index].Size ;
        readbufferSize = bufferSize;
		LogLibInfoPrintf(L"  Processing DDR Section %d, File: %ls", index, Filename);

        while (offset.QuadPart < sectionEnd.QuadPart){

			wprintf(L"      ");
			switch (counter%4)
			{
				case 0:
					wprintf(L"-");
					break;
				case 1:
					wprintf(L"\\");
					break;
				case 2:
					wprintf(L"|");
					break;
				case 3:
					wprintf(L"/");
					break;
				default:
					wprintf(L"*");
					counter++;
					break;
			}
			wprintf(L" Processing DDR Memory %d MB     \r", counter*bufferSize / (1024 * 1024));
			counter++;

	        if (!ReadDisk(Context->hDisk,
                          tempBuffer,
                          readbufferSize,
                          &offset
                          )) {
                LogLibInfoPrintf(L"Failed to read DDR section from device. Status: 0x%x\r\n",
                            status);
                goto Exit;
		    }
            
			if(Context->DumpDDR == TRUE){
                status = NtWriteFile( hFile,
                                NULL,
                                NULL,
                                NULL,
                                &statusBlock,
                                tempBuffer,
                                readbufferSize,
                                &fileoffset,
                                NULL ); 
			if (!NT_SUCCESS(status)) {
                    LogLibInfoPrintf(L"Failed to write DDR section to Bin file. Status: 0x%x\r\n", 
                                status);
                    goto Exit;
                }

                NtFlushBuffersFile(hFile, &statusBlock);
                fileoffset.QuadPart += readbufferSize;
			}

			//
			// as we already read data to memory we can process it
			//
			if(!foundDumpHeader){
                foundDumpHeader = ISContainDumpHeader(Context, (BYTE*)tempBuffer, readbufferSize, &internalOffset);
                if(foundDumpHeader) {
                    Context->DumpHeaderAddress.QuadPart = offset.QuadPart + internalOffset;
                    LogLibInfoPrintf(L"We found the magicstring at %llx", Context->DumpHeaderAddress.QuadPart);
                }
            }

            if(!foundAPRG){
                foundAPRG = ISContainAPREG((BYTE*)tempBuffer , readbufferSize, &internalOffset);
                if(foundAPRG) {
                    Context->APRegAddress.QuadPart = offset.QuadPart + internalOffset;
                    LogLibInfoPrintf(L"we found the AP_REG at 0x%llx",  Context->APRegAddress.QuadPart);
                }
            }
            
            if(foundAPRG && foundDumpHeader && (!Context->DumpDDR) && !(Context->isAPREG64)){
             LogLibInfoPrintf(L"we found both AP_REG and dump header");
             break;            
            }
            
            offset.QuadPart += readbufferSize;
            remainsize = sectionEnd.QuadPart - offset.QuadPart;
            if (remainsize  < readbufferSize) {
                readbufferSize = (ULONG)remainsize;
            }
        }
        readbufferSize = bufferSize;
        if(Context->DumpDDR == TRUE){
            LogLibInfoPrintf(L"\r      DDR Section %d - write completed",index );
            CloseHandle(hFile);
        }
		LogLibInfoPrintf(L"      Processed DDR Memory %d MB.\n", counter*bufferSize / (1024 * 1024));
	}
	status = STATUS_SUCCESS;

Exit:
    if(!foundDumpHeader){
        status =E_FAIL;
        LogLibErrorPrintf(
                result,
                __LINE__,
                WIDEN(__FUNCTION__),
                __WFILE__, 
                L"We Searched the whole memory could not find pre-built Dump header Status:0x%x\r\n", status);
    }
    
    if (!foundAPRG) {
        Context ->HasValidAP_REG = FALSE;
        LogLibInfoPrintf(L"Error:Failed to find APReg, looks like this offline dump not triggered by secure watchdog! \n"); 
    }
    if (tempBuffer != NULL) {
        HeapFree(GetProcessHeap(),0, tempBuffer);
    }
    return status;
}


NTSTATUS
WriteSVSpecific(
    _Inout_ PDMP_CONTEXT Context
    )
/*++

Routine Description:

    This function writes SV Specific sections based on the DUMP_HEADER's physical memory descriptor 
    to dump file.

Arguments:

    Context - Pointer to the global context structure.

Return Value:

    NT status code.

--*/
{
    UINT32                   sectionsCount = 0;
    UINT32                   sectionIndex = 0;
    UINT32                   namelength = 0 ;
    PRAW_DUMP_SECTION_HEADER section = NULL;
    NTSTATUS                status = STATUS_UNSUCCESSFUL;
    PVOID                   tempBuffer = NULL;
    LARGE_INTEGER           offset;
    WCHAR                   Filename[MAX_PATH];
    CHAR                    name[RAW_DUMP_SECTION_HEADER_NAME_LENGTH];    
    WCHAR                   sectionName[RAW_DUMP_SECTION_HEADER_NAME_LENGTH+1];
    LARGE_INTEGER           fileoffset;
    DWORD                   flag = 0;
    HANDLE                  hFile = INVALID_HANDLE_VALUE;
    IO_STATUS_BLOCK         statusBlock;

    sectionName[RAW_DUMP_SECTION_HEADER_NAME_LENGTH] = 0;

    sectionsCount = Context->RawDumpHeader->SectionsCount;
    tempBuffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (UINT32)Context->LargestSVSpecificSectionSize);
    if (tempBuffer == NULL) {
        LogLibInfoPrintf(L"Failed to allocate intermediate buffer for writing SV specific sections. Size: %u\r\n", 
                  Context->LargestSVSpecificSectionSize);
        goto Exit;
    }

    for (sectionIndex = 0; sectionIndex < sectionsCount; sectionIndex++) {
        section = &(Context->RawDumpHeader->SectionTable[sectionIndex]);
        if (section->Type == RAW_DUMP_SECTION_TYPE_SV_SPECIFIC)  {
            offset.QuadPart = Context->diskoffset.QuadPart + section->Offset;

            if (!ReadDisk(Context->hDisk,
                    tempBuffer,
                    (ULONG)section->Size,
                    &offset
                    )) {
                LogLibInfoPrintf(L"Failed to read SV section from device. Status: 0x%x\r\n",
                          status);
                goto Exit;
            }

            memset(name, 0, RAW_DUMP_SECTION_HEADER_NAME_LENGTH);
            memcpy(name, (void*) &(section->Name), RAW_DUMP_SECTION_HEADER_NAME_LENGTH);
            namelength = (UINT32)strlen(name);
            if(0 == namelength) {
                   LogLibErrorPrintf(
                    E_FAIL,
                    __LINE__,
                    WIDEN(__FUNCTION__),
                    __WFILE__,
                    L" The SV Section Name length is 0!!! Seciton Index is = %d \n", sectionIndex);
                    goto Exit;
            }
            sectionName[namelength] = 0;
            mbstowcs(sectionName, name, namelength);
            //write to disk
            wsprintf(Filename, L"SV_%s.bin", sectionName);

            fileoffset.QuadPart = 0;
            flag = FILE_ATTRIBUTE_NORMAL;
            flag |= FILE_FLAG_OVERLAPPED;

            hFile = CreateFileW( Filename,
                                GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                NULL,
                                OPEN_ALWAYS,
                                flag,
                                NULL );
            if (hFile == INVALID_HANDLE_VALUE){
                   LogLibErrorPrintf(
                    E_FAIL,
                    __LINE__,
                    WIDEN(__FUNCTION__),
                    __WFILE__,
                    L" Create File Failed. INVALID_HANDLE_VALUE. %s\n", Filename);
                goto Exit;
            }
           
            status = NtWriteFile( hFile,
                        NULL,
                        NULL,
                        NULL,
                        &statusBlock,
                        tempBuffer,
                        (ULONG)section->Size,
                        &fileoffset,
                        NULL ); 
            if (!NT_SUCCESS(status)) {
                LogLibInfoPrintf(L"Failed to write SV section to dump file. Status: 0x%x\r\n", 
                            status);
                goto Exit;
            }

            CloseHandle(hFile);
            LogLibInfoPrintf(L"Written SV section to File %s.\r\n", Filename);
        } // RAW_DUMP_SECTION_TYPE_SV_SPECIFIC
    }//for sectionindex

    status = STATUS_SUCCESS;
Exit:
    if (tempBuffer != NULL) {
        HeapFree(GetProcessHeap(),0, tempBuffer);
    }

    return status;
}

BOOL
ValidateKdDebuggerDataBlock(
    _In_ PDBGKD_DEBUG_DATA_HEADER64 Header
    )
/*++

Routine Description:

    This function will consider that there's a valid data block 
    if the following conditions are met:
    1. Header.Size is sizeof(KDDEBUGGER_DATA64)
    2. Header.OwnerTag is 'GBDK'

Arguments:

    Header - Pointer to the KdDebuggerDataBlock.Header.

Return Value:

    TRUE - Datablock is valid.
--*/
{
    if (Header->Size != sizeof(KDDEBUGGER_DATA64))  {
        LogLibInfoPrintf(L"Header size does not equal sizeof KDDEBUGGER_DATA64. Actual: 0x%x Expected: 0x%x\r\n",
                          Header->Size,
                          sizeof(KDDEBUGGER_DATA64));
        return FALSE;
    }

    if (Header->OwnerTag != KDBG_TAG) {
        LogLibInfoPrintf(L"OwnerTag does not match expected. Actual: 0x%x Expected: 0x%x\r\n",
                          Header->OwnerTag,
                          KDBG_TAG);
        return FALSE;
    }

    LogLibInfoPrintf(L"KdDebuggerDataBlock.Header checks out.\r\n");
    return TRUE;
}

NTSTATUS
WriteToDumpByPhysicalAddress(
    _Inout_ PDMP_CONTEXT Context,
    _In_ LARGE_INTEGER PhysicalAddress,
    _In_ UINT32 Size,
    _Inout_ PVOID Buffer
    )
/*++

Routine Description:

    This function reads and writes to the dedicated dump file based on physical address.

    Important assumption is that the physical memory descriptors are not contiguous and that
    all pages should be contained in one run.

Arguments:

    Context - Pointer to PDMP_CONTEXT

    IsWrite - Boolean. TRUE for write. FALSE for read.

    PhysicalAddress - The base physical address of memory to be read from or written to in the 
                      dedicated dump file.

    Size - Size of buffer in bytes to write. 

    Buffer - Pointer to the buffer. 

Return Value:

    NT status code.

--*/
{
    BOOLEAN                         foundDescriptor = FALSE;
    LARGE_INTEGER                   endPA;
    LARGE_INTEGER                   endPD;
    UINT32                          indexPD = 0;
    LARGE_INTEGER                   ioOffset;
    LARGE_INTEGER                   lengthPD;
    LARGE_INTEGER                   offset ;
    PPHYSICAL_MEMORY_DESCRIPTOR32   physDesc = NULL;
    LARGE_INTEGER                   startPA;
    LARGE_INTEGER                   startPD;
    NTSTATUS                        status =  STATUS_UNSUCCESSFUL;
    IO_STATUS_BLOCK                 statusBlock;

    endPA.QuadPart = 0;
    endPD.QuadPart = 0;
    startPA.QuadPart = 0;
    startPD.QuadPart = 0;
    ioOffset.QuadPart = 0;
    offset.QuadPart = 0;
    lengthPD.QuadPart = 0;

    physDesc = &(Context->DumpHeader->PhysicalMemoryBlock);

    startPA.QuadPart = PhysicalAddress.QuadPart;
    endPA.QuadPart = startPA.QuadPart + Size - 1;

    LogLibInfoPrintf(L"Looking up the physical memory descriptor containing the memory range of interest. Start: 0x%I64x End: 0x%I64x\r\n",
             startPA, 
             endPA);

    //
    // Look for the memory containing the memory
    //
    for (indexPD = 0; indexPD < physDesc->NumberOfRuns; indexPD++) {
        startPD.QuadPart = physDesc->Run[indexPD].BasePage * PAGE_SIZE;
        lengthPD.QuadPart = physDesc->Run[indexPD].PageCount * PAGE_SIZE;
        endPD.QuadPart = startPD.QuadPart + lengthPD.QuadPart - 1;

        if ((startPA.QuadPart >= startPD.QuadPart) &&
            (endPD.QuadPart >= endPA.QuadPart)) {
            foundDescriptor = TRUE;
            LogLibInfoPrintf(L"Descriptor %u contains the memory. Base: 0x%I64x End:0x%I64x Offset: 0x%I64x\r\n",
                      indexPD,
                      startPD.QuadPart, 
                      endPD.QuadPart,
                      offset.QuadPart);
            break;
        }

        offset.QuadPart += lengthPD.QuadPart;
    }

    if (!foundDescriptor) {
        LogLibInfoPrintf(L"Failed to locate physical memory descriptor.\r\n");
        status = STATUS_INVALID_PARAMETER;
        goto Exit;
    }

    //
    // Now that we got the memory descriptor, calculate the offset and
    // perform the IO.
    //
    ioOffset.QuadPart = (startPA.QuadPart - startPD.QuadPart) + offset.QuadPart + Context->DDRFileOffset.QuadPart;

    LogLibInfoPrintf(L"IO offset is: 0x%I64x\r\n", ioOffset.QuadPart);

    status = NtWriteFile( Context->DedicatedDumpHandle,
                            NULL,
                            NULL,
                            NULL,
                            &statusBlock,
                            Buffer,
                            Size,
                            &ioOffset,
                            NULL ); 
    if (!NT_SUCCESS(status)) {
        LogLibInfoPrintf(L"Failed to write to dump file at offset 0x%I64x for %u bytes.\r\n", 
                    ioOffset, 
                    Size);
        goto Exit;
    }
    
    NtFlushBuffersFile(Context->DedicatedDumpHandle, &statusBlock);
    status = STATUS_SUCCESS;

Exit:
    LogLibInfoPrintf(L"Context: 0x%I64x\r\n", &(Context->DumpHeader->PhysicalMemoryBlock));

    return status;
}

BOOL ReadRawDumpPartitionToFile( PDMP_CONTEXT Context,
                                 WCHAR* FilePath)
/*++

Routine Description:

    This function Read Raw Dump Partition to a single file.

Arguments:

    Context - Pointer to PDMP_CONTEXT
    FilePath - the bin file name
Return Value:

    TURE/FALSE

--*/
{
    BOOL            result = FALSE;
    BYTE*           buffer;
    DWORD           flag = 0;
    LONG            counter = 0;
    LARGE_INTEGER   curoffset;
    LARGE_INTEGER   fileoffset;
    LONGLONG        RemainingSize = 0;
    LONGLONG        partend = 0;
    ULONG           ReadbufferSize = DEFAULT_DMP_BUF_SZ;
    IO_STATUS_BLOCK statusBlock;
    NTSTATUS        status;
    HANDLE          hFile = INVALID_HANDLE_VALUE; 

    curoffset =  Context->diskoffset;
    partend = Context->diskoffset.QuadPart + Context->RawDumpPartitionLength.QuadPart;
    fileoffset.QuadPart = 0;

    flag = FILE_ATTRIBUTE_NORMAL;
    flag |= FILE_FLAG_OVERLAPPED;

    hFile = CreateFileW( FilePath,
                        GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                        NULL,
                        OPEN_ALWAYS,
                        flag,
                        NULL );
    //
    // Allocate the buffer.
    //
    buffer = (BYTE*) malloc( ReadbufferSize );
    if (!buffer) {
        LogLibInfoPrintf(L"Could not allocate memory for buffer ");
        goto EXIT;
    }

    while (curoffset.QuadPart < partend ) {
        //
        // Read data from disk
        //
        ZeroMemory(buffer, ReadbufferSize);

        status = NtReadFile( Context->hDisk,
                            NULL,
                            NULL,
                            NULL,
                            &statusBlock,
                            buffer,
                            (ULONG)ReadbufferSize,
                            &curoffset,
                            NULL );

        if(!NT_SUCCESS(status)) {
            result = FALSE;
            LogLibInfoPrintf(L"Failed to Read partition! ");
            goto EXIT;
        }

         status = NtWriteFile( hFile,
                            NULL,
                            NULL,
                            NULL,
                            &statusBlock,
                            buffer,
                            (ULONG)ReadbufferSize,
                            &fileoffset,
                            NULL ); 
   
         
         if(!NT_SUCCESS(status)) {
            result = FALSE;
            LogLibInfoPrintf(L"Failed to Write to file! ");
            goto EXIT;
            
        }
         
        status = NtFlushBuffersFile(hFile, &statusBlock);
        if (NT_SUCCESS(status) && (status!=STATUS_PENDING))  {
            status = statusBlock.Status;
        }

         fileoffset.QuadPart += ReadbufferSize;
         curoffset.QuadPart += ReadbufferSize;
         
         RemainingSize = partend - curoffset.QuadPart;
         if (RemainingSize< DEFAULT_DMP_BUF_SZ) {
             ReadbufferSize = (ULONG)RemainingSize;
         }

         //Write data to file
         counter++;
         LogLibInfoPrintf(L"%ld MB written to file successfully! ", counter*2);
    }
    
    result = TRUE;
    LogLibInfoPrintf(L"Writing raw memory data to file completed successfully! ");

EXIT:
    if (buffer != NULL) {
        free (buffer);
    }

    if (hFile != INVALID_HANDLE_VALUE) {
    CloseHandle (hFile);
    }

    return result;

}

NTSTATUS
ExtractRawDumpPartitionToFiles(PDMP_CONTEXT Context )
{
    NTSTATUS    status = STATUS_UNSUCCESSFUL;
    HRESULT     result = ERROR_SUCCESS;
    
    LogLibInfoPrintf(L"=========== Raw dump is expected. Finding the dedicated partition. ===========\r\n");
    result = FindDumpPartition(Context);
    if (result != ERROR_SUCCESS){
            LogLibErrorPrintf(
            result,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__, 
            L"Error: Failed to Find the RAW_Dump Partition : %d\n",
            GetLastError());
            goto Exit;
    }
      
    LogLibInfoPrintf(L"=========== Found the partition. Checking if there is a valid RAW_DUMP_HEADER. ===========\r\n");

    status = ExtractRawDumpToFiles(Context);

Exit:
    return status;
}

BOOL
ExistValidOfflineDump( PDMP_CONTEXT Context)
/*++

Routine Description:

    This function validate whether there is a valid offlinedump on the disk.

Arguments:

    Context - Pointer to PDMP_CONTEXT
Return Value:

    TURE/FALSE

--*/
{
    BOOL        ValidateResult= FALSE;
    HRESULT     result = ERROR_SUCCESS;
    
    LogLibInfoPrintf(L"=========== Raw dump is expected. Finding the dedicated partition. ===========\r\n");
    result = FindDumpPartition(Context);
    if (result != ERROR_SUCCESS){
            LogLibErrorPrintf(
            result,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__, 
            L"Error: Failed to Find the RAW_Dump Partition: %d\n",
            GetLastError());
            goto Exit;
    }
      
    LogLibInfoPrintf(L"=========== Found the partition. Checking if there is a valid RAW_DUMP_HEADER. ===========\r\n");

    VerifyRawDumpHeader(Context, &ValidateResult);

Exit:
    return ValidateResult;

}

BOOL
WipeRawDumpHeader( PDMP_CONTEXT Context)
/*++

Routine Description:

    This function will wipe the Raw Dump Partition header.

Arguments:

    Context - Pointer to PDMP_CONTEXT
Return Value:

    TURE/FALSE

--*/
{
    BOOL            WipePartiton= FALSE;
    HRESULT         result = ERROR_SUCCESS;
    BYTE*            buffer;
    IO_STATUS_BLOCK statusBlock;
    NTSTATUS        status = E_FAIL;
    
    LogLibInfoPrintf(L"=========== Raw dump is expected. Finding the dedicated partition. ===========\r\n");
    result = FindDumpPartition(Context);
    if (result != ERROR_SUCCESS){
            LogLibErrorPrintf(
            result,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__, 
            L"Error: Failed to Find the RAW_Dump Partition: %d\n",
            GetLastError());
            goto Exit;
    }
   
    LogLibInfoPrintf(L"=========== Found the Raw Dump header ===========\r\n");
    buffer = (BYTE*) malloc( DEFUALT_SECTOR_SZ );
    ZeroMemory( buffer,  DEFUALT_SECTOR_SZ );
    status =  NtWriteFile( Context->hDisk,
                            NULL,
                            NULL,
                            NULL,
                            &statusBlock,
                            buffer,
                            DEFUALT_SECTOR_SZ,
                            &Context->diskoffset,
                            NULL ); 
    
    free(buffer);

    if (!NT_SUCCESS(status)) {
    LogLibInfoPrintf(L"Failed to wipe dump header. Status: 0x%x\r\n", 
                status);
    goto Exit;
    }

    WipePartiton = TRUE;
    LogLibInfoPrintf(L"=========== Wipped the Raw Dump header ===========\r\n");

Exit:
    return WipePartiton;

}

NTSTATUS
ExtractRawDumpFileToFiles(
            PDMP_CONTEXT Context, 
            LPCWSTR FileName )
{
    NTSTATUS   status = E_FAIL;

    Context->hDisk = INVALID_HANDLE_VALUE;
    Context->diskoffset.QuadPart = 0;
    Context->RawDumpPartitionLength .QuadPart = 0 ;

    Context->hDisk = CreateFileW (FileName, FILE_READ_DATA, 0, NULL, OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
                                    NULL);
    if (Context->hDisk == INVALID_HANDLE_VALUE) {
        LogLibErrorPrintf(
                            E_FAIL,
                            __LINE__,
                            WIDEN(__FUNCTION__),
                            __WFILE__, 
                            L"Error: Failed to Open Raw Dump Partition file \n");
        goto Exit;
    }

    if(!GetFileSizeEx(Context->hDisk, &Context->RawDumpPartitionLength)) {
        LogLibErrorPrintf(
                            E_FAIL,
                            __LINE__,
                            WIDEN(__FUNCTION__),
                            __WFILE__, 
                            L"Error: Failed to Get File Size \n");
        goto Exit;
    }

    if ( 0 == Context->RawDumpPartitionLength.QuadPart) {
        LogLibErrorPrintf(
                            E_FAIL,
                            __LINE__,
                            WIDEN(__FUNCTION__),
                            __WFILE__, 
                            L"Error: RawDumpPartitionLength is invalid \n");
    }
        
    status = ExtractRawDumpToFiles(Context);

Exit:
    return status;
}

NTSTATUS
ExtractRawDumpToFiles(PDMP_CONTEXT Context )
{
    NTSTATUS   status = STATUS_UNSUCCESSFUL;
    BOOL        ValidateResult= FALSE;
    HRESULT     hr;

    Context->DumpHeaderAddress.QuadPart = 0;
    Context->DumpHeader = NULL;    
    Context->ApReg = NULL;


    LogLibInfoPrintf(L"=========== Found the partition. Checking if there is a valid RAW_DUMP_HEADER. ===========\r\n");
    VerifyRawDumpHeader(Context, &ValidateResult);
    if (ValidateResult == FALSE) {
            LogLibErrorPrintf(
            E_FAIL,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__, 
            L"Error: Raw Dump Partition header is invalid \n");
            goto Exit;
    }

    LogLibInfoPrintf(L"=========== Found a valid dump header. Validating the section table. ===========\r\n");
    VerifyRawDumpSectionTable(Context, &ValidateResult);
      
    if (ValidateResult == FALSE) {
            LogLibErrorPrintf(
            E_FAIL,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__, 
            L"Error: Raw Dump Partition Section table is invalid \n");
            goto Exit;
    }
     
    LogLibInfoPrintf(L"=========== Got a valid section table. Building a memory based on DDR sections. ===========\r\n");
    status = BuildDDRMemoryMap(Context);
    if ( FAILED(status)) {
            LogLibErrorPrintf(
            E_FAIL,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__, 
            L"Error: Failed to Build DDR Memory Map \n");
            goto Exit;
    }
     
 
    LogLibInfoPrintf(L"=========== Write DDR sections to file ===========\r\n");
    status = WriteRAWDDRToBinAndSearchHeaders(Context);
    if ( FAILED(status)) {
            LogLibErrorPrintf(
            E_FAIL,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__, 
            L"Error: Failed to Write DDR to files \n");
            goto Exit;
    }
    
     if(Context->isAPREG64) {
        //
        // 64 bit APREG format.
        //
        LogLibInfoPrintf(L"=========== This is a 64 bit APREG format ===========\r\n");
        if(Context->APRegAddress.QuadPart == 0) {
            LogLibInfoPrintf(L"=========== Trying to get APREG Address from OCIMEM ===========\r\n");
            hr = GetAPREG64AddrFromOCIMEMSection(Context);
            if ( FAILED(hr)) {
                LogLibErrorPrintf(
                E_FAIL,
                __LINE__,
                WIDEN(__FUNCTION__),
                __WFILE__, 
                L"Error: Failed to get APREG Address from OCIMEM section \n");
                goto Exit;
            }
            LogLibInfoPrintf(L" Got APREG from OCIMEM\r\n");
        }     
        LogLibInfoPrintf(L"=========== Parsing 64 bit APREG format ===========\r\n");        
        hr = FindAndParseAPREG64(Context, &ValidateResult);
        if ( FAILED(hr)) {
            LogLibErrorPrintf(
            E_FAIL,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__, 
            L"Error: Failed to Parse APREG sections \n");
            goto Exit;
        }
    }
    
    LogLibInfoPrintf(L"=========== Writing Silicon Specific data to bins ===========\r\n");
    status = WriteSVSpecific(Context);
    if ( FAILED(status)) {
        LogLibErrorPrintf(
        E_FAIL,
        __LINE__,
        WIDEN(__FUNCTION__),
        __WFILE__, 
        L"Error: Failed WriteSVSpecific \n");
        goto Exit;
    }

    if (Context->Is64Bit){
        status = ExtractWindowsDumpFile64(Context);
    }
    else {
        status = ExtractWindowsDumpFile32(Context);
    }

Exit:
    return status;
}

BOOL MergeFile(HANDLE hTo, HANDLE hFrom)
{
    BOOL     fRet = FALSE;
    DWORD    bufferSize = DEFAULT_DMP_BUF_SZ;
    PBYTE    pbBuf = NULL;
    DWORD    cbDone;
    LONG     counter = 0;
	DWORD    bytesRead = 0;

   
    pbBuf = (PBYTE) malloc(bufferSize);

    if (NULL == pbBuf){
        LogLibErrorPrintf(
                        E_FAIL,
                        __LINE__,
                        WIDEN(__FUNCTION__),
                        __WFILE__, 
                        L"Error: Failed to allocate buffer");
        goto Exit;
    }

    SetFilePointer(hTo, 0, 0, FILE_END);
	for (int i = 0; ;i++) {
		if (!ReadFile(hFrom, pbBuf, bufferSize, &cbDone, NULL)) {
			LogLibInfoPrintf(L"[ERROR] read the input file failed.\n");
			goto Exit;
		}
		if (cbDone == 0) {
 			LogLibInfoPrintf(L"[INFO] reached the end of file at 0X%X.\n", bytesRead);
			break;
		}
		bytesRead += cbDone;

		if (!WriteFile(hTo, pbBuf, cbDone, &cbDone, NULL)) {
			LogLibInfoPrintf(L"[ERROR] Write to merged file failed at offset 0x%x.\n", bytesRead);
			goto Exit;
		}
		counter++;
		wprintf(L"    ");
		switch (i)
		{
			case 4: 
				i = 0;
			case 0:
				wprintf(L"-");
				break;
			case 1:
				wprintf(L"\\");
				break;
			case 2:
				wprintf(L"|");
				break;
			case 3:
				wprintf(L"/");
                break;
            default:
				wprintf(L"*");
                i=3;
                break;
		}
		wprintf(L" merged %d MB.\r", counter*bufferSize / (1024 * 1024));
	}
	LogLibInfoPrintf(L"    Completed merge of %d MB.\n", counter*bufferSize / (1024 * 1024));

    fRet = TRUE;

Exit:
    if (pbBuf) {
        free(pbBuf);
    }
    return fRet;
}


BOOL MergeDDRFiles( PDMP_CONTEXT Context, 
                   PCOMMAND_LINE_ARGS arguments,
                   LPWSTR mergedfile )
{
    HANDLE hmerged = INVALID_HANDLE_VALUE;
    ULONGLONG lowestbase = 0xFFFFFFFFFFFFFFFF;
    LARGE_INTEGER filesize = { 0 };
    UINT sectionid = 0;
    UINT mergedsections = 0 ;
    ULONGLONG Gap = 0;
    HANDLE  hFrom = INVALID_HANDLE_VALUE;
    BOOL bRet = FALSE;

    Context->DDRMemoryMap = new DDR_MEMORY_MAP[arguments->DDRCount];
    ZeroMemory(Context->DDRMemoryMap, (sizeof(DDR_MEMORY_MAP)*arguments->DDRCount) );

    Context->DDRMemoryMapCount = arguments->DDRCount ;
    Context->DDRSectionCount = arguments->DDRCount;

//    CopyArgsToDdrMap(Context, &arguments->ddr[0], arguments->DDRCount);
//    SortDdrArgsByBase(&arguments->ddr[0], arguments->DDRCount);


#ifndef _DEBUG_NO_MERGE
    hmerged = CreateFileW( mergedfile,
                            GENERIC_READ | GENERIC_WRITE,
                            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                            NULL,
                            CREATE_ALWAYS,
                            FILE_FLAG_WRITE_THROUGH,
                            NULL );

    if (hmerged == INVALID_HANDLE_VALUE){
            LogLibErrorPrintf(
            E_FAIL,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__,
            L" Create File Failed. INVALID_HANDLE_VALUE. %ls\n", mergedfile);
        goto Exit;
    }
#endif

    wprintf(L"Merge file \'%s\' successfully created (handle=0x%08x) \n\n", mergedfile, (UINT32)hmerged);


    //
    // Find the lowest DDR section for use as the base address
    //
    for( UINT index = 0; index < arguments->DDRCount; index++) {
		if (!arguments->ddr[index].AlreadyMerged) {
			if( ( (ULONGLONG)arguments->ddr[index].DDRBase.QuadPart) < lowestbase) {
				lowestbase = (ULONGLONG)arguments->ddr[index].DDRBase.QuadPart;
			}
		}
    }
	wprintf(L"Lowest DDR section; lowestbase = 0x%llx\n", lowestbase);

	// To merge the DDR sections and extract the windows dump file.
	wprintf(L"Begin DDR section Merge for %d files\n", arguments->DDRCount );
	for (mergedsections = 0; mergedsections < arguments->DDRCount; mergedsections++){
		ULONGLONG currentBaseOffset = 0xffffffffffffffff;

		//
		// Find the lowest available DDR section (base)
		//
		for( UINT index = 0; index < arguments->DDRCount; index++) {
			if (arguments->ddr[index].AlreadyMerged == TRUE) {
				continue;
			}
			else if( (ULONGLONG)arguments->ddr[index].DDRBase.QuadPart < currentBaseOffset )
			{ // This will revise the value to the lowest one
				currentBaseOffset = arguments->ddr[index].DDRBase.QuadPart;
				sectionid = index;
			}
		}
		wprintf(L"Processing DDR section --> sectionid = %d, base = 0x%llx, File = %ls\n", sectionid, currentBaseOffset, arguments->ddr[sectionid].DDRFileName);

		//
        // the identified section is the lowest DDR section. Read the file, merge it
        //
        if(mergedsections > 0) {           
			Gap = arguments->ddr[sectionid].DDRBase.QuadPart - currentBaseOffset;
		}

        if(Gap!= 0 ){
			LogLibInfoPrintf(
				L"======= The DDR sections are discontinuous --> hole = 0x%llx, DDRbase = 0x%llx, currentbase = 0x%llx ============\n",
                Gap,
				arguments->ddr[sectionid].DDRBase.QuadPart,
				currentBaseOffset);
        }

		hFrom = CreateFileW(arguments->ddr[sectionid].DDRFileName,
							GENERIC_READ,
                            0,
                            NULL, 
                            OPEN_EXISTING,
                            0,
                            NULL);
        if (hFrom == INVALID_HANDLE_VALUE) {
                LogLibErrorPrintf(
                                E_FAIL,
                                __LINE__,
                                WIDEN(__FUNCTION__),
                                __WFILE__, 
								L"Error: Failed to Open input file: %ls\n", arguments->ddr[sectionid].DDRFileName);
            goto Exit;
        }
        wprintf(L"  Reading DDR section %d from file: %ls\n", sectionid, arguments->ddr[sectionid].DDRFileName);
            
        if(!GetFileSizeEx(hFrom, &filesize)) {
            LogLibErrorPrintf(
					E_FAIL,
	                __LINE__,
		            WIDEN(__FUNCTION__),
			        __WFILE__, 
				    L"Error: Failed to Get File Size \n");
            goto Exit;
        }
		wprintf(L"    size of file 0x%llx (%lld)\n", filesize, filesize);

#ifndef _DEBUG_NO_MERGE
        if (!MergeFile(hmerged, hFrom)) {
                LogLibErrorPrintf(
                E_FAIL,
                __LINE__,
                WIDEN(__FUNCTION__),
                __WFILE__,
				L" Failed to merge File %s\n", arguments->ddr[sectionid].DDRFileName);
				goto Exit;
        }
#endif
		Context->DDRMemoryMap[mergedsections].Base = arguments->ddr[sectionid].DDRBase.QuadPart;
		Context->DDRMemoryMap[mergedsections].Size = filesize.QuadPart;

		if( currentBaseOffset >= lowestbase ) {
			Context->DDRMemoryMap[mergedsections].Offset = currentBaseOffset - lowestbase;
		}
        else {
			Context->DDRMemoryMap[mergedsections].Offset = currentBaseOffset;
		}

		Context->DDRMemoryMap[mergedsections].End = arguments->ddr[sectionid].DDRBase.QuadPart + filesize.QuadPart - 1;
		Context->DDRMemoryMap[mergedsections].Contiguous = TRUE;
		Context->TotalDDRSizeInBytes += filesize.QuadPart;

		currentBaseOffset = arguments->ddr[sectionid].DDRBase.QuadPart + filesize.QuadPart;

		arguments->ddr[sectionid].AlreadyMerged = TRUE;

        if (hFrom != INVALID_HANDLE_VALUE)
        {
            CloseHandle(hFrom);
        }
		
        //merged successfully 
        if (hmerged != INVALID_HANDLE_VALUE)
        {
            FlushFileBuffers(hmerged);
        }

        LogLibInfoPrintf(L"         Context->DDRMemoryMap[%d].Base = 0x%I64x", mergedsections, Context->DDRMemoryMap[mergedsections].Base);
        LogLibInfoPrintf(L"          Context->DDRMemoryMap[%d].End = 0x%I64x", mergedsections, Context->DDRMemoryMap[mergedsections].End );
        LogLibInfoPrintf(L"         Context->DDRMemoryMap[%d].Size = 0x%I64x", mergedsections, Context->DDRMemoryMap[mergedsections].Size);
		LogLibInfoPrintf(L"   Context->DDRMemoryMap[%d].Contiguous = 0x%ls",   mergedsections, (Context->DDRMemoryMap[mergedsections].Contiguous?L"TRUE":L"FALSE") );
        LogLibInfoPrintf(L"                      currentBaseOffset = 0x%I64x", currentBaseOffset);
        LogLibInfoPrintf(L"       Context->DDRMemoryMap[%d].Offset = 0x%I64x", mergedsections, Context->DDRMemoryMap[mergedsections].Offset);
        LogLibInfoPrintf(L"                      filesize.QuadPart = 0x%I64x", filesize.QuadPart);
	}
    
    if (hmerged != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hmerged);
    }
    bRet = TRUE;

Exit:
    return bRet;
}

NTSTATUS
ExtractWindowsDumpFromDDR( PDMP_CONTEXT Context, 
                           LPWSTR FileName
                          )
{
    NTSTATUS   status = STATUS_SUCCESS;
    BOOL        ValidateResult= FALSE;
    HRESULT     hr;

    Context->hDisk = INVALID_HANDLE_VALUE;
    Context->diskoffset.QuadPart = 0;
    Context->RawDumpPartitionLength .QuadPart = 0 ;
    Context->DDRFileOffset.QuadPart = 0;
    Context->CPUContextSectionCount = 1;
    Context->SVSectionCount = 0;
    Context->TotalSVSpecificSizeInBytes = 0;    
    Context->DumpHeaderAddress.QuadPart = 0;
    Context->DumpHeader = NULL;
    Context->ApReg = NULL;


    Context->hDisk = CreateFileW (FileName, FILE_READ_DATA, 0, NULL, OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
                                    NULL);
    if (Context->hDisk == INVALID_HANDLE_VALUE) {
        LogLibErrorPrintf(
                            E_FAIL,
                            __LINE__,
                            WIDEN(__FUNCTION__),
                            __WFILE__, 
                            L"Error: Failed to Open Raw Dump Partition file \n");
        goto Exit;
    }
	wprintf(L"Opened Raw Dump Partition file \'%s\' for read access (handle=0x%08x\n", FileName, (UINT32)Context->hDisk );

    if(!GetFileSizeEx(Context->hDisk, &Context->RawDumpPartitionLength)) {
        LogLibErrorPrintf(
                            E_FAIL,
                            __LINE__,
                            WIDEN(__FUNCTION__),
                            __WFILE__, 
                            L"Error: Failed to Get File Size \n");
        goto Exit;
    }
	wprintf(L"  file size = 0x%llx (%lld)\n", Context->RawDumpPartitionLength, Context->RawDumpPartitionLength);

    if ( 0 == Context->RawDumpPartitionLength.QuadPart) {
        LogLibErrorPrintf(
                            E_FAIL,
                            __LINE__,
                            WIDEN(__FUNCTION__),
                            __WFILE__, 
                            L"Error: RawDumpPartitionLength is invalid \n");
    }
  
    
    LogLibInfoPrintf(L" Try searching windows dump header and other sections. ===========\r\n");
    status = SearchDumperHeaderandAPREG(Context);
    if ( FAILED(status)) {
        LogLibErrorPrintf(
        E_FAIL,
        __LINE__,
        WIDEN(__FUNCTION__),
        __WFILE__, 
        L"Error: Failed get windows dump header, no need continue!\n");
        goto Exit;
   // Dont go to exit as of now
    }
    
    if(Context->isAPREG64) {
        //
        // 64 bit APREG format.
        // If APREG64 Address is provided in the command line, do not try to find the address 
        // from OCIMEM section.
        //
        if(Context->APRegAddress.QuadPart == 0) {
            LogLibInfoPrintf(L"=========== Trying to get APREG Address from OCIMEM ===========\r\n");
            hr = GetAPREG64AddrFromOCIMEMSection(Context);
            if ( FAILED(hr)) {
                LogLibErrorPrintf(
                E_FAIL,
                __LINE__,
                WIDEN(__FUNCTION__),
                __WFILE__, 
                L"Error: Failed to get APREG Address from OCIMEM section \n");
                goto Exit;
            }
            LogLibInfoPrintf(L" Got APREG from OCIMEM\r\n");
        }       
        LogLibInfoPrintf(L"=========== Parsing 64 bit APREG format ===========\r\n");        
        hr = FindAndParseAPREG64(Context, &ValidateResult);
        if ( FAILED(hr)) {
            LogLibErrorPrintf(
            E_FAIL,
            __LINE__,
            WIDEN(__FUNCTION__),
            __WFILE__, 
            L"Error: Failed to Parse APREG sections \n");
            goto Exit;
        }
    }    

    else if(Context->IsAPREGRequested && Context->HasValidAP_REG) {
    
        // This code path is for 32 bit (legacy) APREG format.        
        LogLibInfoPrintf(L" ============ Try getting AP_REG. (Legacy) ===========\r\n");
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
    }
    //
    //
    // Extract Windows dumps
    //
    
    if (Context->Is64Bit){
        status = ExtractWindowsDumpFile64(Context);
    }
    else {
        status = ExtractWindowsDumpFile32(Context);
    }

Exit:
    return status;
}
