// Copyright(C) Microsoft. All rights reserved.

#pragma once
//
// Processor tests
DWORD Test_Processor_ObjectCreate();
DWORD Test_Processor_ConnectionCreateInvalidValues();
DWORD Test_Processor_ConnectionCreateInvalidParams();
DWORD Test_Processor_ConnectionReleaseInvalid();
DWORD Test_Processor_DuplicateConnection();
DWORD Test_APORegister_SystemAPOsRegistered();
DWORD Test_Processor_CreateAllApos();
DWORD Test_Processor_ReleaseApoInvalid();
DWORD Test_Processor_AttachApoConnectionValid();
DWORD Test_Processor_ReleaseAPOWithAttachedConnection();
DWORD Test_Processor_AttachApoConnectionInvalid();
DWORD Test_Processor_AttachEndpointTest();
DWORD Test_Processor_RemoveApoConnection();
DWORD Test_Processor_AttachMultipleConnectionInvalid();
DWORD Test_Processor_ActivateApoTest();
DWORD Test_Processor_TransactionTest();
DWORD Test_Processor_SwapConnectionTest();
DWORD Test_Processor_TransactionTimeoutTest();

//
// APO Tests
//
DWORD Test_SRCAPO_InitializeInvalid();
DWORD Test_SRCAPO_CalculateFrames();
DWORD Test_SRCAPO_CalcOutputFramesStateTesting();
DWORD Test_SRCAPO_CalcInputFramesStateTesting();


//
// SRC Functional tests
//
DWORD Test_FixedFormatConvertor_SNR_Linear32();
DWORD Test_FixedFormatConvertor_SNR_Linear32_Multi();
DWORD Test_FixedFormatConvertor_SNR_Linear32_Pull();
DWORD Test_FixedFormatConvertor_SNR_Linear32_Multi_Pull();
DWORD Test_FixedFormatConvertor_SNR_Linear32_Nyq();
DWORD Test_FixedFormatConvertor_SNR_Linear16();
DWORD Test_FixedFormatConvertor_SNR32_KMIXER_2();
DWORD Test_FixedFormatConvertor_SNR32_KMIXER_1();

//
// SRC Sinc Functional tests
DWORD Test_FixedFormatConvertor_SNR_Sinc1_32();
DWORD Test_FixedFormatConvertor_SNR_Sinc2_32();
DWORD Test_FixedFormatConvertor_SNR_Sinc3_32();
//
// 16 bit sinc mono, push
DWORD Test_FixedFormatConvertor_SNR_Sinc1_16();
DWORD Test_FixedFormatConvertor_SNR_Sinc2_16();
DWORD Test_FixedFormatConvertor_SNR_Sinc3_16();
//
// Nyquist test sinc mono, push
DWORD Test_FixedFormatConvertor_SNR_Sinc1_32_Nyq();
DWORD Test_FixedFormatConvertor_SNR_Sinc2_32_Nyq();
DWORD Test_FixedFormatConvertor_SNR_Sinc3_32_Nyq();
//
// Multichannel test sinc  push
DWORD Test_FixedFormatConvertor_SNR_Sinc1_32_Multi();
DWORD Test_FixedFormatConvertor_SNR_Sinc2_32_Multi();
DWORD Test_FixedFormatConvertor_SNR_Sinc3_32_Multi();
//
// Mono test sinc pull
DWORD Test_FixedFormatConvertor_SNR_Sinc1_32_Pull();
DWORD Test_FixedFormatConvertor_SNR_Sinc2_32_Pull();
DWORD Test_FixedFormatConvertor_SNR_Sinc3_32_Pull();
//
// Multichannel test sinc pull
DWORD Test_FixedFormatConvertor_SNR_Sinc1_32_Multi_Pull();
DWORD Test_FixedFormatConvertor_SNR_Sinc2_32_Multi_Pull();
DWORD Test_FixedFormatConvertor_SNR_Sinc3_32_Multi_Pull();

//
// Generic APO API test
//
DWORD Test_APO_RequiredInterfaces();
DWORD Test_APO_LockForProcess();
DWORD Test_APO_ValidateBasicInterface();
DWORD Test_APO_LockForProcessValidateBufferSize();

//
// Device graph functional test cases
//
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual510();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual55();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1020();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1020After();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1020Before();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1010();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1010After();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1010Before();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual2020();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual2040();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual3030();
DWORD Test_DeviceGraph_SimpleGraphPlay_ManualMix();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual2010();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual2010After();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1020_Budget0();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1020After_Budget0();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1020Before_Budget0();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual11Before();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual22Before();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual33Before();

DWORD Test_DeviceGraph_SimpleGraphPlay_Manual2010Before();
DWORD Test_DeviceGraph_SimpleGraphPlay_CreateWhilePlaying();
DWORD Test_DeviceGraph_SimpleGraphPlay_StopTest();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1020_DeviceFmt();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1020_Generic_MultichannelQuad();
DWORD Test_DeviceGraph_SimpleGraphPlay_SimulateSilence();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual_Generic_SRC();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual_Generic_SRC_Doppler();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual_InsertGFX();
DWORD Test_DeviceGraph_SimpleGraphPlay_DestroyStream();
DWORD Test_DeviceGraph_SimpleGraphPlay_LocalExtensible_24Bit();
DWORD Test_DeviceGraph_SimpleGraphPlay_DeviceExtensible_Bit();
DWORD Test_DeviceGraph_SimpleGraphPlay_EndpointExtensible_24Bit();
DWORD Test_DeviceGraph_SimpleGraphPlay_EndpointExtensible_6Channels();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual_Generic_SRC_Doppler();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1010_DelayStart2s();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual_StartStop();

