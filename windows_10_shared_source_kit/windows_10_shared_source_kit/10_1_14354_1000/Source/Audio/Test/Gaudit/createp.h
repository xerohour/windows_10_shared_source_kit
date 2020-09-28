// Copyright(C) Microsoft.All rights reserved.

#pragma once

#define MSB_FORMAT          WAVE_FORMAT_96S16

typedef struct {
    ULONG   Channels;
    ULONG   BitsPerSample;
    ULONG   SampleFrequency;
} DataRangeInfo;
DataRangeInfo FormatFlagToFormatInfo(DWORD dwFormatFlag);
BOOL CheckFormatForRange(DWORD dwFormat, PKSDATARANGE_AUDIO pKsDataRangeAudio, DataRangeInfo* pDataFormatInfo ) ;

DWORD WINAPI  Generic_Create_Pin_1(PGAUDITPARAMS pParams);
DWORD WINAPI  Generic_Create_Pin_2(PGAUDITPARAMS pParams);
DWORD WINAPI  Generic_Create_Pin_3(PGAUDITPARAMS pParams);
DWORD WINAPI  Generic_Create_Pin_4(PGAUDITPARAMS pParams);
DWORD WINAPI  Generic_Create_Pin_Invalid(PGAUDITPARAMS pParams, ULONG TestCase);
DWORD WINAPI  Generic_Create_Pin_5(PGAUDITPARAMS pParams);
DWORD WINAPI  Generic_Create_Pin_6(PGAUDITPARAMS pParams);
DWORD WINAPI  Generic_Create_Pin_7(PGAUDITPARAMS pParams);

BOOL CreateAudioPinInstance(DWORD dwFormat, PKSDATARANGE_AUDIO pKsDataRangeAudio, HANDLE* phNewPin);

DWORD   Tst_KCP_AllPinsValid (PGAUDITPARAMS pParams);


DWORD WINAPI Generic_Create_Pin_Mode(PGAUDITPARAMS pParams, GUID guidProcessingMode, ULONG TestCase);
DWORD WINAPI Create_Mode_Pin_ULONG_PINID_DEFAULT(PGAUDITPARAMS pParams);
DWORD WINAPI Create_Mode_Pin_ULONG_PINID_RAW(PGAUDITPARAMS pParams);
DWORD WINAPI Create_Mode_Pin_GUIDNULL(PGAUDITPARAMS pParams);
DWORD WINAPI Create_Mode_Pin_GUID_RANDOM(PGAUDITPARAMS pParams);
