// Copyright(C) Microsoft. All rights reserved.//

// AudioEngineTest.cpp
//
// Description:
//
//  Shell98 module implementation
//

#include "stdafx.h"

#ifdef BUILD_TAEF

#include <basicloghelper.h>

BEGIN_MODULE()
    MODULE_PROPERTY(L"Feature", L"Core Audio - UMACore")
END_MODULE()

HRESULT hrModuleCoInit = E_FAIL;

MODULE_SETUP(SetupSuite);
MODULE_CLEANUP(CleanupSuite);

bool SetupSuite()
{
    hrModuleCoInit = VERIFY_SUCCEEDED_RETURN(
        CoInitializeEx(NULL, COINIT_MULTITHREADED)
        );

    return true;
}

bool CleanupSuite()
{
    if(SUCCEEDED(hrModuleCoInit))
    {
        CoUninitialize();
    }

    return true;
}

#define ENABLE_HLK_CERTIFICATION() \
    BEGIN_TEST_METHOD_PROPERTIES() \
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification") \
    END_TEST_METHOD_PROPERTIES()
#else
#define ENABLE_HLK_CERTIFICATION()
#endif


//
// Appplication name to fill shell98 variables
LPSTR gszModuleName = "Audio Engine API Test";


#include "guid.cpp"


BOOL g_bEnumerateByEndpoints = TRUE;

//
// Globals
//
//ITestShell*             g_IShell;
//CAudioEngineModule*     g_pModule;

//typedef struct dv_dvinfo_tag
//{
//    LPTSTR       pszCaseID;      // shell    - test case ID
//    LPTSTR       pszName;        // shell    - test case name
//    LPARAM      lpCaseFlags;    // shell    - test case flags
//    LPARAM      lpDeviceFlags;  // shell    - device type flags
//    LPARAM      lpSTFlags;      // selftest - server/cable/input flags
//    LPARAM      lpUser;         // User-defined field
//    TESTFNPROC  pfnTest;        // shell    - Pointer to the test case function
//    LPTSTR       pszGuid;        // shell    - test case GUID identifier
//} DV_TCINFO, *PDV_TCINFO;

