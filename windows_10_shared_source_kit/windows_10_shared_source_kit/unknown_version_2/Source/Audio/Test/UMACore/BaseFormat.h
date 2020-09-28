// Copyright(C) Microsoft. All rights reserved.

#pragma once
#pragma warning(disable : 4995)


int& convert_from_24bit(int& Dst, s24Bit& Src);

DWORD Tc_BaseFormat_SetDitherState();
DWORD Tc_BaseFormat_GetDitherState();

class CBaseFormatTest : public CComBase
{
public:
    CBaseFormatTest()
    {
    };
    virtual ~CBaseFormatTest()
    {
    };
    DWORD APOTestCreate(REFCLSID rclsid);
    DWORD APOTestProcess( LPVOID lpSource,
                          AUDIO_FRAME_FORMAT& SourceFormat,
                          REFCLSID  SourceClassID,
                          LPVOID lpDest,
                          AUDIO_FRAME_FORMAT& IntermediateFormat,
                          REFCLSID  IntermediateClassID,
                          GUID SampleDataType = KSDATAFORMAT_SUBTYPE_PCM,
                          UINT32    NumFrames = 480, //default is 480 frames
                          BOOL   bGenerateSine = TRUE
                        );
    HRESULT LockAPO( IAudioProcessingObjectRT* pAPORT,
                     UINT32 ui32NumInConnDesc,
                     IAudioMediaType* pInAudioMediaType,
                     UINT32 ui32NumOutConnDesc,
                     IAudioMediaType* pOutAudioMediaType
                   );
    DWORD   APOTestSetDitherState(REFCLSID rclsid);
    DWORD   APOTestGetDitherState(REFCLSID rclsid);
};
