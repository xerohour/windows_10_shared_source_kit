nkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H264")
   TEST_METHOD_PROPERTY(L"ID", L"1602")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"66317C96-0FDD-44C2-96D9-278ECBDDD43A")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"dc2a1912-7432-4db5-a30b-803c5dc5c085")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Advanced Encode Test #1602")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"66317C96-0FDD-44C2-96D9-278ECBDDD439")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"d8d381a4-c235-4098-8b93-6fdc45196fe1")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Advanced Encode Test #1602")
#endif
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Difficult_Edge_02_720x480p24f.mp4")
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Slice Control (Mode 2) Test #1602")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1603)
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H264")
   TEST_METHOD_PROPERTY(L"ID", L"1603")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"07A43A8B-5905-4863-8262-99A0E4B102B2")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"4abbeedd-6edc-427c-8132-5274f7981e05")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Advanced Encode Test #1603")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"07A43A8B-5905-4863-8262-99A0E4B102B1")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"5dc89762-c9e7-4e9d-b094-1d8540bb3d81")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Advanced Encode Test #1603")
#endif
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Difficult_Edge_02_720x480p24f.mp4")
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Slice Control (Mode 0) Test #1603")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1604)
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H264")
   TEST_METHOD_PROPERTY(L"ID", L"1604")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"5CFB3E0F-66DD-41F4-A73A-0C82608345BE")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"6d8d1a29-62d4-416d-a8d3-eccad8179462")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Advanced Encode Test #1604")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"5CFB3E0F-66DD-41F4-A73A-0C82608345BD")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"723cea99-258c-4b53-ba88-04c196a9972c")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Advanced Encode Test #1604")
#endif
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Difficult_Edge_02_720x480p24f.mp4")
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 EncodeQP Test #1604")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1605)
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H264")
   TEST_METHOD_PROPERTY(L"ID", L"1605")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"CB56F6B2-10F0-43A1-AFC7-FF7D3D92D50F")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"8edf0e4d-a38b-496f-99bc-6444c3526221")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Advanced Encode Test #1605")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"CB56F6B2-10F0-43A1-AFC7-FF7D3D92D50E")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"4e44c41b-f815-4e5b-9373-d3e9abbfcee6")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Advanced Encode Test #1605")
#endif
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_FastCam_Action_03_1920x1088p24f.mp4")
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Slice Control (Mode 1) + Static Temporal Layer + GOP Adjustment Test #1605")

   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_T