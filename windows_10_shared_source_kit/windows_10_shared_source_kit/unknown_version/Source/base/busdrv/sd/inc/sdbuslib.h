/*++

Copyright (c) 2002  Microsoft Corporation

Module Name:

    sdbuslib.h

Abstract:

    This is the include file that defines types shared between SDBUS.LIB
    and SDBUS.SYS.

Author:

    Neil Sandlin

Revision History:

--*/

#ifndef _SDBUSLIBH_
#define _SDBUSLIBH_

typedef
VOID
(*PSDBUS_INTERFACE_DESTRUCTOR) (
    IN PVOID DestructorContext
    );


#define SDBUS_INTERFACE_SIGNATURE 'CIDS'

typedef struct _SDBUS_INTERFACE_CONTEXT {
    //
    // Signature helps to validate this cookie
    //
    ULONG Signature;
    //
    // Target device object for IOCTLs from the bus interface library
    //
    PDEVICE_OBJECT TargetObject;
    //
    // context for the device driver which exposed the sdbus interface
    //
    PVOID DeviceContext;
    //
    // The destructor will be called when the interface reference count
    // goes to zero
    //
    PSDBUS_INTERFACE_DESTRUCTOR Destructor;
    //
    // interface reference count
    //
    LONG ReferenceCount;
} SDBUS_INTERFACE_CONTEXT, *PSDBUS_INTERFACE_CONTEXT;

#define SDBUS_REENUMERATION_INTERFACE_SIGNATURE 'CRDS'

typedef struct _SDBUS_REENUMERATION_INTERFACE_CONTEXT {
    //
    // Signature helps to validate this cookie
    //
    ULONG Signature;
    //
    // context for the device driver which exposed the sdbus interface
    //
    PVOID DeviceContext;
    //
    // interface reference count
    //
    LONG ReferenceCount;
} SDBUS_REENUMERATION_INTERFACE_CONTEXT, *PSDBUS_REENUMERATION_INTERFACE_CONTEXT;

#endif
