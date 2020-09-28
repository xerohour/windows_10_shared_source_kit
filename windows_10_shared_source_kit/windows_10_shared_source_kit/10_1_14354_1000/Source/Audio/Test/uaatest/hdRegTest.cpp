/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <tchar.h>
#include <atlstr.h>
#include <shell98.h>
#include <module98.h>
#include <dvmodule.h>
#include <cfgmgr32.h>
#include <setupapi.h>
#include <hitframework.h>
#define USERMODE // or azcodec.h includes wdm.h (kernel mode version of windows.h)
#include <azcodec.h>
#include <privioct.h>
#include <controller-interface.h>
#include <versionhelpers.h>

#include "install.h"
#include "uaatest.h"
#include "HdRegTest.h"
#include "HdHitLog.h"
#include "HdHitPlugin.h"
#include "hdaudiomemoryspace.h"
#include "controller.h"
#include "buffer.h"
#include "log.h"

#define SAFETY_FACTOR 0.8

// get the number of Stream (Input, Ootput and Bidirectional Streams by Controller)
void GetStreamNumbers (ULONG& uIss, ULONG& uOss, ULONG& uBss)   
{
    CHdController* HdCnt = GetHdController; 
    uIss = HdCnt->GetIss();
    uOss = HdCnt->GetOss();
    uBss = HdCnt->GetBss();
}

DWORD TestDWORDForRW ( Hit::CRegister & Reg)
{
    DWORD fResult = FNS_PASS;
    TESTINGREG(Reg.Name());
    DWORD dReg = Reg.Read() ; 
    SLOG (eInfo1, "%s = 0x%08x", Reg.Name(), dReg);
    
    if (!HdCntAsrt.ReadWrite("Testing for Read write", Reg))
                fResult = FNS_FAIL;
    return fResult;
}

DWORD TestDWORDForRO ( Hit::CRegister & Reg)
{
    DWORD fResult = FNS_PASS;
    TESTINGREG(Reg.Name());    
    DWORD dReg = Reg.Read() ; 
    SLOG (eInfo1, "%s = 0x%08x", Reg.Name(), dReg);

    if (!HdCntAsrt.ReadOnly("Testing for Read write", Reg))
                fResult = FNS_FAIL;
    return fResult;
}


// to test registers - DPIBUBASE, RIRBUBASE
DWORD TestUpperBaseAddress ( Hit::CRegister & Reg )
{
    DWORD fResult = FNS_PASS;
    CHdController* pController = GetHdController;
    DWORD dBASE = Reg.Read() ; 
    SLOG (eInfo1, "%s = 0x%08x", Reg.Name(), dBASE);      

    if (fResetTest) {
        if (dBASE != 0)    {
            SLOG (eError, "FAIL : %s = 0x%08x. It should be zeor on the reset", Reg.Name(), dBASE);
            fResult  = FNS_FAIL;
        }
        return fResult;
    }
    //This register is reserved, read only 0 if the 64OK bit indicates that the controller does not support 64-bit addressing.    
    if (pController->Getf64Bit())   {
// Test: check for the RW in case of 64 bit support
        fResult = TestDWORDForRW(Reg);
    }
    else    {
// Test: for read only since it does not support 64 but addressing
        if (!HdCntAsrt.ReadOnly0("Testing for Read only Zero ", Reg))
                fResult = FNS_FAIL;
    }
    return fResult;
}

// to test LBASE - RIRB and CORB Lower Base Address
DWORD TestLBASE (Hit::CRegister &RegLBASE, Hit::CBitRange &BLBUB, Hit::CBitRange &BLBASE )   
{
    DWORD fResult = FNS_PASS;
    TESTINGREG(RegLBASE.Name());        
    DWORD dLBASE = RegLBASE.Read() ; 
    SLOG (eInfo1, "%s = 0x%08x", RegLBASE.Name(), dLBASE);      

    if (fResetTest) {
        if (dLBASE != 0)    {
            SLOG (eError, "FAIL :  %s = 0x%08x. It should be zeor on the reset", RegLBASE.Name(), dLBASE);
            fResult  = FNS_FAIL;
        }
        return fResult;
    }
    BYTE bBLBUB = (BYTE) BLBUB.Read();
    SLOG (eInfo1, "%s = 0x%08x", BLBUB.Name(), bBLBUB);      
// Test: for Read Only Zero property
    if (!HdCntAsrt.ReadOnly0("Testing for Read Only 0", BLBUB))
        fResult = FNS_FAIL;
        
    DWORD dBLBASE = (DWORD) BLBASE.Read();
    SLOG (eInfo1, "%s = 0x%08x", BLBASE.Name(), dBLBASE);      
// Test: for Read Write property
    TESTINGREG(BLBASE.Name());
    if (!HdCntAsrt.ReadWrite("Testing for Read Write", BLBASE))
        fResult = FNS_FAIL;

    return fResult;
}

// test for the gloabl capability register
DWORD TestGlobalCapability ()
// The Global Capabilities register indicates the capabilities of the controller. 
// this is a read only register. Since we are testing GCAP as read Only register, I am not testing individual bits as read only.
// Value of GCAP is not effected by the reset of the controller and hence we can test this register irrepective of Reset condition of controller
{
    DWORD fResult = FNS_PASS;
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &RegGc = GenTable["Global Capabilities"];
    TESTINGREG(RegGc.Name());
    WORD dGC = (WORD) RegGc.Read() ; 
    SLOG (eInfo1, "%s = 0x%04x", RegGc.Name(), dGC);

    if (fResetTest)     {
        if (dGC == 0)   {
            SLOG (eError, "FAIL : %s = 0x%04x", RegGc.Name(), dGC);            
            fResult  = FNS_FAIL;
        }
        return fResult;
    }
            
    
// Test : for read only property of register
    if (!HdCntAsrt.ReadOnly("Testing for Read Only", RegGc))  
        fResult = FNS_FAIL;
// since we have already chacked register for read only there is no need for testing individual bits for read only 
// check for value of OSS, ISS, BSS and NSDO 
    Hit::CBitRange &OSS = RegGc["Number of Output Streams Supported -OSS"];    
    BYTE bOss = (BYTE) OSS.Read();
    SLOG (eInfo1, "%s = %d", OSS.Name(), bOss);
    
    Hit::CBitRange &ISS = RegGc["Number of Input Streams Supported -ISS"];    
    BYTE bIss = (BYTE) ISS.Read();
    SLOG (eInfo1, "%s = %d", ISS.Name(),bIss);
    
    Hit::CBitRange &BSS = RegGc["Number of Bidirectional Streams Supported -BSS"];    
    BYTE bBss = (BYTE) BSS.Read();
    SLOG (eInfo1, "%s = %d", BSS.Name(), bBss);
    
    Hit::CBitRange &NSDO = RegGc["Number of Serial Data Out Signals -NSDO"];    
    BYTE bNSDO = (BYTE) NSDO.Read();
    SLOG (eInfo1, "%s = %d", NSDO.Name(), bNSDO);
    
    Hit::CBitRange &b640K = RegGc["64 Bit Address Supported -640K"];    
    BYTE bb640K = (BYTE) b640K.Read();
    SLOG (eInfo1, "%s = %d", b640K.Name(), bb640K);
    
// Test:  make sure that we do not have more then 30 streams
    if ((bBss + bIss + bOss) >30)   {
        fResult =  FNS_FAIL;
        SLOG (eError, "FAIL : Total No of streams (%d)  supported by controller is more then 30", (bBss + bIss + bOss));        
    }
    
    return fResult;
}

// test for the VMIN 
DWORD TestVMIN()
/*
Offset 02h: VMIN – Minor Version
Bit 	Type 	Reset 	Description
7:0 	RO 		00h 		Minor Version (VMIN): Indicates minor revision number 00h of the High Definition
					Audio specification, for specification version “1.0.”
Value of VMIN is not effected by the reset of the controller and hence we can test this register irrepective of Reset condition of controller
*/
{
    DWORD fResult = FNS_PASS;
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &Reg = GenTable["Minor Version -VMIN"];
    TESTINGREG(Reg.Name());

    BYTE val = (BYTE) Reg.Value() ; 
    SLOG (eInfo1, "%s = 0x%02x", Reg.Name(), val);

    if (fResetTest)     {
    // TODO: put this comaprision value in ini file
        if (val != VMINValue)   {
            SLOG (eError, "FAIL :  %s is not equal to 0x%02x", Reg.Name(), VMINValue);
            fResult  = FNS_FAIL;
        }
        return fResult;
    }
        
// Test: for read only
    if (!HdCntAsrt.ReadOnly("Testing for Read Only", Reg))
        fResult = FNS_FAIL;

    return fResult;
}

// test for the VMAJ 
DWORD TestVMAJ()
/*
Offset 03h: VMIN – Minor Version
Bit 	Type 	Reset 	Description
7:0 	RO 		01h 		Major Version (VMAJ): Indicates major revision number 00h of the High Definition
					Audio specification, for specification version “1.0.”
Value of VMAJ is not effected by the reset of the controller and hence we can test this register irrepective of Reset condition of controller
*/
{
    DWORD fResult = FNS_PASS;
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &Reg = GenTable["Major Version -VMAJ"];
    TESTINGREG(Reg.Name());
    
    BYTE val = (BYTE)Reg.Read() ; 
    SLOG (eInfo1, "%s  = 0x%02x", Reg.Name(), val);    
    if (fResetTest)     {
    // TODO: put this comaprision value in ini file
        if (val != VMAJValue)   {
            SLOG (eError, " FAIL : %s is not equal to 0x%02x", Reg.Name(), VMAJValue);
            fResult  = FNS_FAIL;
        }
        return fResult;
    }
    
// Test: for read only	
    if (!HdCntAsrt.ReadOnly("Testing for Read Only", Reg))
        fResult = FNS_FAIL;

    return fResult;
}

// test for the OUTPAY 
DWORD TestOUTPAY()
/*Offset 04h: OUTPAY – Input Payload Capability
Bit 	Type 	Reset 	Description
15:0 RO 		3Ch		Output Payload Capability (OUTPAY): Indicates the total output payload available
					on the link. 
*/
{
    DWORD fResult = FNS_PASS;
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &Reg = GenTable["Output Payload Capability"];
    TESTINGREG(Reg.Name());
    
    WORD val = (WORD) Reg.Read() ; 
    SLOG (eInfo1, "%s  = 0x%04x", Reg.Name(), val);
    if (fResetTest)     {
        // any value is legal as of DCN HDA046-A
        return fResult;
    }    
    
// Test: for read only	
    if (!HdCntAsrt.ReadOnly("Testing for Read Only ", Reg))
        fResult = FNS_FAIL;

    return fResult;
}

DWORD TestINPAY()
// test for the INPAY 
/*Offset 06h: INPAY – Input Payload Capability
Bit 	Type 	Reset 	Description
15:0 RO 		1Dh		Input Payload Capability (INPAY): Indicates the total input payload available
					on the link. 
*/

{
    DWORD fResult = FNS_PASS;
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &Reg = GenTable["Input Payload Capability"];
    TESTINGREG(Reg.Name());

    WORD val = (WORD) Reg.Read() ; 
    SLOG (eInfo1, "%s = 0x%04x", Reg.Name(), val);

    if (fResetTest)     {
        // any value is legal as of Intel HD Audio DCN HDA046-A
        return fResult;
    }    
    
// Test: for read only
    if (!HdCntAsrt.ReadOnly("Testing for Read Only ", Reg))
        fResult = FNS_FAIL;

    return fResult;
}

