/*++

Copyright (c) Microsoft Corporation

Module Name:

    debug.c

Abstract:

    OS Loader debugger support.

Environment:

    Boot Environment.

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "osloader.h"

//
// -------------------------------------------------------------------- Globals
//

//
// Kernel debugger device and type.
//

PDEBUG_DEVICE_DESCRIPTOR KdDebugDevice;

//
// ----------------------------------------------------------------- Prototypes
//

NTSTATUS
OslpGetKdTransportPath (
    _Out_writes_(PathLength) PWCHAR KdTransportPath,
    _In_ ULONG PathLength
    );

NTSTATUS
OslpLoadTransports (
    _In_ PLOADER_PARAMETER_BLOCK LoaderBlock,
    _At_((SystemFilePath->Buffer), _Inout_z_) PUNICODE_STRING SystemFilePath,
    _In_ DEVICE_ID OsDeviceId,
    _In_ PWSTR KdTransportPath,
    _In_opt_ PWSTR KdExtensionPath,
    _Outptr_opt_ PKLDR_DATA_TABLE_ENTRY *KdTransportEntry,
    _Outptr_opt_result_maybenull_ PKLDR_DATA_TABLE_ENTRY *KdExtensionEntry
    );

//
// ------------------------------------------------------------------ Functions
//

NTSTATUS
OslKdInitialize (
    _In_ PLOADER_PARAMETER_BLOCK LoaderBlock,
    _At_((PWSTR)(SystemFilePath->Buffer), _Inout_z_) PUNICODE_STRING SystemFilePath,
    _In_ DEVICE_ID OsDeviceId,
    _Outptr_opt_ PKLDR_DATA_TABLE_ENTRY *KdTransportEntry,
    _Outptr_opt_result_maybenull_ PKLDR_DATA_TABLE_ENTRY *KdExtensionEntry
    )

/*++

Routine Description:

    This routine finds and configures the kernel debugging device.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block.

    SystemFilePath - The file path to the Operating System32 directory.

    OsDeviceId - Handle to the device containing the Operating System.

    KdTransportEntry - Supplies a pointer that will be set to the entry of the
        loaded transport image on success.

    KdExtensionEntry - Supplies a pointer that will be set to the entry of the
        loaded extension entry if applicable on success.

Return Value:

    NT status code.

--*/

