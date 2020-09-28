// Copyright(C) Microsoft.All rights reserved.

// eqbands.cpp

#include "common.h"
#include "eqbands.h"

// helper function for GetChannelCount,
// to determine how big of a buffer to allocate for equalizer nodes
DWORD GetEqBandsCount(HANDLE hFile, ULONG ulChannel, ULONG ulNodeId, OUT PULONG pulEqBands) {
    DWORD dwResult = FNS_PASS;
    ULONG ulEqBands = -1;

    SLOG(eInfo1, "Attempting to query number of equalization bands...");

    KSNODEPROPERTY_AUDIO_CHANNEL NodeAudioChannelProperty = {0};
    NodeAudioChannelProperty.NodeProperty.Property.Set = KSPROPSETID_Audio;
    NodeAudioChannelProperty.NodeProperty.Property.Id = KSPROPERTY_AUDIO_NUM_EQ_BANDS;
    NodeAudioChannelProperty.NodeProperty.Property.Flags =
        KSPROPERTY_TYPE_TOPOLOGY | KSPROPERTY_TYPE_GET;
        // KSPROPERTY_TYPE_TOPOLOGY goes with KSNODEPROPERTY
    NodeAudioChannelProperty.NodeProperty.NodeId = ulNodeId;
    NodeAudioChannelProperty.NodeProperty.Reserved = 0;
    NodeAudioChannelProperty.Channel = ulChannel;
    NodeAudioChannelProperty.Reserved = 0;

    ULONG OutBufferSize = sizeof(ULONG);
    ULONG cbExpectedWritten = OutBufferSize;

    CGenericIoControl gnt_eq(  hFile, &NodeAudioChannelProperty, 
                             sizeof(KSNODEPROPERTY_AUDIO_CHANNEL), sizeof(KSNODEPROPERTY_AUDIO_CHANNEL),
                             OutBufferSize, OutBufferSize);
    dwResult = gnt_eq.Test(FALSE, cbExpectedWritten); // expect it to succeed
    if (FAULT_NO_FAULT != gnt_eq.GetFault()) { // unexpected
        SLOG(eError, "Could not query number of EQ bands for channel %u (0-based)", ulChannel);
        return FNS_FAIL;
    }

    ulEqBands = *(ULONG*)gnt_eq.GetData();

    if (NULL != pulEqBands) {
        *pulEqBands = ulEqBands;
    }

    return dwResult;
}
