/*++
Copyright (c) Microsoft Corporation

Module Name:

    descvalidation.h

Abstract:

    Declarations for descriptor validation code

--*/


#pragma once

// {00000000-0000-0000-0000-000000000000}
DEFINE_GUID(GUID_NULL, 
0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);


typedef enum _VALIDATION_RESULT {
    ValidationSuccess,
    ValidationFatalError,
    ValidationFailed
} VALIDATION_RESULT, *PVALIDATION_RESULT;

typedef enum _DESCRIPTOR_VALIDATION_ERROR {    
    DescriptorValidationError20HubBlengthTooLarge,
    DescriptorValidationError20HubBlengthTooSmall,
    DescriptorValidationError20HubBufferNull,
    DescriptorValidationError20HubBufferTooSmall,
    DescriptorValidationError20HubDeviceRemovableBitOneForNonExistentPorts,
    DescriptorValidationError20HubDeviceRemovableNonZeroReservedBit,
    DescriptorValidationError20HubInvalidType,
    DescriptorValidationError20HubNumberOfPortsZero,
    DescriptorValidationError20HubPortPwrCtrlMaskZero,
    DescriptorValidationError20HubWHubCharacteristicsNonZeroReservedBits,
    DescriptorValidationError30HubBHubHdrDecLatHasReservedValue,
    DescriptorValidationError30HubBlengthTooLarge,
    DescriptorValidationError30HubBlengthTooSmall,
    DescriptorValidationError30HubBufferTooSmall,
    DescriptorValidationError30HubDeviceRemovableBitOneForNonExistentPorts,
    DescriptorValidationError30HubDeviceRemovableNonZeroReservedBit,
    DescriptorValidationError30HubInvalidType,
    DescriptorValidationError30HubNumberOfPortsTooLarge,
    DescriptorValidationError30HubNumberOfPortsZero,
    DescriptorValidationError30HubWHubCharacteristicsNonZeroReservedBits,
    DescriptorValidationErrorBosBlengthTooLarge,
    DescriptorValidationErrorBosBlengthTooSmall,
    DescriptorValidationErrorBosBufferTooLarge,
    DescriptorValidationErrorBosBufferTooSmall,
    DescriptorValidationErrorBosInvalidType,
    DescriptorValidationErrorBosMismatchBetweenBNumDeviceCapsAndDevicCapsFound,
    DescriptorValidationErrorBosNoSuperSpeedCap,
    DescriptorValidationErrorBosNull,
    DescriptorValidationErrorBosUnknownDescriptorType,
    DescriptorValidationErrorBosWTotalLengthTooSmall,
    DescriptorValidationErrorBulkEndpointAtLowSpeed,
    DescriptorValidationErrorBulkEndpointBmAttributesNonZeroReservedBits,
    DescriptorValidationErrorBulkEndpointInvalidWMaxPacketSize,
    DescriptorValidationErrorCompanionBulkEndpointBmAttributesNonZeroReservedBits,
    DescriptorValidationErrorCompanionBulkEndpointBMaxBurstTooLarge,
    DescriptorValidationErrorCompanionBulkEndpointMaxStreamsTooLarge,
    DescriptorValidationErrorCompanionBulkEndpointNonZeroWBytesPerInterval,
    DescriptorValidationErrorCompanionControlEndpointNonZeroBmAttributes,
    DescriptorValidationErrorCompanionControlEndpointNonZeroBMaxBurst,
    DescriptorValidationErrorCompanionControlEndpointNonZeroWBytesPerInterval,
    DescriptorValidationErrorCompanionEndpointAtNonSuperSpeed,
    DescriptorValidationErrorCompanionEndpointAtUnexpectedLocation,
    DescriptorValidationErrorCompanionEndpointBlengthTooLarge,
    DescriptorValidationErrorCompanionEndpointBlengthTooSmall,
    DescriptorValidationErrorCompanionEndpointBufferTooSmall,
    DescriptorValidationErrorCompanionInterruptEndpointBMaxBurstTooLarge,
    DescriptorValidationErrorCompanionInterruptEndpointIncompatibleBMaxBurstAndWMaxPacketSize,
    DescriptorValidationErrorCompanionInterruptEndpointNonZeroBmAttributes, 
    DescriptorValidationErrorCompanionInterruptEndpointWBytesPerIntervalTooLarge,
    DescriptorValidationErrorCompanionIoschEndpointBmAttributesNonZeroReservedBits,
    DescriptorValidationErrorCompanionIsochEndpointBMaxBurstTooLarge,
    DescriptorValidationErrorCompanionIsochEndpointIncompatibleBMaxBurstAndWMaxPacketSize,
    DescriptorValidationErrorCompanionIsochEndpointMultTooLarge,
    DescriptorValidationErrorCompanionIsochEndpointWBytesPerIntervalTooLarge,
    DescriptorValidationErrorConfigBlengthTooLarge,
    DescriptorValidationErrorConfigBlengthTooSmall,
    DescriptorValidationErrorConfigBufferTooSmall,
    DescriptorValidationErrorConfigBufferTooSmallToHaveInterface,
    DescriptorValidationErrorConfigInvalidType,
    DescriptorValidationErrorConfigMissingCompanionEndpoint,
    DescriptorValidationErrorConfigNull,
    DescriptorValidationErrorConfigWTotalLengthTooLarge,
    DescriptorValidationErrorConfigWTotalLengthTooSmallToHaveInterface,
    DescriptorValidationErrorContainerIdBlengthTooLarge,
    DescriptorValidationErrorContainerIdBlengthTooSmall,
    DescriptorValidationErrorContainerIdBufferTooSmall,
    DescriptorValidationErrorContainerIdNonZeroReservedBits,
    DescriptorValidationErrorControlEndpointBmAttributesNonZeroReservedBits,
    DescriptorValidationErrorControlEndpointInvalidWMaxPacketSize,
    DescriptorValidationErrorDeviceBcdDeviceHasIncorrectFormat,
    DescriptorValidationErrorDeviceBlengthTooSmall,
    DescriptorValidationErrorDeviceBufferTooSmall,
    DescriptorValidationErrorDeviceCapBlengthTooSmall,
    DescriptorValidationErrorDeviceCapBufferTooSmall,
    DescriptorValidationErrorDeviceCapMultipleContainerId,
    DescriptorValidationErrorDeviceCapMultipleSuperSpeedCap,
    DescriptorValidationErrorDeviceCapMultipleUSB20Cap,
    DescriptorValidationErrorDeviceIncorrectType,
    DescriptorValidationErrorDeviceInvalidBMaxPacketSize,
    DescriptorValidationErrorDeviceNullBuffer,
    DescriptorValidationErrorEndpointBlengthTooLarge,
    DescriptorValidationErrorEndpointBlengthTooSmall,
    DescriptorValidationErrorEndpointBmAttributesNonZeroReservedBits,
    DescriptorValidationErrorEndpointBufferTooSmall,
    DescriptorValidationErrorEndpointNonZeroReservedBits,
    DescriptorValidationErrorEndpointWithNoPreceedingInterface,   
    DescriptorValidationErrorEndpointZeroAddress,
    DescriptorValidationErrorHeaderBlengthLargerThanRemainingBuffer,
    DescriptorValidationErrorHeaderBlengthLargerThanWTotalLength,
    DescriptorValidationErrorHeaderBlengthTooSmall,
    DescriptorValidationErrorHeaderBufferTooSmall,
    DescriptorValidationErrorHeaderZeroBlength,
    DescriptorValidationErrorIadBlengthTooLarge,
    DescriptorValidationErrorIadBlengthTooSmall,
    DescriptorValidationErrorIadBufferTooSmall,
    DescriptorValidationErrorIadInterfaceCountTooLargeForTheConfig,
    DescriptorValidationErrorIadInterfaceCountZero,
    DescriptorValidationErrorIadInterfaceNumberTooLarge,
    DescriptorValidationErrorInterfaceBlengthTooLarge,
    DescriptorValidationErrorInterfaceBlengthTooSmall,
    DescriptorValidationErrorInterfaceBufferTooSmall,
    DescriptorValidationErrorInterfaceDuplicateAlternateSetting,
    DescriptorValidationErrorInterfaceDuplicateEndpointAddress,
    DescriptorValidationErrorInterfaceDuplicateInterfaceNumber,
    DescriptorValidationErrorInterfaceFirstDescriptorWithNonZeroAlternateSetting,
    DescriptorValidationErrorInterfaceOutOfSequenceAlternateSetting,
    DescriptorValidationErrorInterfaceOutOfSequenceInterfaceNumber,
    DescriptorValidationErrorInterfaceParsedEndpointCountTooSmall,
    DescriptorValidationErrorInterruptEndpointBmAttributesNonZeroReservedBits,
    DescriptorValidationErrorInterruptEndpointInvalidBInterval,
    DescriptorValidationErrorInterruptEndpointInvalidWMaxPacketSize,
    DescriptorValidationErrorIsochEndpointAtLowSpeed,
    DescriptorValidationErrorIsochEndpointInvalidBInterval,
    DescriptorValidationErrorIsochEndpointInvalidWMaxPacketSize,
    DescriptorValidationErrorMSOSContainerIdAllZeroes,
    DescriptorValidationErrorMSOSContainerIdInvalidBcdVersion,
    DescriptorValidationErrorMSOSContainerIdInvalidBcdWIndex,
    DescriptorValidationErrorMSOSContainerIdInvalidBufferSize,
    DescriptorValidationErrorMSOSContainerIdInvalidDwLength,
    DescriptorValidationErrorMSOSExtendedConfigBFirstInterfaceNumberTooLarge,
    DescriptorValidationErrorMSOSExtendedConfigInvalidBcdVersion,
    DescriptorValidationErrorMSOSExtendedConfigInvalidBCount,
    DescriptorValidationErrorMSOSExtendedConfigInvalidBufferSize,
    DescriptorValidationErrorMSOSExtendedConfigInvalidCompatibleId,
    DescriptorValidationErrorMSOSExtendedConfigInvalidSubCompatibleId,
    DescriptorValidationErrorMSOSExtendedConfigInvalidWIndex,
    DescriptorValidationErrorMSOSExtendedConfigMismatchBetweenBufferSizeAndDwLength,
    DescriptorValidationErrorMSOSExtendedConfigMismatchBetweenDwLengthAndBCount,
    DescriptorValidationErrorStringBlengthNotEven,
    DescriptorValidationErrorStringBlengthTooLarge,
    DescriptorValidationErrorStringBlengthTooSmall,
    DescriptorValidationErrorStringBufferTooSmall,
    DescriptorValidationErrorStringInvalidCharacter,
    DescriptorValidationErrorStringInvalidType,
    DescriptorValidationErrorStringMismatchBetweenBlengthAndBufferLength,
    DescriptorValidationErrorSuperSpeedCapBlengthTooLarge, 
    DescriptorValidationErrorSuperSpeedCapBlengthTooSmall, 
    DescriptorValidationErrorSuperSpeedCapBmAttributesNonZeroReservedBits, 
    DescriptorValidationErrorSuperSpeedCapBU1DevExitLatTooLarge, 
    DescriptorValidationErrorSuperSpeedCapBU2DevExitLatTooLarge, 
    DescriptorValidationErrorSuperSpeedCapBufferTooSmall, 
    DescriptorValidationErrorSuperSpeedCapIncompatibleWSpeedsSupportedAndBFunctionalitySupported, 
    DescriptorValidationErrorSuperSpeedCapReservedValueInBFunctionalitySupported, 
    DescriptorValidationErrorSuperSpeedCapReservedValueInWSpeedsSupported, 
    DescriptorValidationErrorSuperSpeedCapZeroWSpeedsSupported, 
    DescriptorValidationErrorUSB20CapBlengthTooLarge, 
    DescriptorValidationErrorUSB20CapBlengthTooSmall, 
    DescriptorValidationErrorUSB20CapBmAttributesNonZeroReservedBits, 
    DescriptorValidationErrorUSB20CapBufferTooSmall, 
    DescriptorValidationErrorUSB20CapIncompatibleBaselineBESLValidAndBaselineBESL, 
    DescriptorValidationErrorUSB20CapIncompatibleBESLAndAlternateHIRDSupportedAndLPMCapable, 
    DescriptorValidationErrorUSB20CapIncompatibleDeepBESLAndBaselineBESL, 
    DescriptorValidationErrorUSB20CapIncompatibleDeepBESLValidAndDeepBESL, 
    DescriptorValidationErrorContainerIdAllZeros,
    DescriptorValidationErrorMsOs20MultipleDescriptorSetHeaders,
    DescriptorValidationErrorMsOs20DescriptorSetHeaderInvalidwLength,
    DescriptorValidationErrorMsOs20ConfigurationSubsetHeaderInvalidwLength,
    DescriptorValidationErrorMsOs20ConfigurationSubsetHeaderTooSmallwTotalLength,
    DescriptorValidationErrorMsOs20ConfigurationSubsetHeaderTooLargewTotalLength,
    DescriptorValidationErrorMsOs20ConfigurationSubsetHeaderInvalidFeatureDescriptor,
    DescriptorValidationErrorMsOs20ConfigurationSubsetHeaderInvalidFeatureDescriptorLength,
    DescriptorValidationErrorMsOs20FunctionSubsetHeaderInvalidwLength,
    DescriptorValidationErrorMsOs20FunctionSubsetHeaderTooSmallwTotalLength,
    DescriptorValidationErrorMsOs20FunctionSubsetHeaderTooLargewTotalLength,
    DescriptorValidationErrorMsOs20FunctionSubsetHeaderInvalidFeatureDescriptor,
    DescriptorValidationErrorMsOs20FunctionSubsetHeaderInvalidFeatureDescriptorLength,
    DescriptorValidationErrorMsOs20ComatibleIdDescriptorMultipleDescriptors,
    DescriptorValidationErrorMsOs20ComatibleIdDescriptorInvalidwLength,
    DescriptorValidationErrorMsOs20ComatibleIdDescriptorNonNullAfterNullTermination,
    DescriptorValidationErrorMsOs20RegistryValueDescriptorTooSmallwLength,
    DescriptorValidationErrorMsOs20RegistryValueDescriptorTooLargewPropertyNameLength,
    DescriptorValidationErrorMsOs20RegistryValueDescriptorOddOrZerowPropertyNameLength,
    DescriptorValidationErrorMsOs20RegistryValueDescriptorTooLargewPropertyDataLength,
    DescriptorValidationErrorMsOs20RegistryValueDescriptorZerowPropertyDataLength,
    DescriptorValidationErrorMsOs20RegistryValueDescriptorInvalidwPropertyDataType,
    DescriptorValidationErrorMsOs20MinumumResumeTimeDescriptorMultipleDescriptors,
    DescriptorValidationErrorMsOs20MinumumResumeTimeDescriptorInvalidwLength,
    DescriptorValidationErrorMsOs20MinumumResumeTimeDescriptorInvalidbResumeRecoveryTime,
    DescriptorValidationErrorMsOs20MinumumResumeTimeDescriptorInvalidbResumeSignalingTimeValue,
    DescriptorValidationErrorMsOs20RecoveryTimeDescriptorMultipleDescriptors,
    DescriptorValidationErrorMsOs20RecoveryTimeInvalidwLength,
    DescriptorValidationErrorMsOs20ModelIdDescriptorMultipleDescriptors,
    DescriptorValidationErrorMsOs20ModelIdDescriptorInvalidwLength,
    DescriptorValidationErrorMsOs20ModelIdDescriptorNullUuid,
    DescriptorValidationErrorMsOs20AlternateDeviceDescriptorMultipleInstances,
    DescriptorValidationErrorMsOs20AlternateDeviceDescriptorInvalidwLength,
    DescriptorValidationErrorMSOS20AlternateBosDescriptorMultipleInstances,
    DescriptorValidationErrorMSOS20AlternateBosDescriptorwLengthTooSmall,
    DescriptorValidationErrorMSOS20DescriptorSetBytesReturnNotEqualToDescriptorSetwLength,
    DescriptorValidationErrorMSOS20DescriptorSetBytesReturnTooSmall,
    DescriptorValidationErrorMSOS20DescriptorSetHeaderInvalidwLength,
    DescriptorValidationErrorMSOS20DescriptorSetwTotalLengthMismatch,
    DescriptorValidationErrorMSOS20DescriptordwWindowsVersionMismatch,
    DescriptorValidationErrorMSOS20DescriptorOutOfOrderTopLevelFeatureDescriptor,
    DescriptorValidationErrorMSOS20DescriptorRunsPastEndOfDescriptorSet,
    DescriptorValidationErrorPlatformbReservedNonZero,
    DescriptorValidationErrorPlatformIdAllZeros,
    DescriptorValidationErrorMsOS20PlatformDescriptorMultipleDescriptors,
    DescriptorValidationErrorMsOS20PlatformDescriptorInvalidbLength,
    DescriptorValidationErrorMsOS20PlatformDescriptorDuplicateWindowsVersion,
    DescriptorValidationErrorMsOS20PlatformDescriptorNoWindowsVersionMatchFound,
    DescriptorValidationErrorMsOs20CcgpDeviceMultipleDescriptors,
    DescriptorValidationErrorMsOs20CcgpDeviceDescriptorInvalidwLength,
    DescriptorValidationErrorDeviceCapMultipleSuperSpeedPlusCap,
    DescriptorValidationErrorCompanionIsochEndpointWBytesPerIntervalNotOne,
    DescriptorValidationErrorSSPCompanionEndpointAtNonSuperSpeed,
    DescriptorValidationErrorSSPCompanionEndpointAtUnexpectedLocation,
    DescriptorValidationErrorSSPCompanionEndpointBlengthTooLarge,
    DescriptorValidationErrorSSPCompanionEndpointBlengthTooSmall,
    DescriptorValidationErrorSSPCompanionEndpointBufferTooSmall,
    DescriptorValidationErrorSSPCompanionIoschEndpointNonZeroReservedBits,
    DescriptorValidationErrorSSPCompanionIsochEndpointDWBytesPerIntervalTooSmallLarge,
    DescriptorValidationErrorConfigMissingSSPCompanionEndpoint,
    DescriptorValidationErrorSuperSpeedPlusCapBlengthInvalid,
    DescriptorValidationErrorSuperSpeedPlusCapBlengthTooSmall,
    DescriptorValidationErrorSuperSpeedPlusCapBmAttributesNonZeroReservedBits,
    DescriptorValidationErrorSuperSpeedPlusCapWFunctionalitySupportNonZeroReservedBits,
    DescriptorValidationErrorSuperSpeedPlusCapBufferTooSmall,
    DescriptorValidationErrorSuperSpeedPlusCapNonZeroBReserved,
    DescriptorValidationErrorSuperSpeedPlusCapNonZeroWReserved,
    DescriptorValidationErrorSuperSpeedPlusCapAttrRx,
    DescriptorValidationErrorSuperSpeedPlusCapAttrTx,
    DescriptorValidationErrorSuperSpeedPlusCapAttrTxIncomplete,
    DescriptorValidationErrorSuperSpeedPlusCapAttrPairInvalid,
    DescriptorValidationErrorSuperSpeedPlusCapAttrSymmetricPairInvalid,
    DescriptorValidationErrorSuperSpeedPlusCapAttrMinimumSymmetricPairInvalid,
    DescriptorValidationErrorSuperSpeedPlusCapAttrSpeedIDDuplication,
    DescriptorValidationErrorSuperSpeedPlusCapAttrSpeedIDCount,
    DescriptorValidationErrorSuperSpeedPlusCapAttrMinimumNotFound,
    DescriptorValidationErrorPDBlengthTooLarge,
    DescriptorValidationErrorPDBlengthTooSmall,
    DescriptorValidationErrorPDBufferTooSmall,
    DescriptorValidationErrorPDNonZeroReservedBits,
    DescriptorValidationErrorAltSetting0IsochEndpointNonZeroMaxPacketSize,
    DescriptorValidationErrorDuplicateSerialNumber,
    DescriptorValidationErrorBillboardCapBlengthTooSmall,
    DescriptorValidationErrorBillboardCapBlengthInvalid,
    DescriptorValidationErrorBillboardCapBufferTooSmall,
    DescriptorValidationErrorBillboardCapZeroAlternateModes,
    DescriptorValidationErrorBillboardCapTooManyAlternateModes,
    DescriptorValidationErrorBillboardCapPreferredAlternateModeZero,
    DescriptorValidationErrorBillboardCapPreferredAlternateModeInvalid,
    DescriptorValidationErrorBillboardCapNonZeroBReserved,
    DescriptorValidationErrorBillboardCapReservedVconnPowerValueUsed,
    DescriptorValidationErrorBillboardCapNonZeroVConnPowerReserved,
    DescriptorValidationErrorDeviceCapMultipleBillboardCap,
    DescriptorValidationErrorBillboardDeviceIncorrectBcdUsb,
    DescriptorValidationErrorUsbFeaturesPlatformDescriptorInvalidbLength,
    DescriptorValidationErrorUsbFeaturesPlatformDescriptorMultipleDescriptors,
    DescriptorValidationErrorUsbFeaturesPlatformDescriptorInvalidVersionNumber,
    DescriptorValidationErrorUsbFeaturesPlatformDescriptorbLengthTooSmall,
    DescriptorValidationErrorPlatformBlengthTooSmall,
    DescriptorValidationErrorPlatformBlengthTooLarge,
    DescriptorValidationErrorPlatformBufferTooSmall,
    DescriptorValidationErrorLast
} DESCRIPTOR_VALIDATION_ERROR, *PDESCRIPTOR_VALIDATION_ERROR;       