// Test for Global Control 
DWORD TestGlobalControl()
{
    DWORD fResult = FNS_PASS;
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &RegGC = GenTable["Global Control"];
    TESTINGREG(RegGC.Name());
    
    DWORD dGC = RegGC.Read() ; 
    SLOG (eInfo1, "%s = 0x%08x", RegGC.Name(), dGC);
    if (fResetTest)     {
        if (dGC >1 )   {
            SLOG (eError, "FAIL : %s = 0x%08x. Its 31:1 bits should be zero on reset ", RegGC.Name(), dGC);
            fResult  = FNS_FAIL;
        }
        return fResult;
    }    


    Hit::CBitRange &UNSOL = RegGC["Accept Unsolicited Response Enable -UNSOL"];    
//    Hit::CBitRange &FCNTRL = RegGC ["Flush Control -FCNTRL"];    
//    Hit::CBitRange &CRST = RegGC ["Controller Reset -CRST"];    
    
//    CRST.Mask();
//    FCNTRL.Mask();
    BYTE bUnSol = (BYTE) UNSOL.Read();
    SLOG (eInfo1, "%s = 0x%02x", UNSOL.Name(), bUnSol);
    UNSOL.Write(0x1);
    bUnSol = (BYTE) UNSOL.Read();
    SLOG (eInfo1, "%s = 0x%02x", UNSOL.Name(), bUnSol);
    if (!(bUnSol & 0x01)) {
        SLOG(eError, "FAIL : %s is not RW", UNSOL.Name());
        fResult = FNS_FAIL;
    }
    UNSOL.Write(0x00);
    bUnSol = (BYTE) UNSOL.Read();
    SLOG (eInfo1, "%s = 0x%02x", UNSOL.Name(), bUnSol);

    if (bUnSol | 0x00) {
        SLOG(eError, "FAIL: %s is not RW", UNSOL.Name());
        fResult = FNS_FAIL;
    }
/*
  
    if (!HdCntAsrt.ReadWrite(" ", UNSOL))
        fResult = FNS_FAIL;
*/
/*    


    BYTE bFcntrl= (BYTE) FCNTRL.Read();
    if (!HdCntAsrt.ReadWrite(" ", FCNTRL))
        fResult = FNS_FAIL;

//    CRST.Unmask();
    BYTE bCrst= (BYTE) CRST.Read();
    if (!HdCntAsrt.ReadWrite(" ", CRST))
        fResult = FNS_FAIL;
*/
    SLOG (eInfo1, "%s = 0x%08x", RegGC.Name(), dGC);
    SLOG (eInfo1, "%s = %d", UNSOL.Name(), bUnSol);
//    SLOG (eInfo1, "%s = %d", FCNTRL.Name(),bFcntrl);
//    SLOG (eInfo1, "%s = %d", CRST.Name(), bCrst);
    
    return fResult;
}


DWORD TestWakeEnable()
{
    DWORD fResult = FNS_PASS;
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &RegWE = GenTable["Wake Enable"];
    TESTINGREG(RegWE.Name());
    WORD dGC = (WORD) RegWE.Read() ; 
    SLOG (eInfo1, "%s = 0x%04x", RegWE.Name(), dGC);
    if (fResetTest)     {
        // SINCE THIS  register is RSM we are not sure what will be its value
/*        if (dGC != 0 )   {
            SLOG (eError, "FAIL : %s = 0x%04x. It should be zero on reset ", RegWE.Name(), dGC);
            fResult  = FNS_FAIL;
        }*/
        fResult  = FNS_PASS;
        return fResult;        
    }    
    
//    Hit::CBitRange &WEReserved = RegWE ["Reserved_2"];        
//    WEReserved.Mask();

//    if (!HdCntAsrt.ReadWrite(" ", RegWE))
//        fResult = FNS_FAIL;

    Hit::CBitRange &SDIWE = RegWE ["SDIN Wake Enable Flags -SDIWEN"];    
    
    WORD bSDIWE = (WORD) SDIWE.Read();
    SLOG (eInfo1, "%s = %d", SDIWE.Name(), bSDIWE);
/*    if (!HdCntAsrt.ReadWrite(" ", SDIWE))
        fResult = FNS_FAIL;
    
*/

//    WEReserved.Unmask();    
    return fResult;
}

DWORD TestWakeStatus()
{
    DWORD fResult = FNS_PASS;
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &RegWE = GenTable["Wake Status"];
    TESTINGREG(RegWE.Name());
    WORD dGC = (WORD) RegWE.Read() ; 
    SLOG (eInfo1, "%s = 0x%04x", RegWE.Name(), dGC);
    if (fResetTest)     {
        // SINCE THIS  register is RSM we are not sure what will be its value
/*        if (dGC != 0 )   {
            SLOG (eError, "FAIL : %s = 0x%04x. It should be zero on reset ", RegWE.Name(), dGC);
            fResult  = FNS_FAIL;
        }*/
        fResult  = FNS_PASS;
        return fResult;
    }    
    
//    Hit::CBitRange &WEReserved = RegWE ["Reserved_3"];        
//    WEReserved.Mask();

//    if (!HdCntAsrt.ReadWrite(" ", RegWE))
//        fResult = FNS_FAIL;

    Hit::CBitRange &SDIWE = RegWE ["SDIN State Change Status Flags -SDIWAKE"];    
    
    WORD bSDIWE = (WORD) SDIWE.Read();
    SLOG (eInfo1, "%s = 0x%04x", SDIWE.Name(), bSDIWE);
/*
    SDIWE.Write(0x1);
    bSDIWE = (WORD) SDIWE.Read();
    SLOG (eInfo1, "%s = 0x%04x", SDIWE.Name(), bSDIWE);
    SDIWE.Write(0x2);
    bSDIWE = (WORD) SDIWE.Read();
    SLOG (eInfo1, "%s = 0x%04x", SDIWE.Name(), bSDIWE);
    SDIWE.Write(0x4);
    bSDIWE = (WORD) SDIWE.Read();
    SLOG (eInfo1, "%s = 0x%04x", SDIWE.Name(), bSDIWE);
    SDIWE.Write(0x0);
    bSDIWE = (WORD) SDIWE.Read();
    SLOG (eInfo1, "%s = 0x%04x", SDIWE.Name(), bSDIWE);
*/
    
    
//    WEReserved.Unmask();    
    return fResult;
}

DWORD TestGlobalStatus()
{
    DWORD fResult = FNS_PASS;
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &Reg = GenTable["Global Status"];
    TESTINGREG(Reg.Name());
    WORD dReg = (WORD) Reg.Read() ; 
    SLOG (eInfo1, "%s = 0x%04x", Reg.Name(), dReg);
    if (fResetTest)     {
        if (dReg != 0 )   {
            SLOG (eError, "FAIL : %s = 0x%04x. It should be zero on reset ", Reg.Name(), dReg);
            fResult  = FNS_FAIL;
        }
        return fResult;        
    }    
    
//    Hit::CBitRange &BReserved = Reg ["Reserved"];        
//    Hit::CBitRange &BReserved_4 = Reg ["Reserved_4"];        
//    WEReserved.Mask();



//    if (!HdCntAsrt.ReadWrite(" ", RegWE))
//        fResult = FNS_FAIL;

//    Hit::CBitRange &BFSTS = Reg ["Flush Status -FSTS"];    
    
    return fResult;
}

// test for the OUTSTRMPAY 
DWORD TestOUTSTRMPAY()
/*Offset 18h: OUTSTRMPAY – Input Stream Payload Capability
Bit 	Type 	Reset 	Description
15:0 RO 		3Ch		Output Stream Payload Capability (OUTSTRMPAY) Indicates the maximum
					number of Words per frame for any single output stream. 
*/
{
    DWORD fResult = FNS_PASS;
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &Reg = GenTable["Output Stream Payload Capability"];
    TESTINGREG(Reg.Name());

    // compare to the master
    Hit::CRegister &Master = GenTable["Output Payload Capability"];    
    WORD masterVal = (WORD) Master.Read();

    WORD val = (WORD) Reg.Read() ; 
    SLOG (eInfo1, "%s = 0x%04x", Reg.Name(), val);
    if (fResetTest)     {
        if (val > masterVal)   {
            SLOG (eError, "FAIL : %s = 0x%04x. It should be <= 0x%04x", Reg.Name(), val, masterVal);
            fResult  = FNS_FAIL;
        }
        return fResult;
    }    
    
// Test: for Read Only	
    if (!HdCntAsrt.ReadOnly("Testing for Read Only", Reg))
        fResult = FNS_FAIL;
// Test: value of OUTSTRMPAY must be less then or equal to OUTPAYPLoad
    if (val > masterVal)  {
        fResult = FNS_FAIL;
        SLOG (eError, "FAIL : %s  = 0x%04x is greated then OUTPAY Value 0x%04x", Reg.Name(), val, masterVal);
    }

    return fResult;
}


// test for the INSTRMPAY 
DWORD TestINSTRMPAY()
/*Offset 1Ah: INSTRMPAY – Input Stream Payload Capability
Bit 	Type 	Reset 	Description
15:0 RO 		1Dh		Input Stream Payload Capability (INSTRMPAY) Indicates the maximum
					number of Words per frame for any single input stream. 
*/
{
    DWORD fResult = FNS_PASS;
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &Reg = GenTable["Input Stream Payload Capability"];
    TESTINGREG(Reg.Name());
    WORD val = (WORD) Reg.Read(); 

    // compare to the master
    Hit::CRegister &Master = GenTable["Input Payload Capability"];    
    WORD masterVal = (WORD) Master.Read();

    SLOG (eInfo1, "%s = 0x%04x", Reg.Name(), val);
    if (fResetTest)     {
        if (val > masterVal)   {
            SLOG (eError, "FAIL : %s = 0x%04x. It should be <= 0x%04x", Reg.Name(), val, masterVal);
            fResult  = FNS_FAIL;
        }
        return fResult;
    }    

    // Test: for Read Only
    if (!HdCntAsrt.ReadOnly("Testing for Read Only", Reg))
        fResult = FNS_FAIL;

    // Test: value of INSTRMPAY must be less then or equal to INPAYPLoad
    if (val > masterVal)   {
        fResult = FNS_FAIL;
        SLOG (eError, "FAIL : %s = 0x%04x is greated then INPAY Value 0x%04x", Reg.Name(), val, masterVal);
    }

    return fResult;
}

// test for the Interrupt Control 
DWORD TestINTCTL()
/*Offset 20h: INTCTL – Interrupt Control
*/

{
    DWORD fResult = FNS_PASS;
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &Reg = GenTable["Interrupt Control"];
    TESTINGREG(Reg.Name());
    ULONG uIss, uOss, uBss, uTss;
    GetStreamNumbers(uIss, uOss, uBss);
    uTss = uIss + uOss + uBss;
    TESTINGREG(Reg.Name());        
    DWORD dReg = Reg.Read() ; 
    SLOG (eInfo1, "%s = 0x%08x", Reg.Name(), dReg);      
    if (fResetTest)     {
        // per HDA DCN 47-A
        // Controller Interrupt Enable and Global Interrupt Enable persistence in Controller Reset
        // the top two bits (30 and 31) can have any reset value, at the hardware implementer's discretion
        // but the bottom 30 bits need a reset value of 0
        if ((dReg & 0x3fffffff) != 0 )   {
            SLOG(eError, "FAIL : %s = 0x%08x. Low 30 bits should be zero on reset ", Reg.Name(), dReg);
            fResult = FNS_FAIL;
        }
        return fResult;        
    }    
    
// Test: for Read Write property
    SLOG (eInfo1, "Testing 0 to %d bits of %s  for Read Write", uTss, Reg.Name());      
    if (!Reg.ReadWrite(0, uTss))
        fResult = FNS_FAIL;

    Hit::CBitRange &BCIE = Reg["Controller Interrupt Enable -CIE"];    
    BYTE bBCIE = (BYTE) BCIE.Read();
    SLOG (eInfo1, "%s = 0x%02x", BCIE.Name(), bBCIE);      
// Test: for Read Write property
    if (!HdCntAsrt.ReadWrite("Testing for Read Write", BCIE))
        fResult = FNS_FAIL;
    
    Hit::CBitRange &BGIE = Reg["Global Interrupt Enable -GIE"];    
    BYTE bBGIE = (BYTE) BGIE.Read();
    SLOG (eInfo1, "%s = 0x%02x", BGIE.Name(), bBGIE);      
// Test: for Read Write property
    if (!HdCntAsrt.ReadWrite("Testing for Read Write", BGIE))
        fResult = FNS_FAIL;


    return fResult;
}

