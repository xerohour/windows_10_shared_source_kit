// Copyright(C) Microsoft.All rights reserved.

// channels.h

DWORD Generic_Property_KSNODEPROPERTY_AUDIO_CHANNEL_ULONG_Channel(
    ULONG ulChannel,
    IN PGAUDITPARAMS pParams,
    IN eTestCaseType TestCase,
    OPTIONAL IN PLONG pResultData
);

DWORD Generic_Property_KSAUDIOCHANNELNODEPROPERTY_Channel(
    ULONG ulChannel,
    IN PGAUDITPARAMS pParams,
    IN eTestCaseType TestCase,
    IN LPVOID pSetData,
    ULONG DataSize
);

DWORD GetChannelCount(
    HANDLE hFile,
    GUID guidPropSet,
    ULONG ulPropId,
    ULONG ulNodeId,
    OUT PULONG pulChannels,
    OUT PBOOL pBasicSupport,
    OUT PBOOL pBasicSupportMultichannel
);
