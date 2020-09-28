
/*++
Copyright (c) Microsoft Corporation

Module Name:

    descvalidation.c

Abstract:

    Code to parse and validate different USB descriptors

Author:

Environment:

    Kernel mode only.

--*/

#include "pch.h"
#include "descvalidation.tmh"
    
#define HIGHEST_KNOWN_LS_VERSION 0x0210
#define HIGHEST_KNOWN_FS_VERSION 0x0210
#define HIGHEST_KNOWN_HS_VERSION 0x0210
#define HIGHEST_KNOWN_SS_VERSION 0x0310
#define HIGHEST_KNOWN_USB2_VERSION 0x0210
#define HIGHEST_KNOWN_USB3_VERSION 0x0310

typedef enum {
    Usb20CapDescriptor,
    Usb30CapDescriptor,
    Usb31CapDescriptor
} USB_CAP_DESCRIPTOR_TYPE, *PUSB_CAP_DESCRIPTOR_TYPE;

BOOLEAN
HUBDESC_ShouldEnforceWin8ValidationMutableCapDescriptor(
    __in
        PVALIDATION_DEVICE_INFO     DevInfo,
    __in
        USB_CAP_DESCRIPTOR_TYPE     DescriptorType
    )
/*++

Routine Description:

    Checks the device properties to determine if descriptor validation failures of
    speed specific descriptors that are also included when operating in different speeds
    should result in blocking enumeration.  

--*/
{
    if (DevInfo->EnforceStrictestParsing) {
        return TRUE;
    }

    if ((DescriptorType == Usb31CapDescriptor) &&
        (DevInfo->USBVersion >= 0x0310) &&
        (DevInfo->USBVersion <= HIGHEST_KNOWN_USB3_VERSION)) {
        return TRUE;
    }

    if ((DescriptorType == Usb30CapDescriptor) && 
        (DevInfo->USBVersion >= 0x0300) &&
        (DevInfo->USBVersion <= HIGHEST_KNOWN_USB3_VERSION)) {
        return TRUE;
    }

    if ((DescriptorType == Usb20CapDescriptor) &&
        (DevInfo->USBVersion > 0x0200) &&
        (DevInfo->USBVersion < HIGHEST_KNOWN_USB2_VERSION)) {
        return TRUE;
    }

    return FALSE;
}


BOOLEAN
HUBDESC_ShouldEnforceMutableReservedFields(
    __in
        PVALIDATION_DEVICE_INFO     DevInfo
    )
/*++

Routine Description:

    Checks the device properties to determine if reserved field descriptor validation
    failures should result in blocking enumeration.  This should be called for
    those aspects that have known definition today, but could change without updating
    the device's USBVersion.

--*/
{
    if (DevInfo->ValidateReservedFields) {
        return TRUE;
    }

    return FALSE;
}

BOOLEAN
HUBDESC_ShouldEnforceWin8ValidationImmutable(
    __in
        PVALIDATION_DEVICE_INFO     DevInfo
    )
/*++

Routine Description:

    Checks the device properties to determine if new descriptor validation
    failures should result in blocking enumeration.  This should be called for
    those aspects that cannot possibly be changed in future USB spec revisions

--*/
{
    if (DevInfo->USBVersion >= 0x0300) {
        return TRUE;
    }

    if (DevInfo->USBVersion > 0x0200) {
        return TRUE;
    }

    if (DevInfo->EnforceStrictestParsing) {
        return TRUE;
    }

    return FALSE;
}

BOOLEAN
HUBDESC_ShouldEnforceWin8ValidationMutable(
    __in
        PVALIDATION_DEVICE_INFO     DevInfo
    )
/*++

Routine Description:

    Checks the device properties to determine if new descriptor validation
    failures should result in blocking enumeration.  This should be called for
    those aspects that could possibly be changed in future USB spec revisions

--*/
{
    if ((DevInfo->USBVersion >= 0x0300) &&
        (DevInfo->USBVersion <= HIGHEST_KNOWN_USB3_VERSION)) {
        return TRUE;
    }

    if ((DevInfo->USBVersion > 0x0200) &&
        (DevInfo->USBVersion <= HIGHEST_KNOWN_USB2_VERSION)) {
        return TRUE;
    }

    if (DevInfo->EnforceStrictestParsing) {
        return TRUE;
    }

    return FALSE;
}       

BOOLEAN
HUBDESC_ShouldEnforceWin7StrictValidation(
    __in
        PVALIDATION_DEVICE_INFO     DevInfo
    )
/*++

Routine Description:

    Checks the device properties to determine if stricter Win7 descriptor validation
    failures should result in blocking enumeration.  This should be called for
    those aspects that cannot possibly be changed in future USB spec revisions

--*/
{
    if (HUBDESC_ShouldEnforceWin8ValidationImmutable(DevInfo)) {
        return TRUE;
    }

    if (DevInfo->EnableWin7StricterParsing) {
        return TRUE;
    }

    return FALSE;

}

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
PUSB_COMMON_DESCRIPTOR
HUBDESC_ParseDescriptors(
    __in_bcount(TotalLength)
        PVOID               DescriptorBuffer,
    __in
        size_t              TotalLength,
    __in
        PVOID               StartPosition,
    __in
        LONG                DescriptorType,
    __in
        RECORDER_LOG        IfrLog
    )
/*++

Routine Description:

    Parses a group of standard USB configuration descriptors (returned
    from a device) for a specific descriptor type.

    Ported from Windows 7 USBHUB driver

Arguments:

    DescriptorBuffer - Configuration descriptor set.

    StartPosition - Starting position in the buffer to begin parsing.  This
                    must point to the begining of a USB descriptor.

    DescriptorType - USB descritor type to locate.

Return Value:

    Pointer to a USB descriptor with a matching descriptor type.

    NULL if not found.

--*/
{
    PUCHAR                  currentDescriptor;
    PUCHAR                  endDescriptorSet;
    PUSB_COMMON_DESCRIPTOR  usbDescriptor;
    PUSB_COMMON_DESCRIPTOR  foundUsbDescriptor;

    TRY {

        currentDescriptor = (PUCHAR)StartPosition;
        endDescriptorSet = ((PUCHAR)(DescriptorBuffer)) + TotalLength;

        foundUsbDescriptor = NULL;

        while (currentDescriptor < endDescriptorSet) {

            usbDescriptor = (PUSB_COMMON_DESCRIPTOR)currentDescriptor;

            //
            // Exit if the current descriptor has an invalid length
            //
            if (usbDescriptor->bLength == 0) {
                DbgTraceTo(IfrLog, TL_ERROR, Device, "HW_COMPLIANCE:Descriptor has bLength of 0!");
                break;
            }

            //
            // Check if we've found a matching descriptor
            //
            if (usbDescriptor->bDescriptorType == DescriptorType) {

                foundUsbDescriptor = usbDescriptor;
                break;
            }

            currentDescriptor += usbDescriptor->bLength;
        }

    } FINALLY {

    }

    return foundUsbDescriptor;
}


__drv_maxIRQL(DISPATCH_LEVEL)
PUSB_INTERFACE_DESCRIPTOR
HUBDESC_ParseConfigurationDescriptor(
    __in
        PUSB_CONFIGURATION_DESCRIPTOR   ConfigurationDescriptor,
    __in
        PVOID                           StartPosition,
    __in
        LONG                            InterfaceNumber,
    __in
        LONG                            AlternateSetting,
    __in
        LONG                            InterfaceClass,
    __in
        LONG                            InterfaceSubClass,
    __in
        LONG                            InterfaceProtocol,
    __in_opt
        PBOOLEAN                        HasAlternateSettings,
    __in
        RECORDER_LOG                    IfrLog
    )
/*++

Routine Description:

    Parses a standard USB configuration descriptor (returned from a device)
    for a specific interface, alternate setting class subclass or protocol
    codes.

    Ported from Windows 7 USBHUB driver

Arguments:

    ConfigurationDescriptor - Pointer to a configuartion descriptor, including
                              all the interface and endpoint data.

    StartPosition - Starting postition in the buffer to begin searching.

    InterfaceNumber - Interface number to locate, -1 means is wildcard.

    AlternateSetting - Alternate setting to locate, -1 means is wildcard.

    InterfaceClass - Interface calss code to locate, -1 means is wildcard.

    InterfaceSubClass - Interface subclass code to locate, -1 means is wildcard.

    InterfaceProtocol - Interface protocol to locate in buffer, -1 means is wildcard.

Return Value:

    Pointer to the matching interface descriptor.

    NULL is returned if no descriptor matches the input parameters.

--*/
{
    PUSB_INTERFACE_DESCRIPTOR   foundInterfaceDescriptor;
    PUSB_INTERFACE_DESCRIPTOR   interfaceDescriptor;
    PUSB_INTERFACE_DESCRIPTOR   matchingInterfaceDescriptor;
    ULONG                       matchingInterfaceCount;

    TRY {

        foundInterfaceDescriptor = NULL;
        matchingInterfaceDescriptor = NULL;
        matchingInterfaceCount = 0;

        if (HasAlternateSettings != NULL) {
            *HasAlternateSettings = FALSE;
        }

        //
        // Minimum validation of ConfigurationDescriptor arg
        //
        if (ConfigurationDescriptor->bLength <
            sizeof(USB_CONFIGURATION_DESCRIPTOR)) {

            LEAVE;
        }

        if (ConfigurationDescriptor->bDescriptorType !=
            USB_CONFIGURATION_DESCRIPTOR_TYPE) {

            LEAVE;
        }

        if (ConfigurationDescriptor->wTotalLength <
            sizeof(USB_CONFIGURATION_DESCRIPTOR)) {

            LEAVE;
        }

        
        //
        // Walk the table of descriptors looking for an interface descriptor
        // with parameters matching those passed in.
        //
        do {

            //
            // Search for a descriptor of type USB_INTERFACE_DESCRIPTOR_TYPE
            //
            interfaceDescriptor =
                (PUSB_INTERFACE_DESCRIPTOR)HUBDESC_ParseDescriptors(
                                                ConfigurationDescriptor,
                                                ConfigurationDescriptor->wTotalLength,
                                                StartPosition,
                                                USB_INTERFACE_DESCRIPTOR_TYPE,
                                                IfrLog);

            //
            // Check if it matches all of our parameters
            //
            if (interfaceDescriptor != NULL) {

                foundInterfaceDescriptor = interfaceDescriptor;

                if (InterfaceNumber != -1) {

                    if (interfaceDescriptor->bInterfaceNumber != InterfaceNumber) {
                        foundInterfaceDescriptor = NULL;
                    } else {
                        matchingInterfaceCount++;
                    }
                }

                if (AlternateSetting != -1 &&
                    interfaceDescriptor->bAlternateSetting !=
                    AlternateSetting) {

                    foundInterfaceDescriptor = NULL;
                }

                if (InterfaceClass != -1 &&
                    interfaceDescriptor->bInterfaceClass !=
                    InterfaceClass) {

                    foundInterfaceDescriptor = NULL;
                }

                if (InterfaceSubClass != -1 &&
                    interfaceDescriptor->bInterfaceSubClass !=
                        InterfaceSubClass) {

                    foundInterfaceDescriptor = NULL;
                }

                if (InterfaceProtocol != -1 &&
                    interfaceDescriptor->bInterfaceProtocol !=
                    InterfaceProtocol) {

                    foundInterfaceDescriptor = NULL;
                }

                StartPosition = ((PUCHAR)interfaceDescriptor) + interfaceDescriptor->bLength;
            }

            if (matchingInterfaceDescriptor != NULL) {
                if ((matchingInterfaceCount > 1) && (HasAlternateSettings != NULL)) {
                    *HasAlternateSettings = TRUE;
                }

                break;
            }

            if (foundInterfaceDescriptor != NULL) {

                matchingInterfaceDescriptor = foundInterfaceDescriptor;

                if (HasAlternateSettings == NULL) {
                    break;
                } 
                
                if (matchingInterfaceCount > 1) {
                    *HasAlternateSettings = TRUE;
                    break;
                }
            }

        } while (interfaceDescriptor != NULL);

    } FINALLY {

    }

    return (matchingInterfaceDescriptor);
}

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
HUBDESC_InternalValidateCommonDescriptorHeader(
    __in
        PUSB_COMMON_DESCRIPTOR          CommonDesc,
    __in
        PINTERNAL_VALIDATION_DATA       ValidationData,
    __out
        PULONG                          AdjustedDescriptorLength,
    __inout_opt
        PVALIDATION_RESULT              ValidationResult,
    __in
        RECORDER_LOG                    IfrLog
    )
/*++

Routine Description:

    Performs validation of a common descriptor header.


--*/
{
    VALIDATION_RESULT       localResult;
    BOOLEAN                 result;    
    ULONG                   bytesLeft;
    ULONG                   bufferLeft;
    ULONG                   offset;

    TRY {
        
        if (ValidationResult == NULL) {
            ValidationResult = &localResult;
            *ValidationResult = ValidationSuccess;
        } else if (ValidationData->DevInfo.UseWin8DescriptorValidationBehavior) {
            *ValidationResult = ValidationSuccess;
        }

        *AdjustedDescriptorLength = 0;

        bytesLeft = (ULONG) (ValidationData->DescriptorSetEnd - (PUCHAR) CommonDesc);       
        offset = (ULONG)((PUCHAR)CommonDesc - ValidationData->DescriptorSetBeginning);
        bufferLeft = ValidationData->BufferLength - offset;

        if (bufferLeft < sizeof(USB_COMMON_DESCRIPTOR)) {

            *ValidationResult = ValidationFatalError;
            
            DbgTraceTo(IfrLog,
                       TL_ERROR, 
                       Device, 
                       "HW_COMPLIANCE:Descriptor Validation Failed due to a descriptor at offset 0x%x being defined too close to the end of the buffer to contain even the standard descriptor header (0x%x).  Aborting further validation.",
                       bufferLeft,
                       sizeof(USB_COMMON_DESCRIPTOR));
            
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorHeaderBufferTooSmall);

            LEAVE;

        }

        //
        // It is now OK to look at the common descriptor fields
        //

        *AdjustedDescriptorLength = CommonDesc->bLength;            


        if (CommonDesc->bLength > bufferLeft) {
            
            *ValidationResult = ValidationFailed;
                        
            DbgTraceTo(IfrLog,
                       TL_ERROR, 
                       Device, 
                       "HW_COMPLIANCE:Descriptor Validation Failed due to descriptor->bLength (0x%x) of descriptor at offset (0x%x) exceeding the number of bytes remaining in the descriptor set buffer (0x%x).  Assuming bLength is 0x%x for further validation.",
                       CommonDesc->bLength,
                       offset,
                       bufferLeft,
                       bufferLeft);              
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorHeaderBlengthLargerThanRemainingBuffer);
            *AdjustedDescriptorLength = bufferLeft;
            
        }

        if (CommonDesc->bLength > bytesLeft) {
                
            *ValidationResult = ValidationFailed;
            
            DbgTraceTo(IfrLog,
                       TL_ERROR, 
                       Device, 
                       "HW_COMPLIANCE:Descriptor Validation Failed due to descriptor->bLength (0x%x) of descriptor at offset (0x%x) exceeding the number of bytes remaining in the descriptor set according to wTotalLength (0x%x).  Assuming wTotalLength is incorrect for further validation.",
                       CommonDesc->bLength,
                       offset,
                       bytesLeft);                         
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorHeaderBlengthLargerThanWTotalLength);

        }

        if (CommonDesc->bLength < sizeof(USB_COMMON_DESCRIPTOR)) {

            if (HUBDESC_ShouldEnforceWin8ValidationImmutable(&ValidationData->DevInfo)) {

                *ValidationResult = ValidationFailed;

            }
            
            DbgTraceTo(IfrLog,
                       TL_ERROR, 
                       Device, 
                       "HW_COMPLIANCE:Descriptor Validation Failed due to descriptor->bLength (0x%x) of descriptor at offset (0x%x) being less than minimum for a descriptor header (0x%x).  Assuming bLength is correct for further validation.",
                       CommonDesc->bLength,
                       offset,
                       sizeof(USB_COMMON_DESCRIPTOR));       
            
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorHeaderBlengthTooSmall);

        }

        //
        // A zero length descriptor can cause serious parsing issues 
        //
        if (CommonDesc->bLength == 0) {

            if (HUBDESC_ShouldEnforceWin8ValidationImmutable(&ValidationData->DevInfo)) {

                *ValidationResult = ValidationFatalError;

            }
            DbgTraceTo(IfrLog,
                       TL_ERROR, 
                       Device, 
                       "HW_COMPLIANCE:Descriptor Validation Failed due to descriptor->bLength of descriptor at offset (0x%x) being set to zero.  Aborting further validation.",
                       offset); 
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorHeaderZeroBlength);


        }

    } FINALLY {

        if (*ValidationResult == ValidationSuccess) {
            result = TRUE;
        } else {
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in the descriptor header.");
            result = FALSE;
        }

    }

    return result;

}


__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBDESC_InitializeConfigValidationData (
    __in_bcount(BufferLength)
        PUSB_CONFIGURATION_DESCRIPTOR       ConfigDesc,
    __in
        ULONG                               BufferLength,
    __in
        PVALIDATION_DEVICE_INFO             DevInfo,
    __out
        PINTERNAL_CONFIG_VALIDATION_DATA    ConfigValidationData
    )
/*++

Routine Description:

    Performs validation of a configuration descriptor

--*/
{
    
    TRY {
        
        RtlZeroMemory(ConfigValidationData,
                      sizeof(INTERNAL_CONFIG_VALIDATION_DATA));

        

        
        //
        // Since RtlInitializeBitmap is paged, we can't call it, so instead we call our own version
        //
        HUBMISC_InitializeBitmap(&ConfigValidationData->InterfaceNumberBitmap,
                                 ConfigValidationData->InterfaceNumbers,
                                 256);
        HUBMISC_InitializeBitmap(&ConfigValidationData->AlternateSettingsBitmap,
                                 ConfigValidationData->AlternateSettings,
                                 256);
        HUBMISC_InitializeBitmap(&ConfigValidationData->EndpointAddressesBitmap,
                                 ConfigValidationData->EndpointAddresses,
                                 256);

        RtlClearAllBits(&ConfigValidationData->InterfaceNumberBitmap);
        RtlClearAllBits(&ConfigValidationData->AlternateSettingsBitmap);
        RtlClearAllBits(&ConfigValidationData->EndpointAddressesBitmap);

        ConfigValidationData->DescriptorSetBeginning = (PUCHAR)ConfigDesc;
        ConfigValidationData->DescriptorSetEnd = (PUCHAR)ConfigDesc + BufferLength;
        ConfigValidationData->BufferLength = BufferLength;
        ConfigValidationData->DevInfo = *DevInfo;

    } FINALLY {

    }

    return;

}

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
HUBDESC_InternalValidateConfigDescriptor(
    __in
        PUSB_CONFIGURATION_DESCRIPTOR       ConfigDesc,
    __inout
        PINTERNAL_CONFIG_VALIDATION_DATA    ConfigValidationData,
    __out
        PULONG                              AdjustedDescriptorLength,
    __inout_opt
        PVALIDATION_RESULT                  ValidationResult,
    __in
        RECORDER_LOG                        IfrLog
    )
/*++

Routine Description:

    Performs validation of a configuration descriptor itself.

--*/
{
    VALIDATION_RESULT   localResult;
    BOOLEAN             result;    
    ULONG               minimumSize;

    TRY {
        
        if (ValidationResult == NULL) {
            ValidationResult = &localResult;
            *ValidationResult = ValidationSuccess;
        } else if (ConfigValidationData->DevInfo.UseWin8DescriptorValidationBehavior) {
            *ValidationResult = ValidationSuccess;
        }

        *AdjustedDescriptorLength = sizeof(USB_CONFIGURATION_DESCRIPTOR);
        
        //
        // Validate some basic header information.  We know the header is in-bounds.
        //
        if (ConfigDesc->bLength < sizeof(USB_CONFIGURATION_DESCRIPTOR)) {   

            *ValidationResult = ValidationFatalError;
            
            DbgTraceTo(IfrLog,
                       TL_ERROR, 
                       Device, 
                       "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to ConfigDesc->bLength (0x%x) being less than expected (0x%x).  Assuming that bLength is (0x%x) for further validation.",
                       ConfigDesc->bLength,
                       sizeof(USB_CONFIGURATION_DESCRIPTOR),
                       sizeof(USB_CONFIGURATION_DESCRIPTOR));
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorConfigBlengthTooSmall);

            //
            // Lets ignore the failure, and continue parsing as if everything was ok
            //

        }

        //
        // This wasn't traditionally checked, but it was assumed in further validation
        //
        if (ConfigDesc->bLength > sizeof(USB_CONFIGURATION_DESCRIPTOR)) {

            if (HUBDESC_ShouldEnforceWin8ValidationImmutable(&ConfigValidationData->DevInfo)) {

                *ValidationResult = ValidationFailed;

            }
            
            DbgTraceTo(IfrLog,
                       TL_ERROR, 
                       Device, 
                       "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to ConfigDesc->bLength (0x%x) being greater than expected (0x%x). Assuming that bLength is (0x%x) for further validation.",
                       ConfigDesc->bLength,
                       sizeof(USB_CONFIGURATION_DESCRIPTOR),
                       sizeof(USB_CONFIGURATION_DESCRIPTOR));
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorConfigBlengthTooLarge);

        }

        if (ConfigDesc->bDescriptorType != USB_CONFIGURATION_DESCRIPTOR_TYPE) {

            *ValidationResult = ValidationFatalError;
            
            DbgTraceTo(IfrLog,
                       TL_ERROR, 
                       Device, 
                       "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to value of ConfigDesc->bDescriptorType (0x%x) being different than expected (0x%x). Assuming that bDescriptorType is 0x%x for further validation.",
                       ConfigDesc->bDescriptorType,
                       USB_CONFIGURATION_DESCRIPTOR_TYPE,
                       USB_CONFIGURATION_DESCRIPTOR_TYPE);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorConfigInvalidType);

            //
            // Continue looking for more failures
            //
        }

        if (*AdjustedDescriptorLength > ConfigValidationData->BufferLength) {
            if (HUBDESC_ShouldEnforceWin7StrictValidation(&ConfigValidationData->DevInfo)) {

                *ValidationResult = ValidationFailed;

            }

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to insufficient buffer length for a Configuration Descriptor. Aborting further validation.");
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorConfigBufferTooSmall);
            LEAVE;
        }

        minimumSize = sizeof(USB_CONFIGURATION_DESCRIPTOR) +
            ConfigDesc->bNumInterfaces*sizeof(USB_INTERFACE_DESCRIPTOR);

        if (minimumSize > ConfigValidationData->BufferLength) {
            
            if (HUBDESC_ShouldEnforceWin7StrictValidation(&ConfigValidationData->DevInfo)) {

                *ValidationResult = ValidationFailed;

            }
            
            DbgTraceTo(IfrLog,
                       TL_ERROR, 
                       Device, 
                       "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to buffer length (0x%x) being less than the minimum to hold the configuration descriptor and the interface descriptors (0x%x).  Performing further validation on remaining buffer.",
                       ConfigValidationData->BufferLength,
                       minimumSize);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorConfigBufferTooSmallToHaveInterface);

        }

        ConfigValidationData->DescriptorSetEnd = ConfigValidationData->DescriptorSetBeginning + ConfigDesc->wTotalLength;

        if (ConfigDesc->wTotalLength > ConfigValidationData->BufferLength) {

            if (HUBDESC_ShouldEnforceWin7StrictValidation(&ConfigValidationData->DevInfo)) {

                *ValidationResult = ValidationFailed;

            }

            DbgTraceTo(IfrLog,
                       TL_ERROR, 
                       Device, 
                       "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to value of ConfigDesc->wTotalLength (0x%x) exceeding the buffer length (0x%x). Assuming that wTotalLength is 0x%x for further validation.",
                       ConfigDesc->wTotalLength,
                       ConfigValidationData->BufferLength,
                       ConfigValidationData->BufferLength);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorConfigWTotalLengthTooLarge);

            ConfigValidationData->DescriptorSetEnd = ConfigValidationData->DescriptorSetBeginning + ConfigValidationData->BufferLength;
            
        } 

        if (ConfigDesc->wTotalLength < minimumSize) {

            if (HUBDESC_ShouldEnforceWin7StrictValidation(&ConfigValidationData->DevInfo)) {

                *ValidationResult = ValidationFailed;

            }

            DbgTraceTo(IfrLog,
                       TL_ERROR, 
                       Device, 
                       "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to value of ConfigDesc->wTotalLength (0x%x) being less than the minimum to hold the configuration descriptor and the interface descriptors (0x%x). Assuming wTotalLength is 0x%x for further validation.",
                       ConfigDesc->wTotalLength,
                       minimumSize,
                       ConfigValidationData->BufferLength);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorConfigWTotalLengthTooSmallToHaveInterface);

            //
            // Value is probably just totally wrong, so lets parse everything
            //
            ConfigValidationData->DescriptorSetEnd = ConfigValidationData->DescriptorSetBeginning + ConfigValidationData->BufferLength;
        }

    } FINALLY {
      
        if (*ValidationResult == ValidationSuccess) {
            result = TRUE;
        } else {
            result = FALSE;
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in configuration descriptor.");
        }

    }

    return result;

}




__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
HUBDESC_InternalValidateLastEndpoint (
    __inout
        PINTERNAL_CONFIG_VALIDATION_DATA    ConfigValidationData,
    __inout_opt
        PVALIDATION_RESULT                  ValidationResult,
    __in
        RECORDER_LOG                        IfrLog
    )
/*++

Routine Description:

    Validates that all appropriate descriptors were found since the last endpoint
    descriptor

--*/
{
    VALIDATION_RESULT   localResult;
    BOOLEAN             result;    

    TRY {
                
        if (ValidationResult == NULL) {
            ValidationResult = &localResult;
            *ValidationResult = ValidationSuccess;
        } else if (ConfigValidationData->DevInfo.UseWin8DescriptorValidationBehavior) {
            *ValidationResult = ValidationSuccess;
        }

        if (ConfigValidationData->ValidationFlags.FirstEndpointFound == 0) {
            ConfigValidationData->ValidationFlags.FirstEndpointFound = 1;
            LEAVE;
        }

        if (ConfigValidationData->DevInfo.USBSpeed == UsbSuperSpeed) {
            if (ConfigValidationData->ValidationFlags.EndpointCompanionFound == 0) {

                *ValidationResult = ValidationFailed;
                DbgTraceTo(IfrLog,
                           TL_ERROR, 
                           Device, 
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to an endpoint descriptor not immediately followed by an endpoint companion descriptor.  Ignoring failure for further validation");
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorConfigMissingCompanionEndpoint);
            }

            if ((ConfigValidationData->ValidationFlags.ExpectingSSPEndpointCompanion != 0) &&
                (ConfigValidationData->ValidationFlags.SSPEndpointCompanionFound == 0)) {

                *ValidationResult = ValidationFailed;
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to an endpoint companion descriptor not immediately followed by a superspeedplus endpoint companion descriptor.  Ignoring failure for further validation");
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorConfigMissingSSPCompanionEndpoint);
            }

        }

    } FINALLY {

        ConfigValidationData->ValidationFlags.EndpointCompanionFound = 0;
        ConfigValidationData->ValidationFlags.ExpectingEndpointCompanion = 0;
        ConfigValidationData->ValidationFlags.SSPEndpointCompanionFound = 0;
        ConfigValidationData->ValidationFlags.ExpectingSSPEndpointCompanion = 0;

        if (*ValidationResult == ValidationSuccess) {
            result = TRUE;
        } else {
            result = FALSE;
        }

    }

    return result;

}


__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
HUBDESC_InternalValidateLastInterface (
    __inout
        PINTERNAL_CONFIG_VALIDATION_DATA    ConfigValidationData,
    __inout_opt
        PVALIDATION_RESULT                  ValidationResult,
    __in
        RECORDER_LOG                        IfrLog
    )
/*++

Routine Description:

    Validates that all appropriate descriptors were found since the last interface
    descriptor

--*/
{
    VALIDATION_RESULT   localResult;
    BOOLEAN             result;    

    TRY {
        
        if (ValidationResult == NULL) {
            ValidationResult = &localResult;
            *ValidationResult = ValidationSuccess;
        } else if (ConfigValidationData->DevInfo.UseWin8DescriptorValidationBehavior) {
            *ValidationResult = ValidationSuccess;
        }

        if (ConfigValidationData->ValidationFlags.FirstInterfaceFound == 0) {

            ConfigValidationData->ValidationFlags.FirstInterfaceFound = 1;
            LEAVE;

        }
        
        //
        // Ensure we found everything we were supposed to since the last endpoint descriptor
        //
        HUBDESC_InternalValidateLastEndpoint(ConfigValidationData,
                                             ValidationResult,
                                             IfrLog);
        if (*ValidationResult == ValidationFatalError) {
            LEAVE;
        }

        if (ConfigValidationData->EndpointCount != ConfigValidationData->NumberEndpoints) {
            if (HUBDESC_ShouldEnforceWin7StrictValidation(&ConfigValidationData->DevInfo)) {
                
                *ValidationResult = ValidationFailed;

            }

            DbgTraceTo(IfrLog,
                       TL_ERROR, 
                       Device, 
                       "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the number of endpoint descriptors found (0x%x) for an interface was not equal to the number specified (0x%x) in the corresponding interface descriptor (bInterfaceNumber 0x%x and bAlternateSetting 0x%x).  Ignoring failure for further validation.",
                       ConfigValidationData->EndpointCount,
                       ConfigValidationData->NumberEndpoints,
                       ConfigValidationData->CurrentInterfaceNumber,
                       ConfigValidationData->CurrentAltSetting);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorInterfaceParsedEndpointCountTooSmall);

        }

        

    } FINALLY {

        if (*ValidationResult == ValidationSuccess) {
            result = TRUE;
        } else {
            result = FALSE;
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in numbering of endpoint/interface descriptors.");
        }

    }

    return result;

}


__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
HUBDESC_InternalValidateEndpointDescriptor(
    __in
        PUSB_ENDPOINT_DESCRIPTOR            EndpointDesc,
    __inout
        PINTERNAL_CONFIG_VALIDATION_DATA    ConfigValidationData,
    __out
        PULONG                              AdjustedDescriptorLength,
    __inout_opt
        PVALIDATION_RESULT                  ValidationResult,
    __in
        RECORDER_LOG                        IfrLog
    )