//
// Format testing
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1010_8BT();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1010_16BT();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1010_20BT();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1010_24BT();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1010_FLBT();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1010_8KSR();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1010_11KSR();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1010_16KSR();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1010_22KSR();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1010_32KSR();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1010_44KSR();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1010_48KSR();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1010_96KSR();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1010_1CHs();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1010_2CHs();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1010_1CH();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1010_2CH();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1010_4CH();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1010_6CH();
DWORD Test_DeviceGraph_SimpleGraphPlay_Manual1010_8CH();

//
// Device graph API testing
DWORD Test_DeviceGraph_Invalid_CaptureGraphs();
DWORD Test_DeviceGraph_Invalid_Graphs();
DWORD Test_DeviceGraph_Initialize();
DWORD Test_DeviceGraph_CreateStream();
DWORD Test_DeviceGraph_Initialize_Gfx();

//
// Capture specific
DWORD Test_DeviceGraph_SimpleGraphCapture_Manual1010();
DWORD Test_DeviceGraph_SimpleGraphCapture_Manual1010_Multiple();
DWORD Test_DeviceGraph_SimpleGraphCapture_Manual1010_Multiple_Reasonable();
DWORD Test_DeviceGraph_GraphDuplex_Manual1010();


//
// Loopback specific
DWORD Test_DeviceGraph_SimpleGraphLoopback1010();
DWORD Test_DeviceGraph_SimpleGraphLoopbackFirst1010();
DWORD Test_DeviceGraph_SimpleGraphLoopback_NoRender();
DWORD Test_DeviceGraph_SimpleGraphLoopback_RelativePosition1010();
DWORD Test_DeviceGraph_SimpleGraphLoopback_RelativePosition33();
DWORD Test_DeviceGraph_SimpleGraphLoopbackFirst1010_DataBig();
DWORD Test_DeviceGraph_SimpleGraphLoopbackFirst1010_DataNan();
DWORD Test_DeviceGraph_SimpleGraphLoopbackFirst1010_AboveOne();


//
// Local graph functional
//
DWORD Test_GraphPathology_SRC();
DWORD Test_GraphPathology_Inplace();


//
// Performance
DWORD Test_DeviceGraph_Latency();
DWORD Test_DeviceGraph_LatencyDuplex();
DWORD Test_DeviceGraph_GraphDuplex_Manual1010_Drift();
DWORD Test_DeviceGraph_SignalBasedRenderLatency();
DWORD Test_DeviceGraph_SignalBasedRenderLatency2();
DWORD Test_DeviceGraph_SignalBasedRenderLatency1();
DWORD Test_DeviceGraph_SignalBasedRenderLatency4();
DWORD Test_DeviceGraph_SignalBasedRenderLatency5();
DWORD Test_DeviceGraph_SignalBasedRenderLatency7();
DWORD Test_DeviceGraph_SignalBasedRenderLatency8();
DWORD Test_DeviceGraph_SignalBasedRenderLatency9();
DWORD Test_DeviceGraph_SignalBasedRenderLatency3();
DWORD Test_DeviceGraph_SignalBasedRenderLatency6();
DWORD Test_DeviceGraph_SWLoopback10();
DWORD Test_DeviceGraph_SWLoopback8();
DWORD Test_DeviceGraph_SWLoopback6();
DWORD Test_DeviceGraph_SWLoopback4();
DWORD Test_DeviceGraph_SWLoopback2();
DWORD Test_DeviceGraph_SWLoopback1();

//
// APO CPU

#define FORMAT_CONVERTER_TEST_CASE_DECL(suffix)	\
	DWORD Test_DeviceGraph_SimpleGraphPlay_CpuApo_##suffix();	\

FORMAT_CONVERTER_TEST_CASE_DECL(i16f32)
FORMAT_CONVERTER_TEST_CASE_DECL(f32i16)
FORMAT_CONVERTER_TEST_CASE_DECL(i32f32)
FORMAT_CONVERTER_TEST_CASE_DECL(f32i32)
FORMAT_CONVERTER_TEST_CASE_DECL(i24f32)
FORMAT_CONVERTER_TEST_CASE_DECL(f32i24)
FORMAT_CONVERTER_TEST_CASE_DECL(f32i8)
FORMAT_CONVERTER_TEST_CASE_DECL(i8f32)
FORMAT_CONVERTER_TEST_CASE_DECL(f32i24w)
FORMAT_CONVERTER_TEST_CASE_DECL(i24wf32)
FORMAT_CONVERTER_TEST_CASE_DECL(f32i20)
FORMAT_CONVERTER_TEST_CASE_DECL(i20f32)

FORMAT_CONVERTER_TEST_CASE_DECL(MATRIX)
FORMAT_CONVERTER_TEST_CASE_DECL(SRC)
FORMAT_CONVERTER_TEST_CASE_DECL(MIXER)
FORMAT_CONVERTER_TEST_CASE_DECL(VOLUME)
FORMAT_CONVERTER_TEST_CASE_DECL(METER)
