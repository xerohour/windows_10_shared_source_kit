/*++

Copyright (c) Microsoft Corporation

Module Name:

    si.c

Abstract:

    Implements OS loader-specific System Integrity functionality.

Environment:

    Boot

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "osloader.h"
#include <fvetpmtypes.h>
#include <strsafe.h>
#include <tpmapi.h>
#include <wbcl.h>
#include <sipaapi.h>
#include <sipap.h>
#include <blsi.h>

//
// ------------------------------------------------------------------- Defines
//

#define MAX_PATH 260

#define DRIVER_LOAD_POLICY_DEFAULT 0x1

#define REG_KEY_NAME_FILESYSTEM L"FileSystem"
#define REG_KEY_NAME_POLICIES L"Policies"

//
// The following names for registry values are kept the same as in ntfsinit.c
// so these will hopefully be discovered upon any changes to page file encryption
// in ntfs.
//

#define ENCRYPT_PAGINGFILE_GUARDED_HOSTS_VALUE_NAME L"NtfsEncryptPagingFileForGuardedHosts"

#define DISABLE_ENCRYPTION_VALUE_NAME L"NtfsDisableEncryption"

#define ENCRYPT_PAGINGFILE_VALUE_NAME L"NtfsEncryptPagingFile"

//
// ------------------------------------------------------------------- Extern
//

extern const PCWSTR OslControlString;

//
// -------------------------------------------------------Forward declarations
//

NTSTATUS
OslpIsPagefileEncryptionEnabled(
    _In_ ULONG SystemHiveHandle,
    _Out_ PBOOLEAN PagefileEncryptionEnabled
    );

//
// ------------------------------------------------------------------ Functions
//

NTSTATUS
OslMeasureEarlyLaunchHive (
    __in ULONG ElamHiveHandle
    )

/*++

Routine Description:

    This routine measures the early launch anti-malware registry hive. The
    Measurement is best effort. If the data is not measured the machine is
    not trustworthy.

Arguments:

    ElamHiveHandle - Supplies a handle to the early launch anti-malware
        registry hive.

Return Value:

    NT status code.

--*/

