// Copyright(C) Microsoft. All rights reserved.

//
// TestParams.h
//  
//
// Description:
//
//  IAudioClient API Parameter Space Definitions
//

#pragma once

#include <ks.h>
#include <ksmedia.h>

typedef enum { AUDIO_CLIENT_RENDER=0, AUDIO_CLIENT_CAPTURE, AUDIO_CLIENT_LOOPBACK } AudioClientType;
    
// special types used as no value placeholder
typedef struct _Nil {} Nil;

typedef struct _Invalid {} Invalid;

// a class that wraps any type and extends
// the type with nil and invalid
template<class T>
struct _FIELD 
{
    T    _data;
    bool _nil;
    bool _invalid;
    _FIELD() {}
    _FIELD( T data ) : _data(data), _nil(false),        _invalid(false) {} 
    _FIELD( Nil )    : _nil(true),  _invalid(false),    _data(T())      {} 
    _FIELD( Invalid ): _nil(false), _invalid(true),     _data(T())      {} 
    operator T() { return _data; }
};


typedef _FIELD<WORD>    TWORD;
typedef _FIELD<DWORD>   TDWORD;
typedef _FIELD<GUID>    TGUID;

//
// WAVEFORMATEX parameter space
//
namespace WAVEFORMATEX_PARAM_SPACE
{
    // waveformatex wrapper
    typedef struct _TWAVEFORMATEX
    {
        TWORD  Tag;
        TWORD  Channels;
        TDWORD Rate;
        TDWORD BitRate;
        TWORD  Align;
        TWORD  Bits;
        TWORD  Size;

        operator WAVEFORMATEX() {
            WAVEFORMATEX _wfx = { 
                getTag(), 
                getChannels(), 
                getRate(), 
                getBitRate(),
                getAlign(), 
                getBits(), 
                getSize() 
            };
            return _wfx;
        }
        
        WORD  getTag()         { 
            if ( Tag._nil ) {
                switch( Bits ) {
                    case 16:
                    case 8:
                    case 24:
                    case 20:
                        return WAVE_FORMAT_PCM;
                        break;
                    case 32:
                        return WAVE_FORMAT_IEEE_FLOAT;
                        break;
                }
            }
            _ASSERT(!Tag._invalid);
            return Tag; 
        }
        WORD  getChannels()     { return Channels; }

        DWORD getRate()         { return Rate; }

        DWORD getBitRate()   
        { 
            if (BitRate._nil) return Rate*getAlign();  
            else if (BitRate._invalid )
                return Rate*getAlign()/2;
            else return BitRate;
        }

        WORD  getAlign()     
        { 
            if(Align._nil)
            {
                return Channels*Bits/8;
            }
            else if(Align._invalid)
            {
                return Channels*Bits/8 - 1; 
            }
            else
            {
                return Align;
            }
        }

        WORD  getBits()         
        { 
            return Bits; 
        }

        WORD  getSize()      
        { 
            if (Size._nil) 
            {
                switch(getTag()) 
                {
                    case WAVE_FORMAT_PCM:
                    case WAVE_FORMAT_IEEE_FLOAT:         
                    default: 
                        return 0;
                }
            } 
            if (Size._invalid)
            {
                return 0xabab;
            }

            return Size;
        }
    } TWAVEFORMATEX;

