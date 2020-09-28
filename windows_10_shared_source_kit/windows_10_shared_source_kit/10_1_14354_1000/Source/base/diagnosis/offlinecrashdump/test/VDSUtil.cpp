/*++

Copyright (c) April 2013 Microsoft Corporation, All Rights Reserved

Module Name:
   VDSUtil
   This file has a set of Disk access routines using 
   windows Virtual Disk Service APIs.

Environment:
   User Mode

Author:
   Jinsheng Shi (jinsh)  04/10/2013

--*/

#include "DiskUtil.h"
#include "common.h"

HRESULT GetVdsService(
    IVdsService **VdsService)
{
    HRESULT hr = S_OK;
    IVdsServiceLoader *pLoader = NULL;
    IVdsService *pService = NULL;

    if (VdsService == NULL) {
        LogLibInfoPrintf(L"Input parameter is wrong \n");
        goto Exit;
    }

    hr = CoCreateInstance(CLSID_VdsLoader,
            NULL,
            CLSCTX_LOCAL_SERVER,
            IID_IVdsServiceLoader,
            (void **)&pLoader
            );
   
    //
    // Load VDS service.
    //

    hr = pLoader->LoadService( NULL, &pService);
    LogLibInfoPrintf(L"Loading VDS service");

    //
    // Wait for VDS service to finish loading.
    //

    hr = pService->WaitForServiceReady();
    *VdsService = pService;

    LogLibInfoPrintf(L"Waiting for VDS service to finish loading");

Exit:
    return hr;
}

HRESULT VDSGetDisk(
    __in IVdsService * VdsService,
    __in_ecount(cchDiskName) LPWSTR DiskName,
    __in size_t cchDiskName,
    __deref_out_opt IVdsDisk **FirstMatchingDisk
    )