C_ASSERT(DescriptorValidationErrorLast < (DESCRIPTOR_VALIDATION_ARRAY_COUNT * 32));

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
(LOG_DESCRIPTOR_VALIDATION_ERROR_FUNCTION)(
    __in
        PVOID                           DeviceContext,
    __in
        DESCRIPTOR_VALIDATION_ERROR     Error);

typedef LOG_DESCRIPTOR_VALIDATION_ERROR_FUNCTION* PLOG_DESCRIPTOR_VALIDATION_ERROR_FUNCTION;      

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
(LOG_DESCRIPTOR_VALIDATION_WARNING_FUNCTION)(
    __in
        PVOID                           DeviceContext,
    __in
        DESCRIPTOR_VALIDATION_ERROR     Error);

typedef LOG_DESCRIPTOR_VALIDATION_WARNING_FUNCTION* PLOG_DESCRIPTOR_VALIDATION_WARNING_FUNCTION;      


typedef struct _VALIDATION_DEVICE_INFO {

    USHORT                  USBVersion;
    USB_DEVICE_SPEED        USBSpeed;
    MAX_PORT_POWER          MaxPortPower;
    BOOLEAN                 EnforceStrictestParsing;
    BOOLEAN                 EnableWin7StricterParsing;
    BOOLEAN                 RelaxParsing;  
    BOOLEAN                 ValidateReservedFields;
    ULONG                   SuperSpeedPlusIsochBurstCount;

    PLOG_DESCRIPTOR_VALIDATION_ERROR_FUNCTION
                            LogErrorFunction;
    PLOG_DESCRIPTOR_VALIDATION_WARNING_FUNCTION
                            LogWarningFunction;
    PVOID                   LogFunctionContext;

    BOOLEAN                 UseWin8DescriptorValidationBehavior;
} VALIDATION_DEVICE_INFO, *PVALIDATION_DEVICE_INFO;