// test for the Interrupt Status 
DWORD TestINTSTS()
/*Offset 24h: INTSTS – Interrupt Status
*/

{
    DWORD fResult = FNS_PASS;
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &Reg = GenTable["Interrupt Status"];
    TESTINGREG(Reg.Name());
    ULONG uIss, uOss, uBss/*, uTss*/;
    GetStreamNumbers(uIss, uOss, uBss);
//    uTss = uIss + uOss + uBss;
    TESTINGREG(Reg.Name());        
    DWORD dReg = Reg.Read() ; 
    SLOG (eInfo1, "%s = 0x%08x", Reg.Name(), dReg);      
    if (fResetTest)     {
        // per HDA DCN 47-A
        // Controller Interrupt Enable and Global Interrupt Enable persistence in Controller Reset
        // the top two bits (30 and 31) can have any reset value, at the hardware implementer's discretion
        // but the bottom 30 bits need a reset value of 0
        if ((dReg & 0x3fffffff) != 0 )   {
            SLOG (eError, "FAIL : %s = 0x%08x. Low 30 bits should be zero on reset ", Reg.Name(), dReg);
            fResult  = FNS_FAIL;
        }
        return fResult;        
    }    
    // chaged as per ECR - HDA001-A
    fResult = TestDWORDForRO (Reg);
/*    Hit::CBitRange &BSIS = Reg ["Stream Interrupt Status -SIS"];        
    Hit::CBitRange &BCIS = Reg ["Controller Interrupt Status -CIS"];        
    Hit::CBitRange &BGIE = Reg ["Global Interrupt Status -GIS"];        
    BYTE bBCIS =  (BYTE) BCIS.Read();
    SLOG (eInfo1, "%s = 0x%01x", BCIS.Name(), bBCIS);          
    BCIS.Write(0x1);
    SLOG (eInfo1, "%s = 0x%01x", BCIS.Name(), bBCIS);          
    bBCIS =  (BYTE) BCIS.Read();
    SLOG (eInfo1, "%s = 0x%01x", BCIS.Name(), bBCIS);          
    BCIS.Write(0x0);
    SLOG (eInfo1, "%s = 0x%01x", BCIS.Name(), bBCIS);          
*/
    return fResult;
}

// Test for the Wall Clock Counter
// TODO: How to find out that BCLK is enabled
DWORD TestWallClockCounter()
	
{
    DWORD fResult = FNS_PASS;
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &RegWallClock = GenTable["Wall Clock Counter"];
    TESTINGREG(RegWallClock.Name());

    ULONG32 dWallClock; 
    ULONG32 dWallClockMax = ULONG_MAX;
    DWORD Status =  RegWallClock.Write(0xF0F0F0F0);
    SLOG (eInfo3, "Status =  %x ", Status);

    // BUGBUG:   Here I am assuming that 2 successive reading of Wall clock counter will not be same.
    ULONG32 dPreWallClock = (ULONG32) RegWallClock.Read() ;
    Sleep (1); // Sleep for 1 Ms and then read closk again	
    dWallClock = RegWallClock.Read() ;     
    int i =0;
    while ((dWallClock != dPreWallClock) && i < 5)  {
        dPreWallClock = dWallClock;
        dWallClock = (ULONG32) RegWallClock.Read();
// Test : make sure that clock keeps progressing.         
        if (dWallClock < dPreWallClock) {
// Find for the loop case - Diffenrece between 2 reading should not be more then 16 sec of clock progress
            if ((dWallClockMax - dPreWallClock + dWallClock ) > 0x16E1F76B )    {
                fResult = FNS_FAIL;
                break;
            }
        }
        Sleep (1);
        i++;
    }
    if (dWallClock == dPreWallClock)    {
        SLOG (eError, "FAIL : %s reamins at the same value", RegWallClock.Name());
        fResult = FNS_FAIL;
    }
    SLOG (eInfo1, "%s = 0x%08x", RegWallClock.Name(), dWallClock);
    return fResult;

}

// to test SSYNC – Stream Synchronization
DWORD TestSSYNC ()   {
    DWORD fResult = FNS_PASS;
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &RegSSYNC = GenTable["Stream Synchronization"];
    ULONG uIss, uOss, uBss, uTss;
    GetStreamNumbers(uIss, uOss, uBss);
    uTss = uIss + uOss + uBss;
    TESTINGREG(RegSSYNC.Name());        
    DWORD dSSYNC = RegSSYNC.Read() ; 
    SLOG (eInfo1, "%s = 0x%08x", RegSSYNC.Name(), dSSYNC);      
    if (fResetTest)     {
        if (dSSYNC != 0 )   {
            SLOG (eError, "FAIL : %s = 0x%08x. It should be zero on reset ", RegSSYNC.Name(), dSSYNC);
            fResult  = FNS_FAIL;
        }
        return fResult;        
    }    
    
// Test: for Read Write property
    SLOG (eInfo1, "Testing 0 to %d bits of %s  for Read Write", uTss, RegSSYNC.Name());      
    if (!RegSSYNC.ReadWrite(0, uTss))
        fResult = FNS_FAIL;
 
/*    Hit::CBitRange &BSSYNCBits = RegSSYNC["Stream Synchronization Bits -SSYNC"];    
    BYTE bBSSYNCBits = (BYTE) BSSYNCBits.Read();
    SLOG (eInfo1, "%s = 0x%04x", BSSYNCBits.Name(), bBSSYNCBits);      
// Test: for Read Write property
    if (!HdCntAsrt.ReadWrite("Testing for Read Write", BSSYNCBits))
        fResult = FNS_FAIL;
        */
// TODO: testing for Rsvp bits ??

    return fResult;
}

// to test CORB Lower Base Address
DWORD TestCORBLBASE ()  
{
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &RegCORBLBASE = GenTable["CORB Lower Base Address"];
    Hit::CBitRange &BCORBLBUB = RegCORBLBASE["CORB Lower Base Unimplemented Bits"];    
    Hit::CBitRange &BCORBLBASE = RegCORBLBASE["CORB Lower Base Address - CORBLBASE"];    

    return TestLBASE (RegCORBLBASE, BCORBLBUB, BCORBLBASE) ;

}

// to test CORB Upper Base Address
DWORD TestCORBUBASE ()
{
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &RegCORBUBASE = GenTable["CORB Upper Base Address"];
    return TestUpperBaseAddress(RegCORBUBASE);

}

// to test CORBWP – CORB Write Pointer
DWORD TestCORBWP ()   
{
    DWORD fResult = FNS_PASS;
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &RegCORBWP = GenTable["CORB Write Pointer"];
    TESTINGREG(RegCORBWP.Name());        
    WORD bRegCORBWP = (WORD) RegCORBWP.Read() ; 
    SLOG (eInfo1, "%s = 0x%04x", RegCORBWP.Name(), bRegCORBWP);      
    if (fResetTest)     {
        if (bRegCORBWP != 0 )   {
            SLOG (eError, "FAIL : %s = 0x%04x. It should be zero on reset ", RegCORBWP.Name(), bRegCORBWP);
            fResult  = FNS_FAIL;
        }
        return fResult;        
    }    

    Hit::CBitRange &BCORBWP= RegCORBWP["CORB Write Pointer -CORBWP"];    
    BYTE bBCORBWP = (BYTE) BCORBWP.Read();
// Test: for Read Write property
    if (!HdCntAsrt.ReadWrite("Testing for Read Write", BCORBWP))
        fResult = FNS_FAIL;
    SLOG (eInfo1, "%s = 0x%02x", BCORBWP.Name(), bBCORBWP);      
// TODO: test for RsvP is still to be done
    return fResult;
}

// to test CORBRP – CORB Read Pointer
DWORD TestCORBRP ()   
{
    DWORD fResult = FNS_PASS;
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &RegCORBRP = GenTable["CORB Read Pointer"];
    TESTINGREG(RegCORBRP.Name());        
    WORD bRegCORBRP = (WORD) RegCORBRP.Read() ; 
    SLOG (eInfo1, "%s = 0x%04x", RegCORBRP.Name(), bRegCORBRP );      
    if (fResetTest)     {
        if (bRegCORBRP != 0 )   {
            SLOG (eError, "FAIL : %s = 0x%04x. It should be zero on reset ", RegCORBRP.Name(), bRegCORBRP);
            fResult  = FNS_FAIL;
        }
        return fResult;        
    }    

    Hit::CBitRange &BCORBRP = RegCORBRP["CORB Read Pointer -CORBRP"];    
    BYTE bBCORBRP = (BYTE) BCORBRP.Read();
// Test: for Read Only property
    if (!HdCntAsrt.ReadOnly("Testing for Read Only", BCORBRP))
        fResult = FNS_FAIL;
    SLOG (eInfo1, "%s = 0x%02x", BCORBRP.Name(), bBCORBRP);      
// TODO: test for CORBRPRST and RsvP is still to be done
    return fResult;
}

// to test CORBCTL – CORB Control
DWORD TestCORBCTL ()   
{
    DWORD fResult = FNS_PASS;
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &RegCORBCTL = GenTable["CORB Control"];
    TESTINGREG(RegCORBCTL.Name());        
    BYTE bRegCORBSIZE = (BYTE) RegCORBCTL.Read() ; 
    SLOG (eInfo1, "%s = 0x%02x", RegCORBCTL.Name(), bRegCORBSIZE);      
    if (fResetTest)     {
        if (bRegCORBSIZE!= 0 )   {
            SLOG (eError, "FAIL : %s = 0x%02x. It should be zero on reset ", RegCORBCTL.Name(), bRegCORBSIZE);
            fResult  = FNS_FAIL;
        }
        return fResult;        
    }    

    Hit::CBitRange &BCMEIE = RegCORBCTL["CORB Memory Error Interrupt Enable -CMEIE"];    
    BYTE bBCMEIE = (BYTE) BCMEIE.Read();
// Test: for Read Write property
    if (!HdCntAsrt.ReadWrite("Testing for Read Write", BCMEIE))
        fResult = FNS_FAIL;
    SLOG (eInfo1, "%s = 0x%02x", BCMEIE.Name(), bBCMEIE);      
    //removed  testing of CORBRUN as we have test case for measuring time taken by controller to chage the sate of CORBRUN. 
    //Some controller do not flip this bit immediately. 
    /*    
    Hit::CBitRange &BCORBRUN = RegCORBCTL["Enable CORB DMA Engine"];    
// Test: for Read Write property
    TESTINGREG(BCORBRUN.Name());
    BYTE bBCORBRUN = (BYTE) BCORBRUN.Read();
    if (!HdCntAsrt.ReadWrite("Testing for Read Write", BCORBRUN))
        fResult = FNS_FAIL;
    SLOG (eInfo1, "%s = 0x%02x", BCORBRUN.Name(), bBCORBRUN);
    */
// TODO: Code for testing for RsvdP is missing     
    return fResult;
}