    typedef struct _TWAVEFORMATEXTENSIBLE 
    {
        TWAVEFORMATEX Wfx;
        TWORD  ValidBitsPerSample;
        TDWORD SpeakerMask;
        TGUID  Guid;

        operator WAVEFORMATEXTENSIBLE() {
            WAVEFORMATEXTENSIBLE _wfx = { 
                { 
                    getTag(), 
                    Wfx.getChannels(), 
                    Wfx.getRate(), 
                    Wfx.getBitRate(),
                    Wfx.getAlign(), 
                    Wfx.getBits(), 
                    getSize() 
                },
                getValidBits(),
                getSpeakerMask(),
                getGuid()
            };
            return _wfx;
        }

        // overrides for extensible
        WORD  getSize()      
        { 
            if (Wfx.Size._nil )
            {
                return sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
            } 
            if ( Wfx.Size._invalid )
            {
                return 0;
            }
            return Wfx.Size;
        }

        WORD  getTag()         
        { 
            if ( Wfx.Tag._nil ) 
            {
                return WAVE_FORMAT_EXTENSIBLE;
            }

            _ASSERT(!Wfx.Tag._invalid);
            return Wfx.Tag; 
        }
    
        WORD  getValidBits() 
        { 
            return ValidBitsPerSample; 
        }

        GUID  getGuid() { 
            if ( Guid._nil ) {
                switch( Wfx.Bits ) {
                    case 16:
                    case 8:
                    case 24:
                    case 20:
                        return KSDATAFORMAT_SUBTYPE_PCM;break;
                    case 32:
                        if ( getValidBits() == 32 ) 
                        {
                            return KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
                        } 
                        else 
                        {
                            return KSDATAFORMAT_SUBTYPE_PCM;
                        }
                        break;
                    default:
                        _ASSERT(0);
                }
            }
            // invalid
            return Guid;
        }

        DWORD getSpeakerMask() 
        { 
            return SpeakerMask; 
        }
    } TWAVEFORMATEXTENSIBLE;

__declspec( selectany ) TWAVEFORMATEX g_WaveValidCases[] = 
{
    { WAVE_FORMAT_PCM,  2,  48000,  192000, 4,  16, 0},
};

__declspec( selectany ) TWAVEFORMATEX g_WaveCornerTestCases[]=
{
    {    Nil(),    1,           48000,      Nil(),      Nil(),      11,         Nil()},
    {    Nil(),    2,           48000,      Nil(),      Nil(),      11,         Nil()},
    {    Nil(),    0,           48000,      Nil(),      Nil(),      11,         Nil()},
    {    Nil(),    2,           (DWORD)-1,  Nil(),      Nil(),      11,         Nil()},
    {    Nil(),    (WORD)-1,    48000,      Nil(),      Nil(),      32,         Nil()},    
    {    Nil(),    (WORD)-1,    48000,      Nil(),      Nil(),      16,         Nil()},
    {    Nil(),    (WORD)-1,    48000,      Nil(),      Nil(),      11,         Nil()},
    {    Nil(),    1,           1,          Nil(),      Nil(),      11,         Nil()},
    {    Nil(),    0,           48000,      Nil(),      Nil(),      32,         Nil()},
    {    Nil(),    1,           0,          Nil(),      Nil(),      32,         Nil()},
    {    Nil(),    1,           48000,      0    ,      Nil(),      11,         Nil()},
    {    Nil(),    2,           48000,      Nil(),      (WORD)-1,   16,         Nil()},
    {    Nil(),    2,           48000,      Nil(),      Nil(),      (WORD)-1,   Nil()},
    {    Nil(),    2,           48000,      Nil(),      Nil(),      0,          Nil()},
    {    Nil(),    1,           48000,      Nil(),      Invalid(),  32,         Nil()},
    {    Nil(),    1,           (DWORD)-1,  Nil(),      Nil(),      32,         Nil()},
    {    Nil(),    1,           48000,      Invalid(),  Nil(),      16,         Nil()},
    {    Nil(),    2,           48000,      Nil(),      Invalid(),  11,         Nil()},
    {    Nil(),    2,           0,          Nil(),      Nil(),      16,         Nil()},
    {    Nil(),    2,           48000,      0    ,      Nil(),      16,         Nil()},
    {    Nil(),    2,           48000,      Invalid(),  Nil(),      32,         Nil()},
    {    Nil(),    2,           0,          Nil(),      Nil(),      11,         Nil()},
    {    Nil(),    1,           48000,      Nil(),      (WORD)-1,   32,         Nil()},
    {    0xabab,   1,           48000,      Nil(),      Nil(),      11,         Nil()},
    {    Nil(),    1,           48000,      Nil(),      Nil(),      0,          Nil()},
    {    Nil(),    0,           48000,      Nil(),      Nil(),      16,         Nil()},
    {    Nil(),    2,           48000,      0    ,      Nil(),      32,         Nil()},
    {    Nil(),    2,           (DWORD)-1,  Nil(),      Nil(),      16,         Nil()},
    {    Nil(),    2,           48000,      Nil(),      Invalid(),  16,         Nil()},
    {    0xabab,   2,           48000,      Nil(),      Nil(),      32,         Nil()},
    {    0xabab,   2,           48000,      Nil(),      Nil(),      16,         Nil()},
    {    Nil(),    2,           48000,      Invalid(),  Nil(),      11,         Nil()},
    {    Nil(),    1,           48000,      Nil(),      Nil(),      (WORD)-1,   Nil()},
    {    Nil(),    1,           48000,      Nil(),      (WORD)-1,   11,         Nil()}
};


__declspec( selectany ) TWAVEFORMATEXTENSIBLE g_WaveExtCornerTestCases[]=
{
    {    { Nil(),    1,    48000,    Nil(),    Nil(),    16,    Nil()     }, 18, Nil(), Nil()     }, // container too small
    {    { Nil(),    1,    48000,    Nil(),    Nil(),    16,    Nil()     }, 1,  Nil(), Nil()     }, // valid bits invalid
    {    { Nil(),    1,    48000,    Nil(),    Nil(),    16,    Nil()     }, 0,  Nil(), Nil()     }, // valid bits 0
    {    { Nil(),    1,    48000,    Nil(),    Nil(),    24,    Nil()     }, 32, Nil(), GUID_NULL }, // invalid GUID
    {    { Nil(),    1,    48000,    Nil(),    Nil(),    24,    Nil()     }, 32, Nil(), KSDATAFORMAT_SUBTYPE_IEEE_FLOAT }, // invalid GUID for the bit numbers
    {    { Nil(),    1,    48000,    Nil(),    Nil(),    24,    Invalid() }, 32, Nil(), Nil()     }, // invalid size
    {    { Nil(),    1,    48000,    Nil(),    Nil(),    24,    Nil()     }, (WORD)-1, Nil(), Nil()     } // huge number of valid bits
};

__declspec( selectany ) TWAVEFORMATEX g_WaveTestCases[]=
{
{ Nil(),            8,      64000,      Nil(),  Nil(),  16, Nil()},
{ Nil(),            1,      192000,     Nil(),  Nil(),  32, Nil()},
{ Nil(),            8,      16000,      Nil(),  Nil(),  24, Nil()},
{ Nil(),            1,      11025,      Nil(),  Nil(),  8,  Nil()},
{ Nil(),            2,      22050,      Nil(),  Nil(),  32, Nil()},
{ Nil(),            6,      64000,      Nil(),  Nil(),  24, Nil()},
{ Nil(),            8,      11025,      Nil(),  Nil(),  32, Nil()},
{ Nil(),            2,      3000,       Nil(),  Nil(),  16, Nil()},
{ Nil(),            1,      88200,      Nil(),  Nil(),  16, Nil()},
{ Nil(),            6,      32000,      Nil(),  Nil(),  16, Nil()},
{ Nil(),            1,      96000,      Nil(),  Nil(),  24, Nil()},
{ Nil(),            1,      64000,      Nil(),  Nil(),  8,  Nil()},
{ Nil(),            8,      3000,       Nil(),  Nil(),  8,  Nil()},
{ Nil(),            2,      96000,      Nil(),  Nil(),  8,  Nil()},
{ Nil(),            6,      88200,      Nil(),  Nil(),  32, Nil()},
{ Nil(),            1,      48000,      Nil(),  Nil(),  24, Nil()},
{ Nil(),            8,      96000,      Nil(),  Nil(),  32, Nil()},
{ Nil(),            6,      16000,      Nil(),  Nil(),  8,  Nil()},
{ Nil(),            6,      3000,       Nil(),  Nil(),  32, Nil()},
{ Nil(),            2,      48000,      Nil(),  Nil(),  16, Nil()},
{ Nil(),            2,      64000,      Nil(),  Nil(),  32, Nil()},
{ Nil(),            6,      8000,       Nil(),  Nil(),  8,  Nil()},
{ Nil(),            2,      11025,      Nil(),  Nil(),  24, Nil()},
{ Nil(),            2,      192000,     Nil(),  Nil(),  16, Nil()},
{ Nil(),            8,      48000,      Nil(),  Nil(),  8,  Nil()},
{ Nil(),            1,      8000,       Nil(),  Nil(),  24, Nil()},
{ Nil(),            6,      96000,      Nil(),  Nil(),  16, Nil()},
{ Nil(),            6,      11025,      Nil(),  Nil(),  16, Nil()},
{ Nil(),            8,      192000,     Nil(),  Nil(),  24, Nil()},
{ Nil(),            8,      8000,       Nil(),  Nil(),  32, Nil()},
{ Nil(),            8,      88200,      Nil(),  Nil(),  24, Nil()},
{ Nil(),            2,      8000,       Nil(),  Nil(),  16, Nil()},
{ Nil(),            2,      44100,      Nil(),  Nil(),  32, Nil()},
{ Nil(),            8,      22050,      Nil(),  Nil(),  24, Nil()},
{ Nil(),            2,      32000,      Nil(),  Nil(),  8,  Nil()},
{ Nil(),            6,      48000,      Nil(),  Nil(),  32, Nil()},
{ Nil(),            1,      32000,      Nil(),  Nil(),  24, Nil()},
{ Nil(),            6,      192000,     Nil(),  Nil(),  8,  Nil()},
{ Nil(),            8,      32000,      Nil(),  Nil(),  32, Nil()},
{ Nil(),            6,      44100,      Nil(),  Nil(),  24, Nil()},
{ Nil(),            6,      22050,      Nil(),  Nil(),  8,  Nil()},
{ Nil(),            1,      44100,      Nil(),  Nil(),  8,  Nil()},
{ Nil(),            8,      44100,      Nil(),  Nil(),  16, Nil()},
{ Nil(),            1,      3000,       Nil(),  Nil(),  24, Nil()},
{ Nil(),            1,      22050,      Nil(),  Nil(),  16, Nil()},
{ Nil(),            1,      16000,      Nil(),  Nil(),  32, Nil()},
{ Nil(),            2,      88200,      Nil(),  Nil(),  8,  Nil()},
{ Nil(),            2,      16000,      Nil(),  Nil(),  16, Nil()},
{ Nil(),            2,      48000,      Nil(),  Nil(),  16, Nil()},
{ Nil(),            2,      48000,      Nil(),  Nil(),  24, Nil()},
{ Nil(),            2,      48000,      Nil(),  Nil(),  32, Nil()},
{ WAVE_FORMAT_PCM,  2,      48000,      Nil(),  Nil(),  32, Nil()}, 
{ Nil(),            2,      48000,      Nil(),  Nil(),  16, Nil()},
{ Nil(),            2,      44100,      Nil(),  Nil(),  16, Nil()},
{ Nil(),            2,      96000,      Nil(),  Nil(),  16, Nil()},
{ Nil(),            2,      192000,     Nil(),  Nil(),  16, Nil()},
{ Nil(),            2,      16000,      Nil(),  Nil(),  16, Nil()},
{ Nil(),            2,      8000,       Nil(),  Nil(),  16, Nil()},
{ Nil(),            2,      11025,      Nil(),  Nil(),  16, Nil()},
{ Nil(),            2,      22050,      Nil(),  Nil(),  16, Nil()},
{ Nil(),            2,      48000,      Nil(),  8,      24, Nil()},
{ Nil(),            2,      48000,      Nil(),  8,      20, Nil()}       
};


__declspec( selectany ) TWAVEFORMATEX g_WaveTestCasesSpecial[]=
{
// one rate all bit depths
{ Nil(),            2,    48000,    Nil(),    Nil(),    16,    Nil()},
{ Nil(),            2,    48000,    Nil(),    Nil(),    24,    Nil()},
{ Nil(),            2,    48000,    Nil(),    Nil(),    32,    Nil()},
{ WAVE_FORMAT_PCM,  2,    48000,    Nil(),    Nil(),    32,    Nil()},

// 2 channels 16 bit all rates
{ Nil(),    2,    48000,    Nil(),    Nil(),    16,    Nil()},
{ Nil(),    2,    44100,    Nil(),    Nil(),    16,    Nil()},
{ Nil(),    2,    96000,    Nil(),    Nil(),    16,    Nil()},
{ Nil(),    2,    192000,   Nil(),    Nil(),    16,    Nil()},
{ Nil(),    2,    16000,    Nil(),    Nil(),    16,    Nil()},
{ Nil(),    2,    8000,     Nil(),    Nil(),    16,    Nil()},
{ Nil(),    2,    11025,    Nil(),    Nil(),    16,    Nil()},
{ Nil(),    2,    22050,    Nil(),    Nil(),    16,    Nil()},

// 32 bit container
{ Nil(),    2,    48000,    Nil(),    8,    24,    Nil()},
{ Nil(),    2,    48000,    Nil(),    8,    20,    Nil()}
};

__declspec( selectany ) TWAVEFORMATEXTENSIBLE g_WaveExtTestCases[]=
{
    // specifying bit depth in a different way
    { {Nil(),    2,    48000,    Nil(),    Nil(),    32,    Nil()}, 20, Nil(), Nil() },
    { {Nil(),    2,    48000,    Nil(),    Nil(),    32,    Nil()}, 24, Nil(), Nil() },
    { {Nil(),    2,    48000,    Nil(),    Nil(),    32,    Nil()}, 32, Nil(), Nil() },

    // quad format
    { {Nil(),    4,    48000,    Nil(),    Nil(),    16,    Nil()}, 16, KSAUDIO_SPEAKER_QUAD, Nil() },
    // surround
    { {Nil(),    4,    48000,    Nil(),    Nil(),    16,    Nil()}, 16, KSAUDIO_SPEAKER_SURROUND, Nil() },
    // 5.1
    { {Nil(),    6,    48000,    Nil(),    Nil(),    16,    Nil()}, 16, KSAUDIO_SPEAKER_5POINT1, Nil() },
    // 7.1
    { {Nil(),    8,    48000,    Nil(),    Nil(),    16,    Nil()}, 16, KSAUDIO_SPEAKER_7POINT1, Nil() },
    // play stereo mono
    { {Nil(),    2,    48000,    Nil(),    Nil(),    16,    Nil()}, 16, KSAUDIO_SPEAKER_MONO, Nil() },
    // play 5.1 stereo
    { {Nil(),    6,    48000,    Nil(),    Nil(),    16,    Nil()}, 16, KSAUDIO_SPEAKER_STEREO, Nil() },
};


