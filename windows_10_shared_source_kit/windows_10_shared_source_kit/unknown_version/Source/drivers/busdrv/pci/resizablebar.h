/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    resizablebar.h

Abstract:











--*/

#pragma once

#define PCI_RESIZABLE_BAR_COUNT_MAX 6

typedef struct _PCI_RESIZABLE_BAR_INFO {

    //
    // Offset of the resizable bar capability in PCIe config space.
    //

    USHORT Capability;

    //
    // For each BAR, a bit array of the sizes supported, with the minimum
    // being 1MB (bit 0), and each succeeding bit a power of 2:
    //
    // Bit 0 -> 1MB supported
    // Bit 1 -> 2MB supported
    // Bit 2 -> 4MB supported
    // ...
    // Bit 20 -> 512GB supported
    //
    // If the entry for a BAR is 0, resizing is not supported on that bar.
    //

    ULONG SizesSupported[PCI_RESIZABLE_BAR_COUNT_MAX];

    //
    // The following array maps from a BAR to a Resizable BAR control register.
    // The entry is only valid if the equivalent ResizableBarSizesSupported
    // entry is nonzero.
    //

    BYTE ControlRegister[PCI_RESIZABLE_BAR_COUNT_MAX];
} PCI_RESIZABLE_BAR_INFO, *PPCI_RESIZABLE_BAR_INFO;