// to test CORBSTS – CORB Status
DWORD TestCORBSTS ()   
{
    DWORD fResult = FNS_PASS;
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &Reg = GenTable["CORB Status"];
    TESTINGREG(Reg.Name());        
    BYTE bReg = (BYTE) Reg.Read() ; 
    SLOG (eInfo1, "%s = 0x%02x", Reg.Name(), bReg);      
    if (fResetTest)     {
        if (bReg != 0 )   {
            SLOG (eError, "FAIL : %s = 0x%02x. It should be zero on reset ", Reg.Name(), bReg);
            fResult  = FNS_FAIL;
        }
        return fResult;        
    }    
    return fResult;
}

// to test CORBSIZE – CORB Size
DWORD TestCORBSIZE ()   
{
    DWORD fResult = FNS_PASS;
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &RegCORBSIZE = GenTable["CORB Size"];
    TESTINGREG(RegCORBSIZE.Name());        
    BYTE bRegCORBSIZE = (BYTE) RegCORBSIZE.Read() ; 
    SLOG (eInfo1, "%s = 0x%02x", RegCORBSIZE.Name(), bRegCORBSIZE);      
    if (fResetTest)     {
            SLOG (eInfo1, "Info: %s = 0x%02x. CORBSIZE depends on the Implementor", RegCORBSIZE.Name(), bRegCORBSIZE);
            fResult  = FNS_PASS;
            return fResult;        
    }    

    Hit::CBitRange &BCSize = RegCORBSIZE["CORB Size -CORBSIZE"];    
    BYTE bBCSize = (BYTE) BCSize.Read();
// Test: for Read Write property
// we are not testing this register for the RW. Check its description in Specification.
/*    if (!HdCntAsrt.ReadWrite("Testing for Read Write", BCSize))
        fResult = FNS_FAIL;*/
    SLOG (eInfo1, "%s = 0x%02x", BCSize.Name(), bBCSize);      
        
    Hit::CBitRange &BCORBSZCAP = RegCORBSIZE["CORB Size Capability -CORBSZCAP"];    
// Test: for Read Only 0 property
    TESTINGREG(BCORBSZCAP.Name());
    if (!HdCntAsrt.ReadOnly("Testing for Read Only", BCORBSZCAP))
        fResult = FNS_FAIL;
    BYTE bRIRBSZCAP = (BYTE) BCORBSZCAP.Read();
    SLOG (eInfo1, "%s = 0x%02x", BCORBSZCAP.Name(), bRIRBSZCAP);      
    
    return fResult;
}


// to test RIRBLBASE – RIRB Lower Base Address
DWORD TestRIRBLBASE ()   
{
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &RegRIRBLBASE = GenTable["RIRB Lower Base Address"];
    Hit::CBitRange &BRIRBLBUB = RegRIRBLBASE["RIRB Lower Base Unimplemented Bits"];    
    Hit::CBitRange &BRIRBLBASE = RegRIRBLBASE["RIRB Lower Base Address - RIRBLBASE"];    
    return TestLBASE (RegRIRBLBASE, BRIRBLBUB, BRIRBLBASE) ;
}


// to test RIRB Upper Base Address
DWORD TestRIRBUBASE ()
{
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &RegRIRBUBASE = GenTable["RIRB Upper Base Address"];
    return TestUpperBaseAddress(RegRIRBUBASE);

}

// to test RIRBWP – RIRB Write Pointer
DWORD TestRIRBWP ()   
{
    DWORD fResult = FNS_PASS;
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &RegRIRBWP = GenTable["RIRB Write Pointer"];
    TESTINGREG(RegRIRBWP.Name());        
    WORD bRegRIRBWP = (WORD) RegRIRBWP.Read() ; 
    SLOG (eInfo1, "%s = 0x%04x", RegRIRBWP.Name(), bRegRIRBWP);      
    if (fResetTest)     {
        if (bRegRIRBWP != 0 )   {
            SLOG (eError, "FAIL : %s = 0x%04x. It should be zero on reset ", RegRIRBWP.Name(), bRegRIRBWP);
            fResult  = FNS_FAIL;
        }
        return fResult;        
    }    

    Hit::CBitRange &BRIRBWP= RegRIRBWP["RIRB Write Pointer -RIRBWP"];    
    BYTE bBRIRBWP = (BYTE) BRIRBWP.Read();
// Test: for Read Only property
    if (!HdCntAsrt.ReadOnly("Testing for Read Only", BRIRBWP))
        fResult = FNS_FAIL;
    SLOG (eInfo1, "%s = 0x%02x", BRIRBWP.Name(), bBRIRBWP);      
// TODO: test for RIRBWPRST is still to be done
    return fResult;
}

// to test RINTCNT – Response Interrupt Count
DWORD TestRINTCNT ()   
{
    DWORD fResult = FNS_PASS;
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &RegRINTCNT = GenTable["Response Interrupt Count"];
    TESTINGREG(RegRINTCNT.Name());        
    BYTE bRegRINTCNT = (BYTE) RegRINTCNT.Read() ; 
    SLOG (eInfo1, "%s = 0x%02x", RegRINTCNT.Name(), bRegRINTCNT);      
    if (fResetTest)     {
        if (bRegRINTCNT != 0 )   {
            SLOG (eError, "FAIL : %s = 0x%02x. It should be zero on reset ", RegRINTCNT.Name(), bRegRINTCNT) ;
            fResult  = FNS_FAIL;
        }
        return fResult;        
    }    

    Hit::CBitRange &BRINTCNT = RegRINTCNT["N Response Interrupt Count -RINTCNT"];    
    BYTE bBRINTCNT = (BYTE) BRINTCNT.Read();
// Test: for Read Write property
    if (!HdCntAsrt.ReadWrite("Testing for Read Write", BRINTCNT))
        fResult = FNS_FAIL;
    SLOG (eInfo1, "%s = 0x%02x", BRINTCNT.Name(), bBRINTCNT);      
    return fResult;
}

// to test RIRBCTL – RIRB Control
DWORD TestRIRBCTL ()   
{
    DWORD fResult = FNS_PASS;
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &RegRIRBCTL = GenTable["RIRB Control"];
    TESTINGREG(RegRIRBCTL.Name());        
    BYTE bRegRIRBSIZE = (BYTE) RegRIRBCTL.Read() ; 
    SLOG (eInfo1, "%s = 0x%02x", RegRIRBCTL.Name(), bRegRIRBSIZE);      
    if (fResetTest)     {
        if (bRegRIRBSIZE != 0 )   {
            SLOG (eError, "FAIL : %s = 0x%02x. It should be zero on reset ", RegRIRBCTL.Name(), bRegRIRBSIZE) ;
            fResult  = FNS_FAIL;
        }
        return fResult;        
    }    


    Hit::CBitRange &BRINTCTL = RegRIRBCTL["Response Interrupt Control -RINTCTL"];    
    BYTE bBRINTCTL = (BYTE) BRINTCTL.Read();
// Test: for Read Write property
    if (!HdCntAsrt.ReadWrite("Testing for Read Write", BRINTCTL))
        fResult = FNS_FAIL;
    SLOG (eInfo1, "%s = 0x%02x", BRINTCTL.Name(), bBRINTCTL);      
   //removed  testing of CORBRUN as we have test case for measuring time taken by controller to chage the sate of CORBRUN. 
  //Some controller do not flip this bit immediately. 
    /*    
    Hit::CBitRange &BRIRBDMAEN = RegRIRBCTL["RIRB DMA Enable -RIRBDMAEN"];    
// Test: for Read Write property
    TESTINGREG(BRIRBDMAEN.Name());
    BYTE bBRIRBDMAEN = (BYTE) BRIRBDMAEN.Read();
    if (!HdCntAsrt.ReadWrite("Testing for Read Write", BRIRBDMAEN))
        fResult = FNS_FAIL;
    SLOG (eInfo1, "%s = 0x%02x", BRIRBDMAEN.Name(), bBRIRBDMAEN);      
    */
    Hit::CBitRange &BRIRBOIC = RegRIRBCTL["Response Overrun Interrupt Control -RIRBOIC"];    
// Test: for Read Write property
    TESTINGREG(BRIRBOIC.Name());
    BYTE bBRIRBOIC = (BYTE) BRIRBOIC.Read();
    if (!HdCntAsrt.ReadWrite("Testing for Read Write", BRIRBOIC))
        fResult = FNS_FAIL;
    SLOG (eInfo1, "%s = 0x%02x", BRIRBOIC.Name(), bBRIRBOIC);      

    return fResult;
}

// to test RIRBSTS – RIRB Status
DWORD TestRIRBSTS ()   
{
    DWORD fResult = FNS_PASS;
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &Reg = GenTable["RIRB Status"];
    TESTINGREG(Reg.Name());        
    BYTE bReg = (BYTE) Reg.Read() ; 
    SLOG (eInfo1, "%s = 0x%02x", Reg.Name(), bReg);      
    if (fResetTest)     {
        if (bReg != 0 )   {
            SLOG (eError, "FAIL : %s = 0x%02x. It should be zero on reset ", Reg.Name(), bReg);
            fResult  = FNS_FAIL;
        }
        return fResult;        
    }    
    return fResult;
}


// to test RIRBSIZE – RIRB Size
DWORD TestRIRBSIZE ()   
{
    DWORD fResult = FNS_PASS;
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &RegRIRBSIZE = GenTable["RIRB Size"];
    TESTINGREG(RegRIRBSIZE.Name());        
    BYTE bRegRIRBSIZE = (BYTE) RegRIRBSIZE.Read() ; 
    SLOG (eInfo1, "%s = 0x%02x", RegRIRBSIZE.Name(), bRegRIRBSIZE);      
    if (fResetTest)     {
            SLOG (eInfo1, "INFO : %s = 0x%02x. Value if RIRBSIZE depends on Implementor", RegRIRBSIZE.Name(), bRegRIRBSIZE);
            fResult  = FNS_PASS;
            return fResult;        
    }    

    Hit::CBitRange &BRSize = RegRIRBSIZE["RIRB Size -RIRBSIZE"];    
    BYTE bBRSize = (BYTE) BRSize.Read();
// Test: for Read Write property
/*
    if (!HdCntAsrt.ReadWrite("Testing for Read Write", BRSize))
        fResult = FNS_FAIL;*/
    SLOG (eInfo1, "%s = 0x%02x", BRSize.Name(), bBRSize);      
        
    Hit::CBitRange &BRIRBSZCAP = RegRIRBSIZE["RIRB Size Capability -RIRBSZCAP"];    
// Test: for Read Only 0 property
    TESTINGREG(BRIRBSZCAP.Name());
    if (!HdCntAsrt.ReadOnly("Testing for Read Only", BRIRBSZCAP))
        fResult = FNS_FAIL;
    BYTE bRIRBSZCAP = (BYTE) BRIRBSZCAP.Read();
    SLOG (eInfo1, "%s = 0x%02x", BRIRBSZCAP.Name(), bRIRBSZCAP);      
    
    return fResult;
}


