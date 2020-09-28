EST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1606)
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H264")
   TEST_METHOD_PROPERTY(L"ID", L"1606")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"7D97027D-DF1A-4287-BE74-D199449BEB2F")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"37481494-d3f6-4867-9fd2-81f3c2bbb750")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Advanced Encode Test #1606")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"7D97027D-DF1A-4287-BE74-D199449BEB2E")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"f67cb7d9-5fea-4666-aa3c-14976ac7fb6b")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Advanced Encode Test #1606")
#endif
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_FastCam_Action_03_1920x1088p24f.mp4")
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Dynamic Temporal Layer Test #1606")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1607)
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H264")
   TEST_METHOD_PROPERTY(L"ID", L"1607")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"2E4FCD1F-CD97-4340-8A30-23D426172ADC")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"d5ca9e21-95b0-4800-81a1-f465bc276dd2")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Advanced Encode Test #1607")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"2E4FCD1F-CD97-4340-8A30-23D426172ADB")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"3418751a-a7c9-44bf-bccf-71910d290de4")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Advanced Encode Test #1607")
#endif
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Texture_High_01_320x240p30f.mp4")
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 LTR Test #1607")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1608)
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H264")
   TEST_METHOD_PROPERTY(L"ID", L"1608")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"D25E049A-C1F7-4419-9575-7819C0FFC319")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"3120620d-fece-4e4d-8004-3ad0260816d3")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Advanced Encode Test #1608")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"D25E049A-C1F7-4419-9575-7819C0FFC318")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"1f714df0-dbeb-4c74-b1da-1b09cfb008a5")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Advanced Encode Test #1608")
#endif
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Difficult_Edge_02_720x480p24f.mp4")
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 LTR + Dynamic TL + Max Num ref Frame Test #1608")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1609)
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H264")
   TEST_METHOD_PROPERTY(L"ID", L"1609")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"CCC2D1C3-4D79-4132-9079-1C39A877ABF8")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"bbe68813-00eb-41dc-9a11-c44ddad471ac")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Advanced Encode Test #1609")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"CCC2D1C3-4D79-4132-9079-1C39A877ABF7")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"90d1cfb7-78cd-4927-925f-37459ad49fc2")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Advanced Encode Test #1609")
#endif
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_FastCam_Action_03_1920x1088p24f.mp4")

   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 LTR + Dynamic TL + Max Num ref Frame Test #1609")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1610)
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynamicType")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H265")
   TEST_METHOD_PROPERTY(L"ID", L"1610")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"F55EE139-A0A3-4800-AF6A-9BB72D379C63")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"eac8a1d9-cab9-4871-aed8-a32f2b8d768b")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Mobile Advanced Encode Test #1610")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"F55EE139-A0A3-4800-AF6A-9BB72D379C62")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"3754b869-058f-4e2f-8cbf-4a10fd96637d")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Advanced Encode Test #1610")
#endif
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_FastCam_Action_03_1920x1088p24f.mp4")
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H265 Advanced Encode Test # 1610 - Dynamic Type Test - Change every frame")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1611)
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynamicType")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H265")
   TEST_METHOD_PROPERTY(L"ID", L"1611")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"53EF43F9-75C8-4CE7-A535-C3A3A693F1E3")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"e397528a-98cc-40bf-8507-7621fe0e2bcd")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Mobile Advanced Encode Test #1611")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"53EF43F9-75C8-4CE7-A535-C3A3A693F1E2")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"67e0e2c3-cbe0-4b3b-b58c-51dbdf50d4c7")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Advanced Encode Test #1611")
#endif
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_FastCam_Action_03_1920x1088p24f.mp4")
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H265 Advanced Encode Test # 1611 - Dynamic Type Test - Change every 30 frames")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1612)
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H265")
   TEST_METHOD_PROPERTY(L"ID", L"1612")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"174BCE71-F8F9-481F-A78B-6C31EFFDE45A")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"2a2f24c5-c0da-40b3-be1e-3e1cf12818bd")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Mobile Advanced Encode Test #1612")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"174BCE71-F8F9-481F-A78B-6C31EFFDE459")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"f567ebe8-16d0-4549-b5d7-b25cd063fa40")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Advanced Encode Test #1612")
#endif
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Difficult_Edge_02_720x480p24f.mp4")
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H265 Slice Control (Mode 2) Test #1612")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderTest801)
#ifdef WINPHONE
        COMMON_MOBILE_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"45FA48A3-3555-4931-AE1E-FB4E29CD717C")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"a3adf584-61d1-4da4-8ecd-5a1566df3c18")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Advanced Encode Test #801")
#else
        COMMON_DESKTOP_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"45FA48A3-3555-4931-AE1E-FB4E29CD717B")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"f994de53-d517-4dcd-8e95-c3919746eafd")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Advanced Encode Test #801")
#endif
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\HIMOTION16_9_15_180p.yuv")
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\HIMOTION16_9_30_360p.yuv")
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\HIMOTION16_9_30_720p.yuv")
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\HANDHELD9_16_30_720x1280.yuv")
        TEST_METHOD_PROPERTY(L"Kits.TestType", L"Compatibility")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Advanced Encode Test #801 - Dynamic changes on resolution, IDR, temporal layer count, and QP")
        TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
        TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific HMFT")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderTest802)
