/*++

Copyright (c) Microsoft Corporation

Module Name:

    bootmgr.h

Abstract:

    Header file for the Windows Boot Manager.

Environment:

    Boot

Revision History:

--*/

#pragma once

//
// ------------------------------------------------------------------- Includes
//

#include <bootlib.h>
#include <wmdinfo.h>
#include <ntstrsafe.h>
#include "baerror.h"
#include "msg.h"
#include <inbv.h>
#include "bgfx.h"
#include <mincrypl.h>

//
// -------------------------------------------------------------------- Globals
//

//
// Indicates whether cached display state is to be shown.
//

extern BOOLEAN BmDisplayStateCached;
extern BOOLEAN BmpDisplayBootMenu;

//
// The identifier used to retrieve settings for this application from the
// configuration data file.
//

extern GUID BmApplicationIdentifier;

//
// Contains the total time spent waiting for user input at bitlocker
// display screens.
//

extern LARGE_INTEGER BmFveUserInputTime;

//
// Base path for SecureBoot and Platform Manifest paths
//

extern _Null_terminated_ const WCHAR g_BmFwSecureBootBasePath[];

//
// Path to Legacy Token File
//

extern _Null_terminated_ const WCHAR g_BmFwSecureBootLegacyFlightTokenPath[];

//
// ---------------------------------------------------------------- Definitions
//

typedef enum _BM_ERROR_HELP_TYPE {
    BmErrorHelpDefault,
    BmErrorHelpIo,
    BmErrorHelpHash
} BM_ERROR_HELP_TYPE, *PBM_ERROR_HELP_TYPE;

#define DEFAULT_BOOT_INDEX 0

typedef struct _INTERNAL_BOOT_ERROR {
    ULONG ErrorCode;
    NTSTATUS Status;
    ULONG SourceLine;
    PWCHAR ErrorMsg;
    PWCHAR FileName;
    BM_ERROR_HELP_TYPE HelpType;
    PCHAR SourceFile;
} INTERNAL_BOOT_ERROR, *PINTERNAL_BOOT_ERROR;

#define BM_ERROR_CONFIGURATION_DATA     0x00000001
#define BM_ERROR_BOOTAPP_LOAD_FAILURE   0x00000002
#define BM_ERROR_NO_VALID_OS_ENTRY      0x00000003
#define BM_ERROR_FAILURE_STATUS         0x00000004
#define BM_ERROR_BOOT_DEVICE_FAILURE    0x00000005
#define BM_ERROR_RAMDISK_MEMORY_FAILURE 0x00000006
#define BM_ERROR_INVALID_BCD_STORE      0x00000007
#define BM_ERROR_INVALID_BCD_ENTRY      0x00000008
#define BM_ERROR_NO_VALID_PXE_OS_ENTRY  0x00000009
#define BM_ERROR_NO_SECUREBOOT_POLICY   0x0000000A
#define BM_ERROR_NO_PAE_SUPPORT         0x0000000B
// #define BM_ERROR_BOOTINI_FAILURE     0x0000000C   //deprecated
#define BM_ERROR_UNSEALING_NOT_POSSIBLE 0x0000000D

typedef enum _BM_ERROR_DISPOSITION {
    BmContinue,
    BmQuit,
    BmRecover,
    BmRecoverOem,
    BmOsSelection,
    BmNextOs,
    BmTryAgain,
    BmAdvancedOptions,
    BmEditOptions
} BM_ERROR_DISPOSITION, *PBM_ERROR_DISPOSITION;

#if EFI

#define BM_REVOCATION_LIST_FILE         L"\\boot.stl"
#define BM_BSD_LOG_NAME                 L"\\bootstat.dat"
#define BM_DATA_STORE_FILE_NAME         L"\\BCD"
#define BM_BGFX_DIRECTORY               L"\\EFI\\Microsoft"
#define BM_DEFAULT_BOOT_DIRECTORY       L"\\EFI\\Microsoft\\Boot"
#define BM_DEFAULT_OS_LOADER_PATH       L"\\Windows\\System32\\winload.efi"
#define BM_DEFAULT_WINPE_OS_LOADER_PATH L"\\Windows\\System32\\boot\\winload.efi"
#define BM_DEFAULT_RESUME_LOADER_PATH   L"\\Windows\\System32\\winresume.efi"

