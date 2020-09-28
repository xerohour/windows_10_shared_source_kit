// Copyright(C) Microsoft. All rights reserved.

//
// AudioClient2APITests.h
//  
//
// Description:
//
//  IAudioClient2 API Tests
//

#pragma once

typedef UINT AudioClient2TCPath;

enum AudioClient2TCFlags
{
    INITIALIZE_IN_SHARED =              ( 0x0001 << 0 ),
    INITIALIZE_IN_EXCLUSIVE =           ( 0x0001 << 1 ),

    ISOFFLOADCAPABLE_INVALID_PARAM =    ( 0x0001 << 2 ),
    ISOFFLOADCAPABLE_VALID_PARAM =      ( 0x0001 << 3 ),

    SETCLIENTPROPERTIES_INVALID_PARAM = ( 0x0001 << 4 ),
    SETCLIENTPROPERTIES_INVALID_STATE = ( 0x0001 << 5 ),
    SETCLIENTPROPERTIES_VALID_PARAM =   ( 0x0001 << 6 ),

    GETBUFFERSIZELIMITS_INVALID_PARAM = ( 0x0001 << 7 ),
    GETBUFFERSIZELIMITS_INVALID_STATE = ( 0x0001 << 8 ),
    GETBUFFERSIZELIMITS_VALID_PARAM =   ( 0x0001 << 9 ),

    TIMERDRIVENOFFLOAD_INVALID_STATE =  ( 0x0001 << 10),

    TESTCASE_MASK =                     ( ~(0xFFFF << 11) )
};

DWORD RunAudioClient2Test( AudioClient2TCPath tc );

//
// Initialize Scenarios
//

DWORD InitializeInSharedMode( IAudioClient2* pAudioClient2, AUDIO_STREAM_CATEGORY streamCategory, bool& isInitialized );
DWORD InitializeInExclusiveMode( IAudioClient2* pAudioClient2, IMMDevice* pMMDevice, AUDIO_STREAM_CATEGORY streamCategory, bool& isInitialized );

//
// Invalid Scenarios
//

DWORD IsOffloadCapable_InvalidParamsTest( IAudioClient2* pAudioClient2, IMMDevice* pMMDevice );
DWORD SetClientProperties_InvalidParamsTest( IAudioClient2* pAudioClient2, IMMDevice* pMMDevice );
DWORD GetBufferSizeLimits_InvalidParamsTest( IAudioClient2* pAudioClient2 );

DWORD SetClientProperties_InvalidStateTest( IAudioClient2* pAudioClient2 );
DWORD GetBufferSizeLimits_InvalidStateTest( IAudioClient2* pAudioClient2 );
DWORD TimerDrivenOffload_InvalidStateTest( IAudioClient2* pAudioClient2 );

//
// Valid Scenarios
//

DWORD IsOffloadCapable_ValidParamsTest( IAudioClient2* pAudioClient2, IMMDevice* pMMDevice );
DWORD SetClientProperties_ValidParamsTest( IAudioClient2* pAudioClient2, bool isInitialized, IMMDevice* pMMDevice );
DWORD GetBufferSizeLimits_ValidParamsTest( AudioClient2TCPath Flags, IAudioClient2* pAudioClient2, bool isInitialized );

//
// Helper Functions
//

HRESULT GetExclusiveModeFormat( IAudioClient2* pAudioClient2, IMMDevice* pMMDevice, PWAVEFORMATEX* ppWfx );