LOG_DESCRIPTOR_VALIDATION_ERROR_FUNCTION HUBMISC_LogDescriptorValidationErrorForDevice; 
LOG_DESCRIPTOR_VALIDATION_WARNING_FUNCTION HUBMISC_LogDescriptorValidationWarningForDevice; 
LOG_DESCRIPTOR_VALIDATION_ERROR_FUNCTION HUBMISC_LogDescriptorValidationErrorForHub;
LOG_DESCRIPTOR_VALIDATION_WARNING_FUNCTION HUBMISC_LogDescriptorValidationWarningForHub; 

typedef struct _BOS_DESC_SET_INFO {
    PUSB_DEVICE_CAPABILITY_USB20_EXTENSION_DESCRIPTOR       USB20CapabilityDescriptor;
    PUSB_DEVICE_CAPABILITY_SUPERSPEED_USB_DESCRIPTOR        USB30CapabilityDescriptor;
    PUSB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_USB_DESCRIPTOR    USB31CapabilityDescriptor;
    PUSB_DEVICE_CAPABILITY_CONTAINER_ID_DESCRIPTOR          ContainerIDDescriptor;
    PUSB_DEVICE_CAPABILITY_BILLBOARD_DESCRIPTOR             BillboardDescriptor;
    BOOLEAN                                                 DiscardContainerID;
    BOOLEAN                                                 ChargingPolicySupported;
    PMSOS20_DESCRIPTOR_SET_INFO                             SelectedDescriptorSetInfo;
    PUSB_PLATFORM_FEATURES_DESCRIPTOR_V1                    UsbDualRoleFeatures;
} BOS_DESC_SET_INFO, *PBOS_DESC_SET_INFO;

