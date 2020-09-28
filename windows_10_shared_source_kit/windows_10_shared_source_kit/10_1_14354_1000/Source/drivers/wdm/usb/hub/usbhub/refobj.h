//
//    Copyright (C) Microsoft.  All rights reserved.
//
/* generated prototypes from file: refobj.c */


KIRQL
UsbhLock(
    VOID
    );


VOID
UsbhUnlock(
    KIRQL Irql
    );


NTSTATUS
UsbhReferenceListOpen(
    PDEVICE_OBJECT HubFdo
    );


VOID
UsbhReferenceListClose(
    PDEVICE_OBJECT HubFdo
    );


NTSTATUS
UsbhReferenceListAdd(
    __in PDEVICE_OBJECT HubFdo,
    __in __drv_aliasesMem PVOID Object,
    __in ULONG Tag
    );

#if DBG
#endif //DBG

VOID
UsbhReferenceListRemove(
    PDEVICE_OBJECT HubFdo,
    PVOID Object
    );