    UINT32                  getInvalidCasesCount();
    WAVEFORMATEX            getInvalidCase(UINT32 i);

    UINT32                  getValidCasesCount();
    WAVEFORMATEX            getValidCase(UINT32 i);

    UINT32                  getValidSpecialCasesCount();
    WAVEFORMATEX            getValidSpecialCase(UINT32 i);

    UINT32                  getInvalidExtCasesCount();
    WAVEFORMATEXTENSIBLE    getInvalidExtCase( UINT32 i);

    UINT32                  getValidExtCasesCount();
    WAVEFORMATEXTENSIBLE    getValidExtCase( UINT32 i);

}


namespace ISFORMATSUPPORTED_PARAM_SPACE
{
    typedef struct _FIELD<WAVEFORMATEX*> TPWAVEFORMATEX;
    typedef struct _FIELD<DWORD>         TDWORD;

    typedef struct _ISFORMATSUPPORTEDINPUT
    {
        WAVEFORMATEX* pWfx;
        DWORD         SharedMode;

    } ISFORMATSUPPORTEDINPUT;

    typedef struct _ISFORMATSUPPORTED_PARAMS {

        TPWAVEFORMATEX WaveFormatEx;
        AUDCLNT_SHAREMODE           ShareMode;

        WAVEFORMATEX           _WaveFormatEx;
        WAVEFORMATEXTENSIBLE   _WaveFormatExt;


        _ISFORMATSUPPORTED_PARAMS() : WaveFormatEx(NULL), ShareMode( AUDCLNT_SHAREMODE_SHARED ) {}
        
        _ISFORMATSUPPORTED_PARAMS( WAVEFORMATEX_PARAM_SPACE::TWAVEFORMATEX twfx ) {
            ShareMode      = AUDCLNT_SHAREMODE_SHARED; 
            WaveFormatEx  = &_WaveFormatEx;
            _WaveFormatEx = twfx;
        }

        _ISFORMATSUPPORTED_PARAMS( WAVEFORMATEX_PARAM_SPACE::TWAVEFORMATEXTENSIBLE twfx ) {
            ShareMode      = AUDCLNT_SHAREMODE_SHARED; 
            WaveFormatEx   = (WAVEFORMATEX*)&_WaveFormatExt;
            _WaveFormatExt = twfx;
        }

        _ISFORMATSUPPORTED_PARAMS( TPWAVEFORMATEX tpwfx, AUDCLNT_SHAREMODE dwShareMode  ) {
            ShareMode = dwShareMode;
            WaveFormatEx = tpwfx;
        }

        operator ISFORMATSUPPORTEDINPUT() {
            ISFORMATSUPPORTEDINPUT _str = { getFormat(), getShareMode() };
            return _str;
        }

        WAVEFORMATEX* getFormat() {
            if ( WaveFormatEx._nil ) WaveFormatEx = &_WaveFormatEx; 
            return WaveFormatEx; 
        }

        AUDCLNT_SHAREMODE getShareMode() {
            return ShareMode; 
        }
    } ISFORMATSUPPORTED_PARAMS;


__declspec( selectany )    ISFORMATSUPPORTED_PARAMS g_IsFormatSupportedInvalidParams[] = {
      ISFORMATSUPPORTED_PARAMS() ,
      ISFORMATSUPPORTED_PARAMS(NULL, AUDCLNT_SHAREMODE_SHARED),
    };


