#pragma once
#ifndef __ME_H_TESTS__
#define __ME_H_TESTS__

#include "MFTDMediaEngineTest.h"
//******************************************************************************

BEGIN_MODULE()
MODULE_PROPERTY(L"Feature", L"MediaEngine")
MODULE_PROPERTY(L"ThreadingModel", L"MTA")
END_MODULE()

#define COMMON_MOBILE_TEST_PROPERTIES \
    COMMON_TEST_PROPERTIES \
    TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"Graphics\\U32OverCUI.dll") \
    TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"Graphics\\kato.dll") \
    TEST_METHOD_PROPERTY(L"Kits.SupportedOS", L"Windows v10.0 Client ARM") \
    TEST_METHOD_PROPERTY(L"Kits.SupportedOS", L"Windows v10.0 Client ARM64") \
    TEST_METHOD_PROPERTY(L"Kits.SupportedOS", L"Windows v10.0 Client x86") \
    TEST_METHOD_PROPERTY(L"Kits.MinRelease", L"TH1") \
    TEST_METHOD_PROPERTY(L"Kits.CorePackageComposition", L"Mobile") \

#define COMMON_ONECORE_TEST_PROPERTIES \
    COMMON_MOBILE_TEST_PROPERTIES \
    /* OneCore UAP */ \
    TEST_METHOD_PROPERTY(L"Kits.SupportedOS", L"Windows v10.0 Client x86") \
    TEST_METHOD_PROPERTY(L"Kits.SupportedOS", L"Windows v10.0 Client x64") \
    TEST_METHOD_PROPERTY(L"Kits.SupportedOS", L"Windows v10.0 Client ARM") \
    TEST_METHOD_PROPERTY(L"Kits.MinRelease", L"TH1") \
    TEST_METHOD_PROPERTY(L"Kits.CorePackageComposition", L"OneCoreUAP") \

#define COMMON_DESKTOP_TEST_PROPERTIES \
    COMMON_TEST_PROPERTIES \
    TEST_METHOD_PROPERTY(L"Kits.SupportedOS", L"Windows v10.0 Client x86") \
    TEST_METHOD_PROPERTY(L"Kits.SupportedOS", L"Windows v10.0 Client x64") \
    TEST_METHOD_PROPERTY(L"Kits.SupportedOS", L"Windows v10.0 Client ARM") \
    TEST_METHOD_PROPERTY(L"Kits.MinRelease", L"TH1") \
    TEST_METHOD_PROPERTY(L"Kits.CorePackageComposition", L"Full") \

#define COMMON_TEST_PROPERTIES \
    TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development") \
    TEST_METHOD_PROPERTY(L"Kits.RunElevated", L"False") \
    TEST_METHOD_PROPERTY(L"Kits.RequiresReboot", L"False") \
    TEST_METHOD_PROPERTY(L"Kits.PublishingOrganization", L"Microsoft Corporation") \
    TEST_METHOD_PROPERTY(L"Kits.DevelopmentPhase", L"Development and Integration") \
    TEST_METHOD_PROPERTY(L"Kits.HasSupplementalContent", L"TRUE") \

#define CONTENT_SOURCE_PROPERTIES(Path) \
    TEST_METHOD_PROPERTY(L"Kits.Parameter", L"ContentSource") \
    TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", Path) \
    TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.IsRequired", L"TRUE") \
    TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Description", L"Path to source File") \

#define HMFT_CONTENT_SOURCE_PROPERTIES \
    CONTENT_SOURCE_PROPERTIES(L"[WTT\\TestBinRoot]\\..\\HMFTContent") \

#define DXVA_CONTENT_SOURCE_PROPERTIES \
    CONTENT_SOURCE_PROPERTIES(L"[WTT\\TestBinRoot]\\..\\DXVAContent") \

#define GUID_PARAMETER_PROPERTIES(Description) \
    TEST_METHOD_PROPERTY(L"Kits.Parameter", L"GUID") \
    TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Default", L"#{DETECT}#") \
    TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.IsRequired", L"TRUE") \
    TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", Description) \

