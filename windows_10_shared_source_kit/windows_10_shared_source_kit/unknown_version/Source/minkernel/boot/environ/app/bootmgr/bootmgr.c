/*++

Copyright (c) Microsoft Corporation

Module Name:

    bootmgr.c

Abstract:

    Implementation of the Windows Boot Manager.

Environment:

    Boot Environment.

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "bootmgr.h"
#include "bltracelogging.h"
#include "systemevents.h"
#include "UnlockIdLib.h"

//
// -------------------------------------------------------------------- Defines
//

#define INVALID_BOOT_INDEX 0xFFFFFFFF

#define IS_TRUSTED_WIM_BOOT_LAUNCH_REASON(_LaunchReason) \
    ((IS_AUTO_RECOVERY_LAUNCH_REASON(_LaunchReason)) || \
     (_LaunchReason == BSD_APP_LAUNCH_REASON_MANUAL) || \
     (_LaunchReason == BSD_APP_LAUNCH_REASON_UPDATE))

#define IS_AUTO_RECOVERY_LAUNCH_REASON(_LaunchReason) \
    ((_LaunchReason == BSD_APP_LAUNCH_REASON_AUTO_FAILOVER) || \
     (_LaunchReason == BSD_APP_LAUNCH_REASON_BAD_BOOT) || \
     (_LaunchReason == BSD_APP_LAUNCH_REASON_BAD_SHUTDOWN))


//
// -------------------------------------------------------------------- Globals
//

//
// Default for a Windows clean install is to not display the boot menu.
//

BOOLEAN BmpDisplayBootMenu = FALSE;
BOOLEAN BmDisplayStateCached;

GUID BmApplicationIdentifier;

//
// Define a GUID that can be set as a debugger override for the boot entry to
// select.  This is designed to be used through the debugger and is not set in
// code.
//

volatile BOOLEAN BmBootEntryOverridePresent;
volatile GUID BmBootEntryOverride;

#if !defined(_X86_)

#pragma comment(linker, "/include:BmBootEntryOverridePresent")
#pragma comment(linker, "/include:BmBootEntryOverride")

#else

#pragma comment(linker, "/include:_BmBootEntryOverridePresent")
#pragma comment(linker, "/include:_BmBootEntryOverride")

#endif

//
// Do not perform tracelogging on PCAT systems as it bloats the size of bootmgr
//

#if !defined(PCAT)

TRACELOGGING_DECLARE_PROVIDER(BmBootProvider);

//
// Defines the registration entry and associated metadata for the boot telemetry
// provider.
//

// {23b76a75-ce4f-56ef-f903-c3a2d6ae3f6b}
TRACELOGGING_DEFINE_PROVIDER(
    BmBootProvider,
    BOOT_PROVIDER_NAME,
    (0x23b76a75, 0xce4f, 0x56ef, 0xf9, 0x03, 0xc3, 0xa2, 0xd6, 0xae, 0x3f, 0x6b),
    TraceLoggingOptionMicrosoftTelemetry());

#endif

//
// --------------------------------------------------------------------- Locals
//

//
// Global keeps track of when the boot manager started executing. This is
// necessary to figure out the time spent in the boot manager and log it as an
// ETW event.
//

LARGE_INTEGER ApplicationStartTime;
LARGE_INTEGER BootMenuTime;
LARGE_INTEGER PostTime;
BOOLEAN PostTimeLogged;

//
// Any array of booleans indicating which boot entries have previous failed.
//

PBOOLEAN BmpFailedBootEntries = NULL;

//
// ----------------------------------------------- Internal Function Prototypes
//

NTSTATUS
BmpCreateDevices (
    __inout PBOOT_APPLICATION_ENTRY BootEntry
    );

NTSTATUS
BmpGetCustomAction (
    __in_opt PUSHORT Value,
    __out_opt PBOOTMGR_CUSTOM_ACTION_ENTRY CustomAction
    );

VOID
BmpGetDefaultBootEntry (
    __in_ecount(BootEntryCount) PBOOT_APPLICATION_ENTRY *BootEntryList,
    __in ULONG BootEntryCount,
    __out PBOOT_APPLICATION_ENTRY *DefaultEntry,
    __out PULONG DefaultEntryIndex
    );

NTSTATUS
BmpGetSelectedBootEntry (
    __in HANDLE DataStoreHandle,
    __out PBOOT_APPLICATION_ENTRY *BootEntryOut,
    __out PULONG BootEntryIndex,
    __out PBOOLEAN ReturnToFirmware
    );

NTSTATUS
BmpLaunchBootEntry (
    __in PBOOT_APPLICATION_ENTRY BootEntry,
    __in_opt PULONG BootEntryIndex,
    __in ULONG LaunchReason,
    __in BOOLEAN AllowRecovery
    );

NTSTATUS
BmpLaunchCustomBootSequence (
    __in HANDLE DataStoreHandle,
    __in ULONG ElementName
    );

VOID
BmpProcessBadMemory (
    VOID
    );

VOID
BmpProcessBootEntry (
    __in HANDLE DataStoreHandle,
    __in PBOOT_APPLICATION_ENTRY BootEntry,
    __out PBOOLEAN ReturnToFirmware
    );

NTSTATUS
BmpTransferExecution (
    __inout PBOOT_APPLICATION_ENTRY BootEntry,
    __inout PULONG LaunchReason,
    __out PBOOLEAN Recover
    );

NTSTATUS
BmpUpdateApplicationOptions (
    __in HANDLE DataStoreHandle
    );

VOID
BmpInitializeBootStatusDataLog (
    VOID
    );

VOID
BmpLogApplicationLaunchEvent (
    __in GUID* ApplicationIdentifier,
    __in ULONG LaunchReason,
    __in_opt PCWSTR ApplicationPath
    );

VOID
BmpLogApplicationReturnEvent (
    __in GUID* ApplicationIdentifier,
    __in PBOOT_APPLICATION_RETURN_ARGUMENTS BaReturnArgs
    );

VOID
BmpLogBootResolutions (
    VOID
    );

VOID
BmpLogEtwTotalTime (
    );

VOID
BmpLogEtwApplicationScenarioTime (
    __in PCEVENT_DESCRIPTOR ScenarioEventDescriptor,
    __in GUID* ApplicationIdentifier,
    __in LARGE_INTEGER StartTime
    );

VOID
BmpLogEtwEvents (
    __in SI_EVENT_TYPE EventType,
    __in_opt PNOTIFICATION_EVENT_CONTEXT EventContext,
    __in_opt PVOID HandlerContext1,
    __in_opt PVOID HandlerContext2
    );

NTSTATUS
BmpSelfIntegrityCheck (
    VOID
    );

VOID
BmpProcessCustomActions (
    __in HANDLE DataStoreHandle
    );

VOID
BmpRegisterForTrustedWimBootIfWinReEntry (
    _In_ PBOOT_APPLICATION_ENTRY BootEntry,
    _Out_ PBOOLEAN Registered
    );

NTSTATUS
BmpCheckUnsealingEligibility (
    _In_ PBOOT_APPLICATION_ENTRY BootEntry
    );

//
// ------------------------------------------------------------------ Functions
//

NTSTATUS
BmMain (
    __in PBOOT_APPLICATION_PARAMETERS Parameters
    )

/*++

Routine Description:

    Entry point for the Boot Manager.

Arguments:

    Parameters - Incoming parameters to the boot manager, including firmware
        specific handles and an allocated memory descriptor list.

Return Value:

    Error code if fatal failure is experienced.

--*/