{
    NTSTATUS Status = STATUS_SUCCESS;
    PCM_KEY_NODE elamRootNode = NULL;
    PCM_KEY_NODE avKey = NULL;
    ULONG n = 0;
    PWSTR szName = NULL;
    ULONG cchName = 0;
    PBYTE pbConfig = NULL;
    ULONG cbConfig = 0;
    PBYTE pbPolicy = NULL;
    ULONG cbPolicy = 0;
    PBYTE pbMeasured = NULL;
    ULONG cbMeasured = 0;
    A_SHA_CTX shaContext = {0};
    BYTE digest[A_SHA_DIGEST_LEN] = {0};
    PTCGEventQueue ElamQueue = NULL;
    PTCGEventQueue Event = NULL;
    size_t cbAggregation = 0;
    PBYTE pbAggregation = NULL;

    //
    // Ensure that there is a consumer of the generated information
    //

    if(BlpSiEnvironmentReady() == FALSE) {
        goto OslMeasureEarlyLaunchHiveEnd;
    }

    // Get the root node from the ELAM hive
    if((elamRootNode = OslGetRootCell(ElamHiveHandle)) == NULL)
    {
        goto OslMeasureEarlyLaunchHiveEnd;
    }

    // Create the elam Queue
    if(SipapCreateQueue(0, 0, &ElamQueue) != S_OK)
    {
        goto OslMeasureEarlyLaunchHiveEnd;
    }

    // Iterate through the entries in the hive
    while(NT_SUCCESS(Status = OslGetSubkeyByNumber(ElamHiveHandle,
                                                   elamRootNode,
                                                   n,
                                                   &avKey,
                                                   &szName)))
    {
        // Obtain the key name
        if(szName == NULL)
        {
            // If this key has no name, we are going to ignore it
            goto OslMeasureEarlyLaunchHiveIterationEnd;
        }
        cchName = (ULONG)wcslen(szName);

        // Obtain the Config value
        OslGetBinaryValue(ElamHiveHandle,
                          avKey,
                          L"Config",
                          &pbConfig,
                          &cbConfig);

        // Obtain the Policy value
        OslGetBinaryValue(ElamHiveHandle,
                          avKey,
                          L"Policy",
                          &pbPolicy,
                          &cbPolicy);

        // Obtain the measured value
        OslGetBinaryValue(ElamHiveHandle,
                          avKey,
                          L"Measured",
                          &pbMeasured,
                          &cbMeasured);

        // Do we have data to record?
        if(((pbConfig == NULL) || (cbConfig == 0)) &&
           ((pbPolicy == NULL) || (cbPolicy == 0)) &&
           ((pbMeasured == NULL) || (cbMeasured == 0)))
        {
            // No data recorded
            goto OslMeasureEarlyLaunchHiveIterationEnd;
        }

        // Create a Queue that will group the key specific data
        if(SipapCreateQueue(0, 0, &Event) != S_OK)
        {
            goto OslMeasureEarlyLaunchHiveIterationEnd;
        }

        // Measure the collected data
        if(SipaQueueConfigEntryToQueue(SIPAEVENT_ELAM_KEYNAME,
                                       (UINT32)(cchName + 1) * sizeof(WCHAR),
                                       (PBYTE)szName,
                                       &Event) != S_OK)
        {
            goto OslMeasureEarlyLaunchHiveIterationEnd;
        }
        if((pbConfig != NULL) && (cbConfig != 0))
        {
            A_SHAInit(&shaContext);
            A_SHAUpdate(&shaContext, pbConfig, cbConfig);
            A_SHAFinal(&shaContext, digest);
            if(SipaQueueConfigEntryToQueue(SIPAEVENT_ELAM_CONFIGURATION,
                                           sizeof(digest),
                                           digest,
                                           &Event) != S_OK)
            {
                goto OslMeasureEarlyLaunchHiveIterationEnd;
            }
        }
        if((pbPolicy != NULL) && (cbPolicy != 0))
        {
            A_SHAInit(&shaContext);
            A_SHAUpdate(&shaContext, pbPolicy, cbPolicy);
            A_SHAFinal(&shaContext, digest);
            if(SipaQueueConfigEntryToQueue(SIPAEVENT_ELAM_POLICY,
                                           sizeof(digest),
                                           digest,
                                           &Event) != S_OK)
            {
                goto OslMeasureEarlyLaunchHiveIterationEnd;
            }
        }
        if((pbMeasured != NULL) && (cbMeasured != 0))
        {
            A_SHAInit(&shaContext);
            A_SHAUpdate(&shaContext, pbMeasured, cbMeasured);
            A_SHAFinal(&shaContext, digest);
            if(SipaQueueConfigEntryToQueue(SIPAEVENT_ELAM_MEASURED,
                                           sizeof(digest),
                                           digest,
                                           &Event) != S_OK)
            {
                goto OslMeasureEarlyLaunchHiveIterationEnd;
            }
        }

        // Close the bag for this key and append to the ELAM aggregation
        cbAggregation = Event->QueueLength;
        if((pbAggregation = BlMmAllocateHeap(cbAggregation)) == NULL)
        {
            goto OslMeasureEarlyLaunchHiveIterationEnd;
        }
        memcpy(pbAggregation, Event->Log, cbAggregation);
        if(SipaQueueConfigEntryToQueue(SIPAEVENT_ELAM_AGGREGATION,
                                       (UINT32)cbAggregation,
                                       pbAggregation,
                                       &ElamQueue) != S_OK)
        {
            goto OslMeasureEarlyLaunchHiveIterationEnd;
        }

OslMeasureEarlyLaunchHiveIterationEnd:

        // Release all resources acquired for this iteration
        if(szName != NULL)
        {
            BlMmFreeHeap(szName);
            szName = NULL;
        }
        cchName = 0;
        if(pbConfig != NULL)
        {
            BlMmFreeHeap(pbConfig);
            pbConfig = NULL;
        }
        cbConfig = 0;
        if(pbPolicy != NULL)
        {
            BlMmFreeHeap(pbPolicy);
            pbPolicy = NULL;
        }
        cbPolicy = 0;
        if(pbMeasured != NULL)
        {
            BlMmFreeHeap(pbMeasured);
            pbMeasured = NULL;
        }
        cbMeasured = 0;
        if(pbAggregation != NULL)
        {
            BlMmFreeHeap(pbAggregation);
            pbAggregation = NULL;
        }
        cbAggregation = 0;
        if(Event != NULL)
        {
            BlMmFreeHeap(Event);
            Event = NULL;
        }
        n++;
    }
    Status = STATUS_SUCCESS;

    // Close the bag for the whole ELAM aggregation and append to global queue
    if(n > 0)
    {
        // If there is something to report, wrap it up
        cbAggregation = ElamQueue->QueueLength;
        if((pbAggregation = (PBYTE)BlMmAllocateHeap(cbAggregation)) == NULL)
        {
            goto OslMeasureEarlyLaunchHiveEnd;
        }
        memcpy(pbAggregation, ElamQueue->Log, cbAggregation);
    }
    if(n == 1)
    {
        // A single entry is just being added to the queue
        if(SipapAppendEntry((UINT32)cbAggregation,
                            pbAggregation,
                            &SipaDataPointers.PendingEvents) != S_OK)
        {
            goto OslMeasureEarlyLaunchHiveEnd;
        }
    }
    else if(n > 1)
    {
        // Multiple entries are packaged in a container before adding them to the queue
        if(SipaQueueConfigEntryToQueue(SIPAEVENT_ELAM_AGGREGATION,
                                       (UINT32)cbAggregation,
                                       pbAggregation,
                                       &SipaDataPointers.PendingEvents) != S_OK)
        {
            goto OslMeasureEarlyLaunchHiveEnd;
        }
    }

OslMeasureEarlyLaunchHiveEnd:
    if(szName != NULL)
    {
        BlMmFreeHeap(szName);
    }
    if(pbConfig != NULL)
    {
        BlMmFreeHeap(pbConfig);
    }
    if(pbPolicy != NULL)
    {
        BlMmFreeHeap(pbPolicy);
    }
    if(pbMeasured != NULL)
    {
        BlMmFreeHeap(pbMeasured);
    }
    if(pbAggregation != NULL)
    {
        BlMmFreeHeap(pbAggregation);
    }
    if(Event != NULL)
    {
        BlMmFreeHeap(Event);
    }
    if(ElamQueue != NULL)
    {
        BlMmFreeHeap(ElamQueue);
    }
    return Status;
}