#ifdef WINPHONE
        COMMON_MOBILE_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"297015FC-0032-47B0-8315-36F632B0338E")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"34c0108a-a4e2-47d0-a64a-584a464d2c20")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Advanced Encode Test #802")
#else
        COMMON_DESKTOP_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"297015FC-0032-47B0-8315-36F632B0338D")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"e30e5486-bc8d-4bba-ace7-b22ec9bef66e")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Advanced Encode Test #802")
#endif
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\HIMOTION16_9_30_720p.yuv")
        TEST_METHOD_PROPERTY(L"Kits.TestType", L"Compatibility")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Advanced Encode Test #802 - Mark and use LTR syntax and semantic validation")
        TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
        TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific HMFT")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderTest803)
#ifdef WINPHONE
        COMMON_MOBILE_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"D67DD85D-9356-4450-A146-A8857587CCE3")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"d0cff954-644f-437f-a121-3788601ddc99")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Advanced Encode Test #803")
#else
        COMMON_DESKTOP_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"D67DD85D-9356-4450-A146-A8857587CCE2")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"d6f42ab1-c4a7-4058-8b8e-d942afbd5790")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Advanced Encode Test #803")
#endif
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\HIMOTION16_9_15_180p.yuv")
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\HIMOTION16_9_30_360p.yuv")
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\HIMOTION16_9_30_720p.yuv")
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\HANDHELD3_4_15_240x320.yuv")
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\HANDHELD3_4_30_480x640.yuv")
        TEST_METHOD_PROPERTY(L"Kits.TestType", L"Compatibility")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Advanced Encode Test #803 - Four concurrent instances")
        TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
        TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific HMFT")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderTest851)
#ifdef WINPHONE
        COMMON_MOBILE_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"D8B04EC3-53BC-49A7-AA4C-64EC930781B9")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"3ff12ac6-b854-4b8d-9483-636659ab9667")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Benchmark MobileTest #851")
#else
        COMMON_DESKTOP_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"D8B04EC3-53BC-49A7-AA4C-64EC930781B8")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"eaa36a32-18ab-4a45-b814-3285c464efea")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Benchmark Test #851")
#endif
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\HIMOTION16_9_15_240p.yuv")
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\HIMOTION16_9_30_720p.yuv")
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\HIMOTION16_9_30_1080p.yuv")
        TEST_METHOD_PROPERTY(L"Kits.TestType", L"Benchmark")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Benchmark Test #851- Maximal and average encoding and decoding time for 1080p")
        TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
        TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific HMFT")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderTest852)
#ifdef WINPHONE
        COMMON_MOBILE_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"E55FEA94-659C-419F-BA0C-3103FCB5802A")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"c39a4009-eba2-40d0-80f2-c9dcb962b6f3")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Benchmark MobileTest #852")
#else
        COMMON_DESKTOP_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"E55FEA94-659C-419F-BA0C-3103FCB58029")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"51cd240f-d6d7-4eb0-a7bf-edd028def593")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Benchmark Test #852")
#endif
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\HIMOTION16_9_15_240p.yuv")
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\HIMOTION16_9_30_720p.yuv")
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\HIMOTION16_9_30_1080p.yuv")
        TEST_METHOD_PROPERTY(L"Kits.TestType", L"Benchmark")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Benchmark Test #852 - Encoder memory usage and memory leak detection")
        TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
        TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific HMFT")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderTest853)
#ifdef WINPHONE
        COMMON_MOBILE_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"76054FD0-D298-4D8E-80CE-3E27A862CF24")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"15a28636-5519-4970-a76a-00a6ca5ccf01")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Benchmark MobileTest #853")
#else
        COMMON_DESKTOP_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"76054FD0-D298-4D8E-80CE-3E27A862CF23")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"1d7948cf-8430-40b9-9a52-b59e15cd62f3")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Benchmark Test #853")
#endif
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\HIMOTION16_9_15_240p.yuv")
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\LOWLIGHT16_9_15_360p.yuv")
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\HIMOTION16_9_30_720p.yuv")
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\HIMOTION16_9_30_1080p.yuv")
        TEST_METHOD_PROPERTY(L"Kits.TestType", L"Benchmark")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Benchmark Test #851- Maximal and average encoding and decoding time for 1080p")
        TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
        TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific HMFT")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderGradualIntraRefresh1800)
#ifdef WINPHONE
    COMMON_MOBILE_PROPERTIES
    TEST_METHOD_PROPERTY(L"Kits.TestId", L"C5F6219B-6E1E-4BC1-B457-C08F999041F0")
    TEST_METHOD_PROPERTY(L"Kits.TestId2", L"c4e6094e-ca33-4681-8fad-c03242368057")
    TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Basic Encode Test #1800")
#else
    COMMON_DESKTOP_PROPERTIES
    TEST_METHOD_PROPERTY(L"Kits.TestId", L"0ACE1FFF-841B-48B4-8538-8031FFD18A68")
    TEST_METHOD_PROPERTY(L"Kits.TestId2", L"a61af452-6af0-410c-ba35-f1366efebdb6")
    TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Basic Encode Test #1800")
