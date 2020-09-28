/*++
Copyright (c) Microsoft Corporation

Module Name:

    tags.h

Abstract:

    USB XHCI Driver

--*/

#pragma once

#define SWIZZLE(tag) \
    ((((tag) >> 24) & 0x000000FF) | \
     (((tag) >>  8) & 0x0000FF00) | \
     (((tag) <<  8) & 0x00FF0000) | \
     (((tag) << 24) & 0xFF000000))

//
// These are unique tags that are used throughout the xHCI driver to identify
// the location of a common buffer allocation.
//
#define CONTROLLER_ALLOC_TAG1           SWIZZLE('Ctr1')

#define COMMAND_ALLOC_TAG1              SWIZZLE('Cmd1')

#define DEVICESLOT_ALLOC_TAG1           SWIZZLE('Slt1')
#define DEVICESLOT_ALLOC_TAG2           SWIZZLE('Slt2')
#define DEVICESLOT_ALLOC_TAG3           SWIZZLE('Slt3')

#define ENDPOINT_ALLOC_TAG1             SWIZZLE('Ept1')
#define ENDPOINT_ALLOC_TAG2             SWIZZLE('Ept2')

#define TRANSFERRING_ALLOC_TAG_TRANSFER_SEGMENT         SWIZZLE('Rng1')
#define TRANSFERRING_ALLOC_TAG_DOUBLEBUFFER             SWIZZLE('Rng2')

#define INTERRUPTER_ALLOC_TAG1          SWIZZLE('Int1')
#define INTERRUPTER_ALLOC_TAG2          SWIZZLE('Int2')

#define USBDEVICE_ALLOC_TAG1            SWIZZLE('Dev1')
#define USBDEVICE_ALLOC_TAG2            SWIZZLE('Dev2')
#define USBDEVICE_ALLOC_TAG3            SWIZZLE('Dev3')
#define USBDEVICE_ALLOC_TAG4            SWIZZLE('Dev4')
#define USBDEVICE_ALLOC_TAG5            SWIZZLE('Dev5')

#define CRASHDUMP_ALLOC_TAG1            SWIZZLE('Dmp1')