/*++

Routine Description:

    Performs validation of an endpoint descriptor.

--*/
{
    VALIDATION_RESULT           localResult;
    BOOLEAN                     result;    
    ULONG                       bufferLeft;
    ULONG                       offset;
    UCHAR                       endpointAddress;
    UCHAR                       endpointType;
    USB_HIGH_SPEED_MAXPACKET    muxPacket;

    TRY {

        if (ValidationResult == NULL) {
            ValidationResult = &localResult;
            *ValidationResult = ValidationSuccess;
        } else if (ConfigValidationData->DevInfo.UseWin8DescriptorValidationBehavior) {
            *ValidationResult = ValidationSuccess;
        }

        offset = (ULONG)((PUCHAR)EndpointDesc - ConfigValidationData->DescriptorSetBeginning);        
        bufferLeft = (ULONG)(ConfigValidationData->BufferLength - offset);

        *AdjustedDescriptorLength = EndpointDesc->bLength;

        //
        // Lets first determine if the last endpoint was good
        //
        HUBDESC_InternalValidateLastEndpoint(ConfigValidationData,
                                             ValidationResult,
                                             IfrLog);

        if (*ValidationResult == ValidationFatalError) {
            LEAVE;
        }

        if (EndpointDesc->bLength < sizeof(USB_ENDPOINT_DESCRIPTOR)){

            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorEndpointBlengthTooSmall);
            
            //
            // if we can, lets assume that the descriptor is the correct size
            //
            if (sizeof(USB_ENDPOINT_DESCRIPTOR) <= bufferLeft) {
                *AdjustedDescriptorLength = sizeof(USB_ENDPOINT_DESCRIPTOR);
            }

            if (HUBDESC_ShouldEnforceWin7StrictValidation(&ConfigValidationData->DevInfo)) {

                *ValidationResult = ValidationFailed;

            }

            if (*AdjustedDescriptorLength < sizeof(USB_ENDPOINT_DESCRIPTOR)) {

                *ValidationResult = ValidationFatalError;
                
                DbgTraceTo(IfrLog,
                           TL_ERROR, 
                           Device, 
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the bLength value (0x%x) of the endpoint descriptor at offset (0x%x) is less than expected (0x%x). Aborting further validation as buffer is insufficient to validate other fields of the endpoint descriptor.",
                           EndpointDesc->bLength,
                           offset,
                           sizeof(USB_ENDPOINT_DESCRIPTOR));
                LEAVE;

            } else {

                DbgTraceTo(IfrLog,
                           TL_ERROR, 
                           Device, 
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the bLength value (0x%x) of the endpoint descriptor at offset (0x%x) is less than expected (0x%x). Assuming bLength is (0x%x) for further validation.",
                           EndpointDesc->bLength,
                           offset,
                           sizeof(USB_ENDPOINT_DESCRIPTOR),
                           *AdjustedDescriptorLength);
            }

        }

        //
        // We are removing the check for endpoint descriptors being
        // exactly equal to the size of USB_ENDPOINT_DESCRIPTOR (7) 
        // because we know of at least one exception. Audio endpoint 
        // descriptors can be of length 9 instead of 7.
        // Since we are anyways validating the size against the 
        // remaining buffer, it is ok to just go by the length
        // defined in the descriptor
        //
        if (EndpointDesc->bLength > sizeof(USB_ENDPOINT_DESCRIPTOR)) {

            DbgTraceTo(IfrLog,
                       TL_WARNING,
                       Device,
                       "HW_COMPLIANCE:Configuration Descriptor Validation warning due to the bLength value (0x%x) of the endpoint descriptor at offset (0x%x) is greater than expected (0x%x). This is allowed by the audio class, but others should avoid it.",
                       EndpointDesc->bLength,
                       offset,
                       sizeof(USB_ENDPOINT_DESCRIPTOR));
            LOG_DESCRIPTOR_VALIDATION_WARNING(&ConfigValidationData->DevInfo, DescriptorValidationErrorEndpointBlengthTooLarge);

        }

        if (*AdjustedDescriptorLength > bufferLeft) {

            if (HUBDESC_ShouldEnforceWin7StrictValidation(&ConfigValidationData->DevInfo)) {

                *ValidationResult = ValidationFailed;

            }
            
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the endpoint descriptor at offset (0x%x) exceeding the descriptor buffer.  Aborting further validation.",
                       offset);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorEndpointBufferTooSmall);

            LEAVE;
        }

                   
        endpointAddress = EndpointDesc->bEndpointAddress;

        if (ConfigValidationData->ValidationFlags.FirstInterfaceFound == 0) {
                    
            //
            // Endpoint descriptor comes before interface descriptor!!
            //
            if (HUBDESC_ShouldEnforceWin7StrictValidation(&ConfigValidationData->DevInfo)) {

                *ValidationResult = ValidationFailed;

            }

            DbgTraceTo(IfrLog,
                       TL_ERROR, 
                       Device, 
                       "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to an endpoint descriptor (offset 0x%x) defined before the first interface descriptor.  Ignoring error for further validation.",
                       offset);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorEndpointWithNoPreceedingInterface);   

            LEAVE;

        }

        if ((EndpointDesc->bEndpointAddress & 0x0F)== 0) {
            if (HUBDESC_ShouldEnforceWin8ValidationImmutable(&ConfigValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the endpoint descriptor (offset 0x%x) having a bEndpointAddress specifying an address of 0 (any direction).  Ignoring error for further validation.",
                       offset);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorEndpointZeroAddress);

        }

        if (EndpointDesc->bEndpointAddress & 0x70) {
            if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the endpoint descriptor (offset 0x%x) having a bEndpointAddress (0x%x) with non-zero values in the reserved bits.  Ignoring error for further validation.",
                       offset, 
                       EndpointDesc->bEndpointAddress);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorEndpointNonZeroReservedBits);
        }

        // bits 6 and 7 of the bmAttributes are reserved
        if ((EndpointDesc->bmAttributes & 0xC0) != 0) {

            if (HUBDESC_ShouldEnforceMutableReservedFields(&ConfigValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }
            
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to reserved bits 6 and 7 of the bmAttributes (0x%x) field of an endpoint descriptor (offset 0x%x) are non-zero.  Ignoring error for further validation.",
                       EndpointDesc->bmAttributes,
                       offset);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorEndpointBmAttributesNonZeroReservedBits);
        }

        endpointType = EndpointDesc->bmAttributes & USB_ENDPOINT_TYPE_MASK;

        
        muxPacket.us = EndpointDesc->wMaxPacketSize;

        switch (endpointType) {
        case USB_ENDPOINT_TYPE_CONTROL:
            if ((EndpointDesc->bmAttributes & ~USB_ENDPOINT_TYPE_MASK) != 0) {
                if (HUBDESC_ShouldEnforceMutableReservedFields(&ConfigValidationData->DevInfo)) {
                        *ValidationResult = ValidationFailed;
                }
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to reserved bits 2..7 of the bmAttributes (0x%x) field of the endpoint descriptor (offset 0x%x) are non-zero for a control endpoint.  Ignoring error for further validation.",
                           EndpointDesc->bmAttributes,
                           offset);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorControlEndpointBmAttributesNonZeroReservedBits);
            }
            switch (ConfigValidationData->DevInfo.USBSpeed) {
            case UsbLowSpeed:
                if (EndpointDesc->wMaxPacketSize != 8) {
                    if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                        *ValidationResult = ValidationFailed;
                    }

                    //
                    // These specific cases were checked for low speed devices
                    // in Win7
                    // 
                    if ((EndpointDesc->wMaxPacketSize > 8) ||
                        (EndpointDesc->wMaxPacketSize == 0)) {
                        *ValidationResult = ValidationFailed;
                    }
                    
                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the wMaxPacketSize field (0x%x) of the endpoint descriptor (offset 0x%x) is not set to a valid value.  Ignoring error for further validation.",
                               EndpointDesc->wMaxPacketSize,
                               offset);
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorControlEndpointInvalidWMaxPacketSize);

                }

                break;
            case UsbFullSpeed:
                if ((EndpointDesc->wMaxPacketSize != 8) &&
                    (EndpointDesc->wMaxPacketSize != 16) &&
                    (EndpointDesc->wMaxPacketSize != 32) &&
                    (EndpointDesc->wMaxPacketSize != 64)) {
                
                    if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                        *ValidationResult = ValidationFailed;
                    }

                    //
                    // This specific case was checked for full speed devices
                    // in Win7
                    // 
                    if (EndpointDesc->wMaxPacketSize == 0) {
                        *ValidationResult = ValidationFailed;
                    }

                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the wMaxPacketSize field (0x%x) of the endpoint descriptor (offset 0x%x) is not set to a valid value.  Ignoring error for further validation.",
                               EndpointDesc->wMaxPacketSize,
                               offset);
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorControlEndpointInvalidWMaxPacketSize);
                }
                break;
            case UsbHighSpeed:
                if (EndpointDesc->wMaxPacketSize != 64) {
                    if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                        *ValidationResult = ValidationFailed;
                    }

                   
                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the wMaxPacketSize field (0x%x) of the endpoint descriptor (offset 0x%x) is not set to a valid value.  Ignoring error for further validation.",
                               EndpointDesc->wMaxPacketSize,
                               offset);
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorControlEndpointInvalidWMaxPacketSize);
                }
                break;
            case UsbSuperSpeed:
                if (EndpointDesc->wMaxPacketSize != 512) {
                    if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                        *ValidationResult = ValidationFailed;
                    }

                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the wMaxPacketSize field (0x%x) of the endpoint descriptor (offset 0x%x) is not set to a valid value.  Ignoring error for further validation.",
                               EndpointDesc->wMaxPacketSize,
                               offset);
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorControlEndpointInvalidWMaxPacketSize);
                }
                break;
            default:
                break;
            }
            break;
        case USB_ENDPOINT_TYPE_INTERRUPT:
            switch (ConfigValidationData->DevInfo.USBSpeed) {
            case UsbLowSpeed:
                if ((EndpointDesc->bmAttributes & ~USB_ENDPOINT_TYPE_MASK) != 0) {
                    
                    if (HUBDESC_ShouldEnforceMutableReservedFields(&ConfigValidationData->DevInfo)) {
                        *ValidationResult = ValidationFailed;
                    }

                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to reserved bits 2..7 of the bmAttributes (0x%x) field of the endpoint descriptor (offset 0x%x) are non-zero for a Interrupt endpoint.  Ignoring error.",
                               EndpointDesc->bmAttributes,
                               offset);
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorInterruptEndpointBmAttributesNonZeroReservedBits);

                }
                if (EndpointDesc->wMaxPacketSize > 8) {
                    *ValidationResult = ValidationFailed;
                    
                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the wMaxPacketSize field (0x%x) of the endpoint descriptor (offset 0x%x) is not set to a valid value.  Ignoring error for further validation.",
                               EndpointDesc->wMaxPacketSize,
                               offset);                     
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorInterruptEndpointInvalidWMaxPacketSize);
                }

                if (EndpointDesc->wMaxPacketSize == 0) {
                    *ValidationResult = ValidationFailed;
                    
                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the wMaxPacketSize field (0x%x) of the endpoint descriptor (offset 0x%x) is not set to a valid value.  Ignoring error for further validation.",
                               EndpointDesc->wMaxPacketSize,
                               offset);                     
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorInterruptEndpointInvalidWMaxPacketSize);
                }

                if (EndpointDesc->bInterval == 0) {
                    if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                        *ValidationResult = ValidationFailed;
                    }

                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the bInterval field (0x%x) of the endpoint descriptor (offset 0x%x) is not set to a valid value.  Ignoring error for further validation.",
                               EndpointDesc->bInterval,
                               offset);
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorInterruptEndpointInvalidBInterval);
                }
                break;
            case UsbFullSpeed:
                if ((EndpointDesc->bmAttributes & ~USB_ENDPOINT_TYPE_MASK) != 0) {
                    if (HUBDESC_ShouldEnforceMutableReservedFields(&ConfigValidationData->DevInfo)) {
                        *ValidationResult = ValidationFailed;
                    }

                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to reserved bits 2..7 of the bmAttributes (0x%x) field of the endpoint descriptor (offset 0x%x) are non-zero for a Interrupt endpoint.  Ignoring error.",
                               EndpointDesc->bmAttributes,
                               offset);
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorInterruptEndpointBmAttributesNonZeroReservedBits);
                }

                if (EndpointDesc->wMaxPacketSize > 64) {
                    *ValidationResult = ValidationFailed;
                    
                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the wMaxPacketSize field (0x%x) of the endpoint descriptor (offset 0x%x) is not set to a valid value.  Ignoring error for further validation.",
                               EndpointDesc->wMaxPacketSize,
                               offset);  
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorInterruptEndpointInvalidWMaxPacketSize);
                }

                if (EndpointDesc->bInterval == 0) {
                    if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                        *ValidationResult = ValidationFailed;
                    }

                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the bInterval field (0x%x) of the endpoint descriptor (offset 0x%x) is not set to a valid value.  Ignoring error for further validation.",
                               EndpointDesc->bInterval,
                               offset);
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorInterruptEndpointInvalidBInterval);
                }
                break;
            case UsbHighSpeed:
                if ((EndpointDesc->bmAttributes & ~USB_ENDPOINT_TYPE_MASK) != 0) {
                    if (HUBDESC_ShouldEnforceMutableReservedFields(&ConfigValidationData->DevInfo)) {
                        *ValidationResult = ValidationFailed;
                    }

                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to reserved bits 2..7 of the bmAttributes (0x%x) field of the endpoint descriptor (offset 0x%x) are non-zero for a Interrupt endpoint.  Ignoring error.",
                               EndpointDesc->bmAttributes,
                               offset);
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorInterruptEndpointBmAttributesNonZeroReservedBits);
                }

                if (muxPacket.MaxPacket > 1024) {
                    if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                        *ValidationResult = ValidationFailed;
                    }

                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the wMaxPacketSize field (0x%x) of the endpoint descriptor (offset 0x%x) is not set to a valid value.  Ignoring error for further validation.",
                               EndpointDesc->wMaxPacketSize,
                               offset);                         
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorInterruptEndpointInvalidWMaxPacketSize);
                }

                if (muxPacket.Reserved != 0) {
                    if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                        *ValidationResult = ValidationFailed;
                    }

                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the wMaxPacketSize field (0x%x) of the endpoint descriptor (offset 0x%x) has non-zero values for reserved bits.  Ignoring error for further validation.",
                               EndpointDesc->wMaxPacketSize,
                               offset);
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorInterruptEndpointInvalidWMaxPacketSize);
                }

                if (muxPacket.HSmux == 0x03) {
                    if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                        *ValidationResult = ValidationFailed;
                    }

                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the wMaxPacketSize field (0x%x) of the endpoint descriptor (offset 0x%x) has an invalid multiplier value.  Ignoring error for further validation.",
                               EndpointDesc->wMaxPacketSize,
                               offset);
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorInterruptEndpointInvalidWMaxPacketSize);

                }
                break;
            case UsbSuperSpeed:
                if (EndpointDesc->wMaxPacketSize > 1024) {
                    if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                        *ValidationResult = ValidationFailed;
                    }

                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the wMaxPacketSize field (0x%x) of the endpoint descriptor (offset 0x%x) is not set to a valid value.  Ignoring error for further validation.",
                               EndpointDesc->wMaxPacketSize,
                               offset);                         
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorInterruptEndpointInvalidWMaxPacketSize);
                }
                break;
            default:
                break;
            }
            break;
            
        case USB_ENDPOINT_TYPE_BULK:

            if ((EndpointDesc->bmAttributes & ~USB_ENDPOINT_TYPE_MASK) != 0) {
                if (HUBDESC_ShouldEnforceMutableReservedFields(&ConfigValidationData->DevInfo)) {
                    *ValidationResult = ValidationFailed;
                }

                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to reserved bits 2..7 of the bmAttributes (0x%x) field of the endpoint descriptor (offset 0x%x) are non-zero for a bulk endpoint.  Ignoring error.",
                           EndpointDesc->bmAttributes,
                           offset);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorBulkEndpointBmAttributesNonZeroReservedBits);
            }
            switch (ConfigValidationData->DevInfo.USBSpeed) {
            case UsbLowSpeed:

                *ValidationResult = ValidationFailed;
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the presence of a bulk endpoint descriptor (offset 0x%x) which is illegal for low speed devices.  Ignoring error for further validation.",
                           offset);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorBulkEndpointAtLowSpeed);

                break;
            case UsbFullSpeed:
                if ((EndpointDesc->wMaxPacketSize != 8) &&
                    (EndpointDesc->wMaxPacketSize != 16) &&
                    (EndpointDesc->wMaxPacketSize != 32) &&
                    (EndpointDesc->wMaxPacketSize != 64)) {

                    if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                        *ValidationResult = ValidationFailed;
                    }

                    //
                    // This specific case was checked for full speed devices
                    // in Win7
                    // 
                    if (EndpointDesc->wMaxPacketSize == 0) {
                        *ValidationResult = ValidationFailed;
                    }

                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the wMaxPacketSize field (0x%x) of the endpoint descriptor (offset 0x%x) is not set to a valid value.  Ignoring error for further validation.",
                               EndpointDesc->wMaxPacketSize,
                               offset);   
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorBulkEndpointInvalidWMaxPacketSize);
                }
                break;
            case UsbHighSpeed:
                if (EndpointDesc->wMaxPacketSize != 512) {
                    if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                        *ValidationResult = ValidationFailed;
                    }

                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the wMaxPacketSize field (0x%x) of the endpoint descriptor (offset 0x%x) is not set to a valid value.  Ignoring error for further validation.",
                               EndpointDesc->wMaxPacketSize,
                               offset);   
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorBulkEndpointInvalidWMaxPacketSize);
                }
                break;
            case UsbSuperSpeed:
                if (EndpointDesc->wMaxPacketSize != 1024) {
                    if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                        *ValidationResult = ValidationFailed;
                    }
                    
                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the wMaxPacketSize field (0x%x) of the endpoint descriptor (offset 0x%x) is not set to a valid value.  Ignoring error for further validation.",
                               EndpointDesc->wMaxPacketSize,
                               offset);   
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorBulkEndpointInvalidWMaxPacketSize);
                }
                break;
            default:
                break;
            }
            break;
            
        case USB_ENDPOINT_TYPE_ISOCHRONOUS:

            if ((ConfigValidationData->CurrentAltSetting == 0) &&
                (EndpointDesc->wMaxPacketSize > 0)) {

                DbgTraceTo(IfrLog,
                            TL_WARNING,
                            Device,
                            "HW_COMPLIANCE:Isochronous endpoint descriptor (offset 0x%x) in alternate interface 0 has non-zero wMaxPacketSize field (0x%x)",
                            offset,
                            EndpointDesc->wMaxPacketSize);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorAltSetting0IsochEndpointNonZeroMaxPacketSize);
            }

            switch (ConfigValidationData->DevInfo.USBSpeed) {
            case UsbLowSpeed:
                *ValidationResult = ValidationFailed;
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the presence of an Isochronous  endpoint descriptor (offset 0x%x) which is illegal for low speed devices.  Ignoring error for further validation.",
                           offset);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorIsochEndpointAtLowSpeed);

                break;
            case UsbFullSpeed:
                if (EndpointDesc->wMaxPacketSize > 1023) {
                    *ValidationResult = ValidationFailed;
                    
                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the wMaxPacketSize field (0x%x) of the endpoint descriptor (offset 0x%x) is not set to a valid value.  Ignoring error for further validation.",
                               EndpointDesc->wMaxPacketSize,
                               offset);   
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorIsochEndpointInvalidWMaxPacketSize);
                            
                }
                        
                if ((EndpointDesc->bInterval == 0) ||
                    (EndpointDesc->bInterval > 16)) {
                    if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                        *ValidationResult = ValidationFailed;
                    }

                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the bInterval field (0x%x) of the endpoint descriptor (offset 0x%x) is not set to a valid value.  Ignoring error for further validation.",
                               EndpointDesc->bInterval,
                               offset);
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorIsochEndpointInvalidBInterval);
                }
                break;
            case UsbHighSpeed:
                if (muxPacket.MaxPacket > 1024) {
                    if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                        *ValidationResult = ValidationFailed;
                    }

                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the wMaxPacketSize field (0x%x) of the endpoint descriptor (offset 0x%x) is not set to a valid value.  Ignoring error for further validation.",
                               EndpointDesc->wMaxPacketSize,
                               offset);                            
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorIsochEndpointInvalidWMaxPacketSize);
                }

                if (muxPacket.Reserved != 0) {
                    if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                        *ValidationResult = ValidationFailed;
                    }

                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the wMaxPacketSize field (0x%x) of the endpoint descriptor (offset 0x%x) has non-zero values for reserved bits.  Ignoring error for further validation.",
                               EndpointDesc->wMaxPacketSize,
                               offset);
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorIsochEndpointInvalidWMaxPacketSize);

                }

                if (muxPacket.HSmux == 0x03) {
                    if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                        *ValidationResult = ValidationFailed;
                    }

                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the wMaxPacketSize field (0x%x) of the endpoint descriptor (offset 0x%x) has an invalid multiplier value.  Ignoring error for further validation.",
                               EndpointDesc->wMaxPacketSize,
                               offset);
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorIsochEndpointInvalidWMaxPacketSize);
                }

                if ((EndpointDesc->bInterval == 0) ||
                    (EndpointDesc->bInterval > 16)) {
                    if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                        *ValidationResult = ValidationFailed;
                    }

                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the bInterval field (0x%x) of the endpoint descriptor (offset 0x%x) is not set to a valid value.  Ignoring error for further validation.",
                               EndpointDesc->bInterval,
                               offset);
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorIsochEndpointInvalidBInterval);
                }
                break;
            case UsbSuperSpeed:
                if (EndpointDesc->wMaxPacketSize > 1024) {
                    if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                        *ValidationResult = ValidationFailed;
                    }

                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the wMaxPacketSize field (0x%x) of the endpoint descriptor (offset 0x%x) is not set to a valid value.  Ignoring error for further validation.",
                               EndpointDesc->wMaxPacketSize,
                               offset);   
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorIsochEndpointInvalidWMaxPacketSize);
                }
                if ((EndpointDesc->bInterval == 0) ||
                    (EndpointDesc->bInterval > 16)) {
                    if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                        *ValidationResult = ValidationFailed;
                    }

                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the bInterval field (0x%x) of the endpoint descriptor (offset 0x%x) is not set to a valid value.  Ignoring error for further validation.",
                               EndpointDesc->bInterval,
                               offset);
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorIsochEndpointInvalidBInterval);
                }
                break;
            default:
                break;
            }
            break;
                       
        }



        if (RtlCheckBit(&ConfigValidationData->EndpointAddressesBitmap,
                        endpointAddress) != 0) {
            
            //
            // Endpoint Address already exists for this interface
            //
            if (HUBDESC_ShouldEnforceWin7StrictValidation(&ConfigValidationData->DevInfo)) {

                *ValidationResult = ValidationFailed;

            }

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the bEndpointAddress value (0x%x) of endpoint descriptor (offset 0x%x) was the same as one already defined for this interface (bInterfaceNumber 0x%x bAlternateSetting 0x%x)). Ignoring failure for further validation.",
                       endpointAddress,
                       offset,
                       ConfigValidationData->CurrentInterfaceNumber,
                       ConfigValidationData->CurrentAltSetting);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorInterfaceDuplicateEndpointAddress);
        }

        RtlSetBit(&ConfigValidationData->EndpointAddressesBitmap,
                  endpointAddress);
        ConfigValidationData->EndpointCount++;

        ConfigValidationData->ValidationFlags.EndpointCompanionFound = 0;
        ConfigValidationData->ValidationFlags.ExpectingEndpointCompanion = 1;
        ConfigValidationData->LastEndpointDescriptor = EndpointDesc;


    } FINALLY {

        if (*ValidationResult == ValidationSuccess) {
            result = TRUE;
        } else {
            result = FALSE;
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in endpoint descriptor.");
        }

    }

    return result;

}


__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
HUBDESC_InternalValidateSuperSpeedEndpointCompanionDescriptor(
    __in
        PUSB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR   EndpointCompDesc,
    __inout
        PINTERNAL_CONFIG_VALIDATION_DATA                ConfigValidationData,
    __out
        PULONG                                          AdjustedDescriptorLength,
    __inout_opt
        PVALIDATION_RESULT                              ValidationResult,
    __in
        RECORDER_LOG                                    IfrLog,
    __out_opt
        PBOOLEAN                                        SupportsStreams
    )
/*++

Routine Description:

    Performs validation of an endpoint descriptor.

--*/
{
    VALIDATION_RESULT           localResult;
    BOOLEAN                     result;    
    ULONG                       bufferLeft;
    ULONG                       offset;
    UCHAR                       endpointType;

    TRY {

        if (ValidationResult == NULL) {
            ValidationResult = &localResult;
            *ValidationResult = ValidationSuccess;
        } else if (ConfigValidationData->DevInfo.UseWin8DescriptorValidationBehavior) {
            *ValidationResult = ValidationSuccess;
        }

        if (SupportsStreams != NULL) {
            *SupportsStreams = FALSE;
        }

        offset = (ULONG)((PUCHAR)EndpointCompDesc - ConfigValidationData->DescriptorSetBeginning);        
        bufferLeft = (ULONG)(ConfigValidationData->BufferLength - offset);

        *AdjustedDescriptorLength = EndpointCompDesc->bLength;

        //
        //  Personal Healthcare Device Class devices use 0x30 for descriptor type in PHDC_11073PHD_FUNCTION_DESCRIPTOR
        //  This would conflict with Superspeed endpoint companion descriptor descriptor type
        //  Will not mark ValidationFailed if Non-SS have this descriptor for COMPAT reasons. Maintaining Compliance message so that the error gets flagged
        //

        if (ConfigValidationData->DevInfo.USBSpeed != UsbSuperSpeed) {
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the presence of a superspeed endpoint companion descriptor (offset 0x%x) for a non superspeed device.  Allowed only for USB 2.0 PHDC devices. Ignoring error for further validation.",
                       offset);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorCompanionEndpointAtNonSuperSpeed);
            // *ValidationResult = ValidationFailed;
            LEAVE;
        }
                
        if (EndpointCompDesc->bLength < sizeof(USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR)){

            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorCompanionEndpointBlengthTooSmall);
            //
            // if we can, lets assume that the descriptor is the correct size
            //
            if (sizeof(USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR) <= bufferLeft) {
                *AdjustedDescriptorLength = sizeof(USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR);
            }

            *ValidationResult = ValidationFailed;
            
            if (*AdjustedDescriptorLength < sizeof(USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR)) {

                *ValidationResult = ValidationFatalError;
                
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the bLength value (0x%x) of the endpoint companion descriptor at offset (0x%x) is less than expected (0x%x). Aborting further validation as buffer is insufficient to validate other fields of the endpoint descriptor.",
                           EndpointCompDesc->bLength,
                           offset,
                           sizeof(USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR));
                LEAVE;

            } else {

                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the bLength value (0x%x) of the endpoint companion descriptor at offset (0x%x) is less than expected (0x%x). Assuming bLength is (0x%x) for further validation.",
                           EndpointCompDesc->bLength,
                           offset,
                           sizeof(USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR),
                           *AdjustedDescriptorLength);

            }

        }

        if (EndpointCompDesc->bLength > sizeof(USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR)) {
        
            if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }                 

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to descriptor->bLength (0x%x) of descriptor at offset (0x%x) exceeding the expected descriptor length (0x%x).  Ignoring error for further validation.",
                       EndpointCompDesc->bLength,
                       offset,
                       sizeof(USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR));
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorCompanionEndpointBlengthTooLarge);

        }     

        if (ConfigValidationData->ValidationFlags.ExpectingEndpointCompanion == 0) {
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the presence of a superspeed endpoint companion descriptor in an unexpected location (offset 0x%x).  Ignoring error for further validation.",
                       offset);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorCompanionEndpointAtUnexpectedLocation);

            *ValidationResult = ValidationFailed;
            LEAVE;
        } else {

            ConfigValidationData->ValidationFlags.EndpointCompanionFound = 1;

        }
        
        ConfigValidationData->ValidationFlags.ExpectingEndpointCompanion = 0;


        if (*AdjustedDescriptorLength > bufferLeft) {
            if (HUBDESC_ShouldEnforceWin8ValidationImmutable(&ConfigValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the SuperSpeed endpoint companion descriptor at offset (0x%x) exceeding the descriptor buffer. Aborting further validation.",
                       offset);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorCompanionEndpointBufferTooSmall);
            LEAVE;
        }

        endpointType = ConfigValidationData->LastEndpointDescriptor->bmAttributes & USB_ENDPOINT_TYPE_MASK;

        switch (endpointType) {
        case USB_ENDPOINT_TYPE_CONTROL:
            if (EndpointCompDesc->bMaxBurst != 0) {
                if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                    *ValidationResult = ValidationFailed;
                }
                
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to a control endpoint's endpoint companion descriptor (offset 0x%x) specifying a non-zero bMaxBurst (0x%x).  Ignoring error for further validation.",
                           offset,
                           EndpointCompDesc->bMaxBurst);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorCompanionControlEndpointNonZeroBMaxBurst);
            }             

            if (EndpointCompDesc->bmAttributes.AsUchar != 0) {
                if (HUBDESC_ShouldEnforceMutableReservedFields(&ConfigValidationData->DevInfo)) {
                    *ValidationResult = ValidationFailed;
                }

                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to a control endpoint's endpoint companion descriptor (offset 0x%x) specifying a non-zero bmAttributes (0x%x).  Ignoring error for further validation.",
                           offset,
                           EndpointCompDesc->bmAttributes.AsUchar);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorCompanionControlEndpointNonZeroBmAttributes);
            }

            if (EndpointCompDesc->wBytesPerInterval != 0) {
                if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                    *ValidationResult = ValidationFailed;
                }

                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to a control endpoint's endpoint companion descriptor (offset 0x%x) specifying a non-zero wBytesPerInterval (0x%x).  Ignoring error for further validation.",
                           offset,
                           EndpointCompDesc->wBytesPerInterval);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorCompanionControlEndpointNonZeroWBytesPerInterval);
            }
            break;
        case USB_ENDPOINT_TYPE_BULK:
            if (EndpointCompDesc->bMaxBurst > 15) {
                if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                    *ValidationResult = ValidationFailed;
                }

                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to a bulk endpoint's endpoint companion descriptor (offset 0x%x) specifying a bMaxBurst (0x%x) greater than 15.  Ignoring error for further validation.",
                           offset,
                           EndpointCompDesc->bMaxBurst);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorCompanionBulkEndpointBMaxBurstTooLarge);
            }

            if ((EndpointCompDesc->bmAttributes.Bulk.MaxStreams > 0) &&
                (SupportsStreams != NULL)) {
                *SupportsStreams = TRUE;
            }
            if (EndpointCompDesc->bmAttributes.Bulk.MaxStreams > 16) {
                if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                    *ValidationResult = ValidationFailed;
                }

                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to a bulk endpoint's endpoint companion descriptor (offset 0x%x) specifying a MaxStreams (%d) greater than 16.  Ignoring error for further validation.",
                           offset,
                           EndpointCompDesc->bmAttributes.Bulk.MaxStreams);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorCompanionBulkEndpointMaxStreamsTooLarge);
            }

            if (EndpointCompDesc->bmAttributes.Bulk.Reserved1 != 0) {
                if (HUBDESC_ShouldEnforceMutableReservedFields(&ConfigValidationData->DevInfo)) {
                    *ValidationResult = ValidationFailed;
                }

                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to a bulk endpoint's endpoint companion descriptor (offset 0x%x) specifying non-zero values for the reserved fields of the bmAttributes (0x%x).  Ignoring error for further validation.",
                           offset,
                           EndpointCompDesc->bmAttributes.AsUchar);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorCompanionBulkEndpointBmAttributesNonZeroReservedBits);
            }

            if (EndpointCompDesc->wBytesPerInterval != 0) {
                if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                    *ValidationResult = ValidationFailed;
                }

                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to a bulk endpoint's endpoint companion descriptor (offset 0x%x) specifying a non-zero wBytesPerInterval (0x%x).  Ignoring error for further validation.",
                           offset,
                           EndpointCompDesc->wBytesPerInterval);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorCompanionBulkEndpointNonZeroWBytesPerInterval);
            }
            break;

        case USB_ENDPOINT_TYPE_INTERRUPT:
            if (EndpointCompDesc->bMaxBurst > 15) {
                if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                    *ValidationResult = ValidationFailed;
                }


                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to a interrupt endpoint's endpoint companion descriptor (offset 0x%x) specifying a bMaxBurst (0x%x) greater than 15.  Ignoring error for further validation.",
                           offset,
                           EndpointCompDesc->bMaxBurst);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorCompanionInterruptEndpointBMaxBurstTooLarge);
            }

            if ((EndpointCompDesc->bMaxBurst != 0) &&
                (ConfigValidationData->LastEndpointDescriptor->wMaxPacketSize != 1024)) {
                if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                    *ValidationResult = ValidationFailed;
                }


                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to an interrupt endpoint's endpoint companion descriptor (offset 0x%x) specifying a non-zero bMaxBurst (0x%x) while the endpoint's wMaxPacketSize (0x%x) is not 1024.  Ignoring error for further validation.",
                           offset,
                           EndpointCompDesc->bMaxBurst,
                           ConfigValidationData->LastEndpointDescriptor->wMaxPacketSize);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorCompanionInterruptEndpointIncompatibleBMaxBurstAndWMaxPacketSize);
            }

            if (EndpointCompDesc->bmAttributes.AsUchar != 0) {
                if (HUBDESC_ShouldEnforceMutableReservedFields(&ConfigValidationData->DevInfo)) {
                    *ValidationResult = ValidationFailed;
                }

                 DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to an interrupt endpoint's endpoint companion descriptor (offset 0x%x) specifying a non-zero bmAttributes (0x%x).  Ignoring error for further validation.",
                         offset,
                         EndpointCompDesc->bmAttributes.AsUchar);
                 LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorCompanionInterruptEndpointNonZeroBmAttributes);
            }

            if (EndpointCompDesc->wBytesPerInterval > 
                (ConfigValidationData->LastEndpointDescriptor->wMaxPacketSize 
                 * (EndpointCompDesc->bMaxBurst+1) 
                 * (EndpointCompDesc->bmAttributes.Isochronous.Mult+1))) {
                //*ValidationResult = ValidationFailed;
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to an interrupt endpoint's endpoint companion descriptor (offset 0x%x) specifying wBytesPerInterval(%d) that is greater than wMaxPacketSize * (bMaxBurst+1) * (Mult+1) (%d).  Ignoring error for further validation.",
                           offset,
                           EndpointCompDesc->wBytesPerInterval,
                           ConfigValidationData->LastEndpointDescriptor->wMaxPacketSize *  (EndpointCompDesc->bMaxBurst+1) * (EndpointCompDesc->bmAttributes.Isochronous.Mult+1));
                LOG_DESCRIPTOR_VALIDATION_WARNING(&ConfigValidationData->DevInfo, DescriptorValidationErrorCompanionInterruptEndpointWBytesPerIntervalTooLarge);
            }

            break;
        case USB_ENDPOINT_TYPE_ISOCHRONOUS:
            ConfigValidationData->ValidationFlags.SSPEndpointCompanionFound = 0;
            ConfigValidationData->ValidationFlags.ExpectingSSPEndpointCompanion = EndpointCompDesc->bmAttributes.Isochronous.SspCompanion;

            if (EndpointCompDesc->bMaxBurst > 15) {
                if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                    *ValidationResult = ValidationFailed;
                }

                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to an isochronous endpoint's endpoint companion descriptor (offset 0x%x) specifying a bMaxBurst (0x%x) greater than 15.  Ignoring error for further validation.",
                           offset,
                           EndpointCompDesc->bMaxBurst);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorCompanionIsochEndpointBMaxBurstTooLarge);
            }

            if ((EndpointCompDesc->bMaxBurst != 0) &&
                (ConfigValidationData->LastEndpointDescriptor->wMaxPacketSize != 1024)) {

                if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                    *ValidationResult = ValidationFailed;
                }

                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to an isochronous endpoint's endpoint companion descriptor (offset 0x%x) specifying a non-zero bMaxBurst (0x%x) while the endpoint's wMaxPacketSize (0x%x) is not 1024.  Ignoring error for further validation.",
                           offset,
                           EndpointCompDesc->bMaxBurst,
                           ConfigValidationData->LastEndpointDescriptor->wMaxPacketSize);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorCompanionIsochEndpointIncompatibleBMaxBurstAndWMaxPacketSize);
            }

            if ((ConfigValidationData->ValidationFlags.ExpectingSSPEndpointCompanion == 0) &&
                (EndpointCompDesc->bmAttributes.Isochronous.Mult > 2)) {
                if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                    *ValidationResult = ValidationFailed;
                }

                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to an isochronous endpoint's endpoint companion descriptor (offset 0x%x) specifying a Mult (%d) greater than 2.  Ignoring error for further validation.",
                           offset,
                           EndpointCompDesc->bmAttributes.Isochronous.Mult);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorCompanionIsochEndpointMultTooLarge);
            }

            if (EndpointCompDesc->bmAttributes.Isochronous.Reserved2 != 0) {
                if (HUBDESC_ShouldEnforceMutableReservedFields(&ConfigValidationData->DevInfo)) {
                    *ValidationResult = ValidationFailed;
                }

                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to an isochronous endpoint's endpoint companion descriptor (offset 0x%x) specifying non-zero values for the reserved fields of the bmAttributes (0x%x).  Ignoring error for further validation.",
                           offset,
                           EndpointCompDesc->bmAttributes.AsUchar);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorCompanionIoschEndpointBmAttributesNonZeroReservedBits);
            }


            if (ConfigValidationData->ValidationFlags.ExpectingSSPEndpointCompanion == 0) {
                if (EndpointCompDesc->wBytesPerInterval > ConfigValidationData->LastEndpointDescriptor->wMaxPacketSize * (EndpointCompDesc->bMaxBurst+1) * (EndpointCompDesc->bmAttributes.Isochronous.Mult+1)) {
                    //*ValidationResult = ValidationFailed;
                    DbgTraceTo(IfrLog,
                        TL_ERROR,
                        Device,
                        "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to an isochronous endpoint's endpoint companion descriptor (offset 0x%x) specifying wBytesPerInterval(%d) that is greater than wMaxPacketSize * (bMaxBurst+1) * (Mult+1) (%d).  Ignoring error for further validation.",
                        offset,
                        EndpointCompDesc->wBytesPerInterval,
                        ConfigValidationData->LastEndpointDescriptor->wMaxPacketSize *  (EndpointCompDesc->bMaxBurst+1) * (EndpointCompDesc->bmAttributes.Isochronous.Mult+1));
                    LOG_DESCRIPTOR_VALIDATION_WARNING(&ConfigValidationData->DevInfo, DescriptorValidationErrorCompanionIsochEndpointWBytesPerIntervalTooLarge);

                }
            } else if (EndpointCompDesc->wBytesPerInterval != 1) {
                if (HUBDESC_ShouldEnforceMutableReservedFields(&ConfigValidationData->DevInfo)) {
                    *ValidationResult = ValidationFailed;
                }

                DbgTraceTo(IfrLog,
                    TL_ERROR,
                    Device,
                    "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to an isochronous endpoint's endpoint companion descriptor (offset 0x%x) specifying wBytesPerInterval(%d) that is not 1 while expecting a susperspeedplus companion descriptor.  Ignoring error for further validation.",
                    offset,
                    EndpointCompDesc->wBytesPerInterval);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorCompanionIsochEndpointWBytesPerIntervalNotOne);
            }
            break;
        default:
            break;
        }
        
    } FINALLY {

        if (*ValidationResult == ValidationSuccess) {
            result = TRUE;
        } else {
            result = FALSE;
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in SuperSpeed endpoint companion descriptor.");
        }

    }

    return result;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBDESC_InternalValidateSuperSpeedPlusIsochEndpointCompanionDescriptor(
    __in
        PUSB_SUPERSPEEDPLUS_ISOCH_ENDPOINT_COMPANION_DESCRIPTOR   EndpointCompDesc,
    __inout
        PINTERNAL_CONFIG_VALIDATION_DATA                          ConfigValidationData,
    __out
        PULONG                                                    AdjustedDescriptorLength,
    __inout_opt
        PVALIDATION_RESULT                                        ValidationResult,
    __in
        RECORDER_LOG                                              IfrLog
    )