{

    ULONG Attributes;
    PBOOT_APPLICATION_ENTRY BootEntry;
    PULONG BootEntryIndex;
    ULONG BootEntryIndexBuffer;
    PBOOT_APPLICATION_ENTRY *BootEntrySequence;
    ULONG BootSequenceCount;
    GUID *BootSequenceList;
    BOOLEAN BootSequenceProcessed;
    BOOLEAN ContinueLoop;
    HANDLE DataStoreHandle;
    BOOLEAN DisableProgress;
    BOOLEAN DisplayInitialized;
    BOOLEAN ErrorHandled;
    LPCGUID Identifier;
    PBOOT_ENTRY_OPTION InitialOptions;
    LIBRARY_PARAMETERS LibraryParameters;
    BOOLEAN PersistBootSequence;
    BOOLEAN RestartOnFailure;
    PBOOT_APPLICATION_RETURN_ARGUMENTS ReturnArguments;
    BOOLEAN ReturnToFirmware;
    BOOLEAN RunningCustomActionsFirst;
    ULONG SequenceCount;
    ULONG SequenceIndex;
    BOOLEAN SkipStartupSequence;
    ULONG StartupSequenceCount;
    GUID *StartupSequenceList;
    BOOLEAN StartupSequenceProcessed;

    NTSTATUS Status;
    NTSTATUS StoreStatus;
    PWSTR StyleSheet;

    BootEntrySequence = NULL;
    BootSequenceList = NULL;
    BootSequenceProcessed = FALSE;
    DataStoreHandle = NULL;
    DisplayInitialized = FALSE;
    ErrorHandled = FALSE;
    InitialOptions = NULL;
    RestartOnFailure = FALSE;
    SequenceCount = 0;
    StartupSequenceList = NULL;
    StartupSequenceProcessed = FALSE;

    //
    // Record the time that the boot manager started executing.
    //

    ApplicationStartTime = BlTimeQueryPerformanceCounter(NULL);
    PostTime = ApplicationStartTime;

    //
    // Initialize the boot environment library.
    //
    // On PCAT systems, the boot manager is present in the root of the active
    // system partition.  However, ancillary files (such as MUI files) are
    // present in the boot subfolder.  Specify the application base directory
    // so that the boot environment can locate these files.
    //

    LibraryParameters = BlGetDefaultLibraryParameters();
    LibraryParameters.Flags &= ~(LF_BOOT_OPTIONS_INITIALIZED);
    LibraryParameters.Flags |= LF_PRESERVE_CONSOLE_CONTROL_INPUT;
    LibraryParameters.Flags |= LF_ALLOCATE_KEYRING;
    LibraryParameters.Flags |= LF_CREATE_SI_DATA;
    LibraryParameters.Flags |= (LF_SVN_CHAIN_START | LF_SVN_CHAIN_CHECK_SELF | LF_SVN_CHAIN_CHECK_CHILDREN);
    LibraryParameters.Flags |= (LF_NO_TCG_LOG_WRITETHROUGH);

    //
    // Uncomment the following line to force enable serial debugging of the BCD
    // specified debug transport.  This is useful when developing a new boot
    // debug transport.
    //

    //LibraryParameters.Flags |= LF_DEBUG_TRANSPORT;

    LibraryParameters.TranslationType = VA_TRANSLATION_TYPE_NONE;
    LibraryParameters.MinimumPageAllocation = _64KB >> PAGE_SHIFT;

    //
    // Set the default heap extend size to be 512KB. Boot manager usually has
    // a very small foot print but in some scenarios, the boot library may
    // make a large number of heap allocations.
    // Having a larger heap extend size prevents the number of heap blocks
    // (and thereby memory descriptors) from getting too large in such
    // scenarios.
    //

    LibraryParameters.MinimumHeapSize = _512KB;

#if defined(PCAT)

    LibraryParameters.ApplicationBaseDirectory = L"\\Boot";

#endif

    Status = BlInitializeLibrary(Parameters, &LibraryParameters);
    if (!NT_SUCCESS(Status)) {
        if (Status != STATUS_INVALID_PARAMETER_9) {
            ConsolePrint(L"BlInitializeLibrary failed 0x%x\r\n", Status);
        }

        goto BmExit;
    }

    //
    // Perform any initialization required for handling errors. A failure to
    // initialize is not treated as fatal. At worst, the user may not be
    // notified about an erroneous BCD entry (if such an entry exists).
    //

    BmErrorInitialize();

#if !defined(PCAT)

    //
    // Register the Boot Environment TraceLogging provider. A failure is not
    // fatal and simply no-ops TraceLoggingWrite calls.

    TraceLoggingRegister(BmBootProvider);

#endif

    //
    // Register the application launching handler so an etw event can be
    // logged with the time spent in the boot manager. Failure here is not
    // lethal.
    //

    (VOID)BlEnRegisterEventHandler(EVENT_APPLICATION_LAUNCHING,
                                   BmpLogEtwEvents,
                                   NULL,
                                   NULL);

    //
    // Save the identifier used to retrieve settings for this application from
    // the configuration data store.  If an identifier is not specified in the
    // boot entry for this application, use a default.
    //

    Identifier = BlGetApplicationIdentifier();
    if (Identifier == NULL) {
        Identifier = &GUID_WINDOWS_BOOTMGR;
    }

    RtlCopyMemory(&BmApplicationIdentifier, Identifier, sizeof(GUID));

    //
    // Initialize the boot directory. This is either the parent directory of the
    // boot manager if a BCD store exists there or a hard-coded fallback path.
    //

    BmFwInitializeBootDirectoryPath();

    //
    // Open the configuration data store that boot settings will be read from,
    // and update the application options from the options contained in the
    // store.  Do this before XML initialization because this step will enable
    // boot debugging if it has been specified as a store option, and this
    // order allows XML initialization to be debugged.  Remember any errors
    // that occur so that they can be reported and an error message displayed
    // after XML init has completed.
    //

    StoreStatus = BmOpenDataStore(&DataStoreHandle);
    if (NT_SUCCESS(StoreStatus)) {

        //
        // Save the initial boot options and read any boot options for this
        // application out of the configuration data store.  This is done
        // before the resume code to enable boot debugging if appropriate.
        //

        StoreStatus = BlCopyApplicationOptions(&InitialOptions);
        if (NT_SUCCESS(StoreStatus)) {
            StoreStatus = BmpUpdateApplicationOptions(DataStoreHandle);
            if (!NT_SUCCESS(StoreStatus)) {
                BmFatalError(BM_ERROR_INVALID_BCD_STORE,
                             (ULONG_PTR)BM_DATA_STORE_FILE_NAME,
                             StoreStatus,
                             0);
            }
        }
    }

    //
    // Now that the BCD store is ready for use, activate the Secure Boot policy
    // for this machine.
    //

    Status = BmSecureBootInitializeMachinePolicy();
    if (!NT_SUCCESS(Status)) {
        BmFatalError(BM_ERROR_NO_SECUREBOOT_POLICY,
                     Status,
                     0,
                     0);

        goto BmExit;
    }

    //
    // Register any system integrity policies before launching any boot apps,
    // and initializing the library which causes MUI files to be loaded.
    //

    Status = BmFwRegisterSystemIntegrityPolicies();
    if (!NT_SUCCESS(Status)) {
        goto BmMainEnd;
    }

    //
    // Now that the BCD store has been processed, reinitialize the library
    // to take into account any new application options for the boot manager.
    //
    // N.B. Do this regardless of whether the BCD store was successfully
    //      processed so that any library components with boot option
    //      dependencies have a chance to initialize.
    //

    LibraryParameters = BlGetCurrentLibraryParameters();
    LibraryParameters.Flags |=
        LF_BOOT_OPTIONS_INITIALIZED | LF_REINITIALIZE;

    Status = BlInitializeLibrary(Parameters, &LibraryParameters);
    if (!NT_SUCCESS(Status) && NT_SUCCESS(StoreStatus)) {
        StoreStatus = Status;
    }

    //
    // Complete firmware specific memory initialization.
    //

    BmFwMemoryInitialize();

    //
    // Initialize the Boot Status Data (BSD) logging engine.
    //

    BmpInitializeBootStatusDataLog();

    //
    // Log the original resolution handed by the firmware and the resolution
    // currently being used as an ETW event.
    //

    BmpLogBootResolutions();

    //
    // Fire up the XML engine.
    //

    StyleSheet = BlResourceFindHtml(L"BOOTMGR.XSL");
    if (StyleSheet == NULL) {
        Status = STATUS_NOT_FOUND;
        ConsolePrint(L"BlResourceFindMessage failed 0x%x\r\n", Status);
        goto BmMainEnd;
    }

    Status = BlXmiInitialize(StyleSheet, &BmOemInputCallback);
    if (!NT_SUCCESS(Status)) {
        ConsolePrint(L"\r\nBlXmiInitialize failed 0x%x\r\n", Status);
        goto BmMainEnd;
    }

    //
    // Now that the display is initialized, report any errors from opening the
    // store.
    //

    DisplayInitialized = TRUE;
    if (!NT_SUCCESS(StoreStatus)) {
        Status = StoreStatus;
        goto BmMainEnd;
    }

    //
    // Check for a valid OEM bitmap to determine whether or not to clear the
    // display.
    //

    if (BlDisplayValidOemBitmap(TRUE) == FALSE) {
        BmDisplayClearScreen();
    }

    //
    // Register routines for doing FVE UI. This enables FVE recovery, PIN, and
    // passphrase entry support.
    //

    BlFveRegisterCallbacks(BmFveDisplayScreen);

    //
    // Perform a self integrity check before allowing the boot manager to load
    // any other boot applications.
    //

    Status = BmpSelfIntegrityCheck();
    if (!NT_SUCCESS(Status)) {
        goto BmMainEnd;
    }

    //
    // Let the remote console know that the system is booting, and start
    // watching for key-strokes indicating the user want the OS selection
    // screen. This impacts resume for the hiberboot case.
    //

    BlXmiWrite(L"<bootmgr/>");

    //
    // Before loading the revocation list, see if a secure boot "factory"
    // reset is needed.
    //

    BlSecureBootCheckForFactoryReset();

    //
    // Register the revocation list before launching any boot apps.
    //

    Status = BmFwRegisterRevocationList();
    if (!NT_SUCCESS(Status)) {
        goto BmMainEnd;
    }

    //
    // Register a routine to handle progress updates, unless progress UX is
    // disabled.
    //

    Status =
        BlGetApplicationOptionBoolean(BCDE_LIBRARY_TYPE_BOOTUX_PROGRESS_DISABLE,
                                      &DisableProgress);

    if (!NT_SUCCESS(Status) || (DisableProgress == FALSE)) {
        BlUtlRegisterProgressRoutine(BmUpdateProgress,
                                     BmUpdateProgressInfo,
                                     NULL,
                                     1);
    }

    //
    // Determine whether resume from hibernation should occur on this boot.
    // Control will return if neither boot configuration data nor a legacy
    // hiberfile indicates that resume should be attempted.  An error status
    // is generated if and only if the BCD store could not be reopened after a
    // resume attempt, implying that no further boot manager action is
    // possible. The resume code may update the display state in order to force
    // the multi-OS menu to show in certain BitLocker scenarios; initialize it
    // here.
    //

    BmDisplayStateCached = FALSE;
    Status = BmResumeFromHibernate(&DataStoreHandle);
    if (!NT_SUCCESS(Status)) {
        goto BmMainEnd;
    }

    //
    // Register routines to handle multicast session information.
    //

    BlUtlRegisterMulticastRoutine(BmDisplayMulticastMessage,
                                  BmUpdateMulticastMessage,
                                  NULL);

    //
    // Check for an option that specifies if the bootmgr should skip
    // the startup sequence.
    //

    Status = BlGetApplicationOptionBoolean(
                 BCDE_BOOTMGR_TYPE_SKIP_STARTUP_SEQUENCE,
                 &SkipStartupSequence);

    if (!NT_SUCCESS(Status)) {
        SkipStartupSequence = FALSE;
    }

    //
    // Check for an option that specifies if the bootmgr should persist a
    // boot sequence.
    //

    Status = BlGetApplicationOptionBoolean(
                 BCDE_BOOTMGR_TYPE_PERSIST_BOOT_SEQUENCE,
                 &PersistBootSequence);

    if (!NT_SUCCESS(Status)) {
        PersistBootSequence = FALSE;
    }

    //
    // Check for an option that specificies if the bootmgr should run custom
    // actions prior to the boot sequence.
    //

    Status = BlGetApplicationOptionBoolean(
                 BCDE_BOOTMGR_TYPE_PROCESS_CUSTOM_ACTIONS_FIRST,
                 &RunningCustomActionsFirst);

    if (NT_SUCCESS(Status) && (RunningCustomActionsFirst != FALSE)) {
        BmpProcessCustomActions(DataStoreHandle);
    }

    //
    // Setup UnlockID related paramaters.
    // Ignore failure, failure to setup IDs
    // should not halt booting process.
    //

    (VOID)BmFwSetupUnlockId();

    //
    // Display the boot options and transfer to the selected boot application.
    // Do this in a loop so that the boot options will be re-displayed if the
    // selected boot application returns control.  Re-parse the boot entries
    // and global parameters from the boot configuration data file on each
    // iteration through the loop so that if a boot application (like a
    // configuration tool) modifies the data file, the changes are reflected
    // the next time the boot menu is displayed.
    //

    SequenceIndex = 0;
    do {
        BootEntry = NULL;
        ContinueLoop = FALSE;

        if (SkipStartupSequence != FALSE) {

            //
            // If the startup sequence is skipped, delete the option to skip it
            // from the BCD store. This way, skipping the startup sequence is a
            // one time thing.
            //

            Status = BmPurgeOption(DataStoreHandle,
                                   &BmApplicationIdentifier,
                                   BCDE_BOOTMGR_TYPE_SKIP_STARTUP_SEQUENCE);

            if (!NT_SUCCESS(Status)) {
                goto BmMainLoopEnd;
            }

            //
            // Mark the Startup Sequence as processed.
            //

            StartupSequenceProcessed = TRUE;
        }

        //
        // First handle the startup sequence. This sequence gets executed on
        // every boot. Retrieve the sequence from the data store if it exists
        // and it has not already been processed.
        //

        if ((BootEntrySequence == NULL) && (StartupSequenceProcessed == FALSE)) {
            SequenceIndex = 0;
            Status = BlGetApplicationOptionGuidList(
                         BCDE_BOOTMGR_TYPE_STARTUP_SEQUENCE,
                         &StartupSequenceList,
                         &StartupSequenceCount);

            if (!NT_SUCCESS(Status)) {
                BootEntrySequence = NULL;
                goto BmMainProcessBootSequence;
            }

            Status = BmGetBootSequence(DataStoreHandle,
                                       StartupSequenceList,
                                       StartupSequenceCount,
                                       BOOT_ENTRY_SEQUENCE,
                                       &BootEntrySequence,
                                       &SequenceCount);

            if (NT_SUCCESS(Status)) {
                BlRemoveApplicationOption(BCDE_BOOTMGR_TYPE_STARTUP_SEQUENCE);
                StartupSequenceProcessed = TRUE;
            } else {
                BootEntrySequence = NULL;
                goto BmMainProcessBootSequence;
            }
        }

BmMainProcessBootSequence:

        //
        // Handle a one-time boot sequence. Retrieve the sequence from the
        // data store, if it exists, and it has not already been marked as
        // processed.
        //

        if ((BootEntrySequence == NULL) && (BootSequenceProcessed == FALSE)) {
            SequenceIndex = 0;
            Status = BlGetApplicationOptionGuidList(
                        BCDE_BOOTMGR_TYPE_BOOT_SEQUENCE,
                        &BootSequenceList,
                        &BootSequenceCount);

            if (!NT_SUCCESS(Status)) {
                BootEntrySequence = NULL;
                goto BmMainLaunchNextBootEntry;
            }

            //
            // Retrieve the boot sequence. Upon success, remove it from the
            // application options so the sequence will not be processed in
            // subsequent loops. If the sequence is not marked as persistent,
            // additionally purge it from the BCD store on disk.
            //

            Status = BmGetBootSequence(DataStoreHandle,
                                       BootSequenceList,
                                       BootSequenceCount,
                                       BOOT_ENTRY_SEQUENCE,
                                       &BootEntrySequence,
                                       &SequenceCount);

            if (NT_SUCCESS(Status)) {
                BlRemoveApplicationOption(BCDE_BOOTMGR_TYPE_BOOT_SEQUENCE);

                //
                // If the boot sequence is not to be persisted, delete it from
                // the BCD store. This ensures it will not be processed in
                // subsequent loops. For persistent boot sequences, mark them
                // as processed so subsequent loops do not re-process them.
                //

                if (PersistBootSequence == FALSE) {
                    Status = BmPurgeOption(DataStoreHandle,
                                           &BmApplicationIdentifier,
                                           BCDE_BOOTMGR_TYPE_BOOT_SEQUENCE);

                    if (!NT_SUCCESS(Status)) {
                        goto BmMainLoopEnd;
                    }

                    //
                    // Log that this boot is from a one-time boot sequence to
                    // ETW for SQM pick-up.
                    //

                    BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                                  &BOOT_SEQUENCE,
                                  NULL,
                                  0,
                                  NULL);

                } else {
                    BootSequenceProcessed = TRUE;
                }

            } else {
                BootEntrySequence = NULL;
                goto BmMainLaunchNextBootEntry;
            }
        }

BmMainLaunchNextBootEntry:

        //
        // Determine the next boot entry to launch.  If an initial boot
        // sequence is present, pull the next entry off of this list.
        // If not, display the boot menu and retrieve the selected boot entry.
        //

        if ((BootEntrySequence != NULL) && (SequenceIndex < SequenceCount)) {
            BootEntryIndex = NULL;
            BootEntry = BootEntrySequence[SequenceIndex];
            BootEntrySequence[SequenceIndex] = NULL;

            //
            // Advance past this entry in the sequence, freeing the sequence
            // if it is complete.  Note that each boot entry is freed as it
            // is executed and does not need to be freed here.
            //

            SequenceIndex += 1;
            if (SequenceIndex == SequenceCount) {
                BlMmFreeHeap(BootEntrySequence);
                BootEntrySequence = NULL;
            }

        } else {
            BootEntryIndex = &BootEntryIndexBuffer;
            ReturnToFirmware = FALSE;
            Status = BmpGetSelectedBootEntry(DataStoreHandle,
                                             &BootEntry,
                                             BootEntryIndex,
                                             &ReturnToFirmware);

            if (!NT_SUCCESS(Status) || (ReturnToFirmware != FALSE)) {
                goto BmMainLoopEnd;
            }
        }

        //
        // Close the data store before transferring execution, so that any
        // store changes are flushed.
        //

        if (DataStoreHandle != NULL) {
            BmCloseDataStore(DataStoreHandle);
            DataStoreHandle = NULL;
        }

        //
        // Assume RestartonFailure not set on read failure.
        //

        Status = BlGetBootOptionBoolean(BootEntry->Options,
                     BCDE_LIBRARY_TYPE_RESTART_ON_FAILURE,
                     &RestartOnFailure);

        if (!NT_SUCCESS(Status)) {
            RestartOnFailure = FALSE;
        }

        //
        // Execute the boot entry, handling any recovery sequence it may have.
        //

        Status = BmpLaunchBootEntry(BootEntry,
                                    BootEntryIndex,
                                    BSD_APP_LAUNCH_REASON_DEFAULT,
                                    TRUE);

        if (!NT_SUCCESS(Status)) {
            if (Status == STATUS_CANCELLED) {
                BmDisplayStateCached = TRUE;
                Status = STATUS_SUCCESS;

            } else {
                ErrorHandled = TRUE;
                goto BmMainLoopEnd;
            }

        } else {
            BmDisplayStateCached = FALSE;

            //
            // Process any bad memory returned by WMD such that it can't be
            // used by another boot application, or the OS.
            //

            BmpProcessBadMemory();
        }

        //
        // Reopen the data store and update the application options for this
        // application based on the parameters in the configuration file.  Do
        // this on each iteration in case the options were changed (as by a
        // configuration utility).
        //

        Status = BmOpenDataStore(&DataStoreHandle);
        if (!NT_SUCCESS(Status)) {
            goto BmMainEnd;
        }

        BlReplaceApplicationOptions(InitialOptions);
        Status = BmpUpdateApplicationOptions(DataStoreHandle);
        if (!NT_SUCCESS(Status)) {
            goto BmMainLoopEnd;
        }

        //
        // Indicate that the iteration through the loop completed successfully.
        //

        ContinueLoop = TRUE;

BmMainLoopEnd:

        //
        // Free the selected boot entry.
        //

        if (BootEntry != NULL) {
            BlDestroyBootEntry(BootEntry);
        }

        if (!NT_SUCCESS(Status) && (RestartOnFailure != FALSE)) {
            goto BmMainEnd;
        }

    } while (ContinueLoop != FALSE);

BmMainEnd:

    //
    // If an error has occurred, record the error.  Since this error only has a
    // failure status as context, only record the error if a more specific
    // error has not already been recorded.
    //

    if (ErrorHandled == FALSE) {
        if (!NT_SUCCESS(Status) && (BmErrorGetLast() == NULL)) {
            BmFatalError(BM_ERROR_FAILURE_STATUS, (ULONG_PTR)Status, 0, 0);
        }

        //
        // Display a screen describing any logged error, and then reset the
        // error state.  If the user selects to continue the boot manager
        // operation even though this error exists, continue with the loop.
        //

        if ((BmErrorGetLast() != NULL) && (DisplayInitialized != FALSE)) {
            BmDisplayDumpError(NULL, 0);
            BmErrorPurge();
        }
    }

    //
    // Execution of the boot manager is complete.  Cleanup boot environment
    // and exit, rebooting the system if appropriate.
    //

    if (InitialOptions != NULL) {
        BlMmFreeHeap(InitialOptions);
    }

    if (BootEntrySequence != NULL) {
        for (SequenceIndex = 0;
             SequenceIndex < SequenceCount;
             SequenceIndex += 1) {

            if (BootEntrySequence[SequenceIndex] != NULL) {
                BlDestroyBootEntry(BootEntrySequence[SequenceIndex]);
            }
        }

        BlMmFreeHeap(BootEntrySequence);
    }

    if (BmpFailedBootEntries != NULL) {
        BlMmFreeHeap(BmpFailedBootEntries);
    }

    if (BootSequenceList != NULL) {
        BlMmFreeHeap(BootSequenceList);
    }

    if (DataStoreHandle != NULL) {
        BmCloseDataStore(DataStoreHandle);
        DataStoreHandle = NULL;
    }

    //
    // Close the BSD log file.
    //

    BlBsdCloseLog();

    //
    // Destroy the boot graphics library.
    //

    BgLibraryDestroy();

    //
    // Either reboot or return to caller on exit.
    //

BmExit:

    //
    // Make sure there are no BitLocker secrets left in memory.
    //

    BlFveEraseAllSecrets();

    Attributes = BlGetApplicationEntry()->Attributes;
    if ((RestartOnFailure != FALSE) ||
        (CHECK_FLAG(Attributes, BOOT_ENTRY_REBOOT_ON_EXIT) != FALSE)) {

        BlFwReboot();
        return STATUS_SUCCESS;

    } else {
        ReturnArguments = Add2Ptr(Parameters, Parameters->ReturnArgumentOffset);
        ReturnArguments->Version = BOOT_APPLICATION_RETURN_ARGUMENTS_VERSION;
        ReturnArguments->ReturnStatus = Status;
        BlDestroyLibrary();
        return Status;
    }
}

NTSTATUS
BmLaunchRecoverySequence (
    __in PBOOT_APPLICATION_ENTRY BootEntry,
    __in ULONG LaunchReason
    )

/*++

Routine Description:

    This routine launches the recovery sequence of a boot application. If any
    of the recovery applications fails, the routine exits and does not continue
    executing the rest of the recovery applications.

Arguments:

    BootEntry - Supplies a pointer to the boot entry whose recovery sequence
        should be launched.

    LaunchReason - Supplies the reason for the application launch:

        BSD_APP_LAUNCH_REASON_DEFAULT - User selected the entry or it was the
            default boot entry.

        BSD_APP_LAUNCH_REASON_MANUAL - User chose system recovery options on the
            advanced OSLOADER menu, or a boot application requested that its
            recovery sequence be executed.

        BSD_APP_LAUNCH_REASON_AUTO_FAILOVER - The boot manager decided to launch
            this entry to recover from a boot failure.

        BSD_APP_LAUNCH_REASON_FVE_RECOVERY - The boot manager decided to launch
            this entry to recovery from an FVE error.

        BSD_APP_LAUNCH_REASON_MULTI_BOOT_REQUESTED - The boot manager decided
            to launch this entry to display the multi boot menu.

            N.B. This is only used for launching WinRE to display multi-boot on
                 touch-only devices in an FVE recovery corner-case.

        BSD_APP_LAUNCH_RECOVERY_BAD_BOOT - The application returned to the boot
            manager indicating that recovery should be launched because of bad
            boot attempts.

        BSD_APP_LAUNCH_RECOVERY_BAD_SHUTDOWN - The application returned to the
            boot manager indicating that recovery should be launched because of
            bad shutdown attempts.

Return Value:

    NT status code.

--*/