typedef union _CONFIG_VALIDATION_FLAGS {
    ULONG       AsUlong;

    struct {
        ULONG   FirstInterfaceFound:1;
        ULONG   FirstEndpointFound:1;
        ULONG   ExpectingEndpointCompanion:1;
        ULONG   ExpectingSSPEndpointCompanion:1;
        ULONG   EndpointCompanionFound:1;
        ULONG   SSPEndpointCompanionFound:1;
    };

} CONFIG_VALIDATION_FLAGS, *PCONFIG_VALIDATION_FLAGS;

typedef struct _INTERNAL_VALIDATION_DATA {
    VALIDATION_DEVICE_INFO  DevInfo;
    PUCHAR                  DescriptorSetBeginning;
    PUCHAR                  DescriptorSetEnd;
    ULONG                   BufferLength;
} INTERNAL_VALIDATION_DATA, *PINTERNAL_VALIDATION_DATA;

typedef struct _INTERNAL_CONFIG_VALIDATION_DATA {

    INTERNAL_VALIDATION_DATA;         
    UCHAR                       CurrentInterfaceNumber;
    UCHAR                       CurrentAltSetting;
    RTL_BITMAP                  AlternateSettingsBitmap;
    ULONG                       AlternateSettings[8];
    RTL_BITMAP                  EndpointAddressesBitmap;
    ULONG                       EndpointAddresses[8];
    UCHAR                       NumberEndpoints;
    UCHAR                       EndpointCount;
    PUSB_ENDPOINT_DESCRIPTOR    LastEndpointDescriptor;
    BOOLEAN                     EndpointCompanionFound;
    USHORT                      InterfacesFound;
    RTL_BITMAP                  InterfaceNumberBitmap;
    ULONG                       InterfaceNumbers[8];
    CONFIG_VALIDATION_FLAGS ValidationFlags;

} INTERNAL_CONFIG_VALIDATION_DATA, *PINTERNAL_CONFIG_VALIDATION_DATA;