#else

#define BM_BGFX_DIRECTORY               L""
#define BM_BSD_LOG_PATH                 L"\\Boot\\bootstat.dat"
#define BM_DATA_STORE_FILE_NAME         L"\\Boot\\BCD"
#define BM_DEFAULT_OS_LOADER_PATH       L"\\Windows\\System32\\winload.exe"
#define BM_DEFAULT_WINPE_OS_LOADER_PATH L"\\Windows\\System32\\boot\\winload.exe"
#define BM_DEFAULT_RESUME_LOADER_PATH   L"\\Windows\\System32\\winresume.exe"

#endif

typedef struct _BOOT_MENU_STATUS {
    struct {
        ULONG AnyKey : 1;
        ULONG AdvancedOptions : 1;
        ULONG BootOptions : 1;
        ULONG OemAction : 1;
        ULONG Exit : 1;
        ULONG Reserved : 27;
    };

    ULONG BootIndex;
    WCHAR OemKey;
} BOOT_MENU_STATUS, *PBOOT_MENU_STATUS;

typedef struct _INVALID_BCD_ENTRY_ERROR {
    PWSTR ObjectName;
    PWSTR Description;
    PWSTR ElementName;
    NTSTATUS Status;
    PWSTR ErrorMessage;
} INVALID_BCD_ENTRY_ERROR, *PINVALID_BCD_ENTRY_ERROR;

//
// Resource IDs
//

#define BM_SECUREBOOTPOLICY_RESOURCE_ID 1

//
// ----------------------------------------------------------------- Prototypes
//

NTSTATUS
BmMain (
    __in PBOOT_APPLICATION_PARAMETERS Parameters
    );

NTSTATUS
BmEnumerateBootEntries (
    __in HANDLE DataStoreHandle,
    __deref_out_ecount(*EntryCount) PBOOT_APPLICATION_ENTRY **EntryArrayOut,
    __out PULONG EntryCount
    );

NTSTATUS
BmEnumerateToolEntries (
    __in HANDLE DataStoreHandle,
    __deref_out_ecount(*EntryCount) PBOOT_APPLICATION_ENTRY **EntryArrayOut,
    __out PULONG EntryCount
    );

NTSTATUS
BmFwBootToFirmware (
    VOID
    );

BOOLEAN
BmFwBootToFirmwareSupported (
    VOID
    );

NTSTATUS
BmFwGetBcdFilePath (
    __deref_out_z PWCHAR *FilePath,
    __out PBOOLEAN FilePathAllocated
    );

PWCHAR
BmFwGetBgfxRootDirectory (
    __out PBOOLEAN RootDirectoryAllocated
    );

PWCHAR
BmFwGetBsdLogPath (
    __out PBOOLEAN FilePathAllocated
    );

PWCHAR
BmFwGetRevocationListPath (
    VOID
    );

VOID
BmFwInitializeBootDirectoryPath (
    VOID
    );

VOID
BmFwMemoryInitialize (
    VOID
    );

BOOLEAN
BmFwIsUnlockIdVariablePresent (
    VOID
    );

NTSTATUS
BmFwSetUnlockIdVariable (
    VOID
    );

NTSTATUS
BmGetBootSequence (
    __in HANDLE DataStoreHandle,
    __in_ecount(Count) GUID *IdentifierSequence,
    __in ULONG Count,
    __in ULONG Attributes,
    __deref_out_ecount(*SequenceCount)
        PBOOT_APPLICATION_ENTRY **BootEntrySequence,
    __out PULONG SequenceCount
    );

NTSTATUS
BmGetEntryDescription (
    __in HANDLE DataStoreHandle,
    __in GUID *Identifier,
    __out PBCD_OBJECT_DESCRIPTION Description
    );

NTSTATUS
BmEnumerateGlobalParameters (
    __deref_out PBOOT_ENTRY_OPTION *GlobalParameters
    );

BOOLEAN
BmIsMultiBootSystem (
    __out_opt PULONG BootEntryCount
    );

NTSTATUS
BmLaunchRecoverySequence (
    __in PBOOT_APPLICATION_ENTRY BootEntry,
    __in ULONG LaunchReason
    );