#endif
    TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Difficult_Edge_02_720x480p24f.mp4")
    TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
    TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
    TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
    TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Basic Encode Test #1800 - 480p encode using gradual intra refresh, Mode 0, high profile, GOP 30")
    TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
    TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
    END_TEST_METHOD()


    BEGIN_TEST_METHOD(VisValEncoderGradualIntraRefresh1801)
#ifdef WINPHONE
    COMMON_MOBILE_PROPERTIES
    TEST_METHOD_PROPERTY(L"Kits.TestId", L"FCDBA495-E714-40B7-8248-E6DFA7A66ED9")
    TEST_METHOD_PROPERTY(L"Kits.TestId2", L"f6aa7708-66ce-4321-b607-09123aa3a0c6")
    TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Basic Encode Test #1801")
#else
    COMMON_DESKTOP_PROPERTIES
    TEST_METHOD_PROPERTY(L"Kits.TestId", L"520F5C47-F8DB-4DF2-B0F3-80C8FCD272A0")
    TEST_METHOD_PROPERTY(L"Kits.TestId2", L"b028385f-94d6-451e-b5e6-80cb034ae841")
    TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Basic Encode Test #1801")
#endif
    TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Difficult_Edge_02_720x480p24f.mp4")
    TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
    TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
    TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
    TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Basic Encode Test #1801 - 480p encode using gradual intra refresh, Mode 1, high profile, GOP 30")
    TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
    TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
    END_TEST_METHOD()


    BEGIN_TEST_METHOD(VisValEncoderGradualIntraRefresh1802)
#ifdef WINPHONE
    COMMON_MOBILE_PROPERTIES
    TEST_METHOD_PROPERTY(L"Kits.TestId", L"D9B7AD44-7C65-4982-A57B-AEFADD1B2BB5")
    TEST_METHOD_PROPERTY(L"Kits.TestId2", L"c1ddf859-2112-4180-8029-f2136ff46f88")
    TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Basic Encode Test #1802")
#else
    COMMON_DESKTOP_PROPERTIES
    TEST_METHOD_PROPERTY(L"Kits.TestId", L"D166FAA4-9C59-4F2E-9EFD-EA85C2C5095B")
    TEST_METHOD_PROPERTY(L"Kits.TestId2", L"6664fc66-11bc-4a49-8dfc-52fb3e46017a")
    TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Basic Encode Test #1802")
#endif
    TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Difficult_Edge_02_720x480p24f.mp4")
    TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
    TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
    TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
    TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Basic Encode Test #1800 - 480p encode using gradual intra refresh, Mode 2, high profile, infinite GOP")
    TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
    TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
    END_TEST_METHOD()



    BEGIN_TEST_METHOD(CustomXML)
        TEST_METHOD_PROPERTY(L"CustomXML", L"TRUE")
    END_TEST_METHOD()

protected:
    HRESULT RunActualTest(const DWORD dwTestID, const GUID* pGUIDMFTUnderTest, const WCHAR* pszHWURL);
};

#endif //__MFTDiagTAEF_H__
#define D3D_OVERLOADS
#include "Tessellation.h"
// ----------------------------------------------------------------------------

bool Tessellation::SetDefaultMatrices(void)
{   
    HRESULT     hr;
	float 		fAspect = 1.33f;
	float		fFOV = pi/4.0f;
	D3DXVECTOR3 	m_vAt, m_vUp;

	m_vAt  = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vUp  = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

    D3DXMatrixIdentity( &m_ProjMatrix );
	D3DXMatrixPerspectiveFovLH(&m_ProjMatrix, fFOV, fAspect, 1.0f, 1000.0f);
    if (!SetTransform(D3DTS_PROJECTION, &m_ProjMatrix))
        return false;
	
    D3DXMatrixIdentity( &m_WorldMatrix );
	if (!SetTransform(D3DTS_WORLD, &m_WorldMatrix))
        return false;

	// Setup the view matrices
	D3DXVECTOR3 v[] = {
		D3DXVECTOR3( 0.0f,  0.0f, -3.0f),
		D3DXVECTOR3( 3.0f,  0.0f, -2.0f),
		D3DXVECTOR3( 0.0f,  3.0f, -2.0f),
		//D3DXVECTOR3(-3.0f,  0.0f, -2.0f),
		//D3DXVECTOR3( 0.0f, -3.0f, -2.0f)
	};
	m_nWVMatrices = m_nViewMatrices = sizeof(v)/sizeof(D3DXVECTOR3);
	for (UINT i = 0; i < m_nViewMatrices; i++)
	{
        D3DXMatrixLookAtLH(&(m_ViewMatrices[i]), &(v[i]), &m_vAt, &m_vUp);
		D3DXMatrixMultiply(&(m_WVMatrices[i]), &m_WorldMatrix, &(m_ViewMatrices[i]));
	}
	
    return true;
}

// ----------------------------------------------------------------------------

