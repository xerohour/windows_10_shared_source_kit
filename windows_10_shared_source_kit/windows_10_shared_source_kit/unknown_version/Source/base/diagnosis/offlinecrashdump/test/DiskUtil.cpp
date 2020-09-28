/*++

    Copyright (C) Microsoft. All rights reserved.

Module Name:
   DiskUtil.cpp

Environment:
   User Mode

Author:
   radutta 
--*/

#include "DiskUtil.h"
#include "common.h"

NTSTATUS
SetDiskLayout(
    IN HANDLE diskHandle,
    IN PDRIVE_LAYOUT_INFORMATION_EX DriveLayout
    )

/*++

Routine Description:

    Perform the NT API actions of opening Partition0 for
    the specified drive and setting the drive layout.

Arguments:

    diskHandle  - HANDLE to disk

    DriveLayout - new layout to set

Return Value:

    NT status

--*/

{
    IO_STATUS_BLOCK statusBlock;
    ULONG           bufferSize;

    bufferSize = FIELD_OFFSET(DRIVE_LAYOUT_INFORMATION_EX, PartitionEntry) +
                 DriveLayout->PartitionCount * sizeof(PARTITION_INFORMATION_EX);

    return NtDeviceIoControlFile(diskHandle,
                                 0,
                                 NULL,
                                 NULL,
                                 &statusBlock,
                                 IOCTL_DISK_SET_DRIVE_LAYOUT_EX,
                                 DriveLayout,
                                 bufferSize,
                                 NULL,
                                 0);
}

ULONG
GetNewPartitionNumber(
    IN PDRIVE_LAYOUT_INFORMATION_EX DriveLayout
    )

/*++

Routine Description:

    Returns the Partition Number which should be assigned to a new partition in the layout

Arguments:

    DriveLayout - pointer to pointer for the drive layout

Return Value:

    Partition Number

--*/

{
    ULONG                          partitionNum;
    DWORD                          partitionCount;
    UINT                           iPartition;

    partitionCount = DriveLayout->PartitionCount;
    for (partitionNum = 1;;partitionNum++) {
        iPartition = 0;
        while ( (DriveLayout->PartitionEntry[iPartition].PartitionNumber != partitionNum) &&
                (iPartition < partitionCount) ) {
            iPartition++;
        }
        if ( iPartition >= partitionCount ) {
            // we found something with no matches
            break;
        }
    }
    return partitionNum;
}

VOID
GetFreeSpaceAtEnd(
    IN LONGLONG DiskSize,
    IN PDRIVE_LAYOUT_INFORMATION_EX DriveLayout,
    OUT LONGLONG* FreeSpace
    )

/*++

Routine Description:

    Returns available free space present at the end of the drive

Arguments:

    FreeSpace - Amount of space to look for
    DiskSize - Size of the disk
    FreeSpace - FreeSPace on the disk

Return Value:

    Boolean indicating if enough space is present at the end

--*/

{
    DWORD                          partitionStyle;
    DWORD                          partitionCount;
    UINT                           iPartition;
    PARTITION_INFORMATION_EX       partitionInfo;
    LONGLONG                       partitionEndOffset;
    LONGLONG                       highestEndingOffset = 0;

    partitionStyle = DriveLayout->PartitionStyle;
    partitionCount = DriveLayout->PartitionCount;

    LogLibInfoPrintf( L"PartitionCount: %d\n", partitionCount ); 

    if ( partitionStyle == PARTITION_STYLE_GPT ) {
        if (partitionCount >= DriveLayout->Gpt.MaxPartitionCount) {
            *FreeSpace = 0;
        }
    } 

    for (iPartition = 0; iPartition < partitionCount; iPartition++) {
        partitionInfo = DriveLayout->PartitionEntry[iPartition];
        partitionEndOffset = partitionInfo.StartingOffset.QuadPart + 
                             partitionInfo.PartitionLength.QuadPart;


        if (
            ( partitionInfo.PartitionStyle == PARTITION_STYLE_GPT ) ||
            ( ( partitionInfo.PartitionStyle == PARTITION_STYLE_MBR ) &&
              ( ( partitionInfo.Mbr.PartitionType != PARTITION_XINT13_EXTENDED ) &&
                ( partitionInfo.Mbr.PartitionType != PARTITION_EXTENDED )
              )
            ) 
           ) {
            if ( partitionEndOffset  > highestEndingOffset ) {
                 highestEndingOffset = partitionEndOffset;
            } 
        }

    }
    LogLibInfoPrintf( L"Highest ending offset: %I64d\n", highestEndingOffset  );
    *FreeSpace = DiskSize - highestEndingOffset;
    return;
}

