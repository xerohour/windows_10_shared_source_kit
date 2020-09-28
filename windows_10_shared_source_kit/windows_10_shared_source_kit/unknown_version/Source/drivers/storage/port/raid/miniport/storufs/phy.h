/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    phy.h

Abstract:

    This file contains all the structures and definitions related to
    initializing the link layer

Author:

    Erik Schmidt (eriksch) - April 2013

Environment:



Revision History:

--*/

#pragma once

#define MIBAttribute(m,g)                 ((m & 0x0000FFFF) << 16) | (g & 0x0000FFFF)

//
// M-PHY MIB Attributes (Layer 1)
//

#define TX_HSMODE_Capability              0x0001
#define TX_HSGEAR_Capability              0x0002
#define TX_PWMG0_Capability               0x0003
#define TX_PWMGEAR_Capability             0x0004
#define TX_HSRATE_Series                  0x0022
#define TX_LCC_Enable                     0x002C
#define TX_FSM_State                      0x0041
#define RX_HSMODE_Capability              0x0081
#define RX_HSGEAR_Capability              0x0082
#define RX_PWMG0_Capability               0x0083
#define RX_PWMGEAR_Capability             0x0084
#define RX_HSRATE_Series                  0x00A2

//
// Physical Adapter (PA) MIB Attributes (Layer 1.5)
//

#define PA_AvailTxDataLanes               0x1520
#define PA_AvailRxDataLanes               0x1540
#define PA_ActiveTxDataLanes              0x1560
#define PA_TxGear                         0x1568
#define PA_TxTermination                  0x1569
#define PA_HSSeries                       0x156A
#define PA_PWRMode                        0x1571
#define PA_ActiveRxDataLanes              0x1580
#define PA_RxGear                         0x1583
#define PA_RxTermination                  0x1584
#define PA_MaxRxPWMGear                   0x1586
#define PA_MaxRxHsGear                    0x1587
#define PA_RxHSUnterminationCapability    0x15A5
#define PA_PWRModeUserData0               0x15B0
#define PA_PWRModeUserData1               0x15B1
#define PA_PWRModeUserData2               0x15B2

//
// DME MIB Attributes
//

#define DME_Local_FC0ProtectionTimeOutVal 0xD041
#define DME_Local_TC0ReplayTimeOutVal     0xD042
#define DME_Local_AFC0ReqTimeOutVal       0xD043
#define DME_Local_FC1ProtectionTimeOutVal 0xD044
#define DME_Local_TC1ReplayTimeOutVal     0xD045
#define DME_Local_AFC1ReqTimeOutVal       0xD046

//
// GenSelectorIndexes
//

#define GEN_SELECTOR_TX_LANE_0            0x0
#define GEN_SELECTOR_TX_LANE_1            0x1
#define GEN_SELECTOR_TX_LANE_2            0x2
#define GEN_SELECTOR_TX_LANE_3            0x3
#define GEN_SELECTOR_RX_LANE_0            0x4
#define GEN_SELECTOR_RX_LANE_1            0x5
#define GEN_SELECTOR_RX_LANE_2            0x6
#define GEN_SELECTOR_RX_LANE_3            0x7
#define GEN_SELECTOR_UNIPRO               0x0

//
// MIB Attribute Capability Values
//

typedef enum {
    NO_HS = 0,
    HS_G1_ONLY,
    HS_G1_TO_G2,
    HS_G1_TO_G3
} UFS_HSGEAR_CAPABILITY;

typedef enum {
    HSRATE_UNDEFINED = 0,
    HSRATE_A,
    HSRATE_B
} UFS_HSRATE_SERIES;

typedef enum {
    NO_PWM = 0,
    PWM_G1_ONLY,
    PWM_G1_TO_G2,
    PWM_G1_TO_G3,
    PWM_G1_TO_G4,
    PWM_G1_TO_G5,
    PWM_G1_TO_G6,
    PWM_G1_TO_G7
} UFS_PWMGEAR_CAPABILITY;

typedef enum {
    FAST_MODE = 1,
    SLOW_MODE,
    RESERVED1,
    FASTAUTO_MODE,
    SLOWAUTO_MODE,
    RESERVED2,
    UNCHANGED
} UFS_PWRMODE;

//
// Link Layer Capabilities to be cached in AdapterExtension
//

typedef struct _UFS_MPHY_CONFIG {
    BOOLEAN HsModeEnabled;
    UFS_HSGEAR_CAPABILITY HsGear;
    UFS_HSRATE_SERIES Series;
    BOOLEAN PwmModeEnabled;
    UFS_PWMGEAR_CAPABILITY PwmGear;
    ULONG RxDataLanes;
    ULONG TxDataLanes;
} UFS_MPHY_CONFIG, *PUFS_MPHY_CONFIG;

//
// Function Prototypes
//

VOID
UfsPhyGetCapabilities(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    );

ULONG
UfsPhySetSpeedMode(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ PUFS_MPHY_CONFIG TargetState
    );