{

    PWSTR BusParameters;
    BOOLEAN DebuggerEnabled;
    BCDE_DEBUGGER_TYPE DebuggerType;
    PVOID DllBase;
    BOOLEAN EventLoggingEnabled;
    ULONG Flags;
    ULONGLONG IntegerOption;
    PWCHAR KdExtensionPath;
    PWCHAR KdTransportPath;
    NTSTATUS Status;

    BusParameters = NULL;
    DebuggerEnabled = FALSE;    
    KdExtensionPath = NULL;
    KdTransportPath = NULL;
    DebuggerType = DebuggerInvalid;
    EventLoggingEnabled = FALSE;

    //
    // First, determine if the debugger is enabled.
    //

    Status = BlGetApplicationOptionBoolean(BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_ENABLED,
                                      &DebuggerEnabled);

    if (!NT_SUCCESS(Status)) {
        DebuggerEnabled = FALSE;
    }

    Status = BlGetApplicationOptionBoolean(BCDE_OSLOADER_TYPE_EVENT_LOGGING_ENABLED,
                                           &EventLoggingEnabled);

    if (!NT_SUCCESS(Status)) {
        EventLoggingEnabled = FALSE;
    }

    //
    // Even if the debugger is not enabled, a stub transport should be loaded.
    //

    KdTransportPath = BlMmAllocateHeap(MAX_MODULE_NAME_LENGTH * sizeof(WCHAR));
    if (KdTransportPath == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(KdTransportPath, MAX_MODULE_NAME_LENGTH * sizeof (WCHAR));
    wcscpy_s(KdTransportPath, MAX_MODULE_NAME_LENGTH, KD_DEFAULT_NAME);
    if ((DebuggerEnabled == FALSE) && (EventLoggingEnabled == FALSE)) {
        Status = STATUS_SUCCESS;
        goto KdInitializeEnd;
    }

    //
    // Get the debugger type from the BCD store. Check only the kernel debugger
    // settings.  If set, this type will be used with overridden non standard
    // transports.
    //

    Status = BlGetApplicationOptionInteger(
                    BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_TYPE,
                    &IntegerOption);

    if (NT_SUCCESS(Status)) {
        DebuggerType = (BCDE_DEBUGGER_TYPE)IntegerOption;
    }

    //
    // If the debugger is enabled, get the real transport path.
    //

    Status = OslpGetKdTransportPath(KdTransportPath, MAX_MODULE_NAME_LENGTH);
    if (!NT_SUCCESS(Status)) {
        goto KdInitializeEnd;
    }

    if (wcscmp(KdTransportPath, KD_SERIAL_NAME) == 0) {
        DebuggerType = DebuggerSerial;

    } else if (wcscmp(KdTransportPath, KD_HV_COM_NAME) == 0) {
        DebuggerType = DebuggerSerial;

    } else if (wcscmp(KdTransportPath, KD_1394_NAME) == 0) {
        DebuggerType = Debugger1394;

    } else if (wcscmp(KdTransportPath, KD_HV_1394_NAME) == 0) {
        DebuggerType = Debugger1394;

    } else if (wcscmp(KdTransportPath, KD_USB_NAME) == 0) {
        DebuggerType = DebuggerUsb;

    } else if (wcscmp(KdTransportPath, KD_NET_NAME) == 0) {
        DebuggerType = DebuggerNet;

    } else if (wcscmp(KdTransportPath, KD_LOCAL_NAME) == 0) {
        DebuggerType = DebuggerLocal;
    }

    //
    // Get the bus parameters, failure doesn't matter. First try to get the
    // kernel bus parameters if any, otherwise fallback to the library settings.
    //

    Status = BlGetApplicationOptionString(
                BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_BUS_PARAMETERS,
                &BusParameters);

    if (!NT_SUCCESS(Status)) {
        Status = BlGetApplicationOptionString(
                     BCDE_LIBRARY_TYPE_DEBUGGER_BUS_PARAMETERS,
                     &BusParameters);
        
        if (!NT_SUCCESS(Status)) {
            BusParameters = NULL;
        }

        Status = STATUS_SUCCESS;
    }

    //
    // Initialize the global kernel debug descriptor.
    //

    NT_ASSERT(KdDebugDevice == NULL);

    Status = BlBdInitializeDeviceDescriptor(&KdDebugDevice,
                                            DebuggerType,
                                            BusParameters);

    if (!NT_SUCCESS(Status)) {
        goto KdInitializeEnd;
    }

    //
    // Setup this device,
    //

    Flags = (BD_FLAGS_FORCE_CONFIGURATION |
             BD_FLAGS_KERNEL_VISIBLE |
             BD_FLAGS_MAP_BARS);

    Status = BlBdSetupDebuggingDevice(KdDebugDevice, Flags);
    if (!NT_SUCCESS(Status)) {
        goto KdInitializeEnd;
    }

    LoaderBlock->Extension->KdDebugDevice = KdDebugDevice;

    //
    // Get the extension path if any.
    //

    Status = BlBdGetExtensionName(KdDebugDevice, 
                                  DebuggerType, 
                                  &KdExtensionPath);

    if (!NT_SUCCESS(Status)) {
        goto KdInitializeEnd;
    }

KdInitializeEnd:

    //
    // Attempt to load the transport and see if that succeeds.
    //

    if (NT_SUCCESS(Status)) {
        Status = OslpLoadTransports(LoaderBlock,
                                    SystemFilePath,
                                    OsDeviceId,
                                    KdTransportPath,
                                    KdExtensionPath,
                                    KdTransportEntry,
                                    KdExtensionEntry);

        //
        // Initialize extension and allocate scratch memory for the device.
        //

        if (NT_SUCCESS(Status) && (KdDebugDevice != NULL)) {
            if (*KdExtensionEntry != NULL) {
                DllBase = (*KdExtensionEntry)->DllBase;
                
                NT_ASSERT(DllBase != NULL);

                Status = BlBdInitializeTransportExtension(KdDebugDevice,
                                                          DllBase,
                                                          NULL,
                                                          FALSE);
            }

            if (NT_SUCCESS(Status)) {
                Flags = (BD_FLAGS_ALLOCATE_SCRATCH_MEMORY |
                         BD_FLAGS_KERNEL_VISIBLE);

                Status = BlBdSetupDebuggingDevice(KdDebugDevice, Flags);
            }

            if (!NT_SUCCESS(Status)) {
                if (*KdTransportEntry != NULL) {
                    BlLdrUnloadImage(*KdTransportEntry);
                    *KdTransportEntry = NULL;
                }
                
                if (*KdExtensionEntry!= NULL) {
                    BlLdrUnloadImage(*KdExtensionEntry);
                    *KdExtensionEntry = NULL;
                }
            }
        }
    }

    //
    // Release resources on failure and reset the transport to the default.
    //

    if (!NT_SUCCESS(Status)) {
        RtlZeroMemory(KdTransportPath, MAX_MODULE_NAME_LENGTH * sizeof (WCHAR));
        wcscpy_s(KdTransportPath, MAX_MODULE_NAME_LENGTH, KD_DEFAULT_NAME);
        if (KdDebugDevice != NULL) {
            BlBdReleaseDebuggingDevice(KdDebugDevice);
        }

        //
        // Always load atleast the stub transport.
        //

        Status = OslpLoadTransports(LoaderBlock,
                                    SystemFilePath,
                                    OsDeviceId,
                                    KdTransportPath,
                                    NULL,
                                    KdTransportEntry,
                                    KdExtensionEntry);
    }

    if (KdExtensionPath != NULL) {
        BlMmFreeHeap(KdExtensionPath);
    }

    if (BusParameters != NULL) {
        BlMmFreeHeap(BusParameters);
    }

    if (KdTransportPath != NULL) {
        BlMmFreeHeap(KdTransportPath);
    }

    //
    // This function should not fail to avoid failing boot unless loading
    // kdstub itself fails since that is required to boot.
    //

    return Status;
}

VOID
OslPersistBootDebugger (
    _Inout_ PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine will modify the kernel data structures necessary to persist
    the boot debugger through kernel initialization.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block.

Return Value:

    NT status code.

--*/

{

    PKLDR_DATA_TABLE_ENTRY DataTableEntry;
    PLIST_ENTRY NextEntry;

    //
    // Set the flag in the loader block so the kernel knows the boot debugger
    // is still active.
    //

    LoaderBlock->Extension->BootDebuggerActive = 1;

    //
    // Install the boot debugger's trap handlers in the kernel's initial IDT.
    //

    OslArchDebugPatchKernelInterrupts();

    //
    // Walk the list of loaded modules, and load symbols for all modules that
    // are marked to not be moved by the memory manager.  This includes the
    // kernel, the hal, the kd transport, and all hal and kd transport
    // extensibility modules.
    //

    for (NextEntry = LoaderBlock->LoadOrderListHead.Flink;
         NextEntry->Flink != LoaderBlock->LoadOrderListHead.Flink;
         NextEntry = NextEntry->Flink) {

        DataTableEntry = CONTAINING_RECORD(NextEntry,
                                           KLDR_DATA_TABLE_ENTRY,
                                           InLoadOrderLinks);

        if ((DataTableEntry->Flags & LDRP_DONT_RELOCATE) == FALSE) {
            continue;
        }

        BlBdLoadImageSymbols(&(DataTableEntry->BaseDllName),
                             DataTableEntry->DllBase);
    }

    return;
}

NTSTATUS
OslpGetKdTransportPath (
    _Out_writes_(PathLength) PWCHAR KdTransportPath,
    _In_ ULONG PathLength
    )

/*++

Routine Description:

    This routine returns the string paths to the debugger transport and
    extension.

Arguments:

    KdTransportPath - Supplies an allocated buffer that will be populated with
        the transport path.

    PathLength - Supplies the maximum length of the path in WCHARs.

Return Value:

    NT status code.

--*/

{

    BCDE_DEBUGGER_TYPE DebuggerType;
    BOOLEAN HvOverride;
    PWCHAR HvPath;
    ULONGLONG IntegerOption;
    NTSTATUS Status;
    PWCHAR UserTransportPath;

    if (PathLength < MAX_MODULE_NAME_LENGTH) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Here's the logic for this. If the user specified a transport path,
    // then just use it. Otherwise, if only the transport type was specified
    // then attempt to figure out the best transport path based on the type
    // and the HV overrides.
    //

    Status = BlGetApplicationOptionString(BCDE_OSLOADER_TYPE_DBG_TRANSPORT_PATH,
                                          &UserTransportPath);

    if (NT_SUCCESS(Status)) {
        wcscpy_s(KdTransportPath, PathLength, UserTransportPath);
        BlMmFreeHeap(UserTransportPath);
        goto GetTransportPathsEnd;
    }

    //
    // If there is no valid transport path then attempt to set one based on the
    // debugger type in the BCD store. First check the kernel debugger settings
    // then fall back to the loader settings.
    //

    Status = BlGetApplicationOptionInteger(
                    BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_TYPE,
                    &IntegerOption);

    if (!NT_SUCCESS(Status)) {
        Status = BlGetApplicationOptionInteger(BCDE_LIBRARY_TYPE_DEBUGGER_TYPE,
                                               &IntegerOption);
    }

    if (!NT_SUCCESS(Status)) {
        goto GetTransportPathsEnd;
    }

    DebuggerType = (BCDE_DEBUGGER_TYPE)IntegerOption;
    switch(DebuggerType) {
        case DebuggerSerial:
            wcscpy_s(KdTransportPath, PathLength, KD_SERIAL_NAME);
            break;

        case Debugger1394:
            wcscpy_s(KdTransportPath, PathLength, KD_1394_NAME);
            break;

        case DebuggerUsb:
            wcscpy_s(KdTransportPath, PathLength, KD_USB_NAME);
            break;

        case DebuggerNet:
            wcscpy_s(KdTransportPath, PathLength, KD_NET_NAME);
            break;

        case DebuggerLocal:
            wcscpy_s(KdTransportPath, PathLength, KD_LOCAL_NAME);
            break;

        default:
            Status = STATUS_INVALID_PARAMETER;
            goto GetTransportPathsEnd;
    }

    //
    // Correlate against the Hypervisor debug type to determine if one of the
    // hv dlls should be used instead.
    //

    HvOverride = OslGetHypervisorDebuggerOverrides(DebuggerType, &HvPath);
    if (HvOverride != FALSE) {
        wcscpy_s(KdTransportPath, PathLength, HvPath);
    }

GetTransportPathsEnd:
    return Status;
}

NTSTATUS
OslpLoadTransports (
    _In_ PLOADER_PARAMETER_BLOCK LoaderBlock,
    _At_((SystemFilePath->Buffer), _Inout_z_) PUNICODE_STRING SystemFilePath,
    _In_ DEVICE_ID OsDeviceId,
    _In_ PWSTR KdTransportPath,
    _In_opt_ PWSTR KdExtensionPath,
    _Outptr_opt_ PKLDR_DATA_TABLE_ENTRY *KdTransportEntry,
    _Outptr_opt_result_maybenull_ PKLDR_DATA_TABLE_ENTRY *KdExtensionEntry
    )

/*++

Routine Description:

    This routine loads the passed in transport and extension paths and
    randomizes their locations.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block.

    SystemFilePath - The file path to the Operating System32 directory.

    OsDeviceId - Handle to the device containing the Operating System.

    KdTransportPath - Supplies an allocated buffer that will be populated with
        the transport path.

    KdExtensionPath - Supplies an allocated buffer that will be populated with
        the extension path.

    KdTransportEntry - Supplies a pointer that will be set to the entry of the
        loaded transport image on success.

    KdExtensionEntry - Supplies a pointer that will be set to the entry of the
        loaded extension entry if applicable on success.

Return Value:

    NT status code.

--*/

{

    NTSTATUS Status;
    BOOLEAN Success;
    USHORT SystemPathOffset;

    //
    // Load the extension first, if it's available.
    //

    if (ARGUMENT_PRESENT(KdExtensionEntry)) {
        *KdExtensionEntry = NULL;
    }

    SystemPathOffset = SystemFilePath->Length;
    if (ARGUMENT_PRESENT(KdExtensionPath) != FALSE) {
        Success = BlAppendUnicodeToString(SystemFilePath, KdExtensionPath);
        if (Success == FALSE) {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto LoadTransportsEnd;
        }

        Status = OslLoadImage(OsDeviceId,
                              OSL_MEMORY_TYPE_SYSTEM_CODE,
                              SystemFilePath->Buffer,
                              KD_EXTENSION_FRIENDLY_NAME,
                              NULL,
                              NULL,
                              0,
                              &LoaderBlock->LoadOrderListHead,
                              KdExtensionEntry,
                              0,
                              0,
                              IMAGE_FLAGS_NONE,
                              OSL_IMAGE_FLAGS_LOAD_IMPORTS,
                              NULL);

            SystemFilePath->Buffer[SystemPathOffset / sizeof(WCHAR)] = L'\0';
            SystemFilePath->Length = SystemPathOffset;

        //
        // If the extension image doesn't exist, fallback to the extension stub
        // module.
        //

        if (Status == STATUS_NO_SUCH_FILE) {
            Success = BlAppendUnicodeToString(SystemFilePath,
                                              KD_EXTENSION_DEFAULT_NAME);

            if (Success == FALSE) {
                Status = STATUS_INSUFFICIENT_RESOURCES;
                goto LoadTransportsEnd;
            }

            Status = OslLoadImage(OsDeviceId,
                                  OSL_MEMORY_TYPE_SYSTEM_CODE,
                                  SystemFilePath->Buffer,
                                  KD_EXTENSION_FRIENDLY_NAME,
                                  NULL,
                                  NULL,
                                  0,
                                  &LoaderBlock->LoadOrderListHead,
                                  KdExtensionEntry,
                                  0,
                                  0,
                                  IMAGE_FLAGS_NONE,
                                  OSL_IMAGE_FLAGS_LOAD_IMPORTS,
                                  NULL);

                SystemFilePath->Buffer[SystemPathOffset / sizeof(WCHAR)] = L'\0';
                SystemFilePath->Length = SystemPathOffset;
        }

        if (!NT_SUCCESS(Status)) {
            goto LoadTransportsEnd;
        }

        if (ARGUMENT_PRESENT(KdExtensionEntry)) {
            (*KdExtensionEntry)->Flags |= LDRP_DONT_RELOCATE;
        }
    }

    Success = BlAppendUnicodeToString(SystemFilePath, KdTransportPath);
    if (Success == FALSE) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto LoadTransportsEnd;
    }

    Status = OslLoadImage(OsDeviceId,
                          OSL_MEMORY_TYPE_SYSTEM_CODE,
                          SystemFilePath->Buffer,
                          KD_FRIENDLY_NAME,
                          NULL,
                          NULL,
                          0,
                          &LoaderBlock->LoadOrderListHead,
                          KdTransportEntry,
                          0,
                          0,
                          IMAGE_FLAGS_NONE,
                          OSL_IMAGE_FLAGS_LOAD_IMPORTS,
                          NULL);

        SystemFilePath->Buffer[SystemPathOffset / sizeof(WCHAR)] = L'\0';
        SystemFilePath->Length = SystemPathOffset;
    if (!NT_SUCCESS(Status)) {
        goto LoadTransportsEnd;
    }

    if (ARGUMENT_PRESENT(KdTransportEntry)) {
        (*KdTransportEntry)->Flags |= LDRP_DONT_RELOCATE;
    }

LoadTransportsEnd:
    return Status;
}