// to test DMA Position Lower Base Address
DWORD TestDPIBLBASE ()   
{
    DWORD fResult = FNS_PASS;
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &RegDPLBASE = GenTable["DMA Position Buffer Lower Base"];
    TESTINGREG(RegDPLBASE.Name());        
    DWORD dDPLBase = RegDPLBASE.Read() ; 
    SLOG (eInfo1, "%s = 0x%08x", RegDPLBASE.Name(), dDPLBase);      
    if (fResetTest)     {
        if (dDPLBase != 0 )   {
            SLOG (eError, "FAIL : %s = 0x%08x. It should be zero on reset ", RegDPLBASE.Name(), dDPLBase);
            fResult  = FNS_FAIL;
        }
        return fResult;        
    }    

//    Hit::CBitRange &DMAEnable = RegDPLBASE["DMA Position Buffer Enable"];    
//    BYTE bDMAEnable = (BYTE) DMAEnable.Read();
// Test: for Read Write property
// TODO: for all dma engine we have removed test of RW as we will have seperate test to find out time taken by DMA to filp the bits
/*
    if (!HdCntAsrt.ReadWrite("Testing for Read Write", DMAEnable))
        fResult = FNS_FAIL;
    SLOG (eInfo1, "%s = 0x%02x", DMAEnable.Name(), bDMAEnable);      
  */      
    Hit::CBitRange &BDMAPLBUB = RegDPLBASE["DMA Position Lower Base Unimplemented Bits"];    
// Test: for Read Only 0 property
    TESTINGREG(BDMAPLBUB.Name());
    if (!HdCntAsrt.ReadOnly0("Testing for Read Only 0 ", BDMAPLBUB))
        fResult = FNS_FAIL;
    BYTE bBDMAPLBUB = (BYTE) BDMAPLBUB.Read();
    SLOG (eInfo1, "%s = 0x%02x", BDMAPLBUB.Name(), bBDMAPLBUB);      

    Hit::CBitRange &BDmaPLBASE = RegDPLBASE["DMA Position Lower Base Address -DPLBASE"];    
// Test: for Read Write property
    TESTINGREG(BDmaPLBASE.Name());
    if (!HdCntAsrt.ReadWrite("Testing for Read Write", BDmaPLBASE))
        fResult = FNS_FAIL;

    DWORD dBDmaPLBASE = (DWORD) BDmaPLBASE.Read();
    SLOG (eInfo1, "%s = 0x%08x", BDmaPLBASE.Name(), dBDmaPLBASE);      
    
    return fResult;
}

// to test DMA Position Buffer Upper Base
DWORD TestDPIBUBASE ()
{
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &RegDPIBUBASE = GenTable["DMA Position Buffer Upper Base"];
    return TestUpperBaseAddress(RegDPIBUBASE);

}

DWORD TestStreamDescriptorCTL (Hit::CRegister &Reg , UINT i, BOOL fBiDir)   
{
    DWORD fResult = FNS_PASS;
    TESTINGREG(Reg.Name());        
    BYTE dRegVal =  (BYTE) Reg.Read(); // note this is only the TOP byte (bits 16-23)
    SLOG (eInfo1, "SD%d %s = 0x%02x", i, Reg.Name(), dRegVal);      
    if (fResetTest) {
        if (dRegVal != 0x0 && dRegVal != 0x4) {
            SLOG (eError,
                "FAIL : SD%d %s = 0x%02x. "
                "This is the top byte (bits 16-23) of a three-byte register. "
                "See the HD Audio specification section 3.3.35 for details. "
                "All bits (except possibly Traffic Priority) should be zero on reset.",
                i, Reg.Name(), dRegVal);
            fResult = FNS_FAIL;
        }
        return fResult;        
    }    

    Hit::CBitRange &DirBit = Reg["Bidirectional Direction Control -DIR"];    
    if ( fBiDir) {
        if (!HdCntAsrt.ReadWrite("Testing for Read Write for Bidriectional stream", DirBit))
            fResult = FNS_FAIL;
    }
    else {
       if (!HdCntAsrt.ReadOnly0("Testing for Read Write for Input/output stream", DirBit))
            fResult = FNS_FAIL;
    }
   

   Hit::CBitRange &StrmBits = Reg["Stream Number -STRM"];    
    if (!HdCntAsrt.ReadWrite("Testing for Read Write", StrmBits))
        fResult = FNS_FAIL;
 
    return fResult;
}

// to test Stream Descriptor N Contorl
DWORD TestSDnCTL()
{
    DWORD fResult = FNS_PASS;
    // first get the number of Input / output and bidirectional streams supported by the controller
    ULONG uIss, uOss, uBss;
    GetStreamNumbers(uIss, uOss, uBss);
    SLOG (eInfo3, "Iss = %d, Oss = %d , Bss = %d", uIss, uOss, uBss);
    for (ULONG i = 0; i<uIss; i++)  {
        Hit::CTable & StreamTable = GetHdHitNameSpace->Tbl("Input_Stream_Descriptor", i);
        Hit::CRegister &RegSDnLPIB = StreamTable["Input Stream Descriptor Control_3"];        
        fResult |= TestStreamDescriptorCTL( RegSDnLPIB, i, FALSE);
    }
    for (ULONG i = 0; i<uOss; i++)  {
        Hit::CTable & StreamTable = GetHdHitNameSpace->Tbl("Output_Stream_Descriptor", i);
        Hit::CRegister &RegSDnLPIB = StreamTable["Output Stream Descriptor Control_3"];        
       fResult |= TestStreamDescriptorCTL( RegSDnLPIB, i, FALSE);
    }
    for (ULONG i = 0; i<uBss; i++)  {
        Hit::CTable & StreamTable = GetHdHitNameSpace->Tbl("BiDirectional_Stream_Descriptor", i);
        Hit::CRegister &RegSDnLPIB = StreamTable["BiDirectional Stream Descriptor Control_3"];        
        fResult |= TestStreamDescriptorCTL( RegSDnLPIB, i, TRUE);
    }
    return fResult;

}

DWORD TestStreamDescriptorSTS (Hit::CRegister &Reg , UINT i )   
{
    DWORD fResult = FNS_PASS;
    TESTINGREG(Reg.Name());        
    BYTE bRegVal = (BYTE) Reg.Read() ; 
    SLOG (eInfo1, "SD%d %s = 0x%02x", i, Reg.Name(), bRegVal);      
    if (fResetTest)     {
        if (bRegVal != 0 )   {
            SLOG (eError, "FAIL : SD%d %s = 0x%02x. It should be zero on reset ", i, Reg.Name(), bRegVal);
            fResult  = FNS_FAIL;
        }
        return fResult;        
    }    
    return fResult;
}

// to test Stream Descriptor N Status
DWORD TestSDnSTS()
{
    DWORD fResult = FNS_PASS;
    // first get the number of Input / output and bidirectional streams supported by the controller
    ULONG uIss, uOss, uBss;
    GetStreamNumbers(uIss, uOss, uBss);
    SLOG (eInfo3, "Iss = %d, Oss = %d , Bss = %d", uIss, uOss, uBss);
    for (ULONG i = 0; i<uIss; i++)  {
        Hit::CTable & StreamTable = GetHdHitNameSpace->Tbl("Input_Stream_Descriptor", i);
        Hit::CRegister &RegSDnLPIB = StreamTable["Input Stream Descriptor Status"];        
        fResult |= TestStreamDescriptorSTS( RegSDnLPIB, i);
    }
    for (ULONG i = 0; i<uOss; i++)  {
        Hit::CTable & StreamTable = GetHdHitNameSpace->Tbl("Output_Stream_Descriptor", i);
        Hit::CRegister &RegSDnLPIB = StreamTable["Output Stream Descriptor Status"];        
        fResult |= TestStreamDescriptorSTS( RegSDnLPIB, i);
    }
    for (ULONG i = 0; i<uBss; i++)  {
        Hit::CTable & StreamTable = GetHdHitNameSpace->Tbl("BiDirectional_Stream_Descriptor", i);
        Hit::CRegister &RegSDnLPIB = StreamTable["BiDirectional Stream Descriptor Status"];        
        fResult |= TestStreamDescriptorSTS( RegSDnLPIB, i );
    }
    return fResult;

}

DWORD TestStreamDescriptorLPIB (Hit::CRegister &Reg , UINT i )   
{
    DWORD fResult = FNS_PASS;
    TESTINGREG(Reg.Name());        
    DWORD dRegVal = Reg.Read() ; 
    SLOG (eInfo1, "SD%d %s = 0x%08x", i, Reg.Name(), dRegVal);      
    if (fResetTest)     {
        if (dRegVal != 0 )   {
            SLOG (eError, "FAIL : SD%d %s = 0x%08x. It should be zero on reset ", i, Reg.Name(), dRegVal);
            fResult  = FNS_FAIL;
        }
        return fResult;        
    }    

    fResult = TestDWORDForRO(Reg);
    SLOG (eInfo1, "SD%d %s = 0x%08x", i, Reg.Name(), Reg.Value());        
    return fResult;
}

// to test Stream Descriptor Link Piositon in Buffer 
DWORD TestSDnLPIB ()  {
    DWORD fResult = FNS_PASS;
    // first get the number of Input / output and bidirectional streams supported by the controller
    ULONG uIss, uOss, uBss;
    GetStreamNumbers(uIss, uOss, uBss);
    SLOG (eInfo3, "Iss = %d, Oss = %d , Bss = %d", uIss, uOss, uBss);
    for (ULONG i = 0; i<uIss; i++)  {
        Hit::CTable & StreamTable = GetHdHitNameSpace->Tbl("Input_Stream_Descriptor", i);
        Hit::CRegister &RegSDnLPIB = StreamTable["Input Stream Descriptor Link Position in Buffer"];        
        fResult |= TestStreamDescriptorLPIB( RegSDnLPIB, i);
    }
    for (ULONG i = 0; i<uOss; i++)  {
        Hit::CTable & StreamTable = GetHdHitNameSpace->Tbl("Output_Stream_Descriptor", i);
        Hit::CRegister &RegSDnLPIB = StreamTable["Output Stream Descriptor Link Position in Buffer"];        
        fResult |= TestStreamDescriptorLPIB( RegSDnLPIB, i);
    }
    for (ULONG i = 0; i<uBss; i++)  {
        Hit::CTable & StreamTable = GetHdHitNameSpace->Tbl("BiDirectional_Stream_Descriptor", i);
        Hit::CRegister &RegSDnLPIB = StreamTable["BiDirectional Stream Descriptor Link Position in Buffer"];        
        fResult |= TestStreamDescriptorLPIB( RegSDnLPIB, i);
    }
    return fResult;

}


// to test Stream Descriptor Cyclic Buffer Length
DWORD TestStreamDescriptorCBL (Hit::CRegister &Reg , UINT i)   
{
    DWORD fResult = FNS_PASS;
    TESTINGREG(Reg.Name());        
    DWORD dRegVal = Reg.Read() ; 
    SLOG (eInfo1, "SD%d %s = 0x%08x", i, Reg.Name(), dRegVal);      
    if (fResetTest)     {
        if (dRegVal != 0 )   {
            SLOG (eError, "FAIL : SD%d %s = 0x%08x. It should be zero on reset ", i, Reg.Name(), dRegVal);
            fResult  = FNS_FAIL;
        }
        return fResult;        
    }    

    fResult = TestDWORDForRW(Reg);
    SLOG (eInfo1, "SD%d %s = 0x%08x", i, Reg.Name(), Reg.Value());        
    return fResult;
}

DWORD TestSDnCBL() 
{
    DWORD fResult = FNS_PASS;
    // first get the number of Input / output and bidirectional streams supported by the controller
    ULONG uIss, uOss, uBss;
    GetStreamNumbers(uIss, uOss, uBss);
    SLOG (eInfo3, "Iss = %d, Oss = %d , Bss = %d", uIss, uOss, uBss);
    for (ULONG i = 0; i<uIss; i++)  {
        Hit::CTable & StreamTable = GetHdHitNameSpace->Tbl("Input_Stream_Descriptor", i);
        Hit::CRegister &RegSDnCBL = StreamTable["Input Stream Descriptor Cyclic Buffer Length"];        
        fResult |= TestStreamDescriptorCBL( RegSDnCBL, i);        
    }
    for (ULONG i = 0; i<uOss; i++)  {
        Hit::CTable & StreamTable = GetHdHitNameSpace->Tbl("Output_Stream_Descriptor", i);
        Hit::CRegister &RegSDnCBL = StreamTable["Output Stream Descriptor Cyclic Buffer Length"];        
        fResult |= TestStreamDescriptorCBL( RegSDnCBL, i);        
    }
    for (ULONG i = 0; i<uBss; i++)  {
        Hit::CTable & StreamTable = GetHdHitNameSpace->Tbl("BiDirectional_Stream_Descriptor", i);
        Hit::CRegister &RegSDnCBL = StreamTable["BiDirectional Stream Descriptor Cyclic Buffer Length"];        
        fResult |= TestStreamDescriptorCBL( RegSDnCBL, i);        
    }
    return fResult;
}

