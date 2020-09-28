// Copyright(C) Microsoft.All rights reserved.

// aac.h

#pragma pack(push, 1)
struct AudioSpecificConfig {
    WORD audioObjectType : 5;
    WORD samplingFrequencyIndex : 4; // samplingFrequency is disallowed
    WORD channelConfiguration : 4;
    
    // GASpecificConfig
    WORD frameLengthFlag : 1;
    WORD dependsOnCoreEncoder : 1;
    WORD extensionFlag : 1;
};
static_assert(sizeof(AudioSpecificConfig) == 2, "AudioSpecificConfig should be two bytes");

struct AacFormat {
    HEAACWAVEINFO info;
    AudioSpecificConfig config;
};
static_assert(sizeof(AacFormat) == sizeof(HEAACWAVEINFO) + sizeof(AudioSpecificConfig), "AacFormat should be HEAACWAVEINFO + AudioSpecificConfig");
#pragma pack(pop)
