/*++
Copyright (c) Microsoft Corporation

Module Name:

    crashdumpcommon.h

Abstract:

    USB XHCI Crashdump write module.

--*/

#pragma once

//
// Since this code will run in Crashdump environment, only kernel debug prints
// can be used to get debug messages out.
//
#define CrashdumpLogError(_s_, ...)                                                         \
    DbgPrintEx(DPFLTR_CRASHDUMPXHCI_ID, 1, "XHCIDUMP: " _s_ "\n" , __VA_ARGS__)             \

#define CrashdumpLogWarning(_s_, ...)                                                       \
    DbgPrintEx(DPFLTR_CRASHDUMPXHCI_ID, 2, "XHCIDUMP: " _s_ "\n" , __VA_ARGS__)             \

#define CrashdumpLogInfo(_s_, ...)                                                          \
    DbgPrintEx(DPFLTR_CRASHDUMPXHCI_ID, 3, "XHCIDUMP: " _s_ "\n" , __VA_ARGS__)             \

#define CrashdumpAllocationTag              'CCHX'

#define INIT_SETUP_PACKET(s, brequest, direction, recipient, typ, wvalue, windex, wlength)  \
{                                                                                           \
    (s).bRequest = (brequest);                                                              \
    (s).bmRequestType.Dir = (direction);                                                    \
    (s).bmRequestType.Type = (typ);                                                         \
    (s).bmRequestType.Recipient = (recipient);                                              \
    (s).bmRequestType.Reserved = 0;                                                         \
    (s).wValue.W = (wvalue);                                                                \
    (s).wIndex.W = (windex);                                                                \
    (s).wLength = (wlength);                                                                \
}                                                                                           \

#define TOGGLE(s)                                                                           \
    (s) = ((s) == 0) ? 1 : 0                                                                \

#define TrbTypeMatch(t, m)                                                                  \
    ((t)->TransferEvent.Type == m)                                                          \

#define IsEventDataTrb(t)                                                                   \
    (((t)->TransferEvent.Type == TRB_TYPE_TRANSFER_EVENT) &&                                \
     ((t)->TransferEvent.EventData == 1))                                                   \

#define IsPageAligned(p)                                                                    \
    (((ULONG_PTR)(p) & (ULONG_PTR)(PAGE_SIZE - 1)) == 0)                                    \

#define Is64ByteAligned(p)                                                                  \
    (((ULONG_PTR)(p) & (ULONG_PTR)(64 - 1)) == 0)                                           \

#define PageByteOffset(p)                                                                   \
    ((ULONG)((ULONG_PTR)(p) & (ULONG_PTR)(PAGE_SIZE - 1)))                                  \

#define PageBytesLeft(l)                                                                    \
    (IsPageAligned(l) ? 0 : (ULONG)(PAGE_SIZE - PageByteOffset(l)))                         \

#define EndpointAddressToDCI(e)                                                             \
    ((((e) & (~USB_ENDPOINT_DIRECTION_MASK)) * 2) +                                         \
     (USB_ENDPOINT_DIRECTION_IN(e) ? 1 : 0))                                                \

#define ROUND_TO(x, y)                                                                      \
    (((x) + (y) - 1) & (~((y) - 1)))                                                        \

#define ROUND_TO_64(x)                                                                      \
    ROUND_TO((x), 64)                                                                       \

//
// Worst case number of pages that a buffer of arbitrary size can span.
//
#define BufferSizeToSpanPages(s) ADDRESS_AND_SIZE_TO_SPAN_PAGES(PAGE_SIZE-1, s)

FORCEINLINE
ULONG
Min(
    __in
        ULONG   Number1,
    __in
        ULONG   Number2
    )
{
    return (((Number1) < (Number2)) ? (Number1) : (Number2));
}

typedef struct _URB_PIPE_REQUEST *PURB_PIPE_REQUEST;
typedef struct _URB_BULK_OR_INTERRUPT_TRANSFER *PURB_BULK_OR_INTERRUPT_TRANSFER;
typedef struct _URB_CONTROL_TRANSFER *PURB_CONTROL_TRANSFER;

typedef struct _CRASHDUMP_BUFFER_DATA {

    PHYSICAL_ADDRESS    LogicalAddress;
    PUCHAR              VirtualAddress;
    ULONG               Size;

} CRASHDUMP_BUFFER_DATA, *PCRASHDUMP_BUFFER_DATA;

typedef enum _CRASHDUMP_PORT_TYPE {
    CrashdumpPortTypeUnknown    = 0,
    CrashdumpPortType20         = 20,
    CrashdumpPortType30         = 30
} CRASHDUMP_PORT_TYPE;