NTSTATUS
InsertPartitionGpt(
    IN PDRIVE_LAYOUT_INFORMATION_EX DriveLayout,
    IN ULONG PartitionNumber,
    IN LONGLONG DiskLength, 
    IN LONGLONG PartitionLength,
    IN HANDLE DiskHandle
)

    
/*++

Routine Description:

    Adds a partition of the given length to a GPT DriveLayout

Arguments:

    DriveLayout - pointer to pointer for the drive layout
    DiskLength - size of the disk
    PartitionLength - size of the partition in bytes

Return Value:

    NT status

--*/

{
    DWORD                        partitionCount;
    UINT                         iPartition;
    LONGLONG                     diskoffset;
    LONGLONG                     filledarea;
    PDRIVE_LAYOUT_INFORMATION_EX currentLayout; 
    NTSTATUS                     status;
    UNREFERENCED(DiskLength);

    currentLayout = DriveLayout;
    partitionCount = currentLayout->PartitionCount;
    diskoffset = 0;


    iPartition = partitionCount; // partition entries are all next to each other

    filledarea = currentLayout->PartitionEntry[iPartition - 1].StartingOffset.QuadPart +
                 currentLayout->PartitionEntry[iPartition - 1].PartitionLength.QuadPart;

    diskoffset = filledarea + DEFUALT_ALIGNMENT - (filledarea % DEFUALT_ALIGNMENT);

    status = ZeroPartitionFirstSector (DiskHandle, diskoffset);
    if (!NT_SUCCESS(status)) {
        LogLibInfoPrintf( L"Error writing First Sector %d\n", status);               
        return status;
    }

    LogLibInfoPrintf( L"Adding at %d\n",  iPartition); 

    currentLayout->PartitionEntry[iPartition].PartitionStyle = PARTITION_STYLE_GPT;
    currentLayout->PartitionEntry[iPartition].StartingOffset.QuadPart = diskoffset;            
    currentLayout->PartitionEntry[iPartition].PartitionLength.QuadPart = PartitionLength;
    currentLayout->PartitionEntry[iPartition].RewritePartition = TRUE;
    currentLayout->PartitionEntry[iPartition].PartitionNumber = PartitionNumber; // It's ok to have gaps or collide with others
    currentLayout->PartitionEntry[iPartition].Gpt.PartitionType = RAWDUMP_GUID;
    swprintf(currentLayout->PartitionEntry[iPartition].Gpt.Name, L"SVRawDump");
    currentLayout->PartitionEntry[iPartition].Gpt.Attributes = 0x0;
    // partition GUID is auto-assigned    
    currentLayout->PartitionCount = partitionCount + 1;

    LogLibInfoPrintf( L"Incrementing count to: %d\n",  partitionCount + 1); 
    
    return STATUS_SUCCESS;

}

NTSTATUS
OpenHandle(
    IN WCHAR* Path,
    IN PHANDLE Handle
    )
/*++

Routine Description:

    This is an internal "Low" routine to handle open requests.

Arguments:

    Path - path to handle to open
    Handle - pointer for the handle returned.

Return Value:

    NT status

--*/
{
    OBJECT_ATTRIBUTES oa;
    NTSTATUS          status;
    IO_STATUS_BLOCK   statusBlock;
    UNICODE_STRING    unicodeName;

    status = RtlCreateUnicodeString(&unicodeName, Path);

    LogLibInfoPrintf( L"Opening handle to : %s\n", Path );

    if (!NT_SUCCESS(status))
    {
        LogLibInfoPrintf( L"Failed to create unicode string. status:%lX\n", status );
        return status;
    }

    RtlZeroMemory(&statusBlock, sizeof(IO_STATUS_BLOCK));
    RtlZeroMemory(&oa, sizeof(OBJECT_ATTRIBUTES));
    oa.Length = sizeof(OBJECT_ATTRIBUTES);
    oa.ObjectName = &unicodeName;
    oa.Attributes = OBJ_CASE_INSENSITIVE;

    status = NtOpenFile(Handle,
                    GENERIC_READ | GENERIC_WRITE,
                    &oa,
                    &statusBlock,
                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                    0
                    );


    RtlFreeUnicodeString(&unicodeName);
    return status;
}