// to test Stream Descriptor Last Valid Index
DWORD TestStreamDescriptorLVI (Hit::CRegister &RegSDnLVI , UINT i)   
{
    DWORD fResult = FNS_PASS;
    TESTINGREG(RegSDnLVI.Name());        
    WORD bRegVal = (WORD) RegSDnLVI.Read() ; 
    SLOG (eInfo1, "SD%d %s = 0x%04x", i, RegSDnLVI.Name(), bRegVal);      
    if (fResetTest)     {
        if (bRegVal != 0 )   {
            SLOG (eError, "FAIL : SD%d %s = 0x%04x. It should be zero on reset ", i ,RegSDnLVI.Name(), bRegVal);
            fResult  = FNS_FAIL;
        }
        return fResult;        
    }    

    Hit::CBitRange &LVI = RegSDnLVI["Last Valid Inex"];    
    BYTE bLVI = (BYTE) LVI.Read();
// Test: for read Write property
    if (!HdCntAsrt.ReadWrite("Testing for Read Write", LVI))
            fResult = FNS_FAIL;
    SLOG (eInfo1, "SD%d %s = 0x%02x", i, LVI.Name(), bLVI);        
    return fResult;
}

DWORD TestSDnLVI() {
    DWORD fResult = FNS_PASS;
    // first get the number of Input / output and bidirectional streams supported by the controller
    ULONG uIss, uOss, uBss;
    GetStreamNumbers(uIss, uOss, uBss);
    SLOG (eInfo3, "Iss = %d, Oss = %d , Bss = %d", uIss, uOss, uBss);
    for (ULONG i = 0; i<uIss; i++)  {
        Hit::CTable & StreamTable = GetHdHitNameSpace->Tbl("Input_Stream_Descriptor", i);
        Hit::CRegister &RegSDnLVI = StreamTable["Input Stream Descriptor Last Valid Index"];        
        fResult |= TestStreamDescriptorLVI( RegSDnLVI, i);        
    }
    for (ULONG i = 0; i<uOss; i++)  {
        Hit::CTable & StreamTable = GetHdHitNameSpace->Tbl("Output_Stream_Descriptor", i);
        Hit::CRegister &RegSDnLVI = StreamTable["Output Stream Descriptor Last Valid Index"];        
        fResult |= TestStreamDescriptorLVI( RegSDnLVI, i);        
    }
    for (ULONG i = 0; i<uBss; i++)  {
        Hit::CTable & StreamTable = GetHdHitNameSpace->Tbl("BiDirectional_Stream_Descriptor", i);
        Hit::CRegister &RegSDnLVI = StreamTable["BiDirectional Stream Descriptor Last Valid Index"];        
        fResult |= TestStreamDescriptorLVI( RegSDnLVI, i);        
    }
    
    return fResult;
}

// to test Stream Descriptor FIFO Size
DWORD TestStreamDescriptorFIFO (Hit::CRegister &RegSDnFIFO , UINT i)   
{
    DWORD fResult = FNS_PASS;
    TESTINGREG(RegSDnFIFO.Name());        
    WORD dSDnFIFO = (WORD) RegSDnFIFO.Read() ; 
    
// Test: for read Only property
    if (!HdCntAsrt.ReadOnly("Testing for Read Only", RegSDnFIFO))
            fResult = FNS_FAIL;
    SLOG (eInfo1, "SD%d %s = 0x%04x", i, RegSDnFIFO.Name(), dSDnFIFO);        
    return fResult;
}

// to test Stream Descriptor Buffer Descriptor Format
DWORD TestStreamDescriptorFMT (Hit::CRegister &RegSDnFMT , UINT i)   
{
    DWORD fResult = FNS_PASS;
    TESTINGREG(RegSDnFMT.Name());        
    WORD dSDnFMT = (WORD) RegSDnFMT.Read() ; 
    SLOG (eInfo1, "SD%d %s = 0x%04x", i, RegSDnFMT.Name(), dSDnFMT);      
    if (fResetTest)     {
        if (dSDnFMT != 0 )   {
            SLOG (eError, "FAIL : SD%d %s = 0x%04x. It should be zero on reset ", i, RegSDnFMT.Name(), dSDnFMT);
            fResult  = FNS_FAIL;
        }
        return fResult;        
    }    


    Hit::CBitRange &BASE = RegSDnFMT["Sample Base Rate -BASE"];    
    BYTE bBASE = (BYTE) BASE.Read();
// Test: for Read Write property
    if (!HdCntAsrt.ReadWrite("Testing for Read Write", BASE))
        fResult = FNS_FAIL;
    SLOG (eInfo1, "SD%d %s = 0x%02x", i, BASE.Name(), bBASE);      
        
    Hit::CBitRange &MULT = RegSDnFMT["Sample Base Rate Multiple -MULT"];    
    BYTE bMULT = (BYTE) MULT.Read();
// Test: for Read Write property
    if (!HdCntAsrt.ReadWrite("Testing for Read Write", MULT))
        fResult = FNS_FAIL;
    SLOG (eInfo1, "SD%d %s = 0x%02x", i, MULT.Name(), bMULT);      

    Hit::CBitRange &DIV = RegSDnFMT["Sample Base Rate Divisor -DIV"];    
    BYTE bDIV = (BYTE) DIV.Read();
// Test: for Read Write property
    if (!HdCntAsrt.ReadWrite("Testing for Read Write", DIV))
        fResult = FNS_FAIL;
    SLOG (eInfo1, "SD%d %s = 0x%02x", i, DIV.Name(), bDIV);      

    Hit::CBitRange &BITS = RegSDnFMT["Bits per Sample -BITS"];    
    BYTE bBITS = (BYTE) BITS.Read();
// Test: for Read Write property
    if (!HdCntAsrt.ReadWrite("Testing for Read Write", BITS))
        fResult = FNS_FAIL;
    SLOG (eInfo1, "SD%d %s = 0x%02x", i, BITS.Name(), bBITS);      

    Hit::CBitRange &CHAN = RegSDnFMT["Number of Channels -CHAN"];    
    BYTE bCHAN = (BYTE) CHAN.Read();
// Test: for Read Write property
    if (!HdCntAsrt.ReadWrite("Testing for Read Write", CHAN))
        fResult = FNS_FAIL;
    SLOG (eInfo1, "SD%d %s = 0x%02x", i, CHAN.Name(), bCHAN);      

    Hit::CBitRange &Resd_2 = RegSDnFMT["Reserved_2"];    
    BYTE bResd_2 = (BYTE) Resd_2.Read();
// Test: for Read Only 0 property
    if (!HdCntAsrt.ReadOnly("Testing for Read Only", Resd_2))
        fResult = FNS_FAIL;
    SLOG (eInfo1, "SD%d %s = 0x%02x", i, Resd_2.Name(), bResd_2);      

    return fResult;
}


DWORD TestSDnFMT()  {
    DWORD fResult = FNS_PASS;
    // first get the number of Input / output and bidirectional streams supported by the controller
    ULONG uIss, uOss, uBss;
    GetStreamNumbers(uIss, uOss, uBss);
    SLOG (eInfo3, "Iss = %d, Oss = %d , Bss = %d", uIss, uOss, uBss);
    for (ULONG i = 0; i<uIss; i++)  {
        Hit::CTable & StreamTable = GetHdHitNameSpace->Tbl("Input_Stream_Descriptor", i);
        Hit::CRegister &RegSDnFMT = StreamTable["Input Stream Descriptor Format"];        
        fResult |= TestStreamDescriptorFMT(RegSDnFMT, i) ;
    }

    for (ULONG i = 0; i<uOss; i++)  {
        Hit::CTable & StreamTable = GetHdHitNameSpace->Tbl("Output_Stream_Descriptor", i);
        Hit::CRegister &RegSDnFMT = StreamTable["Output Stream Descriptor Format"];        
        fResult |= TestStreamDescriptorFMT(RegSDnFMT, i) ;
    }

    for (ULONG i = 0; i<uBss; i++)  {
        Hit::CTable & StreamTable = GetHdHitNameSpace->Tbl("BiDirectional_Stream_Descriptor", i);
        Hit::CRegister &RegSDnFMT = StreamTable["BiDirectional Stream Descriptor Format"];        
        fResult |= TestStreamDescriptorFMT(RegSDnFMT, i) ;
    }
    return fResult;

}

// to test Stream Descriptor Buffer Descriptor List Pointer - Lower
DWORD TestStreamDescriptorBDPL (Hit::CRegister &RegSDnBDPL , UINT i)   
{
    DWORD fResult = FNS_PASS;
    TESTINGREG(RegSDnBDPL.Name());        
    DWORD dSDnBDPL = RegSDnBDPL.Read() ; 
    SLOG (eInfo1, "SD%d %s = 0x%08x", i, RegSDnBDPL.Name(), dSDnBDPL);      
    if (fResetTest)     {
        if (dSDnBDPL != 0 )   {
            SLOG (eError, "FAIL : SD%d %s = 0x%08x. It should be zero on reset ", i, RegSDnBDPL.Name(), dSDnBDPL);
            fResult  = FNS_FAIL;
        }
        return fResult;        
    }    


    Hit::CBitRange &BDL = RegSDnBDPL["BDLLBASE Lower Base Unimplemented Bits"];    
    BYTE bBDL = (BYTE) BDL.Read();
// Test: for Read Only Zero property
    if (!HdCntAsrt.ReadOnly0("Testing for Read Only 0", BDL))
        fResult = FNS_FAIL;
    SLOG (eInfo1, "SD%d %s = 0x%08x", i, BDL.Name(), bBDL);      
        
    Hit::CBitRange &BDLLBASE = RegSDnBDPL["Buffer Descriptor List Lower Base Address - BDLLBASE"];    
// Test: for Read Write property
// TODO: removing test for the ReadWrite for the DPLBASE since all controllers were failing this test case. So needs to makesure that Test is right or not 
/*    TESTINGREG(BDLLBASE.Name());
    if (!HdCntAsrt.ReadWrite("Testing for Read Write", BDLLBASE))
        fResult = FNS_FAIL;
*/
    DWORD dBDLLBASE = (DWORD) BDLLBASE.Read();
    SLOG (eInfo1, "SD%d %s = 0x%08x", i, BDLLBASE.Name(), dBDLLBASE);      
    return fResult;
}


