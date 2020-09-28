/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/

#ifndef CONTROLLER_H
#define CONTROLLER_H

#define STRSAFE_NO_DEPRECATE

#ifndef UAATEST_H
#error controller.h requires uaatest.h
#endif

#ifndef _WINDOWS_
#error controller.h requires windows.h
#endif

#ifndef __HIT_DEFINE_FRAMEWORK_H__
#error controller.h requires HitFramework.h
#endif

#ifndef HDREGTEST_H
#error controller.h requires hdregtest.h
#endif

#ifndef __HDHITLOG_H__
#error controller.h requires hdhitlog.h
#endif

#ifndef _PCI_HCT_PLUG_IN_H_
#error controller.h requires hdhitplugin.h
#endif

#ifndef _PCI_EXPRESS_CONFIG_SPACE_H_
#error controller.h requires hdaudiomemoryspace.h
#endif

extern fResetTest;
//extern CUAATestModule* g_pUAATest;
extern Hit::ILog * Log;                                                     // Must initialize before using!!
//extern Hit::CNamespace HdAudioCntroller;

#define TESTINGCAP(CapName) Log->Info ("Testing %s", CapName);
#define TESTINGICAP(CapName, Index) Log->Info ("Testing %s #%d", CapName, Index);

#define TESTINGREG(RegName) Log->Note ("Testing %s", RegName);
#define TESTINGIREG(RegName, Index) Log->Note ("Testing %s #%d", RegName, Index);

#define EXCEPTION(ObjName) Log->Error (LOCATION, "An exception occurred while testing %s", ObjName);
#define NOTFOUND Log->Info ("Not found");

#define HdCntAsrt ((CHdController*)(g_pUAATest->m_pCurDevice))->Asrt
#define GetHdControllerHandle ((CHdController*)(g_pUAATest->m_pCurDevice))->GetHandle()
#define GetHdHitNameSpace ((CHdController*)(g_pUAATest->m_pCurDevice))->pHdAudioCntrollerNameSpace
#define GetHdController ((CHdController*)(g_pUAATest->m_pCurDevice))

#define VMINValue    0x00
#define VMAJValue    0x01
#define ULONG_MAX 0xFFFFFFFF

// Controller definition
class CHdController : public CDeviceDescriptor
{
   HANDLE hController ;	// Handle to controller
   ULONG uOSS;       // number of Output Streams Supported
   ULONG uISS;       // number of Input Streams Supported
   ULONG uBSS;       // number of Bidirectional Streams Supported   
   ULONG uNSDO;     //number of Serial Data Out Singnals supported
   BOOL f64bit;         // flag to tell that 64bit address supported or not - 1 means it supprts 64 bit addressing. 0 means that it does not suppot 64 bit addressing
public:
   Hit::CAssertion  Asrt;	                                                    // Must initialize before using!!
   Hit::CNamespace* pHdAudioCntrollerNameSpace;
   CHdController(HANDLE hControllerInterface);
   ~CHdController();
   HANDLE GetHandle () {return hController;}
   ULONG GetOss () {return uOSS;}
   ULONG GetIss () {return uISS;}
   ULONG GetBss () {return uBSS;}
   BOOL Getf64Bit () {return f64bit;}   

};

#endif // CONTROLLER_H