/*++

Routine Description:

    Enumerates all disks on the system, searching for a disk with the
    requested disk name.

Arguments:

    DiskName - Name of the disk to find

Return Values:

    S_OK on success.

--*/
{
    HRESULT hr = E_FAIL;
    ULONG FetchedProviders = 0L;
    ULONG FetchedPacks = 0L;
    ULONG FetchedDisks = 0L;

    VDS_DISK_PROP DiskProp = {0};
    VDS_PACK_PROP PackProp = {0};

    IUnknown *punk = NULL;
    IEnumVdsObject *SoftwareProviders = NULL;
    IVdsSwProvider *SoftwareProvider = NULL;
    IEnumVdsObject *Packs = NULL;
    IVdsPack *Pack = NULL;
    IEnumVdsObject *Disks = NULL;
    IVdsDisk *Disk = NULL;
    size_t DiskNameLen = 0;
    WCHAR *ConformingDiskName = NULL;

    *FirstMatchingDisk = NULL;

    if(VdsService == NULL || DiskName == NULL){
         LogLibErrorPrintf(
                hr,
                __LINE__,
                WIDEN(__FUNCTION__),
                __WFILE__,
                L"VDS service or Diskname is NULL");
        goto Exit;
    }

    hr = StringCchLengthW(
            DiskName,
            MAX_PATH,
            &DiskNameLen
            );
    
    if(cchDiskName != DiskNameLen || DiskNameLen < 2 ){
        LogLibErrorPrintf(
                hr,
                __LINE__,
                WIDEN(__FUNCTION__),
                __WFILE__,
                L" DiskName is invalid");
        goto Exit;
    }

    //
    // Ensure that the provided DiskName conforms to our requirement
    //

    ConformingDiskName = _wcsdup(DiskName);
    ConformingDiskName[2] = '?';

    //
    // Get all software providers.
    //

    hr = VdsService->QueryProviders(
            VDS_QUERY_SOFTWARE_PROVIDERS,
            &SoftwareProviders
            );

        if (SUCCEEDED(hr)){
        LogLibInfoPrintf(L"Querying VDS Software Providers Successully");

        //
        // Enumerate over all the providers.
        //

        for (;;){
            FetchedProviders = 0L;
            hr = SoftwareProviders->Next(
                    1,                  //Number of objects to return.
                    &punk,              //Array of IUnknown initialized by VDS on return.
                    &FetchedProviders   //Number of objects returned.
                    );
            if (hr == S_FALSE){
               // LogLibInfoPrintf(L"No more VDS Software Providers");
                break;
            }

            //
            // Get the software provider interface.
            //

            hr = punk->QueryInterface(
                    IID_IVdsSwProvider,
                    (void **)&SoftwareProvider
                    );
            SAFE_RELEASE(punk);
            if (FAILED(hr)){
                LogLibInfoPrintf(L"Software provider interface is unavailable");
                continue;
            }

            //
            // Get the packs in the software provider.
            //

            hr = SoftwareProvider->QueryPacks(&Packs);
            SAFE_RELEASE(SoftwareProvider);
            if (FAILED(hr)){
                LogLibInfoPrintf(L"No packs in the software provider");
                continue;
            }

            //
            // Enumerate over each pack.
            //

            for (;;){
                FetchedPacks = 0L;
                hr = Packs->Next(
                        1,              //Number of objects to return.
                        &punk,          //Array of IUnknown initialized by VDS on return.
                        &FetchedPacks   //Number of objects returned.
                        );
                if (hr == S_FALSE){
                  //  LogLibInfoPrintf(L"No more VDS Packs");
                    break;
                }

                //
                // Get the pack interface.
                //

                hr = punk->QueryInterface(IID_IVdsPack, (void **)&Pack);
                if (FAILED(hr)){
                    LogLibInfoPrintf(L"Pack interface is unavailable");
                    continue;
                }

                hr = Pack->GetProperties(&PackProp);
                if (FAILED(hr)){
                    LogLibInfoPrintf(L"Pack properties are unavailable");
                    continue;
                }

                //
                // Do not enumerate disks which belong to a foreign pack.
                //

                if (TEST_FLAG(PackProp.ulFlags, VDS_PKF_FOREIGN)){
                    LogLibInfoPrintf(L"Current disk belongs to foreign pack");
                    continue;
                }

                //
                // Get all the disks in the pack.
                //

                hr = Pack->QueryDisks(&Disks);
                if (FAILED(hr)){
                    LogLibInfoPrintf(L"Disks in the pack are unavailable");
                    continue;
                }

                //
                // Enumerate over each disk in the pack.
                //

                for (;;){
                    FetchedDisks = 0L;
                    hr = Disks->Next(
                            1,              //Number of objects to return.
                            &punk,          //Array of IUnknown initialized by VDS on return.
                            &FetchedDisks //Number of objects returned.
                            );
                    if (hr == S_FALSE){
                       // LogLibInfoPrintf(L"No more VDS Disks");
                        break;
                    }

                    hr = punk->QueryInterface(IID_IVdsDisk, (void **)&Disk);
                    SAFE_RELEASE(punk);
                    if (FAILED(hr)){
                        LogLibInfoPrintf(L"Disk interface is unavailable");
                        continue;
                    }

                    //
                    // Check if matching disk.
                    //

                    hr = Disk->GetProperties(&DiskProp);

                    if (_wcsicmp(DiskProp.pwszName, ConformingDiskName) == 0){
                        LogLibInfoPrintf(L"First Matching VDS Disk found");
                        hr = S_OK;
                        *FirstMatchingDisk = Disk;
                        Disk = NULL;
                        goto Exit;
                    }

                    SAFE_RELEASE(Disk);
                }

                SAFE_RELEASE(Disks);
            }

            SAFE_RELEASE(Packs);
        }
    }

    //
    // Finished enumerating software providers, didn't find a matching disk.
    //

    LogLibInfoPrintf(L"Matching VDS Disk not found");

    hr = S_FALSE;

Exit:
    if(ConformingDiskName != NULL) {
        free(ConformingDiskName);
    }

    SAFE_RELEASE(punk);
    SAFE_RELEASE(SoftwareProviders);
    SAFE_RELEASE(SoftwareProvider);
    SAFE_RELEASE(Packs);
    SAFE_RELEASE(Pack);
    SAFE_RELEASE(Disks);
    SAFE_RELEASE(Disk);

    SAFE_FREE(DiskProp.pwszDiskAddress);
    SAFE_FREE(DiskProp.pwszName);
    SAFE_FREE(DiskProp.pwszFriendlyName);
    SAFE_FREE(DiskProp.pwszAdaptorName);
    SAFE_FREE(DiskProp.pwszDevicePath);
    SAFE_FREE(PackProp.pwszName);

    return hr;
}