bool Tessellation::SetDefaultLights(void)
{
	int				m_nLightCount;
	D3DXVECTOR3 	m_vLoc, m_vAt;
    DWORD dwPosLightsSupported = 1;
    
	m_vLoc = D3DXVECTOR3(0.0f, 0.0f, -10.0f);
	m_vAt  = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	CAPS caps;
	m_pSrcDevice->GetDeviceCaps(&caps); 

    // Light1: D3DLIGHT_DIRECTIONAL (no position, not affected by attenuation or range)
	m_pLights[0].Type           = D3DLIGHT_DIRECTIONAL;
	m_pLights[0].Diffuse.r      = 1.0f;
	m_pLights[0].Diffuse.g      = 1.0f;
	m_pLights[0].Diffuse.b      = 1.0f;
	m_pLights[0].Ambient.r      = 1.0f;
	m_pLights[0].Ambient.g      = 1.0f;
	m_pLights[0].Ambient.b      = 1.0f;
	m_pLights[0].Specular.r     = 1.0f;
	m_pLights[0].Specular.g     = 1.0f;
	m_pLights[0].Specular.b     = 1.0f;
	m_pLights[0].Direction      = m_vAt;
	m_pLights[0].Falloff		= 1.0f;

	//Light2: D3DLIGHT_POINT (no direction)
	m_pLights[1].Type           = D3DLIGHT_POINT;
	m_pLights[1].Diffuse.r      = 1.0f;
	m_pLights[1].Diffuse.g      = 1.0f;
	m_pLights[1].Diffuse.b      = 1.0f;
	m_pLights[1].Ambient.r      = 1.0f;
	m_pLights[1].Ambient.g      = 1.0f;
	m_pLights[1].Ambient.b      = 1.0f;
	m_pLights[1].Specular.r     = 1.0f;
	m_pLights[1].Specular.g     = 1.0f;
	m_pLights[1].Specular.b     = 1.0f;
	m_pLights[1].Position       = D3DXVECTOR3(0.0f, 0.0f, -3.0f);
	m_pLights[1].Attenuation0   = 0.98f;//0.98f;
	m_pLights[1].Attenuation1   = 0.015f;//0.015f;
	m_pLights[1].Attenuation2   = 0.005f;//0.05f;
	m_pLights[1].Range          = 1000.0f;
	m_pLights[1].Falloff		= 1.0f;

	//Light0: D3DLIGHT_SPOT
	m_pLights[2].Type           = D3DLIGHT_SPOT;
    m_pLights[2].Diffuse.r      = 1.0f;
    m_pLights[2].Diffuse.g      = 1.0f;
    m_pLights[2].Diffuse.b      = 1.0f;
    m_pLights[2].Ambient.r      = 1.0f;
    m_pLights[2].Ambient.g      = 1.0f;
    m_pLights[2].Ambient.b      = 1.0f;
    m_pLights[2].Specular.r     = 1.0f;
    m_pLights[2].Specular.g     = 1.0f;
    m_pLights[2].Specular.b     = 1.0f;
    m_pLights[2].Position       = m_vLoc;
    m_pLights[2].Direction      = m_vAt;
    m_pLights[2].Attenuation0   = 0.98f;
	m_pLights[2].Attenuation1   = 0.015f;
	m_pLights[2].Attenuation2   = 0.005f;
	m_pLights[2].Theta			= D3DX_PI/10;
    m_pLights[2].Phi			= D3DX_PI/6;
    m_pLights[2].Range          = 1000.0f;
	m_pLights[2].Falloff	    = 1.0f; 

    // Check what kind of lights are available
    if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
    {
        dwPosLightsSupported = (caps.VertexProcessingCaps & D3DVTXPCAPS_POSITIONALLIGHTS);
		if ( dwPosLightsSupported )
			SetLight(0, &(m_pLights[1]));
        else 
		{
			if( !(caps.VertexProcessingCaps & D3DVTXPCAPS_DIRECTIONALLIGHTS) )
			{
				WriteToLog("(FAIL) T&L Device does not support at least Directional Lights.  Test cannot continue.\n");
				return false;
			}
			else 
				SetLight(0, &(m_pLights[0])); 
		}
	}
	// set spot light
	if (KeySet(_T("l2"))) 
		SetLight(0, &(m_pLights[2]));
	// set point light
	else if (KeySet(_T("l1"))) 
		SetLight(0, &(m_pLights[1]));
	// set directional light
	else if (KeySet(_T("l0")))
		SetLight(0, &(m_pLights[0]));
   	
	LightEnable(0, TRUE);

    return true;
}