typedef struct _INTERNAL_BOS_VALIDATION_DATA {
    INTERNAL_VALIDATION_DATA;
    PBOS_DESC_SET_INFO          BOSInfo;
} INTERNAL_BOS_VALIDATION_DATA, *PINTERNAL_BOS_VALIDATION_DATA;       

//
// Define these MACROs to be NULL if the descriptor validation 
// code needs to be compiled for user mode 
//
#define LOG_DESCRIPTOR_VALIDATION_ERROR( _DeviceInfo, _Error ) \
    (_DeviceInfo)->LogErrorFunction((_DeviceInfo)->LogFunctionContext, \
                                    _Error);

#define LOG_DESCRIPTOR_VALIDATION_WARNING( _DeviceInfo, _Error ) \
    (_DeviceInfo)->LogWarningFunction((_DeviceInfo)->LogFunctionContext, \
                                       _Error);

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
FORCEINLINE
HUBDESC_InitializeDeviceInfo (
    __in
        PDEVICE_CONTEXT         DeviceContext,
    __out
        PVALIDATION_DEVICE_INFO DevInfo
    )
{
    PHUB_GLOBALS            hubGlobals; 

    hubGlobals = GetHubGlobals(WdfGetDriver());

    RtlZeroMemory(DevInfo,
                  sizeof(VALIDATION_DEVICE_INFO));
    DevInfo->USBVersion = DeviceContext->DeviceDescriptor.bcdUSB;
    DevInfo->USBSpeed = DeviceContext->UsbDeviceInfo.DeviceSpeed;

    if (hubGlobals->HubGlobalFlags.EnableExtendedValidation) {
        DevInfo->EnableWin7StricterParsing = TRUE;
    }
    if (hubGlobals->HubGlobalFlags.EnableReservedValidation) {
        DevInfo->ValidateReservedFields = TRUE;
    }
    if (hubGlobals->HubGlobalFlags.EnableStrictestValidation) {
        DevInfo->EnforceStrictestParsing = TRUE;
    }
    if (DeviceContext->DeviceHackFlags.UseWin8DescriptorValidation == 1) {
        DevInfo->UseWin8DescriptorValidationBehavior = TRUE;
    }
    DevInfo->SuperSpeedPlusIsochBurstCount = DeviceContext->PortContext->PortInformation.SuperSpeedPlusIsochBurstCount;
    DevInfo->LogFunctionContext = DeviceContext;
    DevInfo->LogErrorFunction = HUBMISC_LogDescriptorValidationErrorForDevice;
    DevInfo->LogWarningFunction = HUBMISC_LogDescriptorValidationWarningForDevice;
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
    );

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
    );

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
FORCEINLINE
HUBDESC_ValidateCommonDescriptorHeader(
    __in
        PUSB_COMMON_DESCRIPTOR          CommonDesc,
    __in
        ULONG                           BufferLength,
    __in
        PVALIDATION_DEVICE_INFO         DevInfo,
    __in
        RECORDER_LOG                    IfrLog

    )
{
    INTERNAL_VALIDATION_DATA    data;
    ULONG                       length;

    data.DevInfo = *DevInfo;
    data.DescriptorSetBeginning  = (PUCHAR)CommonDesc;
    data.DescriptorSetEnd = data.DescriptorSetBeginning + BufferLength;
    data.BufferLength = BufferLength;
    
    return HUBDESC_InternalValidateCommonDescriptorHeader(CommonDesc,
                                                          &data,
                                                          &length,
                                                          NULL,
                                                          IfrLog);
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
    );



