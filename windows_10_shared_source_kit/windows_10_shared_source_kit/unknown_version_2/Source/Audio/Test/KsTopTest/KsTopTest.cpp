// -----------------------------------------------------------------------------
// File:    KSTopTest.cpp
//
// Desc:    This file contains the definition of the classes needed to run 
//          shell98 and test cases.
//
//
// Copyright (C) Microsoft Corporation. All rights reserved.
// -----------------------------------------------------------------------------

#include "common.h"
#include "propkey.h"
#include <mmdeviceapi.h>
#include <BasicLogHelper.h>
#include <strsafe.h>


// -----------------------------------------------------------------------------
// Global Variables and Defines
// -----------------------------------------------------------------------------
// App static name
#define SZSHELLAPP          "Ks Topology Filter Test - 1.0"
#define DT_GENERAL          0x1
#define DT_DUMMY            0x2

static  LPSTR           szModuleName = SZSHELLAPP;
CKsDeviceTestModule*    g_pMMDeviceTestModule;

#ifndef BUILD_TAEF
ITestShell*             g_IShell;
#else //#ifndef BUILD_TAEF

#define g_pModule g_pMMDeviceTestModule

CTaefShellStub g_IShellObject;
CTaefShellStub * g_IShell = &g_IShellObject;
CTaefShellStub * g_pShell = g_IShell;
IBasicLog * g_pBasicLog = NULL;
using namespace WEX::Common;
using namespace WEX::TestExecution;
#endif //#ifndef BUILD_TAEF



// -----------------------------------------------------------------------------
// Name:    TC_XXX
// Desc:    Test Cases functions.These functions mostly call test code from the 
//          two main testing classes, CNodeReq and CPropDescReq.
// -----------------------------------------------------------------------------

DWORD WINAPI TC_NodesIOCount()
{
    CNodeReq NodeReq;
    return NodeReq.CheckNodesIOCount(g_pMMDeviceTestModule->GetKsDevice());
}

DWORD WINAPI TC_NodeCheck_AEC()
{
    CNodeReq NodeReq;
    return NodeReq.CheckNodeProperties(g_pMMDeviceTestModule->GetKsDevice(), KSNODETYPE_ACOUSTIC_ECHO_CANCEL);
}

DWORD WINAPI TC_NodeCheck_AGC()
{
    CNodeReq NodeReq;
    return NodeReq.CheckNodeProperties(g_pMMDeviceTestModule->GetKsDevice(), KSNODETYPE_AGC);
}

DWORD WINAPI TC_NodeCheck_Chorus()
{
    CNodeReq NodeReq;
    return NodeReq.CheckNodeProperties(g_pMMDeviceTestModule->GetKsDevice(), KSNODETYPE_CHORUS);
}

DWORD WINAPI TC_NodeCheck_Delay()
{
    CNodeReq NodeReq;
    return NodeReq.CheckNodeProperties(g_pMMDeviceTestModule->GetKsDevice(), KSNODETYPE_DELAY);
}

DWORD WINAPI TC_NodeCheck_Demux()
{
    CNodeReq NodeReq;
    return NodeReq.CheckNodeProperties(g_pMMDeviceTestModule->GetKsDevice(), KSNODETYPE_DEMUX);
}

DWORD WINAPI TC_NodeCheck_Dev_Specific()
{
    CNodeReq NodeReq;
    return NodeReq.CheckNodeProperties(g_pMMDeviceTestModule->GetKsDevice(), KSNODETYPE_DEV_SPECIFIC);
}

DWORD WINAPI TC_NodeCheck_Equalizer()
{
    CNodeReq NodeReq;
    return NodeReq.CheckNodeProperties(g_pMMDeviceTestModule->GetKsDevice(), KSNODETYPE_EQUALIZER);
}

DWORD WINAPI TC_NodeCheck_Loudness()
{
    CNodeReq NodeReq;
    return NodeReq.CheckNodeProperties(g_pMMDeviceTestModule->GetKsDevice(), KSNODETYPE_LOUDNESS);
}

DWORD WINAPI TC_NodeCheck_Mute()
{
    CNodeReq NodeReq;
    return NodeReq.CheckNodeProperties(g_pMMDeviceTestModule->GetKsDevice(), KSNODETYPE_MUTE);
}

DWORD WINAPI TC_NodeCheck_Mux()
{
    CNodeReq NodeReq;
    return NodeReq.CheckNodeProperties(g_pMMDeviceTestModule->GetKsDevice(), KSNODETYPE_MUX);
}

DWORD WINAPI TC_NodeCheck_Noise_Suppress()
{
    CNodeReq NodeReq;
    return NodeReq.CheckNodeProperties(g_pMMDeviceTestModule->GetKsDevice(), KSNODETYPE_NOISE_SUPPRESS);
}

DWORD WINAPI TC_NodeCheck_Peakmeter()
{
    CNodeReq NodeReq;
    return NodeReq.CheckNodeProperties(g_pMMDeviceTestModule->GetKsDevice(), KSNODETYPE_PEAKMETER);
}

DWORD WINAPI TC_NodeCheck_Prologic_Decoder()
{
    CNodeReq NodeReq;
    return NodeReq.CheckNodeProperties(g_pMMDeviceTestModule->GetKsDevice(), KSNODETYPE_PROLOGIC_DECODER);
}

DWORD WINAPI TC_NodeCheck_Reverb()
{
    CNodeReq NodeReq;
    return NodeReq.CheckNodeProperties(g_pMMDeviceTestModule->GetKsDevice(), KSNODETYPE_REVERB);
}

DWORD WINAPI TC_NodeCheck_Stereo_Wide()
{
    CNodeReq NodeReq;
    return NodeReq.CheckNodeProperties(g_pMMDeviceTestModule->GetKsDevice(), KSNODETYPE_STEREO_WIDE);
}

DWORD WINAPI TC_NodeCheck_Supermix()
{
    CNodeReq NodeReq;
    return NodeReq.CheckNodeProperties(g_pMMDeviceTestModule->GetKsDevice(), KSNODETYPE_SUPERMIX);
}

DWORD WINAPI TC_NodeCheck_Synthesizer()
{
    CNodeReq NodeReq;
    return NodeReq.CheckNodeProperties(g_pMMDeviceTestModule->GetKsDevice(), KSNODETYPE_SYNTHESIZER);
}

DWORD WINAPI TC_NodeCheck_Tone()
{
    CNodeReq NodeReq;
    return NodeReq.CheckNodeProperties(g_pMMDeviceTestModule->GetKsDevice(), KSNODETYPE_TONE);
}

DWORD WINAPI TC_NodeCheck_Volume()
{
    CNodeReq NodeReq;
    return NodeReq.CheckNodeProperties(g_pMMDeviceTestModule->GetKsDevice(), KSNODETYPE_VOLUME);
}

#if (NTDDI_VERSION >= NTDDI_WIN8)
DWORD WINAPI TC_NodeCheck_AudioEngine()
{
    CNodeReq NodeReq;
    return NodeReq.CheckNodeProperties(g_pMMDeviceTestModule->GetKsDevice(), KSNODETYPE_AUDIO_ENGINE);
}
#endif //#if (NTDDI_VERSION >= NTDDI_WIN8)

DWORD WINAPI TC_CheckOrphanedNodes()
{
    CNodeReq NodeReq;
    return NodeReq.CheckOrphanedNodes(g_pMMDeviceTestModule->GetKsDevice());
}

DWORD WINAPI TC_PropertyCheck_AGC()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_AGC);
}