NTSTATUS ZeroPartitionFirstSector(
    IN HANDLE DiskHandle,
    IN LONGLONG  Offset
    )

{
    BYTE*            buffer;
    LARGE_INTEGER    diskOffet;
    IO_STATUS_BLOCK  statusBlock;
    NTSTATUS         status = STATUS_INSUFFICIENT_RESOURCES;

    diskOffet.QuadPart = Offset;
    buffer = (BYTE*) malloc( DEFUALT_SECTOR_SZ );

    if (buffer != NULL) {
        ZeroMemory( buffer,  DEFUALT_SECTOR_SZ );
        status =  NtWriteFile( DiskHandle, NULL, NULL, NULL, &statusBlock, buffer, DEFUALT_SECTOR_SZ, &diskOffet, NULL);
        free(buffer);
    }

    return status;
}


LONG
GetDiskIndexber(
    IN HANDLE DiskHandle
    )

/*++

Routine Description:

    Get disk number

Arguments:

    DiskHandle - Handle to Disk

Return Value:
    
    DiskIndexber - Device number of disk

--*/

{
    STORAGE_DEVICE_NUMBER           num;
    IO_STATUS_BLOCK                 statusBlock;
    NTSTATUS                        status;
    ULONG                           bufferSize;

    bufferSize = sizeof(STORAGE_DEVICE_NUMBER);

    status = NtDeviceIoControlFile(DiskHandle,
                                   0,
                                   NULL,
                                   NULL,
                                   &statusBlock,
                                   IOCTL_STORAGE_GET_DEVICE_NUMBER,
                                   NULL,
                                   0,
                                   &num,
                                   bufferSize);
    if (NT_SUCCESS(status)) {
        return num.DeviceNumber;
    } 
    return -1;
}



LONGLONG
GetDiskSize(
    IN HANDLE DiskHandle
    )

/*++

Routine Description:

    Get size of disk

Arguments:

    DiskHandle - Handle to Disk

Return Value:
    
    DiskSize - size of the disk in bytes

--*/

{
    GET_LENGTH_INFORMATION          disksize;
    IO_STATUS_BLOCK                 statusBlock;
    NTSTATUS                        status;
    ULONG                           bufferSize;

    bufferSize = sizeof(GET_LENGTH_INFORMATION);

    status = NtDeviceIoControlFile(DiskHandle,
                                   0,
                                   NULL,
                                   NULL,
                                   &statusBlock,
                                   IOCTL_DISK_GET_LENGTH_INFO,
                                   NULL,
                                   0,
                                   &disksize,
                                   bufferSize);
    if (NT_SUCCESS(status)) {
        return disksize.Length.QuadPart;
    } 
    return -1;
}


NTSTATUS
PrintDiskLayout(
    IN PDRIVE_LAYOUT_INFORMATION_EX DriveLayout
    )

/*++

Routine Description:

    Prints out the Partition Layout in a readable format

Arguments:

    DriveLayout - pointer to pointer for the drive layout

Return Value:

    NT status

--*/