/*++

Routine Description:

    Performs validation of the SuperSpeedPlus Isoch companion endpoint descriptor.

--*/
{
    VALIDATION_RESULT           localResult;
    ULONG                       bufferLeft;
    ULONG                       offset;

    TRY {

        if (ValidationResult == NULL) {
            ValidationResult = &localResult;
            *ValidationResult = ValidationSuccess;
        }
        else if (ConfigValidationData->DevInfo.UseWin8DescriptorValidationBehavior) {
            *ValidationResult = ValidationSuccess;
        }

        offset = (ULONG)((PUCHAR)EndpointCompDesc - ConfigValidationData->DescriptorSetBeginning);
        bufferLeft = (ULONG)(ConfigValidationData->BufferLength - offset);

        *AdjustedDescriptorLength = EndpointCompDesc->bLength;

        if ((ConfigValidationData->DevInfo.USBSpeed != UsbSuperSpeed) ||
            (ConfigValidationData->DevInfo.SuperSpeedPlusIsochBurstCount == 0)) {

            DbgTraceTo(IfrLog,
                TL_ERROR,
                Device,
                "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the presence of a superspeedplus endpoint companion descriptor (offset 0x%x) for a non superspeedplus device.  Ignoring error for further validation.",
                offset);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorSSPCompanionEndpointAtNonSuperSpeed);
            *ValidationResult = ValidationFailed;
            LEAVE;
        }

        if (EndpointCompDesc->bLength < sizeof(USB_SUPERSPEEDPLUS_ISOCH_ENDPOINT_COMPANION_DESCRIPTOR)){

            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorSSPCompanionEndpointBlengthTooSmall);
            //
            // if we can, lets assume that the descriptor is the correct size
            //
            if (sizeof(USB_SUPERSPEEDPLUS_ISOCH_ENDPOINT_COMPANION_DESCRIPTOR) <= bufferLeft) {
                *AdjustedDescriptorLength = sizeof(USB_SUPERSPEEDPLUS_ISOCH_ENDPOINT_COMPANION_DESCRIPTOR);
            }

            *ValidationResult = ValidationFailed;

            if (*AdjustedDescriptorLength < sizeof(USB_SUPERSPEEDPLUS_ISOCH_ENDPOINT_COMPANION_DESCRIPTOR)) {

                *ValidationResult = ValidationFatalError;

                DbgTraceTo(IfrLog,
                    TL_ERROR,
                    Device,
                    "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the bLength value (0x%x) of the superspeedplus endpoint companion descriptor at offset (0x%x) is less than expected (0x%x). Aborting further validation as buffer is insufficient to validate other fields of the endpoint descriptor.",
                    EndpointCompDesc->bLength,
                    offset,
                    sizeof(USB_SUPERSPEEDPLUS_ISOCH_ENDPOINT_COMPANION_DESCRIPTOR));
                LEAVE;

            }
            else {

                DbgTraceTo(IfrLog,
                    TL_ERROR,
                    Device,
                    "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the bLength value (0x%x) of the superspeedplus endpoint companion descriptor at offset (0x%x) is less than expected (0x%x). Assuming bLength is (0x%x) for further validation.",
                    EndpointCompDesc->bLength,
                    offset,
                    sizeof(USB_SUPERSPEEDPLUS_ISOCH_ENDPOINT_COMPANION_DESCRIPTOR),
                    *AdjustedDescriptorLength);

            }

        }

        if (EndpointCompDesc->bLength > sizeof(USB_SUPERSPEEDPLUS_ISOCH_ENDPOINT_COMPANION_DESCRIPTOR)) {

            if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ConfigValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }

            DbgTraceTo(IfrLog,
                TL_ERROR,
                Device,
                "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to descriptor->bLength (0x%x) of descriptor at offset (0x%x) exceeding the expected descriptor length (0x%x).  Ignoring error for further validation.",
                EndpointCompDesc->bLength,
                offset,
                sizeof(USB_SUPERSPEEDPLUS_ISOCH_ENDPOINT_COMPANION_DESCRIPTOR));
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorSSPCompanionEndpointBlengthTooLarge);

        }

        if (ConfigValidationData->ValidationFlags.ExpectingSSPEndpointCompanion == 0) {
            DbgTraceTo(IfrLog,
                TL_ERROR,
                Device,
                "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the presence of a superspeedplus endpoint companion descriptor in an unexpected location (offset 0x%x).  Ignoring error for further validation.",
                offset);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorSSPCompanionEndpointAtUnexpectedLocation);

            *ValidationResult = ValidationFailed;
            LEAVE;
        }

        ConfigValidationData->ValidationFlags.SSPEndpointCompanionFound = 1;
        ConfigValidationData->ValidationFlags.ExpectingSSPEndpointCompanion = 0;


        if (*AdjustedDescriptorLength > bufferLeft) {
            if (HUBDESC_ShouldEnforceWin8ValidationImmutable(&ConfigValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }

            DbgTraceTo(IfrLog,
                TL_ERROR,
                Device,
                "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the superspeedplus endpoint companion descriptor at offset (0x%x) exceeding the descriptor buffer. Aborting further validation.",
                offset);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorSSPCompanionEndpointBufferTooSmall);
            LEAVE;
        }

        if (EndpointCompDesc->wReserved != 0) {
            if (HUBDESC_ShouldEnforceMutableReservedFields(&ConfigValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }

            DbgTraceTo(IfrLog,
                TL_ERROR,
                Device,
                "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to an isochronous endpoint's superspeedplus companion descriptor (offset 0x%x) specifying non-zero value for the reserved field wReserved (0x%x).  Ignoring error for further validation.",
                offset,
                EndpointCompDesc->wReserved);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorSSPCompanionIoschEndpointNonZeroReservedBits);
        }

        if ((EndpointCompDesc->dwBytesPerInterval < USB_SUPERSPEEDPLUS_ISOCHRONOUS_MIN_BYTESPERINTERVAL) ||
            (EndpointCompDesc->dwBytesPerInterval > USB_SUPERSPEEDPLUS_ISOCHRONOUS_MAX_BYTESPERINTERVAL)) {

            if (HUBDESC_ShouldEnforceMutableReservedFields(&ConfigValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }

            DbgTraceTo(IfrLog,
                TL_ERROR,
                Device,
                "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to an isochronous endpoint's superspeedplus companion descriptor (offset 0x%x) specifying dwBytesPerInterval(%d) that is lower than %d or greater than %d.  Ignoring error for further validation.",
                offset,
                EndpointCompDesc->dwBytesPerInterval,
                USB_SUPERSPEEDPLUS_ISOCHRONOUS_MIN_BYTESPERINTERVAL,
                USB_SUPERSPEEDPLUS_ISOCHRONOUS_MAX_BYTESPERINTERVAL);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorSSPCompanionIsochEndpointDWBytesPerIntervalTooSmallLarge);
        }

    } FINALLY {

        if (*ValidationResult != ValidationSuccess) {
            DbgTraceTo(IfrLog,
                TL_ERROR,
                Device,
                "HW_COMPLIANCE: Descriptor validation found errors in superspeedplus endpoint companion descriptor.");
        }

    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
HUBDESC_InternalValidateInterfaceDescriptor(
    __in
        PUSB_INTERFACE_DESCRIPTOR           InterfaceDesc,
    __inout
        PINTERNAL_CONFIG_VALIDATION_DATA    ConfigValidationData,
    __out
        PULONG                              AdjustedDescriptorLength,
    __inout_opt
        PVALIDATION_RESULT                  ValidationResult,
    __in
        RECORDER_LOG                        IfrLog
    )
/*++

Routine Description:

    Performs validation of an interface descriptor.

--*/
{
    VALIDATION_RESULT   localResult;
    BOOLEAN             result;    
    ULONG               bytesLeft;
    ULONG               offset;
    UCHAR               interfaceNumber;
    UCHAR               altSetting;
    BOOLEAN             newInterface;   
    BOOLEAN             firstInterface;

    TRY {
        
        if (ValidationResult == NULL) {
            ValidationResult = &localResult;
            *ValidationResult = ValidationSuccess;
        } else if (ConfigValidationData->DevInfo.UseWin8DescriptorValidationBehavior) {
            *ValidationResult = ValidationSuccess;
        }

        *AdjustedDescriptorLength = InterfaceDesc->bLength;
        offset = (ULONG)((PUCHAR)InterfaceDesc - ConfigValidationData->DescriptorSetBeginning);
        bytesLeft = ConfigValidationData->BufferLength - offset;
              
        //
        // Validate some basic fields of the descriptor
        //
        
        if (InterfaceDesc->bLength < sizeof(USB_INTERFACE_DESCRIPTOR)){

            
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorInterfaceBlengthTooSmall);
            //
            // if we can, lets assume that the descriptor is the correct size
            //
            if (sizeof(USB_INTERFACE_DESCRIPTOR) <= bytesLeft) {
                *AdjustedDescriptorLength = sizeof(USB_INTERFACE_DESCRIPTOR);
            }

            
            if (HUBDESC_ShouldEnforceWin7StrictValidation(&ConfigValidationData->DevInfo)) {

                *ValidationResult = ValidationFailed;

            }

            if (*AdjustedDescriptorLength < sizeof(USB_INTERFACE_DESCRIPTOR)) {

                *ValidationResult = ValidationFatalError;
                
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the bLength value (0x%x) of the interface descriptor at offset (0x%x) is less than expected (0x%x). Aborting further validation as buffer is insufficient to validate other fields of the interface descriptor.",
                           InterfaceDesc->bLength,
                           offset,
                           sizeof(USB_INTERFACE_DESCRIPTOR));
                LEAVE;

            } else {

                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the bLength value (0x%x) of the interface descriptor at offset (0x%x) is less than expected (0x%x). Assuming bLength is (0x%x) for further validation.",
                           InterfaceDesc->bLength,
                           offset,
                           sizeof(USB_INTERFACE_DESCRIPTOR),
                           *AdjustedDescriptorLength);

            }


        }

        if (InterfaceDesc->bLength > sizeof(USB_INTERFACE_DESCRIPTOR)) {

            if (HUBDESC_ShouldEnforceWin7StrictValidation(&ConfigValidationData->DevInfo)) {

                *ValidationResult = ValidationFailed;

            }

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to descriptor->bLength (0x%x) of descriptor at offset (0x%x) exceeding the expected descriptor length (0x%x).  Assuming bLength is correct for further validation.",
                       InterfaceDesc->bLength,
                       offset,
                       sizeof(USB_INTERFACE_DESCRIPTOR));
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorInterfaceBlengthTooLarge);

        }                  


        if (*AdjustedDescriptorLength > bytesLeft) {

            if (HUBDESC_ShouldEnforceWin7StrictValidation(&ConfigValidationData->DevInfo)) {

                *ValidationResult = ValidationFailed;

            }

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the interface descriptor at offset (0x%x) exceeding the descriptor buffer.  Aborting further validation.",
                       offset);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorInterfaceBufferTooSmall);
            LEAVE;
        }
        
        interfaceNumber = InterfaceDesc->bInterfaceNumber;
        altSetting = InterfaceDesc->bAlternateSetting;

        if ((ConfigValidationData->ValidationFlags.FirstInterfaceFound == 0) || 
            (interfaceNumber != ConfigValidationData->CurrentInterfaceNumber)) {
            newInterface = TRUE; 
        } else {
            newInterface = FALSE;
        }
                 
        //
        // Lets first validate that everything was good with the last interface and its
        // endpoint descriptors before we validate this one.
        //
        if (ConfigValidationData->ValidationFlags.FirstInterfaceFound == 0) {
            firstInterface = TRUE;
        } else {
            firstInterface = FALSE;
        }

        HUBDESC_InternalValidateLastInterface(ConfigValidationData,
                                              ValidationResult,
                                              IfrLog);

        if (*ValidationResult == ValidationFatalError) {
            LEAVE;
        }


        if (newInterface) {
            //
            // This marks the first interface descriptor with a new interface number
            //

            //
            // Check for duplicate interface number
            //
            if (RtlCheckBit(&ConfigValidationData->InterfaceNumberBitmap,
                            interfaceNumber) != 0) {
                            
                if (HUBDESC_ShouldEnforceWin7StrictValidation(&ConfigValidationData->DevInfo)) {

                    *ValidationResult = ValidationFailed;

                }

                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to interface descriptor (offset 0x%x) containing a bInterfaceNumber value (0x%x) which was already defined in another interface descriptor.  Ignoring failure for further validation.",
                           offset,
                           interfaceNumber);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorInterfaceDuplicateInterfaceNumber);
            }                          

            if ((firstInterface == FALSE) &&
                (ConfigValidationData->CurrentInterfaceNumber > interfaceNumber)) {

                //
                // Non-sequential interfaces
                //
                if (HUBDESC_ShouldEnforceWin7StrictValidation(&ConfigValidationData->DevInfo)) {

                    *ValidationResult = ValidationFailed;

                }

                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the interface descriptor (offset 0x%x) having a bInterfaceNumber value (0x%x) that is out of sequence (equal or lower value than previous interface number (0x%x).  Ignoring failure for further validation.",
                           offset,
                           interfaceNumber,
                           ConfigValidationData->CurrentInterfaceNumber);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorInterfaceOutOfSequenceInterfaceNumber);

            }

            if (altSetting != 0) {

                //
                // Invalid default alt setting
                //

                if (HUBDESC_ShouldEnforceWin7StrictValidation(&ConfigValidationData->DevInfo)) {

                    *ValidationResult = ValidationFailed;

                }

                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the interface descriptor (offset 0x%x) which is the first descriptor with the new bInterfaceNumber (0x%x) has a non-zero bAlternateSetting value (0x%x).  Ignoring failure for further validation.",
                           offset,
                           interfaceNumber,
                           altSetting);              
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorInterfaceFirstDescriptorWithNonZeroAlternateSetting);
                            
            }

            //
            // Re-initialize the alternate settings info for this new interface number
            //
            RtlClearAllBits(&ConfigValidationData->AlternateSettingsBitmap);
            RtlSetBit(&ConfigValidationData->InterfaceNumberBitmap,
                      interfaceNumber);
            ConfigValidationData->InterfacesFound++;

            ConfigValidationData->CurrentInterfaceNumber = interfaceNumber;

        } else {

            //
            // This must be an alternate setting interface descriptor
            //

            if (RtlCheckBit(&ConfigValidationData->AlternateSettingsBitmap,
                            altSetting) != 0) {

                //
                // Multiple interface descriptors with the same alt setting
                //
                if (HUBDESC_ShouldEnforceWin7StrictValidation(&ConfigValidationData->DevInfo)) {

                    *ValidationResult = ValidationFailed;

                }
            
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the bAlternateSetting value (0x%x) of interface descriptor (offset 0x%x) was the same as one already defined for this interface number (0x%x). Ignoring failure for further validation.",
                           altSetting,
                           offset,
                           interfaceNumber);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorInterfaceDuplicateAlternateSetting);
            }

            if (ConfigValidationData->CurrentAltSetting >= altSetting) {

                //
                // Non-sequential alt settings
                //
                if (HUBDESC_ShouldEnforceWin7StrictValidation(&ConfigValidationData->DevInfo)) {

                    *ValidationResult = ValidationFailed;
        
                }

                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the interface descriptor (offset 0x%x) having a bAlternateSetting value (0x%x) that is out of sequence (equal or lower value than previous alternate setting (0x%x).  Ignoring failure for further validation.",
                           offset,
                           altSetting,
                           ConfigValidationData->CurrentAltSetting);              
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorInterfaceOutOfSequenceAlternateSetting);

            }

        }

        //
        // Clear the endpoint address array for the next interface
        //
        RtlClearAllBits(&ConfigValidationData->EndpointAddressesBitmap);
        ConfigValidationData->EndpointCount = 0;
        ConfigValidationData->NumberEndpoints = InterfaceDesc->bNumEndpoints;
        ConfigValidationData->ValidationFlags.FirstEndpointFound = 0;

        //
        // Save info about this alternate setting
        //
        RtlSetBit(&ConfigValidationData->AlternateSettingsBitmap,
                  altSetting);
        ConfigValidationData->CurrentAltSetting = altSetting;

    } FINALLY {

        if (*ValidationResult == ValidationSuccess) {
            result = TRUE;
        } else {
            result = FALSE;
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in interface descriptor.");
        }

    }

    return result;

}


__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
HUBDESC_InternalValidateIADescriptor(
    __in
        PUSB_INTERFACE_ASSOCIATION_DESCRIPTOR   IADesc,
    __inout
        PINTERNAL_CONFIG_VALIDATION_DATA        ConfigValidationData,
    __out
        PULONG                                  AdjustedDescriptorLength,
    __inout_opt
        PVALIDATION_RESULT                      ValidationResult,
    __in
        RECORDER_LOG                            IfrLog
    )
/*++

Routine Description:

    Performs validation of an endpoint descriptor.

--*/
{
    VALIDATION_RESULT   localResult;
    BOOLEAN             result;    
    PUSB_CONFIGURATION_DESCRIPTOR   configDesc;
    ULONG                           bufferLeft;
    ULONG                           offset;

    TRY {
        
        if (ValidationResult == NULL) {
            ValidationResult = &localResult;
            *ValidationResult = ValidationSuccess;
        } else if (ConfigValidationData->DevInfo.UseWin8DescriptorValidationBehavior) {
            *ValidationResult = ValidationSuccess;
        }

        offset = (ULONG)((PUCHAR)IADesc - ConfigValidationData->DescriptorSetBeginning);
        bufferLeft = ConfigValidationData->BufferLength - offset;
        configDesc = (PUSB_CONFIGURATION_DESCRIPTOR)ConfigValidationData->DescriptorSetBeginning;
        *AdjustedDescriptorLength = IADesc->bLength;     

        if (IADesc->bLength < sizeof(USB_INTERFACE_ASSOCIATION_DESCRIPTOR)) {
            
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorIadBlengthTooSmall);
            
            if (HUBDESC_ShouldEnforceWin7StrictValidation(&ConfigValidationData->DevInfo)) {

                *ValidationResult = ValidationFailed;

            }

            if (sizeof(USB_INTERFACE_ASSOCIATION_DESCRIPTOR) <= bufferLeft) {

                *AdjustedDescriptorLength = sizeof(USB_INTERFACE_ASSOCIATION_DESCRIPTOR);
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to Interface Association Descriptor at offset 0x%x containing a bLength value (0x%x) less than expected (0x%x).  Assuming bLength is 0x%x for further validation.",
                           offset,
                           IADesc->bLength,
                           sizeof(USB_INTERFACE_ASSOCIATION_DESCRIPTOR),
                           *AdjustedDescriptorLength);
            } else {
   
                *ValidationResult = ValidationFatalError;
                
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to Interface Association Descriptor at offset 0x%x containing a bLength value (0x%x) less than expected (0x%x).  Aborting further validation.",
                           offset,
                           IADesc->bLength,
                           sizeof(USB_INTERFACE_ASSOCIATION_DESCRIPTOR));
                LEAVE;

            }

        }

        if (IADesc->bLength > sizeof(USB_INTERFACE_ASSOCIATION_DESCRIPTOR)) {

            if (HUBDESC_ShouldEnforceWin7StrictValidation(&ConfigValidationData->DevInfo)) {

                *ValidationResult = ValidationFailed;

            }

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to Interface Association Descriptor at offset 0x%x containing a bLength value (0x%x) greater than expected (0x%x).  Ignoring error for further validation.",
                       offset,
                       IADesc->bLength,
                       sizeof(USB_INTERFACE_ASSOCIATION_DESCRIPTOR));
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorIadBlengthTooLarge);

        }


        if (*AdjustedDescriptorLength > bufferLeft) {

            if (HUBDESC_ShouldEnforceWin7StrictValidation(&ConfigValidationData->DevInfo)) {

                *ValidationResult = ValidationFailed;

            }

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to the interface association descriptor at offset (0x%x) exceeding the descriptor buffer.  Aborting further validation.",
                       offset);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorIadBufferTooSmall);
            LEAVE;
        }

        if ((256 - IADesc->bFirstInterface) <  IADesc->bInterfaceCount) {
            
            if (HUBDESC_ShouldEnforceWin7StrictValidation(&ConfigValidationData->DevInfo)) {

                *ValidationResult = ValidationFailed;

            }

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to Interface Association Descriptor at offset 0x%x specifying higher interface numbers (0x%x + 0x%x) than can fit in 8 bits.  Ignoring error for further validation.",
                       offset,
                       IADesc->bFirstInterface,
                       IADesc->bInterfaceCount);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorIadInterfaceNumberTooLarge);

        }

        if (IADesc->bInterfaceCount > configDesc->bNumInterfaces) {

            if (HUBDESC_ShouldEnforceWin7StrictValidation(&ConfigValidationData->DevInfo)) {

                *ValidationResult = ValidationFailed;

            }
            
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to Interface Association Descriptor at offset 0x%x specifying a bInterfaceCount value (0x%x) greater than the config descriptors bNumInterfaces value (0x%x).  Ignoring error for further validation.",
                       offset,
                       IADesc->bInterfaceCount,
                       configDesc->bNumInterfaces);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorIadInterfaceCountTooLargeForTheConfig);
        }

        //
        // This validation is done in USBCCGP.  Adding it here for consistency.  There may be a compatibility
        // issue if a device used a custom USBCCGP driver and the device failed this validation.
        //
        if ((IADesc->bInterfaceCount + ConfigValidationData->InterfacesFound) > configDesc->bNumInterfaces) {
            
            if (HUBDESC_ShouldEnforceWin8ValidationImmutable(&ConfigValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }
            
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to Interface Association Descriptor at offset 0x%x specifying a bInterfaceCount value (0x%x) that when added to the number of interfaces found so far (0x%x) is greater than the config descriptors bNumInterfaces value (0x%x).  Ignoring error for further validation.",
                       offset,
                       IADesc->bInterfaceCount,
                       ConfigValidationData->InterfacesFound,
                       configDesc->bNumInterfaces);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorIadInterfaceCountTooLargeForTheConfig);

        }

        if (IADesc->bInterfaceCount == 0) {

            if (HUBDESC_ShouldEnforceWin7StrictValidation(&ConfigValidationData->DevInfo)) {

                *ValidationResult = ValidationFailed;

            }
            
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to Interface Association Descriptor at offset 0x%x specifying a bInterfaceCount value of zero.  Ignoring error for further validation.",
                       offset);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ConfigValidationData->DevInfo, DescriptorValidationErrorIadInterfaceCountZero);

        }
         
    } FINALLY {

        if (*ValidationResult == ValidationSuccess) {
            result = TRUE;
        } else {
            result = FALSE;
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in interface association descriptor.");
        }

    }

    return result;

}


__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
HUBDESC_ValidateConfigurationDescriptorSet(
    __in_bcount(BufferLength)
        PUSB_CONFIGURATION_DESCRIPTOR   ConfigDesc,
    __in
        ULONG                           BufferLength,
    __in
        PVALIDATION_DEVICE_INFO         DevInfo,
    __in
        RECORDER_LOG                    IfrLog,
    __out_opt
        PBOOLEAN                        SupportsStreams
    )
/*++

Routine Description:

    Performs validation of a configuration descriptor set
    
   
--*/
{

    VALIDATION_RESULT               localResult;
    BOOLEAN                         result;    
    PUCHAR                          offset;
    INTERNAL_CONFIG_VALIDATION_DATA configData;    
    ULONG                           descriptorLength;
        
    TRY {
        
        result = TRUE;

        if (SupportsStreams != NULL) {
            *SupportsStreams = FALSE;
        }

        HUBDESC_InitializeConfigValidationData(ConfigDesc,
                                               BufferLength,
                                               DevInfo,
                                               &configData);

        //
        // Validate the input
        //   
        if (ConfigDesc == NULL) {

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to NULL descriptor. Aborting further validation.");
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorConfigNull);
            result = FALSE;
            
            LEAVE;
            
        }
        
        if (BufferLength < sizeof(USB_CONFIGURATION_DESCRIPTOR)) {

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to buffer length (0x%x) being less than sizeof(CONFIGURATION_DESCRIPTOR) (0x%x).  Aborting further validation.",
                       BufferLength,
                       sizeof(USB_CONFIGURATION_DESCRIPTOR));
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorConfigBufferTooSmall);
            result = FALSE;
            
            LEAVE;

        }

        localResult = ValidationSuccess;
        HUBDESC_InternalValidateConfigDescriptor(ConfigDesc,
                                                 &configData,
                                                 &descriptorLength,
                                                 &localResult,
                                                 IfrLog);

        if (localResult == ValidationFatalError) {
            result = FALSE;
            LEAVE;
        } else if (localResult == ValidationFailed) {
            result = FALSE;
        }
        
        offset = ((PUCHAR)ConfigDesc) + sizeof(USB_CONFIGURATION_DESCRIPTOR);
        
        while (offset < configData.DescriptorSetEnd) {

            PUSB_COMMON_DESCRIPTOR                  commonDesc;
                        
            //
            // Ensure the descriptor header is in bounds.  We always
            // need to ensure we have enough data to look at the descriptor
            // header fields.  Sometimes descriptors are zeroed at the end,
            // this has been allowed in the past.  Moving forward, this padding
            // should be eliminated to protect client drivers during their parsing.            
            //

            commonDesc = (PUSB_COMMON_DESCRIPTOR)offset;
            descriptorLength = 0;
            localResult = ValidationSuccess;
            HUBDESC_InternalValidateCommonDescriptorHeader(commonDesc,
                                                           (PINTERNAL_VALIDATION_DATA)&configData,
                                                           &descriptorLength,
                                                           &localResult,
                                                           IfrLog);
            if (localResult == ValidationFatalError) {
                result = FALSE;
                LEAVE;
            //
            // do not fail if UseWin8DescriptorValidationBehavior is set.  This is a point
            // fix for the Cypress mouse.
            // TODO: Refactor validation code in RS to clean up usage of 
            //       UseWin8DescriptorValidationBehavior
            //
            } else if ((localResult == ValidationFailed) &&
                       (configData.DevInfo.UseWin8DescriptorValidationBehavior == 0)) {
                result = FALSE;
            }

            //
            // The descriptor is in bounds, we can look at it.
            //
           
            localResult = ValidationSuccess;
            switch (commonDesc->bDescriptorType) {
            
            case USB_INTERFACE_DESCRIPTOR_TYPE:

                //
                // An endpoint companion descriptor is required to immediately follow its corresponding
                // endpoint descriptor.  Thus we should no longer see an endpoint companion descriptor
                // until the next endpoint descriptor
                //
                configData.ValidationFlags.ExpectingEndpointCompanion = 0;
                configData.ValidationFlags.ExpectingSSPEndpointCompanion = 0;
                HUBDESC_InternalValidateInterfaceDescriptor((PUSB_INTERFACE_DESCRIPTOR)commonDesc,
                                                            &configData,
                                                            &descriptorLength,
                                                            &localResult,
                                                            IfrLog);
                if (localResult == ValidationFatalError) {
                    result = FALSE;
                    LEAVE;
                } else if (localResult == ValidationFailed) {
                    result = FALSE;
                }
                 
                break;
                    
            case USB_ENDPOINT_DESCRIPTOR_TYPE:

                //
                // An SSP Isoch endpoint companion descriptor is required to immediately follow its corresponding
                // SS companion descriptor.  Thus we should no longer see an endpoint companion descriptor
                // until the next endpoint descriptor
                //
                configData.ValidationFlags.ExpectingSSPEndpointCompanion = 0;
                HUBDESC_InternalValidateEndpointDescriptor((PUSB_ENDPOINT_DESCRIPTOR)commonDesc,
                                                           &configData,
                                                           &descriptorLength,
                                                           &localResult,
                                                           IfrLog);
                if (localResult == ValidationFatalError) {
                    result = FALSE;
                    LEAVE;
                } else if (localResult == ValidationFailed) {
                    result = FALSE;
                }

                break;

            case USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR_TYPE:

                //
                // An SSP Isoch endpoint companion descriptor is required to immediately follow its corresponding
                // SS companion descriptor.  We set it in HUBDESC_InternalValidateSuperSpeedEndpointCompanionDescriptor
                // if corresponding bit is set.
                //
                configData.ValidationFlags.ExpectingSSPEndpointCompanion = 0;
                HUBDESC_InternalValidateSuperSpeedEndpointCompanionDescriptor((PUSB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR) commonDesc,
                                                                              &configData,
                                                                              &descriptorLength,
                                                                              &localResult,
                                                                              IfrLog,
                                                                              SupportsStreams);
                if (localResult == ValidationFatalError) {
                    result = FALSE;
                    LEAVE;
                } else if (localResult == ValidationFailed) {
                    result = FALSE;
                }

                break;

            case USB_SUPERSPEEDPLUS_ISOCH_ENDPOINT_COMPANION_DESCRIPTOR_TYPE:
                HUBDESC_InternalValidateSuperSpeedPlusIsochEndpointCompanionDescriptor((PUSB_SUPERSPEEDPLUS_ISOCH_ENDPOINT_COMPANION_DESCRIPTOR)commonDesc,
                                                                                       &configData,
                                                                                       &descriptorLength,
                                                                                       &localResult,
                                                                                       IfrLog);
                if (localResult == ValidationFatalError) {
                    result = FALSE;
                    LEAVE;
                }
                else if (localResult == ValidationFailed) {
                    result = FALSE;
                }

                break;

            case USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE:
    
                //
                // An endpoint companion descriptor is required to immediately follow its corresponding
                // endpoint descriptor.  Thus we should no longer see an endpoint companion descriptor
                // until the next endpoint descriptor
                //
                configData.ValidationFlags.ExpectingEndpointCompanion = 0;
                configData.ValidationFlags.ExpectingSSPEndpointCompanion = 0;
                HUBDESC_InternalValidateIADescriptor((PUSB_INTERFACE_ASSOCIATION_DESCRIPTOR)commonDesc,
                                                     &configData,
                                                     &descriptorLength,
                                                     &localResult,
                                                     IfrLog);
                if (localResult == ValidationFatalError) {
                    result = FALSE;
                    LEAVE;
                } else if (localResult == ValidationFailed) {
                    result = FALSE;
                }

                break;

            default:

                //
                // An endpoint companion descriptor is required to immediately follow its corresponding
                // endpoint descriptor.  Thus we should no longer see an endpoint companion descriptor
                // until the next endpoint descriptor
                //
                configData.ValidationFlags.ExpectingEndpointCompanion = 0;
                configData.ValidationFlags.ExpectingSSPEndpointCompanion = 0;
                break;

            } // switch(commonDesc->bDescriptorType)

            //
            // Occassionally devices will report wTotalLength too big and have zeroed
            // data at the end.  This should not be a problem, just ignore it.
            //

            if (commonDesc->bLength == 0) {
                break;
            }

            offset += descriptorLength;

        } // while (offset < end)

        localResult = ValidationSuccess;

        HUBDESC_InternalValidateLastInterface(&configData,
                                              &localResult,
                                              IfrLog);

        if (localResult == ValidationFatalError) {
            result = FALSE;
            LEAVE;
        } else if (localResult == ValidationFailed) {
            result = FALSE;
        }

        if (configData.InterfacesFound != ConfigDesc->bNumInterfaces) {
            
            if (HUBDESC_ShouldEnforceWin7StrictValidation(&configData.DevInfo)) {

                result = FALSE;

            }
        }

    } FINALLY {

        if (result == FALSE) {
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in configuration descriptor set.");
        }

    }

    return result;
}


