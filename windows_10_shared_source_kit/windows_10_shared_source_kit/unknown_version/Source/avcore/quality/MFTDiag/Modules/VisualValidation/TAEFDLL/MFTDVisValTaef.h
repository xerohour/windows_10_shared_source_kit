#pragma once
#ifndef __MFTDVisValTAEF_H__
#define __MFTDVisValTAEF_H__


#define __NEW_METADATA__

#include "MFTDTaefBase.h"

BEGIN_MODULE()
    MODULE_PROPERTY(L"Feature", L"MFTDiagVisVal")
    MODULE_PROPERTY(L"ThreadingModel", L"MTA")
END_MODULE()

#define COMMON_MOBILE_PROPERTIES \
    TEST_CLASS_PROPERTY(L"Kits.SupportedOS", L"Windows v10.0 Client x86") \
    TEST_CLASS_PROPERTY(L"Kits.SupportedOS", L"Windows v10.0 Client x64") \
    TEST_CLASS_PROPERTY(L"Kits.SupportedOS", L"Windows v10.0 Client ARM") \
    TEST_CLASS_PROPERTY(L"Kits.SupportedOS", L"Windows v10.0 Client ARM64") \
    TEST_CLASS_PROPERTY(L"Kits.MinRelease", L"TH1") \
    TEST_CLASS_PROPERTY(L"Kits.CorePackageComposition", L"OneCoreUAP") \
    TEST_CLASS_PROPERTY(L"Kits.CorePackageComposition", L"Mobile") \
    TEST_CLASS_PROPERTY(L"Kits.RequiresReboot", L"False") \
    TEST_CLASS_PROPERTY(L"Kits.DevelopmentPhase", L"Development and Integration") \
    TEST_CLASS_PROPERTY(L"Kits.HasSupplementalContent", L"TRUE") \
    TEST_CLASS_PROPERTY(L"Kits.Parameter", L"ContentSource") \
    TEST_CLASS_PROPERTY(L"Kits.Parameter.ContentSource.IsRequired", L"TRUE") \
    TEST_CLASS_PROPERTY(L"Kits.Parameter.ContentSource.Description", L"Path to source File") \
    TEST_CLASS_PROPERTY(L"Kits.Parameter", L"GUID") \
    TEST_CLASS_PROPERTY(L"Kits.Parameter.GUID.Default", L"#{DETECT}#") \
    TEST_CLASS_PROPERTY(L"Kits.Parameter.GUID.IsRequired", L"TRUE") \
    TEST_CLASS_PROPERTY(L"Kits.PublishingOrganization", L"Microsoft Corporation") \
    TEST_METHOD_PROPERTY(L"Kits.Specification", L"Device.Streaming.HMFT.Mobile.Encoding") \
    TEST_METHOD_PROPERTY(L"Kits.Specification", L"Device.Streaming.HMFT.OneCoreUAP.Encoding") \

#define COMMON_DESKTOP_PROPERTIES \
    TEST_CLASS_PROPERTY(L"Kits.SupportedOS", L"Windows v10.0 Client x86") \
    TEST_CLASS_PROPERTY(L"Kits.SupportedOS", L"Windows v10.0 Client x64") \
    TEST_CLASS_PROPERTY(L"Kits.MinRelease", L"TH1") \
    TEST_CLASS_PROPERTY(L"Kits.CorePackageComposition", L"Full") \
    TEST_CLASS_PROPERTY(L"Kits.RequiresReboot", L"False") \
    TEST_CLASS_PROPERTY(L"Kits.DevelopmentPhase", L"Development and Integration") \
    TEST_CLASS_PROPERTY(L"Kits.HasSupplementalContent", L"TRUE") \
    TEST_CLASS_PROPERTY(L"Kits.Parameter", L"ContentSource") \
    TEST_CLASS_PROPERTY(L"Kits.Parameter.ContentSource.IsRequired", L"TRUE") \
    TEST_CLASS_PROPERTY(L"Kits.Parameter.ContentSource.Description", L"Path to source File") \
    TEST_CLASS_PROPERTY(L"Kits.Parameter", L"GUID") \
    TEST_CLASS_PROPERTY(L"Kits.Parameter.GUID.Default", L"#{DETECT}#") \
    TEST_CLASS_PROPERTY(L"Kits.Parameter.GUID.IsRequired", L"TRUE") \
    TEST_CLASS_PROPERTY(L"Kits.PublishingOrganization", L"Microsoft Corporation") \
    TEST_METHOD_PROPERTY(L"Kits.Specification", L"Device.Streaming.HMFT.Encoding") \

class CMFTDiagVisValTAEF : public WEX::TestClass<CMFTDiagVisValTAEF>, public CMFTDTaefBaseWithActivate
{
public:
    CMFTDiagVisValTAEF(void);
    ~CMFTDiagVisValTAEF(void);

    BEGIN_TEST_CLASS(CMFTDiagVisValTAEF)
    END_TEST_CLASS()

    TEST_CLASS_SETUP(InitializeTestClass)
    TEST_CLASS_CLEANUP(FinalizeTestClass)