VOID
OslReportKernelLaunch (
    __in PBOOT_ENTRY_OPTION OsLoaderOptions,
    __in ULONG SystemHiveHandle
    )

/*++

Routine Description:

    This routine reports boot loader and kernel policy to the system integrity
    boot library.

    N.B. Changes to the boot options can not be made after this routine is
         called.

Arguments:

    OsLoaderOptions - Supplies the boot options for the Operating System.

    SystemHiveHandle - Supplies a handle to the system registry hive.

Return Value:

    None.

--*/

{

    size_t cchSystemRoot;
    ULONG DriverLoadPolicy;
    BOOLEAN FlightSigningEnabled;
    ULONGLONG HVLaunchType;
    BOOLEAN IntegrityChecksDisabled;
    BOOLEAN KernelDebuggingEnabled;
    ULONGLONG NxPolicy;
    PBOOT_ENVIRONMENT_DEVICE OSDevice;
    UINT32 OSDeviceCode;
    ULONGLONG PAEPolicy;
    BOOLEAN PagefileEncryptionEnabled;
    BOOLEAN SafeModeBoot;
    ULONGLONG SafeModeState;
    NTSTATUS Status;
    PWSTR SystemRoot = NULL;
    BOOLEAN TestSigningEnabled;
    ULONGLONG VsmLaunchType;
    BOOLEAN WinPE;
#if defined(NT_IUM)
    ULONGLONG HvciPolicy;
    BOOLEAN True;
    VSM_POLICY VsmPolicy;
#endif

    //
    // Make sure that the data being reported into the boot configuration log
    // is of the length expected in the log format.
    //

    C_ASSERT((sizeof(KernelDebuggingEnabled) == 1) &&
             (sizeof(WinPE) == 1) &&
             (sizeof(SafeModeBoot) == 1) &&
             (sizeof(PagefileEncryptionEnabled) == 1));

    //
    // Ensure that there is a consumer of the generated information
    //

    if(BlpSiEnvironmentReady() == FALSE) {
        return;
    }

    //
    // Extract system integrity relevant information from the boot option list.
    //

    Status = BlGetBootOptionBoolean(OsLoaderOptions,
                                    BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_ENABLED,
                                    &KernelDebuggingEnabled);

    if (Status == STATUS_NOT_FOUND) {
        KernelDebuggingEnabled = FALSE;

    } else if (!NT_SUCCESS(Status)) {
        BlSiEnterInsecureState(SiInsecureUnexpectedLibraryError,
                               "Error getting kernel debugging boot option");

        return;
    }

    Status = BlGetBootOptionInteger(OsLoaderOptions,
                                    BCDE_OSLOADER_TYPE_NX_POLICY,
                                    &NxPolicy);

    if (Status == STATUS_NOT_FOUND) {
        NxPolicy = NxPolicyOptIn;

    } else if (!NT_SUCCESS(Status)) {
        BlSiEnterInsecureState(SiInsecureUnexpectedLibraryError,
                               "Error getting NX policy boot option");

        return;

    }

    Status = BlGetBootOptionDevice(OsLoaderOptions,
                                    BCDE_OSLOADER_TYPE_OS_DEVICE,
                                    &OSDevice,
                                    NULL);

    if (Status == STATUS_NOT_FOUND) {
        RtlZeroMemory(&OSDevice, sizeof(OSDevice));

    } else if (!NT_SUCCESS(Status)) {
        BlSiEnterInsecureState(SiInsecureUnexpectedLibraryError,
                               "Error getting OS device boot option");

        return;

    }
    else
    {
        switch(OSDevice->DeviceType)
        {
            case DEVICE_BLOCK_IO:
                switch(OSDevice->u.BlockIo.BlockIoType)
                {
                    case BLOCK_IO_HARD_DISK:
                        OSDeviceCode = OSDEVICE_TYPE_BLOCKIO_HARDDISK;
                        break;
                    case BLOCK_IO_REMOVABLE_DISK:
                        OSDeviceCode = OSDEVICE_TYPE_BLOCKIO_REMOVABLEDISK;
                        break;
                    case BLOCK_IO_CDROM:
                        OSDeviceCode = OSDEVICE_TYPE_BLOCKIO_CDROM;
                        break;
                    case BLOCK_IO_RAMDISK:
                        OSDeviceCode = OSDEVICE_TYPE_BLOCKIO_RAMDISK;
                        break;
                    case BLOCK_IO_PARTITION:
                        OSDeviceCode = OSDEVICE_TYPE_BLOCKIO_PARTITION;
                        break;
                    case BLOCK_IO_FILE:
                        OSDeviceCode = OSDEVICE_TYPE_BLOCKIO_FILE;
                        break;
                    case BLOCK_IO_VIRTUAL_HARD_DISK:
                        OSDeviceCode = OSDEVICE_TYPE_BLOCKIO_VIRTUALHARDDISK;
                        break;
                    default:
                        OSDeviceCode = OSDEVICE_TYPE_UNKNOWN;
                        break;
                }
                break;
            case DEVICE_PARTITION:
                OSDeviceCode = OSDEVICE_TYPE_BLOCKIO_PARTITION;
                break;
            case DEVICE_SERIAL_PORT:
                OSDeviceCode = OSDEVICE_TYPE_SERIAL;
                break;
            case DEVICE_UDP:
                OSDeviceCode = OSDEVICE_TYPE_UDP;
                break;
            case DEVICE_PARTITION_EX:
                OSDeviceCode = OSDEVICE_TYPE_BLOCKIO_PARTITION;
                break;
            default:
                OSDeviceCode = OSDEVICE_TYPE_UNKNOWN;
                break;
        }
    }

    Status = BlGetBootOptionString(OsLoaderOptions,
                                    BCDE_OSLOADER_TYPE_SYSTEM_ROOT,
                                    &SystemRoot);

    if (Status == STATUS_NOT_FOUND) {
        SystemRoot = NULL;
        cchSystemRoot = 0;

    } else if (!NT_SUCCESS(Status)) {
        BlSiEnterInsecureState(SiInsecureUnexpectedLibraryError,
                               "Error getting system root boot option");
        if(SystemRoot != NULL) {
            BlMmFreeHeap(SystemRoot);
            SystemRoot = NULL;
        }

        return;

    } else if(StringCchLengthW(SystemRoot, MAX_PATH, &cchSystemRoot) != S_OK)
    {
        BlSiEnterInsecureState(SiInsecureUnexpectedLibraryError,
                               "Error processing system root boot option");
        if(SystemRoot != NULL) {
            BlMmFreeHeap(SystemRoot);
            SystemRoot = NULL;
        }

        return;

    }

    Status = BlGetBootOptionBoolean(OsLoaderOptions,
                                    BCDE_OSLOADER_TYPE_WINPE,
                                    &WinPE);

    if (Status == STATUS_NOT_FOUND) {
        WinPE = FALSE;

    } else if (!NT_SUCCESS(Status)) {
        BlSiEnterInsecureState(SiInsecureUnexpectedLibraryError,
                               "Error getting WinPE boot option");

        return;
    }

    Status = BlGetBootOptionInteger(OsLoaderOptions,
                                    BCDE_OSLOADER_TYPE_PAE_POLICY,
                                    &PAEPolicy);

    if (Status == STATUS_NOT_FOUND) {
        PAEPolicy = PaePolicyDefault;

    } else if (!NT_SUCCESS(Status)) {
        BlSiEnterInsecureState(SiInsecureUnexpectedLibraryError,
                               "Error getting PAE policy boot option");

        return;

    }

    Status = BlGetBootOptionInteger(OsLoaderOptions,
                                    BCDE_OSLOADER_TYPE_HYPERVISOR_LAUNCH_TYPE,
                                    &HVLaunchType);

    if (Status == STATUS_NOT_FOUND) {
        HVLaunchType = HypervisorLaunchOff;

    } else if (!NT_SUCCESS(Status)) {
        BlSiEnterInsecureState(SiInsecureUnexpectedLibraryError,
                               "Error getting HV launch type boot option");

        return;

    }

    Status = BlGetBootOptionInteger(OsLoaderOptions,
                                    BCDE_OSLOADER_TYPE_VSM_LAUNCH_TYPE,
                                    &VsmLaunchType);

    if (Status == STATUS_NOT_FOUND) {
        VsmLaunchType = VsmLaunchOff;

    } else if (!NT_SUCCESS(Status)) {
        BlSiEnterInsecureState(SiInsecureUnexpectedLibraryError,
                               "Error getting VSM launch type boot option");

        return;
    }

#if defined(NT_IUM)
    BlVsmGetSystemPolicy(&VsmPolicy);
#endif

    //
    // For system integrity purposes, treat all types of safe mode equivalently.
    //

    Status = BlGetBootOptionInteger(OsLoaderOptions,
                                    BCDE_OSLOADER_TYPE_SAFEBOOT,
                                    &SafeModeState);

    if (Status == STATUS_NOT_FOUND) {
        SafeModeBoot = FALSE;

    } else if (!NT_SUCCESS(Status)) {
        BlSiEnterInsecureState(SiInsecureUnexpectedLibraryError,
                               "Error getting safe mode boot option");

        return;

    } else {
        SafeModeBoot = TRUE;
    }

    Status = OslpIsPagefileEncryptionEnabled(SystemHiveHandle,
                                             &PagefileEncryptionEnabled);
    
    if (!NT_SUCCESS(Status)) {
        BlSiEnterInsecureState(SiInsecureUnexpectedLibraryError,
                               "Error getting pagefile encryption setting");

        return;
    }

    //
    // Log the values for the System Integrity-relevant boot policy.
    //

    BlSiPaRecordEvent(SIPAEVENT_OSKERNELDEBUG,
                      sizeof(KernelDebuggingEnabled),
                      &KernelDebuggingEnabled);

    BlSiPaRecordEvent(SIPAEVENT_OSDEVICE,
                      sizeof(OSDeviceCode),
                      &OSDeviceCode);

    BlSiPaRecordEvent(SIPAEVENT_SYSTEMROOT,
                      (UINT32)(cchSystemRoot + 1) * sizeof(WCHAR),
                      SystemRoot);

    if (SystemRoot != NULL) {
        BlMmFreeHeap(SystemRoot);
        SystemRoot = NULL;
    }

    BlSiPaRecordEvent(SIPAEVENT_DATAEXECUTIONPREVENTION,
                      sizeof(NxPolicy),
                      &NxPolicy);

    BlSiPaRecordEvent(SIPAEVENT_PHYSICALADDRESSEXTENSION,
                      sizeof(PAEPolicy),
                      &PAEPolicy);

    BlSiPaRecordEvent(SIPAEVENT_SAFEMODE,
                      sizeof(SafeModeBoot),
                      &SafeModeBoot);

    BlSiPaRecordEvent(SIPAEVENT_WINPE,
                      sizeof(WinPE),
                      &WinPE);

    BlSiPaRecordEvent(SIPAEVENT_HYPERVISOR_LAUNCH_TYPE,
                      sizeof(HVLaunchType),
                      &HVLaunchType);

    BlSiPaRecordEvent(SIPAEVENT_VSM_LAUNCH_TYPE,
                      sizeof(VsmLaunchType),
                      &VsmLaunchType);

#if NTDDI_VERSION >= NTDDI_WIN10_RS1 

    BlSiPaRecordEvent(SIPAEVENT_PAGEFILE_ENCRYPTION_ENABLED,
                      sizeof(PagefileEncryptionEnabled),
                      &PagefileEncryptionEnabled);

#endif

    //
    // Log the VSM policy state.
    //

#if defined(NT_IUM)
    if (VsmPolicy.AllOptions != 0) {

        True = TRUE;

        if (VsmPolicy.Options.Mandatory != 0) {
            BlSiPaRecordEvent(SIPAEVENT_VBS_MANDATORY_ENFORCEMENT,
                              sizeof(True),
                              &True);
        }

        if (VsmPolicy.Options.VsmRequired != 0) {
            BlSiPaRecordEvent(SIPAEVENT_VBS_VSM_REQUIRED,
                              sizeof(True),
                              &True);
        }

        if (VsmPolicy.Options.SecureBootRequired != 0) {
            BlSiPaRecordEvent(SIPAEVENT_VBS_SECUREBOOT_REQUIRED,
                              sizeof(True),
                              &True);
        }

        if (VsmPolicy.Options.IommuProtectionRequired != 0) {
            BlSiPaRecordEvent(SIPAEVENT_VBS_IOMMU_REQUIRED,
                              sizeof(True),
                              &True);
        }

        if (VsmPolicy.Options.MmioNxRequired != 0) {
            BlSiPaRecordEvent(SIPAEVENT_VBS_MMIO_NX_REQUIRED,
                              sizeof(True),
                              &True);
        }

        if (VsmPolicy.Options.StrongMsrFiltering != 0) {
            BlSiPaRecordEvent(SIPAEVENT_VBS_MSR_FILTERING_REQUIRED,
                              sizeof(True),
                              &True);
        }

        if (VsmPolicy.Options.Hvci != 0) {

            HvciPolicy = VsmPolicy.Options.Hvci;
            BlSiPaRecordEvent(SIPAEVENT_VBS_HVCI_POLICY,
                              sizeof(HvciPolicy),
                              &HvciPolicy);
        }

        if (VsmPolicy.Options.RequireMicrosoftSignedBootChain != FALSE) {
            BlSiPaRecordEvent(SIPAEVENT_VBS_MICROSOFT_BOOT_CHAIN_REQUIRED,
                              sizeof(True),
                              &True);
        }
    }
#endif

    //
    // Log driver load policy state.
    //

    Status = OslHiveGetDriverLoadPolicy(SystemHiveHandle, &DriverLoadPolicy);
    if (!NT_SUCCESS(Status)) {
        DriverLoadPolicy = DRIVER_LOAD_POLICY_DEFAULT;
    }

    BlSiPaRecordEvent(SIPAEVENT_DRIVER_LOAD_POLICY,
                      sizeof(DriverLoadPolicy),
                      &DriverLoadPolicy);

    //
    // If code integrity is disabled in the kernel, report that a non-validated
    // image has been loaded.  This isn't accurate, but from a security
    // perspective, disabling integrity checks is equivalent to loading an
    // unchecked image.
    //

    //
    // Also check whether test signing is enabled. Test signing is initially
    // checked when an application is loaded.  This check handles the F10
    // scenario where the boot option was added during the OS loader's
    // execution.
    //

    BlImgQueryCodeIntegrityBootOptions(BlGetApplicationEntry(),
                                       &IntegrityChecksDisabled,
                                       &TestSigningEnabled,
                                       &FlightSigningEnabled);

    if (IntegrityChecksDisabled != FALSE) {
        BlEnNotifyEvent(EVENT_NVPE_IMAGE_LOADED, NULL);
    }

    if (TestSigningEnabled != FALSE) {
        BlEnNotifyEvent(EVENT_TEST_SIGNING_ENABLED, NULL);
    }

    if (FlightSigningEnabled != FALSE) {
        BlEnNotifyEvent(EVENT_FLIGHT_SIGNING_ENABLED, NULL);
    }

    //
    // Flush any pending measurements and complete the aggregation event for winload.
    // Keep the SI library open though as we may need to measure VSM IDK.
    //
    BlSiFlushCurrentMeasurements();
    return;
}

