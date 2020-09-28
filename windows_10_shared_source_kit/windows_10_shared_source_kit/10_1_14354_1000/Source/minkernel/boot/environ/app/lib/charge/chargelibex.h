//
// Copyright (C) Microsoft. All rights reserved.
//

#pragma once
#include<bootlib.h>

typedef enum
{
    StartChargeType_NormalOperation,    // normal charging for normal boot
    StartChargeType_UserInvoked,        // charge to threshold before doing a special operation like reset
    StartChargeType_DrainBattery,       // DEBUG mode to drain the battery for testing
    StartChargeType_IgnoreFlags,        // DEBUG mode to ignore flags that prevent or extend charging
    StartChargeType_MAXTYPES
} StartChargeType;

NTSTATUS StartCharge (StartChargeType ChargeType);