    BEGIN_TEST_METHOD(VisValPlaybackTest61) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"61") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 

 
    BEGIN_TEST_METHOD(VisValPlaybackTest62) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"62") 
        TEST_METHOD_PROPERTY(L"DriverDrop", L"TRUE") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"2")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValPlaybackTest64) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"64") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValPlaybackTest65) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"65") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValPlaybackTest67) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"67") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValPlaybackTest68) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"68") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValPlaybackTest69) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"69") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValPlaybackTest70) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"70") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValPlaybackTest71) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"71") 
        TEST_METHOD_PROPERTY(L"DriverDrop", L"TRUE") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"2")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValPlaybackTest72) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"72") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValPlaybackTest73) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"73") 
        TEST_METHOD_PROPERTY(L"DriverDrop", L"TRUE") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"2")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValPlaybackTest74) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"74") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValPlaybackTest77) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"77") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValPlaybackTest78) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"78") 
        TEST_METHOD_PROPERTY(L"DriverDrop", L"TRUE") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"2")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValPlaybackTest79) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"79") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValPlaybackTest81) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"81") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValPlaybackTest82) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"82") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValPlaybackTest83) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"83") 
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValPlaybackTest84) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"84") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValPlaybackTest85) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"85") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValPlaybackTest86) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"86") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValPlaybackTest87) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"87") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValPlaybackTest88) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"88") 
        TEST_METHOD_PROPERTY(L"DriverDrop", L"TRUE") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"2")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValPlaybackTest89) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"89") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValPlaybackTest93) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MJPEG") 
        TEST_METHOD_PROPERTY(L"ID",         L"93") 
        TEST_METHOD_PROPERTY(L"DriverDrop", L"TRUE") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"2")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 

    BEGIN_TEST_METHOD(VisValPlaybackTest94) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MJPEG") 
        TEST_METHOD_PROPERTY(L"ID",         L"94") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 

    BEGIN_TEST_METHOD(VisValPlaybackTest95) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MJPEG") 
        TEST_METHOD_PROPERTY(L"ID",         L"95") 
        TEST_METHOD_PROPERTY(L"DriverDrop", L"TRUE") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"2")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 

    BEGIN_TEST_METHOD(VisValPlaybackTest96) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MJPEG") 
        TEST_METHOD_PROPERTY(L"ID",         L"96") 
        TEST_METHOD_PROPERTY(L"DriverDrop", L"TRUE") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"2")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 

    BEGIN_TEST_METHOD(VisValPlaybackTest97) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MJPEG") 
        TEST_METHOD_PROPERTY(L"ID",         L"97") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 

    BEGIN_TEST_METHOD(VisValPlaybackTest98) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MJPEG") 
        TEST_METHOD_PROPERTY(L"ID",         L"98") 
        TEST_METHOD_PROPERTY(L"DriverDrop", L"TRUE") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"2")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 

    BEGIN_TEST_METHOD(VisValPlaybackTest99) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MJPEG") 
        TEST_METHOD_PROPERTY(L"ID",         L"99") 
        TEST_METHOD_PROPERTY(L"DriverDrop", L"TRUE") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"2")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 

    BEGIN_TEST_METHOD(VisValPlaybackTest100) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Playback") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MJPEG") 
        TEST_METHOD_PROPERTY(L"ID",         L"100") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValEncoderSettingsTest201)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"201") 
        TEST_METHOD_PROPERTY(L"DriverDrop", L"TRUE")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"2")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest202)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"202") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest203)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"203") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest204)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"204") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest205)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"205") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest206)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"206") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest207)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"207") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest208)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"WMV9") 
        TEST_METHOD_PROPERTY(L"ID",         L"208") 
        TEST_METHOD_PROPERTY(L"DriverDrop", L"TRUE") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"2")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest209)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"209") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest210)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"WMV9") 
        TEST_METHOD_PROPERTY(L"ID",         L"210") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest211)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"211") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest212)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"212") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest213)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"213") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest214)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"214") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest215)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettingsPerSample") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"215") 
        TEST_METHOD_PROPERTY(L"DriverDrop", L"TRUE") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"2")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest216)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettingsPerSample") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"216") 
        TEST_METHOD_PROPERTY(L"DriverDrop", L"TRUE") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"2")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest217)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettingsPerSample") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"217") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest218)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"218") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest219)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"219") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest220)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"220") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest221)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"WMV9") 
        TEST_METHOD_PROPERTY(L"ID",         L"221") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest222)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"WMV9") 
        TEST_METHOD_PROPERTY(L"ID",         L"222") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValSeekTest376) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Seek") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"376") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValSeekTest377) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Seek") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"377") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValSeekTest378) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Seek") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"378") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValSeekTest379) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Seek") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"379") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValSeekTest380) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Seek") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"380") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValSeekTest381) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Seek") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"381") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValSeekTest382) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Seek") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"382") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValSeekTest383) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Seek") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"383") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValSeekTest384) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Seek") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"384") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValSeekTest385) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Seek") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"385") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValRateTest406) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Rate") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"406") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValRateTest407) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Rate") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"407") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValRateTest408) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Rate") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"408") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValRateTest409) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Rate") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"409") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValRateTest410) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Rate") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"410") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValRateTest411) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Rate") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"411") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValRateTest412) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Rate") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"412") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValRateTest413) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Rate") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"413") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValRateTest414) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Rate") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"414") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValRateTest415) 
        TEST_METHOD_PROPERTY(L"VisValType", L"Rate") 
        TEST_METHOD_PROPERTY(L"InputType",  L"MPEG4") 
        TEST_METHOD_PROPERTY(L"ID",         L"415") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD() 
 
    BEGIN_TEST_METHOD(VisValEncoderSettingsTest426)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"426") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest427)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"427") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest428)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"428") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest429)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"429") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest430)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"430") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest431)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"431") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest432)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"432") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest433)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"433") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest434)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"434") 
        TEST_METHOD_PROPERTY(L"DriverDrop", L"TRUE") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest435)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"435") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest436)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"436") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest437)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"437") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest438)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"438") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest439)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"439") 
        TEST_METHOD_PROPERTY(L"DriverDrop", L"TRUE") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"2")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest440)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"440") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest441)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"441") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest442)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"442") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest443)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"443") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest444)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"444") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest445)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"445") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest446)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"446") 
        TEST_METHOD_PROPERTY(L"DriverDrop", L"TRUE") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"2")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest447)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"447") 
        TEST_METHOD_PROPERTY(L"DriverDrop", L"TRUE") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"2")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest448)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"448") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest449)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"449") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest450)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"450") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest451)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"451") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest452)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"452") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest453)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"453") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest454)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"454") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest455)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"455") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest456)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"456") 
        TEST_METHOD_PROPERTY(L"DriverDrop", L"TRUE") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest457)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"457") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest458)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"458") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest459)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"459") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest460)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"460") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest461)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"461") 
        TEST_METHOD_PROPERTY(L"DriverDrop", L"TRUE") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"2")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest462)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"462") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest463)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"463") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest464)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"464") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest465)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"465") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest466)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"466") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest467)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"467") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest468)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"468") 
        TEST_METHOD_PROPERTY(L"DriverDrop", L"TRUE") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"2")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest469)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"469") 
        TEST_METHOD_PROPERTY(L"DriverDrop", L"TRUE") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"2")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderSettingsTest470)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"YUV") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"470") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest471)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"471") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest472)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"472") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest473)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"473") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest474)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"474") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest475)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"475") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest476)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"476") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest477)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"477") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest478)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"478") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest479)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"479") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest480)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"480") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest481)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"481") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest482)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"482") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest483)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"483") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()   

    BEGIN_TEST_METHOD(VisValQualityTest485)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"485") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest486)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"486") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest487)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"487") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest488)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"488") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest489)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"489") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest490)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"490") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest491)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"491") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest492)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"492") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest493)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"493") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest494)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"494") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest495)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"495") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest496)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"I420") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"496") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest497)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"I420") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"497") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest498)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"498") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest499)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"499") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest500)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"500") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest501)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"501") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest502)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"502") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest503)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"503") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest504)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"504") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest505)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"505") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest506)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"506") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest507)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"507") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest508)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"508") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest509)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"509") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest510)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"510") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()
    

    BEGIN_TEST_METHOD(VisValQualityTest512)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"512") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest513)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"513") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest514)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"514") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest515)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"515") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest516)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"516") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest517)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"517") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest518)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"518") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest519)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"519") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest520)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"520") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest521)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"521") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest522)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"WMV9") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"522") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest523)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"I420") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"523") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValQualityTest524)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderQuality") 
        TEST_METHOD_PROPERTY(L"InputType",  L"I420") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"524") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderPerformanceTest525)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderPerformance") 
        TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"525") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderPerformanceTestWOA526)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderPerformanceWOA") 
        TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"526") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderPerformanceTest527)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderPerformance") 
        TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
        TEST_METHOD_PROPERTY(L"OutputType", L"VC-1") 
        TEST_METHOD_PROPERTY(L"ID",         L"527") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderDynamicSettingsTest600)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynamicSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"600") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderDynamicSettingsTest601)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynamicSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"601") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderDynamicSettingsTest602)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynamicSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"602") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderDynamicSettingsTest603)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynamicSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"603") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderDynamicSettingsTest604)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynamicSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"604") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderDynamicSettingsTest605)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynamicSettings") 
        TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"605") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderWinBlueTest606)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"606") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest607)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"607") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest608)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"608") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest609)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"609") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest610)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"610") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest611)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"611") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest612)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"612") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest613)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"613") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest614)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"614") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest615)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"615") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest616)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"616") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest617)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"617") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest618)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"618") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest619)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"619") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest620)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"620") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest621)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"621") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()
   
   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest622)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"622") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest623)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"623") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest624)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"624") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest625)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"625") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest626)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"626") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest627)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"627") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest628)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"628") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest629)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"629") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest630)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"630") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest631)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"631") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest632)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"632") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest633)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"633") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest634)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"634") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderTemporalLayersTest640)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderTemporalLayers") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"640") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderTemporalLayersTest641)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderTemporalLayers") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"641") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderTemporalLayersTest642)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderTemporalLayers") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"642") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderLTRTest650)
        TEST_METHOD_PROPERTY(L"VisValType", L"EncoderLTR") 
        TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
        TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
        TEST_METHOD_PROPERTY(L"ID",         L"650") 
        TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
        TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
    END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderLTRTest651)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderLTR") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"651") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderLTRTest652)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderLTR") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"652") 
       TEST_METHOD_PROPERTY(L"DriverDrop", L"TRUE") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderLTRTest653)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderLTR") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"653") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderLTRTest654)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderLTR") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"654") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderLTRTest655)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderLTR") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"655") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderLTRTest656)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderLTR") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"656") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderLTRRDCurveTest657)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderLTRRDCurve") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"657") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderLTRRDCurveTest658)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderLTRRDCurve") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"658") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderLTRRDCurveTest659)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderLTRRDCurve") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"659") 
       TEST_METHOD_PROPERTY(L"DriverDrop", L"TRUE") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"2")
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderLTRTest660)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderLTR") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"660") 
       TEST_METHOD_PROPERTY(L"DriverDrop", L"TRUE") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"2")
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderLTRTest661)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderLTR") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"661") 
       TEST_METHOD_PROPERTY(L"DriverDrop", L"TRUE") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"2")
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderLTRTest662)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderLTR") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"662") 
       TEST_METHOD_PROPERTY(L"DriverDrop", L"TRUE") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"2")
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderTemporalLayersTest675)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderTemporalLayers") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"675") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest676)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"676") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest677)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"677") 
       TEST_METHOD_PROPERTY(L"DriverDrop", L"TRUE") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"2")
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest678)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"678") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest679)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"679") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"3")
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest680)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"680") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest681)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"681") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest682)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"682") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest683)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"683") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest684)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"684") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest685)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"685") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest686)
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"686") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1200)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_FastCam_Action_03_1920x1088p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H264")
   TEST_METHOD_PROPERTY(L"ID", L"1200")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"892DE4F9-AEF3-4B3A-8C3C-C30C64357A80")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"91144adc-dbf5-4b13-a7c2-e5c9f2ba7fc2")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Basic Encode Test #1200")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"892DE4F9-AEF3-4B3A-8C3C-C30C64357A7F")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"29dfa485-f9b4-48cc-ba23-ed10587388d8")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Basic Encode Test #1200")