HRESULT
GetLastPartition(
    IVdsDisk* Disk,
    ULONGLONG *Offset
    )
{
    IVdsAdvancedDisk *AdvancedDisk = NULL;
    HRESULT hr = E_FAIL;
    VDS_PARTITION_PROP *PartitionProp=NULL;
    LONG numberOfPartitions = 0;

    //
    // In case there is no partitions, default offset to 100 MB from start
    // of the disk.
    //
    *Offset = 0;
    if (Disk == NULL || Offset == NULL) {
        LogLibInfoPrintf(L"Input parameter invalid");
        goto Exit;
    }

    hr = Disk->QueryInterface(
        IID_IVdsAdvancedDisk,
        (void **)(&AdvancedDisk));
    LogLibInfoPrintf(L"Querying for Advanced disk");

    hr = AdvancedDisk->QueryPartitions(
        &PartitionProp,
        &numberOfPartitions);
    if (hr == S_FALSE)
    {
        //
        // This means the disk contains no partitions. numberOfPartitions is
        // already initialized to zero.
        //
        hr = S_OK;
    }
    LogLibInfoPrintf(L"Querying for Partitions");

    if(numberOfPartitions==0)
    {
        LogLibInfoPrintf(L"Number of partitions is zero");
        goto Exit;
    }

    *Offset = PartitionProp[numberOfPartitions -1].ullOffset;

Exit:
    SAFE_RELEASE(AdvancedDisk);
    if(PartitionProp) {
        CoTaskMemFree(PartitionProp);
        PartitionProp = NULL;
    }

    return hr;
}

HRESULT
GetDiskExtent(
    IVdsService * VDSService,
    __in IVdsVolume *Volume,
    __deref_out_opt VDS_DISK_EXTENT** DiskExtent
    )
/*++

Routine Description:

    Retrieves the IVdsDisk interface pointer that corresponds to the given
    Volume. Volume must be a basic volume. Disks have zero or more volumes.

Arguments:

    Volume - Pointer to volume interface

    Disk - On success, receives the disk associated with the volume.

Return Values:

    S_OK on success.
    E_UNEXPECTED if the given volume is not a basic volume.

--*/
{
    HRESULT hr = S_OK;
    ULONG FetchedPlex = 0;
    LONG NumberOfExtents = 0;
    IVdsVolumePlex *VolumePlex = NULL;

    IEnumVdsObject *EnumVolumePlex = NULL;
    IUnknown *punk = NULL;

    *DiskExtent = NULL;
    if( VDSService  == NULL || Volume == NULL){
        LogLibInfoPrintf(L" Input parameters are invalid");
        goto Cleanup;
    }

    hr = Volume->QueryPlexes(&EnumVolumePlex);

    if (SUCCEEDED(hr)){
    // Get the first volume plex (should only be one for simple basic volumes).
        hr = EnumVolumePlex->Next(
                1,              //Number of objects to return.
                &punk,          //Array of IUnknown initialized by VDS on return.
                &FetchedPlex    //Number of objects returned.
                );

         if (SUCCEEDED(hr)) {
                hr = punk->QueryInterface(IID_IVdsVolumePlex, (void **)&VolumePlex);
                SAFE_RELEASE(punk);
                if (SUCCEEDED(hr)){
                        hr = VolumePlex->QueryExtents(DiskExtent, &NumberOfExtents);}
                 }
    }
    //
    // A basic volume should have exactly 1 extent.
    //

    if (NumberOfExtents != 1 || NULL == DiskExtent) {
        hr = E_UNEXPECTED;
        LogLibErrorPrintf(
                hr,
                __LINE__,
                WIDEN(__FUNCTION__),
                __WFILE__,
                L"Verifying that the volume has exactly one extent");
    }

Cleanup:
    SAFE_RELEASE(punk);
    SAFE_RELEASE(EnumVolumePlex);
    SAFE_RELEASE(VolumePlex);

    return hr;
}