DWORD WINAPI TC_PropertyCheck_Bass()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_BASS);
}

DWORD WINAPI TC_PropertyCheck_Bass_Boost()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_BASS_BOOST);
}

DWORD WINAPI TC_PropertyCheck_Chorus_Level()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_CHORUS_LEVEL);
}

DWORD WINAPI TC_PropertyCheck_CPU_Resources()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_CPU_RESOURCES);
}

DWORD WINAPI TC_PropertyCheck_Delay()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_DELAY);
}

DWORD WINAPI TC_PropertyCheck_Demux_Dest()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_DEMUX_DEST);
}

DWORD WINAPI TC_PropertyCheck_Dev_Specific()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_DEV_SPECIFIC);
}

DWORD WINAPI TC_PropertyCheck_Dynamic_Range()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_DYNAMIC_RANGE);
}

DWORD WINAPI TC_PropertyCheck_Dynamic_Sampling_Rate()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_DYNAMIC_SAMPLING_RATE);
}

DWORD WINAPI TC_PropertyCheck_Eq_Bands()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_EQ_BANDS);
}

DWORD WINAPI TC_PropertyCheck_Eq_Level()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_EQ_LEVEL);
}

DWORD WINAPI TC_PropertyCheck_Loudness()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_LOUDNESS);
}

DWORD WINAPI TC_PropertyCheck_Mid()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_MID);
}

DWORD WINAPI TC_PropertyCheck_Mix_Level_Caps()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_MIX_LEVEL_CAPS);
}

DWORD WINAPI TC_PropertyCheck_Mix_Level_Table()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_MIX_LEVEL_TABLE);
}

DWORD WINAPI TC_PropertyCheck_Mute()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_MUTE);
}

DWORD WINAPI TC_PropertyCheck_Mux_Source()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_MUX_SOURCE);
}

DWORD WINAPI TC_PropertyCheck_Num_Eq_Bands()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_NUM_EQ_BANDS);
}

DWORD WINAPI TC_PropertyCheck_Peakmeter()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_PEAKMETER);
}

#if (NTDDI_VERSION >= NTDDI_WIN8)
DWORD WINAPI TC_PropertyCheck_Peakmeter2()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_PEAKMETER2);
}
#endif //#if (NTDDI_VERSION >= NTDDI_WIN8)

DWORD WINAPI TC_PropertyCheck_Quality()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_QUALITY);
}

DWORD WINAPI TC_PropertyCheck_Reverb_Level()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_REVERB_LEVEL);
}

DWORD WINAPI TC_PropertyCheck_Sampling_Rate()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_SAMPLING_RATE);
}

DWORD WINAPI TC_PropertyCheck_Stereo_Speaker_Geometry()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_STEREO_SPEAKER_GEOMETRY);
}

DWORD WINAPI TC_PropertyCheck_Treble()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_TREBLE);
}

DWORD WINAPI TC_PropertyCheck_VolumeLevel()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_VOLUMELEVEL);
}

DWORD WINAPI TC_PropertyCheck_Wideness()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_WIDENESS);
}

#if (NTDDI_VERSION >= NTDDI_WIN8)
DWORD WINAPI TC_PropertyCheck_LfxEnable()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_AudioEngine, KSPROPERTY_AUDIOENGINE_LFXENABLE);
}

DWORD WINAPI TC_PropertyCheck_GfxEnable()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_AudioEngine, KSPROPERTY_AUDIOENGINE_GFXENABLE);
}

DWORD WINAPI TC_PropertyCheck_MixFormat()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_AudioEngine, KSPROPERTY_AUDIOENGINE_MIXFORMAT);
}

DWORD WINAPI TC_PropertyCheck_DeviceFormat()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_AudioEngine, KSPROPERTY_AUDIOENGINE_DEVICEFORMAT);
}

DWORD WINAPI TC_PropertyCheck_SupportedDeviceFormats()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_AudioEngine, KSPROPERTY_AUDIOENGINE_SUPPORTEDDEVICEFORMATS);
}

DWORD WINAPI TC_PropertyCheck_AEDescriptor()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_AudioEngine, KSPROPERTY_AUDIOENGINE_DESCRIPTOR);
}

DWORD WINAPI TC_PropertyCheck_WaveRTCurrentWritePosition()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_WAVERT_CURRENT_WRITE_POSITION);
}

#if (NTDDI_VERSION >= NTDDI_WINBLUE)

DWORD WINAPI TC_PropertyCheck_WaveRTCurrentWriteLastBufferPosition()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_WAVERT_CURRENT_WRITE_LASTBUFFER_POSITION);
}

#endif //#if (NTDDI_VERSION >= NTDDI_WINBLUE)

DWORD WINAPI TC_PropertyCheck_BufferSizeRange()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_AudioEngine, KSPROPERTY_AUDIOENGINE_BUFFER_SIZE_RANGE);
}

DWORD WINAPI TC_PropertyCheck_LoopbackProtection()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_AudioEngine, KSPROPERTY_AUDIOENGINE_LOOPBACK_PROTECTION);
}

DWORD WINAPI TC_PropertyCheck_PresentationPosition()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_PRESENTATION_POSITION);
}

DWORD WINAPI TC_PropertyCheck_LinearBufferPosition()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_Audio, KSPROPERTY_AUDIO_LINEAR_BUFFER_POSITION);
}

DWORD WINAPI TC_PropertyCheck_EngVolumeLevel()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropInValidNodes(g_pMMDeviceTestModule->GetKsDevice(), &KSPROPSETID_AudioEngine, KSPROPERTY_AUDIOENGINE_VOLUMELEVEL);
}

#endif //#if (NTDDI_VERSION >= NTDDI_WIN8)

DWORD WINAPI TC_CheckPropertyAccessFlags()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropAccessFlags(g_pMMDeviceTestModule->GetKsDevice());
}

DWORD WINAPI TC_CheckPropertyDescriptorSize()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropDescHeaders(g_pMMDeviceTestModule->GetKsDevice());
}

DWORD WINAPI TC_CheckPropertyGetSetRanges()
{
    CPropDescReq PropDescReq;
    return PropDescReq.CheckPropGetSetRanges(g_pMMDeviceTestModule->GetKsDevice());
}

DWORD WINAPI TC_CheckPinKsDataRange()
{
    CPinReq PinReq;
    return PinReq.CheckPinKsDataRange(g_pMMDeviceTestModule->GetKsDevice());
}

DWORD WINAPI TC_CheckPinKsDataFormat()
{
    CPinReq PinReq;
    return PinReq.CheckPinKsDataFormat(g_pMMDeviceTestModule->GetKsDevice());
}

DWORD WINAPI TC_CheckPinDataFlow()
{
    CPinReq PinReq;
    return PinReq.CheckPinDataFlow(g_pMMDeviceTestModule->GetKsDevice());
}

DWORD WINAPI TC_CheckOrphanedPins()
{
    CPinReq PinReq;
    return PinReq.CheckOrphanedPins(g_pMMDeviceTestModule->GetKsDevice());
}

DWORD WINAPI TC_CheckJackDesc()
{
    CPinReq PinReq;
    return PinReq.CheckJackDesc(g_pMMDeviceTestModule->GetKsDevice());
}

DWORD WINAPI TC_CheckJackDesc2()
{
    CPinReq PinReq;
    return PinReq.CheckJackDesc2(g_pMMDeviceTestModule->GetKsDevice());
}

