// Copyright (C) Microsoft. All rights reserved.
#ifndef _TEST_FLAG_H
#define _TEST_FLAG_H

// Test case flags for Stream Mix tests.
#define TEST_FLAG_PCM_DRM_DRM_PCM               0x00000001
#define TEST_FLAG_PCM_DRM_PCM_DRM               0x00000002
#define TEST_FLAG_DRM_PCM_PCM_DRM               0x00000003
#define TEST_FLAG_DRM_PCM_DRM_PCM               0x00000004
#define TEST_FLAG_DRMT_DRM_DRM_DRMT             0x00000005
#define TEST_FLAG_DRMT_DRM_DRMT_DRM             0x00000006
#define TEST_FLAG_DRM_DRMT_DRMT_DRM             0x00000007
#define TEST_FLAG_DRM_DRMT_DRM_DRMT             0x00000008
#define TEST_FLAG_MAX_INSTANCE_FIRST            0x00000009
#define TEST_FLAG_MAX_INSTANCE_MID              0x0000000A
#define TEST_FLAG_MAX_INSTANCE_LAST             0x0000000B

// Test flags used in frame.cpp
#define TEST_FLAG_FORMAT_PCM                    0x00000001
#define TEST_FLAG_USE_FILE                      0x00000002

// Test flags for fduplex (adapter.cpp);
#define TEST_FLAG_WAVEOUT_TRUE                  0x00000000
#define TEST_FLAG_WAVEOUT_FALSE                 0x00000001
#define TEST_FLAG_WAVEOUT_DESTROY               0x00000002
#define TEST_FLAG_WAVEOUT_MIXER                 0x00000003
#define TEST_FLAG_WAVEOUT_ALLFORMATS_TRUE       0x00000004
#define TEST_FLAG_WAVEOUT_ALLFORMATS_FALSE      0x00000005
#define TEST_FLAG_WAVEOUT_CAPTURE               0x00000006
#define TEST_FLAG_WAVEOUT_MIXER_START           0x00000007
#define TEST_FLAG_WAVEOUT_CAPTURE_FIRST         0x00000008

/*
#define TEST_FLAG_AUTHENTICATE_MINDRMLEVEL      0x00000801
#define TEST_FLAG_AUTHENTICATE_IMAGE            0x00000802
#define TEST_FLAG_AUTHENTICATE_ALL              0x00000803
#define TEST_FLAG_AUTHENTICATE_BUILDCERTLIST    0x00000804
#define TEST_FLAG_AUTHENTICATE_FINDALLDRIVERS   0x00000805
*/
// Test Flags for SPDIF testing
#define TEST_FLAG_DIGITAL_TRUE                  0x00000000
#define TEST_FLAG_DIGITAL_FALSE                 0x00000001
#define TEST_FLAG_DIGITAL_DESTROY               0x00000002
#define TEST_FLAG_DIGITAL_MIXER                 0x00000003

// Test flags for dsound.cpp and waveout.cpp
#define TEST_FLAG_USE_DEFAULT                   0x00000001
#define TEST_FLAG_USE_ZERO_STREAM_ID            0x00000002

#define TEST_FLAG_PLAYBACK_DUPLICATE            0x00000001
#define TEST_FLAG_PLAYBACK_DUPLICATE_LOOP       0x00000002
#define TEST_FLAG_PLAYBACK_POSITION             0x00000003
#define TEST_FLAG_PLAYBACK_POSITION_LOOP        0x00000004
#define TEST_FLAG_PLAYBACK_SETVOLUME            0x00000005
#define TEST_FLAG_PLAYBACK_SETPAN               0x00000006
#define TEST_FLAG_PLAYBACK_SETFREQUENCY         0x00000007
#define TEST_FLAG_PLAYBACK_GETPOSITION          0x00000008
#define TEST_FLAG_PLAYBACK_AC3                  0x00000009
#define TEST_FLAG_PLAYBACK_MULTICHANPCM         0x0000000a
#define TEST_FLAG_PLAYBACK_HIGHBITPCM           0x0000000b

#define TEST_FLAG_MASK                          0x0000FFFF

#define TEST_FLAG(x) \
    (x & TEST_FLAG_MASK)

// Player Flags
#define PLAYER_FLAG_DIRECTSOUND                 0x10000000
#define PLAYER_FLAG_WAVEOUT                     0x20000000
#define PLAYER_FLAG_SYSAUDIO                    0x40000000
#define PLAYER_FLAG_MASK                        0xF0000000

#define PLAYER_FLAG(x) \
    (x & PLAYER_FLAG_MASK)
#define IS_DSOUND(x) \
    (PLAYER_FLAG(x) & PLAYER_FLAG_DIRECTSOUND)
#define IS_WAVEOUT(x) \
    (PLAYER_FLAG(x) & PLAYER_FLAG_WAVEOUT)
#define IS_SYSAUDIO(x) \
    (PLAYER_FLAG(x) & PLAYER_FLAG_SYSAUDIO)

// Location Flags
#define LOCATION_FLAG_SW                        0x01000000
#define LOCATION_FLAG_HW                        0x02000000
#define LOCATION_FLAG_MASK                      0x0F000000

#define LOCATION_FLAG(x) \
    (x & LOCATION_FLAG_MASK)
#define IS_HW(x) \
    (LOCATION_FLAG(x) & LOCATION_FLAG_HW)
#define IS_SW(x) \
    (LOCATION_FLAG(x) & LOCATION_FLAG_SW)

#endif