{

    ULONG Count;
    HANDLE DataStoreHandle;
    PBOOT_APPLICATION_ENTRY *EntryList;
    GUID *IdentifierList;
    ULONG Index;
    ULONG RecoveryCount;
    NTSTATUS Status;

    EntryList = NULL;
    IdentifierList = NULL;
    RecoveryCount = 0;

    //
    // Open the BCD store and populate the array with boot entries
    // corresponding to each entry in the boot sequence.
    //

    DataStoreHandle = NULL;
    Status = BmOpenDataStore(&DataStoreHandle);
    if (!NT_SUCCESS(Status)) {
        goto LaunchRecoverySequenceEnd;
    }

    Status = BlGetBootOptionGuidList(BootEntry->Options,
                                     BCDE_LIBRARY_TYPE_RECOVERY_SEQUENCE,
                                     &IdentifierList,
                                     &Count);

    if (!NT_SUCCESS(Status)) {
        goto LaunchRecoverySequenceEnd;
    }

    Status = BmGetBootSequence(DataStoreHandle,
                               IdentifierList,
                               Count,
                               BOOT_ENTRY_RECOVERY,
                               &EntryList,
                               &RecoveryCount);

    if (!NT_SUCCESS(Status)) {
        goto LaunchRecoverySequenceEnd;
    }

    if (DataStoreHandle != NULL) {
        BmCloseDataStore(DataStoreHandle);
        DataStoreHandle = NULL;
    }

    //
    // Iterate through the recovery entries, executing one at a time. If a
    // recovery boot entry fails, do not allow a recovery sequence to execute
    // on this boot entry.
    //

    for (Index = 0; Index < RecoveryCount; Index += 1) {
        if ((LaunchReason == BSD_APP_LAUNCH_REASON_AUTO_FAILOVER) ||
            (LaunchReason == BSD_APP_LAUNCH_REASON_BAD_BOOT)) {

            BlRemoveBootOption(
                EntryList[Index]->Options,
                BCDE_LIBRARY_TYPE_BOOTUX_DISPLAY_MESSAGE_OVERRIDE);

            (VOID)BlAppendBootOptionInteger(
                EntryList[Index],
                BCDE_LIBRARY_TYPE_BOOTUX_DISPLAY_MESSAGE_OVERRIDE,
                BcdeBootuxMessageStartupRepair);

        } else if (LaunchReason == BSD_APP_LAUNCH_REASON_FVE_RECOVERY) {

            BlRemoveBootOption(
                EntryList[Index]->Options,
                BCDE_LIBRARY_TYPE_BOOTUX_DISPLAY_MESSAGE_OVERRIDE);

            (VOID)BlAppendBootOptionInteger(
                EntryList[Index],
                BCDE_LIBRARY_TYPE_BOOTUX_DISPLAY_MESSAGE_OVERRIDE,
                BcdeBootuxMessageFveRecovery);
        }

        Status = BmpLaunchBootEntry(EntryList[Index],
                                    NULL,
                                    LaunchReason,
                                    FALSE);

        if (!NT_SUCCESS(Status)) {
            goto LaunchRecoverySequenceEnd;
        }
    }

LaunchRecoverySequenceEnd:
    if (EntryList != NULL) {
        for (Index = 0; Index < RecoveryCount; Index += 1) {
            if (EntryList[Index] != NULL) {
                BlDestroyBootEntry(EntryList[Index]);
            }
        }

        BlMmFreeHeap(EntryList);
    }

    if (IdentifierList != NULL) {
        BlMmFreeHeap(IdentifierList);
    }

    return Status;
}

BOOLEAN
BmOemCustomActionExists (
    VOID
    )

/*++

Routine Description:

    This routine determines whether or not an OEM custom action exists in the
    BCD store.

Arguments:

    None.

Return Value:

    TRUE if an OEM action exists. FALSE otherwise.

--*/

{

    if (NT_SUCCESS(BmpGetCustomAction(NULL, NULL))) {
        return TRUE;
    }

    return FALSE;
}

BOOLEAN
BmOemInputCallback (
    __in WCHAR Key
    )

/*++

Routine Description:

    This callback function checks whether a console input character should
    be treated as an OEM action key based upon whether the character is
    described in the OEM actions BCD element.

Arguments:

    Key - Supplies the input character to test.

Return Value:

    Returns TRUE if the input should be regarded as an OEM action character,
    otherwise FALSE.

--*/

{

    if (NT_SUCCESS(BmpGetCustomAction(&Key, NULL))) {
        return TRUE;
    }

    return FALSE;
}

NTSTATUS
BmProcessCustomAction (
    __in HANDLE DataStoreHandle,
    __in_opt PUSHORT Value
    )

/*++

Routine Description:

    This routine processes custom OEM key actions.  The value in this case
    should correspond to one of the ACTION(n)_KEY scancodes (see bootdef.h).

    N.B. In order for this routine to have any effect, there must also be
         a custom boot action BCD entry with matching value that describes
         the desired action to perform.

Arguements:

    DataStoreHandle - Supplies a handle to the data store to query for
        custom actions.

    Value - Supplies the value of the custom action.

Return Value:

    NT status code.

--*/

{

    BOOTMGR_CUSTOM_ACTION_ENTRY CustomAction;
    NTSTATUS Status;

    //
    // Fetch the custom action from the data store.
    //

    Status = BmpGetCustomAction(Value, &CustomAction);
    if (!NT_SUCCESS(Status)) {
        goto ProcessCustomActionEnd;
    }

    //
    // Perform the desired action.
    //

    switch (CustomAction.Action) {
        case BOOTMGR_ACTION_LAUNCH_BOOTSEQUENCE:
            Status = BmpLaunchCustomBootSequence(
                         DataStoreHandle,
                         CustomAction.ActionData.BootSequence.ElementName);

            break;

        default:
            Status = STATUS_NOT_SUPPORTED;
            break;
    }

ProcessCustomActionEnd:
    return Status;
}

//
// --------------------------------------------------------- Internal Functions
//

VOID
BmpAllocateBadMemoryRegion (
    __in PFN_NUMBER BasePfn,
    __in ULONGLONG PageCount
    )

/*++

Routine Description:

    This routine will attempt to allocate the region in the largest chunks
    poissible, and add them to the local bad memory list.

Arguements:

    BasePfn - Supplies the base physical page frame number of the bad
        memory region.

    PageCount - Supplies the number of pages of the bad memory region.

Return Value:

    None

--*/

{

    MEMORY_TYPE BadMemoryType;
    ULONGLONG PagesRemaining;
    PHYSICAL_ADDRESS PhysAddr;
    NTSTATUS Status;

    if (PageCount < 1) {
        return;
    }

    BadMemoryType = GET_MEMORY_TYPE(MEMORY_CLASS_PLATFORM,
                                    MEMORY_PLATFORM_TYPE_BAD);

    //
    // Attempt to allocate the largest possible chunk starting at this base.
    //

    PagesRemaining = PageCount;
    while (PagesRemaining > 0) {
        PhysAddr.QuadPart = (ULONGLONG)BasePfn << PAGE_SHIFT;
        Status = BlMmAllocatePhysicalPages(
                     &PhysAddr,
                     PageCount,
                     BadMemoryType,
                     MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS,
                     1);

        if (NT_SUCCESS(Status)) {

#if DBG

            BlStatusPrint(L"Allocated *BAD* Memory region: %06x, ",
                          BasePfn);

            BlStatusPrint(L"(%d)\n", PageCount);

#endif

            MARK_SAFE_ADDITION(BasePfn, (PFN_NUMBER)PageCount);
            BasePfn += (PFN_NUMBER)PageCount;
            PagesRemaining -= PageCount;
            PageCount = PagesRemaining;
            continue;
        }

        //
        // If we reach here, and the page count reaches zero, then we
        // failed to allocate even one page starting at this base.
        // Increment the base, subtract one from the pages remaining, and
        // start over with page count equal to the number of pages
        // remaining.
        //

        PageCount -= 1;
        if (PageCount == 0) {

#if DBG

            BlStatusPrint(L"**Failed *BAD* Memory region: %06x, (1)\n",
                          BasePfn);

#endif

            BasePfn += 1;
            PagesRemaining -= 1;
            PageCount = PagesRemaining;
        }
    }

    return;
}

ULONG
BmpDeterminePfnRun (
    __in_ecount(Count) PPFN_NUMBER PfnArray,
    __in ULONG Count,
    __out PPFN_NUMBER PfnBase
    )

/*++

Routine Description:

    This routine determines the number of contiguous pages, either ascending,
    or descending, in the array starting from PfnArray[0].

Arguments:

    PfnArray - Supplies a pointer to an array of page frame numbers.

    Count - Supplies the number of elements in the array.

    PfnBase - Supplies a pointer to a page frame number which returns
        the base page of the run.

Return Value:

    Returns the number of contiguous pages in the run starting at
    PfnArray[0].

--*/

{

    ULONG Index;
    ULONG RunLength;

    *PfnBase = PfnArray[0];
    if ((Count == 0) || (Count == 1)) {
        return Count;
    }

    //
    // Snoop to see if the next pfn is ascending, or descending, then
    // loop to the extent of the run to detemine its length and base.
    //

    MARK_SAFE_ADDITION(PfnArray[0], 1);
    MARK_SAFE_SUBTRACTION(PfnArray[0], 1);
    RunLength = 1;
    if (PfnArray[1] == (PfnArray[0] + 1)) {
        RunLength += 1;
        for (Index = 1; Index < (Count - 1); Index++) {
            if (PfnArray[Index + 1] == (PfnArray[Index] + 1)) {
                RunLength += 1;

            } else {
                break;
            }
        }

    } else if (PfnArray[1] == (PfnArray[0] - 1)) {
        RunLength += 1;
        *PfnBase = PfnArray[1];
        for (Index = 1; Index < (Count - 1); Index++) {
            if (PfnArray[Index + 1] == (PfnArray[Index] - 1)) {
                RunLength += 1;
                *PfnBase = PfnArray[Index + 1];

            } else {
                break;
            }
        }
    }

    return RunLength;
}

NTSTATUS
BmpGetCustomAction (
    __in_opt PUSHORT Value,
    __out_opt PBOOTMGR_CUSTOM_ACTION_ENTRY CustomAction
    )

/*++

Routine Description:

    This routine will search the boot manager custom actions integer list and
    return the first element found with matching value. If no value is
    supplied, the first element found is returned.

Arguements:

    Value - Supplies a pointer to the value of the custom action element to
        retrieve. This parameter is optional.

    CustomAction - Supplies an optional pointer to a buffer that receives the
        custom action element.

Return Value:

    NT status code.

--*/

{

    ULONGLONG Count;
    PBOOTMGR_CUSTOM_ACTION_ENTRY CustomActionsList;
    ULONG Index;
    NTSTATUS Status;

    CustomActionsList = NULL;

    //
    // Read in the custom actions list.
    //

    Status = BlGetApplicationOptionIntegerList(
                 BCDE_BOOTMGR_TYPE_CUSTOM_ACTIONS_LIST,
                 (PULONGLONG *)&CustomActionsList,
                 &Count);

    if (!NT_SUCCESS(Status)) {
        goto GetCustomActionEnd;
    }

    //
    // The Custom actions are stored in integer list format to leverage BCD
    // (edit) support for integer lists, therefore the size must be converted.
    //

    Count /= sizeof(BOOTMGR_CUSTOM_ACTION_ENTRY) / sizeof(ULONGLONG);
    if (Count == 0) {
        Status = STATUS_UNSUCCESSFUL;
        goto GetCustomActionEnd;
    }

    //
    // Search for a custom entry with an input that matches the supplied
    // value.  Bail out if there is a version mismatch.
    //

    Status = STATUS_NOT_FOUND;
    for (Index = 0; Index < Count; Index += 1) {
        if (CustomActionsList[Index].Version !=
            BOOTMGR_CUSTOM_ACTION_VERSION) {

            Status = STATUS_REVISION_MISMATCH;
            break;
        }

        //
        // If no value is supplied or if there is a match, return the current
        // action. In the case of no supplied value, this returns the first
        // action.
        //

        if ((ARGUMENT_PRESENT(Value) == FALSE) ||
            (CustomActionsList[Index].Input.Value == *Value)) {

            if (ARGUMENT_PRESENT(CustomAction) != FALSE) {
                RtlCopyMemory(CustomAction,
                              &CustomActionsList[Index],
                              sizeof(BOOTMGR_CUSTOM_ACTION_ENTRY));
            }

            Status = STATUS_SUCCESS;
            break;
        }
    }

GetCustomActionEnd:
    if (CustomActionsList != NULL) {
        BlMmFreeHeap(CustomActionsList);
    }

    return Status;
}

VOID
BmpGetDefaultBootEntry (
    __in_ecount(BootEntryCount) PBOOT_APPLICATION_ENTRY *BootEntryList,
    __in ULONG BootEntryCount,
    __out PBOOT_APPLICATION_ENTRY *DefaultEntry,
    __out PULONG DefaultEntryIndex
    )

/*++

Routine Description:

    This routine returns the default boot entry for the system.

Arguments:

    BootEntryList - Supplies a pointer to an array of boot entry elements.

    BootEntryCount - Supplies the number of elements in the array.

    DefaultEntry - Supplies a pointer that receives the address of the default
        application entry.

    DefaultEntryIndex - Supplies a pointer that receives the index of the
        default application entry.

Return Value:

    None.

--*/

{

    GUID DefaultObject;
    ULONG Index;
    NTSTATUS Status;

    NT_ASSERT(BootEntryCount != 0);

    //
    // Default to the first entry in the case where there is only 1 entry
    // and/or the default GUID is not set.
    //

    *DefaultEntry = BootEntryList[0];
    *DefaultEntryIndex = 0;
    if (BootEntryCount == 1) {
        return;
    }

    Status = BlGetApplicationOptionGuid(BCDE_BOOTMGR_TYPE_DEFAULT_OBJECT,
                                        &DefaultObject);

    if (!NT_SUCCESS(Status)) {
        return;
    }

    //
    // Search the boot entries for the entry whose GUID matches the default
    // GUID.
    //

    for (Index = 0; Index < BootEntryCount; Index += 1) {
        if (IsEqualGUID(&BootEntryList[Index]->Identifier, &DefaultObject)) {
            *DefaultEntry = BootEntryList[Index];
            *DefaultEntryIndex = Index;
            break;
        }
    }

    return;
}

NTSTATUS
BmpGetSelectedBootEntry (
    __in HANDLE DataStoreHandle,
    __out PBOOT_APPLICATION_ENTRY *BootEntryOut,
    __out PULONG BootEntryIndex,
    __out PBOOLEAN ReturnToFirmware
    )

/*++

Routine Description:

    This routine displays a boot menu, allowing the user to select the boot
    entry to launch.  The selected boot entry is returned.

Arguments:

    DataStoreHandle - Supplies a handle to the boot configuration data store
        containing the boot entries.

    BootEntryOut - Supplies a pointer to a variable that receives the selected
        boot entry.  The caller is responsible for freeing this memory.

    BootEntryIndex - Supplies a pointer to a variable that receives the index
        of the selected boot entry.

    ReturnToFirmware - Supplies a pointer to a variable that receives a
        boolean indicating whether the user has selected to exit the boot
        manager.

Return Value:

    STATUS_SUCCESS if the boot entry is selected successfully.

    STATUS_FILE_INVALID if the boot configuration data file does not contain
        any boot entries.

    Another error status code if boot entries could not be enumerated or
        displayed properly.

--*/