#endif

   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Basic Encode Test #1200 - 1080p encode using quality rate control, high profile, GOP 30")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1201)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_DifDirect_01_1280x720p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H264")
   TEST_METHOD_PROPERTY(L"ID", L"1201")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"89612A78-DFE0-4CCF-A70D-59F8E58E634A")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"6ddf498d-27d6-4e20-8f49-0a4725d4c8de")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Basic Encode Test #1201")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"89612A78-DFE0-4CCF-A70D-59F8E58E6349")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"e6243b72-2b4b-48a6-ac12-a6d09747d8ba")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Basic Encode Test #1201")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")  
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Basic Encode Test #1201 - 720p encode using quality rate control, main profile, GOP 30")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1202)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Texture_High_01_320x240p30f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H264")
   TEST_METHOD_PROPERTY(L"ID", L"1202")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"AD3EC879-DFA1-46B4-8DD1-49771720E3B9")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"e01f45e3-ed04-4350-9442-ea6733881eac")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Basic Encode Test #1202")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"AD3EC879-DFA1-46B4-8DD1-49771720E3B8")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"095951e8-1beb-42d3-a326-c2b7123e889b")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Basic Encode Test #1202")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")   
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Basic Encode Test #1202 - 240p encode using VBR rate control @ 500 kbps, base profile, GOP 60")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1203)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_DifDirect_01_1280x720p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H264")
   TEST_METHOD_PROPERTY(L"ID", L"1203")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"5640D477-8FFD-4A7C-9847-CC3510DDCAEA")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"cba7c70f-d9d9-4f76-a4a0-2f0d517ed41a")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Basic Encode Test #1203")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"5640D477-8FFD-4A7C-9847-CC3510DDCAE9")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"81d8aec3-fb76-418a-a842-80d5b6a3bcc5")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Basic Encode Test #1203")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Basic Encode Test #1203 - 720p encode using CBR rate control @ 6 mbps, main profile, GOP 90")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1204)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Difficult_Edge_02_720x480p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H264")
   TEST_METHOD_PROPERTY(L"ID", L"1204")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"1C254FCA-D728-4437-B8C2-F6B0F73935CC")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"4fb150f3-507d-46f7-9453-8738cdb74ac6")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Basic Encode Test #1204")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"1C254FCA-D728-4437-B8C2-F6B0F73935CB")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"0116ed56-cbe2-4adc-8bc2-ed0e88ad87cb")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Basic Encode Test #1204")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Basic Encode Test #1204 - 480p encode using CBR rate control @ 1 mbps, main profile, GOP 60")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1205)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Texture_High_01_320x240p30f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H264")
   TEST_METHOD_PROPERTY(L"ID", L"1205")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"908C39DF-FAB2-4123-9F6F-6D545F17552B")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"f01f2a87-cb35-4672-88ad-5ad4be345c1d")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Basic Encode Test #1205")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"908C39DF-FAB2-4123-9F6F-6D545F17552A")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"5a403eaf-42b4-47f5-9a06-ed76c1e1c52e")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Basic Encode Test #1205")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Basic Encode Test #1205 - Check GetParameterRange and GetParameterValues")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1206)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_FastCam_Action_03_1920x1088p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H265")
   TEST_METHOD_PROPERTY(L"ID", L"1206")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"DD2F4D1C-6BDF-4C6E-8AC5-237389677D70")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"1e017cd8-6013-4f9b-b5bd-e70351c1ed3d")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Mobile Basic Encode Test #1206")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"DD2F4D1C-6BDF-4C6E-8AC5-237389677D6F")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"0e09c7db-4006-4ab5-9092-bd5cabd424f1")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Basic Encode Test #1206")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")   
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H265 Basic Encode Test #1206 - 1080p encode using quality rate control, main profile, GOP 30")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1207)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_DifDirect_01_1280x720p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H265")
   TEST_METHOD_PROPERTY(L"ID", L"1207")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"8E4F219B-5376-43BA-AC34-815B13DC9066")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"93539b00-b2fb-42e1-bc94-ad9c7eb73063")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Mobile Basic Encode Test #1207")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"8E4F219B-5376-43BA-AC34-815B13DC9065")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"79b5b99c-9828-4d5d-aa6f-8746588de8fb")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Basic Encode Test #1207")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H265 Basic Encode Test #1207 - 720p encode using quality rate control, main profile, GOP 60")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1208)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Texture_High_01_320x240p30f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H265")
   TEST_METHOD_PROPERTY(L"ID", L"1208")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"11DE8B76-DE09-4435-9177-F340E67DD181")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"8b752c31-4062-480d-9411-e56a992f4dcc")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Mobile Basic Encode Test #1208")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"11DE8B76-DE09-4435-9177-F340E67DD180")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"daf5489c-015f-4da1-9f65-2b32d9d139e8")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Basic Encode Test #1208")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H265 Basic Encode Test #1208 - 240p encode using VBR rate control @ 500 kbps, main profile, GOP 60")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1209)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_DifDirect_01_1280x720p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H265")
   TEST_METHOD_PROPERTY(L"ID", L"1209")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"19F444FF-716F-4118-9345-B7D7B9FD48E8")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"900a5a59-cfb7-4866-a1e7-311080e7bd78")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Mobile Basic Encode Test #1209")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"19F444FF-716F-4118-9345-B7D7B9FD48E7")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"ddcd3fde-8168-4254-9fce-3dfd69400466")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Basic Encode Test #1209")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H265 Basic Encode Test #1209 - 720p encode using CBR rate control @ 6 mbps, main profile, GOP 90")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1210)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Difficult_Edge_02_720x480p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H265")
   TEST_METHOD_PROPERTY(L"ID", L"1210")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"C4545F8C-FBDE-4F3E-B72F-081EF1F18E17")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"c1bb29aa-7fed-423a-9391-0547ecb3365b")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Mobile Basic Encode Test #1210")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"C4545F8C-FBDE-4F3E-B72F-081EF1F18E16")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"31edac0a-6358-4f21-9146-2349d72b5f3f")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Basic Encode Test #1210")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H265 Basic Encode Test #1210 - 480p encode using CBR rate control @ 1 mbps, main profile, GOP 60")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1211)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Texture_High_01_320x240p30f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H265")
   TEST_METHOD_PROPERTY(L"ID", L"1211")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"DFF8D9F9-3445-4B16-B68B-0EB103B13470")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"d06597d8-1bed-48c2-9e09-979490791f67")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Mobile Basic Encode Test #1211")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"DFF8D9F9-3445-4B16-B68B-0EB103B1346F")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"117bf4cd-c69f-4659-ac92-b9cc980e3169")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Basic Encode Test #1211")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H265 Basic Encode Test #1211 - Check GetParameterRange and GetParameterValues")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1212)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Texture_High_01_320x240p30f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"VP9")
   TEST_METHOD_PROPERTY(L"ID", L"1212")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"4ec05d66-e0cb-4682-bf75-0bdf0151fd8a")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"1d251151-5941-4e47-95d0-bd266ff54f82")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"VP9 Mobile Basic Encode Test #1212")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"d81afe79-cf57-43c1-997a-dedbd6f27ec2")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"0256b249-1d73-4c04-a030-8cb77d346e85")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"VP9 Basic Encode Test #1212")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 Basic Encode Test #1212 - 240p encode using quality rate control, main profile, GOP 30")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1213)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_DifDirect_01_1280x720p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"VP9")
   TEST_METHOD_PROPERTY(L"ID", L"1213")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"0ebd5dd6-dc5b-4461-bb88-2e865ea4f2ce")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"7bfb3653-b7a2-4c37-9cb8-5a79bac6b6c9")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"VP9 Mobile Basic Encode Test #1213")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"4e174337-8f9e-4d30-93dc-e57f70c19c84")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"d363589a-8b46-4c4b-873c-e8faee3d1ad3")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"VP9 Basic Encode Test #1213")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 Basic Encode Test #1213 - 720p encode using quality rate control, main profile, GOP 60")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1214)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Difficult_Edge_02_720x480p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"VP9")
   TEST_METHOD_PROPERTY(L"ID", L"1214")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"d7fae442-a96d-4531-b18d-fe207525274b")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"c418bc93-112a-43ab-b67e-77caaecbe86e")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"VP9 Mobile Basic Encode Test #1214")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"e6af394f-87a8-451d-b6da-59bb47b16702")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"ce4aad86-f398-40ae-8010-6fee25be733f")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"VP9 Basic Encode Test #1214")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 Basic Encode Test #1214 - 480p encode using peak VBR rate control @ 1 Mbps, main profile, Infinite GOP")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1215)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_FastCam_Action_03_1920x1088p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"VP9")
   TEST_METHOD_PROPERTY(L"ID", L"1215")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"d8b2df64-85cc-4597-9b0d-f984388bbecc")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"8324f797-a521-41f4-b8dd-7a77c1edd892")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"VP9 Mobile Basic Encode Test #1215")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"ce22be7f-db25-489f-9ffc-91964240e4db")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"40a8adc8-999b-4f78-863a-13790a9fcec8")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"VP9 Basic Encode Test #1215")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 Basic Encode Test #1215 - 1080p encode using CBR rate control @ 8 mbps, main profile, GOP 90")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder")
   END_TEST_METHOD()

    BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1300)
    TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Difficult_Edge_02_720x480p24f.mp4")
