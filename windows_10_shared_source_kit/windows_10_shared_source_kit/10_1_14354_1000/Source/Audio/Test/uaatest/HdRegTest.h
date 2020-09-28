/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/

#ifndef HDREGTEST_H
#define HDREGTEST_H

const int CONTROLLER_RESET_TIMEOUT = 1000;

DWORD TestGlobalCapability ();
DWORD TestVMIN();
DWORD TestVMAJ();
DWORD TestOUTPAY();
DWORD TestINPAY();
DWORD TestGlobalControl ();
DWORD TestWakeEnable();
DWORD TestWakeStatus();
DWORD TestGlobalStatus();
DWORD TestOUTSTRMPAY();
DWORD TestINSTRMPAY();
DWORD TestINTCTL();
DWORD TestINTSTS();
DWORD TestWallClockCounter();
DWORD TestSSYNC();
DWORD TestCORBLBASE ();
DWORD TestCORBUBASE ();
DWORD TestCORBWP();
DWORD TestCORBRP();
DWORD TestCORBCTL ();
DWORD TestCORBSTS ();
DWORD TestCORBSIZE ();
DWORD TestRIRBLBASE ();
DWORD TestRIRBUBASE ();
DWORD TestRIRBWP();
DWORD TestRINTCNT ();
DWORD TestRIRBCTL ();
DWORD TestRIRBSTS ();
DWORD TestRIRBSIZE ();
DWORD TestICOI();
DWORD TestIRII();
DWORD TestICIS();
DWORD TestDPIBLBASE();
DWORD TestDPIBUBASE();

DWORD TestSDnCTL();
DWORD TestSDnSTS();
DWORD TestSDnLPIB();
DWORD TestSDnCBL();
DWORD TestSDnLVI();
DWORD TestSDnFIFO();
DWORD TestSDnFMT();
DWORD TestSDnBDPL();
DWORD TestSDnBDPU();

//DWORD TestUpperBaseAddress ( Hit::CRegister & Reg);
DWORD TestResetState();
DWORD TestController();
DWORD TestWallClockController ();
DWORD TestResetTimeForDMAEngines();

#endif // UAATEST_H
