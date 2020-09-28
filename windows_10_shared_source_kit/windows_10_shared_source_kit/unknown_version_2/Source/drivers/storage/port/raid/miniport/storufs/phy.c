/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    phy.c

Abstract:

    This file contains all the functions required to set gear speeds, and
    configure the Physical Link of the controller.

Author:

    Erik Schmidt (eriksch) - April 2013

Environment:



Revision History:

--*/

#include "storufs.h"

VOID
UfsPhyGetCapabilities(
    _In_  PUFS_ADAPTER_EXTENSION AdapterExtension
    )
/*++

Routine Description:

    Retrieve the capabilities of the PHY. This includes the number of lanes
    and the highest Gear Mode supported

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

Return Value:

    N/A

--*/
{
    PUFS_MPHY_CONFIG MPhyCap = &AdapterExtension->MPHYCapabilities;
    ULONG errorStatus = UIC_GENERIC_SUCCESS;

    UNREFERENCED_PARAMETER(errorStatus);
       
    //
    // Get the fastest supported HS Gear Mode. This takes into account
    // both the device and controller capabilities.
    //

    errorStatus = UfsSendUicDmeGet(AdapterExtension, 
                                   PA_MaxRxHsGear, 
                                   GEN_SELECTOR_UNIPRO, 
                                   (PULONG)&MPhyCap->HsGear);
    NT_ASSERT(errorStatus == UIC_GENERIC_SUCCESS);

    MPhyCap->HsModeEnabled = (MPhyCap->HsGear > 0);
       
    //
    // Get the supported Series rate. Series B operates at a slightly faster
    // clock than Series A for better performance.
    //

    errorStatus = UfsSendUicDmeGet(AdapterExtension, 
                                   PA_HSSeries, 
                                   GEN_SELECTOR_UNIPRO, 
                                   (PULONG)&MPhyCap->Series);
    NT_ASSERT(errorStatus == UIC_GENERIC_SUCCESS);
        
    //
    // Get the fastest supported PWM Gear Mode. This takes into account
    // both the device and controller capabilities.
    //

    errorStatus = UfsSendUicDmeGet(AdapterExtension, 
                                   PA_MaxRxPWMGear, 
                                   GEN_SELECTOR_UNIPRO, 
                                   (PULONG)&MPhyCap->PwmGear);
    NT_ASSERT(errorStatus == UIC_GENERIC_SUCCESS);
    
    //
    // Get the number of available data lanes
    //

    errorStatus = UfsSendUicDmeGet(AdapterExtension, 
                                   PA_AvailRxDataLanes, 
                                   GEN_SELECTOR_UNIPRO, 
                                   (PULONG)&MPhyCap->RxDataLanes);
    NT_ASSERT(errorStatus == UIC_GENERIC_SUCCESS);
    
    errorStatus = UfsSendUicDmeGet(AdapterExtension, 
                                   PA_AvailTxDataLanes, 
                                   GEN_SELECTOR_UNIPRO, 
                                   (PULONG)&MPhyCap->TxDataLanes);
    NT_ASSERT(errorStatus == UIC_GENERIC_SUCCESS);
}

ULONG
UfsPhySetSpeedMode(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ PUFS_MPHY_CONFIG TargetState
    )