#ifndef __NEW_METADATA__
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"1300") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
#endif

#ifdef WINPHONE
       COMMON_MOBILE_PROPERTIES
       TEST_METHOD_PROPERTY(L"Kits.TestId", L"19F7A75E-ECA5-4BFD-9825-7AF10E0E9817")
       TEST_METHOD_PROPERTY(L"Kits.TestId2", L"4bccdc04-7330-45a5-8a26-38685e5a9093")
#else
       COMMON_DESKTOP_PROPERTIES
       TEST_METHOD_PROPERTY(L"Kits.TestId", L"19F7A75E-ECA5-4BFD-9825-7AF10E0E9816")
       TEST_METHOD_PROPERTY(L"Kits.TestId2", L"133763f4-fc6c-49ff-8946-4ef11d73b987")
#endif
       TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Miracast Encode Test #1300")
       TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
       TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
       TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
       TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Miracast Encode Test #1300 - Scenario Info Test 1")
       TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
       TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1301)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Difficult_Edge_02_720x480p24f.mp4")
#ifndef __NEW_METADATA__
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"1301") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
#endif

#ifdef WINPHONE
       COMMON_MOBILE_PROPERTIES
       TEST_METHOD_PROPERTY(L"Kits.TestId", L"3A66D256-D0D3-4E58-8122-DD0C867EE3D4")
       TEST_METHOD_PROPERTY(L"Kits.TestId2", L"787910d3-1f7b-4c57-9735-200661b73a08")
