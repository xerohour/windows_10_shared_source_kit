/*++

Copyright (c) 2012  Microsoft Corporation

Module Name:

    MSOS20DESC.H

Abstract:

    Internal definitions for the MS OS 2.0 Descriptors.  

Environment:

    Kernel & user mode

Revision History:

    11-23-12 : created

--*/

#ifndef   __MSOS20DESC_H__
#define   __MSOS20DESC_H__

#include <pshpack1.h>

//
// Flags used for SQM
//
typedef union _MSOS20_FLAGS {
    ULONG   AsUlong32;
    struct {
        ULONG   DescriptorSetInfo:1;
        ULONG   DescriptorSetHeader:1;
        ULONG   AlternateEnumeration:1;
        ULONG   ConfigurationSubset:1;
        ULONG   FunctionSubset:1;
        ULONG   CompatibleId:1;
        ULONG   RegistryProperty:1;
        ULONG   MinResumeTime:1;
        ULONG   ModelId:1;
        ULONG   CcgpDevice:1;
    };
} MSOS20_FLAGS, *PMSOS20_FLAGS;

C_ASSERT(sizeof(MSOS20_FLAGS) == sizeof(ULONG));

//
// Microsoft OS 2.0 Descriptor Index
//
#define MSOS20_DESCRIPTOR_INDEX                     0x07
#define MSOS20_SET_ALT_ENUMERATION                  0x08

//
// Microsoft OS 2.0 Descriptor Types
//
#define MSOS20_SET_HEADER_DESCRIPTOR                0x00
#define MSOS20_SUBSET_HEADER_CONFIGURATION          0x01
#define MSOS20_SUBSET_HEADER_FUNCTION               0x02
#define MSOS20_FEATURE_COMPATIBLE_ID                0x03
#define MSOS20_FEATURE_REGISTRY_PROPERTY            0x04
#define MSOS20_FEATURE_MINIMUM_RESUME_TIME          0x05
#define MSOS20_FEATURE_MODEL_ID                     0x06
#define MSOS20_FEATURE_CCGP_DEVICE                  0x07


//
// Microsoft OS 2.0 Descriptor Set Information
//
typedef struct _MSOS20_DESCRIPTOR_SET_INFO {
    ULONG                       dwWindowsVersion;
    USHORT                      wLength;
    UCHAR                       bMS_VendorCode;
    UCHAR                       bAltEnumCmd;
} MSOS20_DESCRIPTOR_SET_INFO, *PMSOS20_DESCRIPTOR_SET_INFO;


//
// Microsoft OS 2.0 Platform Capability Descriptor Header
//
typedef struct _MSOS20_PLATFORM_CAP_DESC_HEADER {
    UCHAR                               bLength;
    UCHAR                               bDescriptorType;
    UCHAR                               bDevCapabilityType;
    UCHAR                               bReserved;
    GUID                                MSOS20_Platform_Capability_ID;
    MSOS20_DESCRIPTOR_SET_INFO          DescriptorSetInfo[1];
} MSOS20_PLATFORM_CAP_DESC_HEADER, *PMSOS20_PLATFORM_CAP_DESC_HEADER;


//
// Microsoft OS 2.0 Common Descriptor
//
typedef struct _MSOS20_COMMON_DESCRIPTOR {
    USHORT  wLength;                // Length of descriptor
    USHORT  wDescriptorType;        // Descriptor Type
} MSOS20_COMMON_DESCRIPTOR, *PMSOS20_COMMON_DESCRIPTOR;


//
// Microsoft OS 2.0 Descriptor Set Header
//
typedef struct _MSOS20_DESCRIPTOR_SET_HEADER {
    USHORT  wLength;                // Length of this header
    USHORT  wDescriptorType;        // MSOS20_SET_HEADER_DESCRIPTOR
    ULONG   dwWindowsVersion;       // Windows Version for this descriptor set
    USHORT  wTotalLength;           // Length of the entire MS OS 2.0 descriptor set
} MSOS20_DESCRIPTOR_SET_HEADER, *PMSOS20_DESCRIPTOR_SET_HEADER;


//
// Microsoft OS 2.0 Configuration Subset Header
//
typedef struct _MSOS20_CONFIGURATION_SUBSET_HEADER {
    USHORT  wLength;                // Length of this header
    USHORT  wDescriptorType;        // MSOS20_SUBSET_HEADER_CONFIGURATION
    UCHAR   bConfigurationValue;    // Configuration value this subset applies to
    UCHAR   bReserved;              // Must be 0
    USHORT  wTotalLength;           // Size of this subset, including this header
} MSOS20_CONFIGURATION_SUBSET_HEADER, *PMSOS20_CONFIGURATION_SUBSET_HEADER;


