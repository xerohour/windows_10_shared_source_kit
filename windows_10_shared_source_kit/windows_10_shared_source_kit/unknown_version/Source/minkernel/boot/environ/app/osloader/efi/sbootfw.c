/*++

Copyright (c) Microsoft Corporation

Module Name:

    sbootfw.c

Abstract:

    This module contains the EFI firmware support for the secure boot.

Environment:

    Boot.

--*/

// ------------------------------------------------------------------- Includes

#include "osloader.h"
#include "secconfig.h"

// ------------------------------------------------------------------- Defines


//
// Test hook flags for HKLM\System\CurrentControlSet\Control\CI!TestFlags
//

#define CI_TEST_FLAG_IGNORE_DISABLE_USN0_PROTECTION (1UL << 0)


// -------------------------------------------------------------------- Globals

//
// EFI variable vendor GUID for the kernel private variables.
//

const EFI_GUID OslFwpSecureBootVendorGuid = SECUREBOOT_EFI_NAMESPACE_GUID;

// ------------------------------------------------------------------ Functions

VOID
OslpLogSecureBootDeployFailure (
    _In_ DWORD Reason,
    _In_ NTSTATUS ReasonStatus
    );

#ifdef SECCONFIG_VARIABLES
VOID
OslFwProtectSecConfigVars(
    _In_ ULONG SystemHiveHandle
    )
/*++
    
Routine Description:

    This routine protects configuration values by creating runtime
    variables that cannot be modified from user mode within the OS.

Arguments:

    SystemHiveHandle - Supplies an opaque handle to the system hive.

Return Value:

    None.
    
 --*/
    
{
    ULONG BootAttributes;
    ULONG RegConfig;
    UINTN Size;
    NTSTATUS Status;
    BOOLEAN VarExistsAndValid;
    ULONG VarConfig;
    PVOID Cleared;
    ULONG ClearedSize;

    RegConfig = 0;
    Status = OslHiveReadWriteControlDword(SystemHiveHandle,
                                          FALSE,
                                          LSACONFIG_REGISTRY_KEY,
                                          LSAPPL_REGISTRY_VALUE,
                                          &RegConfig);

    if(NT_SUCCESS(Status) && 
       RegConfig != 0) {
       
        RegConfig = LSA_RUNASPPL_MODE;
        VarExistsAndValid = FALSE;
        Size = sizeof(VarConfig);
        Status = EfiGetVariable(SECCONFIG_LSA_PPL_VARIABLE,
                                &OslFwpSecureBootVendorGuid,
                                &BootAttributes,
                                &Size,
                                &VarConfig);
        
        if((NT_SUCCESS(Status)) && 
           (Size == sizeof(VarConfig)) &&
           (VarConfig == LSA_RUNASPPL_MODE) &&
           (CHECK_FLAG(BootAttributes, EFI_VARIABLE_RUNTIME_ACCESS) != FALSE) &&
           (CHECK_FLAG(BootAttributes, EFI_VARIABLE_BOOTSERVICE_ACCESS) != FALSE)) {
           
            VarExistsAndValid = TRUE;
        }

        if (!VarExistsAndValid) {
            
            Status = EfiSetVariable(
                        SECCONFIG_LSA_PPL_VARIABLE,
                        &OslFwpSecureBootVendorGuid,
                        EFI_VARIABLE_NON_VOLATILE | \
                            EFI_VARIABLE_BOOTSERVICE_ACCESS |
                            EFI_VARIABLE_RUNTIME_ACCESS,
                        sizeof(RegConfig),
                        &RegConfig);
        }
    }

    //
    // The following code is only applicable on platforms that support virtualization based security
    //

    Cleared = NULL;
    ClearedSize = 0;
#if PLATFORM_SUPPORTS_LSAISO
    RegConfig = 0;

    //
    // Check if SecConfig has been cleared through the EFI tool.
    //

    Status = BlSecureBootGetBootPrivateVariable(SECCONFIG_LSA_CFG_FLAGS_CLEARED_VARIABLE,
                                                &Cleared,
                                                &ClearedSize);
    if (NT_SUCCESS(Status)) {
        BlMmFreeHeap(Cleared);

        //
        // Clear the registry, so we don't re-enable the UEFI variable
        //

        Status = OslHiveReadWriteControlDword(SystemHiveHandle,
                                              TRUE,
                                              LSACONFIG_REGISTRY_KEY,
                                              LSA_CFG_FLAGS_REGISTRY_VALUE,
                                              &RegConfig);
        NT_ASSERT(NT_SUCCESS(Status));

        Status = BlSecureBootDeleteBootPrivateVariable(SECCONFIG_LSA_CFG_FLAGS_CLEARED_VARIABLE);
        NT_ASSERT(NT_SUCCESS(Status));
    }
    else {
        Status = OslHiveReadWriteControlDword(SystemHiveHandle,
                                              FALSE,
                                              LSACONFIG_REGISTRY_KEY,
                                              LSA_CFG_FLAGS_REGISTRY_VALUE,
                                              &RegConfig);

        RegConfig &= LSA_VALID_UEFI_PERSISTED_CFG_FLAGS;

        if (NT_SUCCESS(Status) &&
            RegConfig != 0) {

            VarExistsAndValid = FALSE;
            Size = sizeof(VarConfig);
            Status = EfiGetVariable(SECCONFIG_LSA_CFG_FLAGS_VARIABLE,
                                    &OslFwpSecureBootVendorGuid,
                                    &BootAttributes,
                                    &Size,
                                    &VarConfig);

            if (!(NT_SUCCESS(Status)) ||
                (Size != sizeof(VarConfig))) {

                VarConfig = 0;

            } else if ((VarConfig | RegConfig) == VarConfig && // only check for new bits being set
                       (CHECK_FLAG(BootAttributes, EFI_VARIABLE_RUNTIME_ACCESS) != FALSE) &&
                       (CHECK_FLAG(BootAttributes, EFI_VARIABLE_BOOTSERVICE_ACCESS) != FALSE)) {

                VarExistsAndValid = TRUE;
            }

            if (!VarExistsAndValid) {

                RegConfig |= VarConfig; // Flags are only set, never cleared, in this path
                RegConfig &= LSA_VALID_UEFI_PERSISTED_CFG_FLAGS;

                Status = EfiSetVariable(
                    SECCONFIG_LSA_CFG_FLAGS_VARIABLE,
                    &OslFwpSecureBootVendorGuid,
                    EFI_VARIABLE_NON_VOLATILE | \
                    EFI_VARIABLE_BOOTSERVICE_ACCESS |
                    EFI_VARIABLE_RUNTIME_ACCESS,
                    sizeof(RegConfig),
                    &RegConfig);
            }
        }
    }
#endif
}
#endif