{

    BOOLEAN AutoFailover;
    PWCHAR BcdFilePath;
    ULONG BootEntryCount;
    PBOOT_APPLICATION_ENTRY *BootEntryList;
    LARGE_INTEGER BootMenuEndTime;
    ULONGLONG BootMenuPolicy;
    LARGE_INTEGER BootMenuStartTime;
    EVENT_DATA_DESCRIPTOR EventDataDescriptor[1];
    SIZE_T FailedBootArraySize;
    ULONG Index;
    PPXE_INFO PxeInfo;
    PBOOT_APPLICATION_ENTRY SelectedEntry;
    ULONG SelectedEntryIndex;
    NTSTATUS Status;
    ULONGLONG Timeout;
    ULONG ToolCount;
    PBOOT_APPLICATION_ENTRY *ToolEntryList;

    BootEntryCount = 0;
    BootEntryList = NULL;
    SelectedEntry = NULL;
    ToolCount = 0;
    ToolEntryList = NULL;

    //
    // Retrieve the list of boot entries for the system.
    //

    Status = BmEnumerateBootEntries(DataStoreHandle,
                                    &BootEntryList,
                                    &BootEntryCount);

    if (!NT_SUCCESS(Status)) {
        goto GetSelectedBootEntryEnd;
    }

    //
    // If the configuration file does not contain information about any
    // boot entries, fail.
    //

    if (BootEntryCount == 0) {
        Status = STATUS_FILE_INVALID;
        PxeInfo = BlNetGetPxeInfo();

        //
        // Check if this error has occurred in the context of a PXE boot
        // scenario.  If so, the error is customized for that case.
        //

        if ((PxeInfo == NULL) || (PxeInfo->PxenvEntry == NULL)) {
            BmFatalError(BM_ERROR_NO_VALID_OS_ENTRY,
                         (ULONG_PTR)BM_DATA_STORE_FILE_NAME,
                         Status,
                         0);

        } else {
            if (PxeInfo->BcdFilePath == NULL) {
                BcdFilePath = BM_DATA_STORE_FILE_NAME;
            } else {
                BcdFilePath = PxeInfo->BcdFilePath;
            }

            BmFatalError(BM_ERROR_NO_VALID_PXE_OS_ENTRY,
                         (ULONG_PTR)BcdFilePath,
                         Status,
                         0);
        }

        goto GetSelectedBootEntryEnd;
    }

    //
    // Allocate the array recording failed boot entries if it does not already
    // exist.
    //

    if (BmpFailedBootEntries == NULL) {
        FailedBootArraySize = BootEntryCount * sizeof(BOOLEAN);
        BmpFailedBootEntries = BlMmAllocateHeap(FailedBootArraySize);
        if (BmpFailedBootEntries != NULL) {
            RtlZeroMemory(BmpFailedBootEntries, FailedBootArraySize);
        }
    }

    //
    // If a boot entry override is present, search the entry list for a match.
    // This allows the debugger to set an override to boot a specific entry.
    //
    // N.B. Do not look at the failed boot entries list. A debugger override
    //      should still attempt to boot an application that previously failed.
    //

    if (BmBootEntryOverridePresent != FALSE) {
        for (Index = 0; Index < BootEntryCount; Index += 1) {
            if ((BootEntryList[Index] != NULL) &&
                (IsEqualGUID(&BootEntryList[Index]->Identifier,
                             (PGUID)&BmBootEntryOverride) != FALSE)) {

                SelectedEntry = BootEntryList[Index];
                *BootEntryIndex = Index;
                break;
            }
        }
    }

    if (SelectedEntry != NULL) {
        Status = STATUS_SUCCESS;
        *BootEntryOut = SelectedEntry;
        *ReturnToFirmware = FALSE;
        goto GetSelectedBootEntryEnd;
    }

    //
    // Log the boot entry count to ETW for SQM pick-up.
    //

    EventDataDescCreate(&EventDataDescriptor[0],
                        (PVOID)&BootEntryCount,
                        sizeof(ULONG));

    BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                  &BOOT_BOOTMGR_MULTI_OS_COUNT,
                  NULL,
                  1,
                  EventDataDescriptor);

    //
    // If there is only one boot entry, process it to determine if the boot
    // if there is user input. Otherwise, for multi-boot, determine the boot
    // menu policy of the default entry. If the policy is "MenuPolicyStandard"
    // and the boot menu is not forced via BCD, then boot that option without
    // reading key strokes. In any case where an application will be launched
    // without displaying the boot menu, mark the boot menu to be displayed if
    // the user cancels that application and control is returned to the boot
    // manager.
    //

    if (BmDisplayStateCached == FALSE) {
        Status = BlGetApplicationOptionBoolean(
                     BCDE_BOOTMGR_TYPE_DISPLAY_BOOT_MENU,
                     &BmpDisplayBootMenu);

        if (!NT_SUCCESS(Status)) {
            BmpDisplayBootMenu = FALSE;
        }
    }

    if (BootEntryCount == 1) {
        SelectedEntry = BootEntryList[0];
        BmpProcessBootEntry(DataStoreHandle, SelectedEntry, ReturnToFirmware);
        if (BmpDisplayBootMenu == FALSE) {
            *BootEntryOut = SelectedEntry;
            *BootEntryIndex = 0;
            Status = STATUS_SUCCESS;
            BmpDisplayBootMenu = TRUE;
            goto GetSelectedBootEntryEnd;
        }

    } else {
        BmpGetDefaultBootEntry(BootEntryList,
                               BootEntryCount,
                               &SelectedEntry,
                               &SelectedEntryIndex);

        //
        // If the default boot entry previously failed, find the first boot
        // entry that has not failed. This is an auto-failover option and
        // should never display a boot menu.
        //

        AutoFailover = FALSE;
        if ((BmpFailedBootEntries != NULL) &&
            (BmpFailedBootEntries[SelectedEntryIndex] != FALSE)) {

            for (Index = 0; Index < BootEntryCount; Index += 1) {
                if ((BootEntryList[Index] != NULL) &&
                    (BmpFailedBootEntries[Index] == FALSE)) {

                    SelectedEntry = BootEntryList[Index];
                    SelectedEntryIndex = Index;
                    AutoFailover = TRUE;
                    BmpDisplayBootMenu = FALSE;
                    break;
                }
            }
        }

        if (CHECK_FLAG(SelectedEntry->Attributes,
                       BOOT_ENTRY_OS_LOADER) != FALSE) {

            //
            // Boot entries with standard menu policy are always booted
            // automatically unless a BCD override is present. Boot entries
            // with non-standard policy are only auto-booted in the
            // auto-failover case.
            //

            BootMenuPolicy = BmGetBootMenuPolicy(SelectedEntry);
            if ((BootMenuPolicy == MenuPolicyStandard) &&
                (BmpDisplayBootMenu == FALSE)) {

                //
                // Mark that this is a multi-boot system if the bootmgr's
                // timeout setting is non-zero. This will be consumed
                // by the loader, as it has Standard boot menu policy.
                //

                Status = BlGetApplicationOptionInteger(
                             BCDE_BOOTMGR_TYPE_TIMEOUT,
                             &Timeout);

                if (!NT_SUCCESS(Status) || (Timeout != 0)) {
                    (VOID)BlAppendBootOptionBoolean(
                        SelectedEntry,
                        BCDE_LIBRARY_TYPE_MULTI_BOOT_SYSTEM,
                        TRUE);
                }

                *BootEntryIndex = SelectedEntryIndex;
                *BootEntryOut = SelectedEntry;
                Status = STATUS_SUCCESS;
                *ReturnToFirmware = FALSE;
                BmpDisplayBootMenu = TRUE;
                goto GetSelectedBootEntryEnd;

            } else if (AutoFailover != FALSE) {
                *BootEntryIndex = SelectedEntryIndex;
                *BootEntryOut = SelectedEntry;
                Status = STATUS_SUCCESS;
                *ReturnToFirmware = FALSE;
                BmpDisplayBootMenu = TRUE;
                goto GetSelectedBootEntryEnd;
            }
        }

        //
        // Check the boot menu timeout. If it is zero, don't even launch the
        // boot menu and just continue with the default, processing key strokes
        // as appropriate.
        //

        Status = BlGetApplicationOptionInteger(BCDE_BOOTMGR_TYPE_TIMEOUT,
                                               &Timeout);

        if (NT_SUCCESS(Status) && (Timeout == 0)) {
            BmpProcessBootEntry(DataStoreHandle,
                                SelectedEntry,
                                ReturnToFirmware);

            //
            // If the boot menu should not be shown based on user input, launch
            // the application. If it should be shown, clear the timeout.
            //

            if (BmpDisplayBootMenu == FALSE) {
                *BootEntryOut = SelectedEntry;
                *BootEntryIndex = SelectedEntryIndex;
                Status = STATUS_SUCCESS;
                BmpDisplayBootMenu = TRUE;
                goto GetSelectedBootEntryEnd;

            } else {
                BlRemoveApplicationOption(BCDE_BOOTMGR_TYPE_TIMEOUT);
            }
        }
    }

    //
    // Retrieve the list of tool application entries for the system
    // ignoring any failures as tools are non-critical, then display the
    // boot options and tools.
    //

    SelectedEntry = NULL;
    BmEnumerateToolEntries(DataStoreHandle, &ToolEntryList, &ToolCount);

    //
    // Log the tool entry count to ETW for SQM pick-up.
    //

    EventDataDescCreate(&EventDataDescriptor[0],
                        (PVOID)&ToolCount,
                        sizeof(ULONG));

    BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                  &BOOT_BOOTMGR_MULTI_OS_TOOLS_COUNT,
                  NULL,
                  1,
                  EventDataDescriptor);

    //
    // Log that the multi-OS boot menu is about to be displayed.
    //

    BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                  &BOOT_BOOTMGR_MULTI_OS_MENU_DISPLAYED,
                  NULL,
                  0,
                  NULL);

    BootMenuStartTime = BlTimeQueryPerformanceCounter(NULL);
    Status = BmDisplayBootMenu(DataStoreHandle,
                               BootEntryList,
                               BootEntryCount,
                               ToolEntryList,
                               ToolCount,
                               &SelectedEntry,
                               ReturnToFirmware);

    BootMenuEndTime = BlTimeQueryPerformanceCounter(NULL);
    BootMenuTime.QuadPart = BootMenuEndTime.QuadPart -
        BootMenuStartTime.QuadPart;

    ApplicationStartTime.QuadPart += BootMenuTime.QuadPart;
    if (!NT_SUCCESS(Status)) {
        goto GetSelectedBootEntryEnd;
    }

    *BootEntryOut = SelectedEntry;
    Status = STATUS_SUCCESS;

    //
    // Free each tool application entry.  Do not free the selected tool
    // application entry on success, however.
    //

GetSelectedBootEntryEnd:
    if (ToolEntryList != NULL) {
        for (Index = 0; Index < ToolCount; Index += 1) {
            if ((ToolEntryList[Index] != NULL) &&
                ((ToolEntryList[Index] != SelectedEntry) ||
                 !NT_SUCCESS(Status))) {

                BlDestroyBootEntry(ToolEntryList[Index]);

            } else if (ToolEntryList[Index] == SelectedEntry) {
                *BootEntryIndex = INVALID_BOOT_INDEX;
            }
        }

        BlMmFreeHeap(ToolEntryList);
    }

    //
    // Free each boot entry.  Do not free the selected boot entry on
    // success, however.
    //

    if (BootEntryList != NULL) {
        for (Index = 0; Index < BootEntryCount; Index += 1) {
            if ((BootEntryList[Index] != NULL) &&
                ((BootEntryList[Index] != SelectedEntry) ||
                 !NT_SUCCESS(Status))) {

                BlDestroyBootEntry(BootEntryList[Index]);

            } else if (BootEntryList[Index] == SelectedEntry) {
                *BootEntryIndex = Index;
            }
        }

        BlMmFreeHeap(BootEntryList);
    }

    return Status;
}

BOOLEAN
BmpIsTime1MoreRecentThanTime2 (
    __in PTIME_FIELDS Time1,
    __in PTIME_FIELDS Time2
    )

/*++

Routine Description:

    This routine compares two time fields to determine which one
    is the most recent.

Arguments:

    Time1 - The first time fields to compare.

    Time2 - The second Time field to compare.

Return Value:

    TRUE if Time1 is more recent than Time2, otherwise FALSE.

--*/

{

    if (Time1->Year > Time2->Year) {
        return TRUE;

    } else if (Time1->Year < Time2->Year) {
        return FALSE;
    }

    //
    // Years are equal at this point.
    //

    if (Time1->Month > Time2->Month) {
        return TRUE;

    } else if (Time1->Month < Time2->Month) {
        return FALSE;
    }

    //
    // Months are equal at this point.
    //

    if (Time1->Day > Time2->Day) {
        return TRUE;

    } else if (Time1->Day < Time2->Day) {
        return FALSE;
    }

    //
    // Days are equal at this point.
    //

    if (Time1->Hour > Time2->Hour) {
        return TRUE;

    } else if (Time1->Hour < Time2->Hour) {
        return FALSE;
    }

    //
    // Hours are equal at this point.
    //

    if (Time1->Minute > Time2->Minute) {
        return TRUE;

    } else if (Time1->Minute < Time2->Minute) {
        return FALSE;
    }

    //
    // Minutes are equal at this point.
    //

    if (Time1->Second > Time2->Second) {
        return TRUE;

    } else if (Time1->Second < Time2->Second) {
        return FALSE;
    }

    //
    // Seconds are equal at this point.
    //

    if (Time1->Milliseconds > Time2->Milliseconds) {
        return TRUE;

    } else if (Time1->Milliseconds < Time2->Milliseconds) {
        return FALSE;
    }

    //
    // Milliseconds are equal at this point.
    //

    //
    // As best we can tell, these times are equivalent.
    //

    return FALSE;
}

NTSTATUS
BmpLaunchBootEntry (
    __in PBOOT_APPLICATION_ENTRY BootEntry,
    __in_opt PULONG BootEntryIndex,
    __in ULONG LaunchReason,
    __in BOOLEAN AllowRecovery
    )

/*++

Routine Description:

    This routine launches the boot application associated with a boot entry.
    If the application fails, it also launches the recovery sequence associated
    with the entry, if appropriate.

    N.B. This routine is called recursively in the case of a recovery sequence.

Arguments:

    BootEntry - Supplies the boot entry to execute.

    BootEntryIndex - Supplies a pointer to the boot entries index into the list
        of boot entries. This parameter is optional.

    LaunchReason - Supplies the reason for the application launch:

        BSD_APP_LAUNCH_REASON_DEFAULT - User selected the entry or it was the
            default boot entry.

        BSD_APP_LAUNCH_REASON_MANUAL - User chose system recovery options on the
            advanced OSLOADER menu, or a boot application requested that its
            recovery sequence be executed.

        BSD_APP_LAUNCH_REASON_AUTO_FAILOVER - The boot manager decided to launch
            this entry to recover from a boot failure.

        BSD_APP_LAUNCH_REASON_FVE_RECOVERY - The boot manager decided to launch
            this entry to recovery from an FVE error.

        BSD_APP_LAUNCH_REASON_MULTI_BOOT_REQUESTED - The boot manager decided
            to launch this entry to display the multi boot menu.

            N.B. This is only used for launching WinRE to display multi-boot on
                 touch-only devices in an FVE recovery corner-case.

        BSD_APP_LAUNCH_RECOVERY_BAD_BOOT - The application returned to the boot
            manager indicating that recovery should be launched because of bad
            boot attempts.

        BSD_APP_LAUNCH_RECOVERY_BAD_SHUTDOWN - The application returned to the
            boot manager indicating that recovery should be launched because of
            bad shutdown attempts.

    AllowRecovery - Supplies a flag indicating if recovery is allowed.  This
        flag may be used to prevent an object in the recovery sequnece from
        triggering it's own recovery sequence recursion.

Return Value:

    The status code returned from the boot application.  The application
    may not return, in which case there is no return value.

--*/

