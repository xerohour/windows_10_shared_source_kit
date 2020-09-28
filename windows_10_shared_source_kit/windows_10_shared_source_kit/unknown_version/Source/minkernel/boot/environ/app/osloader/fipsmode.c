/*++

Copyright (c) Microsoft Corporation

Module Name:

    fipsmode.c

Abstract:

    This module contains a routine for checking the FIPS mode flag in winload. 

Environment:

    Boot

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "osloader.h"

//
// ---------------------------------------------------------------- Definitions
//

#define SYSTEM_KEY_NAME                 L"System"
#define CURRENT_CONTROL_SET_KEY_NAME    L"CurrentControlSet"
#define CONTROL_KEY_NAME                L"Control"
#define LSA_KEY_NAME                    L"Lsa"
#define FIPS_ALGORITHM_POLICY_KEY_NAME  L"FipsAlgorithmPolicy"
#define FIPS_MODE_VALUE_NAME            L"Enabled"
#define FIPS_MODE_MDM_VALUE_NAME        L"MDMEnabled"

#define POLICIES_KEY_NAME               L"Policies"
#define MICROSOFT_KEY_NAME              L"Microsoft"
#define CRYPTOGRAPHY_KEY_NAME           L"Cryptography"
#define CONFIGURATION_KEY_NAME          L"Configuration"
#define SELFTEST_ALGORITHMS_VALUE_NAME  L"SelfTestAlgorithms"


VOID
OslCheckFipsMode (
    __in  ULONG     HiveHandle,
    __in  BOOLEAN   bImcHiveHandle,
    __out PBOOLEAN  pbFipsModeEnabled
    )
    
/*++

Routine Description:

    This routine checks if the flag is set in 4 registry keys and 
    if one of them is set it returns TRUE. 
    
    The 4 registry keys are:
    BCRYPT_CFG_FIPS_MODE_KEY:
        \HKLM\System\CurrentControlSet\Control\Lsa\FipsAlgorithmPolicy\Enabled (DWORD)
    BCRYPT_CFG_LEGACY_FIPS_MODE_KEY:
        \HKLM\System\CurrentControlSet\Control\Lsa\FipsAlgorithmPolicy (DWORD)
    BCRYPT_MDM_FIPS_MODE_KEY:
        \HKLM\System\CurrentControlSet\Control\Lsa\FipsAlgorithmPolicy\MDMEnabled (DWORD)
    BCRYPT_CRYPTO_CONFIG_KEY:
        \HKLM\SYSTEM\CurrentControlSet\Policies\Microsoft\Cryptography\Configuration\SelfTestAlgorithms (DWORD)
    
Arguments:

    HiveHandle - Supplies a handle to the system hive or the IMC hive
    
    bImcHiveHandle - flag that is set if we got the IMC hive handle

    pbFipsModeEnabled - Supplies a pointer to a BOOLEAN that receives
        the results of the registry check.

Return Value:

    None

--*/

