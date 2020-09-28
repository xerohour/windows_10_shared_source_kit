//
//    Copyright (C) Microsoft.  All rights reserved.
//
/*++

Module Name:

    queue.h

Abstract:

    This file contains the queue definitions.

Author:

    Arvind Aiyar March 2011

Environment:

    Kernel-mode Driver Framework

--*/

#ifndef _QUEUE_H_
#define _QUEUE_H_

//
// WDF Queue callback events
//
EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL OnInternalDeviceIoControl;
EVT_WDF_IO_QUEUE_IO_STOP                    OnIoStop;
EVT_WDF_REQUEST_CANCEL                      OnDeviceResetNotificationRequestCancel;



//
// Private Functions
//

PCHAR
_DbgInternalIoctlString(
    _In_ ULONG        IoControlCode
    );


#endif