VOID
OslCloseSipaReporting ()

/*++

Routine Description:

    This routine closes the SIPA measuring / reporting sub-system.
    No measured boot actions are possible after this method has been called.

Arguments:

    None.

Return Value:

    None.

--*/

{
    BlSiCloseEnvironment();
}

BOOLEAN
OslSwitchTpmPcrBanks (
    __in PBOOT_ENVIRONMENT_DEVICE BootDevice
    )

/*++

Routine Description:

    This routine tries to switch the TPM PCR banks to the OS preferred
    banks. Before doing so, it checks if Bitlocker is active.

Arguments:

    BootDevice - the boot device for the OS.

Return Value:

    FALSE - if no further action is required.

    TRUE - if a reboot is required.

--*/

{
    NTSTATUS Status;
    ULONG FveStaticFlags;
    ULONG FvePcrBitmap;

    Status = STATUS_SUCCESS;

    if (BootDevice != NULL) {

        Status = BlFveQueryStaticDeviceStatus(BootDevice,
                                              &FveStaticFlags,
                                              &FvePcrBitmap);

        if (NT_SUCCESS(Status)) {

            //
            // Check if Bitlocker is on, TPM protector is enabled, and is not suspended.
            // If so, don't touch PCR banks.
            //

            if ((BlFveBitlockerEnabled(FveStaticFlags) == TRUE) &&
                (BlFveBitlockerTpmProtected(FveStaticFlags) == TRUE) &&
                (BlFveBitlockerSuspended(FveStaticFlags) == FALSE)) {

                return FALSE;
            }
        }
    }

    //
    // Change the active PCR bank.
    //
    // At this point Bitlocker is off or suspended.
    //

    Status = BlTpmSwitchPcrBanks();

    if (Status == STATUS_RESTART_BOOT_APPLICATION) {

        return TRUE;
    }

    //
    // ignore all other return values
    //

    return FALSE;
}