//
// Microsoft OS 2.0 Function Subset Header
//
typedef struct _MSOS20_FUNCTION_SUBSET_HEADER {
    USHORT  wLength;                // Length of this header
    USHORT  wDescriptorType;        // MSOS20_SUBSET_HEADER_FUNCTION
    UCHAR   bFirstInterface;        // First interface of the function this subset applies to
    UCHAR   bReserved;              // Must be 0
    USHORT  wTotalLength;           // Size of this subset, including this header
} MSOS20_FUNCTION_SUBSET_HEADER, *PMSOS20_FUNCTION_SUBSET_HEADER;


//
// Microsoft OS 2.0 Compatible ID Feature Descriptor
//
typedef struct _MSOS20_FEATURE_COMPATIBLE_ID_DESCRIPTOR {
    USHORT  wLength;                // Length of this descriptor
    USHORT  wDescriptorType;        // MSOS20_FEATURE_COMPATIBLE_ID
    UCHAR   CompatibleId[8];        // Compatible ID String
    UCHAR   SubCompatibleId[8];     // Sub Compatible ID String
} MSOS20_FEATURE_COMPATIBLE_ID_DESCRIPTOR, *PMSOS20_FEATURE_COMPATIBLE_ID_DESCRIPTOR;


//
// Microsoft OS 2.0 Registry Value Feature Descriptor
//
typedef struct _MSOS20_FEATURE_REGISTRY_VALUE_DESCRIPTOR {
    USHORT  wLength;                // Length of this descriptor
    USHORT  wDescriptorType;        // MSOS20_FEATURE_REGISTRY_VALUE
    USHORT  wPropertyDataType;      // Type of registry property type
    USHORT  wPropertyNameLength;    // Length of the property name
    WCHAR   PropertyName[1];        // Name of the registry property
} MSOS20_FEATURE_REGISTRY_VALUE_DESCRIPTOR, *PMSOS20_FEATURE_REGISTRY_VALUE_DESCRIPTOR;

//
// Microsoft OS 2.0 Registry Property Data
//
typedef struct _MSOS20_FEATURE_REGISTRY_VALUE_PROPERTY_DATA {
    USHORT  wPropertyDataLength;    // Length of the property data
    UCHAR   PropertyData[1];        // Property data
} MSOS20_FEATURE_REGISTRY_VALUE_PROPERTY_DATA, *PMSOS20_FEATURE_REGISTRY_VALUE_PROPERTY_DATA;


//
// Microsoft OS 2.0 Minimum Recovery Time Feature Descriptor
//
typedef struct _MSOS20_FEATURE_MINIMUM_RESUME_TIME_DESCRIPTOR {
    USHORT  wLength;                // Length of this descriptor
    USHORT  wDescriptorType;        // MSOS20_FEATURE_MINIMUM_RECOVERY_TIME
    UCHAR   bResumeRecoveryTime;    // Number of milliseconds needed to recover from resume
    UCHAR   bResumeSignalingTime;   // Number of milliseconds needed for resume signaling
} MSOS20_FEATURE_MINIMUM_RESUME_TIME_DESCRIPTOR, *PMSOS20_FEATURE_MINIMUM_RESUME_TIME_DESCRIPTOR;


//
// Microsoft OS 2.0 Model ID Feature Descriptor
//
typedef struct _MSOS20_FEATURE_MODEL_ID_DESCRIPTOR {
    USHORT  wLength;                // Length of this descriptor
    USHORT  wDescriptorType;        // MSOS20_FEATURE_MODEL_ID
    GUID    ModelId;                // Model ID GUID
} MSOS20_FEATURE_MODEL_ID_DESCRIPTOR, *PMSOS20_FEATURE_MODEL_ID_DESCRIPTOR;


//
// Microsoft OS 2.0 CCGP Device Descriptor
//
typedef struct _MSOS20_FEATURE_CCGP_DEVICE_DESCRIPTOR {
    USHORT  wLength;                // Length of this descriptor
    USHORT  wDescriptorType;        // MSOS20_FEATURE_CCGP_DEVICE
} MSOS20_FEATURE_CCGP_DEVICE_DESCRIPTOR, *PMSOS20_FEATURE_CCGP_DEVICE_DESCRIPTOR;


#include <poppack.h>

#endif /* __MSOS20DESC_H__ */