BEGIN_TEST_CASE_INFO(UMACore, CAudioEngineModule)
	DEVICE_TEST_PARENT("",           "Audio Engine Test",                                                              0, 0,                                          0, NULL, NULL,                                                                 "{08964923-4a2d-4b8a-90d9-66d1e8b005e1}")
    DEVICE_TEST_PARENT("",           " API Test",                                                                      0, 0,                                          0, NULL, NULL,                                                                 "{f079b901-b92f-423e-b147-d3078405152e}")
    DEVICE_TEST_PARENT("",           "  Processor",                                                                    0, 0,                                          0, NULL, NULL,                                                                 "{a8233b2b-e9fc-43cc-89a2-f2a8ea2c06c4}")
    DEVICE_TEST_METHOD("100.1",      "   Object registered",                                                           0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_Processor_ObjectCreate,                                          "")
    DEVICE_TEST_PARENT("",           "   Connection",                                                                  0, 0,                                          0, NULL, NULL,                                                                 "{bca660a1-32bd-4752-822a-e81419d11cbd}")
    DEVICE_TEST_METHOD("100.2.1",    "    Create  invalid values",                                                     0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_Processor_ConnectionCreateInvalidValues,                         "")
    DEVICE_TEST_METHOD("100.2.2",    "    Create  invalid param",                                                      0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_Processor_ConnectionCreateInvalidParams,                         "")
    DEVICE_TEST_METHOD("100.2.3",    "    Release invalid connection",                                                 0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_Processor_ConnectionReleaseInvalid,                              "")
    DEVICE_TEST_METHOD("100.2.6",    "    Attach connection endpoint",                                                 0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_Processor_AttachEndpointTest,                                    "")
    DEVICE_TEST_METHOD("100.2.7",    "    Remove APO connection",                                                      0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_Processor_RemoveApoConnection,                                   "")
    DEVICE_TEST_METHOD("100.2.9",    "    Swap connection ",                                                           0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_Processor_SwapConnectionTest,                                    "")
    DEVICE_TEST_PARENT("",           "   APO",                                                                         0, 0,                                          0, NULL, NULL,                                                                 "{1d60ec0f-4a5c-4d18-b4e9-bf28af0b2361}")
    DEVICE_TEST_METHOD("100.3.1",    "    Can instantiate all system APOs",                                            0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_Processor_CreateAllApos,                                         "")
    DEVICE_TEST_METHOD("100.3.2",    "    Release APO invalid",                                                        0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_Processor_ReleaseApoInvalid,                                     "")
    DEVICE_TEST_METHOD("100.3.3",    "    Attach connection valid",                                                    0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_Processor_AttachApoConnectionValid,                              "")
    DEVICE_TEST_METHOD("100.3.4",    "    Release APO with attached connection",                                       0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_Processor_ReleaseAPOWithAttachedConnection,                      "")
    DEVICE_TEST_METHOD("100.3.5",    "    Attach connection invalid",                                                  0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_Processor_AttachApoConnectionInvalid,                            "")
    DEVICE_TEST_METHOD("100.3.6",    "    Activate APO test",                                                          0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_Processor_ActivateApoTest,                                       "")
    DEVICE_TEST_PARENT("",           "   SRC APO",                                                                     0, 0,                                          0, NULL, NULL,                                                                 "{60bb1e77-2d4c-4fc4-92b4-96edcdf77e3b}")
    DEVICE_TEST_METHOD("100.4.1",    "    Invalid flags",                                                              0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_SRCAPO_InitializeInvalid,                                        "")
    DEVICE_TEST_METHOD("100.4.2",    "    Calculate max in/out frames",                                                0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_SRCAPO_CalculateFrames,                                          "")
    DEVICE_TEST_METHOD("100.4.3",    "    CalcInputFrames state testing",                                              0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_SRCAPO_CalcInputFramesStateTesting,                              "")
    DEVICE_TEST_METHOD("100.4.4",    "    CalcOutputFrames state testing",                                             0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_SRCAPO_CalcOutputFramesStateTesting,                             "")
    DEVICE_TEST_PARENT("",           "  Registration API",                                                             0, 0,                                          0, NULL, NULL,                                                                 "{df0a1950-5452-4f9c-a474-d63e1b2f623a}")
    DEVICE_TEST_METHOD("101.1",      "   System APO's are registered",                                                 0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_APORegister_SystemAPOsRegistered,                                "")
    DEVICE_TEST_PARENT("",           "  Generic APO",                                                                  0, 0,                                          0, NULL, NULL,                                                                 "{37df9eb3-792f-4d9e-8cb6-87c645355475}")
    DEVICE_TEST_METHOD("102.1",      "   All registered APO's support required interfaces",                            0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_APO_RequiredInterfaces,                                          "")
    DEVICE_TEST_METHOD("102.2",      "   LockForProcess invalid params",                                               TESTFLAG_DISABLED, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_APO_LockForProcess,                                              "")
    DEVICE_TEST_METHOD("102.3",      "   Basic params for other methods",                                              0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_APO_ValidateBasicInterface,                                      "")
    DEVICE_TEST_METHOD("102.4",      "   LockForProcess buffer size validation",                                       TESTFLAG_DISABLED, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_APO_LockForProcessValidateBufferSize,                            "")
    DEVICE_TEST_PARENT("",           "  Dynamic changes",                                                              0, DEVICE_TYPE_NODEVICE,                       0, NULL, NULL,                                                                 "{e08bdade-a3c6-4370-b0b7-0517e84f1e90}")
    DEVICE_TEST_METHOD("103.1",      "   Invalid transaction params/ops",                                              0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_Processor_TransactionTest,                                       "")
    DEVICE_TEST_METHOD("103.2",      "   Timeout tests",			                                                      0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_Processor_TransactionTimeoutTest,                                "")
    DEVICE_TEST_PARENT("",           "  Pump",                                                                         0, 0,                                          0, NULL, NULL,                                                                 "{3df42b5e-46e6-4dea-9e3f-0c1d1727cc04}")
    DEVICE_TEST_METHOD("200.1",      "   Object registered",                                                           0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_AudioPumpCreate,                                                   "")
    DEVICE_TEST_METHOD("200.2",      "   Initialize",                                                                  TESTFLAG_DISABLED, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_AudioPumpInitialize,                                               "")
    DEVICE_TEST_METHOD("200.6",      "   Configure Timer",                                                             0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_AudioPumpReconfigureTimer,                                         "")
    DEVICE_TEST_PARENT("",           "  Mixer APO",                                                                    0, 0,                                          0, NULL, NULL,                                                                 "{3cfdd248-7778-4c56-bde8-86ecb86ab4cf}")
    DEVICE_TEST_METHOD("600.1",      "   Object registered",                                                           0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOMixerCreate,                                                    "")
    DEVICE_TEST_PARENT("",           "  Matrix APO",                                                                   0, 0,                                          0, NULL, NULL,                                                                 "{a2f25d1b-3a3c-458b-b65c-c4c99cbfeee6}")
    DEVICE_TEST_METHOD("700.1",      "   Object registered",                                                           0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOMatrixCreate,                                                   "")
    DEVICE_TEST_METHOD("700.2",      "   Initialize",                                                                  0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOMatrixInitialize,                                               "")
    DEVICE_TEST_METHOD("700.3",      "   SetMatrixCoefficients",                                                       0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOMatrixSetMatrixCoeffns,                                         "")
    DEVICE_TEST_METHOD("700.4",      "   GetMatrixCoefficients",                                                       0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOMatrixGetMatrixCoeffns,                                         "")
    DEVICE_TEST_PARENT("",           "  Meter APO",                                                                    0, DEVICE_TYPE_NODEVICE,                       0, NULL, NULL,                                                                 "{98420b91-bbf4-40cd-baca-3de89b7bcafc}")
    DEVICE_TEST_METHOD("701.1",      "   Object registered",                                                           0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOMeterCreate,                                                    "")
    DEVICE_TEST_METHOD("701.2",      "   Initialize",                                                                  0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOMeterInitialize,                                                "")
    DEVICE_TEST_METHOD("701.3",      "   GetMeteringData",                                                             0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOMeterGetMeteringData,                                           "")
    DEVICE_TEST_PARENT("",           "  FormatFl32Int16 APO",                                                          0, 0,                                          0, NULL, NULL,                                                                 "{3450471b-d721-4ed8-b4e9-34dbc1141442}")
    DEVICE_TEST_METHOD("801.1",      "   Object registered",                                                           0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatFl32Int16Create,                                          "")
    DEVICE_TEST_METHOD("801.17",     "   SetDitherState",                                                              0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_BaseFormat_SetDitherState,                                         "")
    DEVICE_TEST_METHOD("801.18",     "   GetDitherState",                                                              0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_BaseFormat_GetDitherState,                                         "")
    DEVICE_TEST_PARENT("",           "  FormatInt16Fl32 APO",                                                          0, DEVICE_TYPE_NODEVICE,                       0, NULL, NULL,                                                                 "{303b69ac-bc74-4346-886c-453c468f01f5}")
    DEVICE_TEST_METHOD("801.2",      "   Object registered",                                                           0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatInt16Fl32Create,                                          "")
    DEVICE_TEST_PARENT("",           "  FormatInt24Fl32 APO",                                                          0, DEVICE_TYPE_NODEVICE,                       0, NULL, NULL,                                                                 "{ce6eda51-9e9c-402b-8c74-a9599bb17077}")
    DEVICE_TEST_METHOD("801.3",      "   Object registered",                                                           0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatInt24Fl32Create,                                          "")
    DEVICE_TEST_PARENT("",           "  FormatFl32Int24 APO",                                                          0, DEVICE_TYPE_NODEVICE,                       0, NULL, NULL,                                                                 "{7a2a5c8e-afcf-4d2c-ba67-a16d47a3ed76}")
    DEVICE_TEST_METHOD("801.4",      "   Object registered",                                                           0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatFl32Int24Create,                                          "")
    DEVICE_TEST_PARENT("",           "  FormatFl32Int32 APO",                                                          0, DEVICE_TYPE_NODEVICE,                       0, NULL, NULL,                                                                 "{62e97dd5-3c2c-44be-bb89-1158aa666837}")
    DEVICE_TEST_METHOD("801.5",      "   Object registered",                                                           0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatFl32Int32Create,                                          "")
    DEVICE_TEST_PARENT("",           "  FormatInt32Fl32 APO",                                                          0, DEVICE_TYPE_NODEVICE,                       0, NULL, NULL,                                                                 "{a3bb74cc-c8ee-4410-8ee9-069bf6f72567}")
    DEVICE_TEST_METHOD("801.6",      "   Object registered",                                                           0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatInt32Fl32Create,                                          "")
    DEVICE_TEST_PARENT("",           "  FormatInt32Fl64 APO",                                                          0, DEVICE_TYPE_NODEVICE,                       0, NULL, NULL,                                                                 "{b4735108-bc82-407c-897e-8aa30470c587}")
    DEVICE_TEST_METHOD("801.7",      "   Object registered",                                                           0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatInt32Fl64Create,                                          "")
    DEVICE_TEST_PARENT("",           "  FormatFl64Int32 APO",                                                          0, DEVICE_TYPE_NODEVICE,                       0, NULL, NULL,                                                                 "{14eb774d-117f-48b0-8839-a74066281756}")
    DEVICE_TEST_METHOD("801.8",      "   Object registered",                                                           0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatFl64Int32Create,                                          "")
    DEVICE_TEST_PARENT("",           "  FormatInt16Fl64 APO",                                                          0, DEVICE_TYPE_NODEVICE,                       0, NULL, NULL,                                                                 "{a73ec3ea-4c17-4d79-a158-4cbd2d2a4bf3}")
    DEVICE_TEST_METHOD("801.9",      "   Object registered",                                                           0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatInt16Fl64Create,                                          "")
    DEVICE_TEST_PARENT("",           "  FormatFl64Int16 APO",                                                          0, DEVICE_TYPE_NODEVICE,                       0, NULL, NULL,                                                                 "{262a7d59-ba10-4469-8061-8bacbce3d9be}")
    DEVICE_TEST_METHOD("801.10",     "   Object registered",                                                           0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatFl64Int16Create,                                          "")
    DEVICE_TEST_PARENT("",           "  FormatFl32Int24wide APO",                                                      0, DEVICE_TYPE_NODEVICE,                       0, NULL, NULL,                                                                 "{ace988fe-016a-4584-8cd2-8e3f3dc59f11}")
    DEVICE_TEST_METHOD("801.11",     "   Object registered",                                                           0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatFl32Int24wideCreate,                                      "")
    DEVICE_TEST_PARENT("",           "  FormatInt24wideFl32 APO",                                                      0, DEVICE_TYPE_NODEVICE,                       0, NULL, NULL,                                                                 "{0e58a597-06c6-4e6f-99e6-32d55bf37733}")
    DEVICE_TEST_METHOD("801.12",     "   Object registered",                                                           0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatInt24wideFl32Create,                                      "")
    DEVICE_TEST_PARENT("",           "  FormatFl32Uint8 APO",                                                          0, DEVICE_TYPE_NODEVICE,                       0, NULL, NULL,                                                                 "{93cb4f17-a5f8-48da-9f4d-acd0f07a2047}")
    DEVICE_TEST_METHOD("801.13",     "   Object registered",                                                           0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatFl32Uint8Create,                                          "")
    DEVICE_TEST_PARENT("",           "  FormatUint8Fl32 APO",                                                          0, DEVICE_TYPE_NODEVICE,                       0, NULL, NULL,                                                                 "{2115565d-fcfb-42c8-add5-e71fa17d458f}")
    DEVICE_TEST_METHOD("801.14",     "   Object registered",                                                           0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatUint8Fl32Create,                                          "")
    DEVICE_TEST_PARENT("",           "  FormatFl32Int20 APO",                                                          0, DEVICE_TYPE_NODEVICE,                       0, NULL, NULL,                                                                 "{3ad109a9-3062-41e1-8456-b4a41b7d1e9d}")
    DEVICE_TEST_METHOD("801.15",     "   Object registered",                                                           0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatFl32Int20Create,                                          "")
    DEVICE_TEST_PARENT("",           "  FormatInt20Fl32 APO",                                                          0, DEVICE_TYPE_NODEVICE,                       0, NULL, NULL,                                                                 "{4f9dbcbb-f525-421d-82c2-d76b57ef3692}")
    DEVICE_TEST_METHOD("801.16",     "   Object registered",                                                           0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatInt20Fl32Create,                                          "")
    DEVICE_TEST_PARENT("",           "  KS Render Endpoint",                                                           0, 0,                                          0, NULL, NULL,                                                                 "{2a8f7bcd-e8b0-49c5-a2c9-6a84448df52d}")
    DEVICE_TEST_METHOD("804.1",      "   Object registered",                                                           0, DEVICE_TYPE_RENDER,                         0, NULL, Tc_KSRenderEndpointCreate,                                            "")
    DEVICE_TEST_METHOD("804.21",     "   CPInitialize",                                                                TESTFLAG_DISABLED, DEVICE_TYPE_RENDER,                         0, NULL, Tc_KSRenderEndpointCPClientInitialize,                                "")
        ENABLE_HLK_CERTIFICATION()
    DEVICE_TEST_METHOD("804.3",      "   GetFrequency",                                                                0, DEVICE_TYPE_RENDER,                         0, NULL, Tc_KSRenderEndpointGetFrequency,                                      "")
        ENABLE_HLK_CERTIFICATION()
    DEVICE_TEST_METHOD("804.4",      "   GetPosition",                                                                 0, DEVICE_TYPE_RENDER,                         0, NULL, Tc_KSRenderEndpointGetPosition,                                       "")
        ENABLE_HLK_CERTIFICATION()
    DEVICE_TEST_METHOD("804.5",      "   SetBuffer",                                                                   TESTFLAG_DISABLED, DEVICE_TYPE_RENDER,                         0, NULL, Tc_KSRenderEndpointSetBuffer,                                         "")
        ENABLE_HLK_CERTIFICATION()
    DEVICE_TEST_METHOD("804.6",      "   WriteExclusiveModeParametersToSharedMemory",                                  0, DEVICE_TYPE_RENDER,                         0, NULL, Tc_KSRenderEndpointWriteExclusiveModeParametersToSharedMemory,        "")
        ENABLE_HLK_CERTIFICATION()
    DEVICE_TEST_METHOD("804.7",      "   GetRTCaps",                                                                   0, DEVICE_TYPE_RENDER,                         0, NULL, Tc_KSRenderEndpointGetRTCaps,                                         "")
        ENABLE_HLK_CERTIFICATION()
    DEVICE_TEST_METHOD("804.8",      "   GetFrameFormat",                                                              0, DEVICE_TYPE_RENDER,                         0, NULL, Tc_KSRenderEndpointGetFrameFormat,                                    "")
        ENABLE_HLK_CERTIFICATION()
    DEVICE_TEST_METHOD("804.9",      "   GetLatency",                                                                  0, DEVICE_TYPE_RENDER,                         0, NULL, Tc_KSRenderEndpointGetLatency,                                        "")
        ENABLE_HLK_CERTIFICATION()
    DEVICE_TEST_METHOD("804.10",     "   GetFramesPerPacket",                                                          0, DEVICE_TYPE_RENDER,                         0, NULL, Tc_KSRenderEndpointGetFramesPerPacket,                                "")
        ENABLE_HLK_CERTIFICATION()
    DEVICE_TEST_METHOD("804.11",     "   GetProtectedOutputController",                                                0, DEVICE_TYPE_RENDER,                         0, NULL, Tc_KSRenderEndpointGetProtectedOutputController,                      "")
        ENABLE_HLK_CERTIFICATION()
    DEVICE_TEST_METHOD("804.12",     "   CreateVirtualProtectedOutput",                                                0, DEVICE_TYPE_RENDER,                         0, NULL, Tc_KSRenderEndpointCreateVirtualProtectedOutput,                      "")
        ENABLE_HLK_CERTIFICATION()
    DEVICE_TEST_METHOD("804.13",     "   GetDescription",                                                              0, DEVICE_TYPE_RENDER,                         0, NULL, Tc_KSRenderEndpointGetDescription,                                    "")
        ENABLE_HLK_CERTIFICATION()
    DEVICE_TEST_METHOD("804.14",     "   SetPolicySchemas",                                                            0, DEVICE_TYPE_RENDER,                         0, NULL, Tc_KSRenderEndpointSetPolicySchemas,                                  "")
        ENABLE_HLK_CERTIFICATION()
    DEVICE_TEST_PARENT("",           "  KS Capture Endpoint",                                                          0, 0,                                          0, NULL, NULL,                                                                 "{94152f5e-7a00-4f30-89d7-20802d19daae}")
    DEVICE_TEST_METHOD("805.1",      "   Object registered",                                                           0, DEVICE_TYPE_CAPTURE,                        0, NULL, Tc_KSCaptureEndpointCreate,                                           "")
    DEVICE_TEST_METHOD("805.3",      "   GetFrequency",                                                                0, DEVICE_TYPE_CAPTURE,                        0, NULL, Tc_KSCaptureEndpointGetFrequency,                                     "")
        ENABLE_HLK_CERTIFICATION()
    DEVICE_TEST_METHOD("805.4",      "   GetPosition",                                                                 0, DEVICE_TYPE_CAPTURE,                        0, NULL, Tc_KSCaptureEndpointGetPosition,                                      "")
        ENABLE_HLK_CERTIFICATION()
    DEVICE_TEST_METHOD("805.5",      "   SetBuffer",                                                                   TESTFLAG_DISABLED, DEVICE_TYPE_CAPTURE,                        0, NULL, Tc_KSCaptureEndpointSetBuffer,                                        "")
        ENABLE_HLK_CERTIFICATION()
    DEVICE_TEST_METHOD("805.6",      "   WriteExclusiveModeParametersToSharedMemory",                                  0, DEVICE_TYPE_CAPTURE,                        0, NULL, Tc_KSCaptureEndpointWriteExclusiveModeParametersToSharedMemory,       "")
        ENABLE_HLK_CERTIFICATION()
    DEVICE_TEST_METHOD("805.7",      "   GetRTCaps",                                                                   0, DEVICE_TYPE_CAPTURE,                        0, NULL, Tc_KSCaptureEndpointGetRTCaps,                                        "")
        ENABLE_HLK_CERTIFICATION()
    DEVICE_TEST_METHOD("805.8",      "   GetFrameFormat",                                                              0, DEVICE_TYPE_CAPTURE,                        0, NULL, Tc_KSCaptureEndpointGetFrameFormat,                                   "")
        ENABLE_HLK_CERTIFICATION()
    DEVICE_TEST_METHOD("805.9",      "   GetLatency",                                                                  0, DEVICE_TYPE_CAPTURE,                        0, NULL, Tc_KSCaptureEndpointGetLatency,                                       "")
        ENABLE_HLK_CERTIFICATION()
    DEVICE_TEST_METHOD("805.10",     "   GetFramesPerPacket",                                                          0, DEVICE_TYPE_CAPTURE,                        0, NULL, Tc_KSCaptureEndpointGetFramesPerPacket,                               "")
        ENABLE_HLK_CERTIFICATION()
    DEVICE_TEST_PARENT("",           " Functional Test",                                                               0, 0,                                          0, NULL, NULL,                                                                 "{0e80006e-2880-4258-9045-5ee371265628}")
    DEVICE_TEST_PARENT("",           "  SRC",                                                                          0, 0,                                          0, NULL, NULL,                                                                 "{08c8fe01-e6e0-4235-9752-bcf6e3a82ba1}")
    DEVICE_TEST_METHOD("900.2",      "   SNR Linear float mono push (1K)",                                             0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_FixedFormatConvertor_SNR_Linear32,                               "")
    DEVICE_TEST_METHOD("900.4",      "   SNR Linear 16 bit mono push(1K)",                                             0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_FixedFormatConvertor_SNR_Linear16,                               "")
    DEVICE_TEST_METHOD("900.7",      "   SNR Linear float mono push (Nyq)",                                            0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_FixedFormatConvertor_SNR_Linear32_Nyq,                           "")
    DEVICE_TEST_METHOD("900.10",     "   SNR Linear float multi push (1K)",                                            0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_FixedFormatConvertor_SNR_Linear32_Multi,                         "")
    DEVICE_TEST_METHOD("900.12",     "   SNR Linear float mono pull (1K)",                                             TESTFLAG_DISABLED, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_FixedFormatConvertor_SNR_Linear32_Pull,                          "")
    DEVICE_TEST_METHOD("900.14",     "   SNR Linear float multi pull (1K)",                                            TESTFLAG_DISABLED, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_FixedFormatConvertor_SNR_Linear32_Multi_Pull,                    "")
    DEVICE_TEST_METHOD("900.15",     "   SNR SINC 1 float mono push(1K)",                                              0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_FixedFormatConvertor_SNR_Sinc1_32,                               "")
    DEVICE_TEST_METHOD("900.16",     "   SNR SINC 2 float mono push(1K)",                                              0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_FixedFormatConvertor_SNR_Sinc2_32,                               "")
    DEVICE_TEST_METHOD("900.17",     "   SNR SINC 3 float mono push(1K)",                                              0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_FixedFormatConvertor_SNR_Sinc3_32,                               "")
    DEVICE_TEST_METHOD("900.18",     "   SNR SINC 1 16 bit mono push(1K)",                                             0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_FixedFormatConvertor_SNR_Sinc1_16,                               "")
    DEVICE_TEST_METHOD("900.19",     "   SNR SINC 2 16 bit mono push(1K)",                                             0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_FixedFormatConvertor_SNR_Sinc2_16,                               "")
    DEVICE_TEST_METHOD("900.20",     "   SNR SINC 3 16 bit mono push(1K)",                                             0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_FixedFormatConvertor_SNR_Sinc3_16,                               "")
    DEVICE_TEST_METHOD("900.21",     "   SNR SINC 1 float mono push(Nyq)",                                             0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_FixedFormatConvertor_SNR_Sinc1_32_Nyq,                           "")
    DEVICE_TEST_METHOD("900.22",     "   SNR SINC 2 float mono push(Nyq)",                                             0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_FixedFormatConvertor_SNR_Sinc2_32_Nyq,                           "")
    DEVICE_TEST_METHOD("900.23",     "   SNR SINC 3 float mono push(Nyq)",                                             0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_FixedFormatConvertor_SNR_Sinc3_32_Nyq,                           "")
    DEVICE_TEST_METHOD("900.24",     "   SNR SINC 1 float multi push(1K)",                                             0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_FixedFormatConvertor_SNR_Sinc1_32_Multi,                         "")
    DEVICE_TEST_METHOD("900.25",     "   SNR SINC 2 float multi push(1K)",                                             0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_FixedFormatConvertor_SNR_Sinc2_32_Multi,                         "")
    DEVICE_TEST_METHOD("900.26",     "   SNR SINC 3 float multi push(1K)",                                             0, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_FixedFormatConvertor_SNR_Sinc3_32_Multi,                         "")
    DEVICE_TEST_METHOD("900.27",     "   SNR SINC 1 float mono pull(1K)",                                              TESTFLAG_DISABLED, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_FixedFormatConvertor_SNR_Sinc1_32_Pull,                          "")
    DEVICE_TEST_METHOD("900.28",     "   SNR SINC 2 float mono pull(1K)",                                              TESTFLAG_DISABLED, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_FixedFormatConvertor_SNR_Sinc2_32_Pull,                          "")
    DEVICE_TEST_METHOD("900.29",     "   SNR SINC 3 float mono pull(1K)",                                              TESTFLAG_DISABLED, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_FixedFormatConvertor_SNR_Sinc3_32_Pull,                          "")
    DEVICE_TEST_METHOD("900.30",     "   SNR SINC 1 float multi pull(1K)",                                             TESTFLAG_DISABLED, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_FixedFormatConvertor_SNR_Sinc1_32_Multi_Pull,                    "")
    DEVICE_TEST_METHOD("900.31",     "   SNR SINC 2 float multi pull(1K)",                                             TESTFLAG_DISABLED, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_FixedFormatConvertor_SNR_Sinc2_32_Multi_Pull,                    "")
    DEVICE_TEST_METHOD("900.32",     "   SNR SINC 3 float multi pull(1K)",                                             TESTFLAG_DISABLED, DEVICE_TYPE_NODEVICE,                       0, NULL, Test_FixedFormatConvertor_SNR_Sinc3_32_Multi_Pull,                    "")
    DEVICE_TEST_PARENT("",           "  Matrix",                                                                       0, 0,                                          0, NULL, NULL,                                                                 "{7733cd74-58c2-4000-93fd-2bef69a82886}")
    DEVICE_TEST_METHOD("1000.4",     "   Normal Matrix APO behavior",                                                  0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOMatrixProcess,                                                  "")