NTSTATUS
BmResumeFromHibernate (
    __inout PHANDLE DataStoreHandle
    );

NTSTATUS
BmDisplayBootMenu (
    __in HANDLE DataStoreHandle,
    __in_ecount(NumBootEntries) PBOOT_APPLICATION_ENTRY *BootEntryList,
    __in ULONG NumBootEntries,
    __in_ecount(NumToolEntries) PBOOT_APPLICATION_ENTRY *ToolEntryList,
    __in ULONG NumToolEntries,
    __deref_out PBOOT_APPLICATION_ENTRY *SelectedBootEntry,
    __out PBOOLEAN ReturnToFirmware
    );

VOID
BmDisplayClearScreen (
    VOID
    );

VOID
BmDisplayGetBootMenuStatus (
    __out PBOOT_MENU_STATUS BootStatus
    );

NTSTATUS
BmFveDisplayScreen (
    __inout PFVE_DISPLAY_CONTEXT DisplayContext
    );

VOID
BmDisplayGetOsLoaderKeyInput (
    __in PBOOT_MENU_STATUS BootMenuStatus,
    __in PBOOT_APPLICATION_ENTRY BootEntry,
    __out PBOOLEAN DisplayBootMenu
    );

BOOLEAN
BmDisplayBootGraphics (
    VOID
    );

BOOLEAN
BmDisplayIsKeyboardLessSystem (
    VOID
    );

VOID
BmDisplayMulticastMessage (
    __in PVOID Parameter,
    __in BOOLEAN DisplayMessage
    );

NTSTATUS
BmUpdateMulticastMessage (
    __in PVOID Parameter
    );

BOOLEAN
BmOemInputCallback (
    __in WCHAR Key
    );

VOID
BmUpdateProgress (
    __in PVOID Context,
    __in ULONG Percentage,
    __out_opt PBOOLEAN ProgressDisplayComplete
    );

VOID
BmUpdateProgressInfo (
    __in PVOID Context,
    __in PWSTR ProgressInfo
    );

NTSTATUS
BmOpenDataStore (
    __out PHANDLE DataStoreHandle
    );

VOID
BmCloseDataStore (
    __in HANDLE DataStoreHandle
    );

NTSTATUS
BmGetOptionList (
    __in HANDLE DataStoreHandle,
    __in LPCGUID ObjectGuid,
    __out PBOOT_ENTRY_OPTION *OptionList
    );

BOOLEAN
BmOemCustomActionExists (
    VOID
    );

NTSTATUS
BmProcessCustomAction (
    __in HANDLE DataStoreHandle,
    __in_opt PUSHORT Value
    );

NTSTATUS
BmPurgeOption (
    __in HANDLE DataStoreHandle,
    __in LPCGUID ObjectGuid,
    __in BCDE_DATA_TYPE DataType
    );

NTSTATUS
BmCreateResumeBootEntry (
    __in PWCHAR FileName,
    __deref_out PBOOT_APPLICATION_ENTRY *BootEntry
    );

PBOOT_APPLICATION_ENTRY
BmGetSelectedBootEntry (
    VOID
    );

VOID
BmSetSelectedBootEntry (
    __in PBOOT_APPLICATION_ENTRY BootEntry
    );

ULONGLONG
BmGetBootMenuPolicy (
    __in_opt PBOOT_APPLICATION_ENTRY BootEntry
    );

ULONGLONG
BmGetBootErrorUxType (
    __in_opt PBOOT_APPLICATION_ENTRY BootEntry,
    __in BOOLEAN NoKeyboard
    );

NTSTATUS
BmFindAssociatedOsEntry (
    _In_ PGUID WinReIdentifier,
    _Outptr_ PBOOT_APPLICATION_ENTRY *AssociatedOsEntry
    );

//
// ----------------------------------------  Boot Manager BGFX Display Routines
//

NTSTATUS
BmBgDisplayAddBodyRegion (
    __in_z PWCHAR String,
    __out_opt HANDLE *Handle
    );

NTSTATUS
BmBgDisplayAddFooterRegion (
    __in_z PWCHAR String
    );

