/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/

#ifndef __AZNODE_H__
#define __AZNODE_H__

// forwards
class CCodec;

class CAzNode
{
public:
    CCodec* m_pCodec;
    USHORT m_usNode;

public:
    CAzNode(USHORT usNode, CCodec* pCodec) : m_usNode(usNode), m_pCodec(pCodec) {};
    CAzNode() {};  // should never get called - exists only to make CConnChecker happy
    virtual BOOL RequiredParameterSupport() = 0;
    virtual BOOL RequiredVerbSupport() = 0;
};
#endif // __AZNODE_H__