NTSTATUS
OslpIsPagefileEncryptionEnabled(
    _In_ ULONG SystemHiveHandle,
    _Out_ PBOOLEAN PagefileEncryptionEnabled
    )

/*++

Routine Description:

    This helper routine determines if page file encryption is enabled
    by checking the same set of registry key triggers as done by the ntfs
    driver (see ntfsinit.c::DriverEntry).

    This routine must be updated if the above conditon in the ntfs driver 
    is ever changed.

Arguments:

    SystemHiveHandle - Supplies a handle to the system registry hive.

    PagefileEncryptionEnabled - Supplies a pointer to a variable to receive 
        a flag indicating whether page file encryption is enabled.

Return Value:

    NTSTATUS code.

--*/

{
    NTSTATUS Status;
    PCM_KEY_NODE CurrentControlSetCell;
    PCM_KEY_NODE ControlCell;
    PCM_KEY_NODE FileSystemCell;
    PCM_KEY_NODE PoliciesCell;
    DWORD RegValue;

    Status = OslGetSubkey(SystemHiveHandle,
                          NULL,
                          NULL,
                          &CurrentControlSetCell);

    if (!NT_SUCCESS(Status)) {
        goto IsPagefileEncryptionEnabledEnd;
    }

    Status = OslGetSubkey(SystemHiveHandle,
                          CurrentControlSetCell,
                          OslControlString,
                          &ControlCell);

    if (!NT_SUCCESS(Status)) {
        goto IsPagefileEncryptionEnabledEnd;
    }

    //
    // Condition #1.
    //
    // If COMPATIBILITY_MODE_KEY_NAME::ENCRYPT_PAGINGFILE_GUARDED_HOSTS_VALUE_NAME(DWORD) 
    // is present and set to 1 then page file encryption is enabled.
    //
    // The registry path above is:
    // \\CurrentControlSet\\Control\\FileSystem::NtfsEncryptPagingFileForGuardedHosts
    //

    Status = OslGetSubkey(SystemHiveHandle,
                          ControlCell,
                          REG_KEY_NAME_FILESYSTEM,
                          &FileSystemCell);

    if (!NT_SUCCESS(Status)) {
        goto IsPagefileEncryptionEnabledEnd;
    }

    Status = OslGetDWordValue(SystemHiveHandle,
                              FileSystemCell,
                              ENCRYPT_PAGINGFILE_GUARDED_HOSTS_VALUE_NAME,
                              &RegValue);

    if ((Status != STATUS_NOT_FOUND) && 
        (Status != STATUS_OBJECT_TYPE_MISMATCH)) {

        if (!NT_SUCCESS(Status)) {
            goto IsPagefileEncryptionEnabledEnd;
        }

        if (RegValue == 1) {
            *(PagefileEncryptionEnabled) = TRUE;

            goto IsPagefileEncryptionEnabledEnd;
        }
    }

    //
    // Condition #2. 
    //
    // If either GROUP_POLICY_KEY_NAME::DISABLE_ENCRYPTION_VALUE_NAME or
    // COMPATIBILITY_MODE_KEY_NAME::DISABLE_ENCRYPTION_VALUE_NAME exist and 
    // are set to (DWORD)1 then page file encryption is disabled for good.
    //
    // The registry path above is:
    // \\CurrentControlSet\\Policies::NtfsDisableEncryption and
    // \\CurrentControlSet\\Control\\FileSystem::NtfsDisableEncryption
    //

    Status = OslGetSubkey(SystemHiveHandle,
                          CurrentControlSetCell,
                          REG_KEY_NAME_POLICIES,
                          &PoliciesCell);

    if (!NT_SUCCESS(Status)) {
        goto IsPagefileEncryptionEnabledEnd;
    }

    Status = OslGetDWordValue(SystemHiveHandle,
                              PoliciesCell,
                              DISABLE_ENCRYPTION_VALUE_NAME,
                              &RegValue);

    if ((Status != STATUS_NOT_FOUND) && 
        (Status != STATUS_OBJECT_TYPE_MISMATCH)) {

        if (!NT_SUCCESS(Status)) {
            goto IsPagefileEncryptionEnabledEnd;
        }

        if (RegValue == 1) {
            *(PagefileEncryptionEnabled) = FALSE;

            goto IsPagefileEncryptionEnabledEnd;
        }
    }

    Status = OslGetDWordValue(SystemHiveHandle,
                              FileSystemCell,
                              DISABLE_ENCRYPTION_VALUE_NAME,
                              &RegValue);

    if ((Status != STATUS_NOT_FOUND) && 
        (Status != STATUS_OBJECT_TYPE_MISMATCH)) {

        if (!NT_SUCCESS(Status)) {
            goto IsPagefileEncryptionEnabledEnd;
        }

        if (RegValue == 1) {
            *(PagefileEncryptionEnabled) = FALSE;

            goto IsPagefileEncryptionEnabledEnd;
        }
    }

    //
    // Condition #3 (requires Condition #2 above to be false). 
    //
    // If either GROUP_POLICY_KEY_NAME::ENCRYPT_PAGINGFILE_VALUE_NAME or
    // COMPATIBILITY_MODE_KEY_NAME::ENCRYPT_PAGINGFILE_VALUE_NAME exist and 
    // are set to (DWORD)1 then page file encryption is enabled.
    //
    // The registry path above is:
    // \\CurrentControlSet\\Policies::NtfsEncryptPagingFile and
    // \\CurrentControlSet\\Control\\FileSystem::NtfsEncryptPagingFile
    //

    Status = OslGetDWordValue(SystemHiveHandle,
                              PoliciesCell,
                              ENCRYPT_PAGINGFILE_VALUE_NAME,
                              &RegValue);

    if ((Status != STATUS_NOT_FOUND) && 
        (Status != STATUS_OBJECT_TYPE_MISMATCH)) {

        if (!NT_SUCCESS(Status)) {
            goto IsPagefileEncryptionEnabledEnd;
        }

        if (RegValue == 1) {
            *(PagefileEncryptionEnabled) = TRUE;

            goto IsPagefileEncryptionEnabledEnd;
        }
    }

    Status = OslGetDWordValue(SystemHiveHandle,
                              FileSystemCell,
                              ENCRYPT_PAGINGFILE_VALUE_NAME,
                              &RegValue);

    if ((Status != STATUS_NOT_FOUND) && 
        (Status != STATUS_OBJECT_TYPE_MISMATCH)) {

        if (!NT_SUCCESS(Status)) {
            goto IsPagefileEncryptionEnabledEnd;
        }

        if (RegValue == 1) {
            *(PagefileEncryptionEnabled) = TRUE;

            goto IsPagefileEncryptionEnabledEnd;
        }
    }

    Status = STATUS_SUCCESS;
    *(PagefileEncryptionEnabled) = FALSE;

IsPagefileEncryptionEnabledEnd:
    return Status;
}