{

    BOOLEAN AdvancedOptionsOneTime;
    BOOLEAN AllowTrustedWimBoot;
    BOOLEAN AutoRecover;
    HANDLE DataStoreHandle;
    BM_ERROR_DISPOSITION Disposition;
    GUID Identifier;
    BOOLEAN OptionsEditOneTime;
    BOOLEAN Recover;
    BOOLEAN RestartOnFailure;
    BOOLEAN RetryEntry;
    BOOLEAN RetryRecovery;
    NTSTATUS ReturnStatus;
    NTSTATUS Status;
    BOOLEAN TrustedWimBootRegistered;

    //
    // If a one-time boot option exists, delete it from disk before entering
    // the loop. Subsequent attempts to retry this entry should not use
    // one-time options set in the BCD store.
    //

    if (CHECK_FLAG(BootEntry->Attributes, BOOT_ENTRY_OS_LOADER) != FALSE) {
        if (BlBootOptionExists(
                BootEntry->Options,
                BCDE_OSLOADER_TYPE_ADVANCED_OPTIONS_ONE_TIME) != FALSE) {

            DataStoreHandle = NULL;
            Status = BmOpenDataStore(&DataStoreHandle);
            if (NT_SUCCESS(Status)) {
                RtlCopyMemory(&Identifier,
                              &BootEntry->Identifier,
                              sizeof(GUID));

                BmPurgeOption(DataStoreHandle,
                              &Identifier,
                              BCDE_OSLOADER_TYPE_ADVANCED_OPTIONS_ONE_TIME);

                BmCloseDataStore(DataStoreHandle);
            }
        }

        if (BlBootOptionExists(
                BootEntry->Options,
                BCDE_OSLOADER_TYPE_OPTIONS_EDIT_ONE_TIME) != FALSE) {

            DataStoreHandle = NULL;
            Status = BmOpenDataStore(&DataStoreHandle);
            if (NT_SUCCESS(Status)) {
                RtlCopyMemory(&Identifier,
                              &BootEntry->Identifier,
                              sizeof(GUID));

                BmPurgeOption(DataStoreHandle,
                              &Identifier,
                              BCDE_OSLOADER_TYPE_OPTIONS_EDIT_ONE_TIME);

                BmCloseDataStore(DataStoreHandle);
            }
        }
    }

    //
    // Attempt to launch the selected application.
    //

    do {
        AllowTrustedWimBoot = FALSE;
        AutoRecover = FALSE;
        Recover = FALSE;
        RetryEntry = FALSE;
        TrustedWimBootRegistered = FALSE;

        //
        // Set the launching boot entry as the selected boot entry.
        //

        BmSetSelectedBootEntry(BootEntry);

        //
        // Before transferring execution to the selected boot entry, run through
        // the boot options and create any device options that require this
        // step.  This has the result that the options list for an application
        // will not require any creation steps in that application.
        //

        if (LaunchReason == BSD_APP_LAUNCH_REASON_DEFAULT) {

            //
            // If this is a WinRE default launch, register Associated OS
            // for trusted WIM boot. This is a best effort so do not error.
            //

            BmpRegisterForTrustedWimBootIfWinReEntry(BootEntry,
                                                     &TrustedWimBootRegistered);
        }

        Status = BmpCreateDevices(BootEntry);
        if (!NT_SUCCESS(Status)) {

            if (TrustedWimBootRegistered != FALSE) {

                //
                // BmpLaunchBootEntry can be called recursively by recovery
                // below. Make sure to unregister so other sceanrios aren't
                // unintentionally pulled into trusted wim boot.
                //

                BlFveRegisterBootEntryForTrustedWimBoot(NULL, FALSE);
                TrustedWimBootRegistered = FALSE;
            }

            ReturnStatus = Status;
            if (AllowRecovery != FALSE) {
                LaunchReason = BSD_APP_LAUNCH_REASON_AUTO_FAILOVER;
                goto BootFailure;

            } else {
                goto LaunchBootEntryEnd;
            }
        }

        //
        // Convert any one-time options into in-memory settings that the
        // OS loader actually reads.
        //

        if (CHECK_FLAG(BootEntry->Attributes, BOOT_ENTRY_OS_LOADER) != FALSE) {
            Status = BlGetBootOptionBoolean(
                         BootEntry->Options,
                         BCDE_OSLOADER_TYPE_ADVANCED_OPTIONS_ONE_TIME,
                         &AdvancedOptionsOneTime);

            if (NT_SUCCESS(Status)) {
                if (AdvancedOptionsOneTime != FALSE) {
                    (VOID)BlAppendBootOptionBoolean(
                        BootEntry,
                        BCDE_LIBRARY_TYPE_DISPLAY_ADVANCED_OPTIONS,
                        TRUE);

                } else {
                    BlRemoveBootOption(
                        BootEntry->Options,
                        BCDE_LIBRARY_TYPE_DISPLAY_ADVANCED_OPTIONS);
                }

                BlRemoveBootOption(
                    BootEntry->Options,
                    BCDE_OSLOADER_TYPE_ADVANCED_OPTIONS_ONE_TIME);
            }

            Status = BlGetBootOptionBoolean(
                         BootEntry->Options,
                         BCDE_OSLOADER_TYPE_OPTIONS_EDIT_ONE_TIME,
                         &OptionsEditOneTime);

            if (NT_SUCCESS(Status)) {
                if (OptionsEditOneTime != FALSE) {
                    (VOID)BlAppendBootOptionBoolean(
                        BootEntry,
                        BCDE_LIBRARY_TYPE_DISPLAY_OPTIONS_EDIT,
                        TRUE);

                } else {
                    BlRemoveBootOption(
                        BootEntry->Options,
                        BCDE_LIBRARY_TYPE_DISPLAY_OPTIONS_EDIT);
                }

                BlRemoveBootOption(
                    BootEntry->Options,
                    BCDE_OSLOADER_TYPE_OPTIONS_EDIT_ONE_TIME);
            }
        }

        //
        // The user has selected a boot entry.  Attempt to transfer execution
        // to the specified boot application.
        //

        Status = BmpTransferExecution(BootEntry, &LaunchReason, &Recover);
        ReturnStatus = Status;

        if (TrustedWimBootRegistered != FALSE) {

            //
            // BmpLaunchBootEntry can be called recursively by recovery
            // below. Make sure to unregister so other sceanrios aren't
            // unintentionally pulled into trusted wim boot.
            //

            BlFveRegisterBootEntryForTrustedWimBoot(NULL, FALSE);
            TrustedWimBootRegistered = FALSE;
        }

        //
        // If allowed, attempt recovery unless the application succeeded and
        // recovery was not explicitly requested.
        //

        if (AllowRecovery != FALSE) {
            if ((NT_SUCCESS(Status) || (Status == STATUS_CANCELLED)) &&
                (Recover == FALSE)) {

                return ReturnStatus;
            }

        } else {
            return ReturnStatus;
        }

        //
        // The reason for the recovery sequence being launched must be
        // determined.
        //
        // When launching has failed because unsealing is no longer
        // possible the user - if there is one - must be prompted to
        // reboot.
        //

        if ((Recover == FALSE) &&
            (LaunchReason != BSD_APP_LAUNCH_REASON_VSM_CANT_UNSEAL)) {

            LaunchReason = BSD_APP_LAUNCH_REASON_AUTO_FAILOVER;
        }

        //
        // If a recovery sequence exists, determine whether it should be
        // auto-launched.  Auto-launch the recovery sequence if the application
        // failed and it contains an auto-recovery option, or if recovery was
        // explicitly requested. Disable the explicit requests for recovery if a
        // recovery sequence does not exist.
        //

    BootFailure:
        if (BlBootOptionExists(BootEntry->Options,
                           BCDE_LIBRARY_TYPE_RECOVERY_SEQUENCE)) {

            AllowTrustedWimBoot = IS_TRUSTED_WIM_BOOT_LAUNCH_REASON(
                                    LaunchReason);

            if (IS_AUTO_RECOVERY_LAUNCH_REASON(LaunchReason) != FALSE) {
                Status = BlGetBootOptionBoolean(
                             BootEntry->Options,
                             BCDE_LIBRARY_TYPE_AUTO_RECOVERY_ENABLED,
                             &AutoRecover);

                if (NT_SUCCESS(Status)) {
                    Recover = AutoRecover;
                }
            }

        } else {
            Recover = FALSE;
        }

        //
        // If restart on failure option is set, then explicity disable recovery.
        // Assume not set on failure.
        //

        RestartOnFailure = FALSE;
        Status = BlGetBootOptionBoolean(BootEntry->Options,
                     BCDE_LIBRARY_TYPE_RESTART_ON_FAILURE,
                     &RestartOnFailure);

        if (!NT_SUCCESS(Status)) {
            RestartOnFailure = FALSE;
        }

        if (RestartOnFailure != FALSE) {
            Recover = FALSE;
        }

        //
        // Loop as long as the recovery sequence is meant to be retried based
        // on user input.
        //

        do {
            RetryRecovery = FALSE;

            //
            // Attempt recovery if it should be launched. If the recovery
            // sequence succeeds, then exit with status success. If it fails,
            // remove it from the boot entry's options so it is not tried again.
            //

            if (Recover != FALSE) {

                //
                // Register the boot entry for trusted WIM boot if this is
                // a supported launch reason.
                //

                if (AllowTrustedWimBoot != FALSE) {
                    BlFveRegisterBootEntryForTrustedWimBoot(BootEntry, TRUE);
                }

                Status = BmLaunchRecoverySequence(BootEntry, LaunchReason);

                //
                // After a recovery attempt (either successful or otherwise),
                // clear any trusted WIM boot registration and set
                // AllowTrustedWimBoot to false.
                //

                if (AllowTrustedWimBoot != FALSE) {
                    BlFveRegisterBootEntryForTrustedWimBoot(BootEntry, FALSE);
                    AllowTrustedWimBoot = FALSE;
                }

                if (NT_SUCCESS(Status)) {
                    ReturnStatus = STATUS_SUCCESS;
                    goto LaunchBootEntryEnd;

                } else {
                    BlRemoveBootOption(BootEntry->Options,
                                       BCDE_LIBRARY_TYPE_RECOVERY_SEQUENCE);
                }
            }

            //
            // If auto-recovery was not enabled or recovery failed, display a
            // screen describing the first error. If auto-recovery is not
            // enabled, the user can manually choose to recover. If the user
            // chooses to continue displaying the boot choices, convert the
            // failure status to a specific status code indicating that the
            // execution of the application was cancelled. This directs the
            // main loop to use the cached boot manager display state from
            // before the application execution.
            //

            if (BmErrorGetLast() == NULL) {
                BmFatalError(BM_ERROR_FAILURE_STATUS,
                             (ULONG_PTR)ReturnStatus,
                             0,
                             0);
            }

            Disposition = BmDisplayDumpError(BootEntry, LaunchReason);
            BmErrorPurge();
            if (Disposition == BmTryAgain) {
                RetryEntry = TRUE;

            } else if (Disposition == BmNextOs) {

                //
                // This can only be returned from a multi-boot system with
                // standard menu policies. Mark that the boot logic should skip
                // the current entry.
                //

                if ((ARGUMENT_PRESENT(BootEntryIndex) != FALSE) &&
                    (*BootEntryIndex != INVALID_BOOT_INDEX) &&
                    (BmpFailedBootEntries != NULL)) {

                    BmpFailedBootEntries[*BootEntryIndex] = TRUE;
                }

                ReturnStatus = STATUS_SUCCESS;

            } else if (Disposition == BmOsSelection) {
                ReturnStatus = STATUS_CANCELLED;

            } else if (Disposition == BmRecoverOem) {
                DataStoreHandle = NULL;
                Status = BmOpenDataStore(&DataStoreHandle);
                if (NT_SUCCESS(Status)) {
                    ReturnStatus = BmProcessCustomAction(DataStoreHandle, NULL);
                }

                if (DataStoreHandle != NULL) {
                    BmCloseDataStore(DataStoreHandle);
                    DataStoreHandle = NULL;
                }

            } else if (Disposition == BmRecover) {
                RetryRecovery = TRUE;
                Recover = TRUE;

                //
                // Modify the reason for launching the recovery sequence.
                //

                LaunchReason = BSD_APP_LAUNCH_REASON_MANUAL;

            } else if (Disposition == BmAdvancedOptions) {

                ASSERT(CHECK_FLAG(BootEntry->Attributes,
                                  BOOT_ENTRY_OS_LOADER) != FALSE);

                RetryEntry = TRUE;
                (VOID)BlAppendBootOptionBoolean(
                    BootEntry,
                    BCDE_OSLOADER_TYPE_ADVANCED_OPTIONS_ONE_TIME,
                    TRUE);

            } else if (Disposition == BmEditOptions) {

                ASSERT(CHECK_FLAG(BootEntry->Attributes,
                                  BOOT_ENTRY_OS_LOADER) != FALSE);

                RetryEntry = TRUE;
                (VOID)BlAppendBootOptionBoolean(
                    BootEntry,
                    BCDE_OSLOADER_TYPE_OPTIONS_EDIT_ONE_TIME,
                    TRUE);

            } else if ((Disposition == BmQuit) &&
                       (RestartOnFailure != FALSE)) {
                ReturnStatus = STATUS_UNSUCCESSFUL;
            } else {
                ReturnStatus = STATUS_CANCELLED;
            }

        } while (RetryRecovery != FALSE);

    } while (RetryEntry != FALSE);

LaunchBootEntryEnd:
    return ReturnStatus;
}

NTSTATUS
BmpLaunchCustomBootSequence (
    __in HANDLE DataStoreHandle,
    __in ULONG ElementName
    )

/*++

Routine Description:

    This routine executes a custom OEM boot sequence.

Arguments:

    DataStoreHandle - Supplies a handle to the boot configuration data store
        containing the custom boot sequence.

    ElementName - Supplies the BCD element name of the OEM custom boot
        sequence to execute.

Return Value:

   NT status code.

--*/

{

    PBOOT_APPLICATION_ENTRY *BootEntrySequence;
    ULONG BootSequenceCount;
    GUID *BootSequenceList;
    ULONG Index;
    ULONG SequenceCount;
    NTSTATUS Status;

    BootEntrySequence = NULL;
    BootSequenceList = NULL;
    SequenceCount = 0;

    //
    // Retrieve a list of the elements in the custom boot sequence.
    //

    Status = BlGetApplicationOptionGuidList(ElementName,
                                            &BootSequenceList,
                                            &BootSequenceCount);

    if (!NT_SUCCESS(Status)) {
        goto LaunchCustomBootSequenceEnd;
    }

    //
    // Create a list of boot objects for each element.
    //

    Status = BmGetBootSequence(DataStoreHandle,
                               BootSequenceList,
                               BootSequenceCount,
                               BOOT_ENTRY_SEQUENCE,
                               &BootEntrySequence,
                               &SequenceCount);

    if (!NT_SUCCESS(Status)) {
        goto LaunchCustomBootSequenceEnd;
    }

    //
    // Execute each of the custom boot objects.
    //

    for (Index = 0; Index < SequenceCount; Index++) {
        Status = BmpLaunchBootEntry(BootEntrySequence[Index],
                                    NULL,
                                    BSD_APP_LAUNCH_REASON_DEFAULT,
                                    FALSE);

        if (!NT_SUCCESS(Status)) {
            goto LaunchCustomBootSequenceEnd;
        }
    }

LaunchCustomBootSequenceEnd:
    if (BootEntrySequence != NULL) {
        for (Index = 0; Index < SequenceCount; Index += 1) {
            if (BootEntrySequence[Index] != NULL) {
                BlDestroyBootEntry(BootEntrySequence[Index]);
            }
        }

        BlMmFreeHeap(BootEntrySequence);
    }

    if (BootSequenceList != NULL) {
        BlMmFreeHeap(BootSequenceList);
    }

    return Status;
}

VOID
BmpProcessBadMemory (
    VOID
    )

/*++

Routine Description:

    This routine processes bad memory described in BCD, or WMD Results, such
    that it won't be used by down-level boot applications, or in the OS.

Arguments:

    None

Return Value:

    None

--*/