BOOLEAN
HUBDESC_InternalValidateContainerIDCapabilityDescriptor (
    __in
        PUSB_DEVICE_CAPABILITY_CONTAINER_ID_DESCRIPTOR  ContainerIDDescriptor,
    __in
        PINTERNAL_BOS_VALIDATION_DATA                   ValidationData,
    __inout
        PULONG                                          AdjustedDescriptorLength,
    __inout_opt
        PVALIDATION_RESULT                              ValidationResult,
    __in
        RECORDER_LOG                                    IfrLog
    )
/*++

Routine Description:

    Performs validation of a Continer ID Capability Descriptor

Returns:


--*/
{
    VALIDATION_RESULT   localResult;
    BOOLEAN             result;    
    ULONG               bufferLeft;
    ULONG               offset;

    TRY {
        
        if (ValidationResult == NULL) {
            ValidationResult = &localResult;
            *ValidationResult = ValidationSuccess;
        } else if (ValidationData->DevInfo.UseWin8DescriptorValidationBehavior) {
            *ValidationResult = ValidationSuccess;
        }

        offset = (ULONG)((PUCHAR)ContainerIDDescriptor - ValidationData->DescriptorSetBeginning);
        bufferLeft = ValidationData->BufferLength - offset;
        
        if (ContainerIDDescriptor->bLength < sizeof(USB_DEVICE_CAPABILITY_CONTAINER_ID_DESCRIPTOR)) {
                        
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorContainerIdBlengthTooSmall);
            
            if (sizeof(USB_DEVICE_CAPABILITY_CONTAINER_ID_DESCRIPTOR) <= bufferLeft) {

                *ValidationResult = ValidationFailed;
                
                *AdjustedDescriptorLength = sizeof(USB_DEVICE_CAPABILITY_CONTAINER_ID_DESCRIPTOR);
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Descriptor Validation failed due to Container ID descriptor at offset 0x%x containing a bLength value (0x%x) less than expected (0x%x).  Assuming bLength is 0x%x for further validation.",
                           offset,
                           ContainerIDDescriptor->bLength,
                           sizeof(USB_DEVICE_CAPABILITY_CONTAINER_ID_DESCRIPTOR),
                           *AdjustedDescriptorLength);
 
            } else {
   
                *ValidationResult = ValidationFatalError;
                
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Descriptor Validation failed due to Container ID descriptor at offset 0x%x containing a bLength value (0x%x) less than expected (0x%x).  Aborting further validation.",
                           offset,
                           ContainerIDDescriptor->bLength,
                           sizeof(USB_DEVICE_CAPABILITY_CONTAINER_ID_DESCRIPTOR));
                LEAVE;

            }

        }

        if (ContainerIDDescriptor->bLength > sizeof(USB_DEVICE_CAPABILITY_CONTAINER_ID_DESCRIPTOR)) {

            if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Descriptor Validation Failed due to Container ID descriptor at offset 0x%x containing a bLength value (0x%x) greater than expected (0x%x).  Ignoring error for further validation.",
                       offset,
                       ContainerIDDescriptor->bLength,
                       sizeof(USB_DEVICE_CAPABILITY_CONTAINER_ID_DESCRIPTOR));
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorContainerIdBlengthTooLarge);

        }      


        if (*AdjustedDescriptorLength > bufferLeft) {
            if (HUBDESC_ShouldEnforceWin8ValidationImmutable(&ValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:BOS Descriptor Validation Failed due to the Container ID descriptor at offset (0x%x) exceeding the descriptor buffer.  Aborting further validation.",
                       offset);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorContainerIdBufferTooSmall);
            LEAVE;
        }

        if (ContainerIDDescriptor->bReserved != 0) {

            if (HUBDESC_ShouldEnforceMutableReservedFields(&ValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Container ID descriptor contains a non-zero value (0x%x) in the reserved field. Ignoring error.",
                       ContainerIDDescriptor->bReserved);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorContainerIdNonZeroReservedBits);
        }

        if (IsEqualGUID((LPGUID) &ContainerIDDescriptor->ContainerID, (LPGUID) &GUID_NULL)) {
            
            //
            // COMPAT: Win8 succeeds enumeration even if the BOS container ID is all zeros.  We must continue
            //         successfully enumerating such devices, however we will discard the container ID.
            //
            ValidationData->BOSInfo->DiscardContainerID = TRUE;

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:BOS Descriptor Validation Failed due to the Container ID descriptor containing all zeros.");
            LOG_DESCRIPTOR_VALIDATION_WARNING(&ValidationData->DevInfo, DescriptorValidationErrorContainerIdAllZeros);

            LEAVE;
        }

    } FINALLY {

        if (*ValidationResult == ValidationSuccess) {
            result = TRUE;
        } else {
            result = FALSE;
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in container ID capability descriptor.");
        }

    }

    return result;

}


__drv_requiresIRQL(PASSIVE_LEVEL)
BOOLEAN
HUBDESC_InternalValidatePowerDeliveryCapabilityDescriptor (
    __in
        PUSB_DEVICE_CAPABILITY_POWER_DELIVERY_DESCRIPTOR    PowerDeliveryDescriptor,
    __in
        PINTERNAL_BOS_VALIDATION_DATA                       ValidationData,
    __inout
        PULONG                                              AdjustedDescriptorLength,
    __inout_opt
        PVALIDATION_RESULT                                  ValidationResult,
    __in
        RECORDER_LOG                                        IfrLog
    )
/*++

Routine Description:

    Performs validation of a BOS Power Delivery Capability Descriptor.

Returns:

    BOOLEAN

--*/
{
    VALIDATION_RESULT   localResult;
    BOOLEAN             result;
    ULONG               bufferLeft;
    ULONG               offset;

    UNREFERENCED_PARAMETER(AdjustedDescriptorLength);

    TRY {

        if (ValidationResult == NULL) {
            ValidationResult = &localResult;
            *ValidationResult = ValidationSuccess;
        }
        else if (ValidationData->DevInfo.UseWin8DescriptorValidationBehavior) {
            *ValidationResult = ValidationSuccess;
        }

        offset = (ULONG)((PUCHAR)PowerDeliveryDescriptor - ValidationData->DescriptorSetBeginning);
        bufferLeft = ValidationData->BufferLength - offset;

        if (PowerDeliveryDescriptor->bLength < sizeof(USB_DEVICE_CAPABILITY_POWER_DELIVERY_DESCRIPTOR)) {

            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorPDBlengthTooSmall);

            if (sizeof(USB_DEVICE_CAPABILITY_POWER_DELIVERY_DESCRIPTOR) <= bufferLeft) {

                *ValidationResult = ValidationFailed;

                *AdjustedDescriptorLength = sizeof(USB_DEVICE_CAPABILITY_POWER_DELIVERY_DESCRIPTOR);
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Descriptor Validation failed due to Power Delivery descriptor at offset 0x%x containing a bLength value (0x%x) less than expected (0x%x).  Assuming bLength is 0x%x for further validation.",
                           offset,
                           PowerDeliveryDescriptor->bLength,
                           sizeof(USB_DEVICE_CAPABILITY_POWER_DELIVERY_DESCRIPTOR),
                           *AdjustedDescriptorLength);

            } else {

                *ValidationResult = ValidationFatalError;

                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Descriptor Validation failed due to Power Delivery descriptor at offset 0x%x containing a bLength value (0x%x) less than expected (0x%x).  Aborting further validation.",
                           offset,
                           PowerDeliveryDescriptor->bLength,
                           sizeof(USB_DEVICE_CAPABILITY_POWER_DELIVERY_DESCRIPTOR));
                LEAVE;

            }

        }

        if (PowerDeliveryDescriptor->bLength > sizeof(USB_DEVICE_CAPABILITY_POWER_DELIVERY_DESCRIPTOR)) {

            if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Descriptor Validation Failed due to Power Delivery descriptor at offset 0x%x containing a bLength value (0x%x) greater than expected (0x%x).  Ignoring error for further validation.",
                       offset,
                       PowerDeliveryDescriptor->bLength,
                       sizeof(USB_DEVICE_CAPABILITY_POWER_DELIVERY_DESCRIPTOR));
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorPDBlengthTooLarge);

        }


        if (*AdjustedDescriptorLength > bufferLeft) {
            if (HUBDESC_ShouldEnforceWin8ValidationImmutable(&ValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:BOS Descriptor Validation Failed due to the Power Delivery descriptor at offset (0x%x) exceeding the descriptor buffer.  Aborting further validation.",
                       offset);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorPDBufferTooSmall);
            LEAVE;
        }

        if ((PowerDeliveryDescriptor->bReserved != 0) ||
            (PowerDeliveryDescriptor->bmAttributes.Reserved1 != 0) ||
            (PowerDeliveryDescriptor->bmAttributes.Reserved2 != 0) ||
            (PowerDeliveryDescriptor->bmAttributes.Reserved3 != 0)) {

            if (HUBDESC_ShouldEnforceMutableReservedFields(&ValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Power Delivery descriptor contains a non-zero value in the reserved fields (bReserved=0x%x,Reserved1=0x%x,Reserved2=0x%x,Reserved3=0x%x). Ignoring error.",
                       PowerDeliveryDescriptor->bReserved, PowerDeliveryDescriptor->bmAttributes.Reserved1, PowerDeliveryDescriptor->bmAttributes.Reserved2, PowerDeliveryDescriptor->bmAttributes.Reserved3);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorPDNonZeroReservedBits);
        }

        if (PowerDeliveryDescriptor->bmAttributes.ChargingPolicy) {
            ValidationData->BOSInfo->ChargingPolicySupported = TRUE;
            DbgTraceTo(IfrLog,
                       TL_INFO,
                       Device,
                       "HW_COMPLIANCE: Charging policy is enabled in power delivery capability descriptor.");
        }

    } FINALLY {

        if (*ValidationResult == ValidationSuccess) {
            result = TRUE;
        } else {
            result = FALSE;
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in power delivery capability descriptor.");
        }

    }

    return result;
}


__drv_requiresIRQL(PASSIVE_LEVEL)
BOOLEAN
HUBDESC_InternalValidatePlatformCapabilityDescriptor (
    __in
        PUSB_DEVICE_CAPABILITY_PLATFORM_DESCRIPTOR      PlatformDescriptor,
    __in
        PINTERNAL_BOS_VALIDATION_DATA                   ValidationData,
    __inout
        PULONG                                          AdjustedDescriptorLength,
    __inout_opt
        PVALIDATION_RESULT                              ValidationResult,
    __in
        RECORDER_LOG                                    IfrLog
    )
/*++

Routine Description:

    Performs validation of a BOS Platform Capability Descriptor.

Returns:

    BOOLEAN

--*/
{
    VALIDATION_RESULT                   localResult;
    BOOLEAN                             result;    
    ULONG                               descriptorInfoCount;
    ULONG                               i;
    ULONG                               highestValidWinVer;
    PMSOS20_DESCRIPTOR_SET_INFO         descriptorSetInfo;
    PMSOS20_PLATFORM_CAP_DESC_HEADER    msOs20PlatformDescriptor;
    ULONG                               bufferLeft;
    ULONG                               offset;
    ULONG                               minimumDescriptorSize;

    TRY {
        
        if (ValidationResult == NULL) {
            ValidationResult = &localResult;
            *ValidationResult = ValidationSuccess;
        } else if (ValidationData->DevInfo.UseWin8DescriptorValidationBehavior) {
            *ValidationResult = ValidationSuccess;
        }

        offset = (ULONG)((PUCHAR)PlatformDescriptor - ValidationData->DescriptorSetBeginning);
        bufferLeft = ValidationData->BufferLength - offset;
        minimumDescriptorSize = FIELD_OFFSET(USB_DEVICE_CAPABILITY_PLATFORM_DESCRIPTOR, CapabililityData);

        if (PlatformDescriptor->bLength < minimumDescriptorSize) {

            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorPlatformBlengthTooSmall);

            if (minimumDescriptorSize <= bufferLeft) {

                *ValidationResult = ValidationFailed;

                *AdjustedDescriptorLength = minimumDescriptorSize;
                DbgTraceTo(IfrLog,
                    TL_ERROR,
                    Device,
                    "HW_COMPLIANCE:Descriptor Validation failed due to Platform Capability descriptor at offset 0x%x containing a bLength value (0x%x) less than expected (0x%x).  Assuming bLength is 0x%x for further validation.",
                    offset,
                    PlatformDescriptor->bLength,
                    minimumDescriptorSize,
                    *AdjustedDescriptorLength);

            } else {

                *ValidationResult = ValidationFatalError;

                DbgTraceTo(IfrLog,
                    TL_ERROR,
                    Device,
                    "HW_COMPLIANCE:Descriptor Validation failed due to Platform Capability descriptor at offset 0x%x containing a bLength value (0x%x) less than expected (0x%x).  Aborting further validation.",
                    offset,
                    PlatformDescriptor->bLength,
                    minimumDescriptorSize);
                LEAVE;

            }

        }

        if (*AdjustedDescriptorLength > bufferLeft) {
            if (HUBDESC_ShouldEnforceWin8ValidationImmutable(&ValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }

            DbgTraceTo(IfrLog,
                TL_ERROR,
                Device,
                "HW_COMPLIANCE:BOS Descriptor Validation Failed due to the Platform Capability descriptor at offset (0x%x) exceeding the descriptor buffer.  Aborting further validation.",
                offset);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorPlatformBufferTooSmall);
            LEAVE;
        }

        if (PlatformDescriptor->bReserved != 0) {
            if (HUBDESC_ShouldEnforceMutableReservedFields(&ValidationData->DevInfo)) {            
                 *ValidationResult = ValidationFailed;
            }

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:BOS Platform Descriptor Validation Error - bReserved value non-zero (%d).  Non-fatal error.",
                       PlatformDescriptor->bReserved);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorPlatformbReservedNonZero);
        }

        //
        // Verify the Platform GUID is not all zeros
        //
        if (IsEqualGUID((LPGUID) &PlatformDescriptor->PlatformCapabilityUuid, 
                        (LPGUID) &GUID_NULL)) {

            *ValidationResult = ValidationFatalError;

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:BOS Platform Descriptor Validation Error - Platform ID all zeros.");
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorPlatformIdAllZeros);
            LEAVE;
        }
    
        //
        // See if this is a MSOS 2.0 Platform Capability Descriptor and validate it if it is.
        //
        if (IsEqualGUID((LPGUID) &PlatformDescriptor->PlatformCapabilityUuid, 
                        (LPGUID) &GUID_USB_MSOS20_PLATFORM_CAPABILITY_ID)) {

            if (ValidationData->BOSInfo->SelectedDescriptorSetInfo != NULL) {
                *ValidationResult = ValidationFatalError;

                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:MS OS 2.0 Platform Descriptor Validation Failure due to multiple MS OS 2.0 platform descriptors.");
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorMsOS20PlatformDescriptorMultipleDescriptors);
                LEAVE;
            }

                            
            msOs20PlatformDescriptor = (PMSOS20_PLATFORM_CAP_DESC_HEADER)PlatformDescriptor;

            //
            // Make sure we have one or more valid descriptor information structures
            //
            if (msOs20PlatformDescriptor->bLength < sizeof(MSOS20_PLATFORM_CAP_DESC_HEADER)) {
                *ValidationResult = ValidationFatalError;

                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:MS OS 2.0 Platform Descriptor Validation Failure due to invalid bLength (%d bytes).",
                           msOs20PlatformDescriptor->bLength);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorMsOS20PlatformDescriptorInvalidbLength);
                LEAVE;
            }

            //
            // Make sure the size is the header plus an exact multiple of the descriptor set info structure
            //
            if (((msOs20PlatformDescriptor->bLength - sizeof(MSOS20_PLATFORM_CAP_DESC_HEADER)) % 
                 sizeof(MSOS20_DESCRIPTOR_SET_INFO)) != 0) {

                *ValidationResult = ValidationFatalError;

                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:MS OS 2.0 Platform Descriptor Validation Failure due to invalid bLength (%d bytes).",
                           msOs20PlatformDescriptor->bLength);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorMsOS20PlatformDescriptorInvalidbLength);
                LEAVE;
            }

            descriptorInfoCount = ((msOs20PlatformDescriptor->bLength - sizeof(MSOS20_PLATFORM_CAP_DESC_HEADER)) / 
                                   sizeof(MSOS20_DESCRIPTOR_SET_INFO)) + 1;

            highestValidWinVer = 0;
        
            //
            // Look for duplicate Windows versions
            //
            for (i=0; i < descriptorInfoCount; i++) {
                ULONG j;

                descriptorSetInfo = &msOs20PlatformDescriptor->DescriptorSetInfo[i];

                for (j = i+1; j < descriptorInfoCount; j++) {
                
                    if (msOs20PlatformDescriptor->DescriptorSetInfo[j].dwWindowsVersion ==
                         descriptorSetInfo->dwWindowsVersion) {

                        *ValidationResult = ValidationFatalError;
                        ValidationData->BOSInfo->SelectedDescriptorSetInfo = NULL;

                        DbgTraceTo(IfrLog,
                                   TL_ERROR,
                                   Device,
                                   "HW_COMPLIANCE:MS OS 2.0 Platform Descriptor Validation Failure due to multiple descriptor sets with the same Windows version (%x).",
                                   descriptorSetInfo->dwWindowsVersion);
                        LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorMsOS20PlatformDescriptorDuplicateWindowsVersion);
                    }
                }
            }        
        
            //
            // Walk the array of descriptor set info structures and find the best match for the current
            // version of Windows.        
            for (i=0; i < descriptorInfoCount; i++) {

                descriptorSetInfo = &msOs20PlatformDescriptor->DescriptorSetInfo[i];

                if (descriptorSetInfo->dwWindowsVersion > highestValidWinVer) {
                    if (RtlIsNtDdiVersionAvailable(descriptorSetInfo->dwWindowsVersion)) {

                        highestValidWinVer = descriptorSetInfo->dwWindowsVersion;
                        ValidationData->BOSInfo->SelectedDescriptorSetInfo = descriptorSetInfo;
                    }
                }
            }
        
            if (highestValidWinVer == 0) {
                DbgTraceTo(IfrLog,
                            TL_ERROR,
                            Device,
                            "HW_COMPLIANCE:MS OS 2.0 Platform Descriptor Validation - no matching Windows versions found.");
                LOG_DESCRIPTOR_VALIDATION_WARNING(&ValidationData->DevInfo, DescriptorValidationErrorMsOS20PlatformDescriptorNoWindowsVersionMatchFound);
            } 

        } else if (IsEqualGUID((LPGUID) &PlatformDescriptor->PlatformCapabilityUuid, 
                               (LPGUID) &GUID_USB_PLATFORM_FEATURES)) {

            PUSB_PLATFORM_FEATURES_DESCRIPTOR_V1 platformFeaturesDescriptor;

            platformFeaturesDescriptor = (PUSB_PLATFORM_FEATURES_DESCRIPTOR_V1)PlatformDescriptor;

            if (platformFeaturesDescriptor->bLength < sizeof(USB_PLATFORM_FEATURES_DESCRIPTOR_V1)) {
                *ValidationResult = ValidationFatalError;

                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:USB Features Platform Descriptor Validation Failure due to too small bLength (%d bytes).",
                           platformFeaturesDescriptor->bLength );
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorUsbFeaturesPlatformDescriptorbLengthTooSmall);
                LEAVE;
            } else if (platformFeaturesDescriptor->Version == USB_PLATFORM_FEATURES_VERSION_1) {
                if (platformFeaturesDescriptor->bLength != sizeof(USB_PLATFORM_FEATURES_DESCRIPTOR_V1)) {
                    *ValidationResult = ValidationFatalError;

                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:USB Features Platform Descriptor Validation Failure due to invalid bLength (%d bytes).",
                               platformFeaturesDescriptor->bLength );
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorUsbFeaturesPlatformDescriptorInvalidbLength);
                    LEAVE;
                }
            } else if (platformFeaturesDescriptor->Version < USB_PLATFORM_FEATURES_VERSION_1) {
                //
                // Minimum version number is 1
                //
                *ValidationResult = ValidationFatalError;

                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:USB Features Platform Descriptor Validation Failure due to invalid version (%d).",
                           platformFeaturesDescriptor->Version);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorUsbFeaturesPlatformDescriptorInvalidVersionNumber);
                LEAVE;
            }

            if (ValidationData->BOSInfo->UsbDualRoleFeatures != NULL) {
                *ValidationResult = ValidationFatalError;

                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:MS OS 2.0 Platform Descriptor Validation Failure due to multiple MS OS 2.0 platform descriptors.");
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorUsbFeaturesPlatformDescriptorMultipleDescriptors);
                LEAVE;
            }

            ValidationData->BOSInfo->UsbDualRoleFeatures = (PUSB_PLATFORM_FEATURES_DESCRIPTOR_V1)PlatformDescriptor;

        }

    } FINALLY {

        if (*ValidationResult == ValidationSuccess) {
            result = TRUE;
        } else {
            result = FALSE;
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in platform capability descriptor.");
        }

    }

    return result;

}

VOID
HUBDESC_InternalValidateBillboardCapabilityDescriptor(
    __in
        PUSB_DEVICE_CAPABILITY_BILLBOARD_DESCRIPTOR             BillboardDescriptor,
    __in
        PINTERNAL_BOS_VALIDATION_DATA                           ValidationData,
    __inout
        PULONG                                                  AdjustedDescriptorLength,
    __inout_opt
        PVALIDATION_RESULT                                      ValidationResult,
    __in
        RECORDER_LOG                                            IfrLog
)
/*++

Routine Description:

Performs validation of a Billboard Capability Descriptor

Returns:


--*/
{
    VALIDATION_RESULT   localResult;
    ULONG               bufferLeft;
    ULONG               offset;

    TRY {

        if (ValidationResult == NULL) {
            ValidationResult = &localResult;
            *ValidationResult = ValidationSuccess;
        }
        else if (ValidationData->DevInfo.UseWin8DescriptorValidationBehavior) {
            *ValidationResult = ValidationSuccess;
        }

        offset = (ULONG)((PUCHAR)BillboardDescriptor - ValidationData->DescriptorSetBeginning);
        bufferLeft = ValidationData->BufferLength - offset;

        if (bufferLeft < sizeof(USB_DEVICE_CAPABILITY_BILLBOARD_DESCRIPTOR)) {

            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorBillboardCapBufferTooSmall);
            *ValidationResult = ValidationFatalError;

            DbgTraceTo(IfrLog,
                TL_ERROR,
                Device,
                "HW_COMPLIANCE:Descriptor Validation failed because buffer left (0x%x) at offset 0x%x is less than the size of the Billboard Device Capability descriptor (0x%x).  Aborting further validation.",
                bufferLeft,
                offset,
                sizeof(USB_DEVICE_CAPABILITY_BILLBOARD_DESCRIPTOR));
            LEAVE;

        }

        if (BillboardDescriptor->bLength != (sizeof(USB_DEVICE_CAPABILITY_BILLBOARD_DESCRIPTOR) +
                                             sizeof(BillboardDescriptor->AlternateMode[0]) * (BillboardDescriptor->bNumberOfAlternateModes - 1))) {

            *AdjustedDescriptorLength = sizeof(USB_DEVICE_CAPABILITY_BILLBOARD_DESCRIPTOR) +
                                        sizeof(BillboardDescriptor->AlternateMode[0]) * (BillboardDescriptor->bNumberOfAlternateModes - 1);
            
            DbgTraceTo(IfrLog,
                TL_ERROR,
                Device,
                "HW_COMPLIANCE:Descriptor Validation Failed due to Billboard Device Capability descriptor at offset 0x%x containing a bLength value (0x%x) other than expected (0x%x).  Ignoring error for further validation.",
                offset,
                BillboardDescriptor->bLength,
                ((sizeof(USB_DEVICE_CAPABILITY_BILLBOARD_DESCRIPTOR) + 
                  sizeof(BillboardDescriptor->AlternateMode[0]) * (BillboardDescriptor->bNumberOfAlternateModes - 1))));
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorBillboardCapBlengthInvalid);

            //
            // Original billboard spec had a bug in it where it mandated the device to
            // hardcode the length to 40 instead of the actual length. Therefore, we 
            // need to tolerate 40. If the length is 40, assume the device is following 
            // the original spec. Also, we saw a few devices that used 44 as that is the 
            // base length of the descriptor. Ignore that as well.
            // Now that the error is logged, fix the descriptor length 
            // for further processing
            //
            if ((BillboardDescriptor->bLength == 40)||
                (BillboardDescriptor->bLength == 44)) {
                BillboardDescriptor->bLength = (UCHAR)(*AdjustedDescriptorLength);

            } else if (HUBDESC_ShouldEnforceWin8ValidationMutableCapDescriptor(&ValidationData->DevInfo, Usb31CapDescriptor)) {
                *ValidationResult = ValidationFailed;
            }

        }

        if (*AdjustedDescriptorLength > bufferLeft) {
            if (HUBDESC_ShouldEnforceWin8ValidationImmutable(&ValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }

            DbgTraceTo(IfrLog,
                TL_ERROR,
                Device,
                "HW_COMPLIANCE:BOS Descriptor Validation Failed due to the Billboard Device Capability descriptor at offset (0x%x) exceeding the descriptor buffer.  Aborting further validation.",
                offset);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorBillboardCapBufferTooSmall);
            LEAVE;
        }

        
        if (BillboardDescriptor->bNumberOfAlternateModes == 0) {

            *ValidationResult = ValidationFatalError;

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:BOS Billboard Descriptor Validation Error - bNumberOfAlternateModes is zero.");
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorBillboardCapZeroAlternateModes);
            LEAVE;
        }

        if (BillboardDescriptor->bNumberOfAlternateModes > 128) {

            *ValidationResult = ValidationFatalError;

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:BOS Billboard Descriptor Validation Error - bNumberOfAlternateModes (0x%x) is greater than 128.",
                       BillboardDescriptor->bNumberOfAlternateModes);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorBillboardCapTooManyAlternateModes);
            LEAVE;
        }

        if (BillboardDescriptor->bPreferredAlternateMode > BillboardDescriptor->bNumberOfAlternateModes) {
            *ValidationResult = ValidationFatalError;

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:BOS Billboard Descriptor Validation Error - bNumberOfAlternateModes (0x%x) is greater than 128.",
                       BillboardDescriptor->bPreferredAlternateMode);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorBillboardCapPreferredAlternateModeInvalid);
            LEAVE;
        }


        //
        // Do not check for reserved bits to be 0. This is because the original 
        // version of Billboard spec had a severe limiation that it does not 
        // has a billboard version number in it. Also, we do not want to tie the 
        // reserved bits to the USB spec version because it is desirable to keep
        // billboard spec updates indepedent of the core spec. Therefore, there 
        // is no way to find out if a reserved bit is being used or not. A newer 
        // version is currently being defined where the proposal is to use part 
        // of the reserved bits for revision info. 
        //
        #if 0
        if (BillboardDescriptor->bReserved != 0) {

            if (HUBDESC_ShouldEnforceMutableReservedFields(&ValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }                  

            DbgTraceTo(IfrLog,
                TL_ERROR,
                Device,
                "HW_COMPLIANCE:Billboard Device Capability descriptor's bReserved value (0x%x) is non-zero.",
                BillboardDescriptor->bReserved);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorBillboardCapNonZeroBReserved);
        }
        
        if (BillboardDescriptor->VconnPower.VConnPowerNeededForFullFunctionality == 7) {

            if (HUBDESC_ShouldEnforceMutableReservedFields(&ValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }                  

            DbgTraceTo(IfrLog,
                TL_ERROR,
                Device,
                "HW_COMPLIANCE:Billboard Device Capability descriptor's VCONN Power field is using a reserved value (7) for VConnPowerNeededForFullFunctionality");
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorBillboardCapReservedVconnPowerValueUsed);
        }

        if (BillboardDescriptor->VconnPower.Reserved != 0) {

            if (HUBDESC_ShouldEnforceMutableReservedFields(&ValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }                  

            DbgTraceTo(IfrLog,
                TL_ERROR,
                Device,
                "HW_COMPLIANCE:Billboard Device Capability descriptor's VCONN Power Reserved value (0x%x) is non-zero.",
                BillboardDescriptor->VconnPower.Reserved);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorBillboardCapNonZeroVConnPowerReserved);
        }
        #endif


    } FINALLY {

        if (*ValidationResult != ValidationSuccess) {
            DbgTraceTo(IfrLog,
                TL_ERROR,
                Device,
                "HW_COMPLIANCE: Descriptor validation found errors in Billboard device capability descriptor.");
        }

    }
}

