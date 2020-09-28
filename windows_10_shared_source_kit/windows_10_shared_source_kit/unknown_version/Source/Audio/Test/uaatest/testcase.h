/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/

#ifndef _TESTCASE_H
#define _TESTCASE_H

#ifndef _shell98_h
#error testcase.h requires shell98.h
#endif

#ifndef _MODULE98_H
#error testcase.h requires module98.h
#endif

#ifndef _DVMODULE_H
#error testcase.h requires dvmodule.h
#endif

#ifndef CODEC_H
#error testcase.h requires codec.h
#endif

#ifndef __UTIL_H__
#error testcase.h requires util.h
#endif

//
// Test case table
//
/* For reference
typedef struct dv_dvinfo_tag
{
	LPSTR       pszCaseID;      // shell    - test case ID
    LPSTR       pszName;        // shell    - test case name
    LPARAM      lpCaseFlags;    // shell    - test case flags
    LPARAM      lpDeviceFlags;  // shell    - device type flags
    LPARAM      lpSTFlags;      // selftest - server/cable/input flags
    LPARAM      lpUser;         // User-defined field
    TESTFNPROC  pfnTest;        // shell    - Pointer to the test case function
    LPSTR       pszGuid;        // shell    - test case GUID identifier
} DV_TCINFO, *PDV_TCINFO;
*/

DV_TCINFO rgTestCaseInfo[] =
{
    { "",       "Codec Tests",                  0, DT_CODEC, 0, 0,  NULL,                       "{5D14A5A9-6EFD-4f6c-A2C3-B9DC6425BD1B}"}, 
    { "1",      " RequiredParameterSupport",    0, DT_CODEC, 0, 0,  RequiredParameterSupport,   "{93730492-00BF-4ce2-ABC4-8DE6C2D2045A}"},
    { "2",      " RequiredVerbSupport",         0, DT_CODEC, 0, 0,  RequiredVerbSupport,        "{A76E5D58-379D-475e-AE07-B484B2E2EDE8}"},
    { "3",      " FunctionGroupReset",          0, DT_CODEC, 0, 0,  FGReset,                    "{C8C645D2-ED0D-4188-BE4B-125429F8697B}"},
    { "4",      " ValidatePinConfigs",          0, DT_CODEC, 0, 0,  ValidatePinConfigs,         "{4736BC9D-DFB1-4d98-A06B-7FA9C0E87079}"},
    { "5",      " ValidateHDMI",                0, DT_CODEC, 0, 0,  ValidateHDMI,               "{BECDE2FD-6257-4174-BC7D-AC7E3D1AE97D}"},
    { "",       "Controller Tests",             0, DT_CTRL,  0, 0,  NULL,                       "{823EA50E-18D1-48c2-AF07-DFEED0420245}"}, 
        { "21", " Testing controller's registers for read Write property",TESTFLAG_MAINTHREAD, DT_CTRL, 0, 0, TestController, "{1E9DE834-2B62-436d-938B-E3F9C64761D6}"},
        { "22", " Testing controller's Wall clock register",TESTFLAG_MAINTHREAD, DT_CTRL, 0, 0, TestWallClockController, "{19EED834-2B62-436d-938B-E3F9C64761D6}"},
        { "23", " Testing controller's DMA Reset Timing",TESTFLAG_MAINTHREAD, DT_CTRL, 0, 0, TestResetTimeForDMAEngines, "{B182FE5F-FC09-4658-B655-A9737E9F9E49}"},
//    { "21",     " TBD",                         0, DT_CTRL,  0, 0,  NULL,                       "{D4FDD6AE-01C6-46a0-96FB-C2EA95F9713C}"}, 
#ifndef EXTERNAL
    { "",       "Audio Function Driver Tests",  0, DT_AFD,   0, 0,  NULL,                       "{5EC80B92-EC02-4e2d-B936-77323B2AC613}"}, 
#if 0
#if (NTDDI_VERSION < NTDDI_VISTA)
    { "41",     " Policy IOCTLs",               0, DT_AFD,   0, 0,  PolicyIOCTLs,               "{5D992C59-33C8-4cb2-A3DF-186B5BB232B3}"}, 
    { "42",     " Invalid Policy IOCTLs",       0, DT_AFD,   0, 0,  NULL,                       "{E7FAFF2F-0327-4ede-A175-AE73662AF092}"}, 
#endif // #if (NTDDI_VERSION < NTDDI_VISTA)
#endif // #if 0
    { "",       "Feature Support",              0, DT_GEN,   0, 0,  NULL,                       "{0D70EC3F-9190-4ae2-BE19-A151EAC1F71E}"}, 
    // Can streaming tests be covered by wave-filter testing??
    //{ "61",     " Streaming formats",           0, DT_AFD,   0, 0,  NULL,                       "{E5C51D85-941D-4233-B3E5-D1AD961735C9}"}, 
    { "62",     " Jack-retasking",              0, DT_AFD,   0, 0,  NULL,                       "{9B2FBC2F-2FC2-4be2-9CE0-51688AFDCFE3}"}, 
    { "63",     " Independent streaming",       0, DT_GEN,   0, 0,  NULL,                       "{862C0754-5ADB-4e29-A474-36302E8ABDD7}"}, 
    { "64",     " Multi-streaming",             0, DT_GEN,   0, 0,  NULL,                       "{673BDD33-64A5-4047-BE44-58F036E51B49}"},     
#endif // #ifndef EXTERNAL
#ifndef WHQL
    { "",       "Misc Tests/Utilities",         0, DT_GEN,   0, 0,  NULL,                       "{5D14A5A9-6EFD-4f6c-A2C3-B9DC6425BD1B}"}, 
    { "100",    " Dump Codec",                  0, DT_CODEC, 0, 0,  CollectAFGInfo,             "{00C35236-0A14-4cdd-969A-2C96CBAB70DE}"},     
#endif // #ifndef WHQL
};

#endif // _TESTCASE_H