{
    DWORD                          partitionStyle;
    DWORD                          partitionCount;
    UINT                           iPartition;
    PARTITION_INFORMATION_EX       partitionInfo;

    LogLibInfoPrintf( L"Partition Layout\n" );    
    LogLibInfoPrintf( L"================\n" );    

    partitionStyle = DriveLayout->PartitionStyle;

    switch(partitionStyle) {
        case PARTITION_STYLE_MBR:
            LogLibInfoPrintf( L"PartitionStyle: MBR\n"  ); 
            break;
        case PARTITION_STYLE_GPT:
            LogLibInfoPrintf( L"PartitionStyle: GPT\n"  ); 
            break;
        case PARTITION_STYLE_RAW:
            LogLibInfoPrintf( L"PartitionStyle: RAW\n"  ); 
            break;
    default:
            LogLibInfoPrintf( L"None\n"  );        
    }

    partitionCount = DriveLayout->PartitionCount;
    LogLibInfoPrintf( L"PartitionCount: %d\n", partitionCount ); 

    if ( partitionStyle == PARTITION_STYLE_GPT ) {
        LogLibInfoPrintf( STR_GUID_FMT, GUID_PRINTF_ARG(DriveLayout->Gpt.DiskId) );
        LogLibInfoPrintf( L"GPT UsableLength: %I64d\n", DriveLayout->Gpt.UsableLength.QuadPart );
        LogLibInfoPrintf( L"GPT StartingUsableOffset: %I64d\n", DriveLayout->Gpt.StartingUsableOffset.QuadPart );
        LogLibInfoPrintf( L"GPT MaxPartitionCount: %lu\n", DriveLayout->Gpt.MaxPartitionCount );

    } else {
        LogLibInfoPrintf( L"MBR Signature: %lu\n", DriveLayout->Mbr.Signature );        
    }


    for (iPartition = 0; iPartition < partitionCount; iPartition++) {
        partitionInfo = DriveLayout->PartitionEntry[iPartition];
        LogLibInfoPrintf( L"\nPartitionInfo StartingOffset: %I64d\n", partitionInfo.StartingOffset.QuadPart );        
        LogLibInfoPrintf( L"\r\t  PartitionLength: %I64d\n", partitionInfo.PartitionLength.QuadPart );        
        LogLibInfoPrintf( L"\r\t  PartitionNumber: %d\n", partitionInfo.PartitionNumber );
        LogLibInfoPrintf( L"\r\t  RewritePartition: %u\n", partitionInfo.RewritePartition );        
        if ( partitionInfo.PartitionStyle == PARTITION_STYLE_GPT ) { 
            LogLibInfoPrintf( L"PartitionInfo GPT PartitionType: " );
            LogLibInfoPrintf( STR_GUID_FMT, GUID_PRINTF_ARG( partitionInfo.Gpt.PartitionType ) );
            LogLibInfoPrintf( L"\nPartitionInfo GPT PartitionId: " );
            LogLibInfoPrintf( STR_GUID_FMT, GUID_PRINTF_ARG( partitionInfo.Gpt.PartitionId ) );
            LogLibInfoPrintf( L"\nPartitionInfo GPT PartitionAttributes: 0x%I64x\n", partitionInfo.Gpt.Attributes );
            LogLibInfoPrintf( L"PartitionInfo GPT Name: %ws\n", partitionInfo.Gpt.Name );
        } 
    }
    LogLibInfoPrintf( L"\n" );
    return STATUS_SUCCESS;
}

BOOL GetDriveLayout (HANDLE hDevice,
                     PDRIVE_LAYOUT_INFORMATION_EX * DriveLayout ,
                     ULONG* DriveLayoutSize, 
                     ULONG* ReturnedLength)
{
    ULONG i;
    BOOL result = FALSE;
    PVOID AllocTemp;

    //
    // Get the Disk Layout Structure.  This doesn't return needed buffer size, so keep trying.
    // Starting with size of disk and 5 partitions.
    //   Don't care about skipping offline Dynamic disks.
    //
    *ReturnedLength = 0;
    *DriveLayoutSize = sizeof(DRIVE_LAYOUT_INFORMATION_EX) + (5 * sizeof(PARTITION_INFORMATION_EX));;
    *DriveLayout = (PDRIVE_LAYOUT_INFORMATION_EX) malloc(*DriveLayoutSize);

    if (*DriveLayout == NULL){
        LogLibInfoPrintf (L"PartitionInfo: Allocation failed");
        goto EXIT;
    }

    for (i=0; i < 20; i++){

        if (DeviceIoControl (   hDevice,
                                IOCTL_DISK_GET_DRIVE_LAYOUT_EX,
                                NULL,
                                0,
                                *DriveLayout,
                                *DriveLayoutSize,
                                ReturnedLength,
                                NULL) == FALSE ) {

            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {

                *DriveLayoutSize = *DriveLayoutSize * 2;
                AllocTemp = realloc(*DriveLayout, *DriveLayoutSize);
                if (AllocTemp == NULL){
                    LogLibInfoPrintf (L"PartitionInfo: Memory reallocation failed");
                    goto EXIT;
                }

                *DriveLayout = (PDRIVE_LAYOUT_INFORMATION_EX) AllocTemp;
                AllocTemp = NULL;
                ZeroMemory(*DriveLayout, *DriveLayoutSize);
                
            }else{

                LogLibInfoPrintf (L"PartitionInfo Get Drive Layout IOCTL failed: %#x", GetLastError());
                CloseHandle(hDevice);
                hDevice = INVALID_HANDLE_VALUE;
                break;
            }

        }else{

            result = TRUE;
            // IOCTL Success
            break;

        }
    }

EXIT:
    return result ;

}