#ifdef TEST_LEAP_MATRIX
    DEVICE_TEST_METHOD("100.104",    "   LEAP Matrix APO behavior",                                                    0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_LEAPMatrixProcess,                                                 "")
#endif //TEST_LEAP_MATRIX
    DEVICE_TEST_PARENT("",           "  Mixer",								                                          0, 0,                                          0, NULL, NULL,                                                                 "{8ddeb622-fa45-4022-ab47-3f31e0881de6}")
    DEVICE_TEST_METHOD("1000.5",     "   Normal Mixer APO behavior",                                                   0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOMixerProcess,                                                   "")
    DEVICE_TEST_METHOD("1000.75",    "   Mixer APO \"MIX & COPY \" ",                                                  0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOMixerProcesswProcMix_Copy,                                      "")
    DEVICE_TEST_PARENT("",           "  FormatFl32Int16",                                                              0, 0,                                          0, NULL, NULL,                                                                 "{49edf3b1-cb83-42f2-b176-c7a581a2b56c}")
    DEVICE_TEST_METHOD("1000.80",    "   Normal Format APO behavior",                                                  0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatFl32Int16Process,                                         "")
    DEVICE_TEST_PARENT("",           "  FormatInt16Fl32",                                                              0, 0,                                          0, NULL, NULL,                                                                 "{56424e0d-a60e-4ace-8977-0e5877f89f89}")
    DEVICE_TEST_METHOD("1000.81",    "   Normal Format APO behavior",                                                  0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatInt16Fl32Process,                                         "")
    DEVICE_TEST_PARENT("",           "  FormatInt24Fl32",                                                              0, 0,                                          0, NULL, NULL,                                                                 "{4a2d73cf-2412-4ae0-bc83-50d676de32b6}")
    DEVICE_TEST_METHOD("1000.82",    "   Normal Format APO behavior",                                                  0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatInt24Fl32Process,                                         "")
    DEVICE_TEST_PARENT("",           "  FormatFl32Int24",                                                              0, 0,                                          0, NULL, NULL,                                                                 "{e708e177-af65-4646-8c33-385f9fa64cf0}")
    DEVICE_TEST_METHOD("1000.83",    "   Normal Format APO behavior",                                                  0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatFl32Int24Process,                                         "")
    DEVICE_TEST_PARENT("",           "  FormatFl32Int32",                                                              0, 0,                                          0, NULL, NULL,                                                                 "{b7ea7041-d6f2-4c06-ba86-23f77a47aed3}")
    DEVICE_TEST_METHOD("1000.84",    "   Normal Format APO behavior",                                                  0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatFl32Int32Process,                                         "")
    DEVICE_TEST_PARENT("",           "  FormatInt32Fl32",                                                              0, 0,                                          0, NULL, NULL,                                                                 "{3f5d52cc-767b-4411-b5de-88df11af3c47}")
    DEVICE_TEST_METHOD("1000.85",    "   Normal Format APO behavior",                                                  0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatInt32Fl32Process,                                         "")
    DEVICE_TEST_PARENT("",           "  FormatInt32Fl64",                                                              0, 0,                                          0, NULL, NULL,                                                                 "{7d78e339-d06e-4312-92ec-981d2cbe157b}")
    DEVICE_TEST_METHOD("1000.86",    "   Normal Format APO behavior",                                                  0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatInt32Fl64Process,                                         "")
    DEVICE_TEST_PARENT("",           "  FormatFl64Int32",                                                              0, 0,                                          0, NULL, NULL,                                                                 "{69c9eb67-27db-40aa-84d2-84b8e3908711}")
    DEVICE_TEST_METHOD("1000.87",    "   Normal Format APO behavior",                                                  0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatFl64Int32Process,                                         "")
    DEVICE_TEST_PARENT("",           "  FormatFl64Int16",                                                              0, 0,                                          0, NULL, NULL,                                                                 "{6077c408-25e5-4229-81af-c2edc000485c}")
    DEVICE_TEST_METHOD("1000.88",    "   Normal Format APO behavior",                                                  0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatFl64Int16Process,                                         "")
    DEVICE_TEST_PARENT("",           "  FormatInt16Fl64",                                                              0, 0,                                          0, NULL, NULL,                                                                 "{dee7638d-7f23-4734-af6f-bdef378d88ee}")
    DEVICE_TEST_METHOD("1000.89",    "   Normal Format APO behavior",                                                  0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatInt16Fl64Process,                                         "")
    DEVICE_TEST_PARENT("",           "  FormatInt24wideFl32",                                                          0, 0,                                          0, NULL, NULL,                                                                 "{44938de2-1869-4bd7-a28c-7897077ee324}")
    DEVICE_TEST_METHOD("1000.891",   "   Normal Format APO behavior",                                                  0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatInt24wideFl32Process,                                     "")
    DEVICE_TEST_PARENT("",           "  FormatFl32Int24wide",                                                          0, 0,                                          0, NULL, NULL,                                                                 "{8dc8f11e-7ca0-40f6-9888-02593716c8f1}")
    DEVICE_TEST_METHOD("1000.892",   "   Normal Format APO behavior",                                                  0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatFl32Int24wideProcess,                                     "")
    DEVICE_TEST_PARENT("",           "  FormatFl32Uint8",                                                              0, 0,                                          0, NULL, NULL,                                                                 "{75896800-54d8-4766-a428-666092fc6723}")
    DEVICE_TEST_METHOD("1000.893",   "   Normal Format APO behavior",                                                  0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatFl32Uint8Process,                                         "")
    DEVICE_TEST_PARENT("",           "  FormatUint8Fl32",                                                              0, 0,                                          0, NULL, NULL,                                                                 "{add2bcb2-b5d3-4d4f-9619-b2e72c3cc7b0}")
    DEVICE_TEST_METHOD("1000.894",   "   Normal Format APO behavior",                                                  0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatUint8Fl32Process,                                         "")
    DEVICE_TEST_PARENT("",           "  FormatFl32Int20",                                                              0, 0,                                          0, NULL, NULL,                                                                 "{a34f683d-6e92-48b4-b8a0-be24a9774e1f}")
    DEVICE_TEST_METHOD("1000.895",   "   Normal Format APO behavior",                                                  0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatFl32Int20Process,                                         "")
    DEVICE_TEST_PARENT("",           "  FormatInt20Fl32",                                                              0, 0,                                          0, NULL, NULL,                                                                 "{830a02ac-2be6-43de-acc2-5b4e54e74424}")
    DEVICE_TEST_METHOD("1000.896",   "   Normal Format APO behavior",                                                  0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOFormatInt20Fl32Process,                                         "")
    DEVICE_TEST_PARENT("",           "  Meter",                                                                        0, 0,                                          0, NULL, NULL,                                                                 "{50300f87-a6a2-4db0-85b0-c663b2f01ca0}")
    DEVICE_TEST_METHOD("1000.90",    "   Normal Meter APO behavior",                                                   0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOMeterProcess,                                                   "")
    DEVICE_TEST_METHOD("1000.91",    "   Meter APO memory allocation check",                                           0, DEVICE_TYPE_NODEVICE,                       0, NULL, Tc_APOMeterCheckAllocation,                                           "")
    DEVICE_TEST_PARENT("",           "  Graph pathology",                                                              0, 0,                                          0, NULL, NULL,                                                                 "{1982d39a-c859-4559-b85e-df27619942e9}")
    DEVICE_TEST_METHOD("1700.1",     "   SRC APO only",                                                                0, DEVICE_TYPE_RENDER,                         0, NULL, Test_GraphPathology_SRC,                                              "")
    DEVICE_TEST_METHOD("1700.2",     "   In-place APO only",                                                           0, DEVICE_TYPE_RENDER,                         0, NULL, Test_GraphPathology_Inplace,                                          "")
END_TEST_CASE_INFO(UMATest);

//
// Externals required by the module library
//
#ifndef BUILD_TAEF
UINT32       gnTestCases=_countof(rgTestCaseInfo);
UINT32       gnIconId=APPICON;
UINT32       gnAppId=1000;
#endif
