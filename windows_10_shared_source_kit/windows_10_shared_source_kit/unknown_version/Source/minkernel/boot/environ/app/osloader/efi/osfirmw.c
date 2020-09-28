/*++

Copyright (c) Microsoft Corporation

Module Name:

    oslfirmw.c

Abstract:

    This module contains EFI firmware code that is specific to an OS loader.

Environment:

    Boot.

--*/

// ------------------------------------------------------------------- Includes

#include "osloader.h"

// ------------------------------------------------------------------- Defines

#define PAGEFILE_SYS L"\\pagefile.sys"
#define TEMP_PAGEFILE_SYS L"\\tmpgfile.sys"

#define IS_ADDRESS_IN_REGION(func, phy, size)                               \
    ((((EFI_PHYSICAL_ADDRESS)((ULONG_PTR) (func))) >= (phy)) &&             \
    (((EFI_PHYSICAL_ADDRESS)((ULONG_PTR) (func))) < ((phy) + (size))))

#define SET_VIRTUAL_ADDRESS(func, phy, virt)                \
    (ULONG_PTR)((virt) + (((EFI_PHYSICAL_ADDRESS)((ULONG_PTR) (func))) - (phy)))

#define IS_RUNTIME_TYPE_VALID(Type)                                           \
    ((Type == EfiReservedMemoryType) ||      \
     (Type == EfiRuntimeServicesCode) ||     \
     (Type == EfiRuntimeServicesData) ||     \
     (Type == EfiACPIMemoryNVS) ||           \
     (Type == EfiMemoryMappedIO) ||          \
     (Type == EfiMemoryMappedIOPortSpace) || \
     (Type == EfiPalCode) ||                 \
     (Type == EfiACPIReclaimMemory))

//
// Defines to specify the upper and lower bounds of which firmware boot services
// 1:1 mappings need to be zero'd. The boot code will specifically zero PTEs in
// the HAL VA range and MM will zero all the user memory space.
//

#define ZERO_FIRMWARE_SERVICE_END HAL_VA_START

#if defined(AMD64)

#define ZERO_FIRMWARE_SERVICE_START (ULONG_PTR)MI_SYSTEM_RANGE_START

#else

#define ZERO_FIRMWARE_SERVICE_START KSEG0_BASE

#endif

//
// OslFwpFindFreeFirmwareVaRange definitions.
//

#define SubtractFromPtr(_pointer, _bytes) \
    ((PVOID)((PUCHAR)(_pointer) - (_bytes)))

//
// EFI runtime services are mapped downward starting from HAL VA space.
//

#define FIRMWARE_RESERVE_MEMORY_START ((ULONG_PTR)HAL_VA_START)
#define FIRMWARE_RESERVE_MEMORY_END   ((ULONG_PTR)(PTE_TOP + 1))


//
// A formatting string used to construct the EFI version string
// and constants regarding the size of the EFI version string.
//

#define EFI_VERSION_SEPARATOR_AND_REVISION_FORMAT L" - %X"
#define EFI_VERSION_SEPARATOR_CHARACTER_COUNT 3
#define MAX_EFI_REVISION_CHARACTER_COUNT 8
#define EFI_VERSION_SEPARATOR_AND_REVISION_BUFFER_LENGTH \
    (((EFI_VERSION_SEPARATOR_CHARACTER_COUNT + \
       MAX_EFI_REVISION_CHARACTER_COUNT) * sizeof(WCHAR)) + \
     sizeof(UNICODE_NULL))

//
// The maximum length of the FirmwareVendor portion of the EFI version
// string in bytes. If the FirmwareVendor string in the EFI system table is
// longer than this length, it will be truncated.
//

#define MAX_EFI_VENDOR_LENGTH \
    (MAXUSHORT - EFI_VERSION_SEPARATOR_AND_REVISION_BUFFER_LENGTH)

// -------------------------------------------------------------------- Globals

//
// Flag used to prevent debugging from being shutdown before ExitBootServices.
// Can be modified in the debugger when winload first connects to the debugger
// to enable debugging additional code on platforms that support it.  Some
// platforms incorrectly modify the debug device PCI configuration when
// ExitBootServices is called, and thereby break debugging.
//

volatile LOGICAL BdDebugAfterExitBootServices;

//
// Define the values used to track the dimensions of the preallocated buffer
// used when querying the EFI memory map in preparation for ExitBootServices.
//

PVOID OslFwpMemoryMapBufferVa;
ULONG_PTR OslFwpMemoryMapSize;

//
// Define buffer for EFI descriptors that holds the virtual translation for EFI
// runtime services. Also track the current and total size of this buffer.
//

EFI_MEMORY_DESCRIPTOR *OslFwpRuntimeVirtualDescVa;
ULONG_PTR OslFwpRuntimeVirtualDescSizeTotal;
ULONG_PTR OslFwpRuntimeVirtualDescSizeSet;

//
// Define buffer a temporary buffer to be used during the virtual translation
// of runtime services. Also track the current and total size of this buffer.
//

EFI_MEMORY_DESCRIPTOR *OslFwpAltRuntimeVirtualDescVa;
ULONG_PTR OslFwpAltRuntimeVirtualDescSizeTotal;
ULONG_PTR OslFwpAltRuntimeVirtualDescSizeSet;

//
// Define a buffer for the preallocated range descriptors to be used during
// the marking of zeroed memory.
//
// Extern the global variable for the memory descriptors that are preallocated
// while building the kernel's memory map.
//

BOOLEAN OslFwCanMarkZeroedMemory;
POSL_HC_BOOT_ZEROED_PFN_RANGE OslFwpZeroedMemoryRanges;

// ----------------------------------------------------------------- Prototypes

NTSTATUS
OslFwpAllocateRuntimeVirtualDescBuffers (
    VOID
    );

VOID
OslFwpCacheAttributeToMmType (
    __in ULONG CacheAttribute,
    __out MEMORY_CACHING_TYPE* CachingType
    );

NTSTATUS
OslFwpFindEfiRuntimeMemoryDescriptorInList (
    __in EFI_MEMORY_DESCRIPTOR *FindDescriptor,
    __in_bcount (MemoryMapSize) EFI_MEMORY_DESCRIPTOR *MemoryMap,
    __in UINTN MemoryMapSize,
    __in UINTN DescriptorSize
    );

NTSTATUS
OslFwpFindFreeFirmwareVaRange (
    __in ULONGLONG Size,
    __out PVOID *FreeRangeStartAddress
    );

ULONG
OslFwpGetOsAttributeType (
    __in UINT64 EfiAttributes
    );

NTSTATUS
OslFwpKernelSetupPhase0 (
    __inout PLOADER_PARAMETER_BLOCK LoaderBlock
    );

NTSTATUS
OslFwpKernelSetupPhase1 (
    __inout PLOADER_PARAMETER_BLOCK LoaderBlock
    );

VOID
OslFwpMarkZeroedMemory (
    __inout PLOADER_PARAMETER_BLOCK LoaderBlock
    );

VOID
OslFwpMarkFirmwareMemoryRegion (
    __in PLIST_ENTRY OsDescriptorListHead,
    __inout PLIST_ENTRY *CurrentOsDescriptorListEntry,
    __in ULONGLONG StartPage,
    __in ULONGLONG NumberOfPages
    );

VOID
OslFwpUpdateVirtualRuntimeServicesTable (
    __in EFI_RUNTIME_SERVICES *EfiRTRefTable,
    __in EFI_MEMORY_DESCRIPTOR *MemoryMap,
    __in UINTN MemoryMapSize,
    __in UINTN DescriptorSize,
    __out VIRTUAL_EFI_RUNTIME_SERVICES *EfiRTVirtualTable
    );

NTSTATUS
OslFwpVirtualizeRuntimeServices (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    );

NTSTATUS
OslFwpZeroFirmwareServicesBootMappings (
    __in EFI_MEMORY_DESCRIPTOR *MemoryMap,
    __in UINTN MemoryMapSize,
    __in UINTN DescriptorSize
    );

NTSTATUS
OslFwpUpdateResumeContext (
    __in UINTN MemoryMapSize,
    __in UINTN DescriptorSize,
    __in UINT32 DescriptorVersion,
    __in_bcount(MemoryMapSize) EFI_MEMORY_DESCRIPTOR *VirtualMemoryMap
    );

USHORT
OslFwpEfiVersionBufferLength (
    __in USHORT VendorLength
    );

// ------------------------------------------------------------------ Functions