bool Tessellation::SetDefaultRenderStates(void)
{
	SetRenderState(D3DRS_AMBIENT,0x00505050);
    SetRenderState(D3DRS_NORMALIZENORMALS, (DWORD)TRUE);
	SetRenderState(D3DRS_CULLMODE, (DWORD)D3DCULL_NONE);
	if (KeySet(_T("ccw")))
		SetRenderState(D3DRS_CULLMODE, (DWORD)D3DCULL_CCW);
	SetRenderState(D3DRS_LIGHTING, (DWORD)TRUE);
	SetRenderState(D3DRS_ZENABLE, (DWORD)FALSE);
	if(KeySet(_T("wire")))
		SetRenderState(D3DRS_FILLMODE, (DWORD)D3DFILL_WIREFRAME);
	else if(KeySet(_T("wiresrc")))
		m_pSrcDevice->SetRenderState(D3DRS_FILLMODE, (DWORD)D3DFILL_WIREFRAME);
	else if(KeySet(_T("wireref")))
	{
		if ( NULL != m_pRefDevice )
			m_pRefDevice->SetRenderState(D3DRS_FILLMODE, (DWORD)D3DFILL_WIREFRAME);
	}
	else
		SetRenderState(D3DRS_FILLMODE, (DWORD)D3DFILL_SOLID);
	if (KeySet(_T("continuous")))
		SetRenderState(D3DRS_PATCHEDGESTYLE, (DWORD)D3DPATCHEDGE_CONTINUOUS);
	else
		SetRenderState(D3DRS_PATCHEDGESTYLE, (DWORD)D3DPATCHEDGE_DISCRETE);
	if (m_AdapTess)
	{
		float MinTessLevel = 1.f;
		SetRenderState(D3DRS_MINTESSELLATIONLEVEL, *(DWORD*)&MinTessLevel);
		SetRenderState(D3DRS_MAXTESSELLATIONLEVEL, *(DWORD*)&m_MaxTess);
		SetRenderState(D3DRS_ENABLEADAPTIVETESSELLATION, (DWORD)TRUE);
	}
		
	SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	
	return true;
}

bool Tessellation::CapsCheck()
{
	CAPS caps;
	m_pSrcDevice->GetDeviceCaps(&caps); 
	if ( !( caps.PrimitiveMiscCaps & D3DPMISCCAPS_CULLNONE) )
	{
		WriteToLog("CapsCheck:: - Culling  is not supported.\n");
        return false;
    }
	if ( !KeySet(_T("notex")) && !(caps.LineCaps & D3DLINECAPS_TEXTURE) )
	{
		WriteToLog("CapsCheck: Texture-mapping is not supported\n");
		return false;
	}
	if (!(caps.DevCaps & D3DDEVCAPS_QUINTICRTPATCHES))
		m_UseQuinticRT = false;
	
	m_MaxTess =  min(20.f, caps.MaxNpatchTessellationLevel);
	
	return true;
}
#include "Test_Include.fx"

int Test_Count = 140;

string TestInfo
<
    string TestType = "PS";
>
= "ps3_0_flow_control_dsx";

//cases with dsx r2, r#, s0

PS_CRITERIA PS_001_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_001_Desc = "ps_3_0 : dsx r# is allowed";
string PS_001 =
    "ps_3_0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "dsx r2, r0 "
    "mov oC0, r2 ";

PS_CRITERIA PS_002_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_002_Desc = "ps_3_0 : dsx r# inside if_gt block is NOT allowed";
string PS_002 =
	"ps_3_0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_gt c0.x, r0.x "
    "	dsx r2, r0 "
    "else "
    "	dsx r2, r1 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_003_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_003_Desc = "ps_3_0 : dsx r# inside if_lt block is NOT allowed";
string PS_003 =
	"ps_3_0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_lt c0.y, r0.y "
    "	dsx r2, r0 "
    "else "
    "	dsx r2, r1 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_004_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_004_Desc = "ps_3_0 : dsx r# inside if_ge block is NOT allowed";
string PS_004 =
	"ps_3_0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_ge c0.z, r0.z "
    "	dsx r2, r0 "
    "else "
    "	dsx r2, r1 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_005_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_005_Desc = "ps_3_0 : dsx r# inside if_le block is NOT allowed";
string PS_005 =
	"ps_3_0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_le c0.w, r0.w "
    "	dsx r2, r0 "
    "else "
    "	dsx r2, r1 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_006_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_006_Desc = "ps_3_0 : dsx r# inside if_eq block is NOT allowed";
string PS_006 =
	"ps_3_0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_eq c0.x, r0.y "
    "	dsx r2, r0 "
    "else "
    "	dsx r2, r1 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_007_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_007_Desc = "ps_3_0 : dsx r# inside if_ne block is NOT allowed";
string PS_007 =
	"ps_3_0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_ne c0.z, r0.w "
    "	dsx r2, r0 "
    "else "
    "	dsx r2, r1 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_008_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_008_Desc = "ps_3_0 : dsx r# inside if p.x block is NOT allowed";
string PS_008 =
	"ps_3_0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if p0.x "
    "	dsx r2, r0 "
    "else "
    "	dsx r2, r1 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_009_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_009_Desc = "ps_3_0 : dsx r# inside if !p.y block is NOT allowed";
string PS_009 =
	"ps_3_0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if !p0.y "
    "	dsx r2, r0 "
    "else "
    "	dsx r2, r1 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_010_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_010_Desc = "ps_3_0 : dsx r# inside if p.z block is NOT allowed";
string PS_010 =
	"ps_3_0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if p0.z "
    "	dsx r2, r0 "
    "else "
    "	dsx r2, r1 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_011_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_011_Desc = "ps_3_0 : dsx r# inside if !p.w block is NOT allowed";
