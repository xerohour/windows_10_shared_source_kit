/*++

Copyright (c) 2013 Microsoft Corporation

Module Name:

    lookaside.h

Abstract:

    Header file for lookaside.c

Environment:

    kernel mode only

Notes:

Revision History:

    04-05-13 : created

--*/

#pragma once

NTSTATUS
USBPORT_InitLookasideLists();

VOID
USBPORT_FreeLookasideLists();

__drv_allocatesMem(Mem)
_Post_writable_byte_size_(NumberOfBytes)
PVOID
UsbTryAllocPoolFromNonPagedLookaside_Z(
    __in ULONG NumberOfBytes
    );

VOID
UsbFreeToLookaside(
    __in __drv_freesMem(Mem) PVOID Ptr
    );