__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
FORCEINLINE
HUBDESC_ValidateConfigDescriptor(
    __in
        PUSB_CONFIGURATION_DESCRIPTOR   ConfigDesc,
    __in
        ULONG                           BufferLength,
    __in
        PVALIDATION_DEVICE_INFO         DevInfo,
    __in
        RECORDER_LOG                    IfrLog
    )
{
    INTERNAL_CONFIG_VALIDATION_DATA     cfgData;
    ULONG                               length;

    HUBDESC_InitializeConfigValidationData(ConfigDesc,
                                           BufferLength,
                                           DevInfo,
                                           &cfgData);
    return HUBDESC_InternalValidateConfigDescriptor(ConfigDesc,
                                                    &cfgData,
                                                    &length,
                                                    NULL,
                                                    IfrLog);
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
    );

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
HUBDESC_InternalValidateLastInterface (
    __inout
        PINTERNAL_CONFIG_VALIDATION_DATA    ConfigValidationData,
    __inout_opt
        PVALIDATION_RESULT                  ValidationResult,
    __in
        RECORDER_LOG                        IfrLog
    );

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
    );

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
    );

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
    );

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
    );

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
    );

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
    );

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
    );

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
    );

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
    );


BOOLEAN
FORCEINLINE
HUBDESC_ValidateBOSDescriptor (
    __in
        PUSB_BOS_DESCRIPTOR         BOSDescriptor,
    __in
        ULONG                       BufferLength,
    __in
        PVALIDATION_DEVICE_INFO     DevInfo,
    __in
        RECORDER_LOG                IfrLog
    )
{
    INTERNAL_BOS_VALIDATION_DATA    data;
    BOS_DESC_SET_INFO               bosInfo;
    ULONG                           length;

    data.DevInfo = *DevInfo;
    data.DescriptorSetBeginning  = (PUCHAR)BOSDescriptor;
    data.DescriptorSetEnd = data.DescriptorSetBeginning + BufferLength;
    data.BufferLength = BufferLength;
    
    RtlZeroMemory(&bosInfo, sizeof(BOS_DESC_SET_INFO));

    data.BOSInfo = &bosInfo;

    return HUBDESC_InternalValidateBOSDescriptor(BOSDescriptor,
                                                 &data,
                                                 &length,
                                                 NULL,
                                                 IfrLog);
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
    );


