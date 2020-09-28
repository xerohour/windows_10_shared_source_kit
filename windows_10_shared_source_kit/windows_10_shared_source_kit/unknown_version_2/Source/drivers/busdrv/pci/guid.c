/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    guid.c

Abstract:

    This module instantiates the guids used by pci that are not part of
    wdmguid.{h|lib}.

    It relies on the DEFINE_GUID macros being outside the #ifdef _FOO_H
    multiple inclusion fix.











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"
#include <initguid.h>
#include "pciintrf.h"
#include "halpnpp.h"

// --------------------------------------------------------------------- Defines


// {69A770DD-1CDB-46c0-91C9-CD2A9B76E061}

DEFINE_GUID(GUID_PCI_TRACELOG_PROVIDER, 
0x69a770dd, 0x1cdb, 0x46c0, 0x91, 0xc9, 0xcd, 0x2a, 0x9b, 0x76, 0xe0, 0x61);

DEFINE_GUID(GUID_PCI_BRIDGE_INTERFACE,
            0x41c45ced,
            0xc210, 0x4084,
            0x89, 0xca, 0xb2, 0x1b, 0x9e, 0x2b, 0x3c, 0xfa);

// {E5C937D0-3553-4d7a-9117-EA4D19C3434D}

DEFINE_GUID(PCI_DSM_UUID,
            0xE5C937D0,
            0x3553, 0x4d7a,
            0x91, 0x17, 0xEA, 0x4D, 0x19, 0xC3, 0x43, 0x4D);

//
// IgnorePciRootBusNumberRange.
//
// {F4E014A2-815B-4be8-9665-40FFF9CDFD5A}
//

DEFINE_GUID(GUID_EM_RULE_IGNORE_PCI_ROOTBUS_NUMBER_RANGE,
            0xf4e014a2,
            0x815b, 0x4be8,
            0x96, 0x65, 0x40, 0xff, 0xf9, 0xcd, 0xfd, 0x5a);

//
// IgnoreOverlappingPciRootBusNumberRanges.
//
// {CE69EE33-0145-439d-A738-73AD320A1253}
//

DEFINE_GUID(GUID_EM_RULE_IGNORE_OVERLAPPING_PCI_ROOTBUS_NUMBER_RANGES,
            0xce69ee33,
            0x145, 0x439d,
            0xa7, 0x38, 0x73, 0xad, 0x32, 0xa, 0x12, 0x53);

//
// IgnoreCBMemLimits.
//
// {50B15253-7D7D-4348-9432-35A7E2485111}
//

DEFINE_GUID(GUID_EM_RULE_IGNORE_CB_MEM_LIMITS,
            0x50b15253,
            0x7d7d, 0x4348,
            0x94, 0x32, 0x35, 0xa7, 0xe2, 0x48, 0x51, 0x11);

//
// AvoidAssertOnBadIdDataForHostBridge.
//
// {4E23CD72-B4DB-4f17-A90E-BE0B97558383}
//

DEFINE_GUID(GUID_EM_RULE_AVOID_ASSERT_ON_BAD_ID_DATA_FOR_HOST_BRIDGE,
            0x4e23cd72,
            0xb4db, 0x4f17,
            0xa9, 0xe, 0xbe, 0xb, 0x97, 0x55, 0x83, 0x83);

//
// IgnoreIsaVgaBitConflict.
//
// {6A77BB4C-5772-41AC-803B-C12A61553976}
//

DEFINE_GUID(GUID_EM_RULE_IGNORE_ISA_VGA_BIT_CONFLICT,
            0x6a77bb4c,
            0x5772,
            0x41ac,
            0x80, 0x3b, 0xc1, 0x2a, 0x61, 0x55, 0x39, 0x76);

//
// {4DFBAE6B-D3E3-4fdc-804A-6F3E8A0F9F2C}
//

DEFINE_GUID(GUID_EM_RULE_DISABLE_PCI_EXPRESS_ASPM,
            0x4dfbae6b,
            0xd3e3, 0x4fdc,
            0x80, 0x4a, 0x6f, 0x3e, 0x8a, 0xf, 0x9f, 0x2c);

