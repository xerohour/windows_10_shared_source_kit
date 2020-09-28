/*++
Copyright (c) Microsoft Corporation

Module Name:

    acpi.h

Abstract:

    Support structures and functions for ACPI

--*/


#pragma once

//
// ACPI method evaluation output size (arbitrarily large to hold any result)
//
#define ACPI_METHOD_OUTPUT_BUFFER_SIZE  1024

typedef struct _ACPI_UPC_PACKAGE {

    //
    // port is physically connectable if the value is non-zero (i.e. 0xFF)
    //
    UCHAR   PortIsConnectable;

    //
    // port connector type (e.g. type-A, mini-A/B, express card, OEM)
    //
    UCHAR   PortConnectorType;

    //
    // reserved fields (must be zero)
    //
    ULONG   Reserved0;
    ULONG   Reserved1;

} ACPI_UPC_PACKAGE, *PACPI_UPC_PACKAGE;


// {CE2EE385-00E6-48CB-9F05-2EDB927C4899}
DEFINE_GUID(GUID_USB_ACPI_DSM,
0xce2ee385, 0x00e6, 0x48cb, 0x9f, 0x05, 0x2e, 0xdb, 0x92, 0x7c, 0x48, 0x99);

#define USB_ACPI_DSM_REVISION_0 0

#define USB_ACPI_DSM_INTERCONNECT_INDEX 2

#define USB_ACPI_DSM_INTERCONNECT_TYPE_STANDARD 0
#define USB_ACPI_DSM_INTERCONNECT_TYPE_HSIC     1
#define USB_ACPI_DSM_INTERCONNECT_TYPE_SSIC     2

#define ACPI_UPC_TYPE_C_USB_20_ONLY 0x08
#define ACPI_UPC_TYPE_C_USB_20_AND_SS_WITH_SWITCH 0x09
#define ACPI_UPC_TYPE_C_USB_20_AND_SS_WITHOUT_SWITCH 0x0A

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
HUBACPI_EvaluateDsmMethod(
    __in
        PHUB_FDO_CONTEXT            HubFdoContext,
    __in
        PANSI_STRING                AcpiDeviceName,
    __in 
        const GUID *                Guid,
    __in
        ULONG                       RevisionID,
    __in 
        ULONG                       FunctionIndex,
    __in
        WDFMEMORY                   AcpiEvalOutputBufferMemory
    );


__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
HUBACPI_EvalAcpiMethodEx (
    __in
        PHUB_FDO_CONTEXT            HubFdoContext,
    __in
        PANSI_STRING                AcpiDeviceName,
    __in    
        ULONG                       AcpiMethodName,
    __in
        WDFMEMORY                   AcpiEvalOutputBufferMemory
    );


__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
HUBACPI_EnumChildren(
    __in
        PHUB_FDO_CONTEXT                    HubFdoContext,
    __in
        WDFMEMORY                           AcpiEnumOutputBufferMemory
    );


__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
HUBACPI_GetAcpiPortAttributes(
    __in
        PHUB_FDO_CONTEXT                HubFdoContext
    );
