/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/

#ifndef _AzVerbUtils_h_Included_
#define _AzVerbUtils_h_Included_
#ifndef USERMODE
  #ifdef __cplusplus
  extern "C"
  {
  #endif
    #include <wdm.h>
  #ifdef __cplusplus
  }
  #endif
#else
  #include <windows.h>
#endif
#include <azcodec.h>
ULONG MakeVerb(USHORT _LinkId, USHORT _NodeId, AzCodecNodeParameter _Param, USHORT _Indirect = 0);
ULONG MakeVerb(USHORT _LinkId, USHORT _NodeId, AzCodecNodeControl _Control, USHORT _Payload, USHORT _Indirect = 0);
#endif