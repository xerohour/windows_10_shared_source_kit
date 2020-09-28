/*++

Copyright (c) 1995	Microsoft Corporation

Module Name:

    USBDLIBI.H

Abstract:

   Services exported by USBD for use by USB port drivers and
   the usb hub driver.

Environment:

    Kernel & user mode

Revision History:

    01-27-96 : created
    12/10/03: Removed obsolete stub functions

--*/

#ifndef   __USBCOMMON_H__
#define   __USBCOMMON_H__

#include "msosdesc.h"

typedef enum _USBD_CHILD_STATUS {
    USBD_CHILD_STATUS_INVALID = 0,
    USBD_CHILD_STATUS_INSERTED,
    USBD_CHILD_STATUS_DUPLICATE_PENDING_REMOVAL,
    USBD_CHILD_STATUS_DUPLICATE_FOUND,
    USBD_CHILD_STATUS_FAILURE
} USBD_CHILD_STATUS;

#ifndef MAX_USHORT
#define MAX_USHORT ((USHORT)0xFFFF) 
#endif

//
// This MACRO checks if a particular field in a structure lies within
// the buffer bounds marked by end.
// FIELD_OFFSET returns the offset of the field within a buffer need to
// add the size of the field to be validated to the offset.
//
#define IsFieldInBounds(Start, End, Type, Field) \
    (((PUCHAR)(End) >= (PUCHAR)(Start)) && \
     ((PUCHAR)(End) - (PUCHAR)(Start) <= MAX_USHORT) && \
     (((PUCHAR)(End) - (PUCHAR)(Start)) >= \
      (FIELD_OFFSET(Type, Field) + FIELD_SIZE(Type, Field)) \
     ) ? TRUE : FALSE \
    )


PURB 
USBD_CreateMsOsFeatureRequest(
      USHORT MS_FeatureDescriptorIndex,
      __in_bcount(TransferBufferLength) PVOID TransferBuffer,
      ULONG TransferBufferLength,
      UCHAR InterfaceNumber,
      UCHAR PageNumber,
      UCHAR FeatureDescVendorCode,
      ULONG Tag
      );

NTSTATUS 
USBD_InstallExtPropDescSections (
    __in PDEVICE_OBJECT DeviceObject,
    __in_bcount(BufferLength) PMS_EXT_PROP_DESC MsExtPropDesc,
    ULONG BufferLength
    );


PMS_EXT_CONFIG_DESC_FUNCTION 
USBD_ParseExtendedConfigurationDescriptor(
    __in_bcount(BufferLength) PMS_EXT_CONFIG_DESC ExtendedConfigDescriptor,
    ULONG BufferLength,
    IN USHORT FirstInterface
    );


NTSTATUS 
USBD_ValidateExtendedConfigurationDescriptor(
    __in_bcount(ExtendedDescLength) PMS_EXT_CONFIG_DESC ExtendedConfigDescriptor,
    ULONG ExtendedDescLength,
    __in_bcount(ConfigDescLength) PUSB_CONFIGURATION_DESCRIPTOR ConfigDescriptor,
    ULONG ConfigDescLength,
    ULONG Tag
    );

NTSTATUS 
USBD_ValidateExtendedPropertyDescriptor(
    __in_bcount(Length) PMS_EXT_PROP_DESC PropertyDescriptor,
    IN ULONG Length
    );
/*
*
* USBD_ValidateConfigurationDescriptorInternal
*
* Validates a USB Configuration Descriptor
*
* Parameters:
*
*  ConfigDesc:  Pointer to the entire USB Configuration descriptor returned by the device
*  BufferLength: Known size of buffer pointed to by ConfigDesc (NOT NECESSARILY wTotalLength)
*  Level: Level of checking desired:
*           1: Basic Config Descriptor header check only
*           2: Full pass-through of the config descriptor checking for invalid
*              endpoint addresses, interface numbers, descriptor structures, 
*               interface alternate settings, number of interfaces and bLength
*               fields of all descriptors.
*           3: Additional checking of the number of endpoints in each interface
*              and enforcement of the USB spec descriptor bLength sizes.
*           4:  Additional checking to see if all interface numbers are in 
*               sequential(not necessarily consecutive) order.
*
*   Offset: if the USBD_STATUS returned is not USBD_STATUS_SUCCESS, offet will
*   be set to the address within the ConfigDesc buffer where the failure occured.
*
* Return Value:  One of the USBD_STATUS codes declared in usb.h
*/

USBD_STATUS
USBD_ValidateConfigurationDescriptorInternal(
    __in_bcount(BufferLength) PUSB_CONFIGURATION_DESCRIPTOR ConfigDesc,
    ULONG BufferLength,
    USHORT Level,
    __out PUCHAR *Offset,
    ULONG Tag
    );

USBD_CHILD_STATUS
USBD_AddDeviceToGlobalList (
    __in PVOID ChildInstance,
    __in PVOID HubInstance,
    __in USHORT PortNumber,
    __in PVOID ConnectorId,
    __in USHORT IdVendor,
    __in USHORT IdProduct,
    __in PUSB_ID_STRING SerialNumber
	);


VOID
USBD_MarkDeviceAsDisconnected (
	__in PVOID ChildInstance
	);


VOID
USBD_RemoveDeviceFromGlobalList (
	__in PVOID ChildInstance
	);


__drv_maxIRQL(PASSIVE_LEVEL)
ULONG
USBD_AllocateHubNumber (
    );


__drv_maxIRQL(PASSIVE_LEVEL)
VOID
USBD_ReleaseHubNumber (
    __in ULONG HubNumber
    );



#endif /* __USBCOMMON_H__ */