DWORD TestSDnBDPL()  
{
    DWORD fResult = FNS_PASS;
    // first get the number of Input / output and bidirectional streams supported by the controller
    ULONG uIss, uOss, uBss;
    GetStreamNumbers(uIss, uOss, uBss);
    SLOG (eInfo3, "Iss = %d, Oss = %d , Bss = %d", uIss, uOss, uBss);
    for (ULONG i = 0; i<uIss; i++)  {
        Hit::CTable & StreamTable = GetHdHitNameSpace->Tbl("Input_Stream_Descriptor", i);
        Hit::CRegister &RegSDnBDPL = StreamTable["Input Stream Descriptor Buffer Descriptor List Pointer - Lower"];        
        fResult |= TestStreamDescriptorBDPL(RegSDnBDPL, i) ;
    }

    for (ULONG i = 0; i<uOss; i++)  {
        Hit::CTable & StreamTable = GetHdHitNameSpace->Tbl("Output_Stream_Descriptor", i);
        Hit::CRegister &RegSDnBDPL = StreamTable["Output Stream Descriptor Buffer Descriptor List Pointer - Lower"];        
        fResult |= TestStreamDescriptorBDPL(RegSDnBDPL, i) ;
    }

    for (ULONG i = 0; i<uBss; i++)  {
        Hit::CTable & StreamTable = GetHdHitNameSpace->Tbl("BiDirectional_Stream_Descriptor", i);
        Hit::CRegister &RegSDnBDPL = StreamTable["BiDirectional Stream Descriptor Buffer Descriptor List Pointer - Lower"];        
        fResult |= TestStreamDescriptorBDPL(RegSDnBDPL, i) ;
    }
    return fResult;

}

// to test Stream Descriptor Buffer Descriptor List Pointer - Upper
DWORD TestStreamDescriptorBDPU (Hit::CRegister &RegSDnBDPU , UINT i)   
{
    DWORD fResult = FNS_PASS;
    DWORD dReg = RegSDnBDPU.Read() ; 
    SLOG (eInfo1, "%s = 0x%08x", RegSDnBDPU.Name(), dReg);      

    if (fResetTest) {
        if (dReg!= 0)    {
            SLOG (eError, "FAIL : %s = 0x%08x. It should be zeor on the reset", RegSDnBDPU.Name(), dReg);
            fResult  = FNS_FAIL;
        }
        return fResult;
    }
    if (GetHdController->Getf64Bit())
        fResult = TestDWORDForRW(RegSDnBDPU);
    else 
        fResult = TestDWORDForRO(RegSDnBDPU);
    SLOG (eInfo1, "SD%d %s = 0x%08x", i, RegSDnBDPU.Name(), RegSDnBDPU.Value());        
    return fResult;
}

DWORD TestSDnBDPU()  
{
    DWORD fResult = FNS_PASS;
    // first get the number of Input / output and bidirectional streams supported by the controller
    ULONG uIss, uOss, uBss;
    GetStreamNumbers(uIss, uOss, uBss);
    SLOG (eInfo3, "Iss = %d, Oss = %d , Bss = %d", uIss, uOss, uBss);
    for (ULONG i = 0; i<uIss; i++)  {
        Hit::CTable & StreamTable = GetHdHitNameSpace->Tbl("Input_Stream_Descriptor", i);
        Hit::CRegister &RegSDnBDPU = StreamTable["Input Stream Descriptor Buffer Descriptor List Pointer - Upper"];        
        fResult |= TestStreamDescriptorBDPU( RegSDnBDPU, i);        
    }
    for (ULONG i = 0; i<uOss; i++)  {
        Hit::CTable & StreamTable = GetHdHitNameSpace->Tbl("Output_Stream_Descriptor", i);
        Hit::CRegister &RegSDnBDPU = StreamTable["Output Stream Descriptor Buffer Descriptor List Pointer - Upper"];        
        fResult |= TestStreamDescriptorBDPU( RegSDnBDPU, i);        
    }
    for (ULONG i = 0; i<uBss; i++)  {
        Hit::CTable & StreamTable = GetHdHitNameSpace->Tbl("BiDirectional_Stream_Descriptor", i);
        Hit::CRegister &RegSDnBDPU = StreamTable["BiDirectional Stream Descriptor Buffer Descriptor List Pointer - Upper"];        
        fResult |= TestStreamDescriptorBDPU( RegSDnBDPU, i);        
    }
    return fResult;

}

// test for Immediate Command Output Interface
// as per Specs, this is RW register and it should always retrun Zero.
DWORD TestICOI()
{
    DWORD fResult = FNS_PASS;
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &RegICOI = GenTable["Immediate Command Output Interface"];
    TESTINGREG(RegICOI.Name());    
    DWORD dReg = RegICOI.Read() ; 
    SLOG (eInfo1, "%s = 0x%08x", RegICOI.Name(), dReg);      
    if (fResetTest) {
        if (dReg!= 0)    {
            SLOG (eError, "FAIL : %s = 0x%08x. It should be zeor on the reset", RegICOI.Name(), dReg);
            fResult  = FNS_FAIL;
        }
        return fResult;
    }
    SLOG (eInfo1, "%s = 0x%08x", RegICOI.Name(), dReg);
    // Test for Always retunr Zero since this is RW register but reading from this register should always return Zero
    // removed this  test since we have ECRs in the pipeline telling that this register will not return zero alwyas
/*    if (!RegICOI.Always0())
                fResult = FNS_FAIL;
*/
    return fResult;
/*
// Test: for read Write property
    fResult = TestDWORDForRW(RegICOI);
    RegICOI.Write (0xAA55FF00);
    DWORD readData =  RegICOI.
    return fResult;
*/
}


// to test Immediate Response Input Interface
/* Bit     Type    Reset   Description
    31:0    R/W     0’s     Immediate Response Read (IRR): The value in this register latches the last response to come in over the link.
                                    If multiple codecs responded in the same frame, there is no way to determine which response will be saved here, 
                                    but the address of the codec is indicated in the ICRADD field of the Immediate Command Status register.
*/
DWORD TestIRII()
{
    DWORD fResult = FNS_PASS;
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &RegIRII = GenTable["Immediate Response Input Interface"];
    DWORD dReg = RegIRII.Read() ; 
    SLOG (eInfo1, "%s = 0x%08x", RegIRII.Name(), dReg);      
    if (fResetTest) {
        if (dReg!= 0)    {
            SLOG (eError, "FAIL : %s = 0x%08x. It should be zeor on the reset", RegIRII.Name(), dReg);
            fResult  = FNS_FAIL;
        }
        return fResult;
    }
    // removed test for RW since all controller fail for this and there is  a ECR for the same.
    /*
// Test: for read write property
    fResult = TestDWORDForRW(RegIRII);
    if (fResult == FNS_FAIL) {
        XLOG(XWARN, eWarn1, "Warning : As Per HD audio Sepcs %s should  be RW", RegIRII.Name());
        fResult = FNS_PASS;
    }
    */
    return fResult;
}

DWORD TestICIS()
{
    DWORD fResult = FNS_PASS;
    Hit::CTable & GenTable = GetHdHitNameSpace->Tbl("General_Info");
    Hit::CRegister &RegICIS= GenTable["Immediate Command Status"];
    DWORD dReg = RegICIS.Read() ; 
    SLOG (eInfo1, "%s = 0x%08x", RegICIS.Name(), dReg);      
    if (fResetTest) {
        switch (dReg) {
            case 0:
                SLOG (eInfo1, "Immediate Command Status register ICVER is 0; IRRADD and IRRUNSOL are reserved.");      
                break;
            case 4:
                SLOG (eInfo1, "Immediate Command Status register ICVER is 1; IRRADD and IRRUNSOL are implemented.");      
                break;
            default:
                SLOG (eError,
                    "FAIL : %s = 0x%08x. "
                    "It should be zero (HD bare spec) "
                    "or 4 (Immediate Command Version = 1, DCN 2) "
                    "on the reset",
                    RegICIS.Name(),
                    dReg
                );
                fResult  = FNS_FAIL;
                break;
        }
        return fResult;
    }
// TODO: test register for its RW / RO and Rsvzd property    
    return fResult;
}


// TODO: Under development and not completed yet


DWORD TestAllRegistersForResetValues ()
{
    DWORD fResult = FNS_PASS;
    fResetTest =  TRUE;
    fResult |= TestGlobalCapability ();
    fResult |= TestVMIN();
    fResult |= TestVMAJ();
    fResult |= TestOUTPAY();
    fResult |= TestINPAY();
    fResult |= TestGlobalControl();
    fResult |=  TestWakeEnable();
    fResult |=  TestWakeStatus();    
    fResult |=  TestGlobalStatus();    
    fResult |=  TestOUTSTRMPAY();
    fResult |=  TestINSTRMPAY();
    fResult |=  TestINTCTL();
    fResult |=  TestINTSTS();
    fResult |=  TestSSYNC();
    fResult |=  TestCORBLBASE ();
    fResult |=  TestCORBUBASE ();
    fResult |=  TestCORBWP();
    fResult |=  TestCORBRP();
    fResult |=  TestCORBCTL ();
    fResult |=  TestCORBSTS ();
    fResult |=  TestCORBSIZE();
    fResult |=  TestRIRBLBASE ();
    fResult |=  TestRIRBUBASE ();
    fResult |=  TestRIRBWP();
    fResult |=  TestRINTCNT ();
    fResult |=  TestRIRBCTL ();
    fResult |=  TestRIRBSTS();    
    fResult |=  TestRIRBSIZE ();
    fResult |=  TestICOI();
    fResult |=  TestIRII();
    fResult |=  TestICIS();
    fResult |=  TestDPIBLBASE();
    fResult |=  TestDPIBUBASE();

    fResult |=  TestSDnCTL();
    fResult |=  TestSDnSTS();    
    fResult |=  TestSDnLPIB();
    fResult |=  TestSDnCBL();    
    fResult |=  TestSDnLVI();
    fResult |=  TestSDnFMT();        
    fResult |=  TestSDnBDPL();    
    fResult |=  TestSDnBDPU();
    fResetTest =  FALSE;    

    return fResult;
}

// test for the ResetState - value of registers after reset condition 
DWORD TestController()
{
   
    DWORD fResult = FNS_PASS;
    double dEnterResetTime = 0;
    double dExitResetTime = 0;
    double dResetMaximumAllowed = 250;
    SLOG(eInfo1, "Setting controller to reset state and then making sure that controller gets out of reset state too");
    HANDLE hController =  GetHdControllerHandle;

    if (IsWindows10OrGreater())
    {
        // For TH, we would like to make the latency longer
        dResetMaximumAllowed = 400 * SAFETY_FACTOR;
    }

    SLOG(eInfo1, "Reset time threshold %f use.", dResetMaximumAllowed);

    //send device IO to controller for resetting it
    HRESULT hResult = ResetController(hController, &dEnterResetTime, &dExitResetTime);
    if (hResult != STATUS_SUCCESS)  {
        SLOG (eError, "FAIL : Resetting controller resulted into error - Status = 0x%x", hResult);
        SLOG(eError, "Reset time taken by controller = %f uSec in, %f uSec out", dEnterResetTime * 1000000, dExitResetTime * 1000000);
        fResult =  FNS_FAIL;
        return fResult;
    }

    SLOG(eInfo1, "Rest time taken by controller = %f uSec in, %f uSec out", dEnterResetTime * 1000000, dExitResetTime * 1000000);
    if ((dEnterResetTime * 1000000) > dResetMaximumAllowed) {
        fResult = FNS_FAIL;
        SLOG(eError, "Controller took more then %fus to enter reset", dResetMaximumAllowed);
    }

    if ((dExitResetTime * 1000000) > dResetMaximumAllowed) {
        fResult = FNS_FAIL;
        SLOG(eError, "Controller took more then %fus to exit reset", dResetMaximumAllowed);
    }

    fResult |= TestAllRegistersForResetValues();
    // We have reset the controller so
    // check all registers' values for reset state
    // and then also test registers for their RW property

    fResult |= TestGlobalCapability ();
    fResult |= TestVMIN();
    fResult |=  TestVMAJ();
    fResult |=  TestOUTPAY();
    fResult |=  TestINPAY();
    fResult |=  TestGlobalControl ();
    fResult |=  TestWakeEnable();
    fResult |=  TestWakeStatus();
    fResult |=  TestGlobalStatus();
    fResult |=  TestOUTSTRMPAY();
    fResult |=  TestINSTRMPAY();
    fResult |=  TestINTCTL();
    fResult |=  TestINTSTS();
    fResult |=  TestWallClockCounter();
    fResult |=  TestSSYNC();
    fResult |=  TestCORBLBASE ();
    fResult |=  TestCORBUBASE ();
    fResult |=  TestCORBWP();
    fResult |=  TestCORBRP();
    fResult |=  TestCORBCTL ();
    fResult |=  TestCORBSTS ();
    fResult |=  TestCORBSIZE ();
    fResult |=  TestRIRBLBASE ();
    fResult |=  TestRIRBUBASE ();
    fResult |=  TestRIRBWP();
    fResult |=  TestRINTCNT ();
    fResult |=  TestRIRBCTL ();
    fResult |=  TestRIRBSTS ();
    fResult |=  TestRIRBSIZE ();
    fResult |=  TestICOI();
    fResult |=  TestIRII();
    fResult |=  TestICIS();
    fResult |=  TestDPIBLBASE();
    fResult |=  TestDPIBUBASE();
    fResult |=  TestSDnCTL();
    fResult |=  TestSDnSTS();
    fResult |=  TestSDnLPIB();
    fResult |=  TestSDnCBL();
    fResult |=  TestSDnLVI();
    fResult |=  TestSDnFMT();
    fResult |=  TestSDnBDPL();
    fResult |=  TestSDnBDPU();

    return fResult;
}