#define HMFT_GUID_PARAMETER_PROPERTIES \
    GUID_PARAMETER_PROPERTIES(L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific HMFT") \

#define DXVA_GUID_PARAMETER_PROPERTIES \
    GUID_PARAMETER_PROPERTIES(L"The GUID of the DXVA Decoder to test - {DETECT} should be used for certification, this can be overridden to test a specific decoder") \

#define HMFT_TEST_PROPERTIES \
    TEST_METHOD_PROPERTY(L"Kits.Specification", L"Device.Streaming.HMFT.Mobile.Decoding") \
    HMFT_CONTENT_SOURCE_PROPERTIES \
    HMFT_GUID_PARAMETER_PROPERTIES \
    
#define DXVA_TEST_PROPERTIES \
    DXVA_CONTENT_SOURCE_PROPERTIES \
    DXVA_GUID_PARAMETER_PROPERTIES \
    
#define DXVA_TEST_PROPERTIES_MOBILE \
    TEST_METHOD_PROPERTY(L"Kits.Specification", L"Device.Graphics.Mobile.Render.D3D11VideoDecoding") \
    DXVA_TEST_PROPERTIES \
    
#define DXVA_TEST_PROPERTIES_ONECORE \
    TEST_METHOD_PROPERTY(L"Kits.Specification", L"Device.Graphics.OneCoreUAP.Render.D3D11VideoDecoding") \
    DXVA_TEST_PROPERTIES_MOBILE \


#define DXVA_TEST_PROPERTIES_DESKTOP \
    TEST_METHOD_PROPERTY(L"Kits.Specification", L"Device.Graphics.WDDM12.Render.D3D11VideoDecoding") \

#ifdef BUILD_MINWIN
    #define DXVA_TEST_PROPERTIES_VPX \
        DXVA_TEST_PROPERTIES_ONECORE 
#else
    #define DXVA_TEST_PROPERTIES_VPX \
        DXVA_TEST_PROPERTIES_DESKTOP 
#endif

#ifdef BUILD_MINWIN
    #define DXVA_TEST_PROPERTIES_DRM \
        TEST_METHOD_PROPERTY(L"Kits.Specification", L"Device.Graphics.Mobile.Render.D3D11VideoDecoding") \
        TEST_METHOD_PROPERTY(L"Kits.Specification", L"Device.Graphics.OneCoreUAP.Render.D3D11VideoDecoding") \
        TEST_METHOD_PROPERTY(L"Kits.Specification", L"System.Fundamentals.Security.PlayReady") \
        DXVA_TEST_PROPERTIES \
        COMMON_DRM_TEST_TASK_PROPERTIES
#else
    #define DXVA_TEST_PROPERTIES_DRM \
        TEST_METHOD_PROPERTY(L"Kits.Specification", L"Device.Graphics.WDDM11.Render.ContentProtection.ContentProtection") \
        TEST_METHOD_PROPERTY(L"Kits.Specification", L"Device.Graphics.WDDM12.Render.PremiumContentPlayback") \
        TEST_METHOD_PROPERTY(L"Kits.Specification", L"System.Fundamentals.Graphics.PremiumContentPlayback") \
        TEST_METHOD_PROPERTY(L"Kits.Specification", L"System.Fundamentals.Security.PlayReady") \
        DXVA_TEST_PROPERTIES \
        COMMON_DRM_TEST_TASK_PROPERTIES
#endif

#ifdef BUILD_MINWIN
    #define MPO_TEST_PROPERTIES \
        TEST_METHOD_PROPERTY(L"Kits.Specification", L"Device.Graphics.Mobile.Render.D3D11VideoDecoding") \
        TEST_METHOD_PROPERTY(L"Kits.Specification", L"Device.Graphics.OneCoreUAP.Render.D3D11VideoDecoding") \
        TEST_METHOD_PROPERTY(L"Kits.RunElevated", L"True") \
        DXVA_TEST_PROPERTIES
#else
    #define MPO_TEST_PROPERTIES \
        TEST_METHOD_PROPERTY(L"Kits.Specification", L"Device.Graphics.WDDM13.DisplayRender.MultiplaneOverlaySupport") \
        TEST_METHOD_PROPERTY(L"Kits.RunElevated", L"True") \
        DXVA_TEST_PROPERTIES
#endif

#define COMMON_DRM_TEST_TASK_PROPERTIES \
    TEST_METHOD_PROPERTY(L"Kits.TestTask", L"SetDRMReg") \
    TEST_METHOD_PROPERTY(L"Kits.TestTask.SetDRMReg.CommandLine", L"cmd /c REG ADD \"HKLM\\SOFTWARE\\Microsoft\\Windows Media Foundation\\ContentProtection\" /f /v EnableClassicActivationOfITA /t REG_DWORD /d 1") \
    TEST_METHOD_PROPERTY(L"Kits.TestTask.SetDRMReg.RunElevated", L"True") \
    TEST_METHOD_PROPERTY(L"Kits.TestTask.SetDRMReg.FailAndContinue", L"False") \
    TEST_METHOD_PROPERTY(L"Kits.TestTask.SetDRMReg.Order", L"1") \
    TEST_METHOD_PROPERTY(L"Kits.TestTask.SetDRMReg.Stage", L"Setup") \
    TEST_METHOD_PROPERTY(L"Kits.TestTask.SetDRMReg.ExpectedExitCode", L"0") \
    TEST_METHOD_PROPERTY(L"Kits.TestTask", L"CleanDRMReg") \
    TEST_METHOD_PROPERTY(L"Kits.TestTask.CleanDRMReg.CommandLine", L"cmd /c REG DELETE \"HKLM\\SOFTWARE\\Microsoft\\Windows Media Foundation\\ContentProtection\" /f") \
    TEST_METHOD_PROPERTY(L"Kits.TestTask.CleanDRMReg.RunElevated", L"True") \
    TEST_METHOD_PROPERTY(L"Kits.TestTask.CleanDRMReg.FailAndContinue", L"True") \
    TEST_METHOD_PROPERTY(L"Kits.TestTask.CleanDRMReg.Order", L"1") \
    TEST_METHOD_PROPERTY(L"Kits.TestTask.CleanDRMReg.Stage", L"Cleanup") \
    TEST_METHOD_PROPERTY(L"Kits.TestTask.CleanDRMReg.ExpectedExitCode", L"0") \

#ifdef BUILD_MINWIN
    #define DUAL_ONECORE_AND_DESKTOP_TEST(testName, testId_OneCore, testId_Desktop) \
        COMMON_ONECORE_TEST_PROPERTIES \
        TEST_METHOD_PROPERTY(L"Kits.TestId", testId_OneCore) \
        TEST_METHOD_PROPERTY(L"Kits.TestId2", testId_OneCore) \
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys " testName) 
#else        
    #define DUAL_ONECORE_AND_DESKTOP_TEST(testName, testId_OneCore, testId_Desktop) \
        COMMON_DESKTOP_TEST_PROPERTIES \
        TEST_METHOD_PROPERTY(L"Kits.TestId", testId_Desktop) \
        TEST_METHOD_PROPERTY(L"Kits.TestId2", testId_Desktop) \
        TEST_METHOD_PROPERTY(L"Kits.TestName", testName) 
#endif
        
class CMediaEngineTests : public WEX::TestClass<CMediaEngineTests>, public CMFTDTaefBase
{
public:
    BEGIN_TEST_CLASS(CMediaEngineTests)
        TEST_CLASS_PROPERTY(L"DataDriven", L"true") 
    END_TEST_CLASS()

    TEST_CLASS_SETUP(ClassSetup);                 // Runs before the first test in the class
    TEST_CLASS_CLEANUP(ClassCleanup);



#ifdef BUILD_MINWIN
    BEGIN_TEST_METHOD(PlaybackTest110)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\33_H264_368x208_15_796kbps.3gp")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"3798383E-36F6-4FB6-A313-06855FB731B9")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"5c5b72e5-3eab-4786-9040-0c9347110f5a")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys MPEG4 Decoder  HMFT Playback test 110")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Playback Test: InputType MPEG4 - MPEG4 Basic Functionality Decodes MPEG4 Content using HMFT Decoder")
        HMFT_TEST_PROPERTIES
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(PlaybackTest111)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\02_MPEG4_1920x1080_23976_7213kbps_Pro.avi")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"D1F84D1C-E9BC-44E7-A5DF-E207744277AE")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"5e5934c3-17f6-429f-8927-2b6ac2ceea11")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys MPEG4 Decoder  HMFT Playback test 111")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Playback Test: InputType MPEG4 - MPEG4 Basic Functionality Decodes MPEG4 Content using HMFT Decoder")
        HMFT_TEST_PROPERTIES
        END_TEST_METHOD()


        BEGIN_TEST_METHOD(PlaybackTest118)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\27_MSHDRef_Difficult_Edge_06_720x576i25f_h263_3080kbps.avi")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"9CD20A70-2D64-4F7D-A683-24987741F0FC")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"21206396-072f-4567-92b2-239645500ac0")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys MPEG4 Decoder  HMFT Playback test 118")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Playback Test: InputType MPEG4 - MPEG4 Basic Functionality Decodes MPEG4 Content using HMFT Decoder")
        HMFT_TEST_PROPERTIES
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(PlaybackTest125)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\52_MSHDRef_Texture_High_01_320x240p30f_MPEG_850kbps.avi")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"A0F65FD9-C05B-4E32-A5DD-8942B62103D8")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"e0ea4cf0-998c-48bc-a4c2-be75ae7ebe85")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys MPEG4 Decoder - HMFT Playback test 125")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Playback Test: InputType MPEG4 - MPEG4 Basic Functionality Decodes MPEG4 Content using HMFT Decoder")
        HMFT_TEST_PROPERTIES
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(PlaybackTest129)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Texture_SkinTones_05_352x288p15f_900k.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"218F27A8-9CF1-45D1-96B5-44ACC911E1F7")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"4c73248a-824e-4a7f-9072-13464962d892")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys MPEG4 Decoder - HMFT Playback test 129")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Playback Test: InputType MPEG4 - MPEG4 Basic Functionality Decodes MPEG4 Content using HMFT Decoder")
        HMFT_TEST_PROPERTIES
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(PlaybackTest131)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Texture_SkinTones_05_352x288p15f_900kbps_XVid_MP4.avi")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"9773B660-8D89-487E-B001-24460B1829F3")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"9e36f0df-e6fe-4c50-8453-6d7bc3b73e46")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys MPEG4 Decoder - HMFT Playback test 131")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Playback Test: InputType MPEG4 - MPEG4 Basic Functionality Decodes MPEG4 Content using HMFT Decoder")
        HMFT_TEST_PROPERTIES
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(PlaybackTest150)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\22_AVI_MJPEG_1920x1080_44Mbps_2997_ADPCM_11025kHz.avi")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"CA8B1E4F-66FE-4FC3-ADCD-FEC47F424C28")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"0c3bb702-8bae-4f1b-8f7d-80bd68355e60")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys MJPEG Decoder - HMFT Playback test 150")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Playback Test: InputType MJPEG - MJPEG Basic Functionality Decodes MJPEG Content using HMFT")
        HMFT_TEST_PROPERTIES
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(PlaybackTest151)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\23_AVI_MJPEG_720x480_10Mbps_2997_ADPCM_8kHz.avi")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"9551C465-1ECE-4A6F-99E0-F54E08E61200")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"59a3837a-40c7-42f2-8853-16ad117da52c")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys MJPEG Decoder - HMFT Playback test 151")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Playback Test: InputType MJPEG - MJPEG Basic Functionality Decodes MJPEG Content using HMFT")
        HMFT_TEST_PROPERTIES
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(PlaybackTest152)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\24_AVI_MJPEG_1280x720_21800Kbps_2997_ADPCM_11025kHz.avi")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"C8FDEB1D-1186-4B57-AE14-D904CF262F2B")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"b07d378d-f6e2-455a-9e89-93f53fdf2694")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys MJPEG Decoder - HMFT Playback test 152")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Playback Test: InputType MJPEG - MJPEG Basic Functionality Decodes MJPEG Content using HMFT")
        HMFT_TEST_PROPERTIES
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(PlaybackTest153)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\39_MOV_PhotoJPEG_1280x720_BEST_30_PCM_48kHz.mov")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"37CBBF65-E0DA-4A71-809C-EAA3A68933AE")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"c53abc3e-febd-4a05-97ad-a87b98fce9f4")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys MJPEG Decoder - HMFT Playback test 153")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Playback Test: InputType MJPEG - MJPEG Basic Functionality Decodes MJPEG Content using HMFT")
        HMFT_TEST_PROPERTIES
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(PlaybackTest154)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\39_MOV_PhotoJPEG_1280x720_LOSSLESS_30_PCM_48kHz.mov")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"56EF924E-1A56-479D-AA83-552390108D46")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"c90d9920-a1fb-4751-a240-9e4695b236bc")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys MJPEG Decoder - HMFT Playback test 154")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Playback Test: InputType MJPEG - MJPEG Basic Functionality Decodes MJPEG Content using HMFT")
        HMFT_TEST_PROPERTIES
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(PlaybackTest155)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MJPEG_144x176_15fps.avi")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"FCB273D1-A78A-4817-B86D-4C4F283F21EF")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"9fad03c8-f881-4e52-a7e2-b79b30df9c68")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys MJPEG Decoder - HMFT Playback test 155")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Playback Test: InputType MJPEG - MJPEG Basic Functionality Decodes MJPEG Content using HMFT")
        HMFT_TEST_PROPERTIES
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(PlaybackTest156)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MJPEG_320x240_30fps.avi")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"2FA58327-51C5-4D50-AE1F-59F816CC0027")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"a7e1f8a6-a6f8-40b5-ad68-62863f2b56a7")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys MJPEG Decoder - HMFT Playback test 156")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Playback Test: InputType MJPEG - MJPEG Basic Functionality Decodes MJPEG Content using HMFT")
        HMFT_TEST_PROPERTIES
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(PlaybackTest157)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MJPEG_640x360_30fps.avi")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"68A9D0F3-A24B-4AFC-AA89-C0823763DCAC")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"4454dc0a-f768-4b6b-9444-9fd219b28a24")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys MJPEG Decoder - HMFT Playback test 157")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Playback Test: InputType MJPEG - MJPEG Basic Functionality Decodes MJPEG Content using HMFT")
        HMFT_TEST_PROPERTIES
        END_TEST_METHOD()

        //H263 Playback tests

        BEGIN_TEST_METHOD(PlaybackTest160)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\H263_704x480_30fps_VBR_AMR.3gp") 
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"36E7640F-08C8-40FD-9E1D-09514A6EE89C")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"34b5d1e7-4170-4f51-afac-bbc26f33ffb6")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H263 Decoder - HMFT Playback test 160")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Playback Test: InputType H263 - H263 Basic Functionality Decodes H263 Content using HMFT")
        HMFT_TEST_PROPERTIES
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(PlaybackTest161)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\H263_128X96_30fps_250k_VBR_AMR.3g2") 
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"46B78317-14FA-4710-BFBE-AF2912E14B54")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"c183fff9-edc1-4c26-98ff-978ae0f8a421")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H263 Decoder - HMFT Playback test 161")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Playback Test: InputType H263 - H263 Basic Functionality Decodes H263 Content using HMFT")
        HMFT_TEST_PROPERTIES
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(PlaybackTest162)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\H263_320X240_15fps_384k_CBR_AMR.3g2") 
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"D0E82839-C4E4-4AC4-978A-CEFED0189DB6")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"26c20ebd-8624-41fc-a0cd-8637b2cf628c")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H263 Decoder - HMFT Playback test 162")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Playback Test: InputType H263 - H263 Basic Functionality Decodes H263 Content using HMFT")
        HMFT_TEST_PROPERTIES
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(PlaybackTest163)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\H263_704x480_10fps_VBR_AMR.3gp") 
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"FE2339C9-66A1-408C-928C-A29A8D51F167")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"04a03943-543b-45b9-a884-5bd2e93c3e34")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H263 Decoder - HMFT Playback test 163")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Playback Test: InputType H263 - H263 Basic Functionality Decodes H263 Content using HMFT")
        HMFT_TEST_PROPERTIES
        END_TEST_METHOD()


        //DXVA H264 Tests
        BEGIN_TEST_METHOD(H264PlaybackTest1)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Soccer_1280x720p_3mbps_25fps_High_at_L4.1_Cabac_frext_slice.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"FA6634B5-A166-48D3-B5F1-1A91FDBCEAFF")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"b7cf42d6-dc9b-453b-9f26-e56d3ddfceff")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H264 playback test 1")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Decoder H.264 Compliance: InputType H264 - Verify DXVA Decoding for H.264")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264PlaybackTest2)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Tallship_1920x1088i_field_14mbps_25fps_High_at_L4.1_Cabac_Slice.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"32A43A17-51E8-4A56-B788-6FBB889B124F")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"547b619b-e102-4503-a448-9caa746e361b")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H264 playback test 2")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Decoder H.264 Compliance: InputType H264 - Verify DXVA Decoding for H.264")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264PlaybackTest3)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Tallship_720x480p_5mbps_24fps_Main_at_L3.2_CABAC_DifficultEdge.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"C9E0165C-D82B-4A1A-AC78-4B73DE393910")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"d00eca67-a008-4345-a9d6-3e2a2cb9529d")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H264 playback test 3")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Decoder H.264 Compliance: InputType H264 - Verify DXVA Decoding for H.264")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264PlaybackTest4)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Dancing_1920x1088p_9mbps_25fps_High_at_L4.1_Cabac_Frext.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"4CF25C08-11CF-43E9-BF5D-2E9689EBCB1E")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"0c76620f-834b-453d-9b52-8688deda1bf1")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H264 playback test 4")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Decoder H.264 Compliance: InputType H264 - Verify DXVA Decoding for H.264")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264PlaybackTest5)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Dancing_1920x1088_6mbps_25fps_High_at_L4.1_Cavlc.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"73D95FB4-10C5-4036-8695-69CFEDF600C7")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"8a570b2a-9eaf-41df-b72d-24a1b340d758")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H264 playback test 5")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Decoder H.264 Compliance: InputType H264 - Verify DXVA Decoding for H.264")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264PlaybackTest6)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Dancing_1920x1088i_6mbps_25fps_High_at_L4.1_Cabac_Frext.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"11909292-4431-4080-AE30-9160B5846D95")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"bf46e1f3-661a-47d4-a63d-1c91542b4e03")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H264 playback test 6")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Decoder H.264 Compliance: InputType H264 - Verify DXVA Decoding for H.264")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264PlaybackTest7)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Dancing_1920x1088i_9mbps_25fps_Main_at_L4.1_Cabac_PureMbaff.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"FABB40DF-6811-4917-9AA5-A9C345692C45")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"7ca0ef83-e1a8-415d-8472-fa47bcc7efdd")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H264 playback test 7")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Decoder H.264 Compliance: InputType H264 - Verify DXVA Decoding for H.264")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264PlaybackTest8)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Dancing_1920x1088i_8mbps_25fps_Main_at_L4.1_Cabac_Frext_Frm.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"BC49E33E-32ED-4BC8-9AFA-47D491A9D932")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"39185e28-13a5-43d8-a428-c7eddebe3f80")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H264 playback test 8")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Decoder H.264 Compliance: InputType H264 - Verify DXVA Decoding for H.264")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264PlaybackTest9)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Soccer_544x480_1mbps_29.97fps_Main_at_L3.0_Cabac_DTV.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"32F4D155-3501-426B-A806-45F05D24A2A0")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"0078e819-58af-4705-8dce-458fe285dfd9")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H264 playback test 9")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Decoder H.264 Compliance: InputType H264 - Verify DXVA Decoding for H.264")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264PlaybackTest11)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Dancing_1920x1088p_9mbps_25fps_High_at_L4.1_Cabac_Frext.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"9A21A7AD-9133-4C87-9413-766F0A45D10C")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"b253eb08-20c3-4374-b785-b6ab7b582b57")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H264 playback test 11")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Decoder H.264 Compliance: InputType H264 - Verify DXVA Decoding for H.264")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264PlaybackTest12)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Dancing_1920x1088_9mbps_25fps_Main_at_L4.1_Cabac_NoFrext.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"FC3A0BEF-2FF3-43EF-B0D8-B88E2D6CFE71")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"3bfa9d3a-826a-456f-af7a-f5c697404a3b")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H264 playback test 12")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Decoder H.264 Compliance: InputType H264 - Verify DXVA Decoding for H.264")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264PlaybackTest13)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Soccer_1280x720p_5mbps_60fps_Main_at_L4.0_Cabac_ChromaMoving_VBR_DTV.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"5511AAAF-F959-4D51-8044-A8BDBF08E2CD")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"33148423-3c65-4e08-b4c4-c55e16ad18ab")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H264 playback test 13")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Decoder H.264 Compliance: InputType H264 - Verify DXVA Decoding for H.264")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264PlaybackTest14)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Soccer_1280x720p_4mbps_30fps_Baseline_at_L3.1_8slices.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"A3B88398-F94B-4913-B4C6-15AA277899A8")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"6741209b-330a-44ea-9487-1783bb8f5412")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H264 playback test 14")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Decoder H.264 Compliance: InputType H264 - Verify DXVA Decoding for H.264")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264PlaybackTest15)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Singing_320x240p_62kbps_24fps_Baseline_at_L3.0_TextureSkinTones.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"767D83C7-345C-46B4-9DEE-DADBED266387")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"f862766d-a320-4942-8d1a-dba7e891d8d8")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H264 playback test 15")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Decoder H.264 Compliance: InputType H264 - Verify DXVA Decoding for H.264")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264PlaybackTest16)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Tallship_1920x1080i_10mbps_30fps_Baseline_at_L4.0_fmoaso.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"D7E73F54-03B1-4908-9900-27B7FD28BCBC")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"52cc2322-6044-4baa-a096-307be129fbbf")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H264 playback test 16")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Decoder H.264 Compliance: InputType H264 - Verify DXVA Decoding for H.264")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264PlaybackTest17)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Soccer_544x480i_2mbps_29.97fps_Main_at_L3.0_Cabac.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"E1107254-59DE-4B6D-B717-91396FF70F33")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"656dc1a6-b790-494e-bfe8-04b2e3c7c7e5")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H264 playback test 17")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Decoder H.264 Compliance: InputType H264 - Verify DXVA Decoding for H.264")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264PlaybackTest18)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\H264FormatChange_720x480p_HP_at_L3.0_320x240p_MP.264")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"194C08DF-5A45-4E71-A259-FFE4DB243D7C")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"620d9131-1538-4cce-a41b-64d82cc788a0")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H264 playback test 18")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Decoder H.264 Compliance: InputType H264 - Verify DXVA Decoding for H.264")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264PlaybackTest19)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\H264formatChange_1920x1088_High_at_L4.1_1280x720_Main.264")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"C813AE3C-2CED-4C95-BE3C-C2C1124C2AD2")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"3436e82a-070d-40e4-8de6-f38fc9e75521")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H264 playback test 19")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Decoder H.264 Compliance: InputType H264 - Verify DXVA Decoding for H.264")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(WMVPlaybackTest32)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Singing_352x288p_10mbps_15fps_AP_at_L0_cbr_TextureSkinTones_Noloopfilter.wmv")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"3556AA91-8824-46D3-81FB-50754E3B6D5D")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"d126faf3-f62c-401b-b54a-796f1b9aabce")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys WMV playback test 32")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"WMV Playback DXVA Test: InputType WMV - Verify DXVA Decoding for VC-1")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(WMVPlaybackTest33)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Soccer_720x480i_Field_TFF_3mbps_29.97fps_AP_at_L1_7BFr.wmv")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"127BEDB7-75C7-40B4-A0C3-0EB05BA72E24")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"56086bce-7774-4ad8-aeb5-b122f6fc0646")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys WMV playback test 33")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"WMV Playback DXVA Test: InputType WMV - Verify DXVA Decoding for VC-1")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(WMVPlaybackTest34)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Tallship_720x576p_10mbps_25fps_AP_at_L1_6BFr_Noloopfilter.wmv")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"AA7ECE99-ABE2-44D3-8C0F-4DFB9A24D9E6")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"d2ef9cd3-50b3-412b-af7b-06ca41787fe9")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys WMV playback test 34")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"WMV Playback DXVA Test: InputType WMV - Verify DXVA Decoding for VC-1")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(WMVPlaybackTest35)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Soccer_1280x720p_15mbps_60fps_AP_at_L2.wmv")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"9EAA4E27-0FCB-45F8-8681-5774432A09D4")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"23a886fc-8957-4f94-86bc-1378c75ba5a9")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys WMV playback test 35")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"WMV Playback DXVA Test: InputType WMV - Verify DXVA Decoding for VC-1")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(WMVPlaybackTest36)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Soccer_1920x1080i_Frame_18mbps_29.97fps_AP_at_L2.wmv")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"87779128-32f5-4525-b2ee-cf1d4dd0a944")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"1c3ccfdf-6986-4d56-b8b7-0b387cd68d14")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys WMV playback test 36")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"WMV Playback DXVA Test: InputType WMV - Verify DXVA Decoding for VC-1")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(WMVPlaybackTest37)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Soccer_1920x1080p_10mbps_30fps_AP_at_L3_Motion_VBR_5bFr_0Complx_3Smoothe_Loopfilter.wmv")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"05c9a351-192b-48c3-af66-1b96c8dcc433")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"0f3bd224-f3b6-4cd8-9dce-5a0d6f7e4e39")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys WMV playback test 37")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"WMV Playback DXVA Test: InputType WMV - Verify DXVA Decoding for VC-1")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(WMVPlaybackTest38)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Soccer_1280x720p_10mbps_24fps_AP_at_L3_CBR_1BFr_Noloopfilter.wmv")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"cf175637-964b-4e03-a67f-44ec3e76663e")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"c94c681e-b17f-4238-9b4d-28bdca58c14e")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys WMV playback test 38")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"WMV Playback DXVA Test: InputType WMV - Verify DXVA Decoding for VC-1")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(WMVPlaybackTest39)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Tallship_1920x1080i_MBR_4mbps_29.97fps_AP_at_L3.wmv")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"74e0a7dc-85db-41f2-82ba-21dfd903d022")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"f7ce4879-1d75-4848-9609-541491d94a15")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys WMV playback test 39")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"WMV Playback DXVA Test: InputType WMV - Verify DXVA Decoding for VC-1")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(WMVPlaybackTest40)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Soccer_320x240p_500kbps_29.97fps_WMV3_MP.wmv")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"ae43af39-bde3-4b5f-914d-0958fc3582f5")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"8f00ec8f-a0f0-4b1b-8743-7c07aae19a41")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys WMV playback test 40")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"WMV Playback DXVA Test: InputType WMV - Verify DXVA Decoding for VC-1")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(WMVPlaybackTest41)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Soccer_1280x720p_15mbps_60fps_MP_at_HL.wmv")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"e8062515-6802-4c9a-aa95-06563895fe6e")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"860bc8dc-c3a5-420f-a4e7-368c9424bb1c")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys WMV playback test 41")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"WMV Playback DXVA Test: InputType WMV - Verify DXVA Decoding for VC-1")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(WMVPlaybackTest42)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\multi_resolution_with_sp.asf")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"67ced7d6-8bf2-4a43-8c25-267e497b1854")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"d13202ef-4b63-4074-932d-bddcdaf46cf8")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys WMV playback test 42")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"WMV Playback DXVA Test: InputType WMV - Verify DXVA Decoding for VC-1")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(WMVPlaybackTest43)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\no_sp_cif_hd_vga.asf")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"b8f0ea0c-c996-4570-91ec-b309e5363307")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"2bab420d-0336-4f5a-876f-5d7199ed7ac7")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys WMV playback test 43")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"WMV Playback DXVA Test: InputType WMV - Verify DXVA Decoding for VC-1")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()



        BEGIN_TEST_METHOD(H264FuzzTest100)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Singing_320x240p_380kbps_24fps_Baseline_at_L3.0_-fuzztype5_-RFrs4.264")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"DF7034A4-EB47-4C30-A93A-CAACDBC095C9")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"f7a32c5d-14b2-40bb-b8ca-62023f89668e")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H264 fuzz test 100")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Fuzz DXVA Test: InputType H264 - H.264 Fuzzing test")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264FuzzTest101)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Soccer_640x480p_1.2mbps_29.97fps_High_at_L4.1_-fuzztype0_-RFrp2_-RFer21_-RFrs121_-RFep26_-RFf38.264")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"35135282-3F7E-44D0-980A-B16F8A6A5A31")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"f3425e78-a43c-401b-a869-63fa992922fe")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H264 fuzz test 101")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Fuzz DXVA Test: InputType H264 - H.264 Fuzzing test")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264FuzzTest102)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Tallship_1920x1088i_field_14mbps_FuzzNalu_PPS_ER5.264")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"e6ebf868-6daf-4a92-b015-fe27b45f5f94")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"32194143-8e72-4728-88b1-175e2696d3d3")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H264 fuzz test 102")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Fuzz DXVA Test: InputType H264 - H.264 Fuzzing test")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(WMVFuzzTest114)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Singing_320x240p_10mbps_FuzzAction_DropBDU_ER20.wmv")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"2eb8c049-6b56-48cb-8a0d-01d1e620c0bf")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"7fb9b131-48f5-4b2c-9d40-9153e76ef792")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys WMV fuzz test 114")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"WMV Fuzz DXVA Test: InputType WMV - WMV/VC-1 Fuzzing Test")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(WMVFuzzTest115)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Soccer_720x480i_Field_TFF_3mbps_FuzzActions_RepeatBDU_ER15.wmv")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"491f59cd-4576-42d3-bc03-68fe70f180e1")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"1f9dd3f0-8f56-469f-bf5d-7e487f85f624")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys WMV fuzz test 115")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"WMV Fuzz DXVA Test: InputType WMV - WMV/VC-1 Fuzzing Test")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()


        BEGIN_TEST_METHOD(WMVFuzzTest116)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Soccer_720x480i_frame_3mbps_FuzzActions_ReplaceBDU_ER25.wmv")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"264e1c9d-8818-4a1a-bec9-89ca45f29b0c")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"b37f3dc8-6c9a-4dd1-9faf-73ab0718892f")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys WMV fuzz test 116")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"WMV Fuzz DXVA Test: InputType WMV - WMV/VC-1 Fuzzing Test")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264QualityTest200)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Tallship_1920x1088_10mbps_25fps_High_at_L4.1_Cabac_Mbaff.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"25a65551-a0a2-4888-8a3f-8489c056b4aa")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"1f582a97-e27e-4ba3-8697-b9e77604b41b")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H264 quality test 200")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Quality DXVA Test: InputType H264 - H264 Quality")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264QualityTest201)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Tallship_1920x1088_6mbps_25fps_High_at_L4.1_Cabac_Picaff.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"e3f33739-928f-4f15-b0ba-45b6f0303094")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"76b08198-4685-4ef0-9e87-b70ff7e566ab")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H264 quality test 201")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Quality DXVA Test: InputType H264 - H264 Quality")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264QualityTest202)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Tallship_1920x1088_10mbps_25fps_High_at_L4.1_Cabac_Prog.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"6cdd218b-0f2c-4138-9da7-5aada7ff79da")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"38f97baf-5c35-499f-af22-252ec83c8a26")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H264 quality test 202")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Quality DXVA Test: InputType H264 - H264 Quality")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264QualityTest203)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Soccer_1280x720p_5mbps_60fps_Main_at_L4.0_Cabac_ChromaMoving_VBR_DTV.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"0b865dc9-669c-4b7b-a14b-15970dcabf25")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"f9175dbf-b9be-461d-ba8f-98752be55251")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H264 quality test 203")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Quality DXVA Test: InputType H264 - H264 Quality")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264QualityTest204)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Lanczos_Mobileportrait_720x1280p_9mbps_25fps_Main_at_L3.1_Cabac.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"e0e6e750-464e-4707-b7de-a66e34ed30c1")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"fd36c54c-9918-4aa8-91d7-e880623de38f")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H264 quality test 204")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Quality DXVA Test: InputType H264 - H264 Quality")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264QualityTest205)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Tallship_1920x1080p_4mbps_30fps_Baseline_at_L4.0_4slices.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"9256cd0b-7e71-43a8-9fa7-5a4029311da7")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"3b352648-32b4-44fd-9c10-cea829b8a167")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H264 quality test 205")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Quality DXVA Test: InputType H264 - H264 Quality")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(WMVQualityTest212)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Tallship_320x240p_10mbps_29.97fps_AP_at_L0_6BFr_CBR_Noloopfilter.wmv")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"e549a1c8-323f-459a-add0-6993ee66b7d2")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"b858b5be-44f0-4d44-b660-93f36ca9e6a9")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys WMV quality test 212")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"WMV Quality DXVA Test: InputType WMV - WMV Quality")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(WMVQualityTest213)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Soccer_720x480i_Field_TFF_3mbps_29.97fps_AP_at_L1_2BFr.wmv")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"a8fb9183-841f-4711-bd22-4a81b0e38bfa")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"3bae93c7-c98b-42ec-b071-9d0845d47f69")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys WMV quality test 213")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"WMV Quality DXVA Test: InputType WMV - WMV Quality")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(WMVQualityTest214)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Soccer_720x480i_frame_3mbps_29.97fps_AP_at_L1_4444rangeredux.wmv")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"f6aac4fe-ccce-4928-bb3c-465621425185")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"12256083-7f4c-44df-9df9-cc5ddb370217")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys WMV quality test 214")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"WMV Quality DXVA Test: InputType WMV - WMV Quality")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(WMVQualityTest215)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Soccer_1280x720_15mbps_29.97fps_AP_at_L2.wmv")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"ba5c174f-f414-4305-99d3-12db8aed3f9b")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"9e1d6673-d652-4c5c-822b-4b2e0cd7cd57")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys WMV quality test 215")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"WMV Quality DXVA Test: InputType WMV - WMV Quality")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(WMVQualityTest216)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Soccer_MotionDifDirect_1280x720p_10mbps_24fps_AP_at_L2_vbr_5bfr_2complx_55smoothe_loopfilter.wmv")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"93f9827e-6d0d-4cdb-b5ec-f7143792f710")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"7ba10187-82ae-4210-8e8d-59035ef8d23c")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys WMV quality test 216")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"WMV Quality DXVA Test: InputType WMV - WMV Quality")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(WMVQualityTest217)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Soccer_1280x720p_15mbps_60fps_AP_at_L3_2dquantoption.wmv")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"42432993-7f3d-4db1-96ee-1ec60781f63f")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"ec61494e-f044-4bc9-824e-2767cd799b2b")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys WMV quality test 217")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"WMV Quality DXVA Test: InputType WMV - WMV Quality")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(WMVQualityTest218)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Soccer_960x720p_15mbps_60fps_AP_at_L3_4x3PAR.wmv")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"6bc585ff-c6a5-4dac-be3f-bcb4c08ea2a9")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"d7006779-fbd0-48eb-9bee-35eafe59b8e9")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys WMV quality test 218")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"WMV Quality DXVA Test: InputType WMV - WMV Quality")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(WMVQualityTest219)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Soccer_1920x1080i_Field_TFF_18mbps_29.97fps_AP_at_L3_1LoopFilter.wmv")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"7321d586-cc93-4431-950c-1043f37e7ac9")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"b359eda4-bff3-4c74-bda2-b07e6bac6cb9")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys WMV quality test 219")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"WMV Quality DXVA Test: InputType WMV - WMV Quality")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        //Soccer_1920x1080i_Frame_18mbps_29.97fps_AP_at_L3.wmv
        BEGIN_TEST_METHOD(WMVQualityTest220)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Soccer_1920x1080i_Frame_18mbps_29.97fps_AP_at_L3.wmv")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"482F56C9-4A05-0345-71CB-C8920AC6740B")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"6ccb67f9-d629-4896-a4d7-6a3c7c6f386e")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys WMV quality test 220")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"WMV Quality DXVA Test: InputType WMV - WMV Quality")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        //spFramesNewBitstreamLayout.asf
        BEGIN_TEST_METHOD(WMVQualityTest221)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\spFramesNewBitstreamLayout.asf")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"8C9C7907-4C76-325D-2F23-A7970591EE6C")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"5b5a40a8-b39a-4986-bc95-3b90314ea2c4")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys WMV quality test 221")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"WMV Quality DXVA Test: InputType ASF - WMV Quality")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        //Rotate_180.mov
        BEGIN_TEST_METHOD(H264QualityTest299)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Rotate_180.mov")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"858C4C14-41EE-4DE3-8F6A-9D8308E43595")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"ad862873-b3c9-4fbf-8202-833199d426b6")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys H264 quality test 299")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Quality DXVA Test: InputType H264 - H264 Quality")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()


        BEGIN_TEST_METHOD(VisValPlaybackTest90)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Texture_SkinTones_05_352x288p15f_900k.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"06e32c66-b634-4edd-97db-0bf0f3440e73")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"ffb7e72a-04dd-4d34-8969-e032c9aadbd0")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys MPEG4 Decoder - HMFT VisVal playback test 90")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"5")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"10")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Playback Visual Validation #90: MPEG-4 Visual AdvancedSimple_at_L3 352x288")
        HMFT_TEST_PROPERTIES
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VisValPlaybackTest91)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_FastCam_Action_03_1920x1080i30f_IYUV_30000kbps.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"0bfbc20d-6bcc-4071-a5fb-943a5bf17f15")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"4da1659f-8e6b-49a0-af1c-278e90772deb")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys MPEG4 Decoder - HMFT VisVal playback test 91")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"5")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"10")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Playback Visual Validation #91: MPEG-4 Visual AdvancedSimple_at_L3 1920x1080")
        HMFT_TEST_PROPERTIES
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VisValPlaybackTest92)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Texture_SkinTones_05_352x288p15f_900kbps_XVid_MP4.avi")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"7327f4b9-c995-4efe-b3fa-54d21ea27050")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"f0a80e52-fc23-4e6e-92b4-3cdf4fe96bbd")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys MPEG4 Decoder - HMFT VisVal playback test 92")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"5")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"10")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Playback Visual Validation #92: MPEG-4 Visual Simple_at_L3 352x288")
        HMFT_TEST_PROPERTIES
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCPlaybackTest50)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Motion_FastCam_Action_03_1920x1080p24f_lowlevel_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"8B45DFDA-7C84-4E61-B91B-9D3DDB5CA00F")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"e240ea5d-44b0-4bf7-a920-0b8eb232f756")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCPlaybackTest50")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC Playback test")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCPlaybackTest51)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Motion_FastCam_Action_03_1920x1080p24f_3834kbps_randomaccess_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"7DF4D309-4F48-4DE1-87A5-E5C88F8D85BA")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"ace9a77e-ef98-4b8c-ab05-69002d358592")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCPlaybackTest51")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC Playback test")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCPlaybackTest52)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Texture_High_01_320x240p30f_lowdelay_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"45BAA25C-ED0F-43A6-BDAD-0EF040E41137")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"90b944a6-7e8e-42f0-8646-17d07b652ac4")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCPlaybackTest52")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC Playback test")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCPlaybackTest53)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Texture_SkinTones_05_352x288p15f_intra_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"EDE37072-75A3-4FCB-A349-072EDD3300AC")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"b5a05a2d-72e4-4da7-834f-51881fcaa19e")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCPlaybackTest53")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC Playback test")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCPlaybackTest54)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Texture_SkinTones_05_352x288p15f_randomaccess_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"57A82623-7656-4045-99FC-E32A0A949C04")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"4000db20-6736-4847-9b70-1a2bf19e47c9")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCPlaybackTest54")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC Playback test")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCPlaybackTest55)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Texture_SkinTones_05_352x288p25f_lowdelay_P_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"00D20D6A-4535-4E09-AEB9-A2CB96B77B62")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"b35784fe-dc1d-48c0-9a20-4db44e66ee5b")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCPlaybackTest55")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC Playback test")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCPlaybackTest56)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Texture_SkinTones_08_320x240p24f_lowdelay_P_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"CFE6F95B-1646-4D29-8AC9-2D0F224AB2D8")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"a62a4eca-001c-4482-bd87-69574584d52a")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCPlaybackTest56")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC Playback test")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCPlaybackTest57)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Difficult_Edge_02_720x480i30f_intra_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"13E2BFD6-59E8-4BF5-A9BE-3E41666B25BF")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"ee72383d-57de-4664-b906-5161ce1e141f")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCPlaybackTest57")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC Playback test")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCPlaybackTest58)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Difficult_Edge_02_720x480p30f_1739kbps_randomaccess_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"F15E5600-4C42-4CDE-8D09-56B1424F4460")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"7e9aeabc-40d7-4f31-8197-04323cfda59a")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCPlaybackTest58")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC Playback test")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCPlaybackTest59)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Difficult_Edge_06_720x576i25f_lowdelay_P_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"0B416085-49E1-41F1-B6F2-5D8B0DB659A6")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"e6207154-5e71-49e0-bdc7-01f55877b453")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCPlaybackTest59")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC Playback test")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCPlaybackTest60)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Difficult_Edge_06_720x576p25f_randomaccess_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"CC3A62F6-A39A-4ADD-8222-22DC2A1A4EF4")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"b7f21a7c-5403-4665-b889-4ca6cfe5c786")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCPlaybackTest60")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC Playback test")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCPlaybackTest61)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Motion_DifDirect_01_1280x720p24f_2983kbps_randomaccess_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"88F0F463-F948-403F-8E14-F57214E3D39A")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"29006297-2713-4ce2-bab5-ff903c43427f")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCPlaybackTest61")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC Playback test")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCPlaybackTest62)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Motion_FastCam_Action_03_1920x1080i30f_intra_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"682913F9-941A-4BBC-A0AB-EC2F46592367")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"3b94bcd7-025e-49e5-8543-12d7f089f6d8")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCPlaybackTest62")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC Playback test")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCPlaybackTest63)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Motion_FastCam_Action_03_1920x1080i30f_lowdelay_P_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"D373FBD6-ECA0-4668-9C0B-6D37AD86FC15")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"1c6757c4-7b62-4174-9b0a-f02cdc0aaf5f")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCPlaybackTest63")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC Playback test")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCPlaybackTest64)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Motion_FastCam_Action_03_1920x1080p24f_intra_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"207BBCCB-394C-4967-929C-9E40A34E1F7F")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"3cb47302-c3ee-4676-bdb4-4baedfc78d25")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCPlaybackTest64")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC Playback test")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCPlaybackTest65)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\modechange.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"E6EF2A46-51D1-4838-BB57-FBCA40F98510")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"aa4e1c2d-2f12-4efc-8003-d4815a8741fe")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCPlaybackTest65")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC Playback test")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCPlaybackTest66)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\4K_10bit.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"173E7FA8-B8CF-4114-90FD-53752BCD719B")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"C9D7961D-C1F4-4A6D-917B-2FE44355B422")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCPlaybackTest66")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC Playback test")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCFuzzTest120)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Difficult_Edge_06_720x576i25f_intra_main_fuzzed.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"DF190163-8723-48A2-95E5-B61AB258AEC6")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"621a29f5-6ee4-4497-95f2-7a59c2b589ec")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCFuzzTest120")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC Fuzz test")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCFuzzTest121)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Difficult_Edge_06_720x576p25f_lowdelay_main_fuzzed.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"B61CBEF7-A6F5-4E30-8857-30C3828AB655")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"67ebc2b4-44b1-4db9-9509-d9716ab11236")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCFuzzTest121")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC Fuzz test")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCFuzzTest122)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Motion_FastCam_Action_03_1920x1080i30f_lowdelay_main_fuzzed.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"3D5BB8B5-3EAC-41EA-ADDF-3C7DF0C8D8AA")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"c522a538-cc8c-493f-bdfb-83a69b3d56d4")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCFuzzTest122")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC Fuzz test")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCFuzzTest123)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Motion_FastCam_Action_03_1920x1080p24f_randomacess_fuzzed.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"EBB001BB-78E4-46F4-8BEB-D037C69D45B7")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"5e50b28c-3bc5-4826-b093-26291f177846")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCFuzzTest123")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC Fuzz test")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCFuzzTest124)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Texture_High_01_320x240p30f_lowdelay_main_fuzzed.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"B1BF464B-3B60-49B3-B5AF-78A4F08BB033")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"fec17076-e4c9-4094-9b8b-7c1043989500")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCFuzzTest124")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC Fuzz test")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCFuzzTest125)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Texture_SkinTones_08_320x240p24f_lowlevel_main_fuzzed.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"55AB4D6B-3A3C-456C-9963-DB78794C85AD")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"f9142b44-c7ce-4a3c-96d8-89596f0f5b9f")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCFuzzTest125")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC Fuzz test")
        DXVA_TEST_PROPERTIES_MOBILE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest230)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Difficult_Edge_02_720x480i30f_lowdelay_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"68E24AE7-3DD4-4EA4-8339-7732DE2AE028")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"37bba435-ab0d-4d5f-a96e-b57a9ee91245")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest230")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest231)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Difficult_Edge_02_720x480i30f_lowdelay_P_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"1218ADC6-954E-4F74-AF6F-9E4800EC9B51")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"7188404d-fae3-403e-89d1-37af79bbee3c")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest231")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest232)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Difficult_Edge_02_720x480p24f_lowdelay_P_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"06FC6C78-9A10-43D7-890A-E7F030857ED2")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"338cd8ac-c9f1-42a2-966a-6f764d9898dd")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest232")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest233)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Difficult_Edge_02_720x480p30f_560kbps_randomaccess_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"D5DC3D1F-D169-4A10-A9DB-F9ADAF53D2FD")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"fc686d01-99b9-4412-b80b-eff80b2fdfce")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest233")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest234)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Difficult_Edge_02_720x480p30f_intra_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"A851B83F-3FB8-4FBA-8152-22E95C11D703")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"12087ab9-2f82-4f29-b307-156049c07f65")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest234")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest235)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Difficult_Edge_02_720x480p30f_lowdelay_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"2C9C7BA0-B078-4CF3-ADE2-FC2199AD5664")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"11bdec93-b8f2-46d3-a3a8-e861ee2fe313")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest235")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest236)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Difficult_Edge_02_720x480p30f_lowdelay_P_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"DE03317B-77C6-440A-B804-3AB45C43F827")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"2755cbe9-bc77-4793-9280-f27dc6ce3f07")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest236")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest237)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Difficult_Edge_02_720x480p30f_1047kbps_randomaccess_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"EAD6CFD1-E952-4EAB-8C73-635FAF2FABD8")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"77db3535-47ce-44cf-b1e9-19105750bfa4")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest237")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest238)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Difficult_Edge_06_720x576i25f_intra_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"D4C293F6-AC3E-438A-879C-06BB15836A84")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"f3164040-f9d0-4a38-9515-89084b5c1ba5")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest238")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest239)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Difficult_Edge_06_720x576i25f_randomaccess_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"1E0E02FD-7B67-4AE1-9DB1-E113191819FB")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"3938b884-7889-4a95-84db-573f147ad7a1")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest239")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest240)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Difficult_Edge_06_720x576p25f_intra_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"1C5B9A5A-AB9E-40C1-A4D5-223470966006")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"3c80d945-5319-444c-a888-334eda380d9c")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest240")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest241)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Difficult_Edge_06_720x576p25f_lowdelay_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"9ACD1550-2756-4A99-960A-59D738616B40")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"fc2a3719-d17c-4264-beb5-d3c785e11924")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest241")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest242)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Motion_DifDirect_01_1280x720p24f_intra_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"3E42DC1A-E279-43DD-BE82-ED224025804D")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"78ecf104-ee90-4c13-9e9d-63f63da81d61")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest242")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest243)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Motion_DifDirect_01_1280x720p24f_lowdelay_P_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"E68A494F-256F-42E6-80A4-5D0D1DA601F5")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"a162e651-a1de-4b3e-9da3-c778ce53e8a4")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest243")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest244)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Motion_DifDirect_01_1280x720p24f_2340kbps_randomaccess_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"78957CFA-E768-422D-98A2-1223DA683010")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"85d7b652-2777-4d19-9cb9-ca7246c9bb8d")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest244")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest245)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Motion_FastCam_Action_03_1920x1080i30f_lowdelay_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"7C908DBB-D2B9-440E-9EF4-61E3E6B5EB3F")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"3215a589-cb76-40b7-9454-a128c534baf0")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest245")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest246)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Motion_FastCam_Action_03_1920x1080p24f_5760kbps_randomaccess_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"C8026864-A6D6-456F-A23A-E25FA8A126E4")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"976d2896-01ea-45a8-bb3b-10858872b00c")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest246")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest247)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Motion_FastCam_Action_03_1920x1080p24f_lowdelay_P_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"A18D66B2-8B02-4938-83A1-1A7F83476DD8")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"f578ffb9-e059-43a5-bb57-695b3312754c")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest247")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest248)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Texture_High_01_320x240p30f_randomaccess_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"F71EFFD3-1D84-416A-B810-7FD46D07F015")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"deec6a18-e1ab-493e-afcd-989d65f29b3e")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest248")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest249)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Texture_SkinTones_05_352x288p15f_lowdelay_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"2EC8200E-9CFD-4ECF-9B50-A950F7B9FA18")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"5d2ff91c-ff36-429e-a8f5-e1bd54425065")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest249")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest250)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Texture_SkinTones_05_352x288p15f_lowdelay_P_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"2067169F-1CF2-4D87-9071-7B999A18F1DE")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"1410b263-233b-46c6-8e16-3fca9c5f9c6c")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest250")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest251)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Texture_SkinTones_05_352x288p25f_intra_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"1C8875AD-1F94-43AC-BCF3-9D65B423A498")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"8615e23d-6d87-46e5-be35-a52b0c714ddb")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest251")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest252)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Texture_SkinTones_05_352x288p25f_lowdelay_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"87DBACCC-EC4A-4EF4-88C6-8130390DBF6C")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"13149588-c649-4f94-b485-5be4bc50b5d3")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest252")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest253)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Texture_SkinTones_08_320x240p24f_lowlevel_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"B5CC7B92-BD07-458D-B353-133F13208CD3")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"f8400725-8efb-45db-a990-eb0b8ff6b4f3")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest253")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest254)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Texture_SkinTones_08_320x240p24f_randomaccess_main.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"6BFAEA7E-1B25-4C7A-8F14-84EDBF6D1453")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"01c3d8ce-399c-4a01-9121-4e4dd13558de")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest254")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest255)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\HEVC_3840x2160_64tiles_sao_cross_tile.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"2F1D489C-F001-4CDC-801B-7D5F8E98E8ED")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"97c12598-2771-4663-9c19-b1e6a994a99a")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest255")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test UHD 3840x2160 multiple tiles")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest256)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\HEVC_3840x2160_64tiles_sao_no_cross_tile.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"F7E63DD5-492E-4159-A9A6-D0C591E2A853")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"09052f50-e7c8-4321-b330-887d7a207df0")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest256")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test UHD 3840x2160 multiple tiles no sao cross tile")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest257)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\HEVC_4096x2160_64tiles_sao_cross_tile.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"927D9C76-9115-4D5D-8DD9-2B937771BC64")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"e79d1b5a-610e-4dd7-bfa8-0338d284dab0")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest257")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test 4k 4096x2160 multiple tiles")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest258)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\HEVC_4096x2160_64tiles_sao_no_cross_tile.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"FBD01FE6-73FB-44F0-BB53-4F1034D8C37E")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"8c577fb0-3cd0-43c3-99c5-1a72d9a484cf")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest258")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test 4k 4096x2160 multiple tiles no sao cross tile")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest259)
        COMMON_ONECORE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\4K_10bit.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"BD3FF50A-4FCA-4168-87B6-8FD01528F5C7")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"CFFC535F-DBF7-435E-898A-90CF54D3E2A9")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys HEVCQualityTest259")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test 4k 4096x2160 10 bit content")
        DXVA_TEST_PROPERTIES_ONECORE
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(PlaybackTest444)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_FastCam_Action_03_1920x1080i30f_IYUV_30000kbps_XVid.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"66F7C537-C317-4248-9578-5B9CEF6438A9")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"49bd2c2c-b6ef-4fd6-985e-b403ae948670")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys MPEG4 Decoder  HMFT Playback test 444")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Playback Test: InputType MPEG4 - MPEG4 Basic Functionality Decodes MPEG4 Content using HMFT Decoder")
        HMFT_TEST_PROPERTIES
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(PlaybackTest450)
        COMMON_MOBILE_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\22_AVI_MJPEG_1920x1080_44Mbps_2997_ADPCM_11025kHz.avi")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"76AB8F53-ABB1-421F-BF2E-A0DED04D3A48")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"41f9398f-cdeb-4590-9a62-0584a33bf33f")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"CoreSys MJPEG Decoder - HMFT Playback test 450")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Playback Test: InputType MJPEG - MJPEG Basic Functionality Decodes MJPEG Content using HMFT")
        HMFT_TEST_PROPERTIES
        END_TEST_METHOD()


        //OneCore tests

        //Quality Test: InputType MJPEG - MJPEG Basic Functionality Decodes MJPEG Content using HMFT
        BEGIN_TEST_METHOD(MJPEGQualityTest1)
        END_TEST_METHOD()

        //Quality Test: InputType Windows Media Screen codec 
        BEGIN_TEST_METHOD(MSS1QualityTest2)
        END_TEST_METHOD()

        //Quality Test: InputType DVC/DV Video 
        BEGIN_TEST_METHOD(DVQualityTest3)
        END_TEST_METHOD()


        //Quality Test: InputType Microsoft MPEG 4 codec version 3 
        BEGIN_TEST_METHOD(MP43QualityTest4)
        END_TEST_METHOD()

        //Quality Test: InputType CLSID_CMpeg4DecMediaObject 
        BEGIN_TEST_METHOD(Mpeg4DecQualityTest5)
        END_TEST_METHOD()

        //Quality Test: InputType ISO MPEG 4 codec version 1
        BEGIN_TEST_METHOD(MP4SQualityTest6)
        END_TEST_METHOD()
        
        //Quality Test: InputType mp4 resolution change
        BEGIN_TEST_METHOD(MP4ResChange)
        END_TEST_METHOD()