VOID
HUBDESC_InternalValidateSuperSpeedPlusDeviceCapabilityDescriptor(
    __in
        PUSB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_USB_DESCRIPTOR    SSPDevCapDescriptor,
    __in
        PINTERNAL_BOS_VALIDATION_DATA                           ValidationData,
    __inout
        PULONG                                                  AdjustedDescriptorLength,
    __inout_opt
        PVALIDATION_RESULT                                      ValidationResult,
    __in
        RECORDER_LOG                                            IfrLog
)
/*++

Routine Description:

Performs validation of a SuperSpeedPlus Capability Descriptor

Returns:


--*/
{
    VALIDATION_RESULT   localResult;
    BOOLEAN             minimumFound;
    ULONG               bufferLeft;
    ULONG               offset;
    ULONG               speedsBitmap;
    ULONG               speedsUnique;
    ULONG               lastSpeedAttr;
    ULONG               i;

    TRY {

        if (ValidationResult == NULL) {
            ValidationResult = &localResult;
            *ValidationResult = ValidationSuccess;
        }
        else if (ValidationData->DevInfo.UseWin8DescriptorValidationBehavior) {
            *ValidationResult = ValidationSuccess;
        }

        offset = (ULONG)((PUCHAR)SSPDevCapDescriptor - ValidationData->DescriptorSetBeginning);
        bufferLeft = ValidationData->BufferLength - offset;

        if (SSPDevCapDescriptor->bLength < sizeof(USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_USB_DESCRIPTOR)) {

            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorSuperSpeedPlusCapBlengthTooSmall);

            if (sizeof(USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_USB_DESCRIPTOR) <= bufferLeft) {

                *ValidationResult = ValidationFailed;

                *AdjustedDescriptorLength = sizeof(USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_USB_DESCRIPTOR);
                DbgTraceTo(IfrLog,
                    TL_ERROR,
                    Device,
                    "HW_COMPLIANCE:Descriptor Validation failed due to SuperSpeedPlus Device Capability descriptor at offset 0x%x containing a bLength value (0x%x) less than expected (0x%x).  Assuming bLength is 0x%x for further validation.",
                    offset,
                    SSPDevCapDescriptor->bLength,
                    sizeof(USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_USB_DESCRIPTOR),
                    *AdjustedDescriptorLength);

            }
            else {

                *ValidationResult = ValidationFatalError;

                DbgTraceTo(IfrLog,
                    TL_ERROR,
                    Device,
                    "HW_COMPLIANCE:Descriptor Validation failed due to SuperSpeedPlus Device Capability descriptor at offset 0x%x containing a bLength value (0x%x) less than expected (0x%x).  Aborting further validation.",
                    offset,
                    SSPDevCapDescriptor->bLength,
                    sizeof(USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_USB_DESCRIPTOR));
                LEAVE;

            }

        }

        lastSpeedAttr = SSPDevCapDescriptor->bmAttributes.SublinkSpeedAttrCount;
        if (SSPDevCapDescriptor->bLength != (sizeof(USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_USB_DESCRIPTOR)+sizeof(SSPDevCapDescriptor->bmSublinkSpeedAttr[0])*lastSpeedAttr)) {

            if (HUBDESC_ShouldEnforceWin8ValidationMutableCapDescriptor(&ValidationData->DevInfo, Usb31CapDescriptor)) {
                *ValidationResult = ValidationFailed;
            }


            DbgTraceTo(IfrLog,
                TL_ERROR,
                Device,
                "HW_COMPLIANCE:Descriptor Validation Failed due to SuperSpeedPlus Device Capability descriptor at offset 0x%x containing a bLength value (0x%x) other than expected (0x%x).  Ignoring error for further validation.",
                offset,
                SSPDevCapDescriptor->bLength,
                (sizeof(USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_USB_DESCRIPTOR)+sizeof(SSPDevCapDescriptor->bmSublinkSpeedAttr[0])*lastSpeedAttr));
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorSuperSpeedPlusCapBlengthInvalid);

        }

        if (*AdjustedDescriptorLength > bufferLeft) {
            if (HUBDESC_ShouldEnforceWin8ValidationImmutable(&ValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }

            DbgTraceTo(IfrLog,
                TL_ERROR,
                Device,
                "HW_COMPLIANCE:BOS Descriptor Validation Failed due to the SuperSpeedPlus Device Capability descriptor at offset (0x%x) exceeding the descriptor buffer.  Aborting further validation.",
                offset);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorSuperSpeedPlusCapBufferTooSmall);
            LEAVE;
        }


        if (SSPDevCapDescriptor->bmAttributes.Reserved != 0) {

            if (HUBDESC_ShouldEnforceMutableReservedFields(&ValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }


            DbgTraceTo(IfrLog,
                TL_ERROR,
                Device,
                "HW_COMPLIANCE:SuperSpeedPlus Device Capability descriptor's bmAttributes value (0x%x) contains reserved values. Ignoring error.",
                SSPDevCapDescriptor->bmAttributes.AsUlong);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorSuperSpeedPlusCapBmAttributesNonZeroReservedBits);
        }

        if (SSPDevCapDescriptor->wFunctionalitySupport.Reserved != 0) {

            if (HUBDESC_ShouldEnforceMutableReservedFields(&ValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }


            DbgTraceTo(IfrLog,
                TL_ERROR,
                Device,
                "HW_COMPLIANCE:SuperSpeedPlus Device Capability descriptor's wFunctionalitySupport value (0x%x) contains reserved values. Ignoring error.",
                SSPDevCapDescriptor->wFunctionalitySupport.AsUshort);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorSuperSpeedPlusCapWFunctionalitySupportNonZeroReservedBits);
        }

        if (SSPDevCapDescriptor->bReserved != 0) {

            if (HUBDESC_ShouldEnforceMutableReservedFields(&ValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }


            DbgTraceTo(IfrLog,
                TL_ERROR,
                Device,
                "HW_COMPLIANCE:SuperSpeedPlus Device Capability descriptor's bReserved value (0x%x) is non-zero. Ignoring error.",
                SSPDevCapDescriptor->bReserved);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorSuperSpeedPlusCapNonZeroBReserved);
        }

        if (SSPDevCapDescriptor->wReserved != 0) {

            if (HUBDESC_ShouldEnforceMutableReservedFields(&ValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }


            DbgTraceTo(IfrLog,
                TL_ERROR,
                Device,
                "HW_COMPLIANCE:SuperSpeedPlus Device Capability descriptor's wReserved value (0x%x) is non-zero. Ignoring error.",
                SSPDevCapDescriptor->wReserved);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorSuperSpeedPlusCapNonZeroWReserved);
        }

        //
        // 32-bit bitmap is sufficient to accommodate all speed IDs since max number of unique speeds is 16
        //
        speedsBitmap = 0;

        speedsUnique = 0;
        minimumFound = FALSE;
        for (i = 0; i <= lastSpeedAttr; i += 2) {

            if (SSPDevCapDescriptor->bmSublinkSpeedAttr[i].SublinkTypeDir != USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_DIR_RX) {
                if (HUBDESC_ShouldEnforceWin8ValidationImmutable(&ValidationData->DevInfo)) {
                    *ValidationResult = ValidationFailed;
                }

                DbgTraceTo(IfrLog,
                    TL_ERROR,
                    Device,
                    "HW_COMPLIANCE:SuperSpeedPlus Device Capability sublink attribute's (%i) direction is not RX.  Aborting further validation.",
                    i);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorSuperSpeedPlusCapAttrRx);
                LEAVE;
            }

            //
            // Each RX entry should have a corresponding TX pair. If number of entries is odd,
            // the last RX entry (i == lastSpeedAttr) will not have a corresponding TX pair
            // at (i+1) position, and we need to report an error.
            //
            if (i == lastSpeedAttr) {
                if (HUBDESC_ShouldEnforceWin8ValidationImmutable(&ValidationData->DevInfo)) {
                    *ValidationResult = ValidationFailed;
                }

                DbgTraceTo(IfrLog,
                    TL_ERROR,
                    Device,
                    "HW_COMPLIANCE:SuperSpeedPlus Device Capability sublink attribute (%i) with expected TX direction does not exist.  Aborting further validation.",
                    i+1);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorSuperSpeedPlusCapAttrTxIncomplete);
                LEAVE;
            }

            if (SSPDevCapDescriptor->bmSublinkSpeedAttr[i+1].SublinkTypeDir != USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_DIR_TX) {
                if (HUBDESC_ShouldEnforceWin8ValidationImmutable(&ValidationData->DevInfo)) {
                    *ValidationResult = ValidationFailed;
                }

                DbgTraceTo(IfrLog,
                    TL_ERROR,
                    Device,
                    "HW_COMPLIANCE:SuperSpeedPlus Device Capability sublink attribute's (%i) direction is not TX.  Aborting further validation.",
                    i+1);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorSuperSpeedPlusCapAttrTx);
                LEAVE;
            }

            if ((SSPDevCapDescriptor->bmSublinkSpeedAttr[i].SublinkSpeedAttrID != SSPDevCapDescriptor->bmSublinkSpeedAttr[i+1].SublinkSpeedAttrID) ||
                (SSPDevCapDescriptor->bmSublinkSpeedAttr[i].SublinkTypeMode != SSPDevCapDescriptor->bmSublinkSpeedAttr[i+1].SublinkTypeMode) ||
                (SSPDevCapDescriptor->bmSublinkSpeedAttr[i].LinkProtocol != SSPDevCapDescriptor->bmSublinkSpeedAttr[i+1].LinkProtocol)) {

                if (HUBDESC_ShouldEnforceWin8ValidationImmutable(&ValidationData->DevInfo)) {
                    *ValidationResult = ValidationFailed;
                }

                DbgTraceTo(IfrLog,
                    TL_ERROR,
                    Device,
                    "HW_COMPLIANCE:SuperSpeedPlus Device Capability sublink attribute (%i): SpeedID=0x%x,Mode=0x%x,Protocol=0x%x cannot be paired with (%i): SpeedID=0x%x,Mode=0x%x,Protocol=0x%x. Ignoring error.",
                    i, SSPDevCapDescriptor->bmSublinkSpeedAttr[i].SublinkSpeedAttrID, SSPDevCapDescriptor->bmSublinkSpeedAttr[i].SublinkTypeMode, SSPDevCapDescriptor->bmSublinkSpeedAttr[i].LinkProtocol,
                    i+1, SSPDevCapDescriptor->bmSublinkSpeedAttr[i].SublinkSpeedAttrID, SSPDevCapDescriptor->bmSublinkSpeedAttr[i+1].SublinkTypeMode, SSPDevCapDescriptor->bmSublinkSpeedAttr[i+1].LinkProtocol);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorSuperSpeedPlusCapAttrPairInvalid);
            }

            if ((SSPDevCapDescriptor->bmSublinkSpeedAttr[i].SublinkTypeMode == USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_MODE_SYMMETRIC) &&
                ((SSPDevCapDescriptor->bmSublinkSpeedAttr[i].LaneSpeedExponent != SSPDevCapDescriptor->bmSublinkSpeedAttr[i+1].LaneSpeedExponent) ||
                 (SSPDevCapDescriptor->bmSublinkSpeedAttr[i].LaneSpeedMantissa != SSPDevCapDescriptor->bmSublinkSpeedAttr[i+1].LaneSpeedMantissa))) {

                if (HUBDESC_ShouldEnforceWin8ValidationImmutable(&ValidationData->DevInfo)) {
                    *ValidationResult = ValidationFailed;
                }

                DbgTraceTo(IfrLog,
                    TL_ERROR,
                    Device,
                    "HW_COMPLIANCE:SuperSpeedPlus Device Capability symmetric sublink attribute (%i): SpeedID=0x%x,Mantissa=0x%x,Exponent=0x%x cannot be paired with (%i): SpeedID=0x%x,Mantissa=0x%x,Exponent=0x%x. Ignoring error.",
                    i, SSPDevCapDescriptor->bmSublinkSpeedAttr[i].SublinkSpeedAttrID, SSPDevCapDescriptor->bmSublinkSpeedAttr[i].LaneSpeedMantissa, SSPDevCapDescriptor->bmSublinkSpeedAttr[i].LaneSpeedExponent,
                    i+1, SSPDevCapDescriptor->bmSublinkSpeedAttr[i].SublinkSpeedAttrID, SSPDevCapDescriptor->bmSublinkSpeedAttr[i+1].LaneSpeedMantissa, SSPDevCapDescriptor->bmSublinkSpeedAttr[i+1].LaneSpeedExponent);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorSuperSpeedPlusCapAttrSymmetricPairInvalid);
            }

            if (SSPDevCapDescriptor->bmSublinkSpeedAttr[i].SublinkSpeedAttrID == SSPDevCapDescriptor->wFunctionalitySupport.SublinkSpeedAttrID) {
                minimumFound = TRUE;

                if ((SSPDevCapDescriptor->bmSublinkSpeedAttr[i].SublinkTypeMode == USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_MODE_SYMMETRIC) &&
                    (SSPDevCapDescriptor->wFunctionalitySupport.MinRxLaneCount != SSPDevCapDescriptor->wFunctionalitySupport.MinTxLaneCount)) {

                    if (HUBDESC_ShouldEnforceWin8ValidationImmutable(&ValidationData->DevInfo)) {
                        *ValidationResult = ValidationFailed;
                    }

                    DbgTraceTo(IfrLog,
                        TL_ERROR,
                        Device,
                        "HW_COMPLIANCE:SuperSpeedPlus Device Capability minimum symmetric speed (SpeedID=0x%x) has invalid lane counts RX=0x%x,TX=0x%x. Ignoring error.",
                        SSPDevCapDescriptor->bmSublinkSpeedAttr[i].SublinkSpeedAttrID, SSPDevCapDescriptor->wFunctionalitySupport.MinRxLaneCount, SSPDevCapDescriptor->wFunctionalitySupport.MinTxLaneCount);
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorSuperSpeedPlusCapAttrMinimumSymmetricPairInvalid);
                }
            }

            if (speedsBitmap & (1U << SSPDevCapDescriptor->bmSublinkSpeedAttr[i].SublinkSpeedAttrID)) {
                if (HUBDESC_ShouldEnforceWin8ValidationImmutable(&ValidationData->DevInfo)) {
                    *ValidationResult = ValidationFailed;
                }

                DbgTraceTo(IfrLog,
                    TL_ERROR,
                    Device,
                    "HW_COMPLIANCE:SuperSpeedPlus Device Capability sublink attribute (%i) duplicates SpeedID=0x%x. Ignoring error.",
                    i, SSPDevCapDescriptor->bmSublinkSpeedAttr[i].SublinkSpeedAttrID);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorSuperSpeedPlusCapAttrSpeedIDDuplication);

            } else {
                speedsBitmap |= (1U << SSPDevCapDescriptor->bmSublinkSpeedAttr[i].SublinkSpeedAttrID);
                speedsUnique++;
            }
        }

        if ((SSPDevCapDescriptor->bmAttributes.SublinkSpeedIDCount + 1) != speedsUnique) {
            if (HUBDESC_ShouldEnforceWin8ValidationImmutable(&ValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }

            DbgTraceTo(IfrLog,
                TL_ERROR,
                Device,
                "HW_COMPLIANCE:SuperSpeedPlus Device Capability contains %i unique speedIDs, expected to have %i. Ignoring error.",
                speedsUnique, SSPDevCapDescriptor->bmAttributes.SublinkSpeedIDCount + 1);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorSuperSpeedPlusCapAttrSpeedIDCount);
        }

        if (minimumFound == FALSE) {
            if (HUBDESC_ShouldEnforceWin8ValidationImmutable(&ValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }

            DbgTraceTo(IfrLog,
                TL_ERROR,
                Device,
                "HW_COMPLIANCE:SuperSpeedPlus Device Capability minimum speed (SpeedID=0x%x) not found. Ignoring error.",
                SSPDevCapDescriptor->bmSublinkSpeedAttr[i].SublinkSpeedAttrID);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorSuperSpeedPlusCapAttrMinimumNotFound);
        }

    } FINALLY {

        if (*ValidationResult != ValidationSuccess) {
            DbgTraceTo(IfrLog,
                TL_ERROR,
                Device,
                "HW_COMPLIANCE: Descriptor validation found errors in SuperSpeedPlus device capability descriptor.");
        }

    }
}


BOOLEAN
HUBDESC_InternalValidateSuperSpeedDeviceCapabilityDescriptor (
    __in
        PUSB_DEVICE_CAPABILITY_SUPERSPEED_USB_DESCRIPTOR    SSDevCapDescriptor,
    __in
        PINTERNAL_BOS_VALIDATION_DATA                       ValidationData,
    __inout
        PULONG                                              AdjustedDescriptorLength,
    __inout_opt
        PVALIDATION_RESULT                                  ValidationResult,
    __out_opt
        PUSHORT                                             U1ExitLatency,
    __out_opt
        PUSHORT                                             U2ExitLatency,
    __out_opt
        PBOOLEAN                                            LtmCapable,
    __in
        RECORDER_LOG                                        IfrLog
    )
/*++

Routine Description:

    Performs validation of a SuperSpeed Capability Descriptor

Returns:


--*/
{
    VALIDATION_RESULT   localResult;
    BOOLEAN             result;    
    ULONG               bufferLeft;
    ULONG               offset;

    TRY {
        
        if (ValidationResult == NULL) {
            ValidationResult = &localResult;
            *ValidationResult = ValidationSuccess;
        } else if (ValidationData->DevInfo.UseWin8DescriptorValidationBehavior) {
            *ValidationResult = ValidationSuccess;
        }

        if (U1ExitLatency) {
            *U1ExitLatency = 0;
        }

        if (U2ExitLatency) {
            *U2ExitLatency = 0;
        }

        if (*LtmCapable) {
            *LtmCapable = FALSE;
        }

        offset = (ULONG)((PUCHAR)SSDevCapDescriptor - ValidationData->DescriptorSetBeginning);
        bufferLeft = ValidationData->BufferLength - offset;
        
        if (SSDevCapDescriptor->bLength < sizeof(USB_DEVICE_CAPABILITY_SUPERSPEED_USB_DESCRIPTOR)) {                       

            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorSuperSpeedCapBlengthTooSmall); 
            
            if (sizeof(USB_DEVICE_CAPABILITY_SUPERSPEED_USB_DESCRIPTOR) <= bufferLeft) {

                *ValidationResult = ValidationFailed;
                
                *AdjustedDescriptorLength = sizeof(USB_DEVICE_CAPABILITY_SUPERSPEED_USB_DESCRIPTOR);
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Descriptor Validation failed due to SuperSpeed Device Capability descriptor at offset 0x%x containing a bLength value (0x%x) less than expected (0x%x).  Assuming bLength is 0x%x for further validation.",
                           offset,
                           SSDevCapDescriptor->bLength,
                           sizeof(USB_DEVICE_CAPABILITY_SUPERSPEED_USB_DESCRIPTOR),
                           *AdjustedDescriptorLength);
 
            } else {

                *ValidationResult = ValidationFatalError;
   
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Descriptor Validation failed due to SuperSpeed Device Capability descriptor at offset 0x%x containing a bLength value (0x%x) less than expected (0x%x).  Aborting further validation.",
                           offset,
                           SSDevCapDescriptor->bLength,
                           sizeof(USB_DEVICE_CAPABILITY_SUPERSPEED_USB_DESCRIPTOR));
                LEAVE;

            }

        }

        if (SSDevCapDescriptor->bLength > sizeof(USB_DEVICE_CAPABILITY_SUPERSPEED_USB_DESCRIPTOR)) {
            
            if (HUBDESC_ShouldEnforceWin8ValidationMutableCapDescriptor(&ValidationData->DevInfo, Usb30CapDescriptor)) {
                *ValidationResult = ValidationFailed;
            }


            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Descriptor Validation Failed due to SuperSpeed Device Capability descriptor at offset 0x%x containing a bLength value (0x%x) greater than expected (0x%x).  Ignoring error for further validation.",
                       offset,
                       SSDevCapDescriptor->bLength,
                       sizeof(USB_DEVICE_CAPABILITY_SUPERSPEED_USB_DESCRIPTOR));
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorSuperSpeedCapBlengthTooLarge); 

        }      


        if (*AdjustedDescriptorLength > bufferLeft) {
            if (HUBDESC_ShouldEnforceWin8ValidationImmutable(&ValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:BOS Descriptor Validation Failed due to the SuperSpeed Device Capability descriptor at offset (0x%x) exceeding the descriptor buffer.  Aborting further validation.",
                       offset);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorSuperSpeedCapBufferTooSmall); 
            LEAVE;
        }


        if ((SSDevCapDescriptor->bmAttributes & USB_DEVICE_CAPABILITY_SUPERSPEED_BMATTRIBUTES_RESERVED_MASK) != 0) {

            if (HUBDESC_ShouldEnforceMutableReservedFields(&ValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }


            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:SuperSpeed Device Capability descriptor's bmAttributes value (0x%x) contains reserved values. Ignoring error.",
                       SSDevCapDescriptor->bmAttributes);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorSuperSpeedCapBmAttributesNonZeroReservedBits); 
        }
        
        if ((SSDevCapDescriptor->bmAttributes & USB_DEVICE_CAPABILITY_SUPERSPEED_BMATTRIBUTES_LTM_CAPABLE) != 0) {
            if (*LtmCapable) {
                *LtmCapable = TRUE;
            }   
        }

        if ((SSDevCapDescriptor->wSpeedsSupported & USB_DEVICE_CAPABILITY_SUPERSPEED_SPEEDS_SUPPORTED_RESERVED_MASK) != 0) {


            if (HUBDESC_ShouldEnforceMutableReservedFields(&ValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:SuperSpeed Device Capability descriptor's wSpeedsSupported value (0x%x) contains reserved values. Ignoring error.",
                       SSDevCapDescriptor->wSpeedsSupported);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorSuperSpeedCapReservedValueInWSpeedsSupported); 
        }

        if (SSDevCapDescriptor->wSpeedsSupported == 0) {
            
            *ValidationResult = ValidationFailed;

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:SuperSpeed Device Capability descriptor's wSpeedsSupported value is zero. Ignoring error for further validation.");
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorSuperSpeedCapZeroWSpeedsSupported); 
        }

        //
        // Only values 0,1,2,3 are legal
        // 
        if (SSDevCapDescriptor->bFunctionalitySupport > 3) {

            //
            // We can't normally fail this check as in the future, more speeds may get defined, and this is 
            // how the device reports that capability.
            // 
            
            if (HUBDESC_ShouldEnforceMutableReservedFields(&ValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:SuperSpeed Device Capability descriptor's bFunctionalitySupported value (0x%x) contains reserved value. Ignoring error.",
                       SSDevCapDescriptor->bFunctionalitySupport);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorSuperSpeedCapReservedValueInBFunctionalitySupported); 
        } else {

            //
            // Make sure that the corresponding speed bit is set in wSpeedsSupported
            // 
            if ((SSDevCapDescriptor->wSpeedsSupported & (1 << SSDevCapDescriptor->bFunctionalitySupport)) == 0) {
                *ValidationResult = ValidationFailed;

                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:SuperSpeed Device Capability descriptor's bFunctionalitySupport value (%d) specifies a speed that is not supported by the device according to the wSpeedsSupported value (0x%x)",
                           SSDevCapDescriptor->bFunctionalitySupport,
                           SSDevCapDescriptor->wSpeedsSupported);
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorSuperSpeedCapIncompatibleWSpeedsSupportedAndBFunctionalitySupported); 
            }

        }

        if (SSDevCapDescriptor->bU1DevExitLat >= 0x0B) {

            if (HUBDESC_ShouldEnforceWin8ValidationMutableCapDescriptor(&ValidationData->DevInfo, Usb30CapDescriptor)) {
                *ValidationResult = ValidationFailed;
            }


            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:SuperSpeed Device Capability descriptor's bU1DevExitLat value (0x%x) is in the reserved range. Ignoring error.",
                       SSDevCapDescriptor->bU1DevExitLat);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorSuperSpeedCapBU1DevExitLatTooLarge); 
        } else {
            if (U1ExitLatency) {
                *U1ExitLatency = SSDevCapDescriptor->bU1DevExitLat;
                DbgTraceTo(IfrLog,
                           TL_INFO,
                           Device,
                           "SuperSpeed Device Capability bU1DevExitLat value is (0x%x).",
                           SSDevCapDescriptor->bU1DevExitLat);
            }
        }

        if (SSDevCapDescriptor->wU2DevExitLat >= 0x0800) {

            if (HUBDESC_ShouldEnforceWin8ValidationMutableCapDescriptor(&ValidationData->DevInfo, Usb30CapDescriptor)) {
                *ValidationResult = ValidationFailed;
            }


            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:SuperSpeed Device Capability descriptor's wU2DevExitLat value (0x%x) is in the reserved range. Ignoring error.",
                       SSDevCapDescriptor->wU2DevExitLat);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorSuperSpeedCapBU2DevExitLatTooLarge); 
        } else {
            if (U2ExitLatency) {
                *U2ExitLatency = SSDevCapDescriptor->wU2DevExitLat;
                DbgTraceTo(IfrLog,
                           TL_INFO,
                           Device,
                           "SuperSpeed Device Capability bU2DevExitLat value is (0x%x).",
                           SSDevCapDescriptor->wU2DevExitLat);
            }   
        }


                    

    } FINALLY {

        if (*ValidationResult == ValidationSuccess) {
            result = TRUE;
        } else {
            result = FALSE;
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in SuperSpeed device capability descriptor.");
        }

    }

    return result;

}

BOOLEAN
HUBDESC_InternalValidateUSB20DeviceCapabilityDescriptor (
    __in
        PUSB_DEVICE_CAPABILITY_USB20_EXTENSION_DESCRIPTOR   USB20CapDescriptor,
    __in
        PINTERNAL_BOS_VALIDATION_DATA                       ValidationData,
    __inout
        PULONG                                              AdjustedDescriptorLength,
    __inout_opt
        PVALIDATION_RESULT                                  ValidationResult,
    __inout_opt
        PDEVICE_20_LPM_ATTRIBUTES                           Lpm20Attributes,
    __in
        RECORDER_LOG                                        IfrLog
    )
/*++

Routine Description:

    Performs validation of a USB 2.0 capability extension

Returns:


--*/
{
    VALIDATION_RESULT   localResult;
    BOOLEAN             result;
    ULONG               bufferLeft;
    ULONG               offset;

    TRY {
        
        if (ValidationResult == NULL) {
            ValidationResult = &localResult;
            *ValidationResult = ValidationSuccess;
        } else if (ValidationData->DevInfo.UseWin8DescriptorValidationBehavior) {
            *ValidationResult = ValidationSuccess;
        }

        offset = (ULONG)((PUCHAR)USB20CapDescriptor - ValidationData->DescriptorSetBeginning);
        bufferLeft = ValidationData->BufferLength - offset;
        
        if (USB20CapDescriptor->bLength < sizeof(USB_DEVICE_CAPABILITY_USB20_EXTENSION_DESCRIPTOR)) {            

            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorUSB20CapBlengthTooSmall); 
            
            if (sizeof(USB_DEVICE_CAPABILITY_USB20_EXTENSION_DESCRIPTOR) <= bufferLeft) {
                *ValidationResult = ValidationFailed;
                *AdjustedDescriptorLength = sizeof(USB_DEVICE_CAPABILITY_USB20_EXTENSION_DESCRIPTOR);
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Descriptor Validation failed due to USB 2.0 Extension descriptor at offset 0x%x containing a bLength value (0x%x) less than expected (0x%x).  Assuming bLength is 0x%x for further validation.",
                           offset,
                           USB20CapDescriptor->bLength,
                           sizeof(USB_DEVICE_CAPABILITY_USB20_EXTENSION_DESCRIPTOR),
                           *AdjustedDescriptorLength);
            } else {
   
                *ValidationResult = ValidationFatalError;
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Descriptor Validation failed due to USB 2.0 Extension descriptor at offset 0x%x containing a bLength value (0x%x) less than expected (0x%x).  Aborting further validation.",
                           offset,
                           USB20CapDescriptor->bLength,
                           sizeof(USB_DEVICE_CAPABILITY_USB20_EXTENSION_DESCRIPTOR));
                LEAVE;

            }

        }

        if (USB20CapDescriptor->bLength > sizeof(USB_DEVICE_CAPABILITY_USB20_EXTENSION_DESCRIPTOR)) {

            if (HUBDESC_ShouldEnforceWin8ValidationMutableCapDescriptor(&ValidationData->DevInfo, Usb20CapDescriptor)) {
                *ValidationResult = ValidationFailed;
            }

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Descriptor Validation Failed due to USB 2.0 Extension descriptor at offset 0x%x containing a bLength value (0x%x) greater than expected (0x%x).  Ignoring error for further validation.",
                       offset,
                       USB20CapDescriptor->bLength,
                       sizeof(USB_DEVICE_CAPABILITY_USB20_EXTENSION_DESCRIPTOR));
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorUSB20CapBlengthTooLarge); 

        }      

        if (*AdjustedDescriptorLength > bufferLeft) {
            if (HUBDESC_ShouldEnforceWin8ValidationImmutable(&ValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:BOS Descriptor Validation Failed due to the USB2.0 Device Capability descriptor at offset (0x%x) exceeding the descriptor buffer.  Aborting further validation.",
                       offset);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorUSB20CapBufferTooSmall); 
            LEAVE;
        }
        if ((USB20CapDescriptor->bmAttributes.AsUlong & USB_DEVICE_CAPABILITY_USB20_EXTENSION_BMATTRIBUTES_RESERVED_MASK) != 0) {
            
            if (HUBDESC_ShouldEnforceMutableReservedFields(&ValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:USB 2.0 Extension descriptor's bmAttributes value (0x%x) contains reserved values. Ignoring error.",
                       USB20CapDescriptor->bmAttributes.AsUlong);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorUSB20CapBmAttributesNonZeroReservedBits); 
        }

        if (USB20CapDescriptor->bmAttributes.LPMCapable == 1) {
            Lpm20Attributes->LPMCapable = 1;
            Lpm20Attributes->BESLAndAlternateHIRDSupported = USB20CapDescriptor->bmAttributes.BESLAndAlternateHIRDSupported;
            
            if (USB20CapDescriptor->bmAttributes.BaselineBESLValid == 1) {
                if (USB20CapDescriptor->bmAttributes.BaselineBESL != 0) {
                    Lpm20Attributes->BaselineBESLValid = 1;
                    Lpm20Attributes->BaselineBESL = USB20CapDescriptor->bmAttributes.BaselineBESL;
                } else {
                    if (HUBDESC_ShouldEnforceWin8ValidationImmutable(&ValidationData->DevInfo)) {
                        *ValidationResult = ValidationFailed;
                    }

                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:USB 2.0 Extension descriptor's bmAttributes has BaselineBESLValid as 1 but BaselineBESL is 0. Ignoring error for further validation.");
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorUSB20CapIncompatibleBaselineBESLValidAndBaselineBESL); 

                }
            }
            
            if (USB20CapDescriptor->bmAttributes.DeepBESLValid == 1) {
                if ((USB20CapDescriptor->bmAttributes.BaselineBESLValid == 1) &&
                    (USB20CapDescriptor->bmAttributes.DeepBESL <= USB20CapDescriptor->bmAttributes.BaselineBESL) ) {
                    if (HUBDESC_ShouldEnforceWin8ValidationImmutable(&ValidationData->DevInfo)) {
                        *ValidationResult = ValidationFailed;
                    }

                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:USB 2.0 Extension descriptor's bmAttributes has DeepBESL(0x%x) less than or equal to the BaselineBESL(0x%x). Ignoring error for further validation.",
                               USB20CapDescriptor->bmAttributes.DeepBESL,
                               USB20CapDescriptor->bmAttributes.BaselineBESL);
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorUSB20CapIncompatibleDeepBESLAndBaselineBESL); 

                } else if (USB20CapDescriptor->bmAttributes.DeepBESL != 0) {
                    
                    Lpm20Attributes->DeepBESLValid = 1;
                    Lpm20Attributes->DeepBESL = USB20CapDescriptor->bmAttributes.DeepBESL;

                } else {
                    
                    if (HUBDESC_ShouldEnforceWin8ValidationImmutable(&ValidationData->DevInfo)) {
                        *ValidationResult = ValidationFailed;
                    }

                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:USB 2.0 Extension descriptor's bmAttributes has DeepBESLValid as 1 but DeepBESL is 0. Ignoring error for further validation.");
                    LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorUSB20CapIncompatibleDeepBESLValidAndDeepBESL); 

                }
            }

        } else if (USB20CapDescriptor->bmAttributes.BESLAndAlternateHIRDSupported == 1) {
            
            if (HUBDESC_ShouldEnforceWin8ValidationImmutable(&ValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:USB 2.0 Extension descriptor's bmAttributes value (0x%x) has BESLAndAlternateHIRDSupported bit 1 but LPMCapable bit 0, Ignoring error for further validation.",
                       USB20CapDescriptor->bmAttributes.AsUlong);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorUSB20CapIncompatibleBESLAndAlternateHIRDSupportedAndLPMCapable); 

        }


    } FINALLY {

        if (*ValidationResult == ValidationSuccess) {
            result = TRUE;
        } else {
            result = FALSE;
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in USB 2.0 device capability descriptor.");
        }

    }

    return result;

}

__drv_requiresIRQL(PASSIVE_LEVEL)
BOOLEAN
HUBDESC_InternalValidateDeviceCapabilityDescriptor (
    __in
        PUSB_DEVICE_CAPABILITY_DESCRIPTOR   CapDescriptor,
    __in
        PINTERNAL_BOS_VALIDATION_DATA       ValidationData,
    __out
        PULONG                              AdjustedDescriptorLength,
    __inout_opt
        PVALIDATION_RESULT                  ValidationResult,
    __out_opt
        PUSHORT                             U1ExitLatency,
    __out_opt
        PUSHORT                             U2ExitLatency,
    __out_opt
        PBOOLEAN                            LtmCapable,
    __inout_opt
        PDEVICE_20_LPM_ATTRIBUTES           Lpm20Attributes,
    __in
        RECORDER_LOG                        IfrLog
    )