#else
       COMMON_DESKTOP_PROPERTIES
       TEST_METHOD_PROPERTY(L"Kits.TestId", L"3A66D256-D0D3-4E58-8122-DD0C867EE3D3")
       TEST_METHOD_PROPERTY(L"Kits.TestId2", L"61332665-3925-4df2-8691-11d7ae51ceba")
#endif
       TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Miracast Encode Test #1301")
       TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
       TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
       TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
       TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Miracast Encode Test #1301 - Scenario Info Test 2")
       TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
       TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1302)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Difficult_Edge_02_720x480p24f.mp4")
#ifndef __NEW_METADATA__
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"1302") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
#endif

#ifdef WINPHONE
       COMMON_MOBILE_PROPERTIES
       TEST_METHOD_PROPERTY(L"Kits.TestId", L"33C95067-8600-4582-A5FA-8EF672C11E66")
       TEST_METHOD_PROPERTY(L"Kits.TestId2", L"55e52ccf-b646-4afc-9597-23905aef1f98")
#else
       COMMON_DESKTOP_PROPERTIES
       TEST_METHOD_PROPERTY(L"Kits.TestId", L"33C95067-8600-4582-A5FA-8EF672C11E65")
       TEST_METHOD_PROPERTY(L"Kits.TestId2", L"19563500-c609-4d0f-b2d1-0753538e5dd4")
#endif
       TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Miracast Encode Test #1302")
       TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
       TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
       TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
       TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Miracast Encode Test #1302 - Scenario Info Test 3")
       TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
       TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1303)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Difficult_Edge_02_720x480p24f.mp4")
#ifndef __NEW_METADATA__
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"1303") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
#endif

#ifdef WINPHONE
       COMMON_MOBILE_PROPERTIES
       TEST_METHOD_PROPERTY(L"Kits.TestId", L"16D7027E-A16B-4418-8F93-D0FBA5EB09A7")
       TEST_METHOD_PROPERTY(L"Kits.TestId2", L"99db55a9-7680-4001-84e8-c67d7fa67512")
#else
       COMMON_DESKTOP_PROPERTIES
       TEST_METHOD_PROPERTY(L"Kits.TestId", L"16D7027E-A16B-4418-8F93-D0FBA5EB09A6")
       TEST_METHOD_PROPERTY(L"Kits.TestId2", L"338a2abe-3333-48c4-b054-2b6e1a271f46")
#endif
       TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Miracast Encode Test #1303")
       TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
       TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
       TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
       TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Miracast Encode Test #1303 - Scenario Info Test 4")
    
       TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
       TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1304)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Difficult_Edge_02_720x480p24f.mp4")
#ifndef __NEW_METADATA__
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"1304") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
#endif

#ifdef WINPHONE
       COMMON_MOBILE_PROPERTIES
       TEST_METHOD_PROPERTY(L"Kits.TestId", L"0EE4BB81-30C1-4F03-8D44-2BCE7904311A")
       TEST_METHOD_PROPERTY(L"Kits.TestId2", L"a97987d8-cf72-4e60-b12b-bb9951456575")
#else
       COMMON_DESKTOP_PROPERTIES
       TEST_METHOD_PROPERTY(L"Kits.TestId", L"0EE4BB81-30C1-4F03-8D44-2BCE7904311B")
       TEST_METHOD_PROPERTY(L"Kits.TestId2", L"40a7aab7-614b-4a22-a3f1-36b5fccdef12")
#endif
       TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Miracast Encode Test #1304")
       TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
       TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
       TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
       TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Miracast Encode Test #1304 - Low Power Test")
       TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
       TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1305)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Difficult_Edge_02_720x480p24f.mp4")
#ifndef __NEW_METADATA__
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"1305") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
#endif

#ifdef WINPHONE
       COMMON_MOBILE_PROPERTIES
       TEST_METHOD_PROPERTY(L"Kits.TestId", L"156B8D3C-5A89-41EC-BA07-2F55724A8F0E")
       TEST_METHOD_PROPERTY(L"Kits.TestId2", L"1275ffe0-e02b-4465-ba24-4a0ea9142f3f")
#else
       COMMON_DESKTOP_PROPERTIES
       TEST_METHOD_PROPERTY(L"Kits.TestId", L"156B8D3C-5A89-41EC-BA07-2F55724A8F0D")
       TEST_METHOD_PROPERTY(L"Kits.TestId2", L"1c5987f1-38bd-4cf2-969b-668e7c667529")