#else
        BEGIN_TEST_METHOD(HEVCPlaybackTest66)
        COMMON_DESKTOP_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\4K_10bit.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"DBE8AC27-FB06-4224-AACB-A4710E3496C8")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"47AE6924-C8F9-48D9-90F5-F947BECE7D8B")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"HEVCPlaybackTest66")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC Playback test 4k 4096x2160 10 bit content")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCQualityTest259)
        COMMON_DESKTOP_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\4K_10bit.mp4")
        TEST_METHOD_PROPERTY(L"Kits.TestId", L"4A7C88A3-E686-4B60-B5C3-9A8062ABBE75")
        TEST_METHOD_PROPERTY(L"Kits.TestId2", L"B63EA078-90AB-48B6-AD24-AC08467CE9D3")
        TEST_METHOD_PROPERTY(L"Kits.TestName", L"HEVCQualityTest259")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC VisualValidation (Quality) test 4k 4096x2160 10 bit content")
        END_TEST_METHOD()
#endif
        BEGIN_TEST_METHOD(MediaPowerProfileTest)
        DUAL_ONECORE_AND_DESKTOP_TEST(L"Media Power Profile Compliance", L"34337ce3-c853-4734-88cb-2f5d106dfecb", L"80b14d93-811b-4257-9b47-d5cb3b660a77")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"1")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"1")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"Media Power Profile Compliance")
        TEST_METHOD_PROPERTY(L"Kits.Specification", L"System.Client.VideoPlayback.GlitchfreeHDVideoPlayback") \
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264DRMTest1)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\boat.main30.1047kbps.2s.720x480.h264.2ch.320kbps.aac_dashinit.enc.singleKey.mp4")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"H264 DRM Test 1", L"2D90C60B-5390-4647-9B0F-50F75E66C7B4", L"52F9EDFF-8A75-418E-9A42-CCF227A23545")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"H.264 DRM Compliance: InputType H264 - Verify Playback of H.264 DRM File")
        DXVA_TEST_PROPERTIES_DRM
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264DRMTest2)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\boat.main31.2340kbps.2s.1280x720.h264.2ch.320kbps.aac_dashinit.enc.singleKey.mp4")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"H264 DRM Test 2", L"28A5C857-891E-44F6-A0FC-D6BA1C23E01C" , L"1C7FD41E-D5AE-49C9-9F9E-EC38BC87117C")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"H.264 DRM Compliance: InputType H264 - Verify Playback of H.264 DRM File")
        DXVA_TEST_PROPERTIES_DRM
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264DRMTest3)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\ms_media_test.main40.30fps.idr2.0slice.3834kbps.1920x1080.h264.2ch.320kbps.aac_dashinit.cenc.sameKeys.mp4")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"H264 DRM Test 3", L"8289D32B-1D32-4EBC-85F0-C1CAA9E45A9C", L"E310AC5F-3781-4BEC-8B57-15A14EE3A029")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"H.264 DRM Compliance: InputType H264 - Verify Playback of H.264 DRM File")
        DXVA_TEST_PROPERTIES_DRM
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264DRMTest4)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\ms_media_test.main50.60fps.idr2.0slice.7982kbps.3840x2160.h264.2ch.320kbps.aac_dashinit.cenc.sameKeys.mp4")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"H264 DRM Test 4", L"D85D36BB-226E-4E1C-9BB6-E396A0FF64CC", L"CEA064B0-FF84-4E3F-873C-AA440E5FC323")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"H.264 DRM Compliance: InputType H264 - Verify Playback of H.264 DRM File")
        DXVA_TEST_PROPERTIES_DRM
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264DRMTest5)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\ms_media_test.main50.60fps.idr2.0slice.7982kbps.3840x2160.h265.2ch.320kbps.aac_dashinit.cenc.sameKeys.mp4")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"H264 DRM Test 5", L"F20DA0EF-6CF2-4549-BC9A-9BC55F124965", L"29897D61-D785-45E4-B3D3-0EB140C847B3")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"H.264 DRM Compliance: InputType H264 - Verify Playback of H.264 DRM File")
        DXVA_TEST_PROPERTIES_DRM
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCDRMTest6)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\ms_media_test.main31.30fps.idr2.0slice.2340kbps.1280x720.h264.2ch.320kbps.aac_dashinit.cenc.sameKeys.mp4")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"HEVC DRM Test 6", L"6895BD68-F547-43B7-9DB1-BE4DD4638236", L"725DC757-7FEC-4BBC-9941-9BF55D27687A")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC DRM Compliance: InputType H264 - Verify Playback of HEVC DRM File")
        DXVA_TEST_PROPERTIES_DRM
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCDRMTest7)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\ms_media_test.main31.30fps.idr2.0slice.2340kbps.1280x720.h265.2ch.320kbps.aac_dashinit.cenc.sameKeys.mp4")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"HEVC DRM Test 7", L"41812E23-F1E0-4142-91E6-7919043CBDC0", L"FD877AD6-E87D-4F66-A06E-C75117313846")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC DRM Compliance: InputType H264 - Verify Playback of HEVC DRM File")
        DXVA_TEST_PROPERTIES_DRM
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCDRMTest8)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\boat.main40.4278kbps.2s.1920x1080.h265.2ch.320kbps.aac_dashinit.enc.singleKey.mp4")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"HEVC DRM Test 8", L"CE3C38F5-08C6-410A-87B1-7CFBE798DB9A", L"F7BE683E-30B0-409C-8DA2-788547CD1837")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC DRM Compliance: InputType H264 - Verify Playback of HEVC DRM File")
        DXVA_TEST_PROPERTIES_DRM
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCDRMTest9)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\boat.main50.5760kbps.2s.2560x1440.h265.2ch.320kbps.aac_dashinit.enc.singleKey.mp4")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"HEVC DRM Test 9", L"A468B599-80F7-4BE6-9B26-ED70AF1AB8D7", L"2F533B4B-B10A-4EBC-9CBB-F139D027470A")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC DRM Compliance: InputType H264 - Verify Playback of HEVC DRM File")
        DXVA_TEST_PROPERTIES_DRM
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCDRMTest10)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\boat.main51.15900kbps.2s.3840x2160.h265.2ch.320kbps.aac_dashinit.enc.singleKey.mp4")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"HEVC DRM Test 10", L"3160A536-8609-4758-BDBD-94728657ADC3", L"90352866-9C0A-4066-B031-658FCCF5D861")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC DRM Compliance: InputType H264 - Verify Playback of HEVC DRM File")
        DXVA_TEST_PROPERTIES_DRM
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(H264MPOTest11)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\Dancing_1920x1088p_9mbps_25fps_High_at_L4.1_Cabac_Frext.mp4")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"H.264 MPO Test 11", L"5A0692D1-CCF6-4F0C-9A1B-BBA408A07577", L"CC86DB60-7526-48A9-AB97-74BDED8BA815")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 MPO: InputType H264 - Verify Playback with MPO for H.264 File")
        MPO_TEST_PROPERTIES
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(HEVCMPOTest12)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Difficult_Edge_02_720x480p30f_1047kbps_randomaccess_main.mp4")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"HEVC MPO Test 12", L"8A2B05B9-63EC-40EF-AF1C-3D3AC5531BA3", L"78842773-7D8A-4AD4-8A3F-8AD99F83E9BC")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"HEVC MPO: InputType H265 - Verify Playback with MPO for HEVC File")
        MPO_TEST_PROPERTIES
        END_TEST_METHOD()
 
        BEGIN_TEST_METHOD(VP9PlaybackTest1)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Texture_SkinTones_08_320x240p24f.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP9 Playback Test 1", L"7DB40C55-7EC0-427E-97FA-EEE2C04EEAE5", L"E1B35249-9B60-4D95-ABFA-0395F6DFF750")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 Playback test for 320x240p content ")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP9PlaybackTest2)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Texture_SkinTones_05_352x288p25f.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP9 Playback Test 2", L"AD8C5E34-15E6-4C3B-B595-AA76714638A8", L"62DA5397-4F1E-4CAD-8020-B7484DD18876")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 Playback test for 352x288p content ")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP9PlaybackTest3)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Difficult_Edge_02_720x480p24f.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP9 Playback Test 3", L"6E63226E-6A9B-4236-879B-FABDB38C6458", L"AF021475-0EBE-4895-8A94-261B7A7EC98E")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 Playback test for 720x480p content ")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP9PlaybackTest4)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Difficult_Edge_02_720x480i30f.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP9 Playback Test 4", L"6DDB7789-9726-42DD-8C8A-D139213AD8FE", L"87D0B286-C6F2-4B29-9BDF-86C12F113CFD")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 Playback test for 720x480i content ")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP9PlaybackTest5)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Motion_FastCam_Action_03_1920x1080p24f_tile_1_4.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP9 Playback Test 5", L"2B735876-5D8E-4D58-BF04-2FF54A01DE21", L"8F74E53D-FD88-4319-8833-A06227740719")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 Playback test for 1920x1080p content with 4 column tiles")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP9PlaybackTest6)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Motion_FastCam_Action_03_1920x1080p24f_tile_4_1.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP9 Playback Test 6", L"89FE145B-0F52-4FC5-883D-8B943DBFE14E", L"02F82271-712A-48D9-A1BE-4A6EB3A14127")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 Playback test for 1920x1080p content with 4 row tiles ")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP9PlaybackTest7)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Motion_FastCam_Action_03_1920x1080p24f_tile_4_4.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP9 Playback Test 7", L"40A5E8A7-42B4-426A-9052-DC1998BB08A9", L"2C72D9EF-596E-466F-BC07-BB33C53585F4")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 Playback test for 31920x1080p content with 4 row and 4 column tiles ")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP9PlaybackTest8)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\4096x2160_25f.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP9 Playback Test 8", L"5C88C9D4-CC59-4F22-A76A-ABBD0A2A25CD", L"8A183ECD-4B13-4AE7-BB4F-214776140750")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 Playback test for 4096x2160 content ")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP9PlaybackTest9)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\4096x2160_25f_tile_4_4.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP9 Playback Test 9", L"3C553162-2C7C-41FF-B546-A5020FD32E0D", L"22DB7E13-05CD-4619-BAA8-E7EA9CF8F0C3")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 Playback test for 4096x2160 content with 4 row and 4 column tiles")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()


        BEGIN_TEST_METHOD(VP9QualityTest50)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Texture_SkinTones_08_320x240p24f.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP9 Quality Test 50", L"C380D1F6-EACA-4878-9B99-CFBF172E4604", L"25EA744F-5F15-4A35-A229-38CF0C3BC214")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 Quality test for 320x240p content ")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP9QualityTest51)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Texture_SkinTones_05_352x288p25f.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP9 Quality Test 51", L"0C586EB0-84FE-426A-8CCA-F3420A8D93F1", L"4D2C51FE-2862-495E-B2A5-F103EAD361F0")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 Quality test for 352x288p content ")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP9QualityTest52)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Difficult_Edge_02_720x480p24f.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP9 Quality Test 52", L"E14ADCD7-A2E5-42A6-B50B-F5BAB41F77F4", L"B3FECD2C-733D-45F9-9448-CA0F9E62A1B1")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 Quality test for 720x480p content ")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP9QualityTest53)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Difficult_Edge_02_720x480i30f.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP9 Quality Test 53", L"6BCFAD0F-4EF2-4EFE-AD1F-BA1778C1CD1C", L"C8903BDE-8113-484C-B3F2-166BD6EF2AD8")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 Quality test for 720x480i content ")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP9QualityTest54)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Motion_FastCam_Action_03_1920x1080p24f_tile_1_4.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP9 Quality Test 54", L"052C8C56-0A28-4F60-90CA-050D393699B4", L"20E08BB0-9712-4008-BDF0-6473109740B0")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 Quality test for 1920x1080p content with 4 column tiles")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP9QualityTest55)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Motion_FastCam_Action_03_1920x1080p24f_tile_4_1.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP9 Quality Test 55", L"31B8EE96-A4DC-4EBB-8543-638E1D4E6D08", L"5E701EC8-EFE8-42E6-A5AB-8BF9B1A5B576")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 Quality test for 1920x1080p content with 4 row tiles ")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP9QualityTest56)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\MSHDRef_Motion_FastCam_Action_03_1920x1080p24f_tile_4_4.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP9 Quality Test 56", L"BA9ECC2B-151F-40BF-8E46-DA083E8704DB", L"F12AEF66-57EF-4969-B17E-2ECF0399A7E5")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 Quality test for 31920x1080p content with 4 row and 4 column tiles ")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP9QualityTest57)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\4096x2160_25f.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP9 Quality Test 57", L"610D47C3-58B4-410F-ADC5-1BE1E24F7CD7", L"06A73F81-1882-4AFE-9306-E9E117B0DA70")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 Quality test for 4096x2160 content ")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP9QualityTest58)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\4096x2160_25f_tile_4_4.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP9 Quality Test 58", L"E759DB74-CBDF-4180-8063-5A44EBBE5FC0", L"D66986C5-B4FB-42F4-AE32-3F8CD092C1E2")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 Quality test for 4096x2160 content with 4 row and 4 column tiles")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()


        BEGIN_TEST_METHOD(VP8PlaybackTest20)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\VP8_MSHDRef_Texture_SkinTones_08_320x240p24f.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP8 Playback Test 1", L"4AC5A31F-5E86-4B92-806F-08D3D9A7DEDE", L"7A57171E-9B8B-4588-9879-089317C0D557")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP8 Playback test for 320x240p content ")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP8PlaybackTest21)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\VP8_MSHDRef_Texture_SkinTones_05_352x288p25f.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP8 Playback test 2", L"CC858992-AA47-404D-A33C-DBF0E80AAF79", L"E6AF2D79-2DF2-432C-AA4B-DE58F5BB6D67")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP8 Playback test for 352x288p content ")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP8PlaybackTest22)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\VP8_MSHDRef_Difficult_Edge_02_720x480i30f.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP8 Playback test 3", L"CF57384F-2D8D-4EE3-AA2B-B44327FB3AC9", L"85BEAF56-8E2B-4B32-918E-D857FDCBC69A")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP8 Playback test for 720x480i content ")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP8PlaybackTest23)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\VP8_MSHDRef_Difficult_Edge_02_720x480p24f.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP8 Playback test 4", L"A736556D-DA92-4691-8033-CA091FEEF679", L"0E13000F-12F2-4A6D-B36E-C60566D46854")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP8 Playback test for 720x480p content ")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP8PlaybackTest24)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\VP8_MSHDRef_Motion_FastCam_Action_03_1920x1080p24f.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP8 Playback test 1", L"E6BE0273-3053-41CF-9E57-24B79557D389", L"7B1275C8-1C4B-4CF3-BF66-90725C3D1FE4")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP8 Playback test for 1920x1080p content")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()



        BEGIN_TEST_METHOD(VP8QualityTest70)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\VP8_MSHDRef_Texture_SkinTones_08_320x240p24f.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP9 Quality Test 1", L"B0FF7751-D6C8-465D-BF09-3E7F73F245AE", L"DE7C7A33-80E8-4CA2-AA75-2CEED5497DFD")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP8 Quality test for 320x240p content ")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP8QualityTest71)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\VP8_MSHDRef_Texture_SkinTones_05_352x288p25f.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP8 Quality test 2", L"3B33CE81-C422-4890-ADE7-849AB47A1056", L"83E66913-1D01-441E-B511-B9AE05D46772")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP8 Quality test for 352x288p content ")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP8QualityTest72)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\VP8_MSHDRef_Difficult_Edge_02_720x480i30f.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP8 Quality test 3", L"547AF274-B8A3-4501-A98B-4738ABF8FCF3", L"1CD888F2-2FEF-4168-B86C-CE0FCA43BD8F")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP8 Quality test for 720x480i content ")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP8QualityTest73)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\VP8_MSHDRef_Difficult_Edge_02_720x480p24f.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP8 Quality test 4", L"F891B0C2-F0FA-4EC5-9689-A4A2E61FAFBD", L"4FE4D0CF-BCA4-4794-8241-E85EEE502EBF")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP8 Quality test for 720x480p content ")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP8QualityTest74)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\VP8_MSHDRef_Motion_FastCam_Action_03_1920x1080p24f.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP8 Quality test 1", L"A800ECD1-F961-4FAD-876B-B8A4A2B9B357", L"79499B3F-5983-4F23-8A72-F91E6F879315")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP8 Quality test for 1920x1080p content")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()



        BEGIN_TEST_METHOD(VP9_10bitPlaybackTest90)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\VP_10bit_content.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP9 10 bit playback test 1", L"76331B75-6E7E-49B6-B0AE-3C7BE8E6D6B8", L"E74A5AE5-0DBD-463C-9175-F96813360A5F")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 10 bit playback test 1")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP9_10bitQualityTest91)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\VP_10bit_content.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP9 10 bit quality test 1", L"CD35B4B0-DFA7-4573-979C-38CC062161BB", L"D4B01B3E-5970-4C97-9970-4DADB1547908")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 10 bit quality test 1")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP9_10bitPlaybackTest92)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\VP_10bit_tile_4_4.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP9 10 bit playback test 2", L"6DE7FCA6-3F15-4DC3-94A7-B5C82DD74155", L"6CE0C73A-A3BA-48CF-B80F-5BD7C804E196")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 10 bit playback test 2")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()        

        BEGIN_TEST_METHOD(VP9_10bitQualityTest93)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\VP_10bit_tile_4_4.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP9 10 bit quality test 2", L"DD5144C0-334D-44E7-9A77-760E2A5EA0DB", L"101D6D18-70B1-496E-87CD-386AEC4DB45A")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 10 bit quality test 2")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()


        BEGIN_TEST_METHOD(VP8FuzzTest100)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\VP8_FUZZ1.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP8 Fuzz test test 1", L"3F2E1B21-3B77-4CE4-948B-185A77B4CF43", L"E1FCD820-20DA-4633-BA10-7BE759572B2E")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP8 Fuzz test")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP9FuzzTest101)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\VP9_FUZZ1.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP9 Fuzz test 1", L"1FD7D18B-4017-4294-9B28-E44FBE59FB3F", L"B7AC3706-FEDF-4E57-A9AB-6413180B6242")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 Fuzz test 1")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP9FuzzTest102)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\VP9_FUZZ2.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP9 Fuzz test 2", L"80633A32-7816-4D0F-A36C-DC4F127F28ED ", L"2EFF9632-72BF-4F4C-9442-AF9194CDD65F")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 Fuzz test 2")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP9FuzzTest103)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\VP9_FUZZ3.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP9 Fuzz test 3", L"B1B61B2A-7D30-4B95-B5EB-A5CD596A2EBB", L"F005E85D-4A75-402E-9D16-810A087D3745")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 Fuzz test 3")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP9FuzzTest104)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\VP9_FUZZ1_10bit.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP9 10 bit Fuzz test 1", L"342CE41E-EA8F-4009-AEF4-0F56B4A574A4", L"3A5B6B1A-552D-4808-A4CD-E0A91ACB5514")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 10 bit Fuzz test 1")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(VP9FuzzTest105)
        TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\DXVAContent\\VP9_FUZZ2_10bit.webm")
        DUAL_ONECORE_AND_DESKTOP_TEST(L"VP9 10 bit Fuzz test 2", L"7E552A0A-5BA4-469E-B816-0C0746F6CC8B", L"5085A3F4-0BD4-44EE-8368-23259DE59A73")
        TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
        TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
        TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 10 bit Fuzz test 2")
        DXVA_TEST_PROPERTIES_VPX
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(CustomXML)
        COMMON_TEST_PROPERTIES
        TEST_METHOD_PROPERTY(L"CustomXML", L"TRUE")
        END_TEST_METHOD()

protected:

#ifdef BUILD_MINWIN
    HRESULT ExecuteTest();
    HRESULT ExecuteTest(DWORD dwTestId);
    HRESULT ExecuteDXVATest(DWORD dwTestId);   
    HRESULT ExecuteVisValTest(DWORD dwTestId);
    HRESULT ExecuteOneCoreTest(DWORD dwTestId);
#endif
    HRESULT ExecuteVPxDXVATest(DWORD dwTestId);

    HRESULT ExecutePipelineTest(DWORD dwTestId);

    HRESULT ExecuteHEVCDXVATest(DWORD dwTestId);

    HRESULT RunActualTest(const DWORD dwTestID, const GUID* pGUIDMFTUnderTest, const WCHAR* pszHWURL);
    HRESULT RunActualChildTest(
        const DWORD     dwInstance,
        const WCHAR*    pwszContextName,
        const DWORD     dwContextSize
        );
    HRESULT GetGUIDsToTest(
        const DWORD dwTestID,
        GUID pGUIDsToTest [],
        WCHAR* pppszHWURLs [],
        const DWORD dwMaxGUIDsToTest,
        DWORD *pdwNumGUIDs);
};


#endif // __ME_H_TESTS__