/*++

Routine Description:

    Performs validation of a Device Capability Descriptor

Returns:


--*/
{
    VALIDATION_RESULT   localResult;
    BOOLEAN             result;
    ULONG               bufferLeft;
    ULONG               offset;

    TRY {
        
        if (ValidationResult == NULL) {
            ValidationResult = &localResult;
            *ValidationResult = ValidationSuccess;
        } else if (ValidationData->DevInfo.UseWin8DescriptorValidationBehavior) {
            *ValidationResult = ValidationSuccess;
        }

        if (U1ExitLatency) {
            *U1ExitLatency = 0;
        }

        if (U2ExitLatency) {
            *U2ExitLatency = 0;
        }

        *AdjustedDescriptorLength = CapDescriptor->bLength;
        offset = (ULONG)((PUCHAR)CapDescriptor - ValidationData->DescriptorSetBeginning);
        bufferLeft = ValidationData->BufferLength - offset;
        
        if (CapDescriptor->bLength < sizeof(USB_DEVICE_CAPABILITY_DESCRIPTOR)) {                        

            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorDeviceCapBlengthTooSmall);
            if (sizeof(USB_DEVICE_CAPABILITY_DESCRIPTOR) <= bufferLeft) {
                *ValidationResult = ValidationFailed;
                *AdjustedDescriptorLength = sizeof(USB_DEVICE_CAPABILITY_DESCRIPTOR);
                DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Descriptor Validation failed due to Device Capability descriptor at offset 0x%x containing a bLength value (0x%x) less than expected (0x%x).  Assuming bLength is 0x%x for further validation.",
                       offset,
                       CapDescriptor->bLength,
                       sizeof(USB_DEVICE_CAPABILITY_DESCRIPTOR),
                       *AdjustedDescriptorLength);
 
            } else {
   
                *ValidationResult = ValidationFatalError;
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Descriptor Validation failed due to Container ID descriptor at offset 0x%x containing a bLength value (0x%x) less than expected (0x%x).  Aborting further validation.",
                           offset,
                           CapDescriptor->bLength,
                           sizeof(USB_DEVICE_CAPABILITY_DESCRIPTOR));
                LEAVE;

            }

        }

        if (sizeof(USB_DEVICE_CAPABILITY_DESCRIPTOR) > bufferLeft) {
        
            if (HUBDESC_ShouldEnforceWin8ValidationImmutable(&ValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:BOS Descriptor Validation Failed due to the Device Capability descriptor at offset (0x%x) exceeding the descriptor buffer.  Aborting further validation.",
                       offset);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorDeviceCapBufferTooSmall);
            LEAVE;
        
        }

        switch (CapDescriptor->bDevCapabilityType) {
        case USB_DEVICE_CAPABILITY_WIRELESS_USB:
            break;
        case USB_DEVICE_CAPABILITY_USB20_EXTENSION:
            HUBDESC_InternalValidateUSB20DeviceCapabilityDescriptor((PUSB_DEVICE_CAPABILITY_USB20_EXTENSION_DESCRIPTOR)CapDescriptor,
                                                                    ValidationData,
                                                                    AdjustedDescriptorLength,
                                                                    ValidationResult,
                                                                    Lpm20Attributes,
                                                                    IfrLog);
            if (*ValidationResult == ValidationFatalError) {
                LEAVE;
            }

            if (ValidationData->BOSInfo->USB20CapabilityDescriptor != NULL) {
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Descriptor Validation failed due to multiple instances of a USB2.0 Device Capability Descriptor being present in the BOS descriptor set.  Ignoring error for further validation.");
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorDeviceCapMultipleUSB20Cap);
                *ValidationResult = ValidationFailed;
            } else {
                ValidationData->BOSInfo->USB20CapabilityDescriptor = (PUSB_DEVICE_CAPABILITY_USB20_EXTENSION_DESCRIPTOR)CapDescriptor;
            }

           
            break;
        case USB_DEVICE_CAPABILITY_SUPERSPEED_USB:
            HUBDESC_InternalValidateSuperSpeedDeviceCapabilityDescriptor((PUSB_DEVICE_CAPABILITY_SUPERSPEED_USB_DESCRIPTOR)CapDescriptor,
                                                                         ValidationData,
                                                                         AdjustedDescriptorLength,
                                                                         ValidationResult,
                                                                         U1ExitLatency,
                                                                         U2ExitLatency,
                                                                         LtmCapable,
                                                                         IfrLog);
            if (*ValidationResult == ValidationFatalError) {
                LEAVE;
            }
            
            if (ValidationData->BOSInfo->USB30CapabilityDescriptor != NULL) {
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Descriptor Validation failed due to multiple instances of a SuperSpeed Device Capability Descriptor being present in the BOS descriptor set.  Ignoring error for further validation.");
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorDeviceCapMultipleSuperSpeedCap);
                *ValidationResult = ValidationFailed;
            } else {
                ValidationData->BOSInfo->USB30CapabilityDescriptor = (PUSB_DEVICE_CAPABILITY_SUPERSPEED_USB_DESCRIPTOR)CapDescriptor;
            }

            break;
        case USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_USB:
            HUBDESC_InternalValidateSuperSpeedPlusDeviceCapabilityDescriptor((PUSB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_USB_DESCRIPTOR)CapDescriptor,
                                                                             ValidationData,
                                                                             AdjustedDescriptorLength,
                                                                             ValidationResult,
                                                                             IfrLog);
            if (*ValidationResult == ValidationFatalError) {
                LEAVE;
            }

            if (ValidationData->BOSInfo->USB31CapabilityDescriptor != NULL) {
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Descriptor Validation failed due to multiple instances of a SuperSpeedPlus Device Capability Descriptor being present in the BOS descriptor set.  Ignoring error for further validation.");
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorDeviceCapMultipleSuperSpeedPlusCap);
                *ValidationResult = ValidationFailed;
            }
            else {
                ValidationData->BOSInfo->USB31CapabilityDescriptor = (PUSB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_USB_DESCRIPTOR)CapDescriptor;
            }

            break;
        case USB_DEVICE_CAPABILITY_CONTAINER_ID:
            HUBDESC_InternalValidateContainerIDCapabilityDescriptor((PUSB_DEVICE_CAPABILITY_CONTAINER_ID_DESCRIPTOR)CapDescriptor,
                                                                    ValidationData,
                                                                    AdjustedDescriptorLength,
                                                                    ValidationResult,
                                                                    IfrLog);

            if (*ValidationResult == ValidationFatalError) {
                LEAVE;
            }
            if (ValidationData->BOSInfo->ContainerIDDescriptor != NULL) {
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Descriptor Validation failed due to multiple instances of a Container ID Descriptor being present in the BOS descriptor set.  Ignoring error for further validation.");
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorDeviceCapMultipleContainerId);
                *ValidationResult = ValidationFailed;
            } else if (ValidationData->BOSInfo->DiscardContainerID == FALSE) {
                ValidationData->BOSInfo->ContainerIDDescriptor = (PUSB_DEVICE_CAPABILITY_CONTAINER_ID_DESCRIPTOR)CapDescriptor;
            }
            break;
        case USB_DEVICE_CAPABILITY_PLATFORM:
            HUBDESC_InternalValidatePlatformCapabilityDescriptor((PUSB_DEVICE_CAPABILITY_PLATFORM_DESCRIPTOR)CapDescriptor,
                                                                 ValidationData,
                                                                 AdjustedDescriptorLength,
                                                                 ValidationResult,
                                                                 IfrLog);

            if (*ValidationResult == ValidationFatalError) {
                LEAVE;
            }
            break;
        case USB_DEVICE_CAPABILITY_POWER_DELIVERY:
            HUBDESC_InternalValidatePowerDeliveryCapabilityDescriptor((PUSB_DEVICE_CAPABILITY_POWER_DELIVERY_DESCRIPTOR)CapDescriptor,
                                                                      ValidationData,
                                                                      AdjustedDescriptorLength,
                                                                      ValidationResult,
                                                                      IfrLog);

            if (*ValidationResult == ValidationFatalError) {
                LEAVE;
            }
            break;
        case USB_DEVICE_CAPABILITY_BILLBOARD:
            HUBDESC_InternalValidateBillboardCapabilityDescriptor((PUSB_DEVICE_CAPABILITY_BILLBOARD_DESCRIPTOR)CapDescriptor,
                                                                  ValidationData,
                                                                  AdjustedDescriptorLength,
                                                                  ValidationResult,
                                                                  IfrLog);

            if (*ValidationResult == ValidationFatalError) {
                LEAVE;
            }
            if (ValidationData->BOSInfo->BillboardDescriptor != NULL) {
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Descriptor Validation failed due to multiple instances of a Billboard Device Capability Descriptor being present in the BOS descriptor set.  Ignoring error for further validation.");
                LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorDeviceCapMultipleBillboardCap);
                *ValidationResult = ValidationFailed;
            }
            else {
                ValidationData->BOSInfo->BillboardDescriptor = (PUSB_DEVICE_CAPABILITY_BILLBOARD_DESCRIPTOR)CapDescriptor;
            }

            break;
        default:
            break;
        }

    } FINALLY {

        if (*ValidationResult == ValidationSuccess) {
            result = TRUE;
        } else {
            result = FALSE;
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in the device capability descriptor.");
        }

    }

    return result;
}
        

BOOLEAN
HUBDESC_InternalValidateBOSDescriptor (
    __in
        PUSB_BOS_DESCRIPTOR             BOSDescriptor,
    __in
        PINTERNAL_BOS_VALIDATION_DATA   ValidationData,
    __out_opt
        PULONG                          AdjustedDescriptorLength,
    __inout_opt
        PVALIDATION_RESULT              ValidationResult,
    __in
        RECORDER_LOG                    IfrLog
    )
/*++

Routine Description:

    Performs validation of the BOS descriptor 
    Header itself.  Used by both HUBDTX_ValidateAndCacheBOSDescriptorHeader
    and HUBDTX_ValidateAndCacheBOSDescriptor

Returns:


--*/
{
    VALIDATION_RESULT   localResult;
    BOOLEAN             result;
    ULONG               minLength;
    
    TRY {
                
        if (ValidationResult == NULL) {
            ValidationResult = &localResult;
            *ValidationResult = ValidationSuccess;
        } else if (ValidationData->DevInfo.UseWin8DescriptorValidationBehavior) {
            *ValidationResult = ValidationSuccess;
        }

        if (AdjustedDescriptorLength != NULL) {
            *AdjustedDescriptorLength = sizeof(USB_BOS_DESCRIPTOR);
        }
        
        if (BOSDescriptor == NULL) {
            *ValidationResult = ValidationFatalError;            
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Failing BOS descriptor validation due to NULL buffer.  Aborting validation");
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorBosNull);
            LEAVE;

        }       

        if (ValidationData->BufferLength < sizeof(USB_BOS_DESCRIPTOR)) {
            *ValidationResult = ValidationFatalError;
            
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Failing BOS descriptor validation due buffer length (%d) being less than minimum to parse (%d)to NULL buffer.  Aborting validation.",
                       ValidationData->BufferLength,
                       sizeof(USB_BOS_DESCRIPTOR));
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorBosBufferTooSmall);
        }

        if (BOSDescriptor->bDescriptorType != USB_BOS_DESCRIPTOR_TYPE) {

            *ValidationResult = ValidationFailed;
            
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:BOS Descriptor's bDescriptorType (%x) is different than expected (%x).  Assuming it is correct for further validation",
                       BOSDescriptor->bDescriptorType,
                       USB_BOS_DESCRIPTOR_TYPE);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorBosInvalidType);
            

        }

        if (BOSDescriptor->bLength < sizeof(USB_BOS_DESCRIPTOR)) {

            *ValidationResult = ValidationFailed;
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:BOS Descriptor's bLength (%d) is less than the expected size of a BOS descriptor (%d). Assuming it is %d for further validation.",
                       BOSDescriptor->bLength,
                       sizeof(USB_BOS_DESCRIPTOR),
                       sizeof(USB_BOS_DESCRIPTOR));
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorBosBlengthTooSmall);

        }

        ValidationData->DescriptorSetEnd = ValidationData->DescriptorSetBeginning + BOSDescriptor->wTotalLength;

        if (BOSDescriptor->bLength > sizeof(USB_BOS_DESCRIPTOR)) {
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:BOS Descriptor's bLength (%d) is greater than the expected size of a BOS descriptor (%d).  Assuming bLength is %d for further validation.",
                       BOSDescriptor->bLength,
                       sizeof(USB_BOS_DESCRIPTOR),
                       BOSDescriptor->bLength);

            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorBosBlengthTooLarge);
            if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }

            if (AdjustedDescriptorLength != NULL) {
                *AdjustedDescriptorLength = BOSDescriptor->bLength;
            }


        }

        if (BOSDescriptor->wTotalLength < sizeof(USB_BOS_DESCRIPTOR)) {
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:BOS Descriptor's wTotalLength (%d) is too small to even contain the BOS descriptor itself.  Ignoring error for further validation.",
                       BOSDescriptor->wTotalLength);

            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorBosWTotalLengthTooSmall);
            ValidationData->DescriptorSetEnd = ValidationData->DescriptorSetBeginning + ValidationData->BufferLength;
            *ValidationResult = ValidationFailed;
        }

        //
        // See if wLength could possibly contain the number of caps specified
        //
        minLength = BOSDescriptor->bLength + BOSDescriptor->bNumDeviceCaps * sizeof(USB_COMMON_DESCRIPTOR);

        if (BOSDescriptor->wTotalLength < minLength) {
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:BOS Descriptor's wTotalLength (%d) is too small to hold the minimum number of bytes (%d) for the specified number of device caps (%d).  Ignoring error for further validation.",
                       BOSDescriptor->wTotalLength,
                       minLength,
                       BOSDescriptor->bNumDeviceCaps);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorBosWTotalLengthTooSmall);

            ValidationData->DescriptorSetEnd = ValidationData->DescriptorSetBeginning + ValidationData->BufferLength;
            *ValidationResult = ValidationFailed;

        }

        if (BOSDescriptor->bNumDeviceCaps == 0) {
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:BOS Descriptor's bNumDeviceCaps is 0");

            if (HUBDESC_ShouldEnforceWin8ValidationMutable(&ValidationData->DevInfo)) {
                *ValidationResult = ValidationFailed;
            }

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:BOS Descriptor's wTotalLength (%d) is too small to hold the minimum number of bytes (%d) for the specified number of device caps (%d).  Ignoring error for further validation.",
                       BOSDescriptor->wTotalLength,
                       minLength,
                       BOSDescriptor->bNumDeviceCaps);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&ValidationData->DevInfo, DescriptorValidationErrorBosWTotalLengthTooSmall);

        }

    } FINALLY {

        if (*ValidationResult == ValidationSuccess) {
            result = TRUE;
        } else {
            result = FALSE;
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in the BOS descriptor.");
        }

    }

    return result;

}


__drv_requiresIRQL(PASSIVE_LEVEL)
BOOLEAN
HUBDESC_ValidateBOSDescriptorSet (
    __in
        PUSB_BOS_DESCRIPTOR         BOSDescriptor,
    __in
        ULONG                       BufferLength,
    __in
        PVALIDATION_DEVICE_INFO     DevInfo,
    __out_opt
        PBOS_DESC_SET_INFO          BOSInfo,
    __out_opt
        PUSHORT                     U1ExitLatency,
    __out_opt
        PUSHORT                     U2ExitLatency,
    __out_opt
        PBOOLEAN                    LtmCapable,
    __inout_opt
        PDEVICE_20_LPM_ATTRIBUTES   Lpm20Attributes,
    __inout_opt
        PUSB_DEVICE_PROPERTIES      DeviceFlags,
    __in
        RECORDER_LOG                IfrLog
    )
/*++

Routine Description:

    Performs validation of the entire BOS descriptor 
    set.  

Returns:

    

--*/
{
    VALIDATION_RESULT               localResult;
    BOOLEAN                         result;
    PUCHAR                          offset;
    ULONG                           descriptorLength;
    INTERNAL_BOS_VALIDATION_DATA    validationData;
    BOS_DESC_SET_INFO               localBOSInfo;
    ULONG                           NumberOfDeviceCapabilityDescriptors;

    TRY {
    
        validationData.BufferLength = BufferLength;
        validationData.DescriptorSetBeginning = (PUCHAR)BOSDescriptor;
        validationData.DescriptorSetEnd = validationData.DescriptorSetBeginning + BufferLength;
        validationData.DevInfo = *DevInfo;

        result = TRUE;

        if (BOSInfo == NULL) {
            BOSInfo = &localBOSInfo;
        }

        if (U1ExitLatency) {
            *U1ExitLatency = 0;
        }

        if (U2ExitLatency) {
            *U2ExitLatency = 0;
        }

        RtlZeroMemory(BOSInfo, sizeof(BOS_DESC_SET_INFO));

        validationData.BOSInfo = BOSInfo;

        if (BufferLength < sizeof(USB_BOS_DESCRIPTOR)) {

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Number of bytes returned for BOS Descriptor (%d) is less than the expected size of a BOS descriptor (%d).  Aborting further validation",
                       BufferLength,
                       sizeof(USB_BOS_DESCRIPTOR));
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorBosBufferTooSmall);
            result = FALSE;

            LEAVE;

        }
        
        //
        // Validate the BOS descriptor itself
        //
        localResult = ValidationSuccess;
        HUBDESC_InternalValidateBOSDescriptor(BOSDescriptor,
                                              &validationData,
                                              &descriptorLength,
                                              &localResult,
                                              IfrLog);

        if (localResult == ValidationFatalError) {
            result = FALSE;
            LEAVE;
        } else if (localResult == ValidationFailed) {
            result = FALSE;
        }

        if (BufferLength < BOSDescriptor->wTotalLength) {

            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorBosBufferTooSmall);
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Number of bytes returned for BOS Descriptor set (%d) is less than specified in wTotalLength (%d).  Assuming wTotalLength is %d for further validation.",
                       BufferLength,
                       BOSDescriptor->wTotalLength,
                       BufferLength);

            result = FALSE;

        }

        if (BufferLength > BOSDescriptor->wTotalLength) {

            LOG_DESCRIPTOR_VALIDATION_WARNING(DevInfo, DescriptorValidationErrorBosBufferTooLarge);
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Number of bytes returned for BOS Descriptor set (%d) is greater than specified in wTotalLength (%d).  Ignoring error",
                       BufferLength,
                       BOSDescriptor->wTotalLength);

            //
            // Genesys 2.1 hub will return more bytes, if we ask for more bytes. 
            // If we ask for exact bytes, then it will return exact bytes. 
            // The impact of this incorrect behavior is extremely small.
            // Ignoring the failure for now.
            //
            // localResult = ValidationFailed;

        }

        offset = ((PUCHAR)BOSDescriptor) + descriptorLength;
        
        NumberOfDeviceCapabilityDescriptors = 0;

        while (offset < validationData.DescriptorSetEnd) {
            PUSB_COMMON_DESCRIPTOR                  commonDesc;
                        
            //
            // Ensure the descriptor header is in bounds.  We always
            // need to ensure we have enough data to look at the descriptor
            // header fields.  
            //

            commonDesc = (PUSB_COMMON_DESCRIPTOR)offset;
            localResult = ValidationSuccess;
            HUBDESC_InternalValidateCommonDescriptorHeader(commonDesc,
                                                           (PINTERNAL_VALIDATION_DATA)&validationData,
                                                           &descriptorLength,
                                                           &localResult,
                                                           IfrLog);
            if (localResult == ValidationFatalError) {
                result = FALSE;
                LEAVE;
            } else if (localResult == ValidationFailed) {
                result = FALSE;
            }

            //
            // The descriptor is in bounds, we can look at it.
            //

            localResult = ValidationSuccess;
            switch (commonDesc->bDescriptorType) {
            case USB_DEVICE_CAPABILITY_DESCRIPTOR_TYPE:

                HUBDESC_InternalValidateDeviceCapabilityDescriptor((PUSB_DEVICE_CAPABILITY_DESCRIPTOR)commonDesc,
                                                                   &validationData,
                                                                   &descriptorLength,
                                                                   &localResult,
                                                                   U1ExitLatency,
                                                                   U2ExitLatency,
                                                                   LtmCapable,
                                                                   Lpm20Attributes,
                                                                   IfrLog);

                if (localResult == ValidationFatalError) {
                    result = FALSE;
                    LEAVE;
                } else if (localResult == ValidationFailed) {
                    result = FALSE;
                }

                break;
            default: 
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:BOS Descriptor contains a descriptor at offset %d with an unknown descriptor type (%d).  Ignoring error for further validation.",
                           (ULONG)(offset - (PUCHAR)BOSDescriptor),
                           commonDesc->bDescriptorType);
                LOG_DESCRIPTOR_VALIDATION_WARNING(DevInfo, DescriptorValidationErrorBosUnknownDescriptorType);

                break;

            }

            offset = offset + descriptorLength;
            NumberOfDeviceCapabilityDescriptors++;
        }

        if (BOSDescriptor->bNumDeviceCaps != NumberOfDeviceCapabilityDescriptors) {
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Number of Device Capability Descriptors (%d) does no match BOS Descriptor's bNumDeviceCaps (%d)",
                       NumberOfDeviceCapabilityDescriptors,
                       BOSDescriptor->bNumDeviceCaps);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorBosMismatchBetweenBNumDeviceCapsAndDevicCapsFound);
            result = FALSE;
        }


        if (validationData.BOSInfo->USB30CapabilityDescriptor != NULL) {
            if (DeviceFlags) {
                DeviceFlags->DeviceIsSuperSpeedCapable = 1;
            }
        } else if (validationData.DevInfo.USBSpeed == UsbSuperSpeed) {
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:BOS Descriptor set validation failed due to a SuperSpeed device not including a SuperSpeed Device Capability Descriptor");
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorBosNoSuperSpeedCap);

            result = FALSE;
        }

        if (DeviceFlags) {
            if (validationData.BOSInfo->USB31CapabilityDescriptor != NULL) {
                DeviceFlags->DeviceIsEnhancedSuperSpeedCapable = 1;
            }

            if (validationData.BOSInfo->ChargingPolicySupported) {
                DeviceFlags->ChargingPolicySupported = 1;
            }
        }

    } FINALLY {

        if (result == FALSE) {
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in the BOS descriptor set.");
        }

    }

    return result;

}


BOOLEAN
HUBDESC_InternalValidateStringDescriptor (
    __in
        PUSB_STRING_DESCRIPTOR  StringDescriptor,
    __in
        ULONG                   BufferLength,
    __out
        PULONG                  AdjustedDescriptorLength,
    __inout_opt
        PVALIDATION_RESULT      ValidationResult,
    __in
        RECORDER_LOG            IfrLog,
    __in
        PVALIDATION_DEVICE_INFO DevInfo
    )
/*++

Routine Description:

    Performs validation of an arbitrary string descriptor.

Arguments

    AdjustedDescriptorLength - on return of this function, this field will
        contain the value that should be used as the bLength value for any
        further validation.  If this value is zero, then no further validation
        should be performed.

Returns:

    False if there is a descriptor error
    True if no fatal error found

--*/
{
    VALIDATION_RESULT       localResult;
    BOOLEAN                 result;
   
    TRY {
        
        if (ValidationResult == NULL) {
            ValidationResult = &localResult;
            *ValidationResult = ValidationSuccess;
        } else if (DevInfo->UseWin8DescriptorValidationBehavior) {
            *ValidationResult = ValidationSuccess;
        }

        if ((ULONG)BufferLength < sizeof(USB_COMMON_DESCRIPTOR)) {

            //
            // Device didn't return enough buffer to parse, so we have
            // to bail here
            //
            *ValidationResult = ValidationFatalError;
            *AdjustedDescriptorLength = 0;

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:String Descriptor Buffer Too Small (%d bytes). Aborting further validation.",
                       BufferLength);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorStringBufferTooSmall);
            LEAVE;
        }

        *AdjustedDescriptorLength = (ULONG)StringDescriptor->bLength;

        if ((ULONG)StringDescriptor->bLength > BufferLength) {

            //
            // bLength says the descriptor is longer than the buffer,
            // the device must have shorted us on the data -- this can
            // lead to a buffer overrun.
            //
            *ValidationResult = ValidationFailed;
            *AdjustedDescriptorLength = BufferLength;

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Number of bytes returned (%d) less than String Descriptor bLength (%d).  Assuming bLength is (%d) for further validation",
                       BufferLength,
                       StringDescriptor->bLength,
                       BufferLength);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorStringBlengthTooLarge);
        }

        if (StringDescriptor->bLength <= sizeof(USB_COMMON_DESCRIPTOR)) {

            //
            // this is a null string (no chars), we consider this invalid
            //
            *ValidationResult = ValidationFailed;
            *AdjustedDescriptorLength = BufferLength;

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:String descriptor bLength value implies NULL string.  Assuming bLength is %d for further validation",
                       BufferLength);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorStringBlengthTooSmall);
        }

        //
        // Note that even though the bLength is smaller than the common
        // descriptor header, we know that the device returned enough bytes
        // in order to pretend that it is valid for the sake of further validation
        //
        if (StringDescriptor->bDescriptorType != USB_STRING_DESCRIPTOR_TYPE) {

            //
            // Device most likely returned garbage
            //
            *ValidationResult = ValidationFailed;

            DbgTraceTo(IfrLog, TL_ERROR, Device, "HW_COMPLIANCE:Invalid bDescriptorType (%x) for String Descriptor (%x)",
                     StringDescriptor->bDescriptorType,
                     USB_STRING_DESCRIPTOR_TYPE);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorStringInvalidType);

        }

        if (StringDescriptor->bLength & 0x1) {

            //
            // String descriptors are unicode so they must be an even
            // number of bytes
            //
            *ValidationResult = ValidationFailed;

            //
            // Lets just round down the descriptor length to see if anything else
            // can be made of it
            //
            *AdjustedDescriptorLength = *AdjustedDescriptorLength & ~1;

            DbgTraceTo(IfrLog, TL_ERROR, Device, "HW_COMPLIANCE:String Descriptor bLength (%d) is not even.  Assuming bLength is %d for further validation",
                     StringDescriptor->bLength,
                     *AdjustedDescriptorLength);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorStringBlengthNotEven);

        }

        if (StringDescriptor->bLength != BufferLength) {

            //
            // The device must have returned more data than the descriptor
            // length.  This isn't a failure, but we should log it anyway.
            //
            DbgTraceTo(IfrLog,  TL_ERROR, Device, "HW_COMPLIANCE:String Descriptor bLength (%d) doesn't match number of returned bytes (%d)",
                     StringDescriptor->bLength,
                     BufferLength);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorStringMismatchBetweenBlengthAndBufferLength);
            
        }

    } FINALLY {

        if (*ValidationResult == ValidationSuccess) {
            result = TRUE;
        } else {
            result = FALSE;
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in the string descriptor.");
        }

    }

    return result;

}


BOOLEAN
HUBDESC_ValidateDeviceDescriptor(
    __in
        PUSB_DEVICE_DESCRIPTOR      DeviceDescriptor,
    __in
        ULONG                       BufferLength,
    __in
        PVALIDATION_DEVICE_INFO     DevInfo,
    __out_opt
        PBOOLEAN                    IsBillboard,
    __in
        RECORDER_LOG                IfrLog
    )
/*++

Routine Description:

    Performs validation of a USB Device Descriptor.

Returns:

    False if there is a descriptor error
    True if no fatal error found

--*/
{
    VALIDATION_RESULT       localResult;
    BOOLEAN                 result;

    UNREFERENCED_PARAMETER(DevInfo);

    TRY {
        
        localResult = ValidationSuccess;

        if (DeviceDescriptor == NULL) {
            DbgTraceTo(IfrLog, TL_ERROR, Device, "HW_COMPLIANCE:Device Descriptor buffer is NULL");
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorDeviceNullBuffer);
            localResult = ValidationFatalError;
            LEAVE;
        }

        if ((ULONG)BufferLength < sizeof(USB_COMMON_DESCRIPTOR)) {
            //
            // Device didn't return enough buffer to parse, so we have
            // to bail here
            //
            localResult = ValidationFatalError;
            
            DbgTraceTo(IfrLog,  TL_ERROR, Device, "HW_COMPLIANCE:Device Descriptor Buffer Too Small (%d bytes). Aborting further validation.", BufferLength);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorDeviceBufferTooSmall);
            LEAVE;
        }


        if (DeviceDescriptor->bLength < sizeof(USB_DEVICE_DESCRIPTOR) ) {

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:USB_DEVICE_DESCRIPTOR bLength too small - %d bytes",
                       DeviceDescriptor->bLength);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorDeviceBlengthTooSmall);

            
            localResult = ValidationFailed;
            
        }

        if (DeviceDescriptor->bDescriptorType != USB_DEVICE_DESCRIPTOR_TYPE) {

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:USB_DEVICE_DESCRIPTOR bDescriptorType incorrect - %d",
                       DeviceDescriptor->bDescriptorType);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorDeviceIncorrectType);

            localResult = ValidationFailed;

        }

        if ((ULONG) BufferLength < 8) {
            //
            // Device didn't return enough to validate bMaxPacketSize0
            //
            DbgTraceTo(IfrLog, TL_ERROR, Device, "HW_COMPLIANCE:Device Descriptor Buffer Too Small (%d bytes). Aborting further validation.", BufferLength);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorDeviceBufferTooSmall);
            localResult = ValidationFatalError;
            LEAVE;
        }
        
        if (((DeviceDescriptor->bcdDevice >> 12) > 9) ||
            (((DeviceDescriptor->bcdDevice >> 8) & 0xf) > 9) ||
            (((DeviceDescriptor->bcdDevice >> 4) & 0xf) > 9) ||
            ((DeviceDescriptor->bcdDevice & 0xf) > 9)) {

            //
            // It is not a serious enough violation to care too much about. The
            // only problem is that for legacy devices, the hardware id might
            // appear incorrectly
            //
            
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Device Descriptor is specifying a bcdDevice that does not follow Binary Coded Decimal Format (0x%X).  Ignoring error for further validation.",
                       DeviceDescriptor->bcdDevice);
            LOG_DESCRIPTOR_VALIDATION_WARNING(DevInfo, DescriptorValidationErrorDeviceBcdDeviceHasIncorrectFormat);
        }

        //
        // Billboard class is treated as a special case for validation 
        // also for generating compat id based on device descriptor
        //
        if ((DeviceDescriptor->bDeviceClass == 0x11) &&
            (DeviceDescriptor->bDeviceSubClass == 0) &&
            (DeviceDescriptor->bDeviceProtocol == 0)) {

            if ((DeviceDescriptor->bcdUSB <= 0x200) ||
                (DeviceDescriptor->bcdUSB >= 0x300)) {

                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Billboard Device has a version (0x%x) less than 0x201.  Ignoring error for further validation.",
                           DeviceDescriptor->bcdUSB);
                LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorBillboardDeviceIncorrectBcdUsb);
                
                localResult = ValidationFailed;

            } else {

                if (IsBillboard) {
                    *IsBillboard = TRUE;
                }
            }
        } 

        if (DevInfo->USBSpeed == UsbLowSpeed) {
            
            if (DeviceDescriptor->bMaxPacketSize0 != 0x08) {
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Device Descriptor is specifying an invalid bMaxPacketSize (0x%x) which is not allowed for low-speed devices.  Ignoring error for further validation.",
                           DeviceDescriptor->bMaxPacketSize0);
                LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorDeviceInvalidBMaxPacketSize);
                //
                // COMPAT:  In the Win7 stack, we wouldn't validate the descriptor with the speed, so this shouldn't be a fatal error for legacy devices.
                //
                if (HUBDESC_ShouldEnforceWin8ValidationMutable(DevInfo)) {
                    localResult = ValidationFailed;
                }


                if ((DeviceDescriptor->bMaxPacketSize0 != 0x08) &&
                    (DeviceDescriptor->bMaxPacketSize0 != 0x10) &&
                    (DeviceDescriptor->bMaxPacketSize0 != 0x20) &&
                    (DeviceDescriptor->bMaxPacketSize0 != 0x40)) {

                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:Device Descriptor is specifying an invalid bMaxPacketSize (0x%x).  Ignoring error for further validation.",
                               DeviceDescriptor->bMaxPacketSize0);
                    LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorDeviceInvalidBMaxPacketSize);

                    localResult = ValidationFailed;

                }
            }

        } else if (DevInfo->USBSpeed == UsbFullSpeed) {

            if ((DeviceDescriptor->bMaxPacketSize0 != 0x08) &&
                (DeviceDescriptor->bMaxPacketSize0 != 0x10) &&
                (DeviceDescriptor->bMaxPacketSize0 != 0x20) &&
                (DeviceDescriptor->bMaxPacketSize0 != 0x40)) {

                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Device Descriptor is specifying an invalid bMaxPacketSize (0x%x) which is not allowed for full-speed devices.  Ignoring error for further validation.",
                           DeviceDescriptor->bMaxPacketSize0);
                LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorDeviceInvalidBMaxPacketSize);

                localResult = ValidationFailed;

            }

        } else if (DevInfo->USBSpeed == UsbHighSpeed) {

            if (DeviceDescriptor->bMaxPacketSize0 != 0x40) {
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Device Descriptor is specifying a bMaxPacketSize (0x%x) which is invalid for a high-speed device. Ignoring error for further validation.",
                           DeviceDescriptor->bMaxPacketSize0);
                LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorDeviceInvalidBMaxPacketSize);

                //
                // COMPAT:  In the Win7 stack, we wouldn't validate the descriptor with the speed, so this shouldn't be a fatal error for legacy devices.
                //
                if (HUBDESC_ShouldEnforceWin8ValidationMutable(DevInfo)) {
                    localResult = ValidationFailed;
                }


                if ((DeviceDescriptor->bMaxPacketSize0 != 0x08) &&
                    (DeviceDescriptor->bMaxPacketSize0 != 0x10) &&
                    (DeviceDescriptor->bMaxPacketSize0 != 0x20) &&
                    (DeviceDescriptor->bMaxPacketSize0 != 0x40)) {

                    DbgTraceTo(IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:Device Descriptor is specifying an invalid bMaxPacketSize (0x%x). Ignoring error for further validation.",
                               DeviceDescriptor->bMaxPacketSize0);
                    LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorDeviceInvalidBMaxPacketSize);

                    localResult = ValidationFailed;

                }

            } 

        } else if (DevInfo->USBSpeed == UsbSuperSpeed) {

            if (DeviceDescriptor->bMaxPacketSize0 != 0x09) {
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Device Descriptor is specifying a bMaxPacketSize (0x%x) which is invalid for a super-speed device. Ignoring error for further validation.",
                           DeviceDescriptor->bMaxPacketSize0);
                LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorDeviceInvalidBMaxPacketSize);

                if (HUBDESC_ShouldEnforceWin8ValidationMutable(DevInfo)) {
                    localResult = ValidationFailed;
                }

            }

        }

        
    } FINALLY {

        if (localResult == ValidationSuccess) {
            result = TRUE;
        } else {
            result = FALSE;
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in the device descriptor.");
        }

    }

    return result;
}

BOOLEAN
HUBDESC_ValidateMSOSContainerIDDescriptorHeader (
    __in
        PMS_CONTAINER_ID_DESC_HEADER    DescriptorHeader,
    __in
        ULONG                           BufferLength,
    __in
        PVALIDATION_DEVICE_INFO         DevInfo,
    __in
        RECORDER_LOG                    IfrLog
    )