{

    ULONG BadPageCount;
    PPFN_NUMBER BadPfnArray;
    PFN_NUMBER BasePfn;
    ULONG Index;
    ULONGLONG PageCount;
    NTSTATUS Status;
    TIME_FIELDS TestTime;
    BOOLEAN WmdResultFound;
    GUID WmdResultGUID = MT_RESULT_GUID;
    PMT_RESULT_HEADER WmdResultHeader;
    ULONG WmdResultIndex;
    GENERIC_BUFFER WmdResults;
    TIME_FIELDS WmdResultTime;

    //
    // Test whether or not any WMD Results even exist.
    //

    BlInitGenericBuffer(&WmdResults, NULL, 0);
    Status = BlPdQueryData(NULL, &WmdResultGUID, 0, &WmdResults);
    if (Status != STATUS_BUFFER_TOO_SMALL) {
        return;
    }

    //
    // Allocate the largest result buffer so we can simply poll all the
    // WMD Results looking for the most recent one.
    //

    WmdResultHeader = BlMmAllocateHeap(MT_RESULT_ALLOCATION_SIZE);
    if (WmdResultHeader == NULL) {
        return;
    }

    BlInitGenericBuffer(&WmdResults,
                        WmdResultHeader,
                        MT_RESULT_ALLOCATION_SIZE);

    //
    // Iterate through all the WMD Results, and save the index of the most
    // recent result.
    //

    Index = 0;
    WmdResultIndex = 0;
    WmdResultFound = FALSE;
    do {
        Status = BlPdQueryData(NULL, &WmdResultGUID, Index, &WmdResults);

        //
        // This is the first WMD result, save its TestTime,
        //

        if (NT_SUCCESS(Status)) {
            if (WmdResultFound == FALSE) {
                WmdResultFound = TRUE;
                WmdResultIndex = Index;
                WmdResultTime = WmdResultHeader->TestTime;

                ASSERT(Index == 0); // Assert correct behavior of BapdpGet()

                //
                // We found another insance of WMD result, test whether its
                // more recent.
                //

            } else {
                TestTime = WmdResultHeader->TestTime;
                if (BmpIsTime1MoreRecentThanTime2(&TestTime,
                                                  &WmdResultTime)) {

                    //
                    // These results are the most recent, save info.
                    //

                    WmdResultIndex = Index;
                    WmdResultTime = TestTime;
                }
            }
        }

        Index++;
    } while (NT_SUCCESS(Status));

    ASSERT(WmdResultFound == TRUE);

    if (WmdResultIndex != (Index - 1)) {

        //
        // The current result buffer is not the most recent, fetch
        // the desired result.
        //

        Status = BlPdQueryData(NULL,
                               &WmdResultGUID,
                               WmdResultIndex,
                               &WmdResults);

        if (!NT_SUCCESS(Status)) {
            goto BmpProcessBadMemoryEnd;
        }

        ASSERT(RtlEqualMemory(&WmdResultTime,
                              &WmdResultHeader->TestTime,
                              sizeof(TIME_FIELDS)));
    }

    //
    // Check if WMD Results contain bad pages.
    //

    BadPageCount = WmdResultHeader->BadPageCount;
    if (BadPageCount > 0) {
        BadPfnArray = Add2Ptr(WmdResultHeader,
                              WmdResultHeader->BadPagePfnArrayOffset);

        //
        // For each contiguous run of bad pages, attempt to allocate the
        // physical memory region so it cannot be used by other boot
        // applications, or the OS.
        //

        do {
            PageCount = BmpDeterminePfnRun(BadPfnArray,
                                           BadPageCount,
                                           &BasePfn);

            BmpAllocateBadMemoryRegion(BasePfn, PageCount);
            BadPfnArray += PageCount;
            BadPageCount -= (ULONG)PageCount;

        } while (BadPageCount > 0);
    }

BmpProcessBadMemoryEnd:

    BlMmFreeHeap(WmdResultHeader);
    return;
}

VOID
BmpProcessBootEntry (
    __in HANDLE DataStoreHandle,
    __in PBOOT_APPLICATION_ENTRY BootEntry,
    __out PBOOLEAN ReturnToFirmware
    )

/*++

Routine Description:

    This routine processes BCD and local user input in the case where,
    ordinarily, the given boot entry would be booted by default without
    displaying the boot menu. BCD and user input can force the boot menu,
    initiate OEM custom actions, exit the boot manager, or launch an advanced
    menu from the OS loader or the OS.

Arguments:

    DataStoreHandle - Supplies a handle to the BCD store.

    BootEntry - Supplies a pointer to the boot entry that is to be processed.

    ReturnToFirmware - Supplies a pointer to a variable that receives TRUE if
        the boot manager should return to the firmware instead of launching an
        application. Otherwise, it receives FALSE.

Return Value:

    None.

--*/

{

    ULONGLONG BootMenuPolicy;
    BOOT_MENU_STATUS BootStatus;
    BOOLEAN DisplayBootMenu;

    *ReturnToFirmware = FALSE;
    BootMenuPolicy = BmGetBootMenuPolicy(BootEntry);
    if (BootMenuPolicy == MenuPolicyStandard) {
        goto ProcessBootEntryEnd;
    }

    //
    // Read key board input. If no keys were pressed, return.
    //

    BmDisplayGetBootMenuStatus(&BootStatus);
    if (BootStatus.AnyKey == 0) {
        goto ProcessBootEntryEnd;
    }

    //
    // Indicate that the boot manager should return to the firmware if the user
    // requested to exit.
    //

    if (BootStatus.Exit != 0) {
        *ReturnToFirmware = TRUE;
        BmpDisplayBootMenu = FALSE;
        goto ProcessBootEntryEnd;
    }

    //
    // Process the OEM actions if the OEM key was pressed.
    //

    if (BootStatus.OemAction != 0) {
        BmProcessCustomAction(DataStoreHandle, &BootStatus.OemKey);
        goto ProcessBootEntryEnd;
    }

    //
    // Get key input for OS loader entries.
    //

    BmDisplayGetOsLoaderKeyInput(&BootStatus, BootEntry, &DisplayBootMenu);

    //
    // Only update the display boot menu status based on user input if there
    // was not a BCD override.
    //

    if (BlApplicationOptionExists(
            BCDE_BOOTMGR_TYPE_DISPLAY_BOOT_MENU) == FALSE) {

        BmpDisplayBootMenu = DisplayBootMenu;
    }

ProcessBootEntryEnd:
    return;
}

NTSTATUS
BmpUpdateApplicationOptions (
    __in HANDLE DataStoreHandle
    )

/*++

Routine Description:

    This routine updates the options of the running application with those
    stored in the configuration data store.

Arguments:

    DataStoreHandle - Supplies a handle to the boot configuration data store.

Return Value:

    STATUS_SUCCESS if the options are updated successfully.

    An error status code if the application option list could not be retrieved.

--*/

{

    NTSTATUS Status;
    PBOOT_ENTRY_OPTION StoredOptions;

    //
    // Get the option list for this application.
    //

    Status = BmGetOptionList(DataStoreHandle,
                             &BmApplicationIdentifier,
                             &StoredOptions);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Append these options to the application boot entry.
    //

    Status = BlAppendApplicationOptions(StoredOptions);
    BlMmFreeHeap(StoredOptions);
    return Status;
}

NTSTATUS
BmpCreateDevices (
    __inout PBOOT_APPLICATION_ENTRY BootEntry
    )

/*++

Routine Description:

    This routine iterates through the options for the given boot entry, creating
    any device format option that requires this step.  This has the result that
    the options list for an application on input will not require any creation
    steps in that application.

Arguments:

    BootEntry - Supplies a pointer to the boot entry to process.

Return Value:

    STATUS_SUCCESS if the creation succeeds.

    An error status code if creating a device or appending a newly created
    device to the options list fails.

--*/

{

    PBOOT_ENTRY_OPTION AssociatedOptions;
    PBOOT_ENVIRONMENT_DEVICE Device;
    PBCDE_DEVICE DeviceElement;
    PBOOT_ENVIRONMENT_DEVICE DeviceIdentifier;
    NTSTATUS LocalStatus;
    ULONG NextOffset;
    PBOOT_ENTRY_OPTION Option;
    PBOOT_ENVIRONMENT_DEVICE ParentDevice;
    NTSTATUS Status;

    NextOffset = 0;
    do {

        //
        // Read the current option, skipping invalid options, options that are
        // not devices, and device options that do not need to be created.
        //

        Option = Add2Ptr(BootEntry->Options, NextOffset);
        NextOffset = Option->NextOptionOffset;
        if (Option->Invalid != FALSE) {
            continue;
        }

        if (GET_BCDE_DATA_FORMAT(Option->Type) != BCDE_FORMAT_DEVICE) {
            continue;
        }

        DeviceElement = Add2Ptr(Option, Option->DataOffset);
        if (CHECK_FLAG(DeviceElement->Device.Flags,
                       DEVICE_FLAG_IDENTIFIER_UNSPECIFIED) == FALSE) {

            continue;
        }

        //
        // Create the device, constructing a new identifier for the fully
        // created device.
        //

        AssociatedOptions = NULL;
        if (Option->AssociatedOptionsOffset != 0) {
            AssociatedOptions = Add2Ptr(Option,
                                        Option->AssociatedOptionsOffset);
        }

        Status = BlDeviceCreate(&DeviceElement->Device,
                                AssociatedOptions,
                                &DeviceIdentifier);

        //
        // If ramdisk device creation failed, try to re-enumerate the device
        // containing the ramdisk source, and retry the creation.
        //

        if ((Status == STATUS_NOT_FOUND) &&
            (DeviceElement->Device.DeviceType == DEVICE_BLOCK_IO) &&
            (DeviceElement->Device.u.BlockIo.BlockIoType == BLOCK_IO_RAMDISK)) {

            ParentDevice =
                BlGetDevice(&DeviceElement->Device.u.BlockIo.u.RamDisk.Source.SourcePath);

            LocalStatus = BlFwEnumerateDevice(ParentDevice);
            if (NT_SUCCESS(LocalStatus)) {
                Status = BlDeviceCreate(&DeviceElement->Device,
                                        AssociatedOptions,
                                        &DeviceIdentifier);
            }
        }

        if (!NT_SUCCESS(Status)) {
            goto DeviceCreateFailure;
        }

        //
        // Remove the existing option and create a new one of the same type
        // containing the new device identifier.  Note that the append will
        // free the existing options list, so the option cannot be accessed
        // after these calls without requerying it from the boot entry.
        //

        BlRemoveBootOption(BootEntry->Options, Option->Type);
        Status = BlAppendBootOptionDevice(BootEntry,
                                          Option->Type,
                                          DeviceIdentifier,
                                          AssociatedOptions);

        BlMmFreeHeap(DeviceIdentifier);
        if (!NT_SUCCESS(Status)) {
            goto DeviceCreateFailure;
        }

    } while (NextOffset != 0);

    return STATUS_SUCCESS;

DeviceCreateFailure:

    ASSERT(!NT_SUCCESS(Status));

    //
    // Check if this is a ramdisk device failure caused by not enough
    // memory, and output a specific error screen in that case.  Otherwise,
    // display a general device failure message.
    //

    Device = &DeviceElement->Device;
    if ((Device->u.BlockIo.BlockIoType == BLOCK_IO_RAMDISK) &&
        (Status == STATUS_NO_MEMORY)) {

        BmFatalError(BM_ERROR_RAMDISK_MEMORY_FAILURE,
                     (ULONG_PTR)Status,
                     0,
                     0);

    } else {
        BmFatalError(BM_ERROR_BOOT_DEVICE_FAILURE,
                     (ULONG_PTR)Status,
                     0,
                     0);
    }

    return Status;
}

NTSTATUS
BmpCheckUnsealingEligibility (
    __in PBOOT_APPLICATION_ENTRY BootEntry
    )

/*++

Routine Description -

    This routine determines whether the boot entry specified
    requires the ability to perform unsealing in boot and, if it
    does, whether that ability is still present.

Arguments:

    BootEntry - The boot entry Over which evaluation is performed.

Return Value:

    STATUS_SUCCESS - The entry can be launched.

    STATUS_TOO_LATE - The entry cannot be launched because it
        requires the ability to unseal and the boot environment
        is finalized.

--*/

{

    ULONG BootEntryCount;
    PBOOT_APPLICATION_ENTRY *BootEntryList;
    HANDLE DataStoreHandle;
    ULONG Index;
    ULONGLONG LaunchType;
    GUID ResumeEntryGuid;
    NTSTATUS Status;

    BootEntryCount = 0;
    BootEntryList = NULL;
    DataStoreHandle = NULL;
    Status = STATUS_SUCCESS;

    if (BlSiEnvironmentFinalized() == FALSE) {
        goto CheckUnsealingEligibilityEnd;
    }

    if (CHECK_FLAG(BootEntry->Attributes,
                   BOOT_ENTRY_OS_LOADER) != FALSE) {

        //
        // Specified entry is an OS Loader.  Check to see if
        // it requires unsealing ability by looking at the
        // VSM Launch Type.
        //

        Status = BlGetBootOptionInteger(BootEntry->Options,
                            BCDE_OSLOADER_TYPE_VSM_LAUNCH_TYPE,
                            &LaunchType);

        if ((NT_SUCCESS(Status)) && (LaunchType == VsmLaunchAuto)) {
            Status = STATUS_TOO_LATE;
            goto CheckUnsealingEligibilityEnd;
        }

    } else if (CHECK_FLAG(BootEntry->Attributes,
                          BOOT_ENTRY_OS_RESUME_LOADER) != FALSE) {

        //
        // Specified entry is a Resume Loader.  Look through the
        // data store for an OS loader associated with this resume
        // loader and check the VSM launch type on that OS Loader.
        //

        Status = BmOpenDataStore(&DataStoreHandle);
        if (!NT_SUCCESS(Status)) {
            goto CheckUnsealingEligibilityEnd;
        }

        Status = BmEnumerateBootEntries(DataStoreHandle,
                                &BootEntryList,
                                &BootEntryCount);
        if (!NT_SUCCESS(Status)) {
            goto CheckUnsealingEligibilityEnd;
        }

        for (Index = 0; Index < BootEntryCount; Index += 1) {
            if ((BootEntryList[Index] != NULL) &&
                (CHECK_FLAG(BootEntryList[Index]->Attributes,
                                BOOT_ENTRY_OS_LOADER) != FALSE)) {

                Status = BlGetBootOptionGuid(BootEntryList[Index]->Options,
                                 BCDE_OSLOADER_TYPE_ASSOCIATED_RESUME_OBJECT,
                                 &ResumeEntryGuid);

                if ((NT_SUCCESS(Status)) &&
                    (IsEqualGUID(&ResumeEntryGuid,
                                 &BootEntry->Identifier) != FALSE)) {

                    // The specified entry is a resume object for the currently
                    // enumerated boot loader, check that loader.

                    Status = BlGetBootOptionInteger(BootEntryList[Index]->Options,
                                        BCDE_OSLOADER_TYPE_VSM_LAUNCH_TYPE,
                                        &LaunchType);

                    if ((NT_SUCCESS(Status)) &&
                        (LaunchType == VsmLaunchAuto)) {
                            Status = STATUS_TOO_LATE;
                            goto CheckUnsealingEligibilityEnd;
                    }

                    break;
                }
            }
        }
   }

CheckUnsealingEligibilityEnd:

    if (BootEntryList != NULL) {
        for (Index = 0; Index < BootEntryCount; Index += 1) {
            if (BootEntryList[Index] != NULL) {
                BlDestroyBootEntry(BootEntryList[Index]);
            }
        }

        BlMmFreeHeap(BootEntryList);
    }

    if (DataStoreHandle != NULL) {
        BmCloseDataStore(DataStoreHandle);
    }

    return Status;
}

NTSTATUS
BmpTransferExecution (
    __inout PBOOT_APPLICATION_ENTRY BootEntry,
    __inout PULONG LaunchReason,
    __out PBOOLEAN Recover
    )