BOOLEAN
HUBDESC_InternalValidateStringDescriptor (
    __in
        PUSB_STRING_DESCRIPTOR  StringDescriptor,
    __in
        ULONG                   DescriptorLength,
    __out
        PULONG                  AdjustedDescriptorLength,
    __inout_opt
        PVALIDATION_RESULT      ValidationResult,
    __in
        RECORDER_LOG            IfrLog,
    __in
        PVALIDATION_DEVICE_INFO DevInfo
    );


BOOLEAN
FORCEINLINE
HUBDESC_ValidateStringDescriptor (
    __in
        PUSB_STRING_DESCRIPTOR  StringDescriptor,
    __in
        ULONG                   BufferLength,
    __in
        RECORDER_LOG            IfrLog,
    __in
        PVALIDATION_DEVICE_INFO DevInfo
    )
{
    ULONG       length;
    return HUBDESC_InternalValidateStringDescriptor(StringDescriptor,
                                                    BufferLength,
                                                    &length,
                                                    NULL,
                                                    IfrLog,
                                                    DevInfo);
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
    );

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
    );

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
    );

BOOLEAN
HUBDESC_ValidateMSOSDescriptor (
    __in
        POS_STRING                  Descriptor,
    __in
        ULONG                       BufferLength,
    __in
        PVALIDATION_DEVICE_INFO     DevInfo
    );

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
    );


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
    );

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
    );

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
    );

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
    );

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
    );

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
    );


BOOLEAN
HUBDESC_GetNextMsOs20Descriptor(
    __in
        PUCHAR EndOfDescriptorSet,
    __inout
        PMSOS20_COMMON_DESCRIPTOR *CurrentDescriptor
    );


BOOLEAN
HUBDESC_ValidateMsOs20DescriptorSet(
    __in
        PVALIDATION_DEVICE_INFO         DevInfo,
    __in
        PMSOS20_INFO                    MsOs20Info
    );