    UINT32 getInvalidParamCasesCount(); 
    ISFORMATSUPPORTED_PARAMS getInvalidParamCase(UINT32 i);

    UINT32 getInvalidCasesCount();
    ISFORMATSUPPORTED_PARAMS getInvalidCase(UINT32 i);

    UINT32 getInvalidCasesExtCount();
    ISFORMATSUPPORTED_PARAMS getInvalidCaseExt(UINT32 i);

}


namespace INITIALIZE_PARAM_SPACE
{
    typedef struct _FIELD<WAVEFORMATEX*>      TPWAVEFORMATEX;
    typedef struct _FIELD<BOOL>               TBOOL;
    typedef struct _FIELD<LPCGUID>            TPGUID;
    typedef struct _FIELD<REFERENCE_TIME>      TREFERENCE_TIME;


    typedef struct _INITIALIZEINPUT
    {
        AUDCLNT_SHAREMODE       shareMode;
        DWORD                   streamFlags;
        WAVEFORMATEX*           pWfx;
        REFERENCE_TIME          bufferTime;
        LPCGUID                 submixGuid;
    } INITIALIZEINPUT;


    typedef struct _INITIALIZE_PARAMS {

        TPWAVEFORMATEX                  WaveFormatEx;
        AUDCLNT_SHAREMODE               Mode;
        DWORD                           StreamFlags;
        TREFERENCE_TIME                 Time;
        TPGUID                          Submix;
        AudioClientType                 Type;

        WAVEFORMATEX                    _WaveFormatEx;
        WAVEFORMATEXTENSIBLE            _WaveFormatExt;
        GUID                            _Submix;

        _INITIALIZE_PARAMS() { 
            WaveFormatEx = NULL;
            Submix = NULL;
            StreamFlags = 0;
            Mode = AUDCLNT_SHAREMODE_SHARED;
            Time = 0;
            Type = AUDIO_CLIENT_RENDER;
        }

        _INITIALIZE_PARAMS( TPWAVEFORMATEX twfx, 
                            AUDCLNT_SHAREMODE tmode, 
                            DWORD tflags,
                            TREFERENCE_TIME ttime, 
                            TPGUID tsubmix, 
                            AudioClientType clientType) {
            WaveFormatEx = twfx;
            StreamFlags = tflags;
            Time = ttime;
            Submix = tsubmix;
            Mode = tmode;
            Type = clientType;
        }

        _INITIALIZE_PARAMS( WAVEFORMATEX_PARAM_SPACE::TWAVEFORMATEX twfx, AUDCLNT_SHAREMODE mode, DWORD sflags, AudioClientType clientType ) {
            WaveFormatEx  = &_WaveFormatEx;
            _WaveFormatEx = twfx;
            StreamFlags = sflags;
            Submix = &GUID_NULL;
            Mode = mode;
            Time = 0;
            Type = clientType;
        }

        _INITIALIZE_PARAMS( WAVEFORMATEX_PARAM_SPACE::TWAVEFORMATEXTENSIBLE twfx, AUDCLNT_SHAREMODE mode, DWORD sflags, AudioClientType clientType ) {
            WaveFormatEx   = (WAVEFORMATEX*)&_WaveFormatExt;
            _WaveFormatExt = twfx;
            Submix = &GUID_NULL;
            StreamFlags = sflags;
            Mode = mode;
            Time = 0;
            Type = clientType;
        }

        operator INITIALIZEINPUT() {
            INITIALIZEINPUT _str = { Mode, StreamFlags, WaveFormatEx, Time, Submix };
            return _str;
        }


        // BUGBUG!
        WAVEFORMATEX* getFormat(){
            if ( WaveFormatEx._nil ) {
                if ( Type == AUDIO_CLIENT_RENDER ) {
                    WaveFormatEx = NULL;
                }
                else {
                    WaveFormatEx = NULL;
                }
            }

            return WaveFormatEx;
        }
    } INITIALIZE_PARAMS;

__declspec( selectany )    INITIALIZE_PARAMS g_InitializeInvalidParams[] = {
      // need format for compat and exclusive modes
      INITIALIZE_PARAMS(NULL,  AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM, 0, &GUID_NULL, AUDIO_CLIENT_RENDER), 
      INITIALIZE_PARAMS(NULL,  AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM, 0, &GUID_NULL, AUDIO_CLIENT_CAPTURE), 
      INITIALIZE_PARAMS(NULL,  AUDCLNT_SHAREMODE_EXCLUSIVE, 0, 0, &GUID_NULL, AUDIO_CLIENT_RENDER), 
      INITIALIZE_PARAMS(NULL,  AUDCLNT_SHAREMODE_EXCLUSIVE, 0, 0, &GUID_NULL, AUDIO_CLIENT_CAPTURE), 
      // format should be null for shared mode
      INITIALIZE_PARAMS(Nil(), AUDCLNT_SHAREMODE_SHARED, 0, 0, &GUID_NULL, AUDIO_CLIENT_RENDER), 
      INITIALIZE_PARAMS(Nil(), AUDCLNT_SHAREMODE_SHARED, 0, 0, &GUID_NULL, AUDIO_CLIENT_CAPTURE),
      // SRC_DEFAULT_QUALITY is a qualifier and needs AUTOCONVERTPCM flag to be used
      INITIALIZE_PARAMS(WAVEFORMATEX_PARAM_SPACE::g_WaveValidCases[0],  AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY, AUDIO_CLIENT_RENDER), 
      INITIALIZE_PARAMS(WAVEFORMATEX_PARAM_SPACE::g_WaveValidCases[0],  AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY, AUDIO_CLIENT_CAPTURE), 
      // ref time too big should fail in all modes
      INITIALIZE_PARAMS(NULL, AUDCLNT_SHAREMODE_SHARED, 0, -1, &GUID_NULL, AUDIO_CLIENT_RENDER),
      INITIALIZE_PARAMS(NULL, AUDCLNT_SHAREMODE_SHARED, 0, -1, &GUID_NULL, AUDIO_CLIENT_CAPTURE),
      INITIALIZE_PARAMS(Nil(), AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM, -1, &GUID_NULL, AUDIO_CLIENT_RENDER),
      INITIALIZE_PARAMS(Nil(), AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM, -1, &GUID_NULL, AUDIO_CLIENT_CAPTURE),
      INITIALIZE_PARAMS(Nil(), AUDCLNT_SHAREMODE_EXCLUSIVE, 0, -1, &GUID_NULL, AUDIO_CLIENT_RENDER),
      INITIALIZE_PARAMS(Nil(), AUDCLNT_SHAREMODE_EXCLUSIVE, 0, -1, &GUID_NULL, AUDIO_CLIENT_CAPTURE),
      // loopback only accepted in shared mode
      INITIALIZE_PARAMS(Nil(), AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM|AUDCLNT_STREAMFLAGS_LOOPBACK, 0, &GUID_NULL, AUDIO_CLIENT_RENDER),
      INITIALIZE_PARAMS(Nil(), AUDCLNT_SHAREMODE_EXCLUSIVE, AUDCLNT_STREAMFLAGS_LOOPBACK, 0, &GUID_NULL, AUDIO_CLIENT_RENDER),
      // loopback not valid for capture
      INITIALIZE_PARAMS(NULL, AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 0, &GUID_NULL, AUDIO_CLIENT_CAPTURE),
      INITIALIZE_PARAMS(Nil(), AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM|AUDCLNT_STREAMFLAGS_LOOPBACK, 0, &GUID_NULL, AUDIO_CLIENT_CAPTURE),
      INITIALIZE_PARAMS(Nil(), AUDCLNT_SHAREMODE_EXCLUSIVE, AUDCLNT_STREAMFLAGS_LOOPBACK, 0, &GUID_NULL, AUDIO_CLIENT_CAPTURE),      
    };

    UINT32 getInvalidParamCasesCount(); 
    INITIALIZE_PARAMS getInvalidParamCase(UINT32 i);

    UINT32 getInvalidCasesCount();
    INITIALIZE_PARAMS getInvalidCase(UINT32 i, AudioClientType type, AUDCLNT_SHAREMODE mode, DWORD dwStreamFlags); 

    UINT32 getInvalidCasesExtCount();
    INITIALIZE_PARAMS getInvalidCaseExt(UINT32 i, AudioClientType type, AUDCLNT_SHAREMODE mode, DWORD dwStreamFlags); 
}
