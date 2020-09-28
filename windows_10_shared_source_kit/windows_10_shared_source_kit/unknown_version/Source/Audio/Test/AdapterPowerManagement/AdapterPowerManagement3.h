//--------------------------------------------------------------------------
//
//  File: AdapterPowerManagement3.h
//
// Copyright (C) Microsoft. All rights reserved.
//
//  History:
//      02/14/2012    CaWerner       Initial implementation.
//
//--------------------------------------------------------------------------
#pragma once

const static PowerPolicyEvent Verify_S0_D0toD3toD0[] = {
    { PC_ETYPE_PWR_POLICY_D3_EXIT_LATENCY_CHANGE,
      (ULONGLONG)(-1),
      PcExitLatencyFast,
      0,
      (ULONGLONG)(-1),
      (ULONGLONG)(-1)
    },
    { PC_ETYPE_PWR_POLICY_D3_EXIT_LATENCY_CHANGE,
      (ULONGLONG)(-1),
      PcExitLatencyFast,
      1,
      (ULONGLONG)(-1),
      (ULONGLONG)(-1)
    },
    { PC_ETYPE_PWR_POLICY_D3_EXIT_LATENCY_CHANGE,
      (ULONGLONG)(-1),
      PcExitLatencyInstant,
      0,
      (ULONGLONG)(-1),
      (ULONGLONG)(-1)
    },
    { PC_ETYPE_PWR_POLICY_D3_EXIT_LATENCY_CHANGE,
      (ULONGLONG)(-1),
      PcExitLatencyInstant,
      1,
      (ULONGLONG)(-1),
      (ULONGLONG)(-1)
    }
};

const static PowerPolicyEvent Verify_ConnectedStandby_D0toD3toD0[] = {
    { PC_ETYPE_PWR_POLICY_D3_EXIT_LATENCY_CHANGE,
      (ULONGLONG)(-1),
      PcExitLatencyResponsive,
      0,
      (ULONGLONG)(-1),
      (ULONGLONG)(-1)
    },
    { PC_ETYPE_PWR_POLICY_D3_EXIT_LATENCY_CHANGE,
      (ULONGLONG)(-1),
      PcExitLatencyResponsive,
      1,
      (ULONGLONG)(-1),
      (ULONGLONG)(-1)
    },
    { PC_ETYPE_PWR_POLICY_D3_EXIT_LATENCY_CHANGE,
      (ULONGLONG)(-1),
      PcExitLatencyInstant,
      0,
      (ULONGLONG)(-1),
      (ULONGLONG)(-1)
    },
    { PC_ETYPE_PWR_POLICY_D3_EXIT_LATENCY_CHANGE,
      (ULONGLONG)(-1),
      PcExitLatencyInstant,
      1,
      (ULONGLONG)(-1),
      (ULONGLONG)(-1)
    }
};

const static PowerPolicyEvent Verify_EnterExitConnectedStandby_D0[] = {
    { PC_ETYPE_PWR_POLICY_LPE_NOTIFICATION,
      (ULONGLONG)(-1),
      ENTERING_CS,
      0,
      (ULONGLONG)(-1),
      (ULONGLONG)(-1)
    },
    { PC_ETYPE_PWR_POLICY_LPE_NOTIFICATION,
      (ULONGLONG)(-1),
      ENTERING_CS,
      1,
      (ULONGLONG)(-1),
      (ULONGLONG)(-1)
    },
    { PC_ETYPE_PWR_POLICY_LPE_NOTIFICATION,
      (ULONGLONG)(-1),
      EXITING_CS,
      FALSE,
      (ULONGLONG)(-1),
      (ULONGLONG)(-1)
    },
    { PC_ETYPE_PWR_POLICY_LPE_NOTIFICATION,
      (ULONGLONG)(-1),
      EXITING_CS,
      TRUE,
      (ULONGLONG)(-1),
      (ULONGLONG)(-1)
    }
};

const static PowerPolicyEvent Verify_EnterExitConnectedStandby_D3[] = {
    { PC_ETYPE_PWR_POLICY_LPE_NOTIFICATION,
      (ULONGLONG)(-1),
      ENTERING_CS,
      0,
      (ULONGLONG)(-1),
      (ULONGLONG)(-1)
    },
    { PC_ETYPE_PWR_POLICY_D3_EXIT_LATENCY_CHANGE,
      (ULONGLONG)(-1),
      PcExitLatencyResponsive,
      0,
      (ULONGLONG)(-1),
      (ULONGLONG)(-1)
    },
    { PC_ETYPE_PWR_POLICY_D3_EXIT_LATENCY_CHANGE,
      (ULONGLONG)(-1),
      PcExitLatencyResponsive,
      1,
      (ULONGLONG)(-1),
      (ULONGLONG)(-1)
    },
    { PC_ETYPE_PWR_POLICY_LPE_NOTIFICATION,
      (ULONGLONG)(-1),
      ENTERING_CS,
      1,
      (ULONGLONG)(-1),
      (ULONGLONG)(-1)
    },
    { PC_ETYPE_PWR_POLICY_LPE_NOTIFICATION,
      (ULONGLONG)(-1),
      EXITING_CS,
      FALSE,
      (ULONGLONG)(-1),
      (ULONGLONG)(-1)
    },
    { PC_ETYPE_PWR_POLICY_D3_EXIT_LATENCY_CHANGE,
      (ULONGLONG)(-1),
      PcExitLatencyFast,
      FALSE,
      (ULONGLONG)(-1),
      (ULONGLONG)(-1)
    },
    { PC_ETYPE_PWR_POLICY_D3_EXIT_LATENCY_CHANGE,
      (ULONGLONG)(-1),
      PcExitLatencyFast,
      TRUE,
      (ULONGLONG)(-1),
      (ULONGLONG)(-1)
    },
    { PC_ETYPE_PWR_POLICY_LPE_NOTIFICATION,
      (ULONGLONG)(-1),
      EXITING_CS,
      TRUE,
      (ULONGLONG)(-1),
      (ULONGLONG)(-1)
    }
};
