//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
//*************************************************************************

#pragma prefast(push)
#pragma prefast(disable:28718, "disabling warning for test code")
#pragma prefast(push)
#pragma prefast(disable:26006, "disabling warning for test code")
#pragma prefast(push)
#pragma prefast(disable:26007, "disabling warning for test code")

#include "MFTestSinkInterface.h"
#include "H264ConfigParser.h"
#include "H264EncVerifier.h"
#include "H264EncMFTW8.h"
#include "MFLogHelper.h"
#include "CodecAPILogHelper.h"
#include "FuzzHelper.h"
#include "CodecAPIFuzzHelper.h"
#include "MFAttrsFuzzHelper.h"
#include "MediaFuzzerMFTInterface.h"
#include "CEncSinkWriterTests.h"
#include "CPKHash.h"
#include "vfwmsgs.h"
#include "cpudetect.h"

#define S_SKIPPED (S_FALSE + 1)

#ifndef MINWIN
#include <Shellapi.h>
#endif //#ifndef MINWIN


#define _CRT_RAND_S

#include "H264EncLayerExtract.h"

#define SAFE_CLOSE(x) \
    { \
    if((x) != NULL) \
        { \
        fclose(x); \
        (x) = NULL; \
        } \
    }

#define SAFEFREE(x) \
    { \
    if((x) != NULL) \
        { \
        free(x); \
        (x) = NULL; \
        } \
    }

#define EPSILON 0.1 // Max deviation of actual bitrate from bitrate set in config file (for unconstrained VBR encoding)
#define QP_RDCURVE_BITRATE 1000000
#define DYNAMIC_VISVAL_PSNR_BAR 30.0
#define ALLOWED_PSNR_LOWER_PERCENT_BOUND 10
#define HUNDREDS_NANO_PER_SEC 10000000L
#define ROI_ZERO_LEEWAY 0.2

#define E_SKIP                  _HRESULT_TYPEDEF_(0x800FF001L)

//Below defines are used to generate a random number between SSE2 & SSE 4.1 and the corresponding regkey is set on the fly
#define SSE_MAX 4
#define SSE_MIN 1

//Below GUID is used to set the private MF attribute that is used to dump reconstructed frames from the encoder
static const GUID H264ENC_DUMP_RECON = { 0x4808147B, 0x5028, 0x4743, { 0x8D, 0xFF, 0x6E, 0x7A, 0x1F, 0x82, 0x49, 0x96 } };
//const GUID* arrCodecAPI[] = {

// Frame number parse error tolerance for M2TS files
#define M2TS_PARSE_TOLERANCE 1

//This is for extra logging
//#define VERBOSE_LOGS 1


struct tag_CodecAPI
{
    const GUID* guid;
    const WCHAR* lpwstrName;
} arrCodecAPI[] =    
{
    {&CODECAPI_AVEncCommonFormatConstraint, L"CODECAPI_AVEncCommonFormatConstraint"},
    {&CODECAPI_GUID_AVEncCommonFormatUnSpecified, L"CODECAPI_GUID_AVEncCommonFormatUnSpecified"},
    {&CODECAPI_GUID_AVEncCommonFormatDVD_V, L"CODECAPI_GUID_AVEncCommonFormatDVD_V"},
    {&CODECAPI_GUID_AVEncCommonFormatDVD_DashVR, L"CODECAPI_GUID_AVEncCommonFormatDVD_DashVR"},
    {&CODECAPI_GUID_AVEncCommonFormatDVD_PlusVR, L"CODECAPI_GUID_AVEncCommonFormatDVD_PlusVR"},
    {&CODECAPI_GUID_AVEncCommonFormatVCD, L"CODECAPI_GUID_AVEncCommonFormatVCD"},
    {&CODECAPI_GUID_AVEncCommonFormatSVCD, L"CODECAPI_GUID_AVEncCommonFormatSVCD"},
    {&CODECAPI_GUID_AVEncCommonFormatATSC, L"CODECAPI_GUID_AVEncCommonFormatATSC"},
    {&CODECAPI_GUID_AVEncCommonFormatDVB, L"CODECAPI_GUID_AVEncCommonFormatDVB"},
    {&CODECAPI_GUID_AVEncCommonFormatMP3, L"CODECAPI_GUID_AVEncCommonFormatMP3"},
    {&CODECAPI_GUID_AVEncCommonFormatHighMAT, L"CODECAPI_GUID_AVEncCommonFormatHighMAT"},
    {&CODECAPI_GUID_AVEncCommonFormatHighMPV, L"CODECAPI_GUID_AVEncCommonFormatHighMPV"},
    {&CODECAPI_AVEncCodecType, L"CODECAPI_AVEncCodecType"},
    {&CODECAPI_GUID_AVEncMPEG1Video, L"CODECAPI_GUID_AVEncMPEG1Video"},
    {&CODECAPI_GUID_AVEncMPEG2Video, L"CODECAPI_GUID_AVEncMPEG2Video"},
    {&CODECAPI_GUID_AVEncMPEG1Audio, L"CODECAPI_GUID_AVEncMPEG1Audio"},
    {&CODECAPI_GUID_AVEncMPEG2Audio, L"CODECAPI_GUID_AVEncMPEG2Audio"},
    {&CODECAPI_GUID_AVEncWMV, L"CODECAPI_GUID_AVEncWMV"},
    {&CODECAPI_GUID_AVEndMPEG4Video, L"CODECAPI_GUID_AVEndMPEG4Video"},
    {&CODECAPI_GUID_AVEncH264Video, L"CODECAPI_GUID_AVEncH264Video"},
    {&CODECAPI_GUID_AVEncDV, L"CODECAPI_GUID_AVEncDV"},
    {&CODECAPI_GUID_AVEncWMAPro, L"CODECAPI_GUID_AVEncWMAPro"},
    {&CODECAPI_GUID_AVEncWMALossless, L"CODECAPI_GUID_AVEncWMALossless"},
    {&CODECAPI_GUID_AVEncWMAVoice, L"CODECAPI_GUID_AVEncWMAVoice"},
    {&CODECAPI_GUID_AVEncDolbyDigitalPro, L"CODECAPI_GUID_AVEncDolbyDigitalPro"},
    {&CODECAPI_GUID_AVEncDolbyDigitalConsumer, L"CODECAPI_GUID_AVEncDolbyDigitalConsumer"},
    {&CODECAPI_GUID_AVEncDolbyDigitalPlus, L"CODECAPI_GUID_AVEncDolbyDigitalPlus"},
    {&CODECAPI_GUID_AVEncDTSHD, L"CODECAPI_GUID_AVEncDTSHD"},
    {&CODECAPI_GUID_AVEncDTS, L"CODECAPI_GUID_AVEncDTS"},
    {&CODECAPI_GUID_AVEncMLP, L"CODECAPI_GUID_AVEncMLP"},
    {&CODECAPI_GUID_AVEncPCM, L"CODECAPI_GUID_AVEncPCM"},
    {&CODECAPI_GUID_AVEncSDDS, L"CODECAPI_GUID_AVEncSDDS"},
    {&CODECAPI_AVEncCommonRateControlMode, L"CODECAPI_AVEncCommonRateControlMode"},
    {&CODECAPI_AVEncCommonLowLatency, L"CODECAPI_AVEncCommonLowLatency"},
    {&CODECAPI_AVEncCommonMultipassMode, L"CODECAPI_AVEncCommonMultipassMode"},
    {&CODECAPI_AVEncCommonPassStart, L"CODECAPI_AVEncCommonPassStart"},
    {&CODECAPI_AVEncCommonPassEnd, L"CODECAPI_AVEncCommonPassEnd"},
    {&CODECAPI_AVEncCommonRealTime, L"CODECAPI_AVEncCommonRealTime"},
    {&CODECAPI_AVEncCommonQuality, L"CODECAPI_AVEncCommonQuality"},
    {&CODECAPI_AVEncCommonQualityVsSpeed, L"CODECAPI_AVEncCommonQualityVsSpeed"},
    {&CODECAPI_AVEncCommonTranscodeEncodingProfile, L"CODECAPI_AVEncCommonTranscodeEncodingProfile"},
    {&CODECAPI_AVEncCommonMeanBitRate, L"CODECAPI_AVEncCommonMeanBitRate"},
    {&CODECAPI_AVEncCommonMeanBitRateInterval, L"CODECAPI_AVEncCommonMeanBitRateInterval"},
    {&CODECAPI_AVEncCommonMaxBitRate, L"CODECAPI_AVEncCommonMaxBitRate"},
    {&CODECAPI_AVEncCommonMinBitRate, L"CODECAPI_AVEncCommonMinBitRate"},
    {&CODECAPI_AVEncCommonBufferSize, L"CODECAPI_AVEncCommonBufferSize"},
    {&CODECAPI_AVEncCommonBufferInLevel, L"CODECAPI_AVEncCommonBufferInLevel"},
    {&CODECAPI_AVEncCommonBufferOutLevel, L"CODECAPI_AVEncCommonBufferOutLevel"},
    {&CODECAPI_AVEncCommonStreamEndHandling, L"CODECAPI_AVEncCommonStreamEndHandling"},
    {&CODECAPI_AVEncStatCommonCompletedPasses, L"CODECAPI_AVEncStatCommonCompletedPasses"},
    {&CODECAPI_AVEncVideoOutputFrameRate, L"CODECAPI_AVEncVideoOutputFrameRate"},
    {&CODECAPI_AVEncVideoOutputFrameRateConversion, L"CODECAPI_AVEncVideoOutputFrameRateConversion"},
    {&CODECAPI_AVEncVideoPixelAspectRatio, L"CODECAPI_AVEncVideoPixelAspectRatio"},
    {&CODECAPI_AVDecVideoAcceleration_MPEG2, L"CODECAPI_AVDecVideoAcceleration_MPEG2"},
    {&CODECAPI_AVDecVideoAcceleration_H264, L"CODECAPI_AVDecVideoAcceleration_H264"},
    {&CODECAPI_AVDecVideoAcceleration_VC1, L"CODECAPI_AVDecVideoAcceleration_VC1"},
    {&CODECAPI_AVDecVideoProcDeinterlaceCSC, L"CODECAPI_AVDecVideoProcDeinterlaceCSC"},
    {&CODECAPI_AVEncVideoForceSourceScanType, L"CODECAPI_AVEncVideoForceSourceScanType"},
    {&CODECAPI_AVEncVideoNoOfFieldsToEncode, L"CODECAPI_AVEncVideoNoOfFieldsToEncode"},
    {&CODECAPI_AVEncVideoNoOfFieldsToSkip, L"CODECAPI_AVEncVideoNoOfFieldsToSkip"},
    {&CODECAPI_AVEncVideoEncodeDimension, L"CODECAPI_AVEncVideoEncodeDimension"},
    {&CODECAPI_AVEncVideoEncodeOffsetOrigin, L"CODECAPI_AVEncVideoEncodeOffsetOrigin"},
    {&CODECAPI_AVEncVideoDisplayDimension, L"CODECAPI_AVEncVideoDisplayDimension"},
    {&CODECAPI_AVEncVideoOutputScanType, L"CODECAPI_AVEncVideoOutputScanType"},
    {&CODECAPI_AVEncVideoInverseTelecineEnable, L"CODECAPI_AVEncVideoInverseTelecineEnable"},
    {&CODECAPI_AVEncVideoInverseTelecineThreshold, L"CODECAPI_AVEncVideoInverseTelecineThreshold"},
    {&CODECAPI_AVEncVideoSourceFilmContent, L"CODECAPI_AVEncVideoSourceFilmContent"},
    {&CODECAPI_AVEncVideoSourceIsBW, L"CODECAPI_AVEncVideoSourceIsBW"},
    {&CODECAPI_AVEncVideoFieldSwap, L"CODECAPI_AVEncVideoFieldSwap"},
    {&CODECAPI_AVEncVideoInputChromaResolution, L"CODECAPI_AVEncVideoInputChromaResolution"},
    {&CODECAPI_AVEncVideoOutputChromaResolution, L"CODECAPI_AVEncVideoOutputChromaResolution"},
    {&CODECAPI_AVEncVideoInputChromaSubsampling, L"CODECAPI_AVEncVideoInputChromaSubsampling"},
    {&CODECAPI_AVEncVideoOutputChromaSubsampling, L"CODECAPI_AVEncVideoOutputChromaSubsampling"},
    {&CODECAPI_AVEncVideoInputColorPrimaries, L"CODECAPI_AVEncVideoInputColorPrimaries"},
    {&CODECAPI_AVEncVideoOutputColorPrimaries, L"CODECAPI_AVEncVideoOutputColorPrimaries"},
    {&CODECAPI_AVEncVideoInputColorTransferFunction, L"CODECAPI_AVEncVideoInputColorTransferFunction"},
    {&CODECAPI_AVEncVideoOutputColorTransferFunction, L"CODECAPI_AVEncVideoOutputColorTransferFunction"},
    {&CODECAPI_AVEncVideoInputColorTransferMatrix, L"CODECAPI_AVEncVideoInputColorTransferMatrix"},
    {&CODECAPI_AVEncVideoOutputColorTransferMatrix, L"CODECAPI_AVEncVideoOutputColorTransferMatrix"},
    {&CODECAPI_AVEncVideoInputColorLighting, L"CODECAPI_AVEncVideoInputColorLighting"},
    {&CODECAPI_AVEncVideoOutputColorLighting, L"CODECAPI_AVEncVideoOutputColorLighting"},
    {&CODECAPI_AVEncVideoInputColorNominalRange, L"CODECAPI_AVEncVideoInputColorNominalRange"},
    {&CODECAPI_AVEncVideoOutputColorNominalRange, L"CODECAPI_AVEncVideoOutputColorNominalRange"},
    {&CODECAPI_AVEncInputVideoSystem, L"CODECAPI_AVEncInputVideoSystem"},
    {&CODECAPI_AVEncVideoHeaderDropFrame, L"CODECAPI_AVEncVideoHeaderDropFrame"},
    {&CODECAPI_AVEncVideoHeaderHours, L"CODECAPI_AVEncVideoHeaderHours"},
    {&CODECAPI_AVEncVideoHeaderMinutes, L"CODECAPI_AVEncVideoHeaderMinutes"},
    {&CODECAPI_AVEncVideoHeaderSeconds, L"CODECAPI_AVEncVideoHeaderSeconds"},
    {&CODECAPI_AVEncVideoHeaderFrames, L"CODECAPI_AVEncVideoHeaderFrames"},
    {&CODECAPI_AVEncVideoDefaultUpperFieldDominant, L"CODECAPI_AVEncVideoDefaultUpperFieldDominant"},
    {&CODECAPI_AVEncVideoCBRMotionTradeoff, L"CODECAPI_AVEncVideoCBRMotionTradeoff"},
    {&CODECAPI_AVEncVideoCodedVideoAccessUnitSize, L"CODECAPI_AVEncVideoCodedVideoAccessUnitSize"},
    {&CODECAPI_AVEncVideoMaxKeyframeDistance, L"CODECAPI_AVEncVideoMaxKeyframeDistance"},
    {&CODECAPI_AVEncH264CABACEnable, L"CODECAPI_AVEncH264CABACEnable"},
    {&CODECAPI_AVEncVideoContentType, L"CODECAPI_AVEncVideoContentType"},
    {&CODECAPI_AVEncNumWorkerThreads, L"CODECAPI_AVEncNumWorkerThreads"},
    {&CODECAPI_AVEncVideoEncodeQP, L"CODECAPI_AVEncVideoEncodeQP"},
    {&CODECAPI_AVEncVideoMinQP, L"CODECAPI_AVEncVideoMinQP"},
    {&CODECAPI_AVEncVideoForceKeyFrame, L"CODECAPI_AVEncVideoForceKeyFrame"},
    {&CODECAPI_AVEncH264SPSID, L"CODECAPI_AVEncH264SPSID"},
    {&CODECAPI_AVEncH264PPSID, L"CODECAPI_AVEncH264PPSID"},
    {&CODECAPI_AVEncAdaptiveMode, L"CODECAPI_AVEncAdaptiveMode"},
    {&CODECAPI_AVEncVideoSelectLayer, L"CODECAPI_AVEncVideoSelectLayer"},
    {&CODECAPI_AVEncVideoTemporalLayerCount, L"CODECAPI_AVEncVideoTemporalLayerCount"},
    {&CODECAPI_AVEncVideoUsage, L"CODECAPI_AVEncVideoUsage"},
    {&CODECAPI_AVEncVideoRateControlParams, L"CODECAPI_AVEncVideoRateControlParams"},
    {&CODECAPI_AVEncVideoSupportedControls, L"CODECAPI_AVEncVideoSupportedControls"},
    {&CODECAPI_AVEncVideoEncodeFrameTypeQP, L"CODECAPI_AVEncVideoEncodeFrameTypeQP"},
    {&CODECAPI_AVEncSliceControlMode, L"CODECAPI_AVEncSliceControlMode"},
    {&CODECAPI_AVEncSliceControlSize, L"CODECAPI_AVEncSliceControlSize"},
    {&CODECAPI_AVEncVideoMaxNumRefFrame, L"CODECAPI_AVEncVideoMaxNumRefFrame"},
    {&CODECAPI_AVEncVideoMeanAbsoluteDifference, L"CODECAPI_AVEncVideoMeanAbsoluteDifference"},
    {&CODECAPI_AVEncVideoMaxQP, L"CODECAPI_AVEncVideoMaxQP"},
    {&CODECAPI_AVEncVideoLTRBufferControl, L"CODECAPI_AVEncVideoLTRBufferControl"},
    {&CODECAPI_AVEncVideoMarkLTRFrame, L"CODECAPI_AVEncVideoMarkLTRFrame"},
    {&CODECAPI_AVEncVideoUseLTRFrame, L"CODECAPI_AVEncVideoUseLTRFrame"},
    {&CODECAPI_AVEncVideoROIEnabled, L"CODECAPI_AVEncVideoROIEnabled"},
    {&CODECAPI_AVEncMuxOutputStreamType, L"CODECAPI_AVEncMuxOutputStreamType"},
    {&CODECAPI_AVEncStatVideoOutputFrameRate, L"CODECAPI_AVEncStatVideoOutputFrameRate"},
    {&CODECAPI_AVEncStatVideoCodedFrames, L"CODECAPI_AVEncStatVideoCodedFrames"},
    {&CODECAPI_AVEncStatVideoTotalFrames, L"CODECAPI_AVEncStatVideoTotalFrames"},
    {&CODECAPI_AVEncAudioIntervalToEncode, L"CODECAPI_AVEncAudioIntervalToEncode"},
    {&CODECAPI_AVEncAudioIntervalToSkip, L"CODECAPI_AVEncAudioIntervalToSkip"},
    {&CODECAPI_AVEncAudioDualMono, L"CODECAPI_AVEncAudioDualMono"},
    {&CODECAPI_AVEncAudioMeanBitRate, L"CODECAPI_AVEncAudioMeanBitRate"},
    {&CODECAPI_AVEncAudioMapDestChannel0, L"CODECAPI_AVEncAudioMapDestChannel0"},
    {&CODECAPI_AVEncAudioMapDestChannel1, L"CODECAPI_AVEncAudioMapDestChannel1"},
    {&CODECAPI_AVEncAudioMapDestChannel2, L"CODECAPI_AVEncAudioMapDestChannel2"},
    {&CODECAPI_AVEncAudioMapDestChannel3, L"CODECAPI_AVEncAudioMapDestChannel3"},
    {&CODECAPI_AVEncAudioMapDestChannel4, L"CODECAPI_AVEncAudioMapDestChannel4"},
    {&CODECAPI_AVEncAudioMapDestChannel5, L"CODECAPI_AVEncAudioMapDestChannel5"},
    {&CODECAPI_AVEncAudioMapDestChannel6, L"CODECAPI_AVEncAudioMapDestChannel6"},
    {&CODECAPI_AVEncAudioMapDestChannel7, L"CODECAPI_AVEncAudioMapDestChannel7"},
    {&CODECAPI_AVEncAudioMapDestChannel8, L"CODECAPI_AVEncAudioMapDestChannel8"},
    {&CODECAPI_AVEncAudioMapDestChannel9, L"CODECAPI_AVEncAudioMapDestChannel9"},
    {&CODECAPI_AVEncAudioMapDestChannel10, L"CODECAPI_AVEncAudioMapDestChannel10"},
    {&CODECAPI_AVEncAudioMapDestChannel11, L"CODECAPI_AVEncAudioMapDestChannel11"},
    {&CODECAPI_AVEncAudioMapDestChannel12, L"CODECAPI_AVEncAudioMapDestChannel12"},
    {&CODECAPI_AVEncAudioMapDestChannel13, L"CODECAPI_AVEncAudioMapDestChannel13"},
    {&CODECAPI_AVEncAudioMapDestChannel14, L"CODECAPI_AVEncAudioMapDestChannel14"},
    {&CODECAPI_AVEncAudioMapDestChannel15, L"CODECAPI_AVEncAudioMapDestChannel15"},
    {&CODECAPI_AVEncStatAudioPeakPCMValue, L"CODECAPI_AVEncStatAudioPeakPCMValue"},
    {&CODECAPI_AVEncStatAudioAveragePCMValue, L"CODECAPI_AVEncStatAudioAveragePCMValue"},
    {&CODECAPI_AVEncStatAudioAverageBPS, L"CODECAPI_AVEncStatAudioAverageBPS"},
    {&CODECAPI_AVEncStatAverageBPS, L"CODECAPI_AVEncStatAverageBPS"},
    {&CODECAPI_AVEncStatHardwareProcessorUtilitization, L"CODECAPI_AVEncStatHardwareProcessorUtilitization"},
    {&CODECAPI_AVEncStatBandwidthProcessorUtilitization, L"CODECAPI_AVEncStatBandwidthProcessorUtilitization"},
    {&CODECAPI_AVEncMPVGOPSize, L"CODECAPI_AVEncMPVGOPSize"},
    {&CODECAPI_AVEncMPVGOPOpen, L"CODECAPI_AVEncMPVGOPOpen"},
    {&CODECAPI_AVEncMPVDefaultBPictureCount, L"CODECAPI_AVEncMPVDefaultBPictureCount"},
    {&CODECAPI_AVEncMPVProfile, L"CODECAPI_AVEncMPVProfile"},
    {&CODECAPI_AVEncMPVLevel, L"CODECAPI_AVEncMPVLevel"},
    {&CODECAPI_AVEncMPVFrameFieldMode, L"CODECAPI_AVEncMPVFrameFieldMode"},
    {&CODECAPI_AVEncMPVAddSeqEndCode, L"CODECAPI_AVEncMPVAddSeqEndCode"},
    {&CODECAPI_AVEncMPVGOPSInSeq, L"CODECAPI_AVEncMPVGOPSInSeq"},
    {&CODECAPI_AVEncMPVUseConcealmentMotionVectors, L"CODECAPI_AVEncMPVUseConcealmentMotionVectors"},
    {&CODECAPI_AVEncMPVSceneDetection, L"CODECAPI_AVEncMPVSceneDetection"},
    {&CODECAPI_AVEncMPVGenerateHeaderSeqExt, L"CODECAPI_AVEncMPVGenerateHeaderSeqExt"},
    {&CODECAPI_AVEncMPVGenerateHeaderSeqDispExt, L"CODECAPI_AVEncMPVGenerateHeaderSeqDispExt"},
    {&CODECAPI_AVEncMPVGenerateHeaderPicExt, L"CODECAPI_AVEncMPVGenerateHeaderPicExt"},
    {&CODECAPI_AVEncMPVGenerateHeaderPicDispExt, L"CODECAPI_AVEncMPVGenerateHeaderPicDispExt"},
    {&CODECAPI_AVEncMPVGenerateHeaderSeqScaleExt, L"CODECAPI_AVEncMPVGenerateHeaderSeqScaleExt"},
    {&CODECAPI_AVEncMPVScanPattern, L"CODECAPI_AVEncMPVScanPattern"},
    {&CODECAPI_AVEncMPVIntraDCPrecision, L"CODECAPI_AVEncMPVIntraDCPrecision"},
    {&CODECAPI_AVEncMPVQScaleType, L"CODECAPI_AVEncMPVQScaleType"},
    {&CODECAPI_AVEncMPVIntraVLCTable, L"CODECAPI_AVEncMPVIntraVLCTable"},
    {&CODECAPI_AVEncMPVQuantMatrixIntra, L"CODECAPI_AVEncMPVQuantMatrixIntra"},
    {&CODECAPI_AVEncMPVQuantMatrixNonIntra, L"CODECAPI_AVEncMPVQuantMatrixNonIntra"},
    {&CODECAPI_AVEncMPVQuantMatrixChromaIntra, L"CODECAPI_AVEncMPVQuantMatrixChromaIntra"},
    {&CODECAPI_AVEncMPVQuantMatrixChromaNonIntra, L"CODECAPI_AVEncMPVQuantMatrixChromaNonIntra"},
    {&CODECAPI_AVEncMPALayer, L"CODECAPI_AVEncMPALayer"},
    {&CODECAPI_AVEncMPACodingMode, L"CODECAPI_AVEncMPACodingMode"},
    {&CODECAPI_AVEncDDService, L"CODECAPI_AVEncDDService"},
    {&CODECAPI_AVEncDDDialogNormalization, L"CODECAPI_AVEncDDDialogNormalization"},
    {&CODECAPI_AVEncDDCentreDownMixLevel, L"CODECAPI_AVEncDDCentreDownMixLevel"},
    {&CODECAPI_AVEncDDSurroundDownMixLevel, L"CODECAPI_AVEncDDSurroundDownMixLevel"},
    {&CODECAPI_AVEncDDProductionInfoExists, L"CODECAPI_AVEncDDProductionInfoExists"},
    {&CODECAPI_AVEncDDProductionRoomType, L"CODECAPI_AVEncDDProductionRoomType"},
    {&CODECAPI_AVEncDDProductionMixLevel, L"CODECAPI_AVEncDDProductionMixLevel"},
    {&CODECAPI_AVEncDDCopyright, L"CODECAPI_AVEncDDCopyright"},
    {&CODECAPI_AVEncDDOriginalBitstream, L"CODECAPI_AVEncDDOriginalBitstream"},
    {&CODECAPI_AVEncDDDigitalDeemphasis, L"CODECAPI_AVEncDDDigitalDeemphasis"},
    {&CODECAPI_AVEncDDDCHighPassFilter, L"CODECAPI_AVEncDDDCHighPassFilter"},
    {&CODECAPI_AVEncDDChannelBWLowPassFilter, L"CODECAPI_AVEncDDChannelBWLowPassFilter"},
    {&CODECAPI_AVEncDDLFELowPassFilter, L"CODECAPI_AVEncDDLFELowPassFilter"},
    {&CODECAPI_AVEncDDSurround90DegreeePhaseShift, L"CODECAPI_AVEncDDSurround90DegreeePhaseShift"},
    {&CODECAPI_AVEncDDSurround3dBAttenuation, L"CODECAPI_AVEncDDSurround3dBAttenuation"},
    {&CODECAPI_AVEncDDDynamicRangeCompressionControl, L"CODECAPI_AVEncDDDynamicRangeCompressionControl"},
    {&CODECAPI_AVEncDDRFPreEmphasisFilter, L"CODECAPI_AVEncDDRFPreEmphasisFilter"},
    {&CODECAPI_AVEncDDSurroundExMode, L"CODECAPI_AVEncDDSurroundExMode"},
    {&CODECAPI_AVEncDDPreferredStereoDownMixMode, L"CODECAPI_AVEncDDPreferredStereoDownMixMode"},
    {&CODECAPI_AVEncDDLtRtCenterMixLvl_x10, L"CODECAPI_AVEncDDLtRtCenterMixLvl_x10"},
    {&CODECAPI_AVEncDDLtRtSurroundMixLvl_x10, L"CODECAPI_AVEncDDLtRtSurroundMixLvl_x10"},
    {&CODECAPI_AVEncDDLoRoCenterMixLvl_x10, L"CODECAPI_AVEncDDLoRoCenterMixLvl_x10"},
    {&CODECAPI_AVEncDDLoRoSurroundMixLvl_x10, L"CODECAPI_AVEncDDLoRoSurroundMixLvl_x10"},
    {&CODECAPI_AVEncDDAtoDConverterType, L"CODECAPI_AVEncDDAtoDConverterType"},
    {&CODECAPI_AVEncDDHeadphoneMode, L"CODECAPI_AVEncDDHeadphoneMode"},
    {&CODECAPI_AVEncWMVKeyFrameDistance, L"CODECAPI_AVEncWMVKeyFrameDistance"},
    {&CODECAPI_AVEncWMVInterlacedEncoding, L"CODECAPI_AVEncWMVInterlacedEncoding"},
    {&CODECAPI_AVEncWMVDecoderComplexity, L"CODECAPI_AVEncWMVDecoderComplexity"},
    {&CODECAPI_AVEncWMVKeyFrameBufferLevelMarker, L"CODECAPI_AVEncWMVKeyFrameBufferLevelMarker"},
    {&CODECAPI_AVEncWMVProduceDummyFrames, L"CODECAPI_AVEncWMVProduceDummyFrames"},
    {&CODECAPI_AVEncStatWMVCBAvg, L"CODECAPI_AVEncStatWMVCBAvg"},
    {&CODECAPI_AVEncStatWMVCBMax, L"CODECAPI_AVEncStatWMVCBMax"},
    {&CODECAPI_AVEncStatWMVDecoderComplexityProfile, L"CODECAPI_AVEncStatWMVDecoderComplexityProfile"},
    {&CODECAPI_AVEncStatMPVSkippedEmptyFrames, L"CODECAPI_AVEncStatMPVSkippedEmptyFrames"},
    {&CODECAPI_AVEncMP12PktzSTDBuffer, L"CODECAPI_AVEncMP12PktzSTDBuffer"},
    {&CODECAPI_AVEncMP12PktzStreamID, L"CODECAPI_AVEncMP12PktzStreamID"},
    {&CODECAPI_AVEncMP12PktzInitialPTS, L"CODECAPI_AVEncMP12PktzInitialPTS"},
    {&CODECAPI_AVEncMP12PktzPacketSize, L"CODECAPI_AVEncMP12PktzPacketSize"},
    {&CODECAPI_AVEncMP12PktzCopyright, L"CODECAPI_AVEncMP12PktzCopyright"},
    {&CODECAPI_AVEncMP12PktzOriginal, L"CODECAPI_AVEncMP12PktzOriginal"},
    {&CODECAPI_AVEncMP12MuxPacketOverhead, L"CODECAPI_AVEncMP12MuxPacketOverhead"},
    {&CODECAPI_AVEncMP12MuxNumStreams, L"CODECAPI_AVEncMP12MuxNumStreams"},
    {&CODECAPI_AVEncMP12MuxEarliestPTS, L"CODECAPI_AVEncMP12MuxEarliestPTS"},
    {&CODECAPI_AVEncMP12MuxLargestPacketSize, L"CODECAPI_AVEncMP12MuxLargestPacketSize"},
    {&CODECAPI_AVEncMP12MuxInitialSCR, L"CODECAPI_AVEncMP12MuxInitialSCR"},
    {&CODECAPI_AVEncMP12MuxMuxRate, L"CODECAPI_AVEncMP12MuxMuxRate"},
    {&CODECAPI_AVEncMP12MuxPackSize, L"CODECAPI_AVEncMP12MuxPackSize"},
    {&CODECAPI_AVEncMP12MuxSysSTDBufferBound, L"CODECAPI_AVEncMP12MuxSysSTDBufferBound"},
    {&CODECAPI_AVEncMP12MuxSysRateBound, L"CODECAPI_AVEncMP12MuxSysRateBound"},
    {&CODECAPI_AVEncMP12MuxTargetPacketizer, L"CODECAPI_AVEncMP12MuxTargetPacketizer"},
    {&CODECAPI_AVEncMP12MuxSysFixed, L"CODECAPI_AVEncMP12MuxSysFixed"},
    {&CODECAPI_AVEncMP12MuxSysCSPS, L"CODECAPI_AVEncMP12MuxSysCSPS"},
    {&CODECAPI_AVEncMP12MuxSysVideoLock, L"CODECAPI_AVEncMP12MuxSysVideoLock"},
    {&CODECAPI_AVEncMP12MuxSysAudioLock, L"CODECAPI_AVEncMP12MuxSysAudioLock"},
    {&CODECAPI_AVEncMP12MuxDVDNavPacks, L"CODECAPI_AVEncMP12MuxDVDNavPacks"},
    {&CODECAPI_AVEncMPAOriginalBitstream, L"CODECAPI_AVEncMPAOriginalBitstream"},
    {&CODECAPI_AVEncMPAEnableRedundancyProtection, L"CODECAPI_AVEncMPAEnableRedundancyProtection"},
    {&CODECAPI_AVEncMPAPrivateUserBit, L"CODECAPI_AVEncMPAPrivateUserBit"},
    {&CODECAPI_AVEncMPAEmphasisType, L"CODECAPI_AVEncMPAEmphasisType"},
    {&CODECAPI_AVDecCommonOutputFormat, L"CODECAPI_AVDecCommonOutputFormat"},
    {&CODECAPI_AVDecCommonInputFormat, L"CODECAPI_AVDecCommonInputFormat"},
    {&CODECAPI_AVDecCommonMeanBitRate, L"CODECAPI_AVDecCommonMeanBitRate"},
    {&CODECAPI_AVDecCommonMeanBitRateInterval, L"CODECAPI_AVDecCommonMeanBitRateInterval"},
    {&CODECAPI_GUID_AVDecAudioOutputFormat_PCM_Stereo_MatrixEncoded, L"CODECAPI_GUID_AVDecAudioOutputFormat_PCM_Stereo_MatrixEncoded"},
    {&CODECAPI_GUID_AVDecAudioOutputFormat_PCM, L"CODECAPI_GUID_AVDecAudioOutputFormat_PCM"},
    {&CODECAPI_GUID_AVDecAudioOutputFormat_SPDIF_PCM, L"CODECAPI_GUID_AVDecAudioOutputFormat_SPDIF_PCM"},
    {&CODECAPI_GUID_AVDecAudioOutputFormat_SPDIF_Bitstream, L"CODECAPI_GUID_AVDecAudioOutputFormat_SPDIF_Bitstream"},
    {&CODECAPI_GUID_AVDecAudioOutputFormat_PCM_Headphones, L"CODECAPI_GUID_AVDecAudioOutputFormat_PCM_Headphones"},
    {&CODECAPI_GUID_AVDecAudioOutputFormat_PCM_Stereo_Auto, L"CODECAPI_GUID_AVDecAudioOutputFormat_PCM_Stereo_Auto"},
    {&CODECAPI_AVDecVideoImageSize, L"CODECAPI_AVDecVideoImageSize"},
    {&CODECAPI_AVDecVideoInputScanType, L"CODECAPI_AVDecVideoInputScanType"},
    {&CODECAPI_AVDecVideoPixelAspectRatio, L"CODECAPI_AVDecVideoPixelAspectRatio"},
    {&CODECAPI_AVDecVideoThumbnailGenerationMode, L"CODECAPI_AVDecVideoThumbnailGenerationMode"},
    {&CODECAPI_AVDecVideoDropPicWithMissingRef, L"CODECAPI_AVDecVideoDropPicWithMissingRef"},
    {&CODECAPI_AVDecVideoSoftwareDeinterlaceMode, L"CODECAPI_AVDecVideoSoftwareDeinterlaceMode"},
    {&CODECAPI_AVDecVideoFastDecodeMode, L"CODECAPI_AVDecVideoFastDecodeMode"},
    {&CODECAPI_AVLowLatencyMode, L"CODECAPI_AVLowLatencyMode"},
    {&CODECAPI_AVDecVideoH264ErrorConcealment, L"CODECAPI_AVDecVideoH264ErrorConcealment"},
    {&CODECAPI_AVDecVideoMPEG2ErrorConcealment, L"CODECAPI_AVDecVideoMPEG2ErrorConcealment"},
    {&CODECAPI_AVDecVideoCodecType, L"CODECAPI_AVDecVideoCodecType"},
    {&CODECAPI_AVDecVideoDXVAMode, L"CODECAPI_AVDecVideoDXVAMode"},
    {&CODECAPI_AVDecVideoDXVABusEncryption, L"CODECAPI_AVDecVideoDXVABusEncryption"},
    {&CODECAPI_AVDecVideoSWPowerLevel, L"CODECAPI_AVDecVideoSWPowerLevel"},
    {&CODECAPI_AVDecVideoMaxCodedWidth, L"CODECAPI_AVDecVideoMaxCodedWidth"},
    {&CODECAPI_AVDecVideoMaxCodedHeight, L"CODECAPI_AVDecVideoMaxCodedHeight"},
    {&CODECAPI_AVDecNumWorkerThreads, L"CODECAPI_AVDecNumWorkerThreads"},
    {&CODECAPI_AVDecSoftwareDynamicFormatChange, L"CODECAPI_AVDecSoftwareDynamicFormatChange"},
    {&CODECAPI_GUID_AVDecAudioInputWMA, L"CODECAPI_GUID_AVDecAudioInputWMA"},
    {&CODECAPI_GUID_AVDecAudioInputWMAPro, L"CODECAPI_GUID_AVDecAudioInputWMAPro"},
    {&CODECAPI_GUID_AVDecAudioInputDolby, L"CODECAPI_GUID_AVDecAudioInputDolby"},
    {&CODECAPI_GUID_AVDecAudioInputDTS, L"CODECAPI_GUID_AVDecAudioInputDTS"},
    {&CODECAPI_GUID_AVDecAudioInputPCM, L"CODECAPI_GUID_AVDecAudioInputPCM"},
    {&CODECAPI_GUID_AVDecAudioInputMPEG, L"CODECAPI_GUID_AVDecAudioInputMPEG"},
    {&CODECAPI_GUID_AVDecAudioInputAAC, L"CODECAPI_GUID_AVDecAudioInputAAC"},
    {&CODECAPI_GUID_AVDecAudioInputHEAAC, L"CODECAPI_GUID_AVDecAudioInputHEAAC"},
    {&CODECAPI_GUID_AVDecAudioInputDolbyDigitalPlus, L"CODECAPI_GUID_AVDecAudioInputDolbyDigitalPlus"},
    {&CODECAPI_AVDecAACDownmixMode, L"CODECAPI_AVDecAACDownmixMode"},
    {&CODECAPI_AVDecHEAACDynamicRangeControl, L"CODECAPI_AVDecHEAACDynamicRangeControl"},
    {&CODECAPI_AVDecAudioDualMonoReproMode, L"CODECAPI_AVDecAudioDualMonoReproMode"},
    {&CODECAPI_AVAudioChannelCount, L"CODECAPI_AVAudioChannelCount"},
    {&CODECAPI_AVAudioChannelConfig, L"CODECAPI_AVAudioChannelConfig"},
    {&CODECAPI_AVAudioSampleRate, L"CODECAPI_AVAudioSampleRate"},
    {&CODECAPI_AVDDSurroundMode, L"CODECAPI_AVDDSurroundMode"},
    {&CODECAPI_AVDecDDOperationalMode, L"CODECAPI_AVDecDDOperationalMode"},
    {&CODECAPI_AVDecDDMatrixDecodingMode, L"CODECAPI_AVDecDDMatrixDecodingMode"},
    {&CODECAPI_AVDecDDDynamicRangeScaleHigh, L"CODECAPI_AVDecDDDynamicRangeScaleHigh"},
    {&CODECAPI_AVDecDDDynamicRangeScaleLow, L"CODECAPI_AVDecDDDynamicRangeScaleLow"},
    {&CODECAPI_AVDecDDStereoDownMixMode, L"CODECAPI_AVDecDDStereoDownMixMode"},
    {&CODECAPI_AVDSPLoudnessEqualization, L"CODECAPI_AVDSPLoudnessEqualization"},
    {&CODECAPI_AVDSPSpeakerFill, L"CODECAPI_AVDSPSpeakerFill"},
    {&CODECAPI_AVPriorityControl, L"CODECAPI_AVPriorityControl"}
};

using namespace WEX::Common;
using namespace WEX::Logging;
using namespace WEX::TestExecution;
using namespace H264EncMFTW8ConfigurationClasses;

static IBasicLog*       g_pLogger       = NULL;
static volatile ULONG   g_ulLoggerCount = 0;

int LogToGlobalLogger(const TCHAR* ptcFormat, ...)
{
    static const DWORD  dwMaxMessageLen                 = 1024;
    static TCHAR        ptszMessage[dwMaxMessageLen]    = {0};

    if(g_pLogger != NULL)
    {
        va_list args;
        va_start(
            args,
            ptcFormat
            );

        StringCchVPrintfW(
            ptszMessage,
            dwMaxMessageLen,
            ptcFormat,
            args
            );

        va_end(args);

        g_pLogger->Log(
            XMSG,
            LogLevel_Info,
            ptszMessage
            );

        if(IsDebuggerPresent())
        {
            OutputDebugStringW(ptszMessage);
            OutputDebugStringW(L"\r\n");
        }
    }

    return 0;
}

HRESULT CreateH264EncodeVerify(
    const WCHAR*        pcwszInput,
    const WCHAR*        pcwszOutput,
    const WCHAR*        pcwszConfig,
    IBasicLog*          pLog,
    IH264EncodeVerify** ppH264EncodeVerify)
{
    HRESULT                     hr          = S_OK;
    ITAEFConfigCustomParser*    pH264Config = NULL;
    ITAEFConfigHelper*          pTAEFConfig = NULL;
    CH264EncodeVerify*          pVerify     = NULL;

    do
    {
        if( (pcwszInput == NULL)        ||
            (pcwszOutput == NULL)       ||
            (pcwszConfig == NULL)       ||
            (pLog == NULL)              ||
            (ppH264EncodeVerify == NULL))
        {
            hr = E_POINTER;
            break;
        }

        hr = CH264ConfigParser::CreateInstance(
            &pH264Config,
            pLog
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = CreateTAEFConfigHelperFromString(
            pLog,
            pH264Config,
            &pTAEFConfig,
            pcwszInput,
            pcwszOutput,
            pcwszConfig
            );
        if(FAILED(hr))
        {
            break;
        }

        pVerify = new CH264EncodeVerify(
            pTAEFConfig,
            pLog
            );
        if(pVerify == NULL)
        {
            hr = E_OUTOFMEMORY;
            break;
        }

        hr = pVerify->QueryInterface(
            __uuidof(IH264EncodeVerify),
            (void**)ppH264EncodeVerify
            );
    }while(false);

    SAFE_RELEASE(pH264Config);
    SAFE_RELEASE(pTAEFConfig);
    SAFE_RELEASE(pVerify);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::RunStreamingScenario()
{
    HRESULT hr = S_OK;
    BOOL bServerOS = FALSE;
    do
    {

#if defined(_ARM_) || defined(_ARM64_)
        if (m_pConfigHelper->IsReconTest())
        {
            Log::Comment(L"Compare test not supported on ARM");
            hr = S_OK;
            break;
        }
#endif 
        //for reconstructed frame dump compare scenarios; Just encode & compare the reconstructed frames
        if (m_pConfigHelper->IsReconTest())
        {
            _HR(SetSSERegKey());
            InternalLog(L"SSESupport %d", m_bIsSSESupported);
            if(m_bIsSSESupported)
            {
                _HR(H264BitStreamEncode());
                Log::Comment(L"Encoding Complete,comparing the reconstructed frame hash with the reference");

                //  _HR(CompareReconstructedFrame()); Using compare.exe directly as a task in WTT

                Log::Comment(L"Reconstructed frame hash comparison successfull, deleting the SSE reg keys");
                if(ERROR_SUCCESS != RegDeleteKeyEx(HKEY_CURRENT_USER,L"Software\\Microsoft\\Scrunch\\H264Enc",KEY_WOW64_32KEY,0))
                {
                    hr = S_OK;
                    Log::Comment(L"Unable to delete the reg key"); //if Reg Key cant be deleted, dont fail the test;
                }
            }
        }
        else
        {
            _HR(H264BitStreamEncode());
            if (MFTopologyHelper::Mode::EncodeVerify == m_pConfigHelper->GetMode())
            {
                _HR(H264BitStreamVerify());
                _HR(IsServerOS(&bServerOS));
                if (m_pConfigHelper->IsVideoQualityAnalysis() && 
                    !bServerOS)
                {
                    //_HR(VQAVerify());
                }
            }
        }        
        if(!m_pConfigHelper->IsKeepFilesSet()){
            if( m_bIsSWFileCreated){
                DeleteFile(m_strSWOutputFile);
            }
            DeleteFile(m_strHWOutputFile);

            if(m_bIsRawFileCreated){
                DeleteFile(m_pConfigHelper->GetOutputPath());
            }
        }

    } while (FALSE);

    return hr;
}

STDMETHODIMP
CH264EncodeVerify::RunDynamicTypeScenario(WCHAR* pTestPath)
{
    BOOL bVisVal = FALSE;
    HRESULT hr = S_OK;
    WEX::Common::String strWorkingPath = WEX::Common::String(L"");
    WEX::Common::String strFirstpassPath = WEX::Common::String(L"");
    WEX::Common::String strSecondpassPath = WEX::Common::String(L"");
    WEX::Common::String strTestSignature = WEX::Common::String(L"");

    do
    {
        _HR((m_pConfigHelper) ? S_OK : E_UNEXPECTED);
        _HR((m_pLog) ? S_OK : E_UNEXPECTED);

        _HR(m_pConfigHelper->GetConfigCustomParser((ITAEFConfigCustomParser **)&m_pH264CustomParser));
        _HR(m_pH264CustomParser->SetIsTopology(FALSE)); //I'm not sure if this is set anywhere else,
        //But I am setting it here to be sure
        UINT32 uiDynamicCount= 0;
        m_pH264CustomParser->GetDynamicCount(&uiDynamicCount);

        // Read input file path from the test xml
        WEX::Common::String strInputFile = m_pConfigHelper->GetInputPath();
        if (strInputFile.IsEmpty())
        {
            _HR(E_INVALIDARG);
        }

        if (pTestPath == NULL)
        {
            // Internal run, we have to create output path for each dynamic pass
            m_strOutputFile = m_pConfigHelper->GetOutputPath();
            if (m_strOutputFile.IsEmpty())
            {
                _HR(E_INVALIDARG);
            }

            int iLastSlash = m_strOutputFile.ReverseFind(L'\\');
            if (iLastSlash != -1)
            {
                strWorkingPath = m_strOutputFile.Left(iLastSlash + 1);
            }

            strTestSignature = m_pConfigHelper->GetTestSignature();

            strFirstpassPath.Append(strWorkingPath);
            strFirstpassPath.Append(strTestSignature);
            strFirstpassPath.Append(L"_");
            strSecondpassPath.Append(strWorkingPath);
            strSecondpassPath.Append(strTestSignature);
            strSecondpassPath.Append(L"_");
        }
        else
        {
            // HCK already created output folder for each test
            strWorkingPath = WEX::Common::String(pTestPath);
            strWorkingPath.Append(L"\\");

            strFirstpassPath.Append(strWorkingPath);
            strSecondpassPath.Append(strWorkingPath);
        }
        strFirstpassPath.Append(L"firstpass\\");
        strSecondpassPath.Append(L"secondpass\\");

        StringCchCopyW(m_pwcLocalInputFile, MAX_PATH, (LPCWSTR)strInputFile);

        CreateDirectoryW(strFirstpassPath,NULL);

        m_pSampleProcessor = new CDynamicDirectSampleProcessor(this, uiDynamicCount, strFirstpassPath);
        if (NULL == m_pSampleProcessor)
        {
            _HR(E_OUTOFMEMORY);
        }
        _HRL(((CDynamicDirectSampleProcessor*)m_pSampleProcessor)->Initialize(),
            InternalLog(L"Failed to initialize DynamicSampleProcessor\n"));

        hr = m_pSampleProcessor->ProcessSamples(0);
        if (E_SKIP == hr)
        {
            // Don't log error message for skipped tests - just causes confusion (the fact that the test is being skipped is already logged)
            break;
        }
        else
        {
            _HRL(hr, InternalLog(L"Failed first pass encode with hr = 0x%x", hr));
        }
        InternalLog(L"Completed first encode pass");
        SAFE_DELETE(m_pSampleProcessor);

#if 0   // Disable this validation for two reasons: 
        // 1) We are comparing SW encoder to HW encode and failing if PSNR difference is less than a threshold. This means we could 
        //    fail simply because the HW encode is much better than the SW encode
        // 2) There could be segment alignment issues that could cause failure - each dynamic format segment needs to be the same size between
        //     HW and SW encoded versions which may possibly not be the case.
        // If we want validation we need a better method.
        if(m_pConfigHelper->GetHWMode() == MFTopologyHelper::HWMode::HW){
            m_pConfigHelper->StrictlyForceSWMode();
            bVisVal = TRUE;
            CreateDirectoryW(strSecondpassPath,NULL);
            m_pSampleProcessor = new CDynamicDirectSampleProcessor(this, uiDynamicCount, strSecondpassPath);
            if (NULL == m_pSampleProcessor)
            {
                _HR(E_OUTOFMEMORY);
            }
            _HRL(((CDynamicDirectSampleProcessor*)m_pSampleProcessor)->Initialize(),
                InternalLog(L"Failed to initialize DynamicSampleProcessor\n"));
            _HRL(m_pSampleProcessor->ProcessSamples(0),InternalLog(L"Failed second pass encode with hr = 0x%x",hr));
            SAFE_DELETE(m_pSampleProcessor);
        }

        if(bVisVal)
        {
            InternalLog(L"Visually validating output\n");
            _HR(DynamicTypeVisVal(strWorkingPath));
        }
#endif

    } while (FALSE);

    SAFE_DELETE(m_pSampleProcessor);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::RunDynamicTemporalLayerScenario()
{
    HRESULT hr = S_OK;

    do
    {
        _HR((m_pConfigHelper) ? S_OK : E_UNEXPECTED);
        _HR((m_pLog) ? S_OK : E_UNEXPECTED);

        _HR(m_pConfigHelper->GetConfigCustomParser((ITAEFConfigCustomParser **)&m_pH264CustomParser));
        _HR(m_pH264CustomParser->SetIsTopology(FALSE)); //I'm not sure if this is set anywhere else,
        //But I am setting it here to be sure

        // Read input file path from the test xml
        WEX::Common::String strInputFile = m_pConfigHelper->GetInputPath();
        if (strInputFile.IsEmpty())
        {
            _HR(E_INVALIDARG);
        }

        StringCchCopyW(m_pwcLocalInputFile, MAX_PATH, (LPCWSTR)strInputFile);

        // Read output file format from the test xml (L"container")
        m_output = m_pConfigHelper->GetOutputFormat();

        // Read output file path from the test xml
        if(MFTopologyHelper::Renderer != m_output)
        {
            m_strOutputFile = m_pConfigHelper->GetOutputPath();

        }
        if (m_strOutputFile.IsEmpty())
        {
            _HR(E_INVALIDARG);
        }

        _HR(m_pConfigHelper->GetConfigCustomParser((ITAEFConfigCustomParser **)&m_pH264CustomParser));
        _HR(m_pH264CustomParser->SetIsTopology(FALSE)); //I'm not sure if this is set anywhere else,
        //But I am setting it here to be sure

        UINT32 uiDynamicCount= 0;
        m_pSampleProcessor = new CDynamicTemporalLayerDirectSampleProcessor(this, uiDynamicCount, NULL);
        if (NULL == m_pSampleProcessor)
        {
            _HR(E_OUTOFMEMORY);
        }
        _HR(m_pSampleProcessor->ProcessSamples(0));

    } while (FALSE);
    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::RunROIScenario()
{
    HRESULT hr = S_OK;

    do
    {
        _HR((m_pConfigHelper) ? S_OK : E_UNEXPECTED);
        _HR((m_pLog) ? S_OK : E_UNEXPECTED);

        _HR(m_pConfigHelper->GetConfigCustomParser((ITAEFConfigCustomParser **)&m_pH264CustomParser));
        _HR(m_pH264CustomParser->SetIsTopology(FALSE)); //I'm not sure if this is set anywhere else,
        //But I am setting it here to be sure

        // Read input file path from the test xml
        WEX::Common::String strInputFile = m_pConfigHelper->GetInputPath();
        if (strInputFile.IsEmpty())
        {
            _HR(E_INVALIDARG);
        }

        StringCchCopyW(m_pwcLocalInputFile, MAX_PATH, (LPCWSTR)strInputFile);

        // Read output file format from the test xml (L"container")
        m_output = m_pConfigHelper->GetOutputFormat();

        // Read output file path from the test xml
        if(MFTopologyHelper::Renderer != m_output)
        {
            m_strOutputFile = m_pConfigHelper->GetOutputPath();

            //For ROI VisVal, these are not actually hardware/software passes, but rather
            //ROI vs non ROI. We will use these variables because they are referenced
            //for PSNR calculation
            m_strHWOutputFile = WEX::Common::String(m_strOutputFile);
            m_strSWOutputFile = WEX::Common::String(m_strOutputFile);
            int index = m_strHWOutputFile.ReverseFind(L'.');
            m_strHWOutputFile.Insert(index,L"_p1");
            m_strSWOutputFile.Insert(index,L"_p2");

            m_strOutputFile = m_strHWOutputFile;
        }
        if (m_strOutputFile.IsEmpty())
        {
            _HR(E_INVALIDARG);
        }

        ///*
        //* Maybe want to make the ROI struct in the CustomParser something special - look into that
        //*/
        m_pH264CustomParser->EnableROI();

        m_pSampleProcessor = new CDirectSampleProcessor(this);
        if (NULL == m_pSampleProcessor)
        {
            _HR(E_OUTOFMEMORY);
        }

        _HRL(m_pSampleProcessor->ProcessSamples(0),InternalLog(L"Failed to encode in pass 1\n"));

        //This will return false if ROI isn't supported
        if (m_pH264CustomParser->IsROIModeSet() == FALSE){
            hr = S_OK;
            InternalLog(L"ROI Not supported, so skipping ROI test");
            break;
        }

        //Need to switch names, because SampleProcessor refers for m_strOutputFile
        m_strHWOutputFile = m_strOutputFile;
        m_strOutputFile = m_strSWOutputFile;

        m_pH264CustomParser->DisableROI();

        SAFE_DELETE(m_pSampleProcessor);
        m_pSampleProcessor = new CDirectSampleProcessor(this);
        if (NULL == m_pSampleProcessor)
        {
            _HR(E_OUTOFMEMORY);
        }
        _HRL(m_pSampleProcessor->ProcessSamples(0),InternalLog(L"Failed to encode in pass 2\n"));

        //Switching names back
        m_strSWOutputFile = m_strOutputFile;
        m_strOutputFile = m_strHWOutputFile;

        _HRL(H264CheckROIPSNR(), InternalLog(L"Failed to calculate ROI PSNR\n"));


        DeleteFileW((LPCWSTR)m_strHWOutputFile);
        DeleteFileW((LPCWSTR)m_strSWOutputFile);

    } while (FALSE);

    SAFE_DELETE(m_pSampleProcessor);
    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::DynamicTypeVisVal(LPCWSTR dirName)
{
    HRESULT hr = S_OK;

    IMFMediaType* pInType = NULL;
    IMFMediaType* pOutType = NULL;
    IMFSample* pSWSample = NULL;
    IMFSample* pHWSample = NULL;
    IMFSourceReader *pSWReader = NULL;
    IMFSourceReader *pHWReader = NULL;
    CDXCompare* dxCompare = NULL;
    DWORD dwWidth, dwHeight;
    DWORD dwSWLength, dwHWLength;
    DWORD dwStreamFlags = 0;
    WIN32_FIND_DATA FindFileData;
    BYTE* pSWBuf = NULL;
    BYTE* pHWBuf = NULL;
    IMFMediaBuffer *pSWMediaBuffer = NULL;
    IMFMediaBuffer *pHWMediaBuffer = NULL;
    double dYPSNR, dUPSNR, dVPSNR;
    HANDLE hFind;
    char* buffer;

    WEX::Common::String strFirstpassPath = WEX::Common::String(dirName);
    strFirstpassPath.Append(L"firstpass\\");
    WEX::Common::String strSecondpassPath = WEX::Common::String(dirName);
    strSecondpassPath.Append(L"secondpass\\");
    WEX::Common::String strFindPath = WEX::Common::String(dirName);
    strFindPath.Append(L"firstpass\\*");

    //This finds . and .., so the next FindNextFile will give us the first actual file
    hFind = FindFirstFile(strFindPath, &FindFileData);
    FindNextFile(hFind,&FindFileData);

    //Process each file
    while(FindNextFile(hFind,&FindFileData))
    {
        SAFE_DELETE(dxCompare); //Just in case
        dxCompare = new CDXCompare();
        dwStreamFlags = 0;
        WEX::Common::String strNoPathFilename = WEX::Common::String(FindFileData.cFileName);
        if(strNoPathFilename.Find(L"output") != 0){
            hr = E_FAIL;
            InternalLog(L"ERROR: Invalid file found in Visual Validation directory\n");
            break;
        }
        WEX::Common::String strSWFilename = WEX::Common::String(strFirstpassPath);
        WEX::Common::String strHWFilename = WEX::Common::String(strSecondpassPath);
        strSWFilename.Append(strNoPathFilename);
        strHWFilename.Append(strNoPathFilename);

        ////Open the file we found
        _HRL(MFCreateSourceReaderFromURL(strSWFilename,NULL,&pSWReader),InternalLog(L"Failed to open first file for visval with hr %x\n",hr));
        _HRL(MFCreateSourceReaderFromURL(strHWFilename,NULL,&pHWReader),InternalLog(L"Failed to open second file for visval with hr %x\n",hr));
        _HRL(pSWReader->GetNativeMediaType(0,0,&pInType),InternalLog(L"Failed to get native media type\n"));


        ////Create decoder and set input type
        _HRL(MFCreateMediaType(&pOutType),InternalLog(L"Failed to create new media type\n"));
        _HRL(pInType->CopyAllItems(pOutType),InternalLog(L"Failed to copy media type attributes over to new media type\n"));
        _HRL(pOutType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video),InternalLog(L"Failed to set output type to video\n"));
        _HRL(pOutType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_NV12),InternalLog(L"Failed to set output type to NV12\n"));
        _HRL(pSWReader->SetCurrentMediaType(0,NULL,pOutType),InternalLog(L"Failed to set output type on SW source reader\n"));
        _HRL(pHWReader->SetCurrentMediaType(0,NULL,pOutType),InternalLog(L"Failed to set output type on HW source reader\n"));
        _HRL(MFGetAttributeSize(pOutType,MF_MT_FRAME_SIZE,(UINT32*)&dwWidth,(UINT32*)&dwHeight),InternalLog(L"Failed to get resolution from media type\n"));

        ////Read IMFMediaType for info for DXCompare
        _HRL(dxCompare->EnableSingleFrame(dwWidth,dwWidth,dwHeight,MFVideoFormat_NV12,TRUE,FALSE),InternalLog(L"Failed to set DXCompare to Single frame mode"));
        while(!(dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM))
        {
            _HRL(pSWReader->ReadSample(0,0,NULL,&dwStreamFlags,NULL,&pSWSample),InternalLog(L"Failed to read decoded sample from SW file\n"));
            _HRL(pHWReader->ReadSample(0,0,NULL,&dwStreamFlags,NULL,&pHWSample),InternalLog(L"Failed to read decoded sample from HW file\n"));

            if(pSWSample != NULL && pHWSample != NULL)
            {
                //Lock and send buffers to DXcompare
                _HRL(pSWSample->GetBufferByIndex(0,&pSWMediaBuffer),InternalLog(L"Failed to get buffer from SW sample\n"));
                _HRL(pHWSample->GetBufferByIndex(0,&pHWMediaBuffer),InternalLog(L"Failed to get buffer from HW sample\n"));
                _HRL(pSWMediaBuffer->Lock(&pSWBuf,NULL,&dwSWLength),InternalLog(L"Failed to unlock SW buffer\n"));
                _HRL(pHWMediaBuffer->Lock(&pHWBuf,NULL,&dwHWLength),InternalLog(L"Failed to unlock HW buffer\n"));
                if(dwSWLength != dwHWLength)
                {
                    InternalLog(L"ERROR: Sample sizes do not agree: SW: %d HW: %d\n",dwSWLength, dwHWLength);
                    hr = E_FAIL;
                }else{
                    dxCompare->ProcessSingleFrame(pSWBuf, pHWBuf,&dYPSNR,&dUPSNR,&dVPSNR);
                }
                pHWMediaBuffer->Unlock();
                pSWMediaBuffer->Unlock();
            }
            SAFE_RELEASE(pSWMediaBuffer);
            SAFE_RELEASE(pHWMediaBuffer);
            SAFE_RELEASE(pSWSample);
            SAFE_RELEASE(pHWSample);
        }
        _HR(hr);

        //Check that PSNRs are good
        dxCompare->GetSingleFrameAccumulatedPSNR(&dYPSNR,&dUPSNR,&dVPSNR);
        if(dYPSNR < DYNAMIC_VISVAL_PSNR_BAR)
        {
            InternalLog(L"VisVal quality check fail on file: %s with PSNR %f\n",FindFileData.cFileName,dYPSNR);
            hr = E_FAIL;
            break;
        }else{
            //InternalLog(L"Validated file: %s with PSNR %f",FindFileData.cFileName,dYPSNR);
        }

        SAFE_RELEASE(pInType);
        SAFE_RELEASE(pOutType);
        SAFE_DELETE(dxCompare);
        SAFE_RELEASE(pSWReader);
        SAFE_RELEASE(pHWReader);
    }
    if(SUCCEEDED(hr)){
        //Delete everything here
        hFind = FindFirstFile(strFindPath, &FindFileData);
        FindNextFile(hFind,&FindFileData);

        //Process each file
        while(FindNextFile(hFind,&FindFileData))
        {
            WEX::Common::String strNoPathFilename = WEX::Common::String(FindFileData.cFileName);
            WEX::Common::String strSWFilename = WEX::Common::String(strFirstpassPath);
            WEX::Common::String strHWFilename = WEX::Common::String(strSecondpassPath);
            strSWFilename.Append(strNoPathFilename);
            strHWFilename.Append(strNoPathFilename);
            DeleteFileW(strSWFilename);
            DeleteFileW(strHWFilename);
        }
        //RemoveDirectoryW(strFirstpassPath);
        //RemoveDirectoryW(strSecondpassPath);
    }

    SAFE_RELEASE(pInType);
    SAFE_RELEASE(pOutType);
    SAFE_DELETE(dxCompare);
    SAFE_RELEASE(pSWSample);
    SAFE_RELEASE(pHWSample);
    SAFE_RELEASE(pSWReader);
    SAFE_RELEASE(pHWReader);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::GetLTRRDPoint(int run, double* pLTRPSNR, double* pPSNR, UINT32 *pLTRBitrate, UINT32 *pBitrate){
        HRESULT hr = S_OK;

        do
        {
            _HR((m_pConfigHelper) ? S_OK : E_UNEXPECTED);

            // Read input file path from the test xml
            WEX::Common::String strInputFile = m_pConfigHelper->GetInputPath();
            if (strInputFile.IsEmpty())
            {
                _HR(E_INVALIDARG);
            }

            StringCchCopyW(m_pwcLocalInputFile, MAX_PATH, (LPCWSTR)strInputFile);

            // Read output file format from the test xml (L"container")
            m_output = m_pConfigHelper->GetOutputFormat();

            // Read output file path from the test xml
            if(MFTopologyHelper::Renderer != m_output)
            {
                m_strOutputFile = m_pConfigHelper->GetOutputPath();

                //For LTR VisVal, these are not actually hardware/software passes, but rather
                //LTR vs non LTR. We will use these variables because they are referenced
                //for PSNR calculation
                m_strHWOutputFile = WEX::Common::String(m_strOutputFile);
                m_strSWOutputFile = WEX::Common::String(m_strOutputFile);
                int index = m_strHWOutputFile.ReverseFind(L'.');
                m_strHWOutputFile.Insert(index,L"_p1");
                m_strSWOutputFile.Insert(index,L"_p2");

                m_strOutputFile = m_strHWOutputFile;
            }
            if (m_strOutputFile.IsEmpty())
            {
                _HR(E_INVALIDARG);
            }

            /*
            * Maybe want to make the LTR struct in the CustomParser something special - look into that
            */
            m_pH264CustomParser->EnableLTR();

            m_pSampleProcessor = new CDirectSampleProcessor(this);
            if (NULL == m_pSampleProcessor)
            {
                _HR(E_OUTOFMEMORY);
            }
            _HRL(m_pSampleProcessor->ProcessSamples(run),InternalLog(L"Failed to encode in pass 1\n"));

            //Need to switch names, because SampleProcessor refers for m_strOutputFile
            m_strHWOutputFile = m_strOutputFile;
            m_strOutputFile = m_strSWOutputFile;

            m_pH264CustomParser->DisableLTR();

            SAFE_DELETE(m_pSampleProcessor);
            m_pSampleProcessor = new CDirectSampleProcessor(this);
            if (NULL == m_pSampleProcessor)
            {
                _HR(E_OUTOFMEMORY);
            }
            _HRL(m_pSampleProcessor->ProcessSamples(run),InternalLog(L"Failed to encode in pass 2\n"));

            //Switching names back
            m_strSWOutputFile = m_strOutputFile;
            m_strOutputFile = m_strHWOutputFile;

            _HRL(H264CheckPSNR(pLTRPSNR,pPSNR,pLTRBitrate,pBitrate,FALSE),InternalLog(L"Failed to calculate LTR PSNR\n"));

            DeleteFileW((LPCWSTR)m_strHWOutputFile);
            DeleteFileW((LPCWSTR)m_strSWOutputFile);

        } while (FALSE);

        SAFE_DELETE(m_pSampleProcessor);
        SAFE_DELETE(m_pH264EncVerifier);

        return hr;
}

STDMETHODIMP
    CH264EncodeVerify::RunLTRNetLossScenario()
{
    HRESULT hr = S_OK;
    BOOL bServerOS = FALSE;
    do
    {
         _HR(H264BitStreamEncodeWithNetLoss());       
        if(!m_pConfigHelper->IsKeepFilesSet())
        {
            DeleteFile(m_pwcLTRWithLossOutputFile);    
            DeleteFile(m_pwcLTRWithoutLossOutputFile);
        }
    
    } while (FALSE);

    return hr;
}
STDMETHODIMP
    CH264EncodeVerify::RunLTRRDCurveScenario()
{
    HRESULT hr = S_OK;
    IRateDistortionCurve *pLTRRDCurve = NULL;
    IRateDistortionCurve *pNormalRDCurve = NULL;
    UINT32 index = 0;
    UINT32 uiVal = 0;
    BOOL fRDQuality = TRUE;
    UINT32 dwLTRBitrateMin = UINT_MAX;
    UINT32 dwLTRBitrateMax = 0;
    UINT32 dwNormalBitrateMin = UINT_MAX;
    UINT32 dwNormalBitrateMax = 0;
    UINT32 dwBitrateMin = UINT_MAX;
    UINT32 dwBitrateMax = 0;
    UINT32 dwLTRBitrate = 0L;
    UINT32 dwNormalBitrate = 0L;
    double dLTRPSNR = 0.0;
    double dNormalPSNR = 0.0;
    double dLTRRDArea = 0.0;
    double dNormalRDArea = 0.0;
    double dRCCurveDelta = 0.0;
    BOOL bOnTheFlyNormal = FALSE;

    do
    {
        _HR((m_pConfigHelper) ? S_OK : E_UNEXPECTED);
        _HR((m_pLog) ? S_OK : E_UNEXPECTED);

        _HR(m_pConfigHelper->GetConfigCustomParser((ITAEFConfigCustomParser **)&m_pH264CustomParser));

        _HRL(CreateRateDistortionCurve(m_pLog, &pLTRRDCurve), 
            InternalLog(L"Failed to create LTR IRateDistortionCurve object, hr = 0x%08X", hr));
        _HRL(CreateRateDistortionCurve(m_pLog, &pNormalRDCurve), 
            InternalLog(L"Failed to create Normal IRateDistortionCurve object, hr = 0x%08X", hr));

        do
        {
            hr = m_pConfigHelper->GetRDVariationByQuality(index, &uiVal);
            if (MF_E_NOT_FOUND == hr)
            {
                hr = m_pConfigHelper->GetRDVariationByBitrate(index, &uiVal);
                if (MF_E_NOT_FOUND == hr)
                {
                    InternalLog(L"Failed to find RD curve configuration parameters");
                    break;
                }
                else
                {
                    fRDQuality = FALSE;
                }
            }
            if (MF_E_INVALIDINDEX == hr)
            {
                hr = S_OK;
                break;
            }
            if (FAILED(hr))
            {
                InternalLog(L"Failed to Get RD Variation");
                break;
            }

            InternalLog(L"Running encoding scenario for %s = %lu", (fRDQuality ? L"quality" : L"bitrate"), uiVal);

            _HR(GetLTRRDPoint(index, &dLTRPSNR, &dNormalPSNR, &dwLTRBitrate,&dwNormalBitrate));

            InternalLog(L"Quality %d: %lu - %f, %lu - %f",uiVal, dwLTRBitrate, dLTRPSNR, dwNormalBitrate, dNormalPSNR);

            index++;

            dwLTRBitrateMax = max(dwLTRBitrateMax, dwLTRBitrate);
            dwLTRBitrateMin = min(dwLTRBitrateMin, dwLTRBitrate);

            dwNormalBitrateMax = max(dwNormalBitrateMax, dwNormalBitrate);
            dwNormalBitrateMin = min(dwNormalBitrateMin, dwNormalBitrate);

            _HR( pLTRRDCurve->AddRatePsnrPair((double)dwLTRBitrate, dLTRPSNR));
            _HR( pNormalRDCurve->AddRatePsnrPair((double)dwNormalBitrate, dNormalPSNR));
        } while (TRUE);

        if (FAILED(hr))
        {
            break;
        }

        dwBitrateMin = max(dwLTRBitrateMin, dwNormalBitrateMin); //We want the area in between the two sets
        dwBitrateMax = min(dwLTRBitrateMax, dwNormalBitrateMax);

        //Holding up a second, what if they two sets overlap only a little, or do not overlap at all?
        //Let's get the range of the two sets first:
        double dRangeNorm = dwNormalBitrateMax - dwNormalBitrateMin;
        double dRangeLTR = dwLTRBitrateMax - dwLTRBitrateMin;

        //The thershhold for overlad should be 20% of the average of the two range lengths
        double dOverlapThreshold = (dRangeNorm + dRangeLTR)/10;

        if(dwBitrateMax - dwBitrateMin < dOverlapThreshold)
        {
            InternalLog(L"Warning: LTR and non-LTR bitrates do no overlap for average PSNR-based Rate Distortion level comparison");
            InternalLog(L"Warning: Using entire bitrate range for comparison");
            InternalLog(L"Warning: LTR and non-LTR bitrates do no overlap for average PSNR-based Rate Distortion level comparison");
            InternalLog(L"Warning: Using entire bitrate range for comparison");
            dwBitrateMin = min(dwNormalBitrateMin, dwLTRBitrateMin);
            dwBitrateMax = max(dwNormalBitrateMax, dwLTRBitrateMax);
        }

        _HR( pLTRRDCurve->IntegrateOverRange((double)dwBitrateMin, (double)dwBitrateMax, &dLTRRDArea));
        _HR( pNormalRDCurve->IntegrateOverRange((double)dwBitrateMin, (double)dwBitrateMax, &dNormalRDArea));
        InternalLog(L"Average LTR RD level is %f dB", dLTRRDArea);
        InternalLog(L"Average Normal RD level is %f dB", dNormalRDArea);


        dRCCurveDelta = dNormalRDArea - dLTRRDArea;
        if(dRCCurveDelta > 1.5)
        {
            InternalLog(L"Quality verification FAILED because average PSNR-based Rate-Distortion level of LTR encoder is more that 1.5Db worse than one of normal encoder");
            hr = E_FAIL;
            break;
        }
        else
        {
            if(dRCCurveDelta > 0.0)
            {
                InternalLog(L"Quality verification PASSED because average PSNR-based Rate-Distortion level of LTR encoder is less than 1.5Db worse than one of normal encoder");
            }
            else
            {
                InternalLog(L"Quality verification PASSED because average PSNR-based Rate-Distortion level of LTR encoder is better than normal encoder");
            }
        }

    } while (FALSE);

    SAFE_RELEASE(pLTRRDCurve);
    SAFE_RELEASE(pNormalRDCurve);

    return hr;

}


STDMETHODIMP
    CH264EncodeVerify::RunRDCurveScenario()
{
    HRESULT hr = S_OK;
    IRateDistortionCurve *pHWRDCurve = NULL;
    IRateDistortionCurve *pSWRDCurve = NULL;
    UINT32 index = 0;
    UINT32 uiVal = 0;
    BOOL fRDQuality = TRUE;
    DWORD dwHWBitrateMin = UINT_MAX;
    DWORD dwHWBitrateMax = 0;
    DWORD dwSWBitrateMin = UINT_MAX;
    DWORD dwSWBitrateMax = 0;
    DWORD dwBitrateMin = UINT_MAX;
    DWORD dwBitrateMax = 0;
    DWORD dwHWBitrate = 0L;
    DWORD dwSWBitrate = 0L;
    double dHWPSNR = 0.0;
    double dSWPSNR = 0.0;
    double dHWRDArea = 0.0;
    double dSWRDArea = 0.0;
    double dRCCurveDelta = 0.0;
    BOOL bOnTheFlySW = FALSE;
    BOOL bCABAC = FALSE;

    do
    {
        _HR((m_pConfigHelper) ? S_OK : E_UNEXPECTED);
        _HR((m_pLog) ? S_OK : E_UNEXPECTED);

        _HR(m_pConfigHelper->GetConfigCustomParser((ITAEFConfigCustomParser **)&m_pH264CustomParser));

        _HRL(CreateRateDistortionCurve(m_pLog, &pHWRDCurve), 
            InternalLog(L"Failed to create HW IRateDistortionCurve object, hr = 0x%08X", hr));
        _HRL(CreateRateDistortionCurve(m_pLog, &pSWRDCurve), 
            InternalLog(L"Failed to create SW IRateDistortionCurve object, hr = 0x%08X", hr));

        do
        {
            m_pH264CustomParser->GetCABACEnable(&bCABAC);

            hr = m_pConfigHelper->GetRDVariationByQuality(index, &uiVal);
            if (MF_E_NOT_FOUND == hr)
            {
                hr = m_pConfigHelper->GetRDVariationByBitrate(index, &uiVal);
                if (MF_E_NOT_FOUND == hr)
                {
                    break;
                }
                else
                {
                    fRDQuality = FALSE;
                }
            }
            if (MF_E_INVALIDINDEX == hr)
            {
                hr = S_OK;
                break;
            }
            if (FAILED(hr))
            {
                break;
            }

            hr = m_pConfigHelper->GetReferenceRDCurveParameterAt(index, bCABAC, &dwSWBitrate, &dSWPSNR);
            if(MF_E_INVALIDINDEX == hr)
            {
                hr = S_OK;
                break;
            }
            else if(MF_E_NOT_FOUND == hr)
            {
                bOnTheFlySW = TRUE;
            }
            else if(FAILED(hr))
            {
                InternalLog(L"Failed in IMFTaefConfigHelper::GetReferenceRDCurveParameterAt, hr = 0x%08X", hr);
                break;
            }
            else
            {
                InternalLog(L"Successfully loaded Reference RD Curve parameter: %d, %f",dwSWBitrate,dSWPSNR);
            }

            InternalLog(L"Running encoding scenario for %s = %lu", (fRDQuality ? L"quality" : L"bitrate"), uiVal);

            if(bOnTheFlySW)
            {
                _HR(GetPSNR(&dwHWBitrate, &dHWPSNR, &dwSWBitrate, &dSWPSNR, index));
            }else{
                _HR(GetPSNR(&dwHWBitrate, &dHWPSNR, NULL, NULL, index));
            }


            index++;

            dwHWBitrateMax = max(dwHWBitrateMax, dwHWBitrate);
            dwHWBitrateMin = min(dwHWBitrateMin, dwHWBitrate);

            dwSWBitrateMax = max(dwSWBitrateMax, dwSWBitrate);
            dwSWBitrateMin = min(dwSWBitrateMin, dwSWBitrate);

            _HR( pHWRDCurve->AddRatePsnrPair((double)dwHWBitrate, dHWPSNR));
            _HR( pSWRDCurve->AddRatePsnrPair((double)dwSWBitrate, dSWPSNR));



        } while (TRUE);

        if (FAILED(hr))
        {
            break;
        }

        dwBitrateMin = max(dwHWBitrateMin, dwSWBitrateMin); //We want the area in between the two sets
        dwBitrateMax = min(dwHWBitrateMax, dwSWBitrateMax);

        //Holding up a second, what if they two sets overlap only a little, or do not overlap at all?
        //Let's get the range of the two sets first:
        double dRangeInbox = dwSWBitrateMax - dwSWBitrateMin;
        double dRangeHardware = dwHWBitrateMax - dwHWBitrateMin;

        //The thershhold for overlad should be 20% of the average of the two range lengths
        double dOverlapThreshold = (dRangeInbox + dRangeHardware)/10;

        if(dwBitrateMax - dwBitrateMin < dOverlapThreshold)
        {
            InternalLog(L"Warning: Inbox and Hardware Encoders bitrates do no overlap for average PSNR-based Rate Distortion level comparison");
            InternalLog(L"Warning: Using entire bitrate range for comparison");
            InternalLog(L"Warning: Inbox and Hardware Encoders bitrates do no overlap for average PSNR-based Rate Distortion level comparison");
            InternalLog(L"Warning: Using entire bitrate range for comparison");
            dwBitrateMin = min(dwSWBitrateMin, dwHWBitrateMin);
            dwBitrateMax = max(dwSWBitrateMax, dwHWBitrateMax);
        }

        _HR( pHWRDCurve->IntegrateOverRange((double)dwBitrateMin, (double)dwBitrateMax, &dHWRDArea));
        _HR( pSWRDCurve->IntegrateOverRange((double)dwBitrateMin, (double)dwBitrateMax, &dSWRDArea));
        InternalLog(L"Average HW RD level is %f dB", dHWRDArea);
        InternalLog(L"Average SW RD level is %f dB", dSWRDArea);


        dRCCurveDelta = dSWRDArea - dHWRDArea;
        if(dRCCurveDelta > 1.0)
        {
            InternalLog(L"Quality verification FAILED because average PSNR-based Rate-Distortion level of HW encoder is more that 1Db worse than one of inbox encoder");
            hr = E_FAIL;
            break;
        }
        else
        {
            if(dRCCurveDelta > 0.0)
            {
                InternalLog(L"Quality verification PASSED because average PSNR-based Rate-Distortion level of HW encoder is less than 1Db worse than one of inbox encoder");
            }
            else
            {
                InternalLog(L"Quality verification PASSED because average PSNR-based Rate-Distortion level of HW encoder is better than one of inbox encoder");
            }
        }

    } while (FALSE);

    SAFE_RELEASE(pHWRDCurve);
    SAFE_RELEASE(pSWRDCurve);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::RunSinkWriterScenario(
    const BOOL  bSaveOutput)
{
    HRESULT                 hr                  = S_OK;
    CSyncWriterEncodeTest*  pSinkWriterTest     = NULL;
    IMFAttributes*          pEncodingAttributes = NULL;
    IMFSample*              pSample             = NULL;
    FILE*                   fpOutput            = NULL;
    FILE*                   fpHash              = NULL;
    TCHAR                   hashFileName[MAX_PATH];

    do
    {
        if(_wfopen_s(
            &fpOutput,
            (const WCHAR*)m_pConfigHelper->GetOutputPath(),
            L"wb"
            ) != 0)
        {
            _HRL(E_FAIL, Log::Comment(L"Failed to output output file"));
        }

        if(m_pConfigHelper->IsHashOutputBuffersSet() == TRUE)
        {
            //Set Hash File name from the output file name provided in XML
            memset(hashFileName, 0, sizeof(TCHAR)* MAX_PATH);
            WEX::Common::String outputFilePath = m_pConfigHelper->GetOutputPath();

            TCHAR* outPutFileName = NULL;
            outPutFileName = _tcsrchr(outputFilePath, _T('\\'));

            if(outPutFileName != NULL)
            {
                outPutFileName++;
            }

            hr = _tcscat_s(hashFileName, MAX_PATH, outPutFileName);

            if(FAILED(hr))
            {
                _HRL(E_FAIL, Log::Comment(L"Failed in copy hash file name!"));
                break;
            }

            hr = _tcscat_s(hashFileName, MAX_PATH, _T(".hash"));

            if(FAILED(hr))
            {
                _HRL(E_FAIL, Log::Comment(L"Failed in appending .hash to file name!"));
                break;
            }

            if(_wfopen_s(&fpHash, (const WCHAR*)hashFileName, L"wb") != 0)
            {
                _HRL(E_FAIL, Log::Comment(L"Failed to open Hash file"));
            }

        }

        _HR(SetSSERegKey());

        if(m_bIsSSESupported == FALSE)
        {
            Log::Comment(L"SSE type not supported on the machine; Skipping the rest of the test");
            hr = S_OK;
            break;
        }


        _HR(m_pConfigHelper->GetConfigCustomParser((ITAEFConfigCustomParser **)&m_pH264CustomParser));

        _HR(MFCreateAttributes(
            &pEncodingAttributes,
            1
            ));

        // Convert the config params into encoding params
        hr = SetupSinkWriterEncodingAttributes(pEncodingAttributes);
        if(FAILED(hr))
        {
            break;
        }    

        hr = pEncodingAttributes->SetGUID(
            MF_MT_SUBTYPE,
            MFVideoFormat_H264
            );


        _HRL(CSyncWriterEncodeTest::CreateInstance(
            (const WCHAR*)m_pConfigHelper->GetInputPath(),
            FALSE,
            pEncodingAttributes,
            &pSinkWriterTest
            ), Log::Comment(L"Failed to create SyncWriter Test"));

        _HRL(pSinkWriterTest->StartEncoding(), Log::Comment(L"Failed to start SyncWriter Encoding"));

        while(true)
        {
            SAFE_RELEASE(pSample);

            _HRL(pSinkWriterTest->GetEncodedSample(
                &pSample
                ), Log::Comment(L"Failed to get sample from SyncWriter Test"));

            if(hr != S_OK)
            {
                break;
            }

            if(bSaveOutput != FALSE)
            {
                _HRL(WriteSampleToFile(
                    pSample,
                    fpOutput,
                    fpHash
                    ), Log::Comment(L"Failed to write sample to file"));
            }
        }

        _HRL(hr, Log::Comment(L"Sink Writer test failed"));
        if(FAILED(hr))
            break;

        Log::Comment(L"Sink Writer test passed");
        m_bIsRawFileCreated = TRUE;  //used later to decide if file needs to be deleted or not -keepfiles config param

#if defined(_ARM_) || defined(_ARM64_)
        if(m_pConfigHelper->IsCompareTest() && !FAILED(hr))
        {
            Log::Comment(L"Compare test not supported on ARM");
            break;
        }
#endif 

        if(m_pConfigHelper->IsCompareTest() && !FAILED(hr))
        {
            if(fpHash)
            {			
                fclose(fpHash);
            } 
            hr = GetReferenceFileAndCompare(hashFileName);
            if(hr == E_SKIP)
            {
                hr = S_OK;
                Log::Comment(L"SSE type not supported, Skipping the test");
            }
            if(FAILED(hr))
            {
                Log::Comment(L"Failed to execute compare.exe");
            }
        }


    }while(false);

    SAFE_DELETE(pSinkWriterTest);
    SAFE_RELEASE(pEncodingAttributes);
    SAFE_RELEASE(pSample);
    SAFE_CLOSE(fpOutput);
    SAFE_CLOSE(fpHash);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::RunSinkWriterDynamicScenario(
    const BOOL  bSaveOutput)
{
    HRESULT                 hr                  = S_OK;

    do
    {
        _HRL(m_pConfigHelper->GetConfigCustomParser((ITAEFConfigCustomParser **)&m_pH264CustomParser),
            InternalLog(L"Failed to get custom parser"));

        // Read input file path from the test xml
        WEX::Common::String strInputFile = m_pConfigHelper->GetInputPath();
        if (strInputFile.IsEmpty())
        {
            _HR(E_INVALIDARG);
        }

        StringCchCopyW(m_pwcLocalInputFile, MAX_PATH, (LPCWSTR)strInputFile);

        // Read output file path from the test xml
        m_strOutputFile = m_pConfigHelper->GetOutputPath();

        m_strHWOutputFile = WEX::Common::String(m_strOutputFile);
        m_strSWOutputFile = WEX::Common::String(m_strOutputFile);
        int index = m_strHWOutputFile.ReverseFind(L'.');
        m_strHWOutputFile.Insert(index,L"_p1");
        m_strSWOutputFile.Insert(index,L"_p2");

        m_strOutputFile = m_strHWOutputFile;

        if (m_strOutputFile.IsEmpty())
        {
            _HR(E_INVALIDARG);
        }

        //Setup LTR structure for verification
        UINT32 uiMaxLTR, uiTrustMode;
        if(S_OK == m_pH264CustomParser->GetLTRInfo(&uiMaxLTR,&uiTrustMode)){
            SAFE_DELETE(m_pLTRFrameInfo);
            m_pLTRFrameInfo = new stLTRFrameInfo;
            if(m_pLTRFrameInfo == NULL){
                _HR(E_OUTOFMEMORY);
            }
            memset(m_pLTRFrameInfo,0,sizeof(stLTRFrameInfo));
            m_pLTRFrameInfo->uiMaxLTRs = uiMaxLTR;
            m_pLTRFrameInfo->bLTRUseConstraint = TRUE;
        }

        m_pSampleProcessor = new CSinkWriterSampleProcessor(this);
        if(m_pSampleProcessor == NULL){
            _HRL(E_OUTOFMEMORY,InternalLog(L"Failed to create SinkWriterSampleProcessor\n"));
        }

        hr = m_pSampleProcessor->ProcessSamples(1);
        if (E_SKIP == hr)
        {
            // Don't log error message for skipped tests - just causes confusion (the fact that the test is being skipped is already logged)
            break;
        }
        else
        {
            _HRL(hr, InternalLog(L"Failed to encode in pass 1 with hr=%x\n", hr));
        }

        if (MFTopologyHelper::Mode::EncodeVerify == m_pConfigHelper->GetMode())
        {
            _HRL(H264BitStreamVerify(),InternalLog(L"Failed bitstream verification with hr=%x\n",hr));
            if(SUCCEEDED(hr)){
                InternalLog(L"Bitstream verification passed");
            }
        }

    }while(false);

	if(!m_pConfigHelper->IsKeepFilesSet()) {
		DeleteFile(m_strOutputFile);
	}

    SAFE_DELETE(m_pLTRFrameInfo);


    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::SetSSERegKey()
{
    HRESULT hr	= S_OK;
    HKEY hk = NULL;
    DWORD dwData = 0;
    const TCHAR *lpcszValueName = L"SIMD Level";
    unsigned int random ;
    BOOL bSSETypeSupported = FALSE;
    Log::Comment(L"Entering Reconstructed frame comparison test to compare the reconstructed Hash file with the reference hash from H264 decoder");

    do{

        if(m_pConfigHelper->IsSSESet())
        {
            m_strSSEType =  m_pConfigHelper->GetSSEType();
            if (m_strSSEType.Compare(L"sse2")==0)
                dwData = 1;
            else if (m_strSSEType.Compare(L"sse3")==0)
                dwData = 2;
            else if (m_strSSEType.Compare(L"ssse3")==0)
                dwData = 3;
            else if (m_strSSEType.Compare(L"sse4.1")==0)
                dwData = 4;
        }
        else //if SSE Type isnt set via Config, set to now
        {
            random = rand() % SSE_MAX + SSE_MIN  ; //generate random number between the range 1 & 4
            // Supported values : sse2(reg key value 1), sse3(reg key value 2), ssse3(reg key value 3) , sse4.1 (reg key value 4)

            if(random == 1 || random == 2)
            {
                m_strSSEType.Append(L"sse");
                m_strSSEType.AppendFormat(L"%u",random+1);
            }            
            else if (random == 3)
            {
                m_strSSEType.Append(L"ssse");
                m_strSSEType.AppendFormat(L"%u",random);
            }
            else if (random == 4)
            {
                m_strSSEType.Append(L"sse");
                m_strSSEType.AppendFormat(L"%u",random);
                m_strSSEType.Append(L".1");
            }

            dwData = random; //Set REG KEY VALUE for the corresponding SIMD LEVEL
        } 

        // dwData can have one of the following val : // Supported values : sse2(reg key value 1), sse3(reg key value 2), ssse3(reg key value 3) , sse4.1 (reg key value 4)

        switch (dwData)
        {
        case 1:
            bSSETypeSupported = g_SupportSSE2();
            break;
        case 2:
            bSSETypeSupported = g_SupportSSE3();
            break;
        case 3:
            bSSETypeSupported = g_SupportSSSE3();
            break;
        case 4:
            bSSETypeSupported = g_SupportSSE4_1();
            break;
        default : 
            bSSETypeSupported = FALSE;
            break;
        };
        InternalLog(L"SSE type to be used : %s; Supported : %d ", (const WCHAR*)m_strSSEType, (int)bSSETypeSupported);

        if (bSSETypeSupported)
        {
            InternalLog(L"%s is supported on this machine, setting the corresponding reg key", (const WCHAR*)m_strSSEType);

            if(dwData != 0)
            {
                if(ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Scrunch\\H264Enc", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hk, NULL))
                {
                    if(RegSetValueEx(hk, lpcszValueName, 0, REG_DWORD, (LPBYTE) &dwData, sizeof(DWORD)) != ERROR_SUCCESS)    
                    {
                        Log::Comment(TEXT("ERROR>> Registry value could not be set for desired SIMD Level."));
                        break;
                    }
                }
            }
        }
        else
        {
            // If SSE Type isnt supported on the machine, skip the test & mark as passed
            InternalLog(L"%s is not supported on this machine, skipping the test", (const WCHAR*)m_strSSEType);
            m_bIsSSESupported = FALSE;
            hr = S_OK;
        }

    } while(FALSE);

    if (hk)
    {
        RegCloseKey(hk);
        hk = NULL;
    }
    return hr;       

}
//below is an outline.
STDMETHODIMP
    CH264EncodeVerify::CompareReconstructedFrame()
{
    HRESULT hr	= S_OK;
    FILE *fpRefFile = NULL;
    TCHAR strRefFileName[MAX_PATH];
    TCHAR strReconFile[MAX_PATH];
    do
    {   
        memset(strRefFileName, 0 , sizeof(TCHAR)* MAX_PATH);
        memset(strReconFile, 0 , sizeof(TCHAR)* MAX_PATH);

        WEX::Common::String reconFilePath = m_pConfigHelper->GetReconFilePath();
        WEX::Common::String refFilePath = m_pConfigHelper->GetRefFilePath();
        TCHAR* reconFileName = NULL;

        reconFileName = _tcsrchr(reconFilePath, _T('\\'));

        if(reconFileName != NULL)
        {
            reconFileName++;
        }

        // Get Reference file name \\refFilePath\SSEType\reconfilename

        hr = _tcscat_s(strRefFileName, MAX_PATH, (LPCWSTR)refFilePath);
        if(FAILED(hr))
        {
            _HRL(E_FAIL, Log::Comment(L"Failed in copy ref file name!"));
            break;
        }

        _tcsncat(strRefFileName,L"\\",1);

        hr = _tcscat_s(strRefFileName, MAX_PATH, (LPCWSTR)m_strSSEType);
        if(FAILED(hr))
        {
            _HRL(E_FAIL, Log::Comment(L"Failed in append SSE type!"));
            break;
        }

        _tcsncat(strRefFileName,L"\\",1);

        hr = _tcscat_s(strRefFileName, MAX_PATH, reconFileName);
        if(FAILED(hr))
        {
            _HRL(E_FAIL, Log::Comment(L"Failed in append SSE type!"));
            break;
        }

        InternalLog(L"Reference File: %s",(const WCHAR*)strRefFileName);


        if(_wfopen_s(&fpRefFile, (const WCHAR*)strRefFileName, L"wb") != 0)
        {
            _HRL(E_FAIL, Log::Comment(L"Failed to open Reference file"));
        }

        hr = _tcscat_s(strReconFile, MAX_PATH, (LPCWSTR)reconFilePath);
        if(FAILED(hr))
        {
            _HRL(E_FAIL, Log::Comment(L"Failed in copy ref file name!"));
            break;
        }

        if(fpRefFile)
            fclose(fpRefFile);

        _HRL(ExecuteCompare(strRefFileName, strReconFile), Log::Comment(L"Failed to run ExecuteCompare on the files"));

    }while(false);

    return hr;       

}



STDMETHODIMP
    CH264EncodeVerify::GetReferenceFileAndCompare(TCHAR* hashFile)
{
    HRESULT hr	= S_OK;
    Log::Comment(L"Entering Compare type test to compare the generated Hash file with the reference");
    FILE* fpHashFile = NULL;
    FILE* fpRefFile  = NULL;

    do{
        TCHAR strRefFileFullPath[MAX_PATH];
        memset(strRefFileFullPath, 0, sizeof(TCHAR)*MAX_PATH); 
        WEX::Common::String refFilePath = L"";
        refFilePath = m_pConfigHelper->GetRefFilePath();

        hr = _tcscat_s(strRefFileFullPath, MAX_PATH, (LPCWSTR)refFilePath);
        if(FAILED(hr))
        {
            Log::Comment(L"Failed in copy ref file path!");
            break;
        }

        _tcsncat_s(strRefFileFullPath,_T("\\"), 1);

        hr = _tcscat_s(strRefFileFullPath, MAX_PATH, (LPCWSTR)m_strSSEType);

        if(FAILED(hr))
        {
            Log::Comment(L"Failed in copy ref file path!");
            break;
        }

        _tcsncat_s(strRefFileFullPath,_T("\\"),1);


        if(hashFile != NULL && strRefFileFullPath != NULL )
        {
            TCHAR strRefFile[MAX_PATH];
            memset(strRefFile, 0, sizeof(TCHAR)* MAX_PATH); 

            //Set Ref File Name from refFileFullPath & HashFileName
            hr = _tcscat_s(strRefFile, MAX_PATH, strRefFileFullPath);
            if(FAILED(hr))
            {
                Log::Comment(L"Failed in copying ref file full path!");
                break;
            }

            hr = _tcscat_s(strRefFile, MAX_PATH, hashFile);
            if(FAILED(hr))
            {
                Log::Comment(L"Failed in setting reference file name!");
                break;
            }

            InternalLog(L"Reference File: %s",(const WCHAR*)strRefFile);

            if(_wfopen_s(&fpHashFile, (const WCHAR*)hashFile, L"r") != 0)
            {
                _HRL(E_FAIL, Log::Comment(L"Failed to open Hash file"));
                break;
            }

            if(_wfopen_s(&fpRefFile, (const WCHAR*)strRefFile, L"r") != 0)
            {
                _HRL(E_FAIL, Log::Comment(L"Failed to open Reference Hash file"));
                break;
            }

            if(fpHashFile != NULL)
            {
                fclose(fpHashFile);
            }
            if(fpRefFile != NULL)
            {
                fclose(fpRefFile);
            }

            _HRL(ExecuteCompare(strRefFile, hashFile), Log::Comment(L"Failed to run ExecuteCompare on the files"));

        }
    } while(FALSE);

    if(fpHashFile != NULL)
    {
        fclose(fpHashFile);
    }

    return hr;
}
#ifndef MINWIN  
BOOL ShellExecuteExPrivate(_Inout_ SHELLEXECUTEINFO *pExecInfo)
{

    typedef BOOL(WINAPI* LPFUNCTION)(SHELLEXECUTEINFO*);

    HINSTANCE hlibShell32 = LoadLibraryA("Shell32.DLL");
    if (hlibShell32 == NULL)
    {
        return FALSE;
    }

    LPFUNCTION lpfnShellExecuteEx = (LPFUNCTION)GetProcAddress(
        hlibShell32, "ShellExecuteEx");
    if (lpfnShellExecuteEx == NULL)
    {
        return FALSE;
    }

    return(*lpfnShellExecuteEx)(pExecInfo);
}
#endif

STDMETHODIMP
    CH264EncodeVerify::ExecuteCompare(TCHAR* strRefFile, TCHAR* strHashFile)
{
    HRESULT hr = S_OK;
    TCHAR compareFileCmdLine[MAX_PATH];
    FILE *fpCompLog = NULL;
    char *pszFileText = NULL;
    char *pszResult = NULL;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    do
    {
        pszResult = (char*)calloc(6, sizeof(char));
        if(NULL == pszResult)
        {
            hr = E_OUTOFMEMORY;
            break;
        }

        memset(compareFileCmdLine, 0, sizeof(TCHAR)* MAX_PATH);
        if(NULL == compareFileCmdLine)
        {
            hr = E_OUTOFMEMORY;
            break;
        }
        //creating parameters -1"filename1" -2"filename2"  for compare.exe
        _tcsncat_s(compareFileCmdLine,_T(" -1"),3);
        _tcsncat_s(compareFileCmdLine,_T("\""),1);
        _tcsncat_s(compareFileCmdLine,strRefFile,_tcslen(strRefFile));
        _tcsncat_s(compareFileCmdLine,_T("\""),1);
        _tcsncat_s(compareFileCmdLine,_T(" -2"),3);
        _tcsncat_s(compareFileCmdLine,_T("\""),1);
        _tcsncat_s(compareFileCmdLine,strHashFile,_tcslen(strHashFile));
        _tcsncat_s(compareFileCmdLine,_T("\""),1);

#ifndef MINWIN       
        SHELLEXECUTEINFO m_shellExecuteInfo;

        if(_tremove(_T("CompLog.txt")) == -1)
        {
            Log::Comment(L"\nSTATUS>> Could not delete complog.txt\n");
        }
        ZeroMemory(&m_shellExecuteInfo, sizeof(SHELLEXECUTEINFO));
        m_shellExecuteInfo.cbSize=sizeof(SHELLEXECUTEINFO);
        m_shellExecuteInfo.fMask=SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
        m_shellExecuteInfo.lpFile=_T("compare.exe");
        m_shellExecuteInfo.nShow = SW_HIDE;
        m_shellExecuteInfo.hInstApp=NULL;
        m_shellExecuteInfo.lpParameters=compareFileCmdLine;
        if (!ShellExecuteExPrivate(&m_shellExecuteInfo))
        {
            Log::Comment(L"\nERROR>>Unable to execute Compare\n");
            hr = E_FAIL;
            break;
        }
        WaitForSingleObject(m_shellExecuteInfo.hProcess,INFINITE);
#else
#pragma warning(suppress: 6335)
        if (!CreateProcess(L"compare.exe",
            compareFileCmdLine,
            NULL, NULL, FALSE, 0, NULL, NULL, 
            &si,
            &pi))
        {
            Log::Comment(L"\nERROR>>Unable to execute Compare\n");
            hr = E_FAIL;
            break;
        }
        WaitForSingleObject(pi.hProcess,INFINITE);
#endif //#ifndef MINWIN
        fpCompLog = _tfopen( _T("complog.txt"), _T("r+t"));
        if(fpCompLog == NULL)
        {
            Log::Comment(L"ERROR>> Unable to open the log file\n");
            hr = E_FAIL;
            break;
        }
        fseek(fpCompLog, 0L, SEEK_END);
        long lFileLength = ftell(fpCompLog);
        rewind(fpCompLog);
        pszFileText = (char*)calloc(lFileLength + 1, sizeof(char));
        if(!(pszFileText))
        {
            Log::Comment(L"\nERROR>> Insufficient memory to read file\n");
            hr = E_FAIL;
            break;
        }
        int n = fread((pszFileText), sizeof(char), lFileLength, fpCompLog); 

        if(ferror(fpCompLog))
        {
            Log::Comment(L"\nERROR>> Failed to read the log file\n");
            hr = E_FAIL;
            break;
        }

        pszResult = strncpy(pszResult,pszFileText,5);

        if(strcmp(pszResult,"Error")==0)
        {
            hr = E_FAIL;            
            Log::Comment(L"\n Failed in comparing the hash files\n");

            break;
        } 
    }while(FALSE);

    if(fpCompLog)
        fclose(fpCompLog);

    SAFE_FREE(pszResult);
    SAFE_FREE(pszFileText);
    return hr;
}

STDMETHODIMP 
    CH264EncodeVerify::H264BitStreamEncodeWithNetLoss(UINT32 run)
{
    HRESULT hr = S_OK;
    BOOL fIsTopologyConfig = TRUE; 

    do
    {
        _HR((m_pConfigHelper) ? S_OK : E_UNEXPECTED);

        // Read input file path from the test xml
        WEX::Common::String strInputFile = m_pConfigHelper->GetInputPath();
        if (strInputFile.IsEmpty())
        {
            _HR(E_INVALIDARG);
        }

        _HRL(StringCchCopyW(m_pwcLocalInputFile, MAX_PATH, (LPCWSTR)strInputFile) , InternalLog(L" \n Failed to copy the input file name \n"));

        // Read output file format from the test xml (L"container")
        m_output = m_pConfigHelper->GetOutputFormat();

        // Read output file path from the test xml
        if(MFTopologyHelper::Renderer != m_output)
        {
            m_strOutputFile = m_pConfigHelper->GetOutputPath();
            if (m_strOutputFile.IsEmpty())
            {
                _HR(E_INVALIDARG);
            }
            TCHAR* truncFileName = NULL;
            _HRL(StringCchCopyW(m_pwcLTRWithLossOutputFile, MAX_PATH, (LPCWSTR)m_strOutputFile), InternalLog(L" \n Failed to copy the output file name\n"));
            _HRL(StringCchCopyW(m_pwcLTRWithoutLossOutputFile, MAX_PATH, (LPCWSTR)m_strOutputFile), InternalLog(L" \n Failed to copy the output file name\n"));
                        
            truncFileName = _tcsstr(m_pwcLTRWithLossOutputFile, L".mp4");
            if(truncFileName != NULL)
            {
                m_pwcLTRWithLossOutputFile[truncFileName - m_pwcLTRWithLossOutputFile ] = '\0';
                _tcsncat(m_pwcLTRWithLossOutputFile, L"_Loss.mp4", 9);
                
            }
            
            truncFileName = _tcsstr(m_pwcLTRWithoutLossOutputFile, L".mp4");
            if(truncFileName != NULL)
            {
                m_pwcLTRWithoutLossOutputFile[truncFileName - m_pwcLTRWithoutLossOutputFile ] = '\0';
                _tcsncat(m_pwcLTRWithoutLossOutputFile, L"_NoLoss.mp4", 11);
            }

        }

        _HR(m_pConfigHelper->GetConfigCustomParser((ITAEFConfigCustomParser **)&m_pH264CustomParser));

        //Get LTR Info - MAX Ref Frames and Trust Mode
        UINT32 uiMaxLTR, uiTrustMode;
        m_bIsLTRNetLossMode = FALSE;
        m_bIsLTRNetLossAndErrorMode = FALSE;

        if(S_OK == m_pH264CustomParser->GetLTRInfo(&uiMaxLTR,&uiTrustMode))
        {
            SAFE_DELETE(m_pLTRFrameInfo);
            m_pLTRFrameInfo = new stLTRFrameInfo;
            if(m_pLTRFrameInfo == NULL){
                _HR(E_OUTOFMEMORY);
            }
            memset(m_pLTRFrameInfo,0,sizeof(stLTRFrameInfo));
            m_pLTRFrameInfo->uiMaxLTRs = uiMaxLTR;
            m_pLTRFrameInfo->bLTRUseConstraint = TRUE;
        }
        
        UINT32 uiNumFrames = 0;
        UINT32 uiGopSize = 0;
        _HRL(m_pConfigHelper->GetGOPSize(&uiGopSize), InternalLog(L"Couldn't get GOP size\n"));
        _HRL(GetInputFrameCount(&uiNumFrames), InternalLog(L"Couldn't get total input frame count\n"));

        InternalLog(L" \n #Input Frame Count : %d \n", uiNumFrames);

        //GENERATE MODEL

        m_uiRandomSeed = m_pConfigHelper->GetRandomSeed();
        
        if(m_uiRandomSeed == 0)
            m_uiRandomSeed = GetTickCount(); //Obtain the Random seed ; use this random seed to generate the Markov Sequence per sample basis

        srand(m_uiRandomSeed);
        InternalLog(L" \n RandomSeed used in model generation : %d \n", m_uiRandomSeed);

        NetworkLossModel model = MODEL_NULL;
        
        m_pModelSeq = new Model();

        if(m_pModelSeq == NULL)
        {
            _HR(E_OUTOFMEMORY);		
        }
        
        _HRL(m_pH264CustomParser->GetModelType(&model), InternalLog(L"Couldn't get the Model type from the custom parser\n"));
        DOUBLE pGb = 0.0;
        DOUBLE pBg = 0.0; 
        DOUBLE errorRate = 0.0;
        DOUBLE burstnessK = 0.0; 
        DOUBLE dErrRate = 0.0;

        switch(model) {

        case MARKOV:
           
            _HRL(m_pH264CustomParser->GetModelParams(0, &pGb), InternalLog(L"Couldn't get the Probability Pgb from the custom parser\n"));
            _HRL(m_pH264CustomParser->GetModelParams(1, &pBg), InternalLog(L"Couldn't get the Probability Pbg from the custom parser\n"));
            _HRL(m_pModelSeq->GenerateEmissions(m_uiRandomSeed, uiNumFrames, pGb, pBg), InternalLog(L"Failed to generate emissions for the model\n"));
            break;

        case SKYPE:
           
            _HRL(m_pH264CustomParser->GetModelParams(0, &errorRate), InternalLog(L"Couldn't get the Error Rate from the custom parser\n"));
            _HRL(m_pH264CustomParser->GetModelParams(1, &burstnessK), InternalLog(L"Couldn't get the Avg Burstness from the custom parser\n"));
            _HRL(m_pModelSeq->ConvertRK2Pgb(errorRate, burstnessK, &pGb), InternalLog(L"Failed to calculate Probability Pgb\n"));
            _HRL(m_pModelSeq->ConvertRK2Pbg(errorRate, burstnessK, &pBg), InternalLog(L"Failed to calculate Probability Pbg\n"));
            _HRL(m_pModelSeq->GenerateEmissions(m_uiRandomSeed, uiNumFrames, pGb, pBg), InternalLog(L"Failed to generate emissions for the model\n"));
            break;		

        case UNIFORM:
            
            _HRL(m_pH264CustomParser->GetModelParams(0, &dErrRate), InternalLog(L"Couldn't get the Error rate for the uniform model from the custom parser\n"));
            _HRL(m_pModelSeq->GenerateEmissions(uiNumFrames, dErrRate), InternalLog(L"Failed to generate emissions for the model\n"));
            break;
            
        default:
            _HRL(E_FAIL, InternalLog(L"\n Unknown Model Type \n"));
        }

        ErrorLossType errorType = ERROR_NULL;
        _HRL(m_pH264CustomParser->GetErrorLossType(&errorType), InternalLog(L"Couldn't get the Error type from the custom parser\n"));

        //Encode with LTR enabled but no loss
        InternalLog(L"\n Encoding with LTR enabled but no loss \n");
        
        m_bIsLTRNetLossMode = TRUE;
        m_bIsLTRNetLossAndErrorMode = FALSE;
        m_strOutputFile = m_pwcLTRWithoutLossOutputFile; //set output file
        
        m_pSampleProcessor = new CDirectSampleProcessor(this);
        if (NULL == m_pSampleProcessor)
        {
            _HR(E_OUTOFMEMORY);
        }
        _HRL(m_pSampleProcessor->ProcessSamples(run),InternalLog(L"Failed to encode\n"));
        SAFE_DELETE(m_pSampleProcessor);


        //Encode with LTR enabled and with network loss
        InternalLog(L"Encoding with LTR enabled but with packet loss \n");
        m_bIsLTRNetLossMode = TRUE; //Set Network Loss & error mode to TRUE ; the actual checks for various modes of loss is carried out in CDirectSampleProcessor::HandleOutput
        m_bIsLTRNetLossAndErrorMode = TRUE;
        m_strOutputFile = m_pwcLTRWithLossOutputFile; //set output file
        
        m_pSampleProcessor = new CDirectSampleProcessor(this);
        if (NULL == m_pSampleProcessor)
        {
            _HR(E_OUTOFMEMORY);
        }
        _HRL(m_pSampleProcessor->ProcessSamples(run),InternalLog(L"Failed to encode\n"));
        SAFE_DELETE(m_pSampleProcessor);

        InternalLog(L"\n Random Seed to Generate Emissions of Markov Model:%d \n", m_uiRandomSeed);

        //Save the FrameTypeQP values from each frame to a file for repro purposes
        String strQPVal= L"QPValuesForAllFrames.txt";
        FILE *fpQPValFile = _wfopen(strQPVal,L"w+");
        
        if(fpQPValFile!= NULL)
        {
            fprintf_s(fpQPValFile, " \n RandomSeed used in model generation : %d \n", m_uiRandomSeed);
            for(int i = 0 ; i < uiNumFrames; i++)
            {
                UINT64 ui64FrameQP = 0 ;
                _HRL(m_pH264CustomParser->GetFrameTypeQP(i, &ui64FrameQP), InternalLog(L"\n Unable to obtain QP value for frame %d\n",i));
                fprintf_s(fpQPValFile, " Frame %d : %llx \n", i, ui64FrameQP);
            }

            fclose(fpQPValFile);
        }

        //Decoding and verification
        DWORD dwFrameSize = 0;
        UINT32 uiTotalSamples = 0;

        uiTotalSamples = uiNumFrames;

        InternalLog (L"\n Decoding File 1\t\t\n");
        WCHAR* strYuvNoloss = L"LTR_no_loss.yuv";
        _HRL(DecodeH264File (m_pwcLTRWithoutLossOutputFile, strYuvNoloss, &dwFrameSize, uiTotalSamples), InternalLog(L"Failed to decode the first file\n"));

        InternalLog (L"\n Decoding File 2\t\t\n");
        WCHAR* strYuvloss = L"LTR_loss.yuv";

        if(errorType == BURST_ERROR) 
            uiTotalSamples = uiNumFrames - m_arrLTRLostFrames.GetSize(); //since frame lost == frame not written to file

        _HRL(DecodeH264File (m_pwcLTRWithLossOutputFile, strYuvloss, &dwFrameSize, uiTotalSamples), InternalLog(L"Failed to decode the second file \n"));
        
        InternalLog (L"\n Comparing Two Files\t\t\n"); 
        hr = VerifyLTROutputFiles (strYuvNoloss, strYuvloss, dwFrameSize, uiTotalSamples, errorType); 
        if(hr != E_SKIP )
        {
            break;
        }
       
    } while (TRUE);

    SAFE_DELETE(m_pSampleProcessor);
    SAFE_DELETE(m_pLTRFrameInfo);
    SAFE_DELETE(m_pH264EncVerifier);


    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::H264BitStreamEncode(UINT32 run)
{
    HRESULT hr = S_OK;
    BOOL fIsTopologyConfig = TRUE; 

    do
    {
        _HR((m_pConfigHelper) ? S_OK : E_UNEXPECTED);

        // Read input file path from the test xml
        WEX::Common::String strInputFile = m_pConfigHelper->GetInputPath();
        if (strInputFile.IsEmpty())
        {
            _HR(E_INVALIDARG);
        }

        StringCchCopyW(m_pwcLocalInputFile, MAX_PATH, (LPCWSTR)strInputFile);

        // Read output file format from the test xml (L"container")
        m_output = m_pConfigHelper->GetOutputFormat();

        // Read output file path from the test xml
        if(MFTopologyHelper::Renderer != m_output)
        {
            m_strOutputFile = m_pConfigHelper->GetOutputPath();

            m_strHWOutputFile = WEX::Common::String(m_strOutputFile);
            m_strSWOutputFile = WEX::Common::String(m_strOutputFile);
            int index = m_strHWOutputFile.ReverseFind(L'.');
            m_strHWOutputFile.Insert(index,L"_p1");
            m_strSWOutputFile.Insert(index,L"_p2");

            m_strOutputFile = m_strHWOutputFile;

        }
        if (m_strOutputFile.IsEmpty())
        {
            _HR(E_INVALIDARG);
        }

        _HR(m_pConfigHelper->GetConfigCustomParser((ITAEFConfigCustomParser **)&m_pH264CustomParser));

        if(m_pH264CustomParser->IsDynamicStressTest()){
            m_pSampleProcessor = new CStressDirectSampleProcessor(this);
            if (NULL == m_pSampleProcessor)
            {
                _HR(E_OUTOFMEMORY);
            }
            _HRL(m_pSampleProcessor->ProcessSamples(run),InternalLog(L"Failed to encode\n"));
            break;
        }

        // Read sample processing mode (topology/no_topology)
        hr = m_pH264CustomParser->GetIsTopology(&fIsTopologyConfig);
        if (S_OK != hr)
        {
            InternalLog(L"Failed ITAEFConfigCustomParser::GetIsTopology() with hr = 0x%08X", hr);
        }

        if (TRUE == fIsTopologyConfig)
        {
            m_pSampleProcessor = new CTopologySampleProcessor(this);
            if(m_pSampleProcessor == NULL){
                _HRL(E_OUTOFMEMORY,InternalLog(L"Failed to create TopologySampleProcessor\n"));
            }
            _HRL(m_pSampleProcessor->ProcessSamples(run),InternalLog(L"Failed to encode in pass 1\n"));
            m_pOldTopologyHelper = m_pTopologyHelper;
            SAFE_ADDREF(m_pOldTopologyHelper);

            InternalLog(L"The HW mode is %d\n",m_pConfigHelper->GetHWMode());
            if (TRUE == fIsTopologyConfig && m_pConfigHelper->GetHWMode() == MFTopologyHelper::HWMode::HW)
            {
                InternalLog(L"HW was actually used so also encoding with SW for PSNR comparison\n");
                //Need to make software file too
                m_bIsSWFileCreated = TRUE;
                m_strHWOutputFile = m_strOutputFile;
                m_strOutputFile = m_strSWOutputFile;
                _HR(m_pConfigHelper->StrictlyForceSWMode());
                //GetHWMode will now return SW, so future HWMode checks should also check for !m_bIsSWFileCreated
                SAFE_DELETE(m_pSampleProcessor);
                SAFE_RELEASE(m_pTopologyHelper);
                m_pSampleProcessor = new CTopologySampleProcessor(this);
                if (NULL == m_pSampleProcessor)
                {
                    _HR(E_OUTOFMEMORY);
                }
                _HRL(m_pSampleProcessor->ProcessSamples(run),InternalLog(L"Failed to encode in pass 2\n"));
                InternalLog(L"Completed software encode");

                m_strSWOutputFile = m_strOutputFile;
                m_strOutputFile = m_strHWOutputFile;

                IMFTopologyHelper* pSwapTopologyHelper = m_pTopologyHelper;
                m_pTopologyHelper = m_pOldTopologyHelper;
                m_pOldTopologyHelper = pSwapTopologyHelper;
                _HR(m_pConfigHelper->StrictlyForceHWMode());
            }
        }
        else
        {
            UINT32 uiMaxLTR, uiTrustMode;
            if(S_OK == m_pH264CustomParser->GetLTRInfo(&uiMaxLTR,&uiTrustMode)){
                SAFE_DELETE(m_pLTRFrameInfo);
                m_pLTRFrameInfo = new stLTRFrameInfo;
                if(m_pLTRFrameInfo == NULL){
                    _HR(E_OUTOFMEMORY);
                }
                memset(m_pLTRFrameInfo,0,sizeof(stLTRFrameInfo));
                m_pLTRFrameInfo->uiMaxLTRs = uiMaxLTR;
                m_pLTRFrameInfo->bLTRUseConstraint = TRUE;
            }
            m_pSampleProcessor = new CDirectSampleProcessor(this);
            if (NULL == m_pSampleProcessor)
            {
                _HR(E_OUTOFMEMORY);
            }
            hr = m_pSampleProcessor->ProcessSamples(run);
            if (E_SKIP == hr)
            {
                // Don't log error message for skipped tests - just causes confusion (the fact that the test is being skipped is already logged)
                break;
            }
            else
            {
                _HRL(hr, InternalLog(L"Failed to encode\n"));
            }

            if(m_pH264CustomParser->IsLTRModeSet()){
                SAFE_DELETE(m_pH264EncVerifier);
                m_pH264EncVerifier = new CH264EncVerifier();
                if (NULL == m_pH264EncVerifier)
                {
                    _HR(hr = E_OUTOFMEMORY);
                }
                _HRL(m_pH264EncVerifier->SetLTRInfo(m_pLTRFrameInfo),InternalLog(L"Failed to set LTR info on Bitstream Analyzer")); 
                InterlockedIncrement(&g_ulLoggerCount);
                g_pLogger   = m_pLog;

                // Parse H.264 bitstream using bitstream analyzer
                _HRL(hr = m_pH264EncVerifier->ParseBSAnalyzer(m_strOutputFile, &LogToGlobalLogger),InternalLog(L"Failed bitstream analysis with hr = %x\n",hr));

                // Cleanup logging wrapper function
                if(InterlockedDecrement(&g_ulLoggerCount) == 0)
                {
                    g_pLogger   = NULL;
                } 
            }
        }

        if(hr != E_SKIP || m_pConfigHelper->IsHWModeSet() ||(m_pConfigHelper->GetHWMode() == MFTopologyHelper::HWMode::SW && !m_bIsSWFileCreated))
        {
            break;
        }
        InternalLog(L"Using SW encoder as the HW encoder tested didn't support the test settings Profile Level / Bit rate");
        _HRL(m_pConfigHelper->ForceSWMode(),InternalLog(L"Failed in ITaefConfigHelper::ForceSWMode as the HW mode was already set in the configurations, hr = 0x%08X" , hr));         

    } while (TRUE);


    SAFE_DELETE(m_pLTRFrameInfo);
    SAFE_DELETE(m_pH264EncVerifier);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::TopologyInsertH264FuzzMFT(
    __in IMFTopology* pTopology)
{
    HRESULT hr = S_OK;

    CConfigureFuzzEncoder* pFuzzMFTSetup = NULL;
    IMFTransform* pMFT = NULL;
    IMFTransform* pFuzzMFT = NULL;
    IMFTopologyNode* pFuzzNode = NULL;
    IMFTopologyNode* pPrevNode = NULL;
    IMFTopologyNode* pNode = NULL;
    IMFMediaType* pMTFuzzInput = NULL;
    IMFMediaType* pMTFuzzOutput = NULL;
    IMFMediaType* pMTOutput = NULL;
    IUnknown* pObject = NULL;
    IMFMediaType* pMTInput = NULL;

    BOOL bFound = FALSE;
    BOOL bPrevFound = FALSE;
    WORD wNodes = 0;
    MF_TOPOLOGY_TYPE topoType;
    MF_TOPOLOGY_TYPE prevTopoType;
    GUID guidMajorType = GUID_NULL;
    GUID guidSubType = GUID_NULL;
    DWORD dwStreams = 0;
    BOOL bSelected = FALSE;

    if (!pTopology)
    {
        return E_POINTER;
    }

    do
    {
        _HR(CConfigureFuzzEncoder::CreateInstance(m_pConfigHelper, m_pLog, (ITestMFTSetup**)(&pFuzzMFTSetup)));

        _HR(MFCreateTopologyNode(MF_TOPOLOGY_TRANSFORM_NODE, &pFuzzNode));
        _HR(CoCreateInstance(CLSID_MediaFuzzerMFT, NULL, CLSCTX_INPROC_SERVER, IID_IMFTransform, (LPVOID*)&pFuzzMFT));
        _HR(pTopology->GetNodeCount(&wNodes));

        for (WORD w = 0; w < wNodes; w++)
        {
            SAFE_RELEASE(pNode);
            SAFE_RELEASE(pObject);
            SAFE_RELEASE(pMFT);
            SAFE_RELEASE(pMTOutput);
            SAFE_RELEASE(pPrevNode);
            SAFE_RELEASE(pMTFuzzInput);
            SAFE_RELEASE(pMTFuzzOutput);

            _HR(pTopology->GetNode(w, &pNode));
            _HR(pNode->GetNodeType(&topoType));

            if (MF_TOPOLOGY_TRANSFORM_NODE == topoType)
            {
                _HR(pNode->GetObject(&pObject));
                _HR(pObject->QueryInterface(IID_IMFTransform, (LPVOID*)&pMFT));
                _HR(pMFT->GetOutputCurrentType(0, &pMTOutput));

                pMTOutput->GetGUID(MF_MT_MAJOR_TYPE, &guidMajorType);
                pMTOutput->GetGUID(MF_MT_SUBTYPE, &guidSubType);

                // We found H.264 encoder MFT, insert fuzz MFT right before
                if (MFMediaType_Video == guidMajorType && 
                    MFVideoFormat_H264 == guidSubType)
                {
                    _HR(pMFT->GetInputCurrentType(0, &pMTInput));

                    DWORD dwIndex = 0;
                    _HR(pNode->GetInput(0, &pPrevNode, &dwIndex));

                    // Copy H.264 encoder's input media type to Fuzz MFT's input and output types
                    // (Fuzz MFT doesn't change media type, just fuzzes the samples)
                    _HR(MFCreateMediaType(&pMTFuzzInput));
                    _HR(pMTInput->CopyAllItems(pMTFuzzInput));
                    _HR(pFuzzMFT->SetInputType(0, pMTFuzzInput, 0));
                    _HR(MFCreateMediaType(&pMTFuzzOutput));
                    _HR(pMTInput->CopyAllItems(pMTFuzzOutput));
                    _HR(pFuzzMFT->SetOutputType(0, pMTFuzzOutput, 0));

                    bFound = TRUE;
                    break;
                }
            }
        }

        if (S_OK != hr || 
            FALSE == bFound)
        {
            break;
        }

        _HR(pFuzzNode->SetObject(pFuzzMFT));
        _HR(pTopology->AddNode(pFuzzNode));
        _HR(pPrevNode->DisconnectOutput(0));
        _HR(pPrevNode->ConnectOutput(0, pFuzzNode, 0));
        _HR(pFuzzNode->ConnectOutput(0, pNode, 0));

        if (pFuzzMFTSetup)
        {
            _HRL(pFuzzMFTSetup->Configure(pFuzzMFT, pMTInput, &pMTFuzzInput, &pMTFuzzOutput), 
                InternalLog(L"Failed to configure Fuzz MFT, hr = 0x%08X", hr));
        }

    } while (FALSE);

    SAFE_RELEASE(pFuzzMFTSetup);
    SAFE_RELEASE(pMFT);
    SAFE_RELEASE(pFuzzMFT);
    SAFE_RELEASE(pFuzzNode);
    SAFE_RELEASE(pPrevNode);
    SAFE_RELEASE(pNode);
    SAFE_RELEASE(pMTFuzzInput);
    SAFE_RELEASE(pMTFuzzOutput);
    SAFE_RELEASE(pMTOutput);
    SAFE_RELEASE(pObject);
    SAFE_RELEASE(pMTInput);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::WriteSampleToFile(
    IMFSample*  pSample,
    FILE*       fpFile,
    FILE*       fpHashFile)
{
    HRESULT         hr              = S_OK;
    IMFMediaBuffer* pBuffer         = NULL;
    BYTE*           pbBufferData    = NULL;
    DWORD           dwBufferLength  = 0;
    ULONGLONG       ullHashVal      = 0; 

    do
    {
        if( (pSample == NULL)   ||
            (fpFile == NULL)    )
        {
            hr = E_POINTER;
            break;
        }

        hr = pSample->ConvertToContiguousBuffer(
            &pBuffer
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->Lock(
            &pbBufferData,
            NULL,
            &dwBufferLength
            );
        if(FAILED(hr))
        {
            break;
        }

        if(fpHashFile != NULL)
        {
            CalculateFNV1aHash(pbBufferData, dwBufferLength, &ullHashVal);
            if(fwrite((BYTE*)&ullHashVal, sizeof(ULONGLONG), 1, fpHashFile) != 1)
            {
                hr = E_FAIL;
                Log::Comment(L"Failed to write data to Hash file");
                break;
            }

        }

        if(fwrite(
            pbBufferData,
            sizeof(BYTE),
            dwBufferLength,
            fpFile
            ) != dwBufferLength)
        {
            hr = E_FAIL;
            break;
        }
    }while(false);

    if(pbBufferData != NULL)
    {
        pBuffer->Unlock();
    }
    SAFE_RELEASE(pBuffer);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::H264CheckPSNR(double *pdHWPSNR, double *pdSWPSNR, UINT32 *puiHWBitrate, UINT32* puiSWBitrate, BOOL bValidate)
{
    HRESULT hr = S_OK;

    IMFSourceReader* hardwareSource;
    IMFSourceReader* softwareSource;
    IMFSourceReader* originalSource;

    IMFAttributes *pAllowXVP = NULL;

    IMFMediaType *currentType;
    UINT32 dwHeight, dwWidth;
    UINT32 numFrames = 0;

    LONGLONG llTotalDuration = 0;
    LONGLONG llHWBits = 0;
    LONGLONG llSWBits = 0;

    IMFSample *pHWSample = NULL, *pSWSample = NULL, *pOrigSample = NULL;
    DWORD streamIndex;
    DWORD streamFlags;
    LONGLONG timeStamp;

    IMFMediaBuffer *pHWMediaBuffer = NULL, *pSWMediaBuffer = NULL, *pOrigMediaBuffer = NULL;
    BYTE *pHWBuffer = NULL, *pSWBuffer = NULL, *pOrigBuffer = NULL;
    DWORD dwHWBufSize = 0, dwSWBufSize = 0, dwOrigBufSize = 0;
    LONGLONG llSampleDuration;

    BYTE *pHWCroppedBuffer = NULL, *pSWCroppedBuffer = NULL;

    BOOLEAN bCropNeeded = FALSE;

    FILE* hwFile = NULL;
    FILE* swFile = NULL;

    CDXCompare dxCompare, dxCompare2;

    WEX::Common::String strInputFile = m_pConfigHelper->GetInputPath();

    do{
        //Get file sizes for bitrate calculation
        if(puiHWBitrate != NULL && puiSWBitrate != NULL){
            hwFile = _wfopen(m_strHWOutputFile,L"rb");
            if(hwFile != NULL){
                fseek(hwFile,0,SEEK_END);
                llHWBits = ftell(hwFile) * 8;
                fclose(hwFile);
                hwFile = NULL;
            }else{
                _HRL(E_FAIL,InternalLog(L"Failed to open HW file for file size calculation\n"));
            }
            swFile = _wfopen(m_strSWOutputFile,L"rb");
            if(swFile != NULL){
                fseek(swFile,0,SEEK_END);
                llSWBits = ftell(swFile) * 8;
                fclose(swFile);
                swFile = NULL;
            }else{
                _HRL(E_FAIL,InternalLog(L"Failed to open SW file for file size calculation\n"));
            }
        }

        //Get source readers for all the files and set htem up
        _HRL(MFCreateSourceReaderFromURL((LPCWSTR)m_strHWOutputFile ,NULL, &hardwareSource),
            InternalLog(L"Fail at CreateSourceReaderFromURL\n"));
        _HRL(MFCreateSourceReaderFromURL((LPCWSTR)m_strSWOutputFile ,NULL, &softwareSource),
            InternalLog(L"Fail at CreateSourceReaderFromURL\n"));
        _HRL(MFCreateAttributes(&pAllowXVP,2),InternalLog(L"Fail at CreateAttributes\n"));
        _HRL(pAllowXVP->SetUINT32(MF_SOURCE_READER_ENABLE_ADVANCED_VIDEO_PROCESSING,TRUE),
            InternalLog(L"Fail at SetUINT32\n"));
        _HRL(MFCreateSourceReaderFromURL((LPCWSTR)strInputFile ,pAllowXVP, &originalSource),
            InternalLog(L"Fail at CreateSourceReaderFromURL\n"));
        _HRL(originalSource->GetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, &currentType),InternalLog(L"Fail at GetCurrentMediaType\n"));
        _HRL(MFGetAttributeSize(currentType, MF_MT_FRAME_SIZE,&dwWidth, &dwHeight),InternalLog(L"Fail at MFGetAttributeSize"));

        InternalLog(L"Width: %d, Height %d\n",dwWidth,dwHeight);

        //We have the type, width and height, original file name, and HW and SW file names
        _HRL(currentType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_NV12),InternalLog(L"SetGUID to NV12 failed\n"));
        _HRL(hardwareSource->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, currentType),
            InternalLog(L"SetCurrentMediaType failed with code %x\n",hr));
        _HRL(softwareSource->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, currentType),
            InternalLog(L"SetCurrentMediaType failed with code %x\n",hr));
        _HRL(originalSource->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, currentType),
            InternalLog(L"SetCurrentMediaType for Original Source failed with code %x\n",hr));

        if(dwHeight % 16 != 0){
            bCropNeeded = TRUE;
        }

        dxCompare.EnableSingleFrame(dwWidth,dwWidth,dwHeight,MFVideoFormat_NV12,TRUE,FALSE);
        dxCompare2.EnableSingleFrame(dwWidth,dwWidth,dwHeight,MFVideoFormat_NV12,TRUE,FALSE);

        do{
            _HRL(hardwareSource->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM,NULL,&streamIndex,&streamFlags,&timeStamp,&pHWSample),
                InternalLog(L"Failed to read sample from pass 1 file with hr %x",hr));
            _HRL(softwareSource->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM,NULL,&streamIndex,&streamFlags,&timeStamp,&pSWSample),
                InternalLog(L"Failed to read sample from pass 2 file with hr %x",hr));
            _HRL(originalSource->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM,NULL,&streamIndex,&streamFlags,&timeStamp,&pOrigSample),
                InternalLog(L"Failed to read sample from source file with hr %x",hr));
            if(pHWSample != NULL && pSWSample != NULL && pOrigSample != NULL){
                _HRL(pHWSample->GetSampleDuration(&llSampleDuration),InternalLog(L"Failed beacuse sample duration not set on output sample"));
                llTotalDuration += llSampleDuration;
                _HRL(pHWSample->ConvertToContiguousBuffer(&pHWMediaBuffer),InternalLog(L"Failed to get buffer from pass 1 sample with hr = %x",hr));
                _HRL(pSWSample->ConvertToContiguousBuffer(&pSWMediaBuffer),InternalLog(L"Failed to get buffer from pass 2 sample with hr = %x",hr));
                _HRL(pOrigSample->ConvertToContiguousBuffer(&pOrigMediaBuffer),InternalLog(L"Failed to get buffer from OR sample with hr = %x",hr));

                _HRL(pHWMediaBuffer->Lock(&pHWBuffer,NULL,&dwHWBufSize),InternalLog(L"Failed to unlock pass 1 buffer with hr = %x",hr));
                _HRL(pSWMediaBuffer->Lock(&pSWBuffer,NULL,&dwSWBufSize),InternalLog(L"Failed to unlock pass 2 buffer with hr = %x",hr));
                _HRL(pOrigMediaBuffer->Lock(&pOrigBuffer,NULL,&dwOrigBufSize),InternalLog(L"Failed to unlock OR buffer with hr = %x",hr));

                if(bCropNeeded){
                    pHWCroppedBuffer = (BYTE*)calloc(dwHeight*dwWidth*3/2,1);
                    pSWCroppedBuffer = (BYTE*)calloc(dwHeight*dwWidth*3/2,1);
                    if(pHWCroppedBuffer == NULL || pSWCroppedBuffer == NULL){
                        _HRL(E_OUTOFMEMORY,InternalLog(L"ERROR: Out of memory to create PSNR comparison buffers"));
                    }
                    memcpy(pHWCroppedBuffer,pHWBuffer,dwHeight*dwWidth);
                    memcpy(pHWCroppedBuffer+(dwWidth*dwHeight),pHWBuffer+(dwHeight+8)*dwWidth,dwHeight*dwWidth/2);
                    memcpy(pSWCroppedBuffer,pSWBuffer,dwHeight*dwWidth);
                    memcpy(pSWCroppedBuffer+(dwWidth*dwHeight),pSWBuffer+(dwHeight+8)*dwWidth,dwHeight*dwWidth/2);

                    _HRL(dxCompare.ProcessSingleFrame(pHWCroppedBuffer,pOrigBuffer),InternalLog(L"Failed PSNR calculation with hr = %x",hr));
                    _HRL(dxCompare2.ProcessSingleFrame(pSWCroppedBuffer,pOrigBuffer),InternalLog(L"Failed PSNR calculation with hr = %x",hr));

                    SAFEFREE(pHWCroppedBuffer);
                    SAFEFREE(pSWCroppedBuffer);
                }else{
                    _HRL(dxCompare.ProcessSingleFrame(pHWBuffer,pOrigBuffer),InternalLog(L"Failed PSNR calculation with hr = %x",hr));
                    _HRL(dxCompare2.ProcessSingleFrame(pSWBuffer,pOrigBuffer),InternalLog(L"Failed PSNR calculation with hr = %x",hr));
                }

                pHWMediaBuffer->Unlock();
                pSWMediaBuffer->Unlock();
                pOrigMediaBuffer->Unlock();
            }

            SAFEFREE(pHWCroppedBuffer);
            SAFEFREE(pSWCroppedBuffer);

            SAFE_RELEASE(pHWMediaBuffer);
            SAFE_RELEASE(pSWMediaBuffer);
            SAFE_RELEASE(pOrigMediaBuffer);

            SAFE_RELEASE(pHWSample);
            SAFE_RELEASE(pSWSample);
            SAFE_RELEASE(pOrigSample);

        }while((streamFlags & MF_SOURCE_READERF_ENDOFSTREAM) == 0);
        _HR(hr);


        double HWY, HWU, HWV, SWY, SWU, SWV;
        dxCompare.GetSingleFrameAccumulatedPSNR(&HWY, &HWU, &HWV);
        dxCompare2.GetSingleFrameAccumulatedPSNR(&SWY, &SWU, &SWV);

        InternalLog(L"Returned PSNR Values: File1 (HW): %f \t File2 (SW): %f\n",HWY,SWY);
        //Validate the PSNR values here

        if(bValidate){
            double lowerBound = SWY*((double)1.0-((double)ALLOWED_PSNR_LOWER_PERCENT_BOUND/(double)100));

            InternalLog(L"Bound: %f\n",lowerBound);

            if(HWY < lowerBound){
                _HRL(E_FAIL,InternalLog(L"Failed PSNR validation\n"));
            }

        }

        if(puiHWBitrate != NULL && puiSWBitrate != NULL){
            *puiHWBitrate = (UINT32)((llHWBits * HUNDREDS_NANO_PER_SEC)/(llTotalDuration));
            *puiSWBitrate = (UINT32)((llSWBits * HUNDREDS_NANO_PER_SEC)/(llTotalDuration));
        }

        if(pdHWPSNR != NULL && pdSWPSNR != NULL){
            *pdHWPSNR = HWY;
            *pdSWPSNR = SWY;
        }


    }while(false);

    if(hwFile != NULL){
        fclose(hwFile);
    }
    if(swFile != NULL){
        fclose(swFile);
    }


    SAFEFREE(pHWCroppedBuffer);
    SAFEFREE(pSWCroppedBuffer);

    SAFE_RELEASE(pHWMediaBuffer);
    SAFE_RELEASE(pSWMediaBuffer);
    SAFE_RELEASE(pOrigMediaBuffer);

    SAFE_RELEASE(pHWSample);
    SAFE_RELEASE(pSWSample);
    SAFE_RELEASE(pOrigSample);

    SAFE_RELEASE(currentType);
    SAFE_RELEASE(hardwareSource);
    SAFE_RELEASE(softwareSource);
    SAFE_RELEASE(originalSource);
    SAFE_RELEASE(pAllowXVP);

    return hr;

}


STDMETHODIMP
    CH264EncodeVerify::H264CheckROIPSNR()
{
    HRESULT hr = S_OK;
    HRESULT hr2 = S_OK;

    IMFSourceReader* pRoiSource = NULL;
    IMFSourceReader* pNormalSource = NULL;
    IMFSourceReader* pOriginalSource = NULL;

    IMFAttributes *pAllowXVP = NULL;

    IMFMediaType *currentType = NULL;
    UINT32 dwHeight = 0, dwWidth = 0;
    UINT32 numFrames = 0;


    IMFSample *pROISample = NULL, *pNormSample = NULL, *pOrigSample = NULL;
    DWORD streamIndex = 0;
    DWORD streamFlags = 0;
    LONGLONG timeStamp = 0;

    IMFMediaBuffer *pROIMediaBuffer = NULL, *pNormMediaBuffer = NULL, *pORMediaBuffer = NULL;

    BYTE* bBuffer = NULL;
    DWORD bufSize  = 0;

    double roiYPSNR  = 0, roiUPSNR = 0, roiVPSNR = 0;
    double normYPSNR = 0,normUPSNR = 0, normVPSNR = 0;

    RECT rect;
    INT32 QPDelta = 0;

    BYTE* pROIBuf = NULL;
    BYTE* pNormBuf = NULL;
    BYTE* pORBuf = NULL;

    BYTE* pPreCropNormBuf = NULL;
    BYTE* pPreCropROIBuf  = NULL;

    BYTE* pRoiCropBuf = NULL;
    BYTE* pNormalCropBuf = NULL;
    BYTE* pOrigCropBuf = NULL;

    DWORD dwROILength = 0;
    DWORD dwNormLength = 0;
    DWORD dwORLength = 0; 

    BOOL bInitialCropped = FALSE;

    WEX::Common::String strInputFile = m_pConfigHelper->GetInputPath();

    do{
        _HR(m_pConfigHelper->GetConfigCustomParser((ITAEFConfigCustomParser **)&m_pH264CustomParser));

        _HRL(MFCreateSourceReaderFromURL((LPCWSTR)m_strHWOutputFile ,NULL, &pRoiSource),InternalLog(L"Fail at CreateSourceReaderFromURL with hr = %x",hr));
        _HRL(MFCreateSourceReaderFromURL((LPCWSTR)m_strSWOutputFile ,NULL, &pNormalSource),InternalLog(L"Fail at CreateSourceReaderFromURL with hr = %x",hr));
        _HRL(MFCreateAttributes(&pAllowXVP,2),InternalLog(L"Fail at CreateAttributes with hr = %x",hr));
        _HRL(pAllowXVP->SetUINT32(MF_SOURCE_READER_ENABLE_ADVANCED_VIDEO_PROCESSING,TRUE),InternalLog(L"Fail at SetUINT32 with hr = %x",hr));

        _HRL(MFCreateSourceReaderFromURL((LPCWSTR)strInputFile ,pAllowXVP, &pOriginalSource),InternalLog(L"Fail at CreateSourceReaderFromURL with hr = %x",hr));
        _HRL(pOriginalSource->GetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, &currentType),InternalLog(L"Fail at GetCurrentMediaType with hr = %x",hr));
        _HRL(MFGetAttributeSize(currentType, MF_MT_FRAME_SIZE,&dwWidth, &dwHeight),InternalLog(L"Fail at MFGetAttributeSize with hr = %x",hr));

        // We have the type, width and height, original file name, and ROI and Norm file names
        _HRL(currentType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_NV12),InternalLog(L"SetGUID to NV12 failed  with hr = %x",hr));
        _HRL(pRoiSource->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, currentType),
            InternalLog(L"SetCurrentMediaType failed with code %x",hr));

        _HRL(pNormalSource->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, currentType),
            InternalLog(L"SetCurrentMediaType failed with code %x",hr));

        _HRL(pOriginalSource->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, currentType),
            InternalLog(L"SetCurrentMediaType failed with code %x",hr));


        numFrames = 0;
        do{
            _HRL(pRoiSource->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM,NULL,&streamIndex,&streamFlags,&timeStamp,&pROISample),
                InternalLog(L"Failed to read ROI sample from file with hr %x",hr));
            _HRL(pNormalSource->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM,NULL,&streamIndex,&streamFlags,&timeStamp,&pNormSample),
                InternalLog(L"Failed to read ROI sample from file with hr %x",hr));
            _HRL(pOriginalSource->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM,NULL,&streamIndex,&streamFlags,&timeStamp,&pOrigSample),
                InternalLog(L"Failed to read ROI sample from file with hr %x",hr));
            if(pROISample != NULL && pNormSample != NULL && pOrigSample != NULL){

                // Read ROI from config
                m_pH264CustomParser->EnableROI();
                hr2 = m_pH264CustomParser->GetFrameROI(numFrames, &QPDelta, &rect);
                m_pH264CustomParser->DisableROI();

                if(hr2 == S_OK){
                    CDXCompare dxCompare;
                    CDXCompare dxCompare2;
                    dxCompare.EnableSingleFrame(rect.right-rect.left,rect.right-rect.left,rect.bottom-rect.top,MFVideoFormat_NV12,TRUE,FALSE);
                    dxCompare2.EnableSingleFrame(rect.right-rect.left,rect.right-rect.left,rect.bottom-rect.top,MFVideoFormat_NV12,TRUE,FALSE);
                    /* Get and crop buffers*/
                    // Lock and send buffers to DXcompare
                    _HRL(pROISample->ConvertToContiguousBuffer(&pROIMediaBuffer),InternalLog(L"Failed to get buffer from ROI sample with hr = %x",hr));
                    _HRL(pNormSample->ConvertToContiguousBuffer(&pNormMediaBuffer),InternalLog(L"Failed to get buffer from Norm sample with hr = %x",hr));
                    _HRL(pOrigSample->ConvertToContiguousBuffer(&pORMediaBuffer),InternalLog(L"Failed to get buffer from OR sample with hr = %x",hr));

                    _HRL(pROIMediaBuffer->Lock(&pROIBuf,NULL,&dwROILength),InternalLog(L"Failed to unlock ROI buffer with hr = %x",hr));
                    _HRL(pNormMediaBuffer->Lock(&pNormBuf,NULL,&dwNormLength),InternalLog(L"Failed to unlock Norm buffer with hr = %x",hr));
                    _HRL(pORMediaBuffer->Lock(&pORBuf,NULL,&dwORLength),InternalLog(L"Failed to unlock OR buffer with hr = %x",hr));


                    if ((dwNormLength != dwROILength) || (dwNormLength != dwORLength)) {
                        if (dwHeight % 16 == 0){
                            hr = E_FAIL;
                            _HRL(hr, InternalLog(L"ERROR: Sample sizes do not agree: Norm: %d ROI: %d Orig: %d\n",dwNormLength, dwROILength, dwORLength));
                        }else{
                            bInitialCropped = TRUE;
                            pPreCropNormBuf = (BYTE*)calloc(dwHeight*dwWidth*3/2,1);
                            pPreCropROIBuf = (BYTE*)calloc(dwHeight*dwWidth*3/2,1);
                            if(pPreCropNormBuf == NULL || pPreCropROIBuf == NULL){
                                _HRL(E_OUTOFMEMORY,InternalLog(L"ERROR: Out of memory to create PSNR comparison buffers"));
                            }
                            memcpy(pPreCropNormBuf,pNormBuf,dwHeight*dwWidth);
                            memcpy(pPreCropNormBuf+(dwWidth*dwHeight),pNormBuf+(dwHeight+8)*dwWidth,dwHeight*dwWidth/2);
                            memcpy(pPreCropROIBuf,pROIBuf,dwHeight*dwWidth);
                            memcpy(pPreCropROIBuf+(dwWidth*dwHeight),pROIBuf+(dwHeight+8)*dwWidth,dwHeight*dwWidth/2);
                        }
                    }

                    // Compare ROI area only
                    if(bInitialCropped){
                        _HRL(CropROIBuffer(pPreCropROIBuf, dwWidth, dwHeight, rect, &pRoiCropBuf),InternalLog(L"Cropping buffer to ROI area failed with hr = %x",hr));
                        _HRL(CropROIBuffer(pPreCropNormBuf, dwWidth, dwHeight, rect, &pNormalCropBuf),InternalLog(L"Cropping buffer to ROI area failed with hr = %x",hr));
                    }else{
                        _HRL(CropROIBuffer(pROIBuf, dwWidth, dwHeight, rect, &pRoiCropBuf),InternalLog(L"Cropping buffer to ROI area failed with hr = %x",hr));
                        _HRL(CropROIBuffer(pNormBuf, dwWidth, dwHeight, rect, &pNormalCropBuf),InternalLog(L"Cropping buffer to ROI area failed with hr = %x",hr));
                    }
                    _HRL(CropROIBuffer(pORBuf, dwWidth, dwHeight, rect, &pOrigCropBuf),InternalLog(L"Cropping buffer to ROI area failed with hr = %x",hr));

                    _HRL(dxCompare.ProcessSingleFrame(pRoiCropBuf,pOrigCropBuf,&roiYPSNR,&roiUPSNR,&roiVPSNR),InternalLog(L"Failed PSNR calculation with hr = %x",hr));
                    _HRL(dxCompare2.ProcessSingleFrame(pNormalCropBuf,pOrigCropBuf,&normYPSNR,&normUPSNR,&normVPSNR),InternalLog(L"Failed PSNR calculation with hr = %x",hr));


                    //The majority of frame quality is going to come from the Y plane. So we will only check the Y-plane for ROI
                    if(QPDelta < 0){
                        if(roiYPSNR <= normYPSNR /*|| roiUPSNR <= normUPSNR || roiVPSNR <= normVPSNR*/){
                            hr = E_FAIL;
                        }
                    }else if(QPDelta == 0){
                        /*We'll allow a little leeway for randomness */
                        if(roiYPSNR > normYPSNR + ROI_ZERO_LEEWAY || roiYPSNR < normYPSNR - ROI_ZERO_LEEWAY
                            /*  || roiUPSNR > normUPSNR + ROI_ZERO_LEEWAY || roiUPSNR < normUPSNR - ROI_ZERO_LEEWAY
                            || roiVPSNR > normVPSNR + ROI_ZERO_LEEWAY || roiVPSNR < normVPSNR - ROI_ZERO_LEEWAY */)
                        {
                            hr = E_FAIL;
                        }
                    }else{
                        if(roiYPSNR >= normYPSNR /*|| roiUPSNR >= normUPSNR || roiVPSNR >= normVPSNR*/){
                            hr = E_FAIL;
                        }
                    }
                    if(FAILED(hr)){
                        InternalLog(L"Frame: %d Delta QP: %d ROI YPSNR: %f, Normal YPSNR: %f",numFrames,QPDelta,roiYPSNR,normYPSNR);
                        InternalLog(L"ROI UPSNR: %f, Normal UPSNR: %f, ROI VPSNR: %f, Normal VPSNR: %f",roiUPSNR,normUPSNR,roiVPSNR,normVPSNR);
                    }
                    _HRL(hr,InternalLog(L"Failed ROI PSNR test"));


                    if(bInitialCropped){
                        SAFEFREE(pPreCropNormBuf);
                        SAFEFREE(pPreCropROIBuf);
                    }
                    pPreCropNormBuf = NULL;
                    pPreCropROIBuf = NULL;

                    pROIMediaBuffer->Unlock();
                    pNormMediaBuffer->Unlock();
                    pORMediaBuffer->Unlock();


                    SAFE_RELEASE(pROIMediaBuffer);
                    SAFE_RELEASE(pNormMediaBuffer);
                    SAFE_RELEASE(pORMediaBuffer);
                }

                numFrames++;

                SAFE_RELEASE(pROISample);
                SAFE_RELEASE(pNormSample);
                SAFE_RELEASE(pOrigSample);
            }

            if(pROISample == NULL || pNormSample == NULL || pOrigSample == NULL){
                if(!(pROISample == NULL && pNormSample == NULL && pOrigSample == NULL)){
                    hr = E_FAIL;
                    InternalLog(L"Read one file to completion before other files finished in ROI PSNR checks");
                    break;
                }
            }

        }while((streamFlags & MF_SOURCE_READERF_ENDOFSTREAM) == 0);
        if(FAILED(hr)){
            break;
        }

    }while(false);

    if(bInitialCropped){
        SAFEFREE(pPreCropNormBuf);
        SAFEFREE(pPreCropROIBuf);
    }

    SAFE_RELEASE(pROIMediaBuffer);
    SAFE_RELEASE(pNormMediaBuffer);
    SAFE_RELEASE(pORMediaBuffer);

    SAFE_RELEASE(pROISample);
    SAFE_RELEASE(pNormSample);
    SAFE_RELEASE(pOrigSample);

    SAFE_RELEASE(currentType);
    SAFE_RELEASE(pRoiSource);
    SAFE_RELEASE(pNormalSource);
    SAFE_RELEASE(pAllowXVP);
    SAFE_RELEASE(pOriginalSource);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::CropROIBuffer(
    BYTE* pBufferOrig,
    UINT32 dwWidth, 
    UINT32 dwHeight,
    RECT ROI,
    BYTE** ppCroppedBuffer
    )
{
    HRESULT hr = S_OK;

    if (pBufferOrig == NULL || ppCroppedBuffer == NULL)
    {
        return E_POINTER;
    }

    if ((ROI.left >= ROI.right) || 
        (ROI.top >= ROI.bottom) ||
        (ROI.right > dwWidth) ||
        (ROI.bottom > dwHeight))
    {
        return E_UNEXPECTED;
    }

    unsigned char* pY = pBufferOrig;
    unsigned char* pUV = pBufferOrig + dwWidth*dwHeight;

    // Be sure we use even numbers
    // We did this also when we set it
    ROI.left &= ~1;
    ROI.top &= ~1;
    ROI.right &= ~1;
    ROI.bottom &= ~1;

    *ppCroppedBuffer = (BYTE*)calloc((ROI.right - ROI.left) * (ROI.bottom - ROI.top) * 3 / 2, sizeof(BYTE));
    if(*ppCroppedBuffer == NULL){
        return E_OUTOFMEMORY;
    } 
    unsigned char* pROI = *ppCroppedBuffer;

    for (int i = ROI.top; i < ROI.bottom; i++)
    {
        memcpy(pROI, pY + dwWidth * i + ROI.left, ROI.right - ROI.left);
        pROI += (ROI.right - ROI.left);
    }
    for (int i = ROI.top; i < ROI.bottom; i+=2)
    {
        memcpy(pROI, pUV + dwWidth * i / 2 + ROI.left, ROI.right - ROI.left);
        pROI += (ROI.right - ROI.left);
    }


    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::H264BitStreamVerify()
{
    HRESULT hr = S_OK;

    // Values from H.264 encoded file
    ULONG ulGOPSize = 0;
    ULONG ulGOPBufferSize = 0;
    ULONG ulBFramesMin = 0;
    ULONG ulBFramesMax = 0;
    ULONG ulFrameCount = 0;
    ULONG ulFrameSize = 0;
    ULONG ulTemporalLayer = 0;
    ULONG ulTotalFrameIndex = 0;
    ULONGLONG ullGOPSizeTime = 0;
    ULONGLONG ullFrameTimeStamp = 0;
    ULONGLONG ullFrameDecodeTimeStamp = 0;
    LONG lFrameQPNumber = 0;
    LONG lFrameQPNumberMin = LONG_MAX;
    LONG lFrameQPNumberMax = 0;

    do
    {
        /////////////////////////
        // Create helper classes
        /////////////////////////
        _HR((m_pConfigHelper) ? S_OK : E_UNEXPECTED);

        m_pH264EncVerifier = new CH264EncVerifier();
        if (NULL == m_pH264EncVerifier)
        {
            _HR(hr = E_OUTOFMEMORY);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        // Read configuration to member variables, parse bitstream and read actual values from encoded file
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        InternalLog(L"\nVerifying the encoded file %s against properties in the test xml:", (LPCWSTR)m_strOutputFile);

        _HRL(H264BitStreamVerifyGetConfigValues(), InternalLog(L"Failed to GetConfigValues\n"));
        _HRL(H264BitStreamVerifySetUpVerifier(), InternalLog(L"Failed to SetUpVerifier\n"));
        _HRL(H264BitStreamVerifyParseBitStream(), InternalLog(L"Failed to H264BitStreamVerifyParseBitStream\n"));
        _HRL(H264BitStreamVerifyReadActualValuesFromEncodedFile(), InternalLog(L"Failed to H264BitStreamVerifyReadActualValuesFromEncodedFile\n"));

        //////////////////////////////////////
        // Run checks on file-level properties
        //////////////////////////////////////
        _HRL(H264BitStreamVerifyCheckH264Profile(), InternalLog(L"Failed to H264BitStreamVerifyCheckH264Profile\n"));
        _HRL(H264BitStreamVerifyCheckConstrainFlag(), InternalLog(L"Failed to H264BitStreamVerifyCheckConstrainFlag\n"));
        _HRL(H264BitStreamVerifyCheckLevel(), InternalLog(L"Failed to H264BitStreamVerifyCheckLevel\n"));
        _HRL(H264BitStreamVerifyCheckSequenceParameter(), InternalLog(L"Failed to H264BitStreamVerifyCheckSequenceParamater\n"));
        _HRL(H264BitStreamVerifyCheckPPSID(), InternalLog(L"Failed to H264BitStreamVerifyCheckPPSID\n"));
        _HRL(H264BitStreamVerifyCheckEntropyCoding(), InternalLog(L"Failed to H264BitStreamVerifyCheckEntropyCoding\n"));
        _HRL(H264BitStreamVerifyCheckMaxNumRefFrames(), InternalLog(L"Failed to H264BitStreamVerifyCheckMaxNumRefFrames\n"));
        _HR(H264BitStreamVerifyLogFileDuration());
        _HRL(H264BitStreamVerifyCheckBitrate(), InternalLog(L"Failed to H264BitStreamVerifyCheckBitrate\n"));
        _HRL(H264BitStreamVerifyCheckTemporalLayers(), InternalLog(L"Failed to H264BitStreamVerifyCheckTemporalLayers\n"));

        InternalLog(L"");

        ////////////////////
        // Loop through GOPs
        ////////////////////
        for (ULONG ulGOPIndex = 0; ulGOPIndex < m_ulEncodedGOPCount; ulGOPIndex++)
        {
            /////////////////////////////////////////
            // Read GOP properties to local variables
            /////////////////////////////////////////
            _HR(m_pH264EncVerifier->GetGOPDistanceAt(ulGOPIndex, &ulGOPSize));
            _HR(m_pH264EncVerifier->GetGOPBufferSizeAt(ulGOPIndex, &ulGOPBufferSize));
            _HR(m_pH264EncVerifier->GetGOPDistanceTimeAt(ulGOPIndex, &ullGOPSizeTime));
            _HR(m_pH264EncVerifier->GetGOPBFrameCountAt(ulGOPIndex, &ulBFramesMin, &ulBFramesMax));
            _HR(m_pH264EncVerifier->GetFrameCountAt(ulGOPIndex, &ulFrameCount));

            _HR(H264BitStreamVerifyLogGOPProperties(ulGOPIndex, ulGOPSize, ullGOPSizeTime, ulGOPBufferSize, ulBFramesMin, ulBFramesMax));

            //////////////////////////////////////
            // Run checks on GOP-level properties
            /////////////////////////////////////
            _HRL(H264BitStreamVerifyCheckBFrames(ulBFramesMin, ulBFramesMax, ulGOPIndex, ulGOPSize), InternalLog(L"Failed to H264BitStreamVerifyCheckBFrames\n"));
            _HRL(H264BitStreamVerifyCheckGOPSize(ulGOPIndex, ulGOPSize), InternalLog(L"Failed to H264BitStreamVerifyCheckGOPSize\n"));

            //////////////////////
            // Loop through frames
            //////////////////////
            for (ULONG ulFrameIndexInGOP = 0; ulFrameIndexInGOP < ulFrameCount; ulFrameIndexInGOP++)
            {
                ULONG frameType = FrameType_None;

                _HR(m_pH264EncVerifier->GetFrameSizeAt(ulGOPIndex, ulFrameIndexInGOP, &ulFrameSize));
                _HR(m_pH264EncVerifier->GetFrameTypeAt(ulGOPIndex, ulFrameIndexInGOP, &frameType));
                _HR(m_pH264EncVerifier->GetFrameTimeStampAt(ulGOPIndex, ulFrameIndexInGOP, &ullFrameTimeStamp));
                _HR(m_pH264EncVerifier->GetFrameDecodeTimeStampAt(ulGOPIndex, ulFrameIndexInGOP, &ullFrameDecodeTimeStamp));
                _HR(m_pH264EncVerifier->GetQPNumberAt(ulGOPIndex, ulFrameIndexInGOP, &lFrameQPNumber));				
                _HR(m_pH264EncVerifier->GetTemporalLayerAt(ulGOPIndex, ulFrameIndexInGOP, &ulTemporalLayer));				

                _HR(H264BitStreamVerifyLogFrameProperties(ulGOPIndex, ulFrameIndexInGOP, ulTemporalLayer));

                _HR(H264BitStreamVerifyCheckQPRange(lFrameQPNumber, lFrameQPNumberMin, lFrameQPNumberMax));
                _HR(H264BitStreamVerifyCheckFrameQP(lFrameQPNumber, ulTotalFrameIndex, frameType));
                _HR(H264BitStreamVerifyCheckTemporalLayers(ulTotalFrameIndex, ulTemporalLayer, ulFrameIndexInGOP, ulGOPIndex));
                _HR(H264BitStreamVerifyCheckFrameLayerQP(ulGOPIndex, ulGOPSize, ulFrameIndexInGOP, lFrameQPNumber, frameType));
                _HR(H264BitStreamVerifyCheckSliceControl(ulGOPIndex, ulFrameIndexInGOP, ulTotalFrameIndex, ulGOPSize));

                ulTotalFrameIndex++;
            }

            if (S_OK != hr)
            {
                break;
            }
        }

        _HR(H264BitStreamVerifyCheckExtractAndVerifyLayers());
        _HR(H264BitStreamVerifyLogFrameQPMinMax(lFrameQPNumberMin, lFrameQPNumberMax));

        if (S_OK != hr)
        {
            break;
        }

        H264BitStreamVerifyCheckPSNR();

    } while (FALSE);
    SAFE_DELETE(m_pH264EncVerifier);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::H264BitStreamVerifyGetConfigValues()
{
    HRESULT hr = S_OK;
    UINT32 uiInputFrameCount = 0;
    UINT32 ui32TemporalLayersConfig = 0;
    WEX::Common::String strProfileConfig;
    WEX::Common::String strLevelConfig;
    MFNominalRange eNominalRange;
    eAVEncH264VProfile h264Profile = eAVEncH264VProfile_unknown;

    do
    {
        ////////////////////////////////////////////////////
        // Read values from config file to member variables
        ////////////////////////////////////////////////////

        InternalLog(L"\nConfiguration settings:");

        GetInputFrameCount(&uiInputFrameCount);
        m_pConfigHelper->GetNominalRange(&eNominalRange);
        m_bIsLowLatency = m_pConfigHelper->IsLowLatency();
        m_uiTestConfigUsed = m_pConfigHelper->GetTestConfigUsed();
        m_hwMode = m_pConfigHelper->GetHWMode();
        m_bIsLTRModeSet = m_pH264CustomParser->IsLTRModeSet();
        m_bIsSetTemporalLayers =
            (m_pH264CustomParser->IsDynamicTemporalSet() ||
            (S_OK == m_pH264CustomParser->GetStaticTemporalLayerCount(&ui32TemporalLayersConfig)));
        m_bIsFrameCheckDisabled = m_pH264CustomParser->IsBFrameCheckDisabled();

        // Read profile from config file
        strProfileConfig = m_pConfigHelper->GetEncodingProfile();
        CH264EncHelper::ConvertEncodingProfile(strProfileConfig, &h264Profile);
        m_ulProfileConfig = (UINT32)h264Profile;
        strProfileConfig.ReleaseBuffer();

        // Read level from config file
        strLevelConfig = m_pConfigHelper->GetEncodingLevel();
        CH264EncHelper::ConvertEncodingLevel(strLevelConfig, &m_ulLevelConfig);
        strLevelConfig.ReleaseBuffer();

        // Read B-Frames from config file
        _HR(m_pConfigHelper->GetBFrames(&m_ulBFrameCountConfig));
        if(m_ulBFrameCountConfig == -1)
        {
            BOOL bLowLatency = FALSE;
            bLowLatency = m_pConfigHelper->IsLowLatency();
            m_ulBFrameCountConfig =  bLowLatency ? 0 : 1;
        }

        // Read bitrate from config file
        hr = m_pConfigHelper->GetBitrate(&m_ulBitrateConfig);
        if (S_OK != hr)
        {
            InternalLog(L"Failed ITAEFConfigHelper::GetBitrate() with hr = 0x%08X", hr);
            break;
        }

        // Read bitrate control mode from config file
        m_BitRateControlModeConfig = (eAVEncCommonRateControlMode)m_pConfigHelper->GetBitrateControlMode();

        // Read quality level from config file
        if (eAVEncCommonRateControlMode_Quality == m_BitRateControlModeConfig)
        {
            _HR(m_pConfigHelper->GetQualityLevel(&m_ulQualityConfig));
            m_ulQPConfig = m_pH264EncVerifier->CalculateQPFromQuality(m_ulQualityConfig);
        }

        m_bIsEncodeQPConfigSet = m_pH264CustomParser->IsEncodeQPSet();
        if(m_bIsEncodeQPConfigSet){
            UINT64 ui64EncodeQP;
            m_pH264CustomParser->GetEncodeQP(&ui64EncodeQP);
            m_ulQPConfig = (ui64EncodeQP & 0x000000000000FFFF);
        }

        // Read buffer size from config file, it may bs missing
        hr = m_pConfigHelper->GetBufferSize(&m_ulBufferSizeConfig);

        // Read mean bitrate from config file, it may be missing
        hr = m_pH264CustomParser->GetMeanBitRate(&m_ulMeanBitRateConfig);

        // Read max bitrate from config file, it may be missing
        hr = m_pH264CustomParser->GetMaxBitRate(&m_ulMaxBitrateConfig);

        //Get the Frame rate Numerator and Denominator set on the transform
        hr = m_pH264CustomParser->GetFrameRate(&m_ulFRNumerator,&m_ulFRDenominator);
        if(S_OK != hr)
        {
            m_bIsFrameRateSet = FALSE;
        }

        m_bIsDynamicLayerConfigSet = m_pH264CustomParser->IsDynamicTemporalSet();
        m_bIsFrameTypeQPConfigSet = m_pH264CustomParser->IsFrameTypeQPSet();

        // Read GOP size from config file
        hr = m_pConfigHelper->GetGOPSize(&m_ulGOPSizeConfig);
        if(hr == MF_E_NOT_FOUND){
            m_ulGOPSizeConfig = 30;
            hr = S_OK;
        }else if(FAILED(hr)){
            break;
        }

        _HRL(m_pH264CustomParser->CalculateExpectedGOPSizes(m_uiInputFrameCount, m_ulGOPSizeConfig, 0, FALSE),
            InternalLog(L"Expected GOP size and Temporal layer calculation failed\n"));

        // Read quality-vs-speed from config file
        _HR(m_pConfigHelper->GetQualityVsSpeed(&m_ulQualityVsSpeedConfig));

        // Read entropy coding mode from config file
        _HR(m_pH264CustomParser->GetCABACEnable(&m_fCABACEnableConfig));
        m_ulEntropyCodingConfig = (UINT32)m_fCABACEnableConfig;

        // Read delta quantization from config file
        //        _HR(m_pH264CustomParser->GetDeltaQuant(&m_ulDeltaQuantConfig));

        // Read content type from config file 
        //        _HR(m_pH264CustomParser->GetContentType(&m_ulContentTypeConfig));

        InternalLog(L"    Profile: %lu", m_ulProfileConfig);
        InternalLog(L"    Level: %lu", m_ulLevelConfig);
        InternalLog(L"    B-Frames: %lu", m_ulBFrameCountConfig);
        if (eAVEncCommonRateControlMode_CBR == m_BitRateControlModeConfig)
        {
            InternalLog(L"    Bitrate control mode: %s", L"CBR");
        }
        else if (eAVEncCommonRateControlMode_PeakConstrainedVBR == m_BitRateControlModeConfig)
        {
            InternalLog(L"    Bitrate control mode: %s", L"Peak constrained VBR");
        }
        else if (eAVEncCommonRateControlMode_UnconstrainedVBR == m_BitRateControlModeConfig)
        {
            InternalLog(L"    Bitrate control mode: %s", L"Unconstrained VBR");
        }
        else if (eAVEncCommonRateControlMode_Quality == m_BitRateControlModeConfig)
        {
            InternalLog(L"    Bitrate control mode: %s", L"Quality");
        }
        else
        {
            InternalLog(L"    Unknown control mode");
        }

        InternalLog(L"    Quality: %lu (QP: %lu)", m_ulQualityConfig, m_ulQPConfig);
        InternalLog(L"    Buffer size: %lu kbytes", m_ulBufferSizeConfig/1000);
        InternalLog(L"    GOP Size: %lu frames", m_ulGOPSizeConfig);
        InternalLog(L"    Quality-vs-speed: %lu", m_ulQualityVsSpeedConfig);
        InternalLog(L"    Bitrate: %lu kbits/sec", m_ulBitrateConfig/1000);
        InternalLog(L"    Mean bitrate: %lu kbits/sec", m_ulMeanBitRateConfig/1000);
        if (0 != m_ulMaxBitrateConfig)
        {
            InternalLog(L"    Max bitrate: %lu kbits/sec", m_ulMaxBitrateConfig/1000);
        }
        InternalLog(L"    Entropy coding: %s", m_ulEntropyCodingConfig == 0 ? L"CAVLC" : L"CABAC");

        // Read SPS ID from config file
        if (S_OK == m_pH264CustomParser->GetSPSID(&m_ulSPSIDConfig))
        {
            m_bIsSPSIDConfigSet = TRUE;
            InternalLog(L"    SPS ID: %lu", m_ulSPSIDConfig);
        }

        // Read PPS ID from config file
        if (S_OK == m_pH264CustomParser->GetPPSID(&m_ulPPSIDConfig))
        {
            m_bIsPPSIDConfigSet = TRUE;
            InternalLog(L"    PPS ID: %lu", m_ulPPSIDConfig);
        }

        // Read Min QP from config file
        if (S_OK == m_pH264CustomParser->GetMinQP(&m_ulMinQPConfig))
        {
            m_bIsMinQPConfigSet = TRUE;
            InternalLog(L"    Min QP: %lu", m_ulMinQPConfig);
        }

        // Read Max QP from config file
        if (S_OK == m_pH264CustomParser->GetMaxQP(&m_ulMaxQPConfig))
        {
            m_bIsMaxQPConfigSet = TRUE;
            InternalLog(L"    Max QP: %lu", m_ulMaxQPConfig);
        }

        // Read Frame QP from config file
        if (S_OK == m_pConfigHelper->GetFrameLayerQP(0, &m_ullFrameQPConfig))
        {
            m_bIsFrameQPConfigSet = TRUE;
            InternalLog(L"    Frame QP: %llu", m_ullFrameQPConfig);
        }

        // Read video temporal layer count from config file
        if (S_OK == m_pH264CustomParser->GetStaticTemporalLayerCount(&m_ulLayerCountConfig))
        {
            m_bIsLayerCountConfigSet = TRUE;
            InternalLog(L"    Video temporal layer count %lu", m_ulLayerCountConfig);
        }

        // Read frame  temporal layer count from config file
        ULONGLONG ullFrameLayerQPConfig = 0;

        if (S_OK == m_pConfigHelper->GetFrameLayerQP(0, &ullFrameLayerQPConfig))
        {
            m_bIsFrameLayerQPConfigSet = TRUE;
            InternalLog(L"    Frame layer QP is set");
        }

        // slice control
        if (S_OK == m_pH264CustomParser->GetSliceControlMode(&m_ulSliceControlMode))
        {
            m_bIsSliceControlModeSet = TRUE;
            InternalLog(L"    Slice control mode is set to %lu", m_ulSliceControlMode);
        }        

        if (S_OK == m_pH264CustomParser->GetSliceControlSize(&m_ulSliceControlSize))
        {
            m_bIsSliceControlSizeSet = TRUE;
            InternalLog(L"    Slice control size is set to %lu", m_ulSliceControlSize);
        }        

        if (S_OK == m_pH264CustomParser->GetIgnoredSliceControlMode(&m_ulIgnoredSliceControlMode))
        {
            m_bIsIgnoredSliceControlModeSet = TRUE;
            InternalLog(L"    Ignored slice control mode is set to %lu", m_ulIgnoredSliceControlMode);
        }        
        if (S_OK == m_pH264CustomParser->GetDynamicInitialSliceControlSize(&m_ulDynamicInitialSliceControlSize))
        {
            m_bIsDynamicInitialSliceControlSizeSet = TRUE;
            InternalLog(L"    Dynamic initial slice control size is set to %lu", m_ulDynamicInitialSliceControlSize);
        }        
        if (S_OK == m_pH264CustomParser->GetMaxNumRefFrames(&m_uiMaxNumRefFrame)){
            m_bIsMaxNumRefFrameSet = TRUE;
            InternalLog(L"    Maximum number of reference frames: %lu", m_uiMaxNumRefFrame);
        }
        if (S_OK == m_pH264CustomParser->GetScenarioInfo(&m_uiScenarioInfo)){  
            m_bScenarioInfoSet = TRUE;
            InternalLog(L"    Scenario Info: %lu", m_uiScenarioInfo);
        }
        if (S_OK == m_pH264CustomParser->GetEnableLowPower(&m_bEnableLowPower)){  
            m_bEnableLowPowerSet = TRUE;
            InternalLog(L"    Low Power Mode Enabled: %lu", m_bEnableLowPower);
        }
        if (S_OK == m_pH264CustomParser->GetIntraRefreshInfo(&m_uiIntraRefreshMode, &m_uiIntraRefreshFrames)){  
            m_bIntraRefreshSet = TRUE;
            InternalLog(L"    Intra Refresh Mode: %lu", m_uiIntraRefreshMode);
            InternalLog(L"    Intra Refresh Frames: %lu", m_uiIntraRefreshFrames);
        }

    } while (FALSE);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::H264BitStreamVerifySetUpVerifier()
{
    HRESULT hr = S_OK;
    do
    {
        if (m_bIsLowLatency && (m_ulBFrameCountConfig == 0))
        {
            m_pH264EncVerifier->SetLowLatency(TRUE);
        }

        if (m_bIsLTRModeSet && m_pLTRFrameInfo != NULL)
        {
            _HRL(m_pH264EncVerifier->SetLTRInfo(m_pLTRFrameInfo), InternalLog(L"Failed to set LTR info on Bitstream Analyzer"));
        }

        if (m_bIsSetTemporalLayers)
        {
            m_pH264EncVerifier->SetTemporalLayers(TRUE);
        }
        else
        {
            m_pH264EncVerifier->SetTemporalLayers(FALSE);
        }

    } while (FALSE);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::H264BitStreamVerifyParseBitStream()
{
    HRESULT hr = S_OK;
    do 
    {
        // Parse H.264 bitstream using instrumented H.264 decoder
        /*#ifdef ELEMENTARY_STREAM
        _HR(hr = m_pH264EncVerifier->ParseBSInstrumentedDecoder(L"input.264"));
        #else
        _HR(hr = m_pH264EncVerifier->ParseBSInstrumentedDecoder(L"input.mp4"));
        #endif
        */

        InterlockedIncrement(&g_ulLoggerCount);
        g_pLogger = m_pLog;

        // Parse H.264 bitstream using bitstream analyzer
        _HRL(hr = m_pH264EncVerifier->ParseBSAnalyzer(m_strOutputFile, &LogToGlobalLogger), InternalLog(L"Failed bitstream analysis with hr = %x\n", hr));

        // Cleanup logging wrapper function
        if (InterlockedDecrement(&g_ulLoggerCount) == 0)
        {
            g_pLogger = NULL;
        }
    } while (FALSE);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::H264BitStreamVerifyReadActualValuesFromEncodedFile()
{
    HRESULT hr = S_OK;
    do
    {
        // Now that we know ulBitrateConfig and ulBufferSizeConfig we can populate buffer sizes for each frame
        if (eAVEncCommonRateControlMode_CBR == m_BitRateControlModeConfig)
        {
            m_ulBitrateCalcConfig = ((0 != m_ulMeanBitRateConfig) ? m_ulMeanBitRateConfig : m_ulBitrateConfig);
            _HR(hr = m_pH264EncVerifier->BufferSizesPopulate(m_ulBitrateCalcConfig, m_ulBufferSizeConfig));
        }
        else if (eAVEncCommonRateControlMode_PeakConstrainedVBR == m_BitRateControlModeConfig)
        {
            m_ulBitrateCalcConfig = m_ulMaxBitrateConfig;
            _HR(hr = m_pH264EncVerifier->BufferSizesPopulate(m_ulBitrateCalcConfig, m_ulBufferSizeConfig));
        }
        else if (eAVEncCommonRateControlMode_UnconstrainedVBR == m_BitRateControlModeConfig)
        {
            m_ulBitrateCalcConfig = ((0 != m_ulMeanBitRateConfig) ? m_ulMeanBitRateConfig : m_ulBitrateConfig);
        }

        // Read actual values from the encoded file
        _HR(m_pH264EncVerifier->GetProfile(&m_ulEncodedProfile));
        _HR(m_pH264EncVerifier->GetConstraintFlags(&m_ulEncodedCFBitset));
        _HR(m_pH264EncVerifier->GetLevel(&m_ulEncodedLevel));
        _HR(m_pH264EncVerifier->GetEntropyCoding(&m_ulEncodedEntropyCoding));
        _HR(m_pH264EncVerifier->GetVuiInfoPresentFlag(&m_uiEncodedVuiInfoPresentFlag));
        _HR(m_pH264EncVerifier->GetTimingInfoPresentFlag(&m_uiEncodedTimingInfoPresentFlag));
        _HR(m_pH264EncVerifier->GetNumUnitsInTick(&m_uiEncodedNumUnitsInTick));
        _HR(m_pH264EncVerifier->GetTimeScale(&m_uiEncodedTimeScale));
        _HR(m_pH264EncVerifier->GetFixedFrameFlag(&m_uiEncodedFixedFrameRateFlag));
        _HR(m_pH264EncVerifier->GetGOPCount(&m_ulEncodedGOPCount));
        _HR(m_pH264EncVerifier->GetVideoFullRangeFlag(&m_uiEncodedColorRangeFlag));
        _HR(m_pH264EncVerifier->GetGapsInFrameNumValueFlag(&m_uiEncodedGapsInFrameNumValueFlag));
        _HR(m_pH264EncVerifier->GetAverageBitrate(&m_ulEncodedActualBitrate));
        _HR(m_pH264EncVerifier->GetTemporalLayerCount(&m_ulEncodedTemporalLayerCount));

        if (m_bIsMaxNumRefFrameSet)
        {
            m_pH264EncVerifier->GetMaxNumRefFrame(&m_uiEncodedMaxNumRefFrame);
        }

        //Adjust GOPSizeConfig if Dynamic temporal layers are set
        // get the maximum dynamic supported temporal layer through Encoder CodecAPI
        // I am using the static number SW H.264 encoder has, 3
        // please get the right value through CodecAPI calls for SW or HW encoders and replace 3 
        if (m_bIsDynamicLayerConfigSet || m_bIsLayerCountConfigSet)
        {
            //unsigned int uiMinGOPSize = 1 << (m_pH264CustomParser->m_uiMaxTemporalLayers-1);
            unsigned int uiMinGOPSize = 1 << (3 - 1);
            if (m_ulGOPSizeConfig % uiMinGOPSize != 0){
                m_ulGOPSizeConfig = m_ulGOPSizeConfig + (uiMinGOPSize - (m_ulGOPSizeConfig % uiMinGOPSize));
            }
        }

        _HR(m_pH264EncVerifier->GetIsBFrames(&m_bEncodedIsBFrames));

        // Read delta quantization from encoded file
        _HR(m_pH264EncVerifier->GetFixedQP(&m_lEncodedQPNumber));

        InternalLog(L"\nEncoded file settings:");
        InternalLog(L"    Profile = %lu", m_ulEncodedProfile);
        InternalLog(L"    Level = %lu", m_ulEncodedLevel);
        InternalLog(L"    Entropy coding = %s", (m_ulEncodedEntropyCoding == 0 ? L"CAVLC" : L"CABAC"));
        InternalLog(L"    GOPCount = %lu", m_ulEncodedGOPCount);
        InternalLog(L"    InputFrameCount = %lu", m_uiInputFrameCount);
        InternalLog(L"    Bitstream %s B-Frames", ((TRUE == m_bEncodedIsBFrames) ? L"contains" : L"does NOT contain"));
        InternalLog(L"    QP = %d", m_lEncodedQPNumber);

        // Read SPS ID from encoded file
        if (TRUE == m_bIsSPSIDConfigSet)
        {
            _HR(m_pH264EncVerifier->GetSPSID(&m_ulEncodedSPSID));
            InternalLog(L"    SPS ID = %lu", m_ulEncodedSPSID);
        }

        // Read PPS ID from encoded file
        if (TRUE == m_bIsPPSIDConfigSet)
        {
            _HR(m_pH264EncVerifier->GetPPSID(&m_ulEncodedPPSID));
            InternalLog(L"    PPS ID = %lu", m_ulEncodedPPSID);
        }

    } while (FALSE);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::H264BitStreamVerifyCheckH264Profile()
{
    HRESULT hr = S_OK;
    do 
    {
        if (eAVEncH264VProfile_Base == m_ulProfileConfig || eAVEncH264VProfile_ConstrainedBase == m_ulProfileConfig)
        {
            if (eAVEncH264VProfile_Base != m_ulEncodedProfile)
            {
                _HRL(
                    E_FAIL,
                    InternalLog(L"    ERROR: Profile doesn't match! Encoded file profile = %lu; Config file = %lu",
                    m_ulEncodedProfile,
                    m_ulProfileConfig));
            }
        }
        else if (eAVEncH264VProfile_High == m_ulProfileConfig || eAVEncH264VProfile_UCConstrainedHigh == m_ulProfileConfig)
        {
            if (eAVEncH264VProfile_High != m_ulEncodedProfile)
            {
                _HRL(
                    E_FAIL,
                    InternalLog(L"    ERROR: Profile doesn't match! Encoded file profile = %lu; Config file = %lu",
                    m_ulEncodedProfile,
                    m_ulProfileConfig));
            }
        }
        else if (eAVEncH264VProfile_Main == m_ulProfileConfig)
        {
            if (eAVEncH264VProfile_Main != m_ulEncodedProfile)
            {
                _HRL(
                    E_FAIL,
                    InternalLog(L"    ERROR: Profile doesn't match! Encoded file profile = %lu; Config file = %lu",
                    m_ulEncodedProfile,
                    m_ulProfileConfig));
            }
        }
        else {
            if (m_ulEncodedProfile != m_ulProfileConfig){
                _HRL(
                    E_FAIL,
                    InternalLog(L"    ERROR: Profile doesn't match! Encoded file profile = %lu; Config file = %lu",
                    m_ulEncodedProfile,
                    m_ulProfileConfig));
            }

        }

    } while (FALSE);

    return hr;
}

STDMETHODIMP 
    CH264EncodeVerify::H264BitStreamVerifyCheckConstrainFlag()
{
    HRESULT hr = S_OK;
    do 
    {
        //Constraint flag checks for Const. High and Baseline
        if (eAVEncH264VProfile_ConstrainedBase == m_ulProfileConfig){
            if ((m_ulEncodedCFBitset & (1 << 1)) == 0){
                _HRL(
                    E_FAIL,
                    InternalLog(L"    ERROR: Constraint flags aren't set for correct profile! Constraint flags = 0x%x;  Config file = %lu - expect flag 1 set",
                    m_ulEncodedCFBitset,
                    m_ulProfileConfig));
            }
        }

        if (eAVEncH264VProfile_UCConstrainedHigh == m_ulProfileConfig){
            if (((m_ulEncodedCFBitset & (1 << 4)) == 0) || ((m_ulEncodedCFBitset & (1 << 5)) == 0)){
                _HRL(E_FAIL,
                    InternalLog(L"    ERROR: Constraint flags aren't set for correct profile! Constraint flags = 0x%x;  Config file = %lu - expect flags 4 and 5 set",
                    m_ulEncodedCFBitset,
                    m_ulProfileConfig));
            }
        }

    } while (FALSE);

    return hr;
}

STDMETHODIMP 
    CH264EncodeVerify::H264BitStreamVerifyCheckLevel()
{
    HRESULT hr = S_OK;
    do 
    {
        if (m_ulEncodedLevel != m_ulLevelConfig && m_ulLevelConfig != -1)
        {
            _HRL(
                E_FAIL,
                InternalLog(L"    ERROR: Level doesn't match! Encoded file level = %lu; Config file = %lu",
                m_ulEncodedLevel,
                m_ulLevelConfig));
        }

    } while (FALSE);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::H264BitStreamVerifyCheckSequenceParameter()
{
    HRESULT hr = S_OK;
    do
    {
        // Check SPS ID
        if ((m_uiTestConfigUsed & CODECAPI_H264SPSID) && (m_ulEncodedSPSID != m_ulSPSIDConfig))
        {
            _HRL(
                E_FAIL,
                InternalLog(L"    ERROR: SPS ID doesn't match! Encoded file SPS ID = %lu; Config file = %lu",
                m_ulEncodedSPSID,
                m_ulSPSIDConfig));
        }

        if (1 == m_uiEncodedVuiInfoPresentFlag)
        {
            //Check Timing info present flag 
            if (1 == m_uiEncodedTimingInfoPresentFlag)
            {
                if (0 != m_uiEncodedFixedFrameRateFlag)
                {
                    if (0 == m_uiEncodedTimeScale)
                    {
                        _HRL(
                            E_FAIL,
                            InternalLog(L"    ERROR: time_scale is set to 0 with non-zero fixed frame rate flag"));
                    }
                    if (0 == m_uiEncodedNumUnitsInTick)
                    {
                        _HRL(
                            E_FAIL,
                            InternalLog(L"    ERROR: num_units _in_tick is set to 0 with non-zero fixed frame rate flag"));
                    }
                }
            }
            if ((m_uiEncodedColorRangeFlag == 1 && m_eNominalRange != MFNominalRange_0_255) ||
                (m_uiEncodedColorRangeFlag == 0 && m_eNominalRange != MFNominalRange_16_235)){
                _HRL(
                    E_FAIL,
                    InternalLog(L"    ERROR: Nominal color range doesn't match! Encoded file video_full_range_flag = %lu; Input type range = %lu",
                    m_uiEncodedColorRangeFlag,
                    m_eNominalRange));
            }

            //Check if Frame rate set on the transform match the one form Timing information set on the bit stream
            //(if the timing info is present and valid)
            if (m_bIsFrameRateSet && m_uiEncodedFixedFrameRateFlag)
            {
                double configFPS = (double)m_ulFRNumerator / (double)m_ulFRDenominator;
                double bitStreamFPS = ((double)m_uiEncodedTimeScale / (2 * (double)m_uiEncodedNumUnitsInTick));
                double deviation = configFPS - bitStreamFPS;
                if (deviation > 0.5 || deviation < -0.5)
                {
                    _HRL(
                        E_FAIL,
                        InternalLog(L"    ERROR: Frame rate doesnot match between the config and from Timing Info on the bit stream Encoded file FPS = %f; Config file = %f",
                        bitStreamFPS,
                        configFPS));
                }
            }
        }
        else if (m_hwMode != MFTopologyHelper::HWMode::HW || m_eNominalRange == MFNominalRange_0_255)
        {
            _HRL(
                E_FAIL,
                InternalLog(L"    ERROR:Video Usability Information is not set in the sequence parameter set on the compressed bit stream"));
        }

    } while (FALSE);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::H264BitStreamVerifyCheckPPSID()
{
    HRESULT hr = S_OK;
    do 
    {
        if ((m_uiTestConfigUsed & CODECAPI_H264PPSID) && (m_ulEncodedPPSID != m_ulPPSIDConfig))
        {
            _HRL(
                E_FAIL,
                InternalLog(L"    ERROR: PPS ID doesn't match! Encoded file PPS ID = %lu; Config file = %lu",
                m_ulEncodedPPSID,
                m_ulPPSIDConfig));
        }

    } while (FALSE);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::H264BitStreamVerifyCheckEntropyCoding()
{
    HRESULT hr = S_OK;
    do
    {
        if (m_ulEncodedEntropyCoding != m_ulEntropyCodingConfig)
        {
            _HRL(
                E_FAIL,
                InternalLog(L"    ERROR: Entropy coding doesn't match! Encoded file entropy coding = %lu; Config file = %lu",
                m_ulEncodedEntropyCoding,
                m_ulEntropyCodingConfig));
        }

    } while (FALSE);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::H264BitStreamVerifyCheckMaxNumRefFrames()
{
    HRESULT hr = S_OK;
    do 
    {
        if (m_uiMaxNumRefFrame < m_uiEncodedMaxNumRefFrame)
        {
            _HRL(
                E_FAIL,
                InternalLog(L"    ERROR: Maximum number of reference frame mismatch! Encoded file maxnumref = %lu; Config file = %lu",
                m_uiEncodedMaxNumRefFrame,
                m_uiMaxNumRefFrame));
        }

    } while (FALSE);

    return hr;
}

STDMETHODIMP
CH264EncodeVerify::H264BitStreamVerifyCheckBitrate()
{
    HRESULT hr = S_OK;

    const ULONG ulBitrateFrameCount = 700; // min number of frames required for bitrate convergence in VBR mode

    ULONG ulBufferDrainGOP = 0;
    ULONG ulBufferDrainFrame = 0;
    BOOL bBufferSizesValid = TRUE;

    do
    {
        InternalLog(L"    Actual bitrate = %lu kbits/s", m_ulEncodedActualBitrate / 1000);
        InternalLog(L"    Config bitrate = %lu kbits/s", m_ulBitrateCalcConfig / 1000);

        // Validate buffer sizes in encoded file to match config file for CBR/PeakVBR 
        // (using bitrate and buffer size from config file)
        if (m_ulBufferSizeConfig != 0 && (eAVEncCommonRateControlMode_CBR == m_BitRateControlModeConfig ||
            eAVEncCommonRateControlMode_PeakConstrainedVBR == m_BitRateControlModeConfig))
        {
            if (m_strOutputFile.Find(L".m2ts", 0) == -1){
                _HR(m_pH264EncVerifier->BufferSizesValidate(&bBufferSizesValid, &ulBufferDrainGOP, &ulBufferDrainFrame));
                if (TRUE == bBufferSizesValid)
                {
                    InternalLog(L"    Buffer never drains");
                }
                else
                {
                    _HRL(E_FAIL,
                        InternalLog(L"    ERROR: Buffer drains on GOP %lu, frame %lu", ulBufferDrainGOP, ulBufferDrainFrame));
                }
            }
            else{
                InternalLog(L"    Skipping buffer checks for m2ts file");
            }
        }

        // Calculate bitrate in encoded file to match config file for CBR/PeakVBR/UnconstrainedVBR
        if (eAVEncCommonRateControlMode_CBR == m_BitRateControlModeConfig)
        {

            float fBitrateDeviation = 0.0;
            fBitrateDeviation = (float)(abs((int)(m_ulEncodedActualBitrate - m_ulBitrateCalcConfig))) / (float)m_ulEncodedActualBitrate;
            InternalLog(
                L"    Actual bitrate is off config settings by %5.2f%%, allowed deviation = %5.2f%%",
                fBitrateDeviation * 100,
                EPSILON * 100);

            // To check there must be at least 700 frames for bitrate to stabilize
            if (m_uiInputFrameCount >= ulBitrateFrameCount)
            {
                if (fBitrateDeviation < EPSILON)
                {
                    InternalLog(L"    Bitrate matches after %lu frames", m_uiInputFrameCount);
                }
                else
                {
                    _HRL(E_FAIL,
                        InternalLog(L"    ERROR: Bitrate DOESN'T match after %lu frames", m_uiInputFrameCount));
                }
            }
            else
            {
                InternalLog(L"    %lu frames is not enough to check bitrate, %lu frames are required", m_uiInputFrameCount, ulBitrateFrameCount);
            }

        }

    } while (FALSE);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::H264BitStreamVerifyCheckTemporalLayers()
{
    HRESULT hr = S_OK;
    do 
    {
        //Check that if temporal layers are set, gaps_in_frame_num_value_allowed_flag is 1
        if (m_bIsLayerCountConfigSet == TRUE || TRUE == m_bIsDynamicLayerConfigSet){
            if (m_uiEncodedGapsInFrameNumValueFlag == 0){
                _HRL(
                    E_FAIL,
                    InternalLog(L"    ERROR: gaps_in_frame_num_value_allowed_flag is set to 0 with temporal layers enabled (Count: %d)",
                    m_ulLayerCountConfig));
            }
        }

        // Check temporal layer count against config setting
        // Set nextLayers to config TL count for verification
        if (TRUE == m_bIsLayerCountConfigSet && FALSE == m_bIsDynamicLayerConfigSet)
        {

            if (m_ulEncodedTemporalLayerCount != m_ulLayerCountConfig)
            {
                _HRL(
                    E_FAIL,
                    InternalLog(L"    ERROR: Temporal layer count doesn't match! Encoded file temporal layer count = %lu; Config file = %lu",
                    m_ulEncodedTemporalLayerCount,
                    m_ulLayerCountConfig));
            }
        }

    } while (FALSE);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::H264BitStreamVerifyCheckBFrames(ULONG ulBFramesMin, ULONG ulBFramesMax, ULONG ulGOPIndex, ULONG ulGOPSize)
{
    HRESULT hr = S_OK;
    UINT32 uiLastPBSequenceLength = 0;

    do
    {
        // Only without temporal layers!
        if ((!m_bIsLayerCountConfigSet && !m_bIsDynamicLayerConfigSet) && (!m_bIsFrameCheckDisabled))
        {
            if (m_uiTestConfigUsed & CODECAPI_BPICTURE_COUNT)
            {
                if (m_ulBFrameCountConfig == 0) // Special case when BFrame check enabled but BFrame configured to 0
                {
                    if ((ulBFramesMin != 0) || (ulBFramesMax != 0))
                    {
                        _HRL(
                            E_FAIL,
                            InternalLog(L"    ERROR: B-Frames don't match in GOP #%d! BFrame is present but it configured to 0! Encoded file min = %lu, max = %lu; Config file = %lu",
                            ulGOPIndex,
                            ulBFramesMin,
                            ulBFramesMax,
                            m_ulBFrameCountConfig));
                    }
                }
                else
                {
                    // Checks for all GOPs
                    if (ulBFramesMin > m_ulBFrameCountConfig) // BFrame minimum error -> always break
                    {
                        _HRL(
                            E_FAIL,
                            InternalLog(L"    ERROR: B-Frames don't match in GOP #%d! Minimum BFrame count greater than configured! Encoded file min = %lu, max = %lu; Config file = %lu",
                            ulGOPIndex,
                            ulBFramesMin,
                            ulBFramesMax,
                            m_ulBFrameCountConfig));
                    }

                    uiLastPBSequenceLength = ((ulGOPSize - 1) % (m_ulBFrameCountConfig + 1));
                    if (ulGOPSize > (m_ulBFrameCountConfig + 1)) // GOP size greater than a full IPBBB.. sequence
                    {
                        if (ulBFramesMax != m_ulBFrameCountConfig) // BFrame maximum check
                        {
                            _HRL(
                                E_FAIL,
                                InternalLog(L"    ERROR: B-Frames don't match in GOP #%d! Maximum BFrame count less than configured! Encoded file min = %lu, max = %lu; Config file = %lu",
                                ulGOPIndex,
                                ulBFramesMin,
                                ulBFramesMax,
                                m_ulBFrameCountConfig));
                        }

                        switch (uiLastPBSequenceLength)
                        {
                        case 0: // Last sequence is full -> Min equal Max
                            if (ulBFramesMin != ulBFramesMax) // Last BFrame(s) check 
                            {
                                _HRL(
                                    E_FAIL,
                                    InternalLog(L"    ERROR: B-Frames don't match in GOP #%d!! Min BFrame should be equal to max! Encoded file min = %lu, max = %lu; Config file = %lu",
                                    ulGOPIndex,
                                    ulBFramesMin,
                                    ulBFramesMax,
                                    m_ulBFrameCountConfig));
                            }
                            break;
                        case 1: // Only P frame in the last sequence -> Min == 0
                            if (ulBFramesMin != 0) // Last BFrame(s) check 
                            {
                                _HRL(
                                    E_FAIL,
                                    InternalLog(L"    ERROR: B-Frames don't match in GOP #%d!! Minimum BFrame should be 0! Encoded file min = %lu, max = %lu; Config file = %lu",
                                    ulGOPIndex,
                                    ulBFramesMin,
                                    ulBFramesMax,
                                    m_ulBFrameCountConfig));
                            }
                            break;
                        default: // PB....
                            if (ulBFramesMin != (uiLastPBSequenceLength - 1)) // Last BFrame(s) check 
                            {
                                _HRL(
                                    E_FAIL,
                                    InternalLog(L"    ERROR: B-Frames don't match in GOP #%d!! Minimum BFrame count error! Encoded file min = %lu, max = %lu; Config file = %lu",
                                    ulGOPIndex,
                                    ulBFramesMin,
                                    ulBFramesMax,
                                    m_ulBFrameCountConfig));
                            }
                            break;
                        }
                    }
                    else // Extreme case - GOP size smaller than the full IPBBB... sequence
                    {
                        switch (uiLastPBSequenceLength)
                        {
                        case 0: // Only one I frame present -> Min == 0, Max == 0
                        case 1: // Only one P frame in the last sequence -> Min == 0, Max == 0
                            if ((ulBFramesMin != 0) && (ulBFramesMax != 0))
                            {
                                _HRL(
                                    E_FAIL,
                                    InternalLog(L"    ERROR: B-Frames don't match in GOP #%d!! Min and max BFrame should be zero! Encoded file min = %lu, max = %lu; Config file = %lu",
                                    ulGOPIndex,
                                    ulBFramesMin,
                                    ulBFramesMax,
                                    m_ulBFrameCountConfig));
                            }
                            break;
                        default: // PB....
                            if (ulBFramesMin != (uiLastPBSequenceLength - 1)) // Last BFrame(s) check 
                            {
                                _HRL(
                                    E_FAIL,
                                    InternalLog(L"    ERROR: B-Frames don't match in GOP #%d!! Minimum BFrame count error (Extreme small GOP size)! Encoded file min = %lu, max = %lu; Config file = %lu",
                                    ulGOPIndex,
                                    ulBFramesMin,
                                    ulBFramesMax,
                                    m_ulBFrameCountConfig));
                            }
                            break;
                        }
                    }
                }
            }
        }

    } while (FALSE);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::H264BitStreamVerifyCheckGOPSize(ULONG ulGOPIndex, ULONG ulGOPSize)
{
    HRESULT hr = S_OK;
    ULONG ulExpectedGOPSize = 0;
    ULONG ulGOPSizeTolerance = 0;
    ULONG ulGOPSizeDiff = 0;

    do
    {
        if (S_OK == m_pH264CustomParser->GetExpectedGOPSizeByGOPIndex(ulGOPIndex, &ulExpectedGOPSize))
        {
            ulGOPSizeTolerance = 0;
            if (ulGOPIndex == (m_ulEncodedGOPCount - 1))
            {
                if (m_strOutputFile.Find(L".m2ts", 0) != -1)
                {
                    ulGOPSizeTolerance = M2TS_PARSE_TOLERANCE;
                }
            }
            ((ulExpectedGOPSize - ulGOPSize) > 0) ? ulGOPSizeDiff = ulExpectedGOPSize - ulGOPSize : ulGOPSizeDiff = ulGOPSize - ulExpectedGOPSize;
            if (ulGOPSizeDiff > ulGOPSizeTolerance)
            {
                _HRL(
                    E_FAIL,
                    InternalLog(L"    ERROR: GOP sizes don't match! Encoded file = %lu; Config file = %lu",
                    ulGOPSize,
                    ulExpectedGOPSize));
            }
        }

    } while (FALSE);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::H264BitStreamVerifyCheckFrameQP(LONG lFrameQPNumber, ULONG ulTotalFrameIndex, ULONG frameType)
{
    HRESULT hr = S_OK;
    HRESULT hrGetQP = S_OK;
    UINT64 ui64CurrentFrameTypeQP = 0x0000000000000000;

    do
    {
        if (eAVEncCommonRateControlMode_Quality == m_BitRateControlModeConfig)
        {
            if (FALSE == m_bIsFrameQPConfigSet &&
                FALSE == m_bIsFrameLayerQPConfigSet &&
                FALSE == m_bIsFrameTypeQPConfigSet &&
                FALSE == m_bIsEncodeQPConfigSet &&
                lFrameQPNumber != (LONG)m_ulQPConfig)
            {
                //If the QP isn't equal, it can be different if it's set to the min or max
                if (!((TRUE == m_bIsMinQPConfigSet && lFrameQPNumber == (LONG)m_ulMinQPConfig) ||
                    (TRUE == m_bIsMaxQPConfigSet && lFrameQPNumber == (LONG)m_ulMaxQPConfig))){
                    _HRL(E_FAIL,
                        InternalLog(L"    ERROR: Actual QP number %d != QP number in config file %d ", lFrameQPNumber, (LONG)m_ulQPConfig));
                }
            }
        }

        if (TRUE == m_bIsEncodeQPConfigSet && FALSE == m_bIsFrameTypeQPConfigSet)
        {
            if (lFrameQPNumber != (LONG)m_ulQPConfig)
            {
                if (!((TRUE == m_bIsMinQPConfigSet && lFrameQPNumber == (LONG)m_ulMinQPConfig) ||
                    (TRUE == m_bIsMaxQPConfigSet && m_lEncodedQPNumber == (LONG)m_ulMaxQPConfig)))
                {
                    _HRL(E_FAIL,
                        InternalLog(L"    ERROR: Actual QP number %d != QP number in config file %d ", lFrameQPNumber, (LONG)m_ulQPConfig));
                }
            }
        }

        if (TRUE == m_bIsFrameTypeQPConfigSet)
        {
            UINT64 ui64FrameTypeQPFromConfig = 1;
            UINT64 ui64ExpectedQP = 0;
            hrGetQP = m_pH264CustomParser->GetFrameTypeQP(ulTotalFrameIndex, &ui64FrameTypeQPFromConfig);
            if (hrGetQP == S_OK)
            {
                ui64CurrentFrameTypeQP = ui64FrameTypeQPFromConfig;
            }
            if (ui64CurrentFrameTypeQP != 0)
            {
                switch (frameType)
                {
                case FrameType_IFrame:
                    ui64ExpectedQP = ui64CurrentFrameTypeQP & 0x000000000000FFFF;
                    if (lFrameQPNumber != ui64ExpectedQP)
                    {
                        _HRL(E_FAIL,
                            InternalLog(L"ERROR: Invalid QP on I frame - expected: %lld returned: %d Frame index: %u", ui64ExpectedQP, lFrameQPNumber, ulTotalFrameIndex));
                    }
                    break;
                case FrameType_PFrame:
                    ui64ExpectedQP = (ui64CurrentFrameTypeQP & 0x00000000FFFF0000) >> 16;
                    if (lFrameQPNumber != ui64ExpectedQP)
                    {
                        _HRL(E_FAIL,
                            InternalLog(L"ERROR: Invalid QP on P frame - expected: %lld returned: %d Frame index: %u", ui64ExpectedQP, lFrameQPNumber, ulTotalFrameIndex));
                    }
                    break;
                case FrameType_BFrame:
                    ui64ExpectedQP = (ui64CurrentFrameTypeQP & 0x0000FFFF00000000) >> 32;
                    if (lFrameQPNumber != ui64ExpectedQP)
                    {
                        _HRL(E_FAIL,
                            InternalLog(L"ERROR: Invalid QP on B frame - expected: %lld returned: %d Frame index: %u", ui64ExpectedQP, lFrameQPNumber, ulTotalFrameIndex));
                    }
                    break;
                default:
                    _HRL(E_FAIL, 
                        InternalLog(L"ERROR: Unexpected frame type found: %d", frameType));
                    break;
                }
            }
        }

    } while (FALSE);
    
    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::H264BitStreamVerifyCheckQPRange(LONG lFrameQPNumber, LONG &lFrameQPNumberMin, LONG &lFrameQPNumberMax)
{
    HRESULT hr = S_OK;
    do 
    {
        if ((m_uiTestConfigUsed & CODECAPI_MINQP) && TRUE == m_bIsMinQPConfigSet &&
            FALSE == m_bIsFrameQPConfigSet &&
            lFrameQPNumber < lFrameQPNumberMin)
        {
            lFrameQPNumberMin = lFrameQPNumber;
            if (lFrameQPNumberMin < (LONG)m_ulMinQPConfig)
            {
                _HRL(
                    E_FAIL,
                    InternalLog(L"    ERROR: Min QP doesn't match! Encoded file = %d; Config file = %lu",
                    lFrameQPNumberMin,
                    m_ulMinQPConfig));
            }
        }

        if ((m_uiTestConfigUsed & CODECAPI_MAXQP) && TRUE == m_bIsMaxQPConfigSet &&
            FALSE == m_bIsFrameQPConfigSet &&
            lFrameQPNumber > lFrameQPNumberMax)
        {
            lFrameQPNumberMax = lFrameQPNumber;
            if (lFrameQPNumberMax > (LONG)m_ulMaxQPConfig)
            {
                _HRL(
                    E_FAIL,
                    InternalLog(L"    ERROR: Max QP doesn't match! Encoded file = %d; Config file = %lu",
                    lFrameQPNumberMax,
                    m_ulMaxQPConfig));
            }
        }

    } while (FALSE);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::H264BitStreamVerifyCheckTemporalLayers(ULONG ulTotalFrameIndex, ULONG ulTemporalLayer, ULONG ulFrameIndexInGOP, ULONG ulGOPIndex)
{
    HRESULT hr = S_OK;
    ULONG ulExpectedTemporalLayer = 0;

    do 
    {
        if (TRUE == m_bIsDynamicLayerConfigSet || TRUE == m_bIsLayerCountConfigSet)
        {
            if (S_OK == m_pH264CustomParser->GetLayerIndexByFrame(ulTotalFrameIndex, &ulExpectedTemporalLayer))
            {
                if (ulTemporalLayer != ulExpectedTemporalLayer)
                {
                    _HRL(E_FAIL, InternalLog(L"Unexpected temporal layer found at frame %d in gop index %d. Global frame index %d. Current layer %d, expected layer %d",
                        ulFrameIndexInGOP, ulGOPIndex, ulTotalFrameIndex, ulTemporalLayer, ulExpectedTemporalLayer));
                    InternalLog(L"NO MATCH FOUND: ulTotalFrameIndex=%d, ulTemporalLayer=%d, ulExpectedTemporalLayer=%d", ulTotalFrameIndex, ulTemporalLayer, ulExpectedTemporalLayer);
                }
            }
            else
            {
                InternalLog(L"ERROR: GetLayerIndexByFrame() failed! Invalid frame index in GOP calculation. Frame number: %u.", ulTotalFrameIndex);
            }
        }

        if (TRUE == m_bIsLayerCountConfigSet && FALSE == m_bIsDynamicLayerConfigSet)
        {
            if (S_OK == m_pH264CustomParser->GetLayerIndexByFrame(ulTotalFrameIndex, &ulExpectedTemporalLayer))
            {
                if (ulTemporalLayer != ulExpectedTemporalLayer)
                {
                    _HRL(
                        E_FAIL,
                        InternalLog(L"    ERROR: Frame layer doesn't match! GOP #: %ul, Frame #: %ul. Encoded file = %d; Config file = %lu",
                        ulGOPIndex,
                        ulFrameIndexInGOP,
                        ulTemporalLayer,
                        ulExpectedTemporalLayer));
                }
            }
        }

    } while (FALSE);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::H264BitStreamVerifyCheckFrameLayerQP(ULONG ulGOPIndex, ULONG ulGOPSize, ULONG ulFrameIndexInGOP, LONG lFrameQPNumber, ULONG frameType)
{
    HRESULT hr = S_OK;
    ULONGLONG ullFrameLayerQPConfig = 0;

    do 
    {
        if (TRUE == m_bIsFrameLayerQPConfigSet)
        {
            hr = m_pConfigHelper->GetFrameLayerQP(ulGOPIndex * ulGOPSize + ulFrameIndexInGOP, &ullFrameLayerQPConfig);
            if (S_OK != hr)
            {
                InternalLog(L"Failed ITAEFConfigHelper::GetFrameLayerQP() with hr = 0x%08X", hr);
            }

            if (lFrameQPNumber != ullFrameLayerQPConfig)
            {
                int delta = lFrameQPNumber - (ULONG)ullFrameLayerQPConfig;
                if (frameType != FrameType_BFrame || delta < MIN_ACCEPTABLE_QP_DISTANCE || delta >= MAX_ACCEPTABLE_QP_DISTANCE)
                {

                    _HRL(
                        E_FAIL,
                        InternalLog(L"    ERROR: Frame Layer QP doesn't match! GOP #: %ul, Frame #: %ul. Encoded file = %d; Config file = %llu",
                        ulGOPIndex,
                        ulFrameIndexInGOP,
                        lFrameQPNumber,
                        ullFrameLayerQPConfig));
                }
            }
        }

    } while (FALSE);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::H264BitStreamVerifyLogFrameProperties(ULONG ulGOPIndex, ULONG ulFrameIndexInGOP, ULONG ulTemporalLayer)
{
    HRESULT hr = S_OK;
    LONG lBufferSize = 0;
    WCHAR pwcTemporalLayer[MAX_PATH] = { 0 };
    WCHAR pwcBufferSize[MAX_PATH] = { 0 };

    do 
    {
        if (eAVEncCommonRateControlMode_CBR == m_BitRateControlModeConfig ||
            eAVEncCommonRateControlMode_PeakConstrainedVBR == m_BitRateControlModeConfig)
        {
            _HR(m_pH264EncVerifier->GetBufferSizeAt(ulGOPIndex, ulFrameIndexInGOP, &lBufferSize));

            StringCchPrintf(pwcBufferSize, MAX_PATH, L", Buffer size(kbytes) = %d", lBufferSize / 1000);
        }

        if (TRUE == m_bIsFrameLayerQPConfigSet)
        {
            StringCchPrintf(pwcTemporalLayer, MAX_PATH, L", Temporal Layer = %lu", ulTemporalLayer);
        }

        // This is a verbose log for frame data
        //InternalLog(
        //    L"        Frame #%lu: Size(bytes) = %lu; Time Stamp(ms) = %I64d, Decode Time Stamp(ms) = %I64d, QPNumber = %d%s%s, Frame Type =%lu", 
        //    ulFrameIndexInGOP, 
        //    ulFrameSize, 
        //    (LONGLONG)ullFrameTimeStamp/10000, 
        //    (LONGLONG)ullFrameDecodeTimeStamp/10000, 
        //    lFrameQPNumber, 
        //    pwcTemporalLayer, 
        //    pwcBufferSize,
        //    frameType);

    } while (FALSE);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::H264BitStreamVerifyCheckSliceControl(ULONG ulGOPIndex, ULONG ulFrameIndexInGOP, ULONG ulTotalFrameIndex, ULONG ulGOPSize)
{
    HRESULT hr = S_OK;
    ULONG arrSliceControlSize[MAX_NUMBER_SLICES_PER_FRAME];
    ULONG arrSliceControlSizeBits[MAX_NUMBER_SLICES_PER_FRAME];

    do 
    {
        if (m_bIsSliceControlModeSet && m_bIsSliceControlSizeSet)
        {
            _HR(m_pH264EncVerifier->GetSliceControlSizeAt(ulGOPIndex, ulFrameIndexInGOP, arrSliceControlSize));
            _HR(m_pH264EncVerifier->GetSliceControlSizeBitsAt(ulGOPIndex, ulFrameIndexInGOP, arrSliceControlSizeBits));

            int exitIndex = 0;
            UINT32 ulBSWidth = 0;
            UINT32 ulBSHeight = 0;
            UINT32 mbSize = 16;
            UINT32 mbPerRow = 0;
            DWORD modeOneErrorMargin = (m_ulSliceControlSize / 10) / 8;

            ULONG ulSumSliceSize = 0;
            ULONG ulLastSliceSize = 0;
            ULONG ulAverageSliceSize = 0;
            ULONG ulCurrentSliceControlSize = 0;
            ULONG ulDynamicFrameSliceControlSize = 0;

            // Use dynamic SliceControlSize if set
            if ((m_bIsDynamicInitialSliceControlSizeSet) && (m_pH264CustomParser->IsDynamicInitialSliceControlSizeApplied()))
            {
                ulCurrentSliceControlSize = m_ulDynamicInitialSliceControlSize;
            }
            else
            {
                ulCurrentSliceControlSize = m_ulSliceControlSize;
            }

            if (m_pH264CustomParser->IsDynamicFrameSliceControlSizeSet())
            {
                HRESULT hrDynSliceControlSize = S_OK;
                hrDynSliceControlSize = m_pH264CustomParser->GetLastAppliedDynamicFrameSliceControlSize(ulTotalFrameIndex, &ulDynamicFrameSliceControlSize);
                if (SUCCEEDED(hrDynSliceControlSize))
                {
                    ulCurrentSliceControlSize = ulDynamicFrameSliceControlSize;
                }
            }

            // Note: m_ulSliceControlSize the same for all frames - currently not dynamically set
            switch (m_ulSliceControlMode)
            {
            case 0:
                // When the value is equal to 0, it controls the size in the number of MBs per slice, 
                // except the last slice which might be smaller.

                for (int i = 0; i < MAX_NUMBER_SLICES_PER_FRAME; i++)
                {
                    if (ULONG_MAX == arrSliceControlSize[i])
                        break;

                    if (0 == i)
                        continue;

                    ULONG diffMB = arrSliceControlSize[i] - arrSliceControlSize[i - 1];

                    if (diffMB != ulCurrentSliceControlSize)
                    {
                        hr = E_FAIL;
                        InternalLog(
                            L"    ERROR: SliceControlSize doesn't match! SliceControlMode= %u, SliceControlSize= Set: %u, Get: %u", m_ulSliceControlMode, ulCurrentSliceControlSize, diffMB);
                        InternalLog(L"    Failed on gop %d, frame %d, slice number %d", ulGOPIndex, ulFrameIndexInGOP, i);
                        exitIndex = i;
                        break;
                    }
                }
                break;

            case 1:
                // When the value is equal to 1, it controls the size in bits per slice. 
                for (int i = 0; i < MAX_NUMBER_SLICES_PER_FRAME; i++)
                {
                    if (ULONG_MAX == arrSliceControlSizeBits[i]){
                        exitIndex = i;
                        break;
                    }

                    ulSumSliceSize += arrSliceControlSizeBits[i];
                    ulLastSliceSize = arrSliceControlSizeBits[i];

                    if (i == MAX_NUMBER_SLICES_PER_FRAME - 1){
                        exitIndex = i;
                    }

                }

                //We need to make an exception for the last slice in the frame
                //it can be smaller - but we don't want it throwing off our average
                ulSumSliceSize -= ulLastSliceSize;
                if (exitIndex > 1){
                    ulAverageSliceSize = (ulSumSliceSize / (exitIndex - 1));

                    if ((ulAverageSliceSize < (ulCurrentSliceControlSize / 8) - modeOneErrorMargin) ||
                        (ulAverageSliceSize >(ulCurrentSliceControlSize / 8) + modeOneErrorMargin))
                    {
                        hr = E_FAIL;
                        InternalLog(
                            L"    ERROR: SliceControlSize doesn't match! Frame= %d, SliceControlMode= %u, SliceControlSize= Set: %u, Get: %u", ulTotalFrameIndex, m_ulSliceControlMode, m_ulSliceControlSize, ulAverageSliceSize * 8);
                        InternalLog(L"    Failed on GOP %d, frame %d", ulGOPIndex, ulFrameIndexInGOP);
                        break;
                    }

                    if (ulLastSliceSize > (ulCurrentSliceControlSize / 8) + modeOneErrorMargin){
                        hr = E_FAIL;
                        InternalLog(
                            L"    ERROR: Last slice in frame is too large! Frame= %d, SliceControlMode= %u, SliceControlSize= Set: %u, Get: %u", ulTotalFrameIndex, m_ulSliceControlMode, m_ulSliceControlSize, ulLastSliceSize * 8);
                        InternalLog(L"    Failed on GOP %d, frame %d", ulGOPIndex, ulFrameIndexInGOP);
                        break;
                    }
                }
                else if (exitIndex == 1){
                    //Only one slice in frame
                    if (ulLastSliceSize > (ulCurrentSliceControlSize / 8) + modeOneErrorMargin){
                        hr = E_FAIL;
                        InternalLog(
                            L"    ERROR: Last slice in frame is too large! Frame= %d, SliceControlMode= %u, SliceControlSize= Set: %u, Get: %u", ulTotalFrameIndex, m_ulSliceControlMode, m_ulSliceControlSize, ulLastSliceSize * 8);
                        InternalLog(L"    Failed on GOP %d, frame %d", ulGOPIndex, ulFrameIndexInGOP);
                        break;
                    }
                }
                else{
                    //we should never hit this case, since a frame always has at least one slice
                    hr = E_FAIL;
                    InternalLog(L"ERROR: Encountered slice-less frame at gop %d, frame %d", ulGOPIndex, ulFrameIndexInGOP);
                    break;
                }
                break;

            case 2:
                // When the value is equal to 2, it controls the size in MB row per slice, except the last slice which might be smaller. 

                _HR(m_pH264EncVerifier->GetResolution(&ulBSWidth, &ulBSHeight));
                mbPerRow = ulBSWidth / mbSize;
                if ((ulBSWidth % mbSize) != 0)
                    mbPerRow++;

                for (int i = 0; i < MAX_NUMBER_SLICES_PER_FRAME; i++)
                {
                    if (ULONG_MAX == arrSliceControlSize[i])
                        break;

                    if (0 == i)
                        continue;

                    ULONG diffMB = arrSliceControlSize[i] - arrSliceControlSize[i - 1];
                    if (diffMB != ulCurrentSliceControlSize * mbPerRow)
                    {
                        hr = E_FAIL;

                        InternalLog(
                            L"    ERROR: SliceControlSize doesn't match! Frame= %d, SliceControlMode= %u, SliceControlSize= Set: %u, Get: %u", ulTotalFrameIndex, m_ulSliceControlMode, ulCurrentSliceControlSize, diffMB / mbPerRow);
                        InternalLog(L"    Failed on GOP %d, frame %d, slice number %d", ulGOPIndex, ulFrameIndexInGOP, i);
                        exitIndex = i;
                        break;
                    }
                }
                break;
            }
            if (FAILED(hr)){
                break;
            }

            //Do ROI verification here
            //IF slice control is enabled!
            RECT rectROI;
            INT32 dQP = 0;
            ULONG arrSliceQP[MAX_NUMBER_SLICES_PER_FRAME] = { 0 };

            hr = m_pH264CustomParser->GetFrameROI(ulGOPIndex * ulGOPSize + ulFrameIndexInGOP, &dQP, &rectROI);
            if (SUCCEEDED(hr))
            {
                //figure out which slice ROI is in
                //Assume we're in slice control mode 2
                int numSlices = 0;
                _HR(m_pH264EncVerifier->GetResolution(&ulBSWidth, &ulBSHeight));
                if (m_ulSliceControlMode == 2){
                    numSlices = ulBSHeight / 16 / m_ulSliceControlSize;
                    if (ulBSHeight % 16 == 0 ? ulBSHeight / 16 % m_ulSliceControlSize != 0 : (ulBSHeight / 16 + 1) % m_ulSliceControlSize != 0){
                        numSlices++;
                    }
                    //Which slice is the ROI in?
                    int topMB = rectROI.top % 16 == 0 ? rectROI.top / 16 : rectROI.top / 16 + 1;
                    int botMB = rectROI.bottom % 16 == 0 ? rectROI.bottom / 16 : rectROI.bottom / 16 + 1;

                    int topSlice = topMB / m_ulSliceControlSize;
                    int botSlice = botMB / m_ulSliceControlSize;

                    int checkAgainst = 0;

                    if (topSlice == botSlice){
                        _HR(m_pH264EncVerifier->GetSliceQPAt(ulGOPIndex, ulFrameIndexInGOP, arrSliceQP));
                        if (topSlice == 0)
                            checkAgainst++;
                        if (dQP > 0){
                            if (arrSliceQP[checkAgainst] <= arrSliceQP[topSlice]){
                                _HR(E_FAIL);
                            }
                        }
                        else{
                            if (arrSliceQP[checkAgainst] >= arrSliceQP[topSlice]){
                                _HR(E_FAIL);
                            }
                        }

                    }
                }
            }
            else{
                hr = S_OK;
            }
        }

    } while (FALSE);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::H264BitStreamVerifyCheckExtractAndVerifyLayers()
{
    HRESULT hr = S_OK;
    do 
    {
        //If temporal layers are enabled and the bitstream is dumped, do extraction tests
        if (m_bIsLayerCountConfigSet == TRUE || TRUE == m_bIsDynamicLayerConfigSet)
        {
            if (m_pH264CustomParser->IsBitstreamDumpSet())
            {
                //Extraction tests here
                UINT32 ulBSWidth = 0;
                UINT32 ulBSHeight = 0;
                _HR(m_pH264EncVerifier->GetResolution(&ulBSWidth, &ulBSHeight));
                _HR(ExtractAndVerifyLayers(3, m_uiInputFrameCount, ulBSWidth, ulBSHeight));
            }
        }

    } while (FALSE);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::H264BitStreamVerifyCheckPSNR()
{
    HRESULT hr = S_OK;
    do 
    {
        if (m_bIsSWFileCreated)
        {
            InternalLog(L"Checking PSNRs now\n");

            _HR(H264CheckPSNR());

            InternalLog(L"PSNR Verification: SUCCESSFUL\n");
        }

    } while (FALSE);

    if (S_OK != hr)
    {
        InternalLog(L"PSNR Verification: FAILED\n");
    }

    return hr;
}

STDMETHODIMP
CH264EncodeVerify::H264BitStreamVerifyLogFileDuration()
{
    HRESULT hr = S_OK;
    ULONGLONG ullAPIDuration = 0;
    double duration = 0.0;

    do
    {
        _HR(m_pH264EncVerifier->GetPresentationDuration(&ullAPIDuration));
        InternalLog(L"    Presentation duration is %I64u  (in 100ns) or %I64u ms or %5.2f s", ullAPIDuration, ullAPIDuration / 10000, ((float)(ullAPIDuration / 10000)) / 1000);

        duration = (double)(ullAPIDuration) / 10000000.0;
        InternalLog(L"\n    Encoding duration (wall time): %5.2f s, %5.2f real-time\n", ((float)(m_dwSessionDurationMs)) / 1000, ((float)(ullAPIDuration / 10000)) / ((float)(m_dwSessionDurationMs)));
    } while (FALSE);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::H264BitStreamVerifyLogGOPProperties(ULONG ulGOPIndex, ULONG ulGOPSize, ULONGLONG ullGOPSizeTime, ULONG ulGOPBufferSize, ULONG ulBFramesMin, ULONG ulBFramesMax)
{
    HRESULT hr = S_OK;

#ifdef VERBOSE_LOGS
    InternalLog(
        L"    GOP #%lu: GOP Size(frames) = %lu;  GOP Size Time(sec) = %f;  GOP Buffer Size(bytes) = %lu;  B-Frames = %lu-%lu",
        ulGOPIndex,
        ulGOPSize,
        (double)ullGOPSizeTime / (double)10000000,
        ulGOPBufferSize,
        ulBFramesMin,
        ulBFramesMax);
#endif
    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::H264BitStreamVerifyLogFrameQPMinMax(LONG lFrameQPNumberMin, LONG lFrameQPNumberMax)
{
    HRESULT hr = S_OK;

    if (TRUE == m_bIsMinQPConfigSet)
    {
        InternalLog(L"    Min QP Number = %d", lFrameQPNumberMin);
    }
    if (TRUE == m_bIsMaxQPConfigSet)
    {
        InternalLog(L"    Max QP Number = %d", lFrameQPNumberMax);
    }

    return hr;
}



STDMETHODIMP CH264EncodeVerify::SetupSinkWriterEncodingAttributes(
    IMFAttributes*  pEncodingAttributes)
{
    HRESULT hr                      = S_OK;
    UINT32  un32Bitrate             = 0;
    UINT32  un32Quality             = 0;
    UINT32  un32BFrameCount         = 0;
    UINT32  un32BufferSize          = 0;
    UINT32  un32GOPSize             = 0;
    UINT32  un32QualityVsSpeed      = 0;
    UINT32  un32MeanBitRate         = 0;
    UINT32  un32MaxBitRate          = 0;
    UINT32  un32SPSID               = 0;
    UINT32  un32PPSID               = 0;
    UINT32  un32MinQP               = 0;
    UINT32  un32MaxQP               = 0;
    UINT32  un32TemporalLayerCount  = 0;
    UINT32  un32LTRBufferControl    = 0;
    BOOL    bIsQuality              = FALSE;
    BOOL    bCabacEnabled           = FALSE;

    do
    {
        if(pEncodingAttributes == NULL)
        {
            hr = E_POINTER;
            break;
        }

        _HR(H264BitStreamVerifyGetConfigValues());

        if(m_pConfigHelper->GetHWMode() == MFTopologyHelper::HW){
            _HRL(pEncodingAttributes->SetUINT32 (MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, TRUE),
                InternalLog(L"Failed to set MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS on Encoding Attributes")
                );
        }else{
            _HRL(pEncodingAttributes->SetUINT32 (MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, FALSE),
                InternalLog(L"Failed to set MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS on Encoding Attributes")
                );
        }

        _HRL(pEncodingAttributes->SetUINT32(
            MF_MT_MPEG2_PROFILE,
            m_ulProfileConfig
            ), InternalLog(L"Failed to set MF_MT_MPEG2_PROFILE on Encoding Attributes"));

        if(m_ulLevelConfig != -1)
        {
            _HRL(pEncodingAttributes->SetUINT32(
                MF_MT_MPEG2_LEVEL,
                m_ulLevelConfig
                ), InternalLog(L"Failed to set MF_MT_MPEG2_PROFILE on Encoding Attributes"));
        }

        if(m_pConfigHelper->IsRDCurve(
            NULL,
            &bIsQuality
            ) && FALSE == bIsQuality)
        {
            UINT32  un32RDCurveBitrate  = 0;
            if(S_OK == m_pConfigHelper->GetRDVariationByBitrate(
                0,
                &un32RDCurveBitrate))
            {
                un32Bitrate = un32RDCurveBitrate;
            }
            else
            {
                InternalLog(L"Failed ITAEFConfigHelper::GetRDVariationByBitrate() with hr = 0x%08X", hr);
            }
        }
        // Not RD curve scenario
        else
        {
            // MF_MT_AVG_BITRATE must be set
            un32Bitrate = m_ulBitrateConfig;
        }

        if(SUCCEEDED(hr))
        {
            _HRL(pEncodingAttributes->SetUINT32(
                MF_MT_AVG_BITRATE,
                un32Bitrate
                ), InternalLog(L"Failed to set MF_MT_AVG_BITRATE on Encoding Attributes"));
        }

        _HRL(pEncodingAttributes->SetUINT32(
            CODECAPI_AVEncCommonRateControlMode,
            m_BitRateControlModeConfig
            ), InternalLog(L"Failed to set CODECAPI_AVEncCommonRateControlMode on Encoding Attributes"));

        if(bIsQuality != FALSE)
        {
            UINT32  un32RDQuality   = 0;

            if(S_OK == m_pConfigHelper->GetRDVariationByQuality(
                0,
                &un32RDQuality))
            {
                un32Quality = un32Quality;
            }
            else
            {
                InternalLog(L"Failed ITAEFConfigHelper::GetRDVariationByQuality() with hr = 0x%08X", hr);
            }
        }
        // Not RD curve scenario
        else
        {
            un32Quality = m_ulQPConfig;
        }

        if(SUCCEEDED(hr))
        {
            _HRL(pEncodingAttributes->SetUINT32(
                CODECAPI_AVEncCommonQuality,
                un32Quality
                ), InternalLog(L"Failed to set CODECAPI_AVEncCommonQuality on Encoding Attributes"));
        }

        if(SUCCEEDED(m_pConfigHelper->GetBFrames(
            &un32BFrameCount)) && un32BFrameCount != -1)
        {
            _HRL(pEncodingAttributes->SetUINT32(
                CODECAPI_AVEncMPVDefaultBPictureCount,
                un32BFrameCount
                ), InternalLog(L"Failed to set CODECAPI_AVEncMPVDefaultBPictureCount on Encoding Attributes"));

            m_pConfigHelper->TestConfigUsed(CODECAPI_BPICTURE_COUNT);
        }

        if(SUCCEEDED(m_pConfigHelper->GetBufferSize(
            &un32BufferSize)))
        {
            _HRL(pEncodingAttributes->SetUINT32(
                CODECAPI_AVEncCommonBufferSize,
                un32BufferSize
                ), InternalLog(L"Failed to set CODECAPI_AVEncCommonBufferSize on Encoding Attributes"));
        }

        if(SUCCEEDED(m_pConfigHelper->GetGOPSize(
            &un32GOPSize)))
        {
            _HRL(pEncodingAttributes->SetUINT32(
                CODECAPI_AVEncMPVGOPSize,
                un32GOPSize
                ), InternalLog(L"Failed to set CODECAPI_AVEncMPVGOPSize on Encoding Attributes"));
        }

        if(SUCCEEDED(m_pConfigHelper->GetQualityVsSpeed(
            &un32QualityVsSpeed)))
        {
            _HRL(pEncodingAttributes->SetUINT32(
                CODECAPI_AVEncCommonQualityVsSpeed,
                un32QualityVsSpeed
                ), InternalLog(L"Failed to set CODECAPI_AVEncCommonQualityVsSpeed on Encoding Attributes"));
        }

        if(SUCCEEDED(m_pH264CustomParser->GetMeanBitRate(
            &un32MeanBitRate)))
        {
            _HRL(pEncodingAttributes->SetUINT32(
                CODECAPI_AVEncCommonMeanBitRate,
                un32MeanBitRate
                ), InternalLog(L"Failed to set CODECAPI_AVEncCommonMeanBitRate on Encoding Attributes"));
        }

        if(SUCCEEDED(m_pH264CustomParser->GetMaxBitRate(
            &un32MaxBitRate)))
        {
            _HRL(pEncodingAttributes->SetUINT32(
                CODECAPI_AVEncCommonMaxBitRate,
                un32MaxBitRate
                ), InternalLog(L"Failed to set CODECAPI_AVEncCommonMaxBitRate on Encoding Attributes"));
        }

        if(SUCCEEDED(m_pH264CustomParser->GetCABACEnable(
            &bCabacEnabled)))
        {
            _HRL(pEncodingAttributes->SetUINT32(
                CODECAPI_AVEncH264CABACEnable,
                (UINT32)bCabacEnabled
                ), InternalLog(L"Failed to set CODECAPI_AVEncH264CABACEnable on Encoding Attributes"));
        }

        if(SUCCEEDED(m_pH264CustomParser->GetSPSID(
            &un32SPSID)))
        {
            _HRL(pEncodingAttributes->SetUINT32(
                CODECAPI_AVEncH264SPSID,
                un32SPSID
                ), InternalLog(L"Failed to set CODECAPI_AVEncH264SPSID on Encoding Attributes"));
        }

        if(SUCCEEDED(m_pH264CustomParser->GetPPSID(
            &un32PPSID)))
        {
            _HRL(pEncodingAttributes->SetUINT32(
                CODECAPI_AVEncH264PPSID,
                un32PPSID
                ), InternalLog(L"Failed to set CODECAPI_AVEncH264PPSID on Encoding Attributes"));
        }

        if(SUCCEEDED(m_pH264CustomParser->GetMinQP(
            &un32MinQP)))
        {
            _HRL(pEncodingAttributes->SetUINT32(
                CODECAPI_AVEncVideoMinQP,
                un32MinQP
                ), InternalLog(L"Failed to set CODECAPI_AVEncVideoMinQP on Encoding Attributes"));
        }

        if(SUCCEEDED(m_pH264CustomParser->GetMaxQP(
            &un32MaxQP)))
        {
            _HRL(pEncodingAttributes->SetUINT32(
                CODECAPI_AVEncVideoMaxQP,
                un32MaxQP
                ), InternalLog(L"Failed to set CODECAPI_AVEncVideoMaxQP on Encoding Attributes"));
        }

        if(SUCCEEDED(m_pH264CustomParser->GetStaticTemporalLayerCount(
            &un32TemporalLayerCount)))
        {
            _HRL(pEncodingAttributes->SetUINT32(
                CODECAPI_AVEncVideoTemporalLayerCount,
                un32TemporalLayerCount
                ), InternalLog(L"Failed to set CODECAPI_AVEncVideoTemporalLayerCount on Encoding Attributes"));
        }

        _HRL(pEncodingAttributes->SetUINT32(
            CODECAPI_AVLowLatencyMode,
            (UINT32)(m_pConfigHelper->IsLowLatency())
            ), InternalLog(L"Failed to set CODECAPI_AVLowLatencyMode on Encoding Attributes"));

        UINT32 ulMaxLTRFrames = 0;
        UINT32 ulfTrustUntil = 0;
        UINT32 ulLTRSetting = 0;
        if(SUCCEEDED(m_pH264CustomParser->GetLTRInfo(&ulMaxLTRFrames, &ulfTrustUntil)))
        {
            ulLTRSetting = ulMaxLTRFrames | (ulfTrustUntil << 16);
            _HRL(pEncodingAttributes->SetUINT32(
                CODECAPI_AVEncVideoLTRBufferControl,
                ulLTRSetting
                ), InternalLog(L"Failed to set CODECAPI_AVEncVideoLTRBufferControl on Encoding Attributes"));
        }


        //remove randomness for Compare type tests as this affects the bit exact verification
        if(m_pConfigHelper->IsLowLatency() != FALSE || m_pH264CustomParser->IsLTRLossMode())
        {
            ULONG   ulWorkerThreads = 1;
            if(!m_pConfigHelper->IsCompareTest() &&  
                !m_pConfigHelper->IsHashOutputBuffersSet())
            {

                if(FAILED(m_pConfigHelper->GetNumWorkerThreads(
                    &ulWorkerThreads)))
                {
                    srand(m_pConfigHelper->GetRandomSeed());
                    ulWorkerThreads = (rand() % 16) + 1;
                }
            }
            _HRL(pEncodingAttributes->SetUINT32(
                CODECAPI_AVEncNumWorkerThreads,
                (UINT32)ulWorkerThreads
                ), InternalLog(L"Failed to set CODECAPI_AVEncNumWorkerThreads on Encoding Attributes"));
        } 

        //remove randomness for Compare type tests as this affects the bit exact verification
        if(m_pConfigHelper->IsLowLatency() == FALSE || m_pH264CustomParser->IsLTRLossMode())
        {
            if(m_pConfigHelper->IsCompareTest() ||  
                m_pConfigHelper->IsHashOutputBuffersSet())
            {
                ULONG   ulWorkerThreads = 1;
                _HRL(pEncodingAttributes->SetUINT32(
                    CODECAPI_AVEncNumWorkerThreads,
                    (UINT32)ulWorkerThreads
                    ), InternalLog(L"Failed to set CODECAPI_AVEncNumWorkerThreads on Encoding Attributes"));
            }

        }

        if(m_pH264CustomParser->IsMADEnabled()){
            _HRL(pEncodingAttributes->SetUINT32(
                CODECAPI_AVEncVideoMeanAbsoluteDifference,
                (UINT32)1
                ), InternalLog(L"Failed to set CODECAPI_AVEncVideoMeanAbsoluteDifference on Encoding Attributes")); 
        }
        UINT32 uiMaxNumRef = 0;
        if(SUCCEEDED(m_pH264CustomParser->GetMaxNumRefFrames(&uiMaxNumRef))){
            _HRL(pEncodingAttributes->SetUINT32(
                CODECAPI_AVEncVideoMaxNumRefFrame,
                (UINT32)uiMaxNumRef
                ), InternalLog(L"Failed to set CODECAPI_AVEncVideoMaxNumRefFrame on Encoding Attributes")); 
        }



    }while(false);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::VQARDCalculate(
    UINT32 run, 
    __inout DWORD* pdwBitrate, 
    __inout double* pdPSNR)
{
    HRESULT hr = S_OK;

    WEX::Common::String strInputPath;
    WEX::Common::String strOutputPath;
    IVisualQualityAnalysis* pVQA = NULL;
    EncoderQualityHelper::CalculationMethod calculationMethod;
    EncoderQualityHelper::CalculationResult  calculationResult;
    EncoderQualityHelper::ColorSubSpace colorSubSpace;
    EncoderQualityHelper::ColorPlane colorPlane;
    double dYVal = 0.0;
    DWORD dwBitrate = 0;

    do
    {
        _HR((m_pConfigHelper) ? S_OK : E_UNEXPECTED);
        _HR((m_pTopologyHelper) ? S_OK : E_UNEXPECTED);

        strInputPath = m_pConfigHelper->GetInputPath();
        if (strInputPath.IsEmpty())
        {
            _HR(E_INVALIDARG);
        }

        strOutputPath = m_pConfigHelper->GetOutputPath();
        if (strOutputPath.IsEmpty())
        {
            _HR(E_INVALIDARG);
        }

        _HR(CreateVQA(m_pLog, &pVQA));

        _HR(pVQA->SetSourceFile((const WCHAR*)strInputPath, NULL));
        _HR(pVQA->SetEncodedFile((const WCHAR*)strOutputPath, NULL));

        UINT64 ui64Duration = 0;
        UINT32 uiFPSNum = 0;
        UINT32 uiFPSDenom = 0;
        DWORD dwTotalFrames = 0;
        ULONGLONG ullOutputFileSize = 0L;

        // get output file size, in bytes
        BOOL fRes = FALSE;
        WIN32_FILE_ATTRIBUTE_DATA fileInfo = {0};
        fRes = GetFileAttributesEx((LPCWSTR)strOutputPath, GetFileExInfoStandard, (void*)&fileInfo);
        if(TRUE == fRes)
        {
            ullOutputFileSize = ((((ULONGLONG)fileInfo.nFileSizeHigh)<<32)|(ULONGLONG)(fileInfo.nFileSizeLow));
        }

        if(S_OK == m_pTopologyHelper->GetPresentationDuration(&ui64Duration) &&
            S_OK == m_pTopologyHelper->GetSourceFrameRate(&uiFPSNum, &uiFPSDenom) && uiFPSDenom > 0)
        {
            InternalLog(L"Presentation duration is %I64u  (in 100ns) or %I64u ms", ui64Duration, ui64Duration/10000);
            InternalLog(L"Frame rate is %lu / %lu or ~ %f fps", uiFPSNum, uiFPSDenom, (double)uiFPSNum/(double)uiFPSDenom);
            double fps = (double)uiFPSNum / (double) uiFPSDenom;
            double duration = (double)(ui64Duration)/10000000.0;
            dwTotalFrames = (DWORD)(fps * duration + 0.5);
            InternalLog(L"Estimated number of frames: %lu", dwTotalFrames);

        }

        if(ui64Duration && ullOutputFileSize)
        {
            dwBitrate = (DWORD)((ullOutputFileSize * 80000000L)/(ui64Duration)); // 8 is 8 bits per byte; 10000000L converts 100ns to sec            
        }

        calculationMethod = EncoderQualityHelper::CalculationMethod::PSNR;
        calculationResult = EncoderQualityHelper::CalculationResult::Average;
        colorSubSpace = EncoderQualityHelper::ColorSubSpace::Luma;
        colorPlane = EncoderQualityHelper::ColorPlane::YComponent;

        _HR(pVQA->EnableCalculationMethod(
            calculationMethod,
            calculationResult,
            colorSubSpace));

        _HR(pVQA->SetFrameFrequency(1));

        if(dwTotalFrames)
        {
            (void)pVQA->SetTotalFramesEstimate(dwTotalFrames);
        }
        /*
        WCHAR wcDeltaPath[MAX_PATH] = {0};

        StringCchCopyW(wcDeltaPath, MAX_PATH, (LPCWSTR)strOutputPath);
        StringCchCatW(wcDeltaPath, MAX_PATH, L"_delta.yuv");

        _HR(pVQA->EnableVisualization(
        EncoderQualityHelper::SaveDelta,
        wcDeltaPath, 
        EncoderQualityHelper::AbsoluteDifferenceSquared));
        */                        

        WCHAR wcCSV[MAX_PATH] = {0};
        StringCchPrintfW(wcCSV, MAX_PATH, L"VQA_pass%d.csv", run);

        (void)pVQA->GenerateCSV(wcCSV);

        DWORD dwFramesProcessed = 0L;

        _HR(pVQA->RunMeasurements(&dwFramesProcessed));

        _HR(pVQA->GetResult(
            calculationMethod,
            calculationResult,
            colorPlane,
            &dYVal));

    } while (FALSE);

    InternalLog(L"Quality analysis: bitrate = %lu bps", dwBitrate);
    InternalLog(L"Average PSNR, Y-component: %-05.2f", dYVal);

    *pdwBitrate = dwBitrate;
    *pdPSNR = dYVal; //average PSNR, Y-component

    SAFE_RELEASE(pVQA);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::VQAVerify()
{
    HRESULT hr = S_OK;
    WEX::Common::String strInputPath;
    WEX::Common::String strOutputPath;
    IVisualQualityAnalysis* pVQA = NULL;
    UINT32 uiVQAMethod = 0;
    EncoderQualityHelper::CalculationMethod calculationMethod[3]; // RMSE, PSNR, DSIM
    UINT32 uiCalculationResults[3]; // RMSE, PSNR, DSIM
    EncoderQualityHelper::ColorSubSpace colorSubSpace[3]; // RMSE, PSNR, DSIM
    UINT32 dwFreq[3] = {0, 0, 0}; // RMSE, PSNR, DSIM
    double dYVal[3][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}; // RMSE(max, min, avg), PSNR(max, min, avg), DSIM(max, min, avg)
    double dUVal[3][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}; // RMSE(max, min, avg), PSNR(max, min, avg), DSIM(max, min, avg)
    double dVVal[3][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}; // RMSE(max, min, avg), PSNR(max, min, avg), DSIM(max, min, avg)
    DWORD dwFrame[3][2] = {{0, 0}, {0, 0}, {0, 0}}; // RMSE(max, min), PSNR(max, min), DSIM(max, min)
    DWORD dwBitrate = 0;
    double dLumaThreshold[3] = {0.0, 0.0, 0.0}; // RMSE, PSNR, DSIM
    double dChromaThreshold[3] = {0.0, 0.0, 0.0}; // RMSE, PSNR, DSIM
    UINT32 uiCalculationMethodCount = 0;

    do
    {
        _HR((m_pConfigHelper) ? S_OK : E_UNEXPECTED);
        _HR((m_pTopologyHelper || m_pSampleProcessor) ? S_OK : E_UNEXPECTED);

        strInputPath = m_pConfigHelper->GetInputPath();
        if (strInputPath.IsEmpty())
        {
            _HR(E_INVALIDARG);
        }

        strOutputPath = m_strOutputFile;
        if (strOutputPath.IsEmpty())
        {
            _HR(E_INVALIDARG);
        }

        _HR(CreateVQA(m_pLog, &pVQA));

        _HR(pVQA->SetSourceFile((const WCHAR*)strInputPath, NULL));
        _HR(pVQA->SetEncodedFile((const WCHAR*)strOutputPath, NULL));

        // get the VQA methods mask.
        m_pConfigHelper->GetVQAMethods(&uiVQAMethod);

        if (EncoderQualityHelper::CalculationMethod::RMSE == (EncoderQualityHelper::CalculationMethod)uiVQAMethod)
        {
            calculationMethod[0] = EncoderQualityHelper::CalculationMethod::RMSE;
            uiCalculationMethodCount = 1;
        }
        else if (EncoderQualityHelper::CalculationMethod::PSNR == (EncoderQualityHelper::CalculationMethod)uiVQAMethod)
        {
            calculationMethod[0] = EncoderQualityHelper::CalculationMethod::RMSE;
            calculationMethod[1] = EncoderQualityHelper::CalculationMethod::PSNR;
            uiCalculationMethodCount = 2;
        }
        else if (EncoderQualityHelper::CalculationMethod::DSIM == (EncoderQualityHelper::CalculationMethod)uiVQAMethod)
        {
            calculationMethod[0] = EncoderQualityHelper::CalculationMethod::RMSE;
            calculationMethod[1] = EncoderQualityHelper::CalculationMethod::PSNR;
            calculationMethod[2] = EncoderQualityHelper::CalculationMethod::DSIM;
            uiCalculationMethodCount = 3;
        }

        UINT64 ui64Duration = 0;
        UINT32 uiFPSNum = 0;
        UINT32 uiFPSDenom = 0;
        DWORD dwTotalFrames = 0;
        ULONGLONG ullOutputFileSize = 0L;

        // get output file size, in bytes
        BOOL fRes = FALSE;
        WIN32_FILE_ATTRIBUTE_DATA fileInfo = {0};
        fRes = GetFileAttributesEx((LPCWSTR)strOutputPath, GetFileExInfoStandard, (void*)&fileInfo);
        if(TRUE == fRes)
        {
            ullOutputFileSize = ((((ULONGLONG)fileInfo.nFileSizeHigh)<<32)|(ULONGLONG)(fileInfo.nFileSizeLow));
        }

        BOOL bTopologyHelper = FALSE;
        BOOL bDirectSampleProcessor = FALSE;

        bTopologyHelper = m_pTopologyHelper && 
            S_OK == m_pTopologyHelper->GetPresentationDuration(&ui64Duration) &&
            S_OK == m_pTopologyHelper->GetSourceFrameRate(&uiFPSNum, &uiFPSDenom) && uiFPSDenom > 0;

        if(!bTopologyHelper)
        {
            bDirectSampleProcessor = m_pSampleProcessor && 
                S_OK == m_pSampleProcessor->GetPresentationDuration(&ui64Duration) && 
                S_OK == m_pSampleProcessor->GetSourceFrameRate(&uiFPSNum, &uiFPSDenom) && uiFPSDenom > 0;
        }
        if (bTopologyHelper || 
            bDirectSampleProcessor)
        {
            InternalLog(L"Presentation duration is %I64u  (in 100ns) or %I64u ms", ui64Duration, ui64Duration/10000);
            InternalLog(L"Frame rate is %lu / %lu or ~ %f fps", uiFPSNum, uiFPSDenom, (double)uiFPSNum/(double)uiFPSDenom);
            double fps = (double)uiFPSNum / (double) uiFPSDenom;
            double duration = (double)(ui64Duration)/10000000.0;
            dwTotalFrames = (DWORD)(fps * duration + 0.5);
            InternalLog(L"Estimated number of frames: %lu", dwTotalFrames);

        }

        if(ui64Duration && ullOutputFileSize)
        {
            dwBitrate = (DWORD)((ullOutputFileSize * 80000000L)/(ui64Duration)); // 8 is 8 bits per byte; 10000000L converts 100ns to sec            
        }

        for (UINT32 ui = 0; ui < uiCalculationMethodCount; ui++)
        {
            m_pConfigHelper->GetVQAResults(calculationMethod[ui], &uiCalculationResults[ui]);

            _HR(m_pConfigHelper->GetVQALumaThreshold(calculationMethod[ui], &dLumaThreshold[ui]));
            hr = m_pConfigHelper->GetVQAChromaThreshold(calculationMethod[ui], &dChromaThreshold[ui]);
            if (SUCCEEDED(hr))
            {
                colorSubSpace[ui] = EncoderQualityHelper::ColorSubSpace::Chroma;
            }
            else
            {
                colorSubSpace[ui] = EncoderQualityHelper::ColorSubSpace::Luma;
            }

            _HR(pVQA->EnableCalculationMethod(
                calculationMethod[ui],
                uiCalculationResults[ui],
                colorSubSpace[ui]));

            _HR(m_pConfigHelper->GetVQASamplingFrequency(&dwFreq[ui]));

            _HR(pVQA->SetFrameFrequency(dwFreq[ui]));
        }

        if(dwTotalFrames)
        {
            (void)pVQA->SetTotalFramesEstimate(dwTotalFrames);
        }

        WCHAR wcCSV[MAX_PATH] = {0};
        StringCchPrintfW(wcCSV, MAX_PATH, L"VQA.csv");

        (void)pVQA->GenerateCSV(wcCSV);

        DWORD dwFramesProcessed = 0L;

        _HR(pVQA->RunMeasurements(&dwFramesProcessed));

        EncoderQualityHelper::CalculationResult calculationResult;

        for (UINT32 ui = 0; ui < uiCalculationMethodCount; ui++)
        {
            // Maximum values
            if (uiCalculationResults[ui] & EncoderQualityHelper::CalculationResult::Maximum)
            {
                calculationResult = EncoderQualityHelper::CalculationResult::Maximum;

                // Luma is always present
                _HR(pVQA->GetResult(
                    calculationMethod[ui],
                    calculationResult,
                    EncoderQualityHelper::ColorPlane::YComponent,
                    &dYVal[ui][0], 
                    &dwFrame[ui][0]));
                if (EncoderQualityHelper::ColorSubSpace::Chroma & colorSubSpace[ui])
                {
                    _HR(pVQA->GetResult(
                        calculationMethod[ui],
                        calculationResult,
                        EncoderQualityHelper::ColorPlane::UComponent,
                        &dUVal[ui][0], 
                        &dwFrame[ui][0]));
                    _HR(pVQA->GetResult(
                        calculationMethod[ui],
                        calculationResult,
                        EncoderQualityHelper::ColorPlane::VComponent,
                        &dVVal[ui][0], 
                        &dwFrame[ui][0]));
                }
            }

            // Minimum values
            if (uiCalculationResults[ui] & EncoderQualityHelper::CalculationResult::Minimum)
            {
                calculationResult = EncoderQualityHelper::CalculationResult::Minimum;

                // Luma is always present
                _HR(pVQA->GetResult(
                    calculationMethod[ui],
                    calculationResult,
                    EncoderQualityHelper::ColorPlane::YComponent,
                    &dYVal[ui][1], 
                    &dwFrame[ui][1]));
                if (EncoderQualityHelper::ColorSubSpace::Chroma & colorSubSpace[ui])
                {
                    _HR(pVQA->GetResult(
                        calculationMethod[ui],
                        calculationResult,
                        EncoderQualityHelper::ColorPlane::UComponent,
                        &dUVal[ui][1], 
                        &dwFrame[ui][1]));
                    _HR(pVQA->GetResult(
                        calculationMethod[ui],
                        calculationResult,
                        EncoderQualityHelper::ColorPlane::VComponent,
                        &dVVal[ui][1], 
                        &dwFrame[ui][1]));
                }
            }

            // Average values
            if (uiCalculationResults[ui] & EncoderQualityHelper::CalculationResult::Average)
            {
                calculationResult = EncoderQualityHelper::CalculationResult::Average;

                // Luma is always present
                _HR(pVQA->GetResult(
                    calculationMethod[ui],
                    calculationResult,
                    EncoderQualityHelper::ColorPlane::YComponent,
                    &dYVal[ui][2]));
                if (EncoderQualityHelper::ColorSubSpace::Chroma & colorSubSpace[ui])
                {
                    _HR(pVQA->GetResult(
                        calculationMethod[ui],
                        calculationResult,
                        EncoderQualityHelper::ColorPlane::UComponent,
                        &dUVal[ui][2]));
                    _HR(pVQA->GetResult(
                        calculationMethod[ui],
                        calculationResult,
                        EncoderQualityHelper::ColorPlane::VComponent,
                        &dVVal[ui][2]));
                }
            }
        }

        // checking against thresholds
        // ui goes through RMSE, PSNR, DSIM
        // uj goes through maximum, minimum, average
        InternalLog(L"\nQuality Analysis Results: ");
        for (UINT32 ui = 0; ui < 3; ui++)
        {
            for (UINT32 uj = 0; uj < 3; uj++)
            {
                // for DSIM it's top threshold
                if (2 == ui)
                {
                    if (dYVal[ui][uj] > dLumaThreshold[ui])
                    {
                        InternalLog(
                            L"DSIM Y-component doesn't meet the threshold %-05.2f > %-05.2f", 
                            dYVal[ui][uj], 
                            dLumaThreshold[ui]);
                        _HR(E_FAIL);
                    }
                    if (dUVal[ui][uj] > dChromaThreshold[ui] || 
                        dVVal[ui][uj] > dChromaThreshold[ui])
                    {
                        InternalLog(
                            L"DSIM U/V-components don't meet the threshold %-05.2f > %-05.2f or %-05.2f > %-05.2f", 
                            dUVal[ui][uj], 
                            dChromaThreshold[ui], 
                            dVVal[ui][uj], 
                            dChromaThreshold[ui]);
                        _HR(E_FAIL);
                    }
                }
                // for RMSE and PSNR it's bottom threshold
                else
                {
                    if (dYVal[ui][uj] < dLumaThreshold[ui])
                    {
                        InternalLog(
                            L"%s Y-component doesn't meet the threshold %-05.2f < %-05.2f", 
                            (0 == ui ? L"RMSE" : L"PSNR"), 
                            dYVal[ui][uj], 
                            dLumaThreshold[ui]);
                        _HR(E_FAIL);
                    }
                    if (dUVal[ui][uj] < dChromaThreshold[ui] || 
                        dVVal[ui][uj] < dChromaThreshold[ui])
                    {
                        InternalLog(
                            L"%s U/V-components don't meet the threshold %-05.2f < %-05.2f or %-05.2f < %-05.2f", 
                            (0 == ui ? L"RMSE" : L"PSNR"), 
                            dUVal[ui][uj], 
                            dChromaThreshold[ui], 
                            dVVal[ui][uj], 
                            dChromaThreshold[ui]);
                        _HR(E_FAIL);
                    }
                }
            }
            if (E_FAIL == hr)
            {
                _HR(E_FAIL);
            }
        }

    } while (FALSE);

    InternalLog(L"Quality analysis: bitrate = %lu bps", dwBitrate);

    for (UINT32 ui = 0; ui < uiCalculationMethodCount; ui++)
    {
        if (0 == ui)
        {
            InternalLog(L"\nCalculation method: RMSE");
        }
        else if (1 == ui)
        {
            InternalLog(L"\nCalculation method: PSNR");
        }
        else if (2 == ui)
        {
            InternalLog(L"\nCalculation method: DSIM");
        }

        InternalLog(L"Luma threshold: %-05.2f", dLumaThreshold[ui]);
        InternalLog(L"Chroma threshold: %-05.2f", dChromaThreshold[ui]);

        InternalLog(L"            Y      U      V      Frame#");
        InternalLog(L"Max         %-05.2f  %-05.2f  %-05.2f  %ld", dYVal[ui][0], dUVal[ui][0], dVVal[ui][0], dwFrame[ui][0]);
        InternalLog(L"Min         %-05.2f  %-05.2f  %-05.2f  %ld", dYVal[ui][1], dUVal[ui][1], dVVal[ui][1], dwFrame[ui][1]);
        InternalLog(L"Average     %-05.2f  %-05.2f  %-05.2f", dYVal[ui][2], dUVal[ui][2], dVVal[ui][2]);

        if (uiCalculationMethodCount >= 2)
        {
            InternalLog(L"\nAverage PSNR value for Y-component: %-05.2f\n", dYVal[1][2]);

            // This is per Mohammad's request, dumping average PSNR for Y-component to a test file for qualperf analysis
            WCHAR pwszPSNR[MAX_PATH] = L"";
            swprintf_s(pwszPSNR, MAX_PATH, L"echo %-05.2f > PSNR.txt", dYVal[1][2]);
            _wsystem(pwszPSNR);
        }
    }

    SAFE_RELEASE(pVQA);

    return hr;
}

HRESULT
    CH264EncodeVerify::IsServerOS(
    __out BOOL *pbServerOS)
{
    HRESULT hr = S_OK;
    HKEY hSubKey = NULL;
    LONG lRetVal = 0;
    DWORD dwType = REG_NONE;
    BYTE* pbData = NULL;
    DWORD dwDataLen = 0;
    WCHAR *pwcSubStr = NULL;

    do
    {
        lRetVal = RegOpenKeyEx(
            HKEY_LOCAL_MACHINE, 
            L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 
            NULL, 
            KEY_QUERY_VALUE, 
            &hSubKey);
        if (ERROR_SUCCESS != lRetVal)
        {
            _HR(HRESULT_FROM_WIN32(lRetVal));
        }

        // Get size of string
        lRetVal = RegQueryValueEx(hSubKey, L"ProductName", NULL, &dwType, NULL, &dwDataLen);
        if (ERROR_SUCCESS != lRetVal)
        {
            _HR(HRESULT_FROM_WIN32(lRetVal));
        }
        pbData = new BYTE[dwDataLen];
        if(NULL != pbData)
        {
            lRetVal = RegQueryValueEx(hSubKey, L"ProductName", NULL, &dwType, pbData, &dwDataLen);
            if (ERROR_SUCCESS != lRetVal)
            {
                _HR(HRESULT_FROM_WIN32(lRetVal));
            }

            // Find substring of "Server" in L"ProductName"
            if (REG_SZ == dwType)
            {
                pwcSubStr = wcsstr((LPWSTR)pbData, L"Server");
                if (NULL != pwcSubStr)
                {
                    *pbServerOS = TRUE;
                }
            }
        }
    } while (FALSE);

    if (hSubKey)
    {
        RegCloseKey(hSubKey);
        hSubKey = NULL;
    }
    SAFE_ARRAY_DELETE(pbData);

    return hr;
}

//-----------------------------------------------------------------------------
// Reading H.264 encoding settings from TAEF test xml by using generic 
// config helper (ITAEFConfigHelper) and a custom config helper for H.264 
// (ITAEFConfigCustomParser), then setting output IMFAttributes and 
// ICodecAPI properties based on the read values.
//-----------------------------------------------------------------------------

STDMETHODIMP
    CConfigureH264Encoder::Configure(
    __in IMFTransform* pMFT,
    __in IMFMediaType* pUpstreamType,
    __inout IMFMediaType** ppInputType,
    __inout IMFMediaType** ppOutputType)
{
    HRESULT hr = S_OK;

    IMFMediaType* pOutputType = NULL;
    IMFMediaType* pUpstreamTypeFuzz = NULL;
    IMFMediaType* pEncoderProvidedOutputType = NULL;
    ICodecAPI *pCodecAPI = NULL;
    IMFAttributes *pMFTAttributes = NULL;
    UINT32 uintVal1 = 0;
    UINT32 uintVal2 = 0;
    VARIANTARG va;
    UINT32 uintBitrate = 0;
    CH264ConfigParser *pH264CustomParser = NULL;
    UINT32 uiFRNumerator = 0;
    UINT32 uiFRDenominator = 0;
    WEX::Common::String reconFilePath = L"";

    ULONG ulSliceControlMode = 0;
    ULONG ulSliceControlSize = 0;
    ULONG ulIgnoredSliceControlMode = 0;
    ULONG ulDynamicInitialSliceControlSize = 0;

    if(!pMFT || !pUpstreamType || !ppInputType || !ppOutputType)
    {
        return E_POINTER;
    }

    do
    {
        InternalLog(L"*** H.264 Encoder upstream type:");
        DescribeMediaType(pUpstreamType, m_pLog);

        if (TAEFConfigHelper::FuzzMediaType == m_pConfig->GetFuzzType())
        {
            // Fuzz upstream media type
            _HRL(MFCreateMediaType(&pUpstreamTypeFuzz),InternalLog(L"MFCreateMediaType() failed with hr = 0x%08X", hr));
#ifndef MINWIN
            hr = ::FuzzMFAttrs(
                pUpstreamType, 
                pUpstreamTypeFuzz, 
                FuzzHelper::eFuzzSameSetRandomValues, 
                m_pLog);
#else
            hr = E_NOTIMPL;
#endif //#ifndef MINWIN
            // Ignore failures in fuzzing, just log the failures and proceed
            if (S_OK != hr)
            {
                InternalLog(L"FuzzMediaType() failed with hr = 0x%08X", hr);
                hr = S_OK;
            }

            InternalLog(L"*** H.264 Encoder upstream type after fuzzing:");
            DescribeMediaType(pUpstreamTypeFuzz, m_pLog);

            hr = pUpstreamTypeFuzz->CopyAllItems(pUpstreamType);

            // Ignore failures in copying, just log the failures and proceed
            if (S_OK != hr)
            {
                InternalLog(L"pUpstreamTypeFuzz->CopyAllItems() failed with hr = 0x%08X", hr);
                hr = S_OK;
            }
            SAFE_RELEASE(pUpstreamTypeFuzz);
        }

        // Get H.264 specific properties through ITAEFConfigCustomParser
        _HR(m_pConfig->GetConfigCustomParser((ITAEFConfigCustomParser **)&pH264CustomParser));

        //---------------------------
        // Set output media type attributes
        //---------------------------
        SAFE_RELEASE(pOutputType);

        _HR( MFCreateMediaType(&pOutputType));

        // Major type
        MFTopologyHelper::MajorTypes majortypes = MFTopologyHelper::NoAudioVideo;
        majortypes = m_pConfig->GetMajorTypes();
        if (majortypes == MFTopologyHelper::BothAudioVideo || majortypes == MFTopologyHelper::Video)
        {
            _HRL(pOutputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video), 
                InternalLog(L"Failed IMFMediaType::SetGUID(MF_MT_MAJOR_TYPE, ...) with hr = 0x%08X", hr));
        }
        else
        {
            _HR(E_INVALIDARG);
        }

        // Subtype


        CLSID clsidVideoEncoder = m_pConfig->GetVideoEncoder();
        if (CLSID_H264MFTEncoder ==  clsidVideoEncoder)
        {
            _HRL(pOutputType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264), 
                InternalLog(L"Failed IMFMediaType::SetGUID(MF_MT_SUBTYPE, ...) with hr = 0x%08X", hr));
        }
        else if (CLSID_H265MFTEncoder ==  clsidVideoEncoder)
        {
            _HRL(pOutputType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_HEVC), 
                InternalLog(L"Failed IMFMediaType::SetGUID(MF_MT_SUBTYPE, ...) with hr = 0x%08X", hr));
        }
        else if (CLSID_CMSVPXEncoderMFT == clsidVideoEncoder)
        {
            _HRL(pOutputType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_VP90),
                InternalLog(L"Failed IMFMediaType::SetGUID(MF_MT_SUBTYPE, ...) with hr = 0x%08X", hr));
        }
        else
        {
            _HR(E_INVALIDARG);
        }

        // Interlace mode
        UINT32 uintInterlaceMode = MFGetAttributeUINT32(pUpstreamType, MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
        if(m_pConfig->GetHWMode() == MFTopologyHelper::HWMode::HW)
        {
            // Hardware should always be progressive
            uintInterlaceMode   = MFVideoInterlace_Progressive;
        }
        _HRL(pOutputType->SetUINT32(MF_MT_INTERLACE_MODE, uintInterlaceMode), 
            InternalLog(L"Failed IMFMediaType::SetUINT32(MF_MT_INTERLACE_MODE, ...) with hr = 0x%08X", hr));

        // copy frame size from input type
        _HRL(MFGetAttributeSize(pUpstreamType, MF_MT_FRAME_SIZE, &uintVal1, &uintVal2), 
            InternalLog(L"Failed MFGetAttributeSize(..., MF_MT_FRAME_SIZE, ...) with hr = 0x%08X", hr));
        _HRL(MFSetAttributeSize(pOutputType, MF_MT_FRAME_SIZE, uintVal1, uintVal2), 
            InternalLog(L"Failed MFSetAttributeSize(..., MF_MT_FRAME_SIZE, ...) with hr = 0x%08X", hr));

        // copy frame rate from input type
        _HRL(MFGetAttributeRatio(pUpstreamType, MF_MT_FRAME_RATE, &uintVal1, &uintVal2), 
            InternalLog(L"Failed MFGetAttributeRatio(..., MF_MT_FRAME_RATE, ...) with hr = 0x%08X", hr));
        uiFRNumerator = uintVal1;
        uiFRDenominator = uintVal2; 
        _HRL(MFSetAttributeRatio(pOutputType, MF_MT_FRAME_RATE, uintVal1, uintVal2), 
            InternalLog(L"Failed MFSetAttributeRatio(..., MF_MT_FRAME_RATE, ...) with hr = 0x%08X", hr));

        // copy pixel aspect ratio from input type
        _HRL(MFGetAttributeRatio(pUpstreamType, MF_MT_PIXEL_ASPECT_RATIO, &uintVal1, &uintVal2), 
            InternalLog(L"Failed MFGetAttributeRatio(..., MF_MT_PIXEL_ASPECT_RATIO, ...) with hr = 0x%08X", hr));
        _HRL(MFSetAttributeRatio(pOutputType, MF_MT_PIXEL_ASPECT_RATIO, uintVal1, uintVal2), 
            InternalLog(L"Failed MFSetAttributeRatio(..., MF_MT_PIXEL_ASPECT_RATIO, ...) with hr = 0x%08X", hr));

        // set color nominal range based on input type
        MFNominalRange eNomRange = MFNominalRange_16_235;
        hr = pUpstreamType->GetUINT32(MF_MT_VIDEO_NOMINAL_RANGE, (UINT32*)&eNomRange);
        if(FAILED(hr)){
            InternalLog(L"MF_MT_VIDEO_NOMINAL_RANGE not set on input so using MFNominalRange_16_235 for output", hr);
        }
        _HRL(pOutputType->SetUINT32(MF_MT_VIDEO_NOMINAL_RANGE,eNomRange),
            InternalLog(L"Failed to set MF_MT_VIDEO_NOMINAL_RANGE with hr = 0x%08X", hr));
        m_pConfig->SetNominalRange(eNomRange);

        if (CLSID_H264MFTEncoder ==  clsidVideoEncoder)
        {
            // set profile
            eAVEncH264VProfile h264Profile = eAVEncH264VProfile_unknown;
            WEX::Common::String strEncodingProfile = m_pConfig->GetEncodingProfile();
            CH264EncHelper::ConvertEncodingProfile(strEncodingProfile, &h264Profile);
            strEncodingProfile.ReleaseBuffer();
            _HRL(pOutputType->SetUINT32(MF_MT_MPEG2_PROFILE, h264Profile), 
                InternalLog(L"Failed IMFMediaType::SetUINT32(MF_MT_MPEG2_PROFILE, ...) with hr = 0x%08X", hr));

            // set level
            WEX::Common::String strEncodingLevel = m_pConfig->GetEncodingLevel();
            UINT32 ulEncodingLevel = 0;
            CH264EncHelper::ConvertEncodingLevel(strEncodingLevel, &ulEncodingLevel);
            strEncodingLevel.ReleaseBuffer();
            _HRL(pOutputType->SetUINT32(MF_MT_MPEG2_LEVEL, (eAVEncH264VLevel)ulEncodingLevel), 
                InternalLog(L"Failed IMFMediaType::SetUINT32(MF_MT_MPEG2_LEVEL, ...) with hr = 0x%08X", hr));
        } else if (CLSID_H265MFTEncoder ==  clsidVideoEncoder)
        {
            // set profile
            eAVEncH265VProfile h265Profile = eAVEncH265VProfile_unknown;
            WEX::Common::String strEncodingProfile = m_pConfig->GetEncodingProfile();
            CH264EncHelper::ConvertEncodingProfile_HEVC(strEncodingProfile, &h265Profile);
            strEncodingProfile.ReleaseBuffer();
            _HRL(pOutputType->SetUINT32(MF_MT_MPEG2_PROFILE, h265Profile), 
                InternalLog(L"Failed IMFMediaType::SetUINT32(MF_MT_MPEG2_PROFILE, ...) with hr = 0x%08X", hr));

            // set level
            WEX::Common::String strEncodingLevel = m_pConfig->GetEncodingLevel();
            UINT32 ulEncodingLevel = 0;
            CH264EncHelper::ConvertEncodingLevel(strEncodingLevel, &ulEncodingLevel);
            strEncodingLevel.ReleaseBuffer();
            _HRL(pOutputType->SetUINT32(MF_MT_MPEG2_LEVEL, (eAVEncH265VLevel)ulEncodingLevel), 
                InternalLog(L"Failed IMFMediaType::SetUINT32(MF_MT_MPEG2_LEVEL, ...) with hr = 0x%08X", hr));
        }
        else if (CLSID_CMSVPXEncoderMFT == clsidVideoEncoder)
        {
            // set profile
            eAVEncVP9VProfile VPXProfile = eAVEncVP9VProfile_unknown;
            WEX::Common::String strEncodingProfile = m_pConfig->GetEncodingProfile();
            CH264EncHelper::ConvertEncodingProfile_VPX(strEncodingProfile, &VPXProfile);
            strEncodingProfile.ReleaseBuffer();
            _HRL(pOutputType->SetUINT32(MF_MT_MPEG2_PROFILE, VPXProfile),
                InternalLog(L"Failed IMFMediaType::SetUINT32(MF_MT_MPEG2_PROFILE, ...) with hr = 0x%08X", hr));
        }
        else 
        {
            _HR(E_INVALIDARG);
        }

        // RD curve scenario (variation by bitrate)
        UINT32 uintRDBitrate = 0;
        BOOL bIsQuality = FALSE;
        RDCurveType rdType = RDCURVE_NULL;
        m_pConfig->GetRDCurveType(&rdType);
        //if (m_pConfig->IsRDCurve(NULL, &bIsQuality) && FALSE == bIsQuality)
        if( rdType == RDCURVE_BITRATE)
        {
            if (S_OK == m_pConfig->GetRDVariationByBitrate(m_cPass, &uintRDBitrate))
            {
                uintBitrate = uintRDBitrate;
            }
            else
            {
                InternalLog(L"Failed ITAEFConfigHelper::GetRDVariationByBitrate() with hr = 0x%08X", hr);
            }
        }
        else if(rdType == RDCURVE_QP || rdType == RDCURVE_QUALITY)
        {
            /*This bitrate is arbitrary - only set because the MFT needs it to start*/
            uintBitrate = QP_RDCURVE_BITRATE;
        }
        // Not RD curve scenario
        else
        {
            // MF_MT_AVG_BITRATE must be set
            hr = m_pConfig->GetBitrate(&uintBitrate);
            if (S_OK != hr)
            {
                InternalLog(L"Failed ITAEFConfigHelper::GetBitrate() with hr = 0x%08X", hr);
            }
        }

        // set MF_MT_AVG_BITRATE
        if (SUCCEEDED(hr))
        {
            _HRL(pOutputType->SetUINT32(MF_MT_AVG_BITRATE, uintBitrate), 
                InternalLog(L"Failed IMFMediaType::SetUINT32(MF_MT_AVG_BITRATE, ...) with hr = 0x%08X", hr));
        }

        InternalLog(L"*** H.264 Encoder output type:");
        DescribeMediaType(pOutputType, m_pLog);

        //---------------------------------------------------------
        // Set generic output ICodecAPI properties through ITAEFConfigHelper
        //---------------------------------------------------------
        SAFE_RELEASE(pCodecAPI);

        _HR(pMFT->QueryInterface(IID_ICodecAPI, (LPVOID*)&pCodecAPI));

        SAFE_RELEASE(pMFTAttributes);
        hr = pMFT->GetAttributes(&pMFTAttributes);
        if( hr == E_NOTIMPL){
            pMFTAttributes = NULL;
            InternalLog(L"Failed IMFTransform::GetAttributed with E_NOTIMPL, not using it to set configuration");
        }else if(FAILED(hr)){
            break;
        }

        //Set private attribute to dump the reconstructed file

        if(m_pConfig->IsReconTest() )
        {
            reconFilePath = m_pConfig->GetReconFilePath();

            if(reconFilePath.IsEmpty())
            {
                InternalLog(L"Reconstructed file path is empty");
                break;
            }
            _HRL(pMFTAttributes->SetString(H264ENC_DUMP_RECON, (LPCWSTR)reconFilePath), InternalLog(L"Failed to set the reconstructed file name"));
        }

        if (m_pConfig->IsRunAPITests())
        {
            // Test GetParamaterValue and GetParameterRange APIs
            hr = VerifyAPI_GetParameterValues(pCodecAPI);
            if (FAILED(hr))
            {
                InternalLog(L"Failed API support tests - VerifyAPI_GetParameterValues");
                break;
            }
            hr = VerifyAPI_GetParameterRange(pCodecAPI);
            if (FAILED(hr))
            {
                InternalLog(L"Failed API support tests - VerifyAPI_GetParameterRange");
                break;
            }
        }

        // Set CODECAPI_AVEncCommonRateControlMode 
        VariantInit(&va);    
        va.vt = VT_UI4;
        va.uintVal = (UINT32)(eAVEncCommonRateControlMode)m_pConfig->GetBitrateControlMode();

        // No checking for va.uintVal is needed as the value of 0 corresponds to CBR
        _HRL(pCodecAPI->SetValue(&CODECAPI_AVEncCommonRateControlMode , (VARIANT*)&va), 
            InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncCommonRateControlMode, ...) with hr = 0x%08X", hr));

        VariantClear(&va);

        //Need to set GOPSize before TL so the encoder can adjust as necessary
        // Set CODECAPI_AVEncMPVGOPSize (in frames)
        UINT32 ulGOPSize = 0;
        VariantInit(&va);
        va.vt = VT_UI4;
        hr = m_pConfig->GetGOPSize(&ulGOPSize);
        if (SUCCEEDED(hr) )
        {
            va.uintVal = ulGOPSize;
            _HRL(pCodecAPI->SetValue(&CODECAPI_AVEncMPVGOPSize, (VARIANT*)&va), 
                InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncMPVGOPSize, ...) with hr = 0x%08X", hr));
        }
        else
        {
            InternalLog(L"GOP size not set so using 30 by default", hr);
            va.uintVal = 30;
            _HRL(pCodecAPI->SetValue(&CODECAPI_AVEncMPVGOPSize, (VARIANT*)&va), 
                InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncMPVGOPSize, ...) with hr = 0x%08X", hr));
        }
        VariantClear(&va);


        //Let's set temporal layer count right off the bat so that we can SelectLayer for quality/QP
        // Set CODECAPI_AVEncVideoTemporalLayerCount
        UINT32 ulLayerCountConfig = 1;
        VariantInit(&va);
        va.vt = VT_UI4;

        if(S_OK == pCodecAPI->IsSupported(&CODECAPI_AVEncVideoTemporalLayerCount)){
            VARIANTARG va_min;
            VARIANTARG va_max;
            VARIANTARG va_delta;
            VariantInit(&va_min);
            VariantInit(&va_max);
            VariantInit(&va_delta);
            ULONG ValuesCount;
            if(S_OK == (hr = pCodecAPI->GetParameterRange(&CODECAPI_AVEncVideoTemporalLayerCount,&va_min,&va_max,&va_delta))){
                pH264CustomParser->m_uiMaxTemporalLayers = va_max.uintVal;
                wprintf(L"Max layers: %d\n",pH264CustomParser->m_uiMaxTemporalLayers);
            }
            VariantClear(&va_min);
            VariantClear(&va_max);
            VariantClear(&va_delta);

            hr = pH264CustomParser->GetStaticTemporalLayerCount(&ulLayerCountConfig);
            if ((SUCCEEDED(hr) || pH264CustomParser->IsDynamicTemporalSet()))
            {
                va.uintVal = ulLayerCountConfig;
                _HRL(pCodecAPI->SetValue(&CODECAPI_AVEncVideoTemporalLayerCount, (VARIANT*)&va), 
                    InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncVideoTemporalLayerCount, ...) with hr = 0x%08X", hr));
            }
        }else{
            InternalLog(L"CODECAPI_AVEncVideoTemporalLayerCount is not supported, skipping");
        }
        VariantClear(&va);

        // Set CODECAPI_AVEncCommonQuality
        UINT32 ulQuality = 0;

        // RD curve scenario (variation by quality)
        UINT32 ulRDQuality = 0;
        m_pConfig->GetRDCurveType(&rdType);
        BOOL bEncodeQPUsed = FALSE;
        if (rdType == RDCURVE_QUALITY)
        {
            if (S_OK == m_pConfig->GetRDVariationByQuality(m_cPass, &ulRDQuality))
            {
                ulQuality = ulRDQuality;
            }
            else
            {
                InternalLog(L"Failed ITAEFConfigHelper::GetRDVariationByQuality() with hr = 0x%08X", hr);
            }
        }
        else if(rdType == RDCURVE_QP)
        {
            if (S_OK == m_pConfig->GetRDVariationByQuality(m_cPass, &ulRDQuality))
            {
                VariantInit(&va);
                va.vt = VT_UI8;
                va.ullVal = (ULONGLONG)ulRDQuality;         
                _HRL(pCodecAPI->SetValue(&CODECAPI_AVEncVideoEncodeQP , (VARIANT*)&va),
                    InternalLog(L"ICodecAPI error: failed to set CODECAPI_AVEncVideoEncodeQP, hr = 0x%08x", hr));
                bEncodeQPUsed = TRUE;

                UINT32 uiTLCount;

                if(S_OK == pCodecAPI->IsSupported(&CODECAPI_AVEncVideoTemporalLayerCount)){
                    hr = pH264CustomParser->GetStaticTemporalLayerCount(&uiTLCount);
                    if ((SUCCEEDED(hr) || pH264CustomParser->IsDynamicTemporalSet())){
                        VARIANT layerVariant;
                        VariantInit(&layerVariant);
                        layerVariant.vt      = VT_UI4;
                        for(int iLayer=0;iLayer<uiTLCount;iLayer++){
                            layerVariant.ulVal   = iLayer;
                            _HRL(pCodecAPI->SetValue(&CODECAPI_AVEncVideoSelectLayer,&layerVariant),
                                InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncVideoSelectLayer, ...) with hr = 0x%08X", hr));
                            _HRL(pCodecAPI->SetValue(&CODECAPI_AVEncVideoEncodeQP , (VARIANT*)&va),
                                InternalLog(L"ICodecAPI error: failed to set CODECAPI_AVEncVideoEncodeQP, hr = 0x%08x", hr));
                        }
                        VariantClear(&layerVariant);
                    }
                }


                VariantClear(&va);
            }
            else
            {
                InternalLog(L"Failed ITAEFConfigHelper::GetRDVariationByQuality() with hr = 0x%08X", hr);
            }
        }
        // Not RD curve scenario
        else
        {
            hr = m_pConfig->GetQualityLevel(&ulQuality);
            if (S_OK != hr)
            {
                InternalLog(L"Failed ITAEFConfigHelper::GetQualityLevel() with hr = 0x%08X", hr);
            }
        }

        //Set AVEncCommonQuality
        VariantInit(&va);
        va.vt = VT_UI4;
        if (SUCCEEDED(hr) && !bEncodeQPUsed)
        {
            va.uintVal = ulQuality;
            _HRL(pCodecAPI->SetValue(&CODECAPI_AVEncCommonQuality , (VARIANT*)&va), 
                InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncCommonQuality, ...) with hr = 0x%08X", hr));

            UINT32 uiTLCount;
            if(S_OK == pCodecAPI->IsSupported(&CODECAPI_AVEncVideoTemporalLayerCount)){
                hr = pH264CustomParser->GetStaticTemporalLayerCount(&uiTLCount);
                if ((SUCCEEDED(hr) || pH264CustomParser->IsDynamicTemporalSet())){
                    VARIANT layerVariant;
                    VariantInit(&layerVariant);
                    layerVariant.vt      = VT_UI4;
                    for(int iLayer=0;iLayer<uiTLCount;iLayer++){
                        layerVariant.ulVal   = iLayer;
                        _HRL(pCodecAPI->SetValue(&CODECAPI_AVEncVideoSelectLayer,&layerVariant),
                            InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncVideoSelectLayer, ...) with hr = 0x%08X", hr));
                        _HRL(pCodecAPI->SetValue(&CODECAPI_AVEncCommonQuality , (VARIANT*)&va), 
                            InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncCommonQuality, ...) with hr = 0x%08X", hr));
                    }
                    VariantClear(&layerVariant);
                }
            }
        }
        VariantClear(&va);


        // Get Bframe parameter range
        VARIANTARG va_min;
        VARIANTARG va_max;
        VARIANTARG va_delta;

        VariantInit(&va_min);
        VariantInit(&va_max);
        VariantInit(&va_delta);

        // Get parameter range
        if (S_OK == pCodecAPI->IsSupported(&CODECAPI_AVEncMPVDefaultBPictureCount))
        {
            VariantInit(&va_min);
            VariantInit(&va_max);
            VariantInit(&va_delta);
            if (S_OK == pCodecAPI->GetParameterRange(&CODECAPI_AVEncMPVDefaultBPictureCount, &va_min, &va_max, &va_delta))
            {
                pH264CustomParser->m_ulMaxSupportedBFrameCount = va_max.uintVal;
            }
        }

        VariantClear(&va_min);
        VariantClear(&va_max);
        VariantClear(&va_delta);

        // Set CODECAPI_AVEncMPVDefaultBPictureCount
        UINT32 ulBFrameCount = 0;
        VariantInit(&va);
        va.vt = VT_UI4;
        hr = m_pConfig->GetBFrames(&ulBFrameCount);
        if (SUCCEEDED(hr) )
        {
            if(ulBFrameCount != -1)
            {
                va.uintVal = ulBFrameCount;
                hr = pCodecAPI->SetValue(&CODECAPI_AVEncMPVDefaultBPictureCount, (VARIANT*)&va);
                if(FAILED(hr))
                {
                    if(hr != E_INVALIDARG)
                    { 
                        InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncMPVDefaultBPictureCount, ...) with hr = 0x%08X", hr);
                        break;
                    }
                    hr = S_OK;
                }
                else
                {
                    m_pConfig->TestConfigUsed(CODECAPI_BPICTURE_COUNT);
                }
            }else{
                InternalLog(L"BFrame set to -1, so using default encoder value\n");
            }
        }
        VariantClear(&va);

        // Set CODECAPI_AVEncCommonBufferSize
        UINT32 ulBufferSize = 0;
        VariantInit(&va);
        va.vt = VT_UI4;
        hr = m_pConfig->GetBufferSize(&ulBufferSize);
        if (SUCCEEDED(hr) && ulBufferSize > 1 )
        {
            va.uintVal = ulBufferSize;
            _HRL(pCodecAPI->SetValue(&CODECAPI_AVEncCommonBufferSize, (VARIANT*)&va), 
                InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncCommonBufferSize, ...) with hr = 0x%08X", hr));
        }
        VariantClear(&va);


        //Set the Frame rate numerator and denominator set on the output type
        _HR(pH264CustomParser->SetFrameRate(uiFRNumerator,uiFRDenominator));

        //----------------------------------------------------------
        // Set output H.264 specific ICodecAPI properties through custom parser
        //----------------------------------------------------------
        // Set CODECAPI_AVEncCommonQualityVsSpeed
        UINT32 ulQualityVsSpeed = 0;
        VariantInit(&va);
        va.vt = VT_UI4;
        hr = m_pConfig->GetQualityVsSpeed(&ulQualityVsSpeed);
        if (SUCCEEDED(hr) )
        {
            va.uintVal = ulQualityVsSpeed;
            _HRL(pCodecAPI->SetValue(&CODECAPI_AVEncCommonQualityVsSpeed, (VARIANT*)&va), 
                InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncCommonQualityVsSpeed, ...) with hr = 0x%08X", hr));
        }
        VariantClear(&va);

        // Set CODECAPI_AVEncCommonMeanBitRate
        UINT32 ulMeanBitRate = 0;
        VariantInit(&va);
        va.vt = VT_UI4;
        hr = pH264CustomParser->GetMeanBitRate(&ulMeanBitRate);
        if (SUCCEEDED(hr) )
        {
            va.uintVal = ulMeanBitRate;
            hr = pCodecAPI->SetValue(&CODECAPI_AVEncCommonMeanBitRate, (VARIANT*)&va);
            if(FAILED(hr))
            {                
                InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncCommonMeanBitRate, ...) with hr = 0x%08X", hr);
                if(hr == E_INVALIDARG)
                {
                    hr = E_SKIP;
                }
                break;
            }
        }
        else if (hr != MF_E_NOT_FOUND)
        VariantClear(&va);

        // Set CODECAPI_AVEncCommonMaxBitRate
        UINT32 ulMaxBitRate = 0;
        VariantInit(&va);
        va.vt = VT_UI4;
        hr = pH264CustomParser->GetMaxBitRate(&ulMaxBitRate);
        if (SUCCEEDED(hr) )
        {
            va.uintVal = ulMaxBitRate;
            hr = pCodecAPI->SetValue(&CODECAPI_AVEncCommonMaxBitRate, (VARIANT*)&va);
            if(FAILED(hr))
            {  
                InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncCommonMaxBitRate, ...) with hr = 0x%08X", hr);
                if(hr == E_INVALIDARG)
                {
                    hr = E_SKIP;
                }
                break;
            }
        }
        VariantClear(&va);

        // Set CODECAPI_AVEncH264CABACEnable
        BOOL fCABACEnable = 0;
        VariantInit(&va);
        va.vt = VT_BOOL;
        hr = pH264CustomParser->GetCABACEnable(&fCABACEnable);
        if (SUCCEEDED(hr) && (S_OK == pCodecAPI->IsSupported(&CODECAPI_AVEncH264CABACEnable)) )
        {
            va.boolVal = (fCABACEnable == TRUE ? VARIANT_TRUE : VARIANT_FALSE);

            _HRL(pCodecAPI->SetValue(&CODECAPI_AVEncH264CABACEnable, (VARIANT*)&va), 
                InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncH264CABACEnable, ...) with hr = 0x%08X", hr));
        }
        VariantClear(&va);

        // Set CODECAPI_AVEncH264SPSID
        UINT32 ulSPSID = 0;
        VariantInit(&va);
        va.vt = VT_UI4;
        hr = pH264CustomParser->GetSPSID(&ulSPSID);
        if (SUCCEEDED(hr) && (S_OK == pCodecAPI->IsSupported(&CODECAPI_AVEncH264SPSID)))
        {
            va.uintVal = ulSPSID;
            _HRL(pCodecAPI->SetValue(&CODECAPI_AVEncH264SPSID, (VARIANT*)&va), 
                InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncH264SPSID, ...) with hr = 0x%08X", hr));
            m_pConfig->TestConfigUsed(CODECAPI_H264SPSID);
        }
        VariantClear(&va);

        // Set CODECAPI_AVEncH264PPSID
        UINT32 ulPPSID = 0;
        VariantInit(&va);
        va.vt = VT_UI4;
        hr = pH264CustomParser->GetPPSID(&ulPPSID);
        if (SUCCEEDED(hr) && (S_OK == pCodecAPI->IsSupported(&CODECAPI_AVEncH264PPSID)))
        {
            va.uintVal = ulPPSID;
            _HRL(pCodecAPI->SetValue(&CODECAPI_AVEncH264PPSID, (VARIANT*)&va), 
                InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncH264PPSID, ...) with hr = 0x%08X", hr));
            m_pConfig->TestConfigUsed(CODECAPI_H264PPSID);
        }
        VariantClear(&va);

        // Set CODECAPI_AVEncVideoMinQP
        UINT32 ulMinQP = 0;
        VariantInit(&va);
        va.vt = VT_UI4;
        hr = pH264CustomParser->GetMinQP(&ulMinQP);
        if (SUCCEEDED(hr) && S_OK == pCodecAPI->IsSupported(&CODECAPI_AVEncVideoMinQP))
        {
            va.uintVal = ulMinQP;
            _HRL(pCodecAPI->SetValue(&CODECAPI_AVEncVideoMinQP, (VARIANT*)&va),
                InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncVideoMinQP, ...) with hr = 0x%08X", hr)); 	
            m_pConfig->TestConfigUsed(CODECAPI_MINQP);
        }
        else{
            pH264CustomParser->DisableMinQPChecks();
        }
        VariantClear(&va);

        // Set CODECAPI_AVEncVideoMaxQP
        UINT32 ulMaxQP = 0;
        VariantInit(&va);
        va.vt = VT_UI4;
        hr = pH264CustomParser->GetMaxQP(&ulMaxQP);
        if (SUCCEEDED(hr) && S_OK == pCodecAPI->IsSupported(&CODECAPI_AVEncVideoMaxQP))
        {
            va.uintVal = ulMaxQP;
            _HRL(pCodecAPI->SetValue(&CODECAPI_AVEncVideoMaxQP, (VARIANT*)&va),
                InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncVideoMaxQP, ...) with hr = 0x%08X", hr)); 	
            m_pConfig->TestConfigUsed(CODECAPI_MAXQP);
        }
        else{
            pH264CustomParser->DisableMaxQPChecks();
        }
        VariantClear(&va);

        //Set CODECAPI_AvEncVideoEncodeQP
        UINT64 ui64EncodeQP = 0;
        VariantInit(&va);
        va.vt = VT_UI8;
        hr = pH264CustomParser->GetEncodeQP(&ui64EncodeQP);
        if (SUCCEEDED(hr) && S_OK == pCodecAPI->IsSupported(&CODECAPI_AVEncVideoEncodeQP))
        {
            va.ullVal = ui64EncodeQP;
            _HRL(pCodecAPI->SetValue(&CODECAPI_AVEncVideoEncodeQP, (VARIANT*)&va),
                InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncVideoEncodeQP, ...) with hr = 0x%08X", hr)); 

            UINT32 uiTLCount;

            if(S_OK == pCodecAPI->IsSupported(&CODECAPI_AVEncVideoTemporalLayerCount)){
                hr = pH264CustomParser->GetStaticTemporalLayerCount(&uiTLCount);
                if ((SUCCEEDED(hr) || pH264CustomParser->IsDynamicTemporalSet())){
                    VARIANT layerVariant;
                    VariantInit(&layerVariant);
                    layerVariant.vt      = VT_UI4;
                    for(int iLayer=0;iLayer<uiTLCount;iLayer++){
                        layerVariant.ulVal   = iLayer;
                        _HRL(pCodecAPI->SetValue(&CODECAPI_AVEncVideoSelectLayer,&layerVariant),
                            InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncVideoSelectLayer, ...) with hr = 0x%08X", hr));
                        _HRL(pCodecAPI->SetValue(&CODECAPI_AVEncVideoEncodeQP , (VARIANT*)&va),
                            InternalLog(L"ICodecAPI error: failed to set CODECAPI_AVEncVideoEncodeQP, hr = 0x%08x", hr));
                    }
                    VariantClear(&layerVariant);
                }
            }

        }
        VariantClear(&va);


        // Set CODECAPI_AVLowLatencyMode; no need for randomness in Recontest type tests
        if(m_pConfig->IsReconTest())
        {
            ULONG ulNumWorkerThreads = 1;
            _HRL(pMFTAttributes->SetUINT32(
                CODECAPI_AVLowLatencyMode,
                (UINT32)(m_pConfig->IsLowLatency())
                ), InternalLog(L"Failed to set CODECAPI_AVLowLatencyMode on Encoding Attributes"));

            //remove randomness for Compare type tests as this affects the bit exact verification
            if(m_pConfig->IsLowLatency() != FALSE)
            {
                ULONG   ulWorkerThreads = 1;
                _HRL(pMFTAttributes->SetUINT32(
                    CODECAPI_AVEncNumWorkerThreads,
                    (UINT32)ulWorkerThreads
                    ), InternalLog(L"Failed to set CODECAPI_AVEncNumWorkerThreads on Encoding Attributes"));
            } 

        }
        else if (pH264CustomParser->IsLTRLossMode())
        {
             VariantInit(&va);
             va.vt = VT_BOOL;
             va.boolVal = VARIANT_TRUE;		
             if(S_OK == pCodecAPI->IsSupported(&CODECAPI_AVLowLatencyMode))
             {
                 _HRL(pCodecAPI->SetValue( &CODECAPI_AVLowLatencyMode, (VARIANT*)&va ), 
                     InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVLowLatencyMode, ...) with hr = 0x%08X", hr));
             }
             VariantClear(&va);

        
             VariantInit(&va);
             va.vt = VT_UI4;
             va.ullVal = 1;
             if(S_OK == pCodecAPI->IsSupported(&CODECAPI_AVEncNumWorkerThreads))
             {
                 _HRL(pCodecAPI->SetValue( &CODECAPI_AVEncNumWorkerThreads, (VARIANT*)&va ), 
                     InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncNumWorkerThreads, ...) with hr = 0x%08X", hr));		
             }
             VariantClear(&va);
        }
        else if(m_pConfig->IsLowLatency() && S_OK == pCodecAPI->IsSupported(&CODECAPI_AVLowLatencyMode))
        {
            //Use CodecAPI half the time, and the IMFAttribute on the MFT the other half (unless GetAttribute was not supported)
            if(rand() %2 == 0 || pMFTAttributes == NULL){
                VariantInit(&va);
                va.vt = VT_BOOL;
                va.boolVal = VARIANT_TRUE;			
                _HRL(pCodecAPI->SetValue( &CODECAPI_AVLowLatencyMode, (VARIANT*)&va ), 
                    InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVLowLatencyMode, ...) with hr = 0x%08X", hr));
                VariantClear(&va);

                ULONG ulNumWorkerThreads = 0;
                hr = m_pConfig->GetNumWorkerThreads(&ulNumWorkerThreads);
                VariantInit(&va);
                va.vt = VT_UI4;
                if(FAILED(hr))
                {
                    srand(m_pConfig->GetRandomSeed());
                    va.ulVal = (rand() % 16) + 1;
                }
                else 
                {
                    va.ullVal = ulNumWorkerThreads;
                }
                if(S_OK == pCodecAPI->IsSupported(&CODECAPI_AVEncNumWorkerThreads))
                {
                    _HRL(pCodecAPI->SetValue( &CODECAPI_AVEncNumWorkerThreads, (VARIANT*)&va ), 
                        InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncNumWorkerThreads, ...) with hr = 0x%08X", hr));		
                }
                VariantClear(&va);
            }else{
                pMFTAttributes->SetUINT32(MF_LOW_LATENCY,TRUE);
            }
        }	
        else if(S_OK == (pCodecAPI->IsSupported(&CODECAPI_AVLowLatencyMode)))
        {
            if(rand() %2 == 0 || pMFTAttributes == NULL){
                VariantInit(&va);
                va.vt = VT_BOOL;
                va.boolVal = VARIANT_FALSE;
                _HRL(pCodecAPI->SetValue( &CODECAPI_AVLowLatencyMode, (VARIANT*)&va ), 
                    InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVLowLatencyMode, ...) with hr = 0x%08X", hr));
                VariantClear(&va);
            }else{
                pMFTAttributes->SetUINT32(MF_LOW_LATENCY,FALSE);
            }

            m_pConfig->SetLowLatency(FALSE);
        }

        //Set LTR Related properties
        if (pCodecAPI->IsSupported(&CODECAPI_AVEncVideoLTRBufferControl) == S_OK){
            if(pCodecAPI->IsSupported(&CODECAPI_AVEncVideoMarkLTRFrame) != S_OK ||
                pCodecAPI->IsSupported(&CODECAPI_AVEncVideoUseLTRFrame) != S_OK){
                    _HRL(E_FAIL,
                        L"ERROR>> CODECAPI_AVEncVideoLTRBufferControl is supported, but Mark/Use controls are not!");
            }
            UINT32 ulMaxLTRFrames = 0;
            UINT32 ulfTrustUntil = 0;
            VariantInit(&va);
            va.vt = VT_UI4;
            hr = pH264CustomParser->GetLTRInfo(&ulMaxLTRFrames, &ulfTrustUntil);
            if (SUCCEEDED(hr))
            {
                va.uintVal = ulMaxLTRFrames | (ulfTrustUntil << 16);
                _HRL(pCodecAPI->SetValue(&CODECAPI_AVEncVideoLTRBufferControl, (VARIANT*)&va), 
                    InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncVideoLTRBufferControl, ...) with hr = 0x%08X", hr));
            }
            else
            {
                if(hr != MF_E_NOT_FOUND)
                {
                    InternalLog(L"Failed ITAEFConfigHelper::GetLTRInfo() with hr = 0x%08X", hr);
                    break;
                }

                // MF_E_NOT_FOUND means we’re not running an LTR test, ignore the failure
                hr = S_OK;
            }
            VariantClear(&va);
        }

        //Set ROI

        if(pCodecAPI->IsSupported(&CODECAPI_AVEncVideoROIEnabled) == S_OK){
            UINT32 bROIEnabled = 1;
            VariantInit(&va);
            va.vt = VT_UI4;
            if(pH264CustomParser->IsROIModeSet()){
                va.uintVal = bROIEnabled;
                _HRL(pCodecAPI->SetValue(&CODECAPI_AVEncVideoROIEnabled, (VARIANT*)&va), 
                    InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncVideoROIEnabled, ...) with hr = 0x%08X", hr));
            }
            VariantClear(&va);
        }else{
            pH264CustomParser->DisableROI();
        }

        //Set MaxNumRefFrames
        if (pCodecAPI->IsSupported(&CODECAPI_AVEncVideoMaxNumRefFrame) == S_OK){
            UINT32 ulMaxRefFrames = 0;
            VariantInit(&va);
            va.vt = VT_UI4;
            hr = pH264CustomParser->GetMaxNumRefFrames(&ulMaxRefFrames);
            if (SUCCEEDED(hr))
            {
                va.uintVal = ulMaxRefFrames;
                _HRL(pCodecAPI->SetValue(&CODECAPI_AVEncVideoMaxNumRefFrame, (VARIANT*)&va), 
                    InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncVideoMaxNumRefFrame, ...) with hr = 0x%08X", hr));
            }
            else
            {
                if(hr != MF_E_NOT_FOUND)
                {
                    InternalLog(L"Failed ITAEFConfigHelper::GetMaxNumRefFrames() with hr = 0x%08X", hr);
                    break;
                }
                hr = S_OK;
            }
            VariantClear(&va);
        }

        BOOL bFoundSliceMode = FALSE;

        // Set SliceControlMode
        if (pCodecAPI->IsSupported(&CODECAPI_AVEncSliceControlMode) == S_OK) 
        { 
            VariantInit(&va);
            va.vt = VT_UI4;
            hr = pH264CustomParser->GetSliceControlMode(&ulSliceControlMode);
            if (SUCCEEDED(hr) )
            {
                VARIANT vamin;
                VARIANT vamax;
                VARIANT vastep;
                VariantInit(&vamin);
                VariantInit(&vamax);
                VariantInit(&vastep);
                do{
                    hr = pCodecAPI->GetParameterRange(&CODECAPI_AVEncSliceControlMode,&vamin,&vamax,&vastep);
                    _HRL(hr,InternalLog(L"Failed ICodecAPI::GetParameterRange(&CODECAPI_AVEncSliceControlMode, ...) with hr = 0x%08X", hr));

                    wprintf(L"vamin: %d vamax: %d vastep: %d\n",vamin.ulVal,vamax.ulVal,vastep.ulVal);

                    if(vamin.ulVal > 2 || vamax.ulVal > 2){
                        hr  = E_FAIL;
                        InternalLog(L"ICodecAPI::GetParameterRange(&CODECAPI_AVEncSliceControlMode) returned unexpected values");
                        InternalLog(L"Expected: Min >= 0, Max <= 2. Recieved: Min: %d Max: %d",vamin.ulVal,vamax.ulVal);
                        break;
                    }

                    if(vamin.ulVal > vamax.ulVal){
                        hr  = E_FAIL;
                        InternalLog(L"ICodecAPI::GetParameterRange(&CODECAPI_AVEncSliceControlMode) returned higher min than max");
                        break;
                    }

                    if(vamin.ulVal != vamax.ulVal && vastep.ulVal == 0 && vastep.ulVal != VT_EMPTY){
                        hr  = E_FAIL;
                        InternalLog(L"ICodecAPI::GetParameterRange(&CODECAPI_AVEncSliceControlMode) returned step of 0 with non-equal min and max");
                        break;
                    }
                    int increment = 0;
                    if(vastep.ulVal == VT_EMPTY){
                        increment = 1;
                    }else{
                        increment = vastep.ulVal;
                    }
                    for(int i = vamin.ulVal;i<=vamax.ulVal;i += increment){
                        if(i == ulSliceControlMode){
                            bFoundSliceMode = TRUE;
                            break;
                        }
                        if(increment == 0){
                            break;
                        }
                    }

                    if(bFoundSliceMode){
                        va.ulVal = ulSliceControlMode;
                        hr = pCodecAPI->SetValue(&CODECAPI_AVEncSliceControlMode, (VARIANT*)&va);
                        if(FAILED(hr))
                        {  
                            InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncSliceControlMode, ...) with hr = 0x%08X", hr);
                            if(hr == E_INVALIDARG)
                            {
                                hr = E_SKIP;
                            }
                            break;
                        }
                    }else{
                        InternalLog(L"Requested slice mode not supported. Disabling slice control checks.");
                        pH264CustomParser->DisableSliceControlChecks();
                        hr = S_OK;
                        break;
                    }
                }while(FALSE);
                VariantClear(&vamin);
                VariantClear(&vamax);
                VariantClear(&vastep);
                _HR(hr);
            }
            VariantClear(&va);
        }else{
            pH264CustomParser->DisableSliceControlChecks();
        }

        // Set SliceControlSize
        if (pCodecAPI->IsSupported(&CODECAPI_AVEncSliceControlSize) == S_OK) 
        { 
            VariantInit(&va);
            va.vt = VT_UI4;
            hr = pH264CustomParser->GetSliceControlSize(&ulSliceControlSize);
            if (SUCCEEDED(hr) && bFoundSliceMode)
            {
                va.ulVal = ulSliceControlSize;
                hr = pCodecAPI->SetValue(&CODECAPI_AVEncSliceControlSize, (VARIANT*)&va);
                if(FAILED(hr))
                {  
                    InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncSliceControlSize, ...) with hr = 0x%08X", hr);
                    if(hr == E_INVALIDARG)
                    {
                        hr = E_SKIP;
                    }
                    break;
                }
            }
            VariantClear(&va);
        }

        // ScenarioInfo
        if (pCodecAPI->IsSupported(&CODECAPI_AVScenarioInfo) == S_OK) 
        { 
            VariantInit(&va);
            va.vt = VT_UI4;
            UINT32 uiScenarioInfo = 0;

            hr = pH264CustomParser->GetScenarioInfo(&uiScenarioInfo);
            if (SUCCEEDED(hr))
            {
                // The encoder should accept any value for this property because:
                // 1. It is just a hint as to the scenario and
                // 2. New scenario values may be added in the future
                // Therefore, perform a special check here to ensure that the encoder does not
                // fail a value that is beyond the current defined range of values (use a very large value)
                va.ulVal = 0xFFFFFFFF;
                hr = pCodecAPI->SetValue(&CODECAPI_AVScenarioInfo, (VARIANT*)&va);
                if(FAILED(hr))
                {  
                    InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVScenarioInfo, ...) with hr = 0x%08X", hr);
                    InternalLog(L"The encoder failed the setting of CODECAPI_AVScenarioInfo to 0xFFFFFFFF. The encoder must accept all values for this property");
                    break;
                }

                // Now try setting the value from the test configuration. The output bitstream will be verified to confirm
                // that the encoder produces decodable output with this value set
                va.ulVal = uiScenarioInfo;
                hr = pCodecAPI->SetValue(&CODECAPI_AVScenarioInfo, (VARIANT*)&va);
                if(FAILED(hr))
                {  
                    InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVScenarioInfo, ...) with hr = 0x%08X", hr);
                    InternalLog(L"The encoder failed the setting of CODECAPI_AVScenarioInfo to %d.", uiScenarioInfo);
                    break;
                }
            }  
            VariantClear(&va);
        }

        // EnableLowPower
        if (pCodecAPI->IsSupported(&CODECAPI_AVEncLowPowerEncoder) == S_OK) 
        { 
            VariantInit(&va);
            va.vt = VT_UI4;
            BOOL fEnableLowPower = FALSE;

            hr = pH264CustomParser->GetEnableLowPower(&fEnableLowPower);
            if (SUCCEEDED(hr))
            {
                va.ulVal = fEnableLowPower ? 1 : 0;
                hr = pCodecAPI->SetValue(&CODECAPI_AVEncLowPowerEncoder, (VARIANT*)&va);
                if(FAILED(hr))
                {  
                    InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncLowPowerEncoder, ...) with hr = 0x%08X", hr);
                    if(hr == E_INVALIDARG)
                    {
                        hr = E_SKIP;
                    }
                    break;
                }
            }  
            VariantClear(&va);
        }

        // GradualIntraRefresh
        UINT32 uiIntraRefreshMode = 0;
        UINT32 uiIntraRefreshFrames = 0;

        hr = pH264CustomParser->GetIntraRefreshInfo(&uiIntraRefreshMode, &uiIntraRefreshFrames);
        if (uiIntraRefreshMode != 0)
        {
            if (pCodecAPI->IsSupported(&CODECAPI_AVEncVideoGradualIntraRefresh) == S_OK)
            {
                InternalLog(L"CODECAPI_AVEncVideoGradualIntraRefresh called ");
                VariantInit(&va);
                va.vt = VT_UI4;

                if (SUCCEEDED(hr))
                {
                    ULONG nValuesCount = 0;
                    VARIANT* Values;
                    _HRL(pCodecAPI->GetParameterValues(&CODECAPI_AVEncVideoGradualIntraRefresh, &Values, &nValuesCount),
                        InternalLog(L"Failed ICodecAPI::GetParameterValues(&CODECAPI_AVEncVideoGradualIntraRefresh, ...) with hr = 0x%08X", hr));

                    InternalLog(L"GetParameterValues values succeeded ");

                    if (Values->vt != VT_UI4)
                    {
                        InternalLog(L"Failed in correct vtype returned by ICodecAPI::GetParameterValues(&CODECAPI_AVEncVideoGradualIntraRefresh, ...)", hr);
                        break;
                    }

                    BOOL fModeSupported = FALSE;
                    for (ULONG i = 0; i < nValuesCount; i++)
                    {
                        if (uiIntraRefreshMode == Values[i].ulVal)
                        {
                            fModeSupported = TRUE;
                            break;
                        }
                    }

                    if (fModeSupported)
                    {
                        InternalLog(L"uiIntraRefreshMode suppoerted = %d", uiIntraRefreshMode);

                        for (ULONG i = 0; i < nValuesCount; i++)
                        {
                            VariantClear(Values + i);
                        }

                        va.ulVal = uiIntraRefreshMode | (uiIntraRefreshFrames << 16);
                        hr = pCodecAPI->SetValue(&CODECAPI_AVEncVideoGradualIntraRefresh, (VARIANT*) &va);
                        if (FAILED(hr))
                        {
                            InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncVideoGradualIntraRefresh, ...) with hr = 0x%08X", hr);
                            if (hr == E_INVALIDARG)
                            {
                                hr = E_SKIP;
                            }
                            break;
                        }
                    }
                    else
                    {
                        InternalLog(L"Requested intra refresh mode not supported = %d", uiIntraRefreshMode);
                        hr = E_SKIP;
                        break;
                    }
                }
                VariantClear(&va);
            }
            else
            {
                hr = E_SKIP;
                break;
            }
        }

        //Enable MAD   
        if(pCodecAPI->IsSupported(&CODECAPI_AVEncVideoMeanAbsoluteDifference) == S_OK){
            VariantInit(&va);
            va.vt = VT_UI4;
            if(pH264CustomParser->IsMADEnabled()){
                va.ulVal = 1;
            }else{
                va.ulVal = 0;
            }
            _HRL(pCodecAPI->SetValue(&CODECAPI_AVEncVideoMeanAbsoluteDifference, (VARIANT*)&va),
                InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncVideoMeanAbsoluteDifference, ...) with hr = 0x%08X", hr));
            VariantClear(&va);
        }

        // Output CodecAPI settings
        DescribeICodecAPI(pCodecAPI, m_pLog);

        if (TAEFConfigHelper::FuzzCodecAPI == m_pConfig->GetFuzzType())
        {
#ifndef MINWIN
            // Fuzz CodecAPI
            hr = ::FuzzCodecAPIProps(
                pCodecAPI, 
                FuzzHelper::iCodecAPIFuzzModeVideoEncoder, 
                FuzzHelper::eFuzzSameSetRandomValues, 
                m_pLog);
#else
            hr = E_NOTIMPL;
#endif //#ifndef MINWIN
            // Ignore failures in fuzzing, just log the failures and proceed
            if (S_OK != hr)
            {
                InternalLog(L"FuzzCodecAPIProps() failed with hr = 0x%08X", hr);
                hr = S_OK;
            }

            InternalLog(L"*** H.264 Encoder ICodecAPI properties after fuzzing:");
            DescribeICodecAPI(pCodecAPI, m_pLog);
        }

        // set output MT on encoder
        for(int i = 0 ; i < 2 ; i++)
        {
            hr = pMFT->SetOutputType(0, pOutputType, 0);
            UINT32 uintInterlaceMode = MFGetAttributeUINT32(pOutputType, MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
            if(hr == MF_E_INVALIDMEDIATYPE && MFVideoInterlace_Progressive != uintInterlaceMode)
            {
                _HRL(pOutputType->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive), InternalLog(L"ERROR>> Failed setting interlace mode (%u) on output type, hr = 0x%08X", uintInterlaceMode, hr));								
                continue;
            }
            break;
        }

        if((hr == MF_E_INVALIDMEDIATYPE || hr == MF_E_INVALIDTYPE || hr == MF_E_UNSUPPORTED_D3D_TYPE)&& !m_pConfig->IsHWModeSet() && m_pConfig->GetHWMode() == MFTopologyHelper::HWMode::HW)
        {
            hr = E_SKIP;
            break;
        }

        _HR(pMFT->GetOutputCurrentType(0,&pEncoderProvidedOutputType));

        _HR(hr);
        if(ppOutputType)
        {
            SAFE_ADDREF(pEncoderProvidedOutputType);
            *ppOutputType = pEncoderProvidedOutputType;
        }

        //Set IgnoredLTR2 - these setting should be ignored by the encoder
        if (pCodecAPI->IsSupported(&CODECAPI_AVEncVideoLTRBufferControl) == S_OK){
            if(pCodecAPI->IsSupported(&CODECAPI_AVEncVideoMarkLTRFrame) != S_OK ||
                pCodecAPI->IsSupported(&CODECAPI_AVEncVideoUseLTRFrame) != S_OK){
                    _HRL(E_FAIL,
                        L"ERROR>> CODECAPI_AVEncVideoLTRBufferControl is supported, but Mark/Use controls are not!");
            }
            UINT32 ulMaxLTRFrames = 0;
            UINT32 ulfTrustUntil = 0;
            VariantInit(&va);
            va.vt = VT_UI4;
            HRESULT hr_SetValue = S_OK;

            hr = pH264CustomParser->GetIgnoredLTRInfo(&ulMaxLTRFrames, &ulfTrustUntil);
            if (SUCCEEDED(hr))
            {
                va.uintVal = ulMaxLTRFrames | (ulfTrustUntil << 16);
                hr_SetValue = pCodecAPI->SetValue(&CODECAPI_AVEncVideoLTRBufferControl, (VARIANT*)&va);
                if (S_OK != hr_SetValue)
                {
                    InternalLog(L"Failed Ignored ICodecAPI::SetValue(&CODECAPI_AVEncVideoLTRBufferControl, ...) with hr = 0x%08X", hr_SetValue);
                    break;
                }
            }
            else
            {
                if(hr != MF_E_NOT_FOUND)
                {
                    InternalLog(L"Failed ITAEFConfigHelper::GetIgnoredLTRInfo() with hr = 0x%08X", hr);
                    break;
                }
                hr = S_OK;
            }
            VariantClear(&va);
        }

        //Set IgnoredSliceControlMode - these setting should be ignored by the encoder
        bFoundSliceMode = FALSE;
        if (pCodecAPI->IsSupported(&CODECAPI_AVEncSliceControlMode) == S_OK) 
        { 
            VariantInit(&va);
            va.vt = VT_UI4;
            hr = pH264CustomParser->GetIgnoredSliceControlMode(&ulIgnoredSliceControlMode);
            if (SUCCEEDED(hr) )
            {
                VARIANT vamin;
                VARIANT vamax;
                VARIANT vastep;
                VariantInit(&vamin);
                VariantInit(&vamax);
                VariantInit(&vastep);
                do{
                    hr = pCodecAPI->GetParameterRange(&CODECAPI_AVEncSliceControlMode,&vamin,&vamax,&vastep);
                    _HRL(hr,InternalLog(L"Failed ICodecAPI::GetParameterRange(&CODECAPI_AVEncSliceControlMode, ...) with hr = 0x%08X", hr));
                    if(vamin.ulVal > 2 || vamax.ulVal > 2){
                        hr  = E_FAIL;
                        InternalLog(L"ICodecAPI::GetParameterRange(&CODECAPI_AVEncSliceControlMode) returned unexpected values");
                        InternalLog(L"Expected: Min >= 0, Max <= 2. Recieved: Min: %d Max: %d",vamin.ulVal,vamax.ulVal);
                        break;
                    }
                    if(vamin.ulVal > vamax.ulVal){
                        hr  = E_FAIL;
                        InternalLog(L"ICodecAPI::GetParameterRange(&CODECAPI_AVEncSliceControlMode) returned higher min than max");
                        break;
                    }
                    if(vamin.ulVal != vamax.ulVal && vastep.ulVal == 0 && vastep.ulVal != VT_EMPTY){
                        hr  = E_FAIL;
                        InternalLog(L"ICodecAPI::GetParameterRange(&CODECAPI_AVEncSliceControlMode) returned step of 0 with non-equal min and max");
                        break;
                    }
                    int increment = 0;
                    if(vastep.ulVal == VT_EMPTY){
                        increment = 1;
                    }else{
                        increment = vastep.ulVal;
                    }
                    for(int i = vamin.ulVal;i<=vamax.ulVal;i += increment){
                        if (i == ulIgnoredSliceControlMode){
                            bFoundSliceMode = TRUE;
                            break;
                        }
                        if(increment == 0){
                            break;
                        }
                    }
                    if(bFoundSliceMode)
                    {
                        va.ulVal = ulIgnoredSliceControlMode;
                        // Encoder should reject SliceControlMode setting after SetOutputType called
                        hr = pCodecAPI->SetValue(&CODECAPI_AVEncSliceControlMode, (VARIANT*)&va);
                        if (SUCCEEDED(hr))
                        {
                            InternalLog(L"Error: Encoder did not reject ICodecAPI::SetValue(&CODECAPI_AVEncSliceControlMode, ...) after SetOutputType called!");
                            hr = E_FAIL;
                            break;
                        }
                        else
                        {
                            hr = S_OK;
                        }
                    }
                }while(FALSE);
                VariantClear(&vamin);
                VariantClear(&vamax);
                VariantClear(&vastep);
                _HR(hr);
            }else{
                hr = S_OK;
            }
            VariantClear(&va);
        }

        //Set DynamicInitialSliceControlSize
        if (pCodecAPI->IsSupported(&CODECAPI_AVEncSliceControlSize) == S_OK) 
        { 
            VariantInit(&va);
            va.vt = VT_UI4;
            hr = pH264CustomParser->GetDynamicInitialSliceControlSize(&ulDynamicInitialSliceControlSize);
            if (SUCCEEDED(hr))
            {
                va.ulVal = ulDynamicInitialSliceControlSize;
                hr = pCodecAPI->SetValue(&CODECAPI_AVEncSliceControlSize, (VARIANT*)&va);
                if (SUCCEEDED(hr))
                {
                    if (bFoundSliceMode)
                    {
                        InternalLog(L"Error: Encoder did not ignore ICodecAPI::SetValue(&CODECAPI_AVEncSliceControlSize, ...) after SliceControlMode attempted to set!");
                        hr = E_FAIL;
                        break;
                    }
                    else
                    {
                        pH264CustomParser->ApplyDynamicInitialSliceControlSize();
                        hr = S_OK;
                    }
                }
                else
                {
                    if (bFoundSliceMode)
                    {
                        hr = S_OK;
                    }
                    else
                    {
                        InternalLog(L"Failed ICodecAPI::SetValue(&CODECAPI_AVEncSliceControlMode, ...) with hr = 0x%08X", hr);
                        if (hr == E_INVALIDARG)
                        {
                            hr = E_SKIP;
                        }
                    }
                }
            }
            else
            {
                hr = S_OK;
            }
            VariantClear(&va);
        }

    } while(FALSE);

    //VariantClear(&va);

    SAFE_RELEASE(pEncoderProvidedOutputType);
    SAFE_RELEASE(pOutputType);
    SAFE_RELEASE(pCodecAPI);
    SAFE_RELEASE(pMFTAttributes);
    SAFE_RELEASE(pH264CustomParser);

    return hr;
}

HRESULT CConfigureH264Encoder::VerifyAPI_GetParameterValues(ICodecAPI *pCodecAPI)
{
    HRESULT hr = S_OK;
    HRESULT hr_getParameterValues = S_OK;

    if (pCodecAPI == NULL)
        return E_SKIP;

    int iSizeCodecAPI = sizeof(arrCodecAPI)/sizeof(arrCodecAPI[0]);

    for(int i=0; i<iSizeCodecAPI ;i++){
        if(S_OK == pCodecAPI->IsSupported(arrCodecAPI[i].guid)){
            ULONG nValuesCount = 0;
            VARIANT* Values;
            InternalLog(L"    Calling ICodecAPI::GetParameterValues(%s, ...)", arrCodecAPI[i].lpwstrName);
            hr_getParameterValues = pCodecAPI->GetParameterValues(arrCodecAPI[i].guid, &Values, &nValuesCount);
            if (&CODECAPI_AVEncVideoTemporalLayerCount == arrCodecAPI[i].guid){
                if (VFW_E_CODECAPI_LINEAR_RANGE != hr_getParameterValues){
                    InternalLog(L"Failed ICodecAPI::GetParameterValues(&CODECAPI_AVEncVideoTemporalLayerCount, ...) with hr = 0x%08X", hr_getParameterValues);
                    hr = E_FAIL;
                }
                else {
                    // ToDo - check return values?
                }
            }
            else if (&CODECAPI_AVEncVideoLTRBufferControl == arrCodecAPI[i].guid){
                if (S_OK != hr_getParameterValues){
                    InternalLog(L"Failed ICodecAPI::GetParameterValues(&CODECAPI_AVEncVideoLTRBufferControl, ...) with hr = 0x%08X", hr_getParameterValues);
                    hr = E_FAIL;
                }
                else {
                    // ToDo - check return values?
                }
            }
            else {
                // Property is not required to support GetParameterValues so it must either return E_NOTIMPL, VFW_E_CODECAPI_LINEAR_RANGE or S_OK
                if (E_NOTIMPL != hr_getParameterValues && S_OK != hr_getParameterValues && VFW_E_CODECAPI_LINEAR_RANGE != hr_getParameterValues){
                    InternalLog(L"Failed ICodecAPI::GetParameterValues(%s, ...) with hr = 0x%08X", arrCodecAPI[i].lpwstrName, hr_getParameterValues);
                    hr = E_FAIL;
                }
            }

            for (UINT i =0; i<nValuesCount;i++)
            {
                VariantClear(Values+i);
            }
        }
    }

    return hr;
}

HRESULT CConfigureH264Encoder::VerifyAPI_GetParameterRange(ICodecAPI *pCodecAPI)
{
    HRESULT hr = S_OK;
    HRESULT hr_getParameterRange = S_OK;

    if (pCodecAPI == NULL)
        return E_SKIP;

    int iSizeCodecAPI = sizeof(arrCodecAPI)/sizeof(arrCodecAPI[0]);

    for(int i=0; i<iSizeCodecAPI ;i++){
        if(S_OK == pCodecAPI->IsSupported(arrCodecAPI[i].guid)){
            ULONG nValuesCount = 0;
            VARIANTARG va_min;
            VARIANTARG va_max;
            VARIANTARG va_delta;
            VariantInit(&va_min);
            VariantInit(&va_max);
            VariantInit(&va_delta);

            InternalLog(L"    Calling ICodecAPI::GetParameterValues(%s, ...)", arrCodecAPI[i].lpwstrName);
            hr_getParameterRange = pCodecAPI->GetParameterRange(arrCodecAPI[i].guid,&va_min,&va_max,&va_delta);

            if (&CODECAPI_AVEncVideoTemporalLayerCount == arrCodecAPI[i].guid ||
                &CODECAPI_AVEncSliceControlMode == arrCodecAPI[i].guid ||
                &CODECAPI_AVEncSliceControlSize == arrCodecAPI[i].guid ||
                &CODECAPI_AVEncVideoMaxNumRefFrame == arrCodecAPI[i].guid ||
                &CODECAPI_AVEncVideoMaxQP == arrCodecAPI[i].guid ||
                &CODECAPI_AVEncVideoMinQP == arrCodecAPI[i].guid)
            {
                if (S_OK != hr_getParameterRange){
                    InternalLog(L"Failed ICodecAPI::GetParameterRange(%s, ...) with hr = 0x%08X", arrCodecAPI[i].lpwstrName, hr_getParameterRange);
                    hr = E_FAIL;
                }
            }
            else {
                if (E_NOTIMPL != hr_getParameterRange && S_OK != hr_getParameterRange && VFW_E_CODECAPI_ENUMERATED  != hr_getParameterRange){
                    // Property is not required to support GetParameterRange so it must either return E_NOTIMPL, VFW_E_CODECAPI_ENUMERATED or S_OK
                    InternalLog(L"Failed ICodecAPI::GetParameterRange(%s, ...) with hr = 0x%08X", arrCodecAPI[i].lpwstrName, hr_getParameterRange);
                    hr = E_FAIL;
                }
            }

            VariantClear(&va_min);
            VariantClear(&va_max);
            VariantClear(&va_delta);
        }
    }

    return hr;
}

//-----------------------------------------------------------------------------
// Setting hardcoded audio output attributes as we check only video output 
// for conformance to the settings in the test xml.
//-----------------------------------------------------------------------------
STDMETHODIMP
    CConfigureAACEncoder::Configure(
    __in IMFTransform* pMFT,
    __in IMFMediaType* pUpstreamType,
    __inout IMFMediaType** ppInputType,
    __inout IMFMediaType** ppOutputType)
{
    HRESULT hr = S_OK;

    IMFMediaType* pOutputType = NULL;
    UINT32 uintChannels = 1;
    UINT32 uintSamplesPerSec = 44100;
    UINT32 uintOutputBitrate = 128000;

    if(!pMFT || !pUpstreamType || !ppInputType || !ppOutputType)
    {
        return E_POINTER;
    }
    do
    {
        InternalLog(L"*** AAC Encoder input type:");
        DescribeMediaType(pUpstreamType, m_pLog);

        SAFE_RELEASE(pOutputType);
        _HR(MFCreateMediaType(&pOutputType));

        // Major type
        MFTopologyHelper::MajorTypes majortypes = MFTopologyHelper::NoAudioVideo;
        majortypes = m_pConfig->GetMajorTypes();
        if (majortypes == MFTopologyHelper::BothAudioVideo || majortypes == MFTopologyHelper::Audio)
        {
            _HR(pOutputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio));
        }
        else
        {
            _HR(E_INVALIDARG);
        }

        // Subtype
        CLSID clsidAudioEncoder = m_pConfig->GetAudioEncoder();
        if (CLSID_AACMFTEncoder == clsidAudioEncoder)
        {
            _HR(pOutputType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_AAC));
        }

        // number of channels
        uintChannels = MFGetAttributeUINT32(pUpstreamType, MF_MT_AUDIO_NUM_CHANNELS, 1);
        if(uintChannels <1 || uintChannels >2)
        {
            uintChannels = 1;
        }
        _HR(pOutputType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, uintChannels));

        // Bits per sample (always 16 for AAC)
        _HR(pOutputType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 16));

        // Samples per second
        uintSamplesPerSec = MFGetAttributeUINT32(pUpstreamType, MF_MT_AUDIO_SAMPLES_PER_SECOND, 44100);
        if((uintSamplesPerSec != 44100) && (uintSamplesPerSec != 48000))
        {
            uintSamplesPerSec = 44100;
        }
        _HR(pOutputType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, uintSamplesPerSec));

        // average bytes per second
        _HR(pOutputType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, uintOutputBitrate/8));

        // profile-level indication 
        _HR(pOutputType->SetUINT32(MF_MT_AAC_AUDIO_PROFILE_LEVEL_INDICATION, 0x29));

        // payload type (0 means The stream contains raw_data_block elements only)
        _HR(pOutputType->SetUINT32(MF_MT_AAC_PAYLOAD_TYPE, 0));

        Log::Comment(L"*** AAC Encoder output type:");
        DescribeMediaType(pOutputType, m_pLog);

        // set output MT on encoder
        _HR(pMFT->SetOutputType(0, pOutputType, 0));

        // Set created media type on output
        if(ppOutputType)
        {
            SAFE_ADDREF(pOutputType);
            *ppOutputType = pOutputType;
        }

    } while(FALSE);

    SAFE_RELEASE(pOutputType);

    return hr;
} // ConfigureAACEncoder

//-----------------------------------------------------------------------------
// Reading H.264 encoding settings from TAEF test xml by using generic 
// config helper (ITAEFConfigHelper) and a custom config helper for H.264 
// (ITAEFConfigCustomParser), then setting output IMFAttributes and 
// ICodecAPI properties based on the read values.
//-----------------------------------------------------------------------------
STDMETHODIMP
    CConfigureFuzzEncoder::Configure(
    __in IMFTransform* pMFT,
    __in IMFMediaType* pUpstreamType,
    __inout IMFMediaType** ppInputType,
    __inout IMFMediaType** ppOutputType)
{
    HRESULT hr = S_OK;

    IMediaFuzzerMFT *pMediaFuzzerMFT = NULL;
    IMFMediaType* pOutputType = NULL;
    IMFAttributes* pAttrInputMT = NULL;
    IMFAttributes* pAttrOutputMT = NULL;

    if(!pMFT || !pUpstreamType || !ppInputType || !ppOutputType)
    {
        return E_POINTER;
    }

    do
    {
        InternalLog(L"*** H.264 Encoder upstream type:");
        DescribeMediaType(pUpstreamType, m_pLog);

        //---------------------------
        // Set output media type attributes
        //---------------------------
        SAFE_RELEASE(pOutputType);

        _HR( MFCreateMediaType(&pOutputType));

        // If no callback provided, assume that MFT is an in-place transform and inherit output media type

        _HRL( pUpstreamType->QueryInterface(IID_IMFAttributes, (LPVOID*)&pAttrInputMT),
            InternalLog(L"Failed to query IMFAttributes from MFT's input media type, hr = 0x%08X", hr));

        _HRL( pOutputType->QueryInterface(IID_IMFAttributes, (LPVOID*)&pAttrOutputMT),
            InternalLog(L"Failed to query IMFAttributes from MFT's output media type, hr = 0x%08X", hr));

        _HRL( pAttrInputMT->CopyAllItems(pAttrOutputMT), 
            InternalLog(L"Failed to copy MFT's input media type to output media type, hr = 0x%08X", hr));

        // Configure fuzz MFT
        _HRL(pMFT->QueryInterface(__uuidof(IMediaFuzzerMFT), (LPVOID*)&pMediaFuzzerMFT), 
            InternalLog(L"Failed to query IMediaFuzzerMFT from IMFTransform, hr = 0x%08X", hr));

        FuzzMFTransformType fuzzMFTransformType = m_pConfig->GetFuzzMFTransformType();

        if (FuzzMFTransformType::Random == fuzzMFTransformType)
        {
            _HR(pMediaFuzzerMFT->SetFuzzType(eRANDOM));
        }
        else if (FuzzMFTransformType::Data == fuzzMFTransformType)
        {
            WCHAR pwszSourceData[] = L"abcde";

            _HR(pMediaFuzzerMFT->SetFuzzType(eDATA));
            _HR(pMediaFuzzerMFT->SetDataFuzzType(eCORRUPT));

            _HR(pMediaFuzzerMFT->SetSourceData(pwszSourceData));
            UINT32 uiStartFrame = m_pConfig->GetFuzzStartFrame();
            UINT32 uiEndFrame = m_pConfig->GetFuzzEndFrame();
            _HR(pMediaFuzzerMFT->SetStartFrame(uiStartFrame));
            _HR(pMediaFuzzerMFT->SetEndFrame(uiEndFrame));
        }
        else if (FuzzMFTransformType::TimeStamp == fuzzMFTransformType)
        {
            _HR(pMediaFuzzerMFT->SetFuzzType(eTIMESTAMP));
            UINT32 uiStartFrame = m_pConfig->GetFuzzStartFrame();
            UINT32 uiEndFrame = m_pConfig->GetFuzzEndFrame();
            _HR(pMediaFuzzerMFT->SetTSStartFrame(uiStartFrame));
            _HR(pMediaFuzzerMFT->SetTSEndFrame(uiEndFrame));
        }
        else if (FuzzMFTransformType::Drop == fuzzMFTransformType)
        {
            _HR(pMediaFuzzerMFT->SetFuzzType(eDROP));
            UINT32 uiDropFrame = m_pConfig->GetFuzzDropFrame();
            _HR(pMediaFuzzerMFT->SetDropFrame(uiDropFrame));
        }

        _HR(pMediaFuzzerMFT->SetRandomSeed((BYTE)123));
        _HR(pMediaFuzzerMFT->SetErrorPoint(0));
        _HR(pMediaFuzzerMFT->SetErrorRate(1));


        /*
        _HR(pMediaFuzzerMFT->SetStartFrame(0));
        _HR(pMediaFuzzerMFT->SetEndFrame(0));
        _HR(pMediaFuzzerMFT->SetSourceData());
        _HR(pMediaFuzzerMFT->SetDestData());
        _HR(pMediaFuzzerMFT->SetTSStartFrame(0));
        _HR(pMediaFuzzerMFT->SetTSEndFrame(0));
        _HR(pMediaFuzzerMFT->SetRepeatFrame(0));
        _HR(pMediaFuzzerMFT->SetDropFrame(0));
        _HR(pMediaFuzzerMFT->SetSeekLoop());
        */

        // set output MT on encoder
        _HR(pMFT->SetOutputType(0, pOutputType, 0));

        if(ppOutputType)
        {
            SAFE_ADDREF(pOutputType);
            *ppOutputType = pOutputType;
        }

    } while(FALSE);

    SAFE_RELEASE(pOutputType);
    SAFE_RELEASE(pAttrInputMT);
    SAFE_RELEASE(pAttrOutputMT);
    SAFE_RELEASE(pMediaFuzzerMFT);

    return hr;
}

HRESULT CH264EncHelper::ConvertEncodingProfile(
    __in WEX::Common::String strEncodingProfile, 
    __inout eAVEncH264VProfile *ph264Profile)
{
    HRESULT hr=  S_OK;
    eAVEncH264VProfile h264Profile = eAVEncH264VProfile_unknown;

    if (NULL == ph264Profile)
    {
        return E_POINTER;
    }
    if (strEncodingProfile.IsEmpty())
    {
        return E_INVALIDARG;
    }

    if (strEncodingProfile == L"unknown")
    {
        h264Profile = eAVEncH264VProfile_unknown;
    }
    else if (strEncodingProfile == L"simple" || strEncodingProfile == L"base")
    {
        h264Profile = eAVEncH264VProfile_Base;
    }
    else if (strEncodingProfile == L"main")
    {
        h264Profile = eAVEncH264VProfile_Main;
    }
    else if (strEncodingProfile == L"high")
    {
        h264Profile = eAVEncH264VProfile_High;
    }
    else if (strEncodingProfile == L"422")
    {
        h264Profile = eAVEncH264VProfile_422;
    }
    else if (strEncodingProfile == L"High10")
    {
        h264Profile = eAVEncH264VProfile_High10;
    }
    else if (strEncodingProfile == L"444")
    {
        h264Profile = eAVEncH264VProfile_444;
    }
    else if (strEncodingProfile == L"extended")
    {
        h264Profile = eAVEncH264VProfile_Extended;
    }
    else if (strEncodingProfile == L"constrainedhigh")
    {
        h264Profile = eAVEncH264VProfile_UCConstrainedHigh;
    }
    else if (strEncodingProfile == L"constrainedbase" || strEncodingProfile == L"constrainedbaseline")
    {
        h264Profile = eAVEncH264VProfile_ConstrainedBase;
    }
    else
    {
        h264Profile = eAVEncH264VProfile_unknown;
    }

    *ph264Profile = h264Profile;

    return hr;
}

HRESULT CH264EncHelper::ConvertEncodingProfile_HEVC(
    __in WEX::Common::String strEncodingProfile, 
    __inout eAVEncH265VProfile *ph265Profile)
{
    HRESULT hr=  S_OK;
    eAVEncH265VProfile h265Profile = eAVEncH265VProfile_unknown;

    if (NULL == ph265Profile)
    {
        return E_POINTER;
    }
    if (strEncodingProfile.IsEmpty())
    {
        return E_INVALIDARG;
    }

    if (strEncodingProfile == L"unknown")
    {
        h265Profile = eAVEncH265VProfile_unknown;
    }
    else if (strEncodingProfile == L"main8_420")
    {
        h265Profile = eAVEncH265VProfile_Main_420_8;
    }
    else if (strEncodingProfile == L"main10_420")
    {
        h265Profile = eAVEncH265VProfile_Main_420_10;
    }
    else if (strEncodingProfile == L"main12_420")
    {
        h265Profile = eAVEncH265VProfile_Main_420_12;
    }
    else if (strEncodingProfile == L"main10_422")
    {
        h265Profile = eAVEncH265VProfile_Main_422_10;
    }
    else if (strEncodingProfile == L"main12_422")
    {
        h265Profile = eAVEncH265VProfile_Main_422_12;
    }
    else if (strEncodingProfile == L"main8_444")
    {
        h265Profile = eAVEncH265VProfile_Main_444_8;
    }
    else if (strEncodingProfile == L"main10_444")
    {
        h265Profile = eAVEncH265VProfile_Main_444_10;
    }
    else if (strEncodingProfile == L"main12_444")
    {
        h265Profile = eAVEncH265VProfile_Main_444_12;
    }
    else
    {
        h265Profile = eAVEncH265VProfile_unknown;
    }

    *ph265Profile = h265Profile;

    return hr;
}

HRESULT CH264EncHelper::ConvertEncodingProfile_VPX(
    __in WEX::Common::String strEncodingProfile,
    __inout eAVEncVP9VProfile *pVPXProfile)
{
    HRESULT hr = S_OK;
    eAVEncVP9VProfile VPXProfile = eAVEncVP9VProfile_unknown;

    if (NULL == pVPXProfile)
    {
        return E_POINTER;
    }
    if (strEncodingProfile.IsEmpty())
    {
        return E_INVALIDARG;
    }

    if (strEncodingProfile == L"unknown")
    {
        VPXProfile = eAVEncVP9VProfile_unknown;
    }
    else if (strEncodingProfile == L"main8_420")
    {
        VPXProfile = eAVEncVP9VProfile_420_8;
    }
    else if (strEncodingProfile == L"main10_420")
    {
        VPXProfile = eAVEncVP9VProfile_420_10;
    }
    else if (strEncodingProfile == L"main12_420")
    {
        VPXProfile = eAVEncVP9VProfile_420_12;
    }
    else
    {
        VPXProfile = eAVEncVP9VProfile_unknown;
    }

    *pVPXProfile = VPXProfile;

    return hr;
}

HRESULT CH264EncHelper::ConvertEncodingLevel(
    __in WEX::Common::String strEncodingLevel, 
    __inout UINT32 *pulEncodingLevel)
{
    HRESULT hr=  S_OK;

    if (NULL == pulEncodingLevel)
    {
        return E_POINTER;
    }
    if (strEncodingLevel.IsEmpty())
    {
        // No Input so setting it to the minimum value
        //*pulEncodingLevel = (UINT32)eAVEncH264VLevel1;
        *pulEncodingLevel = 0xffffffff;
    }

    if (!strEncodingLevel.IsEmpty())
    {
        unsigned short *pusEncodingLevel = strEncodingLevel.GetBuffer();
        swscanf_s(pusEncodingLevel, L"%lu", pulEncodingLevel);
    }

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify:: DecodeH264File(WCHAR* pszH264InFile, 
                      WCHAR* pszYUVOutFile, DWORD *pdwFrameSize, UINT32 uiTotalSamples)
{  
    
    HRESULT hr = S_OK;

    IMFMediaType* pInType = NULL;
    IMFMediaType* pOutType = NULL;
    IMFSample* pSample = NULL;
    IMFSourceReader *pReader = NULL;
    DWORD dwWidth, dwHeight;
    DWORD dwLength;
    DWORD dwStreamFlags = 0;
    BYTE* pBuf = NULL;
    IMFMediaBuffer *pMediaBuffer = NULL;
    char* buffer;
    HANDLE hOutFile = NULL;
    dwStreamFlags = 0;
    DWORD dwOutputFrameSize = 0;
    DWORD dwDecodedFrames = 0;
    do
    {
        //Create handle for the output file to write the decoded samples
        hOutFile = CreateFile (pszYUVOutFile, 
            GENERIC_WRITE, 
            FILE_SHARE_READ, 
            NULL, 
            CREATE_ALWAYS, 
            FILE_ATTRIBUTE_NORMAL, 
            NULL);

        if (hOutFile == INVALID_HANDLE_VALUE)
        {
            hr = E_FAIL;
            hOutFile = NULL;
            break;
        }
    
        ///Open the Input file 
        _HRL(MFCreateSourceReaderFromURL(pszH264InFile,NULL,&pReader),InternalLog(L"Failed to open first file for visval with hr %x\n",hr));
        _HRL(pReader->GetNativeMediaType(0,0,&pInType),InternalLog(L"Failed to get native media type\n"));
        
        ////Create decoder and set input type
        _HRL(MFCreateMediaType(&pOutType),InternalLog(L"Failed to create new media type\n"));
        _HRL(pInType->CopyAllItems(pOutType),InternalLog(L"Failed to copy media type attributes over to new media type\n"));
        _HRL(pOutType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video),InternalLog(L"Failed to set output type to video\n"));
        _HRL(pOutType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_IYUV),InternalLog(L"Failed to set output type to IYUV\n"));
        _HRL(pReader->SetCurrentMediaType(0,NULL,pOutType),InternalLog(L"Failed to set output type on source reader\n"));
        _HRL(MFGetAttributeSize(pOutType,MF_MT_FRAME_SIZE,(UINT32*)&dwWidth,(UINT32*)&dwHeight),InternalLog(L"Failed to get resolution from media type\n"));

        m_Width = dwWidth;
        m_Height = dwHeight;

        dwOutputFrameSize = dwWidth*dwHeight*3/2;
        
        while(!(dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM))
        {
            _HRL(pReader->ReadSample(0,0,NULL,&dwStreamFlags,NULL,&pSample),InternalLog(L"Failed to read decoded sample from file\n"));
            if(pSample != NULL )
            {
                _HRL(pSample->GetBufferByIndex(0,&pMediaBuffer),InternalLog(L"Failed to get buffer from sample\n"));
                _HRL(pMediaBuffer->Lock(&pBuf,NULL,&dwLength),InternalLog(L"Failed to lock buffer\n"));
                _HRL(WriteFrame(hOutFile,pBuf,dwLength), InternalLog(L"Failed to write frame to file"));
                _HRL(pMediaBuffer->Unlock(), InternalLog(L"Failed to unlock buffer\n"));
            }
            
            wprintf(L"\rProcessing: %5.2f%%", (100*(double)dwDecodedFrames/(double)(uiTotalSamples)));
            dwDecodedFrames++;
        }
    } while (FALSE);
        
    *pdwFrameSize = dwOutputFrameSize;

    if (hOutFile)
        CloseHandle(hOutFile);
    
    SAFE_RELEASE(pMediaBuffer);
    SAFE_RELEASE(pInType);
    SAFE_RELEASE(pOutType);
    SAFE_RELEASE(pSample);
    SAFE_RELEASE(pReader);

    return hr;
}


STDMETHODIMP
    CH264EncodeVerify::VerifyLTROutputFiles (WCHAR* pszFileNoLoss, WCHAR* pszFileLoss, DWORD dwFrameSize, DWORD dwTotalSamples, ErrorLossType errType)
                           
{
    HRESULT hr = S_OK;
    BOOL bEOFReachedNoLoss = FALSE;
    BOOL bEOFReachedLoss = FALSE;
    HANDLE hFileNoLoss = NULL;
    HANDLE hFileLoss = NULL;
    DWORD dwBytesRead = 0;
    BYTE *pInputbufferNoLoss = NULL;
    BYTE *pInputbufferLoss = NULL;

    DWORD dwLostFrame = 0;
    DWORD dwRecoveryFrame = 0;
    DWORD dwLostFrameIdx = 0;
    DWORD dwRecoveryFrameIdx = 0;
    DWORD dwCurrentSample = 0;
    BOOL bIsFrameLost = FALSE;
    DWORD dwFramePrevToLost = 0;

    String strErrFileNoLoss = L"NoLossErrfile.yuv";
    String strErrFileLoss = L"LossErrFile.yuv";

    HANDLE hErrFileNoLoss = CreateFile (strErrFileNoLoss.GetBuffer (), 
        GENERIC_WRITE, 
        FILE_SHARE_READ, 
        NULL, 
        CREATE_ALWAYS, 
        FILE_ATTRIBUTE_NORMAL, 
        NULL);

    HANDLE hErrFileLoss = CreateFile (strErrFileLoss.GetBuffer (), 
        GENERIC_WRITE, 
        FILE_SHARE_READ, 
        NULL, 
        CREATE_ALWAYS, 
        FILE_ATTRIBUTE_NORMAL, 
        NULL);

    do
    {
        pInputbufferNoLoss = new BYTE[dwFrameSize];
        pInputbufferLoss = new BYTE[dwFrameSize];

        if(pInputbufferNoLoss == NULL || pInputbufferLoss == NULL)
        {
            hr = E_OUTOFMEMORY;
            InternalLog(L"\n Failed to allocate memory \n");
            break;
        }

        if (hErrFileNoLoss == INVALID_HANDLE_VALUE || hErrFileLoss == INVALID_HANDLE_VALUE)
        {
            hr = E_FAIL;
            hErrFileNoLoss = NULL;
            hErrFileLoss = NULL;
            InternalLog(L"Invalid File handle\n");
            break;
        }

        if (pInputbufferNoLoss == NULL || pInputbufferLoss == NULL)
        {
            hr = E_FAIL;
            break;
        }

        hFileNoLoss = CreateFile (pszFileNoLoss,
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        hFileLoss = CreateFile (pszFileLoss,
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if (hFileNoLoss == INVALID_HANDLE_VALUE || hFileLoss == INVALID_HANDLE_VALUE)
        {
            hr = E_FAIL;
            hFileNoLoss = NULL;
            hFileLoss = NULL;
            break;
        }
        

        for (dwCurrentSample = 0 ; dwCurrentSample < dwTotalSamples ; dwCurrentSample++ )
        {
            if(dwLostFrameIdx < m_arrLTRLostFrames.GetSize())
                dwLostFrame = m_arrLTRLostFrames[dwLostFrameIdx];
            else
                dwLostFrame = dwTotalSamples; //verify from the last recovered frame until the last frame

            //*************add this for two pass without loss
            // errType = BIT_ERROR;
            //******************************//

            //Used to control the frame comparison in case of burst error /frame lost
            if(errType == BURST_ERROR)
            {
                dwFramePrevToLost = dwLostFrame - dwLostFrameIdx;
            }
            else if(errType == BIT_ERROR)
            {
                dwFramePrevToLost = dwLostFrame ;
            }
            else
            {
                _HRL(E_FAIL, InternalLog(L"\n Unknown error type \n"));
            }
            if (dwCurrentSample == dwLostFrame && dwCurrentSample!= 0)
            {
                if( m_arrRecoveryFrames.GetAt(dwRecoveryFrameIdx, &dwRecoveryFrame))
                {
                    dwRecoveryFrameIdx++;
                    dwLostFrameIdx++;
                }

                //Used to control the frame comparison in case of burst error /frame lost
                if(errType == BURST_ERROR)
                {
                    bIsFrameLost = TRUE;
                }
            }
                
            if (dwCurrentSample >= dwRecoveryFrame && dwCurrentSample < dwLostFrame && dwCurrentSample < dwFramePrevToLost)
            {
                if (!ReadFile(hFileNoLoss,pInputbufferNoLoss,dwFrameSize,&dwBytesRead,NULL) || (dwFrameSize != dwBytesRead))
                {
                    bEOFReachedNoLoss = true;
                }
                
                if (!ReadFile(hFileLoss,pInputbufferLoss,dwFrameSize,&dwBytesRead,NULL) || (dwFrameSize != dwBytesRead))
                {
                    bEOFReachedLoss = true;
                }

                if (bEOFReachedNoLoss || bEOFReachedLoss)
                {
                    break;
                }

                if (0 != memcmp (pInputbufferNoLoss, pInputbufferLoss, dwFrameSize))
                {
                    //write error samples to file here 
                    InternalLog(L"\n Failed in bit exact comparison on Sample number %d\n", dwCurrentSample);
                    _HRL(WriteFrame (hErrFileNoLoss, pInputbufferNoLoss, dwFrameSize), InternalLog(L"Failed to write error frame from LTR_No_loss.yuv to file \n"));
                    _HRL(WriteFrame (hErrFileLoss, pInputbufferLoss, dwFrameSize), InternalLog(L"Failed to write error frame from LTR_loss.yuv to file \n"));
                    hr = E_FAIL;
                    break;
                }            
                bIsFrameLost = FALSE;
            }
            else
            {
                LONG lTemp = 0;
                if(bIsFrameLost)
                {
                    //go forward a frame in the LTR No Loss file every time a frame is lost as the lost frame isnt written
                    if (!SetFilePointer(hFileNoLoss, dwFrameSize*2, &lTemp, FILE_CURRENT))
                    {
                        break;
                    }
                    bIsFrameLost = FALSE;
                }
                else
                {
                    if(!SetFilePointer(hFileNoLoss, dwFrameSize, &lTemp, FILE_CURRENT))
                    {
                        break;
                    }
                }
                lTemp = 0;
                if (!SetFilePointer(hFileLoss, dwFrameSize, &lTemp, FILE_CURRENT))
                {
                    break;
                }
            }
            wprintf (L"Processing: %5.2f percent\r", (100 * (double)dwCurrentSample / (double)dwTotalSamples));
        }

    }while(FALSE);

    SAFE_ARRAY_DELETE (pInputbufferNoLoss);
    SAFE_ARRAY_DELETE (pInputbufferLoss); 

    if (hFileNoLoss)
        CloseHandle(hFileNoLoss);
    if (hFileLoss)
        CloseHandle(hFileLoss);

    if (hErrFileNoLoss)
        CloseHandle(hErrFileNoLoss);
    if (hErrFileLoss)
        CloseHandle(hErrFileLoss);

    return hr;
}



STDMETHODIMP
CH264EncodeVerify::VerifyIntraRefreshOutputFiles(WCHAR* pszFile, WCHAR* pszFileFrameDrop, DWORD dwFrameSize, DWORD dwTotalSamples)

{
    HRESULT hr = S_OK;
    BOOL bEOFReached = FALSE;
    BOOL bEOFReachedFrameDrop = FALSE;
    HANDLE hFile = NULL;
    HANDLE hFileFrameDrop = NULL;
    DWORD dwBytesRead = 0;
    BYTE *pInputbuffer = NULL;
    BYTE *pInputbufferFrameDrop = NULL;

    INTRA_REFRESH_FRAMETYPE eIntraRefreshFrameType = INTRA_REFRESH_FRAMETYPE(0);
    DWORD dwCurrentSample = 0;
    LONG lDistanceToMove = dwFrameSize;
    int nComp = 0;

    CDXCompare dxCompare;

    // Mean square error
    double lPSNRY = 0;
    double lPSNRU = 0;
    double lPSNRV = 0;
    

    String strErrFile = L"Errfile.yuv";
    String strErrFileFrameDrop = L"FrameDropErrFile.yuv";

    HANDLE hErrFile = CreateFile(strErrFile.GetBuffer(),
        GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    HANDLE hErrFileFrameDrop = CreateFile(strErrFileFrameDrop.GetBuffer(),
        GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    do
    {
        pInputbuffer = new BYTE[dwFrameSize];
        pInputbufferFrameDrop = new BYTE[dwFrameSize];

        if (pInputbuffer == NULL || pInputbufferFrameDrop == NULL)
        {
            hr = E_OUTOFMEMORY;
            InternalLog(L"\n Failed to allocate memory \n");
            break;
        }

        if (hErrFile == INVALID_HANDLE_VALUE || hErrFileFrameDrop == INVALID_HANDLE_VALUE)
        {
            hr = E_FAIL;
            hErrFile = NULL;
            hErrFileFrameDrop = NULL;
            InternalLog(L"Invalid File handle\n");
            break;
        }

        if (pInputbuffer == NULL || pInputbufferFrameDrop == NULL)
        {
            hr = E_FAIL;
            break;
        }

        hFile = CreateFile(pszFile,
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        hFileFrameDrop = CreateFile(pszFileFrameDrop,
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if (hFile == INVALID_HANDLE_VALUE || hFileFrameDrop == INVALID_HANDLE_VALUE)
        {
            hr = E_FAIL;
            hFile = NULL;
            hFileFrameDrop = NULL;
            break;
        }

        UINT32 uiMode = 0; 
        UINT32 uiFrames = 0;
        UINT32 uiContinualRecoveryFrames = 0;
        UINT32 uiMaxNonIdentical = 0;
        if (S_OK != m_pH264CustomParser->GetIntraRefreshInfo(&uiMode, &uiFrames))
        {
            // We should never get here because this is checked in the calling function but just in case
            _HRL(S_SKIPPED, InternalLog(L"Test skipped because -intrarefresh was not specified\n"));
        }
        uiMaxNonIdentical = uiFrames / 2; // If we are in intra refresh mode 2 then we expect the video to not recover within half the refresh interval 
                                

        _HRL(dxCompare.EnableSingleFrame(m_Width, m_Width, m_Height, MFVideoFormat_IYUV, TRUE, FALSE), InternalLog(L"Failed to set DXCompare to Single frame mode"));

        for (dwCurrentSample = 0; dwCurrentSample < dwTotalSamples; dwCurrentSample++)
        {
            eIntraRefreshFrameType = m_arrIntraRefreshFrames[dwCurrentSample];


            if (eIntraRefreshFrameType == INTRA_REFRESH_FRAMETYPE_DROPPED)
            {
                //nothing to compare, just skip this frame
                if (!SetFilePointer(hFile, lDistanceToMove, NULL, FILE_CURRENT))
                {
                    break;
                }
            }
            else
            {
                //Skip till we get a frame from the decoder in the case of frame drop
                if (!ReadFile(hFile, pInputbuffer, dwFrameSize, &dwBytesRead, NULL) || (dwFrameSize != dwBytesRead))
                {
                    bEOFReached = true;
                }

                if (bEOFReached)
                {
                    break;
                }

                if (!ReadFile(hFileFrameDrop, pInputbufferFrameDrop, dwFrameSize, &dwBytesRead, NULL) || (dwFrameSize != dwBytesRead))
                {
                    bEOFReachedFrameDrop = true;
                }

                if (bEOFReachedFrameDrop)
                {
                    break;
                }

                if (eIntraRefreshFrameType != INTRA_REFRESH_FRAMETYPE_CORRUPT)
                {
                    nComp = memcmp(pInputbuffer, pInputbufferFrameDrop, dwFrameSize);
                    if (eIntraRefreshFrameType == INTRA_REFRESH_FRAMETYPE_IDENTICAL)
                    {
                        //should be identical
                        if (nComp != 0)
                        {
                            //write error samples to file here 
                            InternalLog(L"\n Failed in bit exact comparison on Sample number %d\n", dwCurrentSample);
                            _HRL(WriteFrame(hErrFile, pInputbuffer, dwFrameSize), InternalLog(L"Failed to write error frame from IntraRefresh_No_FrameDrop.yuv to file \n"));
                            _HRL(WriteFrame(hErrFileFrameDrop, pInputbufferFrameDrop, dwFrameSize), InternalLog(L"Failed to write error frame from IntraRefresh_FrameDrop.yuv to file \n"));
                            hr = E_FAIL;
                            break;
                        }
                    }
                    else if (eIntraRefreshFrameType == INTRA_REFRESH_FRAMETYPE_PARTIALLY_RECOVERED)
                    {
                        BOOL fNonRecoveredFrame = TRUE;
                        if (2 == uiMode)
                        {
                            // If we are using mode 2 then we only check the first interval / 2 frames to ensure that there is not full recovery
                            if (uiContinualRecoveryFrames++ > uiMaxNonIdentical)
                            {
                                fNonRecoveredFrame = FALSE;
                            }
                        }
                        if (fNonRecoveredFrame && nComp == 0)
                        {
                            _HRL(E_FAIL, InternalLog(L"\n Error in test... The partially recovered frame %d cannot be identical\n", dwCurrentSample))
                        }
                    }
                    else if (eIntraRefreshFrameType == INTRA_REFRESH_FRAMETYPE_RECOVERED)
                    {
                        dxCompare.ProcessSingleFrame(pInputbuffer, pInputbufferFrameDrop, &lPSNRY, &lPSNRU, &lPSNRV);
                        if (lPSNRY < 50 || lPSNRU < 50 || lPSNRV < 50) 
                        {
                            _HRL(E_FAIL, InternalLog(L"\n PSNR is not acceptable for recovered frames %d, YPSNR = %f, UPSNR = %f, VPSNR = %f\n", dwCurrentSample, lPSNRY, lPSNRU, lPSNRV));
                        }
                    }
                }
            }

            wprintf(L"Processing: %5.2f percent\r", (100 * (double) dwCurrentSample / (double) dwTotalSamples));
        }

    } while (FALSE);

    SAFE_ARRAY_DELETE(pInputbuffer);
    SAFE_ARRAY_DELETE(pInputbufferFrameDrop);

    if (hFile)
        CloseHandle(hFile);
    if (hFileFrameDrop)
        CloseHandle(hFileFrameDrop);

    if (hErrFile)
        CloseHandle(hErrFile);
    if (hErrFileFrameDrop)
        CloseHandle(hErrFileFrameDrop);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify:: WriteFrame (HANDLE hStream, BYTE *pBuffer, DWORD dwBytesToWrite)
{
    HRESULT hr = S_OK;

    DWORD dwBytesWritten;
    if (!hStream || hStream == INVALID_HANDLE_VALUE)
        hr = E_HANDLE;

    BOOL bResult = WriteFile (hStream, pBuffer, dwBytesToWrite, &dwBytesWritten, NULL);
    if (!bResult || dwBytesWritten != dwBytesToWrite) {
        hr = E_FAIL;
    }

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::ExtractAndVerifyLayers(DWORD dwLayerCount, DWORD dwTotalSampleCount, DWORD dwWidth, DWORD dwHeight)
{
    HRESULT hr = S_OK;
    CH264EncVerifier *pEncVerifier = NULL;
    CH264EncLayerExtract *pEncLayerExtract = NULL;
    WCHAR* pBaseFileNameWch = NULL;
    WCHAR* pInputFileNameWch = NULL;
    WEX::Common::String strTempFileName;
    WCHAR* pTempFileNameWch0 = NULL;
    WCHAR* pTempFileNameWch1 = NULL;
    WCHAR* pTempFileNameWch2 = NULL;
    DWORD dwGOPCount = 0;
    DWORD dwFrameSize = 0;
    PROPVARIANT propvar;
    HANDLE hInFile0 = NULL;
    HANDLE hInFile1 = NULL;
    HANDLE hInFile2 = NULL;
    BYTE *inputbuffer0 = NULL;
    BYTE *inputbuffer1 = NULL;
    BYTE *inputbuffer2 = NULL;
    BOOL bEOFReached0 = FALSE;
    BOOL bEOFReached1 = FALSE;
    BOOL bEOFReached2 = FALSE;

    PropVariantInit(&propvar);

    do{
        if (dwLayerCount < 2)
            break;

        pEncLayerExtract = new CH264EncLayerExtract(m_pLog);
        if (NULL == pEncLayerExtract)
        {
            _HR(hr = E_OUTOFMEMORY);
        }

        // Check extension
        pInputFileNameWch = _wcsdup(m_strOutputFile);
        WCHAR* pExtension = wcsrchr(pInputFileNameWch,L'.');
        if ( wcslen(pExtension) < 3){
            InternalLog(L"ERROR: Input file extension is not 3 characters long! File name: %s", pInputFileNameWch);
            hr = E_FAIL;
            break;
        }
        // Create verifier and parse original H.264 bitstream using bitstream analyzer
        pEncVerifier = new CH264EncVerifier();
        if (NULL == pEncVerifier)
        {
            _HR(hr = E_OUTOFMEMORY);
        }
        _HRL (pEncVerifier->ParseBSAnalyzer (pInputFileNameWch, &LogToGlobalLogger),
            InternalLog(L"Failed bitstream analysis on input stream %s with hr = %x\n", pInputFileNameWch, hr));
        SAFE_DELETE(pEncVerifier);

        //  Get base file name
        wcscpy(pExtension, L".264");
        pBaseFileNameWch = _wcsdup(pInputFileNameWch);
        pExtension = wcsrchr(pBaseFileNameWch,L'.');
        *pExtension = L'\0';

        // Create file name for layer 0
        strTempFileName = WEX::Common::String(pBaseFileNameWch);
        strTempFileName = strTempFileName + L"Layer0.264"; 
        pTempFileNameWch0 = strTempFileName.GetBuffer();

        _HRL(pEncLayerExtract->ExtractLayer(pInputFileNameWch, pTempFileNameWch0, 0),
            InternalLog(L"Failed to extract layer-0 from %s", pInputFileNameWch));

        // Create verifier and parse layer0 bitstream using bitstream analyzer
        pEncVerifier = new CH264EncVerifier();
        if (NULL == pEncVerifier)
        {
            _HR(hr = E_OUTOFMEMORY);
        }
        _HRL(hr = pEncVerifier->ParseBSAnalyzer(pTempFileNameWch0, &LogToGlobalLogger),
            InternalLog(L"Failed bitstream analysis on input stream %s with hr = %x\n",pTempFileNameWch0, hr));
        SAFE_DELETE(pEncVerifier);

        // Extract and parse layer1
        if(dwLayerCount > 2){
            strTempFileName = WEX::Common::String(pBaseFileNameWch);
            strTempFileName = strTempFileName + L"Layer1.264"; 
            pTempFileNameWch0 = strTempFileName.GetBuffer();
            _HRL(pEncLayerExtract->ExtractLayer ( pInputFileNameWch, pTempFileNameWch0, 1),
                InternalLog(L"Failed to extract layer-1 from %s", pInputFileNameWch));
            pEncVerifier = new CH264EncVerifier();
            if (NULL == pEncVerifier)
            {
                _HR(hr = E_OUTOFMEMORY);
            }
            // Create verifier parse layer1 bitstream using bitstream analyzer
            _HRL(hr = pEncVerifier->ParseBSAnalyzer(pTempFileNameWch0, &LogToGlobalLogger),
                InternalLog(L"Failed bitstream analysis on input stream %s with hr = %x\n",pTempFileNameWch0, hr));
            SAFE_DELETE(pEncVerifier);
        }

        InternalLog(L"Start decoding whole stream\n");

        strTempFileName = WEX::Common::String(pBaseFileNameWch);
        strTempFileName = strTempFileName + L"AllLayers.yuv"; 
        pTempFileNameWch0 = strTempFileName.GetBuffer();

        _HR (pEncLayerExtract->Decode264File(
            pInputFileNameWch, 
            pTempFileNameWch0,
            MFVideoFormat_I420,
            dwWidth,
            dwHeight,
            &dwFrameSize, 
            dwTotalSampleCount,
            FALSE));

        InternalLog(L"Start decoding layer-0\n");
        dwFrameSize = 0;
        strTempFileName = WEX::Common::String(pBaseFileNameWch);
        strTempFileName = strTempFileName + L"Layer0.yuv"; 
        pTempFileNameWch1 = strTempFileName.GetBuffer();

        _HR (pEncLayerExtract->Decode264File(
            pInputFileNameWch, 
            pTempFileNameWch1,
            MFVideoFormat_I420,
            dwWidth,
            dwHeight,
            &dwFrameSize, 
            dwTotalSampleCount,
            FALSE));

        if (dwFrameSize == 0)
        {
            hr = E_FAIL;
            _HR (hr);
        }

        inputbuffer0 = new BYTE[dwFrameSize];
        inputbuffer1 = new BYTE[dwFrameSize];

        if (inputbuffer0 == NULL || inputbuffer1 == NULL)
        {
            hr = E_FAIL;
            _HR (hr);
        }

        hInFile0 = CreateFile (pTempFileNameWch0,
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
        hInFile1 = CreateFile (pTempFileNameWch1,
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL); 
        if (hInFile0 == INVALID_HANDLE_VALUE || hInFile1 == INVALID_HANDLE_VALUE)
        {
            hr = E_FAIL;
            hInFile0 = NULL;
            hInFile1 = NULL;
            _HR (hr);
        }

        if (dwLayerCount > 2)
        {
            InternalLog(L"Start decoding layer-0 and layer-1\n");
            dwFrameSize = 0;
            strTempFileName = WEX::Common::String(pBaseFileNameWch);
            strTempFileName = strTempFileName + L"Layer01.yuv"; 
            pTempFileNameWch2 = strTempFileName.GetBuffer();

            _HR (pEncLayerExtract->Decode264File (
                pInputFileNameWch, 
                pTempFileNameWch2,
                MFVideoFormat_I420, 
                dwWidth,
                dwHeight,
                &dwFrameSize, 
                dwTotalSampleCount,
                FALSE));
            if (dwFrameSize == 0)
            {
                hr = E_FAIL;
                _HR (hr);
            }
            inputbuffer2 = new BYTE[dwFrameSize];

            if (inputbuffer2 == NULL)
            {
                hr = E_FAIL;
                _HR (hr);
            }

            hInFile2 = CreateFile (pTempFileNameWch2,
                GENERIC_READ,
                FILE_SHARE_READ,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL);
            if (hInFile2 == INVALID_HANDLE_VALUE)
            {
                hr = E_FAIL;
                hInFile2 = NULL;
                _HR (hr);
            }
        }

        // Verifying
        InternalLog(L"Decoding done, now verifying the extracted layers for bit exactness\n");
        pEncVerifier = new CH264EncVerifier();
        if (NULL == pEncVerifier)
        {
            _HR(hr = E_OUTOFMEMORY);
        }

        PropVariantClear(&propvar);
        _HR (pEncVerifier->GetValue (GOPCount, &propvar));
        NT_ASSERT (VT_UI4 == propvar.vt);
        dwGOPCount = propvar.ulVal;


        for (DWORD dwGOPIndex = 0; dwGOPIndex < dwGOPCount; dwGOPIndex++)
        {
            PropVariantClear(&propvar);
            _HR (pEncVerifier->GetValueAtGOPIndex (dwGOPIndex, FrameCountAt, &propvar));
            NT_ASSERT (VT_UI4 == propvar.vt);
            DWORD dwFrameCount = propvar.ulVal;

            for (DWORD dwFrameIndex= 0; dwFrameIndex < dwFrameCount; dwFrameIndex++)
            {            
                DWORD dwBytesRead = 0;
                DWORD dwLayerId = 0;
                PropVariantClear(&propvar);
                _HR (pEncVerifier->GetValueAtFrameIndex (dwGOPIndex, dwFrameIndex, TemporalLayerAt, &propvar));
                NT_ASSERT (VT_UI4 == propvar.vt);
                dwLayerId = propvar.ulVal;

                if ( dwLayerId == 0 || dwLayerId == 1)
                {
                    if (!ReadFile(hInFile0, inputbuffer0, dwFrameSize, &dwBytesRead, NULL) || (dwFrameSize != dwBytesRead))
                    {
                        bEOFReached0 = true;
                        break;
                    }

                    if ( dwLayerId == 0)
                    {
                        if (!ReadFile(hInFile1, inputbuffer1, dwFrameSize, &dwBytesRead, NULL) || (dwFrameSize != dwBytesRead))
                        {
                            bEOFReached1 = true;
                            break;
                        }
                        if (0 != memcmp (inputbuffer0, inputbuffer1, dwFrameSize))
                        {
                            InternalLog(L"\n Memcmp failed on FrameIndex=%d\n", dwFrameIndex);
                            hr = E_FAIL;
                            _HR (hr);
                        }
                    }

                    if (dwLayerCount > 2)
                    {
                        if (!ReadFile(hInFile2, inputbuffer2, dwFrameSize, &dwBytesRead, NULL) || (dwFrameSize != dwBytesRead))
                        {
                            bEOFReached2 = true;
                            break;
                        }
                        if (0 != memcmp (inputbuffer0, inputbuffer2, dwFrameSize))
                        {
                            InternalLog(L"\n Memcmp failed on FrameIndex=%d\n", dwFrameIndex);
                            hr = E_FAIL;
                            _HR (hr);
                        }
                    }
                }
                else
                {
                    LONG lTemp = 0;
                    if (!SetFilePointer(hInFile0, dwFrameSize, &lTemp, FILE_CURRENT))
                    {
                        bEOFReached0 = true;
                        break;
                    }
                }
            }

            if (bEOFReached0 || bEOFReached1 || bEOFReached2)
            {
                hr = E_FAIL;
                _HR (hr);
            }
        }  

        strTempFileName.~String();

    }while(FALSE);

    SAFE_DELETE(pEncVerifier);
    SAFE_DELETE(pEncLayerExtract);

    SAFE_FREE(pTempFileNameWch2);
    SAFE_FREE(pTempFileNameWch1);
    SAFE_FREE(pTempFileNameWch0);
    SAFE_FREE(pBaseFileNameWch);
    SAFE_FREE(pInputFileNameWch);

    SAFE_ARRAY_DELETE (inputbuffer0);
    SAFE_ARRAY_DELETE (inputbuffer1);
    SAFE_ARRAY_DELETE (inputbuffer2); 

    if (hInFile0)
        CloseHandle(hInFile0);
    if (hInFile1)
        CloseHandle(hInFile1);
    if (hInFile2)
        CloseHandle(hInFile2);

    return hr;
}

STDMETHODIMP
    CH264EncodeVerify::GetInputFrameCount(UINT32 *uiInputFrameCount)
{
    HRESULT hr = S_OK;
    IMFSourceReader *pInputSource = NULL;
    IMFMediaType *pCurrentType = NULL;
    IMFSample *pInputSample = NULL;
    UINT32 uiFrameCount = 0;
    DWORD streamFlags = 0;
    ULONGLONG ullDuration = 0;
    LONGLONG llTimeStamp = 0;

    if (NULL == uiInputFrameCount)
    {
        return E_POINTER;
    }

    do
    {
        if (m_uiInputFrameCount != 0)
        {
            break;
        }

        _HR((m_pConfigHelper) ? S_OK : E_UNEXPECTED);

        // Read input frame number
        m_pConfigHelper->GetInputFrameCountFromConfig(&uiFrameCount);

        if(uiFrameCount != 0)
        {
            m_uiInputFrameCount = uiFrameCount;
            break;
        }

        m_pConfigHelper->GetTestDuration(&ullDuration);
        if (ullDuration == 0)
        {
            ullDuration = (ULONGLONG)-1;
        }


        // Read input file path from the test xml
        WEX::Common::String strInputFile = m_pConfigHelper->GetInputPath();
        if (strInputFile.IsEmpty())
        {
            _HR(E_INVALIDARG);
        }

        _HRL(MFCreateSourceReaderFromURL((LPCWSTR)strInputFile , NULL, &pInputSource), InternalLog(L"Fail at CreateSourceReaderFromURL\n"));
        _HRL(pInputSource->GetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, &pCurrentType),InternalLog(L"Fail at GetCurrentMediaType\n"));
        _HRL(pCurrentType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_NV12),InternalLog(L"SetGUID to NV12 failed\n"));
        do
        {
            SAFE_RELEASE(pInputSample);
            _HRL(pInputSource->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, NULL,&streamFlags,&llTimeStamp,&pInputSample),
                InternalLog(L"Failed to read sample from source file with hr %x",hr));
            if(pInputSample != NULL)
            {
                uiFrameCount++;
            }
        }while((streamFlags & MF_SOURCE_READERF_ENDOFSTREAM) == 0 || llTimeStamp >= ullDuration);

        m_uiInputFrameCount = uiFrameCount;
    }while(false);

    *uiInputFrameCount = m_uiInputFrameCount;

    SAFE_RELEASE(pInputSample);
    SAFE_RELEASE(pCurrentType);
    SAFE_RELEASE(pInputSource);
   
    return hr;

}

STDMETHODIMP
    CH264EncodeVerify::OverrideEntropyConfig(BOOL fEnableCABAC)
{
    m_ulEntropyCodingConfig = (UINT32)fEnableCABAC;

    return S_OK;
}


STDMETHODIMP
CH264EncodeVerify::H264BitStreamEncodeWithIntraRefresh()
{
    HRESULT hr = S_OK;

    do
    {
        _HR((m_pConfigHelper) ? S_OK : E_UNEXPECTED);

        // Read input file path from the test xml
        WEX::Common::String strInputFile = m_pConfigHelper->GetInputPath();
        if (strInputFile.IsEmpty())
        {
            _HR(E_INVALIDARG);
        }

        _HRL(StringCchCopyW(m_pwcLocalInputFile, MAX_PATH, (LPCWSTR) strInputFile), InternalLog(L" \n Failed to copy the input file name \n"));

        // Read output file format from the test xml (L"container")
        m_output = m_pConfigHelper->GetOutputFormat();

        // Read output file path from the test xml
        if (MFTopologyHelper::Renderer != m_output)
        {
            m_strOutputFile = m_pConfigHelper->GetOutputPath();
            if (m_strOutputFile.IsEmpty())
            {
                _HR(E_INVALIDARG);
            }
            TCHAR* truncFileName = NULL;
            _HRL(StringCchCopyW(m_pwcIntraRefreshLossOutputFile, MAX_PATH, (LPCWSTR) m_strOutputFile), InternalLog(L" \n Failed to copy the output file name\n"));
            _HRL(StringCchCopyW(m_pwcIntraRefreshOutputFile, MAX_PATH, (LPCWSTR) m_strOutputFile), InternalLog(L" \n Failed to copy the output file name\n"));

            truncFileName = _tcsstr(m_pwcIntraRefreshLossOutputFile, L".mp4");
            if (truncFileName != NULL)
            {
                m_pwcIntraRefreshLossOutputFile[truncFileName - m_pwcIntraRefreshLossOutputFile] = '\0';
                _tcsncat(m_pwcIntraRefreshLossOutputFile, L"_Loss.mp4", 9);

            }

            truncFileName = _tcsstr(m_pwcIntraRefreshOutputFile, L".mp4");
            if (truncFileName != NULL)
            {
                m_pwcIntraRefreshOutputFile[truncFileName - m_pwcIntraRefreshOutputFile] = '\0';
                _tcsncat(m_pwcIntraRefreshOutputFile, L"_.mp4", 11);
            }

        }

        _HR(m_pConfigHelper->GetConfigCustomParser((ITAEFConfigCustomParser **) &m_pH264CustomParser));


        //Encode with IntraRefresh enabled but no FrameDrop
        InternalLog(L"\n Encoding with IntraRefresh enabled but no FrameDrop \n");
        m_strOutputFile = m_pwcIntraRefreshOutputFile; //set output file

        m_pSampleProcessor = new CDirectSampleProcessor(this);
        if (NULL == m_pSampleProcessor)
        {
            _HR(E_OUTOFMEMORY);
        }
        _HRL(m_pSampleProcessor->ProcessSamples(0), InternalLog(L"Failed to encode\n"));
        SAFE_DELETE(m_pSampleProcessor);


        //Encode with IntraRefresh enabled and with network FrameDrop
        InternalLog(L"\nEncoding with IntraRefresh enabled but with packet FrameDrop \n");
        m_strOutputFile = m_pwcIntraRefreshLossOutputFile; //set output file

        //Get Intra Refresh mode and value
        UINT32 uiMode, uiFrames;
        if (S_OK != m_pH264CustomParser->GetIntraRefreshInfo(&uiMode, &uiFrames))
        {
            _HRL(S_SKIPPED, InternalLog(L"Test skipped because -intrarefresh was not specified\n"));
        }
        else
        {
            m_bIntraRefreshSet = TRUE;
            m_uiIntraRefreshFrames = uiFrames;
            m_uiIntraRefreshMode = uiMode;
        }

        UINT32 uiNumFrames = 0;
        UINT32 uiGopSize = 0;
        _HRL(m_pConfigHelper->GetGOPSize(&uiGopSize), InternalLog(L"Couldn't get GOP size\n"));
        _HRL(GetInputFrameCount(&uiNumFrames), InternalLog(L"Couldn't get total input frame count\n"));

        InternalLog(L" \n #Input Frame Count : %d \n", uiNumFrames);

        if (m_bIntraRefreshSet == TRUE)
        {
            m_arrIntraRefreshFrames.InsertAt(0, INTRA_REFRESH_FRAMETYPE_IDENTICAL);

            //Always drop the second frame. If we drop the first frame, the decoder tries to get an IDR for 120 frames.
            m_arrIntraRefreshFrames.InsertAt(1, INTRA_REFRESH_FRAMETYPE_DROPPED);
            
            UINT32 idxStartRecovery = 0;
            UINT32 idxStopRecovery = 0;

            switch (m_uiIntraRefreshMode)
            {
            case 0:
                //In mode 0, the default behavior is applicable where full recovery happens at next IDR frame.
                idxStartRecovery = uiGopSize;
                idxStopRecovery = uiGopSize;
                break;
            case 1:
                //In mode 1, the recovery starts at the beginning of the next GOP and recovery should have completed in m_uiIntraRefreshFrames
                idxStartRecovery = uiGopSize;
                idxStopRecovery = uiGopSize + m_uiIntraRefreshFrames - 1;
                break;
            case 2:
                //Whereas, in mode 2, the recovery starts at the next frame and continues to recover hence forth.
                //Since the requirement is to hve every macroblock refreshed atleast once in m_uiIntraRefreshFrames, the actual recovery might happen much later.
                //The interval after we expect complete recovery is 5*m_uiIntraRefreshFrames
                idxStartRecovery = 2;
                idxStopRecovery = 2 + m_uiIntraRefreshFrames * 5; 
                break;

            };

            for (int i = 2; i < idxStartRecovery; i++)
            {
                m_arrIntraRefreshFrames.InsertAt(i, INTRA_REFRESH_FRAMETYPE_CORRUPT);
            }
            for (int i = idxStartRecovery; i < idxStopRecovery; i++)
            {
                m_arrIntraRefreshFrames.InsertAt(i, INTRA_REFRESH_FRAMETYPE_PARTIALLY_RECOVERED);
            }
            for (int i = idxStopRecovery; i < uiNumFrames; i++)
            {
                if (m_uiIntraRefreshMode == 2)
                {
                    m_arrIntraRefreshFrames.InsertAt(i, INTRA_REFRESH_FRAMETYPE_RECOVERED);
                }
                else
                {
                    m_arrIntraRefreshFrames.InsertAt(i, INTRA_REFRESH_FRAMETYPE_IDENTICAL);
                }
            }
        }

        m_pSampleProcessor = new CDirectSampleProcessor(this);
        if (NULL == m_pSampleProcessor)
        {
            _HR(E_OUTOFMEMORY);
        }
        _HRL(m_pSampleProcessor->ProcessSamples(0), InternalLog(L"Failed to encode\n"));
        SAFE_DELETE(m_pSampleProcessor);

        //Decoding and verification
        DWORD dwFrameSize = 0;
        UINT32 uiTotalSamples = 0;

        uiTotalSamples = uiNumFrames;

        InternalLog(L"\n Decoding File 1\t\t\n");
        WCHAR* strYuv = L"IntraRefresh.yuv";
        _HRL(DecodeH264File(m_pwcIntraRefreshOutputFile, strYuv, &dwFrameSize, uiTotalSamples), InternalLog(L"Failed to decode the first file\n"));

        InternalLog(L"\n Decoding File 2\t\t\n");
        WCHAR* strYuvFrameDrop = L"IntraRefresh_framedrop.yuv";

        uiTotalSamples = m_arrIntraRefreshFrames.GetSize();

        _HRL(DecodeH264File(m_pwcIntraRefreshLossOutputFile, strYuvFrameDrop, &dwFrameSize, uiTotalSamples), InternalLog(L"Failed to decode the second file with dropped frames \n"));

        InternalLog(L"\n Comparing Two Files\t\t\n");
        hr = VerifyIntraRefreshOutputFiles(strYuv, strYuvFrameDrop, dwFrameSize, uiTotalSamples);
        if (hr != E_SKIP)
        {
            break;
        }

    } while (TRUE);

    SAFE_DELETE(m_pSampleProcessor);
    SAFE_DELETE(m_pH264EncVerifier);

    return hr;
}

STDMETHODIMP
CH264EncodeVerify::RunGradualIntraRefreshScenario()
{
    HRESULT hr = S_OK;
    do
    {
        _HR(H264BitStreamEncodeWithIntraRefresh());
        if (!m_pConfigHelper->IsKeepFilesSet())
        {
            DeleteFile(m_pwcIntraRefreshOutputFile);
            DeleteFile(m_pwcIntraRefreshLossOutputFile);
        }

    } while (FALSE);

    return hr;
}