NTSTATUS
OslFwProtectSecureBootVariables (
    __in ULONG SystemHiveHandle
    )

/*++

Routine Description:

    This routine processes the runtime Secure Boot variables that need
    protecting by boot services variables.

Arguments:

    SystemHiveHandle - Supplies an opaque handle to the system hive.

Return Value:

    STATUS_SUCCESS if successful.

    Othe status codes when unsuccessful.

--*/

{
    ULONG BootAttributes;
    BOOLEAN BootValid;
    SECUREBOOT_CI_PROTECTED_VALUES BootValues;
    BOOLEAN RuntimeExists;
    BOOLEAN RuntimeValid;
    SECUREBOOT_CI_PROTECTED_VALUES RuntimeValues;
    BOOLEAN SecureBoot;
    UINTN Size;
    NTSTATUS Status;
    ULONG TestFlags;
    BOOLEAN WinPE;

#ifdef SECCONFIG_VARIABLES
    OslFwProtectSecConfigVars(SystemHiveHandle);
#endif

    Status = BlSecureBootIsEnabled(&SecureBoot);
    if (!NT_SUCCESS(Status)) {
        SecureBoot = FALSE;
    }

    //
    // If Secure Boot is not enabled or the policy requires no protection,
    // check if the test hook to enable protection is set.
    //

    if (SecureBoot == FALSE ||
        BlSecureBootArePolicyOptionsSet(
                SECUREBOOT_POLICY_DISABLE_UMCI_USN_0_PROTECTION) != FALSE) {

        Status = OslHiveReadWriteControlDword(SystemHiveHandle,
                                              FALSE,
                                              L"CI",
                                              L"TestFlags",
                                              &TestFlags);

        if (!NT_SUCCESS(Status) ||
            CHECK_FLAG(TestFlags,
                       CI_TEST_FLAG_IGNORE_DISABLE_USN0_PROTECTION) == FALSE) {

            //
            // The test hook to protect even with secure boot off is not set,
            // so just exit.
            //

            return STATUS_SUCCESS;
        }
    }

    //
    // Try to read the boot and runtime variables.
    //

    BootValid = FALSE;
    RuntimeExists = FALSE;
    RuntimeValid = FALSE;
    Size = sizeof(RuntimeValues);
    Status = EfiGetVariable(SECUREBOOT_CI_PROTECTED_VALUES_VARIABLE_NAME,
                            &OslFwpSecureBootVendorGuid,
                            NULL,
                            &Size,
                            &RuntimeValues);

    if (NT_SUCCESS(Status) || (Status == STATUS_BUFFER_TOO_SMALL)) {
        RuntimeExists = TRUE;
    }

    if (NT_SUCCESS(Status) && (Size == sizeof(RuntimeValues))) {
        RuntimeValid = TRUE;
    }

    Size = sizeof(BootValues);
    Status = EfiGetVariable(
                SECUREBOOT_COPY_OF_CI_PROTECTED_VALUES_VARIABLE_NAME,
                &OslFwpSecureBootVendorGuid,
                &BootAttributes,
                &Size,
                &BootValues);

    if (NT_SUCCESS(Status) &&
        (Size == sizeof(BootValues)) &&
        (CHECK_FLAG(BootAttributes, EFI_VARIABLE_RUNTIME_ACCESS) == FALSE)) {

        BootValid = TRUE;
    }

    //
    // If neither of the variables have been set, we are booting into WinPE, and
    // Secure Boot is on then write the variable so that the machine is
    // tainted. We cannot trust USN Zero if the user is trying to boot WinPE
    // before they have booted the full OS as they could be trying to lay down
    // unsigned code.
    //

    if ((SecureBoot != FALSE) &&
        (BootValid == FALSE) &&
        (RuntimeValid == FALSE)) {

        Status = BlGetApplicationOptionBoolean(BCDE_OSLOADER_TYPE_WINPE, &WinPE);
        if (NT_SUCCESS(Status) && (WinPE != FALSE)) {
            BootValues.SystemVolumeJournalID = MAXULONGLONG;
            BootValues.RevocationListTime.QuadPart = MAXLONGLONG;
            Status = EfiSetVariable(
                        SECUREBOOT_COPY_OF_CI_PROTECTED_VALUES_VARIABLE_NAME,
                        &OslFwpSecureBootVendorGuid,
                        EFI_VARIABLE_NON_VOLATILE | \
                        EFI_VARIABLE_BOOTSERVICE_ACCESS,
                        sizeof(BootValues),
                        &BootValues);

            //
            // Even if the write fails, mark this as valid so that the runtime
            // variable is updated below.
            //

            BootValid = TRUE;
        }
    }

    //
    // If the runtime variable is valid either the boot variable needs to be
    // tested for equality or written out if it doesn't exist.
    //

    if (RuntimeValid != FALSE) {
        if (BootValid != FALSE) {
            if (RtlEqualMemory(&BootValues,
                               &RuntimeValues,
                               sizeof(RuntimeValues)) == FALSE) {

                RuntimeValid = FALSE;
            }

        } else {
            Status = EfiSetVariable(
                        SECUREBOOT_COPY_OF_CI_PROTECTED_VALUES_VARIABLE_NAME,
                        &OslFwpSecureBootVendorGuid,
                        EFI_VARIABLE_NON_VOLATILE | \
                        EFI_VARIABLE_BOOTSERVICE_ACCESS,
                        sizeof(RuntimeValues),
                        &RuntimeValues);
        }
    }

    //
    // If the boot variable is valid but the runtime is not, try to delete the
    // runtime variable and then just create a volatile runtime variable. This
    // has the nice property that the variable is read-only at runtime.
    //

    if ((BootValid != FALSE) && (RuntimeValid == FALSE)) {
        if (RuntimeExists != FALSE) {
            Status = EfiSetVariable(
                        SECUREBOOT_CI_PROTECTED_VALUES_VARIABLE_NAME,
                        &OslFwpSecureBootVendorGuid,
                        EFI_VARIABLE_NON_VOLATILE | \
                        EFI_VARIABLE_BOOTSERVICE_ACCESS |
                        EFI_VARIABLE_RUNTIME_ACCESS,
                        0,
                        NULL);
        }

        Status = EfiSetVariable(
                    SECUREBOOT_CI_PROTECTED_VALUES_VARIABLE_NAME,
                    &OslFwpSecureBootVendorGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS |
                    EFI_VARIABLE_RUNTIME_ACCESS,
                    sizeof(BootValues),
                    &BootValues);
    }
    
    //
    // Always return success.
    //

    return STATUS_SUCCESS;
}

