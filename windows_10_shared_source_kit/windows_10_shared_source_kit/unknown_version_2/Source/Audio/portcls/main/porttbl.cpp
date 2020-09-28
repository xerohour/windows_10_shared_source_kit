/*****************************************************************************
 * porttbl.c - WDM port class driver port table
 *****************************************************************************
 *
 * Copyright (c) Microsoft.  All rights reserved.
 */

#define PUT_GUIDS_HERE
#include "private.h"



extern NTSTATUS CreateMiniportMidiUart(
    PUNKNOWN *Unknown,
    REFCLSID  ClassID,
    PUNKNOWN  UnknownOuter OPTIONAL,
    _When_((PoolType&NonPagedPoolMustSucceed)!=0,
        __drv_reportError("Must succeed pool allocations are forbidden. "
                          "Allocation failures cause a system crash"))
    POOL_TYPE PoolType);

extern NTSTATUS CreateMiniportDMusUART(
    PUNKNOWN *Unknown,
    REFCLSID  ClassID,
    PUNKNOWN  UnknownOuter OPTIONAL,
    _When_((PoolType & NonPagedPoolMustSucceed) != 0,
       __drv_reportError("Must succeed pool allocations are forbidden. "
                         "Allocation failures cause a system crash"))
    POOL_TYPE PoolType);


// ==============================================================================
// MiniportDrivers
// Structures which map the miniport class ID to the create functions.
// ==============================================================================
PORT_DRIVER
MiniportDriverUart =
{
    &CLSID_MiniportDriverUart,
    CreateMiniportMidiUart
};
PORT_DRIVER
MiniportDriverDMusUART =
{
    &CLSID_MiniportDriverDMusUART,
    CreateMiniportDMusUART
};

extern PORT_DRIVER PortDriverWaveCyclic;
extern PORT_DRIVER PortDriverWavePci;
extern PORT_DRIVER PortDriverWaveRT;
extern PORT_DRIVER PortDriverTopology;
extern PORT_DRIVER PortDriverMidi;
extern PORT_DRIVER PortDriverDMus;


PPORT_DRIVER PortDriverTable[] =
{
    &PortDriverWaveCyclic,
    &PortDriverWavePci,
    &PortDriverWaveRT,
    &PortDriverTopology,
    &PortDriverMidi,
    &PortDriverDMus,
    &MiniportDriverUart,
    &MiniportDriverDMusUART
};

#pragma code_seg("PAGE")

/*****************************************************************************
 * GetClassInfo()
 *****************************************************************************
 * Get information regarding a class.



 */
NTSTATUS
GetClassInfo
(
	IN	REFCLSID            ClassId,
    OUT PFNCREATEINSTANCE * Create
)
{
    PAGED_CODE();

    ASSERT(Create);

    PPORT_DRIVER *  portDriver = PortDriverTable;

    for
    (
        ULONG count = SIZEOF_ARRAY(PortDriverTable);
        count--;
        portDriver++
    )
    {
        if (IsEqualGUIDAligned(ClassId,*(*portDriver)->ClassId))
        {
            *Create = (*portDriver)->Create;
            return STATUS_SUCCESS;
        }
    }

    return STATUS_NOT_FOUND;
}

#pragma code_seg()