NTSTATUS
OslFwBuildMemoryCachingRequirementsList (
    __inout PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine builds a map of cache attributes for platform memory.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block.

Return Value:

    NTSTATUS code.

--*/

{

    ULONG BufferSize;
    PMEMORY_CACHING_REQUIREMENTS CachingRequirements;
    PMEMORY_CACHING_REQUIREMENTS CachingRequirementsBuffer;
    ULONG CachingRequirementsCount;
    ULONGLONG CurrentRegionEnd;
    PMEMORY_DESCRIPTOR Descriptor;
    ULONG DescriptorCount;
    MEMORY_CACHING_TYPE MemoryCachingType;
    LIST_ENTRY MemoryMap;
    GENERIC_BUFFER MemoryMapDescriptors;
    PLIST_ENTRY NextEntry;
    NTSTATUS Status;

    BlInitGenericBuffer(&MemoryMapDescriptors, NULL, 0);
    CachingRequirementsBuffer = NULL;

    //
    // Obtain a copy of the firmware memory map.
    //

    Status = BlMmGetMemoryMap(&MemoryMap,
                              &MemoryMapDescriptors,
                              MEMORYMAP_TYPE_FIRMWARE,
                              MDL_FLAGS_ALLOCATE_DESCRIPTORS);

    if (!NT_SUCCESS(Status)) {
        goto BuildMemoryCachingRequirementsListEnd;
    }

    //
    // Allocate storage for the cache attribute map.
    //

    DescriptorCount = MemoryMapDescriptors.DataSize / sizeof(MEMORY_DESCRIPTOR);
    if (DescriptorCount == 0) {
        Status = STATUS_UNSUCCESSFUL;
        goto BuildMemoryCachingRequirementsListEnd;
    }

    BufferSize = DescriptorCount * sizeof(MEMORY_CACHING_REQUIREMENTS);
    CachingRequirementsBuffer = BlMmAllocateHeap(BufferSize);
    if (CachingRequirementsBuffer == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto BuildMemoryCachingRequirementsListEnd;
    }

    //
    // Walk through the firmware map, and create cache attribute descriptors
    // for each memory descriptor.  Coalesce any adjacent entries with the
    // same cache attributes.
    //

    CachingRequirements = CachingRequirementsBuffer;
    NextEntry = MemoryMap.Flink;

    NT_ASSERT(NextEntry != &MemoryMap);

    Descriptor = CONTAINING_RECORD(NextEntry,
                                   MEMORY_DESCRIPTOR,
                                   ListEntry);

    OslFwpCacheAttributeToMmType(Descriptor->Attributes,
                                 &MemoryCachingType);

    CachingRequirementsCount = 1;
    CachingRequirements->BasePage = Descriptor->BasePage;
    CachingRequirements->PageCount = Descriptor->PageCount;
    CachingRequirements->MemoryCachingType = MemoryCachingType;
    NextEntry = NextEntry->Flink;
    while (NextEntry != &MemoryMap) {
        Descriptor = CONTAINING_RECORD(NextEntry,
                                       MEMORY_DESCRIPTOR,
                                       ListEntry);

        OslFwpCacheAttributeToMmType(Descriptor->Attributes,
                                     &MemoryCachingType);

        CurrentRegionEnd =
            CachingRequirements->BasePage + CachingRequirements->PageCount;

        if ((CachingRequirements->MemoryCachingType == MemoryCachingType) &&
            (CurrentRegionEnd == Descriptor->BasePage)) {

            CachingRequirements->PageCount += Descriptor->PageCount;

        } else {
            CachingRequirements += 1;

            ASSERT((ULONG_PTR)CachingRequirements + sizeof(*CachingRequirements) <
                   (ULONG_PTR)CachingRequirementsBuffer + BufferSize);

            CachingRequirementsCount += 1;
            CachingRequirements->BasePage = Descriptor->BasePage;
            CachingRequirements->PageCount = Descriptor->PageCount;
            CachingRequirements->MemoryCachingType = MemoryCachingType;
        }

        NextEntry = NextEntry->Flink;
    }

    LoaderBlock->Extension->MemoryCachingRequirementsCount =
        CachingRequirementsCount;

    LoaderBlock->Extension->MemoryCachingRequirements =
        CachingRequirementsBuffer;

BuildMemoryCachingRequirementsListEnd:
    if ((NT_SUCCESS(Status) == FALSE) &&
        (CachingRequirementsBuffer != NULL)) {

        BlMmFreeHeap(CachingRequirementsBuffer);
    }

    if (MemoryMapDescriptors.Buffer != NULL) {
        BlMmFreeHeap(MemoryMapDescriptors.Buffer);
    }

    return Status;
}

NTSTATUS
OslFwBuildRuntimeMemoryMap (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine builds all firmware memory mappings that are required
    at OS runtime.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block.

Return Value:

    NTSTATUS code.

--*/

{

    UNREFERENCED_PARAMETER(LoaderBlock);

    return OslFwpVirtualizeRuntimeServices(LoaderBlock);
}

NTSTATUS
OslFwKernelSetup (
    __in ULONG Phase,
    __inout PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine will perform any steps firmware steps required to transition
    into the kernel.  This process is broken into two phases.  In the first
    phase, MM (library) services can be utilized.  In the second phase, MM
    services are illegal.  Upon successful return of the second phase, no
    firmware services can be called by the OS Loader or the Boot Environment
    Library.

Arguments:

    Phase - The phase of kernel setup / firmware completion.

    LoaderBlock - A pointer to the argument structure that will eventually
        be passed into the kernel.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_BUFFER_TOO_SMALL if the global scratch buffer is too small for
        the firmware memory map.


--*/

{

    NTSTATUS Status;

    if (Phase == 0) {
        Status = OslFwpKernelSetupPhase0(LoaderBlock);

    } else {

        ASSERT(Phase == 1);

        //
        // Stop the boot debugger before calling ExitBootServices unless
        // explicitly disabled.
        //

        if (BdDebugAfterExitBootServices == FALSE) {
            BlBdStop();
        }

        Status = OslFwpKernelSetupPhase1(LoaderBlock);
    }

    return Status;
}

NTSTATUS
OslFwLoadDriver (
    __in DEVICE_ID DeviceId,
    __in PWSTR ImagePath,
    __in BOOLEAN StartImage,
    __out PHANDLE DriverHandle
    )

/*++

Routine Description:

    This routine will load the EFI driver and, if requested, start the driver.

Arguments:

    DeviceId - Supplies a handle to the device containing the image to load.

    ImagePath - Supplies the file path of the image to load.

    StartImage - Supplies a flag indicating whether the driver should be
        started.

    DriverHandle - Supplies a pointer used upon success to return a handle to
        the loaded driver.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_CONFLICTING_ADDRESSES if memory map conflicts cannot be resolved.

    STATUS_NO_MEMORY if there is insufficient memory to perform a request.

    STATUS_NO_SUCH_FILE if a file required to load does not exist.

    STATUS_UNSUCCESSFUL if file system can not be mounted on OsDeviceId.

--*/

{

    EFI_DEVICE_PATH EmptyDevicePath;
    FILE_ID FileId;
    FILE_INFORMATION FileInfo;
    PUCHAR ImageBase;
    EFI_HANDLE ImageHandle;
    ULONG ImageSize;
    NTSTATUS Status;
    NTSTATUS SyncStatus;
    BOOLEAN SynchronizeMemoryMap;

    ImageBase = NULL;
    ImageSize = 0;
    SynchronizeMemoryMap = FALSE;

    //
    // Load the file via the boot libraries routines for consistency with other
    // module loading and to ensure firmware drivers can be loaded without
    // the firmware understanding the filesystem containing the drivers.
    //

    FileId = INVALID_FILE_ID;
    Status = BlFileOpen(DeviceId, ImagePath, OPEN_READ, &FileId);
    if (!NT_SUCCESS(Status)) {
        goto LoadDriverEnd;
    }

    //
    // Find the file size
    //

    Status = BlFileGetInformation(FileId, &FileInfo);
    if (!NT_SUCCESS(Status)) {
        goto LoadDriverEnd;
    }

    //
    // Load the file to physical memory.
    //

    ImageSize = (ULONG)FileInfo.FileSize;
    Status = BlImgAllocateImageBuffer(&ImageBase,
                                      ImageSize,
                                      OSL_MEMORY_TYPE_RECLAIMABLE_ALLOCATION,
                                      MEMORY_ATTRIBUTE_NONE,
                                      0,
                                      IMAGE_FLAGS_MAP_ONE_TO_ONE);

    if (!NT_SUCCESS(Status)) {
        goto LoadDriverEnd;
    }

    Status = BlFileRead(FileId, ImageBase, ImageSize, NULL);
    if (!NT_SUCCESS(Status)) {
        goto LoadDriverEnd;
    }

    SetDevicePathEndNode(&EmptyDevicePath);
    ImageHandle = NULL;
    SynchronizeMemoryMap = TRUE;
    Status = EfiLoadImage(EfiImageHandle,
                          &EmptyDevicePath,
                          ImageBase,
                          ImageSize,
                          &ImageHandle);

    if (!NT_SUCCESS(Status)) {
        goto LoadDriverEnd;
    }

    //
    // From experimentation, it appears that if EfiStartImage returns
    // STATUS_DEVICE_ALREADY_ATTACHED, this EFI driver will have been
    // automatically unloaded.
    //

    if (StartImage != FALSE) {
        Status = EfiStartImage(ImageHandle, NULL, NULL);
        if (!NT_SUCCESS(Status) &&
            (Status != STATUS_DEVICE_ALREADY_ATTACHED)) {

            EfiUnloadImage(ImageHandle);
            goto LoadDriverEnd;
        }
    }

    if (NT_SUCCESS(Status)) {
        *DriverHandle = (HANDLE)ImageHandle;
    }

LoadDriverEnd:
    if (FileId != INVALID_FILE_ID) {
        BlFileClose(FileId);
    }

    if (ImageBase != NULL) {
        BlImgUnallocateImageBuffer(ImageBase,
                                   ImageSize,
                                   IMAGE_FLAGS_MAP_ONE_TO_ONE);

    }

    //
    // Loading FW Drivers may change the firmware memory map, if needed,
    // synchronize internal maps with any changes.
    //

    if (SynchronizeMemoryMap != FALSE) {
        SyncStatus = BlMmSynchronizeMemoryMap(MEMORY_SYNC_FLAGS_BOOT_DEFAULT);
        if (!NT_SUCCESS(SyncStatus)) {
            Status = SyncStatus;
            OslFatalError(OSL_ERROR_MEMORY_MAP_CONFLICT,
                          Status,
                          0,
                          0);

        }
    }

    return Status;
}

NTSTATUS
OslFwUnloadDriver(
    __in HANDLE DriverHandle
    )

/*++

Routine Description:

    This routine is responsible for unloading EFI drivers.

Arguments:

    DriverHandle - Supplies a handle for the firmware driver to be unloaded.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_CONFLICTING_ADDRESSES if memory map conflicts cannot be resolved.

    STATUS_INVALID_PARAMETER if any of the parameters has an invalid value.

    STATUS_NOT_SUPPORTED if the image has been started, and does not support
        unload.

--*/

{

    NTSTATUS ReturnStatus;
    NTSTATUS Status;

    ReturnStatus = EfiUnloadImage(DriverHandle);
    Status = BlMmSynchronizeMemoryMap(MEMORY_SYNC_FLAGS_BOOT_DEFAULT);
    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
        OslFatalError(OSL_ERROR_MEMORY_MAP_CONFLICT,
                      Status,
                      0,
                      0);

    }

    return ReturnStatus;
}

NTSTATUS
OslFwLoadFirmwareDrivers(
    __inout PUNICODE_STRING OsFilePath,
    __in DEVICE_ID OsDeviceId,
    __out PLIST_ENTRY LoadedFirmwareDriverList
    )

/*++

Routine Description:

    This routine is responsible for loading all required firmware drivers.

Arguments:

    LoadedFirmwareDriverList - Supplies a list used to track all loaded
        drivers.

Return Value:

    STATUS_SUCCESS.

--*/

{

    return OslArchLoadFirmwareDrivers(OsFilePath,
                                      OsDeviceId,
                                      LoadedFirmwareDriverList);
}

NTSTATUS
OslFwMapPhysicalAddressInFirmwareVaSpace (
    __in PHYSICAL_ADDRESS PhysicalStartAddress,
    __in ULONGLONG Size,
    __in ULONG Attributes,
    __out PVOID *VirtualStartAddress
    )

/*++

Routine Description:

    This routine will find a free virtual address range and map it to the
    specified physical address range.

Arguments:

    PhysicalStartAddress - Supplies the starting address of the physical address
        range being mapped.

    Size - Supplies the size in bytes of the address range to map.

    Attributes - Supplies the Page attributes to set for the mapped VA range.

    VirtualStartAddress - Supplies a pointer to a variable that receives
        the starting address of the VA range where the physical address
        was mappped.

Return Value:

    NTSTATUS code.

--*/

{

    NTSTATUS Status;

    if (VirtualStartAddress == NULL) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // This routine is only expected to be called with translation enabled.
    //

    ASSERT(BlMmQueryTranslationType() != VA_TRANSLATION_TYPE_NONE);

    //
    // Find a free PTE range that spans the required size.
    //

    Size = ALIGN_RANGE_UP(Size, PAGE_SIZE);

    Status = OslFwpFindFreeFirmwareVaRange(Size, VirtualStartAddress);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // The return value from OslpArchEfiFindFreeFirmwareVaRange on success
    // should always be non-null and page-aligned.
    //

    ASSERT(*VirtualStartAddress != NULL);

    ASSERT(((ULONG_PTR)*VirtualStartAddress & (PAGE_SIZE - 1)) == 0);

    //
    // Request MM to map the physical address at the exact virtual address we
    // specify.
    //

    Attributes |= MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS;
    Status = BlMmMapPhysicalAddress(VirtualStartAddress,
                                    PhysicalStartAddress,
                                    Size,
                                    Attributes);

    //
    // The VA range was determined to be free, so the mapping should succeed.
    //

    ASSERT(NT_SUCCESS(Status));

    return Status;
}

// ------------------------------------------------------------ Local Functions

NTSTATUS
OslFwpAllocateRuntimeVirtualDescBuffers (
    VOID
    )

/*++

Routine Description:

    This routine allocates pages required to hold the EFI descriptors that
    virtually map EFI runtime services.

Arguments:

    None.

Return Value:

    NT status code.

--*/

{

    UINTN DescriptorSize;
    UINT32 DescriptorVersion;
    UINTN MapKey;
    PVOID MemoryMapBuffer;
    UINTN MemoryMapSize;
    ULONGLONG PagesNeeded;
    NTSTATUS Status;

    //
    // Invoke GetMemoryMap to determine the size of the current memory map.
    // This will provide an estimate for the number of descriptors required.
    //

    MemoryMapSize = 0;
    Status = EfiGetMemoryMap(&MemoryMapSize,
                             NULL,
                             &MapKey,
                             &DescriptorSize,
                             &DescriptorVersion);

    if (Status != STATUS_BUFFER_TOO_SMALL) {
        return Status;
    }

    //
    // The Runtime Services (code/data etc.) are a subset of the current map,
    // hence the current size should be sufficient to hold virtual translation
    // for all runtime services.
    //

    PagesNeeded = ALIGN_RANGE_UP(MemoryMapSize, PAGE_SIZE) >> PAGE_SHIFT;
    MemoryMapBuffer = NULL;
    Status = BlMmAllocatePages(&MemoryMapBuffer,
                               PagesNeeded,
                               OSL_MEMORY_TYPE_KERNEL_INIT_DATA,
                               MEMORY_ATTRIBUTE_ALLOCATION_KSEG,
                               0);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Initialize the VA and size for the runtime buffer.
    //

    OslFwpRuntimeVirtualDescVa = (EFI_MEMORY_DESCRIPTOR *)MemoryMapBuffer;
    OslFwpRuntimeVirtualDescSizeTotal = (UINTN)(PagesNeeded * PAGE_SIZE);

    //
    // Allocate space for OslFwpAltRuntimeVirtualDescBuffer.
    // This buffer needs to be large enough to hold all the descriptors,
    // so allocate twice the current size.
    //

    PagesNeeded = ALIGN_RANGE_UP(2 * MemoryMapSize, PAGE_SIZE) >> PAGE_SHIFT;
    MemoryMapBuffer = NULL;
    Status = BlMmAllocatePages(&MemoryMapBuffer,
                               PagesNeeded,
                               OSL_MEMORY_TYPE_KERNEL_INIT_DATA,
                               MEMORY_ATTRIBUTE_ALLOCATION_KSEG,
                               0);

    if (!NT_SUCCESS(Status)) {
        BlMmFreePages(OslFwpRuntimeVirtualDescVa);
        OslFwpRuntimeVirtualDescVa = NULL;
        return Status;
    }

    //
    // Initialize the VA and size for the alternate buffer.
    //

    OslFwpAltRuntimeVirtualDescVa = (EFI_MEMORY_DESCRIPTOR *)MemoryMapBuffer;
    OslFwpAltRuntimeVirtualDescSizeTotal = (UINTN)(PagesNeeded * PAGE_SIZE);
    return STATUS_SUCCESS;
}

VOID
OslFwpCacheAttributeToMmType (
    __in ULONG CacheAttribute,
    __out MEMORY_CACHING_TYPE* CachingType
    )

/*++

Routine Description:

    This routine translates a MEMORY_ATTRIBUTE_CACHE_* value into a
    MEMORY_CACHING_TYPE.

Arguments:

    CacheAttribute - Supplies memory attributes from the firmware.

    CachingType - Returns the corresponding MEMORY_CACHING_TYPE for the
        supplied attributes.

Return Value:

    None.

--*/

{

    if (((CacheAttribute & MEMORY_ATTRIBUTE_CACHE_UNCACHED) != 0) ||
        ((CacheAttribute & MEMORY_ATTRIBUTE_CACHE_MASK) == 0)) {

        *CachingType = MmNonCached;

    } else {
        *CachingType = MmCached;
    }

    return;
}

INT
__cdecl
OslFwpEfiDescriptorCompare (
    const VOID *Left,
    const VOID *Right
    )

/*++

Routine Description:

    Compares two EFI memory descriptors by physical address.

Arguments:

    Left - The left entry for comparison.

    Right - The right entry for comparison.

Return Value:

    < 0, 0, or > 0 as standard for comparison routines.

--*/

{

    const EFI_MEMORY_DESCRIPTOR *LeftDescriptor;
    const EFI_MEMORY_DESCRIPTOR *RightDescriptor;

    LeftDescriptor = Left;
    RightDescriptor = Right;
    if (LeftDescriptor->PhysicalStart < RightDescriptor->PhysicalStart) {
        return -1;

    } else if (LeftDescriptor->PhysicalStart == RightDescriptor->PhysicalStart) {
        return 0;

    } else {
        return 1;
    }
}

NTSTATUS
OslFwpFindEfiRuntimeMemoryDescriptorInList (
    __in EFI_MEMORY_DESCRIPTOR *FindDescriptor,
    __in_bcount (MemoryMapSize) EFI_MEMORY_DESCRIPTOR *MemoryMap,
    __in UINTN MemoryMapSize,
    __in UINTN DescriptorSize
    )

/*++

Routine Description:

    This routine searches for a given descriptor within the specified memory
    descriptor list.

Arguments:

    FindDescriptor - Supplies a pointer to the EFI memory descriptor that needs
        to be searched.

    MemoryMap - Supplies a pointer to the EFI memory descriptor list that will
        be used to search FindDescriptor.

    MemoryMapSize - Supplies the size (in bytes) for the MemoryMap list.

    DescriptorSize - Supplies the size (in bytes) for each descriptor within
        the MemoryMap list.

Return Value:

    STATUS_SUCCESS if the descriptor is found.
    STATUS_NOT_FOUND otherwise.

--*/

{

    EFI_MEMORY_DESCRIPTOR *EfiDescriptor;

    for (EfiDescriptor = MemoryMap;
         MemoryMapSize != 0;
         MemoryMapSize -= DescriptorSize) {

        if (((EfiDescriptor->Attribute & EFI_MEMORY_RUNTIME) != 0) &&
            (EfiDescriptor->PhysicalStart == FindDescriptor->PhysicalStart) &&
            (EfiDescriptor->NumberOfPages == FindDescriptor->NumberOfPages)) {

           return STATUS_SUCCESS;
        }

        EfiDescriptor = Add2Ptr(EfiDescriptor, DescriptorSize);
    }

    return STATUS_NOT_FOUND;
}

ULONG
OslFwpGetOsAttributeType (
    __in UINT64 EfiAttributes
    )

/*++

Routine Description:

    Maps an EFI Attributes bitmask to a OS defined bitmask for a memory
    region's attributes.

Arguments:

    EfiAttributes - Efi Attribute bitmask. Check MmFwpGetOsAttributeType for
        detailed description of the attribtues

Return Value:

    OS defined Attribute bitmask.

--*/

{

    ULONG OsAttributes = 0;

    //
    // If memory region supports multiple attributes, then the priority of
    // mapping should be: WRITE_BACK, WRITE_COMBINED, WRITE_THROUGH, and
    // UNCACHED.
    //
    // N.B. WRITE_COMBINED and UNCACHED_EXPORTED are not yet supported by MM
    //      library.
    //

    if (CHECK_FLAG(EfiAttributes, EFI_MEMORY_WB)) {
        SET_FLAGS(OsAttributes, MEMORY_ATTRIBUTE_CACHE_WRITE_BACK);

    } else if (CHECK_FLAG(EfiAttributes, EFI_MEMORY_WC)) {

        ASSERT(FALSE);

        //
        // N.B. Set the attribute to WRITE_COMBINED once it is supported.
        //      Map the memory as UNCACHED till then.
        //
        // SET_FLAGS(OsAttributes, MEMORY_ATTRIBUTE_CACHE_WRITE_COMBINED);
        //

        SET_FLAGS(OsAttributes, MEMORY_ATTRIBUTE_CACHE_UNCACHED);

    } else if (CHECK_FLAG(EfiAttributes, EFI_MEMORY_WT)) {
        SET_FLAGS(OsAttributes, MEMORY_ATTRIBUTE_CACHE_WRITE_THROUGH);

    } else if (CHECK_FLAG(EfiAttributes, EFI_MEMORY_UC)) {
        SET_FLAGS(OsAttributes, MEMORY_ATTRIBUTE_CACHE_UNCACHED);

    } else if (CHECK_FLAG(EfiAttributes, EFI_MEMORY_UCE)) {

        ASSERT(FALSE);

        //
        // N.B. Enable this once UNCACHED_EXPORTED attribute is supported.
        // Map the memory as default (CACHE_INDIFFERENT) till then.
        //
        // SET_FLAGS(OsAttributes, MEMORY_ATTRIBUTE_CACHE_UNCACHED_EXPORTED);
        //
    }

    if (CHECK_FLAG(EfiAttributes, EFI_MEMORY_RO)) {
        SET_FLAGS(OsAttributes, MEMORY_ATTRIBUTE_PROTECT_WRITE);
    }

    if (CHECK_FLAG(EfiAttributes, EFI_MEMORY_RP)) {
        SET_FLAGS(OsAttributes, MEMORY_ATTRIBUTE_PROTECT_READ);
    }

    if (CHECK_FLAG(EfiAttributes, EFI_MEMORY_XP)) {
        SET_FLAGS(OsAttributes, MEMORY_ATTRIBUTE_PROTECT_EXECUTE);
    }

    if (CHECK_FLAG(EfiAttributes, EFI_MEMORY_RUNTIME)) {
        SET_FLAGS(OsAttributes, MEMORY_ATTRIBUTE_DESCRIPTION_FIRMWARE_RUNTIME);
    }

    return OsAttributes;
}

NTSTATUS
OslFwpKernelSetupPhase0 (
    __inout PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine performs the firmware steps required for kernel transition
    which require MM services.

    In particular, the following is performed:

    - Preallocate memory for the GetMemoryMap calls that are necessary in
      order to call ExitBootServices.

Arguments:

    LoaderBlock - A pointer to the argument structure that will eventually
        be passed into the kernel.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if allocation fails.


--*/

{

    UINTN DescriptorSize;
    UINT32 DescriptorVersion;
    UINTN MapKey;
    PVOID MemoryMapBuffer;
    UINTN MemoryMapSize;
    ULONGLONG PagesNeeded;
    NTSTATUS Status;
    BOOLEAN CanGetBootZeroedMemory;

    //
    // Record that the system in running on EFI firmware.
    //

    LoaderBlock->FirmwareInformation.FirmwareTypeEfi = 1;

#if defined (_ARM64_)

    //
    // If the firmware runs in EL2, report this to the OS since it
    // will need to start APs using the EL2 stub.
    //

    LoaderBlock->FirmwareInformation.FirmwareStartedInEL2 =
            (EfiFirmwareParameters->FirmwareStartedInEL2 != FALSE) ? 1 : 0;

#endif

    //
    // Preallocate the buffer that will be used when calling GetMemoryMap
    // immediately before calling ExitBootServices.
    //
    // NT requires the OS loader is required to pass the physical address of a
    // buffer containing the EFI memory map associated with the map key used
    // when calling ExitBootServices.  The HAL will process and make an
    // internal copy of this buffer during phase 1 initialization.  This
    // implies that the global descriptor buffer cannot be used to hold the
    // memory map; it is part of the loader image and will thus be reclaimed
    // during phase 0 initialization.  Instead, the memory map must be stored
    // within an explicit persistent allocation.
    //
    // Additional descriptors may appear in the memory map between this point
    // and the ExitBootServices call.  This routine queries the amount of
    // memory needed to hold the current memory map and allocates twice this
    // amount in an attempt to ensure that the buffer will be able to contain
    // the memory map that is current at ExitBootServices time.
    //
    // N.B. BlMmAllocatePages is guaranteed to return a physically contiguous
    //      allocation.
    //

    MemoryMapSize = 0;
    Status = EfiGetMemoryMap(&MemoryMapSize,
                             NULL,
                             &MapKey,
                             &DescriptorSize,
                             &DescriptorVersion);

    if (Status != STATUS_BUFFER_TOO_SMALL) {
        return Status;
    }

    PagesNeeded = ALIGN_RANGE_UP(MemoryMapSize * 2, PAGE_SIZE) >> PAGE_SHIFT;
    Status = BlMmAllocatePages(&MemoryMapBuffer,
                               PagesNeeded,
                               OSL_MEMORY_TYPE_KERNEL_INIT_DATA,
                               MEMORY_ATTRIBUTE_ALLOCATION_KSEG,
                               0);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Perform the initial setup for marking zeroed memory and allocate the
    // array required for marking zeroed memory.
    //
    // Allocate the maximum ammount of ranges that might be needed for marking
    // zeroed memory.
    //

    OslFwCanMarkZeroedMemory = TRUE;
    Status = OslHcSetup();
    if (!NT_SUCCESS(Status)) {
        OslFwCanMarkZeroedMemory = FALSE;
    }

    //
    // If the setup was successful and we are able to obtain the necessary ranges
    // pre-allocate the array for future usage.
    //

    if (OslFwCanMarkZeroedMemory != FALSE) {
        CanGetBootZeroedMemory = OslHcCanGetBootZeroedMemory();
        if (CanGetBootZeroedMemory != FALSE) {
            OslFwpZeroedMemoryRanges = BlMmAllocateHeap(
                sizeof(OSL_HC_BOOT_ZEROED_PFN_RANGE) * OSL_HC_BOOT_ZEROED_MEMORY_MAX_RANGES);
        } else {
            OslFwCanMarkZeroedMemory = FALSE;
        }
    }

    //
    // The phase 1 code will check global variables to determine the physical
    // address and size of the buffer it should use when calling GetMemoryMap.
    // Update these values to reflect the dimensions of our allocated buffer.
    //

    OslFwpMemoryMapBufferVa = MemoryMapBuffer;
    OslFwpMemoryMapSize = (UINTN)(PagesNeeded * PAGE_SIZE);
    return STATUS_SUCCESS;
}

NTSTATUS
OslFwpKernelSetupPhase1 (
    __inout PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine performs the firmware steps requied for kernel transition
    which do not require MM services.  Upon successful completion of this
    routine, firmware services are no longer available.

    In particular, the following in performed:

    - Calls EFI.ExitBootServices, terminating all firmware boot services.

    N.B. When successful, this routine may move the processor into an
         incompatible addressing mode before returning.  After this routine
         returns successfully, the only memory accesses guaranteed to work are
         those directed at the boot application image or stack.

Arguments:

    LoaderBlock - A pointer to the argument structure that will eventually
        be passed into the kernel.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_BUFFER_TOO_SMALL if the global scratch buffer is too small for
        the firmware memory map.

--*/

{

    ULONG Attempt;
    PLIST_ENTRY CurrentOsDescriptorListEntry;
    UINTN DescriptorSize;
    UINT32 DescriptorVersion;
    EFI_MEMORY_DESCRIPTOR *EfiDescriptor;
    ULONGLONG EfiDescriptorEndAddress;
    PEFI_FIRMWARE_INFORMATION EfiInformation;
    ULONG Flags;
    BOOLEAN FoundAcpiRoot;
    ULONG MainMemoryUCCount;
    ULONG MainMemoryWCCount;
    UINTN MapKey;
    EFI_MEMORY_DESCRIPTOR *MemoryMap;
    ULONG MemoryMappedIoCount;
    ULONG MemoryMappedIoWCCount;
    UINTN MemoryMapSize;
    ULONGLONG RegionLength;
    UINTN ReturnedMemoryMapSize;
    PHYSICAL_ADDRESS RsdtPa;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(EfiInformation);

    Flags = 0;

    //
    // Locate the RSDP so code further down in this routine will be able to
    // determine the cachability of the ACPI tables.
    //
    // N.B. This call will not use any MM services (which are not available at
    //      this point) since the physical RSDP address is stored directly
    //      within the EFI system table.
    //

    Status = BlFwGetSystemTable(FW_TABLE_RSDT, &RsdtPa);
    if (!NT_SUCCESS(Status)) {
        FoundAcpiRoot = FALSE;

    } else {
        FoundAcpiRoot = TRUE;
    }

    EfiInformation = &LoaderBlock->FirmwareInformation.u.EfiInformation;
    EfiInformation->MissedMappingsCount = 0;

    //
    // As described below, ExitBootServices will only succeed if given the
    // "key" associated with the current memory map.  Although this routine
    // takes no explicit actions that would result in memory map modification,
    // the firmware may need to handle interruptions of various types between
    // the time when we retrieve the key and the time when we pass it back to
    // ExitBootServices.  When handling such an interruption, the firmware may
    // need to take some action that will alter the memory map and invalidate
    // the old memory map key.
    //
    // To account for such cases, retry the GetMemoryMap -> ExitBootServices
    // sequence a few times if ExitBootServices does not return a success
    // status in response to the first call.
    //

    Attempt = 0;
    do {
        Attempt += 1;
        MainMemoryUCCount = 0;
        MainMemoryWCCount = 0;
        MemoryMappedIoCount = 0;
        MemoryMappedIoWCCount = 0;
        CurrentOsDescriptorListEntry =
            LoaderBlock->MemoryDescriptorListHead.Flink;

        //
        // Get the current firmware memory map.  The purpose of this memory
        // map is two-fold.  The Operating System would like to have a copy of
        // the firmware memory map (equivalent to the E820 tables which are
        // passed in the configuration tree in the PC/AT case).  Additionally,
        // in order to successfully call ExitBootServices, a current memory
        // map key is required.
        //
        // N.B. The memory map is read into a buffer preallocated by phase 0
        //      firmware kernel setup.  The buffer should be large enough for
        //      this call to succeed unless the memory map grows to twice its
        //      size between phase 0 and phase 1 kernel initialization. The
        //      memory map should be a virtual address as the EFI library
        //      routine will handle all address translation where appropriate.
        //

        MemoryMap = (EFI_MEMORY_DESCRIPTOR *)OslFwpMemoryMapBufferVa;
        MemoryMapSize = OslFwpMemoryMapSize;
        Status = EfiGetMemoryMap(&MemoryMapSize,
                                 MemoryMap,
                                 &MapKey,
                                 &DescriptorSize,
                                 &DescriptorVersion);

        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        ASSERT((MemoryMapSize % DescriptorSize) == 0);

        //
        // Walk the firmware memory descriptor list in attempt to determine if
        // main memory supports uncachable or write-combined cache semantics.
        // Additionally, determine if ACPI tables are cached by firmware.
        //
        // N.B. We need to be careful to use the virtual address of the
        //      preallocated memory map buffer for the direct accesses done in
        //      this loop.
        //

        ReturnedMemoryMapSize = MemoryMapSize;
        for (EfiDescriptor = MemoryMap;
             MemoryMapSize != 0;
             MemoryMapSize -= DescriptorSize,
                EfiDescriptor = Add2Ptr(EfiDescriptor, DescriptorSize)) {

            //
            // Maintain a count of:
            //
            //  - Main memory regions which support uncached memory access.
            //
            //  - Main memory regions which support write-combined memory
            //    access.
            //
            //  - MMIO regions.
            //
            //  - MMIO regions which support write-combined memory access.
            //

            if (EfiDescriptor->Type == EfiConventionalMemory) {
                if (CHECK_FLAG(EfiDescriptor->Attribute, EFI_MEMORY_WC)) {
                    MainMemoryWCCount += 1;
                }

                if (CHECK_FLAG(EfiDescriptor->Attribute, EFI_MEMORY_UC)) {
                    MainMemoryUCCount += 1;
                }

            } else if (EfiDescriptor->Type == EfiMemoryMappedIO) {
                MemoryMappedIoCount += 1;
                if (CHECK_FLAG(EfiDescriptor->Attribute, EFI_MEMORY_WC)) {
                    MemoryMappedIoWCCount += 1;
                }

            } else {

                //
                // The HAL prefers to map the ACPI tables non-cached.  This is
                // assumed to be the case, unless the ACPI tables are found in
                // a memory region that does not support uncached memory.
                //

                EfiDescriptorEndAddress = EfiDescriptor->PhysicalStart +
                    (EfiDescriptor->NumberOfPages << EFI_PAGE_SHIFT);

                if ((FoundAcpiRoot != FALSE) &&
                    (EfiDescriptor->PhysicalStart
                         <= (ULONGLONG)RsdtPa.QuadPart) &&
                    ((ULONGLONG)RsdtPa.QuadPart < EfiDescriptorEndAddress) &&
                    (CHECK_FLAG(EfiDescriptor->Attribute, EFI_MEMORY_WB)) &&
                    (!CHECK_FLAG(EfiDescriptor->Attribute, EFI_MEMORY_UC))) {

                    SET_FLAGS(Flags, HAL_PLATFORM_ACPI_TABLES_CACHED);
                }
            }

            //
            // For each firmware descriptor that is marked with a firmware
            // memory type that is off limits to the OS, make sure that each OS
            // descriptor describing this range additionally marks the region
            // off limits to the OS.
            //
            // N.B. Both the firmware and OS memory descriptor lists are
            //      ordered by physical address. OslFwpMarkFirmwareMemoryRegion
            //      will update the current OS descriptor so that the list is
            //      only walked once.
            //

            if ((EfiDescriptor->Type != EfiLoaderData) &&
                (EfiDescriptor->Type != EfiLoaderCode) &&
                (EfiDescriptor->Type != EfiBootServicesCode) &&
                (EfiDescriptor->Type != EfiBootServicesData) &&
                (EfiDescriptor->Type != EfiConventionalMemory)) {

                RegionLength = EfiDescriptor->NumberOfPages << EFI_PAGE_SHIFT;
                OslFwpMarkFirmwareMemoryRegion(
                    &LoaderBlock->MemoryDescriptorListHead,
                    &CurrentOsDescriptorListEntry,
                    EfiDescriptor->PhysicalStart >> PAGE_SHIFT,
                    RegionLength >> PAGE_SHIFT);
            }

            //
            // Additionally for each runtime descriptor, make sure we have
            // seen the descriptor and mapped it. It is possible that the
            // firmware created some new runtime mappings between our last
            // snapshot in OlsFwVirtualizeRuntimeServicesPhase0 and now.
            //

            if ((EfiDescriptor->Attribute & EFI_MEMORY_RUNTIME) != 0) {
                Status = OslFwpFindEfiRuntimeMemoryDescriptorInList(
                                EfiDescriptor,
                                EfiInformation->EfiMemoryMap,
                                EfiInformation->EfiMemoryMapSize,
                                EfiInformation->EfiMemoryMapDescriptorSize);

                if (!NT_SUCCESS(Status)) {
                    EfiInformation->MissedMappingsCount += 1;
                }
            }
        }

        //
        // If no descriptor reported main memory that supported uncached or
        // write-combined memory access, report this to the Operating System.
        //

        if (MainMemoryWCCount == 0) {
            SET_FLAGS(Flags, HAL_PLATFORM_DISABLE_WRITE_COMBINING);
        }

        if (MainMemoryUCCount == 0) {
            SET_FLAGS(Flags, HAL_PLATFORM_DISABLE_UC_MAIN_MEMORY);
        }

        //
        // Enable write-combined memory access if there is MMIO which supports
        // write-combined semantics OR if MMIO does not exist.
        //

        if ((MemoryMappedIoCount == 0) || (MemoryMappedIoWCCount != 0)) {
            SET_FLAGS(Flags, HAL_PLATFORM_ENABLE_WRITE_COMBINING_MMIO);
        }

        //
        // Terminate EFI Boot Services, breaking out of the retry loop if this
        // request succeeds.
        //
        // N.B. When successful, this routine may move the processor into an
        //      incompatible addressing mode before returning.  This is
        //      necessary since some addressing modes require firmware support
        //      that does not exist after calling ExitBootServices.
        //

        Status = EfiExitBootServices(MapKey);

    } while (!NT_SUCCESS(Status) && (Attempt < 4));

    ASSERT(Status == STATUS_SUCCESS);

    //
    // If ExitBootServices call succeeded, then map all the runtime code
    // segments from physical to corresponding virtual addresses and
    // invoke SetVirtualAddressMap to virtualize runtime services.
    //

    if (NT_SUCCESS(Status)) {
        OslFwpUpdateVirtualRuntimeServicesTable(
            EfiRT,
            EfiInformation->EfiMemoryMap,
            EfiInformation->EfiMemoryMapSize,
            EfiInformation->EfiMemoryMapDescriptorSize,
            EfiInformation->VirtualEfiRuntimeServices);

        EfiInformation->SetVirtualAddressMapStatus =
            EfiSetVirtualAddressMap(EfiInformation->EfiMemoryMapSize,
                                    EfiInformation->EfiMemoryMapDescriptorSize,
                                    DescriptorVersion,
                                    EfiInformation->EfiMemoryMap);

        //
        // Update the resume context with runtime services virtual address
        // information.
        //

        if (NT_SUCCESS(EfiInformation->SetVirtualAddressMapStatus)) {
            EfiInformation->SetVirtualAddressMapStatus =
                OslFwpUpdateResumeContext(EfiInformation->EfiMemoryMapSize,
                                          EfiInformation->EfiMemoryMapDescriptorSize,
                                          DescriptorVersion,
                                          EfiInformation->EfiMemoryMap);
        }

        //
        // Zero the 1:1 virtual mappings consumed by the runtime services
        // because NTOS will not clean these up. Additionally zero the boot
        // services 1:1 mappings because in certain scenarios NTOS will not
        // clean these up.
        //

        Status = OslFwpZeroFirmwareServicesBootMappings(OslFwpMemoryMapBufferVa,
                                                        ReturnedMemoryMapSize,
                                                        DescriptorSize);

        if (!NT_SUCCESS(Status)) {
            return Status;
        }


        //
        // Attempt to mark memory segments as zeroed on the memory map as
        // requested by the hypervisor.
        //

        if (OslFwCanMarkZeroedMemory != FALSE) {
            OslHcOnExitBootServices();
            OslFwpMarkZeroedMemory(LoaderBlock);
            OslHcTeardown();
        }
    }

    return Status;
}

VOID
OslFwpMarkZeroedMemory (
    __inout PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine marks the ranges described by the hypervisor as
    zeroed on the memory map that the OS loader hands to the kernel.

Arguments:

    LoaderBlock - A pointer to the argument structure that will eventually
        be passed into the kernel.

Return Value:

    None.

--*/

{

    NTSTATUS Status;
    UINT64 RangeCount;

    RangeCount = OSL_HC_BOOT_ZEROED_MEMORY_MAX_RANGES;

    //
    // This should've been preallocated by the call to OslFwpKernelSetupPhase0.
    //

    if (OslFwpZeroedMemoryRanges == NULL) {
        goto OslFwpMarkZeroedMemoryEnd;
    }

    //
    // Request the array containing the ranges that are to be marked as zeroed.
    //

    Status = OslHcGetBootZeroedMemory(OslFwpZeroedMemoryRanges, &RangeCount);

    if (!NT_SUCCESS(Status)) {
        goto OslFwpMarkZeroedMemoryEnd;
    }

    if (IsListEmpty(&LoaderBlock->MemoryDescriptorListHead)) {
        goto OslFwpMarkZeroedMemoryEnd;
    }

    OslFwpMarkZeroedMemoryWorker(&LoaderBlock->MemoryDescriptorListHead,
                                 OslFwpZeroedMemoryRanges,
                                 RangeCount,
                                 &OslFreeMemoryDesctiptorsList);

OslFwpMarkZeroedMemoryEnd:
    return;
}

VOID
OslFwpMarkFirmwareMemoryRegion (
    __in PLIST_ENTRY OsDescriptorListHead,
    __inout PLIST_ENTRY *CurrentOsDescriptorListEntry,
    __in ULONGLONG StartPage,
    __in ULONGLONG NumberOfPages
    )

/*++

Routine Description:

    This routine ensures that the specified firmware memory region is marked
    off limits in the OS memory descriptor list.  If the OS's memory descriptor
    list contained inappropriately marked memory descriptors, they are updated
    to mark the entire region as off-limits.

    N.B. This routine assumes that EFI uses a top-down allocation policy and
         the boot library uses a bottom-up allocation policy.  Therefere, an
         inappropriately marked pages do not contain OS allocations and this
         routine can modify the OS descriptor to not include firmware pages.

    N.B. This routine assumes it is called when walking the firmware memory
         map.  It will update the current OS descriptor to allow the OS memory
         descriptor list to be walked exactly once when walking the firmware
         memory map.

    N.B. This routine exclusively uses OS pages and OS page sizes.

Arguments:

    OsDescriptorListHead - Supplies a pointer to the OS memory descriptor
        list's head.

    CurrentOsDescriptorListEntry - Supplies a pointer to a variable that
        contains the current OS memory descriptor.  On return, the variable is
        updated to contain a pointer to the OS descriptor that contains the
        last firmware memory page (start page plus number of pages).

    StartPage - Supplies the start of the firmware memory range.

    NumberOfPages - Supplies the length of the firmware memory range.

Return Value:

    None.

--*/

{

    PLIST_ENTRY CurrentEntry;
    ULONGLONG CurrentPage;
    ULONGLONG EndPage;
    ULONG_PTR NewPageCount;
    PLIST_ENTRY NextEntry;
    PMEMORY_ALLOCATION_DESCRIPTOR OsDescriptor;
    ULONG_PTR PagesTrimmed;

    //
    // For each physical page in the firmware memory region check to see if
    // the OS memory descriptor list describes the page as off limits.  If it
    // does not, update the OS memory descriptor to exclude the page.
    //

    EndPage = StartPage + NumberOfPages;
    NextEntry = *CurrentOsDescriptorListEntry;
    OsDescriptor = CONTAINING_RECORD(NextEntry,
                                     MEMORY_ALLOCATION_DESCRIPTOR,
                                     ListEntry);

    CurrentPage = StartPage;
    while (CurrentPage < EndPage) {

        //
        // Walk the OS memory descriptor list in search of the descriptor
        // containing the current firmware page.  Since the OS descriptor list
        // is walked with the firmware list, this generally only requires one
        // iteration.
        //

        while ((NextEntry != OsDescriptorListHead) &&
               ((OsDescriptor->BasePage + OsDescriptor->PageCount) <=
                CurrentPage)) {

            NextEntry = NextEntry->Flink;
            OsDescriptor = CONTAINING_RECORD(NextEntry,
                                             MEMORY_ALLOCATION_DESCRIPTOR,
                                             ListEntry);
        }

        //
        // Terminate search if the end of the OS memory descriptor list is
        // hit.  If the firmware page is not in the OS memory descriptor list,
        // proceed to the next page.
        //

        if (NextEntry == OsDescriptorListHead) {
            break;
        }

        //
        // If the current EFI descriptor page falls short of the current OS
        // descriptor, skip to the start of the OS descriptor.
        //

        if (CurrentPage < OsDescriptor->BasePage) {
            CurrentPage = OsDescriptor->BasePage;
            continue;
        }

        //
        // Validate that the OS memory descriptor describes the firmware page
        // as off-limits.  If it doesn't, update the OS memory descriptor to
        // exclude the firmware page and skip over the pages that were trimmed
        // from the tail of the OS descriptor.
        //
        // Note that enclave memory is enumerated as a firmware memory type,
        // so enclave memory must be allowed to pass through.
        //

        ASSERT((OsDescriptor->MemoryType == LoaderBad) ||
               (OsDescriptor->MemoryType == LoaderEnclaveMemory) ||
               (OsDescriptor->MemoryType == LoaderFirmwareCode) ||
               (OsDescriptor->MemoryType == LoaderFirmwareData) ||
               (OsDescriptor->MemoryType == LoaderFirmwareReserved) ||
               (OsDescriptor->MemoryType == LoaderFirmwarePermanent));

        if ((OsDescriptor->MemoryType != LoaderBad) &&
            (OsDescriptor->MemoryType != LoaderEnclaveMemory) &&
            (OsDescriptor->MemoryType != LoaderFirmwareCode) &&
            (OsDescriptor->MemoryType != LoaderFirmwareData) &&
            (OsDescriptor->MemoryType != LoaderFirmwareReserved) &&
            (OsDescriptor->MemoryType != LoaderFirmwarePermanent)) {

            CurrentEntry = NextEntry;
            NewPageCount = (ULONG_PTR)(CurrentPage - OsDescriptor->BasePage);
            PagesTrimmed = OsDescriptor->PageCount - NewPageCount;
            OsDescriptor->PageCount = NewPageCount;
            CurrentPage += PagesTrimmed;

            //
            // If the new page count is zero, remove the descriptor from the
            // list.  Before doing so, update the current OS memory descriptor.
            //

            NextEntry = NextEntry->Flink;
            OsDescriptor = CONTAINING_RECORD(NextEntry,
                                             MEMORY_ALLOCATION_DESCRIPTOR,
                                             ListEntry);

            if (NewPageCount == 0) {
                RemoveEntryList(CurrentEntry);
            }

        } else {
            CurrentPage += 1;
        }
    }

    *CurrentOsDescriptorListEntry = NextEntry;
    return;
}

NTSTATUS
OslFwpFindFreeFirmwareVaRange (
    __in ULONGLONG Size,
    __out PVOID *FreeRangeStartAddress
    )

/*++

Routine Description:

    Finds a free virtual range to map the specified number of bytes.  The
    virtual range is allocated downwards, starting from the bottom of the HAL's
    reserved VA region.

Arguments:

    Size - Supplies the size of free virtual address range required (in bytes)
        Must be page aligned.

    FreeRangeStartAddress - Supplies a pointer to variable that receives the
        starting virtual address of the free range.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_UNSUCCESSFUL otherwise.

--*/

{

    PVOID CurrentAddress;
    BOOLEAN IsMapped;
    ULONGLONG MaximumSize;
    ULONGLONG NumberOfPages;
    ULONGLONG PagesMapped;
    PHYSICAL_ADDRESS PhysicalAddress;
    PVOID RangeEnd;
    PVOID RangeStart;
    NTSTATUS Status;

    ASSERT((Size & (PAGE_SIZE - 1)) == 0);

    MaximumSize = FIRMWARE_RESERVE_MEMORY_START - FIRMWARE_RESERVE_MEMORY_END;
    if (Size > MaximumSize) {
        return STATUS_UNSUCCESSFUL;
    }

    //
    // Set the starting and ending ranges.
    // N.B. It is possible for ending range to be less than RangeStart.
    //

    NumberOfPages = (Size >> PAGE_SHIFT);
    RangeStart = SubtractFromPtr(FIRMWARE_RESERVE_MEMORY_START, Size);
    RangeEnd = (PVOID)FIRMWARE_RESERVE_MEMORY_START;
    PagesMapped = 0;

    //
    // Walk through the boot PDE to find a consecutive chunk of free PTEs that
    // span NumberOfPages pages.
    //

    while (PagesMapped < NumberOfPages) {
        CurrentAddress = RangeStart;
        while ((CurrentAddress != RangeEnd) && (PagesMapped < NumberOfPages)) {

            //
            // Determine whether the VA CurrentAddress is already mapped. If it
            // is mapped, then start search with a new range that begins on the
            // adjacent page. If it is not mapped, increment the number of pages
            // mapped and continue with the range.
            //

            IsMapped = BlMmTranslateVirtualAddress(CurrentAddress,
                                                   &PhysicalAddress);

            if (IsMapped != FALSE) {
                RangeStart = SubtractFromPtr(CurrentAddress, Size);
                PagesMapped = 0;
                break;
            }

            CurrentAddress = Add2Ptr(CurrentAddress, PAGE_SIZE);
            PagesMapped += 1;
        }

        //
        // Terminate search if end of VA range is reached.
        //

        if (CurrentAddress < (PVOID)FIRMWARE_RESERVE_MEMORY_END) {
            break;
        }
    }

    if (PagesMapped == NumberOfPages) {

        ASSERT(RangeStart >= (PVOID)FIRMWARE_RESERVE_MEMORY_END);

        ASSERT(Add2Ptr(RangeStart, Size) <=
               (PVOID)FIRMWARE_RESERVE_MEMORY_START);

        *FreeRangeStartAddress = RangeStart;
        Status = STATUS_SUCCESS;

    } else {
        *FreeRangeStartAddress = NULL;
        Status = STATUS_UNSUCCESSFUL;
    }

    return Status;
}

VOID
OslFwpUpdateVirtualRuntimeServicesTable (
    __in EFI_RUNTIME_SERVICES *EfiRTRefTable,
    __in EFI_MEMORY_DESCRIPTOR *MemoryMap,
    __in UINTN MemoryMapSize,
    __in UINTN DescriptorSize,
    __out VIRTUAL_EFI_RUNTIME_SERVICES *EfiRTVirtualTable
    )

/*++

Routine Description:

    This routine sets the virtual address for the specified function inside the
    virtual runtime services table.

    N.B. At this point (Phase = 1) the kernel memory map has already been built.
         Hence no MM allocation routines should be called from this routine.

Arguments:

    EfiRTRefTable - Supplies a pointer to a runtime services table to be
        used as reference for looking up physical addresses.

    MemoryMap - Supplies a pointer to a list of descriptors that were
        previously mapped using OslFwpMapEfiRuntimeEnvironment.

    MemoryMapSize - Supplies the size (in bytes) of the MemoryMap buffer.

    DescriptorSize - Supplies the size of individual descriptors within the
        MemoryMap.

    EfiRTVirtualTable - Supplies a pointer to the virtual runtime services table
        that needs to be updated.

Return Value:

    None.

--*/

{

    EFI_MEMORY_DESCRIPTOR *EfiDescriptor;
    UINT64 Size;

    //
    // Zero the entire virtual runtime services table before initializing
    // individual entries. This ensures that any uninitialized entries are
    // zeroed.
    //

    RtlZeroMemory(EfiRTVirtualTable, sizeof(VIRTUAL_EFI_RUNTIME_SERVICES));

    //
    // Walk through the descriptor list identifying runtime descriptors.
    // For each runtime descriptor found, map it into reserved firmware virtual
    // address space.
    //

    for (EfiDescriptor = MemoryMap;
         MemoryMapSize != 0;
         MemoryMapSize -= DescriptorSize,
             EfiDescriptor = Add2Ptr(EfiDescriptor, DescriptorSize)) {

        if (((EfiDescriptor->Attribute & EFI_MEMORY_RUNTIME) == 0) ||
            (EfiDescriptor->Type != EfiRuntimeServicesCode)) {

            continue;
        }

        Size = (EfiDescriptor->NumberOfPages << EFI_PAGE_SHIFT);

        if (IS_ADDRESS_IN_REGION(EfiRTRefTable->GetTime,
                                 EfiDescriptor->PhysicalStart,
                                 Size)) {

            EfiRTVirtualTable->GetTime = SET_VIRTUAL_ADDRESS(
                                             EfiRTRefTable->GetTime,
                                             EfiDescriptor->PhysicalStart,
                                             EfiDescriptor->VirtualStart);
        }

        if (IS_ADDRESS_IN_REGION(EfiRTRefTable->SetTime,
                                 EfiDescriptor->PhysicalStart,
                                 Size)) {

            EfiRTVirtualTable->SetTime = SET_VIRTUAL_ADDRESS(
                                             EfiRTRefTable->SetTime,
                                             EfiDescriptor->PhysicalStart,
                                             EfiDescriptor->VirtualStart);

        }

        if (IS_ADDRESS_IN_REGION(EfiRTRefTable->GetWakeupTime,
                                 EfiDescriptor->PhysicalStart,
                                 Size)) {

            EfiRTVirtualTable->GetWakeupTime = SET_VIRTUAL_ADDRESS(
                                                   EfiRTRefTable->GetWakeupTime,
                                                   EfiDescriptor->PhysicalStart,
                                                   EfiDescriptor->VirtualStart);

        }

        if (IS_ADDRESS_IN_REGION(EfiRTRefTable->SetWakeupTime,
                                 EfiDescriptor->PhysicalStart,
                                 Size)) {

            EfiRTVirtualTable->SetWakeupTime = SET_VIRTUAL_ADDRESS(
                                                   EfiRTRefTable->SetWakeupTime,
                                                   EfiDescriptor->PhysicalStart,
                                                   EfiDescriptor->VirtualStart);

        }

        if (IS_ADDRESS_IN_REGION(EfiRTRefTable->SetVirtualAddressMap,
                                 EfiDescriptor->PhysicalStart,
                                 Size)) {

            EfiRTVirtualTable->SetVirtualAddressMap =
                        SET_VIRTUAL_ADDRESS(EfiRTRefTable->SetVirtualAddressMap,
                                            EfiDescriptor->PhysicalStart,
                                            EfiDescriptor->VirtualStart);

        }

        if (IS_ADDRESS_IN_REGION(EfiRTRefTable->ConvertPointer,
                                 EfiDescriptor->PhysicalStart,
                                 Size)) {

            EfiRTVirtualTable->ConvertPointer = SET_VIRTUAL_ADDRESS(
                                                    EfiRTRefTable->ConvertPointer,
                                                    EfiDescriptor->PhysicalStart,
                                                    EfiDescriptor->VirtualStart);

        }

        if (IS_ADDRESS_IN_REGION(EfiRTRefTable->GetVariable,
                                 EfiDescriptor->PhysicalStart,
                                 Size)) {

            EfiRTVirtualTable->GetVariable = SET_VIRTUAL_ADDRESS(
                                                 EfiRTRefTable->GetVariable,
                                                 EfiDescriptor->PhysicalStart,
                                                 EfiDescriptor->VirtualStart);

        }

        if (IS_ADDRESS_IN_REGION(EfiRTRefTable->GetNextVariableName,
                                 EfiDescriptor->PhysicalStart,
                                 Size)) {

            EfiRTVirtualTable->GetNextVariableName =
                                SET_VIRTUAL_ADDRESS(EfiRTRefTable->GetNextVariableName,
                                                    EfiDescriptor->PhysicalStart,
                                                    EfiDescriptor->VirtualStart);

        }

        if (IS_ADDRESS_IN_REGION(EfiRTRefTable->SetVariable,
                                 EfiDescriptor->PhysicalStart,
                                 Size)) {

            EfiRTVirtualTable->SetVariable = SET_VIRTUAL_ADDRESS(
                                                 EfiRTRefTable->SetVariable,
                                                 EfiDescriptor->PhysicalStart,
                                                 EfiDescriptor->VirtualStart);

        }

        if (IS_ADDRESS_IN_REGION(EfiRTRefTable->GetNextHighMonotonicCount,
                                 EfiDescriptor->PhysicalStart,
                                 Size)) {

            EfiRTVirtualTable->GetNextHighMonotonicCount =
                    SET_VIRTUAL_ADDRESS(EfiRTRefTable->GetNextHighMonotonicCount,
                                        EfiDescriptor->PhysicalStart,
                                        EfiDescriptor->VirtualStart);

        }

        if (IS_ADDRESS_IN_REGION(EfiRTRefTable->ResetSystem,
                                 EfiDescriptor->PhysicalStart,
                                 Size)) {

            EfiRTVirtualTable->ResetSystem = SET_VIRTUAL_ADDRESS(
                                                 EfiRTRefTable->ResetSystem,
                                                 EfiDescriptor->PhysicalStart,
                                                 EfiDescriptor->VirtualStart);
        }

        //
        // Initialize the UEFI 2.0 runtime service routines only if the table
        // revision number indicates that the firmware table contains valid
        // entries.
        //

        if (EfiRTRefTable->Hdr.Revision >= EFI_2_0_RUNTIME_TABLE_REVISION) {
            if (IS_ADDRESS_IN_REGION(EfiRTRefTable->UpdateCapsule,
                                     EfiDescriptor->PhysicalStart,
                                     Size)) {

                EfiRTVirtualTable->UpdateCapsule =
                    SET_VIRTUAL_ADDRESS(EfiRTRefTable->UpdateCapsule,
                                        EfiDescriptor->PhysicalStart,
                                        EfiDescriptor->VirtualStart);
            }

            if (IS_ADDRESS_IN_REGION(EfiRTRefTable->QueryCapsuleCapabilities,
                                     EfiDescriptor->PhysicalStart,
                                     Size)) {

                EfiRTVirtualTable->QueryCapsuleCapabilities =
                    SET_VIRTUAL_ADDRESS(EfiRTRefTable->QueryCapsuleCapabilities,
                                        EfiDescriptor->PhysicalStart,
                                        EfiDescriptor->VirtualStart);
            }

            if (IS_ADDRESS_IN_REGION(EfiRTRefTable->QueryVariableInfo,
                                     EfiDescriptor->PhysicalStart,
                                     Size)) {

                EfiRTVirtualTable->QueryVariableInfo =
                    SET_VIRTUAL_ADDRESS(EfiRTRefTable->QueryVariableInfo,
                                        EfiDescriptor->PhysicalStart,
                                        EfiDescriptor->VirtualStart);
            }
        }
    }
}

NTSTATUS
OslFwpVirtualizeRuntimeServices (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine maps EFI Runtime Code/Data into virtual memory space, and
    communicates the necessary VA space requirements to the OS.

    N.B. All the mapping operations that may use MM library services must be
         done only during Phase 0. Invoking MM library routines after Phase 0
         can cause the system memory map to change and hence the memory map
         built in OslBuildKernelMemoryMap won't be accurate.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block that holds
        the VIRTUAL_EFI_RUNTIME_SERVICES structure.

Return Value:

    NT status code.

--*/

{
    ULONG Attribute;
    PEFI_MEMORY_ATTRIBUTES_TABLE AttributesTable;
    SIZE_T AttributesTableSize;
    UINTN DescriptorSize;
    UINT32 DescriptorVersion;
    EFI_MEMORY_DESCRIPTOR *EfiDescriptor;
    PEFI_FIRMWARE_INFORMATION EfiInformation;
    UINTN MapKey;
    BOOLEAN Mapped;
    EFI_MEMORY_DESCRIPTOR *MemoryAttribute;
    UINTN MemoryMapSize;
    ULONG NumAttributeEntries;
    PHYSICAL_ADDRESS PhysicalAddress;
    BOOLEAN RwxPages;
    NTSTATUS Status;
    PVOID VirtualAddress;
    EFI_MEMORY_DESCRIPTOR *VirtualMemoryMap;

    AttributesTable = NULL;
    AttributesTableSize = 0;

    //
    // Allocate the buffers needed to hold descriptors for virtual translation
    // of runtime services.
    //
    // N.B. Failure to allocate memory for runtime buffers is fatal.
    //

    Status = OslFwpAllocateRuntimeVirtualDescBuffers();
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Read the current firmware memory map into the alternate buffer.
    // Ensure that address translation is enabled as virtual address is being
    // supplied to EfiGetMemoryMap.
    //

    ASSERT(BlMmIsTranslationEnabled() != FALSE);

    MemoryMapSize = OslFwpAltRuntimeVirtualDescSizeTotal;
    Status = EfiGetMemoryMap(&MemoryMapSize,
                             OslFwpAltRuntimeVirtualDescVa,
                             &MapKey,
                             &DescriptorSize,
                             &DescriptorVersion);

    if (!NT_SUCCESS(Status)) {
        goto OslFwVirtualizeRuntimeServicesExit;
    }

    EfiInformation = &LoaderBlock->FirmwareInformation.u.EfiInformation;
    EfiInformation->FirmwareVersion = EfiRT->Hdr.Revision;

    //
    // Walk through the descriptor list identifying runtime descriptors.
    //

    Status = STATUS_SUCCESS;
    VirtualMemoryMap = (EFI_MEMORY_DESCRIPTOR *)OslFwpRuntimeVirtualDescVa;
    OslFwpRuntimeVirtualDescSizeSet = 0;
    for (EfiDescriptor = OslFwpAltRuntimeVirtualDescVa;
         MemoryMapSize != 0;
         MemoryMapSize -= DescriptorSize,
             EfiDescriptor = Add2Ptr(EfiDescriptor, DescriptorSize)) {

        if ((EfiDescriptor->Attribute & EFI_MEMORY_RUNTIME) == 0) {
            continue;
        }

        //
        // Validate that descriptor Type and VirtualStart fields have expected
        // values. Type should hold one of the predefined values and
        // VirtualStart field is expected to be 0 as the runtime services have
        // not been virtualized.
        //

        ASSERT(IS_RUNTIME_TYPE_VALID(EfiDescriptor->Type) != FALSE);

        ASSERT(EfiDescriptor->VirtualStart == 0);

        //
        // Make sure that there is enough space left in the virtual
        // runtime descriptor buffer to hold a new runtime descriptor.
        //

        if ((OslFwpRuntimeVirtualDescSizeSet + DescriptorSize) >
            OslFwpRuntimeVirtualDescSizeTotal) {

            Status = STATUS_BUFFER_TOO_SMALL;
            goto OslFwVirtualizeRuntimeServicesExit;
        }

        //
        // Copy the descriptor into the virtual runtime descriptor buffer.
        //

        RtlCopyMemory(VirtualMemoryMap, EfiDescriptor, DescriptorSize);
        VirtualMemoryMap = Add2Ptr(VirtualMemoryMap, DescriptorSize);
        OslFwpRuntimeVirtualDescSizeSet += DescriptorSize;
    }

    //
    // No need to continue if no runtime descriptors were found.
    //

    if (OslFwpRuntimeVirtualDescSizeSet == 0) {
        goto OslFwVirtualizeRuntimeServicesExit;
    }

    //
    // Sort descriptors by physical address.
    //

    qsort(OslFwpRuntimeVirtualDescVa,
          OslFwpRuntimeVirtualDescSizeSet / DescriptorSize,
          DescriptorSize,
          OslFwpEfiDescriptorCompare);

    //
    // Retrieve the EFI memory attributes table if it exists. This table
    // contains descriptors which describe subsets of the ranges present in the
    // memory map. These descriptors can be used to set more restrictive page
    // protections on the memory regions.
    //

    AttributesTable = EfiGetMemoryAttributesTable(&AttributesTableSize);
    if (AttributesTable != NULL) {
        NumAttributeEntries = AttributesTable->NumberOfEntries;

    } else {
        NumAttributeEntries = 0;
    }

    //
    // For each runtime descriptor found, map it into reserved firmware virtual
    // address space. The list is walked backwards, so that runtime mappings are
    // allocated in bottom up order.
    //

    EfiDescriptor = Add2Ptr(OslFwpRuntimeVirtualDescVa,
                            OslFwpRuntimeVirtualDescSizeSet);

    RwxPages = FALSE;
    while (EfiDescriptor != OslFwpRuntimeVirtualDescVa) {
        EfiDescriptor = (EFI_MEMORY_DESCRIPTOR*)
            ((ULONG_PTR)EfiDescriptor - (ULONG_PTR)DescriptorSize);

        Mapped = FALSE;

        NT_ASSERT(EfiDescriptor->VirtualStart == 0);

        //
        // Remove protection attributes.
        //

        EfiDescriptor->Attribute &= ~(EFI_MEMORY_RO | EFI_MEMORY_XP);

        //
        // The memory attributes table may define multiple entries to describe
        // subregions that comprise a single entry in the memory map. However,
        // the subregions must combine to completely describe the larger region
        // and may not cross boundaries between memory map entries. If a runtime
        // region returned in the memory map is not described within the memory
        // attributes table, this region is assumed to not be compatible with
        // any memory protections.
        //
        // This table is sorted by physical start address in ascending order. It
        // is walked in reverse order to match how virtual addresses are
        // assigned by the boot loader.
        //

        for (; NumAttributeEntries != 0; NumAttributeEntries -= 1) {
            MemoryAttribute = Add2Ptr(AttributesTable->Entry,
                                      (NumAttributeEntries - 1) * AttributesTable->DescriptorSize);

            //
            // According to the UEFI 2.6 spec, any entries of type other than
            // runtime services code or data are to be ignored.
            //

            if ((MemoryAttribute->Type != EfiRuntimeServicesCode) &&
                (MemoryAttribute->Type != EfiRuntimeServicesData)) {

                continue;
            }

            //
            // If the current attribute descriptor is beyond the range of the
            // current memory map descriptor, stop processing attributes for
            // this memory region.
            //

            if (MemoryAttribute->PhysicalStart < EfiDescriptor->PhysicalStart) {
                break;
            }

            //
            // If the attribute descriptor is invalid, log a debug message and
            // abort.
            //

            if (((MemoryAttribute->PhysicalStart + (MemoryAttribute->NumberOfPages << EFI_PAGE_SHIFT)) >
                 (EfiDescriptor->PhysicalStart + (EfiDescriptor->NumberOfPages << EFI_PAGE_SHIFT))) ||
                (EfiDescriptor->Type != MemoryAttribute->Type)) {

                BlStatusPrint(
                    L"Invalid memory attribute region [%I64x:%I64x] (type %x, "
                    L"attr %I64x) while processing descriptor [%I64x:%I64x] ("
                    L"type %x, attr %I64x)\n",
                    MemoryAttribute->PhysicalStart,
                    MemoryAttribute->PhysicalStart + (MemoryAttribute->NumberOfPages << EFI_PAGE_SHIFT),
                    MemoryAttribute->Type,
                    MemoryAttribute->Attribute,
                    EfiDescriptor->PhysicalStart,
                    EfiDescriptor->PhysicalStart + (EfiDescriptor->NumberOfPages << EFI_PAGE_SHIFT),
                    EfiDescriptor->Type,
                    EfiDescriptor->Attribute);

                Status = STATUS_UNSUCCESSFUL;
                goto OslFwVirtualizeRuntimeServicesExit;
            }

            NT_ASSERT(MemoryAttribute->VirtualStart == 0);

            //
            // Combine the protection attributes specified by the attribute
            // descriptor with the attributes provided in the memory map
            // descriptor, and convert to loader format.
            //

            Attribute = OslFwpGetOsAttributeType(
                            EfiDescriptor->Attribute | MemoryAttribute->Attribute);

            //
            // Check for the presence of regions that are both writable and
            // executable.
            //

            if ((Attribute & (MEMORY_ATTRIBUTE_PROTECT_WRITE | MEMORY_ATTRIBUTE_PROTECT_EXECUTE)) == 0) {
                RwxPages = TRUE;
            }

            PhysicalAddress.QuadPart = MemoryAttribute->PhysicalStart;

            //
            // Map the memory region, using whatever protection attributes were
            // specified in the memory attributes table descriptor.
            //

            Status = OslFwMapPhysicalAddressInFirmwareVaSpace(
                         PhysicalAddress,
                         (MemoryAttribute->NumberOfPages << EFI_PAGE_SHIFT),
                         Attribute,
                         &VirtualAddress);

            //
            // If the map operation failed, no need to map remaining descriptors.
            //

            if (!NT_SUCCESS(Status)) {
                goto OslFwVirtualizeRuntimeServicesExit;
            }

            Mapped = TRUE;

            //
            // Copy the descriptor into the virtual runtime descriptor buffer
            // and update the corresponding VA.
            //

            if (MemoryAttribute->PhysicalStart == EfiDescriptor->PhysicalStart) {
                EfiDescriptor->VirtualStart = (EFI_VIRTUAL_ADDRESS)VirtualAddress;
            }
        }

        //
        // If the region has already been mapped (due to having memory
        // attributes table descriptors) proceed to the next memory map
        // descriptor.
        //

        if (Mapped != FALSE) {
            continue;
        }

        Attribute = OslFwpGetOsAttributeType(EfiDescriptor->Attribute);
        PhysicalAddress.QuadPart = EfiDescriptor->PhysicalStart;

        //
        // Only runtime services code and data regions count as RWX regions
        // (thereby being incompatible with HVCI).
        //

        if ((EfiDescriptor->Type == EfiRuntimeServicesCode) ||
            (EfiDescriptor->Type == EfiRuntimeServicesData)) {

            RwxPages = TRUE;
        }

        Status = OslFwMapPhysicalAddressInFirmwareVaSpace(
                     PhysicalAddress,
                     (EfiDescriptor->NumberOfPages << EFI_PAGE_SHIFT),
                     Attribute,
                     &VirtualAddress);

        //
        // If the map operation failed, no need to map remaining descriptors.
        //

        if (!NT_SUCCESS(Status)) {
            goto OslFwVirtualizeRuntimeServicesExit;
        }

        //
        // Copy the descriptor into the virtual runtime descriptor buffer
        // and update the corresponding VA.
        //

        EfiDescriptor->VirtualStart =
            (EFI_VIRTUAL_ADDRESS)((ULONG_PTR)VirtualAddress);
    }

    //
    // Check for any remaining (unparented) memory attribute descriptors, which
    // are in violation of the UEFI 2.6 specification.
    //

    for (; NumAttributeEntries != 0; NumAttributeEntries -= 1) {
        MemoryAttribute = Add2Ptr(AttributesTable->Entry,
                                  (NumAttributeEntries - 1) * AttributesTable->DescriptorSize);

        //
        // According to the UEFI 2.6 spec, any entries of type other than
        // runtime services code or data are to be ignored.
        //

        if ((MemoryAttribute->Type != EfiRuntimeServicesCode) &&
            (MemoryAttribute->Type != EfiRuntimeServicesData)) {

            continue;
        }

        BlStatusPrint(
            L"Unexpected memory attribute region [%I64x:%I64x] (type %x, attr "
            L"%I64x) outside of memory map\n",
            MemoryAttribute->PhysicalStart,
            MemoryAttribute->PhysicalStart + (MemoryAttribute->NumberOfPages << EFI_PAGE_SHIFT),
            MemoryAttribute->Type,
            MemoryAttribute->Attribute);

        Status = STATUS_UNSUCCESSFUL;
        goto OslFwVirtualizeRuntimeServicesExit;
    }

    EfiInformation->EfiMemoryMap = OslFwpRuntimeVirtualDescVa;
    EfiInformation->EfiMemoryMapSize = (ULONG)OslFwpRuntimeVirtualDescSizeSet;
    EfiInformation->EfiMemoryMapDescriptorSize = (ULONG)DescriptorSize;

    LoaderBlock->FirmwareInformation.EfiRuntimePageProtectionSupported =
        (RwxPages == FALSE);

#if defined(NT_IUM)

    Status = OslVsmEfiSetup(EfiInformation->EfiMemoryMap,
                            EfiInformation->EfiMemoryMapSize,
                            EfiInformation->EfiMemoryMapDescriptorSize,
                            AttributesTable,
                            AttributesTableSize);

#endif

OslFwVirtualizeRuntimeServicesExit:
    if (!NT_SUCCESS(Status)) {
        if (OslFwpRuntimeVirtualDescVa != NULL) {
            BlMmFreePages(OslFwpRuntimeVirtualDescVa);
            OslFwpRuntimeVirtualDescVa = NULL;
        }

        if (OslFwpAltRuntimeVirtualDescVa != NULL) {
            BlMmFreePages(OslFwpAltRuntimeVirtualDescVa);
            OslFwpAltRuntimeVirtualDescVa = NULL;
        }
    }

    if (AttributesTable != NULL) {
        BlMmUnmapVirtualAddress(AttributesTable, AttributesTableSize);
    }

    return Status;
}

NTSTATUS
OslFwpUpdateResumeContext (
    __in UINTN MemoryMapSize,
    __in UINTN DescriptorSize,
    __in UINT32 DescriptorVersion,
    __in_bcount(MemoryMapSize) EFI_MEMORY_DESCRIPTOR *VirtualMemoryMap
    )

/*++

Routine Description:

    This routine updates the resume context with virtual address information
    for the EFI runtime services.

Arguments:

    MemoryMapSize - Supplies the size (in bytes) of the MemoryMap buffer.

    DescriptorSize - Supplies the size of individual descriptors within the
        MemoryMap.
    DescriptorVersion - Supplies the version for EFI memory descriptors.

    VirtualMemoryMap - Supplies a pointer to a list of descriptors describing
        the virtual addresses for EFI runtime services.

Return Value:

    NT status code.

--*/

{

    ULONGLONG BasePage;
    PMEMORY_DESCRIPTOR Descriptor;
    EFI_MEMORY_DESCRIPTOR *EfiDescriptor;
    ULONG Index;

    UNREFERENCED_PARAMETER(DescriptorVersion);

    if ((MemoryMapSize == 0) ||
        (DescriptorSize == 0) ||
        (VirtualMemoryMap == NULL)) {

        return STATUS_INVALID_PARAMETER;
    }

    if (ResumeContext == NULL) {
        return STATUS_UNSUCCESSFUL;
    }

    //
    // For each EFI descriptor, find its corresponding descriptor in the
    // resume context, and update its virtual address.
    //

    ASSERT((MemoryMapSize % DescriptorSize) == 0);

    for (EfiDescriptor = VirtualMemoryMap;
         MemoryMapSize >= DescriptorSize;
         MemoryMapSize -= DescriptorSize,
             EfiDescriptor = Add2Ptr(EfiDescriptor, DescriptorSize)) {

        BasePage = EfiDescriptor->PhysicalStart >> PAGE_SHIFT;
        Descriptor = &ResumeContext->PhysicalMemoryMap[0];
        for (Index = 0;
             Index < ResumeContext->PhysicalMemoryMapCount;
             Index += 1) {

            //
            // The descriptors have been coalesced on initialization. Coalesced
            // descriptors are both physically and virtually contiguous. Find
            // the coalesced MDL this EFI descriptor falls in. If the page is
            // at an MDL boundary, set the value of the mapped base page
            // appropriately.
            //

            if ((BasePage >= Descriptor->BasePage) &&
                (BasePage < (Descriptor->BasePage + Descriptor->PageCount))) {

                if (BasePage == Descriptor->BasePage) {
                    Descriptor->MappedBasePage =
                        (EfiDescriptor->VirtualStart >> PAGE_SHIFT);

                }
                break;
            }

            Descriptor += 1;
        }

        if (Index == ResumeContext->PhysicalMemoryMapCount) {
            return STATUS_UNSUCCESSFUL;
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS
OslFwpZeroFirmwareServicesBootMappings (
    __in EFI_MEMORY_DESCRIPTOR *MemoryMap,
    __in UINTN MemoryMapSize,
    __in UINTN DescriptorSize
    )

/*++

Routine Description:

    This routine reclaims 1:1 boot mappings used for the firmware services.

    The runtime services are doubly mapped just below the HAL virtual address
    region. Because the physical pages mapped at both locations are described as
    firmware permanent memory, NTOS will not reclaim either mapping. During OS
    execution, the 1:1 mappings do not need to exist and NTOS does not know to
    clean them up. Therefore they will be reclaimed now; this consists of
    clearing the PTEs.

    The boot services are only mapped 1:1. In some scenarios MM will not reclaim
    these mappings and the system will bugcheck due to invalid PFN share
    counts. Zero the PTEs to reclaim these.

Arguments:

    MemoryMap - Supplies a pointer to a list of UEFI memory descriptors
        collected just before exit boot services.

    MemoryMapSize - Supplies the number of descriptors in the memory map.

    DescriptorSize - Supplies the size of each descriptor as indicated by the
        firmware.

Return Value:

    NTSTATUS Code.

--*/

{

    ULONG_PTR BootRangeEnd;
    PVOID BootVirtualAddress;
    EFI_MEMORY_DESCRIPTOR *EfiDescriptor;
    BOOLEAN IsMapped;
    PHYSICAL_ADDRESS PhysicalAddress;
    ULONGLONG RangeSize;
    NTSTATUS Status;
    PVOID TempAddress;

    //
    // Check to see that our memory map size is descriptor-aligned.
    //

    ASSERT((MemoryMapSize % DescriptorSize) == 0);

    //
    // Loop through the memory map, clearing any 1:1 mappings for firmware
    // services (runtime and boot). Note that there are other UEFI memory
    // regions but the boot library only maps the boot and runtimes services
    // 1:1.
    //

    PhysicalAddress.QuadPart = 0;
    for (EfiDescriptor = MemoryMap;
         MemoryMapSize != 0;
         MemoryMapSize -= DescriptorSize,
             EfiDescriptor = Add2Ptr(EfiDescriptor, DescriptorSize)) {

        //
        // Do nothing with PFN 0. It was not mapped 1:1 because it is marked as
        // MEMORY_TYPE_PLATFORM_RESERVED by the boot library. If this is ever
        // changed, and BlMmTranslateVirtualAddress can handle virtual address
        // 0, then this check can be removed.
        //

        if (EfiDescriptor->PhysicalStart == 0) {
            continue;
        }

        //
        // Only zero the 1:1 mappings for the runtime and boot services code and
        // data regions.
        //

        if ((EfiDescriptor->Type != EfiRuntimeServicesCode) &&
            (EfiDescriptor->Type != EfiRuntimeServicesData) &&
            (EfiDescriptor->Type != EfiBootServicesCode) &&
            (EfiDescriptor->Type != EfiBootServicesData)) {

            continue;
        }

        //
        // This descriptor should not have been mapped from the firmware's
        // perspective when it was collected before exit boot services.
        //

        ASSERT(EfiDescriptor->VirtualStart == 0);

        //
        // Get the 1:1 virtual address. Skip it if it is in the HAL VA
        // range or in user VA space. These will get zero'd separately.
        //
        // N.B. This cast is safe on x86 because UEFI runs in physical mode.
        //

        BootVirtualAddress = (PVOID)(ULONG_PTR)EfiDescriptor->PhysicalStart;
        RangeSize = EfiDescriptor->NumberOfPages << PAGE_SHIFT;
        BootRangeEnd = (ULONG_PTR)Add2Ptr(BootVirtualAddress, RangeSize);
        if (((ULONG_PTR)BootVirtualAddress >= ZERO_FIRMWARE_SERVICE_END) ||
            (BootRangeEnd < ZERO_FIRMWARE_SERVICE_START)) {

            continue;
        }

        //
        // Validate that this region was mapped 1:1. If it was not mapped 1:1,
        // then do not zero the 1:1 virtual address region as this could
        // zero other valid PTEs.
        //

        do {

            //
            // Search for contiguous run of 1:1 mapped pages.
            //

            TempAddress = BootVirtualAddress;
            RangeSize = 0;
            do {
                IsMapped = BlMmTranslateVirtualAddress(TempAddress,
                                                       &PhysicalAddress);

                if ((IsMapped == FALSE) ||
                    ((ULONG_PTR)PhysicalAddress.QuadPart !=
                     (ULONG_PTR)TempAddress)) {

                    TempAddress = Add2Ptr(TempAddress, PAGE_SIZE);
                    break;

                } else {
                    RangeSize += PAGE_SIZE;
                    TempAddress = Add2Ptr(TempAddress, PAGE_SIZE);
                }

            } while ((ULONG_PTR)TempAddress != BootRangeEnd);

            //
            // Zero the virtual address region consumed by this runtime/boot
            // services descriptor.
            //

            if (RangeSize > 0) {
                Status = BlMmZeroVirtualAddressRange(BootVirtualAddress, RangeSize);
                if (!NT_SUCCESS(Status)) {
                    return Status;
                }
            }

            //
            // Advance to the next page.
            //

            BootVirtualAddress = TempAddress;
        } while ((ULONG_PTR)BootVirtualAddress != BootRangeEnd);
    }

    return STATUS_SUCCESS;
}

NTSTATUS
OslpFwSaveCapsuleDump(
    __in DEVICE_ID TriageDeviceId
    )
/*++

Routine Description:

    This routine determines if the firmware has a persisted triage dump
    and passes this to the OS via the page file.

    On EFI this dump may have been preserved via the capsule mechanism
    and will be present in the EFI system table.

Arguments:

    TriageDeviceId - The id of the device where we should write the triage
        dump in the pagefile.

Return Value:

    NT_STATUS

--*/

{
    PHYSICAL_ADDRESS Address;
    ULONG ByteCount;
    PUINT32 CapsuleCount;
    PVOID CapsuleDump;
    EFI_CAPSULE_HEADER *CapsuleHeader;
    UINT32 CapsuleHeaderSize;
    UINT32 CapsuleImageSize;
    PUCHAR CapsuleTable;
    ULONG CapsuleTableSize;
    DEVICE_INFORMATION DeviceInformation;
    NTSTATUS DeviceStatus;
    BOOLEAN DisableFveOnWrite;
    PDUMP_HEADER DumpHeader;
    ULONG DumpOffset;
    ULONG DumpSize;
    ULONG FveDeviceFlags;
    PMEMORY_DUMP MemoryDump;
    ULONG OriginalIoAccessFlags;
    FILE_ID PageFileId;
    ULONG PcrBitmap;
    NTSTATUS Status;
    PULONG ValidPtr;
    PBOOT_ENVIRONMENT_DEVICE TempPageFileDevice;
    DEVICE_ID TempPageFileDeviceId;

    ASSERT(TriageDeviceId != INVALID_DEVICE_ID);

    MemoryDump = NULL;
    CapsuleDump = NULL;
    DumpSize = 0;
    CapsuleHeader = NULL;
    CapsuleTable = NULL;
    CapsuleTableSize = sizeof(UINT32) + sizeof(PVOID);
    PageFileId = INVALID_FILE_ID;
    TempPageFileDevice = NULL;
    TempPageFileDeviceId = INVALID_DEVICE_ID;
    Status = BlFwGetSystemTable(FW_TABLE_WINDOWS_DUMP, &Address);
    if (!NT_SUCCESS(Status)) {
        goto OslpFwLoadCrashDumpEnd;
    }

    //
    // Map the capsule table which is a UINT32 count followed by N
    // void pointers to capsules.  We're only interested in the first
    // entry.
    //
    // N.B. There is an ambiguity in the EFI specification as to
    //      whether the UINT32 value is a count or a size.  TianoCore
    //      appears to interpret it as a count.  This code is should
    //      work either way.
    //

    Status = BlMmMapPhysicalAddress(&CapsuleTable,
                                    Address,
                                    CapsuleTableSize,
                                    MEMORY_ATTRIBUTE_NONE);

    if (!NT_SUCCESS(Status)) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto OslpFwLoadCrashDumpEnd;
    }

    //
    // Get first capsule associated with this GUID
    //

    CapsuleCount = (PUINT32) CapsuleTable;
    if (*CapsuleCount == 0) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto OslpFwLoadCrashDumpEnd;
    }

    Address.QuadPart = (ULONG_PTR) *((PVOID *)(CapsuleTable + sizeof(UINT32)));

    //
    // Map the capsule header.
    //

    Status = BlMmMapPhysicalAddress(&CapsuleHeader,
                                    Address,
                                    sizeof(EFI_CAPSULE_HEADER),
                                    MEMORY_ATTRIBUTE_NONE);

    if (!NT_SUCCESS(Status)) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto OslpFwLoadCrashDumpEnd;
    }

    //
    // N.B. There is an ambiguity in the EFI specification as to
    //      whether the field CapsuleImageSize includes the HeaderSize or not.
    //      The Tianocore implementation appears to include the HeaderSize.
    //

    CapsuleHeaderSize = CapsuleHeader->HeaderSize;
    CapsuleImageSize = CapsuleHeader->CapsuleImageSize;
    DumpSize = CapsuleImageSize - CapsuleHeaderSize;
    DumpOffset = CapsuleHeaderSize;

    //
    // Map the dump
    //

    Address.QuadPart += DumpOffset;
    Status = BlMmMapPhysicalAddress(&CapsuleDump,
                                    Address,
                                    DumpSize,
                                    MEMORY_ATTRIBUTE_NONE);

    if (!NT_SUCCESS(Status)) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto OslpFwLoadCrashDumpEnd;
    }

    //
    // Check to see if the dump has the appropriate signature. If it does not,
    // test to see if BitLocker is enabled. If BitLocker is enabled, assume the
    // header check failed because the dump is in encrypted and
    // proceed. Otherwise, fail.
    //

    DumpHeader = CapsuleDump;
    if ((DumpHeader->ValidDump == DUMP_VALID_DUMP) &&
        (DumpHeader->DumpType == DUMP_TYPE_TRIAGE) &&
        (DumpHeader->Signature == DUMP_SIGNATURE)) {

        MemoryDump = CapsuleDump;
        ValidPtr = Add2Ptr(MemoryDump, MemoryDump->Triage.ValidOffset);
        if ((MemoryDump->Triage.ValidOffset >
             (TRIAGE_DUMP_SIZE - sizeof(ULONG))) ||
            (*ValidPtr != TRIAGE_DUMP_VALID)) {

            Status = STATUS_INVALID_IMAGE_FORMAT;
            goto OslpFwLoadCrashDumpEnd;
        }

        DisableFveOnWrite = FALSE;

    } else {
        Status = BlFveQueryStaticDeviceStatus(OslLoadDevice,
                                              &FveDeviceFlags,
                                              &PcrBitmap);

        if (!NT_SUCCESS(Status) ||
            (BlFveBitlockerEnabled(FveDeviceFlags) == FALSE)) {

            Status = STATUS_INVALID_IMAGE_FORMAT;
            goto OslpFwLoadCrashDumpEnd;
        }

        DisableFveOnWrite = TRUE;
    }

    //
    // Open the pagefile to write the triage dump back in.
    //

    Status = BlFileOpen(TriageDeviceId, PAGEFILE_SYS, OPEN_WRITE, &PageFileId);
    if (!NT_SUCCESS(Status)) {
        Status = BlGetApplicationOptionDevice(BCDE_OSLOADER_TYPE_TEMP_PAGEFILE_DEVICE,
                                              &TempPageFileDevice,
                                              NULL);

        if (NT_SUCCESS(Status)) {

            //
            // The BCD store specifies an override for the device containing the
            // temporary page file. Open the device and attempt to open the page file.
            //

            Status = BlDeviceOpen(TempPageFileDevice,
                                  OPEN_READ_WRITE,
                                  &TempPageFileDeviceId);

            if (NT_SUCCESS(Status)) {

                //
                // The temporary pagefile device is only valid for upgrade scenario now. 
                // Upgrade always suspends bitlocker against the OS volume on downlevel. 
                // So there is no security concern to write page file to the device.
                //

                TriageDeviceId = TempPageFileDeviceId;

                Status = BlFileOpen(TempPageFileDeviceId, TEMP_PAGEFILE_SYS, OPEN_WRITE, &PageFileId);
            }
        }
        if (!NT_SUCCESS(Status)) {
            goto OslpFwLoadCrashDumpEnd;
        }
    }

    //
    // Disable encryption for this write if necessary. The dump contents are
    // already encrypted and this is required to avoid double encryption.
    //

    OriginalIoAccessFlags = 0;
    RtlZeroMemory(&DeviceInformation, sizeof(DeviceInformation));
    if (DisableFveOnWrite != FALSE) {
        DeviceStatus = BlDeviceGetInformation(TriageDeviceId,
                                              &DeviceInformation);

        ASSERT(DeviceStatus == STATUS_SUCCESS);

        if (DeviceInformation.DeviceType == DEVICE_BLOCK_IO) {
            OriginalIoAccessFlags = DeviceInformation.u.BlockIo.IoAccessFlags;
            SET_FLAGS(DeviceInformation.u.BlockIo.IoAccessFlags,
                      BLOCK_IO_ACCESS_FLAGS_NO_FVE_ON_WRITE);

            DeviceStatus = BlDeviceSetInformation(TriageDeviceId,
                                                  &DeviceInformation);

            ASSERT(DeviceStatus == STATUS_SUCCESS);
        }
    }

    //
    // Write the dump header out to the pagefile.
    //

    Status = BlFileWriteAtOffset(PageFileId,
                                 DumpSize,
                                 0,
                                 CapsuleDump,
                                 &ByteCount);

    //
    // Re-enable encrypted writes, if necessary.
    //

    if (DisableFveOnWrite != FALSE) {
        if (DeviceInformation.DeviceType == DEVICE_BLOCK_IO) {
            DeviceInformation.u.BlockIo.IoAccessFlags = OriginalIoAccessFlags;
            DeviceStatus = BlDeviceSetInformation(TriageDeviceId,
                                                  &DeviceInformation);

            ASSERT(DeviceStatus == STATUS_SUCCESS);
        }
    }

    if (!NT_SUCCESS(Status) || (ByteCount != DumpSize)) {
        goto OslpFwLoadCrashDumpEnd;
    }

    //
    // Clean up allocations and resources.
    //

OslpFwLoadCrashDumpEnd:

    if (CapsuleTable != NULL) {
        BlMmUnmapVirtualAddress(CapsuleTable, CapsuleTableSize);
    }

    if (CapsuleHeader != NULL) {
        BlMmUnmapVirtualAddress(CapsuleHeader, sizeof(EFI_CAPSULE_HEADER));
    }

    if (CapsuleDump != NULL) {
        BlMmUnmapVirtualAddress(CapsuleDump, DumpSize);
    }

    if (PageFileId != INVALID_FILE_ID) {
        BlFileClose(PageFileId);
        PageFileId = INVALID_FILE_ID;
    }

    if (TempPageFileDeviceId != INVALID_DEVICE_ID) {
        BlDeviceClose(TempPageFileDeviceId);
    }

    if (TempPageFileDevice != NULL) {
        BlMmFreeHeap(TempPageFileDevice);
    }

    return Status;
}

NTSTATUS
OslFwGetEfiVersion (
    __out PUNICODE_STRING Version
    )

/*++

Routine Description:

    This routine retrieves the EFI firmware's version.

Arguments:

    Version - Supplies the string to which this machine will write
        the EFI firmware's version. If this routine succeeds, it will have
        allocated a buffer for this string and the caller will be responsible
        for freeing that buffer. If this routine fails, it will leave this
        string unchanged.

Return Value:

    NTSTATUS.

--*/

{

    PWCHAR Buffer;
    USHORT CharactersPrintedCount;
    USHORT MaximumLength;
    NTSTATUS Status;
    PWSTR Vendor;
    ULONG_PTR VendorCharacterCount;
    USHORT VendorLength;

    Buffer = NULL;

    //
    // Get the number of characters in the EFI system table's
    // FirmwareVendor string.
    //

    Vendor = EfiST->FirmwareVendor;
    if (Vendor == NULL) {
        Status = STATUS_UNSUCCESSFUL;
        goto GetEfiVersionEnd;
    }

    VendorCharacterCount = wcslen(Vendor);

    //
    // Compute the vendor string's size in bytes. Fail if the string is
    // too long.
    //

    if (VendorCharacterCount <= (MAX_EFI_VENDOR_LENGTH / sizeof(WCHAR))) {
        VendorLength = (USHORT)VendorCharacterCount * sizeof(WCHAR);

    } else {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto GetEfiVersionEnd;
    }

    //
    // Allocate a buffer for the version string.
    //

    MaximumLength = OslFwpEfiVersionBufferLength(VendorLength);
    Buffer = BlMmAllocateHeap(MaximumLength);
    if (Buffer == NULL) {
        Status = STATUS_NO_MEMORY;
        goto GetEfiVersionEnd;
    }

    //
    // Copy the vendor string to the version string.
    //

    RtlCopyMemory(Buffer, Vendor, VendorLength);

    //
    // Print the firmware's revision to the version string.
    //

    CharactersPrintedCount =
        (USHORT)swprintf_s(Add2Ptr(Buffer, VendorLength),
                           (MaximumLength - VendorLength) / sizeof(WCHAR),
                           EFI_VERSION_SEPARATOR_AND_REVISION_FORMAT,
                           EfiST->FirmwareRevision);

    if (CharactersPrintedCount == -1) {
        Status = STATUS_UNSUCCESSFUL;
        goto GetEfiVersionEnd;
    }

    //
    // Write the results of this routine's work to the Version string
    // that the caller passed and set the return status to STATUS_SUCCESS.
    //

    Version->Buffer = Buffer;
    Version->Length = VendorLength + (CharactersPrintedCount * sizeof(WCHAR));
    Version->MaximumLength = MaximumLength;
    Status = STATUS_SUCCESS;

GetEfiVersionEnd:
    if ((!NT_SUCCESS(Status)) && (Buffer != NULL)) {
        BlMmFreeHeap(Buffer);
    }

    ASSERT((!NT_SUCCESS(Status)) ||
           (OslIsUnicodeStringNullTerminated(Version) != FALSE));

    return Status;
}

USHORT
OslFwpEfiVersionBufferLength (
    __in USHORT VendorLength
    )

/*++

Routine Description:

    This routine calculates how large the EFI version string's buffer
    should be given the length of the FirmwareVendor string.

Arguments:

    VendorLength - The length of the FirmwareVendor string in bytes.

Return Value:

    How long the EFI version string's buffer should be.

--*/

{

    return VendorLength +
           EFI_VERSION_SEPARATOR_AND_REVISION_BUFFER_LENGTH;
}

NTSTATUS
OslCopyOfflineCrashdumpTable(
    __out POFFLINE_CRASHDUMP_CONFIGURATION_TABLE ConfigTable
    )

/*++

Routine Description:

    This routine retrieves the offline crashdump configuration table from
    firmware.

Arguments:

    ConfigTable - Supplies a pointer that will recieve the offline crashdump
        configuration table.

Return Value:

    NTSTATUS;

--*/

{

    ULONG ActualTableSize;
    ULONG BaseTableSize;
    NTSTATUS Status;
    PHYSICAL_ADDRESS TablePa;
    ULONG TableSize;
    POFFLINE_CRASHDUMP_CONFIGURATION_TABLE TempTable;

    TempTable = NULL;
    TableSize = sizeof(OFFLINE_CRASHDUMP_CONFIGURATION_TABLE);
    BaseTableSize = sizeof(OFFLINE_CRASHDUMP_CONFIGURATION_TABLE_V1);
    ActualTableSize = BaseTableSize;
    Status = BlFwGetSystemTable(FW_TABLE_OFFLINE_CRASHDUMP, &TablePa);
    if (!NT_SUCCESS(Status)) {
        goto CopyOfflineCrashdumpTableEnd;
    }

    //
    // First map with the base table.
    //

    Status = BlMmMapPhysicalAddress(&TempTable,
                                    TablePa,
                                    BaseTableSize,
                                    MEMORY_ATTRIBUTE_NONE);

    if ((!NT_SUCCESS(Status)) ||
        (TempTable->Version < OFFLINE_CRASHDUMP_VERSION_1) ||
        (TempTable->Version > OFFLINE_CRASHDUMP_VERSION_MAX)) {
        goto CopyOfflineCrashdumpTableEnd;
    }

    if (TempTable->Version > OFFLINE_CRASHDUMP_VERSION_1) {
        BlMmUnmapVirtualAddress(TempTable, BaseTableSize);
        TempTable = NULL;

        //
        // Remap with the full-size table.
        //

        Status = BlMmMapPhysicalAddress (&TempTable,
                                         TablePa,
                                         TableSize,
                                         MEMORY_ATTRIBUTE_NONE);
        ActualTableSize = TableSize;

        if (!NT_SUCCESS(Status)) {
            goto CopyOfflineCrashdumpTableEnd;
        }
    }

    //
    //Always return full-size table, with the unused members zero-ed out.
    //

    RtlZeroMemory(ConfigTable, TableSize);
    RtlCopyMemory(ConfigTable, TempTable, ActualTableSize);

CopyOfflineCrashdumpTableEnd:
    if (TempTable != NULL) {
        BlMmUnmapVirtualAddress(TempTable, ActualTableSize);
    }

    return Status;
}

BOOLEAN
OslIsRunningInSecureKernel (
    _Out_opt_ PBOOLEAN SecureKernelRunning
    )

/*++

Routine Description:

    This routine will determine if host environment is a secure kernel.

Arguments:

    SecureKernelRunning - TRUE if running, FALSE if not.

Return Value:

    BOOLEAN - TRUE if running in the secure side, FALSE if host is a normal kernel.

--*/

{

    if (ARGUMENT_PRESENT(SecureKernelRunning)) {
        *SecureKernelRunning = FALSE;
    }

    return FALSE;
}



