/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/

#ifndef __UTIL_H__
#define __UTIL_H__

//CR: [df] This test should include hdaparse.h here rather then redefine everything!
#define MAX_NUMBER_OF_WIDGETS     64
#define MAX_NUMBER_OF_CONNECTIONs 32


#define INVALID_LONGFORM  0xe0000001
#define INVALID_INCOMPLETE_CONNECTION_RESPONSE 0xf0000002
#define INVALID_BOGUS_CONNECTION_RESPONSE      0xf0000003
#define INVALID_INVALID_NODE_FOUND             0xf0000004


#define WARNING_INVALID_NODE_FOUND             0x00000005

typedef struct _WidgetParamType
{
  ULONG id;
  ULONG type;
 //parameters
 ULONG capabilities;            // Parameter ID 0x09
 ULONG supportedDataDepthNRate; // parameter ID 0x0a
 ULONG supportedStreamFormats;  // Parameter ID 0x0b
 ULONG inAMPCaps;               // Parameter ID 0x0d
 ULONG outAMPCaps;              // Parameter ID 0x12
 ULONG pinCapabilities;         // Parameter ID 0x0c
 ULONG supportedPowerStates;    // Parameter ID 0x0f
 ULONG configurationDefault;    // Control: Verb ID: F1C
 ULONG connectListLength;       // Parameter ID 0x0e
 
 //controls 
 ULONG connectList[MAX_NUMBER_OF_CONNECTIONs]; //control id is F02
}WidgetParamType;


typedef struct _AudioFunctionGroupParams
{
  ULONG vendevID;            
  ULONG revID;
  //parameters
  ULONG capabilities;            // parameter ID 0x08
  ULONG supportedDataDepthNRate; // Parameter ID 0x0a
  ULONG supportedStreamFormats;  // Parameter ID 0x0b
  ULONG supportedPowerStates;    // Parameter ID 0x0f
  ULONG nWidgetCount;
}AudioFunctionGroupParams;


typedef struct _FunctionGroupInfoType
{
  ULONG InvalidFG ;//0->valid, nonzero--> invalid reason
  AudioFunctionGroupParams params; 
  WidgetParamType widgets[MAX_NUMBER_OF_WIDGETS];
  
}FunctionGroupInfoType;

#define OUTPUT_FILENAME_STRING L"\\AzaliaFG.dat"

// Prototypes
DWORD CollectAFGInfo();

bool StringEndsWith(LPCSTR haystack, LPCSTR needle);

#endif // __UTIL_H__