HRESULT
FindVolume(
    IVdsService * VDSService,
    IVdsDisk *Disk,
    ULONGLONG Offset,
    IVdsVolume** FirstMatchingVolume
    )
/*++

Routine Description:

    Enumerates all Volumes on the system, searching for a Volume with the
    begins at the specified Offset.

Arguments:

    VolumeName - Name of the Volume to find

Return Values:

    S_OK on success.

--*/
{
    HRESULT         hr = S_OK;
    ULONG           FetchedProviders = 0L;
    ULONG           FetchedPacks = 0L;
    ULONG           FetchedVolumes = 0L;
    VDS_VOLUME_PROP VolumeProp= {0};
    VDS_PACK_PROP   PackProp = {0};
    IUnknown        *punk = NULL;
    IEnumVdsObject  *SoftwareProviders = NULL;
    IVdsSwProvider  *SoftwareProvider = NULL;
    IEnumVdsObject  *Packs = NULL;
    IVdsPack        *Pack = NULL;
    IEnumVdsObject  *Volumes = NULL;
    IVdsVolume      *Volume = NULL;
    WCHAR           *ConformingVolumeName = NULL;
    VDS_DISK_EXTENT *DiskExtent = NULL;
    VDS_DISK_PROP   DiskProperties = {0};
    VDS_OBJECT_ID   DiskId;

    if( VDSService == NULL || FirstMatchingVolume == NULL) {
        LogLibInfoPrintf(L"Parameters are invalid"); 
        goto Cleanup;
    }

    //Get properties of the disk
    hr = Disk->GetProperties(&DiskProperties);
    LogLibInfoPrintf(L"Getting properties of the disk");

    //Store the Disk ID for comparison
    DiskId = DiskProperties.id;


    //
    // Get all software providers.
    //

    hr = VDSService->QueryProviders(
            VDS_QUERY_SOFTWARE_PROVIDERS,
            &SoftwareProviders
            );
    LogLibInfoPrintf(L"Querying VDS Software Providers");

    //
    // Enumerate over all the providers.
    //

    for (;;){
        FetchedProviders = 0L;
        hr = SoftwareProviders->Next(
                1,                  //Number of objects to return.
                &punk,              //Array of IUnknown initialized by VDS on return.
                &FetchedProviders   //Number of objects returned.
                );
        if (hr == S_FALSE){
           // LogLibInfoPrintf(L"No more VDS Software Providers");
            break;
        }
        LogLibInfoPrintf(L"Getting next VDS Software Providers");

        //
        // Get the software provider interface.
        //

        hr = punk->QueryInterface(
                IID_IVdsSwProvider,
                (void **)&SoftwareProvider
                );
        SAFE_RELEASE(punk);
        if (FAILED(hr)){
            LogLibInfoPrintf(L"Software provider interface is unavailable");
            continue;
        }

        //
        // Get the packs in the software provider.
        //

        hr = SoftwareProvider->QueryPacks(&Packs);
        SAFE_RELEASE(SoftwareProvider);
        if (FAILED(hr)){
            LogLibInfoPrintf(L"No packs in the software provider");
            continue;
        }

        //
        // Enumerate over each pack.
        //

        for (;;){
            FetchedPacks = 0L;
            hr = Packs->Next(
                    1,              //Number of objects to return.
                    &punk,          //Array of IUnknown initialized by VDS on return.
                    &FetchedPacks   //Number of objects returned.
                    );
            if (hr == S_FALSE){
              //  LogLibInfoPrintf(L"No more VDS Packs");
                break;
            }

            //
            // Get the pack interface.
            //

            hr = punk->QueryInterface(IID_IVdsPack, (void **)&Pack);
            SAFE_RELEASE(punk);
            if (FAILED(hr)){
                LogLibInfoPrintf(L"Pack interface is unavailable");
                continue;
            }

            hr = Pack->GetProperties(&PackProp);
            if (FAILED(hr)){
                LogLibInfoPrintf(L"Pack properties are unavailable");
                continue;
            }

            //
            // Do not enumerate Volumes which belong to a foreign pack.
            //

            if (TEST_FLAG(PackProp.ulFlags, VDS_PKF_FOREIGN)){
                LogLibInfoPrintf(L"Current Volume belongs to foreign pack");
                continue;
            }

            //
            // Get all the Volumes in the pack.
            //

            hr = Pack->QueryVolumes(&Volumes);
            if (FAILED(hr)){
                LogLibInfoPrintf(L"Volumes in the pack are unavailable");
                continue;
            }

            //
            // Enumerate over each Volume in the pack.
            //

            for (;;){
                FetchedVolumes = 0L;
                hr = Volumes->Next(
                        1,              //Number of objects to return.
                        &punk,          //Array of IUnknown initialized by VDS on return.
                        &FetchedVolumes //Number of objects returned.
                        );
                if (hr == S_FALSE){
                    //LogLibInfoPrintf(L"No more VDS Volumes");
                    break;
                }

                hr = punk->QueryInterface(IID_IVdsVolume, (void **)&Volume);
                SAFE_RELEASE(punk);
                if (FAILED(hr)){
                    LogLibInfoPrintf(L"Volume interface is unavailable");
                    continue;
                }

                //
                // Check if matching Volume.
                //
                hr = GetDiskExtent( VDSService, Volume,&DiskExtent);

                if(DiskExtent->diskId == DiskId && DiskExtent->ullOffset == Offset)
                {
                    LogLibInfoPrintf(L"First Matching VDS Volume found");
                    *FirstMatchingVolume = Volume;
                    Volume = NULL;
                    hr = S_OK;
                    SAFE_FREE(DiskExtent);
                    goto Cleanup;
                }
                SAFE_FREE(DiskExtent);
                SAFE_RELEASE(Volume);
            }

            //
            // Finished enumerating Volumes in this pack.
            //

            SAFE_RELEASE(Volumes);
        }

        //
        // Finished enumerating packs for this software provider.
        //

        SAFE_RELEASE(Packs);
    }

    //
    // Finished enumerating software providers, didn't find a matching Volume.
    //

    LogLibInfoPrintf(L"Matching VDS Volume not found");

    hr = S_FALSE;

Cleanup:
    if(ConformingVolumeName != NULL) {
    free(ConformingVolumeName);
    }

    SAFE_RELEASE(punk);
    SAFE_RELEASE(SoftwareProviders);
    SAFE_RELEASE(SoftwareProvider);
    SAFE_RELEASE(Packs);
    SAFE_RELEASE(Pack);
    SAFE_RELEASE(Volumes);
    SAFE_RELEASE(Volume);
    SAFE_FREE(DiskProperties.pwszDiskAddress);
    SAFE_FREE(DiskProperties.pwszName);
    SAFE_FREE(DiskProperties.pwszFriendlyName);
    SAFE_FREE(DiskProperties.pwszAdaptorName);
    SAFE_FREE(DiskProperties.pwszDevicePath);
    SAFE_FREE(VolumeProp.pwszName);
    SAFE_FREE(PackProp.pwszName);

    return hr;
}


