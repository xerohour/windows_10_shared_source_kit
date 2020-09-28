/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    pcisym.c

--*/

#include "pcip.h"
#include <wppstruct.h>
#include <wpprecorder.h>
#include <wppifr.h>
#include <wppifrstructures.h>

#define DECLARE_TYPE(Name) Name _DECL_##Name

DECLARE_TYPE(PCI_BRIDGE);
DECLARE_TYPE(PCI_BRIDGE_REQUIREMENTS);
DECLARE_TYPE(PCI_BRIDGE_RESOURCES);
DECLARE_TYPE(PCI_BRIDGE_WINDOW_REQUIREMENT);

DECLARE_TYPE(PCI_BUS);
DECLARE_TYPE(PCI_DEVICE);
DECLARE_TYPE(PCI_DEVICE_REQUIREMENTS);
DECLARE_TYPE(PCI_DEVICE_RESOURCES);

DECLARE_TYPE(PCI_SAVE_RESTORE_DATA);

DECLARE_TYPE(PCI_SEGMENT);

DECLARE_TYPE(WHEA_PCIEXPRESS_ERROR_SECTION);

DECLARE_TYPE(WPP_TRACE_CONTROL_BLOCK);

int cdecl main() {
    return 0;
}