#endif
       TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Miracast Encode Test #1305")
       TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
       TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
       TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
       TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Miracast Encode Test #1305 - Intra Refresh Mode 1 Test")
       TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
       TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1306)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Difficult_Edge_02_720x480p24f.mp4")
#ifndef __NEW_METADATA__
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"NV12") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"1306") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
#endif

#ifdef WINPHONE
       COMMON_MOBILE_PROPERTIES
       TEST_METHOD_PROPERTY(L"Kits.TestId", L"E8B308E3-C45B-4C56-A23E-98FBACD73C9E")
       TEST_METHOD_PROPERTY(L"Kits.TestId2", L"e702fe41-2bce-42ef-bcaf-1fdbac618613")
#else
       COMMON_DESKTOP_PROPERTIES
       TEST_METHOD_PROPERTY(L"Kits.TestId", L"E8B308E3-C45B-4C56-A23E-98FBACD73C9D")
       TEST_METHOD_PROPERTY(L"Kits.TestId2", L"a3ce3374-754f-440b-87a5-5aa30b8cb836")
#endif
       TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Miracast Encode Test #1306")
       TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
       TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
       TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
       TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Miracast Encode Test #1306 - Intra Refresh Mode 2 Test")
       TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
       TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1307)
       TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Difficult_Edge_02_720x480p24f.mp4")
#ifndef __NEW_METADATA__
       TEST_METHOD_PROPERTY(L"VisValType", L"EncoderWinBlue") 
       TEST_METHOD_PROPERTY(L"InputType",  L"ARGB32") 
       TEST_METHOD_PROPERTY(L"OutputType", L"H264") 
       TEST_METHOD_PROPERTY(L"ID",         L"1307") 
       TEST_METHOD_PROPERTY(L"TestLevel",  L"4")
#endif

#ifdef WINPHONE
       COMMON_MOBILE_PROPERTIES
       TEST_METHOD_PROPERTY(L"Kits.TestId", L"56DBB594-5E22-4D66-B097-6CC81991AD7C")
       TEST_METHOD_PROPERTY(L"Kits.TestId2", L"3ba1478b-c445-4eca-a586-1d102d98b050")
#else
       COMMON_DESKTOP_PROPERTIES
       TEST_METHOD_PROPERTY(L"Kits.TestId", L"56DBB594-5E22-4D66-B097-6CC81991AD7B")
       TEST_METHOD_PROPERTY(L"Kits.TestId2", L"56bd3e77-602d-4d7c-9aa8-43f63167f09c")