string PS_011 =
	"ps_3_0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if !p0.w "
    "	dsx r2, r0 "
    "else "
    "	dsx r2, r1 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_012_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_012_Desc = "ps_3_0 : dsx r# inside if (true) block is allowed";
string PS_012 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defb b0, true "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if b0 "
    "	dsx r2, r0 "
    "else "
    "	dsx r2, r1 "
    "endif "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_013_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_013_Desc = "ps_3_0 : dsx r# inside if !(true) block is allowed";
string PS_013 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defb b0, true "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if !b0 "
    "	dsx r2, r0 "
    "else "
    "	dsx r2, r1 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_014_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_014_Desc = "ps_3_0 : dsx r# inside if (false) block is allowed";
string PS_014 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if b0 "
    "	dsx r2, r0 "
    "else "
    "	dsx r2, r1 "
    "endif "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_015_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_015_Desc = "ps_3_0 : dsx r# inside if !(false) block is allowed";
string PS_015 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if !b0 "
    "	dsx r2, r0 "
    "else "
    "	dsx r2, r1 "
    "endif "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_016_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_016_Desc = "ps_3_0 : dsx r# inside loop block is allowed";
string PS_016 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	dsx r2, r0 "
    "endloop "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_017_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_017_Desc = "ps_3_0 : dsx r# inside rep block is allowed";
string PS_017 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	dsx r2, r0 "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_018_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_018_Desc = "ps_3_0 : dsx r# inside call block is allowed";
string PS_018 =
	"ps_3_0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "call l0 "
    "mov oC0, r2 "
	"ret "
    "label l0 "
    "	dsx r2, r0 "
    "ret ";
    
PS_CRITERIA PS_019_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_019_Desc = "ps_3_0 : dsx r# inside callnz (true) block is allowed";
string PS_019 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defb b0, true "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "callnz l0, b0 "
    "mov oC0, r2 "
	"ret "
    "label l0 "
    "	dsx r2, r0 "
    "ret ";
    
PS_CRITERIA PS_020_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_020_Desc = "ps_3_0 : dsx r# inside callnz !(true) block is allowed";
string PS_020 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defb b0, true "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "callnz l0, !b0 "
    "mov oC0, r2 "
	"ret "
    "label l0 "
    "	dsx r2, r0 "
    "ret ";
    
PS_CRITERIA PS_021_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_021_Desc = "ps_3_0 : dsx r# inside callnz (false) block is allowed";
string PS_021 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "callnz l0, b0 "
    "mov oC0, r2 "
	"ret "
    "label l0 "
    "	dsx r2, r0 "
    "ret ";
    
PS_CRITERIA PS_022_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_022_Desc = "ps_3_0 : dsx r# inside callnz !(false) block is allowed";
string PS_022 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "callnz l0, !b0 "
    "mov oC0, r2 "
	"ret "
    "label l0 "
    "	dsx r2, r0 "
    "ret ";
    
PS_CRITERIA PS_023_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_023_Desc = "ps_3_0 : dsx r# inside callnz !p.x block is NOT allowed";
string PS_023 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "callnz l0, !p0.x "
    "mov oC0, r2 "
	"ret "
    "label l0 "
    "	dsx r2, r0 "
    "ret ";

PS_CRITERIA PS_024_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_024_Desc = "ps_3_0 : dsx r# inside callnz p.y block is NOT allowed";
string PS_024 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "callnz l0, p0.y "
    "mov oC0, r2 "
	"ret "
    "label l0 "
    "	dsx r2, r0 "
    "ret ";
    
PS_CRITERIA PS_025_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_025_Desc = "ps_3_0 : dsx r# inside callnz !p.z block is NOT allowed";
string PS_025 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "callnz l0, !p0.z "
    "mov oC0, r2 "
	"ret "
    "label l0 "
    "	dsx r2, r0 "
    "ret ";
    
PS_CRITERIA PS_026_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_026_Desc = "ps_3_0 : dsx r# inside callnz p.w block is NOT allowed";
string PS_026 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "callnz l0, p0.w "
    "mov oC0, r2 "
	"ret "
    "label l0 "
    "	dsx r2, r0 "
    "ret ";
    
PS_CRITERIA PS_027_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_027_Desc = "ps_3_0 : dsx r# inside loop block with break is NOT allowed";
string PS_027 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	dsx r2, r0 "
    "	break "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_028_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_028_Desc = "ps_3_0 : dsx r# inside loop block with break is NOT allowed";
string PS_028 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break "
    "	dsx r2, r0 "
    "endloop "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_029_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_029_Desc = "ps_3_0 : dsx r# inside rep block with break is NOT allowed";
string PS_029 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	dsx r2, r0 "
    "	break "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_030_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_030_Desc = "ps_3_0 : dsx r# inside rep block with break is NOT allowed";
string PS_030 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break "
    "	dsx r2, r0 "
    "endrep "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_031_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_031_Desc = "ps_3_0 : dsx r# inside loop block with break_gt is NOT allowed";
string PS_031 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	dsx r2, r0 "
    "	break_gt r0.x, r1.x "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_032_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_032_Desc = "ps_3_0 : dsx r# inside loop block with break_gt is NOT allowed";