NTSTATUS
BmBgDisplayClearScreen (
    VOID
    );

NTSTATUS
BmBgDisplayDestroy (
    VOID
    );

NTSTATUS
BmBgDisplayInitialize (
    VOID
    );

NTSTATUS
BmBgDisplayInitializeInput (
    VOID
    );

VOID
BmBgDisplayRender (
    __in BOOLEAN RenderSimpleMessage
    );

NTSTATUS
BmBgDisplaySetHeader (
    __in_z PWCHAR String
    );

NTSTATUS
BmBgDisplaySetTitle (
    __in_z PWCHAR String
    );

NTSTATUS
BmBgDisplaySetSimpleMessage (
    __in_z PWCHAR String
    );

VOID
BmBgDisplayUpdateBodyRegion (
    __in HANDLE Handle,
    __in_z PWCHAR String
    );

NTSTATUS
BmBgDisplayUpdateInput (
    __in_z PWCHAR String,
    __in ULONG Flags,
    __in ULONG CursorIndex
    );

NTSTATUS
BmBgDisplayUpdateTitle (
    __in_z PWCHAR String
    );

//
// -------------------------------------------------- Local Function Prototypes
//

BM_ERROR_DISPOSITION
BmDisplayDumpError (
    __in_opt PBOOT_APPLICATION_ENTRY BootEntry,
    __in ULONG LaunchReason
    );

NTSTATUS
BmErrorInitialize (
    VOID
    );

PINTERNAL_BOOT_ERROR
BmErrorGetLast (
    VOID
    );

NTSTATUS
BmErrorGetData (
    VOID
    );

VOID
BmErrorPurge (
    VOID
    );

VOID
BmFatalErrorEx (
    __in ULONG ErrorCode,
    __in ULONG_PTR ErrorParameter1,
    __in ULONG_PTR ErrorParameter2,
    __in ULONG_PTR ErrorParameter3,
    __in ULONG_PTR ErrorParameter4,
    __in_opt PCHAR SourceFile,
    __in_opt ULONG SourceLine
    );

#if DBG
#define BmFatalError(ErrorCode, ErrorParameter1, ErrorParameter2, ErrorParameter3) \
            BmFatalErrorEx((ErrorCode), (ErrorParameter1), (ErrorParameter2), (ErrorParameter3), 0, __FILE__, __LINE__);
#else
#define BmFatalError(ErrorCode, ErrorParameter1, ErrorParameter2, ErrorParameter3) \
            BmFatalErrorEx((ErrorCode), (ErrorParameter1), (ErrorParameter2), (ErrorParameter3), 0, NULL, 0);
#endif

//
// ----------------------------------------------- Firmware Specific Prototypes
//

NTSTATUS
BmFwVerifySelfIntegrity (
    __in ULONG ImageFlags
    );

NTSTATUS
BmFwRegisterRevocationList (
    VOID
    );

NTSTATUS
BmSecureBootInitializeMachinePolicy (
    VOID
    );

NTSTATUS
BmSecureBootLoadSupplementalPoliciesFromAllPartitions ( 
    _In_ PUNICODE_STRING SupplementalPolicyDirectory,
    _In_ BOOLEAN TestRootTrusted
    );


NTSTATUS
BmFwSecureBootLoadCustomPolicy (
    _In_ BOOLEAN TestRootTrusted,
    _In_ BOOLEAN SecureBootEnabled,
    _Outptr_ PSECUREBOOT_POLICY *CustomPolicy
    );

VOID
BmFwSecureBootDeleteCustomPolicy (
    VOID
    );

NTSTATUS
BmpSecureBootLoadFile (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in PCWSTR FilePath,
    __deref_out_bcount(*ContentSize) PVOID* Content,
    __out PULONG ContentSize
    );

NTSTATUS
BmFwGetUniqueDeviceId (
    _Out_ PCRYPT_DER_BLOB UniqueDeviceId
    );

NTSTATUS
BmFwRegisterSystemIntegrityPolicies (
    VOID
    );

VOID
BmFwInferTestRootTrustedFromSecureBootEnabledForPolicy (
    _In_ BOOLEAN SecureBootEnabled,
    _Inout_ PBOOLEAN TestRootTrusted
    );