#endif
       TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Miracast Encode Test #1307")
       TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
       TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
       TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
       TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Miracast Encode Test #1307 - ARGB32 Input Type Support Test")
       TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
       TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1400)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_FastCam_Action_03_1920x1088p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H264")
   TEST_METHOD_PROPERTY(L"ID", L"1400")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"A15E50B8-2B77-48D7-81E0-C4B5AF9BB18D")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"0de30c1a-5c4c-4fdc-89b9-0144e14f771d")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Intermediate Encode Test #1400")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"A15E50B8-2B77-48D7-81E0-C4B5AF9BB18C")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"35de6e71-eefe-4499-9f9d-9b42350b0d2b")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Intermediate Encode Test #1400")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Intermediate Encode Test #1400 - 1080p encode using quality rate control, constrainedhigh profile, cabac, low latency, GOP 30")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1401)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_FastCam_Action_03_1920x1088p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H264")
   TEST_METHOD_PROPERTY(L"ID", L"1401")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"8979B4B0-4276-4F9D-8527-BEDF2F84484F")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"7aad1ec5-c0ba-4b3a-98ba-ccfa4f1adbe2")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Intermediate Encode Test #1401")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"8979B4B0-4276-4F9D-8527-BEDF2F84484E")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"e48a81dc-34ff-4387-80fd-b48a7dd205c8")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Intermediate Encode Test #1401")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Intermediate Encode Test #1401 - 1080p encode using CBR @ 10 mbps, constrainedbase profile, low latency, GOP 60")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1402)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_DifDirect_01_1280x720p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H264")
   TEST_METHOD_PROPERTY(L"ID", L"1402")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"5553D6CB-5E83-4BC8-A747-0223701568FC")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"895903c4-89db-4378-a7fa-3f17c4643ca9")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Intermediate Encode Test #1402")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"5553D6CB-5E83-4BC8-A747-0223701568FB")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"5af92cca-5cd1-46b2-af12-0ecdd40d184e")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Intermediate Encode Test #1402")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Intermediate Encode Test #1402 - 720p encode using CBR rate control @ 5 mbps, constrainedhigh profile, cabac, low latency, GOP 60")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1403)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_DifDirect_01_1280x720p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H264")
   TEST_METHOD_PROPERTY(L"ID", L"1403")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"E762E8C5-CB72-4104-AB87-6A0B79F15338")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"59b683a8-e7fe-450e-8176-b32964559acc")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Intermediate Encode Test #1403")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"E762E8C5-CB72-4104-AB87-6A0B79F15337")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"dcf52d22-15fa-4723-a3fe-ba5ed6fb9219")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Intermediate Encode Test #1403")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Intermediate Encode Test #1403 - 720p encode using vbr rate control @ 6 mbps, high profile, cabac, 2 b frames, GOP 90")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1404)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_FastCam_Action_03_1920x1088p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H264")
   TEST_METHOD_PROPERTY(L"ID", L"1404")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"B967D9D4-C395-4DC1-9B25-893E0CA45021")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"e652a2b5-8f6e-412d-8508-e8a1e8d41413")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Intermediate Encode Test #1404")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"B967D9D4-C395-4DC1-9B25-893E0CA45020")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"97b4fdb6-3012-4188-94f2-519d83673756")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Intermediate Encode Test #1404")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Intermediate Encode Test #1404 - 1080p encode using PeakVBR rate control @ 8 mbps, main profile, 1 b frame, GOP 60")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1405)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_FastCam_Action_03_1920x1088p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H265")
   TEST_METHOD_PROPERTY(L"ID", L"1405")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"A33036E9-C03F-44B0-9955-B974C75C17AB")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"70d0611f-6878-462e-b2ac-baae5684034b")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Mobile Intermediate Encode Test #1405")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"A33036E9-C03F-44B0-9955-B974C75C17AA")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"885842ce-be32-4d2d-a4c5-0005bd579f5a")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Intermediate Encode Test #1405")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H265 Intermediate Encode Test #1405 - 1080p encode using quality rate control, main profile, low latency, GOP 30")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1406)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_FastCam_Action_03_1920x1088p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H265")
   TEST_METHOD_PROPERTY(L"ID", L"1406")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"9E752676-5ADD-408B-BAE9-7803C4BED591")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"f2451613-cc02-46e9-9552-8b1756029402")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Mobile Intermediate Encode Test #1406")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"9E752676-5ADD-408B-BAE9-7803C4BED590")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"be68b7e0-22a0-4856-806a-6a3e22a4ef29")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Intermediate Encode Test #1406")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H265 Intermediate Encode Test #1406 - 1080p encode using CBR @ 10 mbps, main profile, low latency, GOP 60")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1407)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_DifDirect_01_1280x720p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H265")
   TEST_METHOD_PROPERTY(L"ID", L"1407")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"F8ADB450-6A13-4E91-B727-BD25CAA33BD6")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"6a8187a5-802b-4b49-a4eb-509ce87bbead")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Mobile Intermediate Encode Test #1407")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"F8ADB450-6A13-4E91-B727-BD25CAA33BD5")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"e1a10efd-29d1-4d9e-b61d-0a7ad22dfd6f")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Intermediate Encode Test #1407")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H265 Intermediate Encode Test #1407 - 720p encode using CBR rate control @ 5 mbps, main profile, low latency, GOP 60")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1408)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_DifDirect_01_1280x720p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H265")
   TEST_METHOD_PROPERTY(L"ID", L"1408")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"1CD36984-705F-4911-B280-1FC1FD13B188")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"4953f739-3fa5-4b30-9290-8fb2865aef3a")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Mobile Intermediate Encode Test #1408")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"1CD36984-705F-4911-B280-1FC1FD13B187")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"53de472a-fe1f-4976-8611-ea8599b1c95d")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Intermediate Encode Test #1408")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H265 Intermediate Encode Test #1408 - 720p encode using vbr rate control @ 6 mbps, main profile, 2 b frames, GOP 90")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1409)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_FastCam_Action_03_3840x2160p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H265")
   TEST_METHOD_PROPERTY(L"ID", L"1409")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"DB94E2DF-FA18-4E82-A573-DCB3E9483CE0")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"8d7be909-c7f9-4545-9f22-50c8af3845a2")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Mobile Intermediate Encode Test #1409")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"DB94E2DF-FA18-4E82-A573-DCB3E9483CDF")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"1c2df096-0a95-4067-8a60-63794b067543")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Intermediate Encode Test #1409")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H265 Intermediate Encode Test #1409 - 1080p encode using PeakVBR rate control @ 8 mbps, main profile, 1 b frame, GOP 60")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1500)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Texture_High_01_320x240p30f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderFastQuality")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H264")
   TEST_METHOD_PROPERTY(L"ID", L"1500")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"F796FC13-3D47-45C4-A886-F50592E32D8F")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"9b905da1-c40b-4b78-b8e2-0aa6ec6420df")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Encode Quality Test #1500")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"F796FC13-3D47-45C4-A886-F50592E32D8E")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"1bdde864-7a9e-4171-a62a-6c3b6a0cee62")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Encode Quality Test #1500")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Encode Quality Test #1500 - RD Curve at 240p")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1501)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Difficult_Edge_02_720x480p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderFastQuality")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H264")
   TEST_METHOD_PROPERTY(L"ID", L"1501")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"D55F7DB3-CEAC-47B4-8391-0EF51295898A")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"34dbe7dc-9475-44d1-94a3-d3a49723b244")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Encode Quality Test Test #1501")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"D55F7DB3-CEAC-47B4-8391-0EF512958989")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"ce08edb2-200c-474c-ae3d-de0813160a18")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Encode Quality Test Test #1501")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Encode Quality Test #1501 - RD Curve at 480p")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1502)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_DifDirect_01_1280x720p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderFastQuality")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H264")
   TEST_METHOD_PROPERTY(L"ID", L"1502")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"4D6DB826-40A1-4295-94BF-C04B02B9AAE4")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"8d5b7e37-6290-4f5f-ba34-6648572bc0b2")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Encode Quality Test Test #1502")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"4D6DB826-40A1-4295-94BF-C04B02B9AAE3")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"549d61bf-a099-4523-8cc3-cee1dba6f409")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Encode Quality Test Test #1502")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Encode Quality Test #1502 - RD Curve at 720p")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1503)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_FastCam_Action_03_1920x1088p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderFastQuality")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H264")
   TEST_METHOD_PROPERTY(L"ID", L"1503")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"6BA5723D-FBA0-46F6-97F7-2FE49B76B233")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"74bb94b5-526c-4152-8fbb-6d6c0c372446")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Encode Quality Test Test #1503")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"6BA5723D-FBA0-46F6-97F7-2FE49B76B232")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"49b4e026-ed99-4d7a-b283-08e5bd4d7143")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Encode Quality Test Test #1503")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Encode Quality Test #1503 - RD Curve at 1080p")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1504)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_FastCam_Action_03_1920x1088p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderFastQuality")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H265")
   TEST_METHOD_PROPERTY(L"ID", L"1504")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"AFEEEB96-C3E8-4454-80E2-3143D84CB718")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"63e6130e-eabc-4e57-94f1-fcea67933c91")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Encode Quality Miracast Test #1504")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"AFEEEB96-C3E8-4454-80E2-3143D84CB717")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"d0e129a6-6b4d-4185-84e6-8b447fda1702")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Encode Quality Miracast Test #1504")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Encode Quality Miracast Test #1504 - RD Curve at 1080p")

   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1505)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Texture_High_01_320x240p30f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderFastQuality")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H265")
   TEST_METHOD_PROPERTY(L"ID", L"1505")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"942658D4-0367-4E76-8857-9283E5523868")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"85791d71-1f19-4188-bdd8-6b9650b77ea0")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Mobile Encode Quality Test #1505")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"942658D4-0367-4E76-8857-9283E5523867")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"e84b77b8-f74c-4157-ab05-b0d05aa54c69")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Encode Quality Test #1505")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H265 Encode Quality Test #1505 - RD Curve at 240p")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1506)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Difficult_Edge_02_720x480p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderFastQuality")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H265")
   TEST_METHOD_PROPERTY(L"ID", L"1506")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"CC72C2F1-432E-45C0-A48D-74CED5F8A33D")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"b880d660-137c-420d-87d7-ee603af3963d")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Mobile Encode Quality Test #1506")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"CC72C2F1-432E-45C0-A48D-74CED5F8A33C")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"2d05cc60-2fb2-40e7-aac9-6070f7b7db00")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Encode Quality Test #1506")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H265 Encode Quality Test #1506 - RD Curve at 480p")

   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1507)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_DifDirect_01_1280x720p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderFastQuality")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H265")
   TEST_METHOD_PROPERTY(L"ID", L"1507")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"B9F46E07-D697-41D1-A5EA-7997B982DACD")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"32e45592-bb1c-4508-b0d0-981f84732ef1")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Mobile Encode Quality Test #1507")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"B9F46E07-D697-41D1-A5EA-7997B982DACC")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"72baeb94-57bb-4b42-a574-b53b0f642cfd")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Encode Quality Test #1507")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"30")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H265 Encode Quality Test #1507 - RD Curve at 720p")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1508)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_FastCam_Action_03_1920x1088p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderFastQuality")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H265")
   TEST_METHOD_PROPERTY(L"ID", L"1508")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"28B3EC3A-907D-46FE-B553-96BF706EA6AA")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"6c95176e-fecb-4ce6-9218-162c95ba8ee5")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Mobile Encode Quality Test #1508")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"28B3EC3A-907D-46FE-B553-96BF706EA6A9")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"6af5dd43-9b6f-4c37-accd-775cc421f9fe")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H265 Encode Quality Test #1508")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"30")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H265 Encode Quality Test #1508 - RD Curve at 1080p")

   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1509)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Texture_High_01_320x240p30f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderFastQuality")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"VP9")
   TEST_METHOD_PROPERTY(L"ID", L"1509")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"1329dc7d-2030-4d30-b742-f13e401ff68b")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"94501c7f-720e-4790-a4b1-125cb6a92e92")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"VP9 Mobile Encode Quality Test #1509")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"789c31d3-bea5-4660-a617-7b78a0d2d9ee")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"853c57dd-cfc3-4d04-9f4d-7a8498a61ae4")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"VP9 Encode Quality Test #1509")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"30")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 Encode Quality Test #1509 - RD Curve at 1080p")

   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1510)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Difficult_Edge_02_720x480p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderFastQuality")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"VP9")
   TEST_METHOD_PROPERTY(L"ID", L"1510")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"7be66eb6-7592-44df-8b01-d1fa5c77eacd")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"d6b9e162-52e9-44f9-884c-e568431f4820")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"VP9 Mobile Encode Quality Test #1510")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"6ac7ccae-eed6-49d3-9cae-7176444ced33")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"f5ec37eb-78ad-4d5e-b85f-1e5d3cd4e6a9")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"VP9 Encode Quality Test #1510")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 Encode Quality Test #1510 - RD Curve at 480p")

   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1511)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_DifDirect_01_1280x720p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderFastQuality")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"VP9")
   TEST_METHOD_PROPERTY(L"ID", L"1511")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"295d3c9f-6b99-40d3-829e-57d722fcd939")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"27d9f6e5-c58d-4a82-9a19-ae784f6dbf63")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"VP9 Mobile Encode Quality Test #1511")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"dc6b7509-7c2f-4ab0-ae70-bfd4313b657c")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"6f2dd2f3-5293-4fe4-8403-5436649eff13")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"VP9 Encode Quality Test #1511")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"30")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 Encode Quality Test #1511 - RD Curve at 720p")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1512)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_FastCam_Action_03_1920x1088p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderFastQuality")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"VP9")
   TEST_METHOD_PROPERTY(L"ID", L"1512")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"09216c80-3544-4ea6-8be4-b29965da0657")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"ddfd07ae-6643-4674-857b-4337aee7e5bd")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"VP9 Mobile Encode Quality Test #1512")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"e1eb2bfd-12d2-43c6-84cf-1ad57f5856c4")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"8a3f21aa-51b6-4bf2-b5e9-4ccd71145d88")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"VP9 Encode Quality Test #1512")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"30")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"VP9 Encode Quality Test #1512 - RD Curve at 1080p")

   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1600)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_FastCam_Action_03_1920x1088p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynamicType")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H264")
   TEST_METHOD_PROPERTY(L"ID", L"1600")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"710B19DC-E1C8-46DE-83B8-8CAC8BB33941")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"ecde4f0b-a54c-4564-a6f5-a7d400cdb042")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Advanced Encode Test #1600")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"710B19DC-E1C8-46DE-83B8-8CAC8BB33940")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"42e90606-7318-4318-83c9-10a5ef2223ea")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Advanced Encode Test #1600")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Advanced Encode Test # 1600 - Dynamic Type Test - Change every frame")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1601)
   TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"..\\HMFTContent\\MSHDRef_Motion_FastCam_Action_03_1920x1088p24f.mp4")
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynamicType")
   TEST_METHOD_PROPERTY(L"InputType", L"NV12")
   TEST_METHOD_PROPERTY(L"OutputType", L"H264")
   TEST_METHOD_PROPERTY(L"ID", L"1601")
   TEST_METHOD_PROPERTY(L"TestLevel", L"1")
   TEST_METHOD_PROPERTY(L"TestLevel", L"2")
