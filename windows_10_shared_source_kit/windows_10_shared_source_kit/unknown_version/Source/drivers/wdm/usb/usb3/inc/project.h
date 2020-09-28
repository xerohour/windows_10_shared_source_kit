//
//    Copyright (C) Microsoft.  All rights reserved.
//
#pragma once

#include <usb.h>

// 4127 -- Conditional Expression is Constant warning
#define WHILE(constant) \
    __pragma(warning(suppress: 4127)) \
    while(constant) 

#define TRY
#define LEAVE   goto __tryLabel;
#define FINALLY goto __tryLabel; __tryLabel:
#undef __try
#undef __finally

//
// Note: The reason this is a Macro and not an inline function is that we want to be able to 
// pass both ULONGs and Pointers as Params in this call whthout having to cast them.
//
#define Usb3_KeBugCheckEx(usbcode, param2, param3, param4) \
    __pragma(warning(suppress: 28159))\
    KeBugCheckEx(BUGCODE_USB3_DRIVER, usbcode, (ULONG_PTR)param2, (ULONG_PTR)param3, (ULONG_PTR)param4);

#define ARRAY_SIZE( _A_ ) ( sizeof(_A_) / sizeof(_A_[0]) )

FORCEINLINE
VOID
TRANSFER_LIST(
    __out
        PLIST_ENTRY dest_head, 
    __inout
        PLIST_ENTRY source_head
    ) 
{
    if (IsListEmpty(source_head)){
        InitializeListHead(dest_head);
    } else {
        dest_head->Flink = source_head->Flink;
        dest_head->Blink = source_head->Blink;
        dest_head->Flink->Blink = dest_head;
        dest_head->Blink->Flink = dest_head;
        InitializeListHead(source_head);
    }
}  

FORCEINLINE
VOID
TRANSFER_LIST_COUNTED(
    __out
        PLIST_ENTRY dest_head,
    __inout
        PLIST_ENTRY source_head,
    __in
        ULONG       element_count
    )
{
    PLIST_ENTRY first;
    ULONG       i;
    PLIST_ENTRY last;

    first = source_head->Flink;
    last  = source_head;

    for (i = 0; i < element_count; i += 1) {
        last = last->Flink;
    }

    source_head->Flink = last->Flink;
    last->Flink->Blink = source_head;

    dest_head->Flink   = first;
    dest_head->Blink   = last;
    first->Blink       = dest_head;
    last->Flink        = dest_head;
}
  
FORCEINLINE
VOID
APPEND_LIST(
    __inout
        PLIST_ENTRY dest_head, 
    __inout
        PLIST_ENTRY source_head
    ) 
{
    if (!IsListEmpty(source_head)) {
        dest_head->Blink->Flink   = source_head->Flink;
        source_head->Flink->Blink = dest_head->Blink;
        source_head->Blink->Flink = dest_head;
        dest_head->Blink          = source_head->Blink;
        InitializeListHead(source_head);
    }
} 
  
FORCEINLINE
VOID
PREPEND_LIST(
    __inout
        PLIST_ENTRY dest_head, 
    __inout
        PLIST_ENTRY source_head
    ) 
{
    if (!IsListEmpty(source_head)) {
        source_head->Blink->Flink = dest_head->Flink;
        dest_head->Flink->Blink   = source_head->Blink;
        dest_head->Flink          = source_head->Flink;
        source_head->Flink->Blink = dest_head;
        InitializeListHead(source_head);
    }
} 

#define FOR_ALL_IN_LIST(Type, Head, Field, Current)                 \
    for((Current) = CONTAINING_RECORD((Head)->Flink, Type, Field);  \
       (Head) != &(Current)->Field;                                 \
       (Current) = CONTAINING_RECORD((Current)->Field.Flink,        \
                                     Type,                          \
                                     Field)                         \
       )

#define FOR_ALL_IN_LIST_SAFE(Type, Head, Field, Current, Next)          \
    for((Current) = CONTAINING_RECORD((Head)->Flink, Type, Field),      \
            (Next) = CONTAINING_RECORD((Current)->Field.Flink,          \
                                       Type, Field);                    \
       (Head) != &(Current)->Field;                                     \
       (Current) = (Next),                                              \
            (Next) = CONTAINING_RECORD((Current)->Field.Flink,          \
                                     Type, Field)                       \
       )

//
// This macro cannot be defined as an inline function
//
#define TAG(val) ((PVOID)(ULONG_PTR)(val))
#define TAG_ULONG(val) ((ULONG)(ULONG_PTR)(val))