DWORD WINAPI TC_CheckJackSinkInfo()
{
    CPinReq PinReq;
    return PinReq.CheckJackSinkInfo(g_pMMDeviceTestModule->GetKsDevice());
}

DWORD WINAPI TC_CheckPinInstanceCount()
{
    CPinReq PinReq;
    return PinReq.CheckPinInstanceCount(g_pMMDeviceTestModule->GetKsDevice());
}

DWORD WINAPI TC_CheckMicArrayGeometry()
{
    CPinReq PinReq;
    return PinReq.CheckMicArrayGeometry(g_pMMDeviceTestModule->GetKsDevice());
}

DWORD WINAPI TC_CheckProcessingModes()
{
    CPinReq PinReq;
    return PinReq.CheckProcessingModes(g_pMMDeviceTestModule->GetKsDevice());
}

DWORD WINAPI TC_CheckProposeDataformat2()
{
    CPinReq PinReq;
    return PinReq.CheckProposeDataformat2(g_pMMDeviceTestModule->GetKsDevice());
}

DWORD WINAPI TC_CheckPhysicalConnections()
{
    CFilterReq FilterReq;
    return FilterReq.CheckPhysicalConnections(g_pMMDeviceTestModule->GetKsDevice());
}

DWORD WINAPI TC_CheckInternalConnections()
{
    CFilterReq FilterReq;
    return FilterReq.CheckInternalConnections(g_pMMDeviceTestModule->GetKsDevice());
}

DWORD WINAPI TC_CheckBridgeCommPath()
{
    CKsGraph Graph;
    Graph.BuildFromDevices(g_pMMDeviceTestModule->GetEnumDevice(), g_pMMDeviceTestModule->GetDevice());
    return Graph.CheckBridgeCommPath();
}

DWORD WINAPI TC_CheckKsComponentId()
{
    CFilterReq FilterReq;
    return FilterReq.CheckKsComponentId(g_pMMDeviceTestModule->GetKsDevice());
}

DWORD WINAPI TC_CheckMicrophoneSensitivity()
{
    CPinReq PinReq;
    return PinReq.CheckMicrophoneSensitivity(g_pMMDeviceTestModule->GetKsDevice());
}

DWORD WINAPI TC_CheckMicrophoneSNR()
{
    CPinReq PinReq;
    return PinReq.CheckMicrophoneSNR(g_pMMDeviceTestModule->GetKsDevice());
}