//
// This rule disables PCI express ASPM globally on systems
// that have unsupported PCI express hubs.
//
// {6A7D7F12-A114-4d0d-92CA-55A6E925C85D}
//

DEFINE_GUID(GUID_EM_PCI_DEVICE_DISABLE_PCI_EXPRESS_ASPM_RULE,
            0x6a7d7f12,
            0xa114, 0x4d0d,
            0x92, 0xca, 0x55, 0xa6, 0xe9, 0x25, 0xc8, 0x5d);

//
// This rule is for Intel Iov chipsets which require Dma remapping
// root port alternate error handing.
//
// {6780B3EB-0AC1-4E64-8C05-D8E28429A8B3}
//

DEFINE_GUID(GUID_EM_PCI_DEVICE_DMA_REMAPPING_ERROR_HANDLING_RULE,
            0x6780b3eb,
            0x0ac1, 0x4e64,
            0x8c, 0x05, 0xd8, 0xe2, 0x84, 0x29, 0xa8, 0xb3);

//
// This rule is for AMD IOMMU devices that don't remap all interrupts
//
// {B08C8DBB-5AA0-46D3-8A2E-8765FD4738F0}
//
DEFINE_GUID(GUID_EM_PCI_DEVICE_INTERRUPT_REMAPPING_NOT_ALL_REMAPPED_RULE,
            0xb08c8dbb,
            0x5aa0, 0x46d3,
            0x8a, 0x2e, 0x87, 0x65, 0xfd, 0x47, 0x38, 0xf0);

//
// An Erratum rule that applies the appropriate hack flags for a
// list of matching bios that requires them.
//
// {3AE39896-3D96-4568-A806-B6500FABAC4F}
//

DEFINE_GUID(GUID_EM_PCI_DEVICE_HACK_BIOS_MATCH_RULE,
            0x3ae39896,
            0x3d96, 0x4568,
            0xa8, 0x6, 0xb6, 0x50, 0xf, 0xab, 0xac, 0x4f);

//
// An Erratum rule that applies an appropriate device hack flags for a
// matching CPU.
//
// {FB80FFBA-98E8-4e03-8774-A929320F2F17}
//

DEFINE_GUID(GUID_EM_PCI_DEVICE_HACK_CPU_MATCH_RULE,
            0xfb80ffba,
            0x98e8, 0x4e03,
            0x87, 0x74, 0xa9, 0x29, 0x32, 0xf, 0x2f, 0x17);

//
// An Erratum rule that applies the appropriate hackflags for a
// list of PCI devices that requires hackflags.
//
// {11BC7F8A-A266-4829-BD0C-442EE5B18653}
//

DEFINE_GUID(GUID_EM_PCI_DEVICE_HACK_RULE,
            0x11bc7f8a,
            0xa266, 0x4829,
            0xbd, 0xc, 0x44, 0x2e, 0xe5, 0xb1, 0x86, 0x53);

//
// An Erratum rule that applies an alternate delay for returning to D0 from
// a low-power state.  The PCI and PCIe specs stipulate 100ms after the link/bus
// come up before touching device config space.  This rule allows an override.
//
// {4817D19B-EB55-4A72-9A70-28871205F2AF}
//

DEFINE_GUID(GUID_EM_PCI_D0_DELAY_RULE,
            0x4817D19B,
            0xEB55, 0x4A72,
            0x9a, 0x70, 0x28, 0x87, 0x12, 0x05, 0xf2, 0xaf);


//
// An Erratum rule that applies an alternate Maximum Packet Size mask.
// The system determines the minimum MPS value of all PCI devices, and enforces
// that value across the entire PCI hierarchy. Therefore, any one device which
// misreports its MPS value can have a performance impact on all other devices
// in the system. This override allows changing the MPS value for devices which
// misreport their MPS.
//
// {FCD363D0-4488-499C-9AED-2D67318F1976}
//

DEFINE_GUID(GUID_EM_PCI_DEVICE_SET_MPSSIZE_RULE,
            0xfcd363d0,
            0x4488, 0x499c,
            0x9a, 0xed, 0x2d, 0x67, 0x31, 0x8f, 0x19, 0x76);

