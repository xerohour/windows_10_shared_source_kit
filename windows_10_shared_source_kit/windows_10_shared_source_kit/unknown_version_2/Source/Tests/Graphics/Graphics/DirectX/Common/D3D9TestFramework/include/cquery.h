SION L".LOG2"
#define HARDWARE_POLICY_NAME L"hwpolicy.sys"
#define ELAM_HIVE_NAME      L"elam"
#define SECUREKERNEL_NAME L"securekernel.exe"
#define APISET_SCHEMA_NAME  API_SET_STRING_U (API_SET_SCHEMA_NAME) PS_SYSTEM_DLL_SUFFIX
#define CI_REVOCATION_LIST_PATH SYSTEM_PATH L"\\CodeIntegrity\\"
#define CI_REVOCATION_LIST_NAME L"driver.stl"


typedef struct _ADVANCED_BOOT_OPTION {
    ULONG AdvancedOption;
    PWCHAR AdvancedOptionString;
} ADVANCED_BOOT_OPTION, *PADVANCED_BOOT_OPTION;

#define OSL_ERROR_CRITICAL_DRIVER               0x00000001
#define OSL_ERROR_LOAD_IMPORT                   0x00000002
#define OSL_ERROR_IMPORT                        0x00000003
#define OSL_ERROR_NO_ACPI_DETECTED              0x00000004
#define OSL_ERROR_REGISTRY                      0x00000005
#define OSL_ERROR_KERNEL                        0x00000006
#define OSL_ERROR_HAL                           0x00000007
#define OSL_ERROR_KD_DLL                        0x00000008
#define OSL_ERROR_NLS_DATA                      0x00000009
#define OSL_ERROR_INVALID_RSDP                  0x0000000A
#define OSL_ERROR_CODE_INTEGRITY_INITIALIZATION 0x0000000B
#define OSL_ERROR_UPDATE_DLL                    0x0000000C
#define OSL_ERROR_MEMORY_MAP_CONFLICT           0x0000000D
#define OSL_ERROR_OS_EXTENSION                  0x0000000E
#define OSL_ERROR_BAD_BOOT                      0x0000000F
#define OSL_ERROR_BAD_SHUTDOWN                  0x00000010
#define OSL_ERROR_MANUFACTURING_MODE            0x00000011
#define OSL_ERROR_MANUFACTURING_MODE_SECURITY_LOCK_SCREEN      0x00000012
#define OSL_ERROR_MANUFACTURING_MODE_SECURITY_BITLOCKER        0x00000013
#define OSL_ERROR_MANUFACTURING_MODE_SECURITY_RESET_PROTECTION 0x00000014

//
// The numbers in this table must match exactly to the options listed
// in the osloader.xsl "osboot-advops" section.
//

#define SAFEMODE                  0
#define SAFEMODE_NET              1
#define SAFEMODE_CMD              2
#define RECOVERY_SEQUENCE         3
#define BOOT_LOGGING              4
#define BASICDISPLAY_MODE         5
#define LAST_KNOWN_GOOD           6
#define DS_REPAIR_MODE            7
#define DEBUG_MODE                8
#define DISABLE_CRASH_AUTOREBOOT  9
#define DISABLE_CODE_INTEGRITY   10
#define BOOT_NORMAL              11
#define NOGUIBOOT                12
#define DISABLE_ELAM_DRIVERS     13
#define MAX_ADVANCED_BOOT_OPTION 14

//
// Define memory types used by the OS Loader.
//

#define OSL_MEMORY_TYPE(_x) (MEMORY_APPLICATION_TYPE_DEFINABLE_START + (_x))

#define OSL_MEMORY_TYPE_KERNEL_DATA_STRUCTURE \
    GET_MEMORY_TYPE(MEMORY_CLASS_APPLICATION, OSL_MEMORY_TYPE(0x01))

#define OSL_MEMORY_TYPE_SYSTEM_CODE \
    GET_MEMORY_TYPE(MEMORY_CLASS_APPLICATION, OSL_MEMORY_TYPE(0x02))

#define OSL_MEMORY_TYPE_BOOT_DRIVER \
    GET_MEMORY_TYPE(MEMORY_CLASS_APPLICATION, OSL_MEMORY_TYPE(0x03))

#define OSL_MEMORY_TYPE_MISC_MODULE \
    GET_MEMORY_TYPE(MEMORY_CLASS_APPLICATION, OSL_MEMORY_TYPE(0x04))

#define OSL_MEMORY_TYPE_NLS_DATA \
    GET_MEMORY_TYPE(MEMORY_CLASS_APPLICATION, OSL_MEMORY_TYPE(0x05))

#define OSL_MEMORY_TYPE_REGISTRY_DATA \
    GET_MEMORY_TYPE(MEMORY_CLASS_APPLICATION, OSL_MEMORY_TYPE(0x06))

#define OSL_MEMORY_TYPE_KERNEL_INIT_DATA \
    GET_MEMO