// -----------------------------------------------------------------------------
// Shell98 test definition
//typedef struct dv_dvinfo_tag
//{
//    LPSTR       pszCaseID;      // shell    - test case ID
//    LPSTR       pszName;        // shell    - test case name
//    LPARAM      lpCaseFlags;    // shell    - test case flags
//    LPARAM      lpDeviceFlags;  // shell    - device type flags
//    LPARAM      lpSTFlags;      // selftest - server/cable/input flags
//    LPARAM      lpUser;         // User-defined field
//    TESTFNPROC  pfnTest;        // shell    - Pointer to the test case function
//    LPSTR       pszGuid;        // shell    - test case GUID identifier
//} DV_TCINFO, *PDV_TCINFO;
//
//
// Unit/ Invalid Test Cases for DeviceTopology set of API's. Each test case maps to a method in DeviceTopology. 
// However one method may have one or more test cases associated with it depending on various scenarious that
// we are testing the method for.
#ifndef BUILD_TAEF
DV_TCINFO  rgTestCaseInfo[]= 
{
    { "",       "KS Topology Filters",                      0,  DT_GENERAL, 0, 0,  NULL, "{3B62C5A4-BBD1-4767-A0DF-A3F2D5A4A778}"},
    { "",       " KS Nodes",                                0,  DT_GENERAL, 0, 0,  NULL, "{B5B2BB05-1D37-477B-8C7B-1616F6B68982}"},
    { "1.1",    "  All Nodes Pin I/O Count",                0,  DT_GENERAL, 0, 0,  TC_NodesIOCount, "{06014298-6174-428C-9A3E-5CD61D45C4E0}"},
    { "1.2",    "  Acoustic Echo Cancel Property Check",    0,  DT_GENERAL, 0, 0,  TC_NodeCheck_AEC, "{FC6D2415-B375-48CC-B109-7A509146DD69}"},
    { "1.3",    "  Automatic Gain Control Property Check",  0,  DT_GENERAL, 0, 0,  TC_NodeCheck_AGC, "{1550213E-9455-404A-BCD5-99F899D80582}"},    
    { "1.4",    "  Delay Property Check",                   0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Delay, "{C0A55BDE-80B1-4DF8-B045-0D8887C1852A}"},
    { "1.5",    "  Demux Property Check",                   0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Demux, "{408F0334-3694-4E80-879F-85639AA03E7E}"},
    { "1.6",    "  Device Specific Property Check",         0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Dev_Specific, "{57A75B75-B47E-40B0-A354-CD08798D5713}"},
    { "1.7",    "  Equalizer Property Check",               0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Equalizer, "{FA82AD9D-33AB-4871-9D17-138FB34626C9}"},
    { "1.8",    "  Loudness Property Check",                0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Loudness, "{75402C1B-C96F-454B-87F7-823862233FCD}"},
    { "1.9",    "  Mute Property Check",                    0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Mute, "{F13D678A-3DDA-4EF3-8237-5B8506B0F94C}"},
    { "1.10",   "  Mux Property Check",                     0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Mux, "{56D0537D-DE5E-493E-AB4A-87933C20A9C6}"},
    { "1.11",   "  Noise Suppress Property Check",          0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Noise_Suppress, "{DEB801FA-9B33-48EA-BD35-F597711FB5F7}"},
    { "1.12",   "  Peakmeter Property Check",               0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Peakmeter, "{591290FE-364B-4DB0-82BF-5947CF432F41}"},
    { "1.13",   "  Prologic Decoder Property Check",        0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Prologic_Decoder, "{E374D8CB-BF37-4434-B667-27FA57C002E5}"},
    { "1.14",   "  Reverb Property Check",                  0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Reverb, "{DC03E1A4-B57D-44B4-B598-6F2DC523AE21}"},
    { "1.15",   "  Stereo Wide Property Check",             0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Stereo_Wide, "{E26FA547-727D-488F-9D1D-625DDDE15AF8}"},
    { "1.16",   "  Supermix Property Check",                0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Supermix, "{41B7E748-0239-4370-8390-CCAA60B83BBD}"},
    { "1.17",   "  Synthesizer Property Check",             0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Synthesizer, "{65CB6E67-54B3-4A19-B7B4-8AC7F0D6EE6C}"},
    { "1.18",   "  Tone Property Check",                    0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Tone, "{325DCF3A-BD9F-4678-9C2C-104C678FD234}"},
    { "1.19",   "  Volume Property Check",                  0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Volume, "{D5DD8CD6-14D2-46D7-B365-7D2326F1A4FA}"},
#if (NTDDI_VERSION >= NTDDI_WIN8)
    { "1.20",   "  Audio Engine Property Check",            0,  DT_GENERAL, 0, 0,  TC_NodeCheck_AudioEngine, "{A8393016-7F20-4B9C-B671-7326E7345C50}"},
#endif //#if (NTDDI_VERSION >= NTDDI_WIN8)
    { "1.21",   "  Check Orphaned Nodes",                   0,  DT_GENERAL, 0, 0,  TC_CheckOrphanedNodes, "{7E4E24F7-F04E-4231-ADD8-672E88C74207}"},
    { "",       " KS Properties",                           0,  DT_GENERAL, 0, 0,  NULL, "{BBB5867C-CFDF-40E7-ACDA-DF08F716EC51}"},
    { "2.1",    "  Check All Properties Access Flags",      0,  DT_GENERAL, 0, 0,  TC_CheckPropertyAccessFlags, "{7D3463C4-D05A-4B66-BC82-B089CCF8D621}"},
    { "2.2",    "  Check Property: Audio Gain Control",     0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_AGC, "{AF04F7CE-EBBB-4D72-9CD2-ACBAE84362FE}"},
    { "2.3",    "  Check Property: Bass",                   0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Bass, "{005546A9-E4E4-4ED4-A861-58BDF41AD520}"},
    { "2.4",    "  Check Property: Bass Boost",             0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Bass_Boost, "{ACB9E2B8-3F34-4E77-88C7-342A7FD81FB4}"},
    { "2.5",    "  Check Property: Chorus Level",           0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Chorus_Level, "{62F30FCB-C900-4A9B-8E12-55052AA02F5C}"},
    { "2.6",    "  Check Property: CPU Resources",          0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_CPU_Resources, "{12AA23A8-1E96-4D9F-9194-F285DE24D104}"},
    { "2.7",    "  Check Property: Delay",                  0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Delay, "{D941D863-DB60-4246-870F-169957F9E5BA}"},
    { "2.8",    "  Check Property: Demux Destination",      0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Demux_Dest, "{3D04D73D-8964-4F26-A541-65E41EEF7317}"},
    { "2.9",    "  Check Property: Device Specific",        0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Dev_Specific, "{6D61D0B0-B8E4-42F1-9773-B0A5BB4398B5}"},
    { "2.10",   "  Check Property: Dynamic Range",          0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Dynamic_Range, "{CD247AA7-504A-4572-86B4-023F8576D702}"},
    { "2.11",   "  Check Property: Dynamic Sampling Rate",  0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Dynamic_Sampling_Rate, "{B09695E4-8925-47E5-B53F-BD39B709C68A}"},
    { "2.12",   "  Check Property: Equalizer Bands",        0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Eq_Bands, "{1FD30170-561A-4F80-8697-CED7A6469BF8}"},
    { "2.13",   "  Check Property: Equalizer Level",        0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Eq_Level, "{BBD09CF4-D225-4826-B9FA-C2EE1AFFA897}"},
    { "2.14",   "  Check Property: Loudness",               0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Loudness, "{082F9CC1-6A17-487C-A20A-02B17134EADB}"},
    { "2.15",   "  Check Property: Mid",                    0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Mid, "{EB8E9F0F-4FC8-4423-B8F8-0EACDAED138E}"},
    { "2.16",   "  Check Property: Mix Level Caps",         0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Mix_Level_Caps, "{7C6CD864-F495-460C-99DE-BF20DFB20426}"},
    { "2.17",   "  Check Property: Mix Level Table",        0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Mix_Level_Table, "{DD0D40B0-ECAD-432C-ACCE-2A376AE886C0}"},
    { "2.18",   "  Check Property: Mute",                   0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Mute, "{8E3F308B-E7D1-493E-B257-AE9ED97BAE41}"},
    { "2.19",   "  Check Property: Mux Source",             0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Mux_Source, "{A10EAD63-12C6-46D7-9C37-1A51610EB880}"},
    { "2.20",   "  Check Property: Number of Equalizer Bands",0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Num_Eq_Bands, "{648075F0-8EB3-457D-A68A-24644336A0A7}"},
    { "2.21",   "  Check Property: Peakmeter",              0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Peakmeter, "{46BF03B2-4D93-43CE-B94D-C5FEF7D7025D}"},
#if (NTDDI_VERSION >= NTDDI_WIN8)
    { "2.22",   "  Check Property: Peakmeter2",             0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Peakmeter2, "{768BAF7A-24BB-4E4F-95B9-28670C3383C5}"},
#endif //#if (NTDDI_VERSION >= NTDDI_WIN8)
    { "2.23",   "  Check Property: Quality",                0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Quality, "{55F1F867-11D6-4283-A8C2-B23D8EAC10D7}"},
    { "2.24",   "  Check Property: Reverb Level",           0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Reverb_Level, "{7E6F852A-D4E0-4CC7-9032-A8E03824063A}"},
    { "2.25",   "  Check Property: Sampling Rate",          0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Sampling_Rate, "{55CA73D2-AC8F-4D0B-9169-1FFE8111C952}"},
    { "2.26",   "  Check Property: Stereo Speaker Geometry",0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Stereo_Speaker_Geometry, "{2E22FD22-43C4-4A1E-AEB1-F5F0C13D89A1}"},
    { "2.27",   "  Check Property: Treble",                 0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Treble, "{9A028CF7-AF95-478E-8530-1F8DAA0C6B5D}"},
    { "2.28",   "  Check Property: Volume Level",           0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_VolumeLevel, "{8DEE327D-B445-4E4D-80B8-98BE6377B0F6}"},
    { "2.29",   "  Check Property: Wideness",               0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Wideness, "{614DED21-DE8D-4E02-BAA5-F95AE3AFBE67}"},
#if (NTDDI_VERSION >= NTDDI_WIN8)
    { "2.30",   "  Check Property: WaveRT Current Write Position", 0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_WaveRTCurrentWritePosition, "{AA09315F-FA14-493D-9F8C-33779E4D5FC9}"},
    { "2.31",   "  Check Property: Presentation Position",  0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_PresentationPosition, "{813BC979-9B0B-4CB0-ACC0-9031B78439E3}"},
    { "2.32",   "  Check Property: Linear Buffer Position", 0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_LinearBufferPosition, "{997F29E0-0922-401B-99C7-21402DE82EC2}"},
    { "2.33",   "  Check Property: AE LFX Enable",          0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_LfxEnable, "{8bff720c-9194-4775-8a4d-ba8b24f64757}"},
    { "2.34",   "  Check Property: AE GFX Enable",          0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_GfxEnable, "{0be50f96-6824-446d-9643-9b502df15388}"},
    { "2.35",   "  Check Property: AE Mix Format",          0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_MixFormat, "{f9840f11-870d-4e93-8a2e-99857b58c949}"},
    { "2.37",   "  Check Property: AE Device Format",       0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_DeviceFormat, "{5f96379c-db40-41cd-8cc7-3fb10fa770c1}"},
    { "2.38",   "  Check Property: AE Supported Device Formats", 0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_SupportedDeviceFormats, "{bf472abb-5a2b-463e-8d08-4c39d90caf2e}"},
    { "2.39",   "  Check Property: AE Descriptor",          0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_AEDescriptor, "{dae1a2ee-ca1d-4ffb-9b54-7ddf061f316b}"},
    { "2.40",   "  Check Property: AE Buffer Size Range",   0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_BufferSizeRange, "{F07C00A8-AC43-4473-95B8-486DD72685C4}"},
    { "2.41",   "  Check Property: AE Loopback Protection", 0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_LoopbackProtection, "{2FAC8033-5FA9-41C2-B75D-106A86F5256F}"},
    { "2.42",   "  Check Property: AE Volume Level",        0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_EngVolumeLevel, "{15B906A0-5FA9-41C2-B75D-102FAC80336F}"},
#endif //#if (NTDDI_VERSION >= NTDDI_WIN8)
    { "2.43",   "  Check Property Descriptor Size and Headers",0,  DT_GENERAL, 0, 0,  TC_CheckPropertyDescriptorSize, "{37D7F5B6-7062-4E6A-810E-D3E2947E1783}"},
    { "2.44",   "  Check Property Get and Set Ranges",      0,  DT_GENERAL, 0, 0,  TC_CheckPropertyGetSetRanges, "{D33DA24A-6730-4419-AE7E-F628106EBDB9}"},
#if (NTDDI_VERSION >= NTDDI_WINBLUE)
    { "2.45",   "  Check Property: WaveRT Current Write Last Buffer Position", 0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_WaveRTCurrentWriteLastBufferPosition, "{EBEEDC7B-63DC-41b4-B873-0144D94E4ADD}"},
#endif //#if (NTDDI_VERSION >= NTDDI_WINBLUE)
    { "",       " KS Pins",                                 0,  DT_GENERAL, 0, 0,  NULL, "{443D1244-8135-4270-BB7F-69FF1F5E2E58}"},
    { "3.1",    "  Check Pin KsDataRange",                  0,  DT_GENERAL, 0, 0,  TC_CheckPinKsDataRange, "{64088E95-4B4F-41D0-A1A3-9408111D3A7A}"},
    { "3.2",    "  Check Pin KsDataFormat",                 0,  DT_GENERAL, 0, 0,  TC_CheckPinKsDataFormat, "{1EA593CF-7B31-4277-B4C1-983944FB776E}"},
    { "3.3",    "  Check Pin DataFlow",                     0,  DT_GENERAL, 0, 0,  TC_CheckPinDataFlow, "{5D36DF52-6D09-495F-8738-9805B906A0ED}"},
    { "3.4",    "  Check Orphaned Pins",                    0,  DT_GENERAL, 0, 0,  TC_CheckOrphanedPins, "{F12BD817-1E2B-4AF1-9FE7-9FDF3D5B3CB9}"},
    { "3.5",    "  Check KSPROPERTY_JACK_DESCRIPTION",      0,  DT_GENERAL, 0, 0,  TC_CheckJackDesc,  "{FED74C35-8B68-4D5F-8976-1C45435EBEAA}"},
    { "3.6",    "  Check KSPROPERTY_JACK_DESCRIPTION2",     0,  DT_GENERAL, 0, 0,  TC_CheckJackDesc2, "{51DED38D-C10A-4074-A419-194A043DE1E8}"},
    { "3.7",    "  Check KSPROPERTY_JACK_SINK_INFO",        0,  DT_GENERAL, 0, 0,  TC_CheckJackSinkInfo, "{1F23BCA4-7A3A-4CE1-A2CD-1AC2C4210961}"},
    { "3.8",    "  Check KSPROPERTY_AUDIO_MIC_ARRAY_GEOMETRY",0,DT_GENERAL, 0, 0,  TC_CheckMicArrayGeometry, "{34EB6FB1-180D-47A0-A5AB-ABD24CBE2131}"},
    { "3.9",    "  Check Pin Instance Count",               0,  DT_GENERAL, 0, 0,  TC_CheckPinInstanceCount, "{86B8A7A5-4642-4320-A8BD-F8BEB68504C2}"},
#if (NTDDI_VERSION >= NTDDI_WINBLUE)
    { "3.10",    "  Check Processing Modes",                 0,  DT_GENERAL, 0, 0,  TC_CheckProcessingModes, "{36147B60-A72F-46F0-A432-BFB6BB657099}"},
    { "3.11",    "  Check Propose Dataformat 2",            0,  DT_GENERAL, 0, 0,  TC_CheckProposeDataformat2, "{4417BB4C-2C25-4963-866B-E30D59F3646E}"},
#endif //#if (NTDDI_VERSION >= NTDDI_WINBLUE)
    { "3.12",   "  Check KSPROPERTY_AUDIO_MIC_SENSITIVITY", 0,  DT_GENERAL, 0, 0,  TC_CheckMicrophoneSensitivity, "{8DA77419-9D82-4125-A9FE-2ED46739F3FC}"},
    { "3.13",   "  Check KSPROPERTY_AUDIO_MIC_SNR",         0,  DT_GENERAL, 0, 0,  TC_CheckMicrophoneSNR, "{34FF8AF6-0545-4CD7-ACAE-2C7F6A40C419}"},
    { "",       " KS Filters",                              0,  DT_GENERAL, 0, 0,  NULL, "{52D4EF3F-9659-48C4-8816-2ABFF77662C7}"},
    { "4.1",    "  Check Physical Connections",             0,  DT_GENERAL, 0, 0,  TC_CheckPhysicalConnections, "{6E2CE4E4-F76B-4554-BC44-537397593E49}"},
    { "4.2",    "  Check Internal Connections",             0,  DT_GENERAL, 0, 0,  TC_CheckInternalConnections, "{053D5439-CEE0-41E4-BD89-1001984DC0D4}"},
    { "4.3",    "  Check Bridge/Communication Pins Paths",  TESTFLAG_DISABLED,  DT_GENERAL, 0, 0,  TC_CheckBridgeCommPath, "{9A5A7562-7FF3-4D86-9636-0FE087F2CDEC}"},
    { "4.4",    "  Check KsComponentId",                    0,  DT_GENERAL, 0, 0,  TC_CheckKsComponentId, "{5DF25981-4B31-4659-B9EA-44149631457B}"},
};
#else // #ifndef BUILD_TAEF
BEGIN_TEST_CASE_INFO(KsTopTest, CKsDeviceTestModule)
    DEVICE_TEST_PARENT( "",       "KS Topology Filters",                      0,  DT_GENERAL, 0, 0,  NULL, "{3B62C5A4-BBD1-4767-A0DF-A3F2D5A4A778}")
    DEVICE_TEST_PARENT( "",       " KS Nodes",                                0,  DT_GENERAL, 0, 0,  NULL, "{B5B2BB05-1D37-477B-8C7B-1616F6B68982}")
    DEVICE_TEST_METHOD( "1.1",    "  All Nodes Pin I/O Count",                0,  DT_GENERAL, 0, 0,  TC_NodesIOCount, "{06014298-6174-428C-9A3E-5CD61D45C4E0}")
    DEVICE_TEST_METHOD( "1.2",    "  Acoustic Echo Cancel Property Check",    0,  DT_GENERAL, 0, 0,  TC_NodeCheck_AEC, "{FC6D2415-B375-48CC-B109-7A509146DD69}")
    DEVICE_TEST_METHOD( "1.3",    "  Automatic Gain Control Property Check",  0,  DT_GENERAL, 0, 0,  TC_NodeCheck_AGC, "{1550213E-9455-404A-BCD5-99F899D80582}")    
    DEVICE_TEST_METHOD( "1.4",    "  Delay Property Check",                   0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Delay, "{C0A55BDE-80B1-4DF8-B045-0D8887C1852A}")
    DEVICE_TEST_METHOD( "1.5",    "  Demux Property Check",                   0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Demux, "{408F0334-3694-4E80-879F-85639AA03E7E}")
    DEVICE_TEST_METHOD( "1.6",    "  Device Specific Property Check",         0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Dev_Specific, "{57A75B75-B47E-40B0-A354-CD08798D5713}")
    DEVICE_TEST_METHOD( "1.7",    "  Equalizer Property Check",               0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Equalizer, "{FA82AD9D-33AB-4871-9D17-138FB34626C9}")
    DEVICE_TEST_METHOD( "1.8",    "  Loudness Property Check",                0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Loudness, "{75402C1B-C96F-454B-87F7-823862233FCD}")
    DEVICE_TEST_METHOD( "1.9",    "  Mute Property Check",                    0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Mute, "{F13D678A-3DDA-4EF3-8237-5B8506B0F94C}")
    DEVICE_TEST_METHOD( "1.10",   "  Mux Property Check",                     0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Mux, "{56D0537D-DE5E-493E-AB4A-87933C20A9C6}")
    DEVICE_TEST_METHOD( "1.11",   "  Noise Suppress Property Check",          0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Noise_Suppress, "{DEB801FA-9B33-48EA-BD35-F597711FB5F7}")
    DEVICE_TEST_METHOD( "1.12",   "  Peakmeter Property Check",               0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Peakmeter, "{591290FE-364B-4DB0-82BF-5947CF432F41}")
    DEVICE_TEST_METHOD( "1.13",   "  Prologic Decoder Property Check",        0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Prologic_Decoder, "{E374D8CB-BF37-4434-B667-27FA57C002E5}")
    DEVICE_TEST_METHOD( "1.14",   "  Reverb Property Check",                  0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Reverb, "{DC03E1A4-B57D-44B4-B598-6F2DC523AE21}")
    DEVICE_TEST_METHOD( "1.15",   "  Stereo Wide Property Check",             0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Stereo_Wide, "{E26FA547-727D-488F-9D1D-625DDDE15AF8}")
    DEVICE_TEST_METHOD( "1.16",   "  Supermix Property Check",                0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Supermix, "{41B7E748-0239-4370-8390-CCAA60B83BBD}")
    DEVICE_TEST_METHOD( "1.17",   "  Synthesizer Property Check",             0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Synthesizer, "{65CB6E67-54B3-4A19-B7B4-8AC7F0D6EE6C}")
    DEVICE_TEST_METHOD( "1.18",   "  Tone Property Check",                    0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Tone, "{325DCF3A-BD9F-4678-9C2C-104C678FD234}")
    DEVICE_TEST_METHOD( "1.19",   "  Volume Property Check",                  0,  DT_GENERAL, 0, 0,  TC_NodeCheck_Volume, "{D5DD8CD6-14D2-46D7-B365-7D2326F1A4FA}")
    DEVICE_TEST_METHOD( "1.20",   "  Audio Engine Property Check",            0,  DT_GENERAL, 0, 0,  TC_NodeCheck_AudioEngine, "{A8393016-7F20-4B9C-B671-7326E7345C50}")
    DEVICE_TEST_METHOD( "1.21",   "  Check Orphaned Nodes",                   0,  DT_GENERAL, 0, 0,  TC_CheckOrphanedNodes, "{7E4E24F7-F04E-4231-ADD8-672E88C74207}")
    DEVICE_TEST_PARENT( "",       " KS Properties",                           0,  DT_GENERAL, 0, 0,  NULL, "{BBB5867C-CFDF-40E7-ACDA-DF08F716EC51}")
    DEVICE_TEST_METHOD( "2.1",    "  Check All Properties Access Flags",      0,  DT_GENERAL, 0, 0,  TC_CheckPropertyAccessFlags, "{7D3463C4-D05A-4B66-BC82-B089CCF8D621}")
    DEVICE_TEST_METHOD( "2.2",    "  Check Property: Audio Gain Control",     0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_AGC, "{AF04F7CE-EBBB-4D72-9CD2-ACBAE84362FE}")
    DEVICE_TEST_METHOD( "2.3",    "  Check Property: Bass",                   0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Bass, "{005546A9-E4E4-4ED4-A861-58BDF41AD520}")
    DEVICE_TEST_METHOD( "2.4",    "  Check Property: Bass Boost",             0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Bass_Boost, "{ACB9E2B8-3F34-4E77-88C7-342A7FD81FB4}")
    DEVICE_TEST_METHOD( "2.5",    "  Check Property: Chorus Level",           0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Chorus_Level, "{62F30FCB-C900-4A9B-8E12-55052AA02F5C}")
    DEVICE_TEST_METHOD( "2.6",    "  Check Property: CPU Resources",          0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_CPU_Resources, "{12AA23A8-1E96-4D9F-9194-F285DE24D104}")
    DEVICE_TEST_METHOD( "2.7",    "  Check Property: Delay",                  0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Delay, "{D941D863-DB60-4246-870F-169957F9E5BA}")
    DEVICE_TEST_METHOD( "2.8",    "  Check Property: Demux Destination",      0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Demux_Dest, "{3D04D73D-8964-4F26-A541-65E41EEF7317}")
    DEVICE_TEST_METHOD( "2.9",    "  Check Property: Device Specific",        0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Dev_Specific, "{6D61D0B0-B8E4-42F1-9773-B0A5BB4398B5}")
    DEVICE_TEST_METHOD( "2.10",   "  Check Property: Dynamic Range",          0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Dynamic_Range, "{CD247AA7-504A-4572-86B4-023F8576D702}")
    DEVICE_TEST_METHOD( "2.11",   "  Check Property: Dynamic Sampling Rate",  0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Dynamic_Sampling_Rate, "{B09695E4-8925-47E5-B53F-BD39B709C68A}")
    DEVICE_TEST_METHOD( "2.12",   "  Check Property: Equalizer Bands",        0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Eq_Bands, "{1FD30170-561A-4F80-8697-CED7A6469BF8}")
    DEVICE_TEST_METHOD( "2.13",   "  Check Property: Equalizer Level",        0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Eq_Level, "{BBD09CF4-D225-4826-B9FA-C2EE1AFFA897}")
    DEVICE_TEST_METHOD( "2.14",   "  Check Property: Loudness",               0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Loudness, "{082F9CC1-6A17-487C-A20A-02B17134EADB}")
    DEVICE_TEST_METHOD( "2.15",   "  Check Property: Mid",                    0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Mid, "{EB8E9F0F-4FC8-4423-B8F8-0EACDAED138E}")
    DEVICE_TEST_METHOD( "2.16",   "  Check Property: Mix Level Caps",         0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Mix_Level_Caps, "{7C6CD864-F495-460C-99DE-BF20DFB20426}")
    DEVICE_TEST_METHOD( "2.17",   "  Check Property: Mix Level Table",        0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Mix_Level_Table, "{DD0D40B0-ECAD-432C-ACCE-2A376AE886C0}")
    DEVICE_TEST_METHOD( "2.18",   "  Check Property: Mute",                   0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Mute, "{8E3F308B-E7D1-493E-B257-AE9ED97BAE41}")
    DEVICE_TEST_METHOD( "2.19",   "  Check Property: Mux Source",             0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Mux_Source, "{A10EAD63-12C6-46D7-9C37-1A51610EB880}")
    DEVICE_TEST_METHOD( "2.20",   "  Check Property: Number of Equalizer Bands",0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Num_Eq_Bands, "{648075F0-8EB3-457D-A68A-24644336A0A7}")
    DEVICE_TEST_METHOD( "2.21",   "  Check Property: Peakmeter",              0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Peakmeter, "{46BF03B2-4D93-43CE-B94D-C5FEF7D7025D}")
    DEVICE_TEST_METHOD( "2.22",   "  Check Property: Peakmeter2",             0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Peakmeter2, "{768BAF7A-24BB-4E4F-95B9-28670C3383C5}")
    DEVICE_TEST_METHOD( "2.23",   "  Check Property: Quality",                0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Quality, "{55F1F867-11D6-4283-A8C2-B23D8EAC10D7}")
    DEVICE_TEST_METHOD( "2.24",   "  Check Property: Reverb Level",           0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Reverb_Level, "{7E6F852A-D4E0-4CC7-9032-A8E03824063A}")
    DEVICE_TEST_METHOD( "2.25",   "  Check Property: Sampling Rate",          0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Sampling_Rate, "{55CA73D2-AC8F-4D0B-9169-1FFE8111C952}")
    DEVICE_TEST_METHOD( "2.26",   "  Check Property: Stereo Speaker Geometry",0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Stereo_Speaker_Geometry, "{2E22FD22-43C4-4A1E-AEB1-F5F0C13D89A1}")
    DEVICE_TEST_METHOD( "2.27",   "  Check Property: Treble",                 0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Treble, "{9A028CF7-AF95-478E-8530-1F8DAA0C6B5D}")
    DEVICE_TEST_METHOD( "2.28",   "  Check Property: Volume Level",           0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_VolumeLevel, "{8DEE327D-B445-4E4D-80B8-98BE6377B0F6}")
    DEVICE_TEST_METHOD( "2.29",   "  Check Property: Wideness",               0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_Wideness, "{614DED21-DE8D-4E02-BAA5-F95AE3AFBE67}")
    DEVICE_TEST_METHOD( "2.30",   "  Check Property: WaveRT Current Write Position", 0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_WaveRTCurrentWritePosition, "{AA09315F-FA14-493D-9F8C-33779E4D5FC9}")
    DEVICE_TEST_METHOD( "2.31",   "  Check Property: Presentation Position",  0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_PresentationPosition, "{813BC979-9B0B-4CB0-ACC0-9031B78439E3}")
    DEVICE_TEST_METHOD( "2.32",   "  Check Property: Linear Buffer Position", 0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_LinearBufferPosition, "{997F29E0-0922-401B-99C7-21402DE82EC2}")
    DEVICE_TEST_METHOD( "2.33",   "  Check Property: AE LFX Enable",          0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_LfxEnable, "{8bff720c-9194-4775-8a4d-ba8b24f64757}")
    DEVICE_TEST_METHOD( "2.34",   "  Check Property: AE GFX Enable",          0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_GfxEnable, "{0be50f96-6824-446d-9643-9b502df15388}")
    DEVICE_TEST_METHOD( "2.35",   "  Check Property: AE Mix Format",          0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_MixFormat, "{f9840f11-870d-4e93-8a2e-99857b58c949}")
    DEVICE_TEST_METHOD( "2.37",   "  Check Property: AE Device Format",       0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_DeviceFormat, "{5f96379c-db40-41cd-8cc7-3fb10fa770c1}")
    DEVICE_TEST_METHOD( "2.38",   "  Check Property: AE Supported Device Formats", 0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_SupportedDeviceFormats, "{bf472abb-5a2b-463e-8d08-4c39d90caf2e}")
    DEVICE_TEST_METHOD( "2.39",   "  Check Property: AE Descriptor",          0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_AEDescriptor, "{dae1a2ee-ca1d-4ffb-9b54-7ddf061f316b}")
    DEVICE_TEST_METHOD( "2.40",   "  Check Property: AE Buffer Size Range",   0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_BufferSizeRange, "{F07C00A8-AC43-4473-95B8-486DD72685C4}")
    DEVICE_TEST_METHOD( "2.41",   "  Check Property: AE Loopback Protection", 0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_LoopbackProtection, "{2FAC8033-5FA9-41C2-B75D-106A86F5256F}")
    DEVICE_TEST_METHOD( "2.42",   "  Check Property Descriptor Size and Headers",0,  DT_GENERAL, 0, 0,  TC_CheckPropertyDescriptorSize, "{37D7F5B6-7062-4E6A-810E-D3E2947E1783}")
    DEVICE_TEST_METHOD( "2.43",   "  Check Property Get and Set Ranges",      0,  DT_GENERAL, 0, 0,  TC_CheckPropertyGetSetRanges, "{D33DA24A-6730-4419-AE7E-F628106EBDB9}")
    DEVICE_TEST_METHOD( "2.45",   "  Check Property: WaveRT Current Write Last Buffer Position", 0,  DT_GENERAL, 0, 0,  TC_PropertyCheck_WaveRTCurrentWriteLastBufferPosition, "{EBEEDC7B-63DC-41b4-B873-0144D94E4ADD}")
    DEVICE_TEST_PARENT( "",       " KS Pins",                                 0,  DT_GENERAL, 0, 0,  NULL, "{443D1244-8135-4270-BB7F-69FF1F5E2E58}")
    DEVICE_TEST_METHOD( "3.1",    "  Check Pin KsDataRange",                  0,  DT_GENERAL, 0, 0,  TC_CheckPinKsDataRange, "{64088E95-4B4F-41D0-A1A3-9408111D3A7A}")
    DEVICE_TEST_METHOD( "3.2",    "  Check Pin KsDataFormat",                 0,  DT_GENERAL, 0, 0,  TC_CheckPinKsDataFormat, "{1EA593CF-7B31-4277-B4C1-983944FB776E}")
    DEVICE_TEST_METHOD( "3.3",    "  Check Pin DataFlow",                     0,  DT_GENERAL, 0, 0,  TC_CheckPinDataFlow, "{5D36DF52-6D09-495F-8738-9805B906A0ED}")
    DEVICE_TEST_METHOD( "3.4",    "  Check Orphaned Pins",                    0,  DT_GENERAL, 0, 0,  TC_CheckOrphanedPins, "{F12BD817-1E2B-4AF1-9FE7-9FDF3D5B3CB9}")
    DEVICE_TEST_METHOD( "3.5",    "  Check KSPROPERTY_JACK_DESCRIPTION",      0,  DT_GENERAL, 0, 0,  TC_CheckJackDesc,  "{FED74C35-8B68-4D5F-8976-1C45435EBEAA}")
    DEVICE_TEST_METHOD( "3.6",    "  Check KSPROPERTY_JACK_DESCRIPTION2",     0,  DT_GENERAL, 0, 0,  TC_CheckJackDesc2, "{51DED38D-C10A-4074-A419-194A043DE1E8}")
    DEVICE_TEST_METHOD( "3.7",    "  Check KSPROPERTY_JACK_SINK_INFO",        0,  DT_GENERAL, 0, 0,  TC_CheckJackSinkInfo, "{1F23BCA4-7A3A-4CE1-A2CD-1AC2C4210961}")
    DEVICE_TEST_METHOD( "3.8",    "  Check KSPROPERTY_AUDIO_MIC_ARRAY_GEOMETRY",0,DT_GENERAL, 0, 0,  TC_CheckMicArrayGeometry, "{34EB6FB1-180D-47A0-A5AB-ABD24CBE2131}")
    DEVICE_TEST_METHOD( "3.9",    "  Check Pin Instance Count",               0,  DT_GENERAL, 0, 0,  TC_CheckPinInstanceCount, "{86B8A7A5-4642-4320-A8BD-F8BEB68504C2}")
    DEVICE_TEST_METHOD( "3.12",   "  Check KSPROPERTY_AUDIO_MIC_SENSITIVITY", 0,  DT_GENERAL, 0, 0,  TC_CheckMicrophoneSensitivity, "{8DA77419-9D82-4125-A9FE-2ED46739F3FC}")
    DEVICE_TEST_METHOD( "3.13",   "  Check KSPROPERTY_AUDIO_MIC_SNR",         0,  DT_GENERAL, 0, 0,  TC_CheckMicrophoneSNR, "{34FF8AF6-0545-4CD7-ACAE-2C7F6A40C419}")
    DEVICE_TEST_METHOD( "4.0",    "  CheckProcessingModes",					 0,  DT_GENERAL,  0, 0,  TC_CheckProcessingModes, "{36147B60-A72F-46F0-A432-BFB6BB657099}")
    DEVICE_TEST_PARENT( "",       " KS Filters",                              0,  DT_GENERAL, 0, 0,  NULL, "{52D4EF3F-9659-48C4-8816-2ABFF77662C7}")
    DEVICE_TEST_METHOD( "4.1",    "  Check Physical Connections",             0,  DT_GENERAL, 0, 0,  TC_CheckPhysicalConnections, "{6E2CE4E4-F76B-4554-BC44-537397593E49}")
    DEVICE_TEST_METHOD( "4.2",    "  Check Internal Connections",             0,  DT_GENERAL, 0, 0,  TC_CheckInternalConnections, "{053D5439-CEE0-41E4-BD89-1001984DC0D4}")
    DEVICE_TEST_METHOD( "4.3",    "  Check Bridge/Communication Pins Paths",  TESTFLAG_DISABLED,  DT_GENERAL, 0, 0,  TC_CheckBridgeCommPath, "{9A5A7562-7FF3-4D86-9636-0FE087F2CDEC}")
    DEVICE_TEST_METHOD( "4.4",    "  Check KsComponentId",                    0,  DT_GENERAL, 0, 0,  TC_CheckKsComponentId, "{5DF25981-4B31-4659-B9EA-44149631457B}")	