HANDLE 
CreateGPTRawDumpPartition( )
{
   
    BOOL                        RawDumpPartExist;
    WCHAR                       DevicePath[MAX_PATH];
    SYSTEM_DEVICE_INFORMATION   DeviceInfo;
    PDRIVE_LAYOUT_INFORMATION_EX DriveLayout;
    ULONG                       DriveLayoutSize;
    HANDLE                      hDevice = INVALID_HANDLE_VALUE;
    UINT32                      DiskIndex;
    PPARTITION_INFORMATION_EX   PartitionEntry;
    ULONG                       PartitionId;
    ULONG                       ReturnedLength;
    NTSTATUS                    Status;
    HRESULT                     result = ERROR_SUCCESS;
    ULONG                       PartIndex = 0;
    HANDLE                      hVolume = INVALID_HANDLE_VALUE;
    WCHAR*                      pwszVolume;
    LONGLONG                   disksize = 0;
    LONGLONG                   availFreeSpace = 0;
    LONGLONG                   ReqdFreeSpace = 0;
    LONGLONG                   LargestPartitionsize =0;
    LONG                        LargestPartitionindex =0;
    ULONGLONG                   volumeOffset = 0;
    ULONGLONG                   freeSpaceStart = 0 ;
    DWORD                       retries = 5;
    DWORD                       retryCount = 0;
    SYSTEM_BASIC_INFORMATION    BasicInfo;

    DriveLayout = NULL;
    RawDumpPartExist = FALSE;
    pwszVolume = NULL;


    //
    //  Get system physical memory size, Raw Partition size is memroy size+  500MB
    //
    NtQuerySystemInformation( SystemBasicInformation,
                              &BasicInfo,
                              sizeof(BasicInfo),
                              NULL);
    ReqdFreeSpace = (LONGLONG) (BasicInfo.NumberOfPhysicalPages) *(LONGLONG) (BasicInfo.PageSize);
    ReqdFreeSpace += 500*1024*1024;
    LogLibInfoPrintf (L"Required Dump partiton size should be  %llu MB", ReqdFreeSpace/(1024*1024) );

    //
    // Get the number of Disks in the system
    //
    Status = NtQuerySystemInformation( SystemDeviceInformation, &DeviceInfo, sizeof(SYSTEM_DEVICE_INFORMATION), 0 );
    if ( Status != ERROR_SUCCESS){
         result = E_FAIL;
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
            result = E_FAIL;
            LogLibErrorPrintf(
                result,
                __LINE__,
                WIDEN(__FUNCTION__),
                __WFILE__,
                L" Create File Failed. INVALID_HANDLE_VALUE. %s\n", DevicePath);
            goto EXIT;
        }

      if (GetDriveLayout (hDevice , &DriveLayout , &DriveLayoutSize, &ReturnedLength) == FALSE) {
            result = E_FAIL;
            LogLibErrorPrintf(
                result,
                __LINE__,
                WIDEN(__FUNCTION__),
                __WFILE__,
                L" Failed to get  DriveLayout");
            goto EXIT;
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
            LogLibInfoPrintf (L"Partition  type not GPT, %u", DriveLayout->PartitionStyle);
            CloseHandle(hDevice);
            hDevice = INVALID_HANDLE_VALUE;
            DiskIndex++;
            continue;
        }

        //
        // We need check whether we already have RAW_DUMP Partition there
        //
        
        //
        // Loop through the partitions
        //
        for (PartitionId = 0; PartitionId < DriveLayout->PartitionCount; PartitionId++){
            PartitionEntry = &DriveLayout->PartitionEntry[PartitionId];

            //
            // Check for the RAW Dump partition type
            //
            if (IsEqualGUID(&PartitionEntry->Gpt.PartitionType, &RAWDUMP_GUID)){
                PartIndex = PartitionId +1 ;
                LogLibInfoPrintf( L"RawDump Partiton already exit Partition = %d", PartIndex); 
                RawDumpPartExist = TRUE;
                break;
            }
        }

        //
        // RAW dump Partition not exist , need create one
        //
        if (FALSE == RawDumpPartExist) {
            //
            // Need determine whether this GPT disk have enough free space to create RAW Dump partion
            // Raw partiton is System memory size + 500 MB
            //
             if ( (disksize =  GetDiskSize(hDevice)) == 0 ) { 
                LogLibInfoPrintf( L"Failed to get disk size\n" ); 
                continue;
            }
            LogLibInfoPrintf( L"Disk size = %llu MB", disksize/(1024*1024) ); 

            GetFreeSpaceAtEnd(disksize, DriveLayout, &availFreeSpace );
            LogLibInfoPrintf( L"AvailFreeSpace = %llu MB\n", availFreeSpace/(1024*1024) ); 
            if ( availFreeSpace < ReqdFreeSpace ) {
                 LogLibInfoPrintf( L"This disk don't have enough free space for create RAW Dump partiton Disk Number:= %d\n", DiskIndex );
               
                  // Need find the larget partition resize it
                   for (PartitionId = 0; PartitionId < DriveLayout->PartitionCount; PartitionId++){
                         PartitionEntry = &DriveLayout->PartitionEntry[PartitionId];
                         if (PartitionEntry->PartitionLength.QuadPart > LargestPartitionsize) {
                             LargestPartitionsize = PartitionEntry->PartitionLength.QuadPart;
                             LargestPartitionindex = PartitionId + 1;
                         }
                   }
                   
                   result = ShrinkLargetPartition(DiskIndex, ReqdFreeSpace, &volumeOffset, &freeSpaceStart);
                   if (SUCCEEDED(result)) {
                         if (DriveLayout) {
                             free (DriveLayout);
                         }
                         
                         if (GetDriveLayout (hDevice , &DriveLayout , &DriveLayoutSize, &ReturnedLength) == FALSE) {
                                result = E_FAIL;
                                LogLibErrorPrintf(
                                    result,
                                    __LINE__,
                                    WIDEN(__FUNCTION__),
                                    __WFILE__,
                                    L" Failed to get  DriveLayout");
                                goto EXIT;
                          }

                         GetFreeSpaceAtEnd(disksize, DriveLayout, &availFreeSpace );
                         LogLibInfoPrintf( L"after Shrink the new availFreeSpace = %llu", availFreeSpace ); 
                         if ( availFreeSpace < ReqdFreeSpace )
                         {
                             LogLibInfoPrintf( L"Even after shrink still no available space avaibel =%llu, ReqdFreeSpace = %llu \n", availFreeSpace, ReqdFreeSpace ); 
                             CloseHandle(hDevice);
                             hDevice = INVALID_HANDLE_VALUE;
                             DiskIndex++;
                             continue;
                            
                         }
                         LogLibInfoPrintf( L" Finally we get enough free space for dump partition\n"); 
                   }
                   else {
                         LogLibInfoPrintf( L"Failed Shrink the partition id = %d", LargestPartitionindex ); 
                         CloseHandle(hDevice);
                         hDevice = INVALID_HANDLE_VALUE;
                         DiskIndex++;
                         continue;
                   }
                
            }

            //
            // GPT disk and disk have eough free space
            // 

            LogLibInfoPrintf( L"This disk is the disk have meet the requirement  Disk Number:= %d\n", DiskIndex ); 

            PartIndex = GetNewPartitionNumber( DriveLayout );
            LogLibInfoPrintf( L"New Partition Number %lu\n", PartIndex ); 
        
            Status = InsertPartitionGpt(DriveLayout, PartIndex, disksize, ReqdFreeSpace, hDevice);
            if (!NT_SUCCESS(Status)) {
                LogLibErrorPrintf(
                    result,
                    __LINE__,
                    WIDEN(__FUNCTION__),
                    __WFILE__, 
                    L"Error: Failed at  InsertPartitionGpt : %d\n",
                    GetLastError());
                    goto EXIT;
            }

            PrintDiskLayout( DriveLayout );

            Status = SetDiskLayout( hDevice, DriveLayout );
            if (!NT_SUCCESS(Status) || !DriveLayout ) {
                LogLibErrorPrintf(
                    result,
                    __LINE__,
                    WIDEN(__FUNCTION__),
                    __WFILE__, 
                    L"Failed to set disk layout. status:%lX\n", Status);
                    goto EXIT;
            }
        }

        pwszVolume = (PWCHAR) malloc (DEVICE_NAME_SIZE * sizeof(WCHAR));
        if (!pwszVolume) {
            LogLibInfoPrintf( L"Could not allocate memory for volume name\n" );
            return INVALID_HANDLE_VALUE;
        }

        swprintf(pwszVolume, L"\\Device\\Harddisk%lu\\Partition%lu", DiskIndex, PartIndex);
        LogLibInfoPrintf( L"Volume Handle: %ws\n",  pwszVolume );             

        Status = OpenHandle(pwszVolume, &hVolume);
        //
        // it might take a while for PNP to install the volume
        //

        for ( retryCount = 0; retryCount < retries; retryCount++ ) {
            Status = OpenHandle(pwszVolume, &hVolume);           
            Sleep(1000);
            if (NT_SUCCESS(Status)) {
                break;
            }
            LogLibInfoPrintf( L"Failed to get volume Handle. status:%lX Retrying ...\n", Status );
        }
        if (!NT_SUCCESS(Status)) {
            LogLibInfoPrintf( L"Failed to get volume Handle. status:%lX\n", Status );
            hVolume = INVALID_HANDLE_VALUE;
            goto EXIT;
        }
        LogLibInfoPrintf( L"Handle opened successfully. status:%lX\n", Status );

        //
        //
        //Exit the find the disk
        //
        //
        break;
    }

