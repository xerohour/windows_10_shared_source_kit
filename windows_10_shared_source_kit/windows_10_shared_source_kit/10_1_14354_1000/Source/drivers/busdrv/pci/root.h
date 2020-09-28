/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    root.h

Abstract:

    This file contains definitions for root.c which must be scanned as part
    of WPP tracing, and thus can't be in the same file as the other WPP
    definitions (pcip.h).









  --*/

typedef enum _PCI_ROOT_PME_EVENTS {
    PciRootPmeEventRootPortAttention = 0,
    PciRootPmeEventSendWaitWake,
    PciRootPmeEventCompleteWaitWake,
    PciRootPmeEventCancelWaitWake,
    PciRootPmeEventSendD0,
    PciRootPmeEventRetireD0,
    PciRootPmeEventQueueWakeStateEvent,
    PciRootPmeEventExitThread,
    PciRootPmeEventMaximum
} PCI_ROOT_PME_EVENTS, *PPCI_ROOT_PME_EVENTS;

// begin_wpp config
// CUSTOM_TYPE(ROOT_PME_EVENTS, ItemEnum(_PCI_ROOT_PME_EVENTS));
// end_wpp