VOID
OslFwCheckDeploySecureBoot (
    __in ULONG SystemHiveHandle
    )

/*++

Routine Description:

    This routine switches the system from User Mode to Deployed Mode
    if needed.

    The switch will only be done for UEFI 2.5+ if Secure Boot is on
    and in User Mode, and if the Unattend setting for skipping the
    Deployment has not been set.

Arguments:

    SystemHiveHandle - Supplies an opaque handle to the system hive.

Return Value:

    STATUS_SUCCESS if successful.

    Other status codes when unsuccessful.

--*/

{
    ULONG AuditBoot;
    PCWSTR AuditBootValueName = L"AuditBoot";
    UINT8 AuditMode;
    PCWSTR ControlKeyName = L"Control";
    UINT8 DeployedMode;
    UINTN EfiSize;
    DWORD NoDeployReason;
    NTSTATUS NoDeployReasonStatus;
    PBYTE NoDeployUnattendBinaryValue;
    DWORD NoDeployUnattendDWordValue;
    PCWSTR NoDeployUnattendValueName = L"NoDeployUnattend";
    ULONG RegSize;
    PCM_KEY_NODE RootCell;
    BOOLEAN SecureBoot;
    PCM_KEY_NODE SecureBootCell;
    PCWSTR SecureBootKeyName = L"SecureBoot";
    PCM_KEY_NODE SetupCell;
    PCWSTR SetupKeyName = L"Setup";
    UINT8 SetupMode;
    NTSTATUS Status;
    PCM_KEY_NODE StatusCell;
    PCWSTR StatusKeyName = L"Status";
    PCM_KEY_NODE SystemControlCell;
    PCM_KEY_NODE SystemControlSetCell;
    BOOLEAN WinPE;

    NoDeployReason = 0;
    NoDeployReasonStatus = STATUS_SUCCESS;
    NoDeployUnattendBinaryValue = NULL;
    RootCell = NULL;
    SecureBootCell = NULL;
    SetupCell = NULL;
    StatusCell = NULL;
    SystemControlCell = NULL;
    SystemControlSetCell = NULL;

    //
    // If Secure Boot is not enabled, just exit
    //

    Status = BlSecureBootIsEnabled(&SecureBoot);
    if (!NT_SUCCESS(Status)) {
        NoDeployReason = EFI_NODEPLOY_NO_SECURE_BOOT;
        NoDeployReasonStatus = Status;

        Status = STATUS_SUCCESS;
        goto CheckSecureBootDeployedEnd;
    }

    else if (SecureBoot == FALSE) {
        NoDeployReason = EFI_NODEPLOY_SECURE_BOOT_OFF;
        NoDeployReasonStatus = Status;

        Status = STATUS_SUCCESS;
        goto CheckSecureBootDeployedEnd;
    }

    //
    // Check for the binary Unattend setting to skip setting Deployed Mode.
    // Also, use this setting as a test hook to attempt to clear Deployed
    // Mode, by setting it to a binary value with one non-zero byte.
    //
    // Get the CurrentControlSet key from the system hive.
    // Passing NULL for HiveCellParent causes the CCS to be returned if the
    // hive is a System hive.
    //

    RegSize = 0;
    Status = OslGetSubkey(SystemHiveHandle,
                          NULL,
                          NULL,
                          &SystemControlSetCell);

    if (NT_SUCCESS(Status)) {
        Status = OslGetSubkey(SystemHiveHandle,
                              SystemControlSetCell,
                              ControlKeyName,
                              &SystemControlCell);
    }

    if (NT_SUCCESS(Status)) {
        Status = OslGetSubkey(SystemHiveHandle,
                              SystemControlCell,
                              SecureBootKeyName,
                              &SecureBootCell);
    }

    if (NT_SUCCESS(Status)) {

        //
        // Try to read the Unattend setting as a binary value
        //

        Status = OslGetBinaryValue(SystemHiveHandle,
                                   SecureBootCell,
                                   NoDeployUnattendValueName,
                                   &NoDeployUnattendBinaryValue,
                                   &RegSize);

        if (!NT_SUCCESS(Status)) {

            //
            // Reading the setting as a binary value failed; try as a DWORD value
            //

            RegSize = sizeof(NoDeployUnattendDWordValue);
            NoDeployUnattendBinaryValue = (PBYTE)&NoDeployUnattendDWordValue;

            Status = OslGetDWordValue(SystemHiveHandle,
                                      SecureBootCell,
                                      NoDeployUnattendValueName,
                                      (PDWORD)NoDeployUnattendBinaryValue);
        }
    }

    if (NT_SUCCESS(Status)) {
        if ((RegSize == sizeof(BYTE)) &&
            (*NoDeployUnattendBinaryValue != 0)) {

            //
            // Test hook to attempt to clear the Deployed Mode variable
            //

            DeployedMode = 0;
            EfiSize = sizeof(DeployedMode);
            Status = EfiSetVariable(EFI_DEPLOYEDMODE_VARIABLE,
                                    &EfiGlobalVariable,
                                    EFI_VARIABLE_RUNTIME_ACCESS |
                                    EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                    EfiSize,
                                    &DeployedMode);

            NoDeployReason = EFI_NODEPLOY_TEST_HOOK;
            NoDeployReasonStatus = Status;

            Status = STATUS_SUCCESS;
            goto CheckSecureBootDeployedEnd;
        }

        else if ((RegSize == sizeof(DWORD)) &&
                 (*(DWORD*)NoDeployUnattendBinaryValue != 0)) {

            //
            // The Unattend setting to not set Deployed Mode is present.
            // Skip setting Deployed Mode.
            //

            NoDeployReason = EFI_NODEPLOY_UNATTEND;
            NoDeployReasonStatus = STATUS_SUCCESS;

            Status = STATUS_SUCCESS;
            goto CheckSecureBootDeployedEnd;
        }
    }

    //
    // If "DeployedMode" is not present or is already set, nothing to do.
    //

    EfiSize = sizeof(DeployedMode);
    Status = EfiGetVariable(EFI_DEPLOYEDMODE_VARIABLE,
                            &EfiGlobalVariable,
                            NULL,
                            &EfiSize,
                            &DeployedMode);

    if (!NT_SUCCESS(Status))
    {
        NoDeployReason = EFI_NODEPLOY_NO_DEPLOYEDMODE;
        NoDeployReasonStatus = Status;

        Status = STATUS_SUCCESS;
        goto CheckSecureBootDeployedEnd;
    }

    else if (DeployedMode != 0)
    {
        NoDeployReason = EFI_NODEPLOY_ALREADY_DEPLOYED;
        NoDeployReasonStatus = Status;

        Status = STATUS_SUCCESS;
        goto CheckSecureBootDeployedEnd;
    }

    //
    // If "AuditMode" is not present or is set, nothing to do.
    //

    EfiSize = sizeof(AuditMode);
    Status = EfiGetVariable(EFI_AUDITMODE_VARIABLE,
                            &EfiGlobalVariable,
                            NULL,
                            &EfiSize,
                            &AuditMode);

    if (!NT_SUCCESS(Status))
    {
        NoDeployReason = EFI_NODEPLOY_NO_AUDITMODE;
        NoDeployReasonStatus = Status;

        Status = STATUS_SUCCESS;
        goto CheckSecureBootDeployedEnd;
    }

    else if (AuditMode != 0)
    {
        NoDeployReason = EFI_NODEPLOY_AUDIT_MODE;
        NoDeployReasonStatus = Status;

        Status = STATUS_SUCCESS;
        goto CheckSecureBootDeployedEnd;
    }

    //
    // If "SetupMode" is not present or is set, nothing to do.
    //

    EfiSize = sizeof(SetupMode);
    Status = EfiGetVariable(EFI_SETUPMODE_VARIABLE,
                            &EfiGlobalVariable,
                            NULL,
                            &EfiSize,
                            &SetupMode);

    if (!NT_SUCCESS(Status))
    {
        NoDeployReason = EFI_NODEPLOY_NO_SETUPMODE;
        NoDeployReasonStatus = Status;

        Status = STATUS_SUCCESS;
        goto CheckSecureBootDeployedEnd;
    }

    else if (SetupMode != 0)
    {
        NoDeployReason = EFI_NODEPLOY_SETUP_MODE;
        NoDeployReasonStatus = Status;

        Status = STATUS_SUCCESS;
        goto CheckSecureBootDeployedEnd;
    }

    //
    // If we are booting into WinPE, skip setting Deployed Mode.
    //

    Status = BlGetApplicationOptionBoolean(BCDE_OSLOADER_TYPE_WINPE, &WinPE);
    if (NT_SUCCESS(Status) && (WinPE != FALSE)) {
        NoDeployReason = EFI_NODEPLOY_WINPE;
        NoDeployReasonStatus = Status;

        Status = STATUS_SUCCESS;
        goto CheckSecureBootDeployedEnd;
    }

    //
    // Read the Audit Boot setting.  Don't set Deployed Mode on Audit Boot.
    // If the value or any portion of the path is missing, treat it as not set.
    // { L"\\Registry\\Machine\\SYSTEM\\Setup\\Status", L"AuditBoot" }
    //

    AuditBoot = 0;
    RootCell = OslGetRootCell(SystemHiveHandle);
    Status = OslGetSubkey(SystemHiveHandle,
                          RootCell,
                          SetupKeyName,
                          &SetupCell);

    if (NT_SUCCESS(Status)) {
        Status = OslGetSubkey(SystemHiveHandle,
                              SetupCell,
                              StatusKeyName,
                              &StatusCell);
    }

    if (NT_SUCCESS(Status)) {
        Status = OslGetDWordValue(SystemHiveHandle,
                                  StatusCell,
                                  AuditBootValueName,
                                  (PDWORD)&AuditBoot);
    }

    if ((NT_SUCCESS(Status)) &&
        (AuditBoot != 0)) {

        NoDeployReason = EFI_NODEPLOY_AUDIT_BOOT;
        NoDeployReasonStatus = Status;

        Status = STATUS_SUCCESS;
        goto CheckSecureBootDeployedEnd;
    }

    //
    // Set the Deployed Mode variable
    //

    DeployedMode = 1;
    EfiSize = sizeof(DeployedMode);
    Status = EfiSetVariable(EFI_DEPLOYEDMODE_VARIABLE,
                            &EfiGlobalVariable,
                            EFI_VARIABLE_RUNTIME_ACCESS |
                            EFI_VARIABLE_BOOTSERVICE_ACCESS,
                            EfiSize,
                            &DeployedMode);

    if (!NT_SUCCESS(Status)) {
        NoDeployReason = EFI_NODEPLOY_DEPLOY_FAILED;
        NoDeployReasonStatus = Status;
    }

    else {
        NoDeployReason = EFI_NODEPLOY_DEPLOYED;
        NoDeployReasonStatus = Status;
    }

CheckSecureBootDeployedEnd:

    if (NoDeployUnattendBinaryValue != NULL &&
        NoDeployUnattendBinaryValue != (PBYTE)&NoDeployUnattendDWordValue) {

        (VOID)BlMmFreeHeap(NoDeployUnattendBinaryValue);
    }
    NoDeployUnattendBinaryValue = NULL;

    //
    // Log telemetry indicating that deployment succeeded, or why it did not occur
    //

    if (NoDeployReason != 0)
    {
        OslpLogSecureBootDeployFailure(NoDeployReason, NoDeployReasonStatus);
    }

    return;
}

VOID
OslpLogSecureBootDeployFailure (
    _In_ DWORD Reason,
    _In_ NTSTATUS ReasonStatus
    )

/*++

Routine Description:

    This routine logs for telemetry the failure of Customized Deployment of
        Secure Boot to enter the Deployed Mode.

Arguments:

    Reason - Specifies that Customized Deployment of Secure Boot was just
        Deployed, or specifies a reason code for not deploying it
        (defined in 'efisecurity.h').

    Status - Provides an NTSTATUS error or success code for additional
        information.

Return Value:

    None.

--*/

{
    TraceLoggingWrite(OslBootProvider,
                      "SecureBootCustomizedDeploymentResult",
                      TraceLoggingLevel(TRACE_LEVEL_INFORMATION),
                      TraceLoggingKeyword(MICROSOFT_KEYWORD_TELEMETRY),
                      TraceLoggingUInt32(Reason, "NoDeployReason"),
                      TraceLoggingHexInt32(ReasonStatus, "NoDeployStatus"));

    return;
}


