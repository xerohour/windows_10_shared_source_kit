ù!ù!ù!ù!ù!ù!ù!ù!ù!ù!ù!ù!ù!ù!ù!ù!ù!ù!ù!ù!ù!û!ù!ù!ù!ù!ù!û!û!û!û!û!û!û!ù!ù!ù!ù!ù!ù!û!û!ü!ü!ü!ü!ü!ü!ü!ü!ü!ü!ü!ü!ü!ü!ü!ü!ü!ü!ü!ü!ü!ü!ü!ü!ü!ü!ü!ü!ü!ü!ü!ü!ü!ü!ü!ü!ü!ü!†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"†"°"°"°"°"°"°"°"°"°"°"¢"¢"¢"¢"¢"£"£"£"£"§"§"§"•"•"•"•#•#¶#¶#¶#ß#ß#ß#ß#©#©#™#™#™#™#™#™#™#™#®#®#®#®#®#®#®#®#®#®#®#®#®#®#®#®#®#®#ß#ß#ß#ß#ß#ß#ß#ß#ß#®#®#®#®#®#®#®#®#®#®#®#®#/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    wake.hpp

Abstract:

    Header file for the scan handlers

Environment:

    Kernel mode

Revision History:

--*/
#pragma once

//==============================================================================================

/*++

General PM

OID_PM_CURRENT_CAPABILITIES         Q
    NDIS_PM_CAPABILITIES
OID_PM_HARDWARE_CAPABILITIES        Q
    NDIS_PM_CAPABILITIES

OID_PM_PARAMETERS                   M
    NDIS_PM_PARAMETERS

Packet pattern wake

OID_PM_ADD_WOL_PATTERN           S
OID_PM_REMOVE_WOL_PATTERN       S
OID_PM_WOL_PATTERN_LIST         Q

    NDIS_PM_PROTOCOL_OFFLOAD

--*/

/*++

Handle  Set and Get OID_DOT11_POWER_MGMT_REQUEST

If we are in AutoPSM, we ignore all these.
If we are not in AutoPSM, we are in MAX_PERF. Just settle with FW to do MAX_PERF in this case.
No need to do anything to firmware for this.
 
-- */
class CSetPmParametersJob : public CSimpleSetOidJob
{
public:
    CSetPmParametersJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleSetOidJob(ActivityId) 
    {
    };

    ~CSetPmParametersJob()
    {
    };

protec