FORCEINLINE
VOID
IoSetCompletionRoutineSmart(
    __in 
        PDEVICE_OBJECT                DeviceObject,
    __in 
        PIRP                          Irp,
    __in 
        PIO_COMPLETION_ROUTINE        CompletionRoutine,
    __in_opt 
        PVOID                         Context,
    __in 
        BOOLEAN                       InvokeOnSuccess,
    __in 
        BOOLEAN                       InvokeOnError,
    __in 
        BOOLEAN                       InvokeOnCancel
    )
{
    if (!NT_SUCCESS(IoSetCompletionRoutineEx(DeviceObject, 
                                             Irp, 
                                             CompletionRoutine, 
                                             Context, 
                                             InvokeOnSuccess, 
                                             InvokeOnError, 
                                             InvokeOnCancel))) {
        IoSetCompletionRoutine(Irp,
                               CompletionRoutine,
                               Context, 
                               InvokeOnSuccess, 
                               InvokeOnError,
                               InvokeOnCancel);
    }
}

FORCEINLINE
VOID
InitializeListEntry(
    __out PLIST_ENTRY ListEntry
    )
/*++

Routine Description:

    Initialize a list entry to NULL.

    - Using this improves catching list manipulation errors
    - This should not be called on a list head
    - Callers may depend on use of NULL value

--*/
{
    ListEntry->Flink = ListEntry->Blink = NULL;
}

FORCEINLINE
BOOLEAN
IsListEntryNULL(
    __in PLIST_ENTRY ListHead
    )
{
    if (ListHead->Flink == NULL && ListHead->Blink == NULL) {
        return TRUE;
    } else {
        return FALSE;
    }
}

FORCEINLINE
ULONG
InterlockedDecrementULONG(
    __in
        PULONG      Operand
    )
/*++

Routine Description:

    This is a wrapper around InterlockedDecrement that performs some validation
    and type casting specifically for ULONG values

Return Value:

    ULONG   - the new value of Operand after the decrement

--*/
{
    LONG    newOperand;

    newOperand = InterlockedDecrement((LONG*) Operand);
    NT_ASSERT(newOperand >= 0);
    return (ULONG) newOperand;

}

FORCEINLINE
ULONG
InterlockedIncrementULONG(
    __in
        PULONG      Operand
    )
/*++

Routine Description:

    This is a wrapper around InterlockedIncrement that performs some validation
    and type casting specifically for ULONG values

Return Value:

    ULONG   - the new value of Operand after the increment

--*/
{
    LONG    newOperand;

    newOperand = InterlockedIncrement((LONG*) Operand);
    NT_ASSERT(newOperand >= 0);
    return (ULONG) newOperand;

}

FORCEINLINE
ULONG
InterlockedAddULONG(
    __in
        PULONG       Addend,
    __in
        ULONG        Value
    )
/*++

Routine Description:

    This is a wrapper around InterlockedExchangeAdd that performs some validation
    and type casting specifically for ULONG values

Return Value:

    ULONG   - the new value of Operand after the increment

--*/
{
    LONG    newOperand;

    newOperand = InterlockedExchangeAdd((LONG*)Addend,
                                        (LONG) Value);
    //
    // newOperand now contains the PREVIOUS value of Addend
    // so we need to manually Do another addition to figure out
    // what we actually updated the value to.
    // 
    newOperand += (LONG) Value;
    
    NT_ASSERT(newOperand >= 0);
    return (ULONG) newOperand;

}

FORCEINLINE
ULONG
InterlockedSubtractULONG(
    __in
        PULONG       Addend,
    __in
        ULONG        Value
    )
/*++

Routine Description:

    This is a wrapper around InterlockedExchangeAdd that performs some validation
    and type casting specifically for ULONG values

Return Value:

    ULONG   - the new value of Operand after the increment

--*/
{
    LONG    newOperand;
    LONG    difference;

    NT_ASSERT(((LONG)Value) >= 0);
    difference = 0 - (LONG)Value;

    newOperand = InterlockedExchangeAdd((LONG*)Addend,
                                        difference);

    //
    // newOperand now contains the PREVIOUS value of Addend
    // so we need to manually Do another subtraction to figure out
    // what we actually updated the value to.
    // 
    newOperand -= (LONG) Value;

    NT_ASSERT(newOperand >= 0);
    return (ULONG) newOperand;

}

