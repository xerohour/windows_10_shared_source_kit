/*++

Copyright (c) 2010  Microsoft Corporation

Module Name:

    usbehcidump.h

Abstract:



Environment:

    Kernel mode

Revision History:

    8-6-10 : created

--*/

#ifndef   __EHCI_DUMP_H__
#define   __EHCI_DUMP_H__

#include "usbboot.h"

#define MAXIMUM_TRANSFER_SIZE       0x10000
#define MINIMUM_TRANSFER_SIZE       0x8000
#define MDL_ARRAY_PFN_COUNT         ((MAXIMUM_TRANSFER_SIZE / PAGE_SIZE) + 2)

#define EHCIDUMP_DESCRIPTOR_MAKE_TYPE_AND_INDEX(d, i) ((USHORT)((USHORT)d<<8 | i))

#define EHCIDUMP_INIT_SETUP_PACKET(s, brequest, \
    direction, recipient, typ, wvalue, windex, wlength) \
    {\
    (s).bRequest = (brequest);\
    (s).bmRequestType.Dir = (direction);\
    (s).bmRequestType.Type = (typ);\
    (s).bmRequestType.Recipient = (recipient);\
    (s).bmRequestType.Reserved = 0;\
    (s).wValue.W = (wvalue);\
    (s).wIndex.W = (windex);\
    (s).wLength = (wlength);\
    }

NTKERNELAPI
VOID
MmMapMemoryDumpMdl (
    __inout PMDLX MemoryDumpMdl
    );

typedef enum _DUMP_ENDPOINT_TYPE{
    DumpControl,
    DumpEndpointTypeCount = DUMP_MAX_ENDPOINT_COUNT
}DUMP_ENDPOINT_TYPE;

NTSTATUS
__stdcall
EHCIDumpInitializeInterface(
    __inout PUSB_DUMP_CONTROLLER_INFO ControllerData
    );

NTSTATUS
__stdcall
EHCIDumpCleanUpInterface(
    __inout PEHCI_CRASHDUMP_CONTEXT DumpContext
    );

NTSTATUS
__stdcall
EHCIDumpControllerInitialize(
    __inout PEHCI_CRASHDUMP_CONTEXT DumpContext
    );

NTSTATUS
__stdcall
EHCIDumpControllerCleanUp(
    __inout PEHCI_CRASHDUMP_CONTEXT DumpContext
    );


NTSTATUS
__stdcall
EHCIDumpUrbSend(
    __inout PEHCI_CRASHDUMP_CONTEXT DumpContext,
    __in PURB TransferURB,
    __in PPHYSICAL_ADDRESS TransferPAArray
    );

NTSTATUS
EHCIDumpControlTransfer(
    __inout PEHCI_CRASHDUMP_CONTEXT DumpContext,
    __in_xcount(8) PUCHAR SetupPacket,
    __inout PHYSICAL_ADDRESS Buffer,
    __inout ULONG BufferLength
    );

NTSTATUS
EHCIDumpResetExternalHubPort(
    __inout PEHCI_CRASHDUMP_CONTEXT DumpContext,
    __in ULONG PortNum
    );

NTSTATUS
EHCIDumpResetRHPort(
    __inout PEHCI_CRASHDUMP_CONTEXT DumpContext,
    __in ULONG PortNum
    );

NTSTATUS
__stdcall
EHCIDumpResetPipe(
    __inout PEHCI_CRASHDUMP_CONTEXT DumpContext,
    __in UCHAR EndpointAddress
    );

NTSTATUS
__stdcall
EHCIDumpResetDevice(
    __inout PEHCI_CRASHDUMP_CONTEXT DumpContext
    );

NTSTATUS
EHCIDumpEnableAsyncSchedule(
    PHC_OPERATIONAL_REGISTER    OperationalRegister
    );

NTSTATUS
EHCIDumpDisableSchedule(
    PHC_OPERATIONAL_REGISTER    OperationalRegister
    );

VOID
EHCIDumpInitializeQH(
    __inout PHC_OPERATIONAL_REGISTER OperationalRegister,
    __inout PEHCI_CRASHDUMP_CONTEXT DumpContext
    );

#endif /* __EHCI_H__ */