//
// PCI device entry type for EM.
//
// {8213EB69-7FCA-432f-AC8C-1E5C59B1772E}
//

DEFINE_GUID(GUID_EM_PCI_DEVICE_TYPE,
            0x8213eb69,
            0x7fca, 0x432f,
            0xac, 0x8c, 0x1e, 0x5c, 0x59, 0xb1, 0x77, 0x2e);

//
// PCI device callback function that matches the device with an entry
// in the EM.
//
// {DFBFD6FE-435A-419e-8F2C-9B13A3C04C9E}
//

DEFINE_GUID(GUID_EM_PCI_DEVICE_MATCH_CALLBACK,
            0xdfbfd6fe,
            0x435a, 0x419e,
            0x8f, 0x2c, 0x9b, 0x13, 0xa3, 0xc0, 0x4c, 0x9e);

//
// PCI device callback function that sets the hackflags as a side-effect.
//
// {F79DE8DC-F3D1-4802-9C4B-6BF742D65FBD}
//

DEFINE_GUID(GUID_EM_PCI_DEVICE_SET_HACKFLAGS_CALLBACK,
            0xf79de8dc,
            0xf3d1, 0x4802,
            0x9c, 0x4b, 0x6b, 0xf7, 0x42, 0xd6, 0x5f, 0xbd);

//
// PCI device callback function that sets the power-on delay for a device.
//
// {898a8e39-096c-4a25-87e5-5bb0ed1d6704}
//

DEFINE_GUID(GUID_EM_PCI_DEVICE_SET_D0DELAY_CALLBACK,
            0x898a8e39,
            0x096c, 0x4a25,
            0x87, 0xe5, 0x5b, 0xb0, 0xed, 0x1d, 0x67, 0x04);

//
// PCI device callback that applies an alternate Maximum Packet Size mask.
// The system determines the minimum MPS value of all PCI devices, and enforces
// that value across the entire PCI hierarchy. Therefore, any one device which
// misreports its MPS value can have a performance impact on all other devices
// in the system. This override allows changing the MPS value for devices which
// misreport their MPS.
//
// {B9EB207B-E0C8-4C01-A575-49DD7D510B46}
//

DEFINE_GUID(GUID_EM_PCI_DEVICE_SET_MPSSIZE_CALLBACK,
            0xb9eb207b,
            0xe0c8, 0x4c01,
            0xa5, 0x75, 0x49, 0xdd, 0x7d, 0x51, 0xb, 0x46);

//
// PCI device callback function that checks the existence of Iov nonworking devices.
//
// {1E66F3D7-0FC9-4829-AA45-C430EA96A434}
//
DEFINE_GUID(GUID_EM_PCI_DEVICE_QUERY_RULE_CALLBACK,
            0x1e66f3d7,
            0x0fc9, 0x4829,
            0xaa, 0x45, 0xc4, 0x30, 0xea, 0x96, 0xa4, 0x34);


//
// PCI arbiter initialization hack rule.  A resource range is in use but is not
// reported by the BIOS.
//
// {8F8FFD54-B39C-4b51-96F8-F4B3EC773737}
//

DEFINE_GUID(GUID_EM_PCI_ARBITER_UNREPORTED_MEMORY_AT_F8_RULE,
            0x8f8ffd54,
            0xb39c, 0x4b51,
            0x96, 0xf8, 0xf4, 0xb3, 0xec, 0x77, 0x37, 0x37);

//
// Match the CPU type for AMD processor string.
//
// {9E6DF23B-88B4-4f6a-A802-253325CA8FD5}
//

DEFINE_GUID(GUID_EM_CPU_TYPE_AMD,
            0x9e6df23b,
            0x88b4, 0x4f6a,
            0xa8, 0x2, 0x25, 0x33, 0x25, 0xca, 0x8f, 0xd5);

// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes
// --------------------------------------------------------------------- Pragmas
// --------------------------------------------------------------------- Globals
// ------------------------------------------------------------------- Functions