/*++

Routine Description:

    Configure and initialize the High Speed Gears. This is done by sending
    a series of DME_SET commands to configure the Host Controller

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

    TargetState - The targeted speed mode for the speed mode transition

Return Value:

    The UIC Error Status of the DME_SET operations

--*/
{
    PUFS_MPHY_CONFIG MPhyCap = &AdapterExtension->MPHYCapabilities;
    PUFS_MPHY_CONFIG MPhyState = &AdapterExtension->MPHYState;
    PUFS_HCI_REG reg = AdapterExtension->UfsRegisters;
    ULONG errorStatus;
    UFS_HCI_IS is;
    ULONG i;
    UFS_HCI_HCS hcs;
    ULONG pwrMode;
    PUFS_REG_VALUES RegValues = &AdapterExtension->RegValues;

    //
    // Configure the Number of Lanes
    //

    NT_ASSERT(TargetState->RxDataLanes <= MPhyCap->RxDataLanes);
    MPhyState->RxDataLanes = (ULONG)TargetState->RxDataLanes;

    if (RegValues->NumLanes != 0) {
        MPhyState->RxDataLanes = MIN(RegValues->NumLanes,
                                     MPhyState->RxDataLanes);
    }

    errorStatus = UfsSendUicDmeSet(AdapterExtension, 
                                   PA_ActiveRxDataLanes, 
                                   GEN_SELECTOR_UNIPRO, 
                                   MPhyState->RxDataLanes);
    NT_ASSERT(errorStatus == UIC_GENERIC_SUCCESS);

    NT_ASSERT(TargetState->TxDataLanes <= MPhyCap->TxDataLanes);
    MPhyState->TxDataLanes = (ULONG)TargetState->TxDataLanes;

    if (RegValues->NumLanes) {
        MPhyState->TxDataLanes = MIN(RegValues->NumLanes,
                                     MPhyState->TxDataLanes);
    }

    errorStatus = UfsSendUicDmeSet(AdapterExtension, 
                                   PA_ActiveTxDataLanes, 
                                   GEN_SELECTOR_UNIPRO, 
                                   MPhyState->TxDataLanes);
    NT_ASSERT(errorStatus == UIC_GENERIC_SUCCESS);

    if (TargetState->HsModeEnabled) {

        //
        // Configure the desired HS Gear Mode
        //

        MPhyState->HsGear = (ULONG)TargetState->HsGear;

        if (RegValues->HighSpeedGearMode != 0) {
            MPhyState->HsGear = MIN(RegValues->HighSpeedGearMode,
                                    MPhyState->HsGear);
        }

        errorStatus = UfsSendUicDmeSet(AdapterExtension,
                                       PA_RxGear,
                                       GEN_SELECTOR_UNIPRO,
                                       MPhyState->HsGear);
        NT_ASSERT(errorStatus == UIC_GENERIC_SUCCESS);

        errorStatus = UfsSendUicDmeSet(AdapterExtension,
                                       PA_TxGear,
                                       GEN_SELECTOR_UNIPRO,
                                       MPhyState->HsGear);
        NT_ASSERT(errorStatus == UIC_GENERIC_SUCCESS);
    } else if (TargetState->PwmModeEnabled) {

        //
        // Configure the desired PWM Gear Mode
        //

        MPhyState->PwmGear = (ULONG)TargetState->PwmGear;

        errorStatus = UfsSendUicDmeSet(AdapterExtension,
                                       PA_RxGear,
                                       GEN_SELECTOR_UNIPRO,
                                       MPhyState->PwmGear);
        NT_ASSERT(errorStatus == UIC_GENERIC_SUCCESS);

        errorStatus = UfsSendUicDmeSet(AdapterExtension,
                                       PA_TxGear,
                                       GEN_SELECTOR_UNIPRO,
                                       MPhyState->PwmGear);
        NT_ASSERT(errorStatus == UIC_GENERIC_SUCCESS);
    }

    //
    // Set the Termination
    //

    errorStatus = UfsSendUicDmeSet(AdapterExtension, 
                                   PA_RxTermination, 
                                   GEN_SELECTOR_UNIPRO, 
                                   (ULONG)TRUE);
    NT_ASSERT(errorStatus == UIC_GENERIC_SUCCESS);
    
    errorStatus = UfsSendUicDmeSet(AdapterExtension, 
                                   PA_TxTermination, 
                                   GEN_SELECTOR_UNIPRO, 
                                   (ULONG)TRUE);
    NT_ASSERT(errorStatus == UIC_GENERIC_SUCCESS);
    
    //
    // Set the Series (A or B - different frequencies)
    //

    MPhyState->Series = (ULONG)MPhyCap->Series;

    if (RegValues->HSSeries) {
        NT_ASSERT(RegValues->HSSeries <= HSRATE_B);
        MPhyState->Series = RegValues->HSSeries;
    }

    errorStatus = UfsSendUicDmeSet(AdapterExtension, 
                                   PA_HSSeries, 
                                   GEN_SELECTOR_UNIPRO, 
                                   MPhyState->Series);
    NT_ASSERT(errorStatus == UIC_GENERIC_SUCCESS);












































    //
    // Set the Power Mode. Once this command is sent, the controller actively
    // makes the transition to the speed mode we specified previously.
    //

    if (TargetState->HsModeEnabled) {
        pwrMode = FAST_MODE | (FAST_MODE << 4);
    } else if (TargetState->PwmModeEnabled) {
        pwrMode = SLOW_MODE | (SLOW_MODE << 4);
    } else {
        pwrMode = UNCHANGED;
    }

    StorPortWriteRegisterUlong(AdapterExtension,
                               &reg->Runtime.Is.AsUlong,
                               0xFFFFFFFF);

    errorStatus = UfsSendUicDmeSet(AdapterExtension, 
                                   PA_PWRMode, 
                                   GEN_SELECTOR_UNIPRO, 
                                   pwrMode);
    NT_ASSERT(errorStatus == UIC_GENERIC_SUCCESS);

    //
    // Wait for the UIC Power Mode Status Interrupt to fire to signal the speed
    // mode transition completed
    //

    for (i = 0; i < UFS_COMMAND_TIMEOUT; i++) {
        UfsDelay(1);

        is = UfsPollForCompletion(AdapterExtension, NULL);
        if (is.UicPowerModeStatus != 0) {
            break;
        }
    }

    //
    // Ensure that the HCS register indicates there was a speed mode change
    //

    hcs.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                            &reg->Runtime.Hcs.AsUlong);

    NT_ASSERT(hcs.UicPowerModeChangeReqStatus <= 1);
    MPhyState->HsModeEnabled = TRUE;
    MPhyState->PwmModeEnabled = FALSE;








    return errorStatus;
}