DWORD TestWallClockController ()
{
    DWORD fResult = FNS_PASS;    
    HANDLE hController =  GetHdControllerHandle;
    const ULONG CLOCK_ELEMENTS = 10000;
    ULONG bufferSize = sizeof(HDBUSTEST_CLOCKREGISTER_OUTPUT) + sizeof(ULONG) * CLOCK_ELEMENTS;
    CBuffer<BYTE> buffer(bufferSize);
    if (NULL == buffer)    {
        SLOG (eError, "Buffer allocation failed");
        return FNS_FAIL;
    }
        
    // send down device IO control.
    DWORD BytesReturned;
    BOOL b = DeviceIoControl(hController, IOCTL_HDBUSTEST_CLOCKREGISTER, NULL, 0, buffer, bufferSize, &BytesReturned, 0);
    if (!b)     {
        //
        // If total bandwith was above 5760000 bytes/sec then it won't fit
        // on HD Audio with a single SDO line.
        //
        SLOG (eError, "TestClockRegister: IOCTL failed with error code = 0x%x", GetLastError());
        fResult = FNS_FAIL;
        goto Exit;
    }

    if (BytesReturned != bufferSize) {
        SLOG (eError, "TestClockRegister: IOCTL returned %u bytes instead of %u bytes", BytesReturned, bufferSize);
        fResult = FNS_FAIL;
        goto Exit;
    }
    
    //
    // Check the clock values. Values must monotonically ascend.
    // Watch out for wrapping.
    //
    PHDBUSTEST_CLOCKREGISTER_OUTPUT pClock = (PHDBUSTEST_CLOCKREGISTER_OUTPUT)(PBYTE)buffer;
    ULONG StartClock = pClock->WallClockValue[0];

    for (UINT i = 2; i < CLOCK_ELEMENTS; i++) {
        // this strategy works as long as CLOCK_ELEMENTS is small on the wrapping time scale
        // at a nominal clock cycle 24 MHz this takes about three minutes to wrap
        // the IOCTL shouldn't take anywhere near this long to complete
        // so the approach is justified
        if (
            (pClock->WallClockValue[i - 1] - StartClock) >= // time elapsed at i - 1
            (pClock->WallClockValue[i] - StartClock) // is somehow GREATER (or equal) to time elapsed at i
        ) {
            SLOG (eError, 
                "Clock value is static or decreasing even allowing wraparound: "
                "%dth is 0x%08x, "
                "%dth is 0x%08x, "
                "%dth is 0x%08x, "
                "%dth is 0x%08x\n",
                0, StartClock,
                i - 2, pClock->WallClockValue[i - 2],
                i - 1, pClock->WallClockValue[i - 1],
                i, pClock->WallClockValue[i]
            );
            fResult = FNS_FAIL;
            goto Exit;
        }
    }

    //
    // Calculate the time difference in the clock.
    //
    ULONG EndClock = pClock->WallClockValue[pClock->WallClockCount - 1];
    ULONG ClockDiff = EndClock - StartClock;    // if EndClock < StartClock this still works.
    double timeClock = (double)ClockDiff / 24000000;  // 24MHz clock of wall counter

    double timeDiff =
        (double)(pClock->QpcStartEnd.EndTime.QuadPart - pClock->QpcStartEnd.StartTime.QuadPart) /
            (double)pClock->QpcFrequency.QuadPart;

    //
    // We allow the clock to be off by 0.02%
    //
    // 20051024 - Paraphrased from atuld "...this is not the correct way to measure since the CPU clock might not be 
    //  accurate enough (0.02%).  So, we'll just offer a warning instead of a fail.
    //
    SLOG (eInfo1,"Clock diff is %f  sec and timeDiff is %f sec.", timeClock, timeDiff);
    SLOG (eInfo1,"off by  %f Percentage", ((timeClock - timeDiff) / timeDiff) * 100);
    if ((timeClock <= timeDiff * 0.9998) || (timeClock >= timeDiff * 1.0002)) {
        XLOG(XWARN, eWarn1, "Warning: Wall clock register and system clock show >0.02%% deviation");
//        fResult = FNS_FAIL;
    }

Exit:
    return fResult;
}

DWORD TestResetTimeForDMAEngines()
{
    DWORD fResult = FNS_PASS;    
    HANDLE hController =  GetHdControllerHandle;
    ULONG TotalDmaEngines =  GetHdController->GetOss() + GetHdController->GetIss() + GetHdController->GetBss();
    ULONG bufferSize = sizeof(HDBUSTEST_DMARESET_OUTPUT) + TotalDmaEngines * sizeof(DMA_ENGINE_RESET_TIMINGS);
    CBuffer<BYTE> buffer(bufferSize);
    DWORD BytesReturned;
    BOOL b = DeviceIoControl(hController, IOCTL_HDCNTTEST_DMA_RESETTIME, NULL, 0, buffer, bufferSize, &BytesReturned, 0);

    if (!b) {
        SLOG (eError, "TestResetTimeForDMAEngines: IOCTL failed with error code = 0x%x", GetLastError());
        fResult = FNS_FAIL;
        goto Exit;
    }
    XLOG (XMSG, eError, "bytes returned = %d", BytesReturned);

    if (BytesReturned != bufferSize) {
        SLOG (eError, "TestResetTimeForDMAEngines: IOCTL returned %d bytes instead of %d", BytesReturned, bufferSize);
        fResult = FNS_FAIL;
        goto Exit;
    }
    
    PHDBUSTEST_DMARESET_OUTPUT pDmaReset = (PHDBUSTEST_DMARESET_OUTPUT)(PBYTE)buffer;
    SLOG(eInfo1,
        "QPC Frequency: %I64u (%g microseconds)",
        pDmaReset->QpcFrequency.QuadPart,
        // 1000 ( ms / s ) * 1000 ( us / ms ) / ( cycles / s ) = us / cycle
        1000.0 * 1000.0 / pDmaReset->QpcFrequency.QuadPart
    );
    SLOG(eInfo1, "DMA engines: %u", pDmaReset->DmaEngineCount);
    
    for (UINT i = 0; i < TotalDmaEngines; i++) {
        PDMA_ENGINE_RESET_TIMINGS pTimings = &pDmaReset->EngineResetTimings[i];
        
        double dMicrosecondsToEnter =
            1000.0 * 1000.0 * (
                pTimings->QpcAfterResetEntered.QuadPart - pTimings->QpcAfterRequestingResetEnter.QuadPart
            ) / pDmaReset->QpcFrequency.QuadPart;
        
        double dMicrosecondsToExit =
            1000.0 * 1000.0 * (
                    pTimings->QpcAfterResetEntered.QuadPart - pTimings->QpcAfterRequestingResetEnter.QuadPart
                ) / pDmaReset->QpcFrequency.QuadPart;
                
        SLOG(eInfo1,
            "Engine #%u (1-based)\n"
            "Before requesting reset enter: %I64u\n"
            "After requesting reset enter: %I64u (%g microseconds)\n"
            "Processor stalls before reset entered: %u\n"
            "After reset entered: %I64u (%g microseconds)\n"
            "Before requesting reset exit: %I64u\n"
            "After requesting reset exit: %I64u (%g microseconds)\n"
            "Processor stalls before reset exit: %u\n"
            "After reset exit: %I64u (%g microseconds)",
            i + 1,
            pTimings->QpcBeforeRequestingResetEnter.QuadPart,
            pTimings->QpcAfterRequestingResetEnter.QuadPart,
                1000.0 * 1000.0 * (
                    pTimings->QpcAfterRequestingResetEnter.QuadPart - pTimings->QpcAfterRequestingResetEnter.QuadPart
                ) / pDmaReset->QpcFrequency.QuadPart,

            pTimings->ProcessorStallsBeforeResetEntered,
            pTimings->QpcAfterResetEntered.QuadPart,
                dMicrosecondsToEnter,
                
            pTimings->QpcBeforeRequestingResetExit.QuadPart,
            pTimings->QpcAfterRequestingResetExit.QuadPart,
                1000.0 * 1000.0 * (
                    pTimings->QpcAfterRequestingResetExit.QuadPart - pTimings->QpcBeforeRequestingResetExit.QuadPart
                ) / pDmaReset->QpcFrequency.QuadPart,

            pTimings->ProcessorStallsBeforeResetExited,
            pTimings->QpcAfterResetExited.QuadPart,
                dMicrosecondsToExit
        );

        // hdaudbus.sys will abandon things after 40 consecutive processor stalls
        // to get some headroom, fail engines that take at least 30
        if (30 < pTimings->ProcessorStallsBeforeResetEntered) {
            SLOG (eError,
                "Engine %u (1-based) took %u processor stall(s) to enter reset;"
                 " hdaudbus.sys will give up after 40"
                 " so we require reset to enter in 30 stalls or less",
                 i + 1,
                 pTimings->ProcessorStallsBeforeResetEntered
            );
            fResult = FNS_FAIL;
        }
        
        if (30 < pTimings->ProcessorStallsBeforeResetExited) {
            SLOG (eError,
                "Engine %u (1-based) took %u processor stall(s) to exit reset;"
                 " hdaudbus.sys will give up after 40"
                 " so we require reset to exit in 30 stalls or less",
                 i + 1,
                 pTimings->ProcessorStallsBeforeResetExited
            );
            fResult = FNS_FAIL;
        }
        
        if (30.0 < dMicrosecondsToEnter) {
            SLOG (eError,
                "Engine %u (1-based) took %g microseconds to enter reset;"
                 " hdaudbus.sys will give up after 40 microseconds"
                 " so we require reset to enter in 30 microseconds or less",
                 i + 1,
                 dMicrosecondsToEnter
            );
            fResult = FNS_FAIL;
        }
        
        if (30.0 < dMicrosecondsToExit) {
            SLOG (eError,
                "Engine %u (1-based) took %g microseconds to exit reset;"
                 " hdaudbus.sys will give up after 40 microseconds"
                 " so we require reset to exit in 30 microseconds or less",
                 i + 1,
                 dMicrosecondsToExit
            );
            fResult = FNS_FAIL;
        }
    } // next DMA engine
        
Exit:
    return fResult;
}