HRESULT DoShrinkVolume(
    IVdsVolume *Volume, 
    ULONGLONG numberOfBytesToShrink)
{
    HRESULT hr = E_FAIL;
    IVdsAsync *asyncP = NULL;
    HRESULT asyncHr;
    VDS_ASYNC_OUTPUT ShrinkOut;

    if(Volume == NULL) {
        LogLibInfoPrintf(L"volume is null");
        goto Cleanup;
    }

    hr = Volume->Shrink(numberOfBytesToShrink,&asyncP);
    LogLibInfoPrintf(L"Shrinking the volume");
    hr = asyncP->Wait(&asyncHr,&ShrinkOut);
    LogLibInfoPrintf(L"Waiting for the shrink operation to complete");
    hr= asyncHr;
    LogLibInfoPrintf(L"Checking Shrink operation status");

Cleanup:
    SAFE_RELEASE(asyncP);
    return hr;
}

HRESULT
ShrinkVolume(
    IVdsService * VDSService,
    IVdsDisk *Disk,
    __in  ULONGLONG Offset,
    __out ULONGLONG *StartOffset,
    __in  ULONGLONG DesiredShrinkSize
    )
{
    HRESULT hr = E_FAIL;
    IVdsVolume *MatchingVolume = NULL;
    VDS_DISK_EXTENT *DiskExtent = NULL;

    if (VDSService == NULL || Disk == NULL) {
        LogLibInfoPrintf(L"Input Parameters is wrong");
        goto Cleanup;
    }

    *StartOffset = 0;
    hr = FindVolume(VDSService, Disk,Offset,&MatchingVolume);
    LogLibInfoPrintf(L"Finding volume");

    hr = DoShrinkVolume(MatchingVolume, DesiredShrinkSize);
    LogLibInfoPrintf( L"Shrinking volume");

    hr = GetDiskExtent(VDSService,
                        MatchingVolume,
                        &DiskExtent);
    LogLibInfoPrintf(L"Getting disk for volume");


Cleanup:
    SAFE_RELEASE(MatchingVolume);
    SAFE_FREE(DiskExtent);
    return hr;
}


