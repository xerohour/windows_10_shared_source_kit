// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
// File Name:
//
//  tests.h
//
// Abstract:
//
//  include file for test case declarations
//



// -------------------------------------------------------------------------------

DWORD   Tc_DataRanges(void);
DWORD   Tc_KsCreatePin(void);
DWORD   Tc_KsCreatePinMulti(void);
DWORD   Tc_KsWriteStream(void);
DWORD   Tc_KsState(void);
DWORD   Tc_KsGetPosition(void);
DWORD   Tc_KsGetSetPosition(void);

DWORD   Tc_woOpen(void);
DWORD   Tc_woOpenMulti(void);
DWORD   Tc_woWrite(void);
DWORD   Tc_woWriteLooped(void);
DWORD   Tc_woPause(void);
DWORD   Tc_woReset(void);
DWORD   Tc_woGetPosition(void);
DWORD   Tc_woOpenAC3thenPCM(void);
DWORD   Tc_woOpenPCMthenAC3(void);

//DWORD   Tc_CreateSoundBufferPrimary(void);
//DWORD   Tc_CreateSoundBufferSecondary(void);
//DWORD   Tc_DuplicateSoundBuffer(void);
//DWORD   Tc_Play(void);
//DWORD   Tc_Stop(void);
//DWORD   Tc_GetCurrentPosition(void);
//DWORD   Tc_GetSetCurrentPosition(void);
//DWORD   Tc_GetSetFormat(void);
//DWORD   Tc_GetSetFrequency(void);
//DWORD   Tc_GetSetPan(void);
//DWORD   Tc_GetStatus(void);
//DWORD   Tc_GetSetVolume(void);
//DWORD   Tc_CreateBufferAC3thenPCM(void);
//DWORD   Tc_CreateBufferPCMthenAC3(void);

DWORD   Tc_Regression183493(void);