/*++

Routine Description:

    Provided a boot entry, this routine will attemp to load and transfer
    execution to the OS Loader / Boot Environment Application pointed to by
    the boot entry.

Arguments:

    BootEntry - The boot entry selected by the user.

    LaunchReason - Supplies a pointer to the reason for the application
        launch. It can receive a reason to launch the next
        application. Possible values:

        BSD_APP_LAUNCH_REASON_DEFAULT - User selected the entry or it was the
            default boot entry.

        BSD_APP_LAUNCH_REASON_MANUAL - User chose system recovery options on the
            advanced OSLOADER menu, or a boot application requested that its
            recovery sequence be executed.

        BSD_APP_LAUNCH_REASON_AUTO_FAILOVER - The boot manager decided to launch
            this entry to recover from a boot failure.

        BSD_APP_LAUNCH_REASON_FVE_RECOVERY - The boot manager decided to launch
            this entry to recovery from an FVE error.

        BSD_APP_LAUNCH_REASON_MULTI_BOOT_REQUESTED - The boot manager decided
            to launch this entry to display the multi boot menu.

            N.B. This is only used for launching WinRE to display multi-boot on
                 touch-only devices in an FVE recovery corner-case.

        BSD_APP_LAUNCH_RECOVERY_BAD_BOOT - The application returned to the boot
            manager indicating that recovery should be launched because of bad
            boot attempts.

        BSD_APP_LAUNCH_RECOVERY_BAD_SHUTDOWN - The application returned to the
            boot manager indicating that recovery should be launched because of
            bad shutdown attempts.

        BSD_APP_LAUNCH_REASON_VSM_CANT_UNSEAL - The application cannot be launched
            because it requires the ability to unseal data such as VSM keys
            and that ability no longer exists.

    Recover - Supplies a pointer to a variable that receives a boolean
        indicating whether the recovery sequence is to be executed.  This is
        correctly set by this routine regardless of the status code returned
        from the routine.  It is legal to interpret this flag even if the
        routine status code indicates failure.

Return Value:

    STATUS_NOT_SUPPORTED if the selected image type is unsupported by the
        current executing environment.

    STATUS_INVALID_PARAMETER if the size of the file path structure is
        smaller than the required size of a Boot Environment Device Identifier.

    STATUS_IO_DEVICE_ERROR if the device could not be opened.

    STATUS_INVALID_FILE if the loader file could not be opened.

    Otherwise, the status returned from the boot application.  The application
    may not return, in which case there is no return value.

--*/

{

    BOOLEAN AdvancedOptions;
    PBOOT_ENVIRONMENT_DEVICE AppDevice;
    PWCHAR AppPath;
    BOOT_APPLICATION_RETURN_ARGUMENTS BaReturnArgs;
    ULONGLONG BootMenuPolicy;
    GUID Identifier;
    ULONG ImageHandle;
    NTSTATUS LocalStatus;
    LARGE_INTEGER StartTime;
    NTSTATUS Status;

    *Recover = FALSE;

    //
    // Retrieve the application path in case of error.
    //

    AppPath = NULL;
    LocalStatus = BlGetBootOptionString(BootEntry->Options,
                                        BCDE_LIBRARY_TYPE_APPLICATION_PATH,
                                        &AppPath);

    if (!NT_SUCCESS(LocalStatus)) {
        AppPath = NULL;
    }

    //
    // If this boot entry represents a network soft reboot, do this.
    //
    // N.B. The soft reboot should not return when successful.
    //

    if (CHECK_FLAG(BootEntry->Attributes, BOOT_ENTRY_RESTART_LOADER)) {
        Status = BlNetSoftReboot(BootEntry);

        ASSERT(Status != STATUS_SUCCESS);

        goto TransferExecutionEnd;
    }

    do {

        RtlCopyMemory(&Identifier, &BootEntry->Identifier, sizeof(GUID));

        //
        // Load the boot environment application into memory.  If the load
        // fails with status not found, attempt to reenumerate the device, and
        // if successful, try to load the image once more.
        //

        StartTime = BlTimeQueryPerformanceCounter(NULL);
        Status = BlImgLoadBootApplication(BootEntry, NULL, NULL, &ImageHandle);
        if (Status == STATUS_NOT_FOUND) {
            LocalStatus = BlGetBootOptionDevice(
                              BootEntry->Options,
                              BCDE_LIBRARY_TYPE_APPLICATION_DEVICE,
                              &AppDevice,
                              NULL);

            if (NT_SUCCESS(LocalStatus)) {
                LocalStatus = BlFwEnumerateDevice(AppDevice);
                if (NT_SUCCESS(LocalStatus)) {
                    Status = BlImgLoadBootApplication(BootEntry,
                                                      NULL,
                                                      NULL,
                                                      &ImageHandle);
                }
            }
        }

        BmpLogEtwApplicationScenarioTime(&BOOT_APPLICATION_LOAD_TIME,
                                         &Identifier,
                                         StartTime);

        //
        // Handle returns from FVE load failure. If the load is cancelled, this
        // indicates that the user requested the multi-boot menu. Decide
        // whether or not to launch recovery for the multi-boot menu based on
        // the menu policy of the application that was being loaded. For other
        // recovery errors, launch the recovery sequence. If EMS is enabled,
        // send the user to the legacy boot menu.
        //

        if (Status == STATUS_CANCELLED) {
            BootMenuPolicy = BmGetBootMenuPolicy(BootEntry);
            if ((BootMenuPolicy == MenuPolicyStandard) &&
                (BlBootOptionExists(BootEntry->Options,
                                    BCDE_LIBRARY_TYPE_RECOVERY_SEQUENCE))) {

                *LaunchReason = BSD_APP_LAUNCH_REASON_MULTI_BOOT_REQUESTED;
                *Recover = TRUE;
            }

            goto TransferExecutionEnd;
        }

        if (Status == STATUS_FVE_LOCKED_VOLUME) {
            *LaunchReason = BSD_APP_LAUNCH_REASON_FVE_RECOVERY;
            *Recover = TRUE;
            goto TransferExecutionEnd;
        }

        if (!NT_SUCCESS(Status)) {
            BmFatalError(BM_ERROR_BOOTAPP_LOAD_FAILURE,
                         (ULONG_PTR)AppPath,
                         Status,
                         0);

            goto TransferExecutionEnd;
        }

        //
        // Check whether the boot application is able to understand
        // and requires the ability to peform unsealing.  If it does and
        // the environment is capped, then fail the launch.
        //
        // If this fails, don't trigger recovery, prompt to reboot
        // in order to clear TPM PCRs.
        //

        Status = BmpCheckUnsealingEligibility(BootEntry);
        if (Status == STATUS_TOO_LATE) {

            BmFatalError(BM_ERROR_UNSEALING_NOT_POSSIBLE,
                         (ULONG_PTR)Status,
                         0,
                         0);

            *LaunchReason = BSD_APP_LAUNCH_REASON_VSM_CANT_UNSEAL;
            *Recover = FALSE;
            goto TransferExecutionEnd;
        }

        //
        // Attempt to transfer execution to the loaded image.
        //

        RtlZeroMemory(&BaReturnArgs, sizeof(BOOT_APPLICATION_RETURN_ARGUMENTS));

        //
        // Log the application launch event.
        //

        BmpLogApplicationLaunchEvent(&Identifier, *LaunchReason, AppPath);

        //
        // Transfer control to the boot application.
        //

        StartTime = BlTimeQueryPerformanceCounter(NULL);
        Status = BlImgStartBootApplication(ImageHandle,
                                           IMAGE_FLAGS_NONE,
                                           &BaReturnArgs);

        BmpLogEtwApplicationScenarioTime(&BOOT_APPLICATION_EXECUTION_TIME,
                                         &Identifier,
                                         StartTime);

        //
        // Notify FVE of the return from the application.
        //

        BlFveSecureBootCheckpointAppReturn(BootEntry, &BaReturnArgs);

        //
        // Log the return from application event.
        //

        BmpLogApplicationReturnEvent(&Identifier, &BaReturnArgs);

        //
        // If the application returns control to the boot manager, free up the
        // image's memory and return to the caller.
        //

        BlImgUnloadBootApplication(ImageHandle);

        //
        // It is possible that the boot application wants bootmgr to reload
        // the application and call the application again. One example is if
        // UI was vetoed for security reasons, in which case the process of
        // reloading the application will prompt for credentials.
        //

    } while (Status == STATUS_RESTART_BOOT_APPLICATION);

    //
    // Attempt to retrieve any error information passed back from the
    // application.
    //

    if (!NT_SUCCESS(Status)) {
        BmErrorGetData();
    }

    //
    // Return an indication of whether the application is requesting a recovery
    // sequence to be run. Mark the launch reason for recovery based on whether
    // or not the user speficially requested it.
    //

    if (CHECK_FLAG(BaReturnArgs.Flags, RETURN_ARGUMENT_LAUNCH_RECOVERY)) {

        //
        // Legacy applications do not make a distinction between manually
        // selected recovery and automatic recovery. Check to see if the
        // advanced options menu was displayed to make the decision for them.
        //

        LocalStatus = BlGetBootOptionBoolean(
                          BootEntry->Options,
                          BCDE_LIBRARY_TYPE_DISPLAY_ADVANCED_OPTIONS,
                          &AdvancedOptions);

        if (!NT_SUCCESS(LocalStatus)) {
            AdvancedOptions = FALSE;
        }

        if (AdvancedOptions != FALSE) {
            *LaunchReason = BSD_APP_LAUNCH_REASON_MANUAL;

        } else {
            *LaunchReason = BSD_APP_LAUNCH_REASON_AUTO_FAILOVER;
        }

        *Recover = TRUE;

    } else if (CHECK_FLAG(BaReturnArgs.Flags,
                          RETURN_ARGUMENT_LAUNCH_RECOVERY_MANUAL)) {

        *LaunchReason = BSD_APP_LAUNCH_REASON_MANUAL;
        *Recover = TRUE;

    } else if (CHECK_FLAG(BaReturnArgs.Flags,
                          RETURN_ARGUMENT_LAUNCH_RECOVERY_FOR_BAD_BOOT)) {

        *LaunchReason = BSD_APP_LAUNCH_REASON_BAD_BOOT;
        *Recover = TRUE;

    } else if (CHECK_FLAG(BaReturnArgs.Flags,
                          RETURN_ARGUMENT_LAUNCH_RECOVERY_FOR_BAD_SHUTDOWN)) {

        *LaunchReason = BSD_APP_LAUNCH_REASON_BAD_SHUTDOWN;
        *Recover = TRUE;

    } else if (CHECK_FLAG(BaReturnArgs.Flags,
                          RETURN_ARGUMENT_LAUNCH_RECOVERY_FOR_UPDATE)) {

        *LaunchReason = BSD_APP_LAUNCH_REASON_UPDATE;
        *Recover = TRUE;

    } else if (CHECK_FLAG(BaReturnArgs.Flags,
                          RETURN_ARGUMENT_NO_PAE_SUPPORT)) {

        BmFatalError(BM_ERROR_NO_PAE_SUPPORT, Status, 0, 0);
    }

TransferExecutionEnd:
    if (AppPath != NULL) {
        BlMmFreeHeap(AppPath);
    }

    return Status;
}

VOID
BmpInitializeBootStatusDataLog (
    VOID
    )

/*++

Routine Description:

    This routine will initialize boot status data (BSD) logging.  No result
    is returned, since no action can be taken if this fails.

Arguments:

    None.

Return Value:

    None.

--*/

{

    ULONG InitializationFlags;
    PBOOT_ENVIRONMENT_DEVICE LogDevice;
    PBOOT_ENVIRONMENT_DEVICE LogDeviceOverride;
    PWCHAR LogPath;
    BOOLEAN LogPathAllocated;
    PWSTR LogPathOverride;
    BOOLEAN PreserveDataOverride;
    NTSTATUS Status;

    LogDeviceOverride = NULL;
    LogPathAllocated = FALSE;
    LogPathOverride = NULL;

    //
    // Obtain any override values for the logging device.
    //

    Status = BlGetApplicationOptionDevice(BCDE_LIBRARY_TYPE_BSD_LOG_DEVICE,
                                          &LogDeviceOverride,
                                          NULL);

    //
    // If is an override present, use that, otherwise use the boot device.
    //

    if (NT_SUCCESS(Status)) {
        LogDevice = LogDeviceOverride;

    } else {
        LogDevice = BlGetBootDevice();
    }

    //
    // Obtain any override values for the logging path.
    //

    Status = BlGetApplicationOptionString(BCDE_LIBRARY_TYPE_BSD_LOG_PATH,
                                          &LogPathOverride);

    //
    // If is an override present, use that, otherwise use the same path as the
    // boot manager binary.
    //

    if (NT_SUCCESS(Status)) {
        LogPath = LogPathOverride;
        LogPathAllocated = TRUE;

    } else {
        LogPath = BmFwGetBsdLogPath(&LogPathAllocated);
    }

    //
    // Determine whether the flag is set to preserve log data at initialization.
    //

    Status = BlGetApplicationOptionBoolean(BCDE_LIBRARY_TYPE_BSD_PRESERVE_LOG,
                                           &PreserveDataOverride);

    //
    // If is an override present, use that, otherwise use the same path as the
    // boot manager binary.
    //

    if (NT_SUCCESS(Status) && (PreserveDataOverride != FALSE)) {
        InitializationFlags = BSD_INIT_FLAGS_PRESERVE_LOG;

    } else {
        InitializationFlags = 0;
    }

    //
    // Initialize the logging engine.  For the boot manager, the offset to the
    // start of the log data section of the file is zero.
    //

    BlBsdInitializeLog(BCD_APPLICATION_TYPE_WINDOWS_BOOT_MANAGER,
                       LogDevice,
                       LogPath,
                       0,
                       InitializationFlags);

    //
    // Clean up memory allocations if required.
    //

    if (LogDeviceOverride != NULL) {
        BlMmFreeHeap(LogDeviceOverride);
    }

    if ((LogPathAllocated != FALSE) && (LogPath != NULL)) {
        BlMmFreeHeap(LogPath);
    }
}

VOID
BmpLogApplicationLaunchEvent (
    __in GUID* ApplicationIdentifier,
    __in ULONG LaunchReason,
    __in_opt PCWSTR ApplicationPath
    )

/*++

Routine Description:

    This routine will log a boot status data logging event in response to a
    boot application being launched.

Arguments:

    ApplicationIdentifier - The object identifier (if any) of the application
        being launched.

    LaunchReason - Supplies the reason for the application launch:

        BSD_APP_LAUNCH_REASON_DEFAULT - User selected the entry or it was the
            default boot entry.

        BSD_APP_LAUNCH_REASON_MANUAL - User chose system recovery options on the
            advanced OSLOADER menu, or a boot application requested that its
            recovery sequence be executed.

        BSD_APP_LAUNCH_REASON_AUTO_FAILOVER - The boot manager decided to launch
            this entry to recover from a boot failure.

        BSD_APP_LAUNCH_REASON_FVE_RECOVERY - The boot manager decided to launch
            this entry to recovery from an FVE error.

        BSD_APP_LAUNCH_REASON_MULTI_BOOT_REQUESTED - The boot manager decided
            to launch this entry to display the multi boot menu.

            N.B. This is only used for launching WinRE to display multi-boot on
                 touch-only devices in an FVE recovery corner-case.

        BSD_APP_LAUNCH_RECOVERY_BAD_BOOT - The application returned to the boot
            manager indicating that recovery should be launched because of bad
            boot attempts.

        BSD_APP_LAUNCH_RECOVERY_BAD_SHUTDOWN - The application returned to the
            boot manager indicating that recovery should be launched because of
            bad shutdown attempts.

    ApplicationPath - The application path of the application being launched.

Return Value:

    None.

--*/

{

    PBSD_EVENT_LOGDATA_BOOTMGR_APP_LAUNCH LaunchEvent;
    ULONG LaunchEventSize;

    //
    // Log the application launch event.  Calculate the size of the log
    // entry according the the length of the application path.  If the
    // application path doesn't exist, then log the empty string.
    //

    if (ApplicationPath != NULL) {
        LaunchEventSize = (ULONG)
            (FIELD_OFFSET(BSD_EVENT_LOGDATA_BOOTMGR_APP_LAUNCH,
                         ApplicationPath) +
                ((wcslen(ApplicationPath) + 1) * sizeof(WCHAR)));


    } else {
        LaunchEventSize = sizeof(WCHAR) +
            FIELD_OFFSET(BSD_EVENT_LOGDATA_BOOTMGR_APP_LAUNCH,
                         ApplicationPath);
    }

    //
    // Allocate the memory, construct the event data, and log the event.
    //

    LaunchEvent = BlMmAllocateHeap(LaunchEventSize);
    if (LaunchEvent == NULL) {
        return;
    }

    RtlZeroMemory(LaunchEvent, LaunchEventSize);
    RtlCopyMemory(&LaunchEvent->ApplicationIdentifier,
                  ApplicationIdentifier,
                  sizeof(GUID));

    LaunchEvent->LaunchReason = LaunchReason;
    if (ApplicationPath != NULL) {
        wcscpy_s(&LaunchEvent->ApplicationPath[0],
                 wcslen(ApplicationPath) + 1,
                 ApplicationPath);
    }

    BlBsdLogEntry(BSD_EVENT_LEVEL_INFORMATION,
                  BSD_BOOTMGR_EVENT_APP_LAUNCH,
                  LaunchEvent,
                  LaunchEventSize);

    BlMmFreeHeap(LaunchEvent);
}