FORCEINLINE
NTSTATUS
USBD_STATUS_To_NTSTATUS (
    __in
        USBD_STATUS             UsbdStatus
    )
{
    //
    // [Mimiced from USBPORT_SetUSBDError from usb2.0 stack]
    //
    switch (UsbdStatus) {
    case USBD_STATUS_SUCCESS:
    case USBD_STATUS_PORT_OPERATION_PENDING:
        return STATUS_SUCCESS;
        
    case USBD_STATUS_INSUFFICIENT_RESOURCES:
        return STATUS_INSUFFICIENT_RESOURCES;
        
    case USBD_STATUS_INVALID_URB_FUNCTION:    
    case USBD_STATUS_INVALID_PARAMETER:
    case USBD_STATUS_INVALID_PIPE_HANDLE:
    case USBD_STATUS_BAD_START_FRAME:
        return STATUS_INVALID_PARAMETER;
        
    case USBD_STATUS_NOT_SUPPORTED:
        return STATUS_NOT_SUPPORTED;
    case USBD_STATUS_DEVICE_GONE:   
        //
        // The 2.0 Macro used STATUS_DEVICE_NOT_CONNECTED, and then manaully 
        // changed the value to STAUTS_NO_SUCH_DEVICE. 
        //
        return STATUS_NO_SUCH_DEVICE;
    case USBD_STATUS_CANCELED:
        return STATUS_CANCELLED;
    }

    return STATUS_UNSUCCESSFUL;

}

FORCEINLINE
USBD_STATUS
NTSTATUS_To_USBD_STATUS(
    __in
        LONG NtStatus
    )
/*++

Routine Description:

    map NT status codes to one of the USB status codes
    
    //
    // [Mimiced from USBPORT_Ntstatus_TO_UsbdStatus from usb2.0 stack]
    //

Arguments:

Return Value:


--*/
{
    USBD_STATUS usbdStatus;

    switch (NtStatus) {
        
    case STATUS_SUCCESS:
        usbdStatus = USBD_STATUS_SUCCESS;
        break;

    case STATUS_NO_SUCH_DEVICE:
        usbdStatus = USBD_STATUS_DEVICE_GONE;
        break;

    case STATUS_INSUFFICIENT_RESOURCES: 
        usbdStatus = USBD_STATUS_INSUFFICIENT_RESOURCES;
        break;
        
    case STATUS_NOT_SUPPORTED: 
        usbdStatus = USBD_STATUS_NOT_SUPPORTED;
        break;

    case STATUS_CANCELLED: 
        usbdStatus = USBD_STATUS_CANCELED;
        break;
                
    default:        
        usbdStatus = USBD_STATUS_INVALID_PARAMETER;
    }

    return usbdStatus;
}

#define INTERLOCKED_SET_BIT_FIELD(_Type, _Pointer, _Field)                                                  \
{                                                                                                           \
    _Type _temp;                                                                                            \
    _temp.AsUlong32 = 0;                                                                                    \
    _temp._Field = 1;                                                                                       \
    InterlockedOr((LONG volatile *)&((_Pointer)->AsUlong32), _temp.AsUlong32);                              \
}

#define INTERLOCKED_CLEAR_BIT_FIELD(_Type, _Pointer, _Field)                                                \
{                                                                                                           \
    _Type _temp;                                                                                            \
    _temp.AsUlong32 = ULONG_MAX;                                                                            \
    _temp._Field = 0;                                                                                       \
    InterlockedAnd((LONG volatile *)&((_Pointer)->AsUlong32), _temp.AsUlong32);                             \
}

#define INTERLOCKED_GET_AND_SET_BIT_FIELD(_Type, _Pointer, _Field, _OutPreviousValue)                       \
{                                                                                                           \
    _Type _temp;                                                                                            \
    _temp.AsUlong32 = 0;                                                                                    \
    _temp._Field = 1;                                                                                       \
    _temp.AsUlong32 = (ULONG)InterlockedOr((LONG volatile *)&((_Pointer)->AsUlong32), _temp.AsUlong32);     \
    *(_OutPreviousValue) = _temp._Field;                                                                    \
}

#define INTERLOCKED_GET_AND_TOGGLE_BIT_FIELD(_Type, _Pointer, _Field, _OutPreviousValue)                    \
{                                                                                                           \
    _Type _temp;                                                                                            \
    _temp.AsUlong32 = 0;                                                                                    \
    _temp._Field = 1;                                                                                       \
    _temp.AsUlong32 = (ULONG)InterlockedXor((LONG volatile *)&((_Pointer)->AsUlong32), _temp.AsUlong32);    \
    *(_OutPreviousValue) = _temp._Field;                                                                    \
}