END_TEST_CASE_INFO(KsTopTest)
#endif //#ifndef BUILD_TAEF

#ifndef BUILD_TAEF
// -----------------------------------------------------------------------------
// Shell98 extern
CTestModule* WINAPI NewTestModule
(
    CTestShell* pShell,
    HINSTANCE   hInstDLL
)
{
    g_IShell = (ITestShell*)pShell;
    g_pMMDeviceTestModule = new CKsDeviceTestModule(pShell, hInstDLL);
    return (CTestModule*)g_pMMDeviceTestModule;
}
#endif //#ifndef BUILD_TAEF

//--------------------------------------------------------------------------
//  CKsDevice constructor
CKsDevice::CKsDevice
(   
    CDevice    *pDevice
)
{
    m_pDevice = pDevice;

    StringCchCopyA(m_szRep, ARRAYSIZE(m_szRep), pDevice->GetFriendlyName());
    StringCchCopyA(m_szPnPId, ARRAYSIZE(m_szPnPId), pDevice->GetInterfaceId());
    m_lpType = DT_GENERAL;
}


//--------------------------------------------------------------------------
//  CKsDeviceTestModule constructor
#ifndef BUILD_TAEF
CKsDeviceTestModule::CKsDeviceTestModule(CTestShell *pShell, HINSTANCE hInstDll)
: CDeviceTestModule(pShell, hInstDll), m_pDevice(NULL), m_pDevices(NULL)
{
    m_dwAppID        = 1007;
    m_hInstance      = hInstDll;
    m_pstrModuleName = szModuleName;
    m_nIconID        = APPICON;
    
    m_rgTestCaseInfo = rgTestCaseInfo;
    m_cTestCases     = ARRAY_ELEMENTS(rgTestCaseInfo);
    m_dwWHQLChapter  = WHQL_CHAPTER_AUDIO;

    CComPtr<IBasicLog> spBasicLog;
    pShell->GetBasicLog( &spBasicLog );
    m_pDevices = new CEnumDevice( spBasicLog );
} // CDevTopologyTestModule
#else //#ifndef BUILD_TAEF
CKsDeviceTestModule::CKsDeviceTestModule(DV_TCINFO * ptcInfo)
: CDeviceTestModule(), m_pDevice(NULL), m_pDevices(NULL)
{
    m_rgTestCaseInfo = ptcInfo;
    m_cTestCases     = 1;

    CComPtr<IBasicLog> spBasicLog;
    spBasicLog = g_pBasicLog;
    m_pDevices = new CEnumDevice( spBasicLog );
    
    // Do we have a TAEF parameter to select the device?
    String targetDevice;
    if (SUCCEEDED(RuntimeParameters::TryGetValue(L"InstanceId", targetDevice)))
    {
        g_IShell->SetTestDevice(targetDevice);
    }
} // CDevTopologyTestModule#endif
#endif //#ifndef BUILD_TAEF