string PS_032 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_gt r0.x, r1.x "
    "	dsx r2, r0 "
    "endloop "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_033_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_033_Desc = "ps_3_0 : dsx r# inside loop block with break_lt is NOT allowed";
string PS_033 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	dsx r2, r0 "
    "	break_lt r0.y, r1.y "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_034_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_034_Desc = "ps_3_0 : dsx r# inside loop block with break_lt is NOT allowed";
string PS_034 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_lt r0.y, r1.y "
    "	dsx r2, r0 "
    "endloop "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_035_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_035_Desc = "ps_3_0 : dsx r# inside loop block with break_ge is NOT allowed";
string PS_035 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	dsx r2, r0 "
    "	break_ge r0.z, r1.z "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_036_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_036_Desc = "ps_3_0 : dsx r# inside loop block with break_ge is NOT allowed";
string PS_036 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_ge r0.z, r1.z "
    "	dsx r2, r0 "
    "endloop "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_037_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_037_Desc = "ps_3_0 : dsx r# inside loop block with break_le is NOT allowed";
string PS_037 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	dsx r2, r0 "
    "	break_le r0.w, r1.w "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_038_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_038_Desc = "ps_3_0 : dsx r# inside loop block with break_le is NOT allowed";
string PS_038 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_le r0.w, r1.w "
    "	dsx r2, r0 "
    "endloop "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_039_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_039_Desc = "ps_3_0 : dsx r# inside loop block with break_eq is NOT allowed";
string PS_039 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	dsx r2, r0 "
    "	break_eq r0.x, r1.y "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_040_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_040_Desc = "ps_3_0 : dsx r# inside loop block with break_eq is NOT allowed";
string PS_040 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_eq r0.x, r1.y "
    "	dsx r2, r0 "
    "endloop "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_041_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_041_Desc = "ps_3_0 : dsx r# inside loop block with break_ne is NOT allowed";
string PS_041 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	dsx r2, r0 "
    "	break_ne r0.z, r1.w "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_042_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_042_Desc = "ps_3_0 : dsx r# inside loop block with break_ne is NOT allowed";
string PS_042 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_ne r0.z, r1.w "
    "	dsx r2, r0 "
    "endloop "
    "mov oC0, r2 ";    
    
PS_CRITERIA PS_043_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_043_Desc = "ps_3_0 : dsx r# inside rep block with break_gt is NOT allowed";
string PS_043 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	dsx r2, r0 "
    "	break_gt r0.x, r1.x "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_044_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_044_Desc = "ps_3_0 : dsx r# inside rep block with break_gt is NOT allowed";
string PS_044 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_gt r0.x, r1.x "
    "	dsx r2, r0 "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_045_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_045_Desc = "ps_3_0 : dsx r# inside rep block with break_lt is NOT allowed";
string PS_045 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	dsx r2, r0 "
    "	break_lt r0.y, r1.y "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_046_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_046_Desc = "ps_3_0 : dsx r# inside rep block with break_lt is NOT allowed";
string PS_046 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_lt r0.y, r1.y "
    "	dsx r2, r0 "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_047_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_047_Desc = "ps_3_0 : dsx r# inside rep block with break_ge is NOT allowed";
string PS_047 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	dsx r2, r0 "
    "	break_ge r0.z, r1.z "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_048_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_048_Desc = "ps_3_0 : dsx r# inside rep block with break_ge is NOT allowed";
string PS_048 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_ge r0.z, r1.z "
    "	dsx r2, r0 "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_049_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_049_Desc = "ps_3_0 : dsx r# inside rep block with break_le is NOT allowed";
string PS_049 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	dsx r2, r0 "
    "	break_le r0.w, r1.w "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_050_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_050_Desc = "ps_3_0 : dsx r# inside rep block with break_le is NOT allowed";
string PS_050 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_le r0.w, r1.w "
    "	dsx r2, r0 "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_051_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_051_Desc = "ps_3_0 : dsx r# inside rep block with break_eq is NOT allowed";
string PS_051 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	dsx r2, r0 "
    "	break_eq r0.x, r1.y "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_052_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_052_Desc = "ps_3_0 : dsx r# inside rep block with break_eq is NOT allowed";
string PS_052 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_eq r0.x, r1.y "
    "	dsx r2, r0 "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_053_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_053_Desc = "ps_3_0 : dsx r# inside rep block with break_ne is NOT allowed";
string PS_053 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	dsx r2, r0 "
    "	break_ne r0.z, r1.w "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_054_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_054_Desc = "ps_3_0 : dsx r# inside rep block with break_ne is NOT allowed";
string PS_054 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_ne r0.z, r1.w "
    "	dsx r2, r0 "
    "endrep "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_055_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_055_Desc = "ps_3_0 : dsx r# inside loop block with breakp !p.x is NOT allowed";
string PS_055 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	breakp !p0.x "
    "	dsx r2, r0 "
    "endloop "
    "mov oC0, r2 "; 


