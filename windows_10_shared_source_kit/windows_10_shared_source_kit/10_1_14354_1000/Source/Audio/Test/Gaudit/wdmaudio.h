// Copyright(C) Microsoft.All rights reserved.

#pragma once

#include <wdmaud.h> // this is in nt\drivers\wdm\audio\inc


// Wdmaud device type
typedef enum {DEVICE_UNKNOWN = 0, DEVICE_WAVE, DEVICE_MIDI, DEVICE_MIXER, DEVICE_USB  } eDeviceType;


DWORD WINAPI Generic_Ioctl_IOCTL_WDMAUD_INIT(PGAUDITPARAMS pParams);
DWORD WINAPI Generic_Ioctl_IOCTL_WDMAUD_ADD_DEVNODE(PGAUDITPARAMS pParams);
DWORD WINAPI Generic_Ioctl_IOCTL_WDMAUD_GET_CAPABILITIES(PGAUDITPARAMS pParams);
eDeviceType IsValidAudioDevice(CKsFilter* pFilter);
BOOL WdmaudInit(PGAUDITPARAMS pParams);
BOOL WdmaudAdd(PGAUDITPARAMS pParams, LPDEVICEINFO pDevInfo);
BOOL WdmaudRemove(PGAUDITPARAMS pParams, LPDEVICEINFO pDevInfo);
//BOOL IsFilterWdmAud(CKsFilter* pFilter);