CKsDeviceTestModule::~CKsDeviceTestModule()
{
    if( NULL != m_pDevices )
    {
        delete m_pDevices;
        m_pDevices = NULL;
    }
}

DWORD CKsDeviceTestModule::OnInitialUpdate(VOID)
{
    BOOL            fRes = TRUE;
    UINT            iDevice = 0;
    OSVERSIONINFO   osVersion = {0};

    if( NULL == m_pDevices )
    {
        XLOG( XFAIL, eError, "ERROR: Unable to create CEnumDevice Object" );
        return FNS_FAIL;
    }

    // Get OS Version to know how to enumerate devices
    osVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if(!GetVersionEx(&osVersion))
    {
        XLOG(XFAIL, eError, "ERROR: Unable to get OS Version Info");
        return FNS_FAIL;
    }
    if(osVersion.dwMajorVersion >= 6) 
    {
        // For Vista and beyond, enumerate all devices including \\root\media
        // devices so that MSVAD gets enumerated
        fRes = m_pDevices->Initialize((LPGUID)&KSCATEGORY_AUDIO, ENUMDEVICE_INCLUDE_ROOT_MEDIA);
    }
    else
    {
        // For pre-Vista, enumerate only non \\root devices
        fRes = m_pDevices->Initialize((LPGUID)&KSCATEGORY_AUDIO, 0);
    }

    // Make sure that the devices were allocated and created properly
    if(!fRes)
    {
        XLOG(XFAIL, eError, "ERROR: Unable to create CEnumDevice object.");
        return FNS_FAIL;
    }

    // Make sure that there are devices enumerated and add them to shell
    iDevice = m_pDevices->GetCountDevices();
    if(0 == iDevice)
    {
        SLOG(eInfo1, "No Ks Topology Audio devices found.");
    }
    else
    {   
        SLOG(eInfo1, "Enumerating %d Ks Topology Audio devices.", iDevice);
        // Get all the audio devices
        for (; iDevice; iDevice--)
        {
            CKsDevice  *pDevice = new CKsDevice(m_pDevices->GetDevice(iDevice - 1));
            if (NULL != pDevice)
            {
                pDevice->m_fSelected = TRUE;
                g_IShell->AddDevice(pDevice);
            }
        }
    }
    

    return FNS_PASS;
} // OnInitialUpdate


//--------------------------------------------------------------------------
//  CDevTopologyTestModule::SetTestDevice handler

VOID CKsDeviceTestModule::SetTestDevice(CDeviceDescriptor* pDevice)
{
    m_pDevice = (CKsDevice*)pDevice;
}

DWORD CKsDeviceTestModule::RunTest
(
    DWORD   dwTestID
)
{
    DWORD       dw;
    
    PDV_TCINFO rgTestCaseInfo = (PDV_TCINFO)m_rgTestCaseInfo;

#ifndef BUILD_TAEF
    m_pShell->IncrementIndent();
#endif //#ifndef BUILD_TAEF

    dw = ((rgTestCaseInfo[dwTestID].pfnTest)());

#ifndef BUILD_TAEF
    m_pShell->DecrementIndent();
#endif //#ifndef BUILD_TAEF

    return(dw);
}