PS_CRITERIA PS_056_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_056_Desc = "ps_3_0 : dsx r# inside loop block with breakp p.x is NOT allowed";
string PS_056 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	dsx r2, r0 "
    "	breakp p0.x "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_057_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_057_Desc = "ps_3_0 : dsx r# inside loop block with breakp p.y is NOT allowed";
string PS_057 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	breakp p0.y "
    "	dsx r2, r0 "
    "endloop "
    "mov oC0, r2 "; 


PS_CRITERIA PS_058_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_058_Desc = "ps_3_0 : dsx r# inside loop block with breakp !p.y is NOT allowed";
string PS_058 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	dsx r2, r0 "
    "	breakp !p0.y "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_059_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_059_Desc = "ps_3_0 : dsx r# inside loop block with breakp !p.z is NOT allowed";
string PS_059 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	breakp !p0.z "
    "	dsx r2, r0 "
    "endloop "
    "mov oC0, r2 "; 


PS_CRITERIA PS_060_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_060_Desc = "ps_3_0 : dsx r# inside loop block with breakp p.z is NOT allowed";
string PS_060 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	dsx r2, r0 "
    "	breakp p0.z "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_061_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_061_Desc = "ps_3_0 : dsx r# inside loop block with breakp p.w is NOT allowed";
string PS_061 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	breakp p0.w "
    "	dsx r2, r0 "
    "endloop "
    "mov oC0, r2 "; 


PS_CRITERIA PS_062_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_062_Desc = "ps_3_0 : dsx r# inside loop block with breakp !p.w is NOT allowed";
string PS_062 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	dsx r2, r0 "
    "	breakp !p0.w "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_063_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_063_Desc = "ps_3_0 : dsx r# inside rep block with breakp p.x is NOT allowed";
string PS_063 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	add r1, r1, c0.x "
    "	breakp p0.x "
    "	dsx r2, r0 "
    "endrep "
    "mov oC0, r2 "; 

PS_CRITERIA PS_064_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_064_Desc = "ps_3_0 : dsx r# inside rep block with breakp !p.x is NOT allowed";
string PS_064 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	dsx r2, r0 "
    "	breakp !p0.x "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_065_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_065_Desc = "ps_3_0 : dsx r# inside rep block with breakp !p.y is NOT allowed";
string PS_065 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	add r1, r1, c0.x "
    "	breakp !p0.y "
    "	dsx r2, r0 "
    "endrep "
    "mov oC0, r2 "; 

PS_CRITERIA PS_066_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_066_Desc = "ps_3_0 : dsx r# inside rep block with breakp p.y is NOT allowed";
string PS_066 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	dsx r2, r0 "
    "	breakp p0.y "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_067_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_067_Desc = "ps_3_0 : dsx r# inside rep block with breakp p.z is NOT allowed";
string PS_067 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	add r1, r1, c0.x "
    "	breakp p0.z "
    "	dsx r2, r0 "
    "endrep "
    "mov oC0, r2 "; 

PS_CRITERIA PS_068_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_068_Desc = "ps_3_0 : dsx r# inside rep block with breakp !p.z is NOT allowed";
string PS_068 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	dsx r2, r0 "
    "	breakp !p0.z "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_069_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_069_Desc = "ps_3_0 : dsx r# inside rep block with breakp !p.w is NOT allowed";
string PS_069 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	add r1, r1, c0.x "
    "	breakp !p0.w "
    "	dsx r2, r0 "
    "endrep "
    "mov oC0, r2 "; 

PS_CRITERIA PS_070_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_070_Desc = "ps_3_0 : dsx r# inside rep block with breakp p.w is NOT allowed";
string PS_070 =
	"ps_3_0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1vfgbty7u, c1 " 
    "rep i0 "
    "	dsx r2, r0 "
    "	breakp p0.w "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
//cases with dsx r2, t#, s0

PS_CRITERIA PS_071_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_071_Desc = "ps_3_0 : dsx v# is allowed";
string PS_071 =
    "ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "dsx r2, v0 "
    "mov oC0, r2 ";

PS_CRITERIA PS_072_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_072_Desc = "ps_3_0 : dsx v# inside if_gt block is allowed";
string PS_072 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_gt c0.x, r0.x "
    "	dsx r2, v0 "
    "else "
    "	dsx r2, v1 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_073_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_073_Desc = "ps_3_0 : dsx v# inside if_lt block is allowed";
string PS_073 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_lt c0.y, r0.y "
    "	dsx r2, v0 "
    "else "
    "	dsx r2, v1 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_074_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_074_Desc = "ps_3_0 : dsx v# inside if_ge block is allowed";
string PS_074 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_ge c0.z, r0.z "
    "	dsx r2, v0 "
    "else "
    "	dsx r2, v1 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_075_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_075_Desc = "ps_3_0 : dsx v# inside if_le block is allowed";
string PS_075 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_le c0.w, r0.w "
    "	dsx r2, v0 "
    "else "
    "	dsx r2, v1 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_076_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_076_Desc = "ps_3_0 : dsx v# inside if_eq block is allowed";
string PS_076 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_eq c0.x, r0.y "
    "	dsx r2, v0 "
    "else "
    "	dsx r2, v1 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_077_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_077_Desc = "ps_3_0 : dsx v# inside if_ne block is allo