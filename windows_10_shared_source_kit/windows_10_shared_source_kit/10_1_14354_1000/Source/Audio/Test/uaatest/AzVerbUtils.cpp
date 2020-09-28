/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/

#define USERMODE

#include "AzVerbUtils.h"

ULONG MakeVerb(USHORT _LinkId, USHORT _NodeId, AzCodecNodeParameter _Param, USHORT _Indirect)
{
  //ASSERT(_LinkId <= 15 );
  //ASSERT(_NodeId <= 127);
  //ASSERT(_Indirect <= 1);
  return (_LinkId << 28) + (_Indirect << 27) + (_NodeId << 20) + _Param;
}

ULONG MakeVerb(USHORT _LinkId, USHORT _NodeId, AzCodecNodeControl _Control, USHORT _Payload, USHORT _Indirect)
{
  //ASSERT(_LinkId <= 15 );
  //ASSERT(_NodeId <= 127);
  //ASSERT(_Indirect <= 1);
  ULONG x = (_LinkId << 28) + (_Indirect << 27) + (_NodeId << 20) + _Control;
  switch(_Control & 0xf0000)
  {
    case 0x70000:  
    case 0xf0000: 
    {
      // 12 bit verb id
      if(_Payload > 0xff)
      {
        // DOUT(DBGErr, ("12 bit verb ID has 8 bits payload"));
        //ASSERT(FALSE);
        return 0xffffffff; //AzInvalidCorbEntry;
      }
      break;
    }
    default:
    {
      // 4 bit verb id
      if(_Payload > 0xffff)
      {
        // DOUT(DBGErr, ("12 bit verb ID has 8 bits payload"));
        //ASSERT(FALSE);
        return 0xffffffff;
      }
      break;
    }
  }
  return x + _Payload;  
}