/*++

Routine Description:

    Performs validation of the MS OS container ID descriptor header.

Returns:

--*/
{
    BOOLEAN     result;

    UNREFERENCED_PARAMETER(DevInfo);

    TRY {

        result = TRUE;

        if (BufferLength != sizeof(MS_CONTAINER_ID_DESC_HEADER)) {

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Number of bytes returned for MS OS Container ID Descriptor Header invalid - %d",
                       BufferLength);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMSOSContainerIdInvalidBufferSize);

            result = FALSE;
            LEAVE;
        }

        if (DescriptorHeader->bcdVersion != MS_CONTAINER_ID_DESC_VER) {

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS Container ID Descriptor Header bcdVersion (%x) invalid. Expected %x.",
                       DescriptorHeader->bcdVersion,
                       MS_CONTAINER_ID_DESC_VER);

            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMSOSContainerIdInvalidBcdVersion);
            result = FALSE;            
        }

        if (DescriptorHeader->wIndex != MS_CONTAINER_ID_DESCRIPTOR_INDEX) {

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS Container ID Descriptor Header wIndex (0x%x) invalid.  Expected %x.",
                       DescriptorHeader->wIndex,
                       MS_CONTAINER_ID_DESCRIPTOR_INDEX);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMSOSContainerIdInvalidBcdWIndex);

            result = FALSE;
            
        }

        if (DescriptorHeader->dwLength != sizeof(MS_CONTAINER_ID_DESC))  {

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS Container ID Descriptor Header dwLength (0x%x) invalid.  Expected 0x%x",
                       DescriptorHeader->dwLength,
                       sizeof(MS_CONTAINER_ID_DESC));
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMSOSContainerIdInvalidDwLength);

            result = FALSE;
            
        }

    } FINALLY {

        if (result == FALSE) {
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in the MS OS container ID descriptor header.");
        }
    }

    return result;

}

BOOLEAN
HUBDESC_ValidateMSOSContainerIDDescriptor (
    __in
        PMS_CONTAINER_ID_DESC       Descriptor,
    __in
        ULONG                       BufferLength,
    __in
        PVALIDATION_DEVICE_INFO     DevInfo,
    __in
        RECORDER_LOG                IfrLog
    )
/*++

Routine Description:

    Performs validation of the MS OS container ID descriptor.

Returns:

--*/
{
    BOOLEAN     result;

    UNREFERENCED_PARAMETER(DevInfo);

    TRY {

        result = TRUE;

        if (BufferLength != sizeof(MS_CONTAINER_ID_DESC)) {

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Number of bytes returned (%d) for MS OS Container ID Descriptor invalid. Expected %d",
                       BufferLength,
                       sizeof(MS_CONTAINER_ID_DESC));
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMSOSContainerIdInvalidBufferSize);

            result = FALSE;
            LEAVE;
        }

        //
        // Check for Container ID of all zeros
        //
        if (IsEqualGUID((LPGUID) &Descriptor->Data.bContainerID, (LPGUID) &GUID_NULL)) {

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS Container ID invalid - all zeros");
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMSOSContainerIdAllZeroes);

            result = FALSE;
            LEAVE;
        }

    } FINALLY {
        if (result == FALSE) {
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in the MS OS container ID descriptor.");
        }

    }

    return result;

}

BOOLEAN
HUBDESC_ValidateMSOSDescriptor (
    __in
        POS_STRING                  Descriptor,
    __in
        ULONG                       BufferLength,
    __in
        PVALIDATION_DEVICE_INFO     DevInfo
    )
/*++

Routine Description:

    Performs validation of the MS OS container ID descriptor.

Returns:

--*/
{
    BOOLEAN     result;

    UNREFERENCED_PARAMETER(DevInfo);
    UNREFERENCED_PARAMETER(BufferLength);

    TRY {

        result = FALSE;

        if (RtlCompareMemory(Descriptor->MicrosoftString,
                             MS_OS_STRING_SIGNATURE,
                             sizeof(Descriptor->MicrosoftString)) == 
                             sizeof(Descriptor->MicrosoftString)) {
            
            result = TRUE;                                 
        }

    } FINALLY {

    }

    return result;

}

BOOLEAN
HUBDESC_ValidateMSOSExtendedConfigDescriptor (
    __in
        PMS_EXT_CONFIG_DESC         Descriptor,
    __in
        ULONG                       BufferLength,
    __in
        PVALIDATION_DEVICE_INFO     DevInfo,
    __in
        RECORDER_LOG                IfrLog
    )
/*++

Routine Description:

    Performs validation of the MS OS extended configuration descriptor.

Returns:

--*/
{
    BOOLEAN                         result;
    PMS_EXT_CONFIG_DESC_FUNCTION    functionDescriptor;
    BOOLEAN                         gotNull;
    ULONG                           i;

    UNREFERENCED_PARAMETER(DevInfo);
    UNREFERENCED_PARAMETER(BufferLength);

    TRY {

        result = TRUE;

        if (BufferLength != Descriptor->Header.dwLength) {

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:number of bytes returned (%d) for MS OS Extended Configuration Descriptor doesn't match dwLength (%d).  Ignoring for further validation.",
                       BufferLength,
                       Descriptor->Header.dwLength);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMSOSExtendedConfigMismatchBetweenBufferSizeAndDwLength);

            if (HUBDESC_ShouldEnforceWin8ValidationImmutable(DevInfo)) {
                result = FALSE;
            }

            
        }


        functionDescriptor =
            (PMS_EXT_CONFIG_DESC_FUNCTION) Descriptor->Function;

        if (functionDescriptor->bFirstInterfaceNumber > 256) {

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS Ext. Config Descriptor Function bFirstInterfaceNumber (%d) is larger than 256 which is invalid",
                       functionDescriptor->bFirstInterfaceNumber);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMSOSExtendedConfigBFirstInterfaceNumberTooLarge);

            result = FALSE;
            LEAVE;
        }

        //
        // COMPAT: For non-composite devices the Windows 7 hub driver
        //         does not verify that the interface number in the
        //         function descriptor matches the interface number
        //         in the interface descriptor, as some devices have
        //         bad interface numbers in the function descriptor.
        //

        //
        // Make sure that the CompatibleID is valid.
        // Valid characters are 'A' through 'Z', '0' through '9', and '_"
        // and null padded to the the right end of the array, but not
        // necessarily null terminated.
        //
        gotNull = FALSE;

        for (i = 0; i < sizeof(functionDescriptor->CompatibleID); i++) {
            UCHAR   idCharacter;

            idCharacter = functionDescriptor->CompatibleID[i];

            if (idCharacter == 0) {

                gotNull = TRUE;

            } else if (gotNull ||
                       !((idCharacter >= 'A' && idCharacter <= 'Z') ||
                         (idCharacter >= '0' && idCharacter <= '9') ||
                         (idCharacter == '_'))) {

                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:MS OS Extended Configuration Descriptor Function Compatible ID Invalid");
                LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMSOSExtendedConfigInvalidCompatibleId);


                result = FALSE;
                LEAVE;
            }
        }

        gotNull = FALSE;

        for (i = 0; i < sizeof(functionDescriptor->SubCompatibleID); i++) {
            UCHAR   idCharacter;

            idCharacter = functionDescriptor->SubCompatibleID[i];

            if (idCharacter == 0) {

                gotNull = TRUE;

            } else if (gotNull ||
                       !((idCharacter >= 'A' && idCharacter <= 'Z') ||
                         (idCharacter >= '0' && idCharacter <= '9') ||
                         (idCharacter == '_'))) {

                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:MS OS Extended Configuration Descriptor Function SubCompatible ID Invalid");
                LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMSOSExtendedConfigInvalidSubCompatibleId);

                result = FALSE;
                LEAVE;
            }
        }

    } FINALLY {
        if (result == FALSE) {
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in the MS OS extended configuration descriptor.");
        }

    }

    return result;

}


BOOLEAN
HUBDESC_ValidateMSOSExtendedConfigDescriptorHeader (
    __in
        PMS_EXT_CONFIG_DESC_HEADER  Descriptor,
    __in
        ULONG                       BufferLength,
    __in
        PVALIDATION_DEVICE_INFO     DevInfo,
    __in
        RECORDER_LOG                IfrLog
    )
/*++

Routine Description:

    Performs validation of the MS OS extended configuration descriptor header.

Returns:

--*/
{
    BOOLEAN                         result;

    UNREFERENCED_PARAMETER(DevInfo);
    UNREFERENCED_PARAMETER(BufferLength);

    TRY {

        result = TRUE;

        if (BufferLength != sizeof(MS_EXT_CONFIG_DESC_HEADER)) {

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Number of bytes returned (%d) for MS OS Extended Configuration Descriptor Header invalid. Expected %d.  Aborting further validation.",
                       BufferLength,
                       sizeof(MS_EXT_CONFIG_DESC_HEADER));
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMSOSExtendedConfigInvalidBufferSize);

            result = FALSE;
            LEAVE;
        }

        if (Descriptor->bcdVersion != MS_EXT_CONFIG_DESC_VER) {

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS Extended Configuration Descriptor Header bcdVersion (%x) invalid.  Expected %x.  Ignoring for further validation.",
                       Descriptor->bcdVersion,
                       MS_EXT_CONFIG_DESC_VER);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMSOSExtendedConfigInvalidBcdVersion);

            result = FALSE;            
        }

        if (Descriptor->wIndex != MS_EXT_CONFIG_DESCRIPTOR_INDEX) {

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS Extended Configuration Descriptor Header wIndex (%x) invalid.  Expected %x.  Ignoring error for further validation.",
                       Descriptor->wIndex,
                       MS_EXT_CONFIG_DESCRIPTOR_INDEX);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMSOSExtendedConfigInvalidWIndex);

            result = FALSE;
        }

        if (Descriptor->bCount != 1) {

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS Extended Configuration Descriptor Header bCount != 1.  Aborting further validation.");
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMSOSExtendedConfigInvalidBCount);

            result = FALSE;
            LEAVE;
        }

        if (Descriptor->dwLength !=
            (sizeof(MS_EXT_CONFIG_DESC_HEADER) +
             (Descriptor->bCount *
              sizeof(MS_EXT_CONFIG_DESC_FUNCTION)))) {

            result = FALSE;

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS Extended Configuration Descriptor dwLength (%d) does not match bCount function descriptors (%d)",
                       Descriptor->dwLength,
                       (sizeof(MS_EXT_CONFIG_DESC_HEADER) + 
                          (Descriptor->bCount * sizeof(MS_EXT_CONFIG_DESC_FUNCTION))));
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMSOSExtendedConfigMismatchBetweenDwLengthAndBCount);
            LEAVE;
        }        

    } FINALLY {
        if (result == FALSE) {
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in the MS OS extended configuration descriptor header.");
        }

    }

    return result;

}

BOOLEAN
HUBDESC_ValidateSerialNumberStringDescriptor (
    __in
        PUSB_STRING_DESCRIPTOR      Descriptor,
    __inout
        PULONG                      BufferLength,
    __in
        PVALIDATION_DEVICE_INFO     DevInfo,
    __in
        RECORDER_LOG                IfrLog
    )
/*++

Routine Description:

    Performs validation of the serial number string descriptor

Returns:

--*/
{
    BOOLEAN                         result;
    ULONG                           i;
    PWCHAR                          p;

    UNREFERENCED_PARAMETER(DevInfo);    

    TRY {

        result = HUBDESC_ValidateStringDescriptor(Descriptor,
                                                  *BufferLength,
                                                  IfrLog,
                                                  DevInfo);

        if (result == FALSE) {

            LEAVE;
        }

        *BufferLength = Descriptor->bLength;


        //
        // Now lets check if the string itself is a valid serial number
        //

        //
        // Determine the length of the string itself by subtracting
        // the descriptor header
        //
        *BufferLength -= sizeof(USB_COMMON_DESCRIPTOR);

        for (i = 0; i < *BufferLength/2; i++) {

            p = &Descriptor->bString[i];

            if (*p == 0) {
                //
                // Found a NULL termination
                // 
                break;
            }
            if ((*p < L' ')
                || (*p > (WCHAR) 0x7F)
                || (*p == L',')) {

                //
                // Invalid characters in the string
                //
                result = FALSE;
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE: Invalid character detected in serial number string descriptor : 0x%u", *p);
                LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorStringInvalidCharacter);
            }
        }

        if (result == FALSE) {
            LEAVE;
        }

        //
        // Add space for a NULL to the string buffer
        //
        *BufferLength += sizeof(WCHAR);

    } FINALLY {
        if (result == FALSE) {
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in serial number string descriptor.");
        }

    }

    return result;

}

BOOLEAN
HUBDESC_Validate30HubDescriptor (
    __in
        PUSB_30_HUB_DESCRIPTOR      HubDescriptor,
    __in
        ULONG                       BufferLength,
    __in
        PVALIDATION_DEVICE_INFO     DevInfo,
    __in
        RECORDER_LOG                IfrLog
    )
/*++

Routine Description:

    Performs validation of the SuperSpeed Hub Descriptor.

Returns:

--*/
{
    VALIDATION_RESULT       localResult;
    BOOLEAN                 result;
    USB30_HUB_CHAR_BITS     usb30Characteristics;
    USHORT                  mask;
    
    TRY {
        
        localResult = ValidationSuccess;

        if (HubDescriptor == NULL) {
            DbgTraceTo(IfrLog, TL_ERROR, Device, "USB_30_HUB_DESCRIPTOR buffer is NULL");
            localResult = ValidationFatalError;
            LEAVE;
        }

        if ((ULONG)BufferLength < sizeof(USB_COMMON_DESCRIPTOR)) {
            //
            // Device didn't return enough buffer to parse, so we have
            // to bail here
            //
            localResult = ValidationFatalError;
            
            DbgTraceTo(IfrLog,  TL_ERROR, Device, "HW_COMPLIANCE:USB_30_HUB_DESCRIPTOR Buffer Too Small (%d bytes). Aborting further validation.", BufferLength);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationError30HubBufferTooSmall);

            LEAVE;
        }


        if (HubDescriptor->bLength < sizeof(USB_30_HUB_DESCRIPTOR) ) {

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:USB_30_HUB_DESCRIPTOR bLength too small - %d bytes",
                       HubDescriptor->bLength);

            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationError30HubBlengthTooSmall);
            
            localResult = ValidationFailed;
            
        }

        if (HubDescriptor->bLength > sizeof(USB_30_HUB_DESCRIPTOR) ) {

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:USB_30_HUB_DESCRIPTOR bLength too large - %d bytes",
                       HubDescriptor->bLength);
            
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationError30HubBlengthTooLarge);
            
            if (HUBDESC_ShouldEnforceWin8ValidationMutable(DevInfo)) {
                localResult = ValidationFailed;
            }
        }

        if (HubDescriptor->bDescriptorType != USB_30_HUB_DESCRIPTOR_TYPE) {

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:USB_30_HUB_DESCRIPTOR bDescriptorType incorrect - %d",
                       HubDescriptor->bDescriptorType);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationError30HubInvalidType);

            localResult = ValidationFailed;

        }

        if (HubDescriptor->bNumberOfPorts > 15) {
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:USB_30_HUB_DESCRIPTOR bNumberOfPorts (%d) is greater than 15",
                       HubDescriptor->bNumberOfPorts);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationError30HubNumberOfPortsTooLarge);
            localResult = ValidationFailed;
        }


        if (HubDescriptor->bNumberOfPorts == 0) {
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:USB_30_HUB_DESCRIPTOR bNumberOfPorts (%d) is 0",
                       HubDescriptor->bNumberOfPorts);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationError30HubNumberOfPortsZero);
            localResult = ValidationFailed;
        }


        usb30Characteristics.AsUshort16 = HubDescriptor->wHubCharacteristics;

        if (usb30Characteristics.Reserved_1 != 0) {
            if (HUBDESC_ShouldEnforceMutableReservedFields(DevInfo)) {
                localResult = ValidationFailed;
            }
            
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:USB_30_HUB_DESCRIPTOR Validation Failed due to reserved bit 1 of the wHubCharacteristics field (0x%x) is non-zero.  Ignoring error for further validation.",
                       usb30Characteristics.AsUshort16);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationError30HubWHubCharacteristicsNonZeroReservedBits);
        }

        if (usb30Characteristics.Unused != 0) {
            if (HUBDESC_ShouldEnforceMutableReservedFields(DevInfo)) {
                localResult = ValidationFailed;
            }
            
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:USB_30_HUB_DESCRIPTOR Validation Failed due to reserved bits 5-15 of the wHubCharacteristics field (0x%x) are non-zero.  Ignoring error for further validation.",
                       usb30Characteristics.AsUshort16);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationError30HubWHubCharacteristicsNonZeroReservedBits);
        }

        //
        // Note that the original upper limit for bHubHdrDecLat was 4. It was later 
        // changed to 10 in an ECN
        // 
        //
        if (HubDescriptor->bHubHdrDecLat >= 11) {
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:USB_30_HUB_DESCRIPTOR bHubHdrDecLat field (0x%x) is set to a reserved value",
                       HubDescriptor->bHubHdrDecLat);
            
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationError30HubBHubHdrDecLatHasReservedValue);
            if (HUBDESC_ShouldEnforceWin8ValidationMutable(DevInfo)) {
                localResult = ValidationFailed;
            }
        }

        if (HubDescriptor->DeviceRemovable & 0x1) {

            if (HUBDESC_ShouldEnforceMutableReservedFields(DevInfo)) {
                localResult = ValidationFailed;
            }
            
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:USB_30_HUB_DESCRIPTOR Validation Failed due to reserved bit 0 of the DeviceRemovable field is non-zero.  Ignoring error for further validation.");
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationError30HubDeviceRemovableNonZeroReservedBit);
        }


        //
        // Calculate a bitmask containing 1's in each bit beyond the number used to represent real 
        // ports
        //         
        mask = 0xffff << (HubDescriptor->bNumberOfPorts + 1);
        if ((HubDescriptor->DeviceRemovable & mask) != 0) {
            
            if (HUBDESC_ShouldEnforceWin8ValidationMutable(DevInfo)) {
                localResult = ValidationFailed;
            }
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:USB_30_HUB_DESCRIPTOR Validation Failed due to DeviceRemovable bit locations not pertaining to a port are 1");
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationError30HubDeviceRemovableBitOneForNonExistentPorts);
        }
    

        
    } FINALLY {

        if (localResult == ValidationSuccess) {
            result = TRUE;
        } else {
            result = FALSE;
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in USB_30_HUB_DESCRIPTOR.");
        }

    }
    return result;

}

BOOLEAN
HUBDESC_Validate20HubDescriptor (
    __in
        PUSB_HUB_DESCRIPTOR         HubDescriptor,
    __in
        ULONG                       BufferLength,
    __in
        PVALIDATION_DEVICE_INFO     DevInfo,
    __in
        RECORDER_LOG                IfrLog
    )
/*++

Routine Description:

    Performs validation of the USB 2.0 Hub Descriptor.

Returns:

--*/
{
    VALIDATION_RESULT       localResult;
    BOOLEAN                 result;
    USB20_HUB_CHAR_BITS     usb20Characteristics;
    ULONG                   expectedLength;
    ULONG                   expectedDeviceRemovableSize;
    ULONG                   expectedPortPwrCtrlMaskSize;
    UCHAR                   mask;
    ULONG                   i;    
    
    TRY {
        
        localResult = ValidationSuccess;

        if (HubDescriptor == NULL) {
            DbgTraceTo(IfrLog, TL_ERROR, Device, "USB_20_HUB_DESCRIPTOR buffer is NULL");
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationError20HubBufferNull);
            localResult = ValidationFatalError;
            LEAVE;
        }

        if ((ULONG)BufferLength < FIELD_OFFSET(USB_HUB_DESCRIPTOR,bRemoveAndPowerMask)) {        
            //
            // Device didn't return enough buffer to parse, so we have
            // to bail here
            //
            localResult = ValidationFatalError;
            
            DbgTraceTo(IfrLog,  TL_ERROR, Device, "HW_COMPLIANCE:USB_20_HUB_DESCRIPTOR Buffer Too Small (%d bytes). Aborting further validation.", BufferLength);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationError20HubBufferTooSmall);
            LEAVE;
        }

        //
        // COMPAT: The only validation we do for the hub descriptor
        //         in the Windows 7 USBHUB driver is for bNumberOfPorts
        //
        if (HubDescriptor->bNumberOfPorts == 0) {
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:USB_20_HUB_DESCRIPTOR bNumberOfPorts (%d) is 0",
                       HubDescriptor->bNumberOfPorts);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationError20HubNumberOfPortsZero);
            localResult = ValidationFailed;
        }

        //
        // The HUB Descriptor has 7 bytes, plus the last two fields which are variable length
        // based on the number of ports.
        // DeviceRemovable field has 1 bit per port, plus 1 reserved bit, and then rounded up to the
        //     nearest whole byte
        // PortPwrCtrlMask has one bit per port and then rounded up to the nearest whole byte.
        // 
        expectedDeviceRemovableSize = (HubDescriptor->bNumberOfPorts + 1 + 7) / 8;
        expectedPortPwrCtrlMaskSize = (HubDescriptor->bNumberOfPorts + 7) / 8;
        
        expectedLength = FIELD_OFFSET(USB_HUB_DESCRIPTOR,bRemoveAndPowerMask) +
                         expectedDeviceRemovableSize +
                         expectedPortPwrCtrlMaskSize;

        
        if (HubDescriptor->bDescriptorLength < expectedLength) {

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:USB_20_HUB_DESCRIPTOR bLength (%d) is smaller than expected (%d)",
                       HubDescriptor->bDescriptorLength,
                       expectedLength);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationError20HubBlengthTooSmall);
            if (HUBDESC_ShouldEnforceWin8ValidationImmutable(DevInfo)) {
                localResult = ValidationFailed;
            }                       
        }

        if (HubDescriptor->bDescriptorLength > expectedLength) {

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:USB_20_HUB_DESCRIPTOR bLength (%d) is larger than expected (%d)",
                       HubDescriptor->bDescriptorLength,
                       expectedLength);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationError20HubBlengthTooLarge);
            
            if (HUBDESC_ShouldEnforceWin8ValidationMutable(DevInfo)) {
                localResult = ValidationFailed;
            }
        }

        if (HubDescriptor->bDescriptorType != USB_20_HUB_DESCRIPTOR_TYPE) {

            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:USB_20_HUB_DESCRIPTOR bDescriptorType incorrect - %d",
                       HubDescriptor->bDescriptorType);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationError20HubInvalidType);

            if (HUBDESC_ShouldEnforceWin8ValidationImmutable(DevInfo)) {
                localResult = ValidationFailed;
            }                       

        }

        usb20Characteristics.AsUshort16 = HubDescriptor->wHubCharacteristics;

        //
        // Only 1.0 hubs should have set this bit, but there aren't enough of those
        // to bother checking the version number here
        // 
        if (usb20Characteristics.Reserved_1 != 0) {

            if (HUBDESC_ShouldEnforceMutableReservedFields(DevInfo)) {
                localResult = ValidationFailed;
            }
            
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:USB_20_HUB_DESCRIPTOR Validation Failed due to reserved bit 1 of the wHubCharacteristics field (0x%x) are non-zero.  Ignoring error for further validation.",
                       usb20Characteristics.AsUshort16);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationError20HubWHubCharacteristicsNonZeroReservedBits);
        }

        if (usb20Characteristics.Unused != 0) {
            if (HUBDESC_ShouldEnforceMutableReservedFields(DevInfo)) {
                localResult = ValidationFailed;
            }
            
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:USB_20_HUB_DESCRIPTOR Validation Failed due to reserved bits 5-15 of the wHubCharacteristics field (0x%x) are non-zero.  Ignoring error for further validation.",
                       usb20Characteristics.AsUshort16);
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationError20HubWHubCharacteristicsNonZeroReservedBits);
        }

        //
        // The first bit of the DeviceRemovable field is reserved
        // 
        if (HubDescriptor->bRemoveAndPowerMask[0] & 1) {

            if (HUBDESC_ShouldEnforceMutableReservedFields(DevInfo)) {
                localResult = ValidationFailed;
            }
            
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:USB_20_HUB_DESCRIPTOR Validation Failed due to reserved bit 0 of the DeviceRemovable field is non-zero.  Ignoring error for further validation.");
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationError20HubDeviceRemovableNonZeroReservedBit);
        }

        //
        // Calculate a bitmask containing 1's in each bit beyond the number used to represent real 
        // ports
        //         
        mask = 0xff << ((HubDescriptor->bNumberOfPorts + 1) % 8);
        if ((mask != 0xff) &&
            (HubDescriptor->bRemoveAndPowerMask[expectedDeviceRemovableSize-1] & mask) != 0) {
            
            if (HUBDESC_ShouldEnforceWin8ValidationMutable(DevInfo)) {
                localResult = ValidationFailed;
            }
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:USB_20_HUB_DESCRIPTOR Validation Failed due to DeviceRemovable bit locations not pertaining to a port are 1");
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationError20HubDeviceRemovableBitOneForNonExistentPorts);
        }

        for (i = 0; i < expectedPortPwrCtrlMaskSize; i++) {
            if (HubDescriptor->bRemoveAndPowerMask[expectedDeviceRemovableSize + i] != 0xff) {

                if (HUBDESC_ShouldEnforceWin8ValidationMutable(DevInfo)) {
                    localResult = ValidationFailed;
                }
                DbgTraceTo(IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:USB_20_HUB_DESCRIPTOR Validation Failed due to bits set to zero in the PortPwrCtrlMask field");
                LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationError20HubPortPwrCtrlMaskZero);
            }
        }

    } FINALLY {

        if (localResult == ValidationSuccess) {
            result = TRUE;
        } else {
            result = FALSE;
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in USB_20_HUB_DESCRIPTOR.");
        }

    }

    return result;

}


BOOLEAN
HUBDESC_GetNextMsOs20Descriptor(
    __in
        PUCHAR EndOfDescriptorSet,
    __inout
        PMSOS20_COMMON_DESCRIPTOR *CurrentDescriptor
    )
/*++

Routine Description:

    Returns the next MS OS 2.0 Descriptor

Returns:

    TRUE if next descriptor found or end of descriptor set reached. 
    *CurrentDescriptor points to the next descriptor or NULL

    FALSE if an error was detected


--*/
{
    PUCHAR                      nextDescriptor;
    PMSOS20_COMMON_DESCRIPTOR   msOs20CommonDescriptor;

    //
    // For subset descriptors walk over the the entire subset, rather returning
    // than the first descriptor within the subset
    //
    if ((*CurrentDescriptor)->wDescriptorType == MSOS20_SUBSET_HEADER_FUNCTION) {
        PMSOS20_FUNCTION_SUBSET_HEADER functionSubset;

        functionSubset = (PMSOS20_FUNCTION_SUBSET_HEADER)*CurrentDescriptor;

        if (functionSubset->wLength < sizeof(MSOS20_FUNCTION_SUBSET_HEADER)) {
            *CurrentDescriptor = NULL;
            return FALSE;
        }

        if (functionSubset->wTotalLength < 
            (sizeof(MSOS20_COMMON_DESCRIPTOR) + sizeof(MSOS20_FUNCTION_SUBSET_HEADER))) {
            
            *CurrentDescriptor = NULL;
            return FALSE;
        }
        
        nextDescriptor = (PUCHAR)functionSubset + functionSubset->wTotalLength;

    } else if ((*CurrentDescriptor)->wDescriptorType == MSOS20_SUBSET_HEADER_CONFIGURATION) {
        PMSOS20_CONFIGURATION_SUBSET_HEADER configurationSubset;

        configurationSubset = (PMSOS20_CONFIGURATION_SUBSET_HEADER)*CurrentDescriptor;

        if (configurationSubset->wLength < sizeof(MSOS20_CONFIGURATION_SUBSET_HEADER)) {
            *CurrentDescriptor = NULL;
            return FALSE;
        }

        if (configurationSubset->wTotalLength < 
            (sizeof(MSOS20_COMMON_DESCRIPTOR) + sizeof(MSOS20_CONFIGURATION_SUBSET_HEADER))) {
            
            *CurrentDescriptor = NULL;
            return FALSE;
        }

        nextDescriptor = (PUCHAR)configurationSubset + configurationSubset->wTotalLength;
    
    } else {
        
        if ((*CurrentDescriptor)->wLength < sizeof(MSOS20_COMMON_DESCRIPTOR)) {
            *CurrentDescriptor = NULL;
            return FALSE;
        }
        
        nextDescriptor = (PUCHAR)*CurrentDescriptor + (*CurrentDescriptor)->wLength;
    }

    if (nextDescriptor == EndOfDescriptorSet) {
        *CurrentDescriptor = NULL;
        return TRUE;
    }

    //
    // Make sure we aren't past the end of the set/subset
    //
    if (nextDescriptor > EndOfDescriptorSet) {
        *CurrentDescriptor = NULL;
        return FALSE;
    }

    //
    // Make sure there is room for a minimal descriptor
    //
    if ((nextDescriptor + sizeof(MSOS20_COMMON_DESCRIPTOR)) > EndOfDescriptorSet) {
        *CurrentDescriptor = NULL;
        return FALSE;
    }

    msOs20CommonDescriptor = (PMSOS20_COMMON_DESCRIPTOR) nextDescriptor;

    //
    // Make sure there is room for the entire descriptor
    //
    if ((nextDescriptor + msOs20CommonDescriptor->wLength) > EndOfDescriptorSet) {
        *CurrentDescriptor = NULL;
        return FALSE;
    }

    *CurrentDescriptor = msOs20CommonDescriptor;

    return TRUE;
}


typedef
BOOLEAN
MSOS20_VALIDATION_HANDLER(
    __in
        PVALIDATION_DEVICE_INFO     DevInfo,
    __in
        PMSOS20_INFO                MsOs20Info,
    __in
        PMSOS20_COMMON_DESCRIPTOR   MsOs20Descriptor,
    __in
        PUCHAR                      EndOfDescriptorSet
    );


typedef MSOS20_VALIDATION_HANDLER *PMSOS20_VALIDATION_HANDLER;

MSOS20_VALIDATION_HANDLER HUBDESC_ValidateMsOs20DescriptorSetHeader;
MSOS20_VALIDATION_HANDLER HUBDESC_ValidateMsOs20ConfigurationSubset;
MSOS20_VALIDATION_HANDLER HUBDESC_ValidateMsOs20FunctionSubset;
MSOS20_VALIDATION_HANDLER HUBDESC_ValidateMsOs20CompatibleId;
MSOS20_VALIDATION_HANDLER HUBDESC_ValidateMsOs20RegistryProperty;
MSOS20_VALIDATION_HANDLER HUBDESC_ValidateMsOs20MimimumRecoveryTime;
MSOS20_VALIDATION_HANDLER HUBDESC_ValidateMsOs20ModelId;
MSOS20_VALIDATION_HANDLER HUBDESC_ValidateMsOs20CcgpDevice;


typedef struct _MSOS20_DISPATCH_ENTRY {
    PMSOS20_VALIDATION_HANDLER  MsOs20ValidationHandler;
    BOOLEAN                     ValidWithinConfigurationSubset;
    BOOLEAN                     ValidWithinFunctionSubset;
} MSOS20_DISPATCH_ENTRY, *PMSOS20_DISPATCH_ENTRY;

MSOS20_DISPATCH_ENTRY  MsOs20DispatchTable[] = {
    HUBDESC_ValidateMsOs20DescriptorSetHeader,      FALSE,  FALSE,
    HUBDESC_ValidateMsOs20ConfigurationSubset,      FALSE,  FALSE,
    HUBDESC_ValidateMsOs20FunctionSubset,           TRUE,   FALSE,
    HUBDESC_ValidateMsOs20CompatibleId,             TRUE,   TRUE,
    HUBDESC_ValidateMsOs20RegistryProperty,         TRUE,   TRUE,
    HUBDESC_ValidateMsOs20MimimumRecoveryTime,      FALSE,  FALSE,
    HUBDESC_ValidateMsOs20ModelId,                  FALSE,  FALSE,
    HUBDESC_ValidateMsOs20CcgpDevice,               FALSE,  FALSE
};


BOOLEAN
HUBDESC_ValidateMsOs20DescriptorSetHeader(
    __in
        PVALIDATION_DEVICE_INFO     DevInfo,
    __in
        PMSOS20_INFO                MsOs20Info,
    __in
        PMSOS20_COMMON_DESCRIPTOR   MsOs20Descriptor,
    __in
        PUCHAR                      EndOfDescriptorSet
    )  
/*++
    
    Routine Description:

        Performs validation of a Microsoft OS 2.0 Descriptor Set Header

    Arguments:

        MsOs20Info - Structure containing info on parsed and validated MS OS 2.0 descriptors

        MsOs20Descriptor - MS OS 2.0 descriptor being validated

        EndOfDescriptorSet - End of the MS OS 2.0 Descriptor Set, or Subset, 
                             that contains the descriptor

    Returns:

        BOOLEAN
--*/
{  
    BOOLEAN     result;

    UNREFERENCED_PARAMETER(EndOfDescriptorSet);

    TRY {

        result = TRUE;
        
        // Check for duplicate MS OS 2.0 descriptor set header
        if (MsOs20Info->Flags.DescriptorSetHeader == 1) {
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 Descriptor Set Header Validation Failed due to multiple descriptor set headers.  Ignoring failure for further validation.");
   
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20MultipleDescriptorSetHeaders);
            result = FALSE;
        }

        // Validate the size of the descriptor set header
        if (MsOs20Descriptor->wLength != sizeof(MSOS20_DESCRIPTOR_SET_HEADER)) {
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 Descriptor Set Header Validation Failed due to invalid wLength (%x).",
                       MsOs20Descriptor->wLength);
   
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20DescriptorSetHeaderInvalidwLength);
            result = FALSE;
        }
    } FINALLY {
        if (result == TRUE) {
            MsOs20Info->Flags.DescriptorSetHeader = 1;
        } else {
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in MS OS 2.0 Descriptor Set Header.");
        }
    }

    return result;
}

BOOLEAN
HUBDESC_ValidateMsOs20ConfigurationSubset(
    __in
        PVALIDATION_DEVICE_INFO     DevInfo,
    __in
        PMSOS20_INFO                MsOs20Info,
    __in
        PMSOS20_COMMON_DESCRIPTOR   MsOs20Descriptor,
    __in
        PUCHAR                      EndOfDescriptorSet
    )  
