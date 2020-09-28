// Copyright(C) Microsoft.All rights reserved.

#pragma once

BOOL CreatePinInstance(HANDLE hFilter, PKSDATARANGE prange, ULONG PinId, DWORD dwFilterFlags, HANDLE* phPin);
BOOL CreateClock(HANDLE hFile, HANDLE* phClock);
DWORD GetPinFormat(PGAUDITPARAMS pParams, PKSDATAFORMAT_WAVEFORMATEX pWfx, PKSDATAFORMAT_DSOUND pDS, BOOL& fIsWfx);
BOOL IsPropertySupported(HANDLE hFile, KSPROPERTY* pProperty, ULONG cbSize, ULONG Flags = 0);
BOOL IsPropertySetSupported(HANDLE hFile, KSPROPERTY* pProperty, ULONG cbSize);
void CopyGuid(GUID& dest, const GUID& src);
DWORD GetPinPrefferedFormat(HANDLE hFile, PKSDATAFORMAT_WAVEFORMATEX pFmt, ULONG cbOutSize);
// right now no one use this function
DWORD GetPinDataintersection(HANDLE hFile, PKSDATARANGE_AUDIO pAudioFmt);
void FillPKsPinConnect1(PKSPIN_CONNECT pconnect, ULONG PinId, HANDLE SinkHandle, KSPIN_INTERFACE *Interface, KSPIN_MEDIUM *Medium , KSPRIORITY *Priority);
__inline void FillStandardConnect(PKSPIN_CONNECT pConnect, ULONG PinId, HANDLE hPin = NULL);