{
    NTSTATUS Status = STATUS_SUCCESS;

    // Registry cells
    PCM_KEY_NODE RootCell = NULL;
    PCM_KEY_NODE SystemCell = NULL;
    PCM_KEY_NODE CurrentControlSetCell = NULL;
    PCM_KEY_NODE ControlCell = NULL;
    PCM_KEY_NODE LsaCell = NULL;
    PCM_KEY_NODE FipsAlgorithmPolicyCell = NULL;
    PCM_KEY_NODE PoliciesCell = NULL;
    PCM_KEY_NODE MicrosoftCell = NULL;
    PCM_KEY_NODE CryptographyCell = NULL;
    PCM_KEY_NODE ConfigurationCell = NULL;


    // Registry results
    DWORD dwResult = 0;
    ULONG DataLength = 0;
    ULONG Type = 0;
    
    if (bImcHiveHandle)
    {
        //
        // IMC Hive
        //
        
        // RootCell
        RootCell = OslGetRootCell(HiveHandle);
        
        // System
        if (RootCell != NULL)
        {
            OslGetSubkey(HiveHandle,
                         RootCell,
                         SYSTEM_KEY_NAME,
                         &SystemCell);     
        }
        
        // CurrentControlSet
        if (SystemCell != NULL)
        {
            OslGetSubkey(HiveHandle,
                         SystemCell,
                         CURRENT_CONTROL_SET_KEY_NAME,
                         &CurrentControlSetCell);
        }
    }
    else
    {
        // System Hive
        
        // CurrentControlSet
        OslGetSubkey(HiveHandle,
                     NULL,
                     NULL,
                     &CurrentControlSetCell);
    }
    
    //
    // Key: \HKLM\System\CurrentControlSet\Control\Lsa\FipsAlgorithmPolicy
    // Value: Enabled (DWORD)
    //
    
    // Control
    if (CurrentControlSetCell != NULL)
    {
        OslGetSubkey(HiveHandle,
                     CurrentControlSetCell,
                     CONTROL_KEY_NAME,
                     &ControlCell);
    }

    // Lsa
    if (ControlCell != NULL)
    {
        OslGetSubkey(HiveHandle,
                     ControlCell,
                     LSA_KEY_NAME,
                     &LsaCell);     
    }

    // FipsAlgorithmPolicy
    if (LsaCell != NULL)
    {
        OslGetSubkey(HiveHandle,
                     LsaCell,
                     FIPS_ALGORITHM_POLICY_KEY_NAME,
                     &FipsAlgorithmPolicyCell);    
    }

    // Enabled
    if (FipsAlgorithmPolicyCell != NULL)
    {
        DataLength = sizeof(dwResult);
        Status = OslGetValue(HiveHandle,
                             FipsAlgorithmPolicyCell,
                             FIPS_MODE_VALUE_NAME,
                             &Type,
                             &DataLength,
                             (PBYTE)&dwResult); 
    }


    if( NT_SUCCESS(Status) &&
        (FipsAlgorithmPolicyCell != NULL) &&
        Type == REG_DWORD &&
        DataLength == sizeof(DWORD) &&
        dwResult != 0)
    {
        // if the flag is set don't check the other locations
        goto OslCheckFipsModeEnd;
    }
    else
    {
        dwResult = 0;
    }
    
    // MDMEnabled
    if (FipsAlgorithmPolicyCell != NULL)
    {
        DataLength = sizeof(dwResult);
        Status = OslGetValue(HiveHandle,
                             FipsAlgorithmPolicyCell,
                             FIPS_MODE_MDM_VALUE_NAME,
                             &Type,
                             &DataLength,
                             (PBYTE)&dwResult); 
    }


    if( NT_SUCCESS(Status) &&
        (FipsAlgorithmPolicyCell != NULL) &&
        Type == REG_DWORD &&
        DataLength == sizeof(DWORD) &&
        dwResult != 0)
    {
        // if the flag is set don't check the other locations
        goto OslCheckFipsModeEnd;
    }
    else
    {
        dwResult = 0;
    }
    
    //
    // Key: \HKLM\System\CurrentControlSet\Control\Lsa
    // Value: FipsAlgorithmPolicy (DWORD)
    //
    
    // FipsAlgorithmPolicy
    if (LsaCell != NULL)
    {
        DataLength = sizeof(dwResult);
        Status = OslGetValue(HiveHandle,
                             LsaCell,
                             FIPS_ALGORITHM_POLICY_KEY_NAME, // Value name same as key name
                             &Type,
                             &DataLength,
                             (PBYTE)&dwResult); 
    }


    if( NT_SUCCESS(Status) &&
        (LsaCell != NULL) &&
        Type == REG_DWORD &&
        DataLength == sizeof(DWORD) &&
        dwResult != 0)
    {
        // if the flag is set don't check the other locations
        goto OslCheckFipsModeEnd;
    }
    else
    {
        dwResult = 0;
    }
    
    //
    // Key: \HKLM\SYSTEM\CurrentControlSet\Policies\Microsoft\Cryptography\Configuration
    // Value: SelfTestAlgorithms (DWORD)
    //
    
    // Policies
    if (CurrentControlSetCell != NULL)
    {
        OslGetSubkey(HiveHandle,
                     CurrentControlSetCell,
                     POLICIES_KEY_NAME,
                     &PoliciesCell);
    }

    // Microsoft
    if (PoliciesCell != NULL)
    {
        OslGetSubkey(HiveHandle,
                     PoliciesCell,
                     MICROSOFT_KEY_NAME,
                     &MicrosoftCell);      
    }

    // Cryptography
    if (MicrosoftCell != NULL)
    {
        OslGetSubkey(HiveHandle,
                     MicrosoftCell,
                     CRYPTOGRAPHY_KEY_NAME,
                     &CryptographyCell); 
    }
    
    // Configuration
    if (CryptographyCell != NULL)
    {
        OslGetSubkey(HiveHandle,
                     CryptographyCell,
                     CONFIGURATION_KEY_NAME,
                     &ConfigurationCell);    
    }

    // SelfTestAlgorithms
    if (ConfigurationCell != NULL)
    {
        DataLength = sizeof(dwResult);
        Status = OslGetValue(HiveHandle,
                             ConfigurationCell,
                             SELFTEST_ALGORITHMS_VALUE_NAME,
                             &Type,
                             &DataLength,
                             (PBYTE)&dwResult); 
    }
    
    if( NT_SUCCESS(Status) &&
        (ConfigurationCell != NULL) &&
        Type == REG_DWORD &&
        DataLength == sizeof(DWORD) &&
        dwResult != 0)
    {
        // if the flag is set don't check the other locations
        goto OslCheckFipsModeEnd;
    }
    else
    {
        dwResult = 0;
    }

OslCheckFipsModeEnd:
    
    *pbFipsModeEnabled = (dwResult!=0);
    
}