/*++
    
        Performs validation of a MSOS 2.0 Configuration Subset Header and all 
        feature descriptors and funcion subsets within the subset

    Arguments:

        MsOs20Info - Structure containing info on parsed and validated MS OS 2.0 descriptors

        MsOs20Descriptor - MS OS 2.0 descriptor being validated

        EndOfDescriptorSet - End of the MS OS 2.0 Descriptor Set, or Subset, 
                             that contains the descriptor

    Returns:

        BOOLEAN
            
--*/
{  
    PMSOS20_CONFIGURATION_SUBSET_HEADER msOs20Subset;
    PMSOS20_COMMON_DESCRIPTOR           msOs20CommonDescriptor;
    PUCHAR                              endOfSubset;
    BOOLEAN                             descriptorSubsetValid;
    BOOLEAN                             result;
    ULONG                               descriptorType;
    MSOS20_INFO                         subsetInfo;

    TRY {

        descriptorSubsetValid = FALSE;

        RtlZeroBytes(&subsetInfo, sizeof(subsetInfo));

        msOs20Subset = (PMSOS20_CONFIGURATION_SUBSET_HEADER)MsOs20Descriptor;

        if (msOs20Subset->wLength != sizeof(MSOS20_CONFIGURATION_SUBSET_HEADER)) {
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 Configuration Subset Header Validation Failed due to invalid wLength (%x).",
                       msOs20Subset->wLength);
   
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20ConfigurationSubsetHeaderInvalidwLength);
            LEAVE;
        }

        //
        // Verify that the subset has room for at least one minimal OS20 descriptor
        //
        if (msOs20Subset->wTotalLength < (sizeof(MSOS20_CONFIGURATION_SUBSET_HEADER) +  
                                          sizeof(MSOS20_COMMON_DESCRIPTOR))) {

            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 Configuration Subset Header Validation Failed due to too small wTotalLength (%x).",
                       msOs20Subset->wTotalLength);
   
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20ConfigurationSubsetHeaderTooSmallwTotalLength);
            LEAVE;
        }

        //
        // Make sure the subset doesn't extend past the end of the MS OS 2.0 descriptor set
        //
        if (((PUCHAR)msOs20Subset + msOs20Subset->wTotalLength) > EndOfDescriptorSet) {
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 Configuration Subset Header Validation Failed due to too large wTotalLength (%x).",
                       msOs20Subset->wTotalLength);
   
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20ConfigurationSubsetHeaderTooLargewTotalLength);
            LEAVE;
        }

        endOfSubset = (PUCHAR)msOs20Subset + msOs20Subset->wTotalLength;
        msOs20CommonDescriptor = (PMSOS20_COMMON_DESCRIPTOR)((PUCHAR)msOs20Subset + msOs20Subset->wLength);

        descriptorSubsetValid = TRUE;

        //
        // validate each feature descriptor within the subset
        //
        do {

            descriptorType = msOs20CommonDescriptor->wDescriptorType;

            if (descriptorType < (sizeof(MsOs20DispatchTable) / sizeof(MSOS20_DISPATCH_ENTRY))) {

                PMSOS20_VALIDATION_HANDLER msOs20ValidationHandler;
            
                msOs20ValidationHandler = MsOs20DispatchTable[descriptorType].MsOs20ValidationHandler;

                //
                // Make sure the descriptor is valid within a configuration subset
                //
                if (MsOs20DispatchTable[descriptorType].ValidWithinConfigurationSubset == FALSE) {
                    DbgTraceTo(MsOs20Info->IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:MS OS 2.0 Configuration Subset Header Validation Failed due to invalid feature descriptor type (%d) within subset. Ignoring failure for further validation.",
                               descriptorType);
   
                    LOG_DESCRIPTOR_VALIDATION_WARNING(DevInfo, DescriptorValidationErrorMsOs20ConfigurationSubsetHeaderInvalidFeatureDescriptor);
                }

                result = msOs20ValidationHandler(DevInfo,
                                                 &subsetInfo,
                                                 msOs20CommonDescriptor,
                                                 endOfSubset);
            
                if (result == FALSE) {
                    descriptorSubsetValid = FALSE;
                }
            } else {
                DbgTraceTo(MsOs20Info->IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Found MS OS 2.0 Descriptor with unknown descriptor type %d.  Not error, continuing validation.",
                           descriptorType);
            }

            result = HUBDESC_GetNextMsOs20Descriptor(endOfSubset,
                                                     &msOs20CommonDescriptor);

            if (result == FALSE) {
                DbgTraceTo(MsOs20Info->IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:MS OS 2.0 Configuration Subset Header Validation Failed due to invalid feature descriptor length.");
   
                LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20ConfigurationSubsetHeaderInvalidFeatureDescriptorLength);

                descriptorSubsetValid = FALSE;

                LEAVE;
            }

        } while (msOs20CommonDescriptor != NULL);

    } FINALLY {
        if (descriptorSubsetValid == TRUE) {
            MsOs20Info->Flags.ConfigurationSubset = 1;
        } else {
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in MS OS 2.0 Descriptor Configuration Subset Header.");
        }
    }

    return descriptorSubsetValid;
}


BOOLEAN
HUBDESC_ValidateMsOs20FunctionSubset(
    __in
        PVALIDATION_DEVICE_INFO     DevInfo,
    __in
        PMSOS20_INFO                MsOs20Info,
    __in
        PMSOS20_COMMON_DESCRIPTOR   MsOs20Descriptor,
    __in
        PUCHAR                      EndOfDescriptorSet
    )  
/*++
    
    Routine Description:

        Performs validation of a MSOS 2.0 Function Subset Header and all 
        feature descriptors within the subset

    Arguments:

        MsOs20Info - Structure containing info on parsed and validated MS OS 2.0 descriptors

        MsOs20Descriptor - MS OS 2.0 descriptor being validated

        EndOfDescriptorSet - End of the MS OS 2.0 Descriptor Set, or Subset, 
                             that contains the descriptor

    Returns:

        BOOLEAN
            
--*/
{  
    PMSOS20_FUNCTION_SUBSET_HEADER  msOs20Subset;
    PMSOS20_COMMON_DESCRIPTOR       msOs20CommonDescriptor;
    PUCHAR                          endOfSubset;
    BOOLEAN                         descriptorSubsetValid;
    BOOLEAN                         result;
    ULONG                           descriptorType;
    MSOS20_INFO                     subsetInfo;

    TRY {

        descriptorSubsetValid = FALSE;

        RtlZeroBytes(&subsetInfo, sizeof(subsetInfo));

        msOs20Subset = (PMSOS20_FUNCTION_SUBSET_HEADER)MsOs20Descriptor;

        if (msOs20Subset->wLength != sizeof(MSOS20_FUNCTION_SUBSET_HEADER)) {
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 Function Subset Header Validation Failed due to invalid wLength (%x).",
                       msOs20Subset->wLength);
   
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20FunctionSubsetHeaderInvalidwLength);
            LEAVE;
        }

        //
        // Verify that the subset has room for at least one minimal feature descriptor
        //
        if (msOs20Subset->wTotalLength < (sizeof(MSOS20_FUNCTION_SUBSET_HEADER) +  
                                          sizeof(MSOS20_COMMON_DESCRIPTOR))) {

            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 Function Subset Header Validation Failed due to too small wTotalLength (%x).",
                       msOs20Subset->wTotalLength);
   
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20FunctionSubsetHeaderTooSmallwTotalLength);
            LEAVE;
        }

        if (((PUCHAR)msOs20Subset + msOs20Subset->wTotalLength) > EndOfDescriptorSet) {
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 Function Subset Header Validation Failed due to too large wTotalLength (%x).",
                       msOs20Subset->wTotalLength);
   
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20FunctionSubsetHeaderTooLargewTotalLength);
            LEAVE;
        }

        endOfSubset = (PUCHAR)msOs20Subset + msOs20Subset->wTotalLength;
        msOs20CommonDescriptor = (PMSOS20_COMMON_DESCRIPTOR)((PUCHAR)msOs20Subset + msOs20Subset->wLength);

        descriptorSubsetValid = TRUE;

        //
        // validate each feature descriptor within the subset
        //
        do {

            descriptorType = msOs20CommonDescriptor->wDescriptorType;

            if (descriptorType < (sizeof(MsOs20DispatchTable) / sizeof(MSOS20_DISPATCH_ENTRY))) {

                PMSOS20_VALIDATION_HANDLER msOs20ValidationHandler;
            
                msOs20ValidationHandler = MsOs20DispatchTable[descriptorType].MsOs20ValidationHandler;

                //
                // Make sure the feature descriptor is valid within a function subset
                //
                if (MsOs20DispatchTable[descriptorType].ValidWithinFunctionSubset == FALSE) {
                    DbgTraceTo(MsOs20Info->IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:MS OS 2.0 Function Subset Header Validation Failed due to invalid feature descriptor type (%d) within subset. Ignoring failure for further validation.",
                               descriptorType);
   
                    LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20FunctionSubsetHeaderInvalidFeatureDescriptor);

                    descriptorSubsetValid = FALSE;
                }

                // Validate the descriptor
                result = msOs20ValidationHandler(DevInfo,
                                                 &subsetInfo,
                                                 msOs20CommonDescriptor,
                                                 endOfSubset);
            
                if (result == FALSE) {
                    descriptorSubsetValid = FALSE;
                }
            } else {
                DbgTraceTo(MsOs20Info->IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Found MS OS 2.0 Descriptor with unknown descriptor type %d.  Not error, continuing validation.",
                           descriptorType);
            }

            result = HUBDESC_GetNextMsOs20Descriptor(endOfSubset,
                                                     &msOs20CommonDescriptor);

            if (result == FALSE) {
                DbgTraceTo(MsOs20Info->IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:MS OS 2.0 Function Subset Header Validation Failed due to invalid feature descriptor length.");
   
                LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20FunctionSubsetHeaderInvalidFeatureDescriptorLength);

                descriptorSubsetValid = FALSE;

                LEAVE;
            }

        } while (msOs20CommonDescriptor != NULL);

    } FINALLY {
        if (descriptorSubsetValid == TRUE) {
            MsOs20Info->Flags.FunctionSubset = 1;
        } else {
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in MS OS 2.0 Descriptor Function Subset Header.");
        }

    }

    return descriptorSubsetValid;
}


BOOLEAN
HUBDESC_ValidateMsOs20CompatibleId(
    __in
        PVALIDATION_DEVICE_INFO     DevInfo,
    __in
        PMSOS20_INFO                MsOs20Info,
    __in
        PMSOS20_COMMON_DESCRIPTOR   MsOs20Descriptor,
    __in
        PUCHAR                      EndOfDescriptorSet
    )  
/*++
    
    Routine Description:

        Performs validation of a MS OS 2.0 Compatible ID

    Arguments:

        MsOs20Info - Structure containing info on parsed and validated MS OS 2.0 descriptors

        MsOs20Descriptor - MS OS 2.0 descriptor being validated

        EndOfDescriptorSet - End of the MS OS 2.0 Descriptor Set, or Subset, 
                             that contains the descriptor

    Returns:

        BOOLEAN
            
--*/
{  
    PMSOS20_FEATURE_COMPATIBLE_ID_DESCRIPTOR    msOs20CompatibleId;
    ULONG                                       i;
    UCHAR                                       idChar;
    BOOLEAN                                     nullTermination;
    BOOLEAN                                     result;

    UNREFERENCED_PARAMETER(EndOfDescriptorSet);
    
    TRY {

        result = TRUE;

        if (MsOs20Info->Flags.CompatibleId == 1) {
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 Compatible ID Descriptor Validation Failed due to multiple compatible ID descriptors.");
   
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20ComatibleIdDescriptorMultipleDescriptors);
            result = FALSE;
        }

        msOs20CompatibleId = (PMSOS20_FEATURE_COMPATIBLE_ID_DESCRIPTOR)MsOs20Descriptor;

        if (msOs20CompatibleId->wLength != sizeof(MSOS20_FEATURE_COMPATIBLE_ID_DESCRIPTOR)) {
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 Compatible ID Descriptor Validation Failed due to invalid wLength (%x).",
                       msOs20CompatibleId->wLength);
   
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20ComatibleIdDescriptorInvalidwLength);
            result = FALSE;
            LEAVE;
        }

        nullTermination = FALSE;

        for (i = 0; i < sizeof(msOs20CompatibleId->CompatibleId); i++) {

            idChar = msOs20CompatibleId->CompatibleId[i];

            if ((idChar != 0) && (nullTermination == TRUE)) {
                
                DbgTraceTo(MsOs20Info->IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:MS OS 2.0 Compatible ID Descriptor Validation Failed due non-NULL character after NULL termination.");
    
                LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20ComatibleIdDescriptorNonNullAfterNullTermination);
                result = FALSE;
                LEAVE;

            } else if (idChar == 0) {
                nullTermination = TRUE;
            } else if (!((idChar >= 'A' && idChar <= 'Z') || 
                         (idChar >= '0' && idChar <= '9') || 
                         (idChar == '_'))) {

                DbgTraceTo(MsOs20Info->IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:MS OS 2.0 Compatible ID Descriptor Validation Failed due to invalid character (%x).",
                            idChar);
    
                LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20ComatibleIdDescriptorInvalidwLength);
                result = FALSE;
                LEAVE;
            }
        }

        nullTermination = FALSE;

        for (i = 0; i < sizeof(msOs20CompatibleId->SubCompatibleId); i++) {

            idChar = msOs20CompatibleId->SubCompatibleId[i];

            if ((idChar != 0) && (nullTermination == TRUE)) {

                DbgTraceTo(MsOs20Info->IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:MS OS 2.0 Compatible ID Descriptor Validation Failed due non-NULL character after NULL termination.");
    
                LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20ComatibleIdDescriptorNonNullAfterNullTermination);
                result = FALSE;
                LEAVE;

            } else if (idChar == 0) {
                nullTermination = TRUE;
            } else if (!((idChar >= 'A' && idChar <= 'Z') || 
                         (idChar >= '0' && idChar <= '9') || 
                         (idChar == '_'))) {

                DbgTraceTo(MsOs20Info->IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:MS OS 2.0 Compatible ID Descriptor Validation Failed due to invalid character (%x).",
                            idChar);
    
                LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20ComatibleIdDescriptorInvalidwLength);
                result = FALSE;
                LEAVE;
            }
        }
    
    } FINALLY {
        if (result == TRUE) {
            MsOs20Info->CompatibleIdDescriptor = msOs20CompatibleId;
            MsOs20Info->Flags.CompatibleId= 1;
        } else {
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in MS OS 2.0 Compatible ID Descriptor.");
        }
    }

    return result;
}



BOOLEAN
HUBDESC_ValidateMsOs20RegistryProperty(
    __in
        PVALIDATION_DEVICE_INFO     DevInfo,
    __in
        PMSOS20_INFO                MsOs20Info,
    __in
        PMSOS20_COMMON_DESCRIPTOR   MsOs20Descriptor,
    __in
        PUCHAR                      EndOfDescriptorSet
    )  
/*++
    
    Routine Description:

        Performs validation of a MS OS 2.0 Registry Value Descriptor

    Arguments:

        MsOs20Info - Structure containing info on parsed and validated MS OS 2.0 descriptors

        MsOs20Descriptor - MS OS 2.0 descriptor being validated

        EndOfDescriptorSet - End of the MS OS 2.0 Descriptor Set, or Subset, 
                             that contains the descriptor

    Returns:

        BOOLEAN
            
--*/
{  
    PMSOS20_FEATURE_REGISTRY_VALUE_DESCRIPTOR   msOs20RegistryValue;
    USHORT                                      propertyNameLength;
    USHORT                                      propertyDataLength;
    BOOLEAN                                     result;

    UNREFERENCED_PARAMETER(EndOfDescriptorSet);

    TRY {

        result = TRUE;
        
        msOs20RegistryValue = (PMSOS20_FEATURE_REGISTRY_VALUE_DESCRIPTOR)MsOs20Descriptor;

        if (msOs20RegistryValue->wLength < (sizeof(MSOS20_FEATURE_REGISTRY_VALUE_DESCRIPTOR) + 
                                            sizeof(MSOS20_FEATURE_REGISTRY_VALUE_PROPERTY_DATA))) {
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 Registry Value Descriptor Validation Failed due to too small wLength (%x).",
                        msOs20RegistryValue->wLength);
    
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20RegistryValueDescriptorTooSmallwLength);
            result = FALSE;
            LEAVE;
        }

        propertyNameLength = msOs20RegistryValue->wPropertyNameLength;

        if (msOs20RegistryValue->wLength < (sizeof(MSOS20_FEATURE_REGISTRY_VALUE_DESCRIPTOR) - sizeof(WCHAR) + 
                                            propertyNameLength + 
                                            sizeof(MSOS20_FEATURE_REGISTRY_VALUE_PROPERTY_DATA))) {
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 Registry Value Descriptor Validation Failed due to wPropertyNameLength too large (%x).",
                        propertyNameLength);
    
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20RegistryValueDescriptorTooLargewPropertyNameLength);
            result = FALSE;
            LEAVE;
        }

        if ((propertyNameLength == 0) ||
            ((propertyNameLength % sizeof(WCHAR)) != 0)) {

            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 Registry Value Descriptor Validation Failed due to wPropertyNameLength being odd value or zero (%x).",
                        propertyNameLength);
    
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20RegistryValueDescriptorOddOrZerowPropertyNameLength);
            result = FALSE;
            LEAVE;
        }

        propertyDataLength = *(PUSHORT)((PUCHAR)MsOs20Descriptor + 
                                        FIELD_OFFSET(MSOS20_FEATURE_REGISTRY_VALUE_DESCRIPTOR, PropertyName) +
                                        propertyNameLength);

        if (msOs20RegistryValue->wLength < (sizeof(MSOS20_FEATURE_REGISTRY_VALUE_DESCRIPTOR) - sizeof(WCHAR) + 
                                            sizeof(MSOS20_FEATURE_REGISTRY_VALUE_PROPERTY_DATA) - sizeof(UCHAR) + 
                                            propertyNameLength + 
                                            propertyDataLength)) {
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 Registry Value Descriptor Validation Failed due to wPropertyDataLength too large (%x).",
                        propertyDataLength);
    
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20RegistryValueDescriptorTooLargewPropertyDataLength);
            result = FALSE;
            LEAVE;
        }

        if (propertyDataLength == 0) {
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 Registry Value Descriptor Validation Failed due to zero wPropertyDataLength (%x).",
                        propertyDataLength);
    
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20RegistryValueDescriptorZerowPropertyDataLength);
            result = FALSE;
            LEAVE;
        }

        if ((msOs20RegistryValue->wPropertyDataType < REG_SZ) || 
            (msOs20RegistryValue->wPropertyDataType > REG_MULTI_SZ)) {

            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 Registry Value Descriptor Validation Failed due to invalid wPropertyDataType (%d).",
                        msOs20RegistryValue->wPropertyDataType);
    
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20RegistryValueDescriptorInvalidwPropertyDataType);
            result = FALSE;
            LEAVE;
        }
    } FINALLY {
        if (result == TRUE) {
            MsOs20Info->Flags.RegistryProperty = 1;
        } else {
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in MS OS 2.0 Registry Value Descriptor.");
        }
    }

    return result;
}


BOOLEAN
HUBDESC_ValidateMsOs20MimimumRecoveryTime(
    __in
        PVALIDATION_DEVICE_INFO     DevInfo,
    __in
        PMSOS20_INFO                MsOs20Info,
    __in
        PMSOS20_COMMON_DESCRIPTOR   MsOs20Descriptor,
    __in
        PUCHAR                      EndOfDescriptorSet
    )  
/*++
    
    Routine Description:

        Performs validation of a MS OS 2.0 Minimum Recovery Time Descriptor

    Arguments:

        MsOs20Info - Structure containing info on parsed and validated MS OS 2.0 descriptors

        MsOs20Descriptor - MS OS 2.0 descriptor being validated

        EndOfDescriptorSet - End of the MS OS 2.0 Descriptor Set, or Subset, 
                             that contains the descriptor

    Returns:

        BOOLEAN
            
--*/
{  
    PMSOS20_FEATURE_MINIMUM_RESUME_TIME_DESCRIPTOR      msOs20MinResumeTime;
    BOOLEAN                                             result;

    UNREFERENCED_PARAMETER(EndOfDescriptorSet);
    
    TRY {

        result = TRUE;

        msOs20MinResumeTime = (PMSOS20_FEATURE_MINIMUM_RESUME_TIME_DESCRIPTOR)MsOs20Descriptor;

        if (MsOs20Info->Flags.MinResumeTime == 1) {
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 Minimum Resume Time Descriptor Validation Failed due to multiple recovery time descriptors.");
    
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20MinumumResumeTimeDescriptorMultipleDescriptors);
            result = FALSE;
        }

        if (msOs20MinResumeTime->wLength != sizeof(MSOS20_FEATURE_MINIMUM_RESUME_TIME_DESCRIPTOR)) {
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 Minimum Resume Time Descriptor Validation Failed due to invalid wLength (%x).",
                       msOs20MinResumeTime->wLength);
    
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20MinumumResumeTimeDescriptorInvalidwLength);
            result = FALSE;
            LEAVE;
        }

        if (msOs20MinResumeTime->bResumeRecoveryTime > 10) {
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 Minimum Resume Time Descriptor Validation Failed due to invalid bResumeRecoveryTime (%x).  Ignoring error for further validation.",
                       msOs20MinResumeTime->bResumeRecoveryTime);
    
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20MinumumResumeTimeDescriptorInvalidbResumeRecoveryTime);
            result = FALSE;
        }

        if ((msOs20MinResumeTime->bResumeSignalingTime == 0) || (msOs20MinResumeTime->bResumeSignalingTime > PORT_RESUME_TIME)) {
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 Minimum Resume Time Descriptor Validation Failed due to invalid bResumeSignalingTime value (%x).  Ignoring error for further validation.",
                       msOs20MinResumeTime->bResumeSignalingTime);
    
            LOG_DESCRIPTOR_VALIDATION_WARNING(DevInfo, DescriptorValidationErrorMsOs20MinumumResumeTimeDescriptorInvalidbResumeSignalingTimeValue);
        }
    } FINALLY {
        if (result == TRUE) {
            MsOs20Info->MinResumeTimeDescriptor = msOs20MinResumeTime;
            MsOs20Info->Flags.MinResumeTime = 1;
        } else {
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in MS OS 2.0 Minimum Recovery Time Descriptor.");
        }
    }

    return result;
}


BOOLEAN
HUBDESC_ValidateMsOs20ModelId(
    __in
        PVALIDATION_DEVICE_INFO     DevInfo,
    __in
        PMSOS20_INFO                MsOs20Info,
    __in
        PMSOS20_COMMON_DESCRIPTOR   MsOs20Descriptor,
    __in
        PUCHAR                      EndOfDescriptorSet
    )  
/*++
    
    Routine Description:

        Performs validation of a MS OS 2.0 Model ID Descriptor

    Arguments:

        MsOs20Info - Structure containing info on parsed and validated MS OS 2.0 descriptors

        MsOs20Descriptor - MS OS 2.0 descriptor being validated

        EndOfDescriptorSet - End of the MS OS 2.0 Descriptor Set, or Subset, 
                             that contains the descriptor

    Returns:

        BOOLEAN
            
--*/
{  
    PMSOS20_FEATURE_MODEL_ID_DESCRIPTOR     msOs20ModelId;
    BOOLEAN                                 result;

    UNREFERENCED_PARAMETER(EndOfDescriptorSet);
    
    TRY {

        result = TRUE;

        msOs20ModelId = (PMSOS20_FEATURE_MODEL_ID_DESCRIPTOR)MsOs20Descriptor;

        if (MsOs20Info->Flags.ModelId == 1) {
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 Model ID Descriptor Validation Failed due to multiple instances of the descriptor.");
    
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20ModelIdDescriptorMultipleDescriptors);
            result = FALSE;
            LEAVE;
        }

        if (msOs20ModelId->wLength != sizeof(MSOS20_FEATURE_MODEL_ID_DESCRIPTOR)) {
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 Model ID Descriptor Validation Failed due to invalid wLength (%x).",
                       msOs20ModelId->wLength);
    
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20ModelIdDescriptorInvalidwLength);
            result = FALSE;
            LEAVE;
        }

        if (IsEqualGUID((LPGUID) &msOs20ModelId->ModelId, (LPGUID) &GUID_NULL)) {
        DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 Model ID Descriptor Validation Failed due to NULL UUID.");
    
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20ModelIdDescriptorNullUuid);
            result = FALSE;
            LEAVE;
        }
    } FINALLY {
        if (result == TRUE) {
            MsOs20Info->ModelIdDescriptor = msOs20ModelId;
            MsOs20Info->Flags.ModelId = 1;
        } else {
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in MS OS 2.0 Model ID Descriptor.");
        }
    }

    return result;
}


BOOLEAN
HUBDESC_ValidateMsOs20CcgpDevice(
    __in
        PVALIDATION_DEVICE_INFO     DevInfo,
    __in
        PMSOS20_INFO                MsOs20Info,
    __in
        PMSOS20_COMMON_DESCRIPTOR   MsOs20Descriptor,
    __in
        PUCHAR                      EndOfDescriptorSet
    )  
/*++
    
    Routine Description:

        Performs validation of a MS OS 2.0 CCGP Descriptor

    Arguments:

        MsOs20Info - Structure containing info on parsed and validated MS OS 2.0 descriptors

        MsOs20Descriptor - MS OS 2.0 descriptor being validated

        EndOfDescriptorSet - End of the MS OS 2.0 Descriptor Set, or Subset, 
                             that contains the descriptor

    Returns:

        BOOLEAN
            
--*/
{  
    PMSOS20_FEATURE_CCGP_DEVICE_DESCRIPTOR  msOs20CcgpDevice;
    BOOLEAN                                 result;

    UNREFERENCED_PARAMETER(EndOfDescriptorSet);
    
    TRY {

        result = TRUE;

        msOs20CcgpDevice = (PMSOS20_FEATURE_CCGP_DEVICE_DESCRIPTOR)MsOs20Descriptor;

        if (MsOs20Info->Flags.CcgpDevice == 1) {
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 CCGP Device Descriptor Validation Failed due to multiple instances of the descriptor.");
    
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20CcgpDeviceMultipleDescriptors);
            result = FALSE;
            LEAVE;
        }

        if (msOs20CcgpDevice->wLength != sizeof(MSOS20_FEATURE_CCGP_DEVICE_DESCRIPTOR)) {
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 CCGP Device Descriptor Validation Failed due to invalid wLength (%x).",
                       msOs20CcgpDevice->wLength);
    
            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMsOs20CcgpDeviceDescriptorInvalidwLength);
            result = FALSE;
            LEAVE;
        }

    } FINALLY {
        if (result == TRUE) {
            MsOs20Info->CcgpDeviceDescriptor = msOs20CcgpDevice;
            MsOs20Info->Flags.CcgpDevice = 1;
        } else {
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in MS OS 2.0 CCGP Device Descriptor.");
        }
    }

    return result;
}


BOOLEAN
HUBDESC_ValidateMsOs20DescriptorSet(
    __in
        PVALIDATION_DEVICE_INFO         DevInfo,
    __in
        PMSOS20_INFO                    MsOs20Info
    )
/*++

Routine Description:

    Performs validation of the entire MS OS 2.0 descriptor set.  

Arguments:

    MsOs20DescriptorSet - MS OS 2.0 descriptor set to be validated

Returns:

    BOOLEAN

--*/
{
    PMSOS20_COMMON_DESCRIPTOR       msOs20CommonDescriptor;
    PMSOS20_DESCRIPTOR_SET_HEADER   msOs20DescriptorSet;
    PUCHAR                          endOfDescriptorSet;
    BOOLEAN                         descriptorSetValid;
    BOOLEAN                         result;
    ULONG                           descriptorType;
    PMSOS20_DESCRIPTOR_SET_INFO     descriptorSetInfo;

    TRY {

        msOs20DescriptorSet = MsOs20Info->DescriptorSet;
        descriptorSetInfo = &MsOs20Info->DescriptorSetInfo;
        
        descriptorSetValid = TRUE;

        if (MsOs20Info->BytesReturned != descriptorSetInfo->wLength) {
            
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 Descriptor Set Validation Failed due to bytes returned (%d bytes) not matching descriptor set info wLength (%d bytes). Aborting further validation.",
                       MsOs20Info->BytesReturned,
                       descriptorSetInfo->wLength);

            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMSOS20DescriptorSetBytesReturnNotEqualToDescriptorSetwLength);
            descriptorSetValid = FALSE;
            LEAVE;
        }

        if (MsOs20Info->BytesReturned < sizeof(MSOS20_DESCRIPTOR_SET_HEADER)) {
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 Descriptor Set Validation Failed due to bytes returned (%d bytes) being to small to contain descriptor set header. Aborting further validation.",
                       MsOs20Info->BytesReturned);

            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMSOS20DescriptorSetBytesReturnTooSmall);
            descriptorSetValid = FALSE;
            LEAVE;
        }

        if (msOs20DescriptorSet->wLength != sizeof(MSOS20_DESCRIPTOR_SET_HEADER)) {
            
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 Descriptor Set Validation Failed due to invalid descriptor set header wLength (%d bytes). Aborting further validation.",
                       msOs20DescriptorSet->wLength);

            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMSOS20DescriptorSetHeaderInvalidwLength);
            descriptorSetValid = FALSE;
            LEAVE;
        }

        if (descriptorSetInfo->wLength != msOs20DescriptorSet->wTotalLength) {
            
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 Descriptor Set Validation Failed due to descriptor set header wLength (%d bytes) not equal to descriptor info wLength (%d bytes). Aborting further validation.",
                       descriptorSetInfo->wLength,
                       msOs20DescriptorSet->wTotalLength);

            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMSOS20DescriptorSetwTotalLengthMismatch);
            descriptorSetValid = FALSE;
            LEAVE;
        }

        //
        // Verify the descriptor set Windows version is less than or equal to the selected Windows version
        //
        if (msOs20DescriptorSet->dwWindowsVersion > descriptorSetInfo->dwWindowsVersion) {
        
            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MS OS 2.0 Descriptor Set Validation Failed due descriptor set dwWindowsVersion (%x) being greater than the descriptor set info dwWindowsVersion (%x). Aborting further validation.",
                       msOs20DescriptorSet->dwWindowsVersion,
                       descriptorSetInfo->dwWindowsVersion);

            LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMSOS20DescriptordwWindowsVersionMismatch);
            descriptorSetValid = FALSE;
            LEAVE;
        }

        msOs20CommonDescriptor = (PMSOS20_COMMON_DESCRIPTOR)msOs20DescriptorSet;
        endOfDescriptorSet = (PUCHAR)msOs20DescriptorSet + msOs20DescriptorSet->wTotalLength;

        //
        // Walk the descriptor set and validate each descriptor
        //
        do {
            
            descriptorType = msOs20CommonDescriptor->wDescriptorType;

            if (descriptorType < (sizeof(MsOs20DispatchTable) / sizeof(MSOS20_DISPATCH_ENTRY))) {

                PMSOS20_VALIDATION_HANDLER msOs20ValidationHandler;
            
                msOs20ValidationHandler = MsOs20DispatchTable[descriptorType].MsOs20ValidationHandler;

                result = msOs20ValidationHandler(DevInfo,
                                                 MsOs20Info,
                                                 msOs20CommonDescriptor,
                                                 endOfDescriptorSet);

                if (result == FALSE) {
                    descriptorSetValid = FALSE;
                }

                //
                // Look for out-of-order top-level feature descriptors.
                // Top-level feature descriptors should precede all subsets
                //
                if (((descriptorType != MSOS20_SUBSET_HEADER_CONFIGURATION) &&
                     (descriptorType != MSOS20_SUBSET_HEADER_FUNCTION)) && 
                    ((MsOs20Info->Flags.FunctionSubset == 1) || 
                     (MsOs20Info->Flags.ConfigurationSubset == 1))) {

                    DbgTraceTo(MsOs20Info->IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE:MS OS 2.0 Descriptor Set Validation Failed due to top-level feature descriptor found after configuration or function subset.");

                    LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMSOS20DescriptorOutOfOrderTopLevelFeatureDescriptor);
                    descriptorSetValid = FALSE;
                }
            } else {
                DbgTraceTo(MsOs20Info->IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE:Found MS OS 2.0 Descriptor with unknown descriptor type %d.  Not error, continuing validation.",
                           descriptorType);
            }
            
            result = HUBDESC_GetNextMsOs20Descriptor(endOfDescriptorSet,
                                                        &msOs20CommonDescriptor);

            if (result == FALSE) {
                DbgTraceTo(MsOs20Info->IfrLog,
                            TL_ERROR,
                            Device,
                            "HW_COMPLIANCE:MS OS 2.0 Descriptor Set Validation Failed due to descriptor extending past end of descriptor set.  Aborting further validation.");

                LOG_DESCRIPTOR_VALIDATION_ERROR(DevInfo, DescriptorValidationErrorMSOS20DescriptorRunsPastEndOfDescriptorSet);
                descriptorSetValid = FALSE;
                LEAVE;
            }

        } while (msOs20CommonDescriptor != NULL);
        
    } FINALLY {
        if (descriptorSetValid == FALSE) {
            MsOs20Info->Flags.AsUlong32 = 0;

            DbgTraceTo(MsOs20Info->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Descriptor validation found errors in MS OS 2.0 Descriptor Set.");
        }
    }
    
    return descriptorSetValid;
}