EXIT:

    if (hDevice != INVALID_HANDLE_VALUE){
        CloseHandle(hDevice);
        hDevice = INVALID_HANDLE_VALUE;
    }

    if (pwszVolume ) {
        free(pwszVolume);
    }
    if (DriveLayout)
        free (DriveLayout);
    return  hVolume;
}

HANDLE
OpenVolume(UINT32 DiskIndex,
           ULONG PartIndex  )
{
    WCHAR*  pwszVolume;
    NTSTATUS Status;
    HANDLE hVolume = INVALID_HANDLE_VALUE;
    DWORD retries = 5;
    DWORD retryCount = 0;

    pwszVolume = (PWCHAR) malloc (DEVICE_NAME_SIZE * sizeof(WCHAR));
    if (!pwszVolume) {
        LogLibInfoPrintf( L"Could not allocate memory for volume name\n" );
        return INVALID_HANDLE_VALUE;
    }

    swprintf(pwszVolume, L"\\Device\\Harddisk%lu\\Partition%lu", DiskIndex, PartIndex);
    LogLibInfoPrintf( L"Volume Handle: %ws\n",  pwszVolume );

    Status = OpenHandle(pwszVolume, &hVolume);
    //
    // it might take a while for PNP to install the volume
    //

    for ( retryCount = 0; retryCount < retries; retryCount++ ) {
        Status = OpenHandle(pwszVolume, &hVolume);           
        Sleep(1000);
        if (NT_SUCCESS(Status)) {
            break;
        }
        LogLibInfoPrintf( L"Failed to get volume Handle. status:%lX Retrying ...\n", Status );
    }

    if (!NT_SUCCESS(Status)) {
        LogLibInfoPrintf( L"Failed to get volume Handle. status:%lX\n", Status );
        hVolume = INVALID_HANDLE_VALUE;
        goto EXIT;
    }
    LogLibInfoPrintf( L"volume Handle opened successfully. status:%lX\n", Status );

EXIT:

    if (pwszVolume ) {
        free(pwszVolume);
    }
    
    return hVolume;
}