#endif

#ifdef WINPHONE
   COMMON_MOBILE_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"907964EF-94D9-4558-98EB-DCD1E9D1C18F")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"bdafd01d-b08d-4d55-8d96-b7a468fb7ab0")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Mobile Advanced Encode Test #1601")
#else
   COMMON_DESKTOP_PROPERTIES
   TEST_METHOD_PROPERTY(L"Kits.TestId", L"907964EF-94D9-4558-98EB-DCD1E9D1C18E")
   TEST_METHOD_PROPERTY(L"Kits.TestId2", L"ee9474fb-6fa7-4fbd-b0de-3f1c0dacb464")
   TEST_METHOD_PROPERTY(L"Kits.TestName", L"H264 Advanced Encode Test #1601")
#endif
   TEST_METHOD_PROPERTY(L"Kits.TestType", L"Development")
   TEST_METHOD_PROPERTY(L"Kits.ExpectedRuntime", L"15")
   TEST_METHOD_PROPERTY(L"Kits.TimeoutInMinutes", L"15")
   TEST_METHOD_PROPERTY(L"Kits.Description", L"H264 Advanced Encode Test # 1601 - Dynamic Type Test - Change every 30 frames")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.ContentSource.Default", L"[WTT\\TestBinRoot]\\..\\HMFTContent")
   TEST_METHOD_PROPERTY(L"Kits.Parameter.GUID.Description", L"The GUID of the HMFT to test - {DETECT} should be used for certification, this can be overridden to test a specific encoder HMFT")
   END_TEST_METHOD()

   BEGIN_TEST_METHOD(VisValEncoderWinBlueTest1602)
#ifndef __NEW_METADATA__
   TEST_METHOD_PROPERTY(L"VisValType", L"EncoderDynSinkWriter")
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
   END_TEST_METHOD()

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