VOID
BmpLogApplicationReturnEvent (
    __in GUID* ApplicationIdentifier,
    __in PBOOT_APPLICATION_RETURN_ARGUMENTS BaReturnArgs
    )

/*++

Routine Description:

    This routine will log a boot status data logging event in response to a
    boot application being launched.

Arguments:

    ApplicationIdentifier - The object identifier (if any) of the application
        being launched.

    BaReturnArgs - Supplies structure containing status code returned by the
        application.

Return Value:

    None.

--*/

{

    BSD_EVENT_LOGDATA_BOOTMGR_APP_RETURN ReturnEvent = {0};

    ReturnEvent.Status = BaReturnArgs->ReturnStatus;
    RtlCopyMemory(&ReturnEvent.ApplicationIdentifier,
                  ApplicationIdentifier,
                  sizeof(GUID));

    BlBsdLogEntry(BSD_EVENT_LEVEL_INFORMATION,
                  BSD_BOOTMGR_EVENT_APP_RETURN,
                  &ReturnEvent,
                  sizeof(BSD_EVENT_LOGDATA_BOOTMGR_APP_RETURN));

    return;
}

VOID
BmpLogBootResolutions (
    VOID
    )

/*++

Routine Description:

    This routine logs an ETW event with the resolution used by the firmware and
    the resolution eventually used by the library.

Arguments:

    None.

Return Value:

    None.

--*/

{

    EVENT_DATA_DESCRIPTOR EventDataDescriptor[3];
    ULONG Format;
    ULONG Height;
    ULONG OriginalFormat;
    ULONG OriginalHeight;
    ULONG OriginalWidth;
    NTSTATUS Status;
    ULONG Width;

    Status = BlDisplayGetOriginalResolution(&OriginalWidth, &OriginalHeight);
    if (!NT_SUCCESS(Status)) {
        goto LogBootResolutionsEnd;
    }

    Status = BlDisplayGetOriginalFormat(&OriginalFormat);
    if (!NT_SUCCESS(Status)) {
        goto LogBootResolutionsEnd;
    }

    EventDataDescCreate(&EventDataDescriptor[0],
                        (PVOID)&OriginalWidth,
                        sizeof(ULONG));

    EventDataDescCreate(&EventDataDescriptor[1],
                        (PVOID)&OriginalHeight,
                        sizeof(ULONG));

    EventDataDescCreate(&EventDataDescriptor[2],
                        (PVOID)&OriginalFormat,
                        sizeof(ULONG));

    BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                  &BOOT_ENVIRONMENT_FIRMWARE_RESOLUTION,
                  NULL,
                  3,
                  EventDataDescriptor);

    Status = BlDisplayGetScreenResolution(&Width, &Height);
    if (!NT_SUCCESS(Status)) {
        goto LogBootResolutionsEnd;
    }

    Status = BlDisplayGetScreenFormat(&Format);
    if (!NT_SUCCESS(Status)) {
        goto LogBootResolutionsEnd;
    }

    EventDataDescCreate(&EventDataDescriptor[0],
                        (PVOID)&Width,
                        sizeof(ULONG));

    EventDataDescCreate(&EventDataDescriptor[1],
                        (PVOID)&Height,
                        sizeof(ULONG));

    EventDataDescCreate(&EventDataDescriptor[2],
                        (PVOID)&Format,
                        sizeof(ULONG));

    BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                  &BOOT_ENVIRONMENT_OS_RESOLUTION,
                  NULL,
                  3,
                  EventDataDescriptor);

LogBootResolutionsEnd:

    return;
}

VOID
BmpLogEtwTotalTime (
    )

/*++

Routine Description:

    This routine logs two ETW events; one that captures the total time spent
    in the boot manager, the other that records the timestamp when we first
    enter the boot manager.

Arguments:

    None.

Return Value:

    None.

--*/

{

    LARGE_INTEGER ApplicationEndTime;
    ULONGLONG BootMenuTimeTaken;
    EVENT_DATA_DESCRIPTOR EventDataDescriptor[1];
    LARGE_INTEGER Frequency;
    ULONGLONG FrequencyInMilliseconds;
    ULONGLONG PreBootTime;
    ULONGLONG TimeTaken;
    ULONGLONG UserTimeTaken;

    //
    // Calculate total time spent in the boot manager in milliseconds.
    //

    ApplicationEndTime = BlTimeQueryPerformanceCounter(&Frequency);
    FrequencyInMilliseconds = Frequency.QuadPart / 1000;

    // _ARM_WORKITEM_ real fix will be put in before this RIs
    if (FrequencyInMilliseconds == 0) {
        FrequencyInMilliseconds = 100;
    }

    TimeTaken = (ApplicationEndTime.QuadPart - ApplicationStartTime.QuadPart) /
                (FrequencyInMilliseconds);

    EventDataDescCreate(&EventDataDescriptor[0],
                        (PVOID)&TimeTaken,
                        sizeof(ULONGLONG));

    BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                  &BOOT_BOOTMGR_TIME,
                  NULL,
                  1,
                  EventDataDescriptor);

    //
    // Log the time spent displaying the boot menu
    //

    BootMenuTimeTaken = BootMenuTime.QuadPart / FrequencyInMilliseconds;

    //
    // Calculate the pre-boot time in milliseconds
    //

    PreBootTime = PostTime.QuadPart / FrequencyInMilliseconds;

    //
    // If the bootmgr fails to launch an app and returns, do not log the pre
    // boot time twice.
    //

    if (PostTimeLogged == FALSE) {
        EventDataDescCreate(&EventDataDescriptor[0],
                            (PVOID)&PreBootTime,
                            sizeof(ULONGLONG));

        BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                      &BOOT_PRE_BOOT_TIME,
                      NULL,
                      1,
                      &EventDataDescriptor[0]);

        PostTimeLogged = TRUE;
    }

    //
    // Log the time taken waiting for user input.
    //

    UserTimeTaken = BmFveUserInputTime.QuadPart / FrequencyInMilliseconds;
    EventDataDescCreate(&EventDataDescriptor[0],
                        (PVOID)&UserTimeTaken,
                        sizeof(ULONGLONG));

    BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                  &BOOT_USER_INPUT_TIME,
                  NULL,
                  1,
                  &EventDataDescriptor[0]);

#if !defined(PCAT)

    TraceLoggingWrite(BmBootProvider,
                      "BootMgrLaunch",
                      TraceLoggingLevel(TRACE_LEVEL_INFORMATION),
                      TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES),
                      TraceLoggingUInt64(TimeTaken, "BootMgrTime"),
                      TraceLoggingUInt64(PreBootTime, "PostTime"),
                      TraceLoggingUInt64(BootMenuTimeTaken, "BootMenuTime"),
                      TraceLoggingUInt64(UserTimeTaken, "UserTime"));

#endif

    return;
}

VOID
BmpLogEtwEvents (
    __in SI_EVENT_TYPE EventType,
    __in_opt PNOTIFICATION_EVENT_CONTEXT EventContext,
    __in_opt PVOID HandlerContext1,
    __in_opt PVOID HandlerContext2
    )

/*++

Routine Description:

    This routine creates all of the ETW events that happen when the boot
    manager hands control to another application.

Arguments:

    EventType - Supplies the type of event this function was called back for.

    EventContext - Supplies the context of the event.

    HandlerContext1 - Unused Parameter.

    HandlerContext2 - Unused Parameter.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(EventContext);
    UNREFERENCED_PARAMETER(HandlerContext1);
    UNREFERENCED_PARAMETER(HandlerContext2);

    //
    // Write out all ETW events that should get logged.
    //

    if (EventType == EVENT_APPLICATION_LAUNCHING) {
        BmpLogEtwTotalTime();
        BlLogEtwTotalIo();
    }
}

VOID
BmpLogEtwApplicationScenarioTime (
    __in PCEVENT_DESCRIPTOR ScenarioEventDescriptor,
    __in GUID* ApplicationIdentifier,
    __in LARGE_INTEGER StartTime
    )

/*++

Routine Description:

    This routine creates an ETW event indicating how long an application
    scenario took to execute.

Arguments:

    ScenarioEventDescriptor - Supplies the scenario that was executed.

    ApplicationIdentifier - Supplies the application.

    StartTime - Supplies when the application scenario started.

Return Value:

    None.

--*/

{

    LARGE_INTEGER EndTime;
    EVENT_DATA_DESCRIPTOR EventDataDescriptor[2];
    LARGE_INTEGER Frequency;
    ULONGLONG FrequencyInMilliseconds;
    ULONGLONG TimeTaken;

    EndTime = BlTimeQueryPerformanceCounter(&Frequency);
    FrequencyInMilliseconds = Frequency.QuadPart / 1000;
    TimeTaken = (EndTime.QuadPart - StartTime.QuadPart) /
        (FrequencyInMilliseconds);

    EventDataDescCreate(&EventDataDescriptor[0],
                        (PVOID)ApplicationIdentifier,
                        sizeof(GUID));

    EventDataDescCreate(&EventDataDescriptor[1],
                        (PVOID)&TimeTaken,
                        sizeof(ULONGLONG));

    BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                  ScenarioEventDescriptor,
                  NULL,
                  2,
                  EventDataDescriptor);
}

NTSTATUS
BmpSelfIntegrityCheck (
    VOID
    )

/*++

Routine Description:

    This function checks whether the running boot manager can be verified
    against a self-signed certificate.  On PC/AT machines, this verification
    is done using a copy of the image that has been loaded into memory by
    the startup module.  On other systems, verification is done by rereading
    the boot manager binary from disk.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS is returned if the integrity of the image was successfully
        verified or application boot options have disabled integrity checking.

    An error code is reutrned if the integrity of the image could not be
        verified.  In this case it is expected that the boot manager will
        take no further actions other than displaying an error screen.

--*/

{

    ULONG ImageFlags;
    BOOLEAN IntegrityChecksDisabled;
    NTSTATUS Status;

#if defined(_ARM_) || defined(_ARM64_)

    BOOLEAN ForceFipsCrypto;
    BOOLEAN SecureBootEnabled;

#endif

    //
    // Query the code integrity configuration elements from the option list
    // associated with the boot manager.
    //

    BlImgQueryCodeIntegrityBootOptions(BlGetApplicationEntry(),
                                       &IntegrityChecksDisabled,
                                       NULL,
                                       NULL);

    //
    // Do not make any attempt at code integrity initialization or self
    // verification if integrity checks are disabled in the boot option list.
    //

    if (IntegrityChecksDisabled != FALSE) {
        return STATUS_SUCCESS;
    }

#if defined(_ARM_) || defined(_ARM64_)

    //
    // On ARM machines, if secure boot is enabled and the
    // BCDE_LIBRARY_TYPE_FORCE_FIPS_CRYPTO option is FALSE, do not perform an
    // integrity check.
    //
    // When boot option queries fail, assume that secure boot is not enabled
    // and that the BCDE_LIBRARY_TYPE_FORCE_FIPS_CRYPT option is FALSE.
    //

    Status = BlSecureBootIsEnabled(&SecureBootEnabled);
    if (NT_SUCCESS(Status) && (SecureBootEnabled != FALSE)) {
        Status = BlGetApplicationOptionBoolean(
                    BCDE_LIBRARY_TYPE_FORCE_FIPS_CRYPTO,
                    &ForceFipsCrypto);

        if ((Status == STATUS_NOT_FOUND) ||
            (NT_SUCCESS(Status) && (ForceFipsCrypto == FALSE))) {

            Status = STATUS_SUCCESS;
            goto SelfIntegrityCheckExit;
        }
    } else if (SecureBootEnabled == FALSE){
        Status = STATUS_SUCCESS;
        goto SelfIntegrityCheckExit;
    }
#endif  // defined(_ARM_) || defined(_ARM64_)

    //
    // Integrity checks are enabled.  Verify the integrity of the running boot
    // manager, returning an error status if the integrity check fails.
    //

    ImageFlags = IMAGE_FLAGS_VALIDATE_IMAGE_HASH |
                 IMAGE_FLAGS_VALID_HASH_REQUIRED |
                 IMAGE_FLAGS_NO_SIGNATURE_EXPIRATION_CHECKS;

    Status = BmFwVerifySelfIntegrity(ImageFlags);
    if (!NT_SUCCESS(Status)) {
        goto SelfIntegrityCheckExit;
    }

SelfIntegrityCheckExit:
    return Status;
}

VOID
BmpProcessCustomActions (
    __in HANDLE DataStoreHandle
    )

/*++

Routine Description:

    This routine processes user input to initiate OEM custom actions.

Arguments:

    DataStoreHandle - Supplies a handle to the BCD store.

Return Value:

    None.

--*/

{

    BOOT_MENU_STATUS BootStatus;

    //
    // Read key board input. If no keys were pressed, return.
    //

    BmDisplayGetBootMenuStatus(&BootStatus);
    if (BootStatus.AnyKey == 0) {
        goto ProcessCustomActionsEnd;
    }

    //
    // Process the OEM actions if the OEM key was pressed.
    //

    if (BootStatus.OemAction != 0) {
        BmProcessCustomAction(DataStoreHandle, &BootStatus.OemKey);
        goto ProcessCustomActionsEnd;
    }

ProcessCustomActionsEnd:
    return;
}

VOID
BmpRegisterForTrustedWimBootIfWinReEntry (
    _In_ PBOOT_APPLICATION_ENTRY BootEntry,
    _Out_ PBOOLEAN Registered
    )

/*++

Routine Description:

    This routine registers a WinRE Associated OS boot entry for trusted
    WIM boot. First verify this is indeed WinRE, then find the Associated
    OS device by searching the recovery sequences of boot loader entries
    for an identifier matching that of the WinRE boot entry. Unlock the
    Associated OS device and register it for trusted Wim Boot.

Arguments:

    BootEntry - The potential WinRE application entry.

    Registered - TRUE if the Trusted WIM Boot registration occured.

Return Value:

    None.

--*/

{
    PBOOT_APPLICATION_ENTRY AssociatedOsEntry;
    BOOLEAN IsWinRE;
    NTSTATUS Status;
    GUID WinReIdentifier;

    AssociatedOsEntry = NULL;
    IsWinRE = FALSE;
    *Registered = FALSE;
    Status = STATUS_SUCCESS;

    //
    // Make sure this entry actually has an identifier.
    //

    if (CHECK_FLAG(BootEntry->Attributes,
                   BOOT_ENTRY_IDENTIFIER_UNSPECIFIED)) {
        goto BmpRegisterForTrustedWimBootIfWinReEntryEnd;
    }

    //
    // Is this BootEntry WinRE? If not do nothing.
    //

    Status = BlGetBootOptionBoolean(BootEntry->Options,
                                    BCDE_TEMPLATE_TYPE_RECOVERY_OS,
                                    &IsWinRE);
    if (!NT_SUCCESS(Status) || IsWinRE == FALSE) {

        //
        // Entry not found or IsWinRE is false.
        // Nothing to do.
        //

        goto BmpRegisterForTrustedWimBootIfWinReEntryEnd;
    }

    //
    // We have a WinRE boot entry, let's see if we can find an OS
    // loader that uses this WinRE as part of its recovery sequence.
    // Copy the Identifier GUID as it may be unaligned.
    //

    RtlCopyMemory(&WinReIdentifier, &BootEntry->Identifier, sizeof(GUID));

    Status = BmFindAssociatedOsEntry(&WinReIdentifier,
                                     &AssociatedOsEntry);
    if (!NT_SUCCESS(Status)) {
        goto BmpRegisterForTrustedWimBootIfWinReEntryEnd;
    }

    Status = BlFveUnlockAndRegisterForTrustedWimBoot(AssociatedOsEntry);
    if (!NT_SUCCESS(Status)) {
        goto BmpRegisterForTrustedWimBootIfWinReEntryEnd;
    }

    *Registered = TRUE;

BmpRegisterForTrustedWimBootIfWinReEntryEnd:

    if (AssociatedOsEntry != NULL) {
        BlMmFreeHeap(AssociatedOsEntry);
    }

    return;
}