//
// find the largest free space partition Shrik the size
//
HRESULT ShrinkLargetPartition(
      __in  ULONG DiskIndex,
      __in LONGLONG ReqdFreeSpace,
    __out ULONGLONG *volumeOffset,
    __out ULONGLONG *freeSpaceStart)
{
    HRESULT hr = E_FAIL;
    WCHAR szPhyDisk[MAX_PATH];
    IVdsDisk *Disk = NULL;
    IVdsService *VdsService;

    hr = GetVdsService(&VdsService);
    if (FAILED(hr)) {
        LogLibInfoPrintf(L" Failed to get VDS serivce");
        goto Exit;
    }

    LogLibInfoPrintf(L"DiskIndex=%d,pVolOffset=%llu,pStart=%llu",   DiskIndex, volumeOffset, freeSpaceStart);

    *volumeOffset = 0;
    *freeSpaceStart = 0;
    hr = StringCchPrintfW( szPhyDisk,
                          ARRAYSIZE(szPhyDisk),
                          L"\\\\.\\PhysicalDrive%d",
                          DiskIndex);
    LogLibInfoPrintf(L"Create physical disk name. %s", szPhyDisk );

    hr = VDSGetDisk(VdsService,szPhyDisk, wcslen(szPhyDisk), &Disk);
    LogLibInfoPrintf(L"Getting disk");

    hr = GetLastPartition(Disk, volumeOffset);
    LogLibInfoPrintf(L"Getting largest partition hr = %x", hr);

    if (*volumeOffset == 0)
    {
        //
        // If volumeOffset is zero, it means the disk has no partition.
        // So we don't need to shrink the partitions. We will just
        // return a free space of 100MB starting at an offset of 100MB
        // from the beginning of the disk.
        //
        *freeSpaceStart = 0x6400000;
    }
    else
    {
        hr = ShrinkVolume(VdsService, Disk, *volumeOffset, freeSpaceStart, ReqdFreeSpace);
        LogLibInfoPrintf(L"Calling shrink");
    }

Exit:
    SAFE_RELEASE(Disk);

    LogLibInfoPrintf(L" (volOffset=%I64d,start=%I64d,size=%I64d)", *volumeOffset, *freeSpaceStart);
    return hr;
}